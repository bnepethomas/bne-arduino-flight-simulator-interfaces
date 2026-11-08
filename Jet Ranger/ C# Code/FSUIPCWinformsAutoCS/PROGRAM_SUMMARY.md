# FSUIPCWinformsAutoCS — Program Summary

Bridge for Prepar3D / FSX that reads data through **FSUIPC** (raw memory offsets)
instead of SimConnect. Named `FSUIPCTest` internally. It reads engine, electrical, radio
and annunciator offsets and sends each panel board its own `"D,..."` UDP packet (Servo,
Stepper, Dual Stepper and Radio controllers). It also listens on port 27001 for
radio-swap / battery / alternator / standby-frequency commands from the panel and acts on
them through the FSUIPC API. `MSFSWinformsAutoCS` is the MSFS counterpart; only one
bridge should run against the panel network at a time.

## Program flow

1. **Constructor**: calls `configureForm()`, builds a live annunciator
   light list (`BuildLightList()`, one row per bit in the 0x2F28
   annunciator offset), and starts `timerConnection` (polls once/second
   looking for FSUIPC).
2. **`timerConnection_Tick`**: repeatedly tries `FSUIPCConnection.Open()`;
   once it succeeds, stops itself and starts `timerMain` (ticks every 50ms).
3. **`timerMain_Tick`** (20Hz):
   - Calls `FSUIPCConnection.Process()` to refresh all declared offsets
     (airspeed, avionics master, turbine-out %, gas producer, oil
     pressure/temp, transmission pressure/temp, fuel qty/capacity, battery
     load amps, fuel pressure, torque, rotor RPM, engine N2, altitude, VSI,
     radar altimeter, attitude pitch/bank, main/avionics bus voltage) and
     the separate `"RadioStack"` offset group (COM1/2 active+standby,
     NAV1/2 active+standby, ADF, transponder).
   - Formats a human-readable summary into `textBox1` for on-screen
     debugging.
   - Reads the 21-bit annunciator field (`_annunciators`, offset `0x2F28`)
     and updates the on-screen light list (`UpdateLights`).
   - Runs each changed instrument value through the matching
     `<CODE>_Process()` mapping function (same formulas/tables as
     `JET_RANGER_SERVO_CONTROLLER`) and
     appends it to a `"D,..."` payload, along with every annunciator's
     on/off state by short code. **`RPMR`/
     `RPME` (rotor/engine RPM %) are no longer part of this shared
     payload or run through a `_Process()` mapping** — they used to be
     mapped onto a servo position range (`RPMR_Process()`/`RPME_Process()`,
     since removed) for real Rotor/Engine RPM servos; those gauges are now
     driven by `RSstepper`/`TSstepper` on the Stepper Controller instead,
     which does its own real-percent-to-step conversion
     (`RS_PCT_TABLE`/`TS_PCT_TABLE`), so this app just forwards the raw
     FSUIPC percent value in the Stepper Controller's own payload (see
     next bullet) - now formatted to **one decimal place**
     (`.ToString("F1")`, e.g. `"82.4"`) rather than truncated to a whole
     number: `rpmrPctForStepper`/`rpmePctForStepper` are `double` (were
     `int`, with an explicit truncating cast), since both boards'
     `tsPctToSteps()`/`rsPctToSteps()`/`setTS()`/`setRS()` now take a
     `float` and parse the wire value with `toFloat()` instead of
     `toInt()`, so the fractional percent actually reaches their
     calibration-table interpolation instead of being rounded away
     before it left this app. Leftover from the removal: the
     `Servos.RotorRpmPct1`/
     `Servos.GeneralEngPctMaxRpm1` enum members (and their
     `ServMinPosition`/`ServMaxPosition` array entries) are now unused —
     left in place rather than removed, since deleting an enum member
     risks shifting every other array index that follows it.
   - Throttled to ≥100ms between sends (was ≥200ms). The accumulated
     shared payload is still built but is no longer sent to any board (it
     was only ever read by the retired OLED Controller). The Servo,
     Stepper Controller and OLED Dual Stepper Controller each get their
     own separate, purpose-built payload built fresh each tick.
     The Stepper Controller's payload is `ALT`, `VSI` as raw fpm, `AGL`,
     and `RPMR`/`RPME` as real unmapped percent. The OLED Dual Stepper
     Controller's payload is `ALT`, `RPMR`/`RPME`, and (new) **`ZULU`** —
     the current Zulu time-of-day, HHMM-encoded (e.g. `1430` for 14:30)
     from the new `zuluHours`/`zuluMinutes` FSUIPC offsets (`0x0238`/
     `0x0239`), driving that board's Clock OLED. No `VSI`/`AGL` in this
     payload — that board's UDP handler has no `AGL` case (its Radar Alt
     stepper was repurposed as Fuel Load), and VSI wasn't asked for.
   - Also tracks COM1/2 active+standby frequency and main bus voltage
     (via the same `FsFrequencyCOM`/`FsFrequencyNAV` helpers used for the
     on-screen text) and, throttled the same way (≥200ms if changed, or
     every ≥5s regardless), sends a `"D,C1A:...,C1S:...,C2A:...,C2S:...,
     MAINBUS:...,NAVCOM1:..."` packet to the Radio Controller board.
     `NAVCOM1` is `1` when the avionics bus voltage (offset `0x2850`) is 24 V or more
     and `0` below that (it used to follow the main bus at 20 V); the Radio Controller
     blanks its COM/NAV displays when it receives `NAVCOM1:0`.
4. **`StartListener`** (background task, started from the constructor):
   binds to UDP port 27001 and passes every received string to
   `UpdateRadios()`.
5. **`UpdateRadios(command)`** — the FSUIPC radio command handler:
   - `COM1_RADIO_SWAP` / `COM2_RADIO_SWAP` → `FSUIPCConnection.SendControlToFS`
     with `FsControl.COM_STBY_RADIO_SWAP` / `FsControl.COM2_RADIO_SWAP` —
     FSUIPC's equivalent of a SimConnect `TransmitClientEvent`.
   - `NAV1_RADIO_SWAP` / `NAV2_RADIO_SWAP` → `FsControl.NAV1_RADIO_SWAP` /
     `FsControl.NAV2_RADIO_SWAP` (new; sent by the Radio Controller's NAV1/NAV2
     FREQ switches; without a handler they would fall into the COM1
     standby-frequency parser).
   - `AVIONICS_MASTER_SET` → writes directly to the avionics-master offset
     (`0x2E80`) already used by `chkAvionicsMaster_CheckedChanged`, rather
     than an unverified control ID.
   - `MASTER_BATTERY_ON` / `MASTER_BATTERY_OFF` → FSUIPC's classic control
     list only exposes `TOGGLE_MASTER_BATTERY` (no direction-aware "set"),
     so these are guarded against the main bus voltage already read every
     tick, sending the toggle only when it would actually move the switch
     the requested way.
   - `ALTERNATOR_ON` / `ALTERNATOR_OFF` → `FsControl.TOGGLE_MASTER_ALTERNATOR`.
     Unlike battery, no alternator-state offset is read in this project, so
     this is an unconditional toggle and may need re-sending if it fires
     out of sync with the aircraft's actual state.
   - Anything else that parses as a float in 118.00–136.975 is treated as a
     COM1 standby-frequency request: it's BCD-encoded via
     `new FsFrequencyCOM(value).ToBCD()` and sent with
     `FsControl.COM_STBY_RADIO_SET`.
6. **`chkAvionicsMaster_CheckedChanged`**: writes back to the avionics
   master FSUIPC offset when the operator toggles the checkbox.
7. **`frmMain_FormClosing`** stops both timers and closes the FSUIPC
   connection.

## FSUIPC offsets read/written

| Offset | Meaning |
|---|---|
| `0x02BC` | Airspeed |
| `0x2E80` | Avionics master (read/write) |
| `0x08F0` | Turbine-out % / turbine-out temperature |
| `0x08BA` / `0x08B8` | Engine oil pressure / temperature |
| `0x0900` / `0x0904` | Transmission oil pressure / temperature |
| `0x0B74` / `0x0B78` | Fuel percent quantity / fuel capacity |
| `0x282C` | Battery load amps |
| `0x08F8` | Fuel pressure |
| `0x08F4` | Torque percent |
| `0x0898` | Gas producer |
| `0x0908` / `0x0896` | Rotor RPM / engine N2 |
| `0x3324` | Altitude |
| `0x02C8` | Vertical speed |
| `0x31E4` | Radio altimeter |
| `0x2F70` / `0x2F78` | Attitude pitch / bank |
| `0x2F28` | 21-bit annunciator/warning-lamp bitfield |
| `0x2840` / `0x2850` | Main bus voltage / avionics bus voltage |
| `0x0238` / `0x0239` | Zulu hours / Zulu minutes (documented FSUIPC offsets, bytes) — combined into the HHMM-encoded `ZULU` field sent to the OLED Dual Stepper Controller's Clock OLED |
| `RadioStack:0x034E/0x311A/0x3118/0x311C/0x0350/0x311E/0x0352/0x3120/0x0354/0x034C/0x0356` | COM1/2 active+standby, NAV1/2 active+standby, transponder, ADF |

## Panel LEDs and panel controls

- **LEDs:** the radio packet carries 19 `LED_xxx:0/1` tokens (Auto Pilot and Audio panel LEDs on the Radio controller's MAX7219), built by `BuildPanelLedPayload()` from the standard offsets
  `0x07BC` (AP master), `0x07C4` (NAV1 lock = CRS), `0x07C8` (heading lock), `0x07D0` (altitude lock), `0x07DC` (airspeed hold), `0x0800` (approach hold), `0x0804` (back course hold),
  `0x2EE0` (flight director), `0x3122` (audio switches: bit 7 COM1 tx, 6 COM2 tx, 5 COM both, 4 NAV1, 3 NAV2, 2 marker, 1 DME, 0 ADF) and `0x0BAC` / `0x0BAE` / `0x0BB0` (inner / middle /
  outer marker). All are read in the `RadioStack` group and all 19 are 0 while the avionics bus is below 24 V. A change is sent like a frequency change. The DodoSim aircraft may not drive every offset.
- **Backlight:** the radio packet also carries `BPWR` (1 while the main bus, offset `0x2840`, is 20 V or more) for the Radio controller's panel backlight, like the Servo, Stepper and Dual Stepper packets.
- **Controls:** `TryPanelCommand()` (checked first in `UpdateRadios()`) maps the control names sent on UDP 27001 by the Radio and ADF controllers to FSUIPC controls (`PanelControls`): `AP_MASTER`,
  `AP_HDG_HOLD`, `AP_NAV1_HOLD`, `AP_ALT_HOLD`, `AP_APR_HOLD`, `TOGGLE_FLIGHT_DIRECTOR`, `AP_BC_HOLD`, `AP_PANEL_SPEED_HOLD`, `VOR1_OBI_INC/DEC`, `AP_ALT_VAR_INC/DEC`, `AP_VS_VAR_INC/DEC`,
  `AP_SPD_VAR_INC/DEC`, `HEADING_BUG_INC/DEC`, `COM1/COM2_TRANSMIT_SELECT`, `COM_RECEIVE_ALL_TOGGLE`, `RADIO_VOR1/VOR2/DME1/ADF_IDENT_TOGGLE`, `MARKER_SOUND_TOGGLE`, `GEAR_UP/DOWN`,
  `FLAPS_UP/DOWN`, `SPOILERS_ON/OFF`, `PARKING_BRAKES`, each sent with parameter 0; a `:N` suffix repeats the control N times (1 to 50). Built clean; not run against a simulator.

## Real-unit payloads per board (2026-10-16)

The Servo, Stepper and Dual Stepper boards each now get their **own** packet in
**real units** (the shared "front-panel" packet of Bell 206 servo-position numbers
from `IAS_Process()`/`VSI_Process()`/`PITCH_Process()`... is built as before but
no board takes it any more). Values come straight from
`sFrontPanel` - the same sources the `*_Process()` functions map from - formatted
with the invariant culture. Each board converts to steps / servo positions with
its own calibration tables.

| Board | Packet |
|---|---|
| Servo `.102` | `PITCH` (degrees, one decimal, negated as `PITCH_Process()` did: FSUIPC pitch is nose-down positive, the servo table nose-up), `BANK` (degrees, one decimal), plus the annunciator lamp codes (`EOUT`, `TOPW`, `BHOT`, `EC`, `BD`, `GENF`, `TOTW`, `RLOW`, `FFLTR`, `BTMP`, `TC`, `TRC`, `FPMP`, `LOWF`, `SCF`) as `:1`/`:0` |
| Single-board Stepper `.105` | `IAS` (knots, one decimal - FSUIPC offset 0x02BC is knots x 128, so the resolution is finer than 0.1; the Stepper sketch interpolates the fraction), `VSI` (fpm), `AGL` (feet, the existing power-gated string), `FUEL` (US gal), `TQ` (%), `ITT` (deg C), `N1` (%) - no longer `ALT`/`RPMR`/`RPME`, which that board does not have |
| Dual Stepper `.106` | `PWR` (first in the packet: `1`, or `0` while the main bus is below 24 V, which blanks the Baro, Altimeter, Clock and Temperature OLEDs), `ALT` (feet), `RPMR`/`RPME` (%, one decimal), `ZULU` (HHMM), `OILT` (deg C), `OILP` (psi), `XMSNT` (deg C), `XMSNP` (psi), `FUELLOAD` (psi, from fuel pressure), `ELECTRICALLOAD` (%, from `ELECTRICAL_TOTAL_LOAD_AMPS`, the same input `ELOAD_Process()` took), `TEMP` (outside air temperature as text, e.g. `21C`, from new offset `0x0E8C` = deg C x 256 - a standard FSUIPC offset, not verified against DodoSim) |

Whole-number values are rounded; the boards clamp to their calibrated ranges.
**Built clean in place (0 errors). Connected to Prepar3D v6.1 and sending
about 10 packets per second per board (the timer runs at its default 100 ms); the
payload values have not yet been checked against the gauges.**
Pre-existing quirks left alone: several unused annunciator bits (`SPARE_*`,
`SIMCONNECT_FAIL`, `LOW_INLET_PRESSURE`) all share the short code `SCF`, so the last
one in the packet wins for that lamp; and the `TURB_ENG_ITT_1` field is assigned
twice per tick (percent, then temperature), so the temperature wins.



| Setting | Value |
|---|---|
| Local listen port | **27001** — bound in `StartListener()` (started from the constructor); receives radio/battery/alternator/standby-frequency commands from the Radio/Upper controller boards |

## Remote endpoints this app talks to

| Target | Port | Purpose |
|---|---|---|
| `172.16.1.101` (Radio Controller) | 13136 | Radio/bus-voltage data packets (`udpClient.Send`) |
| `172.16.1.102` (Servo Controller) | 13136 | Front-panel instrument + annunciator data packets (`frontPanelClient.Send`) |
| `172.16.1.105` (Stepper Controller) | 13136 | Its own minimal payload (`stepperClient.Send`), separate from the Servo Controller's — `ALT`, `VSI` (raw fpm, not the Servo Controller's Bell 206 `VSI_Process()` number), `AGL` (radar altitude), and `RPMR`/`RPME` (real, unmapped percent — see Program flow above). Built fresh each tick from `ALTITUDE`/`vsiRawFpm`/`PLANE_ALT_ABOVE_GROUND`/`rpmrPctForStepper`/`rpmePctForStepper` rather than derived from the shared front-panel payload. (`VSI` was dropped temporarily while diagnosing a reported ALT-needle reversal on real hardware — confirmed unrelated to VSI, root cause was the stepper driver electronics mishandling small step deltas; see `JET_RANGER_STEPPER_CONTROLLER`'s summary.) `IAS`/annunciator bits/etc. were never part of this payload, since this board has no gauges for them |
| `172.16.1.106` (OLED Dual Stepper Controller) | 13136 | Its own minimal payload (`dualStepperClient.Send`), separate from the Stepper Controller's — `ALT`, `RPMR`/`RPME` (real, unmapped percent), and `ZULU` (HHMM-encoded Zulu time, new — drives that board's Clock OLED). Built fresh each tick, not derived from the shared front-panel payload. No `VSI`/`AGL` — that board's UDP handler has no `AGL` case (Radar Alt repurposed as Fuel Load) |
| `172.16.1.2` | 26028 | `OutputClient` — connected but unused |

> Note: all six `.Connect(...)` calls that wire up these targets live in
> `Form1.Designer.cs` rather than `Form1.cs`'s constructor — an unusual
> location (that file is normally generated UI layout only) but functionally
> equivalent since it still runs during `InitializeComponent()`.

## Programs this communicates with

- **FSUIPC** (must be installed/running, bridging to FSX/P3D) — data source
  and event/control sink (`SendControlToFS`, offset writes).
- **[JET_RANGER_SERVO_CONTROLLER](../../Jet%20Ranger%20Arduino%20Sketches/JET_RANGER_SERVO_CONTROLLER/JET_RANGER_SERVO_CONTROLLER.ino)**
  (`172.16.1.102:13136`) — receives this app's instrument/annunciator data.
- **[JET_RANGER_RADIO_CONTROLLER](../../Jet%20Ranger%20Arduino%20Sketches/JET_RANGER_RADIO_CONTROLLER/JET_RANGER_RADIO_CONTROLLER.ino)**
  (`172.16.1.101:13136`) — receives radio/bus data; sends control commands
  back to this app's port 27001.
- **[JET_RANGER_UPPER_CONTROLLER](../../Jet%20Ranger%20Arduino%20Sketches/JET_RANGER_UPPER_CONTROLLER/JET_RANGER_UPPER_CONTROLLER.ino)**
  — also a possible sender of control commands to port 27001.
- **[JET_RANGER_STEPPER_CONTROLLER](../../Jet%20Ranger%20Arduino%20Sketches/JET_RANGER_STEPPER_CONTROLLER/JET_RANGER_STEPPER_CONTROLLER.ino)**
  (`172.16.1.105:13136`) — receives its own separate, purpose-built
  payload (`ALT`/`VSI`/`AGL`/`RPMR`/`RPME`), not the shared front-panel
  bytes.
- **[JET_RANGER_OLED_DUAL_STEPPER_CONTROLLER](../../Jet%20Ranger%20Arduino%20Sketches/JET_RANGER_OLED_DUAL_STEPPER_CONTROLLER/JET_RANGER_OLED_DUAL_STEPPER_CONTROLLER.ino)**
  (`172.16.1.106:13136`) — receives its own separate, purpose-built
  payload (`ALT`/`RPMR`/`RPME`/`ZULU`), not the shared front-panel bytes;
  the new `ZULU` field drives its Clock OLED.
- Alternative to **MSFSWinformsAutoCS** for sims that expose data through FSUIPC; only one bridge
  app should run against the panel network at a time.

## Aircraft information for the GPS interface (2026-10-18)

Every 500 ms `SendGpsPacket()` sends `D,LAT:..,LON:..,ALT:..,IAS:..,HDG:..` to **`LowranceAirMapGps`** (UDP `127.0.0.1:27002`;
change `GpsHost` / `GpsPort` in `Form1.cs` if the GPS interface runs on another PC). Four new offsets are read: `0x0560` latitude
(90 degrees = 10001750 x 65536 x 65536), `0x0568` longitude (360 degrees = 65536^4), `0x0570` altitude in metres (64-bit, 32 bits of
fraction, converted to feet) and `0x0580` true heading (360 degrees = 65536^2). `IAS` is the airspeed already read from `0x02BC`.
The packet also carries `UTC:yyyyMMddHHmmss`, the simulator's UTC date and time from the FSUIPC client library's
`FSUIPCConnection.UTCDateTime` (left out if the read fails, so the GPS interface uses the PC clock). Send errors (GPS interface not running) are ignored. Built clean; **not yet run against Prepar3D**, so the offset conversions are
unchecked against the simulator.

## Backlight power (2026-10-18)

`BPWR:1` / `BPWR:0` leads the Servo and Stepper packets and follows `PWR` in the Dual Stepper packet: 1 while the main bus (offset `0x2840`) is
20 V or more. The boards use it to switch their backlight off when the main bus is down (see their summaries).

## Auto Pilot display values and position commands (2026-10-25)

- **Auto Pilot display:** the radio packet also carries `APHDG`, `APCRS`, `APIAS`, `APVS` and `APALT` (`BuildPanelValuePayload()`): heading from `0x07CC` (degrees x 65536 / 360, three digits, north is 360), course from the NAV1
  OBS `0x0C4E` (three digits), airspeed `0x07E2` (knots), vertical speed `0x07F2` (feet per minute) and altitude `0x07D4` (metres x 65536, sent in feet), all read in the `RadioStack` group; a change goes out like a frequency change.
- **Toggle switch positions:** `TryPositionCommand()` (before `TryPanelCommand()`) handles `GEAR_SET:1/0`, `FLAPS_SET:1/0` and `SPOILERS_SET:1/0` from the ADF controller as the FSUIPC controls `GEAR_SET`
  (1 or 0), `FLAPS_SET` and `SPOILERS_SET` (16383 or 0), so the simulator follows the switch instead of toggling. Built clean; not run against a simulator.

## Anti-ice position and ADF / Upper backlight power (2026-10-25)

- **`ANTI_ICE_SET:1/0`:** `TryPositionCommand()` also handles the ADF panel's anti-ice switch position and sends the FSUIPC control `ANTI_ICE_SET` (1 or 0). The DodoSim 206 V2 may ignore the plain control: bind its own `ANTI_ICE_SWITCH_SET` event to the joystick button in FSUIPC instead (hyd boost `HYDRAULICS_SWITCH_SET` and caution dim `CAUTION_LIGHT_SWITCH_SET` have no safe bridge command).
- **ADF and Upper boards:** (the Upper board was merged into the ADF board on 31 October 2026, so `upperClient` now sends to a retired address and should be removed) new `adfClient` (172.16.1.107:13136) and `upperClient` (172.16.1.103:13136) send `D,BPWR:1/0` (main bus 20 V or more) with the other boards' packets, so those boards' backlights follow the main bus and their 30 s no-data timeouts see the bridge. Built clean; not run against a simulator.

## Panel emulators (added 1 November 2026)

The window has two check boxes under the Avionics Master box, **Send to the boards** (on) and **Send to the emulators** (off). Ticked, every board packet is also sent to `JetRangerPanelEmulator`
(UDP 13136 on 127.0.0.<the board's last number>, for example 127.0.0.105 for the Stepper); with **Send to the boards** unticked the emulators get them alone and no packet goes to
a board. Command line: `--emulators` (emulators as well as the boards), `--emulators-only`, `--emulator-net <a.b.c>` (the first three numbers of the emulators' addresses, default
127.0.0). `BoardTargets.cs` holds a second UDP client for each board client and sends to the board and/or the emulator; the GPS interface is not a board and is unaffected.
The seven board sends in `Form1.cs` go through `BoardTargets` (the retired Upper .103 is a board with no emulator). Built and the window checked; no simulator data has been sent through it to the emulators yet.

## Packet monitor (added 1 November 2026)

**Show packets...** (a button under the Avionics Master box, beside the two Send check boxes) opens a window that lists every payload as it is sent: one line per packet with the time, the target board (Radio .101, Servo .102, Stepper .105,
Dual Stepper .106, ADF and Upper .107, the retired Upper .103 (this bridge still sends it the backlight packet), and the GPS interface) and where it went (*board*, *emulator*, *board + emulator*, or *local* for the GPS interface), then
the payload text, for example `15:15:00.234  Stepper .105  board  D,BPWR:1,IAS:82,AGL:450,...`. The **Target board** box filters the list to one board (or *All boards*); **Pause** stops
adding lines (the packets in between are dropped), **Clear** empties the list, and the window counts the lines shown and the packets sent since it opened. Command line: `--show-packets` opens
the window at start and `--packet-board <radio, servo, stepper, dual, adf, gps, or a last number such as 105>` opens it filtered to that board. `BoardTargets` (in `BoardTargets.cs`) raises a
`PacketSent` event for every send (the board packets through `Send`, the GPS packets through `SendDirect`) and `PacketMonitorForm` (`PacketMonitor.cs`) queues them and shows them ten times a
second, keeping the last 6,000 for re-filtering, so a busy bridge is not slowed. Only packets that are really sent are shown: nothing appears while both the boards and the emulators boxes are
unticked, or (DCS and X-Plane bridges) while no data is arriving. Tested with the DCS bridge and its synthetic stream (all boards, the `--packet-board stepper` filter, and the filter box changed
while open); the MSFS and FSUIPC windows were checked (button, `--show-packets`) but no simulator data has gone through their monitor.

## DME readout (8 November 2026)

The bridge reads NAV1's DME from three more offsets in the `RadioStack` group: `0x0300` (distance, nautical miles x 10, 16-bit), `0x0302` (speed, knots x 10, signed 16-bit) and bit 4 of the status word at `0x3300` ("NAV1 has DME"). `BuildDmePayload()` adds `,DMEDIST:12.3,DMESPD:120` to the Radio packet (distance to one decimal, speed in whole knots, a negative speed sent as 0); with no DME it sends `DMEDIST:-1,DMESPD:-1`. A change goes out like a frequency change (at most every 200 ms), otherwise every 5 seconds. The Radio controller (build 12 or later) shows them on its DME display. The offsets are from "FSUIPC for Programmers"; the bridge builds (0 errors) but has not been run against Prepar3D with a tuned DME station, so check the values against the aircraft's DME. The DodoSim 206 may not drive them.

## About window, version and build number (8 November 2026)

The **About...** button (under Show packets...) opens it. The **data out** text box (the readout) now grows and shrinks with the window, the lamp list stays against the right edge, and a **Pause data out** check box freezes the text box while the data keeps going to the boards (the readout is only redrawn when the box is unticked). Checked by resizing the running window (the box went from 281 x 441 to 798 x 702 pixels at a 1300 x 900 window). **Raise `<BuildNumber>` in the project file by one every time this program is changed** (and `<Version>` for a larger change); the build date and time are stamped in automatically at every build, so they are always the time of the last build. `AboutForm.cs` reads them from the program (assembly metadata `BuildNumber` and `BuildDateTime`, and the assembly version) and shows the program name, version, build number and build date and time. The version is 1.0.0 and the build number started at 1 on 8 November 2026.

## DME on the data out box, and the station identifier (8 November 2026, build 2)

The data out text box now has a line `DME (NAV 1): IBN  12.3 NM  120 KT` (`none` with no DME). The identifier is read from offset `0x3000` (VOR1 identity, a 6 byte string) and `BuildDmePayload()` adds `,DMEID:IBN` (capitals, letters and digits only, at most 4 characters; `-` when there is none) to the Radio packet. Builds clean; not run against Prepar3D. The Radio shows the identifier from build 14.

## Radio packet twice a second (8 November 2026, build 3)

The radio packet now goes out at least every 500 ms (`RadioRefreshMs`), instead of every 5 seconds when nothing changed; a change is still sent at once (at most every 200 ms). Built clean; not run against Prepar3D. (The same change in the MSFS bridge was checked with a UDP listener: 0.53 to 0.56 s between packets.)

## Scroll bar on the data out box (8 November 2026, build 4)

The data out text box has a vertical scroll bar. Because setting its text sends the view back to the top, the readout is now written through `ShowReadout()`, which only rewrites the box when the text changed and puts the scroll position back afterwards, so the box can be scrolled while the data keeps updating (Pause data out still freezes it completely). Built clean; not run against Prepar3D.

## Autopilot and audio status on the data out box (8 November 2026, build 5)

The data out text box now shows what the simulator reports for the Radio panel's LEDs and Auto Pilot display, so a LED that does not light can be traced: `Auto Pilot LEDs lit: none` (or a list such as `AP ENGAGE, HDG, ALT`), `Auto Pilot display: HDG 360  CRS 196  IAS 0  VS 0  ALT 0` and `Audio LEDs lit: COM1, NAV2, DME, NAV1, MKR`. These are the states before the radio power rule (the Radio turns the LEDs off when the avionics bus is below 24 V). Built clean; not run against Prepar3D.

## Live NAV active frequencies (8 November 2026, build 6)

The radio packet now has `N1A` and `N2A`, the NAV1 and NAV2 active frequencies (from offsets `0x0350` and `0x0352`, shown as in the COM frequencies), so the Radio's NAV display follows the simulator. The standby frequencies are not sent. A change goes out like a COM frequency change. The Radio shows them from build 15. Builds clean; not run against Prepar3D.

## Checked live against Prepar3D v6.1 (8 November 2026, build 7)

Run with `--emulators-only` and a UDP listener on 127.0.0.101 while Prepar3D v6.1 was flying: the bridge connected, the data out box showed `DME (NAV 1): OTT  15.9 NM  0 KT`, `NAV 1: 113.70`, `NAV 2: 110.60` and the Auto Pilot and Audio lines, and the radio packet carried `N1A:113.70`, `N2A:110.60` (the active frequencies only), `DMEDIST:15.9`, `DMESPD:0` and `DMEID:OTT`, with the Audio LEDs `COM1` and `MKR` lit. The Auto Pilot LEDs were all off in that aircraft (no autopilot engaged). Build 7 only changes the Auto Pilot display line in the data out box to `HDG 360  CRS 340  IAS 0  VS 0  ALT 0`, like the MSFS bridge.

## DodoSim 206 SAS on the Auto Pilot LEDs (8 November 2026, build 8)

The DodoSim 206's SAS (stability augmentation system and autopilot) keeps its annunciator LEDs in local variables, named in `DodoSim206.gau`: `DodoSim_SasSasLed`, `DodoSim_SasHdgLed`, `DodoSim_SasNavLed`, `DodoSim_SasSpdLed`, `DodoSim_SasAltLed`, `DodoSim_SasVrtLed` and `DodoSim_SasPwrLed`. They are bit fields: bit 1 (value 1) is yellow (primed or standby), bit 2 (value 2) is green (engaged); the SAS LED read 6 in attitude hold with HDG and ALT on, so another bit (probably the red or bypass flash) can be set with the green. `ReadDodoSas()` reads them with FSUIPC's local variable access (`FSUIPCConnection.ReadLVar`) about five times a second, and the green bit of each lights the matching Auto Pilot LED: **SAS = AP ENGAGE, HDG = HDG, NAV = CRS, SPD = IAS, ALT = ALT**; VRT has no LED on the panel, and the standard autopilot offsets still work (the LED is on if either source says so). The data out box has a line `DodoSim SAS LEDs (0 off, 1 yellow, 2 green): SAS 1  HDG 0 …`. As the DodoSim manual says, the SAS must be enabled first (see the DodoSim 206 V2 Settings window, `Has SAS fitted`). Checked live against Prepar3D v6.1 with the DodoSim 206: in attitude hold with HDG and ALT engaged the line read `SAS 6  HDG 2  NAV 0  SPD 0  ALT 2  VRT 0  PWR 2` and `Auto Pilot LEDs lit: AP ENGAGE, HDG, ALT`; in standby (`SAS 1`) none lit. The Release build could not be rebuilt while a copy of the bridge was running.

## Upper keypad: * toggles the ATC menu (8 November 2026, build 9)

The bridge listens on UDP port 27003 (`StartUpperKeyListener()`) for `UPPER_KEY:<key>`, which the ADF and Upper controller (build 10 or later) sends for each press on the Upper keypad. **`*` toggles the ATC menu** in Prepar3D / FSX with the FSUIPC control `ATC` (`FSUIPCConnection.SendControlToFS(FsControl.ATC, 0)`); other keys are only noted. The data out box has a line `Last Upper keypad key: * at 10:24:05 - ATC menu toggled` (or `… NOT toggled (<reason>)` if FSUIPC refused it). Checked by sending the packets from a script to a Debug copy of the bridge while Prepar3D v6.1 was running: `UPPER_KEY:5` was noted, and each `UPPER_KEY:*` reported `ATC menu toggled` with no error (sent twice, so the menu ended as it started). The ADF controller's other events on the same port (frequency, radar follow ID, power status) are ignored. This is the first part of the To Do item for simulator-dependent keystrokes on the Upper keypad.

## ADF frequency to the ADF controller (8 November 2026, build 10)

The packet to the ADF controller (.107) now carries the simulator's active ADF frequency as well as the backlight power: `D,BPWR:1,ADF:284` (from the ADF offsets `0x034C` and `0x0356`, rounded to the kHz; left out outside 190 to 1799). The ADF controller shows it from build 11. Checked live with Prepar3D v6.1: the data out box read `ADF 1 : 0284.0` and the packet to the ADF controller's address was `D,BPWR:1,ADF:284` (43 packets in 16 seconds).

## Bridge icon (8 November 2026, build 11)

The program icon is a suspension bridge (`bridge.ico` in the project folder: 256, 64, 48, 32, 24 and 16 pixel images, drawn with GDI+, orange cables and towers over blue water). It is set as the application icon in the project file (`<ApplicationIcon>bridge.ico</ApplicationIcon>`, so it is the icon of the exe) and, from the exe, as the icon of the main window and the About window (`Icon.ExtractAssociatedIcon`). The same icon is used by all four bridges.

## Keys the bridge handles (8 November 2026, build 12)

The packet to the ADF controller now ends with `,BRIDGEKEYS:*` (`D,BPWR:1,ADF:284,BRIDGEKEYS:*`): it tells the ADF and Upper controller (build 12 or later) that this bridge handles `*` on the Upper keypad itself (the ATC menu), so the keypad does not also type an asterisk. While the bridge is not running the ADF controller types every key as before. Checked with a UDP listener on the ADF controller's loopback address (34 packets in 16 seconds, each ending `BRIDGEKEYS:*`).

## ADF keypad sets the squawk code (8 November 2026, build 13)

The packet to the ADF controller now also carries `ADFKEYPAD:SQUAWK` (the ADF keypad enters the squawk code while this bridge runs) and the simulator's transponder code, `SQK:3374` (from `FsTransponderCode`, offset `0x0354`). The ADF controller (build 13 or later) sends `SQUAWK_SET:<code>` to port 27003 when the code is complete and `#` is pressed; the bridge sets the transponder with the generic control **`XPNDR_SET`** (`FSUIPCConnection.SendControlToFS(FsControl.XPNDR_SET, <the four digits as BCD>)`), no DodoSim-specific event. A code that is not four digits 0 to 7 is ignored. The data out box has a line `Squawk (transponder): 3374   last ADF keypad entry: 11:00:29 squawk 1234 set`. Checked live against Prepar3D v6.1 with the DodoSim 206 by sending the packets to a copy of the bridge: `SQUAWK_SET:1234` changed the simulator's code from 3374 to 1234, `SQUAWK_SET:9999` was ignored, and the original 3374 was sent back; the packet to the ADF controller's address ended `BRIDGEKEYS:*,ADFKEYPAD:SQUAWK,SQK:3374`.

## ADF knob frequency to the simulator (8 November 2026, build 14)

The ADF controller sends `ADF_FREQ:<kHz>` to port 27003 a moment after its knobs stop turning (it always did; nothing read it). `HandleAdfFrequency()` now sets the simulator's ADF frequency with the generic control **`ADF_COMPLETE_SET`** for 190 to 1799 kHz; anything else is ignored. The parameter, found by trying it against Prepar3D v6.1, is the frequency in **kHz x 10000 as BCD digits** (340 kHz is `0x3400000`, 1340 kHz `0x13400000`); read back from the ADF offsets, 190, 340, 530, 1340 and 1799 kHz all came out right. The data out box has a line `Last ADF knob frequency from the ADF controller: 11:07:38 340 kHz set`. Checked through the bridge on Prepar3D: `ADF_FREQ:340` set 340.0, `ADF_FREQ:99` was ignored, and 284 was sent back. The sim's ADF frequency still goes the other way to the ADF controller's display (`ADF:284` in its packet), which ignores it for a second after a knob turn, so the two do not fight. The ADF knobs also send joystick buttons 13 to 16, which FSUIPC may map to the DodoSim's own ADF events; the exact frequency sent after the knob stops is the final value either way.
