//  Hornet UFC
// *************************************************************
// *************************************************************
// Board MEGA 256
// SN - 7543331363935150A1E0
// Location
// UIP (inside Upper Console)
// *************************************************************
// *************************************************************
// Current COM 11 - Check S/N First - Before any new Uploads
// *************************************************************
// *************************************************************
// The project intends to drive the OLED displays on a F18C Hornet Up Front Controller
//
// To do  - grab Analog 0 use that to locally to determine brightness - no real value in getting that from the Sim
//        - develop font based on 14 segment display
//
// The UFC has a large display on the top left hand corner, five mid sized display on the right hand side, and then two smaller
// displays at the bottom left and bottom right hand side.
//
// As a number of the same OLEDs are used, which the same target I2C addresses an I2C multiplexor is used, TCA9548A.
// The TCA9548A is an 8 Channel I2C switch.  It is possible for different devices to share a common host I2C bus.
// https://e2e.ti.com/blogs_/b/analogwire/archive/2015/10/15/how-to-simplify-i2c-tree-when-connecting-multiple-slaves-to-an-i2c-master
//
// Prototyping is being done with an Adafruit 2717 TCA9548A board.
// The 8 channels have their own SCL and SDA
// Had one weirdness I'm yet to understand - the SCL SDA appear to be reversed on the Adafruit outputs, the input pins
// is as epxected, but the outputs appeared to be swapped around.

// The initial test OLEDs have addresses of 0x3C - despite being labelled - 0x78 or 0x7A - selectable by soldering a jumper. I'm assuming these are the same SSD1306
// used in the Hornet Altimeter.
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


// Basic logic
// Initialise I2C Bus (wire)
// Iterate through each port on the Mux and list connected devices to serial port (simple troubleshooting aid), may use UDP later
// Initialise each display
// Wait for UDP updates
// during timeout grab and smooth analo 0 and get brightness for all displays

#define Ethernet_In_Use 1
#define DCSBIOS_In_Use 0
#define Reflector_In_Use 1


// Port 3 is used for either a channel or Scratchpad display
//#define Opt_OLED_Port_1 6
//#define Opt_OLED_Port_2 4
//#define Opt_OLED_Port_3 5
//#define Opt_OLED_Port_4 2
//#define Opt_OLED_Port_5 7

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
//#include "dseg14_v3.h"

#include <SPI.h>
#include <Wire.h>
#include <Ethernet.h>
#include <EthernetUdp.h>


// These local Mac and IP Address will be reassigned early in startup based on
// the device ID as set by address pins
byte mac[] = { 0x00, 0xDD, 0x3E, 0xCA, 0x36, 0x99 };
IPAddress ip(172, 16, 1, 100);
String strMyIP = "X.X.X.X";

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


#define STATUS_LED_PORT LED_BUILTIN
#define FLASH_TIME 500

unsigned long NEXT_STATUS_TOGGLE_TIMER = 0;
bool RED_LED_STATE = false;
unsigned long timeSinceRedLedChanged = 0;


// Scratch Pad OLED
U8G2_SSD1305_128X32_ADAFRUIT_F_HW_I2C u8g2_Scratch_Pad(U8G2_R2, 12);

// Op OLEDs
U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C u8g2_OPT1(U8G2_R0, /* reset=*/U8X8_PIN_NONE);
U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C u8g2_OPT2(U8G2_R0, /* reset=*/U8X8_PIN_NONE);
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


int CurrentDisplay = 0;
int Brightness = 0;
char buffer[20];  //plenty of space for the value of millis() plus a zero terminator

String strOpt1 = "";
String strOpt2 = "";
String strOpt3 = "";
String strOpt4 = "";
String strOpt5 = "";
String strComm1 = "";
String strComm2 = "";
String strScratchString1 = "";
String strScratchString2 = "";
String strScratchNumber = "";
String CombinedScratchString = "";

String LaststrOpt1 = "";
String LaststrOpt2 = "";
String LaststrOpt3 = "";
String LaststrOpt4 = "";
String LaststrOpt5 = "";
String LaststrComm1 = "";
String LaststrComm2 = "";
String LaststrScratchString1 = "";
String LaststrScratchString2 = "";
String LaststrScratchNumber = "";
String LastCombinedScratchString = "";

char* ptrtopass;

void tcaselect(uint8_t i) {
  if (i > 7) return;

  Wire.beginTransmission(TCAADDR);
  Wire.write(1 << i);
  Wire.endTransmission();
}

void SendDebug(String MessageToSend) {
  if ((Reflector_In_Use == 1) && (Ethernet_In_Use == 1)) {
    udp.beginPacket(reflectorIP, reflectorport);
    udp.println(MessageToSend);
    udp.endPacket();
  }
}

void setup() {
  //Serial.begin(115200);

  pinMode(STATUS_LED_PORT, OUTPUT);

  digitalWrite(STATUS_LED_PORT, true);
  delay(FLASH_TIME);
  digitalWrite(STATUS_LED_PORT, false);

  delay(FLASH_TIME);

  if (Ethernet_In_Use == 1) {
    Ethernet.begin(mac, ip);
    udp.begin(localport);
    SendDebug("Init UDP - " + strMyIP + " " + String(millis()) + "mS since reset.");
  }


  delay(500);
  Wire.begin();


  // Had to comment out these debugging messages as they created a conflict with the IRQ definition in DCS BIOS
  //Serial.println("\nScanning I2C Bus");

  for (uint8_t t = 0; t < 8; t++) {
    tcaselect(t);
    // Had to comment out these debugging messages as they created a conflict with the IRQ definition in DCS BIOS
    //Serial.print("TCA Port #"); Serial.println(t);

    for (uint8_t addr = 0; addr <= 127; addr++) {
      //if (addr == TCAADDR) continue;

      uint8_t data;
      if (!twi_writeTo(addr, &data, 0, 1, 1)) {
        // Had to comment out these debugging messages as they created a conflict with the IRQ definition in DCS BIOS
        SendDebug("Found I2C 0x");  //Serial.println(addr,HEX);
      }
    }
  }
  // Had to comment out these debugging messages as they created a conflict with the IRQ definition in DCS BIOS
  SendDebug("I2C scan complete");
  if (false) {

    // tcaselect(ScratchPad_OLED_PORT);
    // u8g2_Scratch_Pad.begin();
    // u8g2_Scratch_Pad.clearBuffer();
    // u8g2_Scratch_Pad.setFont(u8g2_DcsFontHornet4_BIOS_09_tf);
    // u8g2_Scratch_Pad.sendBuffer();


    // tcaselect(COM1_OLED_PORT);
    // u8g2_COM1.begin();
    // u8g2_COM1.clearBuffer();
    // u8g2_COM1.setFont(u8g2_DcsFontHornet4_BIOS_09_tf);
    // u8g2_COM1.sendBuffer();

    // tcaselect(COM2_OLED_PORT);
    // u8g2_COM2.begin();
    // u8g2_COM2.clearBuffer();
    // u8g2_COM2.setFont(u8g2_DcsFontHornet4_BIOS_09_tf);
    // u8g2_COM2.sendBuffer();
  }


  tcaselect(Opt_OLED_Port_1);
  u8g2_OPT1.begin();
  u8g2_OPT1.clearBuffer();
  u8g2_OPT1.setFont(u8g2_font_logisoso30_tn);
  //u8g2_OPT1.setFont(u8g2_font_inb30_mf);
  //u8g2_OPT1.setFont(u8g2_font_courB24_tn);
  // u8g2_DcsFontHornet4_BIOS_09_tf
  // u8g2_DcsFontHornet3_BIOS_09_tf
  //u8g2_OPT1.setFont(u8g2_DcsFontHornet3_BIOS_09_tf);
  u8g2_OPT1.sendBuffer();
  //updateOpt1("OPT 111");
  updateOpt1("111");
  delay(3000);




  if (false) {
    tcaselect(Opt_OLED_Port_2);
    u8g2_OPT2.begin();
    u8g2_OPT2.clearBuffer();
    u8g2_OPT2.setFont(u8g2_DcsFontHornet4_BIOS_09_tf);
    u8g2_OPT2.sendBuffer();

    tcaselect(Opt_OLED_Port_3);
    u8g2_OPT3.begin();
    u8g2_OPT3.clearBuffer();
    u8g2_OPT3.setFont(u8g2_DcsFontHornet4_BIOS_09_tf);
    u8g2_OPT3.sendBuffer();

    tcaselect(Opt_OLED_Port_4);
    u8g2_OPT4.begin();
    u8g2_OPT4.clearBuffer();
    u8g2_OPT4.setFont(u8g2_DcsFontHornet4_BIOS_09_tf);
    u8g2_OPT4.sendBuffer();

    tcaselect(Opt_OLED_Port_5);
    u8g2_OPT5.begin();
    u8g2_OPT5.clearBuffer();
    u8g2_OPT5.setFont(u8g2_DcsFontHornet4_BIOS_09_tf);
    u8g2_OPT5.sendBuffer();


    if (DCSBIOS_In_Use == 1) DcsBios::setup();
  }


  if (false) {
    updateOpt1("OPT 1");
    updateOpt2("OPT 2");
    updateOpt3("OPT 3");
    updateOpt4("OPT 4");
    updateOpt5("OPT 5");
    updateComm1("00");
    updateComm2("11");

    updateAllOfScratchpad("888", "-", "88-888");
    delay(8000);

    updateOpt1("");
    updateOpt2("");
    updateOpt3("");
    updateOpt4("");
    updateOpt5("");
    updateComm1("");
    updateComm2("");
    updateAllOfScratchpad("", "", "");
  }
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



void SendIPString(String state) {

  String outString;

  if (Ethernet_In_Use == 1) {
    outString = String(state);

    udp.beginPacket(targetIP, reflectorport);
    udp.print(outString);
    udp.endPacket();


    udp.beginPacket(targetIP, remoteport);
    udp.print(outString);
    udp.endPacket();
  }
}

void updateOpt1(String strnewValue) {

  const char* newValue = strnewValue.c_str();
  tcaselect(Opt_OLED_Port_1);
  u8g2_OPT1.setFontMode(0);
  u8g2_OPT1.setDrawColor(0);
  u8g2_OPT1.drawBox(0, 0, 128, 32);
  u8g2_OPT1.setDrawColor(1);
  u8g2_OPT1.drawStr(5, 32, newValue);
  u8g2_OPT1.sendBuffer();
}

void onUfcOptionDisplay1Change(char* newValue) {
  strOpt1 = String(newValue);
}
DcsBios::StringBuffer<4> ufcOptionDisplay1Buffer(0x7432, onUfcOptionDisplay1Change);


void updateOpt2(String strnewValue) {

  const char* newValue = strnewValue.c_str();
  tcaselect(Opt_OLED_Port_2);
  u8g2_OPT2.setFontMode(0);
  u8g2_OPT2.setDrawColor(0);
  u8g2_OPT2.drawBox(0, 0, 128, 32);
  u8g2_OPT2.setDrawColor(1);
  u8g2_OPT2.drawStr(5, 32, newValue);
  u8g2_OPT2.sendBuffer();
}



void onUfcOptionDisplay2Change(char* newValue) {
  strOpt2 = String(newValue);
}
DcsBios::StringBuffer<4> ufcOptionDisplay2Buffer(0x7436, onUfcOptionDisplay2Change);


void updateOpt3(String strnewValue) {
  const char* newValue = strnewValue.c_str();
  tcaselect(Opt_OLED_Port_3);
  u8g2_OPT3.setFontMode(0);
  u8g2_OPT3.setDrawColor(0);
  u8g2_OPT3.drawBox(0, 0, 128, 32);
  u8g2_OPT3.setDrawColor(1);
  u8g2_OPT3.drawStr(5, 32, newValue);
  u8g2_OPT3.sendBuffer();
}

void onUfcOptionDisplay3Change(char* newValue) {
  strOpt3 = String(newValue);
}
DcsBios::StringBuffer<4> ufcOptionDisplay3Buffer(0x743a, onUfcOptionDisplay3Change);


void updateOpt4(String strnewValue) {
  const char* newValue = strnewValue.c_str();
  tcaselect(Opt_OLED_Port_4);
  u8g2_OPT4.setFontMode(0);
  u8g2_OPT4.setDrawColor(0);
  u8g2_OPT4.drawBox(0, 0, 128, 32);
  u8g2_OPT4.setDrawColor(1);
  u8g2_OPT4.drawStr(5, 32, newValue);
  u8g2_OPT4.sendBuffer();
}

void onUfcOptionDisplay4Change(char* newValue) {
  strOpt4 = String(newValue);
}
DcsBios::StringBuffer<4> ufcOptionDisplay4Buffer(0x743e, onUfcOptionDisplay4Change);

void updateOpt5(String strnewValue) {
  const char* newValue = strnewValue.c_str();
  tcaselect(Opt_OLED_Port_5);
  u8g2_OPT5.setFontMode(0);
  u8g2_OPT5.setDrawColor(0);
  u8g2_OPT5.drawBox(0, 0, 128, 32);
  u8g2_OPT5.setDrawColor(1);
  u8g2_OPT5.drawStr(5, 32, newValue);
  u8g2_OPT5.sendBuffer();
}

void onUfcOptionDisplay5Change(char* newValue) {
  strOpt5 = String(newValue);
}
DcsBios::StringBuffer<4> ufcOptionDisplay5Buffer(0x7442, onUfcOptionDisplay5Change);



void updateComm1(String strnewValue) {
  const char* newValue = strnewValue.c_str();
  tcaselect(COM1_OLED_PORT);

  u8g2_COM1.firstPage();
  do {
    u8g2_COM1.setCursor(COM1_XPOS, COM1_YPOS);
    u8g2_COM1.print(newValue);
  } while (u8g2_COM1.nextPage());
}

void onUfcComm1DisplayChange(char* newValue) {
  strComm1 = String(newValue);
}
DcsBios::StringBuffer<2> ufcComm1DisplayBuffer(0x7424, onUfcComm1DisplayChange);

void updateComm2(String strnewValue) {
  const char* newValue = strnewValue.c_str();
  tcaselect(COM2_OLED_PORT);

  u8g2_COM2.firstPage();
  do {
    u8g2_COM2.setCursor(COM2_XPOS, COM2_YPOS);
    u8g2_COM2.print(newValue);
  } while (u8g2_COM2.nextPage());
}

void onUfcComm2DisplayChange(char* newValue) {
  strComm2 = String(newValue);
}
DcsBios::StringBuffer<2> ufcComm2DisplayBuffer(0x7426, onUfcComm2DisplayChange);


void updateScratchpadString1(String strnewValue) {
  SendIPString("Scratchpad String1");
  SendIPString(strnewValue);
  const char* newValue = strnewValue.c_str();

  tcaselect(ScratchPad_OLED_PORT);
  u8g2_Scratch_Pad.setFontMode(0);
  u8g2_Scratch_Pad.setDrawColor(0);
  u8g2_Scratch_Pad.drawBox(ScratchPad_String1_Pos, 0, ScratchPad_String2_Pos, 32);
  u8g2_Scratch_Pad.setDrawColor(1);
  u8g2_Scratch_Pad.drawStr(ScratchPad_String1_Pos, ScratchPad_Vertical_Pos, newValue);  // write something to the internal memory
  u8g2_Scratch_Pad.sendBuffer();                                                        // transfer internal memory to the display    /* your code here */
}


void onUfcScratchpadString1DisplayChange(char* newValue) {
  strScratchString1 = String(newValue);
}
DcsBios::StringBuffer<2> ufcScratchpadString1DisplayBuffer(0x744e, onUfcScratchpadString1DisplayChange);


void updateScratchpadString2(String strnewValue) {

  SendIPString("Scratchpad String2");
  SendIPString(strnewValue);

  const char* newValue = strnewValue.c_str();

  tcaselect(ScratchPad_OLED_PORT);
  u8g2_Scratch_Pad.setFontMode(0);
  u8g2_Scratch_Pad.setDrawColor(0);
  u8g2_Scratch_Pad.drawBox(ScratchPad_String2_Pos, 0, ScratchPad_Number_Pos - ScratchPad_String2_Pos, 32);
  u8g2_Scratch_Pad.setDrawColor(1);
  u8g2_Scratch_Pad.drawStr(ScratchPad_String2_Pos, ScratchPad_Vertical_Pos, newValue);  // write something to the internal memory
  u8g2_Scratch_Pad.sendBuffer();                                                        // transfer internal memory to the display    /* your code here */    /* your code here */
}

void onUfcScratchpadString2DisplayChange(char* newValue) {
  strScratchString2 = String(newValue);
}
DcsBios::StringBuffer<2> ufcScratchpadString2DisplayBuffer(0x7450, onUfcScratchpadString2DisplayChange);


void updateScratchpadNumber(String strnewValue) {
  SendIPString("Scratchpad Number");
  SendIPString(strnewValue);
  SendIPString("Scratchpad Number Length");
  SendIPString(String(strnewValue.length()));

  const char* newValue = strnewValue.c_str();

  tcaselect(ScratchPad_OLED_PORT);
  u8g2_Scratch_Pad.setFontMode(0);
  u8g2_Scratch_Pad.setDrawColor(0);
  u8g2_Scratch_Pad.drawBox(ScratchPad_Number_Pos, 0, 128 - ScratchPad_Number_Pos, 32);
  u8g2_Scratch_Pad.setDrawColor(1);
  if (strnewValue != "        ")
    u8g2_Scratch_Pad.drawStr(ScratchPad_Number_Pos, ScratchPad_Vertical_Pos, newValue);
  u8g2_Scratch_Pad.sendBuffer();
}

void onUfcScratchpadNumberDisplayChange(char* newValue) {
  strScratchNumber = String(newValue);
}
DcsBios::StringBuffer<8> ufcScratchpadNumberDisplayBuffer(0x7446, onUfcScratchpadNumberDisplayChange);


void updateAllOfScratchpad(String ScratchString1, String ScratchString2, String ScratchNumber) {

  tcaselect(ScratchPad_OLED_PORT);
  String CombinedString = ScratchString1 + ScratchString2 + ScratchNumber;

  const char* ptr_ScratchString1 = ScratchString1.c_str();
  const char* ptr_ScratchString2 = ScratchString2.c_str();
  const char* ptr_ScratchNumber = ScratchNumber.c_str();

  u8g2_Scratch_Pad.setFontMode(0);
  u8g2_Scratch_Pad.setDrawColor(0);
  u8g2_Scratch_Pad.drawBox(0, 0, 128, 32);
  u8g2_Scratch_Pad.setDrawColor(1);
  u8g2_Scratch_Pad.drawStr(ScratchPad_String1_Pos, ScratchPad_Vertical_Pos, ptr_ScratchString1);
  u8g2_Scratch_Pad.drawStr(ScratchPad_String2_Pos, ScratchPad_Vertical_Pos, ptr_ScratchString2);
  u8g2_Scratch_Pad.drawStr(ScratchPad_Number_Pos, ScratchPad_Vertical_Pos, ptr_ScratchNumber);
  u8g2_Scratch_Pad.sendBuffer();
}

#define u8g_logo_width 24
#define u8g_logo_height 32
static unsigned char u8g_logo_bits[] = {
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

void loop() {


  if (millis() >= NEXT_STATUS_TOGGLE_TIMER) {
    RED_LED_STATE = !RED_LED_STATE;
    digitalWrite(STATUS_LED_PORT, RED_LED_STATE);
    // enableAllSteppers();
    // cycleSteppers(650);
    // disableAllSteppers();
    // SendDebug("Uptime " + String(millis()) + " (" + String(millis() / 60000) + ")");
    NEXT_STATUS_TOGGLE_TIMER = millis() + FLASH_TIME;
    // Check to see if model time is updating - if nothing after 30 seconds disble steppers
  }


  if (false) {
    for (int i = 0; i <= 60; i++) {

      String strnewValue = "2";
      String straboveValue = "1";
      String strbelowValue = "3";
      String firstchr = "0";
      unsigned long TimeToProcess = millis();
      const char* newValue = strnewValue.c_str();
      const char* aboveValue = straboveValue.c_str();
      const char* belowValue = strbelowValue.c_str();

      int CharacterHeightSpacer = 38;

      const char* firstValue = firstchr.c_str();


      //tcaselect(Opt_OLED_Port_3);
      u8g2_OPT3.setFontMode(0);
      u8g2_OPT3.setDrawColor(0);
      u8g2_OPT3.drawBox(0, 0, 128, 32);

      u8g2_OPT1.setDrawColor(1);
      u8g2_OPT1.drawStr(10, 30, firstValue);
      u8g2_OPT1.drawStr(40, i, newValue);
      u8g2_OPT1.drawStr(40, i - CharacterHeightSpacer, aboveValue);
      u8g2_OPT1.drawStr(40, i + CharacterHeightSpacer, belowValue);
      u8g2_OPT1.drawXBM(70, 0, u8g_logo_width, u8g_logo_height, u8g_logo_bits);
      u8g2_OPT1.sendBuffer();
      ;
      TimeToProcess = millis() - TimeToProcess;
      SendDebug("OLED Update time :" + String(TimeToProcess));
    }
    delay(1000);
  }

  //for (int i = 660; i >= 0; i--) {
  for (long i = 12000; i >= 0; i=i-20) {

    String strnewValue = String(i);
    int itensAboveDigit = 0;
    int itensBelowDigit = 0;

    String straboveValue = "1";
    String strbelowValue = "3";
    // int tensDigit = (i % 100) / 10;



    int iThousandsAboveDigit = 0;
    int iThousandsBelowDigit = 0;
    int iThousandsValue = ((i % 10000) / 1000);
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


    int iTenThousandsValue = (i / 10000);
    String sTenThousandsDigit = String(iTenThousandsValue);
    //SendDebug("TenThousandsDigit : " + sTenThousandsDigit);

    //String firstchr = String(i / 10000);
    //tring tenDigitchr = String(tensDigit);


    String thirdchr = String(i % 1000);
    unsigned long TimeToProcess = millis();
    const char* cThousandsValue = sThousandValue.c_str();
    const char* cThousandsaboveValue = sThousandsAboveDigit.c_str();
    const char* cThousandsbelowValue = sThousandsBelowDigit.c_str();

    //const char* tensValue = tenDigitchr.c_str();
    //const char* tensAboveValue = tensAboveDigitchr.c_str();
    //const char* tensBelowValue = tensBelowDigitchr.c_str();

    int CharacterHeightSpacer = 38;
    int CharacterOffset = ((i % 1000) / 32);
    // SendDebug("Character Offset : " + String(CharacterOffset));


    const char* cTenThousandsValue = sTenThousandsDigit.c_str();


    //tcaselect(Opt_OLED_Port_3);
    u8g2_OPT3.setFontMode(0);
    u8g2_OPT3.setDrawColor(0);
    u8g2_OPT3.drawBox(0, 0, 128, 32);
    u8g2_OPT1.setDrawColor(1);
    // u8g2_OPT1.drawStr(75, 30, newValue);
    if (sTenThousandsDigit != "0") {
      u8g2_OPT1.drawStr(10, 30, cTenThousandsValue);
    } else {
      u8g2_OPT1.drawXBM(0, 0, u8g_logo_width, u8g_logo_height, u8g_logo_bits);
    }

    u8g2_OPT1.drawStr(40, -2 + CharacterOffset, cThousandsaboveValue);
    //u8g2_OPT1.drawStr(40, 30 + int(onesDigit * 3), cThousandsbelowValue);
    u8g2_OPT1.drawStr(40, 30 + CharacterOffset, cThousandsValue);
    u8g2_OPT1.sendBuffer();
    ;
    TimeToProcess = millis() - TimeToProcess;
    //SendDebug("OLED Update time :" + String(TimeToProcess));
    delay(0);
  }
  delay(1000);




  if (false) {
    if (DCSBIOS_In_Use == 1) DcsBios::loop();
    if (strOpt1 != LaststrOpt1) {
      updateOpt1(strOpt1);
      LaststrOpt1 = strOpt1;
    }

    if (strOpt2 != LaststrOpt2) {
      updateOpt2(strOpt2);
      LaststrOpt2 = strOpt2;
    }

    if (strOpt3 != LaststrOpt3) {
      updateOpt3(strOpt3);
      LaststrOpt3 = strOpt3;
    }

    if (strOpt4 != LaststrOpt4) {
      updateOpt4(strOpt4);
      LaststrOpt4 = strOpt4;
    }


    if (strOpt5 != LaststrOpt5) {
      updateOpt5(strOpt5);
      LaststrOpt5 = strOpt5;
    }


    if (strComm1 != LaststrComm1) {
      updateComm1(strComm1);
      LaststrComm1 = strComm1;
    }

    if (strComm2 != LaststrComm2) {
      updateComm2(strComm2);
      LaststrComm2 = strComm2;
    }

    CombinedScratchString = strScratchString1 + strScratchString2 + strScratchNumber;

    if (CombinedScratchString != LastCombinedScratchString) {
      updateAllOfScratchpad(strScratchString1, strScratchString2, strScratchNumber);
      LastCombinedScratchString = CombinedScratchString;
    }

    //  if (strScratchString1 != LaststrScratchString1) {
    //    updateScratchpadString1(strScratchString1);
    //    LaststrScratchString1 = strScratchString1;
    //  }
    //
    //  if (strScratchString2 != LaststrScratchString2) {
    //
    //    updateScratchpadNumber(strScratchNumber);
    //    updateScratchpadString2(strScratchString2);
    //    LaststrScratchString2 = strScratchString2;
    //  }
    //
    //
    //  if (strScratchNumber != LaststrScratchNumber) {
    //    updateScratchpadNumber(strScratchNumber);
    //    LaststrScratchNumber = strScratchNumber;
    //
    //    updateScratchpadString2(strScratchString2);
    //
    //  }
  }
}