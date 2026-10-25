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

#define BACK_LIGHTS 5  // PCB: D5 = BACKLIGHT_PWM (330 ohm to an IRF540N that switches the AUDIO, AUTOPILOT and COMM_NAV panel backlights)

// Backlight control, the same rules as the Servo, Stepper and Dual Stepper boards. The backlight follows the main bus (BPWR:1/0 from the
// bridge: 1 = main bus 20 V or more) and the last brightness command (C,B:0-15). Until a C,B command has been received it uses
// BacklightDefaultPwm; once one has, the last commanded level is used every time power is applied. While the main bus is below 20 V the
// backlight is off. It is also off while the displays are blanked by the no-data timeout (no UDP packet and no knob or key for 30 s,
// including after power-up with no PC), and comes back at the same level when data or local input resumes.
#define BacklightDefaultPwm 65
bool backlightPowered = true;      // true until the bridge reports the main bus is down
int backlightCommandedPwm = -1;    // PWM from the last C,B command; -1 = none received yet

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

// A send to a host that does not answer ARP (switched off, unplugged) fails, and each failed send holds the sketch up for the W5500's ARP
// timeout (200 ms x 8 = 1.6 s by default, about 80 ms with the fast retransmission timeout set in setup()). So a destination that has failed is
// not tried again for a while: the sends to it are skipped until its retry time, which keeps the knobs, keys and joystick buttons responsive
// while the USB HID board or the PC is not there.
#define DEBUG_RETRY_MS 10000UL
unsigned long debugDownUntil = 0;

void SendDebug(String MessageToSend) {
  MessageToSend = BoardName + MessageToSend;
  if ((Reflector_In_Use == 1) && (Ethernet_In_Use == 1)) {
    if ((long)(millis() - debugDownUntil) < 0) return;  // the debug listener's host did not answer: skipped until the retry time
    udp.beginPacket(reflectorIP, reflectorport);
    udp.print(MessageToSend);
    if (!udp.endPacket()) debugDownUntil = millis() + DEBUG_RETRY_MS;
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

// I2C scan, sent to the debug stream (the UDP monitor on the PC). It looks at the Mega's own I2C bus and then at each of the eight TCA9548A
// channels, and lists every address that answers, for example:
//   I2C main bus: 0x70 (TCA9548A)
//   I2C TCA port 0: 0x3C (OLED)
//   I2C TCA port 1: 0x3C (OLED)
//   I2C TCA ports 2 to 7: none
// It runs once at start-up and again whenever "C,I2CSCAN:1" arrives. A scan takes a second or two, during which the knobs and keys are
// not read. A stuck bus (SDA or SCL held low) is reported instead of hanging the board.
String i2cDeviceName(uint8_t address) {
  if (address == TCAADDR) return " (TCA9548A)";
  if (address == 0x3C || address == 0x3D) return " (OLED)";
  return "";
}

// Addresses that answer on the bus as it is now, as text ("0x3C (OLED), 0x70 (TCA9548A)"), or "none"; counts them in found.
String scanI2CBus(int& found, bool skipMux) {
  String list = "";
  for (uint8_t address = 0x08; address <= 0x77; address++) {
    if (skipMux && address == TCAADDR) continue;
    Wire.beginTransmission(address);
    if (Wire.endTransmission() == 0) {
      if (list.length() > 0) list += ", ";
      list += "0x" + String(address, HEX) + i2cDeviceName(address);
      found++;
    }
    if (Wire.getWireTimeoutFlag()) return "bus timeout (SDA or SCL stuck low?)";
  }
  list.toUpperCase();
  list.replace("0X", "0x");
  return list.length() > 0 ? list : String("none");
}

void scanI2C() {
  Wire.setWireTimeout(5000, true);  // a stuck bus gives up after 5 ms instead of hanging the sketch
  Wire.clearWireTimeoutFlag();
  SendDebug("I2C scan start");

  // Main bus first, with every TCA9548A channel switched off
  Wire.beginTransmission(TCAADDR);
  Wire.write(0);
  Wire.endTransmission();
  int found = 0;
  SendDebug("I2C main bus: " + scanI2CBus(found, false));

  // Then each channel of the multiplexer in turn (the multiplexer itself is skipped)
  String noneRange = "";
  for (uint8_t port = 0; port < 8; port++) {
    tcaselect(port);
    int onPort = 0;
    String list = scanI2CBus(onPort, true);
    found += onPort;
    if (list == "none") {
      noneRange += String(port) + " ";
    } else {
      SendDebug("I2C TCA port " + String(port) + ": " + list);
    }
    Wire.clearWireTimeoutFlag();
  }
  if (noneRange.length() > 0) {
    noneRange.trim();
    SendDebug("I2C TCA ports with nothing connected: " + noneRange);
  }

  // Leave every channel off, as at start-up
  Wire.beginTransmission(TCAADDR);
  Wire.write(0);
  Wire.endTransmission();
  SendDebug("I2C scan done, " + String(found) + " device(s) answered");
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
// The DME display is the Audio panel's character OLED (the PCB net AUDIO_OLED, TCA9548A channel 3; the Auto Pilot panel's OLED is channel 2).
#define DME_OLED_Port 3

// The Auto Pilot panel's character OLED (the PCB net AP_OLED, TCA9548A channel 2) sits behind a mask, so each value goes in its own window:
//   top row     HDG at the left hand end, VS at the right hand end
//   bottom row  CRS at the left hand end, IAS in the centre, ALT at the right hand end
// (16 columns: HDG and CRS start at column 0, IAS ends at column 8 so it is centred, VS and ALT end at column 15.)
#define AP_OLED_Port 2

void placeLeft(char* line, int column, const char* text) {
  for (int i = 0; text[i] != 0 && column + i < 16; i++) line[column + i] = text[i];
}

void placeRight(char* line, int lastColumn, const char* text) {
  int start = lastColumn - (int)strlen(text) + 1;
  if (start < 0) start = 0;
  placeLeft(line, start, text);
}

// Shows the five Auto Pilot values on the panel's OLED. Values are text so they can carry leading zeros (090).
void showAutoPilot(const char* heading, const char* course, const char* airspeed, const char* verticalSpeed, const char* altitude) {
  char top[17];
  char bottom[17];
  memset(top, ' ', 16);
  memset(bottom, ' ', 16);
  top[16] = 0;
  bottom[16] = 0;
  placeLeft(top, 0, heading);
  placeRight(top, 15, verticalSpeed);
  placeLeft(bottom, 0, course);
  placeRight(bottom, 8, airspeed);
  placeRight(bottom, 15, altitude);
  tcaselect(AP_OLED_Port);
  sendCommand(0x80);
  send_string(top);
  sendCommand(cmd_NewLine);
  send_string(bottom);
}

// The values on the Auto Pilot display, as text. The bridges send them as APHDG, APCRS, APIAS, APVS and APALT in the radio data packet (heading and
// course as three digits, the others as plain numbers); the start-up values are shown until the first packet arrives.
char apHeadingText[6] = "090";
char apCourseText[6] = "110";
char apAirspeedText[6] = "80";
char apVerticalSpeedText[6] = "0";
char apAltitudeText[6] = "3200";
bool apValuesChanged = false;
bool apDrawnPower = true;  // power state the Auto Pilot display was last drawn with

// Draws the Auto Pilot display: the five values while the radio has power, blank otherwise (like the COM and NAV displays).
void updateAP() {
  apDrawnPower = powerAvailable;
  if (powerAvailable) {
    showAutoPilot(apHeadingText, apCourseText, apAirspeedText, apVerticalSpeedText, apAltitudeText);
  } else {
    tcaselect(AP_OLED_Port);
    sendCommand(cmd_CLS);
  }
}

// "APHDG", "090": returns true if the token was one of the Auto Pilot values (a value longer than 5 characters is ignored).
bool setAutoPilotValueFromToken(const String& name, String value) {
  char* destination = NULL;
  if (name == "APHDG") destination = apHeadingText;
  else if (name == "APCRS") destination = apCourseText;
  else if (name == "APIAS") destination = apAirspeedText;
  else if (name == "APVS") destination = apVerticalSpeedText;
  else if (name == "APALT") destination = apAltitudeText;
  if (destination == NULL) return false;
  value.trim();
  if (value.length() > 0 && value.length() <= 5 && strcmp(destination, value.c_str()) != 0) {
    value.toCharArray(destination, 6);
    apValuesChanged = true;
  }
  return true;
}

// Writes two lines of up to 16 characters to the DME display ("10M" and "0M" at start-up). Each line is padded with spaces so a shorter text
// replaces a longer one.
void showDme(const char* topLine, const char* bottomLine) {
  char line[17];
  tcaselect(DME_OLED_Port);
  snprintf(line, sizeof(line), "%-16s", topLine);
  sendCommand(0x80);
  send_string(line);
  snprintf(line, sizeof(line), "%-16s", bottomLine);
  sendCommand(cmd_NewLine);
  send_string(line);
}

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

void applyBacklight() {
  int pwm = 0;
  if (backlightPowered && !noDataBlanked) pwm = (backlightCommandedPwm >= 0) ? backlightCommandedPwm : BacklightDefaultPwm;
  analogWrite(BACK_LIGHTS, pwm);
}

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

// ############################################# BEGIN PANEL SWITCHES AND ENCODERS ##########################################
// The Auto Pilot and Audio panels' switches (matrix columns 1 to 4) and the Auto Pilot panel's five encoders. Each one is sent two ways:
//   1. as a joystick button on the USB HID board (UDP 172.16.1.11:4210, "index,state"), in the buttons that nothing else uses:
//      Joystick 3 (indexes 64-95) is the Auto Pilot panel and Joystick 4 (96-127) the Audio and ADF panels. Windows numbers
//      buttons from 1, so index 64 is Joystick 3 button 1.
//   2. as a command to the PC bridge (UDP 172.16.1.10:27001), where the FSUIPC and MSFS bridges turn it into a simulator control.
// The command is the name of the simulator event, sent when the switch is pressed (and once per click for an encoder, with a
// ":N" repeat count when the knob is turned quickly).
IPAddress hidIP(172, 16, 1, 11);
const unsigned int hidPort = 4210;
#define BRIDGE_COMMAND_PORT 27001
extern unsigned long matrixQuietUntil;  // start-up quiet period, defined with the matrix scanner below

struct PanelSwitch {
  uint8_t row;
  uint8_t col;
  uint8_t hidIndex;
  const char* command;
  const char* name;
};

const PanelSwitch panelSwitches[] = {
  // Auto Pilot panel (Joystick 3, buttons 1 to 8)
  { 0, 1, 64, "AP_MASTER", "AP ENGAGE" },
  { 1, 1, 65, "AP_HDG_HOLD", "AP HDG" },
  { 2, 1, 66, "AP_NAV1_HOLD", "AP CRS" },
  { 3, 1, 67, "AP_ALT_HOLD", "AP ALT" },
  { 0, 2, 68, "AP_APR_HOLD", "AP APPR" },
  { 1, 2, 69, "TOGGLE_FLIGHT_DIRECTOR", "AP FD" },
  { 2, 2, 70, "AP_BC_HOLD", "AP BC" },
  { 3, 2, 71, "AP_PANEL_SPEED_HOLD", "AP IAS" },
  // Audio panel (Joystick 4, buttons 1 to 8)
  { 0, 3, 96, "COM1_TRANSMIT_SELECT", "AUDIO COM1" },
  { 1, 3, 97, "COM_RECEIVE_ALL_TOGGLE", "AUDIO BOTH" },
  { 2, 3, 98, "RADIO_VOR2_IDENT_TOGGLE", "AUDIO NAV2" },
  { 3, 3, 99, "RADIO_DME1_IDENT_TOGGLE", "AUDIO DME" },
  { 0, 4, 100, "COM2_TRANSMIT_SELECT", "AUDIO COM2" },
  { 1, 4, 101, "RADIO_VOR1_IDENT_TOGGLE", "AUDIO NAV1" },
  { 2, 4, 102, "MARKER_SOUND_TOGGLE", "AUDIO MKR" },
  { 3, 4, 103, "RADIO_ADF_IDENT_TOGGLE", "AUDIO ADF" },
};
const int panelSwitchCount = sizeof(panelSwitches) / sizeof(panelSwitches[0]);

// Forwards a button press (1) or release (0) to the USB HID board, which drives the matching virtual joystick button.
// The USB HID board is not always connected. If a send to it fails, joystick buttons are skipped for HID_RETRY_MS, so only one press or release in
// that time pays the ARP timeout; when it answers again the buttons that are held down are sent again (hidNeedsResync).
#define HID_RETRY_MS 30000UL
unsigned long hidDownUntil = 0;
bool hidWasDown = false;
bool hidNeedsResync = false;

void sendButtonToHid(int index, int state) {
  if (Ethernet_In_Use != 1) return;
  if ((long)(millis() - hidDownUntil) < 0) return;  // the HID board did not answer: skipped until the retry time
  unsigned long started = millis();
  udp.beginPacket(hidIP, hidPort);
  udp.print(String(index) + "," + String(state));
  bool sent = udp.endPacket();
  unsigned long took = millis() - started;
  if (!sent) {
    hidDownUntil = millis() + HID_RETRY_MS;
    hidWasDown = true;
    SendDebug("USB HID board 172.16.1.11 not answering (send gave up after " + String(took) + " ms): joystick buttons paused for " + String(HID_RETRY_MS / 1000) + " s");
  } else {
    if (took > 100) SendDebug("HID send took " + String(took) + " ms");
    if (hidWasDown) {
      hidWasDown = false;
      hidNeedsResync = true;
      SendDebug("USB HID board is answering again");
    }
  }
}

// A knob click has no release, so it is sent as a short press: the release goes out from the main loop a few milliseconds later.
#define HID_PULSE_MS 40
#define HID_PULSE_SLOTS 6
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

// Sends a simulator control to the bridge ("AP_MASTER", or "HEADING_BUG_INC:5" for five clicks at once).
void sendCommandToBridge(const char* command, int count) {
  if (Ethernet_In_Use != 1) return;
  if (millis() < matrixQuietUntil) return;
  udp.beginPacket(reflectorIP, BRIDGE_COMMAND_PORT);
  udp.print(command);
  if (count > 1) {
    udp.print(":");
    udp.print(count);
  }
  udp.endPacket();
}

// Auto Pilot panel encoders. Each is kept near the middle of its range and recentred after every read, so the change since the last read is
// the distance from the middle; three counts make one click. Turning clockwise gives the first command, anticlockwise the second. If a knob
// works the wrong way, swap its A and B pins below.
#define PanelEncoderCentre 1000
struct PanelEncoder {
  RotaryEncoder* encoder;
  int16_t remainder;
  const char* clockwise;
  const char* anticlockwise;
  uint8_t hidClockwise;
  uint8_t hidAnticlockwise;
  const char* name;
};

RotaryEncoder apCrsEncoder(11, 12, 0, 2 * PanelEncoderCentre, 1);   // CRS_A D11, CRS_B D12
RotaryEncoder apAltEncoder(A4, A5, 0, 2 * PanelEncoderCentre, 1);   // ALT_A A4, ALT_B A5
RotaryEncoder apVsEncoder(46, 47, 0, 2 * PanelEncoderCentre, 1);    // VS_A D46, VS_B D47
RotaryEncoder apIasEncoder(7, 6, 0, 2 * PanelEncoderCentre, 1);     // IAS_A D7, IAS_B D6
RotaryEncoder apHdgEncoder(8, 9, 0, 2 * PanelEncoderCentre, 1);     // HDG_A D8, HDG_B D9

PanelEncoder panelEncoders[] = {
  { &apCrsEncoder, 0, "VOR1_OBI_INC", "VOR1_OBI_DEC", 72, 73, "AP CRS knob" },
  { &apAltEncoder, 0, "AP_ALT_VAR_INC", "AP_ALT_VAR_DEC", 74, 75, "AP ALT knob" },
  { &apVsEncoder, 0, "AP_VS_VAR_INC", "AP_VS_VAR_DEC", 76, 77, "AP VS knob" },
  { &apIasEncoder, 0, "AP_SPD_VAR_INC", "AP_SPD_VAR_DEC", 78, 79, "AP IAS knob" },
  { &apHdgEncoder, 0, "HEADING_BUG_INC", "HEADING_BUG_DEC", 80, 81, "AP HDG knob" },
};
const int panelEncoderCount = sizeof(panelEncoders) / sizeof(panelEncoders[0]);

void initPanelEncoders() {
  for (int i = 0; i < panelEncoderCount; i++) {
    RotaryEncoder* e = panelEncoders[i].encoder;
    e->begin();
    e->setAccelerationParams(5, 80, 5, 8);
    e->enableAcceleration(true);
    e->setPosition(PanelEncoderCentre);
  }
}

void pollPanelEncoders() {
  for (int i = 0; i < panelEncoderCount; i++) {
    RotaryEncoder* e = panelEncoders[i].encoder;
    e->poll();
    int16_t delta = e->getPosition() - PanelEncoderCentre;
    if (delta == 0) continue;
    e->setPosition(PanelEncoderCentre);
    panelEncoders[i].remainder += delta;
    int clicks = panelEncoders[i].remainder / ClicksPerIncrement;
    panelEncoders[i].remainder -= clicks * ClicksPerIncrement;
    if (clicks == 0) continue;
    if (millis() < matrixQuietUntil) continue;  // knob positions at start-up are not input
    wakeDisplays();
    bool clockwise = clicks > 0;
    int count = abs(clicks);
    if (count > 50) count = 50;
    SendDebug(String(panelEncoders[i].name) + (clockwise ? " clockwise " : " anticlockwise ") + String(count));
    sendCommandToBridge(clockwise ? panelEncoders[i].clockwise : panelEncoders[i].anticlockwise, count);
    pulseButtonToHid(clockwise ? panelEncoders[i].hidClockwise : panelEncoders[i].hidAnticlockwise);
  }
}

// A switch on the Auto Pilot or Audio panel (matrix column 1 to 4) was pressed (true) or released (false). Returns true if it was one of them.
bool handlePanelSwitch(int row, int col, bool pressed) {
  for (int i = 0; i < panelSwitchCount; i++) {
    if (panelSwitches[i].row == row && panelSwitches[i].col == col) {
      SendDebug(String(panelSwitches[i].name) + (pressed ? " pressed" : " released") + " (joystick button index " + String(panelSwitches[i].hidIndex) + ")");
      sendButtonToHid(panelSwitches[i].hidIndex, pressed ? 1 : 0);
      if (pressed) sendCommandToBridge(panelSwitches[i].command, 1);
      return true;
    }
  }
  return false;
}
// ############################################# END PANEL SWITCHES AND ENCODERS ##########################################

// ############################################# BEGIN MATRIX SCANNER  ##########################################
// Matrix Scanner with Debounce
// Rows (ROW0-ROW3): D22-D25, columns (COL0-COL4): D26, D27, D28, D29 and D48. Each switch has a diode to its column.
//   COL0  COM and NAV panel   (four frequency swap switches)
//   COL1  Auto Pilot panel    (ENGAGE, HDG, CRS, ALT)       COL2  Auto Pilot panel (APPR, FD, BC, IAS)
//   COL3  Audio panel         (COM1, BOTH, NAV2, DME)       COL4  Audio panel      (COM2, NAV1, MKR, ADF)

// Pin definitions
const int rowPins[4] = { 22, 23, 24, 25 };      // Row pins D22-D25
const int colPins[5] = { 26, 27, 28, 29, 48 };  // Column pins (COL4 is D48 on the Radio Auto Pilot PCB)

// Matrix configuration
const int ROWS = 4;
const int COLS = 5;

// Debounce settings
// Debounce by integration: each key is sampled once per millisecond and a counter moves one step towards 0 (key down) or
// MATRIX_INTEGRATOR_MAX (key up) with every sample. A key changes state only when its counter reaches an end, so a lone noisy reading
// (the MAX7219 LED multiplexing runs next to the column lines in the panel cables) cannot hold a release back or make a false press,
// and a switch is reported about MATRIX_INTEGRATOR_MAX ms after it changes. The old scheme restarted a 50 ms timer on every reading that
// differed from the last one, so a release could be held off for as long as the noise lasted.
#define MATRIX_INTEGRATOR_MAX 12
uint8_t keyIntegrator[ROWS][COLS];  // 0 = down, MATRIX_INTEGRATOR_MAX = up
unsigned long lastMatrixSampleMs = 0;
bool buttonState[ROWS][COLS];                // Current state of each button
bool keyPressed[ROWS][COLS];                 // Tracks if key is currently pressed

// Optional: Define key mapping (customize as needed)
char keyMap[ROWS][COLS] = {
  { '1', '2', '3', 'A', 'E' },
  { '4', '5', '6', 'B', 'F' },
  { '7', '8', '9', 'C', 'G' },
  { '*', '0', '#', 'D', 'H' }
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
      keyIntegrator[row][col] = MATRIX_INTEGRATOR_MAX;
      keyPressed[row][col] = false;
    }
  }
}

void scanMatrix() {
  // One sample of every key per millisecond (the loop runs much faster than that)
  unsigned long nowMs = millis();
  if (nowMs == lastMatrixSampleMs) return;
  lastMatrixSampleMs = nowMs;

  for (int row = 0; row < ROWS; row++) {
    // Set current row LOW, others HIGH
    for (int i = 0; i < ROWS; i++) {
      digitalWrite(rowPins[i], (i == row) ? LOW : HIGH);
    }

    // Allow the row and the long panel cables to settle
    delayMicroseconds(20);

    // Read all columns for this row
    for (int col = 0; col < COLS; col++) {
      // Two readings a few microseconds apart: if they disagree this is a noise spike and the sample is skipped
      bool reading = digitalRead(colPins[col]);
      delayMicroseconds(3);
      if (digitalRead(colPins[col]) != reading) continue;

      // Move the integrator one step towards the reading (LOW = pressed because of the pull-up)
      if (reading == LOW) {
        if (keyIntegrator[row][col] > 0) keyIntegrator[row][col]--;
      } else {
        if (keyIntegrator[row][col] < MATRIX_INTEGRATOR_MAX) keyIntegrator[row][col]++;
      }

      bool newState = buttonState[row][col];
      if (keyIntegrator[row][col] == 0) newState = LOW;
      else if (keyIntegrator[row][col] == MATRIX_INTEGRATOR_MAX) newState = HIGH;

      if (newState != buttonState[row][col]) {
        buttonState[row][col] = newState;

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
    updateAP();
    applyBacklight();
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
  String line = "Matrix raw idle (all rows HIGH), cols 0-4: ";
  for (int c = 0; c < COLS; c++) line += (digitalRead(colPins[c]) ? "H" : "L");
  SendDebug(line);
  for (int r = 0; r < ROWS; r++) {
    for (int i = 0; i < ROWS; i++) digitalWrite(rowPins[i], (i == r) ? LOW : HIGH);
    delayMicroseconds(50);
    String rl = "Matrix raw row " + String(r) + " LOW, cols 0-4: ";
    for (int c = 0; c < COLS; c++) rl += (digitalRead(colPins[c]) ? "H" : "L");
    SendDebug(rl);
  }
  for (int i = 0; i < ROWS; i++) digitalWrite(rowPins[i], HIGH);
}

// After the HID board has been missing, the switches that are held down are pressed again on it (called from loop()).
void resyncHidButtons() {
  for (int i = 0; i < panelSwitchCount; i++) {
    if (keyPressed[panelSwitches[i].row][panelSwitches[i].col]) sendButtonToHid(panelSwitches[i].hidIndex, 1);
  }
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
  if (col >= 1 && handlePanelSwitch(row, col, true)) return;  // Auto Pilot and Audio panels
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
  if (col >= 1) handlePanelSwitch(row, col, false);  // releases the joystick button of an Auto Pilot or Audio panel switch
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
    if (apValuesChanged || apDrawnPower != powerAvailable) {  // new Auto Pilot values, or the radio power changed
      apValuesChanged = false;
      updateAP();
    }
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

  // Backlight power from the bridge: 1 = main bus 20 V or more, 0 = off. See applyBacklight().
  if (str.startsWith("BPWR:")) {
    backlightPowered = (str.substring(5).toInt() != 0);
    applyBacklight();
    return;
  }

  // Auto Pilot display values ("APHDG:090" ...): not logged one by one either.
  if (str.startsWith("AP")) {
    int apColon = str.indexOf(':');
    if (apColon > 0 && setAutoPilotValueFromToken(str.substring(0, apColon), str.substring(apColon + 1))) return;
  }

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
        backlightCommandedPwm = map(isetting, 0, 15, 0, 255);
        applyBacklight();
        setPanelLedBrightness(isetting);
        SendDebug("Brightness " + String(isetting) + " (backlight PWM " + String(backlightCommandedPwm) + ", panel LEDs)");
      }
    //else if (Debug_Display || bLocalDebug ) SendDebug("Invalid Brightness value passed. Value is :" + String(setting));

    // Remote reset: C,RESET:1
    if (command == "RESET") RebootBoard();

    // Matrix diagnostic: C,MATRIX:1 (see reportMatrixRaw())
    if (command == "MATRIX") reportMatrixRaw();

    // I2C scan: C,I2CSCAN:1 sends what answers on the I2C bus and behind each TCA9548A channel to the debug monitor (see scanI2C())
    if (command == "I2CSCAN") scanI2C();

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


// The 5 V rail (the ADC's AVcc reference) in volts, measured against the internal 1.1 V band-gap, as text for the heartbeat: " V5:5.02". The
// JetRangerHealthMonitor matches heartbeats by their first word, so the voltage after it does not change which board it is, and the monitor shows the
// last reading from each board.
String vccHeartbeatText() {
  ADMUX = _BV(REFS0) | _BV(MUX4) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
  ADCSRB &= ~_BV(MUX5);
  delay(2);  // let the band-gap settle after the reference change
  long sum = 0;
  for (int i = 0; i < 9; i++) {
    ADCSRA |= _BV(ADSC);
    while (bit_is_set(ADCSRA, ADSC)) {}
    if (i > 0) sum += ADC;  // first conversion thrown away
  }
  long reading = sum / 8;
  if (reading <= 0) return "";
  return " V5:" + String(1125.3f / reading, 2);
}

void setup() {

  pinMode(GREEN_STATUS_LED_PORT, OUTPUT);
  pinMode(RED_STATUS_LED_PORT, OUTPUT);
  digitalWrite(GREEN_STATUS_LED_PORT, true);
  digitalWrite(RED_STATUS_LED_PORT, false);
  pinMode(BACK_LIGHTS, OUTPUT);
  applyBacklight();  // the default level until a brightness command or the main bus says otherwise

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
  scanI2C();  // what is on the I2C bus and behind each multiplexer channel, to the debug monitor




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
  initCharOLED(DME_OLED_Port);
  showDme("10M", "0M");  // start-up text on the DME display
  initCharOLED(AP_OLED_Port);
  showAutoPilot("090", "110", "80", "0", "3200");  // start-up values: heading 090, course 110, airspeed 80, vertical speed 0, altitude 3200
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
  initPanelEncoders();

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
      aliveudp.print(vccHeartbeatText());
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
  pollPanelEncoders();
  serviceHidPulses();
  if (hidNeedsResync) {
    hidNeedsResync = false;
    resyncHidButtons();
  }

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
        updateAP();
        applyBacklight();
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
    updateAP();
    applyBacklight();
    SendDebug("no UDP data or local input for " + String(noDataTimeoutMs / 1000) + "s, displays blanked");
  }
}
