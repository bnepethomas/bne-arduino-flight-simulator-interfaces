/*
  Tell DCS-BIOS to use a serial connection and use interrupt-driven
  communication. The main program will be interrupted to prioritize
  processing incoming data.
  
  This should work on any Arduino that has an ATMega328 controller
  (Uno, Pro Mini, many others).
 */
#define DCSBIOS_IRQ_SERIAL
#include "LedControl.h"
#include "DcsBios.h"

#define LEFT_EWI 0
#define RIGHT_EWI 1

// NO GO  - LEFT EWI - ORANGE
#define NO_GO_A_ROW 0
#define NO_GO_A_COL 0
#define NO_GO_B_ROW 1
#define NO_GO_B_COL 0

// GO  - LEFT EWI - GREEN
#define GO_A_ROW 2
#define GO_A_COL 0
#define GO_B_ROW 3
#define GO_B_COL 0

// RIGHT BLEED - LEFT EWI - RED
#define R_BLEED_A_ROW 0
#define R_BLEED_A_COL 1
#define R_BLEED_B_ROW 1
#define R_BLEED_B_COL 1


// LEFT BLEED - LEFT EWI - RED
#define L_BLEED_A_ROW 2
#define L_BLEED_A_COL 1
#define L_BLEED_B_ROW 3
#define L_BLEED_B_COL 1

// STBY - LEFT EWI - ORANGE
#define STBY_A_ROW 0
#define STBY_A_COL 2
#define STBY_B_ROW 1
#define STBY_B_COL 2

// SPD BRK- LEFT EWI - GREEN
#define SPD_BRK_A_ROW 2
#define SPD_BRK_A_COL 2
#define SPD_BRK_B_ROW 3
#define SPD_BRK_B_COL 2

// REC - LEFT EWI - ORANGE
#define REC_A_ROW 0
#define REC_A_COL 3
#define REC_B_ROW 1
#define REC_B_COL 3

// LAUNCH BAR RED - LEFT EWI - RED
#define L_BAR_RED_A_ROW 2
#define L_BAR_RED_A_COL 3
#define L_BAR_RED_B_ROW 3
#define L_BAR_RED_B_COL 3

// XMIT - LEFT EWI - GREEN
#define XMIT_A_ROW 0
#define XMIT_A_COL 4
#define XMIT_B_ROW 1
#define XMIT_B_COL 4

// LAUNCH BAR GREEN - LEFT EWI - GREEN
#define L_BAR_GREEN_A_ROW 2
#define L_BAR_GREEN_A_COL 4
#define L_BAR_GREEN_B_ROW 3
#define L_BAR_GREEN_B_COL 4

// ASPJ OH - LEFT EWI - ORANGE
#define ASPJ_OH_A_ROW 0
#define ASPJ_OH_A_COL 5
#define ASPJ_OH_B_ROW 1
#define ASPJ_OH_B_COL 5

// MASTER CAUTION - LEFT EWI - ORANGE
#define MSTR_CAUT_A_ROW 4
#define MSTR_CAUT_A_COL 1
#define MSTR_CAUT_B_ROW 4
#define MSTR_CAUT_B_COL 2
#define MSTR_CAUT_C_ROW 5
#define MSTR_CAUT_C_COL 1
#define MSTR_CAUT_D_ROW 5
#define MSTR_CAUT_D_COL 2

// LEFT FIRE - LEFT EWI - RED
#define LEFT_FIRE_A_ROW 6
#define LEFT_FIRE_A_COL 1
#define LEFT_FIRE_B_ROW 6
#define LEFT_FIRE_B_COL 2
#define LEFT_FIRE_C_ROW 7
#define LEFT_FIRE_C_COL 1
#define LEFT_FIRE_D_ROW 7
#define LEFT_FIRE_D_COL 2





// RCDR ON  - RIGHT EWI - GREEN
#define RCDR_ON_A_ROW 0
#define RCDR_ON_A_COL 0
#define RCDR_ON_B_ROW 1
#define RCDR_ON_B_COL 0

// DISP  - RIGHT EWI - GREEN
#define DISP_A_ROW 2
#define DISP_A_COL 0
#define DISP_B_ROW 3
#define DISP_B_COL 0

// SPARE 1 - RIGHT EWI - GREEN
#define SPARE_1_A_ROW 0
#define SPARE_1_A_COL 1
#define SPARE_1_B_ROW 1
#define SPARE_1_B_COL 1

// SPARE 2 - RIGHT EWI - GREEN
#define SPARE_2_A_ROW 2
#define SPARE_2_A_COL 1
#define SPARE_2_B_ROW 3
#define SPARE_2_B_COL 1

// SPARE 3 - RIGHT EWI - GREEN
#define SPARE_3_A_ROW 0
#define SPARE_3_A_COL 2
#define SPARE_3_B_ROW 1
#define SPARE_3_B_COL 2

// SPD BRK - RIGHT EWI - GREEN
#define SPARE_4_A_ROW 2
#define SPARE_4_A_COL 2
#define SPARE_4_B_ROW 3
#define SPARE_4_B_COL 2


// SPARE 5 - RIGHT EWI - GREEN
#define SPARE_5_A_ROW 0
#define SPARE_5_A_COL 3
#define SPARE_5_B_ROW 1
#define SPARE_5_B_COL 3

// SAM - RIGHT EWI - RED
#define SAM_A_ROW 2
#define SAM_A_COL 3
#define SAM_B_ROW 3
#define SAM_B_COL 3

// AI - RIGHT EWI - RED
#define AI_A_ROW 0
#define AI_A_COL 4
#define AI_B_ROW 1
#define AI_B_COL 4

// AAA - RIGHT EWI - RED
#define AAA_A_ROW 2
#define AAA_A_COL 4
#define AAA_B_ROW 3
#define AAA_B_COL 4

// CW - RIGHT EWI - RED
#define CW_A_ROW 0
#define CW_A_COL 5
#define CW_B_ROW 1
#define CW_B_COL 5

// APU FIRE - RIGHT EWI - RED
#define APU_FIRE_A_ROW 4
#define APU_FIRE_A_COL 1
#define APU_FIRE_B_ROW 4
#define APU_FIRE_B_COL 2
#define APU_FIRE_C_ROW 5
#define APU_FIRE_C_COL 1
#define APU_FIRE_D_ROW 5
#define APU_FIRE_D_COL 2

// RIGHT FIRE - RIGHT EWI - RED
#define RIGHT_FIRE_A_ROW 6
#define RIGHT_FIRE_A_COL 1
#define RIGHT_FIRE_B_ROW 6
#define RIGHT_FIRE_B_COL 2
#define RIGHT_FIRE_C_ROW 7
#define RIGHT_FIRE_C_COL 1
#define RIGHT_FIRE_D_ROW 7
#define RIGHT_FIRE_D_COL 2

 


#define STATUS_LED_PORT 6
int devices = 2;

LedControl lc=LedControl(9,8,7,devices); 

/* paste code snippets from the reference documentation here */
DcsBios::LED sjCtrLt(0x742e, 0x4000, 13);

void onRhAdvSamChange(unsigned int newValue) {
  lc.setLed(RIGHT_EWI,SAM_A_COL,SAM_A_ROW,newValue);
  lc.setLed(RIGHT_EWI,SAM_B_COL,SAM_B_ROW,newValue);
}
DcsBios::IntegerBuffer rhAdvSamBuffer(0x740a, 0x0200, 9, onRhAdvSamChange);



void onRhAdvSpareRh5Change(unsigned int newValue) {
  lc.setLed(RIGHT_EWI,SPARE_5_A_COL,SPARE_5_A_ROW,newValue);
  lc.setLed(RIGHT_EWI,SPARE_5_B_COL,SPARE_5_B_ROW,newValue);
}
DcsBios::IntegerBuffer rhAdvSpareRh5Buffer(0x740c, 0x0002, 1, onRhAdvSpareRh5Change);

void onRhAdvAiChange(unsigned int newValue) {
  lc.setLed(RIGHT_EWI,AI_A_COL,AI_A_ROW,newValue);
  lc.setLed(RIGHT_EWI,AI_B_COL,AI_B_ROW,newValue); 
}
DcsBios::IntegerBuffer rhAdvAiBuffer(0x740a, 0x0400, 10, onRhAdvAiChange);


void onRhAdvAaaChange(unsigned int newValue) {
  lc.setLed(RIGHT_EWI,AAA_A_COL,AAA_A_ROW,newValue);
  lc.setLed(RIGHT_EWI,AAA_B_COL,AAA_B_ROW,newValue);   
}
DcsBios::IntegerBuffer rhAdvAaaBuffer(0x740a, 0x0800, 11, onRhAdvAaaChange);


void onRhAdvCwChange(unsigned int newValue) {
  lc.setLed(RIGHT_EWI,CW_A_COL,CW_A_ROW,newValue);
  lc.setLed(RIGHT_EWI,CW_B_COL,CW_B_ROW,newValue);
}
DcsBios::IntegerBuffer rhAdvCwBuffer(0x740a, 0x1000, 12, onRhAdvCwChange);

void setup() {

//  pinMode(STATUS_LED_PORT, OUTPUT);
//  digitalWrite(STATUS_LED_PORT, 0);
//  delay(300);
//  digitalWrite(STATUS_LED_PORT, 1);
//  delay(300);
//  digitalWrite(STATUS_LED_PORT, 0);
//  delay(300); 
//  digitalWrite(STATUS_LED_PORT, 1);
//  delay(300); 
//  digitalWrite(STATUS_LED_PORT, 0);
//  delay(300);
//  digitalWrite(STATUS_LED_PORT, 1);
//  delay(300);
//  digitalWrite(STATUS_LED_PORT, 0);
//  delay(300);  

  
  devices=lc.getDeviceCount();
  
  for(int address=0;address<devices;address++) {
    /*The MAX72XX is in power-saving mode on startup*/
    lc.shutdown(address,false);
    /* Set the brightness to a medium values */
    lc.setIntensity(address,8);
    /* and clear the display */
    lc.clearDisplay(address);
  }
  DcsBios::setup();
}

void loop() {
  DcsBios::loop();
}
