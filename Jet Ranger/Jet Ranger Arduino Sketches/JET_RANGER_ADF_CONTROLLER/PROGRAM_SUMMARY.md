# JET_RANGER_ADF_CONTROLLER - Program Summary

Sketch for the **Jet Ranger ADF Controller** PCB (rev 1.4): an Arduino Mega 2560 shield with a W5500 Ethernet module
(USR-ES1). Built from `JET_RANGER_RADIO_CONTROLLER` (same Ethernet start-up, debug stream, heartbeat, polling encoder
library, matrix scanner, character OLED start-up and remote reset). Address **172.16.1.107**, MAC `A8:61:0A:67:83:6B`.
It compiles for `arduino:avr:mega` (26 KB flash, 1.7 KB RAM) and is flashed on the board; the landing gear lamps have been checked from the SwingTester and the keypad key labels were set from bench results.

## What it does

| Function | Pins | Behaviour |
|---|---|---|
| ADF tuning | D6/D7 major, D8/D9 minor | Major knob 100 kHz per click, minor knob 1 kHz per click (accelerates when spun). Range 190 to 1799 kHz, starts at 340. The frequency is sent 250 ms after the knobs stop |
| Aircraft ID keypad | D22 to D25 rows, D26 to D28 columns | 4 x 3 keypad. Digits build an ID (up to 8), `#` sends it, `*` deletes a digit, `*` on an empty entry stops following. An unfinished entry is dropped after 20 s. `keyMap[][]` holds {6,4,5}, {9,7,8}, {#,*,0}, {3,1,2}: the labels moved to where each key was actually reported (first guess 1 2 3 / 4 5 6 / 7 8 9 / * 0 #) |
| Panel brightness | A0 pot, D5 backlight | **Switched off for the moment** (`USE_BRIGHTNESS_POT` 0): the pot is not read, no `C,B` is sent to the other panels and this board's backlight stays at level 4 (PWM 68). Set it to 1 and reflash to bring the pot back (level 0 to 15 from the smoothed pot, sent to the Radio .101, Servo .102, Upper .103, Stepper .105 and Dual Stepper .106) |
| 12 V monitor | A15 | 47k / 10k divider with 0.1 uF, so volts = pin volts x 5.7, scaled by the measured 5 V |
| 5 V monitor | AREF / AVcc | The ADC reference (AVcc, the 5 V rail the AREF pin follows) is measured against the internal 1.1 V band-gap. The sketch never calls `analogReference()` |
| Power status | | Every 30 s (and 5 s after start-up, or on `C,POWER:1`), see below |
| Display | I2C 0x3C | Character OLED, 2 lines of 16: `ADF  340 kHz` and `ID: 1234_` / `FOLLOW 1234` / `FOLLOW OFF`. Written directly on the Mega's I2C bus (no TCA9548A) |

The landing gear lamps (D2 to D4) are lit by data packets: `D,LDGN:1,LDGL:1,LDGR:0` (1 = lit); the SwingTester has a toggle for each. The landing gear, flaps, speed brake and brake switches
(D29 to D32, to ground when on, 30 ms debounce) are sent as joystick buttons to the USB HID board (172.16.1.11:4210, Joystick 4, indexes 104 to 107, `index,state`). The gear, flaps and speed brake are
**toggle switches, so their position is sent as a value** to the bridge on UDP 27001: `GEAR_SET:1` (down) / `GEAR_SET:0` (up), `FLAPS_SET:1/0` and `SPOILERS_SET:1/0` on every change and again every
5 s (`POSITION_RESEND_MS`), so the simulator follows the switches, including their position at power-up (the aircraft moves about 5 s after the panel is switched on). The brakes push button sends
`PARKING_BRAKES` when pressed. For 1.5 s after start-up the switches are only read. If a switch works the wrong way round, swap its 1 and 0. Each ADF knob click is also a 40 ms press of Joystick 4 buttons
(major clockwise / anticlockwise 108 / 109, minor 110 / 111). The heartbeat is `ADF_PANEL V5:5.03 V12:12.41` (5 V and 12 V rails) for the Health Monitor.

## Network

| Direction | Port | Contents |
|---|---|---|
| In, UDP 13136 | control only | `C,RESET:1` reboot, `C,MATRIX:1` raw keypad dump to the debug stream, `C,POWER:1` send a power status now. Data packets are ignored |
| Out to 172.16.1.10:27000 | debug | Text, prefixed `Jet Ranger ADF:` |
| Out to 172.16.1.10:13137 | heartbeat | `ADF_PANEL` every 10 s (the Health Monitor has no light for it yet) |
| Out to 172.16.1.10:27003 | PC events | `ADF_FREQ:830`, `RADAR_FOLLOW_ID:4521` (`RADAR_FOLLOW_ID:0` = stop), `POWER,V12:12.41,V5:5.02,STATUS:OK` |

The events use port 27003, not 27001, because the FSUIPC and MSFS bridges treat any unrecognised string on 27001 as a COM1 standby
frequency. **The bridges do not read port 27003 yet**, so nothing reaches the simulator until they are extended.

`STATUS` is `OK`, or the rails out of range joined with `+` (`5V_LOW`, `5V_HIGH`, `12V_LOW`, `12V_HIGH`). Limits: 5 V 4.75 to 5.25, 12 V 10.8 to 15.0
(`V5_LOW`, `V5_HIGH`, `V12_LOW`, `V12_HIGH`). `V12_CAL` and `V5_CAL` trim the readings against a meter.

## Things to check on the bench

- **Keypad rows and columns.** The PCB labels the seven keypad lines `KEYPAD_7` to `KEYPAD_1` (D22 to D28). The sketch assumes D22 to D25 are the four rows and D26 to D28
  the three columns, as on the Radio controller. Press keys with `C,MATRIX:1` to confirm, and change `rowPins`, `colPins` and `keyMap` if the keypad is wired the other way.
- **Encoder direction.** If a knob turns the frequency the wrong way, swap its A and B pins in the `RotaryEncoder` lines.
- **OLED type.** The display code is the Radio controller's character OLED (SSD/US2066 style, address 0x3C). If the ADF panel uses a graphics SSD1306 it needs different code.
- **Rail readings.** Compare the first power status lines with a meter and trim `V12_CAL` and `V5_CAL`.
