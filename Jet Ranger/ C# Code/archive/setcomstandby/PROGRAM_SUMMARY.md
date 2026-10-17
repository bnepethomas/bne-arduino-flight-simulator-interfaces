# setcomstandby — Program Summary

Tiny interactive console tool (namespace `SimConnectFlightControl`) for
firing individual SimConnect events by hand, without needing the full
panel-network sim-bridge app running — useful for isolating whether a
given SimConnect event actually works in the currently-loaded aircraft.

## Program flow

1. Opens a `SimConnect("Flight Controller", ...)` session and maps two
   client events: `MASTER_BATTERY_SET` and `COM_STBY_RADIO_SET`.
2. Loops reading console key presses:
   - **B** → `TurnOffBattery()`: transmits `KEY_MASTER_BATTERY_SET` with
     parameter `0`.
   - **C** → `SetCom1Standby()`: transmits `KEY_COM_STBY_RADIO_SET` with a
     hard-coded BCD value `0x2150` (121.50 MHz).
   - **Esc** → exits the loop and the program.

## Local network configuration / IP addresses

None — SimConnect-only, no sockets.

## Programs this communicates with

- **Microsoft Flight Simulator / Prepar3D** (local SimConnect session) —
  the sole interaction. No Arduino boards or other C# projects involved;
  this is a standalone manual test tool, not part of the panel network's
  normal runtime.
