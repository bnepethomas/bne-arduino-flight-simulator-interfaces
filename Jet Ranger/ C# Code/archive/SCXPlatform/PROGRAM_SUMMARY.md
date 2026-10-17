# SCXPlatform (SimConnectCrossPlatform) — Program Summary

A reusable **library** (`src/SimConnectCrossPlatform`) plus a small demo
WinForms app (`src/SimConnectCrossPlatform.WinFormsApp`) and unit tests
(`tests/SimConnectCrossPlatform.Tests`). Unlike every other project in
this folder, this is not itself wired into the Jet Ranger panel network —
it's an engineering effort to replace the copy-pasted SimConnect
boilerplate shared by `P3D_to_UDP`/`SimConnect_to_UDP`/
`MSFSSimConnectExtractor` with a single, properly layered library that can
talk to **either** MSFS2024 or Prepar3D at runtime, with automatic
reconnection.

"Cross-platform" here means *cross-simulator* (MSFS vs Prepar3D), not
cross-OS — it's still a Windows-only WinForms/P/Invoke library.

## Library architecture (`src/SimConnectCrossPlatform`)

| Folder/file | Role |
|---|---|
| `Loader/SimConnectDynamicLoader.cs`, `Win32Native.cs` | Locates and `LoadLibrary`/`GetProcAddress`-loads whichever `SimConnect.dll` is actually installed (MSFS's or Prepar3D's) at runtime, so the app doesn't need to be recompiled per sim |
| `Loader/ISimInterface.cs`, `SimConnectDelegates.cs` | Defines a sim-agnostic interface + native function delegate signatures over the dynamically loaded DLL |
| `Core/SimConnectInterface.cs` | Concrete `ISimInterface` implementation wrapping the loaded SimConnect DLL |
| `Reconnection/ReconnectingSimInterface.cs`, `ReconnectionConfig.cs` | Wraps `ISimInterface` with automatic reconnect/backoff and a heartbeat, exposing `ConnectionStateChanged`/`DataReceived`/`ReconnectionAttempt`/`ExceptionReceived` events |
| `Models/DataDefinition.cs`, `SimVarDefinition.cs`, `SimValue.cs`, `Enums.cs`, `SimConnectConstants.cs`, `SimEvents.cs` | Typed model classes for declaring data definitions/SimVars and reading back typed values (`AsDouble()`, `AsInt()`, etc.) |
| `Translation/SimVarTranslator.cs`, `TranslatedVar.cs` | Helpers for translating between raw SimConnect values and application units |

## Demo app program flow (`SimConnectCrossPlatform.WinFormsApp`)

1. **Connect** button creates a `ReconnectingSimInterface` with backoff/
   heartbeat settings, wires its four events, registers three data
   definitions (`FlightData`: altitude/airspeed/heading/VSI/ground speed/
   lat/lon; `EngineData`: RPM/throttle/fuel flow/fuel qty;
   `AutopilotData`: AP master/heading/altitude/airspeed), requests all
   three once per second, and starts the background worker.
2. **`Sim_DataReceived`** dispatches by request ID to
   `UpdateFlightData`/`UpdateEngineData`/`UpdateAutopilotData`, which
   populate on-screen text boxes (this is a live telemetry viewer, not a
   panel driver).
3. Six buttons (`btnParkBrake`, `btnGear`, `btnLandingLights`,
   `btnNavLights`, `btnBeacon`, `btnStrobes`) call `SendSimEvent(name)`,
   which transmits the named SimConnect event through the wrapped
   interface — a generic "send any named event" demo, not
   Jet-Ranger-specific commands.
4. **Disconnect**/form-closing unwires events and stops the interface.

## Local network configuration / IP addresses

None. All communication is local SimConnect IPC to whichever sim is
installed — there is no UDP/TCP traffic to the Arduino panel boards
anywhere in this project.

## Programs this communicates with

- **Microsoft Flight Simulator** or **Prepar3D** (whichever's SimConnect
  DLL is found at runtime) — data source and event sink.
- No Arduino boards and no other C# project in this repository consume
  its output — it is not currently plumbed into the Jet Ranger panel
  network. If it's intended as the eventual replacement for
  `P3D_to_UDP`/`SimConnect_to_UDP`/`MSFSSimConnectExtractor`, that
  integration (adding the UDP send/receive logic those three apps have)
  hasn't been done yet.
