/*



////////////////////---||||||||||********||||||||||---\\\\\\\\\\\\\\\\\\\\
//||               FUNCTION = A10 FRONT CONSOLE INPUT CONTROLLER      ||\\
//||              LOCATION IN THE PIT = FRONT PORT                     ||\\
//||            ARDUINO PROCESSOR TYPE = Arduino Mega 2560            ||\\
//||      ARDUINO CHIP SERIAL NUMBER = SN -      ||\\
//||                    CONNECTED COM PORT = COM 4                    ||\\
//||               ****ADD ASSIGNED COM PORT NUMBER****               ||\\
//||            ****DO CHECK S/N BEFORE UPLOAD NEW DATA****           ||\\
////////////////////---||||||||||********||||||||||---\\\\\\\\\\\\\\\\\\\\

*/


int Ethernet_In_Use = 1;
int Reflector_In_Use = 1;

const int powerButtonPin = 2;  // Power Down button
bool lastPowerState = HIGH;
unsigned long debounceDelay = 50;


// ###################################### Begin Ethernet Related #############################
#include <SPI.h>
#include <Ethernet.h>
#include <EthernetUdp.h>

#define EthernetStartupDelay 500
#define ES1_RESET_PIN 53

String BoardName = "WoL Test";

// These local Mac and IP Address will be reassigned early in startup based on
// the device ID as set by address pins
byte mac[] = { 0xA8, 0x61, 0x0A, 0x67, 0x83, 0x00 };
String sMac = "A8:61:0A:67:83:00";
IPAddress ip(172, 16, 1, 100);
String strMyIP = "172.16.1.100";

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
char packetBuffer[1000];     //buffer to store the incoming data
char outpacketBuffer[1000];  //buffer to store the outgoing data
const unsigned long delayBeforeSendingPacket = 2000;
unsigned long ethernetStartTime = 0;


void SendDebug(String MessageToSend) {
  if ((Reflector_In_Use == 1) && (Ethernet_In_Use == 1)) {
    udp.beginPacket(reflectorIP, reflectorport);
    udp.print(MessageToSend);
    udp.endPacket();
  }
}
// ###################################### End Ethernet Related #############################

// THE LED PORTS WILL CHANGE FROM THE V1.1 PCB TO THE FOLLOWING
#define RED_STATUS_LED_PORT 12
#define GREEN_STATUS_LED_PORT 13
#define Check_LED_R 14
#define Check_LED_G 15

#define FLASH_TIME 300

unsigned long NEXT_STATUS_TOGGLE_TIMER = 0;
bool GREEN_LED_STATE = false;
bool RED_LED_STATE = false;
unsigned long timeSinceRedLedChanged = 0;







void setup() {

  pinMode(powerButtonPin, INPUT_PULLUP);

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

    SendDebug(BoardName + " Ethernet Started " + strMyIP + " " + sMac);
  };

  SendDebug(BoardName + "Setup Complete");
}

byte targetMAC[] = {0x10, 0xFF, 0xE0, 0xB5, 0xB4, 0xBD };
void sendWOL(byte *mac) {

  // 10:ff:e0:b5:b4:bd
  IPAddress broadcastIP(255, 255, 255, 255);  // LAN broadcast address

  const int wolPort = 9; // UDP port for Wake-on-LAN
  const int packetSize = 102; // 6 x 0xFF + 16 x MAC(6 bytes)
  byte magicPacket[packetSize];

  // 6 times 0xFF
  for (int i = 0; i < 6; i++) {
    magicPacket[i] = 0xFF;
  }

  // 16 times target MAC
  for (int i = 6; i < packetSize; i++) {
    magicPacket[i] = mac[(i - 6) % 6];
  }

  // Send packet to broadcast IP
  udp.beginPacket(broadcastIP, wolPort);
  udp.write(magicPacket, packetSize);
  udp.endPacket();
}

void loop() {

  if (millis() >= NEXT_STATUS_TOGGLE_TIMER) {
    RED_LED_STATE = !RED_LED_STATE;

    digitalWrite(Check_LED_G, RED_LED_STATE);
    digitalWrite(Check_LED_R, !RED_LED_STATE);
    NEXT_STATUS_TOGGLE_TIMER = millis() + FLASH_TIME;
  }

    // === Power Down Button ===
  bool powerState = digitalRead(powerButtonPin);
  if (powerState == LOW && lastPowerState == HIGH) {
    delay(debounceDelay);
    if (digitalRead(powerButtonPin) == LOW) {
      sendWOL(targetMAC);
    }
  }
}
