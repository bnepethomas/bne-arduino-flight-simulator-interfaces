// SARI

// Currently don't understand why VSI must be defined and enabled - if its no the zero doesn't move on step 1


#define Ethernet_In_Use 1
#define Reflector_In_Use 1
#define DCSBIOS_In_Use 1
#define MSFS_In_Use 0  // Used to interface into MSFS - set to 0 if not in use

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
const unsigned int max7219port = 7788;
const unsigned int remoteport = 49000;
const unsigned int reflectorport = 27000;
const unsigned int MSFSport = 13136;

char packetBuffer[1000];     //buffer to store the incoming data
char outpacketBuffer[1000];  //buffer to store the outgoing data

const unsigned long delayBeforeSendingPacket = 2000;
unsigned long ethernetStartTime = 0;

String DebugString = "";

// Packet Length
int max7219packetsize;
int max7219Len;

int MSFSpacketsize;
int MSFSLen;

EthernetUDP max7219udp;  // Max7219
EthernetUDP MSFSudp;     // Listens to MSFS light commands

char max7219packetBuffer[1000];  //buffer to store packet data for both Max7219 and MSFS data


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
bool RED_LED_STATE = false;
long NEXT_STATUS_TOGGLE_TIMER = 0;

#include <AccelStepper.h>

// ###################################### Begin SARI ######################################


#define Check_LED_R 12
#define Check_LED_G 13


Servo SARIServo;



int DCS_On = 0;
int previous_DCS_State = 0;
int DCS_State = 0;


long dcsMillis;

// SARI

long timeToDisable_SARI_ROLL = 0;
bool waitingToDisable_SARI_ROLL = false;
bool SARI_ROLL_ENABLED = false;
#define disable_SARI_ROLL_WaitTime 300  // mS delay after SARI ROll has founds its position \
                                        // Used to help hold the SARI in position

struct SARIStepperConfig {
  unsigned int SARImaxSteps;
  unsigned int SARIacceleration;
  unsigned int maxSpeed;
};
bool SARI_ROLL_INITIALISED = false;



#define VSIstepPin 46
#define VSIdirectionPin 48
// #define VSIstepPin 30
// #define VSIdirectionPin 32
AccelStepper VSIstepper(AccelStepper::DRIVER, VSIstepPin, VSIdirectionPin);


//////SARI - TEST - BEN --------------------------------------------------------------------------------------------------------------
//----------ROLL SERVO----------
//DcsBios::ServoOutput saiPitch(0x74e4, 9, 2400, 544);
DcsBios::ServoOutput saiPitch(0x1028, 9, 2400, 544);

//----------ROLL STEPPER----------

const long zeroTimeout = 50000;
const int SARIenablePin = 56;


class Nema8Stepper : public DcsBios::Int16Buffer {
private:

  AccelStepper& stepper;
  SARIStepperConfig& SARIstepperConfig;
  inline bool SARIzeroDetected() {
    return digitalRead(SARIirDetectorPin) == 0;
  }
  unsigned int (*map_function)(unsigned int);
  unsigned char initState;
  long SARIcurrentStepperPosition;
  long SARIlastAccelStepperPosition;
  unsigned char SARIirDetectorPin;
  long SARIzeroOffset;
  bool SARImovingForward;
  bool SARIlastZeroDetectState;

  long SARIzeroPosSearchStartTime = 0;

  long SARInormalizeStepperPosition(long pos) {
    if (pos < 0) return pos + SARIstepperConfig.SARImaxSteps;
    if (pos >= SARIstepperConfig.SARImaxSteps) return pos - SARIstepperConfig.SARImaxSteps;
    return pos;
  }

  void updateSARIcurrentStepperPosition() {
    // adjust SARIcurrentStepperPosition to include the distance our stepper motor
    // was moved since we last updated it
    long SARImovementSinceLastUpdate = stepper.currentPosition() - SARIlastAccelStepperPosition;
    SARIcurrentStepperPosition = SARInormalizeStepperPosition(SARIcurrentStepperPosition + SARImovementSinceLastUpdate);
    SARIlastAccelStepperPosition = stepper.currentPosition();
  }


public:
  Nema8Stepper(unsigned int address, AccelStepper& stepper, SARIStepperConfig& SARIstepperConfig, unsigned char SARIirDetectorPin, long SARIzeroOffset, unsigned int (*map_function)(unsigned int))
    : Int16Buffer(address), stepper(stepper), SARIstepperConfig(SARIstepperConfig), SARIirDetectorPin(SARIirDetectorPin), SARIzeroOffset(SARIzeroOffset), map_function(map_function), initState(0), SARIcurrentStepperPosition(0), SARIlastAccelStepperPosition(0) {
  }


  virtual void loop() {
    if (initState == 0) {  // not initialized yet
      SendDebug("SARI initState: " + String(initState));
      pinMode(SARIirDetectorPin, INPUT);
      stepper.setMaxSpeed(SARIstepperConfig.maxSpeed);
      stepper.setAcceleration(SARIstepperConfig.SARIacceleration);

      stepper.setMaxSpeed(4000);
      stepper.setAcceleration(500);

      initState = 1;
      SendDebug("Do a quick loop");

      // Microstepping - 16 steps
      // 42HK40 1.8 degrees per step, so 200 steps per turn without microstepping
      // 3200 steps with microstepping
      stepper.moveTo(-1600 * 10);
      while (stepper.distanceToGo() != 0) {
        stepper.run();
      }
      SendDebug("Quick loop complete");
      delay(1000);

      SendDebug("SARI initState moving to State 1");
      SARIzeroPosSearchStartTime = millis();
    }

    if (initState == 1) {
      // move off zero if already there so we always get movement on reset
      // (to verify that the stepper is working)
      if (SARIzeroDetected()) {
        SendDebug("SARI moving off zero sense");
        stepper.move(-300);
        while (stepper.distanceToGo() != 0) {
          stepper.run();
        }

        stepper.runSpeed();
      } else {
        initState = 2;
        SendDebug("SARI initState moving to State 2");
        stepper.setMaxSpeed(SARIstepperConfig.maxSpeed);
        stepper.setAcceleration(SARIstepperConfig.SARIacceleration);
      }
    }

    if (initState == 2) {  // zeroing



      if (!SARIzeroDetected()) {

        if (millis() >= (zeroTimeout + SARIzeroPosSearchStartTime)) {
          SendDebug("SARI Roll - timeoutout finding zero, disabling driver pin");

          initState = 99;
        }

        //SendDebug("SARI Roll - looping - " + String(initState));
        stepper.moveTo(stepper.currentPosition() - 1);
        stepper.run();


      } else {
        stepper.setAcceleration(SARIstepperConfig.SARIacceleration);
        stepper.runToNewPosition(stepper.currentPosition());
        // tell the AccelStepper library that we are at position zero
        stepper.setCurrentPosition(SARIzeroOffset);
        SARIlastAccelStepperPosition = 0;
        // set stepper SARIacceleration in steps per second per second
        // (default is zero)
        stepper.setAcceleration(SARIstepperConfig.SARIacceleration);

        SARIlastZeroDetectState = true;
        initState = 3;
        SendDebug("SARI initState moving to State 3");
        SARI_ROLL_INITIALISED = true;

        ;
      }
    }


    if (initState == 99) {  // Timed out looking for zero do nothing
    }

    //    digitalWrite(enablePin, HIGH);
    if (initState == 3) {  // running normally

      // recalibrate when passing through zero position
      bool SARIcurrentZeroDetectState = SARIzeroDetected();
      if (!SARIlastZeroDetectState && SARIcurrentZeroDetectState && SARImovingForward) {
        // we have moved from left to right into the 'zero detect window'
        // and are now at position 0
        SARIlastAccelStepperPosition = stepper.currentPosition();
        SARIcurrentStepperPosition = SARInormalizeStepperPosition(SARIzeroOffset);
      } else if (SARIlastZeroDetectState && !SARIcurrentZeroDetectState && !SARImovingForward) {
        // we have moved from right to left out of the 'zero detect window'
        // and are now at position (SARImaxSteps-1)
        SARIlastAccelStepperPosition = stepper.currentPosition();
        SARIcurrentStepperPosition = SARInormalizeStepperPosition(SARIstepperConfig.SARImaxSteps + SARIzeroOffset);
      }
      SARIlastZeroDetectState = SARIcurrentZeroDetectState;


      if (hasUpdatedData()) {
        // convert data from DCS to a target position expressed as a number of steps
        long targetPosition = (long)map_function(getData());

        updateSARIcurrentStepperPosition();

        long delta = targetPosition - SARIcurrentStepperPosition;

        // if we would move more than 180 degree counterclockwise, move clockwise instead

        if (delta < -((long)(SARIstepperConfig.SARImaxSteps / 2))) delta += SARIstepperConfig.SARImaxSteps;  //2
        // if we would move more than 180 degree clockwise, move counterclockwise instead
        if (delta > (SARIstepperConfig.SARImaxSteps / 2)) delta -= (long)SARIstepperConfig.SARImaxSteps;  //2

        SARImovingForward = (delta >= 0);





        // tell AccelStepper to move relative to the current position
        stepper.move(delta);
      }
      stepper.run();
      if ((stepper.distanceToGo() == 0) && (waitingToDisable_SARI_ROLL == false) && (SARI_ROLL_ENABLED == true)) {
        // SendDebug("Starting Count down to disable SARI ROLL");
        waitingToDisable_SARI_ROLL = true;
        timeToDisable_SARI_ROLL = millis() + disable_SARI_ROLL_WaitTime;
      }
    }
  }
};

struct SARIStepperConfig SARIstepperConfig = {
  1600,   // SARImaxSteps //200 Native steps with 1/16 MICRO STEPPING
  90000,  // maxSpeed //3200
  60000   // SARIacceleration 3200
};
const int SARIstepPin = 30;
const int SARIdirectionPin = 32;

// define AccelStepper instance
AccelStepper SARIstepperRoll(AccelStepper::DRIVER, SARIstepPin, SARIdirectionPin);

// Hornet Address - 0x74e6
// A10 Address - 0x102a
Nema8Stepper SARIRoll(0x102a,             // address of stepper data
                      SARIstepperRoll,    // name of AccelStepper instance
                      SARIstepperConfig,  // SARIStepperConfig struct instance
                      55,                 // IR Detector Pin (must be LOW in zero position)
                      800,                // zero offset (SET TO 50% of MaX Steps) 1650 was 800
                                          // WIngs Level = 1/2 Max Steps -> "Zero" = 1/2 Turn
                                          // SARI will be upsidedown after Setup, this will correct in Bios
                      [](unsigned int newValue) -> unsigned int {
                        newValue = newValue & 0xffff;
                        return map(newValue, 0, 65535, 0, SARIstepperConfig.SARImaxSteps - 1);
                      });





// ######################################  End SARI  ######################################






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
    SendDebug("A10_SARI_DEV 20240914");
  }


  delay(FLASH_TIME);
  digitalWrite(Check_LED_G, false);
  delay(FLASH_TIME);

  SendDebug("Stepper Initialisation Started");
#define VSIstepPin 46
#define VSIdirectionPin 48
#define STEPS 315 * 16
#define STEPPER_MAX_SPEED 9000
#define STEPPER_ZERO_SEEK_SPEED 600
#define STEPPER_ACCELERATION 9000
#define AllstepperEnablePin 56

  pinMode(AllstepperEnablePin, OUTPUT);
  digitalWrite(AllstepperEnablePin, false);


  VSIstepper.setMaxSpeed(STEPPER_MAX_SPEED);
  VSIstepper.setAcceleration(STEPPER_ACCELERATION);
  // ################# Start VSI Startup #########################
  SendDebug("Start VSI");

  VSIstepper.runToNewPosition(-STEPS * 1.1);
  VSIstepper.setCurrentPosition(0);

  for (int i = 1; i <= 1; i++) {
    SendDebug("Loop :" + String(i));
    VSIstepper.runToNewPosition(STEPS);
    delay(200);
    VSIstepper.runToNewPosition(0);
    delay(200);
  }

  // Move VSI to zero position and set
  VSIstepper.runToNewPosition((STEPS / 2));
  VSIstepper.setCurrentPosition(0);
  SendDebug("End VSI");
  // ################# End VSI Startup #########################

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
}
