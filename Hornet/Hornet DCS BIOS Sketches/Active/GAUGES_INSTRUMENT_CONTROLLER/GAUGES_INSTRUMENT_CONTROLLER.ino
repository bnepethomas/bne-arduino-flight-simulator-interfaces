// UPDATED TO DCS-BIOS FP EDITION FOR OPEN HORNET


////////////////////---||||||||||********||||||||||---\\\\\\\\\\\\\\\\\\\\
//||               FUNCTION = GUAGE CONTROLLER                       ||\\
//||              LOCATION IN THE PIT = LIP LEFT SIDE                ||\\
//||            ARDUINO PROCESSOR TYPE = Arduino Mega                 ||\\
//||      ARDUINO CHIP SERIAL NUMBER = SN -       ||\\
//||            PROGRAM PORT CONNECTED COM PORT = COM XX              ||\\
//||            ****DO CHECK S/N BEFORE UPLOAD NEW DATA****           ||\\
////////////////////---||||||||||********||||||||||---\\\\\\\\\\\\\\\\\\\\

// Framework for Arduinos

int Ethernet_In_Use = 1;
int Reflector_In_Use = 1;

#define MSFS_In_Use 0  // Used to interface into MSFS - set to 0 if not in use


#define DCSBIOS_In_Use 1
#define DCSBIOS_IRQ_SERIAL
#include <DcsBios.h>

// ###################################### Begin Ethernet Related #############################
#include <SPI.h>
#include <Ethernet.h>
#include <EthernetUdp.h>

#define EthernetStartupDelay 500
#define ES1_RESET_PIN 53

// These local Mac and IP Address will be reassigned early in startup based on
// the device ID as set by address pins
byte mac[] = { 0xA8, 0x61, 0x0A, 0x67, 0x83, 0x08 };
String sMac = "A8:61:0A:67:83:08";
IPAddress ip(172, 16, 1, 108);
String strMyIP = "172.16.1.108";

// Reflector
IPAddress reflectorIP(172, 16, 1, 10);
String strReflectorIP = "X.X.X.X";

// Arduino Due for Keystroke translation and Pixel Led driving
IPAddress targetIP(172, 16, 1, 110);
String strTargetIP = "X.X.X.X";

// Computer Running MSFS
IPAddress MSFSIP(172, 16, 1, 10);
String strMSFSIP = "X.X.X.X";

const unsigned int localport = 7788;
const unsigned int localdebugport = 7795;
const unsigned int keyboardport = 7788;
const unsigned int ledport = 7789;
const unsigned int remoteport = 7790;
const unsigned int reflectorport = 27000;
const unsigned int MSFSport = 7791;

const unsigned long delayBeforeSendingPacket = 2000;
unsigned long ethernetStartTime = 0;

int packetSize;
int debugLen;
EthernetUDP udp;
EthernetUDP debugUDP;
char packetBuffer[1000];     //buffer to store the incoming data
char outpacketBuffer[1000];  //buffer to store the outgoing data
String DebugString = "";
String BoardName = "Hornet gauges Instrument Controller";


void SendDebug(String MessageToSend) {
  if ((Reflector_In_Use == 1) && (Ethernet_In_Use == 1)) {
    udp.beginPacket(reflectorIP, reflectorport);
    udp.print(MessageToSend);
    udp.endPacket();
  }
}
// ###################################### End Ethernet Related #############################

// THE LED PORTS WILL CHANGE FROM THE V1.1 PCB TO THE FOLLOWING

#define RED_STATUS_LED_PORT 15
#define GREEN_STATUS_LED_PORT 14
#define Check_LED_R 15
#define Check_LED_G 14
#define FLASH_TIME 300

unsigned long NEXT_STATUS_TOGGLE_TIMER = 0;
bool GREEN_LED_STATE = false;
bool RED_LED_STATE = false;
unsigned long timeSinceRedLedChanged = 0;


#include <Servo.h>
#define DCSBIOS_IRQ_SERIAL
// #include "LedControl.h"
#include "DcsBios.h"

#include <Servo.h>

// ################################### BEGIN LIGHTING ##################################
#define BrightnessWhileRunningSetup 128
#define BrightnessPostSetup 65

#define ASH_DDI_PWM_5V 46
#define BACK_LIGHTS 11
#define BAT_HYD_DIM 3
#define BRK_PRESS_DIM 5
#define CAB_ALT_DIM 2
#define COMPASS_DIM 12
#define MAP_DIM 4
#define RADAR_ALTIMETER_DIM 6
#define SPARE_DIM 45


#define RAD_GN 9
#define RAD_RD 8

Servo RAD_ALT_servo;
#define RadarAltServoPin 7
#define RAD_ALT_servo_Off_Pos 125    // OFF FLAG IN WINDOW
#define RAD_ALT_servo_Hidden_Pos 50  // OFF FLAG NOT SHOWN

#include <Stepper.h>
#include <AccelStepper.h>
#define STEPS 720  // steps per revolution (limited to 315°)


#define COILRA1 A4  // RA = RAD ALT
#define COILRA2 A5
#define COILRA3 A6
#define COILRA4 A7

#define COILBT1_1 22  // BT_1 = BATTERY 1
#define COILBT1_2 23
#define COILBT1_3 24
#define COILBT1_4 25

#define COILBT2_1 28  //26 // BT_2 = BATTERY 2
#define COILBT2_2 29  //27
#define COILBT2_3 26  //28
#define COILBT2_4 27  //29

#define COILHPL_1 30  // HP_1 = HYD PRESSURE L
#define COILHPL_2 31
#define COILHPL_3 32
#define COILHPL_4 33

#define COILHPR_1 34  // HP_2 = HYD PRESSURE R
#define COILHPR_2 35
#define COILHPR_3 36
#define COILHPR_4 37

#define COILCA1 38  // CA = CAB ALT
#define COILCA2 39
#define COILCA3 40
#define COILCA4 41

#define COILBP1 A8  // BP = BRAKE PRESSURE
#define COILBP2 A9
#define COILBP3 A10
#define COILBP4 A11

#define COILCP1 A12  // CP = COMPASS
#define COILCP2 A13
#define COILCP3 A14
#define COILCP4 A15



int CAB_ALT;
int valCA = 0;
int CurCABALT = 0;
int posCA = 0;

int WarnCautionDimmerValue = 0;


#define STEPPER_MAX_SPEED 8300
#define STEPPER_ACCELERATION 2000

AccelStepper stepperRA(AccelStepper::FULL4WIRE, COILRA1, COILRA2, COILRA3, COILRA4);
// AccelStepper stepper(AccelStepper::FULL4WIRE, 46, 47, 48, 49);
// AccelStepper stepperRA(AccelStepper::FULL4WIRE, 46, 47, 48, 49);

// Stepper stepperCA(STEPS, COILCA1, COILCA2, COILCA3, COILCA4);  // CAB ALT
AccelStepper stepperCA(AccelStepper::FULL4WIRE, COILCA1, COILCA2, COILCA3, COILCA4);  // CAB ALT
#define CabinAltMaxPoint 600

AccelStepper stepperBP(AccelStepper::FULL4WIRE, COILBP3, COILBP4, COILBP1, COILBP2);
#define BrakePressureZeroPoint 30
#define BrakePressureMaxPoint 150

AccelStepper stepperBT1(AccelStepper::FULL4WIRE, COILBT1_1, COILBT1_2, COILBT1_3, COILBT1_4);  // BATTERY 1
#define stepperBT1MaxPoint 250
AccelStepper stepperBT2(AccelStepper::FULL4WIRE, COILBT2_1, COILBT2_2, COILBT2_3, COILBT2_4);  // BATTERY 2
#define stepperBT2MaxPoint 250


AccelStepper stepperHPL(AccelStepper::FULL4WIRE, COILHPL_1, COILHPL_2, COILHPL_3, COILHPL_4);  // HYD PRESSURE L
AccelStepper stepperHPR(AccelStepper::FULL4WIRE, COILHPR_1, COILHPR_2, COILHPR_3, COILHPR_4);  // HYD PRESSURE R

int MaxDCSBios = 65535;
int MinDCSBios = 0;
int RadAltChange = 14500;
int RAStepVal = 2620;
int RAStepSteps = 144;
int StepperSteps = 720;
//###########################################################################################

void onRadaltAltPtrChange(unsigned int newValue) {  //2620
  // SendDebug("Radio Alt=" + String(newValue));
  // suspect 0 position in the sim is the needle hidden
  if (newValue >= 3440) {
    newValue = newValue - 3440;
    stepperRA.moveTo(map(newValue, 0, 65535, 0, StepperSteps));
  } else {
    // Below 3400 hide the needle
    stepperRA.moveTo(720);
  }
  // if (newValueRA <= 56000) stepperRA.moveTo(map(newValueRA, 56000, 0, 0, 580));
  // else if (newValueRA >= 56000) stepperRA.moveTo(map(newValueRA, 56001, 65535, 581, 720));
}
DcsBios::IntegerBuffer radaltAltPtrBuffer(0x751a, 0xffff, 0, onRadaltAltPtrChange);

void onPressureAltChange(unsigned int newValueCA) {
  stepperCA.moveTo(map(newValueCA, 0, 65535, 0, CabinAltMaxPoint));
}
DcsBios::IntegerBuffer pressureAltBuffer(0x7514, 0xffff, 0, onPressureAltChange);


void onHydIndBrakeChange(unsigned int newValueBP) {
  stepperBP.moveTo(map(newValueBP, 0, 65535, BrakePressureZeroPoint, BrakePressureMaxPoint));
}
DcsBios::IntegerBuffer hydIndBrakeBuffer(0x7506, 0xffff, 0, onHydIndBrakeChange);

void onHydIndLeftChange(unsigned int newValueHL) {
  stepperHPL.moveTo(map(newValueHL, 0, 65535, 0, 600));
}
DcsBios::IntegerBuffer hydIndLeftBuffer(0x751e, 0xffff, 0, onHydIndLeftChange);

void onHydIndRightChange(unsigned int newValueHR) {
  stepperHPR.moveTo(map(newValueHR, 0, 65535, 0, 600));
}
DcsBios::IntegerBuffer hydIndRightBuffer(0x7520, 0xffff, 0, onHydIndRightChange);

void onVoltEChange(unsigned int newValueB1) {
  stepperBT1.moveTo(map(newValueB1, 0, 65535, 0, stepperBT1MaxPoint));
}
DcsBios::IntegerBuffer voltEBuffer(0x753e, 0xffff, 0, onVoltEChange);

void onVoltUChange(unsigned int newValueB2) {
  stepperBT2.moveTo(map(newValueB2, 0, 65535, 0, stepperBT2MaxPoint));
}
DcsBios::IntegerBuffer voltUBuffer(0x753c, 0xffff, 0, onVoltUChange);


void onRadaltOffFlagChange(unsigned int newValue) {
  if (newValue == 0) {  // ANYTHING ABOVE "0" IS OFF
    RAD_ALT_servo.attach(RadarAltServoPin);
    RAD_ALT_servo.write(RAD_ALT_servo_Hidden_Pos);  // set servo to "Off Point"
    delay(300);
    RAD_ALT_servo.detach();
  } else {
    RAD_ALT_servo.attach(RadarAltServoPin);
    RAD_ALT_servo.write(RAD_ALT_servo_Off_Pos);  // set servo to min
    delay(300);
    RAD_ALT_servo.detach();
  }
}
DcsBios::IntegerBuffer radaltOffFlagBuffer(0x751c, 0xffff, 0, onRadaltOffFlagChange);

//DcsBios::ServoOutput radaltOffFlag(0x751c, RadarAltServoPin, RAD_ALT_servo_Hidden_Pos, RAD_ALT_servo_Off_Pos);
//###########################################################################################

void onConsoleIntLtChange(unsigned int newValue) {
  int ConsolesDimmerValue = 0;

  ConsolesDimmerValue = map(newValue, 0, 65535, 5, 255);
  analogWrite(ASH_DDI_PWM_5V, ConsolesDimmerValue);
  analogWrite(BACK_LIGHTS, ConsolesDimmerValue);
  analogWrite(BAT_HYD_DIM, ConsolesDimmerValue);
  analogWrite(BRK_PRESS_DIM, ConsolesDimmerValue);
  analogWrite(CAB_ALT_DIM, ConsolesDimmerValue);
  analogWrite(COMPASS_DIM, ConsolesDimmerValue);
  analogWrite(MAP_DIM, ConsolesDimmerValue);
  analogWrite(RADAR_ALTIMETER_DIM, ConsolesDimmerValue);
  analogWrite(SPARE_DIM, ConsolesDimmerValue);


}

DcsBios::IntegerBuffer consoleIntLtBuffer(0x7558, 0xffff, 0, onConsoleIntLtChange);



void onWarnCautionDimmerChange(unsigned int newValue) {

  WarnCautionDimmerValue = map(newValue, 0, 65535, 5, 100);
}
DcsBios::IntegerBuffer warnCautionDimmerBuffer(0x754c, 0xffff, 0, onWarnCautionDimmerChange);

void onRadaltGreenLampChange(unsigned int newValue) {
  if (newValue == 1) {
    analogWrite(RAD_GN, WarnCautionDimmerValue);
  } else {
    digitalWrite(RAD_GN, 0);
  }
}
DcsBios::IntegerBuffer radaltGreenLampBuffer(0x74a0, 0x0100, 8, onRadaltGreenLampChange);

void onLowAltWarnLtChange(unsigned int newValue) {
  if (newValue == 1) {
    analogWrite(RAD_RD, WarnCautionDimmerValue);
  } else {
    digitalWrite(RAD_RD, 0);
  }
}
DcsBios::IntegerBuffer lowAltWarnLtBuffer(0x749c, 0x8000, 15, onLowAltWarnLtChange);


// controlPosition: 0 to 65,535 value representing the analog, real world control value
// dcsPosition: 0 to 65,535 value reported from DCS for the provided address
// return: −32,768 to 32,767 signed integer to be sent to the DCS rotary control.  0 will not be sent.
int HornetRadaltMapper(unsigned int controlPosition, unsigned int dcsPosition) {
  unsigned int a = map(controlPosition, 0, 65530, 161000, 1800);  // Silly right now, but to reduce the range if your analog pot doesn't reach max deflection, reduce the first 65535 number
  unsigned int b = map(dcsPosition, 0, 64355, 0, 65535);          // Observationally, in DCS the max value for RADALT_HEIGHT is 64355

  // Careful here since we are on 16 bit microcontrollers and doing some signed v unsigned maths.  Probably a better way to do this, but this works.
  unsigned int delta = (a >= b) ? a - b : b - a;

  const unsigned int MAX_ROTATION = 20000;  // Always keep less than 32767
  if (delta > MAX_ROTATION)
    delta = MAX_ROTATION;

  if (a >= b)
    return (int)delta;
  else
    return -1 * (int)delta;
}

// BEN


DcsBios::RotarySyncingPotentiometer radaltHeight("RADALT_HEIGHT", A0, 0x7518, 0xffff, 0, HornetRadaltMapper);


DcsBios::Switch2Pos radaltTestSw("RADALT_TEST_SW", A1);

// ************************************ End Exterior and Interior Lights Block

// Source
// https://gist.github.com/jboecker/1084b3768c735b164c34d6087d537c18
// the Warthog Project Video on the compass build
// https://www.youtube.com/watch?v=ZN9glqgp9TY&t=332s


#include <AccelStepper.h>
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
    if (initState == 0) {  // not initialized yet
      pinMode(irDetectorPin, INPUT);
      stepper.setMaxSpeed(stepperConfig.maxSpeed);
      stepper.setSpeed(400);

      initState = 1;
      zeroPosSearchStartTime = millis();
    }

    if (initState == 1) {
      // move off zero if already there so we always get movement on reset
      // (to verify that the stepper is working)
      SendDebug(BoardName + "Compass initState 1");
      if (zeroDetected()) {
        stepper.runSpeed();
      } else {
        initState = 2;
        SendDebug(BoardName + "Compass initState 2");
      }
    }

    if (initState == 2) {  // zeroing
      if (!zeroDetected()) {
        // Currently this safety check isn't working
        // Add Ethernet card for more troubleshooting
        // Need to check IP addresses of PC secondary nic
        if (millis() >= (zeroTimeout + zeroPosSearchStartTime)) {
          stepper.disableOutputs();
          initState == 99;
          SendDebug(BoardName + "Compass initState 99 - timeout finding zero point");
        } else
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
        SendDebug(BoardName + "Compass initState 3 - normal running");
      }
    }
    if (initState == 3) {  // running normally

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

    if (initState == 99) {  // Timed out looking for zero do nothing
      stepper.disableOutputs();
    }
  }
};

/* modify below this line */

/* define stepper parameters
   multiple Vid60Stepper instances can share the same StepperConfig object */
struct StepperConfig stepperConfig = {
  720,  // maxSteps
  200,  // maxSpeed
  50    // acceleration
};


// define AccelStepper instance
AccelStepper stepper(AccelStepper::FULL4WIRE, A12, A13, A14, A15);

// define Vid60Stepper class that uses the AccelStepper instance defined in the line above
//           v-- arbitrary name
// Vid60Stepper alt100ftPointer(0x107e,          // address of stepper data
Vid60Stepper standbyCompass(0x0436,         // address of stepper data
                            stepper,        // name of AccelStepper instance
                            stepperConfig,  // StepperConfig struct instance
                            16,             // IR Detector Pin (must be LOW in zero position)
                            440,            // zero offset
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


// ************************************ Begin Compass Block


void setup() {

  pinMode(GREEN_STATUS_LED_PORT, OUTPUT);
  pinMode(RED_STATUS_LED_PORT, OUTPUT);
  digitalWrite(GREEN_STATUS_LED_PORT, true);
  digitalWrite(RED_STATUS_LED_PORT, true);
  delay(FLASH_TIME);
  digitalWrite(GREEN_STATUS_LED_PORT, false);
  delay(FLASH_TIME);

  if (Ethernet_In_Use == 1) {

    // Using manual reset instead of tying to Arduino Reset
    pinMode(ES1_RESET_PIN, OUTPUT);
    digitalWrite(ES1_RESET_PIN, LOW);
    delay(2);
    digitalWrite(ES1_RESET_PIN, HIGH);

    Ethernet.begin(mac, ip);
    udp.begin(localport);

    ethernetStartTime = millis() + delayBeforeSendingPacket;
    while (millis() <= ethernetStartTime) {
      delay(FLASH_TIME);
      digitalWrite(Check_LED_G, false);
      delay(FLASH_TIME);
      digitalWrite(Check_LED_G, true);
    }


    SendDebug(BoardName + " Ethernet Started " + strMyIP + " " + sMac);

    pinMode(RAD_GN, OUTPUT);
    pinMode(RAD_RD, OUTPUT);

    digitalWrite(RAD_GN, HIGH);
    digitalWrite(RAD_RD, HIGH);

    // Lights

    pinMode(ASH_DDI_PWM_5V, OUTPUT);
    pinMode(BACK_LIGHTS, OUTPUT);
    pinMode(BAT_HYD_DIM, OUTPUT);
    pinMode(BRK_PRESS_DIM, OUTPUT);
    pinMode(CAB_ALT_DIM, OUTPUT);
    pinMode(COMPASS_DIM, OUTPUT);
    pinMode(MAP_DIM, OUTPUT);
    pinMode(RADAR_ALTIMETER_DIM, OUTPUT);
    pinMode(SPARE_DIM, OUTPUT);

    SendDebug(BoardName + "Leds On");

    analogWrite(ASH_DDI_PWM_5V, 255);
    analogWrite(BACK_LIGHTS, 255);
    analogWrite(BAT_HYD_DIM, 255);
    analogWrite(BRK_PRESS_DIM, 255);
    analogWrite(CAB_ALT_DIM, 255);
    analogWrite(COMPASS_DIM, 255);
    analogWrite(MAP_DIM, 255);
    analogWrite(RADAR_ALTIMETER_DIM, 255);
    analogWrite(SPARE_DIM, 255);
    delay(3000);

    SendDebug(BoardName + "Dimming Leds");
    for (int Local_Brightness = 255; Local_Brightness >= 0; Local_Brightness--) {
      analogWrite(ASH_DDI_PWM_5V, Local_Brightness);
      analogWrite(BACK_LIGHTS, Local_Brightness);
      analogWrite(BAT_HYD_DIM, Local_Brightness);
      analogWrite(BRK_PRESS_DIM, Local_Brightness);
      analogWrite(CAB_ALT_DIM, Local_Brightness);
      analogWrite(COMPASS_DIM, Local_Brightness);
      analogWrite(MAP_DIM, Local_Brightness);
      analogWrite(RADAR_ALTIMETER_DIM, Local_Brightness);
      analogWrite(SPARE_DIM, Local_Brightness);
      // SendDebug("Led Brightness " + String(Local_Brightness));
      delay(15);
    }

    SendDebug(BoardName + "Leds Brightness to Setup Level");

    analogWrite(ASH_DDI_PWM_5V, BrightnessWhileRunningSetup);
    analogWrite(BACK_LIGHTS, BrightnessWhileRunningSetup);
    analogWrite(BAT_HYD_DIM, BrightnessWhileRunningSetup);
    analogWrite(BRK_PRESS_DIM, BrightnessWhileRunningSetup);
    analogWrite(CAB_ALT_DIM, BrightnessWhileRunningSetup);
    analogWrite(COMPASS_DIM, BrightnessWhileRunningSetup);
    analogWrite(MAP_DIM, BrightnessWhileRunningSetup);
    analogWrite(RADAR_ALTIMETER_DIM, BrightnessWhileRunningSetup);
    analogWrite(SPARE_DIM, BrightnessWhileRunningSetup);


    SendDebug(BoardName + " Start Cycling Radio Altimeter Servo");
    RAD_ALT_servo.attach(RadarAltServoPin);
    SendDebug(BoardName + " Servo Flag Visible");
    RAD_ALT_servo.write(RAD_ALT_servo_Off_Pos);  // set servo to "Off Point"
    // RAD_ALT_servo.detach();
    delay(1000);
    // RAD_ALT_servo.attach(RadarAltServoPin);
    SendDebug(BoardName + " Servo Flag Hidden");
    RAD_ALT_servo.write(RAD_ALT_servo_Hidden_Pos);  // set servo to min
    delay(1000);
    SendDebug(BoardName + " Servo Flag Visible");
    RAD_ALT_servo.write(RAD_ALT_servo_Off_Pos);  // set servo to "Off Point"
    // RAD_ALT_servo.detach();
    delay(1000);
    RAD_ALT_servo.detach();
    //analogWrite(5, LOW);

    //###########################################################################################
    /// RADAR ALT WORKING ======> SET RADAR ALT STEPPER TO 0 FEET
    SendDebug(BoardName + " Start Cycling Radio Altimeter Stepper");
    stepperRA.setMaxSpeed(1000);
    stepperRA.setAcceleration(400);
    stepperRA.runToNewPosition(-630);
    stepperRA.setCurrentPosition(0);
    stepperRA.runToNewPosition(630);
    stepperRA.runToNewPosition(0);

    /// RADAR ALT WORKING ======< SET RADAR ALT STEPPER TO 0 FEET

    /// CABIN ALT WORKING ======> SET CABIN ALT STEPPER TO 0 FEET
    SendDebug(BoardName + " Start Cycling Cabin Altimeter Stepper");
    stepperCA.setMaxSpeed(1000);
    stepperCA.setAcceleration(400);
    stepperCA.runToNewPosition(-720);
    stepperCA.setCurrentPosition(0);
    stepperCA.runToNewPosition(CabinAltMaxPoint);
    stepperCA.runToNewPosition(0);
    SendDebug(BoardName + " End Cycling Cabin Altimeter Stepper");
    /// CABIN ALT WORKING ======< SET CABIN ALT STEPPER TO 0 FEET


    // BRAKE PRESSURE
    SendDebug(BoardName + " Start Cycling Brake Pressure");
    stepperBP.setMaxSpeed(STEPPER_MAX_SPEED);
    stepperBP.setAcceleration(STEPPER_ACCELERATION);
    stepperBP.runToNewPosition(-200);
    stepperBP.setCurrentPosition(0);
    stepperBP.runToNewPosition(BrakePressureZeroPoint);
    stepperBP.setCurrentPosition(0);
    stepperBP.runToNewPosition(BrakePressureMaxPoint);
    stepperBP.runToNewPosition(0);
    SendDebug(BoardName + " End Cycling Brake Pressure");
    /// BRAKE PRESSURE

    // HYD PRESSURE LEFT
    SendDebug(BoardName + " Start Cycling Hyd Pressure Left");
    stepperHPL.setMaxSpeed(STEPPER_MAX_SPEED);
    stepperHPL.setAcceleration(STEPPER_ACCELERATION);
    stepperHPL.runToNewPosition(-650);
    stepperHPL.setCurrentPosition(0);
    stepperHPL.runToNewPosition(80);
    stepperHPL.setCurrentPosition(0);
    stepperHPL.runToNewPosition(600);
    stepperHPL.runToNewPosition(0);


    SendDebug(BoardName + " Start Cycling Hyd Pressure Right");
    stepperHPR.setMaxSpeed(STEPPER_MAX_SPEED);
    stepperHPR.setAcceleration(STEPPER_ACCELERATION);
    stepperHPR.runToNewPosition(-650);
    stepperHPR.setCurrentPosition(0);
    stepperHPR.runToNewPosition(600);
    stepperHPR.runToNewPosition(0);

    // BATT 1 E

    SendDebug(BoardName + " Start Cycling Batttery 1");
    stepperBT1.setMaxSpeed(STEPPER_MAX_SPEED);
    stepperBT1.setAcceleration(STEPPER_ACCELERATION);
    stepperBT1.runToNewPosition(-400);
    stepperBT1.setCurrentPosition(0);
    stepperBT1.runToNewPosition(stepperBT1MaxPoint);
    stepperBT1.runToNewPosition(0);
    // stepperBT1.step(560);   //BATT "16V" POSITION E


    // BATT 2 U
    SendDebug(BoardName + " Start Cycling Batttery 2");
    stepperBT2.setMaxSpeed(STEPPER_MAX_SPEED);
    stepperBT2.setAcceleration(STEPPER_ACCELERATION);
    stepperBT2.runToNewPosition(-400);
    stepperBT2.setCurrentPosition(0);
    stepperBT2.runToNewPosition(stepperBT1MaxPoint);
    stepperBT2.runToNewPosition(0);



    delay(500);
    SendDebug(BoardName + "Leds Brightness to Post Setup Level");

    analogWrite(ASH_DDI_PWM_5V, BrightnessPostSetup);
    analogWrite(BACK_LIGHTS, BrightnessPostSetup);
    analogWrite(BAT_HYD_DIM, BrightnessPostSetup);
    analogWrite(BRK_PRESS_DIM, BrightnessPostSetup);
    analogWrite(CAB_ALT_DIM, BrightnessPostSetup);
    analogWrite(COMPASS_DIM, BrightnessPostSetup);
    analogWrite(MAP_DIM, BrightnessPostSetup);
    analogWrite(RADAR_ALTIMETER_DIM, BrightnessPostSetup);
    analogWrite(SPARE_DIM, BrightnessPostSetup);


    digitalWrite(RAD_GN, LOW);
    digitalWrite(RAD_RD, LOW);

    SendDebug(BoardName + " End Setup");


    DcsBios::setup();
  }
}

void sendToDcsBiosMessage(const char* msg, const char* arg) {


  if (Reflector_In_Use == 1) {
    udp.beginPacket(reflectorIP, reflectorport);
    udp.print(BoardName + String(msg) + ":" + String(arg));
    udp.endPacket();
  }

  sendDcsBiosMessage(msg, arg);
}

void loop() {

  if (millis() >= NEXT_STATUS_TOGGLE_TIMER) {
    GREEN_LED_STATE = !GREEN_LED_STATE;
    RED_LED_STATE = !GREEN_LED_STATE;
    digitalWrite(GREEN_STATUS_LED_PORT, GREEN_LED_STATE);
    digitalWrite(RED_STATUS_LED_PORT, RED_LED_STATE);
    NEXT_STATUS_TOGGLE_TIMER = millis() + FLASH_TIME;
  }


  // **************************** Handle Steppers
  stepperRA.run();
  stepperCA.run();
  stepperBP.run();
  stepperHPL.run();
  stepperHPR.run();
  stepperBT1.run();
  stepperBT2.run();

  DcsBios::loop();
}
