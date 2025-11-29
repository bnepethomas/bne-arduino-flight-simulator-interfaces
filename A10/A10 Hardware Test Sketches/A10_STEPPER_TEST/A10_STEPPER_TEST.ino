/* 
Test framework for Multiple Steppers with Drivers
*/


#define Ethernet_In_Use 1
#define Reflector_In_Use 1
#define DCSBIOS_In_Use 1
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
char* ParameterNamePtr;
char* ParameterValuePtr;


unsigned long currentMillis = 0;
unsigned long previousMillis = 0;
int DCS_On = 0;
int previous_DCS_State = 0;
int DCS_State = 0;
long dcsMillis;
bool DCSACTIVE = false;


// Ranges without Driver Board
#define directMaxStepperSpeed 900
#define directMaxAccel 900
#define directFullSwing 615
// Ranges with Driver Board
#define maxStepperSpeed 90000
#define maxAccel 90000
#define fullSwing 5020
#define waitTime 100

#include <AccelStepper.h>


const int stepper5_A1 = 19;
const int stepper5_A2 = 20;
const int stepper5_B1 = 21;
const int stepper5_B2 = 22;
AccelStepper Stepper5(AccelStepper::FULL4WIRE, stepper5_A1, stepper5_A2, stepper5_B1, stepper5_B2);


const int stepper6_A1 = 24;
const int stepper6_A2 = 26;
const int stepper6_B1 = 28;
const int stepper6_B2 = 30;
AccelStepper Stepper6(AccelStepper::FULL4WIRE, stepper6_A1, stepper6_A2, stepper6_B1, stepper6_B2);


// AOA
const int enable7Pin = 32;
const int step7Pin = 34;
const int direction7Pin = 36;
AccelStepper AOA(AccelStepper::DRIVER, step7Pin, direction7Pin);


// GForce
const int enable8Pin = 38;
const int step8Pin = 40;
const int direction8Pin = 42;
AccelStepper GForce(AccelStepper::DRIVER, step8Pin, direction8Pin);

// SARI
const int enable9Pin = 44;
const int step9Pin = 46;
const int direction9Pin = 48;
AccelStepper SARI(AccelStepper::DRIVER, step9Pin, direction9Pin);


// Current Airspeed - speedCurrent
const int enable10Pin = 23;
const int step10Pin = 25;
const int direction10Pin = 27;
AccelStepper speedCurrent(AccelStepper::DRIVER, step10Pin, direction10Pin);

// Max Airspeed - speedMax
const int enable11Pin = 29;
const int step11Pin = 31;
const int direction11Pin = 33;
AccelStepper speedMax(AccelStepper::DRIVER, step11Pin, direction11Pin);


// Altimeter
const int enable12Pin = 35;
const int step12Pin = 37;
const int direction12Pin = 39;
AccelStepper altimeter(AccelStepper::DRIVER, step12Pin, direction12Pin);

// VSI
const int enable13Pin = 41;
const int step13Pin = 43;
const int direction13Pin = 45;
AccelStepper VSI(AccelStepper::DRIVER, step13Pin, direction13Pin);


void onModTimeChange(char* newValue) {
  // Setting flag to indicae DCS has started - not checking if it is currently active
  DCSACTIVE = true;
}
DcsBios::StringBuffer<6> modTimeBuffer(0x0440, onModTimeChange);

void SendDebug(String MessageToSend) {
  if ((Reflector_In_Use == 1) && (Ethernet_In_Use == 1)) {
    udp.beginPacket(reflectorIP, reflectorport);
    udp.println(MessageToSend);
    udp.endPacket();
  }
}
// ###################################### End Ethernet Related #############################

#define FLASH_TIME 300
bool RED_LED_STATE = false;
long NEXT_STATUS_TOGGLE_TIMER = 0;

#define Check_LED_R 8
#define Check_LED_G 9

#define STATUS_LED_PORT 7
#define FLASH_TIME 200


String outString;




void sendToDcsBiosMessage(const char* msg, const char* arg) {

  sendDcsBiosMessage(msg, arg);
}


void setup() {

  pinMode(Check_LED_G, OUTPUT);
  digitalWrite(Check_LED_G, true);
  pinMode(Check_LED_R, OUTPUT);
  digitalWrite(Check_LED_R, true);



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
      digitalWrite(Check_LED_G, false);
      delay(FLASH_TIME);
      digitalWrite(Check_LED_G, true);
    }

    SendDebug("Ethernet Started");
    SendDebug("A10 STEPPER TEST");
  }

  delay(FLASH_TIME);
  digitalWrite(Check_LED_G, false);
  delay(FLASH_TIME);


  SendDebug("Stepper Setup Started");


  SendDebug("Cycle Stepper5");
  Stepper5.setMaxSpeed(directMaxStepperSpeed);
  Stepper5.setAcceleration(directMaxAccel);
  Stepper5.runToNewPosition(directFullSwing);
  delay(waitTime);
  Stepper5.runToNewPosition(0);
  delay(waitTime);
  Stepper5.runToNewPosition(directFullSwing);
  delay(waitTime);
  Stepper5.runToNewPosition(0);

  SendDebug("Cycle Stepper6");
  Stepper6.setMaxSpeed(directMaxStepperSpeed);
  Stepper6.setAcceleration(directMaxAccel);
  Stepper6.runToNewPosition(directFullSwing);
  delay(waitTime);
  Stepper6.runToNewPosition(0);
  delay(waitTime);
  Stepper6.runToNewPosition(directFullSwing);
  delay(waitTime);
  Stepper6.runToNewPosition(0);

  SendDebug("Cycle Current Airspeed");
  pinMode(enable10Pin, OUTPUT);
  digitalWrite(enable10Pin, false);
  speedCurrent.setMaxSpeed(maxStepperSpeed);
  speedCurrent.setAcceleration(maxAccel);
  speedCurrent.runToNewPosition(fullSwing);
  delay(waitTime);
  speedCurrent.runToNewPosition(0);
  delay(waitTime);
  speedCurrent.runToNewPosition(fullSwing);
  delay(waitTime);
  speedCurrent.runToNewPosition(0);

  SendDebug("Cycle Max Airspeed");
  pinMode(enable11Pin, OUTPUT);
  digitalWrite(enable11Pin, false);
  speedMax.setMaxSpeed(maxStepperSpeed);
  speedMax.setAcceleration(maxAccel);
  speedMax.runToNewPosition(fullSwing);
  delay(waitTime);
  speedMax.runToNewPosition(0);
  delay(waitTime);
  speedMax.runToNewPosition(fullSwing);
  delay(waitTime);
  speedMax.runToNewPosition(0);



  SendDebug("Cycle Altimeter");
  pinMode(enable12Pin, OUTPUT);
  digitalWrite(enable12Pin, false);
  altimeter.setMaxSpeed(maxStepperSpeed);
  altimeter.setAcceleration(maxAccel);
  altimeter.runToNewPosition(fullSwing);
  delay(waitTime);
  altimeter.runToNewPosition(0);
  delay(waitTime);
  altimeter.runToNewPosition(fullSwing);
  delay(waitTime);
  altimeter.runToNewPosition(0);

  SendDebug("Cycle VSI");
  pinMode(enable13Pin, OUTPUT);
  digitalWrite(enable13Pin, false);
  VSI.setMaxSpeed(maxStepperSpeed);
  VSI.setAcceleration(maxAccel);
  VSI.runToNewPosition(fullSwing);
  delay(waitTime);
  VSI.runToNewPosition(0);
  delay(waitTime);
  VSI.runToNewPosition(fullSwing);
  delay(waitTime);
  VSI.runToNewPosition(0);


  SendDebug("Cycle SARI");
  pinMode(enable9Pin, OUTPUT);
  digitalWrite(enable9Pin, false);
  SARI.setMaxSpeed(maxStepperSpeed);
  SARI.setAcceleration(maxAccel);
  SARI.runToNewPosition(fullSwing);
  delay(waitTime);
  SARI.runToNewPosition(0);
  delay(waitTime);
  SARI.runToNewPosition(fullSwing);
  delay(waitTime);
  SARI.runToNewPosition(0);


  SendDebug("Cycle GForce");
  pinMode(enable8Pin, OUTPUT);
  digitalWrite(enable8Pin, false);
  GForce.setMaxSpeed(maxStepperSpeed);
  GForce.setAcceleration(maxAccel);
  GForce.runToNewPosition(fullSwing);
  delay(waitTime);
  GForce.runToNewPosition(0);
  delay(waitTime);
  GForce.runToNewPosition(fullSwing);
  delay(waitTime);
  GForce.runToNewPosition(0);


  SendDebug("Cycle AOA");
  pinMode(enable7Pin, OUTPUT);
  digitalWrite(enable7Pin, false);
  AOA.setMaxSpeed(maxStepperSpeed);
  AOA.setAcceleration(maxAccel);
  AOA.runToNewPosition(fullSwing);
  delay(waitTime);
  AOA.runToNewPosition(0);
  delay(waitTime);
  AOA.runToNewPosition(fullSwing);
  delay(waitTime);
  AOA.runToNewPosition(0);

  SendDebug("Stepper Setup Complete");


  SendDebug("DCS BIOS Setup Started");
  DcsBios::setup();
  SendDebug("DCS BIOS Setup Complete");
}




void loop() {

  if (millis() >= NEXT_STATUS_TOGGLE_TIMER) {
    RED_LED_STATE = !RED_LED_STATE;

    digitalWrite(Check_LED_G, RED_LED_STATE);
    digitalWrite(Check_LED_R, !RED_LED_STATE);
    NEXT_STATUS_TOGGLE_TIMER = millis() + FLASH_TIME;
  }


  DcsBios::loop();
  currentMillis = millis();
}
