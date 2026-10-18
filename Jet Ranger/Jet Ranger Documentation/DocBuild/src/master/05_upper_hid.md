# Upper controller (172.16.1.103)

`JET_RANGER_UPPER_CONTROLLER` runs on an Arduino Mega 2560 with a W5500 Ethernet module. It scans the overhead switch matrix (up to 256 positions, 176 used) and a small numeric keypad, and sends every change over UDP to the USB HID board, which presents them to Windows as joystick buttons and keystrokes. The sketch also contains DCS-BIOS code for DCS World over the USB serial link, but `CreateDcsBiosMessage()` is still a stub, so only the joystick path works.

![Upper controller blocks](diagrams/upper_blocks.svg)

## Program flow

1. **Setup:** set the status LEDs, reset the W5500, start Ethernet and open the sockets (red LED flashes for 3 seconds), configure pins 22 to 37 as matrix row outputs and 38 to 49 as column inputs with pull-ups, clear the button and debounce arrays and start the keypad.
2. **Main loop:**
   - toggle the status LEDs every 200 ms and send the `UPPER_INPUT` heartbeat every 10 seconds;
   - `ScanInputs()` drives each of the 16 rows low in turn with direct `PORTA` and `PORTC` writes and reads the columns with direct `PIND`, `PING` and `PINL` reads, building a 176-button state array;
   - `FindInputChanges()` debounces and, for each change, calls `SendButtonToHid(index, state)`;
   - `scanMatrix()` scans the 4 by 3 keypad and sends `SendKeyToHid(key, 1)` on press and `(key, 0)` on release;
   - every 20 ms `ServiceControlPackets()` polls port 13136 and acts **only** on `C,RESET:1`; every other packet is ignored. The poll is slow on purpose so the extra W5500 access does not slow the matrix scan.

## Pin usage

| Pin | Function |
|---|---|
| 14 | Green status LED |
| 15 | Red status LED |
| 53 | W5500 reset |
| 22 to 37 | 16 matrix row drivers |
| 38 to 49 | Matrix column inputs; pins 50 to 53 are the Ethernet SPI bus, so the last four columns are fixed at 1 (unused) |
| A10, A12, A14, A15 | Keypad rows |
| A9, A11, A13 | Keypad columns |
| USB serial | DCS-BIOS link to a DCS World PC (separate from the Ethernet path) |

## What it sends

| To | Port | Content |
|---|---|---|
| 172.16.1.11 (USB HID board) | 4210 | `index,state` for matrix switches and `KEY,key,state` for keypad keys |
| 172.16.1.10 | 27000 | Debug text |
| 172.16.1.10 | 13137 | `UPPER_INPUT` heartbeat every 10 seconds |

The Upper controller has its own address and MAC (`172.16.1.103`, `...83:03`); it never listens for simulator data, so no bridge sends to it.

## Remote reset

`C,RESET:1` on port 13136 reboots the board with a software reset (see the network chapter). The code is the same as the Radio controller's and the sketch compiles, but it has **not been flashed or bench-tested** because the board was not connected.

## Known issues

- The Upper controller PCB has a Warning Mute connector (J17) on `/ROW8`, Arduino D30, but the sketch's keypad uses analog pins A9 to A15, so J17 may not be read.
- `CreateDcsBiosMessage()` has no bodies, so none of these switches do anything in DCS.
- The PCB (v1.4) also has a backlight driver on D5 (`BACKLIGHT_PWM` to an IRF540N MOSFET that switches the BACKLIGHT connector) and a brightness input on A0 (`GUAGE_BACKLIGHT_BRIGHT`, connector J16), but the sketch uses neither, and it ignores `C,B` packets.

# USB HID board (172.16.1.11)

`UDP_TO_HID` runs on an Arduino with native USB (Leonardo class, board option `arduino:avr:leonardo`) and a W5500 Ethernet module. It is a network-to-USB bridge: it receives button and key commands over UDP from the Upper controller and the Servo controller and re-emits them to Windows as **four virtual 32-button joysticks (128 buttons) and a keyboard**. The joysticks have buttons only: no axes and no hat switches.

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

1. The Upper controller numbers each matrix position as `index = row x 11 + column` (0 to 175).
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
| 128 to 175 | Not sent to Windows (out of range) |

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
