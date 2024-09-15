// SARI

// Currently don't understand why VSI must be defined and enabled - if its no the zero doesn't move on step 1
// Note - the cable supplied with the stepper must be relaid out
//        the center two pins need to be swapped
// A non polarised connector is used so the rotation direction can be swapped
// as the class doesn't allow things to be swapped.

#define Ethernet_In_Use 1
#define Reflector_In_Use 1
#define DCSBIOS_In_Use 1

#define DCSBIOS_IRQ_SERIAL
#include "DcsBios.h"


// ###################################### Begin Ethernet Related #############################
#include <SPI.h>
#include <Ethernet.h>
#include <EthernetUdp.h>

// These local Mac and IP Address will be reassigned early in startup based on
// the device ID as set by address pins
#define EthernetStartupDelay 500
#define ES1_RESET_PIN 53

byte mac[] = { 0xA8, 0x61, 0x0A, 0x9E, 0x83, 0x06 };
IPAddress ip(172, 16, 1, 106);
String strMyIP = "172.16.1.106";

// Reflector
IPAddress reflectorIP(172, 16, 1, 10);
String strReflectorIP = "X.X.X.X";


EthernetUDP udp;

const unsigned int localport = 7788;

const unsigned int remoteport = 49000;
const unsigned int reflectorport = 27000;


char packetBuffer[1000];     //buffer to store the incoming data
char outpacketBuffer[1000];  //buffer to store the outgoing data

const unsigned long delayBeforeSendingPacket = 2000;
unsigned long ethernetStartTime = 0;

String DebugString = "";


bool Debug_Display = false;
char* ParameterNamePtr;
char* ParameterValuePtr;


void SendDebug(String MessageToSend) {
  if ((Reflector_In_Use == 1) && (Ethernet_In_Use == 1)) {
    udp.beginPacket(reflectorIP, reflectorport);
    udp.print(MessageToSend);
    udp.endPacket();
  }
}
// ###################################### End Ethernet Related #############################

#define FLASH_TIME 300
#define Check_LED_R 12
#define Check_LED_G 13

bool RED_LED_STATE = false;
long NEXT_STATUS_TOGGLE_TIMER = 0;


// ###################################### Begin SARI ######################################



//----------ROLL SERVO----------
DcsBios::ServoOutput saiPitch(0x1028, 9, 2400, 544);

#include <AccelStepper.h>
// START VSI
#define VSIstepPin 46
#define VSIdirectionPin 48
AccelStepper VSIstepper(AccelStepper::DRIVER, VSIstepPin, VSIdirectionPin);

#define VSIstepPin 46
#define VSIdirectionPin 48
#define STEPS 315 * 16
#define STEPPER_MAX_SPEED 9000
#define STEPPER_ZERO_SEEK_SPEED 600
#define STEPPER_ACCELERATION 9000
#define AllstepperEnablePin 56



// END VSI


// SARI
const long zeroTimeout = 50000;


unsigned char zinitState;
long zSARIcurrentStepperPosition;
long zSARIlastAccelStepperPosition;
unsigned char zSARIirDetectorPin;
long zSARIzeroOffset;
bool zSARImovingForward;
bool zSARIlastZeroDetectState;
bool zSARI_ROLL_INITIALISED = false;
long zSARIzeroPosSearchStartTime = 0;

#define zSARImaxSteps 1600       // SARImaxSteps //200 Native steps with 1/16 MICRO STEPPING
#define zSARIacceleration 90000  // maxSpeed //3200
#define zmaxSpeed 60000          // SARIacceleration 3200
#define zSARIirDetectorPin 55
#define zSARIstepPin 30
#define zSARIdirectionPin 32
#define zSARIzeroOffset 800

// define AccelStepper instance
AccelStepper zSARIstepperRoll(AccelStepper::DRIVER, zSARIstepPin, zSARIdirectionPin);


bool zSARIzeroDetected() {
  return digitalRead(zSARIirDetectorPin) == 0;
}

long zSARInormalizeStepperPosition(long pos) {
  if (pos < 0) return pos + zSARImaxSteps;
  if (pos >= zSARImaxSteps) return pos - zSARImaxSteps;
  return pos;
}

void zupdateSARIcurrentStepperPosition() {
  // adjust SARIcurrentStepperPosition to include the distance our stepper motor
  // was moved since we last updated it
  long zSARImovementSinceLastUpdate = zSARIstepperRoll.currentPosition() - zSARIlastAccelStepperPosition;
  zSARIcurrentStepperPosition = zSARInormalizeStepperPosition(zSARIcurrentStepperPosition + zSARImovementSinceLastUpdate);
  zSARIlastAccelStepperPosition = zSARIstepperRoll.currentPosition();
}


void zInitSARIStepper() {

  pinMode(zSARIirDetectorPin, INPUT);


  zSARIstepperRoll.setMaxSpeed(4000);
  zSARIstepperRoll.setAcceleration(500);

  SendDebug("Do a quick loop");

  // Microstepping - 16 steps
  // 42HK40 1.8 degrees per step, so 200 steps per turn without microstepping
  // 3200 steps with microstepping
  zSARIstepperRoll.moveTo(-1600 * 10);
  while (zSARIstepperRoll.distanceToGo() != 0) {
    zSARIstepperRoll.run();
  }
  SendDebug("Quick loop complete");
  delay(1000);

  zSARIzeroPosSearchStartTime = millis();

  // move off zero if already there so we always get movement on reset
  // (to verify that the stepper is working)
  if (zSARIzeroDetected()) {
    SendDebug("SARI moving off zero sense");
    zSARIstepperRoll.move(-300);
    while (zSARIstepperRoll.distanceToGo() != 0) {
      zSARIstepperRoll.run();
    }
  }
  SendDebug("zSARI initState finding zero point");
  zSARIstepperRoll.setMaxSpeed(zmaxSpeed);
  zSARIstepperRoll.setAcceleration(zSARIacceleration);

  int zCurrentPos = 0;
  while (!zSARIzeroDetected()) {
    if (millis() >= (zeroTimeout + zSARIzeroPosSearchStartTime)) {
      SendDebug("zSARI Roll - timeoutout finding zero !!");
      break;
    }

    zCurrentPos = zSARIstepperRoll.currentPosition();
    zSARIstepperRoll.runToNewPosition(zCurrentPos + 1);
  }

  zSARIstepperRoll.setCurrentPosition(zSARIzeroOffset);
  zSARIlastAccelStepperPosition = 0;
}


void zSARIStepperLoop() {


  // recalibrate when passing through zero position
  bool zSARIcurrentZeroDetectState = zSARIzeroDetected();
  if (!zSARIlastZeroDetectState && zSARIcurrentZeroDetectState && zSARImovingForward) {
    // we have moved from left to right into the 'zero detect window'
    // and are now at position 0
    zSARIlastAccelStepperPosition = zSARIstepperRoll.currentPosition();
    zSARIcurrentStepperPosition = zSARInormalizeStepperPosition(zSARIzeroOffset);
  } else if (zSARIlastZeroDetectState && !zSARIcurrentZeroDetectState && !zSARImovingForward) {
    // we have moved from right to left out of the 'zero detect window'
    // and are now at position (SARImaxSteps-1)
    zSARIlastAccelStepperPosition = zSARIstepperRoll.currentPosition();
    zSARIcurrentStepperPosition = zSARInormalizeStepperPosition(zSARImaxSteps + zSARIzeroOffset);
  }
  zSARIlastZeroDetectState = zSARIcurrentZeroDetectState;

  zSARIstepperRoll.run();
}



void zsetSARIStepper(long targetPosition) {

  zupdateSARIcurrentStepperPosition();

  long delta = targetPosition - zSARIcurrentStepperPosition;
  // if we would move more than 180 degree counterclockwise, move clockwise instead

  if (delta < -((long)(zSARImaxSteps / 2))) delta += zSARImaxSteps;  //2
  // if we would move more than 180 degree clockwise, move counterclockwise instead
  if (delta > (zSARImaxSteps / 2)) delta -= (long)zSARImaxSteps;  //2

  zSARImovingForward = (delta >= 0);

  zSARIstepperRoll.move(delta);
  
  zSARIstepperRoll.run();
}

void onSaiBankChange(unsigned int newValue) {

  zsetSARIStepper(map(newValue, 0, 65535, 0, zSARImaxSteps - 1));
}
DcsBios::IntegerBuffer saiBankBuffer(0x102a, 0xffff, 0, onSaiBankChange);


// #################################################### END NEW CODE ##################################################
// //----------ROLL STEPPER----------
// long SARIzeroPosSearchStartTime = 0;
// struct SARIStepperConfig {
//   unsigned int SARImaxSteps;
//   unsigned int SARIacceleration;
//   unsigned int maxSpeed;
// };
// bool SARI_ROLL_INITIALISED = false;

// class Nema8Stepper : public DcsBios::Int16Buffer {
// private:

//   AccelStepper& stepper;
//   SARIStepperConfig& SARIstepperConfig;
//   inline bool SARIzeroDetected() {
//     return digitalRead(SARIirDetectorPin) == 0;
//   }
//   unsigned int (*map_function)(unsigned int);
//   unsigned char initState;
//   long SARIcurrentStepperPosition;
//   long SARIlastAccelStepperPosition;
//   unsigned char SARIirDetectorPin;
//   long SARIzeroOffset;
//   bool SARImovingForward;
//   bool SARIlastZeroDetectState;

//   long SARIzeroPosSearchStartTime = 0;

//   long SARInormalizeStepperPosition(long pos) {
//     if (pos < 0) return pos + SARIstepperConfig.SARImaxSteps;
//     if (pos >= SARIstepperConfig.SARImaxSteps) return pos - SARIstepperConfig.SARImaxSteps;
//     return pos;
//   }

//   void updateSARIcurrentStepperPosition() {
//     // adjust SARIcurrentStepperPosition to include the distance our stepper motor
//     // was moved since we last updated it
//     long SARImovementSinceLastUpdate = stepper.currentPosition() - SARIlastAccelStepperPosition;
//     SARIcurrentStepperPosition = SARInormalizeStepperPosition(SARIcurrentStepperPosition + SARImovementSinceLastUpdate);
//     SARIlastAccelStepperPosition = stepper.currentPosition();
//   }


// public:
//   Nema8Stepper(unsigned int address, AccelStepper& stepper, SARIStepperConfig& SARIstepperConfig, unsigned char SARIirDetectorPin, long SARIzeroOffset, unsigned int (*map_function)(unsigned int))
//     : Int16Buffer(address), stepper(stepper), SARIstepperConfig(SARIstepperConfig), SARIirDetectorPin(SARIirDetectorPin), SARIzeroOffset(SARIzeroOffset), map_function(map_function), initState(0), SARIcurrentStepperPosition(0), SARIlastAccelStepperPosition(0) {
//   }


//   virtual void loop() {
//     if (initState == 0) {  // not initialized yet
//       SendDebug("SARI initState: " + String(initState));
//       pinMode(SARIirDetectorPin, INPUT);
//       stepper.setMaxSpeed(SARIstepperConfig.maxSpeed);
//       stepper.setAcceleration(SARIstepperConfig.SARIacceleration);

//       stepper.setMaxSpeed(4000);
//       stepper.setAcceleration(500);

//       initState = 1;
//       SendDebug("Do a quick loop");

//       // Microstepping - 16 steps
//       // 42HK40 1.8 degrees per step, so 200 steps per turn without microstepping
//       // 3200 steps with microstepping
//       stepper.moveTo(-1600 * 10);
//       while (stepper.distanceToGo() != 0) {
//         stepper.run();
//       }
//       SendDebug("Quick loop complete");
//       delay(1000);

//       SendDebug("SARI initState moving to State 1");
//       SARIzeroPosSearchStartTime = millis();
//     }

//     if (initState == 1) {
//       // move off zero if already there so we always get movement on reset
//       // (to verify that the stepper is working)
//       if (SARIzeroDetected()) {
//         SendDebug("SARI moving off zero sense");
//         stepper.move(-300);
//         while (stepper.distanceToGo() != 0) {
//           stepper.run();
//         }

//         stepper.runSpeed();
//       } else {
//         initState = 2;
//         SendDebug("SARI initState moving to State 2");
//         stepper.setMaxSpeed(SARIstepperConfig.maxSpeed);
//         stepper.setAcceleration(SARIstepperConfig.SARIacceleration);
//       }
//     }

//     if (initState == 2) {  // zeroing
//       if (!SARIzeroDetected()) {
//         if (millis() >= (zeroTimeout + SARIzeroPosSearchStartTime)) {
//           SendDebug("SARI Roll - timeoutout finding zero, disabling driver pin");

//           initState = 99;
//         }

//         //SendDebug("SARI Roll - looping - " + String(initState));
//         stepper.moveTo(stepper.currentPosition() - 1);
//         stepper.run();


//       } else {
//         stepper.setAcceleration(SARIstepperConfig.SARIacceleration);
//         stepper.runToNewPosition(stepper.currentPosition());
//         // tell the AccelStepper library that we are at position zero
//         stepper.setCurrentPosition(SARIzeroOffset);
//         SARIlastAccelStepperPosition = 0;
//         // set stepper SARIacceleration in steps per second per second
//         // (default is zero)
//         stepper.setAcceleration(SARIstepperConfig.SARIacceleration);

//         SARIlastZeroDetectState = true;
//         initState = 3;
//         SendDebug("SARI initState moving to State 3");
//         SARI_ROLL_INITIALISED = true;

//         ;
//       }
//     }


//     if (initState == 99) {  // Timed out looking for zero do nothing
//     }

//     //    digitalWrite(enablePin, HIGH);
//     if (initState == 3) {  // running normally

//       // recalibrate when passing through zero position
//       bool SARIcurrentZeroDetectState = SARIzeroDetected();
//       if (!SARIlastZeroDetectState && SARIcurrentZeroDetectState && SARImovingForward) {
//         // we have moved from left to right into the 'zero detect window'
//         // and are now at position 0
//         SARIlastAccelStepperPosition = stepper.currentPosition();
//         SARIcurrentStepperPosition = SARInormalizeStepperPosition(SARIzeroOffset);
//       } else if (SARIlastZeroDetectState && !SARIcurrentZeroDetectState && !SARImovingForward) {
//         // we have moved from right to left out of the 'zero detect window'
//         // and are now at position (SARImaxSteps-1)
//         SARIlastAccelStepperPosition = stepper.currentPosition();
//         SARIcurrentStepperPosition = SARInormalizeStepperPosition(SARIstepperConfig.SARImaxSteps + SARIzeroOffset);
//       }
//       SARIlastZeroDetectState = SARIcurrentZeroDetectState;


//       if (hasUpdatedData()) {
//         // convert data from DCS to a target position expressed as a number of steps
//         long targetPosition = (long)map_function(getData());

//         updateSARIcurrentStepperPosition();

//         long delta = targetPosition - SARIcurrentStepperPosition;

//         // if we would move more than 180 degree counterclockwise, move clockwise instead

//         if (delta < -((long)(SARIstepperConfig.SARImaxSteps / 2))) delta += SARIstepperConfig.SARImaxSteps;  //2
//         // if we would move more than 180 degree clockwise, move counterclockwise instead
//         if (delta > (SARIstepperConfig.SARImaxSteps / 2)) delta -= (long)SARIstepperConfig.SARImaxSteps;  //2

//         SARImovingForward = (delta >= 0);

//         // tell AccelStepper to move relative to the current position
//         stepper.move(delta);
//       }
//       stepper.run();
//     }
//   }
// };

// struct SARIStepperConfig SARIstepperConfig = {
//   1600,   // SARImaxSteps //200 Native steps with 1/16 MICRO STEPPING
//   90000,  // maxSpeed //3200
//   60000   // SARIacceleration 3200
// };
// const int SARIstepPin = 30;
// const int SARIdirectionPin = 32;

// // define AccelStepper instance
// AccelStepper SARIstepperRoll(AccelStepper::DRIVER, SARIstepPin, SARIdirectionPin);

// // Hornet Address - 0x74e6
// // A10 Address - 0x102a
// Nema8Stepper SARIRoll(0x102a,             // address of stepper data
//                       SARIstepperRoll,    // name of AccelStepper instance
//                       SARIstepperConfig,  // SARIStepperConfig struct instance
//                       55,                 // IR Detector Pin (must be LOW in zero position)
//                       800,                // zero offset (SET TO 50% of MaX Steps) 1650 was 800
//                                           // WIngs Level = 1/2 Max Steps -> "Zero" = 1/2 Turn
//                                           // SARI will be upsidedown after Setup, this will correct in Bios
//                       [](unsigned int newValue) -> unsigned int {
//                         newValue = newValue & 0xffff;
//                         return map(newValue, 0, 65535, 0, SARIstepperConfig.SARImaxSteps - 1);
//                       });

// // ######################################  End SARI  ######################################






void setup() {

  pinMode(Check_LED_G, OUTPUT);
  digitalWrite(Check_LED_G, true);
  pinMode(Check_LED_R, OUTPUT);
  digitalWrite(Check_LED_R, true);


  if (Ethernet_In_Use == 1) {

    // Reset Ethernet Module
    pinMode(ES1_RESET_PIN, OUTPUT);
    digitalWrite(ES1_RESET_PIN, LOW);
    delay(2);
    digitalWrite(ES1_RESET_PIN, HIGH);

    Ethernet.begin(mac, ip);
    udp.begin(localport);

    // As it takes a couple of seconds before the Ethernet Stack is operational
    // Flash leds until time period has completed
    ethernetStartTime = millis() + delayBeforeSendingPacket;
    while (millis() <= ethernetStartTime) {
      delay(FLASH_TIME);
      digitalWrite(Check_LED_G, false);
      delay(FLASH_TIME);
      digitalWrite(Check_LED_G, true);
    }

    SendDebug("Ethernet Started");
    SendDebug("A10_CLASSLESS_SARI_DEV 20240915");
  }


  delay(FLASH_TIME);
  digitalWrite(Check_LED_G, false);
  delay(FLASH_TIME);

  SendDebug("Stepper Initialisation Started");

  pinMode(AllstepperEnablePin, OUTPUT);
  digitalWrite(AllstepperEnablePin, false);

  SendDebug("zSARI Stepper Initialisation Started");
  zInitSARIStepper();

  SendDebug("Stepper Initialisation Complete");

  SendDebug("DCS BIOS Setup Started");
  DcsBios::setup();
  SendDebug("DCS BIOS Setup Complete");
}






void loop() {

  if (millis() >= NEXT_STATUS_TOGGLE_TIMER) {
    RED_LED_STATE = !RED_LED_STATE;
    digitalWrite(Check_LED_G, RED_LED_STATE);
    digitalWrite(Check_LED_R, !RED_LED_STATE);
    NEXT_STATUS_TOGGLE_TIMER = millis() + FLASH_TIME;
  }

  DcsBios::loop();
  zSARIStepperLoop();
}
