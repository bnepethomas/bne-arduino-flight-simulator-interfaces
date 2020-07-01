#include <Arduino.h>
#include <SPI.h>
#include <U8g2lib.h>
//#include "hornet_font.c"
#include "hornet_font.h"
#include "dseg14_v3.h"
//#include "u8g2_font.c"
//#include "u8g2_fonts.c"

// For sme reason the small display is twitchy
// After shutting day perfectly the day befoew
// first thing in the morning the output was a 
// mess
// after a few minutes it settled down which is really weird
// Increasingly this looks like either a temperature related H/W issue or some
// like a precharge isn't bring something to the level it needs to be
// Oir more likely need Pull up resistors as behaviour changed when I touched the I2C cable

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
  int xpos2 = 6;
  int ypos1 = 47;
  for (int i=0; i<= 13; i++){
    u8g2.firstPage();
    do {
      //u8g2.setFont(u8g2_font_fub35_tr);
      //u8g2.setFont(DSEG14_Classic_Regular_16);
      //u8g2.setFont(u8g2_DcsFontHornet4_BIOS_09_tf);
      u8g2.setFont(u8g2_font_7Segments_26x42_mn);
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
    delay(50);
    
  }
  u8g2.firstPage();
  do {
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_7Segments_26x42_mn);
    //u8g2.setFont(u8g2_DcsFontHornet4_BIOS_09_tf);
    u8g2.setCursor(xpos2,ypos1);
    u8g2.print("C");

    
  } while ( u8g2.nextPage() );
  delay(400);

}
