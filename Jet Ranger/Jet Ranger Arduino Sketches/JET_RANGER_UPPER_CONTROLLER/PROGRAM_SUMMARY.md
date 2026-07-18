# JET_RANGER_UPPER_CONTROLLER — Program Summary

Arduino Mega 2560 + W5500 Ethernet shield. Scans the large overhead/upper
panel switch matrix (up to 256 inputs, 176 used) plus a small numeric
keypad, and fans the results out two ways: to DCS-BIOS (serial, for DCS
World) and, over UDP, to a separate USB-HID board (`UDP_TO_HID`) that
presents the buttons to Windows/MSFS as joystick buttons and keystrokes.

## Program flow

1. **Setup**
   - Sets status LED pins, resets the W5500 shield, brings up Ethernet on
     the static IP, opens the UDP sockets below, flashes the red LED for 3s
     while the link settles.
   - Configures digital pins 22–37 as outputs (matrix row drivers) and
     38–49 as inputs with pull-ups (matrix column readers).
   - Clears the button-state and debounce arrays, initialises the keypad.
2. **Main loop** (`loop()`)
   - Toggles the status LEDs every 200ms (heartbeat).
   - Sends a `"UPPER_INPUT"` keepalive to the reflector every 10s.
   - `ScanInputs()`: drives each of the 16 rows low in turn (direct
     `PORTA`/`PORTC` register writes) and reads the corresponding column
     inputs (direct `PIND`/`PING`/`PINL` register reads) to build a
     176-button state array, then calls `FindInputChanges()`.
   - `FindInputChanges()`: for each button whose state changed (and is past
     its debounce window), calls `CreateDcsBiosMessage(index, state)` (a
     per-button DCS-BIOS switch mapping — currently a stub with no bodies
     filled in) and `SendButtonToHid(index, state)`, which forwards the
     press/release over UDP to the `UDP_TO_HID` board.
   - `scanMatrix()`: separately scans the small 4x3 numeric keypad; on a
     key press it calls `SendKeyToHid(key, 1)` then, after the key is
     released, `SendKeyToHid(key, 0)` — forwarding to `UDP_TO_HID` as a
     keyboard press/release.

## Pin usage

| Pin(s) | Function |
|---|---|
| 14 | Green status LED |
| 15 | Red status LED (flashes while Ethernet link comes up) |
| 53 | W5500 Ethernet shield manual reset (`ES1_RESET_PIN`) |
| 22–37 | 16 button-matrix row drivers (direct `PORTA`/`PORTC` writes; up to 176 of 256 possible positions wired) |
| 38–49 | 16 button-matrix column inputs (direct `PIND`/`PING`/`PINL` reads); pins 50–53 reserved for the Ethernet SPI bus so the last 4 "columns" are hard-wired to `1` (unused) |
| A10, A12, A14, A15 | 4x3 numeric keypad rows |
| A9, A11, A13 | 4x3 numeric keypad columns |
| Serial (USB) | DCS-BIOS `DCSBIOS_IRQ_SERIAL` link to the DCS World PC (separate from the Ethernet/UDP path below) |

## Local network configuration

| Setting | Value |
|---|---|
| Static IP | `172.16.1.103` |
| MAC | `A8:61:0A:9E:83:03` |
| Local port `localport` | 7788 (bound, source socket for outbound debug packets) |
| Local port `aliveport` | 13137 (bound, used to send keepalives) |

> **Fixed:** this sketch previously hard-coded the same IP (`172.16.1.101`)
> and MAC as `JET_RANGER_RADIO_CONTROLLER`. It has been renumbered to
> `172.16.1.103` / MAC `...83:03`, which is safe because this board never
> listens for MSFS data on the `MSFSport` (unlike Radio/Servo) — nothing on
> the C# side targets its IP directly, so no other project needed updating.

## Remote endpoints this sketch talks to

| Target | Port | Purpose |
|---|---|---|
| `172.16.1.10` (reflector host) | 27000 | Debug/log messages (`SendDebug`) |
| `172.16.1.10` (reflector host) | 13137 | 10s keepalive `"UPPER_INPUT"` |
| `172.16.1.11` (`UDP_TO_HID` board, `hidIP`) | 4210 (`hidPort`) | Button events (`"<idx>,<state>"`) and key events (`"KEY,<key>,<state>"`) |

## C# programs this sketch communicates with

- None directly over IP for the main data path — this board's Ethernet
  traffic is only debug logging and health-monitor keepalives, both to
  `172.16.1.10`.
- **JetRangerHealthMonitor** — listens on UDP **13137** and lights the
  "Upper Input" indicator green on receipt of this sketch's keepalive.
- No C# project in this repository listens on `172.16.1.10:27000` (the
  debug log stream has no in-repo consumer).
- The DCS-BIOS serial link is consumed by whatever DCS-BIOS export/hub
  software runs on the DCS World PC — that bridge isn't a C# project in
  this repository.
- Its actual button/keyboard output is consumed by the **UDP_TO_HID**
  Arduino sketch (not a C# program), at `172.16.1.11:4210`.
