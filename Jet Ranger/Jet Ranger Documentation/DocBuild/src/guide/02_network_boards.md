# Network

All devices sit on one wired `172.16.1.x` subnet with fixed addresses; there is no DHCP on the boards.

![Who talks to whom](diagrams/network_map.svg)

| Device | IP address | MAC | Keep-alive text |
|---|---|---|---|
| Primary PC (simulator, bridge, debug listener, Health Monitor) | 172.16.1.10 | | |
| USB HID board | 172.16.1.11 | DE:AD:BE:EF:FE:ED | `JOYSTICK` |
| Radio controller | 172.16.1.101 | A8:61:0A:9E:83:01 | `COMM_NAV` |
| Servo board | 172.16.1.102 | A8:61:0A:9E:83:02 | `SERVO` |
| Upper controller | 172.16.1.103 | A8:61:0A:9E:83:03 | `UPPER_INPUT` |
| OLED controller | 172.16.1.104 | A8:61:0A:67:83:68 | none |
| Stepper board | 172.16.1.105 | A8:61:0A:67:83:69 | `STEPPER` |
| Dual stepper board | 172.16.1.106 | A8:61:0A:67:83:6A | `DUAL_STEPPER` |

Every live board has its own IP and MAC address. The firmware sends its debug and keep-alive traffic to `172.16.1.10` (`reflectorIP`), so the primary PC must hold that address, or `reflectorIP` must be changed and the sketch uploaded again.

| UDP port | Direction | Purpose |
|---|---|---|
| 13136 | PC to boards | Gauge, lamp and radio data (`D,`) and control (`C,`) packets |
| 27001 | Radio controller to PC bridge | Standby frequencies and commands such as `COM1_RADIO_SWAP` |
| 4210 | Servo and Upper controllers to USB HID board | Switch changes (`index,state`) and keypad keys (`KEY,key,state`) |
| 13137 | Boards to Health Monitor | Keep-alive text every 10 seconds |
| 27000 | Boards to debug listener | Debug messages, each prefixed with the board name |

## Packet format

![Data and control packets](diagrams/packet_format.svg)

A packet can carry any number of pairs, in any order, up to 1000 bytes. Unknown tokens are ignored, so the PC can send only the values that changed. For example `D,PITCH:5,BANK:-10,LOWF:0` sent to the servo board sets 5 degrees pitch, minus 10 degrees bank and turns off Low Fuel.

To test without the simulator, send a packet from a PC on the subnet, for example with PowerShell:

```
$u = New-Object System.Net.Sockets.UdpClient
$b = [Text.Encoding]::ASCII.GetBytes('D,PITCH:5,RLOW:1')
$u.Send($b, $b.Length, '172.16.1.102', 13136)
```

## What a board does

![Board life cycle](diagrams/board_lifecycle.svg)

After start-up and its self-test a gauge board checks UDP 13136 every 5 ms and moves each needle smoothly towards its target. It sends its keep-alive every 10 seconds. If no packet arrives for **30 seconds** every needle returns to zero (the Servo board also switches its lamps off, the Dual Stepper blanks its OLEDs and the Radio blanks its displays); the next packet brings everything back.

![Remote reset](diagrams/remote_reset.svg)

The control packet `C,RESET:1` reboots a board about 2 seconds later. The SwingTester's *Reset boards...* button sends it to the boards you tick.

## When the panel loses power

![Bus voltage rules](diagrams/power_rules.svg)

The bridge turns parts of the panel off when the simulated electrics drop: lamps and radar altitude below 20 V on the main bus, the Dual Stepper's four OLEDs below 24 V on the main bus, and the Radio's COM and NAV displays below 24 V on the avionics bus (the main bus is used if the aircraft reports the avionics bus as 0).

# Arduino servo controller

The servo board is an Arduino Mega 2560 with an Ethernet module, on the Jet Ranger Front Panel Servo PCB at 172.16.1.102. It drives the attitude indicator's two servos, switches the 15 caution and warning lamps and reads three front-panel switches. It runs `JET_RANGER_SERVO_CONTROLLER.ino`. The other gauges that were once servo-driven now run on the two stepper boards.

![Servo board](diagrams/servo_blocks.svg)

## Uploading the firmware

1. Open the sketch in the Arduino IDE and select *Board: Arduino Mega or Mega 2560* (or use `arduino-cli`, see Flashing a board).
2. Install the libraries it uses: Ethernet (with SPI), Servo, Ewma (jonnieZG) and DCS-BIOS.
3. Upload, then watch the start-up self-test: both servos sweep and all 15 lamps light.

## Servos

| Instrument | Mega pin | UDP token | Range |
|---|---|---|---|
| Attitude bank (roll) | 27 | `BANK` | -90 to +90 degrees |
| Attitude pitch | 26 | `PITCH` | -30 to +30 degrees |

Both tokens take attitude in degrees, with one decimal place allowed (`D,PITCH:4.5,BANK:-12`). The board turns degrees into a servo position with the tables in Calibration. `BANKRAW` and `PITCHRAW` take a raw servo position (0 to 180) and skip the tables. The PCB still has 16 servo headers from the earlier design; only pins 26 and 27 are used.

## Other pins

| Function | Pin | Behaviour |
|---|---|---|
| Green status LED | 14 | Flashes alternately with red every 200 ms while running |
| Red status LED | 15 | Fast flash during the 3 second Ethernet start-up, then alternates with green |
| Ethernet module reset | 53 | Pulsed low for 2 ms at power-up |
| Lamps | A1 to A15 | Digital outputs, HIGH = lit (see Reference) |
| TOT clear switch | 43 | Switch to GND, debounced 30 ms; each change goes to the USB HID board as Joystick 1 button 9 (index 8) |
| Warning mute switch | 47 | The same, Joystick 1 button 8 (index 7) |
| Fuel valve switch | 48 | The same, Joystick 1 button 10 (index 9) |

![The 15 lamps and their pins](diagrams/lamp_map.svg)

## What happens at power-up

1. All 15 lamps switch on as a lamp test.
2. The Ethernet module is reset and starts at 172.16.1.102; the red LED flashes for 3 seconds.
3. *Ethernet Started* goes to the debug listener.
4. Pitch, then bank, sweeps from minimum to maximum and back (about 2 seconds each) and parks at its calibrated level position.
5. All lamps switch off.
6. The starting state of the three switches is sent to the USB HID board, *Setup Complete* is logged and the 30 second watchdog starts.

## Normal running

- Every 5 ms the board checks UDP 13136 and stores each value as a new target; each servo steps one position towards its target, so the attitude indicator moves smoothly.
- A servo that has not moved for 1 second is detached to stop buzz; the next movement re-attaches it.
- Lamps change as soon as a 0 or 1 arrives.
- `C,SWINGPITCH:1` or `C,SWINGBANK:1` re-runs one sweep on demand (the board pauses for about 2 seconds).

# Stepper controller (13 Stepper board)

A second Arduino Mega 2560, on the Jet Ranger 13 Stepper PCB at 172.16.1.105, drives X27-style stepper gauges directly from its pins. Unlike the servo board it takes real units and converts them to needle steps with calibration tables in `JET_RANGER_STEPPER_CONTROLLER.ino`.

![Stepper board](diagrams/stepper_blocks.svg)

| Gauge | Token | Mega pins | Units | Calibration |
|---|---|---|---|---|
| Airspeed | `IAS` | 12, 13, 22, 23 | knots 0 to 140 | 10-point table |
| Vertical speed | `VSI` | 7, 8, 9, 11 | ft/min, plus and minus 1750 | 11-point table |
| Radar altitude | `AGL` | 32 to 35 | feet 0 to 2500 | 11-point table |
| Torque | `TQ` | 36 to 39 | percent 0 to 120 | 2-point table |
| Gas producer | `N1` | 40 to 43 | percent 0 to 103 | 3-point table |
| Turbine out temperature | `ITT` | A11 to A14 | degrees C 100 to 900 | 7-point table |
| Fuel quantity | `FUEL` | 2, 3, 4, 6 | US gallons 0 to 75 | 2-point table |

Every token has a raw twin with `RAW` on the end (for example `IASRAW:300`). It moves the needle by raw steps and skips the table, which is how new calibration points are measured on the bench. The board also has four oil and transmission steppers with placeholder scales; those gauges are driven from the Dual Stepper, so wire each physical gauge to one board only. Pins 24 to 31 are free.

## Behaviour

- **Power-up:** status LEDs flash, Ethernet starts, backlighting ramps up, then each gauge does a self-test sweep in turn: fuel, torque, turbine out, gas producer, VSI, airspeed, radar altitude. Each sweep has its own on/off switch (`SwingFA`, `SwingTQ` and so on) near the top of the sketch.
- **Running:** every 5 ms the board checks UDP 13136 and moves each needle towards its target with smooth acceleration.
- **No-data watchdog:** after 30 seconds without a packet every needle returns to its zero.
- **Zero trims:** each gauge has a `...ZeroOffset` value to nudge its true zero without touching the table.
- **Keep-alive:** `STEPPER` to the Health Monitor every 10 seconds; debug text goes to UDP 27000.

## Testing a gauge on demand

Send a control packet to re-run one gauge's sweep without rebooting, for example `C,SWINGTQ:1`.

| Command | Gauge |
|---|---|
| `SWINGFUEL` | Fuel quantity |
| `SWINGTQ` | Torque |
| `SWINGITT` | Turbine out temperature |
| `SWINGN1` | Gas producer |
| `SWINGVSI` | Vertical speed |
| `SWINGIAS` | Airspeed |
| `SWINGAGL` | Radar altitude |

The **StepperVSITester** app can send every token and raw token to this board without the simulator. The **SwingTester** app sends these commands, and the Dual Stepper and servo sweeps, one at a time or all in a fixed order:

![Swing ALL sequence](diagrams/swing_sequence.svg)

It lights the OLEDs and the lamps during the run and sends a keep-alive so the boards' 30 second watchdogs do not fire. *Reset boards...* reboots the boards you tick (Radio, Servo, Upper, Stepper, Dual Stepper, USB HID) by sending each one `C,RESET:1`; each restarts about 2 seconds later and repeats its power-up test.

## Known issues

- The backlight PWM was once on pin 8, which the PCB routes to VSI coil B. It is now pin 5, matching the PCB's `BACKLIGHT_PWM` net, in both stepper sketches.
- Pin 32 is also claimed by an unused roll stepper in the code.
- The oil, transmission and placeholder gauges use a shared linear scale, not bench-measured tables.

# OLED dual stepper controller

A third Mega, on the Jet Ranger OLED Dual Stepper PCB at 172.16.1.106, drives stepper gauges plus four small OLED displays. It runs `JET_RANGER_OLED_DUAL_STEPPER_CONTROLLER.ino` and, like the 13 Stepper board, takes real units.

![Dual stepper board](diagrams/dual_stepper_blocks.svg)

| Gauge | Token | Mega pins | Units | Calibration |
|---|---|---|---|---|
| Engine speed (N2) | `RPME` | 24 to 27 | percent, one decimal | 4-point table |
| Rotor speed | `RPMR` | 28 to 31 | percent, one decimal | 4-point table |
| Altimeter | `ALT` | 40 to 43, zero sensor A15 | feet | 720 steps per 1000 ft |
| Fuel load | `FUELLOAD` | 32 to 35 | psi | 2-point table (0 to 30 psi) |
| Electrical load | `ELECTRICALLOAD` | 36 to 39 | percent | 2-point table |
| Engine oil temperature | `OILT` | 48, A0 to A2 | degrees C | 4-point table |
| Engine oil pressure | `OILP` | 44 to 47 | psi | 4-point table |
| Transmission oil temperature | `XMSNT` | A3 to A6 | degrees C | 4-point table |
| Transmission oil pressure | `XMSNP` | A7 to A10 | psi | 5-point table |

Turbine out temperature (`ITT`) and fuel quantity (`FUEL`) are also accepted, as on the 13 Stepper board. Wire each physical gauge to one board only; the matching channel on the other board then simply has nothing connected. Each token also has a `RAW` twin for bench work. `IAS` is not handled here; the FSUIPC and MSFS bridges send airspeed to the 13 Stepper board with one decimal place so the needle moves smoothly.

## OLED displays

The four 128 by 32 SSD1306 OLEDs share the Mega's I2C pins (20 SDA, 21 SCL) through a TCA9548A multiplexer at address 0x70. The firmware pulses the mux reset on pin 16 at start-up.

| Display | Mux channel | UDP token | Shows |
|---|---|---|---|
| Barometric setting | 1 | none yet | `2992` at boot; not updated |
| Altimeter digits | 2 | `ALT` | Rolling-drum altitude, raw MSL (barometric correction off) |
| Clock | 3 | `ZULU` | Zulu time from HHMM, for example `ZULU:1430` |
| Temperature | 4 | `TEMP` | Text as sent, for example `TEMP:25C` |

Send `C,OLEDTEST:1` to light all four displays solid and `C,OLEDTEST:0` to restore them. When the main bus is below 24 V the bridge sends `PWR:0` and all four displays go blank; `PWR:1` turns them back on and they repaint within about a second. The altimeter needle keeps following altitude. The lamp test still lights all four displays whatever `PWR` says.

## Behaviour

- **Power-up:** OLEDs start first, then each gauge sweeps in turn: engine oil pressure, engine oil temperature, transmission pressure, transmission temperature, electrical load, fuel load, engine RPM, rotor RPM, altimeter. The altimeter seeks its zero sensor before sweeping. Start-up takes about 65 seconds.
- **No-data watchdog:** after 30 seconds without a packet the needles return to zero and all OLEDs blank.
- **On-demand sweeps:** `C,SWINGOILP:1`, `SWINGOILT`, `SWINGXMSNP`, `SWINGXMSNT`, `SWINGELECTRICALLOAD`, `SWINGFUELLOAD`, `SWINGRPME`, `SWINGRPMR`, `SWINGALT`. Each blocks the board for 5 to 7 seconds.
- **Keep-alive:** `DUAL_STEPPER` to the Health Monitor every 10 seconds, so it shows separately from the 13 Stepper board.

## Known issues

- RAM use is 75 percent or more; avoid adding more OLEDs or string-heavy code.
- The latest bench I2C scan found OLEDs on mux channels 1 to 4; check by eye that each shows its text.
- Engine and rotor RPM tables assume 0 percent is 0 steps; `RSoffset` is not yet measured.
- The altimeter zero seek travels up to 10,080 steps (about 14 turns) before giving up.
