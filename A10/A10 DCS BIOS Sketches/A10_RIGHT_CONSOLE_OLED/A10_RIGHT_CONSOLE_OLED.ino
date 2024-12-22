/*
A10_RIGHT_CONSOLE_OLED

PLACEHOLDER UNTIL COMBINED PCB ARRIVES THEN MOVE TO RIGHT CONSOLE OUTPUT

Drives
CMSP_OLED_PORT 0
FUEL_OLED_Port 1
ILS_OLED_Port 2
TACAN_OLED_Port 3


*/
// *************************************************************
// *************************************************************
// Board MEGA 256
// SN -
// Location
// Left Console
// *************************************************************
// *************************************************************
// Current COM 7 - Check S/N First - Before any new Uploads
// *************************************************************
// *************************************************************
//

// I2C Addresses
// I2C 60 for OLED
// I2C 112 for TCA9548A


// Basic logic
// Initialise I2C Bus (wire)
// Iterate through each port on the Mux and list connected devices to serial port (simple troubleshooting aid), may use UDP later
// Initialise each display

#define Ethernet_In_Use 1
#define DCSBIOS_In_Use 1
#define Reflector_In_Use 1


#define CMSP_OLED_PORT 0
#define FUEL_OLED_Port 1
#define ILS_OLED_Port 2
#define TACAN_OLED_Port 3

int lastFuel100s = 0;
int lastFuel1000s = 0;
String CMSP1 = "";
String CMSP2 = "";


#define DCSBIOS_IRQ_SERIAL
#include "DcsBios.h"


#include <SPI.h>
#include <Wire.h>
#include <Ethernet.h>
#include <EthernetUdp.h>

#define EthernetStartupDelay 500
#define ES1_RESET_PIN 53

String BoardName = "A10 RIGHT OLED";
// These local Mac and IP Address will be reassigned early in startup based on
// the device ID as set by address pins
byte mac[] = { 0xA8, 0x61, 0x0A, 0x67, 0x83, 0x09 };
String sMac = "A8:61:0A:67:83:09";
IPAddress ip(172, 16, 1, 109);
String strMyIP = "172.16.1.109";

// Raspberry Pi is Target
IPAddress targetIP(172, 16, 1, 2);
String strTargetIP = "X.X.X.X";

// Reflector
IPAddress reflectorIP(172, 16, 1, 10);
String strReflectorIP = "X.X.X.X";

const unsigned int localport = 7788;
const unsigned int remoteport = 26027;
const unsigned int reflectorport = 27000;

EthernetUDP udp;
char packetBuffer[1000];     //buffer to store the incoming data
char outpacketBuffer[1000];  //buffer to store the outgoing data
const unsigned long delayBeforeSendingPacket = 2000;
unsigned long ethernetStartTime = 0;

#define RED_STATUS_LED_PORT 12
#define GREEN_STATUS_LED_PORT 13
#define STATUS_LED_PORT LED_BUILTIN
#define RED_STATUS_LED_PORT 12
#define GREEN_STATUS_LED_PORT 13
#define Check_LED_R 12
#define Check_LED_G 13

#define FLASH_TIME 300

unsigned long NEXT_STATUS_TOGGLE_TIMER = 0;
bool RED_LED_STATE = false;
unsigned long timeSinceRedLedChanged = 0;

String sAircraftName = "";

extern "C" {
#include "utility/twi.h"  // from Wire library, so we can do bus scanning
}

#define TCAADDR 0x70
//#define TCAADDR 0x112


int CurrentDisplay = 0;
int Brightness = 0;
char buffer[20];  //plenty of space for the value of millis() plus a zero terminator


void tcaselect(uint8_t i) {
  if (i > 7) return;

  Wire.beginTransmission(TCAADDR);
  Wire.write(1 << i);
  Wire.endTransmission();
}

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

void initCharOLED(int PortNo) {
  // *** I2C initial *** //
  tcaselect(PortNo);
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

  send_string("Character OLED");
  sendCommand(cmd_NewLine);
  String wrkstr = "TEST " + String(PortNo);
  send_string(wrkstr.c_str());
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
                           //delay(10);
}

void send_string(const char* String) {
  unsigned char i = 0;
  while (String[i]) {
    sendData(String[i]);  // *** Show String to OLED
    i++;
  }
}


// ############################################# BEGIN CHARACTER OLED ##########################################



void SendDebug(String MessageToSend) {
  if ((Reflector_In_Use == 1) && (Ethernet_In_Use == 1)) {
    udp.beginPacket(reflectorIP, reflectorport);
    udp.print(MessageToSend);
    udp.endPacket();
  }
}

void setup() {
  pinMode(GREEN_STATUS_LED_PORT, OUTPUT);
  pinMode(RED_STATUS_LED_PORT, OUTPUT);
  digitalWrite(GREEN_STATUS_LED_PORT, true);
  digitalWrite(RED_STATUS_LED_PORT, true);
  delay(FLASH_TIME);
  digitalWrite(GREEN_STATUS_LED_PORT, false);
  digitalWrite(RED_STATUS_LED_PORT, false);
  delay(FLASH_TIME);

  if (Ethernet_In_Use == 1) {

    // Reset Ethernet Module
    pinMode(ES1_RESET_PIN, OUTPUT);
    digitalWrite(ES1_RESET_PIN, LOW);
    delay(2);
    digitalWrite(ES1_RESET_PIN, HIGH);

    Ethernet.begin(mac, ip);
    udp.begin(localport);

    // As it takes a couple of seconds before the Ethernet Stack is operational
    // Flash leds until time period has completed
    ethernetStartTime = millis() + delayBeforeSendingPacket;
    while (millis() <= ethernetStartTime) {
      delay(FLASH_TIME);
      digitalWrite(Check_LED_G, false);
      delay(FLASH_TIME);
      digitalWrite(Check_LED_G, true);
    }

    SendDebug(BoardName + " Ethernet Started " + strMyIP + " " + sMac);
  }


  Wire.begin();


  for (uint8_t t = 0; t < 8; t++) {
    tcaselect(t);
    // Had to comment out these debugging messages as they created a conflict with the IRQ definition in DCS BIOS
    SendDebug("TCA Port #" + String(t));

    for (uint8_t addr = 0; addr <= 127; addr++) {
      uint8_t data;
      if (!twi_writeTo(addr, &data, 0, 1, 1)) {
        SendDebug("Found I2C " + String(addr));
      }
    }
  }
  // Had to comment out these debugging messages as they created a conflict with the IRQ definition in DCS BIOS
  SendDebug("I2C scan complete");

  for (int oledptr = 0; oledptr < 8; oledptr++) {
    initCharOLED(oledptr);
  }

  delay(500);

  tcaselect(CMSP_OLED_PORT);
  sendCommand(0x01);
  // DEL - CLS
  sendCommand(0x80);  // Home Pos

  send_string("OFF RDY RDY RDY");
  sendCommand(cmd_NewLine);
  send_string("MWS JMR RWR DISP");
  //send_string("999 999 999 999");
  tcaselect(FUEL_OLED_Port);
  OLED_CLS();
  send_string("   00000");
  tcaselect(ILS_OLED_Port);
  OLED_CLS();
  send_string("     108.95");
  tcaselect(TACAN_OLED_Port);
  OLED_CLS();

  send_string("     067X");


  if (DCSBIOS_In_Use == 1) DcsBios::setup();
}


void setContrast(int contr) {
  //From https://www.youtube.com/watch?v=hFpXfSnDNSY
  //
  // for Adafruit_SSD1306.h library
  // contr values from 1 to 411
  // for i2c and 3.3V VCC works fine

  // Sample calling code

  //    randomSeed(analogRead(0));
  //    Brightness = random(254);
  //    if (Brightness > 120)
  //      setContrast(411);
  //    else
  //      setContrast(1);


  int prech;
  int brigh;
  switch (contr) {
    case 001 ... 255:
      prech = 0;
      brigh = contr;
      break;
    case 256 ... 411:
      prech = 16;
      brigh = contr - 156;
      break;
    default:
      prech = 16;
      brigh = 255;
      break;
  }
}




// ****************** BEGIN A10 ************************************** //

void onAcftNameChange(char* newValue) {
  sAircraftName = String(newValue);
  SendDebug("Aircraft Name : " + sAircraftName);
}
DcsBios::StringBuffer<24> AcftNameBuffer(0x0000, onAcftNameChange);

void onCmscTxtChaffFlareChange(char* newValue) {
  tcaselect(0);
  // txtChaffFlare = String(newValue);
  //updateCMSC();
}
DcsBios::StringBuffer<8> cmscTxtChaffFlareBuffer(0x108e, onCmscTxtChaffFlareChange);

// #define CMSP_OLED_PORT 0
// #define FUEL_OLED_Port 1
// #define ILS_OLED_Port 2
// #define TACAN_OLED_Port 3

void OLED_CLS() {
  sendCommand(0x01);
  // DEL - CLS
  sendCommand(0x80);  // Home Pos
  // DEL - CLS
  sendCommand(0xC0);
}

void onTacanChannelChange(char* newValue) {
  tcaselect(TACAN_OLED_Port);
  OLED_CLS();
  String wrkstr = "     " + String(newValue);
  send_string(wrkstr.c_str());
}
DcsBios::StringBuffer<4> tacanChannelBuffer(0x1162, onTacanChannelChange);


void onIlsFrequencySChange(char* newValue) {
  SendDebug("ILS Channel Change");
  tcaselect(ILS_OLED_Port);
  OLED_CLS();
  String wrkstr = "     " + String(newValue);
  send_string(wrkstr.c_str());
}
DcsBios::StringBuffer<6> ilsFrequencySBuffer(0x135a, onIlsFrequencySChange);


void updateFuelDisplay() {
  String wrkstr = "";
  tcaselect(FUEL_OLED_Port);
  OLED_CLS();
  if (lastFuel1000s > 9) {
    wrkstr = "   " + String(String(lastFuel1000s) + String(lastFuel100s) + "00");
  } else {
    wrkstr = "   0" + String(String(lastFuel1000s) + String(lastFuel100s) + "00");
  }
  send_string(wrkstr.c_str());
}


void onFuelQty100Change(unsigned int newValue) {
  String wrkstr = String(newValue);
  SendDebug("Fuel Quantity 100s " + wrkstr);
  float flt100s = float(newValue);
  int int100s = int(flt100s * 10 / 0xFFFF);
  if (lastFuel100s != int100s) {
    lastFuel100s = int100s;
    updateFuelDisplay();
  }
  SendDebug("Converted Fuel Quantity 100s " + String(int100s));
}
DcsBios::IntegerBuffer fuelQty100Buffer(0x10d2, 0xffff, 0, onFuelQty100Change);


void onFuelQty1000Change(unsigned int newValue) {
  String wrkstr = String(newValue);
  SendDebug("Fuel Quantity 1000s " + wrkstr);
  float flt1000s = float(newValue);
  int int1000s = int(flt1000s * 10 / 0xFFFF);
  if (lastFuel1000s != int1000s) {
    lastFuel1000s = int1000s;
    updateFuelDisplay();
  }
  SendDebug("Converted Fuel Quantity 100s " + String(int1000s));
}
DcsBios::IntegerBuffer fuelQty1000Buffer(0x10d0, 0xffff, 0, onFuelQty1000Change);


void updateCMSPDisplay() {

  tcaselect(CMSP_OLED_PORT);
  sendCommand(0x80);
  send_string(CMSP1.c_str());
  sendCommand(cmd_NewLine);
  send_string(CMSP2.c_str());
}

void onCmsp1Change(char* newValue) {
  CMSP1 = String(newValue);
  updateCMSPDisplay();
}
DcsBios::StringBuffer<19> cmsp1Buffer(0x1000, onCmsp1Change);

void onCmsp2Change(char* newValue) {
  CMSP2 = String(newValue);
  updateCMSPDisplay();
}
DcsBios::StringBuffer<19> cmsp2Buffer(0x1014, onCmsp2Change);



void loop() {


  if (millis() >= NEXT_STATUS_TOGGLE_TIMER) {
    RED_LED_STATE = !RED_LED_STATE;

    digitalWrite(Check_LED_G, RED_LED_STATE);
    digitalWrite(Check_LED_R, !RED_LED_STATE);
    NEXT_STATUS_TOGGLE_TIMER = millis() + FLASH_TIME;
  }

  if (DCSBIOS_In_Use == 1) DcsBios::loop();
}
