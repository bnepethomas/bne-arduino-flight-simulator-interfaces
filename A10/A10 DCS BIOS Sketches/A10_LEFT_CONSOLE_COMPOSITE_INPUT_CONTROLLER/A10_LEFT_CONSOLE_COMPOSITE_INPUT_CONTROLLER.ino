/*
A10_LEFT_CONSOLE_COMPOSITE_INPUT_CONTROLLER

Drives

UHF_PRESET_OLED_Port 0
UHF_FREQUENCY_OLED_Port 1
VHF_AM_OLED_Port 2
VHF_FM_OLED_Port 3

FLOOD_LIGHTS 9
FORMATION_LIGHTS 11
STROBE_LIGHTS 8
NAVIGATION_LIGHTS 7
BACK_LIGHTS 11
DcsBios::LED saspToTrim_1
DcsBios::LED saspToTrim_2

MAG SWITCHES
DcsBios::LED saspPitchSasR
DcsBios::LED saspPitchSasL
DcsBios::LED saspYawSasR
DcsBios::LED saspYawSasL



*/

////////////////////---||||||||||********||||||||||---\\\\\\\\\\\\\\\\\\\\
//||               FUNCTION = HORNET LEFT INPUT                       ||\\
//||              LOCATION IN THE PIT = LEFTHAND SIDE                 ||\\
//||            ARDUINO PROCESSOR TYPE = Arduino Mega 2560            ||\\
//||      ARDUINO CHIP SERIAL NUMBER = SN - SN: 75438313633351A072D0  ||\\
//||      ETHERNET SHEILD MAC ADDRESS = MAC                           ||\\
//||                    CONNECTED COM PORT = COM 5                    ||\\
//||               ****ADD ASSIGNED COM PORT NUMBER****               ||\\
//||            ****DO CHECK S/N BEFORE UPLOAD NEW DATA****           ||\\
////////////////////---||||||||||********||||||||||---\\\\\\\\\\\\\\\\\\\\

/*
   KNOWN ISSUE - UNABLE USE A SINGLE CHARACTER WITH DCS ie ctrl-c doesn't work but crtl-F5 does
   Keystrokes are received is using at command prompt
 */

/*
  This Superceedes udp_input_controller
  Split from udp_input_controller_2 20200802

  Heavily based on
  https://github.com/calltherain/ArduinoUSBJoystick

  UPDATED 20240808 Enabled 12th Column

  Interface for DCS BIOS

  Mega2560 R3,
  digital Pin 22~37 used as rows. 0-15, 16 Rows
  digital pin 38~49 used as columns. 0-11, 12 Columns

  it's a 16 * 12  matrix, due to the loss of pins/Columns used by the Ethernet and SD Card Shield, Digital I/O 50 through 53 are not available.
  Pin 49 is available but isn't used. This gives a total number of usable Inputs as 176 (remember numbering starts at 0 - so 0-175)

  The code pulls down a row and reads values from the Columns.
  Row 0 - Col 0 = Digit 0
  Row 0 - Col 10 = Digit 10
  Row 15 - Col 0 = Digit 165
  Row 15 = Col 10 = Digit 175

  So - Digit = Row * 11 + Col



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
    I2C - 0 = COMM 1
    I2C - 1 = COMM 2
    I2C - 2 = SPAD
    I2C - 3 = OLED (1) TOP
    I2C - 4 = OLED (2) C TOP
    I2C - 5 = OLED (3) CTR
    I2C - 6 = OLED (4) C BOTTEM
    I2C - 7 = OLED (5) BOTTEM
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
// other SSD1309 claases only displayed down to row 8 or 16
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



#define Ethernet_In_Use 1
#define Reflector_In_Use 1
#define DCSBIOS_In_Use 1
#define MSFS_In_Use 1  // Used to interface into MSFS - set to 0 if not in use


#define Check_LED_R 12
#define Check_LED_G 13

#define RED_STATUS_LED_PORT 12
#define GREEN_STATUS_LED_PORT 13

#define FLASH_TIME 300

long NEXT_STATUS_TOGGLE_TIMER = 0;
bool RED_LED_STATE = false;
bool GREEN_LED_STATE = false;
unsigned long timeSinceRedLedChanged = 0;


// Used to Distinguish between the left, front, and right inputs
// Left=0, Front=1, Right=2
#define INPUT_MODULE_NUMBER 0

#define DCSBIOS_IRQ_SERIAL
#include "DcsBios.h"


// ############################### Begin Ethernet Related ####################################
#include <SPI.h>
#include <Ethernet.h>
#include <EthernetUdp.h>

#define EthernetStartupDelay 500
#define ES1_RESET_PIN 53

String BoardName = "A10 Forward Input";

// These local Mac and IP Address will be reassigned early in startup based on
// the device ID as set by address pins

byte myMac[] = { 0xA8, 0x61, 0x0A, 0x67, 0x83, 0x05 };
String sMac = "A8:61:0A:67:83:05";
IPAddress myIP(172, 16, 1, 105);
String strMyIP = "172.16.1.105";

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

int packetSize;
int debugLen;
EthernetUDP udp;
EthernetUDP debugUDP;
// char packetBuffer[1000];     //buffer to store the incoming data
char outpacketBuffer[1000];  //buffer to store the outgoing data

const unsigned long delayBeforeSendingPacket = 2000;
unsigned long ethernetStartTime = 0;

// ############################### End Ethernet Related ####################################

// ############################### Begin Input Related #####################################
#define NUM_BUTTONS 256
#define BUTTONS_USED_ON_PCB 176
#define NUM_AXES 8  // 8 axes, X, Y, Z, etc


//
struct joyReport_t {
  int button[NUM_BUTTONS];  // 1 Button per byte - was originally one bit per byte - but we have plenty of storage space
};

// Go through the man loop a number of times before sending data to the Sim
// This allows analog averages to be established and the DigitalButton array to be populated
// This has to more than simply the number of elements in the array as the array values are initialised
// to 0, so it actually takes more than 30 interations before the average it fully established
int LoopsBeforeSendingAllowed = 40;
bool SendingAllowed = false;


// Debounce delay was 20mS - but encountered longer bounces with Circuit Breakers, increased to 60mS 20210329
const int ScanDelay = 80;      // This is in microseconds
const int DebounceDelay = 40;  // In milliseconds

joyReport_t joyReport;
joyReport_t prevjoyReport;


unsigned long joyEndDebounce[NUM_BUTTONS];  // Holds the time we'll look at any more changes in a given input

long prevLEDTransition = millis();
int cButtonID[16];
bool bFirstTime = false;


unsigned long currentMillis = 0;
unsigned long previousMillis = 0;

// Note Pin 4 and Pin 10 cannot be used for other purposes.
//Arduino communicates with both the W5500 and SD card using the SPI bus (through the ICSP header).
//This is on digital pins 10, 11, 12, and 13 on the Uno and pins 50, 51, and 52 on the Mega.
//On both boards, pin 10 is used to select the W5500 and pin 4 for the SD card. These pins cannot be used for general I/O.
//On the Mega, the hardware SS pin, 53, is not used to select either the W5500 or the SD card,
//but it must be kept as an output or the SPI interface won't work.


char stringind[5];
String outString;

// ############################### END INPUT RELATED #####################################

// ############################### BEGIN GRAPHIC OLED RELATED ############################
#include <U8g2lib.h>
#include <Wire.h>
extern "C" {
#include "utility/twi.h"  // from Wire library, so we can do bus scanning
}

#define TCAADDR 0x70


void tcaselect(uint8_t i) {
  if (i > 7) return;

  Wire.beginTransmission(TCAADDR);
  Wire.write(1 << i);
  Wire.endTransmission();
}

// Found it easier to scan U8g2lib.h when looking for names of classes
// But also found here - https://github.com/olikraus/u8g2/wiki
// Working 2.42" Inch OLED for A10
// U8G2_SSD1309_128X64_NONAME2_F_HW_I2C u8g2_BARO(U8G2_R0, U8X8_PIN_NONE);
// Working 0.96" inch OLED display - ALSO WORKS WITH SSD1309 2.42" DISPLAY
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2_VHF_AM_PRESET(U8G2_R0, U8X8_PIN_NONE);
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2_VHF_FM_PRESET(U8G2_R0, U8X8_PIN_NONE);
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2_UHF_PRESET(U8G2_R0, U8X8_PIN_NONE);
U8G2_SSD1309_128X64_NONAME2_F_HW_I2C u8g2_UHF_FREQUENCY(U8G2_R0, U8X8_PIN_NONE);

#define UHF_PRESET_OLED_Port 0
#define UHF_FREQUENCY_OLED_Port 1
#define VHF_AM_OLED_Port 2
#define VHF_FM_OLED_Port 3

// ############################### END GRAPHIC OLED RELATED ####################################################


// ############################################# BEGIN CHARACTER OLED ##########################################

#define INTERCOMM_OLED_Port 7

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

void send_string(const char *String) {
  unsigned char i = 0;
  while (String[i]) {
    sendData(String[i]);  // *** Show String to OLED
    i++;
  }
}

void OLED_CLS() {
  sendCommand(0x01);
  // DEL - CLS
  sendCommand(0x80);  // Home Pos
  // DEL - CLS
  sendCommand(0xC0);
}

bool INT_Status = true;
bool FM_Status = false;
bool UHF_Status = false;
bool VHF_Status = true;
bool AIM_Status = false;
bool IFF_Status = false;
bool ILS_Status = false;
// TACAN is muted by default
bool TCN_Status = false;



void updateIntercomm() {
  // Ran out of RAM whenpreforming string operation so using Bool
  SendDebug("Updating Intercomm a");
  tcaselect(INTERCOMM_OLED_Port);
  delay(5);
  sendCommand(0x80);
  //workString = INT_Status + FM_Status + UHF_Status + VHF_Status;
  if (INT_Status == true)
    send_string("INT ");
  else
    send_string("    ");

  if (FM_Status == true)
    send_string("FM  ");
  else
    send_string("    ");

  if (UHF_Status == true)
    send_string("UHF ");
  else
    send_string("    ");
  if (VHF_Status == true)
    send_string("VHF ");
  else
    send_string("    ");

  sendCommand(cmd_NewLine);

  if (AIM_Status == true)
    send_string("AIM ");
  else
    send_string("    ");

  if (IFF_Status == true)
    send_string("IFF ");
  else
    send_string("    ");

  if (ILS_Status == true)
    send_string("ILS ");
  else
    send_string("    ");

  if (TCN_Status == true)

    send_string("TCN ");
  else
    send_string("    ");


  SendDebug("Updating Intercomm Complete");
}

void onIntIntUnmuteChange(unsigned int newValue) {
  if (newValue == 1)
    INT_Status = true;
  else
    INT_Status = false;
  updateIntercomm();
}
DcsBios::IntegerBuffer intIntUnmuteBuffer(0x1194, 0x8000, 15, onIntIntUnmuteChange);

void onIntFmUnmuteChange(unsigned int newValue) {
  if (newValue == 1)
    FM_Status = true;
  else
    FM_Status = false;
  updateIntercomm();
}
DcsBios::IntegerBuffer intFmUnmuteBuffer(0x119c, 0x8000, 15, onIntFmUnmuteChange);

void onIntUhfUnmuteChange(unsigned int newValue) {
  if (newValue == 1)
    UHF_Status = true;
  else
    UHF_Status = false;
  updateIntercomm();
  }
  DcsBios::IntegerBuffer intUhfUnmuteBuffer(0x11a6, 0x0002, 1, onIntUhfUnmuteChange);

void onIntVhfUnmuteChange(unsigned int newValue) {
  if (newValue == 1)
    VHF_Status = true;
  else
    VHF_Status = false;
  updateIntercomm();
  }
  DcsBios::IntegerBuffer intVhfUnmuteBuffer(0x11a6, 0x0001, 0, onIntVhfUnmuteChange);


void onIntAimUnmuteChange(unsigned int newValue) {
  if (newValue == 1)
    AIM_Status = true;
  else
    AIM_Status = false;
  updateIntercomm();
}
DcsBios::IntegerBuffer intAimUnmuteBuffer(0x11a6, 0x0004, 2, onIntAimUnmuteChange);

void onIntIffUnmuteChange(unsigned int newValue) {
  if (newValue == 1)
    IFF_Status = true;
  else
    IFF_Status = false;
  updateIntercomm();
}
DcsBios::IntegerBuffer intIffUnmuteBuffer(0x11a6, 0x0008, 3, onIntIffUnmuteChange);

void onIntIlsUnmuteChange(unsigned int newValue) {
  if (newValue == 1)
    ILS_Status = true;
  else
    ILS_Status = false;
  updateIntercomm();
}
DcsBios::IntegerBuffer intIlsUnmuteBuffer(0x11a6, 0x0010, 4, onIntIlsUnmuteChange);

void onIntTcnUnmuteChange(unsigned int newValue) {
  if (newValue == 1)
    TCN_Status = true;
  else
    TCN_Status = false;
  updateIntercomm();
}
DcsBios::IntegerBuffer intTcnUnmuteBuffer(0x11a6, 0x0020, 5, onIntTcnUnmuteChange);

// ############################################# END CHARACTER OLED ##########################################



void update_VHF_AM_PRESET_OLED(String strnewValue) {

  const char *newValue = strnewValue.c_str();
  tcaselect(VHF_AM_OLED_Port);

  // Clear existing text by drawing a black box
  u8g2_VHF_AM_PRESET.setFontMode(0);
  u8g2_VHF_AM_PRESET.setDrawColor(0);
  u8g2_VHF_AM_PRESET.drawBox(0, 0, 127, 63);

  u8g2_VHF_AM_PRESET.setDrawColor(1);
  u8g2_VHF_AM_PRESET.setFontDirection(0);
  u8g2_VHF_AM_PRESET.drawStr(20, 60, newValue);
  u8g2_VHF_AM_PRESET.sendBuffer();
}

void update_VHF_FM_PRESET_OLED(String strnewValue) {

  const char *newValue = strnewValue.c_str();
  tcaselect(VHF_FM_OLED_Port);

  // Clear existing text by drawing a black box
  u8g2_VHF_FM_PRESET.setFontMode(0);
  u8g2_VHF_FM_PRESET.setDrawColor(0);
  u8g2_VHF_FM_PRESET.drawBox(0, 0, 127, 63);

  u8g2_VHF_FM_PRESET.setDrawColor(1);
  u8g2_VHF_FM_PRESET.setFontDirection(0);
  u8g2_VHF_FM_PRESET.drawStr(20, 60, newValue);
  u8g2_VHF_FM_PRESET.sendBuffer();
}

void update_UHF_PRESET_OLED(String strnewValue) {

  const char *newValue = strnewValue.c_str();
  tcaselect(UHF_PRESET_OLED_Port);

  // Clear existing text by drawing a black box
  u8g2_UHF_PRESET.setFontMode(0);
  u8g2_UHF_PRESET.setDrawColor(0);
  u8g2_UHF_PRESET.drawBox(0, 0, 127, 63);

  u8g2_UHF_PRESET.setDrawColor(1);
  u8g2_UHF_PRESET.setFontDirection(0);
  SendDebug("UHF Preset Charar array length :" + String(strnewValue.length()));
  // Handle Test Message
  if (strnewValue.length() == 3) {
    u8g2_UHF_PRESET.drawStr(20, 90, newValue);
  } else {
    if (strnewValue.length() == 1) {
      u8g2_UHF_PRESET.drawStr(40, 60, newValue);
    } else {
      u8g2_UHF_PRESET.drawStr(20, 60, newValue);
    }
  }

  u8g2_UHF_PRESET.sendBuffer();
}

void onUhfPresetChange(char *newValue) {
  String wrkstring = newValue;
  update_UHF_PRESET_OLED(wrkstring);
}
DcsBios::StringBuffer<2> uhfPresetBuffer(0x1188, onUhfPresetChange);



void update_UHF_FREQUENCY_OLED(String strnewValue) {

  const char *newValue = strnewValue.c_str();
  tcaselect(UHF_FREQUENCY_OLED_Port);

  // Clear existing text by drawing a black box
  u8g2_UHF_FREQUENCY.setFontMode(0);
  u8g2_UHF_FREQUENCY.setDrawColor(0);
  u8g2_UHF_FREQUENCY.drawBox(0, 0, 127, 63);

  u8g2_UHF_FREQUENCY.setDrawColor(1);
  u8g2_UHF_FREQUENCY.setFontDirection(0);
  u8g2_UHF_FREQUENCY.drawStr(0, 60, newValue);
  u8g2_UHF_FREQUENCY.sendBuffer();
}

void onUhfFrequencyChange(char *newValue) {
  String wrkstring = newValue;
  update_UHF_FREQUENCY_OLED(wrkstring);
}
DcsBios::StringBuffer<7> uhfFrequencyBuffer(0x1180, onUhfFrequencyChange);


// Pinouts for Version 4 PCB
//#define MAP_LIGHTS 9
//#define NVG_LIGHTS 9
#define FLOOD_LIGHTS 9
#define FORMATION_LIGHTS 11
#define STROBE_LIGHTS 8
#define NAVIGATION_LIGHTS 7
#define BACK_LIGHTS 11

void onIntConsoleLBrightChange(unsigned int newValue) {
  analogWrite(BACK_LIGHTS, map(newValue, 0, 65535, 0, 255));
}
DcsBios::IntegerBuffer intConsoleLBrightBuffer(A_10C_INT_CONSOLE_L_BRIGHT, onIntConsoleLBrightChange);


void onIntFloodLBrightChange(unsigned int newValue) {
  analogWrite(FLOOD_LIGHTS, map(newValue, 0, 65535, 0, 255));
}
DcsBios::IntegerBuffer intFloodLBrightBuffer(A_10C_INT_FLOOD_L_BRIGHT, onIntFloodLBrightChange);


void onExtFormationLightsChange(unsigned int newValue) {
  analogWrite(FORMATION_LIGHTS, map(newValue, 0, 65535, 0, 255));
}
DcsBios::IntegerBuffer extFormationLightsBuffer(0x1352, 0xffff, 0, onExtFormationLightsChange);


void onExtStrobeLeftChange(unsigned int newValue) {
  digitalWrite(STROBE_LIGHTS, newValue);
}
DcsBios::IntegerBuffer extStrobeLeftBuffer(A_10C_EXT_STROBE_LEFT, onExtStrobeLeftChange);

void onExtPositionLightRightChange(unsigned int newValue) {
  digitalWrite(NAVIGATION_LIGHTS, newValue);
}
DcsBios::IntegerBuffer extPositionLightRightBuffer(A_10C_EXT_POSITION_LIGHT_RIGHT, onExtPositionLightRightChange);

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


    Ethernet.begin(myMac, myIP);
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


    // Initialise Exterior Lights
    pinMode(STROBE_LIGHTS, OUTPUT);
    pinMode(NAVIGATION_LIGHTS, OUTPUT);
    pinMode(FORMATION_LIGHTS, OUTPUT);
    pinMode(BACK_LIGHTS, OUTPUT);
    pinMode(FLOOD_LIGHTS, OUTPUT);
    // Turn Everything on for 3 Seconds
    analogWrite(STROBE_LIGHTS, 255);
    analogWrite(NAVIGATION_LIGHTS, 255);
    analogWrite(FORMATION_LIGHTS, 255);
    analogWrite(BACK_LIGHTS, 255);
    analogWrite(FLOOD_LIGHTS, 255);



    SendDebug("Dimming Leds");
    for (int Local_Brightness = 255; Local_Brightness >= 0; Local_Brightness--) {
      analogWrite(STROBE_LIGHTS, Local_Brightness);
      analogWrite(NAVIGATION_LIGHTS, Local_Brightness);
      analogWrite(FORMATION_LIGHTS, Local_Brightness);
      analogWrite(BACK_LIGHTS, Local_Brightness);
      analogWrite(FLOOD_LIGHTS, Local_Brightness);
      // SendDebug("Led Brightness " + String(Local_Brightness));
      delay(15);
    }

#define BrightnessWhileRunningSetup 128
    analogWrite(STROBE_LIGHTS, BrightnessWhileRunningSetup);
    analogWrite(NAVIGATION_LIGHTS, BrightnessWhileRunningSetup);
    analogWrite(FORMATION_LIGHTS, BrightnessWhileRunningSetup);
    analogWrite(BACK_LIGHTS, BrightnessWhileRunningSetup);
    analogWrite(FLOOD_LIGHTS, BrightnessWhileRunningSetup);



    if (Reflector_In_Use == 1) {
      udp.beginPacket(reflectorIP, reflectorport);
      udp.println("INIT LEFT COMPOSITE INPUT - " + strMyIP + " " + String(millis()) + "mS since reset.");
      udp.endPacket();
    }

    // Used to remotely enable Debug and Reflector
    debugUDP.begin(localdebugport);
  }

  Wire.begin();

  //initCharOLED();
  SendDebug("Scanning I2C Bus");

  for (uint8_t t = 0; t < 8; t++) {
    tcaselect(t);
    SendDebug("TCA Port #" + String(t));

    for (uint8_t addr = 0; addr <= 127; addr++) {
      //if (addr == TCAADDR) continue;
      uint8_t data;
      if (!twi_writeTo(addr, &data, 0, 1, 1)) {
        SendDebug("Found I2C " + String(addr));
      }
    }
  }
  SendDebug("I2C scan complete");


  initCharOLED(INTERCOMM_OLED_Port);
  updateIntercomm();



  tcaselect(VHF_AM_OLED_Port);
  u8g2_VHF_AM_PRESET.begin();
  u8g2_VHF_AM_PRESET.clearBuffer();
  //u8g2_VHF_AM_PRESET.setFont(u8g2_font_logisoso16_tn);
  u8g2_VHF_AM_PRESET.setFont(u8g2_font_logisoso58_tn);
  // u8g2_VHF_AM_PRESET.setFont(u8g2_font_7Segments_26x42);
  u8g2_VHF_AM_PRESET.sendBuffer();
  tcaselect(VHF_AM_OLED_Port);
  update_VHF_AM_PRESET_OLED("1");

  tcaselect(VHF_FM_OLED_Port);
  u8g2_VHF_FM_PRESET.begin();
  u8g2_VHF_FM_PRESET.clearBuffer();
  //u8g2_VHF_FM_PRESET.setFont(u8g2_font_logisoso16_tn);
  u8g2_VHF_FM_PRESET.setFont(u8g2_font_logisoso58_tn);
  // u8g2_VHF_FM_PRESET.setFont(u8g2_font_7Segments_26x42);
  u8g2_VHF_FM_PRESET.sendBuffer();
  tcaselect(VHF_FM_OLED_Port);
  update_VHF_FM_PRESET_OLED("2");

  tcaselect(UHF_PRESET_OLED_Port);
  u8g2_UHF_PRESET.begin();
  u8g2_UHF_PRESET.clearBuffer();
  //u8g2_VHF_FM_PRESET.setFont(u8g2_font_logisoso16_tn);
  u8g2_UHF_PRESET.setFont(u8g2_font_logisoso58_tn);
  // u8g2_VHF_FM_PRESET.setFont(u8g2_font_7Segments_26x42);
  u8g2_UHF_PRESET.sendBuffer();
  tcaselect(UHF_PRESET_OLED_Port);
  //update_UHF_PRESET_OLED("3");
  //delay(4000);
  update_UHF_PRESET_OLED("*.*");


  tcaselect(UHF_FREQUENCY_OLED_Port);
  u8g2_UHF_FREQUENCY.begin();
  u8g2_UHF_FREQUENCY.clearBuffer();
  //u8g2_VHF_FM_PRESET.setFont(u8g2_font_logisoso16_tn);
  u8g2_UHF_FREQUENCY.setFont(u8g2_font_logisoso30_tf);
  //u8g2_VHF_FM_PRESET.setFont(u8g2_font_freedoomr25_tn);
  u8g2_UHF_FREQUENCY.sendBuffer();
  tcaselect(UHF_FREQUENCY_OLED_Port);
  update_UHF_FREQUENCY_OLED("888888");

#define UHF_PRESET_OLED_Port 0
#define UHF_FREQUENCY_OLED_Port 1


  // Set the output ports to output
  for (int portId = 22; portId < 38; portId++) {
    pinMode(portId, OUTPUT);
  }


  // Set the input ports to input - port 50-53 used by Ethernet Shield
  for (int portId = 38; portId < 50; portId++) {
    // Even though we've already got 10K external pullups
    pinMode(portId, INPUT_PULLUP);
  }



  // Initialise all arrays
  for (int ind = 0; ind < NUM_BUTTONS; ind++) {

    // Clear current and last values to 0 for button inputs
    joyReport.button[ind] = 0;
    prevjoyReport.button[ind] = 0;

    // Set the end
    joyEndDebounce[ind] = 0;
  }


  if (DCSBIOS_In_Use == 1) DcsBios::setup();

#define BrightnessPostSetup 65

  analogWrite(STROBE_LIGHTS, BrightnessPostSetup);
  analogWrite(NAVIGATION_LIGHTS, BrightnessPostSetup);
  analogWrite(FORMATION_LIGHTS, BrightnessPostSetup);
  analogWrite(BACK_LIGHTS, BrightnessPostSetup);
  analogWrite(FLOOD_LIGHTS, BrightnessPostSetup);


  SendDebug(BoardName + " - " + strMyIP + " Setup Complete. " + String(millis()) + "mS since reset.");
}


void FindInputChanges() {

  for (int ind = 0; ind < NUM_BUTTONS; ind++)
    if (bFirstTime) {

      bFirstTime = false;
      // Just Copy Array and perform no actions - this may change in the future
      prevjoyReport.button[ind] = joyReport.button[ind];
    } else {
      // Not the first time - see if there is a difference from last time
      // If there is perform action and update prev array BUT only if we past the end of the debounce period
      if (prevjoyReport.button[ind] != joyReport.button[ind] && millis() > joyEndDebounce[ind]) {

        // First things first - set a new debounce period
        joyEndDebounce[ind] = millis() + DebounceDelay;

        sprintf(stringind, "%03d", ind);


        if (prevjoyReport.button[ind] == 0) {
          outString = outString + "1";
          if (DCSBIOS_In_Use == 1) createDcsBiosMessage(ind, 1);
          if (Ethernet_In_Use == 1) SendIPMessage(ind, 1);
          if (MSFS_In_Use == 1) SendMSFSMessage(ind, 1);
        } else {
          outString = outString + "0";
          if (DCSBIOS_In_Use == 1) createDcsBiosMessage(ind, 0);
          if (Ethernet_In_Use == 1) SendIPMessage(ind, 0);
          if (MSFS_In_Use == 1) SendMSFSMessage(ind, 0);
        }


        prevjoyReport.button[ind] = joyReport.button[ind];

        if (Reflector_In_Use == 1) {
          udp.beginPacket(reflectorIP, reflectorport);
          udp.println("Left Composite Input - " + String(ind) + ":" + String(joyReport.button[ind]));
          udp.endPacket();
        }
      }
    }
}

void SendIPMessage(int ind, int state) {

  String outString;
  outString = String(ind) + ":" + String(state);

  udp.beginPacket(reflectorIP, reflectorport);
  udp.print(outString);
  udp.endPacket();


  //  udp.beginPacket(targetIP, remoteport);
  //  udp.print(outString);
  //  udp.endPacket();
}

void SendMSFSMessage(int ind, int state) {

  String outString;
  outString = "D" + String(INPUT_MODULE_NUMBER) + "," + String(ind) + ":" + String(state);

  udp.beginPacket(MSFSIP, MSFSport);
  udp.print(outString);
  udp.endPacket();


  //  udp.beginPacket(targetIP, remoteport);
  //  udp.print(outString);
  //  udp.endPacket();
}


void SendIPString(String KeysToSend) {
  // Used to Send Desired Keystrokes to Due acting as Keyboard
  if (Ethernet_In_Use == 1) {

    if (Reflector_In_Use == 1) {
      udp.beginPacket(reflectorIP, reflectorport);
      udp.print("Sending Key Strokes " + KeysToSend);
      udp.endPacket();
    }
    udp.beginPacket(targetIP, keyboardport);
    udp.print(KeysToSend);
    udp.endPacket();
  }
}

void sendToDcsBiosMessage(const char *msg, const char *arg) {


  if (Reflector_In_Use == 1) {
    udp.beginPacket(reflectorIP, reflectorport);
    udp.println("Left Input - " + String(msg) + ":" + String(arg));
    udp.endPacket();
  }

  sendDcsBiosMessage(msg, arg);
}


#define DCS_CheckInterval 100  // Delay before considering DCS is sleeping/paused/not running
bool DCS_On = false;
long nextDCSmillisCheck = 0;
unsigned int DCS_Counter;
unsigned int last_DCS_Counter;

void onUpdateCounterChange(unsigned int newValue) {
  DCS_Counter = newValue;
}
DcsBios::IntegerBuffer UpdateCounterBuffer(0xfffe, 0x00ff, 0, onUpdateCounterChange);


void checkDCSActive() {

  if (millis() >= nextDCSmillisCheck) {

    // onUpdateCounterChange is updaed by DCS BIOS in onUpdateCounterChange
    if (DCS_Counter != last_DCS_Counter) {
      // Counter has changed so DCS is alive
      if (DCS_On == false) {
        // We have had a transition
        SendDebug("DCS has become active");
        digitalWrite(Check_LED_G, true);
      }
      DCS_On = true;
    } else {
      if (DCS_On == true) {
        SendDebug("DCS has become inactive");
        digitalWrite(Check_LED_G, false);
      }
      DCS_On = false;
    }

    last_DCS_Counter = DCS_Counter;
    nextDCSmillisCheck = millis() + DCS_CheckInterval;
  }
}

int CurrentVHFAMPreset = 1;
int LastVHFAMPresetSwitchPos = 1;

void setVHFAMPreset(int PresetSwitchPos) {

  /*
  VHF Preset in the A10 has positions 1 to 24. To give a similar
  feel, the 12 position rotary switch has had its stop removed
  so it can rotate with stops.  The logic takes the positions
  3,15,27,39,52,63 and provides an encoder like experience. These
  map to positions 1 to 6

  Once the preset value hits the limits it will no longer
  decrement or increment.

  LastVHFAMPresetSwitchPos is the physical 'encoder' position
  as opposed to the CurrentVHFAMPreset which is the Sims position

  */

  bool IncrementPos = false;
  bool DecrementPos = false;

  // First deal with exceptions
  if (LastVHFAMPresetSwitchPos == 6 && PresetSwitchPos == 1) {
    IncrementPos = true;
  } else if (LastVHFAMPresetSwitchPos == 1 && PresetSwitchPos == 6) {
    DecrementPos = true;
    // Now deal with general cases
  } else if (PresetSwitchPos > LastVHFAMPresetSwitchPos) {
    IncrementPos = true;
  } else if (PresetSwitchPos < LastVHFAMPresetSwitchPos) {
    DecrementPos = true;
  }
  LastVHFAMPresetSwitchPos = PresetSwitchPos;

  if (CurrentVHFAMPreset < 20 && IncrementPos == true)
    CurrentVHFAMPreset++;
  else if ((CurrentVHFAMPreset > 1 && DecrementPos == true))
    CurrentVHFAMPreset--;

  update_VHF_AM_PRESET_OLED(String(CurrentVHFAMPreset));
  update_VHF_AM_PRESET_TARGET(CurrentVHFAMPreset);
  // targetVhffmPresetString = String(CurrentVHFAMPreset);
}


int CurrentVHFFMPreset = 1;
int LastVHFFMPresetSwitchPos = 1;

void setVHFFMPreset(int PresetSwitchPos) {

  /*
  VHF Preset in the A10 has positions 1 to 24. To give a similar
  feel, the 12 position rotary switch has had its stop removed
  so it can rotate with stops.  The logic takes the positions
  3,15,27,39,52,63 and provides an encoder like experience. These
  map to positions 1 to 6

  Once the preset value hits the limits it will no longer
  decrement or increment.

  LastVHFAMPresetSwitchPos is the physical 'encoder' position
  as opposed to the CurrentVHFAMPreset which is the Sims position

  */

  bool IncrementPos = false;
  bool DecrementPos = false;

  // First deal with exceptions
  if (LastVHFFMPresetSwitchPos == 6 && PresetSwitchPos == 1) {
    IncrementPos = true;
  } else if (LastVHFFMPresetSwitchPos == 1 && PresetSwitchPos == 6) {
    DecrementPos = true;
    // Now deal with general cases
  } else if (PresetSwitchPos > LastVHFFMPresetSwitchPos) {
    IncrementPos = true;
  } else if (PresetSwitchPos < LastVHFFMPresetSwitchPos) {
    DecrementPos = true;
  }
  LastVHFFMPresetSwitchPos = PresetSwitchPos;

  if (CurrentVHFFMPreset < 20 && IncrementPos == true)
    CurrentVHFFMPreset++;
  else if ((CurrentVHFFMPreset > 1 && DecrementPos == true))
    CurrentVHFFMPreset--;

  update_VHF_FM_PRESET_OLED(String(CurrentVHFFMPreset));
  update_VHF_FM_PRESET_TARGET(CurrentVHFFMPreset);
}


int CurrentUHFPreset = 1;
int LastUHFPresetSwitchPos = 1;

void setUHFPreset(int PresetSwitchPos) {

  /*
  UHF Preset in the A10 has positions 1 to 24. To give a similar
  feel, the 12 position rotary switch has had its stop removed
  so it can rotate with stops.  The logic takes the positions
  3,15,27,39,52,63 and provides an encoder like experience. These
  map to positions 1 to 6

  Once the preset value hits the limits it will no longer
  decrement or increment.

  LastVHFAMPresetSwitchPos is the physical 'encoder' position
  as opposed to the CurrentVHFAMPreset which is the Sims position

  */

  bool IncrementPos = false;
  bool DecrementPos = false;

  // First deal with exceptions
  if (LastUHFPresetSwitchPos == 6 && PresetSwitchPos == 1) {
    IncrementPos = true;
  } else if (LastUHFPresetSwitchPos == 1 && PresetSwitchPos == 6) {
    DecrementPos = true;
    // Now deal with general cases
  } else if (PresetSwitchPos > LastUHFPresetSwitchPos) {
    IncrementPos = true;
  } else if (PresetSwitchPos < LastUHFPresetSwitchPos) {
    DecrementPos = true;
  }
  LastUHFPresetSwitchPos = PresetSwitchPos;

  if (CurrentUHFPreset < 20 && IncrementPos == true)
    CurrentUHFPreset++;
  else if ((CurrentUHFPreset > 0 && DecrementPos == true))
    CurrentUHFPreset--;


  String strCurrentUHFPreset = String(CurrentUHFPreset);
  SendDebug("UHF Preset :" + strCurrentUHFPreset);
  sendToDcsBiosMessage("UHF_PRESET_SEL", strCurrentUHFPreset.c_str());
}

/*
"VHFAM_PRESET", { -0.01, 0.01 }, 137, 0.01, { 0, 0.20 }, { " 1", " 2", " 3", " 4", " 5", " 6", " 7", " 8", " 9", "10", "11", "12", "13", "14", "15", "16", "17", "18", "19", "20" }, "VHF AM Radio", "Preset Channel Selector")
"VHFAM_FREQ1",  { -0.1, 0.1 }, 143, 0.05, { 0.15, 0.80 }, { " 3", " 4", " 5", " 6", " 7", " 8", " 9", "10", "11", "12", "13", "14", "15" }, "VHF AM Radio", "Frequency Selector 1")
where is freq2?
"VHFAM_FREQ3",  { -0.1, 0.1 }, 145, 0.1, { 0, 1 }, nil, "VHF AM Radio", "Frequency Selector 3")
"VHFAM_FREQ4", { -0.25, 0.25 }, 146, 0.25, { 0, 1 }, { "00", "25", "50", "75" }, "VHF AM Radio", "Frequency Selector 4")
"VHFFM_PRESET", { -0.01, 0.01 }, 151, 0.01, { 0, 0.20 }, { " 1", " 2", " 3", " 4", " 5", " 6", " 7", " 8", " 9", "10", "11", "12", "13", "14", "15", "16", "17", "18", "19", "20" }, "VHF FM Radio", "Preset Channel Selector")
"VHFFM_FREQ1", { -0.1, 0.1 }, 157, 0.05, { 0.15, 0.80 }, { " 3", " 4", " 5", " 6", " 7", " 8", " 9", "10", "11", "12", "13", "14", "15" }, "VHF FM Radio", "Frequency Selector 1")
"VHFFM_FREQ2", { -0.1, 0.1 }, 158, 0.1, { 0, 1 }, nil, "VHF FM Radio", "Frequency Selector 2")
"VHFFM_FREQ3", { -0.1, 0.1 }, 159, 0.1, { 0, 1 }, nil, "VHF FM Radio", "Frequency Selector 3")
"VHFFM_FREQ4", { -0.25, 0.25 }, 160, 0.25, { 0, 1 }, { "00", "25", "50", "75" }, "VHF FM Radio", "Frequency Selector 4")


"UHF_PRESET_SEL", { 0, 1 }, { " 1", " 2", " 3", " 4", " 5", " 6", " 7", " 8", " 9", "10", "11", "12", "13", "14", "15", "16", "17", "18", "19", "20" }, false, "UHF Radio", "UHF Preset Channel Selector")
"UHF_100MHZ_SEL", { 0, 0.2 }, { "2", "3", "A" }, false, "UHF Radio", "UHF 100MHz Selector")
"UHF_10MHZ_SEL", { 0, 0.9 }, nil, false, "UHF Radio", "UHF 10MHz Selector")
"UHF_1MHZ_SEL", { 0, 0.9 }, nil, false, "UHF Radio", "UHF 1MHz Selector")
"UHF_POINT1MHZ_SEL", { 0, 0.9 }, nil, false, "UHF Radio", "UHF 0.1MHz Selector")
"UHF_POINT25_SEL", { 0, 0.3 }, { "00", "25", "50", "75" }, false, "UHF Radio", "UHF 0.25MHz Selector")

*/

#define delayAfterSendingInstruction 50

String targetVhffmFreq1String = "12";
String currentVhffmFreq1String = "";
#define selectorVhffmFreq1SIZE 13
char *selectorVhffmFreq1[] = { " 3", " 4", " 5", " 6", " 7", " 8", " 9", "10", "11", "12", "13", "14", "15" };

void onVhffmFreq1Change(char *newValue) {
  SendDebug("VHF FM Frequency Change");
  currentVhffmFreq1String = String(newValue);
}
DcsBios::StringBuffer<2> vhffmFreq1StrBuffer(0x119a, onVhffmFreq1Change);

void checkVHFFMFreq1() {
  if (currentVhffmFreq1String != targetVhffmFreq1String) {
    SendDebug("Incrementing currentVhffmFreq1String");

    SendDebug("Current currentVhffmFreq1String " + currentVhffmFreq1String);

    int currentPos = 0;
    int targetPos = 0;
    int deltaPos = 0;
    bool foundCurrent = false;
    bool foundTarget = false;
    for (int i = 0; i < selectorVhffmFreq1SIZE; i++) {
      SendDebug("Walking Array for current :" + String(i));
      SendDebug(String(selectorVhffmFreq1[i]) + ":" + currentVhffmFreq1String);
      if (String(selectorVhffmFreq1[i]) == currentVhffmFreq1String) {
        SendDebug("CurrentReadingString Postion in array :" + String(i));
        currentPos = i;
        foundCurrent = true;
        break;
      }
    }

    for (int i = 0; i < selectorVhffmFreq1SIZE; i++) {
      // SendDebug("Walking Array for target :" + String(i));
      // SendDebug(String(selectorVhffmFreq1[i]) + ":" + currentVhffmFreq1String );
      if (String(selectorVhffmFreq1[i]) == targetVhffmFreq1String) {
        // SendDebug("targetVhffmFreq1String Postion in array :" + String(i));
        targetPos = i;
        foundTarget = true;
        break;
      }
    }

    if (foundCurrent == false) {
      SendDebug("WARNING UNABLE TO FIND CURRENT POSITION IN ARRAY");
    }
    if (foundTarget == false) {
      SendDebug("WARNING UNABLE TO FIND TARGET POSITION IN ARRAY");
    }

    deltaPos = targetPos - currentPos;

    if (deltaPos > 0) {
      sendToDcsBiosMessage("VHFFM_FREQ1", "INC");
    } else {
      sendToDcsBiosMessage("VHFFM_FREQ1", "DEC");
    }

    SendDebug("Current (" + String(currentPos) + ") and target (" + String(targetPos) + ") Delta :" + String(currentPos - targetPos));
  }
}

// ##################################################################################################
String targetVhffmFreq2String = "0";
String currentVhffmFreq2String = "";
#define selectorVhffmFreq2SIZE 10
char *selectorVhffmFreq2[] = { "0", "1", "2", "3", "4", "5", "6", "7", "8", "9" };


void onVhffmFreq2Change(unsigned int newValue) {
  SendDebug("VHF FM Frequency 2 Change");
  currentVhffmFreq2String = String(newValue);
}
DcsBios::IntegerBuffer vhffmFreq2Buffer(0x119c, 0x000f, 0, onVhffmFreq2Change);

void checkVHFFMFreq2() {
  if (currentVhffmFreq2String != targetVhffmFreq2String) {
    SendDebug("Incrementing currentVhffmFreq2String");

    SendDebug("Current currentVhffmFreq2String " + currentVhffmFreq2String);

    int currentPos = 0;
    int targetPos = 0;
    int deltaPos = 0;
    bool foundCurrent = false;
    bool foundTarget = false;
    for (int i = 0; i < selectorVhffmFreq2SIZE; i++) {
      SendDebug("Walking Array for current :" + String(i));
      SendDebug(String(selectorVhffmFreq2[i]) + ":" + currentVhffmFreq2String);
      if (String(selectorVhffmFreq2[i]) == currentVhffmFreq2String) {
        SendDebug("currentRadingString Postion in array :" + String(i));
        currentPos = i;
        foundCurrent = true;
        break;
      }
    }

    for (int i = 0; i < selectorVhffmFreq2SIZE; i++) {
      // SendDebug("Walking Array for target :" + String(i));
      // SendDebug(String(selectorVhffmFreq1[i]) + ":" + currentVhffmFreq2String );
      if (String(selectorVhffmFreq2[i]) == targetVhffmFreq2String) {
        // SendDebug("targetVhffmFreq2String Postion in array :" + String(i));
        targetPos = i;
        foundTarget = true;
        break;
      }
    }

    if (foundCurrent == false) {
      SendDebug("WARNING UNABLE TO FIND CURRENT POSITION IN ARRAY");
    }
    if (foundTarget == false) {
      SendDebug("WARNING UNABLE TO FIND TARGET POSITION IN ARRAY");
    }

    deltaPos = targetPos - currentPos;

    if (deltaPos > 0) {
      sendToDcsBiosMessage("VHFFM_FREQ2", "INC");
    } else {
      sendToDcsBiosMessage("VHFFM_FREQ2", "DEC");
    }

    SendDebug("Current (" + String(currentPos) + ") and target (" + String(targetPos) + ") Delta :" + String(currentPos - targetPos));
  }
}

// ##################################################################################################

// ##################################################################################################
String targetVhffmFreq3String = "0";
String currentVhffmFreq3String = "";
#define selectorVhffmFreq3SIZE 10
char *selectorVhffmFreq3[] = { "0", "1", "2", "3", "4", "5", "6", "7", "8", "9" };


void onVhffmFreq3Change(unsigned int newValue) {
  SendDebug("VHF FM Frequency 3 Change");
  currentVhffmFreq3String = String(newValue);
}
DcsBios::IntegerBuffer vhffmFreq3Buffer(0x119c, 0x00f0, 4, onVhffmFreq3Change);


void checkVHFFMFreq3() {
  if (currentVhffmFreq3String != targetVhffmFreq3String) {
    SendDebug("Incrementing currentVhffmFreq3String");

    SendDebug("Current currentVhffmFreq3String " + currentVhffmFreq3String);

    int currentPos = 0;
    int targetPos = 0;
    int deltaPos = 0;
    bool foundCurrent = false;
    bool foundTarget = false;
    for (int i = 0; i < selectorVhffmFreq3SIZE; i++) {
      SendDebug("Walking Array for current :" + String(i));
      SendDebug(String(selectorVhffmFreq3[i]) + ":" + currentVhffmFreq3String);
      if (String(selectorVhffmFreq3[i]) == currentVhffmFreq3String) {
        SendDebug("currentRadingString Postion in array :" + String(i));
        currentPos = i;
        foundCurrent = true;
        break;
      }
    }

    for (int i = 0; i < selectorVhffmFreq3SIZE; i++) {
      // SendDebug("Walking Array for target :" + String(i));
      // SendDebug(String(selectorVhffmFreq1[i]) + ":" + currentVhffmFreq3String );
      if (String(selectorVhffmFreq3[i]) == targetVhffmFreq3String) {
        // SendDebug("targetVhffmFreq3String Postion in array :" + String(i));
        targetPos = i;
        foundTarget = true;
        break;
      }
    }

    if (foundCurrent == false) {
      SendDebug("WARNING UNABLE TO FIND CURRENT POSITION IN ARRAY");
    }
    if (foundTarget == false) {
      SendDebug("WARNING UNABLE TO FIND TARGET POSITION IN ARRAY");
    }

    deltaPos = targetPos - currentPos;

    if (deltaPos > 0) {
      sendToDcsBiosMessage("VHFFM_FREQ3", "INC");
    } else {
      sendToDcsBiosMessage("VHFFM_FREQ3", "DEC");
    }

    SendDebug("Current (" + String(currentPos) + ") and target (" + String(targetPos) + ") Delta :" + String(currentPos - targetPos));
  }
}

// ##################################################################################################

// ##################################################################################################
String targetVhffmFreq4String = "0";
String currentVhffmFreq4String = "";
#define selectorVhffmFreq4SIZE 4
char *selectorVhffmFreq4[] = { "0", "1", "2", "3" };



void onVhffmFreq4Change(unsigned int newValue) {

  currentVhffmFreq4String = String(newValue);
  SendDebug("VHF FM Frequency 4 Change :" + currentVhffmFreq4String);
}
DcsBios::IntegerBuffer vhffmFreq4Buffer(0x119c, 0x0300, 8, onVhffmFreq4Change);


void checkVHFFMFreq4() {
  if (currentVhffmFreq4String != targetVhffmFreq4String) {
    SendDebug("Incrementing currentVhffmFreq4String");

    SendDebug("Current currentVhffmFreq4String " + currentVhffmFreq4String);

    int currentPos = 0;
    int targetPos = 0;
    int deltaPos = 0;
    bool foundCurrent = false;
    bool foundTarget = false;
    for (int i = 0; i < selectorVhffmFreq4SIZE; i++) {
      SendDebug("Walking Array for current :" + String(i));
      SendDebug(String(selectorVhffmFreq4[i]) + ":" + currentVhffmFreq4String);
      if (String(selectorVhffmFreq4[i]) == currentVhffmFreq4String) {
        SendDebug("currentRadingString Postion in array :" + String(i));
        currentPos = i;
        foundCurrent = true;
        break;
      }
    }

    for (int i = 0; i < selectorVhffmFreq4SIZE; i++) {
      if (String(selectorVhffmFreq4[i]) == targetVhffmFreq4String) {
        targetPos = i;
        foundTarget = true;
        break;
      }
    }

    if (foundCurrent == false) {
      SendDebug("WARNING UNABLE TO FIND CURRENT POSITION IN ARRAY");
    }
    if (foundTarget == false) {
      SendDebug("WARNING UNABLE TO FIND TARGET POSITION IN ARRAY");
    }

    deltaPos = targetPos - currentPos;

    if (deltaPos > 0) {
      sendToDcsBiosMessage("VHFFM_FREQ4", "INC");
      delay(delayAfterSendingInstruction);
    } else {
      sendToDcsBiosMessage("VHFFM_FREQ4", "DEC");
      delay(delayAfterSendingInstruction);
    }

    SendDebug("Current (" + String(currentPos) + ") and target (" + String(targetPos) + ") Delta :" + String(currentPos - targetPos));
  }
}


// ##################################################################################################


// ##################################################################################################
String targetVhffmPresetString = " 1";
String currentVhffmPresetString = "";
#define selectorVhffmPresetSIZE 20
char *selectorVhffmPreset[] = { " 1", " 2", " 3", " 4", " 5", " 6", " 7", " 8", " 9", "10", "11", "12", "13", "14", "15", "16", "17", "18", "19", "20" };

void onVhffmPresetChange(char *newValue) {
  currentVhffmPresetString = String(newValue);
  // SendDebug("VHF FM Preset Change :" + currentVhffmPresetString);
}
DcsBios::StringBuffer<2> vhffmPresetStrBuffer(0x1196, onVhffmPresetChange);

void checkVHFFMPreset() {
  if (currentVhffmPresetString != targetVhffmPresetString) {
    SendDebug("Incrementing currentVhffmPresetString");

    SendDebug("Current currentVhffmPresetString " + currentVhffmPresetString);

    int currentPos = 0;
    int targetPos = 0;
    int deltaPos = 0;
    bool foundCurrent = false;
    bool foundTarget = false;
    for (int i = 0; i < selectorVhffmPresetSIZE; i++) {
      SendDebug("Walking Array for current :" + String(i));
      SendDebug(String(selectorVhffmPreset[i]) + ":" + currentVhffmPresetString);
      if (String(selectorVhffmPreset[i]) == currentVhffmPresetString) {
        SendDebug("currentRadingString Postion in array :" + String(i));
        currentPos = i;
        foundCurrent = true;
        break;
      }
    }

    for (int i = 0; i < selectorVhffmPresetSIZE; i++) {
      if (String(selectorVhffmPreset[i]) == targetVhffmPresetString) {
        targetPos = i;
        foundTarget = true;
        break;
      }
    }

    if (foundCurrent == false) {
      SendDebug("WARNING UNABLE TO FIND CURRENT POSITION IN ARRAY TARGET IS :" + targetVhffmPresetString);
    }
    if (foundTarget == false) {
      SendDebug("WARNING UNABLE TO FIND CURRENT POSITION IN ARRAY TARGET IS :" + targetVhffmPresetString);
    }

    deltaPos = targetPos - currentPos;

    if (deltaPos > 0) {
      sendToDcsBiosMessage("VHFFM_PRESET", "INC");
      delay(delayAfterSendingInstruction);
    } else {
      sendToDcsBiosMessage("VHFFM_PRESET", "DEC");
      delay(delayAfterSendingInstruction);
    }

    SendDebug("Current (" + String(currentPos) + ") and target (" + String(targetPos) + ") Delta :" + String(currentPos - targetPos));
  }
}

// ##################################################################################################


String targetVhfamFreq1String = "12";
String currentVhfamFreq1String = "";
#define selectorVhfamFreq1SIZE 13
char *selectorVhfamFreq1[] = { " 3", " 4", " 5", " 6", " 7", " 8", " 9", "10", "11", "12", "13", "14", "15" };

void onVhfamFreq1Change(char *newValue) {
  SendDebug("VHF AM Frequency Change");
  currentVhfamFreq1String = String(newValue);
}
DcsBios::StringBuffer<2> vhfamFreq1StrBuffer(0x1190, onVhfamFreq1Change);

void checkVHFAMFreq1() {
  if (currentVhfamFreq1String != targetVhfamFreq1String) {
    SendDebug("Incrementing currentVhfamFreq1String");

    SendDebug("Current currentVhfamFreq1String " + currentVhfamFreq1String);

    int currentPos = 0;
    int targetPos = 0;
    int deltaPos = 0;
    bool foundCurrent = false;
    bool foundTarget = false;
    for (int i = 0; i < selectorVhfamFreq1SIZE; i++) {
      SendDebug("Walking Array for current :" + String(i));
      SendDebug(String(selectorVhfamFreq1[i]) + ":" + currentVhfamFreq1String);
      if (String(selectorVhfamFreq1[i]) == currentVhfamFreq1String) {
        SendDebug("currentRadingString Postion in array :" + String(i));
        currentPos = i;
        foundCurrent = true;
        break;
      }
    }

    for (int i = 0; i < selectorVhfamFreq1SIZE; i++) {
      // SendDebug("Walking Array for target :" + String(i));
      // SendDebug(String(selectorVhfamFreq1[i]) + ":" + currentVhfamFreq1String );
      if (String(selectorVhfamFreq1[i]) == targetVhfamFreq1String) {
        // SendDebug("targetVhfamFreq1String Postion in array :" + String(i));
        targetPos = i;
        foundTarget = true;
        break;
      }
    }

    if (foundCurrent == false) {
      SendDebug("WARNING UNABLE TO FIND CURRENT POSITION IN ARRAY");
    }
    if (foundTarget == false) {
      SendDebug("WARNING UNABLE TO FIND TARGET POSITION IN ARRAY");
    }

    deltaPos = targetPos - currentPos;

    if (deltaPos > 0) {
      sendToDcsBiosMessage("VHFAM_FREQ1", "INC");
    } else {
      sendToDcsBiosMessage("VHFAM_FREQ1", "DEC");
    }

    SendDebug("Current (" + String(currentPos) + ") and target (" + String(targetPos) + ") Delta :" + String(currentPos - targetPos));
  }
}

// ##################################################################################################
String targetVhfamFreq2String = "0";
String currentVhfamFreq2String = "";
#define selectorVhfamFreq2SIZE 10
char *selectorVhfamFreq2[] = { "0", "1", "2", "3", "4", "5", "6", "7", "8", "9" };


void onVhfamFreq2Change(unsigned int newValue) {
  SendDebug("VHF FM Frequency 2 Change");
  currentVhfamFreq2String = String(newValue);
}
DcsBios::IntegerBuffer vhfamFreq2Buffer(0x118e, 0x00f0, 4, onVhfamFreq2Change);

void checkVHFAMFreq2() {
  if (currentVhfamFreq2String != targetVhfamFreq2String) {
    SendDebug("Incrementing currentVhfamFreq2String");

    SendDebug("Current currentVhfamFreq2String " + currentVhfamFreq2String);

    int currentPos = 0;
    int targetPos = 0;
    int deltaPos = 0;
    bool foundCurrent = false;
    bool foundTarget = false;
    for (int i = 0; i < selectorVhfamFreq2SIZE; i++) {
      SendDebug("Walking Array for current :" + String(i));
      SendDebug(String(selectorVhfamFreq2[i]) + ":" + currentVhfamFreq2String);
      if (String(selectorVhfamFreq2[i]) == currentVhfamFreq2String) {
        SendDebug("currentRadingString Postion in array :" + String(i));
        currentPos = i;
        foundCurrent = true;
        break;
      }
    }

    for (int i = 0; i < selectorVhfamFreq2SIZE; i++) {
      // SendDebug("Walking Array for target :" + String(i));
      // SendDebug(String(selectorVhfamFreq1[i]) + ":" + currentVhfamFreq2String );
      if (String(selectorVhfamFreq2[i]) == targetVhfamFreq2String) {
        // SendDebug("targetVhfamFreq2String Postion in array :" + String(i));
        targetPos = i;
        foundTarget = true;
        break;
      }
    }

    if (foundCurrent == false) {
      SendDebug("WARNING UNABLE TO FIND CURRENT POSITION IN ARRAY");
    }
    if (foundTarget == false) {
      SendDebug("WARNING UNABLE TO FIND TARGET POSITION IN ARRAY");
    }

    deltaPos = targetPos - currentPos;

    if (deltaPos > 0) {
      sendToDcsBiosMessage("VHFAM_FREQ2", "INC");
    } else {
      sendToDcsBiosMessage("VHFAM_FREQ2", "DEC");
    }

    SendDebug("Current (" + String(currentPos) + ") and target (" + String(targetPos) + ") Delta :" + String(currentPos - targetPos));
  }
}

// ##################################################################################################

// ##################################################################################################
String targetVhfamFreq3String = "0";
String currentVhfamFreq3String = "";
#define selectorVhfamFreq3SIZE 10
char *selectorVhfamFreq3[] = { "0", "1", "2", "3", "4", "5", "6", "7", "8", "9" };


void onVhfamFreq3Change(unsigned int newValue) {
  SendDebug("VHF FM Frequency 3 Change");
  currentVhfamFreq3String = String(newValue);
}
DcsBios::IntegerBuffer vhfamFreq3Buffer(0x118e, 0x0f00, 8, onVhfamFreq3Change);


void checkVHFAMFreq3() {
  if (currentVhfamFreq3String != targetVhfamFreq3String) {
    SendDebug("Incrementing currentVhfamFreq3String");

    SendDebug("Current currentVhfamFreq3String " + currentVhfamFreq3String);

    int currentPos = 0;
    int targetPos = 0;
    int deltaPos = 0;
    bool foundCurrent = false;
    bool foundTarget = false;
    for (int i = 0; i < selectorVhfamFreq3SIZE; i++) {
      SendDebug("Walking Array for current :" + String(i));
      SendDebug(String(selectorVhfamFreq3[i]) + ":" + currentVhfamFreq3String);
      if (String(selectorVhfamFreq3[i]) == currentVhfamFreq3String) {
        SendDebug("currentRadingString Postion in array :" + String(i));
        currentPos = i;
        foundCurrent = true;
        break;
      }
    }

    for (int i = 0; i < selectorVhfamFreq3SIZE; i++) {
      // SendDebug("Walking Array for target :" + String(i));
      // SendDebug(String(selectorVhfamFreq1[i]) + ":" + currentVhfamFreq3String );
      if (String(selectorVhfamFreq3[i]) == targetVhfamFreq3String) {
        // SendDebug("targetVhfamFreq3String Postion in array :" + String(i));
        targetPos = i;
        foundTarget = true;
        break;
      }
    }

    if (foundCurrent == false) {
      SendDebug("WARNING UNABLE TO FIND CURRENT POSITION IN ARRAY");
    }
    if (foundTarget == false) {
      SendDebug("WARNING UNABLE TO FIND TARGET POSITION IN ARRAY");
    }

    deltaPos = targetPos - currentPos;

    if (deltaPos > 0) {
      sendToDcsBiosMessage("VHFAM_FREQ3", "INC");
    } else {
      sendToDcsBiosMessage("VHFAM_FREQ3", "DEC");
    }

    SendDebug("Current (" + String(currentPos) + ") and target (" + String(targetPos) + ") Delta :" + String(currentPos - targetPos));
  }
}

// ##################################################################################################

// ##################################################################################################
String targetVhfamFreq4String = "0";
String currentVhfamFreq4String = "";
#define selectorVhfamFreq4SIZE 4
char *selectorVhfamFreq4[] = { "0", "1", "2", "3" };



void onVhfamFreq4Change(unsigned int newValue) {
  SendDebug("VHF FM Frequency 4 Change");
  currentVhfamFreq4String = String(newValue);
}
DcsBios::IntegerBuffer vhfamFreq4Buffer(0x118e, 0x3000, 12, onVhfamFreq4Change);


void checkVHFAMFreq4() {
  if (currentVhfamFreq4String != targetVhfamFreq4String) {
    SendDebug("Incrementing currentVhfamFreq4String");

    SendDebug("Current currentVhfamFreq4String " + currentVhfamFreq4String);

    int currentPos = 0;
    int targetPos = 0;
    int deltaPos = 0;
    bool foundCurrent = false;
    bool foundTarget = false;
    for (int i = 0; i < selectorVhfamFreq4SIZE; i++) {
      SendDebug("Walking Array for current :" + String(i));
      SendDebug(String(selectorVhfamFreq4[i]) + ":" + currentVhfamFreq4String);
      if (String(selectorVhfamFreq4[i]) == currentVhfamFreq4String) {
        SendDebug("currentRadingString Postion in array :" + String(i));
        currentPos = i;
        foundCurrent = true;
        break;
      }
    }

    for (int i = 0; i < selectorVhfamFreq4SIZE; i++) {
      if (String(selectorVhfamFreq4[i]) == targetVhfamFreq4String) {
        targetPos = i;
        foundTarget = true;
        break;
      }
    }

    if (foundCurrent == false) {
      SendDebug("WARNING UNABLE TO FIND CURRENT POSITION IN ARRAY");
    }
    if (foundTarget == false) {
      SendDebug("WARNING UNABLE TO FIND TARGET POSITION IN ARRAY");
    }

    deltaPos = targetPos - currentPos;

    if (deltaPos > 0) {
      sendToDcsBiosMessage("VHFAM_FREQ4", "INC");
    } else {
      sendToDcsBiosMessage("VHFAM_FREQ4", "DEC");
    }

    SendDebug("Current (" + String(currentPos) + ") and target (" + String(targetPos) + ") Delta :" + String(currentPos - targetPos));
  }
}


// ##################################################################################################


// ##################################################################################################
String targetVhfamPresetString = " 1";
String currentVhfamPresetString = "";
#define selectorVhfamPresetSIZE 20
char *selectorVhfamPreset[] = { " 1", " 2", " 3", " 4", " 5", " 6", " 7", " 8", " 9", "10", "11", "12", "13", "14", "15", "16", "17", "18", "19", "20" };

void onVhfamPresetChange(char *newValue) {
  currentVhfamPresetString = String(newValue);
  // This was creating a lot of noise so disabled debug
  // SendDebug("VHF AM Preset Change :" + currentVhfamPresetString);
}

DcsBios::StringBuffer<2> vhfamPresetStrBuffer(0x118a, onVhfamPresetChange);

void checkVHFAMPreset() {
  if (currentVhfamPresetString != targetVhfamPresetString) {
    SendDebug("Incrementing currentVhfamPresetString");

    SendDebug("Current currentVhfamPresetString " + currentVhfamPresetString);

    int currentPos = 0;
    int targetPos = 0;
    int deltaPos = 0;
    bool foundCurrent = false;
    bool foundTarget = false;
    for (int i = 0; i < selectorVhfamPresetSIZE; i++) {
      SendDebug("Walking Array for current :" + String(i));
      SendDebug(String(selectorVhfamPreset[i]) + ":" + currentVhfamPresetString);
      if (String(selectorVhfamPreset[i]) == currentVhfamPresetString) {
        SendDebug("currentRadingString Postion in array :" + String(i));
        currentPos = i;
        foundCurrent = true;
        break;
      }
    }

    for (int i = 0; i < selectorVhfamPresetSIZE; i++) {
      if (String(selectorVhfamPreset[i]) == targetVhfamPresetString) {
        targetPos = i;
        foundTarget = true;
        break;
      }
    }

    if (foundCurrent == false) {
      SendDebug("WARNING UNABLE TO FIND CURRENT POSITION IN ARRAY TARGET IS :" + targetVhfamPresetString);
    }
    if (foundTarget == false) {
      SendDebug("WARNING UNABLE TO FIND CURRENT POSITION IN ARRAY TARGET IS :" + targetVhfamPresetString);
    }

    deltaPos = targetPos - currentPos;

    if (deltaPos > 0) {
      sendToDcsBiosMessage("VHFAM_PRESET", "INC");
      delay(delayAfterSendingInstruction);
    } else {
      sendToDcsBiosMessage("VHFAM_PRESET", "DEC");
      delay(delayAfterSendingInstruction);
    }

    SendDebug("Current (" + String(currentPos) + ") and target (" + String(targetPos) + ") Delta :" + String(currentPos - targetPos));
  }
}

// ##################################################################################################



void checkRadios() {
  checkVHFFMFreq1();
  checkVHFFMFreq2();
  checkVHFFMFreq3();
  checkVHFFMFreq4();
  checkVHFFMPreset();
  checkVHFAMFreq1();
  checkVHFAMFreq2();
  checkVHFAMFreq3();
  checkVHFAMFreq4();
  checkVHFAMPreset();
}

void update_VHF_AM_PRESET_TARGET(int target) {
  if (target <= 9) {
    targetVhfamPresetString = " " + String(target);
  } else {
    targetVhfamPresetString = String(target);
  }
}

void update_VHF_FM_PRESET_TARGET(int target) {
  if (target <= 9) {
    targetVhffmPresetString = " " + String(target);
  } else {
    targetVhffmPresetString = String(target);
  }
}



DcsBios::Potentiometer uhfVol("UHF_VOL", A2);
DcsBios::Potentiometer vhffmVol("VHFFM_VOL", A1);
DcsBios::Potentiometer vhfamVol("VHFAM_VOL", A0);
// DcsBios::PotentiometerEWMA<5, 128, 5> formationDimmer("FORMATION_DIMMER", 8);
//DcsBios::PotentiometerEWMA<5, 128, 5> vhffmVol("VHFFM_VOL", 0);

DcsBios::LED saspPitchSasR(0x1108, 0x2000, 2);
DcsBios::LED saspPitchSasL(0x1108, 0x1000, 3);
DcsBios::LED saspYawSasR(0x1108, 0x0800, 4);
DcsBios::LED saspYawSasL(0x1108, 0x0400, 5);

DcsBios::LED saspToTrim_1(0x110c, 0x0001, 14);
DcsBios::LED saspToTrim_2(0x110c, 0x0001, 15);

void createDcsBiosMessage(int ind, int state) {

  switch (state) {
      // CLOSE
    case 1:
      switch (ind) {
        case 0:
          targetVhfamFreq1String = " 3";
          break;
        case 1:
          targetVhfamFreq2String = "0";
          break;
        case 2:
          targetVhfamFreq3String = "0";
          break;
        case 3:
          // Have an overlap between these two
          //
          targetVhffmFreq1String = " 3";
          break;
        case 4:
          targetVhffmFreq2String = "0";
          break;
        // CLOSE
        case 5:
          targetVhffmFreq3String = "0";
          break;
        case 6:
          setVHFFMPreset(1);
          break;
        case 7:
          sendToDcsBiosMessage("UHF_100MHZ_SEL", "0");
          break;
        case 8:
          sendToDcsBiosMessage("UHF_10MHZ_SEL", "0");
          break;
        case 9:
          sendToDcsBiosMessage("UHF_1MHZ_SEL", "0");
          break;
        // CLOSE
        case 10:
          sendToDcsBiosMessage("UHF_POINT1MHZ_SEL", "0");
          break;
        case 11:
          setVHFAMPreset(1);
          break;
        case 12:
          targetVhfamFreq1String = " 4";
          break;
        case 13:
          targetVhfamFreq2String = "1";
          break;
        case 14:
          targetVhfamFreq3String = "1";
          break;
        // CLOSE
        case 15:
          //
          targetVhffmFreq1String = " 4";
          break;
        case 16:
          targetVhffmFreq2String = "1";
          break;
        case 17:
          targetVhffmFreq3String = "1";
          break;
        case 18:
          setVHFFMPreset(2);
          break;
        case 19:
          sendToDcsBiosMessage("UHF_100MHZ_SEL", "1");
          break;
        // CLOSE
        case 20:
          sendToDcsBiosMessage("UHF_10MHZ_SEL", "1");
          break;
        case 21:
          sendToDcsBiosMessage("UHF_1MHZ_SEL", "1");
          break;
        case 22:
          sendToDcsBiosMessage("UHF_POINT1MHZ_SEL", "1");
          break;
        case 23:
          setVHFAMPreset(2);
          break;
        case 24:
          targetVhfamFreq1String = " 5";
          break;
        // CLOSE
        case 25:
          targetVhfamFreq2String = "2";
          break;
        case 26:
          targetVhfamFreq3String = "2";
          break;
        case 27:
          //
          targetVhffmFreq1String = " 5";
          break;
        case 28:
          targetVhffmFreq2String = "2";
          break;
        case 29:
          targetVhffmFreq3String = "2";
          break;
        // CLOSE
        case 30:
          setVHFFMPreset(3);
          break;
        case 31:
          sendToDcsBiosMessage("UHF_100MHZ_SEL", "2");

          break;
        case 32:
          sendToDcsBiosMessage("UHF_10MHZ_SEL", "2");
          break;
        case 33:
          sendToDcsBiosMessage("UHF_1MHZ_SEL", "2");
          break;
        case 34:
          sendToDcsBiosMessage("UHF_POINT1MHZ_SEL", "2");
          break;
        // CLOSE
        case 35:
          setVHFAMPreset(3);
          break;
        case 36:
          targetVhfamFreq1String = " 6";
          break;
        case 37:
          targetVhfamFreq2String = "3";
          break;
        case 38:
          targetVhfamFreq3String = "3";
          break;
        case 39:
          //
          targetVhffmFreq1String = " 6";
          break;
        // CLOSE
        case 40:
          targetVhffmFreq2String = "3";
          break;
        case 41:
          targetVhffmFreq3String = "3";
          break;
        case 42:
          setVHFFMPreset(4);
          break;
        case 43:
          sendToDcsBiosMessage("UHF_POINT25_SEL", "0");
          break;
        case 44:
          sendToDcsBiosMessage("UHF_10MHZ_SEL", "3");
          break;
        // CLOSE
        case 45:
          sendToDcsBiosMessage("UHF_1MHZ_SEL", "3");
          break;
        case 46:
          sendToDcsBiosMessage("UHF_POINT1MHZ_SEL", "3");
          break;
        case 47:
          setVHFAMPreset(4);
          break;
        case 48:
          targetVhfamFreq1String = " 7";
          break;
        case 49:
          targetVhfamFreq2String = "4";
          break;
        // CLOSE
        case 50:
          targetVhfamFreq3String = "4";
          break;
        case 51:
          //
          targetVhffmFreq1String = " 7";
          break;
        case 52:
          targetVhffmFreq2String = "4";
          break;
        case 53:
          targetVhffmFreq3String = "4";
          break;
        case 54:
          setVHFFMPreset(5);
          break;
        // CLOSE
        case 55:
          sendToDcsBiosMessage("UHF_POINT25_SEL", "1");
          break;
        case 56:
          sendToDcsBiosMessage("UHF_10MHZ_SEL", "4");
          break;
        case 57:
          sendToDcsBiosMessage("UHF_1MHZ_SEL", "4");
          break;
        case 58:
          sendToDcsBiosMessage("UHF_POINT1MHZ_SEL", "4");
          break;
        case 59:
          setVHFAMPreset(5);
          break;
        // CLOSE
        case 60:
          targetVhfamFreq1String = " 8";
          break;
        case 61:
          targetVhfamFreq2String = "5";
          break;
        case 62:
          targetVhfamFreq3String = "5";
          break;
        case 63:
          //setVHFAMPreset(6);
          targetVhffmFreq1String = " 8";
          break;
        case 64:
          targetVhffmFreq2String = "5";
          break;
        // CLOSE
        case 65:
          targetVhffmFreq3String = "5";
          break;
        case 66:

          break;
        case 67:
          sendToDcsBiosMessage("UHF_POINT25_SEL", "2");
          break;
        case 68:
          sendToDcsBiosMessage("UHF_10MHZ_SEL", "5");
          break;
        case 69:
          sendToDcsBiosMessage("UHF_1MHZ_SEL", "5");
          break;
        // CLOSE
        case 70:
          sendToDcsBiosMessage("UHF_POINT1MHZ_SEL", "5");
          break;
        case 71:
          setVHFFMPreset(6);
          break;
        case 72:
          targetVhfamFreq1String = " 9";
          break;
        case 73:
          targetVhfamFreq2String = "6";
          break;
        case 74:
          targetVhfamFreq3String = "6";
          break;
        // CLOSE
        case 75:
          targetVhffmFreq1String = " 9";
          break;
        case 76:
          targetVhffmFreq2String = "6";
          break;
        case 77:
          targetVhffmFreq3String = "6";
          break;
        case 78:
          break;
        case 79:
          sendToDcsBiosMessage("UHF_POINT25_SEL", "3");
          break;
        // CLOSE
        case 80:
          sendToDcsBiosMessage("UHF_10MHZ_SEL", "6");
          break;
        case 81:
          sendToDcsBiosMessage("UHF_1MHZ_SEL", "6");
          break;
        case 82:
          sendToDcsBiosMessage("UHF_POINT1MHZ_SEL", "6");
          break;
        case 83:
          break;
        case 84:
          targetVhfamFreq1String = "10";
          break;
        // CLOSE
        case 85:
          targetVhfamFreq2String = "7";
          break;
        case 86:
          targetVhfamFreq3String = "7";
          break;
        case 87:
          targetVhffmFreq1String = "10";
          break;
        case 88:
          targetVhffmFreq2String = "7";
          break;
        case 89:
          targetVhffmFreq3String = "7";
          break;
        // CLOSE
        case 90:
          break;
        case 91:
          sendToDcsBiosMessage("UHF_FUNCTION", "0");
          break;
        case 92:
          sendToDcsBiosMessage("UHF_10MHZ_SEL", "7");
          break;
        case 93:
          sendToDcsBiosMessage("UHF_1MHZ_SEL", "7");
          break;
        case 94:
          sendToDcsBiosMessage("UHF_POINT1MHZ_SEL", "7");
          break;
        // CLOSE
        case 95:
          break;
        case 96:
          targetVhfamFreq1String = "11";
          break;
        case 97:
          targetVhfamFreq2String = "8";
          break;
        case 98:
          targetVhfamFreq3String = "8";
          break;
        case 99:
          targetVhffmFreq1String = "11";
          break;
        // CLOSE
        case 100:
          targetVhffmFreq2String = "8";
          break;
        case 101:
          targetVhffmFreq3String = "8";
          break;
        case 102:
          break;
        case 103:
          sendToDcsBiosMessage("UHF_FUNCTION", "1");
          break;
        case 104:
          sendToDcsBiosMessage("UHF_10MHZ_SEL", "8");
          break;
        // CLOSE
        case 105:
          sendToDcsBiosMessage("UHF_1MHZ_SEL", "8");
          break;
        case 106:
          sendToDcsBiosMessage("UHF_POINT1MHZ_SEL", "8");
          break;
        case 107:
          break;
        case 108:
          targetVhfamFreq1String = "12";
          break;
        case 109:
          targetVhfamFreq2String = "9";
          break;
        // CLOSE
        case 110:
          targetVhfamFreq3String = "9";
          break;
        case 111:
          targetVhffmFreq1String = "12";
          break;
        case 112:
          targetVhffmFreq2String = "9";
          break;
        case 113:
          targetVhffmFreq3String = "9";
          break;
        case 114:
          break;
        // CLOSE
        case 115:
          sendToDcsBiosMessage("UHF_FUNCTION", "2");
          break;
        case 116:
          sendToDcsBiosMessage("UHF_10MHZ_SEL", "9");
          break;
        case 117:
          sendToDcsBiosMessage("UHF_1MHZ_SEL", "9");
          break;
        case 118:
          sendToDcsBiosMessage("UHF_POINT1MHZ_SEL", "9");
          break;
        case 119:
          break;
        // CLOSE
        case 120:
          targetVhfamFreq1String = "13";
          break;
        case 121:
          sendToDcsBiosMessage("VHFAM_FREQEMER", "0");
          break;
        case 122:
          sendToDcsBiosMessage("VHFAM_MODE", "0");
          break;
        case 123:
          targetVhffmFreq1String = "13";
          break;
        case 124:
          sendToDcsBiosMessage("VHFFM_FREQEMER", "0");
          break;
        // CLOSE
        case 125:
          sendToDcsBiosMessage("VHFFM_MODE", "0");
          break;
        case 126:
          break;
        case 127:
          sendToDcsBiosMessage("UHF_FUNCTION", "3");
          break;
        case 128:
          sendToDcsBiosMessage("UHF_MODE", "0");
          break;
        case 129:
          sendToDcsBiosMessage("UHF_T_TONE", "0");
          break;
        // CLOSE
        case 130:
          setUHFPreset(1);
          break;
        case 131:
          break;
        case 132:
          targetVhfamFreq1String = "14";
          break;
        case 133:
          sendToDcsBiosMessage("VHFAM_FREQEMER", "1");
          break;
        case 134:
          sendToDcsBiosMessage("VHFAM_MODE", "1");
          break;
        // CLOSE
        case 135:
          targetVhffmFreq1String = "14";
          break;
        case 136:
          sendToDcsBiosMessage("VHFFM_FREQEMER", "1");
          break;
        case 137:
          sendToDcsBiosMessage("VHFFM_MODE", "1");
          break;
        case 138:
          break;
        case 139:
          break;
        // CLOSE
        case 140:
          sendToDcsBiosMessage("UHF_MODE", "1");
          break;
        case 141:
          sendToDcsBiosMessage("UHF_T_TONE", "2");
          break;
        case 142:
          setUHFPreset(2);
          break;
        case 143:
          break;
        case 144:
          targetVhfamFreq4String = "0";
          break;
        case 145:
          sendToDcsBiosMessage("VHFAM_FREQEMER", "2");
          break;
        case 146:
          sendToDcsBiosMessage("VHFAM_MODE", "2");
          break;
        case 147:
          targetVhffmFreq4String = "0";
          break;
        case 148:
          sendToDcsBiosMessage("VHFFM_FREQEMER", "2");
          break;
        case 149:
          sendToDcsBiosMessage("VHFFM_MODE", "2");
          break;
        // CLOSE
        case 150:
          break;
        case 151:
          break;
        case 152:
          sendToDcsBiosMessage("UHF_MODE", "2");
          break;
        case 153:
          sendToDcsBiosMessage("UHF_LOAD", "1");
          break;
        case 154:
          setUHFPreset(3);
          break;
        // CLOSE
        case 155:
          break;
        case 156:
          targetVhfamFreq4String = "1";
          break;
        case 157:
          sendToDcsBiosMessage("VHFAM_FREQEMER", "3");
          break;
        case 158:
          sendToDcsBiosMessage("VHFAM_LOAD", "1");
          break;
        case 159:
          targetVhffmFreq4String = "1";
          break;
        // CLOSE
        case 160:
          sendToDcsBiosMessage("VHFFM_FREQEMER", "3");
          break;
        case 161:
          sendToDcsBiosMessage("VHFFM_LOAD", "1");
          break;
        case 162:
          break;
        case 163:
          break;
        case 164:
          sendToDcsBiosMessage("UHF_TEST", "1");
          break;
        // CLOSE
        case 165:
          break;
        case 166:
          setUHFPreset(4);
          break;
        case 167:
          break;
        case 168:
          targetVhfamFreq4String = "2";
          break;
        case 169:
          sendToDcsBiosMessage("VHFAM_SQUELCH", "0");
          break;
        // CLOSE
        case 170:
          break;
        case 171:
          targetVhffmFreq4String = "2";
          break;
        case 172:
          sendToDcsBiosMessage("VHFFM_SQUELCH", "0");
          break;
        case 173:
          break;
        case 174:
          break;
        // CLOSE
        case 175:
          break;
        case 176:
          sendToDcsBiosMessage("UHF_STATUS", "1");
          break;
        case 177:
          break;
        case 178:
          setUHFPreset(5);
          break;
        case 179:
          break;
        // CLOSE
        case 180:
          targetVhfamFreq4String = "3";
          break;
        case 181:
          // Tone
          // Currently it isn't moving to desired position
          sendToDcsBiosMessage("VHFAM_SQUELCH", "2");
          break;
        case 182:
          break;
        case 183:
          targetVhffmFreq4String = "3";
          break;
        case 184:
          // Currently it isn't moving to desired position
          sendToDcsBiosMessage("VHFFM_SQUELCH", "2");
          break;
        // CLOSE
        case 185:
          break;
        case 186:
          break;
        case 187:
          break;
        case 188:
          sendToDcsBiosMessage("UHF_SQUELCH", "1");
          break;
        case 189:
          break;
        // CLOSE
        case 190:
          setUHFPreset(6);
          break;
        case 191:
          break;
        default:
          break;
      }
      break;
    // RELEASE
    case 0:
      switch (ind) {
        case 0:
          break;
        case 1:
          break;
        case 2:
          break;
        case 3:
          break;
        case 4:
          break;
        // RELEASE
        case 5:
          break;
        case 6:
          break;
        case 7:
          break;
        case 8:
          break;
        case 9:
          break;
        // RELEASE
        case 10:
          break;
        case 11:
          break;
        case 12:
          break;
        case 13:
          break;
        case 14:
          break;
        // RELEASE
        case 15:
          break;
        case 16:
          break;
        case 17:
          break;
        case 18:
          break;
        case 19:
          break;
        // RELEASE
        case 20:
          break;
        case 21:
          break;
        case 22:
          break;
        case 23:
          break;
        case 24:
          break;
        // RELEASE
        case 25:
          break;
        case 26:
          break;
        case 27:
          break;
        case 28:
          break;
        case 29:
          break;
        // RELEASE
        case 30:
          break;
        case 31:
          break;
        case 32:
          break;
        case 33:
          break;
        case 34:
          break;
        // RELEASE
        case 35:
          break;
        case 36:
          break;
        case 37:
          break;
        case 38:
          break;
        case 39:
          break;
        // RELEASE
        case 40:
          break;
        case 41:
          break;
        case 42:
          break;
        case 43:
          break;
        case 44:
          break;
        // RELEASE
        case 45:
          break;
        case 46:
          break;
        case 47:
          break;
        case 48:
          break;
        case 49:
          break;
        // RELEASE
        case 50:
          break;
        case 51:
          break;
        case 52:
          break;
        case 53:
          break;
        case 54:
          break;
        // RELEASE
        case 55:
          break;
        case 56:
          break;
        case 57:
          break;
        case 58:
          break;
        case 59:
          break;
        // RELEASE
        case 60:
          break;
        case 61:
          break;
        case 62:
          break;
        case 63:
          break;
        case 64:
          break;
        // RELEASE
        case 65:
          break;
        case 66:
          break;
        case 67:
          break;
        case 68:
          break;
        case 69:
          break;
        // RELEASE
        case 70:
          break;
        case 71:
          break;
        case 72:
          break;
        case 73:
          break;
        case 74:
          break;
        // RELEASE
        case 75:
          break;
        case 76:
          break;
        case 77:
          break;
        case 78:
          break;
        case 79:
          break;
        // RELEASE
        case 80:
          break;
        case 81:
          break;
        case 82:
          break;
        case 83:
          break;
        case 84:
          break;
        // RELEASE
        case 85:
          break;
        case 86:
          break;
        case 87:
          break;
        case 88:
          break;
        case 89:
          break;
        // RELEASE
        case 90:
          break;
        case 91:
          break;
        case 92:
          break;
        case 93:
          break;
        case 94:
          break;
        // RELEASE
        case 95:
          break;
        case 96:
          break;
        case 97:
          break;
        case 98:
          break;
        case 99:
          break;
        // RELEASE
        case 100:
          break;
        case 101:
          break;
        case 102:
          break;
        case 103:
          break;
        case 104:
          break;
        // RELEASE
        case 105:
          break;
        case 106:
          break;
        case 107:
          break;
        case 108:
          break;
        case 109:
          break;
        // RELEASE
        case 110:
          break;
        case 111:
          break;
        case 112:
          break;
        case 113:
          break;
        case 114:
          break;
        // RELEASE
        case 115:
          break;
        case 116:
          break;
        case 117:
          break;
        case 118:
          break;
        case 119:
          break;
        // RELEASE
        case 120:
          break;
        case 121:
          break;
        case 122:
          break;
        case 123:
          break;
        case 124:
          break;
        // RELEASE
        case 125:
          break;
        case 126:
          break;
        case 127:
          break;
        case 128:
          break;
        case 129:
          sendToDcsBiosMessage("UHF_T_TONE", "1");
          break;
        // RELEASE
        case 130:
          break;
        case 131:
          break;
        case 132:
          break;
        case 133:
          break;
        case 134:
          break;
        // RELEASE
        case 135:
          break;
        case 136:
          break;
        case 137:
          break;
        case 138:
          break;
        case 139:
          break;
        // RELEASE
        case 140:
          break;
        case 141:
          sendToDcsBiosMessage("UHF_T_TONE", "1");
          break;
        case 142:
          break;
        case 143:
          break;
        case 144:
          break;
        case 145:
          break;
        case 146:
          break;
        case 147:
          break;
        case 148:
          break;
        case 149:
          break;
        // RELEASE
        case 150:
          break;
        case 151:
          break;
        case 152:
          break;
        case 153:
          sendToDcsBiosMessage("UHF_LOAD", "0");
          break;
        case 154:
          break;
        // RELEASE
        case 155:
          break;
        case 156:
          break;
        case 157:
          break;
        case 158:
          sendToDcsBiosMessage("VHFAM_LOAD", "0");
          break;
        case 159:
          break;
        // RELEASE
        case 160:
          break;
        case 161:
          sendToDcsBiosMessage("VHFFM_LOAD", "0");
          break;
        case 162:
          break;
        case 163:
          break;
        case 164:
          sendToDcsBiosMessage("UHF_TEST", "0");
          break;
        // RELEASE
        case 165:
          break;
        case 166:
          break;
        case 167:
          break;
        case 168:
          break;
        case 169:
          sendToDcsBiosMessage("VHFAM_SQUELCH", "1");
          break;
        // RELEASE
        case 170:
          break;
        case 171:
          break;
        case 172:
          sendToDcsBiosMessage("VHFFM_SQUELCH", "1");
          break;
        case 173:
          break;
        case 174:
          break;
        // RELEASE
        case 175:
          break;
        case 176:
          sendToDcsBiosMessage("UHF_STATUS", "0");
          break;
        case 177:
          break;
        case 178:
          break;
        case 179:
          break;
        // RELEASE
        case 180:
          break;
        case 181:
          sendToDcsBiosMessage("VHFAM_SQUELCH", "1");
          break;
        case 182:
          break;
        case 183:
          break;
        case 184:
          sendToDcsBiosMessage("VHFFM_SQUELCH", "1");
          break;
        // RELEASE
        case 185:
          break;
        case 186:
          break;
        case 187:
          break;
        case 188:
          sendToDcsBiosMessage("UHF_SQUELCH", "0");
          break;
        case 189:
          break;
        // RELEASE
        case 190:
          break;
        case 191:
          break;
        default:
          break;
      }
      break;
  }
}





void loop() {


  if (millis() >= NEXT_STATUS_TOGGLE_TIMER) {
    RED_LED_STATE = !RED_LED_STATE;
    // digitalWrite(Check_LED_G, RED_LED_STATE);
    digitalWrite(Check_LED_R, RED_LED_STATE);
    digitalWrite(Check_LED_G, !RED_LED_STATE);
    NEXT_STATUS_TOGGLE_TIMER = millis() + FLASH_TIME;
  }


  if (DCSBIOS_In_Use == 1) DcsBios::loop();

  //turn off all rows first
  for (int rowid = 0; rowid < 16; rowid++) {
    //turn on the current row

    if (rowid == 0)
      PORTC = 0xFF;
    if (rowid == 8)
      PORTA = 0xFF;

    if (rowid < 8) {
      // Shift 1 right  - this is actually pulling port down
      PORTA = ~(0x1 << rowid);
    } else {
      PORTC = ~(0x1 << (15 - rowid));
    }



    //we must have such a delay so the digital pin output can go LOW steadily,
    //without this delay, the row PIN will not 100% at LOW during yet,
    //so check the first column pin's value will return incorrect result.
    delayMicroseconds(ScanDelay);

    int colResult[16];
    // Reading upper pins
    //pin 38, PD7
    colResult[0] = (PIND & B10000000) == 0 ? 0 : 1;
    //pin 39, PG2
    colResult[1] = (PING & B00000100) == 0 ? 0 : 1;
    //pin 40, PG1
    colResult[2] = (PING & B00000010) == 0 ? 0 : 1;
    //pin 41, PG0
    colResult[3] = (PING & B00000001) == 0 ? 0 : 1;

    //pin 42, PL7
    colResult[4] = (PINL & B10000000) == 0 ? 0 : 1;
    //pin 43, PL6
    colResult[5] = (PINL & B01000000) == 0 ? 0 : 1;
    //pin 44, PL5
    colResult[6] = (PINL & B00100000) == 0 ? 0 : 1;
    //pin 45, PL4
    colResult[7] = (PINL & B00010000) == 0 ? 0 : 1;

    //pin 46, PL3
    colResult[8] = (PINL & B00001000) == 0 ? 0 : 1;
    //pin 47, PL2
    colResult[9] = (PINL & B00000100) == 0 ? 0 : 1;
    //pin 48, PL1
    colResult[10] = (PINL & B00000010) == 0 ? 0 : 1;
    //pin 49, PL0
    //pin 49 is not used on the PCB design - more a mistake than anything else as it is available for us
    colResult[11] = (PINL & B00000001) == 0 ? 0 : 1;

    // Unable to use pins 50-53 per the following
    //This is on digital pins 10, 11, 12, and 13 on the Uno and pins 50, 51, and 52 on the Mega.
    //On both boards, pin 10 is used to select the W5500 and pin 4 for the SD card. These pins cannot be used for general I/O.
    //On the Mega, the hardware SS pin, 53, is not used to select either the W5500 or the SD card,
    //pin 50, PB3
    //colResult[12] =(PINB & B00001000) == 0 ? 0 : 1;
    colResult[12] = 1;
    //pin 51, PB2
    //colResult[13] =(PINB & B00000100) == 0 ? 0 : 0;
    colResult[13] = 1;
    //pin 52, PB1
    //colResult[14] =(PINB & B00000010) == 0 ? 0 : 0;
    colResult[14] = 1;
    //pin 53, PB0
    //colResult[15] =(PINB & B00000001) == 0 ? 0 : 1;
    colResult[15] = 1;


    // There are 16 Columns per row, 12 Rows - gives a total of 192 possible inputs
    // Have left the arrays dimensioned as per original code - if CPU or Memory becomes scarce reduce array
    for (int colid = 0; colid < 16; colid++) {
      if (colResult[colid] == 0) {
        joyReport.button[(rowid * 12) + colid] = 1;
      } else {
        joyReport.button[(rowid * 12) + colid] = 0;
      }
    }
  }


  FindInputChanges();





  if (DCS_On == true) {
    checkRadios();
  }

  currentMillis = millis();
  checkDCSActive();
}

void CaseTemplate() {
  int state;
  int ind;

  switch (state) {

      // CLOSE
    case 1:
      switch (ind) {
        case 0:
          break;
        case 1:
          break;
        case 2:
          break;
        case 3:
          break;
        case 4:
          break;
        // CLOSE
        case 5:
          break;
        case 6:
          break;
        case 7:
          break;
        case 8:
          break;
        case 9:
          break;
        // CLOSE
        case 10:
          break;
        case 11:
          break;
        case 12:
          break;
        case 13:
          break;
        case 14:
          break;
        // CLOSE
        case 15:
          break;
        case 16:
          break;
        case 17:
          break;
        case 18:
          break;
        case 19:
          break;
        // CLOSE
        case 20:
          break;
        case 21:
          break;
        case 22:
          break;
        case 23:
          break;
        case 24:
          break;
        // CLOSE
        case 25:
          break;
        case 26:
          break;
        case 27:
          break;
        case 28:
          break;
        case 29:
          break;
        // CLOSE
        case 30:
          break;
        case 31:
          break;
        case 32:
          break;
        case 33:
          break;
        case 34:
          break;
        // CLOSE
        case 35:
          break;
        case 36:
          break;
        case 37:
          break;
        case 38:
          break;
        case 39:
          break;
        // CLOSE
        case 40:
          break;
        case 41:
          break;
        case 42:
          break;
        case 43:
          break;
        case 44:
          break;
        // CLOSE
        case 45:
          break;
        case 46:
          break;
        case 47:
          break;
        case 48:
          break;
        case 49:
          break;
        // CLOSE
        case 50:
          break;
        case 51:
          break;
        case 52:
          break;
        case 53:
          break;
        case 54:
          break;
        // CLOSE
        case 55:
          break;
        case 56:
          break;
        case 57:
          break;
        case 58:
          break;
        case 59:
          break;
        // CLOSE
        case 60:
          break;
        case 61:
          break;
        case 62:
          break;
        case 63:
          break;
        case 64:
          break;
        // CLOSE
        case 65:
          break;
        case 66:
          break;
        case 67:
          break;
        case 68:
          break;
        case 69:
          break;
        // CLOSE
        case 70:
          break;
        case 71:
          break;
        case 72:
          break;
        case 73:
          break;
        case 74:
          break;
        // CLOSE
        case 75:
          break;
        case 76:
          break;
        case 77:
          break;
        case 78:
          break;
        case 79:
          break;
        // CLOSE
        case 80:
          break;
        case 81:
          break;
        case 82:
          break;
        case 83:
          break;
        case 84:
          break;
        // CLOSE
        case 85:
          break;
        case 86:
          break;
        case 87:
          break;
        case 88:
          break;
        case 89:
          break;
        // CLOSE
        case 90:
          break;
        case 91:
          break;
        case 92:
          break;
        case 93:
          break;
        case 94:
          break;
        // CLOSE
        case 95:
          break;
        case 96:
          break;
        case 97:
          break;
        case 98:
          break;
        case 99:
          break;
        // CLOSE
        case 100:
          break;
        case 101:
          break;
        case 102:
          break;
        case 103:
          break;
        case 104:
          break;
        // CLOSE
        case 105:
          break;
        case 106:
          break;
        case 107:
          break;
        case 108:
          break;
        case 109:
          break;
        // CLOSE
        case 110:
          break;
        case 111:
          break;
        case 112:
          break;
        case 113:
          break;
        case 114:
          break;
        // CLOSE
        case 115:
          break;
        case 116:
          break;
        case 117:
          break;
        case 118:
          break;
        case 119:
          break;
        // CLOSE
        case 120:
          break;
        case 121:
          break;
        case 122:
          break;
        case 123:
          break;
        case 124:
          break;
        // CLOSE
        case 125:
          break;
        case 126:
          break;
        case 127:
          break;
        case 128:
          break;
        case 129:
          break;
        // CLOSE
        case 130:
          break;
        case 131:
          break;
        case 132:
          break;
        case 133:
          break;
        case 134:
          break;
        // CLOSE
        case 135:
          break;
        case 136:
          break;
        case 137:
          break;
        case 138:
          break;
        case 139:
          break;
        // CLOSE
        case 140:
          break;
        case 141:
          break;
        case 142:
          break;
        case 143:
          break;
        case 144:
          break;
        case 145:
          break;
        case 146:
          break;
        case 147:
          break;
        case 148:
          break;
        case 149:
          break;
        // CLOSE
        case 150:
          break;
        case 151:
          break;
        case 152:
          break;
        case 153:
          break;
        case 154:
          break;
        // CLOSE
        case 155:
          break;
        case 156:
          break;
        case 157:
          break;
        case 158:
          break;
        case 159:
          break;
        // CLOSE
        case 160:
          break;
        case 161:
          break;
        case 162:
          break;
        case 163:
          break;
        case 164:
          break;
        // CLOSE
        case 165:
          break;
        case 166:
          break;
        case 167:
          break;
        case 168:
          break;
        case 169:
          break;
        // CLOSE
        case 170:
          break;
        case 171:
          break;
        case 172:
          break;
        case 173:
          break;
        case 174:
          break;
        // CLOSE
        case 175:
          break;
        case 176:
          break;
        case 177:
          break;
        case 178:
          break;
        case 179:
          break;
        // CLOSE
        case 180:
          break;
        case 181:
          break;
        case 182:
          break;
        case 183:
          break;
        case 184:
          break;
        // CLOSE
        case 185:
          break;
        case 186:
          break;
        case 187:
          break;
        case 188:
          break;
        case 189:
          break;
        // CLOSE
        case 190:
          break;
        case 191:
          break;
        default:
          break;
      }
      break;

    // RELEASE
    case 0:
      switch (ind) {
        case 0:
          break;
        case 1:
          break;
        case 2:
          break;
        case 3:
          break;
        case 4:
          break;
        // RELEASE
        case 5:
          break;
        case 6:
          break;
        case 7:
          break;
        case 8:
          break;
        case 9:
          break;
        // RELEASE
        case 10:
          break;
        case 11:
          break;
        case 12:
          break;
        case 13:
          break;
        case 14:
          break;
        // RELEASE
        case 15:
          break;
        case 16:
          break;
        case 17:
          break;
        case 18:
          break;
        case 19:
          break;
        // RELEASE
        case 20:
          break;
        case 21:
          break;
        case 22:
          break;
        case 23:
          break;
        case 24:
          break;
        // RELEASE
        case 25:
          break;
        case 26:
          break;
        case 27:
          break;
        case 28:
          break;
        case 29:
          break;
        // RELEASE
        case 30:
          break;
        case 31:
          break;
        case 32:
          break;
        case 33:
          break;
        case 34:
          break;
        // RELEASE
        case 35:
          break;
        case 36:
          break;
        case 37:
          break;
        case 38:
          break;
        case 39:
          break;
        // RELEASE
        case 40:
          break;
        case 41:
          break;
        case 42:
          break;
        case 43:
          break;
        case 44:
          break;
        // RELEASE
        case 45:
          break;
        case 46:
          break;
        case 47:
          break;
        case 48:
          break;
        case 49:
          break;
        // RELEASE
        case 50:
          break;
        case 51:
          break;
        case 52:
          break;
        case 53:
          break;
        case 54:
          break;
        // RELEASE
        case 55:
          break;
        case 56:
          break;
        case 57:
          break;
        case 58:
          break;
        case 59:
          break;
        // RELEASE
        case 60:
          break;
        case 61:
          break;
        case 62:
          break;
        case 63:
          break;
        case 64:
          break;
        // RELEASE
        case 65:
          break;
        case 66:
          break;
        case 67:
          break;
        case 68:
          break;
        case 69:
          break;
        // RELEASE
        case 70:
          break;
        case 71:
          break;
        case 72:
          break;
        case 73:
          break;
        case 74:
          break;
        // RELEASE
        case 75:
          break;
        case 76:
          break;
        case 77:
          break;
        case 78:
          break;
        case 79:
          break;
        // RELEASE
        case 80:
          break;
        case 81:
          break;
        case 82:
          break;
        case 83:
          break;
        case 84:
          break;
        // RELEASE
        case 85:
          break;
        case 86:
          break;
        case 87:
          break;
        case 88:
          break;
        case 89:
          break;
        // RELEASE
        case 90:
          break;
        case 91:
          break;
        case 92:
          break;
        case 93:
          break;
        case 94:
          break;
        // RELEASE
        case 95:
          break;
        case 96:
          break;
        case 97:
          break;
        case 98:
          break;
        case 99:
          break;
        // RELEASE
        case 100:
          break;
        case 101:
          break;
        case 102:
          break;
        case 103:
          break;
        case 104:
          break;
        // RELEASE
        case 105:
          break;
        case 106:
          break;
        case 107:
          break;
        case 108:
          break;
        case 109:
          break;
        // RELEASE
        case 110:
          break;
        case 111:
          break;
        case 112:
          break;
        case 113:
          break;
        case 114:
          break;
        // RELEASE
        case 115:
          break;
        case 116:
          break;
        case 117:
          break;
        case 118:
          break;
        case 119:
          break;
        // RELEASE
        case 120:
          break;
        case 121:
          break;
        case 122:
          break;
        case 123:
          break;
        case 124:
          break;
        // RELEASE
        case 125:
          break;
        case 126:
          break;
        case 127:
          break;
        case 128:
          break;
        case 129:
          break;
        // RELEASE
        case 130:
          break;
        case 131:
          break;
        case 132:
          break;
        case 133:
          break;
        case 134:
          break;
        // RELEASE
        case 135:
          break;
        case 136:
          break;
        case 137:
          break;
        case 138:
          break;
        case 139:
          break;
        // RELEASE
        case 140:
          break;
        case 141:
          break;
        case 142:
          break;
        case 143:
          break;
        case 144:
          break;
        case 145:
          break;
        case 146:
          break;
        case 147:
          break;
        case 148:
          break;
        case 149:
          break;
        // RELEASE
        case 150:
          break;
        case 151:
          break;
        case 152:
          break;
        case 153:
          break;
        case 154:
          break;
        // RELEASE
        case 155:
          break;
        case 156:
          break;
        case 157:
          break;
        case 158:
          break;
        case 159:
          break;
        // RELEASE
        case 160:
          break;
        case 161:
          break;
        case 162:
          break;
        case 163:
          break;
        case 164:
          break;
        // RELEASE
        case 165:
          break;
        case 166:
          break;
        case 167:
          break;
        case 168:
          break;
        case 169:
          break;
        // RELEASE
        case 170:
          break;
        case 171:
          break;
        case 172:
          break;
        case 173:
          break;
        case 174:
          break;
        // RELEASE
        case 175:
          break;
        case 176:
          break;
        case 177:
          break;
        case 178:
          break;
        case 179:
          break;
        // RELEASE
        case 180:
          break;
        case 181:
          break;
        case 182:
          break;
        case 183:
          break;
        case 184:
          break;
        // RELEASE
        case 185:
          break;
        case 186:
          break;
        case 187:
          break;
        case 188:
          break;
        case 189:
          break;
        // RELEASE
        case 190:
          break;
        case 191:
          break;
        default:
          break;
      }
      break;
  }
}