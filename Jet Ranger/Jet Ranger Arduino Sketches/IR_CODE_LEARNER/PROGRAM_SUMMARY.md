# IR_CODE_LEARNER — Program Summary

A bench utility (not a cockpit controller) that reads the buttons of an infrared remote control and prints their codes on the
serial monitor, for replacing a projector's remote with an IR interface. Added 27 November 2026.

## Hardware

| Item | Connection |
|---|---|
| Arduino Mega 2560, Uno or Nano | USB to the PC |
| IR receiver, TSOP38238 or VS1838B | OUT to D8, GND to GND, VCC to 5 V (check the pin order on the part's data sheet) |

## Library

`IRremote` (Arduino-IRremote) 4.7.1, installed with `arduino-cli lib install IRremote`.

## Use

1. Open the serial monitor at 115200 baud with "Newline" as the line ending.
2. Type a button name (for example `POWER`) and press Enter. The name is printed above the next codes received.
3. Press that button on the remote, 10 to 30 cm from the receiver. Press it twice to check the code is the same.

For a protocol the library knows, it prints the decoded result and the line of code that sends it (`printIRSendUsage`). For an
unknown protocol it prints the raw timings as a C array. Repeat frames (a button held down) are ignored. On the Mega the raw
buffer is 750 entries, for long unknown codes.

## Status

Compiled for `arduino:avr:uno` (11,650 bytes flash, 540 bytes RAM) and `arduino:avr:mega` (12,786 bytes flash, 1,124 bytes RAM).
Flashed to an Uno (COM3) on 27 November 2026 and its start-up message read back; not yet tried with a remote. It has no network side: the sender sketch, to be written once the codes are known,
is a separate program.
