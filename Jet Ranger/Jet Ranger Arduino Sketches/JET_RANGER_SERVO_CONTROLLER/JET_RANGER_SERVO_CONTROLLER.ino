/*

  ////////////////////---||||||||||********||||||||||---\\\\\\\\\\\\\\\\\\\\
  //||                  FUNCTION = Jet Ranger Servo                     ||\\
  //||            ARDUINO PROCESSOR TYPE = Arduino Mega 2560            ||\\
  //||      ARDUINO CHIP SERIAL NUMBER = SN -                           ||\\
  //||                    CONNECTED COM PORT = COM                      ||\\
  //||            ****DO CHECK S/N BEFORE UPLOAD NEW DATA****           ||\\
  ////////////////////---||||||||||********||||||||||---\\\\\\\\\\\\\\\\\\\\


*/
// Master flag for throwing debug messages
bool Debug_Display = false;

#define GREEN_STATUS_LED_PORT 14
#define RED_STATUS_LED_PORT 15  // RED LED is used for monitoring ethernet
#define FLASH_TIME 200

unsigned long NEXT_STATUS_TOGGLE_TIMER = 0;
bool RED_LED_STATE = false;
bool GREEN_LED_STATE = true;
unsigned long timeSinceRedLedChanged = 0;


// ################################ BEGIN ETHERNET #######################################
#define Ethernet_In_Use 1


int Reflector_In_Use = 1;

// When Using Arduino Due this is not supported
/*
#define DCSBIOS_IRQ_SERIAL
#include "DcsBios.h"
*/

// Ethernet Related
#include <SPI.h>
#include <Ethernet.h>
#include <EthernetUdp.h>

// These local Mac and IP Address will be reassigned early in startup based on
// the device ID as set by address pins
byte mac[] = { 0xA8, 0x61, 0x0A, 0x9E, 0x83, 0x02 };
String sMac = "A8:61:0A:9E:83:02";
IPAddress ip(172, 16, 1, 102);
String strMyIP = "172.16.1.102";

// Raspberry Pi is Target
IPAddress reflectorIP(172, 16, 1, 10);
String strreflectorIP = "X.X.X.X";




const unsigned int localport = 7788;
const unsigned int localdebugport = 7795;
const unsigned int MSFSport = 13136;
const unsigned int aliveport = 13137;
const unsigned int reflectorport = 27000;


int packetSize;
int debugLen;
EthernetUDP udp;
EthernetUDP debugUDP;
EthernetUDP max7219udp;  // Max7219
EthernetUDP MSFSudp;     // Listens to MSFS light commands
EthernetUDP aliveudp;    // Sends keepalives to monitoring application

int MSFSpacketsize;
int MSFSLen;

const unsigned long aliveinterval = 10000;
long lastalivesent = 0;
const unsigned long incomingcheckinterval = 5;
long lastincomingpacketcheck = 0;
bool servosZeroed = false;
// No-data watchdog threshold - see the ResetGaugesToZero() call in loop().
const unsigned long noDataTimeoutMs = 30000;


#define EthernetStartupDelay 500
#define ES1_RESET_PIN 53

const unsigned long delayBeforeSendingPacket = 3000;
unsigned long ethernetStartTime = 0;
String BoardName = "Jet Ranger Servo: ";

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

// ################################ END ETHERNET #######################################


// ********************** Added Smoothing Filter for AOA Indexer Brightness
// Not used in UIP combined controller
// From https://github.com/jonnieZG/EWMA
#include <Ewma.h>

// ********************* End Smoothing Filter *************




// ################################ BEGIN WARNING LIGHTS #######################################

String Rotor_RPM_Low = "0";       //RLOW
String Engine_Out = "0";          //EOUT
String Trans_Oil_Pressure = "0";  //TOPW
String Trans_Oil_Temp = "0";
;                              //TOTW
String Battery_Temp = "0";     //BTMP
String Battery_Hot = "0";      //BHOT
String Trans_Chip = "0";       //TC
String Baggage_Door = "0";     //BD
String Engine_Chip = "0";      //EC
String TR_Chip = "0";          //TRC
String Fuel_Pump = "0";        //FPMP
String AFT_Fuel_Filter = "0";  //FFLTR
String Gen_Fail = "0";         //GENF
String Low_Fuel = "0";         //LOWF
String SC_Fail = "0";          //SCF

#define D_Rotor_RPM_Low A2
#define D_Engine_Out A1
#define D_Trans_Oil_Pressure A3
#define D_Trans_Oil_Temp A4
#define D_Battery_Temp A6
#define D_Battery_Hot A5
#define D_Trans_Chip A9
#define D_Baggage_Door A10
#define D_Engine_Chip A7
#define D_TR_Chip A8
#define D_Fuel_Pump A12
#define D_AFT_Fuel_Filter A11
#define D_Gen_Fail A13
#define D_Low_Fuel A14
#define D_SC_Fail A15
#define SPARE1 XX
#define SPARE2 XX
#define SPARE3 XX

void InitialiseWarningLights() {
  pinMode(D_Rotor_RPM_Low, OUTPUT);
  pinMode(D_Engine_Out, OUTPUT);
  pinMode(D_Trans_Oil_Pressure, OUTPUT);
  pinMode(D_Trans_Oil_Temp, OUTPUT);
  pinMode(D_Battery_Temp, OUTPUT);
  pinMode(D_Battery_Hot, OUTPUT);
  pinMode(D_Trans_Chip, OUTPUT);
  pinMode(D_Baggage_Door, OUTPUT);
  pinMode(D_Engine_Chip, OUTPUT);
  pinMode(D_TR_Chip, OUTPUT);
  pinMode(D_Fuel_Pump, OUTPUT);
  pinMode(D_AFT_Fuel_Filter, OUTPUT);
  pinMode(D_Gen_Fail, OUTPUT);
  pinMode(D_Low_Fuel, OUTPUT);
  pinMode(D_SC_Fail, OUTPUT);

  allOn();
}

void setWarningLightAll(bool State) {
  digitalWrite(D_Rotor_RPM_Low, State);
  digitalWrite(D_Engine_Out, State);
  digitalWrite(D_Trans_Oil_Pressure, State);
  digitalWrite(D_Trans_Oil_Temp, State);
  digitalWrite(D_Battery_Temp, State);
  digitalWrite(D_Battery_Hot, State);
  digitalWrite(D_Trans_Chip, State);
  digitalWrite(D_Baggage_Door, State);
  digitalWrite(D_Engine_Chip, State);
  digitalWrite(D_TR_Chip, State);
  digitalWrite(D_Fuel_Pump, State);
  digitalWrite(D_AFT_Fuel_Filter, State);
  digitalWrite(D_Gen_Fail, State);
  digitalWrite(D_Low_Fuel, State);
  digitalWrite(D_SC_Fail, State);
}


void allOff() {
  setWarningLightAll(false);
}

void allOn() {
  setWarningLightAll(true);
}

// ################################ END WARNING LIGHTS #######################################



// ################################ BEGIN SERVO #######################################


bool frontPanelDataChanged = false;
const unsigned long servoCheckInterval = 5;
long lastServoCheck = 0;


String ATTITUDE_INDICATOR_BANK_DEGREES = "";   // BANK
String ATTITUDE_INDICATOR_PITCH_DEGREES = "";  // PITCH
String ELECTRICAL_MASTER_BATTERY = "";         // BATSW
String navCom1Status = "";                     // NAVCOMM1
String AIRSPEED_2 = "";

bool powerAvailable = true;

#include <Servo.h>

Servo ROLL_SERVO;
Servo PITCH_SERVO;





#define PITCH_PORT 26  // Using Gas Producer Port for the moment
#define ROLL_PORT 27   // Using Radar Alt Port for the moment


enum Servos {
  AttitudeIndicatorBankDegrees,
  AttitudeIndicatorPitchDegrees,
  Number_of_Servos
};

//                       BANK PITCH
int aServMinPosition[] = { 5, 166 };
int aServMaxPosition[] = { 179, 70 };
int aServZeroPosition[] = { 93, 113 };
int aServoPosition[] = { 000, 000 };
int aTargetServoPosition[] = { 444, 555 };
long aServoLastupdate[] = { 000, 000 };
bool aServoIdle[] = { 0, 0 };














// Attitude Pitch degrees-to-servo-position calibration table, hand-measured
// on the bench (same struct/interpolation pattern the stepper sketches use
// for their own real-unit calibration tables, e.g. EGT_C_TABLE in
// JET_RANGER_STEPPER_CONTROLLER.ino). "pos" is the raw servo position for
// PITCH_SERVO.write() (0-180, same units as aServMinPosition[]/
// aServMaxPosition[]/aServZeroPosition[] above). Descending pos as deg
// increases (-30 -> 179, 0 -> 113, +30 -> 70) matches this servo's existing
// min/max/zero convention. Sorted ascending by deg - pitchDegToServoPos()
// below relies on that order. Replaces the previous raw-passthrough
// behaviour, where the incoming "PITCH" value was assumed to already be a
// pre-converted servo position computed on the sender's side - the wire
// value is real degrees now.
struct DegToServoPosEntry {
  long deg;
  int pos;
};

const DegToServoPosEntry PITCH_DEG_TABLE[] = {
  { -30, 179 },
  { 0, 113 },
  { 30, 70 },
};
const int PITCH_DEG_TABLE_SIZE = sizeof(PITCH_DEG_TABLE) / sizeof(PITCH_DEG_TABLE[0]);

// Converts a requested attitude pitch in degrees into a servo position by
// linear interpolation between the two nearest PITCH_DEG_TABLE rows. A deg
// value outside the table's -30..30 range is clamped to whichever end is
// nearest rather than extrapolated.
int pitchDegToServoPos(float deg) {
  if (deg <= PITCH_DEG_TABLE[0].deg) return PITCH_DEG_TABLE[0].pos;
  if (deg >= PITCH_DEG_TABLE[PITCH_DEG_TABLE_SIZE - 1].deg) return PITCH_DEG_TABLE[PITCH_DEG_TABLE_SIZE - 1].pos;

  for (int i = 0; i < PITCH_DEG_TABLE_SIZE - 1; i++) {
    long degLo = PITCH_DEG_TABLE[i].deg;
    long degHi = PITCH_DEG_TABLE[i + 1].deg;
    if (deg >= degLo && deg <= degHi) {
      int posLo = PITCH_DEG_TABLE[i].pos;
      int posHi = PITCH_DEG_TABLE[i + 1].pos;
      return posLo + (int)round((double)(deg - degLo) * (posHi - posLo) / (double)(degHi - degLo));
    }
  }
  return PITCH_DEG_TABLE[0].pos;  // unreachable - every deg is covered by the clamps or the loop above
}

// Pitch
void SetPITCH(int TargetValue) {
  if (PITCH_SERVO.attached() == false) {
    PITCH_SERVO.attach(PITCH_PORT);
  }
  aServoLastupdate[AttitudeIndicatorPitchDegrees] = millis();
  aServoIdle[AttitudeIndicatorPitchDegrees] = false;

  PITCH_SERVO.write(TargetValue);
}

// Attitude Bank/Roll degrees-to-servo-position calibration table,
// hand-measured on the bench (reuses the DegToServoPosEntry struct
// PITCH_DEG_TABLE above). "pos" is the raw servo position for
// ROLL_SERVO.write(). Endpoints (5/179) and the 0deg point (93) all match
// aServMinPosition[]/aServMaxPosition[]/aServZeroPosition[] exactly -
// aServZeroPosition[]'s Bank entry was updated from 91 to 93 to match
// this table's bench-measured 0deg point, so the setup-time self-test
// sweep and the no-data-watchdog's ResetGaugesToZero() both now return
// this servo to the same position bankDegToServoPos(0) does. Sorted
// ascending by deg - bankDegToServoPos() below relies on that order.
// Replaces the previous
// raw-passthrough behaviour, where the incoming "BANK" value was assumed
// to already be a pre-converted servo position - the wire value is real
// degrees now, same as PITCH above.
const DegToServoPosEntry BANK_DEG_TABLE[] = {
  { -90, 5 },
  { 0, 93 },
  { 90, 179 },
};
const int BANK_DEG_TABLE_SIZE = sizeof(BANK_DEG_TABLE) / sizeof(BANK_DEG_TABLE[0]);

// Converts a requested attitude bank/roll in degrees into a servo
// position by linear interpolation between the two nearest
// BANK_DEG_TABLE rows (same pattern as pitchDegToServoPos() above). A deg
// value outside the table's -90..90 range is clamped to whichever end is
// nearest rather than extrapolated.
int bankDegToServoPos(float deg) {
  if (deg <= BANK_DEG_TABLE[0].deg) return BANK_DEG_TABLE[0].pos;
  if (deg >= BANK_DEG_TABLE[BANK_DEG_TABLE_SIZE - 1].deg) return BANK_DEG_TABLE[BANK_DEG_TABLE_SIZE - 1].pos;

  for (int i = 0; i < BANK_DEG_TABLE_SIZE - 1; i++) {
    long degLo = BANK_DEG_TABLE[i].deg;
    long degHi = BANK_DEG_TABLE[i + 1].deg;
    if (deg >= degLo && deg <= degHi) {
      int posLo = BANK_DEG_TABLE[i].pos;
      int posHi = BANK_DEG_TABLE[i + 1].pos;
      return posLo + (int)round((double)(deg - degLo) * (posHi - posLo) / (double)(degHi - degLo));
    }
  }
  return BANK_DEG_TABLE[0].pos;  // unreachable - every deg is covered by the clamps or the loop above
}

// Roll
void SetROLL(int TargetValue) {
  if (ROLL_SERVO.attached() == false) {
    ROLL_SERVO.attach(ROLL_PORT);
  }
  aServoLastupdate[AttitudeIndicatorBankDegrees] = millis();
  aServoIdle[AttitudeIndicatorBankDegrees] = false;

  ROLL_SERVO.write(TargetValue);
}






void UpdateServoPos() {
  bool positionUpdated = false;

  // Walk though all Servos and see if we need to update a servo location
  for (int i = 0; i < Number_of_Servos; i++) {
    positionUpdated = false;

    if (aTargetServoPosition[i] > aServoPosition[i]) {
      aServoPosition[i]++;
      positionUpdated = true;
    } else if (aTargetServoPosition[i] < aServoPosition[i]) {
      aServoPosition[i]--;
      positionUpdated = true;
    }

    if (positionUpdated == true) {
      switch (i) {
        case AttitudeIndicatorBankDegrees:
          SetROLL(aServoPosition[AttitudeIndicatorBankDegrees]);
          break;
        case AttitudeIndicatorPitchDegrees:
          SetPITCH(aServoPosition[AttitudeIndicatorPitchDegrees]);
          break;
        default:
          break;
      }
    }
  }
}

// No-data watchdog - see the timeout check in loop() below. Sets every
// gauge's *target* position back to its calibrated zero
// (aServZeroPosition[], the same array setup()'s post-self-test-sweep
// block and every real "CODE:0" UDP packet's map(0, ...) would resolve
// to) and lets the normal UpdateServoPos() ±1-per-tick easing (right
// below) carry each servo there smoothly, exactly like any other target
// change. Deliberately does NOT write the servos directly the way an
// earlier, since-removed commented-out draft of this feature did - that
// approach wrote the physical position immediately but left
// aServoPosition[]/aTargetServoPosition[] stale, so the next real UDP
// value would have made UpdateServoPos() ease from the wrong remembered
// position instead of the just-zeroed one.
void ResetGaugesToZero() {
  for (int i = 0; i < Number_of_Servos; i++) {
    aTargetServoPosition[i] = aServZeroPosition[i];
  }
  setWarningLightAll(false);

  // Also reset the remembered per-lamp states. HandleOutputValuePair() only
  // drives a lamp when the incoming value differs from these strings, so
  // leaving a stale "1" here would make the sim's next "1" look unchanged
  // and the lamp would stay dark after a timeout.
  Rotor_RPM_Low = "0";
  Engine_Out = "0";
  Trans_Oil_Pressure = "0";
  Trans_Oil_Temp = "0";
  Battery_Temp = "0";
  Battery_Hot = "0";
  Trans_Chip = "0";
  Baggage_Door = "0";
  Engine_Chip = "0";
  TR_Chip = "0";
  Fuel_Pump = "0";
  AFT_Fuel_Filter = "0";
  Gen_Fail = "0";
  Low_Fuel = "0";
  SC_Fail = "0";
}

int ServoIdleTime = 1000;
void CheckServoIdleTime() {

  // PITCH
  if (aServoIdle[AttitudeIndicatorPitchDegrees] == false) {
    //Need to see if we have hit time to detach
    if ((millis() - aServoLastupdate[AttitudeIndicatorPitchDegrees]) >= ServoIdleTime) {
      if (PITCH_SERVO.attached() == true) {
        PITCH_SERVO.detach();
      }
      aServoIdle[AttitudeIndicatorPitchDegrees] = true;
      SendDebug("Detaching Pitch Servo");
    }
  };

  // ROLL
  if (aServoIdle[AttitudeIndicatorBankDegrees] == false) {
    //Need to see if we have hit time to detach
    if ((millis() - aServoLastupdate[AttitudeIndicatorBankDegrees]) >= ServoIdleTime) {
      if (ROLL_SERVO.attached() == true) {
        ROLL_SERVO.detach();
      }
      aServoIdle[AttitudeIndicatorBankDegrees] = true;
      SendDebug("Detaching Roll Servo");
    }
  };
}


// ################################ END SERVO #######################################


// ########################################## BEGIN MSFS DATA RECEIVER ########################################

// This code is based on UIP_MAX7219_NEXTRON_POWER_RELAY in the Hornet Project


void ProcessReceivedMSFSString() {


  // Reading values from packetBuffer which is global
  // All received values are strings for readability
  // NHandles multiple attribute Values in a single packet
  //    D,1:0,2:1,3:1,4:1,5:0,6:0,7:0,8:0,9:1,10:1


  bool bLocalDebug = true;
  char *ParameterNamePtr;
  char *ParameterValuePtr;

  //if (Debug_Display || bLocalDebug ) SendDebug("Processing Packet :" + String(millis()));
  // SendDebug("Processing MSFS Packet");

  bLocalDebug = false;

  String sWrkStr = "";

  const char *delim = ",";

  // Break the received packet into a series of tokens
  // If there is no match the pointer will be null, other points to first parameter
  ParameterNamePtr = strtok(packetBuffer, delim);



  String ParameterNameString(ParameterNamePtr);
  //if (Debug_Display || bLocalDebug ) SendDebug("Parameter Name " + String(ParameterNameString));
  //SendDebug("Parameter Name " + String(ParameterNameString));
  // Print all of the values received as a outer loop
  // and then split inner values
  /* get the first token */

  /* walk through other tokens */

  String wrkstring = "";

  //if (Debug_Display || bLocalDebug )
  //if (ParameterNamePtr != NULL) SendDebug("First Value is: " + String(ParameterNamePtr));
  if (ParameterNameString[0] == 'D') {
    //Handling a Data Packet
    //if (Debug_Display || bLocalDebug ) SendDebug("Handling a Data Packet");
    ParameterNamePtr = strtok(NULL, delim);

    while (ParameterNamePtr != NULL) {
      //if (Debug_Display || bLocalDebug ) SendDebug( "Processing " + String(ParameterNamePtr) );

      wrkstring = String(ParameterNamePtr);
      HandleOutputValuePair(wrkstring);


      ParameterNamePtr = strtok(NULL, delim);
    }



    return;
    // End Handling a Data Packet
  } else if (ParameterNameString[0] == 'C') {
    // Handling a Control Packet
    //if (Debug_Display || bLocalDebug ) SendDebug("Handling a Control Packet");

    ParameterNamePtr = strtok(NULL, delim);

    while (ParameterNamePtr != NULL) {
      //if (Debug_Display || bLocalDebug )SendDebug( "Processing " + String(ParameterNamePtr) );

      wrkstring = String(ParameterNamePtr);
      HandleControlString(wrkstring);

      ParameterNamePtr = strtok(NULL, delim);
    }


    return;

    // Handling a Control Packet
  } else {
    // Unknown Packet Type
    //if (Debug_Display || bLocalDebug ) SendDebug("Unknown Packet Type - ignoring packet");
    return;
  }
}

void HandleOutputValuePair(String str) {

  // We are expected a LedNumber which has XRRCC where X = Max7219 Unit, RR Row, CC Column
  bool bLocalDebug = false;


  //if (Debug_Display || bLocalDebug ) SendDebug("Handling " + str);
  //SendDebug("Handling " + str);

  int delimeterlocation = 0;
  String workstring = "";
  String ParameterName = "";
  String ParameterValue = "";



  delimeterlocation = str.indexOf(':');

  if (delimeterlocation == 0) {
    if (Debug_Display || bLocalDebug) SendDebug("**** WARNING no delimiter passed ****** Looking for :");
  } else {
    //if (Debug_Display || bLocalDebug ) SendDebug("Delimiter is located a position " + String(delimeterlocation));
    ParameterName = getValue(str, ':', 0);
    //if (Debug_Display || bLocalDebug ) SendDebug("lednumber is " + lednumber);
    ParameterValue = getValue(str, ':', 1);
    //if (Debug_Display || bLocalDebug ) SendDebug("ledvalue is " + ledvalue);

    // As the value could contain the null at the end of the string trim it out
    ParameterValue.trim();
    if (ParameterName == "BANK") {
      //SendDebug("Received Bank: " + ParameterValue);
      // Real degrees now, via the bench-measured BANK_DEG_TABLE above
      // (-90deg->5, 0deg->93, +90deg->179) - no longer a raw pass-through
      // servo position. Parsed as float so the one-decimal wire value
      // reaches the interpolation before rounding to a servo position,
      // same as PITCH below.
      aTargetServoPosition[AttitudeIndicatorBankDegrees] = bankDegToServoPos(ParameterValue.toFloat());
    } else if (ParameterName == "PITCH") {
      //SendDebug("Received Pitch: " + ParameterValue);
      // Real degrees now, via the bench-measured PITCH_DEG_TABLE above
      // (-30deg->179, 0deg->113, +30deg->70), same pattern as BANK above.
      // Parsed as float so the one-decimal wire value reaches the
      // interpolation before rounding to a servo position.
      aTargetServoPosition[AttitudeIndicatorPitchDegrees] = pitchDegToServoPos(ParameterValue.toFloat());
    } else if (ParameterName == "BANKRAW") {
      // Distinct raw-position code, bypassing bankDegToServoPos() above -
      // same "<CODE>RAW" pattern the stepper sketches use for their own
      // calibrated gauges.
      aTargetServoPosition[AttitudeIndicatorBankDegrees] = ParameterValue.toInt();
    } else if (ParameterName == "PITCHRAW") {
      // Distinct raw-position code, bypassing pitchDegToServoPos() above.
      aTargetServoPosition[AttitudeIndicatorPitchDegrees] = ParameterValue.toInt();
    } else if (ParameterName == "RLOW") {
      if (Rotor_RPM_Low != ParameterValue) {
        //SendDebug("Rotor Low RPM changed");
        Rotor_RPM_Low = ParameterValue;
        if (Rotor_RPM_Low == "1") {
          digitalWrite(D_Rotor_RPM_Low, true);
        } else {
          digitalWrite(D_Rotor_RPM_Low, false);
        }
      };
    } else if (ParameterName == "EOUT") {
      if (Engine_Out != ParameterValue) {
        //SendDebug("Transmission Temperature changed");
        Engine_Out = ParameterValue;
        if (Engine_Out == "1") {
          digitalWrite(D_Engine_Out, true);
        } else {
          digitalWrite(D_Engine_Out, false);
        }
      };
    } else if (ParameterName == "TOPW") {
      if (Trans_Oil_Pressure != ParameterValue) {
        //SendDebug("Transmission Pressure changed");
        Trans_Oil_Pressure = ParameterValue;
        if (Trans_Oil_Pressure == "1") {
          digitalWrite(D_Trans_Oil_Pressure, true);
        } else {
          digitalWrite(D_Trans_Oil_Pressure, false);
        }
      };
    } else if (ParameterName == "TOTW") {
      //SendDebug("Received Transmission Temperature: " + ParameterValue);
      if (Trans_Oil_Temp != ParameterValue) {
        SendDebug("Transmission Temperature changed");
        Trans_Oil_Temp = ParameterValue;
        if (Trans_Oil_Temp == "1") {
          digitalWrite(D_Trans_Oil_Temp, true);
        } else {
          digitalWrite(D_Trans_Oil_Temp, false);
        }
      };
    } else if (ParameterName == "BTMP") {
      if (Battery_Temp != ParameterValue) {
        SendDebug("Transmission Temperature changed");
        Battery_Temp = ParameterValue;
        if (Battery_Temp == "1") {
          digitalWrite(D_Battery_Temp, true);
        } else {
          digitalWrite(D_Battery_Temp, false);
        }
      };
    } else if (ParameterName == "BHOT") {
      if (Battery_Hot != ParameterValue) {
        SendDebug("Transmission Temperature changed");
        Battery_Hot = ParameterValue;
        if (Battery_Hot == "1") {
          digitalWrite(D_Battery_Hot, true);
        } else {
          digitalWrite(D_Battery_Hot, false);
        }
      };
    } else if (ParameterName == "TC") {
      if (Trans_Chip != ParameterValue) {
        SendDebug("Transmission Temperature changed");
        Trans_Chip = ParameterValue;
        if (Trans_Chip == "1") {
          digitalWrite(D_Trans_Chip, true);
        } else {
          digitalWrite(D_Trans_Chip, false);
        }
      };
    } else if (ParameterName == "BD") {

      if (Baggage_Door != ParameterValue) {
        SendDebug("Transmission Temperature changed");
        Baggage_Door = ParameterValue;
        if (Baggage_Door == "1") {
          digitalWrite(D_Baggage_Door, true);
        } else {
          digitalWrite(D_Baggage_Door, false);
        }
      };
    } else if (ParameterName == "EC") {

      if (Engine_Chip != ParameterValue) {
        SendDebug("Transmission Temperature changed");
        Engine_Chip = ParameterValue;
        if (Engine_Chip == "1") {
          digitalWrite(D_Engine_Chip, true);
        } else {
          digitalWrite(D_Engine_Chip, false);
        }
      };
    } else if (ParameterName == "TRC") {
      if (TR_Chip != ParameterValue) {
        SendDebug("Transmission Temperature changed");
        TR_Chip = ParameterValue;
        if (TR_Chip == "1") {
          digitalWrite(D_TR_Chip, true);
        } else {
          digitalWrite(D_TR_Chip, false);
        }
      };
    } else if (ParameterName == "FPMP") {
      if (Fuel_Pump != ParameterValue) {
        SendDebug("Transmission Temperature changed");
        Fuel_Pump = ParameterValue;
        if (Fuel_Pump == "1") {
          digitalWrite(D_Fuel_Pump, true);
        } else {
          digitalWrite(D_Fuel_Pump, false);
        }
      };
    } else if (ParameterName == "FFLTR") {
      if (AFT_Fuel_Filter != ParameterValue) {
        SendDebug("Transmission Temperature changed");
        AFT_Fuel_Filter = ParameterValue;
        if (AFT_Fuel_Filter == "1") {
          digitalWrite(D_AFT_Fuel_Filter, true);
        } else {
          digitalWrite(D_AFT_Fuel_Filter, false);
        }
      };
    } else if (ParameterName == "GENF") {
      if (Gen_Fail != ParameterValue) {
        SendDebug("Transmission Temperature changed");
        Gen_Fail = ParameterValue;
        if (Gen_Fail == "1") {
          digitalWrite(D_Gen_Fail, true);
        } else {
          digitalWrite(D_Gen_Fail, false);
        }
      };
    } else if (ParameterName == "LOWF") {
      if (Low_Fuel != ParameterValue) {
        Low_Fuel = ParameterValue;
        if (Low_Fuel == "1") {
          digitalWrite(D_Low_Fuel, true);
        } else {
          digitalWrite(D_Low_Fuel, false);
        }
      };
    } else if (ParameterName == "SCF") {
      if (SC_Fail != ParameterValue) {
        SC_Fail = ParameterValue;
        if (SC_Fail == "1") {
          digitalWrite(D_SC_Fail, true);
        } else {
          digitalWrite(D_SC_Fail, false);
        }
      };
    } else if (ParameterName == "NAVCOM1") {
      SendDebug("Received NAVCOM1: " + ParameterValue);
      if (navCom1Status != ParameterValue) {
        SendDebug("NAVCOM1 Status Changed");
        // Only set if it has been more 500mS since last update from local encoder
        navCom1Status = ParameterValue;
        if (navCom1Status == "0")
          powerAvailable = false;
        else
          powerAvailable = true;
      };
    }
    return;
  }
}


void HandleControlString(String str) {
  bool bLocalDebug = false;
  //if (Debug_Display || bLocalDebug ) SendDebug("Handling Control String :" + str);

  // Currnetly just handling Brightness - eg C,B:3

  int delimeterlocation = 0;
  String command = "";
  String setting = "";


  delimeterlocation = str.indexOf(':');

  if (delimeterlocation == 0) {
    //if (Debug_Display || bLocalDebug ) SendDebug("**** WARNING no delimiter passed ****** Looking for :");
  } else {
    //if (Debug_Display || bLocalDebug ) SendDebug("Delimiter is located a position " + String(delimeterlocation));
    command = getValue(str, ':', 0);
    //if (Debug_Display || bLocalDebug ) SendDebug("command is :" + command);
    setting = getValue(str, ':', 1);
    //if (Debug_Display || bLocalDebug ) SendDebug("setting is :" + setting);

    int isetting = setting.toInt();

    // Backlighting and Flood ligghting
    if (command == "B")
      if (isetting >= 0 && isetting <= 15) {
        //analogWrite(FLOOD_LIGHTS, map(isetting, 0, 15, 0, 255));
      }
    //else if (Debug_Display || bLocalDebug ) SendDebug("Invalid Brightness value passed. Value is :" + String(setting));
  }

  return;
}

String getValue(String data, char separator, int index) {
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

boolean isValidNumber(String str) {
  boolean isNum = false;
  if (!(str.charAt(0) == '+' || str.charAt(0) == '-' || isDigit(str.charAt(0)))) return false;

  for (byte i = 1; i < str.length(); i++) {
    if (!(isDigit(str.charAt(i)) || str.charAt(i) == '.')) return false;
  }
  return true;
}


// ########################################## END MSFS DATA RECEIVER ########################################

// ################################ BEGIN SETUP #######################################

void setup() {

  pinMode(GREEN_STATUS_LED_PORT, OUTPUT);
  pinMode(RED_STATUS_LED_PORT, OUTPUT);
  digitalWrite(GREEN_STATUS_LED_PORT, true);
  digitalWrite(RED_STATUS_LED_PORT, false);

  InitialiseWarningLights();

  if (Ethernet_In_Use == 1) {

    // Using manual reset instead of tying to Arduino Reset
    pinMode(ES1_RESET_PIN, OUTPUT);
    digitalWrite(ES1_RESET_PIN, LOW);
    delay(2);
    digitalWrite(ES1_RESET_PIN, HIGH);

    Ethernet.begin(mac, ip);
    udp.begin(localport);
    MSFSudp.begin(MSFSport);
    aliveudp.begin(aliveport);

    ethernetStartTime = millis() + delayBeforeSendingPacket;
    while (millis() <= ethernetStartTime) {
      delay(FLASH_TIME);
      digitalWrite(RED_STATUS_LED_PORT, false);
      delay(FLASH_TIME);
      digitalWrite(RED_STATUS_LED_PORT, true);
    }

    SendDebug("Ethernet Started " + strMyIP + " " + sMac);

    digitalWrite(D_Rotor_RPM_Low, true);
    digitalWrite(D_Engine_Out, true);
    digitalWrite(D_SC_Fail, true);
    digitalWrite(D_Low_Fuel, true);
    digitalWrite(D_Gen_Fail, true);
    digitalWrite(D_Fuel_Pump, true);
    digitalWrite(D_AFT_Fuel_Filter, true);
    digitalWrite(D_TR_Chip, true);
    digitalWrite(D_Baggage_Door, true);
    digitalWrite(D_Trans_Chip, true);
    digitalWrite(D_Engine_Chip, true);
    digitalWrite(D_Battery_Temp, true);
    digitalWrite(D_Battery_Hot, true);
    digitalWrite(D_Trans_Oil_Temp, true);
    digitalWrite(D_Trans_Oil_Pressure, true);


    // Zero Servos

    SetPITCH(aServZeroPosition[AttitudeIndicatorPitchDegrees]);
    PITCH_SERVO.write(aServZeroPosition[AttitudeIndicatorPitchDegrees]);

    SetROLL(aServZeroPosition[AttitudeIndicatorBankDegrees]);
    ROLL_SERVO.write(aServZeroPosition[AttitudeIndicatorBankDegrees]);




    // Pitch
    SetPITCH(aServMinPosition[AttitudeIndicatorPitchDegrees]);
    for (int i = 0; i <= 100; i++) {
      SetPITCH(int(map(i, 0, 100, long(aServMinPosition[AttitudeIndicatorPitchDegrees]), long(aServMaxPosition[AttitudeIndicatorPitchDegrees]))));
      delay(10);
    }
    for (int i = 100; i >= 0; i--) {
      SetPITCH((map(i, 0, 100, long(aServMinPosition[AttitudeIndicatorPitchDegrees]), long(aServMaxPosition[AttitudeIndicatorPitchDegrees]))));
      delay(10);
    }
    SetPITCH(aServZeroPosition[AttitudeIndicatorPitchDegrees]);
    // Give the pitch a little time to settle before rolling
    delay(300);



    // Roll
    SetROLL(aServMinPosition[AttitudeIndicatorBankDegrees]);
    for (int i = 0; i <= 100; i++) {
      SetROLL(int(map(i, 0, 100, long(aServMinPosition[AttitudeIndicatorBankDegrees]), long(aServMaxPosition[AttitudeIndicatorBankDegrees]))));
      delay(10);
    }
    for (int i = 100; i >= 0; i--) {
      SetROLL((map(i, 0, 100, long(aServMinPosition[AttitudeIndicatorBankDegrees]), long(aServMaxPosition[AttitudeIndicatorBankDegrees]))));
      delay(10);
    }
    SetROLL(aServZeroPosition[AttitudeIndicatorBankDegrees]);

    // Park both servos fully at their calibrated zero. The sweep above
    // drives the servos directly, so aServoPosition[]/aTargetServoPosition[]
    // must be brought into line with where they physically are - otherwise
    // loop()'s UpdateServoPos() would ease from the stale 0 towards the
    // array's initial placeholder targets ({444, 555}) the moment setup
    // finishes, instead of holding zero until real data arrives. Same
    // reasoning ResetGaugesToZero()'s comment gives for the watchdog.
    for (int i = 0; i < Number_of_Servos; i++) {
      aServoPosition[i] = aServZeroPosition[i];
      aTargetServoPosition[i] = aServZeroPosition[i];
    }
    SetPITCH(aServZeroPosition[AttitudeIndicatorPitchDegrees]);
    SetROLL(aServZeroPosition[AttitudeIndicatorBankDegrees]);
    // Let the servos finish travelling back before setup ends.
    delay(500);

    // Every warning lamp off - setWarningLightAll(false) covers all 15 pins
    // (the old hand-written list left D_Rotor_RPM_Low switched ON here).
    setWarningLightAll(false);
  }


  // Start the no-data timeout from the END of setup(), not from reset
  // (lastincomingpacketcheck doubles as "last data received" - see the
  // watchdog in loop()), so the long self-test sweep can't use up the 30s.
  lastincomingpacketcheck = millis();
  servosZeroed = false;

  SendDebug("Setup Complete");
}

// ################################ END SETUP #######################################



void loop() {

  if (millis() >= NEXT_STATUS_TOGGLE_TIMER) {
    GREEN_LED_STATE = !GREEN_LED_STATE;
    RED_LED_STATE = !GREEN_LED_STATE;

    digitalWrite(GREEN_STATUS_LED_PORT, GREEN_LED_STATE);
    digitalWrite(RED_STATUS_LED_PORT, RED_LED_STATE);

    NEXT_STATUS_TOGGLE_TIMER = millis() + FLASH_TIME;
  }

  if ((millis() - lastalivesent) >= aliveinterval) {
    if (Ethernet_In_Use == 1) {
      aliveudp.beginPacket(reflectorIP, aliveport);
      aliveudp.print("SERVO");
      aliveudp.endPacket();
    }
    lastalivesent = millis();
  }


  if ((millis() - lastincomingpacketcheck) >= incomingcheckinterval) {

    MSFSpacketsize = MSFSudp.parsePacket();
    if (MSFSpacketsize > 0) {
      SendDebug("Received a MSFS Packet");

      MSFSLen = MSFSudp.read(packetBuffer, 1000);

      if (MSFSLen > 0) {
        packetBuffer[MSFSLen] = 0;
      }
      if (MSFSpacketsize) {

        ProcessReceivedMSFSString();
        //SendDebug("Exiting MSFS Processing");
      }
      lastincomingpacketcheck = millis();
      servosZeroed = false;
    }
  }

  // No-data watchdog: if no MSFS UDP packet has been received for
  // noDataTimeoutMs, ease every gauge back to its calibrated zero (see
  // ResetGaugesToZero() above) and clear all warning lights.
  // lastincomingpacketcheck doubles as "last data received" here since
  // it's only advanced inside the `if (MSFSpacketsize > 0)` block above,
  // not on every poll tick. servosZeroed latches the reset so it only
  // fires once per outage (cleared back to false the moment real data
  // resumes, above) rather than every loop() while still timed out. This
  // replaces an earlier, incomplete draft of the same idea that used to
  // sit here commented out - see ResetGaugesToZero()'s comment for what
  // was wrong with it.
  if (servosZeroed == false && (millis() - lastincomingpacketcheck) >= noDataTimeoutMs) {
    SendDebug(BoardName + " - no UDP data for " + String(noDataTimeoutMs / 1000) + "s, gauges reset to calibrated zero");
    ResetGaugesToZero();
    servosZeroed = true;
  }

  if ((millis() - lastServoCheck) >= servoCheckInterval) {
    UpdateServoPos();
    CheckServoIdleTime();
    lastServoCheck = millis();
  }
}
