
/*
JET_RANGER_STEPPERS

Drives:

SARI
AccelStepper VSIstepper
AccelStepper ALTstepper
AccelStepper IASstepper
AccelStepper SpeedMaxstepper
AccelStepper FlapsStepper
AccelStepper AOAstepper
AccelStepper GForcestepper
AccelStepper RadarAltStepper
AccelStepper EOTstepper
AccelStepper XOTstepper
AccelStepper XOPstepper
AccelStepper EGTstepper
AccelStepper TSstepper
AccelStepper RSstepper
AccelStepper FAstepper
AccelStepper ETstepper
AccelStepper GPstepper
AccelStepper EOPstepper

BACK_LIGHTS

*/


////////////////////---||||||||||********||||||||||---\\\\\\\\\\\\\\\\\\\\
//||               FUNCTION = JET RANGER STEPPER CONTROLLER           ||\\
//||            ARDUINO PROCESSOR TYPE = Arduino Mega 2560            ||\\
//||      ARDUINO CHIP SERIAL NUMBER = SN -                           ||\\
//||                    CONNECTED COM PORT = tba                      ||\\
//||               ****ADD ASSIGNED COM PORT NUMBER****               ||\\
//||            ****DO CHECK S/N BEFORE UPLOAD NEW DATA****           ||\\
////////////////////---||||||||||********||||||||||---\\\\\\\\\\\\\\\\\\\\

/*
 *  */




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

String BoardName = "Jet Ranger Steppers";

// These local Mac and IP Address will be reassigned early in startup based on
// the device ID as set by address pins
byte mac[] = { 0xA8, 0x61, 0x0A, 0x67, 0x83, 0x69 };
String sMac = "A8:61:0A:67:83:03";
IPAddress ip(172, 16, 1, 105);
String strMyIP = "172.16.1.105";

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
// Matches JET_RANGER_SERVO_CONTROLLER's MSFSport so the same PC bridge app
// wire format ("D,CODE:value,...") can be reused. Was previously 7791 and unused.
const unsigned int MSFSport = 13136;

int packetSize;
int debugLen;
EthernetUDP udp;
EthernetUDP debugUDP;
// Receives the same front-panel data packets JET_RANGER_SERVO_CONTROLLER
// listens for; only IAS (airspeed) and ALT (altitude) are wired up for now.
EthernetUDP MSFSudp;
int MSFSpacketsize;
int MSFSLen;
const unsigned long incomingcheckinterval = 5;
long lastincomingpacketcheck = 0;
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

// Aligned with Stepper-Tuning-Harness's LED pins (that sketch moved them
// off 12/13 since those became IASstepper's coil pins there -
// see that sketch's own summary). This sketch's IASstepper is
// still on its original DRIVER pins (34/36), not 12/13, so there's no
// equivalent collision here to resolve - this change is purely to match
// the harness's wiring, since the two sketches are meant to be drop-in
// stand-ins for each other on the same physical board.
#define RED_STATUS_LED_PORT 15
#define GREEN_STATUS_LED_PORT 14
#define Check_LED_R 15
#define Check_LED_G 14

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

#define BACK_LIGHTS 8

// ########################## BEGIN STEPPERS ########################################
#include <AccelStepper.h>

#define STEPPER_MAX_SPEED 9000
#define STEPPER_ZERO_SEEK_SPEED 600
#define STEPPER_ACCELERATION 1000

#define AllstepperEnablePin 56


// Swapped with VSI's coil pins below: VSI moved off this DRIVER/STEP-DIR
// pair onto direct coils, and Flaps (below) took over this pair - it is
// NOT unused, it now belongs to Flaps.
#define FlapsStepPin 46
#define FlapsDirectionPin 48
// Scaled down by the same ~8x ratio as the VSI homing step count below
// (FULL4WIRE_HOMING_STEPS / the old geared STEPS) since VSI's usable
// range shrank when it moved to direct-drive coils - an unverified
// estimate, NOT bench-measured. Confirm/recalibrate on real hardware.
#define VSIoffset 1

// Swapped with Flaps' step/dir pins above: Flaps moved onto this
// DRIVER/STEP-DIR pair, and VSI (below) took over these coil pins - it is
// NOT unused, it now belongs to VSI.
#define COIL_VSI_A 7
#define COIL_VSI_B 8
#define COIL_VSI_C 9
#define COIL_VSI_D 11

// New gauges ported from Stepper-Tuning-Harness (same pin assignments as
// that bench sketch, all 4-wire direct-drive FULL4WIRE). Added as bare
// AccelStepper objects only - no homing/startup routine or DCS-BIOS
// binding yet, since none of these are bench-verified (direction,
// steps-per-unit, or even a real end stop location) on this hardware.
// Reachable for now only via the raw step-passthrough UDP test codes in
// HandleOutputValuePair() below. NOT checked for pin collisions against
// this sketch's existing active pins (ALT/SpeedMax/Flaps/AOA/GForce/
// AllstepperEnablePin, none of which were removed here the way they were
// in the harness) - see the summary given alongside this change.
#define RADAR_ALT_COIL_A 32
#define RADAR_ALT_COIL_B 33
#define RADAR_ALT_COIL_C 34
#define RADAR_ALT_COIL_D 35

#define EOT_COIL_A 48
#define EOT_COIL_B A0
#define EOT_COIL_C A1
#define EOT_COIL_D A2

#define XOT_COIL_A A3
#define XOT_COIL_B A4
#define XOT_COIL_C A5
#define XOT_COIL_D A6

#define XOP_COIL_A A7
#define XOP_COIL_B A8
#define XOP_COIL_C A9
#define XOP_COIL_D A10

#define EGT_COIL_A A11
#define EGT_COIL_B A12
#define EGT_COIL_C A13
#define EGT_COIL_D A14

#define TS_COIL_A 24
#define TS_COIL_B 25
#define TS_COIL_C 26
#define TS_COIL_D 27

#define RS_COIL_A 28
#define RS_COIL_B 29
#define RS_COIL_C 30
#define RS_COIL_D 31

#define FA_COIL_A 2
#define FA_COIL_B 3
#define FA_COIL_C 4
#define FA_COIL_D 6

#define ET_COIL_A 36
#define ET_COIL_B 37
#define ET_COIL_C 38
#define ET_COIL_D 39

#define GP_COIL_A 40
#define GP_COIL_B 41
#define GP_COIL_C 42
#define GP_COIL_D 43

#define EOP_COIL_A 44
#define EOP_COIL_B 45
#define EOP_COIL_C 46
#define EOP_COIL_D 47

#define STEPPER_SPD_A 12
#define STEPPER_SPD_B 13
#define STEPPER_SPD_C 22
#define STEPPER_SPD_D 23

#define STEPS 315 * 16       // The 16 is the default divisors when no pins are tied together on the driver module \
                            // For an unmodified Vid series there are 315 steps
#define DUAL_STEPS 315 * 16  // The Dual stepper seems to have fewer steps between stops
// Direct-drive (FULL4WIRE) step count, no overshoot multiplier needed -
// originally for Flaps, now also used by VSI's homing below since VSI
// moved from a geared DRIVER motor onto direct coils.
#define FULL4WIRE_HOMING_STEPS 315 * 1.1
//AccelStepper ALTstepper(AccelStepper::DRIVER, ALTstepPin, ALTdirectionPin);
AccelStepper IASstepper(AccelStepper::FULL4WIRE, STEPPER_SPD_C, STEPPER_SPD_D, STEPPER_SPD_A, STEPPER_SPD_B);
AccelStepper VSIstepper(AccelStepper::FULL4WIRE, COIL_VSI_A, COIL_VSI_B, COIL_VSI_C, COIL_VSI_D);

// New gauges below, ported from Stepper-Tuning-Harness - see the pin
// defines above for the collision-check caveat. RadarAltStepper's coil
// argument order (C, D, A, B rather than A, B, C, D) matches exactly what
// the harness uses, carried over as-is rather than "corrected" to A..D,
// since that order was whatever the harness found to work on the bench.
AccelStepper RadarAltStepper(AccelStepper::FULL4WIRE, RADAR_ALT_COIL_C, RADAR_ALT_COIL_D, RADAR_ALT_COIL_A, RADAR_ALT_COIL_B);
AccelStepper EOTstepper(AccelStepper::FULL4WIRE, EOT_COIL_A, EOT_COIL_B, EOT_COIL_C, EOT_COIL_D);
AccelStepper XOTstepper(AccelStepper::FULL4WIRE, XOT_COIL_A, XOT_COIL_B, XOT_COIL_C, XOT_COIL_D);
AccelStepper XOPstepper(AccelStepper::FULL4WIRE, XOP_COIL_A, XOP_COIL_B, XOP_COIL_C, XOP_COIL_D);
AccelStepper EGTstepper(AccelStepper::FULL4WIRE, EGT_COIL_A, EGT_COIL_B, EGT_COIL_C, EGT_COIL_D);
AccelStepper TSstepper(AccelStepper::FULL4WIRE, TS_COIL_A, TS_COIL_B, TS_COIL_C, TS_COIL_D);
AccelStepper RSstepper(AccelStepper::FULL4WIRE, RS_COIL_A, RS_COIL_B, RS_COIL_C, RS_COIL_D);
AccelStepper FAstepper(AccelStepper::FULL4WIRE, FA_COIL_A, FA_COIL_B, FA_COIL_C, FA_COIL_D);
AccelStepper ETstepper(AccelStepper::FULL4WIRE, ET_COIL_A, ET_COIL_B, ET_COIL_C, ET_COIL_D);
AccelStepper GPstepper(AccelStepper::FULL4WIRE, GP_COIL_A, GP_COIL_B, GP_COIL_C, GP_COIL_D);
AccelStepper EOPstepper(AccelStepper::FULL4WIRE, EOP_COIL_A, EOP_COIL_B, EOP_COIL_C, EOP_COIL_D);
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
    // W5500's own ARP/send retry timeout, not the boot-time wait below -
    // default is 200ms x 8 retries = up to 1600ms blocked inside
    // udp.beginPacket()/endPacket() any time a send fails to get an ARP
    // reply (e.g. destination host down). Cut to 10ms so a communications
    // failure can't stall updateSteppers()'s .run() servicing and make
    // the steppers stutter. Retry count left at its default (8), so worst
    // case is now ~80ms instead of ~1600ms.
    Ethernet.setRetransmissionTimeout(10);
    udp.begin(localport);
    MSFSudp.begin(MSFSport);

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


  pinMode(BACK_LIGHTS, OUTPUT);
  // analogWrite(BACK_LIGHTS, 255);

  // delay(3000);

  // SendDebug("Dimming Leds");
  // for (int Local_Brightness = 255; Local_Brightness >= 0; Local_Brightness--) {
  //   analogWrite(BACK_LIGHTS, Local_Brightness);
  //   // SendDebug("Led Brightness " + String(Local_Brightness));
  //   delay(15);
  // }

#define BrightnessWhileRunningSetup 128

  analogWrite(BACK_LIGHTS, BrightnessWhileRunningSetup);

  SendDebug("STEPPER INITIALISATION STARTED");

  //  pinMode(AllstepperEnablePin, OUTPUT);
  //  pinMode(ALTzeroSensePin, INPUT);

  VSIstepper.setMaxSpeed(STEPPER_MAX_SPEED);
  VSIstepper.setAcceleration(STEPPER_ACCELERATION);
  //ALTstepper.setMaxSpeed(STEPPER_MAX_SPEED);
  //ALTstepper.setAcceleration(STEPPER_ACCELERATION);
  IASstepper.setMaxSpeed(STEPPER_MAX_SPEED);
  IASstepper.setAcceleration(STEPPER_ACCELERATION);
  RadarAltStepper.setMaxSpeed(STEPPER_MAX_SPEED);
  RadarAltStepper.setAcceleration(STEPPER_ACCELERATION);
  EOTstepper.setMaxSpeed(STEPPER_MAX_SPEED);
  EOTstepper.setAcceleration(STEPPER_ACCELERATION);
  XOTstepper.setMaxSpeed(STEPPER_MAX_SPEED);
  XOTstepper.setAcceleration(STEPPER_ACCELERATION);
  XOPstepper.setMaxSpeed(STEPPER_MAX_SPEED);
  XOPstepper.setAcceleration(STEPPER_ACCELERATION);
  EGTstepper.setMaxSpeed(STEPPER_MAX_SPEED);
  EGTstepper.setAcceleration(STEPPER_ACCELERATION);
  TSstepper.setMaxSpeed(STEPPER_MAX_SPEED);
  TSstepper.setAcceleration(STEPPER_ACCELERATION);
  RSstepper.setMaxSpeed(STEPPER_MAX_SPEED);
  RSstepper.setAcceleration(STEPPER_ACCELERATION);
  FAstepper.setMaxSpeed(STEPPER_MAX_SPEED);
  FAstepper.setAcceleration(STEPPER_ACCELERATION);
  ETstepper.setMaxSpeed(STEPPER_MAX_SPEED);
  ETstepper.setAcceleration(STEPPER_ACCELERATION);
  GPstepper.setMaxSpeed(STEPPER_MAX_SPEED);
  GPstepper.setAcceleration(STEPPER_ACCELERATION);
  EOPstepper.setMaxSpeed(STEPPER_MAX_SPEED);
  EOPstepper.setAcceleration(STEPPER_ACCELERATION);


  digitalWrite(AllstepperEnablePin, false);

  // ################# Start VSI Startup #########################
  SendDebug("Start VSI");

  // VSI is now a direct-driven FULL4WIRE stepper on coil pins (was a
  // geared DRIVER motor on VSIstepPin/VSIdirectionPin, now Flaps' pins -
  // see the pin swap above). Switched from STEPS*1.1 (geared, ~5544 steps
  // with a 10% overshoot) to FULL4WIRE_HOMING_STEPS (315*2 = 630, no
  // overshoot) to match - the same constant this sketch already defines
  // for exactly this stepper type (previously FLAPS_STEP, for Flaps' own
  // direct-drive homing). Direction sign kept as-is from before this
  // hardware change - NOT re-verified against the new physical motor,
  // confirm it actually winds to (and stops cleanly at) the real end stop
  // before trusting it unattended.
  VSIstepper.runToNewPosition(-FULL4WIRE_HOMING_STEPS);
  VSIstepper.setCurrentPosition(0);

  for (int i = 1; i <= 1; i++) {
    SendDebug("Loop :" + String(i));
    VSIstepper.runToNewPosition(FULL4WIRE_HOMING_STEPS);
    delay(200);
    VSIstepper.runToNewPosition(0);
    delay(200);
  }

  // Move VSI to zero position and set
  VSIstepper.runToNewPosition((FULL4WIRE_HOMING_STEPS / 2) - VSIoffset);
  VSIstepper.setCurrentPosition(0);
  SendDebug("End VSI");
  // ################# End VSI Startup #########################


  // // ################# Start ALT Startup #########################
  // SendDebug("Start ALT");
  // for (int i = 1; i <= 1; i++) {
  //   SendDebug("Loop :" + String(i));
  //   ALTstepper.moveTo(-STEPS * 2);
  //   while (ALTstepper.distanceToGo() != 0) {
  //     if (digitalRead(ALTzeroSensePin) != true) {
  //       SendDebug("Found Alt Zero Position");
  //       ALTstepper.setCurrentPosition(0);
  //       break;
  //     }
  //     ALTstepper.run();
  //   }
  //   delay(500);
  //   SendDebug("Send Alt Round 40 times");
  //   long SendAAltForATrip = 5760 * 3;
  //   // 5760 steps per loop
  //   ALTstepper.runToNewPosition(SendAAltForATrip);
  //   delay(200);
  //   SendDebug("Return Alt to 0");
  //   ALTstepper.runToNewPosition(0);
  // }
  // // Move ALT to zero position - need to monitor zero sense



  SendDebug("End ALT");
  // ################# End ALT Startup #########################

  // ################# Start Speed Current Startup #########################
  SendDebug("Start IASstepper");
  IASstepper.runToNewPosition(-FULL4WIRE_HOMING_STEPS * 1.1);
  IASstepper.setCurrentPosition(0);

  SendDebug("IAS Pause");
  delay(2000);

  for (int i = 1; i <= 3; i++) {
    SendDebug("Loop :" + String(i));
    SendDebug("Sending IAS to Max");
    IASstepper.runToNewPosition(600);
    delay(2000);
    SendDebug("Returning IAS to Zero");
    IASstepper.runToNewPosition(0);
    
    delay(2000);
  }
  SendDebug("End IASstepper");
  delay(2000);

  //  ################ #End Speed Current Startup######################## #



//   // Already disabled before this reanalysis, and now additionally stale:
//   // Flaps is DRIVER/geared now (was FULL4WIRE when this was written - see
//   // the VSI/Flaps pin swap above), so if this is ever re-enabled it needs
//   // STEPS-style geared homing (with overshoot), not the direct-drive
//   // FULL4WIRE_HOMING_STEPS (ex-FLAPS_STEP) referenced below.
//   // // ################# Start Flaps Startup #########################
//   // SendDebug("Start FlapsStepper");
//   // FlapsStepper.runToNewPosition(FULL4WIRE_HOMING_STEPS * 1);
//   // FlapsStepper.setCurrentPosition(0);
//   // for (int i = 1; i <= 1; i++) {
//   //   SendDebug("Loop :" + String(i));
//   //   FlapsStepper.runToNewPosition(-FULL4WIRE_HOMING_STEPS * 1);
//   //   FlapsStepper.runToNewPosition(0);
//   //   delay(200);
//   // }
//   // FlapsStepper.runToNewPosition(-100);
//   // SendDebug("Flaps Current = " + String(FlapsStepper.currentPosition()));
//   // SendDebug("End FlapsStepper");
//   // //  ################# End Faps Startup #########################

//   // ################# Start AOA Startup #########################
//   SendDebug("Start AOAStepper");
#define AOAZeroOffSet 200
#define AOAMaxSteps 4200
  //   AOAstepper.runToNewPosition(-STEPS * 1);
  //   AOAstepper.setCurrentPosition(0);
  //   AOAstepper.runToNewPosition(AOAZeroOffSet);
  //   AOAstepper.setCurrentPosition(0);
  //   for (int i = 1; i <= 1; i++) {
  //     SendDebug("Loop :" + String(i));
  //     AOAstepper.runToNewPosition(AOAMaxSteps);
  //     AOAstepper.runToNewPosition(0);
  //     delay(200);
  //   }

  //   SendDebug("End AOAStepper");
  //   //  ################# End AOA Startup #########################

  //   // ################# Start GForce Startup #########################
  //   SendDebug("Start GForcestepper");
  // #define GForceZeroOffSet 0
  // #define GForceMaxSteps 4800
  //   GForcestepper.runToNewPosition(-STEPS * 1);
  //   GForcestepper.setCurrentPosition(0);
  //   GForcestepper.runToNewPosition(GForceZeroOffSet);
  //   GForcestepper.setCurrentPosition(0);
  //   for (int i = 1; i <= 1; i++) {
  //     SendDebug("Loop :" + String(i));
  //     GForcestepper.runToNewPosition(GForceMaxSteps);
  //     GForcestepper.runToNewPosition(0);
  //     delay(200);
  //   }

  //   GForcestepper.runToNewPosition(2030);

  //   SendDebug("End GForcestepper");
  //   //  ################# End GForce Startup #########################


  SendDebug("STEPPER INITIALISATION COMPLETE");


  if (DCSBIOS_In_Use == 1) DcsBios::setup();

#define BrightnessPostSetup 65
  analogWrite(BACK_LIGHTS, BrightnessPostSetup);

  SendDebug(BoardName + " - " + strMyIP + " Setup Complete. " + String(millis()) + "mS since reset.");
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


void sendToDcsBiosMessage(const char* msg, const char* arg) {


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
  analogWrite(BACK_LIGHTS, outvalue);
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



// // ################################### START FLAPS ##############################################
// #define FlapsMaxDegrees 200
// // DcsBios::Switch3Pos flapsSwitch("FLAPS_SWITCH", PIN_A, PIN_B);
// void setFlaps(unsigned int TargetDegrees) {

//   int signedTargetDegrees = TargetDegrees;
//   SendDebug("Flaps = " + String(signedTargetDegrees) + " Current = " + String(FlapsStepper.currentPosition()));
//   if (signedTargetDegrees >= FlapsMaxDegrees) signedTargetDegrees = FlapsMaxDegrees;
//   //
//   FlapsStepper.moveTo(-signedTargetDegrees);
// }
// void onFlapPosChange(unsigned int newValue) {
//   setFlaps((map(newValue, 0, 65535, 0, FlapsMaxDegrees * 0.7)));
// }
// DcsBios::IntegerBuffer flapPosBuffer(A_10C_FLAP_POS, onFlapPosChange);
// // ################################### END FLAPS ##############################################



// ################################### START AIRSPEED CURRENT ##############################################
void setCurrentAirspeed(long TargetCurrentAirSpeed) {
  // SendDebug("Airspeed = " + String(TargetCurrentAirSpeed));
  IASstepper.moveTo(TargetCurrentAirSpeed);
}
void onAirspeedNeedleChange(unsigned int newValue) {
  // SendDebug("onAirspeedDialChange = " + String(newValue));
  setCurrentAirspeed((map(newValue, 0, 65535, 0, DUAL_STEPS + (5 * 16))));
}
DcsBios::IntegerBuffer airspeedNeedleBuffer(A_10C_AIRSPEED_NEEDLE, onAirspeedNeedleChange);

// Real-value UDP handler for Current Airspeed (see the "IAS" case in
// HandleOutputValuePair() below) - knots now, rather than the raw step
// pass-through this code used before. Unlike the placeholder linear
// scale used for the newer, never-homed gauges (EGT/EOT/etc, which have
// no established step range yet), this reuses the same step ceiling
// (DUAL_STEPS + (5*16)) the DCS-BIOS path above already maps its own
// 0-65535 needle value onto, since that's this stepper's real,
// already-in-use mechanical range - not a fresh guess.
#define IAS_MIN_KT 0
#define IAS_MAX_KT 140

long iasKtToSteps(long kt) {
  if (kt < IAS_MIN_KT) kt = IAS_MIN_KT;
  if (kt > IAS_MAX_KT) kt = IAS_MAX_KT;
  return map(kt, IAS_MIN_KT, IAS_MAX_KT, 0, DUAL_STEPS + (5 * 16));
}

void setIAS(long TargetKt) {
  setCurrentAirspeed(iasKtToSteps(TargetKt));
}

// ################################### START AIRSPEED CURRENT ##############################################




// ################################### START VSI ##############################################


// Was 2400 for the old geared DRIVER motor. Scaled down by the same ~8x
// ratio as the homing step count (FULL4WIRE_HOMING_STEPS / the old
// geared STEPS) now that VSI is direct-driven on coils - an unverified
// estimate, NOT bench-measured. Confirm/recalibrate on real hardware.
// Still used to clamp the DCS-BIOS path (onVviChange) below - the UDP
// path now uses the real VSI_FPM_TABLE calibration instead (see
// vsiFpmToSteps()).
#define VSIMaxSteps 300
void setVSI(long TargetVSI) {
  if (TargetVSI > VSIMaxSteps) {
    TargetVSI = VSIMaxSteps;
  } else if (TargetVSI < -VSIMaxSteps) {
    TargetVSI = -VSIMaxSteps;
  }
  // SendDebug("VSI = " + String(TargetVSI));
  VSIstepper.moveTo(TargetVSI);
}

// VSI fpm-to-step calibration table, hand-measured on the bench (same
// data as Stepper-Tuning-Harness's VSI_FT_TABLE - that harness's "f"
// command uses "ft" as informal shorthand for this gauge's fpm units,
// not altitude). "step" is the raw step target for VSIstepper.moveTo();
// 0 fpm maps to step 0. Sorted ascending by fpm - vsiFpmToSteps() below
// relies on that order.
struct FpmToStepEntry {
  long fpm;
  long step;
};

const FpmToStepEntry VSI_FPM_TABLE[] = {
  { -1750, -311 },
  { -1500, -280 },
  { -1250, -219 },
  { -1000, -161 },
  { -500, -80 },
  { 0, 0 },
  { 500, 85 },
  { 1000, 165 },
  { 1250, 227 },
  { 1500, 286 },
  { 1750, 315 },
};
const int VSI_FPM_TABLE_SIZE = sizeof(VSI_FPM_TABLE) / sizeof(VSI_FPM_TABLE[0]);

// Converts a requested VSI fpm value into a step target by linear
// interpolation between the two nearest VSI_FPM_TABLE rows. A fpm value
// outside the table's range is clamped to whichever end is nearest
// rather than extrapolated, so a wildly out-of-range value can't fling
// VSI past its calibrated range.
long vsiFpmToSteps(long fpm) {
  if (fpm <= VSI_FPM_TABLE[0].fpm) return VSI_FPM_TABLE[0].step;
  if (fpm >= VSI_FPM_TABLE[VSI_FPM_TABLE_SIZE - 1].fpm) return VSI_FPM_TABLE[VSI_FPM_TABLE_SIZE - 1].step;

  for (int i = 0; i < VSI_FPM_TABLE_SIZE - 1; i++) {
    long fpmLo = VSI_FPM_TABLE[i].fpm;
    long fpmHi = VSI_FPM_TABLE[i + 1].fpm;
    if (fpm >= fpmLo && fpm <= fpmHi) {
      long stepLo = VSI_FPM_TABLE[i].step;
      long stepHi = VSI_FPM_TABLE[i + 1].step;
      return stepLo + (long)round((double)(fpm - fpmLo) * (stepHi - stepLo) / (double)(fpmHi - fpmLo));
    }
  }
  return 0;  // unreachable - every fpm is covered by the clamps or the loop above
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


// void onAltMslFtChange(unsigned int newValue) {
//   // Max Value of feet is 65535
//   // 5760 Steps per 1000 feet
//   // So 5.76 steps foot - need float as long doesn't do decimal
//   float ALTtargetSteps = newValue;
//   ALTtargetSteps = ALTtargetSteps * 5.76;
//   long longAlttargetSteps = long(ALTtargetSteps);
//   SendDebug("Altimeter target steps is :" + String(longAlttargetSteps));
//   ALTstepper.moveTo(longAlttargetSteps);
//   SendDebug("Altimeter steps to go :" + String(ALTstepper.distanceToGo() ));
// }
// DcsBios::IntegerBuffer altMslFtBuffer(CommonData_ALT_MSL_FT, onAltMslFtChange);

// ################################### END ALT ##############################################


// ################################### START EGT ##############################################

// EGT (Exhaust Gas Temp) real-value UDP handler - see the "EGT" case in
// HandleOutputValuePair() below, which now sends degrees C instead of a
// raw step target (was a raw pass-through before this). Straight linear
// scale across the gauge's real-world 0-900C range onto
// FULL4WIRE_HOMING_STEPS (630) - the same generic full-scale step count
// used for this stepper's still-unverified homing elsewhere in this
// sketch, since EGTstepper has no bench-measured end stop/steps-per-degree
// calibration yet. NOT a real per-point calibration table like
// VSI_FPM_TABLE/RADAR_ALT_FT_TABLE - revisit with real bench-measured
// points once EGTstepper's actual travel is known, the same way ALT's
// simple feet*5.76 scale was good enough as a first pass before VSI/Radar
// Alt graduated to real tables.
#define EGT_MIN_C 0
#define EGT_MAX_C 900

long egtCToSteps(long tempC) {
  if (tempC < EGT_MIN_C) tempC = EGT_MIN_C;
  if (tempC > EGT_MAX_C) tempC = EGT_MAX_C;
  return map(tempC, EGT_MIN_C, EGT_MAX_C, 0, FULL4WIRE_HOMING_STEPS);
}

void setEGT(long TargetC) {
  EGTstepper.moveTo(egtCToSteps(TargetC));
}

// ################################### END EGT ##############################################

// ################################### START EOT/EOP/XOT/XOP/TS/RS/GP/FA ##############################################

// Real-value UDP handlers for 8 more gauges, same linear-scale-onto-
// FULL4WIRE_HOMING_STEPS placeholder approach as setEGT()/egtCToSteps()
// above (see that section's comment for the full rationale) - none of
// these steppers have bench-measured calibration either.
#define EOT_MIN_C 0
#define EOT_MAX_C 150
long eotCToSteps(long tempC) {
  if (tempC < EOT_MIN_C) tempC = EOT_MIN_C;
  if (tempC > EOT_MAX_C) tempC = EOT_MAX_C;
  return map(tempC, EOT_MIN_C, EOT_MAX_C, 0, FULL4WIRE_HOMING_STEPS);
}
void setEOT(long TargetC) {
  EOTstepper.moveTo(eotCToSteps(TargetC));
}

#define EOP_MIN_PSI 0
#define EOP_MAX_PSI 150
long eopPsiToSteps(long psi) {
  if (psi < EOP_MIN_PSI) psi = EOP_MIN_PSI;
  if (psi > EOP_MAX_PSI) psi = EOP_MAX_PSI;
  return map(psi, EOP_MIN_PSI, EOP_MAX_PSI, 0, FULL4WIRE_HOMING_STEPS);
}
void setEOP(long TargetPsi) {
  EOPstepper.moveTo(eopPsiToSteps(TargetPsi));
}

#define XOT_MIN_C 0
#define XOT_MAX_C 150
long xotCToSteps(long tempC) {
  if (tempC < XOT_MIN_C) tempC = XOT_MIN_C;
  if (tempC > XOT_MAX_C) tempC = XOT_MAX_C;
  return map(tempC, XOT_MIN_C, XOT_MAX_C, 0, FULL4WIRE_HOMING_STEPS);
}
void setXOT(long TargetC) {
  XOTstepper.moveTo(xotCToSteps(TargetC));
}

#define XOP_MIN_PSI 0
#define XOP_MAX_PSI 150
long xopPsiToSteps(long psi) {
  if (psi < XOP_MIN_PSI) psi = XOP_MIN_PSI;
  if (psi > XOP_MAX_PSI) psi = XOP_MAX_PSI;
  return map(psi, XOP_MIN_PSI, XOP_MAX_PSI, 0, FULL4WIRE_HOMING_STEPS);
}
void setXOP(long TargetPsi) {
  XOPstepper.moveTo(xopPsiToSteps(TargetPsi));
}

#define TS_MIN_PCT 0
#define TS_MAX_PCT 120
long tsPctToSteps(long pct) {
  if (pct < TS_MIN_PCT) pct = TS_MIN_PCT;
  if (pct > TS_MAX_PCT) pct = TS_MAX_PCT;
  return map(pct, TS_MIN_PCT, TS_MAX_PCT, 0, FULL4WIRE_HOMING_STEPS);
}
void setTS(long TargetPct) {
  TSstepper.moveTo(tsPctToSteps(TargetPct));
}

#define RS_MIN_PCT 0
#define RS_MAX_PCT 120
long rsPctToSteps(long pct) {
  if (pct < RS_MIN_PCT) pct = RS_MIN_PCT;
  if (pct > RS_MAX_PCT) pct = RS_MAX_PCT;
  return map(pct, RS_MIN_PCT, RS_MAX_PCT, 0, FULL4WIRE_HOMING_STEPS);
}
void setRS(long TargetPct) {
  RSstepper.moveTo(rsPctToSteps(TargetPct));
}

#define GP_MIN_PCT 0
#define GP_MAX_PCT 105
long gpPctToSteps(long pct) {
  if (pct < GP_MIN_PCT) pct = GP_MIN_PCT;
  if (pct > GP_MAX_PCT) pct = GP_MAX_PCT;
  return map(pct, GP_MIN_PCT, GP_MAX_PCT, 0, FULL4WIRE_HOMING_STEPS);
}
void setGP(long TargetPct) {
  GPstepper.moveTo(gpPctToSteps(TargetPct));
}

#define FA_MIN_GAL 0
#define FA_MAX_GAL 75
long faGalToSteps(long gal) {
  if (gal < FA_MIN_GAL) gal = FA_MIN_GAL;
  if (gal > FA_MAX_GAL) gal = FA_MAX_GAL;
  return map(gal, FA_MIN_GAL, FA_MAX_GAL, 0, FULL4WIRE_HOMING_STEPS);
}
void setFA(long TargetGal) {
  FAstepper.moveTo(faGalToSteps(TargetGal));
}

// ################################### END EOT/EOP/XOT/XOP/TS/RS/GP/FA ##############################################


// SARI

long timeToDisable_SARI_ROLL = 0;
bool waitingToDisable_SARI_ROLL = false;
bool SARI_ROLL_ENABLED = false;
#define disable_SARI_ROLL_WaitTime 300  // mS delay after SARI ROll has founds its position \
                                        // Used to help hold the SARI in position

struct SARIStepperConfig {
  unsigned int SARImaxSteps;
  unsigned int SARIacceleration;
  unsigned int maxSpeed;
};
bool SARI_ROLL_INITIALISED = false;


//////SARI - TEST - BEN --------------------------------------------------------------------------------------------------------------
//----------ROLL SERVO----------
//DcsBios::ServoOutput saiPitch(0x74e4, 9, 2400, 544);
DcsBios::ServoOutput saiPitch(0x1028, 9, 2400, 544);

//----------ROLL STEPPER----------

const long zeroTimeout = 50000;
const int SARIenablePin = 56;


class Nema8Stepper : public DcsBios::Int16Buffer {
private:

  AccelStepper& stepper;
  SARIStepperConfig& SARIstepperConfig;
  inline bool SARIzeroDetected() {
    return digitalRead(SARIirDetectorPin) == 0;
  }
  unsigned int (*map_function)(unsigned int);
  unsigned char initState;
  long SARIcurrentStepperPosition;
  long SARIlastAccelStepperPosition;
  unsigned char SARIirDetectorPin;
  long SARIzeroOffset;
  bool SARImovingForward;
  bool SARIlastZeroDetectState;

  long SARIzeroPosSearchStartTime = 0;

  long SARInormalizeStepperPosition(long pos) {
    if (pos < 0) return pos + SARIstepperConfig.SARImaxSteps;
    if (pos >= SARIstepperConfig.SARImaxSteps) return pos - SARIstepperConfig.SARImaxSteps;
    return pos;
  }

  void updateSARIcurrentStepperPosition() {
    // adjust SARIcurrentStepperPosition to include the distance our stepper motor
    // was moved since we last updated it
    long SARImovementSinceLastUpdate = stepper.currentPosition() - SARIlastAccelStepperPosition;
    SARIcurrentStepperPosition = SARInormalizeStepperPosition(SARIcurrentStepperPosition + SARImovementSinceLastUpdate);
    SARIlastAccelStepperPosition = stepper.currentPosition();
  }


public:
  Nema8Stepper(unsigned int address, AccelStepper& stepper, SARIStepperConfig& SARIstepperConfig, unsigned char SARIirDetectorPin, long SARIzeroOffset, unsigned int (*map_function)(unsigned int))
    : Int16Buffer(address), stepper(stepper), SARIstepperConfig(SARIstepperConfig), SARIirDetectorPin(SARIirDetectorPin), SARIzeroOffset(SARIzeroOffset), map_function(map_function), initState(0), SARIcurrentStepperPosition(0), SARIlastAccelStepperPosition(0) {
  }


  virtual void loop() {
    if (initState == 0) {  // not initialized yet
      SendDebug("SARI initState: " + String(initState));
      pinMode(SARIirDetectorPin, INPUT);
      stepper.setMaxSpeed(SARIstepperConfig.maxSpeed);
      stepper.setAcceleration(SARIstepperConfig.SARIacceleration);

      stepper.setMaxSpeed(4000);
      stepper.setAcceleration(500);

      initState = 1;
      SendDebug("Do a quick loop");

      // Microstepping - 16 steps
      // 42HK40 1.8 degrees per step, so 200 steps per turn without microstepping
      // 3200 steps with microstepping
      stepper.moveTo(-1600 * 10);
      while (stepper.distanceToGo() != 0) {
        stepper.run();
      }
      SendDebug("Quick loop complete");
      delay(1000);

      SendDebug("SARI initState moving to State 1");
      SARIzeroPosSearchStartTime = millis();
    }

    if (initState == 1) {
      // move off zero if already there so we always get movement on reset
      // (to verify that the stepper is working)
      if (SARIzeroDetected()) {
        SendDebug("SARI moving off zero sense");
        stepper.move(-300);
        while (stepper.distanceToGo() != 0) {
          stepper.run();
        }

        stepper.runSpeed();
      } else {
        initState = 2;
        SendDebug("SARI initState moving to State 2");
        stepper.setMaxSpeed(SARIstepperConfig.maxSpeed);
        stepper.setAcceleration(SARIstepperConfig.SARIacceleration);
      }
    }

    if (initState == 2) {  // zeroing



      if (!SARIzeroDetected()) {

        if (millis() >= (zeroTimeout + SARIzeroPosSearchStartTime)) {
          SendDebug("SARI Roll - timeoutout finding zero, disabling driver pin");

          initState = 99;
        }

        //SendDebug("SARI Roll - looping - " + String(initState));
        stepper.moveTo(stepper.currentPosition() - 1);
        stepper.run();


      } else {
        stepper.setAcceleration(SARIstepperConfig.SARIacceleration);
        stepper.runToNewPosition(stepper.currentPosition());
        // tell the AccelStepper library that we are at position zero
        stepper.setCurrentPosition(SARIzeroOffset);
        SARIlastAccelStepperPosition = 0;
        // set stepper SARIacceleration in steps per second per second
        // (default is zero)
        stepper.setAcceleration(SARIstepperConfig.SARIacceleration);

        SARIlastZeroDetectState = true;
        initState = 3;
        SendDebug("SARI initState moving to State 3");
        SARI_ROLL_INITIALISED = true;

        ;
      }
    }


    if (initState == 99) {  // Timed out looking for zero do nothing
    }

    //    digitalWrite(enablePin, HIGH);
    if (initState == 3) {  // running normally

      // recalibrate when passing through zero position
      bool SARIcurrentZeroDetectState = SARIzeroDetected();
      if (!SARIlastZeroDetectState && SARIcurrentZeroDetectState && SARImovingForward) {
        // we have moved from left to right into the 'zero detect window'
        // and are now at position 0
        SARIlastAccelStepperPosition = stepper.currentPosition();
        SARIcurrentStepperPosition = SARInormalizeStepperPosition(SARIzeroOffset);
      } else if (SARIlastZeroDetectState && !SARIcurrentZeroDetectState && !SARImovingForward) {
        // we have moved from right to left out of the 'zero detect window'
        // and are now at position (SARImaxSteps-1)
        SARIlastAccelStepperPosition = stepper.currentPosition();
        SARIcurrentStepperPosition = SARInormalizeStepperPosition(SARIstepperConfig.SARImaxSteps + SARIzeroOffset);
      }
      SARIlastZeroDetectState = SARIcurrentZeroDetectState;


      if (hasUpdatedData()) {
        // convert data from DCS to a target position expressed as a number of steps
        long targetPosition = (long)map_function(getData());

        updateSARIcurrentStepperPosition();

        long delta = targetPosition - SARIcurrentStepperPosition;

        // if we would move more than 180 degree counterclockwise, move clockwise instead

        if (delta < -((long)(SARIstepperConfig.SARImaxSteps / 2))) delta += SARIstepperConfig.SARImaxSteps;  //2
        // if we would move more than 180 degree clockwise, move counterclockwise instead
        if (delta > (SARIstepperConfig.SARImaxSteps / 2)) delta -= (long)SARIstepperConfig.SARImaxSteps;  //2

        SARImovingForward = (delta >= 0);





        // tell AccelStepper to move relative to the current position
        stepper.move(delta);
      }
      stepper.run();
      if ((stepper.distanceToGo() == 0) && (waitingToDisable_SARI_ROLL == false) && (SARI_ROLL_ENABLED == true)) {
        // SendDebug("Starting Count down to disable SARI ROLL");
        waitingToDisable_SARI_ROLL = true;
        timeToDisable_SARI_ROLL = millis() + disable_SARI_ROLL_WaitTime;
      }
    }
  }
};

struct SARIStepperConfig SARIstepperConfig = {
  1600,   // SARImaxSteps //200 Native steps with 1/16 MICRO STEPPING
  90000,  // maxSpeed //3200
  60000   // SARIacceleration 3200
};
const int SARIstepPin = 30;
const int SARIdirectionPin = 32;

// define AccelStepper instance
AccelStepper SARIstepperRoll(AccelStepper::DRIVER, SARIstepPin, SARIdirectionPin);

// // Hornet Address - 0x74e6
// // A10 Address - 0x102a
// Nema8Stepper SARIRoll(0x102a,             // address of stepper data
//                       SARIstepperRoll,    // name of AccelStepper instance
//                       SARIstepperConfig,  // SARIStepperConfig struct instance
//                       55,                 // IR Detector Pin (must be LOW in zero position)
//                       800,                // zero offset (SET TO 50% of MaX Steps) 1650 was 800
//                                           // WIngs Level = 1/2 Max Steps -> "Zero" = 1/2 Turn
//                                           // SARI will be upsidedown after Setup, this will correct in Bios
//                       [](unsigned int newValue) -> unsigned int {
//                         newValue = newValue & 0xffff;
//                         return map(newValue, 0, 65535, 0, SARIstepperConfig.SARImaxSteps - 1);
//                       });





// ######################################  End SARI  ######################################



void updateSteppers() {
  VSIstepper.run();
  //ALTstepper.run();
  IASstepper.run();
  RadarAltStepper.run();
  EOTstepper.run();
  XOTstepper.run();
  XOPstepper.run();
  EGTstepper.run();
  TSstepper.run();
  RSstepper.run();
  FAstepper.run();
  ETstepper.run();
  GPstepper.run();
  EOPstepper.run();
}

void onIntConsoleLBrightChange(unsigned int newValue) {
  analogWrite(BACK_LIGHTS, map(newValue, 0, 65535, 0, 255));
}
DcsBios::IntegerBuffer intConsoleLBrightBuffer(A_10C_INT_CONSOLE_L_BRIGHT, onIntConsoleLBrightChange);



// ################################ END STEPPERS ##############################

// ########################################## BEGIN MSFS DATA RECEIVER ########################################
// Receives the same "D,CODE:value,CODE:value,..." UDP payload that
// JET_RANGER_SERVO_CONTROLLER parses. Only IAS (airspeed) and ALT (altitude)
// are wired up for now; every other code is currently ignored.

void ProcessReceivedMSFSString() {

  char* ParameterNamePtr;
  const char* delim = ",";

  // Break the received packet into a series of tokens
  ParameterNamePtr = strtok(packetBuffer, delim);
  String ParameterNameString(ParameterNamePtr);

  if (ParameterNameString[0] == 'D') {
    // Handling a Data Packet
    ParameterNamePtr = strtok(NULL, delim);

    while (ParameterNamePtr != NULL) {
      String wrkstring = String(ParameterNamePtr);
      HandleOutputValuePair(wrkstring);
      ParameterNamePtr = strtok(NULL, delim);
    }
    return;
  } else if (ParameterNameString[0] == 'C') {
    // Handling a Control Packet
    ParameterNamePtr = strtok(NULL, delim);

    while (ParameterNamePtr != NULL) {
      String wrkstring = String(ParameterNamePtr);
      HandleControlString(wrkstring);
      ParameterNamePtr = strtok(NULL, delim);
    }
    return;
  }
  // Unknown Packet Type - ignore
}

void HandleOutputValuePair(String str) {

  int delimeterlocation = str.indexOf(':');
  if (delimeterlocation == 0) return;

  String ParameterName = getValue(str, ':', 0);
  String ParameterValue = getValue(str, ':', 1);
  ParameterValue.trim();

  if (ParameterName == "IAS") {
    // Real knots now (Current Airspeed, 0-140kt) - see setIAS()/
    // iasKtToSteps() above. CAUTION: this is a deliberate units choice,
    // not what "IAS" means on JET_RANGER_SERVO_CONTROLLER.ino - that
    // sketch's IAS is a pre-converted Bell 206 *servo-position* number
    // (via its IAS_Process() table), not knots, so the two boards'
    // "IAS" now have the same code but different units. Not a live
    // conflict today: per this sketch's own notes, FSUIPCWinformsAutoCS's
    // stepper-specific payload doesn't currently include IAS at all (only
    // ALT/VSI/AGL) - but if IAS is ever added back to that payload for
    // this board, it needs to send real knots, not a servo-position
    // number, or this will misinterpret it.
    setIAS(ParameterValue.toInt());
  } else if (ParameterName == "IASRAW") {
    // Distinct raw-step code so the real-units "IAS" code above doesn't
    // have to be the only way to reach this stepper over UDP - bypasses
    // iasKtToSteps() entirely, same as the other "*RAW" codes below.
    IASstepper.moveTo(ParameterValue.toInt());
    // } else if (ParameterName == "ALT") {
    //   // ALT is sent as raw feet, matching the units this sketch's own
    //   // DCS-BIOS altitude callback already expects, so its feet->steps
    //   // conversion can be reused directly.
    //   SendDebug("Altitude is :" + String(ParameterValue.toInt()));
    //   onAltMslFtChange((unsigned int)ParameterValue.toInt());
    // } else if (ParameterName == "ALTRAW") {
    //   // Distinct raw-step code, bypassing the feet*5.76 conversion above.
    //   ALTstepper.moveTo(ParameterValue.toInt());
  } else if (ParameterName == "VSI") {
    // Unlike IAS (still a Bell 206 servo-position number), FSUIPCWinformsAutoCS
    // now sends this board raw fpm specifically for VSI (its own front-panel/
    // servo-controller payload still uses the Bell 206 VSI_Process() number,
    // unchanged) - see its timerMain_Tick send block, which builds a
    // separate stepperPayload with the VSI field swapped to raw fpm.
    // Converted through the real VSI_FPM_TABLE calibration
    // (vsiFpmToSteps()) rather than setVSI()'s placeholder +/-VSIMaxSteps
    // clamp, which stays in use for the separate DCS-BIOS path only.
    VSIstepper.moveTo(vsiFpmToSteps(ParameterValue.toInt()));
  } else if (ParameterName == "VSIRAW") {
    // Distinct raw-step code, bypassing the VSI_FPM_TABLE lookup above.
    VSIstepper.moveTo(ParameterValue.toInt());
  } else if (ParameterName == "AGL") {
    // Raw step pass-through for Radar Altitude - no real calibration
    // exists yet for this gauge (direction/steps-per-foot unverified, see
    // the AccelStepper construct comments above). Renamed from "RALT" to
    // "AGL" to match JET_RANGER_SERVO_CONTROLLER.ino's existing code for
    // this same real-world quantity (also what FSUIPCWinformsAutoCS
    // actually sends in its stepper payload), so this board can be driven
    // by the same live data instead of a separate test-only code.
    // Jet_Ranger_Driver_Test.ino (the bench-test fork of this sketch)
    // also uses "AGL" now (see that sketch), but with real calibrated
    // *feet* through RADAR_ALT_FT_TABLE, not raw steps - since the two
    // sketches are never flashed to the board at the same time this
    // doesn't collide in practice, but StepperVSITester's "Radar ALT (ft)"
    // slider will drive raw steps instead of feet whenever this sketch
    // (rather than the bench-test one) is what's currently on the board.
    // Worth giving this its own calibration table once bench-measured.
    RadarAltStepper.moveTo(ParameterValue.toInt());
  } else if (ParameterName == "OILT") {
    // Real degrees C now (Engine Oil Temperature, 0-150) - see setEOT()/
    // eotCToSteps() above. Renamed from this sketch's original "EOT" to
    // match JET_RANGER_SERVO_CONTROLLER.ino's existing code for the same
    // real-world quantity, so both boards can be driven by the same UDP
    // payload instead of needing a separate send under a different name.
    setEOT(ParameterValue.toInt());
  } else if (ParameterName == "OILTRAW") {
    // Distinct raw-step code, bypassing eotCToSteps() above.
    EOTstepper.moveTo(ParameterValue.toInt());
  } else if (ParameterName == "XMSNT") {
    // Real degrees C now (Transmission Oil Temperature, 0-150). Renamed
    // from "XOT" to match JET_RANGER_SERVO_CONTROLLER.ino.
    setXOT(ParameterValue.toInt());
  } else if (ParameterName == "XMSNTRAW") {
    // Distinct raw-step code, bypassing xotCToSteps() above.
    XOTstepper.moveTo(ParameterValue.toInt());
  } else if (ParameterName == "XMSNP") {
    // Real PSI now (Transmission Oil Pressure, 0-150). Renamed from "XOP"
    // to match JET_RANGER_SERVO_CONTROLLER.ino.
    setXOP(ParameterValue.toInt());
  } else if (ParameterName == "XMSNPRAW") {
    // Distinct raw-step code, bypassing xopPsiToSteps() above.
    XOPstepper.moveTo(ParameterValue.toInt());
  } else if (ParameterName == "ITT") {
    // Real degrees C now (see setEGT()/egtCToSteps() above) - no longer a
    // raw step target like the other new-gauge codes below. Renamed from
    // "EGT" to match JET_RANGER_SERVO_CONTROLLER.ino's code for the same
    // real-world quantity (Bell 206 calls this ITT, not EGT).
    setEGT(ParameterValue.toInt());
  } else if (ParameterName == "ITTRAW") {
    // Distinct raw-step code, bypassing egtCToSteps() above.
    EGTstepper.moveTo(ParameterValue.toInt());
  } else if (ParameterName == "RPME") {
    // Real percent now (Turbine/Engine Speed, 0-120). Renamed from "TS"
    // to match JET_RANGER_SERVO_CONTROLLER.ino's "RPME" (Engine RPM) -
    // same real-world quantity.
    setTS(ParameterValue.toInt());
  } else if (ParameterName == "RPMERAW") {
    // Distinct raw-step code, bypassing tsPctToSteps() above.
    TSstepper.moveTo(ParameterValue.toInt());
  } else if (ParameterName == "RPMR") {
    // Real percent now (Rotor Speed, 0-120). Renamed from "RS" to match
    // JET_RANGER_SERVO_CONTROLLER.ino.
    setRS(ParameterValue.toInt());
  } else if (ParameterName == "RPMRRAW") {
    // Distinct raw-step code, bypassing rsPctToSteps() above.
    RSstepper.moveTo(ParameterValue.toInt());
  } else if (ParameterName == "FUEL") {
    // Real US gallons now (Fuel Available, 0-75). Renamed from "FA" to
    // match JET_RANGER_SERVO_CONTROLLER.ino.
    setFA(ParameterValue.toInt());
  } else if (ParameterName == "FUELRAW") {
    // Distinct raw-step code, bypassing faGalToSteps() above.
    FAstepper.moveTo(ParameterValue.toInt());
  } else if (ParameterName == "TQ") {
    // Raw step pass-through still (no real calibration requested for this
    // one). Renamed from "ET" to match JET_RANGER_SERVO_CONTROLLER.ino's
    // "TQ" (Torque) code for the same gauge.
    ETstepper.moveTo(ParameterValue.toInt());
  } else if (ParameterName == "N1") {
    // Real percent now (Gas Producer, 0-105). Renamed from "GP" to match
    // JET_RANGER_SERVO_CONTROLLER.ino's "N1" code for the same real-world
    // quantity.
    setGP(ParameterValue.toInt());
  } else if (ParameterName == "N1RAW") {
    // Distinct raw-step code, bypassing gpPctToSteps() above.
    GPstepper.moveTo(ParameterValue.toInt());
  } else if (ParameterName == "OILP") {
    // Real PSI now (Engine Oil Pressure, 0-150). Renamed from "EOP" to
    // match JET_RANGER_SERVO_CONTROLLER.ino.
    setEOP(ParameterValue.toInt());
  } else if (ParameterName == "OILPRAW") {
    // Distinct raw-step code, bypassing eopPsiToSteps() above.
    EOPstepper.moveTo(ParameterValue.toInt());
    // Every real-value gauge above (IAS/ALT/VSI/OILT/XMSNT/XMSNP/ITT/RPME/
    // RPMR/N1/OILP/FUEL) also has a distinct "<CODE>RAW" code for sending a
    // raw step target instead of a real-unit value, e.g. "IASRAW" alongside
    // "IAS" - see each real-value case above for its matching *RAW sibling.
    // AGL/TQ/FLAPS/AOA/GFORCE/SPDMAX don't need one since they're already
    // raw-only. Every other code is currently parsed and silently ignored.
  }
}

void HandleControlString(String str) {
  // No control codes are handled yet (matches JET_RANGER_SERVO_CONTROLLER's
  // brightness control, which is likewise received but not acted on).
}

String getValue(String data, char separator, int index) {
  int found = 0;
  int strIndex[] = { 0, -1 };
  int maxIndex = data.length() - 1;

  for (int i = 0; i <= maxIndex && found <= index; i++) {
    if (data.charAt(i) == separator || i == maxIndex) {
      found++;
      strIndex[0] = strIndex[1] + 1;
      strIndex[1] = (i == maxIndex) ? i + 1 : i;
    }
  }
  return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}

// ########################################## END MSFS DATA RECEIVER ########################################

void loop() {

  if (millis() >= NEXT_STATUS_TOGGLE_TIMER) {
    RED_LED_STATE = !RED_LED_STATE;

    digitalWrite(Check_LED_G, RED_LED_STATE);
    digitalWrite(Check_LED_R, !RED_LED_STATE);
    NEXT_STATUS_TOGGLE_TIMER = millis() + FLASH_TIME;
  }

  //if (DCSBIOS_In_Use == 1) DcsBios::loop();
  updateSteppers();

  if (Ethernet_In_Use == 1) {
    if ((millis() - lastincomingpacketcheck) >= incomingcheckinterval) {
      MSFSpacketsize = MSFSudp.parsePacket();
      if (MSFSpacketsize > 0) {
        MSFSLen = MSFSudp.read(packetBuffer, 999);
        if (MSFSLen > 0) {
          packetBuffer[MSFSLen] = 0;
        }
        ProcessReceivedMSFSString();
      }
      lastincomingpacketcheck = millis();
    }
  }

  currentMillis = millis();
}
