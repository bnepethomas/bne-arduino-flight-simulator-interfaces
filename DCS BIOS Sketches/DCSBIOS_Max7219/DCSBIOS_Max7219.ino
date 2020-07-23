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

int devices = 1;

LedControl lc=LedControl(9,8,7,1); 

/* paste code snippets from the reference documentation here */
DcsBios::LED sjCtrLt(0x742e, 0x4000, 13);
DcsBios::Switch2Pos sjCtr("SJ_CTR", 8);


void onRhAdvAaaChange(unsigned int newValue) {
  lc.setLed(0,1,1,newValue);    /* your code here */
}
DcsBios::IntegerBuffer rhAdvAaaBuffer(0x740a, 0x0800, 11, onRhAdvAaaChange);


void setup() {
  
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
