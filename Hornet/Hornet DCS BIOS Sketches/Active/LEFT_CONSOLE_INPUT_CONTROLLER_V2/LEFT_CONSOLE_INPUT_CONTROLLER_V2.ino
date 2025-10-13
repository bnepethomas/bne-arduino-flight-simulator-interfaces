////////////////////---||||||||||********||||||||||---\\\\\\\\\\\\\\\\\\\\
//||               FUNCTION = LEFT CONSOLE INPUT                      ||\\
//||              LOCATION IN THE PIT = LEFTHAND SIDE CONSOLE         ||\\
//||            ARDUINO PROCESSOR TYPE = Arduino Mega 2560            ||\\
//||      ARDUINO CHIP SERIAL NUMBER = SN - xxxxxxxxxxxxxxxxxxxx      ||\\
//||                    CONNECTED COM PORT = COM 3                    ||\\
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
EthernetUDP debugUDP;
char packetBuffer[1000];     //buffer to store the incoming data
char outpacketBuffer[1000];  //buffer to store the outgoing data
String DebugString = "";
String BoardName = "Hornet Left Console Combined ";


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

#define RED_STATUS_LED_PORT 20
#define GREEN_STATUS_LED_PORT 21
#define Check_LED_R 20
#define Check_LED_G 21
#define FLASH_TIME 300

unsigned long NEXT_STATUS_TOGGLE_TIMER = 0;
bool GREEN_LED_STATE = false;
bool RED_LED_STATE = false;
unsigned long timeSinceRedLedChanged = 0;


// ################################### BEGIN LIGHTING ##################################
#define BACK_LIGHTS 11
#define LANDINIG_GEAR_HANDLE_LIGHT 12

void onConsoleIntLtChange(unsigned int newValue) {
  SendDebug("Console Lights : " + String(newValue));
  analogWrite(BACK_LIGHTS, map(newValue, 0, 65535, 0, 255));
}
DcsBios::IntegerBuffer consoleIntLtBuffer(0x7558, 0xffff, 0, onConsoleIntLtChange);

// ################################### END LIGHTING ##################################


//************************************************RUDDER TRIM************************************************
long dcsMillis;

#define RudderTrimPotUpdateInterval 100
long NextRudderTrimPositionUpdate = 0;
#define RudderTrimPotAnalogInput A14
int RudderTrimPosition = 0;
int RudderTrimLastPosition = 0;
bool TrimMotorActive = false;
bool TrimMotorPositionFound = false;

#define RudderTrimMotorA 6
#define RudderTrimMotorB 7
#define RudderTrimMotorEn 37

#define MinDesiredPosition 485  // SET VALUES TO THE CENTRE POSITION
#define MaxDesiredPosition 490
#define RudderTrimMotorTimeoutInterval 15000
long RudderTrimMotorTimeout = 0;
//************************************************RUDDER TRIM************************************************


int apuMag = 18;
int apuSw;
int apuLED = 13;
int engLeftMag = 19;
int engRightMag = 19;
int engLeftSw;
int engRightSw;
bool apuMagState = false;
bool aircraftPowerAvailable = false;
bool leftStarterAvailable = true;
bool rightStarterAvailable = true;
int apuBrightness;

void onVoltUChange(unsigned int newValue) {
  if (newValue > 1000) {
    aircraftPowerAvailable = true;
    return aircraftPowerAvailable;
  } else {
    aircraftPowerAvailable = false;
    return aircraftPowerAvailable;
  }
}
DcsBios::IntegerBuffer voltUBuffer(0x753c, 0xffff, 0, onVoltUChange);

/* ==================================================================
    APU Panel - Left Engine RPM
      during start sequence Left Gen comes online when RPM Value exceeds 60% RPM.
      Starter is unavailable to latch when RPM < 60 to prevent continuing start sequence
      when engine is running, or recently flamed out with RPM above 60%
   ================================================================== */
void onIfeiRpmLChange(char* newValue) {
  String currentRPM = String(newValue);
  int gameRPM = currentRPM.toInt();
  int starterOut = 60;

  if (gameRPM > starterOut && leftStarterAvailable == true) {
    digitalWrite(engLeftMag, LOW);
    leftStarterAvailable = false;
    return leftStarterAvailable;
  } else {
    leftStarterAvailable = true;
    return leftStarterAvailable;
  }
}
DcsBios::StringBuffer<3> ifeiRpmLBuffer(0x749e, onIfeiRpmLChange);

/* ==================================================================
    APU Panel - Right Engine RPM
      during start sequence Right Gen comes online when RPM Value exceeds 60% RPM.
      Starter is unavailable to latch when RPM < 60 to prevent continuing start sequence
      when engine is running, or recently flamed out with RPM above 60%
   ================================================================== */
void onIfeiRpmRChange(char* newValue) {
  String currentRPM = String(newValue);
  int gameRPM = currentRPM.toInt();
  int starterOut = 60;

  if (gameRPM > starterOut && rightStarterAvailable == true) {
    digitalWrite(engRightMag, LOW);
    rightStarterAvailable = false;
    return rightStarterAvailable;
  } else {
    rightStarterAvailable = false;
    return rightStarterAvailable;
  }
}
DcsBios::StringBuffer<3> ifeiRpmRBuffer(0x74a2, onIfeiRpmRChange);


DcsBios::Switch2Pos apuControlSw("APU_CONTROL_SW", apuSw);

void onApuControlSwChange(unsigned int newValue) {
  int gameState = newValue;

  if (aircraftPowerAvailable == true) {
    switch (gameState) {
      case 0:
        if (apuMagState != false) {
          digitalWrite(apuMag, LOW);
          // digitalWrite(apuMag, HIGH);
          apuMagState = false;
          return apuMagState;
        }
        break;
      case 1:
        if (apuMagState != true) {
          digitalWrite(apuMag, HIGH);
          //  digitalWrite(apuMag, LOW);
          apuMagState = true;
          return apuMagState;
        }
        break;
      default:
        digitalWrite(apuMag, LOW);
        //  digitalWrite(apuMag, HIGH);
        apuMagState = false;
        return apuMagState;
        break;
    }
  }
}
DcsBios::IntegerBuffer apuControlSwBuffer(0x74c2, 0x0100, 8, onApuControlSwChange);

DcsBios::LED apuReadyLt(0x74c2, 0x0800, apuLED);

void onApuReadyLtChange(unsigned int newValue) {

  bool apuAvailable = newValue;

  if (apuMagState == true && apuAvailable == false) {
    digitalWrite(apuMag, LOW);
    // digitalWrite(apuMag, HIGH);
    delay(100);
    apuMagState = false;
    return apuMagState;
  }
}
DcsBios::IntegerBuffer apuReadyLtBuffer(0x74c2, 0x0800, 11, onApuReadyLtChange);


/* ==================================================================
    APU Panel - Engine Crank Switch
   ================================================================== */
DcsBios::Switch3Pos engineCrankSw("ENGINE_CRANK_SW", engRightSw, engLeftSw);


void shutdownEngineCrankMagSwitch() {
  SendDebug(" Engine Crank to Center/off");
  digitalWrite(engRightMag, LOW);
  digitalWrite(engLeftMag, LOW);
}

void onEngineCrankSwChange(unsigned int newValue) {

  // This does not appear to capture switch being powered off
  // Added additional check in Switch Case Statement
  int gameState = newValue;
  int engLeftSwPos = engLeftSw;
  int engRightSwPos = engRightSw;

  if (newValue == 1) {
    SendDebug(" Engine Crank to Center/off");
    digitalWrite(engRightMag, LOW);
    digitalWrite(engLeftMag, LOW);
  }

  if (aircraftPowerAvailable == true) {
    switch (gameState) {
      case 0:
        if (leftStarterAvailable == true) {
          digitalWrite(engRightMag, LOW);
          digitalWrite(engLeftMag, HIGH);
        }
        break;

      case 1:
        SendDebug(" Engine Crank to Center/off");
        digitalWrite(engRightMag, LOW);
        digitalWrite(engLeftMag, LOW);
        break;

      case 2:
        if (rightStarterAvailable == true) {
          digitalWrite(engRightMag, HIGH);
          digitalWrite(engLeftMag, LOW);
        }
        break;
    }
  }
}
DcsBios::IntegerBuffer engineCrankSwBuffer(0x74c2, 0x0600, 9, onEngineCrankSwChange);

//************************************************RUDDER TRIM************************************************
void StopMotor() {
  digitalWrite(RudderTrimMotorA, 0);
  digitalWrite(RudderTrimMotorB, 0);
  digitalWrite(RudderTrimMotorEn, 0);
  TrimMotorActive = false;
}

void StartMotorClockwise() {
  digitalWrite(RudderTrimMotorA, 0);
  digitalWrite(RudderTrimMotorB, 1);
  digitalWrite(RudderTrimMotorEn, 1);
  TrimMotorActive = true;
}

void StartMotorCounterClockwise() {
  digitalWrite(RudderTrimMotorA, 1);
  digitalWrite(RudderTrimMotorB, 0);
  digitalWrite(RudderTrimMotorEn, 1);
  TrimMotorActive = true;
}

void SetTrimPosition() {
  // For testing - setting Trim to Center Position

  RudderTrimMotorTimeout = millis() + RudderTrimMotorTimeoutInterval;
  TrimMotorPositionFound = false;

  RudderTrimPosition = analogRead(RudderTrimPotAnalogInput);
  if ((RudderTrimPosition >= MinDesiredPosition) && (RudderTrimPosition <= MaxDesiredPosition)) {
    // Within Bounds Stop the motor
    TrimMotorPositionFound = true;
    StopMotor();
  } else {
    if (RudderTrimPosition <= MinDesiredPosition) {
      StartMotorClockwise();
    }
    if (RudderTrimPosition >= MaxDesiredPosition) {
      StartMotorCounterClockwise();
    }
  }
}

void CheckTrimPosition() {

  if (TrimMotorActive == true) {
    if ((millis() <= RudderTrimMotorTimeout) && (TrimMotorPositionFound == false)) {
      RudderTrimPosition = analogRead(RudderTrimPotAnalogInput);
      if (RudderTrimPosition != RudderTrimLastPosition) {
        RudderTrimLastPosition = RudderTrimPosition;
      }
      if ((RudderTrimPosition >= MinDesiredPosition) && (RudderTrimPosition <= MaxDesiredPosition)) {
        // Within Bounds Stop the motor
        TrimMotorPositionFound = true;
        StopMotor();
      } else {
        if (RudderTrimPosition <= MinDesiredPosition) {
          StartMotorClockwise();
        }
        if (RudderTrimPosition >= MaxDesiredPosition) {
          StartMotorCounterClockwise();
        }
      }
    }
  }
}

//************************************************RUDDER TRIM************************************************

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


char stringind[5];
String outString;

bool ParkingFollowupTaskPart1 = false;
bool ParkingFollowupTaskPart2 = false;
long timeParkingBrakeRelease = 0;
const int ParkingBrakeMoveTime = 500;


bool FCSGainFollowupTask = false;
long timeFCSGainOn = 0;
const int ToggleSwitchCoverMoveTime = 500;

bool GenTieFollowupTask = false;
long timeGenTieOn = 0;

// CONSOLE LIGHTS

void onConsolesDimmerChange(unsigned int newValue) {
  analogWrite(11, map(newValue, 0, 65535, 0, 150));
}
DcsBios::IntegerBuffer consolesDimmerBuffer(0x7544, 0xffff, 0, onConsolesDimmerChange);

DcsBios::LED landingGearHandleLt(0x747e, 0x0800, LANDINIG_GEAR_HANDLE_LIGHT);

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
  pinMode(BACK_LIGHTS, OUTPUT);
  pinMode(LANDINIG_GEAR_HANDLE_LIGHT, OUTPUT);
  analogWrite(BACK_LIGHTS, 255);
  analogWrite(LANDINIG_GEAR_HANDLE_LIGHT, 255);
  delay(3000);

  SendDebug("Dimming Leds");
  for (int Local_Brightness = 255; Local_Brightness >= 0; Local_Brightness--) {
    analogWrite(LANDINIG_GEAR_HANDLE_LIGHT, Local_Brightness);
    // analogWrite(NAVIGATION_LIGHTS, Local_Brightness);
    // analogWrite(FORMATION_LIGHTS, Local_Brightness);
    analogWrite(BACK_LIGHTS, Local_Brightness);
    // analogWrite(FLOOD_LIGHTS, Local_Brightness);
    // SendDebug("Led Brightness " + String(Local_Brightness));
    delay(15);
  }

#define BrightnessWhileRunningSetup 128
  //analogWrite(LANDINIG_GEAR_HANDLE_LIGHT, BrightnessWhileRunningSetup);
  // analogWrite(NAVIGATION_LIGHTS, BrightnessWhileRunningSetup);
  // analogWrite(FORMATION_LIGHTS, BrightnessWhileRunningSetup);
  analogWrite(BACK_LIGHTS, BrightnessWhileRunningSetup);
  // analogWrite(FLOOD_LIGHTS, BrightnessWhileRunningSetup);


  // Motors and Mag Switches
  pinMode(apuMag, OUTPUT);
  pinMode(engLeftMag, OUTPUT);
  pinMode(engRightMag, OUTPUT);
  pinMode(RudderTrimMotorA, OUTPUT);
  pinMode(RudderTrimMotorB, OUTPUT);
  digitalWrite(RudderTrimMotorA, 0);
  digitalWrite(RudderTrimMotorB, 0);
  pinMode(RudderTrimMotorEn, OUTPUT);
  digitalWrite(RudderTrimMotorEn, 0);

  // SendDebug("Motor Clockwise for 2 sec");
  // StartMotorClockwise();
  // delay(2000);
  // StopMotor();
  // SendDebug("Motor Counter Clockwise for 2 sec");
  // StartMotorCounterClockwise();
  // delay(2000);
  // StopMotor();
  // SendDebug("Motor stopped");
  // delay(5000);




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


  DcsBios::setup();
  // SetTrimPosition();

#define BrightnessPostSetup 65

  // analogWrite(STROBE_LIGHTS, BrightnessPostSetup);
  // analogWrite(NAVIGATION_LIGHTS, BrightnessPostSetup);
  // analogWrite(FORMATION_LIGHTS, BrightnessPostSetup);
  analogWrite(BACK_LIGHTS, BrightnessPostSetup);
  // analogWrite(FLOOD_LIGHTS, BrightnessPostSetup);

  SendDebug(BoardName + " End Setup");
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
          //outString = outString +  "1";
          if (DCSBIOS_In_Use == 1) CreateDcsBiosMessage(ind, 1);
          // if (Ethernet_In_Use == 1) SendIPMessage(ind, 1);
          // if (MSFS_In_Use == 1) SendMSFSMessage(ind, 1);
        } else {
          // outString = outString + "0";
          if (DCSBIOS_In_Use == 1) CreateDcsBiosMessage(ind, 0);
          // if (Ethernet_In_Use == 1) SendIPMessage(ind, 0);
          // if (MSFS_In_Use == 1) SendMSFSMessage(ind, 0);
        }


        prevjoyReport.button[ind] = joyReport.button[ind];

        SendDebug(BoardName + String(ind) + ":" + String(joyReport.button[ind]));
      }
    }
}


void CreateDcsBiosMessage(int ind, int state) {


  switch (state) {

    // RELEASE
    case 0:
      switch (ind) {
        case 0:  //ILS - ROTARY NO RELEASE
          break;
        case 1:  //ILS - ROTARY NO RELEASE
          break;
        case 2:
          sendToDcsBiosMessage("COM_COMM_RELAY_SW", "1");
          break;
        case 3:
          break;
        case 4:
          break;
        case 5:
          break;
        // RELEASE
        case 6:
          break;
        case 7:
          break;
        case 8:
          break;
        case 9:
          sendToDcsBiosMessage("STROBE_SW", "1");  //STROBE "BRT"
          break;
        case 10:
          sendToDcsBiosMessage("INT_WNG_TANK_SW", "0");  //INHIBIT - PRESSED
          break;
        case 11:  //ILS - ROTARY NO RELEASE
          break;
        case 12:  //ILS - ROTARY NO RELEASE
          break;
        case 13:
          sendToDcsBiosMessage("COM_COMM_RELAY_SW", "1");
          break;
        // RELEASE
        case 14:
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
        case 20:
          sendToDcsBiosMessage("STROBE_SW", "1");  // STROBE "NORMAL"
          break;
        // RELEASE
        case 21:
          // Gen Tie
          sendToDcsBiosMessage("GEN_TIE_SW", "0");
          sendToDcsBiosMessage("GEN_TIE_COVER", "0");
          break;
        case 22:  //ILS - ROTARY NO RELEASE
          break;
        case 23:  //ILS - ROTARY NO RELEASE
          break;
        case 24:
          sendToDcsBiosMessage("COM_CRYPTO_SW", "1");
          break;
        case 25:
          //BM ADDED
          sendToDcsBiosMessage("SEL_JETT_BTN", "0");  // "JETT" BUTTON
          break;
        case 26:
          // BM CODE
          sendToDcsBiosMessage("LDG_TAXI_SW", "1");  // LIGHTS "ON"
          break;
        // RELEASE
        case 27:
          // BM ADDED "SELECT JETT KNOB"
          //No Relese Required
          break;
        case 28:
          sendToDcsBiosMessage("CB_FCS_CHAN1", "1");

          break;
        case 29:
          sendToDcsBiosMessage("CB_SPD_BRK", "1");
          break;
        case 30:

          break;
        case 31:
          sendToDcsBiosMessage("FCS_RESET_BTN", "0");
          break;
        case 32:
          sendToDcsBiosMessage("TO_TRIM_BTN", "0");
          break;
        case 33:  //ILS - ROTARY NO RELEASE
          break;
        case 34:  //ILS - ROTARY NO RELEASE
          break;
        case 35:
          sendToDcsBiosMessage("COM_CRYPTO_SW", "1");
          break;
        case 36:
          sendToDcsBiosMessage("LAUNCH_BAR_SW", "0");
          break;
        case 37:
          sendToDcsBiosMessage("EMERGENCY_PARKING_BRAKE_PULL", "1");
          // delay(5);
          // sendToDcsBiosMessage("EMERGENCY_PARKING_BRAKE_ROTATE", "0");

          // RELEASE
          break;
          // BM ADDED "SELECT JETT KNOB"
          //No Relese Required
          break;
        case 39:
          sendToDcsBiosMessage("CMSD_DISPENSE_BTN", "0");
          break;
        case 40:
          sendToDcsBiosMessage("CB_FCS_CHAN2", "1");
          break;
        case 41:
          sendToDcsBiosMessage("CB_LAUNCH_BAR", "1");
          break;
        case 42:
          // Release is shifting toggle to Norm Position
          // Cover Up = 1
          // Cover Down = 0
          // 0 Gain Switch in Normal Position
          // 1 Gain in Switch Override position
          sendToDcsBiosMessage("GAIN_SWITCH", "0");
          sendToDcsBiosMessage("GAIN_SWITCH_COVER", "0");
          break;
        case 43:
        // RELEASE
        case 44:  //ILS - ROTARY NO RELEASE
          break;
        case 45:  //ILS - ROTARY NO RELEASE
          break;
        case 46:
          sendToDcsBiosMessage("COM_COMM_G_XMT_SW", "1");
          break;
        case 47:
          sendToDcsBiosMessage("EMERGENCY_PARKING_BRAKE_ROTATE", "1");
          break;
        case 48:
          //BM CODE
          sendToDcsBiosMessage("HOOK_BYPASS_SW", "0");  // HOOK "FIELD"
          break;
        case 49:
          // BM ADDED "SELECT JETT KNOB"
          //No Relese Required
          break;
        case 50:
          sendToDcsBiosMessage("ENGINE_CRANK_SW", "1");  // ENG CRANK OFF
          engLeftSw = LOW;
          engRightSw = LOW;
          shutdownEngineCrankMagSwitch();
          break;
        case 51:
          sendToDcsBiosMessage("APU_CONTROL_SW", "0");  // APU "ON"
          apuSw = LOW;
          break;
        case 52:
          // sendToDcsBiosMessage("MC_SW", "1");
          sendToDcsBiosMessage("HYD_ISOLATE_OVERRIDE_SW", "0");
          break;
        case 53:
          sendToDcsBiosMessage("MC_SW", "1");
          break;
        // RELEASE
        case 54:
          sendToDcsBiosMessage("OBOGS_SW", "0");  // OBOGS "ON"
          break;
        case 55:  //ILS - ROTARY NO RELEASE
          break;
        case 56:  //ILS - ROTARY NO RELEASE
          break;
        case 57:
          sendToDcsBiosMessage("COM_COMM_G_XMT_SW", "1");
          break;
        // RELEASE
        case 58:
          sendToDcsBiosMessage("FLAP_SW", "1");  // FLAPS "AUTO"
          break;
        case 59:
          sendToDcsBiosMessage("EMERGENCY_PARKING_BRAKE_ROTATE", "2");

          timeParkingBrakeRelease = millis() + ParkingBrakeMoveTime;
          ParkingFollowupTaskPart1 = true;
          break;
        case 60:
          // BM ADDED "SELECT JETT KNOB"
          //No Relese Required
          break;
        case 61:
          sendToDcsBiosMessage("ENGINE_CRANK_SW", "1");  // ENG CRANK OFF
          engLeftSw = LOW;
          engRightSw = LOW;
          shutdownEngineCrankMagSwitch();
          break;
          // RELEASE
        case 62:
          break;
        case 63:

          break;
        case 64:
          sendToDcsBiosMessage("MC_SW", "1");
          break;
        case 65:
          sendToDcsBiosMessage("OXY_FLOW", "0");  // OXY FLOW "ON"
          break;
        case 66:  //ILS - ROTARY NO RELEASE
          break;
        case 67:  //ILS - ROTARY NO RELEASE
          break;
        case 68:
          sendToDcsBiosMessage("COM_IFF_MASTER_SW", "1");
          break;
        case 69:
          sendToDcsBiosMessage("FLAP_SW", "1");  // FLAPS "FULL"
          break;
        case 70:
          // BM CODE
          sendToDcsBiosMessage("ANTI_SKID_SW", "0");  //X0--
          break;
        case 71:
          // BM ADDED "SELECT JETT KNOB"
          //No Relese Required
          break;
        case 72:
          break;
        case 73:
          break;
        case 74:
          break;
        case 75:
          break;
        case 76:
          break;
        case 77:  //ILS - ROTARY NO RELEASE
          break;
        case 78:  //ILS - ROTARY NO RELEASE
          break;
        case 79:
          sendToDcsBiosMessage("COM_IFF_MODE4_SW", "1");
          break;
        case 80:
          sendToDcsBiosMessage("SEAT_HEIGHT_SW", "1");
          break;
        case 81:
          sendToDcsBiosMessage("EJECTION_HANDLE_SW", "0");
          break;
        case 82:
          sendToDcsBiosMessage("FIRE_TEST_SW", "1");
          break;
        case 83:
          break;
        case 84:
          break;
        case 85:
          break;
        case 86:
          break;
        case 87:
          break;
        case 88:  //ILS - ROTARY NO RELEASE
          break;
        case 89:  //ILS - ROTARY NO RELEASE
          break;
        case 90:
          sendToDcsBiosMessage("COM_ILS_UFC_MAN_SW", "1");
          break;
        case 91:
          sendToDcsBiosMessage("SEAT_HEIGHT_SW", "1");
          break;
          // RELEASE
        case 92:
          sendToDcsBiosMessage("EJECTION_SEAT_ARMED", "1");
          break;
        case 93:
          sendToDcsBiosMessage("FIRE_TEST_SW", "1");
          break;
        case 94:
          break;
        case 95:
          break;
        case 96:
          break;
        case 97:
          break;
        case 98:
          break;
        case 99:  //ILS - ROTARY NO RELEASE
          break;
        case 100:  //ILS - ROTARY NO RELEASE
          break;
        case 101:
          sendToDcsBiosMessage("COM_IFF_MODE4_SW", "1");
          break;
        case 102:
          break;
        case 103:
          sendToDcsBiosMessage("EJECTION_SEAT_MNL_OVRD", "1");
          break;
        case 104:
          break;
        case 105:
          break;
        case 106:
          break;
        case 107:
          break;
        case 108:
          break;
        case 109:
          break;
        case 110:
          sendToDcsBiosMessage("PROBE_SW", "1");  // PROBE "EXTEND"
          break;
        case 111:
          sendToDcsBiosMessage("EXT_WNG_TANK_SW", "1");  // WING "ORIDE"
          break;
        case 112:
          sendToDcsBiosMessage("EXT_CNT_TANK_SW", "1");  // TANKS CTR "ORIDE"
          break;
        case 113:
          sendToDcsBiosMessage("FUEL_DUMP_SW", "0");  // DUMP "ON"
          break;
        case 114:
          sendToDcsBiosMessage("GEAR_DOWNLOCK_OVERRIDE_BTN", "0");
          break;
        case 115:
          sendToDcsBiosMessage("GEAR_SILENCE_BTN", "0");
          break;
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
        case 121:
          sendToDcsBiosMessage("PROBE_SW", "1");  // PROBE "EMERG EXTD"
          break;
        case 122:
          sendToDcsBiosMessage("EXT_WNG_TANK_SW", "1");  //WING "STOP"
          break;
        case 123:
          sendToDcsBiosMessage("EXT_CNT_TANK_SW", "1");  // CTR "STOP"
          break;
        case 125:
          sendToDcsBiosMessage("GEAR_LEVER", "1");
          break;
        case 126:
          sendToDcsBiosMessage("EMERGENCY_GEAR_ROTATE", "1");
          break;
        case 127:
          break;
        case 128:
          break;
        case 129:
          break;
        case 130:
          break;
        case 131:
          break;
        case 132:
          sendToDcsBiosMessage("COMM1_ANT_SELECT_SW", "1");
          break;
        case 133:
          sendToDcsBiosMessage("IFF_ANT_SELECT_SW", "1");
          break;
        case 134:
          break;
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
        case 141:
          break;
        case 142:
          break;
        case 143:
          sendToDcsBiosMessage("COMM1_ANT_SELECT_SW", "1");
          break;
        case 144:
          sendToDcsBiosMessage("IFF_ANT_SELECT_SW", "1");
          break;
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
          sendToDcsBiosMessage("COM_ILS_CHANNEL_SW", "1");  // COMMS PANEL ILS ROTARY SW"
          break;
        case 1:
          sendToDcsBiosMessage("COM_ILS_CHANNEL_SW", "2");  // COMMS PANEL ILS ROTARY SW
          break;
        case 2:
          sendToDcsBiosMessage("COM_COMM_RELAY_SW", "0");
          break;
        case 3:
          break;
        case 4:
          break;
        case 5:
          break;
        case 6:
          break;
        case 7:
          break;
        case 8:
          break;
        case 9:
          sendToDcsBiosMessage("STROBE_SW", "2");
          break;
        case 10:
          sendToDcsBiosMessage("INT_WNG_TANK_SW", "1");
          break;
        // PRESS - CLOSE
        case 11:
          sendToDcsBiosMessage("COM_ILS_CHANNEL_SW", "3");  // COMMS PANEL ILS ROTARY SW
          break;
        case 12:
          sendToDcsBiosMessage("COM_ILS_CHANNEL_SW", "4");  // COMMS PANEL ILS ROTARY SW
          break;
        case 13:
          sendToDcsBiosMessage("COM_COMM_RELAY_SW", "2");
          break;
        case 14:
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
        case 20:
          sendToDcsBiosMessage("STROBE_SW", "0");
          break;
        // PRESS - CLOSE
        case 21:
          // Gen Tie
          sendToDcsBiosMessage("GEN_TIE_COVER", "1");
          GenTieFollowupTask = true;
          timeGenTieOn = millis() + ToggleSwitchCoverMoveTime;
          break;
        case 22:
          sendToDcsBiosMessage("COM_ILS_CHANNEL_SW", "5");  // COMMS PANEL ILS ROTARY SW
          break;
        case 23:
          sendToDcsBiosMessage("COM_ILS_CHANNEL_SW", "6");  // COMMS PANEL ILS ROTARY SW
          break;
        case 24:
          sendToDcsBiosMessage("COM_CRYPTO_SW", "0");
          break;
        case 25:
          //BM ADDED
          sendToDcsBiosMessage("SEL_JETT_BTN", "1");  // "JETT" BUTTON
          break;
        case 26:
          sendToDcsBiosMessage("LDG_TAXI_SW", "0");
          // PT CODE  sendToDcsBiosMessage("LAUNCH_BAR_SW", "0");
          break;
        case 27:
          sendToDcsBiosMessage("SEL_JETT_KNOB", "0");  // KNOB "STORES"
          break;
        case 28:
          sendToDcsBiosMessage("CB_FCS_CHAN1", "0");
          break;
        case 29:
          sendToDcsBiosMessage("CB_SPD_BRK", "0");
          break;
        case 30:

          break;
        // PRESS - CLOSE
        case 31:
          sendToDcsBiosMessage("FCS_RESET_BTN", "1");
          break;
        case 32:
          // sendToDcsBiosMessage("TO_TRIM_BTN", "1");
          // SetTrimPosition();
          break;
        case 33:
          // sendToDcsBiosMessage("COM_ILS_CHANNEL_SW", "7");  // COMMS PANEL ILS ROTARY SW
          break;
        case 34:
          sendToDcsBiosMessage("COM_ILS_CHANNEL_SW", "8");  // COMMS PANEL ILS ROTARY SW
          break;
        case 35:
          sendToDcsBiosMessage("COM_CRYPTO_SW", "2");
          break;
        case 36:
          sendToDcsBiosMessage("LAUNCH_BAR_SW", "1");
          break;
        case 37:
          sendToDcsBiosMessage("EMERGENCY_PARKING_BRAKE_PULL", "0");
          break;
          // PRESS - CLOSE
        case 38:
          // BM ADDED "SELECT JETT KNOB"
          sendToDcsBiosMessage("SEL_JETT_KNOB", "1");  // KNOB "RACK LCHR"
          break;
        case 39:
          sendToDcsBiosMessage("CMSD_DISPENSE_BTN", "1");
          break;
        case 40:
          sendToDcsBiosMessage("CB_FCS_CHAN2", "0");
          break;
        // PRESS - CLOSE
        case 41:
          sendToDcsBiosMessage("CB_LAUNCH_BAR", "0");
          break;
        case 42:
          // Press is shifting toggle to Override Position
          sendToDcsBiosMessage("GAIN_SWITCH_COVER", "1");
          FCSGainFollowupTask = true;
          timeFCSGainOn = millis() + ToggleSwitchCoverMoveTime;
          break;
        case 43:
          break;
        case 44:
          sendToDcsBiosMessage("COM_ILS_CHANNEL_SW", "7");  // COMMS PANEL ILS ROTARY SW
          break;
        case 45:
          sendToDcsBiosMessage("COM_ILS_CHANNEL_SW", "10");  // COMMS PANEL ILS ROTARY SW
          break;
        case 46:
          sendToDcsBiosMessage("COM_COMM_G_XMT_SW", "0");
          break;
          // PRESS - CLOSE
        case 47:
          sendToDcsBiosMessage("EMERGENCY_PARKING_BRAKE_ROTATE", "0");
          break;
        case 48:
          //BM CODE
          sendToDcsBiosMessage("HOOK_BYPASS_SW", "1");  // HOOK "FIELD"
          break;
        case 49:
          // BM ADDED "SELECT JETT KNOB"
          sendToDcsBiosMessage("SEL_JETT_KNOB", "2");  // KNOB "RFUS MSL"
          break;
        case 50:
          sendToDcsBiosMessage("ENGINE_CRANK_SW", "2");  //RIGHT
          engLeftSw = HIGH;
          engRightSw = HIGH;
          break;
        // PRESS - CLOSE
        case 51:
          sendToDcsBiosMessage("APU_CONTROL_SW", "1");
          apuSw = HIGH;
          break;
        case 52:
          sendToDcsBiosMessage("HYD_ISOLATE_OVERRIDE_SW", "1");
          // sendToDcsBiosMessage("MC_SW", "0");
          break;
        case 53:
          sendToDcsBiosMessage("MC_SW", "2");
          break;
        // CLOSE
        case 54:
          sendToDcsBiosMessage("OBOGS_SW", "1");
          break;
        case 55:
          sendToDcsBiosMessage("COM_ILS_CHANNEL_SW", "9");  // COMMS PANEL ILS ROTARY SW
          break;
        case 56:
          sendToDcsBiosMessage("COM_ILS_CHANNEL_SW", "12");  // COMMS PANEL ILS ROTARY SW
          break;
        case 57:
          sendToDcsBiosMessage("COM_COMM_G_XMT_SW", "2");
          break;
        case 58:
          sendToDcsBiosMessage("FLAP_SW", "2");  // FLAPS "AUTO"
          break;
        case 59:
          sendToDcsBiosMessage("EMERGENCY_PARKING_BRAKE_PULL", "0");
          break;
        case 60:
          // BM ADDED "SELECT JETT KNOB"
          sendToDcsBiosMessage("SEL_JETT_KNOB", "3");  // KNOB "SAFE"
          break;
        // PRESS - CLOSE
        case 61:
          sendToDcsBiosMessage("ENGINE_CRANK_SW", "0");  //LEFT
          engLeftSw = HIGH;
          engRightSw = HIGH;
          break;
        case 62:
          break;
        case 63:
          sendToDcsBiosMessage("MC_SW", "2");
          break;
        case 64:
          sendToDcsBiosMessage("MC_SW", "0");
          break;
        case 65:
          sendToDcsBiosMessage("OXY_FLOW", "65535");
          break;
          // PRESS - CLOSE
        case 66:
          sendToDcsBiosMessage("COM_ILS_CHANNEL_SW", "11");  // COMMS PANEL ILS ROTARY SW
          break;
        case 67:
          sendToDcsBiosMessage("COM_ILS_CHANNEL_SW", "14");  // COMMS PANEL ILS ROTARY SW
          break;
        // CLOSE
        case 68:
          sendToDcsBiosMessage("COM_IFF_MASTER_SW", "0");
          break;
        case 69:
          sendToDcsBiosMessage("FLAP_SW", "0");  // FLAPS "FULL"
          break;
        case 70:
          // BM CODE
          sendToDcsBiosMessage("ANTI_SKID_SW", "1");  //X1
          break;
        // PRESS - CLOSE
        case 71:
          // BM ADDED "SELECT JETT KNOB"
          sendToDcsBiosMessage("SEL_JETT_KNOB", "4");  // KNOB "LFUS MSL"
          break;
        case 72:
          break;
        case 73:
          break;
        case 74:
          break;
        case 75:
          break;
        case 76:
          break;
        case 77:
          sendToDcsBiosMessage("COM_ILS_CHANNEL_SW", "13");  // COMMS PANEL ILS ROTARY SW
          break;
        case 78:
          sendToDcsBiosMessage("COM_ILS_CHANNEL_SW", "16");  // COMMS PANEL ILS ROTARY SW
          break;
        case 79:
          sendToDcsBiosMessage("COM_IFF_MODE4_SW", "0");  //"OFF"
          break;
        case 80:
          sendToDcsBiosMessage("SEAT_HEIGHT_SW", "0");
          break;
          // PRESS - CLOSE
        case 81:
          sendToDcsBiosMessage("EJECTION_HANDLE_SW", "1");
          break;
        case 82:
          sendToDcsBiosMessage("FIRE_TEST_SW", "0");
          break;
        case 83:
          break;
        case 84:
          break;
        case 85:
          break;
        case 86:
          break;
        case 87:
          break;
        case 88:
          sendToDcsBiosMessage("COM_ILS_CHANNEL_SW", "15");  // COMMS PANEL ILS ROTARY SW
          break;
        case 89:
          sendToDcsBiosMessage("COM_ILS_CHANNEL_SW", "18");  // COMMS PANEL ILS ROTARY SW
          break;
        case 90:
          sendToDcsBiosMessage("COM_ILS_UFC_MAN_SW", "0");
          break;
        // PRESS - CLOSE
        case 91:
          sendToDcsBiosMessage("SEAT_HEIGHT_SW", "2");
          break;
        case 92:
          sendToDcsBiosMessage("EJECTION_SEAT_ARMED", "0");
          break;
        case 93:
          sendToDcsBiosMessage("FIRE_TEST_SW", "2");
          break;
        case 94:
          break;
        case 95:
          break;
        case 96:
          break;
        case 97:
          break;
        case 98:
          break;
        case 99:
          sendToDcsBiosMessage("COM_ILS_CHANNEL_SW", "17");  // COMMS PANEL ILS ROTARY SW
          break;
        case 100:
          sendToDcsBiosMessage("COM_ILS_CHANNEL_SW", "19");  // COMMS PANEL ILS ROTARY SW
          break;
        // PRESS - CLOSE
        case 101:
          sendToDcsBiosMessage("COM_IFF_MODE4_SW", "2");  // "DIS/AUD"
          break;
        case 102:
          break;
        case 103:
          sendToDcsBiosMessage("EJECTION_SEAT_MNL_OVRD", "0");
          break;
        case 104:
          break;
        case 105:
          break;
        case 106:
          break;
        case 107:
          break;
        case 108:
          break;
        case 109:
          break;
        case 110:
          sendToDcsBiosMessage("PROBE_SW", "2");
          break;
        // PRESS - CLOSE
        case 111:
          sendToDcsBiosMessage("EXT_WNG_TANK_SW", "2");
          break;
        case 112:
          sendToDcsBiosMessage("EXT_CNT_TANK_SW", "2");
          break;
        case 113:
          sendToDcsBiosMessage("FUEL_DUMP_SW", "1");
          break;
        case 114:
          sendToDcsBiosMessage("GEAR_DOWNLOCK_OVERRIDE_BTN", "1");
          break;
        case 115:
          sendToDcsBiosMessage("GEAR_SILENCE_BTN", "1");
          break;
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
          sendToDcsBiosMessage("PROBE_SW", "0");
          break;
        case 122:
          sendToDcsBiosMessage("EXT_WNG_TANK_SW", "0");
          break;
        case 123:
          sendToDcsBiosMessage("EXT_CNT_TANK_SW", "0");
          break;
        case 124:
          break;
        case 125:
          sendToDcsBiosMessage("GEAR_LEVER", "0");
          break;
        case 126:
          sendToDcsBiosMessage("EMERGENCY_GEAR_ROTATE", "2");
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
          sendToDcsBiosMessage("COMM1_ANT_SELECT_SW", "2");
          break;
        case 133:
          sendToDcsBiosMessage("IFF_ANT_SELECT_SW", "2");
          break;
        case 134:
          break;
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
          sendToDcsBiosMessage("COMM1_ANT_SELECT_SW", "0");
          break;
        case 144:
          sendToDcsBiosMessage("IFF_ANT_SELECT_SW", "0");
          break;
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




// Ext Lights Pots
DcsBios::PotentiometerEWMA<5, 128, 5> formationDimmer("FORMATION_DIMMER", A8);
DcsBios::PotentiometerEWMA<5, 128, 5> positionDimmer("POSITION_DIMMER", A15);

// RUDDER TRIM
DcsBios::PotentiometerEWMA<5, 128, 5> rudTrim("RUD_TRIM", A14);

// Com Aux
DcsBios::Potentiometer comAux("COM_AUX", A12);
DcsBios::Potentiometer comIcs("COM_ICS", A1);
DcsBios::Potentiometer comMidsA("COM_MIDS_A", A3);
DcsBios::Potentiometer comMidsB("COM_MIDS_B", A4);
DcsBios::Potentiometer comRwr("COM_RWR", A2);
DcsBios::Potentiometer comTacan("COM_TACAN", A5);
DcsBios::Potentiometer comVox("COM_VOX", A0);
DcsBios::Potentiometer comWpn("COM_WPN", A13);




void loop() {

  if (millis() >= NEXT_STATUS_TOGGLE_TIMER) {
    GREEN_LED_STATE = !GREEN_LED_STATE;
    RED_LED_STATE = !GREEN_LED_STATE;
    digitalWrite(GREEN_STATUS_LED_PORT, GREEN_LED_STATE);
    digitalWrite(RED_STATUS_LED_PORT, RED_LED_STATE);
    NEXT_STATUS_TOGGLE_TIMER = millis() + FLASH_TIME;
  }


  DcsBios::loop();

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


  // Parking Brake
  if (ParkingFollowupTaskPart1 == true) {
    if (millis() >= timeParkingBrakeRelease) {
      SendDebug("Clearing Park Brake Part 1");
      sendToDcsBiosMessage("EMERGENCY_PARKING_BRAKE_PULL", "1");
      ParkingFollowupTaskPart1 = false;
      ParkingFollowupTaskPart2 = true;
      timeParkingBrakeRelease = millis() + ParkingBrakeMoveTime;
    }
  }

  if (ParkingFollowupTaskPart2 == true) {
    if (millis() >= timeParkingBrakeRelease) {
      SendDebug("Clearing Park Brake Part 2");
      sendToDcsBiosMessage("EMERGENCY_PARKING_BRAKE_ROTATE", "1");
      ParkingFollowupTaskPart2 = false;
    }
  }

  // Handle Switches with Safety covers
  if (FCSGainFollowupTask == true) {
    if (millis() >= timeFCSGainOn) {
      sendToDcsBiosMessage("GAIN_SWITCH", "1");
      FCSGainFollowupTask = false;
    }
  }

  if (GenTieFollowupTask == true) {
    if (millis() >= timeGenTieOn) {
      sendToDcsBiosMessage("GEN_TIE_SW", "1");
      GenTieFollowupTask = false;
    }
  }
  CheckTrimPosition();
  if (millis() >= NextRudderTrimPositionUpdate) {
    NextRudderTrimPositionUpdate = millis() + RudderTrimPotUpdateInterval;
    RudderTrimPosition = analogRead(RudderTrimPotAnalogInput);
    if (RudderTrimPosition != RudderTrimLastPosition) {
      RudderTrimLastPosition = RudderTrimPosition;
    }
  }
}
