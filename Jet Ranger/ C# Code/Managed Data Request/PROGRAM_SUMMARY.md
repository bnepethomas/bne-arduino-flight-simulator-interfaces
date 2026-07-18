# Managed Data Request — Program Summary

This is the **unmodified Lockheed Martin Prepar3D SDK sample** ("Managed
Data Request"), kept in the tree as the reference template the three real
bridge apps (`P3D_to_UDP`, `SimConnect_to_UDP`,
`MSFSSimConnectExtractor`) were built from — all three still call
`new SimConnect("Managed Data Request", ...)` internally, which is a
tell-tale sign of that shared ancestry.

## Program flow

1. **Connect** button creates a `SimConnect` client (title "Managed Data
   Request"), registers a single data definition (`title`, latitude,
   longitude, altitude) and hooks `OnRecvOpen`/`OnRecvQuit`/`OnRecvException`.
2. **Request Data** button calls `RequestDataOnSimObjectType` once per
   click (`SIMCONNECT_SIMOBJECT_TYPE.USER`), which triggers a one-shot
   `simconnect_OnRecvSimobjectDataBytype` callback that prints title/lat/lon/
   alt to the on-screen log.
3. **Disconnect** disposes the SimConnect client.

There is no networking beyond the local SimConnect IPC channel to
Prepar3D — no UDP sockets, no offsets, no servo/panel logic.

## Local network configuration / IP addresses

None. This sample only talks to a locally-running Prepar3D instance over
SimConnect (Windows named-pipe/shared-memory IPC, not a UDP/TCP socket).

## Programs this communicates with

- **Prepar3D** (must be running on the same PC) via SimConnect.
- No Arduino boards, no other C# projects — this is a standalone
  reference/template, not part of the Jet Ranger panel network. It exists
  in the tree for comparison against `P3D_to_UDP`, `SimConnect_to_UDP` and
  `MSFSSimConnectExtractor`, which all extended this same starting point
  with UDP output to the panel boards.
