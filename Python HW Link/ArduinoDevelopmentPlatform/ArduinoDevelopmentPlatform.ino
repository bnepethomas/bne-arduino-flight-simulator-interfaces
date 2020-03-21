
#include <SPI.h>
#include <Ethernet.h>
#include <EthernetUdp.h>


const int AddressBit0 = 2;
const int AddressBit1 = 3;
const int AddressBit2 = 14;
const int AddressLED = 5;
const int StatusLED = 6;

const long interval = 1000;           // interval at which to blink (milliseconds)

int AddrBit0 = 0;
int AddrBit1 = 0;
int AddrBit2 = 0;
int NodeAddress = 0;

String deviceID = "01";

unsigned long currentMillis;
unsigned long previousMillis = 0;



int ledState = LOW; 

// These local Mac and IP Address will be reassigned early in startup based on 
// the device ID as set by address pins
byte mac[] = {0x00,0xDD,0x3E,0xCA,0x35,0x02};
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




void setup() {
  
  Serial.begin(115000);
  Serial.println("Ethernet Framework");
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
      byte mac[] = {0x00,0xDD,0x3E,0xCA,0x35,0x02};
      ip = IPAddress(172,16,1,11);
      deviceID = "01";
      break;
    }
  case 1:
    {
      byte mac[] = {0x00,0xDD,0x3E,0xCA,0x35,0x03};
      ip = IPAddress(172,16,1,12);
      deviceID = "02";
      break;
    }
  case 2:
    {
      byte mac[] = {0x00,0xDD,0x3E,0xCA,0x35,0x04};
      ip = IPAddress(172,16,1,13);
      deviceID = "03";
      break;
    }
  case 3:
    {
      byte mac[] = {0x00,0xDD,0x3E,0xCA,0x35,0x04};
      ip = IPAddress(172,16,1,14);
      deviceID = "04";
      break;
    }
  case 4:
    {
      byte mac[] = {0x00,0xDD,0x3E,0xCA,0x35,0x05};
      ip = IPAddress(172,16,1,15);
      deviceID = "05";
      break;
    }
  case 5:
    {
      byte mac[] = {0x00,0xDD,0x3E,0xCA,0x35,0x06};
      ip = IPAddress(172,16,1,16);
      deviceID = "06";
      break;
    }
  case 6:
    {
      byte mac[] = {0x00,0xDD,0x3E,0xCA,0x35,0x07};
      ip = IPAddress(172,16,1,17);
      deviceID = "07";
      break;
    }
  case 7:
    {
      byte mac[] = {0x00,0xDD,0x3E,0xCA,0x35,0x08};
      ip = IPAddress(172,16,1,18);
      deviceID = "08";
      break;
    }
  default:
    // As a last restort assume same values as zero
    Serial.println("Warning hit default in Address Selection");
    Serial.println("Using device ID of 01");
    {
      byte mac[] = {0x00,0xDD,0x3E,0xCA,0x35,0x02};
      ip = IPAddress(172,16,1,11);
      deviceID = "01";
      break;
    }



  }


  // Flash the address led to show the boards Device ID
  int NumberofFlashes = NodeAddress + 1;
  delay(500);
  while (NumberofFlashes > 0) {
    Serial.println("Flashing Address LED " + String(NumberofFlashes) +  " of " + String(NodeAddress + 1));
    digitalWrite(AddressLED, true);  
    delay(500);
    digitalWrite(AddressLED, false);  
    delay(500);
    --NumberofFlashes;
  }

  // Convert IP Addresses to String to simplify log messages later
  strTargetIP = String(targetIP[0]) + "." + String(targetIP[1]) + "." + String(targetIP[2]) + "." + String(targetIP[3]); 
  strMyIP = String(ip[0]) + "." + String(ip[1]) + "." + String(ip[2]) + "." + String(ip[3]);
  
  Serial.print("Device ID: ");
  Serial.println(deviceID);


  Serial.println("Starting IP Stack");
  Serial.println("My IP = " + strMyIP);

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
  

  
}

void loop() {


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
