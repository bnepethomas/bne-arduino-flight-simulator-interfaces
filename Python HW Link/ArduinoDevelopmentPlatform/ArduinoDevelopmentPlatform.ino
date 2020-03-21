
#include <SPI.h>
#include <Ethernet.h>
#include <EthernetUdp.h>


const int AddressBit0 = 2;
const int AddressBit1 = 3;
const int AddressBit2 = 14;
const int AddressLed = 5;

const long interval = 1000;           // interval at which to blink (milliseconds)

int AddrBit0 = 0;
int AddrBit1 = 0;
int AddrBit2 = 0;
int NodeAddress = 0;

String deviceID = "01";

unsigned long currentMillis;
unsigned long previousMillis = 0;
int ledState = LOW; 

byte mac[] = {0x00,0xDD,0x3E,0xCA,0x35,0x02};
IPAddress ip(172,16,1,10);

// Raspberry Pi is Target
IPAddress targetIP(172,16,1,2);

const unsigned int localport = 7788;
const unsigned int remoteport = 26027;
const unsigned int reflectorport = 27000;

EthernetUDP udp;
char packetBuffer[1000];     //buffer to store the incoming data
char outpacketBuffer[1000];  //buffer to store the outgoing data


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
  pinMode(AddressLed, OUTPUT);

  delay(10);

  // Determine Address of device
  // Note inverting result as we are grounding the input pin.
  // Which is using internal pull-ups.
  AddrBit0 = !digitalRead(AddressBit0);
  AddrBit1 = !digitalRead(AddressBit1);
  AddrBit2 = !digitalRead(AddressBit2);

  

  NodeAddress = AddrBit2 * 4 + AddrBit1 * 2 + AddrBit0;
  Serial.println(AddrBit0);
  Serial.println(AddrBit1);
  Serial.println(AddrBit2);
  Serial.println(NodeAddress);


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
    {
      byte mac[] = {0x00,0xDD,0x3E,0xCA,0x35,0x02};
      ip = IPAddress(172,16,1,10);
      deviceID = "01";
      break;
    }



  }

  
  Serial.print("Device ID: ");
  Serial.println(deviceID);


  Serial.println("Starting IP Stack");
  Serial.print("IP = ");
  Serial.println(ip);
  Ethernet.begin( mac, ip);

  Serial.println("Waiting 5 seconds for link to negotiate");
  delay(5000);
  Serial.println("Completed Arbitary wait");  

  udp.begin( localport );
  udp.beginPacket(targetIP, reflectorport);
  udp.println("Init UDP");

  if(!udp.endPacket()) Serial.println(F("packet send failed"));
  else Serial.println(F("packet sent ok"));
  

  
}

void loop() {





//  if (AddrBit0 == HIGH) {
//    digitalWrite(AddressLed, LOW);
//  } else {
//    digitalWrite(AddressLed, HIGH);   
//  }


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
    digitalWrite(AddressLed, ledState);
  }

}
