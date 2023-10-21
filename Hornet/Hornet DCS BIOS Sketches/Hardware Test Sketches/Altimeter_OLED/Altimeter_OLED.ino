/*

OLED Test Framework for OH Altimeter

//
// Based on Hornet UFC
//
// The project intends to drive the OLED displays on a F18C Hornet Up Front Controller
//
// To do  - assign analog and digital inputs
//        - add zero sense for Altimeter
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

// The initial test OLEDs have addresses of 0x3C 
// The I2C Mux lives at 0x70
// Can validate what addresses are on the bus by using I2C scanner


// OLED for 5 Right hand side digits 0.91" 128*32 SSD1306
// https://www.ebay.com/itm/0-91-Inch-128x32-IIC-I2c-White-Blue-OLED-LCD-Display-Module-3-3-5v-For-Arduino/392552169768?ssPageName=STRK%3AMEBIDX%3AIT&var=661536491479&_trksid=p2057872.m2749.l2649


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
*/


#define Ethernet_In_Use 1
#define DCSBIOS_In_Use 1
#define Reflector_In_Use 1

#define DCSBIOS_IRQ_SERIAL
#include "DcsBios.h"


// Ethernet Related
#include <SPI.h>
#include <Ethernet.h>
#include <EthernetUdp.h>

// I2C
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

#include <U8g2lib.h>
#include "hornet_font.h"
#include "newfont.h"




// Opt OLEDs
U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C u8g2_BARO(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);
U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C u8g2_ALT(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

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
      udp.println("Init I2C Test rig - " + strMyIP + " " + String(millis()) + "mS since reset.");
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
  u8g2_BARO.begin();
  u8g2_BARO.clearBuffer();
  //u8g2_BARO.setFont(u8g2_DcsFontHornet4_BIOS_09_tf);
  u8g2_BARO.setFont(u8g2_font_fub14_tr );
  u8g2_BARO.sendBuffer();


  tcaselect(ALT_OLED_Port);
  u8g2_ALT.begin();
  u8g2_ALT.clearBuffer();
  //u8g2_ALT.setFont(u8g2_DcsFontHornet4_BIOS_09_tf);
  //u8g2_ALT.setFont(u8g2_font_t0_11_t_all);
  u8g2_ALT.setFont(u8g2_font_fub20_tr  );
  u8g2_ALT.sendBuffer();

  updateALT("0", "8");
  updateBARO("1018");

}


void updateBARO(String strnewValue) {

  const char* newValue = strnewValue.c_str();
  tcaselect(BARO_OLED_Port);
  u8g2_BARO.setFontMode(0);
  u8g2_BARO.setDrawColor(0);
  u8g2_BARO.drawBox(0, 0, 128 , 32);
  u8g2_BARO.setDrawColor(1);
  u8g2_BARO.setFontDirection(2);
  u8g2_BARO.drawStr(115, 0, newValue);
  u8g2_BARO.sendBuffer();
}




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
  u8g2_ALT.drawBox(0, 0, 128 , 32);
  u8g2_ALT.setDrawColor(1);
  //u8g2_ALT.drawStr(5, 32, newValue);

  if (strTenThousands == "0") {
    u8g2_ALT.setDrawColor(1);
    
    u8g2_ALT.drawBox(Start_X_Pos, 13, Box_Width , 20);
    u8g2_ALT.setDrawColor(0);
    
    u8g2_ALT.drawLine(Start_X_Pos, Start_Y_Pos, End_X_Pos , 32);
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
}






void loop() {


}
