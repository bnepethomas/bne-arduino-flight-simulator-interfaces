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

#define STEPS 200  // steps per revolution
#define FLASH_TIME 600
bool RED_LED_STATE = false;
long NEXT_STATUS_TOGGLE_TIMER = 0;

#define ROLL_ZERO_SENSE_IN 8
bool LAST_SENSOR_STATE = false;

#define COIL_STEPPER_1_A 4
#define COIL_STEPPER_1_B 5
#define COIL_STEPPER_1_C 6
#define COIL_STEPPER_1_D 7


// The Max Speed and Acceleration values are determined with trial and error
// Even with 300 still seeing occasional jamms on start up
// Hornet has roll rate os 225 degrees per second
// F16 has a roll rate of 240 degrees per second
// So if ball can rotate once per second we are good
// 1.8 degrees per step = so 200 steps for 360 degrees
#define STEPPER_MAX_SPEED 30

#define CLOCKWISE_ZERO_OFFSET 2
#define ANTICLOCKWISE_ZERO_OFFSET 0

Stepper STEPPER_1(STEPS, COIL_STEPPER_1_A, COIL_STEPPER_1_B, COIL_STEPPER_1_C, COIL_STEPPER_1_D);

bool SENSOR_STATE;
bool LAST_SENSOR_READING = true;
bool THIS_SENSOR_READING = true;
#define CLOCKWISE 1
#define ANTICLOCKWISE 2
#define STOPPED 0
int direction = STOPPED;

long STEPPER_1_CURRENT_POSITION = 0;
long STEPPER_1_TARGET_POSITION = 0;

// void checkSensor() {
//   bool THIS_SENSOR_READING;

//   if (STEPPER_1.speed() == 0)
//     direction = STOPPED;
//   else if (STEPPER_1.speed() >= 0)
//     direction = ANTICLOCKWISE;
//   else if (STEPPER_1.speed() <= -0)
//     direction = CLOCKWISE;


//   if (digitalRead(ROLL_ZERO_SENSE_IN))
//     THIS_SENSOR_READING = true;
//   else
//     THIS_SENSOR_READING = false;

//   if (LAST_SENSOR_READING == true && THIS_SENSOR_READING == false) {
//     SendDebug("SENSOR TRANSISTION!!");
//     SendDebug("Speed: " + String(STEPPER_1.speed()));
//     if (direction == CLOCKWISE) {
//       SendDebug("Direction : Clockwise");
//     } else {
//       SendDebug("Direction : Anti-Clockwise");
//     }
//     SendDebug("Reported Position :" + String(STEPPER_1.currentPosition()));

//     if ((STEPPER_1.speed() >= 0) && (STEPPER_1.speed() <= 50)) {
//       SendDebug("Setting Anti-clockwise zero point");
//       STEPPER_1.setCurrentPosition(ANTICLOCKWISE_ZERO_OFFSET);
//     } else if (STEPPER_1.speed() <= -0 && (STEPPER_1.speed() >= -50)) {
//       SendDebug("Setting Clockwise zero point");
//       STEPPER_1.setCurrentPosition(CLOCKWISE_ZERO_OFFSET);
//     }
//   }

//   LAST_SENSOR_READING = THIS_SENSOR_READING;
//   if (STEPPER_1.run() == false) {
//     // SendDebug("Position: " + String(STEPPER_1.currentPosition()));
//     if (direction == CLOCKWISE)
//       SendDebug("Clockwise");
//     else if (direction == ANTICLOCKWISE)
//       SendDebug("Anticlockwise");
//   }
// }



void onSaiBankChange(unsigned int newValue) {
  long longer0 = newValue;
  long longer1 = longer0 - 32757;

  STEPPER_1_TARGET_POSITION = map(longer1, 0, 65355, 0, 200);
  SendDebug("SAI Bank: " + String(newValue) + " " + String(longer1) + " "
            + String(STEPPER_1_TARGET_POSITION) + " " + String(STEPPER_1_CURRENT_POSITION));
}
DcsBios::IntegerBuffer saiBankBuffer(0x74e6, 0xffff, 0, onSaiBankChange);


void upDateStepper() {
  if (STEPPER_1_CURRENT_POSITION != STEPPER_1_TARGET_POSITION) {

    if (STEPPER_1_TARGET_POSITION > STEPPER_1_CURRENT_POSITION) {
      SendDebug("Increment Stepper Position");
      STEPPER_1.step(1);
      STEPPER_1_CURRENT_POSITION++;
    } else {
      SendDebug("Decrement Stepper Position");
      STEPPER_1.step(-1);
      STEPPER_1_CURRENT_POSITION--;
    }

    if (digitalRead(ROLL_ZERO_SENSE_IN))
      THIS_SENSOR_READING = true;
    else
      THIS_SENSOR_READING = false;

    if (LAST_SENSOR_READING == true && THIS_SENSOR_READING == false) {
      SendDebug("SENSOR TRANSISTION!!");
      STEPPER_1_CURRENT_POSITION = 0;
    }
    LAST_SENSOR_READING = THIS_SENSOR_READING;
  }
}

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
  }


  delay(FLASH_TIME);
  digitalWrite(LED_BUILTIN, false);
  delay(FLASH_TIME);


  int STEP_COUNTER = 0;


  STEPPER_1.setSpeed(STEPPER_MAX_SPEED);


  SendDebug("Max Speed: " + String(STEPPER_MAX_SPEED));

  SendDebug("Initialise Stepper - Two turns and then return to starting point");


  STEPPER_1.step(430);
  delay(300);

  STEPPER_1.step(-430);
  delay(300);

  SendDebug("Initialise Stepper - Move Random Distance");
  randomSeed(analogRead(0));
  int randNumber = random(10, 200);
  STEPPER_1.step(randNumber);


  // Check to see if stepper already in rest position - if so move 20
  SENSOR_STATE = digitalRead(ROLL_ZERO_SENSE_IN);
  if (SENSOR_STATE == false) {
    SendDebug("Initialise Stepper - Stepping away from Zero");
    STEPPER_1.step(20);
  }

  // If Sensor State is still zero we have a problem
  if (SENSOR_STATE == false)
    SendDebug("Initialise Stepper - WARNING ZERO SENSOR APPEARS JAMMED");

  // Find Zero Point - moving in positive direction
  SendDebug("Initialise Stepper - Find Zero point");


  // Moving in a negative direction translates to clockwise

  int LOOP_COUNTER = 0;
  while (LOOP_COUNTER <= 400) {
    SENSOR_STATE = digitalRead(ROLL_ZERO_SENSE_IN);
    if (SENSOR_STATE == false) {
      STEPPER_1_CURRENT_POSITION = CLOCKWISE_ZERO_OFFSET;
      break;
    }
    LOOP_COUNTER++;
    STEPPER_1.step(1);
  }

  // Warn if zero state not detected - we should have a reading of false
  if (SENSOR_STATE == true)
    SendDebug("Initialise Stepper - WARNING ZERO NOT LOCATED");


  // SendDebug("Initialise Stepper - Found Zero Point");



  SendDebug("All Done");

  DcsBios::setup();
}






void loop() {

  if (millis() >= NEXT_STATUS_TOGGLE_TIMER) {
    RED_LED_STATE = !RED_LED_STATE;
    digitalWrite(LED_BUILTIN, RED_LED_STATE);
    NEXT_STATUS_TOGGLE_TIMER = millis() + FLASH_TIME;
  }

  DcsBios::loop();
  upDateStepper();
  // checkSensor();
}
