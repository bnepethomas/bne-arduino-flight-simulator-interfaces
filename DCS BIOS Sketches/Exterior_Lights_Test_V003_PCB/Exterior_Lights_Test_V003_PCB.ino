/*
 * The V003 PCB supports digital write, but not PWM
 * as the pins selected didn't support PWM, addressed with the V004 PCB
 * which uses a combination of pins on the upper and lower PCB
 */

#define STROBE_LIGHTS 30
#define NAVIGATION_LIGHTS 34
#define FORMATION_LIGHTS 31
#define BACK_LIGHTS 33
#define FLOOD_LIGHTS 32

void setup() {
  pinMode(STROBE_LIGHTS, OUTPUT);
  pinMode(NAVIGATION_LIGHTS, OUTPUT);
  pinMode(FORMATION_LIGHTS, OUTPUT);
  pinMode(BACK_LIGHTS, OUTPUT);
  pinMode(FLOOD_LIGHTS, OUTPUT);
}

void loop() {
  digitalWrite(STROBE_LIGHTS, HIGH);
  digitalWrite(NAVIGATION_LIGHTS, HIGH);
  digitalWrite(FORMATION_LIGHTS, HIGH);
  digitalWrite(BACK_LIGHTS, HIGH);
  digitalWrite(FLOOD_LIGHTS, HIGH);
  delay(500);
  digitalWrite(STROBE_LIGHTS, LOW);
  digitalWrite(NAVIGATION_LIGHTS, LOW);
  digitalWrite(FORMATION_LIGHTS, LOW);
  digitalWrite(BACK_LIGHTS, LOW);
  digitalWrite(FLOOD_LIGHTS, LOW);
  delay(500);
}
