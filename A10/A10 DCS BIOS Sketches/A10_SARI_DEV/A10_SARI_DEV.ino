// SARI

#define Ethernet_In_Use 1
#define Reflector_In_Use 1
#define DCSBIOS_In_Use 1
#define MSFS_In_Use 0  // Used to interface into MSFS - set to 0 if not in use

#define DCSBIOS_IRQ_SERIAL
#include "DcsBios.h"


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

const unsigned long delayBeforeSendingPacket = 2000;
unsigned long ethernetStartTime = 0;

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


#include <AccelStepper.h>
#include <Stepper.h>

#define STEPS 3200  // steps per revolution
#define FLASH_TIME 300
bool RED_LED_STATE = false;
long NEXT_STATUS_TOGGLE_TIMER = 0;

#define ROLL_ZERO_SENSE_IN 8
bool LAST_SENSOR_STATE = false;

#define COIL_STEPPER_1_A 4
#define COIL_STEPPER_1_B 5
#define COIL_STEPPER_1_C 6
#define COIL_STEPPER_1_D 7

#define DRIVER_DIRECTION 7
#define DRIVER_STEP 6
#define DRIVER_ENABLE 4


// The Max Speed and Acceleration values are determined with trial and error
// Even with 300 still seeing occasional jamms on start up
// Hornet has roll rate os 225 degrees per second
// F16 has a roll rate of 240 degrees per second
// So if ball can rotate once per second we are good
// 1.8 degrees per step = so 200 steps for 360 degrees
//#define STEPPER_MAX_SPEED 900
#define STEPPER_MAX_SPEED 90000
#define STEPPER_ZERO_SEEK_SPEED 600
// #define STEPPER_ACCELERATION 600
#define STEPPER_ACCELERATION 60000
#define CLOCKWISE_ZERO_OFFSET 50
#define ANTICLOCKWISE_ZERO_OFFSET 12

// AccelStepper STEPPER_1(AccelStepper::FULL4WIRE, COIL_STEPPER_1_A, COIL_STEPPER_1_B, COIL_STEPPER_1_C, COIL_STEPPER_1_D);
AccelStepper STEPPER_1(AccelStepper::DRIVER, DRIVER_STEP, DRIVER_DIRECTION);

bool SENSOR_STATE;
bool LAST_SENSOR_READING = true;
bool INITIALISING_ROLL = true;
#define CLOCKWISE 1
#define ANTICLOCKWISE 2
#define STOPPED 0
int direction = STOPPED;


void checkSensor() {
  bool THIS_SENSOR_READING;

  if (STEPPER_1.speed() == 0)
    direction = STOPPED;
  else if (STEPPER_1.speed() >= 0)
    direction = ANTICLOCKWISE;
  else if (STEPPER_1.speed() <= -0)
    direction = CLOCKWISE;


  if (digitalRead(ROLL_ZERO_SENSE_IN))
    THIS_SENSOR_READING = true;
  else
    THIS_SENSOR_READING = false;

  if (LAST_SENSOR_READING == true && THIS_SENSOR_READING == false) {
    SendDebug("SENSOR TRANSISTION!!");
    SendDebug("Speed: " + String(STEPPER_1.speed()));
    if (direction == CLOCKWISE) {
      SendDebug("Direction : Clockwise");
    } else {
      SendDebug("Direction : Anti-Clockwise");
    }
    SendDebug("Reported Position :" + String(STEPPER_1.currentPosition()));

    if ((STEPPER_1.speed() >= 0) && (STEPPER_1.speed() <= 50) && (INITIALISING_ROLL == true)) {
      SendDebug("Setting Anti-clockwise zero point");
      STEPPER_1.setCurrentPosition(ANTICLOCKWISE_ZERO_OFFSET);
    } else if (STEPPER_1.speed() <= -0 && (STEPPER_1.speed() >= -50)) {
      SendDebug("Setting Clockwise zero point");
      STEPPER_1.setCurrentPosition(CLOCKWISE_ZERO_OFFSET);
    }
  }

  LAST_SENSOR_READING = THIS_SENSOR_READING;
  if (STEPPER_1.run() == false) {
    // SendDebug("Position: " + String(STEPPER_1.currentPosition()));
    if (direction == CLOCKWISE)
      SendDebug("Clockwise");
    else if (direction == ANTICLOCKWISE)
      SendDebug("Anticlockwise");
  }
}



void onSaiBankChange(unsigned int newValue) {
  long longer0 = newValue;
  long longer1 = longer0 - 32757;
  long TargetPos = map(longer1, 0, 65355, 0, 3200);

  long currentPosition = STEPPER_1.currentPosition() % STEPS;
  long rollCount = long(STEPPER_1.currentPosition() / STEPS);
  

  long Delta = TargetPos - currentPosition;
  // Convert to positive number if neg
  if (Delta <= 0) {
    Delta = Delta * -1;
  }
  static long MaxDelta;
  if (Delta >= MaxDelta)
    MaxDelta = Delta;

  if (Delta >= 1600) {
    // We have done a loop - add number of steps * rollCount +1 to TargetPos
    TargetPos = TargetPos + (STEPS * (rollCount + 1));
  }

  SendDebug("SAI Bank: " + String(newValue) + " Converted Val:" + String(longer1)
            + " Target is:"+ String(TargetPos)
            + " Curr Pos:" + currentPosition
            + " Delta:" + String(Delta)
            + " Max Delta:" + String(MaxDelta)
            + " Roll Count:" + String(rollCount));



  STEPPER_1.moveTo(TargetPos);
}
DcsBios::IntegerBuffer saiBankBuffer(0x74e6, 0xffff, 0, onSaiBankChange);


void setup() {

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, true);
  pinMode(ROLL_ZERO_SENSE_IN, INPUT);


  if (Ethernet_In_Use == 1) {

    // Reset Ethernet Module
    pinMode(ES1_RESET_PIN, OUTPUT);
    digitalWrite(ES1_RESET_PIN, LOW);
    delay(2);
    digitalWrite(ES1_RESET_PIN, HIGH);

    Ethernet.begin(mac, ip);
    udp.begin(localport);

    // As it takes a couple of seconds before the Ethernet Stack is operational
    // Flash leds until time period has completed
    ethernetStartTime = millis() + delayBeforeSendingPacket;
    while (millis() <= ethernetStartTime) {
      delay(FLASH_TIME);
      digitalWrite(LED_BUILTIN, false);
      delay(FLASH_TIME);
    }

    SendDebug("Ethernet Started");
    SendDebug("A10_SARI_DEV");
  }


  delay(FLASH_TIME);
  digitalWrite(LED_BUILTIN, false);
  delay(FLASH_TIME);


  int STEP_COUNTER = 0;


  STEPPER_1.setMaxSpeed(STEPPER_MAX_SPEED);
  STEPPER_1.setAcceleration(STEPPER_ACCELERATION);

  SendDebug("Max Speed: " + String(STEPPER_MAX_SPEED));
  SendDebug("Acceleration: " + String(STEPPER_ACCELERATION));
  SendDebug("Initialise Stepper - Two turns and then return to starting point");

  pinMode(DRIVER_ENABLE, OUTPUT);
  digitalWrite(DRIVER_ENABLE, false);
  STEPPER_1.moveTo(STEPS * 2.5);
  while (STEPPER_1.distanceToGo() != 0) {
    STEPPER_1.run();
    // SendDebug("Step Speed: " + String(STEPPER_1.speed()));
  }
  delay(300);

  STEPPER_1.moveTo(0);
  while (STEPPER_1.distanceToGo() != 0) {
    STEPPER_1.run();
  }
  delay(300);

  SendDebug("Initialise Stepper - Move Random Distance");
  randomSeed(analogRead(0));
  int randNumber = random(10, STEPS);
  STEPPER_1.moveTo(randNumber);
  while (STEPPER_1.distanceToGo() != 0) {
    STEPPER_1.run();
    checkSensor();
  }
  delay(1000);
  // Check to see if stepper already in rest position - if so move 20
  SENSOR_STATE = digitalRead(ROLL_ZERO_SENSE_IN);
  if (SENSOR_STATE == false) {
    SendDebug("Initialise Stepper - Stepping away from Zero");
    STEPPER_1.move(int(STEPS / 10));
    while (STEPPER_1.distanceToGo() != 0) {
      STEPPER_1.run();
      SendDebug("Initialise Stepper - Steps to go :" + String(STEPPER_1.distanceToGo()));
    }
  }

  // If Sensor State is still zero we have a problem
  SENSOR_STATE = digitalRead(ROLL_ZERO_SENSE_IN);
  if (SENSOR_STATE == false)
    SendDebug("Initialise Stepper - WARNING ZERO SENSOR APPEARS JAMMED");

  // Find Zero Point - moving in positive direction
  SendDebug("Initialise Stepper - Find Zero point");
  // Slow stepper down so we don't overshoot the zero point
  STEPPER_1.setMaxSpeed(STEPPER_ZERO_SEEK_SPEED);
  // At this point zero position is arbitary - will have a real zero after this loop
  STEPPER_1.setCurrentPosition(0);
  // Moving in a negative direction translates to clockwise
  STEPPER_1.moveTo(-STEPS * 1.5);
  while (STEPPER_1.distanceToGo() != 0) {
    SENSOR_STATE = digitalRead(ROLL_ZERO_SENSE_IN);
    if (SENSOR_STATE == false) {
      STEPPER_1.setCurrentPosition(CLOCKWISE_ZERO_OFFSET);
      break;
    }
    STEPPER_1.run();
    checkSensor();
  }

  // Warn if zero state not detected - we should have a reading of false
  if (SENSOR_STATE == true)
    SendDebug("Initialise Stepper - WARNING ZERO NOT LOCATED");

  SendDebug("Initialise Stepper - Move to Zero accounting for offset");
  STEPPER_1.runToNewPosition(0);
  SendDebug("Initialise Stepper - Found Zero Point");
  // Return to normal max speed

  SENSOR_STATE = digitalRead(ROLL_ZERO_SENSE_IN);
  if (SENSOR_STATE == false)
    SendDebug("Initialise Stepper - Sensor Still Blocked");

  SendDebug("Now Finding otherside of interruptor");
  STEPPER_1.moveTo(-200);
  SendDebug("Distance to go is :" + String(STEPPER_1.distanceToGo()));

  SENSOR_STATE = digitalRead(ROLL_ZERO_SENSE_IN);
  while (SENSOR_STATE == false) {
    SENSOR_STATE = digitalRead(ROLL_ZERO_SENSE_IN);
    if (SENSOR_STATE == false) {
      SendDebug("Position that Sensor became True is :" + String(STEPPER_1.currentPosition()));
      break;
    }
    STEPPER_1.run();
    checkSensor();
  }


  SENSOR_STATE = digitalRead(ROLL_ZERO_SENSE_IN);
  SendDebug("Checking Sensor is clear, sensor ready is :" + String(SENSOR_STATE));

  STEPPER_1.setMaxSpeed(STEPPER_MAX_SPEED);

  STEPPER_1.moveTo(0);


  INITIALISING_ROLL = false;
  SendDebug("All Done");

  DcsBios::setup();
  // Serial.end();
  //digitalWrite(DRIVER_ENABLE, true);
}






void loop() {

  if (millis() >= NEXT_STATUS_TOGGLE_TIMER) {
    RED_LED_STATE = !RED_LED_STATE;
    digitalWrite(LED_BUILTIN, RED_LED_STATE);
    NEXT_STATUS_TOGGLE_TIMER = millis() + FLASH_TIME;
  }

  DcsBios::loop();
  STEPPER_1.run();
  // checkSensor();
}
