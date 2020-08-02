/*
 * Copyright (c) 2018 https://www.thecoderscorner.com (Nutricherry LTD).
 * This product is licensed under an Apache license, see the LICENSE file in the top-level directory.
 */

#ifndef _KEYBOARD_MANGER_H_
#define _KEYBOARD_MANGER_H_

#include "IoAbstraction.h"

/**
 * @file KeyboardManager.h contains the classes needed to deal with matrix keyboards
 */

/**
 * When there are changes in keyboard manager it will call the appropriate function on your
 * instance of this class to inform you of the change.
 */
class KeyboardListener {
public:
    /**
     * A key has been pressed or held down.
     * @param key the character code for the key
     * @param held if held down
     */
    virtual void keyPressed(char key, bool held)=0;
    /**
     * A key has been released
     * @param key the character code of the key
     */
    virtual void keyReleased(char key)=0;
};

/**
 * The keyboard manager can handle any layout of matrix keyboard by providing it a layout.
 * This class is used to tell the matrix manager the row and column arrangements of your keyboard
 * matrix. There are two standard ones defined in this file. They are LAYOUT_3X4, LAYOUT_4X4.
 * When creating a class of this type, be sure that your string of keyCode mappings is defined as
 * PROGMEM and at rows * cols in size. You can either use one of the standard defined layouts or
 * generate your own.
 */
class KeyboardLayout {
private:
    uint8_t rows;
    uint8_t cols;
    const char *pgmLayout;
    uint8_t *pins;
public:
    /**
     * Create a keyboard layout with a number of rows and columns, the characters that are associated
     * with each key are also provided in a char array.
     * @param rows the number of rows in the keyboard
     * @param cols the number of columns in the keyboard.
     * @param pgmLayout the character codes for each position in the keyboard
     */
    KeyboardLayout(uint8_t rows, uint8_t cols, const char* pgmLayout) {
        this->rows = rows;
        this->cols = cols;
        this->pgmLayout = pgmLayout;
        this->pins = new uint8_t[rows + cols];
    }

    ~KeyboardLayout() {
        delete pins;
    }

    int numColumns() { return cols; }

    int numRows() { return rows; }

    void setColPin(int col, int pin) {
        if(col < cols) pins[rows + col] = pin;
    }

    void setRowPin(int row, int pin) {
        if(row < rows) pins[row] = pin;
    }

    int getRowPin(int row) {
        return pins[row];
    }

    int getColPin(int col) {
        return pins[rows + col];
    }

    char keyFor(uint8_t row, uint8_t col) {
        if(row < rows && col < cols) return pgm_read_byte_near(&pgmLayout[(row * cols)+col]);
        else return 0;
    }
};

/**
 *  Internally used by the keyboard manager to manage the state of keys.
 */
enum KeyMode: uint8_t {
    KEYMODE_NOT_PRESSED,
    KEYMODE_DEBOUNCE,
    KEYMODE_PRESSED,
    KEYMODE_REPEATING
};

#define KEYBOARD_TASK_MILLIS 50

/**
 * A keyboard manager that can determine if a key is pressed or released for a given
 * layout of keyboard. It is configured during initialisation with an IoAbstraction
 * that is used to access hardware, a specific keyboard layout and a listener that
 * will be called back when keys are pressed and released.
 */
class MatrixKeyboardManager : public Executable {
private:
    KeyboardListener* listener;
    KeyboardLayout* layout;
    IoAbstractionRef ioRef;
    uint8_t repeatTicks = 10;
    uint8_t repeatStartTicks = 30;
    char currentKey;
    KeyMode keyMode;
    uint8_t counter;
public:
    MatrixKeyboardManager();
    void initialise(IoAbstractionRef ref, KeyboardLayout* layout, KeyboardListener* listener);
    void setRepeatKeyMillis(int startAfterMillis, int repeatMillis);
    void exec();
private:
    void setToOuput(int i);
};

#define MAKE_KEYBOARD_LAYOUT_3X4(varName) const char KEYBOARD_STD_3X4_KEYS[] PROGMEM = "123456789*0#"; KeyboardLayout varName(4, 3, KEYBOARD_STD_3X4_KEYS);
#define MAKE_KEYBOARD_LAYOUT_4X4(varName) const char KEYBOARD_STD_4X4_KEYS[] PROGMEM = "123A456B789C*0#D"; KeyboardLayout varName(4, 4, KEYBOARD_STD_4X4_KEYS);
// #define MAKE_KEYBOARD_LAYOUT_6X6(varName) const char KEYBOARD_STD_6X6_KEYS[] PROGMEM = "123456789abcdefghijklmnopqrstuv"; KeyboardLayout varName(6, 6, KEYBOARD_STD_6X6_KEYS);

#define MAKE_KEYBOARD_LAYOUT_6X6(varName) const char KEYBOARD_STD_6X6_KEYS[] PROGMEM = {\
0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,\
0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,\
}; KeyboardLayout varName(6, 6, KEYBOARD_STD_6X6_KEYS);


#define MAKE_KEYBOARD_LAYOUT_16X8(varName) const char KEYBOARD_STD_16X8_KEYS[] PROGMEM = {\
0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,\
0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,\
0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F,\
0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,\
0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F,\
0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x5B, 0x5C, 0x5D, 0x5E, 0x5F,\
0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F,\
0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F,\
}; KeyboardLayout varName(16, 8, KEYBOARD_STD_16X8_KEYS);


#endif // _KEYBOARD_MANGER_H_
