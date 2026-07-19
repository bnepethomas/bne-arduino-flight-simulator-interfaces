# JetRangerRadioControllerEmulator — Program Summary

Standalone WinForms panel that stands in for the physical Jet Ranger Radio
Controller board when it isn't connected. It shows the two character-OLED
frequency displays, a clickable rotary knob for each of the 8 real encoders,
the COM1/COM2 frequency-swap ("XFER") keys, and the master battery/alternator
key, and sends the same UDP payloads the board itself would send to the
reflector on port 27001 — so the sim-bridge apps (`P3D_to_UDP` /
`SimConnect_to_UDP` / `MSFSSimConnectExtractor` / `FSUIPCWinformsAutoCS`)
can't tell the difference.

Only controls that actually do something in the firmware are emulated —
same filtering principle as `JetRangerUpperControllerEmulator`. The 4x4
keypad's digit/letter/`*`/`#` keys only log a debug message in
`handleKeyPress()` in `JET_RANGER_RADIO_CONTROLLER.ino` and are omitted; only
the four keys wired to a real action (battery/alternator master, COM1 swap,
COM2 swap) are shown.

## Program flow

1. **Constructor** builds two "OLED" panels (`BuildOledDisplay`) — COMM
   (COM1/COM2) and NAV (NAV1/NAV2) — each a black panel with two green
   monospace lines, mirroring `updateCOMM()`/`updateNAV()`'s two-line,
   two-radio layout. Below each display it adds a row of per-radio group
   boxes (`BuildRadioGroup`), each holding a Major (whole-unit) knob, a
   Minor (5 kHz) knob, and — for COM1/COM2 only — an XFER swap button.
2. **`RadioState`** holds each radio's tuning as `Major`/`Minor` detent
   counts, exactly like the sketch's raw encoder position: `Major` is
   clamped to `0..MajorMax-1` (17 steps from 118 for COM, 10 steps from 108
   for NAV — matching `Comm1MajorEncoder`/`Nav1MajorEncoder`'s configured
   ranges), `Minor` is a 5 kHz index `0..199` that **wraps** (matching the
   sketch's explicit roll-over logic for the minor encoder, e.g. `com1S`
   `tandbyFrequency` rolling from `.995` back to `.000`). `Standby` is
   computed the same way as the sketch's `convertedchann`.
3. **`KnobControl`** (`KnobControl.cs`) is a small custom control standing
   in for a physical encoder: clicking its right half turns it one detent
   clockwise (`+1`), the left half one detent counter-clockwise (`-1`); the
   pointer line is cosmetic feedback only. Each knob's `Turned` event
   updates the relevant `RadioState` field, refreshes both OLED panels, and
   sends the new standby frequency as bare ASCII text (e.g. `"121.010"`) —
   matching `SendButtonToHid`-style direct sends in the sketch's encoder
   handlers.
4. **XFER buttons** (COM1/COM2 only, matching the sketch's row2/col0 and
   row3/col0 keys) swap the local Active/Standby pair so the panel looks
   right immediately (there's no live sim feed in this emulator to report
   the swap back), then send `COM1_RADIO_SWAP` or `COM2_RADIO_SWAP` +
   `AVIONICS_MASTER_SET` (COM2 only, matching `handleKeyPress`'s row3/col0
   branch, which fires both).
5. **MASTER ON / MASTER OFF** buttons (matching the sketch's row0/col0 and
   row1/col0 keys, each of which fires two commands in `handleKeyPress`)
   send `MASTER_BATTERY_ON` + `ALTERNATOR_ON`, or `MASTER_BATTERY_OFF` +
   `ALTERNATOR_OFF`.

## Note: NAV standby-frequency bug (now fixed in the firmware too)

`JET_RANGER_RADIO_CONTROLLER.ino`'s Nav1/Nav2 encoder handlers used to resend
`com1StandbyFrequency`/`com2StandbyFrequency` to port 27001 instead of the
Nav frequency they'd just computed — a copy/paste leftover from the COM
handlers. This emulator was written to send each radio's own
correctly-computed standby frequency from the start, and the same fix has
since been applied to the sketch itself, so the two now agree. Neither
version changed behaviour anywhere it currently works: none of the C#
sim-bridge apps in this repo distinguish NAV from COM on that port anyway (a
bare decimal is always treated as a COM1 standby set — see
`FSUIPCWinformsAutoCS/Form1.cs` `UpdateRadios()`), so the bug was already a
no-op for Nav tuning end-to-end.

## Local network configuration / IP addresses

None locally bound — this tool only sends, it doesn't listen for anything.

## Remote endpoints this app talks to

| Target | Port | Purpose |
|---|---|---|
| `txtHost` (default `172.16.1.10`, the reflector host) | `txtPort` (default `27001`) | Standby-frequency updates (`"<freq>"`) and radio/battery/alternator commands (`COM1_RADIO_SWAP`, `COM2_RADIO_SWAP`, `AVIONICS_MASTER_SET`, `MASTER_BATTERY_ON/OFF`, `ALTERNATOR_ON/OFF`) |

This app doesn't send the sketch's `SendDebug` traffic (port 27000) or its
10s `"COMM_NAV"` keepalive (port 13137) — neither carries any control
information, and `JetRangerUpperControllerEmulator` established the same
scope decision (functional traffic only, no debug/heartbeat).

## Programs this communicates with

- **P3D_to_UDP** / **SimConnect_to_UDP** / **MSFSSimConnectExtractor
  (WindowsFormsApp2)** / **FSUIPCWinformsAutoCS** — whichever sim-bridge app
  is running on the PC at `172.16.1.10`, listening on UDP **27001** for
  standby-frequency and radio/battery/alternator commands, exactly as it
  would from the real
  **[JET_RANGER_RADIO_CONTROLLER](../../Jet%20Ranger%20Arduino%20Sketches/JET_RANGER_RADIO_CONTROLLER/JET_RANGER_RADIO_CONTROLLER.ino)**
  board — this tool is meant to be run *instead of* that board while it's
  disconnected or being worked on.
