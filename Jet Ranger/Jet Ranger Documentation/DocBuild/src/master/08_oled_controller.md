# OLED controller (172.16.1.104)

`JET_RANGER_OLED_CONTROLLER` runs on an Arduino Mega 2560 with a W5500 Ethernet module. Its header comments identify it as an A-10C front-console OLED sketch (`A10_FRONT_CONSOLE_OLED`): its main data source is DCS-BIOS over USB serial, not the Jet Ranger protocol. It is kept in the sketch folder because it can also be driven over UDP by the bridge, which sends it the altitude and barometric setting for two displays.

## What it drives

Small OLEDs through a TCA9548A I2C multiplexer (address 0x70) and eight discrete lamps for an A-10C style console: a barometric-pressure readout, a large digital altimeter, a countermeasures status line, and landing-gear, VHF and angle-of-attack lamps. The altimeter and barometer are the only parts used by the Jet Ranger setup.

| Mux port | Display |
|---|---|
| 1 | Barometer OLED |
| 2 | Altimeter OLED |
| 3 | Countermeasures (CMSC) character OLED at 0x3C |

## Program flow

1. **Setup:** flash the status LEDs, reset the W5500 and bring up Ethernet, set the eight lamp pins as outputs (off), start I2C, initialise the character OLED and show a test splash, scan all eight mux ports for I2C devices (results go to the debug stream), initialise the barometer and altimeter OLEDs, wait 3 seconds, turn the eight lamps on and start DCS-BIOS.
2. **Main loop:** toggle the status LEDs every 300 ms; run `DcsBios::loop()`; when the barometer setting changed recompute the altimeter correction and refresh both displays; every 5 ms check UDP 13136.

## UDP interface

The sketch parses the same `D,CODE:value` packets as the Servo controller but handles only two codes; every other code is ignored.

| Token | Effect |
|---|---|
| `ALT` | Redraws the rolling altitude digits (feet, unconverted) |
| `BARO` | Sets the four barometer digits from one value in inHg x 100 (`2992` is 29.92 inHg) and refreshes both displays |

The FSUIPC bridge sends this board the whole shared front-panel packet and the board reads `ALT` and `BARO` out of it. The MSFS bridge sends it only those two codes.

## Pin usage

| Pin | Function |
|---|---|
| 12 | Red status LED |
| 13 | Green status LED |
| 53 | W5500 reset |
| SDA, SCL | TCA9548A mux |
| 2 | Right landing-gear lamp |
| 3 | Nose landing-gear lamp |
| 4 | Left landing-gear lamp |
| 5, 6 | VHF-AM and VHF-FM lamps |
| 7, 8, 9 | Angle-of-attack indexer lamps (above, on, below) |
| USB serial | DCS-BIOS link to a DCS World PC |

Pin 2 is also used as an OLED reset pin in some declared but unused OLED objects; check the real wiring before reusing those code paths.

## Network

Static IP `172.16.1.104`, MAC `A8:61:0A:67:83:68`, data port 13136, debug to 172.16.1.10:27000. The board sends no heartbeat, so the Health Monitor does not show it.

## Notes

- `PTFont.h`, `dseg14_v3.h`, `er_oled.h` and `er_oled.cpp` in the sketch folder are not included by the sketch; they look like an earlier OLED driver.
- The Dual Stepper board now has its own altimeter and barometer OLEDs, so this board is optional for the Jet Ranger.
