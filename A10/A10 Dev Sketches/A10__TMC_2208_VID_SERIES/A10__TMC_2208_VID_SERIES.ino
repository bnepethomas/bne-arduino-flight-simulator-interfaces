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
char* ParameterNamePtr;
char* ParameterValuePtr;


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

#include <AccelStepper.h>

// ###################################### Begin SARI ######################################


#define Check_LED_R 12
#define Check_LED_G 13

#define HOZ_IR_sen 24  // HOZ
#define VER_IR_sen 25  // VRT
#define WTR_IR_sen 22  // WTR

#define EN_switchV 37  // VER
#define EN_switchH 43  // HOZ
#define EN_switchW 27  // WTR

int DCS_On = 0;
int previous_DCS_State = 0;
int DCS_State = 0;


long dcsMillis;



#define STEPPER_MAX_SPEED 9000
#define STEPPER_ZERO_SEEK_SPEED 600
// #define STEPPER_ACCELERATION 600
#define STEPPER_ACCELERATION 9000
const int SARIenablePin = 4;
const int SARIstepPin = 6;
const int SARIdirectionPin = 7;

#define STEPS 10080
#define STEPS 315 * 8
// define AccelStepper instance
// AccelStepper STEPPER_1(AccelStepper::FULL4WIRE, COIL_STEPPER_1_A, COIL_STEPPER_1_B, COIL_STEPPER_1_C, COIL_STEPPER_1_D);
AccelStepper STEPPER_1(AccelStepper::DRIVER, SARIstepPin, SARIdirectionPin);




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
    SendDebug("A10_TMC_2208_VID_DEV 20240721");
  }


  delay(FLASH_TIME);
  digitalWrite(Check_LED_G, false);
  delay(FLASH_TIME);

  SendDebug("Stepper Initialisation Started");

  pinMode(SARIenablePin, OUTPUT);

  STEPPER_1.setMaxSpeed(STEPPER_MAX_SPEED);
  STEPPER_1.setAcceleration(STEPPER_ACCELERATION);

  SendDebug("Max Speed: " + String(STEPPER_MAX_SPEED));
  SendDebug("Acceleration: " + String(STEPPER_ACCELERATION));
  SendDebug("Initialise Stepper - Two turns and then return to starting point");

  pinMode(SARIenablePin, OUTPUT);
  digitalWrite(SARIenablePin, false);
  for (int i = 1; i <= 4; i++) {
    SendDebug("Loop :" + String(i));
    STEPPER_1.moveTo(-STEPS * 1);
    while (STEPPER_1.distanceToGo() != 0) {
      STEPPER_1.run();
      // SendDebug("Step Speed: " + String(STEPPER_1.speed()));
    }
    delay(200);

    STEPPER_1.moveTo(0);
    while (STEPPER_1.distanceToGo() != 0) {
      STEPPER_1.run();
    }
    delay(200);
  }





  SendDebug("Stepper Initialisation Complete");
}






void loop() {

  if (millis() >= NEXT_STATUS_TOGGLE_TIMER) {
    RED_LED_STATE = !RED_LED_STATE;
    digitalWrite(Check_LED_G, RED_LED_STATE);
    digitalWrite(Check_LED_R, !RED_LED_STATE);
    NEXT_STATUS_TOGGLE_TIMER = millis() + FLASH_TIME;
  }
}
