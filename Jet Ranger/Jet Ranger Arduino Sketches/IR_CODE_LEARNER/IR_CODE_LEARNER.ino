/*
  IR code learner: reads the buttons of an infrared remote control and prints their codes on the serial monitor,
  ready to be copied into a sketch that sends them (for replacing a projector's remote).

  Hardware (Mega 2560, Uno or Nano):
    IR receiver module, TSOP38238 or VS1838B (check the pin order on your part's data sheet; for both it is usually
    OUT, GND, VCC seen from the front):
      OUT -> D8
      GND -> GND
      VCC -> 5 V

  Library: IRremote (Arduino-IRremote), version 4.

  Use:
    1. Open the serial monitor at 115200 baud, with "Newline" as the line ending.
    2. Type the name of a button (for example POWER) and press Enter.
    3. Point the remote at the receiver from about 10 to 30 cm and press that button once.
    4. Check that two presses give the same code, then carry on with the next button.
  Repeat frames (sent while a button is held) are ignored.
*/

#if defined(__AVR_ATmega2560__)
#define RAW_BUFFER_LENGTH 750  // long unknown codes need a bigger buffer; the Mega has the RAM for it
#endif
#include <IRremote.hpp>

#define IR_RECEIVE_PIN 8

char buttonName[24] = "(unnamed)";
uint8_t nameLength = 0;
char typed[24];

void readButtonName() {
  while (Serial.available()) {
    char c = Serial.read();
    if (c == '\n' || c == '\r') {
      if (nameLength > 0) {
        typed[nameLength] = 0;
        strcpy(buttonName, typed);
        nameLength = 0;
        Serial.print(F("Now press the button: "));
        Serial.println(buttonName);
      }
    } else if (nameLength < sizeof(typed) - 1) {
      typed[nameLength++] = c;
    }
  }
}

void setup() {
  Serial.begin(115200);
  IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK);
  Serial.println(F("IR code learner ready on D8. Type a button name and press Enter, then press the button."));
}

void loop() {
  readButtonName();
  if (!IrReceiver.decode()) return;
  if (!(IrReceiver.decodedIRData.flags & IRDATA_FLAGS_IS_REPEAT)) {
    Serial.println();
    Serial.print(F("===== "));
    Serial.print(buttonName);
    Serial.println(F(" ====="));
    IrReceiver.printIRResultShort(&Serial);
    if (IrReceiver.decodedIRData.protocol == UNKNOWN) {
      // Not a protocol the library knows (or a weak or noisy signal): print the raw timings, which can be sent back as they are.
      Serial.println(F("Unknown protocol, raw timings:"));
      IrReceiver.compensateAndPrintIRResultAsCArray(&Serial, true);
    } else {
      IrReceiver.printIRSendUsage(&Serial);
    }
  }
  IrReceiver.resume();
}
