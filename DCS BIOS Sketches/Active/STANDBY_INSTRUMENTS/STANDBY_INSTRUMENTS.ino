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
#define Reflector_In_Use 0


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

// These local Mac and IP Address will be reassigned early in startup based on
// the device ID as set by address pins
byte mac[] = {0xA8, 0x61, 0x0A, 0x9E, 0x83, 0x71};
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
U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C u8g2_BARO(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);
U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C u8g2_ALT(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);


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
#define ALT_ZERO_SENSE_PIN 5
#define ALT_OFFSET_TO_ZERO_POINT 708

//#include <AccelStepper.h>
#include <Stepper.h>
#define  STEPS  720    // steps per revolution (limited to 315°)

// Going in the incorrect direction
// #define  COIL_STANDBY_ALT_A1  41 // STANDBY ALT
// #define  COIL_STANDBY_ALT_A2  43
// #define  COIL_STANDBY_ALT_A3  45
// #define  COIL_STANDBY_ALT_A4  47

// Original Standby PCB #define  COIL_STANDBY_ALT_A1  41 // STANDBY ALT
// Original Standby PCB #define  COIL_STANDBY_ALT_A2  43
// Original Standby PCB #define  COIL_STANDBY_ALT_A3  47
// Original Standby PCB #define  COIL_STANDBY_ALT_A4  45

#define  COIL_STANDBY_ALT_A1  38 // STANDBY ALT
#define  COIL_STANDBY_ALT_A2  40
#define  COIL_STANDBY_ALT_A3  44
#define  COIL_STANDBY_ALT_A4  42


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
// Original Standby PCB #define AIRSPEED_ZERO_SENSE_PIN 40
#define AIRSPEED_ZERO_SENSE_PIN A14
#define AIRSPEED_OFFSET_TO_ZERO_POINT 710 


// Original Standby PCB #define  COIL_AIRSPEED_A1  32
// Original Standby PCB #define  COIL_AIRSPEED_A2  36
// Original Standby PCB #define  COIL_AIRSPEED_A3  38
// Original Standby PCB #define  COIL_AIRSPEED_A4  34

#define  COIL_AIRSPEED_A1  14
#define  COIL_AIRSPEED_A2  16
#define  COIL_AIRSPEED_A3  17
#define  COIL_AIRSPEED_A4  15

int STANDBY_AIRSPEED = 0;
int LastSTANDBY_AIRSPEED = 0;
unsigned int valAIRSPEED = 0;
unsigned int posAIRSPEED = 0;

Stepper stepperSTANDBY_AIRSPEED(STEPS , COIL_AIRSPEED_A1, COIL_AIRSPEED_A2, COIL_AIRSPEED_A3, COIL_AIRSPEED_A4);


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

#define  COIL_VVI_A1  30
#define  COIL_VVI_A2  34
#define  COIL_VVI_A3  36
#define  COIL_VVI_A4  32

int STANDBY_VVI = 0;
int LastSTANDBY_VVI = 0;
long valVVI = 0;
long posVVI = 0;

Stepper stepperSTANDBY_VVI(STEPS , COIL_VVI_A1, COIL_VVI_A2, COIL_VVI_A3, COIL_VVI_A4);



void SendDebug( String MessageToSend) {
  if ((Reflector_In_Use == 1) &&  (Ethernet_In_Use == 1)) {
    udp.beginPacket(reflectorIP, reflectorport);
    udp.println(MessageToSend);
    udp.endPacket();
  }
}


// Original Standby PCB DcsBios::RotaryEncoder saiSet("SAI_SET", "-3200", "+3200", 23, 25);
// Original Standby PCB DcsBios::Switch2Pos saiTestBtn("SAI_TEST_BTN", 29);
// Original Standby PCB DcsBios::Switch2Pos saiCage("SAI_CAGE", 27);
// Original Standby PCB DcsBios::RotaryEncoder stbyPressAlt("STBY_PRESS_ALT", "-3200", "+3200", 37, 39);

DcsBios::RotaryEncoder saiSet("SAI_SET", "-3200", "+3200", 28, 26);
// Unsure if the following two arecorrectly mapped
DcsBios::Switch2Pos saiTestBtn("SAI_TEST_BTN", A12);
DcsBios::Switch2Pos saiCage("SAI_CAGE", A13);
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

  Wire.begin();

  for (uint8_t t = 0; t < 8; t++) {
    SendDebug("TCA Port #" + String(t));
    tcaselect(t);

    for (uint8_t addr = 0; addr <= 127; addr++) {
      //if (addr == TCAADDR) continue;

      uint8_t data;
      if (! twi_writeTo(addr, &data, 0, 1, 1)) {
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
  u8g2_BARO.setFont(u8g2_font_fub14_tr );
  u8g2_BARO.sendBuffer();


  tcaselect(ALT_OLED_Port);
  u8g2_ALT.begin();
  u8g2_ALT.clearBuffer();
  //u8g2_ALT.setFont(u8g2_DcsFontHornet4_BIOS_09_tf);
  //u8g2_ALT.setFont(u8g2_font_t0_11_t_all);
  u8g2_ALT.setFont(u8g2_font_fub20_tr  );
  u8g2_ALT.sendBuffer();

  updateALT("0", "0");
  updateBARO("2992");

  SendDebug("Looking for Altimeter Zero");
  pinMode(ALT_ZERO_SENSE_PIN,  INPUT_PULLUP);


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
  pinMode(AIRSPEED_ZERO_SENSE_PIN,  INPUT_PULLUP);


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
  pinMode(VVI_ZERO_SENSE_PIN,  INPUT_PULLUP);


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
}

void updateBARO(String strnewValue) {

  const char* newValue = strnewValue.c_str();
  tcaselect(BARO_OLED_Port);
  u8g2_BARO.setFontMode(0);
  u8g2_BARO.setDrawColor(0);
  u8g2_BARO.drawBox(0, 0, 128 , 32);
  u8g2_BARO.setDrawColor(1);
  u8g2_BARO.setFontDirection(2);
  u8g2_BARO.drawStr(115, 0, newValue);
  u8g2_BARO.sendBuffer();
}

void buildBAROString() {

  updateBARO( BaroThousands + BaroHundreds + BaroTens + BaroOnes);
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
  u8g2_ALT.drawBox(0, 0, 128 , 32);
  u8g2_ALT.setDrawColor(1);
  //u8g2_ALT.drawStr(5, 32, newValue);

  if (strTenThousands == "0") {
    u8g2_ALT.setDrawColor(1);

    u8g2_ALT.drawBox(Start_X_Pos, 13, Box_Width , 20);
    u8g2_ALT.setDrawColor(0);

    u8g2_ALT.drawLine(Start_X_Pos, Start_Y_Pos, End_X_Pos , 32);
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
  unsigned int tempvar  = 0;
  int Alt100s = 0;

  tempvar = int((newValue % 10000) / 1000) ;


  if (tempvar != LastAlt1000s ) {
    LastAlt1000s = tempvar;
    Alt1000s = String(tempvar);
    AltCounterUpdated = true;
  }

  tempvar = int((newValue % 100000) / 10000) ;
  if (tempvar != LastAlt10000s ) {
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
  if (newValue < 6553 ) BaroOnes = "0";
  else if ( newValue < 13106 ) BaroOnes = "1" ;
  else if ( newValue < 16301 ) BaroOnes = "2" ;
  else if ( newValue < 19660 ) BaroOnes = "3" ;
  else if ( newValue < 29918 ) BaroOnes = "4" ;
  else if ( newValue < 36727 ) BaroOnes = "5" ;
  else if ( newValue < 43536 ) BaroOnes = "6" ;
  else if ( newValue < 50345 ) BaroOnes = "7" ;
  else if ( newValue < 53284 ) BaroOnes = "8" ;
  else BaroOnes = "9" ;


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
  if (newValue < 6553 ) BaroTens = "0";
  else if ( newValue < 13106 ) BaroTens = "1" ;
  else if ( newValue < 16301 ) BaroTens = "2" ;
  else if ( newValue < 19660 ) BaroTens = "3" ;
  else if ( newValue < 29918 ) BaroTens = "4" ;
  else if ( newValue < 36727 ) BaroTens = "5" ;
  else if ( newValue < 43536 ) BaroTens = "6" ;
  else if ( newValue < 50345 ) BaroTens = "7" ;
  else if ( newValue < 53284 ) BaroTens = "8" ;
  else BaroTens = "9" ;


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

  // SendDebug("Returning from CalculateVVIPosition: " + String(newPosition));
  return (newPosition);

}




void loop() {


  DcsBios::loop();

  // Standby Altimeter
  //###########################################################################################

  if (valAltimeter > posAltimeter) {
    stepperSTANDBY_ALT.step(1);      // move one step to the left.
    posAltimeter++;
    // SendDebug(String(valAltimeter) + " Inc v2 Altimeter " + String(posAltimeter));
  }
  else if (valAltimeter < posAltimeter) {
    stepperSTANDBY_ALT.step(-1);       // move one step to the right.
    posAltimeter--;
    //SendDebug(String(valAltimeter) + " Dec v2 Altimeter " + String(posAltimeter));
  }

  //###########################################################################################


  // VVI
  //###########################################################################################

  if (valVVI > posVVI) {
    stepperSTANDBY_VVI.step(1);      // move one step to the left.
    posVVI++;
    // SendDebug(String(valVVI) + " Inc VVI " + String(posVVI));
  }
  else if (valVVI < posVVI) {
    stepperSTANDBY_VVI.step(-1);       // move one step to the right.
    posVVI--;
    //SendDebug(String(valVVI) + " Dec VVI " + String(posVVI));
  }

  //###########################################################################################


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



  if (BaroUpdated == true) buildBAROString();
  if (AltCounterUpdated == true) updateALT(Alt10000s, Alt1000s );



  if (millis() >= nextupdate) {
    outputstate = !outputstate;
    digitalWrite(RedLedMonitorPin, outputstate);
    nextupdate = millis() + flashinterval;


  }


}
