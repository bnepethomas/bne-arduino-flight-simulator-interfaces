# DataReceiver — Program Summary

Standalone prototype sketch (the sketch file itself is named `DataReceiver`
with no `.ino` extension, inside this folder) that receives JSON flight
data over UDP and drives a single servo from one instrument value. It uses
a different IP scheme (`192.168.1.x`, DHCP-router style) to the rest of the
Jet Ranger fleet (`172.16.1.x`) and talks a different wire format (JSON
rather than the `D,CODE:value` CSV used elsewhere) — this is an
experimental/demo sketch, not part of the production panel network.

## Program flow

1. **Setup**
   - Starts Serial at 115200 baud.
   - Brings up Ethernet with a static IP (DHCP code path is present but
     commented out).
   - Opens the UDP listener on the local port below and attaches one servo,
     centring it at 90°.
2. **Main loop** (`loop()`)
   - Checks for an incoming UDP packet; if present, null-terminates it and
     logs the sender IP/port and raw contents to Serial.
   - Parses the packet as JSON (`ArduinoJson`, 256-byte fixed document).
   - Looks up the `"Airspeed Indicator"` key: if it's a float, maps it from
     the configured instrument range (0–250 knots) to the servo's 0–180°
     range (clamped) and writes it to the servo; if it's the string
     `"N/A"`, skips the update; otherwise logs a warning.
   - All other keys in the JSON payload are currently ignored (only one
     instrument is wired to a servo in this prototype).

## Pin usage

| Pin(s) | Function |
|---|---|
| 8 | Servo signal output (`myServo`), mapped from the "Airspeed Indicator" JSON value |

## Local network configuration

| Setting | Value |
|---|---|
| Static IP | `192.168.1.177` |
| Gateway | `192.168.1.1` |
| Subnet | `255.255.255.0` |
| MAC | `DE:AD:BE:EF:FE:ED` (same placeholder MAC as `UDP_TO_HID` — fine so long as the two are never on the same LAN segment, since they're on different subnets/projects) |
| Local port `localUdpPort` | 5005 (listens for JSON instrument packets) |

## Remote endpoints this sketch talks to

None outbound — it only listens on UDP 5005 and replies via Serial print,
it does not send any UDP packets back to the sender.

## C# programs this sketch communicates with

No C# program — its sender is a **Python** script, not a C# app:

- **Export_From_SimConnect.py** (`Jet Ranger Python Code`) connects to
  SimConnect, polls a set of instrument SimVars (airspeed, attitude,
  altimeter, N1/N2, rotor RPM, engine torque/oil/temp, transmission
  oil/temp, fuel level/pressure, electrical load, etc.) every 100ms, and
  sends them as a single JSON object over UDP to `127.0.0.1:5005` by
  default (`UDP_IP`/`UDP_PORT`) — i.e. it's normally run on the same PC as
  a listener rather than targeting this Arduino's `192.168.1.177` address
  directly; the target IP would need to be changed to point at the board.
- **Test_Receiver.py** (`Jet Ranger Python Code`) is a PC-side stand-in for
  this sketch: it binds UDP port 5005 on `0.0.0.0` and prints/decodes the
  same JSON payloads, useful for verifying `Export_From_SimConnect.py`'s
  output without the Arduino hardware attached.
