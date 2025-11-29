// Source
// https://gist.github.com/jboecker/1084b3768c735b164c34d6087d537c18

// For reasons Im yet to work out when including
// the encoder for dcsbios I'm getting the folowing when uploading to a mega
// avrdude: verification error; content mismatch
// problem disappeared when altimeter removed and after reboot root cause tba


// the Warthog Project Video on the compass build
// https://www.youtube.com/watch?v=ZN9glqgp9TY&t=332s

// Zero Offsets for steppers



#define Ethernet_In_Use 1
#define DCSBIOS_In_Use 0
#define Reflector_In_Use 1


#define DCSBIOS_IRQ_SERIAL
#include "DcsBios.h"

// Ethernet Related
#include <SPI.h>
#include <Ethernet.h>
#include <EthernetUdp.h>

// I2C
#include <Wire.h>

extern "C" {
#include "utility/twi.h"  // from Wire library, so we can do bus scanning
}




#define ES1_RESET_PIN 53
// These local Mac and IP Address will be reassigned early in startup based on
// the device ID as set by address pins
byte mac[] = { 0xA8, 0x61, 0x0A, 0x9E, 0x83, 0x71 };
IPAddress ip(172, 16, 1, 114);
String strMyIP = "172.16.1.114";

// Raspberry Pi is Target
IPAddress reflectorIP(172, 16, 1, 10);
String strreflectorIP = "X.X.X.X";

// Arduino Mega holding Max7219
IPAddress max7219IP(172, 16, 1, 106);
String strMax7219IP = "172.16.1.106";



const unsigned int localport = 7788;
const unsigned int remoteport = 26027;
const unsigned int reflectorport = 27000;
const unsigned int max7219port = 7788;

EthernetUDP udp;
char packetBuffer[1000];     //buffer to store the incoming data
char outpacketBuffer[1000];  //buffer to store the outgoing data


String DebugString = "";


unsigned long nextupdate = 0;
bool outputstate;
int flashinterval = 1000;
#define RedLedMonitorPin 5
#define GreenLedMonitorPin 13


#include <AccelStepper.h>
#include <Stepper.h>
#define STEPS 200  // steps per revolution (limited to 315°)


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


void onUpdateCounterChange(unsigned int newValue) {
  DCS_State = newValue;
}
DcsBios::IntegerBuffer UpdateCounterBuffer(0xfffe, 0x00ff, 0, onUpdateCounterChange);


//////SARI - TEST - BEN --------------------------------------------------------------------------------------------------------------
//----------ROLL SERVO----------
DcsBios::ServoOutput saiPitch(0x74e4, 7, 2400, 544);

//----------ROLL STEPPER----------

const long zeroTimeout = 50000;
const int SARIenablePin = 49;


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

      stepper.setMaxSpeed(200);
      stepper.setAcceleration(100);

      initState = 1;
      SendDebug("Do a quick loop");
      stepper.moveTo(600);
      stepper.runToPosition();


      SendDebug("SARI initState moving to State 1");
      SARIzeroPosSearchStartTime = millis();
    }

    if (initState == 1) {
      // move off zero if already there so we always get movement on reset
      // (to verify that the stepper is working)
      if (SARIzeroDetected()) {
        stepper.moveTo(60);
        stepper.runToPosition();
      } else {
        initState = 2;
        SendDebug("SARI initState moving to State 2");
      }
    }

    if (initState == 2) {  // zeroing



      if (!SARIzeroDetected()) {

        if (millis() >= (zeroTimeout + SARIzeroPosSearchStartTime)) {
          SendDebug("SARI Roll - timeoutout finding zero, disabling driver pin");

          initState = 99;
        }


        SendDebug("SARI Roll - looping - " + String(initState));
        stepper.move(-1);
        stepper.runToPosition();
        while (!SARIzeroDetected()) {
          //SendDebug("SARI Roll - looping - " + String(initState));
          stepper.move(1);
          stepper.runToPosition();
        }

        stepper.move(-10);
        stepper.runToPosition();
        while (!SARIzeroDetected()) {
          //SendDebug("SARI Roll - looping - " + String(initState));
          stepper.move(-1);
          stepper.runToPosition();
        }



      } else {

        stepper.runToNewPosition(stepper.currentPosition());
        // tell the AccelStepper library that we are at position zero
        stepper.setCurrentPosition(SARIzeroOffset);
        SARIlastAccelStepperPosition = 0;



        SARIlastZeroDetectState = true;
        initState = 3;
        SendDebug("SARI initState moving to State 3");
        SARI_ROLL_INITIALISED = true;
        // disable_SARI_ROLL();
        ;
      }
    }


    if (initState == 99) {  // Timed out looking for zero do nothing
      // disable_SARI_ROLL();
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

        // Turn off Stepper when not in use
        if (delta != 0) {
          // enable_SARI_ROLL();
        }  // LOW  = stepper ON drive current available




        // tell AccelStepper to move relative to the current position
        stepper.move(delta);
      }
      //SendDebug("Noise");
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
  200,  // SARImaxSteps //200STEPS X 1/8 MICRO STEPPING
  200,  // maxSpeed //3200
  100   // SARIacceleration 3200
};


// define AccelStepper instance

AccelStepper SARIstepperRoll(AccelStepper::FULL4WIRE, 5, 4, 7, 6);

Nema8Stepper SARIRoll(0x74e6,             // address of stepper data
                      SARIstepperRoll,    // name of AccelStepper instance
                      SARIstepperConfig,  // SARIStepperConfig struct instance
                      8,                  // IR Detector Pin (must be LOW in zero position)
                      0,                  // zero offset (SET TO 50% of MaX Steps) 1650
                                          // WIngs Level = 1/2 Max Steps -> "Zero" = 1/2 Turn
                                          // SARI will be upsidedown after Setup, this will correct in Bios
                      [](unsigned int newValue) -> unsigned int {
                        newValue = newValue & 0xffff;
                        return map(newValue, 0, 65535, 0, SARIstepperConfig.SARImaxSteps - 1);
                      });


void SendDebug(String MessageToSend) {
  if ((Reflector_In_Use == 1) && (Ethernet_In_Use == 1)) {
    udp.beginPacket(reflectorIP, reflectorport);
    udp.println(MessageToSend);
    udp.endPacket();
  }
}



void setup() {

  //pinMode(Check_LED_G, OUTPUT);
  // pinMode(Check_LED_R, OUTPUT);


  pinMode(RedLedMonitorPin, OUTPUT);
  pinMode(GreenLedMonitorPin, OUTPUT);
  outputstate = true;
  digitalWrite(RedLedMonitorPin, outputstate);
  digitalWrite(GreenLedMonitorPin, outputstate);



  if (Ethernet_In_Use == 1) {

    // Using manual reset instead of tying to Arduino Reset
    pinMode(ES1_RESET_PIN, OUTPUT);
    digitalWrite(ES1_RESET_PIN, LOW);
    delay(2);
    digitalWrite(ES1_RESET_PIN, HIGH);

    Ethernet.begin(mac, ip);


    udp.begin(localport);
    if (Reflector_In_Use == 1) {
      udp.beginPacket(reflectorIP, reflectorport);
      udp.println("Init I2C Test rig - " + strMyIP + " " + String(millis()) + "mS since reset.");
      udp.endPacket();
    }
  }










  nextupdate = millis() + flashinterval;

  DcsBios::setup();
}




void loop() {


  DcsBios::loop();



  // if (millis() >= nextupdate) {
  //   outputstate = !outputstate;
  //   digitalWrite(RedLedMonitorPin, outputstate);
  //   nextupdate = millis() + flashinterval;
  // }
}
