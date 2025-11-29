// Board Arduino Micro
//#include "NexTouch.h"

// Download Library from https://github.com/itead/ITEADLIB_Arduino_Nextion
// Add ZIP file to project ITEADLIB_Arduino_Nextion-master


// Serial2 compiles errors
// ref https://itead.cc/nextion/nextion-tutorial-based-on-nextion-arduino-library/?srsltid=AfmBOoqjr-2KySzelSjjfIg4BWQui5Hna7WOrQw9ph1gyDOVRar7Zqva
/*

1.Open the file “NexConfig.h” from ITEADLIB_Arduino_Nextion_Library.
Comment out debug serial. #define DEBUG_SERIAL_ENABLE —> //#define DEBUG_SERIAL_ENABLE
Modify the definition of serial 2 as the default serial. #define nexSerial Serial2 —> #define nexSerial Serial
*/

// C:\Users\bnepe\Documents\Arduino\libraries\ITEADLIB_Arduino_Nextion-master\

#include <Nextion.h>  // Nextion Library
#include <Keypad.h>
#include <HID.h>
#include <HID-Project.h>  //include HID_Project library
#include <HID-Settings.h>



#define REVERSED true  //if your controller is reversed change it beginto true
#include <SoftwareSerial.h>
#define NEXTION Serial1  // Use Serial1 for Micro
#define nexSerial Serial1

#include "NexButton.h"
#include "NexText.h"

#define FLASH_TIME 300

int Serial_In_Use = 0;
int Ethernet_In_Use = 0;
int Reflector_In_Use = 0;
// ********************************* Begin Ethernet ***************************************************
// Ethernet Related
#include <SPI.h>
#include <Ethernet.h>
#include <EthernetUdp.h>

#define EthernetStartupDelay 500
#define ES1_RESET_PIN 12

const unsigned long delayBeforeSendingPacket = 3000;
unsigned long ethernetStartTime = 0;
String BoardName = "UIP Controller: ";

// These local Mac and IP Address will be reassigned early in startup based on
// the device ID as set by address pins
byte mac[] = { 0xA8, 0x61, 0x0A, 0x9E, 0x83, 0x74 };
String sMac = "A8:61:0A:67:83:115";
IPAddress ip(172, 16, 1, 116);
String strMyIP = "172.16.1.116";

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

void SendDebug(String MessageToSend) {
  MessageToSend = BoardName + MessageToSend;
  if ((Reflector_In_Use == 1) && (Ethernet_In_Use == 1)) {
    udp.beginPacket(reflectorIP, reflectorport);
    udp.print(MessageToSend);
    udp.endPacket();
  }

  if (Serial_In_Use == 1) {
    Serial.println(MessageToSend);
  }
}

// ********************************* End Ethernet ***************************************************





int VolPot = A4;  //Volume
int DIMPot = A5;  //NEXTION DIMMER

// Define matrix size
const byte ROWS = 2;
const byte COLS = 8;
int VolUp;  // main pit volume from pot UP
int VolDn;  // main pit volume from pot DOWN
int valVol = 0;
int previousvalVol = 0;
int valVol2 = 0;
int brightness = 100;

#define SafetyTimeOut 10000  // 10 Sec
#define WaitOneHour 3600000
long GlobalLastkeyPressed = WaitOneHour;

// Define row and column pins
byte rowPins[ROWS] = { A0, A1 };                    //MATRIX ROWS
byte colPins[COLS] = { 4, 5, 6, 7, 8, 9, 10, 11 };  //MATRIX COLS

// Keymap with unique characters per button
char keys[ROWS][COLS] = {
  { 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h' },  // Row 0 PUSH BUTTONS x 7 BUTTONS NOTE - PB8 NOT USED
  { '1', '2', '3', '4', '5', '6', '7', '8' }   // Row 1 ROTARY SWITCH x 8 INPUTS
  ///// PHYSICAL BUTTONS ABOVE - SOFT KEYS FROM ARDUINO ALSO INCLUDED IN SERIAL
};
// Set up keypad
Keypad keypad = Keypad(makeKeymap(keys),  colPins,rowPins, COLS, ROWS);

// Create keypad object

// Function to map key labels to actual HID keycodes
void pressMappedKey(char key) {
  switch (key) {
    case 'a':  //SW1 //START STOP EXIT
      Keyboard.press(KEY_ESC);
      delay(10);
      Keyboard.releaseAll();
      break;
    case 'b':  //SW3 //"Active Pause" feature is Left Shift + Left Windows Key + Pause/Break
      Keyboard.press(KEY_LEFT_SHIFT);
      delay(10);
      Keyboard.press(KEY_LEFT_WINDOWS);
      delay(10);
      Keyboard.press(KEY_PAUSE);
      delay(10);
      Keyboard.releaseAll();
      break;
    case 'c':  //SW4 - SALUITE Left Control + Left Shift + Left Alt + S
      Keyboard.press(KEY_LEFT_CTRL);
      delay(10);
      Keyboard.press(KEY_LEFT_SHIFT);
      delay(10);
      Keyboard.press(KEY_LEFT_ALT);
      delay(10);
      Keyboard.press('s');
      delay(10);
      Keyboard.releaseAll();
      break;
    case 'd':                         // SW5 //WARP
      Keyboard.press(KEY_LEFT_CTRL);  // FOR YOU TO SET PER YOUR SHORT CUT
      delay(10);
      Keyboard.press(KEY_F4);  // FOR YOU TO SET PER YOUR SHORT CUT
      delay(10);
      Keyboard.releaseAll();
      delay(10);
      Keyboard.press(KEY_F1);  // CURRENTLY TO RETURN TO COCKPIT
      delay(10);
      Keyboard.releaseAll();
      break;
    case 'e':  //SW7 //VIRTUAL COCKPIT (ON OFF)
      Keyboard.press(KEY_LEFT_ALT);
      delay(10);
      Keyboard.press(KEY_F1);
      delay(10);
      Keyboard.releaseAll();
      break;
    case 'f':  //SW8 //ATC
      Keyboard.press('\\');
      delay(10);
      Keyboard.releaseAll();
      break;
    case 'g':  // SW9 //LABELS Left Shift & F10.
      Keyboard.press(KEY_LEFT_SHIFT);
      delay(10);
      Keyboard.press(KEY_F10);
      delay(10);
      Keyboard.releaseAll();
      break;
    ///////////////////ROTARY KNOB///////////////////
    case '1':  /// NOT SET IN STONE YET
      Keyboard.press('KEY_F1');
      delay(10);
      Keyboard.releaseAll();
      break;
    case '2':
      Keyboard.press('KEY_F10');
      delay(10);
      Keyboard.releaseAll();
      break;
    case '3':
      Keyboard.press('KEY_F1');
      delay(10);
      Keyboard.releaseAll();
      break;
    case '4':
      Keyboard.press(KEY_LEFT_CTRL);
      delay(10);
      Keyboard.press('KEY_F4');
      delay(10);
      Keyboard.releaseAll();
      break;
    case '5':
      Keyboard.press('KEY_F');
      delay(10);
      Keyboard.releaseAll();
      break;
    case '6':
      Keyboard.press('KEY_F3');
      delay(10);
      Keyboard.releaseAll();
      break;
    case '7':
      Keyboard.press('KEY_F6');
      delay(10);
      Keyboard.releaseAll();
      break;
    case '8':
      Keyboard.press(KEY_LEFT_CTRL);
      delay(10);
      Keyboard.press('KEY_F5');
      delay(10);
      Keyboard.releaseAll();
      break;
    default:
      return;
  }
  delay(10);
  Keyboard.releaseAll();
}
//PAGE - MENU
//NexButton m_START = NexButton(1, 1, "b0"); //START/FLY
//NexButton m_WARP = NexButton(1, 6, "b2"); //WARP

//PAGE - KEYPAD MAP
// B0 - NOT USED INTERNAL "MAIN MENU"
NexButton k_F1 = NexButton(2, 2, "b1");     //F1
NexButton k_F2 = NexButton(2, 3, "b2");     //F2
NexButton k_F3 = NexButton(2, 4, "b3");     //F3
NexButton k_F4 = NexButton(2, 5, "b4");     //F4
NexButton k_F5 = NexButton(2, 6, "b5");     //F5
NexButton k_F6 = NexButton(2, 7, "b6");     //F6
NexButton k_F7 = NexButton(2, 8, "b7");     //F7
NexButton k_F8 = NexButton(2, 9, "b8");     //F8
NexButton k_F9 = NexButton(2, 10, "b9");    //F9
NexButton k_F10 = NexButton(2, 11, "b10");  //F10
NexButton k_F11 = NexButton(2, 12, "b11");  //F11
NexButton k_F12 = NexButton(2, 13, "b12");  //F12
/*
NexButton k_LAB = NexButton(2, 14, "b13"); //LABELS TOGGLE
NexButton k_ATC = NexButton(2, 15, "b14"); //ATC "\"
NexButton k_SAL = NexButton(2, 16, "b15"); // SALUTE
NexButton k_CPT = NexButton(2, 17, "b16"); //COCKPIT "LEFT ALT F1"
NexButton k_SV = NexButton(2, 18, "b17"); //SNAP VIEW
NexButton k_WARP = NexButton(2, 19, "b18"); //WARP
NexButton k_PAUSE = NexButton(2, 20, "b19"); // PAUSE KEY
NexButton k_ESC = NexButton(2, 21, "b20"); // EXIT 'ECS KEY'
NexButton k_MOUSE = NexButton(2, 22, "b21"); // SIM CONTROL MOUSE ON / OFF
*/
// COMPUTER CONTROL
//NexButton c_VUP = NexButton(4, 1, "b0"); //VOLUME UP  // VOL not working yet
//NexButton c_VMT = NexButton(4, 5, "b4"); //VOLUME MUTE  // VOL not working yet
//NexButton c_VDN = NexButton(4, 3, "b2"); //VOLUME DOWN  // VOL not working yet
NexButton c_TAB = NexButton(3, 2, "b7");
NexButton c_ENT = NexButton(3, 3, "b5");
NexButton c_AE = NexButton(3, 4, "b6");
NexButton c_WIN = NexButton(3, 5, "b8");
//NexButton c_WARP = NexButton(3, 17, "b14");

// VIEWS (not standard code)
NexButton v_IN = NexButton(4, 6, "b0");   //IN
NexButton v_OUT = NexButton(4, 8, "b4");  //OUT
NexButton v_RS = NexButton(4, 7, "b2");   //RESET // fix later

NexButton v_UP = NexButton(4, 1, "b9");   //UP
NexButton v_LF = NexButton(4, 5, "b13");  //LEFT
NexButton v_RT = NexButton(4, 4, "b12");  //RIGHT
NexButton v_CN = NexButton(4, 2, "b10");  //CENTRE
NexButton v_DN = NexButton(4, 3, "b11");  //DOWN


NexButton v_TL = NexButton(4, 9, "b5");   //TOP LEFT "7"
NexButton v_TR = NexButton(4, 10, "b6");  //TOP RIGHT "9"
NexButton v_BL = NexButton(4, 11, "b7");  //BOT LEFT "1"
NexButton v_BR = NexButton(4, 12, "b8");  //BOT RIGHT "3"

//NexButton v_WARP = NexButton(5, 16, "b14"); //WARP

//WARNING - THIS IS A PC SHUTDOWN CODE
NexButton p_PCOFF = NexButton(6, 1, "b0");  //TURN OFF PC REMOTE
//WARNING - THIS IS A PC SHUTDOWN CODE


NexTouch *nex_listen_list[] = {

  // &m_START, &m_WARP,

  &k_F1, &k_F2, &k_F3, &k_F4, &k_F5, &k_F6, &k_F7, &k_F8, &k_F9, &k_F10, &k_F11, &k_F12,
  // &k_LAB, &k_ATC, &k_SAL, &k_CPT, &k_SV, &k_WARP, &k_PAUSE, &k_ESC, &k_MOUSE,

  &c_TAB, &c_ENT, &c_AE, &c_WIN,  //&c_WARP, & c_VUP, & c_VDN, & c_VMT,

  &v_IN, &v_OUT, &v_UP, &v_LF, &v_RT, &v_CN, &v_DN, &v_RS, &v_TL, &v_TR, &v_BL, &v_BR,

  &p_PCOFF,
  NULL
};

//--------------------------------------------------------------------------------
void m_STARTPressCallback(void *ptr) {  //press
  Keyboard.press(208);
  GlobalLastkeyPressed = millis();
}
void m_STARTReleaseCallback(void *ptr) {  //release
  Keyboard.releaseAll();
}
//--------------------------------------------------------------------------------
void m_WARPPressCallback(void *ptr) {  //press
  Keyboard.press(KEY_LEFT_SHIFT);
  delay(10);
  Keyboard.press(KEY_F1);
  GlobalLastkeyPressed = millis();
}
void m_WARPReleaseCallback(void *ptr) {  //release
  Keyboard.releaseAll();
}
//--------------------------------------------------------------------------------
void k_F1PressCallback(void *ptr) {  //press
  Keyboard.press(KEY_F1);
  GlobalLastkeyPressed = millis();
}
void k_F1ReleaseCallback(void *ptr) {  //release
  Keyboard.releaseAll();
}
//--------------------------------------------------------------------------------
void k_F2PressCallback(void *ptr) {  //press
  Keyboard.press(KEY_F2);
  GlobalLastkeyPressed = millis();
}
void k_F2ReleaseCallback(void *ptr) {  //release
  Keyboard.releaseAll();
}
//--------------------------------------------------------------------------------
void k_F3PressCallback(void *ptr) {  //press
  Keyboard.press(KEY_F3);
  GlobalLastkeyPressed = millis();
}
void k_F3ReleaseCallback(void *ptr) {  //release
  Keyboard.releaseAll();
}  //--------------------------------------------------------------------------------
void k_F4PressCallback(void *ptr) {  //press
  Keyboard.press(KEY_F4);
  GlobalLastkeyPressed = millis();
}
void k_F4ReleaseCallback(void *ptr) {  //release
  Keyboard.releaseAll();
}  //--------------------------------------------------------------------------------
void k_F5PressCallback(void *ptr) {  //press
  Keyboard.press(KEY_F5);
  GlobalLastkeyPressed = millis();
}
void k_F5ReleaseCallback(void *ptr) {  //release
  Keyboard.releaseAll();
}
//--------------------------------------------------------------------------------
void k_F6PressCallback(void *ptr) {  //press
  Keyboard.press(KEY_F6);
  GlobalLastkeyPressed = millis();
}
void k_F6ReleaseCallback(void *ptr) {  //release
  Keyboard.releaseAll();
}
//--------------------------------------------------------------------------------
void k_F7PressCallback(void *ptr) {  //press
  Keyboard.press(KEY_F7);
  GlobalLastkeyPressed = millis();
}
void k_F7ReleaseCallback(void *ptr) {  //release
  Keyboard.releaseAll();
}
//--------------------------------------------------------------------------------
void k_F8PressCallback(void *ptr) {  //press
  Keyboard.press(KEY_F8);
  GlobalLastkeyPressed = millis();
}
void k_F8ReleaseCallback(void *ptr) {  //release
  Keyboard.releaseAll();
}
//--------------------------------------------------------------------------------
void k_F9PressCallback(void *ptr) {  //press
  Keyboard.press(KEY_F9);
  GlobalLastkeyPressed = millis();
}
void k_F9ReleaseCallback(void *ptr) {  //release
  Keyboard.releaseAll();
}
//--------------------------------------------------------------------------------
void k_F10PressCallback(void *ptr) {  //press
  Keyboard.press(KEY_F10);
  GlobalLastkeyPressed = millis();
}
void k_F10ReleaseCallback(void *ptr) {  //release
  Keyboard.releaseAll();
}

//--------------------------------------------------------------------------------
void k_F11PressCallback(void *ptr) {  //press
  Keyboard.press(KEY_F11);
  GlobalLastkeyPressed = millis();
}
void k_F11ReleaseCallback(void *ptr) {  //release
  Keyboard.releaseAll();
}
//--------------------------------------------------------------------------------
void k_F12PressCallback(void *ptr) {  //press
  Keyboard.press(KEY_F12);
  GlobalLastkeyPressed = millis();
}
void k_F12ReleaseCallback(void *ptr) {  //release
  Keyboard.releaseAll();
}
//--------------------------------------------------------------------------------
void k_LABPressCallback(void *ptr) {  //press
  Keyboard.press(KEY_LEFT_SHIFT);
  delay(10);
  Keyboard.press(KEY_F2);
  GlobalLastkeyPressed = millis();
}
void k_LABReleaseCallback(void *ptr) {  //release
  Keyboard.releaseAll();
}
//--------------------------------------------------------------------------------
void k_ATCPressCallback(void *ptr) {  //press
  Keyboard.press('\\');
  GlobalLastkeyPressed = millis();
}
void k_ATCReleaseCallback(void *ptr) {  //release
  Keyboard.releaseAll();
}
//--------------------------------------------------------------------------------
void k_SALPressCallback(void *ptr) {  //press
  Keyboard.press(KEY_LEFT_CTRL);      //SALUTE
  delay(10);
  Keyboard.press(KEY_LEFT_SHIFT);
  Keyboard.press('S');
  GlobalLastkeyPressed = millis();
}
void k_SALReleaseCallback(void *ptr) {  //release
  Keyboard.releaseAll();
}
//--------------------------------------------------------------------------------
void k_CPTPressCallback(void *ptr) {  //press
  Keyboard.press(KEY_LEFT_ALT);
  delay(10);
  Keyboard.press(KEY_F1);
  GlobalLastkeyPressed = millis();
}
void k_CPTReleaseCallback(void *ptr) {  //release
  Keyboard.releaseAll();
}
//--------------------------------------------------------------------------------
void k_SVPressCallback(void *ptr) {  //press
  Keyboard.press(KEY_F5);            // -----------------------BM TO CHECK
  GlobalLastkeyPressed = millis();
}
void k_SVReleaseCallback(void *ptr) {  //release
  Keyboard.releaseAll();
}
//--------------------------------------------------------------------------------
void k_WARPPressCallback(void *ptr) {  //press
  Keyboard.press(KEY_LEFT_SHIFT);
  delay(10);
  Keyboard.press(KEY_F1);
  GlobalLastkeyPressed = millis();
}
void k_WARPReleaseCallback(void *ptr) {  //release
  Keyboard.releaseAll();
}
//--------------------------------------------------------------------------------
void k_PAUSEPressCallback(void *ptr) {  //press
  Keyboard.press(0x80);
  GlobalLastkeyPressed = millis();
}
void k_PAUSEReleaseCallback(void *ptr) {  //release
  Keyboard.releaseAll();
}
//--------------------------------------------------------------------------------
void k_ESCPressCallback(void *ptr) {  //press
  Keyboard.press(KEY_ESC);
  GlobalLastkeyPressed = millis();
}
void k_ESCReleaseCallback(void *ptr) {  //release
  Keyboard.releaseAll();
}

// &c_TAB, &c_ENT,  &c_AE,  &c_WIN,


//--------------------------------------------------------------------------------
void c_VUPPressCallback(void *ptr) {  //press
  VolUp = HIGH;
}
void c_VUPReleaseCallback(void *ptr) {  //release
  VolUp = LOW;
  Keyboard.releaseAll();
}
//--------------------------------------------------------------------------------
void c_VDNPressCallback(void *ptr) {  //press
  VolDn = HIGH;
}
void c_VDNReleaseCallback(void *ptr) {  //release
  VolDn = LOW;
  Keyboard.releaseAll();
}
//--------------------------------------------------------------------------------
void c_VMTPressCallback(void *ptr) {  //press
  Keyboard.press(0x7F);
  GlobalLastkeyPressed = millis();
}
void c_VMTReleaseCallback(void *ptr) {  //release
  Keyboard.releaseAll();
}
//--------------------------------------------------------------------------------
void c_TABPressCallback(void *ptr) {  //press
  Keyboard.press(179);
  GlobalLastkeyPressed = millis();
}
void c_TABReleaseCallback(void *ptr) {  //release
  Keyboard.releaseAll();
}
//--------------------------------------------------------------------------------
void c_ENTPressCallback(void *ptr) {  //press
  Keyboard.press(176);
  GlobalLastkeyPressed = millis();
}
void c_ENTReleaseCallback(void *ptr) {  //release
  Keyboard.releaseAll();
}
//--------------------------------------------------------------------------------
void c_AEPressCallback(void *ptr) {  //press
  Keyboard.press(134);
  GlobalLastkeyPressed = millis();
}
void c_AEReleaseCallback(void *ptr) {  //release
  Keyboard.releaseAll();
}
//--------------------------------------------------------------------------------
void c_WINPressCallback(void *ptr) {  //press
  Keyboard.press(KEY_LEFT_GUI);
  GlobalLastkeyPressed = millis();
}
void c_WINReleaseCallback(void *ptr) {  //release
  Keyboard.releaseAll();
}
//--------------------------------------------------------------------------------
void c_WARPPressCallback(void *ptr) {  //press
  Keyboard.press(KEY_LEFT_SHIFT);
  delay(10);
  Keyboard.press(KEY_F1);
  GlobalLastkeyPressed = millis();
}
void c_WARPReleaseCallback(void *ptr) {  //release
  Keyboard.releaseAll();
}
//--------------------------------------------------------------------------------




//--------------------------------------------------------------------------------
void v_INPressCallback(void *ptr) {  //press 221 '\335' Keypad *
  Keyboard.press(KEYPAD_MULTIPLY);
  GlobalLastkeyPressed = millis();
}
void v_INReleaseCallback(void *ptr) {  //release
  Keyboard.releaseAll();
}
//--------------------------------------------------------------------------------
void v_OUTPressCallback(void *ptr) {  //press 220 '\334' Keypad /
  Keyboard.press(KEYPAD_DIVIDE);
  GlobalLastkeyPressed = millis();
}
void v_OUTReleaseCallback(void *ptr) {  //release
  Keyboard.releaseAll();
}
//--------------------------------------------------------------------------------
void v_UPPressCallback(void *ptr) {  //press keypad 8 and Up Arrow 232
  Keyboard.press(KEYPAD_8);
  GlobalLastkeyPressed = millis();
}
void v_UPReleaseCallback(void *ptr) {  //release
  Keyboard.releaseAll();
}
//--------------------------------------------------------------------------------
void v_LFPressCallback(void *ptr) {  //press Keypad 4 and Left Arrow
  Keyboard.press(KEYPAD_4);
  GlobalLastkeyPressed = millis();
}
void v_LFReleaseCallback(void *ptr) {  //release
  Keyboard.releaseAll();
}
//--------------------------------------------------------------------------------
void v_RTPressCallback(void *ptr) {  //press Keypad 6 and Right Arrow
  Keyboard.press(KEYPAD_6);
  GlobalLastkeyPressed = millis();
}
void v_RTReleaseCallback(void *ptr) {  //release
  Keyboard.releaseAll();
}
//--------------------------------------------------------------------------------
void v_CNPressCallback(void *ptr) {  //press Keypad 5
  Keyboard.press(KEYPAD_5);
  GlobalLastkeyPressed = millis();
}
void v_CNReleaseCallback(void *ptr) {  //release
  Keyboard.releaseAll();
}
//--------------------------------------------------------------------------------
void v_DNPressCallback(void *ptr) {  //press Keypad 2 and Down Arrow 226
  Keyboard.press(KEYPAD_2);
  GlobalLastkeyPressed = millis();
}
void v_DNReleaseCallback(void *ptr) {  //release
  Keyboard.releaseAll();
}
//--------------------------------------------------------------------------------
void v_RSPressCallback(void *ptr) {  //press Keypad 6 and Right Arrow 231
  Keyboard.press(KEYPAD_5);
  GlobalLastkeyPressed = millis();
}
void v_RSReleaseCallback(void *ptr) {  //release
  Keyboard.releaseAll();
}
//--------------------------------------------------------------------------------
void v_TLPressCallback(void *ptr) {  //press Keypad 7 and Home 231
  Keyboard.press(KEYPAD_7);
  GlobalLastkeyPressed = millis();
}
void v_TLReleaseCallback(void *ptr) {  //release
  Keyboard.releaseAll();
}
//--------------------------------------------------------------------------------
void v_TRPressCallback(void *ptr) {  //press Keypad 9 and PageUp 233
  Keyboard.press(KEYPAD_9);
  GlobalLastkeyPressed = millis();
}
void v_TRReleaseCallback(void *ptr) {  //release
  Keyboard.releaseAll();
}
//--------------------------------------------------------------------------------
void v_BLPressCallback(void *ptr) {  //press Keypad 1 and End 226 '\342'
  Keyboard.press(KEYPAD_1);
  GlobalLastkeyPressed = millis();
}
void v_BLReleaseCallback(void *ptr) {  //release
  Keyboard.releaseAll();
}
//--------------------------------------------------------------------------------
void v_BRPressCallback(void *ptr) {  //press
  Keyboard.press(KEYPAD_3);
  GlobalLastkeyPressed = millis();
}
void v_BRReleaseCallback(void *ptr) {  //release
  Keyboard.releaseAll();
}
//--------------------------------------------------------------------------------
void v_WARPPressCallback(void *ptr) {  //press
  Keyboard.press(KEY_LEFT_SHIFT);
  delay(10);
  Keyboard.press(KEY_F1);
  GlobalLastkeyPressed = millis();
}
void v_WARPReleaseCallback(void *ptr) {  //release
  Keyboard.releaseAll();
}
//--------------------------------------------------------------------------------

//----------------------------SHUTDOWN CODE------------------------------------------
void p_PCOFFPressCallback(void *ptr) {  //press

  //WARNING - THIS IS A PC SHUTDOWN CODE

  Keyboard.press(KEY_LEFT_GUI);
  delay(10);
  Keyboard.press(KEY_X);
  Keyboard.releaseAll();
  delay(20);
  Keyboard.press(KEY_U);
  delay(10);
  Keyboard.releaseAll();
  delay(10);
  Keyboard.press(KEY_U);
  delay(10);
  Keyboard.releaseAll();

  GlobalLastkeyPressed = millis();
}
void p_PCOFFReleaseCallback(void *ptr) {  //release
  Keyboard.releaseAll();
}

//WARNING - THIS IS A PC SHUTDOWN CODE
//----------------------------SHUTDOWN CODE------------------------------------------


void setup() {

  if (Serial_In_Use == 1) {
    Serial.begin(115200);
    Serial.println("Serial Started");
  }

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
      digitalWrite(LED_BUILTIN, false);
      delay(FLASH_TIME);
      digitalWrite(LED_BUILTIN, true);
    }


    SendDebug("Ethernet Started " + strMyIP + " " + sMac);
  }

  Keyboard.begin();
  Consumer.begin();  //initialize computer connection
  delay(250);        //wait for computer to connect





  for (int a = 0; a < 52; a++) {
    Consumer.write(MEDIA_VOLUME_UP);  //set the volume to 0
    delay(2);
  }

  NEXTION.begin(9600);

  nexInit();  //  Nextion Display initalize

  k_F1.attachPush(k_F1PressCallback, &k_F1);
  k_F1.attachPop(k_F1ReleaseCallback, &k_F1);

  k_F2.attachPush(k_F2PressCallback, &k_F2);
  k_F2.attachPop(k_F2ReleaseCallback, &k_F2);

  k_F3.attachPush(k_F3PressCallback, &k_F3);
  k_F3.attachPop(k_F3ReleaseCallback, &k_F3);

  k_F4.attachPush(k_F4PressCallback, &k_F4);
  k_F4.attachPop(k_F4ReleaseCallback, &k_F4);

  k_F5.attachPush(k_F5PressCallback, &k_F5);
  k_F5.attachPop(k_F5ReleaseCallback, &k_F5);

  k_F6.attachPush(k_F6PressCallback, &k_F6);
  k_F6.attachPop(k_F6ReleaseCallback, &k_F6);

  k_F7.attachPush(k_F7PressCallback, &k_F7);
  k_F7.attachPop(k_F7ReleaseCallback, &k_F7);

  k_F8.attachPush(k_F8PressCallback, &k_F8);
  k_F8.attachPop(k_F8ReleaseCallback, &k_F8);

  k_F9.attachPush(k_F9PressCallback, &k_F9);
  k_F9.attachPop(k_F9ReleaseCallback, &k_F9);

  k_F10.attachPush(k_F10PressCallback, &k_F10);
  k_F10.attachPop(k_F10ReleaseCallback, &k_F10);

  k_F11.attachPush(k_F11PressCallback, &k_F11);
  k_F11.attachPop(k_F11ReleaseCallback, &k_F11);

  k_F12.attachPush(k_F12PressCallback, &k_F12);
  k_F12.attachPop(k_F12ReleaseCallback, &k_F12);

  c_TAB.attachPush(c_TABPressCallback, &c_TAB);
  c_TAB.attachPop(c_TABReleaseCallback, &c_TAB);

  c_ENT.attachPush(c_ENTPressCallback, &c_ENT);
  c_ENT.attachPop(c_ENTReleaseCallback, &c_ENT);

  c_AE.attachPush(c_AEPressCallback, &c_AE);
  c_AE.attachPop(c_AEReleaseCallback, &c_AE);

  c_WIN.attachPush(c_WINPressCallback, &c_WIN);
  c_WIN.attachPop(c_WINReleaseCallback, &c_WIN);

  v_IN.attachPush(v_INPressCallback, &v_IN);
  v_IN.attachPop(v_INReleaseCallback, &v_IN);

  v_OUT.attachPush(v_OUTPressCallback, &v_OUT);
  v_OUT.attachPop(v_OUTReleaseCallback, &v_OUT);

  v_UP.attachPush(v_UPPressCallback, &v_UP);
  v_UP.attachPop(v_UPReleaseCallback, &v_UP);

  v_LF.attachPush(v_LFPressCallback, &v_LF);
  v_LF.attachPop(v_LFReleaseCallback, &v_LF);

  v_RT.attachPush(v_RTPressCallback, &v_RT);
  v_RT.attachPop(v_RTReleaseCallback, &v_RT);

  v_CN.attachPush(v_CNPressCallback, &v_CN);
  v_CN.attachPop(v_CNReleaseCallback, &v_CN);

  v_DN.attachPush(v_DNPressCallback, &v_DN);
  v_DN.attachPop(v_DNReleaseCallback, &v_DN);

  v_RS.attachPush(v_RSPressCallback, &v_RS);
  v_RS.attachPop(v_RSReleaseCallback, &v_RS);

  v_TL.attachPush(v_TLPressCallback, &v_TL);
  v_TL.attachPop(v_TLReleaseCallback, &v_TL);

  v_TR.attachPush(v_TRPressCallback, &v_TR);
  v_TR.attachPop(v_TRReleaseCallback, &v_TR);

  v_BL.attachPush(v_BLPressCallback, &v_BL);
  v_BL.attachPop(v_BLReleaseCallback, &v_BL);

  v_BR.attachPush(v_BRPressCallback, &v_BR);
  v_BR.attachPop(v_BRReleaseCallback, &v_BR);

  p_PCOFF.attachPush(p_PCOFFPressCallback, &p_PCOFF);
  p_PCOFF.attachPop(p_PCOFFReleaseCallback, &p_PCOFF);

  Keyboard.releaseAll();  // INCASE A LOCKED ON KEY

  //  pinMode(selPin, INPUT);  // set button select pin as input
  // digitalWrite(selPin, HIGH);  // Pull button select pin high
  // delay(5);  // short delay to let outputs settle

  Serial1.print("dim=10");  // SET ON TO 100% BRIGHT
  Serial1.write("\xFF\xFF\xFF");
  delay(500);
}


void loop() {
  char key = keypad.getKey();
  if (key != 0) {
    SendDebug("Keypressed");
    pressMappedKey(key);
  }


  valVol = analogRead(VolPot);            //read potentiometer value
  valVol = map(valVol, 0, 1023, 0, 101);  //map it to 102 steps
  if (REVERSED) {
    valVol = 101 - valVol;
  }
  if (abs(valVol - previousvalVol) > 1) {  //check if potentiometer valVolue has changed
    previousvalVol = valVol;
    valVol /= 2;  //divide it by 2 to get 51 steps
    while (valVol2 < valVol) {
      Consumer.write(MEDIA_VOLUME_UP);  //turn volume up to appropiate level
      valVol2++;
      delay(2);
    }
    while (valVol2 > valVol) {
      Consumer.write(MEDIA_VOLUME_DOWN);  //turn volume down to appropiate level
      valVol2--;
      delay(2);
    }
  }
  {
    int brightness = analogRead(DIMPot);
    //       int brightness = 1023;
    int bright = map(brightness, 0, 1023, 100, 10);
    String dim = "dim=" + String(bright);
    brightness = bright;
    Serial1.print(dim.c_str());
    Serial1.write("\xFF\xFF\xFF");
  }

  nexLoop(nex_listen_list);  // Check for any touch event and run the associated function
}
