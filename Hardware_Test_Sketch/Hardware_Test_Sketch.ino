

// Max 7219 Library 
#include "LedControl.h"

/*
 Now we need a LedControl to work with.
 ***** These pin numbers will probably not work with your hardware *****
 pin 12 is connected to the DataIn 
 pin 11 is connected to the CLK 
 pin 10 is connected to LOAD 
 We have only a single MAX72XX.
 LedControl lc=LedControl(6,5,4,1);
 */
LedControl lc=LedControl(6,5,4,4);

/* we always wait a bit between updates of the display */
unsigned long delaytime=250;



#define filename "bne-udp-7Seg-Servo-Rx-20160707a"

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
// The amount of time (in milliseconds) between tests
#define TEST_DELAY   1

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

#include <Wire.h>          // *** I2C Mode 
//  Board  I2C / TWI pins
//  Uno, Ethernet A4 (SDA), A5 (SCL)
//  Mega2560  20 (SDA), 21 (SCL)
//  Leonardo  2 (SDA), 3 (SCL)
//  Due 20 (SDA), 21 (SCL), SDA1, SCL1
#include <SPI.h>


#define OLED_Address 0x3c
#define OLED_Command_Mode 0x80
#define OLED_Data_Mode 0x40

#define cmd_CLS 0x01

#define STX 0x02
#define ETX 0x03



#define minoledPtr 1
#define maxoledPtr 2

#define minLinePtr 1
#define maxLinePtr 2

// Map Output Pins to OLED Displays
int oled1 = 2;                  // First usable output pin is D2 so all pins are incremented by 2 
int oled2 = 3;


char byteIn = 0;

char currentoled = 0;

int loopcounter = 0;

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


initialiseOutPutPins();
  
  Wire.begin();          // Start the I2C bus
  

 
 
 // *** I2C initial *** //
  delay(100);
   sendCommand(0x2A);  // **** Set "RE"=1  00101010B
   sendCommand(0x71);
   sendCommand(0x5C);
   sendCommand(0x28);
  
   sendCommand(0x08); // **** Set Sleep Mode On
   sendCommand(0x2A); // **** Set "RE"=1  00101010B
   sendCommand(0x79); // **** Set "SD"=1  01111001B
  
   sendCommand(0xD5);
   sendCommand(0x70);
   sendCommand(0x78); // **** Set "SD"=0
  
   sendCommand(0x08); // **** Set 5-dot, 3 or 4 line(0x09), 1 or 2 line(0x08)
  
   sendCommand(0x06); // **** Set Com31-->Com0  Seg0-->Seg99
  
   // **** Set OLED Characterization *** //
   sendCommand(0x2A);   // **** Set "RE"=1 
   sendCommand(0x79);   // **** Set "SD"=1
  
   // **** CGROM/CGRAM Management *** //
   sendCommand(0x72);   // **** Set ROM
   sendCommand(0x00);   // **** Set ROM A and 8 CGRAM
  
  
   sendCommand(0xDA);   // **** Set Seg Pins HW Config
   sendCommand(0x10);   
  
   sendCommand(0x81);   // **** Set Contrast
   sendCommand(0xFF); 
  
   sendCommand(0xDB);   // **** Set VCOM deselect level
   sendCommand(0x30);   // **** VCC x 0.83
  
   sendCommand(0xDC);   // **** Set gpio - turn EN for 15V generator on.
   sendCommand(0x03);
  
   sendCommand(0x78);   // **** Exiting Set OLED Characterization
   sendCommand(0x28); 
   sendCommand(0x2A); 
   //sendCommand(0x05);   // **** Set Entry Mode
   sendCommand(0x06);   // **** Set Entry Mode
   sendCommand(0x08);  
   sendCommand(0x28);   // **** Set "IS"=0 , "RE" =0 //28
   sendCommand(0x01); 
   sendCommand(0x80);   // **** Set DDRAM Address to 0x80 (line 1 start)
  
   delay(100);
   sendCommand(0x0C);   // **** Turn on Display  

 
  sendCommand(0x80);
  send_string("                ");
  sendCommand(0xC0);
  send_string("   00001        ");
    

    //Clear the UDP Buffer
    for(int i=0;i<UDP_TX_PACKET_MAX_SIZE;i++) receivePacketBuffer[i] = 0;


  
}



/*
 This method will display the characters for the
 word "Arduino" one after the other on digit 0. 
 */
void writeArduinoOn7Segment() {
  lc.setChar(0,0,'a',false);
  delay(delaytime);
  lc.setRow(0,0,0x05);
  delay(delaytime);
  lc.setChar(0,0,'d',false);
  delay(delaytime);
  lc.setRow(0,0,0x1c);
  delay(delaytime);
  lc.setRow(0,0,B00010000);
  delay(delaytime);
  lc.setRow(0,0,0x15);
  delay(delaytime);
  lc.setRow(0,0,0x1D);
  delay(delaytime);
  lc.clearDisplay(0);
  delay(delaytime);
} 

/*
  This method will scroll all the hexa-decimal
 numbers and letters on the display. You will need at least
 four 7-Segment digits. otherwise it won't really look that good.
 */
void scrollDigits() {
  for(int i=0;i<13;i++) {

    lc.setDigit(0,7,i,false);
    lc.setDigit(0,6,i,false);
    lc.setDigit(0,5,i,false);
    lc.setDigit(0,4,i,false);
    lc.setDigit(0,3,i,false);
    lc.setDigit(0,2,i+1,false);
    lc.setDigit(0,1,i+2,false);
    lc.setDigit(0,0,i+3,false);

    lc.setDigit(1,7,i,false);
    lc.setDigit(1,6,i,false);
    lc.setDigit(1,5,i,false);
    lc.setDigit(1,4,i,false);
    lc.setDigit(1,3,i,false);
    lc.setDigit(1,2,i+1,false);
    lc.setDigit(1,1,i+2,false);
    lc.setDigit(1,0,i+3,false);

    lc.setDigit(2,7,i,false);
    lc.setDigit(2,6,i,false);
    lc.setDigit(2,5,i,false);
    lc.setDigit(2,4,i,false);
    lc.setDigit(2,3,i,false);
    lc.setDigit(2,2,i+1,false);
    lc.setDigit(2,1,i+2,false);
    lc.setDigit(2,0,i+3,false);

    lc.setDigit(3,7,i,false);
    lc.setDigit(3,6,i,false);
    lc.setDigit(3,5,i,false);
    lc.setDigit(3,4,i,false);
    lc.setDigit(3,3,i,false);
    lc.setDigit(3,2,i+1,false);
    lc.setDigit(3,1,i+2,false);
    lc.setDigit(3,0,i+3,false);
    delay(delaytime);
  }
  lc.clearDisplay(0);
  delay(delaytime);
}


void initialiseOutPutPins() {
  pinMode(oled1,OUTPUT);
  pinMode(oled2,OUTPUT);


  digitalWrite(oled1, HIGH);
  digitalWrite(oled2, HIGH);


}

void sendData(unsigned char data)
{
    Wire.beginTransmission(OLED_Address);    // **** Start I2C 
    Wire.write(OLED_Data_Mode);         // **** Set OLED Data mode
    Wire.write(data);
    Wire.endTransmission();                     // **** End I2C 
    
}

void sendCommand(unsigned char command)
{
    Wire.beginTransmission(OLED_Address);    // **** Start I2C 
    Wire.write(OLED_Command_Mode);         // **** Set OLED Command mode
    Wire.write(command);
    Wire.endTransmission();                    // **** End I2C 
    
}

void send_string(const char *String)
{
    unsigned char i=0;
    while(String[i])
    {
        sendData(String[i]);      // *** Show String to OLED
        i++;
    }

}

void loop() {

  String testString;
  testString  = String(loopcounter);
  testString = testString + " Helloworld";
  testString.toCharArray(packetBuffer, 20);
  sendCommand(0xC0);
  send_string(packetBuffer);


  for(int k = 0; k < 1000; k += 1) {
    display.showNumberDec(k, 0);
    delay(TEST_DELAY);
  }

  
  writeArduinoOn7Segment();
  scrollDigits();
  loopcounter = loopcounter + 1;
  

}
