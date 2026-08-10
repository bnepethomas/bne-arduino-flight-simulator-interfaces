# JET_RANGER_STEPPER_CONTROLLER — Program Summary

> **Note:** as with `JET_RANGER_OLED_CONTROLLER`, this sketch's own header
> identifies it as **`A10_FRONT_CONSOLE_STEPPERS`**, front-center of an
> **A-10C Warthog** DCS World pit — not the Jet Ranger helicopter. Its data
> source is **DCS-BIOS** (serial), not the Jet Ranger's `172.16.1.x`
> UDP/CSV protocol. This folder also contains a second, unrelated-looking
> sketch nested inside it (`A10_LEFT_CONSOLE_INPUT_CONTROLLER_A`, covered
> below) — both look like they belong to the same A-10C pit build rather
> than the Jet Ranger fleet, worth confirming with whoever filed them here.

Two Arduino sketches live under this folder:

1. **`JET_RANGER_STEPPER_CONTROLLER.ino`** (this summary's main subject) —
   drives 18 analogue stepper-motor gauges (7 original + 11 newly ported
   from `Stepper-Tuning-Harness`, see below) plus backlighting for an
   A-10C front instrument panel.
2. **`A10_LEFT_CONSOLE_INPUT_CONTROLLER_A/A10_LEFT_CONSOLE_INPUT_CONTROLLER_A.ino`**
   — a separate 176-button matrix input controller for TACAN/ILS/electrical/
   CDU/AAP/lighting panels (see its own section below).

## Build verification

Both sketches in this folder were compiled with `arduino-cli` (target
`arduino:avr:mega:cpu=atmega2560`) and build with **0 errors**:

| Sketch | Flash | RAM |
|---|---|---|
| `JET_RANGER_STEPPER_CONTROLLER.ino` | 25,038 bytes (9%) | 3,639 bytes (44%) |
| `A10_LEFT_CONSOLE_INPUT_CONTROLLER_A.ino` | 23,586 bytes (9%) | 4,962 bytes (60%) |

Flashed to the bench Mega on COM4 (the same physical board the
`Stepper-Tuning-Harness` sketch uses as a drop-in stand-in) after the
VSI/Flaps hardware-swap reanalysis below, again after adding `VSI` UDP
support, and again after adding the `VSI_FPM_TABLE` calibration (see
program flow below). The flash/RAM drop between the `VSI` UDP support
commit and this one is a clean-build artifact (a stale incremental-build
cache inflated the prior number — confirmed by re-running with
`--clean`), not a functional regression; the diff between those two
states is purely additive.

> **Not flashed:** the 11 new gauges added in "New gauges ported from
> Stepper-Tuning-Harness" below were only compile-checked — the bench
> Mega wasn't connected when they were added, so this board is currently
> running whatever was flashed before that change.

Library versions used for this verification: **Ethernet** 2.0.2,
**AccelStepper** 1.64.0, **DCS-BIOS** 0.3.13 (Arduino Library Manager). The
`dcs-bios-arduino-library-0.3.7.zip` bundled inside the
`A10_LEFT_CONSOLE_INPUT_CONTROLLER_A` folder is an older release of the
same library — the Library Manager's 0.3.13 was used for this check and
compiled without needing any code changes, but the two haven't been
diffed against each other line-for-line.

## JET_RANGER_STEPPER_CONTROLLER.ino — Program flow

1. **Setup**
   - Flashes status LEDs, then (if `Ethernet_In_Use`) resets the W5500
     shield, brings up Ethernet on the static IP, and opens the debug UDP
     socket.
   - Ramps the instrument backlighting (`BACK_LIGHTS`, PWM) from full
     brightness down to off over ~4s, then back up to a working level.
   - Runs an elaborate homing/self-test sequence for each of the 5
     "simple" `AccelStepper` gauges (VSI, ALT, current airspeed, max
     airspeed, flaps) and the AOA/G-Force gauges: drive hard against one
     end, zero the position there, sweep through a test motion, and (for
     ALT specifically) home against a physical zero-sense switch
     (`ALTzeroSensePin`) rather than just a timed sweep.
   > **VSI/Flaps hardware swap (reanalysis, current focus):** VSI moved
   > from a geared `DRIVER`/STEP-DIR motor onto direct coils
   > (`AccelStepper::FULL4WIRE`), and Flaps took over VSI's old
   > `DRIVER`/STEP-DIR pins in exchange — a real wiring change on the
   > bench, not a software-only swap. Pin `#define`s were renamed to match
   > what they actually drive now (`VSIstepPin`/`VSIdirectionPin` →
   > `FlapsStepPin`/`FlapsDirectionPin`; `COIL_FLAPS_A..D` →
   > `COIL_VSI_A..D`) — the old names were actively misleading, including a
   > comment claiming the DRIVER pins were "unused" when they're now
   > Flaps'. VSI's homing sequence was switched from the geared `STEPS*1.1`
   > (~5544 steps, calibrated for the old motor) to the direct-drive
   > `FULL4WIRE_HOMING_STEPS` (315×2 = 630, no overshoot — renamed from
   > `FLAPS_STEP`, since it's no longer Flaps-specific), matching the same
   > FULL4WIRE-style homing already established for Flaps. **Not
   > independently verified against the new physical motor:** the homing
   > direction sign (kept unchanged from before the swap), and `VSIoffset`
   > (130 → 16) / `VSIMaxSteps` (2400 → 300), which were scaled down by the
   > same ~8× ratio as the step count as a rough estimate — confirm/
   > recalibrate all three on the bench. A duplicate, dead `#define STEPS
   > 10080` (shadowed by the real `STEPS 315*16` immediately after it, so
   > it never actually took effect) was also removed as incidental cleanup.
   - Starts DCS-BIOS (`DcsBios::setup()`) and sets backlighting to its
     normal running brightness.
2. **Main loop** (`loop()`)
   - Toggles the status LEDs every `FLASH_TIME` (300ms).
   - Pumps `DcsBios::loop()` — every gauge target still comes from DCS-BIOS
     callbacks by default.
   - Calls `updateSteppers()`, which calls `.run()` on all 7
     `AccelStepper` objects every loop iteration (required by the
     AccelStepper library to make non-blocking acceleration-controlled
     moves progress).
   - Every `incomingcheckinterval` (5ms), checks for an incoming UDP packet
     on `MSFSport` and, if present, passes it to
     `ProcessReceivedMSFSString()` — see below.
3. **DCS-BIOS callbacks** map A-10C SimVars to stepper `moveTo()` targets:
   `onFlapPosChange` (`A_10C_FLAP_POS`), `onAirspeedNeedleChange`
   (`A_10C_AIRSPEED_NEEDLE`), `onAirspeedMaxIasChange`
   (`A_10C_AIRSPEED_MAX_IAS`), `onVviChange` (`A_10C_VVI`, VSI),
   `onAoaUnitsChange` (address `0x1078`), `onAltMslFtChange`
   (`CommonData_ALT_MSL_FT`), `onAccelGChange` (address `0x1070`,
   G-force). Two lighting callbacks (`onIntFltInstLBrightChange` /
   `onIntConsoleLBrightChange`) drive `BACK_LIGHTS` PWM from DCS-BIOS
   brightness values (a third, `onIntFloodLBrightChange`, computes a
   brightness value but only logs it — no output pin is actually driven
   for flood lighting in this sketch).
4. **SARI roll stepper** — a custom `Nema8Stepper` class (extending
   `DcsBios::Int16Buffer`) implements a full closed-loop homing/tracking
   state machine for what's commonly the attitude-indicator "ball" roll
   axis: it seeks a zero position using an IR detector pin, then tracks
   DCS-BIOS position updates by computing the shortest angular delta
   (wrapping around `SARImaxSteps`) each time new data arrives. A
   companion `DcsBios::ServoOutput` (`saiPitch`, pin 9) drives the pitch
   axis of the same instrument as a plain hobby servo.
5. **UDP data receiver** (added to let this board also be driven the same
   way as `JET_RANGER_SERVO_CONTROLLER`, alongside its existing DCS-BIOS
   path): `ProcessReceivedMSFSString()` parses the same
   `"D,CODE:value,CODE:value,..."` CSV payload as the Servo Controller
   (`HandleOutputValuePair`/`HandleControlString`/`getValue` are near-verbatim
   ports of that sketch's versions). Three codes are wired up; a fourth
   (`IAS`) is coded but currently unreachable:
   - `IAS` → `setCurrentAirspeed(value)`, reusing the existing airspeed
     stepper mover. **`FSUIPCWinformsAutoCS` no longer sends this board an
     `IAS` field at all** (its stepper-specific payload was narrowed to
     `ALT`/`VSI`/`AGL` only, since this board has no gauges for anything
     else), so this branch is currently dead code reachable only if some
     other/future sender includes an `IAS` field. If it ever does: the
     other PC bridge apps (`P3D_to_UDP`/`SimConnect_to_UDP`) still send
     `IAS` already converted to a *Bell 206 servo-position* number via
     their `IAS_Process()` tables — not raw knots and not an A-10 stepper
     step count — so this would be a straight pass-through pending real
     calibration for this gauge.
   - `ALT` → `onAltMslFtChange(value)`, reusing the exact feet→steps
     conversion the DCS-BIOS altitude callback already uses. This one
     *is* unit-correct as-is, since the PC bridge apps send `ALT` as raw,
     unconverted feet — the same units this sketch's DCS-BIOS altitude
     handler expects.
   - `VSI` → `VSIstepper.moveTo(vsiFpmToSteps(value))`. **Unlike `IAS`,**
     `FSUIPCWinformsAutoCS` sends this board **raw fpm** for `VSI`
     specifically — its own front-panel/servo-controller payload still
     carries the Bell 206 `VSI_Process()` servo-position number unchanged,
     but builds a separate copy with the `VSI` field swapped to raw fpm
     before sending to this board (see that project's `timerMain_Tick`
     send block). `vsiFpmToSteps()` converts that raw fpm into a real step
     target via `VSI_FPM_TABLE`, an 11-row hand-measured calibration table
     (the same data as `Stepper-Tuning-Harness`'s `VSI_FT_TABLE` — that
     harness's `f` command uses "ft" as informal shorthand for this
     gauge's fpm units, not altitude), linearly interpolating between the
     two nearest rows and clamping to whichever end is nearest for values
     outside ±1750 fpm (never extrapolated). This bypasses `setVSI()`'s
     separate `±VSIMaxSteps` clamp entirely — that clamp remains in use
     only for the unrelated DCS-BIOS path (`onVviChange`).
   - `RALT`, `EOT`, `XOT`, `XOP`, `EGT`, `TS`, `RS`, `FA`, `ET`, `GP`, `EOP`
     *(new)* → raw step pass-through (`.moveTo(value)` directly, no
     conversion) onto the 11 gauges ported from `Stepper-Tuning-Harness`
     (see "New gauges ported from Stepper-Tuning-Harness" below). None of
     these have a real calibration table yet. **`RALT` is not the same
     code `FSUIPCWinformsAutoCS` sends for radar altitude** - that's
     `AGL` (see below), which this board still doesn't read - so `RALT`
     only ever fires from a manual test send (e.g.
     `StepperVSITester`'s "Radar ALT" control), never from live flight
     data, until one of the two is renamed to match the other.
   - Every other code is currently parsed and silently ignored, including
     `AGL` (radar altitude) — `FSUIPCWinformsAutoCS` sends it in every
     stepper packet alongside `ALT`/`VSI`, and this board now *has* a
     Radar Alt stepper (`RadarAltStepper`), but nothing wires `AGL` to it
     yet - see the `RALT`/`AGL` naming mismatch just above.

## New gauges ported from Stepper-Tuning-Harness

Eleven more `AccelStepper` objects were added, all `FULL4WIRE`
direct-drive, matching `Stepper-Tuning-Harness`'s pin assignments exactly
(see that sketch's own summary for the full history of how it arrived at
these pins): `RadarAltStepper`, `EOTstepper`, `XOTstepper`, `XOPstepper`,
`EGTstepper`, `TSstepper`, `RSstepper`, `FAstepper`, `ETstepper`,
`GPstepper`, `EOPstepper`. `RadarAltStepper`'s coil argument order (C, D,
A, B rather than A, B, C, D) is carried over as-is from the harness,
since that's whatever direction that sketch found to work on the bench.

Added as bare declarations only:
- Each gets `setMaxSpeed`/`setAcceleration` in `setup()` (same
  `STEPPER_MAX_SPEED`/`STEPPER_ACCELERATION` as every other stepper here)
  and a `.run()` call in `updateSteppers()`, so they're mechanically live.
- **No homing or startup routine** was added for any of them - unlike
  VSI/ALT/SpeedMax's elaborate wind-to-stop-and-sweep sequences in
  `setup()`, none of these 11 have a bench-confirmed end stop, direction,
  or zero reference yet.
- **No DCS-BIOS binding** exists for any of them - they're reachable only
  via the raw UDP test codes listed in the UDP data receiver section
  above.

`Current Airspeed` (`SpeedCurrentstepper`) and `VSI` already existed in
this sketch under those names before this change, with different
pins/interfaces than `Stepper-Tuning-Harness`'s rewired versions of the
same two gauges (that harness moved both to `FULL4WIRE` on different
pins - see its own summary) - this sketch's existing `SpeedCurrentstepper`
(`DRIVER`, pins 34/36) and `VSIstepper` (`FULL4WIRE`, `COIL_VSI_A..D` =
2/3/4/5) were **left untouched**, not resynced to the harness's newer
wiring.

> **Pin collisions (not resolved):** this sketch was never cleaned up the
> way `Stepper-Tuning-Harness` was (`ALTstepper`, `SpeedMaxstepper`,
> `FlapsStepper`, `AOAstepper`, `GForcestepper`, and
> `AllstepperEnablePin` are all still active here), so several of the 11
> new gauges' pins collide with them:
>
> | New gauge | Conflicting pin(s) | Collides with |
> |---|---|---|
> | Radar Alt | 34 | `SpeedCurrentstepPin` |
> | EOT | 48, 54 (A0), 56 (A2) | `FlapsDirectionPin`, `ALTzeroSensePin`, `AllstepperEnablePin` |
> | TS | 24, 26 | `AOAstepPin`, `GForcestepPin` |
> | RS | 28 | `GForcedirectionPin` |
> | FA | 2, 3, 4 | `COIL_VSI_A`/`COIL_VSI_B`/`COIL_VSI_C` |
> | ET | 36, 38 | `SpeedCurrentdirectionPin`, `SpeedMaxstepPin` |
> | GP | 40, 42 | `SpeedMaxdirectionPin`, `ALTstepPin` |
> | EOP | 44, 46 | `ALTdirectionPin`, `FlapsStepPin` |
>
> XOT, XOP, and EGT (all on A3-A14) are the only new gauges clear of
> every other pin in this sketch. None of this blocks compilation - it's
> a real-hardware wiring conflict, only relevant once both sides of a
> collision are physically wired at once.

## Pin usage (JET_RANGER_STEPPER_CONTROLLER.ino)

| Pin(s) | Function |
|---|---|
| 12 | Red status LED |
| 13 | Green status LED |
| 53 | W5500 Ethernet shield manual reset |
| 8 | Backlighting PWM output (`BACK_LIGHTS`) |
| 9 | SARI pitch servo (`DcsBios::ServoOutput saiPitch`) |
| 22, 24 | AOA stepper step/direction |
| 26, 28 | G-Force stepper step/direction |
| 30, 32 | SARI roll stepper step/direction |
| 34, 36 | Current-airspeed stepper step/direction |
| 38, 40 | Max-airspeed stepper step/direction |
| 42, 44 | Altimeter stepper step/direction |
| 46, 48 | Flaps stepper step/direction (`FlapsStepPin`/`FlapsDirectionPin`) — was VSI's pins before the VSI/Flaps hardware swap above |
| 54 | Altimeter zero-sense homing switch input (`ALTzeroSensePin`) |
| 55 | SARI roll IR zero-detector input |
| 56 | Shared stepper-driver enable pin (`AllstepperEnablePin`/`SARIenablePin`) |
| 2, 3, 4, 5 | VSI 4-wire stepper coils (`COIL_VSI_A..D`) — was Flaps' pins before the VSI/Flaps hardware swap above |
| Serial (USB) | DCS-BIOS `DCSBIOS_IRQ_SERIAL` link — the data source for every stepper/servo target |
| 32, 33, 34, 35 | Radar Alt 4-wire stepper coils (`RADAR_ALT_COIL_A..D`) — 34 collides with `SpeedCurrentstepPin` above |
| 48, A0, A1, A2 (54/55/56) | EOT 4-wire stepper coils (`EOT_COIL_A..D`) — 48/54/56 collide with `FlapsDirectionPin`/`ALTzeroSensePin`/`AllstepperEnablePin` above |
| A3, A4, A5, A6 (57-60) | XOT 4-wire stepper coils (`XOT_COIL_A..D`) — no collisions |
| A7, A8, A9, A10 (61-64) | XOP 4-wire stepper coils (`XOP_COIL_A..D`) — no collisions |
| A11, A12, A13, A14 (65-68) | EGT 4-wire stepper coils (`EGT_COIL_A..D`) — no collisions |
| 24, 25, 26, 27 | TS 4-wire stepper coils (`TS_COIL_A..D`) — 24/26 collide with `AOAstepPin`/`GForcestepPin` above |
| 28, 29, 30, 31 | RS 4-wire stepper coils (`RS_COIL_A..D`) — 28 collides with `GForcedirectionPin` above |
| 2, 3, 4, 6 | FA 4-wire stepper coils (`FA_COIL_A..D`) — 2/3/4 collide with `COIL_VSI_A..C` above |
| 36, 37, 38, 39 | ET 4-wire stepper coils (`ET_COIL_A..D`) — 36/38 collide with `SpeedCurrentdirectionPin`/`SpeedMaxstepPin` above |
| 40, 41, 42, 43 | GP 4-wire stepper coils (`GP_COIL_A..D`) — 40/42 collide with `SpeedMaxdirectionPin`/`ALTstepPin` above |
| 44, 45, 46, 47 | EOP 4-wire stepper coils (`EOP_COIL_A..D`) — 44/46 collide with `ALTdirectionPin`/`FlapsStepPin` above |

## Local network configuration (JET_RANGER_STEPPER_CONTROLLER.ino)

| Setting | Value |
|---|---|
| Static IP | `172.16.1.105` |
| MAC | `A8:61:0A:67:83:69` (string label `sMac` says `A8:61:0A:67:83:03`, which doesn't match the actual `mac[]` byte array — same kind of label/byte mismatch seen in other Jet Ranger sketches) |
| Local port `localport`/`keyboardport` | 7788 (bound; also reused as `keyboardport`, though nothing in this sketch sends keyboard commands) |
| Local port `localdebugport` | 7795 (declared, not bound in this sketch) |
| Local port `MSFSport` | **13136** (listens for `D,IAS:...,ALT:...` front-panel data packets). Changed from an unused `7791` to match `JET_RANGER_SERVO_CONTROLLER`'s port exactly, so the same PC bridge apps could feed both boards. |

## Remote endpoints this sketch talks to

| Target | Port | Purpose |
|---|---|---|
| `172.16.1.10` (reflector host) | 27000 | Debug/log messages — extensive step-by-step homing/self-test progress logging via `SendDebug()` |
| `172.16.1.110` (`targetIP`, "Arduino Due for Keystroke translation and Pixel Led driving") | 7788 / 7789 | `SendIPString()`/`SendLedString()` helper functions exist for this, but neither is called anywhere in this sketch — dead code carried over from the input-controller sketches |
| `172.16.1.10` (`MSFSIP`) | 7791 | `SendMSFSMessage()` helper exists but is never called in this sketch — also dead code (unrelated to the new `MSFSport` UDP *receiver*, which is inbound-only and doesn't use `MSFSIP`) |

> **[FSUIPCWinformsAutoCS](../../%20C%23%20Code/FSUIPCWinformsAutoCS/PROGRAM_SUMMARY.md)**
> sends to `172.16.1.105:13136` (a `stepperClient`, with its own minimal
> payload built fresh each tick — no longer derived from the shared
> front-panel payload) — `ALT`, `VSI` (raw fpm), and `AGL` (radar
> altitude). This board reads `ALT`/`VSI` out of it and ignores `AGL` (no
> gauge for it yet — see above) and everything else. `IAS` was dropped
> from this payload earlier and isn't coming back — see the dead-code note
> above. The other sim-bridge apps (`P3D_to_UDP` / `SimConnect_to_UDP` /
> `MSFSSimConnectExtractor`) have **not** been updated to send anything to
> this board, so it only receives UDP data when `FSUIPCWinformsAutoCS`
> specifically is the bridge app running.
>
> **Known hardware limitation — ALT needle reversal (diagnosed, root
> cause found):** on real hardware, `ALTstepper`'s needle was observed
> moving clockwise-only when driven by live, frequent, small-increment
> altitude updates from `FSUIPCWinformsAutoCS`, while moving correctly in
> both directions when driven by large, deliberate jumps from
> `StepperVSITester`/manual testing. VSI was suspected and ruled out (the
> reversal persisted with VSI entirely removed from the stepper payload).
> Root cause: **the stepper driver electronics don't reliably move the
> needle backwards for very small step deltas** — live FSUIPC altitude
> readings are noisy (±1-2 ft of jitter even at a steady altitude), and at
> `onAltMslFtChange()`'s `5.76` steps/foot that's only a handful of steps
> per update, apparently below whatever threshold this driver needs to
> reverse direction reliably. This is a hardware/driver characteristic,
> not a bug in this sketch's or the C# bridge's logic — worth checking for
> the same symptom on any other stepper here that receives frequent,
> fine-grained updates (VSI's `VSI_FPM_TABLE` conversion can also produce
> small deltas for small fpm changes, though no reversal has been reported
> there yet).
>
> **[StepperVSITester](../../%20C%23%20Code/StepperVSITester/PROGRAM_SUMMARY.md)**
> can also send `"D,VSI:<fpm>"` and `"D,ALT:<feet>"` straight to this
> board's `172.16.1.105:13136` for testing/tuning VSI or ALT in isolation,
> without needing FSUIPC or a flight sim running — meant to be run
> *instead of* `FSUIPCWinformsAutoCS` while doing that, not alongside it.

> All three of the `SendIPMessage`/`SendMSFSMessage`/`SendIPString`/
> `SendLedString` helper functions are copy-pasted from the button-matrix
> input-controller sketches (see below) but have no callers here, since
> this sketch has no button matrix of its own — it only drives steppers
> and a servo from DCS-BIOS.

---

## A10_LEFT_CONSOLE_INPUT_CONTROLLER_A.ino — Program Summary

> Its own header comment says **"A10 RIGHT CONSOLE INPUT CONTROLLER"**
> while the containing folder is named for the *left* console — a further
> naming inconsistency worth resolving with whoever maintains this board.

Arduino Mega 2560 176-button row/column matrix scanner (16 rows × 11
columns, identical wiring scheme to `JET_RANGER_UPPER_CONTROLLER`'s
matrix) for a TACAN/ILS/electrical-panel/CDU/AAP/lighting-control-panel
input console, forwarding every press/release straight to DCS-BIOS.

### Program flow

1. **Setup**: flashes status LEDs, configures rows 22–37 as outputs and
   columns 38–49 as inputs, clears the button-state arrays, starts
   DCS-BIOS, then (if Ethernet enabled) resets the W5500 shield, brings up
   Ethernet, and sends an "INIT RIGHT INPUT" debug message to the
   reflector.
2. **Main loop** (`loop()`): pumps `DcsBios::loop()`, scans the 16×11
   matrix using direct `PORTA`/`PORTC` writes and `PIND`/`PING`/`PINL`
   reads (same technique as `JET_RANGER_UPPER_CONTROLLER`), then calls
   `FindInputChanges()`.
3. **`FindInputChanges()`**: for each debounced button-state change, calls
   `CreateDcsBiosMessage(index, state)` — a large switch statement mapping
   each of the 176 matrix positions to a named DCS-BIOS command (TACAN
   mode/test, ILS power, electrical panel switches (`EPP_*`), fuel
   quantity indicator test/select (`FQIS_*`), a full CDU keypad (`CDU_0`–
   `CDU_9`, `CDU_A`–`CDU_Z`, and function keys), an AAP page/steerpoint
   selector, and lighting-control-panel switches (`LCP_*`)) — and (if
   `Ethernet_In_Use`) also calls `SendIPMessage()` to log the raw
   index/state pair to the reflector, and (if `MSFS_In_Use`, currently
   `0`/disabled) `SendMSFSMessage()`.
4. Also registers rotary encoders/potentiometers directly with DCS-BIOS
   (not through the matrix): `tacan10`/`tacan1` (TACAN channel tens/ones),
   `tacanVol`, `ilsKhz`/`ilsMhz`, `ilsVol`, and 5 lighting-control-panel
   potentiometers (`lcpFlood`, `lcpConsole`, `lcpEngInst`, `lcpAuxInst`,
   `lcpFormation`).
5. A `onConsolesDimmerChange` callback (address `0x7544`) calls
   `SendLedString()` with a computed brightness — this is the one place in
   either sketch where that helper is actually used.

### Pin usage

| Pin(s) | Function |
|---|---|
| 5 | Green status LED |
| 6 | Red status LED |
| 53 | W5500 Ethernet shield manual reset |
| 22–37 | 16 button-matrix row drivers |
| 38–48 | 11 usable button-matrix column inputs (of 16 wired; pins 50–53 reserved for Ethernet SPI, matching `JET_RANGER_UPPER_CONTROLLER`) |
| 14, 15 | TACAN "10" rotary encoder (A, B) |
| 17, 16 | TACAN "1" rotary encoder (A, B) |
| 18, 19 | ILS kHz rotary encoder (A, B) |
| 21, 20 | ILS MHz rotary encoder (A, B) |
| A0–A4 (analog pins 0–4) | Lighting-control-panel potentiometers: flood, console, eng-inst, aux-inst, formation |
| A5 | TACAN volume potentiometer |
| A6 | ILS volume potentiometer |

### Local network configuration

| Setting | Value |
|---|---|
| Static IP | `172.16.1.103` |
| MAC | `A8:61:0A:9E:83:03` |
| Local port `localport`/`keyboardport` | 7788 |
| Local port `localdebugport` | 7795 |

> **IP conflict:** this sketch hard-codes `172.16.1.103`, the same address
> `JET_RANGER_UPPER_CONTROLLER` now uses (after that board was renumbered
> from `.101` to `.103` in an earlier change to resolve a different
> conflict). If both this A-10C input controller and the Jet Ranger Upper
> Controller are ever deployed on the same network, one of them needs a
> different IP — this wasn't fixed as part of this documentation pass
> since it wasn't asked for, just flagging it here.

### Remote endpoints this sketch talks to

| Target | Port | Purpose |
|---|---|---|
| `172.16.1.10` (reflector host) | 27000 | Debug/log messages and raw button index/state (`SendIPMessage`) |
| `172.16.1.110` (`targetIP`) | 7788 (keyboard) / 7789 (LED) | `SendIPString()` (declared, unused) / `SendLedString()` (used once, by the consoles-dimmer callback) |
| `172.16.1.10` (`MSFSIP`) | 7791 | `SendMSFSMessage()` — only actually called if `MSFS_In_Use` is set to `1` (currently `0`, disabled) |

### C# / other programs this sketch (and its sibling stepper sketch) communicate with

- **DCS-BIOS**, running on the DCS World PC over the serial/USB link, is
  the real data source/sink for both sketches in this folder — not any of
  the Jet Ranger SimConnect/FSUIPC bridge apps.
- No C# project in this repository listens on `172.16.1.10:27000` (debug
  logs) or `172.16.1.10:7791` (`MSFSport`) — both are dead ends from this
  repo's perspective.
- The `172.16.1.110` target ("Arduino Due for Keystroke translation and
  Pixel Led driving") is not among the Arduino sketches documented in this
  repository — it's referenced by IP only, with no corresponding project
  found here.
