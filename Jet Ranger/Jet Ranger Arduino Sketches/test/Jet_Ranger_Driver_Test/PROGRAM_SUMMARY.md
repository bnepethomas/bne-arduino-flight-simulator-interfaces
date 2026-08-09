# Jet_Ranger_Driver_Test — Program Summary

> **Note on this file:** the `PROGRAM_SUMMARY.md` previously sitting in
> this folder actually documented a *different* sketch —
> `JET_RANGER_STEPPER_CONTROLLER.ino` (and its unrelated neighbour
> `A10_LEFT_CONSOLE_INPUT_CONTROLLER_A.ino`), which live in their own
> sibling folder with their own copy of that same summary
> (`../../JET_RANGER_STEPPER_CONTROLLER/PROGRAM_SUMMARY.md`). Neither of
> those two sketches is present here — this folder only contains
> `Jet_Ranger_Driver_Test.ino`. That mismatched content has been replaced
> with this file, which documents what's actually in this folder. Worth
> checking with whoever filed the old copy here whether it was meant as a
> deliberate starting reference (this sketch is in fact a fork/bench-test
> variant of `JET_RANGER_STEPPER_CONTROLLER.ino`) or just misplaced.

`Jet_Ranger_Driver_Test.ino` is a bench-test fork of
[`JET_RANGER_STEPPER_CONTROLLER`](../../JET_RANGER_STEPPER_CONTROLLER/PROGRAM_SUMMARY.md)
(header identity `A10_FRONT_CONSOLE_STEPPERS`, front-center of an A-10C
Warthog DCS World pit) — same board, same static IP, used to try out
stepper wiring changes and new UDP test commands before they're promoted
back to the production sketch. It has diverged from that sketch in
several ways documented below (an in-progress VSI/SpeedMax/AOA pin
reassignment, a new Radar Altimeter gauge, and two new raw-testing UDP
commands).

## Build verification

Compiled with `arduino-cli` (`arduino:avr:mega`) and flashed to the bench
Mega on **COM4**:

| Change | Flash | RAM |
|---|---|---|
| ALT direct-step jog (`ASTEP`) added | 24,820 bytes (9%) | 2,933 bytes (35%) |
| Radar ALT stepper wired up (startup swing + raw `RALT`) | 25,798 bytes (10%) | 3,107 bytes (37%) |
| Radar ALT `RALT` switched to real ft→step calibration | 26,178 bytes (10%) | 3,137 bytes (38%) |

## ⚠ Known issue: VSI/SpeedMax/AOA pin conflict

`VSIstepper` is currently constructed as `FULL4WIRE` on
`SpeedMaxstepPin`/`SpeedMaxdirectionPin`/`AOAstepPin`/`AOAdirectionPin`
(line ~209) — the exact same four physical pins that `SpeedMaxstepper`
(`DRIVER`, `SpeedMaxstepPin`/`SpeedMaxdirectionPin`) and `AOAstepper`
(`DRIVER`, `AOAstepPin`/`AOAdirectionPin`) are **separately** constructed
on. All three stepper objects are serviced every loop iteration via
`updateSteppers()`, and `SpeedMaxstepper` also runs an active startup
swing in `setup()`. If `SpeedMaxstepper`/`AOAstepper` are physically wired
right now, `VSIstepper`'s coil pulses will collide with them (and vice
versa). Not fixed here since it wasn't asked for and the correct
resolution depends on current bench wiring intent — flagging for
whoever's driving the VSI rewiring to confirm/resolve.

## Program flow

1. **Setup**
   - Flashes status LEDs, brings up Ethernet (static IP, MAC — see
     network table below) if `Ethernet_In_Use`, ramps `BACK_LIGHTS` PWM.
   - **VSI startup**: winds hard to `-FULL4WIRE_HOMING_STEPS` (blind, no
     sensor), zeros there, swings out and back once, then settles at
     `(FULL4WIRE_HOMING_STEPS / 2) - VSIoffset` and re-zeros — same
     direct-drive coil homing style used throughout this sketch for
     `FULL4WIRE` steppers.
   - **Radar ALT startup** (new): same blind wind-to-end-stop pattern as
     VSI — no zero-sense pin exists for this gauge yet. Winds to
     `-FULL4WIRE_HOMING_STEPS`, zeros there, then swings out to
     `+FULL4WIRE_HOMING_STEPS` and back once as a visual self-test at
     boot. Direction sign and step count are **not bench-verified** for
     this specific gauge — confirm it actually reaches its end stop
     before trusting it unattended.
   - **ALT startup**: drives toward `-STEPS*2` while watching
     `ALTzeroSensePin`; on finding zero, sets current position to `-200`
     (not `0` — differs from the production sketch). Then does a 3-lap
     round trip (`5760*3` steps) as a self-test before returning to `0`.
   - **Speed Max startup**: active (unlike the production sketch's
     commented-out equivalent) — winds to `-DUAL_STEPS*1.1`, zeros, does
     one swing lap, then parks at `DUAL_STEPS*0.95`.
   - Speed Current, Flaps, AOA, and G-Force startup sequences are all
     currently commented out (same as upstream).
   - Starts DCS-BIOS, sets backlighting to running brightness.
2. **Main loop** (`loop()`)
   - Toggles status LEDs every `FLASH_TIME` (300ms).
   - `DcsBios::loop()` is commented out — DCS-BIOS callbacks are
     registered but not pumped, so none of them currently fire from a
     live DCS-BIOS link in this build.
   - `updateSteppers()` calls `.run()` on `VSIstepper`, `RadarALTstepper`,
     `ALTstepper`, `SpeedCurrentstepper`, `SpeedMaxstepper`,
     `FlapsStepper`, `AOAstepper`, `GForcestepper` every iteration.
     `SARIstepperRoll` is **not** included here (matches its commented-out
     `Nema8Stepper` instantiation below — SARI is fully inactive).
   - Every `incomingcheckinterval` (5ms), checks `MSFSudp` for an incoming
     packet on `MSFSport` (13136) and passes it to
     `ProcessReceivedMSFSString()`.
3. **DCS-BIOS callbacks** (registered, but not pumped per above): airspeed
   needle/max-IAS, VVI→VSI, AOA, ALT (`onAltMslFtChange`), G-force, two
   backlight-brightness callbacks. Flaps' DCS-BIOS binding is commented
   out. A `Nema8Stepper`-based SARI roll state machine plus a
   `DcsBios::ServoOutput` SAI pitch servo (pin 9) are defined; the SARI
   stepper instantiation itself is commented out, so only the pitch servo
   binding (`saiPitch`) is actually live.
4. **UDP test/data receiver** (`ProcessReceivedMSFSString` →
   `HandleOutputValuePair`, same `"D,CODE:value,CODE:value,..."` CSV
   format used across the Jet Ranger fleet):

   | Code | Value units | Behaviour |
   |---|---|---|
   | `IAS` | Bell 206 servo-position passthrough | `setCurrentAirspeed(value)` — straight passthrough, no A-10 calibration yet |
   | `ALT` | feet | `onAltMslFtChange(value)` — real `feet * 5.76` conversion |
   | `VSI` | fpm | `VSIstepper.moveTo(vsiFpmToSteps(value))` via the 11-row `VSI_FPM_TABLE` |
   | `ASTEP` *(new)* | `"<steps>/<intervalMs>"` | `jogAltimeterSteps()` — bench-only raw jog for the **Altimeter**. Bit-bangs `ALTstepPin`/`ALTdirectionPin` directly with a literal `delay(intervalMs)` between pulses, bypassing `AccelStepper`'s acceleration ramp entirely, for finding exact step timing. `steps` is a signed relative delta from wherever `ALTstepper` currently sits; `ALTstepper`'s tracked position is updated via `setCurrentPosition()` afterward so subsequent `ALT` commands stay accurate. DIR pin is left latched `HIGH` once the jog completes, whichever direction it moved. Blocks for `steps * intervalMs` ms. |
   | `RALT` *(new)* | feet | `setRadarAlt(radarAltFtToSteps(value))` for **RadarALTstepper**. `radarAltFtToSteps()` linearly interpolates within `RADAR_ALT_FT_TABLE`, a hand-supplied 3-row calibration with two distinct linear segments: 0→0 steps, 500 ft→309 steps, 2500 ft (max)→463 steps. Values outside 0–2500 ft clamp to whichever end is nearest. |

   Every other code is parsed and silently ignored.

## Pin usage

| Pin(s) | Function |
|---|---|
| 12 | Red status LED |
| 13 | Green status LED |
| 53 | W5500 Ethernet shield manual reset |
| 8 | Backlighting PWM output (`BACK_LIGHTS`) |
| 9 | SARI pitch servo (`DcsBios::ServoOutput saiPitch`) |
| 22, 24 | `AOAstepPin`/`AOAdirectionPin` — AOA stepper **and** shared with VSI's `FULL4WIRE` construct (see pin-conflict note above) |
| 26, 28 | G-Force stepper step/direction |
| 30, 32 | SARI roll stepper step/direction (defined, never run — see above) |
| 34, 36 | Current-airspeed stepper step/direction |
| 38, 40 | `SpeedMaxstepPin`/`SpeedMaxdirectionPin` — Max-airspeed stepper **and** shared with VSI's `FULL4WIRE` construct (see pin-conflict note above) |
| 42, 44 | Altimeter stepper step/direction (also bit-banged directly by the new `ASTEP` jog) |
| 46, 48 | Flaps stepper step/direction |
| 54 | Altimeter zero-sense homing switch input (`ALTzeroSensePin`) |
| 55 | SARI roll IR zero-detector input (unused — SARI inactive) |
| 56 | Shared stepper-driver enable pin (`AllstepperEnablePin`/`SARIenablePin`) |
| 2, 3, 4, 5 | `COIL_VSI_A..D` — now driving the new `RadarALTstepper` (was VSI's coil pins before VSI moved onto pins 38/40/22/24 above) |
| Serial (USB) | DCS-BIOS `DCSBIOS_IRQ_SERIAL` link (registered but not pumped — see main loop above) |

## Local network configuration

| Setting | Value |
|---|---|
| Static IP | `172.16.1.105` |
| MAC | `A8:61:0A:67:83:69` (label `sMac` string says `...83:03`, a pre-existing mismatch also present in the production sketch) |
| Local port `localport`/`keyboardport` | 7788 |
| Local port `localdebugport` | 7795 (declared, not bound) |
| Local port `MSFSport` | 13136 — test/data UDP receiver, see table above |

## Remote endpoints this sketch talks to

| Target | Port | Purpose |
|---|---|---|
| `172.16.1.10` (reflector host) | 27000 | Debug/log messages via `SendDebug()` — homing progress, jog confirmations, `ASTEP`/`RALT` target logging |
| `172.16.1.110` (`targetIP`) | 7788 / 7789 | `SendIPString()`/`SendLedString()` exist but have no callers in this sketch |
| `172.16.1.10` (`MSFSIP`) | 7791 | `SendMSFSMessage()` exists but has no callers |

## Programs this communicates with

- **[StepperVSITester](../../../%20C%23%20Code/StepperVSITester/PROGRAM_SUMMARY.md)**
  (`172.16.1.105:13136`) — sends `VSI` (fpm), `ALT` (feet), `RALT` (feet,
  **new**), and `ASTEP` (`<steps>/<intervalMs>`, **new**) test packets.
  Meant to run *instead of* `FSUIPCWinformsAutoCS` while bench-testing,
  not alongside it.
