/*

  Receives a space delimited set of characters and sends them to the keyboard

  Also drives Pixel LEDs - this was needed as Interrupts from DCS BIOS using serial disrupted updates. These values are delimited by
  by '='

  Place Modifiers in the first part of the string being sent.

  Unable to find a way using the normal keyboard library ot send pause - so will need to remap to some other key

  Works on an Arduino DUE.

  Will use the logic from pyHWWLink_Keystroke_Sender

  Initially just sending very simple single characters

  ModifiersOfInterest = ['ALT', 'CTRL', 'SHIFT', 'LSHIFT', 'RSHIFT', 'LCTRL', 'RCTRL', 'LALT', 'RALT', 'LWIN', 'RWIN' ]

  USING UDP_TEST_SENDER.PY for testing

*/


/*
   Sim Control Panel Commands
    Head Tracking             Default Key Bindings
    -------------             --------------------
      Freeze
      Normal                  LWin + F1               F1 Head shift movement on/off
      Centre                  Num 5                   View Centre

    Time
    ----
      Fast                   LCtrl + Z                Time Accelerate
      Real                   LShift + Z               Time Normal

    Toggle
    ------
      Night Vision Glasses  RShift + H               Toggle Goggles
      Labels                LShift + F10             All Labels

    Game
    ----
      Pause                 Pause                    Pause
      Normal                Pause                    Pause
      Freeze

    View
    ----
      Cockpit               F1
      Chase                 LCtrl + F4              F4 Chase view
      External              F2                      F2 Aircraft View
      Fly by                F3                      F3 Fly-By View
      Weapon                F6                      F6 Released Weapon View
      Enemy                 LCtrl + F5              F5 Ground Hostile View
      Hud                   LAlt + F1               F1 Hud only view switch
      Map                   F10                     F10 Theater Map view




*/
#define Ethernet_In_Use 1
const int Serial_In_Use = 1;

// Ethernet Related
#include <SPI.h>
#include <Ethernet.h>
#include <EthernetUdp.h>



#include <Keyboard.h>




// PixelLighting
#include <FastLED.h>
String COLOUR   =  "GREEN";   // The color name that you want to show, e.g. Green, Red, Blue, White
int startUpBrightness =   50;      // LED Brightness 0 = Off, 255 = 100%.
#define MAX_BRIGHTNESS 50

// Set your power supplies 5V current limit.

#define CURRENT_LIMIT   20000   // Current in mA (1000mA = 1 Amp). Most ATX PSUs provide 20A maximum.

// Defining how many pixels each backlighting connector has connected, if a connector is not used set it to zero.


#define LEFT_CONSOLE_LED_COUNT 500
#define RIGHT_CONSOLE_LED_COUNT 500
#define LIP_UIP_CONSOLE_LED_COUNT 100

// Defining what data pin each backlighting connector is connected to.

// The Max7219 connector uses Pin 14,15,16
// Order on connector is 5V GND 16 15 14 Last pin is not connected
#define LEFT_CONSOLE_PIN       16  // BL #3 Connector pin
#define RIGHT_CONSOLE_PIN       15  // BL #4 Connector pin
#define LIP_UIP_PIN       14  // BL #5 Connector pin



// Some other setup information. Don't change these unless you have a reason to.

#define LED_TYPE     WS2812B  // OPENHORNET backlighting LEDs are WS2812B
#define COLOUR_ORDER GRB      // OPENHORNET backlighting LEDs are GRB (green, red, blue)
#define SOLID_SPEED  100     // The refresh rate delay in ms. Leave this at around 1000 (1 second)

// Setting up the blocks of memory that will be used for storing and manipulating the led data;

CRGB LEFT_CONSOLE_LED[LEFT_CONSOLE_LED_COUNT];
CRGB RIGHT_CONSOLE_LED[RIGHT_CONSOLE_LED_COUNT];
CRGB LIP_UIP_CONSOLE_LED[LIP_UIP_CONSOLE_LED_COUNT];

unsigned long NEXT_LED_UPDATE = 0;



bool leftAltInUse = false;
bool rightAltInUse = false;
bool altInUse = false;
bool ctrlInUse = false;
bool shiftInUse = false;
bool lShiftInUse = false;
bool rShiftInUse = false;
bool lCtrlInUse = false;
bool rCtrlInUse = false;
bool lWinInUse = false;
bool rWinInUse = false;

// These local Mac and IP Address will be reassigned early in startup based on
// the device ID as set by address pins
byte mac[] = {0x00, 0xDD, 0x3E, 0xCA, 0x37, 0x99};
IPAddress ip(172, 16, 1, 110);
String strMyIP = "X.X.X.X";

// Raspberry Pi is Target
IPAddress targetIP(172, 16, 1, 2);
String strTargetIP = "X.X.X.X";

const unsigned int localport = 7788;
const unsigned int backlightport = 7789;
const unsigned int forwardlightport = 7790;
const unsigned int remoteport = 49000;
const unsigned int reflectorport = 27000;

// Packet Length
int packetSize;
int len;
int backlightPacketSize;
int backlightLen;
int forwardlightPacketSize;
int forwardlightLen;

const int delayBetweenRelease = 100;

EthernetUDP udp;              // Keyboard
EthernetUDP backlightudp;     //Left and Right Consoles
EthernetUDP forwardlightudp;  //LIP and UIP

char packetBuffer[1000];     //buffer to store the incoming data
char backlightpacketBuffer[1000];     //buffer to store the incoming data
char forwardlightpacketBuffer[1000];     //buffer to store the incoming data
char outpacketBuffer[1000];  //buffer to store the outgoing data

// Variables picked up from 737 Code
bool Debug_Display = true;
char *ParameterNamePtr;
char *ParameterValuePtr;


// Variables from Bens Code
int instDim = 0; // Consoles Dimmer Knob Value - Via DCS
int conDim = 0;
int cautDim = 0; // Caution Dimmer Knob Value - Via DCS
int spinLT = 0; //Spin Light Dimmer Value
int spinOn = 0; // Spin Light On or Off
int consSW = 0; // NVG/NITE/DAY Switch
int ecmLT = 0; // EMC JETT LIGHT (GREEN)
int ecmON = 0; // EMC Light ON (GREEN)
int aaLT = 0; // AA light MASTER ARM
int aaON = 0; // AA light MASTER ARM
int agLT = 0; // AG light MASTER ARM
int agON = 0; // AG light MASTER ARM
int rdyLT = 0; // READY light MASTER ARM
int rdyON = 0; // READY light MASTER ARM
int disLT = 0; // DISCH light MASTER ARM
int disON = 0; // DISCH light MASTER ARM


#define ECM_JETT_LED_COUNT 78
#define VIDEO_RECORD_LED_COUNT 16
#define JET_STATION_LED_COUNT 8
#define MASTER_ARM_LED_COUNT 29
#define HUD_CONTROL_LED_COUNT 55
#define SPIN_RECOVERY_LED_COUNT 53

#define MASTER_ARM_READY_1 0
#define MASTER_ARM_READY_2 1
#define MASTER_ARM_DISCH_1 3
#define MASTER_ARM_DISCH_2 4
#define MASTER_ARM_AA_1 25
#define MASTER_ARM_AA_2 26
#define MASTER_ARM_AG_1 27
#define MASTER_ARM_AG_2 28

#define ECM_JETT_1 74
#define ECM_JETT_2 75
#define ECM_JETT_3 76
#define ECM_JETT_4 77

#define SPIN_1 114
#define SPIN_2 115



void setup() {

  if (Serial_In_Use) {
    Serial.begin(250000);
    Serial.println("UDP to Keyboard Startup");
  }


  if (Ethernet_In_Use == 1) {
    Ethernet.begin( mac, ip);

    udp.begin( localport );
    udp.beginPacket(targetIP, reflectorport);
    udp.println("Init UDP - " + strMyIP + " " + String(millis()) + "mS since reset.");


    backlightudp.begin(backlightport);
    forwardlightudp.begin(forwardlightport);

  }

  // Activate Backlights
  // Telling the system the type, their data pin, what color order they are and how many there are;
  FastLED.addLeds<LED_TYPE, LEFT_CONSOLE_PIN, COLOUR_ORDER>(LEFT_CONSOLE_LED, LEFT_CONSOLE_LED_COUNT);
  FastLED.addLeds<LED_TYPE, RIGHT_CONSOLE_PIN, COLOUR_ORDER>(RIGHT_CONSOLE_LED, RIGHT_CONSOLE_LED_COUNT);
  FastLED.addLeds<LED_TYPE, LIP_UIP_PIN, COLOUR_ORDER>(LIP_UIP_CONSOLE_LED, LIP_UIP_CONSOLE_LED_COUNT);



  // The dimming method used. We have a large number of pixels so dithering is not ideal.
  FastLED.setDither(false);

  // Set the LEDs to their defined brightness.
  FastLED.setBrightness(startUpBrightness);

  // FastLED Power management set at 5V, and the defined current limit.
  FastLED.setMaxPowerInVoltsAndMilliamps(5, CURRENT_LIMIT);


  // Now apply everything we just told it about the setup.
  fill_solid( LEFT_CONSOLE_LED, LEFT_CONSOLE_LED_COUNT, CRGB::Green);
  fill_solid( RIGHT_CONSOLE_LED, RIGHT_CONSOLE_LED_COUNT, CRGB::Green);
  fill_solid( LIP_UIP_CONSOLE_LED, LIP_UIP_CONSOLE_LED_COUNT, CRGB::Red);

  FastLED.show();
  NEXT_LED_UPDATE = millis() + 1000;


  Keyboard.begin();
}






void Typeout(int packetLength) {

  Keyboard.print("Packet received - Length ");
  Keyboard.println(String(packetLength));

  String thisSet = "";
  for (int characterPtr = 0; characterPtr < packetLength ; characterPtr++ ) {
    Keyboard.print(packetBuffer[characterPtr]);
    if (String(packetBuffer[characterPtr]) == " ") {
      Keyboard.println("");
      Keyboard.press(KEY_LEFT_SHIFT);
      delay(delayBetweenRelease);
      Keyboard.println(thisSet);
      Keyboard.releaseAll();
      delay(delayBetweenRelease);

      thisSet = "";
    }
    else {
      thisSet = thisSet + String(packetBuffer[characterPtr]);
    }
  }
  if (thisSet != "") {
    Keyboard.println("");
    delay(delayBetweenRelease);
    Keyboard.press(KEY_LEFT_SHIFT);
    Keyboard.println(thisSet);
    Keyboard.releaseAll();
    delay(delayBetweenRelease);
  }

}

void TurnOnAPU(int packetLength) {

  // Now need to walk through received string - Modifers must be sent first, so build up
  // A list of them using a space as a delimiter

  bool leftAltInUse = false;
  String thisElement = "";
  char keyToPress[50];
  if (Serial_In_Use == 1)  {
    Serial.println("Packet Received");

    String thisSet = "";
    for (int characterPtr = 0; characterPtr < packetLength ; characterPtr++ ) {
      //Serial.print(packetBuffer[characterPtr]);
      if (String(packetBuffer[characterPtr]) == " ") {
        if (thisElement == "LALT") {
          leftAltInUse = true;
          Serial.println("Left Alt in Use");
        }
        thisElement = "";
      }
      else {
        thisElement = thisElement + String(packetBuffer[characterPtr]);
      }

    }
    Serial.println(thisElement);
  }


  if (leftAltInUse)
    Keyboard.press(KEY_LEFT_ALT);

  if (thisElement.length() == 1) {
    Serial.println("Correct length of Element");
    thisElement.toCharArray(keyToPress, 2);
    Serial.println(String(keyToPress[0]));
    Keyboard.press(keyToPress[0]);
  } else
    Keyboard.press('r');
  delay(delayBetweenRelease);
  Keyboard.releaseAll();

  if (Serial_In_Use == 1)  {
    Serial.println("Keys Released");
  }

}




void SendCharactersToKeyboard(int packetLength) {

  // Now need to walk through received string - Modifers must be sent first, so build up
  // A list of them using a space as a delimiter


  // Move these declaration to global when complete - still need to get flags on entry
  // change qualifiers for serial in use when complete - currently debuggin at all times

  altInUse = false;
  ctrlInUse = false;
  shiftInUse = false;
  leftAltInUse = false;
  rightAltInUse = false;
  lShiftInUse = false;
  rShiftInUse = false;
  lCtrlInUse = false;
  rCtrlInUse = false;
  lWinInUse = false;
  rWinInUse = false;

  String thisElement = "";
  char keyToPress[50];
  if (Serial_In_Use == 1)  {
    Serial.println("Packet Received");
    Serial.print("Len is ");
    Serial.println(packetLength);

    String thisSet = "";
    for (int characterPtr = 0; characterPtr < packetLength ; characterPtr++ ) {
      //Serial.print(packetBuffer[characterPtr]);

      // Build Out Modifier list
      // ModifiersOfInterest = ['ALT', 'CTRL', 'SHIFT', 'LSHIFT', 'RSHIFT', 'LCTRL', 'RCTRL', 'LALT', 'RALT', 'LWIN', 'RWIN' ]
      // Modifiers to do  [   , 'RWIN' ]
      // Also need to deal with Function Key or pretty much any key that is not a single character


      // We are delimiting by spaces
      if (String(packetBuffer[characterPtr]) == " ") {
        if (thisElement == "LALT") {
          leftAltInUse = true;
          Serial.println("Left Alt in Use");
        }
        else if (thisElement == "RALT") {
          rightAltInUse = true;
          Serial.println("Right Alt in Use");
        }
        else if (thisElement == "ALT") {
          altInUse = true;
          Serial.println("Alt in Use");
        }
        else if (thisElement == "CTRL") {
          ctrlInUse = true;
          Serial.println("Right Alt in Use");
        }
        else if (thisElement == "SHIFT") {
          shiftInUse = true;
          Serial.println("Shift in Use");
        }
        else if (thisElement == "LSHIFT") {
          lShiftInUse = true;
          Serial.println("Left Shift in Use");
        }
        else if (thisElement == "RSHIFT") {
          rShiftInUse = true;
          Serial.println("Right Shift in Use");
        }
        else if (thisElement == "LCTRL") {
          lCtrlInUse = true;
          Serial.println("Left Controlin Use");
        }
        else if (thisElement == "RCTRL") {
          rCtrlInUse = true;
          Serial.println("Right Control in Use");
        }
        else if (thisElement == "LWIN") {
          lWinInUse = true;
          Serial.println("Left Windows in Use");
        }
        else if (thisElement == "RWIN") {
          rWinInUse = true;
          Serial.println("Right Windows in Use");
        }
        thisElement = "";
      }

      else {
        thisElement = thisElement + String(packetBuffer[characterPtr]);
      }

    }
    Serial.println(thisElement);
  }



  // Hold  down the special/modifier keys
  if (altInUse)
    Keyboard.press(KEY_LEFT_ALT);
  if (ctrlInUse)
    Keyboard.press(KEY_LEFT_CTRL);
  if (shiftInUse)
    Keyboard.press(KEY_LEFT_SHIFT);


  if (leftAltInUse)
    Keyboard.press(KEY_LEFT_ALT);
  if (rightAltInUse)
    Keyboard.press(KEY_RIGHT_ALT);
  if (lShiftInUse)
    Keyboard.press(KEY_LEFT_SHIFT);
  if (rShiftInUse)
    Keyboard.press(KEY_RIGHT_SHIFT);
  if (lCtrlInUse)
    Keyboard.press(KEY_LEFT_CTRL);
  if (rCtrlInUse)
    Keyboard.press(KEY_RIGHT_CTRL);
  if (lWinInUse)
    Keyboard.press(KEY_LEFT_GUI);
  if (rWinInUse)
    Keyboard.press(KEY_RIGHT_GUI);




  // If the String includes a Carraige return at the end remove it
  // This can occur while sending test strings

  if (thisElement[thisElement.length() - 1] == 0x0A) {
    Serial.println("Found trailing CR - removing it");
    thisElement.remove(thisElement.length() - 1);
  }




  if (thisElement.length() == 1) {
    // We are hitting a single character to send
    Serial.println("Correct length of Element - Sending");
    Serial.println(thisElement);
    thisElement.toCharArray(keyToPress, 2);

    Keyboard.press(keyToPress[0]);

  } else
  {
    // Function Keys
    if (thisElement == "F1") {
      Serial.println(thisElement);
      Keyboard.press(KEY_F1);
    }
    else if (thisElement == "F2") {
      Serial.println(thisElement);
      Keyboard.press(KEY_F2);
    }
    else if (thisElement == "F3") {
      Serial.println(thisElement);
      Keyboard.press(KEY_F3);
    }
    else if (thisElement == "F4") {
      Serial.println(thisElement);
      Keyboard.press(KEY_F4);
    }
    else if (thisElement == "F5") {
      Serial.println(thisElement);
      Keyboard.press(KEY_F5);
    }
    else if (thisElement == "F6") {
      Serial.println(thisElement);
      Keyboard.press(KEY_F6);
    }
    else if (thisElement == "F7") {
      Serial.println(thisElement);
      Keyboard.press(KEY_F7);
    }
    else if (thisElement == "F8") {
      Serial.println(thisElement);
      Keyboard.press(KEY_F8);
    }
    else if (thisElement == "F9") {
      Serial.println(thisElement);
      Keyboard.press(KEY_F9);
    }
    else if (thisElement == "F10") {
      Serial.println(thisElement);
      Keyboard.press(KEY_F10);
    }
    else if (thisElement == "F11") {
      Serial.println(thisElement);
      Keyboard.press(KEY_F11);
    }
    else if (thisElement == "F12") {
      Serial.println(thisElement);
      Keyboard.press(KEY_F12);
    }




    // NUMBER PAD KEYS
    // Need to add 136 to the value otherwise keyboard.press will try ASCII look up
    // Reference https://forum.arduino.cc/t/keyboard-write-with-number-pad-keys-from-leonardo/175304
    //The keypad keys are 84 through 99 (0x54 through 0x63) but the keyboard.press() function will treat values below 128 (0x7F) as "printable" so it will look them up in a table of ascii keycodes. To get past that you have to add 136 to the keycode. Try these:

    // 220 '\334' Keypad /
    // 221 '\335' Keypad *
    // 222 '\336' Keypad -
    // 223 '\337' Keypad +
    // 224 '\340' Keypad ENTER

    else if (thisElement == "KEYPAD/") {
      Serial.println(thisElement);
      Keyboard.press(220);
    }
    else if (thisElement == "KEYPAD*") {
      Serial.println(thisElement);
      Keyboard.press(233);
    }
    else if (thisElement == "KEYPAD-") {
      Serial.println(thisElement);
      Keyboard.press(233);
    }
    else if (thisElement == "KEYPAD+") {
      Serial.println(thisElement);
      Keyboard.press(233);
    }
    else if (thisElement == "KEYPADENTER") {
      Serial.println(thisElement);
      Keyboard.press(233);
    }
    else if (thisElement == "ESC") {
      Serial.println(thisElement);
      Keyboard.press(KEY_ESC);
    }


    // 225 '\341' Keypad 1 and End
    // 226 '\342' Keypad 2 and Down Arrow
    // 227 '\343' Keypad 3 and PageDn
    // 228 '\344' Keypad 4 and Left Arrow
    // 229 '\345' Keypad 5
    // 230 '\346' Keypad 6 and Right Arrow
    // 231 '\347' Keypad 7 and Home
    // 232 '\350' Keypad 8 and Up Arrow
    // 233 '\351' Keypad 9 and PageUp
    // 234 '\352' Keypad 0 and Insert
    // 235 '\353' Keypad . and Delete

    else if (thisElement == "NUM0") {
      Serial.println(thisElement);
      Keyboard.press(234);
    }
    else if (thisElement == "NUM1") {
      Serial.println(thisElement);
      Keyboard.press(225);
    }
    else if (thisElement == "NUM2") {
      Serial.println(thisElement);
      Keyboard.press(226);
    }
    else if (thisElement == "NUM3") {
      Serial.println(thisElement);
      Keyboard.press(227);
    }
    else if (thisElement == "NUM4") {
      Serial.println(thisElement);
      Keyboard.press(228);
    }
    else if (thisElement == "NUM5") {
      Serial.println(thisElement);
      Keyboard.press(229);
    }
    else if (thisElement == "NUM6") {
      Serial.println(thisElement);
      Keyboard.press(230);
    }
    else if (thisElement == "NUM7") {
      Serial.println(thisElement);
      Keyboard.press(231);
    }
    else if (thisElement == "NUM8") {
      Serial.println(thisElement);
      Keyboard.press(232);
    }
    else if (thisElement == "NUM9") {
      Serial.println(thisElement);
      Keyboard.press(233);
    }


    else {
      Serial.println("Incorrect length of Element");
      Serial.print(Serial.println(thisElement));
    }
  }




  //Key    Hexadecimal value   Decimal value
  //KEY_UP_ARROW  0xDA  218
  //KEY_DOWN_ARROW  0xD9  217
  //KEY_LEFT_ARROW  0xD8  216
  //KEY_RIGHT_ARROW   0xD7  215
  //KEY_BACKSPACE   0xB2  178
  //KEY_TAB   0xB3  179
  //KEY_RETURN  0xB0  176
  //KEY_ESC   0xB1  177
  //KEY_INSERT  0xD1  209
  //KEY_DELETE  0xD4  212
  //KEY_PAGE_UP   0xD3  211
  //KEY_PAGE_DOWN   0xD6  214
  //KEY_HOME  0xD2  210
  //KEY_END   0xD5  213
  //KEY_CAPS_LOCK   0xC1  193



  delay(delayBetweenRelease);

  Keyboard.releaseAll();

  if (Serial_In_Use == 1)  {
    Serial.println("Keys Released");
  }

}



void UpdateGeneralBacklight(int packetLength) {

  // Read up either to first space or packet len
  // Check it is a number and less than max brightness
  // Update Brightness for all initisally and then reduce to just console ports

  String thisElement = "";
  char keyToPress[50];
  int localBrightness;

  thisElement = String(backlightpacketBuffer);


  if (Serial_In_Use == 1)  {
    Serial.println("Packet is " + thisElement);
    if (thisElement[0] == '0') {
      Serial.println("Found a zero");
      FastLED.setBrightness(0);
      FastLED.show();
    }
    else {
      localBrightness = thisElement.toInt();
      if (localBrightness != 0) {
        if (localBrightness >= MAX_BRIGHTNESS) localBrightness = MAX_BRIGHTNESS;
        FastLED.setBrightness(localBrightness);
        FastLED.show();
      }
    }


  }
}


void UpdateLIPUIP(int packetLength) {

  // Takes a single attirbute value pair and sets the levels
  // Only expecting changes

  String thisElement = "";
  char keyToPress[50];
  int localBrightness;

  thisElement = String(backlightpacketBuffer);

  // Check to see if there is a ':' present - if not discard
  // The first attribute is expected to be a string
  // The second attribute should be a number not a string
  // Test but just changing the colour of the first led
  // For performance reasons hold onto the update for 30 or so milliseconds before updating

  if (Serial_In_Use == 1)  {
    Serial.println("Packet is " + thisElement);
    if (thisElement[0] == '0') {
      Serial.println("Found a zero");
      FastLED.setBrightness(0);
      FastLED.show();
    }
    else {
      localBrightness = thisElement.toInt();
      if (localBrightness != 0) {
        if (localBrightness >= MAX_BRIGHTNESS) localBrightness = MAX_BRIGHTNESS;
        FastLED.setBrightness(localBrightness);
        FastLED.show();
      }
    }


  }
}




void ProcessReceivedString()
{

  // Reading values from packetBuffer which is global
  // All received values are strings for readability
  // From 737 Project

  bool bLocalDebug = true;
  int localBrightness = 0;

  if (Debug_Display || bLocalDebug ) Serial.println("Processing Packet");


  String sWrkStr = "";

  const char *delim  = "=";



  ParameterNamePtr = strtok(forwardlightpacketBuffer, delim);
  String ParameterNameString(ParameterNamePtr);
  if (Debug_Display || bLocalDebug ) Serial.println("Parameter Name " + ParameterNameString);

  ParameterValuePtr   = strtok(NULL, delim);
  String ParameterValue(ParameterValuePtr);
  if (Debug_Display || bLocalDebug ) Serial.println("Parameter Value " + ParameterValue);

  // Handle the following attribute types
  //int instDim; // Consoles Dimmer Knob Value - Via DCS
  //int conDim;
  //int cautDim; // Caution Dimmer Knob Value - Via DCS
  //int spinLT; //Spin Light Dimmer Value
  //int spinOn; // Spin Light On or Off
  //int consSW; // NVG/NITE/DAY Switch
  //int ecmLT; // EMC JETT LIGHT (GREEN)
  //int ecmON; // EMC Light ON (GREEN)
  //int aaLT; // AA light MASTER ARM
  //int aaON; // AA light MASTER ARM
  //int agLT; // AG light MASTER ARM
  //int agON; // AG light MASTER ARM
  //int rdyLT; // READY light MASTER ARM
  //int rdyON; // READY light MASTER ARM
  //int disLT; // DISCH light MASTER ARM
  //int disON; // DISCH light MASTER ARM



  if (ParameterNameString.equalsIgnoreCase("Brightness")) {
    Serial.println("Found Brightness");
    localBrightness = ParameterValue.toInt();
    if (localBrightness >= MAX_BRIGHTNESS) localBrightness = MAX_BRIGHTNESS;
    FastLED.setBrightness(localBrightness);
    FastLED.show();
  }


  if (ParameterNameString.equalsIgnoreCase("aaLT")) {
    Serial.println("Found aaLT");
    aaLT = ParameterValue.toInt();
    if (aaLT==1) fill_solid( LIP_UIP_CONSOLE_LED, LIP_UIP_CONSOLE_LED_COUNT, CRGB::Green);
    else if (aaLT==0) fill_solid( LIP_UIP_CONSOLE_LED, LIP_UIP_CONSOLE_LED_COUNT, CRGB::Red);

    
    LIP_UIP_CONSOLE_LED[5] = CRGB::Yellow; 
    
    FastLED.setBrightness(MAX_BRIGHTNESS);
    FastLED.show(); 
  }

  
  if (ParameterNameString.equalsIgnoreCase("LEDON")) {
    Serial.println("Found LedOn");
    aaLT = ParameterValue.toInt();

    if (aaLT == 0) {
      Serial.println("Processing a 0");
      LIP_UIP_CONSOLE_LED[0] = CRGB::Yellow;
    }
    else if (aaLT!=0) {
      Serial.println("Processing a non-0");
      LIP_UIP_CONSOLE_LED[aaLT] = CRGB::Yellow;
    }
    
    FastLED.setBrightness(MAX_BRIGHTNESS);
    FastLED.show(); 
  }
  
}




void loop() {


  // Turn Leds off after initial start up in setup
  if ((millis() >= NEXT_LED_UPDATE) && (startUpBrightness != 0)) {
    NEXT_LED_UPDATE = millis() + 10;

    startUpBrightness--;
    FastLED.setBrightness(startUpBrightness);
    FastLED.show();
  }


  // Handle Keyboard updates
  packetSize = udp.parsePacket();
  len = udp.read(packetBuffer, 999);

  if (len > 0) {
    packetBuffer[len] = 0;
  }

  if (packetSize) {
    SendCharactersToKeyboard(len);
  }



  // Backlighting
  backlightPacketSize = backlightudp.parsePacket();
  backlightLen = backlightudp.read(backlightpacketBuffer, 999);

  if (backlightLen > 0) {
    backlightpacketBuffer[backlightLen] = 0;
  }

  if (backlightPacketSize) {
    UpdateGeneralBacklight(backlightLen);
  }


  forwardlightPacketSize = forwardlightudp.parsePacket();
  forwardlightLen = forwardlightudp.read(forwardlightpacketBuffer, 999);

  if (forwardlightLen > 0) {
    forwardlightpacketBuffer[forwardlightLen] = 0;
  }

  if (forwardlightPacketSize) {
    //UpdateLIPUIP(forwardlightLen);
    ProcessReceivedString();

  }



}
