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
byte mac[] = { 0xA8, 0x61, 0x0A, 0x9E, 0x83, 0x01 };
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


#define ENG_OIL_TEMP_PORT 12
#define ENG_OIL_TEMP_PORT 13
#define ENG_TORQUE_PORT 11
#define AIRSPEED_PORT 2
#define GAS_PRODUCER_PORT

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

#define D_Rotor_RPM_Low A1
#define D_Engine_Out A2
#define D_Trans_Oil_Pressure A3
#define D_Trans_Oil_Temp A4
#define D_Battery_Temp A5
#define D_Battery_Hot A6
#define D_Trans_Chip A7
#define D_Baggage_Door A8
#define D_Engine_Chip A9
#define D_TR_Chip A10
#define D_Fuel_Pump A11
#define D_AFT_Fuel_Filter A12
#define D_Gen_Fail A13
#define D_Low_Fuel A14
#define D_SC_Fail A15


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

String ALTITUDE = "";                          // ALT
String AIRSPEED = "";                          // IAS
String VERTICAL_SPEED = "";                    // VSI
String PLANE_ALT_ABOVE_GROUND = "";            // AGL
String ATTITUDE_INDICATOR_BANK_DEGREES = "";   // BANK
String ATTITUDE_INDICATOR_PITCH_DEGREES = "";  // PITCH
String ROTOR_RPM_PCT_1 = "";                   // RPMR
String GENERAL_ENG_PCT_MAX_RPM_1 = "";         // RPME
String ENG_TORQUE_PERCENT_1 = "";              // TQ
String ELECTRICAL_TOTAL_LOAD_AMPS = "";        // AMPS
String TURB_ENG_ITT_1 = "";                    // ITT
String ENG_OIL_TEMPERATURE_1 = "";             // OILT
String FUEL_TOTAL_QUANTITY = "";               // FUEL
String TURB_ENG_CORRECTED_N1_1 = "";           // N1
String ENG_OIL_PRESSURE_1 = "";                // OILP
String ENG_TRANSMISSION_PRESSURE_1 = "";       // XMSNP
String ENG_TRANSMISSION_TEMPERATURE_1 = "";    // XMSNT
String ELECTRICAL_MASTER_BATTERY = "";         // BATSW
String navCom1Status = "";                     // NAVCOMM1
String AIRSPEED_2 = "";

bool powerAvailable = true;

#include <Servo.h>
Servo ENG_TORQUE_SERVO;
Servo myservo;


#define ENG_OIL_TEMP_PORT 12
#define ENG_OIL_TEMP_PORT 13
#define ENG_TORQUE_PORT 11
#define AIRSPEED_PORT 2
#define GAS_PRODUCER_PORT 12

enum Servos {
  AirSpeed,
  VerticalSpeed,
  AttitudeIndicatorBankDegrees,
  AttitudeIndicatorPitchDegrees,
  RotorRpmPct1,
  GeneralEngPctMaxRpm1,
  EngTorquePercent1,
  ElectricalTotalLoadAmps,
  TurbEngItt1,
  EngOilTemperature1,
  FuelTotalQuantity,
  TurbEngCorrectedN11,
  EngOilPressure1,
  EngTransmissionPressure1,
  EngTransmissionTemperature1,
  PlaneAltAboveGround,
  Number_of_Servos
};

//                            ASP  VSI  BNK  PCH  RPMR RPME TQ   AMPS ITT  OILT FUEL N1  OILP  XMNP XMNT AGL
long aServMinPosition[] = { 44, 10, 444, 555, 28, 242, 176, 527, 121, 310, 124, 121, 560, 9, 424, 222 };
long aServMaxPosition[] = { 955, 952, 444, 555, 895, 986, 37, 740, 802, 20, 736, 000, 864, 288, 107, 222 };
long aServZeroPosition[] = { 44, 498, 444, 555, 28, 242, 176, 527, 121, 310, 124, 121, 560, 9, 424, 222 };
int aServoPosition[] = { 000, 000, 000, 000, 000, 000, 000, 000, 000, 000, 000, 000, 000, 000, 000, 000 };
int aTargetServoPosition[] = { 44, 498, 444, 555, 28, 242, 176, 527, 121, 310, 124, 121, 560, 9, 424, 222 };
long aServoLastupdate[] = { 000, 000, 000, 000, 000, 000, 000, 000, 000, 000, 000, 000, 000, 000, 000, 000 };
bool aServoIdle[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };


void Airspeed(int AirSpeed) {
  int val = 0;
  if (AirSpeed <= 20) {
    val = map(AirSpeed, 0, 20, 44, 70);
  } else if (AirSpeed <= 80) {
    val = map(AirSpeed, 20, 80, 70, 531);
  } else if (AirSpeed <= 100) {
    val = map(AirSpeed, 80, 100, 531, 654);
  } else {
    val = map(AirSpeed, 100, 150, 654, 955);
  }

  myservo.write(val);
}

void VSI(int FPM) {
  /*
  -6000 10
  -4000 105
  -2000 268
  -1000 388
  0     498
  1000  620
  2000  728
  4000  866
  6000  952
  */
}

void RotorSpeed(int Speed) {
  int val = map(Speed, 0, 120, 28, 895);
  myservo.write(val);
}

void TurbineSpeed(int Speed) {
  int val = map(Speed, 0, 120, 242, 986);
  myservo.write(val);
}

void SetEngineTorque(int torque) {
  if (ENG_TORQUE_SERVO.attached() == false) {
    ENG_TORQUE_SERVO.attach(ENG_TORQUE_PORT);
  }
  aServoLastupdate[EngTorquePercent1] = millis();
  aServoIdle[EngTorquePercent1] = false;

  ENG_TORQUE_SERVO.write(torque);
}

void ElectricalLoad(int load) {
  int val = map(load, 0, 100, 527, 740);
  myservo.write(val);
}

void EGT(int temperature) {
  int val = 0;

  if (temperature <= 600) {
    val = map(temperature, 0, 600, 121, 256);
  } else if (temperature <= 700) {
    val = map(temperature, 600, 700, 256, 442);
  } else if (temperature <= 800) {
    val = map(temperature, 700, 800, 442, 656);
  } else {
    val = map(temperature, 800, 910, 656, 802);
  }

  myservo.write(val);
}

void EngineTemperature(int temperature) {
  int val = map(temperature, 0, 150, 310, 20);
  myservo.write(val);
}

void FuelLevel(int Level) {
  // Note Fuel tank is 75 Gallons
  // So multiplying by 10
  int val = map(Level, 0, 75, 124, 736);
  myservo.write(val);
}

void EnginePressure(int pressure) {
  int val = map(pressure, 0, 150, 560, 864);
  myservo.write(val);
}


void TransmissionPressure(int pressure) {
  int val = map(pressure, 0, 150, 9, 288);
  myservo.write(val);
}

void TransmissionTemperature(int temperature) {
  int val = map(temperature, 0, 150, 424, 107);
  myservo.write(val);
}




void GasProducer(int gaspercentage) {
  int val = map(gaspercentage, 0, 106, 57, 848);
  myservo.write(val);
}



void FuelPressure(int pressure) {
  int val = map(pressure, 0, 30, 280, 73);
  myservo.write(val);
}



void UpdateServoPos() {
  bool positionUpdated = false;
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
      if (i == EngTorquePercent1) SetEngineTorque(aServoPosition[EngTorquePercent1]);
    }
  }
}

int ServoIdleTime = 1000;
void CheckServoIdleTime() {
  if (aServoIdle[EngTorquePercent1] == false) {
    //Need to see if we have hit time to detach
    if ((millis() - aServoLastupdate[EngTorquePercent1]) >= ServoIdleTime) {
      if (ENG_TORQUE_SERVO.attached() == true) {
        ENG_TORQUE_SERVO.detach();
      }
      aServoIdle[EngTorquePercent1] = true;
      SendDebug("Detaching Engine Torque Servo");
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
  SendDebug("Handling " + str);

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

    if (ParameterName == "TQ") {
      SendDebug("Received Engine Torque: " + ParameterValue);
      aTargetServoPosition[EngTorquePercent1] = ParameterValue.toInt();

      if (ALTITUDE != ParameterValue) {
        SendDebug("Altitude changed");
        ALTITUDE = ParameterValue;
      };
    } else if (ParameterName == "IAS") {
      //SendDebug("Airspeed: " + ParameterValue);
      if (AIRSPEED != ParameterValue) {
        SendDebug("Airspeed changed");
        AIRSPEED = ParameterValue;
      };
    } else if (ParameterName == "VSI") {
      //SendDebug("Received Vertical Speed: " + ParameterValue);
      if (VERTICAL_SPEED != ParameterValue) {
        SendDebug("Vertical Speed changed");
        VERTICAL_SPEED = ParameterValue;
      };
    } else if (ParameterName == "AGL") {
      //SendDebug("Received Radar Altitude: " + ParameterValue);
      if (PLANE_ALT_ABOVE_GROUND != ParameterValue) {
        SendDebug("Radar Altitude changed");
        PLANE_ALT_ABOVE_GROUND = ParameterValue;
      };
    } else if (ParameterName == "BANK") {
      //SendDebug("Received Bank: " + ParameterValue);
      if (ATTITUDE_INDICATOR_BANK_DEGREES != ParameterValue) {
        SendDebug("Bank changed");
        ATTITUDE_INDICATOR_BANK_DEGREES = ParameterValue;
      };
    } else if (ParameterName == "PITCH") {
      //SendDebug("Received Pitch: " + ParameterValue);
      if (ATTITUDE_INDICATOR_PITCH_DEGREES != ParameterValue) {
        SendDebug("Pitch changed");
        ATTITUDE_INDICATOR_PITCH_DEGREES = ParameterValue;
      };
    } else if (ParameterName == "RPMR") {
      //SendDebug("Received Rotor RPM: " + ParameterValue);
      if (ROTOR_RPM_PCT_1 != ParameterValue) {
        SendDebug("Rotor RPM changed");
        ROTOR_RPM_PCT_1 = ParameterValue;
      };
    } else if (ParameterName == "RPME") {
      //SendDebug("Received Engine RPM: " + ParameterValue);
      if (GENERAL_ENG_PCT_MAX_RPM_1 != ParameterValue) {
        SendDebug("Engine RPM changed");
        GENERAL_ENG_PCT_MAX_RPM_1 = ParameterValue;
      };
    } else if (ParameterName == "TQ") {
      //SendDebug("Received Torque: " + ParameterValue);
      if (ENG_TORQUE_PERCENT_1 != ParameterValue) {
        SendDebug("Torque changed");
        ENG_TORQUE_PERCENT_1 = ParameterValue;
      };
    } else if (ParameterName == "AMPS") {
      //SendDebug("Received Amps: " + ParameterValue);
      if (ELECTRICAL_TOTAL_LOAD_AMPS != ParameterValue) {
        SendDebug("Amps changed");
        ELECTRICAL_TOTAL_LOAD_AMPS = ParameterValue;
      };
    } else if (ParameterName == "ITT") {
      //SendDebug("Received ITT: " + ParameterValue);
      if (TURB_ENG_ITT_1 != ParameterValue) {
        SendDebug("ITT changed");
        TURB_ENG_ITT_1 = ParameterValue;
      };
    } else if (ParameterName == "OILT") {
      //SendDebug("Received il Temp: " + ParameterValue);
      if (ENG_OIL_TEMPERATURE_1 != ParameterValue) {
        SendDebug("Oil Temp changed");
        ENG_OIL_TEMPERATURE_1 = ParameterValue;
      };
    } else if (ParameterName == "FUEL") {
      //SendDebug("Received Fuel: " + ParameterValue);
      if (FUEL_TOTAL_QUANTITY != ParameterValue) {
        SendDebug("Fuel changed");
        FUEL_TOTAL_QUANTITY = ParameterValue;
      };
    } else if (ParameterName == "N1") {
      //SendDebug("Received N1: " + ParameterValue);
      if (TURB_ENG_CORRECTED_N1_1 != ParameterValue) {
        SendDebug("N1 changed");
        TURB_ENG_CORRECTED_N1_1 = ParameterValue;
      };
    } else if (ParameterName == "OILP") {
      //SendDebug("Received Oil Pressure: " + ParameterValue);
      if (ENG_OIL_PRESSURE_1 != ParameterValue) {
        SendDebug("Oil Pressure changed");
        ENG_OIL_PRESSURE_1 = ParameterValue;
      };
    } else if (ParameterName == "XMSNP") {
      //SendDebug("Received Transmission Pressure: " + ParameterValue);
      if (ENG_TRANSMISSION_PRESSURE_1 != ParameterValue) {
        SendDebug("Transmission Pressure changed");
        ENG_TRANSMISSION_PRESSURE_1 = ParameterValue;
      };
    } else if (ParameterName == "XMSNT") {
      //SendDebug("Received Transmission Temperature: " + ParameterValue);
      if (ENG_TRANSMISSION_TEMPERATURE_1 != ParameterValue) {
        SendDebug("Transmission Temperature changed");
        ENG_TRANSMISSION_TEMPERATURE_1 = ParameterValue;
      };
    } else if (ParameterName == "RLOW") {
      //SendDebug("Received Transmission Temperature: " + ParameterValue);
      if (Rotor_RPM_Low != ParameterValue) {
        SendDebug("Transmission Temperature changed");
        Rotor_RPM_Low = ParameterValue;
      };
    } else if (ParameterName == "EOUT") {
      //SendDebug("Received Transmission Temperature: " + ParameterValue);
      if (Engine_Out != ParameterValue) {
        SendDebug("Transmission Temperature changed");
        Engine_Out = ParameterValue;
      };
    } else if (ParameterName == "TOPW") {
      //SendDebug("Received Transmission Temperature: " + ParameterValue);
      if (Trans_Oil_Pressure != ParameterValue) {
        SendDebug("Transmission Temperature changed");
        Trans_Oil_Pressure = ParameterValue;
      };
    } else if (ParameterName == "TOTW") {
      //SendDebug("Received Transmission Temperature: " + ParameterValue);
      if (Trans_Oil_Temp != ParameterValue) {
        SendDebug("Transmission Temperature changed");
        Trans_Oil_Temp = ParameterValue;
      };
    } else if (ParameterName == "BTMP") {
      //SendDebug("Received Transmission Temperature: " + ParameterValue);
      if (Battery_Temp != ParameterValue) {
        SendDebug("Transmission Temperature changed");
        Battery_Temp = ParameterValue;
      };
    } else if (ParameterName == "BHOT") {
      //SendDebug("Received Transmission Temperature: " + ParameterValue);
      if (Battery_Hot != ParameterValue) {
        SendDebug("Transmission Temperature changed");
        Battery_Hot = ParameterValue;
      };
    } else if (ParameterName == "TC") {
      //SendDebug("Received Transmission Temperature: " + ParameterValue);
      if (Trans_Chip != ParameterValue) {
        SendDebug("Transmission Temperature changed");
        Trans_Chip = ParameterValue;
      };
    } else if (ParameterName == "BD") {
      //SendDebug("Received Transmission Temperature: " + ParameterValue);
      if (Baggage_Door != ParameterValue) {
        SendDebug("Transmission Temperature changed");
        Baggage_Door = ParameterValue;
      };
    } else if (ParameterName == "EC") {
      //SendDebug("Received Transmission Temperature: " + ParameterValue);
      if (Engine_Chip != ParameterValue) {
        SendDebug("Transmission Temperature changed");
        Engine_Chip = ParameterValue;
      };
    } else if (ParameterName == "TRC") {
      //SendDebug("Received Transmission Temperature: " + ParameterValue);
      if (TR_Chip != ParameterValue) {
        SendDebug("Transmission Temperature changed");
        TR_Chip = ParameterValue;
      };
    } else if (ParameterName == "FPMP") {
      //SendDebug("Received Transmission Temperature: " + ParameterValue);
      if (Fuel_Pump != ParameterValue) {
        SendDebug("Transmission Temperature changed");
        Fuel_Pump = ParameterValue;
      };
    } else if (ParameterName == "FFLTR") {
      //SendDebug("Received Transmission Temperature: " + ParameterValue);
      if (AFT_Fuel_Filter != ParameterValue) {
        SendDebug("Transmission Temperature changed");
        AFT_Fuel_Filter = ParameterValue;
      };
    } else if (ParameterName == "GENF") {
      //SendDebug("Received Transmission Temperature: " + ParameterValue);
      if (Gen_Fail != ParameterValue) {
        SendDebug("Transmission Temperature changed");
        Gen_Fail = ParameterValue;
      };
    } else if (ParameterName == "LOWF") {
      //SendDebug("Received Transmission Temperature: " + ParameterValue);
      if (Low_Fuel != ParameterValue) {
        SendDebug("Transmission Temperature changed");
        Low_Fuel = ParameterValue;
      };
    } else if (ParameterName == "SCF") {
      //SendDebug("Received Transmission Temperature: " + ParameterValue);
      if (SC_Fail != ParameterValue) {
        SendDebug("Transmission Temperature changed");
        SC_Fail = ParameterValue;
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

    SetEngineTorque(aServZeroPosition[EngTorquePercent1]);

    delay(20);

    SendDebug("Using Map command");
    for (int i = 0; i <= 120; i++) {
      SetEngineTorque((map(i, 0, 120, aServZeroPosition[EngTorquePercent1], aServMaxPosition[EngTorquePercent1])));
      delay(10);
    }
    for (int i = 120; i >= 0; i--) {
      SetEngineTorque((map(i, 0, 120, aServZeroPosition[EngTorquePercent1], aServMaxPosition[EngTorquePercent1])));
      delay(10);
    }

    SetEngineTorque((int(aServZeroPosition[EngTorquePercent1])));


    for (int i = 0; i < Number_of_Servos; i++) {
      aServoPosition[i] = aServZeroPosition[EngTorquePercent1];
      aTargetServoPosition[i] = aServZeroPosition[EngTorquePercent1];
    }
  }


  SendDebug("Setup Complete");
}

// ################################ END SETUP #######################################



void loop() {

  if (millis() >= NEXT_STATUS_TOGGLE_TIMER) {
    GREEN_LED_STATE = !GREEN_LED_STATE;
    RED_LED_STATE = !GREEN_LED_STATE;

    digitalWrite(GREEN_STATUS_LED_PORT, GREEN_LED_STATE);
    digitalWrite(RED_STATUS_LED_PORT, RED_LED_STATE);

    setWarningLightAll(GREEN_LED_STATE);

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
    }
    lastincomingpacketcheck = millis();
  }


  if ((millis() - lastServoCheck) >= servoCheckInterval) {
    UpdateServoPos();
    CheckServoIdleTime();
    lastServoCheck = millis();
  }
}
