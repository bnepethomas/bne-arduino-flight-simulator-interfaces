# P3D_to_UDP — Program Summary

WinForms bridge app that connects to a running **Prepar3D** session via
`LockheedMartin.Prepar3D.SimConnect`, reads flight/engine/radio/electrical
data, converts it into the servo-position and warning-lamp values the
Arduino boards expect, and streams it out as UDP text packets. It also
listens for control commands coming back from the Radio/Upper controller
boards and turns them into SimConnect events (radio swap, battery,
alternator, standby-frequency set). This is the Prepar3D-specific sibling
of `SimConnect_to_UDP` and `MSFSSimConnectExtractor` — all three share
near-identical code, differing mainly in which SimConnect SDK they link
against and small per-sim data-availability quirks noted in code comments.

## Program flow

1. **Startup** (`frmMain()` constructor): connects the three outbound UDP
   clients (radio, front-panel, "output"), starts the inbound command
   listener, and initialises the internal servo-position array to each
   servo's zero position.
2. **Connect** button (`StartSimConnect`): opens a `SimConnect("Managed
   Data Request", ...)` session; on failure, arms a 5s retry timer.
   `initDataRequest()` registers three data definitions (`Struct1` — a
   large "everything" struct, `StructRadio` — just the radio/bus-voltage
   subset, `StructFrontPanel` — the full instrument-panel subset) and
   requests `StructFrontPanel` once per sim frame.
3. **`simconnect_OnRecvSimobjectData`** (fires every sim frame):
   - `DATA_REQUESTS.RADIOS` case: tracks COM1/2 active+standby, main bus
     voltage and NAVCOM1 power; if changed (or every 5s regardless),
     builds a `"D,C1A:...,C1S:...,C2A:...,C2S:...,MAINBUS:...,NAVCOM1:..."`
     packet and sends it to the **Radio Controller** board.
   - `DATA_REQUESTS.REQUEST_FRONTPANEL` case: tracks ~15 instrument values,
     runs each through a `*_Process()` mapping function (identical
     min/max/zero tables to `JET_RANGER_SERVO_CONTROLLER`/`ServoTuner`) to
     convert sim units into raw servo-position values, and builds a
     `"D,ALT:...,IAS:...,RPMR:...,...,BATSW:..."` packet sent (throttled to
     every ≥200ms, or forced every ≥5s) to the **Servo Controller** board.
4. **`StartListener`**: a background UDP listener on port 27001 receives
   control strings from the Radio/Upper controller boards and passes them
   to `UpdateRadios()`, which maps known keywords
   (`COM1_RADIO_SWAP`/`COM2_RADIO_SWAP`/`AVIONICS_MASTER_SET`/
   `MASTER_BATTERY_ON|OFF`/`ALTERNATOR_ON|OFF`) to `SendEvent()` calls, or —
   if the string parses as a plain float in the 118.00–136.975 range —
   sets COM1 standby frequency directly via `KEY_COM_STBY_RADIO_SET_HZ`.
5. **Disconnect** disposes the SimConnect session; **Request Data** is a
   manual one-shot debug trigger.

## FSUIPC / SimConnect data read

Reads Prepar3D SimVars (altitude, airspeed, VSI, radar altitude, bank/pitch,
rotor & engine RPM, torque, ITT, oil pressure/temp, transmission
pressure/temp, fuel qty, N1, electrical load/master battery/main bus
voltage, COM/NAV frequencies, NAVCOM1 circuit state) — see
`initDataRequest()` for the full `AddToDataDefinition` list.

## Local network configuration

| Setting | Value |
|---|---|
| Local listen port | **27001** — control commands from the Radio/Upper controller boards |

## Remote endpoints this app talks to

| Target | Port | Purpose |
|---|---|---|
| `172.16.1.101` (Radio Controller) | 13136 | Radio/bus-voltage data packets (`udpClient`) |
| `172.16.1.102` (Servo Controller) | 13136 | Front-panel instrument data packets (`frontPanelClient`) |
| `172.16.1.2` | 26028 | `OutputClient` — connected but currently unused; all `Output_Payload` send code is commented out |

## Programs this communicates with

- **Prepar3D** (local SimConnect session) — data source and event sink.
- **[JET_RANGER_RADIO_CONTROLLER](../../Jet%20Ranger%20Arduino%20Sketches/JET_RANGER_RADIO_CONTROLLER/JET_RANGER_RADIO_CONTROLLER.ino)**
  (`172.16.1.101:13136`) — receives radio/bus data; sends control commands
  back to this app's port 27001.
- **[JET_RANGER_SERVO_CONTROLLER](../../Jet%20Ranger%20Arduino%20Sketches/JET_RANGER_SERVO_CONTROLLER/JET_RANGER_SERVO_CONTROLLER.ino)**
  (`172.16.1.102:13136`) — receives instrument/servo data.
- **[JET_RANGER_UPPER_CONTROLLER](../../Jet%20Ranger%20Arduino%20Sketches/JET_RANGER_UPPER_CONTROLLER/JET_RANGER_UPPER_CONTROLLER.ino)**
  — also a possible sender of control commands to port 27001, though it
  currently has no battery/alternator/radio-swap keys wired to send them.
- Mutually exclusive alternatives, **not** run at the same time as this
  app: **SimConnect_to_UDP** (MSFS) and **MSFSSimConnectExtractor**
  (MSFS) — whichever matches the flight sim actually in use is the one
  that should be running.
- `172.16.1.2:26028` has no consumer identified in this repository — the
  `OutputClient` connection is established but never used to send data.
