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

void tcaselect(uint8_t i) {
  if (i > 7) return;

  Wire.beginTransmission(TCAADDR);
  Wire.write(1 << i);
  Wire.endTransmission();
}

void SendDebug(String MessageToSend) {
  Serial.println(MessageToSend);
}

void setup() {
  Serial.begin(115200);
  Serial.write("Cable Tester Start");
  SendDebug("hello");
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
  delay(1000);
  updateBARO("STARTING CABLE TEST");
  delay(1000);
  updateBARO("CABLE IS 8 WIDE");


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
 if(digitalRead(54) == LOW) {
  updateBARO("Test Start");
  delay(2000);
  updateBARO("Test Complete");
 }

}
