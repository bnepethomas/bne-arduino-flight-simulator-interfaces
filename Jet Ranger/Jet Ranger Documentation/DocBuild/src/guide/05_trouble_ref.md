# Troubleshooting

![Debug and test tools](diagrams/debug_tools.svg)

| Symptom | Likely cause | What to check |
|---|---|---|
| Red LED keeps fast-flashing, no gauge sweep | Board stuck in Ethernet start-up | Ethernet module seated; reset line on pin 53; module powered |
| Sweep runs but gauges never move in flight | Packets not reaching UDP 13136 | PC on 172.16.1.x; Windows firewall allows outbound UDP; the bridge sends to the right boards (servo .102, steppers .105 and .106); only one bridge running |
| No debug messages, or the Health Monitor shows a board as down | Listener or Health Monitor not running, or PC address wrong | PC at 172.16.1.10; listener bound to UDP 27000, Health Monitor to UDP 13137; firewall allows inbound UDP on both |
| One gauge pinned at an end stop | Value outside the calibration table, or table points wrong | Send the `RAW` token to find the true limits, then re-measure the table (see Calibration) |
| All gauges drop to zero after 30 seconds | No packets arriving (watchdog) | Bridge running and connected; simulator not paused; network link |
| A board feels slower, or its knobs and needles stutter, while the PC (or another board) is off | Each packet sent to a missing device waits up to 80 ms for an ARP answer (the fast Ethernet timeout) | Switch the missing device on, or reduce debug traffic; see *Unreachable hosts and the fast Ethernet timeout* in the Network chapter |
| Attitude indicator lags fast changes | 1 degree per 5 ms step limit | Expected; reduce `servoCheckInterval` to speed up |
| Servo buzzes or jitters | Weak servo supply | Separate 5 to 6 V supply for servos, common ground with the Mega |
| A backlight is off | Main bus below 20 V (`BPWR:0`), no packets for 30 seconds, or brightness set to 0 | Check the main bus voltage in the bridge window; check the bridge is running; send a brightness with the SwingTester slider |
| A backlight stays on at power-up with no PC | Old firmware without the no-data rule, or the board has never been flashed with the backlight code | Flash the current Servo, Stepper and Dual Stepper sketches; the backlight should go out about 30 seconds after power-up |
| The backlight is always on or flickers | MOSFET pinned wrongly on a board from an earlier revision (source and drain swapped) | Check the transistor against the revision notes (PCBs and wiring) |
| The GPS shows Brisbane Airport | No aircraft data for 30 seconds | Check a bridge or the bench tester is running on the same PC and sending to UDP 27002; check no other program owns the port |
| The GPS shows the PC time, not the simulator time | The packets carry no `UTC:` (for example the FSUIPC time read failed) | The window says *PC clock*; check the bridge version |
| The GPS interface cannot open its port | Wrong COM port, or another program holds it | Use *Change serial port...*; close other serial programs |
| A lamp never lights | Wiring or the lamp's driver | It should light at the power-up lamp test; if not, check the A pin and its driver |
| A radio knob tunes the wrong radio | Bare frequency sent instead of a labelled one | Use the real board (it labels COM2, NAV1 and NAV2); the Radio emulator sends bare numbers, so only COM1 tunes |
| COM and NAV displays blank | Avionics bus below 24 V, or no data and no knob use for 30 seconds | Switch avionics on; turn a knob to wake them |
| Dual stepper OLEDs blank | Main bus below 24 V (`PWR:0`) | Switch the battery on; check the main bus voltage in the bridge window |
| Radio swap switches do nothing | Column 0 of the matrix stuck | Send `C,MATRIX:1` and read the debug listener (see Radio controller) |
| FSUIPC binding fires the wrong event | EVT re-indexed after an update | Re-map the binding in FSUIPC |
| Joystick buttons vanish after a reset | USB HID board rebooted | Windows re-detects them within seconds; re-check games that had them open |
| Upload fails with *Access is denied* | Another program holds the COM port | Close the serial monitor and the Arduino IDE port; check the port with `arduino-cli board list` |
| Upload goes to the wrong board | COM port moved | Re-check Device Manager; see COM ports and flashing |
| Data reaches the boards only in bursts, with gaps of several seconds | The PC's Ethernet link to the board switch is dropping in and out | In Windows, check the network adapter stays connected; check the 5 V supply of the network switch stays above 4.75 V (it once dipped below and the switch dropped its ports), then the cable and switch port |

## Known issues in the code

**Servo firmware (`JET_RANGER_SERVO_CONTROLLER.ino`)**

- `powerAvailable` is set from `NAVCOM1` but nothing uses it, so the attitude indicator stays live when the simulator's power is off (the bridge sends all lamps off below 20 V).
- Several lamps log *Transmission Temperature changed* to the debug listener, whichever lamp changed.
- The PCB labels 10 of the 15 lamp outputs on different pins from the sketch (A1/A2, A5/A6, A7 to A10, A11/A12). Test each lamp on its own, for example `D,RLOW:1`, and correct whichever side is wrong.

**FSUIPC event file (`DODSIM206V2.EVT`)**

- The line before event 133 is missing its `//`, so FSUIPC may reject or mis-read it.
- Events 189 and 190 are named the wrong way round: 189 is called `_SET` but toggles, 190 is called `_TOGGLE` but takes 1 or 2.
- Events 69 to 71 mix up the instrument and cockpit light breakers: 69 is named `COCKPIT_LT_CB_TOGGLE`, and `INST_LT_CB_SET` appears twice.

## Open questions

- Power supply rating for the servo rail and the 12 V lamp supply.
- Whether the estimates for the FlyInside and CowaSim aircraft hold at other power settings.

# Reference: UDP tokens

From the current sketches. Gauge tokens take real units; lamp tokens take 0 (off) or 1 (on). `UDP_Interface_Values.xlsx` still lists the old 16-servo pin plan, so use these tables instead.

## Gauge tokens

| Token | Gauge | Board | Units |
|---|---|---|---|
| `BANK` | Attitude bank | Servo (.102) | degrees, plus and minus 90 |
| `PITCH` | Attitude pitch | Servo (.102) | degrees, plus and minus 30 |
| `IAS` | Airspeed | Stepper (.105) | knots, one decimal allowed |
| `VSI` | Vertical speed | Stepper (.105) | ft/min |
| `AGL` | Radar altitude | Stepper (.105) | feet |
| `TQ` | Engine torque | Stepper (.105) | % |
| `N1` | Gas producer | Stepper (.105) | % |
| `ITT` | Turbine out temperature | Stepper (.105) or Dual (.106) | degrees C |
| `FUEL` | Fuel quantity | Stepper (.105) or Dual (.106) | US gal |
| `OILT` | Engine oil temperature | Dual (.106) or Stepper | degrees C |
| `OILP` | Engine oil pressure | Dual (.106) or Stepper | psi |
| `XMSNT` | Transmission oil temperature | Dual (.106) or Stepper | degrees C |
| `XMSNP` | Transmission oil pressure | Dual (.106) or Stepper | psi |
| `RPME` | Engine RPM (N2) | Dual (.106) | %, one decimal |
| `RPMR` | Rotor RPM | Dual (.106) | %, one decimal |
| `ALT` | Altimeter | Dual (.106) | feet |
| `FUELLOAD` | Fuel load | Dual (.106) | psi |
| `ELECTRICALLOAD` | Electrical load | Dual (.106) | % |

Every gauge token also has a `RAW` twin (for example `PITCHRAW`, `IASRAW`) that takes a raw servo position or stepper step count and skips the calibration table.

## Lamp tokens (servo board)

| Token | Lamp | Pin |
|---|---|---|
| `EOUT` | Engine out | A1 |
| `RLOW` | Rotor RPM low | A2 |
| `TOPW` | Transmission oil pressure | A3 |
| `TOTW` | Transmission oil temperature | A4 |
| `BHOT` | Battery hot | A5 |
| `BTMP` | Battery temperature | A6 |
| `EC` | Engine chip | A7 |
| `TRC` | Tail rotor chip | A8 |
| `TC` | Transmission chip | A9 |
| `BD` | Baggage door | A10 |
| `FFLTR` | Aft fuel filter | A11 |
| `FPMP` | Fuel pump | A12 |
| `GENF` | Generator fail | A13 |
| `LOWF` | Low fuel | A14 |
| `SCF` | SC fail (standby compass) | A15 |

## Aircraft information to the GPS interface (UDP 27002)

`D,LAT:<deg>,LON:<deg>,ALT:<feet>,IAS:<knots>,HDG:<degrees true>,UTC:<yyyyMMddHHmmss[.fff]>`, sent every 500 ms by a bridge or the bench tester. Any subset of keys works, and the CSV form `lat,lon,altitude_ft,airspeed_kt,heading_deg[,utc]` is accepted too. South and west are negative.

## Status and control tokens

| Token | Packet | Meaning |
|---|---|---|
| `NAVCOM1` | D | 0 = avionics bus below 24 V (radio COM and NAV displays blank), 1 = on |
| `BPWR` | D | Backlight power for the Servo, Stepper and Dual Stepper: 1 = main bus 20 V or more, 0 = backlight off. Sent first in the Servo and Stepper packets and after `PWR` in the Dual Stepper packet |
| `PWR` | D | Dual stepper board: 1 = all four OLEDs on, 0 = blank them (main bus below 24 V). Sent first in the packet |
| `ZULU` | D | Zulu time as HHMM, for example 1430 (dual stepper clock OLED) |
| `TEMP` | D | Temperature text for the dual stepper's temperature OLED, for example 25C |
| `B` | C | Backlight brightness 0 to 15 (0 off, 15 full). Servo, Stepper and Dual Stepper only; the Radio parses it but does nothing and the Upper ignores it. The ADF controller's pot sends it to the other panels |
| `SWING<CODE>` | C | Runs one gauge's start-up sweep on demand, for example `C,SWINGTQ:1` (stepper boards) or `C,SWINGPITCH:1` (servo board); the value is ignored |
| `OLEDTEST` | C | 1 fills the four OLEDs solid white, 0 restores them (dual stepper board) |
| `KEEPALIVE` | C | Ignored command that still counts as received data and restarts the 30 second no-data timer (SwingTester sends it during a run) |
| `RESET` | C | Reboots the board about 2 seconds later (servo, stepper, dual stepper, radio, upper and ADF controllers on port 13136; USB HID board on port 4210). The radio and upper controllers use a software reset because a watchdog reset hung the radio's bootloader; the upper controller's reset has not been bench-tested |
| `MATRIX` | C | Radio and ADF controllers: print the raw button matrix to the debug listener |
| `POWER` | C | ADF controller: send a power status (12 V and 5 V rails) now |
