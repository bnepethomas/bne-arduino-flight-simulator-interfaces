
#include <SPI.h>
#include <Ethernet.h>
#include <EthernetUdp.h>

// Need to check if this manually assigned Mac has been used elsewhere
byte mac[] = { 
  0xA9,0xE7,0x3E,0xCA,0x34,0x1D};
IPAddress ip(192,168,3,107);
IPAddress targetIP (192,168,3,101);
const unsigned int localport = 13139;
const unsigned int remoteport = 15151;

EthernetUDP udp;
char packetBuffer[200];     //buffer to store the incoming data
char outpacketBuffer[200];  //buffer to store the outgoing data


int val;
int Alt_Hundreds_Pointer_Pos;
int LastAlt_Hundreds_Pointer_Pos = 0;
int CurrentDirection;
boolean AltimeterZeroed = false;
double StepperNumbers[4];

const int MaxSteps = 3780;
const int delaytime = 10;
// const int AltimeterDelay = 23430;            // Should actually be 23.43mS
//const int AltimeterDelay = 2000;
const int AltimeterDelay = 3000;            // Should actually be 23.43mS
const int cClockwise = 1;
const int cCounterClockwise = -1;
const int cUnknownPos = 999;



#include <Wire.h>


int sensorPin = A1; 
int KollmansAdjustPin = A2;
unsigned long Altitude = 0;
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
    StepperNumbers[0] = 0;      // Altimeter

    Alt_Hundreds_Pointer_Pos= cUnknownPos;
    CurrentDirection = cCounterClockwise;


    
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
          
          val = analogRead(0);      // read analog input pin 0  
          Serial.println(val);      // prints the value read
          if (val >= 200) {
            Serial.println("Zero exiting");
            AltimeterZeroed = true;
            delay(1000);
            // Break Out of loop
            x=5000;
          }
        }
        for (int  x = 0; x < 9 ; x++)
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
    delayMicroseconds(AltimeterDelay);

    //Step 2
  
    PORTA &= B00000001;
    PORTA |= B00000001;
    delayMicroseconds(AltimeterDelay);

    //Step 3
  
    PORTA &= B00000111;
    PORTA |= B00000111;
    delayMicroseconds(AltimeterDelay);

    //Step 4
  
    PORTA &= B00000110;
    PORTA |= B00000110;
    delayMicroseconds(AltimeterDelay);

    //Step 5
  
    PORTA &= B00001110;
    PORTA |= B00001110;
    delayMicroseconds(AltimeterDelay);

    //Step 6
  
    PORTA &= B00001000;
    PORTA |= B00001000;
    delayMicroseconds(AltimeterDelay);

    //Step 7
  
    PORTA &= B00001001;
    PORTA |= B00001001;
    delayMicroseconds(AltimeterDelay);

    if (Alt_Hundreds_Pointer_Pos != cUnknownPos)
    {
      Alt_Hundreds_Pointer_Pos = Alt_Hundreds_Pointer_Pos - 1;
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
    delayMicroseconds(AltimeterDelay);

    //Step 2
  
    PORTA &= B00001000;
    PORTA |= B00001000;
    delayMicroseconds(AltimeterDelay);

    //Step 3
  
    PORTA &= B00001110;
    PORTA |= B00001110;
    delayMicroseconds(AltimeterDelay);

    //Step 4
  
    PORTA &= B00000110;
    PORTA |= B00000110;
    delayMicroseconds(AltimeterDelay);

    //Step 5
  
    PORTA &= B00000111;
    PORTA |= B00000111;
    delayMicroseconds(AltimeterDelay);

    //Step 6
  
    PORTA &= B00000001;
    PORTA |= B00000001;
    delayMicroseconds(AltimeterDelay);

    //Step 7
  
    PORTA &= B00001001;
    PORTA |= B00001001;
    delayMicroseconds(AltimeterDelay);

    if (Alt_Hundreds_Pointer_Pos != cUnknownPos)
    {
      Alt_Hundreds_Pointer_Pos = Alt_Hundreds_Pointer_Pos + 1;
    };
   
    CurrentDirection = cClockwise; 
}




void loop()
{
  
  // Grab UDP Packet
  int packetSize = udp.parsePacket();

  // Anthing in the packet?
  if( packetSize > 0) {
    
      //Serial.println("Packet Received");
      udp.read( packetBuffer, packetSize);
      //terminate the buffer manually
      packetBuffer[packetSize] = '\0';
      
      // Will need to take a look at this - for other projects
      //  and data and command pieces, here really just receiving data
      //  should have altitude and perhaps pressure setting

      
      if (packetBuffer[0] == 'S') {    
        // We have some data
        Serial.println(packetBuffer);
        // Split the string up
        char* endPos;
        int i = 0;
        char *p = packetBuffer + 1;
        char *str;
        while ((str = strtok_r(p, ":", &p)) != NULL){
          
          // delimiter is the semicolon
          // Uncomment for troubleshooting
          //Serial.print( i);
          //Serial.print("-");
          //Serial.println(str);
          StepperNumbers[i] = strtod(str, &endPos);
          i++;
          
        }
      }     // Have something in the UDP Packet thats has correct header (eg 'S')
    }       // UDP Packet has a payload


    // Do a bounds check on Altitude
    if (Altitude >= 99999) Altitude = 99999;
    if (Altitude <= 0) Altitude = 0;

    int inputPotPosition = analogRead(sensorPin);
    long KollsmanAdjustPosition = analogRead(KollmansAdjustPin);
    // Adjust Kollamns Position to within range of 870 to 1085
    KollsmanAdjustPosition = 870 + int(KollsmanAdjustPosition * 215/1024);
    // Serial.println(inputPotPosition);
    // Sensor Value Ranges 0 to 1023


    // Throw packets out every 20 seconds for so
    // Currently used for testing but may also end up in production
    // So DCS code only sends data to a single device
    if( (millis() - lastAltimeterOutputPacket) > 50) {
      
      
      
      lastAltimeterOutputPacket = millis();

      // Uncomment for troubleshooting
      //Serial.println("Time to output a packet");
      //Serial.print("Zero Sensor ");
      //Serial.println(val);
      //Serial.print("Potentiometer Sensor: ");
      //Serial.println(inputPotPosition);
      //Serial.print("Altitude: ");
      //Serial.println(Altitude);
      //Serial.print("Kollsman Adjust ");
      //Serial.println(KollsmanAdjustPosition);
      
      // take Altitude (Long Unsigned Int), make it a char string 
      // and thow into a UDP Packet
      sprintf((char*)outpacketBuffer,"%lu,%i",Altitude,KollsmanAdjustPosition);
      udp.beginPacket(targetIP, remoteport);
      udp.write(outpacketBuffer);
      udp.endPacket();
    
    }
   

    // Test code to adjust Altitude
    // Based on pot position we wait a period of time
    // before changing desired Altitude
    if ( millis() >= NextIncrementDecrementTime) {

      // If the pot is mid position do nothing
      if (!(( inputPotPosition > 400) && (inputPotPosition < 600))) {
        if (inputPotPosition > 600) { 
          goingup = true;
          NextIncrementDecrementTime = millis() + (1023 - inputPotPosition); }
        else {
          goingup = false;   
          NextIncrementDecrementTime = millis() + (inputPotPosition);
        }
 
        if (goingup) {
          if (Altitude >= 99999) 
            Altitude = 99999; 
          else
            Altitude++; }
        else {
          if (Altitude <= 0) 
            Altitude = 0;
          else
            Altitude--;}
          

        
      }  // Pot isn't in mid position
    }
  


    val = analogRead(0);       // read analog input pin 0  
    // Serial.println(val); // prints the value read


   // Load the desired set point
   // As we are using a clock mechanism there are 60 steps
   // so round to the nearest degree (ie 60/100)
   StepperNumbers[0] = int(((Altitude/10)%100) * 0.6); 


   // Process Altimeter
   // If the altimeter is zerod don't bother doing anything
   if (AltimeterZeroed) {

      
      if (Alt_Hundreds_Pointer_Pos >= 60) Alt_Hundreds_Pointer_Pos = 0;
      if (Alt_Hundreds_Pointer_Pos <= -1)  Alt_Hundreds_Pointer_Pos = 59;

      LastAlt_Hundreds_Pointer_Pos = Alt_Hundreds_Pointer_Pos;

        
      if (Alt_Hundreds_Pointer_Pos != StepperNumbers[0]) {
        // Not at desired desired compass position so move it
        // if we are climbing don't wind altimeter back to try and catch

        if (LastAltitude > Altitude) {
            //Serial.println("Descending");
            StepCounterClockwise(); }
       else if (LastAltitude < Altitude) {
            //Serial.println("Ascending");
            StepClockwise(); }      
      }
   } // Altimeter Zeroed
  
  
   LastAltitude = Altitude;
 

}

