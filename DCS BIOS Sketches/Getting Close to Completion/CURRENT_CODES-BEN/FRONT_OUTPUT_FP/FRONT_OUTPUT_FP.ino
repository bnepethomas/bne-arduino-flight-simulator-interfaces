// UPDATED TO DSCS-BIOS FP EDITION FOR OPEN HORNET


////////////////////---||||||||||********||||||||||---\\\\\\\\\\\\\\\\\\\\
//||               FUNCTION = FRONT OUTPUT                           ||\\
//||              LOCATION IN THE PIT = LIP LEFT HAND SIDE             ||\\
//||            ARDUINO PROCESSOR TYPE = Arduino Mega                ||\\
//||      ARDUINO CHIP SERIAL NUMBER = SN - 859373138373516121E2      ||\\
//||      ETHERNET SHEILD MAC ADDRESS = MAC -                         ||\\
//||            PROGRAM PORT CONNECTED COM PORT = COM 10             ||\\
//||            NATIVE PORT CONNECTED COM PORT = COM 16               ||\\
//||            ****DO CHECK S/N BEFORE UPLOAD NEW DATA****           ||\\
////////////////////---||||||||||********||||||||||---\\\\\\\\\\\\\\\\\\\\

// Tell DCS-BIOS to use a serial connection and use interrupt-driven
// communication. The main program will be interrupted to prioritize
// processing incoming data.
//
// This should work on any Arduino that has an ATMega328 controller
// (Uno, Pro Mini, many others).
String readString;
#include <Servo.h>
#define DCSBIOS_IRQ_SERIAL

#include "LedControl.h"
#include "DcsBios.h"

#define RED_STATUS_LED_PORT 5               // RED LED is used for monitoring ethernet
#define FLASH_TIME 1000


#define APU_PORT 30
#define ENGINE_CRANK_PORT 31
#define FUEL_DUMP_PORT 29
#define HOOK_BYPASS_PORT 28
#define LAUNCH_BAR_PORT 27

unsigned long NEXT_PORT_TOGGLE_TIMER = 0;
bool PORT_OUTPUT_STATE = false;





void setup() {



  pinMode( RED_STATUS_LED_PORT,  OUTPUT);

  digitalWrite( RED_STATUS_LED_PORT, true);
  delay(FLASH_TIME);
  digitalWrite( RED_STATUS_LED_PORT, false);
  delay(FLASH_TIME);

  pinMode( APU_PORT,  OUTPUT);
  pinMode( ENGINE_CRANK_PORT,  OUTPUT);
  pinMode( FUEL_DUMP_PORT,  OUTPUT);
  pinMode( HOOK_BYPASS_PORT,  OUTPUT);
  pinMode( LAUNCH_BAR_PORT,  OUTPUT);


  // Turn everything off
  digitalWrite(APU_PORT, false);
  digitalWrite(ENGINE_CRANK_PORT, false);
  digitalWrite(FUEL_DUMP_PORT, false);
  digitalWrite(HOOK_BYPASS_PORT, false);
  digitalWrite(LAUNCH_BAR_PORT, false);

  DcsBios::setup();

  NEXT_PORT_TOGGLE_TIMER = millis() + 1000;
}














void onLaunchBarSwChange(unsigned int newValue) {
  digitalWrite( RED_STATUS_LED_PORT, newValue);
  digitalWrite(LAUNCH_BAR_PORT, newValue);
}
DcsBios::IntegerBuffer launchBarSwBuffer(0x7480, 0x2000, 13, onLaunchBarSwChange);






void onApuControlSwChange(unsigned int newValue) {

  digitalWrite(APU_PORT, newValue);
}
DcsBios::IntegerBuffer apuControlSwBuffer(0x74c2, 0x0100, 8, onApuControlSwChange);

void onEngineCrankSwChange(unsigned int newValue) {
  bool CrankSwitchState = false;
  if (newValue != 1) {
    CrankSwitchState = true;
  }
  else
  {
    CrankSwitchState = false;
  }

  digitalWrite(ENGINE_CRANK_PORT, newValue);
}
DcsBios::IntegerBuffer engineCrankSwBuffer(0x74c2, 0x0600, 9, onEngineCrankSwChange);




void onFuelDumpSwChange(unsigned int newValue) {

  digitalWrite(FUEL_DUMP_PORT, newValue);
}
DcsBios::IntegerBuffer fuelDumpSwBuffer(0x74b4, 0x0100, 8, onFuelDumpSwChange);

void loop() {

  //  if (millis() >= NEXT_PORT_TOGGLE_TIMER) {
  //
  //    if ( PORT_OUTPUT_STATE == true) {
  //      PORT_OUTPUT_STATE = false;
  //    } else {
  //      PORT_OUTPUT_STATE = true;
  //
  //    }
  //    if( false) {
  //    digitalWrite(APU_PORT, PORT_OUTPUT_STATE);
  //      digitalWrite(ENGINE_CRANK_PORT, PORT_OUTPUT_STATE);
  //      digitalWrite(FUEL_DUMP_PORT, PORT_OUTPUT_STATE);
  //      digitalWrite(HOOK_BYPASS_PORT, PORT_OUTPUT_STATE);
  //      digitalWrite(LAUNCH_BAR_PORT, PORT_OUTPUT_STATE);
  //    }
  //
  //    digitalWrite( RED_STATUS_LED_PORT, PORT_OUTPUT_STATE);
  //    NEXT_PORT_TOGGLE_TIMER = millis() + 1000;
  //  }


  DcsBios::loop();
}
