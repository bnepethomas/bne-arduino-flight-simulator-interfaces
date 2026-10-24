/*

  ////////////////////---||||||||||********||||||||||---\\\\\\\\\\\\\\\\\\\\
  //||                  FUNCTION = Jet Ranger Radio                      ||\\
  //||            ARDUINO PROCESSOR TYPE = Arduino Mega 2560            ||\\
  //||      ARDUINO CHIP SERIAL NUMBER = SN -       ||\\
  //||                    CONNECTED COM PORT = COM                   ||\\
  //||            ****DO CHECK S/N BEFORE UPLOAD NEW DATA****           ||\\
  ////////////////////---||||||||||********||||||||||---\\\\\\\\\\\\\\\\\\\\

Todos
// Issues when using Due connected to USB closest to Power Socket
// Apears to get further when connected to socket next to reset switch
// Currently Stalls when Ethernet is enabled
// LCD Does started

*/

// Master flag for throwing debug messages
bool Debug_Display = false;

unsigned long lastEncoderUpdate = millis();
bool radioFrequencyChanged = false;
bool powerAvailable = true;

// No-data timeout, same idea as the Servo/Stepper/Dual Stepper boards: if no UDP
// packet (data "D,..." or control "C,...") arrives on MSFSudp for
// noDataTimeoutMs, the COM and NAV displays are blanked - exactly what already
// happens when powerAvailable is false (updateCOMM()/updateNAV() clear the
// display). The timer starts at the END of setup(). When packets resume, the
// previous power state is restored and both displays are redrawn.
// Local input also counts: any encoder movement or key press wakes the displays
// straight away (see wakeDisplays()) and restarts the 30 s, so the radio can be
// used on its own with no PC feed; the displays blank again once there has been
// neither UDP data nor local input for noDataTimeoutMs.
const unsigned long noDataTimeoutMs = 30000;
unsigned long lastMSFSDataMillis = 0;
unsigned long lastLocalActivityMillis = 0;
bool noDataBlanked = false;
bool powerBeforeTimeout = true;
String com1ActiveFrequency = "119.000";
String com2ActiveFrequency = "120.000";
String com1StandbyFrequency = "121.000";
String com2StandbyFrequency = "122.000";
String Nav1ActiveFrequency = "109.000";
String Nav2ActiveFrequency = "110.000";
String Nav1StandbyFrequency = "111.000";
String Nav2StandbyFrequency = "112.000";
String mainBusVoltage = "";
String navCom1Status = "";


#include "RotaryEncoder_Mega_Polling.h"

#define GREEN_STATUS_LED_PORT 14
#define RED_STATUS_LED_PORT 15  // RED LED is used for monitoring ethernet
#define FLASH_TIME 200

unsigned long NEXT_STATUS_TOGGLE_TIMER = 0;
bool RED_LED_STATE = false;
bool GREEN_LED_STATE = true;
unsigned long timeSinceRedLedChanged = 0;

#define Ethernet_In_Use 1
int Reflector_In_Use = 1;

// When Using Arduino Due this is not supported
/*
#define DCSBIOS_IRQ_SERIAL
#include "DcsBios.h"
*/

// Ethernet Related
#include <SPI.h>
#include <Ethernet.h>
#include <EthernetUdp.h>

// These local Mac and IP Address will be reassigned early in startup based on
// the device ID as set by address pins
byte mac[] = { 0xA8, 0x61, 0x0A, 0x9E, 0x83, 0x01 };
String sMac = "A8:61:0A:9E:83:01";
IPAddress ip(172, 16, 1, 101);
String strMyIP = "172.16.1.101";

// Raspberry Pi is Target
IPAddress reflectorIP(172, 16, 1, 10);
String strreflectorIP = "X.X.X.X";




const unsigned int localport = 7788;
const unsigned int localdebugport = 7789;
const unsigned int MSFSport = 13136;
const unsigned int aliveport = 13137;
const unsigned int reflectorport = 27000;


int packetSize;
int debugLen;
EthernetUDP udp;
EthernetUDP debugUDP;

int MSFSpacketsize;
int MSFSLen;

EthernetUDP max7219udp;  // Max7219
EthernetUDP MSFSudp;     // Listens to MSFS light commands
EthernetUDP aliveudp;    // Sends keepalives to monitoring application

#define EthernetStartupDelay 500
#define ES1_RESET_PIN 53

const unsigned long aliveinterval = 10000;
long lastalivesent = 0;
const unsigned long delayBeforeSendingPacket = 3000;
unsigned long ethernetStartTime = 0;
String BoardName = "Jet Ranger Radio: ";

char packetBuffer[1000];     //buffer to store the incoming data
char outpacketBuffer[1000];  //buffer to store the outgoing data

void SendDebug(String MessageToSend) {
  MessageToSend = BoardName + MessageToSend;
  if ((Reflector_In_Use == 1) && (Ethernet_In_Use == 1)) {
    udp.beginPacket(reflectorIP, reflectorport);
    udp.print(MessageToSend);
    udp.endPacket();
  }
}

// ********************** Added Smoothing Filter for AOA Indexer Brightness
// Not used in UIP combined controller
// From https://github.com/jonnieZG/EWMA
#include <Ewma.h>

// ********************* End Smoothing Filter *************

// ############################################# BEGIN I2C FRAMEWORK ##########################################
#include <Wire.h>

// When Using Arduino Due this is not supported
/*
extern "C" {
#include "utility/twi.h"  // from Wire library, so we can do bus scanning
}
*/
#define TCAADDR 0x70



void tcaselect(uint8_t i) {
  if (i > 7) return;

  Wire.beginTransmission(TCAADDR);
  Wire.write(1 << i);
  Wire.endTransmission();
}

// ############################################# END I2C FRAMEWORK ##########################################


// ############################################# BEGIN CHARACTER OLED ##########################################

#define OLED_Address 0x3c
#define OLED_Command_Mode 0x80
#define OLED_Data_Mode 0x40

#define cmd_CLS 0x01
#define cmd_NewLine 0xC0
#define STX 0x02
#define ETX 0x03


// Clear Screen
//  sendCommand(0x01);
// DEL - CLS
//  sendCommand(0x80);          // Home Pos
// DEL - CLS
//  sendCommand(0xC0);
// Clear screen as used in Jet Ranger
//  sendCommand(cmd_CLS);              // Clear Display

void initCharOLED(uint8_t i2c_address) {
  // *** I2C initial *** //

  String wrkString;

  tcaselect(i2c_address);
  delay(10);
  sendCommand(0x2A);  // **** Set "RE"=1	00101010B
  sendCommand(0x71);
  sendCommand(0x5C);
  sendCommand(0x28);

  sendCommand(0x08);  // **** Set Sleep Mode On
  sendCommand(0x2A);  // **** Set "RE"=1	00101010B
  sendCommand(0x79);  // **** Set "SD"=1	01111001B

  sendCommand(0xD5);
  sendCommand(0x70);
  sendCommand(0x78);  // **** Set "SD"=0

  sendCommand(0x08);  // **** Set 5-dot, 3 or 4 line(0x09), 1 or 2 line(0x08)

  sendCommand(0x06);  // **** Set Com31-->Com0  Seg0-->Seg99

  // **** Set OLED Characterization *** //
  sendCommand(0x2A);  // **** Set "RE"=1
  sendCommand(0x79);  // **** Set "SD"=1

  // **** CGROM/CGRAM Management *** //
  sendCommand(0x72);  // **** Set ROM
  sendCommand(0x00);  // **** Set ROM A and 8 CGRAM


  sendCommand(0xDA);  // **** Set Seg Pins HW Config
  sendCommand(0x10);

  sendCommand(0x81);  // **** Set Contrast
  sendCommand(0xFF);

  sendCommand(0xDB);  // **** Set VCOM deselect level
  sendCommand(0x30);  // **** VCC x 0.83

  sendCommand(0xDC);  // **** Set gpio - turn EN for 15V generator on.
  sendCommand(0x03);

  sendCommand(0x78);  // **** Exiting Set OLED Characterization
  sendCommand(0x28);
  sendCommand(0x2A);
  //sendCommand(0x05); 	// **** Set Entry Mode
  sendCommand(0x06);  // **** Set Entry Mode
  sendCommand(0x08);
  sendCommand(0x28);  // **** Set "IS"=0 , "RE" =0 //28
  sendCommand(0x01);
  sendCommand(0x80);  // **** Set DDRAM Address to 0x80 (line 1 start)

  delay(100);
  sendCommand(0x0C);  // **** Turn on Display

  send_string("XXX.XX   XXX.XX");
  sendCommand(cmd_NewLine);
  wrkString = "TEST " + String(i2c_address);
  send_string(wrkString.c_str());
}

void sendData(unsigned char data) {
  Wire.beginTransmission(OLED_Address);  // **** Start I2C
  Wire.write(OLED_Data_Mode);            // **** Set OLED Data mode
  Wire.write(data);
  Wire.endTransmission();  // **** End I2C
}

void sendCommand(unsigned char command) {
  Wire.beginTransmission(OLED_Address);  // **** Start I2C
  Wire.write(OLED_Command_Mode);         // **** Set OLED Command mode
  Wire.write(command);
  Wire.endTransmission();  // **** End I2C
}

void send_string(const char *String) {
  unsigned char i = 0;
  while (String[i]) {
    sendData(String[i]);  // *** Show String to OLED
    i++;
  }
}

#define NAV_OLED_Port 0
#define COMM_OLED_Port 1

// The PC bridges send frequencies with 2 or 3 decimals ("128.30", "127.8").
// Pad to a fixed 7 characters ("128.300") so the two columns on each OLED line up
// and COM2 / NAV2 are not pulled left behind the faceplate.
String padFrequency(const String& f) {
  String s = f;
  s.trim();
  int dot = s.indexOf('.');
  if (dot < 0) {
    s += ".";
    dot = s.length() - 1;
  }
  while ((int)s.length() - dot - 1 < 3) s += "0";
  while (dot < 3) {
    s = " " + s;
    dot++;
  }
  return s;
}

// Power state the NAV display was last drawn with. A data packet only redraws COMM
// (the PC sends no NAV frequencies), so when the power state changes NAV must be
// redrawn too, straight after COMM, or it stays lit when the COM display goes off.
bool navDrawnPower = true;

void updateNAV() {
  tcaselect(NAV_OLED_Port);
  sendCommand(0x80);
  navDrawnPower = powerAvailable;

  if (powerAvailable == true) {
    //sendCommand(cmd_CLS);
    String workstring1 = padFrequency(Nav1ActiveFrequency) + "  " + padFrequency(Nav2ActiveFrequency);
    String workstring2 = padFrequency(Nav1StandbyFrequency) + "  " + padFrequency(Nav2StandbyFrequency);

    send_string(workstring1.c_str());
    sendCommand(cmd_NewLine);
    send_string(workstring2.c_str());
  } else
    sendCommand(cmd_CLS);
}

void updateCOMM() {
  tcaselect(COMM_OLED_Port);
  sendCommand(0x80);

  if (powerAvailable == true) {
    //sendCommand(cmd_CLS);
    String workstring1 = padFrequency(com1ActiveFrequency) + "  " + padFrequency(com2ActiveFrequency);
    String workstring2 = padFrequency(com1StandbyFrequency) + "  " + padFrequency(com2StandbyFrequency);

    send_string(workstring1.c_str());
    sendCommand(cmd_NewLine);
    send_string(workstring2.c_str());
  } else
    sendCommand(cmd_CLS);
}

// ############################################# END CHARACTER OLED ##########################################

// ############################################# BEGIN MAX7219 PANEL LEDS ##########################################
// One MAX7219 on the Radio Auto Pilot PCB drives the 19 LEDs on the Auto Pilot panel (digits 0 and 1) and the Audio panel
// (digits 2 and 3). The segment pins are ANNODE_00 to ANNODE_07 (A to G, DP) and the digit pins are CATHODE_00 to CATHODE_07, so
// a panel LED is lit by one segment of one digit. Serial lines: DIN = A15, CLK = A14, LOAD (CS) = A13 (/MAX_7219-DIN, -CLOCK, -LOAD).
// The PC bridges send each LED as a "LED_xxx:0/1" pair in the radio data packet; the LEDs are only lit while the radio has power
// (NAVCOM1:1, the avionics bus 24 V or more) and data or local input is arriving, the same rule that blanks the COM and NAV displays.
// "C,LEDTEST:1" lights all 19 LEDs, "C,LEDTEST:0" puts them back to the simulator's state.
#include <LedControl.h>

#define MAX7219_DIN A15
#define MAX7219_CLK A14
#define MAX7219_LOAD A13
#define MAX7219_INTENSITY 8  // 0 to 15: the level used until a C,B brightness command arrives

LedControl panelLeds(MAX7219_DIN, MAX7219_CLK, MAX7219_LOAD, 1);

// LedControl column numbers: 0 = DP, 1 = A, 2 = B, 3 = C, 4 = D, 5 = E, 6 = F, 7 = G.
struct PanelLed {
  const char* token;
  uint8_t digit;    // MAX7219 digit = CATHODE_0n
  uint8_t column;   // segment = ANNODE_0n (column 1 = ANNODE_00)
  bool wanted;      // state from the simulator
  bool shown;       // what is on the LED now
};

PanelLed panelLedTable[] = {
  // Auto Pilot panel, digit 0 (CATHODE_00) and digit 1 (CATHODE_01)
  { "LED_CRS", 0, 1, false, false },
  { "LED_HDG", 0, 2, false, false },
  { "LED_IAS", 0, 3, false, false },
  { "LED_ALT", 0, 4, false, false },
  { "LED_APPR", 1, 1, false, false },
  { "LED_FD", 1, 2, false, false },
  { "LED_BC", 1, 3, false, false },
  { "LED_APENG", 1, 4, false, false },
  // Audio panel, digit 2 (CATHODE_02) and digit 3 (CATHODE_03)
  { "LED_COM1", 2, 1, false, false },
  { "LED_BOTH", 2, 2, false, false },
  { "LED_NAV2", 2, 3, false, false },
  { "LED_DME", 2, 4, false, false },
  { "LED_OUTER", 2, 5, false, false },
  { "LED_INNER", 2, 6, false, false },
  { "LED_COM2", 3, 1, false, false },
  { "LED_NAV1", 3, 2, false, false },
  { "LED_MKR", 3, 3, false, false },
  { "LED_ADF", 3, 4, false, false },
  { "LED_MID", 3, 5, false, false },
};
const int panelLedCount = sizeof(panelLedTable) / sizeof(panelLedTable[0]);
bool panelLedTest = false;

void initPanelLeds() {
  panelLeds.shutdown(0, false);  // wake from power-down
  panelLeds.setScanLimit(0, 3);  // only digits 0 to 3 are wired to panel LEDs
  panelLeds.setIntensity(0, MAX7219_INTENSITY);
  panelLeds.clearDisplay(0);
}

// Puts each LED to what it should show: the simulator's state while the radio is powered, otherwise off. Only LEDs that
// changed are written.
void applyPanelLeds() {
  bool powered = powerAvailable;
  for (int i = 0; i < panelLedCount; i++) {
    bool on = panelLedTest || (powered && panelLedTable[i].wanted);
    if (on != panelLedTable[i].shown) {
      panelLeds.setLed(0, panelLedTable[i].digit, panelLedTable[i].column, on);
      panelLedTable[i].shown = on;
    }
  }
}

// Panel brightness, "C,B:0-15" (the same command the other panels' backlights take, for example from the ADF controller's pot):
// 0 switches the LEDs off, 1 to 15 set the MAX7219 intensity (1 dimmest, 15 brightest). The level is kept by the chip.
void setPanelLedBrightness(int level) {
  if (level < 0 || level > 15) return;
  if (level == 0) {
    panelLeds.shutdown(0, true);  // display off, the digit registers keep their contents
  } else {
    panelLeds.shutdown(0, false);
    panelLeds.setIntensity(0, level);
  }
}

// "LED_CRS", "1": returns true if the token was one of the panel LEDs.
bool setPanelLedFromToken(const String& name, const String& value) {
  for (int i = 0; i < panelLedCount; i++) {
    if (name == panelLedTable[i].token) {
      panelLedTable[i].wanted = (value.toInt() != 0);
      return true;
    }
  }
  return false;
}
// ############################################# END MAX7219 PANEL LEDS ##########################################

// ############################################# BEGIN ENCODER ##########################################


// Encoder(PinA, PinB, MinVal, MaxVal, tba)
// 
#define  ClicksPerIncrement 3

// Can use ANY pins on Mega (no interrupt limitation!)
// Create 4 encoders for X, Y, Z, and Menu control
/// Comm1 Major Minor


// For Comm Major the range is 118 to 136 - 18 steps
// For Comm Minor the range is 0 to 975 in 25 kHz steps
// But we want to roll underneath and over the top so add three steps at top and bottom
// that as soon as they are hit we move to the other end of the scale before any processing
// Standard channel spacing (2026-10-16):
//   COM  118.000-136.975 MHz in 25 kHz channels (the major knob runs 118..136, the
//        minor knob 000, 025, 050 ... 975). 8.33 kHz channels are not offered.
//   NAV  108.00-117.95 MHz in 50 kHz steps (major 108..117, minor 000, 050 ... 950).
// A minor knob with N steps per MHz has positions 0..(N+1)*ClicksPerIncrement: position
// 3 is step 0, and the rollover code below wraps at both ends.
#define COM_MINOR_STEP_KHZ 25
#define NAV_MINOR_STEP_KHZ 50
int Comm1MajorEncoderMaxValue = 18 * ClicksPerIncrement;
int CommMinorEncoderMaxValue = (1000 / COM_MINOR_STEP_KHZ + 1) * ClicksPerIncrement;
int NavMinorEncoderMaxValue = (1000 / NAV_MINOR_STEP_KHZ + 1) * ClicksPerIncrement;
RotaryEncoder Comm1MajorEncoder(31, 30, 0, Comm1MajorEncoderMaxValue, 1);
// For Comm Minor see CommMinorEncoderMaxValue above
RotaryEncoder Comm1MinorEncoder(33, 32, 0, CommMinorEncoderMaxValue, 1);



// Comm2 
// Comm2 Major 
RotaryEncoder Comm2MajorEncoder(34, 35, 0, Comm1MajorEncoderMaxValue, 1);
// Comm2 Minor
RotaryEncoder Comm2MinorEncoder(37, 36, 0, CommMinorEncoderMaxValue, 1);


// Nav1 
// Nav1 Major
RotaryEncoder Nav1MajorEncoder(38, 39, 0, 9 * 3, 1);
// Nav1 Minor
RotaryEncoder Nav1MinorEncoder(41, 40, 0, NavMinorEncoderMaxValue, 1);


// Nav2 
// Nav2 Major
RotaryEncoder Nav2MajorEncoder(42, 43, 0, 9 * 3, 1);
// Nav2 Minor
RotaryEncoder Nav2MinorEncoder(45, 44, 0, NavMinorEncoderMaxValue, 1);


// RotaryEncoder encoderZ(6, 7, -1000, 1000);
// RotaryEncoder encoderMenu(8, 9, 0, 10);

// ############################################# END ENCODER  ##########################################

// ############################################# BEGIN MATRIX SCANNER  ##########################################
// Matrix Keypad Scanner with Debounce
// Rows: D22-D25, Columns: D32-D35

// Pin definitions
const int rowPins[4] = { 22, 23, 24, 25 };  // Row pins D22-D25
const int colPins[4] = { 26, 27, 28, 29 };  // Column pins D32-D35

// Matrix configuration
const int ROWS = 4;
const int COLS = 4;

// Debounce settings
const unsigned long DEBOUNCE_DELAY = 50;     // 50ms debounce delay
unsigned long lastDebounceTime[ROWS][COLS];  // Debounce timing for each key
bool buttonState[ROWS][COLS];                // Current state of each button
bool lastButtonState[ROWS][COLS];            // Previous state of each button
bool keyPressed[ROWS][COLS];                 // Tracks if key is currently pressed

// Optional: Define key mapping (customize as needed)
char keyMap[ROWS][COLS] = {
  { '1', '2', '3', 'A' },
  { '4', '5', '6', 'B' },
  { '7', '8', '9', 'C' },
  { '*', '0', '#', 'D' }
};

// Phantom key presses: an unwired / floating matrix column reads LOW as soon as
// scanning starts, which used to be reported as real key presses at every boot
// (and each one sent its command - battery, alternator, COM swaps - to the PC).
// For matrixQuietMs after initMatrixScanner() the scanner now only records what
// it sees (keys that are already down count as "down" but raise no press or
// release event), so only genuine changes after start-up are reported.
#define matrixQuietMs 1500
unsigned long matrixQuietUntil = 0;

void initMatrixScanner() {
  matrixQuietUntil = millis() + matrixQuietMs;
  // Initialize row pins as outputs (will be set HIGH to scan)
  for (int i = 0; i < ROWS; i++) {
    pinMode(rowPins[i], OUTPUT);
    digitalWrite(rowPins[i], HIGH);  // Initially set all rows HIGH
  }

  // Initialize column pins as inputs with pull-up resistors
  for (int i = 0; i < COLS; i++) {
    pinMode(colPins[i], INPUT_PULLUP);
  }

  // Initialize button states
  for (int row = 0; row < ROWS; row++) {
    for (int col = 0; col < COLS; col++) {
      buttonState[row][col] = HIGH;  // HIGH = not pressed (pull-up)
      lastButtonState[row][col] = HIGH;
      keyPressed[row][col] = false;
      lastDebounceTime[row][col] = 0;
    }
  }
}

void scanMatrix() {
  for (int row = 0; row < ROWS; row++) {
    // Set current row LOW, others HIGH
    for (int i = 0; i < ROWS; i++) {
      digitalWrite(rowPins[i], (i == row) ? LOW : HIGH);
    }

    // Small delay to allow signal to stabilize
    delayMicroseconds(10);

    // Read all columns for this row
    for (int col = 0; col < COLS; col++) {
      bool reading = digitalRead(colPins[col]);

      // Check if button state has changed
      if (reading != lastButtonState[row][col]) {
        lastDebounceTime[row][col] = millis();  // Reset debounce timer
      }

      // Check if debounce time has elapsed
      if ((millis() - lastDebounceTime[row][col]) > DEBOUNCE_DELAY) {
        // If button state has changed after debounce period
        if (reading != buttonState[row][col]) {
          buttonState[row][col] = reading;

          if (millis() < matrixQuietUntil) {
            // Start-up quiet period: record the state, report nothing.
            keyPressed[row][col] = false;
          } else if (buttonState[row][col] == LOW && !keyPressed[row][col]) {
            // Key press detection (LOW = pressed due to pull-up): just pressed
            keyPressed[row][col] = true;
            onKeyPress(row, col);
          } else if (buttonState[row][col] == HIGH && keyPressed[row][col]) {
            // Key was just released
            keyPressed[row][col] = false;
            onKeyRelease(row, col);
          }
        }
      }

      lastButtonState[row][col] = reading;
    }
  }

  // Set all rows HIGH again
  for (int i = 0; i < ROWS; i++) {
    digitalWrite(rowPins[i], HIGH);
  }
}

// Sends a knob-set standby frequency to the PC bridge (172.16.1.10:27001).
// Before, every knob sent the bare number ("118.295", "108.000") whichever radio
// it belonged to; the bridge treats a bare number as COM1 standby and anything
// outside 118-136.975 as 121.50, so turning the COM2 or a NAV knob retuned COM1
// (and every boot, which "moves" all four encoders from zero, set COM1 standby to
// 121.50). Now the radio name is sent: COM1 stays the bare number (the SimConnect/
// P3D bridges only know that form), COM2 / NAV1 / NAV2 are sent as
// "COM2_STANDBY:118.295" / "NAV1_STANDBY:108.000" / "NAV2_STANDBY:108.000", which
// FSUIPCWinformsAutoCS routes to the matching radio. Nothing is sent during the
// start-up quiet period (matrixQuietUntil), so the encoders' power-up positions
// are not pushed to the simulator.
void sendStandbyToPC(const char* radio, const String& frequency) {
  if (Ethernet_In_Use != 1) return;
  if (millis() < matrixQuietUntil) return;
  SendDebug("To PC: " + String(radio) + (radio[0] != 0 ? ":" : "COM1 ") + frequency);
  udp.beginPacket(reflectorIP, 27001);
  if (radio[0] != 0) {
    udp.print(radio);
    udp.print(":");
  }
  udp.print(frequency);
  udp.endPacket();
}

// Local activity (any encoder movement or key press): restart the no-data
// timeout and, if the displays are blanked, restore them (power state back to
// what it was, both displays redrawn). Call BEFORE updateCOMM()/updateNAV() in the
// encoder handlers so a turned knob draws on a restored display.
void wakeDisplays() {
  lastLocalActivityMillis = millis();
  if (noDataBlanked) {
    noDataBlanked = false;
    powerAvailable = powerBeforeTimeout;
    updateNAV();
    updateCOMM();
    applyPanelLeds();
    SendDebug("Local input - displays restored");
  }
}

// Diagnostic: UDP control packet "C,MATRIX:1" sends the RAW matrix readings to
// the debug stream, bypassing debounce and the start-up quiet period. First line:
// all four columns with every row HIGH (idle). Then one line per row with only that
// row driven LOW. "H" = column reads HIGH, "L" = column reads LOW. Hold a switch
// down and send the command to see which row/column it changes: a closed switch
// should turn exactly one "H" into "L" on its own row's line.
void reportMatrixRaw() {
  for (int i = 0; i < ROWS; i++) digitalWrite(rowPins[i], HIGH);
  delayMicroseconds(50);
  String line = "Matrix raw idle (all rows HIGH), cols 0-3: ";
  for (int c = 0; c < COLS; c++) line += (digitalRead(colPins[c]) ? "H" : "L");
  SendDebug(line);
  for (int r = 0; r < ROWS; r++) {
    for (int i = 0; i < ROWS; i++) digitalWrite(rowPins[i], (i == r) ? LOW : HIGH);
    delayMicroseconds(50);
    String rl = "Matrix raw row " + String(r) + " LOW, cols 0-3: ";
    for (int c = 0; c < COLS; c++) rl += (digitalRead(colPins[c]) ? "H" : "L");
    SendDebug(rl);
  }
  for (int i = 0; i < ROWS; i++) digitalWrite(rowPins[i], HIGH);
}

// Callback function for key press events
void onKeyPress(int row, int col) {
  wakeDisplays();
  SendDebug("Key Pressed: Row " + String(row) + ", Col " + String(col) + " -> '" + String(keyMap[row][col]) + "'");
  // Add your custom key press handling here
  handleKeyPress(row, col, keyMap[row][col]);
}

// Callback function for key release events
void onKeyRelease(int row, int col) {
  SendDebug("Key Released: Row " + String(row) + ", Col " + String(col) + " -> '" + String(keyMap[row][col]) + "'");
  // Add your custom key release handling here
  handleKeyRelease(row, col, keyMap[row][col]);
}

// Custom key press handler - modify this for your application
void handleKeyPress(int row, int col, char key) {
  if (row == 2 && col == 0) {
    SendDebug("COM1 Swap");
    if (Ethernet_In_Use == 1) {
      udp.beginPacket(reflectorIP, 27001);
      udp.print("COM1_RADIO_SWAP");
      udp.endPacket();
    }
  } else if (row == 3 && col == 0) {
    SendDebug("COM2 Swap");
    if (Ethernet_In_Use == 1) {
      udp.beginPacket(reflectorIP, 27001);
      udp.print("COM2_RADIO_SWAP");
      udp.endPacket();
      udp.beginPacket(reflectorIP, 27001);
      udp.print("AVIONICS_MASTER_SET");
      udp.endPacket();
    }
  } else if (row == 0 && col == 0) {
    // NAV1 swap - the "NAV1 FREQ SWITCH" (SW6) on the Jet Ranger COM and NAV PCB
    // sits at matrix row 0, col 0. Swap the active and standby NAV1 frequencies on
    // the NAV display straight away (the PC does not send NAV frequencies back,
    // unlike COM) and tell the PC bridge to swap NAV1 in the simulator.
    SendDebug("NAV1 Swap");
    String swapTemp = Nav1ActiveFrequency;
    Nav1ActiveFrequency = Nav1StandbyFrequency;
    Nav1StandbyFrequency = swapTemp;
    updateNAV();
    if (Ethernet_In_Use == 1) {
      udp.beginPacket(reflectorIP, 27001);
      udp.print("NAV1_RADIO_SWAP");
      udp.endPacket();
    }
  } else if (row == 1 && col == 0) {
    // NAV2 swap - the "NAV2 FREQ SWITCH" (SW7) on the COM and NAV PCB, row 1, col 0.
    SendDebug("NAV2 Swap");
    String swapTemp = Nav2ActiveFrequency;
    Nav2ActiveFrequency = Nav2StandbyFrequency;
    Nav2StandbyFrequency = swapTemp;
    updateNAV();
    if (Ethernet_In_Use == 1) {
      udp.beginPacket(reflectorIP, 27001);
      udp.print("NAV2_RADIO_SWAP");
      udp.endPacket();
    }
  } else {
    switch (key) {
      case '1':
      case '2':
      case '3':
      case '4':
      case '5':
      case '6':
      case '7':
      case '8':
      case '9':
      case '0':
        SendDebug("Number key pressed: " + String(key));
        break;
      case 'A':
      case 'B':
      case 'C':
      case 'D':
        SendDebug("Letter key pressed: " + String(key));
        break;
      case '*':
        SendDebug("Star key pressed");
        break;
      case '#':
        SendDebug("Hash key pressed");
        break;
      default:
        SendDebug("Unknown key");
        break;
    }
  }
}

// Custom key release handler - modify this for your application
void handleKeyRelease(int row, int col, char key) {
  // Add any key release specific functionality here
  // For example, you might want to clear a display or stop an action
}

// Utility function to check if any key is currently pressed
bool isAnyKeyPressed() {
  for (int row = 0; row < ROWS; row++) {
    for (int col = 0; col < COLS; col++) {
      if (keyPressed[row][col]) {
        return true;
      }
    }
  }
  return false;
}

// Utility function to get the currently pressed key
char getCurrentKey() {
  for (int row = 0; row < ROWS; row++) {
    for (int col = 0; col < COLS; col++) {
      if (keyPressed[row][col]) {
        return keyMap[row][col];
      }
    }
  }
  return '\0';  // No key pressed
}

// ############################################# END MATRIX SCANNER  ##########################################

// ########################################## BEGIN MSFS DATA RECEIVER ########################################

// This code is based on UIP_MAX7219_NEXTRON_POWER_RELAY in the Hornet Project


void ProcessReceivedMSFSString() {


  // Reading values from packetBuffer which is global
  // All received values are strings for readability
  // NHandles multiple attribute Values in a single packet
  //    D,1:0,2:1,3:1,4:1,5:0,6:0,7:0,8:0,9:1,10:1


  bool bLocalDebug = true;
  char *ParameterNamePtr;
  char *ParameterValuePtr;

  //if (Debug_Display || bLocalDebug ) SendDebug("Processing Packet :" + String(millis()));
  // SendDebug("Processing MSFS Packet");

  bLocalDebug = false;

  String sWrkStr = "";

  const char *delim = ",";

  // Break the received packet into a series of tokens
  // If there is no match the pointer will be null, other points to first parameter
  ParameterNamePtr = strtok(packetBuffer, delim);



  String ParameterNameString(ParameterNamePtr);
  //if (Debug_Display || bLocalDebug ) SendDebug("Parameter Name " + String(ParameterNameString));
  //SendDebug("Parameter Name " + String(ParameterNameString));
  // Print all of the values received as a outer loop
  // and then split inner values
  /* get the first token */

  /* walk through other tokens */

  String wrkstring = "";

  //if (Debug_Display || bLocalDebug )
  //if (ParameterNamePtr != NULL) SendDebug("First Value is: " + String(ParameterNamePtr));
  if (ParameterNameString[0] == 'D') {
    //Handling a Data Packet
    //if (Debug_Display || bLocalDebug ) SendDebug("Handling a Data Packet");
    ParameterNamePtr = strtok(NULL, delim);

    while (ParameterNamePtr != NULL) {
      //if (Debug_Display || bLocalDebug ) SendDebug( "Processing " + String(ParameterNamePtr) );

      wrkstring = String(ParameterNamePtr);
      HandleOutputValuePair(wrkstring);


      ParameterNamePtr = strtok(NULL, delim);
    }


    if (radioFrequencyChanged == true) {
      updateCOMM();
      if (navDrawnPower != powerAvailable) updateNAV();  // NAV on/off together with COMM
      radioFrequencyChanged = false;
    }
    applyPanelLeds();  // new LED states, and the LEDs follow the radio power like the displays
    return;
    // End Handling a Data Packet
  } else if (ParameterNameString[0] == 'C') {
    // Handling a Control Packet
    //if (Debug_Display || bLocalDebug ) SendDebug("Handling a Control Packet");

    ParameterNamePtr = strtok(NULL, delim);

    while (ParameterNamePtr != NULL) {
      //if (Debug_Display || bLocalDebug )SendDebug( "Processing " + String(ParameterNamePtr) );

      wrkstring = String(ParameterNamePtr);
      HandleControlString(wrkstring);

      ParameterNamePtr = strtok(NULL, delim);
    }


    return;

    // Handling a Control Packet
  } else {
    // Unknown Packet Type
    //if (Debug_Display || bLocalDebug ) SendDebug("Unknown Packet Type - ignoring packet");
    return;
  }
}

void HandleOutputValuePair(String str) {

  // Panel LEDs ("LED_CRS:1" ...) are not logged one by one: there are 19 of them in every radio packet.
  if (str.startsWith("LED_")) {
    int colon = str.indexOf(':');
    if (colon > 0) {
      String value = str.substring(colon + 1);
      value.trim();
      setPanelLedFromToken(str.substring(0, colon), value);
    }
    return;
  }

  // We are expected a LedNumber which has XRRCC where X = Max7219 Unit, RR Row, CC Column
  bool bLocalDebug = false;


  //if (Debug_Display || bLocalDebug ) SendDebug("Handling " + str);
  SendDebug("Handling " + str);

  int delimeterlocation = 0;
  String workstring = "";
  String ParameterName = "";
  String ParameterValue = "";



  delimeterlocation = str.indexOf(':');

  if (delimeterlocation == 0) {
    if (Debug_Display || bLocalDebug) SendDebug("**** WARNING no delimiter passed ****** Looking for :");
  } else {
    //if (Debug_Display || bLocalDebug ) SendDebug("Delimiter is located a position " + String(delimeterlocation));
    ParameterName = getValue(str, ':', 0);
    //if (Debug_Display || bLocalDebug ) SendDebug("lednumber is " + lednumber);
    ParameterValue = getValue(str, ':', 1);
    //if (Debug_Display || bLocalDebug ) SendDebug("ledvalue is " + ledvalue);

    // As the value could contain the null at the end of the string trim it out
    ParameterValue.trim();

    if (ParameterName == "C1A") {
      //SendDebug("Received COM1 Active Frequency: " + ParameterValue);
      if (com1ActiveFrequency != ParameterValue) {
        SendDebug("COM1 Active Frequency changed");
        com1ActiveFrequency = ParameterValue;
        radioFrequencyChanged = true;
      };
    } else if (ParameterName == "C1S") {
      //SendDebug("Received COM1 Standby Frequency: " + ParameterValue);
      if (com1StandbyFrequency != ParameterValue) {
        SendDebug("COM1 Standby Frequency changed");
        // Only set if it has been more 500mS since last update from local encoder
        if ((millis() - lastEncoderUpdate) >= 500) {
          com1StandbyFrequency = ParameterValue;
        }
        radioFrequencyChanged = true;
      };
    } else if (ParameterName == "C2A") {
      //SendDebug("Received COM2 Standby Frequency: " + ParameterValue);
      if (com2ActiveFrequency != ParameterValue) {
        SendDebug("COM2 Active Frequency changed");
        // Only set if it has been more 500mS since last update from local encoder
        if ((millis() - lastEncoderUpdate) >= 500) {
          com2ActiveFrequency = ParameterValue;
        }
        radioFrequencyChanged = true;
      };
    } else if (ParameterName == "C2S") {
      //SendDebug("Received COM2 Standby Frequency: " + ParameterValue);
      if (com2StandbyFrequency != ParameterValue) {
        SendDebug("COM2 Standby Frequency changed");
        // Only set if it has been more 500mS since last update from local encoder
        if ((millis() - lastEncoderUpdate) >= 500) {
          com2StandbyFrequency = ParameterValue;
        }
        radioFrequencyChanged = true;
      };
    } else if (ParameterName == "NOTMAINBUS") {
      SendDebug("Received MAINBUS: " + ParameterValue);
      if (mainBusVoltage != ParameterValue) {
        SendDebug("Main Bus Voltage Changed");
        // Only set if it has been more 500mS since last update from local encoder
        mainBusVoltage = ParameterValue;
        if (mainBusVoltage == "0.0")
          powerAvailable = false;
        else
          powerAvailable = true;

        radioFrequencyChanged = true;
      }
    } else if (ParameterName == "NAVCOM1") {
      SendDebug("Received NAVCOM1: " + ParameterValue);
      if (navCom1Status != ParameterValue) {
        SendDebug("NAVCOM1 Status Changed");
        // Only set if it has been more 500mS since last update from local encoder
        navCom1Status = ParameterValue;
        if (navCom1Status == "0")
          powerAvailable = false;
        else
          powerAvailable = true;

        radioFrequencyChanged = true;
      };

      return;
    }
  }
}

// ###################### Remote reset over UDP ######################
// UDP control packet "C,RESET:1" reboots this board (value ignored). Done as a
// SOFTWARE reset - quiesce the peripherals, then jump to the reset vector - NOT
// with the AVR watchdog. The watchdog method (used on the Servo, Stepper and Dual
// Stepper boards) hung this board: after a watchdog reset the chip's watchdog
// restarts at its shortest timeout, and this board's bootloader does not switch
// it off, so it reset over and over and never reached the sketch (recovered only
// by uploading again). A jump to 0 does not touch the bootloader or the
// watchdog, so it works whatever bootloader is fitted. The Ethernet module is
// reset again by setup() (ES1_RESET_PIN) and Ethernet.begin() re-initialises SPI.
// The .init3 function below still clears the watchdog first thing after any
// reset (standard avr-libc pattern) in case it was ever left armed.
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
  // Switch off everything that would otherwise survive the jump with live state.
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
// ###################################################################

void HandleControlString(String str) {
  bool bLocalDebug = false;
  //if (Debug_Display || bLocalDebug ) SendDebug("Handling Control String :" + str);

  // Currnetly just handling Brightness - eg C,B:3

  int delimeterlocation = 0;
  String command = "";
  String setting = "";


  delimeterlocation = str.indexOf(':');

  if (delimeterlocation == 0) {
    //if (Debug_Display || bLocalDebug ) SendDebug("**** WARNING no delimiter passed ****** Looking for :");
  } else {
    //if (Debug_Display || bLocalDebug ) SendDebug("Delimiter is located a position " + String(delimeterlocation));
    command = getValue(str, ':', 0);
    //if (Debug_Display || bLocalDebug ) SendDebug("command is :" + command);
    setting = getValue(str, ':', 1);
    //if (Debug_Display || bLocalDebug ) SendDebug("setting is :" + setting);

    int isetting = setting.toInt();

    // Brightness, C,B:0-15: sets the intensity of the Auto Pilot and Audio panel LEDs (0 = off)
    if (command == "B")
      if (isetting >= 0 && isetting <= 15) {
        setPanelLedBrightness(isetting);
        SendDebug("Panel LED brightness " + String(isetting));
      }
    //else if (Debug_Display || bLocalDebug ) SendDebug("Invalid Brightness value passed. Value is :" + String(setting));

    // Remote reset: C,RESET:1
    if (command == "RESET") RebootBoard();

    // Matrix diagnostic: C,MATRIX:1 (see reportMatrixRaw())
    if (command == "MATRIX") reportMatrixRaw();

    // Panel LED test: C,LEDTEST:1 lights all 19 MAX7219 LEDs, C,LEDTEST:0 returns them to the simulator's state
    if (command == "LEDTEST") {
      panelLedTest = (isetting != 0);
      applyPanelLeds();
    }
  }

  return;
}

String getValue(String data, char separator, int index) {
  int found = 0;
  int strIndex[] = { 0, -1 };
  int maxIndex = data.length() - 1;

  for (int i = 0; i <= maxIndex && found <= index; i++) {
    if (data.charAt(i) == separator || i == maxIndex) {
      found++;
      strIndex[0] = strIndex[1] + 1;
      strIndex[1] = (i == maxIndex) ? i + 1 : i;
    }
  }
  return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}

boolean isValidNumber(String str) {
  boolean isNum = false;
  if (!(str.charAt(0) == '+' || str.charAt(0) == '-' || isDigit(str.charAt(0)))) return false;

  for (byte i = 1; i < str.length(); i++) {
    if (!(isDigit(str.charAt(i)) || str.charAt(i) == '.')) return false;
  }
  return true;
}


// ############################################## END DATA RECEIVER ###########################################


void setup() {

  pinMode(GREEN_STATUS_LED_PORT, OUTPUT);
  pinMode(RED_STATUS_LED_PORT, OUTPUT);
  digitalWrite(GREEN_STATUS_LED_PORT, true);
  digitalWrite(RED_STATUS_LED_PORT, false);

  if (Ethernet_In_Use == 1) {

    // Using manual reset instead of tying to Arduino Reset
    pinMode(ES1_RESET_PIN, OUTPUT);
    digitalWrite(ES1_RESET_PIN, LOW);
    delay(2);
    digitalWrite(ES1_RESET_PIN, HIGH);

    Ethernet.begin(mac, ip);
    // W5500's own ARP/send retry timeout: default 200ms x 8 retries = up to 1600ms
    // blocked inside udp.beginPacket()/endPacket() any time a send fails to get an
    // ARP reply (e.g. the reflector PC or HID board is down). Cut to 10ms so a dead
    // destination can't stall the encoder and matrix scanning. Retry count left at its default (8),
    // so worst case is now ~80ms instead of ~1600ms.
    Ethernet.setRetransmissionTimeout(10);
    udp.begin(localport);
    debugUDP.begin(localdebugport);
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




  // When Using Arduino Due this is not supported
  /*
  for (uint8_t t = 0; t < 8; t++) {
    tcaselect(t);
    // Had to comment out these debugging messages as they created a conflict with the IRQ definition in DCS BIOS
    SendDebug("TCA Port #" + String(t));

    for (uint8_t addr = 0; addr <= 127; addr++) {
      //if (addr == TCAADDR) continue;

      uint8_t data;
      if (!twi_writeTo(addr, &data, 0, 1, 1)) {
        SendDebug("Found I2C " + String(addr));
      }
    }
  }
  // Had to comment out these debugging messages as they created a conflict with the IRQ definition in DCS BIOS
  SendDebug("I2C scan complete");
  */

  initCharOLED(0);
  initCharOLED(1);
  initPanelLeds();
  delay(1000);
  // updateNAV(100);
  //updateCOMM( "118.50", "119.50", "120.50", "121.50");

  SendDebug("Start Encoders");
  Comm1MajorEncoder.begin();
  Comm1MinorEncoder.begin();
  Comm2MajorEncoder.begin();
  Comm2MinorEncoder.begin();
  Nav1MajorEncoder.begin();
  Nav1MinorEncoder.begin();
  Nav2MajorEncoder.begin();
  Nav2MinorEncoder.begin();
  // Configure each encoder
  Comm1MajorEncoder.setAccelerationParams(5, 80, 3, 3);
  Comm1MinorEncoder.setAccelerationParams(5, 80, 10, 10);
  Comm2MajorEncoder.setAccelerationParams(5, 80, 3, 3);
  Comm2MinorEncoder.setAccelerationParams(5, 80, 10, 10);
  Nav1MajorEncoder.setAccelerationParams(5, 80, 3, 3);
  Nav1MinorEncoder.setAccelerationParams(5, 80, 10, 10);
  Nav2MajorEncoder.setAccelerationParams(5, 80, 3, 3);
  Nav2MinorEncoder.setAccelerationParams(5, 80, 10, 10);
  Comm1MajorEncoder.enableAcceleration(true);
  Comm1MinorEncoder.enableAcceleration(true);
  Comm2MajorEncoder.enableAcceleration(true);
  Comm2MinorEncoder.enableAcceleration(true);
  Nav1MajorEncoder.enableAcceleration(true);
  Nav1MinorEncoder.enableAcceleration(true);
  Nav2MajorEncoder.enableAcceleration(true);
  Nav2MinorEncoder.enableAcceleration(true);
  Comm1MajorEncoder.setPosition(0);
  Comm1MinorEncoder.setPosition(3);
  Comm2MajorEncoder.setPosition(0);
  Comm2MinorEncoder.setPosition(3);  
  Nav1MajorEncoder.setPosition(0);
  Nav1MinorEncoder.setPosition(3);
  Nav2MajorEncoder.setPosition(0);
  Nav2MinorEncoder.setPosition(3);

  SendDebug("Start Matrix");
  initMatrixScanner();

  // Start the no-data timeout from the END of setup(), not from reset.
  lastMSFSDataMillis = millis();
  lastLocalActivityMillis = millis();
  noDataBlanked = false;

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
      aliveudp.print("COMM_NAV");
      aliveudp.endPacket();
    }
    lastalivesent = millis();  
  }


  Comm1MajorEncoder.poll();
  Comm1MinorEncoder.poll();
  Comm2MajorEncoder.poll();
  Comm2MinorEncoder.poll();
  Nav1MajorEncoder.poll();
  Nav1MinorEncoder.poll();
  Nav2MajorEncoder.poll();
  Nav2MinorEncoder.poll();
  static int16_t LastComm1MajorEncoderPos = 0, LastComm1MinorEncoderPos = 0, lastZ = 0, lastMenu = 0;
  static int16_t LastComm2MajorEncoderPos = 0, LastComm2MinorEncoderPos = 0;
  static int16_t LastNav1MajorEncoderPos = 0, LastNav1MinorEncoderPos = 0;
  static int16_t LastNav2MajorEncoderPos = 0, LastNav2MinorEncoderPos = 0;

  int16_t Comm1MajorEncoderPos = Comm1MajorEncoder.getPosition();
  int16_t Comm1MinorEncoderPos = Comm1MinorEncoder.getPosition();
  int16_t Comm2MajorEncoderPos = Comm2MajorEncoder.getPosition();
  int16_t Comm2MinorEncoderPos = Comm2MinorEncoder.getPosition();
  int16_t Nav1MajorEncoderPos = Nav1MajorEncoder.getPosition();
  int16_t Nav1MinorEncoderPos = Nav1MinorEncoder.getPosition();
  int16_t Nav2MajorEncoderPos = Nav2MajorEncoder.getPosition();
  int16_t Nav2MinorEncoderPos = Nav2MinorEncoder.getPosition();

  // Print if any encoder changed
  if (Comm1MajorEncoderPos != LastComm1MajorEncoderPos || Comm1MinorEncoderPos != LastComm1MinorEncoderPos) {
    wakeDisplays();
    SendDebug("Comm1MajorEncoderPos:" + String(Comm1MajorEncoderPos) + " Comm1MinorEncoderPos: " + String(Comm1MinorEncoderPos));

    // Roll back to top if at deired zero point
    if (Comm1MinorEncoderPos <= 2) {
        Comm1MinorEncoder.setPosition(CommMinorEncoderMaxValue -1);
        Comm1MinorEncoderPos = Comm1MinorEncoder.getPosition();
    } else if (Comm1MinorEncoderPos >= (CommMinorEncoderMaxValue)) {
        Comm1MinorEncoder.setPosition(4);
        Comm1MinorEncoderPos = Comm1MinorEncoder.getPosition();
    }


    long convertedchann = 118000 + int(Comm1MajorEncoderPos / 3) * 1000 + int((Comm1MinorEncoderPos-3) / 3) * COM_MINOR_STEP_KHZ;
    SendDebug("Converted Chan: " + String(convertedchann));
    com1StandbyFrequency = String(convertedchann).substring(0, 3) + "." + String(convertedchann).substring(3, 6);

    sendStandbyToPC("", com1StandbyFrequency);

    updateCOMM();
    LastComm1MajorEncoderPos = Comm1MajorEncoderPos;
    LastComm1MinorEncoderPos = Comm1MinorEncoderPos;



    lastEncoderUpdate = millis();
  } // End Comm1 Processing

  if (Comm2MajorEncoderPos != LastComm2MajorEncoderPos || Comm2MinorEncoderPos != LastComm2MinorEncoderPos) {
    wakeDisplays();
    SendDebug("Comm2MajorEncoderPos:" + String(Comm2MajorEncoderPos) + " Comm2MinorEncoderPos: " + String(Comm2MinorEncoderPos));

    // Roll back to top if at deired zero point
    if (Comm2MinorEncoderPos <= 2) {
        Comm2MinorEncoder.setPosition(CommMinorEncoderMaxValue -1);
        Comm2MinorEncoderPos = Comm2MinorEncoder.getPosition();
    } else if (Comm2MinorEncoderPos >= (CommMinorEncoderMaxValue)) {
        Comm2MinorEncoder.setPosition(4);
        Comm2MinorEncoderPos = Comm2MinorEncoder.getPosition();
    }


    long convertedchann = 118000 + int(Comm2MajorEncoderPos / 3) * 1000 + int((Comm2MinorEncoderPos-3) / 3) * COM_MINOR_STEP_KHZ;
    SendDebug("Converted Chan: " + String(convertedchann));
    com2StandbyFrequency = String(convertedchann).substring(0, 3) + "." + String(convertedchann).substring(3, 6);

    sendStandbyToPC("COM2_STANDBY", com2StandbyFrequency);

    updateCOMM();
    LastComm2MajorEncoderPos = Comm2MajorEncoderPos;
    LastComm2MinorEncoderPos = Comm2MinorEncoderPos;



    lastEncoderUpdate = millis();
  }  // End Comm2 Processing

  if (Nav1MajorEncoderPos != LastNav1MajorEncoderPos || Nav1MinorEncoderPos != LastNav1MinorEncoderPos) {
    wakeDisplays();
    SendDebug("Nav1MajorEncoderPos:" + String(Nav1MajorEncoderPos) + " Nav1MinorEncoderPos: " + String(Nav1MinorEncoderPos));

    // Roll back to top if at deired zero point
    if (Nav1MinorEncoderPos <= 2) {
        Nav1MinorEncoder.setPosition(NavMinorEncoderMaxValue -1);
        Nav1MinorEncoderPos = Nav1MinorEncoder.getPosition();
    } else if (Nav1MinorEncoderPos >= (NavMinorEncoderMaxValue)) {
        Nav1MinorEncoder.setPosition(4);
        Nav1MinorEncoderPos = Nav1MinorEncoder.getPosition();
    }


    long convertedchann = 108000 + int(Nav1MajorEncoderPos / 3) * 1000 + int((Nav1MinorEncoderPos-3) / 3) * NAV_MINOR_STEP_KHZ;
    SendDebug("Converted Chan: " + String(convertedchann));
    Nav1StandbyFrequency = String(convertedchann).substring(0, 3) + "." + String(convertedchann).substring(3, 6);

    sendStandbyToPC("NAV1_STANDBY", Nav1StandbyFrequency);

    updateNAV();
    LastNav1MajorEncoderPos = Nav1MajorEncoderPos;
    LastNav1MinorEncoderPos = Nav1MinorEncoderPos;



    lastEncoderUpdate = millis();
  } // End Nav1 Processing

  if (Nav2MajorEncoderPos != LastNav2MajorEncoderPos || Nav2MinorEncoderPos != LastNav2MinorEncoderPos) {
    wakeDisplays();
    SendDebug("Nav2MajorEncoderPos:" + String(Nav2MajorEncoderPos) + " Nav2MinorEncoderPos: " + String(Nav2MinorEncoderPos));

    // Roll back to top if at deired zero point
    if (Nav2MinorEncoderPos <= 2) {
        Nav2MinorEncoder.setPosition(NavMinorEncoderMaxValue -1);
        Nav2MinorEncoderPos = Nav2MinorEncoder.getPosition();
    } else if (Nav2MinorEncoderPos >= (NavMinorEncoderMaxValue)) {
        Nav2MinorEncoder.setPosition(4);
        Nav2MinorEncoderPos = Nav2MinorEncoder.getPosition();
    }


    long convertedchann = 108000 + int(Nav2MajorEncoderPos / 3) * 1000 + int((Nav2MinorEncoderPos-3) / 3) * NAV_MINOR_STEP_KHZ;
    SendDebug("Converted Chan: " + String(convertedchann));
    Nav2StandbyFrequency = String(convertedchann).substring(0, 3) + "." + String(convertedchann).substring(3, 6);

    sendStandbyToPC("NAV2_STANDBY", Nav2StandbyFrequency);

    updateNAV();
    LastNav2MajorEncoderPos = Nav2MajorEncoderPos;
    LastNav2MinorEncoderPos = Nav2MinorEncoderPos;



    lastEncoderUpdate = millis();
  }  // End Nav2 Processing


  scanMatrix();

  // Check for MSFS Data Updates for COM Radios for staters
  MSFSpacketsize = MSFSudp.parsePacket();
  if (MSFSpacketsize > 0) {
    // SendDebug("Received a MSFS Packet");

    MSFSLen = MSFSudp.read(packetBuffer, 999);

    if (MSFSLen > 0) {
      packetBuffer[MSFSLen] = 0;
    }
    if (MSFSpacketsize) {

      lastMSFSDataMillis = millis();
      if (noDataBlanked) {
        // Data is back: restore the power state and redraw both displays.
        noDataBlanked = false;
        powerAvailable = powerBeforeTimeout;
        radioFrequencyChanged = true;
        updateNAV();
        updateCOMM();
        SendDebug("UDP data resumed - displays restored");
      }

      ProcessReceivedMSFSString();
      //SendDebug("Exiting MSFS Processing");
    }
  }

  // No-data timeout: blank both displays once, until packets resume.
  unsigned long lastActivityMillis = (lastMSFSDataMillis > lastLocalActivityMillis) ? lastMSFSDataMillis : lastLocalActivityMillis;
  if (!noDataBlanked && (millis() - lastActivityMillis) >= noDataTimeoutMs) {
    noDataBlanked = true;
    powerBeforeTimeout = powerAvailable;
    powerAvailable = false;
    updateCOMM();
    updateNAV();
    applyPanelLeds();
    SendDebug("no UDP data or local input for " + String(noDataTimeoutMs / 1000) + "s, displays blanked");
  }
}
