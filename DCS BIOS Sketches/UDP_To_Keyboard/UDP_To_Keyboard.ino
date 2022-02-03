/*

  Receives a space delimited set of characters and sends them to the keyboard

  Works on an Arduino DUE.

  Will use the logic from pyHWWLink_Keystroke_Sender

  Initially just sending very simple single characters

  ModifiersOfInterest = ['ALT', 'CTRL', 'SHIFT', 'LSHIFT', 'RSHIFT', 'LCTRL', 'RCTRL', 'LALT', 'RALT', 'LWIN', 'RWIN' ]

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
const unsigned int remoteport = 49000;
const unsigned int reflectorport = 27000;

const int delayBetweenRelease = 100;

EthernetUDP udp;
char packetBuffer[1000];     //buffer to store the incoming data
char outpacketBuffer[1000];  //buffer to store the outgoing data



// For vanilla alt, ctrl, shift, just use left side
char altKey = KEY_LEFT_ALT;
char ctrlKey = KEY_LEFT_CTRL;
char shiftKey = KEY_LEFT_SHIFT;

char leftShiftKey = KEY_LEFT_SHIFT;
char rightShiftKey = KEY_RIGHT_SHIFT;
char leftALTKey = KEY_LEFT_ALT;
char rightALTKey = KEY_RIGHT_ALT;
char leftCTRLKey = KEY_LEFT_CTRL;
char rightCTRLKey = KEY_RIGHT_CTRL;
char leftWINKey = KEY_LEFT_GUI;
char rightWINKey = KEY_RIGHT_GUI;




void setup() {

  if (Serial_In_Use) {
    Serial.begin(250000);
  }


  if (Ethernet_In_Use == 1) {
    Ethernet.begin( mac, ip);

    udp.begin( localport );
    udp.beginPacket(targetIP, reflectorport);
    udp.println("Init UDP - " + strMyIP + " " + String(millis()) + "mS since reset.");
  }

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
      Keyboard.press(leftShiftKey);
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
    Keyboard.press(leftShiftKey);
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
    Keyboard.press(leftALTKey);

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

    String thisSet = "";
    for (int characterPtr = 0; characterPtr < packetLength ; characterPtr++ ) {
      //Serial.print(packetBuffer[characterPtr]);

      // Build Out Modifier list
      // ModifiersOfInterest = ['ALT', 'CTRL', 'SHIFT', 'LSHIFT', 'RSHIFT', 'LCTRL', 'RCTRL', 'LALT', 'RALT', 'LWIN', 'RWIN' ]
      // Modifiers to do  [   , 'RWIN' ]
      // Also need to deal with Function Key or pretty much any key that is not a single character


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








  lWinInUse = false;
  rWinInUse = false;


  if (altInUse)
    Keyboard.press(altKey);
  if (ctrlInUse)
    Keyboard.press(ctrlKey);
  if (shiftInUse)
    Keyboard.press(shiftKey);
    

  if (leftAltInUse)
    Keyboard.press(leftALTKey);
  if (rightAltInUse)
    Keyboard.press(rightALTKey);
  if (lShiftInUse)
    Keyboard.press(leftShiftKey);
  if (rShiftInUse)
    Keyboard.press(rightShiftKey);
  if (lCtrlInUse)
    Keyboard.press(leftCTRLKey);
  if (rCtrlInUse)
    Keyboard.press(rightCTRLKey);
  if (lWinInUse)
    Keyboard.press(leftWINKey);
  if (rWinInUse)
    Keyboard.press(rightWINKey);

  


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

void loop() {


  int packetSize = udp.parsePacket();

  int len = udp.read(packetBuffer, 999);

  if (len > 0) {

    packetBuffer[len] = 0;

  }

  if (packetSize) {
    // Typeout(len);
    TurnOnAPU(len);
  }

}
