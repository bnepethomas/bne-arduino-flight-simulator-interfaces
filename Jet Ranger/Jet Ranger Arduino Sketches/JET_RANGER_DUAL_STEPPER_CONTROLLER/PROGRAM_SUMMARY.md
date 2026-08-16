# JET_RANGER_DUAL_STEPPER_CONTROLLER — Program Summary

> **Fork in progress:** this sketch started as a straight copy of
> [`JET_RANGER_STEPPER_CONTROLLER.ino`](../JET_RANGER_STEPPER_CONTROLLER/PROGRAM_SUMMARY.md)
> for a new PCB revision ("Jet Ranger X Dual Stepper" under `PCBs/`). It
> is a single-sketch folder — unlike the original folder, there is no
> sibling `A10_LEFT_CONSOLE_INPUT_CONTROLLER_A` input-controller sketch
> here. Despite the "Dual" name, no second/doubled gauge exists yet —
> this is still a 13-gauge roster, with two steppers (previously Radar
> Alt and Torque) repurposed for new gauges, its own separately
> hand-measured RPME/RPMR calibration, a revived Altimeter (now reachable
> via both DCS-BIOS and UDP), and Gas Producer (N1) traded away to make
> room for it.

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
     steps**, a scale wildly larger than every other FULL4WIRE stepper's
     0-635-ish range in this file; worth double-checking this is actually
     intended for a FULL4WIRE stepper before trusting it on the bench).
     On finding zero it sets `setCurrentPosition(-25)` (not `0` - also
     unexplained). It then does a single round trip to
     `Z27_360_FULLWIRE_STEPS * No_Of_Altimeter_Startup_Loops` (a new pair
     of constants - `Z27_360_FULLWIRE_STEPS` = 720, a different X27-style
     full-scale figure than `X27_FULLWIRE_STEPS`'s 630;
     `No_Of_Altimeter_Startup_Loops` = 10, oddly `#define`d *inside* the
     loop body rather than at file scope) = **7200 steps** and back. This
     replaced an earlier `X27_FULLWIRE_STEPS * 3` (1890 steps) version -
     the previously-stale "5760 steps per loop" comment has now been
     corrected to "720 steps per loop" (matching
     `Z27_360_FULLWIRE_STEPS`), though the "per loop" framing is still a
     bit misleading since this is one `runToNewPosition()` call to
     7200, not an actual 10-iteration loop.
   - `ALTstepper` is driven by DCS-BIOS (`onAltMslFtChange()` /
     `DcsBios::IntegerBuffer altMslFtBuffer`, `CommonData_ALT_MSL_FT`,
     `feet * 5.76` steps), revived unchanged from the original sketch's
     own ALT handling. **Additionally, unlike the original sketch, this
     board's UDP `"ALT"`/`"ALTRAW"` cases have been enabled** (they
     remain commented out on `JET_RANGER_STEPPER_CONTROLLER.ino`) - `ALT`
     calls `onAltMslFtChange()` directly with the UDP feet value,
     `ALTRAW` bypasses the conversion entirely. `StepperVSITester`'s ALT
     trackbar now broadcasts to both boards and can drive this one too.
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
| `JET_RANGER_DUAL_STEPPER_CONTROLLER.ino` | 26,026 bytes (10%) | 3,508 bytes (42%) |

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
| `ALTstepper` (Altimeter) | FULL4WIRE, `STEPPER_ALT_A..D` (40/41/42/43) + `ALTzeroSensePin` (A15) - **revived**, was fully commented out on both this board and the original | Active - DCS-BIOS (`onAltMslFtChange()`, `feet * 5.76`) AND UDP `ALT`/`ALTRAW` (enabled here, unlike the original sketch where both remain commented out). Unconditional startup homing block (not `if (false)`) - see caution above about the `-STEPS*2` (-10080 step) zero-seek move, a geared-driver-scale value on a FULL4WIRE stepper |
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
| `ALT` | feet | `onAltMslFtChange()` via `feet * 5.76` - **enabled here**, unlike the original sketch (still commented out there) |
| `ALTRAW` | raw steps | `ALTstepper.moveTo()` directly, bypassing the `feet * 5.76` conversion - also enabled here only |

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
- `ALTstepper`'s zero-seek move (`-STEPS * 2` = -10080 steps, `STEPS` =
  the old `315 * 16` geared-driver constant) and its subsequent round
  trip (`Z27_360_FULLWIRE_STEPS * No_Of_Altimeter_Startup_Loops` = 720 *
  10 = 7200 steps) are both on a completely different scale than every
  other FULL4WIRE stepper's homing move in this file (0-635-ish) - not
  bench-confirmed this is intentional rather than a leftover from before
  `ALTstepper` moved off a geared `DRIVER` interface onto `FULL4WIRE`.
  (The comment stale-ness itself has been fixed - "Send Alt Round 10
  times"/"720 steps per loop" now matches the code - but the underlying
  step-scale question remains open.)
- `N1`/`N1RAW` (Gas Producer) no longer reach any stepper on this board -
  traded away for `ALTstepper`'s pins. `StepperVSITester`'s Raw Step Test
  dropdown still lists `N1RAW` and broadcasts it to both boards; it's now
  a silent no-op on `172.16.1.106` (was live before this change).
- No second/doubled gauge of any kind added — the "Dual Stepper" PCB
  concept isn't reflected in this sketch's stepper count yet.
