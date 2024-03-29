
#include <AccelStepper.h>
#include <Stepper.h>
#define STEPS 720  // steps per revolution (limited to 315°)

#define FLASH_TIME 600
bool RED_LED_STATE = false;
long NEXT_STATUS_TOGGLE_TIMER = 0;
#define COIL_STEPPER_1_A 4
#define COIL_STEPPER_1_B 5
#define COIL_STEPPER_1_C 6
#define COIL_STEPPER_1_D 7

// #define STEPPER_MAX_SPEED 900
#define STEPPER_MAX_SPEED 300
#define STEPPER_ACCELERATION 150

AccelStepper STEPPER_1(AccelStepper::FULL4WIRE, COIL_STEPPER_1_A, COIL_STEPPER_1_B, COIL_STEPPER_1_C, COIL_STEPPER_1_D);




void setup() {
  // put your setup code here, to run once:
  pinMode(LED_BUILTIN, OUTPUT);

  digitalWrite(LED_BUILTIN, true);
  delay(FLASH_TIME);
  digitalWrite(LED_BUILTIN, false);

  delay(FLASH_TIME);


  STEPPER_1.setMaxSpeed(STEPPER_MAX_SPEED);
  STEPPER_1.setAcceleration(STEPPER_ACCELERATION);

  STEPPER_1.moveTo(3000);
  while (STEPPER_1.distanceToGo() != 0) {
    STEPPER_1.run();
  }
    STEPPER_1.moveTo(0);
  while (STEPPER_1.distanceToGo() != 0) {
    STEPPER_1.run();
  }
}




void loop() {

  if (millis() >= NEXT_STATUS_TOGGLE_TIMER) {
    RED_LED_STATE = !RED_LED_STATE;
    digitalWrite(LED_BUILTIN, RED_LED_STATE);
    // enableAllSteppers();
    // cycleSteppers(650);
    // disableAllSteppers();
    // SendDebug("Uptime " + String(millis()) + " (" + String(millis() / 60000) + ")");
    NEXT_STATUS_TOGGLE_TIMER = millis() + FLASH_TIME;
    // Check to see if model time is updating - if nothing after 30 seconds disble steppers
  }
}
