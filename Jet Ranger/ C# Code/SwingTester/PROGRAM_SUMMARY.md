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

## Requirements / status

- Boards must be running sketches that include the swing commands (and
  `OLEDTEST` on the Dual Stepper) — the builds from 2026-10-11 onwards.
- Built clean (Release, 0 warnings) and launches. The same packet sequence
  (swings, `OLEDTEST`, lamps) was run against the real boards on 2026-10-11 with
  a script, and every swing completed; the SwingTester window itself was not
  clicked through.
