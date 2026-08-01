# StepperVSITester — Program Summary

Standalone WinForms tool that lets an operator scrub or type a raw fpm
value and send it straight to `JET_RANGER_STEPPER_CONTROLLER.ino`'s `VSI`
UDP handler, without needing FSUIPC or a flight sim running — the stepper
board's equivalent of `ServoTuner`, but for a single field on the Stepper
Controller rather than the whole Servo Controller.

Built because `FSUIPCWinformsAutoCS` now sends the Stepper Controller raw
fpm for `VSI` (its own front-panel/servo payload still sends the Bell 206
`VSI_Process()` servo-position number, unchanged) — this tool sends
exactly what that board's stepper-specific payload now sends, so VSI
stepper behavior can be exercised/tuned in isolation.

## Program flow

1. **Constructor**: connects `stepperClient` to the Stepper Controller,
   sets the trackbar/label/textbox to `0`.
2. **Trackbar (`trkVsi_Scroll`)**: range `-1750..1750`, sends
   `"D,VSI:<value>"` immediately on every scroll, and mirrors the value
   into `txtRawInput`.
3. **"Send" (`butSendRaw_Click`)/Enter key in the textbox**: parses
   `txtRawInput`, checks it's within the trackbar's `Minimum`/`Maximum`
   (`-1750..1750`, read from the control rather than hard-coded), updates
   the trackbar to match, and sends the same way as the trackbar.
4. **"Zero" (`butZero_Click`)**: resets the trackbar/textbox to `0` and
   sends it — quick way back to level flight without dragging.

No unit conversion happens in this tool — whatever fpm value is shown is
sent as-is in the `VSI:` field, exactly matching what
`FSUIPCWinformsAutoCS`'s stepper-specific payload now sends. The Arduino
side now applies a real fpm→step calibration to it (`VSI_FPM_TABLE`/
`vsiFpmToSteps()` — see `JET_RANGER_STEPPER_CONTROLLER`'s own summary),
the same hand-measured table `Stepper-Tuning-Harness` uses for its own `f`
command, so values sent from this tool should now move the needle
proportionally across the full `-1750..1750` range rather than pegging at
a placeholder clamp.

## Local network configuration / IP addresses

None locally bound — this tool only sends, it doesn't listen for anything.

## Remote endpoints this app talks to

| Target | Port | Purpose |
|---|---|---|
| `172.16.1.105` (Stepper Controller) | 13136 | `"D,VSI:<fpm>"` test packets |

## Programs this communicates with

- **[JET_RANGER_STEPPER_CONTROLLER](../../Jet%20Ranger%20Arduino%20Sketches/JET_RANGER_STEPPER_CONTROLLER/PROGRAM_SUMMARY.md)**
  (`172.16.1.105:13136`) — the sole target; meant to be run *instead of*
  `FSUIPCWinformsAutoCS` while testing/tuning VSI, since both would
  otherwise fight over what's currently driving the stepper.
