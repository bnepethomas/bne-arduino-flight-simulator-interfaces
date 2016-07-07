// Have encoder Tx working, RX UDP packet with no process, and display lit
// Need to now parse info to get and display value.


#define filename "bne-udp-7Seg-Servo-Rx-20160507c"

#include <SPI.h>
#include <Ethernet.h>
#include <EthernetUdp.h>

byte mac[] = { 
  0xA9,0xE7,0x3E,0xCA,0x34,0x1f};
IPAddress ip(192,168,1,205);
IPAddress TargetIPAddress(192,168,1,111);
const unsigned int localport = 13135;

EthernetUDP Udp;
char packetBuffer[UDP_TX_PACKET_MAX_SIZE]; //buffer to store the incoming data


const unsigned int listenport = 9920;
EthernetUDP rxUdp;
char receivePacketBuffer[UDP_TX_PACKET_MAX_SIZE]; //buffer to store the incoming data

//TMS Display
#include <TM1637Display.h>

// Module connection pins (Digital Pins)
#define CLK 8
#define DIO 9


TM1637Display display(CLK, DIO);


#include <Servo.h> 
#define starterOneServoPin 13
#define starterTwoServoPin 12

Servo starterOneServo;
Servo starterTwoServo;

#define apuPin 7

boolean APU_Starting = 0;
boolean APU_Stopping = 0;
boolean AC_Power = false;
boolean DC_Power = false;

boolean Debug_Display = false;

int LAND_ALT_STORE = 0;
int FLT_ALT_STORE = 0;

// End Encoder Variables


void setup() 
{

    digitalWrite(apuPin, HIGH);
    
    Serial.begin(115200); 
    Serial.println(filename);
    
    pinMode(apuPin, OUTPUT);
   
  


    // Sending Infrastructure
    Serial.println("Starting Network");
    Ethernet.begin( mac, ip);

  
    //Receiving Infrastructure

    rxUdp.begin( listenport );
    Serial.println("Network Initialised");

    Serial.println("Start LED Display");
    uint8_t data[] = { 0xff, 0xff, 0xff, 0xff };
    display.setBrightness(0x0f);
    
    // All segments on
    display.setSegments(data);
    Serial.println("Display Initialised");

    // Servo
    Serial.println("Start Servo");

    starterOneServo.attach(starterOneServoPin);
    starterTwoServo.attach(starterTwoServoPin);
    
    starterOneServo.write(60);
    delay(500);
    starterOneServo.write(200);
    delay(500);
    starterOneServo.write(60);

    starterTwoServo.write(60);
    delay(500);
    starterTwoServo.write(200);
    delay(500);
    starterTwoServo.write(60);
  
    Serial.println("Servo Initialised");

    Serial.println("Start Relay");
    digitalWrite(apuPin, LOW);
    delay(1000);
    digitalWrite(apuPin, HIGH);

    Serial.println("Relay Initialised");

    //Clear the UDP Buffer
    for(int i=0;i<UDP_TX_PACKET_MAX_SIZE;i++) receivePacketBuffer[i] = 0;
    
}

// main loop, work is done by interrupt service routines, this one only prints stuff
void loop() {


  int packetSize = rxUdp.parsePacket();
  if(packetSize)
  {
   Serial.println("Received UDP Packet"); 
   //Read the packet into the Buffer
   rxUdp.read(receivePacketBuffer,UDP_TX_PACKET_MAX_SIZE);
   ProcessUDPPacket();  

    //Clear the UDP Buffer
   for(int i=0;i<UDP_TX_PACKET_MAX_SIZE;i++) receivePacketBuffer[i] = 0;
  } 


}
// End Main Loop






void ProcessUDPPacket()
{
  Serial.println("Processing Packet");

  const char *delim  = "=";  

  String PACKET_ID = strtok(receivePacketBuffer,delim);
  String ID_STRING(PACKET_ID);
  Serial.println(ID_STRING);

  
  char *PACKET_DATA;
  PACKET_DATA   = strtok(NULL,delim);   
  Serial.println(PACKET_DATA);

  Serial.println(PACKET_ID);
  if (ID_STRING.substring(0) == "LAND_ALT")
   {    
    Serial.println("Got a value to display");
    //CAUTION_DATA = atoi(PACKET_DATA);
    
    LAND_ALT_STORE = atoi(PACKET_DATA);
    if (DC_Power && AC_Power) {
      display.showNumberDec(LAND_ALT_STORE);
    }

   }

   if (ID_STRING.substring(0) == "STARTER_1_SOLENOID")
   {    
      Serial.println("Got a value to display for STARTER_1_SOLENOID");
  
      int payload;
      payload = atoi(PACKET_DATA);
      if (payload==0){
        // Assume just switched off move servo and then return
      starterOneServo.write(60);
      delay(100);
      starterOneServo.write(200);
      delay(500);
      starterOneServo.write(60);  
      }
   }

   if (ID_STRING.substring(0) == "STARTER_2_SOLENOID")
   {    
      Serial.println("Got a value to display for STARTER_2_SOLENOID");
  
      int payload;
      payload = atoi(PACKET_DATA);
      if (payload==0){
        // Assume just switched off move servo and then return
        starterTwoServo.write(60);
        delay(100);
        starterTwoServo.write(200);
        delay(500);
        starterTwoServo.write(60);  
      }

   }

   if (ID_STRING.substring(0) == "APU_STOPPING")
   {    
    Serial.println("Got a value to display for APU_STOPPING");

    int payload;
    payload = atoi(PACKET_DATA);
    if (payload==0){
        APU_Stopping = false;
      }
      else
      {
        APU_Stopping = true;  
      }
   }

   if (ID_STRING.substring(0) == "APU_STARTING")
   {    
    Serial.println("Got a value to display for APU_STARTING");

    int payload;
    payload = atoi(PACKET_DATA);
    if (payload==0){
        APU_Starting = false;
      }
      else
      {
        APU_Starting = true;  
      }
   } 

   if (APU_Starting || APU_Stopping) {
      digitalWrite(apuPin, LOW);
   } else
   {
      digitalWrite(apuPin, HIGH);
   }



   if (ID_STRING.substring(0) == "AC_POWER")
   {    
    Serial.println("Got a value to display for AC_POWER");

    int payload;
    payload = atoi(PACKET_DATA);
    if (payload==0){
        AC_Power = false;
      }
      else
      {
        AC_Power = true;  
      }
      CheckPowerStatus();
   } 

   if (ID_STRING.substring(0) == "DC_POWER")
   {    
    Serial.println("Got a value to display for DC_POWER");

    int payload;
    payload = atoi(PACKET_DATA);
    if (payload==0){
      DC_Power = false;
    }
    else
    {
      DC_Power = true;  
    }

    CheckPowerStatus();
      
   }


  
}

void CheckPowerStatus()
{

  if (AC_Power) Serial.println("AC Available");
  if (DC_Power) Serial.println("DC Available");  
  if (DC_Power && AC_Power){
    Serial.println("Display On");
    display.setBrightness(0x0f);
    display.showNumberDec(LAND_ALT_STORE);
  } else if (DC_Power && !AC_Power) {
    Serial.println("Display Partial");
    display.setBrightness(0x0f);
    uint8_t data[] = { 0x01, 0x01, 0x01, 0x01 };
    display.setSegments(data);
  } else {
    Serial.println("Display Off");
    display.setBrightness(0x00);
    display.showNumberDec(LAND_ALT_STORE);
  }
}



