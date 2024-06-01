

#define Ethernet_In_Use 1
#define Reflector_In_Use 1
#define DCSBIOS_In_Use 1
#define MSFS_In_Use 0  // Used to interface into MSFS - set to 0 if not in use

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

const unsigned long delayBeforeSendingPacket = 3000;
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


#include <AccelStepper.h>
#include <Stepper.h>
#define STEPS 720  // steps per revolution (limited to 315°)

#define FLASH_TIME 600
bool RED_LED_STATE = false;
long NEXT_STATUS_TOGGLE_TIMER = 0;

#define ROLL_ZERO_SENSE_IN 8
bool LAST_SENSOR_STATE = false;

#define COIL_STEPPER_1_A 4
#define COIL_STEPPER_1_B 5
#define COIL_STEPPER_1_C 6
#define COIL_STEPPER_1_D 7

// MAX_SPEED of 150 is ocnsistently giving 5/2
#define STEPPER_MAX_SPEED 600
#define STEPPER_ACCELERATION 600

AccelStepper STEPPER_1(AccelStepper::FULL4WIRE, COIL_STEPPER_1_A, COIL_STEPPER_1_B, COIL_STEPPER_1_C, COIL_STEPPER_1_D);
bool SENSOR_STATE;
bool LAST_SENSOR_READING = true;
#define CLOCKWISE 1
#define ANTICLOCKWISE 2
#define STOPPED 0
int direction = STOPPED;

void setup() {
  // put your setup code here, to run once:


  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(ROLL_ZERO_SENSE_IN, INPUT);



  digitalWrite(LED_BUILTIN, true);

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
    }
    SendDebug("Ethernet Started");
  }
  // Serial.begin(115200);
  // Serial.println("");
  // Serial.println("Starting");

  delay(FLASH_TIME);
  digitalWrite(LED_BUILTIN, false);
  delay(FLASH_TIME);


  randomSeed(analogRead(0));
  int STEP_COUNTER = 0;


  STEPPER_1.setMaxSpeed(STEPPER_MAX_SPEED);
  STEPPER_1.setAcceleration(STEPPER_ACCELERATION);

  STEPPER_1.moveTo(300);
  while (STEPPER_1.distanceToGo() != 0) {
    STEPPER_1.run();
    SendDebug("Step Speed: " + String(STEPPER_1.speed()));
  }
  STEPPER_1.moveTo(0);
  while (STEPPER_1.distanceToGo() != 0) {
    STEPPER_1.run();
  }
  if (false) {
    SendDebug("Moving Random Distance");
    int randNumber = random(10, 200);
    STEPPER_1.moveTo(randNumber);
    while (STEPPER_1.distanceToGo() != 0) {
      STEPPER_1.run();
    }



    // Check to see if stepper already in rest position - if so move 20
    SENSOR_STATE = digitalRead(ROLL_ZERO_SENSE_IN);
    if (SENSOR_STATE == false) {
      SendDebug("Stepping away from Zero");
      STEPPER_1.move(20);
      while (STEPPER_1.distanceToGo() != 0) {
        STEPPER_1.run();
      }
    }


    // Find Zero Point - moving in positive direction
    SendDebug("Find Zero point");
    STEPPER_1.moveTo(3000);
    while (STEPPER_1.distanceToGo() != 0) {
      SENSOR_STATE = digitalRead(ROLL_ZERO_SENSE_IN);
      if (SENSOR_STATE == false) {
        break;
      }
      STEPPER_1.run();
    }
    SendDebug("Found Zero Point");
    // Now we have Zero Point - move until interruptor has cleared
    STEPPER_1.setAcceleration(100);
    STEPPER_1.setMaxSpeed(600);
    SENSOR_STATE = digitalRead(ROLL_ZERO_SENSE_IN);
    int STEP_COUNTER = 0;

    STEPPER_1.move(400);
    while (SENSOR_STATE == false) {
      // Stepping until Sensor Reads True
      STEP_COUNTER++;
      STEPPER_1.move(1);
      STEPPER_1.runToPosition();
      //while (STEPPER_1.isRunning()) {
      //  STEPPER_1.runToPosition();
      //}
      SENSOR_STATE = digitalRead(ROLL_ZERO_SENSE_IN);
    }
    SendDebug("Total Steps until true : " + String(STEP_COUNTER));

    // Now Step Backwards until Sensor Reads False (ie interruptor is in place)
    SendDebug("Now Step Clockwise until Sensor Reads False (ie interruptor is in place)");
    SENSOR_STATE = digitalRead(ROLL_ZERO_SENSE_IN);
    STEP_COUNTER = 0;
    while (SENSOR_STATE == true) {
      // Stepping until Sensor Reads True
      STEP_COUNTER++;
      STEPPER_1.move(-1);

      STEPPER_1.runToPosition();

      SENSOR_STATE = digitalRead(ROLL_ZERO_SENSE_IN);
    }
    SendDebug("Total Steps until false : " + String(STEP_COUNTER));
  }

  SendDebug("Moving Random Distance - 224");
  STEPPER_1.setMaxSpeed(STEPPER_MAX_SPEED);
  STEPPER_1.setAcceleration(STEPPER_ACCELERATION);
  int randNumber = random(10, 200);
  STEPPER_1.moveTo(randNumber);
  while (STEPPER_1.distanceToGo() != 0) {
    STEPPER_1.run();
    SendDebug("Step Speed: " + String(STEPPER_1.speed()));
    if (STEPPER_1.speed() <= 60) {
      SENSOR_STATE = digitalRead(ROLL_ZERO_SENSE_IN);
      if (SENSOR_STATE == false) {
        STEPPER_1.setCurrentPosition(0);
      }
    }
  }


  SendDebug("Find Zero - 241");
  STEPPER_1.setMaxSpeed(600);
  STEPPER_1.moveTo(420);
  STEPPER_1.setSpeed(60);
  // Speed of 120 is a fail
  // Speed of 100 gives a cont of 211
  // Speed of 90,80 gives count of 203
  // Speed of 70,60,40,30  gives 199
  // Serial.println("Distance to go is :" + String(STEPPER_1.distanceToGo()));
  // Hornet has roll rate os 225 degrees per second
  // F16 has a roll rate of 240 degrees per second
  // So if ball can rotate once per second we are good
  // 1.8 degrees per step = so 200 steps for 360 degrees
  bool READY_TO_BREAK = false;
  SENSOR_STATE = digitalRead(ROLL_ZERO_SENSE_IN);
  long TIME_TO_ROTATE = millis();
  while ((READY_TO_BREAK == false) || (SENSOR_STATE == true)) {

    // If we sure than interruptor has cleared sensor then get ready to stop
    if (READY_TO_BREAK == false) {
      // Serial.println("Distance to go is :" + String(STEPPER_1.distanceToGo()));
      // Serial.println("Checking if ready to break");
      if (100 > STEPPER_1.distanceToGo()) {
        READY_TO_BREAK = true;
      }
    }

    //STEPPER_1.setSpeed(300);
    if (STEPPER_1.runSpeed() == true)
      STEP_COUNTER++;
    SENSOR_STATE = digitalRead(ROLL_ZERO_SENSE_IN);
  }


  SendDebug("Determing Number of Steps in full revolution");
  STEPPER_1.setCurrentPosition(0);

  STEP_COUNTER = 0;
  // Move 180 Steps then idividual step until sensor reads zero
  STEPPER_1.setMaxSpeed(600);
  STEPPER_1.moveTo(220);
  STEPPER_1.setSpeed(60);
  // Speed of 120 is a fail
  // Speed of 100 gives a cont of 211
  // Speed of 90,80 gives count of 203
  // Speed of 70,60,40,30  gives 199
  // Serial.println("Distance to go is :" + String(STEPPER_1.distanceToGo()));
  // Hornet has roll rate os 225 degrees per second
  // F16 has a roll rate of 240 degrees per second
  // So if ball can rotate once per second we are good
  // 1.8 degrees per step = so 200 steps for 360 degrees
  READY_TO_BREAK = false;
  SENSOR_STATE = digitalRead(ROLL_ZERO_SENSE_IN);
  TIME_TO_ROTATE = millis();
  while ((READY_TO_BREAK == false) || (SENSOR_STATE == true)) {

    // If we sure than interruptor has cleared sensor then get ready to stop
    if (READY_TO_BREAK == false) {
      // Serial.println("Distance to go is :" + String(STEPPER_1.distanceToGo()));
      // Serial.println("Checking if ready to break");
      if (100 > STEPPER_1.distanceToGo()) {
        READY_TO_BREAK = true;
      }
    }

    //STEPPER_1.setSpeed(300);
    if (STEPPER_1.runSpeed() == true)
      STEP_COUNTER++;

    SENSOR_STATE = digitalRead(ROLL_ZERO_SENSE_IN);
  }
  SendDebug("Time to Rotate: " + String((millis() - TIME_TO_ROTATE)) + "mS");
  SendDebug("Total Steps in full Revolution : " + String(STEP_COUNTER));

  delay(1000);
  SendDebug("Running Speed Test");
  STEPPER_1.setAcceleration(600);
  STEPPER_1.setMaxSpeed(600);
  STEPPER_1.setCurrentPosition(0);
  TIME_TO_ROTATE = millis();
  SendDebug("STEPPER_1.runToNewPosition(54);");
  STEPPER_1.runToNewPosition(54);
  delay(500);
  SendDebug("STEPPER_1.runToNewPosition(104);");
  STEPPER_1.runToNewPosition(104);
  delay(500);
  SendDebug("STEPPER_1.runToNewPosition(154);");
  STEPPER_1.runToNewPosition(154);
  delay(500);
  SendDebug("STEPPER_1.runToNewPosition(204);");
  STEPPER_1.runToNewPosition(204);
  STEPPER_1.setCurrentPosition(0);

  STEPPER_1.setAcceleration(900);
  STEPPER_1.setMaxSpeed(900);
  SendDebug("STEPPER_1.runToNewPosition(200);");
  TIME_TO_ROTATE = millis();
  STEPPER_1.runToNewPosition(200);
  // delay(500);
  // SendDebug("STEPPER_1.runToNewPosition(100);");
  // STEPPER_1.runToNewPosition(100);
  // delay(500);
  // SendDebug("STEPPER_1.runToNewPosition(150);");
  // STEPPER_1.runToNewPosition(150);
  // delay(500);
  // SendDebug("STEPPER_1.runToNewPosition(0);");
  // STEPPER_1.runToNewPosition(0);
  SendDebug("Time to Rotate: " + String((millis() - TIME_TO_ROTATE)) + "mS");

  SendDebug("Steps until correct zero point: " + String(stepsUntilBreak()));

  // SendDebug("Counting how many steps are stopped by interruptor");
  // // Photo interruptor is 3 steps wide
  // STEPPER_1.runToNewPosition(20);
  // for (int i = 0; i <= 30; i++) {
  //   STEPPER_1.move(-1);
  //   while (STEPPER_1.distanceToGo() != 0) {
  //     stepIt();
  //     // SendDebug("Distance to Go: " + String(STEPPER_1.distanceToGo()));
  //   }
  //   if (digitalRead(ROLL_ZERO_SENSE_IN))
  //     SendDebug("True");
  //   else
  //     SendDebug("False");
  // }


  SendDebug("All Done");
  // Serial.end();
}


void stepIt() {
  bool THIS_SENSOR_READING;

  if (STEPPER_1.distanceToGo() >= 1)
    direction = CLOCKWISE;
  else if (STEPPER_1.distanceToGo() <= -1)
    direction = ANTICLOCKWISE;
  else direction = STOPPED;

  if (digitalRead(ROLL_ZERO_SENSE_IN))
    THIS_SENSOR_READING = true;
  else
    THIS_SENSOR_READING = false;

  if (LAST_SENSOR_READING == true && THIS_SENSOR_READING == false) {
    SendDebug("SENSOR TRANSISTION!!");
    SendDebug("Reported Position :" + String(STEPPER_1.currentPosition()));
  }

  LAST_SENSOR_READING = THIS_SENSOR_READING;
  if (STEPPER_1.run() == false) {
    // SendDebug("Position: " + String(STEPPER_1.currentPosition()));
    if (direction == CLOCKWISE)
      SendDebug("Clockwise");
    else if (direction == ANTICLOCKWISE)
      SendDebug("Anticlockwise");
  }
}


long stepsUntilBreak() {
  long STEP_COUNTER = 0;
  boolean SENSOR_STATE = digitalRead(ROLL_ZERO_SENSE_IN);
  while (SENSOR_STATE == true) {
    // Stepping until Sensor Reads True
    STEP_COUNTER++;
    STEPPER_1.move(1);

    STEPPER_1.runToPosition();

    SENSOR_STATE = digitalRead(ROLL_ZERO_SENSE_IN);
  }
  return STEP_COUNTER;
}

void loop() {

  if (millis() >= NEXT_STATUS_TOGGLE_TIMER) {
    RED_LED_STATE = !RED_LED_STATE;
    digitalWrite(LED_BUILTIN, RED_LED_STATE);
    // enableAllSteppers();
    // cycleSteppers(650);
    // disableAllSteppers();
    // SendDebug("Uptime " + String(millis()) + " (" + String(millis() / 60000) + ")");
    NEXT_STATUS_TOGGLE_TIMER = millis() + FLASH_TIME;
    // Check to see if model time is updating - if nothing after 30 seconds disble steppers
  }
}
