
#define Ethernet_In_Use 1
const int Serial_In_Use = 0;
#define Reflector_In_Use 1

#define DCSBIOS_In_Use 1
#define DCSBIOS_IRQ_SERIAL
#include <DcsBios.h>

// ###################################### Begin Ethernet Related #############################
#include <SPI.h>
#include <Ethernet.h>
#include <EthernetUdp.h>

// These local Mac and IP Address will be reassigned early in startup based on
// the device ID as set by address pins
#define EthernetStartupDelay 500
#define ES1_RESET_PIN 53

byte mac[] = { 0xA8, 0x61, 0x0A, 0x9E, 0x83, 0x06 };
String sMac = "A8:61:0A:65:83:06";
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

const unsigned long delayBeforeSendingPacket = 2000;
unsigned long ethernetStartTime = 0;

String DebugString = "";
String BoardName = "A10 Right Console Output";
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
// #define RED_STATUS_LED_PORT 12
// #define GREEN_STATUS_LED_PORT 13
#define RED_STATUS_LED_PORT 6
#define GREEN_STATUS_LED_PORT 5
#define Check_LED_G 5
#define Check_LED_R 6
#define FLASH_TIME 300

unsigned long NEXT_STATUS_TOGGLE_TIMER = 0;
bool GREEN_LED_STATE = false;
bool RED_LED_STATE = false;
unsigned long timeSinceRedLedChanged = 0;

#define DCSBIOS_IRQ_SERIAL

// ###################################### Begin Max7219 Related #############################


// Pinouts for Version 4 PCB
#define MAP_LIGHTS 6
#define NVG_LIGHTS 6
#define FLOOD_LIGHTS 7
#define FORMATION_LIGHTS 8
#define STROBE_LIGHTS 44
#define NAVIGATION_LIGHTS 45
#define BACK_LIGHTS 46


#include "LedControl.h"
#include "DcsBios.h"
int devices = 2;


#define DAY_MODE 0
#define NIGHT_MODE 1
#define NVG_MODE 2
#define FULL_BRIGHTNESS 15

#define STROBE_BRIGHT 2
#define STROBE_DIM 0
#define STROBE_BRIGHT_LEVEL 255
#define STROBE_DIM_LEVEL 20

int WARN_CAUTION_DIMMER_VALUE = 15;
int AOA_DIMMER_VALUE = 15;
int DAY_NIGHT_SWITCH_MODE = DAY_MODE;
int NEW_AOA_DIMMER_VALUE = 15;
int STROBE_BRIGHT_SWITCH_POS = STROBE_BRIGHT;
long POSITION_BRIGHT_POT_POS = 65534;
bool POSITION_LIGHTS_STATUS = true;

LedControl lc = LedControl(16, 14, 15, devices);

/* paste code snippets from the reference documentation here */
//DcsBios::Switch2Pos lightsTestSw("LIGHTS_TEST_SW", 22);
//DcsBios::LED sjCtrLt(0x742e, 0x4000, 13);
// CHIP Max7219-A
#define UPPER_CAUTION_PANEL 0

// CHIP MAX7219-B
#define LOWER_CAUTION_PANEL 1


#define ENG_START_CYCLE_COL_A 0
#define ENG_START_CYCLE_ROW_A 1
#define ENG_START_CYCLE_COL_B 0
#define ENG_START_CYCLE_ROW_B 2
void setLEDEngStartcycle(unsigned int newValue) {
  lc.setLed(UPPER_CAUTION_PANEL, ENG_START_CYCLE_COL_A, ENG_START_CYCLE_ROW_A, newValue);
  lc.setLed(UPPER_CAUTION_PANEL, ENG_START_CYCLE_COL_B, ENG_START_CYCLE_ROW_B, newValue);
}
void onClA1Change(unsigned int newValue) {
  setLEDEngStartcycle(newValue);
}
DcsBios::IntegerBuffer clA1Buffer(0x10d4, 0x0001, 0, onClA1Change);


#define L_HYD_PRESS_COL_A 0
#define L_HYD_PRESS_ROW_A 3
#define L_HYD_PRESS_COL_B 0
#define L_HYD_PRESS_ROW_B 4
void setLEDLeftHydPress(unsigned int newValue) {
  lc.setLed(UPPER_CAUTION_PANEL, L_HYD_PRESS_COL_A, L_HYD_PRESS_ROW_A, newValue);
  lc.setLed(UPPER_CAUTION_PANEL, L_HYD_PRESS_COL_B, L_HYD_PRESS_ROW_B, newValue);
}
void onClA2Change(unsigned int newValue) {
  setLEDLeftHydPress(newValue);
}
DcsBios::IntegerBuffer clA2Buffer(0x10d4, 0x0002, 1, onClA2Change);


#define R_HYD_PRESS_COL_A 0
#define R_HYD_PRESS_ROW_A 5
#define R_HYD_PRESS_COL_B 0
#define R_HYD_PRESS_ROW_B 6
void setLEDRightHydPress(unsigned int newValue) {
  lc.setLed(UPPER_CAUTION_PANEL, R_HYD_PRESS_COL_A, R_HYD_PRESS_ROW_A, newValue);
  lc.setLed(UPPER_CAUTION_PANEL, R_HYD_PRESS_COL_B, R_HYD_PRESS_ROW_B, newValue);
}
void onClA3Change(unsigned int newValue) {
  setLEDRightHydPress(newValue);
}
DcsBios::IntegerBuffer clA3Buffer(0x10d4, 0x0004, 2, onClA3Change);


#define GUN_UNSAFE_COL_A 0
#define GUN_UNSAFE_ROW_A 7
#define GUN_UNSAFE_COL_B 0
#define GUN_UNSAFE_ROW_B 0
void setLEDGunUnsafe(unsigned int newValue) {
  lc.setLed(UPPER_CAUTION_PANEL, GUN_UNSAFE_COL_A, GUN_UNSAFE_ROW_A, newValue);
  lc.setLed(UPPER_CAUTION_PANEL, GUN_UNSAFE_COL_B, GUN_UNSAFE_ROW_B, newValue);
}
void onClA4Change(unsigned int newValue) {
  setLEDGunUnsafe(newValue);
}
DcsBios::IntegerBuffer clA4Buffer(0x10d4, 0x0008, 3, onClA4Change);


#define ANTI_SKID_COL_A 1
#define ANTI_SKID_ROW_A 1
#define ANTI_SKID_COL_B 1
#define ANTI_SKID_ROW_B 2
void setLEDAntiSkid(unsigned int newValue) {
  lc.setLed(UPPER_CAUTION_PANEL, ANTI_SKID_COL_A, ANTI_SKID_ROW_A, newValue);
  lc.setLed(UPPER_CAUTION_PANEL, ANTI_SKID_COL_B, ANTI_SKID_ROW_B, newValue);
}
void onClB1Change(unsigned int newValue) {
  setLEDAntiSkid(newValue);
}
DcsBios::IntegerBuffer clB1Buffer(0x10d4, 0x0010, 4, onClB1Change);

#define L_HYD_RES_COL_A 1
#define L_HYD_RES_ROW_A 3
#define L_HYD_RES_COL_B 1
#define L_HYD_RES_ROW_B 4
void setLEDLHydRes(unsigned int newValue) {
  lc.setLed(UPPER_CAUTION_PANEL, L_HYD_RES_COL_A, L_HYD_RES_ROW_A, newValue);
  lc.setLed(UPPER_CAUTION_PANEL, L_HYD_RES_COL_B, L_HYD_RES_ROW_B, newValue);
}
void onClB2Change(unsigned int newValue) {
  setLEDLHydRes(newValue);
}
DcsBios::IntegerBuffer clB2Buffer(0x10d4, 0x0020, 5, onClB2Change);

#define R_HYD_RES_COL_A 1
#define R_HYD_RES_ROW_A 5
#define R_HYD_RES_COL_B 1
#define R_HYD_RES_ROW_B 6
void setLEDRHydRes(unsigned int newValue) {
  lc.setLed(UPPER_CAUTION_PANEL, R_HYD_RES_COL_A, R_HYD_RES_ROW_A, newValue);
  lc.setLed(UPPER_CAUTION_PANEL, R_HYD_RES_COL_B, R_HYD_RES_ROW_B, newValue);
}
void onClB3Change(unsigned int newValue) {
  setLEDRHydRes(newValue);
}
DcsBios::IntegerBuffer clB3Buffer(0x10d4, 0x0040, 6, onClB3Change);


#define OXY_LOW_COL_A 1
#define OXY_LOW_ROW_A 7
#define OXY_LOW_COL_B 1
#define OXY_LOW_ROW_B 8
void setLEDOxyLow(unsigned int newValue) {
  lc.setLed(UPPER_CAUTION_PANEL, OXY_LOW_COL_A, OXY_LOW_ROW_A, newValue);
  lc.setLed(UPPER_CAUTION_PANEL, OXY_LOW_COL_B, OXY_LOW_ROW_B, newValue);
}
void onClB4Change(unsigned int newValue) {
  setLEDOxyLow(newValue);
}
DcsBios::IntegerBuffer clB4Buffer(0x10d4, 0x0080, 7, onClB4Change);


#define ELEV_DISENG_COL_A 2
#define ELEV_DISENG_ROW_A 1
#define ELEV_DISENG_COL_B 2
#define ELEV_DISENG_ROW_B 2
void setLEDElevDiseng(unsigned int newValue) {
  lc.setLed(UPPER_CAUTION_PANEL, ELEV_DISENG_COL_A, ELEV_DISENG_ROW_A, newValue);
  lc.setLed(UPPER_CAUTION_PANEL, ELEV_DISENG_COL_B, ELEV_DISENG_ROW_B, newValue);
}
void onClC1Change(unsigned int newValue) {
  setLEDElevDiseng(newValue);
}
DcsBios::IntegerBuffer clC1Buffer(0x10d4, 0x0100, 8, onClC1Change);


#define SEAT_NOT_ARMED_COL_A 2
#define SEAT_NOT_ARMED_ROW_A 5
#define SEAT_NOT_ARMED_COL_B 2
#define SEAT_NOT_ARMED_ROW_B 6
void setLEDSeatNotArmed(unsigned int newValue) {
  lc.setLed(UPPER_CAUTION_PANEL, SEAT_NOT_ARMED_COL_A, SEAT_NOT_ARMED_ROW_A, newValue);
  lc.setLed(UPPER_CAUTION_PANEL, SEAT_NOT_ARMED_COL_B, SEAT_NOT_ARMED_ROW_B, newValue);
}
void onClC3Change(unsigned int newValue) {
  setLEDSeatNotArmed(newValue);
}
DcsBios::IntegerBuffer clC3Buffer(0x10d4, 0x0400, 10, onClC3Change);


#define BLEED_AIR_LEAK_COL_A 2
#define BLEED_AIR_LEAK_ROW_A 7
#define BLEED_AIR_LEAK_COL_B 2
#define BLEED_AIR_LEAK_ROW_B 0
void setLEDBleedAirLeak(unsigned int newValue) {
  lc.setLed(UPPER_CAUTION_PANEL, BLEED_AIR_LEAK_COL_A, BLEED_AIR_LEAK_ROW_A, newValue);
  lc.setLed(UPPER_CAUTION_PANEL, BLEED_AIR_LEAK_COL_B, BLEED_AIR_LEAK_ROW_B, newValue);
}
void onClC4Change(unsigned int newValue) {
  setLEDBleedAirLeak(newValue);
}
DcsBios::IntegerBuffer clC4Buffer(0x10d4, 0x0800, 11, onClC4Change);


#define AIL_DISENG_COL_A 3
#define AIL_DISENG_ROW_A 1
#define AIL_DISENG_COL_B 3
#define AIL_DISENG_ROW_B 2
void setLEDAilDiseng(unsigned int newValue) {
  lc.setLed(UPPER_CAUTION_PANEL, AIL_DISENG_COL_A, AIL_DISENG_ROW_A, newValue);
  lc.setLed(UPPER_CAUTION_PANEL, AIL_DISENG_COL_B, AIL_DISENG_ROW_B, newValue);
}
void onClD1Change(unsigned int newValue) {
  setLEDAilDiseng(newValue);
}
DcsBios::IntegerBuffer clD1Buffer(0x10d4, 0x1000, 12, onClD1Change);


#define L_AIL_TAB_COL_A 3
#define L_AIL_TAB_ROW_A 3
#define L_AIL_TAB_COL_B 3
#define L_AIL_TAB_ROW_B 4
void setLEDLAilTab(unsigned int newValue) {
  lc.setLed(UPPER_CAUTION_PANEL, L_AIL_TAB_COL_A, L_AIL_TAB_ROW_A, newValue);
  lc.setLed(UPPER_CAUTION_PANEL, L_AIL_TAB_COL_B, L_AIL_TAB_ROW_B, newValue);
}
void onClD2Change(unsigned int newValue) {
  setLEDLAilTab(newValue);
}
DcsBios::IntegerBuffer clD2Buffer(0x10d4, 0x2000, 13, onClD2Change);

#define R_AIL_TAB_COL_A 3
#define R_AIL_TAB_ROW_A 5
#define R_AIL_TAB_COL_B 3
#define R_AIL_TAB_ROW_B 6
void setLEDRAilTab(unsigned int newValue) {
  lc.setLed(UPPER_CAUTION_PANEL, R_AIL_TAB_COL_A, R_AIL_TAB_ROW_A, newValue);
  lc.setLed(UPPER_CAUTION_PANEL, R_AIL_TAB_COL_B, R_AIL_TAB_ROW_B, newValue);
}
void onClD3Change(unsigned int newValue) {
  setLEDRAilTab(newValue);
}
DcsBios::IntegerBuffer clD3Buffer(0x10d4, 0x4000, 14, onClD3Change);

#define SERVICE_AIR_HOT_COL_A 3
#define SERVICE_AIR_HOT_ROW_A 7
#define SERVICE_AIR_HOT_COL_B 3
#define SERVICE_AIR_HOT_ROW_B 0
void setLEDServiceAirHot(unsigned int newValue) {
  lc.setLed(UPPER_CAUTION_PANEL, SERVICE_AIR_HOT_COL_A, SERVICE_AIR_HOT_ROW_A, newValue);
  lc.setLed(UPPER_CAUTION_PANEL, SERVICE_AIR_HOT_COL_B, SERVICE_AIR_HOT_ROW_B, newValue);
}
void onClD4Change(unsigned int newValue) {
  setLEDServiceAirHot(newValue);
}
DcsBios::IntegerBuffer clD4Buffer(0x10d4, 0x8000, 15, onClD4Change);

#define PITCH_SAS_COL_A 4
#define PITCH_SAS_ROW_A 1
#define PITCH_SAS_COL_B 4
#define PITCH_SAS_ROW_B 2
void setLEDPitchSas(unsigned int newValue) {
  lc.setLed(UPPER_CAUTION_PANEL, PITCH_SAS_COL_A, PITCH_SAS_ROW_A, newValue);
  lc.setLed(UPPER_CAUTION_PANEL, PITCH_SAS_COL_B, PITCH_SAS_ROW_B, newValue);
}
void onClE1Change(unsigned int newValue) {
  setLEDPitchSas(newValue);
}
DcsBios::IntegerBuffer clE1Buffer(0x10d6, 0x0001, 0, onClE1Change);

#define L_ENG_HOT_COL_A 4
#define L_ENG_HOT_ROW_A 3
#define L_ENG_HOT_COL_B 4
#define L_ENG_HOT_ROW_B 4
void setLEDLEngHot(unsigned int newValue) {
  lc.setLed(UPPER_CAUTION_PANEL, L_ENG_HOT_COL_A, L_ENG_HOT_ROW_A, newValue);
  lc.setLed(UPPER_CAUTION_PANEL, L_ENG_HOT_COL_B, L_ENG_HOT_ROW_B, newValue);
}
void onClE2Change(unsigned int newValue) {
  setLEDLEngHot(newValue);
}
DcsBios::IntegerBuffer clE2Buffer(0x10d6, 0x0002, 1, onClE2Change);

#define R_ENG_HOT_COL_A 4
#define R_ENG_HOT_ROW_A 5
#define R_ENG_HOT_COL_B 4
#define R_ENG_HOT_ROW_B 6
void setLEDREngHot(unsigned int newValue) {
  lc.setLed(UPPER_CAUTION_PANEL, R_ENG_HOT_COL_A, R_ENG_HOT_ROW_A, newValue);
  lc.setLed(UPPER_CAUTION_PANEL, R_ENG_HOT_COL_B, R_ENG_HOT_ROW_B, newValue);
}
void onClE3Change(unsigned int newValue) {
  setLEDREngHot(newValue);
}
DcsBios::IntegerBuffer clE3Buffer(0x10d6, 0x0004, 2, onClE3Change);


#define WINDSCREEN_HOT_COL_A 4
#define WINDSCREEN_HOT_ROW_A 7
#define WINDSCREEN_HOT_COL_B 4
#define WINDSCREEN_HOT_ROW_B 0
void setLEDWindscreenHot(unsigned int newValue) {
  lc.setLed(UPPER_CAUTION_PANEL, WINDSCREEN_HOT_COL_A, WINDSCREEN_HOT_ROW_A, newValue);
  lc.setLed(UPPER_CAUTION_PANEL, WINDSCREEN_HOT_COL_B, WINDSCREEN_HOT_ROW_B, newValue);
}
void onClE4Change(unsigned int newValue) {
  setLEDWindscreenHot(newValue);
}
DcsBios::IntegerBuffer clE4Buffer(0x10d6, 0x0008, 3, onClE4Change);

#define YAW_SAS_COL_A 5
#define YAW_SAS_ROW_A 1
#define YAW_SAS_COL_B 5
#define YAW_SAS_ROW_B 2
void setLEDYawSas(unsigned int newValue) {
  lc.setLed(UPPER_CAUTION_PANEL, YAW_SAS_COL_A, YAW_SAS_ROW_A, newValue);
  lc.setLed(UPPER_CAUTION_PANEL, YAW_SAS_COL_B, YAW_SAS_ROW_B, newValue);
}
void onClF1Change(unsigned int newValue) {
  setLEDYawSas(newValue);
}
DcsBios::IntegerBuffer clF1Buffer(0x10d6, 0x0010, 4, onClF1Change);


#define L_ENG_OIL_PRESS_COL_A 5
#define L_ENG_OIL_PRESS_ROW_A 3
#define L_ENG_OIL_PRESS_COL_B 5
#define L_ENG_OIL_PRESS_ROW_B 4
void setLEDLEngOilPress(unsigned int newValue) {
  lc.setLed(UPPER_CAUTION_PANEL, L_ENG_OIL_PRESS_COL_A, L_ENG_OIL_PRESS_ROW_A, newValue);
  lc.setLed(UPPER_CAUTION_PANEL, L_ENG_OIL_PRESS_COL_B, L_ENG_OIL_PRESS_ROW_B, newValue);
}
void onClF2Change(unsigned int newValue) {
  setLEDLEngOilPress(newValue);
}
DcsBios::IntegerBuffer clF2Buffer(0x10d6, 0x0020, 5, onClF2Change);


#define R_ENG_OIL_PRESS_COL_A 5
#define R_ENG_OIL_PRESS_ROW_A 5
#define R_ENG_OIL_PRESS_COL_B 5
#define R_ENG_OIL_PRESS_ROW_B 6
void setLEDREngOilPress(unsigned int newValue) {
  lc.setLed(UPPER_CAUTION_PANEL, R_ENG_OIL_PRESS_COL_A, R_ENG_OIL_PRESS_ROW_A, newValue);
  lc.setLed(UPPER_CAUTION_PANEL, R_ENG_OIL_PRESS_COL_B, R_ENG_OIL_PRESS_ROW_B, newValue);
}
void onClF3Change(unsigned int newValue) {
  setLEDREngOilPress(newValue);
}
DcsBios::IntegerBuffer clF3Buffer(0x10d6, 0x0040, 6, onClF3Change);


#define CICU_COL_A 5
#define CICU_ROW_A 7
#define CICU_COL_B 5
#define CICU_ROW_B 0
void setLEDCicu(unsigned int newValue) {
  lc.setLed(UPPER_CAUTION_PANEL, CICU_COL_A, CICU_ROW_A, newValue);
  lc.setLed(UPPER_CAUTION_PANEL, CICU_COL_B, CICU_ROW_B, newValue);
}
void onClF4Change(unsigned int newValue) {
  setLEDCicu(newValue);
}
DcsBios::IntegerBuffer clF4Buffer(0x10d6, 0x0080, 7, onClF4Change);


#define GCAS_COL_A 6
#define GCAS_ROW_A 1
#define GCAS_COL_B 6
#define GCAS_ROW_B 2
void setLEDGcas(unsigned int newValue) {
  lc.setLed(UPPER_CAUTION_PANEL, GCAS_COL_A, GCAS_ROW_A, newValue);
  lc.setLed(UPPER_CAUTION_PANEL, GCAS_COL_B, GCAS_ROW_B, newValue);
}
void onClG1Change(unsigned int newValue) {
  setLEDGcas(newValue);
}
DcsBios::IntegerBuffer clG1Buffer(0x10d6, 0x0100, 8, onClG1Change);


#define L_MAIN_PUMP_COL_A 6
#define L_MAIN_PUMP_ROW_A 3
#define L_MAIN_PUMP_COL_B 6
#define L_MAIN_PUMP_ROW_B 4
void setLEDLMainPump(unsigned int newValue) {
  lc.setLed(UPPER_CAUTION_PANEL, L_MAIN_PUMP_COL_A, L_MAIN_PUMP_ROW_A, newValue);
  lc.setLed(UPPER_CAUTION_PANEL, L_MAIN_PUMP_COL_B, L_MAIN_PUMP_ROW_B, newValue);
}
void onClG2Change(unsigned int newValue) {
  setLEDLMainPump(newValue);
}
DcsBios::IntegerBuffer clG2Buffer(0x10d6, 0x0200, 9, onClG2Change);



#define R_MAIN_PUMP_COL_A 6
#define R_MAIN_PUMP_ROW_A 5
#define R_MAIN_PUMP_COL_B 6
#define R_MAIN_PUMP_ROW_B 6
void setLEDRMainPump(unsigned int newValue) {
  lc.setLed(UPPER_CAUTION_PANEL, R_MAIN_PUMP_COL_A, R_MAIN_PUMP_ROW_A, newValue);
  lc.setLed(UPPER_CAUTION_PANEL, R_MAIN_PUMP_COL_B, R_MAIN_PUMP_ROW_B, newValue);
}
void onClG3Change(unsigned int newValue) {
  setLEDRMainPump(newValue);
}
DcsBios::IntegerBuffer clG3Buffer(0x10d6, 0x0400, 10, onClG3Change);

#define L_AIL_COL_A 6
#define L_AIL_ROW_A 7
#define L_AIL_COL_B 6
#define L_AIL_ROW_B 0
void setLEDLAil(unsigned int newValue) {
  lc.setLed(UPPER_CAUTION_PANEL, L_AIL_COL_A, L_AIL_ROW_A, newValue);
  lc.setLed(UPPER_CAUTION_PANEL, L_AIL_COL_B, L_AIL_ROW_B, newValue);
}
void onClG4Change(unsigned int newValue) {
  setLEDLAil(newValue);
}
DcsBios::IntegerBuffer clG4Buffer(0x10d6, 0x0800, 11, onClG4Change);


#define LASTE_COL_A 7
#define LASTE_ROW_A 1
#define LASTE_COL_B 7
#define LASTE_ROW_B 2
void setLEDLaste(unsigned int newValue) {
  lc.setLed(UPPER_CAUTION_PANEL, LASTE_COL_A, LASTE_ROW_A, newValue);
  lc.setLed(UPPER_CAUTION_PANEL, LASTE_COL_B, LASTE_ROW_B, newValue);
}
void onClH1Change(unsigned int newValue) {
  setLEDLaste(newValue);
}
DcsBios::IntegerBuffer clH1Buffer(0x10d6, 0x1000, 12, onClH1Change);


#define L_WING_PUMP_COL_A 7
#define L_WING_PUMP_ROW_A 3
#define L_WING_PUMP_COL_B 7
#define L_WING_PUMP_ROW_B 4
void setLEDLWingPump(unsigned int newValue) {
  lc.setLed(UPPER_CAUTION_PANEL, L_WING_PUMP_COL_A, L_WING_PUMP_ROW_A, newValue);
  lc.setLed(UPPER_CAUTION_PANEL, L_WING_PUMP_COL_B, L_WING_PUMP_ROW_B, newValue);
}
void onClH2Change(unsigned int newValue) {
  setLEDLWingPump(newValue);
}
DcsBios::IntegerBuffer clH2Buffer(0x10d6, 0x2000, 13, onClH2Change);



#define R_WING_PUMP_COL_A 7
#define R_WING_PUMP_ROW_A 5
#define R_WING_PUMP_COL_B 7
#define R_WING_PUMP_ROW_B 6
void setLEDRWingPump(unsigned int newValue) {
  lc.setLed(UPPER_CAUTION_PANEL, R_WING_PUMP_COL_A, R_WING_PUMP_ROW_A, newValue);
  lc.setLed(UPPER_CAUTION_PANEL, R_WING_PUMP_COL_B, R_WING_PUMP_ROW_B, newValue);
}
void onClH3Change(unsigned int newValue) {
  setLEDRWingPump(newValue);
}
DcsBios::IntegerBuffer clH3Buffer(0x10d6, 0x4000, 14, onClH3Change);



#define HARS_COL_A 7
#define HARS_ROW_A 7
#define HARS_COL_B 7
#define HARS_ROW_B 0
void setLEDHars(unsigned int newValue) {
  lc.setLed(UPPER_CAUTION_PANEL, HARS_COL_A, HARS_ROW_A, newValue);
  lc.setLed(UPPER_CAUTION_PANEL, HARS_COL_B, HARS_ROW_B, newValue);
}
void onClH4Change(unsigned int newValue) {
  setLEDHars(newValue);
}
DcsBios::IntegerBuffer clH4Buffer(0x10d6, 0x8000, 15, onClH4Change);

#define IFF_MODE_4_COL_A 0
#define IFF_MODE_4_ROW_A 1
#define IFF_MODE_4_COL_B 0
#define IFF_MODE_4_ROW_B 2
void setLEDIffMode4(unsigned int newValue) {
  lc.setLed(LOWER_CAUTION_PANEL, IFF_MODE_4_COL_A, IFF_MODE_4_ROW_A, newValue);
  lc.setLed(LOWER_CAUTION_PANEL, IFF_MODE_4_COL_B, IFF_MODE_4_ROW_B, newValue);
}
void onClI1Change(unsigned int newValue) {
  setLEDIffMode4(newValue);
}
DcsBios::IntegerBuffer clI1Buffer(0x10d8, 0x0001, 0, onClI1Change);


#define L_MAIN_FUEL_LOW_COL_A 0
#define L_MAIN_FUEL_LOW_ROW_A 3
#define L_MAIN_FUEL_LOW_COL_B 0
#define L_MAIN_FUEL_LOW_ROW_B 4
void setLEDLMainFuelLow(unsigned int newValue) {
  lc.setLed(LOWER_CAUTION_PANEL, L_MAIN_FUEL_LOW_COL_A, L_MAIN_FUEL_LOW_ROW_A, newValue);
  lc.setLed(LOWER_CAUTION_PANEL, L_MAIN_FUEL_LOW_COL_B, L_MAIN_FUEL_LOW_ROW_B, newValue);
}
void onClI2Change(unsigned int newValue) {
  setLEDLMainFuelLow(newValue);
}
DcsBios::IntegerBuffer clI2Buffer(0x10d8, 0x0002, 1, onClI2Change);


#define R_MAIN_FUEL_LOW_COL_A 0
#define R_MAIN_FUEL_LOW_ROW_A 5
#define R_MAIN_FUEL_LOW_COL_B 0
#define R_MAIN_FUEL_LOW_ROW_B 6
void setLEDRMainFuelLow(unsigned int newValue) {
  lc.setLed(LOWER_CAUTION_PANEL, R_MAIN_FUEL_LOW_COL_A, R_MAIN_FUEL_LOW_ROW_A, newValue);
  lc.setLed(LOWER_CAUTION_PANEL, R_MAIN_FUEL_LOW_COL_B, R_MAIN_FUEL_LOW_ROW_B, newValue);
}
void onClI3Change(unsigned int newValue) {
  setLEDRMainFuelLow(newValue);
}
DcsBios::IntegerBuffer clI3Buffer(0x10d8, 0x0004, 2, onClI3Change);

#define L_R_TKS_UNEQUAL_COL_A 0
#define L_R_TKS_UNEQUAL_ROW_A 7
#define L_R_TKS_UNEQUAL_COL_B 0
#define L_R_TKS_UNEQUAL_ROW_B 0
void setLEDLRTksUnequal(unsigned int newValue) {
  lc.setLed(LOWER_CAUTION_PANEL, L_R_TKS_UNEQUAL_COL_A, L_R_TKS_UNEQUAL_ROW_A, newValue);
  lc.setLed(LOWER_CAUTION_PANEL, L_R_TKS_UNEQUAL_COL_B, L_R_TKS_UNEQUAL_ROW_B, newValue);
}
void onClI4Change(unsigned int newValue) {
  setLEDLRTksUnequal(newValue);
}
DcsBios::IntegerBuffer clI4Buffer(0x10d8, 0x0008, 3, onClI4Change);

#define EAC_COL_A 1
#define EAC_ROW_A 1
#define EAC_COL_B 1
#define EAC_ROW_B 2
void setLEDEac(unsigned int newValue) {
  lc.setLed(LOWER_CAUTION_PANEL, EAC_COL_A, EAC_ROW_A, newValue);
  lc.setLed(LOWER_CAUTION_PANEL, EAC_COL_B, EAC_ROW_B, newValue);
}
void onClJ1Change(unsigned int newValue) {
  setLEDEac(newValue);
}
DcsBios::IntegerBuffer clJ1Buffer(0x10d8, 0x0010, 4, onClJ1Change);


#define L_FUEL_PRESS_COL_A 1
#define L_FUEL_PRESS_ROW_A 3
#define L_FUEL_PRESS_COL_B 1
#define L_FUEL_PRESS_ROW_B 4
void setLEDLFuelPress(unsigned int newValue) {
  lc.setLed(LOWER_CAUTION_PANEL, L_FUEL_PRESS_COL_A, L_FUEL_PRESS_ROW_A, newValue);
  lc.setLed(LOWER_CAUTION_PANEL, L_FUEL_PRESS_COL_B, L_FUEL_PRESS_ROW_B, newValue);
}
void onClJ2Change(unsigned int newValue) {
  setLEDLFuelPress(newValue);
}
DcsBios::IntegerBuffer clJ2Buffer(0x10d8, 0x0020, 5, onClJ2Change);

#define R_FUEL_PRESS_COL_A 1
#define R_FUEL_PRESS_ROW_A 5
#define R_FUEL_PRESS_COL_B 1
#define R_FUEL_PRESS_ROW_B 6
void setLEDRFuelPress(unsigned int newValue) {
  lc.setLed(LOWER_CAUTION_PANEL, R_FUEL_PRESS_COL_A, R_FUEL_PRESS_ROW_A, newValue);
  lc.setLed(LOWER_CAUTION_PANEL, R_FUEL_PRESS_COL_B, R_FUEL_PRESS_ROW_B, newValue);
}
void onClJ3Change(unsigned int newValue) {
  setLEDRFuelPress(newValue);
}
DcsBios::IntegerBuffer clJ3Buffer(0x10d8, 0x0040, 6, onClJ3Change);

#define NAV_COL_A 1
#define NAV_ROW_A 7
#define NAV_COL_B 1
#define NAV_ROW_B 0
void setLEDNav(unsigned int newValue) {
  lc.setLed(LOWER_CAUTION_PANEL, NAV_COL_A, NAV_ROW_A, newValue);
  lc.setLed(LOWER_CAUTION_PANEL, NAV_COL_B, NAV_ROW_B, newValue);
}
void onClJ4Change(unsigned int newValue) {
  setLEDNav(newValue);
}DcsBios::IntegerBuffer clJ4Buffer(0x10d8, 0x0080, 7, onClJ4Change);

#define STALL_SYS_COL_A 2
#define STALL_SYS_ROW_A 1
#define STALL_SYS_COL_B 2
#define STALL_SYS_ROW_B 2
void setLEDStallSys(unsigned int newValue) {
  lc.setLed(LOWER_CAUTION_PANEL, STALL_SYS_COL_A, STALL_SYS_ROW_A, newValue);
  lc.setLed(LOWER_CAUTION_PANEL, STALL_SYS_COL_B, STALL_SYS_ROW_B, newValue);
}
void onClK1Change(unsigned int newValue) {
 setLEDStallSys(newValue); 
}DcsBios::IntegerBuffer clK1Buffer(0x10d8, 0x0100, 8, onClK1Change);

#define L_CONV_COL_A 2
#define L_CONV_ROW_A 3
#define L_CONV_COL_B 2
#define L_CONV_ROW_B 4
void setLEDLConv(unsigned int newValue) {
  lc.setLed(LOWER_CAUTION_PANEL, L_CONV_COL_A, L_CONV_ROW_A, newValue);
  lc.setLed(LOWER_CAUTION_PANEL, L_CONV_COL_B, L_CONV_ROW_B, newValue);
}
void onClK2Change(unsigned int newValue) {
  setLEDLConv(newValue);
}DcsBios::IntegerBuffer clK2Buffer(0x10d8, 0x0200, 9, onClK2Change);

#define R_CONV_COL_A 2
#define R_CONV_ROW_A 5
#define R_CONV_COL_B 2
#define R_CONV_ROW_B 6
void setLEDRConv(unsigned int newValue) {
  lc.setLed(LOWER_CAUTION_PANEL, R_CONV_COL_A, R_CONV_ROW_A, newValue);
  lc.setLed(LOWER_CAUTION_PANEL, R_CONV_COL_B, R_CONV_ROW_B, newValue);
}
void onClK3Change(unsigned int newValue) {
  setLEDRConv(newValue);
}DcsBios::IntegerBuffer clK3Buffer(0x10d8, 0x0400, 10, onClK3Change);

#define CADC_COL_A 2
#define CADC_ROW_A 7
#define CADC_COL_B 2
#define CADC_ROW_B 0
void setLEDCadc(unsigned int newValue) {
  lc.setLed(LOWER_CAUTION_PANEL, CADC_COL_A, CADC_ROW_A, newValue);
  lc.setLed(LOWER_CAUTION_PANEL, CADC_COL_B, CADC_ROW_B, newValue);
}
void onClK4Change(unsigned int newValue) {
 setLEDCadc(newValue); 
}DcsBios::IntegerBuffer clK4Buffer(0x10d8, 0x0800, 11, onClK4Change);

#define APU_GEN_COL_A 3
#define APU_GEN_ROW_A 1
#define APU_GEN_COL_B 3
#define APU_GEN_ROW_B 2
void setLEDApuGen(unsigned int newValue) {
  lc.setLed(LOWER_CAUTION_PANEL, APU_GEN_COL_A, APU_GEN_ROW_A, newValue);
  lc.setLed(LOWER_CAUTION_PANEL, APU_GEN_COL_B, APU_GEN_ROW_B, newValue);
}
void onClL1Change(unsigned int newValue) {
  setLEDApuGen(newValue);
}DcsBios::IntegerBuffer clL1Buffer(0x10d8, 0x1000, 12, onClL1Change);

#define L_GEN_COL_A 3
#define L_GEN_ROW_A 3
#define L_GEN_COL_B 3
#define L_GEN_ROW_B 4
void setLEDLGen(unsigned int newValue) {
  lc.setLed(LOWER_CAUTION_PANEL, L_GEN_COL_A, L_GEN_ROW_A, newValue);
  lc.setLed(LOWER_CAUTION_PANEL, L_GEN_COL_B, L_GEN_ROW_B, newValue);
}
void onClL2Change(unsigned int newValue) {
 setLEDLGen(newValue); 
}DcsBios::IntegerBuffer clL2Buffer(0x10d8, 0x2000, 13, onClL2Change);

#define R_GEN_COL_A 3
#define R_GEN_ROW_A 5
#define R_GEN_COL_B 3
#define R_GEN_ROW_B 6
void setLEDRGen(unsigned int newValue) {
  lc.setLed(LOWER_CAUTION_PANEL, R_GEN_COL_A, R_GEN_ROW_A, newValue);
  lc.setLed(LOWER_CAUTION_PANEL, R_GEN_COL_B, R_GEN_ROW_B, newValue);
}
void onClL3Change(unsigned int newValue) {
 setLEDRGen(newValue); 
}DcsBios::IntegerBuffer clL3Buffer(0x10d8, 0x4000, 14, onClL3Change);

#define INST_INV_COL_A 3
#define INST_INV_ROW_A 7
#define INST_INV_COL_B 3
#define INST_INV_ROW_B 0
void setLEDInstInv(unsigned int newValue) {
  lc.setLed(LOWER_CAUTION_PANEL, INST_INV_COL_A, INST_INV_ROW_A, newValue);
  lc.setLed(LOWER_CAUTION_PANEL, INST_INV_COL_B, INST_INV_ROW_B, newValue);
}
void onClL4Change(unsigned int newValue) {
  setLEDInstInv(newValue);
}DcsBios::IntegerBuffer clL4Buffer(0x10d8, 0x8000, 15, onClL4Change);

#define OXY_FLOW_ROW 1
#define OXY_FLOW_COL 4
void onOxyFlowChange(unsigned int newValue) {
  lc.setLed(LOWER_CAUTION_PANEL, OXY_FLOW_COL, OXY_FLOW_ROW, newValue);
}
DcsBios::IntegerBuffer oxyFlowBuffer(0x112a, 0x0800, 11, onOxyFlowChange);

#define TACAN_TEST_ROW 2
#define TACAN_TEST_COL 4
void onTacanTestChange(unsigned int newValue) {
  lc.setLed(LOWER_CAUTION_PANEL, TACAN_TEST_COL, TACAN_TEST_ROW, newValue);
}
DcsBios::IntegerBuffer tacanTestBuffer(0x10da, 0x0400, 10, onTacanTestChange);

void Ledcycle() {
  setLEDInstInv(1);
  //setLEDRGen(1);
  //setLEDLGen(1);
  //setLEDApuGen(1);
  setLEDCadc(1);
  //setLEDRConv(1);
  //setLEDLConv(1);
  //setLEDStallSys(1);
  setLEDNav(1);
  //setLEDRFuelPress(1);
  //setLEDLFuelPress(1);
  //setLEDEac(1);
  setLEDLRTksUnequal(1);
  //setLEDRMainFuelLow(1);
  //setLEDLMainFuelLow(1);
  //setLEDIffMode4(1);
  setLEDHars(1);
  //setLEDRWingPump(1);
  //setLEDLWingPump(1);
  //setLEDLaste(1);
  setLEDLAil(1);
  //setLEDRMainPump(1);
  //setLEDLMainPump(1);
  //setLEDGcas(1);
  setLEDCicu(1);
  //setLEDREngOilPress(1);
  //setLEDLEngOilPress(1);
  //setLEDYawSas(1);
  setLEDWindscreenHot(1);
  //setLEDREngHot(1);
  //setLEDLEngHot(1);
  //SetLEDPitchSas(1);
  setLEDServiceAirHot(1);
  //setLEDRAilTab(1);
  //setLEDLAilTab(1);
  //setLEDAilDiseng(1);
  setLEDBleedAirLeak(1);
  //setLEDSeatNotArmed(1);
  //setLEDElevDiseng(1);
  setLEDOxyLow(1);
  //setLEDRHydRes(1);
  //setLEDLHydRes(1);
  //setLEDAntiSkid(1);
  setLEDGunUnsafe(1);
  //setLEDRightHydPress(1);
  //setLEDLeftHydPress(1);
  //setLEDEngStartcycle(1);
}

// void onFlpLgRightGearLtChange(unsigned int newValue) {
//   lc.setLed(SELECT_JET_PANEL, LEFT_GEAR_COL_A, LEFT_GEAR_ROW_A, newValue);
//   lc.setLed(SELECT_JET_PANEL, LEFT_GEAR_COL_B, LEFT_GEAR_ROW_B, newValue);
// }
// DcsBios::IntegerBuffer flpLgRightGearLtBuffer(0x7430, 0x2000, 13, onFlpLgRightGearLtChange);

void onWarnCautionDimmerChange(unsigned int newValue) {

  WARN_CAUTION_DIMMER_VALUE = map(newValue, 0, 65000, 0, 15);
  updateBrightness();
}
DcsBios::IntegerBuffer warnCautionDimmerBuffer(0x754c, 0xffff, 0, onWarnCautionDimmerChange);

void onCockkpitLightModeSwChangeMax7219(unsigned int newValue) {

  // Called from onCockkpitLightModeSwChange in IFEI block

  if (newValue == NVG_MODE) {
    DAY_NIGHT_SWITCH_MODE = NVG_MODE;
  } else if (newValue == NIGHT_MODE) {
    DAY_NIGHT_SWITCH_MODE = NIGHT_MODE;
  } else {
    DAY_NIGHT_SWITCH_MODE = DAY_MODE;
  }
  updateBrightness();
}


void updateBrightness() {

  if (Reflector_In_Use == 1) {
    max7219udp.beginPacket(reflectorIP, reflectorport);
    max7219udp.println("Entering update brightness");
    max7219udp.endPacket();
  }

  if (Reflector_In_Use == 1) {
    max7219udp.beginPacket(reflectorIP, reflectorport);
    max7219udp.println("Procssing Dimmer Packet: " + String(AOA_DIMMER_VALUE));
    max7219udp.endPacket();
  }


  if (Reflector_In_Use == 1) {
    max7219udp.beginPacket(reflectorIP, reflectorport);
    max7219udp.println("Warning: " + String(WARN_CAUTION_DIMMER_VALUE));
    max7219udp.println("AOA    : " + String(AOA_DIMMER_VALUE));
    max7219udp.endPacket();
  }

  for (int address = 0; address < devices; address++) {

    if (DAY_NIGHT_SWITCH_MODE == DAY_MODE) {
      lc.setIntensity(address, FULL_BRIGHTNESS);
    } else
      lc.setIntensity(address, WARN_CAUTION_DIMMER_VALUE);
  }
}



void AllOn() {
  for (int displayunit = 0; displayunit < 9; displayunit++) {
    for (int row = 0; row < 8; row++) {
      for (int col = 0; col < 8; col++) {
        if (col != 9 && col != 9 && col != 9)
          lc.setLed(displayunit, row, col, true);
      }
    }
  }
}


void AllOff() {
  for (int displayunit = 0; displayunit < 9; displayunit++) {
    for (int row = 0; row < 8; row++) {
      for (int col = 0; col < 8; col++) {
        lc.setLed(displayunit, row, col, false);
      }
    }
  }
}


void SetBrightness(int Brightness) {
  for (int address = 0; address < devices; address++) {
    lc.setIntensity(address, (Brightness));
  }
}


// ###################################### Begin Max7219 Related #############################



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

#define COIL_RIGHT_HYD_A1 27
#define COIL_RIGHT_HYD_A2 29
#define COIL_RIGHT_HYD_A3 25
#define COIL_RIGHT_HYD_A4 23

// #define COIL_RIGHT_HYD_A1 23
// #define COIL_RIGHT_HYD_A2 27
// #define COIL_RIGHT_HYD_A3 25
// #define COIL_RIGHT_HYD_A4 29

#define COIL_LEFT_FUEL_A1 30
#define COIL_LEFT_FUEL_A2 32
#define COIL_LEFT_FUEL_A3 34
#define COIL_LEFT_FUEL_A4 36

#define COIL_RIGHT_FUEL_A1 31
#define COIL_RIGHT_FUEL_A2 33
#define COIL_RIGHT_FUEL_A3 35
#define COIL_RIGHT_FUEL_A4 37

#define COIL_OXY_REG_A1 38
#define COIL_OXY_REG_A2 40
#define COIL_OXY_REG_A3 42
#define COIL_OXY_REG_A4 44

#define COIL_LOX_A1 39
#define COIL_LOX_A2 41
#define COIL_LOX_A3 43
#define COIL_LOX_A4 45

// Pins are slighty Reversed when compared to steppers on Expansion connection
#define COIL_CABIN_PRESS_A1 6
#define COIL_CABIN_PRESS_A2 7
#define COIL_CABIN_PRESS_A3 8
#define COIL_CABIN_PRESS_A4 9


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

    // Reset Ethernet Module
    pinMode(ES1_RESET_PIN, OUTPUT);
    digitalWrite(ES1_RESET_PIN, LOW);
    delay(2);
    digitalWrite(ES1_RESET_PIN, HIGH);

    Ethernet.begin(mac, ip);
    udp.begin(localport);

    // As it takes a couple of seconds before the Ethernet Stack is operational
    // Flash leds until time period has completed
    ethernetStartTime = millis() + delayBeforeSendingPacket;
    while (millis() <= ethernetStartTime) {
      delay(FLASH_TIME);
      digitalWrite(Check_LED_G, false);
      delay(FLASH_TIME);
      digitalWrite(Check_LED_G, true);
    }

    SendDebug(BoardName + " Ethernet Started " + strMyIP + " " + sMac);
  }




  if (true) {
    // Initialise the Max7219
    devices = lc.getDeviceCount();

    for (int address = 0; address < devices; address++) {
      /*The MAX72XX is in power-saving mode on startup*/
      lc.shutdown(address, false);
      /* Set the brightness to a medium values */
      lc.setIntensity(address, 8);
      /* and clear the display */
      lc.clearDisplay(address);
    }

    SendDebug("All Leds on");
    AllOn();
    delay(1000);


    // Slowly Dim the Leds
    for (int Local_Brightness = 15; Local_Brightness >= 0; Local_Brightness--) {
      analogWrite(FORMATION_LIGHTS, map(Local_Brightness, 0, 15, 0, 255));
      analogWrite(NAVIGATION_LIGHTS, map(Local_Brightness, 0, 15, 0, 255));
      analogWrite(NVG_LIGHTS, map(Local_Brightness, 0, 15, 0, 255));
      analogWrite(FLOOD_LIGHTS, map(Local_Brightness, 0, 15, 0, 255));
      analogWrite(BACK_LIGHTS, map(Local_Brightness, 0, 15, 0, 255));
      analogWrite(STROBE_LIGHTS, map(Local_Brightness, 0, 15, 0, 255));
      SetBrightness(Local_Brightness);

      delay(100);
    }

    // Turn off All Leds and set to mid brightness
    AllOff();
    SetBrightness(8);
    SendDebug("Led test complete");
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
    STEPPER_RIGHT_HYD.setMaxSpeed(STEPPER_MAX_SPEED);
    STEPPER_RIGHT_HYD.setAcceleration(STEPPER_ACCELERATION);
    STEPPER_RIGHT_HYD.move(630);
    SendDebug("Start Stepper Right Hyd");
    while (STEPPER_RIGHT_HYD.distanceToGo() != 0) {
      STEPPER_RIGHT_HYD.run();
    }
    STEPPER_RIGHT_HYD.move(-630);
    while (STEPPER_RIGHT_HYD.distanceToGo() != 0) {
      STEPPER_RIGHT_HYD.run();
    }
    STEPPER_RIGHT_HYD.move(630);
    SendDebug("Start Stepper Right Hyd");
    while (STEPPER_RIGHT_HYD.distanceToGo() != 0) {
      STEPPER_RIGHT_HYD.run();
    }
    STEPPER_RIGHT_HYD.move(-630);
    while (STEPPER_RIGHT_HYD.distanceToGo() != 0) {
      STEPPER_RIGHT_HYD.run();
    }
    SendDebug("End Stepper Right Hyd");
  }

  if (false) {
    SendDebug("Start Stepper Left Hyd");
    STEPPER_LEFT_HYD.setMaxSpeed(STEPPER_MAX_SPEED);
    STEPPER_LEFT_HYD.setAcceleration(STEPPER_ACCELERATION);

    STEPPER_LEFT_HYD.move(630);

    while (STEPPER_LEFT_HYD.distanceToGo() != 0) {
      STEPPER_LEFT_HYD.run();
    }
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

  if (false) {
    SendDebug("Start Stepper Left Fuel");
    STEPPER_LEFT_FUEL.setMaxSpeed(STEPPER_MAX_SPEED);
    STEPPER_LEFT_FUEL.setAcceleration(STEPPER_ACCELERATION);
    STEPPER_LEFT_FUEL.move(640);
    while (STEPPER_LEFT_FUEL.distanceToGo() != 0) {
      STEPPER_LEFT_FUEL.run();
    }
    STEPPER_LEFT_FUEL.move(-640);
    while (STEPPER_LEFT_FUEL.distanceToGo() != 0) {
      STEPPER_LEFT_FUEL.run();
    }
    STEPPER_LEFT_FUEL.move(640);
    while (STEPPER_LEFT_FUEL.distanceToGo() != 0) {
      STEPPER_LEFT_FUEL.run();
    }
    STEPPER_LEFT_FUEL.move(-640);
    while (STEPPER_LEFT_FUEL.distanceToGo() != 0) {
      STEPPER_LEFT_FUEL.run();
    }
    SendDebug("End Stepper Left Fuel");
  }

  if (false) {
    SendDebug("Start Stepper Right Fuel");
    STEPPER_RIGHT_FUEL.setMaxSpeed(STEPPER_MAX_SPEED);
    STEPPER_RIGHT_FUEL.setAcceleration(STEPPER_ACCELERATION);
    STEPPER_RIGHT_FUEL.move(640);
    while (STEPPER_RIGHT_FUEL.distanceToGo() != 0) {
      STEPPER_RIGHT_FUEL.run();
    }
    STEPPER_RIGHT_FUEL.move(-640);
    while (STEPPER_RIGHT_FUEL.distanceToGo() != 0) {
      STEPPER_RIGHT_FUEL.run();
    }
    STEPPER_RIGHT_FUEL.move(640);
    while (STEPPER_RIGHT_FUEL.distanceToGo() != 0) {
      STEPPER_RIGHT_FUEL.run();
    }
    STEPPER_RIGHT_FUEL.move(-640);
    while (STEPPER_RIGHT_FUEL.distanceToGo() != 0) {
      STEPPER_RIGHT_FUEL.run();
    }
    SendDebug("End Stepper Right Fuel");
  }

  if (false) {
    SendDebug("Start Stepper OXY REG");
    STEPPER_OXY_REG.setMaxSpeed(STEPPER_MAX_SPEED);
    STEPPER_OXY_REG.setAcceleration(STEPPER_ACCELERATION);
    STEPPER_OXY_REG.move(640);
    while (STEPPER_OXY_REG.distanceToGo() != 0) {
      STEPPER_OXY_REG.run();
    }
    STEPPER_OXY_REG.move(-640);
    while (STEPPER_OXY_REG.distanceToGo() != 0) {
      STEPPER_OXY_REG.run();
    }
    STEPPER_OXY_REG.move(640);
    while (STEPPER_OXY_REG.distanceToGo() != 0) {
      STEPPER_OXY_REG.run();
    }
    STEPPER_OXY_REG.move(-640);
    while (STEPPER_OXY_REG.distanceToGo() != 0) {
      STEPPER_OXY_REG.run();
    }
    SendDebug("End Stepper OXY REG");
  }

  if (false) {
    SendDebug("Start Stepper LOX");
    STEPPER_LOX.setMaxSpeed(STEPPER_MAX_SPEED);
    STEPPER_LOX.setAcceleration(STEPPER_ACCELERATION);
    STEPPER_LOX.move(640);
    while (STEPPER_LOX.distanceToGo() != 0) {
      STEPPER_LOX.run();
    }
    STEPPER_LOX.move(-640);
    while (STEPPER_LOX.distanceToGo() != 0) {
      STEPPER_LOX.run();
    }
    STEPPER_LOX.move(640);
    while (STEPPER_LOX.distanceToGo() != 0) {
      STEPPER_LOX.run();
    }
    STEPPER_LOX.move(-640);
    while (STEPPER_LOX.distanceToGo() != 0) {
      STEPPER_LOX.run();
    }
    SendDebug("End Stepper LOX");
  }


  if (false) {
    SendDebug("Start Stepper Cabin Press");
    STEPPER_CABIN_PRESS.setMaxSpeed(STEPPER_MAX_SPEED);
    STEPPER_CABIN_PRESS.setAcceleration(STEPPER_ACCELERATION);
    STEPPER_CABIN_PRESS.move(640);
    while (STEPPER_CABIN_PRESS.distanceToGo() != 0) {
      STEPPER_CABIN_PRESS.run();
    }
    STEPPER_CABIN_PRESS.move(-640);
    while (STEPPER_CABIN_PRESS.distanceToGo() != 0) {
      STEPPER_CABIN_PRESS.run();
    }
    STEPPER_CABIN_PRESS.move(640);
    while (STEPPER_CABIN_PRESS.distanceToGo() != 0) {
      STEPPER_CABIN_PRESS.run();
    }
    STEPPER_CABIN_PRESS.move(-640);
    while (STEPPER_CABIN_PRESS.distanceToGo() != 0) {
      STEPPER_CABIN_PRESS.run();
    }
    SendDebug("End Stepper Cabin Press");
  }


  SendDebug("End Motor Initialisation");

  // Ledcycle();
  if (DCSBIOS_In_Use == 1) DcsBios::setup();
  SendDebug("End Setup");
}

/*


STEPPER_OXY_REG


*/

void setLeftHyd(long TargetLeftHyd) {
  // SendDebug("Target Right Hyd = " + String(TargetLeftHyd));
  STEPPER_LEFT_HYD.moveTo(TargetLeftHyd);
}
void onLHydPressChange(unsigned int newValue) {
  long LeftHyd = newValue;
  SendDebug("onLHydPressChange = " + String(LeftHyd));
  setLeftHyd(map(LeftHyd, 0, 65535, 0, STEPS));
}
DcsBios::IntegerBuffer lHydPressBuffer(0x10c2, 0xffff, 0, onLHydPressChange);


void setRightHyd(long TargetRightHyd) {
  //SendDebug("Target Right Hyd = " + String(TargetRightHyd));
  STEPPER_RIGHT_HYD.moveTo(TargetRightHyd);
}

void onRHydPressChange(unsigned int newValue) {
  long RightHyd = newValue;
  SendDebug("onRHydPressChange = " + String(RightHyd));
  setRightHyd(map(RightHyd, 0, 65535, 0, STEPS));
}
DcsBios::IntegerBuffer rHydPressBuffer(0x10c4, 0xffff, 0, onRHydPressChange);


void setLeftFuel(long TargetLeftFuel) {
  //SendDebug("Target Left Fuel = " + String(TargetLeftFuel));
  STEPPER_LEFT_FUEL.moveTo(TargetLeftFuel);
}
void onFuelQtyLChange(unsigned int newValue) {
  long LeftFuel = newValue;
  SendDebug("onFuelQtyLChange = " + String(LeftFuel));
  // Needle not fully rotating so reduce number of steps
  setLeftFuel(map(LeftFuel, 0, 65535, 0, STEPS / 2.4));
}
DcsBios::IntegerBuffer fuelQtyLBuffer(0x10ca, 0xffff, 0, onFuelQtyLChange);

void setRightFuel(long TargetRightFuel) {
  //SendDebug("Target Right Fuel = " + String(TargetRightFuel));
  STEPPER_RIGHT_FUEL.moveTo(TargetRightFuel);
}
void onFuelQtyRChange(unsigned int newValue) {
  long RightFuel = newValue;
  SendDebug("onFuelQtyRChange = " + String(RightFuel));
  // Needle not fully rotating so reduce number of steps
  setRightFuel(map(RightFuel, 0, 65535, 0, STEPS / 2.4));
}
DcsBios::IntegerBuffer fuelQtyRBuffer(0x10cc, 0xffff, 0, onFuelQtyRChange);



void setOxPressure(long TargetOxPressure) {
  //SendDebug("Target OxPressure = " + String(TargetOxPressure));
  STEPPER_OXY_REG.moveTo(TargetOxPressure);
}
void onOxyPressChange(unsigned int newValue) {
  long OxPressure = newValue;
  SendDebug("onOxyPressChange = " + String(OxPressure));
  setOxPressure(map(OxPressure, 0, 65535, 0, STEPS / 2));
}
DcsBios::IntegerBuffer oxyPressBuffer(0x1130, 0xffff, 0, onOxyPressChange);


void setOxQty(long TargetOxQty) {
  //SendDebug("Target TargetOxQty = " + String(TargetOxQty));
  STEPPER_LOX.moveTo(TargetOxQty);
}
void onOxyVolumeChange(unsigned int newValue) {
  long OxQty = newValue;
  SendDebug("onOxyVolumeChange = " + String(OxQty));
  setOxQty(map(OxQty, 0, 65535, 0, STEPS / 2));
}
DcsBios::IntegerBuffer oxyVolumeBuffer(0x1132, 0xffff, 0, onOxyVolumeChange);

void setCabinPressure(long TargetCabinPressure) {
  //SendDebug("Target CabinPressure = " + String(TargetRightFuel));
  STEPPER_CABIN_PRESS.moveTo(TargetCabinPressure);
}
void onCabinPressAltChange(unsigned int newValue) {
  long CabinPressure = newValue;
  SendDebug("onCabinPressAltChange = " + String(CabinPressure));
  setCabinPressure(map(CabinPressure, 0, 65535, 0, STEPS));
}
DcsBios::IntegerBuffer cabinPressAltBuffer(0x1134, 0xffff, 0, onCabinPressAltChange);




void updateSteppers() {

  STEPPER_LEFT_HYD.run();
  STEPPER_RIGHT_HYD.run();
  STEPPER_LEFT_FUEL.run();
  STEPPER_RIGHT_FUEL.run();
  STEPPER_OXY_REG.run();
  STEPPER_LOX.run();
  STEPPER_CABIN_PRESS.run();
}



void loop() {

  if (millis() >= NEXT_STATUS_TOGGLE_TIMER) {
    GREEN_LED_STATE = !GREEN_LED_STATE;
    RED_LED_STATE = !GREEN_LED_STATE;
    digitalWrite(GREEN_STATUS_LED_PORT, GREEN_LED_STATE);
    digitalWrite(RED_STATUS_LED_PORT, RED_LED_STATE);
    NEXT_STATUS_TOGGLE_TIMER = millis() + FLASH_TIME;
  }


  if (DCSBIOS_In_Use == 1) DcsBios::loop();
  updateSteppers();
}
