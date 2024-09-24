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



#define DCS_CheckInterval 500  // Delay before considering DCS is sleeping/paused/not running
bool DCS_On = false;
long nextDCSmillisCheck = 0;
unsigned int DCS_Counter;
unsigned int last_DCS_Counter;


long dcsMillis;

#define button8Pin 8
bool button8State;
bool lastButton8State;

#define button9Pin 9
bool button9State;
bool lastButton9State;

#define button11Pin 11
bool button11State;
bool lastButton11State;

void sendToDcsBiosMessage(const char* msg, const char* arg) {

  sendDcsBiosMessage(msg, arg);
}





void onUpdateCounterChange(unsigned int newValue) {
  DCS_Counter = newValue;
}
DcsBios::IntegerBuffer UpdateCounterBuffer(0xfffe, 0x00ff, 0, onUpdateCounterChange);


void checkDCSActive() {

  if (millis() >= nextDCSmillisCheck) {

    // onUpdateCounterChange is updaed by DCS BIOS in onUpdateCounterChange
    if (DCS_Counter != last_DCS_Counter) {
      // Counter has changed so DCS is alive
      if (DCS_On == false) {
        // We have had a transition
        SendDebug("DCS has become active");
        digitalWrite(Check_LED_G, false);
      }
      DCS_On = true;
    } else {
      if (DCS_On == true) {
        SendDebug("DCS has become inactive");
        digitalWrite(Check_LED_G, true);
      }
      DCS_On = false;
    }

    last_DCS_Counter = DCS_Counter;
    nextDCSmillisCheck = millis() + DCS_CheckInterval;
  }
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
    SendDebug("A10_RADIO_SYNCH 20240727");
  }


  delay(FLASH_TIME);
  digitalWrite(Check_LED_G, false);
  delay(FLASH_TIME);

  pinMode(button8Pin, INPUT_PULLUP);
  pinMode(button9Pin, INPUT_PULLUP);
  pinMode(button11Pin, INPUT_PULLUP);

  SendDebug("DCS BIOS Setup Started");
  DcsBios::setup();
  SendDebug("DCS BIOS Setup Complete");
}


String targetString = "12";
String currentReadingString = "";
#define selector1Size 13
char* selector1[] = { " 3", " 4", " 5", " 6", " 7", " 8", " 9", "10", "11", "12", "13", "14", "15" };

void onVhffmFreq1Change(char* newValue) {
  SendDebug("VHF FM Frequency Change");
  currentReadingString = String(newValue);
}
DcsBios::StringBuffer<2> vhffmFreq1StrBuffer(0x119a, onVhffmFreq1Change);



void loop() {

  if (millis() >= NEXT_STATUS_TOGGLE_TIMER) {
    RED_LED_STATE = !RED_LED_STATE;
    // digitalWrite(Check_LED_G, RED_LED_STATE);
    digitalWrite(Check_LED_R, !RED_LED_STATE);
    NEXT_STATUS_TOGGLE_TIMER = millis() + FLASH_TIME;
  }



  if (DCS_On == true) {
    if (currentReadingString != targetString) {
      SendDebug("Incrementing VHF FM Frequency");

      SendDebug("Current Reading " + currentReadingString);

      int currentPos = 0;
      int targetPos = 0;
      int deltaPos = 0;
      bool foundCurrent = false;
      bool foundTarget = false;
      for (int i = 0; i < selector1Size; i++) {
        SendDebug("Walking Array for current :" + String(i));
        SendDebug(String(selector1[i]) + ":" + currentReadingString);
        if (String(selector1[i]) == currentReadingString) {
          SendDebug("currentRadingString Postion in array :" + String(i));
          currentPos = i;
          foundCurrent = true;
          break;
        }
      }

      for (int i = 0; i < selector1Size; i++) {
        // SendDebug("Walking Array for target :" + String(i));
        // SendDebug(String(selector1[i]) + ":" + currentReadingString );
        if (String(selector1[i]) == targetString) {
          // SendDebug("targetString Postion in array :" + String(i));
          targetPos = i;
          foundTarget = true;
          break;
        }
      }

      if (foundCurrent == false) {
        SendDebug("WARNING UNABLE TO FIND CURRENT POSITION IN ARRAY");
      }
      if (foundTarget == false) {
        SendDebug("WARNING UNABLE TO FIND TARGET POSITION IN ARRAY");
      }

      deltaPos = targetPos - currentPos;

      if (deltaPos > 0) {
        sendToDcsBiosMessage("VHFFM_FREQ1", "INC");
      } else {
        sendToDcsBiosMessage("VHFFM_FREQ1", "DEC");
      }

      SendDebug("Current (" + String(currentPos) + ") and target (" + String(targetPos) + ") Delta :" + String(currentPos - targetPos));
    }
  }
  // put your main code here, to run repeatedly:


  button8State = digitalRead(button8Pin);
  if (button8State != lastButton8State) {
    // check if the pushbutton is pressed. If it is, the buttonState is HIGH:
    if (button8State == HIGH) {
      // turn LED on:
      digitalWrite(Check_LED_G, HIGH);
      targetString = " 8";

    } else {
      // turn LED off:
      digitalWrite(Check_LED_G, LOW);
    }
    lastButton8State = button8State;
  }


  button9State = digitalRead(button9Pin);
  if (button9State != lastButton9State) {
    // check if the pushbutton is pressed. If it is, the buttonState is HIGH:
    if (button9State == HIGH) {
      // turn LED on:
      digitalWrite(Check_LED_G, HIGH);
      targetString = " 9";

    } else {
      // turn LED off:
      digitalWrite(Check_LED_G, LOW);
    }
    lastButton9State = button9State;
  }

  button11State = digitalRead(button11Pin);
  if (button11State != lastButton11State) {
    // check if the pushbutton is pressed. If it is, the buttonState is HIGH:
    if (button11State == HIGH) {
      // turn LED on:
      digitalWrite(Check_LED_G, HIGH);
      targetString = "10";

    } else {
      // turn LED off:
      digitalWrite(Check_LED_G, LOW);
    }
    lastButton11State = button11State;
  }


  DcsBios::loop();
  checkDCSActive();
}
