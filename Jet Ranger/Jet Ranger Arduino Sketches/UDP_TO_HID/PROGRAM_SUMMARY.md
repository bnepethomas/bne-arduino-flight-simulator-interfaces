# UDP_TO_HID — Program Summary

Arduino board with native USB HID support (e.g. Leonardo/Micro-class, using
`Joystick.h`/`Keyboard.h`) + W5500 Ethernet shield. Acts as a network-to-USB
bridge: it receives button/key commands over UDP from
`JET_RANGER_UPPER_CONTROLLER` and re-emits them to Windows as four virtual
32-button joysticks (128 buttons total) plus standard keyboard key
presses, so DCS-BIOS/MSFS/DCS World see ordinary USB game controllers and a
keyboard.

## Program flow

1. **Setup**
   - Starts all 4 virtual joysticks and the virtual keyboard (`Joystick.begin()`
     x4, `Keyboard.begin()`).
   - Brings up Ethernet on the static IP, opens the command-listener, log,
     and keepalive UDP sockets, and logs a boot message.
2. **Main loop** (`loop()`)
   - Sends a `"JOYSTICK"` keepalive every 10s.
   - Reads the single local physical button (pin 5) and reflects its state
     onto virtual Joystick 1, button 0.
   - Checks for an incoming UDP command packet; if present, logs the sender
     and payload, then calls `processCommand()`.
   - `processCommand()` recognises two payload formats:
     - `"<idx>,<state>"` — sets button `idx % 32` on virtual joystick
       `idx / 32` (0–31 → Joystick 1, 32–63 → Joystick 2, 64–95 → Joystick 3,
       96–127 → Joystick 4) to pressed/released.
     - `"KEY,<key>,<state>"` — presses/releases a keyboard key. A single
       character is sent as its ASCII value; longer names (e.g. `ENTER`,
       `F5`, `LCTRL`) are mapped to `Keyboard.h` key codes via
       `resolveKeyCode()`.

## Pin usage

| Pin(s) | Function |
|---|---|
| 5 | Local physical pushbutton → Joystick 1, button 0 |

(This board has no other physical panel wiring of its own — its purpose is
purely to translate network commands into USB HID output.)

## Local network configuration

| Setting | Value |
|---|---|
| Static IP | `172.16.1.11` |
| MAC | `DE:AD:BE:EF:FE:ED` |
| Local port `localPort` | 4210 (listens for button/key commands) |

## Remote endpoints this sketch talks to

| Target | Port | Purpose |
|---|---|---|
| `172.16.1.10` (`logHost`) | 27000 (`logPort`) | Log messages (boot, button/key events, malformed-command warnings) |
| `172.16.1.10` (`logHost`) | 13137 (`aliveport`) | 10s keepalive `"JOYSTICK"` |

## C# programs this sketch communicates with

- None directly — its only IP peer in the current fleet is
  `JET_RANGER_UPPER_CONTROLLER` (an Arduino, not a C# program), which sends
  it button/key commands on port 4210.
- **JetRangerHealthMonitor** — listens on UDP **13137** and lights the
  "JoyStick" indicator green on receipt of this sketch's keepalive.
- No C# project in this repository listens on `172.16.1.10:27000` (the log
  stream has no in-repo consumer).
- Once this board presents its virtual joysticks/keyboard to Windows, any
  DCS-BIOS/DCS World or MSFS control-binding software on that PC can pick
  up the button presses like any other USB game controller — that's an OS
  HID interaction, not an IP one.
