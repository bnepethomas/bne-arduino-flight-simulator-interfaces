

////////////////////---||||||||||********||||||||||---\\\\\\\\\\\\\\\\\\\\
//||               FUNCTION = A10 RIGHT CONSOLE INPUT CONTROLLER      ||\\
//||              LOCATION IN THE PIT = LEFT HAND SIDE                ||\\
//||            ARDUINO PROCESSOR TYPE = Arduino Mega 2560            ||\\
//||      ARDUINO CHIP SERIAL NUMBER = SN -      ||\\
//||                    CONNECTED COM PORT = COM x                    ||\\
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

  it's a 16 * 11  matrix, due to the loss of pins/Columns used by the Ethernet and SD Card Shield, Digital I/O 50 through 53 are not available.
  Pin 49 is available but isn't used. This gives a total number of usable Inputs as 176 (remember numbering starts at 0 - so 0-175)

  The code pulls down a row and reads values from the Columns.
  Row 0 - Col 0 = Digit 0
  Row 0 - Col 10 = Digit 10
  Row 15 - Col 0 = Digit 165
  Row 15 = Col 10 = Digit 175

  So - Digit = Row * 11 + Col

  The sendIPstring is used to throw strings at a Leonardo to convert to keystrokes for the PC


  Confgiuring the Commands
  Copy the empty template with Open and Release Cases
  Start DCS, Kicad, Bort and UDP_Reflector.py (in Python HW Link)

  Verify all input changes are reflected in Python HW Link

  Select the panel of interest in Bort, Select Show Arduino Scaffold Code
  Select a input device eg Switch, Rotary Switch
  
  For Rotarys we are generally interested just in Close (the second case statement), 
  whereas for Toggles we will need to configure Close and Release.

  Using Python HW Link note the input numbers associated with the switch. 
  Add a comment for in the case number for the action associated with the input number
  Copy variable names from the aircraft LUA eg A-10C.lua or from BORT
  Switch



*/



int Ethernet_In_Use = 1;
int Reflector_In_Use = 1;
#define DCSBIOS_In_Use 1
#define MSFS_In_Use 0  // Used to interface into MSFS - set to 0 if not in use

// Used to Distinguish between the left, front, and right inputs
// Left=0, Front=1, Right=2
#define INPUT_MODULE_NUMBER 2

#define DCSBIOS_IRQ_SERIAL
#include <DcsBios.h>


// ###################################### Begin Ethernet Related #############################
#include <SPI.h>
#include <Ethernet.h>
#include <EthernetUdp.h>

#define EthernetStartupDelay 500
#define ES1_RESET_PIN 53
String BoardName = "A10 Left Input";

// These local Mac and IP Address will be reassigned early in startup based on
// the device ID as set by address pins
byte mac[] = { 0xA8, 0x61, 0x0A, 0x67, 0x83, 0x06 };
String sMac = "A8:61:0A:67:83:06";
IPAddress ip(172, 16, 1, 106);
String strMyIP = "172.16.1.106";

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



void SendDebug(String MessageToSend) {
  if ((Reflector_In_Use == 1) && (Ethernet_In_Use == 1)) {
    udp.beginPacket(reflectorIP, reflectorport);
    udp.println(MessageToSend);
    udp.endPacket();
  }
}
// ###################################### End Ethernet Related #############################

// THE LED PORTS WILL CHANGE FROM THE V1.1 PCB TO THE FOLLOWING

#define RED_STATUS_LED_PORT 12
#define GREEN_STATUS_LED_PORT 13
#define Check_LED_R 12
#define Check_LED_G 13

#define FLASH_TIME 300

unsigned long NEXT_STATUS_TOGGLE_TIMER = 0;
bool GREEN_LED_STATE = false;
bool RED_LED_STATE = false;
unsigned long timeSinceRedLedChanged = 0;



#define NUM_BUTTONS 256
#define BUTTONS_USED_ON_PCB 176
#define NUM_AXES 8  // 8 axes, X, Y, Z, etc




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
  digitalWrite(RED_STATUS_LED_PORT, true);
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
  }

  // Set the output ports to output
  for (int portId = 22; portId < 38; portId++) {
    pinMode(portId, OUTPUT);
  }


  // Set the input ports to input - port 50-53 used by Ethernet Shield
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


  if (DCSBIOS_In_Use == 1) DcsBios::setup();

  NEXT_STATUS_TOGGLE_TIMER = millis();
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
          if (Ethernet_In_Use == 1) SendIPMessage(ind, 1);
          if (MSFS_In_Use == 1) SendMSFSMessage(ind, 1);
        } else {
          outString = outString + "0";
          if (DCSBIOS_In_Use == 1) CreateDcsBiosMessage(ind, 0);
          if (Ethernet_In_Use == 1) SendIPMessage(ind, 0);
          if (MSFS_In_Use == 1) SendMSFSMessage(ind, 0);
        }


        prevjoyReport.button[ind] = joyReport.button[ind];

        if (Reflector_In_Use == 1) {
          udp.beginPacket(reflectorIP, reflectorport);
          udp.println("Left Input - " + String(ind) + ":" + String(joyReport.button[ind]));
          udp.endPacket();
        }
      }
    }
}

void SendIPMessage(int ind, int state) {

  if (Ethernet_In_Use == 1) {

    String outString;
    outString = String(ind) + ":" + String(state);

    udp.beginPacket(reflectorIP, reflectorport);
    udp.print(outString);
    udp.endPacket();
    UpdateRedStatusLed();
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
  if (Ethernet_In_Use == 1) {
    udp.beginPacket(targetIP, keyboardport);
    udp.print(KeysToSend);
    udp.endPacket();
    UpdateRedStatusLed();
  }
}

void SendLedString(String LedCommandToSend) {

  if (Ethernet_In_Use == 1) {
    udp.beginPacket(targetIP, ledport);
    udp.print(LedCommandToSend);
    udp.endPacket();
    UpdateRedStatusLed();
  }
}

void UpdateRedStatusLed() {
  if ((RED_LED_STATE == false) && (millis() >= (timeSinceRedLedChanged + FLASH_TIME))) {
    digitalWrite(RED_STATUS_LED_PORT, true);
    RED_LED_STATE = true;
    timeSinceRedLedChanged = millis();
  }
}




void sendToDcsBiosMessage(const char *msg, const char *arg) {


  if (Reflector_In_Use == 1) {
    udp.beginPacket(reflectorIP, reflectorport);
    udp.println("Left Input - " + String(msg) + ":" + String(arg));
    udp.endPacket();
  }

  sendDcsBiosMessage(msg, arg);
}

DcsBios::Potentiometer alcpWpnsta("ALCP_WPNSTA", A0);
DcsBios::Potentiometer alcpRcvrLts("ALCP_RCVR_LTS", A1);
DcsBios::Potentiometer saspYawTrim("SASP_YAW_TRIM", A2);

DcsBios::Potentiometer intIntVol("INT_INT_VOL", A6);
DcsBios::Potentiometer intFmVol("INT_FM_VOL", A7);
DcsBios::Potentiometer intUhfVol("INT_UHF_VOL", A8);
DcsBios::Potentiometer intVhfVol("INT_VHF_VOL", A9);
DcsBios::Potentiometer intAimVol("INT_AIM_VOL", A10);
DcsBios::Potentiometer intIffVol("INT_IFF_VOL", A11);
DcsBios::Potentiometer intTcnVol("INT_TCN_VOL", A12);
DcsBios::Potentiometer intVol("INT_VOL", A13);
DcsBios::Potentiometer intIlsVol("INT_ILS_VOL", A4);  // Using VHF_FM_VOL input as 1.1 board din't hve ILS connector

bool HM_MUTE = false;
void Toggle_HM() {
  HM_MUTE = !HM_MUTE;
  if (HM_MUTE)
    sendToDcsBiosMessage("INT_HM", "0");
  else
    sendToDcsBiosMessage("INT_HM", "1");
}

bool INT_MUTE = false;
void Toggle_INT() {
  INT_MUTE = !INT_MUTE;
  if (INT_MUTE)
    sendToDcsBiosMessage("INT_INT_UNMUTE", "0");
  else
    sendToDcsBiosMessage("INT_INT_UNMUTE", "1");
}

bool FM_MUTE = false;
void Toggle_FM() {
  FM_MUTE = !FM_MUTE;
  if (FM_MUTE)
    sendToDcsBiosMessage("INT_FM_UNMUTE", "0");
  else
    sendToDcsBiosMessage("INT_FM_UNMUTE", "1");
}

bool UHF_MUTE = false;
void Toggle_UHF() {
  UHF_MUTE = !UHF_MUTE;
  if (UHF_MUTE)
    sendToDcsBiosMessage("INT_UHF_UNMUTE", "0");
  else
    sendToDcsBiosMessage("INT_UHF_UNMUTE", "1");
}

bool VHF_MUTE = false;
void Toggle_VHF() {
  VHF_MUTE = !VHF_MUTE;
  if (VHF_MUTE)
    sendToDcsBiosMessage("INT_VHF_UNMUTE", "0");
  else
    sendToDcsBiosMessage("INT_VHF_UNMUTE", "1");
}

bool AIM_MUTE = false;
void Toggle_AIM() {
  AIM_MUTE = !AIM_MUTE;
  if (AIM_MUTE)
    sendToDcsBiosMessage("INT_AIM_UNMUTE", "0");
  else
    sendToDcsBiosMessage("INT_AIM_UNMUTE", "1");
}

bool IFF_MUTE = false;
void Toggle_IFF() {
  IFF_MUTE = !IFF_MUTE;
  if (IFF_MUTE)
    sendToDcsBiosMessage("INT_IFF_UNMUTE", "0");
  else
    sendToDcsBiosMessage("INT_IFF_UNMUTE", "1");
}

bool ILS_MUTE = false;
void Toggle_ILS() {
  ILS_MUTE = !ILS_MUTE;
  if (ILS_MUTE)
    sendToDcsBiosMessage("INT_ILS_UNMUTE", "0");
  else
    sendToDcsBiosMessage("INT_ILS_UNMUTE", "1");
}

bool TCN_MUTE = false;
void Toggle_TCN() {
  TCN_MUTE = !TCN_MUTE;
  if (TCN_MUTE)
    sendToDcsBiosMessage("INT_TCN_UNMUTE", "0");
  else
    sendToDcsBiosMessage("INT_TCN_UNMUTE", "1");
}


void CreateDcsBiosMessage(int ind, int state) {


  // Generally one of the following type of commands will be used
  //  Send a messgae to DCS BIOS
  //  sendToDcsBiosMessage("COCKKPIT_LIGHT_MODE_SW", "1");
  //
  //  or send a character to the Leonardo which converts it into a keystroke
  //  SendIPString("LCTRL LSHIFT P");
  //
  //  or a combination of
  // Special Case for Magnetic Switches Canopy Open
  //  if (Ethernet_In_Use == 1) {
  //    SendIPString("LCTRL LSHIFT F9");
  //  } else {
  //   sendToDcsBiosMessage("CANOPY_SW", "2");
  // }
  //
  // If multiple commands or characters are to be sent it is generally a good idea
  // to use a state machine with timers
  //  sendToDcsBiosMessage("RADAR_SW_PULL", "1");
  //  RadarFollowupTask = true;
  //  TimeRadarOn = millis() + RadarMoveTime;
  //  break;
  //
  //
  //  Additionally commands can be sent to MSFS
  //    SendMSFSMessage(int ind, int state)


  switch (state) {

    // RELEASE
    case 0:
      switch (ind) {
        // Release
        case 0:
          break;
        case 1:
          break;
        case 2:
          break;
        case 3:
          // Release
          break;
        case 4:
          break;
        case 5:
          break;
        case 6:
          break;
        case 7:
          break;
        // Release
        case 8:
          sendToDcsBiosMessage("SEAT_ARM", "1");
          break;
        case 9:
          sendToDcsBiosMessage("SASP_YAW_SAS_L", "0");
          break;
        case 10:
          sendToDcsBiosMessage("SASP_PITCH_SAS_L", "0");
          break;
        case 11:
          break;
        case 12:
          // Release
          break;
        case 13:
          break;
        case 14:
          break;
        case 15:
          break;
        // Release
        case 16:
          break;
        case 17:
          break;
        case 18:
          break;
        case 19:
        sendToDcsBiosMessage("LASTE_EAC", "1");
          break;
        // Release
        case 20:
          sendToDcsBiosMessage("SASP_YAW_SAS_R", "0");
          break;
        case 21:
          sendToDcsBiosMessage("SASP_PITCH_SAS_R", "0");
          break;
        case 22:
          break;
        case 23:
          break;
        // Release
        case 24:
          break;
        case 25:
          break;
        case 26:
          break;
        case 27:
          break;
          // Release
        case 28:
          break;
        case 29:
          break;
        case 30:

          break;
        case 31:
          sendToDcsBiosMessage("SASP_MONITOR_TEST", "1");
          break;
        // Release
        case 32:
          sendToDcsBiosMessage("SASP_TO_TRIM", "0");
          break;
        case 33:
          break;
        case 34:
          break;
        case 35:
          break;
        // Release
        case 36:
          break;
        case 37:
          break;
        case 38:
          break;
        case 39:
          break;
        // Release
        case 40:
          break;
        case 41:
          break;
        case 42:
          sendToDcsBiosMessage("SASP_MONITOR_TEST", "1");
          break;
        case 43:
          break;
        // Release
        case 44:
          break;
        case 45:
          break;
        case 46:
          break;
        case 47:
          break;
        // Release
        case 48:
          break;
        case 49:
          break;
        case 50:
          break;
        case 51:
          break;
        // Release
        case 52:
          sendToDcsBiosMessage("LMFD_PWR", "0");
          break;
        case 53:
          sendToDcsBiosMessage("LAMP_TEST_BTN", "0");
          break;
        case 54:
          sendToDcsBiosMessage("ALCP_HARSSAS", "1");
          break;
        case 55:
          sendToDcsBiosMessage("FSCP_EXT_TANKS_WING", "0");
          break;
        // Release
        case 56:
          sendToDcsBiosMessage("FSCP_TK_GATE", "0");
          break;
        case 57:
          sendToDcsBiosMessage("FSCP_EXT_TANKS_FUS", "0");
          break;
        case 58:
          sendToDcsBiosMessage("FSCP_CROSSFEED", "0");
          break;
        case 59:
          sendToDcsBiosMessage("FSCP_BOOST_WING_L", "0");
          break;
        // Release
        case 60:
          sendToDcsBiosMessage("FSCP_BOOST_WING_R", "0");
          break;
        case 61:
          sendToDcsBiosMessage("FSCP_BOOST_MAIN_L", "0");
          break;
        case 62:
          sendToDcsBiosMessage("FSCP_RCVR_LEVER", "1");
          break;
        case 63:
          sendToDcsBiosMessage("RMFD_PWR", "0");
          break;
        // Release
        case 64:
          sendToDcsBiosMessage("ALCP_FDBA_TEST", "0");
          break;
        case 65:
          break;
        case 66:
          sendToDcsBiosMessage("FSCP_BOOST_MAIN_R", "0");
          break;
        case 67:
          break;
        // Release
        case 68:
          sendToDcsBiosMessage("FSCP_FD_WING_L", "1");
          break;
        case 69:
          sendToDcsBiosMessage("FSCP_FD_MAIN_L", "1");
          break;
        case 70:
          sendToDcsBiosMessage("FSCP_FD_WING_R", "1");
          break;
        case 71:
          break;
        // Release
        case 72:
          sendToDcsBiosMessage("FSCP_FD_MAIN_R", "1");
          break;
        case 73:
          break;
        case 74:
          break;
        case 75:
          break;
        // Release
        case 76:
          break;
        case 77:
          break;
        case 78:
          break;
        case 79:
          break;
        // Release
        case 80:
          break;
        case 81:
          break;
        case 82:
          break;
        case 83:
          break;
        // Release
        case 84:
          break;
        case 85:
          break;
        case 86:
          break;
        case 87:
          break;
        // Release
        case 88:
          break;
        case 89:
          break;
        case 90:
          break;
        case 91:
          break;
        // Release
        case 92:
          break;
        case 93:
          break;
        case 94:
          break;
        case 95:
          break;
        // Release
        case 96:
          break;
        case 97:
          break;
        case 98:
          break;
        case 99:
          break;
        // Release
        case 100:
          break;
        case 101:
          break;
        case 102:
          break;
        case 103:
          break;
        // Release
        case 104:
          break;
        case 105:
          break;
        case 106:
          break;
        case 107:
          break;
        // Release
        case 108:
          break;
        case 109:
          break;
        case 110:
          break;
        case 111:
          break;
        // Release
        case 112:
          break;
        case 113:
          break;
        case 114:
          break;
        case 115:
          break;
        // Release
        case 116:
          break;
        case 117:
          break;
        case 118:
          break;
        case 119:
          break;
        // Release
        case 120:
          break;
        case 121:
          break;
        case 122:
          break;
        case 123:
          break;
        // Release
        case 124:
          break;
        case 125:
          break;
        case 126:
          break;
        case 127:
          break;
        // Release
        case 128:
          break;
        case 129:
          break;
        case 130:
          break;
        case 131:
          break;
        // Release
        case 132:
          break;
        case 133:
          break;
        case 134:
          break;
        case 135:
          break;
        // Release
        case 136:
          break;
        case 137:
          break;
        case 138:
          break;
        case 139:
          break;
        // Release
        case 140:
          break;
        case 141:
          break;
        case 142:
          break;
        case 143:
          break;
        case 144:
          // Release
          break;
        case 145:
          break;
        case 146:
          break;
        case 147:
          break;
        // Release
        case 148:
          break;
        case 149:
          break;
        case 150:
          break;
        case 151:
          break;
        // Release
        case 152:
          break;
        case 153:
          break;
        case 154:
          break;
        case 155:
          // Release
          break;
        case 156:
          break;
        case 157:
          break;
        case 158:
          break;
        case 159:
          break;
        // Release
        case 160:
          break;
        case 161:
          break;
        case 162:
          break;
        case 163:
          break;
        // Release
        case 164:
          break;
        case 165:
          break;
        case 166:
          break;
        case 167:
          break;
        // Release
        case 168:
          break;
        case 169:
          break;
        case 170:
          break;
        case 171:
          break;
        // Release
        case 172:
          break;
        case 173:
          break;
        case 174:
          break;
        case 175:
          break;
        // Release
        case 176:
          break;
        case 177:
          break;
        case 178:
          break;
        case 179:
          break;
          // End Release
          // Release
      }
      break;



    case 1:

      // PRESS - CLOSE
      switch (ind) {
          // Close
        case 0:
        SendIPString("F1");
          break;
        case 1:
        SendIPString("F2");
          break;
        case 2:
        SendIPString("F3");   
          break;
        case 3:
        SendIPString("ESC"); 
          break;
        // Close
        case 4:
          break;
        case 5:
          break;
        case 6:
          break;
        case 7:
          break;
        // Close
        case 8:
          sendToDcsBiosMessage("SEAT_ARM", "0");
          break;
        case 9:
          sendToDcsBiosMessage("SASP_YAW_SAS_L", "1");
          break;
        case 10:
          sendToDcsBiosMessage("SASP_PITCH_SAS_L", "1");
          break;
        case 11:
        SendIPString("F4");
          break;
        // Close
        case 12:
        SendIPString("F5");
          break;
        case 13:
        SendIPString("F6");   
          break;
        case 14:
          break;
        case 15:
          break;
        // Close
        case 16:
          break;
        case 17:
          break;
        case 18:
          break;
        case 19:
        sendToDcsBiosMessage("LASTE_EAC", "0");
          break;
        // Close
        case 20:
          sendToDcsBiosMessage("SASP_YAW_SAS_R", "1");
          break;
        case 21:
          sendToDcsBiosMessage("SASP_PITCH_SAS_R", "1");
          break;
        case 22:
        SendIPString("F7");
          break;
        case 23:
        SendIPString("F8");
          break;
        // Close
        case 24:
        SendIPString("F9");   
          break;
        case 25:
          break;
        case 26:
          break;
        case 27:
          break;
        // Close
        case 28:
          break;
        case 29:
          break;
        case 30:

          break;
        case 31:
          sendToDcsBiosMessage("SASP_MONITOR_TEST", "0");
          break;
        // Close
        case 32:
          sendToDcsBiosMessage("SASP_TO_TRIM", "1");
          break;
        case 33:
        SendIPString("F10");
          break;
        case 34:
        SendIPString("F11");
          break;
        case 35:
        SendIPString("F12");
          break;
        // Close
        case 36:
        SendIPString("\\");
          break;
        case 37:
          break;
        case 38:
          break;
        case 39:
          break;
        // Close
        case 40:
          break;
        case 41:
          break;
        case 42:
          sendToDcsBiosMessage("SASP_MONITOR_TEST", "2");
          break;
        case 43:
          break;
        // Close
        case 44:
          break;
        case 45:
          break;
        case 46:
          break;
        case 47:
          break;
        // Close
        case 48:
          break;
        case 49:
          break;
        case 50:
          break;
        case 51:
          break;
        // Close
        case 52:
          sendToDcsBiosMessage("LMFD_PWR", "2");
          break;
        case 53:
          sendToDcsBiosMessage("LAMP_TEST_BTN", "1");
          break;
        case 54:
          sendToDcsBiosMessage("ALCP_HARSSAS", "0");
          break;
        case 55:
          sendToDcsBiosMessage("FSCP_EXT_TANKS_WING", "1");
          break;
        // Close
        case 56:
          sendToDcsBiosMessage("FSCP_TK_GATE", "1");
          break;
        case 57:
          sendToDcsBiosMessage("FSCP_EXT_TANKS_FUS", "1");
          break;
        case 58:
          sendToDcsBiosMessage("FSCP_CROSSFEED", "1");
          break;
        case 59:
          sendToDcsBiosMessage("FSCP_BOOST_WING_L", "1");
          break;
        // Close
        case 60:
          sendToDcsBiosMessage("FSCP_BOOST_WING_R", "1");
          break;
        case 61:
          sendToDcsBiosMessage("FSCP_BOOST_MAIN_L", "1");
          break;
        case 62:
          sendToDcsBiosMessage("FSCP_RCVR_LEVER", "0");
          break;
        case 63:
          sendToDcsBiosMessage("RMFD_PWR", "2");
          break;
        // Close
        case 64:
          sendToDcsBiosMessage("ALCP_FDBA_TEST", "1");
          break;
        case 65:
          break;
        case 66:
          sendToDcsBiosMessage("FSCP_BOOST_MAIN_R", "1");
          break;
        case 67:
          break;
        // Close
        case 68:
          sendToDcsBiosMessage("FSCP_FD_WING_L", "0");
          break;
        case 69:
          sendToDcsBiosMessage("FSCP_FD_MAIN_L", "0");
          break;
        case 70:
          sendToDcsBiosMessage("FSCP_FD_WING_R", "0");
          break;
        case 71:
          break;
        // Close
        case 72:
          sendToDcsBiosMessage("FSCP_FD_MAIN_R", "0");
          break;
        case 73:
          break;
        case 74:
          break;
        case 75:
          break;
        // Close
        case 76:
          break;
        case 77:
          sendToDcsBiosMessage("INT_MODE", "0");
          break;
        case 78:
          Toggle_INT();
          break;
        case 79:
          Toggle_AIM();
          // Close
          break;
        // Close
        case 80:
          Toggle_HM();
          break;
        case 81:
          break;
        case 82:
          break;
        case 83:
          break;
        // Close
        case 84:
          break;
        case 85:
          break;
        case 86:
          break;
        case 87:
          break;
        // Close
        case 88:
          sendToDcsBiosMessage("INT_MODE", "1");
          break;
        case 89:
          Toggle_FM();
          break;
        case 90:
          Toggle_IFF();
          break;
        // Close
        case 91:
          break;
        case 92:
          break;
        case 93:
          break;
        // Close
        case 94:
          break;
        case 95:
          break;
        // Close
        case 96:
          break;
        case 97:
          break;
        case 98:
          break;
        case 99:
          sendToDcsBiosMessage("INT_MODE", "2");
          break;
        // Close
        case 100:
          Toggle_UHF();
          break;
        case 101:
          Toggle_ILS();
          break;
        case 102:
          break;
        case 103:
          break;
        // Close
        case 104:
          break;
        case 105:
          break;
        case 106:
          break;
        case 107:
          break;
        // Close
        case 108:
          break;
        case 109:
          break;
        case 110:
          sendToDcsBiosMessage("INT_MODE", "3");
          break;
        case 111:
          Toggle_VHF();
          break;
        // Close
        case 112:
          Toggle_TCN();
          break;
        case 113:
          break;
        case 114:
          break;
        case 115:
          break;
        // Close
        case 116:
          break;
        case 117:
          break;
        case 118:
          break;
        case 119:
          break;
        // Close
        case 120:
          break;
        case 121:
          break;
        case 122:
          break;
        case 123:
          break;
        // Close
        case 124:
          break;
        case 125:
          break;
        case 126:
          break;
        case 127:
          break;
        // Close
        case 128:
          break;
        case 129:
          break;
        case 130:
          break;
        case 131:
          break;
        // Close
        case 132:
          break;
        case 133:
          break;
        case 134:
          break;
        case 135:
          break;
        // Close
        case 136:
          break;
        case 137:
          break;
        case 138:
          break;
        case 139:
          break;
        // Close
        case 140:
          break;
        case 141:
          break;
        case 142:
          break;
        case 143:
          break;
        case 144:
          // Close
          break;
        case 145:
          break;
        case 146:
          break;
        case 147:
          break;
        // Close
        case 148:
          break;
        case 149:
          break;
        case 150:
          break;
        case 151:
          break;
        // Close
        case 152:
          break;
        case 153:
          break;
        case 154:
          break;
        case 155:
          break;
        // Close
        case 156:
          break;
        case 157:
          break;
        case 158:
          break;
        case 159:
          break;
        // Close
        case 160:
          break;
        case 161:
          break;
        case 162:
          break;
        case 163:
          break;
        // Close
        case 164:
          break;
        case 165:
          break;
        case 166:
          break;
        case 167:
          break;
        // Close
        case 168:
          break;
        case 169:
          break;
        case 170:
          break;
        case 171:
          break;
        // Close
        case 172:
          break;
        case 173:
          break;
        case 174:
          break;
        case 175:
          break;
        // Close
        case 176:
          break;
        case 177:
          break;
        case 178:
          break;
        case 179:
          break;
          // End Close

        default:
          // PRESS - CLOSE
          break;
          break;
      }
  }
}




void onConsolesDimmerChange(unsigned int newValue) {
  int outvalue = 0;
  outvalue = map(newValue, 0, 65534, 0, 255);
  SendLedString("Brightness=" + String(outvalue));
}
DcsBios::IntegerBuffer consolesDimmerBuffer(0x7544, 0xffff, 0, onConsolesDimmerChange);


//KY58 PANEL
// DcsBios::PotentiometerEWMA<5, 128, 5> ky58Volume("KY58_VOLUME", 10);  //"YYY" = DCS_BIOS INPUT NAME and X = PIN

//DcsBios::RotarySyncingPotentiometer radaltHeight("RADALT_HEIGHT", 11, 0x7518, 0xffff, 0, HornetRadaltMapper);






void loop() {

  if (millis() >= NEXT_STATUS_TOGGLE_TIMER) {
    RED_LED_STATE = !RED_LED_STATE;

    digitalWrite(Check_LED_G, RED_LED_STATE);
    digitalWrite(Check_LED_R, !RED_LED_STATE);
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
}

void CaseTemplate() {

  //        // Release
  //        case 0:
  //          break;
  //        case 1:
  //          break;
  //        case 2:
  //          break;
  //        case 3:
  //        // Release
  //          break;
  //        case 4:
  //          break;
  //        case 5:
  //          break;
  //        case 6:
  //          break;
  //        case 7:
  //          break;
  //        // Release
  //        case 8:
  //          break;
  //        case 9:
  //          break;
  //        case 10:
  //          break;
  //        case 11:
  //          break;
  //        case 12:
  //        // Release
  //          break;
  //        case 13:
  //          break;
  //        case 14:
  //          break;
  //        case 15:
  //          break;
  //        // Release
  //        case 16:
  //          break;
  //        case 17:
  //          break;
  //        case 18:
  //          break;
  //        case 19:
  //          break;
  //        // Release
  //        case 20:
  //          break;
  //        case 21:
  //          break;
  //        case 22:
  //          break;
  //        case 23:
  //          break;
  //        // Release
  //        case 24:
  //          break;
  //        case 25:
  //          break;
  //        case 26:
  //          break;
  //        case 27:
  //          break;
  //        case 28:
  //        // Release
  //          break;
  //        case 29:
  //          break;
  //        case 30:
  //          break;
  //        case 31:
  //          break;
  //        // Release
  //        case 32:
  //          break;
  //        case 33:
  //          break;
  //        case 34:
  //          break;
  //        case 35:
  //          break;
  //        // Release
  //        case 36:
  //          break;
  //        case 37:
  //          break;
  //        case 38:
  //          break;
  //        case 39:
  //          break;
  //        // Release
  //        case 40:
  //          break;
  //        case 41:
  //          break;
  //        case 42:
  //          break;
  //        case 43:
  //          break;
  //        // Release
  //        case 44:
  //          break;
  //        case 45:
  //          break;
  //        case 46:
  //          break;
  //        case 47:
  //          break;
  //        // Release
  //        case 48:
  //          break;
  //        case 49:
  //          break;
  //        case 50:
  //          break;
  //        case 51:
  //          break;
  //        // Release
  //        case 52:
  //          break;
  //        case 53:
  //          break;
  //        case 54:
  //          break;
  //        case 55:
  //          break;
  //        // Release
  //        case 56:
  //          break;
  //        case 57:
  //          break;
  //        case 58:
  //          break;
  //        case 59:
  //          break;
  //        // Release
  //        case 60:
  //          break;
  //        case 61:
  //          break;
  //        case 62:
  //          break;
  //        case 63:
  //          break;
  //        // Release
  //        case 64:
  //          break;
  //        case 65:
  //          break;
  //        case 66:
  //          break;
  //        case 67:
  //          break;
  //        // Release
  //        case 68:
  //          break;
  //        case 69:
  //          break;
  //        case 70:
  //          break;
  //        case 71:
  //          break;
  //        // Release
  //        case 72:
  //          break;
  //        case 73:
  //          break;
  //        case 74:
  //          break;
  //        case 75:
  //          break;
  //        // Release
  //        case 76:
  //          break;
  //        case 77:
  //          break;
  //        case 78:
  //          break;
  //        case 79:
  //          break;
  //        // Release
  //        case 80:
  //          break;
  //        case 81:
  //          break;
  //        case 82:
  //          break;
  //        case 83:
  //          break;
  //        // Release
  //        case 84:
  //          break;
  //        case 85:
  //          break;
  //        case 86:
  //          break;
  //        case 87:
  //          break;
  //        // Release
  //        case 88:
  //          break;
  //        case 89:
  //          break;
  //        case 90:
  //          break;
  //        case 91:
  //          break;
  //        // Release
  //        case 92:
  //          break;
  //        case 93:
  //          break;
  //        case 94:
  //          break;
  //        case 95:
  //          break;
  //        // Release
  //        case 96:
  //          break;
  //        case 97:
  //          break;
  //        case 98:
  //          break;
  //        case 99:
  //          break;
  //        // Release
  //        case 100:
  //          break;
  //        case 101:
  //          break;
  //        case 102:
  //          break;
  //        case 103:
  //          break;
  //        // Release
  //        case 104:
  //          break;
  //        case 105:
  //          break;
  //        case 106:
  //          break;
  //        case 107:
  //          break;
  //        // Release
  //        case 108:
  //          break;
  //        case 109:
  //          break;
  //        case 110:
  //          break;
  //        case 111:
  //          break;
  //        // Release
  //        case 112:
  //          break;
  //        case 113:
  //          break;
  //        case 114:
  //          break;
  //        case 115:
  //          break;
  //        // Release
  //        case 116:
  //          break;
  //        case 117:
  //          break;
  //        case 118:
  //          break;
  //        case 119:
  //          break;
  //        // Release
  //        case 120:
  //          break;
  //        case 121:
  //          break;
  //        case 122:
  //          break;
  //        case 123:
  //          break;
  //        // Release
  //        case 124:
  //          break;
  //        case 125:
  //          break;
  //        case 126:
  //          break;
  //        case 127:
  //          break;
  //        // Release
  //        case 128:
  //          break;
  //        case 129:
  //          break;
  //        case 130:
  //          break;
  //        case 131:
  //          break;
  //        // Release
  //        case 132:
  //          break;
  //        case 133:
  //          break;
  //        case 134:
  //          break;
  //        case 135:
  //          break;
  //        // Release
  //        case 136:
  //          break;
  //        case 137:
  //          break;
  //        case 138:
  //          break;
  //        case 139:
  //          break;
  //        // Release
  //        case 140:
  //          break;
  //        case 141:
  //          break;
  //        case 142:
  //          break;
  //        case 143:
  //          break;
  //        case 144:
  //        // Release
  //          break;
  //        case 145:
  //          break;
  //        case 146:
  //          break;
  //        case 147:
  //          break;
  //        // Release
  //        case 148:
  //          break;
  //        case 149:
  //          break;
  //        case 150:
  //          break;
  //        case 151:
  //          break;
  //        // Release
  //        case 152:
  //          break;
  //        case 153:
  //          break;
  //        case 154:
  //          break;
  //        case 155:
  //        // Release
  //          break;
  //        case 156:
  //          break;
  //        case 157:
  //          break;
  //        case 158:
  //          break;
  //        case 159:
  //          break;
  //        // Release
  //        case 160:
  //          break;
  //        case 161:
  //          break;
  //        case 162:
  //          break;
  //        case 163:
  //          break;
  //        // Release
  //        case 164:
  //          break;
  //        case 165:
  //          break;
  //        case 166:
  //          break;
  //        case 167:
  //          break;
  //        // Release
  //        case 168:
  //          break;
  //        case 169:
  //          break;
  //        case 170:
  //          break;
  //        case 171:
  //          break;
  //        // Release
  //        case 172:
  //          break;
  //        case 173:
  //          break;
  //        case 174:
  //          break;
  //        case 175:
  //          break;
  //        // Release
  //        case 176:
  //          break;
  //        case 177:
  //          break;
  //        case 178:
  //          break;
  //        case 179:
  //          break;
  //        // End Release


  //        // Close
  //        case 0:
  //          break;
  //        case 1:
  //          break;
  //        case 2:
  //          break;
  //        case 3:
  //          break;
  //        // Close
  //        case 4:
  //          break;
  //        case 5:
  //          break;
  //        case 6:
  //          break;
  //        case 7:
  //          break;
  //        // Close
  //        case 8:
  //          break;
  //        case 9:
  //          break;
  //        case 10:
  //          break;
  //        case 11:
  //          break;
  //        // Close
  //        case 12:
  //          break;
  //        case 13:
  //          break;
  //        case 14:
  //          break;
  //        case 15:
  //          break;
  //        // Close
  //        case 16:
  //          break;
  //        case 17:
  //          break;
  //        case 18:
  //          break;
  //        case 19:
  //          break;
  //        // Close
  //        case 20:
  //          break;
  //        case 21:
  //          break;
  //        case 22:
  //          break;
  //        case 23:
  //          break;
  //        // Close
  //        case 24:
  //          break;
  //        case 25:
  //          break;
  //        case 26:
  //          break;
  //        case 27:
  //          break;
  //        // Close
  //        case 28:
  //          break;
  //        case 29:
  //          break;
  //        case 30:
  //          break;
  //        case 31:
  //          break;
  //        // Close
  //        case 32:
  //          break;
  //        case 33:
  //          break;
  //        case 34:
  //          break;
  //        case 35:
  //          break;
  //        // Close
  //        case 36:
  //          break;
  //        case 37:
  //          break;
  //        case 38:
  //          break;
  //        case 39:
  //          break;
  //        // Close
  //        case 40:
  //          break;
  //        case 41:
  //          break;
  //        case 42:
  //          break;
  //        case 43:
  //          break;
  //        // Close
  //        case 44:
  //          break;
  //        case 45:
  //          break;
  //        case 46:
  //          break;
  //        case 47:
  //          break;
  //        // Close
  //        case 48:
  //          break;
  //        case 49:
  //          break;
  //        case 50:
  //          break;
  //        case 51:
  //          break;
  //        // Close
  //        case 52:
  //          break;
  //        case 53:
  //          break;
  //        case 54:
  //          break;
  //        case 55:
  //          break;
  //        // Close
  //        case 56:
  //          break;
  //        case 57:
  //          break;
  //        case 58:
  //          break;
  //        case 59:
  //          break;
  //        // Close
  //        case 60:
  //          break;
  //        case 61:
  //          break;
  //        case 62:
  //          break;
  //        case 63:
  //          break;
  //        // Close
  //        case 64:
  //          break;
  //        case 65:
  //          break;
  //        case 66:
  //          break;
  //        case 67:
  //          break;
  //        // Close
  //        case 68:
  //          break;
  //        case 69:
  //          break;
  //        case 70:
  //          break;
  //        case 71:
  //          break;
  //        // Close
  //        case 72:
  //          break;
  //        case 73:
  //          break;
  //        case 74:
  //          break;
  //        case 75:
  //          break;
  //        // Close
  //        case 76:
  //          break;
  //        case 77:
  //          break;
  //        case 78:
  //          break;
  //        case 79:
  //        // Close
  //          break;
  //        // Close
  //        case 80:
  //          break;
  //        case 81:
  //          break;
  //        case 82:
  //          break;
  //        case 83:
  //          break;
  //        // Close
  //        case 84:
  //          break;
  //        case 85:
  //          break;
  //        case 86:
  //          break;
  //        case 87:
  //          break;
  //        // Close
  //        case 88:
  //          break;
  //        case 89:
  //          break;
  //        case 90:
  //          break;
  //        // Close
  //        case 91:
  //          break;
  //        case 92:
  //          break;
  //        case 93:
  //          break;
  //        // Close
  //        case 94:
  //          break;
  //        case 95:
  //          break;
  //        // Close
  //        case 96:
  //          break;
  //        case 97:
  //          break;
  //        case 98:
  //          break;
  //        case 99:
  //          break;
  //        // Close
  //        case 100:
  //          break;
  //        case 101:
  //          break;
  //        case 102:
  //          break;
  //        case 103:
  //          break;
  //        // Close
  //        case 104:
  //          break;
  //        case 105:
  //          break;
  //        case 106:
  //          break;
  //        case 107:
  //          break;
  //        // Close
  //        case 108:
  //          break;
  //        case 109:
  //          break;
  //        case 110:
  //          break;
  //        case 111:
  //          break;
  //        // Close
  //        case 112:
  //          break;
  //        case 113:
  //          break;
  //        case 114:
  //          break;
  //        case 115:
  //          break;
  //        // Close
  //        case 116:
  //          break;
  //        case 117:
  //          break;
  //        case 118:
  //          break;
  //        case 119:
  //          break;
  //        // Close
  //        case 120:
  //          break;
  //        case 121:
  //          break;
  //        case 122:
  //          break;
  //        case 123:
  //          break;
  //        // Close
  //        case 124:
  //          break;
  //        case 125:
  //          break;
  //        case 126:
  //          break;
  //        case 127:
  //          break;
  //        // Close
  //        case 128:
  //          break;
  //        case 129:
  //          break;
  //        case 130:
  //          break;
  //        case 131:
  //          break;
  //        // Close
  //        case 132:
  //          break;
  //        case 133:
  //          break;
  //        case 134:
  //          break;
  //        case 135:
  //          break;
  //        // Close
  //        case 136:
  //          break;
  //        case 137:
  //          break;
  //        case 138:
  //          break;
  //        case 139:
  //          break;
  //        // Close
  //        case 140:
  //          break;
  //        case 141:
  //          break;
  //        case 142:
  //          break;
  //        case 143:
  //          break;
  //        case 144:
  //        // Close
  //          break;
  //        case 145:
  //          break;
  //        case 146:
  //          break;
  //        case 147:
  //          break;
  //        // Close
  //        case 148:
  //          break;
  //        case 149:
  //          break;
  //        case 150:
  //          break;
  //        case 151:
  //          break;
  //        // Close
  //        case 152:
  //          break;
  //        case 153:
  //          break;
  //        case 154:
  //          break;
  //        case 155:
  //          break;
  //        // Close
  //        case 156:
  //          break;
  //        case 157:
  //          break;
  //        case 158:
  //          break;
  //        case 159:
  //          break;
  //        // Close
  //        case 160:
  //          break;
  //        case 161:
  //          break;
  //        case 162:
  //          break;
  //        case 163:
  //          break;
  //        // Close
  //        case 164:
  //          break;
  //        case 165:
  //          break;
  //        case 166:
  //          break;
  //        case 167:
  //          break;
  //        // Close
  //        case 168:
  //          break;
  //        case 169:
  //          break;
  //        case 170:
  //          break;
  //        case 171:
  //          break;
  //        // Close
  //        case 172:
  //          break;
  //        case 173:
  //          break;
  //        case 174:
  //          break;
  //        case 175:
  //          break;
  //        // Close
  //        case 176:
  //          break;
  //        case 177:
  //          break;
  //        case 178:
  //          break;
  //        case 179:
  //          break;
  //        // End Close
}
