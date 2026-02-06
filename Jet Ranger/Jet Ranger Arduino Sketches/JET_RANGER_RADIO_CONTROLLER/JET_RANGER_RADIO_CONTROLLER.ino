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
String com1ActiveFrequency = "119.000";
String com2ActiveFrequency = "120.000";
String com1StandbyFrequency = "121.000";
String com2StandbyFrequency = "122.000";

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
const unsigned int reflectorport = 27000;


int packetSize;
int debugLen;
EthernetUDP udp;
EthernetUDP debugUDP;

int MSFSpacketsize;
int MSFSLen;

EthernetUDP max7219udp;  // Max7219
EthernetUDP MSFSudp;     // Listens to MSFS light commands

#define EthernetStartupDelay 500
#define ES1_RESET_PIN 53

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

void updateNAV(int Channel) {
  tcaselect(NAV_OLED_Port);
  sendCommand(cmd_CLS);
  String workstring = "Channel " + String(Channel);
  send_string(workstring.c_str());
}

void updateCOMM() {
  tcaselect(COMM_OLED_Port);
  sendCommand(0x80);
  //sendCommand(cmd_CLS);
  String workstring1 = com1ActiveFrequency + "  " + com2ActiveFrequency;
  String workstring2 = com1StandbyFrequency + "  " + com2StandbyFrequency;

  send_string(workstring1.c_str());
  sendCommand(cmd_NewLine);
  send_string(workstring2.c_str());
}

// ############################################# END CHARACTER OLED ##########################################

// ############################################# BEGIN ENCODER ##########################################


// Can use ANY pins on Mega (no interrupt limitation!)
// Create 4 encoders for X, Y, Z, and Menu control
RotaryEncoder encoderX(36, 37, 0, 1000);
RotaryEncoder encoderY(38, 39, 0, 1000);
// RotaryEncoder encoderZ(6, 7, -1000, 1000);
// RotaryEncoder encoderMenu(8, 9, 0, 10);

// ############################################# END ENCODER  ##########################################

// ############################################# BEGIN MATRIX SCANNER  ##########################################
// Matrix Keypad Scanner with Debounce
// Rows: D22-D25, Columns: D32-D35

// Pin definitions
const int rowPins[4] = { 22, 23, 24, 25 };  // Row pins D22-D25
const int colPins[4] = { 32, 33, 34, 35 };  // Column pins D32-D35

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

void initMatrixScanner() {
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

          // Key press detection (LOW = pressed due to pull-up)
          if (buttonState[row][col] == LOW && !keyPressed[row][col]) {
            // Key was just pressed
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

// Callback function for key press events
void onKeyPress(int row, int col) {
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
      radioFrequencyChanged = false;
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
        SendDebug("COM1 Frequency changed");
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
    }
  }
  return;
}


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

    // Backlighting and Flood ligghting
    if (command == "B")
      if (isetting >= 0 && isetting <= 15) {
        //analogWrite(FLOOD_LIGHTS, map(isetting, 0, 15, 0, 255));
      }
    //else if (Debug_Display || bLocalDebug ) SendDebug("Invalid Brightness value passed. Value is :" + String(setting));
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
    udp.begin(localport);
    debugUDP.begin(localdebugport);
    MSFSudp.begin(MSFSport);

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
  delay(1000);
  // updateNAV(100);
  //updateCOMM( "118.50", "119.50", "120.50", "121.50");

  SendDebug("Start Encoders");
  encoderX.begin();
  encoderY.begin();
  // Configure each encoder
  encoderX.setAccelerationParams(5, 80, 3, 3);
  // encoderY.setAccelerationParams(5, 80, 3, 3);
  encoderX.enableAcceleration(true);
  // encoderY.enableAcceleration(true);

  SendDebug("Start Matrix");
  initMatrixScanner();

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

  encoderX.poll();
  encoderY.poll();
  static int16_t lastX = 0, lastY = 0, lastZ = 0, lastMenu = 0;

  int16_t x = encoderX.getPosition();
  int16_t y = encoderY.getPosition();



  // Print if any encoder changed
  if (x != lastX || y != lastY) {
    SendDebug("X:" + String(x) + " Y: " + String(y));

    long convertedchann = int(x / 3) * 50 + 118000;
    SendDebug("Converted Chan: " + String(convertedchann));
    com1StandbyFrequency = String(convertedchann).substring(0, 3) + "." + String(convertedchann).substring(3, 6);

    if (Ethernet_In_Use == 1) {
      udp.beginPacket(reflectorIP, 27001);
      udp.print(com1StandbyFrequency);
      udp.endPacket();
    }

    updateCOMM();
    lastX = x;
    lastY = y;
    lastEncoderUpdate = millis();
  }

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

      ProcessReceivedMSFSString();
      //SendDebug("Exiting MSFS Processing");
    }
  }
}
