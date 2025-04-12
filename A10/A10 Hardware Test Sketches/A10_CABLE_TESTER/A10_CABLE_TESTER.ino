//  Cable Tester

// Tests IDC cables
//
// Input Device is a single push button
// Status is reflected using single oled directly attached to I2C. The OLED model
// is the same as used in the OPT displays on the UFC -  0.91" 128*32 SSD1306
// https://www.ebay.com/itm/0-91-Inch-128x32-IIC-I2c-White-Blue-OLED-LCD-Display-Module-3-3-5v-For-Arduino/392552169768?ssPageName=STRK%3AMEBIDX%3AIT&var=661536491479&_trksid=p2057872.m2749.l2649

/*
Program flow
on power on display startup message
create arrays on output and input pings

Pull all inputslow using inbuilt pull down 
On button press
first check all inputs read low
walk each input to determine cable width by pulling high and validating change
display pin being tested
display width at col 0 
Set all outputs to zero
output all high aside from pin being tested - validate pin being tested does not change
repeat until we have reached the lat expected pin.


*/



// Basic logic
// Initialise I2C Bus (wire)
// Iterate through each port on the Mux and list connected devices to serial port (simple troubleshooting aid), may use UDP later
// Initialise each display
// Wait for UDP updates
// during timeout grab and smooth analo 0 and get brightness for all displays

//As we are low on pins - need to use some of the Analog pins
// #define PIN_A0   (54)
// #define PIN_A1   (55)
// #define PIN_A2   (56)
// #define PIN_A3   (57)
// #define PIN_A4   (58)
// #define PIN_A5   (59)
// #define PIN_A6   (60)
// #define PIN_A7   (61)
// #define PIN_A8   (62)
// #define PIN_A9   (63)
// #define PIN_A10  (64)
// #define PIN_A11  (65)
// #define PIN_A12  (66)
// #define PIN_A13  (67)
// #define PIN_A14  (68)
// #define PIN_A15  (69)

#include <U8g2lib.h>
#include <SPI.h>
#include <Wire.h>



#define STATUS_LED_PORT LED_BUILTIN
#define FLASH_TIME 500
#define START_TEST_BUTTON 54

unsigned long NEXT_STATUS_TOGGLE_TIMER = 0;
bool RED_LED_STATE = false;
unsigned long timeSinceRedLedChanged = 0;

U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C u8g2_DISPLAY(U8G2_R0, /* reset=*/U8X8_PIN_NONE);


// extern "C" {
// #include "utility/twi.h"  // from Wire library, so we can do bus scanning
// }

// #define TCAADDR 0x70


int CurrentDisplay = 0;
int Brightness = 0;
char buffer[20];  //plenty of space for the value of millis() plus a zero terminator


char* ptrtopass;

int txPin[30];
int rxPin[30];

void SetupPins() {
  txPin[1] = 1;
  txPin[2] = 2;
  txPin[3] = 3;
  txPin[4] = 4;
  txPin[5] = 5;
  txPin[6] = 6;
  txPin[7] = 7;
  txPin[8] = 8;
  txPin[9] = 9;
  txPin[10] = 10;
  txPin[11] = 11;
  txPin[12] = 12;
  txPin[13] = 13;
  txPin[14] = 14;
  txPin[15] = 15;
  txPin[16] = A15;  //A15
  txPin[17] = A14;  //A14 68
  txPin[18] = 18;
  txPin[19] = 19;
  txPin[20] = 20;
  txPin[21] = 21;
  txPin[22] = 22;
  txPin[23] = 23;
  txPin[24] = 24;
  txPin[25] = 25;
  txPin[26] = 26;

  rxPin[1] = 27;
  rxPin[2] = 28;
  rxPin[3] = 29;
  rxPin[4] = 30;
  rxPin[5] = 31;
  rxPin[6] = 32;
  rxPin[7] = 33;
  rxPin[8] = 34;
  rxPin[9] = 35;
  rxPin[10] = 36;
  rxPin[11] = 37;
  rxPin[12] = 38;
  rxPin[13] = 39;
  rxPin[14] = 40;
  rxPin[15] = 41;
  rxPin[16] = 42;
  rxPin[17] = 43;
  rxPin[18] = 44;
  rxPin[19] = 45;
  rxPin[20] = 46;
  rxPin[21] = 47;
  rxPin[22] = 48;
  rxPin[23] = 49;
  rxPin[24] = 50;
  rxPin[25] = 51;
  rxPin[26] = 52;

  for (int i = 1; i <= 26; i++) {
    pinMode(rxPin[i], INPUT_PULLUP);
  }

  for (int i = 1; i <= 26; i++) {
    pinMode(txPin[i], OUTPUT);
  }
}

void txPinsToFloat() {
  for (int i = 1; i <= 26; i++) {
    pinMode(txPin[i], INPUT_PULLUP);
  }
}

void AllPinsLow() {
  for (int i = 1; i <= 26; i++) {
    pinMode(txPin[i], OUTPUT);
    digitalWrite(txPin[i], LOW);
  }
}

void AllPinsHigh() {
  for (int i = 1; i <= 26; i++) {
    pinMode(txPin[i], OUTPUT);
    digitalWrite(txPin[i], HIGH);
  }
}

void SendDebug(String MessageToSend) {
  // Serial.println(MessageToSend);
}


void setup() {

  pinMode(START_TEST_BUTTON, INPUT_PULLUP);


  Wire.begin();

  u8g2_DISPLAY.begin();
  u8g2_DISPLAY.clearBuffer();
  u8g2_DISPLAY.setFont(u8g2_font_logisoso16_tf);
  u8g2_DISPLAY.sendBuffer();
  DisplayUpdate("CABLE TEST INIT");


  SetupPins();
}



void DisplayUpdate(String strnewValue) {


  Wire.begin();
  const char* newValue = strnewValue.c_str();

  u8g2_DISPLAY.setFontMode(0);
  u8g2_DISPLAY.setDrawColor(0);
  u8g2_DISPLAY.drawBox(0, 0, 128, 32);
  u8g2_DISPLAY.setDrawColor(1);
  u8g2_DISPLAY.setFontDirection(0);
  u8g2_DISPLAY.drawStr(0, 16, newValue);
  u8g2_DISPLAY.sendBuffer();
  Wire.end();
}



void loop() {
  if (digitalRead(START_TEST_BUTTON) == LOW) {
    bool errorEncountered = false;
    DisplayUpdate("Test Start");
    delay(300);



    // This test should pass without any cables connected
    // Just checking that pins go high
    AllPinsHigh();
    for (int i = 1; i <= 26; i++) {
      if (digitalRead(rxPin[i]) != HIGH) {
        DisplayUpdate("Pin " + String(i) + " not HIGH");
        delay(5000);
        errorEncountered = true;
      };
    }

    DisplayUpdate("All High");
    delay(200);
    AllPinsLow();
    DisplayUpdate("All Low");
    delay(200);

    // Find last connected pin
    SendDebug("Finding last connected pin");
    int LastConnectedPin = 0;
    txPinsToFloat();
    for (int i = 1; i <= 26; i++) {
      DisplayUpdate("Pin " + String(i));
      //SendDebug("Setting Low Pin " + String(txPin[i]) + " Reading Pin " + String(rxPin[i]));
      pinMode(txPin[i], OUTPUT);
      digitalWrite(txPin[i], LOW);
      delay(50);
      if (digitalRead(rxPin[i]) == LOW) {
        LastConnectedPin = i;
      };
      pinMode(txPin[i], INPUT);
    }
    if (LastConnectedPin == 0) {
      DisplayUpdate("No cable connected");
      errorEncountered = true;
    } else
      DisplayUpdate("Testing to " + String(LastConnectedPin));
    delay(500);



    // Test that only a single pin changes
    if (errorEncountered != true) {
      for (int i = 1; i <= LastConnectedPin; i++) {
        DisplayUpdate("Short Test " + String(txPin[i]));
        delay(100);
        txPinsToFloat();
        pinMode(txPin[i], OUTPUT);
        digitalWrite(txPin[i], LOW);
        for (int j = 1; j <= LastConnectedPin; j++) {

          if (errorEncountered == true)
            break;
          if (j != i) {
            // Checking to see if other pins have been pulled low
            if (digitalRead(rxPin[j]) == LOW) {
              // We have a different pin to what we expect going low
              errorEncountered = true;

              DisplayUpdate("Short " + String(i) + ":" + String(j));
              txPinsToFloat();
              delay(3000);
              break;
            }
          } else {
            // Check continuity for pin
            if (digitalRead(rxPin[j]) != LOW) {
              errorEncountered = true;

              DisplayUpdate("Open " + String(i) + ":" + String(j));
              txPinsToFloat();
              delay(3000);
              break;
            }
          }
        }
        pinMode(txPin[i], INPUT);
      }
    }

    if (errorEncountered == true)
      DisplayUpdate("Test Failed");
    else
      DisplayUpdate("Pass to : " + String(LastConnectedPin));
  }
}
