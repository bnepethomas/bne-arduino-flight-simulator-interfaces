// UPDATED TO DSCS-BIOS FP EDITION FOR OPEN HORNET


////////////////////---||||||||||********||||||||||---\\\\\\\\\\\\\\\\\\\\
//||               FUNCTION = HORNET LED OUTPUT MAX 7219              ||\\
//||              LOCATION IN THE PIT = LIP RIGHTHAND SIDE             ||\\
//||            ARDUINO PROCESSOR TYPE = Arduino Mega 2560            ||\\
//||      ARDUINO CHIP SERIAL NUMBER =      ||\\
//||                    CONNECTED COM PORT = COM 13                    ||\\
//||            ****DO CHECK S/N BEFORE UPLOAD NEW DATA****           ||\\
////////////////////---||||||||||********||||||||||---\\\\\\\\\\\\\\\\\\\\

// Tell DCS-BIOS to use a serial connection and use interrupt-driven
// communication. The main program will be interrupted to prioritize
// processing incoming data.
//
// This should work on any Arduino that has an ATMega328 controller
// (Uno, Pro Mini, many others).


// TODOS

// Day-Nite-NVG
// Position Day - all caution/warning lights 100%


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
byte mac[] = {0xA8, 0x61, 0x0A, 0x9E, 0x83, 0x06};
IPAddress ip(172, 16, 1, 106);
String strMyIP = "172.16.1.106";

// Reflector
IPAddress reflectorIP(172, 16, 1, 10);
String strReflectorIP = "X.X.X.X";

const unsigned int max7219port = 7788;
const unsigned int remoteport = 49000;
const unsigned int reflectorport = 27000;

const unsigned int MSFSport = 13136;

// Packet Length
int max7219packetsize;
int max7219Len;

int MSFSpacketsize;
int MSFSLen;

EthernetUDP max7219udp;              // Max7219
EthernetUDP MSFSudp;                 // Listens to MSFS light commands

char max7219packetBuffer[1000];      //buffer to store packet data for both Max7219 and MSFS data
char outpacketBuffer[1000];           //buffer to store the outgoing data

bool Debug_Display = false;
char *ParameterNamePtr;
char *ParameterValuePtr;
// ###################################### End Ethernet Related #############################



#define DCSBIOS_IRQ_SERIAL
//#define DCSBIOS_DEFAULT_SERIAL
#include "LedControl.h"
#include "DcsBios.h"


// Master
// CHIP U1 (C)
#define SELECT_JET_PANEL 2
#define AOA 2 // NOT DONE
#define MASTER_ARM 2

// CHIP U2 (A)
#define LEFT_EWI 0
#define UFC_PANEL 0
#define BIT_LED 0
#define LEFT_DIS 0 // CHECK IF NEEDED 
#define Bit_led 0 // Bit Test Ledt HUD BOX   

// CHIP U3 (D)
#define CAUTION_PANEL 3
#define LOCK_SHOOT 3
#define RIGHT_DIS 3 //CHECK IF NEEDED

// CHIP U5 (B)
#define RIGHT_EWI 1

// CHIP U4 (E)
#define AOA_DIM 4

// CHIP U6 (F)
#define RWR_DIM 5

// ######################## LEFT EWI ########################
// NO GO  - LEFT EWI - ORANGE
#define NO_GO_A_ROW 0
#define NO_GO_A_COL 0
#define NO_GO_B_ROW 1
#define NO_GO_B_COL 0

// GO  - LEFT EWI - GREEN
#define GO_A_ROW 2
#define GO_A_COL 0
#define GO_B_ROW 3
#define GO_B_COL 0

// RIGHT BLEED - LEFT EWI - RED
#define R_BLEED_A_ROW 0
#define R_BLEED_A_COL 1
#define R_BLEED_B_ROW 1
#define R_BLEED_B_COL 1

// LEFT BLEED - LEFT EWI - RED
#define L_BLEED_A_ROW 2
#define L_BLEED_A_COL 1
#define L_BLEED_B_ROW 3
#define L_BLEED_B_COL 1

// STBY - LEFT EWI - ORANGE
#define STBY_A_ROW 0
#define STBY_A_COL 2
#define STBY_B_ROW 1
#define STBY_B_COL 2

// SPD BRK- LEFT EWI - GREEN
#define SPD_BRK_A_ROW 2
#define SPD_BRK_A_COL 2
#define SPD_BRK_B_ROW 3
#define SPD_BRK_B_COL 2

// REC - LEFT EWI - ORANGE
#define REC_A_ROW 0
#define REC_A_COL 3
#define REC_B_ROW 1
#define REC_B_COL 3

// LAUNCH BAR RED - LEFT EWI - RED
#define L_BAR_RED_A_ROW 2
#define L_BAR_RED_A_COL 3
#define L_BAR_RED_B_ROW 3
#define L_BAR_RED_B_COL 3

// XMIT - LEFT EWI - GREEN
#define XMIT_A_ROW 0
#define XMIT_A_COL 4
#define XMIT_B_ROW 1
#define XMIT_B_COL 4

// LAUNCH BAR GREEN - LEFT EWI - GREEN
#define L_BAR_GREEN_A_ROW 2
#define L_BAR_GREEN_A_COL 4
#define L_BAR_GREEN_B_ROW 3
#define L_BAR_GREEN_B_COL 4

// ASPJ OH - LEFT EWI - ORANGE
#define ASPJ_OH_A_ROW 0
#define ASPJ_OH_A_COL 5
#define ASPJ_OH_B_ROW 1
#define ASPJ_OH_B_COL 5

// MASTER CAUTION - LEFT EWI - ORANGE
#define MSTR_CAUT_A_ROW 4
#define MSTR_CAUT_A_COL 1
#define MSTR_CAUT_B_ROW 4
#define MSTR_CAUT_B_COL 2
#define MSTR_CAUT_C_ROW 5
#define MSTR_CAUT_C_COL 1
#define MSTR_CAUT_D_ROW 5
#define MSTR_CAUT_D_COL 2

// LEFT FIRE - LEFT EWI - RED
#define LEFT_FIRE_A_ROW 6
#define LEFT_FIRE_A_COL 1
#define LEFT_FIRE_B_ROW 6
#define LEFT_FIRE_B_COL 2
#define LEFT_FIRE_C_ROW 7
#define LEFT_FIRE_C_COL 1
#define LEFT_FIRE_D_ROW 7
#define LEFT_FIRE_D_COL 2

// ######################## RIGHT EWI ########################
// RCDR ON  - RIGHT EWI - GREEN
#define RCDR_ON_A_ROW 0
#define RCDR_ON_A_COL 0
#define RCDR_ON_B_ROW 1
#define RCDR_ON_B_COL 0

// DISP  - RIGHT EWI - GREEN
#define DISP_A_ROW 2
#define DISP_A_COL 0
#define DISP_B_ROW 3
#define DISP_B_COL 0

// SPARE 1 - RIGHT EWI - GREEN
#define SPARE_1_A_ROW 0
#define SPARE_1_A_COL 1
#define SPARE_1_B_ROW 1
#define SPARE_1_B_COL 1

// SPARE 2 - RIGHT EWI - GREEN
#define SPARE_2_A_ROW 2
#define SPARE_2_A_COL 1
#define SPARE_2_B_ROW 3
#define SPARE_2_B_COL 1

// SPARE 3 - RIGHT EWI - GREEN
#define SPARE_3_A_ROW 0
#define SPARE_3_A_COL 2
#define SPARE_3_B_ROW 1
#define SPARE_3_B_COL 2

// SPARE 4 - RIGHT EWI - GREEN
#define SPARE_4_A_ROW 2
#define SPARE_4_A_COL 2
#define SPARE_4_B_ROW 3
#define SPARE_4_B_COL 2

// SPARE 5 - RIGHT EWI - GREEN
#define SPARE_5_A_ROW 0
#define SPARE_5_A_COL 3
#define SPARE_5_B_ROW 1
#define SPARE_5_B_COL 3

// SAM - RIGHT EWI - RED
#define SAM_A_ROW 2
#define SAM_A_COL 3
#define SAM_B_ROW 3
#define SAM_B_COL 3

// AI - RIGHT EWI - RED
#define AI_A_ROW 0
#define AI_A_COL 4
#define AI_B_ROW 1
#define AI_B_COL 4

// AAA - RIGHT EWI - RED
#define AAA_A_ROW 2
#define AAA_A_COL 4
#define AAA_B_ROW 3
#define AAA_B_COL 4

// CW - RIGHT EWI - RED
#define CW_A_ROW 0
#define CW_A_COL 5
#define CW_B_ROW 1
#define CW_B_COL 5

// ######################## FIRE ########################
// APU FIRE - RIGHT EWI - RED
#define APU_FIRE_A_ROW 4
#define APU_FIRE_A_COL 1
#define APU_FIRE_B_ROW 4
#define APU_FIRE_B_COL 2
#define APU_FIRE_C_ROW 5
#define APU_FIRE_C_COL 1
#define APU_FIRE_D_ROW 5
#define APU_FIRE_D_COL 2

// RIGHT FIRE - RIGHT EWI - RED
#define RIGHT_FIRE_A_ROW 6
#define RIGHT_FIRE_A_COL 1
#define RIGHT_FIRE_B_ROW 6
#define RIGHT_FIRE_B_COL 2
#define RIGHT_FIRE_C_ROW 7
#define RIGHT_FIRE_C_COL 1
#define RIGHT_FIRE_D_ROW 7
#define RIGHT_FIRE_D_COL 2

// ######################## CAUTION PANEL ########################
#define CK_SEAT_COL_A 0
#define CK_SEAT_ROW_A 0
#define CK_SEAT_COL_B 0
#define CK_SEAT_ROW_B 1

#define FCS_HOT_COL_A 1
#define FCS_HOT_ROW_A 0
#define FCS_HOT_COL_B 1
#define FCS_HOT_ROW_B 1

#define FUEL_LO_COL_A 2
#define FUEL_LO_ROW_A 0
#define FUEL_LO_COL_B 2
#define FUEL_LO_ROW_B 1

#define L_GEN_COL_A 3
#define L_GEN_ROW_A 0
#define L_GEN_COL_B 3
#define L_GEN_ROW_B 1

#define APU_ACC_COL_A 0
#define APU_ACC_ROW_A 2
#define APU_ACC_COL_B 0
#define APU_ACC_ROW_B 3

#define GEN_TIE_COL_A 1
#define GEN_TIE_ROW_A 2
#define GEN_TIE_COL_B 1
#define GEN_TIE_ROW_B 3

#define FCES_COL_A 2
#define FCES_ROW_A 2
#define FCES_COL_B 2
#define FCES_ROW_B 3

#define R_GEN_COL_A 3
#define R_GEN_ROW_A 2
#define R_GEN_COL_B 3
#define R_GEN_ROW_B 3

#define BATT_SW_COL_A 0
#define BATT_SW_ROW_A 4
#define BATT_SW_COL_B 0
#define BATT_SW_ROW_B 5

#define C_SPARE_1_COL_A 1
#define C_SPARE_1_ROW_A 4
#define C_SPARE_1_COL_B 1
#define C_SPARE_1_ROW_B 5

#define C_SPARE_2_COL_A 2
#define C_SPARE_2_ROW_A 4
#define C_SPARE_2_COL_B 2
#define C_SPARE_2_ROW_B 5

#define C_SPARE_3_COL_A 3
#define C_SPARE_3_ROW_A 4
#define C_SPARE_3_COL_B 3
#define C_SPARE_3_ROW_B 5

// ######################## UFC ########################
#define UFC_OPT1_COL_A 4
#define UFC_OPT1_ROW_A 4
#define UFC_OPT1_COL_B 3
#define UFC_OPT1_ROW_B 4

#define UFC_OPT2_COL_A 4
#define UFC_OPT2_ROW_A 5
#define UFC_OPT2_COL_B 3
#define UFC_OPT2_ROW_B 5

#define UFC_OPT3_COL_A 4
#define UFC_OPT3_ROW_A 6
#define UFC_OPT3_COL_B 3
#define UFC_OPT3_ROW_B 6

#define UFC_OPT4_COL_A 3
#define UFC_OPT4_ROW_A 7
#define UFC_OPT4_COL_B 4
#define UFC_OPT4_ROW_B 7

#define UFC_OPT5_COL_A 5
#define UFC_OPT5_ROW_A 4

// ######################## JETT STATION SELECT ########################
#define SEL_CENTER_COL_A 0
#define SEL_CENTER_ROW_A 0

#define SEL_LEFT_INNER_COL_A 0
#define SEL_LEFT_INNER_ROW_A 1

#define SEL_LEFT_OUTER_COL_A 1
#define SEL_LEFT_OUTER_ROW_A 1

#define SEL_RIGHT_INNER_COL_A 1
#define SEL_RIGHT_INNER_ROW_A 0

#define SEL_RIGHT_OUTER_COL_A 2
#define SEL_RIGHT_OUTER_ROW_A 0

// ######################## GEAR FLAPS ########################
#define NOSE_GEAR_COL_A 3
#define NOSE_GEAR_ROW_A 0
#define NOSE_GEAR_COL_B 3
#define NOSE_GEAR_ROW_B 1

#define LEFT_GEAR_COL_A 2
#define LEFT_GEAR_ROW_A 1
#define LEFT_GEAR_COL_B 1
#define LEFT_GEAR_ROW_B 2

#define RIGHT_GEAR_COL_A 4
#define RIGHT_GEAR_ROW_A 0
#define RIGHT_GEAR_COL_B 4
#define RIGHT_GEAR_ROW_B 1

#define HALF_FLAPS_COL_A 0
#define HALF_FLAPS_ROW_A 2
#define HALF_FLAPS_COL_B 2
#define HALF_FLAPS_ROW_B 2

#define FULL_FLAPS_COL_A 5
#define FULL_FLAPS_ROW_A 0
#define FULL_FLAPS_COL_B 5
#define FULL_FLAPS_ROW_B 1

#define AMBER_FLAPS_COL_A 3
#define AMBER_FLAPS_ROW_A 2
#define AMBER_FLAPS_COL_B 4
#define AMBER_FLAPS_ROW_B 2

// ######################## RWR - ALR_67 ########################
#define RWR_BIT_COL_A 0
#define RWR_BIT_ROW_A 0

#define RWR_ENABLE_OFFSET_COL_A 0
#define RWR_ENABLE_OFFSET_ROW_A 1

#define RWR_ENABLE_SPECIAL_COL_A 0
#define RWR_ENABLE_SPECIAL_ROW_A 2

#define RWR_LIMIT_COL_A 0
#define RWR_LIMIT_ROW_A 3

#define RWR_ON_COL_A 0
#define RWR_ON_ROW_A 4

#define RWR_FAIL_RED_COL_A 1
#define RWR_FAIL_RED_ROW_A 0

#define RWR_OFFSET_COL_A 1
#define RWR_OFFSET_ROW_A 1

#define RWR_SPECIAL_COL_A 1
#define RWR_SPECIAL_ROW_A 2

#define RWR_DISPLAY_COL_A 1
#define RWR_DISPLAY_ROW_A 3

#define RWR_ALR_67_COL_A 1
#define RWR_ALR_67_ROW_A 4

// ######################## LEFT DIS ########################
#define GEAR_KNOB_COL_A 5
#define GEAR_KNOB_ROW_A 3

#define APU_LT_COL_A 5
#define APU_LT_ROW_A 2

// ######################## RIGHT  DIS ########################
#define HOOK_COL_A 5
#define HOOK_ROW_A 1
#define HOOK_COL_B 5 //6
#define HOOK_ROW_B 0 //1

#define RAD_ALT_R_COL_A 6 //5
#define RAD_ALT_R_ROW_A 1 //0
#define RAD_ALT_G_COL_A 6
#define RAD_ALT_G_ROW_A 0

// ######################## BIT LIGHT HUD ########################
// *** not found in DSC FP as yet ***
#define BIT_GREEN_COL_A 5
#define BIT_GREEN_ROW_A 5

#define BIT_RED_COL_A 6
#define BIT_RED_ROW_A 5


// ####################### LOCK SHOOT ################################

#define LOCK_COL_A 5
#define LOCK_ROW_A 2
#define LOCK_COL_B 5
#define LOCK_ROW_B 4

#define AOA_DIM 4

#define SHOOT_COL_A 6
#define SHOOT_ROW_A 3
#define SHOOT_COL_B 6
#define SHOOT_ROW_B 4

#define SHOOT_FLASH_COL_A 5
#define SHOOT_FLASH_ROW_A 3
#define SHOOT_FLASH_COL_B 6
#define SHOOT_FLASH_ROW_B 2


// ########################## AOA ##################
#define AOA_ABOVE_COL 0
#define AOA_ABOVE_ROW 1

#define AOA_ON_COL 1
#define AOA_ON_ROW 1

#define AOA_BELOW_COL 1
#define AOA_BELOW_ROW 0

// #################### EXTERIOR LIGHTS #################

// Pinouts for Version 2 PCBs. Selected pins did not support dimming
//#define STROBE_LIGHTS 30
//#define NAVIGATION_LIGHTS 34
//#define FORMATION_LIGHTS 31
//#define BACK_LIGHTS 33
//#define FLOOD_LIGHTS 32

// Pinouts for Version 4 PCB
#define MAP_LIGHTS 6
#define NVG_LIGHTS 6
#define FLOOD_LIGHTS 7
#define FORMATION_LIGHTS 8
#define STROBE_LIGHTS 44
#define NAVIGATION_LIGHTS 45
#define BACK_LIGHTS 46

// ######################## SETUP ########################
#define STATUS_LED_PORT 6
int devices = 6;

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

#define RELAY_PORT_1 22
#define RELAY_PORT_2 23
#define RELAY_PORT_3 24
#define RELAY_PORT_4 25

#define BACKLIGHTING_RELAY_PORT RELAY_PORT_1
#define RIGHT_SCREEN_RELAY_PORT RELAY_PORT_2
#define LEFT_AND_AMPCD_SCREENS_RELAY_PORT RELAY_PORT_3

bool LEFT_GEN_SWITCH_STATE = true;
bool RIGHT_GEN_SWITCH_STATE = true;
bool BATTERY_SWITCH_STATE = true;




LedControl lc = LedControl(16, 14, 15, devices);

/* paste code snippets from the reference documentation here */
//DcsBios::Switch2Pos lightsTestSw("LIGHTS_TEST_SW", 22);
//DcsBios::LED sjCtrLt(0x742e, 0x4000, 13);


void onFlpLgRightGearLtChange(unsigned int newValue) {
  lc.setLed(SELECT_JET_PANEL, RIGHT_GEAR_COL_A, RIGHT_GEAR_ROW_A, newValue);
  lc.setLed(SELECT_JET_PANEL, RIGHT_GEAR_COL_B, RIGHT_GEAR_ROW_B, newValue);
}
DcsBios::IntegerBuffer flpLgRightGearLtBuffer(0x7430, 0x2000, 13, onFlpLgRightGearLtChange);


void onFlpLgNoseGearLtChange(unsigned int newValue) {
  lc.setLed(SELECT_JET_PANEL, NOSE_GEAR_COL_A, NOSE_GEAR_ROW_A, newValue);
  lc.setLed(SELECT_JET_PANEL, NOSE_GEAR_COL_B, NOSE_GEAR_ROW_B, newValue);
}
DcsBios::IntegerBuffer flpLgNoseGearLtBuffer(0x7430, 0x0800, 11, onFlpLgNoseGearLtChange);


void onFlpLgLeftGearLtChange(unsigned int newValue) {
  lc.setLed(SELECT_JET_PANEL, LEFT_GEAR_COL_A, LEFT_GEAR_ROW_A, newValue);
  lc.setLed(SELECT_JET_PANEL, LEFT_GEAR_COL_B, LEFT_GEAR_ROW_B, newValue);
}
DcsBios::IntegerBuffer flpLgLeftGearLtBuffer(0x7430, 0x1000, 12, onFlpLgLeftGearLtChange);


void onFlpLgHalfFlapsLtChange(unsigned int newValue) {
  lc.setLed(SELECT_JET_PANEL, HALF_FLAPS_COL_A, HALF_FLAPS_ROW_A, newValue);
  lc.setLed(SELECT_JET_PANEL, HALF_FLAPS_COL_B, HALF_FLAPS_ROW_B, newValue);
}
DcsBios::IntegerBuffer flpLgHalfFlapsLtBuffer(0x7430, 0x4000, 14, onFlpLgHalfFlapsLtChange);


void onFlpLgFullFlapsLtChange(unsigned int newValue) {
  lc.setLed(SELECT_JET_PANEL, FULL_FLAPS_COL_A, FULL_FLAPS_ROW_A, newValue);
  lc.setLed(SELECT_JET_PANEL, FULL_FLAPS_COL_B, FULL_FLAPS_ROW_B, newValue);
}
DcsBios::IntegerBuffer flpLgFullFlapsLtBuffer(0x7430, 0x8000, 15, onFlpLgFullFlapsLtChange);


void onFlpLgFlapsLtChange(unsigned int newValue) {
  lc.setLed(SELECT_JET_PANEL, AMBER_FLAPS_COL_A, AMBER_FLAPS_ROW_A, newValue);
  lc.setLed(SELECT_JET_PANEL, AMBER_FLAPS_COL_B, AMBER_FLAPS_ROW_B, newValue);
}
DcsBios::IntegerBuffer flpLgFlapsLtBuffer(0x7466, 0x0001, 0, onFlpLgFlapsLtChange);


void onSjRoLtChange(unsigned int newValue) {
  lc.setLed(SELECT_JET_PANEL, SEL_RIGHT_OUTER_COL_A, SEL_RIGHT_OUTER_ROW_A, newValue);
}
DcsBios::IntegerBuffer sjRoLtBuffer(0x7430, 0x0400, 10, onSjRoLtChange);


void onSjRiLtChange(unsigned int newValue) {
  lc.setLed(SELECT_JET_PANEL, SEL_RIGHT_INNER_COL_A, SEL_RIGHT_INNER_ROW_A, newValue);
}
DcsBios::IntegerBuffer sjRiLtBuffer(0x7430, 0x0200, 9, onSjRiLtChange);


void onSjLoLtChange(unsigned int newValue) {
  lc.setLed(SELECT_JET_PANEL, SEL_LEFT_OUTER_COL_A, SEL_LEFT_OUTER_ROW_A, newValue);
}
DcsBios::IntegerBuffer sjLoLtBuffer(0x7430, 0x0100, 8, onSjLoLtChange);


void onSjLiLtChange(unsigned int newValue) {
  lc.setLed(SELECT_JET_PANEL, SEL_LEFT_INNER_COL_A, SEL_LEFT_INNER_ROW_A, newValue);
}
DcsBios::IntegerBuffer sjLiLtBuffer(0x742e, 0x8000, 15, onSjLiLtChange);


void onSjCtrLtChange(unsigned int newValue) {
  lc.setLed(SELECT_JET_PANEL, SEL_CENTER_COL_A, SEL_CENTER_ROW_A, newValue);
}
DcsBios::IntegerBuffer sjCtrLtBuffer(0x742e, 0x4000, 14, onSjCtrLtChange);


void onUfcOptionCueing1Change(char* newValue) {
  if (newValue[0] == ':') {
    lc.setLed(UFC_PANEL, UFC_OPT1_COL_A, UFC_OPT1_ROW_A, 1);
    lc.setLed(UFC_PANEL, UFC_OPT1_COL_B, UFC_OPT1_ROW_B, 1);
  }
  else {
    lc.setLed(UFC_PANEL, UFC_OPT1_COL_A, UFC_OPT1_ROW_A, 0);
    lc.setLed(UFC_PANEL, UFC_OPT1_COL_B, UFC_OPT1_ROW_B, 0);
  }
}
DcsBios::StringBuffer<1> ufcOptionCueing1Buffer(0x7428, onUfcOptionCueing1Change);


void onUfcOptionCueing2Change(char* newValue) {
  if (newValue[0] == ':') {
    lc.setLed(UFC_PANEL, UFC_OPT2_COL_A, UFC_OPT2_ROW_A, 1);
    lc.setLed(UFC_PANEL, UFC_OPT2_COL_B, UFC_OPT2_ROW_B, 1);
  }
  else {
    lc.setLed(UFC_PANEL, UFC_OPT2_COL_A, UFC_OPT2_ROW_A, 0);
    lc.setLed(UFC_PANEL, UFC_OPT2_COL_B, UFC_OPT2_ROW_B, 0);
  }
}
DcsBios::StringBuffer<1> ufcOptionCueing2Buffer(0x742a, onUfcOptionCueing2Change);


void onUfcOptionCueing3Change(char* newValue) {
  if (newValue[0] == ':') {
    lc.setLed(UFC_PANEL, UFC_OPT3_COL_A, UFC_OPT3_ROW_A, 1);
    lc.setLed(UFC_PANEL, UFC_OPT3_COL_B, UFC_OPT3_ROW_B, 1);
  }
  else {
    lc.setLed(UFC_PANEL, UFC_OPT3_COL_A, UFC_OPT3_ROW_A, 0);
    lc.setLed(UFC_PANEL, UFC_OPT3_COL_B, UFC_OPT3_ROW_B, 0);
  }
}
DcsBios::StringBuffer<1> ufcOptionCueing3Buffer(0x742c, onUfcOptionCueing3Change);


void onUfcOptionCueing4Change(char* newValue) {
  if (newValue[0] == ':') {
    lc.setLed(UFC_PANEL, UFC_OPT4_COL_A, UFC_OPT4_ROW_A, 1);
    lc.setLed(UFC_PANEL, UFC_OPT4_COL_B, UFC_OPT4_ROW_B, 1);
  }
  else {
    lc.setLed(UFC_PANEL, UFC_OPT4_COL_A, UFC_OPT4_ROW_A, 0);
    lc.setLed(UFC_PANEL, UFC_OPT4_COL_B, UFC_OPT4_ROW_B, 0);
  }
}
DcsBios::StringBuffer<1> ufcOptionCueing4Buffer(0x742e, onUfcOptionCueing4Change);


void onUfcOptionCueing5Change(char* newValue) {
  if (newValue[0] == ':') {
    lc.setLed(UFC_PANEL, UFC_OPT5_COL_A, UFC_OPT5_ROW_A, 1);
  }
  else {
    lc.setLed(UFC_PANEL, UFC_OPT5_COL_A, UFC_OPT5_ROW_A, 0);
  }
}
DcsBios::StringBuffer<1> ufcOptionCueing5Buffer(0x7430, onUfcOptionCueing5Change);


void onClipApuAccLtChange(unsigned int newValue) {
  lc.setLed(CAUTION_PANEL, APU_ACC_COL_A, APU_ACC_ROW_A, newValue);
  lc.setLed(CAUTION_PANEL, APU_ACC_COL_B, APU_ACC_ROW_B, newValue);
}
DcsBios::IntegerBuffer clipApuAccLtBuffer(0x74a4, 0x0100, 8, onClipApuAccLtChange);


void onClipBattSwLtChange(unsigned int newValue) {
  lc.setLed(CAUTION_PANEL, BATT_SW_COL_A, BATT_SW_ROW_A, newValue);
  lc.setLed(CAUTION_PANEL, BATT_SW_COL_B, BATT_SW_ROW_B, newValue);
}
DcsBios::IntegerBuffer clipBattSwLtBuffer(0x74a4, 0x0200, 9, onClipBattSwLtChange);


void onClipCkSeatLtChange(unsigned int newValue) {
  lc.setLed(CAUTION_PANEL, CK_SEAT_COL_A, CK_SEAT_ROW_A, newValue);
  lc.setLed(CAUTION_PANEL, CK_SEAT_COL_B, CK_SEAT_ROW_B, newValue);
}
DcsBios::IntegerBuffer clipCkSeatLtBuffer(0x74a0, 0x8000, 15, onClipCkSeatLtChange);


void onClipFcesLtChange(unsigned int newValue) {
  lc.setLed(CAUTION_PANEL, FCES_COL_A, FCES_ROW_A, newValue);
  lc.setLed(CAUTION_PANEL, FCES_COL_B, FCES_ROW_B, newValue);
}
DcsBios::IntegerBuffer clipFcesLtBuffer(0x74a4, 0x4000, 14, onClipFcesLtChange);


void onClipFcsHotLtChange(unsigned int newValue) {
  lc.setLed(CAUTION_PANEL, FCS_HOT_COL_A, FCS_HOT_ROW_A, newValue);
  lc.setLed(CAUTION_PANEL, FCS_HOT_COL_B, FCS_HOT_ROW_B, newValue);
}
DcsBios::IntegerBuffer clipFcsHotLtBuffer(0x74a4, 0x0400, 10, onClipFcsHotLtChange);


void onClipFuelLoLtChange(unsigned int newValue) {
  lc.setLed(CAUTION_PANEL, FUEL_LO_COL_A, FUEL_LO_ROW_A, newValue);
  lc.setLed(CAUTION_PANEL, FUEL_LO_COL_B, FUEL_LO_ROW_B, newValue);
}
DcsBios::IntegerBuffer clipFuelLoLtBuffer(0x74a4, 0x2000, 13, onClipFuelLoLtChange);


void onClipGenTieLtChange(unsigned int newValue) {
  lc.setLed(CAUTION_PANEL, GEN_TIE_COL_A, GEN_TIE_ROW_A, newValue);
  lc.setLed(CAUTION_PANEL, GEN_TIE_COL_B, GEN_TIE_ROW_B, newValue);
}
DcsBios::IntegerBuffer clipGenTieLtBuffer(0x74a4, 0x0800, 11, onClipGenTieLtChange);


void onClipLGenLtChange(unsigned int newValue) {
  lc.setLed(CAUTION_PANEL, L_GEN_COL_A, L_GEN_ROW_A, newValue);
  lc.setLed(CAUTION_PANEL, L_GEN_COL_B, L_GEN_ROW_B, newValue);
}
DcsBios::IntegerBuffer clipLGenLtBuffer(0x74a8, 0x0100, 8, onClipLGenLtChange);


void onClipRGenLtChange(unsigned int newValue) {
  lc.setLed(CAUTION_PANEL, R_GEN_COL_A, R_GEN_ROW_A, newValue);
  lc.setLed(CAUTION_PANEL, R_GEN_COL_B, R_GEN_ROW_B, newValue);
}
DcsBios::IntegerBuffer clipRGenLtBuffer(0x74a8, 0x0200, 9, onClipRGenLtChange);


void onClipSpareCtn1LtChange(unsigned int newValue) {
  lc.setLed(CAUTION_PANEL, C_SPARE_1_COL_A, C_SPARE_1_ROW_A, newValue);
  lc.setLed(CAUTION_PANEL, C_SPARE_1_COL_B, C_SPARE_1_ROW_B, newValue);
}
DcsBios::IntegerBuffer clipSpareCtn1LtBuffer(0x74a4, 0x1000, 12, onClipSpareCtn1LtChange);


void onClipSpareCtn2LtChange(unsigned int newValue) {
  lc.setLed(CAUTION_PANEL, C_SPARE_2_COL_A, C_SPARE_2_ROW_A, newValue);
  lc.setLed(CAUTION_PANEL, C_SPARE_2_COL_B, C_SPARE_2_ROW_B, newValue);
}
DcsBios::IntegerBuffer clipSpareCtn2LtBuffer(0x74a4, 0x8000, 15, onClipSpareCtn2LtChange);


void onClipSpareCtn3LtChange(unsigned int newValue) {
  lc.setLed(CAUTION_PANEL, C_SPARE_3_COL_A, C_SPARE_3_ROW_A, newValue);
  lc.setLed(CAUTION_PANEL, C_SPARE_3_COL_B, C_SPARE_3_ROW_B, newValue);
}
DcsBios::IntegerBuffer clipSpareCtn3LtBuffer(0x74a8, 0x0400, 10, onClipSpareCtn3LtChange);


/*//********** LEFT EWI **********\\*/
void onFireLeftLtChange(unsigned int newValue) {
  lc.setLed(LEFT_EWI, LEFT_FIRE_A_COL, LEFT_FIRE_A_ROW, newValue);
  lc.setLed(LEFT_EWI, LEFT_FIRE_B_COL, LEFT_FIRE_B_ROW, newValue);
  lc.setLed(LEFT_EWI, LEFT_FIRE_C_COL, LEFT_FIRE_C_ROW, newValue);
  lc.setLed(LEFT_EWI, LEFT_FIRE_D_COL, LEFT_FIRE_D_ROW, newValue);
}
DcsBios::IntegerBuffer fireLeftLtBuffer(0x7408, 0x0040, 6, onFireLeftLtChange);


void onMasterCautionLtChange(unsigned int newValue) {
  lc.setLed(LEFT_EWI, MSTR_CAUT_A_COL, MSTR_CAUT_A_ROW, newValue);
  lc.setLed(LEFT_EWI, MSTR_CAUT_B_COL, MSTR_CAUT_B_ROW, newValue);
  lc.setLed(LEFT_EWI, MSTR_CAUT_C_COL, MSTR_CAUT_C_ROW, newValue);
  lc.setLed(LEFT_EWI, MSTR_CAUT_D_COL, MSTR_CAUT_D_ROW, newValue);
}
DcsBios::IntegerBuffer masterCautionLtBuffer(0x7408, 0x0200, 9, onMasterCautionLtChange);


void onLhAdvGoChange(unsigned int newValue) {
  lc.setLed(LEFT_EWI, GO_A_COL, GO_A_ROW, newValue);
  lc.setLed(LEFT_EWI, GO_B_COL, GO_B_ROW, newValue);
}
DcsBios::IntegerBuffer lhAdvGoBuffer(0x740a, 0x0010, 4, onLhAdvGoChange);


void onLhAdvNoGoChange(unsigned int newValue) {
  lc.setLed(LEFT_EWI, NO_GO_A_COL, NO_GO_A_ROW, newValue);
  lc.setLed(LEFT_EWI, NO_GO_B_COL, NO_GO_B_ROW, newValue);
}
DcsBios::IntegerBuffer lhAdvNoGoBuffer(0x740a, 0x0020, 5, onLhAdvNoGoChange);


void onLhAdvLBleedChange(unsigned int newValue) {
  lc.setLed(LEFT_EWI, L_BLEED_A_COL, L_BLEED_A_ROW, newValue);
  lc.setLed(LEFT_EWI, L_BLEED_B_COL, L_BLEED_B_ROW, newValue);
}
DcsBios::IntegerBuffer lhAdvLBleedBuffer(0x7408, 0x0800, 11, onLhAdvLBleedChange);


void onLhAdvRBleedChange(unsigned int newValue) {
  lc.setLed(LEFT_EWI, R_BLEED_A_COL, R_BLEED_A_ROW, newValue);
  lc.setLed(LEFT_EWI, R_BLEED_B_COL, R_BLEED_B_ROW, newValue);
}
DcsBios::IntegerBuffer lhAdvRBleedBuffer(0x7408, 0x1000, 12, onLhAdvRBleedChange);


void onLhAdvSpdBrkChange(unsigned int newValue) {
  lc.setLed(LEFT_EWI, SPD_BRK_A_COL, SPD_BRK_A_ROW, newValue);
  lc.setLed(LEFT_EWI, SPD_BRK_B_COL, SPD_BRK_B_ROW, newValue);
}
DcsBios::IntegerBuffer lhAdvSpdBrkBuffer(0x7408, 0x2000, 13, onLhAdvSpdBrkChange);


void onLhAdvStbyChange(unsigned int newValue) {
  lc.setLed(LEFT_EWI, STBY_A_COL, STBY_A_ROW, newValue);
  lc.setLed(LEFT_EWI, STBY_B_COL, STBY_B_ROW, newValue);
}
DcsBios::IntegerBuffer lhAdvStbyBuffer(0x7408, 0x4000, 14, onLhAdvStbyChange);


void onLhAdvLBarRedChange(unsigned int newValue) {
  lc.setLed(LEFT_EWI, L_BAR_RED_A_COL, L_BAR_RED_A_ROW, newValue);
  lc.setLed(LEFT_EWI, L_BAR_RED_B_COL, L_BAR_RED_B_ROW, newValue);
}
DcsBios::IntegerBuffer lhAdvLBarRedBuffer(0x7408, 0x8000, 15, onLhAdvLBarRedChange);


void onLhAdvRecChange(unsigned int newValue) {
  lc.setLed(LEFT_EWI, REC_A_COL, REC_A_ROW, newValue);
  lc.setLed(LEFT_EWI, REC_B_COL, REC_B_ROW, newValue);
}
DcsBios::IntegerBuffer lhAdvRecBuffer(0x740a, 0x0001, 0, onLhAdvRecChange);


void onLhAdvLBarGreenChange(unsigned int newValue) {
  lc.setLed(LEFT_EWI, L_BAR_GREEN_A_COL, L_BAR_GREEN_A_ROW, newValue);
  lc.setLed(LEFT_EWI, L_BAR_GREEN_B_COL, L_BAR_GREEN_B_ROW, newValue);
}
DcsBios::IntegerBuffer lhAdvLBarGreenBuffer(0x740a, 0x0002, 1, onLhAdvLBarGreenChange);


void onLhAdvXmitChange(unsigned int newValue) {
  lc.setLed(LEFT_EWI, XMIT_A_COL, XMIT_A_ROW, newValue);
  lc.setLed(LEFT_EWI, XMIT_B_COL, XMIT_B_ROW, newValue);
}
DcsBios::IntegerBuffer lhAdvXmitBuffer(0x740a, 0x0004, 2, onLhAdvXmitChange);


void onLhAdvAspjOhChange(unsigned int newValue) {
  lc.setLed(LEFT_EWI, ASPJ_OH_A_COL, ASPJ_OH_A_ROW, newValue);
  lc.setLed(LEFT_EWI, ASPJ_OH_B_COL, ASPJ_OH_B_ROW, newValue);
}
DcsBios::IntegerBuffer lhAdvAspjOhBuffer(0x740a, 0x0008, 3, onLhAdvAspjOhChange);


//RIGHT EWI
void onRhAdvDispChange(unsigned int newValue) {
  lc.setLed(RIGHT_EWI, DISP_A_COL, DISP_A_ROW, newValue);
  lc.setLed(RIGHT_EWI, DISP_B_COL, DISP_B_ROW, newValue);
}
DcsBios::IntegerBuffer rhAdvDispBuffer(0x740a, 0x0100, 8, onRhAdvDispChange);


void onRhAdvRcdrOnChange(unsigned int newValue) {
  lc.setLed(RIGHT_EWI, RCDR_ON_A_COL, RCDR_ON_A_ROW, newValue);
  lc.setLed(RIGHT_EWI, RCDR_ON_B_COL, RCDR_ON_B_ROW, newValue);
}
DcsBios::IntegerBuffer rhAdvRcdrOnBuffer(0x740a, 0x0080, 7, onRhAdvRcdrOnChange);


void onRhAdvSpareRh1Change(unsigned int newValue) {
  lc.setLed(RIGHT_EWI, SPARE_1_A_COL, SPARE_1_A_ROW, newValue);
  lc.setLed(RIGHT_EWI, SPARE_1_B_COL, SPARE_1_B_ROW, newValue);
}
DcsBios::IntegerBuffer rhAdvSpareRh1Buffer(0x740a, 0x2000, 13, onRhAdvSpareRh1Change);


void onRhAdvSpareRh2Change(unsigned int newValue) {
  lc.setLed(RIGHT_EWI, SPARE_2_A_COL, SPARE_2_A_ROW, newValue);
  lc.setLed(RIGHT_EWI, SPARE_2_B_COL, SPARE_2_B_ROW, newValue);
}
DcsBios::IntegerBuffer rhAdvSpareRh2Buffer(0x740a, 0x4000, 14, onRhAdvSpareRh2Change);


void onRhAdvSpareRh3Change(unsigned int newValue) {
  lc.setLed(RIGHT_EWI, SPARE_3_A_COL, SPARE_3_A_ROW, newValue);
  lc.setLed(RIGHT_EWI, SPARE_3_B_COL, SPARE_3_B_ROW, newValue);
}
DcsBios::IntegerBuffer rhAdvSpareRh3Buffer(0x740a, 0x8000, 15, onRhAdvSpareRh3Change);


void onRhAdvSpareRh4Change(unsigned int newValue) {
  lc.setLed(RIGHT_EWI, SPARE_4_A_COL, SPARE_4_A_ROW, newValue);
  lc.setLed(RIGHT_EWI, SPARE_4_B_COL, SPARE_4_B_ROW, newValue);
}
DcsBios::IntegerBuffer rhAdvSpareRh4Buffer(0x740c, 0x0001, 0, onRhAdvSpareRh4Change);


void onRhAdvSamChange(unsigned int newValue) {
  lc.setLed(RIGHT_EWI, SAM_A_COL, SAM_A_ROW, newValue);
  lc.setLed(RIGHT_EWI, SAM_B_COL, SAM_B_ROW, newValue);
}
DcsBios::IntegerBuffer rhAdvSamBuffer(0x740a, 0x0200, 9, onRhAdvSamChange);


void onRhAdvSpareRh5Change(unsigned int newValue) {
  lc.setLed(RIGHT_EWI, SPARE_5_A_COL, SPARE_5_A_ROW, newValue);
  lc.setLed(RIGHT_EWI, SPARE_5_B_COL, SPARE_5_B_ROW, newValue);
}
DcsBios::IntegerBuffer rhAdvSpareRh5Buffer(0x740c, 0x0002, 1, onRhAdvSpareRh5Change);


void onRhAdvAiChange(unsigned int newValue) {
  lc.setLed(RIGHT_EWI, AI_A_COL, AI_A_ROW, newValue);
  lc.setLed(RIGHT_EWI, AI_B_COL, AI_B_ROW, newValue);
}
DcsBios::IntegerBuffer rhAdvAiBuffer(0x740a, 0x0400, 10, onRhAdvAiChange);


void onRhAdvAaaChange(unsigned int newValue) {
  lc.setLed(RIGHT_EWI, AAA_A_COL, AAA_A_ROW, newValue);
  lc.setLed(RIGHT_EWI, AAA_B_COL, AAA_B_ROW, newValue);
}
DcsBios::IntegerBuffer rhAdvAaaBuffer(0x740a, 0x0800, 11, onRhAdvAaaChange);


void onRhAdvCwChange(unsigned int newValue) {
  lc.setLed(RIGHT_EWI, CW_A_COL, CW_A_ROW, newValue);
  lc.setLed(RIGHT_EWI, CW_B_COL, CW_B_ROW, newValue);
}
DcsBios::IntegerBuffer rhAdvCwBuffer(0x740a, 0x1000, 12, onRhAdvCwChange);


void onFireApuLtChange(unsigned int newValue) {
  lc.setLed(RIGHT_EWI, APU_FIRE_A_COL, APU_FIRE_A_ROW, newValue);
  lc.setLed(RIGHT_EWI, APU_FIRE_B_COL, APU_FIRE_B_ROW, newValue);
  lc.setLed(RIGHT_EWI, APU_FIRE_C_COL, APU_FIRE_C_ROW, newValue);
  lc.setLed(RIGHT_EWI, APU_FIRE_D_COL, APU_FIRE_D_ROW, newValue);
}
DcsBios::IntegerBuffer fireApuLtBuffer(0x740c, 0x0004, 2, onFireApuLtChange);


void onFireRightLtChange(unsigned int newValue) {
  lc.setLed(RIGHT_EWI, RIGHT_FIRE_A_COL, RIGHT_FIRE_A_ROW, newValue);
  lc.setLed(RIGHT_EWI, RIGHT_FIRE_B_COL, RIGHT_FIRE_B_ROW, newValue);
  lc.setLed(RIGHT_EWI, RIGHT_FIRE_C_COL, RIGHT_FIRE_C_ROW, newValue);
  lc.setLed(RIGHT_EWI, RIGHT_FIRE_D_COL, RIGHT_FIRE_D_ROW, newValue);
}
DcsBios::IntegerBuffer fireRightLtBuffer(0x740c, 0x0010, 4, onFireRightLtChange);


void onRwrBitLtChange(unsigned int newValue) {
  lc.setLed(RWR_DIM, RWR_BIT_COL_A, RWR_BIT_ROW_A, newValue);
}
DcsBios::IntegerBuffer rwrBitLtBuffer(0x749c, 0x1000, 12, onRwrBitLtChange);


void onRwrEnableLtChange(unsigned int newValue) {
  lc.setLed(RWR_DIM, RWR_ENABLE_OFFSET_COL_A, RWR_ENABLE_OFFSET_ROW_A, newValue);
}
DcsBios::IntegerBuffer rwrEnableLtBuffer(0x749c, 0x0200, 9, onRwrEnableLtChange);


void onRwrSpecialEnLtChange(unsigned int newValue) {
  lc.setLed(RWR_DIM, RWR_ENABLE_SPECIAL_COL_A, RWR_ENABLE_SPECIAL_ROW_A, newValue);
}
DcsBios::IntegerBuffer rwrSpecialEnLtBuffer(0x7498, 0x8000, 15, onRwrSpecialEnLtChange);


void onRwrLimitLtChange(unsigned int newValue) {
  lc.setLed(RWR_DIM, RWR_LIMIT_COL_A, RWR_LIMIT_ROW_A, newValue);
}
DcsBios::IntegerBuffer rwrLimitLtBuffer(0x7498, 0x2000, 13, onRwrLimitLtChange);


void onRwrLowerLtChange(unsigned int newValue) {
  lc.setLed(RWR_DIM, RWR_ON_COL_A, RWR_ON_ROW_A, newValue);
  lc.setLed(RWR_DIM, RWR_ALR_67_COL_A, RWR_ALR_67_ROW_A, newValue);
  //INVESTIGATE TO ADD TO BACKLIGHTING
}
DcsBios::IntegerBuffer rwrLowerLtBuffer(0x7498, 0x1000, 12, onRwrLowerLtChange);


void onRwrFailLtChange(unsigned int newValue) {
  lc.setLed(RWR_DIM, RWR_FAIL_RED_COL_A, RWR_FAIL_RED_ROW_A, newValue);
}
DcsBios::IntegerBuffer rwrFailLtBuffer(0x749c, 0x0800, 11, onRwrFailLtChange);


void onRwrOffsetLtChange(unsigned int newValue) {
  lc.setLed(RWR_DIM, RWR_OFFSET_COL_A, RWR_OFFSET_ROW_A, newValue);
}
DcsBios::IntegerBuffer rwrOffsetLtBuffer(0x749c, 0x0400, 10, onRwrOffsetLtChange);


void onRwrSpecialLtChange(unsigned int newValue) {
  lc.setLed(RWR_DIM, RWR_SPECIAL_COL_A, RWR_SPECIAL_ROW_A, newValue);
}
DcsBios::IntegerBuffer rwrSpecialLtBuffer(0x749c, 0x0100, 8, onRwrSpecialLtChange);


void onRwrDisplayLtChange(unsigned int newValue) {
  lc.setLed(RWR_DIM, RWR_DISPLAY_COL_A, RWR_DISPLAY_ROW_A, newValue);
}
DcsBios::IntegerBuffer rwrDisplayLtBuffer(0x7498, 0x4000, 14, onRwrDisplayLtChange);


void onLandingGearHandleLtChange(unsigned int newValue) {
  lc.setLed(LEFT_DIS, GEAR_KNOB_COL_A, GEAR_KNOB_ROW_A, newValue);
}
DcsBios::IntegerBuffer landingGearHandleLtBuffer(0x747e, 0x0800, 11, onLandingGearHandleLtChange);


void onApuReadyLtChange(unsigned int newValue) {
  lc.setLed(LEFT_DIS, APU_LT_COL_A, APU_LT_ROW_A, newValue);
}
DcsBios::IntegerBuffer apuReadyLtBuffer(0x74c2, 0x0800, 11, onApuReadyLtChange);


void onArrestingHookLtChange(unsigned int newValue) {
  lc.setLed(RIGHT_DIS, HOOK_COL_A, HOOK_ROW_A, newValue);
  lc.setLed(RIGHT_DIS, HOOK_COL_B, HOOK_ROW_B, newValue);
}
DcsBios::IntegerBuffer arrestingHookLtBuffer(0x74a0, 0x0400, 10, onArrestingHookLtChange);


void onRadaltGreenLampChange(unsigned int newValue) {
  lc.setLed(RIGHT_DIS, RAD_ALT_G_COL_A, RAD_ALT_G_ROW_A, newValue);
}
DcsBios::IntegerBuffer radaltGreenLampBuffer(0x74a0, 0x0100, 8, onRadaltGreenLampChange);


void onLowAltWarnLtChange(unsigned int newValue) {
  lc.setLed(RIGHT_DIS, RAD_ALT_R_COL_A, RAD_ALT_R_ROW_A, newValue);
}
DcsBios::IntegerBuffer lowAltWarnLtBuffer(0x749c, 0x8000, 15, onLowAltWarnLtChange);




void onAoaIndexerHighChange(unsigned int newValue) {

  lc.setLed(AOA_DIM, AOA_ABOVE_COL, AOA_ABOVE_ROW, newValue);
}
DcsBios::IntegerBuffer aoaIndexerHighBuffer(0x7408, 0x0008, 3, onAoaIndexerHighChange);


void onAoaIndexerNormalChange(unsigned int newValue) {
  lc.setLed(AOA_DIM, AOA_ON_COL, AOA_ON_ROW, newValue);
}
DcsBios::IntegerBuffer aoaIndexerNormalBuffer(0x7408, 0x0010, 4, onAoaIndexerNormalChange);

void onAoaIndexerLowChange(unsigned int newValue) {
  lc.setLed(AOA_DIM, AOA_BELOW_COL, AOA_BELOW_ROW, newValue);
}
DcsBios::IntegerBuffer aoaIndexerLowBuffer(0x7408, 0x0020, 5, onAoaIndexerLowChange);


void onLsLockChange(unsigned int newValue) {
  lc.setLed(LOCK_SHOOT, LOCK_COL_A, LOCK_ROW_A, newValue);
  lc.setLed(LOCK_SHOOT, LOCK_COL_B, LOCK_ROW_B, newValue);
}
DcsBios::IntegerBuffer lsLockBuffer(0x7408, 0x0001, 0, onLsLockChange);

void onLsShootChange(unsigned int newValue) {
  lc.setLed(LOCK_SHOOT, SHOOT_COL_A, SHOOT_ROW_A, newValue);
  lc.setLed(LOCK_SHOOT, SHOOT_COL_B, SHOOT_ROW_B, newValue);
}
DcsBios::IntegerBuffer lsShootBuffer(0x7408, 0x0002, 1, onLsShootChange);

void onLsShootStrobeChange(unsigned int newValue) {
  lc.setLed(LOCK_SHOOT, SHOOT_FLASH_COL_A, SHOOT_FLASH_ROW_A, newValue);
  lc.setLed(LOCK_SHOOT, SHOOT_FLASH_COL_B, SHOOT_FLASH_ROW_B, newValue);
}
DcsBios::IntegerBuffer lsShootStrobeBuffer(0x7408, 0x0004, 2, onLsShootStrobeChange);



void onWarnCautionDimmerChange(unsigned int newValue) {

  WARN_CAUTION_DIMMER_VALUE = map(newValue, 0, 65000, 0, 15);
  updateBrightness();
}
DcsBios::IntegerBuffer warnCautionDimmerBuffer(0x754c, 0xffff, 0, onWarnCautionDimmerChange);

void onHudAoaIndexerChange(unsigned int newValue) {

  //  AOA_DIMMER_VALUE = map(newValue, 0, 65000, 0, 15);

}
DcsBios::IntegerBuffer hudAoaIndexerBuffer(0x745e, 0xffff, 0, onHudAoaIndexerChange);


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

  if (Reflector_In_Use == 1)  {
    max7219udp.beginPacket(reflectorIP, reflectorport);
    max7219udp.println("Entering update brightness");
    max7219udp.endPacket();
  }

  if (Reflector_In_Use == 1)  {
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
    if (address != AOA_DIM) {
      // skip AOA as it is separately controlled
      if (DAY_NIGHT_SWITCH_MODE == DAY_MODE) {
        lc.setIntensity(address, FULL_BRIGHTNESS);
      } else
        lc.setIntensity(address, WARN_CAUTION_DIMMER_VALUE);
    } else {
      lc.setIntensity(address, (AOA_DIMMER_VALUE));
    }

  }
}



void AllOn() {
  for (int displayunit = 0; displayunit < 9; displayunit++) {
    for (int row = 0; row < 8; row++) {
      for (int col = 0; col < 8 ; col++) {
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

// ************************************ End Max7219

// ************************************ Begin Nextron Block


#include <Arduino.h>
#include <Nextion.h>
#define PetesTxPort 19
#define PetesRxPrt 18
SoftwareSerial nextion(PetesTxPort, PetesRxPrt); // SETS SERIAL TO PINS  18/19


int potPin = A4; // DIRECT IEFI DIMING
int valPin = 0;
int brightness;
int RPML; // RPM LEFT
int RPMR; // RPM RIGHT
int NOZL;
int NOZR;
int FFXL;
int FFXR;
int BINGO;
int OILL;
int OILR;
int TMPL;
int TMPR;
int NOZOFF;
int FLUP;
int FLLW;
//int BINGOBIT;
int CODESBIT;
int SPBIT;
int OCOFFBIT;
int ifeiCol; //IFEI Colour (Green or White)

//################## RPM LEFT ##################Y
void onIfeiRpmLChange(char* newValue) {
  RPML = atol(newValue);
  nextion.print("t0.txt=\"");
  nextion.print(RPML);
  nextion.print("\"");
  nextion.write("\xFF\xFF\xFF");

}
DcsBios::StringBuffer<3> ifeiRpmLBuffer(0x749e, onIfeiRpmLChange);

//################## RPM RIGHT ##################Y
void onIfeiRpmRChange(char* newValue) {
  RPMR = atol(newValue);
  nextion.print("t1.txt=\"");
  nextion.print(RPMR);
  nextion.print("\"");
  nextion.write("\xFF\xFF\xFF");
}
DcsBios::StringBuffer<3> ifeiRpmRBuffer(0x74a2, onIfeiRpmRChange);

//################## TEMP LEFT ##################Y
void onIfeiTempLChange(char* newValue) {
  TMPL = atol(newValue);
  nextion.print("t3.txt=\"");
  nextion.print(TMPL);
  nextion.print("\"");
  nextion.write("\xFF\xFF\xFF");
}
DcsBios::StringBuffer<3> ifeiTempLBuffer(0x74a6, onIfeiTempLChange);


//################## TEMP RIGHT ##################Y
void onIfeiTempRChange(char* newValue) {
  TMPR = atol(newValue);
  nextion.print("t2.txt=\"");
  nextion.print(TMPR);
  nextion.print("\"");
  nextion.write("\xFF\xFF\xFF");
}
DcsBios::StringBuffer<3> ifeiTempRBuffer(0x74aa, onIfeiTempRChange);

//################## FUEL FLOW LEFT ##################Y
void onIfeiFfLChange(char* newValue) {
  FFXL = atol(newValue);
  nextion.print("t4.txt=\"");
  nextion.print(FFXL);
  nextion.print("\"");
  nextion.write("\xFF\xFF\xFF");
}
DcsBios::StringBuffer<3> ifeiFfLBuffer(0x7482, onIfeiFfLChange);

//################## FUEL FLOW RIGHT ##################Y
void onIfeiFfRChange(char* newValue) {
  FFXR = atol(newValue);
  nextion.print("t5.txt=\"");
  nextion.print(FFXR);
  nextion.print("\"");
  nextion.write("\xFF\xFF\xFF");
}
DcsBios::StringBuffer<3> ifeiFfRBuffer(0x7486, onIfeiFfRChange);

//################## OIL LEFT ##################Y
void onIfeiOilPressLChange(char* newValue) {
  OILL = atol(newValue);
  nextion.print("t7.txt=\"");
  nextion.print(OILL);
  nextion.print("\"");
  nextion.write("\xFF\xFF\xFF");
}
DcsBios::StringBuffer<3> ifeiOilPressLBuffer(0x7496, onIfeiOilPressLChange);

//################## OIL RIGHT ##################Y
void onIfeiOilPressRChange(char* newValue) {
  OILR = atol(newValue);
  nextion.print("t6.txt=\"");
  nextion.print(OILR);
  nextion.print("\"");
  nextion.write("\xFF\xFF\xFF");
}
DcsBios::StringBuffer<3> ifeiOilPressRBuffer(0x749a, onIfeiOilPressRChange);

//################## FUEL LOWER ##################

void onIfeiFuelDownChange(char* newValue) {
  if (newValue[2] == 32) {
    nextion.print("t9.txt=\"    ");
    nextion.print(newValue);
    nextion.print("\"");
    nextion.write("\xFF\xFF\xFF");
  }
  else if (newValue[3] == 32) {
    nextion.print("t9.txt=\"   ");
    nextion.print(newValue);
    nextion.print("\"");
    nextion.write("\xFF\xFF\xFF");
  }
  else if (newValue[4] == 32) {
    nextion.print("t9.txt=\"  ");
    nextion.print(newValue);
    nextion.print("\"");
    nextion.write("\xFF\xFF\xFF");
  }
  else if (newValue[5] == 32) {
    nextion.print("t9.txt=\" ");
    nextion.print(newValue);
    nextion.print("\"");
    nextion.write("\xFF\xFF\xFF");
  }
  else {
    nextion.print("t9.txt=\"");
    nextion.print(newValue);
    nextion.print("\"");
    nextion.write("\xFF\xFF\xFF");
  }
}
DcsBios::StringBuffer<6> ifeiFuelDownBuffer(0x748a, onIfeiFuelDownChange);

//################# FUEL UPPER ##################
void onIfeiFuelUpChange(char* newValue) {
  if (newValue[2] == 32) {
    nextion.print("t8.txt=\"    ");
    nextion.print(newValue);
    nextion.print("\"");
    nextion.write("\xFF\xFF\xFF");
  }
  else if (newValue[3] == 32) {
    nextion.print("t8.txt=\"   ");
    nextion.print(newValue);
    nextion.print("\"");
    nextion.write("\xFF\xFF\xFF");
  }
  else if (newValue[4] == 32) {
    nextion.print("t8.txt=\"  ");
    nextion.print(newValue);
    nextion.print("\"");
    nextion.write("\xFF\xFF\xFF");
  }
  else if (newValue[5] == 32) {
    nextion.print("t8.txt=\" ");
    nextion.print(newValue);
    nextion.print("\"");
    nextion.write("\xFF\xFF\xFF");
  }
  else {
    nextion.print("t8.txt=\"");
    nextion.print(newValue);
    nextion.print("\"");
    nextion.write("\xFF\xFF\xFF");
  }
}
DcsBios::StringBuffer<6> ifeiFuelUpBuffer(0x7490, onIfeiFuelUpChange);

//################## FUEL LOWER (MODE Button)##################

//################## BINGO ################## Y
void onIfeiBingoChange(char* newValue) {
  BINGO = atol(newValue);
  nextion.print("t10.txt=\"");
  nextion.print(BINGO);
  nextion.print("\"");
  nextion.write("\xFF\xFF\xFF");
}
DcsBios::StringBuffer<5> ifeiBingoBuffer(0x7468, onIfeiBingoChange);

//################## TOP CLOCK ##################
void onIfeiClockHChange(char* newValue) {
  int TIMETOP = atol(newValue);
  nextion.print("t11.txt=\"");
  nextion.print(TIMETOP);
  nextion.print("\"");
  nextion.write("\xFF\xFF\xFF");
}
DcsBios::StringBuffer<2> ifeiClockHBuffer(0x746e, onIfeiClockHChange);

// ":"

void onIfeiDd1Change(char* newValue) {
  nextion.print("t12.txt=\"");
  nextion.print(newValue);
  nextion.print("\"");
  nextion.write("\xFF\xFF\xFF");
}
DcsBios::StringBuffer<1> ifeiDd1Buffer(0x747a, onIfeiDd1Change);

void onIfeiClockMChange(char* newValue) {
  nextion.print("t22.txt=\"");
  nextion.print(newValue);
  nextion.print("\"");
  nextion.write("\xFF\xFF\xFF");
}
DcsBios::StringBuffer<2> ifeiClockMBuffer(0x7470, onIfeiClockMChange);

// ":"

void onIfeiDd2Change(char* newValue) {
  nextion.print("t23.txt=\"");
  nextion.print(newValue);
  nextion.print("\"");
  nextion.write("\xFF\xFF\xFF");
}
DcsBios::StringBuffer<1> ifeiDd2Buffer(0x747c, onIfeiDd2Change);

void onIfeiClockSChange(char* newValue) {
  nextion.print("t24.txt=\"");
  nextion.print(newValue);
  nextion.print("\"");
  nextion.write("\xFF\xFF\xFF");
}
DcsBios::StringBuffer<2> ifeiClockSBuffer(0x7472, onIfeiClockSChange);

//################# TIMMER CLOCK ########################
void onIfeiTimerHChange(char* newValue) {

  nextion.print("t25.txt=\"");
  nextion.print(newValue);
  nextion.print("\"");
  nextion.write("\xFF\xFF\xFF");
}
DcsBios::StringBuffer<2> ifeiTimerHBuffer(0x7474, onIfeiTimerHChange);

// ":"

void onIfeiDd3Change(char* newValue) {
  nextion.print("t26.txt=\"");
  nextion.print(newValue);
  nextion.print("\"");
  nextion.write("\xFF\xFF\xFF");
}
DcsBios::StringBuffer<1> ifeiDd3Buffer(0x747e, onIfeiDd3Change);

void onIfeiTimerMChange(char* newValue) {
  nextion.print("t27.txt=\"");
  nextion.print(newValue);
  nextion.print("\"");
  nextion.write("\xFF\xFF\xFF");
}
DcsBios::StringBuffer<2> ifeiTimerMBuffer(0x7476, onIfeiTimerMChange);

// ":"

void onIfeiDd4Change(char* newValue) {
  nextion.print("t28.txt=\"");
  nextion.print(newValue);
  nextion.print("\"");
  nextion.write("\xFF\xFF\xFF");
}
DcsBios::StringBuffer<1> ifeiDd4Buffer(0x7480, onIfeiDd4Change);

void onIfeiTimerSChange(char* newValue) {
  nextion.print("t29.txt=\"");
  nextion.print(newValue);
  nextion.print("\"");
  nextion.write("\xFF\xFF\xFF");
}
DcsBios::StringBuffer<2> ifeiTimerSBuffer(0x7478, onIfeiTimerSChange);

////////########  IEFI LABELS    #######/////////

// ************** BINGO ***************************
void onIfeiBingoTextureChange(char* newValue) {
  if (strcmp(newValue, "1") == 0) {
    nextion.print("t19.txt=\"");
    nextion.print("BINGO");
    nextion.print("\"");
    nextion.write("\xFF\xFF\xFF");
    nextion.print("t30.txt=\"");
    nextion.print(" ");
    nextion.print("\"");
    nextion.write("\xFF\xFF\xFF");
    nextion.print("t31.txt=\"");
    nextion.print(" ");
    nextion.print("\"");
    nextion.write("\xFF\xFF\xFF");
  }
  else if ((strcmp(newValue, "0") == 0) && (SPBIT == HIGH) && (OCOFFBIT == HIGH)) {
    nextion.print("t19.txt=\"");
    nextion.print("      ");
    nextion.print("\"");
    nextion.write("\xFF\xFF\xFF");
    nextion.print("t30.txt=\"");
    nextion.print(" ");
    nextion.print("\"");
    nextion.write("\xFF\xFF\xFF");
    nextion.print("t31.txt=\"");
    nextion.print(" ");
    nextion.print("\"");
    nextion.write("\xFF\xFF\xFF");
  }
  else if ((strcmp(newValue, "0") == 0) && (SPBIT == HIGH) && (OCOFFBIT == LOW)) {
    nextion.print("t19.txt=\"");
    nextion.print("      ");
    nextion.print("\"");
    nextion.write("\xFF\xFF\xFF");
    nextion.print("t30.txt=\"");
    nextion.print(" ");
    nextion.print("\"");
    nextion.write("\xFF\xFF\xFF");
    nextion.print("t31.txt=\"");
    nextion.print(" ");
    nextion.print("\"");
    nextion.write("\xFF\xFF\xFF");
  }
  else if ((strcmp(newValue, "0") == 0) && (SPBIT == LOW)) {
    nextion.print("t19.txt=\"");
    nextion.print("      ");
    nextion.print("\"");
    nextion.write("\xFF\xFF\xFF");
    nextion.print("t30.txt=\"");
    nextion.print("L");
    nextion.print("\"");
    nextion.write("\xFF\xFF\xFF");
    nextion.print("t31.txt=\"");
    nextion.print("R");
    nextion.print("\"");
    nextion.write("\xFF\xFF\xFF");
  }
}

DcsBios::StringBuffer<1> ifeiBingoTextureBuffer(0x74c6, onIfeiBingoTextureChange);

// ************** SP (CODES) ***************************
void onIfeiSpChange(char* newValue) {

  if (newValue[0] == 83) { // Letter = "S"
    nextion.print("t3.txt=\"");
    nextion.print(newValue);
    nextion.print("\"");
    nextion.write("\xFF\xFF\xFF");
    nextion.print("t4.txt=\"");
    nextion.print(" ");
    nextion.print("\"");
    nextion.write("\xFF\xFF\xFF");
    nextion.print("t5.txt=\"");
    nextion.print(" ");
    nextion.print("\"");
    nextion.write("\xFF\xFF\xFF");
    nextion.print("t10.txt=\"");
    nextion.print(" ");
    nextion.print("\"");
    nextion.write("\xFF\xFF\xFF");
    SPBIT = HIGH;
  }
  else  if (newValue[0] != 83) {
    nextion.print("t3.txt=\"");
    nextion.print(newValue);
    nextion.print("\"");
    nextion.write("\xFF\xFF\xFF");
    SPBIT = LOW;
  }
}

DcsBios::StringBuffer<3> ifeiSpBuffer(0x74b2, onIfeiSpChange);

void onIfeiCodesChange(char* newValue) {
  if (newValue[0] != 32) {
    nextion.print("t2.txt=\"");
    nextion.print(newValue);
    nextion.print("\"");
    nextion.write("\xFF\xFF\xFF");
    CODESBIT = HIGH;
  }
  else
    nextion.print("t2.txt=\"");
  nextion.print(newValue);
  nextion.print("\"");
  nextion.write("\xFF\xFF\xFF");
  CODESBIT = LOW;
}
DcsBios::StringBuffer<3> ifeiCodesBuffer(0x74ae, onIfeiCodesChange);

///////////// FF Texture ///////////////////////
void onIfeiFfTextureChange(char* newValue) {
  if (strcmp(newValue, "1") == 0) {
    nextion.print("t15.txt=\"");
    nextion.println("  FF");
    nextion.println("x100");
    nextion.print("\"");
    nextion.write("\xFF\xFF\xFF");
  }
  else if (strcmp(newValue, "0") == 0) {
    nextion.print("t15.txt=\"");
    nextion.println(" ");
    nextion.println(" ");
    nextion.print("\"");
    nextion.write("\xFF\xFF\xFF");

  }
}
DcsBios::StringBuffer<1> ifeiFfTextureBuffer(0x74c0, onIfeiFfTextureChange);

////////######## <><> NOZ LEFT WORKING <><> ########\\\\\\\\

void onExtNozzlePosLChange(unsigned int newValue) {
  //if (NOZOFF == HIGH){
  NOZL = map(newValue, 0, 65535, 0, 100);
  if (ifeiCol != 2) {
    switch (NOZL) { // NOZ LEFT POSITION IFEI
      case 0 ... 9: nextion.print("p0.pic=0"); break;
      case 10 ... 19: nextion.print("p0.pic=1"); break;
      case 20 ... 29: nextion.print("p0.pic=2"); break;
      case 30 ... 39: nextion.print("p0.pic=3"); break;
      case 40 ... 49: nextion.print("p0.pic=4"); break;
      case 50 ... 59: nextion.print("p0.pic=5"); break;
      case 60 ... 69: nextion.print("p0.pic=6"); break;
      case 70 ... 79: nextion.print("p0.pic=7"); break;
      case 80 ... 89: nextion.print("p0.pic=8"); break;
      case 90 ... 95: nextion.print("p0.pic=9"); break;
      case 96 ... 100: nextion.print("p0.pic=10"); break;
    }
    nextion.write("\xFF\xFF\xFF");
  }
  else if (ifeiCol == 2) {
    switch (NOZL) { // NOZ RIGHT POSITION IFEI
      case 0 ... 9: nextion.print("p0.pic=26"); break;
      case 10 ... 19: nextion.print("p0.pic=27"); break;
      case 20 ... 29: nextion.print("p0.pic=28"); break;
      case 30 ... 39: nextion.print("p0.pic=29"); break;
      case 40 ... 49: nextion.print("p0.pic=30"); break;
      case 50 ... 59: nextion.print("p0.pic=31"); break;
      case 60 ... 69: nextion.print("p0.pic=32"); break;
      case 70 ... 79: nextion.print("p0.pic=33"); break;
      case 80 ... 89: nextion.print("p0.pic=34"); break;
      case 90 ... 95: nextion.print("p0.pic=35"); break;
      case 96 ... 100: nextion.print("p0.pic=36"); break;
    }
    nextion.write("\xFF\xFF\xFF");
  }
}
DcsBios::IntegerBuffer extNozzlePosLBuffer(0x757a, 0xffff, 0, onExtNozzlePosLChange);
//DcsBios::IntegerBuffer extNozzlePosLBuffer(0x7568, 0xffff, 0, onExtNozzlePosLChange);

////////######## <><> NOZ RIGHT WORKING <><> ########\\\\\\\\

void onExtNozzlePosRChange(unsigned int newValue) {
  NOZR = map(newValue, 0, 65535, 0, 100);
  if (ifeiCol != 2) {
    switch (NOZR) { // NOZ RIGHT POSITION IFEI
      case 0 ... 9: nextion.print("p1.pic=11"); break;
      case 10 ... 19: nextion.print("p1.pic=12"); break;
      case 20 ... 29: nextion.print("p1.pic=13"); break;
      case 30 ... 39: nextion.print("p1.pic=14"); break;
      case 40 ... 49: nextion.print("p1.pic=15"); break;
      case 50 ... 59: nextion.print("p1.pic=16"); break;
      case 60 ... 69: nextion.print("p1.pic=17"); break;
      case 70 ... 79: nextion.print("p1.pic=18"); break;
      case 80 ... 89: nextion.print("p1.pic=19"); break;
      case 90 ... 95: nextion.print("p1.pic=20"); break;
      case 96 ... 100: nextion.print("p1.pic=21"); break;
    }
    nextion.write("\xFF\xFF\xFF");
  }
  else if (ifeiCol == 2) {
    switch (NOZR) { // NOZ RIGHT POSITION IFEI
      case 0 ... 9: nextion.print("p1.pic=37"); break;
      case 10 ... 19: nextion.print("p1.pic=38"); break;
      case 20 ... 29: nextion.print("p1.pic=39"); break;
      case 30 ... 39: nextion.print("p1.pic=40"); break;
      case 40 ... 49: nextion.print("p1.pic=41"); break;
      case 50 ... 59: nextion.print("p1.pic=42"); break;
      case 60 ... 69: nextion.print("p1.pic=43"); break;
      case 70 ... 79: nextion.print("p1.pic=44"); break;
      case 80 ... 89: nextion.print("p1.pic=45"); break;
      case 90 ... 95: nextion.print("p1.pic=46"); break;
      case 96 ... 100: nextion.print("p1.pic=47"); break;
    }
    nextion.write("\xFF\xFF\xFF");
  }
}

DcsBios::IntegerBuffer extNozzlePosRBuffer(0x7578, 0xffff, 0, onExtNozzlePosRChange);
//DcsBios::IntegerBuffer extNozzlePosRBuffer(0x7566, 0xffff, 0, onExtNozzlePosRChange);

///////////// OIL Texture ///////////////////////
void onIfeiOilTextureChange(char* newValue) {
  if (strcmp(newValue, "1") == 0) {
    nextion.print("t16.txt=\"");
    nextion.print("OIL");
    nextion.print("\"");
    nextion.write("\xFF\xFF\xFF");
    nextion.print("t21.txt=\"");
    nextion.print("NOZ"); //NOZ LABLE
    nextion.print("\"");
    nextion.write("\xFF\xFF\xFF");
  }
  else if (strcmp(newValue, "0") == 0) {
    nextion.print("t16.txt=\"");
    nextion.print("    ");
    nextion.print("\"");
    nextion.write("\xFF\xFF\xFF");
    nextion.print("t21.txt=\"");
    nextion.print("    ");
    nextion.print("\"");
    nextion.write("\xFF\xFF\xFF");
  }
}
DcsBios::StringBuffer<1> ifeiOilTextureBuffer(0x74c4, onIfeiOilTextureChange);

///////////// RPM Texture ///////////////////////
void onIfeiRpmTextureChange(char* newValue) {
  if (strcmp(newValue, "1") == 0) {
    nextion.print("t13.txt=\"");
    nextion.print("RPM");
    nextion.print("\"");
    nextion.write("\xFF\xFF\xFF");
  }
  else if (strcmp(newValue, "0") == 0) {
    nextion.print("t13.txt=\"");
    nextion.print("      ");
    nextion.print("\"");
    nextion.write("\xFF\xFF\xFF");
  }
}
DcsBios::StringBuffer<1> ifeiRpmTextureBuffer(0x74bc, onIfeiRpmTextureChange);

///////////// TEMP Texture ///////////////////////
void onIfeiTempTextureChange(char* newValue) {
  if (strcmp(newValue, "1") == 0) {
    nextion.print("t14.txt=\"");
    nextion.print("TEMP");
    nextion.print("\"");
    nextion.write("\xFF\xFF\xFF");
  }
  else if (strcmp(newValue, "0") == 0) {
    nextion.print("t14.txt=\"");
    nextion.print("      ");
    nextion.print("\"");
    nextion.write("\xFF\xFF\xFF");
  }
}
DcsBios::StringBuffer<1> ifeiTempTextureBuffer(0x74be, onIfeiTempTextureChange);

///////////// ZULU Texture ///////////////////////
void onIfeiZTextureChange(char* newValue) {
  if (strcmp(newValue, "1") == 0) {
    nextion.print("t32.txt=\"");
    nextion.print("Z");
    nextion.print("\"");
    nextion.write("\xFF\xFF\xFF");
  }
  else if (strcmp(newValue, "0") == 0) {
    nextion.print("t32.txt=\"");
    nextion.print(" ");
    nextion.print("\"");
    nextion.write("\xFF\xFF\xFF");
  }
}
DcsBios::StringBuffer<1> ifeiZTextureBuffer(0x74dc, onIfeiZTextureChange);



void onIfeiRpointerTextureChange(char* newValue) {
  if (strcmp(newValue, "1") == 0) {
    NOZOFF = HIGH;
    nextion.print("p0.pic=0");
    nextion.write("\xFF\xFF\xFF");
    nextion.print("p1.pic=11");
    nextion.write("\xFF\xFF\xFF");
  }
  else {
    NOZOFF = LOW;
    nextion.print("p0.pic=22");
    nextion.write("\xFF\xFF\xFF");

    nextion.print("p1.pic=22");
    nextion.write("\xFF\xFF\xFF");

    nextion.print("t0.txt=\"");
    nextion.print("");
    nextion.print("\"");
    nextion.write("\xFF\xFF\xFF");

    nextion.print("t1.txt=\"");
    nextion.print("");
    nextion.print("\"");
    nextion.write("\xFF\xFF\xFF");

    nextion.print("t2.txt=\"");
    nextion.print("");
    nextion.print("\"");
    nextion.write("\xFF\xFF\xFF");

    nextion.print("t3.txt=\"");
    nextion.print("");
    nextion.print("\"");
    nextion.write("\xFF\xFF\xFF");

    nextion.print("t4.txt=\"");
    nextion.print("");
    nextion.print("\"");
    nextion.write("\xFF\xFF\xFF");

    nextion.print("t5.txt=\"");
    nextion.print("");
    nextion.print("\"");
    nextion.write("\xFF\xFF\xFF");

    nextion.print("t6.txt=\"");
    nextion.print("");
    nextion.print("\"");
    nextion.write("\xFF\xFF\xFF");

    nextion.print("t7.txt=\"");
    nextion.print("");
    nextion.print("\"");
    nextion.write("\xFF\xFF\xFF");

    nextion.print("t7.txt=\"");
    nextion.print("");
    nextion.print("\"");
    nextion.write("\xFF\xFF\xFF");

    nextion.print("t10.txt=\"");
    nextion.print("");
    nextion.print("\"");
    nextion.write("\xFF\xFF\xFF");

    nextion.print("t11.txt=\"");
    nextion.print("");
    nextion.print("\"");
    nextion.write("\xFF\xFF\xFF");

    nextion.print("t30.txt=\"");
    nextion.print("");
    nextion.print("\"");
    nextion.write("\xFF\xFF\xFF");

    nextion.print("t31.txt=\"");
    nextion.print("");
    nextion.print("\"");
    nextion.write("\xFF\xFF\xFF");

  }

}
DcsBios::StringBuffer<1> ifeiRpointerTextureBuffer(0x74da, onIfeiRpointerTextureChange);

///////////// IFEI COLOUR TEXT GREN OR WHITE ///////////////////////

void onCockkpitLightModeSwChange(unsigned int newValue) {

  // Update the bright of max7219 Leds - PT 20220225
  onCockkpitLightModeSwChangeMax7219(newValue);


  ifeiCol = newValue;
  if (ifeiCol == 2) {
    nextion.print("t0.pco=2016");
    nextion.write("\xFF\xFF\xFF");
    nextion.print("t1.pco=2016");
    nextion.write("\xFF\xFF\xFF");
    nextion.print("t2.pco=2016");
    nextion.write("\xFF\xFF\xFF");
    nextion.print("t3.pco=2016");
    nextion.write("\xFF\xFF\xFF");
    nextion.print("t4.pco=2016");
    nextion.write("\xFF\xFF\xFF");
    nextion.print("t5.pco=2016");
    nextion.write("\xFF\xFF\xFF");
    nextion.print("t6.pco=2016");
    nextion.write("\xFF\xFF\xFF");
    nextion.print("t7.pco=2016");
    nextion.write("\xFF\xFF\xFF");
    nextion.print("t8.pco=2016");
    nextion.write("\xFF\xFF\xFF");
    nextion.print("t9.pco=2016");
    nextion.write("\xFF\xFF\xFF");
    nextion.print("t10.pco=2016");
    nextion.write("\xFF\xFF\xFF");
    nextion.print("t11.pco=2016");
    nextion.write("\xFF\xFF\xFF");
    nextion.print("t12.pco=2016");
    nextion.write("\xFF\xFF\xFF");
    nextion.print("t13.pco=2016");
    nextion.write("\xFF\xFF\xFF");
    nextion.print("t14.pco=2016");
    nextion.write("\xFF\xFF\xFF");
    nextion.print("t15.pco=2016");
    nextion.write("\xFF\xFF\xFF");
    nextion.print("t16.pco=2016");
    nextion.write("\xFF\xFF\xFF");
    nextion.print("t17.pco=2016");
    nextion.write("\xFF\xFF\xFF");
    nextion.print("t18.pco=2016");
    nextion.write("\xFF\xFF\xFF");
    nextion.print("t19.pco=2016");
    nextion.write("\xFF\xFF\xFF");
    nextion.print("t20.pco=2016");
    nextion.write("\xFF\xFF\xFF");
    nextion.print("t21.pco=2016");
    nextion.write("\xFF\xFF\xFF");
    nextion.print("t22.pco=2016");
    nextion.write("\xFF\xFF\xFF");
    nextion.print("t23.pco=2016");
    nextion.write("\xFF\xFF\xFF");
    nextion.print("t24.pco=2016");
    nextion.write("\xFF\xFF\xFF");
    nextion.print("t25.pco=2016");
    nextion.write("\xFF\xFF\xFF");
    nextion.print("t26.pco=2016");
    nextion.write("\xFF\xFF\xFF");
    nextion.print("t27.pco=2016");
    nextion.write("\xFF\xFF\xFF");
    nextion.print("t28.pco=2016");
    nextion.write("\xFF\xFF\xFF");
    nextion.print("t29.pco=2016");
    nextion.write("\xFF\xFF\xFF");
    nextion.print("t30.pco=2016");
    nextion.write("\xFF\xFF\xFF");
    nextion.print("t31.pco=2016");
    nextion.write("\xFF\xFF\xFF");
    nextion.print("t32.pco=2016");
    nextion.write("\xFF\xFF\xFF");
  }
  if (ifeiCol != 2) {
    nextion.print("t0.pco=65535");
    nextion.write("\xFF\xFF\xFF");
    nextion.print("t1.pco=65535");
    nextion.write("\xFF\xFF\xFF");
    nextion.print("t2.pco=65535");
    nextion.write("\xFF\xFF\xFF");
    nextion.print("t3.pco=65535");
    nextion.write("\xFF\xFF\xFF");
    nextion.print("t4.pco=65535");
    nextion.write("\xFF\xFF\xFF");
    nextion.print("t5.pco=65535");
    nextion.write("\xFF\xFF\xFF");
    nextion.print("t6.pco=65535");
    nextion.write("\xFF\xFF\xFF");
    nextion.print("t7.pco=65535");
    nextion.write("\xFF\xFF\xFF");
    nextion.print("t8.pco=65535");
    nextion.write("\xFF\xFF\xFF");
    nextion.print("t9.pco=65535");
    nextion.write("\xFF\xFF\xFF");
    nextion.print("t10.pco=65535");
    nextion.write("\xFF\xFF\xFF");
    nextion.print("t11.pco=65535");
    nextion.write("\xFF\xFF\xFF");
    nextion.print("t12.pco=65535");
    nextion.write("\xFF\xFF\xFF");
    nextion.print("t13.pco=65535");
    nextion.write("\xFF\xFF\xFF");
    nextion.print("t14.pco=65535");
    nextion.write("\xFF\xFF\xFF");
    nextion.print("t15.pco=65535");
    nextion.write("\xFF\xFF\xFF");
    nextion.print("t16.pco=65535");
    nextion.write("\xFF\xFF\xFF");
    nextion.print("t17.pco=65535");
    nextion.write("\xFF\xFF\xFF");
    nextion.print("t18.pco=65535");
    nextion.write("\xFF\xFF\xFF");
    nextion.print("t19.pco=65535");
    nextion.write("\xFF\xFF\xFF");
    nextion.print("t20.pco=65535");
    nextion.write("\xFF\xFF\xFF");
    nextion.print("t21.pco=65535");
    nextion.write("\xFF\xFF\xFF");
    nextion.print("t22.pco=65535");
    nextion.write("\xFF\xFF\xFF");
    nextion.print("t23.pco=65535");
    nextion.write("\xFF\xFF\xFF");
    nextion.print("t24.pco=65535");
    nextion.write("\xFF\xFF\xFF");
    nextion.print("t25.pco=65535");
    nextion.write("\xFF\xFF\xFF");
    nextion.print("t26.pco=65535");
    nextion.write("\xFF\xFF\xFF");
    nextion.print("t27.pco=65535");
    nextion.write("\xFF\xFF\xFF");
    nextion.print("t28.pco=65535");
    nextion.write("\xFF\xFF\xFF");
    nextion.print("t29.pco=65535");
    nextion.write("\xFF\xFF\xFF");
    nextion.print("t30.pco=65535");
    nextion.write("\xFF\xFF\xFF");
    nextion.print("t31.pco=65535");
    nextion.write("\xFF\xFF\xFF");
    nextion.print("t32.pco=65535");
    nextion.write("\xFF\xFF\xFF");
  }
}
DcsBios::IntegerBuffer cockkpitLightModeSwBuffer(0x74c8, 0x0600, 9, onCockkpitLightModeSwChange);


void onIfeiChange(unsigned int newValue) {

  //    int brightness = analogRead(A4);
  int bright = map(newValue, 0, 65500, 0, 100);
  String dim = "dim=" + String(bright);
  brightness = bright;
  nextion.print(dim.c_str());
  nextion.write("\xFF\xFF\xFF");

}
DcsBios::IntegerBuffer ifeiBuffer(0x74de, 0xffff, 0, onIfeiChange);

// ************************************ End Nextron Block



// ************************************ Begin Relay Block

void onChartIntLtChange(unsigned int newValue) {
  if (newValue != 0) {
    digitalWrite(BACKLIGHTING_RELAY_PORT, true);
  } else {
    digitalWrite(BACKLIGHTING_RELAY_PORT, false);
  }
}
DcsBios::IntegerBuffer chartIntLtBuffer(0x755e, 0xffff, 0, onChartIntLtChange);



void onLGenSwChange(unsigned int newValue) {
  if (newValue != 0 ) {
    LEFT_GEN_SWITCH_STATE = true;
  } else {
    LEFT_GEN_SWITCH_STATE = false;
  }
  CheckLeftAndAMPCDScreenPowerState();
}
DcsBios::IntegerBuffer lGenSwBuffer(0x74c4, 0x2000, 13, onLGenSwChange);

void onRGenSwChange(unsigned int newValue) {
  if (newValue != 0 ) {
    RIGHT_GEN_SWITCH_STATE = true;
  } else {
    RIGHT_GEN_SWITCH_STATE = false;
  }
  CheckRightScreenPowerState();
}
DcsBios::IntegerBuffer rGenSwBuffer(0x74c4, 0x4000, 14, onRGenSwChange);


void onBatterySwChange(unsigned int newValue) {
  if (newValue != 1 ) {
    BATTERY_SWITCH_STATE = true;
  } else {
    BATTERY_SWITCH_STATE = false;
  }
  CheckRightScreenPowerState();


  if (newValue == 1) {
    lc.setLed(RWR_DIM, RWR_ALR_67_COL_A, RWR_ALR_67_ROW_A, 0);
  } else {
    lc.setLed(RWR_DIM, RWR_ALR_67_COL_A, RWR_ALR_67_ROW_A, 1);
  }
}
DcsBios::IntegerBuffer batterySwBuffer(0x74c4, 0x1800, 11, onBatterySwChange);


//bool BATTERY_SWITCH_STATE = true;

void CheckLeftAndAMPCDScreenPowerState() {

  if ((LEFT_GEN_SWITCH_STATE == true) || ( BATTERY_SWITCH_STATE == true)) {
    digitalWrite(LEFT_AND_AMPCD_SCREENS_RELAY_PORT, true);
  } else {
    // Not powering down monitors as this causes Windows to disconnect them
    // digitalWrite(LEFT_AND_AMPCD_SCREENS_RELAY_PORT, false);
  }
}

void CheckRightScreenPowerState() {

  if ((RIGHT_GEN_SWITCH_STATE == true) || ( BATTERY_SWITCH_STATE == true)) {
    digitalWrite(RIGHT_SCREEN_RELAY_PORT, true);
  } else {
    // Not powering down monitors as this causes Windows to disconnect them
    // digitalWrite(RIGHT_SCREEN_RELAY_PORT, false);
  }
}

// ************************************ End Relay Block



// ************************************ Begin Exterior and Interior Lights Block

// FORMATION LIGHTS
void onExtFormationLightsChange(unsigned int newValue) {
  analogWrite(FORMATION_LIGHTS, map(newValue, 0, 65535, 0, 255));
}
DcsBios::IntegerBuffer extFormationLightsBuffer(0x7576, 0xffff, 0, onExtFormationLightsChange);

// POSITION/NAVIGATION LIGHTS
void onPositionDimmerChange(unsigned int newValue) {
  POSITION_BRIGHT_POT_POS = newValue;
  if (POSITION_LIGHTS_STATUS == true)
    analogWrite(NAVIGATION_LIGHTS, map(POSITION_BRIGHT_POT_POS, 0, 65535, 0, 255));
}
DcsBios::IntegerBuffer positionDimmerBuffer(0x7524, 0xffff, 0, onPositionDimmerChange);

void onExtPositionLightLeftChange(unsigned int newValue) {
  if (newValue != 0) {
    analogWrite(NAVIGATION_LIGHTS, map(POSITION_BRIGHT_POT_POS, 0, 65535, 0, 255));
    POSITION_LIGHTS_STATUS = true;
  } else {
    digitalWrite(NAVIGATION_LIGHTS, LOW);
  }
}
DcsBios::IntegerBuffer extPositionLightLeftBuffer(0x74d6, 0x0400, 10, onExtPositionLightLeftChange);

// STROBE LIGHTS

// Strobe Switch Positions
// Bright 2
// Off    1
// Dim    0
//POSITION_BRIGHT_POT_POS

void onExtStrobeLightsChange(unsigned int newValue) {
  if (newValue != 0) {
    if (STROBE_BRIGHT_SWITCH_POS == STROBE_BRIGHT)
      analogWrite(STROBE_LIGHTS, STROBE_BRIGHT_LEVEL);
    else
      analogWrite(STROBE_LIGHTS, STROBE_DIM_LEVEL);
  } else {
    digitalWrite(STROBE_LIGHTS, LOW);
  }
}
DcsBios::IntegerBuffer extStrobeLightsBuffer(0x74d6, 0x2000, 13, onExtStrobeLightsChange);


void onStrobeSwChange(unsigned int newValue) {
  STROBE_BRIGHT_SWITCH_POS = newValue;
}
DcsBios::IntegerBuffer strobeSwBuffer(0x74b0, 0x3000, 12, onStrobeSwChange);

void onFloodIntLtChange(unsigned int newValue) {
  analogWrite(FLOOD_LIGHTS, map(newValue, 0, 65535, 0, 255));
}
DcsBios::IntegerBuffer floodIntLtBuffer(0x755a, 0xffff, 0, onFloodIntLtChange);

void onConsoleIntLtChange(unsigned int newValue) {
  if (newValue <= 20000) {
    analogWrite(BACK_LIGHTS, 0);
  } else {
    // analogWrite(BACK_LIGHTS, map(newValue, 7000, 65535, 0, 255));
    analogWrite(BACK_LIGHTS, map(newValue, 7000, 65535, 0, 40));
  }
}
DcsBios::IntegerBuffer consoleIntLtBuffer(0x7558, 0xffff, 0, onConsoleIntLtChange);

void onNvgFloodIntLtChange(unsigned int newValue) {
  if (newValue <= 7000) {
    analogWrite(NVG_LIGHTS, 0);
  } else {
    analogWrite(NVG_LIGHTS, map(newValue, 7000, 65535, 0, 255));
  }
}
DcsBios::IntegerBuffer nvgFloodIntLtBuffer(0x755c, 0xffff, 0, onNvgFloodIntLtChange);



// ************************************ End Exterior and Interior Lights Block

// Source
// https://gist.github.com/jboecker/1084b3768c735b164c34d6087d537c18
// the Warthog Project Video on the compass build
// https://www.youtube.com/watch?v=ZN9glqgp9TY&t=332s


#include <AccelStepper.h>
struct StepperConfig {
  unsigned int maxSteps;
  unsigned int acceleration;
  unsigned int maxSpeed;
};

const long zeroTimeout = 30000;

class Vid60Stepper : public DcsBios::Int16Buffer {
  private:
    AccelStepper& stepper;
    StepperConfig& stepperConfig;
    inline bool zeroDetected() {
      return digitalRead(irDetectorPin) == 0;
    }
    unsigned int (*map_function)(unsigned int);
    unsigned char initState;
    long currentStepperPosition;
    long lastAccelStepperPosition;
    unsigned char irDetectorPin;
    long zeroOffset;
    bool movingForward;
    bool lastZeroDetectState;

    long zeroPosSearchStartTime = 0;

    long normalizeStepperPosition(long pos) {
      if (pos < 0) return pos + stepperConfig.maxSteps;
      if (pos >= stepperConfig.maxSteps) return pos - stepperConfig.maxSteps;
      return pos;
    }

    void updateCurrentStepperPosition() {
      // adjust currentStepperPosition to include the distance our stepper motor
      // was moved since we last updated it
      long movementSinceLastUpdate = stepper.currentPosition() - lastAccelStepperPosition;
      currentStepperPosition = normalizeStepperPosition(currentStepperPosition + movementSinceLastUpdate);
      lastAccelStepperPosition = stepper.currentPosition();
    }
  public:
    Vid60Stepper(unsigned int address, AccelStepper& stepper, StepperConfig& stepperConfig, unsigned char irDetectorPin, long zeroOffset, unsigned int (*map_function)(unsigned int))
      : Int16Buffer(address), stepper(stepper), stepperConfig(stepperConfig), irDetectorPin(irDetectorPin), zeroOffset(zeroOffset), map_function(map_function), initState(0), currentStepperPosition(0), lastAccelStepperPosition(0) {
    }

    virtual void loop() {
      if (initState == 0) { // not initialized yet
        pinMode(irDetectorPin, INPUT);
        stepper.setMaxSpeed(stepperConfig.maxSpeed);
        stepper.setSpeed(400);

        initState = 1;
        zeroPosSearchStartTime = millis();
      }

      if (initState == 1) {
        // move off zero if already there so we always get movement on reset
        // (to verify that the stepper is working)
        if (zeroDetected()) {
          stepper.runSpeed();
        } else {
          initState = 2;
        }
      }

      if (initState == 2) { // zeroing
        if (!zeroDetected()) {
          // Currently this safety check isn't working
          // Add Ethernet card for more troubleshooting
          // Need to check IP addresses of PC secondary nic
          if (millis() >= (zeroTimeout + zeroPosSearchStartTime)) {
            stepper.disableOutputs();
            initState == 99;
          }
          else
            stepper.runSpeed();



        } else {
          stepper.setAcceleration(stepperConfig.acceleration);
          stepper.runToNewPosition(stepper.currentPosition() + zeroOffset);
          // tell the AccelStepper library that we are at position zero
          stepper.setCurrentPosition(0);
          lastAccelStepperPosition = 0;
          // set stepper acceleration in steps per second per second
          // (default is zero)
          stepper.setAcceleration(stepperConfig.acceleration);

          lastZeroDetectState = true;
          initState = 3;
        }
      }
      if (initState == 3) { // running normally

        // recalibrate when passing through zero position
        bool currentZeroDetectState = zeroDetected();
        if (!lastZeroDetectState && currentZeroDetectState && movingForward) {
          // we have moved from left to right into the 'zero detect window'
          // and are now at position 0
          lastAccelStepperPosition = stepper.currentPosition();
          currentStepperPosition = normalizeStepperPosition(zeroOffset);
        } else if (lastZeroDetectState && !currentZeroDetectState && !movingForward) {
          // we have moved from right to left out of the 'zero detect window'
          // and are now at position (maxSteps-1)
          lastAccelStepperPosition = stepper.currentPosition();
          currentStepperPosition = normalizeStepperPosition(stepperConfig.maxSteps + zeroOffset);
        }
        lastZeroDetectState = currentZeroDetectState;


        if (hasUpdatedData()) {
          // convert data from DCS to a target position expressed as a number of steps
          long targetPosition = (long)map_function(getData());

          updateCurrentStepperPosition();

          long delta = targetPosition - currentStepperPosition;

          // if we would move more than 180 degree counterclockwise, move clockwise instead
          if (delta < -((long)(stepperConfig.maxSteps / 2))) delta += stepperConfig.maxSteps;
          // if we would move more than 180 degree clockwise, move counterclockwise instead
          if (delta > (stepperConfig.maxSteps / 2)) delta -= (long)stepperConfig.maxSteps;

          movingForward = (delta >= 0);

          // tell AccelStepper to move relative to the current position
          stepper.move(delta);

        }
        stepper.run();
      }

      if (initState == 99) { // Timed out looking for zero do nothing
        stepper.disableOutputs();
      }
    }
};

/* modify below this line */

/* define stepper parameters
   multiple Vid60Stepper instances can share the same StepperConfig object */
struct StepperConfig stepperConfig = {
  720,  // maxSteps
  200, // maxSpeed
  50 // acceleration
};


// define AccelStepper instance
AccelStepper stepper(AccelStepper::FULL4WIRE, 2, 11, 3, 12);

// define Vid60Stepper class that uses the AccelStepper instance defined in the line above
//           v-- arbitrary name
// Vid60Stepper alt100ftPointer(0x107e,          // address of stepper data
Vid60Stepper standbyCompass(0x0436,          // address of stepper data
                            stepper,         // name of AccelStepper instance
                            stepperConfig,   // StepperConfig struct instance
                            9,              // IR Detector Pin (must be LOW in zero position)
                            0,               // zero offset
[](unsigned int newValue) -> unsigned int {
  /* this function needs to map newValue to the correct number of steps */

  // For most guages this map will do
  //return map(newValue, 0, 65535, 0, stepperConfig.maxSteps - 1);

  // For the compass we only has 360 degrees and need to exclude upper part
  // of 16 bit value
  //Output Type: integer Address: 0x0436 Mask: 0x01ff Shift By: 0 Max. Value: 360 Description: Heading (Degrees)
  // so instead of 0 to 65000 its 0 to 360. Need to exclude upper part of 16 bit value
  newValue = newValue & 0x01ff;
  return map(newValue, 0, 360, 0, stepperConfig.maxSteps - 1);
});


// ************************************ Begin Compass Block



void ProcessReceivedString()
{

  // Reading values from led packetBuffer which is global
  // All received values are strings for readability
  // From 737 Project

  bool bLocalDebug = true;
  int tempVar = 0;

  if ((Debug_Display || bLocalDebug ) && Serial_In_Use)  Serial.println("Processing Max7219 Packet");


  String sWrkStr = "";
  const char *delim  = "=";


  ParameterNamePtr = strtok(max7219packetBuffer, delim);
  String ParameterNameString(ParameterNamePtr);
  if ((Debug_Display || bLocalDebug ) && Serial_In_Use)  Serial.println("Parameter Name " + ParameterNameString);

  ParameterValuePtr   = strtok(NULL, delim);
  String ParameterValue(ParameterValuePtr);
  if ((Debug_Display || bLocalDebug ) && Serial_In_Use)  Serial.println("Parameter Value " + ParameterValue);


  if ((Debug_Display || bLocalDebug ) && Serial_In_Use) Serial.println("Found AOA Brightness");
  AOA_DIMMER_VALUE = ParameterValue.toInt();

  updateBrightness();

}






// ****************************** Start May 28

void ProcessReceivedMSFSString()
{

  // Reading values from packetBuffer which is global
  // All received values are strings for readability

  // Old versions handled a single attribute/value pair at a time eg I01=1
  // New Version Handles multiple attribute Values in a single packet
  //    D,1:0,2:1,3:1,4:1,5:0,6:0,7:0,8:0,9:1,10:1


  bool bLocalDebug = true;

  //if (Debug_Display || bLocalDebug ) Serial.println("Processing Packet :" + String(millis()));
  SendReflectorMessage("Processing Packet");

  bLocalDebug = false;

  String sWrkStr = "";

  // const char *delim  = "=";
  const char *delim  = ",";

  // Break the received packet into a series of tokens
  // If there is no match the pointer will be null, other points to first parameter
  ParameterNamePtr = strtok(max7219packetBuffer, delim);



  String ParameterNameString(ParameterNamePtr);
  //if (Debug_Display || bLocalDebug ) Serial.println("Parameter Name " + String(ParameterNameString));
  SendReflectorMessage("Parameter Name " + String(ParameterNameString));
  // Print all of the values received as a outer loop
  // and then split inner values
  /* get the first token */

  /* walk through other tokens */

  String wrkstring = "";

  //if (Debug_Display || bLocalDebug )
  //if (ParameterNamePtr != NULL) Serial.println("First Value is: " + String(ParameterNamePtr));
  if (ParameterNameString[0] == 'D')
  {
    //Handling a Data Packet
    //if (Debug_Display || bLocalDebug ) Serial.println("Handling a Data Packet");
    ParameterNamePtr = strtok(NULL, delim);

    while ( ParameterNamePtr != NULL ) {
      //if (Debug_Display || bLocalDebug ) Serial.println( "Processing " + String(ParameterNamePtr) );

      wrkstring = String(ParameterNamePtr);
      HandleOutputValuePair(wrkstring);


      ParameterNamePtr = strtok(NULL, delim);
    }

    return;
    // End Handling a Data Packet
  }
  else if (ParameterNameString[0] == 'C')
  {
    // Handling a Control Packet
    //if (Debug_Display || bLocalDebug ) Serial.println("Handling a Control Packet");

    ParameterNamePtr = strtok(NULL, delim);

    while ( ParameterNamePtr != NULL ) {
      //if (Debug_Display || bLocalDebug )Serial.println( "Processing " + String(ParameterNamePtr) );

      wrkstring = String(ParameterNamePtr);
      HandleControlString(wrkstring);

      ParameterNamePtr = strtok(NULL, delim);
    }


    return;

    // Handling a Control Packet
  }
  else
  {
    // Unknown Packet Type
    //if (Debug_Display || bLocalDebug ) Serial.println("Unknown Packet Type - ignoring packet");
    return;
  }

}


void HandleOutputValuePair( String str)
{

  // We are expected a LedNumber which has XRRCC where X = Max7219 Unit, RR Row, CC Column
  bool bLocalDebug = false;

  const int ExpectedLedNumberLength = 5;
  const int ExpectedLedValueLength = 1;

  const int Max7219UnitNumber = 6;
  const int MaxRowNumber = 15;
  const int MaxColNumber = 15;
  const int MaxLedValue = 1;

  //if (Debug_Display || bLocalDebug ) Serial.println("Handling " + str);
  SendReflectorMessage("Handling " + str);

  int delimeterlocation = 0;
  String lednumber = "";
  String ledvalue = "";
  String workstring = "";


  int i7219UnitNumber = 0;
  int iledRow = 0;
  int iledColumn = 0;
  int iledValue = 0;


  delimeterlocation = str.indexOf(':');

  if (delimeterlocation == 0)
  {
    //if (Debug_Display || bLocalDebug ) Serial.println("**** WARNING no delimiter passed ****** Looking for :");
  }
  else
  {
    //if (Debug_Display || bLocalDebug ) Serial.println("Delimiter is located a position " + String(delimeterlocation));
    lednumber = getValue(str, ':', 0);
    //if (Debug_Display || bLocalDebug ) Serial.println("lednumber is " + lednumber);
    ledvalue = getValue(str, ':', 1);
    //if (Debug_Display || bLocalDebug ) Serial.println("ledvalue is " + ledvalue);


    // First Check Lengths are ok
    if (lednumber.length() != ExpectedLedNumberLength) {
      //if (Debug_Display || bLocalDebug ) Serial.println("lednumber has incorrect length of " + String(lednumber.length()) + ". Should be " + String(ExpectedLedNumberLength));
      return;
    }


    // As the value could contain the null at the end of the string trim it out
    ledvalue.trim();
    if (ledvalue.length() != ExpectedLedValueLength) {
      //if (Debug_Display || bLocalDebug ) Serial.println("ledvalue has incorrect length of " + String(ledvalue.length()) + ". Should be " + String(ExpectedLedValueLength));
      return;
    }




    // Check we are only have numberic characters
    if ( ! isValidNumber(lednumber) ) {
      //if (Debug_Display || bLocalDebug ) Serial.println("lednumber is not a number " + lednumber);
      return;
    }

    if ( ! isValidNumber(ledvalue) ) {
      //if (Debug_Display || bLocalDebug ) Serial.println("ledvalue is not a number " + ledvalue);
      return;
    }




    // Pull out Max7219 Unit Number, Row and Column Numbers
    // Max 7219 Number
    workstring = lednumber.substring(0, 1);
    //if (Debug_Display || bLocalDebug ) Serial.println("Max7219 Unit Number is :" + workstring);
    i7219UnitNumber = workstring.toInt();

    if (i7219UnitNumber > Max7219UnitNumber) {
      //if (Debug_Display || bLocalDebug ) Serial.println("Max7219 Unit Number is " + String(i7219UnitNumber) + " and is greater than allowed " + String(Max7219UnitNumber));
      return;
    }

    // Led Row
    workstring = lednumber.substring(1, 3);
    //if (Debug_Display || bLocalDebug ) Serial.println("Led Row Number is :" + workstring);
    iledRow = workstring.toInt();

    if (iledRow > MaxRowNumber) {
      //if (Debug_Display || bLocalDebug ) Serial.println("Led Row Number is " + String(iledRow) + " and is greater than allowed " + String(MaxRowNumber));
      return;
    }

    // Led Column
    workstring = lednumber.substring(3, 5);
    //if (Debug_Display || bLocalDebug ) Serial.println("Led Column Number is :" + workstring);
    iledColumn = workstring.toInt();

    if (iledColumn > MaxColNumber) {
      //if (Debug_Display || bLocalDebug ) Serial.println("Led Column Number is " + String(iledColumn) + " and is greater than allowed " + String(MaxColNumber));
      return;
    }


    //Led Value
    workstring = ledvalue.substring(0, 1);
    //if (Debug_Display || bLocalDebug ) Serial.println("Led Value is :" + workstring);
    iledValue = workstring.toInt();

    if (iledValue > MaxLedValue) {
      //if (Debug_Display || bLocalDebug ) Serial.println("Led Value is " + String(iledValue) + " and is greater than allowed " + String(MaxLedValue));
      return;
    }



    SendReflectorMessage("Set Led " + String(i7219UnitNumber) + ":" + String(iledRow) + ":" +
                         String(iledColumn) + ":" + String(iledValue));

    // Have Now Validated payload - set the led
    if (iledValue == 0)
    {
      //if (Debug_Display || bLocalDebug ) Serial.println("Clearing - Row:" + String(iledRow) + " Column:" +  String(iledColumn));
      lc.setLed(i7219UnitNumber, iledColumn, iledRow, false);

    }
    else
    {
      //if (Debug_Display || bLocalDebug ) Serial.println("Lighting - Row:" + String(iledRow) + " Column:" +  String(iledColumn));
      lc.setLed(i7219UnitNumber, iledColumn, iledRow, true);
    }

  }
  return;

}


void HandleControlString( String str)
{
  bool bLocalDebug = false;
  //if (Debug_Display || bLocalDebug ) Serial.println("Handling Control String :" + str);

  // Currnetly just handling Brightness - eg C,B:3

  int delimeterlocation = 0;
  String command = "";
  String setting = "";


  delimeterlocation = str.indexOf(':');

  if (delimeterlocation == 0)
  {
    //if (Debug_Display || bLocalDebug ) Serial.println("**** WARNING no delimiter passed ****** Looking for :");
  }
  else
  {
    //if (Debug_Display || bLocalDebug ) Serial.println("Delimiter is located a position " + String(delimeterlocation));
    command = getValue(str, ':', 0);
    //if (Debug_Display || bLocalDebug ) Serial.println("command is :" + command);
    setting = getValue(str, ':', 1);
    //if (Debug_Display || bLocalDebug ) Serial.println("setting is :" + setting);

    int isetting = setting.toInt();

    if (command == "B")
      if (isetting >= 0 && isetting <= 15)
      {
        lc.setIntensity(0, isetting);
        /////// Begin Backlight test
        analogWrite(NVG_LIGHTS, map(isetting, 0, 15, 0, 255));
        analogWrite(FLOOD_LIGHTS, map(isetting, 0, 15, 0, 255));
      }
    //else if (Debug_Display || bLocalDebug ) Serial.println("Invalid Brightness value passed. Value is :" + String(setting));
  }

  return;

}

String getValue(String data, char separator, int index)
{
  int found = 0;
  int strIndex[] = { 0, -1 };
  int maxIndex = data.length() - 1;

  for (int i = 0; i <= maxIndex && found <= index; i++) {
    if (data.charAt(i) == separator || i == maxIndex) {
      found++;
      strIndex[0] = strIndex[1] + 1;
      strIndex[1] = (i == maxIndex) ? i + 1 : i;
    }
  }
  return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}

boolean isValidNumber(String str)
{
  boolean isNum = false;
  if (!(str.charAt(0) == '+' || str.charAt(0) == '-' || isDigit(str.charAt(0)))) return false;

  for (byte i = 1; i < str.length(); i++)
  {
    if (!(isDigit(str.charAt(i)) || str.charAt(i) == '.')) return false;
  }
  return true;
}

void SendReflectorMessage(String messageString)
{
  if (Reflector_In_Use == 1) {
    max7219udp.beginPacket(reflectorIP, reflectorport);
    max7219udp.println(messageString + " " +  String(millis()) + "mS since reset.");
    max7219udp.endPacket();
  }
}
// ****************************** End May 28

void setup() {

  // Initialise Relay output ports
  // Note - Generally turning a relay port off triggers the relay
  // but the wiring is such that when the relay isn't triggerd things are powered
  // So it is a ouble negative - turn the port on which leaves the relay unactivated
  // which leaves devices powered.

  pinMode( RELAY_PORT_1,  OUTPUT);
  pinMode( RELAY_PORT_2,  OUTPUT);
  pinMode( RELAY_PORT_3,  OUTPUT);
  pinMode( RELAY_PORT_4,  OUTPUT);

  digitalWrite(RELAY_PORT_1, true);
  digitalWrite(RELAY_PORT_2, true);
  digitalWrite(RELAY_PORT_3, true);
  digitalWrite(RELAY_PORT_4, true);

  // Initialise Exterior Lights
  pinMode(STROBE_LIGHTS, OUTPUT);
  pinMode(NAVIGATION_LIGHTS, OUTPUT);
  pinMode(FORMATION_LIGHTS, OUTPUT);
  pinMode(BACK_LIGHTS, OUTPUT);
  pinMode(FLOOD_LIGHTS, OUTPUT);

  digitalWrite(STROBE_LIGHTS, LOW);
  digitalWrite(NAVIGATION_LIGHTS, LOW);
  digitalWrite(FORMATION_LIGHTS, LOW);
  digitalWrite(BACK_LIGHTS, LOW);
  digitalWrite(FLOOD_LIGHTS, LOW);


  if (Ethernet_In_Use == 1) {
    delay(EthernetStartupDelay);
    Ethernet.begin( mac, ip);

    max7219udp.begin( max7219port );
    MSFSudp.begin( MSFSport );



    if (Reflector_In_Use == 1) {
      max7219udp.beginPacket(reflectorIP, reflectorport);
      max7219udp.println("Init Max7219, Nextron, Power Relay - " + strMyIP + " " + String(millis()) + "mS since reset.");
      max7219udp.endPacket();
    }
  }


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


  // Turn Everything on for 5 Seconds
  digitalWrite(STROBE_LIGHTS, HIGH);
  digitalWrite(NAVIGATION_LIGHTS, HIGH);
  digitalWrite(FORMATION_LIGHTS, HIGH);
  digitalWrite(BACK_LIGHTS, HIGH);
  digitalWrite(FLOOD_LIGHTS, HIGH);
  digitalWrite(NVG_LIGHTS, HIGH);
  AllOn();
  delay(2000);

  // Turn Everything off for 2 Seconds
  AllOff();
  digitalWrite(STROBE_LIGHTS, LOW);
  digitalWrite(NAVIGATION_LIGHTS, LOW);
  digitalWrite(FORMATION_LIGHTS, LOW);
  digitalWrite(BACK_LIGHTS, LOW);
  digitalWrite(FLOOD_LIGHTS, LOW);
  digitalWrite(NVG_LIGHTS, LOW);
  delay(1000);

  // Turn Everything on
  digitalWrite(STROBE_LIGHTS, HIGH);
  digitalWrite(NAVIGATION_LIGHTS, HIGH);
  digitalWrite(FORMATION_LIGHTS, HIGH);
  digitalWrite(BACK_LIGHTS, HIGH);
  digitalWrite(FLOOD_LIGHTS, HIGH);
  digitalWrite(NVG_LIGHTS, HIGH);
  SetBrightness(15);
  AllOn();


  // After Nextron Test is complete Dim all displays, then set back to default brightness and switch off

  nextion.begin(256000);

  //################## IFEI TEST SETUP ##################Y
  // COUNT DOWN -TEST- - 05 - 04 - 03 - 02 - 01 - GO
  delay(50); // delay test allow for NEXTION BOOT UP
  nextion.print("t0.txt=\"");
  nextion.print("\"");
  nextion.write("\xFF\xFF\xFF");
  nextion.print("t1.txt=\"");
  nextion.print("\"");
  nextion.write("\xFF\xFF\xFF");
  delay(100); // delay test allow for NEXTION BOOT UP
  nextion.print("t0.txt=\"");
  nextion.print("TE");
  nextion.print("\"");
  nextion.write("\xFF\xFF\xFF");

  nextion.print("t1.txt=\"");
  nextion.print("ST");
  nextion.print("\"");
  nextion.write("\xFF\xFF\xFF");
  delay (1000);
  nextion.print("t0.txt=\"");
  nextion.print("\"");
  nextion.write("\xFF\xFF\xFF");
  nextion.print("t1.txt=\"");
  nextion.print("05");
  nextion.print("\"");
  nextion.write("\xFF\xFF\xFF");
  delay (1000);
  nextion.print("t1.txt=\"");
  nextion.print("04");
  nextion.print("\"");
  nextion.write("\xFF\xFF\xFF");
  delay (1000);
  nextion.print("t1.txt=\"");
  nextion.print("03");
  nextion.print("\"");
  nextion.write("\xFF\xFF\xFF");
  delay (1000);
  nextion.print("t1.txt=\"");
  nextion.print("02");
  nextion.print("\"");
  nextion.write("\xFF\xFF\xFF");
  delay (1000);
  nextion.print("t1.txt=\"");
  nextion.print("01");
  nextion.print("\"");
  nextion.write("\xFF\xFF\xFF");
  delay (1000);
  nextion.print("t0.txt=\"");
  nextion.print("GO");
  nextion.print("\"");
  nextion.write("\xFF\xFF\xFF");

  nextion.print("t1.txt=\"");
  nextion.print("GO");
  nextion.print("\"");
  nextion.write("\xFF\xFF\xFF");
  delay (1000);
  nextion.print("t0.txt=\"");
  nextion.print("\"");
  nextion.write("\xFF\xFF\xFF");
  nextion.print("t1.txt=\"");
  nextion.print("\"");
  nextion.write("\xFF\xFF\xFF");


  // Slowly Dim the Leds
  for (int Local_Brightness = 15; Local_Brightness >= 0; Local_Brightness--) {
    analogWrite(FORMATION_LIGHTS, map(Local_Brightness, 0, 15, 0, 255));
    analogWrite(NAVIGATION_LIGHTS, map(Local_Brightness, 0, 15, 0, 255));
    analogWrite(NVG_LIGHTS, map(Local_Brightness, 0, 15, 0, 255));
    analogWrite(FLOOD_LIGHTS, map(Local_Brightness, 0, 15, 0, 255));
    analogWrite(BACK_LIGHTS, map(Local_Brightness, 0, 15, 0, 255));
    analogWrite(STROBE_LIGHTS, map(Local_Brightness, 0, 15, 0, 255));
    SetBrightness(Local_Brightness);

    delay(300);
  }

  // Turn off All Leds and set to mid brightness
  AllOff();
  digitalWrite(STROBE_LIGHTS, LOW);
  digitalWrite(NAVIGATION_LIGHTS, LOW);
  digitalWrite(FORMATION_LIGHTS, LOW);
  digitalWrite(BACK_LIGHTS, LOW);
  digitalWrite(FLOOD_LIGHTS, LOW);
  SetBrightness(8);

  DcsBios::setup();

  if (Reflector_In_Use == 1) {
    max7219udp.beginPacket(reflectorIP, reflectorport);
    max7219udp.println("Setup Complete Max7219, Nextron, Power Relay - " + strMyIP + " " + String(millis()) + "mS since reset.");
    max7219udp.endPacket();
  }
}

DcsBios::Switch2Pos masterModeAa("MASTER_MODE_AA", A2);
DcsBios::Switch2Pos masterModeAg("MASTER_MODE_AG", A1);
DcsBios::Switch2Pos fireExtBtn("FIRE_EXT_BTN", A0);
// DcsBios::ActionButton masterModeAaToggle("MASTER_MODE_AA", A2);
// DcsBios::ActionButton masterModeAgToggle("MASTER_MODE_AG", A1);

void loop() {
  DcsBios::loop();

  if (Ethernet_In_Use == 1) {


    max7219packetsize = max7219udp.parsePacket();
    max7219Len = max7219udp.read(max7219packetBuffer, 999);

    if (max7219Len > 0) {
      max7219packetBuffer[max7219Len] = 0;
    }
    if (max7219packetsize) {

      ProcessReceivedString();
    }

    MSFSpacketsize = MSFSudp.parsePacket();
    MSFSLen = MSFSudp.read(max7219packetBuffer, 999);

    if (MSFSLen > 0) {
      max7219packetBuffer[MSFSLen] = 0;
    }
    if (MSFSpacketsize) {

      ProcessReceivedMSFSString();
      SendReflectorMessage("Exiting MSFS Processing");
    }

  }


}
