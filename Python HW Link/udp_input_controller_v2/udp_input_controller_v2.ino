/* 


TO DO - when repsonding to CQ - first send 0 values and then one values to address issue with 3 position toggle switch
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

const  int ScanDelay = 80;
const int DebounceDelay = 20;

joyReport_t joyReport;
joyReport_t prevjoyReport;

long prevLEDTransition = millis();
int cButtonID[16];
bool bFirstTime = false;



#include <SPI.h>
#include <Ethernet.h>
#include <EthernetUdp.h>


const int AddressBit0 = 2;
const int AddressBit1 = 3;
const int AddressBit2 = 14;
const int AddressLED = 5;
const int StatusLED = 6;



int AddrBit0 = 0;
int AddrBit1 = 0;
int AddrBit2 = 0;
int NodeAddress = 0;

String deviceID = "00";


// Analog inputs

// V1 of the Ethernet interface PCB supports a total of 14 analog inputs
// 6 from A0 to A5, 4  from A8 to A11, and 4 from A12 to A15)
// The ports from A8 to A15 need to be jumpered to the PCB

// 


const long interval = 1000;             // interval at which to blink (milliseconds)
const long analogInterval = 10;         // Interval to read analog values
const long analogDisplayInterval = 100; // Interval to display and send updated values
unsigned long currentMillis = 0;
unsigned long previousMillis = 0;
unsigned long analogPreviousMillis = 0;
unsigned long analogDisplayPreviousMillis = 0;



const int numReadings = 10;
const int numAnalogInputs = 14;

// Given that inputs 6 and 7 are sitting underneath the Ethernet Shield and can't be easily accessed
// we can't simply want interfaces from A0 to A15. So create an array to walk for input ports
const int analogInputMapping[numAnalogInputs] = { 0,1,2,3,4,5,8,9,10,11,12,13,14,15};
int analogInputIndex = 0;

int readings[numAnalogInputs][numReadings];       // the readings from the analog input
int readIndex = 0;                                // the index of the current reading
int total[numAnalogInputs];                                    // the running total
int average[numAnalogInputs];                                  // the average
const int minDifferenceToUpdate = 1;  // Minimum change allow to stop jitter

int maxanalog[numAnalogInputs];
int minanalog[numAnalogInputs];
int lastSentAnalog[numAnalogInputs];

int ledState = LOW; 

// These local Mac and IP Address will be reassigned early in startup based on 
// the device ID as set by address pins
byte mac[] = {0x00,0xDD,0x3E,0xCA,0x36,0x00};
IPAddress ip(172,16,1,10);
String strMyIP = "X.X.X.X";

// Raspberry Pi is Target
IPAddress targetIP(172,16,1,2);
String strTargetIP = "X.X.X.X"; 

const unsigned int localport = 7788;
const unsigned int remoteport = 26027;
const unsigned int reflectorport = 27000;

EthernetUDP udp;
char packetBuffer[1000];     //buffer to store the incoming data
char outpacketBuffer[1000];  //buffer to store the outgoing data


// Variables associated with Network State
// Do not progress until a packet can be delivered to the target IP
// Originally planned to just use a ping - but libraries not in synch when using W5100
// Now using a undocumented return from UDP send 'udp.endPacket()' which indicates if packet sent
//    which appear to work nicely as the Ethernet shield trys to resolve ARP for target IP which
//    only completes when the shield has its link up.  If the target IP was on another subnet
//    this test would still be effective, but would only test for reachability of the default gateway

// The Warning LED will be a solid red until we start the Ethernet interface
// and then flash while we wait until we can sent a packet out.  
// The code will 'block' for approxiamtely 1.8 seconds while trying to send the packet
// Once a packet is delivered the target IP address then turn the led off

// A Status led will flash once a second to indicate the Arduino is alive and 
// will flash everytime a packet is sent 

const int NW_WaitingForLinkUp = 0;    // Trying to Sent a packet to the target IP
const int NW_Linkup = 1;              // Able to send a packet towards the target IP

int NW_State = NW_WaitingForLinkUp;
boolean StatusLEDState = 1;

// Note Pin 4 and Pin 10 cannot be used for other purposes.
//Arduino communicates with both the W5500 and SD card using the SPI bus (through the ICSP header). 
//This is on digital pins 10, 11, 12, and 13 on the Uno and pins 50, 51, and 52 on the Mega. 
//On both boards, pin 10 is used to select the W5500 and pin 4 for the SD card. These pins cannot be used for general I/O. 
//On the Mega, the hardware SS pin, 53, is not used to select either the W5500 or the SD card, 
//but it must be kept as an output or the SPI interface won't work.

char outData[100];
char stringind[5];
String outString;


void setup() {
  
  Serial.begin(115000);
  Serial.println("UDP Input Connector Setup");

  
  Serial.println("TODO - Analog inputs - only send data after averages have been determined otherwise ramp up is sent");
  Serial.println("TODO - Debounce array");
  pinMode(AddressBit0,INPUT_PULLUP);
  pinMode(AddressBit1,INPUT_PULLUP);
  pinMode(AddressBit2,INPUT_PULLUP);
  pinMode(AddressLED, OUTPUT);
  pinMode(StatusLED, OUTPUT);


  // Flash LEDs on
  digitalWrite(StatusLED, true);
  digitalWrite(AddressLED, true);
  delay(1000);
  digitalWrite(StatusLED, StatusLEDState);
  digitalWrite(AddressLED, false);


  for( int portId = 22; portId < 38; portId ++ )
  {
    pinMode( portId, OUTPUT );
  }


  // Should probably remove the ports that are used by the shield
  // These translate to port 50 to 53
  Serial.println("Clean up ports that can't be used");
  for( int portId = 38; portId < 54; portId ++ )
  {
    // Even though we've already got 10K external pullups
    pinMode( portId, INPUT_PULLUP);
  }


  for (int ind=0; ind < NUM_BUTTONS ; ind++) {
    joyReport.button[ind] = 0;
    prevjoyReport.button[ind] = 0;
  }




  // Determine Address of device
  // Note inverting result as we are grounding the input pin.
  // Which is using internal pull-ups.
  AddrBit0 = !digitalRead(AddressBit0);
  AddrBit1 = !digitalRead(AddressBit1);
  AddrBit2 = !digitalRead(AddressBit2);

  

  NodeAddress = AddrBit2 * 4 + AddrBit1 * 2 + AddrBit0;
  //Serial.println(AddrBit0);
  //Serial.println(AddrBit1);
  //Serial.println(AddrBit2);
  //Serial.println(NodeAddress);


  switch (NodeAddress) {
  case 0:
    {
      byte mac[] = {0x00,0xDD,0x3E,0xCA,0x36,0x00};
      ip = IPAddress(172,16,1,10);
      deviceID = "00";
      break;
    }
  case 1:
    {
      byte mac[] = {0x00,0xDD,0x3E,0xCA,0x36,0x01};
      ip = IPAddress(172,16,1,11);
      deviceID = "01";
      break;
    }
  case 2:
    {
      byte mac[] = {0x00,0xDD,0x3E,0xCA,0x36,0x02};
      ip = IPAddress(172,16,1,12);
      deviceID = "02";
      break;
    }
  case 3:
    {
      byte mac[] = {0x00,0xDD,0x3E,0xCA,0x36,0x03};
      ip = IPAddress(172,16,1,13);
      deviceID = "03";
      break;
    }
  case 4:
    {
      byte mac[] = {0x00,0xDD,0x3E,0xCA,0x36,0x04};
      ip = IPAddress(172,16,1,14);
      deviceID = "04";
      break;
    }
  case 5:
    {
      byte mac[] = {0x00,0xDD,0x3E,0xCA,0x36,0x05};
      ip = IPAddress(172,16,1,15);
      deviceID = "05";
      break;
    }
  case 6:
    {
      byte mac[] = {0x00,0xDD,0x3E,0xCA,0x36,0x06};
      ip = IPAddress(172,16,1,16);
      deviceID = "06";
      break;
    }
  case 7:
    {
      byte mac[] = {0x00,0xDD,0x3E,0xCA,0x36,0x07};
      ip = IPAddress(172,16,1,17);
      deviceID = "07";
      break;
    }
  default:
    // As a last restort assume same values as zero
    Serial.println("Warning hit default in Address Selection");
    Serial.println("Using device ID of 01");
    {
      byte mac[] = {0x00,0xDD,0x3E,0xCA,0x36,0x00};
      ip = IPAddress(172,16,1,10);
      deviceID = "00";
      break;
    }



  }


  // Flash the address led to show the boards Device ID
  int NumberofFlashes = NodeAddress;
  digitalWrite(StatusLED, false);
  digitalWrite(AddressLED, false);
  delay(500);
  while (NumberofFlashes > 0) {
    Serial.println("Flashing Address LED " + String(NumberofFlashes) +  " of " + String(NodeAddress + 1));
    digitalWrite(AddressLED, true);  
    delay(500);
    digitalWrite(AddressLED, false);  
    delay(500);
    --NumberofFlashes;
  }
  digitalWrite(StatusLED, StatusLEDState);

  // Convert IP Addresses to String to simplify log messages later
  strTargetIP = String(targetIP[0]) + "." + String(targetIP[1]) + "." + String(targetIP[2]) + "." + String(targetIP[3]); 
  strMyIP = String(ip[0]) + "." + String(ip[1]) + "." + String(ip[2]) + "." + String(ip[3]);
  
  Serial.print("Device ID: ");
  Serial.println(deviceID);


  Serial.println("Starting IP Stack");
  Serial.println("My IP = " + strMyIP);

  // Initialise the network interface
  // Check to see if packet was sent (as opposed to acually delivered)
  // What appears to actually happen is the W5100 tries to ARP and waits aroun 1.8 seconds
  //    before given up, so no delay is needed in the retry loop
  // A point of interest is the W5100 itself repsonds to pings without bothering
  //    the Arduino itself, response times is typically sub 02.mS, and means
  //    pings will be responded to even if the Arduino is sitting in a delay()

  Ethernet.begin( mac, ip);
  NW_State = NW_WaitingForLinkUp;
  
  udp.begin( localport );
  udp.beginPacket(targetIP, reflectorport);
  udp.println("Init UDP - " + strMyIP + String(millis()));
  if(!udp.endPacket()) {
    // Failed to send packet
    NW_WaitingForLinkUp;
  }
  else {
    Serial.println("Packet sent ok to " + strTargetIP + " (" + String(millis()) + "mS)");
    NW_State = NW_Linkup;
  }
  
  while (NW_State != NW_Linkup) {
    Serial.println("Packet send failed to " + strTargetIP + " - retrying" + " (" + String(millis()) + "mS)");
    StatusLEDState = !StatusLEDState;
    digitalWrite(StatusLED, StatusLEDState);
    udp.beginPacket(targetIP, reflectorport);
    udp.println("Init UDP");
    if(!udp.endPacket()) {
      NW_WaitingForLinkUp;
    }
    else {
      Serial.println("Packet sent ok to " + strTargetIP + " (" + String(millis()) + "mS)");
      NW_State = NW_Linkup;
    }
  }
  




  StatusLEDState = false;
  digitalWrite(StatusLED, StatusLEDState); 
  

  // Initialise arrays for Analog 
  for (int thisAnalogInput = 0; thisAnalogInput < numAnalogInputs; thisAnalogInput++) {
    
    for (int thisReading = 0; thisReading < numReadings; thisReading++) {
      readings[thisAnalogInput][thisReading] = 0;
    }

    maxanalog[thisAnalogInput] = 0;
    minanalog[thisAnalogInput] = 1023;
    total[thisAnalogInput] = 0;                                   
    average[thisAnalogInput] = 0; 
    lastSentAnalog[numAnalogInputs] = 0;
  }

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

        
        // Whilst internal array is zero based - sending data 1 based.
        sprintf(stringind, "%03d", ind + 1);
 


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


        // Do a little debounce
        // Will change this to debounce array with millis() + debounce delay on a per switch basis
        delay(DebounceDelay);
      }
      
    }
}


void loop() {


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
    colResult[0] = (PIND & B10000000)== 0 ? 0 : 1;
    //pin 39, PG2
    colResult[1] = (PING & B00000100)== 0 ? 0 : 1;
    //pin 40, PG1
    colResult[2] = (PING & B00000010)== 0 ? 0 : 1;
    //pin 41, PG0
    colResult[3] = (PING & B00000001)== 0 ? 0 : 1;

    //pin 42, PL7
    colResult[4] = (PINL & B10000000)== 0 ? 0 : 1;
    //pin 43, PL6
    colResult[5] = (PINL & B01000000)== 0 ? 0 : 1;
    //pin 44, PL5
    colResult[6] = (PINL & B00100000)== 0 ? 0 : 1;
    //pin 45, PL4
    colResult[7] = (PINL & B00010000)== 0 ? 0 : 1;

    //pin 46, PL3
    colResult[8] = (PINL & B00001000)== 0 ? 0 : 1;
    //pin 47, PL2
    colResult[9] = (PINL & B00000100)== 0 ? 0 : 1;
    //pin 48, PL1
    colResult[10] =(PINL & B00000010) == 0 ? 0 : 1;
    //pin 49, PL0
    colResult[11] =(PINL & B00000001) == 0 ? 0 : 1;

    // Unable to use pins 50-53 per the following
    //This is on digital pins 10, 11, 12, and 13 on the Uno and pins 50, 51, and 52 on the Mega. 
    //On both boards, pin 10 is used to select the W5500 and pin 4 for the SD card. These pins cannot be used for general I/O. 
    //On the Mega, the hardware SS pin, 53, is not used to select either the W5500 or the SD card, 
    //pin 50, PB3
    //colResult[12] =(PINB & B00001000) == 0 ? 0 : 1;
    colResult[12] = 0;
    //pin 51, PB2
    //colResult[13] =(PINB & B00000100) == 0 ? 0 : 0;
    colResult[13] = 0;
    //pin 52, PB1
    //colResult[14] =(PINB & B00000010) == 0 ? 0 : 0;
    colResult[14] = 0;
    //pin 53, PB0
    //colResult[15] =(PINB & B00000001) == 0 ? 0 : 1;
    colResult[15] = 0;    

    
    // There are 11 Columns per row - gives a total of 176 possible inputs
    // Have left the arrays dimensioned as per original code - if CPU or Memory becomes scarce reduce array
    for ( int colid = 0; colid < 16; colid ++ )
    {
      if ( colResult[ colid ] == 1 )
      {
        joyReport.button[ (rowid * 11) + colid ] =  1;
      }
      else
      {
        joyReport.button[ (rowid * 11) + colid ] =  0;
      }
    }
  }


  FindInputChanges();


  currentMillis = millis();
  if (currentMillis - analogPreviousMillis >= analogInterval) {

    for (int thisAnalogInput = 0; thisAnalogInput < numAnalogInputs; thisAnalogInput++) {
    
      analogPreviousMillis = currentMillis;
  

      // read the input pin - remebering we have to use mapping as not all inputs can be used
      
      int val = analogRead(analogInputMapping[thisAnalogInput]);  
      if (val > maxanalog[thisAnalogInput]) maxanalog[thisAnalogInput] = val;
      if (val < minanalog[thisAnalogInput]) minanalog[thisAnalogInput] = val;
  
    
  
      // subtract the last reading:
      total[thisAnalogInput] = total[thisAnalogInput] - readings[thisAnalogInput][readIndex];
      // read from the sensor:
      readings[thisAnalogInput][readIndex] = val;
      // add the reading to the total:
      total[thisAnalogInput] = total[thisAnalogInput] + readings[thisAnalogInput][readIndex];
      // advance to the next position in the array:

      
      // calculate the average:
      average[thisAnalogInput] = total[thisAnalogInput] / numReadings;
      // send it to the computer as ASCII digits
  
      currentMillis = millis();
      if (currentMillis - analogDisplayPreviousMillis >= analogDisplayInterval) {
        
        // Only do something if there has been a change larger than minDifferenceToUpdate
        
        if (average[thisAnalogInput] - minDifferenceToUpdate > lastSentAnalog[thisAnalogInput] || average[thisAnalogInput] + minDifferenceToUpdate < lastSentAnalog[thisAnalogInput] ) {
          lastSentAnalog[thisAnalogInput] = average[thisAnalogInput];
          
          Serial.println(String(lastSentAnalog[thisAnalogInput]));        
          Serial.println("Port " + String(analogInputMapping[thisAnalogInput]) + " Analog Value is " + String(val)  + " Average " + String(average[thisAnalogInput]) +  "  min:" + String(minanalog[thisAnalogInput]) + " max:" + String(maxanalog[thisAnalogInput]));

          outString = "A" + String(analogInputMapping[thisAnalogInput]) + ":" + String(average[thisAnalogInput]);
          udp.beginPacket(targetIP, reflectorport);
          udp.print(outString);
          udp.endPacket();
          
          
          udp.beginPacket(targetIP, remoteport);
          udp.print(outString);
          udp.endPacket();
        }
   
      }

    }     // Loop through inputs


    readIndex = readIndex + 1;
    
    // if we're at the end of the array...
    if (readIndex >= numReadings) {
      // ...wrap around to the beginning:
      readIndex = 0;
    }
    
  }


  currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    // save the last time you blinked the LED
    previousMillis = currentMillis;


    // if the LED is off turn it on and vice-versa:
    if (ledState == LOW) {
      ledState = HIGH;
    } else {
      ledState = LOW;
    }

    // set the LED with the ledState of the variable:
    digitalWrite(AddressLED, ledState);
  }

}
