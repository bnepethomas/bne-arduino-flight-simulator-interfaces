/*

  ////////////////////---||||||||||********||||||||||---\\\\\\\\\\\\\\\\\\\\
  //||                  FUNCTION = UIP COMBINED                        ||\\
  //||              LOCATION IN THE PIT = UIP                          ||\\
  //||            ARDUINO PROCESSOR TYPE = Arduino Mega 2560            ||\\
  //||      ARDUINO CHIP SERIAL NUMBER = SN -       ||\\
  //||                    CONNECTED COM PORT = COM 3                    ||\\
  //||            ****DO CHECK S/N BEFORE UPLOAD NEW DATA****           ||\\
  ////////////////////---||||||||||********||||||||||---\\\\\\\\\\\\\\\\\\\\

Todos

Hud Motor Controller - currently looking at delta between TMC2208 and TMC2209

  This Superceedes udp_input_controller
  Split from udp_input_controller_2 20200802

  Heavily based on
  https://github.com/calltherain/ArduinoUSBJoystick

  Interface for DCS BIOS

  Mega2560 R3,
  digital Pin 22~37 used as rows. 0-15, 16 Rows
  digital pin 38~48 used as columns. 0-10, 11 Columns

  it's a 16 * 11  matrix, due to the loss of pins/Columns used by the Ethernet and SD Card Shield, Digital I/O 50 through 53 are not available.
  Pin 49 is available but isn't used. This gives a total number of usable Inputs as 176 (remember numbering starts at 0 - so 0-175)

  The code pulls down a row and reads values from the Columns.
  Row 0 - Col 0 = Digit 0
  Row 0 - Col 10 = Digit 10
  Row 15 - Col 0 = Digit 165
  Row 15 = Col 10 = Digit 175

  So - Digit = Row * 11 + Col

*/


/*
Current AoA seems back to front to circuit diagram

Resolved Issues
Controller
Spare 1 is col 6
Spare 2 is col 7
R5 had CAP in its place remove 3 stray labels on silkscreen
Tie MS2 on both motor drivers to high so only half stepping is used (not 8th when MS1 low and MS2 low)
Issue - Right DDI Bright causes all pots to move - possibly impacting rail?
  Measure 8.5ohms rail short with leds dimming
  When plugged in - wiper is tied to +5V
  Root cause J14 Pins 5 and 6 mapped to Right DDI Bright, Pin 6 should be Right DDI contrast
  This issue validates approach of setting all pots to center position before conntecting for testing

Master ARM
Master Arm A/A A/G and Ready Discharge
Needed to rotate D5 and D14
Change indicator leds from series to parallel
Swap location of D7 and D9 so they are paired with same switch
Possible incorrect colours for Discharge upper should be orange - lower green


Code ToDo's - add check that DCS is active to enable stepper driver


*/
#define GREEN_STATUS_LED_PORT 14
#define RED_STATUS_LED_PORT 15  // RED LED is used for monitoring ethernet
#define FLASH_TIME 3000
#define SYNCH_BACKLIGHT_AT_START 1
#define STARTUP_BACKLIGHT_END 90000  //Keep Backlight on until all panels have completed testing
#define BACKLIGHT_END_LEVEL 50       // Percentage Backlight Level at end of startup

unsigned long NEXT_STATUS_TOGGLE_TIMER = 0;
bool RED_LED_STATE = false;
bool GREEN_LED_STATE = true;
unsigned long timeSinceRedLedChanged = 0;

#define Ethernet_In_Use 1
#define DCSBIOS_In_Use 1

int Reflector_In_Use = 1;

#define DCSBIOS_IRQ_SERIAL
#include "DcsBios.h"

// Forward declarations: the bounded AOA brightness handler is implemented
// alongside the other HUD analogue controls near the end of this sketch.
void UpdateAoaIndexerBrightness();
void MaintainAoaIndexerBrightness();


// Ethernet Related
#include <SPI.h>
#include <Ethernet.h>
#include <EthernetUdp.h>

// These local Mac and IP Address will be reassigned early in startup based on
// the device ID as set by address pins
byte mac[] = { 0xA8, 0x61, 0x0A, 0x9E, 0x83, 0x01 };
String sMac = "A8:61:0A:9E:83:01";
IPAddress ip(172, 16, 1, 101);
String strMyIP = "172.16.1.101";

// Raspberry Pi is Target
IPAddress reflectorIP(172, 16, 1, 10);
String strreflectorIP = "X.X.X.X";




const unsigned int localport = 7788;
const unsigned int localdebugport = 7795;
const unsigned int reflectorport = 27000;
const unsigned int aliveport = 13137;

int packetSize;
int debugLen;
EthernetUDP udp;
EthernetUDP debugUDP;

EthernetUDP aliveudp;  // Sends keepalives to monitoring application
const unsigned long aliveinterval = 10000;
long lastalivesent = 0;


#define EthernetStartupDelay 500
#define ES1_RESET_PIN 53

const unsigned long delayBeforeSendingPacket = 3000;
unsigned long ethernetStartTime = 0;
String BoardName = "UIP Combined Controller: ";

char packetBuffer[1000];     //buffer to store the incoming data
char outpacketBuffer[1000];  //buffer to store the outgoing data

void SendDebug(String MessageToSend) {
  MessageToSend = BoardName + MessageToSend;
  if ((Reflector_In_Use == 1) && (Ethernet_In_Use == 1)) {
    udp.beginPacket(reflectorIP, reflectorport);
    udp.print(MessageToSend);
    udp.endPacket();
  }
}

// ********************** Added Smoothing Filter for AOA Indexer Brightness
// Not used in UIP combined controller
// From https://github.com/jonnieZG/EWMA
#include <Ewma.h>

// ********************* End Smoothing Filter *************

// ########################## BEGIN STEPPERS ########################################
#include <AccelStepper.h>

// Currently the stepper forward reverse is mapped to Master ARM A/A A/G
bool HUD_STEPPER_ENABLED = false;
bool HUD_STEPPER_FORWARD = false;
bool HUD_STEPPER_REVERSE = false;

#define STEPPER_MAX_SPEED 90000
#define STEPPER_ACCELERATION 9000

#define AllstepperEnablePin 19

#define HUDstepPin 18
#define HUDdirectionPin 17


#define STEPS 10080
#define STEPS 315 * 16  // The 16 is the default divisors when no pins are tied together on the driver module \

AccelStepper HUDStepper(AccelStepper::DRIVER, HUDstepPin, HUDdirectionPin);

void updateSteppers() {
  HUDStepper.run();
}

// ########################### END STEPPERS #########################################


// ********************* Begin Spin Leds ******************

#define SPIN_LED_PIN 13

void SPIN_LED_ON() {
  digitalWrite(SPIN_LED_PIN, HIGH);
}

void SPIN_LED_OFF() {
  digitalWrite(SPIN_LED_PIN, LOW);
}

void onSpinLtChange(unsigned int newValue) {
  if (newValue == 1) {
    SPIN_LED_ON();
  } else {
    SPIN_LED_OFF();
  }
}
DcsBios::IntegerBuffer spinLtBuffer(0x742a, 0x0800, 11, onSpinLtChange);

// ********************* End SpinLeds ********************

// ********************** Begin Max7219 ******************

#include "LedControl.h"

/*
LOAD -> A9  -> D63
CLOCK -> A10 -> D64
DIN -> A11 -> D65
*/
//LedControl lc = LedControl(A11, A10, A9, 1);
LedControl lc = LedControl(65, 64, 63, 1);

void Max7219_ALL_ON() {
  SendDebug("Max7219 On");
  for (int row = 0; row < 8; row++) {
    for (int col = 0; col < 8; col++) {
      lc.setLed(0, row, col, true);
    }
  }
}

void Max7219_ALL_OFF() {
  SendDebug("Max7219 Off");
  for (int row = 0; row < 8; row++) {
    for (int col = 0; col < 8; col++) {
      lc.setLed(0, row, col, false);
    }
  }
}


void MASTER_ARM_DISCHARGE_READY(bool ledstate) {
  // Led location
  // Cathode -1, Anode
  lc.setLed(0, 4, 1, ledstate);
}

void onMcReadyChange(unsigned int newValue) {
  if (newValue == 1) {
    MASTER_ARM_DISCHARGE_READY(true);
  } else {
    MASTER_ARM_DISCHARGE_READY(false);
  }
}
DcsBios::IntegerBuffer mcReadyBuffer(0x740c, 0x8000, 15, onMcReadyChange);



void MASTER_ARM_DISCHARGE_DONE(bool ledstate) {
  // Led location
  // Cathode -1, Anode
  lc.setLed(0, 5, 1, ledstate);
}

void onMcDischChange(unsigned int newValue) {
  if (newValue == 1) {
    MASTER_ARM_DISCHARGE_DONE(true);
  } else {
    MASTER_ARM_DISCHARGE_DONE(false);
  }
}
DcsBios::IntegerBuffer mcDischBuffer(0x740c, 0x4000, 14, onMcDischChange);

void MASTER_ARM_AA(bool ledstate) {
  // Led location
  // Cathode -1, Anode
  lc.setLed(0, 4, 2, ledstate);
}

void onMasterModeAaLtChange(unsigned int newValue) {
  if (newValue == 1) {
    MASTER_ARM_AA(true);
  } else {
    MASTER_ARM_AA(false);
  }
}
DcsBios::IntegerBuffer masterModeAaLtBuffer(0x740c, 0x0200, 9, onMasterModeAaLtChange);

void MASTER_ARM_AG(bool ledstate) {
  // Led location
  // Cathode -1, Anode
  lc.setLed(0, 5, 2, ledstate);
}

void onMasterModeAgLtChange(unsigned int newValue) {
  if (newValue == 1) {
    MASTER_ARM_AG(true);
  } else {
    MASTER_ARM_AG(false);
  }
}
DcsBios::IntegerBuffer masterModeAgLtBuffer(0x740c, 0x0400, 10, onMasterModeAgLtChange);

void AOA_ABOVE(bool ledstate) {
  lc.setLed(0, 6, 5, ledstate);
}

void onAoaIndexerHighChange(unsigned int newValue) {
  if (newValue == 1) {
    AOA_ABOVE(true);
  } else {
    AOA_ABOVE(false);
  }
}
DcsBios::IntegerBuffer aoaIndexerHighBuffer(0x7408, 0x0008, 3, onAoaIndexerHighChange);


void AOA_ON(bool ledstate) {
  lc.setLed(0, 7, 5, ledstate);
}

void onAoaIndexerNormalChange(unsigned int newValue) {
  if (newValue == 1) {
    AOA_ON(true);
  } else {
    AOA_ON(false);
  }
}
DcsBios::IntegerBuffer aoaIndexerNormalBuffer(0x7408, 0x0010, 4, onAoaIndexerNormalChange);


void AOA_BELOW(bool ledstate) {
  lc.setLed(0, 7, 4, ledstate);
}
void onAoaIndexerLowChange(unsigned int newValue) {
  if (newValue == 1) {
    AOA_BELOW(true);
  } else {
    AOA_BELOW(false);
  }
}
DcsBios::IntegerBuffer aoaIndexerLowBuffer(0x7408, 0x0020, 5, onAoaIndexerLowChange);


void BIT_LED_A(bool ledstate) {
  lc.setLed(0, 4, 7, ledstate);
}


void BIT_LED_B(bool ledstate) {
  lc.setLed(0, 5, 7, ledstate);
}


void LOCKSHOOT_SHOOT(bool ledstate) {
  lc.setLed(0, 5, 6, ledstate);
  lc.setLed(0, 5, 5, ledstate);
}
void onLsShootChange(unsigned int newValue) {
  if (newValue == 1) {
    LOCKSHOOT_SHOOT(true);
  } else {
    LOCKSHOOT_SHOOT(false);
  }
}
DcsBios::IntegerBuffer lsShootBuffer(0x7408, 0x0002, 1, onLsShootChange);


void LOCKSHOOT_STROBE(bool ledstate) {
  lc.setLed(0, 4, 5, ledstate);
  lc.setLed(0, 5, 4, ledstate);
}
void onLsShootStrobeChange(unsigned int newValue) {
  if (newValue == 1) {
    LOCKSHOOT_STROBE(true);
  } else {
    LOCKSHOOT_STROBE(false);
  }
}
DcsBios::IntegerBuffer lsShootStrobeBuffer(0x7408, 0x0004, 2, onLsShootStrobeChange);


void LOCKSHOOT_LOCK(bool ledstate) {
  lc.setLed(0, 4, 6, ledstate);
  lc.setLed(0, 4, 6, ledstate);
}
void onLsLockChange(unsigned int newValue) {
  if (newValue == 1) {
    LOCKSHOOT_LOCK(true);
  } else {
    LOCKSHOOT_LOCK(false);
  }
}
DcsBios::IntegerBuffer lsLockBuffer(0x7408, 0x0001, 0, onLsLockChange);


void EWI_RCDR_ON(bool ledstate) {
  lc.setLed(0, 3, 1, ledstate);
}
void onRhAdvRcdrOnChange(unsigned int newValue) {
  if (newValue == 1) {
    EWI_RCDR_ON(true);
  } else {
    EWI_RCDR_ON(false);
  }
}
DcsBios::IntegerBuffer rhAdvRcdrOnBuffer(FA_18C_hornet_RH_ADV_RCDR_ON, onRhAdvRcdrOnChange);


void R_EWI_SPARE_1(bool ledstate) {
  lc.setLed(0, 3, 2, ledstate);
}
void onRhAdvSpareRh1Change(unsigned int newValue) {
  if (newValue == 1) {
    R_EWI_SPARE_1(true);
  } else {
    R_EWI_SPARE_1(false);
  }
}
DcsBios::IntegerBuffer rhAdvSpareRh1Buffer(FA_18C_hornet_RH_ADV_SPARE_RH1, onRhAdvSpareRh1Change);


void R_EWI_SPARE_2(bool ledstate) {
  lc.setLed(0, 3, 3, ledstate);
}
void onRhAdvSpareRh2Change(unsigned int newValue) {
  if (newValue == 1) {
    R_EWI_SPARE_2(true);
  } else {
    R_EWI_SPARE_2(false);
  }
}
DcsBios::IntegerBuffer rhAdvSpareRh2Buffer(FA_18C_hornet_RH_ADV_SPARE_RH2, onRhAdvSpareRh2Change);


void R_EWI_SPARE_3(bool ledstate) {
  lc.setLed(0, 3, 4, ledstate);
}
void onRhAdvSpareRh3Change(unsigned int newValue) {
  if (newValue == 1) {
    R_EWI_SPARE_3(true);
  } else {
    R_EWI_SPARE_3(false);
  }
}
DcsBios::IntegerBuffer rhAdvSpareRh3Buffer(FA_18C_hornet_RH_ADV_SPARE_RH3, onRhAdvSpareRh3Change);


void EWI_AI(bool ledstate) {
  lc.setLed(0, 3, 5, ledstate);
}
void onRhAdvAiChange(unsigned int newValue) {
  if (newValue == 1) {
    EWI_AI(true);
  } else {
    EWI_AI(false);
  }
}
DcsBios::IntegerBuffer rhAdvAiBuffer(FA_18C_hornet_RH_ADV_AI, onRhAdvAiChange);


void EWI_CW(bool ledstate) {
  lc.setLed(0, 3, 6, ledstate);
}
void onRhAdvCwChange(unsigned int newValue) {
  if (newValue == 1) {
    EWI_CW(true);
  } else {
    EWI_CW(false);
  }
}
DcsBios::IntegerBuffer rhAdvCwBuffer(FA_18C_hornet_RH_ADV_CW, onRhAdvCwChange);


void EWI_DISP(bool ledstate) {
  lc.setLed(0, 2, 1, ledstate);
}
void onRhAdvDispChange(unsigned int newValue) {
  if (newValue == 1) {
    EWI_DISP(true);
  } else {
    EWI_DISP(false);
  }
}
DcsBios::IntegerBuffer rhAdvDispBuffer(FA_18C_hornet_RH_ADV_DISP, onRhAdvDispChange);


void R_EWI_SPARE_4(bool ledstate) {
  lc.setLed(0, 2, 2, ledstate);
}
void onRhAdvSpareRh4Change(unsigned int newValue) {
  if (newValue == 1) {
    R_EWI_SPARE_4(true);
  } else {
    R_EWI_SPARE_4(false);
  }
}
DcsBios::IntegerBuffer rhAdvSpareRh4Buffer(FA_18C_hornet_RH_ADV_SPARE_RH4, onRhAdvSpareRh4Change);


void R_EWI_SPARE_5(bool ledstate) {
  lc.setLed(0, 2, 3, ledstate);
}
void onRhAdvSpareRh5Change(unsigned int newValue) {
  if (newValue == 1) {
    R_EWI_SPARE_5(true);
  } else {
    R_EWI_SPARE_5(false);
  }
}
DcsBios::IntegerBuffer rhAdvSpareRh5Buffer(FA_18C_hornet_RH_ADV_SPARE_RH5, onRhAdvSpareRh5Change);


void EWI_SAM(bool ledstate) {
  lc.setLed(0, 2, 4, ledstate);
}
void onRhAdvSamChange(unsigned int newValue) {
  if (newValue == 1) {
    EWI_SAM(true);
  } else {
    EWI_SAM(false);
  }
}
DcsBios::IntegerBuffer rhAdvSamBuffer(FA_18C_hornet_RH_ADV_SAM, onRhAdvSamChange);


void EWI_AAA(bool ledstate) {
  lc.setLed(0, 2, 5, ledstate);
}
void onRhAdvAaaChange(unsigned int newValue) {
  if (newValue == 1) {
    EWI_AAA(true);
  } else {
    EWI_AAA(false);
  }
}
DcsBios::IntegerBuffer rhAdvAaaBuffer(FA_18C_hornet_RH_ADV_AAA, onRhAdvAaaChange);


void EWI_RIGHT_FIRE(bool ledstate) {
  lc.setLed(0, 2, 6, ledstate);
}
void onFireRightLtChange(unsigned int newValue) {
  if (newValue == 1) {
    EWI_RIGHT_FIRE(true);
  } else {
    EWI_RIGHT_FIRE(false);
  }
}
DcsBios::IntegerBuffer fireRightLtBuffer(FA_18C_hornet_FIRE_RIGHT_LT, onFireRightLtChange);


void EWI_RIGHT_APU(bool ledstate) {
  lc.setLed(0, 2, 7, ledstate);
}
void onFireApuLtChange(unsigned int newValue) {
  if (newValue == 1) {
    EWI_RIGHT_APU(true);
  } else {
    EWI_RIGHT_APU(false);
  }
}
DcsBios::IntegerBuffer fireApuLtBuffer(FA_18C_hornet_FIRE_APU_LT, onFireApuLtChange);


void EWI_NOGO(bool ledstate) {
  lc.setLed(0, 1, 1, ledstate);
}
void onLhAdvNoGoChange(unsigned int newValue) {
  if (newValue == 1) {
    EWI_NOGO(true);
  } else {
    EWI_NOGO(false);
  }
}
DcsBios::IntegerBuffer lhAdvNoGoBuffer(FA_18C_hornet_LH_ADV_NO_GO, onLhAdvNoGoChange);


void EWI_R_BLEED(bool ledstate) {
  lc.setLed(0, 1, 2, ledstate);
}
void onLhAdvRBleedChange(unsigned int newValue) {
  if (newValue == 1) {
    EWI_R_BLEED(true);
  } else {
    EWI_R_BLEED(false);
  }
}
DcsBios::IntegerBuffer lhAdvRBleedBuffer(FA_18C_hornet_LH_ADV_R_BLEED, onLhAdvRBleedChange);


void EWI_STBY(bool ledstate) {
  lc.setLed(0, 1, 3, ledstate);
}
void onLhAdvStbyChange(unsigned int newValue) {
  if (newValue == 1) {
    EWI_STBY(true);
  } else {
    EWI_STBY(false);
  }
}
DcsBios::IntegerBuffer lhAdvStbyBuffer(FA_18C_hornet_LH_ADV_STBY, onLhAdvStbyChange);


void EWI_REC(bool ledstate) {
  lc.setLed(0, 1, 4, ledstate);
}
void onLhAdvRecChange(unsigned int newValue) {
  if (newValue == 1) {
    EWI_REC(true);
  } else {
    EWI_REC(false);
  }
}
DcsBios::IntegerBuffer lhAdvRecBuffer(FA_18C_hornet_LH_ADV_REC, onLhAdvRecChange);


void EWI_XMIT(bool ledstate) {
  lc.setLed(0, 1, 5, ledstate);
}
void onLhAdvXmitChange(unsigned int newValue) {
  if (newValue == 1) {
    EWI_XMIT(true);
  } else {
    EWI_XMIT(false);
  }
}
DcsBios::IntegerBuffer lhAdvXmitBuffer(FA_18C_hornet_LH_ADV_XMIT, onLhAdvXmitChange);


void EWI_ASPJ(bool ledstate) {
  lc.setLed(0, 1, 6, ledstate);
}
void onLhAdvAspjOhChange(unsigned int newValue) {
  if (newValue == 1) {
    EWI_ASPJ(true);
  } else {
    EWI_ASPJ(false);
  }
}
DcsBios::IntegerBuffer lhAdvAspjOhBuffer(FA_18C_hornet_LH_ADV_ASPJ_OH, onLhAdvAspjOhChange);


void EWI_GO(bool ledstate) {
  lc.setLed(0, 0, 1, ledstate);
}
void onLhAdvGoChange(unsigned int newValue) {
  if (newValue == 1) {
    EWI_GO(true);
  } else {
    EWI_GO(false);
  }
}
DcsBios::IntegerBuffer lhAdvGoBuffer(FA_18C_hornet_LH_ADV_GO, onLhAdvGoChange);


void EWI_L_BLEED(bool ledstate) {
  lc.setLed(0, 0, 2, ledstate);
}
void onLhAdvLBleedChange(unsigned int newValue) {
  if (newValue == 1) {
    EWI_L_BLEED(true);
  } else {
    EWI_L_BLEED(false);
  }
}
DcsBios::IntegerBuffer lhAdvLBleedBuffer(FA_18C_hornet_LH_ADV_L_BLEED, onLhAdvLBleedChange);


void EWI_SPD_BRK(bool ledstate) {
  lc.setLed(0, 0, 3, ledstate);
}
void onLhAdvSpdBrkChange(unsigned int newValue) {
  if (newValue == 1) {
    EWI_SPD_BRK(true);
  } else {
    EWI_SPD_BRK(false);
  }
}
DcsBios::IntegerBuffer lhAdvSpdBrkBuffer(FA_18C_hornet_LH_ADV_SPD_BRK, onLhAdvSpdBrkChange);


void EWI_L_BAR_RED(bool ledstate) {
  lc.setLed(0, 0, 4, ledstate);
}
void onLhAdvLBarRedChange(unsigned int newValue) {
  if (newValue == 1) {
    EWI_L_BAR_RED(true);
  } else {
    EWI_L_BAR_RED(false);
  }
}
DcsBios::IntegerBuffer lhAdvLBarRedBuffer(FA_18C_hornet_LH_ADV_L_BAR_RED, onLhAdvLBarRedChange);


void EWI_L_BAR_GREEN(bool ledstate) {
  lc.setLed(0, 0, 5, ledstate);
}
void onLhAdvLBarGreenChange(unsigned int newValue) {
  if (newValue == 1) {
    EWI_L_BAR_GREEN(true);
  } else {
    EWI_L_BAR_GREEN(false);
  }
}
DcsBios::IntegerBuffer lhAdvLBarGreenBuffer(FA_18C_hornet_LH_ADV_L_BAR_GREEN, onLhAdvLBarGreenChange);


void EWI_L_FIRE(bool ledstate) {
  lc.setLed(0, 0, 6, ledstate);
}
void onFireLeftLtChange(unsigned int newValue) {
  if (newValue == 1) {
    EWI_L_FIRE(true);
  } else {
    EWI_L_FIRE(false);
  }
}
DcsBios::IntegerBuffer fireLeftLtBuffer(FA_18C_hornet_FIRE_LEFT_LT, onFireLeftLtChange);


void EWI_L_CAUTION(bool ledstate) {
  lc.setLed(0, 0, 7, ledstate);
}
void onMasterCautionLtChange(unsigned int newValue) {
  if (newValue == 1) {
    EWI_L_CAUTION(true);
  } else {
    EWI_L_CAUTION(false);
  }
}
DcsBios::IntegerBuffer masterCautionLtBuffer(FA_18C_hornet_MASTER_CAUTION_LT, onMasterCautionLtChange);


void testled(bool ledstate) {
  int row = 2;
  int col = 7;
  SendDebug("Prod row :" + String(row) + " col :" + String(col));
  lc.setLed(0, row, col, ledstate);
}


// ********************* End Max7219 ********************

// ********************* Begin PWM  ********************

#define MAP_LIGHT 2
#define DDI_BACKLIGHT 3
#define COMPASS_BACKLIGHT 4
#define GENERAL_BACKLIGHT 5

void setPWMLights(int newValue) {
  analogWrite(MAP_LIGHT, newValue);
  analogWrite(DDI_BACKLIGHT, newValue);
  analogWrite(COMPASS_BACKLIGHT, newValue);
  analogWrite(GENERAL_BACKLIGHT, newValue);
}


void onConsoleIntLtChange(unsigned int newValue) {
  analogWrite(MAP_LIGHT, map(newValue, 0, 65535, 0, 255));
  analogWrite(DDI_BACKLIGHT, map(newValue, 0, 65535, 0, 255));
  analogWrite(COMPASS_BACKLIGHT, map(newValue, 0, 65535, 0, 255));
  analogWrite(GENERAL_BACKLIGHT, map(newValue, 0, 65535, 0, 255));
}
DcsBios::IntegerBuffer consoleIntLtBuffer(0x7558, 0xffff, 0, onConsoleIntLtChange);

void onFloodIntLtChange(unsigned int newValue) {
}
DcsBios::IntegerBuffer floodIntLtBuffer(0x755a, 0xffff, 0, onFloodIntLtChange);

void onNvgFloodIntLtChange(unsigned int newValue) {
}
DcsBios::IntegerBuffer nvgFloodIntLtBuffer(0x755c, 0xffff, 0, onNvgFloodIntLtChange);

// ********************* End PWM  ********************

// ********************* Begin Compass ***************
#define COILCP1 A12  // CP = COMPASS
#define COILCP2 A13
#define COILCP3 A14
#define COILCP4 A15

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

const long zeroTimeout = 300000;

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
  long nextInit1StatusUpdate = 0;

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
    if (initState == 0) {  // not initialized yet
      pinMode(irDetectorPin, INPUT);
      stepper.setMaxSpeed(stepperConfig.maxSpeed);
      stepper.setSpeed(400);

      initState = 1;
      zeroPosSearchStartTime = millis();
    }

    if (initState == 1) {
      // move off zero if already there so we always get movement on reset
      // (to verify that the stepper is working)
      // Needed to add delay between status messages as when PC is still starting this would
      //  impact timing
      if (millis() >= nextInit1StatusUpdate) {
        SendDebug(BoardName + "Compass initState 1");
        nextInit1StatusUpdate = millis() + 1000;
      }
      if (zeroDetected()) {
        stepper.runSpeed();
      } else {
        initState = 2;
        SendDebug(BoardName + "Compass initState 2");
      }
    }

    if (initState == 2) {  // zeroing
      if (!zeroDetected()) {
        // Currently this safety check isn't working
        // Add Ethernet card for more troubleshooting
        // Need to check IP addresses of PC secondary nic
        if (millis() >= (zeroTimeout + zeroPosSearchStartTime)) {
          stepper.disableOutputs();
          if (initState != 99) {
            // Send warning message only once
            SendDebug(BoardName + "Compass initState 99 - timeout finding zero point");
          }
          initState == 99;

        } else
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
        SendDebug(BoardName + "Compass initState 3 - normal running");
      }
    }
    if (initState == 3) {  // running normally

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

    if (initState == 99) {  // Timed out looking for zero do nothing
      stepper.disableOutputs();
    }
  }
};

/* modify below this line */

/* define stepper parameters
   multiple Vid60Stepper instances can share the same StepperConfig object */
struct StepperConfig stepperConfig = {
  720,  // maxSteps
  200,  // maxSpeed
  50    // acceleration
};


// define AccelStepper instance
AccelStepper stepper(AccelStepper::FULL4WIRE, A12, A13, A14, A15);

// define Vid60Stepper class that uses the AccelStepper instance defined in the line above
//           v-- arbitrary name
// Vid60Stepper alt100ftPointer(0x107e,          // address of stepper data
Vid60Stepper standbyCompass(0x0436,         // address of stepper data
                            stepper,        // name of AccelStepper instance
                            stepperConfig,  // StepperConfig struct instance
                            16,             // IR Detector Pin (must be LOW in zero position)
                            480,            // zero offset
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




// ********************* End Compass ***************

#define NUM_BUTTONS 256
#define BUTTONS_USED_ON_PCB 176
#define NUM_AXES 8  // 8 axes, X, Y, Z, etc
#define STATUS_LED_PORT 7
#define FLASH_TIME 300

//
struct joyReport_t {
  int button[NUM_BUTTONS];  // 1 Button per byte - was originally one bit per byte - but we have plenty of storage space
};

// Go through the man loop a number of times before sending data to the Sim
// This allows analog averages to be established and the DigitalButton array to be populated
// This has to more than simply the number of elements in the array as the array values are initialised
// to 0, so it actually takes more than 30 interations before the average it fully established
int LoopsBeforeSendingAllowed = 40;
bool SendingAllowed = false;

// Matrix reliability settings.
// UIP is predominantly momentary push buttons. Only known maintained switches
// receive the longer confirmation and one DCS-only resend.
const unsigned int MATRIX_ROWS = 16;
const unsigned int MATRIX_COLS = 11;  // Physical matrix columns: pins 38-48 only.
const unsigned int ROW_SETTLE_US = 80;
const unsigned long MOMENTARY_STABLE_MS = 20;
const unsigned long MAINTAINED_STABLE_MS = 50;
const bool SEND_INITIAL_MATRIX_STATES = false;  // Learn physical startup state; do not command DCS at boot.
const bool RESEND_MAINTAINED_CONFIRMED_STATE = true;
const unsigned long MAINTAINED_RESEND_MS = 100;

joyReport_t joyReport;
joyReport_t prevjoyReport;

// Retained for compatibility/diagnostics. The confirmed-state debounce below
// uses candidateButton[] and candidateChangedAt[] instead of a lockout timer.
unsigned long joyEndDebounce[NUM_BUTTONS];

int candidateButton[NUM_BUTTONS];
unsigned long candidateChangedAt[NUM_BUTTONS];
bool resendPending[NUM_BUTTONS];
int resendState[NUM_BUTTONS];
unsigned long resendDueAt[NUM_BUTTONS];

// The resend is deliberately DCS-only. Suppress only the duplicate debug
// line that it would otherwise create; normal debug remains untouched.
bool suppressMatrixResendDebug = false;

long prevLEDTransition = millis();
int cButtonID[16];
bool bFirstTime = true;

unsigned long currentMillis = 0;
unsigned long previousMillis = 0;

// Note Pin 4 and Pin 10 cannot be used for other purposes.
//Arduino communicates with both the W5500 and SD card using the SPI bus (through the ICSP header).
//This is on digital pins 10, 11, 12, and 13 on the Uno and pins 50, 51, and 52 on the Mega.
//On both boards, pin 10 is used to select the W5500 and pin 4 for the SD card. These pins cannot be used for general I/O.
//On the Mega, the hardware SS pin, 53, is not used to select either the W5500 or the SD card,
//but it must be kept as an output or the SPI interface won't work.

char stringind[5];
String outString;

bool SpinFollowupTask = false;  // Spin Button Cover
bool LFBCFollowupTask = false;  // Left Fire Button Cover
bool RFBCFollowupTask = false;  // Right Fire Button Cover
long timeSpinOn = 0;            // Spin Button Cover
long timeLFBCOn = 0;            // Left Fire Button Cover
long timeRFBCOn = 0;            // Right Fire Button Cover
const int ToggleSwitchCoverMoveTime = 500;


bool RWR_POWER_BUTTON_STATE = false;  // Used to latch the RWR Power Switch

void setup() {

  pinMode(GREEN_STATUS_LED_PORT, OUTPUT);
  pinMode(RED_STATUS_LED_PORT, OUTPUT);
  digitalWrite(GREEN_STATUS_LED_PORT, true);
  digitalWrite(RED_STATUS_LED_PORT, true);

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
      digitalWrite(LED_BUILTIN, false);
      delay(FLASH_TIME);
      digitalWrite(LED_BUILTIN, true);
    }

    SendDebug("Ethernet Started " + strMyIP + " " + sMac);

    aliveudp.begin(aliveport);
  }

  // Spin Leds
  pinMode(SPIN_LED_PIN, OUTPUT);
  SPIN_LED_ON();

  lc.shutdown(0, false);
  lc.setIntensity(0, 8);
  lc.clearDisplay(0);
  Max7219_ALL_OFF();
  delay(1000);
  Max7219_ALL_ON();
  setPWMLights(255);

  // 20251026 Comments out for testing
  if (false) {
    SendDebug("Start Cycling PWM");
    for (int i = 0; i < 254; i++) {
      setPWMLights(i);
      // SendDebug("PWM Level :" + String(i));
      delay(20);
    }
    for (int i = 254; i >= 0; i--) {
      setPWMLights(i);
      // SendDebug("PWM Level :" + String(i));
      delay(20);
    }
    SendDebug("End Cycling PWM");
    setPWMLights(128);
  }

  // for (int i = 0; i < 4; i++) {
  //   SendDebug("TestLed off");
  //   testled(false);
  //   delay(2000);
  //   SendDebug("TestLed on");
  //   testled(true);
  //   delay(2000);
  // }



  SendDebug("STEPPER INITIALISATION STARTED");
  pinMode(AllstepperEnablePin, OUTPUT);
  digitalWrite(AllstepperEnablePin, false);

  HUDStepper.setMaxSpeed(STEPPER_MAX_SPEED);
  HUDStepper.setAcceleration(STEPPER_ACCELERATION);





  // ################# Start HUD Startup #########################
  // SendDebug("Stepper movement on startup will be disabled once testing is complete");
  // SendDebug("Start HUD Stepper");

  // HUDStepper.runToNewPosition(-STEPS * 1.1);
  // HUDStepper.setCurrentPosition(0);

  // for (int i = 1; i <= 1; i++) {
  //   SendDebug("Loop :" + String(i));
  //   HUDStepper.runToNewPosition(STEPS);
  //   delay(200);
  //   HUDStepper.runToNewPosition(0);
  //   delay(200);
  // }

  // HUDStepper.setCurrentPosition(0);
  // SendDebug("End HUD Stepper");
  // ################# End HUD Startup #########################




  SendDebug("STEPPER INITIALISATION COMPLETE");


  // Set the output ports to output
  for (int portId = 22; portId < 38; portId++) {
    pinMode(portId, OUTPUT);
  }

  // Set the input ports to input - port 50-53 used by Ethernet Shield
  for (int portId = 38; portId < 50; portId++) {
    // Even though we've already got 10K external pullups
    pinMode(portId, INPUT_PULLUP);
  }

  // Initialise all matrix state arrays.
  for (int ind = 0; ind < NUM_BUTTONS; ind++) {
    joyReport.button[ind] = 0;
    prevjoyReport.button[ind] = 0;
    joyEndDebounce[ind] = 0;
    candidateButton[ind] = 0;
    candidateChangedAt[ind] = millis();
    resendPending[ind] = false;
    resendState[ind] = 0;
    resendDueAt[ind] = 0;
  }

  if (DCSBIOS_In_Use == 1) DcsBios::setup();

  // Send the first bounded AOA indexer brightness value immediately after
  // DCS-BIOS initialisation. The normal 100 ms refresh is maintained in loop().
  UpdateAoaIndexerBrightness();

  if (SYNCH_BACKLIGHT_AT_START == 1) {
    while (millis() <= STARTUP_BACKLIGHT_END) {
       if (DCSBIOS_In_Use == 1) DcsBios::loop();
    }
  }
  for (int i = 254; i >= BACKLIGHT_END_LEVEL; i--) {
    setPWMLights(i);
    // SendDebug("PWM Level :" + String(i));
    delay(20);
  }
  SPIN_LED_OFF();
 // setPWMLights(BACKLIGHT_END_LEVEL);
  Max7219_ALL_OFF();


  SendDebug("Setup Complete");
}

// UIP scope reduction (revision 2):
// AMPCD, UFC and IFEI are now hosted elsewhere in the pit. Their physical matrix
// addresses remain in this controller so the 16 x 11 matrix numbering and wiring are
// unchanged, but their DCS-BIOS command paths below are intentionally disabled.

bool IsDisabledInput(int ind) {
  // These physical input positions remain part of the scanned 16 x 11 matrix
  // so all remaining address assignments stay unchanged. They are intentionally
  // inert in this UIP revision: no DCS-BIOS command, reflector/debug line, or
  // maintained-state resend is produced for AMPCD, UFC or IFEI inputs.
  switch (ind) {
    // UFC: ILS, COMM pulls, keypad, option selects and function keys.
    case 32: case 37: case 48:
    case 59: case 60: case 61: case 62: case 63: case 64: case 65:
    case 70: case 71: case 72: case 73: case 74: case 75: case 76:
    case 81: case 82: case 83: case 84:
    case 92: case 93: case 94: case 95:
    case 103: case 104: case 105: case 106:
    case 114:

    // AMPCD: PB 1-20 and its five selector/brightness controls.
    case 55: case 56: case 57: case 58:
    case 66: case 67: case 68: case 69:
    case 77: case 78: case 79: case 80:
    case 88: case 89: case 90: case 91:
    case 99: case 100: case 101: case 102:
    case 110: case 111: case 112: case 113:
    case 121: case 122: case 123: case 124:
    case 132:

    // IFEI.
    case 143: case 144:
    case 154: case 155:
    case 165: case 166:
      return true;

    default:
      return false;
  }
}

bool IsMomentaryInput(int ind) {
  // UIP is mostly push buttons. This default grouping prevents a DCS resend
  // from turning a press into a second press.
  switch (ind) {
    // Left and right DDI push buttons.
    case 0: case 1: case 2: case 3:
    case 5: case 6: case 7: case 8:
    case 11: case 12: case 13: case 14:
    case 16: case 17: case 18: case 19:
    case 22: case 23: case 24: case 25:
    case 27: case 28: case 29: case 30:
    case 33: case 34: case 35: case 36:
    case 38: case 39: case 40: case 41:
    case 42: case 43:  // Left fire and Master Caution (validated mapping retained).
    case 44: case 45: case 46: case 47:
    case 49: case 50: case 51: case 52:
    case 53: case 54:  // Right fire and APU fire (validated mapping retained).

    // Other active momentary panel buttons.
    case 115: case 116: case 117:
    case 119: case 120:
    case 125:
    case 136: case 138:
    case 147:
    case 158:
    case 169:
    case 171: case 172: case 173: case 174: case 175:  // Select Jett push buttons.
      return true;

    default:
      return false;
  }
}

bool IsMaintainedInput(int ind) {
  // Only known maintained switches/toggles are eligible for the safe 100 ms
  // DCS-only resend. Unknown controls are deliberately treated as momentary.
  switch (ind) {
    // DDI brightness/contrast and directional selector switches.
    case 4: case 9: case 10: case 15: case 20: case 21: case 26: case 31:

    // ECM selector switches. AMPCD positions are intentionally excluded because
    // AMPCD commands have been removed from this controller.
    case 85: case 96: case 107: case 108:
    case 118: case 126: case 129: case 137: case 148:
    case 159: case 170:

    // HUD, master arm, mode and other maintained controls.
    case 139:
    case 140: case 141: case 142:
    case 146:
    case 150: case 151: case 152: case 153:
    case 156: case 157:
    case 161: case 162: case 164:
    case 167: case 168:
      return true;

    default:
      return false;
  }
}

unsigned long StableTimeForInput(int ind) {
  return IsMaintainedInput(ind) ? MAINTAINED_STABLE_MS : MOMENTARY_STABLE_MS;
}

int ReadMatrixColumn(unsigned int colid) {
  switch (colid) {
    case 0:  return (PIND & B10000000) == 0 ? 0 : 1; // pin 38, PD7
    case 1:  return (PING & B00000100) == 0 ? 0 : 1; // pin 39, PG2
    case 2:  return (PING & B00000010) == 0 ? 0 : 1; // pin 40, PG1
    case 3:  return (PING & B00000001) == 0 ? 0 : 1; // pin 41, PG0
    case 4:  return (PINL & B10000000) == 0 ? 0 : 1; // pin 42, PL7
    case 5:  return (PINL & B01000000) == 0 ? 0 : 1; // pin 43, PL6
    case 6:  return (PINL & B00100000) == 0 ? 0 : 1; // pin 44, PL5
    case 7:  return (PINL & B00010000) == 0 ? 0 : 1; // pin 45, PL4
    case 8:  return (PINL & B00001000) == 0 ? 0 : 1; // pin 46, PL3
    case 9:  return (PINL & B00000100) == 0 ? 0 : 1; // pin 47, PL2
    case 10: return (PINL & B00000010) == 0 ? 0 : 1; // pin 48, PL1
    default: return 1;
  }
}

void AllMatrixRowsOff() {
  PORTA = 0xFF;
  PORTC = 0xFF;
}

void SelectMatrixRow(unsigned int rowid) {
  AllMatrixRowsOff();

  if (rowid < 8) {
    PORTA = ~(0x1 << rowid);
  } else {
    PORTC = ~(0x1 << (15 - rowid));
  }
}

void ScanMatrix() {
  for (unsigned int rowid = 0; rowid < MATRIX_ROWS; rowid++) {
    updateSteppers();

    SelectMatrixRow(rowid);
    delayMicroseconds(ROW_SETTLE_US);

    for (unsigned int colid = 0; colid < MATRIX_COLS; colid++) {
      int rawColumn = ReadMatrixColumn(colid);
      int inputIndex = (rowid * MATRIX_COLS) + colid;

      // Matrix inputs are active LOW due to INPUT_PULLUP.
      joyReport.button[inputIndex] = (rawColumn == 0) ? 1 : 0;
    }
  }

  AllMatrixRowsOff();
}

void FindInputChanges() {
  unsigned long now = millis();

  // First complete scan: learn all current physical positions without
  // generating startup DCS, Ethernet or debug traffic.
  if (bFirstTime) {
    for (int ind = 0; ind < BUTTONS_USED_ON_PCB; ind++) {
      prevjoyReport.button[ind] = joyReport.button[ind];
      candidateButton[ind] = joyReport.button[ind];
      candidateChangedAt[ind] = now;
      resendPending[ind] = false;

      if (SEND_INITIAL_MATRIX_STATES && DCSBIOS_In_Use == 1) {
        CreateDcsBiosMessage(ind, prevjoyReport.button[ind]);
      }
    }

    bFirstTime = false;
    updateSteppers();
    return;
  }

  for (int ind = 0; ind < BUTTONS_USED_ON_PCB; ind++) {
    int rawState = joyReport.button[ind];

    // Retired AMPCD/UFC/IFEI positions remain physically scanned so the matrix
    // index map is unchanged, but are fully silent at the application layer.
    if (IsDisabledInput(ind)) {
      prevjoyReport.button[ind] = rawState;
      candidateButton[ind] = rawState;
      candidateChangedAt[ind] = now;
      resendPending[ind] = false;
      continue;
    }

    // Any raw movement starts/restarts the stability timer.
    if (rawState != candidateButton[ind]) {
      candidateButton[ind] = rawState;
      candidateChangedAt[ind] = now;
      resendPending[ind] = false;
      continue;
    }

    // Only accept a new physical state once it has remained stable long enough.
    if ((candidateButton[ind] != prevjoyReport.button[ind]) &&
        ((now - candidateChangedAt[ind]) >= StableTimeForInput(ind))) {
      int confirmedState = candidateButton[ind];
      prevjoyReport.button[ind] = confirmedState;
      joyEndDebounce[ind] = now + StableTimeForInput(ind);

      sprintf(stringind, "%03d", ind);
      outString = outString + String(confirmedState);

      if (DCSBIOS_In_Use == 1) {
        CreateDcsBiosMessage(ind, confirmedState);
      }

      SendDebug("Front Input - " + String(ind) + ":" + String(confirmedState));

      // Only known maintained controls get one DCS-only confirmation resend.
      if (IsMaintainedInput(ind) &&
          RESEND_MAINTAINED_CONFIRMED_STATE &&
          DCSBIOS_In_Use == 1) {
        resendPending[ind] = true;
        resendState[ind] = confirmedState;
        resendDueAt[ind] = now + MAINTAINED_RESEND_MS;
      }
    }

    // DCS-only resend. It deliberately bypasses duplicate debug output; all
    // original normal command, IP and debug behaviour remains as before.
    if (resendPending[ind] &&
        now >= resendDueAt[ind] &&
        prevjoyReport.button[ind] == resendState[ind]) {
      suppressMatrixResendDebug = true;
      CreateDcsBiosMessage(ind, resendState[ind]);
      suppressMatrixResendDebug = false;
      resendPending[ind] = false;
    }
  }

  updateSteppers();
}





void sendToDcsBiosMessage(const char* msg, const char* arg) {
  // Normal debug behaviour is unchanged. Suppress only the intentional
  // DCS-only resend of a confirmed maintained switch state.
  if (!suppressMatrixResendDebug) {
    SendDebug("Front Input - " + String(msg) + ":" + String(arg));
  }
  sendDcsBiosMessage(msg, arg);
}


void CreateDcsBiosMessage(int ind, int state) {

  switch (state) {
    case 0:
      switch (ind) {
        case 0:
          sendToDcsBiosMessage("LEFT_DDI_PB_05", "0");
          break;
        case 1:
          sendToDcsBiosMessage("LEFT_DDI_PB_20", "0");
          break;
        case 2:
          sendToDcsBiosMessage("LEFT_DDI_PB_15", "0");
          break;
        case 3:
          sendToDcsBiosMessage("LEFT_DDI_PB_10", "0");
          break;
        case 4:  // USED BELOW
          break;
        case 5:
          sendToDcsBiosMessage("RIGHT_DDI_PB_05", "0");
          break;
        case 6:
          sendToDcsBiosMessage("RIGHT_DDI_PB_20", "0");
          break;
        case 7:
          sendToDcsBiosMessage("RIGHT_DDI_PB_15", "0");
          break;
        case 8:
          sendToDcsBiosMessage("RIGHT_DDI_PB_10", "0");
          break;
        case 9:  // USED BELOW
          break;
        case 10:
          sendToDcsBiosMessage("LEFT_DDI_CRS_SW", "1");
          break;
        case 11:
          sendToDcsBiosMessage("LEFT_DDI_PB_04", "0");
          break;
        case 12:
          sendToDcsBiosMessage("LEFT_DDI_PB_19", "0");
          break;
        case 13:
          sendToDcsBiosMessage("LEFT_DDI_PB_14", "0");
          break;
        case 14:
          sendToDcsBiosMessage("LEFT_DDI_PB_09", "0");
          break;
        case 15:  // USED BELOW
          break;
        case 16:
          sendToDcsBiosMessage("RIGHT_DDI_PB_04", "0");
          break;
        case 17:
          sendToDcsBiosMessage("RIGHT_DDI_PB_19", "0");
          break;
        case 18:
          sendToDcsBiosMessage("RIGHT_DDI_PB_14", "0");
          break;
        case 19:
          sendToDcsBiosMessage("RIGHT_DDI_PB_09", "0");
          break;
        case 20:  // USED BELOW
          break;
        case 21:
          sendToDcsBiosMessage("LEFT_DDI_CRS_SW", "1");
          break;
        case 22:
          sendToDcsBiosMessage("LEFT_DDI_PB_03", "0");
          break;
        case 23:
          sendToDcsBiosMessage("LEFT_DDI_PB_18", "0");
          break;
        case 24:
          sendToDcsBiosMessage("LEFT_DDI_PB_13", "0");
          break;
        case 25:
          sendToDcsBiosMessage("LEFT_DDI_PB_08", "0");
          break;
        case 26:  // USED BELOW
          break;
        case 27:
          sendToDcsBiosMessage("RIGHT_DDI_PB_03", "0");
          break;
        case 28:
          sendToDcsBiosMessage("RIGHT_DDI_PB_18", "0");
          break;
        case 29:
          sendToDcsBiosMessage("RIGHT_DDI_PB_13", "0");
          break;
        case 30:
          sendToDcsBiosMessage("RIGHT_DDI_PB_08", "0");
          break;
        case 31:  // USED BELOW
          break;
        case 32:
          // UIP removed - sendToDcsBiosMessage("UFC_ILS", "0");
          break;
        case 33:
          sendToDcsBiosMessage("LEFT_DDI_PB_02", "0");
          break;
        case 34:
          sendToDcsBiosMessage("LEFT_DDI_PB_17", "0");
          break;
        case 35:
          sendToDcsBiosMessage("LEFT_DDI_PB_12", "0");
          break;
        case 36:
          sendToDcsBiosMessage("LEFT_DDI_PB_07", "0");
          break;
        case 37:
          // UIP removed - sendToDcsBiosMessage("UFC_COMM1_PULL", "0");
          //sendToDcsBiosMessage("UFC_COMM2_PULL", "0");
          break;
        case 38:
          sendToDcsBiosMessage("RIGHT_DDI_PB_02", "0");
          break;
        case 39:
          sendToDcsBiosMessage("RIGHT_DDI_PB_17", "0");
          break;
        case 40:
          sendToDcsBiosMessage("RIGHT_DDI_PB_12", "0");
          break;
        case 41:
          sendToDcsBiosMessage("RIGHT_DDI_PB_07", "0");
          break;
        case 42:
          sendToDcsBiosMessage("LEFT_FIRE_BTN", "0");
          sendToDcsBiosMessage("LEFT_FIRE_BTN_COVER", "0");
          break;
        case 43:
          sendToDcsBiosMessage("MASTER_CAUTION_RESET_SW", "0");
          break;
        case 44:
          sendToDcsBiosMessage("LEFT_DDI_PB_01", "0");
          break;
        case 45:
          sendToDcsBiosMessage("LEFT_DDI_PB_16", "0");
          break;
        case 46:
          sendToDcsBiosMessage("LEFT_DDI_PB_11", "0");
          break;
        case 47:
          sendToDcsBiosMessage("LEFT_DDI_PB_06", "0");
          break;
        case 48:
          // UIP removed - sendToDcsBiosMessage("UFC_COMM2_PULL", "0");
          break;
        case 49:
          sendToDcsBiosMessage("RIGHT_DDI_PB_01", "0");
          break;
        case 50:
          sendToDcsBiosMessage("RIGHT_DDI_PB_16", "0");
          break;
        case 51:
          sendToDcsBiosMessage("RIGHT_DDI_PB_11", "0");
          break;
        case 52:
          sendToDcsBiosMessage("RIGHT_DDI_PB_06", "0");
          break;
        case 53:
          sendToDcsBiosMessage("RIGHT_FIRE_BTN", "0");
          sendToDcsBiosMessage("RIGHT_FIRE_BTN_COVER", "0");
          break;
        case 54:
          sendToDcsBiosMessage("APU_FIRE_BTN", "0");
          break;
        case 55:
          // UIP removed - sendToDcsBiosMessage("AMPCD_PB_05", "0");
          break;
        case 56:
          // UIP removed - sendToDcsBiosMessage("AMPCD_PB_20", "0");
          break;
        case 57:
          // UIP removed - sendToDcsBiosMessage("AMPCD_PB_15", "0");
          break;
        case 58:
          // UIP removed - sendToDcsBiosMessage("AMPCD_PB_10", "0");
          break;
        case 59:
          // UIP removed - sendToDcsBiosMessage("UFC_1", "0");
          break;
        case 60:
          // UIP removed - sendToDcsBiosMessage("UFC_2", "0");
          break;
        case 61:
          // UIP removed - sendToDcsBiosMessage("UFC_3", "0");
          break;
        case 62:
          // UIP removed - sendToDcsBiosMessage("UFC_OS2", "0");
          break;
        case 63:
          // UIP removed - sendToDcsBiosMessage("UFC_AP", "0");
          break;
        case 64:
          // UIP removed - sendToDcsBiosMessage("UFC_IFF", "0");
          break;
        case 65:
          // UIP removed - sendToDcsBiosMessage("UFC_TCN", "0");
          break;
        case 66:
          // UIP removed - sendToDcsBiosMessage("AMPCD_PB_04", "0");
          break;
        case 67:
          // UIP removed - sendToDcsBiosMessage("AMPCD_PB_19", "0");
          break;
        case 68:
          // UIP removed - sendToDcsBiosMessage("AMPCD_PB_14", "0");
          break;
        case 69:
          // UIP removed - sendToDcsBiosMessage("AMPCD_PB_09", "0");
          break;
        case 70:
          // UIP removed - sendToDcsBiosMessage("UFC_4", "0");
          break;
        case 71:
          // UIP removed - sendToDcsBiosMessage("UFC_5", "0");
          break;
        case 72:
          // UIP removed - sendToDcsBiosMessage("UFC_6", "0");
          break;
        case 73:
          // UIP removed - sendToDcsBiosMessage("UFC_OS3", "0");
          break;
        case 74:
          // UIP removed - sendToDcsBiosMessage("UFC_ONOFF", "0");
          break;
        case 75:
          // UIP removed - sendToDcsBiosMessage("UFC_BCN", "0");
          break;
        case 76:
          // UIP removed - sendToDcsBiosMessage("UFC_DL", "0");
          break;
        case 77:
          // UIP removed - sendToDcsBiosMessage("AMPCD_PB_03", "0");
          break;
        case 78:
          // UIP removed - sendToDcsBiosMessage("AMPCD_PB_18", "0");
          break;
        case 79:
          // UIP removed - sendToDcsBiosMessage("AMPCD_PB_13", "0");
          break;
        case 80:
          // UIP removed - sendToDcsBiosMessage("AMPCD_PB_08", "0");
          break;
        case 81:
          // UIP removed - sendToDcsBiosMessage("UFC_7", "0");
          break;
        case 82:
          // UIP removed - sendToDcsBiosMessage("UFC_8", "0");
          break;
        case 83:
          // UIP removed - sendToDcsBiosMessage("UFC_9", "0");
          break;
        case 84:
          // UIP removed - sendToDcsBiosMessage("UFC_OS4", "0");
          break;
        case 85:  // USED BELOW
          break;
        case 86:
          sendToDcsBiosMessage("CMSD_DISPENSE_SW", "1");
          break;
        case 87:  // EMC, IS THIS USED
          break;
        case 88:
          // UIP removed - sendToDcsBiosMessage("AMPCD_PB_02", "0");
          break;
        case 89:
          // UIP removed - sendToDcsBiosMessage("AMPCD_PB_17", "0");
          break;
        case 90:
          // UIP removed - sendToDcsBiosMessage("AMPCD_PB_12", "0");
          break;
        case 91:
          // UIP removed - sendToDcsBiosMessage("AMPCD_PB_07", "0");
          break;
        case 92:
          // UIP removed - sendToDcsBiosMessage("UFC_CLR", "0");
          break;
        case 93:
          // UIP removed - sendToDcsBiosMessage("UFC_0", "0");
          break;
        case 94:
          // UIP removed - sendToDcsBiosMessage("UFC_ENT", "0");
          break;
        case 95:
          // UIP removed - sendToDcsBiosMessage("UFC_OS5", "0");
          break;
        case 96:  // USED BELOW
          break;
        case 97:
          sendToDcsBiosMessage("CMSD_DISPENSE_SW", "1");
          break;
        case 98:  // EMC, IS THIS USED
          break;
        case 99:
          // UIP removed - sendToDcsBiosMessage("AMPCD_PB_01", "0");
          break;
        case 100:
          // UIP removed - sendToDcsBiosMessage("AMPCD_PB_16", "0");
          break;
        case 101:
          // UIP removed - sendToDcsBiosMessage("AMPCD_PB_11", "0");
          break;
        case 102:
          // UIP removed - sendToDcsBiosMessage("AMPCD_PB_06", "0");
          break;
        case 103:
          // UIP removed - sendToDcsBiosMessage("UFC_ADF", "1");
          break;
        case 104:
          // UIP removed - sendToDcsBiosMessage("UFC_IP", "0");
          break;
        case 105:
          // UIP removed - sendToDcsBiosMessage("UFC_OS1", "0");
          break;
        case 106:
          // UIP removed - sendToDcsBiosMessage("UFC_EMCON", "0");
          break;
        case 107:  // USED BELOW
          break;
        case 108:
          sendToDcsBiosMessage("AUX_REL_SW", "1");  // WIRED BACKWARDS IN MY PIT
          break;
        case 109:
          sendToDcsBiosMessage("SAI_TEST_BTN", "0");
          break;
        case 110:
          // UIP removed - sendToDcsBiosMessage("AMPCD_GAIN_SW", "1");
          break;
        case 111:
          // UIP removed - sendToDcsBiosMessage("AMPCD_CONT_SW", "1");
          break;
        case 112:
          // UIP removed - sendToDcsBiosMessage("AMPCD_SYM_SW", "1");
          break;
        case 113:
          // UIP removed - sendToDcsBiosMessage("AMPCD_NIGHT_DAY", "1");
          break;
        case 114:
          // UIP removed - sendToDcsBiosMessage("UFC_ADF", "1");
          break;
        case 115:
          sendToDcsBiosMessage("HUD_VIDEO_BIT", "0");
          break;
        case 116:  //FA-18C_hornet/MASTER_MODE_AA
          sendToDcsBiosMessage("MASTER_MODE_AA", "0");
          // HUD_STEPPER_FORWARD = false;
          break;
        case 117:  //FA-18C_hornet/FIRE_EXT_BTN
          sendToDcsBiosMessage("FIRE_EXT_BTN", "0");
          break;
        case 118:  // USED BELOW
          break;
        case 119:  //EMC, IS THIS USED
          sendToDcsBiosMessage("CMSD_JET_SEL_BTN", "0");
          break;
        case 120:
          sendToDcsBiosMessage("SAI_CAGE", "0");
          break;
        case 121:
          // UIP removed - sendToDcsBiosMessage("AMPCD_GAIN_SW", "1");
          break;
        case 122:
          // UIP removed - sendToDcsBiosMessage("AMPCD_CONT_SW", "1");
          break;
        case 123:
          // UIP removed - sendToDcsBiosMessage("AMPCD_SYM_SW", "1");
          break;
        case 124:
          // UIP removed - sendToDcsBiosMessage("AMPCD_NIGHT_DAY", "1");
          break;
        case 125:
          sendToDcsBiosMessage("RWR_BIT_BTN", "0");
          break;
        case 126:  // USED BELOW
          break;
        case 127:  //FA-18C_hornet/MASTER_MODE_AG
          sendToDcsBiosMessage("MASTER_MODE_AG", "0");
          // HUD_STEPPER_REVERSE = false;
          break;
        case 128:

          // USED BELOW
          break;
        case 129:  // USED BELOW
          break;
        case 130:  //EMC, IS THIS USED
          break;
        case 131:  // NOT USED
          break;
        case 132:
          // UIP removed - sendToDcsBiosMessage("AMPCD_BRT_CTL", "1");
          break;
        case 133:  // AMPCD, IS THIS USED
          break;
        case 134:  // AMPCD, IS THIS USED
          break;
        case 135:  // AMPCD, IS THIS USED
          break;
        case 136:
          sendToDcsBiosMessage("RWR_OFFSET_BTN", "0");
          break;
        case 137:  // USED BELOW
          break;
        case 138:
          sendToDcsBiosMessage("EMER_JETT_BTN", "0");
          break;
        case 139:
          sendToDcsBiosMessage("MASTER_ARM_SW", "0");
          break;
        case 140:                                   //FA-18C_hornet/HUD_ATT_SW
          sendToDcsBiosMessage("HUD_ATT_SW", "1");  //1 FOR OFF
          break;
        case 141:                                             //FA-18C_hornet/HUD_VIDEO_CONTROL_SW
          sendToDcsBiosMessage("HUD_VIDEO_CONTROL_SW", "1");  //1 FOR OFF
          break;
        case 142:                                       //FA-18C_hornet/HUD_SYM_REJ_SW
          sendToDcsBiosMessage("HUD_SYM_REJ_SW", "1");  //1 FOR OFF
          break;
        case 143:  //FA-18C_hornet/IFEI_MODE_BTN
          // UIP removed - sendToDcsBiosMessage("IFEI_MODE_BTN", "0");
          break;
        case 144:  //FA-18C_hornet/IFEI_DWN_BTN
          // UIP removed - sendToDcsBiosMessage("IFEI_DWN_BTN", "0");
          break;
        case 145:  //IFEI, IS THIS USED
          break;
        case 146:                                         //FA-18C_hornet/MODE_SELECTOR_SW
          sendToDcsBiosMessage("MODE_SELECTOR_SW", "1");  //1 FOR OFF
          break;
        case 147:
          sendToDcsBiosMessage("RWR_SPECIAL_BTN", "0");
          break;
        case 148:  // USED BELOW
          break;
        case 149:  //FA-18C_hornet/SPIN_RECOVERY_SW

          sendToDcsBiosMessage("SPIN_RECOVERY_COVER", "1");
          SpinFollowupTask = true;
          timeSpinOn = millis() + ToggleSwitchCoverMoveTime;
          //FA-18C_hornet/SPIN_RECOVERY_COVER


          break;
        case 150:  //FA-18C_hornet/IR_COOL_SW
          sendToDcsBiosMessage("IR_COOL_SW", "1");
          break;
        case 151:                                   //FA-18C_hornet/HUD_ATT_SW
          sendToDcsBiosMessage("HUD_ATT_SW", "1");  //1 FOR OFF
          break;
        case 152:                                             //FA-18C_hornet/HUD_VIDEO_CONTROL_SW
          sendToDcsBiosMessage("HUD_VIDEO_CONTROL_SW", "1");  //1 FOR OFF
          break;
        case 153:                                       //FA-18C_hornet/HUD_SYM_REJ_SW
          sendToDcsBiosMessage("HUD_SYM_REJ_SW", "1");  //1 FOR OFF
          break;
        case 154:  //FA-18C_hornet/IFEI_QTY_BTN
          // UIP removed - sendToDcsBiosMessage("IFEI_QTY_BTN", "0");
          break;
        case 155:  //FA-18C_hornet/IFEI_ZONE_BTN
          // UIP removed - sendToDcsBiosMessage("IFEI_ZONE_BTN", "0");
          break;
        case 156:                                             //FA-18C_hornet/SELECT_HMD_LDDI_RDDI
          sendToDcsBiosMessage("SELECT_HMD_LDDI_RDDI", "0");  // NEEDS WORK
          break;
        case 157:                                         //FA-18C_hornet/MODE_SELECTOR_SW
          sendToDcsBiosMessage("MODE_SELECTOR_SW", "1");  //1 FOR OFF
          break;
        case 158:
          sendToDcsBiosMessage("RWR_DISPLAY_BTN", "0");
          break;
        case 159:  // USED BELOW
          break;
        case 160:  //SPIN, IS THIS USED
          break;
        case 161:  ////FA-18C_hornet/IR_COOL_SW
          sendToDcsBiosMessage("IR_COOL_SW", "1");
          break;
        case 162:  //FA-18C_hornet/HUD_ALT_SW
          sendToDcsBiosMessage("HUD_ALT_SW", "0");
          break;
        case 163:  //HUD, IS THIS USED
          break;
        case 164:  //FA-18C_hornet/HUD_SYM_BRT_SELECT
          sendToDcsBiosMessage("HUD_SYM_BRT_SELECT", "0");
          break;
        case 165:  //FA-18C_hornet/IFEI_UP_BTN
          // UIP removed - sendToDcsBiosMessage("IFEI_UP_BTN", "0");
          break;
        case 166:  //FA-18C_hornet/IFEI_ET_BTN
          // UIP removed - sendToDcsBiosMessage("IFEI_ET_BTN", "0");
          break;
        case 167:
          sendToDcsBiosMessage("LEFT_DDI_HDG_SW", "1");
          break;
        case 168:
          sendToDcsBiosMessage("LEFT_DDI_HDG_SW", "1");
          break;
        case 169:
          // ######### PETE TO ADD LATCH #########
          break;
        case 170:  // USED BELOW
          break;
        case 171:  //FA-18C_hornet/SJ_CTR
          sendToDcsBiosMessage("SJ_CTR", "0");
          break;
        case 172:  //FA-18C_hornet/SJ_RI
          sendToDcsBiosMessage("SJ_RI", "0");
          break;
        case 173:  //FA-18C_hornet/SJ_LI
          sendToDcsBiosMessage("SJ_LI", "0");
          break;
        case 174:  //FA-18C_hornet/SJ_RO
          sendToDcsBiosMessage("SJ_RO", "0");
          break;
        case 175:  //FA-18C_hornet/SJ_LO
          sendToDcsBiosMessage("SJ_LO", "0");
          break;
        case 176:  // NOT USED
          break;
        case 177:  // NOT USED
          break;
        case 178:  // NOT USED
          break;
        case 179:  // NOT USED
          break;
        default:
          sendToDcsBiosMessage("LIGHTS_TEST_SW", "0");
          break;
      }
      break;
    case 1:
      switch (ind) {
        case 0:
          sendToDcsBiosMessage("LEFT_DDI_PB_05", "1");
          break;
        case 1:
          sendToDcsBiosMessage("LEFT_DDI_PB_20", "1");
          break;
        case 2:
          sendToDcsBiosMessage("LEFT_DDI_PB_15", "1");
          break;
        case 3:
          sendToDcsBiosMessage("LEFT_DDI_PB_10", "1");
          break;
        case 4:
          sendToDcsBiosMessage("LEFT_DDI_BRT_SELECT", "0");
          break;
        case 5:
          sendToDcsBiosMessage("RIGHT_DDI_PB_05", "1");
          break;
        case 6:
          sendToDcsBiosMessage("RIGHT_DDI_PB_20", "1");
          break;
        case 7:
          sendToDcsBiosMessage("RIGHT_DDI_PB_15", "1");
          break;
        case 8:
          sendToDcsBiosMessage("RIGHT_DDI_PB_10", "1");
          break;
        case 9:
          sendToDcsBiosMessage("RIGHT_DDI_BRT_SELECT", "0");
          break;
        case 10:
          sendToDcsBiosMessage("LEFT_DDI_CRS_SW", "2");
          break;
        case 11:
          sendToDcsBiosMessage("LEFT_DDI_PB_04", "1");
          break;
        case 12:
          sendToDcsBiosMessage("LEFT_DDI_PB_19", "1");
          break;
        case 13:
          sendToDcsBiosMessage("LEFT_DDI_PB_14", "1");
          break;
        case 14:
          sendToDcsBiosMessage("LEFT_DDI_PB_09", "1");
          break;
        case 15:
          sendToDcsBiosMessage("LEFT_DDI_BRT_SELECT", "1");
          break;
        case 16:
          sendToDcsBiosMessage("RIGHT_DDI_PB_04", "1");
          break;
        case 17:
          sendToDcsBiosMessage("RIGHT_DDI_PB_19", "1");
          break;
        case 18:
          sendToDcsBiosMessage("RIGHT_DDI_PB_14", "1");
          break;
        case 19:
          sendToDcsBiosMessage("RIGHT_DDI_PB_09", "1");
          break;
        case 20:
          sendToDcsBiosMessage("RIGHT_DDI_BRT_SELECT", "1");
          break;
        case 21:
          sendToDcsBiosMessage("LEFT_DDI_CRS_SW", "0");
          break;
        case 22:
          sendToDcsBiosMessage("LEFT_DDI_PB_03", "1");
          break;
        case 23:
          sendToDcsBiosMessage("LEFT_DDI_PB_18", "1");
          break;
        case 24:
          sendToDcsBiosMessage("LEFT_DDI_PB_13", "1");
          break;
        case 25:
          sendToDcsBiosMessage("LEFT_DDI_PB_08", "1");
          break;
        case 26:
          sendToDcsBiosMessage("LEFT_DDI_BRT_SELECT", "2");
          break;
        case 27:
          sendToDcsBiosMessage("RIGHT_DDI_PB_03", "1");
          break;
        case 28:
          sendToDcsBiosMessage("RIGHT_DDI_PB_18", "1");
          break;
        case 29:
          sendToDcsBiosMessage("RIGHT_DDI_PB_13", "1");
          break;
        case 30:
          sendToDcsBiosMessage("RIGHT_DDI_PB_08", "1");
          break;
        case 31:
          sendToDcsBiosMessage("RIGHT_DDI_BRT_SELECT", "2");
          break;
        case 32:
          // UIP removed - sendToDcsBiosMessage("UFC_ILS", "1");
          break;
        case 33:
          sendToDcsBiosMessage("LEFT_DDI_PB_02", "1");
          break;
        case 34:
          sendToDcsBiosMessage("LEFT_DDI_PB_17", "1");
          break;
        case 35:
          sendToDcsBiosMessage("LEFT_DDI_PB_12", "1");
          break;
        case 36:
          sendToDcsBiosMessage("LEFT_DDI_PB_07", "1");
          break;
        case 37:
          // UIP removed - sendToDcsBiosMessage("UFC_COMM1_PULL", "1");
          //sendToDcsBiosMessage("UFC_COMM2_PULL", "1");
          break;
        case 38:
          sendToDcsBiosMessage("RIGHT_DDI_PB_02", "1");
          break;
        case 39:
          sendToDcsBiosMessage("RIGHT_DDI_PB_17", "1");
          break;
        case 40:
          sendToDcsBiosMessage("RIGHT_DDI_PB_12", "1");
          break;
        case 41:
          sendToDcsBiosMessage("RIGHT_DDI_PB_07", "1");
          break;
        case 42:
          sendToDcsBiosMessage("LEFT_FIRE_BTN_COVER", "1");
          LFBCFollowupTask = true;
          timeLFBCOn = millis() + ToggleSwitchCoverMoveTime;
          break;
        case 43:
          sendToDcsBiosMessage("MASTER_CAUTION_RESET_SW", "1");
          break;
        case 44:
          sendToDcsBiosMessage("LEFT_DDI_PB_01", "1");
          break;
        case 45:
          sendToDcsBiosMessage("LEFT_DDI_PB_16", "1");
          break;
        case 46:
          sendToDcsBiosMessage("LEFT_DDI_PB_11", "1");
          break;
        case 47:
          sendToDcsBiosMessage("LEFT_DDI_PB_06", "1");
          break;
        case 48:
          // UIP removed - sendToDcsBiosMessage("UFC_COMM2_PULL", "1");
          break;
        case 49:
          sendToDcsBiosMessage("RIGHT_DDI_PB_01", "1");
          break;
        case 50:
          sendToDcsBiosMessage("RIGHT_DDI_PB_16", "1");
          break;
        case 51:
          sendToDcsBiosMessage("RIGHT_DDI_PB_11", "1");
          break;
        case 52:
          sendToDcsBiosMessage("RIGHT_DDI_PB_06", "1");
          break;
        case 53:
          sendToDcsBiosMessage("RIGHT_FIRE_BTN_COVER", "1");
          RFBCFollowupTask = true;
          timeRFBCOn = millis() + ToggleSwitchCoverMoveTime;
          break;
        case 54:
          sendToDcsBiosMessage("APU_FIRE_BTN", "1");
          break;
        case 55:
          // UIP removed - sendToDcsBiosMessage("AMPCD_PB_05", "1");
          break;
        case 56:
          // UIP removed - sendToDcsBiosMessage("AMPCD_PB_20", "1");
          break;
        case 57:
          // UIP removed - sendToDcsBiosMessage("AMPCD_PB_15", "1");
          break;
        case 58:
          // UIP removed - sendToDcsBiosMessage("AMPCD_PB_10", "1");
          break;
        case 59:
          // UIP removed - sendToDcsBiosMessage("UFC_1", "1");
          break;
        case 60:
          // UIP removed - sendToDcsBiosMessage("UFC_2", "1");
          break;
        case 61:
          // UIP removed - sendToDcsBiosMessage("UFC_3", "1");
          break;
        case 62:
          // UIP removed - sendToDcsBiosMessage("UFC_OS2", "1");
          break;
        case 63:
          // UIP removed - sendToDcsBiosMessage("UFC_AP", "1");
          break;
        case 64:
          // UIP removed - sendToDcsBiosMessage("UFC_IFF", "1");
          break;
        case 65:
          // UIP removed - sendToDcsBiosMessage("UFC_TCN", "1");
          break;
        case 66:
          // UIP removed - sendToDcsBiosMessage("AMPCD_PB_04", "1");
          break;
        case 67:
          // UIP removed - sendToDcsBiosMessage("AMPCD_PB_19", "1");
          break;
        case 68:
          // UIP removed - sendToDcsBiosMessage("AMPCD_PB_14", "1");
          break;
        case 69:
          // UIP removed - sendToDcsBiosMessage("AMPCD_PB_09", "1");
          break;
        case 70:
          // UIP removed - sendToDcsBiosMessage("UFC_4", "1");
          break;
        case 71:
          // UIP removed - sendToDcsBiosMessage("UFC_5", "1");
          break;
        case 72:
          // UIP removed - sendToDcsBiosMessage("UFC_6", "1");
          break;
        case 73:
          // UIP removed - sendToDcsBiosMessage("UFC_OS3", "1");
          break;
        case 74:
          // UIP removed - sendToDcsBiosMessage("UFC_ONOFF", "1");
          break;
        case 75:
          // UIP removed - sendToDcsBiosMessage("UFC_BCN", "1");
          break;
        case 76:
          // UIP removed - sendToDcsBiosMessage("UFC_DL", "1");
          break;
        case 77:
          // UIP removed - sendToDcsBiosMessage("AMPCD_PB_03", "1");
          break;
        case 78:
          // UIP removed - sendToDcsBiosMessage("AMPCD_PB_18", "1");
          break;
        case 79:
          // UIP removed - sendToDcsBiosMessage("AMPCD_PB_13", "1");
          break;
        case 80:
          // UIP removed - sendToDcsBiosMessage("AMPCD_PB_08", "1");
          break;
        case 81:
          // UIP removed - sendToDcsBiosMessage("UFC_7", "1");
          break;
        case 82:
          // UIP removed - sendToDcsBiosMessage("UFC_8", "1");
          break;
        case 83:
          // UIP removed - sendToDcsBiosMessage("UFC_9", "1");
          break;
        case 84:
          // UIP removed - sendToDcsBiosMessage("UFC_OS4", "1");
          break;
        case 85:
          sendToDcsBiosMessage("ECM_MODE_SW", "0");  // OFF
          break;
        case 86:
          sendToDcsBiosMessage("CMSD_DISPENSE_SW", "0");
          break;
        case 87:  // ECM, IS THIS USED
          break;
        case 88:
          // UIP removed - sendToDcsBiosMessage("AMPCD_PB_02", "1");
          break;
        case 89:
          // UIP removed - sendToDcsBiosMessage("AMPCD_PB_17", "1");
          break;
        case 90:
          // UIP removed - sendToDcsBiosMessage("AMPCD_PB_12", "1");
          break;
        case 91:
          // UIP removed - sendToDcsBiosMessage("AMPCD_PB_07", "1");
          break;
        case 92:
          // UIP removed - sendToDcsBiosMessage("UFC_CLR", "1");
          break;
        case 93:
          // UIP removed - sendToDcsBiosMessage("UFC_0", "1");
          break;
        case 94:
          // UIP removed - sendToDcsBiosMessage("UFC_ENT", "1");
          break;
        case 95:
          // UIP removed - sendToDcsBiosMessage("UFC_OS5", "2");
          break;
        case 96:
          sendToDcsBiosMessage("ECM_MODE_SW", "1");  // STBY
          break;
        case 97:
          sendToDcsBiosMessage("CMSD_DISPENSE_SW", "2");
          break;
        case 98:  // EMC, IS THIS USED
          break;
        case 99:
          // UIP removed - sendToDcsBiosMessage("AMPCD_PB_01", "1");
          break;
        case 100:
          // UIP removed - sendToDcsBiosMessage("AMPCD_PB_16", "1");
          break;
        case 101:
          // UIP removed - sendToDcsBiosMessage("AMPCD_PB_11", "1");
          break;
        case 102:
          // UIP removed - sendToDcsBiosMessage("AMPCD_PB_06", "1");
          break;
        case 103:
          // UIP removed - sendToDcsBiosMessage("UFC_ADF", "2");
          break;
        case 104:
          // UIP removed - sendToDcsBiosMessage("UFC_IP", "1");
          break;
        case 105:
          // UIP removed - sendToDcsBiosMessage("UFC_OS1", "1");
          break;
        case 106:
          // UIP removed - sendToDcsBiosMessage("UFC_EMCON", "1");
          break;
        case 107:
          sendToDcsBiosMessage("ECM_MODE_SW", "2");  // BIT
          break;
        case 108:
          sendToDcsBiosMessage("AUX_REL_SW", "0");  // WIRED BACKWARDS IN MY PIT
          break;
        case 109:
          sendToDcsBiosMessage("SAI_TEST_BTN", "1");
          break;
        case 110:
          // UIP removed - sendToDcsBiosMessage("AMPCD_GAIN_SW", "2");
          break;
        case 111:
          // UIP removed - sendToDcsBiosMessage("AMPCD_CONT_SW", "2");
          break;
        case 112:
          // UIP removed - sendToDcsBiosMessage("AMPCD_SYM_SW", "2");
          break;
        case 113:
          // UIP removed - sendToDcsBiosMessage("AMPCD_NIGHT_DAY", "0");
          break;
        case 114:
          // UIP removed - sendToDcsBiosMessage("UFC_ADF", "0");
          break;
        case 115:
          sendToDcsBiosMessage("HUD_VIDEO_BIT", "1");
          break;
        case 116:
          sendToDcsBiosMessage("MASTER_MODE_AA", "1");
          // HUD_STEPPER_FORWARD = true;
          break;
        case 117:
          sendToDcsBiosMessage("FIRE_EXT_BTN", "1");
          break;
        case 118:
          sendToDcsBiosMessage("ECM_MODE_SW", "3");  // REC
          break;
        case 119:  // EMC,
          sendToDcsBiosMessage("CMSD_JET_SEL_BTN", "1");
          break;
        case 120:
          sendToDcsBiosMessage("SAI_CAGE", "1");
          break;
        case 121:
          // UIP removed - sendToDcsBiosMessage("AMPCD_GAIN_SW", "0");
          break;
        case 122:
          // UIP removed - sendToDcsBiosMessage("AMPCD_CONT_SW", "0");
          break;
        case 123:
          // UIP removed - sendToDcsBiosMessage("AMPCD_SYM_SW", "0");
          break;
        case 124:
          // UIP removed - sendToDcsBiosMessage("AMPCD_NIGHT_DAY", "2");
          break;
        case 125:
          sendToDcsBiosMessage("RWR_BIT_BTN", "1");
          break;
        case 126:
          sendToDcsBiosMessage("RWR_DIS_TYPE_SW", "0");  // "U"
          // CHECK INDIVIDUAL ASSIGNMENTS PER PIT
          break;
        case 127:
          sendToDcsBiosMessage("MASTER_MODE_AG", "1");
          // HUD_STEPPER_REVERSE = true;
          break;
        case 128:

          break;
        case 129:
          sendToDcsBiosMessage("ECM_MODE_SW", "4");  // XMIT
          break;
        case 130:  // EMC, IS THIS USED
          break;
        case 131:  // NOT USED
          break;
        case 132:
          // UIP removed - sendToDcsBiosMessage("AMPCD_BRT_CTL", "0");
          break;
        case 133:  // AMPCD, IS THIS USED
          break;
        case 134:  // AMPCD, IS THIS USED
          break;
        case 135:  // AMPCD, IS THIS USED
          break;
        case 136:
          sendToDcsBiosMessage("RWR_OFFSET_BTN", "1");
          break;
        case 137:
          sendToDcsBiosMessage("RWR_DIS_TYPE_SW", "1");  // "A"
          // CHECK INDIVIDUAL ASSIGNMENTS PER PIT
          break;
        case 138:
          sendToDcsBiosMessage("EMER_JETT_BTN", "1");
          break;
        case 139:
          sendToDcsBiosMessage("MASTER_ARM_SW", "1");
          break;
        case 140:                                   //FA-18C_hornet/HUD_ATT_SW
          sendToDcsBiosMessage("HUD_ATT_SW", "0");  //1 FOR OFF
          break;
        case 141:                                             //FA-18C_hornet/HUD_VIDEO_CONTROL_SW
          sendToDcsBiosMessage("HUD_VIDEO_CONTROL_SW", "0");  //1 FOR OFF
          break;
        case 142:                                       //FA-18C_hornet/HUD_SYM_REJ_SW
          sendToDcsBiosMessage("HUD_SYM_REJ_SW", "0");  //1 FOR OFF
          break;
        case 143:  //FA-18C_hornet/IFEI_MODE_BTN
          // UIP removed - sendToDcsBiosMessage("IFEI_MODE_BTN", "1");
          break;
        case 144:  //FA-18C_hornet/IFEI_DWN_BTN
          // UIP removed - sendToDcsBiosMessage("IFEI_DWN_BTN", "1");
          break;
        case 145:  // IFEI, IS THIS USED
          break;
        case 146:                                         //FA-18C_hornet/MODE_SELECTOR_SW
          sendToDcsBiosMessage("MODE_SELECTOR_SW", "0");  //1 FOR OFF
          break;
        case 147:
          sendToDcsBiosMessage("RWR_SPECIAL_BTN", "1");
          break;
        case 148:
          sendToDcsBiosMessage("RWR_DIS_TYPE_SW", "2");  // "I"
          // CHECK INDIVIDUAL ASSIGNMENTS PER PIT
          break;
        case 149:  //FA-18C_hornet/SPIN_RECOVERY_SW
          sendToDcsBiosMessage("SPIN_RECOVERY_SW", "0");
          //FA-18C_hornet/SPIN_RECOVERY_COVER
          sendToDcsBiosMessage("SPIN_RECOVERY_COVER", "0");

          break;
        case 150:  //FA-18C_hornet/IR_COOL_SW
          sendToDcsBiosMessage("IR_COOL_SW", "2");
          break;
        case 151:                                   //FA-18C_hornet/HUD_ATT_SW
          sendToDcsBiosMessage("HUD_ATT_SW", "2");  //1 FOR OFF
          break;
        case 152:                                             //FA-18C_hornet/HUD_VIDEO_CONTROL_SW
          sendToDcsBiosMessage("HUD_VIDEO_CONTROL_SW", "2");  //1 FOR OFF
          break;
        case 153:                                       //FA-18C_hornet/HUD_SYM_REJ_SW
          sendToDcsBiosMessage("HUD_SYM_REJ_SW", "2");  //1 FOR OFF
          break;
        case 154:  //FA-18C_hornet/IFEI_QTY_BTN
          // UIP removed - sendToDcsBiosMessage("IFEI_QTY_BTN", "1");
          break;
        case 155:  //FA-18C_hornet/IFEI_ZONE_BTN
          // UIP removed - sendToDcsBiosMessage("IFEI_ZONE_BTN", "1");
          break;
        case 156:                                             //FA-18C_hornet/SELECT_HMD_LDDI_RDDI
          sendToDcsBiosMessage("SELECT_HMD_LDDI_RDDI", "1");  // NEEDS WORK
          break;
        case 157:                                         //FA-18C_hornet/MODE_SELECTOR_SW
          sendToDcsBiosMessage("MODE_SELECTOR_SW", "2");  //1 FOR OFF
          break;
        case 158:
          sendToDcsBiosMessage("RWR_DISPLAY_BTN", "1");
          break;
        case 159:
          sendToDcsBiosMessage("RWR_DIS_TYPE_SW", "3");  // "N"
          // CHECK INDIVIDUAL ASSIGNMENTS PER PIT
          break;
        case 160:  // SPIN, IS THIS USED
          break;
        case 161:  //FA-18C_hornet/IR_COOL_SW
          sendToDcsBiosMessage("IR_COOL_SW", "0");
          break;
        case 162:  //FA-18C_hornet/HUD_ALT_SW
          sendToDcsBiosMessage("HUD_ALT_SW", "1");
          break;
        case 163:  // HUD, IS THIS USED
          break;
        case 164:  //FA-18C_hornet/HUD_SYM_BRT_SELECT
          sendToDcsBiosMessage("HUD_SYM_BRT_SELECT", "1");
          break;
        case 165:  //FA-18C_hornet/IFEI_UP_BTN
          // UIP removed - sendToDcsBiosMessage("IFEI_UP_BTN", "1");
          break;
        case 166:  //FA-18C_hornet/IFEI_ET_BTN
          // UIP removed - sendToDcsBiosMessage("IFEI_ET_BTN", "1");
          break;
        case 167:
          sendToDcsBiosMessage("LEFT_DDI_HDG_SW", "2");
          break;
        case 168:
          sendToDcsBiosMessage("LEFT_DDI_HDG_SW", "0");
          break;
        case 169:

          if (RWR_POWER_BUTTON_STATE == true) {
            sendToDcsBiosMessage("RWR_POWER_BTN", "0");
          } else {
            sendToDcsBiosMessage("RWR_POWER_BTN", "1");
          }
          // ######### PETE TO ADD LATCH #########
          break;
        case 170:
          sendToDcsBiosMessage("RWR_DIS_TYPE_SW", "4");  // "F"
          // CHECK INDIVIDUAL ASSIGNMENTS PER PIT
          break;
        case 171:
          sendToDcsBiosMessage("SJ_CTR", "1");
          break;
        case 172:
          sendToDcsBiosMessage("SJ_RI", "1");
          break;
        case 173:
          sendToDcsBiosMessage("SJ_LI", "1");
          break;
        case 174:
          sendToDcsBiosMessage("SJ_RO", "1");
          break;
        case 175:
          sendToDcsBiosMessage("SJ_LO", "1");
          break;
        case 176:  // NOT USED
          break;
        case 177:  // NOT USED
          break;
        case 178:  // NOT USED
          break;
        case 179:  // NOT USED
          break;

        default:
          sendToDcsBiosMessage("LIGHTS_TEST_SW", "1");
          break;
          break;
      }
  }
}




// LEFT DDI
DcsBios::PotentiometerEWMA<5, 512, 10> leftDdiBrtCtl("LEFT_DDI_BRT_CTL", A0);
DcsBios::PotentiometerEWMA<5, 512, 10> leftDdiContCtl("LEFT_DDI_CONT_CTL", A1);

// RIGHT DDI
DcsBios::PotentiometerEWMA<5, 512, 10> rightDdiBrtCtl("RIGHT_DDI_BRT_CTL", A2);
DcsBios::PotentiometerEWMA<5, 512, 10> rightDdiContCtl("RIGHT_DDI_CONT_CTL", A3);

// SPIN (HMD) KNOB
DcsBios::PotentiometerEWMA<5, 512, 10> hmdOffBrt("HMD_OFF_BRT", A4);

// HUD ANALOG INPUTS
DcsBios::PotentiometerEWMA<5, 512, 10> hudSymBrt("HUD_SYM_BRT", A5);

// AOA indexer brightness - manual bounded implementation.
// The original DcsBios::PotentiometerEWMA object could transmit 0 when the
// A6 pot was at its low end, disconnected, or transiently read low. In DCS,
// that can blank both the physical AOA indexer output and the in-game/video
// AOA presentation. Preserve the pot direction, but clamp its output range
// to 75%-100% of the DCS-BIOS 16-bit control range.
const byte AOA_INDEXER_ANALOG_PIN = A6;
const unsigned int AOA_INDEXER_MIN_BRIGHTNESS = 49152;  // 75% of 65535
const unsigned int AOA_INDEXER_MAX_BRIGHTNESS = 65535;  // 100%
const unsigned long AOA_INDEXER_REFRESH_MS = 100;
unsigned long nextAoaIndexerBrightnessUpdate = 0;

void UpdateAoaIndexerBrightness() {
  const int raw = analogRead(AOA_INDEXER_ANALOG_PIN);
  unsigned long mapped = map(raw, 0, 1023,
                             AOA_INDEXER_MIN_BRIGHTNESS,
                             AOA_INDEXER_MAX_BRIGHTNESS);

  if (mapped < AOA_INDEXER_MIN_BRIGHTNESS) mapped = AOA_INDEXER_MIN_BRIGHTNESS;
  if (mapped > AOA_INDEXER_MAX_BRIGHTNESS) mapped = AOA_INDEXER_MAX_BRIGHTNESS;

  // Deliberately bypass sendToDcsBiosMessage(): this is a periodic analogue
  // brightness update, not a matrix event, so it should not create reflector/debug traffic.
  if (DCSBIOS_In_Use == 1) {
    char aoaBrightnessArg[8];
    ultoa(mapped, aoaBrightnessArg, 10);
    sendDcsBiosMessage("HUD_AOA_INDEXER", aoaBrightnessArg);
  }
}

void MaintainAoaIndexerBrightness() {
  if (millis() >= nextAoaIndexerBrightnessUpdate) {
    UpdateAoaIndexerBrightness();
    nextAoaIndexerBrightnessUpdate = millis() + AOA_INDEXER_REFRESH_MS;
  }
}

// Replaced by the bounded manual handler above.
// DcsBios::PotentiometerEWMA<5, 512, 10> hudAoaIndexer("HUD_AOA_INDEXER", A6);
DcsBios::PotentiometerEWMA<5, 512, 10> hudBlackLvl("HUD_BLACK_LVL", A7);
DcsBios::PotentiometerEWMA<5, 512, 10> hudBalance("HUD_BALANCE", A8);



void loop() {

  if (millis() >= NEXT_STATUS_TOGGLE_TIMER) {
    GREEN_LED_STATE = !GREEN_LED_STATE;
    RED_LED_STATE = !GREEN_LED_STATE;

    digitalWrite(GREEN_STATUS_LED_PORT, GREEN_LED_STATE);
    digitalWrite(RED_STATUS_LED_PORT, RED_LED_STATE);

    NEXT_STATUS_TOGGLE_TIMER = millis() + FLASH_TIME;
  }

  if (Ethernet_In_Use == 1) {
    if ((millis() - lastalivesent) >= aliveinterval) {
      if (Ethernet_In_Use == 1) {
        aliveudp.beginPacket(reflectorIP, aliveport);
        aliveudp.print(BoardName);
        aliveudp.endPacket();
      }
      lastalivesent = millis();
    }
  }

  if (DCSBIOS_In_Use == 1) DcsBios::loop();

  // A6 is mapped 0..1023 -> 75%..100%, so HUD_AOA_INDEXER cannot be sent as 0.
  MaintainAoaIndexerBrightness();

  updateSteppers();

  if ((HUD_STEPPER_FORWARD == true) || (HUD_STEPPER_REVERSE == true)) {
    if (HUD_STEPPER_ENABLED == false) {
      HUD_STEPPER_ENABLED = true;
      SendDebug("Enabling Hud Stepper");
      digitalWrite(AllstepperEnablePin, false);
      if (HUD_STEPPER_FORWARD == true) {
        HUDStepper.move(2000);
      } else {
        HUDStepper.move(-2000);
      }
    }

  } else if ((HUD_STEPPER_FORWARD == false) && (HUD_STEPPER_REVERSE == false)) {
    if (HUD_STEPPER_ENABLED == true) {
      HUD_STEPPER_ENABLED = false;
      digitalWrite(AllstepperEnablePin, true);
      HUDStepper.setCurrentPosition(0);
      SendDebug("Disabling Hud Stepper");
    }
  }


  // Scan the physical 16 x 11 matrix only. Pins 49-53 are not matrix inputs.
  ScanMatrix();

  FindInputChanges();
  // Handle Switches with Safety covers
  //SPIN COVER
  if (SpinFollowupTask == true) {
    if (millis() >= timeSpinOn) {
      sendToDcsBiosMessage("SPIN_RECOVERY_SW", "1");
      SpinFollowupTask = false;
    }
  }
  // LEFT FIRE BUTTON COVER
  if (LFBCFollowupTask == true) {
    if (millis() >= timeLFBCOn) {
      sendToDcsBiosMessage("LEFT_FIRE_BTN", "1");
      LFBCFollowupTask = false;
    }
  }
  // RIGHT FIRE BUTTON COVER
  if (RFBCFollowupTask == true) {
    if (millis() >= timeRFBCOn) {
      sendToDcsBiosMessage("RIGHT_FIRE_BTN", "1");
      RFBCFollowupTask = false;
    }
  }
  // END CODE
}
