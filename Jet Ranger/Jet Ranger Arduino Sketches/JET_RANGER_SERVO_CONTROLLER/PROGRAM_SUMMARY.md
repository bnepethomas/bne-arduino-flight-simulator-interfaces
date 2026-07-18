# JET_RANGER_SERVO_CONTROLLER — Program Summary

Arduino Mega 2560 + W5500 Ethernet shield. Drives 15 warning lamps and 15
analogue-gauge servos (airspeed, VSI, engine torque, oil pressure/temp,
transmission pressure/temp, EGT, rotor/engine RPM, N1, fuel, fuel/electrical
load, pitch/bank) from flight-sim data received over Ethernet.

## Program flow

1. **Setup**
   - Sets status LED pins, then calls `InitialiseWarningLights()` which sets
     all 15 warning-lamp pins to `OUTPUT` and turns them all on (lamp test).
   - Resets the W5500 shield, brings up Ethernet on the static IP, opens the
     UDP sockets below, and flashes the red LED for 3s while the link
     settles.
   - Runs a self-test/zero sequence: every servo is driven to its zero
     position, then swept from its minimum to maximum position and back
     (visible "wiggle test"), turning off one warning lamp after each
     servo's sweep completes as a visual progress indicator. All target
     positions are reset to the zero position at the end.
2. **Main loop** (`loop()`)
   - Toggles the status LEDs every 200ms (heartbeat).
   - Sends a `"SERVO"` keepalive to the reflector every 10s.
   - Every 5ms (`incomingcheckinterval`), checks for an incoming UDP packet
     on the MSFS port and, if present, passes it to
     `ProcessReceivedMSFSString()`, which parses the CSV payload
     (`D,TQ:120,IAS:80,...` / `C,B:5`) and updates:
     - 15 servo **target** positions (see `HandleOutputValuePair`), and
     - 15 boolean warning-light states, each driving its lamp pin directly.
   - Every 5ms (`servoCheckInterval`), `UpdateServoPos()` nudges each servo
     one step at a time from its current position towards its target
     (smooth, non-blocking motion), attaching each servo on first use.
     `CheckServoIdleTime()` then detaches any servo that hasn't moved for
     1s (`ServoIdleTime`) to reduce jitter/power draw.

## Pin usage

| Pin(s) | Function |
|---|---|
| 14 | Green status LED |
| 15 | Red status LED (flashes while Ethernet link comes up) |
| 53 | W5500 Ethernet shield manual reset (`ES1_RESET_PIN`) |
| 2 | Airspeed servo |
| 3 | (Radar altitude — port reserved, not driven by a `Set...` routine) |
| 4 | Vertical Speed (VSI) servo |
| 6 | Gas Producer / N1 servo |
| 7 | EGT / turbine ITT servo |
| 8 | Engine RPM (RPME) servo |
| 9 | Rotor RPM (RPMR) servo |
| 11 | Engine Torque servo |
| 12 | Oil Temp (OILT) servo |
| 13 | Oil Pressure (OILP) servo |
| 26 | Attitude Pitch servo |
| 27 | Attitude Bank/Roll servo |
| 28 | Electrical Load servo |
| 29 | Fuel Load servo |
| 44 | Transmission Temp (XMSNT) servo |
| 45 | Transmission Pressure (XMSNP) servo |
| 46 | Fuel Quantity servo |
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
- **JetRangerHealthMonitor** — listens on UDP **13137** and lights the
  "Servo" indicator green on receipt of this sketch's keepalive.
- No C# project in this repository listens on `172.16.1.10:27000`
  (the debug log stream has no in-repo consumer).
