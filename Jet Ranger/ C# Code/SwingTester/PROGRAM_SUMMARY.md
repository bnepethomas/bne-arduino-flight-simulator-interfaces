# SwingTester — Program Summary

WinForms (.NET 10, `net10.0-windows`) bench tool that triggers each gauge's
startup swing on the three Jet Ranger boards over UDP, either one gauge at a
time or all of them in a fixed order. Build output:
`bin\Release\net10.0-windows\SwingTester.exe`
(`dotnet build -c Release` in this folder).

## How it works

Sends a UDP **control** packet `C,<COMMAND>:1` to `<board IP>:13136` — the
`SWING<CODE>` commands each board's sketch handles in `HandleControlString()`
(see the "On-demand swing commands" section of each sketch's
`PROGRAM_SUMMARY.md`). The boards don't report completion over this link, so
after sending a command the app **waits a per-row number of seconds** before
sending the next one in a sequence. Those seconds are estimates (editable per
row in the window, not saved between runs); a swing blocks the board's loop
while it runs, so too short a wait lets the next command queue up in the
board's UDP buffer and the order across *different* boards drifts.

## Window

- Top: the three board IPs (defaults `172.16.1.105` Stepper, `.106` Dual
  Stepper, `.102` Servo).
- **Swing ALL (in order)**, **Stop** (cancels the remaining waits; a swing
  already started on a board still finishes), **OLEDs ON** / **OLEDs OFF**,
  and a checkbox **Light OLEDs during Swing ALL** (on by default).
- One row per gauge with its board, a seconds box and an individual
  **Swing** button. The row being swung is highlighted gold.
- Layout: the 18 gauge rows are on the left and the 15 warning-lamp buttons on
  the right, side by side, so everything is visible at the default window size
  (900 px wide, up to 900 px tall). Nothing in the scrolling panel is docked,
  so if the window is made smaller the panel scrolls (vertical and horizontal)
  and every row and lamp stays reachable. (An earlier layout used docked
  controls inside the scroll panel, which left the lower rows unreachable.)

## Sequence (Swing ALL)

| # | Gauge | Board | Command | Default wait |
|---|---|---|---|---|
| 1 | Engine Oil Pressure | Dual .106 | `SWINGOILP` | 6s |
| 2 | Engine Oil Temp | Dual .106 | `SWINGOILT` | 6s |
| 3 | Transmission Pressure | Dual .106 | `SWINGXMSNP` | 6s |
| 4 | Transmission Temp | Dual .106 | `SWINGXMSNT` | 6s |
| 5 | Fuel Available | Stepper .105 | `SWINGFUEL` | 11s |
| 6 | Electrical Load | Dual .106 | `SWINGELECTRICALLOAD` | 7s |
| 7 | Fuel Load | Dual .106 | `SWINGFUELLOAD` | 7s |
| 8 | Torque | Stepper .105 | `SWINGTQ` | 11s |
| 9 | Turbine Out (EGT/ITT) | Stepper .105 | `SWINGITT` | 11s |
| 10 | Gas Producer | Stepper .105 | `SWINGN1` | 11s |
| 11 | Pitch | Servo .102 | `SWINGPITCH` | 3s |
| 12 | Roll | Servo .102 | `SWINGBANK` | 3s |
| 13 | Turbine RPM | Dual .106 | `SWINGRPME` | 7s |
| 14 | Rotor RPM | Dual .106 | `SWINGRPMR` | 7s |
| 15 | VSI | Stepper .105 | `SWINGVSI` | 11s |
| 16 | Airspeed | Stepper .105 | `SWINGIAS` | 11s |
| 17 | Radar Alt | Stepper .105 | `SWINGAGL` | 11s |
| 18 | Altimeter | Dual .106 | `SWINGALT` | 6s |

"Pitch Roll" is two rows (Pitch then Roll) so each also has its own button.
The default waits are the swing times measured on the real boards on
2026-10-11 (from each board's "End ..." debug line over UDP) plus about 1s of
margin: Dual Stepper 5.0-5.2s (oil/transmission gauges, Altimeter), 6.4-6.5s
(Electrical Load, Fuel Load, Turbine/Rotor RPM); single-board Stepper 9.5-10.7s
(Fuel Available, Torque, Turbine Out, Gas Producer, VSI, Airspeed, Radar Alt);
Servo about 2.1s for Pitch. Airspeed and Radar Alt were re-timed at 10.0s each (down from
10.7s) after their homing was changed to return to 0 instead of -635 steps
(2026-10-11). A full pass took about 220s.

## OLEDs

**OLEDs ON** sends `C,OLEDTEST:1` to the Dual Stepper: its Baro, Altimeter
and Clock OLEDs are filled solid white. **OLEDs OFF** sends `C,OLEDTEST:0`,
which redraws their default content (2992 / 0 / 00:00) and clears the
last-value gates so live data repaints them. With the checkbox on, Swing ALL
sends ON before the first swing and OFF afterwards (also after Stop or an
error). The command is ignored by the board (debug line
`OLEDTEST ignored - OLEDs not initialised`) if its TCA mux wasn't found at boot.

## Reset boards

The **Reset boards...** button opens a dialog with a tick box per board - Radio
(.101), Servo (.102), Upper (.103), Stepper (.105), Dual Stepper (.106) and the
USB HID board (.11), all ticked by default (untick any that are not connected) -
and **Reset** sends `C,RESET:1` to each ticked board (port 13136; the HID board
listens on 4210). The Radio, Upper and HID addresses are extra boxes at the
top. Each board reboots about 2 s later through its watchdog and runs its
start-up again (the Dual Stepper takes about a minute; the HID board briefly
drops its USB joysticks and keyboard). The status line lists the boards the
command was sent to. The button is disabled while a run is active.
**Tested 2026-10-13** by driving the real window with UI automation (Reset
dialog, Radio and Upper unticked): Servo, Stepper, Dual Stepper and HID all
acknowledged and rebooted cleanly.

## Temperature OLED

The Dual Stepper now has a fourth OLED (mux channel 4) that shows a
temperature string. **OLEDs ON/OFF** and Swing ALL's OLED lighting include it
(the firmware's `OLEDTEST` fills/restores all four displays). The **Temp
OLED** box (default `21C`, max 8 characters) with **Send TEMP** sends
`D,TEMP:<text>` to the Dual Stepper (.106).

## Warning lamps

The Servo board's 15 warning lamps are driven by ordinary data packets
`D,<code>:1` / `D,<code>:0` to `172.16.1.102:13136`. The window has a
toggle button per lamp (highlighted gold when on), **Lamps ON (in order)**
(one lamp at a time, **on gap** default 1000 ms per lamp), **Lamps OFF (in
order)** (**off gap** default 400 ms) and the
checkbox **Light warning lamps during Swing ALL** (on by default).

Turn-off order (also the button order):

| # | Lamp | Code |
|---|---|---|
| 1 | SC Fail | `SCF` |
| 2 | Generator Fail | `GENF` |
| 3 | Aft Fuel Filter | `FFLTR` |
| 4 | Baggage Door | `BD` |
| 5 | Engine Chip | `EC` |
| 6 | Battery Hot | `BHOT` |
| 7 | Transmission Oil Press | `TOPW` |
| 8 | Engine Out | `EOUT` |
| 9 | Low Fuel | `LOWF` |
| 10 | Fuel Pump | `FPMP` |
| 11 | Tail Rotor Chip | `TRC` |
| 12 | Transmission Chip | `TC` |
| 13 | Battery Temp | `BTMP` |
| 14 | Transmission Oil Temp | `TOTW` |
| 15 | Rotor Low RPM | `RLOW` |

**Swing ALL** with the checkbox on first lights the 15 lamps one at a time in
the order above (1s per lamp by default, so about 15s), then runs the
swings, then turns the lamps off one at a time in the order above (always at
the end of Swing ALL, also after Stop or an error, whether or not the
checkbox is on). The Servo board only drives a lamp when the incoming value
differs from the state it last remembered, so the toggles and Swing ALL stay
consistent as long as nothing else (e.g. the sim bridge) is sending lamp data
at the same time.

## Keepalive

Every sketch resets its gauges after 30s with no UDP packet (the Dual Stepper
also blanks its OLEDs, the Servo board also switches its lamps off, and the
single-board Stepper zeroes its gauges). During a full run some boards get no
command for 100s+ (e.g. the Servo board between the lamp-on packets and
Pitch), which would have put the lamps and the lit OLEDs out mid-test. While
any run is active (Swing ALL, an individual Swing, or the Lamps ON sequence)
the app therefore sends `C,KEEPALIVE:1` to all three boards every 10s (and once
at the start). All three sketches ignore the command name but still count the
packet as received data. The keepalive stops when the run ends, so lamps or
OLEDs left on manually will still time out after 30s.

## Brightness

A **Brightness (0-15)** slider sends `C,B:<level>` to the Radio (.101), Servo (.102), Upper (.103), Stepper (.105) and Dual Stepper
(.106) - every board except the USB HID board - each time the slider is moved, and **Send to all boards** resends the current level.
The status line lists the boards sent to. The Radio, Servo, Stepper and Dual Stepper set their backlight PWM from it (0 off, 15 full); the
Radio also sets the intensity of its panel LEDs from it (0 off, 1 to 15), and the Upper controller ignores it. The board IP boxes at the top are used.

**Backlight power ON / OFF** send `D,BPWR:1` / `D,BPWR:0` to the Radio, Servo, Stepper and Dual Stepper, the packet the bridges send from the
main bus voltage (1 at 20 V or more): the backlight goes off at 0 and, on 1, returns to the last brightness commanded. These are for
testing the main bus rule from the bench.

Tested with the IP boxes set to this PC and a scratch UDP listener: Send to all boards delivered `C,B:8` five times, the power
buttons delivered `D,BPWR:0` and `D,BPWR:1` three times each, and moving the slider delivered `C,B:9` to the five boards.

## Panel LEDs (Radio .101)

The **Panel LEDs (Radio .101, MAX7219)** group on the right has one toggle button for each of the 19 LEDs on the Auto Pilot panel (CRS, HDG, IAS,
ALT, APPR, FD, BC, AP ENGAGE) and the Audio panel (COM1, BOTH, NAV2, DME, OUTER, INNER, COM2, NAV1, MKR, ADF, MIDDLE). A button lights or clears
that one LED by sending `D,NAVCOM1:1,LED_<name>:1/0` to the Radio controller (`NAVCOM1:1` keeps the radio's power flag on, because the LEDs follow it;
the radio remembers every LED's state, so only the changed one is sent). The button is gold while its LED is on. Other buttons:

- **All ON / All OFF**: all 19 LEDs in one packet.
- **Chase**: lights each LED on its own, in table order, for the *on gap* time (default 1 s), then clears them all. Click again (**Stop chase**) to stop.
- **Board test (C,LEDTEST)**: toggles `C,LEDTEST:1` / `C,LEDTEST:0`, the Radio's own test that lights all 19 LEDs at the MAX7219 regardless of state.

The brightness slider also reaches the LEDs: the Radio sets the MAX7219 intensity from `C,B`. The window is wider (1130 px) to fit the extra group.
Built clean (Release, 0 errors) and the window checked on screen; it has not been run against the Radio board.

## Requirements / status

- Boards must be running sketches that include the swing commands (and
  `OLEDTEST` on the Dual Stepper) — the builds from 2026-10-11 onwards.
- Built clean (Release, 0 warnings) and launches. The same packet sequence
  (swings, `OLEDTEST`, lamps) was run against the real boards on 2026-10-11 with
  a script, and every swing completed; the SwingTester window itself was not
  clicked through.

## Auto Pilot display and ADF landing gear lamps (2026-10-25)

- **Auto Pilot display** group (far right, under the panel LEDs): five boxes (HDG, VS, CRS, IAS, ALT; start-up values 090, 0, 110, 80, 3200) and **Send values** / **Start-up values**, which send
  `D,NAVCOM1:1,APHDG:..,APVS:..,APCRS:..,APIAS:..,APALT:..` to the Radio for the Auto Pilot panel's OLED. Each value is 1 to 5 characters without commas or colons.
- **ADF panel landing gear lamps** group (under the warning lamps): **Nose**, **Left**, **Right** toggles (gold when on) and **All ON / All OFF**, sent as `D,LDGN:1,LDGL:0,LDGR:0` to the ADF controller; a new `ADF (.107)` address
  box at the top (172.16.1.107). The window is 1130 px wide and up to 980 px high. Verified on the bench: the three lamps toggled.
