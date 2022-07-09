// Source
// https://gist.github.com/jboecker/1084b3768c735b164c34d6087d537c18


// the Warthog Project Video on the compass build
// https://www.youtube.com/watch?v=ZN9glqgp9TY&t=332s


#define DCSBIOS_IRQ_SERIAL

#include <AccelStepper.h>
#include "DcsBios.h"

struct StepperConfig {
  unsigned int maxSteps;
  unsigned int acceleration;
  unsigned int maxSpeed;
};


class Vid60Stepper : public DcsBios::Int16Buffer {
  private:
    AccelStepper& stepper;
    StepperConfig& stepperConfig;
    inline bool zeroDetected() {
      return digitalRead(irDetectorPin) == 0;
    }
    unsigned int (*map_function)(unsigned int);
    unsigned char initState;
    long currentStepperPosition;
    long lastAccelStepperPosition;
    unsigned char irDetectorPin;
    long zeroOffset;
    bool movingForward;
    bool lastZeroDetectState;

    long normalizeStepperPosition(long pos) {
      if (pos < 0) return pos + stepperConfig.maxSteps;
      if (pos >= stepperConfig.maxSteps) return pos - stepperConfig.maxSteps;
      return pos;
    }

    void updateCurrentStepperPosition() {
      // adjust currentStepperPosition to include the distance our stepper motor
      // was moved since we last updated it
      long movementSinceLastUpdate = stepper.currentPosition() - lastAccelStepperPosition;
      currentStepperPosition = normalizeStepperPosition(currentStepperPosition + movementSinceLastUpdate);
      lastAccelStepperPosition = stepper.currentPosition();
    }
  public:
    Vid60Stepper(unsigned int address, AccelStepper& stepper, StepperConfig& stepperConfig, unsigned char irDetectorPin, long zeroOffset, unsigned int (*map_function)(unsigned int))
      : Int16Buffer(address), stepper(stepper), stepperConfig(stepperConfig), irDetectorPin(irDetectorPin), zeroOffset(zeroOffset), map_function(map_function), initState(0), currentStepperPosition(0), lastAccelStepperPosition(0) {
    }

    virtual void loop() {
      if (initState == 0) { // not initialized yet
        pinMode(irDetectorPin, INPUT);
        stepper.setMaxSpeed(stepperConfig.maxSpeed);
        stepper.setSpeed(400);

        initState = 1;
      }
      if (initState == 1) {
        // move off zero if already there so we always get movement on reset
        // (to verify that the stepper is working)
        if (zeroDetected()) {
          stepper.runSpeed();
        } else {
          initState = 2;
        }
      }
      if (initState == 2) { // zeroing
        if (!zeroDetected()) {
          stepper.runSpeed();
        } else {
          stepper.setAcceleration(stepperConfig.acceleration);
          stepper.runToNewPosition(stepper.currentPosition() + zeroOffset);
          // tell the AccelStepper library that we are at position zero
          stepper.setCurrentPosition(0);
          lastAccelStepperPosition = 0;
          // set stepper acceleration in steps per second per second
          // (default is zero)
          stepper.setAcceleration(stepperConfig.acceleration);

          lastZeroDetectState = true;
          initState = 3;
        }
      }
      if (initState == 3) { // running normally

        // recalibrate when passing through zero position
        bool currentZeroDetectState = zeroDetected();
        if (!lastZeroDetectState && currentZeroDetectState && movingForward) {
          // we have moved from left to right into the 'zero detect window'
          // and are now at position 0
          lastAccelStepperPosition = stepper.currentPosition();
          currentStepperPosition = normalizeStepperPosition(zeroOffset);
        } else if (lastZeroDetectState && !currentZeroDetectState && !movingForward) {
          // we have moved from right to left out of the 'zero detect window'
          // and are now at position (maxSteps-1)
          lastAccelStepperPosition = stepper.currentPosition();
          currentStepperPosition = normalizeStepperPosition(stepperConfig.maxSteps + zeroOffset);
        }
        lastZeroDetectState = currentZeroDetectState;


        if (hasUpdatedData()) {
          // convert data from DCS to a target position expressed as a number of steps
          long targetPosition = (long)map_function(getData());

          updateCurrentStepperPosition();

          long delta = targetPosition - currentStepperPosition;

          // if we would move more than 180 degree counterclockwise, move clockwise instead
          if (delta < -((long)(stepperConfig.maxSteps / 2))) delta += stepperConfig.maxSteps;
          // if we would move more than 180 degree clockwise, move counterclockwise instead
          if (delta > (stepperConfig.maxSteps / 2)) delta -= (long)stepperConfig.maxSteps;

          movingForward = (delta >= 0);

          // tell AccelStepper to move relative to the current position
          stepper.move(delta);

        }
        stepper.run();
      }
    }
};

/* modify below this line */

/* define stepper parameters
   multiple Vid60Stepper instances can share the same StepperConfig object */
struct StepperConfig stepperConfig = {
  720,  // maxSteps
  200, // maxSpeed
  50 // acceleration
};


// define AccelStepper instance
//AccelStepper stepper(AccelStepper::FULL4WIRE, 8, 9, 10, 11);
AccelStepper stepper(AccelStepper::FULL4WIRE, 10, 11, 8, 9);
// define Vid60Stepper class that uses the AccelStepper instance defined in the line above
//           v-- arbitrary name
// Vid60Stepper alt100ftPointer(0x107e,          // address of stepper data
Vid60Stepper alt100ftPointer(0x7460,          // address of stepper data
                             stepper,         // name of AccelStepper instance
                             stepperConfig,   // StepperConfig struct instance
                             12,              // IR Detector Pin (must be HIGH in zero position)
                             0,               // zero offset
[](unsigned int newValue) -> unsigned int {
  /* this function needs to map newValue to the correct number of steps */
  return map(newValue, 0, 65535, 0, stepperConfig.maxSteps - 1);
});


void setup() {
  DcsBios::setup();
  pinMode(13, OUTPUT);
}

void loop() {
//  PORTB |= (1 << 5);
//  PORTB &= ~(1 << 5);
  DcsBios::loop();
}
