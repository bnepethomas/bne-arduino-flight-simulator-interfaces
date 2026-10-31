# JET_RANGER_ADF_CONTROLLER - Program Summary

Sketch for the **Jet Ranger Upper ADF Controller** PCB (rev 1.0, 26 October 2026, the combined ADF and Upper board and the reference PCB for this sketch): an Arduino Mega 2560 shield with a W5500 Ethernet module
(USR-ES1). Built from `JET_RANGER_RADIO_CONTROLLER` (same Ethernet start-up, debug stream, heartbeat, polling encoder
library, matrix scanner, character OLED start-up and remote reset). Address **172.16.1.107**, MAC `A8:61:0A:67:83:6B`.
On 31 October 2026 the former `JET_RANGER_UPPER_CONTROLLER` sketch (172.16.1.103) was **merged into this one**: the overhead switch matrix and the Upper numeric keypad are scanned here too (see "Upper panel" below), and the old Upper sketch is retired (its DCS-BIOS stubs, every case empty, were not carried over).
It compiles for `arduino:avr:mega` (29,296 bytes flash, 2,557 bytes RAM) and is flashed on COM26. Bench-checked: the 12 V reading (A8), the landing gear lamps (from the SwingTester) and the ADF keypad on the combined board. Not bench-tested on the combined board: the Upper matrix and keypad, the switches at their new pins (D31 to D37) and the blanking and backlight rules (below).

## What it does

| Function | Pins | Behaviour |
|---|---|---|
| ADF tuning | D6/D7 major, D8/D9 minor | Major knob 100 kHz per click, minor knob 1 kHz per click (accelerates when spun). Range 190 to 1799 kHz, starts at 340. The frequency is sent 250 ms after the knobs stop |
| Aircraft ID keypad | A7, A6, A5, A4 rows, A3, A2, A1 columns | 4 x 3 keypad (J41). The keypad lines arrive in reverse order on this PCB, so the rows are A7 to A4 and the columns A3 to A1 (the old ADF board used D22 to D25 and D26 to D28). Digits build an ID (up to 8), `#` sends it, `*` deletes a digit, `*` on an empty entry stops following. An unfinished entry is dropped after 20 s. `keyMap[][]` holds {6,4,5}, {9,7,8}, {#,*,0}, {3,1,2}: the labels moved to where each key was actually reported on the old board, and confirmed correct on the combined board (31 October 2026) |
| Upper switch matrix | D22 to D30 rows (`ROW0` to `ROW8`), D38 to D45 columns (`COL0` to `COL7`) | 9 x 8 diode-isolated matrix (J30, J31, 10 k column pull-ups). One row is scanned per pass of the main loop (80 us settle, 20 ms lockout after each change). A change is sent to the USB HID board as `index,state` with `index = row x 11 + column` (0 to 95: the old 11 column numbering is kept so the joystick buttons and bindings do not change). The Warning Mute (J17) and TOT Test (J18) switches are indexes 88 and 89. Switches closed at power-up are reported straight away |
| Upper keypad | A10, A12, A14, A15 rows, A9, A11, A13 columns | 4 x 3 keypad (J32), keys `2 1 3 / 0 * # / 8 7 9 / 5 4 6`. A press and release are sent to the HID board as `KEY,<key>,1` and `KEY,<key>,0` (non-blocking: the old sketch waited in a loop for the release) |
| Panel brightness | A0 pot, D5 backlight | **Switched off for the moment** (`USE_BRIGHTNESS_POT` 0): the pot is not read, no `C,B` is sent to the other panels and this board's backlight stays at level 4 (PWM 68) until a `C,B` command sets it. Set it to 1 and reflash to bring the pot back (level 0 to 15 from the smoothed pot, sent to the Radio .101, Servo .102, Stepper .105 and Dual Stepper .106) |
| 12 V monitor | A8 | 47k / 10k divider with 0.1 uF, so volts = pin volts x 5.7, scaled by the measured 5 V. On the combined board the divider is on **A8** (the old ADF board used A15, now an Upper keypad line: the sketch reported 5.33 V with 12.3 V on the board until it was changed; 2.12 V measured on A8 matched the divider) |
| 5 V monitor | AREF / AVcc | The ADC reference (AVcc, the 5 V rail the AREF pin follows) is measured against the internal 1.1 V band-gap. The sketch never calls `analogReference()` |
| Power status | | Every 30 s (and 5 s after start-up, or on `C,POWER:1`), see below |
| Display | I2C 0x3C | Character OLED, 2 lines of 16: `ADF  340 kHz` and `ID: 1234_` / `FOLLOW 1234` / `FOLLOW OFF`. Written directly on the Mega's I2C bus (no TCA9548A) |

The landing gear lamps (D2 to D4) are lit by data packets: `D,LDGN:1,LDGL:1,LDGR:0` (1 = lit); the SwingTester has a toggle for each. The landing gear, flaps, speed brake and brake switches
(D31 to D34, to ground when on, 30 ms debounce) are sent as joystick buttons to the USB HID board (172.16.1.11:4210, Joystick 4, indexes 104 to 107, `index,state`). The gear, flaps and speed brake are
**toggle switches, so their position is sent as a value** to the bridge on UDP 27001: `GEAR_SET:1` (down) / `GEAR_SET:0` (up), `FLAPS_SET:1/0` and `SPOILERS_SET:1/0` on every change and again every
5 s (`POSITION_RESEND_MS`), so the simulator follows the switches, including their position at power-up (the aircraft moves about 5 s after the panel is switched on). The brakes push button sends
`PARKING_BRAKES` when pressed. For 1.5 s after start-up the switches are only read. If a switch works the wrong way round, swap its 1 and 0. Each ADF knob click is also a 40 ms press of Joystick 4 buttons
(major clockwise / anticlockwise 108 / 109, minor 110 / 111). The heartbeat is `ADF_PANEL V5:5.03 V12:12.41` (5 V and 12 V rails) for the Health Monitor, followed by the same readings as `UPPER_INPUT V5:5.03 V12:12.41` (the old Upper board's name, so the monitor's Upper light stays green).

## Network

| Direction | Port | Contents |
|---|---|---|
| In, UDP 13136 | control and data | `C,RESET:1` reboot, `C,MATRIX:1` raw keypad dump to the debug stream, `C,POWER:1` send a power status now, `C,B:0-15` backlight brightness, `C,KEEPALIVE:1`, `C,DISPLAY:0/1` (from the Radio). Data packets: `LDGN`, `LDGL`, `LDGR` (lamps) and `BPWR` (backlight power) |
| Out to 172.16.1.10:27000 | debug | Text, prefixed `Jet Ranger ADF:` |
| Out to 172.16.1.10:13137 | heartbeat | `ADF_PANEL` and `UPPER_INPUT` every 10 s |
| Out to 172.16.1.11:4210 | USB HID board | ADF switches and knobs (indexes 104 to 114), the Upper matrix (`index,state`, 0 to 95) and the Upper keypad (`KEY,key,state`) |
| Out to 172.16.1.10:27003 | PC events | `ADF_FREQ:830`, `RADAR_FOLLOW_ID:4521` (`RADAR_FOLLOW_ID:0` = stop), `POWER,V12:12.41,V5:5.02,STATUS:OK` |

The events use port 27003, not 27001, because the FSUIPC and MSFS bridges treat any unrecognised string on 27001 as a COM1 standby
frequency. **The bridges do not read port 27003 yet**, so nothing reaches the simulator until they are extended.

`STATUS` is `OK`, or the rails out of range joined with `+` (`5V_LOW`, `5V_HIGH`, `12V_LOW`, `12V_HIGH`). Limits: 5 V 4.75 to 5.25, 12 V 10.8 to 15.0
(`V5_LOW`, `V5_HIGH`, `V12_LOW`, `V12_HIGH`). `V12_CAL` and `V5_CAL` trim the readings against a meter.

## Things to check on the bench

- **ADF keypad rows and columns.** Checked on the combined board: the keypad lines are reversed (rows A7 to A4, columns A3 to A1). `C,MATRIX:1` prints the raw readings if the keys ever need checking again.
- **Upper matrix and keypad.** Operate each switch and key and watch the debug stream for `Front Input - <index>:<state>` and `Upper key pressed: <key>`; indexes 66 to 95 share Joystick 3 with the Auto Pilot panel's buttons (64 to 81), so do not fit switches at 66 to 73 or 77 to 81.
- **Retired address.** Nothing answers at 172.16.1.103 any more; the bridges and the SwingTester still send to it (harmless, to be removed).
- **Encoder direction.** If a knob turns the frequency the wrong way, swap its A and B pins in the `RotaryEncoder` lines.
- **OLED type.** The display code is the Radio controller's character OLED (SSD/US2066 style, address 0x3C). If the ADF panel uses a graphics SSD1306 it needs different code.
- **Rail readings.** Compare the first power status lines with a meter and trim `V12_CAL` and `V5_CAL`.

## New switches, blanking and backlight (2026-10-25, pins updated 2026-10-31)

- **New switches:** anti-ice (D36, connector J20, Joystick 4 button 17 / index 112), hyd boost (D35, J20, button 18 / 113) and caution dim (D37, J20, button 19 / 114) are in `adfSwitches[]`, to ground when on. Anti-ice also sends `ANTI_ICE_SET:1` / `ANTI_ICE_SET:0` to the bridge on UDP 27001 (on every change and every 5 s, like the other toggle switches); the other two are joystick buttons only. For the DodoSim 206 V2, bind the joystick buttons in FSUIPC to `ANTI_ICE_SWITCH_SET`, `HYDRAULICS_SWITCH_SET` and `CAUTION_LIGHT_SWITCH_SET` (1 on press, 0 on release) and use either that or the bridge's `ANTI_ICE_SET` for anti-ice, not both.
- **Keypad connector:** the ADF keypad lines are on pins 18 to 24 of the 26-pin ADF connector J5 and reach the Mega on A1 to A7 (J41); the ADF panel connectors J5 and J20 keep their pin-outs on the combined board.
- **Panel blanking:** `panelBlank()` is true while the Radio has told the board `C,DISPLAY:0` (`displayOn` false) or the no-data timeout has fired (`noDataBlanked`: 30 s, `NO_DATA_TIMEOUT_MS`, with no data packet, `C,B` or `C,KEEPALIVE` and no local input: a knob click, a key or a switch change, `noteActivity()`). It clears the OLED and puts the landing gear lamps out (`lampWanted[]` remembers what they should show).
- **Backlight:** `applyOwnBacklight()` is off while `BPWR:0` was received (`backlightPowered`) or the no-data timeout has fired; otherwise the level is `brightnessLevel` (level 4 by default, then `C,B:0-15` while `USE_BRIGHTNESS_POT` is 0). The bridges send `D,BPWR:x` to this board, and the SwingTester's brightness, backlight power and keep-alive include it.
- Compiled at 29,296 bytes flash (11%) / 2,557 bytes RAM (31%) with the Upper code merged in.
