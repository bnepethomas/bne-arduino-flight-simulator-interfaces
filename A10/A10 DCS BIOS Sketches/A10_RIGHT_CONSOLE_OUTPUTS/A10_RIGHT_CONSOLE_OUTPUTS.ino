
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





// THE LED PORTS WILL CHANGE FROM THE V1.1 PCB TO THE FOLLOWING
// #define RED_STATUS_LED_PORT 12
// #define GREEN_STATUS_LED_PORT 13
#define RED_STATUS_LED_PORT 6
#define GREEN_STATUS_LED_PORT 5
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
// CHIP U1 (C)
#define SELECT_JET_PANEL 2
#define AOA 2  // NOT DONE
#define MASTER_ARM 2

// CHIP U2 (A)
#define LEFT_EWI 0
#define UFC_PANEL 0
#define BIT_LED 0
#define LEFT_DIS 0  // CHECK IF NEEDED
#define Bit_led 0   // Bit Test Ledt HUD BOX

#define LEFT_GEAR_COL_A 2
#define LEFT_GEAR_ROW_A 1
#define LEFT_GEAR_COL_B 1
#define LEFT_GEAR_ROW_B 2


void onFlpLgRightGearLtChange(unsigned int newValue) {
  lc.setLed(SELECT_JET_PANEL, LEFT_GEAR_COL_A, LEFT_GEAR_ROW_A, newValue);
  lc.setLed(SELECT_JET_PANEL, LEFT_GEAR_COL_B, LEFT_GEAR_ROW_B, newValue);
}
DcsBios::IntegerBuffer flpLgRightGearLtBuffer(0x7430, 0x2000, 13, onFlpLgRightGearLtChange);

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
#define COIL_LEFT_HYD_A2 26
#define COIL_LEFT_HYD_A3 24
#define COIL_LEFT_HYD_A4 28

#define COIL_RIGHT_HYD_A1 23
#define COIL_RIGHT_HYD_A2 27
#define COIL_RIGHT_HYD_A3 25
#define COIL_RIGHT_HYD_A4 29

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



  if (false) {
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


    AllOn();
    delay(5000);


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
    SetBrightness(8);
  }

  digitalWrite(RED_STATUS_LED_PORT, true);
  digitalWrite(GREEN_STATUS_LED_PORT, true);
  delay(FLASH_TIME);
  digitalWrite(RED_STATUS_LED_PORT, false);
  digitalWrite(GREEN_STATUS_LED_PORT, false);


  // For reasons I'm yet to work out - earlier senddebugs are not sent before this point
  // Testing shows a delay of 3 seconds is needed
  delay(3000);
  SendDebug("LED Initialisation Complete");

  SendDebug("Starting Motor Initialisation");

  if (false) {
    STEPPER_RIGHT_HYD.setMaxSpeed(STEPPER_MAX_SPEED);
    STEPPER_RIGHT_HYD.setAcceleration(STEPPER_ACCELERATION);
    STEPPER_RIGHT_HYD.move(4000);
    SendDebug("Start Stepper Right Hyd");
    while (STEPPER_RIGHT_HYD.distanceToGo() != 0) {
      STEPPER_RIGHT_HYD.run();
    }
    STEPPER_RIGHT_HYD.move(-4000);
    while (STEPPER_RIGHT_HYD.distanceToGo() != 0) {
      STEPPER_RIGHT_HYD.run();
    }
    SendDebug("End Stepper Right Hyd");
  }

  if (false) {
    SendDebug("Start Stepper Left Hyd");
    STEPPER_LEFT_HYD.setMaxSpeed(STEPPER_MAX_SPEED);
    STEPPER_LEFT_HYD.setAcceleration(STEPPER_ACCELERATION);
    STEPPER_LEFT_HYD.move(4000);

    while (STEPPER_LEFT_HYD.distanceToGo() != 0) {
      STEPPER_LEFT_HYD.run();
    }
    STEPPER_LEFT_HYD.move(-4000);
    while (STEPPER_LEFT_HYD.distanceToGo() != 0) {
      STEPPER_LEFT_HYD.run();
    }
    SendDebug("End Stepper Left Hyd");
  }

  if (false) {
    SendDebug("Start Stepper Left Fuel");
    STEPPER_LEFT_FUEL.setMaxSpeed(STEPPER_MAX_SPEED);
    STEPPER_LEFT_FUEL.setAcceleration(STEPPER_ACCELERATION);
    STEPPER_LEFT_FUEL.move(4000);
    while (STEPPER_LEFT_FUEL.distanceToGo() != 0) {
      STEPPER_LEFT_FUEL.run();
    }
    STEPPER_LEFT_FUEL.move(-4000);
    while (STEPPER_LEFT_FUEL.distanceToGo() != 0) {
      STEPPER_LEFT_FUEL.run();
    }
    SendDebug("End Stepper Left Fuel");
  }

  if (false) {
    SendDebug("Start Stepper Right Fuel");
    STEPPER_RIGHT_FUEL.setMaxSpeed(STEPPER_MAX_SPEED);
    STEPPER_RIGHT_FUEL.setAcceleration(STEPPER_ACCELERATION);
    STEPPER_RIGHT_FUEL.move(4000);
    while (STEPPER_RIGHT_FUEL.distanceToGo() != 0) {
      STEPPER_RIGHT_FUEL.run();
    }
    STEPPER_RIGHT_FUEL.move(-4000);
    while (STEPPER_RIGHT_FUEL.distanceToGo() != 0) {
      STEPPER_RIGHT_FUEL.run();
    }
    SendDebug("End Stepper Right Fuel");
  }

  if (false) {
    SendDebug("Start Stepper OXY REG");
    STEPPER_OXY_REG.setMaxSpeed(STEPPER_MAX_SPEED);
    STEPPER_OXY_REG.setAcceleration(STEPPER_ACCELERATION);
    STEPPER_OXY_REG.move(4000);
    while (STEPPER_OXY_REG.distanceToGo() != 0) {
      STEPPER_OXY_REG.run();
    }
    STEPPER_OXY_REG.move(-4000);
    while (STEPPER_OXY_REG.distanceToGo() != 0) {
      STEPPER_OXY_REG.run();
    }
    SendDebug("End Stepper OXY REG");
  }

  if (false) {
    SendDebug("Start Stepper LOX");
    STEPPER_LOX.setMaxSpeed(STEPPER_MAX_SPEED);
    STEPPER_LOX.setAcceleration(STEPPER_ACCELERATION);
    STEPPER_LOX.move(4000);
    while (STEPPER_LOX.distanceToGo() != 0) {
      STEPPER_LOX.run();
    }
    STEPPER_LOX.move(-4000);
    while (STEPPER_LOX.distanceToGo() != 0) {
      STEPPER_LOX.run();
    }
    SendDebug("End Stepper LOX");
  }

  SendDebug("Start Stepper Cabin Press");
  STEPPER_CABIN_PRESS.setMaxSpeed(STEPPER_MAX_SPEED);
  STEPPER_CABIN_PRESS.setAcceleration(STEPPER_ACCELERATION);
  STEPPER_CABIN_PRESS.move(4000);
  while (STEPPER_CABIN_PRESS.distanceToGo() != 0) {
    STEPPER_CABIN_PRESS.run();
  }
  STEPPER_CABIN_PRESS.move(-4000);
  while (STEPPER_CABIN_PRESS.distanceToGo() != 0) {
    STEPPER_CABIN_PRESS.run();
  }
  SendDebug("End Stepper Cabin Press");



  SendDebug("End Motor Initialisation");
}

void loop() {

  if (millis() >= NEXT_STATUS_TOGGLE_TIMER) {
    GREEN_LED_STATE = !GREEN_LED_STATE;
    RED_LED_STATE = !RED_LED_STATE;
    digitalWrite(GREEN_STATUS_LED_PORT, GREEN_LED_STATE);

    NEXT_STATUS_TOGGLE_TIMER = millis() + FLASH_TIME;
  }
}
