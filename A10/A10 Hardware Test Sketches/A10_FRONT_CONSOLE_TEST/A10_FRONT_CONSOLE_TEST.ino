/* A10_FRONT_CONSOLE_test

*/
#define Ethernet_In_Use 1
const int Serial_In_Use = 0;
#define Reflector_In_Use 1


// ###################################### Begin Ethernet Related #############################
#include <SPI.h>
#include <Ethernet.h>
#include <EthernetUdp.h>

// These local Mac and IP Address will be reassigned early in startup based on
// the device ID as set by address pins
#define EthernetStartupDelay 500
#define ES1_RESET_PIN 53

byte mac[] = { 0xA8, 0x61, 0x0A, 0x67, 0x83, 0x01 };
String sMac = "A8:61:0A:67:83:01";
IPAddress ip(172, 16, 1, 101);
String strMyIP = "172.16.1.101";

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





// THE LED PORTS WILL CHANGE FROM THE V1.1 PCB TO THE FOLLOWING
#define RED_STATUS_LED_PORT 12
#define GREEN_STATUS_LED_PORT 13
//#define RED_STATUS_LED_PORT 6
//#define GREEN_STATUS_LED_PORT 5
#define FLASH_TIME 1000

unsigned long NEXT_STATUS_TOGGLE_TIMER = 0;
bool GREEN_LED_STATE = false;
bool RED_LED_STATE = false;
unsigned long timeSinceRedLedChanged = 0;

#define DCSBIOS_IRQ_SERIAL



// ###################################### Begin Servo Related #############################
#include <AccelStepper.h>
#include <Stepper.h>
#define STEPS 720  // steps per revolution (limited to 315°)

// Holding current per coil is 14mA, which gives 28mA per stepper
// Mega absolute max is 40mA per pin, with a total max of 200mA
// Gives a total


// Works but slow max speed of 30
// #define COIL_RIGHT_HYD_A1 23
// #define COIL_RIGHT_HYD_A2 25
// #define COIL_RIGHT_HYD_A3 27
// #define COIL_RIGHT_HYD_A4 29
// //  stepperSTANDBY_ALT.setMaxSpeed(30);

// Works but slow max speed of 30
// #define COIL_RIGHT_HYD_A1 25
// #define COIL_RIGHT_HYD_A2 23
// #define COIL_RIGHT_HYD_A3 27
// #define COIL_RIGHT_HYD_A4 29

// Works but slow max speed of 30

// If the stepper is incorrectly configured it will chatter above
// rates of more than 30 steps per minute, if when correctly
// configured speeds can exceed 2300


#define COIL_LEFT_HYD_A1 22
#define COIL_LEFT_HYD_A2 24
#define COIL_LEFT_HYD_A3 26
#define COIL_LEFT_HYD_A4 28

// #define COIL_LEFT_HYD_A1 22
// #define COIL_LEFT_HYD_A2 26
// #define COIL_LEFT_HYD_A3 24
// #define COIL_LEFT_HYD_A4 28

#define COIL_RIGHT_HYD_A1 23
#define COIL_RIGHT_HYD_A2 25
#define COIL_RIGHT_HYD_A3 27
#define COIL_RIGHT_HYD_A4 29

// #define COIL_RIGHT_HYD_A1 23
// #define COIL_RIGHT_HYD_A2 27
// #define COIL_RIGHT_HYD_A3 25
// #define COIL_RIGHT_HYD_A4 29

#define COIL_LEFT_FUEL_A1 30
#define COIL_LEFT_FUEL_A2 34
#define COIL_LEFT_FUEL_A3 32
#define COIL_LEFT_FUEL_A4 36

#define COIL_RIGHT_FUEL_A1 31
#define COIL_RIGHT_FUEL_A2 35
#define COIL_RIGHT_FUEL_A3 33
#define COIL_RIGHT_FUEL_A4 37

#define COIL_OXY_REG_A1 38
#define COIL_OXY_REG_A2 42
#define COIL_OXY_REG_A3 40
#define COIL_OXY_REG_A4 44

#define COIL_LOX_A1 39
#define COIL_LOX_A2 43
#define COIL_LOX_A3 41
#define COIL_LOX_A4 45

// Pins are slighty Reversed when compared to steppers on Expansion connection
#define COIL_CABIN_PRESS_A1 9
#define COIL_CABIN_PRESS_A2 7
#define COIL_CABIN_PRESS_A3 8
#define COIL_CABIN_PRESS_A4 6


// #define STEPPER_MAX_SPEED 900
#define STEPPER_MAX_SPEED 8300
#define STEPPER_ACCELERATION 2000

AccelStepper STEPPER_LEFT_HYD(AccelStepper::FULL4WIRE, COIL_LEFT_HYD_A1, COIL_LEFT_HYD_A2, COIL_LEFT_HYD_A3, COIL_LEFT_HYD_A4);
AccelStepper STEPPER_RIGHT_HYD(AccelStepper::FULL4WIRE, COIL_RIGHT_HYD_A1, COIL_RIGHT_HYD_A2, COIL_RIGHT_HYD_A3, COIL_RIGHT_HYD_A4);
AccelStepper STEPPER_LEFT_FUEL(AccelStepper::FULL4WIRE, COIL_LEFT_FUEL_A1, COIL_LEFT_FUEL_A2, COIL_LEFT_FUEL_A3, COIL_LEFT_FUEL_A4);
AccelStepper STEPPER_RIGHT_FUEL(AccelStepper::FULL4WIRE, COIL_RIGHT_FUEL_A1, COIL_RIGHT_FUEL_A2, COIL_RIGHT_FUEL_A3, COIL_RIGHT_FUEL_A4);
AccelStepper STEPPER_OXY_REG(AccelStepper::FULL4WIRE, COIL_OXY_REG_A1, COIL_OXY_REG_A2, COIL_OXY_REG_A3, COIL_OXY_REG_A4);
AccelStepper STEPPER_LOX(AccelStepper::FULL4WIRE, COIL_LOX_A1, COIL_LOX_A2, COIL_LOX_A3, COIL_LOX_A4);
AccelStepper STEPPER_CABIN_PRESS(AccelStepper::FULL4WIRE, COIL_CABIN_PRESS_A1, COIL_CABIN_PRESS_A2, COIL_CABIN_PRESS_A3, COIL_CABIN_PRESS_A4);

// ###################################### End Stepper Related #############################


// ###################################### Begin I2C Related ###############################
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

void scanI2CBus() {
  for (uint8_t t = 0; t < 8; t++) {
    tcaselect(t);
    // Had to comment out these debugging messages as they created a conflict with the IRQ definition in DCS BIOS
    //Serial.print("TCA Port #"); Serial.println(t);

    for (uint8_t addr = 0; addr <= 127; addr++) {
      //if (addr == TCAADDR) continue;

      uint8_t data;
      if (!twi_writeTo(addr, &data, 0, 1, 1)) {
        // Had to comment out these debugging messages as they created a conflict with the IRQ definition in DCS BIOS

        SendDebug("Found I2C 0x" + String(addr));  // Serial.println(addr,HEX);
      }
    }
  }
  // Had to comment out these debugging messages as they created a conflict with the IRQ definition in DCS BIOS
  //Serial.println("\nI2C scan complete");
}


#define VHF_FM_PORT 0
#define VHF_AM_PORT 1
#define UHF_PORT 2
#define MWS_PORT 3
#define CMSP_PORT 4
#define ILS_PORT 5
#define TACAN_PORT 6
#define SPARE_PORT 7


void send_VHF_FM(String stringToDisplay) {
  tcaselect(VHF_FM_PORT);
  setCursor(0, 0);
  sendString(stringToDisplay);
}

void send_VHF_AM(String stringToDisplay) {
  tcaselect(VHF_AM_PORT);
  setCursor(0, 0);
  sendString(stringToDisplay);
}


void send_UHF(String stringToDisplay) {
  tcaselect(UHF_PORT);
  setCursor(0, 0);
  sendString(stringToDisplay);
}

void send_MWS(String stringToDisplay) {
  tcaselect(MWS_PORT);
  setCursor(0, 0);
  sendString(stringToDisplay);
}

void send_CMSP(String stringToDisplay) {
  tcaselect(CMSP_PORT);
  setCursor(0, 0);
  sendString(stringToDisplay);
}

void send_ILS(String stringToDisplay) {
  tcaselect(ILS_PORT);
  setCursor(0, 0);
  sendString(stringToDisplay);
}

void send_TACAN(String stringToDisplay) {
  tcaselect(TACAN_PORT);
  setCursor(0, 0);
  sendString(stringToDisplay);
}

void send_SPARE(String stringToDisplay) {
  tcaselect(SPARE_PORT);
  setCursor(0, 0);
  sendString(stringToDisplay);
}

void displayOLEDPortUsage() {
  for (uint8_t t = 0; t < 8; t++) {
    tcaselect(t);
    switch (t) {
      case VHF_FM_PORT:
        sendString("VHF FM");
        break;
      case VHF_AM_PORT:
        sendString("VHF AM");
        break;
      case UHF_PORT:
        sendString("UHF PORT");
        break;
      case MWS_PORT:
        sendString("MWS PORT");
        break;
      case CMSP_PORT:
        sendString("CMSP PORT");
        break;
      case ILS_PORT:
        sendString("ILS PORT");
        break;
      case TACAN_PORT:
        sendString("TACAN PORT");
        break;
      case SPARE_PORT:
        sendString("SPARE PORT");
        break;
      default:
        sendString("UNASSIGNED PORT " + String(t));
        break;
    }
  }
}


//////////////////////////////////////////////////////////////////////////////
//                                                                         //
//              OLED INIT                                                  //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////
// OLED Library source - https://forum.arduino.cc/t/setting-up-and-using-2x16-2x20-and-4x20-i2c-oled-displays-by-wide-hk/299364
//  WIDE.HK_I2C_OLED_EXAMPLE
#define OLED_Address 0x3c
#define OLED_Command_Mode 0x80
#define OLED_Data_Mode 0x40


void init_oled() {
  // there are 3 command sets, Fundamental (RE and SD =0), Extended (RE=1), and OLED (RE=1 and SD=1)
  // we switch back and forth between the command tables  and i try to put extra lines beteen the switches

  sendCommand(0x2A);  // function set (extended command set)     RE = 1  (Extended ON)
  sendCommand(0x71);  // function selection A,
  sendData(0x5C);     //  enable internal Vdd regulator (if  you use 4.5-5.5  volts)
  sendCommand(0x28);  // function set (fundamental command set)  RE = 0  (Extended OFF)

  sendCommand(0x08);  // display off, cursor off, blink off

  sendCommand(0x2A);  // function set (extended command set) RE = 1   (Extended ON)
  sendCommand(0x79);  // OLED command set enabled            SD=1     (OLED ON)

  sendCommand(0xD5);  // set display clock divide ratio/oscillator frequency
  sendCommand(0x70);  // Power on Reset (POR) Value

  sendCommand(0x78);  // OLED command set disabled              SD=0    (OLED OFF)  (RE still on)

  sendCommand(0x09);  // extended function set (4-lines)  (0x08 for 2 lines)
  sendCommand(0x06);  // COM SEG direction
  sendCommand(0x72);  // function selection B, CROM, CGRAM selection -  3 CROMs   0= A  4 =B  8 = C
  sendData(0x00);     // 0Rd with CGRAM Selection lower 2 bits  00: 240/8, 01:248/8, 02:250/6, 03: 256/0
  // ROM A            // so the values for ROMs A,B,C could be: 00,04,08 - 01,05,09 - 02,06,0A  or  03,07,0B

  sendCommand(0x2A);  // function set (extended command set)     RE = 1   (Extended ON)
  sendCommand(0x79);  // OLED command set enabled                SD = 1   (OLED ON)

  sendCommand(0xDA);  // set SEG pins hardware configuration
  sendCommand(0x10);  // to POR (power on Reset) values

  sendCommand(0xDC);  // function selection C
  sendCommand(0x00);  // SET VSL & GPIO  internal VSL (POR), and  HI-Z GPIO

  sendCommand(0x81);  // set contrast control
  sendCommand(0xEF);  // 00-ff   EF seems good enough

  sendCommand(0xD9);  // set phase length
  sendCommand(0x78);  // POR values

  sendCommand(0xDB);  // set VCOMH deselect level
  sendCommand(0x30);  // ~0.83 x Vcc

  sendCommand(0x78);  // OLED command set disabled               SD = 0   (OLED OFF)
  sendCommand(0x28);  // function set (fundamental command set)  RE = 0   (Extended OFF)

  sendCommand(0x01);  // clear display
  sendCommand(0x80);  // set DDRAM address to 0x00
  sendCommand(0x0C);  // display ON
}

///////////////////////////////////////////////////////////////////////////////
//
//      END   init_oled()
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  sendData(unsigned char data
//
////////////////////////////////////////////////////////////////////////////////
void sendData(unsigned char data) {
  Wire.beginTransmission(OLED_Address);
  Wire.write(OLED_Data_Mode);
  Wire.write(data);
  Wire.endTransmission();
}
/////////////////////////////////////////////////////////////////////////////////
//
//    END  send Data(unsigned char data
//
/////////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////////////////
//
//    sendCommand(unsigned char command)
//
/////////////////////////////////////////////////////////////////////////////////////////
void sendCommand(unsigned char command) {
  Wire.beginTransmission(OLED_Address);
  Wire.write(OLED_Command_Mode);
  Wire.write(command);
  Wire.endTransmission();
  delay(10);
}
/////////////////////////////////////////////////////////////////////////////////////////
//
//    END  sendCommand(unsigned char command)
//
/////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////
//
//       sendString(String)
//
///////////////////////////////////////////////////////////////////////////////////////
void sendString(String workstring) {
  int str_len = workstring.length() + 1;
  char char_array[str_len];
  workstring.toCharArray(char_array, str_len);
  sendCharArray(char_array);
}
////////////////////////////////////////////////////////////////////////////////////////
//
//       END  sendString(String)
//
///////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////
//
//       sendCharArray(const char *String)
//
///////////////////////////////////////////////////////////////////////////////////////
void sendCharArray(const char *String) {
  unsigned char i = 0;
  while (String[i]) sendData(String[i++]);  // *** Send String to OLED
}
////////////////////////////////////////////////////////////////////////////////////////
//
//       END  sendCharArray(const char *String)
//
///////////////////////////////////////////////////////////////////////////////////////



/////////////////////////////////////////////////////////////////////////////////////////////
//
//   setCursor(uint8_t col, uint8_t row)
//
/////////////////////////////////////////////////////////////////////////////////////////////
void setCursor(uint8_t col, uint8_t row) {
  int row_offsets[] = { 0x00, 0x20, 0x40, 0x60 };  // these offsetts are from the ROM char table map of the SSD1311 spec
  sendCommand(0x80 | (col + row_offsets[row]));    // and will work for all displays as long as you don't try to write to line 3 of a 2 line display, etc.
}
/////////////////////////////////////////////////////////////////////////////////////////////
//
//   END  setCursor(uint8_t col, uint8_t row)
//
/////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////
//
//      sendFloat(float digit, uint8_t dec, uint8_t nad, uint8_t col, uint8_t row)
//
////////////////////////////////////////////////////////////////////////////////////////
void sendFloat(float digit, uint8_t width, uint8_t precision, uint8_t col, uint8_t row) {
  char line[10];
  dtostrf(digit, width, precision, line);  //Convert the float value to a string
  setCursor(col, row);
  sendString(line);
}
////////////////////////////////////////////////////////////////////////////////////////
//
//      END  sendFloat(float digit, uint8_t dec, uint8_t nad, uint8_t col, uint8_t row)
//
////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////
//
//          clearOLED()
//
//////////////////////////////////////////////////////////////////////////////////////
void clearOLED() {
  sendCommand(0x01);  // clear dispaly (fill with blanks)
  sendCommand(0x02);  // set addr counter to 0
}
///////////////////////////////////////////////////////////////////////////////////////
//
//         END clearOLED()
//
//////////////////////////////////////////////////////////////////////////////////////




// ######################################## End I2C Related ###############################


void setup() {
  // put your setup code here, to run once:
  pinMode(RED_STATUS_LED_PORT, OUTPUT);
  pinMode(GREEN_STATUS_LED_PORT, OUTPUT);

  digitalWrite(RED_STATUS_LED_PORT, true);
  digitalWrite(GREEN_STATUS_LED_PORT, true);
  delay(FLASH_TIME);
  digitalWrite(RED_STATUS_LED_PORT, false);
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
  }



  digitalWrite(RED_STATUS_LED_PORT, true);
  digitalWrite(GREEN_STATUS_LED_PORT, true);
  delay(FLASH_TIME);
  digitalWrite(RED_STATUS_LED_PORT, false);
  digitalWrite(GREEN_STATUS_LED_PORT, false);


  // For reasons I'm yet to work out - earlier senddebugs are not sent before this point
  // Testing shows a delay of 3 seconds is needed
  SendDebug("LED Initialisation Complete");

  SendDebug("Starting Motor Initialisation");



  if (false) {
    SendDebug("Start Stepper Left Hyd");
    STEPPER_LEFT_HYD.setMaxSpeed(STEPPER_MAX_SPEED);
    STEPPER_LEFT_HYD.setAcceleration(STEPPER_ACCELERATION);
    STEPPER_LEFT_HYD.move(-630);

    while (STEPPER_LEFT_HYD.distanceToGo() != 0) {
      STEPPER_LEFT_HYD.run();
    }

    STEPPER_LEFT_HYD.move(630);

    while (STEPPER_LEFT_HYD.distanceToGo() != 0) {
      STEPPER_LEFT_HYD.run();
    }
    STEPPER_LEFT_HYD.move(-630);
    while (STEPPER_LEFT_HYD.distanceToGo() != 0) {
      STEPPER_LEFT_HYD.run();
    }
    SendDebug("End Stepper Left Hyd");
  }

  SendDebug("End Motor Initialisation");

  Wire.begin();

  scanI2CBus();


  SendDebug("Initialising OLEDs");
  for (uint8_t t = 0; t < 8; t++) {
    tcaselect(t);
    init_oled();
  }

  SendDebug("Display Startup Text on OLEDs");



  SendDebug("OLED Display Startup Complete");
  delay(2000);
  send_VHF_FM("VHF FM 127.1");
  send_VHF_AM("VHF AM 124.2");
  send_UHF("UHF 330.1");
  send_MWS("MWS AA AG");
  send_CMSP("CMSP ARMED");
  send_ILS("ILS 108.7");
  send_TACAN("TACAN 130.1");
  send_SPARE("SPARE");

#define CAUTION_LAMP A4
#define PRI_LAMP A5
#define SEP_LAMP A6
#define MISSILE_LAMP A7
#define HARS_LAMP A8
#define EGI_LAMP A9
#define TISL_LAMP A10
#define STR_PT_LAMP A11
#define ANCHR_LAMP A12
#define TCN_LAMP A13
#define ILS_LAMP A14
#define ANTI_SKID_COIL A15
#define R_PITCH_COIL 39
#define L_PITCH_COIL 41
#define R_YAW_COIL 43
#define L_YAW_COIL 45

  pinMode(A0, OUTPUT);
  pinMode(A1, OUTPUT);
  pinMode(A2, OUTPUT);
  pinMode(A3, OUTPUT);
  pinMode(CAUTION_LAMP, OUTPUT);
  pinMode(PRI_LAMP, OUTPUT);
  pinMode(SEP_LAMP, OUTPUT);
  pinMode(MISSILE_LAMP, OUTPUT);
  pinMode(HARS_LAMP, OUTPUT);
  pinMode(EGI_LAMP, OUTPUT);
  pinMode(TISL_LAMP, OUTPUT);
  pinMode(STR_PT_LAMP, OUTPUT);
  pinMode(ANCHR_LAMP, OUTPUT);
  pinMode(TCN_LAMP, OUTPUT);
  pinMode(ILS_LAMP, OUTPUT);
  pinMode(ANTI_SKID_COIL, OUTPUT);
  pinMode(R_PITCH_COIL, OUTPUT);
  pinMode(L_PITCH_COIL, OUTPUT);
  pinMode(R_YAW_COIL, OUTPUT);
  pinMode(L_YAW_COIL, OUTPUT);
}

void loop() {

  if (millis() >= NEXT_STATUS_TOGGLE_TIMER) {
    GREEN_LED_STATE = !GREEN_LED_STATE;
    RED_LED_STATE = !RED_LED_STATE;
    digitalWrite(GREEN_STATUS_LED_PORT, GREEN_LED_STATE);

    digitalWrite(A0, GREEN_LED_STATE);
    digitalWrite(A1, GREEN_LED_STATE);
    digitalWrite(A2, GREEN_LED_STATE);
    digitalWrite(A3, GREEN_LED_STATE);
    digitalWrite(CAUTION_LAMP, GREEN_LED_STATE);
    digitalWrite(PRI_LAMP, GREEN_LED_STATE);
    digitalWrite(SEP_LAMP, GREEN_LED_STATE);
    digitalWrite(MISSILE_LAMP, GREEN_LED_STATE);
    digitalWrite(HARS_LAMP, GREEN_LED_STATE);
    digitalWrite(EGI_LAMP, GREEN_LED_STATE);
    digitalWrite(TISL_LAMP, GREEN_LED_STATE);
    digitalWrite(STR_PT_LAMP, GREEN_LED_STATE);
    digitalWrite(ANCHR_LAMP, GREEN_LED_STATE);
    digitalWrite(TCN_LAMP, GREEN_LED_STATE);
    digitalWrite(ILS_LAMP, GREEN_LED_STATE);
    digitalWrite(ANTI_SKID_COIL, GREEN_LED_STATE);
    digitalWrite(R_PITCH_COIL, GREEN_LED_STATE);
    digitalWrite(L_PITCH_COIL, GREEN_LED_STATE);
    digitalWrite(R_YAW_COIL, GREEN_LED_STATE);
    digitalWrite(L_YAW_COIL, GREEN_LED_STATE);



    NEXT_STATUS_TOGGLE_TIMER = millis() + FLASH_TIME;
  }
}
