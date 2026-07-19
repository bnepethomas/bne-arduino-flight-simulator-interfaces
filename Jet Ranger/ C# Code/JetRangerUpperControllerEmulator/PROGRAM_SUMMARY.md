# JetRangerUpperControllerEmulator — Program Summary

Standalone WinForms panel that stands in for the physical Jet Ranger Upper
Controller board when it isn't connected. It shows one toggle button per
switch that the board's firmware actually acts on, and sends the same
`"idx,state"` UDP packets the board itself would send, so `UDP_TO_HID`
(and everything downstream of it) can't tell the difference.

## Program flow

1. **Constructor** calls `BuildSwitchPanel()`, which groups the entries in
   the `Switches` table by logical group (Cabin Heat, Air, Circuit
   Breakers, Instruments, Electrical, Lights) and adds one `GroupBox` per
   group to the scrollable `pnlSwitches` panel, each containing one toggle
   `Button` per switch.
2. **`Switches`** is a fixed `(Index, Label, Group)` table built from the
   `// comment`s in `CreateDcsBiosMessage()` in
   `JET_RANGER_UPPER_CONTROLLER.ino` — only row/column matrix indices that
   carry a comment there are wired to a real switch, so only those 22
   indices appear here. Indices 7‑10, 18‑21, 29‑38, 40‑175 exist in the
   firmware's matrix but drive nothing and are intentionally omitted.
3. **`SwitchButton_Click`** flips the clicked button's stored on/off state,
   recolors it (green = on), and calls `SendButtonToHid`.
4. **`SendButtonToHid`** sends `"<idx>,<0|1>"` as ASCII text over UDP to the
   host/port in `txtHost`/`txtPort`, and reports the outcome in
   `lblStatus`.

## Local network configuration / IP addresses

None locally bound — this tool only sends, it doesn't listen for anything.

## Remote endpoints this app talks to

| Target | Port | Purpose |
|---|---|---|
| `txtHost` (default `172.16.1.11`, the `UDP_TO_HID` board) | `txtPort` (default `4210`) | `"<idx>,<0\|1>"` button press/release packets |

## Programs this communicates with

- **[UDP_TO_HID](../../Jet%20Ranger%20Arduino%20Sketches/UDP_TO_HID/UDP_TO_HID.ino)**
  (`172.16.1.11:4210` by default) — the sole target. `UDP_TO_HID` parses
  `"idx,state"` and drives the corresponding virtual joystick button (idx
  0‑31 → Joystick 1, 32‑63 → Joystick 2, etc.), exactly as it would for a
  packet sent by the real
  **[JET_RANGER_UPPER_CONTROLLER](../../Jet%20Ranger%20Arduino%20Sketches/JET_RANGER_UPPER_CONTROLLER/JET_RANGER_UPPER_CONTROLLER.ino)**
  board — this tool is meant to be run *instead of* that board while it's
  disconnected or being worked on.
