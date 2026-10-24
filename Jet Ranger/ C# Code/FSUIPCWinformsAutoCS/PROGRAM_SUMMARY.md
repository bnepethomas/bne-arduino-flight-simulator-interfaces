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
