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
#define Ethernet_In_Use 0
const int Serial_In_Use = 0;
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

// Connections using Lukes Power Distribution
//#define LEFT_CONSOLE_PIN        40
//#define RIGHT_CONSOLE_PIN       42
//// Not used as locking collides 43
//// Unsure why 44 is here as thought the unused ports would be adjacent
//#define LIP_PIN                 44
//// Not used as locking collides 45
//// Nto sure w
//#define UIP_PIN                 46
//#define SPARE_PIN_1             48

#define LEFT_CONSOLE_PIN        16
#define RIGHT_CONSOLE_PIN       42
// Not used as locking collides 43
// Unsure why 44 is here as thought the unused ports would be adjacent
#define LIP_PIN                 14
// Not used as locking collides 45
// Nto sure w
#define UIP_PIN                 15
#define SPARE_PIN_1             48

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

unsigned long NEXT_LED_UPDATE = 0;

// Indicator Variables
bool ECM_JET = false;
bool MASTER_ARM_DISCH_READY = false;
bool MASTER_ARM_DISCH = false;
bool MASTER_ARM_AA = false;
bool MASTER_ARM_AG = false;
bool SPIN = false;


int ledptr = 0;
int consoleBrightness = 50;                     // Global Value for Console Brightness
int indicatorBrightness = 50;                   // Global value for Indicator Brightness
unsigned long timeBeforeNextLedUpdate = 0;
unsigned long minTimeBetweenLedUpdates = 40;    // Provides time foir several updates to be put together before throwing to the led strings
bool LedUpdateNeeded = false;                   // Flags if we have something to update

int leddelaytime = 1000;



void setup() {



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



}






void loop() {

  // Now apply everything we just told it about the setup.
  fill_solid( LEFT_CONSOLE_LED, LEFT_CONSOLE_LED_COUNT, CRGB::Green);
  fill_solid( RIGHT_CONSOLE_LED, RIGHT_CONSOLE_LED_COUNT, CRGB::Green);
  fill_solid( LIP_CONSOLE_LED, LIP_CONSOLE_LED_COUNT, CRGB::Green);
  fill_solid( UIP_CONSOLE_LED, UIP_CONSOLE_LED_COUNT, CRGB::Green);

  FastLED.show();
  delay(leddelaytime);
  fill_solid( LEFT_CONSOLE_LED, LEFT_CONSOLE_LED_COUNT, CRGB::Red);
  fill_solid( RIGHT_CONSOLE_LED, RIGHT_CONSOLE_LED_COUNT, CRGB::Red);
  fill_solid( LIP_CONSOLE_LED, LIP_CONSOLE_LED_COUNT, CRGB::Red);
  fill_solid( UIP_CONSOLE_LED, UIP_CONSOLE_LED_COUNT, CRGB::Red);

  FastLED.show();
  delay(leddelaytime);

  
  fill_solid( LEFT_CONSOLE_LED, LEFT_CONSOLE_LED_COUNT, CRGB::Blue);
  fill_solid( RIGHT_CONSOLE_LED, RIGHT_CONSOLE_LED_COUNT, CRGB::Blue);
  fill_solid( LIP_CONSOLE_LED, LIP_CONSOLE_LED_COUNT, CRGB::Blue);
  fill_solid( UIP_CONSOLE_LED, UIP_CONSOLE_LED_COUNT, CRGB::Blue);

  FastLED.show();
  delay(leddelaytime);



}
