/*

  ////////////////////---||||||||||********||||||||||---\\\\\\\\\\\\\\\\\\\\
  //||                  FUNCTION = Jet Ranger ADF Controller             ||\\
  //||            ARDUINO PROCESSOR TYPE = Arduino Mega 2560            ||\\
  //||      ARDUINO CHIP SERIAL NUMBER = SN -       ||\\
  //||                    CONNECTED COM PORT = COM                   ||\\
  //||            ****DO CHECK S/N BEFORE UPLOAD NEW DATA****           ||\\
  ////////////////////---||||||||||********||||||||||---\\\\\\\\\\\\\\\\\\\\

  Drives the Jet Ranger ADF Controller PCB (rev 1.4). Built from the Jet Ranger Radio controller sketch
  (same Ethernet start-up, debug stream, heartbeat, encoder library, matrix scanner and character OLED code).

  What it does
    - ADF major and minor tuning knobs (D6/D7 and D8/D9). The major knob moves the ADF frequency in 100 kHz steps,
      the minor knob in 1 kHz steps (it speeds up when turned quickly). Range 190 to 1799 kHz.
    - Key matrix (D22 to D28, 4 rows by 3 columns) to type the ID of the aircraft to follow on the radar:
      digits 0-9 build the ID, '#' sends it, '*' deletes the last digit (on an empty entry '*' stops following).
    - Panel brightness pot on A0. It sets this board's own backlight (D5, IRF540N) and is sent as "C,B:0-15" to the
      other panels (Radio .101, Servo .102, Upper .103, Stepper .105, Dual Stepper .106) whenever the setting changes.
    - 12 V rail monitor on A15 (47k / 10k divider, 0.1 uF filter) and 5 V rail monitor (the ADC's AVcc reference, which is the
      5 V rail the AREF pin is tied to, measured against the internal 1.1 V band-gap). A power status update is sent every
      30 seconds.
    - Character OLED (2 lines of 16) on I2C shows the ADF frequency and the aircraft ID being entered or followed.

  Network
    172.16.1.107, listens on UDP 13136 for control packets only ("C,RESET:1" reboot, "C,MATRIX:1" raw keypad dump,
    "C,POWER:1" send a power status now). Debug text goes to 172.16.1.10:27000 and a heartbeat ("ADF_PANEL") to :13137.
    Events for the PC go to 172.16.1.10:27003 (not 27001: the FSUIPC and MSFS bridges treat anything they do not recognise
    on 27001 as a COM1 standby frequency). The bridges do not read port 27003 yet.
    The landing gear, flaps, speed brake and brakes switches (D29 to D32) and the two ADF knobs are also sent as joystick buttons to
    the USB HID board (172.16.1.11:4210, Joystick 4, indexes 104 to 111). The gear, flaps and speed brake switches are toggle switches, so
    their POSITION is sent to the bridge on UDP 27001 as a value, "GEAR_SET:1" (down) or "GEAR_SET:0" (up), "FLAPS_SET:1/0" and
    "SPOILERS_SET:1/0", when the switch changes and again every few seconds so the simulator always follows the switch; the brakes push
    button sends PARKING_BRAKES when it is pressed.
      ADF_FREQ:<kHz>               the ADF frequency after the knobs stop moving, for example ADF_FREQ:830
      RADAR_FOLLOW_ID:<digits>     the aircraft ID typed on the keypad; RADAR_FOLLOW_ID:0 = stop following
      POWER,V12:12.41,V5:5.02,STATUS:OK     every 30 seconds (STATUS is OK, or the rails that are out of range)

  Pins (from the PCB)
    D2 D3 D4  landing gear lamps (nose, left, right): driven by "D,LDGN:1/0,LDGL:1/0,LDGR:1/0" data packets (high = lit); no bridge sends them yet
    D5        BACKLIGHT_PWM (IRF540N)
    D6 D7     ADF major encoder A, B
    D8 D9     ADF minor encoder A, B
    D14 D15   green and red status LEDs
    D20 D21   I2C (OLED)
    D22-D25   keypad rows, D26-D28 keypad columns (connector pins 1-4 and 5-7: this order is unverified, see C,MATRIX:1)
    D29-D32   landing gear, flaps, speed brake and brake switches (closed to ground = on)
    D53       Ethernet module reset
    A0        panel brightness pot (wiper), A15  12 V monitor

  Do not call analogReference() in this sketch: the 5 V measurement and the pot / 12 V readings all use the default AVcc
  reference.

*/

// Master flag for throwing debug messages
bool Debug_Display = false;

#include "RotaryEncoder_Mega_Polling.h"

#define GREEN_STATUS_LED_PORT 14
#define RED_STATUS_LED_PORT 15  // RED LED is used for monitoring ethernet
#define FLASH_TIME 200

unsigned long NEXT_STATUS_TOGGLE_TIMER = 0;
bool RED_LED_STATE = false;
bool GREEN_LED_STATE = true;

#define BACK_LIGHTS 5  // PCB: D5 = BACKLIGHT_PWM

#define Ethernet_In_Use 1
int Reflector_In_Use = 1;

// Ethernet Related
#include <SPI.h>
#include <Ethernet.h>
#include <EthernetUdp.h>

byte mac[] = { 0xA8, 0x61, 0x0A, 0x67, 0x83, 0x6B };
String sMac = "A8:61:0A:67:83:6B";
IPAddress ip(172, 16, 1, 107);
String strMyIP = "172.16.1.107";

// Windows PC
IPAddress reflectorIP(172, 16, 1, 10);

const unsigned int localport = 7788;
const unsigned int MSFSport = 13136;       // control packets in, and the port the other boards listen on
const unsigned int aliveport = 13137;
const unsigned int reflectorport = 27000;  // debug text
const unsigned int adfEventPort = 27003;   // ADF frequency, radar follow ID and power status for the PC
const unsigned int bridgeCommandPort = 27001;  // simulator controls for the PC bridge (the bridges recognise the control names below)
IPAddress hidIP(172, 16, 1, 11);           // USB HID board: joystick buttons, "index,state"
const unsigned int hidPort = 4210;

EthernetUDP udp;
EthernetUDP MSFSudp;   // Control packets
EthernetUDP aliveudp;  // Keepalives to the Health Monitor

#define EthernetStartupDelay 500
#define ES1_RESET_PIN 53

const unsigned long aliveinterval = 10000;
unsigned long lastalivesent = 0;
const unsigned long delayBeforeSendingPacket = 3000;
unsigned long ethernetStartTime = 0;
String BoardName = "Jet Ranger ADF: ";

char packetBuffer[200];

void SendDebug(String MessageToSend) {
  MessageToSend = BoardName + MessageToSend;
  if ((Reflector_In_Use == 1) && (Ethernet_In_Use == 1)) {
    udp.beginPacket(reflectorIP, reflectorport);
    udp.print(MessageToSend);
    udp.endPacket();
  }
}

void sendToPc(const String& message) {
  if (Ethernet_In_Use != 1) return;
  udp.beginPacket(reflectorIP, adfEventPort);
  udp.print(message);
  udp.endPacket();
}

// ############################################# BEGIN I2C / CHARACTER OLED ##########################################
#include <Wire.h>

#define OLED_Address 0x3c
#define OLED_Command_Mode 0x80
#define OLED_Data_Mode 0x40

#define cmd_CLS 0x01
#define cmd_NewLine 0xC0

void sendData(unsigned char data) {
  Wire.beginTransmission(OLED_Address);
  Wire.write(OLED_Data_Mode);
  Wire.write(data);
  Wire.endTransmission();
}

void sendCommand(unsigned char command) {
  Wire.beginTransmission(OLED_Address);
  Wire.write(OLED_Command_Mode);
  Wire.write(command);
  Wire.endTransmission();
}

void send_string(const char* String) {
  unsigned char i = 0;
  while (String[i]) {
    sendData(String[i]);
    i++;
  }
}

// Same start-up sequence as the Radio controller's two character OLEDs, but on the Mega's own I2C bus (no TCA9548A).
void initCharOLED() {
  delay(10);
  sendCommand(0x2A);  // Set "RE"=1
  sendCommand(0x71);
  sendCommand(0x5C);
  sendCommand(0x28);

  sendCommand(0x08);  // Sleep mode on
  sendCommand(0x2A);  // "RE"=1
  sendCommand(0x79);  // "SD"=1

  sendCommand(0xD5);
  sendCommand(0x70);
  sendCommand(0x78);  // "SD"=0

  sendCommand(0x08);  // 5-dot, 1 or 2 line

  sendCommand(0x06);  // Com31-->Com0  Seg0-->Seg99

  sendCommand(0x2A);  // "RE"=1
  sendCommand(0x79);  // "SD"=1

  sendCommand(0x72);  // Set ROM
  sendCommand(0x00);  // ROM A and 8 CGRAM

  sendCommand(0xDA);  // Seg pins HW config
  sendCommand(0x10);

  sendCommand(0x81);  // Contrast
  sendCommand(0xFF);

  sendCommand(0xDB);  // VCOM deselect level
  sendCommand(0x30);  // VCC x 0.83

  sendCommand(0xDC);  // gpio - turn EN for 15V generator on
  sendCommand(0x03);

  sendCommand(0x78);  // Exiting set OLED characterization
  sendCommand(0x28);
  sendCommand(0x2A);
  sendCommand(0x06);  // Entry mode
  sendCommand(0x08);
  sendCommand(0x28);  // "IS"=0, "RE"=0
  sendCommand(0x01);
  sendCommand(0x80);  // DDRAM address 0x80 (line 1 start)

  delay(100);
  sendCommand(0x0C);  // Display on
}

// ############################################# END I2C / CHARACTER OLED ##########################################

// ############################################# BEGIN LANDING GEAR LAMPS ##########################################
// The three landing gear lamps on the ADF panel (nose, left, right; each LED has its own 330 ohm resistor on the panel, anode to the Mega pin).
// A data packet sets them: "D,LDGN:1,LDGL:1,LDGR:0" (1 = lit). Used by the SwingTester for the bench test; the simulator bridges do not send them yet.
#define LDG_NOSE_PIN 2
#define LDG_LEFT_PIN 3
#define LDG_RIGHT_PIN 4

void initLandingGearLamps() {
  pinMode(LDG_NOSE_PIN, OUTPUT);
  pinMode(LDG_LEFT_PIN, OUTPUT);
  pinMode(LDG_RIGHT_PIN, OUTPUT);
  digitalWrite(LDG_NOSE_PIN, LOW);
  digitalWrite(LDG_LEFT_PIN, LOW);
  digitalWrite(LDG_RIGHT_PIN, LOW);
}

// "LDGN", "1": returns true if the token was one of the landing gear lamps.
bool setLandingGearLampFromToken(const String& name, const String& value) {
  int pin = -1;
  if (name == "LDGN") pin = LDG_NOSE_PIN;
  else if (name == "LDGL") pin = LDG_LEFT_PIN;
  else if (name == "LDGR") pin = LDG_RIGHT_PIN;
  if (pin < 0) return false;
  digitalWrite(pin, value.toInt() != 0 ? HIGH : LOW);
  return true;
}

void HandleDataString(String str) {
  int colon = str.indexOf(':');
  if (colon <= 0) return;
  String name = str.substring(0, colon);
  String value = str.substring(colon + 1);
  name.trim();
  value.trim();
  setLandingGearLampFromToken(name, value);
}
// ############################################# END LANDING GEAR LAMPS ##########################################

// ############################################# BEGIN JOYSTICK BUTTONS AND PANEL SWITCHES ##########################################
unsigned long panelQuietUntil = 0;  // for a moment after start-up the switches are only read, not sent

// Forwards a button press (1) or release (0) to the USB HID board, which drives the matching virtual joystick button (Windows counts
// buttons from 1, so index 104 is Joystick 4 button 9).
void sendButtonToHid(int index, int state) {
  if (Ethernet_In_Use != 1) return;
  udp.beginPacket(hidIP, hidPort);
  udp.print(String(index) + "," + String(state));
  udp.endPacket();
}

// A knob click has no release, so it is sent as a short press and the release goes out from the main loop a few milliseconds later.
#define HID_PULSE_MS 40
#define HID_PULSE_SLOTS 4
struct HidPulse {
  int index;
  unsigned long releaseAt;
};
HidPulse hidPulses[HID_PULSE_SLOTS];

void pulseButtonToHid(int index) {
  for (int i = 0; i < HID_PULSE_SLOTS; i++) {
    if (hidPulses[i].index == 0 || hidPulses[i].index == index) {
      if (hidPulses[i].index == 0) sendButtonToHid(index, 1);
      hidPulses[i].index = index;
      hidPulses[i].releaseAt = millis() + HID_PULSE_MS;
      return;
    }
  }
}

void serviceHidPulses() {
  for (int i = 0; i < HID_PULSE_SLOTS; i++) {
    if (hidPulses[i].index != 0 && (long)(millis() - hidPulses[i].releaseAt) >= 0) {
      sendButtonToHid(hidPulses[i].index, 0);
      hidPulses[i].index = 0;
    }
  }
}

// Sends a simulator control to the bridge on UDP 27001.
void sendCommandToBridge(const char* command) {
  if (Ethernet_In_Use != 1) return;
  udp.beginPacket(reflectorIP, bridgeCommandPort);
  udp.print(command);
  udp.endPacket();
}

// The four switches on the ADF panel, to ground when on. The landing gear, flaps and speed brake switches are toggle switches, so their position
// is sent to the bridge as a value (positionCommand, with :1 when the switch is closed and :0 when it is open: gear down, flaps down, speed brakes
// out) every time it changes and again every POSITION_RESEND_MS, rather than as an event that the simulator could toggle. The brakes are a push
// button: pressing it sends pressCommand (the parking brake toggle) and releasing it sends nothing. If a switch works the wrong way round in the
// simulator, swap the two ends of the switch (or the 1 and 0 here).
#define SWITCH_DEBOUNCE_MS 30
#define POSITION_RESEND_MS 5000UL
struct AdfSwitch {
  uint8_t pin;
  uint8_t hidIndex;
  const char* positionCommand;  // "GEAR_SET": sent as GEAR_SET:1 / GEAR_SET:0; NULL for a push button
  const char* pressCommand;     // sent when the switch closes (push button only)
  const char* name;
  bool closed;         // debounced state
  bool lastReading;    // raw reading last time
  unsigned long changedAt;
};

AdfSwitch adfSwitches[] = {
  { 29, 104, "GEAR_SET", NULL, "LANDING GEAR", false, false, 0 },
  { 30, 105, "FLAPS_SET", NULL, "FLAPS", false, false, 0 },
  { 31, 106, "SPOILERS_SET", NULL, "SPEED BRAKES", false, false, 0 },
  { 32, 107, NULL, "PARKING_BRAKES", "BRAKES", false, false, 0 },
};
unsigned long lastPositionSent = 0;

// Sends the position of one toggle switch to the bridge, for example "GEAR_SET:1".
void sendSwitchPosition(const char* positionCommand, bool closed) {
  if (positionCommand == NULL) return;
  String command = String(positionCommand) + ":" + (closed ? "1" : "0");
  sendCommandToBridge(command.c_str());
}
const int adfSwitchCount = sizeof(adfSwitches) / sizeof(adfSwitches[0]);
bool adfSwitchStartSent = false;

void initAdfSwitches() {
  for (int i = 0; i < adfSwitchCount; i++) {
    pinMode(adfSwitches[i].pin, INPUT_PULLUP);
    adfSwitches[i].lastReading = (digitalRead(adfSwitches[i].pin) == LOW);
    adfSwitches[i].closed = adfSwitches[i].lastReading;
  }
  panelQuietUntil = millis() + 1500;
}

void pollAdfSwitches() {
  for (int i = 0; i < adfSwitchCount; i++) {
    AdfSwitch& s = adfSwitches[i];
    bool reading = (digitalRead(s.pin) == LOW);
    if (reading != s.lastReading) {
      s.lastReading = reading;
      s.changedAt = millis();
    }
    if (reading != s.closed && (millis() - s.changedAt) >= SWITCH_DEBOUNCE_MS) {
      s.closed = reading;
      if (millis() < panelQuietUntil) continue;  // start-up: record the state only
      SendDebug(String(s.name) + (s.closed ? " closed" : " open") + " (joystick button index " + String(s.hidIndex) + ")");
      sendButtonToHid(s.hidIndex, s.closed ? 1 : 0);
      if (s.positionCommand != NULL) {
        sendSwitchPosition(s.positionCommand, s.closed);
        lastPositionSent = millis();
      } else if (s.closed && s.pressCommand != NULL) {
        sendCommandToBridge(s.pressCommand);
      }
    }
  }
  // The positions of the toggle switches are sent again every few seconds once the start-up quiet period is over, so the simulator follows
  // the switches (including the position they were in at power-up, which no change reports).
  if (millis() >= panelQuietUntil && (long)(millis() - lastPositionSent) >= (long)POSITION_RESEND_MS) {
    lastPositionSent = millis();
    for (int i = 0; i < adfSwitchCount; i++) sendSwitchPosition(adfSwitches[i].positionCommand, adfSwitches[i].closed);
  }
  // Once the start-up quiet period is over, the joystick buttons are brought into line with the switches as they are (no simulator
  // command is sent, so the aircraft is not moved at power-up).
  if (!adfSwitchStartSent && millis() >= panelQuietUntil) {
    adfSwitchStartSent = true;
    for (int i = 0; i < adfSwitchCount; i++) sendButtonToHid(adfSwitches[i].hidIndex, adfSwitches[i].closed ? 1 : 0);
  }
}
// ############################################# END JOYSTICK BUTTONS AND PANEL SWITCHES ##########################################

// ############################################# BEGIN ADF TUNING ##########################################
// ADF frequency in kHz. The major knob is 100 kHz per click, the minor knob 1 kHz per click (faster when spun).
#define ADF_MIN_KHZ 190
#define ADF_MAX_KHZ 1799
#define ADF_MAJOR_STEP_KHZ 100
#define ADF_MINOR_STEP_KHZ 1
#define ADF_SEND_SETTLE_MS 250  // send the frequency this long after the last knob movement

int adfKhz = 340;
bool adfFreqDirty = false;
unsigned long adfLastChange = 0;

// The encoder library counts ClicksPerIncrement counts for one detent. Each encoder is kept near the middle of its range and
// recentred after every read, so the change since the last read is simply the distance from the middle.
#define ClicksPerIncrement 3
#define EncoderCentre 1000
RotaryEncoder AdfMajorEncoder(6, 7, 0, 2 * EncoderCentre, 1);
RotaryEncoder AdfMinorEncoder(8, 9, 0, 2 * EncoderCentre, 1);
int16_t majorRemainder = 0;
int16_t minorRemainder = 0;

// Whole detents turned since the last call (positive = clockwise).
int pollEncoderDetents(RotaryEncoder& encoder, int16_t& remainder) {
  encoder.poll();
  int16_t delta = encoder.getPosition() - EncoderCentre;
  if (delta == 0) return 0;
  encoder.setPosition(EncoderCentre);
  remainder += delta;
  int detents = remainder / ClicksPerIncrement;
  remainder -= detents * ClicksPerIncrement;
  return detents;
}
// ############################################# END ADF TUNING ##########################################

// ############################################# BEGIN AIRCRAFT ID ENTRY ##########################################
#define ID_MAX_DIGITS 8
#define ID_ENTRY_TIMEOUT_MS 20000  // an unfinished entry is dropped after this long without a key

String idBuffer = "";   // digits being typed
String activeId = "";   // the ID last sent with '#' ("" = not following)
bool idEntering = false;
unsigned long lastKeyMillis = 0;
bool displayDirty = true;
unsigned long lastDisplayMillis = 0;

void updateDisplay() {
  char line[17];
  snprintf(line, sizeof(line), "ADF %4d kHz    ", adfKhz);
  sendCommand(0x80);
  send_string(line);

  String second;
  if (idEntering) second = "ID: " + idBuffer + "_";
  else if (activeId.length() > 0) second = "FOLLOW " + activeId;
  else second = "FOLLOW OFF";
  while (second.length() < 16) second += ' ';
  if (second.length() > 16) second = second.substring(0, 16);
  sendCommand(cmd_NewLine);
  send_string(second.c_str());
}

void onKeyChar(char key) {
  lastKeyMillis = millis();
  if (key >= '0' && key <= '9') {
    if (!idEntering) {
      idEntering = true;
      idBuffer = "";
    }
    if (idBuffer.length() < ID_MAX_DIGITS) idBuffer += key;
  } else if (key == '#') {
    if (idEntering && idBuffer.length() > 0) {
      activeId = idBuffer;
      idEntering = false;
      sendToPc("RADAR_FOLLOW_ID:" + activeId);
      SendDebug("Radar follow ID " + activeId);
    }
  } else if (key == '*') {
    if (idEntering && idBuffer.length() > 0) {
      idBuffer.remove(idBuffer.length() - 1);
    } else {
      // Nothing being typed: stop following.
      idEntering = false;
      idBuffer = "";
      if (activeId.length() > 0) {
        activeId = "";
        sendToPc("RADAR_FOLLOW_ID:0");
        SendDebug("Radar follow stopped");
      }
    }
  }
  displayDirty = true;
}
// ############################################# END AIRCRAFT ID ENTRY ##########################################

// ############################################# BEGIN MATRIX SCANNER  ##########################################
// 4 rows (D22-D25) by 3 columns (D26-D28). Rows are driven LOW one at a time, columns are read with pull-ups.
const int rowPins[4] = { 22, 23, 24, 25 };
const int colPins[3] = { 26, 27, 28 };
const int ROWS = 4;
const int COLS = 3;

const unsigned long DEBOUNCE_DELAY = 50;
unsigned long lastDebounceTime[ROWS][COLS];
bool buttonState[ROWS][COLS];
bool lastButtonState[ROWS][COLS];
bool keyPressed[ROWS][COLS];

// The label of the key at each (row, column) of the scan, as found on the real keypad: with the first guess ({1,2,3},{4,5,6},{7,8,9},{*,0,#}) the keys
// were reported as * 8, 0 9, # 7, 7 5, 8 6, 9 4, 4 2, 5 3, 6 1, 1 0, 2 # and 3 *, so each key's label is put where it was actually reported
// (the keypad's rows come out of the scan in the order 4-5-6, 7-8-9, *-0-#, 1-2-3 with the columns rotated by one).
char keyMap[ROWS][COLS] = {
  { '6', '4', '5' },
  { '9', '7', '8' },
  { '#', '*', '0' },
  { '3', '1', '2' }
};

// An unwired or floating column reads LOW as soon as scanning starts, which would be reported as key presses at every boot,
// so for matrixQuietMs after start-up only the state is recorded and nothing is reported.
#define matrixQuietMs 1500
unsigned long matrixQuietUntil = 0;

void initMatrixScanner() {
  matrixQuietUntil = millis() + matrixQuietMs;
  for (int i = 0; i < ROWS; i++) {
    pinMode(rowPins[i], OUTPUT);
    digitalWrite(rowPins[i], HIGH);
  }
  for (int i = 0; i < COLS; i++) pinMode(colPins[i], INPUT_PULLUP);
  for (int row = 0; row < ROWS; row++) {
    for (int col = 0; col < COLS; col++) {
      buttonState[row][col] = HIGH;
      lastButtonState[row][col] = HIGH;
      keyPressed[row][col] = false;
      lastDebounceTime[row][col] = 0;
    }
  }
}

void scanMatrix() {
  for (int row = 0; row < ROWS; row++) {
    for (int i = 0; i < ROWS; i++) digitalWrite(rowPins[i], (i == row) ? LOW : HIGH);
    delayMicroseconds(10);

    for (int col = 0; col < COLS; col++) {
      bool reading = digitalRead(colPins[col]);
      if (reading != lastButtonState[row][col]) lastDebounceTime[row][col] = millis();

      if ((millis() - lastDebounceTime[row][col]) > DEBOUNCE_DELAY) {
        if (reading != buttonState[row][col]) {
          buttonState[row][col] = reading;
          if (millis() < matrixQuietUntil) {
            keyPressed[row][col] = false;
          } else if (buttonState[row][col] == LOW && !keyPressed[row][col]) {
            keyPressed[row][col] = true;
            SendDebug("Key Pressed: Row " + String(row) + ", Col " + String(col) + " -> '" + String(keyMap[row][col]) + "'");
            onKeyChar(keyMap[row][col]);
          } else if (buttonState[row][col] == HIGH && keyPressed[row][col]) {
            keyPressed[row][col] = false;
          }
        }
      }
      lastButtonState[row][col] = reading;
    }
  }
  for (int i = 0; i < ROWS; i++) digitalWrite(rowPins[i], HIGH);
}

// "C,MATRIX:1": the RAW matrix readings go to the debug stream (no debounce, no quiet period): first with every row HIGH,
// then one line per row with only that row LOW. H = column reads HIGH, L = LOW. A closed key turns one H into L on its own row's
// line. Use it to check which connector pin is a row and which is a column.
void reportMatrixRaw() {
  for (int i = 0; i < ROWS; i++) digitalWrite(rowPins[i], HIGH);
  delayMicroseconds(50);
  String line = "Matrix raw idle (all rows HIGH), cols 0-2: ";
  for (int c = 0; c < COLS; c++) line += (digitalRead(colPins[c]) ? "H" : "L");
  SendDebug(line);
  for (int r = 0; r < ROWS; r++) {
    for (int i = 0; i < ROWS; i++) digitalWrite(rowPins[i], (i == r) ? LOW : HIGH);
    delayMicroseconds(50);
    String rl = "Matrix raw row " + String(r) + " LOW, cols 0-2: ";
    for (int c = 0; c < COLS; c++) rl += (digitalRead(colPins[c]) ? "H" : "L");
    SendDebug(rl);
  }
  for (int i = 0; i < ROWS; i++) digitalWrite(rowPins[i], HIGH);
}
// ############################################# END MATRIX SCANNER  ##########################################

// ############################################# BEGIN PANEL BRIGHTNESS ##########################################
// Pot on A0 (5 V to GND, wiper to A0). The 0-15 level drives this board's backlight and is sent to the other panels as C,B:level.
#define BRIGHTNESS_PIN A0
// The brightness pot is switched off for the moment: with USE_BRIGHTNESS_POT 0 the pot (A0) is not read, no C,B brightness is sent to the other panels,
// and this board's own backlight stays at the default level below. Set it to 1 to bring the pot back.
#define USE_BRIGHTNESS_POT 0
#define BRIGHTNESS_DEFAULT_LEVEL 4  // 0-15 (4 is PWM 68, close to the other boards' default of 65)
#define BRIGHTNESS_SAMPLE_MS 20
#define BRIGHTNESS_FIRST_SEND_MS 5000  // after start-up, so the other boards are up and the pot position is applied to them

const uint8_t brightnessBoards[] = { 101, 102, 103, 105, 106 };  // Radio, Servo, Upper, Stepper, Dual Stepper (not the USB HID board)
const int brightnessBoardCount = sizeof(brightnessBoards);
uint8_t pendingBrightnessMask = 0;  // one bit per board still to be told; one board is sent per loop so a dead board cannot stall the knobs

int brightnessAverage = 0;  // pot reading, smoothed, 0-1023
int brightnessLevel = 0;    // 0-15
unsigned long lastBrightnessSample = 0;
bool brightnessFirstSent = false;

int levelFromPot(int reading) {
  return constrain((reading * 15L + 511) / 1023, 0, 15);
}

void applyOwnBacklight() {
  analogWrite(BACK_LIGHTS, map(brightnessLevel, 0, 15, 0, 255));
}

void queueBrightness() {
  pendingBrightnessMask = (1 << brightnessBoardCount) - 1;
}

void sendPendingBrightness() {
#if !USE_BRIGHTNESS_POT
  return;
#endif
  if (pendingBrightnessMask == 0) return;
  for (int i = 0; i < brightnessBoardCount; i++) {
    if (pendingBrightnessMask & (1 << i)) {
      pendingBrightnessMask &= ~(1 << i);
      udp.beginPacket(IPAddress(172, 16, 1, brightnessBoards[i]), MSFSport);
      udp.print("C,B:" + String(brightnessLevel));
      udp.endPacket();
      return;
    }
  }
}

void pollBrightness() {
#if !USE_BRIGHTNESS_POT
  return;
#endif
  if (millis() - lastBrightnessSample < BRIGHTNESS_SAMPLE_MS) return;
  lastBrightnessSample = millis();
  brightnessAverage = (brightnessAverage * 7 + analogRead(BRIGHTNESS_PIN)) / 8;
  // A level only changes once the pot is clearly past the boundary, so a reading sitting on a boundary does not flicker.
  int newLevel = levelFromPot(brightnessAverage);
  if (newLevel != brightnessLevel) {
    float exact = brightnessAverage * 15.0f / 1023.0f;
    if (fabs(exact - brightnessLevel) > 0.7f) {
      brightnessLevel = newLevel;
      applyOwnBacklight();
      queueBrightness();
      SendDebug("Panel brightness " + String(brightnessLevel));
    }
  }
  if (!brightnessFirstSent && millis() >= BRIGHTNESS_FIRST_SEND_MS) {
    brightnessFirstSent = true;
    queueBrightness();
  }
}
// ############################################# END PANEL BRIGHTNESS ##########################################

// ############################################# BEGIN POWER MONITOR ##########################################
// 12 V rail: R3 47k from the rail, R4 10k to ground, 0.1 uF across R4, midpoint to A15. 5 V rail: the ADC reference (AVcc) measured
// against the internal 1.1 V band-gap, so the 12 V and pot readings are scaled by the measured 5 V instead of an assumed 5.00.
#define V12_PIN A15
#define V12_R_TOP 47000.0f
#define V12_R_BOTTOM 10000.0f
#define V12_CAL 1.0f           // multiply to trim the reading against a meter
#define V5_CAL 1.0f
#define POWER_STATUS_INTERVAL_MS 30000UL
#define POWER_FIRST_STATUS_MS 5000UL
#define V12_LOW 10.8f
#define V12_HIGH 15.0f
#define V5_LOW 4.75f
#define V5_HIGH 5.25f

unsigned long lastPowerStatus = 0;
bool powerFirstSent = false;

// AVcc in millivolts, from the 1.1 V band-gap (ATmega2560: ADMUX MUX4:0 = 11110, MUX5 = 0).
long readVccMillivolts() {
  analogRead(V12_PIN);  // make sure the ADC is running and settled
  ADMUX = _BV(REFS0) | _BV(MUX4) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
  ADCSRB &= ~_BV(MUX5);
  delay(5);  // the band-gap and the reference capacitor on AREF need a moment to settle
  long sum = 0;
  for (int i = 0; i < 9; i++) {
    ADCSRA |= _BV(ADSC);
    while (bit_is_set(ADCSRA, ADSC)) {}
    if (i > 0) sum += ADC;  // first conversion thrown away
  }
  long reading = sum / 8;
  if (reading <= 0) return 0;
  return (long)(1125300.0f / reading * V5_CAL);
}

float readV12(float vcc) {
  analogRead(V12_PIN);  // first reading after a reference change is discarded
  long sum = 0;
  for (int i = 0; i < 16; i++) sum += analogRead(V12_PIN);
  float pinVolts = (sum / 16.0f) * vcc / 1023.0f;
  return pinVolts * (V12_R_TOP + V12_R_BOTTOM) / V12_R_BOTTOM * V12_CAL;
}

void sendPowerStatus() {
  float v5 = readVccMillivolts() / 1000.0f;
  float v12 = readV12(v5 > 0 ? v5 : 5.0f);

  String status = "";
  if (v5 < V5_LOW) status += "5V_LOW ";
  if (v5 > V5_HIGH) status += "5V_HIGH ";
  if (v12 < V12_LOW) status += "12V_LOW ";
  if (v12 > V12_HIGH) status += "12V_HIGH ";
  status.trim();
  if (status.length() == 0) status = "OK";
  status.replace(' ', '+');

  String message = "POWER,V12:" + String(v12, 2) + ",V5:" + String(v5, 2) + ",STATUS:" + status;
  sendToPc(message);
  SendDebug(message);
  lastPowerStatus = millis();
}
// ############################################# END POWER MONITOR ##########################################

// ###################### Control packets over UDP ######################
// "C,RESET:1" reboots this board by a software jump to the reset vector (not the watchdog, see the Radio controller sketch for why).
#include <avr/wdt.h>
#include <avr/interrupt.h>
void DisableWatchdogAtStartup() __attribute__((naked)) __attribute__((section(".init3"))) __attribute__((used));
void DisableWatchdogAtStartup() {
  MCUSR = 0;
  wdt_disable();
}

void RebootBoard() {
  SendDebug("RESET command received - rebooting");
  delay(100);
  cli();
  wdt_disable();
  EIMSK = 0;
  PCICR = 0;
  TIMSK0 = 0;
  TIMSK1 = 0;
  TIMSK2 = 0;
  TIMSK3 = 0;
  TIMSK4 = 0;
  TIMSK5 = 0;
  TWCR = 0;
  SPCR = 0;
  ADCSRA = 0;
  UCSR0B = 0;
  UCSR1B = 0;
  UCSR2B = 0;
  UCSR3B = 0;
  asm volatile("jmp 0");
}

void HandleControlString(String str) {
  int colon = str.indexOf(':');
  if (colon <= 0) return;
  String command = str.substring(0, colon);
  command.trim();
  if (command == "RESET") RebootBoard();
  else if (command == "MATRIX") reportMatrixRaw();
  else if (command == "POWER") sendPowerStatus();
}

void ProcessReceivedPacket() {
  // Control packets ("C,COMMAND:value,...") and data packets ("D,LDGN:1,...": the landing gear lamps only; everything else in a data packet is ignored).
  char* token = strtok(packetBuffer, ",");
  if (token == NULL || (token[0] != 'C' && token[0] != 'D')) return;
  bool isControl = (token[0] == 'C');
  token = strtok(NULL, ",");
  while (token != NULL) {
    if (isControl) HandleControlString(String(token));
    else HandleDataString(String(token));
    token = strtok(NULL, ",");
  }
}
// ######################################################################

void setup() {
  pinMode(GREEN_STATUS_LED_PORT, OUTPUT);
  pinMode(RED_STATUS_LED_PORT, OUTPUT);
  digitalWrite(GREEN_STATUS_LED_PORT, true);
  digitalWrite(RED_STATUS_LED_PORT, false);

  pinMode(BACK_LIGHTS, OUTPUT);
#if USE_BRIGHTNESS_POT
  brightnessAverage = analogRead(BRIGHTNESS_PIN);
  brightnessLevel = levelFromPot(brightnessAverage);
#else
  brightnessLevel = BRIGHTNESS_DEFAULT_LEVEL;
#endif
  applyOwnBacklight();

  if (Ethernet_In_Use == 1) {
    // Manual reset of the Ethernet module instead of tying it to the Arduino reset
    pinMode(ES1_RESET_PIN, OUTPUT);
    digitalWrite(ES1_RESET_PIN, LOW);
    delay(2);
    digitalWrite(ES1_RESET_PIN, HIGH);

    Ethernet.begin(mac, ip);
    // The W5500 retries an unanswered ARP for 200 ms x 8 by default. Cut to 10 ms so a board that is switched off cannot stall the
    // knobs and keypad while the brightness is being sent.
    Ethernet.setRetransmissionTimeout(10);
    udp.begin(localport);
    MSFSudp.begin(MSFSport);
    aliveudp.begin(aliveport);

    ethernetStartTime = millis() + delayBeforeSendingPacket;
    while (millis() <= ethernetStartTime) {
      delay(FLASH_TIME);
      digitalWrite(RED_STATUS_LED_PORT, false);
      delay(FLASH_TIME);
      digitalWrite(RED_STATUS_LED_PORT, true);
    }
    SendDebug("Ethernet Started " + strMyIP + " " + sMac);
  }

  Wire.begin();
  initCharOLED();
  updateDisplay();

  SendDebug("Start Encoders");
  AdfMajorEncoder.begin();
  AdfMinorEncoder.begin();
  AdfMajorEncoder.setAccelerationParams(5, 80, 3, 3);
  AdfMinorEncoder.setAccelerationParams(5, 80, 5, 10);
  AdfMajorEncoder.enableAcceleration(false);
  AdfMinorEncoder.enableAcceleration(true);
  AdfMajorEncoder.setPosition(EncoderCentre);
  AdfMinorEncoder.setPosition(EncoderCentre);

  SendDebug("Start Matrix");
  initMatrixScanner();
  initAdfSwitches();
  initLandingGearLamps();

  SendDebug("Setup Complete");
}

void loop() {
  if (millis() >= NEXT_STATUS_TOGGLE_TIMER) {
    GREEN_LED_STATE = !GREEN_LED_STATE;
    RED_LED_STATE = !GREEN_LED_STATE;
    digitalWrite(GREEN_STATUS_LED_PORT, GREEN_LED_STATE);
    digitalWrite(RED_STATUS_LED_PORT, RED_LED_STATE);
    NEXT_STATUS_TOGGLE_TIMER = millis() + FLASH_TIME;
  }

  if ((millis() - lastalivesent) >= aliveinterval) {
    if (Ethernet_In_Use == 1) {
      aliveudp.beginPacket(reflectorIP, aliveport);
      aliveudp.print("ADF_PANEL");
      aliveudp.endPacket();
    }
    lastalivesent = millis();
  }

  // ADF knobs
  int majorDetents = pollEncoderDetents(AdfMajorEncoder, majorRemainder);
  int minorDetents = pollEncoderDetents(AdfMinorEncoder, minorRemainder);
  if (majorDetents != 0 && millis() >= panelQuietUntil) pulseButtonToHid(majorDetents > 0 ? 108 : 109);  // Joystick 4 buttons 13 and 14
  if (minorDetents != 0 && millis() >= panelQuietUntil) pulseButtonToHid(minorDetents > 0 ? 110 : 111);  // Joystick 4 buttons 15 and 16
  if (majorDetents != 0 || minorDetents != 0) {
    int newKhz = constrain(adfKhz + majorDetents * ADF_MAJOR_STEP_KHZ + minorDetents * ADF_MINOR_STEP_KHZ, ADF_MIN_KHZ, ADF_MAX_KHZ);
    if (newKhz != adfKhz) {
      adfKhz = newKhz;
      adfFreqDirty = true;
      adfLastChange = millis();
      displayDirty = true;
    }
  }
  if (adfFreqDirty && (millis() - adfLastChange) >= ADF_SEND_SETTLE_MS) {
    adfFreqDirty = false;
    sendToPc("ADF_FREQ:" + String(adfKhz));
    SendDebug("ADF frequency " + String(adfKhz) + " kHz");
  }

  scanMatrix();
  pollAdfSwitches();
  serviceHidPulses();

  // An entry that has been left unfinished is dropped and the display goes back to the ID being followed.
  if (idEntering && (millis() - lastKeyMillis) >= ID_ENTRY_TIMEOUT_MS) {
    idEntering = false;
    idBuffer = "";
    displayDirty = true;
  }

  pollBrightness();
  sendPendingBrightness();

  if (displayDirty && (millis() - lastDisplayMillis) >= 50) {
    displayDirty = false;
    lastDisplayMillis = millis();
    updateDisplay();
  }

  // Power status every 30 seconds (the first one shortly after start-up)
  if (!powerFirstSent && millis() >= POWER_FIRST_STATUS_MS) {
    powerFirstSent = true;
    sendPowerStatus();
  } else if (powerFirstSent && (millis() - lastPowerStatus) >= POWER_STATUS_INTERVAL_MS) {
    sendPowerStatus();
  }

  // Control packets
  int packetSize = MSFSudp.parsePacket();
  if (packetSize > 0) {
    int len = MSFSudp.read(packetBuffer, sizeof(packetBuffer) - 1);
    if (len > 0) {
      packetBuffer[len] = 0;
      ProcessReceivedPacket();
    }
  }
}
