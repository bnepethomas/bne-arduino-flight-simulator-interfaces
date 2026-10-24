# ADF controller (172.16.1.107)

`JET_RANGER_ADF_CONTROLLER` runs on an Arduino Mega 2560 with a W5500 Ethernet module, on the **Jet Ranger ADF Controller** PCB (revision 1.4). It is built from the Radio controller sketch (same Ethernet start-up, debug stream, heartbeat, polling encoder library, matrix scanner, character OLED start-up and software reset) and does five jobs: it tunes the ADF with two knobs, takes the ID of an aircraft to follow on the radar from a keypad, reads the **panel brightness pot** and sends the level to the other panels, and monitors the **12 V and 5 V** supply rails. The sketch compiles for `arduino:avr:mega` (25 KB of flash, 1.5 KB of RAM). It has **not been flashed or bench-tested** yet.

![ADF controller blocks](diagrams/adf_blocks.svg)

## Program flow

1. **Setup:** set the status LEDs, read the brightness pot and set the board's own backlight, reset the W5500, start Ethernet with the 10 ms retransmission timeout and open the sockets (red LED flashes for 3 seconds), start the I2C character OLED, start the two encoders (centred at 1000) and the matrix scanner.
2. **Main loop:**
   - toggle the status LEDs every 200 ms and send the `ADF_PANEL` heartbeat every 10 seconds;
   - read both encoders. Each is kept near the middle of its range and recentred after every read, so the change since the last read is the distance from the middle; three counts make one click. The major knob changes the ADF frequency by 100 kHz per click and the minor knob by 1 kHz per click (the minor knob speeds up when it is turned quickly), clamped to 190 to 1799 kHz. 250 ms after the last movement the frequency is sent as `ADF_FREQ:<kHz>`;
   - `scanMatrix()` scans the 4 by 3 keypad with a 50 ms debounce. For the first 1.5 seconds after start-up it only records the state, so a floating column is not reported as key presses;
   - the keys build an aircraft ID: digits 0 to 9 add to it (up to 8), `#` sends `RADAR_FOLLOW_ID:<digits>` and makes it the followed ID, `*` deletes the last digit, and `*` on an empty entry stops following (`RADAR_FOLLOW_ID:0`). An unfinished entry is dropped after 20 seconds;
   - read the brightness pot (below) and send any pending brightness packet;
   - redraw the OLED when something changed (at most every 50 ms);
   - send the power status (below);
   - poll port 13136 and act on the control packets.

## Pin usage

| Pin | Function |
|---|---|
| D2, D3, D4 | Landing-gear lamps (nose, left, right): on the PCB, not driven yet |
| D5 | `BACKLIGHT_PWM`: this board's backlight MOSFET (IRF540N) |
| D6, D7 | ADF major encoder A, B |
| D8, D9 | ADF minor encoder A, B |
| D14, D15 | Green and red status LEDs |
| D20, D21 | I2C SDA and SCL (character OLED at 0x3C) |
| D22 to D25 | Keypad rows (`KEYPAD_7` to `KEYPAD_4`) |
| D26 to D28 | Keypad columns (`KEYPAD_3` to `KEYPAD_1`) |
| D29 to D32 | Landing gear, flaps, speed brakes and brakes switches (to ground when on) |
| D53 | W5500 reset |
| A0 | Panel brightness pot wiper (`GUAGE_BACKLIGHT_BRIGHT`) |
| A15 | 12 V monitor (`12V-Monitor`) |

## Panel brightness

The pot on A0 sits between 5 V and ground. The reading is smoothed (a running average over 8 samples every 20 ms) and converted to a **level 0 to 15**; a new level is accepted only once the pot is clearly past the boundary, so a reading on a boundary does not flicker. A new level:

- sets this board's own backlight (D5, PWM 0 to 255);
- is sent as `C,B:<level>` on UDP 13136 to the Radio .101, Servo .102, Upper .103, Stepper .105 and Dual Stepper .106 (not the USB HID board).

The level is also sent once 5 seconds after start-up, so the other panels take the pot's position without it being moved. The packets go out **one board per pass of the loop**, so a switched-off board costs at most one 80 ms send timeout and the knobs and keypad keep working. The Radio, Servo, Stepper and Dual Stepper boards apply the level (the Radio also uses it for its panel LEDs); the Upper controller accepts it but does not drive a backlight yet. A brightness packet counts as received data for the receiving board's 30 second no-data watchdog, but it is sent only when the pot changes (and once at start-up), so it does not hold a board's watchdog off.

## Power monitoring

- **12 V rail:** R3 47 kohm from the rail and R4 10 kohm to ground with C1 0.1 uF across R4, midpoint to A15. The sketch averages 16 readings and scales them by the measured 5 V: volts = pin volts x (47 + 10) / 10. At 12 V the pin sees about 2.1 V, and the circuit reads up to about 28 V.
- **5 V rail:** the ADC reference is AVcc, the 5 V rail the AREF pin follows (the AREF pin is only brought out to a header, and the sketch **leaves the reference at its default**: `analogReference()` is never called). The sketch measures it by converting the chip's internal 1.1 V band-gap against AVcc (ATmega2560 `ADMUX` selection `11110` with `MUX5` 0): AVcc in millivolts = 1125300 / reading. It waits 5 ms after switching the input and throws away the first conversion.
- **Status:** `OK`, or the rails outside their limits joined with `+`: `5V_LOW` (below 4.75 V), `5V_HIGH` (above 5.25 V), `12V_LOW` (below 10.8 V), `12V_HIGH` (above 15.0 V). `V12_CAL` and `V5_CAL` at the top of the sketch trim the readings against a meter.

A power status is sent 5 seconds after start-up, then **every 30 seconds**, and whenever `C,POWER:1` arrives.

## What it sends

| To | Port | Content |
|---|---|---|
| 172.16.1.10 (PC) | 27003 | `ADF_FREQ:<kHz>`, `RADAR_FOLLOW_ID:<digits>` (`0` = stop following) and `POWER,V12:12.41,V5:5.02,STATUS:OK` |
| 172.16.1.101, .102, .103, .105, .106 | 13136 | `C,B:<0-15>` panel brightness |
| 172.16.1.10 (PC bridge) | 27001 | The four panel switches as simulator controls: `GEAR_DOWN` / `GEAR_UP`, `FLAPS_DOWN` / `FLAPS_UP`, `SPOILERS_ON` / `SPOILERS_OFF` and `PARKING_BRAKES` (the bridges recognise these names) |
| 172.16.1.11 (USB HID board) | 4210 | The four switches and the two ADF knobs as joystick buttons `index,state`: indexes 104 to 107 and 108 to 111 (Joystick 4, buttons 9 to 16) |
| 172.16.1.10 | 27000 | Debug text, each line prefixed `Jet Ranger ADF:`, including every power status |
| 172.16.1.10 | 13137 | `ADF_PANEL` heartbeat every 10 seconds |

The events use **port 27003, not 27001**, because the FSUIPC and MSFS bridges treat any unrecognised string on 27001 as a COM1 standby frequency and would retune COM1. **Neither bridge reads port 27003 yet**, so the ADF frequency and the radar follow ID do not reach the simulator, and the Health Monitor has no light for `ADF_PANEL`.

## Panel switches

The landing gear (D29), flaps (D30), speed brakes (D31) and brakes (D32) are read with pull-ups and a 30 ms debounce; a closed switch (to ground) is "on". For the first 1.5 seconds after power-up they are only read. Each change sends the joystick button (`index,1` closed or `index,0` open) and the bridge command: the gear switch sends `GEAR_DOWN` when it closes and `GEAR_UP` when it opens, the flaps `FLAPS_DOWN` and `FLAPS_UP`, the speed brakes `SPOILERS_ON` and `SPOILERS_OFF`, and the brakes button sends `PARKING_BRAKES` when it is pressed. When the quiet period ends the joystick buttons are brought into line with the switches as they are, but **no simulator command is sent**, so the aircraft is not moved at power-up. If a switch works the wrong way round, swap its two commands in `adfSwitches[]`. Each ADF knob click is also a 40 ms joystick press (major clockwise and anticlockwise 108 and 109, minor 110 and 111). The full joystick and command table is in the Radio controller chapter.

## What it receives

Only control packets on UDP 13136 (`C,<COMMAND>:<value>`); data packets are ignored and there is **no no-data watchdog** and no power-down of the display.

| Command | Action |
|---|---|
| `C,RESET:1` | Software reset (jump to the reset vector), as on the Radio controller |
| `C,MATRIX:1` | Print the raw keypad readings to the debug stream (idle line, then one line per row with that row driven low) |
| `C,POWER:1` | Send a power status now |

## OLED

The two-line, 16-character OLED is on the Mega's own I2C bus (no TCA9548A) and uses the Radio controller's start-up sequence. Line 1 is `ADF  340 kHz`. Line 2 is `ID: 1234_` while an ID is being typed, `FOLLOW 1234` for the followed ID, or `FOLLOW OFF`.

## Notes

- The keypad assumes the PCB's connector order puts D22 to D25 on the four rows and D26 to D28 on the three columns, as on the Radio controller. This is **not verified**; use `C,MATRIX:1` and change `rowPins`, `colPins` and `keyMap` if it is wired the other way.
- The knob directions have not been checked. If a knob tunes the wrong way, swap its two pins in the `RotaryEncoder` lines.
- The display code is the Radio controller's character OLED code. If the ADF panel's display is a graphics SSD1306 it needs different code.
- The landing-gear lamps and the landing-gear, flaps, speed-brake and brake inputs are not used yet.
- The rail limits (5 V: 4.75 to 5.25, 12 V: 10.8 to 15.0) are first guesses; check them against the real supply.
