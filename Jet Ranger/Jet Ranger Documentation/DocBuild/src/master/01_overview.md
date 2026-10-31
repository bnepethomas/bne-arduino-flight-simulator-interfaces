# Project overview

A home-built Bell 206 Jet Ranger cockpit: real instrument gauges, warning lamps, radio and overhead switch panels and small displays, driven from a flight simulator by Arduino Mega boards on a private Ethernet network. The simulator can be Microsoft Flight Simulator 2020 or 2024 (through SimConnect), Prepar3D / FSX (through FSUIPC), DCS World (through DCS-BIOS, first the UH-1H Huey) or X-Plane (through its UDP datarefs).

This document ties together the three parts of the repository: the **Arduino sketches**, the **Windows (C#) programs** and the **KiCad PCBs**. It is rebuilt from each project's own `PROGRAM_SUMMARY.md` and from the code, and it covers only the projects that are in use.

## How it fits together

![The whole system: simulator, PC bridge, Ethernet and the panel boards](diagrams/system_overview.svg)

- The **simulator** supplies the flight data.
- A **bridge program** on the PC reads the data, converts it to real units and sends one short UDP text packet to each board about ten times a second.
- Each **Arduino board** turns the real units into stepper steps, servo positions, lamp states or display text with calibration tables in its own sketch.
- Switches and knobs go back the other way: the radio board sends commands to the bridge (UDP 27001), the ADF controller sends its frequency, radar follow ID and power status to the PC (UDP 27003, not read by the bridges yet), and the switch boards send joystick button changes to a USB HID board (UDP 4210) which Windows sees as game controllers.
- Every board sends a **heartbeat** every 10 seconds to the Health Monitor (UDP 13137) and its **debug text** to a listener on the PC (UDP 27000).

![From simulator value to moving needle](diagrams/data_pipeline.svg)

## Arduino sketches

All sketches are in `Jet Ranger Arduino Sketches/`. Each is an Arduino Mega 2560 with a W5500 Ethernet module unless noted.

| Sketch | IP | Role |
|---|---|---|
| `JET_RANGER_RADIO_CONTROLLER` | .101 | COM and NAV frequency displays, eight tuning knobs, four frequency-swap switches; sends tuning and swap commands to the PC |
| `JET_RANGER_SERVO_CONTROLLER` | .102 | Attitude indicator pitch and bank servos, 15 warning lamps, three front-panel switches (TOT clear, warning mute, fuel valve) |
| `JET_RANGER_ADF_CONTROLLER` | .107 | The combined ADF and Upper board: ADF major and minor knobs, aircraft ID keypad for radar following, seven panel switches (gear, flaps, speed brakes, brakes, anti-ice, hyd boost, caution dim; joystick buttons and bridge commands), landing gear lamps, 12 V and 5 V rail monitoring with a status every 30 seconds, character OLED, and (merged in from the old Upper controller on 31 October 2026) the overhead switch matrix (72 positions) and the Upper keypad, forwarded to the USB HID board. The brightness pot on the PCB is switched off for the moment |
| `JET_RANGER_UPPER_CONTROLLER` | (.103, retired) | The old overhead switch matrix and keypad board; its code now runs inside the ADF controller sketch. The folder is kept for reference |
| `UDP_TO_HID` | .11 | Leonardo-class board: UDP in, four 32-button joysticks and a keyboard out over USB |
| `JET_RANGER_STEPPER_CONTROLLER` | .105 | Stepper gauges: airspeed, vertical speed, radar altitude, torque, gas producer, turbine out temperature, fuel quantity |
| `JET_RANGER_OLED_DUAL_STEPPER_CONTROLLER` | .106 | Stepper gauges: engine and transmission oil, engine and rotor RPM, electrical load, fuel load, altimeter; four OLEDs (barometer, altimeter, clock, temperature) |

Common to the three gauge boards (Servo, Stepper, Dual Stepper):

- bench-measured calibration tables with linear interpolation from real units to steps or servo positions, plus a raw bypass (`<CODE>RAW`) for calibration work;
- start-up self-test swings, also callable on demand (`C,SWINGTQ:1` and so on);
- a 30 second no-data watchdog that returns every gauge to zero;
- remote reset (`C,RESET:1`).

## Windows programs

All programs are in `Jet Ranger/ C# Code/` (note the leading space in the folder name).

| Program | Purpose |
|---|---|
| `FSUIPCWinformsAutoCS` | Bridge for Prepar3D / FSX using FSUIPC offsets; also sends position, altitude, airspeed, heading and the simulator date and time to the GPS interface |
| `MSFSWinformsAutoCS` | Bridge for MSFS 2020 / 2024 using SimConnect, with estimates for aircraft whose dials do not read the standard variables (FlyInside B206, CowaSim Jet Ranger); also feeds the GPS interface |
| `DCSBiosBridge` | Bridge for DCS World through the DCS-BIOS export stream, output only: Huey caution lamps (Huey Warning Panel LEDs), radar altitude and, for every aircraft, airspeed, altitude, heading and position, plus needle-position gauges for the UH-1H that still need calibrating; profile files per aircraft |
| `XPlaneBridge` | Bridge for X-Plane 11 and 12 over the UDP dataref interface (no plugin), output only: a generic profile for any aircraft plus Robinson R22 and Sikorsky S-76 profiles; not yet run against a real X-Plane |
| `JetRangerHealthMonitor` | Traffic-light dashboard for the 10 second board heartbeats |
| `StepperVSITester` | Bench tester that sends real and raw values to the stepper boards and the servo board |
| `SwingTester` | Runs every gauge's swing individually or in a fixed order, lights the OLEDs and warning lamps, tests the Auto Pilot and Audio panel LEDs one by one, resets boards, and sets the backlight and LED brightness |
| `LowranceAirMapGps` | Makes the PC a GPS source for a Lowrance AirMap 2000C: NMEA over a serial port, fed by the bridges over UDP, defaulting to Brisbane Airport |
| `LowranceAirMapGpsBenchTester` | Bench sender for the GPS interface: typed values, 8-way cursor, country / city / airport selection, Fly mode and a date / time clock |
| `JetRangerRadioControllerEmulator`, `JetRangerUpperControllerEmulator` | Stand-ins for the radio and upper boards when they are disconnected |

Python helpers in `Jet Ranger Python Code/`: a SimConnect exporter, a UDP test receiver and a wake-on-LAN script. The UDP debug listener (port 27000) is a small Python reflector that prints every board's debug text and can forward a copy to a second listener.

Run **only one bridge at a time**: the FSUIPC and MSFS bridges both listen on UDP 27001, and all four send to the same boards. The DCS and X-Plane bridges read their simulator and send only; they do not listen on 27001.

## PCBs

KiCad projects are in `Jet Ranger/PCBs/`. The schematic and board layout of every one are in Appendix E, top-side pictures with the external connectors labelled, for installing the controller boards, are in Appendix F, and a map of every gauge, lamp and display with the connector that drives it is in Appendix G. The boards used by the sketches and programs above are:

| PCB | Revision | Used by |
|---|---|---|
| Jet Ranger Radio Auto Pilot | 1.3 | Radio controller |
| Jet Ranger COM and NAV | no revision set | Radio panel (four swap switches, encoders) |
| Jet Ranger Front Panel Servo | 1.4 | Servo controller |
| Jet Ranger Upper ADF Controller | 1.0 | ADF and Upper controller (the reference PCB for the ADF controller; replaces the Jet Ranger Upper Controller 1.4 and Jet Ranger ADF Controller 1.6 boards) |
| Jet Ranger 13 Stepper | 1.6 | Stepper controller |
| Jet Ranger OLED Dual Stepper | 1.5 | Dual stepper controller |
| Jet Ranger X27 Small Stepper | not set | One per stepper gauge |
| Jet Ranger Audio Panel, Auto Pilot Panel, ADF Panel | 1.1 | Switch panels for the Radio Auto Pilot and ADF controllers |
| Annunciator, Altimeter, Large Guage, Simkits gauge boards | | Other panels and gauges |

On 18 October 2026 the six controller boards of the time (Servo, Upper, 13 Stepper, Dual Stepper, Radio Auto Pilot and ADF Controller) were revised to fix the **backlight MOSFET**: the schematic symbol had source and drain swapped and the part is now an IRF540N. All of them drive their panel backlights from Mega pin D5 through this transistor (see the PCB notes chapter). The Upper and ADF boards were then combined into one, the Jet Ranger Upper ADF Controller (revision 1.0, 26 October 2026), so there are now five controller boards.

![The active PCBs, their revision and the sketches they belong to](diagrams/pcb_map.svg)

## Where things stand

Bench-verified with UDP debug logs from the boards and real swing timings: every swing command, the OLED lamp test, the temperature OLED, the keep-alive, the debounced servo inputs, the radio matrix, the remote reset on every board except the old Upper controller (whose code now runs on the ADF controller), and the full SwingTester sequence. The FSUIPC bridge has been run against Prepar3D v6.1 and the MSFS bridge against MSFS 2024 with the FlyInside B206 and the CowaSim Jet Ranger. The DCS bridge (added 31 October 2026) has run against live DCS with the Huey caution lamps; its gauge values and the X-Plane bridge (also 31 October 2026) are tested only with synthetic data from fake simulators.

Backlight control (Servo, Stepper and Dual Stepper boards, and later the Radio controller) and the GPS interface were added on 18 October 2026: the backlight follows the main bus, the last commanded brightness and the 30 second no-data watchdog, and the GPS interface feeds a Lowrance AirMap 2000C from the simulator. Both are tested on the bench with packets; the GPS interface has not been tested against the real GPS, and the FSUIPC and MSFS bridges' GPS output has not been run against a simulator.

Open items:

- The three servo-board switches reach the HID board but are not sent to the PC itself (the bridges treat an unrecognised string on port 27001 as a radio frequency, so they would need updating first).
- The Upper switch matrix and Upper keypad now run on the combined ADF and Upper board (merged on 31 October 2026) and have not been bench-tested there. Its Warning Mute and TOT Test connectors are read by the matrix (indexes 88 and 89).
- The bridges and the SwingTester still send to the retired Upper address 172.16.1.103; the entries should be removed.
- The 16 Huey needle-position gauges in the DCS bridge are on first-guess scales and need calibrating in flight; the X-Plane bridge has not met a real X-Plane (check units, the R22 and S-76 rotor speed scales and the approximate Huey LEDs); neither sends inputs, port 27001 commands or the landing gear lamps.
- The estimates in the MSFS bridge for the FlyInside and CowaSim aircraft come from a few dial readings; check them at other power settings.
- The brightness pot input on A0 of the combined board is switched off in the sketch (`USE_BRIGHTNESS_POT` is 0); its backlight follows `BPWR`, `C,B` and the no-data timeout.
- The Radio controller emulator still sends a bare frequency for every radio, so only COM1 tunes from it; the real board sends labelled COM2, NAV1 and NAV2 values, which both bridges understand.
- The Dual Stepper's RAM use is high (79 percent); be careful adding code to it.
- Calibration tables, swing times and waits come from bench measurements; recheck them after any mechanical change to a gauge.
