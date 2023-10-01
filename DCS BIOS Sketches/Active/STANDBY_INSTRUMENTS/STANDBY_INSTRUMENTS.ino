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
#define DCSBIOS_In_Use 1
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

#define TCAADDR 0x70

void tcaselect(uint8_t i) {

  if (i > 7) return;

  Wire.beginTransmission(TCAADDR);
  Wire.write(1 << i);
  Wire.endTransmission();
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

#define BARO_OLED_Port 0
#define ALT_OLED_Port 1
#include <U8g2lib.h>

// Opt OLEDs
U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C u8g2_BARO(U8G2_R0, /* reset=*/U8X8_PIN_NONE);
U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C u8g2_ALT(U8G2_R0, /* reset=*/U8X8_PIN_NONE);


String BaroOnes = "2";
String BaroTens = "9";
String BaroHundreds = "9";
String BaroThousands = "2";
bool BaroUpdated = true;

String Alt1000s = "0";
int LastAlt1000s = 0;
String Alt10000s = "0";
int LastAlt10000s = 0;
bool AltCounterUpdated = true;


String VVI = "0";
int LastVVI = 0;

// Altimeter

unsigned long nextAltimeterUpdate = 0;
int updateAltimeterInterval = 100;
// Original Standby PCB #define ALT_ZERO_SENSE_PIN 49
#define ALT_ZERO_SENSE_PIN 18
#define ALT_OFFSET_TO_ZERO_POINT 708

#include <AccelStepper.h>
#include <Stepper.h>
#define STEPS 720  // steps per revolution (limited to 315°)

// Going in the incorrect direction
// #define  COIL_STANDBY_ALT_A1  41 // STANDBY ALT
// #define  COIL_STANDBY_ALT_A2  43
// #define  COIL_STANDBY_ALT_A3  45
// #define  COIL_STANDBY_ALT_A4  47

// Original Standby PCB #define  COIL_STANDBY_ALT_A1  41 // STANDBY ALT
// Original Standby PCB #define  COIL_STANDBY_ALT_A2  43
// Original Standby PCB #define  COIL_STANDBY_ALT_A3  47
// Original Standby PCB #define  COIL_STANDBY_ALT_A4  45

#define COIL_STANDBY_ALT_A1 38  // STANDBY ALT
#define COIL_STANDBY_ALT_A2 40
#define COIL_STANDBY_ALT_A3 44
#define COIL_STANDBY_ALT_A4 42


int STANDBY_ALT = 0;
int LastSTANDBY_ALT = 0;
unsigned int valAltimeter = 0;
unsigned int posAltimeter = 0;

// AccelStepper stepperSTANDBY_ALT(STEPS, COIL_STANDBY_ALT_A1, COIL_STANDBY_ALT_A2, COIL_STANDBY_ALT_A3, COIL_STANDBY_ALT_A4);
// AccelStepper stepperSTANDBY_ALT(AccelStepper::FULL4WIRE , COIL_STANDBY_ALT_A1, COIL_STANDBY_ALT_A2, COIL_STANDBY_ALT_A3, COIL_STANDBY_ALT_A4);
Stepper stepperSTANDBY_ALT(STEPS, COIL_STANDBY_ALT_A1, COIL_STANDBY_ALT_A2, COIL_STANDBY_ALT_A3, COIL_STANDBY_ALT_A4);


// Airspeed
unsigned long nextAirSpeedUpdate = 0;
int updateAirSpeedInterval = 100;
// Original Standby PCB #define AIRSPEED_ZERO_SENSE_PIN 40
#define AIRSPEED_ZERO_SENSE_PIN A14
#define AIRSPEED_OFFSET_TO_ZERO_POINT 710


// Original Standby PCB #define  COIL_AIRSPEED_A1  32
// Original Standby PCB #define  COIL_AIRSPEED_A2  36
// Original Standby PCB #define  COIL_AIRSPEED_A3  38
// Original Standby PCB #define  COIL_AIRSPEED_A4  34

#define COIL_AIRSPEED_A1 14
#define COIL_AIRSPEED_A2 16
#define COIL_AIRSPEED_A3 17
#define COIL_AIRSPEED_A4 15

int STANDBY_AIRSPEED = 0;
int LastSTANDBY_AIRSPEED = 0;
unsigned int valAIRSPEED = 0;
unsigned int posAIRSPEED = 0;

Stepper stepperSTANDBY_AIRSPEED(STEPS, COIL_AIRSPEED_A1, COIL_AIRSPEED_A2, COIL_AIRSPEED_A3, COIL_AIRSPEED_A4);

#define Check_LED_R 8
#define Check_LED_G 9

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

struct SARIStepperConfig {
  unsigned int SARImaxSteps;
  unsigned int SARIacceleration;
  unsigned int maxSpeed;
};

void onUpdateCounterChange(unsigned int newValue) {
  DCS_State = newValue;
}
DcsBios::IntegerBuffer UpdateCounterBuffer(0xfffe, 0x00ff, 0, onUpdateCounterChange);


void onSaiAttWarnFlagLChange(unsigned int newValue) {
  SARI_Flag = newValue;
}
DcsBios::IntegerBuffer saiAttWarnFlagLBuffer(0x74d6, 0x0100, 8, onSaiAttWarnFlagLChange);

//////SARI - TEST - BEN --------------------------------------------------------------------------------------------------------------
//----------ROLL SERVO----------
DcsBios::ServoOutput saiPitch(0x74e4, 7, 2400, 544);
//Servo ROLL_servo;
//----------ROLL SERVO----------

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
      pinMode(SARIirDetectorPin, INPUT);
      stepper.setMaxSpeed(SARIstepperConfig.maxSpeed);
      stepper.setSpeed(600);  //400
      digitalWrite(SARIenablePin, LOW);
      initState = 1;
      SARIzeroPosSearchStartTime = millis();
      digitalWrite(SARIenablePin, HIGH);
    }

    if (initState == 1) {
      // move off zero if already there so we always get movement on reset
      // (to verify that the stepper is working)
      if (SARIzeroDetected()) {
        digitalWrite(SARIenablePin, LOW);
        stepper.runSpeed();
      } else {
        initState = 2;
        digitalWrite(SARIenablePin, HIGH);
      }
    }

    if (initState == 2) {  // zeroing
      if (!SARIzeroDetected()) {

        if (millis() >= (zeroTimeout + SARIzeroPosSearchStartTime)) {
          digitalWrite(SARIenablePin, HIGH);
          initState == 99;
        } else
          digitalWrite(SARIenablePin, LOW);
        stepper.runSpeed();

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
        digitalWrite(SARIenablePin, HIGH);
      }
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
          digitalWrite(SARIenablePin, LOW);
        }  // LOW  = stepper ON drive current available
        else {
          digitalWrite(SARIenablePin, HIGH);
        }  // HIGH  = stepper OFF no drive current

        // tell AccelStepper to move relative to the current position
        stepper.move(delta);
      }
      stepper.run();
    }
    if (initState == 99) {  // Timed out looking for zero do nothing
      digitalWrite(SARIenablePin, HIGH);
    }
  }
};

struct SARIStepperConfig SARIstepperConfig = {
  1600,  // SARImaxSteps //200STEPS X 1/8 MICRO STEPPING
  3600,  // maxSpeed //3200
  3200   // SARIacceleration
};
const int SARIstepPin = 47;
const int SARIdirectionPin = 45;

// define AccelStepper instance

AccelStepper SARIstepperRoll(AccelStepper::DRIVER, SARIstepPin, SARIdirectionPin);

Nema8Stepper SARIRoll(0x74e6,             // address of stepper data
                      SARIstepperRoll,    // name of AccelStepper instance
                      SARIstepperConfig,  // SARIStepperConfig struct instance
                      23,                 // IR Detector Pin (must be LOW in zero position)
                      800,                // zero offset (SET TO 50% of MaX Steps) 1650
                                          // WIngs Level = 1/2 Max Steps -> "Zero" = 1/2 Turn
                                          // SARI will be upsidedown after Setup, this will correct in Bios
                      [](unsigned int newValue) -> unsigned int {
                        newValue = newValue & 0xffff;
                        return map(newValue, 0, 65535, 0, SARIstepperConfig.SARImaxSteps - 1);
                      });




void onSaiManPitchAdjChange(unsigned int newValue) {
  valWAT = map(newValue, 0, 65535, 0, 1500);
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
    valVER = map(newValue, 1, 65535, 1500, 3600);
    stepperV.moveTo(valVER);
  }
}
DcsBios::IntegerBuffer saiPointerVerBuffer(0x756a, 0xffff, 0, onSaiPointerVerChange);

DcsBios::RotaryEncoder saiSet("SAI_SET", "-800", "+800", 28, 26);

DcsBios::Switch2Pos saiTestBtn("SAI_TEST_BTN", A12);
DcsBios::Switch2Pos saiCage("SAI_CAGE", A13);




// VVI
unsigned long nextVVIUpdate = 0;
int updateVVIInterval = 100;
// Original Standby PCB #define VVI_ZERO_SENSE_PIN 30
#define VVI_ZERO_SENSE_PIN A15
#define VVI_OFFSET_TO_ZERO_POINT 392

// Original Standby PCB #define  COIL_VVI_A1  22
// Original Standby PCB #define  COIL_VVI_A2  26
// Original Standby PCB #define  COIL_VVI_A3  28
// Original Standby PCB #define  COIL_VVI_A4  24

#define COIL_VVI_A1 30
#define COIL_VVI_A2 34
#define COIL_VVI_A3 36
#define COIL_VVI_A4 32

int STANDBY_VVI = 0;
int LastSTANDBY_VVI = 0;
long valVVI = 0;
long posVVI = 0;

Stepper stepperSTANDBY_VVI(STEPS, COIL_VVI_A1, COIL_VVI_A2, COIL_VVI_A3, COIL_VVI_A4);



void SendDebug(String MessageToSend) {
  if ((Reflector_In_Use == 1) && (Ethernet_In_Use == 1)) {
    udp.beginPacket(reflectorIP, reflectorport);
    udp.println(MessageToSend);
    udp.endPacket();
  }
}


// Original Standby PCB DcsBios::RotaryEncoder saiSet("SAI_SET", "-3200", "+3200", 23, 25);
// Original Standby PCB DcsBios::Switch2Pos saiTestBtn("SAI_TEST_BTN", 29);
// Original Standby PCB DcsBios::Switch2Pos saiCage("SAI_CAGE", 27);
// Original Standby PCB DcsBios::RotaryEncoder stbyPressAlt("STBY_PRESS_ALT", "-3200", "+3200", 37, 39);



DcsBios::RotaryEncoder stbyPressAlt("STBY_PRESS_ALT", "-3200", "+3200", 46, 48);
DcsBios::Potentiometer rwrRwrIntesity("RWR_RWR_INTESITY", 0);
void setup() {

  pinMode(RedLedMonitorPin, OUTPUT);
  pinMode(GreenLedMonitorPin, OUTPUT);
  outputstate = true;
  digitalWrite(RedLedMonitorPin, outputstate);
  digitalWrite(GreenLedMonitorPin, outputstate);


  STANDBY_ALT = 0;
  STANDBY_AIRSPEED = 0;

  outputstate = false;
  digitalWrite(RedLedMonitorPin, outputstate);
  digitalWrite(GreenLedMonitorPin, outputstate);


  pinMode(SARI_Flag_Pin, OUTPUT);

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


  DcsBios::setup();






  Wire.begin();

  for (uint8_t t = 0; t < 8; t++) {
    SendDebug("TCA Port #" + String(t));
    tcaselect(t);

    for (uint8_t addr = 0; addr <= 127; addr++) {
      //if (addr == TCAADDR) continue;

      uint8_t data;
      if (!twi_writeTo(addr, &data, 0, 1, 1)) {
        // Had to comment out these debugging messages as they created a conflict with the IRQ definition in DCS BIOS
        DebugString = "Found I2C 0x";
        DebugString.concat(String(addr, HEX));
        SendDebug(DebugString);
      }
    }
  }
  SendDebug("I2C scan complete");


  nextupdate = millis() + flashinterval;


  tcaselect(BARO_OLED_Port);
  u8g2_BARO.begin();
  u8g2_BARO.clearBuffer();
  //u8g2_BARO.setFont(u8g2_DcsFontHornet4_BIOS_09_tf);
  u8g2_BARO.setFont(u8g2_font_fub14_tr);
  u8g2_BARO.sendBuffer();


  tcaselect(ALT_OLED_Port);
  u8g2_ALT.begin();
  u8g2_ALT.clearBuffer();
  //u8g2_ALT.setFont(u8g2_DcsFontHornet4_BIOS_09_tf);
  //u8g2_ALT.setFont(u8g2_font_t0_11_t_all);
  u8g2_ALT.setFont(u8g2_font_fub20_tr);
  u8g2_ALT.sendBuffer();

  updateALT("0", "0");
  updateBARO("2992");

  SendDebug("Looking for Altimeter Zero");
  pinMode(ALT_ZERO_SENSE_PIN, INPUT_PULLUP);


  stepperSTANDBY_ALT.setSpeed(60);
  stepperSTANDBY_ALT.step(1000);
  for (int i = 0; i <= 2000; i++) {
    delay(1);
    stepperSTANDBY_ALT.step(1);
    if (digitalRead(ALT_ZERO_SENSE_PIN) == false) {
      SendDebug("Found Zero");
      stepperSTANDBY_ALT.step(ALT_OFFSET_TO_ZERO_POINT);
      posAltimeter = 0;
      break;
    }
  }

  SendDebug("Looking for Airspeed Zero");
  pinMode(AIRSPEED_ZERO_SENSE_PIN, INPUT_PULLUP);


  stepperSTANDBY_AIRSPEED.setSpeed(60);
  stepperSTANDBY_AIRSPEED.step(1000);
  for (int i = 0; i <= 2000; i++) {
    delay(1);
    stepperSTANDBY_AIRSPEED.step(1);
    if (digitalRead(AIRSPEED_ZERO_SENSE_PIN) == false) {
      SendDebug("Found Airspeed Zero");
      stepperSTANDBY_AIRSPEED.step(AIRSPEED_OFFSET_TO_ZERO_POINT);
      posAIRSPEED = 0;
      break;
    }
  }

  SendDebug("Looking for VVI Zero");
  pinMode(VVI_ZERO_SENSE_PIN, INPUT_PULLUP);


  stepperSTANDBY_VVI.setSpeed(60);
  stepperSTANDBY_VVI.step(1000);
  for (int i = 0; i <= 2000; i++) {
    delay(1);
    stepperSTANDBY_VVI.step(1);
    if (digitalRead(VVI_ZERO_SENSE_PIN) == false) {
      SendDebug("Found VVI Zero");
      // Set to 0 point which is -6000
      stepperSTANDBY_VVI.step(VVI_OFFSET_TO_ZERO_POINT);
      posVVI = 0;
      // Set desired point to 0
      valVVI = map(32767, 0, 65535, 0, 660);
      break;
    }
  }


  nextAltimeterUpdate = millis();

  pinMode(Check_LED_G, OUTPUT);
  pinMode(Check_LED_R, OUTPUT);
  pinMode(SARIenablePin, OUTPUT);

  digitalWrite(EN_switchW, LOW);

  pinMode(HOZ_IR_sen, INPUT_PULLUP);
  pinMode(EN_switchH, OUTPUT);

  pinMode(VER_IR_sen, INPUT_PULLUP);
  pinMode(EN_switchV, OUTPUT);

  pinMode(WTR_IR_sen, INPUT_PULLUP);
  pinMode(EN_switchW, OUTPUT);
  delay(5);  // Wait for EasyDriver wake up

  //  Set Max Speed and Acceleration of each Steppers at startup for homing
  stepperH.setMaxSpeed(3000.0);      // Set Max Speed of Stepper (Slower to get better accuracy)
  stepperH.setAcceleration(2000.0);  // Set Acceleration of Stepper

  stepperV.setMaxSpeed(3000.0);      // Set Max Speed of Stepper (Slower to get better accuracy)
  stepperV.setAcceleration(2000.0);  // Set Acceleration of Stepper

  stepperW.setMaxSpeed(3000.0);      // Set Max Speed of Stepper (Slower to get better accuracy)
  stepperW.setAcceleration(2000.0);  // Set Acceleration of Stepper
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // WATER START Homing procedure of Stepper Motor at startup

  SendDebug("Water Stepper is Homing . . . ");
  enable_switchW();
  if (digitalRead(WTR_IR_sen) == LOW)
    SendDebug("Water Stepper is IN the SENSOR . . . ");
  if (digitalRead(WTR_IR_sen) == HIGH)
    SendDebug("Water Stepper is OUT of the SENSOR  . . . ");
  while (digitalRead(WTR_IR_sen)) {    // Make the Stepper move CCW until the switch is activated
    stepperW.moveTo(initial_homingW);  // Set the position to move to
    initial_homingW++;                 // Decrease by 1 for next move if needed
    stepperW.run();                    // Start moving the stepper
    delay(1);
  }
  initial_homingW = 1;

  while (!digitalRead(WTR_IR_sen)) {  // Make the Stepper move CW until the switch is deactivated
    stepperW.moveTo(initial_homingW);
    stepperW.run();
    initial_homingW--;
    delay(1);
  }

  stepperW.setCurrentPosition(400);  //750
  //Serial.println("Water Homing Completed");
  //Serial.println("");
  stepperW.setMaxSpeed(3000.0);      // Set Max Speed of Stepper (Faster for regular movements)
  stepperW.setAcceleration(2000.0);  // Set Acceleration of Stepper
  disableAllPointers();
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // HOR Start Homing procedure of Stepper Motor at startup

  SendDebug("Stepper is HOZ Homing . . . ");
  enable_switchH();
  if (digitalRead(HOZ_IR_sen) == LOW)
    SendDebug("Stepper is IN the SENSOR . . . ");
  if (digitalRead(HOZ_IR_sen) == HIGH)
    SendDebug("Stepper is OUT of the SENSOR  . . . ");
  while (digitalRead(HOZ_IR_sen)) {    // Make the Stepper move CCW until the switch is activated
    stepperH.moveTo(initial_homingH);  // Set the position to move to
    initial_homingH--;                 // Decrease by 1 for next move if needed
    stepperH.run();                    // Start moving the stepper
    delay(1);
  }
  initial_homingH = 1;

  while (!digitalRead(HOZ_IR_sen)) {  // Make the Stepper move CW until the switch is deactivated
    stepperH.moveTo(initial_homingH);
    stepperH.run();
    initial_homingH++;
    delay(1);
  }

  stepperH.setCurrentPosition(800);
  SendDebug("Homing Completed");
  //Serial.println("");
  stepperH.setMaxSpeed(3000.0);      // Set Max Speed of Stepper (Faster for regular movements)
  stepperH.setAcceleration(2000.0);  // Set Acceleration of Stepper

  disableAllPointers();

  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  SendDebug("Stepper VER is Homing . . . ");

  stepperV.setMaxSpeed(800.0);      // Set Max Speed of Stepper (Faster for regular movements)
  stepperV.setAcceleration(200.0);  // Set Acceleration of Stepper


  enable_switchV();

  if (digitalRead(VER_IR_sen) == HIGH) {
    SendDebug("VER Stepper is OUT of the SENSOR  . . . ");

    initial_homingV = 4000;  // Set the maximum number of steps we could be moving before hitting the sensor
    stepperV.moveTo(initial_homingV);
    while (digitalRead(VER_IR_sen)) {  // Make the Stepper move CCW until the switch is activated
      // SendDebug("Moving V forward");
      stepperV.run();  // Set the position to move to
    }

    SendDebug("Moved " + String(initial_homingV) + " steps");
    SendDebug("Steps remaining is " + String(stepperV.distanceToGo()));

    disableAllPointers();
    delay(300);  // Let the ponter settle for a short time
  }

  if (digitalRead(VER_IR_sen) == LOW)
    SendDebug("VER Stepper is IN the SENSOR . . . ");

  enable_switchV();
  initial_homingV = 1;


  stepperV.setCurrentPosition(0);     // Unless we've just moved the sensor into the sensor
  initial_homingV = -4000;            // We don't know the absolute position - so start at 0
  stepperV.moveTo(initial_homingV);   // And the set max steps we should move before hitting a hard stop
  while (!digitalRead(VER_IR_sen)) {  // Make the Stepper move CW until the sensor is deactivated
    stepperV.run();
  }
  SendDebug("Moved " + String(initial_homingV) + " steps");
  SendDebug("Steps remaining is " + String(stepperV.distanceToGo()));


  stepperV.setCurrentPosition(3000);  // 0 is full LEFT <-> HIGHER IS RIGHT
  SendDebug("VER Homing Completed");

  stepperV.setMaxSpeed(3000.0);      // Set Max Speed of Stepper (Faster for regular movements)
  stepperV.setAcceleration(2000.0);  // Set Acceleration of Stepper

  disableAllPointers();
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


  SendDebug("ALL CALIBRATION COMPLETE");

  movePointersToRestPosition();

  SendDebug("Move pointers to center");
  stepperV.moveTo(2900);     //    --- HIGHER IS RIGHT  <->   LOWER IS LEFT ---
  stepperH.moveTo(2500);     //    --- HIGHER IS DOWN   <->   LOWWER IS UP ---
  enableAllPointers();
  while ((stepperW.distanceToGo() != 0) || (stepperV.distanceToGo() != 0) || (stepperH.distanceToGo() != 0)) {
    stepperV.run();
    stepperH.run();
  }
  disableAllPointers();
  delay(5000);

  movePointersToRestPosition();
}


void movePointersToRestPosition() {
  SendDebug("Returning Pointers to rest position");
  stepperW.moveTo(1000);  //  --- HIGHER IS UP    <->   LOWER IS DOWN---
  stepperV.moveTo(0);     //    --- HIGHER IS RIGHT  <->   LOWER IS LEFT ---
  stepperH.moveTo(0);     //    --- HIGHER IS DOWN   <->   LOWWER IS UP ---

  enableAllPointers();
  while ((stepperW.distanceToGo() != 0) || (stepperV.distanceToGo() != 0) || (stepperH.distanceToGo() != 0)) {
    stepperW.run();
    stepperV.run();
    stepperH.run();
  }
  disableAllPointers();
  SendDebug("Completed returning Pointers to rest position");
}


void enable_switchW() {
  digitalWrite(EN_switchW, LOW);
  setStepperLedOn();
}

void enable_switchV() {
  digitalWrite(EN_switchV, LOW);
  setStepperLedOn();
}

void enable_switchH() {
  digitalWrite(EN_switchH, LOW);
  setStepperLedOn();
}


void disable_switchW() {
  digitalWrite(EN_switchW, HIGH);
  checkStepperDisabledStatus();
}

void disable_switchV() {
  digitalWrite(EN_switchV, HIGH);
  checkStepperDisabledStatus();
}

void disable_switchH() {
  digitalWrite(EN_switchH, HIGH);
  checkStepperDisabledStatus();
}

void disableAllPointers() {
  digitalWrite(EN_switchW, HIGH);
  digitalWrite(EN_switchV, HIGH);
  digitalWrite(EN_switchH, HIGH);
  digitalWrite(SARIenablePin, HIGH);
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
  if ((digitalRead(EN_switchW) == HIGH) && (digitalRead(EN_switchV) == HIGH) && (digitalRead(EN_switchH) == HIGH) && (digitalRead(SARIenablePin) == HIGH)) {
    setStepperLedOff();  // CHECK ALL STEPPER DRIVERS ARE DISABLED BEFORE CHANGING LED STATE
  }
}



void updateBARO(String strnewValue) {

  const char* newValue = strnewValue.c_str();
  tcaselect(BARO_OLED_Port);
  u8g2_BARO.setFontMode(0);
  u8g2_BARO.setDrawColor(0);
  u8g2_BARO.drawBox(0, 0, 128, 32);
  u8g2_BARO.setDrawColor(1);
  u8g2_BARO.setFontDirection(2);
  u8g2_BARO.drawStr(115, 0, newValue);
  u8g2_BARO.sendBuffer();
}

void buildBAROString() {

  updateBARO(BaroThousands + BaroHundreds + BaroTens + BaroOnes);
  BaroUpdated = false;
}



void updateALT(String strTenThousands, String strnewThousands) {

  const char* newTenThousandsValue = strTenThousands.c_str();
  const char* newThousandsValue = strnewThousands.c_str();
  int End_X_Pos = 46;
  int End_Y_Pos = 28;
  int Start_Y_Pos = 13;
  int Start_X_Pos = 27;
  int Box_Width = 20;
  tcaselect(ALT_OLED_Port);
  u8g2_ALT.setFontMode(0);
  u8g2_ALT.setDrawColor(0);
  u8g2_ALT.drawBox(0, 0, 128, 32);
  u8g2_ALT.setDrawColor(1);
  //u8g2_ALT.drawStr(5, 32, newValue);

  if (strTenThousands == "0") {
    u8g2_ALT.setDrawColor(1);

    u8g2_ALT.drawBox(Start_X_Pos, 13, Box_Width, 20);
    u8g2_ALT.setDrawColor(0);

    u8g2_ALT.drawLine(Start_X_Pos, Start_Y_Pos, End_X_Pos, 32);
    u8g2_ALT.drawLine(Start_X_Pos, Start_Y_Pos + 1, End_X_Pos - 1, 32);
    u8g2_ALT.drawLine(Start_X_Pos, Start_Y_Pos + 2, End_X_Pos - 2, 32);
    u8g2_ALT.drawLine(Start_X_Pos, Start_Y_Pos + 3, End_X_Pos - 3, 32);
    u8g2_ALT.drawLine(Start_X_Pos, Start_Y_Pos + 4, End_X_Pos - 4, 32);
    u8g2_ALT.drawLine(Start_X_Pos, Start_Y_Pos + 5, End_X_Pos - 5, 32);
    u8g2_ALT.drawLine(Start_X_Pos, Start_Y_Pos + 6, End_X_Pos - 6, 32);

    u8g2_ALT.drawLine(Start_X_Pos, Start_Y_Pos + 11, End_X_Pos - 11, 32);
    u8g2_ALT.drawLine(Start_X_Pos, Start_Y_Pos + 12, End_X_Pos - 12, 32);
    u8g2_ALT.drawLine(Start_X_Pos, Start_Y_Pos + 13, End_X_Pos - 13, 32);
    u8g2_ALT.drawLine(Start_X_Pos, Start_Y_Pos + 14, End_X_Pos - 14, 32);
    u8g2_ALT.drawLine(Start_X_Pos, Start_Y_Pos + 15, End_X_Pos - 15, 32);

    u8g2_ALT.drawLine(Start_X_Pos + 4, Start_Y_Pos, Start_X_Pos + Box_Width, End_Y_Pos + 1);
    u8g2_ALT.drawLine(Start_X_Pos + 5, Start_Y_Pos, Start_X_Pos + Box_Width, End_Y_Pos);
    u8g2_ALT.drawLine(Start_X_Pos + 6, Start_Y_Pos, Start_X_Pos + Box_Width, End_Y_Pos - 1);
    u8g2_ALT.drawLine(Start_X_Pos + 7, Start_Y_Pos, Start_X_Pos + Box_Width, End_Y_Pos - 2);
    u8g2_ALT.drawLine(Start_X_Pos + 8, Start_Y_Pos, Start_X_Pos + Box_Width, End_Y_Pos - 3);
    u8g2_ALT.drawLine(Start_X_Pos + 9, Start_Y_Pos, Start_X_Pos + Box_Width, End_Y_Pos - 4);
    u8g2_ALT.drawLine(Start_X_Pos + 10, Start_Y_Pos, Start_X_Pos + Box_Width, End_Y_Pos - 5);

    u8g2_ALT.drawLine(Start_X_Pos + 15, Start_Y_Pos, Start_X_Pos + Box_Width, End_Y_Pos - 10);
    u8g2_ALT.drawLine(Start_X_Pos + 16, Start_Y_Pos, Start_X_Pos + Box_Width, End_Y_Pos - 11);
    u8g2_ALT.drawLine(Start_X_Pos + 17, Start_Y_Pos, Start_X_Pos + Box_Width, End_Y_Pos - 12);
    u8g2_ALT.drawLine(Start_X_Pos + 18, Start_Y_Pos, Start_X_Pos + Box_Width, End_Y_Pos - 13);
    u8g2_ALT.drawLine(Start_X_Pos + 19, Start_Y_Pos, Start_X_Pos + Box_Width, End_Y_Pos - 14);

    u8g2_ALT.setDrawColor(1);


  } else {
    u8g2_ALT.drawStr(32, 32, newTenThousandsValue);
  }

  u8g2_ALT.drawStr(65, 32, newThousandsValue);
  u8g2_ALT.sendBuffer();

  AltCounterUpdated = false;
}



void onStbyAlt100FtPtrChange(unsigned int newValue) {
  //  SendDebug("100 foot pointer " + String(newValue));
  //  SendDebug("adding drum counters -v2 " + String(LastAlt10000s) + " " + String(LastAlt1000s));
  //
  //  unsigned int targetLocation = int(newValue/91) + (LastAlt10000s * 10 * 720) + (LastAlt1000s * 720);
  //  SendDebug(String(targetLocation));
  //  stepperSTANDBY_ALT.runToNewPosition(targetLocation);
}
DcsBios::IntegerBuffer stbyAlt100FtPtrBuffer(0x74f4, 0xffff, 0, onStbyAlt100FtPtrChange);


//void onStbyAlt1000FtCntChange(unsigned int newValue) {
////  // SendDebug("Alt 1K");
////  Alt1000s = String(newValue);
////  SendDebug(String(newValue));
////
////  if (newValue < 6553 ) Alt1000s = "0";
////  else if ( newValue < 13106 ) Alt1000s = "1" ;
////  else if ( newValue < 16301 ) Alt1000s = "2" ;
////  else if ( newValue < 19660 ) Alt1000s = "3" ;
////  else if ( newValue < 29918 ) Alt1000s = "4" ;
////  else if ( newValue < 36727 ) Alt1000s = "5" ;
////  else if ( newValue < 43536 ) Alt1000s = "6" ;
////  else if ( newValue < 50345 ) Alt1000s = "7" ;
////  else if ( newValue < 53284 ) Alt1000s = "8" ;
////  else Alt1000s = "9" ;
////
////  AltCounterUpdated = true;
//
//}
//DcsBios::IntegerBuffer stbyAlt1000FtCntBuffer(0x74f8, 0xffff, 0, onStbyAlt1000FtCntChange);

//void onStbyAlt10000FtCntChange(unsigned int newValue) {
//  SendDebug("Alt 10K");
//
//  SendDebug(String(newValue));
//
//  if (newValue < 6553 ) Alt10000s = "0";
//  else if ( newValue < 13106 ) Alt10000s = "1" ;
//  else if ( newValue < 16301 ) Alt10000s = "2" ;
//  else if ( newValue < 19660 ) Alt10000s = "3" ;
//  else if ( newValue < 29918 ) Alt10000s = "4" ;
//  else if ( newValue < 36727 ) Alt10000s = "5" ;
//  else if ( newValue < 43536 ) Alt10000s = "6" ;
//  else if ( newValue < 50345 ) Alt10000s = "7" ;
//  else if ( newValue < 53284 ) Alt10000s = "8" ;
//  else Alt10000s = "9" ;
//
//  AltCounterUpdated = true;
//}
//DcsBios::IntegerBuffer stbyAlt10000FtCntBuffer(0x74f6, 0xffff, 0, onStbyAlt10000FtCntChange);

void onAltMslFtChange(unsigned int newValue) {

  int updateAltimeterInterval = 100;
  unsigned int tempvar = 0;
  int Alt100s = 0;

  tempvar = int((newValue % 10000) / 1000);


  if (tempvar != LastAlt1000s) {
    LastAlt1000s = tempvar;
    Alt1000s = String(tempvar);
    AltCounterUpdated = true;
  }

  tempvar = int((newValue % 100000) / 10000);
  if (tempvar != LastAlt10000s) {
    LastAlt10000s = tempvar;
    Alt10000s = String(tempvar);
    AltCounterUpdated = true;
  }


  //unsigned int targetLocation = int (newValue * 0.72);
  valAltimeter = map(newValue, 0, 65000, 0, 46800);  // 46800 = 720 * 65

  //SendDebug("Alt " + String(newValue) + " Translated Rate" + String(valAltimeter));
  // SendDebug("Pointer Target = " + String(targetLocation));
  //stepperSTANDBY_ALT.runToNewPosition(targetLocation);
}
DcsBios::IntegerBuffer altMslFtBuffer(0x0434, 0xffff, 0, onAltMslFtChange);



void onVsiChange(unsigned int newValue) {
  // VVI Range -6000 to +6000
  // 65535  +6000
  // 32768  0
  // 0      -6000


  valVVI = map(newValue, 0, 65535, 0, 680);
  //valVVI = CalculateVVIPosition(VSI);

  //SendDebug("VVI from DCS: " + String(newValue) + "  Mapped:" + String(VSI)
  //          + " Servo Pos: " + String(valVVI));
}


DcsBios::IntegerBuffer vsiBuffer(0x7500, 0xffff, 0, onVsiChange);


void onStbyPressSet0Change(unsigned int newValue) {
  //SendDebug(String(newValue));
  if (newValue < 6553) BaroOnes = "0";
  else if (newValue < 13106) BaroOnes = "1";
  else if (newValue < 16301) BaroOnes = "2";
  else if (newValue < 19660) BaroOnes = "3";
  else if (newValue < 29918) BaroOnes = "4";
  else if (newValue < 36727) BaroOnes = "5";
  else if (newValue < 43536) BaroOnes = "6";
  else if (newValue < 50345) BaroOnes = "7";
  else if (newValue < 53284) BaroOnes = "8";
  else BaroOnes = "9";


  BaroUpdated = true;

  //SendDebug(String(BaroOnes));
}
DcsBios::IntegerBuffer stbyPressSet0Buffer(0x74fa, 0xffff, 0, onStbyPressSet0Change);



// Display for Pressure ranges from
// Disp Press 1 Press 2 Presso 3
// 3100 0       5891    unknown
// 2810 0       6553    0


void onStbyPressSet1Change(unsigned int newValue) {
  //SendDebug(String(newValue));
  if (newValue < 6553) BaroTens = "0";
  else if (newValue < 13106) BaroTens = "1";
  else if (newValue < 16301) BaroTens = "2";
  else if (newValue < 19660) BaroTens = "3";
  else if (newValue < 29918) BaroTens = "4";
  else if (newValue < 36727) BaroTens = "5";
  else if (newValue < 43536) BaroTens = "6";
  else if (newValue < 50345) BaroTens = "7";
  else if (newValue < 53284) BaroTens = "8";
  else BaroTens = "9";


  BaroUpdated = true;

  //SendDebug(String(BaroTens));
}
DcsBios::IntegerBuffer stbyPressSet1Buffer(0x74fc, 0xffff, 0, onStbyPressSet1Change);



// There is a bug here - the value doesn't change
void onStbyPressSet2Change(unsigned int newValue) {
  //SendDebug(String(newValue));
}
DcsBios::IntegerBuffer stbyPressSet2Buffer(0x74fe, 0xffff, 0, onStbyPressSet2Change);



void onStbyAsiAirspeedChange(unsigned int newValue) {
  valAIRSPEED = map(newValue, 0, 65535, 0, 660);
}
DcsBios::IntegerBuffer stbyAsiAirspeedBuffer(0x74f0, 0xffff, 0, onStbyAsiAirspeedChange);



int CalculateAirspeedPosition(int newValue) {

  int newPosition = 0;

  // Offset Values
  // 60  Knots 30
  // 100 Knots 100
  // 150 Knots 230
  // 200 Knots 408
  // 300 Knots 478
  // 400 Knots 525
  // 500 Knots 567
  // 600 Knots 601
  // 700 Knots 634
  // 800 Knots 673
  // 850 Knots 688

#define Pos60Knot 30
#define Pos100Knot 100
#define Pos150Knot 230
#define Pos200Knot 408
#define Pos300Knot 478
#define Pos400Knot 525
#define Pos500Knot 567
#define Pos600Knot 601
#define Pos700Knot 634
#define Pos800Knot 673
#define Pos850Knot 688

  if (newValue <= 60) newPosition = map(newValue, 0, 60, 0, Pos60Knot);
  else if (newValue <= 100) newPosition = map(newValue, 60, 100, Pos60Knot, Pos100Knot);
  else if (newValue <= 150) newPosition = map(newValue, 100, 150, Pos100Knot, Pos150Knot);
  else if (newValue <= 200) newPosition = map(newValue, 150, 200, Pos150Knot, Pos200Knot);
  else if (newValue <= 300) newPosition = map(newValue, 200, 300, Pos200Knot, Pos300Knot);
  else if (newValue <= 400) newPosition = map(newValue, 300, 400, Pos300Knot, Pos400Knot);
  else if (newValue <= 500) newPosition = map(newValue, 400, 500, Pos400Knot, Pos500Knot);
  else if (newValue <= 600) newPosition = map(newValue, 500, 600, Pos500Knot, Pos600Knot);
  else if (newValue <= 700) newPosition = map(newValue, 600, 700, Pos600Knot, Pos700Knot);
  else if (newValue <= 800) newPosition = map(newValue, 700, 800, Pos700Knot, Pos800Knot);
  else if (newValue <= 850) newPosition = map(newValue, 800, 150, Pos800Knot, Pos850Knot);

  // SendDebug("Returning from CalculateAirSpeedPosition: " + String(newPosition));
  return (newPosition);
}


int CalculateVVIPosition(int newValue) {

  // Range -6000 to +6000
  int newPosition = 0;

  // Offset Values
  // 1000 150
  // 2000 220
  // 4000 280
  // 6000 300
#define Pos1000 150
#define Pos2000 220
#define Pos4000 280
#define Pos6000 300


  if (newValue <= -6000) newPosition = -Pos6000;
  else if (newValue <= -4000) newPosition = map(newValue, -4000, -6000, -Pos4000, -Pos6000);
  else if (newValue <= -2000) newPosition = map(newValue, -2000, -4000, -Pos2000, -Pos4000);
  else if (newValue <= -1000) newPosition = map(newValue, -1000, -2000, -Pos1000, -Pos2000);
  else if (newValue <= 0) newPosition = map(newValue, 0, -1000, 0, -Pos1000);
  else if (newValue <= 1000) newPosition = map(newValue, 0, 1000, 0, Pos1000);
  else if (newValue <= 2000) newPosition = map(newValue, 1000, 2000, Pos1000, Pos2000);
  else if (newValue <= 4000) newPosition = map(newValue, 2000, 4000, Pos2000, Pos4000);
  else if (newValue <= 6000) newPosition = map(newValue, 4000, 6000, Pos4000, Pos6000);
  else if (newValue >= 6000) newPosition = Pos6000;

  // SendDebug("Returning from CalculateVVIPosition: " + String(newPosition));
  return (newPosition);
}




void loop() {


  DcsBios::loop();

  // Standby Altimeter
  //###########################################################################################

  if (valAltimeter > posAltimeter) {
    stepperSTANDBY_ALT.step(1);  // move one step to the left.
    posAltimeter++;
    // SendDebug(String(valAltimeter) + " Inc v2 Altimeter " + String(posAltimeter));
  } else if (valAltimeter < posAltimeter) {
    stepperSTANDBY_ALT.step(-1);  // move one step to the right.
    posAltimeter--;
    //SendDebug(String(valAltimeter) + " Dec v2 Altimeter " + String(posAltimeter));
  }

  //###########################################################################################


  // VVI
  //###########################################################################################

  if (valVVI > posVVI) {
    stepperSTANDBY_VVI.step(1);  // move one step to the left.
    posVVI++;
    // SendDebug(String(valVVI) + " Inc VVI " + String(posVVI));
  } else if (valVVI < posVVI) {
    stepperSTANDBY_VVI.step(-1);  // move one step to the right.
    posVVI--;
    //SendDebug(String(valVVI) + " Dec VVI " + String(posVVI));
  }

  //###########################################################################################


  // Airspeed
  //###########################################################################################

  if (valAIRSPEED > posAIRSPEED) {
    stepperSTANDBY_AIRSPEED.step(1);  // move one step to the left.
    posAIRSPEED++;
    // SendDebug(String(valAIRSPEED) + " Inc Airspeed " + String(posAIRSPEED));
  } else if (valAIRSPEED < posAIRSPEED) {
    stepperSTANDBY_AIRSPEED.step(-1);  // move one step to the right.
    posAIRSPEED--;
    //SendDebug(String(valAIRSPEED) + " Dec Airspeed " + String(posAIRSPEED));
  }

  //###########################################################################################



  if (BaroUpdated == true) buildBAROString();
  if (AltCounterUpdated == true) updateALT(Alt10000s, Alt1000s);

  //###########################################################################################
  // SARI_ROLL SET IN CODE ABOVE
  //###########################################################################################



  if (stepperW.distanceToGo() != 0) {
    enable_switchW();  // turn stepper ON
    stepperW.run();
  } else {
    disable_switchW();
  }

  if (stepperH.distanceToGo() != 0) {
    enable_switchH();  // turn stepper ON
    stepperH.run();
  } else {
    disable_switchH();
  }

  if (stepperV.distanceToGo() != 0) {
    enable_switchV();  // turn stepper ON
    stepperV.run();
  } else {
    disable_switchV();
  }


  //******************************************************************************************************************************************
  //         STEPPER SAFETY CHECK CODE, WILL NOT TUNR STEPPERS ON IF DCS BIOS NOT RUNNING, OR IF GAME PAUSED AFTER 1 SECOND   \\

  if (DCS_State != previous_DCS_State) {
    //restart the TIMER
    dcsMillis = millis();
    digitalWrite(Check_LED_R, HIGH);  // RED LED ON PCB TO SHOW ENABLE SWITCHES ARE ACTIVE AND POWER IS ON TO THE STEPPERS IF REQUIRED - SET BY CODE
    digitalWrite(Check_LED_G, LOW);
    previous_DCS_State = DCS_State;
  }


  if (millis() - dcsMillis >= (1000))  // 1 SECOND DELAY BEFORE POWER OFF STEPPERS
  {
    SARI_Flag = 1;  // Actual driving of the output is done futher down in the loop
    disableAllPointers();
  }

  //         STEPPER SAFETY CHECK CODE, WILL NOT TUNR STEPPERS ON IF DCS BIOS NOT RUNNING, OR IF GAME PAUSED AFTER 1 SECOND   \\
  //******************************************************************************************************************************************

  if (SARI_Flag == 1) {
    digitalWrite(SARI_Flag_Pin, LOW);
  } else {
    digitalWrite(SARI_Flag_Pin, HIGH);
  }



  // if (millis() >= nextupdate) {
  //   outputstate = !outputstate;
  //   digitalWrite(RedLedMonitorPin, outputstate);
  //   nextupdate = millis() + flashinterval;
  // }
}
