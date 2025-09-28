/*

  ////////////////////---||||||||||********||||||||||---\\\\\\\\\\\\\\\\\\\\
  //||                  FUNCTION = HORNET FORWARD INPUT                 ||\\
  //||              LOCATION IN THE PIT = LIP LEFTHAND SIDE             ||\\
  //||            ARDUINO PROCESSOR TYPE = Arduino Mega 2560            ||\\
  //||      ARDUINO CHIP SERIAL NUMBER = SN -       ||\\
  //||                    CONNECTED COM PORT = COM 25                    ||\\
  //||            ****DO CHECK S/N BEFORE UPLOAD NEW DATA****           ||\\
  ////////////////////---||||||||||********||||||||||---\\\\\\\\\\\\\\\\\\\\

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

*/


/*
Issue - Right DDI Constrast causes all pots to move - possibly impacting rail?

Passed
Left and Right DDI - all buttons

Resolved Issues
Controller
Spare 1 is col 6
Spare 2 is col 7
R5 had CAP in its place remove 3 stray labels on silkscreen
Tie MS2 on both motor drivers to high so only half stepping is used (not 8th when MS1 low and MS2 low)


Master ARM
Master Arm A/A A/G and Ready Discharge
Needed to rotate D5 and D14
Change indicator leds from series to parallel
Swap location of D7 and D9 so they are paired with same switch
Possible incorrect colours for Discharge upper should be orange - lower green


Code ToDo's - add check that DCS is active to enable stepper driver


*/
#define GREEN_STATUS_LED_PORT 14
#define RED_STATUS_LED_PORT 15  // RED LED is used for monitoring ethernet
#define FLASH_TIME 3000

unsigned long NEXT_STATUS_TOGGLE_TIMER = 0;
bool RED_LED_STATE = false;
bool GREEN_LED_STATE = true;
unsigned long timeSinceRedLedChanged = 0;

#define Ethernet_In_Use 1
#define DCSBIOS_In_Use 1

int Reflector_In_Use = 1;

#define DCSBIOS_IRQ_SERIAL
#include "DcsBios.h"


// Ethernet Related
#include <SPI.h>
#include <Ethernet.h>
#include <EthernetUdp.h>

// These local Mac and IP Address will be reassigned early in startup based on
// the device ID as set by address pins
byte mac[] = { 0xA8, 0x61, 0x0A, 0x9E, 0x83, 0x01 };
String sMac = "A8:61:0A:9E:83:01";
IPAddress ip(172, 16, 1, 101);
String strMyIP = "172.16.1.101";

// Raspberry Pi is Target
IPAddress reflectorIP(172, 16, 1, 10);
String strreflectorIP = "X.X.X.X";




const unsigned int localport = 7788;
const unsigned int localdebugport = 7795;

const unsigned int reflectorport = 27000;


int packetSize;
int debugLen;
EthernetUDP udp;
EthernetUDP debugUDP;

#define EthernetStartupDelay 500
#define ES1_RESET_PIN 53

const unsigned long delayBeforeSendingPacket = 3000;
unsigned long ethernetStartTime = 0;
String BoardName = "UIP Combined Controller: ";

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

// ********************** Added Smoothing Filter for AOA Indexer Brightness
// Not used in UIP combined controller
// From https://github.com/jonnieZG/EWMA
#include <Ewma.h>

// ********************* End Smoothing Filter *************

// ########################## BEGIN STEPPERS ########################################
#include <AccelStepper.h>

// Currently the stepper forward reverse is mapped to Master ARM A/A A/G
bool HUD_STEPPER_ENABLED = false;
bool HUD_STEPPER_FORWARD = false;
bool HUD_STEPPER_REVERSE = false;

#define STEPPER_MAX_SPEED 90000
#define STEPPER_ACCELERATION 9000

#define AllstepperEnablePin 19

#define HUDstepPin 18
#define HUDdirectionPin 17


#define STEPS 10080
#define STEPS 315 * 16  // The 16 is the default divisors when no pins are tied together on the driver module \

AccelStepper HUDStepper(AccelStepper::DRIVER, HUDstepPin, HUDdirectionPin);

void updateSteppers() {
  HUDStepper.run();
}

// ########################### END STEPPERS #########################################


// ********************* Begin Spin Leds ******************

#define SPIN_LED_PIN 13

void SPIN_LED_ON() {
  digitalWrite(SPIN_LED_PIN, HIGH);
}

void SPIN_LED_OFF() {
  digitalWrite(SPIN_LED_PIN, LOW);
}

void onSpinLtChange(unsigned int newValue) {
  if (newValue == 1) {
    SPIN_LED_ON();
  } else {
    SPIN_LED_OFF();
  }
}
DcsBios::IntegerBuffer spinLtBuffer(0x742a, 0x0800, 11, onSpinLtChange);

// ********************* End SpinLeds ********************

// ********************** Begin Max7219 ******************

#include "LedControl.h"

/*
LOAD -> A9  -> D63
CLOCK -> A10 -> D64
DIN -> A11 -> D65
*/
//LedControl lc = LedControl(A11, A10, A9, 1);
LedControl lc = LedControl(65, 64, 63, 1);

void Max7219_ALL_ON() {
  SendDebug("Max7219 On");
  for (int row = 0; row < 8; row++) {
    for (int col = 0; col < 8; col++) {
      lc.setLed(0, row, col, true);
    }
  }
}

void Max7219_ALL_OFF() {
  SendDebug("Max7219 Off");
  for (int row = 0; row < 8; row++) {
    for (int col = 0; col < 8; col++) {
      lc.setLed(0, row, col, false);
    }
  }
}


void MASTER_ARM_DISCHARGE_READY(bool ledstate) {
  // Led location
  // Cathode -1, Anode
  lc.setLed(0, 4, 1, ledstate);
}

void onMcReadyChange(unsigned int newValue) {
  if (newValue == 1) {
    MASTER_ARM_DISCHARGE_READY(true);
  } else {
    MASTER_ARM_DISCHARGE_READY(false);
  }
}
DcsBios::IntegerBuffer mcReadyBuffer(0x740c, 0x8000, 15, onMcReadyChange);



void MASTER_ARM_DISCHARGE_DONE(bool ledstate) {
  // Led location
  // Cathode -1, Anode
  lc.setLed(0, 5, 1, ledstate);
}

void onMcDischChange(unsigned int newValue) {
  if (newValue == 1) {
    MASTER_ARM_DISCHARGE_DONE(true);
  } else {
    MASTER_ARM_DISCHARGE_DONE(false);
  }
}
DcsBios::IntegerBuffer mcDischBuffer(0x740c, 0x4000, 14, onMcDischChange);

void MASTER_ARM_AA(bool ledstate) {
  // Led location
  // Cathode -1, Anode
  lc.setLed(0, 5, 2, ledstate);
}

void onMasterModeAaLtChange(unsigned int newValue) {
  if (newValue == 1) {
    MASTER_ARM_AA(true);
  } else {
    MASTER_ARM_AA(false);
  }
}
DcsBios::IntegerBuffer masterModeAaLtBuffer(0x740c, 0x0200, 9, onMasterModeAaLtChange);

void MASTER_ARM_AG(bool ledstate) {
  // Led location
  // Cathode -1, Anode
  lc.setLed(0, 4, 2, ledstate);
}

void onMasterModeAgLtChange(unsigned int newValue) {
  if (newValue == 1) {
    MASTER_ARM_AG(true);
  } else {
    MASTER_ARM_AG(false);
  }
}
DcsBios::IntegerBuffer masterModeAgLtBuffer(0x740c, 0x0400, 10, onMasterModeAgLtChange);

// ********************* End Max7219 ********************

#define NUM_BUTTONS 256
#define BUTTONS_USED_ON_PCB 176
#define NUM_AXES 8  // 8 axes, X, Y, Z, etc
#define STATUS_LED_PORT 7
#define FLASH_TIME 300

//
struct joyReport_t {
  int button[NUM_BUTTONS];  // 1 Button per byte - was originally one bit per byte - but we have plenty of storage space
};

// Go through the man loop a number of times before sending data to the Sim
// This allows analog averages to be established and the DigitalButton array to be populated
// This has to more than simply the number of elements in the array as the array values are initialised
// to 0, so it actually takes more than 30 interations before the average it fully established
int LoopsBeforeSendingAllowed = 40;
bool SendingAllowed = false;

const int ScanDelay = 80;
const int DebounceDelay = 20;

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

bool SpinFollowupTask = false;  // Spin Button Cover
bool LFBCFollowupTask = false;  // Left Fire Button Cover
bool RFBCFollowupTask = false;  // Right Fire Button Cover
long timeSpinOn = 0;            // Spin Button Cover
long timeLFBCOn = 0;            // Left Fire Button Cover
long timeRFBCOn = 0;            // Right Fire Button Cover
const int ToggleSwitchCoverMoveTime = 500;


bool RWR_POWER_BUTTON_STATE = false;  // Used to latch the RWR Power Switch

void setup() {

  pinMode(GREEN_STATUS_LED_PORT, OUTPUT);
  pinMode(RED_STATUS_LED_PORT, OUTPUT);
  digitalWrite(GREEN_STATUS_LED_PORT, true);
  digitalWrite(RED_STATUS_LED_PORT, true);

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

  // Spin Leds
  pinMode(SPIN_LED_PIN, OUTPUT);
  SPIN_LED_ON();

  lc.shutdown(0, false);
  lc.setIntensity(0, 8);
  lc.clearDisplay(0);
  Max7219_ALL_OFF();
  delay(1000);
  Max7219_ALL_ON();






  SendDebug("STEPPER INITIALISATION STARTED");
  pinMode(AllstepperEnablePin, OUTPUT);
  digitalWrite(AllstepperEnablePin, false);

  HUDStepper.setMaxSpeed(STEPPER_MAX_SPEED);
  HUDStepper.setAcceleration(STEPPER_ACCELERATION);





  // ################# Start HUD Startup #########################
  SendDebug("Stepper movement on startup will be disabled once testing is complete");
  SendDebug("Start HUD Stepper");

  HUDStepper.runToNewPosition(-STEPS * 1.1);
  HUDStepper.setCurrentPosition(0);

  for (int i = 1; i <= 1; i++) {
    SendDebug("Loop :" + String(i));
    HUDStepper.runToNewPosition(STEPS);
    delay(200);
    HUDStepper.runToNewPosition(0);
    delay(200);
  }

  HUDStepper.setCurrentPosition(0);
  SendDebug("End HUD Stepper");
  // ################# End HUD Startup #########################




  SendDebug("STEPPER INITIALISATION COMPLETE");


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

  delay(1000);
  SPIN_LED_OFF();
  Max7219_ALL_OFF();
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

        } else {
          outString = outString + "0";
          if (DCSBIOS_In_Use == 1) CreateDcsBiosMessage(ind, 0);
        }

        prevjoyReport.button[ind] = joyReport.button[ind];


        SendDebug("Front Input - " + String(ind) + ":" + String(joyReport.button[ind]));
      }
    }
  updateSteppers();
}





void sendToDcsBiosMessage(const char *msg, const char *arg) {
  SendDebug("Front Input - " + String(msg) + ":" + String(arg));
  sendDcsBiosMessage(msg, arg);
}


void CreateDcsBiosMessage(int ind, int state) {

  switch (state) {
    case 0:
      switch (ind) {
        case 0:
          sendToDcsBiosMessage("LEFT_DDI_PB_05", "0");
          break;
        case 1:
          sendToDcsBiosMessage("LEFT_DDI_PB_20", "0");
          break;
        case 2:
          sendToDcsBiosMessage("LEFT_DDI_PB_15", "0");
          break;
        case 3:
          sendToDcsBiosMessage("LEFT_DDI_PB_10", "0");
          break;
        case 4:  // USED BELOW
          break;
        case 5:
          sendToDcsBiosMessage("RIGHT_DDI_PB_05", "0");
          break;
        case 6:
          sendToDcsBiosMessage("RIGHT_DDI_PB_20", "0");
          break;
        case 7:
          sendToDcsBiosMessage("RIGHT_DDI_PB_15", "0");
          break;
        case 8:
          sendToDcsBiosMessage("RIGHT_DDI_PB_10", "0");
          break;
        case 9:  // USED BELOW
          break;
        case 10:
          sendToDcsBiosMessage("LEFT_DDI_CRS_SW", "1");
          break;
        case 11:
          sendToDcsBiosMessage("LEFT_DDI_PB_04", "0");
          break;
        case 12:
          sendToDcsBiosMessage("LEFT_DDI_PB_19", "0");
          break;
        case 13:
          sendToDcsBiosMessage("LEFT_DDI_PB_14", "0");
          break;
        case 14:
          sendToDcsBiosMessage("LEFT_DDI_PB_09", "0");
          break;
        case 15:  // USED BELOW
          break;
        case 16:
          sendToDcsBiosMessage("RIGHT_DDI_PB_04", "0");
          break;
        case 17:
          sendToDcsBiosMessage("RIGHT_DDI_PB_19", "0");
          break;
        case 18:
          sendToDcsBiosMessage("RIGHT_DDI_PB_14", "0");
          break;
        case 19:
          sendToDcsBiosMessage("RIGHT_DDI_PB_09", "0");
          break;
        case 20:  // USED BELOW
          break;
        case 21:
          sendToDcsBiosMessage("LEFT_DDI_CRS_SW", "1");
          break;
        case 22:
          sendToDcsBiosMessage("LEFT_DDI_PB_03", "0");
          break;
        case 23:
          sendToDcsBiosMessage("LEFT_DDI_PB_18", "0");
          break;
        case 24:
          sendToDcsBiosMessage("LEFT_DDI_PB_13", "0");
          break;
        case 25:
          sendToDcsBiosMessage("LEFT_DDI_PB_08", "0");
          break;
        case 26:  // USED BELOW
          break;
        case 27:
          sendToDcsBiosMessage("RIGHT_DDI_PB_03", "0");
          break;
        case 28:
          sendToDcsBiosMessage("RIGHT_DDI_PB_18", "0");
          break;
        case 29:
          sendToDcsBiosMessage("RIGHT_DDI_PB_13", "0");
          break;
        case 30:
          sendToDcsBiosMessage("RIGHT_DDI_PB_08", "0");
          break;
        case 31:  // USED BELOW
          break;
        case 32:
          sendToDcsBiosMessage("UFC_ILS", "0");
          break;
        case 33:
          sendToDcsBiosMessage("LEFT_DDI_PB_02", "0");
          break;
        case 34:
          sendToDcsBiosMessage("LEFT_DDI_PB_17", "0");
          break;
        case 35:
          sendToDcsBiosMessage("LEFT_DDI_PB_12", "0");
          break;
        case 36:
          sendToDcsBiosMessage("LEFT_DDI_PB_07", "0");
          break;
        case 37:
          sendToDcsBiosMessage("UFC_COMM1_PULL", "0");
          //sendToDcsBiosMessage("UFC_COMM2_PULL", "0");
          break;
        case 38:
          sendToDcsBiosMessage("RIGHT_DDI_PB_02", "0");
          break;
        case 39:
          sendToDcsBiosMessage("RIGHT_DDI_PB_17", "0");
          break;
        case 40:
          sendToDcsBiosMessage("RIGHT_DDI_PB_12", "0");
          break;
        case 41:
          sendToDcsBiosMessage("RIGHT_DDI_PB_07", "0");
          break;
        case 42:
          sendToDcsBiosMessage("MASTER_CAUTION_RESET_SW", "0");
          break;
        case 43:
          sendToDcsBiosMessage("LEFT_FIRE_BTN", "0");
          sendToDcsBiosMessage("LEFT_FIRE_BTN_COVER", "0");
          break;
        case 44:
          sendToDcsBiosMessage("LEFT_DDI_PB_01", "0");
          break;
        case 45:
          sendToDcsBiosMessage("LEFT_DDI_PB_16", "0");
          break;
        case 46:
          sendToDcsBiosMessage("LEFT_DDI_PB_11", "0");
          break;
        case 47:
          sendToDcsBiosMessage("LEFT_DDI_PB_06", "0");
          break;
        case 48:
          sendToDcsBiosMessage("UFC_COMM2_PULL", "0");
          break;
        case 49:
          sendToDcsBiosMessage("RIGHT_DDI_PB_01", "0");
          break;
        case 50:
          sendToDcsBiosMessage("RIGHT_DDI_PB_16", "0");
          break;
        case 51:
          sendToDcsBiosMessage("RIGHT_DDI_PB_11", "0");
          break;
        case 52:
          sendToDcsBiosMessage("RIGHT_DDI_PB_06", "0");
          break;
        case 53:
          sendToDcsBiosMessage("APU_FIRE_BTN", "0");
          break;
        case 54:
          sendToDcsBiosMessage("RIGHT_FIRE_BTN", "0");
          sendToDcsBiosMessage("RIGHT_FIRE_BTN_COVER", "0");
          break;
        case 55:
          sendToDcsBiosMessage("AMPCD_PB_05", "0");
          break;
        case 56:
          sendToDcsBiosMessage("AMPCD_PB_20", "0");
          break;
        case 57:
          sendToDcsBiosMessage("AMPCD_PB_15", "0");
          break;
        case 58:
          sendToDcsBiosMessage("AMPCD_PB_10", "0");
          break;
        case 59:
          sendToDcsBiosMessage("UFC_1", "0");
          break;
        case 60:
          sendToDcsBiosMessage("UFC_2", "0");
          break;
        case 61:
          sendToDcsBiosMessage("UFC_3", "0");
          break;
        case 62:
          sendToDcsBiosMessage("UFC_OS2", "0");
          break;
        case 63:
          sendToDcsBiosMessage("UFC_AP", "0");
          break;
        case 64:
          sendToDcsBiosMessage("UFC_IFF", "0");
          break;
        case 65:
          sendToDcsBiosMessage("UFC_TCN", "0");
          break;
        case 66:
          sendToDcsBiosMessage("AMPCD_PB_04", "0");
          break;
        case 67:
          sendToDcsBiosMessage("AMPCD_PB_19", "0");
          break;
        case 68:
          sendToDcsBiosMessage("AMPCD_PB_14", "0");
          break;
        case 69:
          sendToDcsBiosMessage("AMPCD_PB_09", "0");
          break;
        case 70:
          sendToDcsBiosMessage("UFC_4", "0");
          break;
        case 71:
          sendToDcsBiosMessage("UFC_5", "0");
          break;
        case 72:
          sendToDcsBiosMessage("UFC_6", "0");
          break;
        case 73:
          sendToDcsBiosMessage("UFC_OS3", "0");
          break;
        case 74:
          sendToDcsBiosMessage("UFC_ONOFF", "0");
          break;
        case 75:
          sendToDcsBiosMessage("UFC_BCN", "0");
          break;
        case 76:
          sendToDcsBiosMessage("UFC_DL", "0");
          break;
        case 77:
          sendToDcsBiosMessage("AMPCD_PB_03", "0");
          break;
        case 78:
          sendToDcsBiosMessage("AMPCD_PB_18", "0");
          break;
        case 79:
          sendToDcsBiosMessage("AMPCD_PB_13", "0");
          break;
        case 80:
          sendToDcsBiosMessage("AMPCD_PB_08", "0");
          break;
        case 81:
          sendToDcsBiosMessage("UFC_7", "0");
          break;
        case 82:
          sendToDcsBiosMessage("UFC_8", "0");
          break;
        case 83:
          sendToDcsBiosMessage("UFC_9", "0");
          break;
        case 84:
          sendToDcsBiosMessage("UFC_OS4", "0");
          break;
        case 85:  // USED BELOW
          break;
        case 86:
          sendToDcsBiosMessage("CMSD_DISPENSE_SW", "1");
          break;
        case 87:  // EMC, IS THIS USED
          break;
        case 88:
          sendToDcsBiosMessage("AMPCD_PB_02", "0");
          break;
        case 89:
          sendToDcsBiosMessage("AMPCD_PB_17", "0");
          break;
        case 90:
          sendToDcsBiosMessage("AMPCD_PB_12", "0");
          break;
        case 91:
          sendToDcsBiosMessage("AMPCD_PB_07", "0");
          break;
        case 92:
          sendToDcsBiosMessage("UFC_CLR", "0");
          break;
        case 93:
          sendToDcsBiosMessage("UFC_0", "0");
          break;
        case 94:
          sendToDcsBiosMessage("UFC_ENT", "0");
          break;
        case 95:
          sendToDcsBiosMessage("UFC_OS5", "0");
          break;
        case 96:  // USED BELOW
          break;
        case 97:
          sendToDcsBiosMessage("CMSD_DISPENSE_SW", "1");
          break;
        case 98:  // EMC, IS THIS USED
          break;
        case 99:
          sendToDcsBiosMessage("AMPCD_PB_01", "0");
          break;
        case 100:
          sendToDcsBiosMessage("AMPCD_PB_16", "0");
          break;
        case 101:
          sendToDcsBiosMessage("AMPCD_PB_11", "0");
          break;
        case 102:
          sendToDcsBiosMessage("AMPCD_PB_06", "0");
          break;
        case 103:
          sendToDcsBiosMessage("UFC_ADF", "1");
          break;
        case 104:
          sendToDcsBiosMessage("UFC_IP", "0");
          break;
        case 105:
          sendToDcsBiosMessage("UFC_OS1", "0");
          break;
        case 106:
          sendToDcsBiosMessage("UFC_EMCON", "0");
          break;
        case 107:  // USED BELOW
          break;
        case 108:
          sendToDcsBiosMessage("AUX_REL_SW", "1");  // WIRED BACKWARDS IN MY PIT
          break;
        case 109:
          sendToDcsBiosMessage("SAI_TEST_BTN", "0");
          break;
        case 110:
          sendToDcsBiosMessage("AMPCD_GAIN_SW", "1");
          break;
        case 111:
          sendToDcsBiosMessage("AMPCD_CONT_SW", "1");
          break;
        case 112:
          sendToDcsBiosMessage("AMPCD_SYM_SW", "1");
          break;
        case 113:
          sendToDcsBiosMessage("AMPCD_NIGHT_DAY", "1");
          break;
        case 114:
          sendToDcsBiosMessage("UFC_ADF", "1");
          break;
        case 115:
          sendToDcsBiosMessage("HUD_VIDEO_BIT", "0");
          break;
        case 116:  //FA-18C_hornet/MASTER_MODE_AA
          sendToDcsBiosMessage("MASTER_MODE_AA", "0");
          HUD_STEPPER_FORWARD = false;
          break;
        case 117:  //FA-18C_hornet/FIRE_EXT_BTN
          sendToDcsBiosMessage("FIRE_EXT_BTN", "0");
          break;
        case 118:  // USED BELOW
          break;
        case 119:  //EMC, IS THIS USED
          sendToDcsBiosMessage("CMSD_JET_SEL_BTN", "0");
          break;
        case 120:
          sendToDcsBiosMessage("SAI_CAGE", "0");
          break;
        case 121:
          sendToDcsBiosMessage("AMPCD_GAIN_SW", "1");
          break;
        case 122:
          sendToDcsBiosMessage("AMPCD_CONT_SW", "1");
          break;
        case 123:
          sendToDcsBiosMessage("AMPCD_SYM_SW", "1");
          break;
        case 124:
          sendToDcsBiosMessage("AMPCD_NIGHT_DAY", "1");
          break;
        case 125:
          sendToDcsBiosMessage("RWR_BIT_BTN", "0");
          break;
        case 126:  // USED BELOW
          break;
        case 127:  //FA-18C_hornet/MASTER_MODE_AG
          sendToDcsBiosMessage("MASTER_MODE_AG", "0");
          HUD_STEPPER_REVERSE = false;
          break;
        case 128:

          // USED BELOW
          break;
        case 129:  // USED BELOW
          break;
        case 130:  //EMC, IS THIS USED
          break;
        case 131:  // NOT USED
          break;
        case 132:
          sendToDcsBiosMessage("AMPCD_BRT_CTL", "1");
          break;
        case 133:  // AMPCD, IS THIS USED
          break;
        case 134:  // AMPCD, IS THIS USED
          break;
        case 135:  // AMPCD, IS THIS USED
          break;
        case 136:
          sendToDcsBiosMessage("RWR_OFFSET_BTN", "0");
          break;
        case 137:  // USED BELOW
          break;
        case 138:
          sendToDcsBiosMessage("EMER_JETT_BTN", "0");
          break;
        case 139:
          sendToDcsBiosMessage("MASTER_ARM_SW", "0");
          break;
        case 140:                                   //FA-18C_hornet/HUD_ATT_SW
          sendToDcsBiosMessage("HUD_ATT_SW", "1");  //1 FOR OFF
          break;
        case 141:                                             //FA-18C_hornet/HUD_VIDEO_CONTROL_SW
          sendToDcsBiosMessage("HUD_VIDEO_CONTROL_SW", "1");  //1 FOR OFF
          break;
        case 142:                                       //FA-18C_hornet/HUD_SYM_REJ_SW
          sendToDcsBiosMessage("HUD_SYM_REJ_SW", "1");  //1 FOR OFF
          break;
        case 143:  //FA-18C_hornet/IFEI_MODE_BTN
          sendToDcsBiosMessage("IFEI_MODE_BTN", "0");
          break;
        case 144:  //FA-18C_hornet/IFEI_DWN_BTN
          sendToDcsBiosMessage("IFEI_DWN_BTN", "0");
          break;
        case 145:  //IFEI, IS THIS USED
          break;
        case 146:                                         //FA-18C_hornet/MODE_SELECTOR_SW
          sendToDcsBiosMessage("MODE_SELECTOR_SW", "1");  //1 FOR OFF
          break;
        case 147:
          sendToDcsBiosMessage("RWR_SPECIAL_BTN", "0");
          break;
        case 148:  // USED BELOW
          break;
        case 149:  //FA-18C_hornet/SPIN_RECOVERY_SW
          sendToDcsBiosMessage("SPIN_RECOVERY_SW", "0");
          //FA-18C_hornet/SPIN_RECOVERY_COVER
          sendToDcsBiosMessage("SPIN_RECOVERY_COVER", "0");
          break;
        case 150:  //FA-18C_hornet/IR_COOL_SW
          sendToDcsBiosMessage("IR_COOL_SW", "1");
          break;
        case 151:                                   //FA-18C_hornet/HUD_ATT_SW
          sendToDcsBiosMessage("HUD_ATT_SW", "1");  //1 FOR OFF
          break;
        case 152:                                             //FA-18C_hornet/HUD_VIDEO_CONTROL_SW
          sendToDcsBiosMessage("HUD_VIDEO_CONTROL_SW", "1");  //1 FOR OFF
          break;
        case 153:                                       //FA-18C_hornet/HUD_SYM_REJ_SW
          sendToDcsBiosMessage("HUD_SYM_REJ_SW", "1");  //1 FOR OFF
          break;
        case 154:  //FA-18C_hornet/IFEI_QTY_BTN
          sendToDcsBiosMessage("IFEI_QTY_BTN", "0");
          break;
        case 155:  //FA-18C_hornet/IFEI_ZONE_BTN
          sendToDcsBiosMessage("IFEI_ZONE_BTN", "0");
          break;
        case 156:                                             //FA-18C_hornet/SELECT_HMD_LDDI_RDDI
          sendToDcsBiosMessage("SELECT_HMD_LDDI_RDDI", "0");  // NEEDS WORK
          break;
        case 157:                                         //FA-18C_hornet/MODE_SELECTOR_SW
          sendToDcsBiosMessage("MODE_SELECTOR_SW", "1");  //1 FOR OFF
          break;
        case 158:
          sendToDcsBiosMessage("RWR_DISPLAY_BTN", "0");
          break;
        case 159:  // USED BELOW
          break;
        case 160:  //SPIN, IS THIS USED
          break;
        case 161:  ////FA-18C_hornet/IR_COOL_SW
          sendToDcsBiosMessage("IR_COOL_SW", "1");
          break;
        case 162:  //FA-18C_hornet/HUD_ALT_SW
          sendToDcsBiosMessage("HUD_ALT_SW", "0");
          break;
        case 163:  //HUD, IS THIS USED
          break;
        case 164:  //FA-18C_hornet/HUD_SYM_BRT_SELECT
          sendToDcsBiosMessage("HUD_SYM_BRT_SELECT", "0");
          break;
        case 165:  //FA-18C_hornet/IFEI_UP_BTN
          sendToDcsBiosMessage("IFEI_UP_BTN", "0");
          break;
        case 166:  //FA-18C_hornet/IFEI_ET_BTN
          sendToDcsBiosMessage("IFEI_ET_BTN", "0");
          break;
        case 167:
          sendToDcsBiosMessage("LEFT_DDI_HDG_SW", "1");
          break;
        case 168:
          sendToDcsBiosMessage("LEFT_DDI_HDG_SW", "1");
          break;
        case 169:
          // ######### PETE TO ADD LATCH #########
          break;
        case 170:  // USED BELOW
          break;
        case 171:  //FA-18C_hornet/SJ_CTR
          sendToDcsBiosMessage("SJ_CTR", "0");
          break;
        case 172:  //FA-18C_hornet/SJ_RI
          sendToDcsBiosMessage("SJ_RI", "0");
          break;
        case 173:  //FA-18C_hornet/SJ_LI
          sendToDcsBiosMessage("SJ_LI", "0");
          break;
        case 174:  //FA-18C_hornet/SJ_RO
          sendToDcsBiosMessage("SJ_RO", "0");
          break;
        case 175:  //FA-18C_hornet/SJ_LO
          sendToDcsBiosMessage("SJ_LO", "0");
          break;
        case 176:  // NOT USED
          break;
        case 177:  // NOT USED
          break;
        case 178:  // NOT USED
          break;
        case 179:  // NOT USED
          break;
        default:
          sendToDcsBiosMessage("LIGHTS_TEST_SW", "0");
          break;
      }
      break;
    case 1:
      switch (ind) {
        case 0:
          sendToDcsBiosMessage("LEFT_DDI_PB_05", "1");
          break;
        case 1:
          sendToDcsBiosMessage("LEFT_DDI_PB_20", "1");
          break;
        case 2:
          sendToDcsBiosMessage("LEFT_DDI_PB_15", "1");
          break;
        case 3:
          sendToDcsBiosMessage("LEFT_DDI_PB_10", "1");
          break;
        case 4:
          sendToDcsBiosMessage("LEFT_DDI_BRT_SELECT", "0");
          break;
        case 5:
          sendToDcsBiosMessage("RIGHT_DDI_PB_05", "1");
          break;
        case 6:
          sendToDcsBiosMessage("RIGHT_DDI_PB_20", "1");
          break;
        case 7:
          sendToDcsBiosMessage("RIGHT_DDI_PB_15", "1");
          break;
        case 8:
          sendToDcsBiosMessage("RIGHT_DDI_PB_10", "1");
          break;
        case 9:
          sendToDcsBiosMessage("RIGHT_DDI_BRT_SELECT", "0");
          break;
        case 10:
          sendToDcsBiosMessage("LEFT_DDI_CRS_SW", "2");
          break;
        case 11:
          sendToDcsBiosMessage("LEFT_DDI_PB_04", "1");
          break;
        case 12:
          sendToDcsBiosMessage("LEFT_DDI_PB_19", "1");
          break;
        case 13:
          sendToDcsBiosMessage("LEFT_DDI_PB_14", "1");
          break;
        case 14:
          sendToDcsBiosMessage("LEFT_DDI_PB_09", "1");
          break;
        case 15:
          sendToDcsBiosMessage("LEFT_DDI_BRT_SELECT", "1");
          break;
        case 16:
          sendToDcsBiosMessage("RIGHT_DDI_PB_04", "1");
          break;
        case 17:
          sendToDcsBiosMessage("RIGHT_DDI_PB_19", "1");
          break;
        case 18:
          sendToDcsBiosMessage("RIGHT_DDI_PB_14", "1");
          break;
        case 19:
          sendToDcsBiosMessage("RIGHT_DDI_PB_09", "1");
          break;
        case 20:
          sendToDcsBiosMessage("RIGHT_DDI_BRT_SELECT", "1");
          break;
        case 21:
          sendToDcsBiosMessage("LEFT_DDI_CRS_SW", "0");
          break;
        case 22:
          sendToDcsBiosMessage("LEFT_DDI_PB_03", "1");
          break;
        case 23:
          sendToDcsBiosMessage("LEFT_DDI_PB_18", "1");
          break;
        case 24:
          sendToDcsBiosMessage("LEFT_DDI_PB_13", "1");
          break;
        case 25:
          sendToDcsBiosMessage("LEFT_DDI_PB_08", "1");
          break;
        case 26:
          sendToDcsBiosMessage("LEFT_DDI_BRT_SELECT", "2");
          break;
        case 27:
          sendToDcsBiosMessage("RIGHT_DDI_PB_03", "1");
          break;
        case 28:
          sendToDcsBiosMessage("RIGHT_DDI_PB_18", "1");
          break;
        case 29:
          sendToDcsBiosMessage("RIGHT_DDI_PB_13", "1");
          break;
        case 30:
          sendToDcsBiosMessage("RIGHT_DDI_PB_08", "1");
          break;
        case 31:
          sendToDcsBiosMessage("RIGHT_DDI_BRT_SELECT", "2");
          break;
        case 32:
          sendToDcsBiosMessage("UFC_ILS", "1");
          break;
        case 33:
          sendToDcsBiosMessage("LEFT_DDI_PB_02", "1");
          break;
        case 34:
          sendToDcsBiosMessage("LEFT_DDI_PB_17", "1");
          break;
        case 35:
          sendToDcsBiosMessage("LEFT_DDI_PB_12", "1");
          break;
        case 36:
          sendToDcsBiosMessage("LEFT_DDI_PB_07", "1");
          break;
        case 37:
          sendToDcsBiosMessage("UFC_COMM1_PULL", "1");
          //sendToDcsBiosMessage("UFC_COMM2_PULL", "1");
          break;
        case 38:
          sendToDcsBiosMessage("RIGHT_DDI_PB_02", "1");
          break;
        case 39:
          sendToDcsBiosMessage("RIGHT_DDI_PB_17", "1");
          break;
        case 40:
          sendToDcsBiosMessage("RIGHT_DDI_PB_12", "1");
          break;
        case 41:
          sendToDcsBiosMessage("RIGHT_DDI_PB_07", "1");
          break;
        case 42:
          sendToDcsBiosMessage("MASTER_CAUTION_RESET_SW", "1");
          break;
        case 43:
          sendToDcsBiosMessage("LEFT_FIRE_BTN_COVER", "1");
          LFBCFollowupTask = true;
          timeLFBCOn = millis() + ToggleSwitchCoverMoveTime;
          break;
        case 44:
          sendToDcsBiosMessage("LEFT_DDI_PB_01", "1");
          break;
        case 45:
          sendToDcsBiosMessage("LEFT_DDI_PB_16", "1");
          break;
        case 46:
          sendToDcsBiosMessage("LEFT_DDI_PB_11", "1");
          break;
        case 47:
          sendToDcsBiosMessage("LEFT_DDI_PB_06", "1");
          break;
        case 48:
          sendToDcsBiosMessage("UFC_COMM2_PULL", "1");
          break;
        case 49:
          sendToDcsBiosMessage("RIGHT_DDI_PB_01", "1");
          break;
        case 50:
          sendToDcsBiosMessage("RIGHT_DDI_PB_16", "1");
          break;
        case 51:
          sendToDcsBiosMessage("RIGHT_DDI_PB_11", "1");
          break;
        case 52:
          sendToDcsBiosMessage("RIGHT_DDI_PB_06", "1");
          break;
        case 53:
          sendToDcsBiosMessage("APU_FIRE_BTN", "1");
          break;
        case 54:
          sendToDcsBiosMessage("RIGHT_FIRE_BTN_COVER", "1");
          RFBCFollowupTask = true;
          timeRFBCOn = millis() + ToggleSwitchCoverMoveTime;
          break;
        case 55:
          sendToDcsBiosMessage("AMPCD_PB_05", "1");
          break;
        case 56:
          sendToDcsBiosMessage("AMPCD_PB_20", "1");
          break;
        case 57:
          sendToDcsBiosMessage("AMPCD_PB_15", "1");
          break;
        case 58:
          sendToDcsBiosMessage("AMPCD_PB_10", "1");
          break;
        case 59:
          sendToDcsBiosMessage("UFC_1", "1");
          break;
        case 60:
          sendToDcsBiosMessage("UFC_2", "1");
          break;
        case 61:
          sendToDcsBiosMessage("UFC_3", "1");
          break;
        case 62:
          sendToDcsBiosMessage("UFC_OS2", "1");
          break;
        case 63:
          sendToDcsBiosMessage("UFC_AP", "1");
          break;
        case 64:
          sendToDcsBiosMessage("UFC_IFF", "1");
          break;
        case 65:
          sendToDcsBiosMessage("UFC_TCN", "1");
          break;
        case 66:
          sendToDcsBiosMessage("AMPCD_PB_04", "1");
          break;
        case 67:
          sendToDcsBiosMessage("AMPCD_PB_19", "1");
          break;
        case 68:
          sendToDcsBiosMessage("AMPCD_PB_14", "1");
          break;
        case 69:
          sendToDcsBiosMessage("AMPCD_PB_09", "1");
          break;
        case 70:
          sendToDcsBiosMessage("UFC_4", "1");
          break;
        case 71:
          sendToDcsBiosMessage("UFC_5", "1");
          break;
        case 72:
          sendToDcsBiosMessage("UFC_6", "1");
          break;
        case 73:
          sendToDcsBiosMessage("UFC_OS3", "1");
          break;
        case 74:
          sendToDcsBiosMessage("UFC_ONOFF", "1");
          break;
        case 75:
          sendToDcsBiosMessage("UFC_BCN", "1");
          break;
        case 76:
          sendToDcsBiosMessage("UFC_DL", "1");
          break;
        case 77:
          sendToDcsBiosMessage("AMPCD_PB_03", "1");
          break;
        case 78:
          sendToDcsBiosMessage("AMPCD_PB_18", "1");
          break;
        case 79:
          sendToDcsBiosMessage("AMPCD_PB_13", "1");
          break;
        case 80:
          sendToDcsBiosMessage("AMPCD_PB_08", "1");
          break;
        case 81:
          sendToDcsBiosMessage("UFC_7", "1");
          break;
        case 82:
          sendToDcsBiosMessage("UFC_8", "1");
          break;
        case 83:
          sendToDcsBiosMessage("UFC_9", "1");
          break;
        case 84:
          sendToDcsBiosMessage("UFC_OS4", "1");
          break;
        case 85:
          sendToDcsBiosMessage("ECM_MODE_SW", "0");  // OFF
          break;
        case 86:
          sendToDcsBiosMessage("CMSD_DISPENSE_SW", "0");
          break;
        case 87:  // ECM, IS THIS USED
          break;
        case 88:
          sendToDcsBiosMessage("AMPCD_PB_02", "1");
          break;
        case 89:
          sendToDcsBiosMessage("AMPCD_PB_17", "1");
          break;
        case 90:
          sendToDcsBiosMessage("AMPCD_PB_12", "1");
          break;
        case 91:
          sendToDcsBiosMessage("AMPCD_PB_07", "1");
          break;
        case 92:
          sendToDcsBiosMessage("UFC_CLR", "1");
          break;
        case 93:
          sendToDcsBiosMessage("UFC_0", "1");
          break;
        case 94:
          sendToDcsBiosMessage("UFC_ENT", "1");
          break;
        case 95:
          sendToDcsBiosMessage("UFC_OS5", "2");
          break;
        case 96:
          sendToDcsBiosMessage("ECM_MODE_SW", "1");  // STBY
          break;
        case 97:
          sendToDcsBiosMessage("CMSD_DISPENSE_SW", "2");
          break;
        case 98:  // EMC, IS THIS USED
          break;
        case 99:
          sendToDcsBiosMessage("AMPCD_PB_01", "1");
          break;
        case 100:
          sendToDcsBiosMessage("AMPCD_PB_16", "1");
          break;
        case 101:
          sendToDcsBiosMessage("AMPCD_PB_11", "1");
          break;
        case 102:
          sendToDcsBiosMessage("AMPCD_PB_06", "1");
          break;
        case 103:
          sendToDcsBiosMessage("UFC_ADF", "2");
          break;
        case 104:
          sendToDcsBiosMessage("UFC_IP", "1");
          break;
        case 105:
          sendToDcsBiosMessage("UFC_OS1", "1");
          break;
        case 106:
          sendToDcsBiosMessage("UFC_EMCON", "1");
          break;
        case 107:
          sendToDcsBiosMessage("ECM_MODE_SW", "2");  // BIT
          break;
        case 108:
          sendToDcsBiosMessage("AUX_REL_SW", "0");  // WIRED BACKWARDS IN MY PIT
          break;
        case 109:
          sendToDcsBiosMessage("SAI_TEST_BTN", "1");
          break;
        case 110:
          sendToDcsBiosMessage("AMPCD_GAIN_SW", "2");
          break;
        case 111:
          sendToDcsBiosMessage("AMPCD_CONT_SW", "2");
          break;
        case 112:
          sendToDcsBiosMessage("AMPCD_SYM_SW", "2");
          break;
        case 113:
          sendToDcsBiosMessage("AMPCD_NIGHT_DAY", "0");
          break;
        case 114:
          sendToDcsBiosMessage("UFC_ADF", "0");
          break;
        case 115:
          sendToDcsBiosMessage("HUD_VIDEO_BIT", "1");
          break;
        case 116:
          sendToDcsBiosMessage("MASTER_MODE_AA", "1");
          HUD_STEPPER_FORWARD = true;
          break;
        case 117:
          sendToDcsBiosMessage("FIRE_EXT_BTN", "1");
          break;
        case 118:
          sendToDcsBiosMessage("ECM_MODE_SW", "3");  // REC
          break;
        case 119:  // EMC,
          sendToDcsBiosMessage("CMSD_JET_SEL_BTN", "1");
          break;
        case 120:
          sendToDcsBiosMessage("SAI_CAGE", "1");
          break;
        case 121:
          sendToDcsBiosMessage("AMPCD_GAIN_SW", "0");
          break;
        case 122:
          sendToDcsBiosMessage("AMPCD_CONT_SW", "0");
          break;
        case 123:
          sendToDcsBiosMessage("AMPCD_SYM_SW", "0");
          break;
        case 124:
          sendToDcsBiosMessage("AMPCD_NIGHT_DAY", "2");
          break;
        case 125:
          sendToDcsBiosMessage("RWR_BIT_BTN", "1");
          break;
        case 126:
          sendToDcsBiosMessage("RWR_DIS_TYPE_SW", "0");  // "U"
          // CHECK INDIVIDUAL ASSIGNMENTS PER PIT
          break;
        case 127:
          sendToDcsBiosMessage("MASTER_MODE_AG", "1");
          HUD_STEPPER_REVERSE = true;
          break;
        case 128:

          break;
        case 129:
          sendToDcsBiosMessage("ECM_MODE_SW", "4");  // XMIT
          break;
        case 130:  // EMC, IS THIS USED
          break;
        case 131:  // NOT USED
          break;
        case 132:
          sendToDcsBiosMessage("AMPCD_BRT_CTL", "0");
          break;
        case 133:  // AMPCD, IS THIS USED
          break;
        case 134:  // AMPCD, IS THIS USED
          break;
        case 135:  // AMPCD, IS THIS USED
          break;
        case 136:
          sendToDcsBiosMessage("RWR_OFFSET_BTN", "1");
          break;
        case 137:
          sendToDcsBiosMessage("RWR_DIS_TYPE_SW", "1");  // "A"
          // CHECK INDIVIDUAL ASSIGNMENTS PER PIT
          break;
        case 138:
          sendToDcsBiosMessage("EMER_JETT_BTN", "1");
          break;
        case 139:
          sendToDcsBiosMessage("MASTER_ARM_SW", "1");
          break;
        case 140:                                   //FA-18C_hornet/HUD_ATT_SW
          sendToDcsBiosMessage("HUD_ATT_SW", "2");  //1 FOR OFF
          break;
        case 141:                                             //FA-18C_hornet/HUD_VIDEO_CONTROL_SW
          sendToDcsBiosMessage("HUD_VIDEO_CONTROL_SW", "0");  //1 FOR OFF
          break;
        case 142:                                       //FA-18C_hornet/HUD_SYM_REJ_SW
          sendToDcsBiosMessage("HUD_SYM_REJ_SW", "0");  //1 FOR OFF
          break;
        case 143:  //FA-18C_hornet/IFEI_MODE_BTN
          sendToDcsBiosMessage("IFEI_MODE_BTN", "1");
          break;
        case 144:  //FA-18C_hornet/IFEI_DWN_BTN
          sendToDcsBiosMessage("IFEI_DWN_BTN", "1");
          break;
        case 145:  // IFEI, IS THIS USED
          break;
        case 146:                                         //FA-18C_hornet/MODE_SELECTOR_SW
          sendToDcsBiosMessage("MODE_SELECTOR_SW", "0");  //1 FOR OFF
          break;
        case 147:
          sendToDcsBiosMessage("RWR_SPECIAL_BTN", "1");
          break;
        case 148:
          sendToDcsBiosMessage("RWR_DIS_TYPE_SW", "2");  // "I"
          // CHECK INDIVIDUAL ASSIGNMENTS PER PIT
          break;
        case 149:  //FA-18C_hornet/SPIN_RECOVERY_SW
          sendToDcsBiosMessage("SPIN_RECOVERY_COVER", "1");
          SpinFollowupTask = true;
          timeSpinOn = millis() + ToggleSwitchCoverMoveTime;
          //FA-18C_hornet/SPIN_RECOVERY_COVER
          break;
        case 150:  //FA-18C_hornet/IR_COOL_SW
          sendToDcsBiosMessage("IR_COOL_SW", "0");
          break;
        case 151:                                   //FA-18C_hornet/HUD_ATT_SW
          sendToDcsBiosMessage("HUD_ATT_SW", "0");  //1 FOR OFF
          break;
        case 152:                                             //FA-18C_hornet/HUD_VIDEO_CONTROL_SW
          sendToDcsBiosMessage("HUD_VIDEO_CONTROL_SW", "2");  //1 FOR OFF
          break;
        case 153:                                       //FA-18C_hornet/HUD_SYM_REJ_SW
          sendToDcsBiosMessage("HUD_SYM_REJ_SW", "2");  //1 FOR OFF
          break;
        case 154:  //FA-18C_hornet/IFEI_QTY_BTN
          sendToDcsBiosMessage("IFEI_QTY_BTN", "1");
          break;
        case 155:  //FA-18C_hornet/IFEI_ZONE_BTN
          sendToDcsBiosMessage("IFEI_ZONE_BTN", "1");
          break;
        case 156:                                             //FA-18C_hornet/SELECT_HMD_LDDI_RDDI
          sendToDcsBiosMessage("SELECT_HMD_LDDI_RDDI", "1");  // NEEDS WORK
          break;
        case 157:                                         //FA-18C_hornet/MODE_SELECTOR_SW
          sendToDcsBiosMessage("MODE_SELECTOR_SW", "2");  //1 FOR OFF
          break;
        case 158:
          sendToDcsBiosMessage("RWR_DISPLAY_BTN", "1");
          break;
        case 159:
          sendToDcsBiosMessage("RWR_DIS_TYPE_SW", "3");  // "N"
          // CHECK INDIVIDUAL ASSIGNMENTS PER PIT
          break;
        case 160:  // SPIN, IS THIS USED
          break;
        case 161:  //FA-18C_hornet/IR_COOL_SW
          sendToDcsBiosMessage("IR_COOL_SW", "2");
        case 162:  //FA-18C_hornet/HUD_ALT_SW
          sendToDcsBiosMessage("HUD_ALT_SW", "1");
          break;
        case 163:  // HUD, IS THIS USED
          break;
        case 164:  //FA-18C_hornet/HUD_SYM_BRT_SELECT
          sendToDcsBiosMessage("HUD_SYM_BRT_SELECT", "1");
          break;
        case 165:  //FA-18C_hornet/IFEI_UP_BTN
          sendToDcsBiosMessage("IFEI_UP_BTN", "1");
          break;
        case 166:  //FA-18C_hornet/IFEI_ET_BTN
          sendToDcsBiosMessage("IFEI_ET_BTN", "1");
          break;
        case 167:
          sendToDcsBiosMessage("LEFT_DDI_HDG_SW", "2");
          break;
        case 168:
          sendToDcsBiosMessage("LEFT_DDI_HDG_SW", "0");
          break;
        case 169:

          if (RWR_POWER_BUTTON_STATE == true) {
            sendToDcsBiosMessage("RWR_POWER_BTN", "0");
          } else {
            sendToDcsBiosMessage("RWR_POWER_BTN", "1");
          }
          // ######### PETE TO ADD LATCH #########
          break;
        case 170:
          sendToDcsBiosMessage("RWR_DIS_TYPE_SW", "4");  // "F"
          // CHECK INDIVIDUAL ASSIGNMENTS PER PIT
          break;
        case 171:
          sendToDcsBiosMessage("SJ_CTR", "1");
          break;
        case 172:
          sendToDcsBiosMessage("SJ_RI", "1");
          break;
        case 173:
          sendToDcsBiosMessage("SJ_LI", "1");
          break;
        case 174:
          sendToDcsBiosMessage("SJ_RO", "1");
          break;
        case 175:
          sendToDcsBiosMessage("SJ_LO", "1");
          break;
        case 176:  // NOT USED
          break;
        case 177:  // NOT USED
          break;
        case 178:  // NOT USED
          break;
        case 179:  // NOT USED
          break;

        default:
          sendToDcsBiosMessage("LIGHTS_TEST_SW", "1");
          break;
          break;
      }
  }
}




// LEFT DDI
DcsBios::Potentiometer leftDdiBrtCtl("LEFT_DDI_BRT_CTL", A0);
DcsBios::Potentiometer leftDdiContCtl("LEFT_DDI_CONT_CTL", A1);

// RIGHT DDI
DcsBios::Potentiometer rightDdiBrtCtl("RIGHT_DDI_BRT_CTL", A2);
DcsBios::Potentiometer rightDdiContCtl("RIGHT_DDI_CONT_CTL", A3);

// SPIN (HMD) KNOB
DcsBios::Potentiometer hmdOffBrt("HMD_OFF_BRT", A4);

// HUD ANALOG INPUTS
DcsBios::Potentiometer hudSymBrt("HUD_SYM_BRT", A5);
// 20220227 Bug in FP DCS-BIOS stops indiexer updates if AoA indexer below 50% - sending over IP
DcsBios::Potentiometer hudAoaIndexer("HUD_AOA_INDEXER", A6);
DcsBios::Potentiometer hudBlackLvl("HUD_BLACK_LVL", A7);
DcsBios::Potentiometer hudBalance("HUD_BALANCE", A8);



void loop() {

  if (millis() >= NEXT_STATUS_TOGGLE_TIMER) {
    GREEN_LED_STATE = !GREEN_LED_STATE;
    RED_LED_STATE = !GREEN_LED_STATE;

    digitalWrite(GREEN_STATUS_LED_PORT, GREEN_LED_STATE);
    digitalWrite(RED_STATUS_LED_PORT, RED_LED_STATE);

    // if (RED_LED_STATE == true) {
    //   MASTER_ARM_AG(true);
    // } else {
    //   MASTER_ARM_AG(false);
    // }

    NEXT_STATUS_TOGGLE_TIMER = millis() + FLASH_TIME;
  }

  if (DCSBIOS_In_Use == 1) DcsBios::loop();

  updateSteppers();

  if ((HUD_STEPPER_FORWARD == true) || (HUD_STEPPER_REVERSE == true)) {
    if (HUD_STEPPER_ENABLED == false) {
      HUD_STEPPER_ENABLED = true;
      SendDebug("Enabling Hud Stepper");
      digitalWrite(AllstepperEnablePin, false);
      if (HUD_STEPPER_FORWARD == true) {
        HUDStepper.move(20000);
      } else {
        HUDStepper.move(-20000);
      }
    }

  } else if ((HUD_STEPPER_FORWARD == false) && (HUD_STEPPER_REVERSE == false)) {
    if (HUD_STEPPER_ENABLED == true) {
      HUD_STEPPER_ENABLED = false;
      digitalWrite(AllstepperEnablePin, false);
      HUDStepper.setCurrentPosition(0);
      SendDebug("Disabling Hud Stepper");
    }
  }


  //turn off all rows first
  for (int rowid = 0; rowid < 16; rowid++) {
    //turn on the current row
    // why differentiate? rows

    updateSteppers();

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
  // Handle Switches with Safety covers
  //SPIN COVER
  if (SpinFollowupTask == true) {
    if (millis() >= timeSpinOn) {
      sendToDcsBiosMessage("SPIN_RECOVERY_SW", "1");
      SpinFollowupTask = false;
    }
  }
  // LEFT FIRE BUTTON COVER
  if (LFBCFollowupTask == true) {
    if (millis() >= timeLFBCOn) {
      sendToDcsBiosMessage("LEFT_FIRE_BTN", "1");
      LFBCFollowupTask = false;
    }
  }
  // RIGHT FIRE BUTTON COVER
  if (RFBCFollowupTask == true) {
    if (millis() >= timeRFBCOn) {
      sendToDcsBiosMessage("RIGHT_FIRE_BTN", "1");
      RFBCFollowupTask = false;
    }
  }
  // END CODE
}
