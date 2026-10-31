# ADF and Upper controller (172.16.1.107)

`JET_RANGER_ADF_CONTROLLER` runs on an Arduino Mega 2560 with a W5500 Ethernet module, on the **Jet Ranger Upper ADF Controller** PCB (revision 1.0, 26 October 2026), the combined ADF and Upper board, which is now the reference PCB for this sketch. On 31 October 2026 the former Upper controller sketch (`JET_RANGER_UPPER_CONTROLLER`, 172.16.1.103) was merged into it, so this one board (172.16.1.107) does the work of both old boards: it also scans the **overhead switch matrix** and the **Upper numeric keypad** and sends them to the USB HID board (see Upper panel matrix and keypad). The old Upper sketch is still in the repository but is retired, and its DCS-BIOS stubs (every case was empty) were not carried over. The sketch is built from the Radio controller sketch (same Ethernet start-up, debug stream, heartbeat, polling encoder library, matrix scanner, character OLED start-up and software reset) and does these jobs: it tunes the ADF with two knobs, takes the ID of an aircraft to follow on the radar from a keypad, sends the landing gear, flaps, speed brake, brakes, anti-ice, hyd boost and caution dim switches to the simulator (as joystick buttons and, for most of them, as bridge commands), drives the three landing gear lamps, blanks its display, lamps and backlight with the other panels (the Radio's displays, the main bus and a 30 second no-data timeout), and monitors the **12 V and 5 V** supply rails. The PCB also has a **panel brightness pot**, which the sketch can read but has switched off for the moment (see Panel brightness). The sketch compiles for `arduino:avr:mega` (29,296 bytes of flash, 2,557 bytes of RAM) and has been flashed to the board (COM26). On the bench so far: the 12 V reading (after the pin fix below), the landing gear lamps (from the SwingTester) and the ADF keypad, whose key labels were checked on this board. The Upper matrix and Upper keypad, the three newer ADF switches in their new positions, and the blanking and backlight rules have not been bench-tested on the combined board yet.

![ADF controller blocks](diagrams/adf_blocks.svg)

## Program flow

1. **Setup:** set the status LEDs, set the board's own backlight (the pot is not read), reset the W5500, start Ethernet with the 10 ms retransmission timeout and open the sockets (red LED flashes for 3 seconds), start the I2C character OLED, start the two encoders (centred at 1000), the ADF keypad scanner, the Upper matrix and keypad, the panel switches and the landing gear lamps.
2. **Main loop:**
   - toggle the status LEDs every 200 ms and send the `ADF_PANEL` heartbeat every 10 seconds, with the 5 V and 12 V readings (`ADF_PANEL V5:5.03 V12:12.41`) for the Health Monitor, followed by the same readings under the old Upper board's name, `UPPER_INPUT`, so the Health Monitor's Upper light stays green;
   - read both encoders. Each is kept near the middle of its range and recentred after every read, so the change since the last read is the distance from the middle; three counts make one click. The major knob changes the ADF frequency by 100 kHz per click and the minor knob by 1 kHz per click (the minor knob speeds up when it is turned quickly), clamped to 190 to 1799 kHz. 250 ms after the last movement the frequency is sent as `ADF_FREQ:<kHz>`, and each click is also a joystick button press;
   - `scanMatrix()` scans the ADF keypad (4 by 3) with a 50 ms debounce. For the first 1.5 seconds after start-up it only records the state, so a floating column is not reported as key presses;
   - `scanUpperMatrixRow()` scans **one row** of the Upper switch matrix per pass (a whole scan is 9 passes, so the encoders keep being polled between rows) and `scanUpperKeypad()` scans the Upper keypad; changes go to the USB HID board;
   - the keys build an aircraft ID: digits 0 to 9 add to it (up to 8), `#` sends `RADAR_FOLLOW_ID:<digits>` and makes it the followed ID, `*` deletes the last digit, and `*` on an empty entry stops following (`RADAR_FOLLOW_ID:0`). An unfinished entry is dropped after 20 seconds;
   - read the seven panel switches and send their changes (below);
   - redraw the OLED when something changed (at most every 50 ms);
   - send the power status (below);
   - check the no-data timeout (30 seconds with no data packet and no local input blanks the panel, see Panel blanking and backlight);
   - poll port 13136 and act on the control and data packets.

## Pin usage

| Pin | Function |
|---|---|
| D2, D3, D4 | Landing gear lamps (nose, left, right), driven from data packets (high = lit) |
| D5 | `BACKLIGHT_PWM`: this board's backlight MOSFET (IRF540N), driven from `BPWR`, `C,B` and the no-data timeout (see Panel blanking and backlight) |
| D6, D7 | ADF major encoder A, B |
| D8, D9 | ADF minor encoder A, B |
| D14, D15 | Green and red status LEDs |
| D20, D21 | I2C SDA and SCL (character OLED at 0x3C) |
| D22 to D30 | Upper switch matrix rows `ROW0` to `ROW8` (connector J30; `ROW8` also feeds the Warning Mute J17 and TOT Test J18 switches) |
| D31 to D34 | Landing gear, flaps, speed brakes and brakes switches (to ground when on) |
| D35 | Hyd boost switch (`HYD_BOOST`, connector J20; to ground when on) |
| D36 | Anti-ice switch (`ANTI_ICE`, connector J20; to ground when on) |
| D37 | Caution dim switch (`CAUTION_DIM`, connector J20; to ground when on) |
| D38 to D45 | Upper switch matrix columns `COL0` to `COL7` (connector J31), each with a 10 k pull-up to 5 V (R40 to R47) |
| D53 | W5500 reset |
| A0 | Panel brightness pot wiper (`GUAGE_BACKLIGHT_BRIGHT`), not read while the pot is switched off |
| A1 to A7 | ADF keypad lines `ADF_KEYPAD_1` to `ADF_KEYPAD_7` (J41 pins 2 to 8): columns A3, A2, A1 and rows A7, A6, A5, A4 |
| A8 | 12 V monitor (`12V-Monitor`) |
| A9 to A15 | Upper numeric keypad `UPPER_KEYPAD_1` to `UPPER_KEYPAD_7` (J32): rows A10, A12, A14, A15, columns A9, A11, A13 |

D46 to D48 are not connected and D49 is the `RESET_pin` header (J2). Pins that moved when the boards were combined: the ADF switches from D29 to D35 to D31 to D37 (hyd boost D35, anti-ice D36 and caution dim D37 are in a different order from the old ADF board), the ADF keypad from D22 to D28 to A1 to A7 and the 12 V monitor from A15 to A8. The ADF panel connectors J5 and J20 keep their pin-outs.

## ADF keypad

The seven ADF keypad lines `ADF_KEYPAD_1` to `ADF_KEYPAD_7` come from pins 2 to 8 of connector J41 on A1 to A7, and reach the keypad through the ADF panel connector J5 (pins 18 to 24). On this PCB the keypad lines arrive **in reverse order**, so the sketch scans **A7, A6, A5 and A4 as the four rows and A3, A2 and A1 as the three columns** (the old ADF board used D22 to D25 as rows and D26 to D28 as columns). The key labels are unchanged from the old board, where the keys were pressed one by one and the labels in `keyMap[][]` moved to where each key was actually reported:

| Scan row | Column 0 (A3) | Column 1 (A2) | Column 2 (A1) |
|---|---|---|---|
| 0 (A7) | 6 | 4 | 5 |
| 1 (A6) | 9 | 7 | 8 |
| 2 (A5) | # | * | 0 |
| 3 (A4) | 3 | 1 | 2 |

With the first pin assignment on the combined board (rows A1 to A4, columns A5 to A7) keys 1, 2 and 3 did nothing and most of the others reported as the wrong key, which showed the reversal. After the swap every key reports correctly (checked on the bench, 31 October 2026). `C,MATRIX:1` prints the raw readings if the keys ever need checking again.

## Upper panel matrix and keypad

These come from the former Upper controller. The Upper panel is the overhead switch panel: a diode-isolated switch matrix and a small numeric keypad.

- **Matrix:** 9 rows (`ROW0` to `ROW8`, D22 to D30) by 8 columns (`COL0` to `COL7`, D38 to D45). One row at a time is driven low and the columns are read with pull-ups; a switch closed to ground reads low. A change is sent to the USB HID board as `index,state` with `index = row x 11 + column`. The stride of 11 is the old board's 11 columns, kept so that the joystick button numbers (0 to 95, Joystick 1 to 3) and the existing FSUIPC and MSFS assignments are the same as before. Changes of one switch are locked out for 20 ms after each report. Switches that are closed at power-up are reported straight away, so the HID board's buttons follow the panel.
- **Scan method:** the old sketch drove 16 rows with direct port writes and scanned all of them on every pass. The combined board has 9 rows (D22 to D30, the same first nine rows as before, so the indexes do not change) and D31 to D37 are now ADF switches, so the scan uses `digitalWrite`, with an 80 microsecond settling time before the columns are read, and does **one row per pass of the main loop** so the ADF encoders keep being polled.
- **Smaller than before:** rows 9 to 15 and columns 8 to 10 of the old matrix no longer exist on the PCB, so the Upper matrix stops at index 95 (the old board had 176 positions). Indexes 96 and above belong to the Audio and ADF panels and the matrix no longer reaches them, but rows 6 to 8 (indexes 66 to 95) still fall inside Joystick 3, shared with the Auto Pilot panel's buttons 64 to 81, so a switch fitted at 66 to 73 or 77 to 81 would clash.
- **Warning Mute and TOT Test:** the PCB's Warning Mute (J17) and TOT Test (J18) switches sit on `ROW8` (D30) behind diodes D5 and D6 on `COL0` and `COL1`, so they are read by the matrix as indexes 88 and 89. The Servo board's own mute and TOT clear buttons (indexes 7 and 8) are unchanged, and nothing is assigned to 88 and 89 yet. The earlier warning that the Upper sketch might not read them no longer applies.
- **Keypad:** 4 rows (A10, A12, A14, A15) by 3 columns (A9, A11, A13), keys `2 1 3 / 0 * # / 8 7 9 / 5 4 6` as laid out in the sketch. A press is sent as `KEY,<key>,1` and the release as `KEY,<key>,0`, typed on the HID board's virtual keyboard. The old sketch waited in a loop until the key was released, which would have stopped the ADF knobs; the merged scan does not wait.
- **Activity:** every matrix or keypad change counts as local input for the no-data timeout (see Panel blanking and backlight).
- **Brightness:** the Upper panel's backlight is this board's backlight (D5, connector J33): the Upper controller no longer has its own.

## Panel brightness (pot switched off)

The PCB has a brightness pot on A0 (5 V to ground, wiper to A0). The sketch can read it, smooth it (a running average over 8 samples every 20 ms), turn it into a **level 0 to 15** with hysteresis, set its own backlight from it and send `C,B:<level>` to the other panels. This is **switched off for the moment**: `USE_BRIGHTNESS_POT` at the top of the brightness code is 0, so the pot is not read, no `C,B` packets are sent, and this board's own backlight stays at the default level 4 (PWM 68, close to the other boards' default of 65) until a `C,B` command sets it (see Panel blanking and backlight). Set `USE_BRIGHTNESS_POT` to 1 and reflash to bring it back. With it on, the level goes to the Radio .101, Servo .102, Stepper .105 and Dual Stepper .106 (not the USB HID board; the Upper panel is on this board), one board per pass of the loop so a switched-off board costs at most one send timeout, once 5 seconds after start-up and then whenever it changes. The other panels' brightness meanwhile follows the SwingTester slider and their own defaults.

## Panel blanking and backlight

The panel follows the same rules as the other boards (see the network chapter). The display, the landing gear lamps and the backlight are controlled like this:

- **Radio displays (`C,DISPLAY`):** the Radio controller sends `C,DISPLAY:0` when it blanks its displays (no avionics power, or no data or input for 30 seconds) and `C,DISPLAY:1` when they come back, on every change and again every 10 seconds. The ADF controller then clears its OLED and puts the landing gear lamps out (it remembers what they should show and restores it). It starts with the display on, so a Radio that is off or missing never blanks it.
- **Main bus (`BPWR`):** the bridges send `D,BPWR:1/0` to the ADF controller, like the other boards. At 0 the backlight is off; at 1 it returns at the same level. Until the first one arrives the backlight is on.
- **Brightness (`C,B:0-15`):** sets the backlight level (0 off, 15 full, mapped to PWM 0 to 255) while the pot is switched off; the default is level 4. If `USE_BRIGHTNESS_POT` is 1 the pot is the source and `C,B` is ignored.
- **No-data timeout:** 30 seconds (`NO_DATA_TIMEOUT_MS`) with no data packet from the bridge, no brightness command or keep-alive and no local input (a knob click, a key or a switch change) blanks the OLED, the lamps and the backlight until something arrives, so a panel with no PC feed goes dark after power-up. Any of those brings the panel back at the same level. The Radio's `C,DISPLAY` messages do not count as data.

Both bridges send `D,BPWR:x` to the ADF controller (172.16.1.107) as well as to the Servo, Stepper, Dual Stepper and Radio, and the SwingTester's brightness slider, backlight power buttons and keep-alive include it. The bridges and the SwingTester **still also send to the retired Upper address 172.16.1.103**, which no longer exists; those packets go nowhere and are harmless, but the entries should be removed (on the to-do list).

## Power monitoring

- **12 V rail:** R3 47 kohm from the rail and R4 10 kohm to ground with C1 0.1 uF across R4, midpoint to **A8** (on the combined board; the old ADF-only board used A15, which is now an Upper keypad line: until the sketch was changed on 31 October 2026 it reported 5.33 V with 12.3 V on the board, and 2.12 V measured on A8 matched the divider). The sketch averages 16 readings and scales them by the measured 5 V: volts = pin volts x (47 + 10) / 10. At 12 V the pin sees about 2.1 V, and the circuit reads up to about 28 V.
- **5 V rail:** the ADC reference is AVcc, the 5 V rail the AREF pin follows (the AREF pin is only brought out to a header, and the sketch **leaves the reference at its default**: `analogReference()` is never called). The sketch measures it by converting the chip's internal 1.1 V band-gap against AVcc (ATmega2560 `ADMUX` selection `11110` with `MUX5` 0): AVcc in millivolts = 1125300 / reading. It waits 5 ms after switching the input and throws away the first conversion.
- **Status:** `OK`, or the rails outside their limits joined with `+`: `5V_LOW` (below 4.75 V), `5V_HIGH` (above 5.25 V), `12V_LOW` (below 10.8 V), `12V_HIGH` (above 15.0 V). `V12_CAL` and `V5_CAL` at the top of the sketch trim the readings against a meter.

A power status is sent 5 seconds after start-up, then **every 30 seconds**, and whenever `C,POWER:1` arrives. The same two readings go out with every 10 second heartbeat, which the Health Monitor shows in its voltage table (the ADF is the only board with a 12 V measurement).

## What it sends

| To | Port | Content |
|---|---|---|
| 172.16.1.10 (PC) | 27003 | `ADF_FREQ:<kHz>`, `RADAR_FOLLOW_ID:<digits>` (`0` = stop following) and `POWER,V12:12.41,V5:5.02,STATUS:OK` |
| 172.16.1.10 (PC bridge) | 27001 | The gear, flaps, speed brake and anti-ice switch **positions** as `GEAR_SET:1` / `GEAR_SET:0`, `FLAPS_SET:1/0`, `SPOILERS_SET:1/0` and `ANTI_ICE_SET:1/0`, and `PARKING_BRAKES` when the brakes button is pressed |
| 172.16.1.11 (USB HID board) | 4210 | The seven ADF switches and the two ADF knobs as joystick buttons `index,state`: indexes 104 to 107, 112 to 114 (the anti-ice, hyd boost and caution dim switches) and 108 to 111 (Joystick 4, buttons 9 to 19) |
| 172.16.1.10 | 27000 | Debug text, each line prefixed `Jet Ranger ADF:`, including every power status |
| 172.16.1.10 | 13137 | `ADF_PANEL V5:5.03 V12:12.41` heartbeat every 10 seconds, and the same readings again as `UPPER_INPUT V5:5.03 V12:12.41` (the old Upper board's name) |
| 172.16.1.11 (USB HID board) | 4210 | also the Upper switch matrix as `index,state` (0 to 95) and the Upper keypad as `KEY,key,state` |

The events use **port 27003, not 27001**, because the FSUIPC and MSFS bridges treat any unrecognised string on 27001 as a COM1 standby frequency and would retune COM1. **Neither bridge reads port 27003 yet**, so the ADF frequency and the radar follow ID do not reach the simulator. The switch commands do go to 27001, which both bridges recognise.

## Panel switches

Seven switches are read with pull-ups and a 30 ms debounce; a closed switch (to ground) is "on". For the first 1.5 seconds after power-up they are only read. Each change sends the joystick button (`index,1` closed or `index,0` open) and, for the ones with a command, the command to the bridge on UDP 27001:

| Switch | Pin | Joystick 4 button (index) | Bridge command |
|---|---|---|---|
| Landing gear (toggle) | D31 | 9 (104) | `GEAR_SET:1/0` |
| Flaps (toggle) | D32 | 10 (105) | `FLAPS_SET:1/0` |
| Speed brakes (toggle) | D33 | 11 (106) | `SPOILERS_SET:1/0` |
| Brakes (push button) | D34 | 12 (107) | `PARKING_BRAKES` when pressed |
| Hyd boost (toggle) | D35 | 18 (113) | none |
| Anti-ice (toggle) | D36 | 17 (112) | `ANTI_ICE_SET:1/0` |
| Caution dim (toggle) | D37 | 19 (114) | none |

The gear, flaps, speed brake and anti-ice switches are **toggle switches**, so each sends its **position as a value**: `GEAR_SET:1` when the gear switch is closed (gear down) and `GEAR_SET:0` when it is open, and the same for `FLAPS_SET` (flaps fully down or up), `SPOILERS_SET` (speed brakes fully out or in) and `ANTI_ICE_SET` (anti-ice on or off). The positions are sent again **every 5 seconds**, so the simulator always follows the switches, including the position they were in at power-up: about 5 seconds after the panel is switched on the aircraft's gear, flaps, speed brakes and anti-ice move to match. This replaced a scheme that sent an event on each change (`GEAR_DOWN` / `GEAR_UP` and so on). The brakes are a push button: its press sends `PARKING_BRAKES` (a toggle) and the joystick button is held while it is pressed. If a switch works the wrong way round in the simulator, swap its 1 and 0 in the sketch. Each ADF knob click is also a 40 ms joystick press (major clockwise and anticlockwise 108 and 109, minor 110 and 111). The full joystick and command table is in the Radio controller chapter.

The anti-ice, hyd boost and caution dim switches were added to the ADF PCB in its revision 1.5 (on the combined board they share the 6-pin connector J20: pin 3 ground, pin 4 hyd boost on D35, pin 5 anti-ice on D36, pin 6 caution dim on D37). Anti-ice also sends `ANTI_ICE_SET`: the FSUIPC bridge sends it as the FSUIPC control `ANTI_ICE_SET` (parameter 1 or 0) and the MSFS bridge as the SimConnect event `ANTI_ICE_SET`. **For the DodoSim 206 V2** the aircraft has its own events for these three switches, `ANTI_ICE_SWITCH_SET`, `HYDRAULICS_SWITCH_SET` (hyd boost) and `CAUTION_LIGHT_SWITCH_SET` (caution lights bright or dim), each taking 1 or 0, which FSUIPC binds to buttons through the `DodoSim206.evt` file. Its fallbacks for the plain simulator controls are toggles (Carb heat / engine anti-ice, and "Concorde nose & visor" retract and extend for hyd boost and caution dim), so a bridge has no clean position command for hyd boost or caution dim, and the Dodo may ignore the plain `ANTI_ICE_SET` event. The reliable route is to bind Joystick 4 buttons 17 to 19 (indexes 112 to 114) in FSUIPC's Buttons + Switches page to the three Dodo events, with 1 on press and 0 on release (the Dodo manual also advises turning on "Sync controls on load"), and to use either that binding or the bridge's `ANTI_ICE_SET`, not both.

## Landing gear lamps

The three lamps on the ADF panel (nose D2, left D3, right D4, each LED with its own 330 ohm resistor to ground on the panel) are lit by a data packet to port 13136: `D,LDGN:1,LDGL:1,LDGR:0` (1 = lit, 0 = off). The SwingTester has a toggle for each lamp. Neither bridge sends them yet (the gear position SimVars and offsets are not read), so for now they are a bench test. They go out with the rest of the panel (see Panel blanking and backlight) and come back as last set.

## What it receives

On UDP 13136: control packets (`C,<COMMAND>:<value>`) and data packets (`LDGN`, `LDGL` and `LDGR` for the lamps and `BPWR` for the backlight power; every other token is ignored). Every data packet, brightness command and keep-alive restarts the 30 second no-data timeout (see Panel blanking and backlight).

| Command | Action |
|---|---|
| `C,RESET:1` | Software reset (jump to the reset vector), as on the Radio controller |
| `C,MATRIX:1` | Print the raw ADF keypad readings to the debug stream (idle line, then one line per row with that row driven low) |
| `C,POWER:1` | Send a power status now |
| `C,DISPLAY:0` / `C,DISPLAY:1` | Sent by the Radio controller: blank the OLED and lamps, or restore them, with the Radio's displays |
| `C,B:<0-15>` | Backlight brightness (ignored while `USE_BRIGHTNESS_POT` is 1) |
| `C,KEEPALIVE:1` | Counts as data: restarts the no-data timeout |

## OLED

The two-line, 16-character OLED is on the Mega's own I2C bus (no TCA9548A) and uses the Radio controller's start-up sequence. Line 1 is `ADF  340 kHz`. Line 2 is `ID: 1234_` while an ID is being typed, `FOLLOW 1234` for the followed ID, or `FOLLOW OFF`. The OLED is cleared while the panel is blanked.

## Notes

- The knob directions have not been checked. If a knob tunes the wrong way, swap its two pins in the `RotaryEncoder` lines.
- The display code is the Radio controller's character OLED code. If the ADF panel's display is a graphics SSD1306 it needs different code.
- The rail limits (5 V: 4.75 to 5.25, 12 V: 10.8 to 15.0) are first guesses; check them against the real supply.
- The ADF frequency and the radar follow ID go to port 27003, which the bridges do not read, so they are not used in the simulator yet.
- The Upper matrix and keypad, the ADF switches at their new pins (D31 to D37), the display and lamp blanking, and the backlight and no-data rules have not been bench-tested on the combined board.
- **Remote reset** (`C,RESET:1`) is the same software reset as the Radio controller. The old Upper board's reset was never bench-tested.
- **No pause for a missing HID board:** unlike the Radio, the sketch has no pause after a failed send to the USB HID board, so with the HID board unplugged each switch or key change can stall the scan for up to about 80 ms (the W5500 retransmission timeout is 10 ms with 8 retries).
- **Retired address:** nothing answers at 172.16.1.103 any more. The bridges, the SwingTester and the Health Monitor's Upper light still know it (the light is kept green by the `UPPER_INPUT` heartbeat from this board).
