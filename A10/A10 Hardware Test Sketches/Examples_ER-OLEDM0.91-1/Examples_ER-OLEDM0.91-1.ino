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

void setup() {
  Serial.begin(9600);
  Serial.print("OLED Example\n");
  Wire.begin();
  
  /* display an image of bitmap matrix */
  er_oled_begin();
  er_oled_clear(oled_buf);
  er_oled_bitmap(0, 0, PIC1, 128, 32, oled_buf);
  er_oled_display(oled_buf);
  delay(3000);  
  command(0xa7);//--set Negative display 
  delay(3000);
  command(0xa6);//--set normal display
  
  er_oled_clear(oled_buf);
  er_oled_bitmap(0, 0, PIC2, 128, 32, oled_buf);
  er_oled_display(oled_buf);
  delay(3000);
  
  command(0xa7);//--set Negative display 
  delay(3000); 
  command(0xa6);//--set normal display  
  
    er_oled_clear(oled_buf);
  /* display images of bitmap matrix */
  er_oled_bitmap(0, 2, Signal816, 16, 8, oled_buf); 
  er_oled_bitmap(24, 2,Bluetooth88, 8, 8, oled_buf); 
  er_oled_bitmap(40, 2, Msg816, 16, 8, oled_buf); 
  er_oled_bitmap(64, 2, GPRS88, 8, 8, oled_buf); 
  er_oled_bitmap(90, 2, Alarm88, 8, 8, oled_buf); 
  er_oled_bitmap(112, 2, Bat816, 16, 8, oled_buf); 

//  er_oled_string(10, 2, "www.buydisplay.com", 12, 1, oled_buf); 
 

  er_oled_char(4, 16, '1' ,16, 1, oled_buf);
  er_oled_char(20, 16, '2', 16, 1, oled_buf);
  er_oled_char(36, 16, ':', 16, 1, oled_buf);
  er_oled_char(52, 16, '3', 16, 1, oled_buf);
  er_oled_char(68, 16, '4', 16, 1, oled_buf);
  er_oled_char(84, 16, ':', 16, 1, oled_buf);
  er_oled_char(100, 16, '5', 16, 1, oled_buf);
  er_oled_char(116, 16, '6', 16, 1, oled_buf);
  
  er_oled_display(oled_buf); 
  delay(3000);    
  er_oled_clear(oled_buf);
  er_oled_string(10, 2, "www.buydisplay.com", 12, 1, oled_buf);  
  er_oled_char(4, 16, '1' ,16, 1, oled_buf);
  er_oled_char(20, 16, '2', 16, 1, oled_buf);
  er_oled_char(36, 16, ':', 16, 1, oled_buf);
  er_oled_char(52, 16, '4', 16, 1, oled_buf);
  er_oled_char(68, 16, '2', 16, 1, oled_buf);
  er_oled_char(84, 16, ':', 16, 1, oled_buf);
  er_oled_char(100, 16, '1', 16, 1, oled_buf);
  er_oled_char(116, 16, '8', 16, 1, oled_buf);
   er_oled_display(oled_buf);  
}

void loop() {

}
