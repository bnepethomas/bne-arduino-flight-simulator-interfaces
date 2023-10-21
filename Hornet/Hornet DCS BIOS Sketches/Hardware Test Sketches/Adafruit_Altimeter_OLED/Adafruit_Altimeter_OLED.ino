/**************************************************************************
 This is an example for our Monochrome OLEDs based on SSD1306 drivers

 Pick one up today in the adafruit shop!
 ------> http://www.adafruit.com/category/63_98

 This example is for a 128x32 pixel display using I2C to communicate
 3 pins are required to interface (two I2C and one reset).

 Adafruit invests time and resources providing this open
 source code, please support Adafruit and open-source
 hardware by purchasing products from Adafruit!

 Written by Limor Fried/Ladyada for Adafruit Industries,
 with contributions from the open source community.
 BSD license, check license.txt for more information
 All text above, and the splash screen below must be
 included in any redistribution.
 **************************************************************************/

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)

Adafruit_SSD1306 baro_display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire);
Adafruit_SSD1306 alt_display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire);


extern "C" {
#include "utility/twi.h"  // from Wire library, so we can do bus scanning
}

#define Ethernet_In_Use 1
#define DCSBIOS_In_Use 1
#define Reflector_In_Use 1

#define DCSBIOS_IRQ_SERIAL
#include "DcsBios.h"


// Ethernet Related
#include <SPI.h>
#include <Ethernet.h>
#include <EthernetUdp.h>


// These local Mac and IP Address will be reassigned early in startup based on
// the device ID as set by address pins
byte mac[] = {0xA8, 0x61, 0x0A, 0x9E, 0x83, 0x01};
IPAddress ip(172, 16, 1, 101);
String strMyIP = "172.16.1.101";

// Raspberry Pi is Target
IPAddress reflectorIP(172, 16, 1, 10);
String strreflectorIP = "X.X.X.X";

// Arduino Mega holding Max7219
IPAddress max7219IP(172, 16, 1, 106);
String strMax7219IP = "172.16.1.106";



const unsigned int localport = 7788;
const unsigned int remoteport = 26027;
const unsigned int reflectorport = 27000;
const unsigned int max7219port = 7788;

EthernetUDP udp;
char packetBuffer[1000];     //buffer to store the incoming data
char outpacketBuffer[1000];  //buffer to store the outgoing data


String DebugString = "";


#define BARO_OLED_Port 0
#define ALT_OLED_Port 1

#define TCAADDR 0x70  
void tcaselect(uint8_t i) {

  if (i > 7) return;

  Wire.beginTransmission(TCAADDR);
  Wire.write(1 << i);
  Wire.endTransmission();

}

void SendDebug( String MessageToSend) {
    if (Reflector_In_Use == 1)  {
      udp.beginPacket(reflectorIP, reflectorport);
      udp.println(MessageToSend);
      udp.endPacket();
    }  
}


void setup() {

    if (Ethernet_In_Use == 1) {
    Ethernet.begin( mac, ip);


    udp.begin( localport );
    if (Reflector_In_Use == 1)  {
      udp.beginPacket(reflectorIP, reflectorport);
      udp.println("Init Adafruit Font I2C Test rig - " + strMyIP + " " + String(millis()) + "mS since reset.");
      udp.endPacket();
    }
  }

  Wire.begin();


  // Had to comment out these debugging messages as they created a conflict with the IRQ definition in DCS BIOS
  SendDebug("Scanning I2C Bus");

  for (uint8_t t = 0; t < 8; t++) {
    SendDebug("TCA Port #" + String(t));
    tcaselect(t);
    // Had to comment out these debugging messages as they created a conflict with the IRQ definition in DCS BIOS


    for (uint8_t addr = 0; addr <= 127; addr++) {
      //if (addr == TCAADDR) continue;

      uint8_t data;
      if (! twi_writeTo(addr, &data, 0, 1, 1)) {
        // Had to comment out these debugging messages as they created a conflict with the IRQ definition in DCS BIOS
        DebugString = "Found I2C 0x";
        DebugString.concat(String(addr, HEX)); 
        SendDebug(DebugString);
      }
    }
  }
  // Had to comment out these debugging messages as they created a conflict with the IRQ definition in DCS BIOS
  SendDebug("I2C scan complete");


  
  tcaselect(BARO_OLED_Port);
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  baro_display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  baro_display.clearDisplay();
  baro_display.display();

  tcaselect(ALT_OLED_Port);

  alt_display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  alt_display.clearDisplay();
  alt_display.display();


  tcaselect(BARO_OLED_Port);
  testdrawchar();      // Draw characters of the default font



  // Invert and restore display, pausing in-between
  baro_display.invertDisplay(true);
  delay(1000);
  baro_display.invertDisplay(false);
  delay(1000);

  SendDebug("Driving Alt OLED");
  tcaselect(ALT_OLED_Port);

  alt_testdrawchar();      // Draw characters of the default font
  alt_display.invertDisplay(true);
  
  SendDebug("work is done");

}

void loop() {
}



void testdrawchar(void) {
  baro_display.clearDisplay();

  baro_display.setTextSize(1);      // Normal 1:1 pixel scale
  baro_display.setTextColor(SSD1306_WHITE); // Draw white text
  baro_display.setCursor(0, 0);     // Start at top-left corner
  baro_display.cp437(true);         // Use full 256 char 'Code Page 437' font

  // Not all the characters will fit on the display. This is normal.
  // Library will draw what it can and the rest will be clipped.
  for(int16_t i=0; i<256; i++) {
    if(i == '\n') baro_display.write(' ');
    else          baro_display.write(i);
  }

  baro_display.display();
  delay(1000);
}

void alt_testdrawchar(void) {
  alt_display.clearDisplay();

  alt_display.setTextSize(1);      // Normal 1:1 pixel scale
  alt_display.setTextColor(SSD1306_WHITE); // Draw white text
  alt_display.setCursor(0, 0);     // Start at top-left corner
  alt_display.cp437(true);         // Use full 256 char 'Code Page 437' font

  // Not all the characters will fit on the display. This is normal.
  // Library will draw what it can and the rest will be clipped.
  for(int16_t i=0; i<256; i++) {
    if(i == '\n') alt_display.write(' ');
    else          alt_display.write(i);
  }

  alt_display.display();
  delay(1000);
}
#define XPOS   0 // Indexes into the 'icons' array in function below
#define YPOS   1
#define DELTAY 2
