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

#define FLASH_TIME 1000
unsigned long NEXT_STATUS_TOGGLE_TIMER = 0;
bool GREEN_LED_STATE = true;

bool SerialDebug = false;

// ********************************* Begin Ethernet ***************************************************
// Ethernet Related

int Ethernet_In_Use = 0;  // Check to see if jumper is present - if it is disable Ethernet calls. Used for Testing
int Reflector_In_Use = 0;


#include <SPI.h>
#include <Ethernet.h>
#include <EthernetUdp.h>

#define EthernetStartupDelay 500
#define ES1_RESET_PIN 12

const unsigned long delayBeforeSendingPacket = 3000;
unsigned long ethernetStartTime = 0;
String BoardName = "Hornet Sim Controller: ";
bool firstLoop = true;

// These local Mac and IP Address will be reassigned early in startup based on
// the device ID as set by address pins
byte mac[] = { 0xA8, 0x61, 0x0A, 0x9E, 0x83, 0x03 };
String sMac = "A8:61:0A:67:83:03";
IPAddress ip(172, 16, 1, 123);
String strMyIP = "172.16.1.123";

// Reflector
IPAddress reflectorIP(172, 16, 1, 10);
String strReflectorIP = "X.X.X.X";

// Arduino Due for Keystroke translation and Pixel Led driving
IPAddress targetIP(172, 16, 1, 110);
String strTargetIP = "X.X.X.X";

// Computer Running MSFS
IPAddress MSFSIP(172, 16, 1, 10);
String strMSFSIP = "X.X.X.X";

const unsigned int localport = 7788;
const unsigned int localdebugport = 7795;
const unsigned int keyboardport = 7788;
const unsigned int ledport = 7789;
const unsigned int remoteport = 7790;
const unsigned int reflectorport = 27000;
const unsigned int MSFSport = 7791;
const unsigned int aliveport = 13137;

int packetSize;
int debugLen;
EthernetUDP udp;
EthernetUDP debugUDP;
char packetBuffer[1000];     //buffer to store the incoming data
char outpacketBuffer[1000];  //buffer to store the outgoing data

EthernetUDP aliveudp;  // Sends keepalives to monitoring application
const unsigned long aliveinterval = 10000;
long lastalivesent = 0;


void SendDebug(String MessageToSend) {


  MessageToSend = BoardName + ": " + MessageToSend;

  if (SerialDebug) {
    Serial.println(MessageToSend);
  }

  if ((Reflector_In_Use == 1) && (Ethernet_In_Use == 1)) {
    udp.beginPacket(reflectorIP, reflectorport);
    udp.print(MessageToSend);
    udp.endPacket();
  }
}

// ********************************* End Ethernet ***************************************************


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
String msg;

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
      // Warp / custom shortcut
      Keyboard.press(KEY_LEFT_CTRL);
      delay(10);
      Keyboard.press(KEY_LEFT_ALT);
      delay(10);
      Keyboard.press(KEY_F7);
      delay(50);
      Keyboard.releaseAll();

      delay(20);
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

    case 'e':
      // ATC
      Keyboard.press('\\');
      delay(50);
      Keyboard.releaseAll();
      GlobalLastkeyPressed = millis();

      break;

    case 'f':
      // Virtual cockpit on/off
      Keyboard.press(KEY_LEFT_ALT);
      delay(10);
      Keyboard.press(KEY_F1);
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
      SendDebug("Pressing Key F1");
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
      delay(50);
      Keyboard.press(KEY_F4);
      delay(50);
      Keyboard.releaseAll();
      GlobalLastkeyPressed = millis();
      break;

    case '5':
      Keyboard.press('KEY_F2');
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


  if (Ethernet_In_Use == 1) {

    // Using manual reset instead of tying to Arduino Reset
    pinMode(ES1_RESET_PIN, OUTPUT);
    digitalWrite(ES1_RESET_PIN, LOW);
    delay(2);
    digitalWrite(ES1_RESET_PIN, HIGH);

    Ethernet.begin(mac, ip);
    udp.begin(localport);

    ethernetStartTime = millis() + delayBeforeSendingPacket;
    while (millis() <= ethernetStartTime) {
      delay(FLASH_TIME);
      digitalWrite(LED_BUILTIN, false);
      delay(FLASH_TIME);
      digitalWrite(LED_BUILTIN, true);
    }


    SendDebug("Ethernet Started " + strMyIP + " " + sMac);

    aliveudp.begin(aliveport);
  }

  if (SerialDebug) {

    Serial.begin(9600);
    while (!Serial) {
      ;  // wait for serial port to connect. Needed for native USB port only
      if (millis() >= NEXT_STATUS_TOGGLE_TIMER) {

        GREEN_LED_STATE = !GREEN_LED_STATE;


        digitalWrite(LED_BUILTIN, GREEN_LED_STATE);
        NEXT_STATUS_TOGGLE_TIMER = millis() + FLASH_TIME;
      }
    }
  }

  // prints title with ending line break
  Serial.println(BoardName);

  Keyboard.begin();
  Consumer.begin();

  delay(250);

  // Initialise volume position
  for (int a = 0; a < 52; a++) {
    Consumer.write(MEDIA_VOLUME_UP);
    delay(2);
  }

  Keyboard.releaseAll();
  msg = "";
}
// ============================================================
// LOOP
// ============================================================

void loop() {

  if (millis() >= NEXT_STATUS_TOGGLE_TIMER) {

    GREEN_LED_STATE = !GREEN_LED_STATE;
    digitalWrite(LED_BUILTIN, GREEN_LED_STATE);
    NEXT_STATUS_TOGGLE_TIMER = millis() + FLASH_TIME;
  }

  if (keypad.getKeys()) {
    for (int i = 0; i < LIST_MAX; i++)  // Scan the whole key list.
    {
      if (keypad.key[i].stateChanged)  // Only find keys that have changed state.
      {
        switch (keypad.key[i].kstate) {  // Report active key state : IDLE, PRESSED, HOLD, or RELEASED
          case PRESSED:
            msg = " PRESSED.";
            // Don't send any keystrokes first time around
            if (!firstLoop) {
              pressMappedKey(keypad.key[i].kchar);
            }
            break;
          case HOLD:
            msg = " HOLD.";
            break;
          case RELEASED:
            msg = " RELEASED.";
            break;
          case IDLE:
            msg = " IDLE.";
        }
        SendDebug("keypad ");
        SendDebug(String(keypad.key[i].kchar));
        SendDebug(msg);
      }
    }
  }

  firstLoop = false;

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