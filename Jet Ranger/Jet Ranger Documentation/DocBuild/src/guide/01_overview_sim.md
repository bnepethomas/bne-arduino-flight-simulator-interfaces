# Overview

The Jet Ranger cockpit drives real gauge needles, warning lights, radio displays and switches from the simulator. A Windows PC exchanges UDP packets over Ethernet with Arduino Mega 2560 panel boards, and a small USB board presents the panel switches to Windows as joysticks.

![The whole system](diagrams/system_overview.svg)

Data flows from the PC to the boards as short text packets. On the primary PC a Python listener shows the boards' debug messages and a separate Windows app, the Health Monitor, tracks their keep-alives. Cockpit switches reach the simulator separately, as joystick buttons (and, in Prepar3D and FSX, FSUIPC events).

| Part | Hardware and software | Address | Section |
|---|---|---|---|
| Primary PC | Windows, MSFS 2020 or 2024, Prepar3D / FSX with DodoSim 206 V2, DCS World or X-Plane, one bridge program, debug listener, Health Monitor | 172.16.1.10 | Windows and simulator |
| Radio controller | Mega 2560 with the Radio Auto Pilot PCB and the COM and NAV panel | 172.16.1.101 | Radio controller |
| Servo board | Mega 2560 and Ethernet module on the Front Panel Servo PCB | 172.16.1.102 | Servo controller |
| ADF and Upper controller | Mega 2560 on the combined Upper ADF Controller PCB: the ADF knobs, aircraft ID keypad, gear, flaps, speed brake, brakes, anti-ice, hyd boost and caution dim switches, landing gear lamps, and the overhead switch matrix and keypad of the old Upper controller (172.16.1.103, retired) | 172.16.1.107 | ADF and Upper controller, Joystick (HID) interface |
| Stepper board | Mega 2560 on the Jet Ranger 13 Stepper PCB | 172.16.1.105 | Stepper controller |
| Dual stepper board | Mega 2560 on the OLED Dual Stepper PCB | 172.16.1.106 | Dual stepper controller |
| USB HID board | Leonardo-class board with native USB | 172.16.1.11 | Joystick (HID) interface |

# Windows and simulator

The Windows PC runs the simulator and a **bridge program** that turns simulator variables into UDP text packets for the Arduino boards. Switch inputs travel the other way as joystick buttons.

The bridge sends the Servo, Stepper and Dual Stepper boards each their own packet in **real units** (degrees, knots, feet per minute, psi, percent, degrees C), and the boards convert them with their own calibration. It sends about 10 packets per second to each board; the radio gets a packet whenever a frequency changes and at least every 5 seconds. The Dual Stepper's temperature OLED shows the outside air temperature from the simulator.

| Component | Role | Notes |
|---|---|---|
| Simulator | Flies the DodoSim 206 or another Bell 206 Jet Ranger, or (with the DCS and X-Plane bridges) the Huey or any other helicopter | MSFS 2020 or 2024, Prepar3D, FSX, DCS World or X-Plane |
| FSUIPCWinformsAutoCS | Bridge for Prepar3D and FSX, reads FSUIPC offsets | Needs FSUIPC |
| MSFSWinformsAutoCS | Bridge for MSFS, reads SimConnect | Has estimates for the FlyInside B206 and CowaSim Jet Ranger |
| DCSBiosBridge | Bridge for DCS World, reads the DCS-BIOS export stream (output only; first the UH-1H Huey) | Needs DCS-BIOS installed in DCS |
| XPlaneBridge | Bridge for X-Plane 11 and 12, reads datarefs over UDP (output only, no plugin) | Not yet tried against a real X-Plane |
| FSUIPC and DODSIM206V2.EVT | Exposes 234 DodoSim custom events so switches can be bound | Prepar3D and FSX only |
| Python debug listener | Shows debug messages from the boards | Listens on UDP 27000 |
| Health Monitor | Tracks each board's keep-alive and flags a board that goes quiet | Listens on UDP 13137 |
| LowranceAirMapGps | Feeds a Lowrance AirMap 2000C GPS from the simulator over a serial port | Listens on UDP 27002; see Lowrance AirMap GPS |

Run **only one bridge at a time**: the FSUIPC and MSFS bridges both listen for the radio board's commands on UDP 27001, and all four bridges send to the same boards. Both bridges also send the aircraft position, altitude, airspeed, heading and simulator date and time to the GPS interface on UDP 27002 (on the same PC).

## Starting up

![Start-up order](diagrams/setup_flow.svg)

1. Power the cockpit (main and avionics).
2. Start the debug listener (UDP 27000), the Health Monitor and the Lowrance GPS interface.
3. Start the simulator.
4. Start **one** bridge.
5. Check: every Health Monitor light should turn green within about 10 seconds, and the GPS window should show the simulator as its time source.
6. Fly. The gauges follow the simulator.

The procedure for each simulator (MSFS, Prepar3D / FSX, DCS World and X-Plane), and the `Start Jet Ranger.ps1` script that does the PC side of it, are in the next chapters (Starting each simulator).

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

## Installing DCS-BIOS (DCS World)

The DCS bridge reads **DCS-BIOS**, a free open-source add-on (DCS-Skunkworks, GitHub: `DCS-Skunkworks/dcs-bios`) that makes DCS broadcast its cockpit state to other programs on the PC. It is installed **once per PC** and is already installed on the cockpit PC (version 0.11.8). DCS-BIOS only reads from DCS; it changes nothing in the simulation unless something sends it commands, and the DCS bridge does not.

1. **Close DCS.**
2. **Download the release.** On the Releases page of `github.com/DCS-Skunkworks/dcs-bios` download the release zip, for example `DCS-BIOS_v0.11.8.zip`. Use the release zip, not the *Source code* download: the release carries the aircraft files (`doc\json`) that the bridge reads.
3. **Find your DCS Saved Games folder:** `C:\Users\<you>\Saved Games\DCS` (press Windows+R and type `%USERPROFILE%\Saved Games` to open the parent). If you fly the open beta use `DCS.openbeta` instead; use the folder your DCS actually writes its `Logs` folder in. If it has no `Scripts` folder, create one.
4. **Copy the `DCS-BIOS` folder** from the zip into that `Scripts` folder, so that you end up with `Saved Games\DCS\Scripts\DCS-BIOS\BIOS.lua` next to `BIOSConfig.lua`, `lib` and `doc`.
5. **Make DCS load it.** DCS runs the file `Saved Games\DCS\Scripts\Export.lua` when a mission starts. Open it in Notepad (create it if it does not exist) and make sure it contains this line:

```
dofile(lfs.writedir() .. [[Scripts\DCS-BIOS\BIOS.lua]])
```

   If `Export.lua` already exists because another tool uses it (for example Tacview or SimpleRadio Standalone), **do not replace it**: add this line at the end, on a line of its own, and leave the other lines as they are. DCS only runs this one file, so every tool's line has to be in it.
6. **Start DCS, start a mission and get into the cockpit.**
7. **Check it worked.**
   - `Saved Games\DCS\Logs\DCS-BIOS.log` now exists and ends with lines such as `starting dcs-bios` and `mission start with aircraft: UH-1H`.
   - Start `DCSBiosBridge.exe` (or the start-up script): the stream line turns green (*receiving*) and shows the aircraft name.

DCS-BIOS sends its data on UDP multicast 239.255.50.10 port 5010 for programs on the PC and listens for commands on port 7778; the settings are in `Scripts\DCS-BIOS\BIOSConfig.lua` and need no change. If Windows asks whether to let DCS use the network, allow it on private networks. With `dev_mode` on in `BIOSConfig.lua` (as in the release installed here) DCS-BIOS also rewrites its aircraft files in `Scripts\DCS-BIOS\doc\json` when DCS runs, so they follow the DCS version you fly.

**Updating DCS-BIOS.** Close DCS, delete the old `Scripts\DCS-BIOS` folder, copy the new release's `DCS-BIOS` folder in its place and leave `Export.lua` as it is. Check the aircraft you fly still shows *receiving* afterwards.

**Removing it.** Delete the `dofile` line from `Export.lua` and the `Scripts\DCS-BIOS` folder.

| Symptom | What to check |
|---|---|
| No `DCS-BIOS.log`, and the bridge says *no data yet* | The `dofile` line is not in `Saved Games\DCS\Scripts\Export.lua` (or the file is in the wrong Saved Games folder, `DCS` against `DCS.openbeta`); the `DCS-BIOS` folder is not directly inside `Scripts` |
| Another tool stopped working after the install | `Export.lua` was replaced instead of having the line added: put that tool's lines back |
| The log exists but the bridge still says *no data yet* | You are not in a mission yet (stay in the cockpit, not the menu), or another program is using UDP 5010 |
| The bridge says *receiving* but the aircraft has no profile | Any aircraft still drives airspeed, altitude, heading and the GPS; only the UH-1H has lamps and gauges so far |

## Running the DCS bridge (DCS World)

The DCS bridge reads **DCS-BIOS**, a free add-on that makes DCS broadcast its cockpit state on the PC. It only sends data to the panel: the panel's switches reach DCS as joystick buttons (bind them in DCS as usual), and the radio knobs do not tune DCS radios yet.

1. Install DCS-BIOS once (see Installing DCS-BIOS above; it is already installed on the cockpit PC). Start DCS and load a mission with an aircraft; DCS-BIOS keeps its aircraft files in `Saved Games\DCS\Scripts\DCS-BIOS\doc\json`.
2. Build `DCSBiosBridge` (in the repository's `C# Code` folder) and run `DCSBiosBridge.exe` from its `bin` folder. Close the other bridges first.
3. The window shows the stream status, the aircraft name DCS-BIOS reports and the profile it chose. With **Send to the boards** ticked the values go to the boards; the token table shows each value and where it came from.
4. **UH-1H Huey.** The 20 caution lights drive the Huey Warning Panel LEDs on the Radio controller; they light only while the Radio is powered, that is while the aircraft has power. The radar altitude goes to the radar altitude gauge. For every aircraft the airspeed, altitude, heading and position are sent (the GPS interface follows the aircraft).
5. **GPS time.** The Lowrance GPS shows the mission clock. DCS gives the mission's local time, so set **Mission time zone (hours from UTC)** to the theatre's offset (for example 4 for the Caucasus and the Persian Gulf, 3 for Syria, -8 for Nevada); the box starts at 0 each run, or start the bridge with `--utc-offset 4`.
6. **Gauges that need calibrating.** The other Huey gauges (torque, gas producer, turbine temperature, fuel, oil, rotor and engine speed, electrical load, vertical speed, pitch and bank) are read as needle positions and shown with a first-guess scale, marked *uncalibrated*. To correct one: fly so the cockpit gauge reads a known value, select its row, press **Calibrate selected...** and type the reading; repeat at two or more readings. The points are kept in `profiles\UH-1H.calibration.json`. **Clear its calibration** removes them.
7. For another aircraft the bridge uses the common values until a profile file for it is added.

## Running the X-Plane bridge

The X-Plane bridge needs no plugin: it asks X-Plane for datarefs over UDP, and it only sends data to the panel (the switches reach X-Plane as joystick buttons). It has not been tried against a real X-Plane yet, so expect to check the values.

1. In X-Plane open Settings, Network and allow incoming UDP connections (the default receive port is 49000).
2. Build `XPlaneBridge` and run `XPlaneBridge.exe` from its `bin` folder. Close the other bridges first. With no options it finds X-Plane from its beacon, or tries `127.0.0.1:49000`; use `--xplane-host` and `--xplane-port` if X-Plane is on another PC.
3. Choose the profile in the Profile box: *Generic (common values, any aircraft)* works with any aircraft that fills in the standard datarefs, *Robinson R22* and *Sikorsky S-76* add their rotor speed and bus voltage. X-Plane does not tell the bridge which aircraft is loaded, so choose it yourself.
4. The window shows whether data is arriving. A gauge that stays at zero with the generic profile means the aircraft does not fill in that dataref: press **Search datarefs...**, type words such as `oil pressure`, and add the right dataref to a profile file in the `profiles` folder.
5. The warning lamps and the Huey LEDs follow X-Plane's generic annunciators, so they are approximate.
6. The Lowrance GPS follows X-Plane's zulu clock and date (the PC's year is used).

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
