# Jet Ranger Project — Overall Summary

A home-built Bell 206 Jet Ranger cockpit: real instrument gauges, warning
lamps, radio/overhead switch panels and small displays driven from a flight
simulator (MSFS 2020/2024 via SimConnect, or FSX/P3D via FSUIPC) by Arduino
Mega boards on a private Ethernet network. This page ties the three parts of
the repository together — **Arduino sketches**, **Windows (C#) programs** and
**KiCad PCBs** — and records where the project stands. Detail lives in each
project's own `PROGRAM_SUMMARY.md` and in
`Jet Ranger Documentation/Jet Ranger Arduino and C# Program Summary.odt`
(the master document, with a table of contents and diagrams) and
`Jet Ranger Documentation/Jet Ranger Cockpit User Guide.odt`.

## How it fits together

```
 Flight sim (MSFS / P3D / FSX)
        |  SimConnect / FSUIPC
        v
 PC bridge app  (MSFSWinformsAutoCS or FSUIPCWinformsAutoCS - one at a time)
        |  UDP text packets, port 13136:  "D,CODE:value,CODE:value,..."
        v
 Arduino Mega boards (172.16.1.x) - gauges, lamps, displays
        ^  switch / radio events back to the PC:  UDP 27001
        |  joystick button changes -> USB HID board, UDP 4210
        |  heartbeats (10 s) -> JetRangerHealthMonitor, port 13137
        |  debug text      -> reflector host 172.16.1.10, port 27000
```

- **Network:** a fixed `172.16.1.0/24` LAN; the PC is `172.16.1.10`. Each
  board has a static IP and a W5500 Ethernet shield.
- **Wire format:** `D,<code>:<value>,...` data packets (for example
  `D,ALT:1200,OILP:55`) in real units; `C,<command>:<value>` control packets
  (swing tests, OLED lamp test, keepalive, reset); a calibrated code has a
  `...RAW` twin that bypasses the calibration (`OILPRAW`).
- **Safety net on the gauge boards:** if no UDP packet arrives for 30 s, every
  gauge returns to its calibrated zero (the Dual Stepper also blanks its OLEDs,
  the Servo board switches its lamps off, the Radio blanks its displays). The
  30 s starts at the end of `setup()`.
- **Power rules in the bridges:** main bus below 20 V switches the lamps and
  radar altitude off; main bus below 24 V blanks the Dual Stepper OLEDs
  (`PWR:0`); avionics bus below 24 V blanks the Radio displays (`NAVCOM1:0`),
  using the main bus if the aircraft reports the avionics bus as 0.
- **Backlight (Servo, Stepper, Dual Stepper):** pin D5 PWM drives each board's backlight MOSFET. `BPWR:1/0` from the bridges (main bus 20 V or more) switches it on and off; `C,B:0-15` sets the brightness, with the last commanded level restored whenever power returns (default PWM 65 until one is received); the 30 s no-data watchdog also switches it off.
- **Remote reset:** `C,RESET:1` reboots a board about 2 s later (AVR watchdog
  on the Servo, Stepper, Dual Stepper and HID boards; a software reset on the
  Radio and Upper boards, because a watchdog reset hung the Radio's
  bootloader). Bench-tested on every board except the Upper, which was not
  connected. SwingTester's Reset boards... button sends it.

## Arduino sketches (`Jet Ranger Arduino Sketches/`)

| Board / sketch | IP | Role |
|---|---|---|
| `JET_RANGER_RADIO_CONTROLLER` | .101 | COM/NAV frequency OLEDs, 8 rotary encoders (standard 25 kHz COM / 50 kHz NAV spacing), 4x4 button matrix (the four COM/NAV swap switches); sends tuning and swap commands to the PC; blanks its displays after 30 s with no UDP data or local input, or when the avionics bus is below 24 V |
| `JET_RANGER_SERVO_CONTROLLER` | .102 | 2 attitude-indicator servos (pitch/bank, calibrated in degrees) and 15 warning lamps; 3 debounced front-panel switch inputs (D43 TOT CLEAR, D47 WARNING MUTE, D48 FUEL VALVE) sent to the HID board as joystick buttons (HID index 8 / 7 / 9) |
| `JET_RANGER_UPPER_CONTROLLER` | .103 | overhead switch matrix (176 of 256 inputs) + keypad; feeds the `UDP_TO_HID` board |
| `UDP_TO_HID` | .11 | USB-HID bridge: UDP in, 4 virtual 32-button joysticks + keyboard out |
| `JET_RANGER_OLED_CONTROLLER` | .104 | altitude and barometer OLEDs fed with `ALT`/`BARO`; the sketch also carries DCS-BIOS code from an A-10C console |
| `JET_RANGER_STEPPER_CONTROLLER` | .105 | stepper gauges: Airspeed, VSI, Radar Alt, Torque, Gas Producer, Turbine Out Temp, Fuel Quantity (all with bench-measured tables and zero trims) |
| `JET_RANGER_OLED_DUAL_STEPPER_CONTROLLER` | .106 | stepper gauges: engine/transmission oil temp and pressure, Electrical Load, Fuel Load, Engine and Rotor RPM, Altimeter; four OLEDs (Baro, Altimeter, Clock, Temperature) behind a TCA9548A mux |

Common to the three gauge boards (Servo, Stepper, Dual Stepper):
- bench-measured calibration tables with linear interpolation (real units to
  steps / servo position), plus per-gauge zero-offset trims on the steppers;
- start-up self-test swings, selectable per gauge with `SwingXXX` flags, and
  each gauge's swing is also a callable function triggered over UDP
  (`C,SWINGOILP:1`, `C,SWINGTQ:1`, `C,SWINGPITCH:1`, ...);
- the 30 s no-data watchdog described above.

The Dual Stepper also has the OLED lamp test (`C,OLEDTEST:1/0`), the
Temperature OLED (`D,TEMP:21C`), a TCA9548A reset on pin 16 and a per-channel
I2C scan reported over UDP. Its RAM use is high (75% or more).

## Windows programs (`Jet Ranger/ C# Code/`)

| Program | Purpose |
|---|---|
| `FSUIPCWinformsAutoCS` | bridge for Prepar3D / FSX: reads FSUIPC offsets, fans real-unit payloads out to the Servo, Stepper, Dual Stepper, OLED and Radio boards, and listens on 27001 for radio/battery/alternator commands |
| `MSFSWinformsAutoCS` | the MSFS 2020/2024 equivalent built on SimConnect: same window, timers, per-board packets and port-27001 commands; aircraft profiles with estimated dials for the FlyInside B206 and the CowaSim Jet Ranger |
| `LowranceAirMapGps` | makes the PC a GPS source for a Lowrance AirMap 2000C: NMEA over a serial port (115200 8-N-1) twice a second, fed by the bridges over UDP 27002 (lat, lon, altitude, airspeed, heading, simulator UTC time and date); falls back to Brisbane Airport after 30 s with no data |
| `LowranceAirMapGpsBenchTester` | bench sender for the GPS interface: typed values, 8-way cursor, country / city / airport selection (embedded OurAirports list), Fly mode and a date / time clock |
| `JetRangerHealthMonitor` | traffic-light dashboard for the 10 s board heartbeats |
| `StepperVSITester` | bench tester sending calibrated and raw values (steppers, dual stepper, servo pitch/bank) and the Temperature OLED text |
| `SwingTester` | runs each gauge's startup swing individually or all 18 in a set order, lights the OLEDs and the 15 warning lamps during the run, resets boards, sets the backlight brightness (`C,B:0-15`, every board except the HID board) and sends a keepalive so the watchdogs do not fire |
| `JetRangerRadioControllerEmulator`, `JetRangerUpperControllerEmulator` | stand-ins for the radio and upper boards |

Python helpers (`Jet Ranger Python Code/`): a SimConnect exporter, a UDP test
receiver and a wake-on-LAN script. The UDP debug listener on port 27000 is a
small Python reflector with forwarding to a second listener.

## Documentation

`Jet Ranger Documentation/DocBuild` holds the sources and tools that generate the
master document and the User Guide: Markdown chapters (`src/master`, `src/guide`),
30 vector diagrams written as Python functions (`diagrams_a.py` to `diagrams_d.py`),
and a LibreOffice-based builder (`build_docs.py`, run through `run_lo.ps1`).

## PCBs (`Jet Ranger/PCBs/`, KiCad)

- **Controller boards:** Radio Auto Pilot (v1.3) and COM and NAV (radio), Upper
  Controller (v1.4), OLED Dual Stepper (v1.5), 13 Stepper (v1.5), Front Panel
  Servo (v1.4), ADF Controller (v1.3), X27 Small Stepper, Annunciator,
  Altimeter; switch panels: Audio, Auto Pilot and ADF panels (v1.0). On
  18 Oct 2026 the six controller boards were revised to fix the backlight
  MOSFET (source and drain were swapped; now an IRF540N driven from D5
  through 330 ohm).
- **Gauge boards:** Simkits small and large (single and dual) and a Large Guage
  board.
- **Matching to sketches is by name** (for example Front Panel Servo with the
  Servo Controller, OLED Dual Stepper with the Dual Stepper sketch). Verified
  from the files: on the Front Panel Servo board, `SW1` TOT CLEAR, `SW3`
  WARNING MUTE and `SW2` FUEL VALVE connect D43, D47 and D48 to GND — the pins
  the Servo sketch reads.
- **Warning Mute:** only two boards have such an input: Front Panel Servo
  (`SW3`, D47) and Upper Controller (`J17` on `/ROW8`, Arduino D30, through diode
  `D5` to `/COL0` in the switch matrix). See "PCB notes" in the master document.

## COM ports (last flash)

Radio .101 COM20 (replacement Mega), Servo .102 COM16 (clone Mega; the number
follows the USB socket), Stepper .105 COM4, Dual Stepper .106 COM5, USB HID .11
COM8 (Leonardo). Upper .103 and OLED .104: not recorded. Check with Device
Manager or `arduino-cli board list` before every upload.

## Where things stand

Working and bench-verified (UDP debug logs from the boards, real swing timings):
all swing commands, OLED lamp test, Temperature OLED handling, the keepalive,
the debounced Servo inputs, the radio matrix and tuning, the remote reset, and the
end-to-end SwingTester sequence. The FSUIPC bridge has run against Prepar3D v6.1
and the MSFS bridge against MSFS 2024 (FlyInside B206 and CowaSim Jet Ranger).

Known gaps / open items:
- The Servo board's three switches go to the HID board (HID index 7, 8, 9 =
  Joystick 1 buttons 8, 9, 10 in Windows) but are not forwarded to the PC itself
  (a bridge treats any unrecognised string on 27001 as a radio frequency, so it
  would need updating first).
- The Upper Controller PCB has a Warning Mute connector on D30, but the Upper
  Controller sketch scans analog pins A9–A15 only, so it may not be read. Its
  remote reset has not been bench-tested.
- The FlyInside and CowaSim estimates in the MSFS bridge come from a few dial
  readings and need checking at other power settings.
- The Radio controller emulator still sends a bare frequency for every radio, so
  only COM1 tunes from it.
- The Dual Stepper's RAM is tight (75%+, compiler low-memory warning).
- The Clock OLED (mux channel 3) answered on the I2C scan but its display has not
  been confirmed by eye; the Baro OLED is never updated after boot.
- The calibration tables, swing times and wait settings come from bench
  measurements; recheck them after any mechanical change to a gauge.
