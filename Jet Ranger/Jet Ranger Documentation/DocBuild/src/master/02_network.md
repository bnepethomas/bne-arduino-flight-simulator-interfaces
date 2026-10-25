# Network and communication

## Devices and addresses

All devices sit on one wired `172.16.1.0/24` network with fixed addresses; there is no DHCP. The firmware sends its debug and heartbeat traffic to `172.16.1.10`, so the primary PC must hold that address (or `reflectorIP` must be changed in each sketch and the sketch uploaded again).

![Who talks to whom](diagrams/network_map.svg)

| Device | IP | MAC | Heartbeat text |
|---|---|---|---|
| Primary PC (simulator, bridge, listeners) | 172.16.1.10 | | |
| USB HID board | 172.16.1.11 | DE:AD:BE:EF:FE:ED | `JOYSTICK` |
| Radio controller | 172.16.1.101 | A8:61:0A:9E:83:01 | `COMM_NAV` |
| Servo controller | 172.16.1.102 | A8:61:0A:9E:83:02 | `SERVO` |
| Upper controller | 172.16.1.103 | A8:61:0A:9E:83:03 | `UPPER_INPUT` |
| Stepper controller | 172.16.1.105 | A8:61:0A:67:83:69 | `STEPPER` |
| Dual stepper controller | 172.16.1.106 | A8:61:0A:67:83:6A | `DUAL_STEPPER` |
| ADF controller | 172.16.1.107 | A8:61:0A:67:83:6B | `ADF_PANEL` |

Every live board has its own IP and MAC address.

## UDP ports

| Port | Direction | Purpose |
|---|---|---|
| 13136 | PC to boards | Data (`D,`) and control (`C,`) packets |
| 27001 | Radio and ADF controllers to PC bridge | Standby frequencies, radio swaps, and the Auto Pilot, Audio and ADF panel controls (`AP_MASTER`, `HEADING_BUG_INC:5`, `GEAR_DOWN`, ...) |
| 27002 | Bridge (or bench tester) to the GPS interface | Aircraft position, altitude, airspeed, heading and simulator UTC time, every 500 ms (both on the PC) |
| 27003 | ADF controller to the PC | ADF frequency, radar follow ID and power status every 30 seconds (no bridge reads it yet; not 27001, where an unknown string is taken as a COM1 standby frequency) |
| 4210 | Servo, Upper, Radio and ADF controllers to USB HID board | Button changes (`index,state`), keys (`KEY,key,state`), reset |
| 13137 | Boards to Health Monitor | Heartbeat text every 10 seconds |
| 27000 | Boards to debug listener | Debug text, each line prefixed with the board name |
| 7788 | Local on the boards | Source port opened at start-up for outgoing packets |

## Packet format

![Data and control packets](diagrams/packet_format.svg)

A packet is plain ASCII text, up to 1000 bytes.

- `D,CODE:value,CODE:value,...` is a **data** packet. It can carry any number of pairs, in any order. Unknown codes are ignored, so the PC can send only the values that changed.
- `C,COMMAND:value` is a **control** packet (swing tests, lamp test, keep-alive, reset). The value is usually ignored.
- Gauge codes take **real units**: degrees, knots, feet per minute, psi, percent, degrees C, US gallons. The board converts them with its own calibration table.
- A calibrated code usually has a raw twin with `RAW` on the end (for example `IASRAW:300`). It moves the needle to a raw step or servo position and skips the table, which is how new calibration points are measured.
- Lamp codes take `0` (off) or `1` (on).

Example sent to the servo board: `D,PITCH:5,BANK:-10,LOWF:0` sets 5 degrees pitch, minus 10 degrees bank and switches the Low Fuel lamp off.

To test without the simulator, send a packet from any PC on the subnet:

```
$u = New-Object System.Net.Sockets.UdpClient
$b = [Text.Encoding]::ASCII.GetBytes('D,PITCH:5,RLOW:1')
$u.Send($b, $b.Length, '172.16.1.102', 13136)
```

## Board life cycle

![What every gauge board does from power-up to shutdown](diagrams/board_lifecycle.svg)

1. **Start-up:** status LEDs flash, the Ethernet module is reset, the board gets its static address and opens its sockets; the red LED flashes for a few seconds while the link settles.
2. **Self-test:** the gauge boards sweep each gauge (the Servo board also lights all lamps) and park at the calibrated zero.
3. **Running:** every 5 ms the board checks UDP 13136, stores each value as a new target and moves the needle towards it with smooth acceleration.
4. **Heartbeat:** every 10 seconds the board sends its heartbeat text to the Health Monitor, followed by its 5 V rail (`SERVO V5:5.02`; the ADF controller also adds its 12 V rail, `ADF_PANEL V5:5.03 V12:12.41`). The Health Monitor matches the first word and shows the voltages in its voltage table. The 5 V rail is the ADC reference (AVcc) measured against the chip's 1.1 V band-gap.
5. **No-data watchdog:** if no packet (data or control) arrives for 30 seconds, every gauge returns to its calibrated zero. The Servo board also switches its lamps off, the Dual Stepper blanks its OLEDs and the Radio blanks its displays. The timer starts at the end of `setup()` so that a long self-test cannot use it up. Normal operation resumes with the next packet.

All sketches call `Ethernet.setRetransmissionTimeout(10)` after `Ethernet.begin()`. The W5500 default of 200 ms times 8 retries can block a UDP send for 1.6 seconds if the destination does not answer ARP (for example the PC is off); with 10 ms the worst case is about 80 ms, so one dead destination cannot stall gauge movement or button scanning.

## Power rules in the bridges

![How bus voltages switch the panel](diagrams/power_rules.svg)

- **Main bus below 20 V:** the Servo board's lamps are sent as off, the Stepper's radar altitude (`AGL`) is sent as zero and `BPWR:0` switches the Radio, Servo, Stepper and Dual Stepper backlights off.
- **Main bus below 24 V:** the bridge sends `PWR:0` to the Dual Stepper, which blanks its four OLEDs; `PWR:1` turns them back on.
- **Avionics bus below 24 V:** the bridge sends `NAVCOM1:0` to the Radio, which blanks its COM and NAV displays together. If the aircraft reports the avionics bus as exactly 0 (the FlyInside B206 does) the main bus is used instead.

There is no hysteresis on these thresholds. The 24 V figure is carried over from the FSUIPC bridge and has not been verified for every MSFS aircraft.

## Backlight

![How the backlight level is decided](diagrams/backlight_logic.svg)

The Radio, Servo, Stepper and Dual Stepper boards drive their panel backlights from Mega pin D5 (PWM). On the Radio the no-data rule is the same 30 second timeout that blanks its displays, which a knob or key press also restarts. Three things decide the level:

- **`BPWR`** (from the bridges, first in the Servo and Stepper packets and after `PWR` in the Dual Stepper packet): 1 while the main bus is 20 V or more, 0 below that. At 0 the backlight is off. Until the first `BPWR` arrives the board assumes 1.
- **`C,B:<0-15>`** sets the brightness (0 off, 15 full, mapped to PWM 0 to 255; larger values are clamped). It is remembered. With no brightness command received the default level, PWM 65, is used; once one has been received every return of power restores the **last commanded level**. A command sent while the bus is down is applied when power returns.
- **The no-data watchdog:** after 30 seconds with no UDP packet, including after power-up with no PC feed, the backlight goes off along with the gauges. The next packet turns it back on at the commanded (or default) level, provided the main bus is up.

The SwingTester's brightness slider sends `C,B` to every board except the USB HID board, and its *Backlight power* buttons send `BPWR` to the Radio, Servo, Stepper and Dual Stepper. The ADF controller can send `C,B` from its brightness pot to the other boards, but the pot is switched off for the moment (see the ADF controller chapter). The Radio also uses `C,B` for the intensity of its panel LEDs. The Upper controller accepts `C,B` but does nothing with it yet.

## Remote reset

![C,RESET:1 on each board](diagrams/remote_reset.svg)

The control packet `C,RESET:1` reboots a board about two seconds later.

- **Servo, Stepper, Dual Stepper and USB HID boards** arm the AVR watchdog for two seconds and spin until it fires. A short `.init3` function clears the watchdog first thing after every reset. The HID board listens on port 4210 (it also accepts the plain text `RESET`); its USB connection drops while it restarts, so Windows briefly loses the four joysticks and the keyboard.
- **Radio, Upper and ADF controllers** use a software reset (a jump to the reset vector after switching off interrupts and peripherals). A watchdog reset hung the Radio's bootloader, which restarts the watchdog at its shortest timeout and never reaches the sketch until it is reflashed.

If a board ever reboot-loops after a reset packet, power-cycle it and reflash. The SwingTester's *Reset boards...* button sends the command to the boards you tick.

> **Network note:** data that arrives in bursts with gaps of several seconds meant the PC's link to the board switch was dropping in and out. The cause on this cockpit was the 5 V rail feeding the network switch dipping below 4.75 V; the switch then dropped its ports. Check that supply first, then the cable and the switch port.

## Debug and monitoring

![Debug and test tools](diagrams/debug_tools.svg)

Boards send debug text to `172.16.1.10:27000`. Only one program can own a UDP port, so if the reflector is running do not start a second listener on 27000; use the reflector's forwarding instead. The Health Monitor owns 13137 and each bridge owns 27001.
