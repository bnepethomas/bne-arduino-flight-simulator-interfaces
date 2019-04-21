/* 
Heavily based on 
https://github.com/calltherain/ArduinoUSBJoystick

Instead of sending to USB - sends over UDP

Mega2560 R3, 
digitalPin 22~ 37 used as row0 ~ row 15, 
digital pin 38~53 used as column 0 ~ 15,
When an Ethernet shield is connected Columns 13 and 14 are pulled up to 1.5V - so do not use

it's a 16 * 14  matrix (due to loss of two columns) 

If using 5100 series Ethernet Shield may need to remove C3 as per 
https://forum.arduino.cc/index.php?topic=99880.15

*/

#define NUM_BUTTONS 256
#define NUM_AXES  8        // 8 axes, X, Y, Z, etc
#define STATUS_LED_PORT 7
#define FLASH_TIME 200

// 
struct joyReport_t {
   int button[NUM_BUTTONS]; // 1 Button per byte - was originally one bit per byte - but we have plenty of storage space
};

const  int ScanDelay = 40;

joyReport_t joyReport;
joyReport_t prevjoyReport;

long prevLEDTransition = millis();
int cButtonID[16];
bool bFirstTime = false;


#include <SPI.h>
#include <Ethernet.h>
#include <EthernetUdp.h>


// Unique Setting area for each input controller

// General Platform Front Input Controller
//byte mac[] = {0xA9,0xE7,0x3E,0xCA,0x35,0x02};
//IPAddress ip(172,16,1,11);
//const String deviceID = "01";

// General Platform Right Input Controller
byte mac[] = {0xA9,0xE7,0x3E,0xCA,0x35,0x04};
IPAddress ip(172,16,1,12);
const String deviceID = "02";


// Raspberry Pi is Target
IPAddress targetIP(172,16,1,2);
const unsigned int localport = 7788;
const unsigned int remoteport = 26027;
const unsigned int reflectorport = 27000;


EthernetUDP udp;
char packetBuffer[1000];     //buffer to store the incoming data
char outpacketBuffer[1000];  //buffer to store the outgoing data

char outData[100];
char stringind[5];
String outString;

//unsigned long loopTime;

void setup() 
{

  Serial.begin(115200);
  Serial.println("UDP Input Connector Setup");


  Serial.println("Initialising I/O Pins");
  // Output Port for Status Monitoring
  pinMode(STATUS_LED_PORT, OUTPUT);
  digitalWrite(STATUS_LED_PORT, LOW);
  
  
  for( int portId = 22; portId < 38; portId ++ )
  {
    pinMode( portId, OUTPUT );
  }
  for( int portId = 38; portId < 54; portId ++ )
  {
    // Even though we've already got 10K external pullups
    pinMode( portId, INPUT_PULLUP);
  }



  for (int ind=0; ind < NUM_BUTTONS ; ind++) {
    joyReport.button[ind] = 0;
    prevjoyReport.button[ind] = 0;
  }
  

  Serial.println("Starting IP Stack");
  Ethernet.begin( mac, ip);
  udp.begin( localport );


  udp.beginPacket(targetIP, reflectorport);
  udp.println("Init UDP");
  udp.endPacket();

  Serial.println("System Initialisation Complete");
}




void FindInputChanges()
{

  for (int ind=0; ind < NUM_BUTTONS; ind++)
    if (bFirstTime) {

      bFirstTime = false;
      // Just Copy Array and perform no actions - this may change in the future
      prevjoyReport.button[ind] = joyReport.button[ind];    
    }
    else {
      // Not the first time - see if there is a difference from last time
      // If there is perform action and update prev array 
      if ( prevjoyReport.button[ind] != joyReport.button[ind] ){

//        Serial.print("Input Change. Input ");
//        Serial.print(ind);
//        Serial.print(" changed to ");

        
        sprintf(stringind, "%03d", ind);
 


//        Serial.println(stringind);

        outString = "D";
        outString = outString + deviceID + ":" + String(stringind) + ":"; 

//        Serial.println(outString);
        
        if (prevjoyReport.button[ind] == 0) {
//          Serial.println("0");
          outString = outString +  "0"; 

        }  
        else {
//          Serial.println("1");      
          outString = outString + "1"; 
        }

//
        //outData = "D01:100:1";
        udp.beginPacket(targetIP, reflectorport);
        udp.print(outString);
        udp.endPacket();
        
        
        udp.beginPacket(targetIP, remoteport);
        udp.print(outString);
        udp.endPacket();
        
        prevjoyReport.button[ind] = joyReport.button[ind]; 
      }
      
    }
}


void loop() 
{


//  Serial.println("Main Loop"); 
//  for (unsigned int ind=0; ind < NUM_BUTTONS ; ind++) {
//    joyReport.button[ind] = 0;
//  }

  
  //turn off all rows first
  for ( int rowid = 0; rowid < 16; rowid ++ )
  {
    //turn on the current row
    // why differentiate? rows


    if (rowid == 0) 
      PORTC =  0xFF;
    if (rowid == 8)
      PORTA = 0xFF;
      
    if (rowid < 8)
    {
      // Shift 1 right  - this is actually pulling port down
      PORTA = ~(0x1 << rowid);
    }
    else
    {
      PORTC = ~(0x1 << (15 - rowid) );
    }


    
    //we must have such a delay so the digital pin output can go LOW steadily, 
    //without this delay, the row PIN will not 100% at LOW during yet,
    //so check the first column pin's value will return incorrect result.
    delayMicroseconds(ScanDelay);

    int colResult[16];
    // Reading upper pins
    //pin 38, PD7
    colResult[0] = (PIND & B10000000)== 0 ? 1 : 0;
    //pin 39, PG2
    colResult[1] = (PING & B00000100)== 0 ? 1 : 0;
    //pin 40, PG1
    colResult[2] = (PING & B00000010)== 0 ? 1 : 0;
    //pin 41, PG0
    colResult[3] = (PING & B00000001)== 0 ? 1 : 0;

    //pin 42, PL7
    colResult[4] = (PINL & B10000000)== 0 ? 1 : 0;
    //pin 43, PL6
    colResult[5] = (PINL & B01000000)== 0 ? 1 : 0;
    //pin 44, PL5
    colResult[6] = (PINL & B00100000)== 0 ? 1 : 0;
    //pin 45, PL4
    colResult[7] = (PINL & B00010000)== 0 ? 1 : 0;

    //pin 46, PL3
    colResult[8] = (PINL & B00001000)== 0 ? 1 : 0;
    //pin 47, PL2
    colResult[9] = (PINL & B00000100)== 0 ? 1 : 0;
    //pin 48, PL1
    colResult[10] =(PINL & B00000010) == 0 ? 1 : 0;
    //pin 49, PL0
    colResult[11] =(PINL & B00000001) == 0 ? 1 : 0;

    //pin 50, PB3
    colResult[12] =(PINB & B00001000) == 0 ? 1 : 0;
    //pin 51, PB2
    
    
    //colResult[13] =(PINB & B00000100) == 0 ? 1 : 0;
    colResult[13] = 0;
    //pin 52, PB1
    //colResult[14] =(PINB & B00000010) == 0 ? 1 : 0;
    colResult[14] = 0;
    
    //pin 53, PB0
    colResult[15] =(PINB & B00000001) == 0 ? 1 : 0;

    for ( int colid = 0; colid < 16; colid ++ )
    {
      if ( colResult[ colid ] == 1 )
      {
        joyReport.button[ (rowid * 16) + colid ] =  1;
      }
      else
      {
        joyReport.button[ (rowid * 16) + colid ] =  0;
      }
    }
  }


  




//  for ( int buttonid = 0; buttonid < NUM_BUTTONS; buttonid ++ )
//  {

//    if ((buttonid % 16) == 0)
//      Serial.println();
      
    //sprintf(cButtonID, "%3d", buttonid);    
//    Serial.print(buttonid);
//    Serial.print("-");
//    Serial.print(joyReport.button[buttonid]);
//    
//    Serial.print(" ");
    
//    if (joyReport.button[buttonid] != 0)
//    {
//      Serial.print(buttonid);
//      Serial.print("-");
//      Serial.print("1 ");
//    }
//    else
//    {
//      Serial.print(cButtonID);
//      Serial.print("-");
//      Serial.print("0 "); 
//    }
//  }

//  Serial.println(""); 
//  delay(1);


  

  // Flash Led 
  if ( (millis() - prevLEDTransition) >=  FLASH_TIME)
    {
      digitalWrite(STATUS_LED_PORT, !digitalRead(STATUS_LED_PORT)); 
      prevLEDTransition = millis();

//      udp.beginPacket(targetIP, reflectorport);
//      udp.print("D01:100:0");
//      udp.endPacket();
//
//
//      udp.beginPacket(targetIP, remoteport);
//      udp.print("D01:100:0");
//      udp.endPacket();

    }

  FindInputChanges();
  //delay(10);
}
