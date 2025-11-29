// Source
// https://gist.github.com/jboecker/1084b3768c735b164c34d6087d537c18

// For reasons Im yet to work out when including
// the encoder for dcsbios I'm getting the folowing when uploading to a mega
// avrdude: verification error; content mismatch
// problem disappeared when altimeter removed and after reboot root cause tba


// the Warthog Project Video on the compass build
// https://www.youtube.com/watch?v=ZN9glqgp9TY&t=332s

// Zero Offsets for steppers



#define Ethernet_In_Use 0
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


// These local Mac and IP Address will be reassigned early in startup based on
// the device ID as set by address pins
byte mac[] = {0xA8, 0x61, 0x0A, 0x9E, 0x83, 0x01};
IPAddress ip(172, 16, 1, 101);
String strMyIP = "172.16.1.101";

// Raspberry Pi is Target
IPAddress reflectorIP(172, 16, 1, 10);
String strreflectorIP = "X.X.X.X";

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


#include <Stepper.h>
#define  STEPS  720    // steps per revolution (limited to 315°)



// Airspeed
unsigned long nextAirSpeedUpdate = 0;
int updateAirSpeedInterval = 100;
#define AIRSPEED_ZERO_SENSE_PIN 9
#define AIRSPEED_OFFSET_TO_ZERO_POINT 710 

// Going in the incorrect direction
// #define  COIL_AIRSPEED_A1  32
// #define  COIL_AIRSPEED_A2  36
// #define  COIL_AIRSPEED_A3  34
// #define  COIL_AIRSPEED_A4  38

// #define  COIL_AIRSPEED_A1  2
// #define  COIL_AIRSPEED_A2  12
// #define  COIL_AIRSPEED_A3  11
// #define  COIL_AIRSPEED_A4  12

#define  COIL_AIRSPEED_A1  2
#define  COIL_AIRSPEED_A2  3
#define  COIL_AIRSPEED_A3  12
#define  COIL_AIRSPEED_A4  11

// #define  COIL_AIRSPEED_A1  2
// #define  COIL_AIRSPEED_A2  11
// #define  COIL_AIRSPEED_A3  3
// #define  COIL_AIRSPEED_A4  12

// #define  COIL_AIRSPEED_A1  2
// #define  COIL_AIRSPEED_A2  3
// #define  COIL_AIRSPEED_A3  11
// #define  COIL_AIRSPEED_A4  12

//AccelStepper stepper(AccelStepper::FULL4WIRE, 2, 11, 3, 12);

int STANDBY_AIRSPEED = 0;
int LastSTANDBY_AIRSPEED = 0;
unsigned int valAIRSPEED = 0;
unsigned int posAIRSPEED = 0;

Stepper stepperSTANDBY_AIRSPEED(STEPS , COIL_AIRSPEED_A1, COIL_AIRSPEED_A2, COIL_AIRSPEED_A3, COIL_AIRSPEED_A4);



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



  STANDBY_AIRSPEED = 0;

  outputstate = false;
  digitalWrite(RedLedMonitorPin, outputstate);
  digitalWrite(GreenLedMonitorPin, outputstate);

  DcsBios::setup();

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



  SendDebug("Looking for Airspeed Zero");
  pinMode(AIRSPEED_ZERO_SENSE_PIN,  INPUT_PULLUP);


  stepperSTANDBY_AIRSPEED.setSpeed(40);
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




}





void onStbyAsiAirspeedChange(unsigned int newValue) {
    valAIRSPEED = map(newValue, 0, 65535, 0, 660);
}
DcsBios::IntegerBuffer stbyAsiAirspeedBuffer(0x74f0, 0xffff, 0, onStbyAsiAirspeedChange);



int CalculateAirspeedPosition( int newValue) {

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

  // SendDebug("Returning from CalculateAirSpeedPosition: " + String(newPosition));
  return (newPosition);

}





void loop() {


  DcsBios::loop();

 

  // Airspeed
  //###########################################################################################

  if (valAIRSPEED > posAIRSPEED) {
    stepperSTANDBY_AIRSPEED.step(1);      // move one step to the left.
    posAIRSPEED++;
    // SendDebug(String(valAIRSPEED) + " Inc Airspeed " + String(posAIRSPEED));
  }
  else if (valAIRSPEED < posAIRSPEED) {
    stepperSTANDBY_AIRSPEED.step(-1);       // move one step to the right.
    posAIRSPEED--;
    //SendDebug(String(valAIRSPEED) + " Dec Airspeed " + String(posAIRSPEED));
  }

  //###########################################################################################




  if (millis() >= nextupdate) {
    outputstate = !outputstate;
    digitalWrite(RedLedMonitorPin, outputstate);
    nextupdate = millis() + flashinterval;


  }


}
