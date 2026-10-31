# Start-up script (Start Jet Ranger.ps1)

`Start Jet Ranger.ps1` (with a double-clickable `Start Jet Ranger.bat` and a sample settings file, `Start Jet Ranger.sample.json`) is in the `Jet Ranger` folder. It starts the PC side of the cockpit for one simulator, in order, and checks the result. It is plain Windows PowerShell 5.1 with no extra modules. Run it through the `.bat`: this PC's execution policy refuses to run `.ps1` files directly, and the `.bat` starts PowerShell with `-ExecutionPolicy Bypass` for that run only (the same works by hand: `powershell -ExecutionPolicy Bypass -File "Start Jet Ranger.ps1" -Simulator DCS`).

## What it does

1. **Finds the programs.** For each program it needs it takes the newest build (Release or Debug) under `Jet Ranger\ C# Code\<project>\bin` (the folder name starts with a space). A program that has not been built is reported by name and nothing is started.
2. **UDP debug reflector.** Reports whether something is listening on UDP 27000. It never starts a second listener on that port; only if the settings file names a `Reflector` command and nothing is listening does it run that command.
3. **One bridge.** Stops any other of the four bridge programs (`FSUIPCWinformsAutoCS`, `MSFSWinformsAutoCS`, `DCSBiosBridge`, `XPlaneBridge`) that is running: it closes the window, then ends the process if it is still there. No other program is ever stopped.
4. **Health Monitor and GPS interface.** Starts `JetRangerHealthMonitor` and `LowranceAirMapGps` unless they are already running.
5. **Simulator.** Only with `-StartSimulator` and a program set under `Simulators` in the settings file (a program path, or an address such as `steam://...`) does it start the simulator; otherwise it prints a reminder. The bridge is started straight afterwards, which is fine because every bridge keeps trying to connect.
6. **Bridge.** Starts the bridge for the simulator: `FSUIPCWinformsAutoCS` for P3D and FSX, `MSFSWinformsAutoCS` for MSFS, `DCSBiosBridge` for DCS (with `--utc-offset` from `-DcsTimeZone` or the settings file) and `XPlaneBridge` for X-Plane (with `--xplane-host` from `-XPlaneHost` or the settings file).
7. **Check.** After five seconds it reports whether the bridge, the Health Monitor and the GPS interface are running, and which process owns UDP 27002 (it should be `LowranceAirMapGps`; if nothing does, the GPS interface is waiting at its serial port window). It ends with what to look at in that simulator.

A program that is already running is left alone, so the script can be run again, or used to switch simulator: only the bridge changes.

## Options

| Option | Meaning |
|---|---|
| `-Simulator MSFS, P3D, FSX, DCS or XPlane` | The simulator; a menu is shown if it is left out |
| `-DcsTimeZone <hours>` | DCS bridge's Mission time zone (hours from UTC) |
| `-XPlaneHost <host>` | X-Plane's host, when it is not this PC |
| `-StartSimulator` | Also start the simulator from the settings file |
| `-NoGps`, `-NoHealthMonitor`, `-NoReflector` | Leave that part out |
| `-DryRun` | Print what would be done; start and stop nothing |
| `-Stop` | Stop the bridges, the GPS interface and the Health Monitor (not the reflector or the simulator) |

## Settings file

`Start Jet Ranger.json`, next to the script, is optional: `Reflector` (a command line that starts the UDP reflector), `Simulators` (a program per simulator for `-StartSimulator`), `DcsTimeZone` and `XPlaneHost`. `Start Jet Ranger.sample.json` shows the layout. The script does not guess simulator paths.

## Per-simulator start-up

| Simulator | Bridge | Before the bridge connects | The GPS interface is fed |
|---|---|---|---|
| MSFS 2020 / 2024 | `MSFSWinformsAutoCS` | A Jet Ranger loaded and not paused | Position, altitude, airspeed and heading from SimConnect, and the zulu date and time |
| Prepar3D / FSX | `FSUIPCWinformsAutoCS` | FSUIPC running, DodoSim 206 loaded; buttons bound to the DS206 events | FSUIPC offsets and the simulator's UTC time |
| DCS World | `DCSBiosBridge` | DCS-BIOS installed, a mission running in the cockpit; set the Mission time zone | CommonData position, altitude, airspeed and heading, and the mission clock less the time zone |
| X-Plane | `XPlaneBridge` | Settings, Network: incoming UDP allowed; a profile chosen | `latitude`, `longitude`, altitude, airspeed and `true_psi` datarefs, and the zulu clock and date |

The step-by-step version for each simulator is in the User Guide (*Starting each simulator*).

## Tested

Dry runs for all four simulators and a live run (Health Monitor, GPS interface and the DCS bridge started; started again with nothing to do; switched to X-Plane, which stopped the DCS bridge; `-Stop` ended all of them) on 31 October 2026. The simulators and the real GPS were not involved. On the PC it was tried on, the saved GPS port (COM25) was not present, so the GPS interface showed its serial port window, which the script's check reports.
