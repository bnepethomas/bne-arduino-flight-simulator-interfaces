# Radio controller (COM and NAV)

The radio panel is another Mega at 172.16.1.101, running `JET_RANGER_RADIO_CONTROLLER.ino`. It shows the COM and NAV frequencies on two OLEDs, reads eight tuning knobs and a 4 by 4 button matrix, and sends knob and button actions back to the PC.

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
| Button matrix | rows 22 to 25, columns 26 to 29 | 4 by 4 |
| OLEDs | I2C through the TCA9548A (0x70) | Port 0 = NAV OLED, port 1 = COM OLED, both at 0x3C |
| Status LEDs | 14 green, 15 red | Red flashes while Ethernet starts |
| Ethernet module | SPI 50 to 52, reset 53 | |

## Data from the PC (UDP 13136)

| Token | Meaning | Example |
|---|---|---|
| `C1A`, `C1S` | COM1 active and standby frequency | `C1A:119.500` |
| `C2A`, `C2S` | COM2 active and standby frequency | `C2S:121.900` |
| `NAVCOM1` | Radio power (0 = off) | `NAVCOM1:1` |
| `MAINBUS` | Main bus voltage | |
| `C,B:n` | Display brightness (control packet) | `C,B:5` |

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

## Display behaviour

- If no data arrives from the PC for 30 seconds and no knob or button is used, both OLEDs blank (as if the radio had no power). Turning a knob, pressing a button or new data switches them back on.
- When the avionics bus is below 24 V the bridge tells the radio the power is off (`NAVCOM1:0`) and the COM and NAV displays go blank together; they light together again when the bus rises.
- At power-up the matrix is ignored for the first 1.5 seconds so a floating input cannot send false button presses.
- The radio sends `COMM_NAV` to the Health Monitor every 10 seconds.

## Known issues

- Only COM1 and COM2 frequencies arrive from the PC; there are no NAV frequency tokens yet, although the NAV knobs are read.
- The Radio Auto Pilot PCB has a backlight driver on D5, autopilot knobs, volume pots and display drivers that the radio sketch does not use yet, and `C,B:n` brightness commands are accepted but have no effect.
- Four encoders (COM1 major and minor, COM2 minor, NAV1 minor) have A and B the other way round from the PCB labels; it only matters if one of those knobs tunes backwards.
- If every button in the matrix's first column stops working, send `C,MATRIX:1` to port 13136: the radio prints the raw H or L state of each column to the debug listener. A column that reads L with nothing pressed is a hardware fault on that Mega pin or board (a faulty or badly seated Mega did exactly this on the bench).

# Joystick (HID) interface

Overhead and console switches reach the simulator as ordinary USB joystick buttons. The upper controller scans the switches and sends each change over UDP to a small USB board, `UDP_TO_HID`, which Windows sees as four joysticks and a keyboard. The joysticks have buttons only: no axes and no hat switches. Bind them like any other controller, in FSUIPC for Prepar3D and FSX or in the MSFS controls menu.

![From switch to joystick button](diagrams/hid_path.svg)

| Board | Sketch | Address | Role |
|---|---|---|---|
| Upper controller (Mega 2560) | `JET_RANGER_UPPER_CONTROLLER.ino` | 172.16.1.103 | Scans a 16 by 11 switch matrix (rows on pins 22 to 37, columns on 38 to 48) and a 4 by 3 keypad (A9 to A15) |
| USB HID board (Leonardo class) | `UDP_TO_HID.ino` | 172.16.1.11 | Listens on UDP 4210 and presents 4 joysticks of 32 buttons plus a keyboard |
| Servo board (Mega 2560) | `JET_RANGER_SERVO_CONTROLLER.ino` | 172.16.1.102 | Also sends three front-panel switches (warning mute, TOT clear, fuel valve) as index 7, 8 and 9 |

## How a switch becomes a joystick button

1. The upper controller numbers each matrix position as `index = row x 11 + column` (0 to 175).
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
| 128 to 175 | Not sent to Windows (out of range) |

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
- `CreateDcsBiosMessage()` is still a stub, so these switches do nothing in DCS; only the joystick path works.
- The HID board reboots when it receives `C,RESET:1` (or `RESET`) on UDP 4210. Its USB connection drops while it restarts, so Windows briefly loses the four joysticks and the keyboard; re-check any game that has them open.
- The servo board's three switches use indexes 7, 8 and 9, which Windows shows as Joystick 1 buttons 8, 9 and 10. If they were meant to be Windows buttons 7, 8 and 9, button 7 is already the Air heat switch (index 6), so the indexes would have to move.
