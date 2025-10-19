//Uni inline Sensivity with 470 ohm good for 1.5m
// Projector needed direct at 0.6 from rrear
// Projector ok at 1.5m slightly offset  with 470 ohm

int Ethernet_In_Use = 1;
int Reflector_In_Use = 1;

// ###################################### Begin Ethernet Related #############################
#include <SPI.h>
#include <Ethernet.h>
#include <EthernetUdp.h>

#define EthernetStartupDelay 500
#define ES1_RESET_PIN 11

// These local Mac and IP Address will be reassigned early in startup based on
// the device ID as set by address pins
byte mac[] = { 0xA8, 0x61, 0x0A, 0x67, 0x83, 0x6D };
String sMac = "A8:61:0A:67:83:6D";
IPAddress ip(172, 16, 1, 109);
String strMyIP = "172.16.1.109";

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

const unsigned long delayBeforeSendingPacket = 2000;
unsigned long ethernetStartTime = 0;

int packetSize;
int debugLen;
EthernetUDP udp;

char packetBuffer[1000];     //buffer to store the incoming data
char outpacketBuffer[1000];  //buffer to store the outgoing data
String DebugString = "";
String BoardName = "IR Sender";


void SendDebug(String MessageToSend) {
  if ((Reflector_In_Use == 1) && (Ethernet_In_Use == 1)) {
    udp.beginPacket(reflectorIP, reflectorport);
    udp.print(BoardName + " " + MessageToSend);
    udp.endPacket();
  }
  Serial.print(MessageToSend);
}


// ###################################### End Ethernet Related #############################



// LED blinking
unsigned long previousMillis = 0;
const long ledInterval = 500;
bool ledState = LOW;


#define RED_STATUS_LED_PORT 16
#define FLASH_TIME 300



const int powerButtonPin = 14;  // Power Down button
const int wakeButtonPin = 15;   // Wake Up button

// === State Tracking ===
bool lastPowerState = HIGH;
bool lastWakeState = HIGH;

unsigned long debounceDelay = 50;


#include "PinDefinitionsAndMore.h"
#include <IRremote.hpp> // include the library


int RECV_PIN = 3;
//IRrecv irrecv(RECV_PIN);
//decode_results results;

void sendShiftLeft() {
  uint8_t sCommand = 0xE;
  uint8_t sRepeats = 0;

  SendDebug("Send now: address=0x00, command=0x" + String(sCommand, HEX)  + " repeat " + String(sRepeats));
  SendDebug("Send standard NEC with 8 bit address");


  // Receiver output for the first loop must be: Protocol=NEC Address=0x102 Command=0x34 Raw-Data=0xCB340102 (32 bits)
  IrSender.sendNEC(0x00, sCommand, sRepeats);

}

void powerDown() {
  uint8_t sCommand = 0x14;
  uint8_t sRepeats = 0;

  SendDebug("Send now: address=0x00, command=0x" + String(sCommand, HEX)  + " repeat " + String(sRepeats));
  SendDebug("Send standard NEC with 8 bit address");


  // Receiver output for the first loop must be: Protocol=NEC Address=0x102 Command=0x34 Raw-Data=0xCB340102 (32 bits)
  IrSender.sendNEC(0x00, sCommand, sRepeats);

  delay(1000);
  sCommand = 0x7;
  IrSender.sendNEC(0x00, sCommand, sRepeats);

}

void setup() {
  pinMode(powerButtonPin, INPUT_PULLUP);
  pinMode(wakeButtonPin, INPUT_PULLUP);

  pinMode(RED_STATUS_LED_PORT, OUTPUT);

  digitalWrite(RED_STATUS_LED_PORT, true);
  delay(FLASH_TIME);

  Serial.begin(9600);

  if (Ethernet_In_Use == 1) {

    // Using manual reset instead of tying to Arduino Reset
    pinMode(ES1_RESET_PIN, OUTPUT);
    digitalWrite(ES1_RESET_PIN, LOW);
    delay(2);
    digitalWrite(ES1_RESET_PIN, HIGH);

    Ethernet.begin(mac, ip);
    udp.begin(localport);

    ethernetStartTime = millis() + delayBeforeSendingPacket;
    while (millis() <= ethernetStartTime) {
      delay(FLASH_TIME);
      digitalWrite(RED_STATUS_LED_PORT, false);
      delay(FLASH_TIME);
      digitalWrite(RED_STATUS_LED_PORT, true);
    }


    SendDebug(" Ethernet Started " + strMyIP + " " + sMac);
  }





  // irrecv.enableIRIn();

  SendDebug("Startup Complete");
}

void loop() {
  unsigned long currentMillis = millis();

  // === LED Flashing Status ===
  if (currentMillis - previousMillis >= ledInterval) {
    previousMillis = currentMillis;
    ledState = !ledState;
    digitalWrite(RED_STATUS_LED_PORT, ledState);
  }

  // === Power Down Button ===
  bool powerState = digitalRead(powerButtonPin);
  if (powerState == LOW && lastPowerState == HIGH) {
    delay(debounceDelay);
    if (digitalRead(powerButtonPin) == LOW) {
      SendDebug("Power Down");
      powerDown();
    }
  }

  // === Wake Up Button ===
  bool wakeState = digitalRead(wakeButtonPin);
  if (wakeState == LOW && lastWakeState == HIGH) {
    delay(debounceDelay);
    if (digitalRead(wakeButtonPin) == LOW) {
      SendDebug("Wake Up");
      sendShiftLeft();
    }
  }

  lastPowerState = powerState;
  lastWakeState = wakeState;

  // if (irrecv.decode(&results)) {
  //   Serial.print("IR RECV Code = 0x ");
  //   Serial.println(results.value, HEX);
  //   irrecv.resume();
  // }
}
