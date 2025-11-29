/*
 * SimpleReceiver.cpp
 *
 * Demonstrates receiving ONLY NEC protocol IR codes with IRremote
 * If no protocol is defined, all protocols (except Bang&Olufsen) are active.
 *
 *  This file is part of Arduino-IRremote https://github.com/Arduino-IRremote/Arduino-IRremote.
 *
 ************************************************************************************
 * MIT License
 *
 * Copyright (c) 2020-2025 Armin Joachimsmeyer
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is furnished
 * to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
 * CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
 * OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 ************************************************************************************
 */

#include <Arduino.h>

/*
 * Specify which protocol(s) should be used for decoding.
 * If no protocol is defined, all protocols (except Bang&Olufsen) are active.
 * This must be done before the #include <IRremote.hpp>
 */
//#define DECODE_DENON        // Includes Sharp
//#define DECODE_JVC
//#define DECODE_KASEIKYO
//#define DECODE_PANASONIC    // alias for DECODE_KASEIKYO
//#define DECODE_LG
//#define DECODE_NEC          // Includes Apple and Onkyo. To enable all protocols , just comment/disable this line.
//#define DECODE_SAMSUNG
//#define DECODE_SONY
//#define DECODE_RC5
//#define DECODE_RC6

//#define DECODE_BOSEWAVE
//#define DECODE_LEGO_PF
//#define DECODE_MAGIQUEST
//#define DECODE_WHYNTER
//#define DECODE_FAST

//#define DECODE_DISTANCE_WIDTH // Universal decoder for pulse distance width protocols
//#define DECODE_HASH         // special decoder for all protocols

//#define DECODE_BEO          // This protocol must always be enabled manually, i.e. it is NOT enabled if no protocol is defined. It prevents decoding of SONY!

//#define DEBUG               // Activate this for lots of lovely debug output from the decoders.

//#define USE_THRESHOLD_DECODER  // May give slightly better results especially for jittering signals and protocols with short 1 pulses / pauses. Saves 110 bytes program memory.
//#define RAW_BUFFER_LENGTH  750 // For air condition remotes it may require up to 750. Default is 200.

/*
 * This include defines the actual pin number for pins like IR_RECEIVE_PIN, IR_SEND_PIN for many different boards and architectures
 */
#include "PinDefinitionsAndMore.h"
#include <IRremote.hpp>  // include the library

void setup() {
  Serial.begin(115200);

  // Just to know which program is running on my Arduino
  Serial.println(F("START " __FILE__ " from " __DATE__ "\r\nUsing library version " VERSION_IRREMOTE));

  // Start the receiver and if not 3. parameter specified, take LED_BUILTIN pin from the internal boards definition as default feedback LED
  IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK);

  Serial.print(F("Ready to receive IR signals of protocols: "));
  printActiveIRProtocols(&Serial);
  Serial.println(F("at pin " STR(IR_RECEIVE_PIN)));
}

void displayreceivedvalue(int commandcode, String description) {
  String wkrString =  String(millis()) + ": Received: " + description + " 0x" + String(commandcode, HEX);
  Serial.println(wkrString);
}

void loop() {
  /*
     * Check if received data is available and if yes, try to decode it.
     * Decoded result is in the IrReceiver.decodedIRData structure.
     *
     * E.g. command is in IrReceiver.decodedIRData.command
     * address is in command is in IrReceiver.decodedIRData.address
     * and up to 32 bit raw data in IrReceiver.decodedIRData.decodedRawData
     */
  if (IrReceiver.decode()) {

    /*
         * Print a summary of received data
         */
    if (IrReceiver.decodedIRData.protocol == UNKNOWN) {
      Serial.println(F("Received noise or an unknown (or not yet enabled) protocol"));
      // We have an unknown protocol here, print extended info
      IrReceiver.printIRResultRawFormatted(&Serial, true);

      IrReceiver.resume();  // Do it here, to preserve raw data for printing with printIRResultRawFormatted()
    } else {
      IrReceiver.resume();  // Early enable receiving of the next IR frame

      IrReceiver.printIRResultShort(&Serial);
      IrReceiver.printIRSendUsage(&Serial);
    }
    Serial.println();

    /*
         * Finally, check the received data and perform actions according to the received command
         */
    if (IrReceiver.decodedIRData.flags & IRDATA_FLAGS_IS_REPEAT) {
      Serial.println(F("Repeat received. Here you can repeat the same action as before."));
    }


    switch (IrReceiver.decodedIRData.command) {
      case 0x1:
        displayreceivedvalue(IrReceiver.decodedIRData.command, String("Toggle Mute"));
        break;
      case 0x2:
        displayreceivedvalue(IrReceiver.decodedIRData.command, String("Down Arrow"));
        break;
      case 0x3:
        displayreceivedvalue(IrReceiver.decodedIRData.command, String("Up Arrow"));
        break;
      case 0x7:
        displayreceivedvalue(IrReceiver.decodedIRData.command, String("OK"));
        break;
      case 0xb:
        displayreceivedvalue(IrReceiver.decodedIRData.command, String("Plus"));
        break;
      case 0xe:
        displayreceivedvalue(IrReceiver.decodedIRData.command, String("Left Arrow"));
        break;
      case 0x13:
        displayreceivedvalue(IrReceiver.decodedIRData.command, String("List Menu"));
        break;
      case 0x14:
        displayreceivedvalue(IrReceiver.decodedIRData.command, String("Toggle Power"));
        break;
      case 0x1a:
        displayreceivedvalue(IrReceiver.decodedIRData.command, String("right Arrow"));
        break;
      case 0x48:
        displayreceivedvalue(IrReceiver.decodedIRData.command, String("Home"));
        break;
      case 0x58:
        displayreceivedvalue(IrReceiver.decodedIRData.command, String("Minus"));
        break;
      case 0x5c:
        displayreceivedvalue(IrReceiver.decodedIRData.command, String("Up Menu"));
        break;
      case 0x82:
        displayreceivedvalue(IrReceiver.decodedIRData.command, String("Mouse Click"));
        break;
      case 0x93:
        displayreceivedvalue(IrReceiver.decodedIRData.command, String("Blanks Screen??"));
        break;
      case 0x98:
        displayreceivedvalue(IrReceiver.decodedIRData.command, String("Switch Input"));
        break;
      default:
        displayreceivedvalue(IrReceiver.decodedIRData.command, String("Not Yet Mapped"));
        break;
    }
  }
}
