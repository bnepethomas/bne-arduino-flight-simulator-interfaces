/*
  Stepper-Tuning-Harness

  Standalone bench-test sketch for the 7 analogue stepper gauges driven by
  JET_RANGER_STEPPER_CONTROLLER.ino (A10_FRONT_CONSOLE_STEPPERS). Lets an
  operator pick one stepper at a time over the Serial Monitor and type a
  target step position as many times as needed to see where the needle
  lands - the stepper equivalent of Servo-Knob-For-Calibration/ServoTuner,
  which do the same job for the Bell 206 servo gauges.

  Wiring, max speed and acceleration all match JET_RANGER_STEPPER_CONTROLLER.ino
  exactly, so any min/max/zero step values found with this harness should
  carry straight over to that sketch.

  Not included: the SARI roll stepper. It uses its own closed-loop IR-sensor
  homing/tracking state machine (see JET_RANGER_STEPPER_CONTROLLER.ino's
  Nema8Stepper class) rather than a simple absolute step target, so it isn't
  a fit for this "type a number, see where it goes" style of tool.

  No Ethernet, no DCS-BIOS - Serial only. Wire this board up exactly as it
  would be for JET_RANGER_STEPPER_CONTROLLER.ino (same stepper drivers, same
  pins) and upload this sketch instead while tuning.

  Serial Monitor usage (115200 baud, newline line ending):
    - On boot, and any time you send "m", the stepper menu is printed.
    - Send "s" followed by a menu number to select that stepper, e.g. "s0"
      selects VSI, "s4" selects Flaps.
    - With a stepper selected, send a plain integer target step position
      (e.g. "1500" or "-200") and press Enter to move it there - repeat as
      many times as you like while watching the gauge. (Selection uses the
      "sN" prefix specifically so a plain number always means "move here",
      never "reselect" - typing "5" moves the current stepper to step 5,
      it does not switch to stepper #5.)
    - Send "z" to zero the CURRENTLY SELECTED stepper wherever it is
      sitting right now (sets that physical position to step 0) - handy for
      establishing a known reference point before searching for min/max.
    - Send "m" at any time to see the menu again (this does not change
      which stepper is selected - use "sN" to actually switch).
*/

#include <AccelStepper.h>

#define STEPPER_MAX_SPEED 9000
#define STEPPER_ACCELERATION 9000

// Shared stepper-driver enable pin (matches JET_RANGER_STEPPER_CONTROLLER.ino)
#define AllstepperEnablePin 56

#define VSIstepPin 46
#define VSIdirectionPin 48

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

#define COIL_FLAPS_A 2
#define COIL_FLAPS_B 3
#define COIL_FLAPS_C 4
#define COIL_FLAPS_D 5

AccelStepper VSIstepper(AccelStepper::DRIVER, VSIstepPin, VSIdirectionPin);
AccelStepper ALTstepper(AccelStepper::DRIVER, ALTstepPin, ALTdirectionPin);
AccelStepper SpeedCurrentstepper(AccelStepper::DRIVER, SpeedCurrentstepPin, SpeedCurrentdirectionPin);
AccelStepper SpeedMaxstepper(AccelStepper::DRIVER, SpeedMaxstepPin, SpeedMaxdirectionPin);
AccelStepper FlapsStepper(AccelStepper::FULL4WIRE, COIL_FLAPS_A, COIL_FLAPS_B, COIL_FLAPS_C, COIL_FLAPS_D);
AccelStepper AOAstepper(AccelStepper::DRIVER, AOAstepPin, AOAdirectionPin);
AccelStepper GForcestepper(AccelStepper::DRIVER, GForcestepPin, GForcedirectionPin);

struct StepperEntry {
  const char *name;
  AccelStepper *stepper;
};

StepperEntry steppers[] = {
  { "VSI", &VSIstepper },
  { "Altimeter", &ALTstepper },
  { "Current Airspeed", &SpeedCurrentstepper },
  { "Max Airspeed", &SpeedMaxstepper },
  { "Flaps", &FlapsStepper },
  { "AOA", &AOAstepper },
  { "G-Force", &GForcestepper },
};
const int NUM_STEPPERS = sizeof(steppers) / sizeof(steppers[0]);

int selectedStepper = -1;
String inputLine = "";

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
    Serial.print(steppers[selectedStepper].stepper->currentPosition());
    Serial.println(F(")"));
    Serial.println(F("Type a target step position (e.g. 1500 or -200) and press Enter to move it."));
    Serial.println(F("Type 'z' to zero it at its current physical position, or 'm' for this menu."));
  }
}

void setup() {
  Serial.begin(115200);
  while (!Serial) { ; }

  pinMode(AllstepperEnablePin, OUTPUT);
  digitalWrite(AllstepperEnablePin, false);  // Enable stepper drivers

  for (int i = 0; i < NUM_STEPPERS; i++) {
    steppers[i].stepper->setMaxSpeed(STEPPER_MAX_SPEED);
    steppers[i].stepper->setAcceleration(STEPPER_ACCELERATION);
  }

  printMenu();
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
  steppers[selectedStepper].stepper->moveTo(target);
  Serial.print(F("Moving "));
  Serial.print(steppers[selectedStepper].name);
  Serial.print(F(" to step "));
  Serial.println(target);
}

void loop() {
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
