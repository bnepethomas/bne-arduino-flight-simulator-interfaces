#include <SPI.h>
#include <Ethernet.h>
#include <EthernetUdp.h>
#include <Joystick.h>
#include <Keyboard.h>

// --- Four virtual joysticks, 32 buttons each, no axes ---
// Windows/DCS-BIOS see these as four separate USB game controllers, each
// with its own report ID, giving 128 addressable buttons in total.
Joystick_ Joystick(
  JOYSTICK_DEFAULT_REPORT_ID,
  JOYSTICK_TYPE_JOYSTICK,
  32,    // button count
  0,     // hat switches
  false, false, false,   // X, Y, Z
  false, false, false,   // Rx, Ry, Rz
  false,                  // rudder
  false,                  // throttle
  false,                  // accelerator
  false,                  // brake
  false                   // steering
);

Joystick_ Joystick2(
  JOYSTICK_DEFAULT_REPORT_ID + 1,
  JOYSTICK_TYPE_JOYSTICK,
  32,    // button count
  0,     // hat switches
  false, false, false,   // X, Y, Z
  false, false, false,   // Rx, Ry, Rz
  false,                  // rudder
  false,                  // throttle
  false,                  // accelerator
  false,                  // brake
  false                   // steering
);

Joystick_ Joystick3(
  JOYSTICK_DEFAULT_REPORT_ID + 2,
  JOYSTICK_TYPE_JOYSTICK,
  32,    // button count
  0,     // hat switches
  false, false, false,   // X, Y, Z
  false, false, false,   // Rx, Ry, Rz
  false,                  // rudder
  false,                  // throttle
  false,                  // accelerator
  false,                  // brake
  false                   // steering
);

Joystick_ Joystick4(
  JOYSTICK_DEFAULT_REPORT_ID + 3,
  JOYSTICK_TYPE_JOYSTICK,
  32,    // button count
  0,     // hat switches
  false, false, false,   // X, Y, Z
  false, false, false,   // Rx, Ry, Rz
  false,                  // rudder
  false,                  // throttle
  false,                  // accelerator
  false,                  // brake
  false                   // steering
);

// --- Physical button on pin 5 -> Button 0 ---
const int buttonPin = 5;
int lastLocalState = HIGH;

// --- Ethernet / UDP command listener setup ---
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };  // change if you run multiple devices
IPAddress ip(172, 16, 1, 11);                          // this device's static IP
unsigned int localPort = 4210;

EthernetUDP Udp;
char packetBuffer[64];

// --- UDP logging target ---
IPAddress logHost(172, 16, 1, 10);
unsigned int logPort = 27000;
EthernetUDP LogUdp;

// --- Heartbeat / keepalive target (JetRangerHealthMonitor) ---
unsigned int aliveport = 13137;
EthernetUDP aliveudp;
const unsigned long aliveinterval = 10000;
unsigned long lastalivesent = 0;

// Helper: convert IPAddress to a printable String
String ipToString(IPAddress addr) {
  return String(addr[0]) + "." + String(addr[1]) + "." + String(addr[2]) + "." + String(addr[3]);
}

void logMsg(const String &msg) {
  LogUdp.beginPacket(logHost, logPort);
  LogUdp.print(msg);
  LogUdp.endPacket();
}

void setup() {
  pinMode(buttonPin, INPUT_PULLUP);
  Joystick.begin();
  Joystick2.begin();
  Joystick3.begin();
  Joystick4.begin();
  Keyboard.begin();

  // Start Ethernet with a static IP (swap in Ethernet.begin(mac) for DHCP)
  Ethernet.begin(mac, ip);
  delay(1000); // give the shield a moment to initialize

  Udp.begin(localPort);
  LogUdp.begin(logPort); // local port for the logging socket (can reuse or pick another)
  aliveudp.begin(aliveport);

  logMsg("Joystick controller booted. IP=" + ipToString(ip) + " listening on port " + String(localPort));
}

void loop() {
  // --- Heartbeat to JetRangerHealthMonitor ---
  if ((millis() - lastalivesent) >= aliveinterval) {
    aliveudp.beginPacket(logHost, aliveport);
    aliveudp.print("JOYSTICK");
    aliveudp.endPacket();
    lastalivesent = millis();
  }

  // --- Local physical button (Button 0) ---
  int currentState = digitalRead(buttonPin);
  if (currentState != lastLocalState) {
    bool pressed = (currentState == LOW);
    Joystick.setButton(0, pressed);
    lastLocalState = currentState;

    logMsg("Local button 0 " + String(pressed ? "PRESSED" : "RELEASED"));
    delay(20); // debounce
  }

  // --- Remote UDP-triggered buttons ---
  int packetSize = Udp.parsePacket();
  if (packetSize) {
    int len = Udp.read(packetBuffer, sizeof(packetBuffer) - 1);
    if (len > 0) packetBuffer[len] = 0;

    IPAddress remoteIp = Udp.remoteIP();
    logMsg("RX from " + ipToString(remoteIp) + ": " + String(packetBuffer));

    processCommand(String(packetBuffer));
  }
}

const int BUTTONS_PER_JOYSTICK = 32;
Joystick_* joysticks[] = { &Joystick, &Joystick2, &Joystick3, &Joystick4 };
const int NUM_JOYSTICKS = 4;

// Expected formats:
//   "idx,state"       e.g. "17,1" presses joystick button 17, "17,0" releases it.
//                     idx 0-31 -> Joystick, 32-63 -> Joystick2, 64-95 -> Joystick3,
//                     96-127 -> Joystick4.
//   "KEY,key,state"   e.g. "KEY,a,1" presses the 'a' key, "KEY,ENTER,0" releases Enter
void processCommand(String cmd) {
  if (cmd.startsWith("KEY,")) {
    processKeyCommand(cmd.substring(4));
    return;
  }

  int commaIndex = cmd.indexOf(',');
  if (commaIndex == -1) {
    logMsg("Malformed command (no comma): " + cmd);
    return;
  }

  int idx = cmd.substring(0, commaIndex).toInt();
  int state = cmd.substring(commaIndex + 1).toInt();

  int joystickNum = idx / BUTTONS_PER_JOYSTICK;
  int localIdx = idx % BUTTONS_PER_JOYSTICK;

  if (idx >= 0 && joystickNum < NUM_JOYSTICKS) {
    joysticks[joystickNum]->setButton(localIdx, state == 1);
    logMsg("Joystick" + String(joystickNum + 1) + " button " + String(localIdx) + " set to " + String(state));
  } else {
    logMsg("Button index out of range: " + String(idx));
  }
}

// Expected format: "key,state"  e.g. "a,1" presses 'a', "ENTER,0" releases Enter.
// A single character is sent as-is; longer names map to Keyboard.h key codes below.
void processKeyCommand(String cmd) {
  int commaIndex = cmd.indexOf(',');
  if (commaIndex == -1) {
    logMsg("Malformed key command (no comma): " + cmd);
    return;
  }

  String keySpec = cmd.substring(0, commaIndex);
  int state = cmd.substring(commaIndex + 1).toInt();

  int keyCode = resolveKeyCode(keySpec);
  if (keyCode == 0) {
    logMsg("Unknown key: " + keySpec);
    return;
  }

  if (state == 1) {
    Keyboard.press(keyCode);
    logMsg("Key " + keySpec + " pressed");
  } else {
    Keyboard.release(keyCode);
    logMsg("Key " + keySpec + " released");
  }
}

// Maps a key name to a Keyboard.h key code. Single characters are sent as
// their ASCII value; longer names cover the common non-printable keys.
int resolveKeyCode(const String &keySpec) {
  if (keySpec.length() == 1) {
    return keySpec[0];
  }

  String k = keySpec;
  k.toUpperCase();

  if (k == "ENTER" || k == "RETURN") return KEY_RETURN;
  if (k == "ESC" || k == "ESCAPE") return KEY_ESC;
  if (k == "TAB") return KEY_TAB;
  if (k == "BACKSPACE") return KEY_BACKSPACE;
  if (k == "DELETE") return KEY_DELETE;
  if (k == "INSERT") return KEY_INSERT;
  if (k == "HOME") return KEY_HOME;
  if (k == "END") return KEY_END;
  if (k == "PAGEUP") return KEY_PAGE_UP;
  if (k == "PAGEDOWN") return KEY_PAGE_DOWN;
  if (k == "CAPSLOCK") return KEY_CAPS_LOCK;
  if (k == "UP") return KEY_UP_ARROW;
  if (k == "DOWN") return KEY_DOWN_ARROW;
  if (k == "LEFT") return KEY_LEFT_ARROW;
  if (k == "RIGHT") return KEY_RIGHT_ARROW;
  if (k == "LCTRL") return KEY_LEFT_CTRL;
  if (k == "RCTRL") return KEY_RIGHT_CTRL;
  if (k == "LSHIFT") return KEY_LEFT_SHIFT;
  if (k == "RSHIFT") return KEY_RIGHT_SHIFT;
  if (k == "LALT") return KEY_LEFT_ALT;
  if (k == "RALT") return KEY_RIGHT_ALT;
  if (k == "LGUI") return KEY_LEFT_GUI;
  if (k == "RGUI") return KEY_RIGHT_GUI;

  // F1-F12: KEY_F1..KEY_F12 are sequential in Keyboard.h
  if (k.length() >= 2 && k.length() <= 3 && k[0] == 'F') {
    int fNum = k.substring(1).toInt();
    if (fNum >= 1 && fNum <= 12) return KEY_F1 + (fNum - 1);
  }

  return 0;
}