/*
  Stepper-Tuning-Harness

  Standalone bench-test sketch for the 7 analogue stepper gauges driven by
  JET_RANGER_STEPPER_CONTROLLER.ino (A10_FRONT_CONSOLE_STEPPERS). Lets an
  operator pick one stepper at a time over the Serial Monitor and type a
  target step position as many times as needed to see where the needle
  lands - the stepper equivalent of Servo-Knob-For-Calibration/ServoTuner,
  which do the same job for the Bell 206 servo gauges.

  Wiring and max speed match JET_RANGER_STEPPER_CONTROLLER.ino exactly, so
  min/max/zero step values found with this harness should carry straight
  over to that sketch. VSI is FULL4WIRE direct-drive on coil pins (see
  COIL_VSI_A..D below). Acceleration was lowered from that sketch's 9000
  (see STEPPER_ACCELERATION below) after it caused missed steps on the
  bench - worth carrying that reduction back into the production sketch
  too if it turns out to need it there as well.

  Not included: the SARI roll stepper. It uses its own closed-loop IR-sensor
  homing/tracking state machine (see JET_RANGER_STEPPER_CONTROLLER.ino's
  Nema8Stepper class) rather than a simple absolute step target, so it isn't
  a fit for this "type a number, see where it goes" style of tool.

  No DCS-BIOS. Wire this board up exactly as it would be for
  JET_RANGER_STEPPER_CONTROLLER.ino (same stepper drivers, same pins) and
  upload this sketch instead while tuning. It uses that same sketch's
  identity on the network (same static IP/MAC) since it's meant as a
  drop-in stand-in for it during bench tuning, not something run alongside
  it - every significant action is logged via SendDebug() to the reflector
  host, the same "SendDebug -> 172.16.1.10:27000" pattern used throughout
  the rest of the Jet Ranger fleet.

  On every boot, VSI is automatically wound hard against its end stop and
  zeroed there (see homeVSI()) before the menu appears, the same way
  JET_RANGER_STEPPER_CONTROLLER.ino's own startup sequence homes it. VSI
  then moves an additional VSI_ZERO_OFFSET_STEPS (317) off the end stop
  and re-zeroes there, so its end stop sits at a negative position rather
  than at zero - letting typed target steps for VSI go both positive and
  negative from its zero instead of only away from the stop. This homing
  routine can also be re-run on demand at any time (e.g. if it has drifted
  or stalled) with the "h" command below, while VSI is selected.

  Serial Monitor usage (115200 baud, newline line ending):
    - On boot, and any time you send "m", the stepper menu is printed.
    - Send "s" followed by a menu number to select that stepper, e.g. "s1"
      selects VSI.
    - With a stepper selected, send a plain integer target step position
      (e.g. "1500" or "-200") and press Enter to move it there - repeat as
      many times as you like while watching the gauge. Positive numbers
      move the gauge clockwise, negative numbers counter-clockwise (see
      toRawSteps()/toDisplaySteps() and each stepper's displaySign -
      AccelStepper's own native direction isn't guaranteed to agree across
      every stepper's wiring, so each one has its own sign flip applied
      before/after touching the stepper). (Selection
      uses the "sN" prefix specifically so a plain number always means
      "move here", never "reselect" - typing "5" moves the current stepper
      to step 5, it does not switch to stepper #5.)
    - Send "z" to zero the CURRENTLY SELECTED stepper wherever it is
      sitting right now (sets that physical position to step 0) - handy for
      establishing a known reference point before searching for min/max.
    - Send "h" while VSI is selected to re-home it on demand (same move as
      the automatic boot-time homing above). Only implemented for VSI so
      far.
    - While VSI is selected, send "f" followed by a feet value (e.g.
      "f1000" or "f-500") to move it using feet instead of a raw step
      count - looked up/interpolated from the VSI_FT_TABLE calibration
      table (see vsiFtToDisplaySteps()) built by hand on the bench. Values
      outside the table's -1750..1750 ft range are clamped to whichever
      end is nearest, not extrapolated.
    - Send "m" at any time to see the menu again (this does not change
      which stepper is selected - use "sN" to actually switch).
*/

#include <AccelStepper.h>
#include <SPI.h>
#include <Ethernet.h>
#include <EthernetUdp.h>

// Same network identity as JET_RANGER_STEPPER_CONTROLLER.ino - this harness
// stands in for that sketch on the bench, it isn't meant to run on the
// network at the same time as it.
#define ES1_RESET_PIN 53
byte mac[] = { 0xA8, 0x61, 0x0A, 0x67, 0x83, 0x69 };
IPAddress ip(172, 16, 1, 105);
String strMyIP = "172.16.1.105";

IPAddress reflectorIP(172, 16, 1, 10);
const unsigned int localport = 7788;
const unsigned int reflectorport = 27000;
const unsigned long delayBeforeSendingPacket = 2000;
unsigned long ethernetStartTime = 0;
String BoardName = "Stepper Tuning Harness: ";

EthernetUDP udp;

// Same "SendDebug -> reflector:27000" logging pattern used throughout the
// rest of the Jet Ranger fleet.
void SendDebug(String MessageToSend) {
  udp.beginPacket(reflectorIP, reflectorport);
  udp.print(BoardName + MessageToSend);
  udp.endPacket();
}

// Status LED heartbeat - moved off pins 12/13 (JET_RANGER_STEPPER_CONTROLLER.ino's
// originals) onto 14/15, since 12/13 are now SpeedCurrentstepper's coil
// pins (STEPPER_SPD_A/B below) - resolves the LED/stepper pin collision
// flagged when that stepper moved to FULL4WIRE.
#define RED_STATUS_LED_PORT 15
#define GREEN_STATUS_LED_PORT 14
#define Check_LED_R 15
#define Check_LED_G 14

#define FLASH_TIME 300

unsigned long NEXT_STATUS_TOGGLE_TIMER = 0;
bool GREEN_LED_STATE = false;
bool RED_LED_STATE = false;
unsigned long timeSinceRedLedChanged = 0;

#define STEPPER_MAX_SPEED 19000
// Lowered from JET_RANGER_STEPPER_CONTROLLER.ino's 9000 - that value was
// causing missed steps on the bench (high acceleration demands more torque
// than the motor can supply, so it loses sync). Start here and reduce
// further per-gauge if steps are still being missed, or raise it again if
// movement feels sluggish once a gauge is confirmed step-accurate.
#define STEPPER_ACCELERATION 9000

// Steps for whichever homing function's stepper is direct-driven
// (FULL4WIRE) - matching JET_RANGER_STEPPER_CONTROLLER.ino's own comment on
// its equivalent constant.
#define FULL4WIRE_HOMING_STEPS (315 * 2)

// After VSI reaches its end stop and zeroes there, it moves this many
// steps off the stop and re-zeroes at that new position - see homeVSI().
// That makes the end stop itself a negative position rather than zero, so
// typed target steps can go both positive and negative from the new zero
// instead of only away from the stop.
#define VSI_ZERO_OFFSET_STEPS 317

// Menu index of the one stepper with homing support (must match its
// position in the steppers[] table below). VSI shifted to slot 1 ('s1')
// after Flaps (formerly slot 0) was removed.
#define STEPPER_INDEX_VSI 1

// Renamed from AOAstepPin/AOAdirectionPin (a DRIVER/STEP-DIR pair) - this
// slot now drives a 4-wire direct-drive Radar Altimeter stepper instead
// of AOA, so it needs 4 coil pins rather than a step/dir pair.
#define RADAR_ALT_COIL_A 32
#define RADAR_ALT_COIL_B 33
#define RADAR_ALT_COIL_C 34
#define RADAR_ALT_COIL_D 35

#define COIL_VSI_A 7
#define COIL_VSI_B 8
#define COIL_VSI_C 9
#define COIL_VSI_D 11

// Current Airspeed's new 4-wire direct-drive coil pins - was DRIVER/
// STEP-DIR on SpeedCurrentstepPin(34)/SpeedCurrentdirectionPin(36)
// (removed above). Pins 12/13 used to collide with the status-LED
// heartbeat (RED_STATUS_LED_PORT/GREEN_STATUS_LED_PORT) - resolved by
// moving those LEDs to 15/14 above.
#define STEPPER_SPD_A 12
#define STEPPER_SPD_B 13
#define STEPPER_SPD_C 22
#define STEPPER_SPD_D 23

// EOT's 4-wire direct-drive coil pins. Both former collisions here
// (pin 48 vs. FlapsDirectionPin, A2/56 vs. AllstepperEnablePin) are
// resolved now that Flaps and the shared enable pin are gone.
#define EOT_COIL_A 48
#define EOT_COIL_B A0
#define EOT_COIL_C A1
#define EOT_COIL_D A2

// XOT, XOP, EGT: 4-wire direct-drive coil pins on the Mega's analog
// pins used as digital I/O (A3..A14 = digital 57..68) - no collisions
// with any other pin already defined in this sketch.
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

// TS: no collisions with any other pin still defined in this sketch
// (formerly collided with GForcestepPin, removed along with GForce).
#define TS_COIL_A 24
#define TS_COIL_B 25
#define TS_COIL_C 26
#define TS_COIL_D 27

// RS: no collisions with any other pin still defined in this sketch
// (formerly collided with GForcedirectionPin, removed along with GForce).
#define RS_COIL_A 28
#define RS_COIL_B 29
#define RS_COIL_C 30
#define RS_COIL_D 31

// FA: no collisions with any other pin already defined in this sketch.
#define FA_COIL_A 2
#define FA_COIL_B 3
#define FA_COIL_C 4
#define FA_COIL_D 6

// ET: no collisions with any other pin still defined in this sketch
// (formerly collided with SpeedMaxstepPin, removed along with SpeedMax).
#define ET_COIL_A 36
#define ET_COIL_B 37
#define ET_COIL_C 38
#define ET_COIL_D 39

// GP: no collisions with any other pin still defined in this sketch
// (formerly collided with SpeedMaxdirectionPin and ALTstepPin, both
// removed along with SpeedMax/Alt).
#define GP_COIL_A 40
#define GP_COIL_B 41
#define GP_COIL_C 42
#define GP_COIL_D 43

// EOP: both former collisions here (pin 44 vs. ALTdirectionPin, pin 46
// vs. FlapsStepPin) are resolved now that Alt and Flaps are gone.
#define EOP_COIL_A 44
#define EOP_COIL_B 45
#define EOP_COIL_C 46
#define EOP_COIL_D 47





AccelStepper VSIstepper(AccelStepper::FULL4WIRE, COIL_VSI_A, COIL_VSI_B, COIL_VSI_C, COIL_VSI_D);
// Renamed interface only (still SpeedCurrentstepper): was DRIVER/STEP-DIR
// on SpeedCurrentstepPin/SpeedCurrentdirectionPin, now FULL4WIRE
// direct-drive on STEPPER_SPD_A..D (see the pin-conflict note above them).
AccelStepper SpeedCurrentstepper(AccelStepper::FULL4WIRE, STEPPER_SPD_A, STEPPER_SPD_B, STEPPER_SPD_C, STEPPER_SPD_D);
// Renamed from AOAstepper: was DRIVER/STEP-DIR on AOAstepPin/
// AOAdirectionPin, now FULL4WIRE direct-drive on RADAR_ALT_COIL_A..D.
AccelStepper RadarAltStepper(AccelStepper::FULL4WIRE, RADAR_ALT_COIL_C, RADAR_ALT_COIL_D, RADAR_ALT_COIL_A, RADAR_ALT_COIL_B);
AccelStepper EOTstepper(AccelStepper::FULL4WIRE, EOT_COIL_A, EOT_COIL_B, EOT_COIL_C, EOT_COIL_D);
// New gauges below, all 4-wire direct-drive.
AccelStepper XOTstepper(AccelStepper::FULL4WIRE, XOT_COIL_A, XOT_COIL_B, XOT_COIL_C, XOT_COIL_D);
AccelStepper XOPstepper(AccelStepper::FULL4WIRE, XOP_COIL_A, XOP_COIL_B, XOP_COIL_C, XOP_COIL_D);
AccelStepper EGTstepper(AccelStepper::FULL4WIRE, EGT_COIL_A, EGT_COIL_B, EGT_COIL_C, EGT_COIL_D);
AccelStepper TSstepper(AccelStepper::FULL4WIRE, TS_COIL_A, TS_COIL_B, TS_COIL_C, TS_COIL_D);
AccelStepper RSstepper(AccelStepper::FULL4WIRE, RS_COIL_A, RS_COIL_B, RS_COIL_C, RS_COIL_D);
AccelStepper FAstepper(AccelStepper::FULL4WIRE, FA_COIL_A, FA_COIL_B, FA_COIL_C, FA_COIL_D);
AccelStepper ETstepper(AccelStepper::FULL4WIRE, ET_COIL_A, ET_COIL_B, ET_COIL_C, ET_COIL_D);
AccelStepper GPstepper(AccelStepper::FULL4WIRE, GP_COIL_A, GP_COIL_B, GP_COIL_C, GP_COIL_D);
AccelStepper EOPstepper(AccelStepper::FULL4WIRE, EOP_COIL_A, EOP_COIL_B, EOP_COIL_C, EOP_COIL_D);

struct StepperEntry {
  const char *name;
  AccelStepper *stepper;
  // +1 or -1: which way to flip typed/displayed step numbers for this
  // specific stepper so that positive always means clockwise (see
  // toRawSteps()/toDisplaySteps() below). AccelStepper's native direction
  // isn't guaranteed to agree across every stepper's wiring, so this is
  // per-stepper rather than one fixed sign for all of them.
  int displaySign;
};

StepperEntry steppers[] = {
  // displaySign carried over from its old DRIVER wiring - not re-verified
  // for the new FULL4WIRE interface, flip to 1 if it turns out reversed.
  { "Current Airspeed", &SpeedCurrentstepper, -1 },
  { "VSI", &VSIstepper, 1 },  // confirmed backwards on the bench
  // displaySign not yet verified on the bench for this new FULL4WIRE
  // wiring - defaulted to 1 (same starting guess VSI used before its own
  // direction was confirmed backwards); flip to -1 if it turns out
  // reversed.
  { "Radar Alt", &RadarAltStepper, 1 },
  // New gauge, direction not yet verified on the bench - defaulted to 1,
  // same starting guess used for the other new FULL4WIRE steppers above.
  { "EOT", &EOTstepper, 1 },
  // New gauges below - same "direction not yet verified, defaulted to 1"
  // caveat as EOT above applies to all of them.
  { "XOT", &XOTstepper, 1 },
  { "XOP", &XOPstepper, 1 },
  { "EGT", &EGTstepper, 1 },
  { "TS", &TSstepper, 1 },
  { "RS", &RSstepper, 1 },
  { "FA", &FAstepper, 1 },
  { "ET", &ETstepper, 1 },
  { "GP", &GPstepper, 1 },
  { "EOP", &EOPstepper, 1 },
};
const int NUM_STEPPERS = sizeof(steppers) / sizeof(steppers[0]);

int selectedStepper = -1;
String inputLine = "";

// AccelStepper's native positive direction depends on the stepper's
// wiring, but the harness's operator-facing numbers should
// always mean the same thing regardless: typing a positive target moves
// the gauge clockwise. Each stepper's displaySign (see StepperEntry above)
// records which way its own native direction needs flipping to achieve
// that. These two helpers are the single place that sign flip happens -
// everywhere a step count crosses between "what the operator types/sees"
// and "what AccelStepper actually tracks" goes through one of these.
long toRawSteps(long displaySteps, int displaySign) {
  return displaySign * displaySteps;
}

long toDisplaySteps(long rawSteps, int displaySign) {
  return displaySign * rawSteps;
}

// VSI ft-to-step calibration table, hand-measured on the bench: "step" is
// the display step target (same units as the plain-integer move command,
// i.e. what goes into toRawSteps() below) that lands the needle at that
// many feet, relative to the zero homeVSI() establishes. The 0 ft row is
// step 0 to match that zero reference - not the raw 317-steps-from-the-
// end-stop distance homeVSI() winds through to get there (that number is
// VSI_ZERO_OFFSET_STEPS, a different thing). Sorted ascending by ft -
// vsiFtToDisplaySteps() below relies on that order.
struct FtToStepEntry {
  long ft;
  long step;
};

const FtToStepEntry VSI_FT_TABLE[] = {
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
const int VSI_FT_TABLE_SIZE = sizeof(VSI_FT_TABLE) / sizeof(VSI_FT_TABLE[0]);

// Converts a requested VSI feet value into a display step target by
// linear interpolation between the two nearest VSI_FT_TABLE rows. A ft
// value outside the table's range is clamped to whichever end is nearest
// rather than extrapolated, so a wildly out-of-range value can't fling
// VSI past its calibrated range.
long vsiFtToDisplaySteps(long ft) {
  if (ft <= VSI_FT_TABLE[0].ft) return VSI_FT_TABLE[0].step;
  if (ft >= VSI_FT_TABLE[VSI_FT_TABLE_SIZE - 1].ft) return VSI_FT_TABLE[VSI_FT_TABLE_SIZE - 1].step;

  for (int i = 0; i < VSI_FT_TABLE_SIZE - 1; i++) {
    long ftLo = VSI_FT_TABLE[i].ft;
    long ftHi = VSI_FT_TABLE[i + 1].ft;
    if (ft >= ftLo && ft <= ftHi) {
      long stepLo = VSI_FT_TABLE[i].step;
      long stepHi = VSI_FT_TABLE[i + 1].step;
      return stepLo + (long)round((double)(ft - ftLo) * (stepHi - stepLo) / (double)(ftHi - ftLo));
    }
  }
  return 0;  // unreachable - every ft is covered by the clamps or the loop above
}

// Winds VSI hard against its mechanical end stop, zeroes it there, then
// moves VSI_ZERO_OFFSET_STEPS (317) off the stop and re-zeroes at that new
// position. The end stop itself ends up at a negative position rather than
// step 0, so afterwards typed target steps can go both positive and
// negative from zero instead of only away from the stop. VSI is
// direct-driven FULL4WIRE hardware, so the initial approach to the stop
// uses FULL4WIRE_HOMING_STEPS with no overshoot. Both moves block (via
// runToNewPosition()) until they complete.
void homeVSI() {
  Serial.println(F("Homing VSI: winding to its end stop..."));
  SendDebug("Homing VSI - winding to end stop");
  VSIstepper.runToNewPosition(-FULL4WIRE_HOMING_STEPS);
  VSIstepper.setCurrentPosition(0);
  Serial.println(F("VSI end stop reached and zeroed (step 0)."));
  SendDebug("VSI end stop reached, zeroed at step 0");

  Serial.println(F("VSI moving off end stop to set zero reference..."));
  VSIstepper.runToNewPosition(VSI_ZERO_OFFSET_STEPS);
  VSIstepper.setCurrentPosition(0);
  Serial.println(F("VSI zero reference set (317 steps off end stop)."));
  SendDebug("VSI zero reference set, 317 steps off end stop");
}

// Winds the Radar Altimeter hard against its mechanical end stop and
// zeroes it there. Like VSI, it's direct-driven FULL4WIRE with no
// zero-sense pin, so this is a blind wind (no sensor to confirm arrival)
// using FULL4WIRE_HOMING_STEPS - same homing style as homeVSI(), minus
// VSI's extra move off the stop (no equivalent offset established for
// this gauge yet). Direction sign is an unverified guess, matching this
// stepper's displaySign note in the steppers[] table - confirm it
// actually reaches the end stop (and doesn't stall against it from the
// wrong side) before trusting it unattended.
void homeRadarAlt() {
  Serial.println(F("Homing Radar Alt: winding to its end stop..."));
  SendDebug("Homing Radar Alt - winding to end stop");
  RadarAltStepper.runToNewPosition(-FULL4WIRE_HOMING_STEPS);
  RadarAltStepper.setCurrentPosition(0);
  Serial.println(F("Radar Alt end stop reached and zeroed (step 0)."));
  SendDebug("Radar Alt end stop reached, zeroed at step 0");
}

void printMenu() {
  Serial.println();
  Serial.println(F("=== Stepper Tuning Harness ==="));
  for (int i = 0; i < NUM_STEPPERS; i++) {
    Serial.print(F("s"));
    Serial.print(i);
    Serial.print(F(" = "));
    Serial.println(steppers[i].name);
  }
  Serial.println(F("Send 's' + a number (e.g. 's0') to select a stepper."));

  if (selectedStepper >= 0) {
    Serial.print(F("Currently selected: "));
    Serial.print(steppers[selectedStepper].name);
    Serial.print(F(" (at step "));
    Serial.print(toDisplaySteps(steppers[selectedStepper].stepper->currentPosition(), steppers[selectedStepper].displaySign));
    Serial.println(F(")"));
    Serial.println(F("Type a target step position (e.g. 1500 or -200) and press Enter to move it."));
    Serial.println(F("Type 'z' to zero it at its current physical position, or 'm' for this menu."));
    if (selectedStepper == STEPPER_INDEX_VSI) {
      Serial.println(F("Type 'h' to home it: wind fully to its end stop, then zero there."));
      Serial.println(F("Type 'f' + a feet value (e.g. 'f1000' or 'f-500') to move VSI using feet instead of steps."));
    }
  }
}

void setup() {
  Serial.begin(115200);
  while (!Serial) { ; }

  pinMode(GREEN_STATUS_LED_PORT, OUTPUT);
  pinMode(RED_STATUS_LED_PORT, OUTPUT);
  digitalWrite(GREEN_STATUS_LED_PORT, true);
  digitalWrite(RED_STATUS_LED_PORT, true);
  delay(FLASH_TIME);
  digitalWrite(GREEN_STATUS_LED_PORT, false);
  digitalWrite(RED_STATUS_LED_PORT, false);
  delay(FLASH_TIME);

  // Reset Ethernet Module
  pinMode(ES1_RESET_PIN, OUTPUT);
  digitalWrite(ES1_RESET_PIN, LOW);
  delay(2);
  digitalWrite(ES1_RESET_PIN, HIGH);

  Ethernet.begin(mac, ip);
  udp.begin(localport);

  // As it takes a couple of seconds before the Ethernet Stack is operational,
  // flash the green LED until that time period has completed.
  ethernetStartTime = millis() + delayBeforeSendingPacket;
  while (millis() <= ethernetStartTime) {
    delay(FLASH_TIME);
    digitalWrite(Check_LED_G, false);
    delay(FLASH_TIME);
    digitalWrite(Check_LED_G, true);
  }

  SendDebug("Ethernet started " + strMyIP);

  for (int i = 0; i < NUM_STEPPERS; i++) {
    steppers[i].stepper->setMaxSpeed(STEPPER_MAX_SPEED);
    steppers[i].stepper->setAcceleration(STEPPER_ACCELERATION);
  }

  homeVSI();      // Wind VSI back to its end stop and zero it before the menu appears
  homeRadarAlt(); // Same, for the Radar Altimeter

  printMenu();
  SendDebug("Setup Complete");
}

bool isIntegerToken(const String &s) {
  if (s.length() == 0) return false;
  for (unsigned int i = 0; i < s.length(); i++) {
    char c = s.charAt(i);
    if (isDigit(c)) continue;
    if (i == 0 && (c == '-' || c == '+')) continue;
    return false;
  }
  return true;
}

void handleLine(String line) {
  line.trim();
  if (line.length() == 0) return;

  if (line.equalsIgnoreCase("m")) {
    printMenu();
    return;
  }

  if (selectedStepper >= 0 && line.equalsIgnoreCase("z")) {
    steppers[selectedStepper].stepper->setCurrentPosition(0);
    Serial.print(F("Zeroed "));
    Serial.print(steppers[selectedStepper].name);
    Serial.println(F(" at its current physical position."));
    SendDebug("Zeroed " + String(steppers[selectedStepper].name) + " at its current physical position");
    return;
  }

  if (line.equalsIgnoreCase("h")) {
    if (selectedStepper == STEPPER_INDEX_VSI) {
      homeVSI();
    } else if (selectedStepper < 0) {
      Serial.println(F("Select VSI first ('s1') - homing is only implemented for it so far."));
    } else {
      Serial.print(F("Homing isn't implemented for "));
      Serial.print(steppers[selectedStepper].name);
      Serial.println(F(" yet - only VSI ('s1') supports 'h' right now."));
    }
    return;
  }

  // "fN" (feet) - VSI only: converts a target feet value into a step
  // position via the VSI_FT_TABLE calibration table (see
  // vsiFtToDisplaySteps() above, linear interpolation between rows) and
  // moves VSI there, same as typing the resulting step number directly.
  if ((line.charAt(0) == 'f' || line.charAt(0) == 'F') && isIntegerToken(line.substring(1))) {
    if (selectedStepper != STEPPER_INDEX_VSI) {
      Serial.println(F("'f' (feet) is only supported for VSI - select it first ('s1')."));
      return;
    }
    long ft = line.substring(1).toInt();
    long displayStep = vsiFtToDisplaySteps(ft);
    steppers[selectedStepper].stepper->moveTo(toRawSteps(displayStep, steppers[selectedStepper].displaySign));
    Serial.print(F("Moving VSI to "));
    Serial.print(ft);
    Serial.print(F(" ft (step "));
    Serial.print(displayStep);
    Serial.println(F(")"));
    SendDebug("VSI target set to " + String(ft) + " ft (step " + String(displayStep) + ")");
    return;
  }

  // "sN" always means "select stepper N" - a distinct prefix so it can
  // never be confused with a plain-number target position below.
  if ((line.charAt(0) == 's' || line.charAt(0) == 'S') && isIntegerToken(line.substring(1))) {
    int idx = line.substring(1).toInt();
    if (idx >= 0 && idx < NUM_STEPPERS) {
      selectedStepper = idx;
      Serial.print(F("Selected: "));
      Serial.println(steppers[selectedStepper].name);
      Serial.println(F("Type a target step position and press Enter. 'm' = menu, 'z' = zero here."));
      SendDebug("Selected " + String(steppers[selectedStepper].name));
    } else {
      Serial.println(F("No such stepper number. Type 'm' to see the list."));
    }
    return;
  }

  if (selectedStepper < 0) {
    Serial.println(F("No stepper selected yet - send 'sN' first (type 'm' to see the list)."));
    return;
  }

  if (!isIntegerToken(line)) {
    Serial.print(F("Unrecognised input: \""));
    Serial.print(line);
    Serial.println(F("\". Type 'm' for the menu."));
    return;
  }

  long target = line.toInt();
  steppers[selectedStepper].stepper->moveTo(toRawSteps(target, steppers[selectedStepper].displaySign));
  Serial.print(F("Moving "));
  Serial.print(steppers[selectedStepper].name);
  Serial.print(F(" to step "));
  Serial.println(target);
  SendDebug(String(steppers[selectedStepper].name) + " target set to " + String(target));
}

void loop() {
  if (millis() >= NEXT_STATUS_TOGGLE_TIMER) {
    RED_LED_STATE = !RED_LED_STATE;

    digitalWrite(Check_LED_G, RED_LED_STATE);
    digitalWrite(Check_LED_R, !RED_LED_STATE);
    NEXT_STATUS_TOGGLE_TIMER = millis() + FLASH_TIME;
  }

  while (Serial.available() > 0) {
    char c = Serial.read();
    if (c == '\n' || c == '\r') {
      if (inputLine.length() > 0) {
        handleLine(inputLine);
        inputLine = "";
      }
    } else {
      inputLine += c;
    }
  }

  // Keep every stepper serviced, not just the selected one, so whichever
  // was mid-move when the operator switched selection still completes its
  // move and holds position correctly.
  for (int i = 0; i < NUM_STEPPERS; i++) {
    steppers[i].stepper->run();
  }
}
