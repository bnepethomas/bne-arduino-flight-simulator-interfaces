#include <HID-Project.h>
#include <HID-Settings.h>

// === Pin Definitions ===
const int powerButtonPin = 2;  // Power Down button
const int wakeButtonPin = 4;   // Wake Up button
const int ledPin = 13;         // Status LED
const int volumePin = A0;      // Analog input for volume control
const int joyXPin = A1;        // Joystick X-axis
const int joyYPin = A2;        // Joystick Y-axis

const int joyButtonPins[] = { 8, 9, 10, 11 };  // Joystick buttons (D8–D11)
const int numJoyButtons = sizeof(joyButtonPins) / sizeof(joyButtonPins[0]);

// === State Tracking ===
bool lastPowerState = HIGH;
bool lastWakeState = HIGH;
bool lastJoyButtonState[4] = { HIGH, HIGH, HIGH, HIGH };

unsigned long debounceDelay = 50;

// LED blinking
unsigned long previousMillis = 0;
const long ledInterval = 500;
bool ledState = LOW;

// Volume tracking
int lastVolumeLevel = -1;
const int volumeStep = 5;  // Minimum change before volume update (0–100 scale)

// Joystick tracking
int lastX = -1;
int lastY = -1;
const int joyDeadzone = 4;  // Minimum change before joystick update

void setup() {
  pinMode(powerButtonPin, INPUT_PULLUP);
  pinMode(wakeButtonPin, INPUT_PULLUP);
  pinMode(ledPin, OUTPUT);

  // Joystick buttons as inputs
  for (int i = 0; i < numJoyButtons; i++) {
    pinMode(joyButtonPins[i], INPUT_PULLUP);
  }

  // Initialize all HID interfaces
  System.begin();    // Power/Sleep/Wake
  Consumer.begin();  // Volume control
  Gamepad.begin();   // Joystick
}

void loop() {
  unsigned long currentMillis = millis();

  // === LED Flashing Status ===
  if (currentMillis - previousMillis >= ledInterval) {
    previousMillis = currentMillis;
    ledState = !ledState;
    digitalWrite(ledPin, ledState);
  }

  // === Power Down Button ===
  bool powerState = digitalRead(powerButtonPin);
  if (powerState == LOW && lastPowerState == HIGH) {
    delay(debounceDelay);
    if (digitalRead(powerButtonPin) == LOW) {
      System.write(SYSTEM_POWER_DOWN);
      blinkLED(3);
    }
  }

  // === Wake Up Button ===
  bool wakeState = digitalRead(wakeButtonPin);
  if (wakeState == LOW && lastWakeState == HIGH) {
    delay(debounceDelay);
    if (digitalRead(wakeButtonPin) == LOW) {
      System.write(SYSTEM_WAKE_UP);
      blinkLED(1);
    }
  }

  lastPowerState = powerState;
  lastWakeState = wakeState;

  // === Volume Control (A0) ===
  int rawValue = analogRead(volumePin);
  int newLevel = map(rawValue, 0, 1023, 0, 100);

  if (abs(newLevel - lastVolumeLevel) >= volumeStep) {
    setVolumeLevel(newLevel);
    lastVolumeLevel = newLevel;
  }

  // === Joystick Axes (A1, A2) ===
  int xRaw = analogRead(joyXPin);
  int yRaw = analogRead(joyYPin);
  int xMapped = map(xRaw, 0, 1023, -127, 127);
  int yMapped = map(yRaw, 0, 1023, -127, 127);

  if (abs(xMapped - lastX) > joyDeadzone || abs(yMapped - lastY) > joyDeadzone) {
    Gamepad.xAxis(xMapped);
    Gamepad.yAxis(yMapped);
    Gamepad.write();
    lastX = xMapped;
    lastY = yMapped;
  }

  // === Joystick Buttons (D8–D11) ===
  for (int i = 0; i < numJoyButtons; i++) {
    bool currentState = digitalRead(joyButtonPins[i]);

    if (currentState != lastJoyButtonState[i]) {
      delay(debounceDelay);
      currentState = digitalRead(joyButtonPins[i]);

      if (currentState == LOW) {
        Gamepad.press(i + 1);  // Press button number (1–4)
      } else {
        Gamepad.release(i + 1);
      }
      Gamepad.write();
      lastJoyButtonState[i] = currentState;
    }
  }
}

// === Helper: Blink LED n times ===
void blinkLED(int times) {
  for (int i = 0; i < times; i++) {
    digitalWrite(ledPin, HIGH);
    delay(150);
    digitalWrite(ledPin, LOW);
    delay(150);
  }
}

// === Helper: Set PC Volume ===
void setVolumeLevel(int level) {
  static int currentLevel = 50;
  int diff = level - currentLevel;

  if (diff > 0) {
    for (int i = 0; i < diff; i++) Consumer.write(MEDIA_VOLUME_UP);
  } else if (diff < 0) {
    for (int i = 0; i < -diff; i++) Consumer.write(MEDIA_VOLUME_DOWN);
  }

  currentLevel = level;
}
