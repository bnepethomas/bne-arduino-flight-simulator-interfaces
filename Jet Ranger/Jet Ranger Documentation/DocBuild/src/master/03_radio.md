# Radio controller (172.16.1.101)

`JET_RANGER_RADIO_CONTROLLER` runs on an Arduino Mega 2560 with a W5500 Ethernet module. It shows the COM and NAV frequencies on two character OLEDs, reads eight rotary encoders (COM1, COM2, NAV1 and NAV2, each with a major and a minor knob) and a 4 by 5 button matrix, and keeps the radio panel in step with the simulator. It also serves the **Auto Pilot and Audio panels**: it reads their switches and the Auto Pilot's five knobs (sent as joystick buttons and as simulator controls) and drives their 19 LEDs through a MAX7219.

![Radio controller blocks](diagrams/radio_blocks.svg)

## Program flow

1. **Setup:** reset the W5500 through pin 53, bring up Ethernet and open the UDP sockets, flash the red LED for 3 seconds while the link settles, send an "Ethernet Started" debug line, scan the I2C bus (results to the debug monitor, see Diagnostics), start the COM and NAV OLEDs through the TCA9548A I2C multiplexer, start the DME display (the Audio panel's OLED) with the text `10M` and `0M`, start the eight COM and NAV encoders (with acceleration), the five Auto Pilot knobs and the matrix scanner.
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
| 5 | `BACKLIGHT_PWM`: the AUDIO, AUTOPILOT and COMM_NAV panel backlights (IRF540N MOSFET) |
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
| `BPWR` | Backlight power: 1 = main bus 20 V or more, 0 = backlight off (the radio assumes 1 until the first one arrives) | `BPWR:1` |
| `LED_xxx` | One of the 19 Auto Pilot and Audio panel LEDs (see below): 1 = on, 0 = off | `LED_HDG:1` |
| `APHDG`, `APCRS`, `APIAS`, `APVS`, `APALT` | The five values on the Auto Pilot display (see below), up to 5 characters each | `APHDG:090` |
| `C,B:n` | Brightness 0 to 15 (control packet): the panel backlight level (0 off, 15 full) and the MAX7219 intensity of the panel LEDs | `C,B:5` |
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
| ADF | Landing gear switch | ADF controller D29 | Joystick 4, button 9 (104) | `GEAR_SET:1` when closed, `GEAR_SET:0` when open |
| ADF | Flaps switch | ADF controller D30 | Joystick 4, button 10 (105) | `FLAPS_SET:1` when closed, `FLAPS_SET:0` when open |
| ADF | Speed brakes switch | ADF controller D31 | Joystick 4, button 11 (106) | `SPOILERS_SET:1` when closed, `SPOILERS_SET:0` when open |
| ADF | Brakes button | ADF controller D32 | Joystick 4, button 12 (107) | `PARKING_BRAKES` when pressed |
| ADF | ADF major knob clockwise, anticlockwise | ADF controller D6, D7 | Joystick 4, buttons 13, 14 (108, 109) | none (the knob tunes the ADF frequency, `ADF_FREQ`) |
| ADF | ADF minor knob clockwise, anticlockwise | ADF controller D8, D9 | Joystick 4, buttons 15, 16 (110, 111) | none |
| ADF | Anti-ice switch | ADF controller D34 | Joystick 4, button 17 (112) | `ANTI_ICE_SET:1` when closed, `ANTI_ICE_SET:0` when open |
| ADF | Hyd boost switch | ADF controller D33 | Joystick 4, button 18 (113) | none (bind the joystick button in FSUIPC for the Dodo) |
| ADF | Caution dim switch | ADF controller D35 | Joystick 4, button 19 (114) | none (bind the joystick button in FSUIPC for the Dodo) |

The Radio controller sends the first 18 Auto Pilot rows and the 8 Audio rows; the ADF rows come from the ADF controller (see its chapter). If a knob turns the wrong way, swap its A and B pins in the `RotaryEncoder` lines for the five Auto Pilot knobs. Which row and column each switch is on comes from the panel schematics: the matrix columns are the Radio Auto Pilot PCB nets `COL1` to `COL4`, and `COL0` is the COM and NAV panel's four swap switches. Each key is sampled once per millisecond and debounced by integration: a counter steps towards down or up with every sample and the key changes state only when it reaches an end (about 12 ms), and each sample takes two readings a few microseconds apart so a noise spike is skipped. This replaced a 50 ms timer that restarted on every reading that differed from the last, which let noise on a column line hold a release back (the MAX7219 LED digit lines run beside the column lines in the panel cables). The knobs use the same polling encoder library, with acceleration, as the COM and NAV knobs.

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

## Auto Pilot display

The Auto Pilot panel's character OLED (the PCB net `AP_OLED`, TCA9548A channel 2; `AP_OLED_Port`) sits behind a mask, so each of the five autopilot values has its own window on the 16-column, two-row display:

| | Left hand end | Centre | Right hand end |
|---|---|---|---|
| Top row | HDG | | VS |
| Bottom row | CRS | IAS | ALT |

HDG and CRS start at column 0, IAS ends at column 8 so it is centred, and VS and ALT end at column 15. At start-up the display shows heading `090`, course `110`, airspeed `80`, vertical speed `0` and altitude `3200`:

```
090            0
110    80   3200
```

(`showAutoPilot()` builds the two lines and `placeLeft()` and `placeRight()` position each value.) The bridges then send the real values as the tokens `APHDG`, `APCRS`, `APIAS`, `APVS` and `APALT` in the radio data packet: heading and course as three digits (north is 360), airspeed in knots, vertical speed in feet per minute (signed) and altitude in feet, each at most 5 characters. `setAutoPilotValueFromToken()` stores them (handled before the per-pair debug line, like the LED tokens) and `updateAP()` redraws the display when one changes; like the COM and NAV displays it goes blank while the radio has no power (`NAVCOM1:0`, or the no-data timeout) and returns with it. The FSUIPC bridge reads the autopilot heading (`0x07CC`), altitude (`0x07D4`), airspeed (`0x07E2`) and vertical speed (`0x07F2`) values and the NAV1 OBS (`0x0C4E`) for the course; the MSFS bridge reads `AUTOPILOT HEADING LOCK DIR`, `NAV OBS:1`, `AUTOPILOT AIRSPEED HOLD VAR`, `AUTOPILOT VERTICAL HOLD VAR` and `AUTOPILOT ALTITUDE LOCK VAR`. The SwingTester's **Auto Pilot display** group sends test values.

## When the USB HID board is missing

Every Audio and Auto Pilot switch and knob click is sent to the USB HID board (172.16.1.11). If the board is switched off or unplugged, the send fails after the W5500's ARP timeout (about 80 ms with the fast retransmission timeout, 1.6 seconds with the chip's default), and that wait used to be paid on every press and release, so the panel felt slow. Now a failed send to the HID board pauses joystick-button sends for **30 seconds** (`HID_RETRY_MS`), so only one press or release in that time pays the wait. The monitor shows `USB HID board 172.16.1.11 not answering (send gave up after N ms)` with the real wait, which also shows whether the fast timeout is working (N should be about 80), and `USB HID board is answering again` on recovery, when the switches that are held down are sent again. Debug sends to a host that does not answer are paused for 10 seconds in the same way (`DEBUG_RETRY_MS`).

## DME display

The Audio panel's character OLED (the PCB net `AUDIO_OLED`, TCA9548A channel 3; the Auto Pilot panel's OLED is channel 2) is the DME display. `DME_OLED_Port` selects the channel. At start-up the sketch starts it with the same OLED sequence as the COM and NAV displays and writes `10M` on the top line and `0M` on the bottom line (`showDme()` pads each line to 16 characters so a shorter text replaces a longer one). Nothing else writes to it yet. It blanks and returns with the COM, NAV and Auto Pilot displays: `updateDme()` clears it while the radio has no power (the avionics bus is below 24 V) or the no-data timeout has fired, and redraws `10M` / `0M` when they come back (it is called wherever `updateAP()` is).

## ADF controller display

The radio also tells the ADF controller (172.16.1.107, port 13136) when its displays blank, so the ADF panel's OLED and landing gear lamps go dark and come back with them. `updateDme()` calls `sendAdfDisplay()`, which sends `C,DISPLAY:0` or `C,DISPLAY:1` on every change and again every 10 seconds (`ADF_DISPLAY_RESEND_MS`, so a restarted ADF controller catches up). The ADF controller may be missing: a failed send pauses these sends for 30 seconds (`ADF_RETRY_MS`) and the monitor shows `ADF controller 172.16.1.107 not answering (send gave up after N ms)` and `ADF controller is answering again`, the same way as the USB HID board, so at most one send in 30 seconds pays the ARP wait.

## Backlight

Mega pin D5 (`BACKLIGHT_PWM`) drives an IRF540N through 330 ohm (10 kohm gate pull-down), which switches the backlights of the **AUDIO, AUTOPILOT and COMM_NAV** panels, using the same rules as the Servo, Stepper and Dual Stepper boards (see the network chapter): the backlight is on only while `BPWR` is 1 (main bus 20 V or more; the radio assumes 1 until the first `BPWR` arrives), at the last brightness commanded with `C,B:0-15` (0 off, 15 full, mapped to PWM 0 to 255) or the default PWM 65 until one has been received. `applyBacklight()` also keeps it off while the displays are blanked by the no-data timeout, which is the same 30 seconds with no UDP packet **and no knob or key use** (a knob or key restarts it, so the radio can be used with no PC), and brings it back at the same level when data or local input resumes. The same `C,B` also sets the panel LEDs' intensity. The bridges send `BPWR` in the radio packet, and the SwingTester's *Backlight power* buttons and brightness slider test it from the bench. The ADF controller's pot sends `C,B` to the radio like any other board.

## Tuning steps

The knobs follow standard channel spacing. COM runs 118.000 to 136.975 MHz in 25 kHz channels (major knob 118 to 136; minor knob 000, 025, 050, 075). NAV runs 108.00 to 117.95 MHz in 50 kHz steps (major 108 to 117). Each minor knob wraps round at its ends. The constants `COM_MINOR_STEP_KHZ` and `NAV_MINOR_STEP_KHZ` set the steps, and the wrap-around limits follow from them. 8.33 kHz channels are not offered. FSUIPC holds COM frequencies to 10 kHz, so in the simulator a channel such as 118.025 may appear as 118.02 or 118.03.

## Power and blanking

- **Avionics power:** when the avionics bus is below 24 V the bridge sends `NAVCOM1:0` and the COM and NAV displays blank together; they light together again when the bus rises.
- **No data for 30 seconds:** if no packet arrives and no knob or key is used, the COM, NAV, Auto Pilot and DME displays blank once, and the ADF controller is told to blank too. A packet, a knob movement or a key press restores them, and they blank again after another 30 seconds of silence.
- **Quiet period:** for 1.5 seconds (`matrixQuietMs`) after the matrix scanner starts it records state but raises no events, so a floating input cannot send false button presses at boot.

## Diagnostics

- **I2C scan:** at start-up, and whenever the control packet `C,I2CSCAN:1` arrives, the radio scans the Mega's own I2C bus and then each of the eight TCA9548A channels and sends every address that answers to the debug stream: `I2C main bus: 0x70 (TCA9548A)`, `I2C TCA port 0: 0x3C (OLED)`, one line for each channel with a device, `I2C TCA ports with nothing connected: ...` and `I2C scan done, N device(s) answered`. The scan uses a 5 ms I2C timeout so a stuck bus (SDA or SCL held low) is reported as `bus timeout` instead of hanging the board, and it takes a second or two, during which the knobs and keys are not read. It shows at once whether an OLED is missing or wired to the wrong mux channel.
- The control packet `C,MATRIX:1` prints the raw matrix (H or L for each of the five columns, with all rows high and then each row driven low) to the debug stream, bypassing debounce and the quiet period. A column that reads L with nothing pressed is a hardware fault on that Mega pin or board. A faulty or badly seated Mega did exactly this on the bench: all four swap switches share column 0, so none could register, and the board also dropped off USB and the network after flashing.
- Remote reset is a software reset (see the network chapter). Bench-tested: "Ethernet Started" about 4 seconds after the command and "Setup Complete" about 5 seconds.

## PCB features the sketch does not use yet

The Jet Ranger Radio Auto Pilot PCB (v1.3) carries more than the sketch handles. What is still unused:

| Mega pins | PCB net | Status in the sketch |
|---|---|---|
| A0 to A3 | `COMM1_VOL`, `COMM2_VOL`, `NAV1_VOL`, `NAV2_VOL` (volume pots) | Not read |
| D2 to D4 | `I2C_SEL_A2`, `A1`, `A0` (mux address) | Not driven |

On the same PCB the COM1 major, COM1 minor, COM2 minor and NAV1 minor encoders have their A and B pins the other way round from the sketch (the sketch uses 31, 30 and 33, 32 for COM1, 37, 36 for COM2 minor and 41, 40 for NAV1 minor, where the PCB labels A as 30, 32, 36 and 40). It only matters if one of those knobs tunes backwards.

## Known issues

- Only COM1 and COM2 frequencies arrive from the PC; there are no NAV frequency tokens yet.
- The Radio controller emulator sends a bare frequency for every radio (see the emulator chapter).
- The mux address pins (D2 to D4) are not driven by the sketch; the mux module's own pull-downs set address 0x70.
