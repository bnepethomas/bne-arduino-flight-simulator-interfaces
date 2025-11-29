// Simple Test sktech to test steppers and servos
String readString;
#include <Servo.h>


#include "LedControl.h"


#define RED_STATUS_LED_PORT 5               // RED LED is used for monitoring ethernet
#define GREEN_STATUS_LED_PORT 13               // RED LED is used for monitoring ethernet
#define FLASH_TIME 1000
unsigned long NEXT_STATUS_TOGGLE_TIMER = 0;
bool GREEN_LED_STATE = false;

#define LAUNCH_BAR_PORT 27
#define HOOK_BYPASS_PORT 28
#define FUEL_DUMP_PORT 29
#define APU_PORT 30
#define ENGINE_CRANK_PORT 31


#define BLEED_AIR_SOL_PORT 22
#define PITOT_HEAT_PORT 23
#define LASER_ARM_PORT 24
#define CANOPY_MAG_PORT 25



unsigned long NEXT_PORT_TOGGLE_TIMER = 0;
bool PORT_OUTPUT_STATE = false;


void setup() {


  pinMode( RED_STATUS_LED_PORT,  OUTPUT);
  pinMode( GREEN_STATUS_LED_PORT,  OUTPUT);

  digitalWrite( RED_STATUS_LED_PORT, true);
  digitalWrite( GREEN_STATUS_LED_PORT, true);
  delay(FLASH_TIME);
  digitalWrite( RED_STATUS_LED_PORT, false);
  digitalWrite( GREEN_STATUS_LED_PORT, false);
  delay(FLASH_TIME);

  pinMode( APU_PORT,  OUTPUT);
  pinMode( ENGINE_CRANK_PORT,  OUTPUT);
  pinMode( FUEL_DUMP_PORT,  OUTPUT);
  pinMode( HOOK_BYPASS_PORT,  OUTPUT);
  pinMode( LAUNCH_BAR_PORT,  OUTPUT);
  pinMode( BLEED_AIR_SOL_PORT,  OUTPUT);
  pinMode( PITOT_HEAT_PORT,  OUTPUT);
  pinMode( LASER_ARM_PORT,  OUTPUT);
  pinMode( CANOPY_MAG_PORT,  OUTPUT);


  // Turn everything off
  digitalWrite(APU_PORT, false);
  digitalWrite(ENGINE_CRANK_PORT, false);
  digitalWrite(FUEL_DUMP_PORT, false);
  digitalWrite(HOOK_BYPASS_PORT, false);
  digitalWrite(LAUNCH_BAR_PORT, false);
  digitalWrite(BLEED_AIR_SOL_PORT, false);
  digitalWrite(PITOT_HEAT_PORT, false);
  digitalWrite(LASER_ARM_PORT, false);
  digitalWrite(CANOPY_MAG_PORT, false);



  NEXT_PORT_TOGGLE_TIMER = millis() + 1000;
  NEXT_STATUS_TOGGLE_TIMER = millis() + 1000;

  digitalWrite( RED_STATUS_LED_PORT, true);
}


void loop() {

  if (millis() > NEXT_STATUS_TOGGLE_TIMER) {
    GREEN_LED_STATE = !GREEN_LED_STATE;
    digitalWrite( GREEN_STATUS_LED_PORT, GREEN_LED_STATE);
    NEXT_STATUS_TOGGLE_TIMER = millis() + FLASH_TIME;
    digitalWrite(APU_PORT, GREEN_LED_STATE);
    digitalWrite(ENGINE_CRANK_PORT, GREEN_LED_STATE);
    digitalWrite(FUEL_DUMP_PORT, GREEN_LED_STATE);
    digitalWrite(HOOK_BYPASS_PORT, GREEN_LED_STATE);
    digitalWrite(LAUNCH_BAR_PORT, GREEN_LED_STATE);
    digitalWrite(BLEED_AIR_SOL_PORT, GREEN_LED_STATE);
    digitalWrite(PITOT_HEAT_PORT, GREEN_LED_STATE);
    digitalWrite(LASER_ARM_PORT, GREEN_LED_STATE);
    digitalWrite(CANOPY_MAG_PORT, GREEN_LED_STATE);
  }

}
