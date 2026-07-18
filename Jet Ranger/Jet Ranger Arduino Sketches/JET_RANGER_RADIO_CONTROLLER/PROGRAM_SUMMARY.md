# JET_RANGER_RADIO_CONTROLLER — Program Summary

Arduino Mega 2560 + W5500 Ethernet shield. Drives the two COM/NAV OLED
frequency displays, reads 8 rotary encoders (COM1/COM2/NAV1/NAV2 major &
minor) and a 4x4 button matrix, and keeps the radio panel in sync with the
flight sim over Ethernet.

## Program flow

1. **Setup**
   - Resets the W5500 shield via `ES1_RESET_PIN` (53), then `Ethernet.begin(mac, ip)`.
   - Opens the four UDP sockets listed below, flashes the red status LED while
     it waits `delayBeforeSendingPacket` (3s) for the link to settle, then
     sends a debug "Ethernet Started" message.
   - Initialises the two character OLEDs (via the TCA9548 I2C mux) and the
     8 rotary encoders (acceleration enabled, start positions set).
   - Initialises the 4x4 keypad matrix scanner.
2. **Main loop** (`loop()`)
   - Toggles the green/red status LEDs every `FLASH_TIME` (200ms) as a
     heartbeat indicator.
   - Every 10s sends a `"COMM_NAV"` keepalive UDP packet to the reflector
     (health monitor).
   - Polls all 8 encoders; any change recalculates the relevant standby
     frequency, sends it to the reflector, and refreshes the OLEDs.
   - Scans the 4x4 matrix; key presses on row 0–3 col 0 trigger COM1/COM2
     swap, master battery on/off and alternator on/off commands sent to the
     reflector on port 27001. Other keys are only logged via `SendDebug`.
   - Reads any pending UDP packet on the MSFS port and passes it to
     `ProcessReceivedMSFSString()`, which parses a CSV-style payload
     (`D,C1A:119.500,C1S:120.000,...` for data, `C,B:5` for control/
     brightness) and updates the active/standby frequencies, main bus
     voltage and NAVCOM1 power state, then refreshes the OLEDs if anything
     changed.

## Pin usage

| Pin(s) | Function |
|---|---|
| 14 | Green status LED |
| 15 | Red status LED (flashes while Ethernet link comes up) |
| 53 | W5500 Ethernet shield manual reset (`ES1_RESET_PIN`) |
| 50–52 (+10) | SPI bus to W5500 shield (Mega ICSP header, reserved) |
| I2C (SDA/SCL) | TCA9548 I2C multiplexer (addr `0x70`), driving two OLEDs (addr `0x3C`) — mux port 0 = NAV OLED, port 1 = COMM OLED |
| 31, 30 | Comm1 Major rotary encoder (A, B) |
| 33, 32 | Comm1 Minor rotary encoder (A, B) |
| 34, 35 | Comm2 Major rotary encoder (A, B) |
| 37, 36 | Comm2 Minor rotary encoder (A, B) |
| 38, 39 | Nav1 Major rotary encoder (A, B) |
| 41, 40 | Nav1 Minor rotary encoder (A, B) |
| 42, 43 | Nav2 Major rotary encoder (A, B) |
| 45, 44 | Nav2 Minor rotary encoder (A, B) |
| 22–25 | 4x4 keypad matrix rows |
| 26–29 | 4x4 keypad matrix columns |

## Local network configuration

| Setting | Value |
|---|---|
| Static IP | `172.16.1.101` |
| MAC | `A8:61:0A:9E:83:01` |
| Subnet role | Fixed on the `172.16.1.0/24` avionics LAN |
| Local port `localport` | 7788 (bound, used as the source socket for outbound debug/command packets) |
| Local port `localdebugport` | 7789 (bound, currently unused for send/receive) |
| Local port `MSFSport` | 13136 (listens for sim data packets) |
| Local port `aliveport` | 13137 (bound, used to send keepalives) |

> **Fixed:** `JET_RANGER_UPPER_CONTROLLER` previously hard-coded this same
> IP (`172.16.1.101`) and MAC. It has been renumbered to `172.16.1.103` /
> MAC `...83:03`, so this address is now unique to the Radio Controller.

## Remote endpoints this sketch talks to

| Target | Port | Purpose |
|---|---|---|
| `172.16.1.10` (reflector host) | 27000 | Debug/log messages (`SendDebug`) |
| `172.16.1.10` (reflector host) | 13137 | 10s keepalive `"COMM_NAV"` |
| `172.16.1.10` (reflector host) | 27001 | Standby-frequency updates and radio/battery/alternator control commands (`COM1_RADIO_SWAP`, `COM2_RADIO_SWAP`, `MASTER_BATTERY_ON/OFF`, `ALTERNATOR_ON/OFF`) |

## C# programs this sketch communicates with

- **P3D_to_UDP** / **SimConnect_to_UDP** / **MSFSSimConnectExtractor
  (WindowsFormsApp2)** — these are the sim-specific variants of the same
  SimConnect↔UDP bridge, normally run on the PC at `172.16.1.10`. They
  listen on UDP **27001** for the control commands this sketch sends
  (radio swap, battery, alternator) and translate them into SimConnect
  events, and they send the `D,C1A:...` / `C,...` frequency/status packets
  this sketch listens for on port **13136**.
- **JetRangerHealthMonitor** — listens on UDP **13137** on the same PC and
  turns the "CommNav" indicator green when this sketch's keepalive arrives
  (red/orange if it's been 15–30s+ since the last one).
- No C# project in this repository currently listens on `172.16.1.10:27000`
  — the debug log stream (`SendDebug`) has no in-repo consumer; it would
  need to be watched with a generic UDP listener/packet capture.
