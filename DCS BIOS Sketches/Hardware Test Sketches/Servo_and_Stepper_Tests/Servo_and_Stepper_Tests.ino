// Simple Test sktech to test steppers and servos
String readString;
#include <Servo.h>


#include "LedControl.h"


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

// 1 Direction is reversed
//#define  COILCA1  36 // CA = CAB ALT
//#define  COILCA2  38
//#define  COILCA3  34
//#define  COILCA4  32

#define  COILCA1  34 // CA = CAB ALT
#define  COILCA2  32
#define  COILCA3  36
#define  COILCA4  38


////1 not working
//#define  COILBP1  39 // BP = BRAKE PRESSURE
//#define  COILBP2  35
//#define  COILBP3  37
//#define  COILBP4  33

//2 working
#define  COILBP1  39 // BP = BRAKE PRESSURE
#define  COILBP2  37
#define  COILBP3  35
#define  COILBP4  33

////3 moving but crazy
//#define  COILBP1  39 // BP = BRAKE PRESSURE
//#define  COILBP2  33
//#define  COILBP3  37
//#define  COILBP4  35

////4 kind of working but buoncnig at zero end
//#define  COILBP1  39 // BP = BRAKE PRESSURE
//#define  COILBP2  33
//#define  COILBP3  35
//#define  COILBP4  37

////5 Kind of working but bouncing at end
//#define  COILBP1  39 // BP = BRAKE PRESSURE
//#define  COILBP2  33
//#define  COILBP3  37
//#define  COILBP4  35


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
  BRAKE_PRESSURE = map(newValueBP, 0, 65000, 45, 720);
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

#define LAUNCH_BAR_PORT 27
#define HOOK_BYPASS_PORT 28
#define FUEL_DUMP_PORT 29
#define APU_PORT 30
#define ENGINE_CRANK_PORT 31


#define BLEED_AIR_SOL_PORT 22
#define PITOT_HEAT_PORT 23
#define LASER_ARM_PORT 24
#define CANOPY_MAG_PORT 25


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
  HYD_LFT_servo.attach(HydLeftServoPin);
  HYD_RHT_servo.attach(HydRightServoPin);
  BATT_U_servo.attach(VoltUServoPin);
  BATT_E_servo.attach(VoltEServoPin);
  TRIM_servo.attach(TrimServoPin);

#define RAD_ALT_servo_Off_Pos     1420
#define HYD_LFT_servo_Max_Pos     2340
#define HYD_RHT_servo_Max_pos     2340
#define BATT_U_servo_Max_Pos      1900
#define BATT_E_servo_Max_Pos      180
#define TRIM_servo_Off_Center_Pos 1100

#define RAD_ALT_servo_Hidden_Pos  1050
#define HYD_LFT_servo_Min_Pos     600
#define HYD_RHT_servo_Min_pos     600
#define BATT_U_servo_Min_Pos      400
#define BATT_E_servo_Min_Pos      1800
#define TRIM_servo_Center_Pos     800


  RAD_ALT_servo.writeMicroseconds(RAD_ALT_servo_Off_Pos);  // set servo to "Off Point"
  HYD_LFT_servo.writeMicroseconds(HYD_LFT_servo_Max_Pos);  // set servo to "Mid Point"
  HYD_RHT_servo.writeMicroseconds(HYD_RHT_servo_Max_pos);  // set servo to "Mid Point"
  BATT_U_servo.writeMicroseconds(BATT_U_servo_Max_Pos);  // set servo to "Mid Point"
  BATT_E_servo.writeMicroseconds(BATT_E_servo_Max_Pos);  // set servo to "Mid Point"
  TRIM_servo.writeMicroseconds(TRIM_servo_Off_Center_Pos);  // set servo to "Mid Point"
  delay(1000);

  RAD_ALT_servo.writeMicroseconds(RAD_ALT_servo_Hidden_Pos); // set servo to hide point - it may be able to go further
  HYD_LFT_servo.writeMicroseconds(HYD_LFT_servo_Min_Pos);  // set servo to "Mid Point"
  HYD_RHT_servo.writeMicroseconds(HYD_RHT_servo_Min_pos);  // set servo to "Mid Point"
  BATT_U_servo.writeMicroseconds(BATT_U_servo_Min_Pos);  // set servo to "Mid Point"
  BATT_E_servo.writeMicroseconds(BATT_E_servo_Min_Pos);  // set servo to "Mid Point"
  TRIM_servo.writeMicroseconds(TRIM_servo_Center_Pos);  // set servo to "Mid Point"

  delay(500);


  RAD_ALT_servo.detach();
  HYD_RHT_servo.detach();
  HYD_LFT_servo.detach();
  BATT_U_servo.detach();
  BATT_E_servo.detach();
  TRIM_servo.detach();



//
//
//  //###########################################################################################
//  /// RADAR ALT WORKING ======> SET RADAR ALT STEPPER TO 0 FEET
//  stepperRA.setSpeed(50);
//  stepperRA.step(720);       //Reset FULL ON Position
//  stepperRA.step(-720);       //Reset FULL OFF Position
//  posRA = 0;
//  RAD_ALT = map(0, 0, 65000, 720, 10);
//  /// RADAR ALT WORKING ======< SET RADAR ALT STEPPER TO 0 FEET
//
//  /// CABIN ALT WORKING ======> SET CABIN ALT STEPPER TO 0 FEET
//  stepperCA.setSpeed(60);
//  stepperCA.step(700);       //Reset FULL ON Position
//  stepperCA.step(-720);       //Reset FULL OFF Position
//  stepperCA.step(30);       //Reset FULL OFF Position  
//
//  /// CABIN ALT WORKING ======< SET CABIN ALT STEPPER TO 0 FEET

//  /// BRAKE PRESSURE
//  stepperBP.setSpeed(60);
//  stepperBP.step(150);       //Reset FULL ON Position
//  delay(1000);
//  stepperBP.step(-150);       //Reset FULL OFF Position
//  /// BRAKE PRESSURE



  NEXT_PORT_TOGGLE_TIMER = millis() + 1000;
  NEXT_STATUS_TOGGLE_TIMER = millis() + 1000;

  digitalWrite( RED_STATUS_LED_PORT, true);
}


void loop() {

  if (millis() > NEXT_STATUS_TOGGLE_TIMER) {
    GREEN_LED_STATE = !GREEN_LED_STATE;
    digitalWrite( GREEN_STATUS_LED_PORT, GREEN_LED_STATE);
    NEXT_STATUS_TOGGLE_TIMER = millis() + FLASH_TIME;
  }

}
