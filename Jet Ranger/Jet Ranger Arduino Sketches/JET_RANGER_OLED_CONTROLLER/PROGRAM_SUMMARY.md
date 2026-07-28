# JET_RANGER_OLED_CONTROLLER — Program Summary

> **Note:** despite the folder name, this sketch's own header comments
> identify it as **`A10_FRONT_CONSOLE_OLED`** for the **left console of an
> A-10C Warthog** DCS World pit — not the Jet Ranger helicopter. Its data
> source is **DCS-BIOS** (serial), not the Jet Ranger's `172.16.1.x`
> UDP/CSV protocol used by the other sketches in this folder. It appears to
> be a different cockpit-build project's code that has ended up filed
> under "Jet Ranger Arduino Sketches" — worth confirming with whoever
> placed it here whether that's intentional.

Arduino Mega 2560 driving 6 small OLED displays (via a TCA9548A I2C
multiplexer) plus 8 discrete panel LEDs for an A-10C/Hornet-style front
console: a barometric-pressure readout, a large digital altimeter, a CMSC
(countermeasures) status line, and landing-gear/VHF/AOA-indexer lamps.

## Program flow

1. **Setup**
   - Flashes the red/green status LEDs briefly, then (if `Ethernet_In_Use`)
     resets the W5500 shield, brings up Ethernet on the static IP, and
     opens the debug UDP socket, flashing the green LED while the link
     settles.
   - Sets the 8 discrete LED pins to `OUTPUT` and turns them off, then
     waits 500ms and starts the I2C bus (`Wire.begin()`).
   - Initialises the character OLED on the CMSC mux port (`initCharOLED()`,
     the same direct-command-byte protocol used by
     `JET_RANGER_RADIO_CONTROLLER`'s OLEDs) and shows a "Character OLED /
     TEST" splash, then calls `updateCMSC()` once.
   - Scans all 8 TCA9548A mux ports for any I2C device present (0–127),
     logging each hit via `SendDebug()` — a bus-troubleshooting aid.
   - Initialises the BARO and ALT `U8g2lib` OLEDs, shows a placeholder
     value on each, waits 3s, then turns all 8 discrete LEDs on and starts
     DCS-BIOS (`DcsBios::setup()`).
2. **Main loop** (`loop()`)
   - Toggles the green/red status LEDs every `FLASH_TIME` (300ms).
   - Pumps `DcsBios::loop()` — all instrument updates in this sketch are
     driven by DCS-BIOS callback buffers (below), not by anything received
     over Ethernet.
   - If `PressureChanged` is set (by one of the barometer-digit callbacks,
     or by a UDP `BARO` update — see below), calls `ProcessPressureChange()`,
     which recomputes the altimeter's baro-correction delta from the
     4-digit pressure setting and refreshes both the BARO display and the
     altimeter digits.
   - Every `incomingcheckinterval` (5ms), checks for an incoming UDP packet
     on `MSFSport` and, if present, passes it to
     `ProcessReceivedMSFSString()` — see below.
3. **DCS-BIOS callbacks** (registered as `DcsBios::IntegerBuffer` /
   `StringBuffer` instances, fire whenever the corresponding DCS aircraft
   value changes):
   - `altMslFtBuffer` (A-10C altitude, address `0x0434`) → redraws the
     rolling ten-thousands/thousands/hundreds altimeter digits
     (`UpdateAltimeterDigits`), with different digit layout depending on
     whether `sAircraftName == "A-10C"` or not (Hornet uses a 2-digit
     layout).
   - Two parallel families of 4 barometer-digit callbacks — one set for
     the Hornet's standby-altimeter pressure knob addresses
     (`0x74fa`/`0x74fc`/`0x74fe`), one for the A-10C's altimeter pressure
     addresses (`0x1086`/`0x1088`/`0x108a`/`0x108c`) — both just update the
     same `iBaroOnes/Tens/Hundreds/Thousands` digits and set
     `PressureChanged`.
   - `AcftNameBuffer` (address `0x0000`, the aircraft-name string DCS-BIOS
     always exports) → sets `sAircraftName`, used to branch the altimeter
     layout and baro defaults above.
   - `cmscTxtChaffFlareBuffer` / `cmscTxtJmrBuffer` / `cmscTxtMwsBuffer`
     (addresses `0x108e`/`0x1096`/`0x133c`) → update the 2-line CMSC
     character-OLED text.
   - `gearLSafeBuffer` / `gearNSafeBuffer` / `gearRSafeBuffer` (all address
     `0x1026`, different bitmasks) → drive the 3 landing-gear-safe LEDs.
   - `aoaIndexerHighBuffer` / `NormalBuffer` / `LowBuffer` (address
     `0x1012`, different bitmasks) → drive the 3 AOA-indexer LEDs.
4. **UDP data receiver** (added to let this board also be driven the same
   way as `JET_RANGER_SERVO_CONTROLLER`, alongside its existing DCS-BIOS
   path): `ProcessReceivedMSFSString()` parses the same
   `"D,CODE:value,CODE:value,..."` CSV payload as the Servo Controller
   (`HandleOutputValuePair`/`HandleControlString`/`getValue` are near-verbatim
   ports of that sketch's versions). Only two codes are wired up for now:
   - `ALT` → `onAltMslFtChange(value)`, reusing the exact altitude-digit
     redraw logic this sketch's own DCS-BIOS altitude callback already
     uses. Unit-correct as-is, since the PC bridge apps send `ALT` as raw,
     unconverted feet.
   - `BARO` → sets `iBaroThousands/Hundreds/Tens/Ones` (and the matching
     `BaroThousands/Hundreds/Tens/Ones` strings) from a single 4-digit
     inHg×100 value (e.g. `2992` = 29.92 inHg), then sets `PressureChanged`
     so the existing `ProcessPressureChange()` call in `loop()` picks it
     up and redraws both the BARO and ALT displays. This is the same final
     representation the DCS-BIOS pressure-digit callbacks build one digit
     at a time — over UDP, all 4 digits arrive in a single value instead.
   - Every other code in the payload (e.g. `IAS`, `TQ`, `RPMR`, the
     warning-lamp bits, etc.) is currently parsed and silently ignored.

## Pin usage

| Pin(s) | Function |
|---|---|
| 12 | Red status LED (`RED_STATUS_LED_PORT`/`Check_LED_R`) |
| 13 | Green status LED (`GREEN_STATUS_LED_PORT`/`Check_LED_G`) |
| 53 | W5500 Ethernet shield manual reset (`ES1_RESET_PIN`) |
| I2C (SDA/SCL) | TCA9548A I2C multiplexer (addr `0x70`) driving: mux port 1 = BARO OLED, port 2 = ALT OLED, port 3 = CMSC character OLED (addr `0x3C`); ports 0/2 also reserved for COM1/COM2 OLEDs (`u8g2_COM1`/`u8g2_COM2` are declared but never initialised/updated in this sketch); ports 3–7 reserved for "Opt OLED" 1–5 (declared, unused) |
| 2 | Reset pin for `u8g2_Scratch_Pad` OLED wiring (declared, unused — no scratch-pad OLED is actually driven) |
| 2 / 11 | Reset pins wired into the unused `u8g2_COM1`/`u8g2_COM2` OLED constructors |
| 2 | Right landing-gear-safe LED (`O_RIGHT_GEAR_LED`) — **shares the same pin number as the OLED reset pins above**; only one of these physical uses can be correct on the real board |
| 3 | Nose landing-gear-safe LED (`O_NOSE_GEAR_LED`) |
| 4 | Left landing-gear-safe LED (`O_LEFT_GEAR_LED`) |
| 5 | VHF-AM lamp (`O_VHF_AM_LED`) |
| 6 | VHF-FM lamp (`O_VHF_FM_LED`) |
| 7 | AOA "above" indexer lamp (`O_AOA_ABOVE_LED`) |
| 8 | AOA "on" indexer lamp (`O_AOA_ON_LED`) |
| 9 | AOA "below" indexer lamp (`O_AOA_BELOW_LED`) |
| Serial (USB) | DCS-BIOS `DCSBIOS_IRQ_SERIAL` link to the DCS World PC — the actual data source for every instrument this board drives |

> **Pin overlap caveat:** pin `2` is used three different ways in this
> sketch's `#define`s — as the BARO/ALT/Scratch-Pad reset pin argument, as
> the COM1 OLED reset pin, and as `O_RIGHT_GEAR_LED`. Since most of those
> OLED objects/pins are never actually initialised or written to, this
> likely isn't a live conflict in practice, but it's worth checking the
> real board wiring before repurposing any of the unused OLED code paths.

## Local network configuration

| Setting | Value |
|---|---|
| Static IP | `172.16.1.104` |
| MAC | `A8:61:0A:67:83:68` |
| Local port `localport` | 7788 (bound, source socket for outbound debug packets) |
| Local port `MSFSport` | **13136** — listens for `D,ALT:...,BARO:...` front-panel data packets, matching `JET_RANGER_SERVO_CONTROLLER`'s port exactly |

## Remote endpoints this sketch talks to

| Target | Port | Purpose |
|---|---|---|
| `172.16.1.10` (reflector host) | 27000 | Debug/log messages (`SendDebug`) — I2C bus-scan results, Ethernet-started message |
| `172.16.1.2` (`targetIP`) | — | Declared (`remoteport` 26027) but never sent to anywhere in this sketch |

## C# / other programs this sketch communicates with

- No C# project in this repository listens on `172.16.1.10:27000` — the
  debug log stream has no in-repo consumer, same gap noted for the other
  Jet Ranger boards.
- Its instrument data now comes from **two** independent sources: DCS-BIOS
  over serial/USB (unchanged), and, new, UDP from
  **[FSUIPCWinformsAutoCS](../../%20C%23%20Code/FSUIPCWinformsAutoCS/PROGRAM_SUMMARY.md)**,
  which sends the same shared front-panel payload it sends to the Servo
  and Stepper Controllers to `172.16.1.104:13136` — this board just reads
  the `ALT`/`BARO` fields out of it and ignores the rest.
- The other SimConnect-based bridge apps (`P3D_to_UDP`, `SimConnect_to_UDP`,
  `MSFSSimConnectExtractor`) do **not** currently send to this board's IP —
  only `FSUIPCWinformsAutoCS` has been updated to do so.

## Unused files in this folder

`PTFont.h`, `dseg14_v3.h`, `er_oled.h`, and `er_oled.cpp` are present in
this sketch folder but are not `#include`d anywhere in
`JET_RANGER_OLED_CONTROLLER.ino` — they look like an earlier, since-replaced
OLED driver approach (the active code uses `U8g2lib` instead, via
`hornet_font.h`, which *is* included). They can likely be removed if
nothing else in the build depends on them, but that's worth confirming
before deleting.
