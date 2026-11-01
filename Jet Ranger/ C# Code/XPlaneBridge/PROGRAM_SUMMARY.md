# XPlaneBridge - Program Summary

Windows Forms bridge (.NET 10) that feeds the Jet Ranger panel boards from **X-Plane** (11 or 12). It is the X-Plane counterpart of `DCSBiosBridge`,
`FSUIPCWinformsAutoCS` and `MSFSWinformsAutoCS`: it reads datarefs over X-Plane's built-in UDP interface (**no plugin**), converts them with a **profile**, and sends them
to the boards in the same `D,TOKEN:value` packets (UDP 13136) the other bridges use, so the board firmware does not change. Run only one bridge at a time.

**Status (31 October 2026): output only.** It builds (Release, 0 warnings) and passes `tests\test_bridge.py` (an end-to-end test with a fake X-Plane that answers the
dataref subscriptions, 11 checks, generic profile and the Robinson R22 profile). It has **not been run against a real X-Plane yet**; the dataref names were checked against
X-Plane 12's `Resources\plugins\DataRefs.txt` (every dataref in the profiles exists there).

## How it works

1. **Dataref link (`XPlaneLink.cs`).** One UDP socket on an ephemeral port. For every dataref the profile needs it sends an `RREF` request (`"RREF\0"`, int32 frequency in Hz,
   int32 index, 400-byte dataref path; an array element is written `name[0]`) to X-Plane (default `127.0.0.1:49000`, X-Plane's receive port). X-Plane replies to the request's
   source address and port with `"RREF,"` followed by 8-byte records of int32 index and float32 value. The requests are repeated every 5 s (so the bridge can start before X-Plane
   and survive X-Plane restarting), at 10 Hz; on exit the bridge cancels them (frequency 0).
2. **Finding X-Plane.** With no `--xplane-host` the bridge listens for X-Plane's beacon (multicast 239.255.1.1 port 49707) and uses the host and port it announces; with
   no beacon it keeps trying `127.0.0.1:49000`. Use `--xplane-host` and `--xplane-port` when X-Plane is on another PC or the receive port was changed (Settings, Network).
   X-Plane must be allowed to receive UDP data (Settings, Network, "Accept incoming connections").
3. **Profiles.** `profiles\_common.json` (the file whose name starts with an underscore) holds the values nearly every aircraft gives through the standard datarefs and is
   added to every aircraft profile (the aircraft's own value for a token wins); on its own ("Generic (common values, any aircraft)" in the Profile box) it drives any aircraft.
   X-Plane gives no reliable aircraft name over this interface, so the profile is **chosen by hand** (or `--module <name>`). A value is `token`, `dataref` (or `datarefs` for a
   clock), optional `divisor` dataref (a ratio, for torque), `type` (`analog`, `lamp` or `hhmm`), `scale`, `offset`, `decimals`, `suffix`, `threshold`, `invert`, `hint` and `note`.
4. **Common values.** `IAS`, `ALT`, `VSI`, `AGL` (radio altimeter), `PITCH`, `BANK`, `HDG`, `LAT`, `LON`, engine 1 `N1`, `RPME` (N2), `ITT`, `TQ` (torque over the
   maximum torque, as a percent), `OILP`, `OILT`, `FUELLOAD` (fuel pressure), `FUEL` (total fuel kg to US gallons), `ZULU` (hhmm from the zulu hours and minutes), `TEMP` (outside
   air, with the C suffix), the six Servo lamps (`RLOW`, `GENF`, `LOWF`, `EC`, `FPMP`, `EOUT`) and ten Huey Warning Panel LEDs from the nearest generic annunciator
   (approximations: X-Plane has no Huey caution panel). Some aircraft leave datarefs at 0, then that gauge sits at zero; use **Search datarefs...** and an aircraft profile.
5. **Aircraft profiles.** `Robinson R22.json` (rotor speed from `prop_speed_rpm[0]`, 530 rpm = 100 percent; the 14 V bus doubled so the boards' 20 V and 24 V power thresholds
   work) and `Sikorsky S-76.json` (293 rpm = 100 percent, 28 V bus). Rotor speeds and bus scales are first guesses: check them against the cockpit gauges.
6. **Boards.** `Payloads.Build` (same as the DCS bridge) sorts the tokens by board and builds one packet each: Radio (.101), Servo (.102), Stepper (.105), Dual Stepper (.106) and the GPS
   interface (127.0.0.1:27002), plus `D,BPWR:x` to the ADF and Upper board (.107). Power rules as in the other bridges: no bus voltage mapped (`MAINBUS`) means powered while data is
   flowing; `MAINBUS` at or above 20 V switches the backlights and servo lamps on, `AVIONICSBUS` (or `MAINBUS`) at or above 24 V powers the radio, and the Huey LEDs go dark with the radio.
   An unpowered aircraft sends `AGL:0` to the Stepper.
7. **Cadence.** Radio packet on change (at most every 200 ms) or every 5 s; the other boards about every 90 ms; GPS twice a second. When X-Plane's data stops for 3 seconds nothing is
   sent, so the boards' 30 second no-data watchdogs return the gauges to zero.

## Window and command line

X-Plane status (address and whether data is arriving), Profile box, **Reload**, **Search datarefs...** (words matched against the names and descriptions in `DataRefs.txt`, best
first, in the log), **Send to the boards**, a table of tokens with dataref, raw value, value sent and board, and a log. Options: `--board-host <ip>` (send every board packet there,
for testing), `--xplane-host`, `--xplane-port`, `--datarefs <DataRefs.txt>` (default `C:\X-Plane 12\Resources\plugins\DataRefs.txt`), `--profile-dir`, `--module <name>`, `--no-send`, `--logfile`.

**GPS date and time.** The common profile's `UTC` value (type `utc`) is the zulu seconds since midnight (`sim/time/zulu_time_sec`) with the zulu month and day (`sim/cockpit2/clock_timer/zulu_month`, `zulu_day`); X-Plane has no year, so the PC's UTC year is used. It goes in the 500 ms GPS packet as `UTC:yyyyMMddHHmmss.fff`, so the Lowrance GPS follows the simulator's clock and date (including a changed time of day or time acceleration).

## Test

`python tests\test_bridge.py [XPlaneBridge.exe]` starts a fake X-Plane on UDP 49100, runs the bridge against it with the generic profile and then the Robinson R22 profile (switching the bus voltage
off half way), and checks the packets on UDP 13136 and 27002. Only the processes the script starts are stopped.

## Not done yet

Inputs (the panels' switches already reach X-Plane through the USB HID board as a joystick, so none are sent here), the radio commands on port 27001, port 27003 (ADF frequency and
radar follow ID), landing gear lamps, an aircraft profile for the X-Plane default fixed-wing and twin helicopters, running it against a real X-Plane. (Documentation: master document and User Guide, done 31 October 2026.)

## Panel emulators (added 1 November 2026)

The window has a second check box, **Send to the emulators** (off by default), beside **Send to the boards**. Ticked, every board packet is also sent to `JetRangerPanelEmulator`
(UDP 13136 on 127.0.0.<the board's last number>, for example 127.0.0.105 for the Stepper); with **Send to the boards** unticked the emulators get them alone and no packet goes to
a board. Command line: `--emulators` (emulators as well as the boards), `--emulators-only`, `--emulator-net <a.b.c>` (the first three numbers of the emulators' addresses, default
127.0.0). `BoardTargets.cs` holds a second UDP client for each board client and sends to the board and/or the emulator; the GPS interface is not a board and is unaffected.
Tested with the real bridge, the fake X-Plane and the five emulators: `JetRangerPanelEmulator\tests\test_bridges_to_emulators.py`.

## Packet monitor (added 1 November 2026)

**Show packets...** (a button in the toolbar, after Send to the emulators) opens a window that lists every payload as it is sent: one line per packet with the time, the target board (Radio .101, Servo .102, Stepper .105,
Dual Stepper .106, ADF and Upper .107, the retired Upper .103, and the GPS interface) and where it went (*board*, *emulator*, *board + emulator*, or *local* for the GPS interface), then
the payload text, for example `15:15:00.234  Stepper .105  board  D,BPWR:1,IAS:82,AGL:450,...`. The **Target board** box filters the list to one board (or *All boards*); **Pause** stops
adding lines (the packets in between are dropped), **Clear** empties the list, and the window counts the lines shown and the packets sent since it opened. Command line: `--show-packets` opens
the window at start and `--packet-board <radio, servo, stepper, dual, adf, gps, or a last number such as 105>` opens it filtered to that board. `BoardTargets` (in `BoardTargets.cs`) raises a
`PacketSent` event for every send (the board packets through `Send`, the GPS packets through `SendDirect`) and `PacketMonitorForm` (`PacketMonitor.cs`) queues them and shows them ten times a
second, keeping the last 6,000 for re-filtering, so a busy bridge is not slowed. Only packets that are really sent are shown: nothing appears while both the boards and the emulators boxes are
unticked, or (DCS and X-Plane bridges) while no data is arriving. Tested with the DCS bridge and its synthetic stream (all boards, the `--packet-board stepper` filter, and the filter box changed
while open); the MSFS and FSUIPC windows were checked (button, `--show-packets`) but no simulator data has gone through their monitor.
