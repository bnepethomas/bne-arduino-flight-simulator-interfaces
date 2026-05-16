// SN_001_RIGHT_SIM_CONTROL_V4_FIXED
// Board: Arduino Micro
//
// IMPORTANT NEXTION NOTE:
// In the ITEAD Nextion library, edit NexConfig.h so it uses Serial1:
//
//   #define nexSerial Serial1
//
// Also comment out debug serial if required:
//
//   //#define DEBUG_SERIAL_ENABLE
//
// IMPORTANT:
// SW1 may show as "a" in the matrix test.
// In this final code, "a" is only the internal matrix label.
// SW1 sends ESC, not the letter a.


#include <Keypad.h>
#include <HID-Project.h>

#define REVERSED true



// ------------------------------------------------------------
// POTS
// ------------------------------------------------------------

int VolPot = A4;  // Volume
int DIMPot = A5;  // Nextion dimmer

int VolUp;
int VolDn;
int valVol = 0;
int previousvalVol = 0;
int valVol2 = 0;
int brightness = 100;

// ------------------------------------------------------------
// MATRIX
// ------------------------------------------------------------

const byte ROWS = 2;
const byte COLS = 8;

byte rowPins[ROWS] = { A0, A1 };
byte colPins[COLS] = { 4, 5, 6, 7, 8, 9, 10, 11 };

char keys[ROWS][COLS] = {
  { 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h' },
  { '1', '2', '3', '4', '5', '6', '7', '8' }
};

// This matches the matrix test that showed SW1 as "a".
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// If all physical buttons fail, try ONLY this line instead:
// Keypad keypad = Keypad(makeKeymap(keys), colPins, rowPins, COLS, ROWS);

// ------------------------------------------------------------
// GENERAL
// ------------------------------------------------------------

#define SafetyTimeOut 10000
#define WaitOneHour 3600000
long GlobalLastkeyPressed = WaitOneHour;


// ============================================================
// PHYSICAL KEYPAD MAPPING
// ============================================================

void pressMappedKey(char key) {
  switch (key) {

    case 'a':
      // SW1 / ESC
      // Internal key is "a", but output is ESC.
      Keyboard.press(KEY_ESC);
      delay(50);
      Keyboard.releaseAll();
      GlobalLastkeyPressed = millis();
      break;

    case 'b':
      // Active Pause: Left Shift + Left Windows/GUI + Pause
      Keyboard.press(KEY_LEFT_SHIFT);
      delay(10);
      Keyboard.press(KEY_LEFT_GUI);
      delay(10);
      Keyboard.press(KEY_PAUSE);
      delay(50);
      Keyboard.releaseAll();
      GlobalLastkeyPressed = millis();
      break;

    case 'c':
      // Salute: Left Ctrl + Left Shift + Left Alt + S
      Keyboard.press(KEY_LEFT_CTRL);
      delay(10);
      Keyboard.press(KEY_LEFT_SHIFT);
      delay(10);
      Keyboard.press(KEY_LEFT_ALT);
      delay(10);
      Keyboard.press('s');
      delay(50);
      Keyboard.releaseAll();
      GlobalLastkeyPressed = millis();
      break;

    case 'd':
      // Warp / custom shortcut
      Keyboard.press(KEY_LEFT_CTRL);
      delay(10);
      Keyboard.press(KEY_F4);
      delay(50);
      Keyboard.releaseAll();

      delay(20);

      Keyboard.press(KEY_F1);  // Return to cockpit
      delay(50);
      Keyboard.releaseAll();
      GlobalLastkeyPressed = millis();
      break;

    case 'e':
      // Virtual cockpit on/off
      Keyboard.press(KEY_LEFT_ALT);
      delay(10);
      Keyboard.press(KEY_F1);
      delay(50);
      Keyboard.releaseAll();
      GlobalLastkeyPressed = millis();
      break;

    case 'f':
      // ATC
      Keyboard.press('\\');
      delay(50);
      Keyboard.releaseAll();
      GlobalLastkeyPressed = millis();
      break;

    case 'g':
      // Labels: Left Shift + F10
      Keyboard.press(KEY_LEFT_SHIFT);
      delay(10);
      Keyboard.press(KEY_F10);
      delay(50);
      Keyboard.releaseAll();
      GlobalLastkeyPressed = millis();
      break;

    case 'h':
      // Spare / not used yet
      Keyboard.releaseAll();
      break;

    case '1':
      Keyboard.press(KEY_F1);
      delay(50);
      Keyboard.releaseAll();
      GlobalLastkeyPressed = millis();
      break;

    case '2':
      Keyboard.press(KEY_F10);
      delay(50);
      Keyboard.releaseAll();
      GlobalLastkeyPressed = millis();
      break;

    case '3':
      Keyboard.press(KEY_F1);
      delay(50);
      Keyboard.releaseAll();
      GlobalLastkeyPressed = millis();
      break;

    case '4':
      Keyboard.press(KEY_LEFT_CTRL);
      delay(10);
      Keyboard.press(KEY_F4);
      delay(50);
      Keyboard.releaseAll();
      GlobalLastkeyPressed = millis();
      break;

    case '5':
      Keyboard.press('f');
      delay(50);
      Keyboard.releaseAll();
      GlobalLastkeyPressed = millis();
      break;

    case '6':
      Keyboard.press(KEY_F3);
      delay(50);
      Keyboard.releaseAll();
      GlobalLastkeyPressed = millis();
      break;

    case '7':
      Keyboard.press(KEY_F6);
      delay(50);
      Keyboard.releaseAll();
      GlobalLastkeyPressed = millis();
      break;

    case '8':
      Keyboard.press(KEY_LEFT_CTRL);
      delay(10);
      Keyboard.press(KEY_F5);
      delay(50);
      Keyboard.releaseAll();
      GlobalLastkeyPressed = millis();
      break;

    default:
      Keyboard.releaseAll();
      break;
  }
}

void setup() {
  Keyboard.begin();
  Consumer.begin();

  delay(250);

  // Initialise volume position
  for (int a = 0; a < 52; a++) {
    Consumer.write(MEDIA_VOLUME_UP);
    delay(2);
  }

  Keyboard.releaseAll();

}
// ============================================================
// LOOP
// ============================================================

void loop() {
  // Physical keypad
  char key = keypad.getKey();

  if (key) {
    pressMappedKey(key);
  }

  // Volume pot
  valVol = analogRead(VolPot);
  valVol = map(valVol, 0, 1023, 0, 101);

  if (REVERSED) {
    valVol = 101 - valVol;
  }

  if (abs(valVol - previousvalVol) > 1) {
    previousvalVol = valVol;
    valVol /= 2;

    while (valVol2 < valVol) {
      Consumer.write(MEDIA_VOLUME_UP);
      valVol2++;
      delay(2);
    }

    while (valVol2 > valVol) {
      Consumer.write(MEDIA_VOLUME_DOWN);
      valVol2--;
      delay(2);
    }
  }
}


/* py code
# SN_001_RIGHT_SIM_CONTROL_V4_FIXED
# Reference only. Not used by Arduino.

keys = [
    ['a', 'b', 'c', 'd', 'e', 'f', 'g', 'h'],
    ['1', '2', '3', '4', '5', '6', '7', '8']
]

print("Matrix internal labels:")
for r, row in enumerate(keys):
    for c, key in enumerate(row):
        print(f"Row {r}, Col {c} = {key}")

print()
print("SW1 reports internal label 'a', but final Arduino output is ESC.")
*/