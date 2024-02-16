#include <SPI.h>
//EPD
#include "Display_EPD_W21_spi.h"
#include "Display_EPD_W21.h"
#include "Ap_29demo.h"

#include "ozhornet_epaper_font_data.h"

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
  unsigned char i;
#if 1  //Full screen refresh, fast refresh, and partial refresh demostration.

  // EPD_Init(); //Full screen refresh initialization.
  // EPD_WhiteScreen_White(); //Clear screen function.


#if 1                                       //Partial refresh demostration. \
                                            //Partial refresh demo support displaying a clock at 5 locations with 00:00.  If you need to perform partial refresh more than 5 locations, please use the feature of using partial refresh at the full screen demo. \
                                            //After 5 partial refreshes, implement a full screen refresh to clear the ghosting caused by partial refreshes. \
                                            //////////////////////Partial refresh time demo/////////////////////////////////////
  EPD_Init();                               //Electronic paper initialization.
  EPD_SetRAMValue_BaseMap(gImage_basemap);  //Please do not delete the background color function, otherwise it will cause unstable display during partial refresh.
                                            // for(i=0;i<6;i++)
                                            // {
                                            //   EPD_Dis_Part_Time(0,15,Num[i],Num[0],gImage_numdot,Num[0],Num[1],5,32,48); //x,y,DATA-A~E,number,Resolution 32*32
                                            // }
                                            //EPD_Dis_Part_Time(0,15,Num[2],Num[0],gImage_numdot,Num[0],Num[1],1,32,48); //x,y,DATA-A~E,number,Resolution 32*32
                                            // EPD_Dis_Part_Time(0, 15, petetest[1], Num[0], gImage_numdot, Num[0], Num[1], 1, 48, 48);  //x,y,DATA-A~E,number,Resolution 32*32

  for (i = 0; i < 9; i++) {
    EPD_Dis_Part_Time(0, 15, petetest[i], Num[0], gImage_numdot, Num[0], Num[1], 1, 48, 48);  //x,y,DATA-A~E,number,Resolution 32*32
    EPD_DeepSleep();                                                                          //Enter the sleep mode and please do not delete it, otherwise it will reduce the lifespan of the screen.
    delay(500);
  }

#endif



#endif

#ifdef ESP8266
  while (1) {
    Sys_run();  //System run
    LED_run();  //Breathing lamp
  }
#endif
#ifdef Arduino_UNO
  while (1)
    ;  // The program stops here
#endif
}




//////////////////////////////////END//////////////////////////////////////////////////
