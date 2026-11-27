/*
  IR projector remote: sends the projector remote's infrared codes when a push button is pressed or a command is typed on the serial port.

  Hardware (Uno, Nano or Mega 2560): IR LED transmitter on D3:
    D3 -> 1 k -> base of an NPN transistor (2N2222 or BC337), emitter to GND,
    5 V -> R1 (47 to 100 ohm) -> IR LED (anode to cathode) -> collector.
  Push buttons, each between its pin and GND (the Uno's internal pull-ups are used, so no resistors):
    A0 POWER   A1 UP   A2 DOWN   A3 LEFT   D5 OK   D6 RIGHT
    A5 STARTUP   A4 SHUTDOWN (start the sequences below)
  A press sends the code, or starts the sequence, once (holding a button does not repeat it). The inputs are debounced for DEBOUNCE_MS.

  Library: IRremote (Arduino-IRremote), version 4.

  Serial: 115200 baud, one command per line:
    POWER   LEFT   RIGHT   UP   DOWN   OK
    OFF       power pressed twice (many projectors ask for a second press to switch off)
    STARTUP   power, wait 35 s, LEFT, wait 1 s, OK (the buttons and other commands keep working while it waits)
    SHUTDOWN  power, wait 1 s, OK
    CANCEL    stops a STARTUP or SHUTDOWN that is still running
    CMD 15    send any NEC command number (hex), to try codes that have not been captured
  Replies "SENT <command>" or "UNKNOWN <text>".

  The codes were captured with IR_CODE_LEARNER on 27 November 2026: NEC, address 0x00.
*/

#include <IRremote.hpp>

#define IR_SEND_PIN_NUMBER 3
#define PROJECTOR_ADDRESS 0x00
#define POWER_CONFIRM_DELAY_MS 1000  // gap between the two presses of OFF
#define DEBOUNCE_MS 30               // a push button must hold its new state this long before it counts
#define STARTUP_WARMUP_MS 35000UL    // STARTUP: wait after POWER before LEFT
#define STARTUP_OK_DELAY_MS 1000UL   // STARTUP: wait after LEFT before OK
#define SHUTDOWN_OK_DELAY_MS 1000UL  // SHUTDOWN: wait after POWER before OK

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

const Button buttons[] = {
  { "POWER", 0x14 },
  { "LEFT", 0x0E },
  { "RIGHT", 0x1A },
  { "UP", 0x03 },
  { "DOWN", 0x02 },
  { "OK", 0x07 },
};
const uint8_t buttonCount = sizeof(buttons) / sizeof(buttons[0]);

struct PushButton {
  uint8_t pin;
  const char *name;      // the entry of buttons[] it sends
  bool pressed;          // debounced state
  bool lastReading;      // raw state at the last look
  unsigned long changedAt;
};

PushButton pushButtons[] = {
  { A0, "POWER", false, false, 0 },
  { A1, "UP", false, false, 0 },
  { A2, "DOWN", false, false, 0 },
  { A3, "LEFT", false, false, 0 },
  { 6, "RIGHT", false, false, 0 },
  { 5, "OK", false, false, 0 },
  { A5, "STARTUP", false, false, 0 },
  { A4, "SHUTDOWN", false, false, 0 },
};
const uint8_t pushButtonCount = sizeof(pushButtons) / sizeof(pushButtons[0]);

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

// The STARTUP and SHUTDOWN sequences run from loop() without blocking: each step is sent when its wait since the step before has passed.
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
const Sequence sequences[] = {
  { "STARTUP", startupSteps, sizeof(startupSteps) / sizeof(startupSteps[0]) },
  { "SHUTDOWN", shutdownSteps, sizeof(shutdownSteps) / sizeof(shutdownSteps[0]) },
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

// Starts a sequence from a push button or a serial command. Refused while one is running: an extra POWER would upset the projector.
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
  if (strcmp(line, "OFF") == 0) {
    sendButton("POWER");
    delay(POWER_CONFIRM_DELAY_MS);
    sendButton("POWER");
  } else if (findSequence(line) != nullptr) {
    if (!startSequence(findSequence(line))) return;
  } else if (strcmp(line, "CANCEL") == 0) {
    running = nullptr;
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

// Non-blocking debounce: a reading counts once it has been steady for DEBOUNCE_MS. The code is sent when a button goes down.
void scanPushButtons() {
  for (uint8_t i = 0; i < pushButtonCount; i++) {
    PushButton &b = pushButtons[i];
    bool reading = digitalRead(b.pin) == LOW;  // a pressed button connects the pin to ground
    if (reading != b.lastReading) {
      b.lastReading = reading;
      b.changedAt = millis();
    } else if (reading != b.pressed && millis() - b.changedAt >= DEBOUNCE_MS) {
      b.pressed = reading;
      if (reading) {
        const Sequence *sequence = findSequence(b.name);
        if (sequence != nullptr ? startSequence(sequence) : sendButton(b.name)) {
          Serial.print(F("SENT "));
          Serial.println(b.name);
        }
      }
    }
  }
}

void setup() {
  Serial.begin(115200);
  IrSender.begin(IR_SEND_PIN_NUMBER);
  for (uint8_t i = 0; i < pushButtonCount; i++) {
    pinMode(pushButtons[i].pin, INPUT_PULLUP);
  }
  delay(5);  // let the pull-ups settle
  for (uint8_t i = 0; i < pushButtonCount; i++) {
    // a button already held at start-up is taken as the starting state, so it does not send
    pushButtons[i].pressed = pushButtons[i].lastReading = digitalRead(pushButtons[i].pin) == LOW;
  }
  Serial.println(F("IR projector remote ready on D3: POWER OFF STARTUP SHUTDOWN CANCEL LEFT RIGHT UP DOWN OK, or CMD <hex>"));
}

void loop() {
  scanPushButtons();
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
