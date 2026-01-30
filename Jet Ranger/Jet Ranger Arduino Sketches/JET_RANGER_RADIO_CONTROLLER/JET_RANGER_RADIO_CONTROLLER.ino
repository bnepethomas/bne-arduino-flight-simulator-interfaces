/*

  ////////////////////---||||||||||********||||||||||---\\\\\\\\\\\\\\\\\\\\
  //||                  FUNCTION = Jet Ranger Radio                      ||\\
  //||            ARDUINO PROCESSOR TYPE = Arduino Mega 2560            ||\\
  //||      ARDUINO CHIP SERIAL NUMBER = SN -       ||\\
  //||                    CONNECTED COM PORT = COM                   ||\\
  //||            ****DO CHECK S/N BEFORE UPLOAD NEW DATA****           ||\\
  ////////////////////---||||||||||********||||||||||---\\\\\\\\\\\\\\\\\\\\

Todos
// Issues when using Due connected to USB closest to Power Socket
// Apears to get further when connected to socket next to reset switch
// Currently Stalls when Ethernet is enabled
// LCD Does started

*/
#define GREEN_STATUS_LED_PORT 14
#define RED_STATUS_LED_PORT 15  // RED LED is used for monitoring ethernet
#define FLASH_TIME 200

unsigned long NEXT_STATUS_TOGGLE_TIMER = 0;
bool RED_LED_STATE = false;
bool GREEN_LED_STATE = true;
unsigned long timeSinceRedLedChanged = 0;

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
String BoardName = "Jet Ranger Radio: ";

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

// ############################################# BEGIN I2C FRAMEWORK ##########################################
#include <Wire.h>

// When Using Arduino Due this is not supported
/*
extern "C" {
#include "utility/twi.h"  // from Wire library, so we can do bus scanning
}
*/
#define TCAADDR 0x70



void tcaselect(uint8_t i) {
  if (i > 7) return;

  Wire.beginTransmission(TCAADDR);
  Wire.write(1 << i);
  Wire.endTransmission();
}

// ############################################# END I2C FRAMEWORK ##########################################


// ############################################# BEGIN CHARACTER OLED ##########################################

#define OLED_Address 0x3c
#define OLED_Command_Mode 0x80
#define OLED_Data_Mode 0x40

#define cmd_CLS 0x01
#define cmd_NewLine 0xC0
#define STX 0x02
#define ETX 0x03


// Clear Screen
//  sendCommand(0x01);
// DEL - CLS
//  sendCommand(0x80);          // Home Pos
// DEL - CLS
//  sendCommand(0xC0);
// Clear screen as used in Jet Ranger
//  sendCommand(cmd_CLS);              // Clear Display

void initCharOLED(uint8_t i2c_address) {
  // *** I2C initial *** //

  String wrkString;

  tcaselect(i2c_address);
  delay(10);
  sendCommand(0x2A);  // **** Set "RE"=1	00101010B
  sendCommand(0x71);
  sendCommand(0x5C);
  sendCommand(0x28);

  sendCommand(0x08);  // **** Set Sleep Mode On
  sendCommand(0x2A);  // **** Set "RE"=1	00101010B
  sendCommand(0x79);  // **** Set "SD"=1	01111001B

  sendCommand(0xD5);
  sendCommand(0x70);
  sendCommand(0x78);  // **** Set "SD"=0

  sendCommand(0x08);  // **** Set 5-dot, 3 or 4 line(0x09), 1 or 2 line(0x08)

  sendCommand(0x06);  // **** Set Com31-->Com0  Seg0-->Seg99

  // **** Set OLED Characterization *** //
  sendCommand(0x2A);  // **** Set "RE"=1
  sendCommand(0x79);  // **** Set "SD"=1

  // **** CGROM/CGRAM Management *** //
  sendCommand(0x72);  // **** Set ROM
  sendCommand(0x00);  // **** Set ROM A and 8 CGRAM


  sendCommand(0xDA);  // **** Set Seg Pins HW Config
  sendCommand(0x10);

  sendCommand(0x81);  // **** Set Contrast
  sendCommand(0xFF);

  sendCommand(0xDB);  // **** Set VCOM deselect level
  sendCommand(0x30);  // **** VCC x 0.83

  sendCommand(0xDC);  // **** Set gpio - turn EN for 15V generator on.
  sendCommand(0x03);

  sendCommand(0x78);  // **** Exiting Set OLED Characterization
  sendCommand(0x28);
  sendCommand(0x2A);
  //sendCommand(0x05); 	// **** Set Entry Mode
  sendCommand(0x06);  // **** Set Entry Mode
  sendCommand(0x08);
  sendCommand(0x28);  // **** Set "IS"=0 , "RE" =0 //28
  sendCommand(0x01);
  sendCommand(0x80);  // **** Set DDRAM Address to 0x80 (line 1 start)

  delay(100);
  sendCommand(0x0C);  // **** Turn on Display

  send_string("Character OLED");
  sendCommand(cmd_NewLine);
  wrkString = "TEST " + String(i2c_address);
  send_string(wrkString.c_str());
}

void sendData(unsigned char data) {
  Wire.beginTransmission(OLED_Address);  // **** Start I2C
  Wire.write(OLED_Data_Mode);            // **** Set OLED Data mode
  Wire.write(data);
  Wire.endTransmission();  // **** End I2C

  Wire1.beginTransmission(OLED_Address);  // **** Start I2C
  Wire1.write(OLED_Data_Mode);            // **** Set OLED Data mode
  Wire1.write(data);
  Wire1.endTransmission();  // **** End I2C
}

void sendCommand(unsigned char command) {
  Wire.beginTransmission(OLED_Address);  // **** Start I2C
  Wire.write(OLED_Command_Mode);         // **** Set OLED Command mode
  Wire.write(command);
  Wire.endTransmission();  // **** End I2C

  Wire1.beginTransmission(OLED_Address);  // **** Start I2C
  Wire1.write(OLED_Command_Mode);         // **** Set OLED Command mode
  Wire1.write(command);
  Wire1.endTransmission();
  //delay(10);
}

void send_string(const char* String) {
  unsigned char i = 0;
  while (String[i]) {
    sendData(String[i]);  // *** Show String to OLED
    i++;
  }
}

#define NAV_OLED_Port 0
#define COMM_OLED_Port 1

void updateNAV(int Channel) {
  tcaselect(NAV_OLED_Port);
  sendCommand(cmd_CLS);
  String workstring = "Channel " + String(Channel);
  send_string(workstring.c_str());
}

void updateCOMM() {
  tcaselect(COMM_OLED_Port);
  sendCommand(cmd_CLS);
  String workstring = "Hello Comm";
  send_string(workstring.c_str());
}

// ############################################# END CHARACTER OLED ##########################################

// ############################################# BEGIN ENCODER ##########################################
// Define the total number of encoders
#define NUM_ENCODERS 6

// Define the pins for each rotary encoder (A and B phases)
// The Arduino Due supports interrupts on ALL digital pins.
// Choose distinct digital pins for each encoder.
const int ENCODER_PINS_A[NUM_ENCODERS] = { 36, 38, 40, 42, 44, 46 };
const int ENCODER_PINS_B[NUM_ENCODERS] = { 37, 39, 41, 43, 45, 47 };

// Volatile variables for interrupt handling for each encoder
// These arrays store the state for each of the NUM_ENCODERS.
volatile long encoderValues[NUM_ENCODERS] = { 0 };                    // Stores the current encoder count
volatile unsigned long lastEncoderChangeTimes[NUM_ENCODERS] = { 0 };  // Stores the micros() timestamp of the last encoder change
volatile int encoderDirections[NUM_ENCODERS] = { 0 };                 // 1 for CW, -1 for CCW, 0 for no change detected by ISR

// Debounce and acceleration parameters (shared across all encoders for simplicity)
const unsigned long DEBOUNCE_DELAY_US = 500;        // Microseconds to debounce (adjust as needed)
const unsigned long FAST_TURN_THRESHOLD_US = 5000;  // If step takes less than this, consider it a fast turn
const int ACCELERATION_MULTIPLIER = 5;              // How much to multiply the step by during fast turns

// --- ISRs for each encoder ---
// Each encoder needs a dedicated ISR. This ISR will be attached to BOTH
// the A and B pins of its respective encoder. When either pin changes,
// this ISR will execute, read both pins, and determine the direction.

void readEncoder0() {
  unsigned long currentTime = micros();
  // Basic software debounce
  if (currentTime - lastEncoderChangeTimes[0] < DEBOUNCE_DELAY_US) return;

  int pinA_state = digitalRead(ENCODER_PINS_A[0]);
  int pinB_state = digitalRead(ENCODER_PINS_B[0]);

  if (pinA_state == pinB_state) {
    encoderDirections[0] = -1;  // Counter-Clockwise
  } else {
    encoderDirections[0] = 1;  // Clockwise
  }
  lastEncoderChangeTimes[0] = currentTime;
}

void readEncoder1() {
  unsigned long currentTime = micros();
  if (currentTime - lastEncoderChangeTimes[1] < DEBOUNCE_DELAY_US) return;
  int pinA_state = digitalRead(ENCODER_PINS_A[1]);
  int pinB_state = digitalRead(ENCODER_PINS_B[1]);
  if (pinA_state == pinB_state) {
    encoderDirections[1] = -1;
  } else {
    encoderDirections[1] = 1;
  }
  lastEncoderChangeTimes[1] = currentTime;
}

void readEncoder2() {
  unsigned long currentTime = micros();
  if (currentTime - lastEncoderChangeTimes[2] < DEBOUNCE_DELAY_US) return;
  int pinA_state = digitalRead(ENCODER_PINS_A[2]);
  int pinB_state = digitalRead(ENCODER_PINS_B[2]);
  if (pinA_state == pinB_state) {
    encoderDirections[2] = -1;
  } else {
    encoderDirections[2] = 1;
  }
  lastEncoderChangeTimes[2] = currentTime;
}

void readEncoder3() {
  unsigned long currentTime = micros();
  if (currentTime - lastEncoderChangeTimes[3] < DEBOUNCE_DELAY_US) return;
  int pinA_state = digitalRead(ENCODER_PINS_A[3]);
  int pinB_state = digitalRead(ENCODER_PINS_B[3]);
  if (pinA_state == pinB_state) {
    encoderDirections[3] = -1;
  } else {
    encoderDirections[3] = 1;
  }
  lastEncoderChangeTimes[3] = currentTime;
}

void readEncoder4() {
  unsigned long currentTime = micros();
  if (currentTime - lastEncoderChangeTimes[4] < DEBOUNCE_DELAY_US) return;
  int pinA_state = digitalRead(ENCODER_PINS_A[4]);
  int pinB_state = digitalRead(ENCODER_PINS_B[4]);
  if (pinA_state == pinB_state) {
    encoderDirections[4] = -1;
  } else {
    encoderDirections[4] = 1;
  }
  lastEncoderChangeTimes[4] = currentTime;
}

void readEncoder5() {
  unsigned long currentTime = micros();
  if (currentTime - lastEncoderChangeTimes[5] < DEBOUNCE_DELAY_US) return;
  int pinA_state = digitalRead(ENCODER_PINS_A[5]);
  int pinB_state = digitalRead(ENCODER_PINS_B[5]);
  if (pinA_state == pinB_state) {
    encoderDirections[5] = -1;
  } else {
    encoderDirections[5] = 1;
  }
  lastEncoderChangeTimes[5] = currentTime;
}

// Array of ISR function pointers for easier iteration in setup
void (*encoderISRs[NUM_ENCODERS])() = {
  readEncoder0,
  readEncoder1,
  readEncoder2,
  readEncoder3,
  readEncoder4,
  readEncoder5
};


// ############################################# END ENCODER  ##########################################



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

    ethernetStartTime = millis() + delayBeforeSendingPacket;
    while (millis() <= ethernetStartTime) {
      delay(FLASH_TIME);
      digitalWrite(RED_STATUS_LED_PORT, false);
      delay(FLASH_TIME);
      digitalWrite(RED_STATUS_LED_PORT, true);
    }

    SendDebug("Ethernet Started " + strMyIP + " " + sMac);
  }

  Wire.begin();
  Wire1.begin();



  // When Using Arduino Due this is not supported
  /*
  for (uint8_t t = 0; t < 8; t++) {
    tcaselect(t);
    // Had to comment out these debugging messages as they created a conflict with the IRQ definition in DCS BIOS
    SendDebug("TCA Port #" + String(t));

    for (uint8_t addr = 0; addr <= 127; addr++) {
      //if (addr == TCAADDR) continue;

      uint8_t data;
      if (!twi_writeTo(addr, &data, 0, 1, 1)) {
        SendDebug("Found I2C " + String(addr));
      }
    }
  }
  // Had to comment out these debugging messages as they created a conflict with the IRQ definition in DCS BIOS
  SendDebug("I2C scan complete");
  */

  initCharOLED(0);
  initCharOLED(1);
  delay(1000);
  updateNAV(100);
  // updateCOMM();

  SendDebug("Start Encoders");

  for (int i = 0; i < NUM_ENCODERS; i++) {
    // Configure encoder pins as inputs with internal pull-up resistors
    pinMode(ENCODER_PINS_A[i], INPUT_PULLUP);
    pinMode(ENCODER_PINS_B[i], INPUT_PULLUP);

    // Attach interrupts to both pins of each encoder
    // Both A and B pins of a single encoder will call the SAME ISR
    attachInterrupt(digitalPinToInterrupt(ENCODER_PINS_A[i]), encoderISRs[i], CHANGE);
    attachInterrupt(digitalPinToInterrupt(ENCODER_PINS_B[i]), encoderISRs[i], CHANGE);
  }

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

    for (int i = 0; i < NUM_ENCODERS; i++) {
    long currentEncoderValueSnapshot;
    unsigned long currentLastChangeTimeSnapshot;
    int currentEncoderDirectionSnapshot;

    // Safely read volatile variables that might be updated by the ISR.
    // 'noInterrupts()' temporarily disables interrupts to prevent data corruption
    // if the ISR tries to update the variables while they are being read in loop().
    noInterrupts(); // Disable interrupts
    currentEncoderValueSnapshot = encoderValues[i]; // (Not strictly needed here as it's updated later)
    currentLastChangeTimeSnapshot = lastEncoderChangeTimes[i];
    currentEncoderDirectionSnapshot = encoderDirections[i];
    encoderDirections[i] = 0; // Reset direction flag after reading it
    interrupts(); // Re-enable interrupts

    // Process the encoder change outside the ISR
    if (currentEncoderDirectionSnapshot != 0) { // If a change was detected by the ISR
      long actualStep = currentEncoderDirectionSnapshot; // Default step is 1 or -1

      unsigned long timeSinceLastChange = micros() - currentLastChangeTimeSnapshot;

      // Apply acceleration if the rotation is fast
      if (timeSinceLastChange < FAST_TURN_THRESHOLD_US) {
        actualStep *= ACCELERATION_MULTIPLIER;
      }

      // Update the main encoder count with acceleration
      encoderValues[i] += actualStep;

      SendDebug("Encoder " + String(i) + " Value: " + String(encoderValues[i]) + ", Step: " +String(actualStep) + ", Time since last change (us): " + String(timeSinceLastChange));
      updateNAV(encoderValues[i]);
    }
  }
}
