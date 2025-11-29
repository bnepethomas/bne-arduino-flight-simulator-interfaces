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


  pinMode( PITOT_HEAT_PORT,  OUTPUT);



  // Turn everything off

  digitalWrite(PITOT_HEAT_PORT, false);



  NEXT_PORT_TOGGLE_TIMER = millis() + 1000;
  NEXT_STATUS_TOGGLE_TIMER = millis() + 1000;

  digitalWrite( RED_STATUS_LED_PORT, true);
}


void loop() {

  if (millis() > NEXT_STATUS_TOGGLE_TIMER) {
    GREEN_LED_STATE = !GREEN_LED_STATE;
    digitalWrite( GREEN_STATUS_LED_PORT, GREEN_LED_STATE);
    NEXT_STATUS_TOGGLE_TIMER = millis() + FLASH_TIME;

    digitalWrite(PITOT_HEAT_PORT, GREEN_LED_STATE);

  }

}
