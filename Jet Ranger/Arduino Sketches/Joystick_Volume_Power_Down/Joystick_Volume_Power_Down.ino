int Ethernet_In_Use = 1;
int Reflector_In_Use = 1;

// ###################################### Begin Ethernet Related #############################
#include <SPI.h>
#include <Ethernet.h>
#include <EthernetUdp.h>

#define EthernetStartupDelay 500
#define ES1_RESET_PIN 11

// These local Mac and IP Address will be reassigned early in startup based on
// the device ID as set by address pins
byte mac[] = { 0xA8, 0x61, 0x0A, 0x67, 0x83, 0x6D };
String sMac = "A8:61:0A:67:83:6D";
IPAddress ip(172, 16, 1, 109);
String strMyIP = "172.16.1.109";

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

const unsigned long delayBeforeSendingPacket = 2000;
unsigned long ethernetStartTime = 0;

int packetSize;
int debugLen;
EthernetUDP udp;
EthernetUDP debugUDP;
char packetBuffer[1000];     //buffer to store the incoming data
char outpacketBuffer[1000];  //buffer to store the outgoing data
String DebugString = "";
String BoardName = "Leonardo Test ";


void SendDebug(String MessageToSend) {
  if ((Reflector_In_Use == 1) && (Ethernet_In_Use == 1)) {
    udp.beginPacket(reflectorIP, reflectorport);
    udp.print(MessageToSend);
    udp.endPacket();
  }
}


// ###################################### End Ethernet Related #############################


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

#define RED_STATUS_LED_PORT 13
#define FLASH_TIME 300

// #include <IRremote.h>

// int RECV_PIN = 8;
// IRrecv irrecv(RECV_PIN);
// decode_results results;

void setup() {
  pinMode(powerButtonPin, INPUT_PULLUP);
  pinMode(wakeButtonPin, INPUT_PULLUP);
  pinMode(ledPin, OUTPUT);


  pinMode(RED_STATUS_LED_PORT, OUTPUT);

  digitalWrite(RED_STATUS_LED_PORT, true);
  delay(FLASH_TIME);


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
      digitalWrite(RED_STATUS_LED_PORT, false);
      delay(FLASH_TIME);
      digitalWrite(RED_STATUS_LED_PORT, true);
    }


    SendDebug(BoardName + " Ethernet Started " + strMyIP + " " + sMac);
  }




  // Joystick buttons as inputs
  for (int i = 0; i < numJoyButtons; i++) {
    pinMode(joyButtonPins[i], INPUT_PULLUP);
  }

  // Initialize all HID interfaces
  System.begin();    // Power/Sleep/Wake
  Consumer.begin();  // Volume control
  Gamepad.begin();   // Joystick

  // irrecv.enableIRIn();

  SendDebug(BoardName + " Startup Complete ");
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

  // if (irrecv.decode(&results)) {
  //   Serial.print("IR RECV Code = 0x ");
  //   Serial.println(results.value, HEX);
  //   irrecv.resume();
  // }
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
