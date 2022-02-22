////////////////////---||||||||||********||||||||||---\\\\\\\\\\\\\\\\\\\\
//||               FUNCTION = HORNET UDP to Keyboard and Pixel LED     ||\\
//||              LOCATION IN THE PIT = LIP LEFT HAND SIDE             ||\\
//||            ARDUINO PROCESSOR TYPE = Arduino Mega 2560            ||\\
//||      ARDUINO CHIP SERIAL NUMBER = SN: 95433343733351201290       ||\\
//||      ETHERNET SHEILD MAC ADDRESS = MAC                           ||\\
//||                    CONNECTED COM PORT = COM 6                    ||\\
//||               ****ADD ASSIGNED COM PORT NUMBER****               ||\\
//||            ****DO CHECK S/N BEFORE UPLOAD NEW DATA****           ||\\
////////////////////---||||||||||********||||||||||---\\\\\\\\\\\\\\\\\\\\

/*
    KNOWN ISSUE - NEED TO UNPLUG NATIVE USB PORT WHEN PROGRAMMING
*/

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
#define Reflector_In_Use 0



// ###################################### Begin Pixel Led Related #############################

// PixelLighting
#include <FastLED.h>
String COLOUR   =  "GREEN";         // The color name that you want to show, e.g. Green, Red, Blue, White
int startUpBrightness =   50;       // LED Brightness 0 = Off, 255 = 100%.
#define MAX_BRIGHTNESS 255          // This is relative to master used with CHSV
#define MAX_MASTER_BRIGHTNESS 100   // Overrides all brightness - used with setbrightness method

// Set your power supplies 5V current limit.

#define CURRENT_LIMIT   20000   // Current in mA (1000mA = 1 Amp). Most ATX PSUs provide 20A maximum.

// Defining how many pixels each backlighting connector has connected, if a connector is not used set it to zero.
// Led Counts for LIP and UIP Panels
#define ECM_JETT_LED_COUNT      78
#define VIDEO_RECORD_LED_COUNT  16
#define PLACARD_LED_COUNT       8
#define MASTER_ARM_LED_COUNT    29
#define HUD_CONTROL_LED_COUNT   56
#define SPIN_RECOVERY_LED_COUNT 53

#define LEFT_CONSOLE_LED_COUNT 500
#define RIGHT_CONSOLE_LED_COUNT 500
const int  LIP_CONSOLE_LED_COUNT = ECM_JETT_LED_COUNT + VIDEO_RECORD_LED_COUNT + PLACARD_LED_COUNT;
const int  UIP_CONSOLE_LED_COUNT = MASTER_ARM_LED_COUNT + HUD_CONTROL_LED_COUNT + SPIN_RECOVERY_LED_COUNT;

// Defining what data pin each backlighting connector is connected to.

// If using small test rig
// The Max7219 connector uses Pin 14,15,16
// Order on connector is 5V GND 16 15 14 Last pin is not connected

#define UIP_PIN                 14
#define LIP_PIN                 15
// Not used as locking collides 44
// Not used as locking collides 46
#define LEFT_CONSOLE_PIN        16
#define RIGHT_CONSOLE_PIN       42



// Some other setup information. Don't change these unless you have a reason to.

#define LED_TYPE     WS2812B  // OPENHORNET backlighting LEDs are WS2812B
#define COLOUR_ORDER GRB      // OPENHORNET backlighting LEDs are GRB (green, red, blue)
#define SOLID_SPEED  100     // The refresh rate delay in ms. Leave this at around 1000 (1 second)

// Setting up the blocks of memory that will be used for storing and manipulating the led data;

CRGB LEFT_CONSOLE_LED[LEFT_CONSOLE_LED_COUNT];
CRGB RIGHT_CONSOLE_LED[RIGHT_CONSOLE_LED_COUNT];
CRGB LIP_CONSOLE_LED[LIP_CONSOLE_LED_COUNT];
CRGB UIP_CONSOLE_LED[UIP_CONSOLE_LED_COUNT];

#define CHSVRed   0
#define CHSVGreen 96
#define CHSVYellow 45

unsigned long NEXT_LED_UPDATE = 0;

// Indicator Variables
bool ECM_JET = false;
bool MASTER_ARM_DISCH_READY = false;
bool MASTER_ARM_DISCH = false;
bool MASTER_ARM_AA = true;
bool MASTER_ARM_AG = false;
bool SPIN = false;


int ledptr = 0;
int consoleBrightness = 50;                     // Global Value for Console Brightness
int indicatorBrightness = 50;                   // Global value for Indicator Brightness
unsigned long timeBeforeNextLedUpdate = 0;
unsigned long minTimeBetweenLedUpdates = 40;    // Provides time foir several updates to be put together before throwing to the led strings
bool LedUpdateNeeded = false;                   // Flags if we have something to update


// The Panels are chained so calculate starting position
// LIP Panel wiring ECM -> Video Record/IFEI -> Placard
// UIP Panel wiring Master Arm -> Hud Control -> Spin Recovery
// Two chains are required for UIP/LIP as the Jet Station Select Placard and
//    (as of the version 1) the Spin Recovery do not pass the data signal through

const int ECM_JET_START_POS       = 0;
const int VID_RECORD_START_POS    = ECM_JETT_LED_COUNT;
const int PLACARD_LED_START_POS   = VID_RECORD_START_POS + VIDEO_RECORD_LED_COUNT;

const int MASTER_ARM_START_POS    = 0;
const int HUD_CONTROL_START_POS   = MASTER_ARM_LED_COUNT;
const int SPIN_RECOVERY_START_POS = HUD_CONTROL_START_POS + HUD_CONTROL_LED_COUNT;


// Special Led Positions on LIP and UIP Panels
#define MASTER_ARM_READY_1 0
#define MASTER_ARM_READY_2 2
#define MASTER_ARM_DISCH_1 1
#define MASTER_ARM_DISCH_2 3
#define MASTER_ARM_AA_1 25
#define MASTER_ARM_AA_2 26
#define MASTER_ARM_AG_1 27
#define MASTER_ARM_AG_2 28


// ECM Testing
#define ECM_JETT_1 1
#define ECM_JETT_2 5
#define ECM_JETT_3 10
#define ECM_JETT_4 11


#define SPIN_1 29
#define SPIN_2 36


// ###################################### End Pixel Led Related #############################



// ###################################### Begin Keyboard Related #############################

#include <Keyboard.h>

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

const int delayBetweenRelease = 200;          // How long is a key held down for - WARNING THIS CURRENTLY BLOCKS THE RUNNING OF OTHER CODE
// This could be optimised to set a flag and clear during the loop
unsigned long timeBeforeReleaseAllKeys = 0;
bool  releaseKeysNeeded = false;

// ###################################### End Keyboard Related #############################




// ###################################### Begin Ethernet Related #############################
#include <SPI.h>
#include <Ethernet.h>
#include <EthernetUdp.h>

// These local Mac and IP Address will be reassigned early in startup based on
// the device ID as set by address pins
#define EthernetStartupDelay 500
byte mac[] = {0x00, 0xDD, 0x3E, 0xCA, 0x37, 0x99};
IPAddress ip(172, 16, 1, 110);
String strMyIP = "X.X.X.X";

// Reflector
IPAddress reflectorIP(172, 16, 1, 10);
String strReflectorIP = "X.X.X.X";

const unsigned int keyboardport = 7788;
const unsigned int ledport = 7789;
const unsigned int remoteport = 49000;
const unsigned int reflectorport = 27000;


// Packet Length
int keyboardpacketSize;
int keyboardLen;
int ledPacketSize;
int ledLen;

EthernetUDP keyboardudp;              // Keyboard
EthernetUDP ledudp;                   //Left and Right Consoles


char keyboardpacketBuffer[1000];      //buffer to store keyboard data
char ledpacketBuffer[1000];           //buffer to store led data
char outpacketBuffer[1000];           //buffer to store the outgoing data


// ###################################### End Ethernet Related #############################









// Variables picked up from 737 Code
bool Debug_Display = true;
char *ParameterNamePtr;
char *ParameterValuePtr;

void setup() {

  if (Serial_In_Use) {
    Serial.begin(250000);
    Serial.println("UDP to Keyboard Startup");
  }


  if (Ethernet_In_Use == 1) {
    delay(EthernetStartupDelay);
    Ethernet.begin( mac, ip);

    keyboardudp.begin( keyboardport );


    if (Reflector_In_Use == 1) {
      keyboardudp.beginPacket(reflectorIP, reflectorport);
      keyboardudp.println("Init UDP Keyboard and Led - " + strMyIP + " " + String(millis()) + "mS since reset.");
      keyboardudp.endPacket();
    }

    ledudp.begin(ledport);


  }

  // Activate Backlights
  // Telling the system the type, their data pin, what color order they are and how many there are;
  FastLED.addLeds<LED_TYPE, LEFT_CONSOLE_PIN, COLOUR_ORDER>(LEFT_CONSOLE_LED, LEFT_CONSOLE_LED_COUNT);
  FastLED.addLeds<LED_TYPE, RIGHT_CONSOLE_PIN, COLOUR_ORDER>(RIGHT_CONSOLE_LED, RIGHT_CONSOLE_LED_COUNT);
  FastLED.addLeds<LED_TYPE, LIP_PIN, COLOUR_ORDER>(LIP_CONSOLE_LED, LIP_CONSOLE_LED_COUNT);
  FastLED.addLeds<LED_TYPE, UIP_PIN, COLOUR_ORDER>(UIP_CONSOLE_LED, UIP_CONSOLE_LED_COUNT);




  // The dimming method used. We have a large number of pixels so dithering is not ideal.
  FastLED.setDither(false);

  // Set the LEDs to their defined brightness.
  FastLED.setBrightness(startUpBrightness);

  // FastLED Power management set at 5V, and the defined current limit.
  FastLED.setMaxPowerInVoltsAndMilliamps(5, CURRENT_LIMIT);


  // Now apply everything we just told it about the setup.
  fill_solid( LEFT_CONSOLE_LED, LEFT_CONSOLE_LED_COUNT, CRGB::Green);
  fill_solid( RIGHT_CONSOLE_LED, RIGHT_CONSOLE_LED_COUNT, CRGB::Green);
  fill_solid( LIP_CONSOLE_LED, LIP_CONSOLE_LED_COUNT, CRGB::Green);
  fill_solid( UIP_CONSOLE_LED, UIP_CONSOLE_LED_COUNT, CRGB::Green);

  FastLED.show();
  delay(2000);
  fill_solid( LEFT_CONSOLE_LED, LEFT_CONSOLE_LED_COUNT, CRGB::Red);
  fill_solid( RIGHT_CONSOLE_LED, RIGHT_CONSOLE_LED_COUNT, CRGB::Red);
  fill_solid( LIP_CONSOLE_LED, LIP_CONSOLE_LED_COUNT, CRGB::Red);
  fill_solid( UIP_CONSOLE_LED, UIP_CONSOLE_LED_COUNT, CRGB::Red);

  FastLED.show();
  delay(2000);


    // Now apply everything we just told it about the setup.
  fill_solid( LEFT_CONSOLE_LED, LEFT_CONSOLE_LED_COUNT, CRGB::Yellow);
  fill_solid( RIGHT_CONSOLE_LED, RIGHT_CONSOLE_LED_COUNT, CRGB::Yellow);
  fill_solid( LIP_CONSOLE_LED, LIP_CONSOLE_LED_COUNT, CRGB::Yellow);
  fill_solid( UIP_CONSOLE_LED, UIP_CONSOLE_LED_COUNT, CRGB::Yellow);

  FastLED.show();
  delay(2000);

  
  fill_solid( LEFT_CONSOLE_LED, LEFT_CONSOLE_LED_COUNT, CRGB::Green);
  fill_solid( RIGHT_CONSOLE_LED, RIGHT_CONSOLE_LED_COUNT, CRGB::Green);
  fill_solid( LIP_CONSOLE_LED, LIP_CONSOLE_LED_COUNT, CRGB::Green);
  fill_solid( UIP_CONSOLE_LED, UIP_CONSOLE_LED_COUNT, CRGB::Green);

  FastLED.show();
  delay(1000);
  NEXT_LED_UPDATE = millis() + 1000;


  Keyboard.begin();
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

  bool bLocalDebug = false;

  if ((Debug_Display || bLocalDebug ) && Serial_In_Use) Serial.println("Packet Received");
  if ((Debug_Display || bLocalDebug ) && Serial_In_Use) Serial.print("Len is ");
  if ((Debug_Display || bLocalDebug ) && Serial_In_Use) Serial.println(packetLength);

  String thisSet = "";
  for (int characterPtr = 0; characterPtr < packetLength ; characterPtr++ ) {
    //Serial.print(packetBuffer[characterPtr]);

    // Build Out Modifier list
    // ModifiersOfInterest = ['ALT', 'CTRL', 'SHIFT', 'LSHIFT', 'RSHIFT', 'LCTRL', 'RCTRL', 'LALT', 'RALT', 'LWIN', 'RWIN' ]
    // Modifiers to do  [   , 'RWIN' ]
    // Also need to deal with Function Key or pretty much any key that is not a single character


    // We are delimiting by spaces
    if (String(keyboardpacketBuffer[characterPtr]) == " ") {
      if (thisElement == "LALT") {
        leftAltInUse = true;
        if ((Debug_Display || bLocalDebug ) && Serial_In_Use) Serial.println("Left Alt in Use");
      }
      else if (thisElement == "RALT") {
        rightAltInUse = true;
        if ((Debug_Display || bLocalDebug ) && Serial_In_Use) Serial.println("Right Alt in Use");
      }
      else if (thisElement == "ALT") {
        altInUse = true;
        if ((Debug_Display || bLocalDebug ) && Serial_In_Use) Serial.println("Alt in Use");
      }
      else if (thisElement == "CTRL") {
        ctrlInUse = true;
        if ((Debug_Display || bLocalDebug ) && Serial_In_Use) Serial.println("Right Alt in Use");
      }
      else if (thisElement == "SHIFT") {
        shiftInUse = true;
        if ((Debug_Display || bLocalDebug ) && Serial_In_Use) Serial.println("Shift in Use");
      }
      else if (thisElement == "LSHIFT") {
        lShiftInUse = true;
        if ((Debug_Display || bLocalDebug ) && Serial_In_Use) Serial.println("Left Shift in Use");
      }
      else if (thisElement == "RSHIFT") {
        rShiftInUse = true;
        if ((Debug_Display || bLocalDebug ) && Serial_In_Use) Serial.println("Right Shift in Use");
      }
      else if (thisElement == "LCTRL") {
        lCtrlInUse = true;
        if ((Debug_Display || bLocalDebug ) && Serial_In_Use) Serial.println("Left Controlin Use");
      }
      else if (thisElement == "RCTRL") {
        rCtrlInUse = true;
        if ((Debug_Display || bLocalDebug ) && Serial_In_Use) Serial.println("Right Control in Use");
      }
      else if (thisElement == "LWIN") {
        lWinInUse = true;
        if ((Debug_Display || bLocalDebug ) && Serial_In_Use) Serial.println("Left Windows in Use");
      }
      else if (thisElement == "RWIN") {
        rWinInUse = true;
        if ((Debug_Display || bLocalDebug ) && Serial_In_Use) Serial.println("Right Windows in Use");
      }
      thisElement = "";
    }

    else {
      thisElement = thisElement + String(keyboardpacketBuffer[characterPtr]);
    }

  }
  if ((Debug_Display || bLocalDebug ) && Serial_In_Use) Serial.println(thisElement);




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




  // If the String includes a Carriage return at the end remove it
  // This can occur while sending test strings

  if (thisElement[thisElement.length() - 1] == 0x0A) {
    if ((Debug_Display || bLocalDebug ) && Serial_In_Use) Serial.println("Found trailing CR - removing it");
    thisElement.remove(thisElement.length() - 1);
  }




  if (thisElement.length() == 1) {
    // We are hitting a single character to send
    if ((Debug_Display || bLocalDebug ) && Serial_In_Use) Serial.println("Correct length of Element - Sending");
    if ((Debug_Display || bLocalDebug ) && Serial_In_Use) Serial.println(thisElement);
    thisElement.toCharArray(keyToPress, 2);

    Keyboard.press(keyToPress[0]);

  } else
  {
    // Function Keys
    if (thisElement == "F1") {
      if ((Debug_Display || bLocalDebug ) && Serial_In_Use) Serial.println(thisElement);
      Keyboard.press(KEY_F1);
    }
    else if (thisElement == "F2") {
      if ((Debug_Display || bLocalDebug ) && Serial_In_Use) Serial.println(thisElement);
      Keyboard.press(KEY_F2);
    }
    else if (thisElement == "F3") {
      if ((Debug_Display || bLocalDebug ) && Serial_In_Use) Serial.println(thisElement);
      Keyboard.press(KEY_F3);
    }
    else if (thisElement == "F4") {
      if ((Debug_Display || bLocalDebug ) && Serial_In_Use) Serial.println(thisElement);
      Keyboard.press(KEY_F4);
    }
    else if (thisElement == "F5") {
      if ((Debug_Display || bLocalDebug ) && Serial_In_Use) Serial.println(thisElement);
      Keyboard.press(KEY_F5);
    }
    else if (thisElement == "F6") {
      if ((Debug_Display || bLocalDebug ) && Serial_In_Use) Serial.println(thisElement);
      Keyboard.press(KEY_F6);
    }
    else if (thisElement == "F7") {
      if ((Debug_Display || bLocalDebug ) && Serial_In_Use) Serial.println(thisElement);
      Keyboard.press(KEY_F7);
    }
    else if (thisElement == "F8") {
      if ((Debug_Display || bLocalDebug ) && Serial_In_Use) Serial.println(thisElement);
      Keyboard.press(KEY_F8);
    }
    else if (thisElement == "F9") {
      if ((Debug_Display || bLocalDebug ) && Serial_In_Use) Serial.println(thisElement);
      Keyboard.press(KEY_F9);
    }
    else if (thisElement == "F10") {
      if ((Debug_Display || bLocalDebug ) && Serial_In_Use) Serial.println(thisElement);
      Keyboard.press(KEY_F10);
    }
    else if (thisElement == "F11") {
      if ((Debug_Display || bLocalDebug ) && Serial_In_Use) Serial.println(thisElement);
      Keyboard.press(KEY_F11);
    }
    else if (thisElement == "F12") {
      if ((Debug_Display || bLocalDebug ) && Serial_In_Use) Serial.println(thisElement);
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
      if ((Debug_Display || bLocalDebug ) && Serial_In_Use) Serial.println(thisElement);
      Keyboard.press(220);
    }
    else if (thisElement == "KEYPAD*") {
      if ((Debug_Display || bLocalDebug ) && Serial_In_Use) Serial.println(thisElement);
      Keyboard.press(233);
    }
    else if (thisElement == "KEYPAD-") {
      if ((Debug_Display || bLocalDebug ) && Serial_In_Use) Serial.println(thisElement);
      Keyboard.press(233);
    }
    else if (thisElement == "KEYPAD+") {
      if ((Debug_Display || bLocalDebug ) && Serial_In_Use) Serial.println(thisElement);
      Keyboard.press(233);
    }
    else if (thisElement == "KEYPADENTER") {
      if ((Debug_Display || bLocalDebug ) && Serial_In_Use) Serial.println(thisElement);
      Keyboard.press(233);
    }
    else if (thisElement == "ESC") {
      if ((Debug_Display || bLocalDebug ) && Serial_In_Use) Serial.println(thisElement);
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
      if ((Debug_Display || bLocalDebug ) && Serial_In_Use) Serial.println(thisElement);
      Keyboard.press(234);
    }
    else if (thisElement == "NUM1") {
      if ((Debug_Display || bLocalDebug ) && Serial_In_Use) Serial.println(thisElement);
      Keyboard.press(225);
    }
    else if (thisElement == "NUM2") {
      if ((Debug_Display || bLocalDebug ) && Serial_In_Use) Serial.println(thisElement);
      Keyboard.press(226);
    }
    else if (thisElement == "NUM3") {
      if ((Debug_Display || bLocalDebug ) && Serial_In_Use) Serial.println(thisElement);
      Keyboard.press(227);
    }
    else if (thisElement == "NUM4") {
      if ((Debug_Display || bLocalDebug ) && Serial_In_Use) Serial.println(thisElement);
      Keyboard.press(228);
    }
    else if (thisElement == "NUM5") {
      if ((Debug_Display || bLocalDebug ) && Serial_In_Use) Serial.println(thisElement);
      Keyboard.press(229);
    }
    else if (thisElement == "NUM6") {
      if ((Debug_Display || bLocalDebug ) && Serial_In_Use) Serial.println(thisElement);
      Keyboard.press(230);
    }
    else if (thisElement == "NUM7") {
      if ((Debug_Display || bLocalDebug ) && Serial_In_Use) Serial.println(thisElement);
      Keyboard.press(231);
    }
    else if (thisElement == "NUM8") {
      if ((Debug_Display || bLocalDebug ) && Serial_In_Use) Serial.println(thisElement);
      Keyboard.press(232);
    }
    else if (thisElement == "NUM9") {
      if ((Debug_Display || bLocalDebug ) && Serial_In_Use) Serial.println(thisElement);
      Keyboard.press(233);
    }


    else {
      if ((Debug_Display || bLocalDebug ) && Serial_In_Use) Serial.println("Incorrect length of Element");
      if ((Debug_Display || bLocalDebug ) && Serial_In_Use) Serial.println(thisElement);
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


  releaseKeysNeeded = true;
  timeBeforeReleaseAllKeys = millis() + delayBetweenRelease;


}




void SetBacklighting()
{

  bool bLocalDebug = false;

  if ((Debug_Display || bLocalDebug ) && Serial_In_Use)  Serial.println("Entering SetBacklighting");
  if ((Debug_Display || bLocalDebug ) && Serial_In_Use)  Serial.println("Console Brightness: " + String(consoleBrightness));

  // Left and Right Consoles are entirely flood so nothing special needed there
  // Forward console has exceptions

  for (ledptr = 0; ledptr <= (LEFT_CONSOLE_LED_COUNT - 1); ledptr++) {
    LEFT_CONSOLE_LED[ledptr] = CHSV( CHSVGreen, 255, consoleBrightness);
  }




  for (ledptr = 0; ledptr <= (LEFT_CONSOLE_LED_COUNT - 1); ledptr++) {
    RIGHT_CONSOLE_LED[ledptr] = CHSV( CHSVGreen, 255, consoleBrightness);
  }


  // LIP and UIP have exceptions - so walk through panel by panel
  // LIP Panel wiring ECM -> Video Record/IFEI -> Placard
  // UIP Panel wiring Master Arm -> Hud Control -> Spin Recovery
  // Panel Led Positions are defined above


  // ******************************************************************************
  // LIP
  // ECM

  for (ledptr = ECM_JET_START_POS;
       ledptr <= (ECM_JET_START_POS + ECM_JETT_LED_COUNT - 1); ledptr++) {
    if (ledptr != ECM_JETT_1 && ledptr !=  ECM_JETT_2 &&
        ledptr != ECM_JETT_3 && ledptr !=  ECM_JETT_4 )
      LIP_CONSOLE_LED[ledptr] = CHSV( CHSVGreen, 255, consoleBrightness);
  }


  // Video Record
  for (ledptr = VID_RECORD_START_POS;
       ledptr <= (VID_RECORD_START_POS + VIDEO_RECORD_LED_COUNT  - 1); ledptr++) {
    // There are no special function leds - so no check needed
    LIP_CONSOLE_LED[ledptr] = CHSV( CHSVGreen, 255, consoleBrightness);
  }

  // Placard
  for (ledptr = PLACARD_LED_START_POS;
       ledptr <= (PLACARD_LED_START_POS + PLACARD_LED_COUNT  - 1); ledptr++) {
    // There are no special function leds - so no check needed
    LIP_CONSOLE_LED[ledptr] = CHSV( CHSVGreen, 255, consoleBrightness);
  }


  // ******************************************************************************

  // UIP
  // MASTER ARM
  for (ledptr = MASTER_ARM_START_POS;
       ledptr <= (MASTER_ARM_START_POS + MASTER_ARM_LED_COUNT - 1); ledptr++) {
    if (ledptr != MASTER_ARM_READY_1 && ledptr != MASTER_ARM_READY_2 &&
        ledptr != MASTER_ARM_DISCH_1 && ledptr != MASTER_ARM_DISCH_2 &&
        ledptr != MASTER_ARM_AA_1 && ledptr != MASTER_ARM_AA_2  &&
        ledptr != MASTER_ARM_AG_1 && ledptr != MASTER_ARM_AG_2)
      UIP_CONSOLE_LED[ledptr] = CHSV( CHSVGreen, 255, consoleBrightness);
  }


  // HUD CONTROL
  for (ledptr = HUD_CONTROL_START_POS;
       ledptr <= (HUD_CONTROL_START_POS + HUD_CONTROL_LED_COUNT  - 1); ledptr++) {
    // There are no special function leds - so no check needed
    UIP_CONSOLE_LED[ledptr] = CHSV( CHSVGreen, 255, consoleBrightness);
  }


  // SPIN
  for (ledptr = SPIN_RECOVERY_START_POS;
       ledptr <= (SPIN_RECOVERY_START_POS + SPIN_RECOVERY_LED_COUNT  - 1); ledptr++) {
    // Check to see if it is a special led - if it isn't adjust brightness
    if (ledptr != SPIN_1 && ledptr != SPIN_2)
      UIP_CONSOLE_LED[ledptr] = CHSV( CHSVGreen, 255, consoleBrightness);
  }

  if (Debug_Display || bLocalDebug ) Serial.println("Exiting SetBacklighting");
}


void Update_ECMJet() {

  if (ECM_JET == true) {
    LIP_CONSOLE_LED[ECM_JET_START_POS + ECM_JETT_1 ] = CHSV( CHSVGreen, 255, indicatorBrightness);
    LIP_CONSOLE_LED[ECM_JET_START_POS + ECM_JETT_2 ] = CHSV( CHSVGreen, 255, indicatorBrightness);
    LIP_CONSOLE_LED[ECM_JET_START_POS + ECM_JETT_3 ] = CHSV( CHSVGreen, 255, indicatorBrightness);
    LIP_CONSOLE_LED[ECM_JET_START_POS + ECM_JETT_4 ] = CHSV( CHSVGreen, 255, indicatorBrightness);
  } else {
    LIP_CONSOLE_LED[ECM_JET_START_POS + ECM_JETT_1 ] = CHSV( CHSVGreen, 255, 0);
    LIP_CONSOLE_LED[ECM_JET_START_POS + ECM_JETT_2 ] = CHSV( CHSVGreen, 255, 0);
    LIP_CONSOLE_LED[ECM_JET_START_POS + ECM_JETT_3 ] = CHSV( CHSVGreen, 255, 0);
    LIP_CONSOLE_LED[ECM_JET_START_POS + ECM_JETT_4 ] = CHSV( CHSVGreen, 255, 0);
  }
  LedUpdateNeeded = true;
}


void Update_MASTER_ARM_DISCH_READY() {

  if (MASTER_ARM_DISCH_READY == true) {
    UIP_CONSOLE_LED[MASTER_ARM_START_POS + MASTER_ARM_READY_1 ] = CHSV( CHSVYellow, 255, indicatorBrightness);
    UIP_CONSOLE_LED[MASTER_ARM_START_POS + MASTER_ARM_READY_2 ] = CHSV( CHSVYellow, 255, indicatorBrightness);
  } else {
    UIP_CONSOLE_LED[MASTER_ARM_START_POS + MASTER_ARM_READY_1 ] = CHSV( CHSVYellow, 255, 0);
    UIP_CONSOLE_LED[MASTER_ARM_START_POS + MASTER_ARM_READY_2 ] = CHSV( CHSVYellow, 255, 0);
  }
  LedUpdateNeeded = true;
}


void Update_MASTER_ARM_DISCH() {

  if (MASTER_ARM_DISCH == true) {
    UIP_CONSOLE_LED[MASTER_ARM_START_POS + MASTER_ARM_DISCH_1 ] = CHSV( CHSVGreen, 255, indicatorBrightness);
    UIP_CONSOLE_LED[MASTER_ARM_START_POS + MASTER_ARM_DISCH_2 ] = CHSV( CHSVGreen, 255, indicatorBrightness);
  } else {
    UIP_CONSOLE_LED[MASTER_ARM_START_POS + MASTER_ARM_DISCH_1 ] = CHSV( CHSVGreen, 255, 0);
    UIP_CONSOLE_LED[MASTER_ARM_START_POS + MASTER_ARM_DISCH_2 ] = CHSV( CHSVGreen, 255, 0);
  }
  LedUpdateNeeded = true;
}

void Update_MASTER_ARM_AA() {

  if (MASTER_ARM_AA == true) {
    UIP_CONSOLE_LED[MASTER_ARM_START_POS + MASTER_ARM_AA_1 ] = CHSV( CHSVGreen, 255, indicatorBrightness);
    UIP_CONSOLE_LED[MASTER_ARM_START_POS + MASTER_ARM_AA_2 ] = CHSV( CHSVGreen, 255, indicatorBrightness);
  } else {
    UIP_CONSOLE_LED[MASTER_ARM_START_POS + MASTER_ARM_AA_1 ] = CHSV( CHSVGreen, 255, 0);
    UIP_CONSOLE_LED[MASTER_ARM_START_POS + MASTER_ARM_AA_2 ] = CHSV( CHSVGreen, 255, 0);
  }
  LedUpdateNeeded = true;
}

void Update_MASTER_ARM_AG() {

  if (MASTER_ARM_AG == true) {
    UIP_CONSOLE_LED[MASTER_ARM_START_POS + MASTER_ARM_AG_1 ] = CHSV( CHSVGreen, 255, indicatorBrightness);
    UIP_CONSOLE_LED[MASTER_ARM_START_POS + MASTER_ARM_AG_2 ] = CHSV( CHSVGreen, 255, indicatorBrightness);
  } else {
    UIP_CONSOLE_LED[MASTER_ARM_START_POS + MASTER_ARM_AG_1 ] = CHSV( CHSVGreen, 255, 0);
    UIP_CONSOLE_LED[MASTER_ARM_START_POS + MASTER_ARM_AG_2 ] = CHSV( CHSVGreen, 255, 0);
  }
  LedUpdateNeeded = true;
}


void Update_SPIN() {

  if (SPIN == true) {
    UIP_CONSOLE_LED[SPIN_RECOVERY_START_POS + SPIN_1 ] = CHSV( CHSVYellow, 255, indicatorBrightness);
    UIP_CONSOLE_LED[SPIN_RECOVERY_START_POS + SPIN_2 ] = CHSV( CHSVYellow, 255, indicatorBrightness);
  } else {
    UIP_CONSOLE_LED[SPIN_RECOVERY_START_POS + SPIN_1 ] = CHSV( CHSVYellow, 255, 0);
    UIP_CONSOLE_LED[SPIN_RECOVERY_START_POS + SPIN_2 ] = CHSV( CHSVYellow, 255, 0);
  }
  LedUpdateNeeded = true;
}

void SetIndicatorLighting()
{

  bool bLocalDebug = false;

  if ((Debug_Display || bLocalDebug ) && Serial_In_Use) Serial.println("Entering SetIndicatorlighting");
  if ((Debug_Display || bLocalDebug ) && Serial_In_Use)  Serial.println("Indicator Brightness: " + String(indicatorBrightness));

  Update_ECMJet();
  Update_MASTER_ARM_DISCH_READY();
  Update_MASTER_ARM_DISCH();
  Update_MASTER_ARM_AA();
  Update_MASTER_ARM_AG();
  Update_SPIN();

  if ((Debug_Display || bLocalDebug ) && Serial_In_Use)  Serial.println("Exiting SetIndicatorlighting");
}




void ProcessReceivedString()
{

  // Reading values from led packetBuffer which is global
  // All received values are strings for readability
  // From 737 Project

  bool bLocalDebug = true;
  int tempVar = 0;

  if ((Debug_Display || bLocalDebug ) && Serial_In_Use)  Serial.println("Processing Led Packet");


  String sWrkStr = "";
  const char *delim  = "=";



  ParameterNamePtr = strtok(ledpacketBuffer, delim);
  String ParameterNameString(ParameterNamePtr);
  if ((Debug_Display || bLocalDebug ) && Serial_In_Use)  Serial.println("Parameter Name " + ParameterNameString);

  ParameterValuePtr   = strtok(NULL, delim);
  String ParameterValue(ParameterValuePtr);
  if ((Debug_Display || bLocalDebug ) && Serial_In_Use)  Serial.println("Parameter Value " + ParameterValue);



  if (ParameterNameString.equalsIgnoreCase("ConsoleBrightness")) {
    if ((Debug_Display || bLocalDebug ) && Serial_In_Use) Serial.println("Found Console Brightness");
    consoleBrightness = ParameterValue.toInt();

    if ((Debug_Display || bLocalDebug ) && Serial_In_Use)  Serial.println("Console Brightness: " + String(consoleBrightness));
    if (consoleBrightness >= MAX_BRIGHTNESS) consoleBrightness = MAX_BRIGHTNESS;

    SetBacklighting();
    LedUpdateNeeded = true;
  }

  if (ParameterNameString.equalsIgnoreCase("IndicatorBrightness")) {
    if ((Debug_Display || bLocalDebug ) && Serial_In_Use) Serial.println("Found Indicator Brightness");
    indicatorBrightness = ParameterValue.toInt();

    if ((Debug_Display || bLocalDebug ) && Serial_In_Use)  Serial.println("Indicator Brightness: " + String(indicatorBrightness));
    if (indicatorBrightness >= MAX_BRIGHTNESS) indicatorBrightness = MAX_BRIGHTNESS;

    SetIndicatorLighting();
    LedUpdateNeeded = true;;
  }

  if (ParameterNameString.equalsIgnoreCase("Brightness")) {
    if ((Debug_Display || bLocalDebug ) && Serial_In_Use) Serial.println("Found Global Brightness");
    indicatorBrightness = ParameterValue.toInt();
    consoleBrightness = indicatorBrightness;
    if ((Debug_Display || bLocalDebug ) && Serial_In_Use)  Serial.println("Global Brightness: " + String(indicatorBrightness));
    if (indicatorBrightness >= MAX_BRIGHTNESS) indicatorBrightness = MAX_BRIGHTNESS;
    if (consoleBrightness >= MAX_BRIGHTNESS) consoleBrightness = MAX_BRIGHTNESS;
    SetBacklighting();
    SetIndicatorLighting();
    LedUpdateNeeded = true;
  }


  if (ParameterNameString.equalsIgnoreCase("ECM_JET")) {
    if ((Debug_Display || bLocalDebug ) && Serial_In_Use)  Serial.println("Found ECM Jet");
    tempVar = ParameterValue.toInt();
    if (tempVar == 1)
      ECM_JET = true;
    else
      ECM_JET = false;
    Update_ECMJet();
  }



  if (ParameterNameString.equalsIgnoreCase("MASTER_ARM_DISCH_READY")) {
    if ((Debug_Display || bLocalDebug ) && Serial_In_Use)  Serial.println("Found MASTER_ARM_DISCH_READY");
    tempVar = ParameterValue.toInt();
    if (tempVar == 1)
      MASTER_ARM_DISCH_READY = true;
    else
      MASTER_ARM_DISCH_READY = false;
    Update_MASTER_ARM_DISCH_READY();
  }




  if (ParameterNameString.equalsIgnoreCase("MASTER_ARM_DISCH")) {
    if ((Debug_Display || bLocalDebug ) && Serial_In_Use)  Serial.println("Found MASTER_ARM_DISCH");
    tempVar = ParameterValue.toInt();
    if (tempVar == 1)
      MASTER_ARM_DISCH = true;
    else
      MASTER_ARM_DISCH = false;
    Update_MASTER_ARM_DISCH();
  }



  if (ParameterNameString.equalsIgnoreCase("MASTER_ARM_AA")) {
    if ((Debug_Display || bLocalDebug ) && Serial_In_Use)  Serial.println("Found MASTER_ARM_AA");
    tempVar = ParameterValue.toInt();
    if (tempVar == 1)
      MASTER_ARM_AA = true;
    else
      MASTER_ARM_AA = false;
    Update_MASTER_ARM_AA();
  }



  if (ParameterNameString.equalsIgnoreCase("MASTER_ARM_AG")) {
    if ((Debug_Display || bLocalDebug ) && Serial_In_Use)  Serial.println("Found MASTER_ARM_AG");
    tempVar = ParameterValue.toInt();
    if (tempVar == 1)
      MASTER_ARM_AG = true;
    else
      MASTER_ARM_AG = false;
    Update_MASTER_ARM_AG();
  }



  if (ParameterNameString.equalsIgnoreCase("SPIN")) {
    if ((Debug_Display || bLocalDebug ) && Serial_In_Use)  Serial.println("Found SPIN");
    tempVar = ParameterValue.toInt();
    if (tempVar == 1)
      SPIN = true;
    else
      SPIN = false;
    Update_SPIN();
  }
}




void loop() {


  // slowly Dim Leds off after initial start up in setup
  if ((millis() >= NEXT_LED_UPDATE) && (startUpBrightness != 0)) {
    NEXT_LED_UPDATE = millis() + 10;

    startUpBrightness--;
    FastLED.setBrightness(startUpBrightness);
    FastLED.show();

    // If we've completed the startup dimming - set master level
    // And then set console leds to 0;
    if (startUpBrightness == 0) {
      // SetBacklightingColour();
      FastLED.setBrightness(MAX_MASTER_BRIGHTNESS);
      consoleBrightness = 0;
      indicatorBrightness = 100;
      SetBacklighting();
      SetIndicatorLighting();
      FastLED.show();
    }

  }



  if ((releaseKeysNeeded == true)  && (millis() >= timeBeforeReleaseAllKeys)) {

    Keyboard.releaseAll();

    if (Serial_In_Use == 1)  {
      Serial.println("Keys Released");
    }
    releaseKeysNeeded = false;
  }




  // Handle Keyboard updates
  keyboardpacketSize = keyboardudp.parsePacket();
  keyboardLen = keyboardudp.read(keyboardpacketBuffer, 999);

  if (keyboardLen > 0) {
    keyboardpacketBuffer[keyboardLen] = 0;
  }
  if (keyboardpacketSize) {
    SendCharactersToKeyboard(keyboardLen);
  }




  ledPacketSize = ledudp.parsePacket();
  ledLen = ledudp.read(ledpacketBuffer, 999);

  if (ledLen > 0) {
    ledpacketBuffer[ledLen] = 0;
  }
  if (ledPacketSize) {
    ProcessReceivedString();
  }




  if ((LedUpdateNeeded == true) && (millis() >= timeBeforeNextLedUpdate)) {
    FastLED.show();
    LedUpdateNeeded = false;
    timeBeforeNextLedUpdate = millis() + minTimeBetweenLedUpdates;
  }

}
