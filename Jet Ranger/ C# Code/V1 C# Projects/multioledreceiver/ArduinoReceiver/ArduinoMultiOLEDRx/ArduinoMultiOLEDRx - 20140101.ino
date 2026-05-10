//***************************************//
// --- WIDE.HK---//
// --- Revised Date : 06/30/2013
// --- I2C Arduino - Arduino UNO Demo ---//
// --- SSD131x PMOLED Controller      ---//
// --- SCL, SDA, GND, VCC(3.3v -5v)   ---//
//***************************************//

#include <Wire.h>          // *** I2C Mode 
#include <LCDI2C4Bit.h>

// lcd
int ADDR = 0xA7;
byte x = 0;
byte data = 1;
byte c;
LCDI2C4Bit lcd = LCDI2C4Bit(ADDR,4,20);
// lcd

#define OLED_Address 0x3c
#define OLED_Command_Mode 0x80
#define OLED_Data_Mode 0x40

#define cmd_CLS 0x01

#define STX 0x02
#define ETX 0x03

// FSM States
#define stateWaitingForSerialData   0
#define stateLookingForStartOfMsg   1
#define stateLookingForOLEDPtr      2
#define stateLookingForLinePtr      3
#define stateRxAndDisplayChar       4

#define minoledPtr 1
#define maxoledPtr 7

#define minLinePtr 1
#define maxLinePtr 2

// Map Output Pins to OLED Displays
int oled1 = 2;                  // First usable output pin is D2 so all pins are incremented by 2 
int oled2 = 3;
int oled3 = 4;
int oled4 = 5;
int oled5 = 6;
int oled6 = 7;
int oled7 = 8;



int fsmState = stateWaitingForSerialData;

char byteIn = 0;

char currentoled = 0;

void setup()
{
  initialiseOutPutPins();
  
  Wire.begin();          // Start the I2C bus
  
 // lcd
   lcd.init();
  lcd.clear();
  //lcd.cursorTo(0,0);
  //lcd.cursorTo(2,0);
  lcd.backLight(true);
 // lcd
 
 
 // *** I2C initial *** //
  delay(100);
   sendCommand(0x2A);	// **** Set "RE"=1	00101010B
   sendCommand(0x71);
   sendCommand(0x5C);
   sendCommand(0x28);
  
   sendCommand(0x08);	// **** Set Sleep Mode On
   sendCommand(0x2A);	// **** Set "RE"=1	00101010B
   sendCommand(0x79);	// **** Set "SD"=1	01111001B
  
   sendCommand(0xD5);
   sendCommand(0x70);
   sendCommand(0x78);	// **** Set "SD"=0
  
   sendCommand(0x08);	// **** Set 5-dot, 3 or 4 line(0x09), 1 or 2 line(0x08)
  
   sendCommand(0x06);	// **** Set Com31-->Com0  Seg0-->Seg99
  
   // **** Set OLED Characterization *** //
   sendCommand(0x2A);  	// **** Set "RE"=1 
   sendCommand(0x79);  	// **** Set "SD"=1
  
   // **** CGROM/CGRAM Management *** //
   sendCommand(0x72);  	// **** Set ROM
   sendCommand(0x00);  	// **** Set ROM A and 8 CGRAM
  
  
   sendCommand(0xDA); 	// **** Set Seg Pins HW Config
   sendCommand(0x10);   
  
   sendCommand(0x81);  	// **** Set Contrast
   sendCommand(0xFF); 
  
   sendCommand(0xDB);  	// **** Set VCOM deselect level
   sendCommand(0x30);  	// **** VCC x 0.83
  
   sendCommand(0xDC);  	// **** Set gpio - turn EN for 15V generator on.
   sendCommand(0x03);
  
   sendCommand(0x78);  	// **** Exiting Set OLED Characterization
   sendCommand(0x28); 
   sendCommand(0x2A); 
   //sendCommand(0x05); 	// **** Set Entry Mode
   sendCommand(0x06); 	// **** Set Entry Mode
   sendCommand(0x08);  
   sendCommand(0x28); 	// **** Set "IS"=0 , "RE" =0 //28
   sendCommand(0x01); 
   sendCommand(0x80); 	// **** Set DDRAM Address to 0x80 (line 1 start)
  
   delay(100);
   sendCommand(0x0C);  	// **** Turn on Display  

  displayOLEDNames();
  lcd.clear();
  send_string("   Waiting for");  
  sendCommand(0xC0);
  lcd.cursorTo(2,0);
  send_string("     Sender"); 
  
  // Toggle All LEDs to indicate ready to roll
  deselectAlloled();
  delay(300);
  selectAlloled();
  delay(300);
  deselectAlloled();
  delay(300);
  selectAlloled();
  delay(300);
   
  
  Serial.begin(115200);
  establishContact();  // send a byte to establish contact until receiver responds 
  send_string("Response Received");   
  sendCommand(0x01);
  
}

void loop()
{
 

 fsmState = stateLookingForStartOfMsg;
 // selectoled(3);
 while(1)
  {
   if (Serial.available() > 0) 
     {
      // get incoming byte:
      byteIn = Serial.read();
      
      switch (fsmState) {
        
        case stateWaitingForSerialData:
          //Curretly caught in setup script
          break;
        
        case stateLookingForStartOfMsg:
          if (byteIn ==  STX) {
            fsmState = stateLookingForOLEDPtr;
          }
          break;
        
        case stateLookingForOLEDPtr:
        
          // Check to see if oledPtr is within expected range
          // If not return to lokoing for Start of Message
          
          if (byteIn < minoledPtr || byteIn > maxoledPtr) {
            fsmState = stateLookingForStartOfMsg;
          } else {
            selectoled(byteIn);
            currentoled = byteIn;
            //lcd.cursorTo(1,0);
            //lcd.backLight(false);
            fsmState = stateLookingForLinePtr;
          }
          break;
        
        case stateLookingForLinePtr:
        
          // Check to see if LinePtr is within expected range
          // If not return to looking for Start of Message
          
          if (byteIn < minLinePtr || byteIn > maxLinePtr) {
            fsmState = stateLookingForStartOfMsg;
          } else 
          {
            //selectoled(byteIn);
            // Currently only working for LCD
            // LCD commands are real bottle necks only send if required
            if ((currentoled == 2) || (currentoled == 7))
            {
              switch (byteIn) 
              {
                case 1: 
                  lcd.cursorTo(0,0);
                  break;
                 case 2:
                  lcd.cursorTo(2,0);
                  break;
                default:
                  // do nothing  
                 fsmState = fsmState; 
              };
            }
            else // OLED
            {
              switch (byteIn) 
              {
                case 1: 
                  sendCommand(0x80);
                  break;
                 case 2:
                  sendCommand(0x80);
                  sendCommand(0xC0);
                  break;
                default:
                  // do nothing  
                 fsmState = fsmState; 
              };
            };            
            fsmState = stateRxAndDisplayChar;
          };
          break;
          
        case stateRxAndDisplayChar:
        
          // Receive characters until ETX is received
          if (byteIn ==  ETX) {
            fsmState = stateLookingForStartOfMsg;
          } 
          else 
          { 
             if (byteIn == 0x7F) { // Clear Screen
              sendCommand(0x01);
              
            if ((currentoled == 2) || (currentoled == 7)){
                lcd.clear();
                //lcd.backLight(false);
              }
              //delay(10);
            }
            else if (byteIn == 0x0D) { // CR
              //sendCommand(0xC0);  //Note this is just CR not CR LF
            }
            else if (byteIn == 0x04) { // DEL - CLS
              sendCommand(0x80); // Home Pos
              //delay(10);
              // sendCommand(0x2C); // large fonts
              //sendCommand(0x0F);  	// BLink Cursor
            }
            else if (byteIn == 0x05) { // DEL - CLS
              //sendCommand(0x02); // Home Pos  This causes flicker so try and avoid
              sendCommand(0xC0); 
              //delay(10);
              // sendCommand(0x2C); // large fonts
              //sendCommand(0x0F);  	// BLink Cursor
            }
            else 
            {
              sendData(byteIn);
              // Only send data to LCDs when needed
              if  ((currentoled == 2) || (currentoled == 7))
                lcdsendData(byteIn);
            }
          }
          break;
        default: 
        
          // Currently just a place holder
          fsmState = fsmState;

      };
    
    };
  };

 // **** Show Data Value *** //
 // ********************************************************************//

}


void sendData(unsigned char data)
{
    Wire.beginTransmission(OLED_Address);  	// **** Start I2C 
    Wire.write(OLED_Data_Mode);     		// **** Set OLED Data mode
    Wire.write(data);
    Wire.endTransmission();                     // **** End I2C 
    
}

void sendCommand(unsigned char command)
{
    Wire.beginTransmission(OLED_Address); 	 // **** Start I2C 
    Wire.write(OLED_Command_Mode);     		 // **** Set OLED Command mode
    Wire.write(command);
    Wire.endTransmission();                 	 // **** End I2C 
    //delay(10);
}

void send_string(const char *String)
{
    unsigned char i=0;
    while(String[i])
    {
        sendData(String[i]);      // *** Show String to OLED
        lcdsendData(String[i]);
        i++;
    }

}

void lcdsendData(unsigned char data) {

  // Need to work out nice way to convert char to pointer
  String test = "0";
  test[0] = data;
  lcd.printIn(&test[0]);
}


void establishContact() {
  while (Serial.available() <= 0) {
    Serial.println("0,0,0");   // send an initial string
    delay(300);
  }
}


void initialiseOutPutPins() {
  pinMode(oled1,OUTPUT);
  pinMode(oled2,OUTPUT);
  pinMode(oled3,OUTPUT);
  pinMode(oled4,OUTPUT);
  pinMode(oled5,OUTPUT);
  pinMode(oled6,OUTPUT);
  pinMode(oled7,OUTPUT);

  digitalWrite(oled1, HIGH);
  digitalWrite(oled2, HIGH);
  digitalWrite(oled3, HIGH);
  digitalWrite(oled4, HIGH);
  digitalWrite(oled5, HIGH);
  digitalWrite(oled6, HIGH); 
  digitalWrite(oled7, HIGH);

}

void selectoled( int oledToWriteTo) {

  int mappedPin;  
  

  // Lower all pins and then raise the desired one

  deselectAlloled(); 

  switch (oledToWriteTo) {
 
    case 1:
      mappedPin = oled1;
      break;
    case 2:
      mappedPin = oled2;
      break;  
    case 3:
      mappedPin = oled3;
      break;
    case 4:
      mappedPin = oled4;
      break; 
    case 5:
      mappedPin = oled5;
      break;
    case 6:
      mappedPin = oled6;
      break; 
    case 7:
      mappedPin = oled7;
      break;
      default:
      selectAlloled();  
  }
  
  digitalWrite(mappedPin, HIGH);
}
 
 
void deselectAlloled() {
  digitalWrite(oled1, LOW);
  digitalWrite(oled2, LOW);
  digitalWrite(oled3, LOW);
  digitalWrite(oled4, LOW);
  digitalWrite(oled5, LOW);
  digitalWrite(oled6, LOW); 
  digitalWrite(oled7, LOW);   

}
 
void selectAlloled() {
  
  // Lower all pins and then raise the desired one
  digitalWrite(oled1, HIGH);
  digitalWrite(oled2, HIGH);
  digitalWrite(oled3, HIGH);
  digitalWrite(oled4, HIGH);
  digitalWrite(oled5, HIGH);
  digitalWrite(oled6, HIGH); 
  digitalWrite(oled7, HIGH);  
  

}
 
void displayOLEDNames() {
 
  selectoled(1);
  send_string("Radio 1");

  selectoled(2);
  send_string("Radio 2");

  selectoled(3);
  send_string("Radio 3");

  selectoled(4);
  send_string("Radio 4");

  selectoled(5);
  send_string("Radio 5");

  selectoled(6);
  send_string("Radio 6");

  selectoled(7);
  send_string("Radio 7");
 
  delay(1000); 
  selectAlloled();
  sendCommand(cmd_CLS);              // Clear Display
  lcd.clear();
   
  
}
