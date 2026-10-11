# Jet Ranger Project — Overall Summary

A home-built Bell 206 Jet Ranger cockpit: real instrument gauges, warning
lamps, radio/overhead switch panels and small displays driven from a flight
simulator (MSFS 2020/2024 via SimConnect, or FSX/P3D via FSUIPC/SimConnect)
by Arduino Mega boards on a private Ethernet network. This page ties the three
parts of the repository together — **Arduino sketches**, **Windows (C#)
programs** and **KiCad PCBs** — and records where the project stands. Detail
lives in each project's own `PROGRAM_SUMMARY.md` and in
`Jet Ranger Documentation/Jet Ranger Arduino and C# Program Summary.odt`
(the master document, with a table of contents).

## How it fits together

```
 Flight sim (MSFS / P3D / FSX)
        |  SimConnect / FSUIPC
        v
 PC bridge app  (FSUIPCWinformsAutoCS, SimConnect_to_UDP, P3D_to_UDP)
        |  UDP text packets, port 13136:  "D,CODE:value,CODE:value,..."
        v
 Arduino Mega boards (172.16.1.x) - gauges, lamps, displays
        ^  switch / radio events back to the PC:  UDP 27001
        |  heartbeats (10 s) -> JetRangerHealthMonitor, port 13137
        |  debug text      -> reflector host 172.16.1.10, port 27000
```

- **Network:** a fixed `172.16.1.0/24` LAN; the PC is `172.16.1.10`. Each
  board has a static IP and a W5500 Ethernet shield.
- **Wire format:** `D,<code>:<value>,...` data packets (for example
  `D,ALT:1200,OILP:55`); `C,<command>:<value>` control packets (swing tests,
  OLED lamp test, keepalive); a calibrated real-unit code (`OILP`) usually has a
  `...RAW` twin that bypasses the calibration (`OILPRAW`).
- **Safety net on the gauge boards:** if no UDP packet arrives for 30 s, every
  gauge returns to its calibrated zero (the Dual Stepper also blanks its OLEDs,
  the Servo board switches its lamps off). The 30 s now starts at the end of
  `setup()`.

## Arduino sketches (`Jet Ranger Arduino Sketches/`)

| Board / sketch | IP | Role |
|---|---|---|
| `JET_RANGER_RADIO_CONTROLLER` | .101 | COM/NAV frequency OLEDs, 8 rotary encoders, 4x4 button matrix; talks radio/battery commands to the PC |
| `JET_RANGER_SERVO_CONTROLLER` | .102 | 2 attitude-indicator servos (pitch/bank, calibrated in degrees) and 15 warning lamps; 3 debounced front-panel switch inputs (D43 TOT CLEAR, D47 WARNING MUTE, D48 FUEL VALVE) reported as debug messages |
| `JET_RANGER_UPPER_CONTROLLER` | .103 | overhead switch matrix (176 of 256 inputs) + keypad; feeds DCS-BIOS and the `UDP_TO_HID` board |
| `UDP_TO_HID` | .11 | USB-HID bridge: UDP in, 4 virtual joysticks + keyboard out |
| `JET_RANGER_OLED_CONTROLLER` | .104 | really an A-10C front-console OLED sketch filed here (DCS-BIOS, not the Jet Ranger protocol) |
| `JET_RANGER_STEPPER_CONTROLLER` | .105 | stepper gauges: Fuel Quantity, Torque, EGT/ITT, Gas Producer, VSI, Airspeed, Radar Alt (plus EOT/EOP/XOT/XOP without boot swings) |
| `JET_RANGER_OLED_DUAL_STEPPER_CONTROLLER` | .106 | stepper gauges: engine/transmission oil temp and pressure, Electrical Load, Fuel Load, Turbine/Rotor RPM, Altimeter; four OLEDs (Baro, Altimeter, Clock, Temperature) behind a TCA9548A mux |
| `Stepper-Tuning-Harness`, `Servo-Knob-For-Calibration` | — | bench tools for finding each gauge's step / servo positions |
| `DataReceiver`, `test`, `retired` | — | prototypes and archived code |

Common to the three gauge boards (Servo, Stepper, Dual Stepper):
- bench-measured calibration tables with linear interpolation (real units to
  steps / servo position), plus per-gauge zero-offset trims on the steppers;
- startup self-test swings, selectable per gauge with `SwingXXX` flags, and
  each gauge's swing is also a callable function triggered over UDP
  (`C,SWINGOILP:1`, `C,SWINGTQ:1`, `C,SWINGPITCH:1`, ...);
- the 30 s no-data watchdog described above.

The Dual Stepper also has the OLED lamp test (`C,OLEDTEST:1/0`), the
Temperature OLED (`D,TEMP:21C`), a TCA9548A reset on pin 16 and an optional
per-channel I2C scan reported over UDP. Its RAM use is high (75%).

## Windows programs (`Jet Ranger/ C# Code/`)

| Program | Purpose |
|---|---|
| `FSUIPCWinformsAutoCS` | main FSUIPC bridge: reads sim data, fans the UDP payloads out to the Servo, Stepper, Dual Stepper, OLED and Radio boards, and listens on 27001 for radio/battery/alternator commands |
| `SimConnect_to_UDP`, `P3D_to_UDP`, `MSFSSimConnectExtractor` | the SimConnect equivalents (MSFS 2020/2024, Prepar3D, and the oldest MSFS variant) |
| `JetRangerHealthMonitor` | traffic-light dashboard for the 10 s board heartbeats |
| `StepperVSITester` | bench tester sending calibrated and raw values (steppers, dual stepper, servo pitch/bank) and the Temperature OLED text |
| `SwingTester` | runs each gauge's startup swing individually or all 18 in a set order, lights the OLEDs and the 15 warning lamps during the run (lamps off in a set order at the end), with a keepalive so the boards' watchdogs do not fire |
| `ServoTuner` | finds servo min / max / zero positions |
| `JetRangerRadioControllerEmulator`, `JetRangerUpperControllerEmulator` | stand-ins for the radio and upper boards |
| `cmdlineWarningMute`, `setcomstandby`, `FSUIPC` | small FSUIPC / SimConnect utilities and test code |
| `SCXPlatform`, `Managed Data Request`, `retired` | a cross-platform SimConnect library effort, the Prepar3D SDK sample the bridges came from, and archived code |

Python helpers (`Jet Ranger Python Code/`): a SimConnect exporter, a UDP test
receiver and a wake-on-LAN script.

## PCBs (`Jet Ranger/PCBs/`, KiCad)

- **Controller boards:** Radio Controller (four revisions: `Jet Ranger Radio
  Controller`, `Prod Radio Controller`, `DUE Prod Radio Controller` and
  `Due Controller as built 20250126`), Upper Controller, OLED Dual Stepper,
  13 Stepper, Front Panel Servo, COM and NAV, Auto Pilot Audio, Annunciator,
  Altimeter, X27 Small Stepper, Test, and an Experimental MAX7219 radio test.
- **Gauge boards:** Simkits Small / Large gauge and dual-gauge boards, a
  Simmeters large-gauge replacement and a Hornet dual gauge.
- **Matching to sketches is by name** (for example Front Panel Servo with the
  Servo Controller, OLED Dual Stepper with the Dual Stepper sketch). Verified
  from the files: on the Front Panel Servo board, `SW1` TOT CLEAR, `SW3`
  WARNING MUTE and `SW2` FUEL VALVE connect D43, D47 and D48 to GND — the pins
  the Servo sketch now reads.
- **Warning Mute:** only two boards have such an input: Front Panel Servo
  (`SW3`, D47) and Upper Controller (`J17` on `/ROW8`, Arduino D30, through diode
  `D5` to `/COL0` in the switch matrix). Nothing else mentions a warning mute or
  horn. See "Part 3 — PCB Notes" in the master document.

## Where things stand

Working and bench-verified (UDP debug logs from the boards, real swing timings):
all swing commands, OLED lamp test, Temperature OLED handling, the keepalive,
the debounced Servo inputs, and the end-to-end SwingTester sequence (driven by
a script sending the same packets; the app window itself was checked from
screenshots).

Known gaps / open items:
- The Servo board only logs the three switch transitions; nothing forwards them
  to the PC. The PC app on 27001 treats any unrecognised string as a radio
  frequency, so it would need updating first.
- The Upper Controller PCB has a Warning Mute connector on D30, but the Upper
  Controller sketch scans analog pins A9–A15 only, so it may not be read.
- `StepperVSITester` still broadcasts the RPME/RPMR controls to the single-board
  Stepper (.105) too; that board no longer handles them.
- The Dual Stepper's RAM is tight (75%, compiler low-memory warning) — be
  careful adding to it.
- The Clock OLED (mux channel 3) answered on the I2C scan in the last Dual
  Stepper boot log, but its display has not been confirmed by eye.
- `JET_RANGER_OLED_CONTROLLER` is an A-10C sketch, probably filed here by
  mistake.
- The calibration tables, swing times and wait settings come from bench
  measurements; recheck them after any mechanical change to a gauge.
