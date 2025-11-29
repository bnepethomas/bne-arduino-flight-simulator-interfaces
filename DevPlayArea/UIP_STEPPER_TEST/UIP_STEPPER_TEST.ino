/*




*/
#define GREEN_STATUS_LED_PORT 14
#define RED_STATUS_LED_PORT 15  // RED LED is used for monitoring ethernet
#define FLASH_TIME 3000

unsigned long NEXT_STATUS_TOGGLE_TIMER = 0;
bool RED_LED_STATE = false;
bool GREEN_LED_STATE = true;
unsigned long timeSinceRedLedChanged = 0;

#define Ethernet_In_Use 1

int Reflector_In_Use = 1;



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


int packetSize;
int debugLen;
EthernetUDP udp;
EthernetUDP debugUDP;

#define EthernetStartupDelay 500
#define ES1_RESET_PIN 53

const unsigned long delayBeforeSendingPacket = 3000;
unsigned long ethernetStartTime = 0;
String BoardName = "UIP Stepper Test: ";

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
  }

 

  SendDebug("STEPPER INITIALISATION STARTED");
  pinMode(AllstepperEnablePin, OUTPUT);
  digitalWrite(AllstepperEnablePin, false);

  HUDStepper.setMaxSpeed(STEPPER_MAX_SPEED);
  HUDStepper.setAcceleration(STEPPER_ACCELERATION);


  // ################# Start HUD Startup #########################
  SendDebug("Stepper movement on startup will be disabled once testing is complete");
  SendDebug("Start HUD Stepper");

  HUDStepper.runToNewPosition(-STEPS * 5);
  HUDStepper.setCurrentPosition(0);

  for (int i = 1; i <= 5; i++) {
    SendDebug("Loop :" + String(i));
    SendDebug("Forward"); 
    HUDStepper.runToNewPosition(STEPS * 5);
    delay(1000);
    SendDebug("Reverse"); 
    HUDStepper.runToNewPosition(0);
    delay(1000);
  }

  HUDStepper.setCurrentPosition(0);
  SendDebug("End HUD Stepper");
  // ################# End HUD Startup #########################

  SendDebug("STEPPER INITIALISATION COMPLETE");


  SendDebug("Setup Complete");

}



void loop() {

  if (millis() >= NEXT_STATUS_TOGGLE_TIMER) {
    GREEN_LED_STATE = !GREEN_LED_STATE;
    RED_LED_STATE = !GREEN_LED_STATE;

    digitalWrite(GREEN_STATUS_LED_PORT, GREEN_LED_STATE);
    digitalWrite(RED_STATUS_LED_PORT, RED_LED_STATE);

    NEXT_STATUS_TOGGLE_TIMER = millis() + FLASH_TIME;
  }



  updateSteppers();

  if ((HUD_STEPPER_FORWARD == true) || (HUD_STEPPER_REVERSE == true)) {
    if (HUD_STEPPER_ENABLED == false) {
      HUD_STEPPER_ENABLED = true;
      SendDebug("Enabling Hud Stepper");
      digitalWrite(AllstepperEnablePin, false);
      if (HUD_STEPPER_FORWARD == true) {
        HUDStepper.move(20000);
      } else {
        HUDStepper.move(-20000);
      }
    }

  } else if ((HUD_STEPPER_FORWARD == false) && (HUD_STEPPER_REVERSE == false)) {
    if (HUD_STEPPER_ENABLED == true) {
      HUD_STEPPER_ENABLED = false;
      digitalWrite(AllstepperEnablePin, false);
      HUDStepper.setCurrentPosition(0);
      SendDebug("Disabling Hud Stepper");
    }
  }


  // END CODE
}
