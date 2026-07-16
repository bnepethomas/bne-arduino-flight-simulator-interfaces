#include <Joystick.h>

// Create the Joystick object.
// Parameters: report ID, joystick type, button count, hat switch count,
// then which axes to include (all false here since we only want a button)
Joystick_ Joystick(
  JOYSTICK_DEFAULT_REPORT_ID,
  JOYSTICK_TYPE_JOYSTICK,
  1,     // button count
  0,     // hat switch count
  false, false, false,   // X, Y, Z axis
  false, false, false,   // Rx, Ry, Rz
  false,                 // rudder
  false,                 // throttle
  false,                 // accelerator
  false,                 // brake
  false                  // steering
);

const int buttonPin = 5;   // Physical pin wired to the switch
const int buttonIndex = 0; // Reports as Button 0 in Windows Game Controllers

int lastState = HIGH;

void setup() {
  // Using internal pull-up: button should connect pin 5 to GND when pressed
  pinMode(buttonPin, INPUT_PULLUP);

  Joystick.begin();
}

void loop() {
  int currentState = digitalRead(buttonPin);

  if (currentState != lastState) {
    // Active LOW: pressed = LOW (connected to GND)
    bool pressed = (currentState == LOW);
    Joystick.setButton(buttonIndex, pressed);
    lastState = currentState;
    delay(20); // simple debounce
  }
}