// Development test framework to determine max number of steppers that
// can safely be driven off an Arduino without the use of drivers
// Absoultely Max current is 200mA

// Measuring using USB power meter
// 0.19A with 4 steppers disabled
// 0.27A with 4 steppers enabled
// So average current draw 20mA per stepper
// 4 Steppers direct with 4 driven by buffer

// Found fuel flow wn't nicely run through L293D, so there are 7 steppers
// After soak test for approx 5 minutes steppers lost sanity
// USB Meter indicating 4.77 Volts 0.62A
// will test by baselining time to loose sanity, and then reduce number of active steppers
// Perhaps overheating or just on margins with timing


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



#define RED_STATUS_LED_PORT 8
#define FLASH_TIME 3000

unsigned long NEXT_STATUS_TOGGLE_TIMER = 0;
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

#define COIL_STEPPER_4_A 16
#define COIL_STEPPER_4_B 17
#define COIL_STEPPER_4_C 14
#define COIL_STEPPER_4_D 15

#define COIL_STEPPER_5_A 18
#define COIL_STEPPER_5_B 19
#define COIL_STEPPER_5_C 20
#define COIL_STEPPER_5_D 21

#define COIL_STEPPER_6_A 26
#define COIL_STEPPER_6_B 28
#define COIL_STEPPER_6_C 22
#define COIL_STEPPER_6_D 24

#define COIL_STEPPER_7_A 30
#define COIL_STEPPER_7_B 32
#define COIL_STEPPER_7_C 34
#define COIL_STEPPER_7_D 36

#define COIL_STEPPER_8_A 38
#define COIL_STEPPER_8_B 40
#define COIL_STEPPER_8_C 42
#define COIL_STEPPER_8_D 44

#define COIL_STEPPER_9_A 23
#define COIL_STEPPER_9_B 25
#define COIL_STEPPER_9_C 27
#define COIL_STEPPER_9_D 29

#define COIL_STEPPER_10_A 31
#define COIL_STEPPER_10_B 33
#define COIL_STEPPER_10_C 35
#define COIL_STEPPER_10_D 37

#define COIL_STEPPER_11_A 39
#define COIL_STEPPER_11_B 41
#define COIL_STEPPER_11_C 43
#define COIL_STEPPER_11_D 45


#define COIL_STEPPER_12_A 47
#define COIL_STEPPER_12_B 49
#define COIL_STEPPER_12_C 46
#define COIL_STEPPER_12_D 48


// #define STEPPER_MAX_SPEED 900
#define STEPPER_MAX_SPEED 8300
#define STEPPER_ACCELERATION 2000


AccelStepper STEPPER_1(AccelStepper::FULL4WIRE, COIL_STEPPER_1_A, COIL_STEPPER_1_B, COIL_STEPPER_1_C, COIL_STEPPER_1_D);
AccelStepper STEPPER_2(AccelStepper::FULL4WIRE, COIL_STEPPER_2_A, COIL_STEPPER_2_B, COIL_STEPPER_2_C, COIL_STEPPER_2_D);
AccelStepper STEPPER_3(AccelStepper::FULL4WIRE, COIL_STEPPER_3_A, COIL_STEPPER_3_B, COIL_STEPPER_3_C, COIL_STEPPER_3_D);
AccelStepper STEPPER_4(AccelStepper::FULL4WIRE, COIL_STEPPER_4_A, COIL_STEPPER_4_B, COIL_STEPPER_4_C, COIL_STEPPER_4_D);
AccelStepper STEPPER_5(AccelStepper::FULL4WIRE, COIL_STEPPER_5_A, COIL_STEPPER_5_B, COIL_STEPPER_5_C, COIL_STEPPER_5_D);
AccelStepper STEPPER_6(AccelStepper::FULL4WIRE, COIL_STEPPER_6_A, COIL_STEPPER_6_B, COIL_STEPPER_6_C, COIL_STEPPER_6_D);
AccelStepper STEPPER_7(AccelStepper::FULL4WIRE, COIL_STEPPER_7_A, COIL_STEPPER_7_B, COIL_STEPPER_7_C, COIL_STEPPER_7_D);
AccelStepper STEPPER_8(AccelStepper::FULL4WIRE, COIL_STEPPER_8_A, COIL_STEPPER_8_B, COIL_STEPPER_8_C, COIL_STEPPER_8_D);
AccelStepper STEPPER_9(AccelStepper::FULL4WIRE, COIL_STEPPER_9_A, COIL_STEPPER_9_B, COIL_STEPPER_9_C, COIL_STEPPER_9_D);
AccelStepper STEPPER_10(AccelStepper::FULL4WIRE, COIL_STEPPER_10_A, COIL_STEPPER_10_B, COIL_STEPPER_10_C, COIL_STEPPER_10_D);
AccelStepper STEPPER_11(AccelStepper::FULL4WIRE, COIL_STEPPER_11_A, COIL_STEPPER_11_B, COIL_STEPPER_11_C, COIL_STEPPER_11_D);
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
  STEPPER_7.setMaxSpeed(STEPPER_MAX_SPEED);
  STEPPER_7.setAcceleration(STEPPER_ACCELERATION);
  STEPPER_8.setMaxSpeed(STEPPER_MAX_SPEED);
  STEPPER_8.setAcceleration(STEPPER_ACCELERATION);
  STEPPER_9.setMaxSpeed(STEPPER_MAX_SPEED);
  STEPPER_9.setAcceleration(STEPPER_ACCELERATION);
  STEPPER_10.setMaxSpeed(STEPPER_MAX_SPEED);
  STEPPER_10.setAcceleration(STEPPER_ACCELERATION);
  STEPPER_11.setMaxSpeed(STEPPER_MAX_SPEED);
  STEPPER_11.setAcceleration(STEPPER_ACCELERATION);
  STEPPER_12.setMaxSpeed(STEPPER_MAX_SPEED);
  STEPPER_12.setAcceleration(STEPPER_ACCELERATION);


  cycleSteppers();
}

void cycleSteppers() {



  SendDebug("Start All Stepper");
  STEPPER_1.move(630);
  STEPPER_2.move(630);
  STEPPER_3.move(630);
  STEPPER_4.move(630);
  STEPPER_5.move(630);
  STEPPER_6.move(630);
  STEPPER_7.move(630);
  STEPPER_8.move(630);
  STEPPER_9.move(630);
  STEPPER_10.move(630);
  STEPPER_11.move(630);
  STEPPER_12.move(630);

  while (STEPPER_1.distanceToGo() != 0) {
    //SendDebug("Stepper_1 distance to go :" + String(STEPPER_1.distanceToGo()));
    STEPPER_1.run();
    STEPPER_2.run();
    STEPPER_3.run();
    STEPPER_4.run();
    STEPPER_5.run();
    STEPPER_6.run();
    STEPPER_7.run();
    STEPPER_8.run();
    STEPPER_9.run();
    STEPPER_10.run();
    STEPPER_11.run();
    STEPPER_12.run();
  }
  SendDebug("Steppers at Max");
  STEPPER_1.move(-630);
  STEPPER_2.move(-630);
  STEPPER_3.move(-630);
  STEPPER_4.move(-630);
  STEPPER_5.move(-630);
  STEPPER_6.move(-630);
  STEPPER_7.move(-630);
  STEPPER_8.move(-630);
  STEPPER_9.move(-630);
  STEPPER_10.move(-630);
  STEPPER_11.move(-630);
  STEPPER_12.move(-630);
  //SendDebug("Returning");
  while (STEPPER_1.distanceToGo() != 0) {
    STEPPER_1.run();
    STEPPER_2.run();
    STEPPER_3.run();
    STEPPER_4.run();
    STEPPER_5.run();
    STEPPER_6.run();
    STEPPER_7.run();
    STEPPER_8.run();
    STEPPER_9.run();
    STEPPER_10.run();
    STEPPER_11.run();
    STEPPER_12.run();
  }
  SendDebug("End All Stepper");
}

void loop() {

  if (millis() >= NEXT_STATUS_TOGGLE_TIMER) {
    RED_LED_STATE = !RED_LED_STATE;
    digitalWrite(RED_STATUS_LED_PORT, RED_LED_STATE);
    cycleSteppers();
    NEXT_STATUS_TOGGLE_TIMER = millis() + FLASH_TIME;
  }
}
