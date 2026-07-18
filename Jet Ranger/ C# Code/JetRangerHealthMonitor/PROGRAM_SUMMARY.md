# JetRangerHealthMonitor — Program Summary

Small WinForms dashboard (namespace `MegaHealthMonitor`) that watches for
the 10-second UDP heartbeat strings sent by every networked Arduino board
in the panel and shows a Comm/Nav, Servo, Joystick and Upper-Input traffic
light so an operator can see at a glance which boards are alive.

## Program flow

1. **`Form1_Load`** calls `StartListener()`, which starts a background task
   binding a `UdpClient` to the listen port below and looping
   `ReceiveAsync()` forever.
2. On each received packet, the payload is matched by prefix:
   - `"COMM_NAV"` → updates `CommNavLastReceived`, turns the CommNav label green.
   - `"SERVO"` → updates `ServoLastReceived`, turns the Servo label green.
   - `"JOYSTICK"` → updates `JoyStickLastReceived`, turns the JoyStick label green.
   - `"UPPER_INPUT"` → updates `UpperInputLastReceived`, turns the Upper Input label green.
   Each match is optionally logged (sender endpoint + timestamp) to a list box.
3. A timer (`tmrHealthCheck_Tick`) calls `CheckHealth()` roughly once a
   second: for each of the four boards, if it's been ≥30s since its last
   packet the indicator turns **red** ("connection lost"); ≥15s turns it
   **orange** ("connection warning"); otherwise it stays green.
4. `cmdToggleLogs_Click` shows/hides the log list box and resizes the form.

## Local network configuration

| Setting | Value |
|---|---|
| Local listen port | **13137** — receives keepalive strings from all four boards |

## Remote endpoints this app talks to

None outbound — this is a passive listener only; it never sends packets.

## Programs this communicates with

Listens for UDP keepalives from all four networked panel boards (no
reply is sent back to any of them):

- **[JET_RANGER_RADIO_CONTROLLER](../../Jet%20Ranger%20Arduino%20Sketches/JET_RANGER_RADIO_CONTROLLER/JET_RANGER_RADIO_CONTROLLER.ino)** — `"COMM_NAV"` every 10s.
- **[JET_RANGER_SERVO_CONTROLLER](../../Jet%20Ranger%20Arduino%20Sketches/JET_RANGER_SERVO_CONTROLLER/JET_RANGER_SERVO_CONTROLLER.ino)** — `"SERVO"` every 10s.
- **[JET_RANGER_UPPER_CONTROLLER](../../Jet%20Ranger%20Arduino%20Sketches/JET_RANGER_UPPER_CONTROLLER/JET_RANGER_UPPER_CONTROLLER.ino)** — `"UPPER_INPUT"` every 10s.
- **[UDP_TO_HID](../../Jet%20Ranger%20Arduino%20Sketches/UDP_TO_HID/UDP_TO_HID.ino)** — `"JOYSTICK"` every 10s.

No dependency on which sim-bridge app (`P3D_to_UDP` / `SimConnect_to_UDP` /
`MSFSSimConnectExtractor`) is running — this monitor only cares about the
Arduino boards, not the flight sim connection.
