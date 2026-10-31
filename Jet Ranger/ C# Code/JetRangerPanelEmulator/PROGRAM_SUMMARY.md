# JetRangerPanelEmulator - Program Summary

Windows Forms program (.NET 10) that stands in for the Jet Ranger **controller boards and their displays**: one window each for the Radio, Servo, Stepper, Dual Stepper and
ADF and Upper controllers. Each window receives the same UDP packets as the board (`D,TOKEN:value,...` data and `C,COMMAND:value` control, port 13136) and draws what the panel
would show: the dial gauges with their needles, the attitude indicator, the caution lamps, the LEDs, the character OLEDs and the landing gear lamps. The four bridges
(`FSUIPCWinformsAutoCS`, `MSFSWinformsAutoCS`, `DCSBiosBridge`, `XPlaneBridge`) can send their output to the emulators instead of the boards or as well as them, so the whole
chain can be tried with no cockpit hardware.

The older `JetRangerRadioControllerEmulator` and `JetRangerUpperControllerEmulator` are different: they stand in for a board's **inputs** (knobs, keys) and send commands to the
bridge. This program shows a board's **outputs**; it sends nothing.

**Status (1 November 2026):** builds clean (Release, 0 warnings); `tests\test_emulator.py` (29 checks) and `tests\test_bridges_to_emulators.py` (9 checks, the real DCS and
X-Plane bridges against the emulators) pass. Not yet checked against a cockpit side by side with the real boards, so the needle speeds and the colours are a close drawing, not a measurement.

## How the bridges reach the emulators

Each emulator listens on the board's own port (UDP 13136) on its own **loopback address with the board's last number**:

| Board | Real address | Emulator address |
|---|---|---|
| Radio controller | 172.16.1.101 | 127.0.0.101 |
| Servo controller | 172.16.1.102 | 127.0.0.102 |
| Stepper controller | 172.16.1.105 | 127.0.0.105 |
| Dual Stepper controller | 172.16.1.106 | 127.0.0.106 |
| ADF and Upper controller | 172.16.1.107 | 127.0.0.107 |

Every address from 127.0.0.1 to 127.255.255.254 is loopback on Windows, so a bridge only has to swap `172.16.1` for `127.0.0` and send the very same packets. No packet
needs sorting by its contents, and the emulators run on the bridge's PC. `--net <a.b.c>` (emulator) and `--emulator-net <a.b.c>` (bridge) change the first three numbers.

In every bridge a `BoardTargets` class holds, for each board's UDP client, a second client for its emulator. The window has two check boxes, **Send to the boards** (on by
default) and **Send to the emulators** (off), and the command line sets them: `--emulators` (emulators as well as the boards), `--emulators-only` (no packet goes to a board) and
`--emulator-net`. The GPS interface is not a board and is unaffected. The DCS and X-Plane bridges' `--no-send` still stops the boards.

## What each window does

- **Common to all.** The board's UDP address in the title and status bar, the packet count and the last packet; `BPWR:0` or no data switches the **backlight** off (the scale and the
  lamp legends go dark, the needles stay visible); `C,B:0-15` sets its brightness (default 0.7 until one arrives); `C,RESET:1` forgets everything; after **30 seconds without data** the
  board blanks, the needles return to their calibrated zero and the lamps and OLEDs go out (the same watchdog as the real boards; `--watchdog <seconds>` shortens it for tests).
  The needles move at a limited speed, like a stepper.
- **Radio .101.** The COM and NAV displays (two radios per display, `xxx.xxx`), the Auto Pilot display (HDG, CRS, IAS, VS and ALT in their windows, 16 columns by 2 rows), the DME
  display, the 8 Auto Pilot and 11 Audio LEDs and the 20 Huey Warning Panel lamps. `NAVCOM1:0` blanks the displays and puts the LEDs out; `C,LEDTEST:1` lights all 39.
  The PC sends only COM1 and COM2, so the NAV displays keep their start-up value (108.000).
- **Servo .102.** The attitude indicator (pitch -30 to +30, bank -90 to +90, the servos move at a limited speed) and the 15 caution lamps; `C,SWINGPITCH:1` and `C,SWINGBANK:1`.
- **Stepper .105.** Airspeed, vertical speed, radar altitude, torque, gas producer, turbine out temperature and fuel quantity on their dials; `C,SWING<token>:1` sweeps a needle.
- **Dual Stepper .106.** Engine and transmission oil pressure and temperature, rotor and engine speed, electrical load, fuel pressure, the altimeter (one turn per 1000 ft) and the
  four OLEDs (barometer `2992`, altitude, zulu clock from `ZULU:HHMM`, temperature text). `PWR:0` blanks the OLEDs; `C,OLEDTEST:1` fills them white.
- **ADF and Upper .107.** The three landing gear lamps (`LDGN`, `LDGL`, `LDGR`), the OLED (`ADF  340 kHz`, `FOLLOW OFF`; `C,DISPLAY:0` blanks it as the Radio does) and the backlight.
  The Upper switch matrix and the keypads are inputs and are not shown.

The faces (ranges, scales, numerals, legends) are those of the instrument map, Appendix G of the master document.

## Running it

`JetRangerPanelEmulator.exe` opens a small launcher with a button for each emulator and **Open all five**. `--board radio|servo|stepper|dual|adf|all` opens those windows directly;
`--dump <folder>` writes each board's state to `<folder>\<board>.json` four times a second (used by the tests). The start-up script (`Start Jet Ranger.bat`) opens them with
`-Emulators` (and sends to them as well as the boards) or `-EmulatorsOnly`.

## Files

| File | Role |
|---|---|
| `Program.cs`, `LauncherForm.cs` | Options, the launcher and the window set |
| `PanelLink.cs` | The UDP listener on `<net>.<board number>:13136` and the packet parser |
| `BoardWindow.cs` | Window base class: backlight, brightness, reset, watchdog, status bar, state dump; `Gauge` (needle speed, swing) |
| `GaugeWindows.cs`, `ServoWindow.cs`, `RadioWindow.cs`, `AdfWindow.cs` | The five emulators (Stepper and Dual Stepper share `GaugeBoardWindow`) |
| `Gfx.cs`, `Specs.cs` | Drawing (dial, attitude indicator, lamp, LED, OLED) and the gauge faces and lamp legends |
| `tests\test_emulator.py`, `tests\test_bridges_to_emulators.py` | The tests |

## Tests

`python tests\test_emulator.py` starts the five emulators with a 3 second watchdog and a dump folder and sends them the bridges' packets: values, needles reaching them, swings, lamps,
blanking (`PWR:0`, `NAVCOM1:0`, `C,DISPLAY:0`), brightness and `BPWR`, `C,LEDTEST:1`, `C,RESET:1` and the watchdog. `python tests\test_bridges_to_emulators.py` runs the real DCS bridge
(with its synthetic DCS-BIOS stream) and then the real X-Plane bridge (against its fake X-Plane) with `--emulators-only` and checks what the emulators show. The MSFS and FSUIPC bridges
need their simulators for data: they were built and their window and command line were checked, and they use the same `BoardTargets` code, but no data has gone through them to the emulators.

## Not done yet

Input emulation on these windows (use the older Radio and Upper emulators for that), heartbeats to the Health Monitor (the emulators do not appear there), start-up swings and the
lamp test of the real boards, the NAV frequency knobs, and a check against the real boards side by side.
