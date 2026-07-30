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

  No DCS-BIOS. Wire this board up exactly as it would be for
  JET_RANGER_STEPPER_CONTROLLER.ino (same stepper drivers, same pins) and
  upload this sketch instead while tuning. It uses that same sketch's
  identity on the network (same static IP/MAC) since it's meant as a
  drop-in stand-in for it during bench tuning, not something run alongside
  it - every significant action is logged via SendDebug() to the reflector
  host, the same "SendDebug -> 172.16.1.10:27000" pattern used throughout
  the rest of the Jet Ranger fleet.

  On every boot, VSI is automatically wound hard against its negative
  mechanical end stop and zeroed there (see homeVSI()) before the menu
  appears, the same way JET_RANGER_STEPPER_CONTROLLER.ino's own startup
  sequence homes it. It can also be re-run on demand at any time (e.g. if
  VSI has drifted or stalled) with the "h" command below.

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
    - Send "h" while VSI is selected to re-home it on demand (same move as
      the automatic boot-time homing above). Only implemented for VSI so far.
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

#define STEPPER_MAX_SPEED 9000
#define STEPPER_ACCELERATION 9000

// Steps for an unmodified Vid-series stepper/driver (matches
// JET_RANGER_STEPPER_CONTROLLER.ino's STEPS). Used only for VSI homing
// below - deliberately overshot by 10% to guarantee reaching the real
// mechanical end stop regardless of exactly how many steps it actually is.
#define STEPS (315 * 16)

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

// Winds VSI hard against its negative mechanical end stop and zeroes it
// there - the same homing move JET_RANGER_STEPPER_CONTROLLER.ino's own
// startup sequence performs for VSI. This blocks (via runToNewPosition())
// until the move completes, same as that sketch's own homing.
void homeVSI() {
  Serial.println(F("Homing VSI: winding to its end stop..."));
  SendDebug("Homing VSI - winding to end stop");
  VSIstepper.runToNewPosition(-STEPS * 1.1);
  VSIstepper.setCurrentPosition(0);
  Serial.println(F("VSI end stop reached and zeroed (step 0)."));
  SendDebug("VSI end stop reached, zeroed at step 0");
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
    Serial.print(steppers[selectedStepper].stepper->currentPosition());
    Serial.println(F(")"));
    Serial.println(F("Type a target step position (e.g. 1500 or -200) and press Enter to move it."));
    Serial.println(F("Type 'z' to zero it at its current physical position, or 'm' for this menu."));
    if (selectedStepper == 0) {
      Serial.println(F("Type 'h' to home it: wind fully to its end stop, then zero there."));
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

  homeVSI();  // Wind VSI back to its end stop and zero it before the menu appears

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
    if (selectedStepper == 0) {  // menu index 0 = VSI
      homeVSI();
    } else if (selectedStepper < 0) {
      Serial.println(F("Select VSI first ('s0') - homing is only implemented for VSI so far."));
    } else {
      Serial.print(F("Homing isn't implemented for "));
      Serial.print(steppers[selectedStepper].name);
      Serial.println(F(" yet - only VSI ('s0') supports 'h' right now."));
    }
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
  steppers[selectedStepper].stepper->moveTo(target);
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
