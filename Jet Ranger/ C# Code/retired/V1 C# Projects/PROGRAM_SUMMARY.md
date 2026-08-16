# V1 C# Projects — Program Summary

A legacy folder (dated ~2014) of early, pre-Ethernet prototypes from
before the current `172.16.1.x` UDP panel network existed. Everything here
talked to the Arduino boards over a plain **serial COM port**, and to DCS
World over its classic **Export.lua / SIOC** integration rather than
SimConnect/FSUIPC. Kept for historical reference; none of these are part of
the current running system. Four sub-folders contain actual C# programs;
two others (`ArduinoMultiOLEDRx - 20140101` and `soic`) contain an Arduino
sketch and SIOC configuration files respectively, not C# code, and aren't
covered below.

## ArduinoSender (`multioledreceiver/ArduinoSender`)

WinForms app (`WindowsFormsApplication1.Form1`). **Button 1** opens a serial
port and, in a tight blocking loop (`Application.DoEvents()` + 50ms sleep),
writes an `STX <oled#> <line#> <text> ETX` framed message to two OLED
displays (`oled1`/`oled2`) showing the current time — a manual demo of the
serial OLED protocol later reused in `JET_RANGER_RADIO_CONTROLLER`'s
character-OLED code. **Button 2** stops the loop and exits. No networking —
serial only, port/settings configured via the WinForms designer.

## ArdunioCLISender (`multioledreceiver/ArdunioCLISender`)

Console CLI tool combining serial + UDP. Opens `COM7` at 115200 baud (with
a manual COM-port prompt fallback), then a UDP listener on **port 7781**
that receives DCS World `Export.lua`-style semicolon-delimited
`"tag=value;tag=value;..."` telemetry strings (paired LUA scripts live in
`ArdunioCLISender/LUA/`: `soic_conv_BeforeNextFrame.lua`,
`soic_conv_ExportStart.lua`). Each incoming packet is SHA1-hashed to detect
changes, then parsed for VHF/NAV/DME/autopilot/ADF tags (200–209) or
CMSP/CMSC countermeasures text, and the changed fields are written to up
to 7 serial-attached OLED displays framed the same way as `ArduinoSender`.
Supports `debug` and `time` command-line flags (the latter just cycles a
clock display on all 7 OLEDs without waiting for DCS data).

## ptdodoexport (`ptdodoexport`)

Minimal console poller that P/Invokes `GetEngOutLightState()` out of
`DodoSim206FSXDataExport.dll` (the DodoSim Bell 206 add-on's own export
DLL) every 500ms and prints the result to the console. A read-only
diagnostic for checking the add-on's engine-out light state via its native
DLL interface — no serial, no UDP.

## udp_sender (`udp sender`, namespace `Soic_Conv_Output`)

Console CLI meant to bridge **SIOC** (Sim I/O Controller, used with
Opencockpits-style hardware) and DCS World's `Export.lua` telemetry via
UDP, using the same paired LUA scripts as `ArdunioCLISender`. As currently
written, `Main()` only runs a hard-coded self-test: it fires 41 UDP packets
of the form `"<n>=1:<n-1>=0:"` to `127.0.0.1:7777` (100↦140), one every
200ms, and then returns — the real bridging logic lives in a separate
`Connect(server, message)` method (TCP to a local SIOC server on port
**8092**, then relaying UDP port **7777** telemetry back to it as
`"Arn.Resp:<data>"`) that `Main()` never actually calls, so in its current
state this program only sends test data and doesn't perform the SIOC
bridging it was written for.

## Local network configuration

| Project | Local port | Purpose |
|---|---|---|
| ArdunioCLISender | 7781 (UDP) | Receives DCS `Export.lua` telemetry |
| udp_sender | none bound (test loop only); `Connect()` — unreachable — would bind 7777 (UDP) | Would receive DCS `Export.lua` telemetry for SIOC relay |

## Remote endpoints these talk to

| Project | Target | Port | Purpose |
|---|---|---|---|
| udp_sender | `127.0.0.1` (loopback) | 7777 (UDP) | Self-test packets (`Main()`) |
| udp_sender | `Connect()`'s `server` parameter | 8092 (TCP) | SIOC server (unreachable code path) |

## Programs these communicate with

- **ArduinoSender** / **ArdunioCLISender** → a **serial-attached Arduino**
  running the old MultiOLED-receiver sketch
  (`multioledreceiver/ArduinoReceiver/ArduinoMultiOLEDRx/ArduinoMultiOLEDRx.ino`,
  not part of the current fleet) — not any of the current
  `172.16.1.x` boards.
- **ArdunioCLISender** / **udp_sender** → **DCS World**, via its
  `Export.lua` scripting hook and the paired `soic_conv_*.lua` scripts —
  not SimConnect/FSUIPC.
- **ptdodoexport** → the locally-installed **DodoSim206FSXDataExport.dll**
  add-on — no other process.
- None of these four programs talk to any project in the current
  `172.16.1.x` fleet (Radio/Servo/Upper controllers, UDP_TO_HID) or to the
  current sim-bridge apps (`P3D_to_UDP`/`SimConnect_to_UDP`/
  `MSFSSimConnectExtractor`/`FSUIPCWinformsAutoCS`).
