
// Max 7219 Library 
#include "LedControl.h"

/*
 Now we need a LedControl to work with.
 ***** These pin numbers will probably not work with your hardware *****
 pin 12 is connected to the DataIn 
 pin 11 is connected to the CLK 
 pin 10 is connected to LOAD 
 We have only a single MAX72XX.

 LedControl lc=LedControl(12,11,10,1);
 */
LedControl lc=LedControl(4,6,5,2);                      // Landing and Flight Displays
LedControl lc_2=LedControl(9,8,7,1);                   // Individual Leds and LE Devices

/* we always wait a bit between updates of the display */
unsigned long delaytime=250;

unsigned long sdelaytime=20;

#define filename "General_Sim_7219"

// 20161231a Stopping processing of OLED updates to isolate freeze

#include <SPI.h>
#include <Ethernet.h>
#include <EthernetUdp.h>

byte mac[] = { 
  0xA9,0xE7,0x3E,0xCA,0x34,0x1f};
IPAddress ip(192,168,2,205);
IPAddress TargetIPAddress(192,168,2,111);
const unsigned int localport = 13135;

EthernetUDP Udp;
char packetBuffer[UDP_TX_PACKET_MAX_SIZE]; //buffer to store the incoming data


const unsigned int listenport = 9920;
EthernetUDP rxUdp;
char receivePacketBuffer[UDP_TX_PACKET_MAX_SIZE]; //buffer to store the incoming data
char *ParameterNamePtr;
char *ParameterValuePtr;



// Module connection pins (Digital Pins)
#define CLK 2
#define DIO 3



// The amount of time (in milliseconds) between tests
#define TEST_DELAY   1


#define backLightPin 7
#define spareRelayPin 11

boolean APU_Starting = 0;
boolean APU_Stopping = 0;
boolean AC_Power = false;
boolean DC_Power = false;

boolean Debug_Display = false;

#define Flight_Altitude_Max7219 0
#define Landing_Altitude_Max7219 1

int LAND_ALT_STORE = 0;
int FLT_ALT_STORE = 0;

#include <Wire.h>          // *** I2C Mode 
//  Board  I2C / TWI pins
//  Uno, Ethernet A4 (SDA), A5 (SCL)
//  Mega2560  20 (SDA), 21 (SCL)
//  Leonardo  2 (SDA), 3 (SCL)
//  Due 20 (SDA), 21 (SCL), SDA1, SCL1

#define cmd_CLS 0x01

#define STX 0x02
#define ETX 0x03

char byteIn = 0;


int loopcounter = 0;

// Servo related timers

long lcurrentmillis = 0;


// Keepalive reporting
long lKeepAlive = 0;
long ltime = 0;

String s_sendstringwrkstr = "";

void setup() {
  // put your setup code here, to run once:
    /*
     The MAX72XX is in power-saving mode on startup,
      we have to do a wakeup call
    */
    lc.shutdown(0,false);
    /* Set the brightness to a medium values */
    lc.setIntensity(0,8);
    /* and clear the display */
    lc.clearDisplay(0);
    
    lc.shutdown(1,false);
    /* Set the brightness to a medium values */
    lc.setIntensity(1,8);
    /* and clear the display */
    lc.clearDisplay(1);
    
    lc.shutdown(2,false);
    /* Set the brightness to a medium values */
    lc.setIntensity(2,8);
    /* and clear the display */
    lc.clearDisplay(2);
    
    lc.shutdown(3,false);
    /* Set the brightness to a medium values */
    lc.setIntensity(3,8);
    /* and clear the display */
    lc.clearDisplay(3);

    lc_2.shutdown(0,false);
    /* Set the brightness to a medium values */
    lc_2.setIntensity(0,8);
    /* and clear the display */
    lc_2.clearDisplay(0);


    
    Serial.begin(115200); 
    Serial.println(filename);
    
  


    // Sending Infrastructure
    Serial.println("Starting Network");
    Ethernet.begin( mac, ip);

  
    //Receiving Infrastructure

    rxUdp.begin( listenport );
    Serial.println("Network Initialised");

    Serial.println("Start LED Display");
    single();
    Serial.println("Display Initialised");



    //Clear the UDP Buffer
    for(int i=0;i<UDP_TX_PACKET_MAX_SIZE;i++) receivePacketBuffer[i] = 0;


  lKeepAlive = 0;
  
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






void PrintMapping (int ledPos)
{
  if (ledPos == 0) Serial.println( "I_OH_LEDEVICES_TRANS_FLAP3");
  if (ledPos == 1) Serial.println( "I_OH_LEDEVICES_TRANS_FLAP4");
  if (ledPos == 2) Serial.println( "I_OH_LEDEVICES_TRANS_SLAT5");
  if (ledPos == 3) Serial.println( "I_OH_LEDEVICES_TRANS_SLAT6");
  if (ledPos == 4) Serial.println( "I_OH_LEDEVICES_TRANS_SLAT7");
  if (ledPos == 5) Serial.println( "I_OH_LEDEVICES_TRANS_SLAT8");
  if (ledPos == 6) Serial.println( "I_OH_IRS_DCFAIL_L");
  if (ledPos == 7) Serial.println( "I_OH_REVERSER1");
  if (ledPos == 8) Serial.println( "I_OH_LEDEVICES_EXT_FLAP3");
  if (ledPos == 9) Serial.println( "I_OH_LEDEVICES_EXT_FLAP4");
  if (ledPos == 10) Serial.println( "I_OH_LEDEVICES_EXT_SLAT5");
  if (ledPos == 11) Serial.println( "I_OH_LEDEVICES_EXT_SLAT6");
  if (ledPos == 12) Serial.println( "I_OH_LEDEVICES_EXT_SLAT7");
  if (ledPos == 13) Serial.println( "I_OH_LEDEVICES_EXT_SLAT8");
  if (ledPos == 14) Serial.println( "I_OH_IRS_FAULT_L");
  if (ledPos == 15) Serial.println( "I_OH_GEAR_NOSE_DOWN");
  if (ledPos == 16) Serial.println( "I_OH_LEDEVICES_EXT_FLAP3");
  if (ledPos == 17) Serial.println( "I_OH_LEDEVICES_EXT_FLAP4");
  if (ledPos == 18) Serial.println( "I_OH_LEDEVICES_FULLEXT_SLAT5");
  if (ledPos == 19) Serial.println( "I_OH_LEDEVICES_FULLEXT_SLAT6");
  if (ledPos == 20) Serial.println( "I_OH_LEDEVICES_FULLEXT_SLAT7");
  if (ledPos == 21) Serial.println( "I_OH_LEDEVICES_FULLEXT_SLAT8");
  if (ledPos == 22) Serial.println( "I_OH_IRS_ONDC_R");
  if (ledPos == 23) Serial.println( "I_OH_ENGINE_CONTROL2");
  if (ledPos == 24) Serial.println( "I_OH_LEDEVICES_TRANS_FLAP2");
  if (ledPos == 25) Serial.println( "I_OH_LEDEVICES_TRANS_FLAP1");
  if (ledPos == 26) Serial.println( "I_OH_LEDEVICES_TRANS_SLAT4");
  if (ledPos == 27) Serial.println( "I_OH_LEDEVICES_TRANS_SLAT3");
  if (ledPos == 28) Serial.println( "I_OH_LEDEVICES_TRANS_SLAT2");
  if (ledPos == 29) Serial.println( "I_OH_LEDEVICES_TRANS_SLAT1");
  if (ledPos == 30) Serial.println( "I_OH_IRS_ALIGN_R");
  if (ledPos == 31) Serial.println( "I_OH_ENGINE_CONTROL1");
  if (ledPos == 32) Serial.println( "I_OH_LEDEVICES_EXT_FLAP2");
  if (ledPos == 33) Serial.println( "I_OH_LEDEVICES_EXT_FLAP1");
  if (ledPos == 34) Serial.println( "I_OH_LEDEVICES_EXT_SLAT4");
  if (ledPos == 35) Serial.println( "I_OH_LEDEVICES_EXT_SLAT3");
  if (ledPos == 36) Serial.println( "I_OH_LEDEVICES_EXT_SLAT2");
  if (ledPos == 37) Serial.println( "I_OH_LEDEVICES_EXT_SLAT1");
  if (ledPos == 38) Serial.println( "I_OH_IRS_ALIGN_L");
  if (ledPos == 39) Serial.println( "I_OH_GEAR_LEFT_DOWN");
  if (ledPos == 40) Serial.println( "I_OH_LEDEVICES_EXT_FLAP2");
  if (ledPos == 41) Serial.println( "I_OH_LEDEVICES_EXT_FLAP1");
  if (ledPos == 42) Serial.println( "I_OH_LEDEVICES_FULLEXT_SLAT4");
  if (ledPos == 43) Serial.println( "I_OH_LEDEVICES_FULLEXT_SLAT3");
  if (ledPos == 44) Serial.println( "I_OH_LEDEVICES_FULLEXT_SLAT2");
  if (ledPos == 45) Serial.println( "I_OH_LEDEVICES_FULLEXT_SLAT1");
  if (ledPos == 46) Serial.println( "I_OH_IRS_DCFAIL_R");
  if (ledPos == 47) Serial.println( "I_OH_REVERSER2");
  if (ledPos == 54) Serial.println( "I_OH_IRS_FAULT_R");
  if (ledPos == 55) Serial.println( "I_OH_GEAR_RIGHT_DOWN");
  if (ledPos == 61) Serial.println( "I_OH_GPS");
  if (ledPos == 62) Serial.println( "I_OH_IRS_ONDC_L");
  if (ledPos == 63) Serial.println( "I_OH_PSEU");
}





void ProcessReceivedString()
{

    // Reading values from packetBuffer which is global
    // All received values are strings for readability

    bool bLocalDebug = true;

    if (Debug_Display || bLocalDebug ) Serial.println("Processing Packet");
   

    String sWrkStr = "";

    const char *delim  = "="; 
    
    ParameterNamePtr = strtok(packetBuffer,delim);
    String ParameterNameString(ParameterNamePtr); 
    if (Debug_Display || bLocalDebug ) Serial.println("Parameter Name " + String(ParameterNameString));
       
    ParameterValuePtr   = strtok(NULL,delim);
    String ParameterValue(ParameterValuePtr);
    if (Debug_Display || bLocalDebug ) Serial.println("Parameter Value " + String(ParameterValuePtr));

    // Handle the following attribute types
    
    
    // Ixx - General indicator - a 0 or 1 
    if (ParameterNameString[0] == 'I')
    {
      // We have a Indicator, the indicator number is always two digits
      // So grab the 2nd and 3rd characters and convert them
      sWrkStr = String(ParameterNameString[1]) + String(ParameterNameString[2]);

      //Work from here need to cleanup string hand;

      // Check to see if parameter is between 0 and 64 if valid proceed
      if (isValidNumber(sWrkStr))
      {
        if (Debug_Display || bLocalDebug ) Serial.println(sWrkStr + " is a valid number");
        
        if (Debug_Display || bLocalDebug ) Serial.println("Buffer Length " + String(sWrkStr.length()));
        
        char buf[sWrkStr.length()+1];
        sWrkStr.toCharArray(buf,sWrkStr.length()+1);

        int iledToChange= atoi(buf); 
        if (Debug_Display || bLocalDebug ) Serial.println("Converted number is " + String(iledToChange));

        if (Debug_Display || bLocalDebug ) Serial.println("Converting to Row Column");

        int iledRow = 0;
        int iledColumn = 0;

        iledRow = iledToChange / 8;
        iledColumn = iledToChange % 8;

        if (Debug_Display || bLocalDebug ) Serial.println("Row:" + String(iledRow) + " Column:" +  String(iledColumn));


        if (String(ParameterValuePtr)=="0")
        {
          if (Debug_Display || bLocalDebug ) Serial.println("Clearing - Row:" + String(iledRow) + " Column:" +  String(iledColumn));
          lc_2.setLed(0,iledRow,iledColumn,false);
          
        }
        else
        {
          if (Debug_Display || bLocalDebug ) Serial.println("Lighting - Row:" + String(iledRow) + " Column:" +  String(iledColumn));
          lc_2.setLed(0,iledRow,iledColumn,true);
        }

        // Our work is done      
        return;
        
      }
      else
      {
        if (Debug_Display || bLocalDebug ) Serial.println(sWrkStr + " is not a valid number");
        return;

      } 
          
    }
   
     
  
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
      //Serial.print("I am alive: ");
      //Serial.println(ltime);
      lKeepAlive = millis();
    
  }

  
  int packetSize = rxUdp.parsePacket();
  
  if( packetSize > 0)
  {
      rxUdp.read( packetBuffer, packetSize);
      //terminate the buffer manually
      packetBuffer[packetSize] = '\0';
  
      ProcessReceivedString();  
  }

}
