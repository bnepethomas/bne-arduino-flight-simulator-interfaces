#include <Dhcp.h>
#include <Dns.h>
#include <ethernet_comp.h>
#include <UIPClient.h>
#include <UIPEthernet.h>
#include <UIPServer.h>
#include <UIPUdp.h>



// Compass working now working on clock


// 20171205 Merged OLED SSD1306 and Stepper code
// Current traget platform must be Mega
// need to include new network driver


#include <SPI.h>
//#include <Ethernet.h>
//#include <EthernetUdp.h>

byte mac[] = { 
  0xA9,0xE7,0x3E,0xCA,0x34,0x1D};
IPAddress ip(192,168,1,107);
const unsigned int localport = 13135;

EthernetUDP udp;
char packetBuffer[200]; //buffer to store the incoming data



int val, Clock_val;
int CompassPos, ClockPos;
int LastCompassPos = 0;
int LastClockPos = 0;
int CurrentDirection, ClockCurrentDirection;
boolean CompassZeroed = false;
boolean ClockZeroed = false;
double StepperNumbers[4];

const int MaxSteps = 3780;
const int delaytime = 10;
// const int clockdelay = 23430;            // Should actually be 23.43mS
const int clockdelay = 2000;            // Should actually be 23.43mS
const int cClockwise = 1;
const int cCounterClockwise = -1;
const int cUnknownPos = 999;



/*********************************************************************
This is an example for our Monochrome OLEDs based on SSD1306 drivers

  Pick one up today in the adafruit shop!
  ------> http://www.adafruit.com/category/63_98

This example is for a 128x64 size display using SPI to communicate
4 or 5 pins are required to interface

Adafruit invests time and resources providing this open source code, 
please support Adafruit and open-source hardware by purchasing 
products from Adafruit!

Written by Limor Fried/Ladyada  for Adafruit Industries.  
BSD license, check license.txt for more information
All text above, and the splash screen must be included in any redistribution
*********************************************************************/

// Need to record last thousands, ten etc and only blackout old entry if a value has changed.



#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Fonts/FreeMonoBoldOblique12pt7b.h>
#include <Fonts/FreeMonoBold18pt7b.h>  
#include <Fonts/FreeMono9pt7b.h>   

// If using software SPI (the default case):
#define OLED_MOSI   9
#define OLED_CLK   10
#define OLED_DC    11
#define OLED_CS    12
#define OLED_RESET 13
Adafruit_SSD1306 display(OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);

/* Uncomment this block to use hardware SPI
#define OLED_DC     6
#define OLED_CS     7
#define OLED_RESET  8
Adafruit_SSD1306 display(OLED_DC, OLED_RESET, OLED_CS);
*/

#define NUMFLAKES 10
#define XPOS 0
#define YPOS 1
#define DELTAY 2

#define LOGO16_GLCD_HEIGHT 16 
#define LOGO16_GLCD_WIDTH  16 
static const unsigned char PROGMEM logo16_glcd_bmp[] =
{ B00000000, B11000000,
  B00000001, B11000000,
  B00000001, B11000000,
  B00000011, B11100000,
  B11110011, B11100000,
  B11111110, B11111000,
  B01111110, B11111111,
  B00110011, B10011111,
  B00011111, B11111100,
  B00001101, B01110000,
  B00011011, B10100000,
  B00111111, B11100000,
  B00111111, B11110000,
  B01111100, B11110000,
  B01110000, B01110000,
  B00000000, B00110000 };

#if (SSD1306_LCDHEIGHT != 64)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif

#define cursor_multiplier 2.5

// Original Font
//#define ten_Column_Pos 35

//#define ten_Column_Pos 40
//#define hundred_Column_Pos 19
//#define thousand_Column_Pos 0

//8
#define ten_Column_Pos 45
#define hundred_Column_Pos 24
#define thousand_Column_Pos 5

int sensorPin = A1; 

int thousandscounter = 0;
int pressure = 1013;
bool goingup = true; 
long startmillis = 0;
long timetaken = 0;
bool debugging = false;
int AltimeterCurrentPos = 0;
int AltimeterDesiredPos = 0;
long NextIncrementDecrementTime = 0;


void setup()
{


  
 
    Serial.begin(115200);
    
    // by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
    display.begin(SSD1306_SWITCHCAPVCC);
    // init done
    
    // Show image buffer on the display hardware.
    // Since the buffer is intialized with an Adafruit splashscreen
    // internally, this will display the splashscreen.
    display.setRotation(3);
    display.display();
    display.setTextSize(3);
    display.setTextColor(WHITE);
    
    // Clear the buffer.
    display.clearDisplay();
    startmillis = millis();
    
    display.setFont(&FreeMonoBold18pt7b);
    display.setTextSize(1);
    
    DrawHatch();  
     
    thousandscounter = 103;
    testtext();
    display.display(); 

    // currently enabling ethernet stops display working - spi clash?
    //Ethernet.begin( mac, ip);
    //udp.begin( localport );
    
    int x;
    StepperNumbers[0] = 0;      // Compass
    StepperNumbers[1] = 0;      // Clock
    CompassPos= cUnknownPos;
    CurrentDirection = cCounterClockwise;
    ClockCurrentDirection = cCounterClockwise;


    for( int i = 22; i <= 42; i++)
    {
        pinMode( i, OUTPUT );
    }

   

    // If zero pos read only step a little bit
    for ( int zulu = 0; zulu < 1; zulu++)
    {
        for (int  x = 0; x < 3000; x++)
        {
          Serial.println("Winding back altimeter");
          StepCounterClockwise();
          
          val = analogRead(0);       // read analog input pin 0  
          Serial.println(val); // prints the value read
          if (val >= 200) {
            Serial.println("Zero exiting");
            CompassZeroed = true;
            delay(5000);
            x=5000;
          }
          //delayMicroseconds(clockdelay);
        }
        for (int  x = 0; x < 29; x++)
        {
          Serial.println("Winding to top altimeter");
          StepCounterClockwise();
        }
 
    }

   
    // If zero not found then step 20 steps clockwise

}




// ******************************************************
// OLED Code merge




void DrawHatch(void) {



  display.fillRect(thousand_Column_Pos, 20, thousand_Column_Pos + 21, 23, WHITE);




  for (int i=5; i < 9;  i++ ) {
    display.drawLine(thousand_Column_Pos,10 + i, thousand_Column_Pos +21, i  + 10 + 10, BLACK);
  }

  for (int i=5; i < 9;  i++ ) {
    display.drawLine(thousand_Column_Pos,17 + i, thousand_Column_Pos +21, i  + 17 + 10, BLACK);
  }

  for (int i=5; i < 9;  i++ ) {
    display.drawLine(thousand_Column_Pos,24 + i, thousand_Column_Pos +21, i  + 24 + 10, BLACK);
  }

    for (int i=5; i < 9;  i++ ) {
    display.drawLine(thousand_Column_Pos,31 + i, thousand_Column_Pos +21, i  + 31 + 10, BLACK);
  }
}


void testtext(void) {

  static int last_thousands_counter = -1;
  static int last_thousands = -1;
  static int last_hundreds = -1;
  static int last_tens = -1;
  static int last_ones = -1;
  static int last_pressure = -1;

  
  // Nothing has changed since last time - get out of here
  if ((thousandscounter == last_thousands_counter) && (last_pressure == pressure))
    return;
    
  
  int ones = (thousandscounter%10);
  int tens = ((thousandscounter/10)%10);
  int hundreds = ((thousandscounter/100)%10);
  int thousands = (thousandscounter/1000);


  if (last_thousands != thousands) {
    
    last_thousands = thousands;
    if (thousands == 0) DrawHatch();
    else {
      // Orginal default font location
      // display.fillRect(0, 21, 15, 21, BLACK);
      //display.setCursor(0,21);
      // New Font location
      display.fillRect(thousand_Column_Pos, 20, 21, 23, BLACK);
      // Orignal Font
      //display.setCursor(0,42);
      // New Font
      display.setCursor(thousand_Column_Pos,42);
      display.println(thousands);
    }
  }


  if (last_hundreds != hundreds) {
    last_hundreds = hundreds;

    // Orginal default font location
    //display.fillRect(16, 21, 15, 21, BLACK);
    //display.setCursor(16,21);
    
    display.fillRect(hundred_Column_Pos, 20, 21, 23, BLACK);
    display.setCursor(hundred_Column_Pos,42);
    display.println(hundreds);
  }


  if (last_ones != ones) {  

    // Catch a possible negative value which would cause font to wrap
    if (ones < 0) 
      return;
    last_ones = ones;
    int ones_converted = ones * -1;

    

    // Orginal Font
    //display.fillRect(35, 0, 15, 65, BLACK);
    // New Font
    display.fillRect(ten_Column_Pos, 0, 22, 65, BLACK);
    // Orginal Font
    //display.setCursor(35,-5 +  ones_converted * cursor_multiplier);
    // New Font
    display.setCursor(ten_Column_Pos,16 +  ones_converted * cursor_multiplier);
    display.println((tens + 9) % 10);
  
  
    // Centre line which aligns with the hundreds and thousands
    // Orginal Font
    //display.setCursor(35,21 +  ones_converted * cursor_multiplier);
    // New Font
    display.setCursor(ten_Column_Pos,42 +  ones_converted * cursor_multiplier);
    display.println(tens);
  
  
    // At the bottom of the screen
    // Orginal Font
    //display.setCursor(35,45 +  ones_converted * cursor_multiplier);
    // New Font
    display.setCursor(ten_Column_Pos,66 +  ones_converted * cursor_multiplier);
    display.println((tens + 1) % 10);
  
  
    // At the bottom below the screen
    // Orginal Font
    //display.setCursor(35,69 +  ones_converted * cursor_multiplier);
    // New Font
    display.setCursor(ten_Column_Pos,90 +  ones_converted * cursor_multiplier);
    display.println((tens + 2) % 10);
  
    // Remove bottom of last character to remove distraction of digit appearing and disappearing
    // Orginal Font
    //display.fillRect(35, 65, 15, 25, BLACK);
    // New Font
    display.fillRect(ten_Column_Pos, 65, 20, 27, BLACK);
  }

  
  if (last_pressure != pressure) {

    last_pressure = pressure;
    display.setCursor(10,122);
    display.setFont(&FreeMono9pt7b);

    display.setTextSize(1);
    display.println(pressure);
    // Reset display font to large
    display.setTextSize(1);
    display.setFont(&FreeMonoBold18pt7b);

  }

  display.display();  

}


// *********************************************************






//pin 22 23 24 - PA0 PA1 PA2
void sendCmdTo595_1( const byte cmd )
{
    byte bitMask = B00000001;
    for( int i = 0; i < 8; i++)
    {
        /*
        WARNING!!!  don't use digitalWrite here because it's speed is too slow,
        use avr c style to control the IO pin HIGH / LOW
        http://arduino.cc/en/Hacking/PinMapping2560
        http://www.instructables.com/id/Arduino-is-Slow-and-how-to-fix-it/?ALLSTEPS
        */
        PORTA = (bitMask & cmd) > 0 ? (PORTA | B00000001) : (PORTA & B11111110);
        //give a raise pulse to 595 input clock pin
        PORTA &= B11111101;
        PORTA |= B00000010;

        bitMask = bitMask << 1;
    }
    //send data from shift register to storage register
    PORTA &= B11111011;
    PORTA |= B00000100;
}


void StepClockwise()
{

    //Digital Port 22 Maps to PortA-0
    //Digital Port 23 Maps to PortA-1

    //Stepping order based on page 2 of VID69 clock datasheet
    //Delay between steps is 3*7.81mS
    //Total pulse train 164mS is 7 steps

    //Step 1
  
    PORTA &= B00001001;
    PORTA |= B00001001;
    delayMicroseconds(clockdelay);

    //Step 2
  
    PORTA &= B00000001;
    PORTA |= B00000001;
    delayMicroseconds(clockdelay);

    //Step 3
  
    PORTA &= B00000111;
    PORTA |= B00000111;
    delayMicroseconds(clockdelay);

    //Step 4
  
    PORTA &= B00000110;
    PORTA |= B00000110;
    delayMicroseconds(clockdelay);

    //Step 5
  
    PORTA &= B00001110;
    PORTA |= B00001110;
    delayMicroseconds(clockdelay);

    //Step 6
  
    PORTA &= B00001000;
    PORTA |= B00001000;
    delayMicroseconds(clockdelay);

    //Step 7
  
    PORTA &= B00001001;
    PORTA |= B00001001;
    delayMicroseconds(clockdelay);

    if (CompassPos != cUnknownPos)
    {
      CompassPos = CompassPos + 1;
    };
   
    CurrentDirection = cClockwise;
}






void StepCounterClockwise()
{

    //Digital Port 22 Maps to PortA-0
    //Digital Port 23 Maps to PortA-1

    //Stepping order based on page 2 of VID69 clock datasheet
    //Delay between steps is 3*7.81mS
    //Total pulse train 164mS is 7 steps

    //Step 1
  
    PORTA &= B00000000;
    PORTA |= B00000000;
    delayMicroseconds(clockdelay);

    //Step 2
  
    PORTA &= B00001000;
    PORTA |= B00001000;
    delayMicroseconds(clockdelay);

    //Step 3
  
    PORTA &= B00001110;
    PORTA |= B00001110;
    delayMicroseconds(clockdelay);

    //Step 4
  
    PORTA &= B00000110;
    PORTA |= B00000110;
    delayMicroseconds(clockdelay);

    //Step 5
  
    PORTA &= B00000111;
    PORTA |= B00000111;
    delayMicroseconds(clockdelay);

    //Step 6
  
    PORTA &= B00000001;
    PORTA |= B00000001;
    delayMicroseconds(clockdelay);

    //Step 7
  
    PORTA &= B00001001;
    PORTA |= B00001001;
    delayMicroseconds(clockdelay);

    if (CompassPos != cUnknownPos)
    {
      CompassPos = CompassPos - 1;
    };
   
    CurrentDirection = cCounterClockwise;
 
}




void loop()
{
 

    // Grab UDP Packet
    int packetSize = udp.parsePacket();
    if( packetSize > 0) {
        Serial.println("Packet Received");
        udp.read( packetBuffer, packetSize);
     //terminate the buffer manually
     packetBuffer[packetSize] = '\0';
        
        if (packetBuffer[0] == 'S') {    
          // We have some data
          Serial.println(packetBuffer);
            // Split the string up
           char* endPos;
           int i = 0;
           char *p = packetBuffer + 1;
           char *str;
           while ((str = strtok_r(p, ":", &p)) != NULL) 
           {// delimiter is the semicolon
             Serial.print( i);
             Serial.print("-");
             Serial.println(str);
             StepperNumbers[i] = strtod(str, &endPos);
             i++;
             
          }
      } 
    }
  

  testtext();

  int sensorValue = analogRead(sensorPin);
  // Serial.println(sensorValue);
  // Sensor Value Ranges 0 to 1023
  



  if ( millis() >= NextIncrementDecrementTime) {

     if (sensorValue > 500) { 
        goingup = true;
        //delay(1023 - sensorValue);
        NextIncrementDecrementTime = millis() + (1023 - sensorValue);
        }
    else {
        goingup = false;
        //delay(sensorValue);
        NextIncrementDecrementTime = millis() + (sensorValue);
    }
    if (goingup) {
      thousandscounter++;
    }
    else{
      thousandscounter--;
    }  
  }
  
  if ((thousandscounter > 100) || (thousandscounter < 0)) {
    if (thousandscounter > 9999) goingup = false;
    if (thousandscounter < -1) goingup = true;
    //thousandscounter=0;
    timetaken =  millis() - startmillis;
    Serial.println(timetaken);
    startmillis = millis();
    
  }



   val = analogRead(0);       // read analog input pin 0  
   // Serial.println(val); // prints the value read


  Serial.print("Altitude: ");
  Serial.println(thousandscounter);

   // Load the desired set point
   
   StepperNumbers[0] = int(((thousandscounter/10)%10) * 6  + (thousandscounter%10)* 0.6);
   Serial.print("Deisred point :");
   Serial.println( StepperNumbers[0]);

   // Process Compass
   if (!CompassZeroed)
   {
    
     if (val > 250)  {   
           CompassPos= 0;
           CompassZeroed = true;     
      }
      else
        StepClockwise();
   }
   else 
   {
      Serial.print("Compass Pos: ");
      Serial.println(CompassPos);
      

      Serial.print("Micros: ");
      Serial.println(micros());
    
      // Check to see if we have hit the zero point
      //if (val > 250) {
      //  if (CurrentDirection == cClockwise)    
      //     CompassPos= 0;   
      //  else
      //     CompassPos = 4;  
      //}
  
      // Dlay when testing to verify we have hit zero point
      //if ((CompassPos == 0) & (LastCompassPos != 0)) {
      //    delay(6000);
      //}
      
      if (CompassPos >= 60) CompassPos = 0;
      if (CompassPos <= -1)  CompassPos = 59;
     
      LastCompassPos = CompassPos;
      if (CompassPos != StepperNumbers[0]) {
        // Not at desired desired compass position so move it
        if (((CompassPos - StepperNumbers[0]) < 30) &&
           ((StepperNumbers[0] - CompassPos) < 30)  ) {
          if (CompassPos > StepperNumbers[0]) 
            StepCounterClockwise();
          else 
            StepClockwise();
        }
        else {
          if (CompassPos > StepperNumbers[0]) 
            StepClockwise();
          else 
            StepCounterClockwise();        
        }
      }
      // StepCounterClockwise();
   }
  
  
  
   
  
  
    delay(delaytime);
   
  

}

