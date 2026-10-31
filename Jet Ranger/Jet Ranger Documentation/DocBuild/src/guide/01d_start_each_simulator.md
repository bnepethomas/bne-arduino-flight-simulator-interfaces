# Starting each simulator

This chapter is the start-up procedure for each simulator: **MSFS**, **Prepar3D / FSX**, **DCS World** and **X-Plane**. Every procedure includes the **Lowrance GPS interface**. You can follow it by hand, or let the start-up script do the PC side for you.

![Start-up order](diagrams/setup_flow.svg)

## The start-up script

`Start Jet Ranger.ps1` (in the `Jet Ranger` folder, with a `Start Jet Ranger.bat` that can be double-clicked) starts the PC programs for the simulator you choose, in the right order, and checks them. It never starts a program that is already running, so it can be run again safely.

| Step | What the script does |
|---|---|
| 1 | Checks that your **UDP debug reflector** is listening on port 27000. It does not start a second listener; if you set `Reflector` in `Start Jet Ranger.json` it starts that command when nothing is listening |
| 2 | Stops any **other bridge** that is running (only the four Jet Ranger bridge programs), because only one bridge may run at a time |
| 3 | Starts the **Health Monitor** |
| 4 | Starts the **Lowrance GPS interface** |
| 5 | Starts the **simulator** only when you give `-StartSimulator` **and** have set a program for it in `Start Jet Ranger.json` (a program path, or an address such as `steam://...` for a Steam game); otherwise it just reminds you to start it. It does not guess where a simulator is installed |
| 6 | Starts the **bridge** for the simulator (the DCS bridge gets the time zone) |
| 7 | Waits five seconds and checks that the bridge, the Health Monitor and the GPS interface are running, and that the GPS interface owns UDP port 27002 |

Windows may refuse to run `.ps1` scripts directly (*running scripts is disabled on this system*; this PC does). The `.bat` gets round that by starting PowerShell with the policy bypassed for that one run, so **use the `.bat`**: double-click it (it asks which simulator) or run it from a command window with the same options. Calling the script yourself needs `powershell -ExecutionPolicy Bypass -File "Start Jet Ranger.ps1" ...`.

```
"Start Jet Ranger.bat"
"Start Jet Ranger.bat" -Simulator MSFS
"Start Jet Ranger.bat" -Simulator P3D
"Start Jet Ranger.bat" -Simulator DCS -DcsTimeZone 4
"Start Jet Ranger.bat" -Simulator XPlane -XPlaneHost 192.168.1.20
"Start Jet Ranger.bat" -Simulator MSFS -StartSimulator
"Start Jet Ranger.bat" -Simulator DCS -DryRun
"Start Jet Ranger.bat" -Stop
```

| Option | Meaning |
|---|---|
| `-Simulator` | `MSFS`, `P3D`, `FSX`, `DCS` or `XPlane`. Left out, the script shows a menu |
| `-DcsTimeZone` | DCS only: the mission's time zone in hours from UTC (4 for the Caucasus and the Persian Gulf, 3 for Syria, -8 for Nevada), so the GPS shows the right UTC time |
| `-XPlaneHost` | X-Plane only: the PC X-Plane runs on, when it is not this one |
| `-StartSimulator` | Also starts the simulator, from the path (or `steam://` address) set in `Start Jet Ranger.json`; nothing is started for a simulator with no path set |
| `-NoGps`, `-NoHealthMonitor`, `-NoReflector` | Leave that part out |
| `-DryRun` | Shows what would be done without starting or stopping anything |
| `-Stop` | Stops the bridges, the GPS interface and the Health Monitor (the reflector and the simulator are left running) |

The optional settings file `Start Jet Ranger.json` (copy `Start Jet Ranger.sample.json` and fill in what you need) holds the `Reflector` command, the `Simulators` program paths, a default `DcsTimeZone` and a default `XPlaneHost`. A program that has not been built yet is reported by name: build it with `dotnet build -c Release` in its folder under `C# Code`. The script uses the newest build (Release or Debug) of each program.

The first time the GPS interface runs, and whenever its saved serial port is not on the PC, it shows a **Select GPS serial port** window and waits: pick the port the GPS cable is on (or press Cancel to run without a GPS). It does not listen for the simulator's data until you answer, so the script's check says it is waiting.

## What is the same for every simulator

1. Power the cockpit (main and avionics). The boards start and swing their gauges.
2. Make sure the **debug reflector** is running (UDP 27000).
3. Start the **Health Monitor**. Its lights turn green as the boards report in (within about 10 seconds).
4. Start the **GPS interface** (`LowranceAirMapGps.exe`) with the GPS cable on the saved serial port. Until a bridge is sending it reports the Brisbane Airport default and the PC clock.
5. Start the **simulator** and load an aircraft.
6. Start **one** bridge, and only one.
7. Check three things: the bridge says it is connected and receiving; the Health Monitor lights are green and the gauges follow the simulator; and the **GPS window says the time source is the simulator** (*simulator time*) and shows the aircraft's position, not Brisbane Airport.

If you change simulator, close the bridge first (or let the script stop it) and start the other one: the two simulators' bridges send to the same boards.

## MSFS 2020 and 2024

1. Reflector, Health Monitor and GPS interface as above.
2. Start MSFS and load a Jet Ranger.
3. Run `MSFSWinformsAutoCS.exe`. The status bar turns from red *Disconnected* to green *Connected to MSFS* with the aircraft name. Orange means the simulator is paused or loading.
4. The window lists the values being sent and the lamps. With the FlyInside B206 or the CowaSim Jet Ranger the bridge uses its estimated dials (see Aircraft with estimated dials).
5. The panel's switches reach MSFS as joystick buttons: bind them in the MSFS controls menu (the DS206 events are not available in MSFS).
6. The GPS interface takes the position, altitude, airspeed and heading from SimConnect and the simulator's zulu date and time.

Script: `"Start Jet Ranger.bat" -Simulator MSFS`.

## Prepar3D and FSX

1. Reflector, Health Monitor and GPS interface as above.
2. Start Prepar3D (or FSX) with FSUIPC installed and the DodoSim 206 loaded. FSUIPC must be running.
3. Run `FSUIPCWinformsAutoCS.exe`. The status bar shows when it is connected. The window lists the values being sent and the 21 annunciator bits.
4. The panel's joystick buttons must be bound to the DS206 events in FSUIPC (see Installing the DodoSim event file); without that nothing happens in the simulator when you press a switch.
5. The GPS interface takes the position, altitude, airspeed and heading from FSUIPC offsets and the simulator's UTC time from FSUIPC.

Script: `"Start Jet Ranger.bat" -Simulator P3D` (or `FSX`).

## DCS World

1. Reflector, Health Monitor and GPS interface as above.
2. DCS-BIOS must be installed once (see Running the DCS bridge): `Saved Games\DCS\Scripts\DCS-BIOS` and the `Export.lua` line. It is already installed on the cockpit PC.
3. Start DCS and **load a mission and sit in the cockpit** of the aircraft (the UH-1H Huey has a profile; any aircraft drives the airspeed, altitude, heading and GPS). DCS-BIOS only sends data while a mission is running.
4. Run `DCSBiosBridge.exe`. The stream line turns green (*receiving*) and shows the aircraft name and the profile it chose.
5. Set **Mission time zone (hours from UTC)** to the theatre's offset (or give `-DcsTimeZone` to the script), so the GPS time is right. The date is the PC's.
6. Fly the Huey and calibrate the needle gauges if they are marked *uncalibrated* (see Running the DCS bridge).
7. The panel's switches reach DCS as joystick buttons: bind them in the DCS controls.
8. The GPS interface follows the aircraft's position, altitude, airspeed and heading, and the mission clock.

Script: `"Start Jet Ranger.bat" -Simulator DCS -DcsTimeZone 4`.

## X-Plane

1. Reflector, Health Monitor and GPS interface as above.
2. In X-Plane open **Settings, Network** and allow incoming UDP connections (the receive port is 49000 by default).
3. Start X-Plane and load an aircraft.
4. Run `XPlaneBridge.exe`. It finds X-Plane from its beacon (or tries `127.0.0.1:49000`; use `-XPlaneHost` or `--xplane-host` if X-Plane is on another PC). The status line turns green and shows how many datarefs are arriving.
5. In the **Profile** box choose the aircraft's profile (*Robinson R22*, *Sikorsky S-76*) or *Generic (common values, any aircraft)*. X-Plane does not tell the bridge which aircraft is loaded.
6. If a gauge stays at zero, use **Search datarefs...** to find the right dataref and add it to the profile.
7. The panel's switches reach X-Plane as joystick buttons: bind them in the X-Plane joystick settings.
8. The GPS interface follows the aircraft's position, altitude, airspeed and heading, and X-Plane's zulu clock and date (the PC's year).

Script: `"Start Jet Ranger.bat" -Simulator XPlane`.

## If something does not come up

| Symptom | What to check |
|---|---|
| The script says a program is not built | Build it with `dotnet build -c Release` in its folder under `C# Code` |
| The script says nothing is listening on UDP 27002 | The GPS interface is waiting at its *Select GPS serial port* window: choose the port |
| The GPS still shows Brisbane Airport | No bridge is sending (check the bridge status), or the simulator is paused (no data for 3 seconds stops the bridge sending) |
| The GPS time is hours out with DCS | Set the bridge's Mission time zone |
| The bridge never connects | MSFS: aircraft loaded and not paused. Prepar3D / FSX: FSUIPC running. DCS: in the cockpit with DCS-BIOS installed. X-Plane: incoming UDP allowed |
| Gauges move but nothing is on the GPS | Another program owns UDP 27002 (close it), or two bridges are running |
