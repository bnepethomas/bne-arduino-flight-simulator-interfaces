#include <Arduino.h>
#include <SPI.h>
#include <U8g2lib.h>
//#include "hornet_font.c"
#include "hornet_font.h"
//#include "u8g2_font.c"
//#include "u8g2_fonts.c"

// For sme reason the small display is twitchy
// After shutting day perfectly the day befoew
// first thing in the morning the output was a 
// mess
// after a few minutes it settled down which is really weird
//
//      tcaselect(2);
//      OLED_3.setFont(u8g2_DcsFont_tf);
//      OLED_3.clearBuffer(); // clear the internal memory
//      OLED_3.drawStr(01, 32, ":ATTH");
//      OLED_3.sendBuffer(); // transfer internal memory to the display
//
//      tcaselect(3);
//      OLED_4.setFont(u8g2_DcsFontHornet4_tf);
//      OLED_4.clearBuffer(); // clear the internal memory
//      OLED_4.drawStr(01, 32, " RALT");
//      OLED_4.sendBuffer(); // transfer internal memory to the display
//           
//      tcaselect(5);
//      OLED_6.setFont(u8g2_DcsFontHornet3_tf);
//      OLED_6.clearBuffer(); // clear the internal memory
//      OLED_6.drawStr(01, 38, " BALT");
//      OLED_6.sendBuffer(); // transfer internal memory to the display

/* Constructor */
//U8G2_SSD1306_128X32_UNIVISION_1_SW_I2C u8g2(U8G2_R0, /* clock=*/ SCL, /* data=*/ SDA, /* reset=*/ U8X8_PIN_NONE); 
U8G2_SSD1306_64X48_ER_1_HW_I2C u8g2(U8G2_R0, /* reset=*/ 4); 

U8G2_SSD1306_64X48_ER_1_HW_I2C u8g2a(U8G2_R0, /* reset=*/ 4); 

/* u8g2.begin() is required and will sent the setup/init sequence to the display */
void setup(void) {
  u8g2.begin();
//  u8g2a.begin();
}

/* draw something on the display with the `firstPage()`/`nextPage()` loop*/
void loop(void) {
  int xpos = 35;
  int xpos2 = 9;
  int ypos1 = 47;
  for (int i=0; i<= 13; i++){
    u8g2.firstPage();
    do {
      //u8g2.setFont(u8g2_font_fub35_tr);
      u8g2.setFont(u8g2_DcsFontHornet4_BIOS_09_tf);
      //u8g2_DcsFontHornet3_BIOS_09_tf
      //u8g2_DcsFontHornet4_BIOS_09_tf
      //u8g2.setFont(u8g2_font_fub35_tr);
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
    delay(100);
    
  }
  u8g2.firstPage();
  do {
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_ncenB12_tr);
    //u8g2.setFont(u8g2_DcsFontHornet4_BIOS_09_tf);
    u8g2.setCursor(xpos2,ypos1);
    u8g2.print("C");

    
  } while ( u8g2.nextPage() );
  delay(40);

}
