# JET_RANGER_SERVO_CONTROLLER — Program Summary

Arduino Mega 2560 + W5500 Ethernet shield. Drives 15 warning lamps and 2
analogue-gauge servos (Attitude Indicator pitch/bank) from flight-sim data
received over Ethernet.

> **Out-of-band change (not made in this doc pass):** this sketch used to
> drive 15 servos - airspeed, VSI, engine torque, oil pressure/temp,
> transmission pressure/temp, EGT, rotor/engine RPM, N1, fuel, and
> fuel/electrical load, alongside pitch/bank. All 15 were removed from the
> `Servos` enum, the position arrays, `UpdateServoPos()`,
> `HandleOutputValuePair()`, `ResetGaugesToZero()`, `CheckServoIdleTime()`,
> and the setup-time self-test sweep - cleanly, with nothing left
> referencing a removed enum member. Only pitch/bank remain servo-driven on
> this board; those other gauges are now driven by
> [`JET_RANGER_STEPPER_CONTROLLER.ino`](../JET_RANGER_STEPPER_CONTROLLER/PROGRAM_SUMMARY.md)/
> [`JET_RANGER_OLED_DUAL_STEPPER_CONTROLLER.ino`](../JET_RANGER_OLED_DUAL_STEPPER_CONTROLLER/PROGRAM_SUMMARY.md)
> instead. Confirmed still compiling clean after the change (20,218 bytes,
> before this pass's Pitch calibration addition below).

## Program flow

1. **Setup**
   - Sets status LED pins, then calls `InitialiseWarningLights()` which sets
     all 15 warning-lamp pins to `OUTPUT` and turns them all on (lamp test).
   - Resets the W5500 shield, brings up Ethernet on the static IP, opens the
     UDP sockets below, and flashes the red LED for 3s while the link
     settles.
   - Runs a self-test/zero sequence: every servo is driven to its zero
     position, then swept from its minimum to maximum position and back
     (visible "wiggle test"). At the end both servos are parked fully at
     their calibrated zero: `aServoPosition[]` and `aTargetServoPosition[]`
     are set to `aServZeroPosition[]` (previously they were left at 0 and
     the placeholder `{444, 555}`, so `UpdateServoPos()` would have eased
     the servos towards those the moment `loop()` started), followed by a
     500ms settle delay. All 15 warning lamps are then switched off with
     `setWarningLightAll(false)` - the old hand-written list left the Rotor
     RPM Low lamp on after setup.
2. **Main loop** (`loop()`)
   - Toggles the status LEDs every 200ms (heartbeat).
   - Sends a `"SERVO"` keepalive to the reflector every 10s.
   - Every 5ms (`incomingcheckinterval`), checks for an incoming UDP packet
     on the MSFS port and, if present, passes it to
     `ProcessReceivedMSFSString()`, which parses the CSV payload
     (`D,TQ:120,IAS:80,...` / `C,B:5`) and updates:
     - 2 servo **target** positions (see `HandleOutputValuePair`) - `PITCH`
       and `BANK` (Attitude Indicator pitch/roll), the only two servos
       left on this board (see the out-of-band-change note above). Both
       are parsed as `float` (not `toInt()`), matching RPME/RPMR's own
       one-decimal wire format (e.g. `"82.4"`), and both now have real
       bench-measured calibration tables converting real attitude degrees
       into a servo position - reusing one shared `DegToServoPosEntry`
       struct + clamp + linear-interpolation pattern (same approach the
       stepper sketches use for their own calibration tables):
       - `PITCH_DEG_TABLE` (3 points: -30°→179, 0°→113, +30°→70) via
         `pitchDegToServoPos()`.
       - `BANK_DEG_TABLE` (3 points: -90°→5, 0°→93, +90°→179) via
         `bankDegToServoPos()` - endpoints match `aServMinPosition[]`/
         `aServMaxPosition[]` for this servo exactly. `aServZeroPosition[]`'s
         Bank entry was also updated (91 → 93) to match this table's
         bench-measured 0° point, so the setup-time self-test sweep and
         the no-data-watchdog's `ResetGaugesToZero()` both now return this
         servo to the same position `bankDegToServoPos(0)` does.
       Neither is a raw servo-position pass-through any more - the wire
       value for both codes is real attitude degrees now, not a
       pre-converted servo position the sender used to compute itself.
       `PITCHRAW`/`BANKRAW` were added alongside as the raw-position
       bypass siblings (bare `toInt()` straight into
       `aTargetServoPosition[]`, skipping the table) - same `"<CODE>RAW"`
       pattern the stepper sketches use for their own calibrated gauges.
     - 15 boolean warning-light states, each driving its lamp pin directly.
   - Every 5ms (`servoCheckInterval`), `UpdateServoPos()` nudges each servo
     one step at a time from its current position towards its target
     (smooth, non-blocking motion), attaching each servo on first use.
     `CheckServoIdleTime()` then detaches any servo that hasn't moved for
     1s (`ServoIdleTime`) to reduce jitter/power draw.
   - **No-data watchdog (new):** if no UDP packet is received for 30s
     (`noDataTimeoutMs`), `ResetGaugesToZero()` sets every servo's
     **target** position back to its calibrated zero
     (`aServZeroPosition[]`, the same array the setup-time self-test
     sweep and a real `"<CODE>:0"` packet's `map(0, ...)` both resolve
     to) and turns off all warning lights (`setWarningLightAll(false)`).
     It also resets the remembered per-lamp state strings to `"0"`:
     `HandleOutputValuePair()` only drives a lamp when the incoming value
     differs from them, so a stale `"1"` would have kept a lamp dark after
     a timeout when the sim next sent `"1"`.
     `UpdateServoPos()` then eases every servo there the normal way -
     this deliberately does **not** write the servos directly the way an
     earlier, incomplete draft of this feature (found commented out,
     using a 5s threshold) used to; that draft wrote the physical
     position immediately but never updated `aServoPosition[]`/
     `aTargetServoPosition[]`, so the next real UDP value would have made
     `UpdateServoPos()` ease from a stale remembered position instead of
     the just-zeroed one. `lastincomingpacketcheck` doubles as "last data
     received" here, armed at the **end of `setup()`**
     (`lastincomingpacketcheck = millis()`) so the self-test sweep can't use
     up the 30s (it's only advanced inside the packet-received
     branch, not on every poll tick) and the existing `servosZeroed` flag
     latches the reset so it only fires once per outage, clearing again
     the moment real data resumes.

## Build verification

Compiled with `arduino-cli` (target `arduino:avr:mega:cpu=atmega2560`),
**0 errors**: 21,568 bytes flash (8%), 2,135 bytes RAM (26%). (Dropped
from 27,646/2,925 after the out-of-band 15-servo removal - see the note
at the top of this file.)

## Pin usage

| Pin(s) | Function |
|---|---|
| 14 | Green status LED |
| 15 | Red status LED (flashes while Ethernet link comes up) |
| 53 | W5500 Ethernet shield manual reset (`ES1_RESET_PIN`) |
| 26 | Attitude Pitch servo (`PITCH_PORT` - its `#define` comment still says "Using Gas Producer Port for the moment", a leftover from before that servo was removed) |
| 27 | Attitude Bank/Roll servo (`ROLL_PORT` - same leftover-comment situation, "Using Radar Alt Port for the moment") |
| A1 | Engine Out warning lamp |
| A2 | Rotor RPM Low warning lamp |
| A3 | Transmission Oil Pressure warning lamp |
| A4 | Transmission Oil Temp warning lamp |
| A5 | Battery Hot warning lamp |
| A6 | Battery Temp warning lamp |
| A7 | Engine Chip warning lamp |
| A8 | Tail Rotor Chip warning lamp |
| A9 | Transmission Chip warning lamp |
| A10 | Baggage Door warning lamp |
| A11 | Aft Fuel Filter warning lamp |
| A12 | Fuel Pump warning lamp |
| A13 | Generator Fail warning lamp |
| A14 | Low Fuel warning lamp |
| A15 | Standby Compass (SC) Fail warning lamp |

## Local network configuration

| Setting | Value |
|---|---|
| Static IP | `172.16.1.102` |
| MAC | `A8:61:0A:9E:83:02` |
| Local port `localport` | 7788 (bound, source socket for outbound debug packets) |
| Local port `localdebugport` | 7795 (bound, currently unused for send/receive) |
| Local port `MSFSport` | 13136 (listens for sim data packets) |
| Local port `aliveport` | 13137 (bound, used to send keepalives) |

> **Fixed:** the `byte mac[]` array in this sketch previously read
> `{0xA8,0x61,0x0A,0x9E,0x83,0x01}` — byte-for-byte identical to
> `JET_RANGER_RADIO_CONTROLLER`'s MAC, even though the human-readable
> `sMac` string already said `...:02`. The byte array now matches the
> label, so all three network boards (Radio, Servo, Upper) have unique
> MACs.

## Remote endpoints this sketch talks to

| Target | Port | Purpose |
|---|---|---|
| `172.16.1.10` (reflector host) | 27000 | Debug/log messages (`SendDebug`) |
| `172.16.1.10` (reflector host) | 13137 | 10s keepalive `"SERVO"` |

## C# programs this sketch communicates with

- **P3D_to_UDP** / **SimConnect_to_UDP** / **MSFSSimConnectExtractor
  (WindowsFormsApp2)** — the SimConnect↔UDP bridge apps. Each one connects
  a UDP client to `172.16.1.102:13136` (this board) and streams the
  `D,TQ:...,IAS:...` front-panel data packets this sketch parses.
- **ServoTuner** — a standalone WinForms calibration tool that connects
  directly to `172.16.1.102:13136` and lets an operator push manual or
  formula-converted `D,<CODE>:<value>` packets to this board to find/verify
  each servo's min/max/zero pulse positions, independent of the flight sim.
- **StepperVSITester** (new) — primarily a stepper-board bench tool, but
  also connects a third UDP client (`servoClient`) to `172.16.1.102:13136`
  for two rows only: `PITCH`/`BANK`, sent one-decimal-place to match this
  sketch's new `toFloat()`/`round()` parsing.
- **JetRangerHealthMonitor** — listens on UDP **13137** and lights the
  "Servo" indicator green on receipt of this sketch's keepalive.
- No C# project in this repository listens on `172.16.1.10:27000`
  (the debug log stream has no in-repo consumer).
