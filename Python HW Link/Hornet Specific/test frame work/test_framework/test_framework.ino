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


boolean Debug_Display = true;
char byteIn = 0;
int loopcounter = 0;

// Keepalive reporting
long lKeepAlive = 0;
long ltime = 0;

long llastDisplayUpdateMillis = 0;
long llastServoMillis = 0;



const int ServoStartUDPPort = 150;
const int OutputStartUDPPort = 200;



const int NoOfOutputs = 14;
// Initially was just using left over ports on dual row digital out
const int BaseOutputPort = 39;
// Gained some additional ports on side connector
const int SecondBasePort = 14;
// Either ponts to first BaseOutputPort or SecondBasePort
int relativeOutputBasePort = 0;
int PortOffset = 0;
int iDesiredOutput[NoOfOutputs + 1];





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

  // We are expected a LedNumber which has XRRCC where X = Max7219 Unit, RR Row, CC Column
  bool bLocalDebug = false;

  const int ExpectedLedNumberLength = 5;
  const int ExpectedLedValueLength = 1;

  const int Max7219UnitNumber = 6;
  const int MaxRowNumber = 15;
  const int MaxColNumber = 15;
  const int MaxLedValue = 1;

  if (Debug_Display || bLocalDebug ) Serial.println("Handling " + str);

  int delimeterlocation = 0;
  String lednumber = "";
  String ledvalue = "";
  String workstring = "";


  int i7219UnitNumber = 0;
  int iledRow = 0;
  int iledColumn = 0;
  int iledValue = 0;


  delimeterlocation = str.indexOf(':');

  if (delimeterlocation == 0)
  {
    if (Debug_Display || bLocalDebug ) Serial.println("**** WARNING no delimiter passed ****** Looking for :");
  }
  else
  {
    if (Debug_Display || bLocalDebug ) Serial.println("Delimiter is located a position " + String(delimeterlocation));
    lednumber = getValue(str, ':', 0);
    if (Debug_Display || bLocalDebug ) Serial.println("lednumber is " + lednumber);
    ledvalue = getValue(str, ':', 1);
    if (Debug_Display || bLocalDebug ) Serial.println("ledvalue is " + ledvalue);


    // First Check Lengths are ok
    if (lednumber.length() != ExpectedLedNumberLength) {
      if (Debug_Display || bLocalDebug ) Serial.println("lednumber has incorrect length of " + String(lednumber.length()) + ". Should be " + String(ExpectedLedNumberLength));
      return;
    }


    // As the value could contain the null at the end of the string trim it out
    ledvalue.trim();
    if (ledvalue.length() != ExpectedLedValueLength) {
      if (Debug_Display || bLocalDebug ) Serial.println("ledvalue has incorrect length of " + String(ledvalue.length()) + ". Should be " + String(ExpectedLedValueLength));
      return;
    }




    // Check we are only have numberic characters
    if ( ! isValidNumber(lednumber) ) {
      if (Debug_Display || bLocalDebug ) Serial.println("lednumber is not a number " + lednumber);
      return;
    }

    if ( ! isValidNumber(ledvalue) ) {
      if (Debug_Display || bLocalDebug ) Serial.println("ledvalue is not a number " + ledvalue);
      return;
    }




    // Pull out Max7219 Unit Number, Row and Column Numbers
    // Max 7219 Number
    workstring = lednumber.substring(0, 1);
    if (Debug_Display || bLocalDebug ) Serial.println("Max7219 Unit Number is :" + workstring);
    i7219UnitNumber = workstring.toInt();

    if (i7219UnitNumber > Max7219UnitNumber) {
      if (Debug_Display || bLocalDebug ) Serial.println("Max7219 Unit Number is " + String(i7219UnitNumber) + " and is greater than allowed " + String(Max7219UnitNumber));
      return;
    }

    // Led Row
    workstring = lednumber.substring(1, 3);
    if (Debug_Display || bLocalDebug ) Serial.println("Led Row Number is :" + workstring);
    iledRow = workstring.toInt();

    if (iledRow > MaxRowNumber) {
      if (Debug_Display || bLocalDebug ) Serial.println("Led Row Number is " + String(iledRow) + " and is greater than allowed " + String(MaxRowNumber));
      return;
    }

    // Led Column
    workstring = lednumber.substring(3, 5);
    if (Debug_Display || bLocalDebug ) Serial.println("Led Column Number is :" + workstring);
    iledColumn = workstring.toInt();

    if (iledColumn > MaxColNumber) {
      if (Debug_Display || bLocalDebug ) Serial.println("Led Column Number is " + String(iledColumn) + " and is greater than allowed " + String(MaxColNumber));
      return;
    }


    //Led Value
    workstring = ledvalue.substring(0, 1);
    if (Debug_Display || bLocalDebug ) Serial.println("Led Value is :" + workstring);
    iledValue = workstring.toInt();

    if (iledValue > MaxLedValue) {
      if (Debug_Display || bLocalDebug ) Serial.println("Led Value is " + String(iledValue) + " and is greater than allowed " + String(MaxLedValue));
      return;
    }




    // Have Now Validated payload - set the led 
    if (iledValue == 0)
    {
      if (Debug_Display || bLocalDebug ) Serial.println("Clearing - Row:" + String(iledRow) + " Column:" +  String(iledColumn));
      lc_2.setLed(i7219UnitNumber, iledRow, iledColumn, false);

    }
    else
    {
      if (Debug_Display || bLocalDebug ) Serial.println("Lighting - Row:" + String(iledRow) + " Column:" +  String(iledColumn));
      lc_2.setLed(i7219UnitNumber, iledRow, iledColumn, true);
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
