/*
Instrument_Test_Harness 20250306

*/



#define Ethernet_In_Use 0
#define Reflector_In_Use 0
#define DCSBIOS_In_Use 0
#define MSFS_In_Use 0  // Used to interface into MSFS - set to 0 if not in use


#define Check_LED_R 15
#define Check_LED_G 14

#define RED_STATUS_LED_PORT 15
#define GREEN_STATUS_LED_PORT 14

#define FLASH_TIME 300

long NEXT_STATUS_TOGGLE_TIMER = 0;
bool RED_LED_STATE = false;
bool GREEN_LED_STATE = false;
unsigned long timeSinceRedLedChanged = 0;


// Used to Distinguish between the left, front, and right inputs
// Left=0, Front=1, Right=2
#define INPUT_MODULE_NUMBER 0

#define DCSBIOS_IRQ_SERIAL
#include "DcsBios.h"


// ############################### Begin Ethernet Related ####################################
#include <SPI.h>
#include <Ethernet.h>
#include <EthernetUdp.h>

#define EthernetStartupDelay 500
#define ES1_RESET_PIN 53

String BoardName = "A10 Forward Input";

// These local Mac and IP Address will be reassigned early in startup based on
// the device ID as set by address pins

byte myMac[] = { 0xA8, 0x61, 0x0A, 0x67, 0x83, 0x05 };
String sMac = "A8:61:0A:67:83:05";
IPAddress myIP(172, 16, 1, 105);
String strMyIP = "172.16.1.105";

// Reflector
IPAddress reflectorIP(172, 16, 1, 10);
String strReflectorIP = "X.X.X.X";

// Arduino Due for Keystroke translation and Pixel Led driving
IPAddress targetIP(172, 16, 1, 110);
String strTargetIP = "X.X.X.X";

// Computer Running MSFS
IPAddress MSFSIP(172, 16, 1, 10);
String strMSFSIP = "X.X.X.X";

const unsigned int localport = 7788;
const unsigned int localdebugport = 7795;
const unsigned int keyboardport = 7788;
const unsigned int ledport = 7789;
const unsigned int remoteport = 7790;
const unsigned int reflectorport = 27000;
const unsigned int MSFSport = 7791;

int packetSize;
int debugLen;
EthernetUDP udp;
EthernetUDP debugUDP;
// char packetBuffer[1000];     //buffer to store the incoming data
char outpacketBuffer[1000];  //buffer to store the outgoing data

const unsigned long delayBeforeSendingPacket = 2000;
unsigned long ethernetStartTime = 0;

// ############################### End Ethernet Related ####################################







void setup() {


  pinMode(GREEN_STATUS_LED_PORT, OUTPUT);
  pinMode(RED_STATUS_LED_PORT, OUTPUT);
  digitalWrite(GREEN_STATUS_LED_PORT, true);
  digitalWrite(RED_STATUS_LED_PORT, true);
  delay(FLASH_TIME);
  digitalWrite(GREEN_STATUS_LED_PORT, false);
  digitalWrite(RED_STATUS_LED_PORT, false);
  delay(FLASH_TIME);

  if (Ethernet_In_Use == 1) {

    // Reset Ethernet Module
    pinMode(ES1_RESET_PIN, OUTPUT);
    digitalWrite(ES1_RESET_PIN, LOW);
    delay(2);
    digitalWrite(ES1_RESET_PIN, HIGH);


    Ethernet.begin(myMac, myIP);
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
    SendDebug(BoardName + " Ethernet Started " + strMyIP + " " + sMac);


    // // Initialise Exterior Lights
    // pinMode(STROBE_LIGHTS, OUTPUT);
    // pinMode(NAVIGATION_LIGHTS, OUTPUT);
    // pinMode(FORMATION_LIGHTS, OUTPUT);
    // pinMode(BACK_LIGHTS, OUTPUT);
    // pinMode(FLOOD_LIGHTS, OUTPUT);
    // // Turn Everything on for 3 Seconds
    // analogWrite(STROBE_LIGHTS, 255);
    // analogWrite(NAVIGATION_LIGHTS, 255);
    // analogWrite(FORMATION_LIGHTS, 255);
    // analogWrite(BACK_LIGHTS, 255);
    // analogWrite(FLOOD_LIGHTS, 255);



    // SendDebug("Dimming Leds");
    // for (int Local_Brightness = 255; Local_Brightness >= 0; Local_Brightness--) {
    //   analogWrite(STROBE_LIGHTS, Local_Brightness);
    //   analogWrite(NAVIGATION_LIGHTS, Local_Brightness);
    //   analogWrite(FORMATION_LIGHTS, Local_Brightness);
    //   analogWrite(BACK_LIGHTS, Local_Brightness);
    //   analogWrite(FLOOD_LIGHTS, Local_Brightness);
    //   // SendDebug("Led Brightness " + String(Local_Brightness));
    //   delay(15);
    // }

// #define BrightnessWhileRunningSetup 128
//     analogWrite(STROBE_LIGHTS, BrightnessWhileRunningSetup);
//     analogWrite(NAVIGATION_LIGHTS, BrightnessWhileRunningSetup);
//     analogWrite(FORMATION_LIGHTS, BrightnessWhileRunningSetup);
//     analogWrite(BACK_LIGHTS, BrightnessWhileRunningSetup);
//     analogWrite(FLOOD_LIGHTS, BrightnessWhileRunningSetup);



    if (Reflector_In_Use == 1) {
      udp.beginPacket(reflectorIP, reflectorport);
      udp.println("INIT LEFT COMPOSITE INPUT - " + strMyIP + " " + String(millis()) + "mS since reset.");
      udp.endPacket();
    }

    // Used to remotely enable Debug and Reflector
    debugUDP.begin(localdebugport);
  }

  

  SendDebug(BoardName + " - " + strMyIP + " Setup Complete. " + String(millis()) + "mS since reset.");
}


void SendDebug(String MessageToSend) {
  if ((Reflector_In_Use == 1) && (Ethernet_In_Use == 1)) {
    udp.beginPacket(reflectorIP, reflectorport);
    udp.print(MessageToSend);
    udp.endPacket();
  }
}



void loop() {


  if (millis() >= NEXT_STATUS_TOGGLE_TIMER) {
    RED_LED_STATE = !RED_LED_STATE;
    // digitalWrite(Check_LED_G, RED_LED_STATE);
    digitalWrite(Check_LED_R, RED_LED_STATE);
    digitalWrite(Check_LED_G, !RED_LED_STATE);
    NEXT_STATUS_TOGGLE_TIMER = millis() + FLASH_TIME;
  }


}

