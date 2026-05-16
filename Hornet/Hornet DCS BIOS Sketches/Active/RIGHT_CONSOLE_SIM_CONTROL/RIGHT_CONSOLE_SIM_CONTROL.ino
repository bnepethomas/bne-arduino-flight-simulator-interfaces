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

#include <Nextion.h>
#include <Keypad.h>
#include <HID-Project.h>

#include "NexButton.h"
#include "NexText.h"

#define REVERSED true
#define NEXTION Serial1

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


// ============================================================
// NEXTION BUTTON OBJECTS
// ============================================================

// Page 1 - Menu
NexButton m_SPC = NexButton(1, 6, "b0");  // SPACE

// Page 2 - Keypad map
NexButton k_F1 = NexButton(2, 2, "b1");
NexButton k_F2 = NexButton(2, 3, "b2");
NexButton k_F3 = NexButton(2, 4, "b3");
NexButton k_F4 = NexButton(2, 5, "b4");
NexButton k_F5 = NexButton(2, 6, "b5");
NexButton k_F6 = NexButton(2, 7, "b6");
NexButton k_F7 = NexButton(2, 8, "b7");
NexButton k_F8 = NexButton(2, 9, "b8");
NexButton k_F9 = NexButton(2, 10, "b9");
NexButton k_F10 = NexButton(2, 11, "b10");
NexButton k_F11 = NexButton(2, 12, "b11");
NexButton k_F12 = NexButton(2, 13, "b12");
NexButton k_SPC = NexButton(2, 14, "b13");  // SPACE

// Page 3 - Computer control
NexButton c_TAB = NexButton(3, 2, "b7");
NexButton c_ENT = NexButton(3, 3, "b5");
NexButton c_AE = NexButton(3, 4, "b6");
NexButton c_WIN = NexButton(3, 5, "b8");

// Page 4 - Views
NexButton v_IN = NexButton(4, 6, "b0");
NexButton v_OUT = NexButton(4, 8, "b4");
NexButton v_RS = NexButton(4, 7, "b2");

NexButton v_UP = NexButton(4, 1, "b9");
NexButton v_LF = NexButton(4, 5, "b13");
NexButton v_RT = NexButton(4, 4, "b12");
NexButton v_CN = NexButton(4, 2, "b10");
NexButton v_DN = NexButton(4, 3, "b11");

NexButton v_TL = NexButton(4, 9, "b5");
NexButton v_TR = NexButton(4, 10, "b6");
NexButton v_BL = NexButton(4, 11, "b7");
NexButton v_BR = NexButton(4, 12, "b8");

// Page 6 - PC shutdown
NexButton p_PCOFF = NexButton(6, 1, "b0");


// ============================================================
// NEXTION LISTEN LIST
// ============================================================

NexTouch *nex_listen_list[] = {
  &m_SPC,

  &k_F1,
  &k_F2,
  &k_F3,
  &k_F4,
  &k_F5,
  &k_F6,
  &k_F7,
  &k_F8,
  &k_F9,
  &k_F10,
  &k_F11,
  &k_F12,
  &k_SPC,

  &c_TAB,
  &c_ENT,
  &c_AE,
  &c_WIN,

  &v_IN,
  &v_OUT,
  &v_UP,
  &v_LF,
  &v_RT,
  &v_CN,
  &v_DN,
  &v_RS,
  &v_TL,
  &v_TR,
  &v_BL,
  &v_BR,

  &p_PCOFF,

  NULL
};


// ============================================================
// NEXTION CALLBACKS
// ============================================================

// ------------------------------------------------------------
// Space buttons
// ------------------------------------------------------------

void m_SPCPressCallback(void *ptr) {
  Keyboard.press(' ');
  delay(50);
  Keyboard.releaseAll();
  GlobalLastkeyPressed = millis();
}

void m_SPCReleaseCallback(void *ptr) {
  Keyboard.releaseAll();
}

void k_SPCPressCallback(void *ptr) {
  Keyboard.press(' ');
  delay(50);
  Keyboard.releaseAll();
  GlobalLastkeyPressed = millis();
}

void k_SPCReleaseCallback(void *ptr) {
  Keyboard.releaseAll();
}


// ------------------------------------------------------------
// F keys
// ------------------------------------------------------------

void k_F1PressCallback(void *ptr) {
  Keyboard.press(KEY_F1);
  GlobalLastkeyPressed = millis();
}

void k_F1ReleaseCallback(void *ptr) {
  Keyboard.releaseAll();
}

void k_F2PressCallback(void *ptr) {
  Keyboard.press(KEY_F2);
  GlobalLastkeyPressed = millis();
}

void k_F2ReleaseCallback(void *ptr) {
  Keyboard.releaseAll();
}

void k_F3PressCallback(void *ptr) {
  Keyboard.press(KEY_F3);
  GlobalLastkeyPressed = millis();
}

void k_F3ReleaseCallback(void *ptr) {
  Keyboard.releaseAll();
}

void k_F4PressCallback(void *ptr) {
  Keyboard.press(KEY_F4);
  GlobalLastkeyPressed = millis();
}

void k_F4ReleaseCallback(void *ptr) {
  Keyboard.releaseAll();
}

void k_F5PressCallback(void *ptr) {
  Keyboard.press(KEY_F5);
  GlobalLastkeyPressed = millis();
}

void k_F5ReleaseCallback(void *ptr) {
  Keyboard.releaseAll();
}

void k_F6PressCallback(void *ptr) {
  Keyboard.press(KEY_F6);
  GlobalLastkeyPressed = millis();
}

void k_F6ReleaseCallback(void *ptr) {
  Keyboard.releaseAll();
}

void k_F7PressCallback(void *ptr) {
  Keyboard.press(KEY_F7);
  GlobalLastkeyPressed = millis();
}

void k_F7ReleaseCallback(void *ptr) {
  Keyboard.releaseAll();
}

void k_F8PressCallback(void *ptr) {
  Keyboard.press(KEY_F8);
  GlobalLastkeyPressed = millis();
}

void k_F8ReleaseCallback(void *ptr) {
  Keyboard.releaseAll();
}

void k_F9PressCallback(void *ptr) {
  Keyboard.press(KEY_F9);
  GlobalLastkeyPressed = millis();
}

void k_F9ReleaseCallback(void *ptr) {
  Keyboard.releaseAll();
}

void k_F10PressCallback(void *ptr) {
  Keyboard.press(KEY_F10);
  GlobalLastkeyPressed = millis();
}

void k_F10ReleaseCallback(void *ptr) {
  Keyboard.releaseAll();
}

void k_F11PressCallback(void *ptr) {
  Keyboard.press(KEY_F11);
  GlobalLastkeyPressed = millis();
}

void k_F11ReleaseCallback(void *ptr) {
  Keyboard.releaseAll();
}

void k_F12PressCallback(void *ptr) {
  Keyboard.press(KEY_F12);
  GlobalLastkeyPressed = millis();
}

void k_F12ReleaseCallback(void *ptr) {
  Keyboard.releaseAll();
}


// ------------------------------------------------------------
// Computer control
// ------------------------------------------------------------

void c_TABPressCallback(void *ptr) {
  Keyboard.press(KEY_TAB);
  GlobalLastkeyPressed = millis();
}

void c_TABReleaseCallback(void *ptr) {
  Keyboard.releaseAll();
}

void c_ENTPressCallback(void *ptr) {
  Keyboard.press(KEY_RETURN);
  GlobalLastkeyPressed = millis();
}

void c_ENTReleaseCallback(void *ptr) {
  Keyboard.releaseAll();
}

void c_AEPressCallback(void *ptr) {
  Keyboard.press(KEY_RIGHT_ALT);
  GlobalLastkeyPressed = millis();
}

void c_AEReleaseCallback(void *ptr) {
  Keyboard.releaseAll();
}

void c_WINPressCallback(void *ptr) {
  Keyboard.press(KEY_LEFT_GUI);
  GlobalLastkeyPressed = millis();
}

void c_WINReleaseCallback(void *ptr) {
  Keyboard.releaseAll();
}


// ------------------------------------------------------------
// View controls
// ------------------------------------------------------------

void v_INPressCallback(void *ptr) {
  Keyboard.press(KEYPAD_MULTIPLY);
  GlobalLastkeyPressed = millis();
}

void v_INReleaseCallback(void *ptr) {
  Keyboard.releaseAll();
}

void v_OUTPressCallback(void *ptr) {
  Keyboard.press(KEYPAD_DIVIDE);
  GlobalLastkeyPressed = millis();
}

void v_OUTReleaseCallback(void *ptr) {
  Keyboard.releaseAll();
}

void v_UPPressCallback(void *ptr) {
  Keyboard.press(KEYPAD_8);
  GlobalLastkeyPressed = millis();
}

void v_UPReleaseCallback(void *ptr) {
  Keyboard.releaseAll();
}

void v_LFPressCallback(void *ptr) {
  Keyboard.press(KEYPAD_4);
  GlobalLastkeyPressed = millis();
}

void v_LFReleaseCallback(void *ptr) {
  Keyboard.releaseAll();
}

void v_RTPressCallback(void *ptr) {
  Keyboard.press(KEYPAD_6);
  GlobalLastkeyPressed = millis();
}

void v_RTReleaseCallback(void *ptr) {
  Keyboard.releaseAll();
}

void v_CNPressCallback(void *ptr) {
  Keyboard.press(KEYPAD_5);
  GlobalLastkeyPressed = millis();
}

void v_CNReleaseCallback(void *ptr) {
  Keyboard.releaseAll();
}

void v_DNPressCallback(void *ptr) {
  Keyboard.press(KEYPAD_2);
  GlobalLastkeyPressed = millis();
}

void v_DNReleaseCallback(void *ptr) {
  Keyboard.releaseAll();
}

void v_RSPressCallback(void *ptr) {
  Keyboard.press(KEYPAD_5);
  GlobalLastkeyPressed = millis();
}

void v_RSReleaseCallback(void *ptr) {
  Keyboard.releaseAll();
}

void v_TLPressCallback(void *ptr) {
  Keyboard.press(KEYPAD_7);
  GlobalLastkeyPressed = millis();
}

void v_TLReleaseCallback(void *ptr) {
  Keyboard.releaseAll();
}

void v_TRPressCallback(void *ptr) {
  Keyboard.press(KEYPAD_9);
  GlobalLastkeyPressed = millis();
}

void v_TRReleaseCallback(void *ptr) {
  Keyboard.releaseAll();
}

void v_BLPressCallback(void *ptr) {
  Keyboard.press(KEYPAD_1);
  GlobalLastkeyPressed = millis();
}

void v_BLReleaseCallback(void *ptr) {
  Keyboard.releaseAll();
}

void v_BRPressCallback(void *ptr) {
  Keyboard.press(KEYPAD_3);
  GlobalLastkeyPressed = millis();
}

void v_BRReleaseCallback(void *ptr) {
  Keyboard.releaseAll();
}


// ------------------------------------------------------------
// WARNING - PC SHUTDOWN CODE
// ------------------------------------------------------------

void p_PCOFFPressCallback(void *ptr) {
  // WARNING - THIS SENDS WINDOWS SHUTDOWN KEYS

  Keyboard.press(KEY_LEFT_GUI);
  delay(10);
  Keyboard.press(KEY_X);
  delay(10);
  Keyboard.releaseAll();

  delay(20);

  Keyboard.press(KEY_U);
  delay(10);
  Keyboard.releaseAll();

  delay(10);

  Keyboard.press(KEY_U);
  delay(10);
  Keyboard.releaseAll();

  GlobalLastkeyPressed = millis();
}

void p_PCOFFReleaseCallback(void *ptr) {
  Keyboard.releaseAll();
}


// ============================================================
// SETUP
// ============================================================

void setup() {
  Keyboard.begin();
  Consumer.begin();

  delay(250);

  // Initialise volume position
  for (int a = 0; a < 52; a++) {
    Consumer.write(MEDIA_VOLUME_UP);
    delay(2);
  }

  NEXTION.begin(9600);
  nexInit();

  // Space buttons
  m_SPC.attachPush(m_SPCPressCallback, &m_SPC);
  m_SPC.attachPop(m_SPCReleaseCallback, &m_SPC);

  k_SPC.attachPush(k_SPCPressCallback, &k_SPC);
  k_SPC.attachPop(k_SPCReleaseCallback, &k_SPC);

  // F keys
  k_F1.attachPush(k_F1PressCallback, &k_F1);
  k_F1.attachPop(k_F1ReleaseCallback, &k_F1);

  k_F2.attachPush(k_F2PressCallback, &k_F2);
  k_F2.attachPop(k_F2ReleaseCallback, &k_F2);

  k_F3.attachPush(k_F3PressCallback, &k_F3);
  k_F3.attachPop(k_F3ReleaseCallback, &k_F3);

  k_F4.attachPush(k_F4PressCallback, &k_F4);
  k_F4.attachPop(k_F4ReleaseCallback, &k_F4);

  k_F5.attachPush(k_F5PressCallback, &k_F5);
  k_F5.attachPop(k_F5ReleaseCallback, &k_F5);

  k_F6.attachPush(k_F6PressCallback, &k_F6);
  k_F6.attachPop(k_F6ReleaseCallback, &k_F6);

  k_F7.attachPush(k_F7PressCallback, &k_F7);
  k_F7.attachPop(k_F7ReleaseCallback, &k_F7);

  k_F8.attachPush(k_F8PressCallback, &k_F8);
  k_F8.attachPop(k_F8ReleaseCallback, &k_F8);

  k_F9.attachPush(k_F9PressCallback, &k_F9);
  k_F9.attachPop(k_F9ReleaseCallback, &k_F9);

  k_F10.attachPush(k_F10PressCallback, &k_F10);
  k_F10.attachPop(k_F10ReleaseCallback, &k_F10);

  k_F11.attachPush(k_F11PressCallback, &k_F11);
  k_F11.attachPop(k_F11ReleaseCallback, &k_F11);

  k_F12.attachPush(k_F12PressCallback, &k_F12);
  k_F12.attachPop(k_F12ReleaseCallback, &k_F12);

  // Computer control
  c_TAB.attachPush(c_TABPressCallback, &c_TAB);
  c_TAB.attachPop(c_TABReleaseCallback, &c_TAB);

  c_ENT.attachPush(c_ENTPressCallback, &c_ENT);
  c_ENT.attachPop(c_ENTReleaseCallback, &c_ENT);

  c_AE.attachPush(c_AEPressCallback, &c_AE);
  c_AE.attachPop(c_AEReleaseCallback, &c_AE);

  c_WIN.attachPush(c_WINPressCallback, &c_WIN);
  c_WIN.attachPop(c_WINReleaseCallback, &c_WIN);

  // Views
  v_IN.attachPush(v_INPressCallback, &v_IN);
  v_IN.attachPop(v_INReleaseCallback, &v_IN);

  v_OUT.attachPush(v_OUTPressCallback, &v_OUT);
  v_OUT.attachPop(v_OUTReleaseCallback, &v_OUT);

  v_UP.attachPush(v_UPPressCallback, &v_UP);
  v_UP.attachPop(v_UPReleaseCallback, &v_UP);

  v_LF.attachPush(v_LFPressCallback, &v_LF);
  v_LF.attachPop(v_LFReleaseCallback, &v_LF);

  v_RT.attachPush(v_RTPressCallback, &v_RT);
  v_RT.attachPop(v_RTReleaseCallback, &v_RT);

  v_CN.attachPush(v_CNPressCallback, &v_CN);
  v_CN.attachPop(v_CNReleaseCallback, &v_CN);

  v_DN.attachPush(v_DNPressCallback, &v_DN);
  v_DN.attachPop(v_DNReleaseCallback, &v_DN);

  v_RS.attachPush(v_RSPressCallback, &v_RS);
  v_RS.attachPop(v_RSReleaseCallback, &v_RS);

  v_TL.attachPush(v_TLPressCallback, &v_TL);
  v_TL.attachPop(v_TLReleaseCallback, &v_TL);

  v_TR.attachPush(v_TRPressCallback, &v_TR);
  v_TR.attachPop(v_TRReleaseCallback, &v_TR);

  v_BL.attachPush(v_BLPressCallback, &v_BL);
  v_BL.attachPop(v_BLReleaseCallback, &v_BL);

  v_BR.attachPush(v_BRPressCallback, &v_BR);
  v_BR.attachPop(v_BRReleaseCallback, &v_BR);

  // PC shutdown button
  p_PCOFF.attachPush(p_PCOFFPressCallback, &p_PCOFF);
  p_PCOFF.attachPop(p_PCOFFReleaseCallback, &p_PCOFF);

  Keyboard.releaseAll();

  // Set Nextion brightness
  Serial1.print("dim=10");
  Serial1.write("\xFF\xFF\xFF");
  delay(500);
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

  // Nextion dimmer pot
  {
    int brightnessRead = analogRead(DIMPot);
    int bright = map(brightnessRead, 0, 1023, 100, 10);

    String dim = "dim=" + String(bright);
    brightness = bright;

    Serial1.print(dim.c_str());
    Serial1.write("\xFF\xFF\xFF");
  }

  // Nextion touch handler
  nexLoop(nex_listen_list);
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