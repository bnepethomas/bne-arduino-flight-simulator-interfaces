// OLED Update 176mS

/***************************************************
//Web: http://www.buydisplay.com
EastRising Technology Co.,LTD
Examples for ER-OLEDM0.91-1
Display is Hardward I2C 2-Wire I2C Interface 
Tested and worked with:
Works with Arduino 1.6.0 IDE  
Test OK : Arduino DUE,Arduino mega2560,Arduino UNO Board 
****************************************************/
#include <Wire.h>
#include "er_oled.h"

/*
  == Hardware connection for 4 PIN module==
    OLED   =>    Arduino
  *1. GND    ->    GND
  *2. VCC    ->    3.3V
  *3. SCL    ->    SCL
  *4. SDA    ->    SDA
*/

uint8_t oled_buf[WIDTH * HEIGHT / 8];

#define EthernetIsNotHere

#ifdef EthernetIsHere
#define Ethernet_In_Use 1
#define DCSBIOS_In_Use 1
#define Reflector_In_Use 1
#endif

#define DCSBIOS_IRQ_SERIAL
#include "DcsBios.h"

#ifdef EthernetIsHere
// Ethernet Related
#include <SPI.h>
#include <Ethernet.h>
#include <EthernetUdp.h>

#define ES1_RESET_PIN 53
// These local Mac and IP Address will be reassigned early in startup based on
// the device ID as set by address pins
byte mac[] = { 0xA8, 0x61, 0x0A, 0x9E, 0x83, 0x71 };
IPAddress ip(172, 16, 1, 114);
String strMyIP = "172.16.1.114";

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

#endif

void SendDebug(String MessageToSend) {
#ifdef EthernetIsHere
  if ((Reflector_In_Use == 1) && (Ethernet_In_Use == 1)) {
    udp.beginPacket(reflectorIP, reflectorport);
    udp.println(MessageToSend);
    udp.endPacket();
  }
#endif
}



void setup() {

#ifdef EthernetIsHere
  if (Ethernet_In_Use == 1) {

    Ethernet.begin(mac, ip);
    udp.begin(localport);
    if (Reflector_In_Use == 1) {
      udp.beginPacket(reflectorIP, reflectorport);
      udp.println("Init I2C Test rig - " + strMyIP + " " + String(millis()) + "mS since reset.");
      udp.endPacket();
    }
  }
#endif
  Wire.begin();

  /* display an image of bitmap matrix */
  er_oled_begin();
  er_oled_clear(oled_buf);
  //er_oled_string(10, 2, "000", 16, 1, oled_buf);
  //er_oled_char(10, 2, '1', 16, 1, oled_buf);
  er_oled_char3216(0, 0, '0', oled_buf);

  int Second_Pos_x = 30;
  int Second_Pos_y = 2;
  er_oled_char3216(Second_Pos_x, Second_Pos_y, '0', oled_buf);
  er_oled_display(oled_buf);
  delay(2000);
  er_oled_char3216(Second_Pos_x, Second_Pos_y, '1', oled_buf);
  er_oled_display(oled_buf);
  delay(2000);
  er_oled_char3216(Second_Pos_x, Second_Pos_y, '2', oled_buf);
  er_oled_display(oled_buf);
  delay(2000);
  er_oled_char3216(Second_Pos_x, Second_Pos_y, '3', oled_buf);
  er_oled_display(oled_buf);
  delay(2000);

  for (char i = 48; i <= 57; i++) {

    unsigned long TimeToProcess = millis();
    er_oled_char3216(Second_Pos_x, Second_Pos_y, i, oled_buf);
    er_oled_display(oled_buf);
    TimeToProcess = millis() - TimeToProcess;
    SendDebug("OLED Update time :" + String(TimeToProcess));
    delay(500);
  }

  // er_oled_char(20, 16, '2', 16, 1, oled_buf);
  // er_oled_char(36, 16, ':', 16, 1, oled_buf);
  // er_oled_char(52, 16, '4', 16, 1, oled_buf);
  // er_oled_char(68, 16, '2', 16, 1, oled_buf);
  // er_oled_char(84, 16, ':', 16, 1, oled_buf);
  // er_oled_char(100, 16, '1', 16, 1, oled_buf);
  // er_oled_char(116, 16, '8', 16, 1, oled_buf);
  er_oled_display(oled_buf);
}


void loop() {
}
