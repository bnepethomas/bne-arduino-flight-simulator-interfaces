/*  A10_FRONT_CONSOLE_OLED

Drives
BARO_OLED_Port 1
ALT_OLED_Port 2
CMSC_OLED_Port 3

O_RIGHT_GEAR_LED 2
O_NOSE_GEAR_LED 3
O_LEFT_GEAR_LED 4
O_VHF_AM_LED 5
O_VHF_FM_LED 6
O_AOA_BELOW_LED 9
O_AOA_ON_LED 8
O_AOA_ABOVE_LED 7

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
//
// As a number of the same OLEDs are used, which the same target I2C addresses an I2C multiplexor is used, TCA9548A.
// The TCA9548A is an 8 Channel I2C switch.  It is possible for different devices to share a common host I2C bus.
// https://e2e.ti.com/blogs_/b/analogwire/archive/2015/10/15/how-to-simplify-i2c-tree-when-connecting-multiple-slaves-to-an-i2c-master
//
//
// The initial test OLEDs have addresses of 0x3C - despite being labelled - 0x78 or 0x7A - selectable by soldering a jumper.
// Can validate what addresses are on the bus by using I2C scanner

// The following OLEDs where sourced from ebay
// OLED for top  2.2" 128 * 32 SSD1305 (not 1306)
// This OLED does require resistors to be set (R3 & R5 Short)(the others open)
// Pin 16 tied to specific reset for module (ie not general Arduino reset
// It does support 3.3V, but works ok on 5V
// 1 Gnd
// 2 +5V
// 4 Gnd
// 7 SCL
// 8 & 9 SDA
// 16 Reset

/* PCB CONNECTIONS
    I2C - 0 = 
    I2C - 1 = ALT
    I2C - 2 = ALT
    I2C - 3 = MWS Character OLED
    I2C - 4 = 
    I2C - 5 = 
    I2C - 6 = 
    I2C - 7 = 
*/
// NBeed to ground unused pins otherwise random stuff happens - see the following
// https://www.buydisplay.com/arduino/Interfacing_ER-OLEDM023-1_to_Arduino.pdf

// Useful reference for troubleshooting OLEDs

// Finding a matching class in UG82 can be trail and error for non name brand OLEDs
// Start with short text say at x=8 y=8
// Once that is displaying sanely then try drawing a box from 0,0 to the full area
// Commonly find classes truncate at 8 or 16 rows
// Also if no change is appearing power cycle as you may no longer be on correct I2C address
// Search in the U8g2lib.h using the oled chip name and its resolution
// noting the entries with 2nd I2C are probably for the alternate I2C address
// the SW I2C entries use a software driver so ignore those unless really needed
// Monitor the I2C initalisation tests to validate OLED is present

// OLED for 5 Right hand side digits 0.91" 128*32 SSD1306
// https://www.ebay.com/itm/0-91-Inch-128x32-IIC-I2c-White-Blue-OLED-LCD-Display-Module-3-3-5v-For-Arduino/392552169768?ssPageName=STRK%3AMEBIDX%3AIT&var=661536491479&_trksid=p2057872.m2749.l2649


//  OLED for Small Digits 0.66" SSD1306 64 * 48
// https://www.ebay.com/itm/0-66-inch-White-64x48-OLED-Display-Module-SPI-I2C-3-3-5V-for-Arduino-AVR-STM32/192332990775?ssPageName=STRK%3AMEBIDX%3AIT&_trksid=p2057872.m2749.l2649
// I2C 0x3C - validate by running scanner

// When using U8G2 library use this constructor for 64x48 display (assuming pin is used for reset which will need to
// change with NW shield is attached
// U8G2_SSD1306_64X48_ER_1_HW_I2C u8g2(U8G2_R0, /* reset=*/ 4);
//
//
// When using a 2.42 inch SSD1309 (128*64) this is used U8G2_SSD1309_128X64_NONAME2_F_HW_I2C
// other SSD1309 classes only displayed down to row 8 or 16
// https://www.ebay.com.au/itm/355105747286 White I2C
// Port 2 VHF_AM OLED
//
//The data format of U8G2 fonts is based on the BDF font format. Its glyph bitmaps are compressed with a
//run-length-encoding algorithm and its header data are designed with variable bit field width to
//minimize flash memory footprint.

//http://oleddisplay.squix.ch/#/home
//<3.0.0 is Thiele with packed bitmaps (and special gotcha)
//>=3.0.0 has a Jump table with aligned bitmaps (and really special gotcha)
//Adafruit_GFX has missing bitmap and glyph entry for 0x7E (tilde)

// https://rop.nl/truetype2gfx/

// FontForge
// https://learn.adafruit.com/custom-fonts-for-pyportal-circuitpython-display/conversion

// I2C Addresses
// I2C 60 for OLED
// I2C 112 for TCA9548A

// Premade 4 way JST Colour code
// 1: Black +V
// 2: Green GND
// 3: White SCL
// 4: Red   SDA


// Basic logic
// Initialise I2C Bus (wire)
// Iterate through each port on the Mux and list connected devices to serial port (simple troubleshooting aid), may use UDP later
// Initialise each display
// Wait for UDP updates
// during timeout grab and smooth analo 0 and get brightness for all displays

#define Ethernet_In_Use 1
#define DCSBIOS_In_Use 1
#define Reflector_In_Use 1


// Port 3 is used for either a channel or Scratchpad display
//#define Opt_OLED_Port_1 6
//#define Opt_OLED_Port_2 4
//#define Opt_OLED_Port_3 5
//#define Opt_OLED_Port_4 2
//#define Opt_OLED_Port_5 7

#define BARO_OLED_Port 1
#define ALT_OLED_Port 2
#define CMSC_OLED_Port 3


#define Opt_OLED_Port_1 3
#define Opt_OLED_Port_2 4
#define Opt_OLED_Port_3 5
#define Opt_OLED_Port_4 6
#define Opt_OLED_Port_5 7
#define COM1_OLED_PORT 0
#define COM2_OLED_PORT 1
#define ScratchPad_OLED_PORT 2

#define ScratchPad_Vertical_Pos 30
#define ScratchPad_String1_Pos 0
#define ScratchPad_String2_Pos 35
#define ScratchPad_Number_Pos 45

#define COM1_XPOS 20
#define COM1_YPOS 33
#define COM2_XPOS 20
#define COM2_YPOS 33


#define DCSBIOS_IRQ_SERIAL
#include "DcsBios.h"

//#include <Arduino.h>
#include <U8g2lib.h>
#include "hornet_font.h"


#include <SPI.h>
#include <Wire.h>
#include <Ethernet.h>
#include <EthernetUdp.h>

#define EthernetStartupDelay 500
#define ES1_RESET_PIN 53

String BoardName = "A10 Forward OLED";
// These local Mac and IP Address will be reassigned early in startup based on
// the device ID as set by address pins
byte mac[] = { 0xA8, 0x61, 0x0A, 0x67, 0x83, 0x03 };
String sMac = "A8:61:0A:67:83:03";
IPAddress ip(172, 16, 1, 103);
String strMyIP = "172.16.1.103";

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



#define O_RIGHT_GEAR_LED 2
#define O_NOSE_GEAR_LED 3
#define O_LEFT_GEAR_LED 4
#define O_VHF_AM_LED 5
#define O_VHF_FM_LED 6
#define O_AOA_BELOW_LED 9
#define O_AOA_ON_LED 8
#define O_AOA_ABOVE_LED 7



String sAircraftName = "";

// Scratch Pad OLED
U8G2_SSD1305_128X32_ADAFRUIT_F_HW_I2C u8g2_Scratch_Pad(U8G2_R2, 12);

// Op OLEDs
U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C u8g2_ALT(U8G2_R0, /* reset=*/U8X8_PIN_NONE);

// Working 2.42" Inch OLED for A10
// U8G2_SSD1309_128X64_NONAME2_F_HW_I2C u8g2_BARO(U8G2_R0, U8X8_PIN_NONE);

// Working 0.96" inch OLED display
U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C u8g2_BARO(U8G2_R0, U8X8_PIN_NONE);
//U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2_BARO(U8G2_R0, U8X8_PIN_NONE);


//U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C u8g2_BARO(U8G2_R0, /* reset=*/U8X8_PIN_NONE);
U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C u8g2_OPT3(U8G2_R0, /* reset=*/U8X8_PIN_NONE);
U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C u8g2_OPT4(U8G2_R0, /* reset=*/U8X8_PIN_NONE);
U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C u8g2_OPT5(U8G2_R0, /* reset=*/U8X8_PIN_NONE);

// Com1 and Com2 OLEDs
U8G2_SSD1306_64X48_ER_1_HW_I2C u8g2_COM1(U8G2_R0, /* reset=*/2);
U8G2_SSD1306_64X48_ER_1_HW_I2C u8g2_COM2(U8G2_R0, /* reset=*/11);




extern "C" {
#include "utility/twi.h"  // from Wire library, so we can do bus scanning
}

#define TCAADDR 0x70
//#define TCAADDR 0x112


int CurrentDisplay = 0;
int Brightness = 0;
char buffer[20];  //plenty of space for the value of millis() plus a zero terminator


// Altimeter delta 1000 feet for 112 pressure units
// which maps to 8.92857 feet per pressure unit with 2992 as reference
// so delta is (pressure reading - 2992) * 8.92857
#define feetDeltaPerPressureUnit 8.92857


int iLastAltitudeValue = 0;
int iAltitudeDelta = 0;
int iBaro = 2992;
int iBaroOnes = 2;
int iBaroTens = 9;
int iBaroHundreds = 9;
int iBaroThousands = 2;
String BaroOnes = String(iBaroOnes);
String BaroTens = String(iBaroTens);
String BaroHundreds = String(iBaroHundreds);
String BaroThousands = String(iBaroThousands);
bool BaroUpdated = true;

// Altimeter
unsigned long nextAltimeterUpdate = 0;
int updateAltimeterInterval = 100;

String Alt1000s = "0";
int LastAlt1000s = 0;
String Alt10000s = "0";
int LastAlt10000s = 0;
bool AltCounterUpdated = true;

char* ptrtopass;

String txtChaffFlare = "S240s120";
String txtJMRstatus = "SBY AIR ";
String txtMWSstatus = "ACTIVE ";


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

void initCharOLED() {
  // *** I2C initial *** //
  tcaselect(CMSC_OLED_Port);
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
  send_string("TEST");
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

  pinMode(O_LEFT_GEAR_LED, OUTPUT);
  pinMode(O_NOSE_GEAR_LED, OUTPUT);
  pinMode(O_RIGHT_GEAR_LED, OUTPUT);
  pinMode(O_VHF_AM_LED, OUTPUT);
  pinMode(O_VHF_FM_LED, OUTPUT);
  pinMode(O_AOA_BELOW_LED, OUTPUT);
  pinMode(O_AOA_ON_LED, OUTPUT);
  pinMode(O_AOA_ABOVE_LED, OUTPUT);



  digitalWrite(O_LEFT_GEAR_LED, 0);
  digitalWrite(O_NOSE_GEAR_LED, 0);
  digitalWrite(O_RIGHT_GEAR_LED, 0);
  digitalWrite(O_VHF_AM_LED, 0);
  digitalWrite(O_VHF_FM_LED, 0);
  digitalWrite(O_AOA_BELOW_LED, 0);
  digitalWrite(O_AOA_ON_LED, 0);
  digitalWrite(O_AOA_ABOVE_LED, 0);


  delay(500);
  Wire.begin();

  initCharOLED();
  updateCMSC();



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

  tcaselect(BARO_OLED_Port);

  u8g2_BARO.begin();
  u8g2_BARO.clearBuffer();
  u8g2_BARO.setFont(u8g2_font_logisoso16_tf);
  u8g2_BARO.sendBuffer();
  tcaselect(BARO_OLED_Port);
  updateBARO("2992");


  tcaselect(ALT_OLED_Port);
  u8g2_ALT.begin();
  u8g2_ALT.clearBuffer();
  u8g2_ALT.setFont(u8g2_font_logisoso32_tn);
  u8g2_ALT.sendBuffer();
  tcaselect(ALT_OLED_Port);
  updateALT("0", "0");

  delay(3000);
  digitalWrite(O_LEFT_GEAR_LED, 1);
  digitalWrite(O_NOSE_GEAR_LED, 1);
  digitalWrite(O_RIGHT_GEAR_LED, 1);
  digitalWrite(O_VHF_AM_LED, 1);
  digitalWrite(O_VHF_FM_LED, 1);
  digitalWrite(O_AOA_BELOW_LED, 1);
  digitalWrite(O_AOA_ON_LED, 1);
  digitalWrite(O_AOA_ABOVE_LED, 1);

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

  //    display.ssd1306_command(SSD1306_SETPRECHARGE);
  //    display.ssd1306_command(prech);
  //    display.ssd1306_command(SSD1306_SETCONTRAST);
  //    display.ssd1306_command(brigh);
}


void TESTALT(String strnewValue) {

  const char* newValue = strnewValue.c_str();
  tcaselect(ALT_OLED_Port);
  u8g2_ALT.setFontMode(0);
  u8g2_ALT.setDrawColor(0);


  u8g2_ALT.drawBox(0, 0, 128, 64);
  u8g2_ALT.sendBuffer();


  // full range with with U8G2_SSD1309_128X64_NONAME2_F_HW_I2C
  // u8g2_BARO.drawBox(0, 1, 80, 64); with U8G2_SSD1309_128X64_NONAME2_2_HW_I2C
  //  u8g2_BARO.drawStr(85,16, newValue);

  u8g2_ALT.setDrawColor(1);
  u8g2_ALT.setFontDirection(0);


  u8g2_ALT.drawStr(60, 32, newValue);
  u8g2_ALT.sendBuffer();
}


void updateBARO(String strnewValue) {

  const char* newValue = strnewValue.c_str();
  tcaselect(BARO_OLED_Port);
  u8g2_BARO.setFontMode(0);
  u8g2_BARO.setDrawColor(0);


  u8g2_BARO.drawBox(0, 0, 128, 64);
  u8g2_BARO.sendBuffer();



  // full range with with U8G2_SSD1309_128X64_NONAME2_F_HW_I2C
  // u8g2_BARO.drawBox(0, 1, 80, 64); with U8G2_SSD1309_128X64_NONAME2_2_HW_I2C
  //  u8g2_BARO.drawStr(85,16, newValue);

  u8g2_BARO.setDrawColor(1);
  u8g2_BARO.setFontDirection(0);


  u8g2_BARO.drawStr(65, 16, newValue);
  u8g2_BARO.sendBuffer();
}

void buildBAROString() {

  updateBARO(BaroThousands + BaroHundreds + BaroTens + BaroOnes);
  BaroUpdated = false;
}





#define hash_width 24
#define hash_height 32
static unsigned char hash_bits[] = {
  0x00,
  0xFE,
  0x01,
  0x01,
  0xFC,
  0x03,
  0x03,
  0xF8,
  0x07,
  0x07,
  0xF0,
  0x0F,
  0x0F,
  0xE0,
  0x1F,
  0x1F,
  0xC0,
  0x3F,
  0x3F,
  0x80,
  0x7F,
  0x7F,
  0x00,
  0xFE,
  0xFE,
  0x01,
  0xFC,
  0xFC,
  0x03,
  0xF8,
  0xF8,
  0x07,
  0xF0,
  0xF0,
  0x0F,
  0xE0,
  0xE0,
  0x1F,
  0xC0,
  0x00,
  0x3F,
  0x80,
  0x00,
  0x7F,
  0x00,
  0x00,
  0xFE,
  0x01,
  0x01,
  0xFC,
  0x03,
  0x03,
  0xF8,
  0x07,
  0x07,
  0xF0,
  0x0F,
  0x0F,
  0xE0,
  0x1F,
  0x1F,
  0xC0,
  0x3F,
  0x3F,
  0x80,
  0x7F,
  0x7F,
  0x00,
  0xFE,
  0xFE,
  0x01,
  0xFC,
  0xFC,
  0x03,
  0xF8,
  0xF8,
  0x07,
  0xF0,
  0xF0,
  0x0F,
  0xE0,
  0xE0,
  0x1F,
  0xC0,
  0xC0,
  0x3F,
  0x80,
  0x00,
  0x7F,
  0x00,
  0x00,
  0xFE,
  0x00,
  0x00,
  0xFC,
  0x00,
};

int lastHundredsValue = 0;
int lastThousandsValue = 0;
int lastTenThousandsValue = 0;
int lastThousandsCharacterOffset = 0;
int lastHundredsCharacterOffset = 0;

void updateALT(String strTenThousands, String strnewThousands) {

  const char* newTenThousandsValue = strTenThousands.c_str();
  const char* newThousandsValue = strnewThousands.c_str();
  int End_X_Pos = 46;
  int End_Y_Pos = 28;
  int Start_Y_Pos = 13;
  int Start_X_Pos = 27;
  int Box_Width = 20;
  tcaselect(ALT_OLED_Port);
  u8g2_ALT.setFontMode(0);
  u8g2_ALT.setDrawColor(0);
  u8g2_ALT.drawBox(0, 0, 128, 32);
  u8g2_ALT.setDrawColor(1);
  //u8g2_ALT.drawStr(5, 32, newValue);

  if (strTenThousands == "0") {
    u8g2_ALT.setDrawColor(1);

    u8g2_ALT.drawBox(Start_X_Pos, 13, Box_Width, 20);
    u8g2_ALT.setDrawColor(0);

    u8g2_ALT.drawLine(Start_X_Pos, Start_Y_Pos, End_X_Pos, 32);
    u8g2_ALT.drawLine(Start_X_Pos, Start_Y_Pos + 1, End_X_Pos - 1, 32);
    u8g2_ALT.drawLine(Start_X_Pos, Start_Y_Pos + 2, End_X_Pos - 2, 32);
    u8g2_ALT.drawLine(Start_X_Pos, Start_Y_Pos + 3, End_X_Pos - 3, 32);
    u8g2_ALT.drawLine(Start_X_Pos, Start_Y_Pos + 4, End_X_Pos - 4, 32);
    u8g2_ALT.drawLine(Start_X_Pos, Start_Y_Pos + 5, End_X_Pos - 5, 32);
    u8g2_ALT.drawLine(Start_X_Pos, Start_Y_Pos + 6, End_X_Pos - 6, 32);

    u8g2_ALT.drawLine(Start_X_Pos, Start_Y_Pos + 11, End_X_Pos - 11, 32);
    u8g2_ALT.drawLine(Start_X_Pos, Start_Y_Pos + 12, End_X_Pos - 12, 32);
    u8g2_ALT.drawLine(Start_X_Pos, Start_Y_Pos + 13, End_X_Pos - 13, 32);
    u8g2_ALT.drawLine(Start_X_Pos, Start_Y_Pos + 14, End_X_Pos - 14, 32);
    u8g2_ALT.drawLine(Start_X_Pos, Start_Y_Pos + 15, End_X_Pos - 15, 32);

    u8g2_ALT.drawLine(Start_X_Pos + 4, Start_Y_Pos, Start_X_Pos + Box_Width, End_Y_Pos + 1);
    u8g2_ALT.drawLine(Start_X_Pos + 5, Start_Y_Pos, Start_X_Pos + Box_Width, End_Y_Pos);
    u8g2_ALT.drawLine(Start_X_Pos + 6, Start_Y_Pos, Start_X_Pos + Box_Width, End_Y_Pos - 1);
    u8g2_ALT.drawLine(Start_X_Pos + 7, Start_Y_Pos, Start_X_Pos + Box_Width, End_Y_Pos - 2);
    u8g2_ALT.drawLine(Start_X_Pos + 8, Start_Y_Pos, Start_X_Pos + Box_Width, End_Y_Pos - 3);
    u8g2_ALT.drawLine(Start_X_Pos + 9, Start_Y_Pos, Start_X_Pos + Box_Width, End_Y_Pos - 4);
    u8g2_ALT.drawLine(Start_X_Pos + 10, Start_Y_Pos, Start_X_Pos + Box_Width, End_Y_Pos - 5);

    u8g2_ALT.drawLine(Start_X_Pos + 15, Start_Y_Pos, Start_X_Pos + Box_Width, End_Y_Pos - 10);
    u8g2_ALT.drawLine(Start_X_Pos + 16, Start_Y_Pos, Start_X_Pos + Box_Width, End_Y_Pos - 11);
    u8g2_ALT.drawLine(Start_X_Pos + 17, Start_Y_Pos, Start_X_Pos + Box_Width, End_Y_Pos - 12);
    u8g2_ALT.drawLine(Start_X_Pos + 18, Start_Y_Pos, Start_X_Pos + Box_Width, End_Y_Pos - 13);
    u8g2_ALT.drawLine(Start_X_Pos + 19, Start_Y_Pos, Start_X_Pos + Box_Width, End_Y_Pos - 14);

    u8g2_ALT.setDrawColor(1);


  } else {
    u8g2_ALT.drawStr(32, 32, newTenThousandsValue);
  }

  u8g2_ALT.drawStr(65, 32, newThousandsValue);
  u8g2_ALT.sendBuffer();

  AltCounterUpdated = false;
}


void UpdateAltimeterDigits(long height) {

  // 
  //SendDebug("Aircraft Name : " + sAircraftName);
  // SendDebug("Raw Height : " + String(height));
  // Adjust for Baro offset
  height = height + iAltitudeDelta;

  String strnewValue = String(height);


  // int itensAboveDigit = 0;
  // int itensBelowDigit = 0;

  int iHundredsAboveDigit = 0;
  int iHundredsBelowDigit = 0;
  int iHundredsValue = ((height % 1000) / 100);
  String sHundredsValue = String(iHundredsValue);
  // SendDebug("Hundreds Value : " + sHundredsValue);
  if (iHundredsValue == 9) {
    iHundredsAboveDigit = 0;
  } else {
    iHundredsAboveDigit = iHundredsValue + 1;
  }
  if (iHundredsValue == 0) {
    iHundredsBelowDigit = 9;
  } else {
    iHundredsBelowDigit = iHundredsValue - 1;
  }

  String sHundredsAboveDigit = String(iHundredsAboveDigit);
  String sHundredsBelowDigit = String(iHundredsBelowDigit);
  const char* cHundredsValue = sHundredsValue.c_str();
  const char* cHundredsaboveValue = sHundredsAboveDigit.c_str();
  const char* cHundredsbelowValue = sHundredsBelowDigit.c_str();



  int iThousandsAboveDigit = 0;
  int iThousandsBelowDigit = 0;
  int iThousandsValue = ((height % 10000) / 1000);
  String sThousandValue = String(iThousandsValue);
  // SendDebug(String(i) + " : " + sThousandValue);
  if (iThousandsValue == 9) {
    iThousandsAboveDigit = 0;
  } else {
    iThousandsAboveDigit = iThousandsValue + 1;
  }
  if (iThousandsValue == 0) {
    iThousandsBelowDigit = 9;
  } else {
    iThousandsBelowDigit = iThousandsValue - 1;
  }

  String sThousandsAboveDigit = String(iThousandsAboveDigit);
  String sThousandsBelowDigit = String(iThousandsBelowDigit);
  const char* cThousandsValue = sThousandValue.c_str();
  const char* cThousandsaboveValue = sThousandsAboveDigit.c_str();
  const char* cThousandsbelowValue = sThousandsBelowDigit.c_str();


  int iTenThousandsValue = (height / 10000);
  String sTenThousandsDigit = String(iTenThousandsValue);
  //SendDebug("TenThousandsDigit : " + sTenThousandsDigit);
  const char* cTenThousandsValue = sTenThousandsDigit.c_str();

  unsigned long TimeToProcess = millis();

  int CharacterHeightSpacer = 38;

  int iHundredsCharacterOffset = ((height % 100) / 3.2);
  // SendDebug("heigh calc :" + String(height % 100));
  int iThousandsCharacterOffset = ((height % 1000) / 32);
  // SendDebug("Character Offset : " + String(iHundredsCharacterOffset));

  // Only attempt to draw of something has changed that will impact display
  if ((iThousandsValue != lastThousandsValue) || (iTenThousandsValue != lastTenThousandsValue) || (iThousandsCharacterOffset != lastThousandsCharacterOffset)
      || (iHundredsValue != lastHundredsValue) || (iHundredsCharacterOffset != lastHundredsCharacterOffset)) {

    tcaselect(ALT_OLED_Port);

    lastHundredsValue = iHundredsValue;
    lastThousandsValue = iThousandsValue;
    lastTenThousandsValue = iTenThousandsValue;
    lastHundredsCharacterOffset = iHundredsCharacterOffset;
    lastThousandsCharacterOffset = iThousandsCharacterOffset;


    u8g2_ALT.setFontMode(0);
    u8g2_ALT.setDrawColor(0);
    u8g2_ALT.drawBox(0, 0, 128, 32);
    u8g2_ALT.setDrawColor(1);

    // If Ten Thousands value is a 0 draw the hash
    // Position was 10 and 0 moved 40 to the right
    if (sTenThousandsDigit != "0") {
      u8g2_ALT.drawStr(50, 30, cTenThousandsValue);
    } else {
      u8g2_ALT.drawXBM(40, 0, hash_width, hash_height, hash_bits);
    }



    // Three digits for A10 Altimer as it is a primary instrument - only two for Hornet
    if (sAircraftName == "A-10C") {
      // u8g2_ALT.drawStr(40, -2 + 0, cThousandsaboveValue);
      // u8g2_ALT.drawStr(40, 30 + 0, cThousandsValue);
      // u8g2_ALT.drawStr(70, -2 + iHundredsCharacterOffset, cHundredsaboveValue);
      // u8g2_ALT.drawStr(70, 30 + iHundredsCharacterOffset, cHundredsValue);
      // X POs was 40 andd 70 - 30 pixels diff
      u8g2_ALT.drawStr(80, -2 + 0, cThousandsaboveValue);
      u8g2_ALT.drawStr(80, 30 + 0, cThousandsValue);
      u8g2_ALT.drawStr(110, 58 - iHundredsCharacterOffset, cHundredsaboveValue);
      u8g2_ALT.drawStr(110, 26 - iHundredsCharacterOffset, cHundredsValue);
    } else {
      u8g2_ALT.drawStr(80, -2 + iThousandsCharacterOffset, cThousandsaboveValue);
      u8g2_ALT.drawStr(80, 30 + iThousandsCharacterOffset, cThousandsValue);
    }
    u8g2_ALT.sendBuffer();
    ;
    TimeToProcess = millis() - TimeToProcess;
    //SendDebug("OLED Update time :" + String(TimeToProcess));
  }
}

void onAltMslFtChange(unsigned int newValue) {
  if (newValue <= 0) newValue = 0;
  iLastAltitudeValue = newValue;
  // SendDebug("Height Update : " + String(newValue));
  UpdateAltimeterDigits(newValue);
}
DcsBios::IntegerBuffer altMslFtBuffer(0x0434, 0xffff, 0, onAltMslFtChange);

bool PressureChanged = false;
void ProcessPressureChange() {

  iBaro = iBaroThousands * 1000 + iBaroHundreds * 100 + iBaroTens * 10 + iBaroOnes;
  // SendDebug("Calc delta :" + String(int((iBaro - 2992) * feetDeltaPerPressureUnit)));
  iAltitudeDelta = int((iBaro - 2992) * feetDeltaPerPressureUnit);

   // SendDebug("iBaro :" + String(iBaro) + "  Alt :" + String(iLastAltitudeValue) + "   Delta : " + String(iAltitudeDelta));
  if (sAircraftName == "FA-18C_hornet") {
    BaroThousands = "2";
    BaroHundreds = "9";
  }

  updateBARO(BaroThousands + BaroHundreds + BaroTens + BaroOnes);
  UpdateAltimeterDigits(iLastAltitudeValue);
  PressureChanged = false;
}

// ****************** BEGIN HORNET ************************************** //
// Smallest Digit
void onStbyPressSet0Change(unsigned int newValue) {
  // 0 64592, 1 6797, 2 14537, 3 18407, 4 26147, 5 33887,
  // 6 37757, 7 45497, 8 53237, 9 57107, 0 64847
  // if (newValue >= 63000)
  //   newValue = 0;
  newValue = newValue + 5;
  iBaroOnes = map(newValue, 0, 65535, 0, 10);
  BaroOnes = String(iBaroOnes);
  PressureChanged = true;
}
DcsBios::IntegerBuffer stbyPressSet0Buffer(0x74fa, 0xffff, 0, onStbyPressSet0Change);


void onStbyPressSet1Change(unsigned int newValue) {
  // if (newValue >= 63000)
  //   newValue = 0;
  newValue = newValue + 5;
  iBaroTens = map(newValue, 0, 65535, 0, 10);
  BaroTens = String(iBaroTens);
  PressureChanged = true;
}

DcsBios::IntegerBuffer stbyPressSet1Buffer(0x74fc, 0xffff, 0, onStbyPressSet1Change);

// Second largest digit - the largest digit doesn't change
void onStbyPressSet2Change(unsigned int newValue) {
  newValue = 0;
}
DcsBios::IntegerBuffer stbyPressSet2Buffer(0x74fe, 0xffff, 0, onStbyPressSet2Change);

// ****************** END HORNET ************************************** //


// ****************** BEGIN A10 ************************************** //
void onAltPressure0Change(unsigned int newValue) {
  // Provide a little bias to deal with integer maths
  newValue = newValue + 5;
  iBaroOnes = map(newValue, 0, 65535, 0, 10);
  BaroOnes = String(iBaroOnes);
  PressureChanged = true;
}
DcsBios::IntegerBuffer altPressure0Buffer(0x1086, 0xffff, 0, onAltPressure0Change);

void onAltPressure1Change(unsigned int newValue) {
  // Provide a little bias to deal with integer maths
  newValue = newValue + 5;
  iBaroTens = map(newValue, 0, 65535, 0, 10);
  BaroTens = String(iBaroTens);
  PressureChanged = true;
}
DcsBios::IntegerBuffer altPressure1Buffer(0x1088, 0xffff, 0, onAltPressure1Change);

void onAltPressure2Change(unsigned int newValue) {
  // Provide a little bias to deal with integer maths
  newValue = newValue + 5;
  iBaroHundreds = map(newValue, 0, 65535, 0, 10);
  BaroHundreds = String(iBaroHundreds);
  PressureChanged = true;
}
DcsBios::IntegerBuffer altPressure2Buffer(0x108a, 0xffff, 0, onAltPressure2Change);

void onAltPressure3Change(unsigned int newValue) {

  // Provide a little bias to deal with integer maths
  // Otherwise digits as a little low
  // 30% is 19660.5 but gets runded down to 19559 I'm guessing
  newValue = newValue + 5;
  iBaroThousands = map(newValue, 0, 65535, 0, 10);
  BaroThousands = String(iBaroThousands);
  PressureChanged = true;
}
DcsBios::IntegerBuffer altPressure3Buffer(0x108c, 0xffff, 0, onAltPressure3Change);

// ******************  END A10  ************************************** //

void onAcftNameChange(char* newValue) {
  sAircraftName = String(newValue);
  // SendDebug("Aircraft Name : " + sAircraftName);
}
DcsBios::StringBuffer<24> AcftNameBuffer(0x0000, onAcftNameChange);

void onCmscTxtChaffFlareChange(char* newValue) {

  txtChaffFlare = String(newValue);
  updateCMSC();
}
DcsBios::StringBuffer<8> cmscTxtChaffFlareBuffer(0x108e, onCmscTxtChaffFlareChange);



void onCmscTxtJmrChange(char* newValue) {
  txtJMRstatus = String(newValue);
  updateCMSC();
}
DcsBios::StringBuffer<8> cmscTxtJmrBuffer(0x1096, onCmscTxtJmrChange);

void onCmscTxtMwsChange(char* newValue) {
  txtMWSstatus = String(newValue);
  updateCMSC();
}
DcsBios::StringBuffer<8> cmscTxtMwsBuffer(0x133c, onCmscTxtMwsChange);

void updateCMSC() {
  tcaselect(CMSC_OLED_Port);
  sendCommand(cmd_CLS);
  send_string(txtChaffFlare.c_str());
  sendCommand(cmd_NewLine);
  String workstring = txtJMRstatus + " " + txtMWSstatus;
  send_string(workstring.c_str());
}




void onGearLSafeChange(unsigned int newValue) {
  if (newValue == 1) {
    digitalWrite(O_LEFT_GEAR_LED, 0);
  } else {
    digitalWrite(O_LEFT_GEAR_LED, 1);
  }
}
DcsBios::IntegerBuffer gearLSafeBuffer(0x1026, 0x1000, 12, onGearLSafeChange);

void onGearNSafeChange(unsigned int newValue) {
  if (newValue == 1) {
    digitalWrite(O_NOSE_GEAR_LED, 0);
  } else {
    digitalWrite(O_NOSE_GEAR_LED, 1);
  }
}
DcsBios::IntegerBuffer gearNSafeBuffer(0x1026, 0x0800, 11, onGearNSafeChange);

void onGearRSafeChange(unsigned int newValue) {
  if (newValue == 1) {
    digitalWrite(O_RIGHT_GEAR_LED, 0);
  } else {
    digitalWrite(O_RIGHT_GEAR_LED, 1);
  }
}
DcsBios::IntegerBuffer gearRSafeBuffer(0x1026, 0x2000, 13, onGearRSafeChange);



void onAoaIndexerHighChange(unsigned int newValue) {
  if (newValue == 1) {
    digitalWrite(O_AOA_ABOVE_LED, 0);
  } else {
    digitalWrite(O_AOA_ABOVE_LED, 1);
  }
}
DcsBios::IntegerBuffer aoaIndexerHighBuffer(0x1012, 0x1000, 12, onAoaIndexerHighChange);

void onAoaIndexerNormalChange(unsigned int newValue) {
  if (newValue == 1) {
    digitalWrite(O_AOA_ON_LED, 0);
  } else {
    digitalWrite(O_AOA_ON_LED, 1);
  }
}
DcsBios::IntegerBuffer aoaIndexerNormalBuffer(0x1012, 0x2000, 13, onAoaIndexerNormalChange);

void onAoaIndexerLowChange(unsigned int newValue) {
  if (newValue == 1) {
    digitalWrite(O_AOA_BELOW_LED, 0);
  } else {
    digitalWrite(O_AOA_BELOW_LED, 1);
  }
}
DcsBios::IntegerBuffer aoaIndexerLowBuffer(0x1012, 0x4000, 14, onAoaIndexerLowChange);

void loop() {


  if (millis() >= NEXT_STATUS_TOGGLE_TIMER) {
    RED_LED_STATE = !RED_LED_STATE;

    digitalWrite(Check_LED_G, RED_LED_STATE);
    digitalWrite(Check_LED_R, !RED_LED_STATE);
    NEXT_STATUS_TOGGLE_TIMER = millis() + FLASH_TIME;
  }

  if (DCSBIOS_In_Use == 1) DcsBios::loop();

  if (PressureChanged == true) ProcessPressureChange();

  // for (long i = 12000; i >= 0; i--) {
  //   UpdateAltimeterDigits(i);
  // }
  // for (long i = 0; i <= 64000; i++) {
  //   UpdateAltimeterDigits(i);
  // }

  // delay(1000);
}
