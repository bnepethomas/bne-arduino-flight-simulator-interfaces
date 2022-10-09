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

unsigned long nextAltimeterUpdate = 0;
int updateAltimeterInterval = 100;
#define ALT_ZERO_SENSE_PIN 49

#include <AccelStepper.h>
#define  STEPS  720    // steps per revolution (limited to 315°)
//
//Working but direction in incorrect
//#define  COIL_STANDBY_ALT_A1  45 // STANDBY ALT
//#define  COIL_STANDBY_ALT_A2  47
//#define  COIL_STANDBY_ALT_A3  41
//#define  COIL_STANDBY_ALT_A4  43

#define  COIL_STANDBY_ALT_A1  41 // STANDBY ALT
#define  COIL_STANDBY_ALT_A2  43
#define  COIL_STANDBY_ALT_A3  45
#define  COIL_STANDBY_ALT_A4  47

int STANDBY_ALT = 0;
int LastSTANDBY_ALT = 0;

// AccelStepper stepperSTANDBY_ALT(STEPS, COIL_STANDBY_ALT_A1, COIL_STANDBY_ALT_A2, COIL_STANDBY_ALT_A3, COIL_STANDBY_ALT_A4); // RAD ALT
AccelStepper stepperSTANDBY_ALT(AccelStepper::FULL4WIRE , COIL_STANDBY_ALT_A1, COIL_STANDBY_ALT_A2, COIL_STANDBY_ALT_A3, COIL_STANDBY_ALT_A4); // RAD ALT

void SendDebug( String MessageToSend) {
  if ((Reflector_In_Use == 1) &&  (Ethernet_In_Use == 1)) {
    udp.beginPacket(reflectorIP, reflectorport);
    udp.println(MessageToSend);
    udp.endPacket();
  }
}

// DcsBios::RotaryEncoder saiSet("SAI_SET", "-3200", "+3200", 37, 39);
DcsBios::RotaryEncoder stbyPressAlt("STBY_PRESS_ALT", "-3200", "+3200", 37, 39);
DcsBios::Switch2Pos lightsTestSw("LIGHTS_TEST_SW", 8);
DcsBios::LED lsLock(0x7408, 0x0001, 13);


void setup() {

  pinMode(RedLedMonitorPin, OUTPUT);
  pinMode(GreenLedMonitorPin, OUTPUT);
  outputstate = true;
  digitalWrite(RedLedMonitorPin, outputstate);
  digitalWrite(GreenLedMonitorPin, outputstate);



  //  stepperSTANDBY_ALT.setMaxSpeed(4000.0);
  //  stepperSTANDBY_ALT.setAcceleration(2000.0);
  stepperSTANDBY_ALT.setMaxSpeed(1000.0);
  stepperSTANDBY_ALT.setAcceleration(1000.0);
  stepperSTANDBY_ALT.runToNewPosition(500);
  STANDBY_ALT = 0;


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
    // Had to comment out these debugging messages as they created a conflict with the IRQ definition in DCS BIOS


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
  // Had to comment out these debugging messages as they created a conflict with the IRQ definition in DCS BIOS
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

  SendDebug("Looking for Zero");
  stepperSTANDBY_ALT.setCurrentPosition(0);
  pinMode(ALT_ZERO_SENSE_PIN,  INPUT_PULLUP);


  for (int i = 0; i <= 2000; i++) {
    delay(1);
    stepperSTANDBY_ALT.moveTo(i);
    stepperSTANDBY_ALT.run();
    if (digitalRead(ALT_ZERO_SENSE_PIN) == false) {
      SendDebug("Found Zero");
      stepperSTANDBY_ALT.setCurrentPosition(0);
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


  if (millis() >= nextAltimeterUpdate) {


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


    unsigned int targetLocation = int (newValue * 0.72);

    // SendDebug("Pointer Target = " + String(targetLocation));
    stepperSTANDBY_ALT.runToNewPosition(targetLocation);

    nextAltimeterUpdate =  millis() + updateAltimeterInterval;
  }
}
DcsBios::IntegerBuffer altMslFtBuffer(0x0434, 0xffff, 0, onAltMslFtChange);



void onVsiChange(unsigned int newValue) {
  // VVI Range -6000 to +6000
  // 65535  +6000
  // 32768  0
  // 0      -6000

  int VSIRate = 0;
  

  if (newValue >= 32768) {
    VSIRate = int((-32768 + newValue)/32);
  } else {
    VSIRate = int((32768 -newValue)/32);
  }

  SendDebug("VVI " + String(newValue) + " Translated Rate" + String(VSIRate));
  stepperSTANDBY_ALT.setMaxSpeed(VSIRate);
  stepperSTANDBY_ALT.setAcceleration(VSIRate);
  

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
void loop() {

  stepperSTANDBY_ALT.run();
  DcsBios::loop();

  if (millis() >= nextupdate) {
    outputstate = !outputstate;
    digitalWrite(RedLedMonitorPin, outputstate);
    nextupdate = millis() + flashinterval;

    if (BaroUpdated == true) buildBAROString();
    if (AltCounterUpdated == true) updateALT(Alt10000s, Alt1000s );

  }
}
