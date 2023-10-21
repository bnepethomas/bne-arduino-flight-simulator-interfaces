// Source
// https://gist.github.com/jboecker/1084b3768c735b164c34d6087d537c18

// For reasons Im yet to work out when including
// the encoder for dcsbios I'm getting the folowing when uploading to a mega
// avrdude: verification error; content mismatch
// problem disappeared when altimeter removed and after reboot root cause tba


// the Warthog Project Video on the compass build
// https://www.youtube.com/watch?v=ZN9glqgp9TY&t=332s

#define Ethernet_In_Use 1
#define DCSBIOS_In_Use 1
#define Reflector_In_Use 1



// Ethernet Related
#include <SPI.h>
#include <Ethernet.h>
#include <EthernetUdp.h>



// These local Mac and IP Address will be reassigned early in startup based on
// the device ID as set by address pins
byte mac[] = {0xA8, 0x61, 0x0A, 0x9E, 0x83, 0x01};
IPAddress ip(172, 16, 1, 101);
String strMyIP = "172.16.1.101";

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



String VVI = "0";
int LastVVI = 0;

// Altimeter

unsigned long nextAltimeterUpdate = 0;
int updateAltimeterInterval = 100;
#define ALT_ZERO_SENSE_PIN 49

//#include <AccelStepper.h>
#include <Stepper.h>
#define  STEPS  720    // steps per revolution (limited to 315°)

#define  COIL_STANDBY_ALT_A1  41 // STANDBY ALT
#define  COIL_STANDBY_ALT_A2  43
#define  COIL_STANDBY_ALT_A3  45
#define  COIL_STANDBY_ALT_A4  47

int STANDBY_ALT = 0;
int LastSTANDBY_ALT = 0;
unsigned int valAltimeter = 0;
unsigned int posAltimeter = 0;

// AccelStepper stepperSTANDBY_ALT(STEPS, COIL_STANDBY_ALT_A1, COIL_STANDBY_ALT_A2, COIL_STANDBY_ALT_A3, COIL_STANDBY_ALT_A4);
// AccelStepper stepperSTANDBY_ALT(AccelStepper::FULL4WIRE , COIL_STANDBY_ALT_A1, COIL_STANDBY_ALT_A2, COIL_STANDBY_ALT_A3, COIL_STANDBY_ALT_A4);
Stepper stepperSTANDBY_ALT(STEPS , COIL_STANDBY_ALT_A1, COIL_STANDBY_ALT_A2, COIL_STANDBY_ALT_A3, COIL_STANDBY_ALT_A4);


// Airspeed
unsigned long nextAirSpeedUpdate = 0;
int updateAirSpeedInterval = 100;
#define AIRSPEED_ZERO_SENSE_PIN 40

#define  COIL_AIRSPEED_A1  32
#define  COIL_AIRSPEED_A2  34
#define  COIL_AIRSPEED_A3  36
#define  COIL_AIRSPEED_A4  38

int STANDBY_AIRSPEED = 0;
int LastSTANDBY_AIRSPEED = 0;
unsigned int valAIRSPEED = 0;
unsigned int posAIRSPEED = 0;

Stepper stepperSTANDBY_AIRSPEED(STEPS , COIL_AIRSPEED_A1, COIL_AIRSPEED_A2, COIL_AIRSPEED_A3, COIL_AIRSPEED_A4);


// VVI
unsigned long nextVVIUpdate = 0;
int updateVVIInterval = 100;
#define VVI_ZERO_SENSE_PIN 30
#define VVIOffSetToZeroPoint 535

// Needle moves but is consistent in direction
//#define  COIL_VVI_A1  22
//#define  COIL_VVI_A2  24
//#define  COIL_VVI_A3  26
//#define  COIL_VVI_A4  28

#define  COIL_VVI_A1  22
#define  COIL_VVI_A2  26
#define  COIL_VVI_A3  24
#define  COIL_VVI_A4  28

int STANDBY_VVI = 0;
int LastSTANDBY_VVI = 0;
unsigned int valVVI = 0;
unsigned int posVVI = 0;

Stepper stepperSTANDBY_VVI(STEPS , COIL_VVI_A1, COIL_VVI_A2, COIL_VVI_A3, COIL_VVI_A4);



void SendDebug( String MessageToSend) {
  if ((Reflector_In_Use == 1) &&  (Ethernet_In_Use == 1)) {
    udp.beginPacket(reflectorIP, reflectorport);
    udp.println(MessageToSend);
    udp.endPacket();
  }
}



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

  Serial.begin(115200);
  Serial.println("Init");

  if (Ethernet_In_Use == 1) {
    Ethernet.begin( mac, ip);


    udp.begin( localport );
    if (Reflector_In_Use == 1)  {
      udp.beginPacket(reflectorIP, reflectorport);
      udp.println("Init I2C Test rig - " + strMyIP + " " + String(millis()) + "mS since reset.");
      udp.endPacket();
    }
  }



  nextupdate = millis() + flashinterval;



  SendDebug("Looking for VVI Zero");
  pinMode(VVI_ZERO_SENSE_PIN,  INPUT_PULLUP);


  stepperSTANDBY_VVI.setSpeed(60);
  stepperSTANDBY_VVI.step(1000);
  for (int i = 0; i <= 2000; i++) {
    delay(1);
    stepperSTANDBY_VVI.step(1);
    if (digitalRead(VVI_ZERO_SENSE_PIN) == false) {
      SendDebug("Found VVI Zero");
      stepperSTANDBY_VVI.step(VVIOffSetToZeroPoint);
      posVVI = 0;
      break;
    }
  }



}

int CalculateAirSpeedPosition( int newValue) {

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

#define Pos60Knot   30
#define Pos100Knot  100
#define Pos150Knot  230
#define Pos200Knot  408
#define Pos300Knot  478
#define Pos400Knot  525
#define Pos500Knot  567
#define Pos600Knot  601
#define Pos700Knot  634
#define Pos800Knot  673
#define Pos850Knot  688

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

  SendDebug("Returning from CalculateAirSpeedPosition: " + String(newPosition));
  return(newPosition);

}


int CalculateVVIPosition( int newValue) {

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
  else if (newValue <= 0)     newPosition = map(newValue, 0, -1000, 0, -Pos1000);
  else if (newValue <= 1000)  newPosition = map(newValue, 0, 1000, 0, Pos1000);
  else if (newValue <= 2000)  newPosition = map(newValue, 1000, 2000, Pos1000, Pos2000);
  else if (newValue <= 4000)  newPosition = map(newValue, 2000, 4000, Pos2000, Pos4000);
  else if (newValue <= 6000)  newPosition = map(newValue, 4000, 6000, Pos4000, Pos6000);
  else if (newValue >= 6000)  newPosition = Pos6000;

  SendDebug("Returning from CalculateVVIPosition: " + String(newPosition));
  return(newPosition);

}


void loop() {
  Serial.println("Enter Location to Move to");
  int red = Serial.parseInt();
  if (Serial.read() == '\n') {
    Serial.println("Moving to " + String(red));
    int targetPos = CalculateVVIPosition(red);
    //int targetPos = red;
    SendDebug("Moving to: " + String(targetPos));
    stepperSTANDBY_VVI.step(targetPos);
    delay(2000);
    stepperSTANDBY_VVI.step(-targetPos);
  }





}
