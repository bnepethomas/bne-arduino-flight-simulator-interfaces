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
   exactly), sets every stepper's max speed/acceleration to the same
   `9000`/`9000` values it uses, calls `homeVSI()` (below) to wind VSI back
   to its end stop and zero it, prints the stepper menu, then sends a
   `"Setup Complete"` debug log.
2. **Main loop** (`loop()`): toggles the red/green status LEDs every
   `FLASH_TIME` (300ms) as a heartbeat, exactly like the production
   sketch's own status-LED block. Then accumulates incoming Serial bytes
   into a line buffer until a newline, and hands the completed line to
   `handleLine()`; every iteration also calls `.run()` on all 7 steppers
   (not just the selected one), so a stepper mid-move when the operator
   switches selection still completes its move instead of freezing
   part-way.
3. **`homeVSI()`**: winds VSI hard against its negative mechanical end stop
   (`-STEPS * 1.1`, deliberately overshooting so it reaches the real stop
   regardless of exactly how many steps that is) and zeroes it there
   (`setCurrentPosition(0)`) — the same homing move
   `JET_RANGER_STEPPER_CONTROLLER.ino`'s own startup sequence performs for
   VSI. Runs once automatically at boot (before the menu is printed), and
   again on demand any time the `h` command is sent. Logs `"Homing VSI -
   winding to end stop"` before and `"VSI end stop reached, zeroed at step
   0"` after.
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
   - `z` — zeroes the currently selected stepper at wherever it is
     physically sitting right now (`setCurrentPosition(0)`), useful for
     establishing a known reference point before hunting for min/max. Logs
     `"Zeroed <name> at its current physical position"`.
   - `h` — re-runs `homeVSI()` on demand, but only while VSI (`s0`) is
     selected; prints a message explaining it isn't implemented for any
     other stepper yet otherwise (not logged - `homeVSI()` logs its own
     lines regardless of how it was triggered).
   - Any other integer (optionally signed, e.g. `1500` or `-200`) — calls
     `.moveTo()` on the currently selected stepper. Can be sent as many
     times as needed; each one just updates the target. Logs `"<name>
     target set to <value>"`.

## Pin usage

Identical wiring to `JET_RANGER_STEPPER_CONTROLLER.ino`'s 7 "simple"
`AccelStepper` gauges (the SARI roll stepper is intentionally not included
— see below):

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
| `172.16.1.10` (reflector host) | 27000 | Debug/log messages (`SendDebug`) — Ethernet-started, setup-complete, VSI homing, stepper selection, zero, and move-to-target events |

## Build verification

Compiled clean with `arduino-cli` (target `arduino:avr:mega:cpu=atmega2560`,
**AccelStepper** 1.64.0, **Ethernet** 2.0.2): 20,344 bytes flash (8%),
1,265 bytes RAM (15%).

## C# / other programs this sketch communicates with

- No C# project in this repository listens on `172.16.1.10:27000` — the
  debug log stream has no in-repo consumer, the same gap noted for every
  other Jet Ranger board.
- Values found with this harness (min/max/zero step positions per gauge)
  are meant to be copied by hand into
  **[JET_RANGER_STEPPER_CONTROLLER](../JET_RANGER_STEPPER_CONTROLLER/PROGRAM_SUMMARY.md)**
  once established, the same way `ServoTuner`/`Servo-Knob-For-Calibration`
  findings feed into `JET_RANGER_SERVO_CONTROLLER`'s servo tables.
