
/*
JET_RANGER_OLED_DUAL_STEPPERS

Based on Jet_Ranger_Steppers drivws dual stepper guages

Drives:

SARI
AccelStepper VSIstepper
AccelStepper IASstepper
AccelStepper FuelLoadStepper
(ALTstepper, SpeedMaxstepper, FlapsStepper, AOAstepper, GForcestepper -
 all removed/commented out; no longer declared in this sketch)
AccelStepper EOTstepper
AccelStepper XOTstepper
AccelStepper XOPstepper
AccelStepper EGTstepper
AccelStepper TSstepper
AccelStepper RSstepper
AccelStepper FAstepper
AccelStepper ElectricalLoadStepper
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

#define SwingLoops 1
#define SwingALT true
#define SwingIAS false
#define SwingVSI false
#define SwingRPM true


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

String BoardName = "Jet Ranger OLED Dual Steppers";

// These local Mac and IP Address will be reassigned early in startup based on
// the device ID as set by address pins
byte mac[] = { 0xA8, 0x61, 0x0A, 0x67, 0x83, 0x6A };
String sMac = "A8:61:0A:67:83:6A";
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
// Matches JET_RANGER_SERVO_CONTROLLER's MSFSport so the same PC bridge app
// wire format ("D,CODE:value,...") can be reused. Was previously 7791 and unused.
const unsigned int MSFSport = 13136;

// Health keepalive - same pattern as JET_RANGER_UPPER_CONTROLLER.ino's
// aliveudp: a bare ASCII prefix string ("DUAL_STEPPER", no
// delimiter/payload) sent to reflectorIP (172.16.1.10, the
// JetRangerHealthMonitor host) every aliveinterval, so that app can show
// this board's live/dead status next to COMM_NAV/SERVO/UPPER_INPUT/
// JOYSTICK/STEPPER. Deliberately distinct from the single-board sketch's
// "STEPPER" prefix - JetRangerHealthMonitor matches by message prefix
// only, not sender IP, so reusing "STEPPER" here would have made the two
// boards indistinguishable to it.
const unsigned int aliveport = 13137;
const unsigned long aliveinterval = 10000;
unsigned long lastalivesent = 0;
EthernetUDP aliveudp;

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
// off 12/13 since those became the Current Airspeed stepper's coil pins
// there - see that sketch's own summary). UPDATE: this was originally
// just cosmetic consistency here, since this sketch's own Current
// Airspeed stepper was still on its old DRIVER pins (34/36) at the time.
// It's since been renamed IASstepper and moved onto the same FULL4WIRE
// pins 12/13/22/23 (STEPPER_SPD_A..D) the harness uses - so the reason
// for this LED move is now genuinely load-bearing here too, not just
// precautionary.
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
#define ALT_STEPPER_MAX_SPEED 600
#define ALT_STEPPER_ZERO_SEEK_SPEED 100
#define ALT_STEPPER_ACCELERATION 600

#define AllstepperEnablePin 56


// STALE as of the Flaps removal below: no `FlapsStepper` object exists
// in this sketch any more (its AccelStepper construct and startup block
// are commented out further down), so these two defines are now
// orphaned - nothing reads them. Left in place rather than deleted since
// it wasn't asked for, but they no longer describe live pin usage.
#define FlapsStepPin 46
#define FlapsDirectionPin 48
// Scaled down by the same ~8x ratio as the VSI homing step count below
// (FULL4WIRE_HOMING_STEPS / the old geared STEPS) since VSI's usable
// range shrank when it moved to direct-drive coils - an unverified
// estimate, NOT bench-measured. Confirm/recalibrate on real hardware.
#define VSIoffset 1

// Fine-trim offsets (steps) added to TS_PCT_TABLE's/RS_PCT_TABLE's
// computed target - same purpose/pattern as VSIoffset above: dial in the
// real needles' true mechanical zero without touching the calibration
// tables themselves. Used both at runtime (setTS()/setRS()) and by the
// Turbine/Rotor Speed startup swings' "return to zero" step (below) -
// must be #define'd before setup(), unlike VSIoffset's neighbours these
// can't live down next to setTS()/setRS() where they're mainly used.
// TSoffset has since been given a real value (20); RSoffset is still an
// unmeasured 0.
#define TSoffset 10
#define RSoffset 10

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

#define STEPPER_FL_COIL_A 32
#define STEPPER_FL_COIL_B 33
#define STEPPER_FL_COIL_C 34
#define STEPPER_FL_COIL_D 35


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

#define EL_COIL_A 36
#define EL_COIL_B 37
#define EL_COIL_C 38
#define EL_COIL_D 39

// #define GP_COIL_A 40
// #define GP_COIL_B 41
// #define GP_COIL_C 42
// #define GP_COIL_D 43

#define EOP_COIL_A 44
#define EOP_COIL_B 45
#define EOP_COIL_C 46
#define EOP_COIL_D 47

#define STEPPER_ALT_A 40
#define STEPPER_ALT_B 41
#define STEPPER_ALT_C 42
#define STEPPER_ALT_D 43
#define ALTzeroSensePin A15

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
#define FULL4WIRE_STEPS 315
#define FULL4WIRE_HOMING_STEPS FULL4WIRE_STEPS + 1
#define X27_FULLWIRE_STEPS 630
#define X27_FULLWIRE_HOMING_STEPS X27_FULLWIRE_STEPS + 1
#define Z27_360_FULLWIRE_STEPS 720
AccelStepper ALTstepper(AccelStepper::FULL4WIRE, STEPPER_ALT_A, STEPPER_ALT_B, STEPPER_ALT_C, STEPPER_ALT_D);
AccelStepper IASstepper(AccelStepper::FULL4WIRE, STEPPER_SPD_C, STEPPER_SPD_D, STEPPER_SPD_A, STEPPER_SPD_B);
AccelStepper VSIstepper(AccelStepper::FULL4WIRE, COIL_VSI_C, COIL_VSI_D, COIL_VSI_A, COIL_VSI_B);

// New gauges below, ported from Stepper-Tuning-Harness - see the pin
// defines above for the collision-check caveat. FuelLoadStepper's coil
// argument order (C, D, A, B rather than A, B, C, D) matches exactly what
// the harness uses, carried over as-is rather than "corrected" to A..D,
// since that order was whatever the harness found to work on the bench.
// STEPPER_FL_COIL_A..D are the same physical pins (32-35) the Radar Alt
// stepper used on the single-board sketch this was forked from - this
// board repurposes that stepper as a Fuel Load gauge instead, so the
// object is renamed to match (was RadarAltStepper).
AccelStepper FuelLoadStepper(AccelStepper::FULL4WIRE, STEPPER_FL_COIL_C, STEPPER_FL_COIL_D, STEPPER_FL_COIL_A, STEPPER_FL_COIL_B);
AccelStepper EOTstepper(AccelStepper::FULL4WIRE, EOT_COIL_A, EOT_COIL_B, EOT_COIL_C, EOT_COIL_D);
AccelStepper XOTstepper(AccelStepper::FULL4WIRE, XOT_COIL_A, XOT_COIL_B, XOT_COIL_C, XOT_COIL_D);
AccelStepper XOPstepper(AccelStepper::FULL4WIRE, XOP_COIL_A, XOP_COIL_B, XOP_COIL_C, XOP_COIL_D);
AccelStepper EGTstepper(AccelStepper::FULL4WIRE, EGT_COIL_A, EGT_COIL_B, EGT_COIL_C, EGT_COIL_D);
AccelStepper TSstepper(AccelStepper::FULL4WIRE, TS_COIL_C, TS_COIL_D, TS_COIL_A, TS_COIL_B);
AccelStepper RSstepper(AccelStepper::FULL4WIRE, RS_COIL_C, RS_COIL_D, RS_COIL_A, RS_COIL_B);
AccelStepper FAstepper(AccelStepper::FULL4WIRE, FA_COIL_A, FA_COIL_B, FA_COIL_C, FA_COIL_D);
// EL_COIL_A..D are the same physical pins (36-39) the Torque stepper
// used on the single-board sketch this was forked from - this board
// repurposes that stepper as an Electrical Load gauge instead, so the
// object is renamed to match (was ETstepper).
AccelStepper ElectricalLoadStepper(AccelStepper::FULL4WIRE, EL_COIL_A, EL_COIL_B, EL_COIL_C, EL_COIL_D);
// AccelStepper GPstepper(AccelStepper::FULL4WIRE, GP_COIL_A, GP_COIL_B, GP_COIL_C, GP_COIL_D);
AccelStepper EOPstepper(AccelStepper::FULL4WIRE, EOP_COIL_A, EOP_COIL_B, EOP_COIL_C, EOP_COIL_D);
// ########################### END STEPPERS #########################################

// ############################# BEGIN OLED #########################################

String sAircraftName = "";

#define BARO_OLED_Port 1
#define ALT_OLED_Port 2
// TCA9548A channel 3 - some mux breakout boards label each channel's pin
// pair SD0/SC0..SD7/SC7, so "SD3/SC3" is this channel's I2C data/clock
// pair. Same physical hardware/display class as the Altimeter OLED
// (u8g2_ALT below), just its own unit on its own mux channel - not a
// second use of the Altimeter's actual display.
#define CLOCK_OLED_Port 3

#include <U8g2lib.h>
#include <SPI.h>
#include <Wire.h>

// Op OLEDs
U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C u8g2_ALT(U8G2_R0, /* reset=*/U8X8_PIN_NONE);
U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C u8g2_BARO(U8G2_R0, U8X8_PIN_NONE);
U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C u8g2_CLOCK(U8G2_R0, U8X8_PIN_NONE);


extern "C" {
#include "utility/twi.h"  // from Wire library, so we can do bus scanning
}

#define TCAADDR 0x70

int CurrentDisplay = 0;
int Brightness = 0;
char buffer[20];  //plenty of space for the value of millis() plus a zero terminator



// Altimeter delta 1000 feet for 112 pressure units
// which maps to 8.92857 feet per pressure unit with 2992 as reference
// so delta is (pressure reading - 2992) * 8.92857
#define feetDeltaPerPressureUnit 8.92857


int iLastAltitudeValue = 0;
// Minimum time between Altimeter OLED redraws, regardless of how often
// onAltMslFtChange() fires or how often the value actually changes -
// separate from (and in addition to) the iLastAltitudeValue change-check
// below. lastAltOledUpdateMillis only advances when an update actually
// happens, so a value change that arrives before the interval has
// elapsed isn't lost - it's just deferred to the next eligible callback.
const unsigned long minAltOledUpdateIntervalMs = 300;
unsigned long lastAltOledUpdateMillis = 0;

// Clock OLED - HHMM-encoded (e.g. 1430 for 14:30 Zulu), received over UDP
// from FSUIPCWinformsAutoCS (no DCS-BIOS callback for this one - FSUIPC
// is the only data source). Same gate-on-change + throttle pattern as
// the Altimeter OLED above.
int iLastZuluTimeValue = -1;
const unsigned long minClockOledUpdateIntervalMs = 300;
unsigned long lastClockOledUpdateMillis = 0;

int iAltitudeDelta = 0;
int iBaro = 2992;
int iBaroOnes = 2;
int iBaroTens = 9;
int iBaroHundreds = 9;
int iBaroThousands = 2;
String BaroOnes = String(iBaroOnes);
String BaroTens = String(iBaroTens);
String BaroHundreds = String(iBaroHundreds);
String BaroThousands = String(iBaroThousands);
bool BaroUpdated = true;

// Altimeter
unsigned long nextAltimeterUpdate = 0;
int updateAltimeterInterval = 100;

String Alt1000s = "0";
int LastAlt1000s = 0;
String Alt10000s = "0";
int LastAlt10000s = 0;
bool AltCounterUpdated = true;

char* ptrtopass;

String txtChaffFlare = "S240s120";
String txtJMRstatus = "SBY AIR ";
String txtMWSstatus = "ACTIVE ";


void tcaselect(uint8_t i) {
  if (i > 7) return;

  Wire.beginTransmission(TCAADDR);
  Wire.write(1 << i);
  Wire.endTransmission();
}


void updateBARO(String strnewValue) {

  const char* newValue = strnewValue.c_str();
  tcaselect(BARO_OLED_Port);
  u8g2_BARO.setFontMode(0);
  u8g2_BARO.setDrawColor(0);


  u8g2_BARO.drawBox(0, 0, 128, 64);
  u8g2_BARO.sendBuffer();



  // full range with with U8G2_SSD1309_128X64_NONAME2_F_HW_I2C
  // u8g2_BARO.drawBox(0, 1, 80, 64); with U8G2_SSD1309_128X64_NONAME2_2_HW_I2C
  //  u8g2_BARO.drawStr(85,16, newValue);

  u8g2_BARO.setDrawColor(1);
  u8g2_BARO.setFontDirection(0);


  u8g2_BARO.drawStr(65, 16, newValue);
  u8g2_BARO.sendBuffer();
}

void buildBAROString() {

  updateBARO(BaroThousands + BaroHundreds + BaroTens + BaroOnes);
  BaroUpdated = false;
}

// Renders "HH:MM" on the Clock OLED - same simple single-string-redraw
// approach as updateBARO() above, not the digit-drum approach
// UpdateAltimeterDigits() uses (a clock face doesn't need that).
void updateClock(int hours, int minutes) {
  char clockBuffer[6];  // "HH:MM" + null terminator
  sprintf(clockBuffer, "%02d:%02d", hours, minutes);

  tcaselect(CLOCK_OLED_Port);
  u8g2_CLOCK.setFontMode(0);
  u8g2_CLOCK.setDrawColor(0);
  u8g2_CLOCK.drawBox(0, 0, 128, 64);
  u8g2_CLOCK.sendBuffer();

  u8g2_CLOCK.setDrawColor(1);
  u8g2_CLOCK.setFontDirection(0);
  u8g2_CLOCK.drawStr(30, 16, clockBuffer);
  u8g2_CLOCK.sendBuffer();
}

// Gate-on-change + 300ms-throttle wrapper for updateClock() - same
// pattern as onAltMslFtChange()'s gating of UpdateAltimeterDigits().
// hhmm is HH*100+MM (e.g. 1430 for 14:30 Zulu) - the wire format the
// "ZULU" UDP code (below) uses.
void onZuluTimeChange(int hhmm) {
  if (hhmm != iLastZuluTimeValue
      && (millis() - lastClockOledUpdateMillis) >= minClockOledUpdateIntervalMs) {
    int hours = hhmm / 100;
    int minutes = hhmm % 100;
    updateClock(hours, minutes);
    iLastZuluTimeValue = hhmm;
    lastClockOledUpdateMillis = millis();
  }
}



#define hash_width 24
#define hash_height 32
static unsigned char hash_bits[] = {
  0x00,
  0xFE,
  0x01,
  0x01,
  0xFC,
  0x03,
  0x03,
  0xF8,
  0x07,
  0x07,
  0xF0,
  0x0F,
  0x0F,
  0xE0,
  0x1F,
  0x1F,
  0xC0,
  0x3F,
  0x3F,
  0x80,
  0x7F,
  0x7F,
  0x00,
  0xFE,
  0xFE,
  0x01,
  0xFC,
  0xFC,
  0x03,
  0xF8,
  0xF8,
  0x07,
  0xF0,
  0xF0,
  0x0F,
  0xE0,
  0xE0,
  0x1F,
  0xC0,
  0x00,
  0x3F,
  0x80,
  0x00,
  0x7F,
  0x00,
  0x00,
  0xFE,
  0x01,
  0x01,
  0xFC,
  0x03,
  0x03,
  0xF8,
  0x07,
  0x07,
  0xF0,
  0x0F,
  0x0F,
  0xE0,
  0x1F,
  0x1F,
  0xC0,
  0x3F,
  0x3F,
  0x80,
  0x7F,
  0x7F,
  0x00,
  0xFE,
  0xFE,
  0x01,
  0xFC,
  0xFC,
  0x03,
  0xF8,
  0xF8,
  0x07,
  0xF0,
  0xF0,
  0x0F,
  0xE0,
  0xE0,
  0x1F,
  0xC0,
  0xC0,
  0x3F,
  0x80,
  0x00,
  0x7F,
  0x00,
  0x00,
  0xFE,
  0x00,
  0x00,
  0xFC,
  0x00,
};

int lastHundredsValue = 0;
int lastThousandsValue = 0;
int lastTenThousandsValue = 0;
int lastThousandsCharacterOffset = 0;
int lastHundredsCharacterOffset = 0;

void updateALT(String strTenThousands, String strnewThousands) {

  const char* newTenThousandsValue = strTenThousands.c_str();
  const char* newThousandsValue = strnewThousands.c_str();
  int End_X_Pos = 46;
  int End_Y_Pos = 28;
  int Start_Y_Pos = 13;
  int Start_X_Pos = 27;
  int Box_Width = 20;
  tcaselect(ALT_OLED_Port);
  u8g2_ALT.setFontMode(0);
  u8g2_ALT.setDrawColor(0);
  u8g2_ALT.drawBox(0, 0, 128, 32);
  u8g2_ALT.setDrawColor(1);
  //u8g2_ALT.drawStr(5, 32, newValue);

  if (strTenThousands == "0") {
    u8g2_ALT.setDrawColor(1);

    u8g2_ALT.drawBox(Start_X_Pos, 13, Box_Width, 20);
    u8g2_ALT.setDrawColor(0);

    u8g2_ALT.drawLine(Start_X_Pos, Start_Y_Pos, End_X_Pos, 32);
    u8g2_ALT.drawLine(Start_X_Pos, Start_Y_Pos + 1, End_X_Pos - 1, 32);
    u8g2_ALT.drawLine(Start_X_Pos, Start_Y_Pos + 2, End_X_Pos - 2, 32);
    u8g2_ALT.drawLine(Start_X_Pos, Start_Y_Pos + 3, End_X_Pos - 3, 32);
    u8g2_ALT.drawLine(Start_X_Pos, Start_Y_Pos + 4, End_X_Pos - 4, 32);
    u8g2_ALT.drawLine(Start_X_Pos, Start_Y_Pos + 5, End_X_Pos - 5, 32);
    u8g2_ALT.drawLine(Start_X_Pos, Start_Y_Pos + 6, End_X_Pos - 6, 32);

    u8g2_ALT.drawLine(Start_X_Pos, Start_Y_Pos + 11, End_X_Pos - 11, 32);
    u8g2_ALT.drawLine(Start_X_Pos, Start_Y_Pos + 12, End_X_Pos - 12, 32);
    u8g2_ALT.drawLine(Start_X_Pos, Start_Y_Pos + 13, End_X_Pos - 13, 32);
    u8g2_ALT.drawLine(Start_X_Pos, Start_Y_Pos + 14, End_X_Pos - 14, 32);
    u8g2_ALT.drawLine(Start_X_Pos, Start_Y_Pos + 15, End_X_Pos - 15, 32);

    u8g2_ALT.drawLine(Start_X_Pos + 4, Start_Y_Pos, Start_X_Pos + Box_Width, End_Y_Pos + 1);
    u8g2_ALT.drawLine(Start_X_Pos + 5, Start_Y_Pos, Start_X_Pos + Box_Width, End_Y_Pos);
    u8g2_ALT.drawLine(Start_X_Pos + 6, Start_Y_Pos, Start_X_Pos + Box_Width, End_Y_Pos - 1);
    u8g2_ALT.drawLine(Start_X_Pos + 7, Start_Y_Pos, Start_X_Pos + Box_Width, End_Y_Pos - 2);
    u8g2_ALT.drawLine(Start_X_Pos + 8, Start_Y_Pos, Start_X_Pos + Box_Width, End_Y_Pos - 3);
    u8g2_ALT.drawLine(Start_X_Pos + 9, Start_Y_Pos, Start_X_Pos + Box_Width, End_Y_Pos - 4);
    u8g2_ALT.drawLine(Start_X_Pos + 10, Start_Y_Pos, Start_X_Pos + Box_Width, End_Y_Pos - 5);

    u8g2_ALT.drawLine(Start_X_Pos + 15, Start_Y_Pos, Start_X_Pos + Box_Width, End_Y_Pos - 10);
    u8g2_ALT.drawLine(Start_X_Pos + 16, Start_Y_Pos, Start_X_Pos + Box_Width, End_Y_Pos - 11);
    u8g2_ALT.drawLine(Start_X_Pos + 17, Start_Y_Pos, Start_X_Pos + Box_Width, End_Y_Pos - 12);
    u8g2_ALT.drawLine(Start_X_Pos + 18, Start_Y_Pos, Start_X_Pos + Box_Width, End_Y_Pos - 13);
    u8g2_ALT.drawLine(Start_X_Pos + 19, Start_Y_Pos, Start_X_Pos + Box_Width, End_Y_Pos - 14);

    u8g2_ALT.setDrawColor(1);


  } else {
    u8g2_ALT.drawStr(32, 32, newTenThousandsValue);
  }

  u8g2_ALT.drawStr(65, 32, newThousandsValue);
  u8g2_ALT.sendBuffer();

  AltCounterUpdated = false;
}


void UpdateAltimeterDigits(long height) {

  //
  //SendDebug("Aircraft Name : " + sAircraftName);
  //SendDebug("Raw Height : " + String(height));
  // Adjust for Baro offset

  // Currnelty ignoring Baro Offset
  // height = height + iAltitudeDelta;

  String strnewValue = String(height);


  // int itensAboveDigit = 0;
  // int itensBelowDigit = 0;

  int iHundredsAboveDigit = 0;
  int iHundredsBelowDigit = 0;
  int iHundredsValue = ((height % 1000) / 100);
  String sHundredsValue = String(iHundredsValue);
  //SendDebug("Hundreds Value : " + sHundredsValue);
  if (iHundredsValue == 9) {
    iHundredsAboveDigit = 0;
  } else {
    iHundredsAboveDigit = iHundredsValue + 1;
  }
  if (iHundredsValue == 0) {
    iHundredsBelowDigit = 9;
  } else {
    iHundredsBelowDigit = iHundredsValue - 1;
  }

  String sHundredsAboveDigit = String(iHundredsAboveDigit);
  String sHundredsBelowDigit = String(iHundredsBelowDigit);
  const char* cHundredsValue = sHundredsValue.c_str();
  const char* cHundredsaboveValue = sHundredsAboveDigit.c_str();
  const char* cHundredsbelowValue = sHundredsBelowDigit.c_str();



  int iThousandsAboveDigit = 0;
  int iThousandsBelowDigit = 0;
  int iThousandsValue = ((height % 10000) / 1000);
  String sThousandValue = String(iThousandsValue);
  //SendDebug(String(i) + " : " + sThousandValue);
  if (iThousandsValue == 9) {
    iThousandsAboveDigit = 0;
  } else {
    iThousandsAboveDigit = iThousandsValue + 1;
  }
  if (iThousandsValue == 0) {
    iThousandsBelowDigit = 9;
  } else {
    iThousandsBelowDigit = iThousandsValue - 1;
  }

  String sThousandsAboveDigit = String(iThousandsAboveDigit);
  String sThousandsBelowDigit = String(iThousandsBelowDigit);
  const char* cThousandsValue = sThousandValue.c_str();
  const char* cThousandsaboveValue = sThousandsAboveDigit.c_str();
  const char* cThousandsbelowValue = sThousandsBelowDigit.c_str();


  int iTenThousandsValue = (height / 10000);
  String sTenThousandsDigit = String(iTenThousandsValue);
  //SendDebug("TenThousandsDigit : " + sTenThousandsDigit);
  const char* cTenThousandsValue = sTenThousandsDigit.c_str();

  unsigned long TimeToProcess = millis();

  int CharacterHeightSpacer = 38;

  int iHundredsCharacterOffset = ((height % 100) / 3.2);
  //SendDebug("heigh calc :" + String(height % 100));
  int iThousandsCharacterOffset = ((height % 1000) / 32);
  //SendDebug("Character Offset : " + String(iHundredsCharacterOffset));

  // Only attempt to draw of something has changed that will impact display
  if ((iThousandsValue != lastThousandsValue) || (iTenThousandsValue != lastTenThousandsValue) || (iThousandsCharacterOffset != lastThousandsCharacterOffset)
      || (iHundredsValue != lastHundredsValue) || (iHundredsCharacterOffset != lastHundredsCharacterOffset)) {

    tcaselect(ALT_OLED_Port);

    lastHundredsValue = iHundredsValue;
    lastThousandsValue = iThousandsValue;
    lastTenThousandsValue = iTenThousandsValue;
    lastHundredsCharacterOffset = iHundredsCharacterOffset;
    lastThousandsCharacterOffset = iThousandsCharacterOffset;

    updateSteppers();

    u8g2_ALT.setFontMode(0);
    u8g2_ALT.setDrawColor(0);
    u8g2_ALT.drawBox(0, 0, 128, 32);
    u8g2_ALT.setDrawColor(1);

    updateSteppers();

    // If Ten Thousands value is a 0 draw the hash
    // Position was 10 and 0 moved 40 to the right
    if (sTenThousandsDigit != "0") {
      u8g2_ALT.drawStr(50, 30, cTenThousandsValue);
    } else {
      u8g2_ALT.drawXBM(40, 0, hash_width, hash_height, hash_bits);
    }

    updateSteppers();


    // Three digits for A10 Altimer as it is a primary instrument - only two for Hornet
    if (sAircraftName == "A-10C") {
      // u8g2_ALT.drawStr(40, -2 + 0, cThousandsaboveValue);
      // u8g2_ALT.drawStr(40, 30 + 0, cThousandsValue);
      // u8g2_ALT.drawStr(70, -2 + iHundredsCharacterOffset, cHundredsaboveValue);
      // u8g2_ALT.drawStr(70, 30 + iHundredsCharacterOffset, cHundredsValue);
      // X POs was 40 andd 70 - 30 pixels diff
      u8g2_ALT.drawStr(80, -2 + 0, cThousandsaboveValue);
      u8g2_ALT.drawStr(80, 30 + 0, cThousandsValue);
      u8g2_ALT.drawStr(110, 58 - iHundredsCharacterOffset, cHundredsaboveValue);
      u8g2_ALT.drawStr(110, 26 - iHundredsCharacterOffset, cHundredsValue);
    } else {
      u8g2_ALT.drawStr(80, -2 + iThousandsCharacterOffset, cThousandsaboveValue);
      u8g2_ALT.drawStr(80, 30 + iThousandsCharacterOffset, cThousandsValue);
    }
    updateSteppers();
    u8g2_ALT.sendBuffer();
    updateSteppers();
    ;
    TimeToProcess = millis() - TimeToProcess;
    //SendDebug("OLED Update time :" + String(TimeToProcess));
  }
}


// ############################## END OLED  #########################################



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
    aliveudp.begin(aliveport);

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


  pinMode(ALTzeroSensePin, INPUT);
  // CAUTION: pinMode(AllstepperEnablePin, OUTPUT) above is commented out,
  // but the digitalWrite(AllstepperEnablePin, false) below is NOT - an
  // Arduino digital pin defaults to INPUT at boot, and digitalWrite() on
  // an INPUT pin just toggles its internal pull-up resistor rather than
  // actually driving the pin LOW as a real output. If the stepper
  // drivers' enable line depends on this pin being actively driven, it
  // may not be enabling them the way this code implies. Worth confirming
  // pinMode was meant to stay commented (enable now tied elsewhere/always
  // on) or should be restored.

  VSIstepper.setMaxSpeed(STEPPER_MAX_SPEED);
  VSIstepper.setAcceleration(STEPPER_ACCELERATION);
  ALTstepper.setMaxSpeed(ALT_STEPPER_MAX_SPEED);
  ALTstepper.setAcceleration(ALT_STEPPER_ACCELERATION);
  IASstepper.setMaxSpeed(STEPPER_MAX_SPEED);
  IASstepper.setAcceleration(STEPPER_ACCELERATION);
  FuelLoadStepper.setMaxSpeed(STEPPER_MAX_SPEED);
  FuelLoadStepper.setAcceleration(STEPPER_ACCELERATION);
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
  ElectricalLoadStepper.setMaxSpeed(STEPPER_MAX_SPEED);
  ElectricalLoadStepper.setAcceleration(STEPPER_ACCELERATION);
  //GPstepper.setMaxSpeed(STEPPER_MAX_SPEED);
  //GPstepper.setAcceleration(STEPPER_ACCELERATION);
  EOPstepper.setMaxSpeed(STEPPER_MAX_SPEED);
  EOPstepper.setAcceleration(STEPPER_ACCELERATION);


  digitalWrite(AllstepperEnablePin, false);

  // ################# Start VSI Startup #########################

  if (SwingVSI) {
    SendDebug("Start VSI");

    // VSI is a direct-driven FULL4WIRE stepper on coil pins (COIL_VSI_A..D,
    // now 7/8/9/11, wired C,D,A,B - matches Stepper-Tuning-Harness's own
    // VSI wiring exactly). Homes against an X27.168-style stepper's real
    // travel: X27_FULLWIRE_STEPS (635) is that motor's full-scale step
    // count, X27_FULLWIRE_HOMING_STEPS adds a small 5-step overshoot to
    // guarantee reaching the physical end stop. CAUTION - macro-precedence
    // bug: X27_FULLWIRE_HOMING_STEPS is #defined as "X27_FULLWIRE_STEPS + 5"
    // (unparenthesized), so the "-X27_FULLWIRE_HOMING_STEPS" below expands
    // to "-635 + 5" = -630, NOT -(635+5) = -640 as the name implies - a
    // real (if minor, ~1.5%) discrepancy between what this homing move
    // actually does and what it looks like it does. Direction sign is
    // otherwise carried over unverified from this stepper's pre-X27
    // homing - confirm it actually winds to (and stops cleanly at) the
    // real end stop before trusting it unattended. Now does 3 swing loops
    // (was 1) as a more thorough self-test.
    VSIstepper.runToNewPosition(-X27_FULLWIRE_HOMING_STEPS);
    VSIstepper.setCurrentPosition(0);

    for (int i = 1; i <= SwingLoops; i++) {
      SendDebug("Loop :" + String(i));
      VSIstepper.runToNewPosition(X27_FULLWIRE_STEPS);
      delay(200);
      VSIstepper.runToNewPosition(0);
      delay(200);
    }

    // Move VSI to zero position and set
    VSIstepper.runToNewPosition((X27_FULLWIRE_STEPS / 2) - VSIoffset);
    VSIstepper.setCurrentPosition(0);
    SendDebug("End VSI");
  }
  // ################# End VSI Startup #########################


  // ################# Start ALT Startup #########################


  if (SwingALT) {
    SendDebug("Start ALT");
    for (int i = 1; i <= 1; i++) {
      SendDebug("Loop :" + String(i));
      ALTstepper.moveTo(-STEPS * 2);
      while (ALTstepper.distanceToGo() != 0) {
        if (digitalRead(ALTzeroSensePin) != true) {
          SendDebug("Found Alt Zero Position");
          ALTstepper.setCurrentPosition(-25);
          break;
        }
        ALTstepper.run();
      }
      delay(500);
      SendDebug("Send Alt Round " + String(SwingLoops) + " times");
      long SendAAltForATrip = Z27_360_FULLWIRE_STEPS * SwingLoops;
      //  720 steps per loop
      ALTstepper.runToNewPosition(SendAAltForATrip);
      delay(200);
      SendDebug("Return Alt to 0");
      ALTstepper.runToNewPosition(0);
    }
    // Move ALT to zero position - need to monitor zero sense



    SendDebug("End ALT");
  }
  // ################# End ALT Startup #########################

  // ################# Start IAS (Current Airspeed) Startup #########################
  // Renamed from "Speed Current" to match IASstepper. Wrapped in
  // `if (false)` - present and compiled, but currently DISABLED: this
  // whole homing/swing sequence never runs at boot. Also note: as
  // written this does two full blocking moves back-to-back with no
  // run()/delay() between them (wind to X27_FULLWIRE_HOMING_STEPS, then
  // immediately wind to -X27_FULLWIRE_STEPS) before zeroing - a bigger
  // back-and-forth swing than VSI's equivalent single approach move, not
  // obviously intentional. Re-verify this sequence before flipping the
  // `if` to true.



  if (SwingIAS) {
    SendDebug("Start IASstepper");
    IASstepper.runToNewPosition(X27_FULLWIRE_HOMING_STEPS);
    IASstepper.runToNewPosition(0);
    IASstepper.setCurrentPosition(0);

    for (int i = 1; i <= SwingLoops; i++) {
      SendDebug("Loop :" + String(i));
      SendDebug("Sending IAS to Max");
      IASstepper.runToNewPosition(X27_FULLWIRE_STEPS);
      delay(200);
      SendDebug("Returning IAS to Zero");
      IASstepper.runToNewPosition(0);
      delay(200);
    }
    SendDebug("End IASstepper");
  }
  //  ################ #End Speed Current Startup######################## #

  // ################# Start Fuel Load Startup #########################
  // Same wind/zero/3-swing-loop pattern as the IAS block above, reusing
  // the same X27_FULLWIRE_STEPS/X27_FULLWIRE_HOMING_STEPS constants (see
  // the macro-precedence caution on VSI's homing above - the same
  // "-X27_FULLWIRE_HOMING_STEPS expands to -630, not -640" issue applies
  // here too). This stepper (pins 32-35) drove Radar Alt on the
  // single-board sketch this was forked from; this board repurposes it as
  // Fuel Load instead (see FuelLoadStepper above), but the physical
  // hardware and homing behaviour is unchanged. Direction sign and step
  // range are an unverified assumption carried over from IAS/VSI's
  // X27-style homing, NOT bench-confirmed for this specific gauge.
  // Wrapped in `if (false)` - present and compiled, but currently
  // DISABLED, matching the single-board sketch's own Radar Alt swing.
  if (false) {
    SendDebug("Start FuelLoadStepper");
    FuelLoadStepper.runToNewPosition(X27_FULLWIRE_HOMING_STEPS);
    FuelLoadStepper.runToNewPosition(-X27_FULLWIRE_STEPS);
    FuelLoadStepper.setCurrentPosition(0);

    for (int i = 1; i <= SwingLoops; i++) {
      SendDebug("Loop :" + String(i));
      SendDebug("Sending Fuel Load to Max");
      FuelLoadStepper.runToNewPosition(X27_FULLWIRE_STEPS);
      delay(200);
      SendDebug("Returning Fuel Load to Zero");
      FuelLoadStepper.runToNewPosition(0);
      delay(200);
    }
    SendDebug("End FuelLoadStepper");
  }
  // ################# End Fuel Load Startup #########################

  // ################# Start Turbine Speed Startup #########################
  // Same wind/zero/3-swing-loop pattern as the IAS/Radar Alt blocks
  // above, reusing the same X27_FULLWIRE_STEPS/X27_FULLWIRE_HOMING_STEPS
  // constants (see the macro-precedence caution on VSI's homing above -
  // the same "-X27_FULLWIRE_HOMING_STEPS expands to -630, not -640"
  // issue applies here too). TSstepper had no startup routine at all
  // before this - direction sign is an unverified assumption carried
  // over from IAS/VSI/Radar Alt's X27-style homing, NOT bench-confirmed
  // for this specific gauge. UPDATE: TS_PCT_TABLE (below, this board's
  // own 4-point table) now gives this gauge's real "RPME" UDP path a max
  // of 600 steps at 110% - close to X27_FULLWIRE_STEPS (635), so this
  // swing's step range is a reasonable match rather than the ~2x
  // overshoot it was before that table existed. Not wrapped in `if
  // (false)` - runs every boot. Confirm on
  // the bench that it actually reaches the real end stop (and doesn't
  // stall against it from the wrong side) before trusting it unattended.
  if (SwingRPM) {
    SendDebug("Start TSstepper");
    TSstepper.runToNewPosition(X27_FULLWIRE_HOMING_STEPS);
    TSstepper.runToNewPosition(0);
    TSstepper.setCurrentPosition(0);

    for (int i = 1; i <= SwingLoops; i++) {
      SendDebug("Loop :" + String(i));
      SendDebug("Sending Turbine Speed to Max");
      TSstepper.runToNewPosition(X27_FULLWIRE_STEPS);
      delay(200);
      // Zero here means the calibrated 0% position (TS_PCT_TABLE's 0
      // step, same as setTS(0) would target), not the stepper's raw
      // internal 0 - TSoffset accounts for the fine-trim applied at
      // runtime by setTS()/tsPctToSteps(), so the swing ends exactly
      // where the gauge will actually rest at 0% RPME.
      SendDebug("Returning Turbine Speed to Zero (offset " + String(TSoffset) + ")");
      TSstepper.runToNewPosition(TSoffset);
      delay(200);
    }
    SendDebug("End TSstepper");
  }
  // ################# End Turbine Speed Startup #########################

  // ################# Start Rotor Speed Startup #########################
  // Same wind/zero/3-swing-loop pattern as the IAS/Radar Alt/Turbine Speed
  // blocks above, reusing the same
  // X27_FULLWIRE_STEPS/X27_FULLWIRE_HOMING_STEPS constants (see the
  // macro-precedence caution on VSI's homing above - the same
  // "-X27_FULLWIRE_HOMING_STEPS expands to -630, not -640" issue applies
  // here too). RSstepper had no startup routine at all before this -
  // direction sign is an unverified assumption carried over from
  // IAS/VSI/Radar Alt/Turbine Speed's X27-style homing, NOT bench-confirmed
  // for this specific gauge. RS_PCT_TABLE (above, this board's own
  // 4-point table) gives this gauge's real "RPMR" UDP path a max of 600
  // steps at 110% - close to X27_FULLWIRE_STEPS (635), same as Turbine
  // Speed, so this swing's step range is a reasonable match rather than
  // a big overshoot. Not wrapped in `if
  // (false)` - runs every boot. Confirm on the bench that it actually
  // reaches the real end stop (and doesn't stall against it from the wrong
  // side) before trusting it unattended.
  if (SwingRPM) {
    SendDebug("Start RSstepper");
    RSstepper.runToNewPosition(X27_FULLWIRE_HOMING_STEPS);
    RSstepper.runToNewPosition(0);
    RSstepper.setCurrentPosition(0);

    for (int i = 1; i <= SwingLoops; i++) {
      SendDebug("Loop :" + String(i));
      SendDebug("Sending Rotor Speed to Max");
      RSstepper.runToNewPosition(X27_FULLWIRE_STEPS);
      delay(200);
      // Same reasoning as TSstepper's swing above: zero here means the
      // calibrated 0% position (RS_PCT_TABLE's 0 step / setRS(0)'s
      // target), not the stepper's raw internal 0 - RSoffset accounts
      // for the fine-trim setRS()/rsPctToSteps() applies at runtime.
      SendDebug("Returning Rotor Speed to Zero (offset " + String(RSoffset) + ")");
      RSstepper.runToNewPosition(RSoffset);
      delay(200);
    }
    SendDebug("End RSstepper");
  }
  // ################# End Rotor Speed Startup #########################



  SendDebug("STEPPER INITIALISATION COMPLETE");

  // ####################### Being OLED Setup ##########################



  for (uint8_t t = 0; t < 8; t++) {
    tcaselect(t);
    // Had to comment out these debugging messages as they created a conflict with the IRQ definition in DCS BIOS
    SendDebug("TCA Port #" + String(t));

    for (uint8_t addr = 0; addr <= 127; addr++) {
      //if (addr == TCAADDR) continue;

      uint8_t data;
      if (!twi_writeTo(addr, &data, 0, 1, 1)) {
        SendDebug("Found I2C " + String(addr));
      }
    }
  }

  SendDebug("I2C scan complete");

  tcaselect(BARO_OLED_Port);

  u8g2_BARO.begin();
  u8g2_BARO.clearBuffer();
  u8g2_BARO.setFont(u8g2_font_logisoso16_tf);
  u8g2_BARO.sendBuffer();
  tcaselect(BARO_OLED_Port);
  updateBARO("2992");


  tcaselect(ALT_OLED_Port);
  u8g2_ALT.begin();
  u8g2_ALT.clearBuffer();
  u8g2_ALT.setFont(u8g2_font_logisoso32_tn);
  u8g2_ALT.sendBuffer();
  tcaselect(ALT_OLED_Port);
  updateALT("0", "0");


  tcaselect(CLOCK_OLED_Port);
  u8g2_CLOCK.begin();
  u8g2_CLOCK.clearBuffer();
  u8g2_CLOCK.setFont(u8g2_font_logisoso16_tf);
  u8g2_CLOCK.sendBuffer();
  tcaselect(CLOCK_OLED_Port);
  updateClock(0, 0);


  // ######################## End OLED Setup ###########################


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
// pass-through this code used before.

// IAS knots-to-step calibration table, hand-measured on the bench (same
// pattern as VSI_FPM_TABLE above). "step" is the raw step target for
// SpeedCurrentstepper.moveTo(). The 0kt row is assumed (not directly
// given) to match this stepper's homed zero, matching every other
// calibration table in this sketch's own convention of 0 real-unit = 0
// steps - confirm on the bench that IAS actually reads 0 (not just
// clamped to the 20kt row) when the aircraft is stopped. Sorted
// ascending by kt - iasKtToSteps() below relies on that order.
struct KtToStepEntry {
  long kt;
  long step;
};

const KtToStepEntry IAS_KT_TABLE[] = {
  { 0, 0 },
  { 20, 24 },
  { 40, 130 },
  { 60, 250 },
  { 80, 364 },
  { 100, 457 },
  { 110, 506 },
  { 120, 543 },
  { 130, 593 },
  { 140, 635 },
};
const int IAS_KT_TABLE_SIZE = sizeof(IAS_KT_TABLE) / sizeof(IAS_KT_TABLE[0]);

// Converts a requested airspeed in knots into a step target by linear
// interpolation between the two nearest IAS_KT_TABLE rows (same pattern
// as vsiFpmToSteps()/radarAltFtToSteps()). A kt value outside the
// table's 0..140 range is clamped to whichever end is nearest rather
// than extrapolated.
long iasKtToSteps(long kt) {
  if (kt <= IAS_KT_TABLE[0].kt) return IAS_KT_TABLE[0].step;
  if (kt >= IAS_KT_TABLE[IAS_KT_TABLE_SIZE - 1].kt) return IAS_KT_TABLE[IAS_KT_TABLE_SIZE - 1].step;

  for (int i = 0; i < IAS_KT_TABLE_SIZE - 1; i++) {
    long ktLo = IAS_KT_TABLE[i].kt;
    long ktHi = IAS_KT_TABLE[i + 1].kt;
    if (kt >= ktLo && kt <= ktHi) {
      long stepLo = IAS_KT_TABLE[i].step;
      long stepHi = IAS_KT_TABLE[i + 1].step;
      return stepLo + (long)round((double)(kt - ktLo) * (stepHi - stepLo) / (double)(ktHi - ktLo));
    }
  }
  return 0;  // unreachable - every kt is covered by the clamps or the loop above
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


void onAltMslFtChange(unsigned int newValue) {
  // Max Value of feet is 65535
  // 720 Steps per 1000 feet
  // So 0.72 steps foot - need float as long doesn't do decimal
  float ALTtargetSteps = newValue;
  ALTtargetSteps = ALTtargetSteps * 0.72;
  long longAlttargetSteps = long(ALTtargetSteps);
  // SendDebug("Altimeter target steps is :" + String(longAlttargetSteps));
  ALTstepper.moveTo(longAlttargetSteps);
  // SendDebug("Altimeter steps to go :" + String(ALTstepper.distanceToGo()));

  // Only touch the OLED if the altitude actually changed since the last
  // callback - iLastAltitudeValue was declared for exactly this but never
  // used before. This is a coarser, cheaper check than the per-digit-group
  // comparison already inside UpdateAltimeterDigits() (which still only
  // redraws the digits/offsets that changed) - it skips calling that
  // function (and its digit-math work) entirely on repeat/no-op DCS-BIOS
  // callbacks. Also throttled to at most once every
  // minAltOledUpdateIntervalMs (300ms), independent of how often
  // onAltMslFtChange() itself fires - a changed value that arrives inside
  // the throttle window is simply deferred (iLastAltitudeValue isn't
  // advanced), so the next eligible callback still picks it up rather
  // than the update being silently dropped. ALTstepper still moves every
  // call, regardless of either gate.
  if ((int)newValue != iLastAltitudeValue
      && (millis() - lastAltOledUpdateMillis) >= minAltOledUpdateIntervalMs) {
    UpdateAltimeterDigits(newValue);
    iLastAltitudeValue = (int)newValue;
    lastAltOledUpdateMillis = millis();
  }
}
DcsBios::IntegerBuffer altMslFtBuffer(CommonData_ALT_MSL_FT, onAltMslFtChange);

// ################################### END ALT ##############################################


// ################################### START EGT ##############################################

// EGT (Exhaust Gas Temp) real-value UDP handler - see the "ITT" case in
// HandleOutputValuePair() below (renamed from "EGT" to match
// JET_RANGER_SERVO_CONTROLLER.ino), which sends degrees C instead of a
// raw step target. Straight linear scale across the gauge's real-world
// 0-900C range onto FULL4WIRE_HOMING_STEPS - CAUTION: that constant was
// redefined from 315*2 (630) to 315+5 (320) elsewhere in this sketch
// (see the pin/stepper section above), which halves this gauge's
// effective step resolution without this comment (or EGTstepper's own
// homing) having been updated to match - EGTstepper still has no actual
// homing routine, so "FULL4WIRE_HOMING_STEPS" here is still a borrowed
// placeholder ceiling, not a bench-measured one. NOT a real per-point
// calibration table like VSI_FPM_TABLE/IAS_KT_TABLE - revisit with real
// bench-measured points once EGTstepper's actual travel is known.
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

// Turbine Speed (RPME) percent-to-step calibration table, hand-measured
// on the bench specifically for this board (diverges from
// JET_RANGER_STEPPER_CONTROLLER.ino's TS_PCT_TABLE, which still has its
// own separate 13-point 0-117% table - the two are no longer identical).
// "step" is the raw step target for TSstepper.moveTo(). The 0-point is
// assumed (not directly given) to match this stepper's homed zero, same
// convention as IAS_KT_TABLE's assumed 0kt row - confirm on the bench
// that RPME actually reads 0 (not just clamped to the 55% row) at rest.
// Sorted ascending by pct - tsPctToSteps() below relies on that order.
// NOTE: this table's max (110% -> 600 steps) is close to
// X27_FULLWIRE_STEPS (635), same reasoning as before for why TSstepper's
// startup swing (above) isn't a large overshoot relative to this gauge's
// real full-scale range.
struct PctToStepEntry {
  long pct;
  long step;
};

const PctToStepEntry TS_PCT_TABLE[] = {
  { 0, 0 },
  { 55, 300 },
  { 100, 535 },
  { 110, 600 },
};
const int TS_PCT_TABLE_SIZE = sizeof(TS_PCT_TABLE) / sizeof(TS_PCT_TABLE[0]);

// Converts a requested turbine speed in percent into a step target by
// linear interpolation between the two nearest TS_PCT_TABLE rows (same
// pattern as vsiFpmToSteps()/iasKtToSteps()). A pct value outside the
// table's 0..110 range is clamped to whichever end is nearest rather
// than extrapolated.
long tsPctToSteps(long pct) {
  if (pct <= TS_PCT_TABLE[0].pct) return TS_PCT_TABLE[0].step;
  if (pct >= TS_PCT_TABLE[TS_PCT_TABLE_SIZE - 1].pct) return TS_PCT_TABLE[TS_PCT_TABLE_SIZE - 1].step;

  for (int i = 0; i < TS_PCT_TABLE_SIZE - 1; i++) {
    long pctLo = TS_PCT_TABLE[i].pct;
    long pctHi = TS_PCT_TABLE[i + 1].pct;
    if (pct >= pctLo && pct <= pctHi) {
      long stepLo = TS_PCT_TABLE[i].step;
      long stepHi = TS_PCT_TABLE[i + 1].step;
      return stepLo + (long)round((double)(pct - pctLo) * (stepHi - stepLo) / (double)(pctHi - pctLo));
    }
  }
  return 0;  // unreachable - every pct is covered by the clamps or the loop above
}

// TSoffset (fine-trim step offset for TS_PCT_TABLE's computed target) is
// defined near VSIoffset above, not here - it has to come before setup()
// since the Turbine Speed startup swing (above) also uses it to return
// to the calibrated zero position, not just this function.

void setTS(long TargetPct) {
  TSstepper.moveTo(tsPctToSteps(TargetPct) + TSoffset);
}

// Rotor Speed (RPMR) percent-to-step calibration table, hand-measured on
// the bench specifically for this board - same values as this board's
// own TS_PCT_TABLE above (both gauges share the same physical
// stepper/dial hardware and percent range), but kept as its own
// table/function pair rather than reused, matching the per-gauge pattern
// used throughout this file. Diverges from
// JET_RANGER_STEPPER_CONTROLLER.ino's RS_PCT_TABLE, which still has its
// own separate 13-point 0-117% table. "step" is the raw step target for
// RSstepper.moveTo(). The 0-point is assumed (not directly given), same
// convention as TS_PCT_TABLE's above. Sorted ascending by pct -
// rsPctToSteps() below relies on that order.
const PctToStepEntry RS_PCT_TABLE[] = {
  { 0, 0 },
  { 55, 300 },
  { 100, 535 },
  { 110, 600 },
};
const int RS_PCT_TABLE_SIZE = sizeof(RS_PCT_TABLE) / sizeof(RS_PCT_TABLE[0]);

// Converts a requested rotor speed in percent into a step target by
// linear interpolation between the two nearest RS_PCT_TABLE rows (same
// pattern as tsPctToSteps() above). A pct value outside the table's
// 0..110 range is clamped to whichever end is nearest rather than
// extrapolated.
long rsPctToSteps(long pct) {
  if (pct <= RS_PCT_TABLE[0].pct) return RS_PCT_TABLE[0].step;
  if (pct >= RS_PCT_TABLE[RS_PCT_TABLE_SIZE - 1].pct) return RS_PCT_TABLE[RS_PCT_TABLE_SIZE - 1].step;

  for (int i = 0; i < RS_PCT_TABLE_SIZE - 1; i++) {
    long pctLo = RS_PCT_TABLE[i].pct;
    long pctHi = RS_PCT_TABLE[i + 1].pct;
    if (pct >= pctLo && pct <= pctHi) {
      long stepLo = RS_PCT_TABLE[i].step;
      long stepHi = RS_PCT_TABLE[i + 1].step;
      return stepLo + (long)round((double)(pct - pctLo) * (stepHi - stepLo) / (double)(pctHi - pctLo));
    }
  }
  return 0;  // unreachable - every pct is covered by the clamps or the loop above
}

// RSoffset (fine-trim step offset for RS_PCT_TABLE's computed target) is
// defined near VSIoffset above, not here - same reasoning as TSoffset's
// relocation note in setTS() above.

void setRS(long TargetPct) {
  RSstepper.moveTo(rsPctToSteps(TargetPct) + RSoffset);
}

// #define GP_MIN_PCT 0
// #define GP_MAX_PCT 105
// long gpPctToSteps(long pct) {
//   if (pct < GP_MIN_PCT) pct = GP_MIN_PCT;
//   if (pct > GP_MAX_PCT) pct = GP_MAX_PCT;
//   return map(pct, GP_MIN_PCT, GP_MAX_PCT, 0, FULL4WIRE_HOMING_STEPS);
// }
// void setGP(long TargetPct) {
//   GPstepper.moveTo(gpPctToSteps(TargetPct));
// }

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

// ################################### START FUEL LOAD ##############################################

// No real calibration table exists yet for Fuel Load (this stepper was
// Radar Alt - and had a real AGL_FT_TABLE - on the single-board sketch
// this was forked from; that table described radar altitude in feet, not
// fuel load, so it was removed rather than reused/renamed). Until a real
// hand-measured percent-or-gallon-to-step table is provided, FUELLOAD is
// raw steps only - see the UDP handler below.

// ################################### END FUEL LOAD ##############################################


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
  ALTstepper.run();
  IASstepper.run();
  FuelLoadStepper.run();
  EOTstepper.run();
  XOTstepper.run();
  XOPstepper.run();
  EGTstepper.run();
  TSstepper.run();
  RSstepper.run();
  FAstepper.run();
  ElectricalLoadStepper.run();
  //GPstepper.run();
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
  } else if (ParameterName == "ALT") {
    // ALT is sent as raw feet, matching the units this sketch's own
    // DCS-BIOS altitude callback already expects, so its feet->steps
    // conversion can be reused directly.
    //SendDebug("Altitude is :" + String(ParameterValue.toInt()));
    onAltMslFtChange((unsigned int)ParameterValue.toInt());
  } else if (ParameterName == "ALTRAW") {
    // Distinct raw-step code, bypassing the feet*5.76 conversion above.
    ALTstepper.moveTo(ParameterValue.toInt());
  } else if (ParameterName == "ZULU") {
    // HHMM-encoded Zulu time (e.g. 1430 for 14:30) for the Clock OLED -
    // no DCS-BIOS callback for this one, FSUIPCWinformsAutoCS is the
    // only source. See onZuluTimeChange()/updateClock() above.
    onZuluTimeChange(ParameterValue.toInt());
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
  } else if (ParameterName == "FUELLOAD") {
    // Raw steps only - no real calibration table exists yet for this
    // gauge (see "START FUEL LOAD" above). This stepper (pins 32-35) was
    // Radar Alt (UDP code "AGL", real AGL_FT_TABLE calibration) on the
    // single-board sketch this was forked from; this board repurposes it
    // as Fuel Load instead, so "AGL"/"AGLRAW" no longer exist here -
    // replaced by this single raw-step code until a real percent- or
    // gallon-to-step table is provided.
    FuelLoadStepper.moveTo(ParameterValue.toInt());
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
    // Real percent now (Turbine/Engine Speed, 0-110 - see this board's own
    // TS_PCT_TABLE above, which diverges from the single-board sketch's
    // table). Renamed from "TS" to match JET_RANGER_SERVO_CONTROLLER.ino's
    // "RPME" (Engine RPM) - same real-world quantity.
    setTS(ParameterValue.toInt());
  } else if (ParameterName == "RPMERAW") {
    // Distinct raw-step code, bypassing tsPctToSteps() above.
    TSstepper.moveTo(ParameterValue.toInt());
  } else if (ParameterName == "RPMR") {
    // Real percent now (Rotor Speed, 0-110 - see this board's own
    // RS_PCT_TABLE above, which diverges from the single-board sketch's
    // table). Renamed from "RS" to match JET_RANGER_SERVO_CONTROLLER.ino.
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
  } else if (ParameterName == "ELECTRICALLOAD") {
    // Raw steps only - no real calibration table exists yet for this
    // gauge. This stepper (pins 36-39) was Torque (UDP code "TQ") on the
    // single-board sketch this was forked from; this board repurposes it
    // as Electrical Load instead, so "TQ" no longer exists here -
    // replaced by this raw-step code until a real calibration table is
    // provided.
    ElectricalLoadStepper.moveTo(ParameterValue.toInt());
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

  if ((millis() - lastalivesent) >= aliveinterval) {
    if (Ethernet_In_Use == 1) {
      aliveudp.beginPacket(reflectorIP, aliveport);
      aliveudp.print("DUAL_STEPPER");
      aliveudp.endPacket();
    }
    lastalivesent = millis();
  }

  currentMillis = millis();
}
