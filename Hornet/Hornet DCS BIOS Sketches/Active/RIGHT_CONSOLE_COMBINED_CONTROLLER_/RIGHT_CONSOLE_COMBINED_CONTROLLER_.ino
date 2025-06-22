

////////////////////---||||||||||********||||||||||---\\\\\\\\\\\\\\\\\\\\
//||               FUNCTION = HORNET LED OUTPUT MAX 7219              ||\\
//||              LOCATION IN THE PIT = LIP RIGHTHAND SIDE            ||\\
//||            ARDUINO PROCESSOR TYPE = Arduino Mega 2560            ||\\
//||      ARDUINO CHIP SERIAL NUMBER = SN - 75834353230351013181     ||\\
//||                    CONNECTED COM PORT = COM 4                    ||\\
//||               ****ADD ASSIGNED COM PORT NUMBER****               ||\\
//||            ****DO CHECK S/N BEFORE UPLOAD NEW DATA****           ||\\
////////////////////---||||||||||********||||||||||---\\\\\\\\\\\\\\\\\\\\

/*
TO DO  
- Add Scotts Mag switch support code for LTD
- Suit temp and Cabin Temp not moving A6 and A7
- Bleed Air R-off is not registering a number on scan - work around applied using open on 117 and 105
*/
/*

  This Superceedes udp_input_controller
  Split from udp_input_controller_2 20200802

  Heavily based on
  https://github.com/calltherain/ArduinoUSBJoystick

  Interface for DCS BIOS

  Mega2560 R3,
  digital Pin 22~37 used as rows. 0-15, 16 Rows
  digital pin 38~48 used as columns. 0-10, 11 Columns

  20250518 For interim Controller v2 
  Digital Pins 22~34 as rows, 0-12
  Digital Pins 35~45 as columns, 0-10

  it's a 16 * 11  matrix, due to the loss of pins/Columns used by the Ethernet and SD Card Shield, Digital I/O 50 through 53 are not available.
  Pin 49 is available but isn't used. This gives a total number of usable Inputs as 176 (remember numbering starts at 0 - so 0-175)

  The code pulls down a row and reads values from the Columns.
  Row 0 - Col 0 = Digit 0
  Row 0 - Col 10 = Digit 10
  Row 15 - Col 0 = Digit 165
  Row 15 = Col 10 = Digit 175

  So - Digit = Row * 11 + Col

*/



int Ethernet_In_Use = 1;  // Check to see if jumper is present - if it is disable Ethernet calls. Used for Testing
int Reflector_In_Use = 1;
#define DCSBIOS_In_Use 1
#define MSFS_In_Use 0  // Used to interface into MSFS - set to 0 if not in use

// Used to Distinguish between the left, front, and right inputs
// Left=0, Front=1, Right=2
#define INPUT_MODULE_NUMBER 2

#define DCSBIOS_IRQ_SERIAL
#include <DcsBios.h>

// ********************************* Begin Ethernet ***************************************************
// Ethernet Related
#include <SPI.h>
#include <Ethernet.h>
#include <EthernetUdp.h>

#define EthernetStartupDelay 500
#define ES1_RESET_PIN 53

const unsigned long delayBeforeSendingPacket = 3000;
unsigned long ethernetStartTime = 0;
String BoardName = "Hornet Right Combined: ";

// These local Mac and IP Address will be reassigned early in startup based on
// the device ID as set by address pins
byte mac[] = { 0xA8, 0x61, 0x0A, 0x9E, 0x83, 0x03 };
String sMac = "A8:61:0A:67:83:03";
IPAddress ip(172, 16, 1, 103);
String strMyIP = "172.16.1.103";

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
  MessageToSend = BoardName + ": " + MessageToSend;
  if ((Reflector_In_Use == 1) && (Ethernet_In_Use == 1)) {
    udp.beginPacket(reflectorIP, reflectorport);
    udp.print(MessageToSend);
    udp.endPacket();
  }
}

// ********************************* End Ethernet ***************************************************


// ********************************* Begin Digital ***************************************************

#define HOOK_LED 4

#define BLEED_AIR_SOL_PORT A12
#define PITOT_HEAT_PORT A13
#define LASER_ARM_PORT A14
#define CANOPY_MAG_PORT A15

// ********************************* End Digital ***************************************************

// ################################### BEGIN LIGHTING ##################################


#define BACKLIGHT_PWM 6

void onConsoleIntLtChange(unsigned int newValue) {
  SendDebug("Console Lights : " + String(newValue));
  analogWrite(BACKLIGHT_PWM, map(newValue, 0, 65535, 0, 255));
}
DcsBios::IntegerBuffer consoleIntLtBuffer(0x7558, 0xffff, 0, onConsoleIntLtChange);

void setConsoleLights(unsigned int newValue) {
  SendDebug("Console Lights : " + String(newValue));
  analogWrite(BACKLIGHT_PWM, map(newValue, 0, 65535, 0, 255));
}

void turnOffAllBacklights() {
  digitalWrite(BACKLIGHT_PWM, LOW);
}

void turnOnAllBacklights() {
  digitalWrite(BACKLIGHT_PWM, HIGH);
}

// ################################### END LIGHTING ##################################


// ######################## BEGIN MAX7219 ########################
#include "LedControl.h"
LedControl lc = LedControl(A11, A10, A9, 1);

void allMax7219On() {
  for (int displayunit = 0; displayunit < 2; displayunit++) {
    for (int row = 0; row < 8; row++) {
      for (int col = 0; col < 8; col++) {
        if (col != 9 && col != 9 && col != 9)
          lc.setLed(displayunit, row, col, true);
        SendDebug(String(displayunit) + ":" + String(row) + ":" + String(col));
      }
    }
  }
}

void debugAllMax7219On() {
  for (int displayunit = 0; displayunit < 2; displayunit++) {
    for (int row = 0; row < 9; row++) {
      for (int col = 0; col < 9; col++) {
        if (col != 9 && col != 9 && col != 9) {
          allMax7219Off();
          SendDebug(String(row) + ":" + String(col));
          lc.setLed(displayunit, row, col, true);
          delay(10000);
        }
      }
    }
  }
}

void allMax7219Off() {
  for (int displayunit = 0; displayunit < 2; displayunit++) {
    for (int row = 0; row < 8; row++) {
      for (int col = 0; col < 8; col++) {
        if (col != 9 && col != 9 && col != 9)
          lc.setLed(displayunit, row, col, false);
      }
    }
  }
}

// ######################## BEGIN MAX7219 ########################

// ######################## BEGIN CAUTION PANEL ##################

void setAllCautionLed(bool newValue) {
  setClipApuAccLt(newValue);
  setClipBattSwLt(newValue);
  setClipCkSeatLt(newValue);
  setClipFcesLt(newValue);
  setClipFcsHotLt(newValue);
  setClipFuelLoLt(newValue);
  setClipGenTieLt(newValue);
  setClipLGenLt(newValue);
  setClipRGenLt(newValue);
  setClipSpareCtn1Lt(newValue);
  setClipSpareCtn2Lt(newValue);
  setClipSpareCtn3Lt(newValue);
}


void setClipApuAccLt(bool newValue) {
  lc.setLed(0, 0, 2, newValue);
  lc.setLed(0, 0, 3, newValue);
}
void onClipApuAccLtChange(unsigned int newValue) {
  if (newValue == 1)
    setClipApuAccLt(true);
  else
    setClipApuAccLt(false);
}
DcsBios::IntegerBuffer clipApuAccLtBuffer(FA_18C_hornet_CLIP_APU_ACC_LT, onClipApuAccLtChange);

void setClipBattSwLt(bool newValue) {
  lc.setLed(0, 0, 4, newValue);
  lc.setLed(0, 0, 5, newValue);
}
void onClipBattSwLtChange(unsigned int newValue) {
  if (newValue == 1)
    setClipBattSwLt(true);
  else
    setClipBattSwLt(false);
}
DcsBios::IntegerBuffer clipBattSwLtBuffer(FA_18C_hornet_CLIP_BATT_SW_LT, onClipBattSwLtChange);

void setClipCkSeatLt(bool newValue) {
  lc.setLed(0, 0, 0, newValue);
  lc.setLed(0, 0, 1, newValue);
}
void onClipCkSeatLtChange(unsigned int newValue) {
  if (newValue == 1)
    setClipCkSeatLt(true);
  else
    setClipCkSeatLt(false);
}
DcsBios::IntegerBuffer clipCkSeatLtBuffer(FA_18C_hornet_CLIP_CK_SEAT_LT, onClipCkSeatLtChange);

void setClipFcesLt(bool newValue) {
  lc.setLed(0, 2, 2, newValue);
  lc.setLed(0, 2, 3, newValue);
}
void onClipFcesLtChange(unsigned int newValue) {
  if (newValue == 1)
    setClipFcesLt(true);
  else
    setClipFcesLt(false);
}
DcsBios::IntegerBuffer clipFcesLtBuffer(FA_18C_hornet_CLIP_FCES_LT, onClipFcesLtChange);

void setClipFcsHotLt(bool newValue) {
  lc.setLed(0, 1, 0, newValue);
  lc.setLed(0, 1, 1, newValue);
}
void onClipFcsHotLtChange(unsigned int newValue) {
  if (newValue == 1)
    setClipFcsHotLt(true);
  else
    setClipFcsHotLt(false);
}
DcsBios::IntegerBuffer clipFcsHotLtBuffer(FA_18C_hornet_CLIP_FCS_HOT_LT, onClipFcsHotLtChange);

void setClipFuelLoLt(bool newValue) {
  lc.setLed(0, 2, 0, newValue);
  lc.setLed(0, 2, 1, newValue);
}
void onClipFuelLoLtChange(unsigned int newValue) {
  if (newValue == 1)
    setClipFuelLoLt(true);
  else
    setClipFuelLoLt(false);
}
DcsBios::IntegerBuffer clipFuelLoLtBuffer(FA_18C_hornet_CLIP_FUEL_LO_LT, onClipFuelLoLtChange);

void setClipGenTieLt(bool newValue) {
  lc.setLed(0, 1, 2, newValue);
  lc.setLed(0, 1, 3, newValue);
}
void onClipGenTieLtChange(unsigned int newValue) {
  if (newValue == 1)
    setClipGenTieLt(true);
  else
    setClipGenTieLt(false);
}
DcsBios::IntegerBuffer clipGenTieLtBuffer(FA_18C_hornet_CLIP_GEN_TIE_LT, onClipGenTieLtChange);

void setClipLGenLt(bool newValue) {
  lc.setLed(0, 3, 0, newValue);
  lc.setLed(0, 3, 1, newValue);
}
void onClipLGenLtChange(unsigned int newValue) {
  if (newValue == 1)
    setClipLGenLt(true);
  else
    setClipLGenLt(false);
}
DcsBios::IntegerBuffer clipLGenLtBuffer(FA_18C_hornet_CLIP_L_GEN_LT, onClipLGenLtChange);

void setClipRGenLt(bool newValue) {
  lc.setLed(0, 3, 2, newValue);
  lc.setLed(0, 3, 3, newValue);
}
void onClipRGenLtChange(unsigned int newValue) {
  if (newValue == 1)
    setClipRGenLt(true);
  else
    setClipRGenLt(false);
}
DcsBios::IntegerBuffer clipRGenLtBuffer(FA_18C_hornet_CLIP_R_GEN_LT, onClipRGenLtChange);

void setClipSpareCtn1Lt(bool newValue) {
  lc.setLed(0, 1, 4, newValue);
  lc.setLed(0, 1, 5, newValue);
}
void onClipSpareCtn1LtChange(unsigned int newValue) {
  if (newValue == 1)
    setClipSpareCtn1Lt(true);
  else
    setClipSpareCtn1Lt(false);
}
DcsBios::IntegerBuffer clipSpareCtn1LtBuffer(FA_18C_hornet_CLIP_SPARE_CTN1_LT, onClipSpareCtn1LtChange);

void setClipSpareCtn2Lt(bool newValue) {
  lc.setLed(0, 2, 4, newValue);
  lc.setLed(0, 2, 5, newValue);
}
void onClipSpareCtn2LtChange(unsigned int newValue) {
  if (newValue == 1)
    setClipSpareCtn2Lt(true);
  else
    setClipSpareCtn2Lt(false);
}
DcsBios::IntegerBuffer clipSpareCtn2LtBuffer(FA_18C_hornet_CLIP_SPARE_CTN2_LT, onClipSpareCtn2LtChange);

void setClipSpareCtn3Lt(bool newValue) {
  lc.setLed(0, 3, 4, newValue);
  lc.setLed(0, 3, 5, newValue);
}

void onClipSpareCtn3LtChange(unsigned int newValue) {
  if (newValue == 1)
    setClipSpareCtn3Lt(true);
  else
    setClipSpareCtn3Lt(false);
}
DcsBios::IntegerBuffer clipSpareCtn3LtBuffer(FA_18C_hornet_CLIP_SPARE_CTN3_LT, onClipSpareCtn3LtChange);

// ######################## END CAUTION PANEL ####################

#define NUM_BUTTONS 256
#define BUTTONS_USED_ON_PCB 176
#define NUM_AXES 8  // 8 axes, X, Y, Z, etc
#define GREEN_STATUS_LED_PORT 14
#define RED_STATUS_LED_PORT 15  // RED LED is used for monitoring ethernet
#define FLASH_TIME 300

unsigned long NEXT_STATUS_TOGGLE_TIMER = 0;
bool RED_LED_STATE = false;
bool GREEN_LED_STATE = true;
unsigned long timeSinceRedLedChanged = 0;
//
struct joyReport_t {
  int button[NUM_BUTTONS];  // 1 Button per byte - was originally one bit per byte - but we have plenty of storage space
};



// Go through the man loop a number of times before sending data to the Sim
// This allows analog averages to be established and the DigitalButton array to be populated
// This has to more than simply the number of elements in the array as the array values are initialised
// to 0, so it actually takes more than 30 interations before the average it fully established
// Could enhance using weighted average
int LoopsBeforeSendingAllowed = 40;
bool SendingAllowed = false;


// Debounce delay was 20mS - but encountered longer bounces with Circuit Breakers, increased to 60mS 20210329
const int ScanDelay = 80;      // This is in microseconds
const int DebounceDelay = 60;  // In milliseconds

joyReport_t joyReport;
joyReport_t prevjoyReport;


unsigned long joyEndDebounce[NUM_BUTTONS];  // Holds the time we'll look at any more changes in a given input

long prevLEDTransition = millis();
int cButtonID[16];
bool bFirstTime = false;


unsigned long currentMillis = 0;
unsigned long previousMillis = 0;



// Note Pin 4 and Pin 10 cannot be used for other purposes.
//Arduino communicates with both the W5500 and SD card using the SPI bus (through the ICSP header).
//This is on digital pins 10, 11, 12, and 13 on the Uno and pins 50, 51, and 52 on the Mega.
//On both boards, pin 10 is used to select the W5500 and pin 4 for the SD card. These pins cannot be used for general I/O.
//On the Mega, the hardware SS pin, 53, is not used to select either the W5500 or the SD card,
//but it must be kept as an output or the SPI interface won't work.


char stringind[5];
String outString;


bool RadarFollowupTask = false;
long TimeRadarOn = 0;
const int RadarMoveTime = 300;

bool RadarPushFollowupTask = false;
long TimeRadarOff = 0;



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
      digitalWrite(GREEN_STATUS_LED_PORT, false);
      delay(FLASH_TIME);
      digitalWrite(GREEN_STATUS_LED_PORT, true);
    }


    SendDebug("Ethernet Started " + strMyIP + " " + sMac);
  }



  // Begin Original Code
  // Set the output ports to output
  for (int portId = 22; portId < 38; portId++) {
    pinMode(portId, OUTPUT);
  }
  // Set the input ports to input - port 50-53 used by Ethernet Shield
  for (int portId = 38; portId < 50; portId++) {
    // Even though we've already got 10K external pullups
    pinMode(portId, INPUT_PULLUP);
  }
  // End Original Code




  // Initialise all arrays
  for (int ind = 0; ind < NUM_BUTTONS; ind++) {

    // Clear current and last values to 0 for button inputs
    joyReport.button[ind] = 0;
    prevjoyReport.button[ind] = 0;

    // Set the end
    joyEndDebounce[ind] = 0;
  }


  pinMode(HOOK_LED, OUTPUT);
  pinMode(BACKLIGHT_PWM, OUTPUT);

  digitalWrite(HOOK_LED, true);

  turnOnAllBacklights();

  /*The MAX72XX is in power-saving mode on startup*/
  lc.shutdown(0, false);
  /* Set the brightness to a medium values */
  lc.setIntensity(0, 15);

  /* and clear the display */
  lc.clearDisplay(0);

  setAllCautionLed(true);

  pinMode(BLEED_AIR_SOL_PORT, OUTPUT);
  digitalWrite(BLEED_AIR_SOL_PORT, false);
  pinMode(PITOT_HEAT_PORT, OUTPUT);
  digitalWrite(PITOT_HEAT_PORT, false);
  pinMode(LASER_ARM_PORT, OUTPUT);
  digitalWrite(LASER_ARM_PORT, false);
  pinMode(CANOPY_MAG_PORT, OUTPUT);
  digitalWrite(CANOPY_MAG_PORT, false);


  if (DCSBIOS_In_Use == 1) DcsBios::setup();

  // Synchronise end of setup scripts across the pit so panel lights dim togehter
  // while (millis() <= 90000) {
  while (millis() <= 0) {
    delay(FLASH_TIME);
    digitalWrite(GREEN_STATUS_LED_PORT, false);
    delay(FLASH_TIME);
    digitalWrite(GREEN_STATUS_LED_PORT, true);
  }



  turnOffAllBacklights();

  // Set Console lights to a mid level for start of game
  setConsoleLights(16000);
  setAllCautionLed(false);



  SendDebug("Setup Complete");
}


void FindInputChanges() {

  for (int ind = 0; ind < NUM_BUTTONS; ind++)
    if (bFirstTime) {

      bFirstTime = false;
      // Just Copy Array and perform no actions - this may change in the future
      prevjoyReport.button[ind] = joyReport.button[ind];
    } else {
      // Not the first time - see if there is a difference from last time
      // If there is perform action and update prev array BUT only if we past the end of the debounce period
      if (prevjoyReport.button[ind] != joyReport.button[ind] && millis() > joyEndDebounce[ind]) {

        // First things first - set a new debounce period
        joyEndDebounce[ind] = millis() + DebounceDelay;

        sprintf(stringind, "%03d", ind);


        if (prevjoyReport.button[ind] == 0) {
          outString = outString + "1";
          if (DCSBIOS_In_Use == 1) CreateDcsBiosMessage(ind, 1);
          if (MSFS_In_Use == 1) SendMSFSMessage(ind, 1);
          SendDebug(String(ind) + ":1");
        } else {
          outString = outString + "0";
          if (DCSBIOS_In_Use == 1) CreateDcsBiosMessage(ind, 0);
          if (MSFS_In_Use == 1) SendMSFSMessage(ind, 0);
          SendDebug(String(ind) + ":0");
        }


        prevjoyReport.button[ind] = joyReport.button[ind];
      }
    }
}



void SendMSFSMessage(int ind, int state) {

  String outString;
  outString = "D" + String(INPUT_MODULE_NUMBER) + "," + String(ind) + ":" + String(state);

  udp.beginPacket(MSFSIP, MSFSport);
  udp.print(outString);
  udp.endPacket();


  //  udp.beginPacket(targetIP, remoteport);
  //  udp.print(outString);
  //  udp.endPacket();
}

void SendIPString(String KeysToSend) {
  // Used to Send Desired Keystrokes to Due acting as Keyboard

  // Begin While testing controller v2 disabled sending character
  return;
  // End While testing controller v2 disabled sending character

  if (Ethernet_In_Use == 1) {
    udp.beginPacket(targetIP, keyboardport);
    udp.print(KeysToSend);
    udp.endPacket();
  }
}

void SendLedString(String LedCommandToSend) {

  if (Ethernet_In_Use == 1) {
    udp.beginPacket(targetIP, ledport);
    udp.print(LedCommandToSend);
    udp.endPacket();
  }
}




void sendToDcsBiosMessage(const char *msg, const char *arg) {


  SendDebug(String(msg) + ":" + String(arg));


  sendDcsBiosMessage(msg, arg);
}



void CreateDcsBiosMessage(int ind, int state) {


  switch (state) {

    // RELEASE
    case 0:
      switch (ind) {

        case 0:
          sendToDcsBiosMessage("FLIR_SW", "1");
          break;
        case 1:
          break;
        case 2:
          break;
        case 3:
          SendIPString("LCTRL LSHIFT P");
          break;
        case 4:
          // Needed to Toggle HUD only view off
          SendIPString("LALT F1");
          break;
        case 5:
          SendIPString("LSHIFT Z");
          break;
        // RELEASE
        case 6:
          break;
        case 7:
          break;
        case 8:
          break;
        case 9:
          sendToDcsBiosMessage("HOOK_LEVER", "1");
          break;
        case 10:
          sendToDcsBiosMessage("WING_FOLD_ROTATE", "1");
          break;
        // RELEASE
        case 11:
          sendToDcsBiosMessage("FLIR_SW", "1");
          break;
        case 12:
          break;
        case 13:
          break;
        case 14:
          SendIPString("ESC");
          break;
        case 15:
          break;
        case 16:
          break;
        case 17:
          break;
        case 18:
          break;
        case 19:
          break;
        // RELEASE
        case 20:
          sendToDcsBiosMessage("WING_FOLD_ROTATE", "1");
          break;
        case 21:
          sendToDcsBiosMessage("AV_COOL_SW", "1");
          break;
        case 22:
          sendToDcsBiosMessage("LTD_R_SW", "0");
          break;
        case 23:
          break;
        case 24:
          break;
        // RELEASE
        case 25:
          break;
        case 26:
          break;
        case 27:
          break;
        case 28:
          break;
        case 29:
          break;
        // RELEASE
        case 30:
          break;
        case 31:
          sendToDcsBiosMessage("WING_FOLD_PULL", "1");
          break;
        case 32:
          break;
        case 33:
          sendToDcsBiosMessage("LST_NFLR_SW", "0");
          break;
        case 34:
          sendToDcsBiosMessage("RADAR_SW_PULL", "1");
          TimeRadarOn = millis() + RadarMoveTime;
          RadarPushFollowupTask = true;
          break;
        // RELEASE
        case 35:
          break;
        case 36:
          break;
        case 37:
          break;
        case 38:
          break;
        case 39:
          break;
        // RELEASE
        case 40:
          break;
        case 41:
          break;
        case 42:
          sendToDcsBiosMessage("L_GEN_SW", "1");  //1
          break;
        case 43:
          sendToDcsBiosMessage("BATTERY_SW", "1");
          break;
        case 44:
          break;
        // RELEASE
        case 45:
          break;
        case 46:
          break;
        case 47:
          SendIPString("LWIN F1");
          break;
        case 48:
          break;
        case 49:
          break;
        // RELEASE
        case 50:
          break;
        case 51:
          break;
        case 52:
          break;
        case 53:
          sendToDcsBiosMessage("R_GEN_SW", "1");
          break;
        case 54:
          sendToDcsBiosMessage("BATTERY_SW", "1");
          break;
        // RELEASE
        case 55:
          break;
        case 56:
          break;
        case 57:
          break;
        case 58:
          SendIPString("LWIN F1");
          break;
        case 59:
          break;
        // RELEASE
        case 60:
          break;
        case 61:
          break;
        case 62:
          break;
        case 63:
          break;
        case 64:
          break;
        // RELEASE
        case 65:
          break;
        case 66:
          break;
        case 67:
          break;
        case 68:
          break;
        case 69:
          break;
        // RELEASE
        case 70:
          break;
        case 71:
          break;
        case 72:
          break;
        case 73:
          break;
        case 74:
          break;
        // RELEASE
        case 75:
          break;
        case 76:
          break;
        case 77:
          break;
        case 78:
          break;
        case 79:
          break;
        // RELEASE
        case 80:
          break;
        case 81:
          break;
        case 82:
          break;
        case 83:
          break;
        case 84:
          break;
        // RELEASE
        case 85:
          break;
        case 86:
          break;
        case 87:
          break;
        case 88:
          sendToDcsBiosMessage("LIGHTS_TEST_SW", "1");
          break;
        case 89:
          sendToDcsBiosMessage("COCKKPIT_LIGHT_MODE_SW", "1");
          break;
        // RELEASE
        case 90:
          sendToDcsBiosMessage("WSHIELD_ANTI_ICE_SW", "1");
          break;
        case 91:
          sendToDcsBiosMessage("ECS_MODE_SW", "1");
          break;
        case 92:
          sendToDcsBiosMessage("CABIN_PRESS_SW", "1");
          break;
        case 93:
          sendToDcsBiosMessage("PITOT_HEAT_SW", "0");
          break;
        case 94:
          break;
        // RELEASE
        case 95:
          sendToDcsBiosMessage("RADALT_TEST_SW", "0");
          break;
        case 96:
          break;
        case 97:
          break;
        case 98:
          break;
        case 99:
          break;
        // RELEASE
        case 100:
          sendToDcsBiosMessage("COCKKPIT_LIGHT_MODE_SW", "1");
          break;
        case 101:
          sendToDcsBiosMessage("WSHIELD_ANTI_ICE_SW", "1");
          break;
        case 102:
          sendToDcsBiosMessage("ECS_MODE_SW", "1");
          break;
        case 103:
          sendToDcsBiosMessage("CABIN_PRESS_SW", "1");
          break;
        case 104:
          break;
        // RELEASE
        case 105:
          sendToDcsBiosMessage("BLEED_AIR_KNOB", "3");
          break;
        case 106:
          break;
        case 107:
          break;
        case 108:
          break;
        case 109:
          break;
        // RELEASE
        case 110:
          sendToDcsBiosMessage("CANOPY_SW", "1");
          break;
        case 111:
          sendToDcsBiosMessage("CB_HOOOK", "1");
          break;
        case 112:
          break;
        case 113:
          sendToDcsBiosMessage("ENG_ANTIICE_SW", "1");
          break;
        case 114:
          break;
        // RELEASE
        case 115:
          sendToDcsBiosMessage("BLEED_AIR_PULL", "0");
          break;
        case 116:
          break;
        case 117:
          break;
        case 118:
          break;
        case 119:
          break;
        // RELEASE
        case 120:
          break;
        case 121:
          sendToDcsBiosMessage("CANOPY_SW", "1");
          break;
        case 122:
          sendToDcsBiosMessage("CB_FCS_CHAN4", "1");
          break;
        case 123:
          break;
        case 124:
          sendToDcsBiosMessage("ENG_ANTIICE_SW", "1");
          break;
        // RELEASE
        case 125:
          break;
        case 126:
          break;
        case 127:
          sendToDcsBiosMessage("BLEED_AIR_KNOB", "3");
          break;
        case 128:
          break;
        case 129:
          break;
        // RELEASE
        case 130:
          break;
        case 131:
          break;
        case 132:
          sendToDcsBiosMessage("CB_FCS_CHAN3", "1");
          break;
        case 133:
          sendToDcsBiosMessage("FCS_BIT_SW", "0");
          break;
        case 134:
          sendToDcsBiosMessage("CB_LG", "1");
          break;
        // RELEASE
        case 135:
          break;
        case 136:
          break;
        case 137:
          break;
        case 138:
          break;
        case 139:
          break;
        // RELEASE
        case 140:
          break;
        case 141:
          break;
        case 142:
          break;
        case 143:
          break;
        case 144:
          break;
        // RELEASE
        case 145:
          break;
        case 146:
          break;
        case 147:
          break;
        case 148:
          break;
        case 149:
          break;
        // RELEASE
        case 150:
          break;
        case 151:
          break;
        case 152:
          break;
        case 153:
          break;
        case 154:
          break;
        // RELEASE
        case 155:
          break;
        case 156:
          break;
        case 157:
          break;
        case 158:
          break;
        case 159:
          break;
        // RELEASE
        case 160:
          break;
        case 161:
          break;
        case 162:
          break;
        case 163:
          break;
        case 164:
          break;
        // RELEASE
        case 165:
          break;
        case 166:
          break;
        case 167:
          break;
        case 168:
          break;
        case 169:
          break;
        // RELEASE
        case 170:
          break;
        case 171:
          break;
        case 172:
          break;
        case 173:
          break;
        case 174:
          break;
        // RELEASE
        case 175:
          break;
        case 176:
          break;
        case 177:
          break;
        case 178:
          break;
        case 179:
          break;
        // RELEASE
        default:
          break;
      }
      break;



    case 1:

      // PRESS - CLOSE
      switch (ind) {
        case 0:
          sendToDcsBiosMessage("FLIR_SW", "0");
          break;
        case 1:
          sendToDcsBiosMessage("RADAR_SW", "0");
          break;
        case 2:
          sendToDcsBiosMessage("INS_SW", "0");  //OFF
          break;
        case 3:
          SendIPString("LCTRL LSHIFT P");
          break;
        case 4:
          SendIPString("LALT F1");
          break;
        // PRESS - CLOSE
        case 5:
          SendIPString("LCTRL Z");
          break;
        case 6:
          break;
        case 7:
          sendToDcsBiosMessage("KY58_MODE_SELECT", "0");
          break;
        case 8:
          sendToDcsBiosMessage("KY58_FILL_SELECT", "10");
          break;
        case 9:
          sendToDcsBiosMessage("HOOK_LEVER", "0");
          break;
        case 10:
          sendToDcsBiosMessage("WING_FOLD_ROTATE", "2");
          break;
        // PRESS - CLOSE
        case 11:
          sendToDcsBiosMessage("FLIR_SW", "2");
          break;
        case 12:
          sendToDcsBiosMessage("RADAR_SW", "1");
          break;
        case 13:
          sendToDcsBiosMessage("INS_SW", "1");  //CV
          break;
        case 14:
          SendIPString("ESC");
          break;
        // PRESS - CLOSE
        case 15:
          SendIPString("F10");
          break;
        case 16:
          break;
        case 17:
          break;
        case 18:
          sendToDcsBiosMessage("KY58_MODE_SELECT", "1");
          break;
        case 19:
          sendToDcsBiosMessage("KY58_FILL_SELECT", "0");
          break;
        // PRESS - CLOSE
        case 20:
          sendToDcsBiosMessage("WING_FOLD_ROTATE", "0");
          break;
        // PRESS - CLOSE
        case 21:
          sendToDcsBiosMessage("AV_COOL_SW", "0");
          break;
        case 22:
          // Special Case for Magnetic Switches LTD/R
          // if (Ethernet_In_Use == 1) {
          //   SendIPString("LCTRL LSHIFT F3");
          // } else {
          sendToDcsBiosMessage("LTD_R_SW", "1");
          //}



          break;
        case 23:
          sendToDcsBiosMessage("RADAR_SW", "2");
          break;
        case 24:
          sendToDcsBiosMessage("INS_SW", "2");  //GND
          break;
        // PRESS - CLOSE
        case 25:
          break;
        case 26:
          SendIPString("F1");
          break;
        case 27:
          break;
        case 28:
          break;
        case 29:
          sendToDcsBiosMessage("KY58_MODE_SELECT", "2");
          break;
        // PRESS - CLOSE
        case 30:
          sendToDcsBiosMessage("KY58_FILL_SELECT", "1");
          break;
        // PRESS - CLOSE
        case 31:
          sendToDcsBiosMessage("WING_FOLD_PULL", "0");
          break;
        case 32:
          break;
        case 33:
          sendToDcsBiosMessage("LST_NFLR_SW", "1");
          break;
        case 34:
          sendToDcsBiosMessage("RADAR_SW_PULL", "1");
          RadarFollowupTask = true;
          TimeRadarOn = millis() + RadarMoveTime;
          break;
        // PRESS - CLOSE
        case 35:
          sendToDcsBiosMessage("INS_SW", "3");  //NAV
          break;
        case 36:
          break;
        case 37:
          SendIPString("LCTRL F4");
          break;
        case 38:
          break;
        case 39:
          break;
        case 40:
          sendToDcsBiosMessage("KY58_MODE_SELECT", "3");
          break;
        // PRESS - CLOSE
        case 41:
          sendToDcsBiosMessage("KY58_FILL_SELECT", "2");
          break;
        case 42:
          sendToDcsBiosMessage("L_GEN_SW", "0");  //0
          break;
        case 43:
          sendToDcsBiosMessage("BATTERY_SW", "0");

          break;
        case 44:
          break;
        // PRESS - CLOSE
        case 45:
          break;
        case 46:
          sendToDcsBiosMessage("INS_SW", "4");  //IFA
          break;
        case 47:
          break;
        case 48:
          SendIPString("F2");
          break;
        case 49:
          break;
        case 50:
          break;
        // PRESS - CLOSE
        case 51:
          sendToDcsBiosMessage("KY58_POWER_SELECT", "0");
          break;
        case 52:
          sendToDcsBiosMessage("KY58_FILL_SELECT", "3");
          break;
        case 53:

          sendToDcsBiosMessage("R_GEN_SW", "0");
          break;
        case 54:
          sendToDcsBiosMessage("BATTERY_SW", "2");
          break;
        // PRESS - CLOSE
        case 55:
          break;
        case 56:
          break;
        case 57:
          sendToDcsBiosMessage("INS_SW", "5");  //GYRO
          break;
        case 58:
          SendIPString("NUM5");
          break;
        case 59:
          SendIPString("F3");
          break;
        case 60:
          break;
        // PRESS - CLOSE
        case 61:
          break;
        case 62:
          sendToDcsBiosMessage("KY58_POWER_SELECT", "1");
          break;
        case 63:
          sendToDcsBiosMessage("KY58_FILL_SELECT", "4");
          break;
        case 64:
          break;
        // PRESS - CLOSE
        case 65:
          break;
        case 66:
          break;
        case 67:
          break;
        case 68:
          sendToDcsBiosMessage("INS_SW", "6");  //GB
          break;
        case 69:
          break;
        // PRESS - CLOSE
        case 70:
          SendIPString("F6");
          break;
        // PRESS - CLOSE
        case 71:
          break;
        case 72:
          break;
        case 73:
          sendToDcsBiosMessage("KY58_POWER_SELECT", "2");
          break;
        case 74:
          sendToDcsBiosMessage("KY58_FILL_SELECT", "5");
          break;
        // PRESS - CLOSE
        case 75:
          break;
        case 76:
          break;
        case 77:
          break;
        case 78:
          break;
        case 79:
          sendToDcsBiosMessage("INS_SW", "7");  //TEST
          break;
        case 80:
          SendIPString("LSHIFT F10");
          break;
        // PRESS - CLOSE
        case 81:
          SendIPString("LCTRL F5");
          break;
        case 82:
          break;
        case 83:
          break;
        case 84:
          break;
        // PRESS - CLOSE
        case 85:
          sendToDcsBiosMessage("KY58_FILL_SELECT", "6");
          break;
        case 86:
          break;
        case 87:
          break;
        case 88:
          sendToDcsBiosMessage("LIGHTS_TEST_SW", "0");
          break;
        case 89:
          sendToDcsBiosMessage("COCKKPIT_LIGHT_MODE_SW", "2");
          break;
        // PRESS - CLOSE
        case 90:
          sendToDcsBiosMessage("WSHIELD_ANTI_ICE_SW", "2");
          break;
        // PRESS - CLOSE
        case 91:
          sendToDcsBiosMessage("ECS_MODE_SW", "2");
          break;
        case 92:
          sendToDcsBiosMessage("CABIN_PRESS_SW", "2");
          break;
        case 93:
          // Special Case for Magnetic Switches Pitot
          // if (Ethernet_In_Use == 1) {
          //   SendIPString("LCTRL LSHIFT F2");
          // } else {
          sendToDcsBiosMessage("PITOT_HEAT_SW", "1");
          //}
          break;
        case 94:
          sendToDcsBiosMessage("BLEED_AIR_KNOB", "3");
          break;
        // PRESS - CLOSE
        case 95:
          sendToDcsBiosMessage("RADALT_TEST_SW", "1");
          break;
        case 96:
          break;
        case 97:
          break;
        case 98:
          break;
        case 99:
          break;
        case 100:
          sendToDcsBiosMessage("COCKKPIT_LIGHT_MODE_SW", "0");
          break;
        // PRESS - CLOSE
        case 101:
          sendToDcsBiosMessage("WSHIELD_ANTI_ICE_SW", "0");
          break;
        case 102:
          sendToDcsBiosMessage("ECS_MODE_SW", "0");
          break;
        case 103:
          sendToDcsBiosMessage("CABIN_PRESS_SW", "0");
          break;
        case 104:
          break;
        // PRESS - CLOSE
        case 105:
          sendToDcsBiosMessage("BLEED_AIR_KNOB", "0");
          break;
        case 106:
          break;
        case 107:
          sendToDcsBiosMessage("KY58_FILL_SELECT", "7");
          break;
        case 108:
          break;
        case 109:
          break;
        // PRESS - CLOSE
        case 110:
          // Special Case for Magnetic Switches Canopy Open
          if (Ethernet_In_Use == 1) {
            SendIPString("LCTRL LSHIFT F9");
          } else {
            sendToDcsBiosMessage("CANOPY_SW", "2");
          }

          break;
        // PRESS - CLOSE
        case 111:
          sendToDcsBiosMessage("CB_HOOOK", "0");
          break;
        case 112:
          break;
        case 113:
          sendToDcsBiosMessage("ENG_ANTIICE_SW", "0");
          break;
        case 114:
          break;
        // PRESS - CLOSE
        case 115:
          sendToDcsBiosMessage("BLEED_AIR_PULL", "1");
          break;
        case 116:
          sendToDcsBiosMessage("BLEED_AIR_KNOB", "1");
          break;
        case 117:
          break;
        case 118:
          sendToDcsBiosMessage("KY58_FILL_SELECT", "8");
          break;
        case 119:
          break;
        case 120:
          // PRESS - CLOSE
          break;
        case 121:
          // On canopy down must hold switch even though it is a magnetic switch
          sendToDcsBiosMessage("CANOPY_SW", "0");

          break;
        case 122:
          sendToDcsBiosMessage("CB_FCS_CHAN4", "0");
          break;
        case 123:
          break;
        case 124:
          sendToDcsBiosMessage("ENG_ANTIICE_SW", "2");
          break;
        // PRESS - CLOSE
        case 125:
          break;
        case 126:
          break;
        case 127:
          sendToDcsBiosMessage("BLEED_AIR_KNOB", "2");
          break;
        case 128:
          break;
        case 129:
          break;
        case 130:
          break;
        // PRESS - CLOSE
        case 131:
          break;
        case 132:
          sendToDcsBiosMessage("CB_FCS_CHAN3", "0");

          break;
        case 133:
          sendToDcsBiosMessage("FCS_BIT_SW", "1");
          break;
        case 134:
          sendToDcsBiosMessage("CB_LG", "0");

          break;
        // PRESS - CLOSE
        case 135:
          break;
        case 136:
          break;
        case 137:
          break;
        case 138:
          break;
        case 139:
          break;
        case 140:
          break;
        // PRESS - CLOSE
        case 141:
          break;
        case 142:
          break;
        case 143:
          break;
        case 144:
          break;
        // PRESS - CLOSE
        case 145:
          break;
        case 146:
          break;
        case 147:
          break;
        case 148:
          break;
        case 149:
          break;
        case 150:
          break;
        // PRESS - CLOSE
        case 151:
          break;
        case 152:
          break;
        case 153:
          break;
        case 154:
          break;
        // PRESS - CLOSE
        case 155:
          break;
        case 156:
          break;
        case 157:
          break;
        case 158:
          break;
        case 159:
          break;
        case 160:
          break;
        // PRESS - CLOSE
        case 161:
          break;
        case 162:
          break;
        case 163:
          break;
        case 164:
          break;
        // PRESS - CLOSE
        case 165:
          break;
        case 166:
          break;
        case 167:
          break;
        case 168:
          break;
        case 169:
          break;
        case 170:
          break;
        // PRESS - CLOSE
        case 171:
          break;
        case 172:
          break;
        case 173:
          break;
        case 174:
          break;
        // PRESS - CLOSE
        case 175:
          break;
        case 176:
          break;
        case 177:
          break;
        case 178:
          break;
        case 179:
          break;

        default:
          // PRESS - CLOSE
          break;
          break;
      }
  }
}

//ECS PANEL
DcsBios::PotentiometerEWMA<5, 128, 5> cabinTemp("CABIN_TEMP", A6);  //"YYY" = DCS_BIOS INPUT NAME and X = PIN
DcsBios::PotentiometerEWMA<5, 128, 5> suitTemp("SUIT_TEMP", A7);    //"YYY" = DCS_BIOS INPUT NAME and X = PIN

//INTR LTS PANEL
DcsBios::Potentiometer chartDimmer("CHART_DIMMER", 3);  
//DcsBios::PotentiometerEWMA<5, 128, 5> chartDimmer("CHART_DIMMER", 3);               //set//"YYY" = DCS_BIOS INPUT NAME and X = PIN
DcsBios::PotentiometerEWMA<5, 128, 5> consolesDimmer("CONSOLES_DIMMER", 0);         //set //"YYY" = DCS_BIOS INPUT NAME and X = PIN
DcsBios::PotentiometerEWMA<5, 128, 5> floodDimmer("FLOOD_DIMMER", 2);               //"YYY" = DCS_BIOS INPUT NAME and X = PIN
DcsBios::PotentiometerEWMA<5, 128, 5> instPnlDimmer("INST_PNL_DIMMER", 1);          //"YYY" = DCS_BIOS INPUT NAME and X = PIN
DcsBios::PotentiometerEWMA<5, 128, 5> warnCautionDimmer("WARN_CAUTION_DIMMER", 4);  //"YYY" = DCS_BIOS INPUT NAME and X = PIN



void onConsolesDimmerChange(unsigned int newValue) {
  int outvalue = 0;
  outvalue = map(newValue, 0, 65534, 0, 255);
  SendLedString("Brightness=" + String(outvalue));
}
DcsBios::IntegerBuffer consolesDimmerBuffer(0x7544, 0xffff, 0, onConsolesDimmerChange);

//DE-FOG PANEL (INTR LTS)

DcsBios::PotentiometerEWMA<5, 128, 5> defogHandle("DEFOG_HANDLE", 5);  //set//"YYY" = DCS_BIOS INPUT NAME and X = PIN

//KY58 PANEL
DcsBios::PotentiometerEWMA<5, 128, 5> ky58Volume("KY58_VOLUME", 10);  //"YYY" = DCS_BIOS INPUT NAME and X = PIN

// controlPosition: 0 to 65,535 value representing the analog, real world control value
// dcsPosition: 0 to 65,535 value reported from DCS for the provided address
// return: −32,768 to 32,767 signed integer to be sent to the DCS rotary control.  0 will not be sent.
int HornetRadaltMapper(unsigned int controlPosition, unsigned int dcsPosition) {
  unsigned int a = map(controlPosition, 0, 65530, 161000, 1800);  // Silly right now, but to reduce the range if your analog pot doesn't reach max deflection, reduce the first 65535 number
  unsigned int b = map(dcsPosition, 0, 64355, 0, 65535);          // Observationally, in DCS the max value for RADALT_HEIGHT is 64355

  // Careful here since we are on 16 bit microcontrollers and doing some signed v unsigned maths.  Probably a better way to do this, but this works.
  unsigned int delta = (a >= b) ? a - b : b - a;

  const unsigned int MAX_ROTATION = 20000;  // Always keep less than 32767
  if (delta > MAX_ROTATION)
    delta = MAX_ROTATION;

  if (a >= b)
    return (int)delta;
  else
    return -1 * (int)delta;
}

// BEN


//DcsBios::RotarySyncingPotentiometer radaltHeight("RADALT_HEIGHT", 11, 0x7518, 0xffff, 0, HornetRadaltMapper);






void loop() {


  if (millis() >= NEXT_STATUS_TOGGLE_TIMER) {

    GREEN_LED_STATE = !GREEN_LED_STATE;
    RED_LED_STATE = !GREEN_LED_STATE;

    digitalWrite(GREEN_STATUS_LED_PORT, GREEN_LED_STATE);
    digitalWrite(RED_STATUS_LED_PORT, RED_LED_STATE);

    digitalWrite(HOOK_LED, RED_LED_STATE);

    // Testing digital outputs

    // digitalWrite(BLEED_AIR_SOL_PORT, RED_LED_STATE);
    // digitalWrite(PITOT_HEAT_PORT, RED_LED_STATE);
    // digitalWrite(LASER_ARM_PORT, RED_LED_STATE);
    // digitalWrite(CANOPY_MAG_PORT, RED_LED_STATE);

    NEXT_STATUS_TOGGLE_TIMER = millis() + FLASH_TIME;
  }


  // if (DCSBIOS_In_Use == 1) DcsBios::loop();

  //turn off all rows first
  for (int rowid = 0; rowid < 16; rowid++) {
    //turn on the current row
    // why differentiate? rows


    if (rowid == 0)
      PORTC = 0xFF;
    if (rowid == 8)
      PORTA = 0xFF;

    if (rowid < 8) {
      // Shift 1 right  - this is actually pulling port down
      PORTA = ~(0x1 << rowid);
    } else {
      PORTC = ~(0x1 << (15 - rowid));
    }



    //we must have such a delay so the digital pin output can go LOW steadily,
    //without this delay, the row PIN will not 100% at LOW during yet,
    //so check the first column pin's value will return incorrect result.
    delayMicroseconds(ScanDelay);

    int colResult[16];
    // Reading upper pins
    //pin 38, PD7
    colResult[0] = (PIND & B10000000) == 0 ? 0 : 1;
    //pin 39, PG2
    colResult[1] = (PING & B00000100) == 0 ? 0 : 1;
    //pin 40, PG1
    colResult[2] = (PING & B00000010) == 0 ? 0 : 1;
    //pin 41, PG0
    colResult[3] = (PING & B00000001) == 0 ? 0 : 1;

    //pin 42, PL7
    colResult[4] = (PINL & B10000000) == 0 ? 0 : 1;
    //pin 43, PL6
    colResult[5] = (PINL & B01000000) == 0 ? 0 : 1;
    //pin 44, PL5
    colResult[6] = (PINL & B00100000) == 0 ? 0 : 1;
    //pin 45, PL4
    colResult[7] = (PINL & B00010000) == 0 ? 0 : 1;

    //pin 46, PL3
    colResult[8] = (PINL & B00001000) == 0 ? 0 : 1;
    //pin 47, PL2
    colResult[9] = (PINL & B00000100) == 0 ? 0 : 1;
    //pin 48, PL1
    colResult[10] = (PINL & B00000010) == 0 ? 0 : 1;
    //pin 49, PL0
    //pin 49 is not used on the PCB design - more a mistake than anything else as it is available for us
    //colResult[11] =(PINL & B00000001) == 0 ? 0 : 1;
    colResult[11] = 1;

    // Unable to use pins 50-53 per the following
    //This is on digital pins 10, 11, 12, and 13 on the Uno and pins 50, 51, and 52 on the Mega.
    //On both boards, pin 10 is used to select the W5500 and pin 4 for the SD card. These pins cannot be used for general I/O.
    //On the Mega, the hardware SS pin, 53, is not used to select either the W5500 or the SD card,
    //pin 50, PB3
    //colResult[12] =(PINB & B00001000) == 0 ? 0 : 1;
    colResult[12] = 1;
    //pin 51, PB2
    //colResult[13] =(PINB & B00000100) == 0 ? 0 : 0;
    colResult[13] = 1;
    //pin 52, PB1
    //colResult[14] =(PINB & B00000010) == 0 ? 0 : 0;
    colResult[14] = 1;
    //pin 53, PB0
    //colResult[15] =(PINB & B00000001) == 0 ? 0 : 1;
    colResult[15] = 1;


    // There are 11 Columns per row - gives a total of 176 possible inputs
    // Have left the arrays dimensioned as per original code - if CPU or Memory becomes scarce reduce array
    for (int colid = 0; colid < 16; colid++) {
      if (colResult[colid] == 0) {
        joyReport.button[(rowid * 11) + colid] = 1;
      } else {
        joyReport.button[(rowid * 11) + colid] = 0;
      }
    }
  }

  FindInputChanges();

  /*
  // Radar Emergency required a pull up and then activate
  if (RadarFollowupTask == true) {
    if (millis() >= TimeRadarOn) {
      sendToDcsBiosMessage("RADAR_SW", "3");
      RadarFollowupTask = false;
    }
  }

  if (RadarPushFollowupTask == true) {
    if (millis() >= TimeRadarOn) {
      sendToDcsBiosMessage("RADAR_SW", "2");
      RadarPushFollowupTask = false;
    }
  }
  */

  currentMillis = millis();
}

void CaseTemplate(int ind, int state) {

  // sendToDcsBiosMessage("R_GEN_SW", "1");
  // SendIPString("LWIN F1");
  switch (state) {

    // RELEASE
    case 0:
      switch (ind) {

        case 0:
          break;
          // PRESS - OPEN
        case 1:
          break;
        case 2:
          break;
        case 3:
          break;
        case 4:
          break;
        case 5:
          break;
          // PRESS - OPEN
        case 6:
          break;
        case 7:
          break;
        case 8:
          break;
        case 9:
          break;
        case 10:
          break;
          // PRESS - OPEN
        case 11:
          break;
        case 12:
          break;
        case 13:
          break;
        case 14:
          break;
        case 15:
          break;
          // PRESS - OPEN
        case 16:
          break;
        case 17:
          break;
        case 18:
          break;
        case 19:
          break;
        case 20:
          break;
          // PRESS - OPEN
        case 21:
          break;
        case 22:
          break;
        case 23:
          break;
        case 24:
          break;
        case 25:
          break;
          // PRESS - OPEN
        case 26:
          break;
        case 27:
          break;
        case 28:
          break;
        case 29:
          break;
        case 30:
          break;
          // PRESS - OPEN
        case 31:
          break;
        case 32:
          break;
        case 33:
          break;
        case 34:
          break;
        case 35:
          break;
          // PRESS - OPEN
        case 36:
          break;
        case 37:
          break;
        case 38:
          break;
        case 39:
          break;
        case 40:
          break;
          // PRESS - OPEN
        case 41:
          break;
        case 42:
          break;
        case 43:
          break;
        case 44:
          break;
        case 45:
          break;
          // PRESS - OPEN
        case 46:
          break;
        case 47:
          break;
        case 48:
          break;
        case 49:
          break;
        case 50:
          break;
          // PRESS - OPEN
        case 51:
          break;
        case 52:
          break;
        case 53:
          break;
        case 54:
          break;
        case 55:
          break;
          // PRESS - OPEN
        case 56:
          break;
        case 57:
          break;
        case 58:
          break;
        case 59:
          break;
        case 60:
          break;
          // PRESS - OPEN
        case 61:
          break;
        case 62:
          break;
        case 63:
          break;
        case 64:
          break;
        case 65:
          break;
          // PRESS - OPEN
        case 66:
          break;
        case 67:
          break;
        case 68:
          break;
        case 69:
          break;
        case 70:
          break;
          // PRESS - OPEN
        case 71:
          break;
        case 72:
          break;
        case 73:
          break;
        case 74:
          break;
        case 75:
          break;
          // PRESS - OPEN
        case 76:
          break;
        case 77:
          break;
        case 78:
          break;
        case 79:
          break;
        case 80:
          break;
          // PRESS - OPEN
        case 81:
          break;
        case 82:
          break;
        case 83:
          break;
        case 84:
          break;
        case 85:
          break;
          // PRESS - OPEN
        case 86:
          break;
        case 87:
          break;
        case 88:
          break;
        case 89:
          break;
        case 90:
          break;
          // PRESS - OPEN
        case 91:
          break;
        case 92:
          break;
        case 93:
          break;
        case 94:
          break;
        case 95:
          break;
          // PRESS - OPEN
        case 96:
          break;
        case 97:
          break;
        case 98:
          break;
        case 99:
          break;
        case 100:
          break;
          // PRESS - OPEN
        case 101:
          break;
        case 102:
          break;
        case 103:
          break;
        case 104:
          break;
        case 105:
          break;
          // PRESS - OPEN
        case 106:
          break;
        case 107:
          break;
        case 108:
          break;
        case 109:
          break;
        case 110:
          break;
          // PRESS - OPEN
        case 111:
          break;
        case 112:
          break;
        case 113:
          break;
        case 114:
          break;
        case 115:
          break;
          // PRESS - OPEN
        case 116:
          break;
        case 117:
          break;
        case 118:
          break;
        case 119:
          break;
        case 120:
          break;
          // PRESS - OPEN
        case 121:
          break;
        case 122:
          break;
        case 123:
          break;
        case 124:
          break;
        case 125:
          break;
          // PRESS - OPEN
        case 126:
          break;
        case 127:
          break;
        case 128:
          break;
        case 129:
          break;
        case 130:
          break;
          // PRESS - OPEN
        case 131:
          break;
        case 132:
          break;
        case 133:
          break;
        case 134:
          break;
        case 135:
          break;
          // PRESS - OPEN
        case 136:
          break;
        case 137:
          break;
        case 138:
          break;
        case 139:
          break;
        case 140:
          break;
          // PRESS - OPEN
        case 141:
          break;
        case 142:
          break;
        case 143:
          break;
        case 144:
          break;
        case 145:
          break;
          // PRESS - OPEN
        case 146:
          break;
        case 147:
          break;
        case 148:
          break;
        case 149:
          break;
        case 150:
          break;
          // PRESS - OPEN
        case 151:
          break;
        case 152:
          break;
        case 153:
          break;
        case 154:
          break;
        case 155:
          break;
          // PRESS - OPEN
        case 156:
          break;
        case 157:
          break;
        case 158:
          break;
        case 159:
          break;
        case 160:
          break;
          // PRESS - OPEN
        case 161:
          break;
        case 162:
          break;
        case 163:
          break;
        case 164:
          break;
        case 165:
          break;
          // PRESS - OPEN
        case 166:
          break;
        case 167:
          break;
        case 168:
          break;
        case 169:
          break;
        case 170:
          break;
          // PRESS - OPEN
        case 171:
          break;
        case 172:
          break;
        case 173:
          break;
        case 174:
          break;
        case 175:
          break;
          // PRESS - OPEN
        case 176:
          break;
        case 177:
          break;
        case 178:
          break;
        case 179:
          break;
        default:
          break;
      }
      break;



    case 1:

      // PRESS - CLOSE
      switch (ind) {
        case 0:
          break;
        case 1:
          break;
        case 2:
          break;
        case 3:
          break;
        case 4:
          break;
        case 5:
          break;
          // PRESS - CLOSE
        case 6:
          break;
        case 7:
          break;
        case 8:
          break;
        case 9:
          break;
        case 10:
          break;
          // PRESS - CLOSE
        case 11:
          break;
        case 12:
          break;
        case 13:
          break;
        case 14:
          break;
        case 15:
          break;
          // PRESS - CLOSE
        case 16:
          break;
        case 17:
          break;
        case 18:
          break;
        case 19:
          break;
        case 20:
          break;
          // PRESS - CLOSE
        case 21:
          break;
        case 22:
          break;
        case 23:
          break;
        case 24:
          break;
        case 25:
          break;
          // PRESS - CLOSE
        case 26:
          break;
        case 27:
          break;
        case 28:
          break;
        case 29:
          break;
        case 30:
          break;
          // PRESS - CLOSE
        case 31:
          break;
        case 32:
          break;
        case 33:
          break;
        case 34:
          break;
        case 35:
          break;
          // PRESS - CLOSE
        case 36:
          break;
        case 37:
          break;
        case 38:
          break;
        case 39:
          break;
        case 40:
          break;
          // PRESS - CLOSE
        case 41:
          break;
        case 42:
          break;
        case 43:
          break;
        case 44:
          break;
        case 45:
          break;
          // PRESS - CLOSE
        case 46:
          break;
        case 47:
          break;
        case 48:
          break;
        case 49:
          break;
        case 50:
          break;
          // PRESS - CLOSE
        case 51:
          break;
        case 52:
          break;
        case 53:
          break;
        case 54:
          break;
        case 55:
          break;
          // PRESS - CLOSE
        case 56:
          break;

        case 57:
          break;
        case 58:
          break;
        case 59:
          break;
        case 60:
          break;
        // PRESS - CLOSE
        case 61:
          break;
        case 62:
          break;
        case 63:
          break;
        case 64:
          break;
        case 65:
          break;
          // PRESS - CLOSE
        case 66:
          break;
        case 67:
          break;
        case 68:
          break;
        case 69:
          break;
        case 70:
          break;
          // PRESS - CLOSE
        case 71:
          break;
        case 72:
          break;
        case 73:
          break;
        case 74:
          break;
        case 75:
          break;
          // PRESS - CLOSE
        case 76:
          break;
        case 77:
          break;
        case 78:
          break;
        case 79:
          break;
        case 80:
          break;
          // PRESS - CLOSE
        case 81:
          break;
        case 82:
          break;
        case 83:
          break;
        case 84:
          break;
        case 85:
          break;
          // PRESS - CLOSE
        case 86:
          break;
        case 87:
          break;
        case 88:
          break;
        case 89:
          break;
        case 90:
          break;
          // PRESS - CLOSE
        case 91:
          break;
        case 92:
          break;
        case 93:
          break;
        case 94:
          break;
        case 95:
          break;
          // PRESS - CLOSE
        case 96:
          break;
        case 97:
          break;
        case 98:
          break;
        case 99:
          break;
        case 100:
          break;
          // PRESS - CLOSE
        case 101:
          break;
        case 102:
          break;
        case 103:
          break;
        case 104:
          break;
        case 105:
          break;
          // PRESS - CLOSE
        case 106:
          break;
        case 107:
          break;
        case 108:
          break;
        case 109:
          break;
        case 110:
          break;
          // PRESS - CLOSE
        case 111:
          break;
        case 112:
          break;
        case 113:
          break;
        case 114:
          break;
        case 115:
          break;
          // PRESS - CLOSE
        case 116:
          break;
        case 117:
          break;
        case 118:
          break;
        case 119:
          break;
        case 120:
          break;
          // PRESS - CLOSE
        case 121:
          break;
        case 122:
          break;
        case 123:
          break;
        case 124:
          break;
        case 125:
          break;
          // PRESS - CLOSE
        case 126:
          break;
        case 127:
          break;
        case 128:
          break;
        case 129:
          break;
        case 130:
          break;
          // PRESS - CLOSE
        case 131:
          break;
        case 132:
          break;
        case 133:
          break;
        case 134:
          break;
        case 135:
          break;
          // PRESS - CLOSE
        case 136:
          break;
        case 137:
          break;
        case 138:
          break;
        case 139:
          break;
        case 140:
          break;
          // PRESS - CLOSE
        case 141:
          break;
        case 142:
          break;
        case 143:
          break;
        case 144:
          break;
        case 145:
          break;
          // PRESS - CLOSE
        case 146:
          break;
        case 147:
          break;
        case 148:
          break;
        case 149:
          break;
        case 150:
          break;
          // PRESS - CLOSE
        case 151:
          break;
        case 152:
          break;
        case 153:
          break;
        case 154:
          break;
        case 155:
          break;
          // PRESS - CLOSE
        case 156:
          break;
        case 157:
          break;
        case 158:
          break;
        case 159:
          break;
        case 160:
          break;
          // PRESS - CLOSE
        case 161:
          break;
        case 162:
          break;
        case 163:
          break;
        case 164:
          break;
        case 165:
          break;
          // PRESS - CLOSE
        case 166:
          break;
        case 167:
          break;
        case 168:
          break;
        case 169:
          break;
        case 170:
          break;
          // PRESS - CLOSE
        case 171:
          break;
        case 172:
          break;
        case 173:
          break;
        case 174:
          break;
        case 175:
          break;
          // PRESS - CLOSE
        case 176:
          break;
        case 177:
          break;
        case 178:
          break;
        case 179:
          break;

        default:
          // PRESS - CLOSE
          break;
          break;
      }
  }
}
