// Test Harness for Max7219 
#include "LedControl.h"

/*
 Now we need a LedControl to work with.
 ***** These pin numbers will probably not work with your hardware *****
 pin 12 is connected to the DataIn 
 pin 11 is connected to the CLK 
 pin 10 is connected to LOAD 
 We have only a single MAX72XX.
 */

LedControl lc=LedControl(9,8,7,1);  

char receivedChar;
boolean newData = false;
/* we always wait a bit between updates of the display */
unsigned long delaytime=100;

void setup() {
  Serial.begin(115200);
  Serial.setTimeout(30000);
  /*
   The MAX72XX is in power-saving mode on startup,
   we have to do a wakeup call
   */
  lc.shutdown(0,false);
  /* Set the brightness to a medium values */
  lc.setIntensity(0,8);
  /* and clear the display */
  lc.clearDisplay(0);
}


void recvOneChar() {
    if (Serial.available() > 0) {
        receivedChar = Serial.read();
        newData = true;
    }
}

void showNewData() {
    if (newData == true) {
        Serial.print("This just in ... ");
        Serial.println(receivedChar);
        newData = false;
    }
}


void single() {

  for(int row=0;row<8;row++) {
    for(int col=0;col<8;col++) {
      delay(delaytime);
      Serial.println("Press Enter to Continue");

      newData = false;
      while (newData == false) {
        recvOneChar();
        delay(10);  
      }
      

      lc.clearDisplay(0);
      Serial.println("Row :" + String(row));
      Serial.println("Col :" + String(col));
      lc.setLed(0,row,col,true);
//      delay(delaytime);
//      for(int i=0;i<col;i++) {
//        lc.setLed(0,row,col,false);
//        delay(delaytime);
//        lc.setLed(0,row,col,true);
//        delay(delaytime);
//      }
    }
  }
}

void AllOn() {
  
  for(int row=0;row<8;row++) {
    for(int col=0;col<8;col++) {
      lc.setLed(0,row,col,true);
    } 
  }
}




void loop() { 
  Serial.println("All Leds on");
  AllOn();
  Serial.println("Press Enter to Continue");

  newData = false;
  while (newData == false) {
    recvOneChar();
    delay(100);  
  }
  lc.clearDisplay(0);      
  single();
}
