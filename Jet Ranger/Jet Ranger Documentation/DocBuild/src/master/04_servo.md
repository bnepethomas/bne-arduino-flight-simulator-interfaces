# Servo controller (172.16.1.102)

`JET_RANGER_SERVO_CONTROLLER` runs on an Arduino Mega 2560 with a W5500 Ethernet module, on the Jet Ranger Front Panel Servo PCB. It drives the attitude indicator's two servos (pitch and bank), switches the 15 caution and warning lamps and reads three front-panel switches. The other gauges that were once servo-driven (airspeed, vertical speed, torque, temperatures, pressures, fuel and RPM) now run on the two stepper boards.

![Servo controller blocks](diagrams/servo_blocks.svg)

## Program flow

1. **Setup:** set the status LED pins; light all 15 lamps as a lamp test; reset the W5500, start Ethernet and open the sockets (red LED flashes for 3 seconds); sweep pitch and then bank from minimum to maximum and back (about 2 seconds each) and park both at their calibrated zero; switch all lamps off; send the starting state of the three switches to the HID board; log "Setup Complete" and arm the 30 second watchdog.
2. **Main loop:**
   - toggle the status LEDs every 200 ms and send the `SERVO` heartbeat every 10 seconds;
   - every 5 ms check UDP 13136 and store each value as a new **target** (servo targets, lamp states);
   - every 5 ms nudge each servo one step towards its target, so the attitude indicator moves smoothly; a servo that has not moved for 1 second is detached to stop buzz and re-attached on the next movement;
   - service the three debounced switch inputs.
3. **Watchdog:** after 30 seconds with no packet both servos are eased back to their calibrated zero and all lamps go out. The remembered lamp states are reset as well, so a stale `1` cannot keep a lamp dark when the simulator next sends `1`.

## Servos and calibration

| Instrument | Mega pin | Token | Range |
|---|---|---|---|
| Attitude pitch | 26 | `PITCH` | -30 to +30 degrees |
| Attitude bank (roll) | 27 | `BANK` | -90 to +90 degrees |

Both tokens take attitude in degrees with one decimal place allowed (for example `D,PITCH:4.5,BANK:-12`). The board converts degrees to a servo position (0 to 180) with two tables, interpolating in a straight line between points and holding the end value beyond them.

| Instrument | Attitude | Servo position |
|---|---|---|
| Pitch (`PITCH_DEG_TABLE`) | -30 degrees | 179 |
| Pitch | 0 (level) | 113 |
| Pitch | +30 degrees | 70 |
| Bank (`BANK_DEG_TABLE`) | -90 degrees | 5 |
| Bank | 0 (level) | 93 |
| Bank | +90 degrees | 179 |

`PITCHRAW` and `BANKRAW` take a raw servo position and skip the tables. The set-up sweep uses `aServMinPosition[]` and `aServMaxPosition[]` (bank 5 to 179, pitch 166 to 70) and parks at `aServZeroPosition[]` (bank 93, pitch 113). The PCB still has 16 servo headers from the earlier design; only pins 26 and 27 are used.

**Calibrating a servo:**

1. Send raw positions (`D,PITCHRAW:113`) until the instrument shows a known attitude.
2. Note the position for at least both ends and level.
3. Enter them in the table and set `aServZeroPosition[]` to the level position.
4. Upload, then check with real values (`D,PITCH:0`, `D,BANK:30`).

Do not drive a servo hard against its mechanical stop: a stalled servo draws high current and gets hot. Never power servos from the Mega's 5 V pin; use a separate 5 to 6 V supply with a common ground.

## Warning lamps

![The 15 lamps and their Mega pins](diagrams/lamp_map.svg)

| Token | Lamp | Pin |
|---|---|---|
| `EOUT` | Engine out | A1 |
| `RLOW` | Rotor RPM low | A2 |
| `TOPW` | Transmission oil pressure | A3 |
| `TOTW` | Transmission oil temperature | A4 |
| `BHOT` | Battery hot | A5 |
| `BTMP` | Battery temperature | A6 |
| `EC` | Engine chip | A7 |
| `TRC` | Tail rotor chip | A8 |
| `TC` | Transmission chip | A9 |
| `BD` | Baggage door | A10 |
| `FFLTR` | Aft fuel filter | A11 |
| `FPMP` | Fuel pump | A12 |
| `GENF` | Generator fail | A13 |
| `LOWF` | Low fuel | A14 |
| `SCF` | SC fail (standby compass) | A15 |

A lamp pin is HIGH when lit (through a driver for lamps above 20 mA). The power-up lamp test lights every lamp, so it cannot show a mis-wired pin: send one lamp at a time (for example `D,RLOW:1`) to check each one.

## Debounced switch inputs

Three front-panel switches are wired between a Mega pin and GND on the Front Panel Servo PCB and read with the internal pull-up (closed = pin LOW). A new reading must stay unchanged for 30 ms (`InputDebounceMs`) before it counts.

| Pin | Switch | PCB net | HID index | Joystick 1 button in Windows |
|---|---|---|---|---|
| D43 | TOT CLEAR (SW1) | `/SW_TOT_CLEAR` | 8 | 9 |
| D47 | WARNING MUTE (SW3) | `/SW_WARNING_MUTE` | 7 | 8 |
| D48 | FUEL VALVE (SW2) | `/SW_FUEL_VALVE` | 9 | 10 |

Every accepted change is logged to the debug stream (for example `Jet Ranger Servo: WARNING MUTE CLOSED -> HID button 7`) and sent to the USB HID board at `172.16.1.11:4210` as `index,state` with 1 = closed. The HID board counts buttons from 0 and Windows from 1. The starting state of each switch is sent at the end of `setup()`; there is no periodic resend, so a HID board that boots later stays out of step until the switch next changes. Nothing is sent to the PC itself.

## On-demand commands

- `C,SWINGPITCH:1` and `C,SWINGBANK:1` run one sweep (the value is ignored); the board pauses for about 2 seconds.
- `C,RESET:1` reboots the board through the AVR watchdog (bench-tested: "Ethernet Started" 6.9 seconds later, "Setup Complete" about 12.8 seconds after the command).

## Pin usage

| Pin | Function |
|---|---|
| 14 | Green status LED |
| 15 | Red status LED |
| 26, 27 | Pitch and bank servos |
| 43, 47, 48 | TOT clear, warning mute and fuel valve switches |
| 53 | W5500 reset |
| A1 to A15 | Warning lamps |
| 50 to 52 | SPI to the Ethernet module |

## Known issues

- Brightness packets (`C,B:n`) are accepted but do nothing: no pin is assigned. The PCB has `BACKLIGHT_PWM` on pin 5 ready for it.
- `powerAvailable` is set from `NAVCOM1` but nothing uses it, so the attitude indicator and lamps stay live when the simulator's power is off (the bridge itself sends all lamps off below 20 V).
- Several lamps log "Transmission Temperature changed" to the debug stream whichever lamp changed.
- The `#define` comments for pins 26 and 27 still describe the old servo layout.
- The PCB labels 10 of the 15 lamp outputs on different Mega pins from the sketch (A1/A2, A5/A6, A7 to A10, A11/A12). Test each lamp on its own and correct whichever side is wrong.
