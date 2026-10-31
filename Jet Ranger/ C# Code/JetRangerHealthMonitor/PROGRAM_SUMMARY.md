# JetRangerHealthMonitor — Program Summary

Small WinForms dashboard (namespace `MegaHealthMonitor`) that watches for
the 10-second UDP heartbeat strings sent by every networked Arduino board
in the panel and shows a Comm/Nav, Servo, Joystick, Upper-Input, Stepper
and Dual Stepper traffic light so an operator can see at a glance which
boards are alive.

## Program flow

1. **`Form1_Load`** calls `StartListener()`, which starts a background task
   binding a `UdpClient` to the listen port below and looping
   `ReceiveAsync()` forever.
2. On each received packet, the payload is matched by prefix:
   - `"COMM_NAV"` → updates `CommNavLastReceived`, turns the CommNav label green.
   - `"SERVO"` → updates `ServoLastReceived`, turns the Servo label green.
   - `"JOYSTICK"` → updates `JoyStickLastReceived`, turns the JoyStick label green.
   - `"UPPER_INPUT"` → updates `UpperInputLastReceived`, turns the Upper Input label green.
   - `"STEPPER"` → updates `StepperLastReceived`, turns the Stepper label green.
   - `"DUAL_STEPPER"` → updates `DualStepperLastReceived`, turns the Dual Stepper label green.
   Each match is optionally logged (sender endpoint + timestamp) to a list box.
3. A timer (`tmrHealthCheck_Tick`) calls `CheckHealth()` roughly once a
   second: for each of the six boards, if it's been ≥30s since its last
   packet the indicator turns **red** ("connection lost"); ≥15s turns it
   **orange** ("connection warning"); otherwise it stays green.
4. `cmdToggleLogs_Click` shows/hides the log list box and resizes the form.

## Local network configuration

| Setting | Value |
|---|---|
| Local listen port | **13137** — receives keepalive strings from all six boards |

## Remote endpoints this app talks to

None outbound — this is a passive listener only; it never sends packets.

## Programs this communicates with

Listens for UDP keepalives from all six networked panel boards (no
reply is sent back to any of them). Matching is by message prefix only,
not sender IP, so any board sending the right prefix is accepted:

- **[JET_RANGER_RADIO_CONTROLLER](../../Jet%20Ranger%20Arduino%20Sketches/JET_RANGER_RADIO_CONTROLLER/JET_RANGER_RADIO_CONTROLLER.ino)** — `"COMM_NAV"` every 10s.
- **[JET_RANGER_SERVO_CONTROLLER](../../Jet%20Ranger%20Arduino%20Sketches/JET_RANGER_SERVO_CONTROLLER/JET_RANGER_SERVO_CONTROLLER.ino)** — `"SERVO"` every 10s.
- **[JET_RANGER_ADF_CONTROLLER](../../Jet%20Ranger%20Arduino%20Sketches/JET_RANGER_ADF_CONTROLLER/JET_RANGER_ADF_CONTROLLER.ino)** (the combined ADF and Upper board, 172.16.1.107) — `"ADF_PANEL"` and `"UPPER_INPUT"` every 10s, each with `V5:` and `V12:` readings. The old `JET_RANGER_UPPER_CONTROLLER` (retired 31 October 2026) sent only `"UPPER_INPUT"`.
- **[UDP_TO_HID](../../Jet%20Ranger%20Arduino%20Sketches/UDP_TO_HID/UDP_TO_HID.ino)** — `"JOYSTICK"` every 10s.
- **[JET_RANGER_STEPPER_CONTROLLER](../../Jet%20Ranger%20Arduino%20Sketches/JET_RANGER_STEPPER_CONTROLLER/JET_RANGER_STEPPER_CONTROLLER.ino)** — `"STEPPER"` every 10s (added alongside this app's Stepper indicator).
- **[JET_RANGER_OLED_DUAL_STEPPER_CONTROLLER](../../Jet%20Ranger%20Arduino%20Sketches/JET_RANGER_OLED_DUAL_STEPPER_CONTROLLER/PROGRAM_SUMMARY.md)** — `"DUAL_STEPPER"` every 10s. Deliberately a distinct prefix from `JET_RANGER_STEPPER_CONTROLLER`'s `"STEPPER"` (not just a longer string that happens to start the same way) — this app matches by `StartsWith()`, so if the two boards' prefixes had overlapped (e.g. `"STEPPER"` vs `"STEPPER_DUAL"`), one would have lit up the other's indicator too.

No dependency on which sim-bridge app (`FSUIPCWinformsAutoCS` /
`MSFSWinformsAutoCS`) is running — this monitor only cares about the
Arduino boards, not the flight sim connection.

## ADF and voltages (2026-10-25)

- **ADF:** a seventh indicator, `lblAdf`, for the `ADF_PANEL` heartbeat (orange after 15 s, red after 30 s, logged like the others).
- **Voltage table:** every board now adds its 5 V rail to its heartbeat after its name (`SERVO V5:5.02`) and the ADF controller also its 12 V rail (`ADF_PANEL V5:5.03 V12:12.41`). `RecordVoltages()` finds `V5:` and `V12:`
  with a regular expression and keeps the last reading and its time per board; the **Show Voltages** button (`cmdToggleVoltages`) shows a `ListView` (`lvVoltages`) with one row per board: 5 V, 12 V (only the ADF; the others
  say "not measured") and the time with "N s ago". Readings outside 4.75 to 5.25 V (5 V) or 10.8 to 15.0 V (12 V) are orange; a reading older than 35 s, or none, is grey. `ApplyLayout()` sizes the window for the log list and
  the table. Tested with simulated heartbeats. A board that has not been reflashed shows "no reading" but its indicator still works.
