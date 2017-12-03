// Compass working now working on clock

#include <SPI.h>
#include <Ethernet.h>
#include <EthernetUdp.h>

byte mac[] = { 
	0xA9,0xE7,0x3E,0xCA,0x34,0x1D};
IPAddress ip(192,168,1,107);
const unsigned int localport = 13135;

EthernetUDP udp;
char packetBuffer[200]; //buffer to store the incoming data



int val, Clock_val;
int CompassPos, ClockPos;
int LastCompassPos = 0;
int LastClockPos = 0;
int CurrentDirection, ClockCurrentDirection;
boolean CompassZeroed = false;
boolean ClockZeroed = false;
double StepperNumbers[4];

const int MaxSteps = 3780;
const int delaytime = 10;
// const int clockdelay = 23430;            // Should actually be 23.43mS
const int clockdelay = 2000;            // Should actually be 23.43mS
const int cClockwise = 1;
const int cCounterClockwise = -1;
const int cUnknownPos = 999;
void setup()
{

    Ethernet.begin( mac, ip);
    udp.begin( localport );
  
    int x;
    StepperNumbers[0] = 0;      // Compass
    StepperNumbers[1] = 0;      // Clock
    CompassPos= cUnknownPos;
    CurrentDirection = cCounterClockwise;
    ClockCurrentDirection = cCounterClockwise;
    Serial.begin(115200);  

    for( int i = 22; i <= 42; i++)
    {
        pinMode( i, OUTPUT );
    }

   

    // If zero pos read only step a little bit
    for ( int zulu = 0; zulu < 5; zulu++)
    {
        for (int  x = 0; x < 1200; x++)
        {
          Serial.println("Winding back Compass");
          StepCounterClockwise();
          
          val = analogRead(0);       // read analog input pin 0  
          Serial.println(val); // prints the value read
          if (val >= 200) {
            Serial.println('found something');
            delay(5000);
          }
          //delayMicroseconds(clockdelay);
        }
  
        for (int  x = 0; x < 1200; x++)
        {
          Serial.println("Winding forward Compass");
          StepClockwise();
          val = analogRead(0);       // read analog input pin 0  
          Serial.println(val); // prints the value read
          if (val >= 300) {
            Serial.println('found something');
            delay(5000);
          }
          //delayMicroseconds(clockdelay);
        }
    }
    //Step Counterwise 10 steps and see if zero found
        for (int  x = 0; x < 10; x++)
      {
        Serial.println("Winding back Clock");
        ClockStepCounterClockwise();
        delayMicroseconds(clockdelay);
      } 

      //Step Counterwise 10 steps and see if zero found
      for (int  x = 0; x < 10; x++)
      {
        Serial.println("Winding forward Clock");
        ClockStepClockwise();
        delayMicroseconds(clockdelay);
      } 

   
    // If zero not found then step 20 steps clockwise

}


//pin 22 23 24 - PA0 PA1 PA2
void sendCmdTo595_1( const byte cmd )
{
    byte bitMask = B00000001;
    for( int i = 0; i < 8; i++)
    {
        /*
        WARNING!!!  don't use digitalWrite here because it's speed is too slow,
        use avr c style to control the IO pin HIGH / LOW
        http://arduino.cc/en/Hacking/PinMapping2560
        http://www.instructables.com/id/Arduino-is-Slow-and-how-to-fix-it/?ALLSTEPS
        */
        PORTA = (bitMask & cmd) > 0 ? (PORTA | B00000001) : (PORTA & B11111110);
        //give a raise pulse to 595 input clock pin
        PORTA &= B11111101;
        PORTA |= B00000010;

        bitMask = bitMask << 1;
    }
    //send data from shift register to storage register
    PORTA &= B11111011;
    PORTA |= B00000100;
}


void StepClockwise()
{

    //Digital Port 22 Maps to PortA-0
    //Digital Port 23 Maps to PortA-1

    //Stepping order based on page 2 of VID69 clock datasheet
    //Delay between steps is 3*7.81mS
    //Total pulse train 164mS is 7 steps

    //Step 1
  
    PORTA &= B00001001;
    PORTA |= B00001001;
    delayMicroseconds(clockdelay);

    //Step 2
  
    PORTA &= B00000001;
    PORTA |= B00000001;
    delayMicroseconds(clockdelay);

    //Step 3
  
    PORTA &= B00000111;
    PORTA |= B00000111;
    delayMicroseconds(clockdelay);

    //Step 4
  
    PORTA &= B00000110;
    PORTA |= B00000110;
    delayMicroseconds(clockdelay);

    //Step 5
  
    PORTA &= B00001110;
    PORTA |= B00001110;
    delayMicroseconds(clockdelay);

    //Step 6
  
    PORTA &= B00001000;
    PORTA |= B00001000;
    delayMicroseconds(clockdelay);

    //Step 7
  
    PORTA &= B00001001;
    PORTA |= B00001001;
    delayMicroseconds(clockdelay);

    if (CompassPos != cUnknownPos)
    {
      CompassPos = CompassPos + 1;
    };
   
    CurrentDirection = cClockwise;
}

void ClockStepClockwise()
{

    //Digital Port 22 Maps to PortA-0
    //Digital Port 23 Maps to PortA-1

    //Stepping order based on page 2 of VID69 clock datasheet
    //Delay between steps is 3*7.81mS
    //Total pulse train 164mS is 7 steps

    //Step 1
  
    PORTA &= B00000000;
    PORTA |= B00000000;
    delayMicroseconds(clockdelay);

    //Step 2
  
    PORTA &= B10000000;
    PORTA |= B10000000;
    delayMicroseconds(clockdelay);

    //Step 3
  
    PORTA &= B11100000;
    PORTA |= B11100000;
    delayMicroseconds(clockdelay);

    //Step 4
  
    PORTA &= B01100000;
    PORTA |= B01100000;
    delayMicroseconds(clockdelay);

    //Step 5
  
    PORTA &= B01110000;
    PORTA |= B01110000;
    delayMicroseconds(clockdelay);

    //Step 6
  
    PORTA &= B00010000;
    PORTA |= B00010000;
    delayMicroseconds(clockdelay);

    //Step 7
  
    PORTA &= B10010000;
    PORTA |= B10010000;
    delayMicroseconds(clockdelay);

    if (ClockPos != cUnknownPos)
    {
      ClockPos = ClockPos + 1;
    };
   
    ClockCurrentDirection = cClockwise;
 
}





void StepCounterClockwise()
{

    //Digital Port 22 Maps to PortA-0
    //Digital Port 23 Maps to PortA-1

    //Stepping order based on page 2 of VID69 clock datasheet
    //Delay between steps is 3*7.81mS
    //Total pulse train 164mS is 7 steps

    //Step 1
  
    PORTA &= B00000000;
    PORTA |= B00000000;
    delayMicroseconds(clockdelay);

    //Step 2
  
    PORTA &= B00001000;
    PORTA |= B00001000;
    delayMicroseconds(clockdelay);

    //Step 3
  
    PORTA &= B00001110;
    PORTA |= B00001110;
    delayMicroseconds(clockdelay);

    //Step 4
  
    PORTA &= B00000110;
    PORTA |= B00000110;
    delayMicroseconds(clockdelay);

    //Step 5
  
    PORTA &= B00000111;
    PORTA |= B00000111;
    delayMicroseconds(clockdelay);

    //Step 6
  
    PORTA &= B00000001;
    PORTA |= B00000001;
    delayMicroseconds(clockdelay);

    //Step 7
  
    PORTA &= B00001001;
    PORTA |= B00001001;
    delayMicroseconds(clockdelay);

    if (CompassPos != cUnknownPos)
    {
      CompassPos = CompassPos - 1;
    };
   
    CurrentDirection = cCounterClockwise;
 
}


void ClockStepCounterClockwise()
{

    //Digital Port 22 Maps to PortA-0
    //Digital Port 23 Maps to PortA-1

    //Stepping order based on page 2 of VID69 clock datasheet
    //Delay between steps is 3*7.81mS
    //Total pulse train 164mS is 7 steps

    //Step 1
  
    PORTA &= B10010000;
    PORTA |= B10010000;
    delayMicroseconds(clockdelay);

    //Step 2
  
    PORTA &= B00010000;
    PORTA |= B00010000;
    delayMicroseconds(clockdelay);

    //Step 3
  
    PORTA &= B01110000;
    PORTA |= B01110000;
    delayMicroseconds(clockdelay);

    //Step 4
  
    PORTA &= B01100000;
    PORTA |= B01100000;
    delayMicroseconds(clockdelay);

    //Step 5
  
    PORTA &= B11100000;
    PORTA |= B11100000;
    delayMicroseconds(clockdelay);

    //Step 6
  
    PORTA &= B10000000;
    PORTA |= B10000000;
    delayMicroseconds(clockdelay);

    //Step 7
  
    PORTA &= B10010000;
    PORTA |= B10010000;
    delayMicroseconds(clockdelay);

    if (ClockPos != cUnknownPos)
    {
      ClockPos = CompassPos - 1;
    };
   
    ClockCurrentDirection = cCounterClockwise;
}

void loop()
{
 

    // Grab UDP Packet
    int packetSize = udp.parsePacket();
    if( packetSize > 0) {
        Serial.println("Packet Received");
        udp.read( packetBuffer, packetSize);
  	 //terminate the buffer manually
  	 packetBuffer[packetSize] = '\0';
        
        if (packetBuffer[0] == 'S') {    
          // We have some data
          Serial.println(packetBuffer);
            // Split the string up
           char* endPos;
           int i = 0;
           char *p = packetBuffer + 1;
           char *str;
           while ((str = strtok_r(p, ":", &p)) != NULL) 
           {// delimiter is the semicolon
             Serial.print( i);
             Serial.print("-");
             Serial.println(str);
             StepperNumbers[i] = strtod(str, &endPos);
             i++;
             
          }
    	}	
    }
  

   val = analogRead(0);       // read analog input pin 0  
   // Serial.println(val); // prints the value read
   Clock_val = analogRead(1);   
   //Serial.println(Clock_val); // prints the value read

   

   // Process Compass
   if (!CompassZeroed)
   {
    
     if (val > 250)  {   
           CompassPos= 0;
           CompassZeroed = true;     
      }
      else
        StepClockwise();
   }
   else 
   {
      // Serial.println(CompassPos);
    
      // Check to see if we have hit the zero point
      if (val > 250) {
        if (CurrentDirection == cClockwise)    
           CompassPos= 0;   
        else
           CompassPos = 4;  
      }
  
      // Dlay when testing to verify we have hit zero point
      //if ((CompassPos == 0) & (LastCompassPos != 0)) {
      //    delay(6000);
      //}
      
      if (CompassPos == 720) CompassPos = 0;
      if (CompassPos == -1)  CompassPos = 719;
     
      LastCompassPos = CompassPos;
      if (CompassPos != StepperNumbers[0]) {
        // Not at desired desired compass position so move it
        if (((CompassPos - StepperNumbers[0]) < 360) &&
           ((StepperNumbers[0] - CompassPos) < 360)  ) {
          if (CompassPos > StepperNumbers[0]) 
            StepCounterClockwise();
          else 
            StepClockwise();
        }
        else {
          if (CompassPos > StepperNumbers[0]) 
            StepClockwise();
          else 
            StepCounterClockwise();        
        }
      }
      // StepCounterClockwise();
   }
  
  
  
     // Process Clock
   if (!ClockZeroed)
   {
    
     if (Clock_val > 500)  {   
           ClockPos= 716;
           ClockZeroed = true;   

      }
      else
        ClockStepClockwise();
   }
   else 
   {
      // Serial.println(CompassPos);
    
      // Check to see if we have hit the zero point
      if (val > 450) {
        if (ClockCurrentDirection == cClockwise)    
           ClockPos= 716;   
        else
           ClockPos = 716;  
      }
  
      // Dlay when testing to verify we have hit zero point
      //if ((CompassPos == 0) & (LastCompassPos != 0)) {
      //    delay(6000);
      //}
      
      if (ClockPos == 720) ClockPos = 0;
      if (ClockPos == -1)  ClockPos = 719;
     
      LastClockPos = ClockPos;
      if (ClockPos != StepperNumbers[1]) {
            ClockStepClockwise();
      }

   }
  
  
  
  
    delay(delaytime);
   
  

}

