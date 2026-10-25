# ADF controller

The ADF panel is another Mega, at 172.16.1.107, running `JET_RANGER_ADF_CONTROLLER.ino` on the Jet Ranger ADF Controller PCB (revision 1.5). It tunes the ADF with two knobs, takes the ID of an aircraft to follow on the radar from a keypad, sends its landing gear, flaps, speed brakes, brakes, anti-ice, hyd boost and caution dim switches to the simulator, lights the three landing gear lamps, blanks its display, lamps and backlight with the other panels, and watches the 12 V and 5 V supplies. The PCB has a brightness pot, but it is **switched off for the moment**. The sketch is flashed; the landing gear lamps and the keypad have been checked on the bench, the new switches and the blanking and backlight rules have not.

![ADF panel](diagrams/adf_blocks.svg)

## Using the panel

| Control | What it does |
|---|---|
| Major knob | Changes the ADF frequency by 100 kHz per click |
| Minor knob | Changes it by 1 kHz per click (faster when you turn it quickly). The range is 190 to 1799 kHz |
| Keypad 0 to 9 | Types the ID of the aircraft to follow (up to 8 digits) |
| Keypad `#` | Sends the ID |
| Keypad `*` | Deletes the last digit; on an empty entry it stops following |
| Landing gear, flaps and speed brakes switches | Set the simulator to match the switch position (see Panel switches) |
| Brakes button | Toggles the parking brake |
| Anti-ice, hyd boost and caution dim switches | Joystick 4 buttons 17 to 19; anti-ice also sets the simulator's anti-ice (see Panel switches) |
| Brightness pot | Not used for the moment: the panel backlight stays at its default and no brightness is sent to the other panels |

The OLED shows the frequency on the first line (`ADF  340 kHz`) and the ID on the second: `ID: 1234_` while you type, `FOLLOW 1234` once it is sent, or `FOLLOW OFF`. An entry you leave unfinished for 20 seconds is dropped.

**Brightness pot.** The pot on the PCB (A0) can set the backlight of this panel and of the Radio, Servo, Upper, Stepper and Dual Stepper panels, but `USE_BRIGHTNESS_POT` in the sketch is 0, so it does nothing: this panel's backlight stays at its default (level 4) until a `C,B` command sets it, and the other panels follow the SwingTester slider or their own defaults. Set it to 1 and reflash to bring the pot back.

## Inputs and outputs

| Function | Mega pins | Notes |
|---|---|---|
| ADF major knob | D6, D7 | Encoder A, B |
| ADF minor knob | D8, D9 | Encoder A, B |
| Keypad | scan rows D22 to D25, scan columns D26 to D28 | 4 by 3; see the key table below |
| Panel brightness pot | A0 | Wiper, 5 V to ground; not read while the pot is switched off |
| 12 V monitor | A15 | 47 k and 10 k divider with 0.1 uF |
| 5 V monitor | AREF / AVcc | Measured against the internal 1.1 V reference; the AREF pin is left at its default |
| Backlight | D5 | IRF540N MOSFET: level 4 by default, then `C,B`; off with `BPWR:0`, the no-data timeout or no data (see Panel blanking and backlight) |
| OLED | I2C D20, D21 (0x3C) | 2 lines of 16 characters |
| Status LEDs | D14 green, D15 red | Red flashes while Ethernet starts |
| Landing gear lamps | D2, D3, D4 | Nose, left, right: lit by data packets (see below) |
| Landing gear, flaps, speed brakes and brakes switches | D29 to D32 | Switches to ground; see Panel switches |
| Hyd boost, anti-ice and caution dim switches | D33, D34, D35 | Switches to ground (connectors J17, J18, J19, added in revision 1.5); see Panel switches |
| Ethernet module | SPI 50 to 52, reset 53 | |

**Keypad keys.** The keypad's seven lines come out of pins 18 to 24 of the ADF panel connector (J5) to D22 to D28. The key labels in the sketch were set by pressing every key and moving each label to where it was reported:

| Scan row | Column 0 (D26) | Column 1 (D27) | Column 2 (D28) |
|---|---|---|---|
| 0 (D22) | 6 | 4 | 5 |
| 1 (D23) | 9 | 7 | 8 |
| 2 (D24) | # | * | 0 |
| 3 (D25) | 3 | 1 | 2 |

If the keys ever need checking again, send `C,MATRIX:1` while holding a key: the debug listener shows which row and column changes.

## Panel switches and joystick buttons

The seven switches are read when they change (30 ms debounce, to ground when on). Each change is sent two ways, like the Auto Pilot and Audio panels:

| Switch | Joystick button (index) | What goes to the bridge (UDP 27001) |
|---|---|---|
| Landing gear (toggle) | Joystick 4, button 9 (104) | The position: `GEAR_SET:1` when closed (gear down), `GEAR_SET:0` when open |
| Flaps (toggle) | Joystick 4, button 10 (105) | `FLAPS_SET:1` (flaps fully down) or `FLAPS_SET:0` |
| Speed brakes (toggle) | Joystick 4, button 11 (106) | `SPOILERS_SET:1` (fully out) or `SPOILERS_SET:0` |
| Brakes (push button) | Joystick 4, button 12 (107) | `PARKING_BRAKES` when pressed |
| Anti-ice (toggle) | Joystick 4, button 17 (112) | `ANTI_ICE_SET:1` or `ANTI_ICE_SET:0` |
| Hyd boost (toggle) | Joystick 4, button 18 (113) | none |
| Caution dim (toggle) | Joystick 4, button 19 (114) | none |
| ADF major knob (clockwise, anticlockwise) | Joystick 4, buttons 13, 14 (108, 109) | none: the knob tunes the ADF frequency |
| ADF minor knob (clockwise, anticlockwise) | Joystick 4, buttons 15, 16 (110, 111) | none |

The gear, flaps, speed brakes and anti-ice toggle switches send their **position as a value** (not an event the simulator could toggle), every time they change and again **every 5 seconds**, so the simulator always follows the switches. That includes the position they were in at power-up: about 5 seconds after the panel is switched on, the aircraft's gear, flaps and speed brakes move to match the switches. For 1.5 seconds after power-up the switches are only read. If a switch works the wrong way round in the simulator, swap its 1 and 0 in the sketch (`adfSwitches[]`). A knob click is a short joystick press.

**Anti-ice, hyd boost and caution dim on the Dodo.** The Dodo Jet Ranger has its own events for these switches: `ANTI_ICE_SWITCH_SET`, `HYDRAULICS_SWITCH_SET` (hyd boost) and `CAUTION_LIGHT_SWITCH_SET` (caution lights bright or dim), each taking 1 or 0. In FSUIPC's Buttons + Switches page, bind Joystick 4 buttons 17, 18 and 19 to them, with 1 on press and 0 on release (and turn on the Dodo's "Sync controls on load" option). The simulator's own controls for these are toggles, so the bridges have no reliable command for hyd boost and caution dim, and the Dodo may ignore the bridge's `ANTI_ICE_SET`: use the FSUIPC binding **or** the bridge command for anti-ice, not both.

## Landing gear lamps

The three lamps on the ADF panel (nose, left and right) are lit by a data packet to the ADF controller on UDP 13136: `D,LDGN:1,LDGL:1,LDGR:0` (1 = lit, 0 = off). The **SwingTester** has a toggle for each lamp and *All ON* and *All OFF* buttons (see Program windows). The simulator bridges do not send them yet, so for now they are a bench test. They go out with the rest of the panel and come back as last set.

## Power status

Five seconds after start-up, and then every **30 seconds**, the board sends the two rail voltages to the PC (UDP 27003) and to the debug listener (UDP 27000):

`POWER,V12:12.41,V5:5.02,STATUS:OK`

`STATUS` is `OK`, or the rails that are out of range: `5V_LOW` (below 4.75 V), `5V_HIGH` (above 5.25 V), `12V_LOW` (below 10.8 V) or `12V_HIGH` (above 15.0 V). Send `C,POWER:1` to get a reading at once. The first time you run the board, compare the readings with a meter and trim `V12_CAL` and `V5_CAL` at the top of the sketch. The same two readings are in the heartbeat the board sends every 10 seconds (`ADF_PANEL V5:5.03 V12:12.41`), and the **Health Monitor** shows them in its voltage table: the ADF is the only board with a 12 V reading.

## Messages to the PC (UDP 27003)

| Message | Meaning |
|---|---|
| `ADF_FREQ:830` | The ADF frequency in kHz, sent 250 ms after the knobs stop |
| `RADAR_FOLLOW_ID:4521` | The aircraft ID typed on the keypad; `RADAR_FOLLOW_ID:0` means stop following |
| `POWER,V12:12.41,V5:5.02,STATUS:OK` | Power status, every 30 seconds |

These use port 27003, not 27001, because the FSUIPC and MSFS bridges treat anything they do not recognise on 27001 as a COM1 standby frequency. (The panel switch commands above do go to 27001: the bridges recognise those names.) **Neither bridge reads port 27003 yet**, so the frequency and the follow ID do not reach the simulator.

## Control packets

The board acts on control packets on UDP 13136: `C,RESET:1` reboots it, `C,MATRIX:1` prints the raw keypad readings to the debug listener, `C,POWER:1` sends a power status, `C,B:0` to `C,B:15` sets the backlight brightness (while the pot is switched off), `C,KEEPALIVE:1` counts as data and `C,DISPLAY:0` / `C,DISPLAY:1` (sent by the Radio controller) blank and restore the panel. Of the data packets it uses the three landing gear lamp tokens and `BPWR`.

## Panel blanking and backlight

The panel follows the other boards' rules:

- **With the Radio.** When the Radio controller blanks its displays (no avionics power, or no data or input for 30 seconds) it tells the ADF controller, which clears its OLED and puts the landing gear lamps out. They come back when the Radio's displays do.
- **Main bus.** The bridges send `BPWR` to the ADF controller like the other boards; `BPWR:0` switches the backlight off and `BPWR:1` brings it back.
- **Brightness.** `C,B:0` to `C,B:15` sets the backlight (the SwingTester slider sends it); the default is level 4.
- **No data.** 30 seconds with no data packet from the bridge and no knob, key or switch use blanks the OLED, the lamps and the backlight until something arrives, so the panel goes dark if the PC is not feeding it.

## Checking the panel for the first time

1. Flash the board (see COM ports and flashing), then watch the debug listener for `Jet Ranger ADF: Setup Complete`.
2. Press each key and check the OLED shows the key you pressed (`C,MATRIX:1` shows the raw readings if not).
3. Turn each knob. If it tunes the wrong way, swap its two pins in the `RotaryEncoder` lines.
4. Light the three landing gear lamps from the SwingTester.
5. Check the OLED. The sketch drives the same kind of character OLED as the Radio controller; a graphics SSD1306 needs different code.
6. Compare the power status with a meter and trim `V12_CAL` and `V5_CAL`.
