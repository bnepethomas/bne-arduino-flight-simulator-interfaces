# Radio controller (COM and NAV)

The radio panel is another Mega at 172.16.1.101, running `JET_RANGER_RADIO_CONTROLLER.ino`. It shows the COM and NAV frequencies on two OLEDs, reads eight tuning knobs and a 4 by 5 button matrix, serves the Auto Pilot and Audio panels (switches, knobs and 19 LEDs), and sends knob and button actions back to the PC.

![Radio panel](diagrams/radio_blocks.svg)

## Boards

| PCB | Role | Main parts |
|---|---|---|
| Jet Ranger Radio Auto Pilot | Radio controller board | Mega 2560 shield, Ethernet module, TCA9548A I2C mux |
| Jet Ranger COM and NAV | Front panel board behind the radio faceplate | 8 rotary encoders, push buttons, four frequency-swap switches |

## Inputs and outputs

| Function | Mega pins | Notes |
|---|---|---|
| COM1 major and minor knob | 31, 30 and 33, 32 | Encoder A, B |
| COM2 major and minor knob | 34, 35 and 37, 36 | Encoder A, B |
| NAV1 major and minor knob | 38, 39 and 41, 40 | Encoder A, B |
| NAV2 major and minor knob | 42, 43 and 45, 44 | Encoder A, B |
| Button matrix | rows 22 to 25, columns 26, 27, 28, 29 and 48 | 4 by 5: column 0 the COM and NAV swap switches, columns 1 and 2 the Auto Pilot panel, columns 3 and 4 the Audio panel |
| Auto Pilot knobs | CRS 11, 12; ALT A4, A5; VS 46, 47; IAS 7, 6; HDG 8, 9 | Encoder A, B |
| Panel LEDs (MAX7219) | DIN A15, CLK A14, LOAD A13 | 19 LEDs on the Auto Pilot and Audio panels and the 20 Huey Warning Panel LEDs |
| Huey Warning Panel switches | A9 TEST, A10 RESET, A8 BRT / DIM | Switches to ground; the panel is hand wired at the moment (see Huey Warning Panel) |
| Backlight | D5 | IRF540N MOSFET for the Audio, Auto Pilot and COM and NAV panel backlights |
| OLEDs | I2C through the TCA9548A (0x70) | Port 0 = NAV OLED, port 1 = COM OLED, both at 0x3C |
| Status LEDs | 14 green, 15 red | Red flashes while Ethernet starts |
| Ethernet module | SPI 50 to 52, reset 53 | |

## Data from the PC (UDP 13136)

| Token | Meaning | Example |
|---|---|---|
| `C1A`, `C1S` | COM1 active and standby frequency | `C1A:119.500` |
| `C2A`, `C2S` | COM2 active and standby frequency | `C2S:121.900` |
| `NAVCOM1` | Radio power (0 = off): the COM and NAV displays and the panel LEDs go off | `NAVCOM1:1` |
| `MAINBUS` | Main bus voltage | |
| `LED_xxx` | One of the 19 Auto Pilot and Audio panel LEDs (1 = on) | `LED_HDG:1` |
| `LED_H_xxx` | One of the 20 Huey Warning Panel LEDs (1 = on); the DCS bridge sends all 20 for the Huey, the X-Plane bridge ten approximations, the FSUIPC and MSFS bridges none | `LED_H_ENG_OIL:1` |
| `APHDG`, `APCRS`, `APIAS`, `APVS`, `APALT` | The five values on the Auto Pilot panel's OLED | `APHDG:090` |
| `BPWR` | Backlight power: 1 = main bus 20 V or more, 0 = backlight off | `BPWR:1` |
| `C,B:n` | Brightness 0 to 15: the panel backlight level (0 off) and the panel LEDs' intensity | `C,B:5` |
| `C,LEDTEST:n` | 1 lights all 39 panel LEDs, 0 returns them to the simulator's state | `C,LEDTEST:1` |

## Commands to the PC (UDP 27001)

![Where each input goes](diagrams/radio_routing.svg)

Turning a knob changes the standby frequency on the OLED and sends the new value to the PC. The four swap switches share matrix column 0:

| Switch (row, column 0) | Command sent |
|---|---|
| Row 0: NAV1 FREQ | `NAV1_RADIO_SWAP` |
| Row 1: NAV2 FREQ | `NAV2_RADIO_SWAP` |
| Row 2: COMM1 | `COM1_RADIO_SWAP` |
| Row 3: COMM2 | `COM2_RADIO_SWAP`, then `AVIONICS_MASTER_SET` |

The bridge listens on 27001 and turns these into simulator events. NAV1 and NAV2 also swap their active and standby frequencies on the NAV OLED straight away, because the PC does not send NAV frequencies back; the COM swaps change the display only when the simulator returns the swapped frequencies. Other buttons are only logged to the debug listener.

Each knob sends its standby frequency with the radio's name: COM1 as the bare number (for example `118.295`), the others as `COM2_STANDBY:118.295`, `NAV1_STANDBY:108.000` or `NAV2_STANDBY:108.000`. Nothing is sent during the first 1.5 seconds after power-up, so the knobs' start positions do not change the simulator. Each OLED line always shows two frequencies in a fixed 16-column layout.

**Tuning steps** follow the standard channel spacing. The COM knobs run 118.000 to 136.975 MHz in 25 kHz channels (the major knob picks 118 to 136, the minor knob 000, 025, 050 and 075). The NAV knobs run 108.00 to 117.95 MHz in 50 kHz steps (major 108 to 117, minor 000, 050, 100 and so on). Each minor knob wraps round at the ends. 8.33 kHz channels are not offered. FSUIPC holds COM frequencies to 10 kHz, so in that simulator a channel such as 118.025 may appear as 118.02 or 118.03.

## Auto Pilot and Audio panels

The Auto Pilot and Audio panels plug into the radio controller. Each panel has eight switches, and the Auto Pilot panel also has five knobs (CRS, ALT, VS, IAS and HDG). Every switch and knob is sent **two ways**:

1. as a **joystick button** to the USB HID board (**Joystick 3** for the Auto Pilot panel, **Joystick 4** for the Audio and ADF panels), which you can bind in FSUIPC or MSFS like any other button. A switch holds its button down while pressed; a knob click is a short press, clockwise on one button and anticlockwise on the next;
2. as a **command to the simulator bridge** on UDP 27001, which the FSUIPC and MSFS bridges turn straight into the simulator control of that name. A switch sends it when pressed; a knob sends one for each click (with a repeat count when you turn it quickly). Nothing needs binding for these.

Nothing is sent for the first 1.5 seconds after power-up. The full list, with the ADF panel's switches, is below. The joystick buttons used (Joystick 3 and 4) are not used by any other switch.

| Panel | Control | Matrix or pin | Joystick button (index) | Command to the bridge (UDP 27001) |
|---|---|---|---|---|
| Auto Pilot | AP ENGAGE | row 0, col 1 | Joystick 3, button 1 (64) | `AP_MASTER` |
| Auto Pilot | HDG | row 1, col 1 | Joystick 3, button 2 (65) | `AP_HDG_HOLD` |
| Auto Pilot | CRS | row 2, col 1 | Joystick 3, button 3 (66) | `AP_NAV1_HOLD` |
| Auto Pilot | ALT | row 3, col 1 | Joystick 3, button 4 (67) | `AP_ALT_HOLD` |
| Auto Pilot | APPR | row 0, col 2 | Joystick 3, button 5 (68) | `AP_APR_HOLD` |
| Auto Pilot | FD | row 1, col 2 | Joystick 3, button 6 (69) | `TOGGLE_FLIGHT_DIRECTOR` |
| Auto Pilot | BC | row 2, col 2 | Joystick 3, button 7 (70) | `AP_BC_HOLD` |
| Auto Pilot | IAS | row 3, col 2 | Joystick 3, button 8 (71) | `AP_PANEL_SPEED_HOLD` |
| Auto Pilot | CRS knob clockwise, anticlockwise | D11, D12 | Joystick 3, buttons 9, 10 (72, 73) | `VOR1_OBI_INC`, `VOR1_OBI_DEC` |
| Auto Pilot | ALT knob | A4, A5 | Joystick 3, buttons 11, 12 (74, 75) | `AP_ALT_VAR_INC`, `AP_ALT_VAR_DEC` |
| Auto Pilot | VS knob | D46, D47 | Joystick 3, buttons 13, 14 (76, 77) | `AP_VS_VAR_INC`, `AP_VS_VAR_DEC` |
| Auto Pilot | IAS knob | D7, D6 | Joystick 3, buttons 15, 16 (78, 79) | `AP_SPD_VAR_INC`, `AP_SPD_VAR_DEC` |
| Auto Pilot | HDG knob | D8, D9 | Joystick 3, buttons 17, 18 (80, 81) | `HEADING_BUG_INC`, `HEADING_BUG_DEC` |
| Audio | COM1 | row 0, col 3 | Joystick 4, button 1 (96) | `COM1_TRANSMIT_SELECT` |
| Audio | BOTH | row 1, col 3 | Joystick 4, button 2 (97) | `COM_RECEIVE_ALL_TOGGLE` |
| Audio | NAV2 | row 2, col 3 | Joystick 4, button 3 (98) | `RADIO_VOR2_IDENT_TOGGLE` |
| Audio | DME | row 3, col 3 | Joystick 4, button 4 (99) | `RADIO_DME1_IDENT_TOGGLE` |
| Audio | COM2 | row 0, col 4 | Joystick 4, button 5 (100) | `COM2_TRANSMIT_SELECT` |
| Audio | NAV1 | row 1, col 4 | Joystick 4, button 6 (101) | `RADIO_VOR1_IDENT_TOGGLE` |
| Audio | MKR | row 2, col 4 | Joystick 4, button 7 (102) | `MARKER_SOUND_TOGGLE` |
| Audio | ADF | row 3, col 4 | Joystick 4, button 8 (103) | `RADIO_ADF_IDENT_TOGGLE` |
| ADF | Landing gear switch | ADF controller D31 | Joystick 4, button 9 (104) | `GEAR_SET:1` when closed, `GEAR_SET:0` when open |
| ADF | Flaps switch | ADF controller D32 | Joystick 4, button 10 (105) | `FLAPS_SET:1` when closed, `FLAPS_SET:0` when open |
| ADF | Speed brakes switch | ADF controller D33 | Joystick 4, button 11 (106) | `SPOILERS_SET:1` when closed, `SPOILERS_SET:0` when open |
| ADF | Brakes button | ADF controller D34 | Joystick 4, button 12 (107) | `PARKING_BRAKES` when pressed |
| ADF | ADF major knob clockwise, anticlockwise | ADF controller D6, D7 | Joystick 4, buttons 13, 14 (108, 109) | none (the knob tunes the ADF frequency, `ADF_FREQ`) |
| ADF | ADF minor knob clockwise, anticlockwise | ADF controller D8, D9 | Joystick 4, buttons 15, 16 (110, 111) | none |
| ADF | Anti-ice switch | ADF controller D36 | Joystick 4, button 17 (112) | `ANTI_ICE_SET:1` when closed, `ANTI_ICE_SET:0` when open |
| ADF | Hyd boost switch | ADF controller D35 | Joystick 4, button 18 (113) | none (bind the joystick button in FSUIPC for the Dodo) |
| ADF | Caution dim switch | ADF controller D37 | Joystick 4, button 19 (114) | none (bind the joystick button in FSUIPC for the Dodo) |
| Huey | TEST switch | Radio controller A9 | Joystick 4, button 20 (115) | none (bind the joystick button in the simulator) |
| Huey | RESET switch | Radio controller A10 | Joystick 4, button 21 (116) | none |
| Huey | BRT / DIM switch | Radio controller A8 | Joystick 4, button 22 (117) | none |

If a knob turns the wrong way in the simulator, swap its two encoder pins in the sketch. The rows and columns come from the panel schematics; check each switch on the bench with the debug listener, which prints a line for every press (for example `AP HDG pressed (joystick button index 65)`), and `C,MATRIX:1` for the raw matrix.

## Auto Pilot and Audio panel LEDs

One MAX7219 chip on the Radio Auto Pilot PCB lights the 19 LEDs on the two panels (and, since the Huey Warning Panel was added to the sketch, 20 more: see Huey Warning Panel below). Each LED sits between one segment and one digit of the chip:

| LED | Token | MAX7219 digit | Segment | | LED | Token | MAX7219 digit | Segment |
|---|---|---|---|---|---|---|---|---|
| CRS | `LED_CRS` | 0 | A | | COM1 | `LED_COM1` | 2 | A |
| HDG | `LED_HDG` | 0 | B | | BOTH | `LED_BOTH` | 2 | B |
| IAS | `LED_IAS` | 0 | C | | NAV2 | `LED_NAV2` | 2 | C |
| ALT | `LED_ALT` | 0 | D | | DME | `LED_DME` | 2 | D |
| APPR | `LED_APPR` | 1 | A | | OUTER | `LED_OUTER` | 2 | E |
| FD | `LED_FD` | 1 | B | | INNER | `LED_INNER` | 2 | F |
| BC | `LED_BC` | 1 | C | | COM2 | `LED_COM2` | 3 | A |
| AP ENGAGE | `LED_APENG` | 1 | D | | NAV1 | `LED_NAV1` | 3 | B |
| | | | | | MKR | `LED_MKR` | 3 | C |
| | | | | | ADF | `LED_ADF` | 3 | D |
| | | | | | MIDDLE | `LED_MID` | 3 | E |

The bridges send each LED as a `LED_xxx:0/1` pair in the radio packet and the radio lights it, so the panel follows the simulator: the Auto Pilot LEDs show the autopilot and flight director modes (CRS is NAV1 lock), the COM1, COM2, BOTH, NAV1, NAV2, DME, ADF and MKR LEDs show the audio panel selections, and OUTER, MIDDLE and INNER light as you fly over a marker beacon. The LEDs go off with the radio power (avionics bus below 24 V, or no data and no use for 30 seconds). `C,LEDTEST:1` lights all 39 (including the Huey panel's) for checking the wiring and `C,LEDTEST:0` returns them to the simulator's state. The brightness command `C,B:0-15` (the SwingTester slider, or the ADF panel's pot) sets their brightness, and 0 switches them off. The **SwingTester** can light each LED on its own (see Program windows). The FSUIPC bridge reads standard FSUIPC offsets that the DodoSim aircraft may not all drive, and the MSFS bridge has not been checked against the simulator, so some LEDs may stay off until that is confirmed.

## Huey Warning Panel

The Huey Warning Panel has 20 caution LEDs and three switches. **It is wired by hand to the Radio controller at the moment** (there is no PCB for it in this guide), and the Radio sketch supports it. The LEDs are on the same MAX7219 as the other two panels, digits 4 to 7, with the tokens `LED_H_ENG_OIL`, `LED_H_ENG_ICE`, `LED_H_ENG_ICE_DETECT`, `LED_H_ENG_CHIP`, `LED_H_LEFT_FUEL_BOOST`, `LED_H_ENG_FUEL_PUMP`, `LED_H_RIGHT_FUEL_BOOST`, `LED_H_20_MINUTE_FUEL`, `LED_H_AUX_FUEL_LOW`, `LED_H_XMSN_OIL_PRESS`, `LED_H_XMSN_OIL_HOT`, `LED_H_HYD_PRESS`, `LED_H_ENG_INLET_AIR`, `LED_H_INST_INVERTER`, `LED_H_DC_GENERATOR`, `LED_H_EXT_POWER`, `LED_H_FUEL_FILTER`, `LED_H_GOV_EMER`, `LED_H_CHIP_DETECTOR` and `LED_H_IFF`. The three switches (TEST on A9, RESET on A10, BRT / DIM on A8; TEST and RESET are the two positions of one switch) are joystick buttons 20 to 22 on Joystick 4 (indexes 115 to 117). **While TEST is closed all 20 LEDs light** (only while the radio has power). The bridges do not send the `LED_H_xxx` tokens yet, so for now the LEDs light only from the SwingTester (the *Panel LEDs* group has a toggle for each), the lamp test or a test packet. Scanning eight digits instead of four makes the Auto Pilot and Audio LEDs dimmer than before. The Radio on COM20 was flashed with this code on 31 October 2026; the switches and LEDs have not been checked on the panel yet.

## Auto Pilot display

The Auto Pilot panel's OLED sits behind a mask, so each value has its own window on the two 16-character lines: **HDG** at the left hand end of the top row and **VS** at the right hand end, **CRS** at the left hand end of the bottom row, **IAS** in the centre of the bottom row and **ALT** at its right hand end. At start-up it shows heading 090, course 110, airspeed 80, vertical speed 0 and altitude 3200:

```
090            0
110    80   3200
```

The bridges then keep the five values up to date from the simulator's autopilot: the heading bug, the NAV1 course (OBS), the autopilot airspeed, vertical speed and altitude settings (heading and course as three digits, north is 360). The display goes blank with the radio power and comes back with it, like the COM and NAV displays. The **SwingTester** has an *Auto Pilot display* group to send test values.

## When the USB HID board is not connected

Every Audio and Auto Pilot switch and knob click is also sent to the USB HID board. If that board is switched off or unplugged, the radio now notices when a send fails, stops sending joystick buttons for 30 seconds, and says so in the monitor (`USB HID board 172.16.1.11 not answering`), so the panel stays responsive. When the board returns the buttons that are held down are sent again.

## Backlight

The radio controller switches the backlights of the Audio, Auto Pilot and COM and NAV panels from Mega pin D5, using the same rules as the other panels (see Network): on only while the main bus is 20 V or more (`BPWR`), at the last brightness sent (`C,B:0-15`) or a default until one arrives, and off when the radio has had no data and no knob or key use for 30 seconds (a knob or key brings it back). The SwingTester's brightness slider and *Backlight power ON / OFF* buttons test it, and the ADF panel's brightness pot sets it. The brightness also sets the intensity of the Auto Pilot and Audio panel LEDs.

## Display behaviour

- If no data arrives from the PC for 30 seconds and no knob or button is used, the COM, NAV, Auto Pilot and DME displays blank (as if the radio had no power) and the radio tells the ADF controller to blank its display and lamps too. Turning a knob, pressing a button or new data switches them back on.
- When the avionics bus is below 24 V the bridge tells the radio the power is off (`NAVCOM1:0`) and the COM, NAV, Auto Pilot and DME displays (and the ADF panel's display) go blank together; they light together again when the bus rises.
- At power-up the matrix is ignored for the first 1.5 seconds so a floating input cannot send false button presses. Each switch is checked every millisecond and reported about 12 ms after it changes, and a noisy reading cannot hold a release back.
- At start-up the radio scans the I2C bus and sends what it finds, behind each multiplexer channel, to the debug monitor (lines starting `I2C`); send `C,I2CSCAN:1` to scan again. The Audio panel's OLED is the **DME display**: it shows `10M` on the top line and `0M` on the bottom line after start-up.

- The radio sends `COMM_NAV` to the Health Monitor every 10 seconds.

## Known issues

- Only COM1 and COM2 frequencies arrive from the PC; there are no NAV frequency tokens yet, although the NAV knobs are read.
- The Radio Auto Pilot PCB has volume pots that the radio sketch does not read yet.
- Four encoders (COM1 major and minor, COM2 minor, NAV1 minor) have A and B the other way round from the PCB labels; it only matters if one of those knobs tunes backwards.
- If every button in the matrix's first column stops working, send `C,MATRIX:1` to port 13136: the radio prints the raw H or L state of each column to the debug listener. A column that reads L with nothing pressed is a hardware fault on that Mega pin or board (a faulty or badly seated Mega did exactly this on the bench).

# Joystick (HID) interface

Overhead and console switches reach the simulator as ordinary USB joystick buttons. The ADF and Upper controller scans the overhead switches (the old Upper controller's job) and sends each change over UDP to a small USB board, `UDP_TO_HID`, which Windows sees as four joysticks and a keyboard. The joysticks have buttons only: no axes and no hat switches. Bind them like any other controller, in FSUIPC for Prepar3D and FSX or in the MSFS controls menu.

![From switch to joystick button](diagrams/hid_path.svg)

| Board | Sketch | Address | Role |
|---|---|---|---|
| ADF and Upper controller (Mega 2560) | `JET_RANGER_ADF_CONTROLLER.ino` | 172.16.1.107 | Scans a 9 by 8 switch matrix (rows on pins 22 to 30, columns on 38 to 45) and a 4 by 3 keypad (A9 to A15), as well as the ADF panel. It replaces the old Upper controller (`JET_RANGER_UPPER_CONTROLLER.ino`, 172.16.1.103) |
| USB HID board (Leonardo class) | `UDP_TO_HID.ino` | 172.16.1.11 | Listens on UDP 4210 and presents 4 joysticks of 32 buttons plus a keyboard |
| Servo board (Mega 2560) | `JET_RANGER_SERVO_CONTROLLER.ino` | 172.16.1.102 | Also sends three front-panel switches (warning mute, TOT clear, fuel valve) as index 7, 8 and 9 |

## How a switch becomes a joystick button

1. The ADF and Upper controller numbers each matrix position as `index = row x 11 + column` (0 to 95). The numbering is the old Upper board's, so existing bindings still apply; the matrix is now 9 rows by 8 columns instead of 16 by 11.
2. When a switch changes it sends `index,state` to `172.16.1.11:4210`, with state 1 = switch on and 0 = off.
3. The HID board picks joystick = index div 32 and button = index mod 32, then holds that button down while the switch is on.
4. Keypad keys are sent as `KEY,<key>,<state>` and typed as keyboard keys 0 to 9, `*` and `#`.
5. The servo board sends the same format for its three switches: index 7 (warning mute, pin 47), 8 (TOT clear, pin 43) and 9 (fuel valve, pin 48), with 1 = closed.

| Index range | Windows device |
|---|---|
| 0 to 31 | Joystick 1 |
| 32 to 63 | Joystick 2 |
| 64 to 95 | Joystick 3 |
| 96 to 127 | Joystick 4 |
| 128 and above | Not sent to Windows (out of range); nothing sends these any more |

## Auto Pilot, Audio and ADF panel buttons

The radio controller (Auto Pilot, Audio and Huey panels) and the ADF controller send their switches and knob clicks in **Joystick 3** (indexes 64 to 95, Auto Pilot panel) and **Joystick 4** (96 to 127, Audio, ADF and Huey panels), buttons nothing else uses (the Upper matrix's rows 6 to 8, indexes 66 to 95, are wired but have no switches; do not fit any at 66 to 73 or 77 to 81). Windows numbers them from 1:

| Index | Windows button | Source |
|---|---|---|
| 64 to 71 | Joystick 3, buttons 1 to 8 | Auto Pilot switches: ENGAGE, HDG, CRS, ALT, APPR, FD, BC, IAS |
| 72 to 81 | Joystick 3, buttons 9 to 18 | Auto Pilot knobs (clockwise, anticlockwise): CRS, ALT, VS, IAS, HDG |
| 96 to 103 | Joystick 4, buttons 1 to 8 | Audio switches: COM1, BOTH, NAV2, DME, COM2, NAV1, MKR, ADF |
| 104 to 107 | Joystick 4, buttons 9 to 12 | ADF panel: landing gear, flaps, speed brakes, brakes |
| 108 to 111 | Joystick 4, buttons 13 to 16 | ADF knobs (clockwise, anticlockwise): major, minor |
| 112 to 114 | Joystick 4, buttons 17 to 19 | ADF panel: anti-ice, hyd boost, caution dim |
| 115 to 117 | Joystick 4, buttons 20 to 22 | Huey Warning Panel: TEST, RESET, BRT / DIM |

Every one of these is also sent to the simulator bridge as a control (see the radio controller chapter), so you only need to bind the joystick buttons if you want to use them differently.

## Switch mapping in use

From `UpperSwitchMapping.ods`. Button numbers are as Windows and the simulators show them (1 to 32); the firmware counts from 0. A latching switch holds its button down while on, so bind both press and release: in FSUIPC use the `_SET` event with parameter 1 on press and 0 on release.

| Index | Source | Switch | Joystick, button | Suggested DodoSim event |
|---|---|---|---|---|
| 0 to 4 | Upper 0, 0-4 | Heater rotary HTR 0 to 4 | J1, 1 to 5 | |
| 5 | Upper 0, 5 | Air vent | J1, 6 | `HEAT_VENT_SWITCH_SET` |
| 6 | Upper 0, 6 | Air heat | J1, 7 | `HEAT_VENT_SWITCH_SET` |
| 7 | Servo D47 | Warning mute | J1, 8 | `DS206.WARNING_MUTE_BUTTON_SET` |
| 8 | Servo D43 | TOT clear (TOT light test) | J1, 9 | `DS206.TOT_TEST_BUTTON_SET` |
| 9 | Servo D48 | Fuel valve | J1, 10 | `DS206.FUEL_VALVE_SWITCH_SET` |
| 11 | Upper 1, 0 | Forward fuel boost breaker | J1, 12 | `FORE_FUEL_PUMP_CB_SET` |
| 12 | Upper 1, 1 | Aft fuel boost breaker | J1, 13 | `AFT_FUEL_PUMP_CB_SET` |
| 13 | Upper 1, 2 | Generator field breaker | J1, 14 | `GENERATOR_FIELD_CB_SET` |
| 14 | Upper 1, 3 | Generator breaker | J1, 15 | `GENERATOR_RESET_CB_SET` |
| 15 | Upper 1, 4 | Directional gyro / attitude indicator | J1, 16 | `DIR_GYRO_SWITCH_SET` |
| 16 | Upper 1, 5 | Battery | J1, 17 | `BATTERY_SWITCH_SET` |
| 17 | Upper 1, 6 | Generator (on / start) | J1, 18 | `GENERATOR_SWITCH_SET` |
| 22 | Upper 2, 0 | Landing light | J1, 23 | `LANDING_LIGHT_SWITCH_SET` |
| 23 | Upper 2, 1 | Instrument lights | J1, 24 | `PANEL_LIGHTS_SWITCH_SET` |
| 24 | Upper 2, 2 | Caution light breaker | J1, 25 | `CAUTION_LT_CB_SET` |
| 25 | Upper 2, 3 | Anti-collision light | J1, 26 | `COLLISION_LIGHTS_SET` |
| 26 | Upper 2, 4 | Position lights | J1, 27 | `POSITION_LIGHTS_SET` |
| 27 | Upper 2, 5 | Pitot heat | J1, 28 | `PITOT_HEAT_SWITCH_SET` |
| 28 | Upper 2, 6 | Avionics | J1, 29 | `AVIONICS_SWITCH_SET` |
| 39 | Upper 3, 6 | Strobe light | J2, 8 | none yet |

The suggested events are the closest match in `DODSIM206V2.EVT`; the bindings actually saved in FSUIPC or MSFS are not in the repository. All other matrix positions are wired but unassigned.

## Setting FSUIPC events from the joystick buttons

A joystick button does nothing in the simulator until it is bound to an event in FSUIPC. The **Warning Mute** and **Fuel Valve** switches on the servo board must be set this way before they work; the **TOT clear** switch beside them is bound the same way. This applies to Prepar3D and FSX with FSUIPC and `DODSIM206V2.EVT` installed. In MSFS, bind the buttons in the controls menu instead; the DS206 events are not available there.

![Binding a switch](diagrams/joystick_binding.svg)

1. Start the simulator with the USB HID board connected. Windows should list the four joysticks (Game Controllers, `joy.cpl`); pressing a switch lights the matching button there.
2. In the simulator open FSUIPC from the Add-ons menu and choose the **Buttons + Switches** tab.
3. Operate the switch you want to bind (for example flip the Fuel Valve switch). FSUIPC shows the joystick letter and button number it saw. FSUIPC counts buttons from 0 and Windows from 1, so Windows button 10 shows as button 9 in FSUIPC.
4. Under the pressed (or closed) side, tick *Select for FS control*, choose the event from the list and type the parameter in the box beside it. The DS206 events are listed alphabetically.
5. A switch that stays on needs the released side set as well: tick *Select for FS control* there, choose the same event and enter the opposite parameter, so the cockpit follows the switch in both directions.
6. Click OK, flip the switch and check the result in the cockpit. After an event file update FSUIPC re-indexes the events alphabetically, so check these bindings again.

- **Warning Mute:** Joystick 1 button 8 (FSUIPC button 7). Event `DS206.WARNING_MUTE_BUTTON_SET`, parameter 1 when pressed and 0 when released. Alternatively bind `DS206.WARNING_MUTE_BUTTON_TRIGGER` (no parameter) to the pressed side only.
- **Fuel Valve:** Joystick 1 button 10 (FSUIPC button 9). Event `DS206.FUEL_VALVE_SWITCH_SET`, parameter 1 when closed and 0 when released. The valve in the simulator follows the switch; if it ends up the wrong way round, swap the two parameters.
- **TOT clear (TOT light test):** Joystick 1 button 9 (FSUIPC button 8). Event `DS206.TOT_TEST_BUTTON_SET`, parameter 1 when pressed and 0 when released.

If FSUIPC does not see a switch, check that Windows shows the button moving in `joy.cpl` first. If it does not, the problem is on the board side: the servo board's debug messages report each change and the HID board logs the button it sets.

## Known issues

- The HID board's own test button on pin 5 also drives Joystick 1 button 1, the same button as HTR 0.
- The old Upper sketch's DCS-BIOS stubs were not carried into the merged ADF and Upper sketch, so nothing is sent to DCS; only the joystick path works.
- The HID board reboots when it receives `C,RESET:1` (or `RESET`) on UDP 4210. Its USB connection drops while it restarts, so Windows briefly loses the four joysticks and the keyboard; re-check any game that has them open.
- The servo board's three switches use indexes 7, 8 and 9, which Windows shows as Joystick 1 buttons 8, 9 and 10. If they were meant to be Windows buttons 7, 8 and 9, button 7 is already the Air heat switch (index 6), so the indexes would have to move.
