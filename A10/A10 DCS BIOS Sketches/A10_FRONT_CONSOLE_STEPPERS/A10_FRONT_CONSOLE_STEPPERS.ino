

////////////////////---||||||||||********||||||||||---\\\\\\\\\\\\\\\\\\\\
//||               FUNCTION = A10 FORWARD STEPPER CONTROLLER          ||\\
//||              LOCATION IN THE PIT = FRONT CENTER                  ||\\
//||            ARDUINO PROCESSOR TYPE = Arduino Mega 2560            ||\\
//||      ARDUINO CHIP SERIAL NUMBER = SN -                           ||\\
//||                    CONNECTED COM PORT = COM 5                    ||\\
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


#define DCSBIOS_IRQ_SERIAL
#include <DcsBios.h>


// ###################################### Begin Ethernet Related #############################
#include <SPI.h>
#include <Ethernet.h>
#include <EthernetUdp.h>

#define EthernetStartupDelay 500
#define ES1_RESET_PIN 53

String BoardName = "A10 Forward Steppers";

// These local Mac and IP Address will be reassigned early in startup based on
// the device ID as set by address pins
byte mac[] = { 0xA8, 0x61, 0x0A, 0x65, 0x83, 0x03 };
String sMac = "A8:61:0A:65:83:03";
IPAddress ip(172, 16, 1, 101);
String strMyIP = "172.16.1.101";

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

// Used to Distinguish between the left, front, and right inputs
// Left=0, Front=1, Right=2
#define INPUT_MODULE_NUMBER 2

void SendDebug(String MessageToSend) {
  if ((Reflector_In_Use == 1) && (Ethernet_In_Use == 1)) {
    udp.beginPacket(reflectorIP, reflectorport);
    udp.print(MessageToSend);
    udp.endPacket();
  }
}
// ###################################### End Ethernet Related #############################

#define RED_STATUS_LED_PORT 12
#define GREEN_STATUS_LED_PORT 13
#define Check_LED_R 12
#define Check_LED_G 13

#define FLASH_TIME 300

unsigned long NEXT_STATUS_TOGGLE_TIMER = 0;
bool GREEN_LED_STATE = false;
bool RED_LED_STATE = false;
unsigned long timeSinceRedLedChanged = 0;






unsigned long currentMillis = 0;
unsigned long previousMillis = 0;



// Note Pin 4 and Pin 10 cannot be used for other purposes.
//Arduino communicates with both the W5500 and SD card using the SPI bus (through the ICSP header).
//This is on digital pins 10, 11, 12, and 13 on the Uno and pins 50, 51, and 52 on the Mega.
//On both boards, pin 10 is used to select the W5500 and pin 4 for the SD card. These pins cannot be used for general I/O.
//On the Mega, the hardware SS pin, 53, is not used to select either the W5500 or the SD card,
//but it must be kept as an output or the SPI interface won't work.

#define BACKLIGHTING 8

// ########################## BEGIN STEPPERS ########################################
#include <AccelStepper.h>

#define STEPPER_MAX_SPEED 9000
#define STEPPER_ZERO_SEEK_SPEED 600
#define STEPPER_ACCELERATION 9000

#define AllstepperEnablePin  56

#define VSIstepPin 46
#define VSIdirectionPin 48
#define VSIoffset 130

#define ALTstepPin 42
#define ALTdirectionPin 44
#define ALTzeroSensePin 54
 

#define SpeedCurrentstepPin 34
#define SpeedCurrentdirectionPin 36

#define SpeedMaxstepPin 38
#define SpeedMaxdirectionPin 40

#define COIL_FLAPS_A 2
#define COIL_FLAPS_B 3
#define COIL_FLAPS_C 4
#define COIL_FLAPS_D 5

#define STEPS 10080
#define STEPS 315 * 16       // The 16 is the default divisors when no pins are tied together on the driver module \
                            // For an unmodified Vid series there are 315 steps
#define DUAL_STEPS 315 * 16  // The Dual stepper seems to have fewer steps between stops
#define FLAPS_STEP 315 * 2   // As flaps is direct driven don't need a multiplier
AccelStepper VSIstepper(AccelStepper::DRIVER, VSIstepPin, VSIdirectionPin);
AccelStepper ALTstepper(AccelStepper::DRIVER, ALTstepPin, ALTdirectionPin);
AccelStepper SpeedCurrentstepper(AccelStepper::DRIVER, SpeedCurrentstepPin, SpeedCurrentdirectionPin);
AccelStepper SpeedMaxstepper(AccelStepper::DRIVER, SpeedMaxstepPin, SpeedMaxdirectionPin);
AccelStepper FlapsStepper(AccelStepper::FULL4WIRE, COIL_FLAPS_A, COIL_FLAPS_B, COIL_FLAPS_C, COIL_FLAPS_D);
// ########################### END STEPPERS #########################################


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



  if (DCSBIOS_In_Use == 1) DcsBios::setup();


  SendDebug("LAMP AND LED TEST START");
  pinMode(BACKLIGHTING, OUTPUT);
  digitalWrite(BACKLIGHTING, true);



  SendDebug("STEPPER INITIALISATION STARTED");

  pinMode(AllstepperEnablePin, OUTPUT);
  pinMode(ALTzeroSensePin, INPUT);

  VSIstepper.setMaxSpeed(STEPPER_MAX_SPEED);
  VSIstepper.setAcceleration(STEPPER_ACCELERATION);
  ALTstepper.setMaxSpeed(STEPPER_MAX_SPEED);
  ALTstepper.setAcceleration(STEPPER_ACCELERATION);
  SpeedCurrentstepper.setMaxSpeed(STEPPER_MAX_SPEED);
  SpeedCurrentstepper.setAcceleration(STEPPER_ACCELERATION);
  SpeedMaxstepper.setMaxSpeed(STEPPER_MAX_SPEED);
  SpeedMaxstepper.setAcceleration(STEPPER_ACCELERATION);
  FlapsStepper.setMaxSpeed(STEPPER_MAX_SPEED);
  FlapsStepper.setAcceleration(STEPPER_ACCELERATION);


  digitalWrite(AllstepperEnablePin, false);

  // ################# Start VSI Startup #########################
  SendDebug("Start VSI");

  VSIstepper.runToNewPosition(-STEPS * 1.1);
  VSIstepper.setCurrentPosition(0);

  for (int i = 1; i <= 1; i++) {
    SendDebug("Loop :" + String(i));
    VSIstepper.runToNewPosition(STEPS);
    delay(200);
    VSIstepper.runToNewPosition(0);
    delay(200);
  }

  // Move VSI to zero position and set
  VSIstepper.runToNewPosition((STEPS / 2) - VSIoffset);
  VSIstepper.setCurrentPosition(0);
  SendDebug("End VSI");
  // ################# End VSI Startup #########################


  // ################# Start ALT Startup #########################
  SendDebug("Start ALT");
  for (int i = 1; i <= 1; i++) {
    SendDebug("Loop :" + String(i));
    ALTstepper.moveTo(-STEPS * 2);
    while (ALTstepper.distanceToGo() != 0) {
      if (digitalRead(ALTzeroSensePin) != true) {
        SendDebug("Found Alt Zero Position");
        ALTstepper.setCurrentPosition(0);
        break;
      }
      ALTstepper.run();
      
    }
    delay(500);
    SendDebug("Send Alt Round 40 times");
    long SendAAltForATrip = 5760 * 3;
    // 5760 steps per loop
    ALTstepper.runToNewPosition(SendAAltForATrip);
    delay(200);
    SendDebug("Return Alt to 0");
    ALTstepper.runToNewPosition(0);
  }
  // Move ALT to zero position - need to monitor zero sense



  SendDebug("End ALT");
  // ################# End ALT Startup #########################

  // ################# Start Speed Current Startup #########################
  SendDebug("Start SpeedCurrentstepper");
  SpeedCurrentstepper.runToNewPosition(-DUAL_STEPS * 1.1);
  SpeedCurrentstepper.setCurrentPosition(0);

  for (int i = 1; i <= 1; i++) {
    SendDebug("Loop :" + String(i));
    SpeedCurrentstepper.runToNewPosition(DUAL_STEPS * 1);
    delay(200);
    SpeedCurrentstepper.runToNewPosition(0);
    delay(200);
  }
  SendDebug("End SpeedCurrentstepper");
  //  ################ #End Speed Current Startup######################## #


  // ################# Start Speed Max Startup #########################
  SendDebug("Start SpeedMaxstepper");
  SpeedMaxstepper.runToNewPosition(-DUAL_STEPS * 1.1);
  SpeedMaxstepper.setCurrentPosition(0);
  for (int i = 1; i <= 1; i++) {
    SendDebug("Loop :" + String(i));
    SpeedMaxstepper.runToNewPosition(DUAL_STEPS * 1);
    delay(200);
    SpeedMaxstepper.runToNewPosition(0);
    delay(200);
  }
  SpeedMaxstepper.runToNewPosition((DUAL_STEPS * 0.95));
  SendDebug("End SpeedMaxstepper");
  //  ################# End Speed Max Startup #########################


  // ################# Start Flaps Startup #########################
  SendDebug("Start FlapsStepper");
  FlapsStepper.runToNewPosition(FLAPS_STEP * 1);
  FlapsStepper.setCurrentPosition(0);
  for (int i = 1; i <= 1; i++) {
    SendDebug("Loop :" + String(i));
    FlapsStepper.runToNewPosition(-FLAPS_STEP * 1);
    FlapsStepper.runToNewPosition(0);
    delay(200);
  }
  FlapsStepper.runToNewPosition(-100);
  SendDebug("Flaps Current = " + String(FlapsStepper.currentPosition()));
  SendDebug("End FlapsStepper");
  //  ################# End Faps Startup #########################



  SendDebug("STEPPER INITIALISATION COMPLETE");

  for (int i = 150; i >= 0; i--) {
    analogWrite(BACKLIGHTING, i);
    delay(15);
  }
  digitalWrite(BACKLIGHTING, false);
  SendDebug("LAMP AND LED TEST END");


  SendDebug("Setup Complete");
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

// ################################ BEGIN TACAN ##############################


void sendToDcsBiosMessage(const char *msg, const char *arg) {


  if (Reflector_In_Use == 1) {
    udp.beginPacket(reflectorIP, reflectorport);
    udp.println("Right Input - " + String(msg) + ":" + String(arg));
    udp.endPacket();
  }

  sendDcsBiosMessage(msg, arg);
}

// ################################ BEGIN LIGHTING ##############################

void onIntFltInstLBrightChange(unsigned int newValue) {
  int outvalue = 0;
  outvalue = map(newValue, 0, 65534, 0, 255);
  SendDebug("Eng Inst Brightness=" + String(outvalue));
  analogWrite(BACKLIGHTING, outvalue);
}
DcsBios::IntegerBuffer intFltInstLBrightBuffer(A_10C_INT_FLT_INST_L_BRIGHT, onIntFltInstLBrightChange);



void onIntFloodLBrightChange(unsigned int newValue) {
  int floodoutvalue = 0;
  floodoutvalue = map(newValue, 0, 65534, 0, 255);
  SendDebug("Flood Brightness=" + String(floodoutvalue));
}
DcsBios::IntegerBuffer intFloodLBrightBuffer(A_10C_INT_FLOOD_L_BRIGHT, onIntFloodLBrightChange);

// ################################ END LIGHTING ##############################

// ################################ BEGIN STEPPERS ##############################



// ################################### START FLAPS ##############################################
#define FlapsMaxDegrees 200
// DcsBios::Switch3Pos flapsSwitch("FLAPS_SWITCH", PIN_A, PIN_B);
void setFlaps(unsigned int TargetDegrees) {

  int signedTargetDegrees = TargetDegrees;
  SendDebug("Flaps = " + String(signedTargetDegrees) + " Current = " + String(FlapsStepper.currentPosition()));
  if (signedTargetDegrees >= FlapsMaxDegrees) signedTargetDegrees = FlapsMaxDegrees;
  //
  FlapsStepper.moveTo(-signedTargetDegrees);
}
void onFlapPosChange(unsigned int newValue) {
  setFlaps((map(newValue, 0, 65535, 0, FlapsMaxDegrees * 0.7)));
}
DcsBios::IntegerBuffer flapPosBuffer(A_10C_FLAP_POS, onFlapPosChange);
// ################################### END FLAPS ##############################################



// ################################### START AIRSPEED CURRENT ##############################################
void setCurrentAirspeed(long TargetCurrentAirSpeed) {
  // SendDebug("Airspeed = " + String(TargetCurrentAirSpeed));
  SpeedCurrentstepper.moveTo(TargetCurrentAirSpeed);
}
void onAirspeedNeedleChange(unsigned int newValue) {
  // SendDebug("onAirspeedDialChange = " + String(newValue));
  setCurrentAirspeed((map(newValue, 0, 65535, 0, DUAL_STEPS + (5 * 16))));
}
DcsBios::IntegerBuffer airspeedNeedleBuffer(A_10C_AIRSPEED_NEEDLE, onAirspeedNeedleChange);

// ################################### START AIRSPEED CURRENT ##############################################



// ################################### START AIRSPEED MAX ##############################################
void setMaxAirspeed(long TargetMaxAirSpeed) {
  // SendDebug("Max Airspeed = " + String(TargetMaxxAirSpeed));
  SpeedMaxstepper.moveTo(TargetMaxAirSpeed);
}
void onAirspeedMaxIasChange(unsigned int newValue) {
  // SendDebug("onAirspeedMaxIasChange = " + String(newValue));
  setMaxAirspeed((map(newValue, 0, 65535, 0, DUAL_STEPS + (5 * 16))));
}
DcsBios::IntegerBuffer airspeedMaxIasBuffer(A_10C_AIRSPEED_MAX_IAS, onAirspeedMaxIasChange);

// ################################### START AIRSPEED MAX ##############################################

// ################################### START VSI ##############################################


#define VSIMaxSteps 2400
void setVSI(long TargetVSI) {
  if (TargetVSI > VSIMaxSteps) {
    TargetVSI = VSIMaxSteps;
  } else if (TargetVSI < -VSIMaxSteps) {
    TargetVSI = -VSIMaxSteps;
  }
  // SendDebug("VSI = " + String(TargetVSI));
  VSIstepper.moveTo(TargetVSI);
}


void onVviChange(unsigned int newValue) {
  
  long VSI = newValue;
  VSI = VSI - 32767;
  // SendDebug("onVviChange = " + String(newValue) + " long VSI = " + String(VSI));
  setVSI(map(VSI, -32767, 32767, -VSIMaxSteps, VSIMaxSteps));
}
DcsBios::IntegerBuffer vviBuffer(A_10C_VVI, onVviChange);

// ################################### END VSI ##############################################


// ################################### BEGIN ALT ##############################################


void onAltMslFtChange(unsigned int newValue) {
  // Max Value of feet is 65535
  // 5760 Steps per 1000 feet
  // So 5.76 steps foot - need float as long doesn't do decimal
  float ALTtargetSteps = newValue;
  ALTtargetSteps = ALTtargetSteps * 5.76;
  long longAlttargetSteps = long(ALTtargetSteps);
   ALTstepper.moveTo(longAlttargetSteps);
}
DcsBios::IntegerBuffer altMslFtBuffer(CommonData_ALT_MSL_FT, onAltMslFtChange);

// ################################### END ALT ##############################################

void updateSteppers() {
  VSIstepper.run();
  ALTstepper.run();
  SpeedCurrentstepper.run();
  SpeedMaxstepper.run();
  FlapsStepper.run();
}

// ################################ END STEPPERS ##############################

void loop() {

  if (millis() >= NEXT_STATUS_TOGGLE_TIMER) {
    RED_LED_STATE = !RED_LED_STATE;

    digitalWrite(Check_LED_G, RED_LED_STATE);
    digitalWrite(Check_LED_R, !RED_LED_STATE);
    NEXT_STATUS_TOGGLE_TIMER = millis() + FLASH_TIME;
  }

  if (DCSBIOS_In_Use == 1) DcsBios::loop();
  updateSteppers();

  currentMillis = millis();
}
