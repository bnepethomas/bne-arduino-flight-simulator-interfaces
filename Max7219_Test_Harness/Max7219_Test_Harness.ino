// Test Harness for Max7219 
#include "LedControl.h"
// NEED TO SWAP COLS and ROWs to match PCB
/*
 Now we need a LedControl to work with.
 ***** These pin numbers will probably not work with your hardware *****
 pin 12 is connected to the DataIn 
 pin 11 is connected to the CLK 
 pin 10 is connected to LOAD 
 We have only a single MAX72XX.
 */

// Single Max7219
//LedControl lc=LedControl(9,8,7,1);  

//Two Max7219
LedControl lc=LedControl(9,8,7,4); 

char receivedChar;
boolean newData = false;
/* we always wait a bit between updates of the display */
unsigned long delaytime=100;

int devices = 1;

void setup() {
  Serial.begin(115200);
  Serial.setTimeout(30000);
  /*
   The MAX72XX is in power-saving mode on startup,
   we have to do a wakeup call
   */

  devices=lc.getDeviceCount();
  
  for(int address=0;address<devices;address++) {
    /*The MAX72XX is in power-saving mode on startup*/
    lc.shutdown(address,false);
    /* Set the brightness to a medium values */
    lc.setIntensity(address,8);
    /* and clear the display */
    lc.clearDisplay(address);
  }
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
  for(int address=0;address<devices;address++) {
    for(int row=0;row<8;row++) {
      for(int col=0;col<8;col++) {
        delay(delaytime);
        Serial.println("Press Enter to Continue");
  
        newData = false;
        while (newData == false) {
          recvOneChar();
          delay(10);  
        }
        
  
        lc.clearDisplay(address);
        Serial.println("Device:" + String(address));
        Serial.println("Row   :" + String(row));
        Serial.println("Col   :" + String(col));
        lc.setLed(address,row,col,true);
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
}

void AllOn() {
  for(int address=0;address<devices;address++) {  
    for(int row=0;row<8;row++) {
      for(int col=0;col<8;col++) {
        lc.setLed(address,row,col,true);
      } 
    }
  }
}

void AllOff() {
  for(int address=0;address<devices;address++) {  
    for(int row=0;row<8;row++) {
      for(int col=0;col<8;col++) {
        lc.setLed(address,row,col,false);
      } 
    }
  }
}


void loop() { 
  Serial.println("There are " + String(devices) + " devices");
  Serial.println("All Leds on");
  AllOn();
  Serial.println("Press Enter to Continue");

  newData = false;
  while (newData == false) {
    recvOneChar();
    delay(100);  
  }
  AllOff();     
  single();
}
