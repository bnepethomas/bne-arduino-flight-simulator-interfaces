// Source
// https://gist.github.com/jboecker/1084b3768c735b164c34d6087d537c18


// the Warthog Project Video on the compass build
// https://www.youtube.com/watch?v=ZN9glqgp9TY&t=332s


#define DCSBIOS_IRQ_SERIAL

#include <AccelStepper.h>
#include "DcsBios.h"


unsigned long nextupdate = 0;
bool outputstate;
int flashinterval = 1000;
#define LedMonitorPin 5

struct StepperConfig {
  unsigned int maxSteps;
  unsigned int acceleration;
  unsigned int maxSpeed;
};

const long zeroTimeout = 30000;

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

    long zeroPosSearchStartTime = 0;

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
        zeroPosSearchStartTime = millis();
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
          // Currently this safety check isn't working
          // Add Ethernet card for more troubleshooting
          // Need to check IP addresses of PC secondary nic
          if (millis() >= (zeroTimeout + zeroPosSearchStartTime)) {
            stepper.disableOutputs();
            initState == 99;
          }
          else
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

      if (initState == 99) { // Timed out looking for zero do nothing
        stepper.disableOutputs();
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
AccelStepper stepper(AccelStepper::FULL4WIRE, 2, 11, 3, 12);

// define Vid60Stepper class that uses the AccelStepper instance defined in the line above
//           v-- arbitrary name
// Vid60Stepper alt100ftPointer(0x107e,          // address of stepper data
Vid60Stepper standbyCompass(0x0436,          // address of stepper data
                            stepper,         // name of AccelStepper instance
                            stepperConfig,   // StepperConfig struct instance
                            9,              // IR Detector Pin (must be LOW in zero position)
                            0,               // zero offset
[](unsigned int newValue) -> unsigned int {
  /* this function needs to map newValue to the correct number of steps */

  // For most guages this map will do
  //return map(newValue, 0, 65535, 0, stepperConfig.maxSteps - 1);

  // For the compass we only has 360 degrees and need to exclude upper part
  // of 16 bit value
  //Output Type: integer Address: 0x0436 Mask: 0x01ff Shift By: 0 Max. Value: 360 Description: Heading (Degrees)
  // so instead of 0 to 65000 its 0 to 360. Need to exclude upper part of 16 bit value
  newValue = newValue & 0x01ff;
  return map(newValue, 0, 360, 0, stepperConfig.maxSteps - 1);
});


void setup() {
  DcsBios::setup();
  pinMode(13, OUTPUT);
  pinMode(LedMonitorPin, OUTPUT);
  outputstate = true;
  digitalWrite(LedMonitorPin,outputstate);

  nextupdate = millis() + flashinterval;
}

void loop() {
  //  PORTB |= (1 << 5);
  //  PORTB &= ~(1 << 5);
  DcsBios::loop();

    if (millis() >= nextupdate) {
    outputstate = !outputstate;
    digitalWrite(LedMonitorPin, outputstate);
    nextupdate = millis() + flashinterval;
  }
}
