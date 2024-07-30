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

#include <AccelStepper.h>

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
  SendDebug("Cycle Current Airspeed");
  pinMode(enable10Pin, OUTPUT);
  digitalWrite(enable10Pin, false);
  speedCurrent.setMaxSpeed(6000);
  speedCurrent.setAcceleration(6000);
  speedCurrent.runToNewPosition(5020);
  delay(050);
  speedCurrent.runToNewPosition(0);
  delay(500);
  speedCurrent.runToNewPosition(5020);
  delay(500);
  speedCurrent.runToNewPosition(0);

  SendDebug("Cycle Max Airspeed");
  pinMode(enable11Pin, OUTPUT);
  digitalWrite(enable11Pin, false);
  speedMax.setMaxSpeed(6000);
  speedMax.setAcceleration(6000);
  speedMax.runToNewPosition(5020);
  delay(050);
  speedMax.runToNewPosition(0);
  delay(500);
  speedMax.runToNewPosition(5020);
  delay(500);
  speedMax.runToNewPosition(0);
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
