# Stepper-Tuning-Harness — Program Summary

Standalone bench-test sketch (paired with `JET_RANGER_STEPPER_CONTROLLER`,
the stepper equivalent of `Servo-Knob-For-Calibration`/`ServoTuner`) that
lets an operator pick one of the 7 analogue stepper gauges and repeatedly
type a target step position over the Serial Monitor to see where the
needle lands — for finding each gauge's real-world min/max/zero step
values by hand before baking them into the production sketch. No DCS-BIOS.
Serial is the interactive control surface; Ethernet is used only
passively, to mirror every significant action to the reflector host as a
debug log line, the same `SendDebug()` pattern used throughout the rest of
the Jet Ranger fleet.

## Program flow

1. **Setup**: starts Serial at 115200 baud, runs the status-LED startup
   flash (green+red on, `FLASH_TIME`, both off, `FLASH_TIME` — ported as-is
   from `JET_RANGER_STEPPER_CONTROLLER.ino`), resets the W5500 shield and
   brings up Ethernet on the same static IP that sketch uses, flashing the
   green LED while the link settles (same `delayBeforeSendingPacket`
   pattern as the rest of the fleet), then enables the shared stepper
   driver pin (`AllstepperEnablePin`, pin 56 — matches that sketch's wiring
   exactly), sets every stepper's max speed/acceleration (see caveat below
   for how these currently compare to production), calls `homeVSI()` and
   `homeFlaps()` (below) to wind VSI and Flaps back to their end stops and
   zero them, prints the stepper menu, then sends a `"Setup Complete"`
   debug log.

> **Speed/acceleration diverge from production, and change during bench
> tuning:** `JET_RANGER_STEPPER_CONTROLLER.ino` uses `9000`/`9000` for max
> speed/acceleration. On the bench that acceleration value caused missed
> steps (high acceleration demands more torque than the motor can supply,
> so it loses sync), so expect these two `#define`s to be adjusted
> experimentally while tuning — check the current values in the sketch
> itself rather than trusting a specific number here. Whatever combination
> turns out to be step-accurate is worth carrying back into
> `JET_RANGER_STEPPER_CONTROLLER.ino` too.
2. **Main loop** (`loop()`): toggles the red/green status LEDs every
   `FLASH_TIME` (300ms) as a heartbeat, exactly like the production
   sketch's own status-LED block. Then accumulates incoming Serial bytes
   into a line buffer until a newline, and hands the completed line to
   `handleLine()`; every iteration also calls `.run()` on all 7 steppers
   (not just the selected one), so a stepper mid-move when the operator
   switches selection still completes its move instead of freezing
   part-way. Finally, it checks `FlapsStepper.distanceToGo() == 0` (Flaps
   has nothing left to move) and, the first loop iteration that's true
   after a move, latches the Flaps DIR pin HIGH directly via
   `digitalWrite()` — the same action as the manual `d` command below,
   just automatic — logging `"Flaps DIR pin auto-forced HIGH (reached
   target)"`. A `flapsDirLatchedHigh` flag edge-detects this so it fires
   once per arrival rather than every idle iteration, and resets as soon
   as Flaps is given a new target. AccelStepper still owns the pin the
   moment Flaps actually steps again, so this only holds while Flaps is
   sitting still. (This logic lives on whichever stepper is the
   DRIVER/STEP-DIR one — only that interface has an actual DIR pin — and
   that's now Flaps; see the VSI/Flaps hardware swap below.)
3. **`homeVSI()`/`homeFlaps()`**: each winds its stepper hard against its
   end stop and zeroes it there (`setCurrentPosition(0)`), the same two-line
   pattern (`runToNewPosition()` then zero) for both, differing only in the
   step count and sign used to reach the stop. VSI and Flaps swapped
   AccelStepper interfaces/pins (see the hardware-swap note below), and the
   step count/overshoot used by each homing function follows that hardware
   rather than the function's name:
   - `homeVSI()` now drives the direct-driven FULL4WIRE hardware (was
     Flaps' pins/interface), so it uses `FULL4WIRE_HOMING_STEPS` with no
     overshoot multiplier — matching that hardware's own homing style.
     Direction is `-FULL4WIRE_HOMING_STEPS`, carried over unchanged from
     what that same physical hardware needed as `homeFlaps()` before the
     swap. `homeVSI()` then does a **second** move: `VSI_ZERO_OFFSET_STEPS`
     (317) further, and re-zeroes there. This puts the actual zero
     reference 317 steps off the end stop instead of at the stop itself,
     so typed target steps for VSI can go both positive (further from the
     stop) and negative (back toward it) — the stop is no longer the only
     usable direction from zero. `homeFlaps()` has no equivalent
     second move.
   - `homeFlaps()` now drives the geared DRIVER/STEP-DIR hardware (was
     VSI's pins/interface), so it uses `DRIVER_HOMING_STEPS * 1.1`
     (deliberately overshooting by 10% so it reaches the real stop
     regardless of exactly how many steps that is, matching `STEPS` in
     `JET_RANGER_STEPPER_CONTROLLER.ino`) — matching that hardware's own
     homing style. Direction is positive, carried over unchanged from what
     that same physical hardware needed as `homeVSI()` before the swap.
   > **Caution:** the +/- signs above are carried over from the
   > corresponding physical hardware's previously-established homing
   > direction, not independently re-verified under the new function
   > names. Confirm on the bench that each still winds to (and stops
   > cleanly at) the correct end stop before trusting it unattended.
   Both run once automatically at boot (before the menu is printed), and
   again on demand any time the `h` command is sent while that stepper
   (`STEPPER_INDEX_VSI` = `s4`, `STEPPER_INDEX_FLAPS` = `s0`) is selected.
   Each logs a `"Homing <name> - winding to end stop"` line before and
   `"<name> end stop reached, zeroed at step 0"` after; `homeVSI()`'s
   second move additionally logs `"VSI moving off end stop to set zero
   reference..."` before and `"VSI zero reference set, 317 steps off end
   stop"` after.
4. **`handleLine(line)`** recognises five input forms (selection, zero, and
   move-to-target each also send a matching `SendDebug()` line):
   - `m` — reprints the stepper menu (does **not** change the current
     selection). Not logged.
   - `sN` (e.g. `s0`, `s4`) — selects stepper `N` from the menu. This is a
     deliberately distinct prefix from a plain number: once a stepper is
     selected, a bare integer always means "move here", so typing `5`
     moves the *current* stepper to step 5 rather than switching to
     stepper #5 — an ambiguity that would exist if selection and target
     entry both accepted plain numbers. Logs `"Selected <name>"`.
     **Note:** menu slots 0 and 4 were swapped on request — `s0` now
     selects **Flaps** and `s4` now selects **VSI** (the reverse of the
     original layout). Only the `steppers[]` array order and
     `STEPPER_INDEX_VSI`/`STEPPER_INDEX_FLAPS` (now `4`/`0`) changed here;
     see the separate hardware swap below for the pins/interfaces
     themselves.
   > **Hardware swap (separate from the menu-slot swap above):** VSI and
   > Flaps also swapped which physical pins/`AccelStepper` interface each
   > one drives. VSI is now `AccelStepper::FULL4WIRE` on `COIL_VSI_A..D`
   > (pins 2, 3, 4, 5 — was Flaps' coil pins). Flaps is now
   > `AccelStepper::DRIVER` on `FlapsStepPin`/`FlapsDirectionPin` (pins 46,
   > 48 — was VSI's step/dir pins). Everything downstream that's tied to
   > "which interface type does this stepper have" follows the hardware,
   > not the name: `homeVSI()`/`homeFlaps()`'s step count and overshoot
   > (see above), each one's `displaySign` (see below), and the Flaps
   > DIR-pin diagnostic (`d`/`e` and the automatic latch, see below) all
   > moved with their hardware. The one thing that did **not** re-verify
   > itself is the +/- homing direction sign for each function — see the
   > caution note in the homing section above.
   - `z` — zeroes the currently selected stepper at wherever it is
     physically sitting right now (`setCurrentPosition(0)`), useful for
     establishing a known reference point before hunting for min/max. Logs
     `"Zeroed <name> at its current physical position"`.
   - `h` — re-runs `homeVSI()`/`homeFlaps()` on demand, but only while VSI
     (`s4`) or Flaps (`s0`) is selected; prints a message explaining it
     isn't implemented for any other stepper yet otherwise (not logged -
     the homing functions log their own lines regardless of how they were
     triggered).
   - Any other integer (optionally signed, e.g. `1500` or `-200`) — calls
     `.moveTo()` on the currently selected stepper. Can be sent as many
     times as needed; each one just updates the target. Logs `"<name>
     target set to <value>"`.
   - `d` / `e` — diagnostic only, available regardless of what's selected:
     drive the Flaps DIR pin (`FlapsDirectionPin`, pin 48) HIGH (`d`) or LOW
     (`e`) directly via `digitalWrite()`, entirely bypassing AccelStepper.
     Neither moves the stepper. Since AccelStepper owns that pin whenever
     it actually steps, this only holds until Flaps' next move overwrites
     it. Logs `"Flaps DIR pin forced HIGH"` / `"Flaps DIR pin forced LOW"`.
     Moved here from VSI when VSI and Flaps swapped AccelStepper
     interfaces/pins — only a DRIVER/STEP-DIR stepper has an actual DIR
     pin, and that's Flaps now.
   - `fN` (e.g. `f1000`, `f-500`) — VSI only: converts a feet value into a
     step target via `vsiFtToDisplaySteps()`/`VSI_FT_TABLE` (item 5 below)
     and calls `.moveTo()` with it, same as typing the resulting step
     number directly. Only valid while VSI (`s4`) is selected; prints a
     message explaining it isn't supported for the other steppers
     otherwise. Logs `"VSI target set to <ft> ft (step <value>)"`.
5. **`VSI_FT_TABLE`/`vsiFtToDisplaySteps()`**: an 11-row ft-to-step
   calibration table, hand-measured on the bench, that lets the `f`
   command above accept feet instead of raw steps ("ft" here is this
   harness's own informal shorthand for VSI's fpm units, not altitude —
   see below). Each row's `step` value is a **display** step target (the
   same units the plain-integer move command uses), with `0` ft mapped to
   display step `0` to match the zero `homeVSI()` establishes — not the
   raw 317-steps-from-the-end-stop distance homing winds through to reach
   that zero (that number is `VSI_ZERO_OFFSET_STEPS`, a separate constant;
   the two are numerically identical by design, since 317 is exactly how
   far off the stop zero sits). `vsiFtToDisplaySteps()` linearly
   interpolates between whichever two table rows bracket the requested ft
   value; a value outside the table's -1750..1750 ft range is clamped to
   whichever end is nearest, never extrapolated.
   > **Same data now used in production:** `JET_RANGER_STEPPER_CONTROLLER.ino`
   > carries this identical fpm→step table as `VSI_FPM_TABLE`/
   > `vsiFpmToSteps()`, applied to the raw fpm values its `VSI` UDP handler
   > now receives from `FSUIPCWinformsAutoCS`/`StepperVSITester`. Any
   > future recalibration on the bench should be applied to both tables.
6. **`toRawSteps()`/`toDisplaySteps()`**: AccelStepper's own native positive
   direction isn't guaranteed to agree between steppers — VSI and Flaps use
   different `AccelStepper` interface types (`FULL4WIRE` vs
   `DRIVER`/STEP-DIR — see the hardware swap below), and both `homeVSI()`
   and `homeFlaps()` already needed opposite raw-direction signs to reach
   their end stops. So each `StepperEntry` carries its own `displaySign`
   (`-1` for Flaps and every other stepper, `+1` for VSI — inherited from
   the physical hardware each now drives, confirmed backwards on the bench
   under its previous name), and the two helpers take that sign as a
   parameter rather than applying one fixed flip to everyone. Regardless
   of the per-stepper sign, the
   operator-facing convention stays the same for every gauge: typing a
   positive target moves it **clockwise**, negative counter-clockwise.
   Every step count that crosses between "what the operator types/sees" and
   "what AccelStepper actually tracks" goes through one of these two
   (single-purpose, symmetric sign-flip) helpers: `toRawSteps()` before
   `.moveTo()`, `toDisplaySteps()` when showing `currentPosition()` back in
   the menu. `homeVSI()`/`homeFlaps()`'s own end-stop-seeking moves are raw
   hardware directions, independent of this display convention, so neither
   is passed through either helper.

## Pin usage

Matches `JET_RANGER_STEPPER_CONTROLLER.ino`'s pin numbers for all 7
"simple" `AccelStepper` gauges (the SARI roll stepper is intentionally not
included — see below). VSI and Flaps had their `AccelStepper`
interfaces/pins swapped here first (see the hardware-swap note above);
`JET_RANGER_STEPPER_CONTROLLER.ino` has since received the matching
real-world wiring change and the same code correction, so the two are back
in sync for these two steppers as well as the other 5.

| Pin(s) | Function |
|---|---|
| 12 | Red status LED (`RED_STATUS_LED_PORT`/`Check_LED_R`) |
| 13 | Green status LED (`GREEN_STATUS_LED_PORT`/`Check_LED_G`) |
| 53 | W5500 Ethernet shield manual reset (`ES1_RESET_PIN`) |
| 56 | Shared stepper-driver enable pin (`AllstepperEnablePin`) |
| 22, 24 | AOA stepper step/direction |
| 26, 28 | G-Force stepper step/direction |
| 34, 36 | Current-airspeed stepper step/direction |
| 38, 40 | Max-airspeed stepper step/direction |
| 42, 44 | Altimeter stepper step/direction |
| 46, 48 | Flaps stepper step/direction (`FlapsStepPin`/`FlapsDirectionPin`) — was VSI's pins before the hardware swap |
| 2, 3, 4, 5 | VSI 4-wire stepper coils (`COIL_VSI_A..D`) — was Flaps' pins before the hardware swap |

> **Not included:** the SARI roll stepper (attitude-indicator roll axis).
> It requires its own closed-loop IR-sensor homing/tracking state machine
> (`Nema8Stepper` in `JET_RANGER_STEPPER_CONTROLLER.ino`) rather than a
> plain absolute step target, so a "type a number, see where it goes" tool
> isn't a meaningful fit for it. Its pitch servo (`saiPitch`, pin 9) is
> likewise out of scope here since it's a hobby servo, not a stepper.
> `JET_RANGER_STEPPER_CONTROLLER.ino`'s altimeter zero-sense homing switch
> (`ALTzeroSensePin`, pin 54) is also not wired up here — this harness
> relies on the operator's own `z` command instead of automatic homing.

## Local network configuration

| Setting | Value |
|---|---|
| Static IP | `172.16.1.105` — same address as `JET_RANGER_STEPPER_CONTROLLER.ino`. This harness is meant as a drop-in stand-in for that sketch while bench-tuning, not something run on the network alongside it, so reusing its identity is intentional rather than a conflict. |
| MAC | `A8:61:0A:67:83:69` — also matches `JET_RANGER_STEPPER_CONTROLLER.ino` |
| Local port `localport` | 7788 (bound, source socket for outbound debug packets) |

No inbound listener is set up (unlike `JET_RANGER_STEPPER_CONTROLLER.ino`'s
`MSFSudp` on port 13136) — this harness is driven purely by Serial input;
Ethernet is send-only, for logging.

## Remote endpoints this sketch talks to

| Target | Port | Purpose |
|---|---|---|
| `172.16.1.10` (reflector host) | 27000 | Debug/log messages (`SendDebug`) — Ethernet-started, setup-complete, VSI/Flaps homing, stepper selection, zero, and move-to-target events |

## Build verification

Compiled clean with `arduino-cli` (target `arduino:avr:mega:cpu=atmega2560`,
**AccelStepper** 1.64.0, **Ethernet** 2.0.2): 22,680 bytes flash (8%),
1,604 bytes RAM (19%). Flashed to the Mega on COM4 and verified via avrdude.

## C# / other programs this sketch communicates with

- No C# project in this repository listens on `172.16.1.10:27000` — the
  debug log stream has no in-repo consumer, the same gap noted for every
  other Jet Ranger board.
- Values found with this harness (min/max/zero step positions per gauge)
  are meant to be copied by hand into
  **[JET_RANGER_STEPPER_CONTROLLER](../JET_RANGER_STEPPER_CONTROLLER/PROGRAM_SUMMARY.md)**
  once established, the same way `ServoTuner`/`Servo-Knob-For-Calibration`
  findings feed into `JET_RANGER_SERVO_CONTROLLER`'s servo tables.
