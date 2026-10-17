#include <SPI.h>
#include <Ethernet.h>
#include <EthernetUdp.h>
#include <Joystick.h>

// --- Joystick setup: 64 buttons, no axes ---
Joystick_ Joystick(
  JOYSTICK_DEFAULT_REPORT_ID,
  JOYSTICK_TYPE_JOYSTICK,
  64,    // button count
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

// Expected format: "idx,state"  e.g. "17,1" presses button 17, "17,0" releases it
void processCommand(String cmd) {
  int commaIndex = cmd.indexOf(',');
  if (commaIndex == -1) {
    logMsg("Malformed command (no comma): " + cmd);
    return;
  }

  int idx = cmd.substring(0, commaIndex).toInt();
  int state = cmd.substring(commaIndex + 1).toInt();

  if (idx >= 0 && idx < 64) {
    Joystick.setButton(idx, state == 1);
    logMsg("Button " + String(idx) + " set to " + String(state));
  } else {
    logMsg("Button index out of range: " + String(idx));
  }
}