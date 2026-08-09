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
  over to that sketch. (VSI and Flaps had their AccelStepper
  interfaces/pins swapped here first - VSI is FULL4WIRE on the pins that
  used to be Flaps' coils, Flaps is DRIVER/STEP-DIR on the pins that used
  to be VSI's step/dir - and JET_RANGER_STEPPER_CONTROLLER.ino has since
  received the matching real-world wiring change and the same correction,
  so the two are back in sync for these two steppers too.) Acceleration
  was lowered from that sketch's 9000 (see STEPPER_ACCELERATION below)
  after it caused missed steps on the bench - worth carrying that
  reduction back into the production sketch too if it turns out to need it
  there as well.

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

  On every boot, VSI and Flaps are each automatically wound hard against
  their end stop and zeroed there (see homeVSI()/homeFlaps()) before the
  menu appears, the same way JET_RANGER_STEPPER_CONTROLLER.ino's own
  startup sequence homes them. VSI then moves an additional
  VSI_ZERO_OFFSET_STEPS (317) off the end stop and re-zeroes there, so its
  end stop sits at a negative position rather than at zero - letting typed
  target steps for VSI go both positive and negative from its zero instead
  of only away from the stop. Either homing routine can also be re-run on
  demand at any time (e.g. if it has drifted or stalled) with the "h" command below,
  while that stepper is selected.

  Serial Monitor usage (115200 baud, newline line ending):
    - On boot, and any time you send "m", the stepper menu is printed.
    - Send "s" followed by a menu number to select that stepper, e.g. "s0"
      selects Flaps, "s4" selects VSI.
    - With a stepper selected, send a plain integer target step position
      (e.g. "1500" or "-200") and press Enter to move it there - repeat as
      many times as you like while watching the gauge. Positive numbers
      move the gauge clockwise, negative numbers counter-clockwise (see
      toRawSteps()/toDisplaySteps() and each stepper's displaySign -
      AccelStepper's own native direction isn't the same for every
      stepper's wiring/interface type, so each one has its own sign flip
      applied before/after touching the stepper; Flaps is inverted
      relative to the rest). (Selection
      uses the "sN" prefix specifically so a plain number always means
      "move here", never "reselect" - typing "5" moves the current stepper
      to step 5, it does not switch to stepper #5.)
    - Send "z" to zero the CURRENTLY SELECTED stepper wherever it is
      sitting right now (sets that physical position to step 0) - handy for
      establishing a known reference point before searching for min/max.
    - Send "h" while VSI or Flaps is selected to re-home it on demand (same
      move as the automatic boot-time homing above). Only implemented for
      those two so far.
    - While VSI is selected, send "f" followed by a feet value (e.g.
      "f1000" or "f-500") to move it using feet instead of a raw step
      count - looked up/interpolated from the VSI_FT_TABLE calibration
      table (see vsiFtToDisplaySteps()) built by hand on the bench. Values
      outside the table's -1750..1750 ft range are clamped to whichever
      end is nearest, not extrapolated.
    - Send "m" at any time to see the menu again (this does not change
      which stepper is selected - use "sN" to actually switch).
    - Send "d" at any time to force the Flaps DIR pin HIGH directly, or "e"
      to force it LOW (diagnostic - bypasses AccelStepper entirely via
      digitalWrite(), does not move the stepper). Note AccelStepper will
      overwrite this the next time Flaps actually steps, so it only holds
      until Flaps' next move. (Moved from VSI to Flaps when VSI and Flaps
      swapped AccelStepper interfaces/pins - only a DRIVER/STEP-DIR
      stepper has a DIR pin at all, and that's Flaps now.)
    - This also happens automatically: as soon as Flaps finishes a move (or
      is already sitting still) its DIR pin is latched HIGH on its own,
      same as sending "d" by hand - no action needed to trigger it.
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

// Status LED heartbeat - ported as-is from JET_RANGER_STEPPER_CONTROLLER.ino
#define RED_STATUS_LED_PORT 12
#define GREEN_STATUS_LED_PORT 13
#define Check_LED_R 12
#define Check_LED_G 13

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

// Steps for an unmodified Vid-series stepper/driver (matches
// JET_RANGER_STEPPER_CONTROLLER.ino's STEPS). Used for whichever homing
// function's stepper is geared/DRIVER-interfaced - deliberately overshot by
// 10% to guarantee reaching the real mechanical end stop regardless of
// exactly how many steps it actually is. This overshoot follows the
// hardware, not a fixed name - see the VSI/Flaps AccelStepper construct
// swap below.
#define DRIVER_HOMING_STEPS (315 * 16)

// Steps for whichever homing function's stepper is direct-driven
// (FULL4WIRE) - matching JET_RANGER_STEPPER_CONTROLLER.ino's own comment on
// its equivalent constant, direct-drive doesn't need the 10% overshoot
// DRIVER_HOMING_STEPS gets.
#define FULL4WIRE_HOMING_STEPS (315 * 2)

// After VSI reaches its end stop and zeroes there, it moves this many
// steps off the stop and re-zeroes at that new position - see homeVSI().
// That makes the end stop itself a negative position rather than zero, so
// typed target steps can go both positive and negative from the new zero
// instead of only away from the stop.
#define VSI_ZERO_OFFSET_STEPS 317

// Menu indices of the two steppers with homing support (must match their
// position in the steppers[] table below). Swapped from the original 0/4 -
// Flaps is now menu slot 0 ('s0') and VSI is menu slot 4 ('s4'); wiring/pins
// are unchanged, only which menu number selects which gauge.
#define STEPPER_INDEX_VSI 4
#define STEPPER_INDEX_FLAPS 0

// Shared stepper-driver enable pin (matches JET_RANGER_STEPPER_CONTROLLER.ino)
#define AllstepperEnablePin 56

// Swapped with VSI's coil pins below: this DRIVER-interface STEP/DIR pair
// physically belongs to whichever hardware is now wired here, and that's
// now Flaps rather than VSI.
#define FlapsStepPin 46
#define FlapsDirectionPin 48

#define ALTstepPin 42
#define ALTdirectionPin 44

#define SpeedCurrentstepPin 34
#define SpeedCurrentdirectionPin 36

#define SpeedMaxstepPin 38
#define SpeedMaxdirectionPin 40

#define AOAstepPin 22
#define AOAdirectionPin 24

#define GForcestepPin 26
#define GForcedirectionPin 28

// Swapped with Flaps' step/dir pins above: these 4 coil pins physically
// belong to whichever hardware is now wired here, and that's now VSI
// rather than Flaps.
#define COIL_VSI_A 2
#define COIL_VSI_B 3
#define COIL_VSI_C 4
#define COIL_VSI_D 5

#define STEPPER_SPD_A 12
#define STEPPER_SPD_B 13
#define STEPPER_SPD_C 22
#define STEPPER_SPD_D 23






// VSI and Flaps swapped AccelStepper interfaces/pins below: VSI is now the
// direct-driven FULL4WIRE hardware (was Flaps' pins/interface), Flaps is
// now the geared DRIVER/STEP-DIR hardware (was VSI's pins/interface).
AccelStepper VSIstepper(AccelStepper::FULL4WIRE, COIL_VSI_A, COIL_VSI_B, COIL_VSI_C, COIL_VSI_D);
AccelStepper ALTstepper(AccelStepper::DRIVER, ALTstepPin, ALTdirectionPin);
AccelStepper SpeedCurrentstepper(AccelStepper::DRIVER, SpeedCurrentstepPin, SpeedCurrentdirectionPin);
AccelStepper SpeedMaxstepper(AccelStepper::DRIVER, SpeedMaxstepPin, SpeedMaxdirectionPin);
AccelStepper FlapsStepper(AccelStepper::DRIVER, FlapsStepPin, FlapsDirectionPin);
AccelStepper AOAstepper(AccelStepper::DRIVER, AOAstepPin, AOAdirectionPin);
AccelStepper GForcestepper(AccelStepper::DRIVER, GForcestepPin, GForcedirectionPin);

struct StepperEntry {
  const char *name;
  AccelStepper *stepper;
  // +1 or -1: which way to flip typed/displayed step numbers for this
  // specific stepper so that positive always means clockwise (see
  // toRawSteps()/toDisplaySteps() below). Steppers on different interface
  // types (VSI's FULL4WIRE vs Flaps' DRIVER/STEP-DIR) don't necessarily
  // share the same native direction, so this is per-stepper rather than
  // one fixed sign for all of them.
  int displaySign;
};

// VSI and Flaps swapped menu slots (0 and 4) below - see STEPPER_INDEX_VSI/
// STEPPER_INDEX_FLAPS above. displaySign values also swapped between them
// to follow the AccelStepper construct/pin swap above - each sign is a
// property of the physical hardware, not the name attached to it.
StepperEntry steppers[] = {
  { "Flaps", &FlapsStepper, -1 },  // now on the old VSI hardware/pins - inherits its sign
  { "Altimeter", &ALTstepper, -1 },
  { "Current Airspeed", &SpeedCurrentstepper, -1 },
  { "Max Airspeed", &SpeedMaxstepper, -1 },
  { "VSI", &VSIstepper, 1 },  // now on the old Flaps hardware/pins - inherits its sign (confirmed backwards on the bench)
  { "AOA", &AOAstepper, -1 },
  { "G-Force", &GForcestepper, -1 },
};
const int NUM_STEPPERS = sizeof(steppers) / sizeof(steppers[0]);

int selectedStepper = -1;
String inputLine = "";

// Tracks whether the Flaps DIR pin has already been auto-latched HIGH for
// the current stop, so the log message in loop() below fires once per
// arrival rather than every iteration while Flaps sits idle at its target.
// (Moved from VSI to Flaps along with the DRIVER/STEP-DIR interface itself.)
bool flapsDirLatchedHigh = false;

// AccelStepper's native positive direction depends on the stepper's
// interface type (VSI's FULL4WIRE vs Flaps' DRIVER/STEP-DIR aren't
// guaranteed to agree), but the harness's operator-facing numbers should
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
// negative from zero instead of only away from the stop. VSI is now the
// direct-driven FULL4WIRE hardware (was Flaps' pins/interface - see the
// AccelStepper construct swap above), so the initial approach to the stop
// uses FULL4WIRE_HOMING_STEPS with no overshoot, matching that hardware's
// own homing style. The negative sign for that first move is carried over
// from what that same physical hardware needed as homeFlaps() before the
// swap - NOT re-verified on the bench under its new name, check it
// actually reaches the end stop (and doesn't stall against it from the
// wrong side) before trusting it. Both moves block (via
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

// Winds Flaps to its mechanical end stop and zeroes it there. Flaps is now
// the geared DRIVER/STEP-DIR hardware (was VSI's pins/interface - see the
// AccelStepper construct swap above), so this uses DRIVER_HOMING_STEPS with
// the 10% overshoot that hardware's own homing style needs. The positive
// sign is carried over from what that same physical hardware needed as
// homeVSI() before the swap - NOT re-verified on the bench under its new
// name, check it actually reaches the end stop (and doesn't stall against
// it from the wrong side) before trusting it.
void homeFlaps() {
  Serial.println(F("Homing Flaps: winding to its end stop..."));
  SendDebug("Homing Flaps - winding to end stop");
  FlapsStepper.runToNewPosition(DRIVER_HOMING_STEPS * 1.1);
  FlapsStepper.setCurrentPosition(0);
  Serial.println(F("Flaps end stop reached and zeroed (step 0)."));
  SendDebug("Flaps end stop reached, zeroed at step 0");
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
  Serial.println(F("Send 'd'/'e' to force the Flaps DIR pin HIGH/LOW directly (diagnostic, bypasses AccelStepper)."));

  if (selectedStepper >= 0) {
    Serial.print(F("Currently selected: "));
    Serial.print(steppers[selectedStepper].name);
    Serial.print(F(" (at step "));
    Serial.print(toDisplaySteps(steppers[selectedStepper].stepper->currentPosition(), steppers[selectedStepper].displaySign));
    Serial.println(F(")"));
    Serial.println(F("Type a target step position (e.g. 1500 or -200) and press Enter to move it."));
    Serial.println(F("Type 'z' to zero it at its current physical position, or 'm' for this menu."));
    if (selectedStepper == STEPPER_INDEX_VSI || selectedStepper == STEPPER_INDEX_FLAPS) {
      Serial.println(F("Type 'h' to home it: wind fully to its end stop, then zero there."));
    }
    if (selectedStepper == STEPPER_INDEX_VSI) {
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

  pinMode(AllstepperEnablePin, OUTPUT);
  digitalWrite(AllstepperEnablePin, false);  // Enable stepper drivers

  for (int i = 0; i < NUM_STEPPERS; i++) {
    steppers[i].stepper->setMaxSpeed(STEPPER_MAX_SPEED);
    steppers[i].stepper->setAcceleration(STEPPER_ACCELERATION);
  }

  homeVSI();    // Wind VSI back to its end stop and zero it before the menu appears
  homeFlaps();  // Same, for Flaps

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

  // Diagnostic only: drives the Flaps DIR pin HIGH/LOW directly via
  // digitalWrite(), bypassing AccelStepper entirely - does not move the
  // stepper, and AccelStepper will overwrite the pin the next time Flaps
  // actually steps. Moved here from VSI along with the DRIVER/STEP-DIR
  // interface itself - only that interface has an actual DIR pin.
  if (line.equalsIgnoreCase("d")) {
    digitalWrite(FlapsDirectionPin, HIGH);
    Serial.println(F("Flaps DIR pin forced HIGH."));
    SendDebug("Flaps DIR pin forced HIGH");
    return;
  }

  if (line.equalsIgnoreCase("e")) {
    digitalWrite(FlapsDirectionPin, LOW);
    Serial.println(F("Flaps DIR pin forced LOW."));
    SendDebug("Flaps DIR pin forced LOW");
    return;
  }

  if (line.equalsIgnoreCase("h")) {
    if (selectedStepper == STEPPER_INDEX_VSI) {
      homeVSI();
    } else if (selectedStepper == STEPPER_INDEX_FLAPS) {
      homeFlaps();
    } else if (selectedStepper < 0) {
      Serial.println(F("Select VSI or Flaps first ('s4' or 's0') - homing is only implemented for those so far."));
    } else {
      Serial.print(F("Homing isn't implemented for "));
      Serial.print(steppers[selectedStepper].name);
      Serial.println(F(" yet - only VSI ('s4') and Flaps ('s0') support 'h' right now."));
    }
    return;
  }

  // "fN" (feet) - VSI only: converts a target feet value into a step
  // position via the VSI_FT_TABLE calibration table (see
  // vsiFtToDisplaySteps() above, linear interpolation between rows) and
  // moves VSI there, same as typing the resulting step number directly.
  if ((line.charAt(0) == 'f' || line.charAt(0) == 'F') && isIntegerToken(line.substring(1))) {
    if (selectedStepper != STEPPER_INDEX_VSI) {
      Serial.println(F("'f' (feet) is only supported for VSI - select it first ('s4')."));
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

  // Once Flaps has finished moving (reached its target), latch its DIR pin
  // HIGH directly via digitalWrite() - the same action as the manual 'd'
  // command - rather than leaving it wherever its last step happened to
  // set it. Edge-detected so this only fires once per arrival; it resets
  // as soon as Flaps is given a new target (distanceToGo() != 0 again), and
  // AccelStepper will overwrite the pin the moment Flaps actually steps.
  // Moved here from VSI along with the DRIVER/STEP-DIR interface itself -
  // only that interface has an actual DIR pin.
  if (FlapsStepper.distanceToGo() == 0) {
    if (!flapsDirLatchedHigh) {
      digitalWrite(FlapsDirectionPin, HIGH);
      flapsDirLatchedHigh = true;
      SendDebug("Flaps DIR pin auto-forced HIGH (reached target)");
    }
  } else {
    flapsDirLatchedHigh = false;
  }
}
