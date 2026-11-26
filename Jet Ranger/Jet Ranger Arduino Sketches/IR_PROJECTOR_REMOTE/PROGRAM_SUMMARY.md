# IR_PROJECTOR_REMOTE — Program Summary

Replaces a projector's infrared remote: an Arduino sends the remote's codes through an IR LED when a push button is pressed
or a command arrives on the serial port. Added 27 November 2026. A bench stage, driven over USB serial and push buttons; no network side is needed.

## Hardware

| Item | Connection |
|---|---|
| Arduino Uno, Nano or Mega 2560 | USB to the PC |
| IR LED transmitter | D3 to 1 k to the base of an NPN transistor (2N2222 or BC337), emitter to GND; 5 V to R1 (47 to 100 ohm) to the LED anode, LED cathode to the collector |

## Push buttons

Six buttons, each between its pin and GND (internal pull-ups, no resistors):

| Pin | Button |
|---|---|
| A0 | POWER |
| A1 | UP |
| A2 | DOWN |
| A3 | LEFT |
| A4 | RIGHT |
| A5 | OK |

Each input is debounced without blocking: a change counts once it has been steady for 30 ms (`DEBOUNCE_MS`). The code is sent
once when a button goes down; holding it does not repeat it. A button already held at start-up sends nothing. To switch the
projector off with the buttons, press POWER twice, as on the remote.

## Library

`IRremote` (Arduino-IRremote) 4.7.1.

## Commands (serial, 115200 baud, one per line, any case)

| Command | NEC command (address 0x00) |
|---|---|
| `POWER` | 0x14 |
| `LEFT` | 0x0E |
| `RIGHT` | 0x1A |
| `UP` | 0x03 |
| `DOWN` | 0x02 |
| `OK` | 0x07 |
| `OFF` | power, 1 second, power again (assumes the projector asks for a second press to switch off; not yet confirmed) |
| `CMD <hex>` | any command number from 00 to FF, to try codes that were not captured |

Replies are `SENT <command>` or `UNKNOWN <text>`. The codes were captured from the projector's remote with `IR_CODE_LEARNER`.
Source, Menu, Back and any other buttons have not been captured.

## Status

Compiled for `arduino:avr:uno` (5,296 bytes flash, 606 bytes RAM) with the push buttons. Not flashed or tried against the projector.
