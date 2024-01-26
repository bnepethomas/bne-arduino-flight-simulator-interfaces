// Development test framework to determine max number of steppers that
// can safely be driven off an Arduino without the use of drivers
// Absoultely Max current is 200mA

// Measuring using USB power meter
// 0.19A with 4 steppers disabled
// 0.27A with 4 steppers enabled
// So average current draw 20mA per stepper
// 4 Steppers direct with 4 driven by buffer

#define Ethernet_In_Use 1
#define Reflector_In_Use 1


// ###################################### Begin Ethernet Related #############################
#include <SPI.h>
#include <Ethernet.h>
#include <EthernetUdp.h>

// These local Mac and IP Address will be reassigned early in startup based on
// the device ID as set by address pins
#define EthernetStartupDelay 500
#define ES1_RESET_PIN 53

byte mac[] = { 0xA8, 0x61, 0x0A, 0x9E, 0x83, 0x06 };
IPAddress ip(172, 16, 1, 106);
String strMyIP = "172.16.1.106";

// Reflector
IPAddress reflectorIP(172, 16, 1, 10);
String strReflectorIP = "X.X.X.X";


EthernetUDP udp;

const unsigned int localport = 7788;
const unsigned int max7219port = 7788;
const unsigned int remoteport = 49000;
const unsigned int reflectorport = 27000;
const unsigned int MSFSport = 13136;

char packetBuffer[1000];     //buffer to store the incoming data
char outpacketBuffer[1000];  //buffer to store the outgoing data


String DebugString = "";

// Packet Length
int max7219packetsize;
int max7219Len;

int MSFSpacketsize;
int MSFSLen;

EthernetUDP max7219udp;  // Max7219
EthernetUDP MSFSudp;     // Listens to MSFS light commands

char max7219packetBuffer[1000];  //buffer to store packet data for both Max7219 and MSFS data


bool Debug_Display = false;
char *ParameterNamePtr;
char *ParameterValuePtr;




void SendDebug(String MessageToSend) {
  if ((Reflector_In_Use == 1) && (Ethernet_In_Use == 1)) {
    udp.beginPacket(reflectorIP, reflectorport);
    udp.println(MessageToSend);
    udp.endPacket();
  }
}
// ###################################### End Ethernet Related #############################





// THE LED PORTS WILL CHANGE FROM THE V1.1 PCB TO THE FOLLOWING
// #define RED_STATUS_LED_PORT 12
// #define GREEN_STATUS_LED_PORT 13
#define RED_STATUS_LED_PORT 8
#define FLASH_TIME 300

unsigned long NEXT_STATUS_TOGGLE_TIMER = 0;
bool GREEN_LED_STATE = false;
bool RED_LED_STATE = false;
unsigned long timeSinceRedLedChanged = 0;

#define DCSBIOS_IRQ_SERIAL


// ###################################### Begin Servo Related #############################
#include <AccelStepper.h>
#include <Stepper.h>
#define STEPS 720  // steps per revolution (limited to 315°)

// Holding current per coil is 14mA, which gives 28mA per stepper
// Mega absolute max is 40mA per pin, with a total max of 200mA
// Gives a total


// Works but slow max speed of 30
// #define COIL_RIGHT_HYD_A1 23
// #define COIL_RIGHT_HYD_A2 25
// #define COIL_RIGHT_HYD_A3 27
// #define COIL_RIGHT_HYD_A4 29
// //  stepperSTANDBY_ALT.setMaxSpeed(30);

// Works but slow max speed of 30
// #define COIL_RIGHT_HYD_A1 25
// #define COIL_RIGHT_HYD_A2 23
// #define COIL_RIGHT_HYD_A3 27
// #define COIL_RIGHT_HYD_A4 29

// Works but slow max speed of 30

// If the stepper is incorrectly configured it will chatter above
// rates of more than 30 steps per minute, if when correctly
// configured speeds can exceed 2300

#define COIL_STEPPER_1_A 12
#define COIL_STEPPER_1_B 13
#define COIL_STEPPER_1_C 9
#define COIL_STEPPER_1_D 11

#define COIL_STEPPER_2_A 4
#define COIL_STEPPER_2_B 5
#define COIL_STEPPER_2_C 7
#define COIL_STEPPER_2_D 6

#define COIL_STEPPER_3_A 2
#define COIL_STEPPER_3_B 3
#define COIL_STEPPER_3_C 0
#define COIL_STEPPER_3_D 1

#define COIL_STEPPER_4_A 14
#define COIL_STEPPER_4_B 15
#define COIL_STEPPER_4_C 16
#define COIL_STEPPER_4_D 17

#define COIL_STEPPER_5_A 18
#define COIL_STEPPER_5_B 19
#define COIL_STEPPER_5_C 20
#define COIL_STEPPER_5_D 21

#define COIL_STEPPER_6_A 26
#define COIL_STEPPER_6_B 28
#define COIL_STEPPER_6_C 22
#define COIL_STEPPER_6_D 24


#define COIL_STEPPER_12_A 47
#define COIL_STEPPER_12_B 49
#define COIL_STEPPER_12_C 46
#define COIL_STEPPER_12_D 48


// // As used in 1.1 Board
// #define COIL_LEFT_HYD_A1 22
// #define COIL_LEFT_HYD_A2 24
// #define COIL_LEFT_HYD_A3 26
// #define COIL_LEFT_HYD_A4 28


// Stepper_5 on v1.2 Board
#define COIL_LEFT_HYD_A1 26
#define COIL_LEFT_HYD_A2 28
#define COIL_LEFT_HYD_A3 22
#define COIL_LEFT_HYD_A4 24

#define COIL_RIGHT_HYD_A1 23
#define COIL_RIGHT_HYD_A2 25
#define COIL_RIGHT_HYD_A3 27
#define COIL_RIGHT_HYD_A4 29

// #define COIL_RIGHT_HYD_A1 23
// #define COIL_RIGHT_HYD_A2 27
// #define COIL_RIGHT_HYD_A3 25
// #define COIL_RIGHT_HYD_A4 29

#define COIL_LEFT_FUEL_A1 30
#define COIL_LEFT_FUEL_A2 32
#define COIL_LEFT_FUEL_A3 34
#define COIL_LEFT_FUEL_A4 36

#define COIL_RIGHT_FUEL_A1 31
#define COIL_RIGHT_FUEL_A2 35
#define COIL_RIGHT_FUEL_A3 33
#define COIL_RIGHT_FUEL_A4 37

#define COIL_OXY_REG_A1 38
#define COIL_OXY_REG_A2 40
#define COIL_OXY_REG_A3 42
#define COIL_OXY_REG_A4 44

#define COIL_LOX_A1 39
#define COIL_LOX_A2 41
#define COIL_LOX_A3 43
#define COIL_LOX_A4 45

// Pins are slighty Reversed when compared to steppers on Expansion connection
#define COIL_CABIN_PRESS_A1 9
#define COIL_CABIN_PRESS_A2 7
#define COIL_CABIN_PRESS_A3 8
#define COIL_CABIN_PRESS_A4 6


// #define STEPPER_MAX_SPEED 900
#define STEPPER_MAX_SPEED 8300
#define STEPPER_ACCELERATION 2000

// AccelStepper STEPPER_LEFT_HYD(AccelStepper::FULL4WIRE, COIL_LEFT_HYD_A1, COIL_LEFT_HYD_A2, COIL_LEFT_HYD_A3, COIL_LEFT_HYD_A4);
// AccelStepper STEPPER_RIGHT_HYD(AccelStepper::FULL4WIRE, COIL_RIGHT_HYD_A1, COIL_RIGHT_HYD_A2, COIL_RIGHT_HYD_A3, COIL_RIGHT_HYD_A4);
// AccelStepper STEPPER_LEFT_FUEL(AccelStepper::FULL4WIRE, COIL_LEFT_FUEL_A1, COIL_LEFT_FUEL_A2, COIL_LEFT_FUEL_A3, COIL_LEFT_FUEL_A4);
// AccelStepper STEPPER_RIGHT_FUEL(AccelStepper::FULL4WIRE, COIL_RIGHT_FUEL_A1, COIL_RIGHT_FUEL_A2, COIL_RIGHT_FUEL_A3, COIL_RIGHT_FUEL_A4);
// AccelStepper STEPPER_OXY_REG(AccelStepper::FULL4WIRE, COIL_OXY_REG_A1, COIL_OXY_REG_A2, COIL_OXY_REG_A3, COIL_OXY_REG_A4);
// AccelStepper STEPPER_LOX(AccelStepper::FULL4WIRE, COIL_LOX_A1, COIL_LOX_A2, COIL_LOX_A3, COIL_LOX_A4);
// AccelStepper STEPPER_CABIN_PRESS(AccelStepper::FULL4WIRE, COIL_CABIN_PRESS_A1, COIL_CABIN_PRESS_A2, COIL_CABIN_PRESS_A3, COIL_CABIN_PRESS_A4);


AccelStepper STEPPER_1(AccelStepper::FULL4WIRE, COIL_STEPPER_1_A, COIL_STEPPER_1_B, COIL_STEPPER_1_C, COIL_STEPPER_1_D);
AccelStepper STEPPER_2(AccelStepper::FULL4WIRE, COIL_STEPPER_2_A, COIL_STEPPER_2_B, COIL_STEPPER_2_C, COIL_STEPPER_2_D);
AccelStepper STEPPER_3(AccelStepper::FULL4WIRE, COIL_STEPPER_3_A, COIL_STEPPER_3_B, COIL_STEPPER_3_C, COIL_STEPPER_3_D);
AccelStepper STEPPER_4(AccelStepper::FULL4WIRE, COIL_STEPPER_4_A, COIL_STEPPER_4_B, COIL_STEPPER_4_C, COIL_STEPPER_4_D);
AccelStepper STEPPER_5(AccelStepper::FULL4WIRE, COIL_STEPPER_5_A, COIL_STEPPER_5_B, COIL_STEPPER_5_C, COIL_STEPPER_5_D);
AccelStepper STEPPER_6(AccelStepper::FULL4WIRE, COIL_STEPPER_6_A, COIL_STEPPER_6_B, COIL_STEPPER_6_C, COIL_STEPPER_6_D);
AccelStepper STEPPER_12(AccelStepper::FULL4WIRE, COIL_STEPPER_12_A, COIL_STEPPER_12_B, COIL_STEPPER_12_C, COIL_STEPPER_12_D);

// ###################################### End Stepper Related #############################




void setup() {
  // put your setup code here, to run once:
  pinMode(RED_STATUS_LED_PORT, OUTPUT);

  digitalWrite(RED_STATUS_LED_PORT, true);
  delay(FLASH_TIME);
  digitalWrite(RED_STATUS_LED_PORT, false);

  delay(FLASH_TIME);



  if (Ethernet_In_Use == 1) {

    // Using manual reset instead of tying to Arduino Reset
    pinMode(ES1_RESET_PIN, OUTPUT);
    digitalWrite(ES1_RESET_PIN, LOW);
    delay(2);
    digitalWrite(ES1_RESET_PIN, HIGH);

    Ethernet.begin(mac, ip);


    udp.begin(localport);
  }


  // For reasons I'm yet to work out - earlier senddebugs are not sent before this point
  // Testing shows a delay of 3 seconds is needed
  SendDebug(" ");
  SendDebug("Stepper 12 Initialising");


  STEPPER_1.setMaxSpeed(STEPPER_MAX_SPEED);
  STEPPER_1.setAcceleration(STEPPER_ACCELERATION);
  STEPPER_2.setMaxSpeed(STEPPER_MAX_SPEED);
  STEPPER_2.setAcceleration(STEPPER_ACCELERATION);
  STEPPER_3.setMaxSpeed(STEPPER_MAX_SPEED);
  STEPPER_3.setAcceleration(STEPPER_ACCELERATION);
  STEPPER_4.setMaxSpeed(STEPPER_MAX_SPEED);
  STEPPER_4.setAcceleration(STEPPER_ACCELERATION);
  STEPPER_5.setMaxSpeed(STEPPER_MAX_SPEED);
  STEPPER_5.setAcceleration(STEPPER_ACCELERATION);
  STEPPER_6.setMaxSpeed(STEPPER_MAX_SPEED);
  STEPPER_6.setAcceleration(STEPPER_ACCELERATION);

  STEPPER_12.setMaxSpeed(STEPPER_MAX_SPEED);
  STEPPER_12.setAcceleration(STEPPER_ACCELERATION);

  SendDebug("Starting Motor Initialisation");


  // if (false) {
  //   STEPPER_RIGHT_HYD.setMaxSpeed(STEPPER_MAX_SPEED);
  //   STEPPER_RIGHT_HYD.setAcceleration(STEPPER_ACCELERATION);
  //   STEPPER_RIGHT_HYD.move(4000);
  //   SendDebug("Start Stepper Right Hyd");
  //   while (STEPPER_RIGHT_HYD.distanceToGo() != 0) {
  //     STEPPER_RIGHT_HYD.run();
  //   }
  //   STEPPER_RIGHT_HYD.move(-4000);
  //   while (STEPPER_RIGHT_HYD.distanceToGo() != 0) {
  //     STEPPER_RIGHT_HYD.run();
  //   }
  //   SendDebug("End Stepper Right Hyd");
  // }

  // if (false) {
  //   SendDebug("Start Stepper Left Hyd");
  //   STEPPER_LEFT_HYD.setMaxSpeed(STEPPER_MAX_SPEED);
  //   STEPPER_LEFT_HYD.setAcceleration(STEPPER_ACCELERATION);
  //   // STEPPER_LEFT_HYD.move(-630);

  //   // while (STEPPER_LEFT_HYD.distanceToGo() != 0) {
  //   //   STEPPER_LEFT_HYD.run();
  //   // }

  //   STEPPER_LEFT_HYD.move(630);

  //   while (STEPPER_LEFT_HYD.distanceToGo() != 0) {
  //     STEPPER_LEFT_HYD.run();
  //   }
  //   STEPPER_LEFT_HYD.move(-630);
  //   while (STEPPER_LEFT_HYD.distanceToGo() != 0) {
  //     STEPPER_LEFT_HYD.run();
  //   }
  //   STEPPER_LEFT_HYD.disableOutputs();
  //   SendDebug("End Stepper Left Hyd");
  // }

  // if (false) {
  //   SendDebug("Start Stepper Left Fuel");
  //   STEPPER_LEFT_FUEL.setMaxSpeed(STEPPER_MAX_SPEED);
  //   STEPPER_LEFT_FUEL.setAcceleration(STEPPER_ACCELERATION);
  //   STEPPER_LEFT_FUEL.move(630);
  //   while (STEPPER_LEFT_FUEL.distanceToGo() != 0) {
  //     STEPPER_LEFT_FUEL.run();
  //   }
  //   STEPPER_LEFT_FUEL.move(-630);
  //   while (STEPPER_LEFT_FUEL.distanceToGo() != 0) {
  //     STEPPER_LEFT_FUEL.run();
  //   }
  //   SendDebug("End Stepper Left Fuel");
  // }

  // if (false) {
  //   SendDebug("Start Stepper Right Fuel");
  //   STEPPER_RIGHT_FUEL.setMaxSpeed(STEPPER_MAX_SPEED);
  //   STEPPER_RIGHT_FUEL.setAcceleration(STEPPER_ACCELERATION);
  //   STEPPER_RIGHT_FUEL.move(4000);
  //   while (STEPPER_RIGHT_FUEL.distanceToGo() != 0) {
  //     STEPPER_RIGHT_FUEL.run();
  //   }
  //   STEPPER_RIGHT_FUEL.move(-4000);
  //   while (STEPPER_RIGHT_FUEL.distanceToGo() != 0) {
  //     STEPPER_RIGHT_FUEL.run();
  //   }
  //   SendDebug("End Stepper Right Fuel");
  // }

  // if (false) {
  //   SendDebug("Start Stepper OXY REG");
  //   STEPPER_OXY_REG.setMaxSpeed(STEPPER_MAX_SPEED);
  //   STEPPER_OXY_REG.setAcceleration(STEPPER_ACCELERATION);
  //   STEPPER_OXY_REG.move(630);
  //   while (STEPPER_OXY_REG.distanceToGo() != 0) {
  //     STEPPER_OXY_REG.run();
  //   }
  //   STEPPER_OXY_REG.move(-630);
  //   while (STEPPER_OXY_REG.distanceToGo() != 0) {
  //     STEPPER_OXY_REG.run();
  //   }
  //   SendDebug("End Stepper OXY REG");
  // }

  // if (false) {
  //   SendDebug("Start Stepper LOX");
  //   STEPPER_LOX.setMaxSpeed(STEPPER_MAX_SPEED);
  //   STEPPER_LOX.setAcceleration(STEPPER_ACCELERATION);
  //   STEPPER_LOX.move(630);
  //   while (STEPPER_LOX.distanceToGo() != 0) {
  //     STEPPER_LOX.run();
  //   }
  //   STEPPER_LOX.move(-630);
  //   while (STEPPER_LOX.distanceToGo() != 0) {
  //     STEPPER_LOX.run();
  //   }
  //   SendDebug("End Stepper LOX");
  // }


  // if (false) {
  //   SendDebug("Start Stepper Cabin Press");
  //   STEPPER_CABIN_PRESS.setMaxSpeed(STEPPER_MAX_SPEED);
  //   STEPPER_CABIN_PRESS.setAcceleration(STEPPER_ACCELERATION);
  //   STEPPER_CABIN_PRESS.move(4000);
  //   while (STEPPER_CABIN_PRESS.distanceToGo() != 0) {
  //     STEPPER_CABIN_PRESS.run();
  //   }
  //   STEPPER_CABIN_PRESS.move(-4000);
  //   while (STEPPER_CABIN_PRESS.distanceToGo() != 0) {
  //     STEPPER_CABIN_PRESS.run();
  //   }
  //   SendDebug("End Stepper Cabin Press");
  // }


  SendDebug("End Motor Initialisation");






  if (true) {
    SendDebug("Start All Stepper");
    STEPPER_1.move(630);
    STEPPER_2.move(630);
    STEPPER_3.move(630);
    STEPPER_4.move(630);
    STEPPER_5.move(630);
    STEPPER_6.move(630);

    STEPPER_12.move(630);
    // STEPPER_LOX.move(630);
    // STEPPER_CABIN_PRESS.move(630);
    // STEPPER_OXY_REG.move(630);
    // STEPPER_LEFT_FUEL.move(630);
    // STEPPER_RIGHT_FUEL.move(630);
    // STEPPER_LEFT_HYD.move(630);
    // STEPPER_RIGHT_HYD.move(630);
    while (STEPPER_1.distanceToGo() != 0) {
      //SendDebug("Stepper_1 distance to go :" + String(STEPPER_1.distanceToGo()));
      STEPPER_1.run();
      STEPPER_2.run();
      STEPPER_3.run();
      STEPPER_4.run();
      STEPPER_5.run();
      STEPPER_6.run();

      STEPPER_12.run();

      // STEPPER_LOX.run();
      // STEPPER_CABIN_PRESS.run();
      // STEPPER_OXY_REG.run();
      // STEPPER_LEFT_FUEL.run();
      // STEPPER_RIGHT_FUEL.run();
      // STEPPER_LEFT_HYD.run();
      // STEPPER_RIGHT_HYD.run();
    }
    SendDebug("Steppers at Max");
    STEPPER_1.move(-630);
    STEPPER_2.move(-630);
    STEPPER_3.move(-630);
    STEPPER_4.move(-630);
    STEPPER_5.move(-630);
    STEPPER_6.move(-630);

    STEPPER_12.move(-630);

    // STEPPER_LOX.move(-630);
    // STEPPER_CABIN_PRESS.move(-630);
    // STEPPER_OXY_REG.move(-630);
    // STEPPER_LEFT_FUEL.move(-630);
    // STEPPER_RIGHT_FUEL.move(-630);
    // STEPPER_LEFT_HYD.move(-630);
    // STEPPER_RIGHT_HYD.move(-630);
    //SendDebug("Returning");
    while (STEPPER_1.distanceToGo() != 0) {
      STEPPER_1.run();
      STEPPER_2.run();
      STEPPER_3.run();
      STEPPER_4.run();
      STEPPER_5.run();
      STEPPER_6.run();

      STEPPER_12.run();

      // STEPPER_LOX.run();
      // STEPPER_CABIN_PRESS.run();
      // STEPPER_OXY_REG.run();
      // STEPPER_LEFT_FUEL.run();
      // STEPPER_RIGHT_FUEL.run();
      // STEPPER_LEFT_HYD.run();
      // STEPPER_RIGHT_HYD.run();
    }
    SendDebug("End All Stepper");
  }
}



void loop() {

  if (millis() >= NEXT_STATUS_TOGGLE_TIMER) {
    RED_LED_STATE = !RED_LED_STATE;
    digitalWrite(RED_STATUS_LED_PORT, RED_LED_STATE);

    NEXT_STATUS_TOGGLE_TIMER = millis() + FLASH_TIME;
  }
}
