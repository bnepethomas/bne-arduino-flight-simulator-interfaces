// Todos
//

//  Document Protocol
//    D,1:0,2:1,3:1,4:1,5:0,6:0,7:0,8:0,9:1,10:1,11:0
//    C,B:15
//  Handle more than 64 leds is second unit daisy chained - including brightness

// Max 7219 Library 
#include "LedControl.h"


// Pins 7,8,9 as the ones used for Hornet, so will standardise on that
LedControl lc_2=LedControl(9,8,7,1);  

/* we always wait a bit between updates of the display */
unsigned long delaytime=250;
unsigned long sdelaytime=1000;

#define filename "General_Sim_7219"


#include <SPI.h>
#include <Ethernet.h>
#include <EthernetUdp.h>

#include <Servo.h>


byte mac[] = { 
  0x00,0xC7,0x3E,0xCA,0x35,0x03};
IPAddress ip(172,16,1,21);
const unsigned int localport = 13135;

EthernetUDP Udp;


// Do not use UDP_TX_PACKET_MAX_SIZE for character buffer as it is only small
// https://stackoverflow.com/questions/54232090/unintentional-strange-characters-added-to-packets-during-udp-communication-in-ar

char packetBuffer[1500]; //buffer to store the incoming data


const unsigned int listenport = 13135;
EthernetUDP rxUdp;
char receivePacketBuffer[1500]; //buffer to store the incoming data
char *ParameterNamePtr;
char *ParameterValuePtr;



// Module connection pins (Digital Pins)
#define CLK 2
#define DIO 3



boolean Debug_Display = false;
char byteIn = 0;
int loopcounter = 0;

// Keepalive reporting
long lKeepAlive = 0;
long ltime = 0;
long llastServoMillis = 0;
long llastDisplayUpdateMillis = 0;
int  iServoDirection = 1;
int  iServoPos = 0;
const int NoOfServos = 18;
const int ServoMinValue = 0;
const int ServoMaxValue = 180;
const int ServoBasePort = 21;

const int ServoStartUDPPort = 150;
const int OutputStartUDPPort = 200;

// Deal with guages such as VSI
int iServoStartPos[NoOfServos + 1];

// Safety limits such as needles  on VOR
int iServoMinPos[NoOfServos + 1];
int iServoMaxPos[NoOfServos + 1];

int  iServoDesiredPos[NoOfServos + 1];
int  iServoCurrentPos[NoOfServos + 1];

const int NoOfOutputs = 9;
const int BaseOutputPort = 39;
int  iDesiredOutput[NoOfOutputs + 1];

Servo myservo_1;
Servo myservo_2;
Servo myservo_3;
Servo myservo_4;
Servo myservo_5;
Servo myservo_6;
Servo myservo_7;
Servo myservo_8;
Servo myservo_9;
Servo myservo_10;
Servo myservo_11;
Servo myservo_12;
Servo myservo_13;
Servo myservo_14;
Servo myservo_15;
Servo myservo_16;
Servo myservo_17;
Servo myservo_18;



String s_sendstringwrkstr = "";

void setup() {

   int iServoSetupPtr = 0;
   
   Serial.begin(115200); 
   Serial.println(filename);
    /*
     The MAX72XX is in power-saving mode on startup,
      we have to do a wakeup call
    */

    lc_2.shutdown(0,false);
    /* Set the brightness to a medium values */
    lc_2.setIntensity(0,15);
    /* and clear the display */
    lc_2.clearDisplay(0);


    // Zero Servo related Data

    for (int iServoPtr = 1; iServoPtr <= NoOfServos; iServoPtr += 1) {
      iServoDesiredPos[iServoPtr] = 0;
      iServoCurrentPos[iServoPtr] = 0;

      // The values in this block should be explicity set later - but just in case
      // they are missed - set a sane value
      iServoStartPos[iServoPtr] = 90;
      iServoMinPos[iServoPtr]  = 0;
      iServoMaxPos[iServoPtr] = 180;
    }  

    // Zero Outputs
   for (int iOutputPtr = 1; iOutputPtr <= NoOfOutputs; iOutputPtr += 1) {
      iDesiredOutput[iOutputPtr] = 0;

    }  



    // Sending Infrastructure
    Serial.println("Starting Network ");
    Ethernet.begin( mac, ip);

  
    //Receiving Infrastructure

    rxUdp.begin( listenport );
    Serial.println("Network Initialised");

    Serial.println("Start LED Display");
    AllOn();
    delay(sdelaytime);
    AllOff();
    Serial.println("Display Initialised");



    //Clear the UDP Buffer
    for(int i=0;i<UDP_TX_PACKET_MAX_SIZE;i++) receivePacketBuffer[i] = 0;


    lKeepAlive = 0;

    // Initialise pins 40 to 52 as output port
    for (int portNo = 40; portNo <= 48; portNo += 1) { 
      pinMode(portNo, OUTPUT);
    }

    // Gauge Description here
    iServoSetupPtr = 1;
    iServoStartPos[iServoSetupPtr] = 90;
    iServoMinPos[iServoSetupPtr]  = 0;
    iServoMaxPos[iServoSetupPtr] = 180;
    
    // Gauge Description here
    iServoSetupPtr = 2;
    iServoStartPos[iServoSetupPtr] = 90;
    iServoMinPos[iServoSetupPtr]  = 0;
    iServoMaxPos[iServoSetupPtr] = 180;

    // Gauge Description here
    iServoSetupPtr = 3;
    iServoStartPos[iServoSetupPtr] = 90;
    iServoMinPos[iServoSetupPtr]  = 0;
    iServoMaxPos[iServoSetupPtr] = 180;

    // Gauge Description here
    iServoSetupPtr = 4;
    iServoStartPos[iServoSetupPtr] = 90;
    iServoMinPos[iServoSetupPtr]  = 0;
    iServoMaxPos[iServoSetupPtr] = 180;

    // Gauge Description here
    iServoSetupPtr = 5;
    iServoStartPos[iServoSetupPtr] = 90;
    iServoMinPos[iServoSetupPtr]  = 0;
    iServoMaxPos[iServoSetupPtr] = 180;

    // Gauge Description here
    iServoSetupPtr = 6;
    iServoStartPos[iServoSetupPtr] = 90;
    iServoMinPos[iServoSetupPtr]  = 0;
    iServoMaxPos[iServoSetupPtr] = 180;

    // Gauge Description here
    iServoSetupPtr = 7;
    iServoStartPos[iServoSetupPtr] = 90;
    iServoMinPos[iServoSetupPtr]  = 0;
    iServoMaxPos[iServoSetupPtr] = 180;

    // Gauge Description here
    iServoSetupPtr = 8;
    iServoStartPos[iServoSetupPtr] = 90;
    iServoMinPos[iServoSetupPtr]  = 0;
    iServoMaxPos[iServoSetupPtr] = 180;

    // Gauge Description here
    iServoSetupPtr = 9;
    iServoStartPos[iServoSetupPtr] = 90;
    iServoMinPos[iServoSetupPtr]  = 0;
    iServoMaxPos[iServoSetupPtr] = 180;

     // Gauge Description here
    iServoSetupPtr = 10;
    iServoStartPos[iServoSetupPtr] = 90;
    iServoMinPos[iServoSetupPtr]  = 0;
    iServoMaxPos[iServoSetupPtr] = 180;

    // Gauge Description here
    iServoSetupPtr = 11;
    iServoStartPos[iServoSetupPtr] = 90;
    iServoMinPos[iServoSetupPtr]  = 0;
    iServoMaxPos[iServoSetupPtr] = 180;
    
    // Gauge Description here
    iServoSetupPtr = 12;
    iServoStartPos[iServoSetupPtr] = 90;
    iServoMinPos[iServoSetupPtr]  = 0;
    iServoMaxPos[iServoSetupPtr] = 180;

    // Gauge Description here
    iServoSetupPtr = 13;
    iServoStartPos[iServoSetupPtr] = 90;
    iServoMinPos[iServoSetupPtr]  = 0;
    iServoMaxPos[iServoSetupPtr] = 180;

    // Gauge Description here
    iServoSetupPtr = 14;
    iServoStartPos[iServoSetupPtr] = 90;
    iServoMinPos[iServoSetupPtr]  = 0;
    iServoMaxPos[iServoSetupPtr] = 180;

    // Gauge Description here
    iServoSetupPtr = 15;
    iServoStartPos[iServoSetupPtr] = 90;
    iServoMinPos[iServoSetupPtr]  = 0;
    iServoMaxPos[iServoSetupPtr] = 180;

    // Gauge Description here
    iServoSetupPtr = 16;
    iServoStartPos[iServoSetupPtr] = 90;
    iServoMinPos[iServoSetupPtr]  = 0;
    iServoMaxPos[iServoSetupPtr] = 180;

    // Gauge Description here
    iServoSetupPtr = 17;
    iServoStartPos[iServoSetupPtr] = 90;
    iServoMinPos[iServoSetupPtr]  = 0;
    iServoMaxPos[iServoSetupPtr] = 180;

    // Gauge Description here
    iServoSetupPtr = 18;
    iServoStartPos[iServoSetupPtr] = 90;
    iServoMinPos[iServoSetupPtr]  = 0;
    iServoMaxPos[iServoSetupPtr] = 180;
       
    // Immediately set start pos of enable servo to prevent servo bending a needle    
    myservo_1.attach(22);
    myservo_1.write(iServoStartPos[1]);
    myservo_2.attach(23);
    myservo_2.write(iServoStartPos[2]);
    myservo_3.attach(24);
    myservo_3.write(iServoStartPos[3]);
    myservo_4.attach(25);
    myservo_4.write(iServoStartPos[4]);
    myservo_5.attach(26);
    myservo_5.write(iServoStartPos[5]);
    myservo_6.attach(27);
    myservo_6.write(iServoStartPos[6]);
    myservo_7.attach(28);
    myservo_7.write(iServoStartPos[7]);
    myservo_8.attach(29);
    myservo_8.write(iServoStartPos[8]);
    myservo_9.attach(30);
    myservo_9.write(iServoStartPos[9]);
    myservo_10.attach(31);
    myservo_10.write(iServoStartPos[10]);
    myservo_11.attach(32);
    myservo_11.write(iServoStartPos[11]);
    myservo_12.attach(33);
    myservo_12.write(iServoStartPos[12]);
    myservo_13.attach(34);
    myservo_13.write(iServoStartPos[13]);
    myservo_14.attach(35);
    myservo_14.write(iServoStartPos[14]);
    myservo_15.attach(36);
    myservo_15.write(iServoStartPos[15]);
    myservo_16.attach(37);
    myservo_16.write(iServoStartPos[16]);
    myservo_17.attach(38);
    myservo_17.write(iServoStartPos[17]);
    myservo_18.attach(39);
    myservo_18.write(iServoStartPos[18]);
}






/*
  This function lights up a some Leds in a row.
 The pattern will be repeated on every row.
 The pattern will blink along with the row-number.
 row number 4 (index==3) will blink 4 times etc.
 */
void rows() {
  for(int row=0;row<8;row++) {
    delay(sdelaytime);
    lc_2.setRow(0,row,B10100000);
    delay(sdelaytime);
    lc_2.setRow(0,row,(byte)0);
    for(int i=0;i<row;i++) {
      delay(sdelaytime);
      lc_2.setRow(0,row,B10100000);
      delay(sdelaytime);
      lc_2.setRow(0,row,(byte)0);
    }
  }
}

/*
  This function lights up a some Leds in a column.
 The pattern will be repeated on every column.
 The pattern will blink along with the column-number.
 column number 4 (index==3) will blink 4 times etc.
 */
void columns() {
  for(int col=0;col<8;col++) {
    delay(sdelaytime);
    lc_2.setColumn(0,col,B10100000);
    delay(sdelaytime);
    lc_2.setColumn(0,col,(byte)0);
    for(int i=0;i<col;i++) {
      delay(sdelaytime);
      lc_2.setColumn(0,col,B10100000);
      delay(sdelaytime);
      lc_2.setColumn(0,col,(byte)0);
    }
  }
}

/* 
 This function will light up every Led on the matrix.
 The led will blink along with the row-number.
 row number 4 (index==3) will blink 4 times etc.
 */
void single() {
  for(int row=0;row<8;row++) {
    for(int col=0;col<8;col++) {
      Serial.println("new Row Col " + String(row) + " " +  String(col) + " Position:" + String( row * 8 +  col));
      PrintMapping(row * 8 +  col);
      delay(sdelaytime);
      lc_2.setLed(0,row,col,true);
      delay(sdelaytime);
      for(int i=0;i<8;i++) {
        lc_2.setLed(0,row,col,false);
        delay(sdelaytime);
        lc_2.setLed(0,row,col,true);
        delay(sdelaytime);
      }
      lc_2.setLed(0,row,col,false);
    }
  }
}


void AllOn() {
  for(int row=0;row<8;row++) {
    for(int col=0;col<8;col++) {
      lc_2.setLed(0,row,col,true);
    }
  }
}

void AllOff() {
  for(int row=0;row<8;row++) {
    for(int col=0;col<8;col++) {
      lc_2.setLed(0,row,col,false);
    }
  }
}


void PrintMapping (int ledPos)
{
  bool bLocalDebug = false;
  if (Debug_Display || bLocalDebug ) 
  {
    if (ledPos == 0) Serial.println( "TBA");
    if (ledPos == 1) Serial.println( "TBA");
    if (ledPos == 2) Serial.println( "TBA");
    if (ledPos == 3) Serial.println( "TBA");
    if (ledPos == 4) Serial.println( "TBA");
    if (ledPos == 5) Serial.println( "TBA");
    if (ledPos == 6) Serial.println( "TBA");
    if (ledPos == 7) Serial.println( "TBA");
    if (ledPos == 8) Serial.println( "TBA");
    if (ledPos == 9) Serial.println( "TBA");
    if (ledPos == 10) Serial.println( "TBA");
    if (ledPos == 11) Serial.println( "TBA");
    if (ledPos == 12) Serial.println( "TBA");
    if (ledPos == 13) Serial.println( "TBA");
    if (ledPos == 14) Serial.println( "TBA");
    if (ledPos == 15) Serial.println( "TBA");
    if (ledPos == 16) Serial.println( "TBA");
    if (ledPos == 17) Serial.println( "TBA");
    if (ledPos == 18) Serial.println( "TBA");
    if (ledPos == 19) Serial.println( "TBA");
    if (ledPos == 20) Serial.println( "TBA");
    if (ledPos == 21) Serial.println( "TBA");
    if (ledPos == 22) Serial.println( "TBA");
    if (ledPos == 23) Serial.println( "TBA");
    if (ledPos == 24) Serial.println( "TBA");
    if (ledPos == 25) Serial.println( "TBA");
    if (ledPos == 26) Serial.println( "TBA");
    if (ledPos == 27) Serial.println( "TBA");
    if (ledPos == 28) Serial.println( "TBA");
    if (ledPos == 29) Serial.println( "TBA");
    if (ledPos == 30) Serial.println( "TBA");
    if (ledPos == 31) Serial.println( "TBA");
    if (ledPos == 32) Serial.println( "TBA");
    if (ledPos == 33) Serial.println( "TBA");
    if (ledPos == 34) Serial.println( "TBA");
    if (ledPos == 35) Serial.println( "TBA");
    if (ledPos == 36) Serial.println( "TBA");
    if (ledPos == 37) Serial.println( "TBA");
    if (ledPos == 38) Serial.println( "TBA");
    if (ledPos == 39) Serial.println( "TBA");
    if (ledPos == 40) Serial.println( "TBA");
    if (ledPos == 41) Serial.println( "TBA");
    if (ledPos == 42) Serial.println( "TBA");
    if (ledPos == 43) Serial.println( "TBA");
    if (ledPos == 44) Serial.println( "TBA");
    if (ledPos == 45) Serial.println( "TBA");
    if (ledPos == 46) Serial.println( "TBA");
    if (ledPos == 47) Serial.println( "TBA");
    if (ledPos == 54) Serial.println( "TBA");
    if (ledPos == 55) Serial.println( "TBA");
    if (ledPos == 61) Serial.println( "TBA");
    if (ledPos == 62) Serial.println( "TBA");
    if (ledPos == 63) Serial.println( "TBA");
  }
}








void ProcessReceivedString()
{

    // Reading values from packetBuffer which is global
    // All received values are strings for readability

    // Old versions handled a single attribute/value pair at a time eg I01=1
    // New Version Handles multiple attribute Values in a single packet
    //    D,1:0,2:1,3:1,4:1,5:0,6:0,7:0,8:0,9:1,10:1
    

    bool bLocalDebug = true;

    if (Debug_Display || bLocalDebug ) Serial.println("Processing Packet :" + String(millis()));
   
    bLocalDebug = false;
    
    String sWrkStr = "";

    // const char *delim  = "="; 
    const char *delim  = ",";
    
    // Break the received packet into a series of tokens
    // If there is no match the pointer will be null, other points to first parameter
    ParameterNamePtr = strtok(packetBuffer,delim);


    
    String ParameterNameString(ParameterNamePtr); 
    if (Debug_Display || bLocalDebug ) Serial.println("Parameter Name " + String(ParameterNameString));

    // Print all of the values received as a outer loop
    // and then split inner values   
    /* get the first token */

    /* walk through other tokens */

    String wrkstring = "";

    if (Debug_Display || bLocalDebug ) 
      if (ParameterNamePtr != NULL) Serial.println("First Value is: " + String(ParameterNamePtr));
    if (ParameterNameString[0] == 'D')
    {
      //Handling a Data Packet
      if (Debug_Display || bLocalDebug ) Serial.println("Handling a Data Packet");
      ParameterNamePtr = strtok(NULL, delim);

      while( ParameterNamePtr != NULL ) {
        if (Debug_Display || bLocalDebug ) Serial.println( "Processing " + String(ParameterNamePtr) );
        
        wrkstring = String(ParameterNamePtr);
        HandleOutputValuePair(wrkstring);

   
        ParameterNamePtr = strtok(NULL, delim);
      }  
          
      return;
      // End Handling a Data Packet
    }
    else if (ParameterNameString[0] == 'C')
    {
      // Handling a Control Packet
      if (Debug_Display || bLocalDebug ) Serial.println("Handling a Control Packet");
      
      ParameterNamePtr = strtok(NULL, delim);

      while( ParameterNamePtr != NULL ) {
        if (Debug_Display || bLocalDebug )Serial.println( "Processing " + String(ParameterNamePtr) );
        
        wrkstring = String(ParameterNamePtr);
        HandleControlString(wrkstring);
      
        ParameterNamePtr = strtok(NULL, delim);
      }  


      return;
      
      // Handling a Control Packet
    }
    else
    {
      // Unknown Packet Type
      if (Debug_Display || bLocalDebug ) Serial.println("Unknown Packet Type - ignoring packet");
      return;
    }
  
}


void HandleOutputValuePair( String str)
{
  bool bLocalDebug = false;
  if (Debug_Display || bLocalDebug ) Serial.println("Handling " + str);

  int delimeterlocation = 0;
  String lednumber = "";
  String ledvalue = "";


  

  delimeterlocation = str.indexOf(':');

  if (delimeterlocation == 0) 
  {
    if (Debug_Display || bLocalDebug ) Serial.println("**** WARNING no delimiter passed ****** Looking for :");
  }
  else
  {
    if (Debug_Display || bLocalDebug ) Serial.println("Delimiter is located a position " + String(delimeterlocation));
    lednumber = getValue(str, ':', 0);
    if (Debug_Display || bLocalDebug ) Serial.println("lednumber is :" + lednumber);
    ledvalue = getValue(str, ':', 1);
    if (Debug_Display || bLocalDebug ) Serial.println("ledvalue is :" + ledvalue);


    int llednumber = lednumber.toInt(); 
    int lledvalue = ledvalue.toInt(); 


    if (llednumber >= 0 && llednumber <= 128) {
      int iledRow = 0;
      int iledColumn = 0;
  
      iledRow = llednumber / 8;
      iledColumn = llednumber % 8;
  
      if (Debug_Display || bLocalDebug ) Serial.println("Row:" + String(iledRow) + " Column:" +  String(iledColumn));
  
  
      if (lledvalue==0)
      {
        if (Debug_Display || bLocalDebug ) Serial.println("Clearing - Row:" + String(iledRow) + " Column:" +  String(iledColumn));
        lc_2.setLed(0,iledRow,iledColumn,false);
        
      }
      else
      {
        if (Debug_Display || bLocalDebug ) Serial.println("Lighting - Row:" + String(iledRow) + " Column:" +  String(iledColumn));
        lc_2.setLed(0,iledRow,iledColumn,true);
      }
    }
    else if (llednumber >= ServoStartUDPPort && llednumber <= (ServoStartUDPPort + NoOfServos  -1)) 
    {
      // Handle Servos
 
      iServoDesiredPos[llednumber - (ServoStartUDPPort -1)] = lledvalue;

   
    }
    // Eight Output Pins for Relays
    else if (llednumber >= OutputStartUDPPort && llednumber <= (OutputStartUDPPort + NoOfOutputs - 1)) 
    {
      // Handle Digital Outputput - Pins 40 to 48
      Serial.println("Setting Digital Output. Port Number is :" + String(llednumber) + " Value is " + String(lledvalue));
      iDesiredOutput[llednumber - (OutputStartUDPPort - 1)] = lledvalue;
    }

  }
  return;
  
}


void HandleControlString( String str)
{
  bool bLocalDebug = false;
  if (Debug_Display || bLocalDebug ) Serial.println("Handling Control String :" + str);

  // Currnetly just handling Brightness - eg C,B:3

  int delimeterlocation = 0;
  String command = "";
  String setting = "";





  delimeterlocation = str.indexOf(':');

  if (delimeterlocation == 0)
  {
    if (Debug_Display || bLocalDebug ) Serial.println("**** WARNING no delimiter passed ****** Looking for :");
  }
  else
  {
    if (Debug_Display || bLocalDebug ) Serial.println("Delimiter is located a position " + String(delimeterlocation));
    command = getValue(str, ':', 0);
    if (Debug_Display || bLocalDebug ) Serial.println("command is :" + command);
    setting = getValue(str, ':', 1);
    if (Debug_Display || bLocalDebug ) Serial.println("setting is :" + setting);

    int isetting = setting.toInt(); 
    
    if (command == "B")
      if (isetting >= 0 && isetting <= 15)
        lc_2.setIntensity(0,isetting);
      else
        if (Debug_Display || bLocalDebug ) Serial.println("Invalid Brightness value passoed. Value is :" + String(setting));
  }
  
  return;
  
}

String getValue(String data, char separator, int index)
{
    int found = 0;
    int strIndex[] = { 0, -1 };
    int maxIndex = data.length() - 1;

    for (int i = 0; i <= maxIndex && found <= index; i++) {
        if (data.charAt(i) == separator || i == maxIndex) {
            found++;
            strIndex[0] = strIndex[1] + 1;
            strIndex[1] = (i == maxIndex) ? i+1 : i;
        }
    }
    return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}

boolean isValidNumber(String str)
{
   boolean isNum=false;
   if(!(str.charAt(0) == '+' || str.charAt(0) == '-' || isDigit(str.charAt(0)))) return false;

   for(byte i=1;i<str.length();i++)
   {
       if(!(isDigit(str.charAt(i)) || str.charAt(i) == '.')) return false;
   }
   return true;
}

void loop() {
  boolean bLocalDebug = false;
  // Check to see if anything has landed in UDP buffer


  ltime = millis();
  if ((ltime - lKeepAlive) > 1000)
  {

      lKeepAlive = millis();
    
  }

  
  int packetSize = rxUdp.parsePacket();
  
  if( packetSize > 0)
  {

      bLocalDebug = true;
      Serial.println("Processing Packet");
      bLocalDebug = false;
      rxUdp.read( packetBuffer, packetSize);
      //terminate the buffer manually
      packetBuffer[packetSize] = '\0';
  
      ProcessReceivedString();  
  }



  // Update Servo and outputs Position
  if (millis() - llastDisplayUpdateMillis >= 3000) {

    Serial.println("Updating Outputs");
    llastDisplayUpdateMillis = millis();

    
     for (int i= 1; i <= NoOfServos; i++) {
      Serial.println("Servo " + String(i) + " Port " + String((i + ServoBasePort)) + ". Current :" + String(iServoCurrentPos[i])
          + " - target :" + String(iServoDesiredPos[i]) );
      }
  
      for (int i= 1; i <= NoOfOutputs; i++) {
        Serial.println("Digital Outout " + String(i) + " Port " + String(i + BaseOutputPort) + " :" + iDesiredOutput[i]);
      }
  }  

  // Update Servo and outputs Position
  if (millis() - llastServoMillis >= 10) {

//    Serial.println("Updating Outputs");
//    llastServoMillis = millis();
//
//    for (int i= 1; i <= NoOfServos; i++) {
//      Serial.println("Servo " + String(i) + " Port " + String(i + ServoBasePort) + ". Current :" + String(iServoCurrentPos[i])
//        + "- target :" + String(iServoDesiredPos[i]) );
//    }
//
//    for (int i= 1; i <= NoOfOutputs; i++) {
//      Serial.println("Digital Outout " + String(i) + " Port " + String(i + BaseOutputPort) + " :" + iDesiredOutput[i]);
//    }

    
    
    // Set Digital Ports
    // Unable to use ports 49 to 52 when Ethernet Shield is attached.
    // 200 = Pin 40, 201 = 41, 208 = 48
    for (int outputportNo = 1; outputportNo <= NoOfOutputs; outputportNo += 1) { 
      if (iDesiredOutput[outputportNo] != 0)
        digitalWrite((BaseOutputPort + outputportNo) , LOW);
      else
        digitalWrite((BaseOutputPort + outputportNo) , HIGH);
    }
    
    

    
//    if (iServoPos >= 180) {
//      iServoDirection = -1; 
//    }   
//    else if (iServoPos < 0)
//    {
//      iServoDirection = 1; 
//    }
//    // Serial.println("Servo Pos :" + String(iServoPos));
//    iServoPos = iServoPos + iServoDirection;
//
//    for (int iServoPtr = 1; iServoPtr <= (NoOfServos -1); iServoPtr += 1) {
//      iServoDesiredPos[iServoPtr] = iServoPos;
//    }

   
   // Check Desired Pos is within limits of 0 to 180
   
   
   // Move needles 1 step per cycle to target   
   for (int iServoPtr = 1; iServoPtr <= (NoOfServos -1); iServoPtr += 1) {
    
      // Check Desired Pos is within limits of 0 to 180
      if (iServoDesiredPos[iServoPtr] > ServoMaxValue)
        iServoDesiredPos[iServoPtr] = ServoMaxValue;
      else if (iServoDesiredPos[iServoPtr] < ServoMinValue)
        iServoDesiredPos[iServoPtr] = ServoMinValue;

      if (iServoDesiredPos[iServoPtr] >= iServoCurrentPos[iServoPtr])
        iServoCurrentPos[iServoPtr] = iServoCurrentPos[iServoPtr] + 1;
      else if (iServoDesiredPos[iServoPtr] <= iServoCurrentPos[1])
        iServoCurrentPos[iServoPtr] = iServoCurrentPos[iServoPtr] - 1;   

    }

      
    myservo_1.write(iServoCurrentPos[1]);
    myservo_2.write(iServoCurrentPos[2]);
    myservo_3.write(iServoCurrentPos[3]);
    myservo_4.write(iServoCurrentPos[4]);
    myservo_5.write(iServoCurrentPos[5]);
    myservo_6.write(iServoCurrentPos[6]);
    myservo_7.write(iServoCurrentPos[7]);
    myservo_8.write(iServoCurrentPos[8]);
    myservo_9.write(iServoCurrentPos[9]);
    myservo_10.write(iServoCurrentPos[1]);
    myservo_11.write(iServoCurrentPos[11]);
    myservo_12.write(iServoCurrentPos[12]);
    myservo_13.write(iServoCurrentPos[13]);
    myservo_14.write(iServoCurrentPos[14]);
    myservo_15.write(iServoCurrentPos[15]);
    myservo_16.write(iServoCurrentPos[16]);
    myservo_17.write(iServoCurrentPos[17]);
    myservo_18.write(iServoCurrentPos[18]);

  }

  // End update Servo Position

  

}
