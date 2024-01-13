
#define Ethernet_In_Use 1
const int Serial_In_Use = 0;
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
#define GREEN_STATUS_LED_PORT 9
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
#define STEP_TIME 8
unsigned long NEXT_STEP_TIMER = 0;

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


#define COIL_LEFT_HYD_A1 22
#define COIL_LEFT_HYD_A2 24
#define COIL_LEFT_HYD_A3 26
#define COIL_LEFT_HYD_A4 28

// #define COIL_LEFT_HYD_A1 22
// #define COIL_LEFT_HYD_A2 26
// #define COIL_LEFT_HYD_A3 24
// #define COIL_LEFT_HYD_A4 28

#define COIL_RIGHT_HYD_A1 23
#define COIL_RIGHT_HYD_A2 25
#define COIL_RIGHT_HYD_A3 27
#define COIL_RIGHT_HYD_A4 29

// #define COIL_RIGHT_HYD_A1 23
// #define COIL_RIGHT_HYD_A2 27
// #define COIL_RIGHT_HYD_A3 25
// #define COIL_RIGHT_HYD_A4 29

#define COIL_LEFT_FUEL_A1 30
#define COIL_LEFT_FUEL_A2 34
#define COIL_LEFT_FUEL_A3 32
#define COIL_LEFT_FUEL_A4 36

#define COIL_RIGHT_FUEL_A1 31
#define COIL_RIGHT_FUEL_A2 35
#define COIL_RIGHT_FUEL_A3 33
#define COIL_RIGHT_FUEL_A4 37

#define COIL_OXY_REG_A1 38
#define COIL_OXY_REG_A2 42
#define COIL_OXY_REG_A3 40
#define COIL_OXY_REG_A4 44

#define COIL_LOX_A1 39
#define COIL_LOX_A2 43
#define COIL_LOX_A3 41
#define COIL_LOX_A4 45




// #define STEPPER_MAX_SPEED 900
#define STEPPER_MAX_SPEED 1200
#define STEPPER_ACCELERATION 300




const int Stepper10_dirPin = 27;
const int Stepper10_stepPin = 25;
const int Stepper10_enablePin = 23;

//AccelStepper STEPPER_10(AccelStepper::DRIVER, Stepper10_stepPin, Stepper10_dirPin);
AccelStepper STEPPER_10(AccelStepper::FULL2WIRE, Stepper10_stepPin, Stepper10_dirPin);




const int stepsPerRevolution = 200;


// ###################################### End Stepper Related #############################




void setup() {
  // put your setup code here, to run once:
  pinMode(RED_STATUS_LED_PORT, OUTPUT);
  pinMode(GREEN_STATUS_LED_PORT, OUTPUT);

  digitalWrite(RED_STATUS_LED_PORT, true);
  digitalWrite(GREEN_STATUS_LED_PORT, true);
  delay(FLASH_TIME);
  digitalWrite(RED_STATUS_LED_PORT, false);
  digitalWrite(GREEN_STATUS_LED_PORT, false);




  if (Ethernet_In_Use == 1) {

    // Using manual reset instead of tying to Arduino Reset
    pinMode(ES1_RESET_PIN, OUTPUT);
    digitalWrite(ES1_RESET_PIN, LOW);
    delay(2);
    digitalWrite(ES1_RESET_PIN, HIGH);

    Ethernet.begin(mac, ip);


    udp.begin(localport);
  }

  delay(3000);

  digitalWrite(RED_STATUS_LED_PORT, true);
  digitalWrite(GREEN_STATUS_LED_PORT, true);
  delay(FLASH_TIME);
  digitalWrite(RED_STATUS_LED_PORT, false);
  digitalWrite(GREEN_STATUS_LED_PORT, false);


  // For reasons I'm yet to work out - earlier senddebugs are not sent before this point
  // Testing shows a delay of 3 seconds is needed
  // SendDebug("LED Initialisation Complete");

  SendDebug("Starting Motor Initialisation");



  if (true) {
    SendDebug("Start Stepper STEPPER_10");
    pinMode(Stepper10_enablePin, OUTPUT);
    digitalWrite(Stepper10_enablePin, LOW);
    STEPPER_10.setMaxSpeed(STEPPER_MAX_SPEED);
    STEPPER_10.setAcceleration(STEPPER_ACCELERATION);
    // STEPPER_10.move(-630);

    // while (STEPPER_10.distanceToGo() != 0) {
    //   STEPPER_10.run();
    // }


    STEPPER_10.move(-2700);
    while (STEPPER_10.distanceToGo() != 0) {
      STEPPER_10.run();
    }

    STEPPER_10.move(2700);
    while (STEPPER_10.distanceToGo() != 0) {
      STEPPER_10.run();
    }
    SendDebug("End Stepper STEPPER_10");
  }

  SendDebug("End Motor Initialisation");





  // Declare pins as Outputs
  // pinMode(Stepper10_dirPin, OUTPUT);
  // pinMode(Stepper10_stepPin, OUTPUT);
  // pinMode(Stepper10_enablePin, OUTPUT);
}

void loop() {

  if (millis() >= NEXT_STATUS_TOGGLE_TIMER) {
    GREEN_LED_STATE = !GREEN_LED_STATE;
    RED_LED_STATE = !RED_LED_STATE;
    digitalWrite(GREEN_STATUS_LED_PORT, GREEN_LED_STATE);
    digitalWrite(RED_STATUS_LED_PORT, RED_LED_STATE);


    NEXT_STATUS_TOGGLE_TIMER = millis() + FLASH_TIME;
  }

  // if (millis() >= NEXT_STEP_TIMER) {

  //   digitalWrite(Stepper10_stepPin, HIGH);
  //   delayMicroseconds(200);
  //   digitalWrite(Stepper10_stepPin, LOW);

  //   NEXT_STEP_TIMER = millis() + STEP_TIME;
  // }
}
