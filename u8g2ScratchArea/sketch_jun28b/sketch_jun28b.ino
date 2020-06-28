#include <Arduino.h>
#include <SPI.h>
#include <U8g2lib.h>

/* Constructor */
//U8G2_SSD1306_128X32_UNIVISION_1_SW_I2C u8g2(U8G2_R0, /* clock=*/ SCL, /* data=*/ SDA, /* reset=*/ U8X8_PIN_NONE); 
U8G2_SSD1306_64X48_ER_1_HW_I2C u8g2(U8G2_R0, /* reset=*/ 4); 

/* u8g2.begin() is required and will sent the setup/init sequence to the display */
void setup(void) {
  u8g2.begin();
}

/* draw something on the display with the `firstPage()`/`nextPage()` loop*/
void loop(void) {
  int xpos = 35;
  int xpos2 = 9;
  int ypos1 = 47;
  for (int i=0; i<= 13; i++){
    u8g2.firstPage();
    do {
      //u8g2.setFont(u8g2_font_ncenB32_tr);
      
      u8g2.setFont(u8g2_font_fub35_tr);
      if (i<= 9)
        u8g2.setCursor(xpos,ypos1);
      else 
        u8g2.setCursor(xpos2,ypos1);
      u8g2.print(i);
      
//      u8g2.setFont(u8g2_font_ncenB12_tr);
//  
//      u8g2.setCursor(0,14);
//      u8g2.print(millis());
    } while ( u8g2.nextPage() );
    delay(10);
    
  }
  u8g2.firstPage();
  do {
 
    u8g2.setFont(u8g2_font_fub35_tr);
    u8g2.setCursor(xpos2,ypos1);
    u8g2.print("C");

    
  } while ( u8g2.nextPage() );
  delay(40);

}
