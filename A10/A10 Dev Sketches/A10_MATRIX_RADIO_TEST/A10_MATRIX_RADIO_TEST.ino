/* 
Test framework to work around the A10 frequency setting interface
as DCS BIOS only supports encoders for tuning aircraft radios

To find values for controls for a given aircraft - checkout 
DCS-BIOS - lib - modules - aircraft_modules - and then the named of the aircrtaft.lua eg A-10C.lua

*/


#define Ethernet_In_Use 1
#define Reflector_In_Use 1
#define DCSBIOS_In_Use 1
#define DCSBIOS_IRQ_SERIAL
#include "DcsBios.h"


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

const unsigned long delayBeforeSendingPacket = 2000;
unsigned long ethernetStartTime = 0;

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
char* ParameterNamePtr;
char* ParameterValuePtr;


unsigned long currentMillis = 0;
unsigned long previousMillis = 0;
int DCS_On = 0;
int previous_DCS_State = 0;
int DCS_State = 0;
long dcsMillis;
bool DCSACTIVE = false;

void onModTimeChange(char* newValue) {
  // Setting flag to indicae DCS has started - not checking if it is currently active
  DCSACTIVE = true;
}
DcsBios::StringBuffer<6> modTimeBuffer(0x0440, onModTimeChange);

void SendDebug(String MessageToSend) {
  if ((Reflector_In_Use == 1) && (Ethernet_In_Use == 1)) {
    udp.beginPacket(reflectorIP, reflectorport);
    udp.println(MessageToSend);
    udp.endPacket();
  }
}
// ###################################### End Ethernet Related #############################

#define FLASH_TIME 300
bool RED_LED_STATE = false;
long NEXT_STATUS_TOGGLE_TIMER = 0;

#define Check_LED_R 12
#define Check_LED_G 13

#define NUM_BUTTONS 256
#define BUTTONS_USED_ON_PCB 176
#define NUM_AXES 8  // 8 axes, X, Y, Z, etc
#define STATUS_LED_PORT 7
#define FLASH_TIME 200

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


const int ScanDelay = 80;
const int DebounceDelay = 20;

joyReport_t joyReport;
joyReport_t prevjoyReport;


unsigned long joyEndDebounce[NUM_BUTTONS];  // Holds the time we'll look at any more changes in a given input

long prevLEDTransition = millis();
int cButtonID[16];
bool bFirstTime = false;






// Note Pin 4 and Pin 10 cannot be used for other purposes.
//Arduino communicates with both the W5500 and SD card using the SPI bus (through the ICSP header).
//This is on digital pins 10, 11, 12, and 13 on the Uno and pins 50, 51, and 52 on the Mega.
//On both boards, pin 10 is used to select the W5500 and pin 4 for the SD card. These pins cannot be used for general I/O.
//On the Mega, the hardware SS pin, 53, is not used to select either the W5500 or the SD card,
//but it must be kept as an output or the SPI interface won't work.


char stringind[5];
String outString;




void sendToDcsBiosMessage(const char* msg, const char* arg) {

  sendDcsBiosMessage(msg, arg);
}


void setup() {

  pinMode(Check_LED_G, OUTPUT);
  digitalWrite(Check_LED_G, true);
  pinMode(Check_LED_R, OUTPUT);
  digitalWrite(Check_LED_R, true);



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

    SendDebug("Ethernet Started");
    SendDebug("A10 INPUT TEST");
  }

  delay(FLASH_TIME);
  digitalWrite(Check_LED_G, false);
  delay(FLASH_TIME);

  // Set the output ports to output
  for (int portId = 22; portId < 38; portId++) {
    pinMode(portId, OUTPUT);
  }


  // Set the input ports to input - port 50-53 used by Ethernet Shield
  for (int portId = 38; portId < 50; portId++) {
    // Even though we've already got 10K external pullups
    pinMode(portId, INPUT_PULLUP);
  }



  // Initialise all arrays
  for (int ind = 0; ind < NUM_BUTTONS; ind++) {

    // Clear current and last values to 0 for button inputs
    joyReport.button[ind] = 0;
    prevjoyReport.button[ind] = 0;

    // Set the end
    joyEndDebounce[ind] = 0;
  }

  SendDebug("DCS BIOS Setup Started");
  DcsBios::setup();
  SendDebug("DCS BIOS Setup Complete");
}


void FindInputChanges() {

  for (int ind = 0; ind < NUM_BUTTONS; ind++)
    if (bFirstTime) {

      bFirstTime = false;
      // Just Copy Array and perform no actions - this may change in the future
      prevjoyReport.button[ind] = joyReport.button[ind];
    } else {
      // Not the first time - see if there is a difference from last time
      // If there is perform action and update prev array BUT only if we past the end of the debounce period
      if (prevjoyReport.button[ind] != joyReport.button[ind] && millis() > joyEndDebounce[ind]) {

        // First things first - set a new debounce period
        joyEndDebounce[ind] = millis() + DebounceDelay;

        sprintf(stringind, "%03d", ind);


        if (prevjoyReport.button[ind] == 0) {

          SendDebug("Pressed : " + String(stringind));
          if (DCSBIOS_In_Use == 1) CreateDcsBiosMessage(ind, 1);
        } else {
          SendDebug("Released :" + String(stringind));
          if (DCSBIOS_In_Use == 1) CreateDcsBiosMessage(ind, 0);
        }

        prevjoyReport.button[ind] = joyReport.button[ind];
      }
    }
}

String targetVhffmFreq1 = " 4";
String currentVhffmFreq1 = " 4";
void onVhffmFreq1Change(char* newValue) {
  SendDebug("VHF FM Frequency Change");
  currentVhffmFreq1 = String(newValue);
}
DcsBios::StringBuffer<2> vhffmFreq1StrBuffer(0x119a, onVhffmFreq1Change);


int selector1Size  = 13;
char* selector1[] = { " 3", " 4", " 5", " 6", " 7", " 8", " 9", "10", "11", "12", "13", "14", "15" };



void setchannel(String localcurrentReadingString, String localtargetString, int localSize, char *localpointertoarray[], char *valuetoset  ) {
// First checks to see if DCS has started to prevent unwanted commands beings thrown at DCSBIOS
// Check to see if target and desired state match. If not walks the passed character array
// to find match and then increments/decrements the passed parameter.

  if (DCSACTIVE == true) {
    if (localcurrentReadingString != localtargetString) {
      String parameterToSet;
      parameterToSet = String(valuetoset);
      SendDebug("Checking " + parameterToSet);

      SendDebug("Current Reading " + localcurrentReadingString);

      int currentPos = 0;
      int targetPos = 0;
      int deltaPos = 0;
      bool foundCurrent = false;
      bool foundTarget = false;
      for (int i = 0; i < localSize; i++) {
        // SendDebug("Walking Array for current :" + String(i));
        // SendDebug(String(localpointertoarray[i]) + ":" + localcurrentReadingString);
        if (String(localpointertoarray[i]) == localcurrentReadingString) {
          //SendDebug("currentRadingString Postion in array :" + String(i));
          currentPos = i;
          foundCurrent = true;
          break;
        }
      }


      for (int i = 0; i < localSize; i++) {
        // SendDebug("Walking Array for target :" + String(i));
        // SendDebug(String(selector1[i]) + ":" + currentReadingString );
        if (String(localpointertoarray[i]) == localtargetString) {
          // SendDebug("targetString Postion in array :" + String(i));
          targetPos = i;
          foundTarget = true;
          break;
        }
      }

      if (foundCurrent == false) {
        SendDebug("WARNING UNABLE TO FIND CURRENT POSITION IN ARRAY");
      }
      if (foundTarget == false) {
        SendDebug("WARNING UNABLE TO FIND TARGET POSITION IN ARRAY");
      }

      deltaPos = targetPos - currentPos;

      if (deltaPos > 0) {
        sendToDcsBiosMessage(valuetoset, "INC");
      } else {
        sendToDcsBiosMessage(valuetoset, "DEC");
      }

      // SendDebug("Current (" + String(currentPos) + ") and target (" + String(targetPos) + ") Delta :" + String(currentPos - targetPos));
    }
  }
}


void loop() {

  if (millis() >= NEXT_STATUS_TOGGLE_TIMER) {
    RED_LED_STATE = !RED_LED_STATE;

    digitalWrite(Check_LED_G, RED_LED_STATE);
    digitalWrite(Check_LED_R, !RED_LED_STATE);
    NEXT_STATUS_TOGGLE_TIMER = millis() + FLASH_TIME;
  }


  //turn off all rows first
  for (int rowid = 0; rowid < 16; rowid++) {
    //turn on the current row
    // why differentiate? rows


    if (rowid == 0)
      PORTC = 0xFF;
    if (rowid == 8)
      PORTA = 0xFF;

    if (rowid < 8) {
      // Shift 1 right  - this is actually pulling port down
      PORTA = ~(0x1 << rowid);
    } else {
      PORTC = ~(0x1 << (15 - rowid));
    }



    //we must have such a delay so the digital pin output can go LOW steadily,
    //without this delay, the row PIN will not 100% at LOW during yet,
    //so check the first column pin's value will return incorrect result.
    delayMicroseconds(ScanDelay);

    int colResult[16];
    // Reading upper pins
    //pin 38, PD7
    colResult[0] = (PIND & B10000000) == 0 ? 0 : 1;
    //pin 39, PG2
    colResult[1] = (PING & B00000100) == 0 ? 0 : 1;
    //pin 40, PG1
    colResult[2] = (PING & B00000010) == 0 ? 0 : 1;
    //pin 41, PG0
    colResult[3] = (PING & B00000001) == 0 ? 0 : 1;

    //pin 42, PL7
    colResult[4] = (PINL & B10000000) == 0 ? 0 : 1;
    //pin 43, PL6
    colResult[5] = (PINL & B01000000) == 0 ? 0 : 1;
    //pin 44, PL5
    colResult[6] = (PINL & B00100000) == 0 ? 0 : 1;
    //pin 45, PL4
    colResult[7] = (PINL & B00010000) == 0 ? 0 : 1;

    //pin 46, PL3
    colResult[8] = (PINL & B00001000) == 0 ? 0 : 1;
    //pin 47, PL2
    colResult[9] = (PINL & B00000100) == 0 ? 0 : 1;
    //pin 48, PL1
    colResult[10] = (PINL & B00000010) == 0 ? 0 : 1;
    //pin 49, PL0
    //pin 49 is not used on the PCB design - more a mistake than anything else as it is available for us
    //colResult[11] =(PINL & B00000001) == 0 ? 0 : 1;
    colResult[11] = 1;

    // Unable to use pins 50-53 per the following
    //This is on digital pins 10, 11, 12, and 13 on the Uno and pins 50, 51, and 52 on the Mega.
    //On both boards, pin 10 is used to select the W5500 and pin 4 for the SD card. These pins cannot be used for general I/O.
    //On the Mega, the hardware SS pin, 53, is not used to select either the W5500 or the SD card,
    //pin 50, PB3
    //colResult[12] =(PINB & B00001000) == 0 ? 0 : 1;
    colResult[12] = 1;
    //pin 51, PB2
    //colResult[13] =(PINB & B00000100) == 0 ? 0 : 0;
    colResult[13] = 1;
    //pin 52, PB1
    //colResult[14] =(PINB & B00000010) == 0 ? 0 : 0;
    colResult[14] = 1;
    //pin 53, PB0
    //colResult[15] =(PINB & B00000001) == 0 ? 0 : 1;
    colResult[15] = 1;


    // There are 11 Columns per row - gives a total of 176 possible inputs
    // Have left the arrays dimensioned as per original code - if CPU or Memory becomes scarce reduce array
    for (int colid = 0; colid < 16; colid++) {
      if (colResult[colid] == 0) {
        joyReport.button[(rowid * 11) + colid] = 1;
      } else {
        joyReport.button[(rowid * 11) + colid] = 0;
      }
    }
  }


  FindInputChanges();

  setchannel(currentVhffmFreq1,targetVhffmFreq1, selector1Size, selector1, "VHFFM_FREQ1");

  // if (DCSACTIVE == true) {
  //   if (currentReadingString != targetString) {
  //     SendDebug("Incrementing VHF FM Frequency");

  //     SendDebug("Current Reading " + currentReadingString);

  //     int currentPos = 0;
  //     int targetPos = 0;
  //     int deltaPos = 0;
  //     bool foundCurrent = false;
  //     bool foundTarget = false;
  //     for (int i = 0; i < selector1Size; i++) {
  //       SendDebug("Walking Array for current :" + String(i));
  //       SendDebug(String(selector1[i]) + ":" + currentReadingString);
  //       if (String(selector1[i]) == currentReadingString) {
  //         SendDebug("currentRadingString Postion in array :" + String(i));
  //         currentPos = i;
  //         foundCurrent = true;
  //         break;
  //       }
  //     }


  //     for (int i = 0; i < selector1Size; i++) {
  //       // SendDebug("Walking Array for target :" + String(i));
  //       // SendDebug(String(selector1[i]) + ":" + currentReadingString );
  //       if (String(selector1[i]) == targetString) {
  //         // SendDebug("targetString Postion in array :" + String(i));
  //         targetPos = i;
  //         foundTarget = true;
  //         break;
  //       }
  //     }

  //     if (foundCurrent == false) {
  //       SendDebug("WARNING UNABLE TO FIND CURRENT POSITION IN ARRAY");
  //     }
  //     if (foundTarget == false) {
  //       SendDebug("WARNING UNABLE TO FIND TARGET POSITION IN ARRAY");
  //     }

  //     deltaPos = targetPos - currentPos;

  //     if (deltaPos > 0) {
  //       sendToDcsBiosMessage("VHFFM_FREQ1", "INC");
  //     } else {
  //       sendToDcsBiosMessage("VHFFM_FREQ1", "DEC");
  //     }

  //     SendDebug("Current (" + String(currentPos) + ") and target (" + String(targetPos) + ") Delta :" + String(currentPos - targetPos));
  //   }
  // }

  DcsBios::loop();
  currentMillis = millis();
}


void CreateDcsBiosMessage(int ind, int state) {


  switch (state) {
    switch (ind) {
      case 0:
        break;
      case 1:
        break;
      case 2:
        break;
      case 3:
        break;
      case 4:
        break;
      case 5:
        break;
      case 6:
        break;
      case 7:
        break;
      case 8:
        break;
      case 9:
        break;
      case 10:
        break;
      case 11:
        break;
      case 12:
        break;
      case 13:
        break;
      case 14:
        break;
      case 15:
        break;
      case 16:
        break;
      case 17:
        break;
      case 18:
        break;
      case 19:
        break;
      case 20:
        break;
      case 21:
        break;
      case 22:
        break;
      case 23:
        break;
      case 24:
        break;
      case 25:
        break;
      case 26:
        break;
      case 27:
        break;
      case 28:
        break;
      case 29:
        break;
      case 30:
        break;
      case 31:
        break;
      case 32:
        break;
      case 33:
        break;
      case 34:
        break;
      case 35:
        break;
      case 36:
        break;
      case 37:
        break;
      case 38:
        break;
      case 39:
        break;
      case 40:
        break;
      case 41:
        break;
      case 42:
        break;
      case 43:
        break;
      case 44:
        break;
      case 45:
        break;
      case 46:
        break;
      case 47:
        break;
      case 48:
        break;
      case 49:
        break;
      case 50:
        break;
      case 51:
        break;
      case 52:
        break;
      case 53:
        break;
      case 54:
        break;
      case 55:
        break;
      case 56:
        break;
      case 57:
        break;
      case 58:
        break;
      case 59:
        break;
      case 60:
        break;
      case 61:
        break;
      case 62:
        break;
      case 63:
        break;
      case 64:
        break;
      case 65:
        break;
      case 66:
        break;
      case 67:
        break;
      case 68:
        break;
      case 69:
        break;
      case 70:
        break;
      case 71:
        break;
      case 72:
        break;
      case 73:
        break;
      case 74:
        break;
      case 75:
        break;
      case 76:
        break;
      case 77:
        break;
      case 78:
        break;
      case 79:
        break;
      case 80:
        break;
      case 81:
        break;
      case 82:
        break;
      case 83:
        break;
      case 84:
        break;
      case 85:
        break;
      case 86:
        break;
      case 87:
        break;
      case 88:
        break;
      case 89:
        break;
      case 90:
        break;
      case 91:
        break;
      case 92:
        break;
      case 93:
        break;
      case 94:
        break;
      case 95:
        break;
      case 96:
        break;
      case 97:
        break;
      case 98:
        break;
      case 99:
        break;
      case 100:
        break;
      case 101:
        break;
      case 102:
        break;
      case 103:
        break;
      case 104:
        break;
      case 105:
        break;
      case 106:
        break;
      case 107:
        break;
      case 108:
        break;
      case 109:
        break;
      case 110:
        break;
      case 111:
        break;
      case 112:
        break;
      case 113:
        break;
      case 114:
        break;
      case 115:
        break;
      case 116:
        break;
      case 117:
        break;
      case 118:
        break;
      case 119:
        break;
      case 120:
        break;
      case 121:
        break;
      case 122:
        break;
      case 123:
        break;
      case 124:
        break;
      case 125:
        break;
      case 126:
        break;
      case 127:
        break;
      case 128:
        break;
      case 129:
        break;
      case 130:
        break;
      case 131:
        break;
      case 132:
        break;
      case 133:
        break;
      case 134:
        break;
      case 135:
        break;
      case 136:
        break;
      case 137:
        break;
      case 138:
        break;
      case 139:
        break;
      case 140:
        break;
      case 141:
        break;
      case 142:
        break;
      case 143:
        break;
      case 144:
        break;
      case 145:
        break;
      case 146:
        break;
      case 147:
        break;
      case 148:
        break;
      case 149:
        break;
      case 150:
        break;
      case 151:
        break;
      case 152:
        break;
      case 153:
        break;
      case 154:
        break;
      case 155:
        break;
      case 156:
        break;
      case 157:
        break;
      case 158:
        break;
      case 159:
        break;
      case 160:
        break;
      case 161:
        break;
      case 162:
        break;
      case 163:
        break;
      case 164:
        break;
      case 165:
        break;
      case 166:
        break;
      case 167:
        break;
      case 168:
        break;
      case 169:
        break;
      case 170:
        break;
      case 171:
        break;
      case 172:
        break;
      case 173:
        break;
      case 174:
        break;
      case 175:
        break;
      case 176:
        break;
      case 177:
        break;
      case 178:
        break;
      case 179:
        break;
    }
    break;


    case 1:

      // PRESS - CLOSE
      switch (ind) {

        case 0:
          targetVhffmFreq1 = " 3";
          break;
        case 1:
          break;
        case 2:
          break;
        case 3:
          break;
        case 4:
          break;
        case 5:
          break;
        case 6:
          break;
        case 7:
          break;
        case 8:
          break;
        case 9:
          break;
        case 10:
          break;
        case 11:
          targetVhffmFreq1 = " 4";
          break;
        case 12:
          break;
        case 13:
          break;
        case 14:
          break;
        case 15:
          break;
        case 16:
          break;
        case 17:
          break;
        case 18:
          break;
        case 19:
          break;
        case 20:
          break;
        case 21:
          break;
        case 22:
          targetVhffmFreq1 = " 5";
          break;
        case 23:
          break;
        case 24:
          break;
        case 25:
          break;
        case 26:
          break;
        case 27:
          break;
        case 28:
          break;
        case 29:
          break;
        case 30:
          break;
        case 31:
          break;
        case 32:
          break;
        case 33:
          targetVhffmFreq1 = " 6";
          break;
        case 34:
          break;
        case 35:
          break;
        case 36:
          break;
        case 37:
          break;
        case 38:
          break;
        case 39:
          break;
        case 40:
          break;
        case 41:
          break;
        case 42:
          break;
        case 43:
          break;
        case 44:
          targetVhffmFreq1 = " 7";
          break;
        case 45:
          break;
        case 46:
          break;
        case 47:
          break;
        case 48:
          break;
        case 49:
          break;
        case 50:
          break;
        case 51:
          break;
        case 52:
          break;
        case 53:
          break;
        case 54:
          break;
        case 55:
          targetVhffmFreq1 = " 8";
          break;
        case 56:
          break;
        case 57:
          break;
        case 58:
          break;
        case 59:
          break;
        case 60:
          break;
        case 61:
          break;
        case 62:
          break;
        case 63:
          break;
        case 64:
          break;
        case 65:
          break;
        case 66:
          targetVhffmFreq1 = " 9";
          break;
        case 67:
          break;
        case 68:
          break;
        case 69:
          break;
        case 70:
          break;
        case 71:
          break;
        case 72:
          break;
        case 73:
          break;
        case 74:
          break;
        case 75:
          break;
        case 76:
          break;
        case 77:
          targetVhffmFreq1 = "10";
          break;
        case 78:
          break;
        case 79:
          break;
        case 80:
          break;
        case 81:
          break;
        case 82:
          break;
        case 83:
          break;
        case 84:
          break;
        case 85:
          break;
        case 86:
          break;
        case 87:
          break;
        case 88:
          targetVhffmFreq1 = "11";
          break;
        case 89:
          break;
        case 90:
          break;
        case 91:
          break;
        case 92:
          break;
        case 93:
          break;
        case 94:
          break;
        case 95:
          break;
        case 96:
          break;
        case 97:
          break;
        case 98:
          break;
        case 99:
          targetVhffmFreq1 = "11";
          break;
        case 100:
          break;
        case 101:
          break;
        case 102:
          break;
        case 103:
          break;
        case 104:
          break;
        case 105:
          break;
        case 106:
          break;
        case 107:
          break;
        case 108:
          break;
        case 109:
          break;
        case 110:
          targetVhffmFreq1 = "12";
          break;
        case 111:
          break;
        case 112:
          break;
        case 113:
          break;
        case 114:
          break;
        case 115:
          break;
        case 116:
          break;
        case 117:
          break;
        case 118:
          break;
        case 119:
          break;
        case 120:
          break;
        case 121:
          targetVhffmFreq1 = "13";
          break;
        case 122:
          break;
        case 123:
          break;
        case 124:
          break;
        case 125:
          break;
        case 126:
          break;
        case 127:
          break;
        case 128:
          break;
        case 129:
          break;
        case 130:
          break;
        case 131:
          break;
        case 132:
          break;
        case 133:
          break;
        case 134:
          break;
        case 135:
          break;
        case 136:
          break;
        case 137:
          break;
        case 138:
          break;
        case 139:
          break;
        case 140:
          break;
        case 141:
          break;
        case 142:
          break;
        case 143:
          break;
        case 144:
          break;
        case 145:
          break;
        case 146:
          break;
        case 147:
          break;
        case 148:
          break;
        case 149:
          break;
        case 150:
          break;
        case 151:
          break;
        case 152:
          break;
        case 153:
          break;
        case 154:
          break;
        case 155:
          break;
        case 156:
          break;
        case 157:
          break;
        case 158:
          break;
        case 159:
          break;
        case 160:
          break;
        case 161:
          break;
        case 162:
          break;
        case 163:
          break;
        case 164:
          break;
        case 165:
          break;
        case 166:
          break;
        case 167:
          break;
        case 168:
          break;
        case 169:
          break;
        case 170:
          break;
        case 171:
          break;
        case 172:
          break;
        case 173:
          break;
        case 174:
          break;
        case 175:
          break;
        case 176:
          break;
        case 177:
          break;
        case 178:
          break;
        case 179:
          break;
        default:
          // PRESS - CLOSE
          break;
      }
  }
}
