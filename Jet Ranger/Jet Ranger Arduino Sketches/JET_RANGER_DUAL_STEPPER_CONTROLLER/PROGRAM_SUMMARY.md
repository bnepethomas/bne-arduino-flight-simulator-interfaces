# JET_RANGER_DUAL_STEPPER_CONTROLLER — Program Summary

> **Fork in progress:** this sketch started as a straight copy of
> [`JET_RANGER_STEPPER_CONTROLLER.ino`](../JET_RANGER_STEPPER_CONTROLLER/PROGRAM_SUMMARY.md)
> for a new PCB revision ("Jet Ranger X Dual Stepper" under `PCBs/`). It
> is a single-sketch folder — unlike the original folder, there is no
> sibling `A10_LEFT_CONSOLE_INPUT_CONTROLLER_A` input-controller sketch
> here. Despite the "Dual" name, no second/doubled gauge exists yet — so
> far this is the same 13-gauge roster as the original, with two
> steppers (previously Radar Alt and Torque) repurposed for new gauges,
> and its own separately hand-measured RPME/RPMR calibration.

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

Everything else — VSI, IAS, the 9 `Stepper-Tuning-Harness`-ported gauges
(`EOTstepper`/`XOTstepper`/`XOPstepper`/`EGTstepper`/`TSstepper`/
`RSstepper`/`FAstepper`/`GPstepper`/`EOPstepper`), SARI, the health
keepalive, and the remaining real calibration tables (`VSI_FPM_TABLE`,
`IAS_KT_TABLE`) — is currently identical to the original sketch. See
that sketch's own summary for the full program flow, roster, cautions,
and pin/network tables; only the deltas above and the roster/UDP table
changes below are called out here.

## Build verification

Compiled with `arduino-cli` (target `arduino:avr:mega:cpu=atmega2560`),
**0 errors**:

| Sketch | Flash | RAM |
|---|---|---|
| `JET_RANGER_DUAL_STEPPER_CONTROLLER.ino` | 25,532 bytes (10%) | 3,532 bytes (43%) |

Flashed to a Mega on **COM13** (first flash to real hardware - previous
passes were compile-only).

## Current stepper roster (deltas only)

| Stepper | Interface/pins | Status |
|---|---|---|
| `FuelLoadStepper` (was `RadarAltStepper`) | FULL4WIRE, `STEPPER_FL_COIL_A..D` (32/33/34/35, wired C,D,A,B — same physical pins the original sketch used for Radar Alt) | Raw steps only (`FUELLOAD` code) — no real calibration table yet. Boot startup/swing exists (X27-style, 3 loops) but is wrapped in `if (false)` — disabled, matching the original sketch's current Radar Alt swing state |
| `ElectricalLoadStepper` (was `ETstepper`) | FULL4WIRE, `EL_COIL_A..D` (36/37/38/39 — same physical pins the original sketch used for Torque) | Raw steps only (`ELECTRICALLOAD` code) — no real calibration table yet, no startup swing (the original's `ETstepper` never had one either) |
| `TSstepper` (`RPME`, Turbine/Engine Speed) | Unchanged pins/interface | Real calibration, but via **this board's own** 4-point `TS_PCT_TABLE` (0/55/100/110% → 0/300/535/600 steps) - diverges from the original sketch's 13-point table |
| `RSstepper` (`RPMR`, Rotor Speed) | Unchanged pins/interface | Same divergence - this board's own 4-point `RS_PCT_TABLE`, identical values to this board's `TS_PCT_TABLE` |

All other steppers (`VSIstepper`, `IASstepper`, `EOTstepper`,
`XOTstepper`, `XOPstepper`, `EGTstepper`, `FAstepper`, `GPstepper`,
`EOPstepper`, `SARIstepperRoll`, `saiPitch`) are unchanged from the
original sketch — see its summary for their status.

## UDP codes (deltas only)

| Code | Real value | Board behaviour |
|---|---|---|
| `FUELLOAD` | raw steps | `FuelLoadStepper.moveTo()` directly — no calibration table |
| `ELECTRICALLOAD` | raw steps | `ElectricalLoadStepper.moveTo()` directly — no calibration table |
| `RPME` | %, 0-110 (was 0-117) | `setTS()` via this board's own `TS_PCT_TABLE` (4 points, not 13) |
| `RPMR` | %, 0-110 (was 0-117) | `setRS()` via this board's own `RS_PCT_TABLE` (4 points, not 13) |

`AGL`/`AGLRAW` and `TQ` no longer exist on this board (see above). Every
other code (`VSI`/`VSIRAW`, `IAS`/`IASRAW`, `OILT`/`OILP`/`XMSNT`/`XMSNP`/
`ITT`/`N1`/`FUEL` and their `*RAW` siblings) is unchanged from the
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
- No second/doubled gauge of any kind added — the "Dual Stepper" PCB
  concept isn't reflected in this sketch's stepper count yet.
- `RADAR_ALT_COIL_A..D` defines are still present but unused (dead code,
  left behind by the `FuelLoadStepper` pin-define rename). `ET_COIL_A..D`
  was cleanly renamed rather than left orphaned, so no equivalent dead
  code exists for the Electrical Load conversion.
- `TS_PCT_TABLE`/`RS_PCT_TABLE`'s 0-point is assumed, not directly
  measured - not yet confirmed on the bench that RPME/RPMR actually read
  0% at rest rather than clamping to the 55% row.
