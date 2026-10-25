# ADF controller (172.16.1.107)

`JET_RANGER_ADF_CONTROLLER` runs on an Arduino Mega 2560 with a W5500 Ethernet module, on the **Jet Ranger ADF Controller** PCB (revision 1.4). It is built from the Radio controller sketch (same Ethernet start-up, debug stream, heartbeat, polling encoder library, matrix scanner, character OLED start-up and software reset) and does these jobs: it tunes the ADF with two knobs, takes the ID of an aircraft to follow on the radar from a keypad, sends the landing gear, flaps, speed brake and brakes switches to the simulator (as joystick buttons and as bridge commands), drives the three landing gear lamps, and monitors the **12 V and 5 V** supply rails. The PCB also has a **panel brightness pot**, which the sketch can read but has switched off for the moment (see Panel brightness). The sketch compiles for `arduino:avr:mega` (26 KB of flash, 1.7 KB of RAM) and has been flashed to the board; the landing gear lamps have been checked from the SwingTester and the keypad key labels were set from bench results. The rest has not been bench-tested.

![ADF controller blocks](diagrams/adf_blocks.svg)

## Program flow

1. **Setup:** set the status LEDs, set the board's own backlight (the pot is not read), reset the W5500, start Ethernet with the 10 ms retransmission timeout and open the sockets (red LED flashes for 3 seconds), start the I2C character OLED, start the two encoders (centred at 1000), the matrix scanner, the panel switches and the landing gear lamps.
2. **Main loop:**
   - toggle the status LEDs every 200 ms and send the `ADF_PANEL` heartbeat every 10 seconds, with the 5 V and 12 V readings (`ADF_PANEL V5:5.03 V12:12.41`) for the Health Monitor;
   - read both encoders. Each is kept near the middle of its range and recentred after every read, so the change since the last read is the distance from the middle; three counts make one click. The major knob changes the ADF frequency by 100 kHz per click and the minor knob by 1 kHz per click (the minor knob speeds up when it is turned quickly), clamped to 190 to 1799 kHz. 250 ms after the last movement the frequency is sent as `ADF_FREQ:<kHz>`, and each click is also a joystick button press;
   - `scanMatrix()` scans the 4 by 3 keypad with a 50 ms debounce. For the first 1.5 seconds after start-up it only records the state, so a floating column is not reported as key presses;
   - the keys build an aircraft ID: digits 0 to 9 add to it (up to 8), `#` sends `RADAR_FOLLOW_ID:<digits>` and makes it the followed ID, `*` deletes the last digit, and `*` on an empty entry stops following (`RADAR_FOLLOW_ID:0`). An unfinished entry is dropped after 20 seconds;
   - read the four panel switches and send their changes (below);
   - redraw the OLED when something changed (at most every 50 ms);
   - send the power status (below);
   - poll port 13136 and act on the control and data packets.

## Pin usage

| Pin | Function |
|---|---|
| D2, D3, D4 | Landing gear lamps (nose, left, right), driven from data packets (high = lit) |
| D5 | `BACKLIGHT_PWM`: this board's backlight MOSFET (IRF540N) |
| D6, D7 | ADF major encoder A, B |
| D8, D9 | ADF minor encoder A, B |
| D14, D15 | Green and red status LEDs |
| D20, D21 | I2C SDA and SCL (character OLED at 0x3C) |
| D22 to D25 | Keypad scan rows (`KEYPAD_7` to `KEYPAD_4`) |
| D26 to D28 | Keypad scan columns (`KEYPAD_3` to `KEYPAD_1`) |
| D29 to D32 | Landing gear, flaps, speed brakes and brakes switches (to ground when on) |
| D53 | W5500 reset |
| A0 | Panel brightness pot wiper (`GUAGE_BACKLIGHT_BRIGHT`), not read while the pot is switched off |
| A15 | 12 V monitor (`12V-Monitor`) |

## Keypad

The seven keypad lines `KEYPAD_1` to `KEYPAD_7` reach the keypad on the 7-pin connector J19 (pin *n* is `KEYPAD_n`) through pins 18 to 24 of the ADF panel cable. The sketch scans D22 to D25 as four rows and D26 to D28 as three columns. The first key labels were a guess (1 2 3 / 4 5 6 / 7 8 9 / * 0 #); the keys were then pressed one by one and the labels in `keyMap[][]` were moved to where each key was actually reported:

| Scan row | Column 0 (D26) | Column 1 (D27) | Column 2 (D28) |
|---|---|---|---|
| 0 (D22) | 6 | 4 | 5 |
| 1 (D23) | 9 | 7 | 8 |
| 2 (D24) | # | * | 0 |
| 3 (D25) | 3 | 1 | 2 |

So the keypad's own rows come out of the scan in the order 4-5-6, 7-8-9, *-0-#, 1-2-3, with the columns rotated by one. `C,MATRIX:1` prints the raw readings if the keys ever need checking again.

## Panel brightness (pot switched off)

The PCB has a brightness pot on A0 (5 V to ground, wiper to A0). The sketch can read it, smooth it (a running average over 8 samples every 20 ms), turn it into a **level 0 to 15** with hysteresis, set its own backlight from it and send `C,B:<level>` to the other panels. This is **switched off for the moment**: `USE_BRIGHTNESS_POT` at the top of the brightness code is 0, so the pot is not read, no `C,B` packets are sent, and this board's own backlight stays at a fixed default of level 4 (PWM 68, close to the other boards' default of 65). Set `USE_BRIGHTNESS_POT` to 1 and reflash to bring it back. With it on, the level goes to the Radio .101, Servo .102, Upper .103, Stepper .105 and Dual Stepper .106 (not the USB HID board), one board per pass of the loop so a switched-off board costs at most one send timeout, once 5 seconds after start-up and then whenever it changes. The other panels' brightness meanwhile follows the SwingTester slider and their own defaults.

## Power monitoring

- **12 V rail:** R3 47 kohm from the rail and R4 10 kohm to ground with C1 0.1 uF across R4, midpoint to A15. The sketch averages 16 readings and scales them by the measured 5 V: volts = pin volts x (47 + 10) / 10. At 12 V the pin sees about 2.1 V, and the circuit reads up to about 28 V.
- **5 V rail:** the ADC reference is AVcc, the 5 V rail the AREF pin follows (the AREF pin is only brought out to a header, and the sketch **leaves the reference at its default**: `analogReference()` is never called). The sketch measures it by converting the chip's internal 1.1 V band-gap against AVcc (ATmega2560 `ADMUX` selection `11110` with `MUX5` 0): AVcc in millivolts = 1125300 / reading. It waits 5 ms after switching the input and throws away the first conversion.
- **Status:** `OK`, or the rails outside their limits joined with `+`: `5V_LOW` (below 4.75 V), `5V_HIGH` (above 5.25 V), `12V_LOW` (below 10.8 V), `12V_HIGH` (above 15.0 V). `V12_CAL` and `V5_CAL` at the top of the sketch trim the readings against a meter.

A power status is sent 5 seconds after start-up, then **every 30 seconds**, and whenever `C,POWER:1` arrives. The same two readings go out with every 10 second heartbeat, which the Health Monitor shows in its voltage table (the ADF is the only board with a 12 V measurement).

## What it sends

| To | Port | Content |
|---|---|---|
| 172.16.1.10 (PC) | 27003 | `ADF_FREQ:<kHz>`, `RADAR_FOLLOW_ID:<digits>` (`0` = stop following) and `POWER,V12:12.41,V5:5.02,STATUS:OK` |
| 172.16.1.10 (PC bridge) | 27001 | The gear, flaps and speed brake switch **positions** as `GEAR_SET:1` / `GEAR_SET:0`, `FLAPS_SET:1/0` and `SPOILERS_SET:1/0`, and `PARKING_BRAKES` when the brakes button is pressed |
| 172.16.1.11 (USB HID board) | 4210 | The four switches and the two ADF knobs as joystick buttons `index,state`: indexes 104 to 107 and 108 to 111 (Joystick 4, buttons 9 to 16) |
| 172.16.1.10 | 27000 | Debug text, each line prefixed `Jet Ranger ADF:`, including every power status |
| 172.16.1.10 | 13137 | `ADF_PANEL V5:5.03 V12:12.41` heartbeat every 10 seconds |

The events use **port 27003, not 27001**, because the FSUIPC and MSFS bridges treat any unrecognised string on 27001 as a COM1 standby frequency and would retune COM1. **Neither bridge reads port 27003 yet**, so the ADF frequency and the radar follow ID do not reach the simulator. The switch commands do go to 27001, which both bridges recognise.

## Panel switches

The landing gear (D29), flaps (D30), speed brakes (D31) and brakes (D32) are read with pull-ups and a 30 ms debounce; a closed switch (to ground) is "on". For the first 1.5 seconds after power-up they are only read. The gear, flaps and speed brake switches are **toggle switches**, so each change sends the joystick button (`index,1` closed or `index,0` open) and the switch's **position as a value** to the bridge: `GEAR_SET:1` when the gear switch is closed (gear down) and `GEAR_SET:0` when it is open, and the same for `FLAPS_SET` (flaps fully down or up) and `SPOILERS_SET` (speed brakes fully out or in). The positions are sent again **every 5 seconds**, so the simulator always follows the switches, including the position they were in at power-up: about 5 seconds after the panel is switched on the aircraft's gear, flaps and speed brakes move to match. This replaced a scheme that sent an event on each change (`GEAR_DOWN` / `GEAR_UP` and so on). The brakes are a push button: its press sends `PARKING_BRAKES` (a toggle) and the joystick button is held while it is pressed. If a switch works the wrong way round in the simulator, swap its 1 and 0 in the sketch. Each ADF knob click is also a 40 ms joystick press (major clockwise and anticlockwise 108 and 109, minor 110 and 111). The full joystick and command table is in the Radio controller chapter.

## Landing gear lamps

The three lamps on the ADF panel (nose D2, left D3, right D4, each LED with its own 330 ohm resistor to ground on the panel) are lit by a data packet to port 13136: `D,LDGN:1,LDGL:1,LDGR:0` (1 = lit, 0 = off). The SwingTester has a toggle for each lamp. Neither bridge sends them yet (the gear position SimVars and offsets are not read), so for now they are a bench test.

## What it receives

On UDP 13136: control packets (`C,<COMMAND>:<value>`) and data packets (only `LDGN`, `LDGL` and `LDGR` are used; every other token is ignored). There is **no no-data watchdog** and no power-down of the display.

| Command | Action |
|---|---|
| `C,RESET:1` | Software reset (jump to the reset vector), as on the Radio controller |
| `C,MATRIX:1` | Print the raw keypad readings to the debug stream (idle line, then one line per row with that row driven low) |
| `C,POWER:1` | Send a power status now |

## OLED

The two-line, 16-character OLED is on the Mega's own I2C bus (no TCA9548A) and uses the Radio controller's start-up sequence. Line 1 is `ADF  340 kHz`. Line 2 is `ID: 1234_` while an ID is being typed, `FOLLOW 1234` for the followed ID, or `FOLLOW OFF`.

## Notes

- The knob directions have not been checked. If a knob tunes the wrong way, swap its two pins in the `RotaryEncoder` lines.
- The display code is the Radio controller's character OLED code. If the ADF panel's display is a graphics SSD1306 it needs different code.
- The rail limits (5 V: 4.75 to 5.25, 12 V: 10.8 to 15.0) are first guesses; check them against the real supply.
- The ADF frequency and the radar follow ID go to port 27003, which the bridges do not read, so they are not used in the simulator yet.
