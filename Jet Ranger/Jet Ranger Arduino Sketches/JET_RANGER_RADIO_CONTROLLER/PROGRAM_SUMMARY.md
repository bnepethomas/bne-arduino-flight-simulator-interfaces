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

## Swap switches, key matrix, start-up quiet period

The four frequency-swap switches are on the **Jet Ranger COM and NAV** PCB, in a
diode matrix that all shares column 0 (D26); rows 0-3 are D22-D25. Matrix
position -> action in `handleKeyPress()`:

| Switch (COM and NAV PCB) | Matrix (row, col) | Action |
|---|---|---|
| NAV1 FREQ SWITCH (`SW6`) | 0, 0 | **NAV1 swap**: swaps `Nav1ActiveFrequency`/`Nav1StandbyFrequency` on the NAV OLED immediately (the PC does not send NAV frequencies back, unlike COM) and sends `NAV1_RADIO_SWAP` to 172.16.1.10:27001 |
| NAV2 FREQ SWITCH (`SW7`) | 1, 0 | **NAV2 swap**: same with NAV2, sends `NAV2_RADIO_SWAP` |
| COMM1 Freq Switch (`SW4`) | 2, 0 | COM1 swap: sends `COM1_RADIO_SWAP` (the display changes when the sim sends the swapped frequencies back) |
| COMM2 FREQ SWITCH (`SW5`) | 3, 0 | COM2 swap: sends `COM2_RADIO_SWAP`, then `AVIONICS_MASTER_SET` |

(The earlier mapping had battery on/off and alternator on/off on (0,0)/(1,0);
it was removed.) Both bridges (`FSUIPCWinformsAutoCS` and `MSFSWinformsAutoCS`)
handle `NAV1_RADIO_SWAP` and `NAV2_RADIO_SWAP`, and any command a bridge does not
recognise falls into its COM1 standby-frequency parser (resets COM1 standby to 121.50).

**Bench-verified 2026-10-15** (Mega on COM20, through the reflector's forwarded
copy): each of the four switches pressed twice logged `Key Pressed`, the matching
`COM1 Swap`/`COM2 Swap`/`NAV1 Swap`/`NAV2 Swap` line and `Key Released`.

- **Phantom key presses fixed:** a floating column used to read LOW as soon as
  scanning started, so every boot reported key presses and sent their commands
  to the PC. For `matrixQuietMs` (1500 ms) after `initMatrixScanner()` the scanner
  now only records the state and raises no press or release events
  (`matrixQuietUntil`).
- **Matrix diagnostic:** UDP control packet `C,MATRIX:1` sends the raw readings
  (`H`/`L` per column, bypassing debounce and the quiet period) as
  `Matrix raw idle ...` and `Matrix raw row N LOW ...` lines. It showed column 0
  stuck LOW on the original Mega even with the ribbon unplugged (a faulty or badly
  seated Mega/pin D26 - all four swap switches use column 0, so none could register);
  on the replacement Mega all four columns read `H`. A badly seated Mega also made
  the board drop off USB and the network after flashing.
- Encoder knobs now name their radio when they send a standby frequency to the PC
  (`sendStandbyToPC()`, port 27001): COM1 stays the bare number
  (`118.295`, which every bridge understands); COM2, NAV1 and NAV2 are sent as
  `COM2_STANDBY:118.295`, `NAV1_STANDBY:108.000`, `NAV2_STANDBY:108.000`, which
  `FSUIPCWinformsAutoCS` routes to the matching radio. Previously every knob sent a
  bare number, the PC took anything outside 118-136.975 as COM1 standby 121.50, so
  NAV and COM2 knobs retuned COM1. Nothing is sent during the start-up quiet period,
  so the encoders' power-up positions are no longer pushed to the simulator. Each send
  is logged as `To PC: ...`. Both bridges handle these labelled forms.
- Display column fix (2026-10-16): the bridges send COM frequencies with 2 decimals
  (`128.30`, `127.80`), and the display line joined COM1 and COM2 with two spaces, so a
  short COM1 pulled COM2 one column left and its hundreds digit went behind the
  faceplate. `padFrequency()` now pads every frequency to `xxx.xxx` (7 characters)
  before `updateCOMM()` / `updateNAV()` build their lines (7 + 2 + 7 = 16 columns).
  Confirmed by eye on the bench.
- Standard channel spacing (2026-10-16): the minor knobs used to step 5 kHz on all four
  radios and the COM major knob stopped at 135. Now COM runs 118.000-136.975 MHz in
  25 kHz channels (`COM_MINOR_STEP_KHZ`, major 118..136) and NAV runs 108.00-117.95 MHz
  in 50 kHz steps (`NAV_MINOR_STEP_KHZ`, major 108..117). `CommMinorEncoderMaxValue` and
  `NavMinorEncoderMaxValue` are derived from the step sizes, so changing a constant also
  moves the wrap-around limits. 8.33 kHz channels are not offered. Flight sims store COM
  frequencies in FSUIPC's 10 kHz BCD form, so a 25 kHz channel such as 118.025 may show as
  118.02 or 118.03 in the sim.
- Compiled at 25,774 bytes flash (10%) / 3,323 bytes RAM (40%), flashed on COM20.

## No-data timeout (30 s)

If no UDP packet - data `D,...` or control `C,...` - arrives on the MSFS port
(13136) for `noDataTimeoutMs` (30 s), the COM and NAV character OLEDs are
blanked once: the loop sets `powerAvailable = false` (the state the displays
already treat as "no power", where `updateCOMM()`/`updateNAV()` clear the
screen) and logs `no UDP data for 30s, displays blanked`. The timer starts at
the end of `setup()` (`lastMSFSDataMillis = millis()`). When a packet arrives
again the previous power state is restored, both displays are redrawn and
`UDP data resumed - displays restored` is logged. **Local input also counts:**
any encoder movement or key press calls `wakeDisplays()`, which restarts the 30 s
and, if the displays are blanked, restores them (`Local input - displays
restored`); they blank again (`no UDP data or local input for 30s, displays
blanked`) only after 30 s with neither UDP data nor local input. Bench-verified:
the first key press after a blank restored the displays. Control packets such as
`C,KEEPALIVE:1` also count as data. Bench-tested on the Radio (COM19) through
the UDP reflector's forwarded copy: blanked about 30 s after boot, restored on
the next `D,C1A:...` packet. Compiled at 24,440 bytes flash (9%) / 3,141 bytes
RAM (38%).

## Auto Pilot and Audio panel switches and knobs

The matrix is now 4 rows (D22-D25) by 5 columns (D26, D27, D28, D29 and **D48**): `COL0` is the COM and NAV swap switches, `COL1`/`COL2` the Auto Pilot panel and
`COL3`/`COL4` the Audio panel (one switch per row on each column, diode to the column). The five Auto Pilot knobs are polling encoders on CRS D11/D12, ALT A4/A5, VS D46/D47,
IAS D7/D6 and HDG D8/D9, read like the ADF knobs (centred, recentred after each read, three counts per click, acceleration on). Every switch and click goes out twice:
a joystick button to the USB HID board (172.16.1.11:4210, `index,state`; Joystick 3 = indexes 64-81 for the Auto Pilot panel, Joystick 4 = 96-103 for the Audio panel; a knob
click is a 40 ms press released from `serviceHidPulses()`), and a simulator control name to the bridge on UDP 27001 (switch: on press; knob: one per click with a `:N` repeat
count). Nothing is sent during the 1.5 s quiet period. `panelSwitches[]` and `panelEncoders[]` hold the tables; each press is logged (`AP HDG pressed (joystick button index 65)`).
Compiled at 30,108 bytes flash (11%) / 4,823 bytes RAM (58%); not flashed or tested on the board yet.

| Switch / knob | Row, col or pins | Index | Command |
|---|---|---|---|
| AP ENGAGE, HDG, CRS, ALT | (0,1) (1,1) (2,1) (3,1) | 64 to 67 | `AP_MASTER`, `AP_HDG_HOLD`, `AP_NAV1_HOLD`, `AP_ALT_HOLD` |
| AP APPR, FD, BC, IAS | (0,2) (1,2) (2,2) (3,2) | 68 to 71 | `AP_APR_HOLD`, `TOGGLE_FLIGHT_DIRECTOR`, `AP_BC_HOLD`, `AP_PANEL_SPEED_HOLD` |
| CRS, ALT, VS, IAS, HDG knobs (cw, ccw) | D11/12, A4/A5, D46/47, D7/6, D8/9 | 72 to 81 | `VOR1_OBI_INC/DEC`, `AP_ALT_VAR_INC/DEC`, `AP_VS_VAR_INC/DEC`, `AP_SPD_VAR_INC/DEC`, `HEADING_BUG_INC/DEC` |
| Audio COM1, BOTH, NAV2, DME | (0,3) (1,3) (2,3) (3,3) | 96 to 99 | `COM1_TRANSMIT_SELECT`, `COM_RECEIVE_ALL_TOGGLE`, `RADIO_VOR2_IDENT_TOGGLE`, `RADIO_DME1_IDENT_TOGGLE` |
| Audio COM2, NAV1, MKR, ADF | (0,4) (1,4) (2,4) (3,4) | 100 to 103 | `COM2_TRANSMIT_SELECT`, `RADIO_VOR1_IDENT_TOGGLE`, `MARKER_SOUND_TOGGLE`, `RADIO_ADF_IDENT_TOGGLE` |

## Auto Pilot and Audio panel LEDs (MAX7219)

One MAX7219 on the Radio Auto Pilot PCB drives 19 LEDs: DIN = A15, CLK = A14, LOAD = A13 (LedControl library, one device, scan limit 3, intensity 8 until a `C,B`
arrives). Segments A to G and DP are `ANNODE_00` to `ANNODE_07`, digits 0 to 7 are `CATHODE_00` to `CATHODE_07`; a panel LED sits between one segment and one
digit. LedControl `setLed(0, digit, column)` uses column 0 for DP and 1 to 7 for A to G.

| LED | Token | Digit | Segment | | LED | Token | Digit | Segment |
|---|---|---|---|---|---|---|---|---|
| CRS | `LED_CRS` | 0 | A | | COM1 | `LED_COM1` | 2 | A |
| HDG | `LED_HDG` | 0 | B | | BOTH | `LED_BOTH` | 2 | B |
| IAS | `LED_IAS` | 0 | C | | NAV2 | `LED_NAV2` | 2 | C |
| ALT | `LED_ALT` | 0 | D | | DME | `LED_DME` | 2 | D |
| APPR | `LED_APPR` | 1 | A | | OUTER | `LED_OUTER` | 2 | E |
| FD | `LED_FD` | 1 | B | | INNER | `LED_INNER` | 2 | F |
| BC | `LED_BC` | 1 | C | | COM2 | `LED_COM2` | 3 | A |
| AP ENGAGE | `LED_APENG` | 1 | D | | NAV1 | `LED_NAV1` | 3 | B |
| | | | | | MKR | `LED_MKR` | 3 | C |
| | | | | | ADF | `LED_ADF` | 3 | D |
| | | | | | MIDDLE | `LED_MID` | 3 | E |

The first two columns are the Auto Pilot panel (digits 0 and 1), the others the Audio panel (digits 2 and 3). The bridges send each LED as `LED_xxx:0/1` in the radio
data packet (see the FSUIPC and MSFS bridges). `HandleOutputValuePair()` handles them before its per-pair debug line. The LEDs are lit only while `powerAvailable`
is true (NAVCOM1:1 and not blanked by the no-data timeout), and `applyPanelLeds()` writes only the LEDs that changed. `C,LEDTEST:1` lights all 19, `C,LEDTEST:0`
returns them to the simulator state. **`C,B:0-15`** sets the MAX7219 intensity (0 shuts the display down, 1 to 15 set the intensity). Compiled at 27,610 bytes
flash (10%) / 3,724 bytes RAM (45%); not flashed or tested on the board yet.

## Remote reset over UDP

UDP control packet `C,RESET:1` (value ignored) to port 13136 reboots this
board with a **software reset**: `RebootBoard()` sends a debug line (`RESET
command received - rebooting`), waits 100ms, disables interrupts, switches off
the peripherals that would survive a jump (external/pin-change interrupts, all
timer interrupts, TWI, SPI, ADC, UARTs) and jumps to the reset vector
(`jmp 0`); `setup()` then runs again and resets the Ethernet module through
`ES1_RESET_PIN`. It does not use the AVR watchdog (used on the Servo, Stepper
and Dual Stepper boards) because that **hung the Radio Controller**: after a
watchdog reset the chip's watchdog restarts at its shortest timeout and the
board's bootloader does not switch it off, so it reset over and over and never
reached the sketch until it was reflashed. A `.init3` function
(`DisableWatchdogAtStartup()`) still clears `MCUSR`/the watchdog first thing
after every reset. Handled in `HandleControlString()` (the existing `C,...` control-packet handler, port 13136). **Bench-tested (2026-10-14, COM19):** acknowledgement, then "Ethernet Started" about 3.9s later and "Setup Complete" about 5s after the command, with no loop. Compiled at 23,944 bytes flash (9%) / 3,060 bytes RAM (37%).

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

## Ethernet retransmission timeout

`Ethernet.setRetransmissionTimeout(10)` is called right after `Ethernet.begin()`
(same as the Stepper and Dual Stepper sketches). The W5500's default is 200ms x
8 retries - up to 1600ms blocked inside a UDP send that gets no ARP reply (for
example when the reflector PC is off) - so one dead destination could stall
the encoder and button-matrix scanning. With 10ms per retry the worst case is about 80ms. Compiled at 24,472 bytes flash (9%);
not yet flashed.

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

- **FSUIPCWinformsAutoCS** / **MSFSWinformsAutoCS** — the bridge apps
  (one at a time), normally run on the PC at `172.16.1.10`. They listen on
  UDP **27001** for the control commands this sketch sends (radio swap,
  standby tuning, battery, alternator) and translate them into simulator
  events, and they send the `D,C1A:...` frequency/status packets this
  sketch listens for on port **13136**.
- **JetRangerHealthMonitor** — listens on UDP **13137** on the same PC and
  turns the "CommNav" indicator green when this sketch's keepalive arrives
  (red/orange if it's been 15–30s+ since the last one).
- No C# project in this repository currently listens on `172.16.1.10:27000`
  — the debug log stream (`SendDebug`) has no in-repo consumer; it would
  need to be watched with a generic UDP listener/packet capture.
