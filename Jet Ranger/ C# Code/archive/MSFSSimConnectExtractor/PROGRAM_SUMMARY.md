# MSFSSimConnectExtractor (WindowsFormsApp2 / SimConnect_To_IP) — Program Summary

The oldest of the three MSFS/Prepar3D bridge apps (solution name
`SimConnect_to_IP.sln`, project `WindowsFormsApp2`, executable class
`frmMain` in namespace `WindowsFormsApp2`). Built against
`Microsoft.FlightSimulator.SimConnect`. Functionally the same shape as
`SimConnect_to_UDP` and `P3D_to_UDP` — reads sim data, maps it to servo
positions, streams it to the panel boards over UDP, and turns inbound
panel commands into SimConnect events — but an earlier revision: its
servo tables have 16 entries (no `Fuel_Load`/`Electrical_Load` columns yet)
and its `IAS_Process`/`ITT_Process` switches use exact-value cases instead
of range comparisons. The project folder also contains a `.odt` write-up,
**"Jet Ranger MSFS UDP Exporter Explanations.odt"**, with the author's own
notes on this exporter (not parsed here — open directly in a word
processor for the prose explanation).

## Program flow

1. Constructor connects the outbound UDP clients, calls `StartListener()`,
   and zeroes the servo-position array.
2. **Connect** button opens `SimConnect("Managed Data Request", ...)`;
   `initDataRequest()` registers `Struct1`/`StructRadio`/`StructFrontPanel`
   and requests `StructFrontPanel` every sim frame.
3. `simconnect_OnRecvSimobjectData()` builds and sends the same two
   packet types as the other two bridge apps: a compact radio/bus-voltage
   packet to the Radio Controller, and a larger instrument-data packet to
   the Servo Controller, throttled to ~200ms with a 5s forced refresh.
4. `StartListener()` (UDP port 27001) receives control strings from the
   panel and routes them through `UpdateRadios()` to the same command set
   (radio swap, avionics/battery/alternator on/off, standby-frequency
   float) as the newer variants.

## SimConnect data read

Same SimVar family as the other two bridges (altitude, airspeed, VSI,
radar altitude, bank/pitch, rotor & engine RPM, torque, ITT, oil
pressure/temp, transmission pressure/temp, fuel qty, N1, electrical
load/master battery/main bus voltage, COM/NAV frequencies, NAVCOM1 circuit
state) — 16-servo table (predates the Fuel Load/Electrical Load servos
added later in `JET_RANGER_SERVO_CONTROLLER`/`ServoTuner`/the other two
bridge apps).

## Local network configuration

| Setting | Value |
|---|---|
| Local listen port | **27001** — control commands from the Radio/Upper controller boards |

## Remote endpoints this app talks to

| Target | Port | Purpose |
|---|---|---|
| `172.16.1.101` (Radio Controller) | 13136 | Radio/bus-voltage data packets |
| `172.16.1.102` (Servo Controller) | 13136 | Front-panel instrument data packets |
| `172.16.1.2` | 26028 | `OutputClient` — connected but unused (no send code active) |

## Programs this communicates with

- **Microsoft Flight Simulator** (local SimConnect session).
- **[JET_RANGER_RADIO_CONTROLLER](../../Jet%20Ranger%20Arduino%20Sketches/JET_RANGER_RADIO_CONTROLLER/JET_RANGER_RADIO_CONTROLLER.ino)**
  (`172.16.1.101:13136`) and
  **[JET_RANGER_SERVO_CONTROLLER](../../Jet%20Ranger%20Arduino%20Sketches/JET_RANGER_SERVO_CONTROLLER/JET_RANGER_SERVO_CONTROLLER.ino)**
  (`172.16.1.102:13136`).
- **[JET_RANGER_UPPER_CONTROLLER](../../Jet%20Ranger%20Arduino%20Sketches/JET_RANGER_UPPER_CONTROLLER/JET_RANGER_UPPER_CONTROLLER.ino)**
  — potential sender of control commands to port 27001.
- Superseded by / duplicate-purpose with **SimConnect_to_UDP** (a newer
  revision of the same MSFS bridge) and **P3D_to_UDP** (the Prepar3D
  equivalent) — only one of the three should be running against the panel
  network at a time.
