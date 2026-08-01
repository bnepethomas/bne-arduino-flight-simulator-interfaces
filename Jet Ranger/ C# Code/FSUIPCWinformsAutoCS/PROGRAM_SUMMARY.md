# FSUIPCWinformsAutoCS — Program Summary

A fourth sim-bridge variant, but reading data through **FSUIPC** (raw
memory offsets, typically used with FSX/P3D via the FSUIPC add-on) instead
of SimConnect. Named `FSUIPCTest` internally — reads engine/electrical/
radio/annunciator offsets and streams the same style of `"D,ALT:...,IAS:..."`
UDP packet to the Servo Controller board that the SimConnect-based bridges
send, and now **also** fans that same payload out to the Stepper and OLED
Controller boards. It now also has the radio interface ported from
`SimConnect_to_UDP`: it sends the `"D,C1A:...,MAINBUS:...,NAVCOM1:..."`
radio packet to the Radio Controller board, and listens on port 27001 for
radio-swap/battery/alternator/standby-frequency commands from the panel,
acting on them through the FSUIPC API instead of SimConnect.

## Program flow

1. **Constructor**: calls `configureForm()`, builds a live annunciator
   light list (`BuildLightList()`, one row per bit in the 0x2F28
   annunciator offset), and starts `timerConnection` (polls once/second
   looking for FSUIPC).
2. **`timerConnection_Tick`**: repeatedly tries `FSUIPCConnection.Open()`;
   once it succeeds, stops itself and starts `timerMain` (ticks every 50ms).
3. **`timerMain_Tick`** (20Hz):
   - Calls `FSUIPCConnection.Process()` to refresh all declared offsets
     (airspeed, avionics master, turbine-out %, gas producer, oil
     pressure/temp, transmission pressure/temp, fuel qty/capacity, battery
     load amps, fuel pressure, torque, rotor RPM, engine N2, altitude, VSI,
     radar altimeter, attitude pitch/bank, main/avionics bus voltage) and
     the separate `"RadioStack"` offset group (COM1/2 active+standby,
     NAV1/2 active+standby, ADF, transponder).
   - Formats a human-readable summary into `textBox1` for on-screen
     debugging.
   - Reads the 21-bit annunciator field (`_annunciators`, offset `0x2F28`)
     and updates the on-screen light list (`UpdateLights`).
   - Runs each changed instrument value through the matching
     `<CODE>_Process()` mapping function (same formulas/tables as
     `JET_RANGER_SERVO_CONTROLLER`/`ServoTuner`/the SimConnect bridges) and
     appends it to a `"D,..."` payload, along with every annunciator's
     on/off state by short code. Also appends a `BARO` field: the
     altimeter/"Kollsman" pressure-setting offset (`0x0330`, hPa×16) is
     converted to the 4-digit inHg×100 form (e.g. `2992`) the OLED
     Controller's BARO display and DCS-BIOS both already use.
   - Throttled to ≥200ms between sends, the accumulated payload is sent to
     the Servo Controller board, and the same bytes are also fanned out to
     the Stepper Controller (which reads `ALT`/`IAS` out of it) and the
     OLED Controller (which reads `ALT`/`BARO` out of it) — each board
     ignores whatever fields it doesn't recognise.
   - Also tracks COM1/2 active+standby frequency and main bus voltage
     (via the same `FsFrequencyCOM`/`FsFrequencyNAV` helpers used for the
     on-screen text) and, throttled the same way (≥200ms if changed, or
     every ≥5s regardless), sends a `"D,C1A:...,C1S:...,C2A:...,C2S:...,
     MAINBUS:...,NAVCOM1:..."` packet to the Radio Controller board.
4. **`StartListener`** (background task, started from the constructor):
   binds to UDP port 27001 and passes every received string to
   `UpdateRadios()`.
5. **`UpdateRadios(command)`** — the FSUIPC port of `SimConnect_to_UDP`'s
   `UpdateRadios()`:
   - `COM1_RADIO_SWAP` / `COM2_RADIO_SWAP` → `FSUIPCConnection.SendControlToFS`
     with `FsControl.COM_STBY_RADIO_SWAP` / `FsControl.COM2_RADIO_SWAP` —
     FSUIPC's equivalent of a SimConnect `TransmitClientEvent`.
   - `AVIONICS_MASTER_SET` → writes directly to the avionics-master offset
     (`0x2E80`) already used by `chkAvionicsMaster_CheckedChanged`, rather
     than an unverified control ID.
   - `MASTER_BATTERY_ON` / `MASTER_BATTERY_OFF` → FSUIPC's classic control
     list only exposes `TOGGLE_MASTER_BATTERY` (no direction-aware "set"),
     so these are guarded against the main bus voltage already read every
     tick, sending the toggle only when it would actually move the switch
     the requested way.
   - `ALTERNATOR_ON` / `ALTERNATOR_OFF` → `FsControl.TOGGLE_MASTER_ALTERNATOR`.
     Unlike battery, no alternator-state offset is read in this project, so
     this is an unconditional toggle and may need re-sending if it fires
     out of sync with the aircraft's actual state.
   - Anything else that parses as a float in 118.00–136.975 is treated as a
     COM1 standby-frequency request: it's BCD-encoded via
     `new FsFrequencyCOM(value).ToBCD()` and sent with
     `FsControl.COM_STBY_RADIO_SET`.
6. **`chkAvionicsMaster_CheckedChanged`**: writes back to the avionics
   master FSUIPC offset when the operator toggles the checkbox.
7. **`frmMain_FormClosing`** stops both timers and closes the FSUIPC
   connection.

## FSUIPC offsets read/written

| Offset | Meaning |
|---|---|
| `0x02BC` | Airspeed |
| `0x2E80` | Avionics master (read/write) |
| `0x08F0` | Turbine-out % / turbine-out temperature |
| `0x08BA` / `0x08B8` | Engine oil pressure / temperature |
| `0x0900` / `0x0904` | Transmission oil pressure / temperature |
| `0x0B74` / `0x0B78` | Fuel percent quantity / fuel capacity |
| `0x282C` | Battery load amps |
| `0x08F8` | Fuel pressure |
| `0x08F4` | Torque percent |
| `0x0898` | Gas producer |
| `0x0908` / `0x0896` | Rotor RPM / engine N2 |
| `0x3324` | Altitude |
| `0x02C8` | Vertical speed |
| `0x31E4` | Radio altimeter |
| `0x2F70` / `0x2F78` | Attitude pitch / bank |
| `0x2F28` | 21-bit annunciator/warning-lamp bitfield |
| `0x2840` / `0x2850` | Main bus voltage / avionics bus voltage |
| `0x0330` | Altimeter/"Kollsman" pressure setting, hPa × 16 (documented FSUIPC offset — sent on as `BARO`, converted to inHg × 100) |
| `RadioStack:0x034E/0x311A/0x3118/0x311C/0x0350/0x311E/0x0352/0x3120/0x0354/0x034C/0x0356` | COM1/2 active+standby, NAV1/2 active+standby, transponder, ADF |

## Local network configuration

| Setting | Value |
|---|---|
| Local listen port | **27001** — bound in `StartListener()` (started from the constructor); receives radio/battery/alternator/standby-frequency commands from the Radio/Upper controller boards |

## Remote endpoints this app talks to

| Target | Port | Purpose |
|---|---|---|
| `172.16.1.101` (Radio Controller) | 13136 | Radio/bus-voltage data packets (`udpClient.Send`) |
| `172.16.1.102` (Servo Controller) | 13136 | Front-panel instrument + annunciator data packets (`frontPanelClient.Send`) |
| `172.16.1.105` (Stepper Controller) | 13136 | Mostly the same payload as the Servo Controller (`stepperClient.Send`), except its `VSI` field is swapped for raw fpm (`(int)sFrontPanel.VERTICAL_SPEED`) right before sending — the Servo Controller's own payload still gets the Bell 206 `VSI_Process()` servo-position number, unchanged. That board reads `ALT`/`IAS`/`VSI` out of it (`IAS` is still the Bell-206 number, same caveat as the Servo Controller's own copy) |
| `172.16.1.104` (OLED Controller) | 13136 | Same payload as the Servo Controller (`oledClient.Send`) — that board reads `ALT`/`BARO` out of it |
| `172.16.1.2` | 26028 | `OutputClient` — connected but unused |

> Note: all five `.Connect(...)` calls that wire up these targets live in
> `Form1.Designer.cs` rather than `Form1.cs`'s constructor — an unusual
> location (that file is normally generated UI layout only) but functionally
> equivalent since it still runs during `InitializeComponent()`.

## Programs this communicates with

- **FSUIPC** (must be installed/running, bridging to FSX/P3D) — data source
  and event/control sink (`SendControlToFS`, offset writes).
- **[JET_RANGER_SERVO_CONTROLLER](../../Jet%20Ranger%20Arduino%20Sketches/JET_RANGER_SERVO_CONTROLLER/JET_RANGER_SERVO_CONTROLLER.ino)**
  (`172.16.1.102:13136`) — receives this app's instrument/annunciator data.
- **[JET_RANGER_RADIO_CONTROLLER](../../Jet%20Ranger%20Arduino%20Sketches/JET_RANGER_RADIO_CONTROLLER/JET_RANGER_RADIO_CONTROLLER.ino)**
  (`172.16.1.101:13136`) — receives radio/bus data; sends control commands
  back to this app's port 27001.
- **[JET_RANGER_UPPER_CONTROLLER](../../Jet%20Ranger%20Arduino%20Sketches/JET_RANGER_UPPER_CONTROLLER/JET_RANGER_UPPER_CONTROLLER.ino)**
  — also a possible sender of control commands to port 27001.
- **[JET_RANGER_STEPPER_CONTROLLER](../../Jet%20Ranger%20Arduino%20Sketches/JET_RANGER_STEPPER_CONTROLLER/JET_RANGER_STEPPER_CONTROLLER.ino)**
  (`172.16.1.105:13136`) — receives the same shared payload as the Servo
  Controller; reads `ALT`/`IAS` out of it to drive its altimeter and
  current-airspeed steppers.
- **[JET_RANGER_OLED_CONTROLLER](../../Jet%20Ranger%20Arduino%20Sketches/JET_RANGER_OLED_CONTROLLER/JET_RANGER_OLED_CONTROLLER.ino)**
  (`172.16.1.104:13136`) — also receives the shared payload; reads
  `ALT`/`BARO` out of it to drive its ALT and BARO OLEDs.
- Alternative to the SimConnect-based bridges (**P3D_to_UDP** /
  **SimConnect_to_UDP** / **MSFSSimConnectExtractor**) for sims that only
  expose data via FSUIPC rather than SimConnect — only one bridge app
  should run against the panel network at a time.
