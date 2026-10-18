# Radio controller (172.16.1.101)

`JET_RANGER_RADIO_CONTROLLER` runs on an Arduino Mega 2560 with a W5500 Ethernet module. It shows the COM and NAV frequencies on two character OLEDs, reads eight rotary encoders (COM1, COM2, NAV1 and NAV2, each with a major and a minor knob) and a 4 by 4 button matrix, and keeps the radio panel in step with the simulator.

![Radio controller blocks](diagrams/radio_blocks.svg)

## Program flow

1. **Setup:** reset the W5500 through pin 53, bring up Ethernet and open the UDP sockets, flash the red LED for 3 seconds while the link settles, send an "Ethernet Started" debug line, start the two OLEDs through the TCA9548A I2C multiplexer, start the eight encoders (with acceleration) and the matrix scanner.
2. **Main loop:**
   - toggle the green and red status LEDs every 200 ms;
   - every 10 seconds send the `COMM_NAV` heartbeat;
   - poll the eight encoders; a change recalculates the standby frequency, sends it to the PC and redraws the OLEDs;
   - scan the matrix and act on key presses (below);
   - read any packet on port 13136 and update the active and standby frequencies, the main bus voltage and the power state.

![Where each input and output goes](diagrams/radio_routing.svg)

## Pin usage

| Pin | Function |
|---|---|
| 14 | Green status LED |
| 15 | Red status LED (flashes while the Ethernet link comes up) |
| 53 | W5500 manual reset |
| 50 to 52 (and 10) | SPI to the W5500 |
| SDA, SCL | TCA9548A mux (0x70): port 0 NAV OLED, port 1 COM OLED, both at 0x3C |
| 31, 30 | COM1 major encoder (A, B) |
| 33, 32 | COM1 minor encoder |
| 34, 35 | COM2 major encoder |
| 37, 36 | COM2 minor encoder |
| 38, 39 | NAV1 major encoder |
| 41, 40 | NAV1 minor encoder |
| 42, 43 | NAV2 major encoder |
| 45, 44 | NAV2 minor encoder |
| 22 to 25 | Matrix rows |
| 26 to 29 | Matrix columns |

## Data from the PC (UDP 13136)

| Token | Meaning | Example |
|---|---|---|
| `C1A`, `C1S` | COM1 active and standby frequency | `C1A:119.500` |
| `C2A`, `C2S` | COM2 active and standby frequency | `C2S:121.900` |
| `MAINBUS` | Main bus voltage | `MAINBUS:27.8` |
| `NAVCOM1` | Radio power: 1 = on, 0 = COM and NAV displays blank | `NAVCOM1:1` |
| `C,B:n` | Display brightness (control packet) | `C,B:5` |

The bridges send COM frequencies with two decimals; the radio pads every frequency to `xxx.xxx` (7 characters) so that the 16-column line (7 + 2 + 7) never slides COM2 or NAV2 behind the faceplate. The PC does not send NAV frequencies back.

## Commands to the PC (UDP 27001)

Turning a knob changes the standby frequency on the OLED and sends it to the PC. COM1 is sent as the bare number (for example `118.295`); the others carry the radio's name: `COM2_STANDBY:118.295`, `NAV1_STANDBY:108.000`, `NAV2_STANDBY:108.000`. Nothing is sent during the first 1.5 seconds after power-up, so the knobs' start positions do not change the simulator.

The four frequency-swap switches are on the Jet Ranger COM and NAV PCB in a diode matrix that shares column 0 (D26); the rows are D22 to D25.

| Switch | Matrix (row, col) | Action |
|---|---|---|
| NAV1 FREQ (SW6) | 0, 0 | Swaps NAV1 active and standby on the NAV OLED at once (the PC sends no NAV frequencies back) and sends `NAV1_RADIO_SWAP` |
| NAV2 FREQ (SW7) | 1, 0 | The same for NAV2, sends `NAV2_RADIO_SWAP` |
| COMM1 (SW4) | 2, 0 | Sends `COM1_RADIO_SWAP`; the display changes when the simulator returns the swapped frequencies |
| COMM2 (SW5) | 3, 0 | Sends `COM2_RADIO_SWAP`, then `AVIONICS_MASTER_SET` |

Other matrix keys are only logged to the debug listener.

## Tuning steps

The knobs follow standard channel spacing. COM runs 118.000 to 136.975 MHz in 25 kHz channels (major knob 118 to 136; minor knob 000, 025, 050, 075). NAV runs 108.00 to 117.95 MHz in 50 kHz steps (major 108 to 117). Each minor knob wraps round at its ends. The constants `COM_MINOR_STEP_KHZ` and `NAV_MINOR_STEP_KHZ` set the steps, and the wrap-around limits follow from them. 8.33 kHz channels are not offered. FSUIPC holds COM frequencies to 10 kHz, so in the simulator a channel such as 118.025 may appear as 118.02 or 118.03.

## Power and blanking

- **Avionics power:** when the avionics bus is below 24 V the bridge sends `NAVCOM1:0` and the COM and NAV displays blank together; they light together again when the bus rises.
- **No data for 30 seconds:** if no packet arrives and no knob or key is used, both OLEDs blank once. A packet, a knob movement or a key press restores them, and they blank again after another 30 seconds of silence.
- **Quiet period:** for 1.5 seconds (`matrixQuietMs`) after the matrix scanner starts it records state but raises no events, so a floating input cannot send false button presses at boot.

## Diagnostics

- The control packet `C,MATRIX:1` prints the raw matrix (H or L for each column, with all rows high and then each row driven low) to the debug stream, bypassing debounce and the quiet period. A column that reads L with nothing pressed is a hardware fault on that Mega pin or board. A faulty or badly seated Mega did exactly this on the bench: all four swap switches share column 0, so none could register, and the board also dropped off USB and the network after flashing.
- Remote reset is a software reset (see the network chapter). Bench-tested: "Ethernet Started" about 4 seconds after the command and "Setup Complete" about 5 seconds.

## PCB features the sketch does not use yet

The Jet Ranger Radio Auto Pilot PCB (v1.3) carries more than the sketch handles. All of this is wired on the PCB:

| Mega pins | PCB net | Status in the sketch |
|---|---|---|
| D5 | `BACKLIGHT_PWM`: 330 ohm to an IRF540N MOSFET that switches the AUDIO, AUTOPILOT and COMM_NAV panel backlights | Not driven. `C,B:n` is parsed but has no effect |
| A0 to A3 | `COMM1_VOL`, `COMM2_VOL`, `NAV1_VOL`, `NAV2_VOL` (volume pots) | Not read |
| A4, A5 | `ALT_A`, `ALT_B` (autopilot altitude knob) | Not read |
| D6, D7 | `IAS_B`, `IAS_A` (autopilot airspeed knob) | Not read |
| D8, D9 | `HDG_A`, `HDG_B` (heading knob) | Not read |
| D11, D12 | `CRS_A`, `CRS_B` (course knob) | Not read |
| D46, D47 | `VS_A`, `VS_B` (vertical speed knob) | Not read |
| A13, A14, A15 | `MAX_7219-LOAD`, `-CLOCK`, `-DIN` (autopilot display drivers) | Not driven |
| D48 | `COL4` (a fifth matrix column for the Audio and Auto Pilot panel switches) | Not scanned (the sketch scans columns 0 to 3) |
| D2 to D4 | `I2C_SEL_A2`, `A1`, `A0` (mux address) | Not driven |

On the same PCB the COM1 major, COM1 minor, COM2 minor and NAV1 minor encoders have their A and B pins the other way round from the sketch (the sketch uses 31, 30 and 33, 32 for COM1, 37, 36 for COM2 minor and 41, 40 for NAV1 minor, where the PCB labels A as 30, 32, 36 and 40). It only matters if one of those knobs tunes backwards.

## Known issues

- Only COM1 and COM2 frequencies arrive from the PC; there are no NAV frequency tokens yet.
- The Radio controller emulator sends a bare frequency for every radio (see the emulator chapter).
- The mux address pins (D2 to D4) are not driven by the sketch; the mux module's own pull-downs set address 0x70.
