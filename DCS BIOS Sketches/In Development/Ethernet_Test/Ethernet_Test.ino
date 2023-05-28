#include <SPI.h>
#include <Ethernet.h>

byte mac[] = {  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(172,16,1,11);

#define RED_STATUS_LED_PORT 13               // RED LED is used for monitoring ethernet
#define GREEN_STATUS_LED_PORT 5               // RED LED is used for monitoring ethernet
#define FLASH_TIME 300
unsigned long NEXT_STATUS_TOGGLE_TIMER = 0;
bool GREEN_LED_STATE = false;
bool RED_LED_STATE = false;
unsigned long timeSinceRedLedChanged = 0;

void setup() {


  // Using manual reset instead of tying to Arudino Reset
  pinMode(53,OUTPUT);
  digitalWrite(53,LOW);
  delay(2);
  digitalWrite(53,HIGH);

  pinMode( RED_STATUS_LED_PORT,  OUTPUT);
  pinMode( GREEN_STATUS_LED_PORT,  OUTPUT);

  digitalWrite( RED_STATUS_LED_PORT, true);
  digitalWrite( GREEN_STATUS_LED_PORT, true);
  delay(FLASH_TIME);
  digitalWrite( RED_STATUS_LED_PORT, false);
  digitalWrite( GREEN_STATUS_LED_PORT, false);

  delay(FLASH_TIME);

  Ethernet.begin(mac,ip);


}

void loop() {

    if (millis() >= NEXT_STATUS_TOGGLE_TIMER) {
    GREEN_LED_STATE = !GREEN_LED_STATE;
    RED_LED_STATE = !RED_LED_STATE;
    digitalWrite( GREEN_STATUS_LED_PORT, GREEN_LED_STATE);
    digitalWrite( RED_STATUS_LED_PORT, RED_LED_STATE);
    NEXT_STATUS_TOGGLE_TIMER = millis() + FLASH_TIME;
  }

//  // Turn off Red status led after flashtime
//  if ((RED_LED_STATE == true) && (millis() >= (timeSinceRedLedChanged + FLASH_TIME ) )) {
//    digitalWrite( RED_STATUS_LED_PORT, false);
//    RED_LED_STATE = false;
//    timeSinceRedLedChanged = millis();
//
//  }
  
}
