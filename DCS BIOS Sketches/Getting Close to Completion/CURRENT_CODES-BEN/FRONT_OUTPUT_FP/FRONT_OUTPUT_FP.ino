// UPDATED TO DSCS-BIOS FP EDITION FOR OPEN HORNET


////////////////////---||||||||||********||||||||||---\\\\\\\\\\\\\\\\\\\\
//||               FUNCTION = FRONT OUTPUT                           ||\\
//||              LOCATION IN THE PIT = LIP LEFT HAND SIDE             ||\\
//||            ARDUINO PROCESSOR TYPE = Arduino Mega                ||\\
//||      ARDUINO CHIP SERIAL NUMBER = SN - 859373138373516121E2      ||\\
//||            PROGRAM PORT CONNECTED COM PORT = COM 10             ||\\
//||            ****DO CHECK S/N BEFORE UPLOAD NEW DATA****           ||\\
////////////////////---||||||||||********||||||||||---\\\\\\\\\\\\\\\\\\\\

// Tell DCS-BIOS to use a serial connection and use interrupt-driven
// communication. The main program will be interrupted to prioritize
// processing incoming data.
//
// This should work on any Arduino that has an ATMega328 controller
// (Uno, Pro Mini, many others).
String readString;
#include <Servo.h>
#define DCSBIOS_IRQ_SERIAL

#include "LedControl.h"
#include "DcsBios.h"

#define RED_STATUS_LED_PORT 5               // RED LED is used for monitoring ethernet
#define GREEN_STATUS_LED_PORT 13               // RED LED is used for monitoring ethernet
#define FLASH_TIME 1000
unsigned long NEXT_STATUS_TOGGLE_TIMER = 0;
bool GREEN_LED_STATE = false;

#define APU_PORT 30
#define ENGINE_CRANK_PORT 31
#define FUEL_DUMP_PORT 29
#define HOOK_BYPASS_PORT 28
#define LAUNCH_BAR_PORT 27

unsigned long NEXT_PORT_TOGGLE_TIMER = 0;
bool PORT_OUTPUT_STATE = false;


#include <Servo.h>
//#define DCSBIOS_IRQ_SERIAL
#define DCSBIOS_DEFAULT_SERIAL
#include "LedControl.h"
#include "DcsBios.h"

#include <Stepper.h>
#define  STEPS  720    // steps per revolution (limited to 315°)

#define  COILRA1  45 // RA = RAD ALT
#define  COILRA2  47
#define  COILRA3  41
#define  COILRA4  43

#define  COILCA1  36 // CA = CAB ALT
#define  COILCA2  38
#define  COILCA3  34
#define  COILCA4  32

#define  COILBP1  39 // BP = BRAKE PRESSURE
#define  COILBP2  37
#define  COILBP3  35
#define  COILBP4  33

int RAD_ALT = 0;
int valRA = 0;
int CAB_ALT;
int valCA = 0;
int CurCABALT = 0;
int BRAKE_PRESSURE = 0;
int valBP = 0;
int posCA = 0;
int posRA = 0;
int posBP = 0;

Stepper stepperRA(STEPS, COILRA1, COILRA2, COILRA3, COILRA4); // RAD ALT
Stepper stepperCA(STEPS, COILCA1, COILCA2, COILCA3, COILCA4); // CAB ALT
Stepper stepperBP(STEPS, COILBP1, COILBP2, COILBP3, COILBP4); // BRAKE PRESSURE
#define BrakePressureZeroPoint 0
#define BrakePressureMaxPoint 150

//###########################################################################################
void onRadaltAltPtrChange(unsigned int newValueRA) {
  RAD_ALT = map(newValueRA, 0, 65000, 720, 10);
}
DcsBios::IntegerBuffer radaltAltPtrBuffer(0x751a, 0xffff, 0, onRadaltAltPtrChange);
//###########################################################################################

void onPressureAltChange(unsigned int newValueCA) {
  CAB_ALT = map(newValueCA, 0, 65000, 45, 720);
}
DcsBios::IntegerBuffer pressureAltBuffer(0x7514, 0xffff, 0, onPressureAltChange);


void onHydIndBrakeChange(unsigned int newValueBP) {
  BRAKE_PRESSURE = map(newValueBP, 0, 65000, BrakePressureZeroPoint, BrakePressureMaxPoint);
}
DcsBios::IntegerBuffer hydIndBrakeBuffer(0x7506, 0xffff, 0, onHydIndBrakeChange);


Servo RAD_ALT_servo;
Servo HYD_LFT_servo;
Servo HYD_RHT_servo;
Servo BATT_U_servo;
Servo BATT_E_servo;
Servo TRIM_servo;

#define RadarAltServoPin 11
#define HydLeftServoPin 7
#define HydRightServoPin 6
#define VoltEServoPin 9
#define VoltUServoPin 8
#define TrimServoPin 12

DcsBios::ServoOutput radaltOffFlag(0x751c, RadarAltServoPin, 1000, 1420);
DcsBios::ServoOutput hydIndLeft(0x751e, HydLeftServoPin, 560, 2300);
DcsBios::ServoOutput hydIndRight(0x7520, HydRightServoPin, 560, 2300);
DcsBios::ServoOutput voltE(0x753e, VoltEServoPin, 1800, 550);
DcsBios::ServoOutput voltU(0x753c, VoltUServoPin, 550, 1800);
// DcsBios::ServoOutput rudTrim(0x7528, TrimServoPin, 544, 2400);


void onLaunchBarSwChange(unsigned int newValue) {
  digitalWrite(LAUNCH_BAR_PORT, newValue);
}
DcsBios::IntegerBuffer launchBarSwBuffer(0x7480, 0x2000, 13, onLaunchBarSwChange);

void onHookBypassSwChange(unsigned int newValue) {
  digitalWrite( RED_STATUS_LED_PORT, newValue);
  digitalWrite(HOOK_BYPASS_PORT, newValue);
}
DcsBios::IntegerBuffer hookBypassSwBuffer(0x7480, 0x4000, 14, onHookBypassSwChange);


void onApuControlSwChange(unsigned int newValue) {
  digitalWrite(APU_PORT, newValue);
}
DcsBios::IntegerBuffer apuControlSwBuffer(0x74c2, 0x0100, 8, onApuControlSwChange);


void onEngineCrankSwChange(unsigned int newValue) {
  bool CrankSwitchState = false;
  if (newValue != 1) {
    CrankSwitchState = true;
  }
  else
  {
    CrankSwitchState = false;
  }

  digitalWrite(ENGINE_CRANK_PORT, newValue);
}
DcsBios::IntegerBuffer engineCrankSwBuffer(0x74c2, 0x0600, 9, onEngineCrankSwChange);




void onFuelDumpSwChange(unsigned int newValue) {
  digitalWrite(FUEL_DUMP_PORT, newValue);
}
DcsBios::IntegerBuffer fuelDumpSwBuffer(0x74b4, 0x0100, 8, onFuelDumpSwChange);


void setup() {


  pinMode( RED_STATUS_LED_PORT,  OUTPUT);
  pinMode( GREEN_STATUS_LED_PORT,  OUTPUT);

  digitalWrite( RED_STATUS_LED_PORT, true);
  digitalWrite( GREEN_STATUS_LED_PORT, true);
  delay(FLASH_TIME);
  digitalWrite( RED_STATUS_LED_PORT, false);
  digitalWrite( GREEN_STATUS_LED_PORT, false);

  delay(FLASH_TIME);

  pinMode( APU_PORT,  OUTPUT);
  pinMode( ENGINE_CRANK_PORT,  OUTPUT);
  pinMode( FUEL_DUMP_PORT,  OUTPUT);
  pinMode( HOOK_BYPASS_PORT,  OUTPUT);
  pinMode( LAUNCH_BAR_PORT,  OUTPUT);


  // Turn everything off
  digitalWrite(APU_PORT, false);
  digitalWrite(ENGINE_CRANK_PORT, false);
  digitalWrite(FUEL_DUMP_PORT, false);
  digitalWrite(HOOK_BYPASS_PORT, false);
  digitalWrite(LAUNCH_BAR_PORT, false);




  RAD_ALT_servo.attach(RadarAltServoPin);
  RAD_ALT_servo.writeMicroseconds(1420);  // set servo to "Off Point"
  delay(300);
  RAD_ALT_servo.detach();

  HYD_LFT_servo.attach(HydLeftServoPin);
  HYD_LFT_servo.writeMicroseconds(1100);  // set servo to "Mid Point"
  delay(300);
  HYD_LFT_servo.detach();

  HYD_RHT_servo.attach(HydRightServoPin);
  HYD_RHT_servo.writeMicroseconds(1100);  // set servo to "Mid Point"
  delay(300);
  HYD_RHT_servo.detach();

  BATT_U_servo.attach(VoltUServoPin);
  BATT_U_servo.writeMicroseconds(1100);  // set servo to "Mid Point"
  delay(300);
  BATT_U_servo.detach();

  BATT_E_servo.attach(VoltEServoPin);
  BATT_E_servo.writeMicroseconds(1100);  // set servo to "Mid Point"
  delay(300);
  BATT_E_servo.detach();


  TRIM_servo.attach(TrimServoPin);
  TRIM_servo.writeMicroseconds(1100);  // set servo to "Mid Point"
  delay(1000);
   TRIM_servo.writeMicroseconds(500);  // set servo to "Mid Point"
  delay(400);
  TRIM_servo.detach();


  //###########################################################################################
  /// RADAR ALT WORKING ======> SET RADAR ALT STEPPER TO 0 FEET
  stepperRA.setSpeed(50);
  stepperRA.step(720);       //Reset FULL ON Position
  stepperRA.step(-720);       //Reset FULL OFF Position
  posRA = 0;
  RAD_ALT = map(0, 0, 65000, 720, 10);
  /// RADAR ALT WORKING ======< SET RADAR ALT STEPPER TO 0 FEET

  /// CABIN ALT WORKING ======> SET CABIN ALT STEPPER TO 0 FEET
  stepperCA.setSpeed(60);
  stepperCA.step(720);       //Reset FULL ON Position
  stepperCA.step(-720);       //Reset FULL OFF Position
  posCA = 0;
  CAB_ALT = map(0, 0, 65000, 40, 720);
  /// CABIN ALT WORKING ======< SET CABIN ALT STEPPER TO 0 FEET



  // BRAKE PRESSURE

  stepperBP.setSpeed(60);
  stepperBP.step(BrakePressureMaxPoint);       //Reset FULL ON Position
  delay(1000);
  stepperBP.step(-(BrakePressureMaxPoint + 100) );       //Reset FULL OFF Position
  posBP = 0;
  BRAKE_PRESSURE = map(0, 0, 65000, BrakePressureZeroPoint, BrakePressureMaxPoint);
  /// BRAKE PRESSURE

  DcsBios::setup();

  NEXT_PORT_TOGGLE_TIMER = millis() + 1000;
  NEXT_STATUS_TOGGLE_TIMER = millis() + 1000;
}


void loop() {

  if (millis() > NEXT_STATUS_TOGGLE_TIMER) {
    GREEN_LED_STATE = !GREEN_LED_STATE;
    digitalWrite( GREEN_STATUS_LED_PORT, GREEN_LED_STATE);
    NEXT_STATUS_TOGGLE_TIMER = millis() + FLASH_TIME;
  }

  // **************************** Handle Steppers

  valRA = RAD_ALT;
  if (abs(valRA - posRA) > 2) {      //if diference is greater than 2 steps.
    if ((valRA - posRA) > 0) {
      stepperRA.step(-1);      // move one step to the left.
      posRA++;
    }
    if ((valRA - posRA) < 0) {
      stepperRA.step(1);       // move one step to the right.
      posRA--;
    }
  }

  /// RADAR ALT LOOP WORKING ======<
  //###########################################################################################
  {
    valCA = CAB_ALT;
    //
    if (abs(valCA - posCA) > 2) {      //if diference is greater than 2 steps.
      if ((valCA - posCA) > 0) {
        stepperCA.step(1);      // move one step to the left.
        posCA++;
      }
      if ((valCA - posCA) < 0) {
        stepperCA.step(-1);       // move one step to the right.
        posCA--;
      }
      /// CABIN ALT LOOP WORKING ======<
      //###########################################################################################
    }

  }


  /// BRAKE PRESSURE
  //###########################################################################################
  {
    valBP = BRAKE_PRESSURE;
    //
    if (abs(valBP - posBP) > 2) {      //if diference is greater than 2 steps.
      if ((valBP - posBP) > 0) {
        stepperBP.step(1);      // move one step to the left.
        posBP++;
      }
      if ((valBP - posBP) < 0) {
        stepperBP.step(-1);       // move one step to the right.
        posBP--;
      }
    }

  }


  DcsBios::loop();
}
