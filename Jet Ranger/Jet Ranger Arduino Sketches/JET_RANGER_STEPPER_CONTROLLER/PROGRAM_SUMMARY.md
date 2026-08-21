# JET_RANGER_STEPPER_CONTROLLER — Program Summary

> **Board identity resolved:** this sketch's header and `BoardName` used
> to identify it as `A10_FRONT_CONSOLE_STEPPERS`/"A10 Forward Steppers"
> (an A-10C Warthog board), even though it's part of the Jet Ranger
> fleet. That's since been fixed by hand: the header now says
> `JET_RANGER_STEPPERS`/"JET RANGER STEPPER CONTROLLER" and `BoardName`
> is `"Jet Ranger Steppers"`. Its data source is still a mix of DCS-BIOS
> (serial, for the A-10C SimVars still wired up) and the Jet Ranger's
> `172.16.1.x` UDP/CSV protocol.

Two Arduino sketches live under this folder:

1. **`JET_RANGER_STEPPER_CONTROLLER.ino`** (this summary's main subject)
   — drives 13 analogue stepper-motor gauges plus backlighting. Started
   as an A-10C front instrument panel board with 7 gauges; has since had
   5 of the original 7 (`ALT`, `Flaps`, `AOA`, `G-Force`, `Max Airspeed`)
   removed/commented out by hand, `Current Airspeed` renamed to
   `IASstepper` and rewired to match `Stepper-Tuning-Harness`, and 11 new
   Bell 206-style gauges added (see below).
2. **`A10_LEFT_CONSOLE_INPUT_CONTROLLER_A/A10_LEFT_CONSOLE_INPUT_CONTROLLER_A.ino`**
   — a separate 176-button matrix input controller for TACAN/ILS/electrical/
   CDU/AAP/lighting panels (see its own section below). Untouched by any
   of the changes described here.

## Build verification

Both sketches in this folder were compiled with `arduino-cli` (target
`arduino:avr:mega:cpu=atmega2560`) and build with **0 errors**:

| Sketch | Flash | RAM |
|---|---|---|
| `JET_RANGER_STEPPER_CONTROLLER.ino` | 27,298 bytes (10%) | 3,735 bytes (45%) |
| `A10_LEFT_CONSOLE_INPUT_CONTROLLER_A.ino` | 23,586 bytes (9%) | 4,962 bytes (60%) |

Flashed to the bench Mega on COM4 several times across this sketch's
history; most recently after the real `IAS_KT_TABLE` calibration was
added. `SendRetransmissionTimeout` note: `Ethernet.setRetransmissionTimeout(10)`
was added right after `Ethernet.begin()` - the W5500's own ARP/send retry
timeout (default 200ms × 8 retries = up to 1600ms blocked inside a UDP
send that fails to get an ARP reply) is now capped at 10ms/retry
(~80ms worst case), separate from the one-time boot-time
`delayBeforeSendingPacket` wait (unchanged, still 2000ms).

Library versions used for this verification: **Ethernet** 2.0.2,
**AccelStepper** 1.64.0, **DCS-BIOS** 0.3.13 (Arduino Library Manager). The
`dcs-bios-arduino-library-0.3.7.zip` bundled inside the
`A10_LEFT_CONSOLE_INPUT_CONTROLLER_A` folder is an older release of the
same library — the Library Manager's 0.3.13 was used for this check and
compiled without needing any code changes, but the two haven't been
diffed against each other line-for-line.

## Current stepper roster

| Stepper | Interface/pins | Status |
|---|---|---|
| `VSIstepper` | FULL4WIRE, `COIL_VSI_A..D` (7/8/9/11, wired C,D,A,B) | Active - real `VSI_FPM_TABLE` calibration, boot homing gated by `SwingVSI` (default `false`, so currently disabled - see selective-swing note below) |
| `IASstepper` (renamed from `SpeedCurrentstepper`) | FULL4WIRE, `STEPPER_SPD_A..D` (12/13/22/23, wired C,D,A,B) | Active - real `IAS_KT_TABLE` calibration (see below); its own boot startup/swing exists but is gated by `SwingIAS` (default `false`) - previously a bare `if (false)`, now a named toggle with identical default behaviour |
| `RadarAltStepper` | FULL4WIRE, `RADAR_ALT_COIL_A..D` (32/33/34/35, wired C,D,A,B) | Active, raw steps only (`AGL` code) - no real calibration yet. Boot startup/swing gated by `SwingAGL` (default `false`) - previously a bare `if (false)` whose own in-code comment incorrectly claimed it ran every boot; that stale claim was corrected when the named gate was added |
| `EOTstepper`, `XOTstepper`, `XOPstepper`, `EGTstepper`, `FAstepper`, `GPstepper`, `EOPstepper` | FULL4WIRE, pins matching `Stepper-Tuning-Harness` exactly | Active, real-unit UDP codes (see table below), all still using the placeholder `FULL4WIRE_HOMING_STEPS` linear scale (see caution below), no boot startup/swing at all |
| `TSstepper`, `RSstepper` | FULL4WIRE, pins matching `Stepper-Tuning-Harness` exactly | Active, real-unit UDP codes via `TS_PCT_TABLE`/`RS_PCT_TABLE` (see table below). Boot startup/swing gated by shared `SwingRPM` (default `true`) - previously ran unconditionally with no gate at all; the default preserves that prior always-on behaviour |
| `ETstepper` | FULL4WIRE, `ET_COIL_A..D` (36/37/38/39) | Active, raw steps only (`TQ` code) |
| `ALTstepper`, `SpeedMaxstepper`, `FlapsStepper`, `AOAstepper`, `GForcestepper` | — | **Removed.** Constructs, pin `#define`s (mostly), startup routines, DCS-BIOS bindings, and UDP codes for all five are commented out or deleted. `FlapsStepPin`/`FlapsDirectionPin` are the one pair of pin `#define`s left behind, now orphaned (nothing reads them). |
| `SARIstepperRoll` | DRIVER, pins 30/32 | Declared and pin-claimed, but its `Nema8Stepper` binding is commented out - never `.run()`, never bound to DCS-BIOS. Still occupies pins 30/32 via its `AccelStepper` constructor. |
| `saiPitch` (`DcsBios::ServoOutput`, pin 9) | — | Active - SAI pitch axis, plain hobby servo |

> **Caution — `FULL4WIRE_HOMING_STEPS` redefined:** was `315 * 2` (630),
> now `315 + 5` (320) — halves the effective step range every
> still-uncalibrated gauge above (`EOT`/`XOT`/`XOP`/`EGT`/`TS`/`RS`/`GP`/`FA`)
> scales its real-unit range onto, since none of them have their own
> bench-measured ceiling yet. Their in-code comments hadn't been updated
> to match until this pass.
>
> **Caution — VSI homing macro-precedence bug:** two new constants,
> `X27_FULLWIRE_STEPS` (635) and `X27_FULLWIRE_HOMING_STEPS`
> (`X27_FULLWIRE_STEPS + 5`, unparenthesized), now drive VSI's homing.
> `VSIstepper.runToNewPosition(-X27_FULLWIRE_HOMING_STEPS)` expands to
> `-635 + 5` = **-630**, not `-(635+5)` = -640 as the name implies - a
> small (~1.5%) but real discrepancy between the code's apparent and
> actual behavior.
>
> **Caution — `AllstepperEnablePin` pinMode vs digitalWrite:**
> `pinMode(AllstepperEnablePin, OUTPUT)` is commented out in `setup()`,
> but `digitalWrite(AllstepperEnablePin, false)` right after it is still
> active. An Arduino pin defaults to `INPUT` at boot, so that
> `digitalWrite()` likely just toggles the pin's internal pull-up rather
> than actually driving it LOW as intended.

## Newly surfaced pin collisions (SARI)

Reviewing the current pin map turned up two collisions with the
still-pin-claiming (if otherwise inactive) `SARIstepperRoll` that weren't
caught in earlier passes:

| Gauge pin | Collides with |
|---|---|
| `RS_COIL_C` (30) | `SARIstepPin` |
| `RADAR_ALT_COIL_A` (32) | `SARIdirectionPin` |

Removing `ALTstepper`/`SpeedMaxstepper`/`FlapsStepper`/`AOAstepper`/
`GForcestepper` resolved every collision documented in earlier passes of
this summary (their pin `#define`s are now mostly gone entirely, not
just unused) — the one exception is `EOT_COIL_D` (A2/56), which still
collides with `AllstepperEnablePin`.

## JET_RANGER_STEPPER_CONTROLLER.ino — Program flow

1. **Setup**: flashes status LEDs, brings up Ethernet (static IP,
   10ms retransmission timeout - see build verification above), ramps
   `BACK_LIGHTS`. Each stepper's boot wind/zero/swing self-test is gated
   by its own named boolean (`SwingVSI`/`SwingIAS`/`SwingAGL`/`SwingRPM`,
   with a shared `SwingLoops` = 3 loop count) — same selective-swing
   pattern `JET_RANGER_OLED_DUAL_STEPPER_CONTROLLER.ino` already used,
   ported here to replace this sketch's previous mix of ad-hoc
   `if (false)` gates (VSI/IAS/Radar Alt) and no gate at all (Turbine/
   Rotor Speed, which ran unconditionally). Defaults preserve prior
   behaviour exactly: `SwingVSI`/`SwingIAS`/`SwingAGL` = `false`,
   `SwingRPM` = `true` (gates both `TSstepper` and `RSstepper`). `ALT`/
   `Flaps`/`AOA`/`G-Force` startup blocks are all commented out (not
   gated - those steppers don't exist in this sketch any more). Starts
   DCS-BIOS, sets running-brightness backlighting.
2. **Main loop** (`loop()`): toggles status LEDs; `DcsBios::loop()` is
   commented out (DCS-BIOS callbacks are registered but never pumped, so
   none fire from a live serial link in this build); `updateSteppers()`
   calls `.run()` on `VSIstepper`, `IASstepper`, `RadarAltStepper`, and
   the 9 newer FULL4WIRE gauges (`ALTstepper` etc. excluded, since they
   no longer exist); polls `MSFSudp` every `incomingcheckinterval` (5ms).
3. **DCS-BIOS callbacks** (registered, not pumped): `onAirspeedNeedleChange`
   (`A_10C_AIRSPEED_NEEDLE`, still uses the old `0-65535 → 0..DUAL_STEPS+80`
   linear map, not `IAS_KT_TABLE` - that table is UDP-path only) and
   `onVviChange` (`A_10C_VVI`, VSI). The `ALT`/`Flaps`/`AOA`/`G-Force`
   DCS-BIOS bindings are commented out along with their steppers. Two
   lighting callbacks drive `BACK_LIGHTS` PWM.
4. **SARI** — see the roster table above; fully inactive.
5. **UDP data receiver** (`ProcessReceivedMSFSString`/`HandleOutputValuePair`,
   same `"D,CODE:value,CODE:value,..."` format as `JET_RANGER_SERVO_CONTROLLER`):

   | Code | Real value | Board behaviour |
   |---|---|---|
   | `IAS` | knots, 0-140 | `setIAS()` → `iasKtToSteps()` via **`IAS_KT_TABLE`**, a 9-row hand-measured calibration table (linear interpolation, same pattern as `VSI_FPM_TABLE`). The `0kt→0 step` row is an assumed zero reference, not directly measured - worth confirming on the bench. |
   | `IASRAW` | raw steps | Bypasses the table, direct `.moveTo()` |
   | `VSI` | fpm, ±1750 | `vsiFpmToSteps()` via `VSI_FPM_TABLE` (unchanged) |
   | `VSIRAW` | raw steps | Bypasses the table |
   | `AGL` | raw steps | No real calibration yet (renamed from `RALT` to match `JET_RANGER_SERVO_CONTROLLER`/FSUIPCWinformsAutoCS's actual code) |
   | `OILT`/`XMSNT`/`ITT` | °C | `setEOT`/`setXOT`/`setEGT` via the shared placeholder linear scale (see the `FULL4WIRE_HOMING_STEPS` caution above) |
   | `OILP`/`XMSNP` | PSI | `setEOP`/`setXOP`, same placeholder scale |
   | `RPME`/`RPMR` | %, 0-120/120, one decimal place (e.g. `82.4`) | `setTS`/`setRS`, same placeholder scale; parsed with `toFloat()` (not `toInt()`) and `tsPctToSteps()`/`rsPctToSteps()`/`setTS()`/`setRS()` all take `float` now, so the fractional percent reaches the interpolation instead of being truncated first |
   | `N1` | %, 0-105 | `setGP`, same placeholder scale |
   | `FUEL` | US gal, 0-75 | `setFA`, same placeholder scale |
   | `OILTRAW`/`OILPRAW`/`XMSNTRAW`/`XMSNPRAW`/`ITTRAW`/`RPMERAW`/`RPMRRAW`/`N1RAW`/`FUELRAW` | raw steps | Each bypasses its real-value sibling's conversion |
   | `TQ` | raw steps | Renamed from `ET`, no real calibration requested |
   | `FLAPS`, `AOA`, `GFORCE`, `SPDMAX` | — | **Removed.** These were added earlier to give `FlapsStepper`/`AOAstepper`/`GForcestepper`/`SpeedMaxstepper` UDP reachability; now that those steppers are gone, their `HandleOutputValuePair` cases are gone too. `StepperVSITester`'s dropdown still lists these codes — sending them is currently a silent no-op on the board. |

   Codes matching `JET_RANGER_SERVO_CONTROLLER.ino`'s naming
   (`OILT`/`OILP`/`XMSNT`/`XMSNP`/`ITT`/`RPME`/`RPMR`/`N1`/`FUEL`/`TQ`/`AGL`)
   are deliberate — see each case's in-code comment for the exact
   quantity match, and `IAS`'s comment for the one case (Bell 206
   servo-position number vs. real knots) where the two boards' same code
   name means different units.
6. **No-data watchdog: every gauge on this board is driven back to its
   calibrated zero if no UDP packet arrives on `MSFSudp` for 30 seconds**
   (`noDataTimeoutMs`) — same feature, same threshold, and the same
   `lastMSFSDataMillis`/`gaugesResetForNoData` latch pattern as
   `JET_RANGER_OLED_DUAL_STEPPER_CONTROLLER.ino`'s own copy (the two
   sketches don't share this code, each has its own `ResetGaugesToZero()`
   matching its own roster). Scoped to `MSFSudp` traffic only — DCS-BIOS
   input doesn't reset the timer. This board's `ResetGaugesToZero()`
   calls `setIAS(0)`/`setVSI(0)`/`setEGT(0)`/`setEOT(0)`/`setEOP(0)`/
   `setXOT(0)`/`setXOP(0)`/`setTS(0)`/`setRS(0)`/`setGP(0)`/`setFA(0)`/
   `setAGL(0)` (each the same target a real `"<CODE>:0"` packet would
   produce), plus a direct `ETstepper.moveTo(0)` (Torque is raw-steps-only,
   no `setXxx()` wrapper exists for it). `ALTstepper` is still fully
   disabled on this sketch, so no altitude reset is included.

## Pin usage (JET_RANGER_STEPPER_CONTROLLER.ino)

| Pin(s) | Function |
|---|---|
| 14 | Green status LED (moved from 13, matching `Stepper-Tuning-Harness`) |
| 15 | Red status LED (moved from 12) |
| 53 | W5500 Ethernet shield manual reset |
| 8 | Backlighting PWM output (`BACK_LIGHTS`) |
| 9 | SARI pitch servo (`DcsBios::ServoOutput saiPitch`) |
| 30, 32 | SARI roll stepper step/direction — collides with `RS_COIL_C`/`RADAR_ALT_COIL_A` below |
| 56 | Shared stepper-driver enable pin (`AllstepperEnablePin`) — `pinMode` commented out, see caution above; collides with `EOT_COIL_D` below |
| 7, 8, 9, 11 | VSI 4-wire stepper coils (`COIL_VSI_A..D`) |
| 12, 13, 22, 23 | IAS (Current Airspeed) 4-wire stepper coils (`STEPPER_SPD_A..D`) |
| 32, 33, 34, 35 | Radar Alt 4-wire stepper coils (`RADAR_ALT_COIL_A..D`) — 32 collides with `SARIdirectionPin` |
| 48, A0, A1, A2 (54/55/56) | EOT 4-wire stepper coils (`EOT_COIL_A..D`) — A2/56 collides with `AllstepperEnablePin` |
| A3, A4, A5, A6 (57-60) | XOT 4-wire stepper coils |
| A7, A8, A9, A10 (61-64) | XOP 4-wire stepper coils |
| A11, A12, A13, A14 (65-68) | EGT 4-wire stepper coils |
| 24, 25, 26, 27 | TS 4-wire stepper coils |
| 28, 29, 30, 31 | RS 4-wire stepper coils — 30 collides with `SARIstepPin` |
| 2, 3, 4, 6 | FA 4-wire stepper coils |
| 36, 37, 38, 39 | ET 4-wire stepper coils |
| 40, 41, 42, 43 | GP 4-wire stepper coils |
| 44, 45, 46, 47 | EOP 4-wire stepper coils |
| 46, 48 | `FlapsStepPin`/`FlapsDirectionPin` — orphaned defines, no stepper reads them any more |
| Serial (USB) | DCS-BIOS `DCSBIOS_IRQ_SERIAL` link (registered, not pumped — see program flow above) |

## Local network configuration (JET_RANGER_STEPPER_CONTROLLER.ino)

| Setting | Value |
|---|---|
| Static IP | `172.16.1.105` |
| MAC | `A8:61:0A:67:83:69` (string label `sMac` still says `A8:61:0A:67:83:03`, a pre-existing mismatch) |
| Local port `localport`/`keyboardport` | 7788 |
| Local port `localdebugport` | 7795 (declared, not bound) |
| Local port `MSFSport` | 13136 |

## Remote endpoints this sketch talks to

| Target | Port | Purpose |
|---|---|---|
| `172.16.1.10` (reflector host) | 27000 | Debug/log messages via `SendDebug()` |
| `172.16.1.110` (`targetIP`) | 7788 / 7789 | `SendIPString()`/`SendLedString()` — declared, no callers |
| `172.16.1.10` (`MSFSIP`) | 7791 | `SendMSFSMessage()` — declared, no callers |
| `reflectorIP` (172.16.1.10, `JetRangerHealthMonitor`'s host) | 13137 | Health keepalive — bare `"STEPPER"` string sent every 10s (`aliveinterval`) via `aliveudp`, same pattern as `JET_RANGER_UPPER_CONTROLLER.ino`'s `"UPPER_INPUT"` keepalive |

> **[StepperVSITester](../../%20C%23%20Code/StepperVSITester/PROGRAM_SUMMARY.md)**
> can send every real-unit and `*RAW` code above straight to this board's
> `172.16.1.105:13136` for bench testing without needing FSUIPC or a
> flight sim running.
>
> **[JetRangerHealthMonitor](../../%20C%23%20Code/JetRangerHealthMonitor/PROGRAM_SUMMARY.md)**
> now shows this board's live/dead status (Stepper indicator) alongside
> Comm/Nav, Servo, Joystick, and Upper Input, based on the `"STEPPER"`
> keepalive above.

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
