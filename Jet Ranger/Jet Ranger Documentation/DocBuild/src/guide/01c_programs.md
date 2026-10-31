# Program windows

This chapter shows every window of the Windows programs in `Jet Ranger/ C# Code/`. The screenshots were taken from the current builds. The two bridges were captured with no simulator running, so they show their disconnected state; with a simulator connected the left box fills with the values being sent and the list shows the lamps lighting.

| Program | Used for | Section |
|---|---|---|
| FSUIPCWinformsAutoCS | Bridge for Prepar3D and FSX | Bridges |
| MSFSWinformsAutoCS | Bridge for MSFS 2020 and 2024 | Bridges |
| DCSBiosBridge | Bridge for DCS World (through DCS-BIOS) | Bridges |
| XPlaneBridge | Bridge for X-Plane (through UDP datarefs) | Bridges |
| JetRangerHealthMonitor | Shows which boards are alive and the last voltage reading from each | Health Monitor |
| SwingTester | Swings every gauge, lamps, OLEDs, tests the Auto Pilot, Audio and Huey panel LEDs, resets boards, sets brightness | SwingTester |
| StepperVSITester | Sends real and raw values to the stepper boards and the servo | StepperVSITester |
| JetRangerRadioControllerEmulator, JetRangerUpperControllerEmulator | Stand in for the radio and upper boards | Emulators |
| LowranceAirMapGps | Feeds the Lowrance AirMap 2000C | GPS interface |
| LowranceAirMapGpsBenchTester | Sends aircraft data to the GPS interface for testing | GPS bench tester |

> **Careful:** the SwingTester, StepperVSITester and the two emulators send packets to the real boards (the addresses are in the boxes) as soon as you use their controls. Close them, or point them at the right addresses, before you press anything while the cockpit is powered.

## Bridges

![FSUIPCWinformsAutoCS (Prepar3D and FSX)](screenshots/fsuipc_bridge.png)

![MSFSWinformsAutoCS (MSFS 2020 and 2024)](screenshots/msfs_bridge.png)

Both bridges have one window with the same layout.

- **Avionics Master** check box: follows the simulator's avionics master switch and can also be used to switch it.
- **Readout box** (left): the values being sent to the boards, one per line (airspeed, altitude, bus voltages, engine and transmission values, radio frequencies and, in the MSFS bridge, the GPS position and heading). It is empty until the simulator is connected.
- **Annunciator list** (right): every warning lamp with its state, ON or OFF. The FSUIPC bridge lists the 21 lamp bits of the DodoSim aircraft; the MSFS bridge lists the 15 lamps of the panel.
- **Status bar** (bottom): *Disconnected. Looking for Flight Simulator...* in red until the simulator is found, then the connection and (MSFS) the aircraft name in green, or orange when the simulator is paused or loading and no data arrives.

Run only one bridge at a time.

![DCSBiosBridge (DCS World) with a synthetic UH-1H stream: three caution lights on, the common values, the mission clock with its time zone box and the Huey LED rows (scroll for the gauges)](screenshots/dcs_bridge.png)

![XPlaneBridge (X-Plane) against a fake X-Plane with the Robinson R22 profile (scroll for the Huey LEDs, rotor speed and bus voltage)](screenshots/xplane_bridge.png)

**DCSBiosBridge** and **XPlaneBridge** have a different window: a status line (the DCS-BIOS stream and aircraft, or the X-Plane address and whether data is arriving), a **Profile** box (Automatic, or an aircraft chosen by hand), **Reload profiles**, a **Send to the boards** check box, a table of every token with its source control or dataref, the raw value, the value sent and the board, and a log at the bottom. The DCS bridge also has **Suggest controls** (finds controls for a new profile), **Calibrate selected...** and **Clear its calibration** for the Huey needle gauges; the X-Plane bridge has **Search datarefs...**. See Running the DCS bridge and Running the X-Plane bridge.

## Health Monitor

![JetRangerHealthMonitor: all seven boards heard](screenshots/health_monitor.png)

One coloured label per board: Comm Nav, Servo, Joystick, Upper Input, Stepper, Dual Stepper and ADF (the combined ADF and Upper board lights both the Upper Input and ADF labels). Each turns green when that board's 10 second heartbeat arrives on UDP 13137. The **Hide Logs** button hides the list of received heartbeats (the sender address and time of each one) and shrinks the window; it then reads **Show Logs**.

The **Show Voltages** button opens a table with one row per board:

![Voltage table](screenshots/health_monitor_voltages.png)

Each board adds its 5 V rail to its heartbeat, and the ADF also adds its 12 V rail, so the table shows the last 5 V reading from every board, the **12 V** reading for the ADF only (the other rows say "not measured") and when the reading arrived. A reading outside its limits (5 V outside 4.75 to 5.25, 12 V outside 10.8 to 15.0) is orange, and a reading that is more than 35 seconds old, or that has not arrived yet, is grey. A board that has not been reflashed since the voltages were added shows "no reading" but still lights green. *Hide Voltages* closes the table.

![Logs hidden](screenshots/health_monitor_logs_hidden.png)

A board that goes quiet turns orange after 15 seconds and red after 30 seconds.

![Stepper and Dual Stepper quiet for 15 seconds: orange](screenshots/health_monitor_warning.png)

![Stepper and Dual Stepper quiet for 30 seconds: red](screenshots/health_monitor_lost.png)

## SwingTester

![SwingTester main window](screenshots/swing_tester.png)

- **Top row:** the IP addresses of the boards (Stepper, Dual Stepper, Servo, Radio, Upper, ADF and HID; the Upper address .103 is the retired board, the ADF and Upper board is .107).
- **Swing ALL (in order)** runs the 18 gauge swings in the fixed order, **Stop** cancels the remaining waits, **OLEDs ON / OFF** fill or restore the Dual Stepper's four OLEDs, **Reset boards...** opens the reset dialog, and **Light OLEDs during Swing ALL** controls the OLEDs. **Temp OLED** with **Send TEMP** sends text to the Dual Stepper's temperature display.
- **Lamps ON (in order)** and **Lamps OFF (in order)** with the **on gap** and **off gap** times, and **Light warning lamps during Swing ALL**.
- **Brightness (0-15)** slider with **Send to all boards** sends the backlight brightness to every board except the USB HID board (the Radio, Servo, Stepper, Dual Stepper and ADF and Upper, and the retired Upper address); **Backlight power ON / OFF** send `BPWR` to those boards to test the main bus rule.
- **Status line:** what was last sent and to which boards.
- **Panel LEDs group** (far right): one toggle for each of the 39 Auto Pilot, Audio and Huey Warning Panel LEDs (gold when on; the Huey LEDs are in two columns, *Huey Warning Panel* and *(continued)*), which lights that LED on its own through the Radio controller (`D,NAVCOM1:1,LED_xxx:1/0` or `LED_H_xxx`). **All ON** and **All OFF** do all 39, **Chase** lights them one at a time for the *on gap* time (click again to stop; a full chase of 39 takes about 39 seconds at the default), and **Board test (C,LEDTEST)** lights all 39 with the Radio's own test. The Radio on COM20 was flashed with the Huey build on 31 October 2026; the LEDs have not been checked on the panel yet.
- **Auto Pilot display group** (far right, below the panel LEDs): five boxes for HDG, VS, CRS, IAS and ALT (start-up values 090, 0, 110, 80 and 3200). **Send values** sends them to the Radio for the Auto Pilot panel's OLED and **Start-up values** puts the start-up values back.
- **ADF panel landing gear lamps** (under the warning lamps): **Nose**, **Left** and **Right** toggles (gold when on) light the three lamps on the ADF panel through the ADF controller (172.16.1.107, set in the `ADF (.107)` box at the top), with **All ON** and **All OFF**.
- **COM ports...** opens a window that lists the PC's COM ports with the board type, the USB serial number and whether another program has the port open, and can restart a USB device (see COM ports window below).
- **Gauge rows** (left): the 18 gauges with the board each is on, the wait in seconds and a **Swing** button for each. The row being swung is highlighted gold.
- **Warning lamps** (right): one button for each of the 15 lamps, in turn-off order. A lamp that is on is gold.

![The COM ports window](screenshots/swing_tester_ports.png)

**COM ports...** lists every COM port Windows knows: the port, Windows' description, the board type (guessed from the USB vendor and product ids), the USB serial number (genuine Arduino boards have one; most clones do not), whether another program has the port open (for example the Arduino IDE serial monitor, which blocks flashing), the status and the USB device id. It refreshes by itself when a device is plugged in or removed. **Copy list** puts the table on the clipboard. **Restart selected USB device** stops and starts the selected board's USB device with `pnputil` (the same as disabling and enabling it in Device Manager, so the port reappears; it cannot cut the power): Windows asks for administrator rights, and the port must not be open in another program.

![The Reset boards dialog](screenshots/swing_tester_reset.png)

The **Reset boards...** dialog has a tick box for each board (all ticked; untick any that are not connected). **Reset** sends `C,RESET:1` to each ticked board and **Cancel** closes the dialog without sending anything.

## StepperVSITester

The window scrolls; both halves are shown.

![StepperVSITester, top half](screenshots/stepper_vsi_tester_1.png)

![StepperVSITester, bottom half](screenshots/stepper_vsi_tester_2.png)

The target is shown at the top (the Stepper board, 172.16.1.105 port 13136). Each gauge has a slider, a text box, **Send** and **Zero**:

- **VSI (fpm)**, **ALT (ft)**, **Radar ALT (ft)**: real values for the vertical speed, altimeter and radar altimeter.
- **ALT Direct Step Jog** sends a signed step count at a fixed interval, bypassing the acceleration ramp.
- **Raw Step Test** sends raw steps to the chosen gauge code (with **-1 Step** and **+1 Step** nudge buttons).
- **Temperature OLED** sends text to the Dual Stepper's temperature display, and the **ITT / EGT** slider below it sends turbine out temperature.
- **Real-Value Gauges** rows for engine oil temperature and pressure, transmission oil temperature and pressure, gas producer and fuel.
- **IAS**, **RPME** and **RPMR** sliders for airspeed, engine speed and rotor speed.
- **Dual Stepper test** rows (fuel load, electrical load, the clock), **Single Stepper test** (torque) and **Servo Controller test** (pitch and bank, in degrees or as raw servo positions).

## Emulators

![JetRangerRadioControllerEmulator](screenshots/radio_emulator.png)

The **Radio controller emulator** stands in for the radio board. Set the reflector (bridge) address and port at the top. The COMM and NAV displays show the active and standby frequencies; each radio has a **MHz** and a **kHz** knob (click the right half to turn it up one detent, the left half to turn it down) and the COM radios have an **XFER** swap button. **MASTER ON** and **MASTER OFF** send the battery and alternator commands. Every control sends its packet to the bridge on UDP 27001 at once.

![JetRangerUpperControllerEmulator](screenshots/upper_emulator.png)

The **Upper controller emulator** stands in for the upper panel (now on the combined ADF and Upper board; the switch numbers are unchanged). Set the USB HID board's address and port at the top (172.16.1.11, 4210). There is one toggle button per switch the firmware uses, in groups: Cabin Heat, Air, Circuit Breakers, Instruments, Electrical and Lights. A button turns green when its switch is on, and each press sends the switch change to the USB HID board.

## GPS interface

![LowranceAirMapGps](screenshots/gps_interface.png)

The window shows, from the top: the serial port, its speed and state and how many updates have been sent; the UDP port and the number of packets accepted; the **source** of the position (green *aircraft data over UDP*, orange *default - Brisbane Airport*); the latitude, longitude, altitude, airspeed and heading being sent; the **GPS time** and whether it comes from the simulator or the PC clock; the **Change serial port...** button; and the sentences being sent to the GPS.

![The serial port dialog](screenshots/gps_serial_dialog.png)

The first time the program runs, or when the saved port is no longer present, this dialog asks which port the GPS is connected to. **Refresh** rereads the list of ports, **OK** saves the choice and **Cancel** leaves the port unselected (the program still runs, without sending to a GPS). **Change serial port...** opens the same dialog later.

## GPS bench tester

![LowranceAirMapGpsBenchTester](screenshots/gps_bench_tester.png)

- **Send to UDP:** the address and port of the GPS interface (the same PC and port 27002 by default) and a check box to turn the sending on and off.
- **Country, City and Airport:** choose an airport to put the aircraft on the ground there.
- **Latitude, Longitude, Altitude, Speed and Heading:** type a value and press Enter (a value that cannot be read turns the box pink).
- **Date / time (UTC):** the date and time the GPS should show, **PC clock** to use the PC's, and **Send date/time** to include it.
- **Back to the selected airport** returns to the airport chosen above.
- **Eight-way cursor** (right): moves the position by the **Cursor step** in metres; the arrow keys work too. **Point heading in direction of travel** sets the heading to the direction moved.
- **Fly** moves the aircraft along its heading at its speed; **Time x** speeds it up.
- **Bottom:** the number of packets sent and the packet being sent.
