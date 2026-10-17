# cmdlineWarningMute — Program Summary

Tiny one-shot CLI utility (namespace `DS206EventTrigger`) that toggles a
single custom FSUIPC control on and off — used as a manual workaround/test
for the DodoSim Bell 206 add-on's generator switch, which apparently isn't
reachable through a normal SimConnect event.

## Program flow

1. Opens an FSUIPC connection (`FSUIPCConnection.Open()`).
2. `SetCustomEvent(32884, 0)` — writes the parameter offset (`0x3114`)
   first and flushes it, then writes the control-number offset (`0x3110`,
   the "confirmed via FSUIPC6.log" generator-switch control ID 32884) and
   flushes it, which fires the control with that parameter. Logs each
   step with a timestamp.
3. Sleeps 2 seconds.
4. Repeats `SetCustomEvent(32884, 1)` to flip the switch back.
5. Closes the FSUIPC connection and exits.

## FSUIPC offsets used

| Offset | Meaning |
|---|---|
| `0x3110` | Custom control number (`ControlNumber`) — set to `32884` (generator switch) |
| `0x3114` | Custom control parameter (`ControlParam`) — `0` then `1` |

## Local network configuration / IP addresses

None — FSUIPC-only, no sockets.

## Programs this communicates with

- **FSUIPC** (must be running, bridging to FSX/P3D) — the sole
  interaction; no Arduino boards or other C# projects are involved. This
  is a standalone diagnostic/workaround tool, not part of the panel
  network's normal runtime.
