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
   drives 7 analogue stepper-motor gauges plus backlighting for an A-10C
   front instrument panel.
2. **`A10_LEFT_CONSOLE_INPUT_CONTROLLER_A/A10_LEFT_CONSOLE_INPUT_CONTROLLER_A.ino`**
   — a separate 176-button matrix input controller for TACAN/ILS/electrical/
   CDU/AAP/lighting panels (see its own section below).

## Build verification

Both sketches in this folder were compiled with `arduino-cli` (target
`arduino:avr:mega:cpu=atmega2560`) and build with **0 errors**:

| Sketch | Flash | RAM |
|---|---|---|
| `JET_RANGER_STEPPER_CONTROLLER.ino` | 23,748 bytes (9%) | 2,805 bytes (34%) |
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
   ports of that sketch's versions). Three codes are wired up for now:
   - `IAS` → `setCurrentAirspeed(value)`, reusing the existing airspeed
     stepper mover. **Caveat:** the PC bridge apps
     (`P3D_to_UDP`/`SimConnect_to_UDP`/`FSUIPCWinformsAutoCS`) send `IAS`
     already converted to a *Bell 206 servo-position* number via their
     `IAS_Process()` tables — not raw knots and not an A-10 stepper step
     count — so this is currently a straight pass-through pending real
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
   - Every other code in the payload (e.g. `TQ`, `RPMR`, the warning-lamp
     bits, etc.) is currently parsed and silently ignored.

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
> now sends to `172.16.1.105:13136` (a new `stepperClient`, sending mostly
> the same shared front-panel payload it sends to the Servo and OLED
> Controllers, except its `VSI` field is swapped for raw fpm before
> sending here specifically) — this board reads the `IAS`/`ALT`/`VSI`
> fields out of it and ignores the rest. The other sim-bridge apps
> (`P3D_to_UDP` / `SimConnect_to_UDP` / `MSFSSimConnectExtractor`) have
> **not** been updated to do the same, so this board only receives UDP
> data when `FSUIPCWinformsAutoCS` specifically is the bridge app running.
>
> **[StepperVSITester](../../%20C%23%20Code/StepperVSITester/PROGRAM_SUMMARY.md)**
> can also send `"D,VSI:<fpm>"` straight to this board's `172.16.1.105:13136`
> for testing/tuning VSI in isolation, without needing FSUIPC or a flight
> sim running — meant to be run *instead of* `FSUIPCWinformsAutoCS` while
> doing that, not alongside it.

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
