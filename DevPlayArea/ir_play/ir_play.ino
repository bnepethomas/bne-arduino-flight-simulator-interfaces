int Ethernet_In_Use = 0;
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
String BoardName = "Leonardo Test ";


void SendDebug(String MessageToSend) {
  if ((Reflector_In_Use == 1) && (Ethernet_In_Use == 1)) {
    udp.beginPacket(reflectorIP, reflectorport);
    udp.print(MessageToSend);
    udp.endPacket();
  }
  Serial.print(MessageToSend);
}


// ###################################### End Ethernet Related #############################



// LED blinking
unsigned long previousMillis = 0;
const long ledInterval = 500;
bool ledState = LOW;


#define RED_STATUS_LED_PORT 13
#define FLASH_TIME 300

#include <IRremote.h>

int RECV_PIN = 3;
//IRrecv irrecv(RECV_PIN);
//decode_results results;

void setup() {

 
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


    SendDebug(BoardName + " Ethernet Started " + strMyIP + " " + sMac);
  }




  SendDebug(BoardName + " Start Joystick Pins Allocation");
 
  // irrecv.enableIRIn();

  SendDebug(BoardName + " Startup Complete ");
}

void loop() {
  unsigned long currentMillis = millis();

  // === LED Flashing Status ===
  if (currentMillis - previousMillis >= ledInterval) {
    previousMillis = currentMillis;
    ledState = !ledState;
    digitalWrite(RED_STATUS_LED_PORT, ledState);
  }

  
  // if (irrecv.decode(&results)) {
  //   Serial.print("IR RECV Code = 0x ");
  //   Serial.println(results.value, HEX);
  //   irrecv.resume();
  // }
}



