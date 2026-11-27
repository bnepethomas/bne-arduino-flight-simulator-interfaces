/*
  IR projector remote: sends the projector remote's infrared codes when a push button is pressed, the projector power switch is
  moved, or a command is typed on the serial port.

  Hardware: an Uno with the IR Projector Remote shield (Jet Ranger/PCBs/IR Projector Remote), or the same wiring by hand:
    IR LED transmitter on D3: D3 -> 1 k -> base of an NPN transistor (BC547), emitter to GND,
      collector to the cathodes of two IR LEDs, each fed from 5 V through its own 100 ohm resistor.
    Projector power switch (SPST) between A5 and GND:
      switch CLOSED (turned on)  -> STARTUP is sent
      switch OPENED (turned off) -> SHUTDOWN is sent
      Commands are sent only when the switch changes. Its position at power-up sends nothing.
    Status LED on D7 (through 330 ohm to GND): flashes twice a second while the sketch is running.
    Push buttons, each between its pin and GND (the Uno's internal pull-ups are used, so no resistors):
      A0 POWER   A1 UP   A2 DOWN   A3 LEFT   D5 OK   D6 RIGHT
      A press sends the code once (holding a button does not repeat it).
  The buttons and the switch are debounced for DEBOUNCE_MS.

  Library: IRremote (Arduino-IRremote), version 4.

  Serial: 115200 baud, one command per line:
    POWER   LEFT   RIGHT   UP   DOWN   OK
    STARTUP   power, wait 35 s, LEFT, wait 1 s, OK (the buttons and other commands keep working while it waits)
    SHUTDOWN  power, wait 1 s, OK
    OFF       power, wait 1 s, power again
    CANCEL    stops the sequence that is running
    CMD 15    send any NEC command number (hex), to try codes that have not been captured
  Replies "SENT <command>" or "UNKNOWN <text>".

  The codes were captured with IR_CODE_LEARNER on 27 November 2026: NEC, address 0x00.
*/

#include <IRremote.hpp>

#define IR_SEND_PIN_NUMBER 3
#define POWER_SWITCH_PIN A5
#define STATUS_LED_PIN 7
#define PROJECTOR_ADDRESS 0x00
#define STATUS_TOGGLE_MS 250UL         // on 250 ms, off 250 ms: two flashes a second
#define DEBOUNCE_MS 30                 // a push button or the switch must hold its new state this long before it counts
#define STARTUP_WARMUP_MS 35000UL      // STARTUP: wait after POWER before LEFT
#define STARTUP_OK_DELAY_MS 1000UL     // STARTUP: wait after LEFT before OK
#define SHUTDOWN_OK_DELAY_MS 1000UL    // SHUTDOWN: wait after POWER before OK
#define POWER_CONFIRM_DELAY_MS 1000UL  // OFF: gap between the two presses of power

struct SequenceStep {
  unsigned long waitMs;  // after the previous step
  const char *name;      // the entry of buttons[] it sends
};

struct Sequence {
  const char *name;
  const SequenceStep *steps;
  uint8_t stepCount;
};

struct Button {
  const char *name;
  uint8_t command;
};

// A debounced input that is active when it connects its pin to ground.
struct DebouncedInput {
  uint8_t pin;
  const char *name;      // for a push button, the entry of buttons[] it sends
  bool active;           // debounced state
  bool lastReading;      // raw state at the last look
  unsigned long changedAt;
};

const Button buttons[] = {
  { "POWER", 0x14 },
  { "LEFT", 0x0E },
  { "RIGHT", 0x1A },
  { "UP", 0x03 },
  { "DOWN", 0x02 },
  { "OK", 0x07 },
};
const uint8_t buttonCount = sizeof(buttons) / sizeof(buttons[0]);

DebouncedInput pushButtons[] = {
  { A0, "POWER", false, false, 0 },
  { A1, "UP", false, false, 0 },
  { A2, "DOWN", false, false, 0 },
  { A3, "LEFT", false, false, 0 },
  { 6, "RIGHT", false, false, 0 },
  { 5, "OK", false, false, 0 },
};
const uint8_t pushButtonCount = sizeof(pushButtons) / sizeof(pushButtons[0]);

DebouncedInput powerSwitch = { POWER_SWITCH_PIN, "SWITCH", false, false, 0 };
bool switchActedOn = false;  // the switch position already dealt with; a difference from powerSwitch.active is waiting to be sent

char line[24];
uint8_t lineLength = 0;

void sendCommand(uint8_t command) {
  IrSender.sendNEC(PROJECTOR_ADDRESS, command, 0);  // one frame, no repeats: a single press
}

bool sendButton(const char *name) {
  for (uint8_t i = 0; i < buttonCount; i++) {
    if (strcmp(name, buttons[i].name) == 0) {
      sendCommand(buttons[i].command);
      return true;
    }
  }
  return false;
}

// The sequences run from loop() without blocking: each step is sent when its wait since the step before has passed.
// Only one sequence runs at a time.
const SequenceStep startupSteps[] = {
  { 0, "POWER" },
  { STARTUP_WARMUP_MS, "LEFT" },
  { STARTUP_OK_DELAY_MS, "OK" },
};
const SequenceStep shutdownSteps[] = {
  { 0, "POWER" },
  { SHUTDOWN_OK_DELAY_MS, "OK" },
};
const SequenceStep offSteps[] = {
  { 0, "POWER" },
  { POWER_CONFIRM_DELAY_MS, "POWER" },
};
const Sequence sequences[] = {
  { "STARTUP", startupSteps, sizeof(startupSteps) / sizeof(startupSteps[0]) },
  { "SHUTDOWN", shutdownSteps, sizeof(shutdownSteps) / sizeof(shutdownSteps[0]) },
  { "OFF", offSteps, sizeof(offSteps) / sizeof(offSteps[0]) },
};
const uint8_t sequenceCount = sizeof(sequences) / sizeof(sequences[0]);

const Sequence *running = nullptr;  // the sequence being sent, or nullptr
uint8_t nextStep = 0;
unsigned long stepSentAt = 0;

const Sequence *findSequence(const char *name) {
  for (uint8_t i = 0; i < sequenceCount; i++) {
    if (strcmp(name, sequences[i].name) == 0) return &sequences[i];
  }
  return nullptr;
}

// Refused while a sequence is running: an extra POWER would upset the projector.
bool startSequence(const Sequence *sequence) {
  if (running != nullptr) {
    Serial.print(running->name);
    Serial.println(F(" is still running (CANCEL stops it)"));
    return false;
  }
  running = sequence;
  nextStep = 0;
  stepSentAt = millis();
  return true;
}

void runSequence() {
  if (running == nullptr) return;
  const SequenceStep &step = running->steps[nextStep];
  if (millis() - stepSentAt < step.waitMs) return;
  sendButton(step.name);
  stepSentAt = millis();
  Serial.print(running->name);
  Serial.print(F(" sent "));
  Serial.println(step.name);
  if (++nextStep >= running->stepCount) {
    Serial.print(running->name);
    Serial.println(F(" done"));
    running = nullptr;
  }
}

void handleLine() {
  for (char *p = line; *p; p++) *p = toupper(*p);
  bool sent = true;
  if (findSequence(line) != nullptr) {
    if (!startSequence(findSequence(line))) return;
  } else if (strcmp(line, "CANCEL") == 0) {
    running = nullptr;
    switchActedOn = powerSwitch.active;  // also drop a switch change that was waiting
  } else if (strncmp(line, "CMD ", 4) == 0) {
    char *end;
    long value = strtol(line + 4, &end, 16);
    sent = end != line + 4 && *end == 0 && value >= 0 && value <= 0xFF;
    if (sent) sendCommand((uint8_t)value);
  } else {
    sent = sendButton(line);
  }
  Serial.print(sent ? F("SENT ") : F("UNKNOWN "));
  Serial.println(line);
}

// Non-blocking debounce: a reading counts once it has been steady for DEBOUNCE_MS. Returns true when the debounced state changes.
bool updateInput(DebouncedInput &input) {
  bool reading = digitalRead(input.pin) == LOW;
  if (reading != input.lastReading) {
    input.lastReading = reading;
    input.changedAt = millis();
  } else if (reading != input.active && millis() - input.changedAt >= DEBOUNCE_MS) {
    input.active = reading;
    return true;
  }
  return false;
}

void scanPushButtons() {
  for (uint8_t i = 0; i < pushButtonCount; i++) {
    if (updateInput(pushButtons[i]) && pushButtons[i].active && sendButton(pushButtons[i].name)) {
      Serial.print(F("SENT "));
      Serial.println(pushButtons[i].name);
    }
  }
}

// STARTUP when the switch closes, SHUTDOWN when it opens, once per change. A change made while a sequence is running waits for
// it to finish; if the switch is put back before then, nothing is sent.
void servicePowerSwitch() {
  if (updateInput(powerSwitch)) {
    Serial.println(powerSwitch.active ? F("Switch ON") : F("Switch OFF"));
    if (running != nullptr && powerSwitch.active != switchActedOn) {
      Serial.print(running->name);
      Serial.println(F(" is running: the switch change will follow it"));
    }
  }
  if (powerSwitch.active != switchActedOn && running == nullptr) {
    switchActedOn = powerSwitch.active;
    startSequence(findSequence(switchActedOn ? "STARTUP" : "SHUTDOWN"));
  }
}

void flashStatusLed() {
  digitalWrite(STATUS_LED_PIN, (millis() / STATUS_TOGGLE_MS) % 2 == 0 ? HIGH : LOW);
}

void setup() {
  Serial.begin(115200);
  IrSender.begin(IR_SEND_PIN_NUMBER);
  pinMode(STATUS_LED_PIN, OUTPUT);
  pinMode(powerSwitch.pin, INPUT_PULLUP);
  for (uint8_t i = 0; i < pushButtonCount; i++) {
    pinMode(pushButtons[i].pin, INPUT_PULLUP);
  }
  delay(5);  // let the pull-ups settle
  // The positions at start-up are the starting state: nothing is sent for them.
  for (uint8_t i = 0; i < pushButtonCount; i++) {
    pushButtons[i].active = pushButtons[i].lastReading = digitalRead(pushButtons[i].pin) == LOW;
  }
  powerSwitch.active = powerSwitch.lastReading = switchActedOn = digitalRead(powerSwitch.pin) == LOW;
  Serial.println(F("IR projector remote ready on D3: POWER STARTUP SHUTDOWN OFF CANCEL LEFT RIGHT UP DOWN OK, or CMD <hex>"));
  Serial.println(powerSwitch.active ? F("Switch is ON (nothing sent)") : F("Switch is OFF (nothing sent)"));
}

void loop() {
  flashStatusLed();
  scanPushButtons();
  servicePowerSwitch();
  runSequence();
  while (Serial.available()) {
    char c = Serial.read();
    if (c == '\n' || c == '\r') {
      if (lineLength > 0) {
        line[lineLength] = 0;
        lineLength = 0;
        handleLine();
      }
    } else if (lineLength < sizeof(line) - 1) {
      line[lineLength++] = c;
    }
  }
}
