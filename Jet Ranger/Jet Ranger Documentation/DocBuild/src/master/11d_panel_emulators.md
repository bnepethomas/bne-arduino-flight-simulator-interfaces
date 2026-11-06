# Panel emulators (JetRangerPanelEmulator)

`JetRangerPanelEmulator` is a Windows Forms program (.NET 10) that stands in for the **controller boards and their displays**: one window each for the Radio, Servo, Stepper, Dual Stepper and ADF and Upper controllers. Each window receives the packets its board would receive (UDP 13136, `D,TOKEN:value,...` data and `C,COMMAND:value` control) and draws what the panel would show: the dial gauges and their needles, the attitude indicator, the caution lamps, the LEDs, the character OLEDs and the landing gear lamps. The four bridges can send their output to the emulators instead of the boards, or as well as them, so the whole chain from simulator to panel can be tried with no cockpit hardware.

It is different from the older `JetRangerRadioControllerEmulator` and `JetRangerUpperControllerEmulator`: those stand in for a board's **inputs** (knobs, keys) and send commands to the bridge. This program shows a board's **outputs** and sends nothing.

![The launcher](screenshots/emulator_launcher.png)

## How the bridges reach the emulators

Each emulator listens on the board's own port, 13136, on a **loopback address with the board's last number**. Every address from 127.0.0.1 to 127.255.255.254 is loopback on Windows, so a bridge only has to swap `172.16.1` for `127.0.0` and send the very same packet. Nothing has to be sorted by content, and the emulators run on the bridge's PC.

| Board | Real address | Emulator address |
|---|---|---|
| Radio controller | 172.16.1.101 | 127.0.0.101 |
| Servo controller | 172.16.1.102 | 127.0.0.102 |
| Stepper controller | 172.16.1.105 | 127.0.0.105 |
| Dual Stepper controller | 172.16.1.106 | 127.0.0.106 |
| ADF and Upper controller | 172.16.1.107 | 127.0.0.107 |

In each bridge a small `BoardTargets` class holds, for every board's UDP client, a second client for its emulator, and sends to the board, the emulator or both. The bridge window has two check boxes, **Send to the boards** (on) and **Send to the emulators** (off), and the command line sets them:

| Option | Meaning |
|---|---|
| `--emulators` | Send to the emulators as well as the boards |
| `--emulators-only` | Send to the emulators only: no packet goes to a real board |
| `--emulator-net <a.b.c>` | The first three numbers of the emulators' addresses, when they are not 127.0.0 (the emulator takes `--net` with the same meaning) |

The GPS interface is not a board and is not affected. To see exactly what is being sent to each board or emulator, open the bridge's packet monitor (**Show packets...**, see the bridge chapters). The start-up script (`Start Jet Ranger.bat -Emulators` or `-EmulatorsOnly`) opens the emulators and starts the bridge with the option.

## What each window does

Common to every window: the board's address in the title and status bar, the packet count and the last packet. `BPWR:0`, or no data, switches the **backlight** off (the scale and the lamp legends go dark; the needles stay visible); `C,B:0-15` sets its brightness (about 0.7 until one arrives); `C,RESET:1` forgets everything. After **30 seconds without data** the board blanks, the needles return to their calibrated zero and the lamps and OLEDs go out, as on the real boards (`--watchdog <seconds>` shortens it for tests). The needles move at a limited speed, like a stepper.

![Stepper controller emulator](screenshots/emulator_stepper.png)

**Stepper .105.** Airspeed, vertical speed, radar altitude, torque, gas producer, turbine out temperature and fuel quantity on their dials. `C,SWING<token>:1` (for example `C,SWINGIAS:1`) sweeps a needle to the end of its scale and back.

![Dual Stepper controller emulator](screenshots/emulator_dual_stepper.png)

**Dual Stepper .106.** Engine and transmission oil pressure and temperature, rotor and engine speed, electrical load, fuel pressure, the altimeter (one turn per 1000 ft) and the four OLEDs: barometer (`2992`), altitude, the zulu clock from `ZULU:HHMM` and the temperature text. `PWR:0` blanks the OLEDs; `C,OLEDTEST:1` fills them white.

![Servo controller emulator](screenshots/emulator_servo.png)

**Servo .102.** The attitude indicator (pitch -30 to +30 degrees, bank -90 to +90; the servos move at a limited speed) and the 15 caution and warning lamps, lit by their tokens (`RLOW:1` and so on). The direction in which the horizon turns for a positive bank is drawn as in a real indicator; it has not been checked against the servo's mounting.

![Radio controller emulator](screenshots/emulator_radio.png)

**Radio .101.** The COM and NAV displays (two radios on each, `xxx.xxx`), the Auto Pilot display (HDG, CRS, IAS, VS and ALT in their windows on a 16 column, two row display), the DME display, the 8 Auto Pilot and 11 Audio LEDs and the 20 Huey Warning Panel lamps. `NAVCOM1:0` blanks the displays and puts the LEDs out; `C,LEDTEST:1` lights all 39. The PC sends only COM1 and COM2, so the NAV displays keep the sketch's start-up values (NAV1 109.000 and 111.000, NAV2 110.000 and 112.000).

![ADF and Upper controller emulator](screenshots/emulator_adf.png)

**ADF and Upper .107.** The three landing gear lamps (`LDGN`, `LDGL`, `LDGR`), the OLED (`ADF  340 kHz`, `FOLLOW OFF`; `C,DISPLAY:0` from the Radio blanks it) and the backlight. The Upper switch matrix and the keypads are inputs and are not shown.

The faces (ranges, scales, numerals and legends) are those of the instrument map, Appendix G.

The emulators do not copy the real boards' start-up hold (backlight, OLEDs and lamps on until the Stepper controller announces start-up on UDP 13138): they start dark and follow the data.

## Running it

`JetRangerPanelEmulator.exe` opens the launcher, with a button for each emulator and **Open all five**. `--board radio|servo|stepper|dual|adf|all` opens those windows directly, `--net <a.b.c>` changes the addresses and `--dump <folder>` writes each board's state to `<folder>\<board>.json` four times a second (used by the tests). Each address can be listened on by one program at a time.

## Tests

`python tests\test_emulator.py` starts the five emulators with a 3 second watchdog and sends them the bridges' packets: values and needles, swings, lamps, blanking (`PWR:0`, `NAVCOM1:0`, `C,DISPLAY:0`), brightness and `BPWR`, `C,LEDTEST:1`, `C,RESET:1` and the watchdog (29 checks). `python tests\test_bridges_to_emulators.py` runs the real DCS bridge, fed its synthetic DCS-BIOS stream, and then the real X-Plane bridge, against its fake X-Plane, both with `--emulators-only`, and checks what the emulators show (9 checks). The MSFS and FSUIPC bridges need their simulators for data: they were built, their windows and command lines were checked, and they use the same `BoardTargets` code, but no simulator data has gone through them to the emulators yet.

## Not done yet

Input emulation on these windows (use the older Radio and Upper emulators), heartbeats to the Health Monitor (the emulators do not appear there), the real boards' start-up swings and lamp test, the NAV frequency knobs, and a side-by-side check against the real boards.
