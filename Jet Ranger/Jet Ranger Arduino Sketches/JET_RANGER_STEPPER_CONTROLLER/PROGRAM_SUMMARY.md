# JET_RANGER_STEPPER_CONTROLLER — Program Summary

> **Board identity resolved:** this sketch's header and `BoardName` used
> to identify it as `A10_FRONT_CONSOLE_STEPPERS`/"A10 Forward Steppers"
> (an A-10C Warthog board), even though it's part of the Jet Ranger
> fleet. That's since been fixed by hand: the header now says
> `JET_RANGER_STEPPERS`/"JET RANGER STEPPER CONTROLLER" and `BoardName`
> is `"Jet Ranger Steppers"`. Its data source is still a mix of DCS-BIOS
> (serial, for the A-10C SimVars still wired up) and the Jet Ranger's
> `172.16.1.x` UDP/CSV protocol.

This folder holds one sketch, **`JET_RANGER_STEPPER_CONTROLLER.ino`**, which drives
the analogue stepper-motor gauges plus backlighting. It started as an A-10C front
instrument panel board with 7 gauges; 5 of the original 7 (`ALT`, `Flaps`, `AOA`,
`G-Force`, `Max Airspeed`) have been removed or commented out by hand,
`Current Airspeed` was renamed `IASstepper`, and Bell 206-style gauges were added
(see below).

> **Current values (checked against the sketch, 2026-10-18):** `SwingLoops` is 2 and
> all seven swing gates (`SwingVSI`, `SwingIAS`, `SwingAGL`, `SwingTQ`, `SwingFA`,
> `SwingEGT`, `SwingGP`) are `true`, so every gauge swings at boot (statements below
> that say VSI, IAS or AGL default to `false` describe an earlier state). Radar
> altitude (`AGL`) now has a real calibration: `AGL_FT_TABLE`, 11 points from 0 ft = 0
> steps to 2500 ft = 469 steps, so `AGL` is feet, not raw steps. `VSI_FPM_TABLE` has 11
> points (-1750 fpm = -311 steps to +1750 fpm = 315 steps) and `IAS_KT_TABLE` 10 points
> (0 to 140 kt = 0 to 635 steps). Zero trims: `AGLZeroOffset` 10, `TQZeroOffset` 23,
> all others 0.

## Build verification

The sketch was compiled with `arduino-cli` (target
`arduino:avr:mega:cpu=atmega2560`) with **0 errors** at 29,734 bytes of flash (11%)
and 3,981 bytes of RAM (48%) when this was last measured.

Flashed to the bench Mega on COM4 several times across this sketch's
history; most recently after the real `IAS_KT_TABLE` calibration was
added. `SendRetransmissionTimeout` note: `Ethernet.setRetransmissionTimeout(10)`
was added right after `Ethernet.begin()` - the W5500's own ARP/send retry
timeout (default 200ms × 8 retries = up to 1600ms blocked inside a UDP
send that fails to get an ARP reply) is now capped at 10ms/retry
(~80ms worst case), separate from the one-time boot-time
`delayBeforeSendingPacket` wait (unchanged, still 2000ms).

Library versions used for this verification: **Ethernet** 2.0.2,
**AccelStepper** 1.64.0, **DCS-BIOS** 0.3.13 (Arduino Library Manager).

## Current stepper roster

| Stepper | Interface/pins | Status |
|---|---|---|
| `VSIstepper` | FULL4WIRE, `COIL_VSI_A..D` (7/8/9/11, wired C,D,A,B) | Active - real `VSI_FPM_TABLE` calibration, boot homing gated by `SwingVSI` (default `false`, so currently disabled - see selective-swing note below) |
| `IASstepper` (renamed from `SpeedCurrentstepper`) | FULL4WIRE, `STEPPER_SPD_A..D` (12/13/22/23, wired C,D,A,B) | Active - real `IAS_KT_TABLE` calibration (see below); its own boot startup/swing exists but is gated by `SwingIAS` (default `false`) - previously a bare `if (false)`, now a named toggle with identical default behaviour |
| `RadarAltStepper` | FULL4WIRE, `RADAR_ALT_COIL_A..D` (32/33/34/35, wired C,D,A,B) | Active, raw steps only (`AGL` code) - no real calibration yet. Boot startup/swing gated by `SwingAGL` (default `false`) - previously a bare `if (false)` whose own in-code comment incorrectly claimed it ran every boot; that stale claim was corrected when the named gate was added |
| `EOTstepper`, `XOTstepper`, `XOPstepper`, `EOPstepper` | FULL4WIRE, dedicated coil pins (see the pin table) | Active, real-unit UDP codes (see table below), all still using the placeholder `FULL4WIRE_HOMING_STEPS` linear scale (see caution below), no boot startup/swing at all |
| `GPstepper` (Gas Producer) | FULL4WIRE, `GP_COIL_A..D` (40/41/42/43) | Real percent now via `setGP()`/`GP_PCT_TABLE` (`N1` code) — 3-point bench-measured table, all rows directly measured (0%→15, 100%→550, 103%→583 steps; uses the shared `PctToStepEntry` struct, also used by the other percent table). Unlike most tables in this sketch, 0% is NOT the stepper's raw zero - the boot swing's "return to zero" step accounts for this (`gpPctToSteps(0) + GPZeroOffset`, not just `GPZeroOffset`, same situation as `FAstepper`'s swing above). Boot startup/swing active, gated by `SwingGP` (default `true`) - overshoots the real 583-step calibrated max, same as every other graduated gauge's swing in this sketch |
| `EGTstepper` (EGT/ITT) | FULL4WIRE, `EGT_COIL_A..D` (A11-A14) | Real degrees C now via `setEGT()`/`EGT_C_TABLE` (`ITT` code) — 7-point bench-measured table (100C→0, 300C→44, 500C→76, 600C→102, 700C→243, 800C→401, 900C→493 steps; reuses the new `CToStepEntry` struct). No 0C row was measured - any tempC ≤100 clamps to the 100C row's 0 steps, same effective result. Boot startup/swing active, gated by `SwingEGT` (default `true`) - overshoots the real 493-step calibrated max, same as every other graduated gauge's swing in this sketch. `egtCToSteps(0)` still clamps to 0 steps, so `EGTZeroOffset` alone remains the correct calibrated-zero target for the swing's return-to-zero step, unlike `FAstepper`'s swing above |
| `TSstepper`, `RSstepper` (Turbine / Rotor Speed) | — | **Removed from this sketch.** Their steppers, coil pins (24-31), `SwingRPM` gate, `TSZeroOffset`/`RSZeroOffset`, `TS_PCT_TABLE`/`RS_PCT_TABLE`, `setTS()`/`setRS()`, the `RPME`/`RPMERAW`/`RPMR`/`RPMRRAW` UDP cases, and their `ResetGaugesToZero()`/`.run()` calls are all gone - those gauges are driven by `JET_RANGER_OLED_DUAL_STEPPER_CONTROLLER.ino` (172.16.1.106). `RPME`/`RPMR` packets reaching this board are now silently ignored (unknown code). The shared `PctToStepEntry` struct was kept (still used by `GP_PCT_TABLE`/`TQ_PCT_TABLE`) |
| `TQstepper` (renamed from `ETstepper`, outside this doc pass) | FULL4WIRE, `ET_COIL_A..D` (36/37/38/39) | Real percent now via `setTQ()`/`TQ_PCT_TABLE` (`TQ` code) — 2-point bench-measured table (0%→0, 120%→600 steps; uses the shared `PctToStepEntry` struct, also used by the other percent table). `TQRAW` added as the raw-step bypass. Boot startup/swing active, gated by `SwingTQ` (default `true`) - previously had no startup routine at all. Uses the same `X27_FULLWIRE_STEPS`/`X27_FULLWIRE_HOMING_STEPS` range as VSI/IAS/Radar Alt's swings — now a reasonable match rather than an arbitrary range, since the real 120% max (600 steps) is close to `X27_FULLWIRE_STEPS` (635) |
| `FAstepper` (Fuel Quantity) | FULL4WIRE, dedicated coil pins (see the pin table) | Real US gallons now via `setFA()`/`FA_GAL_TABLE` (`FUEL`/`FA` code) — 2-point table, both rows bench-measured (0 gal→53 steps, 75 gal→504 steps; reuses the new `GalToStepEntry` struct). Unlike most tables in this sketch, 0 gal is NOT the stepper's raw zero - the boot swing's "return to zero" step accounts for this (`galToSteps(0) + FAZeroOffset`, not just `FAZeroOffset`). `FUELRAW` already existed as the raw-step bypass. Boot startup/swing active, gated by `SwingFA` (default `true`) - overshoots the real 504-step calibrated max, same as every other graduated gauge's swing in this sketch |
| `ALTstepper`, `SpeedMaxstepper`, `FlapsStepper`, `AOAstepper`, `GForcestepper` | — | **Removed.** Constructs, pin `#define`s (mostly), startup routines, DCS-BIOS bindings, and UDP codes for all five are commented out or deleted. `FlapsStepPin`/`FlapsDirectionPin` are the one pair of pin `#define`s left behind, now orphaned (nothing reads them). |
| `SARIstepperRoll` | DRIVER, pins 30/32 | Declared and pin-claimed, but its `Nema8Stepper` binding is commented out - never `.run()`, never bound to DCS-BIOS. Still occupies pins 30/32 via its `AccelStepper` constructor. |
| `saiPitch` (`DcsBios::ServoOutput`, pin 9) | — | Active - SAI pitch axis, plain hobby servo |

> **Caution — `FULL4WIRE_HOMING_STEPS` redefined:** was `315 * 2` (630),
> now `315 + 5` (320) — halves the effective step range every
> still-uncalibrated gauge above (`EOT`/`XOT`/`XOP`/`EGT`/`GP`/`FA`)
> scales its real-unit range onto, since none of them have their own
> bench-measured ceiling yet. Their in-code comments hadn't been updated
> to match until this pass.
>
> **Caution — VSI homing macro-precedence bug:** two new constants,
> `X27_FULLWIRE_STEPS` (635) and `X27_FULLWIRE_HOMING_STEPS`
> (`X27_FULLWIRE_STEPS + 5`, unparenthesized), now drive VSI's homing.
> `VSIstepper.runToNewPosition(-X27_FULLWIRE_HOMING_STEPS)` expands to
> `-635 + 5` = **-630**, not `-(635+5)` = -640 as the name implies - a
> small (~1.5%) but real discrepancy between the code's apparent and
> actual behavior.
>
> **Caution — `AllstepperEnablePin` pinMode vs digitalWrite:**
> `pinMode(AllstepperEnablePin, OUTPUT)` is commented out in `setup()`,
> but `digitalWrite(AllstepperEnablePin, false)` right after it is still
> active. An Arduino pin defaults to `INPUT` at boot, so that
> `digitalWrite()` likely just toggles the pin's internal pull-up rather
> than actually driving it LOW as intended.

## Newly surfaced pin collisions (SARI)

Reviewing the current pin map turned up a collision with the
still-pin-claiming (if otherwise inactive) `SARIstepperRoll` that weren't
caught in earlier passes:

| Gauge pin | Collides with |
|---|---|
| `RADAR_ALT_COIL_A` (32) | `SARIdirectionPin` |

Removing `ALTstepper`/`SpeedMaxstepper`/`FlapsStepper`/`AOAstepper`/
`GForcestepper` resolved every collision documented in earlier passes of
this summary (their pin `#define`s are now mostly gone entirely, not
just unused) — the one exception is `EOT_COIL_D` (A2/56), which still
collides with `AllstepperEnablePin`.

## JET_RANGER_STEPPER_CONTROLLER.ino — Program flow

1. **Setup**: flashes status LEDs, brings up Ethernet (static IP,
   10ms retransmission timeout - see build verification above), ramps
   `BACK_LIGHTS`. Each stepper's boot wind/zero/swing self-test is gated
   by its own named boolean (`SwingVSI`/`SwingIAS`/`SwingAGL`/`SwingTQ`/`SwingFA`/
   `SwingEGT`/`SwingGP`, with a shared `SwingLoops` loop count) — same selective-swing
   pattern `JET_RANGER_OLED_DUAL_STEPPER_CONTROLLER.ino` already used,
   ported here to replace this sketch's previous mix of ad-hoc
   `if (false)` gates (VSI/IAS/Radar Alt). Defaults preserve prior
   behaviour exactly: `SwingVSI`/`SwingIAS`/`SwingAGL` = `false`. (The
   old `SwingRPM` gate for Turbine/Rotor Speed was removed along with
   those steppers.) **Boot swing order** in `setup()`: Fuel
   Quantity (`FAstepper`) → Torque (`TQstepper`) → EGT/ITT
   (`EGTstepper`, "Turbine Out") → Gas Producer (`GPstepper`) → VSI → IAS →
   AGL (`RadarAltStepper`); each still runs only if its `SwingXXX` gate is
   `true`. Four
   more gates were added afterward for gauges that never had a startup
   swing at all before, so there was no prior behaviour to preserve for
   any of them: `SwingTQ` (default `true`) for `TQstepper` (Torque),
   `SwingFA` (default `true`) for `FAstepper` (Fuel Quantity),
   `SwingEGT` (default `true`) for `EGTstepper` (EGT/ITT), and `SwingGP`
   (default `true`) for `GPstepper` (Gas Producer). `ALT`/`Flaps`/`AOA`/
   `G-Force` startup blocks are all commented out (not gated -
   those steppers don't exist in this sketch any more). Starts DCS-BIOS,
   sets running-brightness backlighting.
2. **Main loop** (`loop()`): toggles status LEDs; `DcsBios::loop()` is
   commented out (DCS-BIOS callbacks are registered but never pumped, so
   none fire from a live serial link in this build); `updateSteppers()`
   calls `.run()` on `VSIstepper`, `IASstepper`, `RadarAltStepper`, and
   the newer FULL4WIRE gauges (`ALTstepper` etc. excluded, since they
   no longer exist); polls `MSFSudp` every `incomingcheckinterval` (5ms).
3. **DCS-BIOS callbacks** (registered, not pumped): `onAirspeedNeedleChange`
   (`A_10C_AIRSPEED_NEEDLE`, still uses the old `0-65535 → 0..DUAL_STEPS+80`
   linear map, not `IAS_KT_TABLE` - that table is UDP-path only) and
   `onVviChange` (`A_10C_VVI`, VSI). The `ALT`/`Flaps`/`AOA`/`G-Force`
   DCS-BIOS bindings are commented out along with their steppers. Two
   lighting callbacks drive `BACK_LIGHTS` PWM.
4. **SARI** — see the roster table above; fully inactive.
5. **UDP data receiver** (`ProcessReceivedMSFSString`/`HandleOutputValuePair`,
   same `"D,CODE:value,CODE:value,..."` format as `JET_RANGER_SERVO_CONTROLLER`):

   | Code | Real value | Board behaviour |
   |---|---|---|
   | `IAS` | knots, 0-140, fractional allowed (parsed with `toFloat()`; the FSUIPC bridge sends one decimal, e.g. `93.4`; `iasKtToSteps()` interpolates in `double`) | `setIAS()` → `iasKtToSteps()` via **`IAS_KT_TABLE`**, a 9-row hand-measured calibration table (linear interpolation, same pattern as `VSI_FPM_TABLE`). The `0kt→0 step` row is an assumed zero reference, not directly measured - worth confirming on the bench. |
   | `IASRAW` | raw steps | Bypasses the table, direct `.moveTo()` |
   | `VSI` | fpm, ±1750 | `vsiFpmToSteps()` via `VSI_FPM_TABLE` (unchanged) |
   | `VSIRAW` | raw steps | Bypasses the table |
   | `AGL` | raw steps | No real calibration yet (renamed from `RALT` to match `JET_RANGER_SERVO_CONTROLLER`/FSUIPCWinformsAutoCS's actual code) |
   | `OILT`/`XMSNT`/`ITT` | °C | `setEOT`/`setXOT`/`setEGT` via the shared placeholder linear scale (see the `FULL4WIRE_HOMING_STEPS` caution above) |
   | `OILP`/`XMSNP` | PSI | `setEOP`/`setXOP`, same placeholder scale |
   | `RPME`/`RPMR` | — | **Removed** - Turbine/Rotor Speed are served by the dual-stepper board (172.16.1.106); ignored here |
   | `N1` | %, 0-105 | `setGP`, same placeholder scale |
   | `FUEL` | US gal, 0-75 | `setFA`, same placeholder scale |
   | `OILTRAW`/`OILPRAW`/`XMSNTRAW`/`XMSNPRAW`/`ITTRAW`/`N1RAW`/`FUELRAW` | raw steps | Each bypasses its real-value sibling's conversion |
   | `TQ` | raw steps | Renamed from `ET`, no real calibration requested |
   | `FLAPS`, `AOA`, `GFORCE`, `SPDMAX` | — | **Removed.** These were added earlier to give `FlapsStepper`/`AOAstepper`/`GForcestepper`/`SpeedMaxstepper` UDP reachability; now that those steppers are gone, their `HandleOutputValuePair` cases are gone too. `StepperVSITester`'s dropdown still lists these codes — sending them is currently a silent no-op on the board. |

   Codes matching `JET_RANGER_SERVO_CONTROLLER.ino`'s naming
   (`OILT`/`OILP`/`XMSNT`/`XMSNP`/`ITT`/`N1`/`FUEL`/`TQ`/`AGL`)
   are deliberate — see each case's in-code comment for the exact
   quantity match, and `IAS`'s comment for the one case (Bell 206
   servo-position number vs. real knots) where the two boards' same code
   name means different units.
6. **No-data watchdog: every gauge on this board is driven back to its
   calibrated zero if no UDP packet arrives on `MSFSudp` for 30 seconds**
   (`noDataTimeoutMs`) — same feature, same threshold, and the same
   `lastMSFSDataMillis`/`gaugesResetForNoData` latch pattern as
   `JET_RANGER_OLED_DUAL_STEPPER_CONTROLLER.ino`'s own copy (the two
   sketches don't share this code, each has its own `ResetGaugesToZero()`
   matching its own roster). Scoped to `MSFSudp` traffic only — DCS-BIOS
   input doesn't reset the timer.
   The 30s timer is armed at the **end of `setup()`**
   (`lastMSFSDataMillis = millis()`), not at reset - it used to start at 0,
   so after the long boot swings the watchdog was already expired when
   `loop()` began and zeroed the gauges within a fraction of a second. This board's `ResetGaugesToZero()`
   calls `setIAS(0)`/`setVSI(0)`/`setEGT(0)`/`setEOT(0)`/`setEOP(0)`/
   `setXOT(0)`/`setXOP(0)`/`setGP(0)`/`setFA(0)`/
   `setAGL(0)`/`setTQ(0)` (each the same target a real `"<CODE>:0"` packet
   would produce, including that gauge's own zero-offset trim - see #7
   below) - now every gauge on this board goes through its own `setXxx()`
   wrapper here, `setTQ()` included now that Torque has real calibration.
   `ALTstepper` is still fully disabled on this sketch, so no altitude
   reset is included.
7. **Per-gauge zero-offset trim** (`VSIZeroOffset`/`IASZeroOffset`/
   `AGLZeroOffset`/`EGTZeroOffset`/`EOTZeroOffset`/`EOPZeroOffset`/
   `XOTZeroOffset`/`XOPZeroOffset`/
   `GPZeroOffset`/`FAZeroOffset`/`TQZeroOffset`, all `#define`'d near the
   Swing gates above, all defaulting to `0`): same
   fine-trim-without-touching-the-calibration-table purpose as
   `JET_RANGER_OLED_DUAL_STEPPER_CONTROLLER.ino`'s existing `TSoffset`/
   `RSoffset`, now extended to every gauge in this sketch. Added inside
   each gauge's real-value setter (`setEGT()`/`setEOT()`/etc), so it's
   automatically picked up by that gauge's UDP path, DCS-BIOS callback
   (where one exists), and the no-data-watchdog reset (#6 above) without
   any of those call sites needing their own changes. Two gauges needed
   special handling: **VSI** has two entry points that bypass `setVSI()`
   for its real-unit path (`vsiFpmToSteps()`, called directly from
   `HandleOutputValuePair()`), so `VSIZeroOffset` is added at both
   `setVSI()` and that direct call site - and is deliberately *not* baked
   into the boot swing's homing target, since that would double-apply the
   trim (see the in-code comment on VSI's swing for the full reasoning).
   **`TQstepper`** (Torque) was raw-steps-only with no `setXxx()` wrapper
   at the time this offset was added, so `TQZeroOffset` was applied
   directly at its UDP handler and `ResetGaugesToZero()` call sites -
   Torque has since graduated to real `TQ_PCT_TABLE` calibration (see the
   roster table above) and now has its own `setTQ()` wrapper like every
   other gauge, so `TQZeroOffset` moved into that wrapper too; its `TQRAW`
   sibling deliberately does NOT apply the offset, matching every other
   gauge's own `*RAW` code. The five gauges with an existing boot startup
   swing (VSI excepted, see above) also had their swing's "return to
   zero" step updated to target that gauge's offset
   instead of raw `0`, so the swing's final resting position matches
   where the gauge will actually sit at runtime for a `"<CODE>:0"` packet.

## On-demand swing commands

Each existing boot swing is now its own function (`swingFA()`, `swingTQ()`,
`swingEGT()`, `swingGP()`, `swingVSI()`, `swingIAS()`, `swingAGL()`), called
from `setup()` when its `SwingXXX` gate is true (boot behaviour and order
unchanged) and, **ungated**, by a UDP **control** packet `C,<COMMAND>:1` (the
value is ignored; a `Swing command: <name>` line goes to the debug stream):

| Command | Swing |
|---|---|
| `SWINGFUEL` | Fuel Quantity |
| `SWINGTQ` | Torque |
| `SWINGITT` | EGT/ITT |
| `SWINGN1` | Gas Producer |
| `SWINGVSI` | VSI |
| `SWINGIAS` | IAS |
| `SWINGAGL` | Radar Alt (AGL) |

**IAS/Radar Alt homing fix:** `swingIAS()` and `swingAGL()` used to wind to
`X27_FULLWIRE_HOMING_STEPS` and then to `-X27_FULLWIRE_STEPS` before
`setCurrentPosition(0)`, i.e. 635 steps *past* zero against the stop (about
1,270 steps of travel back from full scale), so the needle visibly swung too
far back. They now return to `0` like the Fuel Quantity/Torque/EGT/Gas
Producer swings. (Their "Sending ... to Max" step still goes to
`X27_FULLWIRE_STEPS` (635), which is beyond Radar Alt's calibrated 469-step
maximum - unchanged, same overshoot every graduated gauge's swing has.)
Compiled; not yet flashed or bench-tested.

`EOTstepper`/`XOTstepper`/`XOPstepper`/`EOPstepper` have no boot swing in
this sketch, so they have no command. The swings block the main loop while
they run and re-home the stepper (`setCurrentPosition(0)`) exactly as at
boot; the needle rests at its calibrated zero until the next data packet.
Compiled but not yet bench-tested via UDP.

## Remote reset over UDP

UDP control packet `C,RESET:1` (value ignored) to port 13136 reboots this
board: `RebootBoard()` sends a debug line (`RESET command received - rebooting
in 2s`), waits 100ms, arms the AVR watchdog for 2s (`wdt_enable(WDTO_2S)`) and
spins until it fires, which resets the whole chip. The 2s timeout is
deliberate: the Mega's bootloader may not switch the watchdog off, so a short
timeout could loop. A `.init3` function, `DisableWatchdogAtStartup()`, clears
`MCUSR` and disables the watchdog first thing after every reset (standard
avr-libc pattern), so the sketch never runs with it armed. **Bench-tested on
this board (COM4, 2026-10-13):** after `C,RESET:1` the debug stream showed the
acknowledgement, then "Ethernet Started" 6.6s later (2s watchdog + bootloader +
the 3s Ethernet start-up), a normal full boot with `millis()` back at zero
("Setup Complete. 72610mS since reset"), and no reboot loop. If a board ever
reboot-loops after a reset packet, power-cycle it (that clears the watchdog)
and reflash.

## Pin usage (JET_RANGER_STEPPER_CONTROLLER.ino)

| Pin(s) | Function |
|---|---|
| 14 | Green status LED (moved from 13) |
| 15 | Red status LED (moved from 12) |
| 53 | W5500 Ethernet shield manual reset |
| 8 | Backlighting PWM output (`BACK_LIGHTS`) |
| 9 | SARI pitch servo (`DcsBios::ServoOutput saiPitch`) |
| 30, 32 | SARI roll stepper step/direction — 32 collides with `RADAR_ALT_COIL_A` below (pin 30 collided with `RS_COIL_C` until Rotor Speed was removed) |
| 56 | Shared stepper-driver enable pin (`AllstepperEnablePin`) — `pinMode` commented out, see caution above; collides with `EOT_COIL_D` below |
| 7, 8, 9, 11 | VSI 4-wire stepper coils (`COIL_VSI_A..D`) |
| 12, 13, 22, 23 | IAS (Current Airspeed) 4-wire stepper coils (`STEPPER_SPD_A..D`) |
| 32, 33, 34, 35 | Radar Alt 4-wire stepper coils (`RADAR_ALT_COIL_A..D`) — 32 collides with `SARIdirectionPin` |
| 48, A0, A1, A2 (54/55/56) | EOT 4-wire stepper coils (`EOT_COIL_A..D`) — A2/56 collides with `AllstepperEnablePin` |
| A3, A4, A5, A6 (57-60) | XOT 4-wire stepper coils |
| A7, A8, A9, A10 (61-64) | XOP 4-wire stepper coils |
| A11, A12, A13, A14 (65-68) | EGT 4-wire stepper coils |
| 24-31 | Unused (previously TS/RS coils, removed with Turbine/Rotor Speed) |
| 2, 3, 4, 6 | FA 4-wire stepper coils |
| 36, 37, 38, 39 | ET 4-wire stepper coils |
| 40, 41, 42, 43 | GP 4-wire stepper coils |
| 44, 45, 46, 47 | EOP 4-wire stepper coils |
| 46, 48 | `FlapsStepPin`/`FlapsDirectionPin` — orphaned defines, no stepper reads them any more |
| Serial (USB) | DCS-BIOS `DCSBIOS_IRQ_SERIAL` link (registered, not pumped — see program flow above) |

## Local network configuration (JET_RANGER_STEPPER_CONTROLLER.ino)

| Setting | Value |
|---|---|
| Static IP | `172.16.1.105` |
| MAC | `A8:61:0A:67:83:69` (the `sMac` debug label was corrected to match on 12 Oct 2026) |
| Local port `localport`/`keyboardport` | 7788 |
| Local port `localdebugport` | 7795 (declared, not bound) |
| Local port `MSFSport` | 13136 |

## Remote endpoints this sketch talks to

| Target | Port | Purpose |
|---|---|---|
| `172.16.1.10` (reflector host) | 27000 | Debug/log messages via `SendDebug()` |
| `172.16.1.110` (`targetIP`) | 7788 / 7789 | `SendIPString()`/`SendLedString()` — declared, no callers |
| `172.16.1.10` (`MSFSIP`) | 7791 | `SendMSFSMessage()` — declared, no callers |
| `reflectorIP` (172.16.1.10, `JetRangerHealthMonitor`'s host) | 13137 | Health keepalive — bare `"STEPPER"` string sent every 10s (`aliveinterval`) via `aliveudp`, same pattern as `JET_RANGER_UPPER_CONTROLLER.ino`'s `"UPPER_INPUT"` keepalive |

> **[StepperVSITester](../../%20C%23%20Code/StepperVSITester/PROGRAM_SUMMARY.md)**
> can send every real-unit and `*RAW` code above straight to this board's
> `172.16.1.105:13136` for bench testing without needing FSUIPC or a
> flight sim running.
>
> **[JetRangerHealthMonitor](../../%20C%23%20Code/JetRangerHealthMonitor/PROGRAM_SUMMARY.md)**
> now shows this board's live/dead status (Stepper indicator) alongside
> Comm/Nav, Servo, Joystick, and Upper Input, based on the `"STEPPER"`
> keepalive above.

## Backlight: UDP brightness and main bus power (2026-10-18)

The backlight on pin 5 (`BACK_LIGHTS`) is now controlled by `applyBacklight()`:

- **Main bus:** the bridge sends `BPWR:1` while the main bus is 20 V or more and `BPWR:0` below that. `BPWR:0` turns the backlight
  off; `BPWR:1` turns it on. Until a `BPWR` packet arrives the backlight is on (the board has no way to know the bus is down).
- **Brightness:** control packet `C,B:<0-15>` (0 off, 15 full, mapped to PWM 0-255, out-of-range values clamped) is remembered.
  If no brightness command has been received the backlight uses the default PWM 65 (`BacklightDefaultPwm`, the old post-setup
  level). Once one has been received, every time power is applied the backlight returns to the **last commanded level**, not the
  default. A command received while the main bus is down is remembered and applied when power returns.
- The backlight still starts at 128 during setup and the no-data watchdog does not change it. The DCS-BIOS brightness handlers
  still call `analogWrite()` directly but are never run (`DcsBios::loop()` is commented out).
- Each change is logged (`Brightness <n> -> PWM <pwm>`, `Backlight power on/off`). The bridges (FSUIPCWinformsAutoCS, MSFSWinformsAutoCS)
  send `BPWR` first in the Stepper packet and after `PWR` in the Dual Stepper packet.

Compiled at 31,446 bytes flash (12%) / 4,097 bytes RAM (50%); **not yet flashed or bench-tested**.
