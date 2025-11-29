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
#define STEPPER_MAX_SPEED 900
#define STEPPER_ZERO_SEEK_SPEED 30
#define STEPPER_ACCELERATION 600
#define CLOCKWISE_ZERO_OFFSET 2
#define ANTICLOCKWISE_ZERO_OFFSET 0


bool SENSOR_STATE;
bool LAST_SENSOR_READING = true;
#define CLOCKWISE 1
#define ANTICLOCKWISE 2
#define STOPPED 0
int direction = STOPPED;
#include <Stepper.h>

const int stepsPerRevolution = 200;  // change this to fit the number of steps per revolution
// for your motor

// initialize the stepper library on pins 8 through 11:
Stepper myStepper(stepsPerRevolution, 4,5,6,7);

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

  // set the speed at 60 rpm:
  myStepper.setSpeed(30);

}

void loop() {
  // step one revolution  in one direction:
  SendDebug("clockwise");
  myStepper.step(600);
  delay(500);

  // step one revolution in the other direction:
  SendDebug("counterclockwise");
  myStepper.step(-600);
  delay(500);
}

