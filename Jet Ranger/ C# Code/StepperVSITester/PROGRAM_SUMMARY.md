# StepperVSITester — Program Summary

Standalone WinForms tool that lets an operator scrub or type raw VSI
(fpm) and ALT (feet) values and send them straight to
`JET_RANGER_STEPPER_CONTROLLER.ino`'s `VSI`/`ALT` UDP handlers, without
needing FSUIPC or a flight sim running — the stepper board's equivalent
of `ServoTuner`, but for two fields on the Stepper Controller rather than
the whole Servo Controller. (Name predates the ALT section being added;
kept as-is rather than renaming mid-project.)

Built because `FSUIPCWinformsAutoCS` now sends the Stepper Controller raw
fpm for `VSI` (its own front-panel/servo payload still sends the Bell 206
`VSI_Process()` servo-position number, unchanged) and raw feet for `ALT`
(unchanged from what it always sent) — this tool sends exactly what that
board's stepper-specific payload sends for both fields, so VSI/ALT
stepper behavior can be exercised/tuned in isolation.

## Program flow

Both sections work identically, just with different ranges/units/UDP
codes; `Send()`/`SendManualValue()`/`UpdateValueLabel()` are shared
helpers parameterised by which trackbar/textbox/label/code/unit to use.

1. **Constructor**: connects `stepperClient` to the Stepper Controller,
   sets both trackbars/labels/textboxes to `0`.
2. **VSI trackbar (`trkVsi_Scroll`)**: range `-1750..1750` fpm, sends
   `"D,VSI:<value>"` immediately on every scroll, and mirrors the value
   into `txtRawInput`.
3. **VSI "Send" (`butSendRaw_Click`)/Enter key in `txtRawInput`**: parses
   the textbox, checks it's within `trkVsi`'s `Minimum`/`Maximum`
   (`-1750..1750`, read from the control rather than hard-coded), updates
   the trackbar to match, and sends the same way as the trackbar.
4. **VSI "Zero" (`butZero_Click`)**: resets `trkVsi`/`txtRawInput` to `0`
   and sends it — quick way back to level flight without dragging.
5. **ALT trackbar (`trkAlt_Scroll`)**: range `0..20000` ft (no firmware-side
   clamp exists for ALT to match against — `onAltMslFtChange()`'s
   `feet * 5.76` conversion accepts anything — so this is just a
   generously-sized test range, not a hard limit from the hardware),
   sends `"D,ALT:<value>"` immediately on every scroll, mirrors into
   `txtAltInput`.
6. **ALT "Send" (`butSendAlt_Click`)/Enter key in `txtAltInput`**: same
   pattern as VSI's, validated against `trkAlt`'s `Minimum`/`Maximum`.
7. **ALT "Zero" (`butAltZero_Click`)**: resets `trkAlt`/`txtAltInput` to
   `0` and sends it.

No unit conversion happens in this tool for either field — whatever
value is shown is sent as-is, exactly matching what
`FSUIPCWinformsAutoCS`'s stepper-specific payload sends. On the Arduino
side, `VSI` goes through a real fpm→step calibration (`VSI_FPM_TABLE`/
`vsiFpmToSteps()` — see `JET_RANGER_STEPPER_CONTROLLER`'s own summary),
the same hand-measured table `Stepper-Tuning-Harness` uses for its own
`f` command. `ALT` needs no such table — `onAltMslFtChange()`'s simple
linear `feet * 5.76` conversion (shared with the DCS-BIOS path) already
produces a correct step count directly from raw feet.

## Local network configuration / IP addresses

None locally bound — this tool only sends, it doesn't listen for anything.

## Remote endpoints this app talks to

| Target | Port | Purpose |
|---|---|---|
| `172.16.1.105` (Stepper Controller) | 13136 | `"D,VSI:<fpm>"` and `"D,ALT:<feet>"` test packets |

## Programs this communicates with

- **[JET_RANGER_STEPPER_CONTROLLER](../../Jet%20Ranger%20Arduino%20Sketches/JET_RANGER_STEPPER_CONTROLLER/PROGRAM_SUMMARY.md)**
  (`172.16.1.105:13136`) — the sole target; meant to be run *instead of*
  `FSUIPCWinformsAutoCS` while testing/tuning VSI or ALT, since both would
  otherwise fight over what's currently driving the stepper.
