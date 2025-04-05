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

unsigned long NEXT_STATUS_TOGGLE_TIMER = 0;
bool RED_LED_STATE = false;
unsigned long timeSinceRedLedChanged = 0;

U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C u8g2_BARO(U8G2_R0, /* reset=*/U8X8_PIN_NONE);


extern "C" {
#include "utility/twi.h"  // from Wire library, so we can do bus scanning
}

#define TCAADDR 0x70


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
  txPin[16] = 69;  //A15
  txPin[17] = 68;  //14
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
    digitalWrite(txPin[i], LOW);
  }
}

void AllPinsHigh() {
  for (int i = 1; i <= 26; i++) {
    digitalWrite(txPin[i], HIGH);
  }
}

void tcaselect(uint8_t i) {
  if (i > 7) return;

  Wire.beginTransmission(TCAADDR);
  Wire.write(1 << i);
  Wire.endTransmission();
}

void SendDebug(String MessageToSend) {
  // Serial.println(MessageToSend);
}


void setup() {
  // Serial.begin(115200);
  // Serial.write("Cable Tester Start");
  // SendDebug("hello");
  pinMode(54, INPUT_PULLUP);


  Wire.begin();


  // Had to comment out these debugging messages as they created a conflict with the IRQ definition in DCS BIOS
  //Serial.println("\nScanning I2C Bus");

  for (uint8_t t = 0; t < 8; t++) {
    tcaselect(t);
    // Had to comment out these debugging messages as they created a conflict with the IRQ definition in DCS BIOS
    SendDebug("TCA Port #" + String(t));

    for (uint8_t addr = 0; addr <= 127; addr++) {
      //if (addr == TCAADDR) continue;

      uint8_t data;
      if (!twi_writeTo(addr, &data, 0, 1, 1)) {
        // Had to comment out these debugging messages as they created a conflict with the IRQ definition in DCS BIOS
        SendDebug("Found I2C 0x" + String(addr));
      }
    }
  }
  // Had to comment out these debugging messages as they created a conflict with the IRQ definition in DCS BIOS
  SendDebug("I2C scan complete");

  u8g2_BARO.begin();
  u8g2_BARO.clearBuffer();
  u8g2_BARO.setFont(u8g2_font_logisoso16_tf);
  u8g2_BARO.sendBuffer();
  updateBARO("CABLE TEST INIT");


  SetupPins();
}



void updateBARO(String strnewValue) {

  const char* newValue = strnewValue.c_str();

  u8g2_BARO.setFontMode(0);
  u8g2_BARO.setDrawColor(0);
  u8g2_BARO.drawBox(0, 0, 128, 32);
  u8g2_BARO.setDrawColor(1);
  u8g2_BARO.setFontDirection(0);
  u8g2_BARO.drawStr(0, 16, newValue);
  u8g2_BARO.sendBuffer();
}



void loop() {
  if (digitalRead(54) == LOW) {
    bool errorEncountered = false;
    updateBARO("Test Start");
    delay(300);



    // This test should pass without any cables connected
    // Just checking that pins go high
    AllPinsHigh();
    for (int i = 1; i <= 26; i++) {
      if (digitalRead(rxPin[i]) != HIGH) {
        updateBARO("Pin " + String(i) + " not HIGH");
        delay(5000);
        errorEncountered = true;
      };
    }

    updateBARO("All High");
    delay(500);
    AllPinsLow();
    updateBARO("All Low");
    delay(500);

    // Find last connected pin
    SendDebug("Finding last connected pin");
    int LastConnectedPin = 0;
    txPinsToFloat();
    for (int i = 1; i <= 26; i++) {
      updateBARO("Pin " + String(i));
      //SendDebug("Setting Low Pin " + String(txPin[i]) + " Reading Pin " + String(rxPin[i]));
      pinMode(txPin[i], OUTPUT);
      digitalWrite(txPin[i], LOW);
      delay(50);
      if (digitalRead(rxPin[i]) == LOW) {
        SendDebug("Pin " + String(rxPin[i]) + " LOW");
        LastConnectedPin = i;
      };
      pinMode(txPin[i], INPUT);
    }
    if (LastConnectedPin == 0) {
      updateBARO("No cable connected");
      errorEncountered = true;
    } else
      updateBARO("Testing to " + String(LastConnectedPin));
    delay(500);

    // Test that only a single pin changes
    if (errorEncountered != true) {
      for (int i = 1; i <= LastConnectedPin; i++) {
        updateBARO("Short Test " + String(txPin[i]));
        delay(500);
        txPinsToFloat();
        pinMode(txPin[i], OUTPUT);
        digitalWrite(txPin[i], LOW);
        for (int j = 1; i <= LastConnectedPin; i++) {
          
          if (errorEncountered == true)
            break;
          if (j != i) {
            // Checking to see if other pins have been pulled low
            if (digitalRead(rxPin[j]) == LOW) {
              // We have a different pin to what we expect going low
              errorEncountered = true;

              updateBARO("Short " + String(i) + ":" + String(j));
              txPinsToFloat();
              delay(10000);
              break;
            }
          }
        }
        pinMode(txPin[i], INPUT);
      }
    }

    if (errorEncountered == true)
      updateBARO("Test Failed");
    else
      updateBARO("Pass to : " + String(LastConnectedPin));
  }
}
