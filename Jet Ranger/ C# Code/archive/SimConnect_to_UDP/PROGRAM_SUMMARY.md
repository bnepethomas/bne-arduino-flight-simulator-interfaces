# SimConnect_to_UDP — Program Summary

WinForms bridge app, functionally identical to `P3D_to_UDP` but built
against `Microsoft.FlightSimulator.SimConnect` for **Microsoft Flight
Simulator (2020/2024)** instead of Prepar3D. Reads flight/engine/radio/
electrical SimVars, converts them to servo-position/warning-lamp values,
and streams them to the Radio and Servo Controller boards over UDP, while
also listening for control commands coming back from the panel.

## Program flow

Identical structure to `P3D_to_UDP` (see that project's summary for full
detail):

1. Constructor connects the outbound UDP clients, starts the port-27001
   command listener, zeroes the internal servo-position array.
2. `StartSimConnect()` opens a `SimConnect("Managed Data Request", ...)`
   session against MSFS, with a 5s auto-retry on failure.
3. `initDataRequest()` registers `Struct1`/`StructRadio`/`StructFrontPanel`
   and requests `StructFrontPanel` once per sim frame.
4. `simconnect_OnRecvSimobjectData()` builds and sends the same
   `"D,C1A:...,MAINBUS:...,NAVCOM1:..."` radio packet and
   `"D,ALT:...,IAS:...,...,BATSW:..."` front-panel packet as `P3D_to_UDP`,
   using the same per-instrument `*_Process()` mapping tables.
5. `UpdateRadios()` handles the same inbound command set (radio swap,
   avionics/battery/alternator, standby-frequency float) from the
   Radio/Upper controller boards.

Code-level differences from `P3D_to_UDP` worth noting: this version uses
C# pattern-matching `switch` cases (`case <= 20:`) instead of exact-value
cases, and its header comments record MSFS-specific data quirks (FlyInside
and CowaSim helicopter add-ons reporting transmission pressure/temp as 0,
rotor RPM pegged at fixed percentages, ITT in Celsius, etc.) that the
`*_Process()` conversions have to work around.

## SimConnect data read

Same SimVar set as `P3D_to_UDP` (altitude, airspeed, VSI, radar altitude,
bank/pitch, rotor & engine RPM, torque, ITT, oil pressure/temp,
transmission pressure/temp, fuel qty, N1, electrical load/master
battery/main bus voltage, COM/NAV frequencies, NAVCOM1 circuit state).

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

- **Microsoft Flight Simulator** (local SimConnect session) — data source
  and event sink.
- **[JET_RANGER_RADIO_CONTROLLER](../../Jet%20Ranger%20Arduino%20Sketches/JET_RANGER_RADIO_CONTROLLER/JET_RANGER_RADIO_CONTROLLER.ino)**
  (`172.16.1.101:13136`) and
  **[JET_RANGER_SERVO_CONTROLLER](../../Jet%20Ranger%20Arduino%20Sketches/JET_RANGER_SERVO_CONTROLLER/JET_RANGER_SERVO_CONTROLLER.ino)**
  (`172.16.1.102:13136`) — same relationship as with `P3D_to_UDP`.
- **[JET_RANGER_UPPER_CONTROLLER](../../Jet%20Ranger%20Arduino%20Sketches/JET_RANGER_UPPER_CONTROLLER/JET_RANGER_UPPER_CONTROLLER.ino)**
  — potential sender of control commands to port 27001.
- Mutually exclusive alternatives — run this one when MSFS (not
  Prepar3D) is the active sim: **P3D_to_UDP** and
  **MSFSSimConnectExtractor** should not be run at the same time as this
  app (all three would fight over UDP port 27001 and duplicate traffic
  to the boards).
