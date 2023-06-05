#include <SPI.h>
#include <Ethernet.h>

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(172, 16, 1, 11);

#define RED_STATUS_LED_PORT 5     // RED LED is used for monitoring ethernet
#define GREEN_STATUS_LED_PORT 13  // RED LED is used for monitoring ethernet
#define FLASH_TIME 300

//#define PROJECTOR_BRAND "OPTOMA"
#define PROJECTOR_BRAND "BENQ"
unsigned long NEXT_STATUS_TOGGLE_TIMER = 0;
bool GREEN_LED_STATE = false;
bool RED_LED_STATE = false;
unsigned long timeSinceRedLedChanged = 0;

#define SWITCH_PRIMARY_PIN 7
bool SWITCH_PRIMARY_STATE = false;
long SWITCH_PRIMARY_DEBOUNCE_FINISH = 0;

#define SWITCH_ROOF_PIN 6
bool SWITCH_ROOF_STATE = false;
long SWITCH_ROOF_DEBOUNCE_FINISH = 0;

long SWITCH_DEBOUNCE_LENGTH = 20;
void setup() {


  // Using manual reset instead of tying to Arduino Reset
  pinMode(53, OUTPUT);
  digitalWrite(53, LOW);
  delay(2);
  digitalWrite(53, HIGH);

  pinMode(SWITCH_PRIMARY_PIN, INPUT_PULLUP);
  SWITCH_PRIMARY_STATE = digitalRead(SWITCH_PRIMARY_PIN);
  pinMode(SWITCH_ROOF_PIN, INPUT_PULLUP);
  SWITCH_ROOF_STATE = digitalRead(SWITCH_ROOF_PIN);

  pinMode(RED_STATUS_LED_PORT, OUTPUT);
  pinMode(GREEN_STATUS_LED_PORT, OUTPUT);

  digitalWrite(RED_STATUS_LED_PORT, true);
  digitalWrite(GREEN_STATUS_LED_PORT, true);
  delay(FLASH_TIME);
  digitalWrite(RED_STATUS_LED_PORT, false);
  digitalWrite(GREEN_STATUS_LED_PORT, false);

  delay(FLASH_TIME);

  Ethernet.begin(mac, ip);

  Serial1.begin(9600);
  Serial2.begin(9600);
  Serial3.begin(9600);
}

void loop() {

  if (millis() >= NEXT_STATUS_TOGGLE_TIMER) {
    GREEN_LED_STATE = !GREEN_LED_STATE;
    RED_LED_STATE = !RED_LED_STATE;
    digitalWrite(GREEN_STATUS_LED_PORT, GREEN_LED_STATE);
    // digitalWrite( RED_STATUS_LED_PORT, RED_LED_STATE);
    NEXT_STATUS_TOGGLE_TIMER = millis() + FLASH_TIME;
  }

  if (millis() >= SWITCH_PRIMARY_DEBOUNCE_FINISH) {
    if (digitalRead(SWITCH_PRIMARY_PIN) != SWITCH_PRIMARY_STATE) {
      SWITCH_PRIMARY_DEBOUNCE_FINISH = millis() + SWITCH_DEBOUNCE_LENGTH;
      SWITCH_PRIMARY_STATE = !SWITCH_PRIMARY_STATE;
      RED_LED_STATE = SWITCH_PRIMARY_STATE;
      digitalWrite(RED_STATUS_LED_PORT, RED_LED_STATE);
      if (SWITCH_PRIMARY_STATE == true) {
        Serial1.println("\r*POW=ON#");
        Serial2.println("\r*POW=ON#");
        Serial3.println("\r*POW=ON#");
      } else {
        Serial1.print("\r*POW=OFF#\r");
        Serial2.print("\r*POW=OFF#\r");
        Serial3.print("\r*POW=OFF#\r");
      }
    }
  }
}

//  // Turn off Red status led after flashtime
//  if ((RED_LED_STATE == true) && (millis() >= (timeSinceRedLedChanged + FLASH_TIME ) )) {
//    digitalWrite( RED_STATUS_LED_PORT, false);
//    RED_LED_STATE = false;
//    timeSinceRedLedChanged = millis();
//
//  }
//}
