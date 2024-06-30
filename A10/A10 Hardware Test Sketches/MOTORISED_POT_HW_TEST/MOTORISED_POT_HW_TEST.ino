/*

TEST FRAMEWORK FOR MOTORISED POT

*/

#define Ethernet_In_Use 1
#define DCSBIOS_In_Use 0
#define Reflector_In_Use 1


#define DCSBIOS_IRQ_SERIAL
#include "DcsBios.h"

// Ethernet Related
#include <SPI.h>
#include <Ethernet.h>
#include <EthernetUdp.h>

// I2C
#include <Wire.h>

extern "C" {
#include "utility/twi.h"  // from Wire library, so we can do bus scanning
}


#define ES1_RESET_PIN 53
// These local Mac and IP Address will be reassigned early in startup based on
// the device ID as set by address pins
byte mac[] = { 0xA8, 0x61, 0x0A, 0x9E, 0x83, 0x71 };
IPAddress ip(172, 16, 1, 114);
String strMyIP = "172.16.1.114";

// Raspberry Pi is Target
IPAddress reflectorIP(172, 16, 1, 10);
String strreflectorIP = "X.X.X.X";

// Arduino Mega holding Max7219
IPAddress max7219IP(172, 16, 1, 106);
String strMax7219IP = "172.16.1.106";



const unsigned int localport = 7788;
const unsigned int remoteport = 26027;
const unsigned int reflectorport = 27000;
const unsigned int max7219port = 7788;

EthernetUDP udp;
char packetBuffer[1000];     //buffer to store the incoming data
char outpacketBuffer[1000];  //buffer to store the outgoing data


String DebugString = "";


unsigned long nextupdate = 0;
bool outputstate;
int flashinterval = 1000;
#define RedLedMonitorPin 20
#define GreenLedMonitorPin 21


int DCS_On = 0;
int previous_DCS_State = 0;
int DCS_State = 0;


long dcsMillis;

#define RudderTrimPotUpdateInterval 100
long NextRudderTrimPositionUpdate = 0;
#define RudderTrimPotAnalogInput A14
int RudderTrimPosition = 0;
int RudderTrimLastPosition = 0;
bool TrimMotorActive = false;
bool TrimMotorPositionFound = false;

#define RudderTrimMotorA 6
#define RudderTrimMotorB 7
#define RudderTrimMotorEn 5

#define MinDesiredPosition 500
#define MaxDesiredPosition 524
#define RudderTrimMotorTimeoutInterval 15000
long RudderTrimMotorTimeout = 0;

void StopMotor() {
  digitalWrite(RudderTrimMotorA, 0);
  digitalWrite(RudderTrimMotorB, 0);
  digitalWrite(RudderTrimMotorEn, 0);
  TrimMotorActive = false;
}

void StartMotorClockwise() {
  digitalWrite(RudderTrimMotorA, 0);
  digitalWrite(RudderTrimMotorB, 1);
  digitalWrite(RudderTrimMotorEn, 1);
  TrimMotorActive = true;
}

void StartMotorCounterClockwise() {
  digitalWrite(RudderTrimMotorA, 1);
  digitalWrite(RudderTrimMotorB, 0);
  digitalWrite(RudderTrimMotorEn, 1);
  TrimMotorActive = true;
}

void SetTrimPosition() {
  // For testing - setting Trim to Center Position

  RudderTrimMotorTimeout = millis() + RudderTrimMotorTimeoutInterval;
  TrimMotorPositionFound = false;

  RudderTrimPosition = analogRead(RudderTrimPotAnalogInput);
  SendDebug("Rudder Trim Pot Position in SetTrimPosition :" + String(RudderTrimPosition));
  
  if ((RudderTrimPosition >= MinDesiredPosition) && (RudderTrimPosition <= MaxDesiredPosition)) {
    // Within Bounds Stop the motor
    TrimMotorPositionFound = true;
    StopMotor();
    SendDebug("Motor in desired zone. Position is :" + String(RudderTrimPosition));
  } else {
    if (RudderTrimPosition <= MinDesiredPosition) {
      StartMotorClockwise();
    }
    if (RudderTrimPosition >= MaxDesiredPosition) {
      StartMotorCounterClockwise();
    }
  }

}

void CheckTrimPosition() {

  if (TrimMotorActive == true) {
    if ((millis() <= RudderTrimMotorTimeout) && (TrimMotorPositionFound == false)) {
      RudderTrimPosition = analogRead(RudderTrimPotAnalogInput);
      if (RudderTrimPosition != RudderTrimLastPosition) {
        RudderTrimLastPosition = RudderTrimPosition;
        SendDebug("Rudder Trim Pot Position :" + String(RudderTrimPosition));
      }
      if ((RudderTrimPosition >= MinDesiredPosition) && (RudderTrimPosition <= MaxDesiredPosition)) {
        // Within Bounds Stop the motor
        TrimMotorPositionFound = true;
        StopMotor();
        SendDebug("Motor in desired zone. Position is :" + String(RudderTrimPosition));
      } else {
        if (RudderTrimPosition <= MinDesiredPosition) {
          StartMotorClockwise();
        }
        if (RudderTrimPosition >= MaxDesiredPosition) {
          StartMotorCounterClockwise();
        }
      }
    }
  }
}

void SendDebug(String MessageToSend) {
  if ((Reflector_In_Use == 1) && (Ethernet_In_Use == 1)) {
    udp.beginPacket(reflectorIP, reflectorport);
    udp.println(MessageToSend);
    udp.endPacket();
  }
}



void setup() {



  pinMode(RedLedMonitorPin, OUTPUT);
  pinMode(GreenLedMonitorPin, OUTPUT);
  outputstate = true;
  digitalWrite(RedLedMonitorPin, outputstate);
  digitalWrite(GreenLedMonitorPin, outputstate);



  if (Ethernet_In_Use == 1) {

    // Using manual reset instead of tying to Arduino Reset
    pinMode(ES1_RESET_PIN, OUTPUT);
    digitalWrite(ES1_RESET_PIN, LOW);
    delay(2);
    digitalWrite(ES1_RESET_PIN, HIGH);

    Ethernet.begin(mac, ip);


    udp.begin(localport);
    delay(3000);
    if (Reflector_In_Use == 1) {
      SendDebug("Motorised Pot Test rig - " + strMyIP + " " + String(millis()) + "mS since reset.");
    }
  }

  nextupdate = millis() + flashinterval;

  DcsBios::setup();

  pinMode(RudderTrimMotorA, OUTPUT);
  pinMode(RudderTrimMotorB, OUTPUT);
  digitalWrite(RudderTrimMotorA, 0);
  digitalWrite(RudderTrimMotorB, 0);
  pinMode(RudderTrimMotorEn, OUTPUT);
  digitalWrite(RudderTrimMotorEn, 0);


  SendDebug("Set Trim Position");
  SetTrimPosition();
}




void loop() {


  DcsBios::loop();

  CheckTrimPosition();

  if (millis() >= NextRudderTrimPositionUpdate) {
    NextRudderTrimPositionUpdate = millis() + RudderTrimPotUpdateInterval;
    RudderTrimPosition = analogRead(RudderTrimPotAnalogInput);
    if (RudderTrimPosition != RudderTrimLastPosition) {
      RudderTrimLastPosition = RudderTrimPosition;
      SendDebug("Rudder Trim Pot Position :" + String(RudderTrimPosition));
    }
  }

  if (millis() >= nextupdate) {
    outputstate = !outputstate;
    digitalWrite(RedLedMonitorPin, outputstate);
    digitalWrite(GreenLedMonitorPin, !outputstate);
    nextupdate = millis() + flashinterval;
  }
}
