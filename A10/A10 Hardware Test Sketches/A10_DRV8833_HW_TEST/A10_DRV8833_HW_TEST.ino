
#include <AccelStepper.h>
#include <Stepper.h>
#define STEPS 720  // steps per revolution (limited to 315°)

#define FLASH_TIME 600
bool RED_LED_STATE = false;
long NEXT_STATUS_TOGGLE_TIMER = 0;

#define ROLL_ZERO_SENSE_IN 8
bool SENSOR_STATE = false;

#define COIL_STEPPER_1_A 4
#define COIL_STEPPER_1_B 5
#define COIL_STEPPER_1_C 6
#define COIL_STEPPER_1_D 7

// MAX_SPEED of 150 is ocnsistently giving 5/2
#define STEPPER_MAX_SPEED 200
#define STEPPER_ACCELERATION 100

AccelStepper STEPPER_1(AccelStepper::FULL4WIRE, COIL_STEPPER_1_A, COIL_STEPPER_1_B, COIL_STEPPER_1_C, COIL_STEPPER_1_D);




void setup() {
  // put your setup code here, to run once:


  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(ROLL_ZERO_SENSE_IN, INPUT);



  digitalWrite(LED_BUILTIN, true);
  Serial.begin(115200);
  Serial.println("");
  Serial.println("Starting");
  delay(FLASH_TIME);
  digitalWrite(LED_BUILTIN, false);
  delay(FLASH_TIME);


  randomSeed(analogRead(0));


  STEPPER_1.setMaxSpeed(STEPPER_MAX_SPEED);
  STEPPER_1.setAcceleration(STEPPER_ACCELERATION);

  STEPPER_1.moveTo(300);
  while (STEPPER_1.distanceToGo() != 0) {
    STEPPER_1.run();
  }
  STEPPER_1.moveTo(0);
  while (STEPPER_1.distanceToGo() != 0) {
    STEPPER_1.run();
  }

  int randNumber = random(10, 200);
  STEPPER_1.moveTo(randNumber);
  while (STEPPER_1.distanceToGo() != 0) {
    STEPPER_1.run();
  }



  // Check to see if stepper already in rest position - if so move 20
  SENSOR_STATE = digitalRead(ROLL_ZERO_SENSE_IN);
  if (SENSOR_STATE == false) {
    STEPPER_1.move(20);
    while (STEPPER_1.distanceToGo() != 0) {
      STEPPER_1.run();
    }
  }


  // Find Zero Point - moving in positive direction
  STEPPER_1.moveTo(3000);
  while (STEPPER_1.distanceToGo() != 0) {
    SENSOR_STATE = digitalRead(ROLL_ZERO_SENSE_IN);
    if (SENSOR_STATE == false) {
      break;
    }
    STEPPER_1.run();
  }

  // Now we have Zero Point - move until interruptor has cleared
  SENSOR_STATE = digitalRead(ROLL_ZERO_SENSE_IN);
  int STEP_COUNTER = 0;
  while (SENSOR_STATE == false) {
    // Stepping until Sensor Reads True
    STEP_COUNTER++;
    STEPPER_1.move(1);
    while (STEPPER_1.isRunning()) {
      STEPPER_1.runToPosition();
    }
    SENSOR_STATE = digitalRead(ROLL_ZERO_SENSE_IN);
  }
  Serial.println("Total Steps until true : " + String(STEP_COUNTER));

  // Now Step Backwards until Sensor Reads False (ie interruptor is in place)
  SENSOR_STATE = digitalRead(ROLL_ZERO_SENSE_IN);
  STEP_COUNTER = 0;
  while (SENSOR_STATE == true) {
    // Stepping until Sensor Reads True
    STEP_COUNTER++;
    STEPPER_1.move(-1);
    while (STEPPER_1.isRunning()) {
      STEPPER_1.runToPosition();
    }
    SENSOR_STATE = digitalRead(ROLL_ZERO_SENSE_IN);
  }
  Serial.println("Total Steps until false : " + String(STEP_COUNTER));


  Serial.println("Determing Number of Steps in full revolution");
  STEPPER_1.setCurrentPosition(0);
  STEP_COUNTER = 0;
  // Move 180 Steps then idividual step until sensor reads zero
  STEPPER_1.moveTo(220);
  Serial.println("Distance to go is :" + String(STEPPER_1.distanceToGo()));
  bool READY_TO_BREAK = false;
  SENSOR_STATE = digitalRead(ROLL_ZERO_SENSE_IN);
  while ((READY_TO_BREAK == false) || (SENSOR_STATE == true)) {

    // If we sure than interruptor has cleared sensor then get ready to stop
    if (READY_TO_BREAK == false) {
      Serial.println("Distance to go is :" + String(STEPPER_1.distanceToGo()));
      Serial.println("Checking if ready to break");
      if (100 > STEPPER_1.distanceToGo()) {
        READY_TO_BREAK = true;
      }
    }

    if (STEPPER_1.runSpeed() == true)
      STEP_COUNTER++;
    SENSOR_STATE = digitalRead(ROLL_ZERO_SENSE_IN);
  }
  Serial.println("Total Steps in full Revolution : " + String(STEP_COUNTER));

  Serial.println("All Done");
  Serial.end();
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
