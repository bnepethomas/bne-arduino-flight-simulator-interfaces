# Overview

The Jet Ranger cockpit drives real gauge needles, warning lights, radio displays and switches from the simulator. A Windows PC exchanges UDP packets over Ethernet with Arduino Mega 2560 panel boards, and a small USB board presents the panel switches to Windows as joysticks.

![The whole system](diagrams/system_overview.svg)

Data flows from the PC to the boards as short text packets. On the primary PC a Python listener shows the boards' debug messages and a separate Windows app, the Health Monitor, tracks their keep-alives. Cockpit switches reach the simulator separately, as joystick buttons (and, in Prepar3D and FSX, FSUIPC events).

| Part | Hardware and software | Address | Section |
|---|---|---|---|
| Primary PC | Windows, MSFS 2020 or 2024, or Prepar3D / FSX with DodoSim 206 V2, one bridge program, debug listener, Health Monitor | 172.16.1.10 | Windows and simulator |
| Radio controller | Mega 2560 with the Radio Auto Pilot PCB and the COM and NAV panel | 172.16.1.101 | Radio controller |
| Servo board | Mega 2560 and Ethernet module on the Front Panel Servo PCB | 172.16.1.102 | Servo controller |
| Upper controller | Mega 2560 scanning the overhead switch matrix | 172.16.1.103 | Joystick (HID) interface |
| ADF controller | Mega 2560 with the ADF knobs, aircraft ID keypad, gear, flaps, speed brake and brakes switches and landing gear lamps | 172.16.1.107 | ADF controller |
| Stepper board | Mega 2560 on the Jet Ranger 13 Stepper PCB | 172.16.1.105 | Stepper controller |
| Dual stepper board | Mega 2560 on the OLED Dual Stepper PCB | 172.16.1.106 | Dual stepper controller |
| USB HID board | Leonardo-class board with native USB | 172.16.1.11 | Joystick (HID) interface |

# Windows and simulator

The Windows PC runs the simulator and a **bridge program** that turns simulator variables into UDP text packets for the Arduino boards. Switch inputs travel the other way as joystick buttons.

The bridge sends the Servo, Stepper and Dual Stepper boards each their own packet in **real units** (degrees, knots, feet per minute, psi, percent, degrees C), and the boards convert them with their own calibration. It sends about 10 packets per second to each board; the radio gets a packet whenever a frequency changes and at least every 5 seconds. The Dual Stepper's temperature OLED shows the outside air temperature from the simulator.

| Component | Role | Notes |
|---|---|---|
| Simulator | Flies the DodoSim 206 or another Bell 206 Jet Ranger | MSFS 2020 or 2024, Prepar3D or FSX |
| FSUIPCWinformsAutoCS | Bridge for Prepar3D and FSX, reads FSUIPC offsets | Needs FSUIPC |
| MSFSWinformsAutoCS | Bridge for MSFS, reads SimConnect | Has estimates for the FlyInside B206 and CowaSim Jet Ranger |
| FSUIPC and DODSIM206V2.EVT | Exposes 234 DodoSim custom events so switches can be bound | Prepar3D and FSX only |
| Python debug listener | Shows debug messages from the boards | Listens on UDP 27000 |
| Health Monitor | Tracks each board's keep-alive and flags a board that goes quiet | Listens on UDP 13137 |
| LowranceAirMapGps | Feeds a Lowrance AirMap 2000C GPS from the simulator over a serial port | Listens on UDP 27002; see Lowrance AirMap GPS |

Run **only one bridge at a time**: both listen for the radio board's commands on UDP 27001. Both bridges also send the aircraft position, altitude, airspeed, heading and simulator date and time to the GPS interface on UDP 27002 (on the same PC).

## Starting up

![Start-up order](diagrams/setup_flow.svg)

1. Power the cockpit (main and avionics).
2. Start the debug listener (UDP 27000).
3. Start the simulator.
4. Start **one** bridge.
5. Open the Health Monitor: every light should turn green within about 10 seconds.
6. Fly. The gauges follow the simulator.

The order matters little: every board waits for data and the bridge reconnects to the simulator on its own every second. A gauge that stays at zero for 30 seconds without data was parked on purpose by the board's watchdog.

![Health Monitor](diagrams/health_monitor.svg)

## Running the MSFS bridge

1. Build the project `MSFSWinformsAutoCS` (in the repository's `C# Code` folder) and run `MSFSWinformsAutoCS.exe` from its `bin\Debug` folder. Build output is not stored in git, so a fresh copy of the repository has to be built first.
2. Close the FSUIPC bridge if it is running, then start MSFS and load a Jet Ranger. The status bar changes from *Disconnected* (red) to *Connected to MSFS* with the aircraft name (green) once data arrives.
3. The window lists the values being sent and the warning lamps. The Avionics Master box follows the simulator and can also be used to switch it.
4. If the status turns orange and says no data, the simulator is paused or loading and the bridge has stopped sending; the boards return their gauges to zero after 30 seconds and carry on when data returns.
5. Five lamps are driven: engine out, rotor low, low fuel, generator fail (engine speed below 10 percent) and transmission oil pressure (below 40 psi, using the estimated pressure on the FlyInside and CowaSim). The other lamps need aircraft-specific variables. Some MSFS Jet Rangers do not report every value (for example transmission pressure and temperature), so those gauges may stay at zero. With the FlyInside B206 the estimated transmission pressure (35 psi) is below the 40 psi threshold, so that lamp stays on.
6. The status bar shows the aircraft name the simulator reports and follows it when you load a different one.
7. The electrical load follows the size of the total load whichever way the current flows, so with the engine stopped it still shows a load.

![Bridge status](diagrams/msfs_states.svg)

### Aircraft with estimated dials

![Which values come from where](diagrams/aircraft_profiles.svg)

**FlyInside B206.** Its own gauges show values that are not in the standard simulator variables: rotor speed, engine speed, gas producer, torque, turbine out temperature, fuel pressure and transmission oil. The bridge reads the aircraft's own rotor and engine speed variables and estimates the rest from cockpit dial readings: torque follows the collective lever, turbine out temperature rises with torque, and the transmission oil pressure and temperature are modelled (the temperature warms up slowly from the outside air temperature and starts again from cold each time the bridge restarts). In this aircraft holding full collective for more than about 2 seconds over-torques the engine and breaks it. Its cockpit load dial stays at about 50 percent whatever the load.

![Torque estimates](diagrams/torque_curves.svg)

**CowaSim Jet Ranger** (shown as `206B3` with its registration). Most gauges match the standard variables. Its torque, turbine out temperature and electrical load dials do not, and it has no transmission oil values: the bridge smooths the torque, takes the turbine out temperature from the exhaust gas temperature, rescales the load and estimates the transmission oil from rotor speed and engine oil temperature.

These estimates were fitted to a few dial readings and may be a few percent off at other power settings, so compare the panel gauges with the cockpit dials. Other aircraft use the standard variables.

## Running the FSUIPC bridge (Prepar3D and FSX)

Start FSUIPC with the simulator, then run `FSUIPCWinformsAutoCS.exe`. The status bar shows when it is connected to Prepar3D. The window lists the values being sent and the 21 annunciator bits. The bridge sends about 10 packets per second to each board and listens on UDP 27001 for the radio board's commands (frequency swaps, standby tuning, battery, alternator, avionics master).

## Installing the DodoSim event file (FSUIPC)

Copy `DODSIM206V2.EVT` into the `{P3D or FSX}/Modules` folder, next to `FSUIPC.ini`. Add this to `FSUIPC.ini` if FSUIPC has not added it itself:

```
[EventFiles]
0=DodoSim206
```

Restart the simulator. The `DS206.*` events now appear in FSUIPC's button and key assignment lists. Bind each physical switch to its event. After an EVT update FSUIPC re-indexes events alphabetically, so check and re-map bindings.

### Event naming

| Suffix | Parameter | Behaviour | Example |
|---|---|---|---|
| `_SET` | Integer (usually 1 = on, 0 = off) | Sets a state directly; best for latching toggle switches | `DS206.BATTERY_SWITCH_SET` |
| `_TOGGLE` | None | Flips the state each press | `DS206.FUEL_VALVE_SWITCH_TOGGLE` |
| `_TRIGGER` | None (some take 1 to 10 as a multiplier) | Momentary; tick Control to repeat while held | `DS206.STARTER_BUTTON_TRIGGER` |
| none | None | One-shot action | `DS206.CLOCK_MODE_BUTTON` |

For physical latching switches prefer the `_SET` events: send 1 on switch-on and 0 on switch-off so the simulator never drifts out of step with the panel.

### Event groups at a glance

| Group | Event numbers | Examples |
|---|---|---|
| Circuit breakers | 36 to 95, 107 to 110 | Hoist, SAS, fuel pumps, governor, ignition, starter |
| Lights and master switches | 96 to 101, 111 to 122, 209 to 212, 224 to 227 | Battery, generator, avionics, pitot heat, landing lights |
| Engine and flight controls | 1 to 2, 207 to 208, 213 to 223, 228 to 232 | Rotor brake, hydraulics, force trim, starter, governor beep |
| Instruments | 3 to 33 | Annunciator test, AI cage, radar altimeter, HSI and OBS knobs |
| Radios and audio | 129 to 192 | COM and NAV swap and tuning, ADF, DME, audio panel |
| Transponder | 193 to 206 | Mode knob, digit buttons, IDT, VFR |
| SAS modes | 123 to 128 | Master, HDG, NAV, SPD, ALT, VRT |
| Other | 102 to 106, 233 to 234 | Difficulty rotary, heat and vent, external power |
