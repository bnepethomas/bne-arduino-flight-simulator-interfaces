//  Hornet UFC
//
// The project intends to drive the OLED displays on a F18C Hornet Up Front Controller
//
// To do  - grab Analog 0 use that to locally to determine brightness - no real value in getting that from the Sim
//        - develop font based on 14 segment display
//
// The UFC has a large display on the top left hand corner, five mid sized display on the right hand side, and then two smaller 
// displays at the bottom left and bottom right hand side.
//
// As a number of the smae OLEDs are used, which the same target I2C addresses an I2C multiplexor is used, TCA9548A.
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
// Pin 16 tied to Arduino Reset
// It does support 3.3V, but works ok on 5V
// 1 Gnd
// 2 +5V
// 4 Gnd
// 7 SCL
// 8 & 9 SDA
// 16 Reset

// Useful reference for troubleshooting OLEDs

// OLED for 5 Right hand side digits 0.91" 128*32 SSD1306
// https://www.ebay.com/itm/0-91-Inch-128x32-IIC-I2c-White-Blue-OLED-LCD-Display-Module-3-3-5v-For-Arduino/392552169768?ssPageName=STRK%3AMEBIDX%3AIT&var=661536491479&_trksid=p2057872.m2749.l2649


//  OLED for Small Digits 0.96" SSD1306
// https://www.ebay.com/itm/0-96-White-Yellow-Blue-128X64-OLED-I2C-IIC-Serial-LCD-LED-SSD-Display-AU/302435912549?ssPageName=STRK%3AMEBIDX%3AIT&var=601268570499&_trksid=p2060353.m1438.l2649
// or may this one 0.66" 64*48 SSD1036
// I2C 0x3C - validate by running scanner


// Basic logic
// Initialise I2C Bus (wire)
// Iterate through each port on the Mux and list connected devices to serial port (simple troubleshooting aid), may use UDP later
// Initialise each display
// Wait for UDP updates
// during timeout grab and smooth analo 0 and get brightness for all displays


#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Fonts/FreeMonoBoldOblique24pt7b.h>
#include <Fonts/FreeMonoBoldOblique12pt7b.h>
#include <Fonts/FreeMonoBold18pt7b.h>  
#include <Fonts/FreeMono9pt7b.h>   
#include "PTFont.h"
#include "er_oled.h"

// Unsure what this does - hopefully not a pin on the mega
#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);

#if (SSD1306_LCDHEIGHT != 32)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif

extern "C" { 
#include "utility/twi.h"  // from Wire library, so we can do bus scanning
}

#define TCAADDR 0x70

uint8_t oled_buf[WIDTH * HEIGHT / 8];
int CurrentDisplay = 0;
int Brightness = 0;
char buffer[20]; //plenty of space for the value of millis() plus a zero terminator
 
void tcaselect(uint8_t i) {
  if (i > 7) return;
 
  Wire.beginTransmission(TCAADDR);
  Wire.write(1 << i);
  Wire.endTransmission();  
}

void setup() {
  Serial.begin(115200);
  Wire.begin();

  Serial.println("\nScanning I2C Bus");
  
  for (uint8_t t=0; t<8; t++) {
    tcaselect(t);
    Serial.print("TCA Port #"); Serial.println(t);

    for (uint8_t addr = 0; addr<=127; addr++) {
      if (addr == TCAADDR) continue;
    
      uint8_t data;
      if (! twi_writeTo(addr, &data, 0, 1, 1)) {
         Serial.print("Found I2C 0x");  Serial.println(addr,HEX);
      }
    }
  }
  Serial.println("\nI2C scan complete"); 

  tcaselect(0);
  er_oled_begin();

  for (uint8_t t=1; t<8; t++) {
    tcaselect(t);
    display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3C (for the 128x32)
    display.display();
  }

}

void setContrast(int contr){
    //From https://www.youtube.com/watch?v=hFpXfSnDNSY
    //
    // for Adafruit_SSD1306.h library
    // contr values from 1 to 411
    // for i2c and 3.3V VCC works fine
    int prech;
    int brigh; 
    switch (contr){
      case 001 ... 255: prech= 0; brigh= contr; break;
      case 256 ... 411: prech=16; brigh= contr-156; break;
      default: prech= 16; brigh= 255; break;}
      
    display.ssd1306_command(SSD1306_SETPRECHARGE);      
    display.ssd1306_command(prech);                            
    display.ssd1306_command(SSD1306_SETCONTRAST);         
    display.ssd1306_command(brigh);                           
}


void loop() {


    
  CurrentDisplay ++;
  if (CurrentDisplay >7) 
    CurrentDisplay = 0;


  tcaselect(CurrentDisplay);
  if (CurrentDisplay == 0)
  {
    er_oled_clear(oled_buf);
    sprintf(buffer, "%d", millis());  //convert millis() to a char array in buffer and terminate it with a zero
    
    er_oled_string(40, 7, buffer, 16, 1, oled_buf);  
    
    er_oled_display(oled_buf);
  } else
  {
    // Clear the buffer.
    display.clearDisplay();
    //display.setFont(&FreeMonoBold18pt7b); 
    display.setFont(&FreeMono9pt7b);
    // text display tests
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0,10);
  
    display.println(CurrentDisplay);
    //display.setFont(&FreeMonoBoldOblique24pt7b);
    display.setFont(&DSEG14_Classic_Regular_33);
    //display.setFont(&FreeMonoBold18pt7b);
    display.setCursor(10,32);
    display.println(millis());

    randomSeed(analogRead(0));
    Brightness = random(254);
    if (Brightness > 120)
      setContrast(411); 
    else
      setContrast(1); 
    display.display();

    



    
    

  }
  

}
