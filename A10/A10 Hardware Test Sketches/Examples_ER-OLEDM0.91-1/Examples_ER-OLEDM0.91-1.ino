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
    er_oled_char3216(Second_Pos_x, Second_Pos_y, i, oled_buf);
    er_oled_display(oled_buf);
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
