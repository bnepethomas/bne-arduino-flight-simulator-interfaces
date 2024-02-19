#include <SPI.h>
//EPD
#include "Display_EPD_W21_spi.h"
#include "Display_EPD_W21.h"
#include "Ap_29demo.h"

#include "ozhornet_epaper_font_data.h"
#include "ozhornet_epaper_hash_data.h"



void setup() {
#ifdef ESP8266
  pinMode(D0, INPUT);   //BUSY
  pinMode(D1, OUTPUT);  //RES
  pinMode(D2, OUTPUT);  //DC
  pinMode(D4, OUTPUT);  //CS
#endif
#ifdef Arduino_UNO
  pinMode(4, INPUT);   //BUSY
  pinMode(5, OUTPUT);  //RES
  pinMode(6, OUTPUT);  //DC
  pinMode(7, OUTPUT);  //CS
#endif
  //SPI
  SPI.beginTransaction(SPISettings(10000000, MSBFIRST, SPI_MODE0));
  SPI.begin();
}

//Tips//
/*
1.Flickering is normal when EPD is performing a full screen update to clear ghosting from the previous image so to ensure better clarity and legibility for the new image.
2.There will be no flicker when EPD performs a partial refresh.
3.Please make sue that EPD enters sleep mode when refresh is completed and always leave the sleep mode command. Otherwise, this may result in a reduced lifespan of EPD.
4.Please refrain from inserting EPD to the FPC socket or unplugging it when the MCU is being powered to prevent potential damage.)
5.Re-initialization is required for every full screen update.
6.When porting the program, set the BUSY pin to input mode and other pins to output mode.
*/
void loop() {
  // unsigned char i;


  EPD_Init(); //Full screen refresh initialization.
  //EPD_WhiteScreen_White(); //Clear screen function.

  for (int i = 79; i > 0; i--) {
    if (i <= 7) {
      EPD_Dis_Part_Time(15, 4, petetest[i], hashtest[i],  Num[0], Num[0], Num[0], 2, 48, 48);  //x,y,DATA-A~E,number,Resolution 32*32
    } else {
      EPD_Dis_Part_Time(15, 4, petetest[i], petetest[i], Num[0], Num[0], Num[0], 2, 48, 48);  //x,y,DATA-A~E,number,Resolution 32*32
    }
    EPD_DeepSleep();  //Enter the sleep mode and please do not delete it, otherwise it will reduce the lifespan of the screen.
    //delay(0);
  }

  EPD_Init(); //Full screen refresh initialization.


  for (int i = 1; i <= 79; i++) {
    if (i <= 7) {
      EPD_Dis_Part_Time(15, 4, petetest[i], hashtest[i],  Num[0], Num[0], Num[0], 2, 48, 48);  //x,y,DATA-A~E,number,Resolution 32*32
    } else {
      EPD_Dis_Part_Time(15, 4, petetest[i], petetest[i], Num[0], Num[0], Num[0], 2, 48, 48);  //x,y,DATA-A~E,number,Resolution 32*32
    }
    EPD_DeepSleep();  //Enter the sleep mode and please do not delete it, otherwise it will reduce the lifespan of the screen.
    //delay(0);
  }
EPD_DeepSleep();


#ifdef Arduino_UNO
  while (1)
    ;  // The program stops here
#endif
}




//////////////////////////////////END//////////////////////////////////////////////////
