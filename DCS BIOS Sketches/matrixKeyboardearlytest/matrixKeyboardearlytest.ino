/**
 * This example shows how to use the matrix keyboard support that's built into IoAbstraction,
 * it can be used out the box with either a 3x4 or 4x4 keypad, but you can modify it to use
 * any matrix keyboard quite easily.
 * It just sends the characters that are typed on the keyboard to Serial. The keyboard in This
 * example is connected directly to Arduino pins, but could just as easily be connected over
 * a PCF8574, MCP23017 or other IoAbstraction.
 */

#include <Wire.h>
#include <IoAbstraction.h>
#include "DCSKeyboardManager.h"

//
// We need to make a keyboard layout that the manager can use. choose one of the below.
// The parameter in brackets is the variable name.
//
MAKE_KEYBOARD_LAYOUT_16X8(keyLayout)
MAKE_KEYBOARD_LAYOUT_6X6(smallKeyLayout)

//
// We need a keyboard manager class too
//
MatrixKeyboardManager keyboard, mykeyboard;

// this examples connects the pins directly to an arduino but you could use
// IoExpanders or shift registers instead.
IoAbstractionRef arduinoIo = ioUsingArduino();

//
// We need a class that extends from KeyboardListener. this gets notified when
// there are changes in the keyboard state.
//
class MyKeyboardListener : public KeyboardListener {
public:
    void keyPressed(char key, bool held) override {
      
        Serial.print("Key ");
        Serial.print(key, HEX);
        Serial.print(" is pressed, held = ");
        Serial.println(held);
    }

    void keyReleased(char key) override {
        Serial.print("Released ");
        Serial.println(key);
    }
} myListener;

class MySmallKeyboardListener : public KeyboardListener {
public:
    void keyPressed(char key, bool held) override {
      
        Serial.print("Small Key ");
        Serial.print(key, HEX);
        Serial.print(" is pressed, held = ");
        Serial.println(held);
    }

    void keyReleased(char key) override {
        Serial.print("Released ");
        Serial.println(key);
    }
} mySmallerListener;


void setup() {
    while(!Serial);
    Serial.begin(115200);

    keyLayout.setRowPin(0, 22);
    keyLayout.setRowPin(1, 23);
    keyLayout.setRowPin(2, 24);
    keyLayout.setRowPin(3, 25);
    keyLayout.setRowPin(4, 26);
    keyLayout.setRowPin(5, 27);
    keyLayout.setRowPin(6, 28);
    keyLayout.setRowPin(7, 29);
    keyLayout.setRowPin(8, 30);
    keyLayout.setRowPin(9, 31);
    keyLayout.setRowPin(10, 32);
    keyLayout.setRowPin(11, 33);
    keyLayout.setRowPin(12, 34);
    keyLayout.setRowPin(13, 35);
    keyLayout.setRowPin(14, 36);
    keyLayout.setRowPin(15, 37);

    smallKeyLayout.setRowPin(0, 38);
    smallKeyLayout.setRowPin(1, 39);
    smallKeyLayout.setRowPin(2, 40);
    smallKeyLayout.setRowPin(3, 41);
    smallKeyLayout.setRowPin(4, 42);
    smallKeyLayout.setRowPin(5, 43);

    smallKeyLayout.setColPin(0, 45);
    smallKeyLayout.setColPin(1, 44);
//    smallKeyLayout.setColPin(2, 51);
//    smallKeyLayout.setColPin(3, 50);
//    smallKeyLayout.setColPin(4, 49);
//    smallKeyLayout.setColPin(5, 48);
        
    keyLayout.setColPin(0, 53);
    keyLayout.setColPin(1, 52);
    keyLayout.setColPin(2, 51);
    keyLayout.setColPin(3, 50);
    keyLayout.setColPin(4, 49);
    keyLayout.setColPin(5, 48);
    keyLayout.setColPin(6, 47);
    keyLayout.setColPin(7, 46);

    
    // create the keyboard mapped to arduino pins and with the layout chosen above.
    // it will callback our listener
    keyboard.initialise(arduinoIo, &keyLayout, &myListener);

    mykeyboard.initialise(arduinoIo, &smallKeyLayout, &mySmallerListener);

    // start repeating at 850 millis then repeat every 350ms
    keyboard.setRepeatKeyMillis(850, 350);

    Serial.println("Keyboard is initialised!");
}

void loop() {
    // as this indirectly uses taskmanager, we must include this in loop.
    taskManager.runLoop();
}
