# Radio controller (172.16.1.101)

`JET_RANGER_RADIO_CONTROLLER` runs on an Arduino Mega 2560 with a W5500 Ethernet module. It shows the COM and NAV frequencies on two character OLEDs, reads eight rotary encoders (COM1, COM2, NAV1 and NAV2, each with a major and a minor knob) and a 4 by 5 button matrix, and keeps the radio panel in step with the simulator. It also serves the **Auto Pilot and Audio panels**: it reads their switches and the Auto Pilot's five knobs (sent as joystick buttons and as simulator controls) and drives their 19 LEDs through a MAX7219.

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
| 22 to 25 | Matrix rows (`ROW0` to `ROW3`) |
| 26, 27, 28, 29, 48 | Matrix columns (`COL0` COM and NAV panel, `COL1` and `COL2` Auto Pilot panel, `COL3` and `COL4` Audio panel) |
| 11, 12 | Auto Pilot CRS knob (`CRS_A`, `CRS_B`) |
| A4, A5 | Auto Pilot ALT knob (`ALT_A`, `ALT_B`) |
| 46, 47 | Auto Pilot VS knob (`VS_A`, `VS_B`) |
| 7, 6 | Auto Pilot IAS knob (`IAS_A`, `IAS_B`) |
| 8, 9 | Auto Pilot HDG knob (`HDG_A`, `HDG_B`) |
| A15, A14, A13 | MAX7219 `DIN`, `CLK` and `LOAD` (the panel LEDs) |

## Data from the PC (UDP 13136)

| Token | Meaning | Example |
|---|---|---|
| `C1A`, `C1S` | COM1 active and standby frequency | `C1A:119.500` |
| `C2A`, `C2S` | COM2 active and standby frequency | `C2S:121.900` |
| `MAINBUS` | Main bus voltage | `MAINBUS:27.8` |
| `NAVCOM1` | Radio power: 1 = on, 0 = COM and NAV displays blank and the panel LEDs off | `NAVCOM1:1` |
| `LED_xxx` | One of the 19 Auto Pilot and Audio panel LEDs (see below): 1 = on, 0 = off | `LED_HDG:1` |
| `C,B:n` | Brightness 0 to 15 (control packet): the MAX7219 intensity of the panel LEDs, 0 = LEDs off | `C,B:5` |
| `C,LEDTEST:n` | Control packet: 1 lights all 19 panel LEDs, 0 returns them to the simulator's state | `C,LEDTEST:1` |

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

Keys in columns 1 to 4 are the Auto Pilot and Audio panel switches (next section); any other key is only logged to the debug listener.

## Auto Pilot and Audio panel switches and knobs

The Jet Ranger Auto Pilot Panel and Audio Panel PCBs plug into the Radio Auto Pilot controller. Each has eight switches in a diode matrix (row to the switch, diode to the column), and the Auto Pilot panel also has five rotary encoders wired straight to the Mega. Every switch and knob is sent **two ways**:

1. as a **joystick button** on the USB HID board (UDP `172.16.1.11:4210`, `index,state`), in the buttons nothing else uses: **Joystick 3** (indexes 64 to 95) for the Auto Pilot panel and **Joystick 4** (96 to 127) for the Audio and ADF panels. A switch holds its button down while it is pressed. A knob click is sent as a 40 ms press (the `HID_PULSE_MS` release goes out from the main loop), clockwise on one button and anticlockwise on the next;
2. as a **command to the PC bridge** (UDP 27001), the name of the simulator control, which the FSUIPC and MSFS bridges recognise (see their chapters). A switch sends its command when pressed; a knob sends one command for each click, with a `:N` repeat count when it is turned quickly (`HEADING_BUG_INC:5`).

Nothing is sent during the first 1.5 seconds after power-up (the quiet period). Windows numbers joystick buttons from 1, so index 64 is Joystick 3 button 1. These indexes correspond to Upper controller matrix positions that are wired but have no switch on them (the Upper's assigned switches are all below index 40).

| Panel | Control | Matrix or pin | Joystick button (index) | Command to the bridge (UDP 27001) |
|---|---|---|---|---|
| Auto Pilot | AP ENGAGE | row 0, col 1 | Joystick 3, button 1 (64) | `AP_MASTER` |
| Auto Pilot | HDG | row 1, col 1 | Joystick 3, button 2 (65) | `AP_HDG_HOLD` |
| Auto Pilot | CRS | row 2, col 1 | Joystick 3, button 3 (66) | `AP_NAV1_HOLD` |
| Auto Pilot | ALT | row 3, col 1 | Joystick 3, button 4 (67) | `AP_ALT_HOLD` |
| Auto Pilot | APPR | row 0, col 2 | Joystick 3, button 5 (68) | `AP_APR_HOLD` |
| Auto Pilot | FD | row 1, col 2 | Joystick 3, button 6 (69) | `TOGGLE_FLIGHT_DIRECTOR` |
| Auto Pilot | BC | row 2, col 2 | Joystick 3, button 7 (70) | `AP_BC_HOLD` |
| Auto Pilot | IAS | row 3, col 2 | Joystick 3, button 8 (71) | `AP_PANEL_SPEED_HOLD` |
| Auto Pilot | CRS knob clockwise, anticlockwise | D11, D12 | Joystick 3, buttons 9, 10 (72, 73) | `VOR1_OBI_INC`, `VOR1_OBI_DEC` |
| Auto Pilot | ALT knob | A4, A5 | Joystick 3, buttons 11, 12 (74, 75) | `AP_ALT_VAR_INC`, `AP_ALT_VAR_DEC` |
| Auto Pilot | VS knob | D46, D47 | Joystick 3, buttons 13, 14 (76, 77) | `AP_VS_VAR_INC`, `AP_VS_VAR_DEC` |
| Auto Pilot | IAS knob | D7, D6 | Joystick 3, buttons 15, 16 (78, 79) | `AP_SPD_VAR_INC`, `AP_SPD_VAR_DEC` |
| Auto Pilot | HDG knob | D8, D9 | Joystick 3, buttons 17, 18 (80, 81) | `HEADING_BUG_INC`, `HEADING_BUG_DEC` |
| Audio | COM1 | row 0, col 3 | Joystick 4, button 1 (96) | `COM1_TRANSMIT_SELECT` |
| Audio | BOTH | row 1, col 3 | Joystick 4, button 2 (97) | `COM_RECEIVE_ALL_TOGGLE` |
| Audio | NAV2 | row 2, col 3 | Joystick 4, button 3 (98) | `RADIO_VOR2_IDENT_TOGGLE` |
| Audio | DME | row 3, col 3 | Joystick 4, button 4 (99) | `RADIO_DME1_IDENT_TOGGLE` |
| Audio | COM2 | row 0, col 4 | Joystick 4, button 5 (100) | `COM2_TRANSMIT_SELECT` |
| Audio | NAV1 | row 1, col 4 | Joystick 4, button 6 (101) | `RADIO_VOR1_IDENT_TOGGLE` |
| Audio | MKR | row 2, col 4 | Joystick 4, button 7 (102) | `MARKER_SOUND_TOGGLE` |
| Audio | ADF | row 3, col 4 | Joystick 4, button 8 (103) | `RADIO_ADF_IDENT_TOGGLE` |
| ADF | Landing gear switch | ADF controller D29 | Joystick 4, button 9 (104) | `GEAR_DOWN` when closed, `GEAR_UP` when open |
| ADF | Flaps switch | ADF controller D30 | Joystick 4, button 10 (105) | `FLAPS_DOWN` when closed, `FLAPS_UP` when open |
| ADF | Speed brakes switch | ADF controller D31 | Joystick 4, button 11 (106) | `SPOILERS_ON` when closed, `SPOILERS_OFF` when open |
| ADF | Brakes button | ADF controller D32 | Joystick 4, button 12 (107) | `PARKING_BRAKES` when pressed |
| ADF | ADF major knob clockwise, anticlockwise | ADF controller D6, D7 | Joystick 4, buttons 13, 14 (108, 109) | none (the knob tunes the ADF frequency, `ADF_FREQ`) |
| ADF | ADF minor knob clockwise, anticlockwise | ADF controller D8, D9 | Joystick 4, buttons 15, 16 (110, 111) | none |

The Radio controller sends the first 18 Auto Pilot rows and the 8 Audio rows; the ADF rows come from the ADF controller (see its chapter). If a knob turns the wrong way, swap its A and B pins in the `RotaryEncoder` lines for the five Auto Pilot knobs. Which row and column each switch is on comes from the panel schematics: the matrix columns are the Radio Auto Pilot PCB nets `COL1` to `COL4`, and `COL0` is the COM and NAV panel's four swap switches. The debounce is 50 ms and the knobs use the same polling encoder library, with acceleration, as the COM and NAV knobs.

## Auto Pilot and Audio panel LEDs (MAX7219)

One MAX7219 on the Radio Auto Pilot PCB (U2, `DOUT` unconnected, ISET resistor 10 kohm) drives 19 LEDs: DIN on A15, CLK on A14, LOAD on A13 (LedControl library, scan limit 3, intensity 8 until a `C,B` arrives). Its segment pins A to G and DP are the PCB nets `ANNODE_00` to `ANNODE_07` and its digit pins 0 to 7 are `CATHODE_00` to `CATHODE_07`; each panel LED sits between one segment (anode) and one digit (cathode). The Auto Pilot panel uses digits 0 and 1 (connector `AUTOPILOT`) and the Audio panel digits 2 and 3 (connector `AUDIO`). LedControl's `setLed(0, digit, column)` uses column 0 for DP and 1 to 7 for A to G; in the chip's own digit registers (address = digit + 1) bit 7 is DP, bit 6 is A and bit 0 is G.

| LED | Token | MAX7219 digit | Segment | | LED | Token | MAX7219 digit | Segment |
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

The first two columns are the Auto Pilot panel and the others the Audio panel. The bridges send each LED as a `LED_xxx:0/1` token in the radio data packet. `HandleOutputValuePair()` handles these before its per-pair debug line (19 extra debug packets per radio packet would flood the debug stream), `applyPanelLeds()` writes only the LEDs that changed, and the LEDs follow the radio power: they are lit only while `NAVCOM1` is 1 and the no-data timeout has not blanked the displays. `C,LEDTEST:1` lights all 19 for checking the wiring, and `C,B:0-15` (for example from the ADF controller's pot or the SwingTester slider) sets the intensity: 0 shuts the MAX7219 down, 1 to 15 set the intensity. The SwingTester has a toggle for each LED, an All ON / OFF, a chase and the board test.

## Tuning steps

The knobs follow standard channel spacing. COM runs 118.000 to 136.975 MHz in 25 kHz channels (major knob 118 to 136; minor knob 000, 025, 050, 075). NAV runs 108.00 to 117.95 MHz in 50 kHz steps (major 108 to 117). Each minor knob wraps round at its ends. The constants `COM_MINOR_STEP_KHZ` and `NAV_MINOR_STEP_KHZ` set the steps, and the wrap-around limits follow from them. 8.33 kHz channels are not offered. FSUIPC holds COM frequencies to 10 kHz, so in the simulator a channel such as 118.025 may appear as 118.02 or 118.03.

## Power and blanking

- **Avionics power:** when the avionics bus is below 24 V the bridge sends `NAVCOM1:0` and the COM and NAV displays blank together; they light together again when the bus rises.
- **No data for 30 seconds:** if no packet arrives and no knob or key is used, both OLEDs blank once. A packet, a knob movement or a key press restores them, and they blank again after another 30 seconds of silence.
- **Quiet period:** for 1.5 seconds (`matrixQuietMs`) after the matrix scanner starts it records state but raises no events, so a floating input cannot send false button presses at boot.

## Diagnostics

- The control packet `C,MATRIX:1` prints the raw matrix (H or L for each of the five columns, with all rows high and then each row driven low) to the debug stream, bypassing debounce and the quiet period. A column that reads L with nothing pressed is a hardware fault on that Mega pin or board. A faulty or badly seated Mega did exactly this on the bench: all four swap switches share column 0, so none could register, and the board also dropped off USB and the network after flashing.
- Remote reset is a software reset (see the network chapter). Bench-tested: "Ethernet Started" about 4 seconds after the command and "Setup Complete" about 5 seconds.

## PCB features the sketch does not use yet

The Jet Ranger Radio Auto Pilot PCB (v1.3) carries more than the sketch handles. What is still unused:

| Mega pins | PCB net | Status in the sketch |
|---|---|---|
| D5 | `BACKLIGHT_PWM`: 330 ohm to an IRF540N MOSFET that switches the AUDIO, AUTOPILOT and COMM_NAV panel backlights | Not driven. `C,B:n` sets the MAX7219 LED intensity, not this backlight |
| A0 to A3 | `COMM1_VOL`, `COMM2_VOL`, `NAV1_VOL`, `NAV2_VOL` (volume pots) | Not read |
| D2 to D4 | `I2C_SEL_A2`, `A1`, `A0` (mux address) | Not driven |

On the same PCB the COM1 major, COM1 minor, COM2 minor and NAV1 minor encoders have their A and B pins the other way round from the sketch (the sketch uses 31, 30 and 33, 32 for COM1, 37, 36 for COM2 minor and 41, 40 for NAV1 minor, where the PCB labels A as 30, 32, 36 and 40). It only matters if one of those knobs tunes backwards.

## Known issues

- Only COM1 and COM2 frequencies arrive from the PC; there are no NAV frequency tokens yet.
- The Radio controller emulator sends a bare frequency for every radio (see the emulator chapter).
- The mux address pins (D2 to D4) are not driven by the sketch; the mux module's own pull-downs set address 0x70.
