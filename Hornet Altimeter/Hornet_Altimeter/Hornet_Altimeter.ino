
#include <SPI.h>
#include <Ethernet.h>
#include <EthernetUdp.h>

byte mac[] = { 
  0xA9,0xE7,0x3E,0xCA,0x34,0x1D};
IPAddress ip(192,168,3,107);
IPAddress targetIP (192,168,3,100);
const unsigned int localport = 13139;
const unsigned int remoteport = 15151;

EthernetUDP udp;
char packetBuffer[200]; //buffer to store the incoming data
char outpacketBuffer[200]; //buffer to store the outgoing data


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




#include <Wire.h>
#define pinZeroSet 2

int sensorPin = A1; 

long thousandscounter = 0;
int pressure = 1013;
bool goingup = true; 
long startmillis = 0;
long lastAltimeterOutputPacket = 0;
long timetaken = 0;
bool debugging = false;
int AltimeterCurrentPos = 0;
int AltimeterDesiredPos = 0;
long NextIncrementDecrementTime = 0;
long LastAltitude = 0;


void setup()
{

 
    Serial.begin(115200);
    

    Ethernet.begin( mac, ip);
    udp.begin( localport );
    
    int x;
    StepperNumbers[0] = 0;      // Compass
    StepperNumbers[1] = 0;      // Clock
    CompassPos= cUnknownPos;
    CurrentDirection = cCounterClockwise;
    ClockCurrentDirection = cCounterClockwise;


    // Pin 2 input for zero position set
    pinMode( pinZeroSet, INPUT_PULLUP );
    
    for( int i = 22; i <= 42; i++)
    {
        pinMode( i, OUTPUT );
    }

   

    // If zero pos read only step a little bit
    for ( int zulu = 0; zulu < 1; zulu++)
    {
        for (int  x = 0; x < 3000; x++)
        {
          Serial.println("Winding back altimeter");
          StepCounterClockwise();
          
          val = analogRead(0);       // read analog input pin 0  
          Serial.println(val); // prints the value read
          if (val >= 200) {
            Serial.println("Zero exiting");
            CompassZeroed = true;
            delay(1000);
            x=5000;
          }
          //delayMicroseconds(clockdelay);
        }
        for (int  x = 0; x < 8 ; x++)
        {
          Serial.println("Winding to top altimeter");
          StepCounterClockwise();
        }
 
    }

   
    // If zero not found then step 20 steps clockwise

}

void StepCounterClockwise()
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
      CompassPos = CompassPos - 1;
    };
   
    CurrentDirection = cCounterClockwise;
}




void StepClockwise()
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
      CompassPos = CompassPos + 1;
    };
   
    CurrentDirection = cClockwise;
 
}




void loop()
{
 

    
    
    
    if (digitalRead(pinZeroSet) == LOW)
    {
      Serial.print("Zeroing ");
      Serial.println(millis());
      CompassPos = 0;
      // A cheats debounce
      delay(100);
      Serial.println("Zeroing Complete");
    }  
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





  int sensorValue = analogRead(sensorPin);
  // Serial.println(sensorValue);
  // Sensor Value Ranges 0 to 1023

  if( (millis() - lastAltimeterOutputPacket) > 1000){


      
      lastAltimeterOutputPacket = millis();
      Serial.println("Time to output a packet");
      Serial.print("Zero Sensor ");
      Serial.println(val);
      Serial.print("Potentiometer Sensor");
      Serial.println(sensorValue);

      
      sprintf((char*)outpacketBuffer,"%u",sensorValue);
      udp.beginPacket(targetIP, remoteport);
      udp.write(outpacketBuffer);
      udp.endPacket();
      
  }


   


  if ( millis() >= NextIncrementDecrementTime) {

     if (sensorValue > 500) { 
        goingup = true;

        NextIncrementDecrementTime = millis() + (1023 - sensorValue);
        }
    else {
        goingup = false;

        NextIncrementDecrementTime = millis() + (sensorValue);
    }
    if (goingup) {
      thousandscounter++;
 
 
      if (thousandscounter >= 99999) thousandscounter = 99999;
    }
    else{
      thousandscounter--;
      if (thousandscounter <= 0) thousandscounter = 0;
    }  
  }
  




   val = analogRead(0);       // read analog input pin 0  
   // Serial.println(val); // prints the value read


//  Serial.print("Altitude: ");
//  Serial.print(thousandscounter);

   // Load the desired set point
   
   //StepperNumbers[0] = int(((thousandscounter/10)%10) * 0.6  + (thousandscounter%10)* 0.06 + (thousandscounter%1)* 0.006);
   StepperNumbers[0] = int(((thousandscounter/10)%100) * 0.6); 
//   Serial.print("  Desired point :");
//   Serial.print(int(((thousandscounter/10)%10) * 0.6  + (thousandscounter%10)* 0.06 + (thousandscounter%1)* 0.006));
//   Serial.print(" Correct point ");
//   Serial.print(int(((thousandscounter/10)%100) * 0.6)); 
   //Serial.print( StepperNumbers[0]);



   // Process Compass
   if (!CompassZeroed)
   {
//    
//     if (val > 250)  {   
//           CompassPos= 0;
//           CompassZeroed = true;     
//      }
//      else
//        StepClockwise();
   }
   else 
   {
//      Serial.print(" Compass Pos: ");
//      Serial.print(CompassPos);
      

      // Serial.print("Micros: ");
      //erial.println(micros());
    
      // Check to see if we have hit the zero point
      //if (val > 250) {
      //  if (CurrentDirection == cClockwise)    
      //     CompassPos= 0;   
      //  else
      //     CompassPos = 4;  
      //}
  
      // Dlay when testing to verify we have hit zero point
      //if ((CompassPos == 0) & (LastCompassPos != 0)) {
      //    delay(6000);
      //}
      
      if (CompassPos >= 60) CompassPos = 0;
      if (CompassPos <= -1)  CompassPos = 59;

      LastCompassPos = CompassPos;

        
      if (CompassPos != StepperNumbers[0]) {
        // Not at desired desired compass position so move it
        // if we are climbing don't wind altimeter back to try and catch
//
        if (LastAltitude > thousandscounter) {
            //Serial.println("Descending");
            StepCounterClockwise(); }
       else if (LastAltitude < thousandscounter) {
            //Serial.println("Ascending");
            StepClockwise(); } 
            
        
//        if (((CompassPos - StepperNumbers[0]) < 30) &&
//           ((StepperNumbers[0] - CompassPos) < 30)  ) {
//          if (CompassPos > StepperNumbers[0]) 
//            StepCounterClockwise();
//          else 
//            StepClockwise();
//        }
//        else {
//          if (CompassPos > StepperNumbers[0]) 
//            StepClockwise();
//          else 
//            StepCounterClockwise();        
//        }
      }


   }
  
  
  
   LastAltitude = thousandscounter;
  
//   Serial.println("");  
    //delay(delaytime);
   
  

}

