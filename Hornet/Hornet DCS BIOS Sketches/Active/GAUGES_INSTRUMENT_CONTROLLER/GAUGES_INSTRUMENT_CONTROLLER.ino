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
    udp.println(MessageToSend);
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

#define DIMMER 11
#define RAD_GN 9
#define RAD_RD 8

Servo RAD_ALT_servo;
#define RadarAltServoPin 5
#define RAD_ALT_servo_Off_Pos 90     // OFF FLAG IN WINDOW
#define RAD_ALT_servo_Hidden_Pos 50  // OFF FLAG NOT SHOWN

#include <Stepper.h>
#define STEPS 720  // steps per revolution (limited to 315°)

#define COILRA1 2  // RA = RAD ALT
#define COILRA2 3
#define COILRA3 5
#define COILRA4 6

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

#define COILBP1 42  // BP = BRAKE PRESSURE
#define COILBP2 43
#define COILBP3 44
#define COILBP4 45

#define COILCP1 46  // CP = COMPASS
#define COILCP2 47
#define COILCP3 48
#define COILCP4 49

int RAD_ALT = 0;
int valRA = 0;
int posRA = 0;

int CAB_ALT;
int valCA = 0;
int CurCABALT = 0;
int posCA = 0;

int BRAKE_PRESSURE = 0;
int valBP = 0;
int posBP = 0;

int BATT1 = 0;
int valBATT1 = 0;
int posBATT1 = 0;

int BATT2 = 0;
int valBATT2 = 0;
int posBATT2 = 0;

int HYDPL = 0;
int valHYDPL = 0;
int posHYDPL = 0;

int HYDPR = 0;
int valHYDPR = 0;
int posHYDPR = 0;

int WarnCautionDimmerValue = 0;

Stepper stepperRA(STEPS, COILRA1, COILRA2, COILRA3, COILRA4);  // RAD ALT

Stepper stepperCA(STEPS, COILCA1, COILCA2, COILCA3, COILCA4);  // CAB ALT

Stepper stepperBP(STEPS, COILBP1, COILBP2, COILBP3, COILBP4);  // BRAKE PRESSURE
#define BrakePressureZeroPoint 0
#define BrakePressureMaxPoint 150

Stepper stepperBT1(STEPS, COILBT1_1, COILBT1_2, COILBT1_3, COILBT1_4);  // BATTERY 1
#define Battery_1_ZeroPoint 0
#define Battery_1_ZeroPoint 150
Stepper stepperBT2(STEPS, COILBT2_1, COILBT2_2, COILBT2_3, COILBT2_4);  // BATTERY 2
#define Battery_2_ZeroPoint 0
#define Battery_2_ZeroPoint 150

Stepper stepperHPL(STEPS, COILHPL_1, COILHPL_2, COILHPL_3, COILHPL_4);  // HYD PRESSURE L
Stepper stepperHPR(STEPS, COILHPR_1, COILHPR_2, COILHPR_3, COILHPR_4);  // HYD PRESSURE R

int MaxDCSBios = 65535;
int MinDCSBios = 0;
int RadAltChange = 14500;
int RAStepVal = 2620;
int RAStepSteps = 144;
int StepSteps = 720;
//###########################################################################################

void onRadaltAltPtrChange(unsigned int newValueRA) {  //2620

  if (newValueRA <= 56000) RAD_ALT = map(newValueRA, 56000, 0, 0, 580);
  else if (newValueRA >= 56000) RAD_ALT = map(newValueRA, 56001, 65535, 581, 720);
}
DcsBios::IntegerBuffer radaltAltPtrBuffer(0x751a, 0xffff, 0, onRadaltAltPtrChange);

void onPressureAltChange(unsigned int newValueCA) {
  CAB_ALT = map(newValueCA, 0, 65535, 45, 720);
}
DcsBios::IntegerBuffer pressureAltBuffer(0x7514, 0xffff, 0, onPressureAltChange);


void onHydIndBrakeChange(unsigned int newValueBP) {
  BRAKE_PRESSURE = map(newValueBP, 0, 65535, BrakePressureZeroPoint, BrakePressureMaxPoint);
}
DcsBios::IntegerBuffer hydIndBrakeBuffer(0x7506, 0xffff, 0, onHydIndBrakeChange);

void onHydIndLeftChange(unsigned int newValueHL) {
  HYDPL = map(newValueHL, 0, 65535, 0, -720);
}
DcsBios::IntegerBuffer hydIndLeftBuffer(0x751e, 0xffff, 0, onHydIndLeftChange);

void onHydIndRightChange(unsigned int newValueHR) {
  HYDPR = map(newValueHR, 0, 65535, 0, -720);
}
DcsBios::IntegerBuffer hydIndRightBuffer(0x7520, 0xffff, 0, onHydIndRightChange);

void onVoltEChange(unsigned int newValueB1) {
  BATT1 = map(newValueB1, 0, 65535, 0, 250);
}
DcsBios::IntegerBuffer voltEBuffer(0x753e, 0xffff, 0, onVoltEChange);

void onVoltUChange(unsigned int newValueB2) {
  BATT2 = map(newValueB2, 0, 65535, 0, 250);
}
DcsBios::IntegerBuffer voltUBuffer(0x753c, 0xffff, 0, onVoltUChange);


void onRadaltOffFlagChange(unsigned int newValue) {
  if (newValue == 0) {  // ANYTHING ABOVE "0" IS OFF
    RAD_ALT_servo.attach(RadarAltServoPin);
    RAD_ALT_servo.write(RAD_ALT_servo_Hidden_Pos);  // set servo to "Off Point"
    delay(500);
    RAD_ALT_servo.detach();
  } else {
    RAD_ALT_servo.attach(RadarAltServoPin);
    RAD_ALT_servo.write(RAD_ALT_servo_Off_Pos);  // set servo to min
    delay(500);
    RAD_ALT_servo.detach();
  }
}
DcsBios::IntegerBuffer radaltOffFlagBuffer(0x751c, 0xffff, 0, onRadaltOffFlagChange);

//DcsBios::ServoOutput radaltOffFlag(0x751c, RadarAltServoPin, RAD_ALT_servo_Hidden_Pos, RAD_ALT_servo_Off_Pos);
//###########################################################################################

void onConsoleIntLtChange(unsigned int newValue) {
  int ConsolesDimmerValue = 0;

  ConsolesDimmerValue = map(newValue, 0, 65535, 5, 255);
  analogWrite(DIMMER, ConsolesDimmerValue);
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


DcsBios::RotarySyncingPotentiometer radaltHeight("RADALT_HEIGHT", A2, 0x7518, 0xffff, 0, HornetRadaltMapper);


DcsBios::Switch2Pos radaltTestSw("RADALT_TEST_SW", 6);

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



    pinMode(DIMMER, OUTPUT);
    pinMode(RAD_GN, OUTPUT);
    pinMode(RAD_RD, OUTPUT);
    digitalWrite(DIMMER, HIGH);
    digitalWrite(RAD_GN, HIGH);
    digitalWrite(RAD_RD, HIGH);

    RAD_ALT_servo.attach(RadarAltServoPin);
    delay(1000);
    RAD_ALT_servo.write(RAD_ALT_servo_Off_Pos);  // set servo to "Off Point"
    // RAD_ALT_servo.detach();
    delay(1000);
    // RAD_ALT_servo.attach(RadarAltServoPin);
    RAD_ALT_servo.write(RAD_ALT_servo_Hidden_Pos);  // set servo to min
    delay(1000);
    RAD_ALT_servo.write(RAD_ALT_servo_Off_Pos);  // set servo to "Off Point"
    // RAD_ALT_servo.detach();
    delay(1000);
    RAD_ALT_servo.detach();
    //analogWrite(5, LOW);

    //###########################################################################################
    /// RADAR ALT WORKING ======> SET RADAR ALT STEPPER TO 0 FEET
    stepperRA.setSpeed(40);
    stepperRA.step(-720);  //Reset FULL ON Position
    stepperRA.step(720);   //Reset FULL OFF Position
    posRA = 0;
    //  RAD_ALT = map(0, 0, 65535, 720, 0);   //RAD_ALT = map(newValueRA, 0, 65500, 720, 0);

    /// RADAR ALT WORKING ======< SET RADAR ALT STEPPER TO 0 FEET

    /// CABIN ALT WORKING ======> SET CABIN ALT STEPPER TO 0 FEET
    stepperCA.setSpeed(40);
    stepperCA.step(700);   //Reset FULL ON Position
    stepperCA.step(-720);  //Reset FULL OFF Position
    stepperCA.step(30);    //Reset FULL OFF Position
    posCA = 0;
    CAB_ALT = map(0, 0, 65535, 40, 720);
    /// CABIN ALT WORKING ======< SET CABIN ALT STEPPER TO 0 FEET

    // BRAKE PRESSURE
    SendDebug(BoardName + " Start Cycling Brake Pressure");
    stepperBP.setSpeed(40);
    stepperBP.step(300);
    delay(2000);
    stepperBP.
    // stepperBP.step(BrakePressureMaxPoint);  //Reset FULL ON Position
    // delay(500);
    // stepperBP.step(-(BrakePressureMaxPoint + 100));  //Reset FULL OFF Position
    // posBP = 0;
    // BRAKE_PRESSURE = map(0, 0, 65535, BrakePressureZeroPoint, BrakePressureMaxPoint);
    SendDebug(BoardName + " End Cycling Brake Pressure");
    /// BRAKE PRESSURE

    // HYD PRESSURE LEFT
    stepperHPL.setSpeed(50);
    stepperHPL.step(720);   //Reset FULL ON Position
    stepperHPL.step(-720);  //Reset FULL OFF Position
    posHYDPL = 0;
    // HYDPL = map(0, 0, 65535, 720, 0);

    stepperHPR.setSpeed(50);
    stepperHPR.step(720);   //Reset FULL ON Position
    stepperHPR.step(-720);  //Reset FULL OFF Position
    posHYDPR = 0;
    // HYDPR = map(0, 0, 65535, 720, 0);

    // BATT 1 E
    stepperBT1.setSpeed(50);
    stepperBT1.step(720);   //Reset FULL ON Position
    stepperBT1.step(-720);  //Reset FULL OFF Position
    stepperBT1.step(560);   //BATT "16V" POSITION E
                            //stepperBT1.step(-250);       //BATT "30V" POSITION E
    posBATT1 = 0;
    // BATT1 = map(0, 0, 65535, 400, 0);

    // BATT 2 U
    stepperBT2.setSpeed(50);
    stepperBT2.step(-720);  //Reset FULL ON Position
    stepperBT2.step(720);   //Reset FULL OFF Position
    stepperBT2.step(-60);   //BATT "16V" POSITION U
                            //stepperBT2.step(250);       //BATT "30V" POSITION U
    posBATT2 = 0;
    //BATT2 = map(0, 0, 65535, 720, 0);


    delay(500);
    digitalWrite(DIMMER, LOW);
    digitalWrite(RAD_GN, LOW);
    digitalWrite(RAD_RD, LOW);

    SendDebug(BoardName + " End Setup");


    DcsBios::setup();
  }
}

void sendToDcsBiosMessage(const char *msg, const char *arg) {


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

  valRA = RAD_ALT;
  if (abs(valRA - posRA) > 2) {  //if diference is greater than 2 steps.
    if ((valRA - posRA) > 0) {
      stepperRA.step(-1);  // move one step to the left.
      posRA++;
    }
    if ((valRA - posRA) < 0) {
      stepperRA.step(1);  // move one step to the right.
      posRA--;
    }
  }

  /// RADAR ALT LOOP WORKING ======<
  //###########################################################################################
  {
    valCA = CAB_ALT;
    //
    if (abs(valCA - posCA) > 2) {  //if diference is greater than 2 steps.
      if ((valCA - posCA) > 0) {
        stepperCA.step(1);  // move one step to the left.
        posCA++;
      }
      if ((valCA - posCA) < 0) {
        stepperCA.step(-1);  // move one step to the right.
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
    if (abs(valBP - posBP) > 2) {  //if diference is greater than 2 steps.
      if ((valBP - posBP) > 0) {
        stepperBP.step(1);  // move one step to the left.
        posBP++;
      }
      if ((valBP - posBP) < 0) {
        stepperBP.step(-1);  // move one step to the right.
        posBP--;
      }
    }
  }


  valHYDPL = HYDPL;
  if (abs(valHYDPL - posHYDPL) > 2) {  //if diference is greater than 2 steps.
    if ((valHYDPL - posHYDPL) > 0) {
      stepperHPL.step(-1);  // move one step to the left.
      posHYDPL++;
    }
    if ((valHYDPL - posHYDPL) < 0) {
      stepperHPL.step(1);  // move one step to the right.
      posHYDPL--;
    }
  }

  valHYDPR = HYDPR;
  if (abs(valHYDPR - posHYDPR) > 2) {  //if diference is greater than 2 steps.
    if ((valHYDPR - posHYDPR) > 0) {
      stepperHPR.step(-1);  // move one step to the left.
      posHYDPR++;
    }
    if ((valHYDPR - posHYDPR) < 0) {
      stepperHPR.step(1);  // move one step to the right.
      posHYDPR--;
    }
  }



  valBATT1 = BATT1;
  if (abs(valBATT1 - posBATT1) > 2) {  //if diference is greater than 2 steps.
    if ((valBATT1 - posBATT1) > 0) {
      stepperBT1.step(-1);  // move one step to the left.
      posBATT1++;
    }
    if ((valBATT1 - posBATT1) < 0) {
      stepperBT1.step(1);  // move one step to the right.
      posBATT1--;
    }
  }


  valBATT2 = BATT2;
  if (abs(valBATT2 - posBATT2) > 2) {  //if diference is greater than 2 steps.
    if ((valBATT2 - posBATT2) > 0) {
      stepperBT2.step(-1);  // move one step to the left.
      posBATT2++;
    }
    if ((valBATT2 - posBATT2) < 0) {
      stepperBT2.step(1);  // move one step to the right.
      posBATT2--;
    }
  }




  DcsBios::loop();
}
