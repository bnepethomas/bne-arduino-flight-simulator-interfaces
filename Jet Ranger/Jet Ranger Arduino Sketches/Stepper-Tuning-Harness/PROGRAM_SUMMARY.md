# Stepper-Tuning-Harness — Program Summary

Standalone bench-test sketch (the stepper equivalent of
`Servo-Knob-For-Calibration`/`ServoTuner`) that lets an operator pick one
stepper gauge at a time and repeatedly type a target step position over
the Serial Monitor to see where the needle lands — for finding each
gauge's real-world min/max/zero step values by hand. No DCS-BIOS. Serial
is the interactive control surface; Ethernet is used only passively, to
mirror every significant action to the reflector host as a debug log
line, the same `SendDebug()` pattern used throughout the rest of the Jet
Ranger fleet.

This sketch has diverged substantially from
[`JET_RANGER_STEPPER_CONTROLLER`](../JET_RANGER_STEPPER_CONTROLLER/PROGRAM_SUMMARY.md)
over a series of bench-wiring changes (see history below) and currently
carries **13 steppers**, none of them geared `DRIVER`/STEP-DIR anymore —
every single one is now direct-driven `FULL4WIRE`.

## History (why this looks so different from production now)

In order, each superseding the last:
1. VSI and Flaps swapped `AccelStepper` interfaces/pins (VSI → `FULL4WIRE`
   coils, Flaps → `DRIVER`/STEP-DIR) — `JET_RANGER_STEPPER_CONTROLLER.ino`
   received the matching real-world change at the time.
2. GForce, SpeedMax, and Alt were removed entirely (code and pins) — not
   ported back to production.
3. AOA was renamed to **Radar Alt** and rewired from `DRIVER`/STEP-DIR to
   `FULL4WIRE` on pins 32/33/34/35, with a boot-time homing routine
   (`homeRadarAlt()`) added.
4. Current Airspeed (`SpeedCurrentstepper`) was rewired from `DRIVER` to
   `FULL4WIRE` on pins 12/13/22/23, which collided with the status LEDs
   (also 12/13) — resolved by moving the LEDs to pins 15 (red) / 14
   (green).
5. Ten new gauges were added as `FULL4WIRE`, ported later into
   `JET_RANGER_STEPPER_CONTROLLER.ino` too: **EOT** (48/A0/A1/A2), **XOT**
   (A3-A6), **XOP** (A7-A10), **EGT** (A11-A14), **TS** (24-27), **RS**
   (28-31), **FA** (2/3/4/6), **ET** (36-39), **GP** (40-43), **EOP**
   (44-47).
6. Flaps and the shared `AllstepperEnablePin` (56) were removed entirely
   (code and pins) — along with them, `DRIVER_HOMING_STEPS` (now dead,
   since Flaps was the last `DRIVER`-interface stepper), the `d`/`e`
   Flaps-DIR diagnostic commands, and the loop's Flaps-DIR auto-latch
   logic. **Nothing in this sketch drives a driver-enable pin anymore** —
   if your driver modules still need their enable line actively pulled
   low by the MCU, none of the steppers will move until that's resolved
   in hardware.

## Program flow

1. **Setup**: starts Serial at 115200 baud, runs the status-LED startup
   flash, resets the W5500 shield and brings up Ethernet on the same
   static IP `JET_RANGER_STEPPER_CONTROLLER.ino` uses, sets every
   stepper's max speed/acceleration (`STEPPER_MAX_SPEED` 19000,
   `STEPPER_ACCELERATION` 9000 — see the divergence-from-production caveat
   below), calls `homeVSI()` then `homeRadarAlt()` to wind those two back
   to their end stops and zero them, prints the stepper menu, then sends
   a `"Setup Complete"` debug log.

> **Speed/acceleration diverge from production, and change during bench
> tuning:** `JET_RANGER_STEPPER_CONTROLLER.ino` uses `9000`/`1000`. On the
> bench `9000` acceleration caused missed steps (high acceleration demands
> more torque than the motor can supply, so it loses sync), so expect
> these `#define`s to keep changing experimentally while tuning — check
> current values in the sketch rather than trusting a specific number
> here. Whatever turns out step-accurate is worth carrying back to
> production.

2. **Main loop** (`loop()`): toggles the red/green status LEDs (pins
   15/14) every `FLASH_TIME` (300ms) as a heartbeat. Accumulates incoming
   Serial bytes into a line buffer until newline and hands the completed
   line to `handleLine()`. Every iteration also calls `.run()` on all 13
   steppers (not just the selected one), so a stepper mid-move when the
   operator switches selection still completes instead of freezing.
3. **`homeVSI()`**: winds VSI hard to `-FULL4WIRE_HOMING_STEPS`
   (blind — no zero-sense sensor), zeroes there, then moves an
   additional `VSI_ZERO_OFFSET_STEPS` (317) further and re-zeroes. This
   puts VSI's usable zero 317 steps off the physical end stop, so typed
   targets can go both positive (away from the stop) and negative (back
   toward it).
4. **`homeRadarAlt()`**: same blind wind-to-end-stop pattern as VSI's
   first move (`-FULL4WIRE_HOMING_STEPS`, then zero) but with **no**
   second offset move — Radar Alt's usable zero sits right at its
   physical end stop. Direction sign is an unverified guess (see the
   history section above); confirm on the bench before trusting it
   unattended.
5. **`handleLine(line)`** recognises:
   - `m` — reprints the stepper menu (does not change selection). Not
     logged.
   - `sN` (e.g. `s0`, `s1`) — selects stepper `N` from the menu (see the
     table below for the current name/index mapping). Logs
     `"Selected <name>"`.
   - `z` — zeroes the currently selected stepper at wherever it's
     physically sitting right now. Logs `"Zeroed <name> at its current
     physical position"`.
   - `h` — re-runs `homeVSI()` on demand, but only while VSI (`s1`) is
     selected; prints an explanatory message otherwise. Only VSI supports
     this — Radar Alt's homing isn't wired to the `h` command, only to
     boot.
   - Any other integer (optionally signed, e.g. `1500` or `-200`) — calls
     `.moveTo()` on the currently selected stepper. Logs `"<name> target
     set to <value>"`.
   - `fN` (e.g. `f1000`, `f-500`) — VSI only: converts a feet value into a
     step target via `vsiFtToDisplaySteps()`/`VSI_FT_TABLE` and calls
     `.moveTo()` with it. Logs `"VSI target set to <ft> ft (step
     <value>)"`.
6. **`VSI_FT_TABLE`/`vsiFtToDisplaySteps()`**: an 11-row ft-to-step
   calibration table, hand-measured on the bench ("ft" is this harness's
   own informal shorthand for VSI's fpm units, not altitude). Linearly
   interpolates between the two nearest rows; clamps to whichever end is
   nearest outside the table's -1750..1750 ft range.
   > **Same data used in production:** `JET_RANGER_STEPPER_CONTROLLER.ino`
   > carries this identical table as `VSI_FPM_TABLE`/`vsiFpmToSteps()`.
7. **`toRawSteps()`/`toDisplaySteps()`**: AccelStepper's native positive
   direction isn't guaranteed to agree across every stepper's wiring, so
   each `StepperEntry` carries its own `displaySign`, and the two helpers
   take that sign as a parameter rather than applying one fixed flip.
   Regardless of the per-stepper sign, the operator-facing convention
   stays the same for every gauge: typing a positive target moves it
   clockwise.

## Stepper menu (current)

| Slot | Name | Interface/pins | displaySign | Homing |
|---|---|---|---|---|
| s0 | Current Airspeed | FULL4WIRE, 12/13/22/23 | -1 (unverified for new interface) | none |
| s1 | VSI | FULL4WIRE, `COIL_VSI_A..D` (7/8/9/11) | 1 (confirmed backwards on the bench) | boot + `h` |
| s2 | Radar Alt | FULL4WIRE, `RADAR_ALT_COIL_A..D` (32/33/34/35, wired C,D,A,B) | 1 (unverified) | boot only |
| s3 | EOT | FULL4WIRE, 48/A0/A1/A2 | 1 (unverified) | none |
| s4 | XOT | FULL4WIRE, A3/A4/A5/A6 | 1 (unverified) | none |
| s5 | XOP | FULL4WIRE, A7/A8/A9/A10 | 1 (unverified) | none |
| s6 | EGT | FULL4WIRE, A11/A12/A13/A14 | 1 (unverified) | none |
| s7 | TS | FULL4WIRE, 24/25/26/27 | 1 (unverified) | none |
| s8 | RS | FULL4WIRE, 28/29/30/31 | 1 (unverified) | none |
| s9 | FA | FULL4WIRE, 2/3/4/6 | 1 (unverified) | none |
| s10 | ET | FULL4WIRE, 36/37/38/39 | 1 (unverified) | none |
| s11 | GP | FULL4WIRE, 40/41/42/43 | 1 (unverified) | none |
| s12 | EOP | FULL4WIRE, 44/45/46/47 | 1 (unverified) | none |

## Pin usage

| Pin(s) | Function |
|---|---|
| 14 | Green status LED (`GREEN_STATUS_LED_PORT`/`Check_LED_G`) — moved from 12 |
| 15 | Red status LED (`RED_STATUS_LED_PORT`/`Check_LED_R`) — moved from 13 |
| 53 | W5500 Ethernet shield manual reset (`ES1_RESET_PIN`) |
| See the stepper menu table above | Every gauge's coil pins |

No shared driver-enable pin exists anymore (see history item 6 above).

## Local network configuration

| Setting | Value |
|---|---|
| Static IP | `172.16.1.105` — same address as `JET_RANGER_STEPPER_CONTROLLER.ino`. Meant as a drop-in stand-in for that sketch while bench-tuning, not run alongside it. |
| MAC | `A8:61:0A:67:83:69` — also matches `JET_RANGER_STEPPER_CONTROLLER.ino` |
| Local port `localport` | 7788 (bound, source socket for outbound debug packets) |

No inbound listener is set up — this harness is driven purely by Serial
input; Ethernet is send-only, for logging.

## Remote endpoints this sketch talks to

| Target | Port | Purpose |
|---|---|---|
| `172.16.1.10` (reflector host) | 27000 | Debug/log messages (`SendDebug`) — Ethernet-started, setup-complete, homing, stepper selection, zero, and move-to-target events |

## Build verification

Compiled clean with `arduino-cli` (target `arduino:avr:mega:cpu=atmega2560`,
**AccelStepper** 1.64.0, **Ethernet** 2.0.2): 21,978 bytes flash (8%),
1,957 bytes RAM (23%). Flashed to the Mega on COM4 and verified via avrdude.

## C# / other programs this sketch communicates with

- No C# project in this repository listens on `172.16.1.10:27000` — the
  debug log stream has no in-repo consumer.
- Values found with this harness (min/max/zero step positions per gauge)
  are meant to be copied by hand into
  **[JET_RANGER_STEPPER_CONTROLLER](../JET_RANGER_STEPPER_CONTROLLER/PROGRAM_SUMMARY.md)**,
  the same way `ServoTuner`/`Servo-Knob-For-Calibration` findings feed
  into `JET_RANGER_SERVO_CONTROLLER`'s servo tables. The 10 newest gauges
  (EOT through EOP) and Radar Alt have already been ported into that
  sketch's pin/stepper declarations, but with no calibration or homing
  yet — see that sketch's own summary for the pin-conflict list this
  created there (production wasn't cleaned up the way this harness was,
  so several of the new pins collide with still-active ALT/SpeedMax/
  Flaps/AOA/GForce/AllstepperEnablePin pins).
