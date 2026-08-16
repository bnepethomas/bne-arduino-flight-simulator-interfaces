# JET_RANGER_DUAL_STEPPER_CONTROLLER — Program Summary

> **Fork in progress:** this sketch started as a straight copy of
> [`JET_RANGER_STEPPER_CONTROLLER.ino`](../JET_RANGER_STEPPER_CONTROLLER/PROGRAM_SUMMARY.md)
> for a new PCB revision ("Jet Ranger X Dual Stepper" under `PCBs/`). It
> is a single-sketch folder — unlike the original folder, there is no
> sibling `A10_LEFT_CONSOLE_INPUT_CONTROLLER_A` input-controller sketch
> here. Despite the "Dual" name, no second/doubled *stepper* gauge exists
> yet — this is still a 13-gauge stepper roster, with two steppers
> (previously Radar Alt and Torque) repurposed for new gauges, its own
> separately hand-measured RPME/RPMR calibration, a revived Altimeter
> (now reachable via both DCS-BIOS and UDP, at a new conversion factor),
> and Gas Producer (N1) traded away to make room for it. The one piece
> that *does* now touch "Dual" hardware: two I2C OLED displays (Baro +
> Altimeter) switched through a TCA9548A multiplexer — initialised at
> boot but not yet wired to live data.

## What's different from `JET_RANGER_STEPPER_CONTROLLER.ino`

1. **Board identity and network address changed** so this board can
   coexist on the network alongside the original:
   - `BoardName`: `"Jet Ranger Steppers"` → `"Jet Ranger Dual Steppers"`
   - MAC: `A8:61:0A:67:83:69` → `A8:61:0A:67:83:6A` (last byte only)
   - Static IP: `172.16.1.105` → `172.16.1.106`
2. **The stepper on pins 32-35 was Radar Alt; it's now Fuel Load.**
   `RadarAltStepper` → `FuelLoadStepper` throughout (declaration,
   `setMaxSpeed`/`setAcceleration`, startup swing, `.run()` call). The
   pins themselves are unchanged — a new `STEPPER_FL_COIL_A..D` define
   set was added pointing at the same 32/33/34/35 as the now-orphaned
   `RADAR_ALT_COIL_A..D` (left declared but unused).
   - `AGL_FT_TABLE`/`aglFtToSteps()`/`setAGL()` were removed entirely —
     that was a real, hand-measured radar-altitude calibration table,
     not something that could be relabeled for fuel load.
   - The `AGL`/`AGLRAW` UDP codes are gone. In their place: **`FUELLOAD`**
     — raw steps only, since no real fuel-load calibration table exists
     yet. Behaves the way `AGL` did before it had real calibration.
   - The startup swing block (still `if (false)`-disabled, matching the
     original's current state) was relabeled "Fuel Load" but is
     otherwise byte-for-byte the same X27-style wind/zero/3-swing-loop
     sequence.
3. **The stepper on pins 36-39 was Torque; it's now Electrical Load.**
   Same pattern as the Fuel Load conversion above: `ETstepper` →
   `ElectricalLoadStepper` throughout (declaration, `setMaxSpeed`/
   `setAcceleration`, `.run()` call). Pins unchanged — `EL_COIL_A..D`
   points at the same 36/37/38/39 the old `ET_COIL_A..D` used (that
   define name no longer exists, cleanly renamed rather than left
   orphaned this time).
   - The `TQ` UDP code is gone. In its place: **`ELECTRICALLOAD`** — raw
     steps only, since (like Fuel Load) no real calibration table exists
     yet for this gauge.
4. **`TS_PCT_TABLE`/`RS_PCT_TABLE` (RPME/RPMR) now use this board's own,
   separately hand-measured calibration** - no longer identical to
   `JET_RANGER_STEPPER_CONTROLLER.ino`'s tables. Only 4 points given
   (vs. the original's 13), and the 0-point is *assumed* here (not
   directly measured), same convention as `IAS_KT_TABLE`'s assumed 0kt
   row:

   | pct | step |
   |---|---|
   | 0 (assumed) | 0 |
   | 55 | 300 |
   | 100 | 535 |
   | 110 | 600 |

   Range is also now `0..110`, not `0..117` - values above 110% clamp to
   600 steps rather than extrapolating. `tsPctToSteps()`/`rsPctToSteps()`
   (the interpolation functions) are unchanged; only the table data
   differs. The startup-swing comments referencing the old table's
   "630 steps at 117%" were updated to this table's "600 steps at 110%".
5. **`ALTstepper` (Altimeter) has been revived - it's active on this
   board.** On the original sketch (and on this fork, until this change)
   `ALTstepper` was fully commented out - construct, `setMaxSpeed`/
   `setAcceleration`, `.run()`, and its startup homing block all dead
   code. Here it's real again:
   - New pins: `STEPPER_ALT_A..D` = 40/41/42/43 (FULL4WIRE), plus
     `ALTzeroSensePin` = `A15`. **These reuse `GPstepper`'s old pins
     (40-43)** - see point 6 below, GP was fully disabled to make room.
   - New speed constants: `ALT_STEPPER_MAX_SPEED` (600),
     `ALT_STEPPER_ACCELERATION` (600) - both much slower than every other
     stepper's shared `STEPPER_MAX_SPEED`/`STEPPER_ACCELERATION` (9000/
     1000). A third, `ALT_STEPPER_ZERO_SEEK_SPEED` (100), is declared but
     never referenced anywhere - dead constant.
   - The ALT startup block (previously entirely commented out) is now
     active and unconditional (not wrapped in `if (false)`): seeks
     `ALTzeroSensePin` LOW by moving `-STEPS * 2` (`STEPS` = `315 * 16` =
     5040, an old geared-DRIVER-era constant - `-STEPS*2` = **-10080
     steps**). On finding zero it sets `setCurrentPosition(-25)` (not `0`
     - unexplained). It then does a single round trip to
     `Z27_360_FULLWIRE_STEPS * No_Of_Altimeter_Startup_Loops` (720 * 10 =
     **7200 steps**) and back, with the comment now correctly saying
     "720 steps per loop" (was a stale "5760" left over from the pre-fork
     `feet * 5.76` conversion - see the ALT conversion-factor change
     below). Now that `Z27_360_FULLWIRE_STEPS` (720) is confirmed to mean
     "one full 1000ft-dial revolution" (see below), the round-trip move
     reads as an intentional ~10-revolution self-test sweep, not
     obviously a scale bug. The zero-seek move is the part still worth
     double-checking, though: `-STEPS*2` (-10080, using the old
     geared-driver `STEPS` constant) is ~14x a single 720-step revolution
     - a wide search range that should just stop early once
     `ALTzeroSensePin` trips, but hasn't been bench-confirmed as
     intentional rather than a leftover unconverted value.
   - `ALTstepper` is driven by DCS-BIOS (`onAltMslFtChange()` /
     `DcsBios::IntegerBuffer altMslFtBuffer`, `CommonData_ALT_MSL_FT`).
     **Its conversion factor has also been changed: `feet * 5.76` →
     `feet * 0.72`** (comment updated from "5760 Steps per 1000 feet" to
     "720 Steps per 1000 feet") - this now matches the new
     `Z27_360_FULLWIRE_STEPS` (720) constant, i.e. one full 1000ft dial
     sweep is one full revolution of this stepper. `JET_RANGER_STEPPER_CONTROLLER.ino`'s
     own (still fully commented-out) ALT handling remains at the old
     `5.76` factor, so this is a genuine, deliberate divergence between
     the two sketches, not just "revived unchanged." **Additionally,
     unlike the original sketch, this board's UDP `"ALT"`/`"ALTRAW"`
     cases have been enabled** (they remain commented out on
     `JET_RANGER_STEPPER_CONTROLLER.ino`) - `ALT` calls
     `onAltMslFtChange()` directly with the UDP feet value, `ALTRAW`
     bypasses the conversion entirely. `StepperVSITester`'s ALT trackbar
     now broadcasts to both boards and can drive this one too.
6. **`GPstepper` (N1/Gas Producer) has been fully disabled** to free its
   pins (40-43) for `ALTstepper` above: `GP_COIL_A..D` defines,
   the `GPstepper` construct, its `setMaxSpeed`/`setAcceleration`/
   `.run()` calls, and `gpPctToSteps()`/`setGP()` are all now commented
   out. The `N1`/`N1RAW` UDP cases are gone entirely (not just
   commented - removed from the `HandleOutputValuePair()` chain), so
   sending either from `StepperVSITester` is now a silent no-op on this
   board, same as `FLAPS`/`AOA`/`GFORCE`/`SPDMAX` already are.
7. **`X27_FULLWIRE_STEPS` changed from 635 to 630**, now derived from a
   new `FULL4WIRE_STEPS` (315) base constant rather than a bare literal.
   `FULL4WIRE_HOMING_STEPS` is redefined too: was `315 + 5` (320), now
   `FULL4WIRE_STEPS + 1` (316). `X27_FULLWIRE_HOMING_STEPS`'s formula is
   unchanged (`X27_FULLWIRE_STEPS + 1`) but now evaluates to 631, not
   636, since its base moved. This shifts every X27-style
   stepper's full-scale/homing range slightly (VSI, IAS, Fuel Load,
   Turbine Speed, Rotor Speed all reference these constants).
8. **Turbine Speed's and Rotor Speed's startup swings are now wrapped in
   `if (false)` - disabled.** These were the only two active (non-`if
   (false)`) startup swings on this board before; now every startup
   swing except the new ALT block (point 5) is disabled, including
   VSI's, IAS's, and Fuel Load's (all already `if (false)` from earlier).
   Both swings' internal sequence also changed: the second
   `runToNewPosition()` call is now `0` instead of `-X27_FULLWIRE_STEPS`
   (moot while disabled, but changes behaviour if ever re-enabled).
   IAS's startup swing (already disabled) got the same `0`-instead-of-
   `-X27_FULLWIRE_STEPS` change.
9. **A substantial new subsystem: two I2C OLED displays (Barometer +
   Altimeter digit readout), switched through a TCA9548A I2C
   multiplexer.** This is the first code on this board that actually
   touches the "Dual" in the PCB's name ("Jet Ranger X Dual Stepper" has
   a TCA9548A footprint) - previously nothing in this sketch used I2C at
   all. New includes: `U8g2lib.h`, `Wire.h`, `utility/twi.h` (for I2C bus
   scanning). Adds ~510 lines and ~12KB flash / ~1.2KB RAM (this is the
   single largest change reviewed on this board so far).
   - Hardware: two `U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C` displays
     (`u8g2_BARO`, `u8g2_ALT`), both hardware-I2C (Mega's pins 20/21,
     SDA/SCL - no dedicated pin `#define`s needed), reached through a
     TCA9548A mux at I2C address `0x70` (`TCAADDR`) via `tcaselect(i)`.
     `BARO_OLED_Port` = channel 1, `ALT_OLED_Port` = channel 2.
   - `setup()` scans all 8 TCA9548A channels for I2C devices (logs each
     found address via `SendDebug()`), then initialises both displays,
     picks fonts (`u8g2_font_logisoso16_tf` for BARO,
     `u8g2_font_logisoso32_tn` for ALT), and renders one-time default
     values: `updateBARO("2992")` (a standard altimeter-setting default)
     and `updateALT("0", "0")` (zeroed altitude).
   - `updateBARO()`, `buildBAROString()`, `updateALT()`, and
     `UpdateAltimeterDigits()` (a ~130-line function with bitmap glyph
     data, e.g. a `hash_bits[]` array, presumably for a digit-drum-style
     rolling altitude counter similar to `JET_RANGER_OLED_CONTROLLER`'s
     approach) all exist as callable functions, but **only the two
     `setup()`-time calls above ever run them** - `UpdateAltimeterDigits()`,
     `buildBAROString()`, and the `BaroUpdated`/`AltCounterUpdated`/
     `CurrentDisplay` state flags are all currently dead: declared/set,
     never read or called from `loop()` or any DCS-BIOS/UDP callback.
     **Both displays currently just show their static boot-splash values
     forever** - nothing wires live baro or altitude data to them yet.

Everything else — VSI, IAS, the 8 remaining `Stepper-Tuning-Harness`-ported
gauges (`EOTstepper`/`XOTstepper`/`XOPstepper`/`EGTstepper`/`TSstepper`/
`RSstepper`/`FAstepper`/`EOPstepper`), SARI, the health keepalive, and the
remaining real calibration tables (`VSI_FPM_TABLE`, `IAS_KT_TABLE`) —
is currently identical to the original sketch. See that sketch's own
summary for the full program flow, roster, cautions, and pin/network
tables; only the deltas above and the roster/UDP table changes below are
called out here.

## Build verification

Compiled with `arduino-cli` (target `arduino:avr:mega:cpu=atmega2560`),
**0 errors**:

| Sketch | Flash | RAM |
|---|---|---|
| `JET_RANGER_DUAL_STEPPER_CONTROLLER.ino` | 38,704 bytes (15%) | 4,739 bytes (57%) |

Flashed to a Mega on **COM4** (also previously flashed to COM13 - this
board has moved between physical Megas/ports across bench sessions;
confirm which one is actually wired to the panel before trusting either
port number).

## Current stepper roster (deltas only)

| Stepper | Interface/pins | Status |
|---|---|---|
| `FuelLoadStepper` (was `RadarAltStepper`) | FULL4WIRE, `STEPPER_FL_COIL_A..D` (32/33/34/35, wired C,D,A,B — same physical pins the original sketch used for Radar Alt) | Raw steps only (`FUELLOAD` code) — no real calibration table yet. Boot startup/swing exists (X27-style, 3 loops) but is wrapped in `if (false)` — disabled, matching the original sketch's current Radar Alt swing state |
| `ElectricalLoadStepper` (was `ETstepper`) | FULL4WIRE, `EL_COIL_A..D` (36/37/38/39 — same physical pins the original sketch used for Torque) | Raw steps only (`ELECTRICALLOAD` code) — no real calibration table yet, no startup swing (the original's `ETstepper` never had one either) |
| `TSstepper` (`RPME`, Turbine/Engine Speed) | Unchanged pins/interface | Real calibration, but via **this board's own** 4-point `TS_PCT_TABLE` (0/55/100/110% → 0/300/535/600 steps) - diverges from the original sketch's 13-point table. Startup swing now `if (false)`-disabled (was active) |
| `RSstepper` (`RPMR`, Rotor Speed) | Unchanged pins/interface | Same divergence - this board's own 4-point `RS_PCT_TABLE`, identical values to this board's `TS_PCT_TABLE`. Startup swing now `if (false)`-disabled (was active) |
| `ALTstepper` (Altimeter) | FULL4WIRE, `STEPPER_ALT_A..D` (40/41/42/43) + `ALTzeroSensePin` (A15) - **revived**, was fully commented out on both this board and the original | Active - DCS-BIOS (`onAltMslFtChange()`, `feet * 0.72` - changed from the original sketch's `5.76`, see above) AND UDP `ALT`/`ALTRAW` (enabled here, unlike the original sketch where both remain commented out). Unconditional startup homing block (not `if (false)`) - see caution above about the `-STEPS*2` (-10080 step) zero-seek move |
| `GPstepper` (`N1`, Gas Producer) | — | **Disabled.** Fully commented out (construct, `setMaxSpeed`/`setAcceleration`/`.run()`, `gpPctToSteps()`/`setGP()`) to free pins 40-43 for `ALTstepper` above. `N1`/`N1RAW` UDP cases removed - silent no-op if sent |

All other steppers (`VSIstepper`, `IASstepper`, `EOTstepper`,
`XOTstepper`, `XOPstepper`, `EGTstepper`, `FAstepper`, `EOPstepper`,
`SARIstepperRoll`, `saiPitch`) are unchanged from the original sketch —
see its summary for their status.

## UDP codes (deltas only)

| Code | Real value | Board behaviour |
|---|---|---|
| `FUELLOAD` | raw steps | `FuelLoadStepper.moveTo()` directly — no calibration table |
| `ELECTRICALLOAD` | raw steps | `ElectricalLoadStepper.moveTo()` directly — no calibration table |
| `RPME` | %, 0-110 (was 0-117) | `setTS()` via this board's own `TS_PCT_TABLE` (4 points, not 13) |
| `RPMR` | %, 0-110 (was 0-117) | `setRS()` via this board's own `RS_PCT_TABLE` (4 points, not 13) |
| `ALT` | feet | `onAltMslFtChange()` via `feet * 0.72` (was `5.76` before this board's fork - see above) - **enabled here**, unlike the original sketch (still commented out there, still at `5.76`) |
| `ALTRAW` | raw steps | `ALTstepper.moveTo()` directly, bypassing the `feet * 0.72` conversion - also enabled here only |

`AGL`/`AGLRAW`, `TQ`, and now `N1`/`N1RAW` no longer exist on this board
(see above - `N1`/`N1RAW` were removed when `GPstepper` was disabled).
Every other code (`VSI`/`VSIRAW`, `IAS`/`IASRAW`, `OILT`/`OILP`/`XMSNT`/
`XMSNP`/`ITT`/`FUEL` and their `*RAW` siblings) is unchanged from the
original sketch. `RPMERAW`/`RPMRRAW` (raw-step bypass) also unchanged.

## Local network configuration

| Setting | Value |
|---|---|
| Static IP | `172.16.1.106` (was `.105` on the original sketch — deliberately different so both boards can be on the network at once) |
| MAC | `A8:61:0A:67:83:6A` (was `...69`) |
| Local port `localport`/`keyboardport` | 7788 |
| Local port `localdebugport` | 7795 (declared, not bound) |
| Local port `MSFSport` | 13136 |
| Local port `aliveport` | 13137 (health keepalive — see below) |

## Remote endpoints this sketch talks to

Same as the original sketch, including the health keepalive added there
this session:

| Target | Port | Purpose |
|---|---|---|
| `172.16.1.10` (reflector host) | 27000 | Debug/log messages via `SendDebug()` |
| `172.16.1.110` (`targetIP`) | 7788 / 7789 | `SendIPString()`/`SendLedString()` — declared, no callers |
| `172.16.1.10` (`MSFSIP`) | 7791 | `SendMSFSMessage()` — declared, no callers |
| `reflectorIP` (172.16.1.10, `JetRangerHealthMonitor`'s host) | 13137 | Health keepalive — bare `"DUAL_STEPPER"` string sent every 10s. Deliberately distinct from the original sketch's `"STEPPER"` prefix, so `JetRangerHealthMonitor` (which matches by message prefix only, not sender IP) can tell the two boards apart — see that app's own summary, which now has a dedicated "Dual Stepper" indicator for this. |

## Outstanding / not yet done

- No real calibration table for `FUELLOAD` or `ELECTRICALLOAD` — both
  currently raw steps only.
- `RADAR_ALT_COIL_A..D` defines are still present but unused (dead code,
  left behind by the `FuelLoadStepper` pin-define rename). `ET_COIL_A..D`
  was cleanly renamed rather than left orphaned, so no equivalent dead
  code exists for the Electrical Load conversion.
- `TS_PCT_TABLE`/`RS_PCT_TABLE`'s 0-point is assumed, not directly
  measured - not yet confirmed on the bench that RPME/RPMR actually read
  0% at rest rather than clamping to the 55% row.
- `ALTstepper`'s zero-seek move still uses `-STEPS * 2` (-10080 steps,
  `STEPS` = the old `315 * 16` geared-driver constant) - not
  bench-confirmed as intentional, since it's ~14x a single 720-step
  revolution (`Z27_360_FULLWIRE_STEPS`, now confirmed as "one full
  1000ft-dial rotation" via the `feet * 0.72` conversion factor). The
  round-trip move itself (`Z27_360_FULLWIRE_STEPS *
  No_Of_Altimeter_Startup_Loops` = 7200 steps, ~10 revolutions) now reads
  as an intentional self-test sweep rather than a scale bug, now that
  720 is understood to be a full revolution - only the zero-seek's
  `STEPS` constant remains a plausible leftover from the pre-`FULL4WIRE`
  geared-`DRIVER` era.
- `N1`/`N1RAW` (Gas Producer) no longer reach any stepper on this board -
  traded away for `ALTstepper`'s pins. `StepperVSITester`'s Raw Step Test
  dropdown still lists `N1RAW` and broadcasts it to both boards; it's now
  a silent no-op on `172.16.1.106` (was live before this change).
- No second/doubled *stepper* gauge added - the "Dual" PCB concept still
  isn't reflected in the stepper roster, only in the new OLED subsystem.
- The new OLED Baro/Altitude displays are initialised but not wired to
  live data: `UpdateAltimeterDigits()`/`buildBAROString()` are defined
  but never called past `setup()`'s one-time defaults, and
  `BaroUpdated`/`AltCounterUpdated`/`CurrentDisplay` are set but never
  read anywhere. Both screens will show `2992`/`00000` forever until
  something (a DCS-BIOS callback, a UDP code, or `loop()` polling) is
  added to call `updateBARO()`/`updateALT()` with real values.
- OLED I2C wiring hasn't been bench-verified in this repo - the boot-time
  8-channel TCA9548A scan logs found addresses via `SendDebug()`, but
  nothing in this codebase parses that debug output to confirm the mux
  and both displays are actually detected and responding.
