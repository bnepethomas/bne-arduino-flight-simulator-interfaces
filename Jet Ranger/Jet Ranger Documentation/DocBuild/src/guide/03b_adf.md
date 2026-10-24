# ADF controller

The ADF panel is another Mega, at 172.16.1.107, running `JET_RANGER_ADF_CONTROLLER.ino` on the Jet Ranger ADF Controller PCB (revision 1.4). It tunes the ADF with two knobs, takes the ID of an aircraft to follow on the radar from a keypad, and its **brightness pot sets the backlight of the other panels**. It also watches the 12 V and 5 V supplies. The sketch compiles but has **not been flashed or bench-tested** yet.

![ADF panel](diagrams/adf_blocks.svg)

## Using the panel

| Control | What it does |
|---|---|
| Major knob | Changes the ADF frequency by 100 kHz per click |
| Minor knob | Changes it by 1 kHz per click (faster when you turn it quickly). The range is 190 to 1799 kHz |
| Keypad 0 to 9 | Types the ID of the aircraft to follow (up to 8 digits) |
| Keypad `#` | Sends the ID |
| Keypad `*` | Deletes the last digit; on an empty entry it stops following |
| Brightness pot | Sets the backlight of this panel and of the Radio, Servo, Upper, Stepper and Dual Stepper panels |

The OLED shows the frequency on the first line (`ADF  340 kHz`) and the ID on the second: `ID: 1234_` while you type, `FOLLOW 1234` once it is sent, or `FOLLOW OFF`. An entry you leave unfinished for 20 seconds is dropped.

The panel brightness is sent when you move the pot, and once 5 seconds after the board starts. The Servo, Stepper and Dual Stepper boards apply it; the Radio and Upper controllers accept it but do not drive a backlight yet.

## Inputs and outputs

| Function | Mega pins | Notes |
|---|---|---|
| ADF major knob | D6, D7 | Encoder A, B |
| ADF minor knob | D8, D9 | Encoder A, B |
| Keypad | rows D22 to D25, columns D26 to D28 | 4 by 3. Row and column order not yet checked on the real panel |
| Panel brightness pot | A0 | Wiper, 5 V to ground |
| 12 V monitor | A15 | 47 k and 10 k divider with 0.1 uF |
| 5 V monitor | AREF / AVcc | Measured against the internal 1.1 V reference; the AREF pin is left at its default |
| Backlight | D5 | IRF540N MOSFET, follows the pot |
| OLED | I2C D20, D21 (0x3C) | 2 lines of 16 characters |
| Status LEDs | D14 green, D15 red | Red flashes while Ethernet starts |
| Landing gear lamps | D2, D3, D4 | Not used yet |
| Landing gear, flaps, speed brakes and brakes switches | D29 to D32 | Switches to ground; see Panel switches |
| Ethernet module | SPI 50 to 52, reset 53 | |

## Panel switches and joystick buttons

The landing gear, flaps, speed brakes and brakes switches are read when they change (30 ms debounce, to ground when on). Each change is sent two ways, like the Auto Pilot and Audio panels:

| Switch | Joystick button (index) | Command to the bridge (UDP 27001) |
|---|---|---|
| Landing gear | Joystick 4, button 9 (104) | `GEAR_DOWN` when closed, `GEAR_UP` when open |
| Flaps | Joystick 4, button 10 (105) | `FLAPS_DOWN` when closed, `FLAPS_UP` when open |
| Speed brakes | Joystick 4, button 11 (106) | `SPOILERS_ON` when closed, `SPOILERS_OFF` when open |
| Brakes (push button) | Joystick 4, button 12 (107) | `PARKING_BRAKES` when pressed |
| ADF major knob (clockwise, anticlockwise) | Joystick 4, buttons 13, 14 (108, 109) | none: the knob tunes the ADF frequency |
| ADF minor knob (clockwise, anticlockwise) | Joystick 4, buttons 15, 16 (110, 111) | none |

For 1.5 seconds after power-up the switches are only read. When that ends the joystick buttons are set to match the switches, but **no command goes to the simulator**, so the aircraft is not moved when the panel is switched on. If a switch works the wrong way round in the simulator, swap its two commands in the sketch (`adfSwitches[]`). A knob click is a short joystick press.

## Power status

Five seconds after start-up, and then every **30 seconds**, the board sends the two rail voltages to the PC (UDP 27003) and to the debug listener (UDP 27000):

`POWER,V12:12.41,V5:5.02,STATUS:OK`

`STATUS` is `OK`, or the rails that are out of range: `5V_LOW` (below 4.75 V), `5V_HIGH` (above 5.25 V), `12V_LOW` (below 10.8 V) or `12V_HIGH` (above 15.0 V). Send `C,POWER:1` to get a reading at once. The first time you run the board, compare the readings with a meter and trim `V12_CAL` and `V5_CAL` at the top of the sketch.

## Messages to the PC (UDP 27003)

| Message | Meaning |
|---|---|
| `ADF_FREQ:830` | The ADF frequency in kHz, sent 250 ms after the knobs stop |
| `RADAR_FOLLOW_ID:4521` | The aircraft ID typed on the keypad; `RADAR_FOLLOW_ID:0` means stop following |
| `POWER,V12:12.41,V5:5.02,STATUS:OK` | Power status, every 30 seconds |

These use port 27003, not 27001, because the FSUIPC and MSFS bridges treat anything they do not recognise on 27001 as a COM1 standby frequency. (The panel switch commands above do go to 27001: the bridges recognise those names.) **Neither bridge reads port 27003 yet**, so the frequency and the follow ID do not reach the simulator. The Health Monitor does not show the board's `ADF_PANEL` heartbeat either.

## Control packets

The board only acts on control packets on UDP 13136: `C,RESET:1` reboots it, `C,MATRIX:1` prints the raw keypad readings to the debug listener, and `C,POWER:1` sends a power status. It has no 30 second no-data watchdog, so its display stays on without a PC.

## Checking the panel for the first time

1. Flash the board (see COM ports and flashing), then watch the debug listener for `Jet Ranger ADF: Setup Complete`.
2. Press each key with `C,MATRIX:1` and check which line changes. If the keys do not match, rows and columns are the other way round: change `rowPins`, `colPins` and `keyMap` in the sketch.
3. Turn each knob. If it tunes the wrong way, swap its two pins in the `RotaryEncoder` lines.
4. Move the brightness pot and check the other panels follow.
5. Check the OLED. The sketch drives the same kind of character OLED as the Radio controller; a graphics SSD1306 needs different code.
6. Compare the power status with a meter and trim `V12_CAL` and `V5_CAL`.
