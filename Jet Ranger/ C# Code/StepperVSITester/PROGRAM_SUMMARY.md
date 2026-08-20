# StepperVSITester — Program Summary

Standalone WinForms tool that lets an operator scrub or type real-unit
values (VSI fpm, ALT feet, Radar ALT feet, IAS knots, EGT/ITT degrees,
RPME/RPMR percent, and 6 more via compact rows) or raw step counts, jog
the Altimeter by a raw step count at a fixed interval, and send it all
straight to the
stepper board's UDP handlers, without needing FSUIPC or a flight sim
running — the stepper board's equivalent of `ServoTuner`, but for
several fields on the Stepper Controller rather than the whole Servo
Controller. Points at `172.16.1.105:13136`, an address/port shared by
two mutually-exclusive sketches (only one is ever flashed to the board
at a time):
[`Jet_Ranger_Driver_Test.ino`](../../Jet%20Ranger%20Arduino%20Sketches/test/Jet_Ranger_Driver_Test/PROGRAM_SUMMARY.md)
(the bench-test fork) and
[`JET_RANGER_STEPPER_CONTROLLER.ino`](../../Jet%20Ranger%20Arduino%20Sketches/JET_RANGER_STEPPER_CONTROLLER/PROGRAM_SUMMARY.md)
(production). (Name predates the ALT/Radar ALT/jog/Raw Step Test sections
being added; kept as-is rather than renaming mid-project.)

Built because `FSUIPCWinformsAutoCS` now sends the Stepper Controller raw
fpm for `VSI` (its own front-panel/servo payload still sends the Bell 206
`VSI_Process()` servo-position number, unchanged) and raw feet for `ALT`
(unchanged from what it always sent) — this tool sends exactly what that
board's stepper-specific payload sends for both fields, so VSI/ALT
stepper behavior can be exercised/tuned in isolation. The Radar ALT, jog,
EGT/IAS/RPME/RPMR/real-value-row, and Raw Step Test sections were added
later for bench-testing features that don't have a real flight-sim data
source yet or hadn't been calibrated at the time.

> **`AGL` (Radar Alt) means different things depending on which sketch is
> flashed:** `Jet_Ranger_Driver_Test.ino` treats it as calibrated feet
> (via `RADAR_ALT_FT_TABLE`); `JET_RANGER_STEPPER_CONTROLLER.ino` treats
> it as a raw step target (no calibration yet). The Radar ALT slider
> below is unit-labelled "ft" and only actually produces feet on the
> bench-test sketch - on production the same values get raw-stepped
> instead. (Renamed from `RALT` to `AGL` on both sketches to match
> `JET_RANGER_SERVO_CONTROLLER.ino`'s code for the same quantity - this
> tool's control name/label didn't change, only the wire code it sends.)
> Worth reconciling units once the production gauge is bench-measured.
>
> **`FLAPS`/`AOA`/`GFORCE`/`SPDMAX` (still in the Raw Step Test dropdown)
> are currently no-ops on `JET_RANGER_STEPPER_CONTROLLER.ino`:** the
> steppers those codes used to reach (`FlapsStepper`/`AOAstepper`/
> `GForcestepper`/`SpeedMaxstepper`) have been removed from that sketch
> entirely, along with their `HandleOutputValuePair` cases. Sending them
> from here does nothing on the current production board.

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
   sends `"D,ALT:<value>"` to **both** `stepperClient` (`172.16.1.105`)
   and `dualStepperClient` (`172.16.1.106`) immediately on every scroll,
   mirrors into `txtAltInput`. Broadcasts because
   `JET_RANGER_OLED_DUAL_STEPPER_CONTROLLER.ino`'s `ALTstepper` was revived
   and its `"ALT"`/`"ALTRAW"` UDP cases enabled specifically so this
   board could be driven the same way as the single-board sketch
   (previously it was DCS-BIOS-only, unreachable over UDP at all).
6. **ALT "Send" (`butSendAlt_Click`)/Enter key in `txtAltInput`**: same
   pattern as VSI's, validated against `trkAlt`'s `Minimum`/`Maximum`,
   via `SendManualValueBroadcast()` (also broadcast).
7. **ALT "Zero" (`butAltZero_Click`)**: resets `trkAlt`/`txtAltInput` to
   `0` and sends it (also broadcast).
8. **Radar ALT trackbar (`trkRadarAlt_Scroll`)**: range `0..2500` ft,
   matching the board's `RADAR_ALT_FT_TABLE` calibration range exactly
   (values outside it clamp on the board side rather than erroring
   here). Sends `"D,AGL:<value>"` (renamed from `RALT` — see the caution
   above), mirrors into `txtRadarAltInput`.
9. **Radar ALT "Send" (`butSendRadarAlt_Click`)/Enter key/"Zero"
   (`butRadarAltZero_Click`)**: same pattern as ALT's.
10. **ALT Direct Step Jog (`butJogSend_Click`)**: reads a signed
    step count from `txtJogSteps` and a positive interval (ms) from
    `txtJogInterval`, validates both, and calls `SendAltJog()`, which
    packs them as `"D,ASTEP:<steps>/<intervalMs>"` — a single UDP code
    carrying two numbers, since the board's `"D,CODE:value"` packet
    parser only reads one value per code. Enter in either textbox
    triggers the same send. This drives `jogAltimeterSteps()` on the
    board, which bit-bangs the ALT step/dir pins directly at a literal
    fixed interval, bypassing `AccelStepper`'s acceleration ramp — for
    finding exact step timing, not normal use.
11. **EGT/ITT trackbar (`trkEgt_Scroll`) + Send/Zero**: range `0..900` °C,
    sends `"D,ITT:<value>"` (wire code is `ITT`, matching
    `JET_RANGER_SERVO_CONTROLLER.ino`; control/field names in code are
    still `Egt*` — the first gauge in this tool to graduate from the Raw
    Step Test dropdown to a dedicated real-unit control once its
    calibration became known).
12. **IAS trackbar (`trkIas_Scroll`) + Send/Zero**: range `0..140` kt,
    sends `"D,IAS:<value>"`. On the board this goes through a real
    hand-measured `IAS_KT_TABLE` (9 points), not a placeholder scale.
13. **RPME trackbar (`trkRpme_Scroll`) + Send/Zero**: range `0..117` %,
    sends `"D,RPME:<value>.0"` (Turbine/Engine Speed) to **both**
    `stepperClient` (`172.16.1.105`) and `dualStepperClient`
    (`172.16.1.106`) via `SendPercentManualValueBroadcast()` (Send
    button/Enter) or the double-typed `Send()`/`SendDual()` overloads
    directly (trackbar scroll/Zero) - `JET_RANGER_STEPPER_CONTROLLER.ino`
    and `JET_RANGER_OLED_DUAL_STEPPER_CONTROLLER.ino` both drive this gauge
    identically (same `TS_PCT_TABLE`, since Turbine Speed wasn't one of
    the Dual Stepper board's repurposed gauges), so one trackbar drives
    both boards' Turbine Speed steppers together. Graduated here from a
    compact value-box row (see #15 below) once the calibration table
    existed, same as EGT/IAS did earlier. **RPME/RPMR are the only two
    codes in this tool sent with one decimal place** (e.g. `"82.0"`,
    matching `FSUIPCWinformsAutoCS`'s own one-decimal RPMR/RPME wire
    format) - the trackbar itself still only offers whole-percent
    granularity (`TrackBar` has no fractional steps), so this only
    changes what's put on the wire, not what the operator can dial in
    here. Every other code in this tool remains a bare integer via the
    original `long`-typed `Send()`/`SendDual()`/`SendManualValue()`/
    `SendManualValueBroadcast()` (still used unchanged by ALT and
    everything else).
14. **RPMR trackbar (`trkRpmr_Scroll`) + Send/Zero**: range `0..117` %,
    sends `"D,RPMR:<value>.0"` (Rotor Speed) to **both** boards, same
    broadcast pattern and one-decimal wire format as RPME above
    (`RS_PCT_TABLE`, same values as `TS_PCT_TABLE`, unchanged on the
    Dual Stepper board too).
15. **Real-Value Gauges compact rows** (`butSendEot`/`butSendEop`/
    `butSendXot`/`butSendXop`/`butSendGp`/`butSendFa`, each with a matching
    `txt*`/Enter handler): six more gauges with known real-world units but
    no dedicated trackbar (would have made the form impractically tall) —
    `OILT` (°C), `OILP` (PSI), `XMSNT` (°C), `XMSNP` (PSI), `N1` (%), `FUEL`
    (US gal), all still on the "uncalibrated linear scale" placeholder.
    Each control's C# name is the pre-rename short code (`Eot`/`Eop`/etc.)
    but sends the servo-controller-aligned wire code (`OILT`/`OILP`/etc.)
    via the shared `SendRealValue()` helper — same "control name unchanged,
    wire code renamed" pattern as EGT/IAS/AGL above. (`TS`/`RS` used to be
    here too, as `RPME`/`RPMR` — see #13/#14 above.)
16. **Raw Step Test panel** (`cboNewGauge` dropdown + `txtNewGaugeSteps` +
    `butNewGaugeSend`/`butNewGaugeZero`, renamed from "New Gauges"): one
    shared raw-step control covering two groups of codes - `TQ`/`FLAPS`/
    `AOA`/`GFORCE`/`SPDMAX` (no real calibration at all; the latter four
    are currently no-ops on production, see the caution above), and the
    distinct `*RAW` siblings of every calibrated gauge (`IASRAW`/
    `ALTRAW`/`VSIRAW`/`OILTRAW`/`OILPRAW`/`XMSNTRAW`/`XMSNPRAW`/`ITTRAW`/
    `RPMERAW`/`RPMRRAW`/`N1RAW`/`FUELRAW`/`FUELLOADRAW`/
    `ELECTRICALLOADRAW`), letting the operator bypass a gauge's unit
    conversion for bench testing without losing its real-value control.
    `FUELLOADRAW`/`ELECTRICALLOADRAW` were added once #18 below graduated
    those two codes to real units. Every send from this panel goes to
    **both** `stepperClient` (`172.16.1.105`) and `dualStepperClient`
    (`172.16.1.106`) - `JET_RANGER_OLED_DUAL_STEPPER_CONTROLLER.ino` accepts
    all of these except `AGLRAW`/`TQ` (that board's Radar Alt/Torque
    steppers were repurposed - see #18 below), which are silent no-ops on
    it, same as `FLAPS`/`AOA`/`GFORCE`/`SPDMAX` already are on both
    boards. `FUELLOADRAW`/`ELECTRICALLOADRAW` are the mirror image - they
    only exist on `172.16.1.106` (`FuelLoadStepper`/`ElectricalLoadStepper`
    aren't declared on the single-board sketch), so they're silent no-ops
    on `172.16.1.105`. `SelectedNewGaugeCode()` centralises the
    dropdown-selection fallback (`"TQ"`, matching the dropdown's actual
    first item - was a stale `"EOT"` fallback left over from before `EOT`
    graduated to its own row).
17. **-1 Step / +1 Step buttons** (`butNewGaugeStepBack_Click`/
    `butNewGaugeStepFwd_Click`): nudge `txtNewGaugeSteps`'s value by ±1
    and resend via the same `SelectedNewGaugeCode()` (also broadcast to
    both boards) - since every code in this panel is an absolute
    `.moveTo()` target on the board (not a relative move), "one step"
    only means one physical step if the stepper actually reached the
    previous target before the next click (`AccelStepper`'s acceleration
    ramp takes a moment).
18. **Dual Stepper Test rows** (`butSendFuelLoad`/`butSendElectricalLoad`,
    each with a matching `txt*`/Enter handler): sends `FUELLOAD` (PSI) and
    `ELECTRICALLOAD` (%) real values **exclusively** to `dualStepperClient`
    (`172.16.1.106:13136`) via `SendDual()`/`SendDualValue()` (renamed from
    `SendDualRawValue()` - the method itself never did any unit conversion,
    only the meaning of the value on the board side changed) - unlike
    #16/#17 and RPME/RPMR above, these two are never sent to
    `172.16.1.105`, since they have no equivalent there. These two codes
    only exist on
    [`JET_RANGER_OLED_DUAL_STEPPER_CONTROLLER.ino`](../../Jet%20Ranger%20Arduino%20Sketches/JET_RANGER_OLED_DUAL_STEPPER_CONTROLLER/PROGRAM_SUMMARY.md)
    (a fork of the production sketch, its own board, its own IP) - that
    sketch repurposed its Radar Alt and Torque steppers as Fuel Load and
    Electrical Load respectively. Both now have a real bench-measured
    calibration table (`FUEL_LOAD_PSI_TABLE`: 0 PSI→0 steps, 30 PSI→160
    steps; `ELECTRICAL_LOAD_PCT_TABLE`: 0%→22 steps, 100%→200 steps) - raw
    steps are still reachable via the `FUELLOADRAW`/`ELECTRICALLOADRAW`
    entries in the Raw Step Test dropdown (#16 above).
19. **Clock row** (`txtClockHour`/`txtClockMinute` + `butSendClock`, each
    with a matching Enter handler on both textboxes): two small textboxes
    (hour 0-23, minute 0-59, each validated separately) combined into the
    HHMM-encoded `ZULU` wire value (e.g. `1430` for 14:30) and sent
    **exclusively** to `dualStepperClient` (`172.16.1.106:13136`) via
    `SendClockValue()`/`SendDual()` - same "exclusive to this board"
    pattern as the Dual Stepper Test rows above (#18), since `ZULU`
    only exists on
    [`JET_RANGER_OLED_DUAL_STEPPER_CONTROLLER.ino`](../../Jet%20Ranger%20Arduino%20Sketches/JET_RANGER_OLED_DUAL_STEPPER_CONTROLLER/PROGRAM_SUMMARY.md)
    (drives its Clock OLED, `u8g2_CLOCK`) - there's no equivalent gauge or
    code on `172.16.1.105`.

No unit conversion happens in this tool for VSI/ALT/Radar ALT/EGT/IAS/
RPME/RPMR/FUELLOAD/ELECTRICALLOAD — whatever value is shown is sent as-is
(RPME/RPMR get a trailing `.0` appended per the one-decimal wire format
above; every other code here is a bare integer). On the Arduino side
(`JET_RANGER_OLED_DUAL_STEPPER_CONTROLLER.ino`), `VSI`, `AGL` (bench-test
sketch only), `IAS`, `RPME`, `RPMR`, `FUELLOAD`, and `ELECTRICALLOAD` each
go through their own real calibration (`VSI_FPM_TABLE`/`vsiFpmToSteps()`,
`RADAR_ALT_FT_TABLE`/`radarAltFtToSteps()` on the bench-test sketch,
`IAS_KT_TABLE`/`iasKtToSteps()`, `TS_PCT_TABLE`/`tsPctToSteps()`,
`RS_PCT_TABLE`/`rsPctToSteps()`, `FUEL_LOAD_PSI_TABLE`/
`fuelLoadPsiToSteps()`, `ELECTRICAL_LOAD_PCT_TABLE`/
`electricalLoadPctToSteps()`), and a placeholder linear scale for
`ITT`/the 6 remaining compact-row gauges pending real calibration, while
`ALT` needs no such table — `onAltMslFtChange()`'s simple linear
`feet * 5.76` conversion already produces a correct step count directly
from raw feet.

## Local network configuration / IP addresses

None locally bound — this tool only sends, it doesn't listen for anything.

## Remote endpoints this app talks to

| Target | Port | Purpose |
|---|---|---|
| `172.16.1.105` (Stepper Controller, via `stepperClient`) | 13136 | `"D,VSI:<fpm>"`, `"D,ALT:<feet>"`, `"D,AGL:<feet or steps>"`, `"D,ITT:<C>"`, `"D,IAS:<kt>"`, `"D,RPME:<pct>.0"`, `"D,RPMR:<pct>.0"`, `"D,ASTEP:<steps>/<intervalMs>"`, `"D,<OILT\|OILP\|XMSNT\|XMSNP\|N1\|FUEL>:<value>"`, and `"D,<TQ\|FLAPS\|AOA\|GFORCE\|SPDMAX\|IASRAW\|ALTRAW\|VSIRAW\|OILTRAW\|OILPRAW\|XMSNTRAW\|XMSNPRAW\|ITTRAW\|RPMERAW\|RPMRRAW\|N1RAW\|FUELRAW\|FUELLOADRAW\|ELECTRICALLOADRAW>:<steps>"` test packets (`FUELLOADRAW`/`ELECTRICALLOADRAW` are silent no-ops here - see #16 above) |
| `172.16.1.106` (Dual Stepper Controller, via `dualStepperClient`) | 13136 | `"D,FUELLOAD:<psi>"`, `"D,ELECTRICALLOAD:<pct>"`, `"D,ZULU:<HHMM>"` (all exclusive to this board); `"D,ALT:<feet>"`/`"D,RPME:<pct>.0"`/`"D,RPMR:<pct>.0"` (broadcast alongside `172.16.1.105` - `RPME`/`RPMR` via `SendPercentManualValueBroadcast()`/the double `Send()`/`SendDual()` overloads, `ALT` via the original `long`-typed `SendManualValueBroadcast()`; `ALT` works here since this board's `ALTstepper`/`ALT` UDP case were re-enabled specifically for it); and every Raw Step Test panel code (`"D,<TQ\|FLAPS\|AOA\|GFORCE\|SPDMAX\|IASRAW\|ALTRAW\|VSIRAW\|OILTRAW\|OILPRAW\|XMSNTRAW\|XMSNPRAW\|ITTRAW\|RPMERAW\|RPMRRAW\|N1RAW\|FUELRAW\|FUELLOADRAW\|ELECTRICALLOADRAW>:<steps>"`, also broadcast alongside `172.16.1.105` - `AGLRAW`/`TQ` are no-ops on this board, `ALTRAW` now works, `FUELLOADRAW`/`ELECTRICALLOADRAW` are new and only work here) |

## Programs this communicates with

- **[Jet_Ranger_Driver_Test](../../Jet%20Ranger%20Arduino%20Sketches/test/Jet_Ranger_Driver_Test/PROGRAM_SUMMARY.md)**
  (`172.16.1.105:13136`) — meant to be run *instead of*
  `FSUIPCWinformsAutoCS` while testing/tuning VSI, ALT, or Radar ALT
  (calibrated feet on this sketch), since both would otherwise fight over
  what's currently driving the stepper.
- **[JET_RANGER_STEPPER_CONTROLLER](../../Jet%20Ranger%20Arduino%20Sketches/JET_RANGER_STEPPER_CONTROLLER/PROGRAM_SUMMARY.md)**
  (`172.16.1.105:13136`, same address — mutually exclusive with the
  sketch above) — the Raw Step Test panel and the ASTEP jog only exist on
  one of these two sketches each (Raw Step Test: production only; ASTEP:
  bench-test only), so which one is actually flashed to the board
  determines which of this tool's controls do anything. Production also
  has real-unit calibration (`IAS_KT_TABLE`) that the bench-test sketch
  doesn't share.
- **[JET_RANGER_OLED_DUAL_STEPPER_CONTROLLER](../../Jet%20Ranger%20Arduino%20Sketches/JET_RANGER_OLED_DUAL_STEPPER_CONTROLLER/PROGRAM_SUMMARY.md)**
  (`172.16.1.106:13136` — a distinct board/address, not mutually exclusive
  with the two above) — the Dual Stepper Raw Test rows (`FUELLOAD`/
  `ELECTRICALLOAD`) and the Clock row (`ZULU`) talk to this board
  exclusively; the ALT and RPME/RPMR trackbars and the entire Raw Step
  Test panel broadcast to it alongside `172.16.1.105` (this sketch's
  `ALTstepper` was revived and its `ALT`/`ALTRAW` UDP cases enabled
  specifically to receive the ALT trackbar's broadcasts). Only the
  VSI/Radar ALT/EGT/IAS trackbars, the six remaining real-value compact
  rows (`OILT`/`OILP`/`XMSNT`/`XMSNP`/`N1`/`FUEL`), and the ALT jog still
  target `172.16.1.105` only.
