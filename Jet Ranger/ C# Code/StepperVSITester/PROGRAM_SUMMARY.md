# StepperVSITester — Program Summary

Standalone WinForms tool that lets an operator scrub or type raw VSI
(fpm), ALT (feet), and Radar ALT (feet) values, jog the Altimeter by a
raw step count at a fixed interval, and send it all straight to the
stepper board's UDP handlers, without needing FSUIPC or a flight sim
running — the stepper board's equivalent of `ServoTuner`, but for
several fields on the Stepper Controller rather than the whole Servo
Controller. Currently pointed at
[`Jet_Ranger_Driver_Test.ino`](../../Jet%20Ranger%20Arduino%20Sketches/test/Jet_Ranger_Driver_Test/PROGRAM_SUMMARY.md),
the bench-test fork of `JET_RANGER_STEPPER_CONTROLLER.ino`, at the same
static IP/port both sketches share. (Name predates the ALT/Radar
ALT/jog sections being added; kept as-is rather than renaming
mid-project.)

Built because `FSUIPCWinformsAutoCS` now sends the Stepper Controller raw
fpm for `VSI` (its own front-panel/servo payload still sends the Bell 206
`VSI_Process()` servo-position number, unchanged) and raw feet for `ALT`
(unchanged from what it always sent) — this tool sends exactly what that
board's stepper-specific payload sends for both fields, so VSI/ALT
stepper behavior can be exercised/tuned in isolation. The Radar ALT and
Direct Step Jog sections were added later for bench-testing two new
board-side features that don't have a real flight-sim data source yet.

## Program flow

The VSI/ALT/Radar ALT sections work identically, just with different
ranges/units/UDP codes; `Send()`/`SendManualValue()`/`UpdateValueLabel()`
are shared helpers parameterised by which trackbar/textbox/label/code/
unit to use.

1. **Constructor**: connects `stepperClient` to the Stepper Controller,
   sets all three trackbars/labels/textboxes to `0`.
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
8. **Radar ALT trackbar (`trkRadarAlt_Scroll`)** *(new)*: range `0..2500`
   ft, matching the board's `RADAR_ALT_FT_TABLE` calibration range
   exactly (values outside it clamp on the board side rather than erroring
   here). Sends `"D,RALT:<value>"`, mirrors into `txtRadarAltInput`.
9. **Radar ALT "Send" (`butSendRadarAlt_Click`)/Enter key/"Zero"
   (`butRadarAltZero_Click`)** *(new)*: same pattern as ALT's.
10. **ALT Direct Step Jog (`butJogSend_Click`)** *(new)*: reads a signed
    step count from `txtJogSteps` and a positive interval (ms) from
    `txtJogInterval`, validates both, and calls `SendAltJog()`, which
    packs them as `"D,ASTEP:<steps>/<intervalMs>"` — a single UDP code
    carrying two numbers, since the board's `"D,CODE:value"` packet
    parser only reads one value per code. Enter in either textbox
    triggers the same send. This drives `jogAltimeterSteps()` on the
    board, which bit-bangs the ALT step/dir pins directly at a literal
    fixed interval, bypassing `AccelStepper`'s acceleration ramp — for
    finding exact step timing, not normal use.

No unit conversion happens in this tool for VSI/ALT/Radar ALT — whatever
value is shown is sent as-is. On the Arduino side, `VSI` and `RALT` each
go through their own real calibration table (`VSI_FPM_TABLE`/
`vsiFpmToSteps()`, `RADAR_ALT_FT_TABLE`/`radarAltFtToSteps()` — see the
Driver Test sketch's own summary), while `ALT` needs no such table —
`onAltMslFtChange()`'s simple linear `feet * 5.76` conversion (shared
with the DCS-BIOS path) already produces a correct step count directly
from raw feet.

## Local network configuration / IP addresses

None locally bound — this tool only sends, it doesn't listen for anything.

## Remote endpoints this app talks to

| Target | Port | Purpose |
|---|---|---|
| `172.16.1.105` (Stepper Controller) | 13136 | `"D,VSI:<fpm>"`, `"D,ALT:<feet>"`, `"D,RALT:<feet>"`, and `"D,ASTEP:<steps>/<intervalMs>"` test packets |

## Programs this communicates with

- **[Jet_Ranger_Driver_Test](../../Jet%20Ranger%20Arduino%20Sketches/test/Jet_Ranger_Driver_Test/PROGRAM_SUMMARY.md)**
  (`172.16.1.105:13136`) — the sole target; meant to be run *instead of*
  `FSUIPCWinformsAutoCS` while testing/tuning VSI, ALT, or Radar ALT,
  since both would otherwise fight over what's currently driving the
  stepper.
