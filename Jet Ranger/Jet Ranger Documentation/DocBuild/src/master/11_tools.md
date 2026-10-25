# Health Monitor (JetRangerHealthMonitor)

A small Windows Forms dashboard (namespace `MegaHealthMonitor`) that watches for the 10 second heartbeat strings sent by the networked Arduino boards and shows one traffic light per board: Comm/Nav, Servo, Joystick, Upper Input, Stepper, Dual Stepper and ADF. A **Show Voltages** button adds a table of the last voltage reading from each board.

![Heartbeats in, one light per board out](diagrams/health_monitor.svg)

## Program flow

1. `Form1_Load` starts a background task that binds a `UdpClient` to port **13137** and loops on `ReceiveAsync()`.
2. Each packet is matched by **prefix** (not by sender address): `COMM_NAV`, `SERVO`, `JOYSTICK`, `UPPER_INPUT`, `STEPPER`, `DUAL_STEPPER` or `ADF_PANEL`. A match updates that board's last-received time and turns its label green; each match can also be logged (sender and timestamp) to a list box.
3. A timer calls `CheckHealth()` about once a second: 30 seconds or more since the last packet turns the indicator **red** (connection lost), 15 seconds or more **orange** (connection warning), otherwise it stays green.
4. *Toggle logs* shows or hides the log list box and resizes the form.
5. **Voltages.** Every board appends its 5 V rail to its heartbeat after its name (`SERVO V5:5.02`) and the ADF controller also its 12 V rail (`ADF_PANEL V5:5.03 V12:12.41`). `RecordVoltages()` keeps the last reading and its time for each board, and the *Show Voltages* button shows a table with one row per board: the last 5 V reading, the 12 V reading (only the ADF measures it; the other rows say "not measured") and the time of the last reading with "N s ago". A reading outside its limits (5 V: 4.75 to 5.25, 12 V: 10.8 to 15.0) is orange, and a reading older than 35 seconds, or none yet, is grey. A board that has not been reflashed since the voltage was added shows "no reading". `ApplyLayout()` sizes the window for whichever of the log list and the voltage table are showing.

The monitor is a passive listener: it never sends anything and does not care which bridge is running. The Dual Stepper's prefix is deliberately distinct from `STEPPER` (the monitor uses `StartsWith`, so overlapping prefixes would light the wrong indicator). The monitor finds the voltages with a pattern (`V5:` and `V12:` followed by a number), so a heartbeat without them still lights the board's indicator.

# Radio and Upper controller emulators

Two small Windows Forms panels stand in for the physical boards when they are not connected. Each shows only the controls that actually do something in the firmware.

![The emulators and where they send](diagrams/emulators.svg)

## JetRangerRadioControllerEmulator

Shows the two character-OLED displays (COM and NAV), a clickable knob for each of the eight encoders (click the right half to turn one detent clockwise, the left half for counter-clockwise), the COM1 and COM2 *XFER* swap keys and the *MASTER ON* and *MASTER OFF* keys. It sends the same payloads the board would send to the bridge on UDP 27001 (default host `172.16.1.10`):

- each knob sends the new standby frequency as a bare ASCII number such as `121.010`;
- the XFER keys swap the local active and standby values and send `COM1_RADIO_SWAP`, or `COM2_RADIO_SWAP` followed by `AVIONICS_MASTER_SET`;
- the MASTER keys send `MASTER_BATTERY_ON` with `ALTERNATOR_ON`, or the two `_OFF` commands.

Limits: the emulator still sends a bare frequency for every radio, so only COM1 tunes correctly from it (the real board sends `COM2_STANDBY:`, `NAV1_STANDBY:` and `NAV2_STANDBY:`). It has no NAV swap keys and sends no debug (27000) or heartbeat (13137) traffic. Its knob model is 5 kHz per minor detent, not the 25 kHz and 50 kHz channel spacing the real board now uses.

## JetRangerUpperControllerEmulator

Shows one toggle button per switch the Upper firmware acts on (22 switches in groups: Cabin Heat, Air, Circuit Breakers, Instruments, Electrical, Lights; green means on) and sends `index,state` packets to the USB HID board (default `172.16.1.11:4210`) exactly as the real board would. Indexes 7 to 10, 18 to 21, 29 to 38 and 40 to 175 are wired in the matrix but drive nothing and are not shown.

# StepperVSITester

A Windows Forms bench tool that sends real-unit values or raw step counts straight to the boards' UDP handlers without a simulator. It started as a vertical speed tester (hence the name) and now covers the whole Stepper and Dual Stepper boards plus the servo pitch and bank.

## What it sends

| Target | Controls |
|---|---|
| Stepper .105 | `VSI`, `AGL`, `ITT`, `IAS`, `TQ`, `N1`, `FUEL` (real values, with sliders for VSI, AGL, ITT, IAS); `OILT`, `OILP`, `XMSNT`, `XMSNP` (also broadcast to .106) |
| Dual Stepper .106 | `ALT`, `RPME`, `RPMR` (broadcast to both stepper boards), `FUELLOAD`, `ELECTRICALLOAD`, `ZULU` (hour and minute boxes), `TEMP` (text) |
| Servo .102 | `PITCH`, `BANK` (degrees, one decimal) and `PITCHRAW`, `BANKRAW` |
| Raw step panel | A drop-down of raw codes (`IASRAW`, `VSIRAW`, `ALTRAW`, `OILTRAW`, `OILPRAW`, `XMSNTRAW`, `XMSNPRAW`, `ITTRAW`, `RPMERAW`, `RPMRRAW`, `N1RAW`, `FUELRAW`, `TQRAW`, `FUELLOADRAW`, `ELECTRICALLOADRAW`) sent to both stepper boards, with -1 step and +1 step buttons |

Each row has a slider or text box, *Send* (or Enter) and *Zero*. Values are sent as typed, with no conversion; `RPME` and `RPMR` carry one decimal. Raw codes that a board does not have are silent no-ops there (for example `FUELLOADRAW` on the Stepper board).

Notes: the raw panel still lists `FLAPS`, `AOA`, `GFORCE`, `SPDMAX` and `N1RAW`; the first four no longer exist on any board, and `N1RAW` does nothing on the Dual Stepper. `RPME` and `RPMR` are no longer handled by the Stepper board, so only the Dual Stepper acts on them. Run it **instead of** a bridge, since both would otherwise fight over what drives the gauges.

# SwingTester

A Windows Forms (.NET 10) bench tool that triggers each gauge's start-up swing over UDP, one gauge at a time or all of them in a fixed order. It also lights the warning lamps and, in a **Panel LEDs** group, each of the 19 Auto Pilot and Audio panel LEDs on its own (a toggle per LED sending `D,NAVCOM1:1,LED_xxx:1/0` to the Radio controller), all on or off, in a chase (one at a time, with the on gap), or with the Radio's own `C,LEDTEST` board test. The **Auto Pilot display** group sends the five values (HDG, VS, CRS, IAS, ALT) to the Radio for the Auto Pilot panel's OLED (`D,NAVCOM1:1,APHDG:090,APVS:0,APCRS:110,APIAS:80,APALT:3200`), with a *Start-up values* button, and the **ADF panel landing gear lamps** group (nose, left, right, All ON and All OFF) lights the lamps on the ADF controller (`D,LDGN:1,LDGL:0,LDGR:0`); an `ADF (.107)` box holds the ADF controller's address. Build with `dotnet build -c Release` in its folder; the executable is `bin\Release\net10.0-windows\SwingTester.exe`.

## How it works

It sends a control packet `C,<COMMAND>:1` to `<board IP>:13136`. The boards do not report completion, so after each command the app waits a per-row number of seconds (editable in the window, not saved between runs) before sending the next. A swing blocks the board's loop; too short a wait lets the next command queue in the board's UDP buffer and the order across different boards drifts.

![Swing ALL sequence](diagrams/swing_sequence.svg)

The default waits are the swing times measured on the real boards plus about one second of margin: Dual Stepper 5.0 to 5.2 s (oil and transmission gauges, altimeter) and 6.4 to 6.5 s (load and RPM gauges); Stepper 9.5 to 10.7 s; Servo about 2.1 s. A full pass takes about 220 seconds.

## Window

- The three board IPs at the top (defaults `172.16.1.105`, `.106`, `.102`).
- **Swing ALL (in order)**, **Stop** (cancels the remaining waits; a swing already started still finishes), **OLEDs ON** and **OLEDs OFF**, and the check box *Light OLEDs during Swing ALL* (on by default).
- One row per gauge (18 rows on the left) with its board, a seconds box and a **Swing** button; the running row is highlighted gold. The 15 warning-lamp buttons are on the right.
- **OLEDs ON** sends `C,OLEDTEST:1` to the Dual Stepper and **OLEDs OFF** sends `C,OLEDTEST:0`. The **Temp OLED** box (default `21C`) with **Send TEMP** sends `D,TEMP:<text>`.
- **Reset boards...** opens a dialog with a tick box per board (Radio .101, Servo .102, Upper .103, Stepper .105, Dual Stepper .106, USB HID .11; all ticked by default) and sends `C,RESET:1` to each ticked board (the HID board on port 4210). The Dual Stepper takes about a minute to restart and the HID board briefly drops its USB devices. The button is disabled while a run is active.

## COM ports window

The **COM ports...** button (next to *Reset boards...*) opens a window that lists the PC's serial ports (`PortsForm`): the port number, Windows' description, the board type guessed from the USB vendor and product ids (Arduino Mega 2560, Leonardo, Uno, Due, CH340 clone, FTDI, CP210x), the **USB serial number** (genuine Arduino boards have one; a clone usually has none, and the window then says so instead of showing Windows' made-up id), whether **another program has the port open** (for example the Arduino IDE serial monitor, which blocks flashing), the device status and the USB device id. The list comes from WMI (`Win32_PnPEntity`) and the device tree (`CM_Get_Parent` finds the USB device behind a composite function) and refreshes by itself when a device is plugged in or removed. **Refresh** rereads it and **Copy list** puts it on the clipboard as tab-separated text.

**Restart selected USB device** runs `pnputil /restart-device` on the selected board's USB device after a confirmation. Windows asks for administrator rights. This stops and starts the device's driver (the same as disabling and enabling it in Device Manager) so the COM port reappears; it cannot cut the power to the port. A board that appears as several devices (the Leonardo) is restarted as a whole. If the administrator prompt is declined or `pnputil` fails the window says so. The window needs the port closed in other programs first.

## Brightness

A **Brightness (0-15)** slider sends `C,B:<level>` to the Radio (.101), Servo (.102), Upper (.103), Stepper (.105), Dual Stepper (.106) and ADF (.107), every board except the USB HID board, each time it is moved; **Send to all boards** resends the current level, and the status line lists the boards sent to. The Radio, Servo, Upper, Stepper, Dual Stepper and ADF controller set their backlight from it (the Radio also sets its panel LEDs' intensity). **Backlight power ON** and **OFF** send `D,BPWR:1` and `D,BPWR:0` to the Radio, Servo, Stepper, Dual Stepper, Upper and ADF controller, the packet the bridges send from the main bus voltage, to test the main bus rule from the bench: OFF switches the backlight off and ON returns it to the last brightness commanded.

## Warning lamps

The Servo board's lamps are driven by ordinary `D,<code>:1` and `D,<code>:0` packets to `172.16.1.102:13136`. There is a toggle per lamp (gold when on), **Lamps ON (in order)** (one lamp at a time, *on gap* default 1000 ms), **Lamps OFF (in order)** (*off gap* default 400 ms) and the check box *Light warning lamps during Swing ALL* (on by default). Turn-off order, also the button order: `SCF`, `GENF`, `FFLTR`, `BD`, `EC`, `BHOT`, `TOPW`, `EOUT`, `LOWF`, `FPMP`, `TRC`, `TC`, `BTMP`, `TOTW`, `RLOW`.

Swing ALL with the check box on first lights the 15 lamps one at a time (about 15 seconds), then runs the swings, then turns the lamps off one at a time (always at the end, also after Stop or an error). The Servo board only drives a lamp when the incoming value differs from the state it last remembered, so keep the bridge off while testing.

## Keep-alive

Every sketch resets its gauges after 30 seconds with no packet. During a full run some boards get no command for 100 seconds or more, so while any run is active (Swing ALL, an individual swing or the lamp sequence) the app sends `C,KEEPALIVE:1` to all three boards every 10 seconds (and once at the start). The boards ignore the command name but count the packet as received data. The keep-alive stops when the run ends, so lamps or OLEDs left on manually still time out after 30 seconds.

# Debug listener (UDP reflector)

Every board sends debug text to `172.16.1.10:27000`. A small Python reflector on the primary PC prints each line and can forward a copy to a second listener, so an analysis script can read the same stream. Only one program can own the port, so use the reflector's forwarding rather than starting a second listener. Keep the reflector running while boards are being flashed and restarted: the start-up lines (Ethernet Started, I2C scan, Setup Complete) are the quickest check that a flash worked.

Other helpers in `Jet Ranger Python Code/`: `Export_From_SimConnect.py` (a SimConnect exporter), `Test_Receiver.py` (a generic UDP JSON receiver) and `send_wol.py` (wake-on-LAN).
