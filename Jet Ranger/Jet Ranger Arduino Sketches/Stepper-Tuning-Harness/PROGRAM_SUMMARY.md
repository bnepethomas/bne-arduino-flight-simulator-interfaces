# Stepper-Tuning-Harness — Program Summary

Standalone bench-test sketch (paired with `JET_RANGER_STEPPER_CONTROLLER`,
the stepper equivalent of `Servo-Knob-For-Calibration`/`ServoTuner`) that
lets an operator pick one of the 7 analogue stepper gauges and repeatedly
type a target step position over the Serial Monitor to see where the
needle lands — for finding each gauge's real-world min/max/zero step
values by hand before baking them into the production sketch. No Ethernet,
no DCS-BIOS, Serial only.

## Program flow

1. **Setup**: starts Serial at 115200 baud, enables the shared stepper
   driver pin (`AllstepperEnablePin`, pin 56 — matches
   `JET_RANGER_STEPPER_CONTROLLER.ino`'s wiring exactly), sets every
   stepper's max speed/acceleration to the same `9000`/`9000` values that
   sketch uses, then prints the stepper menu.
2. **Main loop** (`loop()`): accumulates incoming Serial bytes into a line
   buffer until a newline, then hands the completed line to `handleLine()`;
   every iteration also calls `.run()` on all 7 steppers (not just the
   selected one), so a stepper mid-move when the operator switches
   selection still completes its move instead of freezing part-way.
3. **`handleLine(line)`** recognises four input forms:
   - `m` — reprints the stepper menu (does **not** change the current
     selection).
   - `sN` (e.g. `s0`, `s4`) — selects stepper `N` from the menu. This is a
     deliberately distinct prefix from a plain number: once a stepper is
     selected, a bare integer always means "move here", so typing `5`
     moves the *current* stepper to step 5 rather than switching to
     stepper #5 — an ambiguity that would exist if selection and target
     entry both accepted plain numbers.
   - `z` — zeroes the currently selected stepper at wherever it is
     physically sitting right now (`setCurrentPosition(0)`), useful for
     establishing a known reference point before hunting for min/max.
   - Any other integer (optionally signed, e.g. `1500` or `-200`) — calls
     `.moveTo()` on the currently selected stepper. Can be sent as many
     times as needed; each one just updates the target.

## Pin usage

Identical wiring to `JET_RANGER_STEPPER_CONTROLLER.ino`'s 7 "simple"
`AccelStepper` gauges (the SARI roll stepper is intentionally not included
— see below):

| Pin(s) | Function |
|---|---|
| 56 | Shared stepper-driver enable pin (`AllstepperEnablePin`) |
| 22, 24 | AOA stepper step/direction |
| 26, 28 | G-Force stepper step/direction |
| 34, 36 | Current-airspeed stepper step/direction |
| 38, 40 | Max-airspeed stepper step/direction |
| 42, 44 | Altimeter stepper step/direction |
| 46, 48 | VSI stepper step/direction |
| 2, 3, 4, 5 | Flaps 4-wire stepper coils (`COIL_FLAPS_A..D`) |

> **Not included:** the SARI roll stepper (attitude-indicator roll axis).
> It requires its own closed-loop IR-sensor homing/tracking state machine
> (`Nema8Stepper` in `JET_RANGER_STEPPER_CONTROLLER.ino`) rather than a
> plain absolute step target, so a "type a number, see where it goes" tool
> isn't a meaningful fit for it. Its pitch servo (`saiPitch`, pin 9) is
> likewise out of scope here since it's a hobby servo, not a stepper.
> `JET_RANGER_STEPPER_CONTROLLER.ino`'s altimeter zero-sense homing switch
> (`ALTzeroSensePin`, pin 54) is also not wired up here — this harness
> relies on the operator's own `z` command instead of automatic homing.

## Local network configuration / IP addresses

None — this sketch does not include the Ethernet/SPI libraries and
performs no network communication of any kind; it's Serial-only.

## Build verification

Compiled clean with `arduino-cli` (target `arduino:avr:mega:cpu=atmega2560`,
**AccelStepper** 1.64.0): 11,222 bytes flash (4%), 808 bytes RAM (9%).

## C# / other programs this sketch communicates with

None. Values found with this harness (min/max/zero step positions per
gauge) are meant to be copied by hand into
**[JET_RANGER_STEPPER_CONTROLLER](../JET_RANGER_STEPPER_CONTROLLER/PROGRAM_SUMMARY.md)**
once established, the same way `ServoTuner`/`Servo-Knob-For-Calibration`
findings feed into `JET_RANGER_SERVO_CONTROLLER`'s servo tables.
