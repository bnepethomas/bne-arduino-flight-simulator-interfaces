//  Cable Tester

// Tests IDC cables
//
// Input Device is a single push button
// Status is reflected using single oled directly attached to I2C. The OLED model
// is the same as used in the OPT displays on the UFC -  0.91" 128*32 SSD1306
// https://www.ebay.com/itm/0-91-Inch-128x32-IIC-I2c-White-Blue-OLED-LCD-Display-Module-3-3-5v-For-Arduino/392552169768?ssPageName=STRK%3AMEBIDX%3AIT&var=661536491479&_trksid=p2057872.m2749.l2649

/*
Program flow
on power on display startup message
create arrays on output and input pings

Pull all inputslow using inbuilt pull down 
On button press
first check all inputs read low
walk each input to determine cable width by pulling high and validating change
display pin being tested
display width at col 0 
Set all outputs to zero
output all high aside from pin being tested - validate pin being tested does not change
repeat until we have reached the lat expected pin.


*/


// The initial test OLEDs have addresses of 0x3C - despite being labelled - 0x78 or 0x7A - selectable by soldering a jumper. I'm assuming these are the same SSD1306
// used in the Hornet Altimeter.
// Can validate what addresses are on the bus by using I2C scanner



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
#define DCSBIOS_In_Use 1
#define Reflector_In_Use 1


// Port 3 is used for either a channel or Scratchpad display
//#define Opt_OLED_Port_1 6
//#define Opt_OLED_Port_2 4
//#define Opt_OLED_Port_3 5
//#define Opt_OLED_Port_4 2
//#define Opt_OLED_Port_5 7

#define BARO_OLED_Port 6
#define ALTIMETER_HEIGHT_TCA_PORT 7


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




//#include <Arduino.h>
#include <U8g2lib.h>
#include "hornet_font.h"


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

String sAircraftName = "";

// Scratch Pad OLED
U8G2_SSD1305_128X32_ADAFRUIT_F_HW_I2C u8g2_Scratch_Pad(U8G2_R2, 12);

// Op OLEDs
U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C u8g2_ALT(U8G2_R0, /* reset=*/U8X8_PIN_NONE);
U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C u8g2_BARO(U8G2_R0, /* reset=*/U8X8_PIN_NONE);
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
  Serial.println(MessageToSend);
}

void setup() {
  Serial.begin(115200);
  Serial.write("Cable Tester Start");
  SendDebug("hello");
  pinMode(STATUS_LED_PORT, OUTPUT);

;


  Wire.begin();


  // Had to comment out these debugging messages as they created a conflict with the IRQ definition in DCS BIOS
  //Serial.println("\nScanning I2C Bus");

  for (uint8_t t = 0; t < 8; t++) {
    tcaselect(t);
    // Had to comment out these debugging messages as they created a conflict with the IRQ definition in DCS BIOS
    SendDebug("TCA Port #" + String(t));

    for (uint8_t addr = 0; addr <= 127; addr++) {
      //if (addr == TCAADDR) continue;

      uint8_t data;
      if (!twi_writeTo(addr, &data, 0, 1, 1)) {
        // Had to comment out these debugging messages as they created a conflict with the IRQ definition in DCS BIOS
        SendDebug("Found I2C 0x" + String(addr));
      }
    }
  }
  // Had to comment out these debugging messages as they created a conflict with the IRQ definition in DCS BIOS
  SendDebug("I2C scan complete");



  u8g2_BARO.begin();
  u8g2_BARO.clearBuffer();
  u8g2_BARO.setFont(u8g2_font_logisoso16_tn);
  u8g2_BARO.sendBuffer();
  updateBARO("2992");
  delay(1000);
  updateBARO("737");



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





void updateBARO(String strnewValue) {

  const char* newValue = strnewValue.c_str();
  tcaselect(BARO_OLED_Port);
  u8g2_BARO.setFontMode(0);
  u8g2_BARO.setDrawColor(0);
  u8g2_BARO.drawBox(0, 0, 128, 32);
  u8g2_BARO.setDrawColor(1);
  u8g2_BARO.setFontDirection(0);
  u8g2_BARO.drawStr(0, 16, newValue);
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

void UpdateAltimeterDigits(long height) {

  SendDebug("Aircraft Name : " + sAircraftName);
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
  SendDebug("Hundreds Value : " + sHundredsValue);
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
  SendDebug("heigh calc :" + String(height % 100));
  int iThousandsCharacterOffset = ((height % 1000) / 32);
  SendDebug("Character Offset : " + String(iHundredsCharacterOffset));

  // Only attempt to draw of something has changed that will impact display
  if ((iThousandsValue != lastThousandsValue) || (iTenThousandsValue != lastTenThousandsValue) || (iThousandsCharacterOffset != lastThousandsCharacterOffset)
      || (iHundredsValue != lastHundredsValue) || (iHundredsCharacterOffset != lastHundredsCharacterOffset)) {

    tcaselect(ALTIMETER_HEIGHT_TCA_PORT);

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








void loop() {



}
