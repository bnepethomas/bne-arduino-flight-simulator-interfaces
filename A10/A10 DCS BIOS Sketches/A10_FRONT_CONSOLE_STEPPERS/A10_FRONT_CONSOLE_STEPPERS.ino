// Stepper frame for A10 non-engine cluster
// eg Flaps, Airspeed, VSI, AOA, GForces, Compass


// Development test framework to determine max number of steppers that
// can safely be driven off an Arduino without the use of drivers
// Absoultely Max current is 200mA

// Measuring using USB power meter
// 0.19A with 4 steppers disabled
// 0.27A with 4 steppers enabled
// So average current draw 20mA per stepper
// With 12 Steppers enabled - total peak into board was 490mA with average around 440mA
// With Mega, Ethernet and Led running current was 220mA
// With Mega and Ehternet current is 210mA
// If steppers as disabled using the accelstepper library then current draw returns to 210mA




// Issues with steppers gettting out of synch was largely due to physical binding
// which sometimes only appeared after a 30 minutes run.  Shaft was bings against faceplate.

// 312 MicroSeconds (~0.3mS) to run through stepper loop with a single stepper is needing to step
// 1344 MicroSeconds (~1.3mS) to run through stepper loop with 6 steppers needing to step
// 2576 MicroSecond (~3mS) to run through loop when 12 steppers are needing to step


// Stepper 1 - Flaps
// Stepper 2 -
// Stepper 3 - Avoid use if possible
// Stepper 4 -
// Stepper 5 -
// Stepper 6 -
// Stepper 7 -
// Stepper 8 -
// Stepper 9 -
// Stepper 10 -
// Stepper 11 -
// Stepper 12 -


#define Ethernet_In_Use 1
#define Reflector_In_Use 1
#define DCSBIOS_In_Use 1
#define MSFS_In_Use 0  // Used to interface into MSFS - set to 0 if not in use

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
char *ParameterNamePtr;
char *ParameterValuePtr;




void SendDebug(String MessageToSend) {
  if ((Reflector_In_Use == 1) && (Ethernet_In_Use == 1)) {
    udp.beginPacket(reflectorIP, reflectorport);
    udp.println(MessageToSend);
    udp.endPacket();
  }
}
// ###################################### End Ethernet Related #############################



#define RED_STATUS_LED_PORT 8
#define FLASH_TIME 500

unsigned long NEXT_STATUS_TOGGLE_TIMER = 0;
bool RED_LED_STATE = false;
unsigned long timeSinceRedLedChanged = 0;

#define DCSBIOS_IRQ_SERIAL
#include <DcsBios.h>

// ###################################### Begin Servo Related #############################
#include <AccelStepper.h>
#include <Stepper.h>
#define STEPS 720  // steps per revolution (limited to 315°)


bool TimingCollected = false;
unsigned long TimeToComplete = 0;


// Holding current per coil is 14mA, which gives 28mA per stepper
// Mega absolute max is 40mA per pin, with a total max of 200mA
// Gives a total


// If the stepper is incorrectly configured it will chatter above
// rates of more than 30 steps per minute, if when correctly
// configured speeds can exceed 2300

// Stepper 3 currently maps to Serial IO which DCS BIOS uses - currently disabled it

#define S1InUse true
#define S2InUse true
#define S3InUse true
#define S4InUse true

#define S5InUse true
#define S6InUse true
#define S7InUse true
#define S8InUse true

#define S9InUse true
#define S10InUse true
#define S11InUse true
#define S12InUse true

// Stepper 1 - Left Engine PSI
// Stepper 2 - Right Engine PSI
// Stepper 3 - APU RPM
// Stepper 4 - APU EGT
// Stepper 5 - Left Engine RPM
// Stepper 6 - Right Engine RPM
// Stepper 7 - Left Engine Fuel Flow
// Stepper 8 - Right Engine Fuel Flow
// Stepper 9 - Left Engine Temp
// Stepper 10 - Right Engine Temp
// Stepper 11 - Left Engine Fan RPM
// Stepper 12 - Right Engine Fan RPM


// #define S1InUse false
// #define S2InUse false
// #define S3InUse false
// #define S4InUse false

// #define S5InUse false
// #define S6InUse false
// #define S7InUse false
// #define S8InUse false

// #define S9InUse false
// #define S10InUse false
// #define S11InUse false
// #define S12InUse false




#define COIL_STEPPER_1_A 12
#define COIL_STEPPER_1_B 13
#define COIL_STEPPER_1_C 9
#define COIL_STEPPER_1_D 11

#define COIL_STEPPER_2_A 4
#define COIL_STEPPER_2_B 5
#define COIL_STEPPER_2_C 7
#define COIL_STEPPER_2_D 6

#define COIL_STEPPER_3_A 54
#define COIL_STEPPER_3_B 55
#define COIL_STEPPER_3_C 56
#define COIL_STEPPER_3_D 57

#define COIL_STEPPER_4_A 14
#define COIL_STEPPER_4_B 15
#define COIL_STEPPER_4_C 16
#define COIL_STEPPER_4_D 17

#define COIL_STEPPER_5_A 20
#define COIL_STEPPER_5_B 21
#define COIL_STEPPER_5_C 18
#define COIL_STEPPER_5_D 19

#define COIL_STEPPER_6_A 26
#define COIL_STEPPER_6_B 28
#define COIL_STEPPER_6_C 22
#define COIL_STEPPER_6_D 24

#define COIL_STEPPER_7_A 30
#define COIL_STEPPER_7_B 32
#define COIL_STEPPER_7_C 34
#define COIL_STEPPER_7_D 36

#define COIL_STEPPER_8_A 38
#define COIL_STEPPER_8_B 40
#define COIL_STEPPER_8_C 42
#define COIL_STEPPER_8_D 44

#define COIL_STEPPER_9_A 23
#define COIL_STEPPER_9_B 25
#define COIL_STEPPER_9_C 27
#define COIL_STEPPER_9_D 29

#define COIL_STEPPER_10_A 31
#define COIL_STEPPER_10_B 33
#define COIL_STEPPER_10_C 35
#define COIL_STEPPER_10_D 37

#define COIL_STEPPER_11_A 39
#define COIL_STEPPER_11_B 41
#define COIL_STEPPER_11_C 43
#define COIL_STEPPER_11_D 45

#define COIL_STEPPER_12_A 47
#define COIL_STEPPER_12_B 49
#define COIL_STEPPER_12_C 46
#define COIL_STEPPER_12_D 48

// #define STEPPER_MAX_SPEED 900
#define STEPPER_MAX_SPEED 8300
#define STEPPER_ACCELERATION 2000

#define STEPPER_4_MAX_SPEED 8300
#define STEPPER_4_ACCELERATION 2000

AccelStepper STEPPER_1(AccelStepper::FULL4WIRE, COIL_STEPPER_1_A, COIL_STEPPER_1_B, COIL_STEPPER_1_C, COIL_STEPPER_1_D);
AccelStepper STEPPER_2(AccelStepper::FULL4WIRE, COIL_STEPPER_2_A, COIL_STEPPER_2_B, COIL_STEPPER_2_C, COIL_STEPPER_2_D);
AccelStepper STEPPER_3(AccelStepper::FULL4WIRE, COIL_STEPPER_3_A, COIL_STEPPER_3_B, COIL_STEPPER_3_C, COIL_STEPPER_3_D);
AccelStepper STEPPER_4(AccelStepper::FULL4WIRE, COIL_STEPPER_4_A, COIL_STEPPER_4_B, COIL_STEPPER_4_C, COIL_STEPPER_4_D);
AccelStepper STEPPER_5(AccelStepper::FULL4WIRE, COIL_STEPPER_5_A, COIL_STEPPER_5_B, COIL_STEPPER_5_C, COIL_STEPPER_5_D);
AccelStepper STEPPER_6(AccelStepper::FULL4WIRE, COIL_STEPPER_6_A, COIL_STEPPER_6_B, COIL_STEPPER_6_C, COIL_STEPPER_6_D);
AccelStepper STEPPER_7(AccelStepper::FULL4WIRE, COIL_STEPPER_7_A, COIL_STEPPER_7_B, COIL_STEPPER_7_C, COIL_STEPPER_7_D);
AccelStepper STEPPER_8(AccelStepper::FULL4WIRE, COIL_STEPPER_8_A, COIL_STEPPER_8_B, COIL_STEPPER_8_C, COIL_STEPPER_8_D);
AccelStepper STEPPER_9(AccelStepper::FULL4WIRE, COIL_STEPPER_9_A, COIL_STEPPER_9_B, COIL_STEPPER_9_C, COIL_STEPPER_9_D);
AccelStepper STEPPER_10(AccelStepper::FULL4WIRE, COIL_STEPPER_10_A, COIL_STEPPER_10_B, COIL_STEPPER_10_C, COIL_STEPPER_10_D);
AccelStepper STEPPER_11(AccelStepper::FULL4WIRE, COIL_STEPPER_11_A, COIL_STEPPER_11_B, COIL_STEPPER_11_C, COIL_STEPPER_11_D);
AccelStepper STEPPER_12(AccelStepper::FULL4WIRE, COIL_STEPPER_12_A, COIL_STEPPER_12_B, COIL_STEPPER_12_C, COIL_STEPPER_12_D);


void setZeroPoints() {
  SendDebug("Entering setZeroPoints");
  long SafetyTimer = millis();
  STEPPER_4.move(600);
  if (S4InUse) {
    while (digitalRead(A0) == 1) {
      if (millis() >= (SafetyTimer + 30000)) {
        SendDebug("Safety Timer Event in setZeroPoints");
        break;
      }
      STEPPER_4.run();
    }
    if (digitalRead(A0) == 0) {
      STEPPER_4.setCurrentPosition(0);
      SendDebug("Found Stepper_4 Zero in setZeroPoints");
      STEPPER_4.runToNewPosition(0);
    }
  }
}


// ###################################### End Stepper Related #############################

unsigned int valAltimeter = 0;
String Alt1000s = "0";
int LastAlt1000s = 0;
String Alt10000s = "0";
int LastAlt10000s = 0;
bool AltCounterUpdated = true;

// Altimeter delta 1000 feet for 112 pressure units
// which maps to 8.92857 feet per pressure unit with 2992 as reference
// so delta is (pressure reading - 2992) * 8.92857
#define feetDeltaPerPressureUnit 8.92857
int iLastAltitudeValue = 0;
int iAltitudeDelta = 0;
int iBaro = 2992;
int iBaroOnes = 2;
int iBaroTens = 9;
int iBaroHundreds = 9;
int iBaroThousands = 2;
String BaroOnes = String(iBaroOnes);
String BaroTens = String(iBaroTens);
String BaroHundreds = String(iBaroHundreds);
String BaroThousands = String(iBaroThousands);
bool BaroUpdated = true;

void onAltMslFtChange(unsigned int newValue) {

  int updateAltimeterInterval = 100;
  unsigned int tempvar = 0;
  int Alt100s = 0;

  if (newValue <= 0) newValue = 0;
  iLastAltitudeValue = newValue;

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
  if (S4InUse) STEPPER_4.moveTo(valAltimeter);
  //SendDebug("Alt " + String(newValue) + " Translated Rate" + String(valAltimeter));
  // SendDebug("Pointer Target = " + String(targetLocation));
  //stepperSTANDBY_ALT.runToNewPosition(targetLocation);
}
DcsBios::IntegerBuffer altMslFtBuffer(0x0434, 0xffff, 0, onAltMslFtChange);




bool PressureChanged = false;
String sAircraftName = "";
void ProcessPressureChange() {

  iBaro = iBaroThousands * 1000 + iBaroHundreds * 100 + iBaroTens * 10 + iBaroOnes;
  // SendDebug("Calc delta :" + String(int((iBaro - 2992) * feetDeltaPerPressureUnit)));
  iAltitudeDelta = int((iBaro - 2992) * feetDeltaPerPressureUnit);

  SendDebug("iBaro :" + String(iBaro) + "  Alt :" + String(iLastAltitudeValue) + "   Delta : " + String(iAltitudeDelta));
  if (sAircraftName == "FA-18C_hornet") {
    BaroThousands = "2";
    BaroHundreds = "9";
  }


  PressureChanged = false;
}

// ****************** BEGIN HORNET ************************************** //
// Smallest Digit
void onStbyPressSet0Change(unsigned int newValue) {
  // 0 64592, 1 6797, 2 14537, 3 18407, 4 26147, 5 33887,
  // 6 37757, 7 45497, 8 53237, 9 57107, 0 64847
  // if (newValue >= 63000)
  //   newValue = 0;
  constrain(newValue, 0, 62000);
  iBaroOnes = map(newValue, 0, 63000, 0, 10);
  BaroOnes = String(iBaroOnes);
  PressureChanged = true;
}
DcsBios::IntegerBuffer stbyPressSet0Buffer(0x74fa, 0xffff, 0, onStbyPressSet0Change);


void onStbyPressSet1Change(unsigned int newValue) {
  // if (newValue >= 63000)
  //   newValue = 0;
  constrain(newValue, 0, 62000);
  iBaroTens = map(newValue, 0, 63000, 0, 10);
  BaroTens = String(iBaroTens);
  PressureChanged = true;
}

DcsBios::IntegerBuffer stbyPressSet1Buffer(0x74fc, 0xffff, 0, onStbyPressSet1Change);

// Second largest digit - the largest digit doesn't change
void onStbyPressSet2Change(unsigned int newValue) {
  newValue = 0;
}
DcsBios::IntegerBuffer stbyPressSet2Buffer(0x74fe, 0xffff, 0, onStbyPressSet2Change);

// ****************** END HORNET ************************************** //


// ****************** BEGIN A10 ************************************** //
void onAltPressure0Change(unsigned int newValue) {
  // Provide a little bias to deal with integer maths
  newValue = newValue + 5;
  iBaroOnes = map(newValue, 0, 65535, 0, 10);
  BaroOnes = String(iBaroOnes);
  PressureChanged = true;
}
DcsBios::IntegerBuffer altPressure0Buffer(0x1086, 0xffff, 0, onAltPressure0Change);

void onAltPressure1Change(unsigned int newValue) {
  // Provide a little bias to deal with integer maths
  newValue = newValue + 5;
  iBaroTens = map(newValue, 0, 65535, 0, 10);
  BaroTens = String(iBaroTens);
  PressureChanged = true;
}
DcsBios::IntegerBuffer altPressure1Buffer(0x1088, 0xffff, 0, onAltPressure1Change);

void onAltPressure2Change(unsigned int newValue) {
  // Provide a little bias to deal with integer maths
  newValue = newValue + 5;
  iBaroHundreds = map(newValue, 0, 65535, 0, 10);
  BaroHundreds = String(iBaroHundreds);
  PressureChanged = true;
}
DcsBios::IntegerBuffer altPressure2Buffer(0x108a, 0xffff, 0, onAltPressure2Change);

void onAltPressure3Change(unsigned int newValue) {

  // Provide a little bias to deal with integer maths
  // Otherwise digits as a little low
  // 30% is 19660.5 but gets runded down to 19559 I'm guessing
  newValue = newValue + 5;
  iBaroThousands = map(newValue, 0, 65535, 0, 10);
  BaroThousands = String(iBaroThousands);
  PressureChanged = true;
  
}
DcsBios::IntegerBuffer altPressure3Buffer(0x108c, 0xffff, 0, onAltPressure3Change);

// ******************  END A10  ************************************** //


void onAcftNameChange(char* newValue) {
  sAircraftName = String(newValue);
  SendDebug("Aircraft Name : " + sAircraftName);
}
DcsBios::StringBuffer<24> AcftNameBuffer(0x0000, onAcftNameChange);



void setup() {
  // put your setup code here, to run once:
  pinMode(RED_STATUS_LED_PORT, OUTPUT);

  digitalWrite(RED_STATUS_LED_PORT, true);
  delay(FLASH_TIME);
  digitalWrite(RED_STATUS_LED_PORT, false);

  delay(FLASH_TIME);

  pinMode(A0, INPUT);

  if (Ethernet_In_Use == 1) {

    // Using manual reset instead of tying to Arduino Reset
    pinMode(ES1_RESET_PIN, OUTPUT);
    digitalWrite(ES1_RESET_PIN, LOW);
    delay(2);
    digitalWrite(ES1_RESET_PIN, HIGH);

    Ethernet.begin(mac, ip);


    udp.begin(localport);
  }


  // For reasons I'm yet to work out - earlier senddebugs are not sent before this point
  // Testing shows a delay of 3 seconds is needed
  SendDebug(" ");
  SendDebug("A10 General Steppers Initialising");


  STEPPER_1.setMaxSpeed(STEPPER_MAX_SPEED);
  STEPPER_1.setAcceleration(STEPPER_ACCELERATION);
  STEPPER_2.setMaxSpeed(STEPPER_MAX_SPEED);
  STEPPER_2.setAcceleration(STEPPER_ACCELERATION);
  STEPPER_3.setMaxSpeed(STEPPER_MAX_SPEED);
  STEPPER_3.setAcceleration(STEPPER_ACCELERATION);
  STEPPER_4.setMaxSpeed(STEPPER_MAX_SPEED);
  STEPPER_4.setAcceleration(STEPPER_ACCELERATION);
  STEPPER_4.setMaxSpeed(STEPPER_4_MAX_SPEED);
  STEPPER_4.setAcceleration(STEPPER_4_ACCELERATION);
  STEPPER_5.setMaxSpeed(STEPPER_MAX_SPEED);
  STEPPER_5.setAcceleration(STEPPER_ACCELERATION);
  STEPPER_6.setMaxSpeed(STEPPER_MAX_SPEED);
  STEPPER_6.setAcceleration(STEPPER_ACCELERATION);
  STEPPER_7.setMaxSpeed(STEPPER_MAX_SPEED);
  STEPPER_7.setAcceleration(STEPPER_ACCELERATION);
  STEPPER_8.setMaxSpeed(STEPPER_MAX_SPEED);
  STEPPER_8.setAcceleration(STEPPER_ACCELERATION);
  STEPPER_9.setMaxSpeed(STEPPER_MAX_SPEED);
  STEPPER_9.setAcceleration(STEPPER_ACCELERATION);
  STEPPER_10.setMaxSpeed(STEPPER_MAX_SPEED);
  STEPPER_10.setAcceleration(STEPPER_ACCELERATION);
  STEPPER_11.setMaxSpeed(STEPPER_MAX_SPEED);
  STEPPER_11.setAcceleration(STEPPER_ACCELERATION);
  STEPPER_12.setMaxSpeed(STEPPER_MAX_SPEED);
  STEPPER_12.setAcceleration(STEPPER_ACCELERATION);


  initialiseAllSteppers(630);

  setZeroPoints();

  STEPPER_2.moveTo((200));


  if (DCSBIOS_In_Use == 1) DcsBios::setup();
  SendDebug("A10 General Steppers Initialisation Complete");
}

void moveSteppersUntilTargetReached() {
  while ((STEPPER_1.distanceToGo() != 0) || (STEPPER_2.distanceToGo() != 0) || (STEPPER_3.distanceToGo() != 0) || (STEPPER_4.distanceToGo() != 0) || (STEPPER_5.distanceToGo() != 0) || (STEPPER_6.distanceToGo() != 0) || (STEPPER_7.distanceToGo() != 0) || (STEPPER_8.distanceToGo() != 0) || (STEPPER_9.distanceToGo() != 0) || (STEPPER_9.distanceToGo() != 0) || (STEPPER_10.distanceToGo() != 0) || (STEPPER_11.distanceToGo() != 0) || (STEPPER_12.distanceToGo() != 0)) {
    //SendDebug("Stepper_1 distance to go :" + String(STEPPER_1.distanceToGo()));
    if (TimingCollected == false) {
      TimeToComplete = micros();
    }


    if (S1InUse) STEPPER_1.run();
    if (S2InUse) STEPPER_2.run();
    if (S3InUse) STEPPER_3.run();
    if (S4InUse) STEPPER_4.run();
    if (S5InUse) STEPPER_5.run();
    if (S6InUse) STEPPER_6.run();
    if (S7InUse) STEPPER_7.run();
    if (S8InUse) STEPPER_8.run();
    if (S9InUse) STEPPER_9.run();
    if (S10InUse) STEPPER_10.run();
    if (S11InUse) STEPPER_11.run();
    if (S12InUse) STEPPER_12.run();
    if (TimingCollected == false) {
      TimeToComplete = micros() - TimeToComplete;
      SendDebug("Time to run through cycle: " + String(TimeToComplete));
      TimingCollected = true;
    }
  }
}

void updateSteppers() {
  if (S1InUse) STEPPER_1.run();
  if (S2InUse) STEPPER_2.run();
  if (S3InUse) STEPPER_3.run();
  if (S4InUse) STEPPER_4.run();
  if (S5InUse) STEPPER_5.run();
  if (S6InUse) STEPPER_6.run();
  if (S7InUse) STEPPER_7.run();
  if (S8InUse) STEPPER_8.run();
  if (S9InUse) STEPPER_9.run();
  if (S10InUse) STEPPER_10.run();
  if (S11InUse) STEPPER_11.run();
  if (S12InUse) STEPPER_12.run();
}


#define FlapsMaxDegrees 200
void setFlaps(unsigned int TargetDegrees) {
  if (S1InUse) {
    if (TargetDegrees >= FlapsMaxDegrees) TargetDegrees = FlapsMaxDegrees;
    STEPPER_1.moveTo(TargetDegrees);
  }
}
void onFlapPosChange(unsigned int newValue) {
  setFlaps((map(newValue, 0, 65535, 0, FlapsMaxDegrees * 0.7)));
}
DcsBios::IntegerBuffer flapPosBuffer(0x10a0, 0xffff, 0, onFlapPosChange);



void initialiseAllSteppers(int numberOfDegrees) {
  // 630 for full sweep
  // Do a full swing for all steppers to set 0 point


  //enableAllSteppers();

  SendDebug("Start All Stepper");
  if (S1InUse) STEPPER_1.moveTo(numberOfDegrees);
  if (S2InUse) STEPPER_2.moveTo(numberOfDegrees);
  if (S3InUse) STEPPER_3.moveTo(numberOfDegrees);
  if (S4InUse) STEPPER_4.moveTo(numberOfDegrees);
  if (S5InUse) STEPPER_5.moveTo(numberOfDegrees);
  if (S6InUse) STEPPER_6.moveTo(numberOfDegrees);
  if (S7InUse) STEPPER_7.moveTo(numberOfDegrees);
  if (S8InUse) STEPPER_8.moveTo(numberOfDegrees);
  if (S9InUse) STEPPER_9.moveTo(numberOfDegrees);
  if (S10InUse) STEPPER_10.moveTo(numberOfDegrees);
  if (S11InUse) STEPPER_11.moveTo(numberOfDegrees);
  if (S12InUse) STEPPER_12.moveTo(numberOfDegrees);

  while ((STEPPER_1.distanceToGo() != 0) || (STEPPER_2.distanceToGo() != 0) || (STEPPER_3.distanceToGo() != 0) || (STEPPER_4.distanceToGo() != 0) || (STEPPER_5.distanceToGo() != 0) || (STEPPER_6.distanceToGo() != 0) || (STEPPER_7.distanceToGo() != 0) || (STEPPER_8.distanceToGo() != 0) || (STEPPER_9.distanceToGo() != 0) || (STEPPER_9.distanceToGo() != 0) || (STEPPER_10.distanceToGo() != 0) || (STEPPER_11.distanceToGo() != 0) || (STEPPER_12.distanceToGo() != 0)) {
    //SendDebug("Stepper_1 distance to go :" + String(STEPPER_1.distanceToGo()));
    if (TimingCollected == false) {
      TimeToComplete = micros();
    }


    updateSteppers();
    if (TimingCollected == false) {
      TimeToComplete = micros() - TimeToComplete;
      SendDebug("Time to run through cycle: " + String(TimeToComplete));
      TimingCollected = true;
    }
  }
  // SendDebug("Steppers at Max");
  if (S1InUse) STEPPER_1.moveTo(0);
  if (S2InUse) STEPPER_2.moveTo(0);
  if (S3InUse) STEPPER_3.moveTo(0);
  if (S4InUse) STEPPER_4.moveTo(0);
  if (S5InUse) STEPPER_5.moveTo(0);
  if (S6InUse) STEPPER_6.moveTo(0);
  if (S7InUse) STEPPER_7.moveTo(0);
  if (S8InUse) STEPPER_8.moveTo(0);
  if (S9InUse) STEPPER_9.moveTo(0);
  if (S10InUse) STEPPER_10.moveTo(0);
  if (S11InUse) STEPPER_11.moveTo(0);
  if (S12InUse) STEPPER_12.moveTo(0);


  //SendDebug("Returning");

  while ((STEPPER_1.distanceToGo() != 0) || (STEPPER_2.distanceToGo() != 0) || (STEPPER_3.distanceToGo() != 0) || (STEPPER_4.distanceToGo() != 0) || (STEPPER_5.distanceToGo() != 0) || (STEPPER_6.distanceToGo() != 0) || (STEPPER_7.distanceToGo() != 0) || (STEPPER_8.distanceToGo() != 0) || (STEPPER_9.distanceToGo() != 0) || (STEPPER_9.distanceToGo() != 0) || (STEPPER_10.distanceToGo() != 0) || (STEPPER_11.distanceToGo() != 0) || (STEPPER_12.distanceToGo() != 0))

  {
    updateSteppers();
  }
  //disableAllSteppers();
  SendDebug("End All Stepper");
}


void cycleSteppers(int numberOfDegrees) {
  // 630 for full sweep


  //enableAllSteppers();

  SendDebug("Start All Stepper");
  if (S1InUse) setFlaps(numberOfDegrees);
  // if (S2InUse) setRightEngPSI(numberOfDegrees);
  // if (S3InUse) setAPURPM(numberOfDegrees);
  // if (S4InUse) setAPUEGT(numberOfDegrees);
  // if (S5InUse) setLeftEngRPM(numberOfDegrees);
  // if (S6InUse) setRightEngRPM(numberOfDegrees);
  // if (S7InUse) setLeftFuelFlow(numberOfDegrees);
  // if (S8InUse) setRightFuelFlow(numberOfDegrees);
  // if (S9InUse) setLeftEngTemp(numberOfDegrees);
  // if (S10InUse) setRightEngTemp(numberOfDegrees);
  // if (S11InUse) setLeftFanRPM(numberOfDegrees);
  // if (S12InUse) setRightFanRPM(numberOfDegrees);

  while ((STEPPER_1.distanceToGo() != 0) || (STEPPER_2.distanceToGo() != 0) || (STEPPER_3.distanceToGo() != 0) || (STEPPER_4.distanceToGo() != 0) || (STEPPER_5.distanceToGo() != 0) || (STEPPER_6.distanceToGo() != 0) || (STEPPER_7.distanceToGo() != 0) || (STEPPER_8.distanceToGo() != 0) || (STEPPER_9.distanceToGo() != 0) || (STEPPER_9.distanceToGo() != 0) || (STEPPER_10.distanceToGo() != 0) || (STEPPER_11.distanceToGo() != 0) || (STEPPER_12.distanceToGo() != 0)) {
    //SendDebug("Stepper_1 distance to go :" + String(STEPPER_1.distanceToGo()));
    if (TimingCollected == false) {
      TimeToComplete = micros();
    }


    STEPPER_1.run();
    STEPPER_2.run();
    STEPPER_3.run();
    STEPPER_4.run();
    STEPPER_5.run();
    STEPPER_6.run();
    STEPPER_7.run();
    STEPPER_8.run();
    STEPPER_9.run();
    STEPPER_10.run();
    STEPPER_11.run();
    STEPPER_12.run();
    if (TimingCollected == false) {
      TimeToComplete = micros() - TimeToComplete;
      SendDebug("Time to run through cycle: " + String(TimeToComplete));
      TimingCollected = true;
    }
  }
  // SendDebug("Steppers at Max");
  if (S1InUse) STEPPER_1.moveTo(0);
  if (S2InUse) STEPPER_2.moveTo(0);
  if (S3InUse) STEPPER_3.moveTo(0);
  if (S4InUse) STEPPER_4.moveTo(0);
  if (S5InUse) STEPPER_5.moveTo(0);
  if (S6InUse) STEPPER_6.moveTo(0);
  if (S7InUse) STEPPER_7.moveTo(0);
  if (S8InUse) STEPPER_8.moveTo(0);
  if (S9InUse) STEPPER_9.moveTo(0);
  if (S10InUse) STEPPER_10.moveTo(0);
  if (S11InUse) STEPPER_11.moveTo(0);
  if (S12InUse) STEPPER_12.moveTo(0);
  //SendDebug("Returning");

  while ((STEPPER_1.distanceToGo() != 0) || (STEPPER_2.distanceToGo() != 0) || (STEPPER_3.distanceToGo() != 0) || (STEPPER_4.distanceToGo() != 0) || (STEPPER_5.distanceToGo() != 0) || (STEPPER_6.distanceToGo() != 0) || (STEPPER_7.distanceToGo() != 0) || (STEPPER_8.distanceToGo() != 0) || (STEPPER_9.distanceToGo() != 0) || (STEPPER_9.distanceToGo() != 0) || (STEPPER_10.distanceToGo() != 0) || (STEPPER_11.distanceToGo() != 0) || (STEPPER_12.distanceToGo() != 0))

  {
    updateSteppers();
  }
  //disableAllSteppers();
  SendDebug("End All Stepper");
}

void disableAllSteppers() {
  STEPPER_1.disableOutputs();
  STEPPER_2.disableOutputs();
  STEPPER_3.disableOutputs();
  STEPPER_4.disableOutputs();
  STEPPER_5.disableOutputs();
  STEPPER_6.disableOutputs();
  STEPPER_7.disableOutputs();
  STEPPER_8.disableOutputs();
  STEPPER_9.disableOutputs();
  STEPPER_10.disableOutputs();
  STEPPER_11.disableOutputs();
  STEPPER_12.disableOutputs();
}

void enableAllSteppers() {
  STEPPER_1.enableOutputs();
  STEPPER_2.enableOutputs();
  STEPPER_3.enableOutputs();
  STEPPER_4.enableOutputs();
  STEPPER_5.enableOutputs();
  STEPPER_6.enableOutputs();
  STEPPER_7.enableOutputs();
  STEPPER_8.enableOutputs();
  STEPPER_9.enableOutputs();
  STEPPER_10.enableOutputs();
  STEPPER_11.enableOutputs();
  STEPPER_12.enableOutputs();
}





void loop() {

  if (millis() >= NEXT_STATUS_TOGGLE_TIMER) {
    RED_LED_STATE = !RED_LED_STATE;
    digitalWrite(RED_STATUS_LED_PORT, RED_LED_STATE);
    // enableAllSteppers();
    // cycleSteppers(650);
    // disableAllSteppers();
    // SendDebug("Uptime " + String(millis()) + " (" + String(millis() / 60000) + ")");
    NEXT_STATUS_TOGGLE_TIMER = millis() + FLASH_TIME;
    // Check to see if model time is updating - if nothing after 30 seconds disble steppers
  }

  if (DCSBIOS_In_Use == 1) DcsBios::loop();
    if (PressureChanged == true)  ProcessPressureChange(); 

  updateSteppers();
  //disableAllSteppers();
}
