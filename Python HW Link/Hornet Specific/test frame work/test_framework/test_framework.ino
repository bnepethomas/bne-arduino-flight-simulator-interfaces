// Todos
//

//  Document Protocol
//    D,1:0,2:1,3:1,4:1,5:0,6:0,7:0,8:0,9:1,10:1,11:0
//    C,B:15
//  Handle more than 64 leds is second unit daisy chained - including brightness

// Leds Ports map from 0 to 127
// Servo Ports from 150
// Digital Ports from 200 to

// Max 7219 Library
#include "LedControl.h"


// Pins 7,8,9 as the ones used for Hornet, so will standardise on that
LedControl lc_2 = LedControl(9, 8, 7, 1);

/* we always wait a bit between updates of the display */
unsigned long delaytime = 250;
unsigned long sdelaytime = 1000;

#define filename "General_Sim_7219 2020302a"


#include <SPI.h>
#include <Ethernet.h>
#include <EthernetUdp.h>

#include <Servo.h>


// Uncomment for Left Board
byte mac[] = {0x03, 0xC7, 0x3E, 0xCA, 0x35, 0x02};
IPAddress ip(172, 16, 1, 110);

// Uncomment for Front Board
//byte mac[] = {0x03,0xC7,0x3E,0xCA,0x35,0x03};
//IPAddress ip(172,16,1,21);

// Uncomment for Right Board
//byte mac[] = {0x03,0xC7,0x3E,0xCA,0x35,0x04};
//IPAddress ip(172,16,1,22);

const unsigned int localport = 13135;


IPAddress targetIP(172, 16, 1, 2);
const unsigned int reflectorport = 27000;


EthernetUDP Udp;


// Do not use UDP_TX_PACKET_MAX_SIZE for character buffer as it is only small
// https://stackoverflow.com/questions/54232090/unintentional-strange-characters-added-to-packets-during-udp-communication-in-ar

char packetBuffer[1500]; //buffer to store the incoming data

const unsigned int listenport = 7789;
EthernetUDP rxUdp;
const unsigned int txport = 7788;
EthernetUDP txUDP;

char receivePacketBuffer[1500]; //buffer to store the incoming data
char *ParameterNamePtr;
char *ParameterValuePtr;



// Module connection pins (Digital Pins)
#define CLK 2
#define DIO 3



boolean Debug_Display = true;
char byteIn = 0;
int loopcounter = 0;

// Keepalive reporting
long lKeepAlive = 0;
long ltime = 0;

long llastDisplayUpdateMillis = 0;
long llastServoMillis = 0;

int  iServoDirection = 1;
int  iServoPos = 0;
const int NoOfServos = 18;
const int ServoMinValue = 0;
const int ServoMaxValue = 180;
const int ServoBasePort = 21;
// Number of mS between repositioning the servo
const int iServoUpdatePeriod = 10;

const int ServoStartUDPPort = 150;
const int OutputStartUDPPort = 200;

// Deal with guages such as VSI
int iServoStartPos[NoOfServos + 1];

// Safety limits such as needles  on VOR
int iServoMinPos[NoOfServos + 1];
int iServoMaxPos[NoOfServos + 1];

int  iServoDesiredPos[NoOfServos + 1];
int  iServoCurrentPos[NoOfServos + 1];

const int NoOfOutputs = 14;
// Initially was just using left over ports on dual row digital out
const int BaseOutputPort = 39;
// Gained some additional ports on side connector
const int SecondBasePort = 14;
// Either ponts to first BaseOutputPort or SecondBasePort
int relativeOutputBasePort = 0;
int PortOffset = 0;
int iDesiredOutput[NoOfOutputs + 1];

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
  Serial.println("");
  Serial.println(filename);


  Serial.println("Starting Network");
  Ethernet.begin( mac, ip);
  Serial.print("IP = ");
  rxUdp.begin( listenport );
  Serial.println(Ethernet.localIP());
  Serial.print("Port=");
  Serial.println(listenport);
  Serial.println("");
  Serial.println("Waiting 1 second for link to negotiate");
  delay(1000);
  Serial.println("Completed Arbitary wait");

  //
  txUDP.begin( txport );
  txUDP.beginPacket(targetIP, reflectorport);
  txUDP.println("Init General_Sim_7219 UDP");
  txUDP.endPacket();


  Serial.println("Network Initialised");


  //Clear the UDP Buffer
  for (int i = 0; i < UDP_TX_PACKET_MAX_SIZE; i++) receivePacketBuffer[i] = 0;


  lKeepAlive = 0;


}






/*
  This function lights up a some Leds in a row.
  The pattern will be repeated on every row.
  The pattern will blink along with the row-number.
  row number 4 (index==3) will blink 4 times etc.
*/
void rows() {
  for (int row = 0; row < 8; row++) {
    delay(sdelaytime);
    lc_2.setRow(0, row, B10100000);
    delay(sdelaytime);
    lc_2.setRow(0, row, (byte)0);
    for (int i = 0; i < row; i++) {
      delay(sdelaytime);
      lc_2.setRow(0, row, B10100000);
      delay(sdelaytime);
      lc_2.setRow(0, row, (byte)0);
    }
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
  ParameterNamePtr = strtok(packetBuffer, delim);



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

    while ( ParameterNamePtr != NULL ) {
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

    while ( ParameterNamePtr != NULL ) {
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


      if (lledvalue == 0)
      {
        if (Debug_Display || bLocalDebug ) Serial.println("Clearing - Row:" + String(iledRow) + " Column:" +  String(iledColumn));
        lc_2.setLed(0, iledRow, iledColumn, false);

      }
      else
      {
        if (Debug_Display || bLocalDebug ) Serial.println("Lighting - Row:" + String(iledRow) + " Column:" +  String(iledColumn));
        lc_2.setLed(0, iledRow, iledColumn, true);
      }
    }
    else if (llednumber >= ServoStartUDPPort && llednumber <= (ServoStartUDPPort + NoOfServos  - 1))
    {
      // Handle Servos

      iServoDesiredPos[llednumber - (ServoStartUDPPort - 1)] = lledvalue;


    }
    // Eight Output Pins for Relays
    // UDP Ports start at 200
    else if (llednumber >= OutputStartUDPPort && llednumber <= (OutputStartUDPPort + NoOfOutputs - 1))
    {
      // Handle Digital Outputput - Pins 40 to 48 and 14 to 21
      // First 9 Ports on Digital Output Bank
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
        lc_2.setIntensity(0, isetting);
      else if (Debug_Display || bLocalDebug ) Serial.println("Invalid Brightness value passoed. Value is :" + String(setting));
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
      strIndex[1] = (i == maxIndex) ? i + 1 : i;
    }
  }
  return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}

boolean isValidNumber(String str)
{
  boolean isNum = false;
  if (!(str.charAt(0) == '+' || str.charAt(0) == '-' || isDigit(str.charAt(0)))) return false;

  for (byte i = 1; i < str.length(); i++)
  {
    if (!(isDigit(str.charAt(i)) || str.charAt(i) == '.')) return false;
  }
  return true;
}

void loop() {
  boolean bLocalDebug = true;
  // Check to see if anything has landed in UDP buffer


  ltime = millis();
  if ((ltime - lKeepAlive) > 1000)
  {

    lKeepAlive = millis();

  }


  int packetSize = rxUdp.parsePacket();

  if ( packetSize > 0)
  {


    Serial.println("Processing Packet");
    bLocalDebug = true;
    rxUdp.read( packetBuffer, packetSize);
    //terminate the buffer manually
    packetBuffer[packetSize] = '\0';

    ProcessReceivedString();
  }







}
