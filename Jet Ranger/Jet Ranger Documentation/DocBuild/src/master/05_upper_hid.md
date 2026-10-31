# Upper panel (now on the ADF controller)

The overhead switch matrix and the small numeric keypad used to have their own board, `JET_RANGER_UPPER_CONTROLLER` (172.16.1.103, the **Jet Ranger Upper Controller** PCB). On 31 October 2026 that sketch was **merged into the ADF controller sketch** and runs on the combined **Jet Ranger Upper ADF Controller** PCB at **172.16.1.107**; the pins, scan method, backlight rules and known issues are in the ADF and Upper controller chapter. What stays the same for everything downstream:

- each matrix switch is still sent to the USB HID board as `index,state` with `index = row x 11 + column`, so the joystick button numbers (0 to 95, Joystick 1 to 3) and the table below are unchanged. The matrix is now 9 rows by 8 columns (72 positions, indexes 0 to 95) instead of 16 by 11 (176 positions);
- the keypad is still sent as `KEY,key,state`;
- the remote reset (`C,RESET:1`), `C,B` brightness, `C,KEEPALIVE:1` and `D,BPWR` are handled by the ADF controller's code, which has the same rules;
- the Health Monitor's Upper light is fed by an `UPPER_INPUT` heartbeat that the merged board sends next to its `ADF_PANEL` one.

The old sketch (`JET_RANGER_UPPER_CONTROLLER.ino`) is retired but still in the repository for reference. Its DCS-BIOS code (`CreateDcsBiosMessage()`, every case empty) was not carried over, so nothing here is sent to DCS World. The old Upper board's remote reset and backlight code were never bench-tested because the board was not connected.

# USB HID board (172.16.1.11)

`UDP_TO_HID` runs on an Arduino with native USB (Leonardo class, board option `arduino:avr:leonardo`) and a W5500 Ethernet module. It is a network-to-USB bridge: it receives button and key commands over UDP from the Servo controller, the Radio controller (Auto Pilot and Audio panels) and the ADF and Upper controller (the ADF panel switches and knobs, the Upper switch matrix and the Upper keypad) and re-emits them to Windows as **four virtual 32-button joysticks (128 buttons) and a keyboard**. The joysticks have buttons only: no axes and no hat switches.

![From switch to joystick button](diagrams/hid_path.svg)

## Program flow

1. **Setup:** start the four joysticks and the keyboard, bring up Ethernet on the static address, open the command, log and heartbeat sockets and log a boot message.
2. **Main loop:**
   - send the `JOYSTICK` heartbeat every 10 seconds;
   - read the one local push button on pin 5 and mirror it on Joystick 1, button 0;
   - read any UDP command on port 4210, log the sender and payload and call `processCommand()`.

`processCommand()` understands three formats:

| Payload | Effect |
|---|---|
| `index,state` | Sets button `index mod 32` of joystick `index div 32` (0 to 31 Joystick 1, 32 to 63 Joystick 2, 64 to 95 Joystick 3, 96 to 127 Joystick 4) pressed (1) or released (0) |
| `KEY,key,state` | Presses or releases a keyboard key. A single character is sent as its ASCII value; longer names (`ENTER`, `F5`, `LCTRL`) map to key codes |
| `C,RESET:1` (or `RESET`) | Reboots the board through the AVR watchdog; Windows briefly loses the joysticks and keyboard |

## How a switch becomes a joystick button

1. The Upper matrix (on the ADF and Upper controller) numbers each position as `index = row x 11 + column` (0 to 95: 9 rows by 8 columns).
2. When a switch changes it sends `index,state` to `172.16.1.11:4210` with state 1 = switch on.
3. The HID board holds that button down while the switch is on.
4. Keypad keys are sent as `KEY,<key>,<state>` and typed as keyboard keys 0 to 9, `*` and `#`.
5. The Servo controller sends the same format for its three front-panel switches (indexes 7, 8 and 9).

| Index range | Windows device |
|---|---|
| 0 to 31 | Joystick 1 |
| 32 to 63 | Joystick 2 |
| 64 to 95 | Joystick 3 |
| 96 to 127 | Joystick 4 |
| 128 and above | Not sent to Windows (out of range); nothing sends these any more |

## Auto Pilot, Audio and ADF panel buttons

The Radio controller (Auto Pilot and Audio panels) and the ADF controller send their switches and knob clicks in **Joystick 3** (indexes 64 to 95, Auto Pilot panel) and **Joystick 4** (96 to 127, Audio and ADF panels). Nothing else uses those buttons: the Upper matrix's assigned switches are all below index 40, and the Servo's are 7 to 9. The Upper matrix positions in rows 6 to 8 (indexes 66 to 73, 77 to 84 and 88 to 95) fall inside Joystick 3 and are wired but have no switch on them (88 and 89 are the Warning Mute and TOT Test connectors), so if switches are ever fitted at 66 to 73 or 77 to 81 they would clash with the Auto Pilot buttons. The matrix no longer reaches the Audio and ADF range (96 and above). Windows numbers buttons from 1 (button = index mod 32 + 1). A knob click is a 40 ms press, clockwise and anticlockwise on adjacent buttons.

| Index | Windows button | Source |
|---|---|---|
| 64 to 71 | Joystick 3, buttons 1 to 8 | Auto Pilot panel switches: ENGAGE, HDG, CRS, ALT, APPR, FD, BC, IAS |
| 72 to 81 | Joystick 3, buttons 9 to 18 | Auto Pilot knobs, clockwise then anticlockwise: CRS, ALT, VS, IAS, HDG |
| 96 to 103 | Joystick 4, buttons 1 to 8 | Audio panel switches: COM1, BOTH, NAV2, DME, COM2, NAV1, MKR, ADF |
| 104 to 107 | Joystick 4, buttons 9 to 12 | ADF panel: landing gear, flaps, speed brakes, brakes |
| 108 to 111 | Joystick 4, buttons 13 to 16 | ADF knobs, clockwise then anticlockwise: major, minor |
| 112 to 114 | Joystick 4, buttons 17 to 19 | ADF panel: anti-ice, hyd boost, caution dim |

Each of these is also sent to the bridge as a simulator control, so binding the joystick buttons in FSUIPC or MSFS is optional (see the Radio and ADF controller chapters for the full table).

## Switch mapping in use

From `UpperSwitchMapping.ods`. Button numbers are as Windows and the simulators show them (1 to 32); the firmware counts from 0. A latching switch holds its button down while on, so bind both press and release. The suggested events are the closest match in `DODSIM206V2.EVT`; the bindings actually saved in FSUIPC or MSFS are not in the repository. All other matrix positions are wired but unassigned.

| Index | Source | Switch | Joystick 1 button | Suggested DodoSim event |
|---|---|---|---|---|
| 0 to 4 | Upper 0, 0-4 | Heater rotary HTR 0 to 4 | 1 to 5 | |
| 5 | Upper 0, 5 | Air vent | 6 | `HEAT_VENT_SWITCH_SET` |
| 6 | Upper 0, 6 | Air heat | 7 | `HEAT_VENT_SWITCH_SET` |
| 7 | Servo D47 | Warning mute | 8 | `DS206.WARNING_MUTE_BUTTON_SET` |
| 8 | Servo D43 | TOT clear (TOT light test) | 9 | `DS206.TOT_TEST_BUTTON_SET` |
| 9 | Servo D48 | Fuel valve | 10 | `DS206.FUEL_VALVE_SWITCH_SET` |
| 11 | Upper 1, 0 | Forward fuel boost breaker | 12 | `FORE_FUEL_PUMP_CB_SET` |
| 12 | Upper 1, 1 | Aft fuel boost breaker | 13 | `AFT_FUEL_PUMP_CB_SET` |
| 13 | Upper 1, 2 | Generator field breaker | 14 | `GENERATOR_FIELD_CB_SET` |
| 14 | Upper 1, 3 | Generator breaker | 15 | `GENERATOR_RESET_CB_SET` |
| 15 | Upper 1, 4 | Directional gyro / attitude indicator | 16 | `DIR_GYRO_SWITCH_SET` |
| 16 | Upper 1, 5 | Battery | 17 | `BATTERY_SWITCH_SET` |
| 17 | Upper 1, 6 | Generator (on / start) | 18 | `GENERATOR_SWITCH_SET` |
| 22 | Upper 2, 0 | Landing light | 23 | `LANDING_LIGHT_SWITCH_SET` |
| 23 | Upper 2, 1 | Instrument lights | 24 | `PANEL_LIGHTS_SWITCH_SET` |
| 24 | Upper 2, 2 | Caution light breaker | 25 | `CAUTION_LT_CB_SET` |
| 25 | Upper 2, 3 | Anti-collision light | 26 | `COLLISION_LIGHTS_SET` |
| 26 | Upper 2, 4 | Position lights | 27 | `POSITION_LIGHTS_SET` |
| 27 | Upper 2, 5 | Pitot heat | 28 | `PITOT_HEAT_SWITCH_SET` |
| 28 | Upper 2, 6 | Avionics | 29 | `AVIONICS_SWITCH_SET` |
| 39 | Upper 3, 6 | Strobe light | Joystick 2, 8 | none yet |

## Pin usage and network

| Item | Value |
|---|---|
| Pin 5 | Local push button, Joystick 1 button 0 |
| Static IP, MAC | `172.16.1.11`, `DE:AD:BE:EF:FE:ED` |
| Listen port | 4210 (button, key and reset commands) |
| Log to | 172.16.1.10:27000 (boot, button and key events, malformed commands) |
| Heartbeat to | 172.16.1.10:13137, `JOYSTICK` every 10 seconds |

The Leonardo is flashed on COM8 with `--fqbn arduino:avr:leonardo`. Bench-tested remote reset: "Joystick controller booted" about 4 seconds after the command.

## Known issues

- The board's own test button on pin 5 also drives Joystick 1 button 1, the same button as HTR 0.
- The Servo controller's three switches use indexes 7, 8 and 9, which Windows shows as buttons 8, 9 and 10. If Windows buttons 7, 8 and 9 were wanted, button 7 is already the Air heat switch (index 6), so the indexes would have to move.
- After a reset the USB connection drops; re-check any game that has the joysticks open.
