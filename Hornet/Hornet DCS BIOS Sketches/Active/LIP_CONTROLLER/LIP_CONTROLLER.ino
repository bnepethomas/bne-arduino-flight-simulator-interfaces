

////////////////////---||||||||||********||||||||||---\\\\\\\\\\\\\\\\\\\\
//||               FUNCTION = HORNET LOWER INSTRUMENT PANEL           ||\\
//||              LOCATION IN THE PIT = LIP.                          ||\\
//||            ARDUINO PROCESSOR TYPE = Arduino Mega 2560            ||\\
//||      ARDUINO CHIP SERIAL NUMBER = SN - 75834353230351013181      ||\\
//||                    CONNECTED COM PORT = COM 4                    ||\\
//||               ****ADD ASSIGNED COM PORT NUMBER****               ||\\
//||            ****DO CHECK S/N BEFORE UPLOAD NEW DATA****           ||\\
////////////////////---||||||||||********||||||||||---\\\\\\\\\\\\\\\\\\\\

/*
 *  */
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
String BoardName = "Hornet LIP CONTROLLER: ";

// These local Mac and IP Address will be reassigned early in startup based on
// the device ID as set by address pins
byte mac[] = { 0xA8, 0x61, 0x0A, 0x9E, 0x83, 0x73 };
String sMac = "A8:61:0A:67:83:115";
IPAddress ip(172, 16, 1, 115);
String strMyIP = "172.16.1.115";

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
}

// ********************************* End Ethernet ***************************************************


// ********************************* Begin Digital ***************************************************



// ********************************* End Digital ***************************************************

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

// #################### EXTERIOR LIGHTS #################


#define MAP_LIGHTS 2
#define BACKLIGHT_RWR_PWM 3
#define BACKLIGHT_CAB_ALT_PWM 4
#define BACKLIGHT_IEFI_PANEL_PWM 5
#define BACKLIGHT_IEFI_BUTTONS_PWM 6
#define BACKLIGHT_AMPCD_PWM 7
#define BACKLIGHT_AMPCD_BUT_PWM 8
#define BACKLIGHT_COMPASS_PWM 9
#define BACKLIGHT_JET_BUT_PWM 12

void turnOffAllBacklights() {
  digitalWrite(MAP_LIGHTS, LOW);
  digitalWrite(BACKLIGHT_RWR_PWM, LOW);
  digitalWrite(BACKLIGHT_CAB_ALT_PWM, LOW);
  digitalWrite(BACKLIGHT_IEFI_PANEL_PWM, LOW);
  digitalWrite(BACKLIGHT_IEFI_BUTTONS_PWM, LOW);
  digitalWrite(BACKLIGHT_AMPCD_PWM, LOW);
  digitalWrite(BACKLIGHT_AMPCD_BUT_PWM, LOW);
  digitalWrite(BACKLIGHT_COMPASS_PWM, LOW);
  digitalWrite(BACKLIGHT_JET_BUT_PWM, LOW);
}

void turnOnAllBacklights() {
  digitalWrite(MAP_LIGHTS, HIGH);
  digitalWrite(BACKLIGHT_RWR_PWM, HIGH);
  digitalWrite(BACKLIGHT_CAB_ALT_PWM, HIGH);
  digitalWrite(BACKLIGHT_IEFI_PANEL_PWM, HIGH);
  digitalWrite(BACKLIGHT_IEFI_BUTTONS_PWM, HIGH);
  digitalWrite(BACKLIGHT_AMPCD_PWM, HIGH);
  digitalWrite(BACKLIGHT_AMPCD_BUT_PWM, HIGH);
  digitalWrite(BACKLIGHT_COMPASS_PWM, HIGH);
  digitalWrite(BACKLIGHT_JET_BUT_PWM, HIGH);
}

// ######################## BEGIN MAX7219 ########################
#include "LedControl.h"
LedControl lc = LedControl(A11, A10, A9, 1);

void allMax7219On() {
  for (int displayunit = 0; displayunit < 1; displayunit++) {
    for (int row = 0; row < 8; row++) {
      for (int col = 0; col < 8; col++) {
        if (col != 9 && col != 9 && col != 9)
          lc.setLed(displayunit, row, col, true);
      }
    }
  }
}

void debugAllMax7219On() {
  for (int displayunit = 0; displayunit < 1; displayunit++) {
    for (int row = 0; row < 3; row++) {
      for (int col = 0; col < 4; col++) {
        if (col != 9 && col != 9 && col != 9)
          SendDebug(String(row) + ":" + String(col));
        lc.setLed(displayunit, row, col, true);
        delay(1000);
      }
    }
  }
}

void allMax7219Off() {
  for (int displayunit = 0; displayunit < 1; displayunit++) {
    for (int row = 0; row < 8; row++) {
      for (int col = 0; col < 8; col++) {
        if (col != 9 && col != 9 && col != 9)
          lc.setLed(displayunit, row, col, false);
      }
    }
  }
}

// ######################## BEGIN RWR ########################

// Note Pots are currently inversed
DcsBios::PotentiometerEWMA<5, 128, 5> rwrDmrCtrl("RWR_DMR_CTRL", A1);
DcsBios::PotentiometerEWMA<5, 128, 5> rwrAudioCtrl("RWR_AUDIO_CTRL", A0);

//DcsBios::Potentiometer rwrDmrCtrl("RWR_DMR_CTRL", A0);
//DcsBios::Potentiometer rwrAudioCtrl("RWR_AUDIO_CTRL", A1);
//DcsBios::PotentiometerEWMA<5, 128, 5> suitTemp("SUIT_TEMP", 9);    //"YYY" = DCS_BIOS INPUT NAME and X = PIN


bool RWR_POWER_BUTTON_STATE = false;  // Used to latch the RWR Power Switch
void onRwrPowerBtnChange(unsigned int newValue) {
  if (newValue == 1) {
    RWR_POWER_BUTTON_STATE = true;
  } else {
    RWR_POWER_BUTTON_STATE = false;
  }
}
DcsBios::IntegerBuffer rwrPowerBtnBuffer(0x7488, 0x1000, 12, onRwrPowerBtnChange);

void setAllRWRLed(bool newValue) {
  setRWRPwrLed(newValue);
  setRWRLimitLed(newValue);
  setRWRDisplayLed(newValue);
  setRWRSpecialEnableLed(newValue);
  setRWRSpecialLed(newValue);
  setRWROffsetEnableLed(newValue);
  setRWROffsetLed(newValue);
  setRWRFailLed(newValue);
  setRWRTestLed(newValue);
}

void setRWRPwrLed(bool newValue) {
  lc.setLed(0, 0, 1, newValue);
}

void onRwrLowerLtChange(unsigned int newValue) {
  if (newValue == 1)
    setRWRPwrLed(true);
  else
    setRWRPwrLed(false);
}
DcsBios::IntegerBuffer rwrLowerLtBuffer(FA_18C_hornet_RWR_LOWER_LT, onRwrLowerLtChange);

void setRWRLimitLed(bool newValue) {
  lc.setLed(0, 0, 2, newValue);
}

void onRwrLimitLtChange(unsigned int newValue) {
  if (newValue == 1)
    setRWRLimitLed(true);
  else
    setRWRLimitLed(false);
}
DcsBios::IntegerBuffer rwrLimitLtBuffer(FA_18C_hornet_RWR_LIMIT_LT, onRwrLimitLtChange);

void setRWRDisplayLed(bool newValue) {
  lc.setLed(0, 0, 3, newValue);
}

void onRwrDisplayLtChange(unsigned int newValue) {
  if (newValue == 1)
    setRWRDisplayLed(true);
  else
    setRWRDisplayLed(false);
}
DcsBios::IntegerBuffer rwrDisplayLtBuffer(FA_18C_hornet_RWR_DISPLAY_LT, onRwrDisplayLtChange);

void setRWRSpecialEnableLed(bool newValue) {
  lc.setLed(0, 1, 1, newValue);
}

void onRwrSpecialEnLtChange(unsigned int newValue) {
  if (newValue == 1)
    setRWRSpecialEnableLed(true);
  else
    setRWRSpecialEnableLed(false);
}
DcsBios::IntegerBuffer rwrSpecialEnLtBuffer(FA_18C_hornet_RWR_SPECIAL_EN_LT, onRwrSpecialEnLtChange);

void setRWRSpecialLed(bool newValue) {
  lc.setLed(0, 1, 2, newValue);
}
void onRwrSpecialLtChange(unsigned int newValue) {
  if (newValue == 1)
    setRWRSpecialLed(true);
  else
    setRWRSpecialLed(false);
}
DcsBios::IntegerBuffer rwrSpecialLtBuffer(FA_18C_hornet_RWR_SPECIAL_LT, onRwrSpecialLtChange);

void setRWROffsetEnableLed(bool newValue) {
  lc.setLed(0, 1, 3, newValue);
}
void onRwrEnableLtChange(unsigned int newValue) {
  if (newValue == 1)
    setRWROffsetEnableLed(true);
  else
    setRWROffsetEnableLed(false);
}
DcsBios::IntegerBuffer rwrEnableLtBuffer(FA_18C_hornet_RWR_ENABLE_LT, onRwrEnableLtChange);

void setRWROffsetLed(bool newValue) {
  lc.setLed(0, 2, 1, newValue);
}
void onRwrOffsetLtChange(unsigned int newValue) {
  if (newValue == 1)
    setRWROffsetLed(true);
  else
    setRWROffsetLed(false);
}
DcsBios::IntegerBuffer rwrOffsetLtBuffer(FA_18C_hornet_RWR_OFFSET_LT, onRwrOffsetLtChange);

void setRWRFailLed(bool newValue) {
  lc.setLed(0, 2, 2, newValue);
}
void onRwrFailLtChange(unsigned int newValue) {
  if (newValue == 1)
    setRWRFailLed(true);
  else
    setRWRFailLed(false);
}
DcsBios::IntegerBuffer rwrFailLtBuffer(FA_18C_hornet_RWR_FAIL_LT, onRwrFailLtChange);


void setRWRTestLed(bool newValue) {
  lc.setLed(0, 2, 3, newValue);
}
void onRwrBitLtChange(unsigned int newValue) {
  if (newValue == 1)
    setRWRTestLed(true);
  else
    setRWRTestLed(false);
}
DcsBios::IntegerBuffer rwrBitLtBuffer(FA_18C_hornet_RWR_BIT_LT, onRwrBitLtChange);



// ######################## END RWR ########################

// ######################## SETUP ########################


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

  pinMode(MAP_LIGHTS, OUTPUT);
  pinMode(BACKLIGHT_RWR_PWM, OUTPUT);
  pinMode(BACKLIGHT_CAB_ALT_PWM, OUTPUT);
  pinMode(BACKLIGHT_IEFI_PANEL_PWM, OUTPUT);
  pinMode(BACKLIGHT_IEFI_BUTTONS_PWM, OUTPUT);
  pinMode(BACKLIGHT_AMPCD_PWM, OUTPUT);
  pinMode(BACKLIGHT_AMPCD_BUT_PWM, OUTPUT);
  pinMode(BACKLIGHT_COMPASS_PWM, OUTPUT);
  pinMode(BACKLIGHT_JET_BUT_PWM, OUTPUT);

  turnOnAllBacklights();




  /*The MAX72XX is in power-saving mode on startup*/
  lc.shutdown(0, false);
  /* Set the brightness to a medium values */
  lc.setIntensity(0, 15);
  /* and clear the display */
  lc.clearDisplay(0);

  setAllRWRLed(true);



  // Set the output ports to output
  for (int portId = 22; portId < 38; portId++) {
    pinMode(portId, OUTPUT);
  }
  // Set the input ports to input - port 50-53 used by Ethernet Shield
  // port 49 is used as rest pin on LIP Controller
  for (int portId = 38; portId < 50; portId++) {
    // Even though we've already got 10K external pullups
    pinMode(portId, INPUT_PULLUP);
  }





  // Initialise all arrays
  for (int ind = 0; ind < NUM_BUTTONS; ind++) {

    // Clear current and last values to 0 for button inputs
    joyReport.button[ind] = 0;
    prevjoyReport.button[ind] = 0;

    // Set the end
    joyEndDebounce[ind] = 0;
  }


  while (millis() <= 6000) {
    delay(FLASH_TIME);
    digitalWrite(GREEN_STATUS_LED_PORT, false);
    delay(FLASH_TIME);
    digitalWrite(GREEN_STATUS_LED_PORT, true);
  }
  setAllRWRLed(false);
  turnOffAllBacklights();



  if (DCSBIOS_In_Use == 1) DcsBios::setup();

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
          sendToDcsBiosMessage("RWR_BIT_BTN", "0");
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

          if (RWR_POWER_BUTTON_STATE == true) {
            sendToDcsBiosMessage("RWR_POWER_BTN", "0");
          } else {
            sendToDcsBiosMessage("RWR_POWER_BTN", "1");
          }
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
          sendToDcsBiosMessage("RWR_BIT_BTN", "1");
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



void loop() {


  if (millis() >= NEXT_STATUS_TOGGLE_TIMER) {

    GREEN_LED_STATE = !GREEN_LED_STATE;
    RED_LED_STATE = !GREEN_LED_STATE;

    digitalWrite(GREEN_STATUS_LED_PORT, GREEN_LED_STATE);
    digitalWrite(RED_STATUS_LED_PORT, RED_LED_STATE);


    NEXT_STATUS_TOGGLE_TIMER = millis() + FLASH_TIME;
  }


  if (DCSBIOS_In_Use == 1) DcsBios::loop();

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
