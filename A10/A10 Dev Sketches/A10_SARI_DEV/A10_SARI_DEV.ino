// SARI

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
    udp.println(MessageToSend);
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

#define HOZ_IR_sen 24  // HOZ
#define VER_IR_sen 25  // VRT
#define WTR_IR_sen 22  // WTR

#define EN_switchV 37  // VER
#define EN_switchH 43  // HOZ
#define EN_switchW 27  // WTR

Servo SARIServo;

//HOZ
AccelStepper stepperH(1, 41, 39);  // 1 = Easy Driver interface
//VRT
AccelStepper stepperV(1, 35, 33);  // 1 = Easy Driver interface
//WTR
AccelStepper stepperW(1, 29, 31);  // 1 = Easy Driver interface
long valWAT = 0;
long posWAT = 0;

long valHOZ = 0;
long posHOZ = 0;

long valVER = 0;
long posVER = 0;

// Stepper Travel Variables

long initial_homingH = 0;  // Used to Home Stepper at startup
long initial_homingV = 0;  // Used to Home Stepper at startup
long initial_homingW = 0;  // Used to Home Stepper at startup


int DCS_On = 0;
int previous_DCS_State = 0;
int DCS_State = 0;

int SARI_Flag = 1;
#define SARI_Flag_Pin 5  // SARI FLAG PIN

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


void onSaiAttWarnFlagLChange(unsigned int newValue) {
  //SARI_Flag = newValue;
  if (newValue == 1)
    disable_SARI_FLAG();
  else
    enable_SARI_FLAG();
}
DcsBios::IntegerBuffer saiAttWarnFlagLBuffer(0x74d6, 0x0100, 8, onSaiAttWarnFlagLChange);

//////SARI - TEST - BEN --------------------------------------------------------------------------------------------------------------
//----------ROLL SERVO----------
DcsBios::ServoOutput saiPitch(0x74e4, 7, 2400, 544);

//----------ROLL STEPPER----------

const long zeroTimeout = 50000;
const int SARIenablePin = 4;


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

      enable_SARI_ROLL();
      initState = 1;
      SendDebug("Do a quick loop");

      stepper.moveTo(1800);
      while (stepper.distanceToGo() != 0) {
        stepper.run();
      }


      SendDebug("SARI initState moving to State 1");
      SARIzeroPosSearchStartTime = millis();
    }

    if (initState == 1) {
      // move off zero if already there so we always get movement on reset
      // (to verify that the stepper is working)
      if (SARIzeroDetected()) {

        enable_SARI_ROLL();
        stepper.runSpeed();
      } else {
        initState = 2;
        SendDebug("SARI initState moving to State 2");
        stepper.setMaxSpeed(SARIstepperConfig.maxSpeed);
        stepper.setAcceleration(SARIstepperConfig.SARIacceleration);
      }
    }

    if (initState == 2) {  // zeroing

      enable_SARI_ROLL();

      if (!SARIzeroDetected()) {

        if (millis() >= (zeroTimeout + SARIzeroPosSearchStartTime)) {
          SendDebug("SARI Roll - timeoutout finding zero, disabling driver pin");
          disable_SARI_ROLL();
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
        disable_SARI_ROLL();
        ;
      }
    }


    if (initState == 99) {  // Timed out looking for zero do nothing
      disable_SARI_ROLL();
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
          enable_SARI_ROLL();
        }  // LOW  = stepper ON drive current available




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
  3200,  // SARImaxSteps //200 Native steps with 1/16 MICRO STEPPING
  90000,  // maxSpeed //3200
  60000    // SARIacceleration 3200
};
const int SARIstepPin = 6;
const int SARIdirectionPin = 7;

// define AccelStepper instance

AccelStepper SARIstepperRoll(AccelStepper::DRIVER, SARIstepPin, SARIdirectionPin);

Nema8Stepper SARIRoll(0x74e6,             // address of stepper data
                      SARIstepperRoll,    // name of AccelStepper instance
                      SARIstepperConfig,  // SARIStepperConfig struct instance
                      8,                  // IR Detector Pin (must be LOW in zero position)
                      800,                // zero offset (SET TO 50% of MaX Steps) 1650
                                          // WIngs Level = 1/2 Max Steps -> "Zero" = 1/2 Turn
                                          // SARI will be upsidedown after Setup, this will correct in Bios
                      [](unsigned int newValue) -> unsigned int {
                        newValue = newValue & 0xffff;
                        return map(newValue, 0, 65535, 0, SARIstepperConfig.SARImaxSteps - 1);
                      });




void onSaiManPitchAdjChange(unsigned int newValue) {
  valWAT = map(newValue, 0, 65535, 0, 1500);
  SendDebug("ManPitch " + String(valWAT));
  stepperW.moveTo(valWAT);
}
DcsBios::IntegerBuffer saiManPitchAdjBuffer(0x74ea, 0xffff, 0, onSaiManPitchAdjChange);

void onSaiPointerHorChange(unsigned int newValue) {
  if (newValue == 65535) {
    valHOZ = 0;
    stepperH.moveTo(valHOZ);
  } else {
    valHOZ = map(newValue, 1, 65494, 3600, 1600);
    stepperH.moveTo(valHOZ);
  }
}
DcsBios::IntegerBuffer saiPointerHorBuffer(0x756c, 0xffff, 0, onSaiPointerHorChange);

void onSaiPointerVerChange(unsigned int newValue) {
  if (newValue == 0) {
    valVER = 0;
    stepperV.moveTo(valVER);
  } else {
    // valVER = map(newValue, 1, 65535, 1500, 3600);
    valVER = map(newValue, 1, 65535, 1500, 4150);
    stepperV.moveTo(valVER);
  }
}
DcsBios::IntegerBuffer saiPointerVerBuffer(0x756a, 0xffff, 0, onSaiPointerVerChange);

DcsBios::RotaryEncoder saiSet("SAI_SET", "-800", "+800", 28, 26);

DcsBios::Switch2Pos saiTestBtn("SAI_TEST_BTN", A12);
DcsBios::Switch2Pos saiCage("SAI_CAGE", A13);



// ######################################  End SARI  ######################################


void enable_switchW() {
  digitalWrite(EN_switchW, LOW);
  setStepperLedOn();
  // SendDebug("Enabling switchW");
}

void enable_switchV() {
  digitalWrite(EN_switchV, LOW);
  setStepperLedOn();
  // SendDebug("Enabling switchV");
}

void enable_switchH() {
  digitalWrite(EN_switchH, LOW);
  setStepperLedOn();
  // SendDebug("Enabling switchH");
}

void enable_SARI_ROLL() {
  SARI_ROLL_ENABLED = true;
  digitalWrite(SARIenablePin, LOW);
  setStepperLedOn();
}

void enable_SARI_FLAG() {
  digitalWrite(SARI_Flag_Pin, HIGH);
  setStepperLedOn();
}

void disable_SARI_ROLL() {
  digitalWrite(SARIenablePin, HIGH);
  SARI_ROLL_ENABLED = false;
  checkStepperDisabledStatus();
}

void disable_SARI_FLAG() {
  digitalWrite(SARI_Flag_Pin, LOW);
  checkStepperDisabledStatus();
}

void disableAllPointers() {
  digitalWrite(EN_switchW, HIGH);
  digitalWrite(EN_switchV, HIGH);
  digitalWrite(EN_switchH, HIGH);
  digitalWrite(SARIenablePin, HIGH);
  digitalWrite(SARI_Flag_Pin, LOW);
  checkStepperDisabledStatus();
}


void enableAllPointers() {
  // Currently not touching SARI with enable
  enable_switchW();
  enable_switchV();
  enable_switchH();
}

void setStepperLedOn() {
  digitalWrite(Check_LED_R, HIGH);  // RED LED ON PCB TO SHOW ENABLE SWITCHES ARE ACTIVE AND POWER IS ON TO THE STEPPERS IF REQUIRED - SET BY CODE
  digitalWrite(Check_LED_G, LOW);
}

void setStepperLedOff() {
  digitalWrite(Check_LED_G, HIGH);  // GREEN LED ON PCB TO SHOW **ALL** ENABLE SWITCHES ARE SAFE - OFF
  digitalWrite(Check_LED_R, LOW);
}


void checkStepperDisabledStatus() {
  if ((digitalRead(EN_switchW) == HIGH) && (digitalRead(EN_switchV) == HIGH) && (digitalRead(EN_switchH) == HIGH) && (digitalRead(SARIenablePin) == HIGH) && (digitalRead(SARI_Flag_Pin) == LOW)) {
    setStepperLedOff();  // CHECK ALL STEPPER DRIVERS ARE DISABLED BEFORE CHANGING LED STATE
  }
}




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
    SendDebug("A10_SARI_DEV 20240714");
  }


  delay(FLASH_TIME);
  digitalWrite(Check_LED_G, false);
  delay(FLASH_TIME);



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
