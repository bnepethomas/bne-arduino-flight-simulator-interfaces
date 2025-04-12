////////////////////---||||||||||********||||||||||---\\\\\\\\\\\\\\\\\\\\
//||               FUNCTION = GAUGES INSTRUMENT CONTROLLER             ||\\
//||              LOCATION IN THE PIT = RIGHTHAND SIDE CONSOLE         ||\\
//||            ARDUINO PROCESSOR TYPE = Arduino Mega 2560            ||\\
//||      ARDUINO CHIP SERIAL NUMBER = SN - xxxxxxxxxxxxxxxxxxxx      ||\\
//||                    CONNECTED COM PORT = TBS.                     ||\\
//||               ****ADD ASSIGNED COM PORT NUMBER****               ||\\
//||            ****DO CHECK S/N BEFORE UPLOAD NEW DATA****           ||\\
////////////////////---||||||||||********||||||||||---\\\\\\\\\\\\\\\\\\\\

int Ethernet_In_Use = 1;
int Reflector_In_Use = 1;

#define MSFS_In_Use 0  // Used to interface into MSFS - set to 0 if not in use


#define DCSBIOS_In_Use 1

#define DCSBIOS_IRQ_SERIAL
#include "DcsBios.h"

// ###################################### Begin Ethernet Related #############################
#include <SPI.h>
#include <Ethernet.h>
#include <EthernetUdp.h>

#define EthernetStartupDelay 500
#define ES1_RESET_PIN 53

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

const unsigned long delayBeforeSendingPacket = 2000;
unsigned long ethernetStartTime = 0;

int packetSize;
int debugLen;
EthernetUDP udp;
EthernetUDP debugUDP;
char packetBuffer[1000];     //buffer to store the incoming data
char outpacketBuffer[1000];  //buffer to store the outgoing data
String DebugString = "";
String BoardName = "GAUGES INST CONTROLLER";


void SendDebug(String MessageToSend) {
  if ((Reflector_In_Use == 1) && (Ethernet_In_Use == 1)) {
    udp.beginPacket(reflectorIP, reflectorport);
    udp.print(MessageToSend);
    udp.endPacket();
  }
}

void sendToDcsBiosMessage(const char* msg, const char* arg) {

  SendDebug(BoardName + "DCSBIOS - " + String(msg) + ":" + String(arg));
  sendDcsBiosMessage(msg, arg);
}
// ###################################### End Ethernet Related #############################

#define RED_STATUS_LED_PORT 15
#define GREEN_STATUS_LED_PORT 14
#define Check_LED_R 15
#define Check_LED_G 14
#define FLASH_TIME 300

unsigned long NEXT_STATUS_TOGGLE_TIMER = 0;
bool GREEN_LED_STATE = false;
bool RED_LED_STATE = false;
unsigned long timeSinceRedLedChanged = 0;


// ################################### BEGIN LIGHTING ##################################

#define ASH_DDI_PWM_5V 46
#define BACK_LIGHTS 11
#define BAT_HYD_DIM 3
#define BRK_PRESS_DIM 5
#define CAB_ALT_DIM 2
#define COMPASS_DIM 12
#define MAP_DIM 4
#define RADAR_ALTIMETER_DIM 6
#define SPARE_DIM 45

void onConsoleIntLtChange(unsigned int newValue) {
  SendDebug("Console Lights : " + String(newValue));
  analogWrite(BACK_LIGHTS, map(newValue, 0, 65535, 0, 255));
}
DcsBios::IntegerBuffer consoleIntLtBuffer(0x7558, 0xffff, 0, onConsoleIntLtChange);

// ################################### END LIGHTING ##################################


long dcsMillis;





long prevLEDTransition = millis();
int cButtonID[16];
bool bFirstTime = false;


unsigned long currentMillis = 0;
unsigned long previousMillis = 0;


char stringind[5];
String outString;



// CONSOLE LIGHTS

void onConsolesDimmerChange(unsigned int newValue) {
  analogWrite(11, map(newValue, 0, 65535, 0, 150));
}
DcsBios::IntegerBuffer consolesDimmerBuffer(0x7544, 0xffff, 0, onConsolesDimmerChange);



void setup() {

  pinMode(GREEN_STATUS_LED_PORT, OUTPUT);
  pinMode(RED_STATUS_LED_PORT, OUTPUT);
  digitalWrite(GREEN_STATUS_LED_PORT, true);
  digitalWrite(RED_STATUS_LED_PORT, true);
  delay(FLASH_TIME);
  digitalWrite(GREEN_STATUS_LED_PORT, false);
  delay(FLASH_TIME);

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
      digitalWrite(Check_LED_G, false);
      delay(FLASH_TIME);
      digitalWrite(Check_LED_G, true);
    }


    SendDebug(BoardName + " Ethernet Started " + strMyIP + " " + sMac);
  }

  // Lights

  pinMode(ASH_DDI_PWM_5V, OUTPUT);
  pinMode(BACK_LIGHTS, OUTPUT);
  pinMode(BAT_HYD_DIM, OUTPUT);
  pinMode(BRK_PRESS_DIM, OUTPUT);
  pinMode(CAB_ALT_DIM, OUTPUT);
  pinMode(COMPASS_DIM, OUTPUT);
  pinMode(MAP_DIM, OUTPUT);
  pinMode(RADAR_ALTIMETER_DIM, OUTPUT);
  pinMode(SPARE_DIM, OUTPUT);

  SendDebug("Leds On");

  analogWrite(ASH_DDI_PWM_5V, 255);
  analogWrite(BACK_LIGHTS, 255);
  analogWrite(BAT_HYD_DIM, 255);
  analogWrite(BRK_PRESS_DIM, 255);
  analogWrite(CAB_ALT_DIM, 255);
  analogWrite(COMPASS_DIM, 255);
  analogWrite(MAP_DIM, 255);
  analogWrite(RADAR_ALTIMETER_DIM, 255);
  analogWrite(SPARE_DIM, 255);
  delay(3000);

  SendDebug("Dimming Leds");
  for (int Local_Brightness = 255; Local_Brightness >= 0; Local_Brightness--) {
    analogWrite(ASH_DDI_PWM_5V, Local_Brightness);
    analogWrite(BACK_LIGHTS, Local_Brightness);
    analogWrite(BAT_HYD_DIM, Local_Brightness);
    analogWrite(BRK_PRESS_DIM, Local_Brightness);
    analogWrite(CAB_ALT_DIM, Local_Brightness);
    analogWrite(COMPASS_DIM, Local_Brightness);
    analogWrite(MAP_DIM, Local_Brightness);
    analogWrite(RADAR_ALTIMETER_DIM, Local_Brightness);
    analogWrite(SPARE_DIM, Local_Brightness);
    // SendDebug("Led Brightness " + String(Local_Brightness));
    delay(15);
  }
  SendDebug("Leds Brightness to Setup Level");

#define BrightnessWhileRunningSetup 128
  analogWrite(ASH_DDI_PWM_5V, BrightnessWhileRunningSetup);
  analogWrite(BACK_LIGHTS, BrightnessWhileRunningSetup);
  analogWrite(BAT_HYD_DIM, BrightnessWhileRunningSetup);
  analogWrite(BRK_PRESS_DIM, BrightnessWhileRunningSetup);
  analogWrite(CAB_ALT_DIM, BrightnessWhileRunningSetup);
  analogWrite(COMPASS_DIM, BrightnessWhileRunningSetup);
  analogWrite(MAP_DIM, BrightnessWhileRunningSetup);
  analogWrite(RADAR_ALTIMETER_DIM, BrightnessWhileRunningSetup);
  analogWrite(SPARE_DIM, BrightnessWhileRunningSetup);

  DcsBios::setup();

  SendDebug("Leds Brightness to Post Setup Level");
#define BrightnessPostSetup 65

  analogWrite(ASH_DDI_PWM_5V, BrightnessPostSetup);
  analogWrite(BACK_LIGHTS, BrightnessPostSetup);
  analogWrite(BAT_HYD_DIM, BrightnessPostSetup);
  analogWrite(BRK_PRESS_DIM, BrightnessPostSetup);
  analogWrite(CAB_ALT_DIM, BrightnessPostSetup);
  analogWrite(COMPASS_DIM, BrightnessPostSetup);
  analogWrite(MAP_DIM, BrightnessPostSetup);
  analogWrite(RADAR_ALTIMETER_DIM, BrightnessPostSetup);
  analogWrite(SPARE_DIM, BrightnessPostSetup);

  SendDebug(BoardName + " End Setup");
}









void loop() {

  if (millis() >= NEXT_STATUS_TOGGLE_TIMER) {
    GREEN_LED_STATE = !GREEN_LED_STATE;
    RED_LED_STATE = !GREEN_LED_STATE;
    digitalWrite(GREEN_STATUS_LED_PORT, GREEN_LED_STATE);
    digitalWrite(RED_STATUS_LED_PORT, RED_LED_STATE);
    NEXT_STATUS_TOGGLE_TIMER = millis() + FLASH_TIME;
  }

  DcsBios::loop();
}
