
// FreeTronics OLED declarations
#include <SPI.h>
#include <SD.h>
#include <FTOLED.h>
#include <fonts/SystemFont5x7.h>
#include <fonts/Arial14.h>
#include <fonts/Arial_Black_16.h>
#include <fonts/Droid_Sans_36.h>

// NB: This sketch won't fit on older Arduino Duemilanoves or any other
// Arduino with less than 32k of onboard flash storage.
//
// If the sketch doesn't fit, disable one of the fonts by deleting one of
// the blocks below

const byte pin_cs = 7;
const byte pin_dc = 2;
const byte pin_reset = 3;

OLED oled(pin_cs, pin_dc, pin_reset);

// Ethernet Declarations
#include <Ethernet.h>
#include <EthernetUdp.h>

byte mac[] = { 
	0xA9,0xE7,0x3E,0xCA,0x34,0x1C};
IPAddress ip(192,168,1,108);
const unsigned int localport = 13135;

EthernetUDP udp;
char packetBuffer[200]; //buffer to store the incoming data

String newmsg, SecondsText;
int TimeDigits[4];
int LastReading;

void setup() {

  Serial.begin(115200);
  Serial.print("init...");

  

  Ethernet.begin( mac, ip);
  udp.begin( localport );

  oled.begin();
  oled.selectFont(Droid_Sans_36);
  oled.drawFilledBox(40,80,85,110,BLACK);
  oled.drawString(45,72,"X",GREEN,BLACK);
  oled.drawString(66,72,"X",GREEN,BLACK);
  delay(1000); 
 
 TimeDigits[0] = 0;
 LastReading = 0;

}

void loop() {

  
      // Grab UDP Packet
    int packetSize = udp.parsePacket();
    if( packetSize > 0) {
        Serial.println("Packet Received");
        udp.read( packetBuffer, packetSize);
  	 //terminate the buffer manually
  	 packetBuffer[packetSize] = '\0';
        
        if (packetBuffer[0] == 'S') {    
          // We have some data
          Serial.println(packetBuffer);
            // Split the string up
           char* endPos;
           int i = 0;
           char *p = packetBuffer + 1;
           char *str;
           while ((str = strtok_r(p, ":", &p)) != NULL) 
           {// delimiter is the semicolon
             Serial.print( i);
             Serial.print("-");
             Serial.println(str);
             TimeDigits[i] = strtod(str, &endPos);
             i++;
             
          }
    	}	
    }
  
    
    if (TimeDigits[0] != LastReading) {
      Serial.println("New Time");
      LastReading = TimeDigits[0];
      newmsg = String(int(TimeDigits[0]/10));
      oled.drawFilledBox(40,80,85,110,BLACK);
      
      SecondsText = String(TimeDigits[0] % 10);
      oled.drawString(45,72,newmsg,GREEN,BLACK);
      oled.drawString(66,72,SecondsText,GREEN,BLACK);
      
    }
    
//  for( int i = 0; i <= 5; i++) {
//	
//    for (int x = 0; x <= 9; x++) { 
//	
//        newmsg = String(i);
//        SecondsText = String(x);
//
//        oled.drawFilledBox(40,80,85,110,BLACK);
//        oled.drawString(45,72,newmsg,GREEN,BLACK);
//        oled.drawString(66,72,SecondsText,GREEN,BLACK);
//
//     }
//   }       
}
