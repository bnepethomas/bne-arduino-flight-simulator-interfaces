# Appendix A: UDP token reference {-}

From the current sketches. Gauge tokens take real units; lamp tokens take 0 (off) or 1 (on). `UDP_Interface_Values.xlsx` still lists the old 16-servo plan, so use these tables instead. Every gauge token also has a `RAW` twin (`PITCHRAW`, `IASRAW`) that takes a raw servo position or step count and skips the calibration table.

## Gauge tokens

| Token | Gauge | Board | Unit |
|---|---|---|---|
| `PITCH` | Attitude pitch | Servo .102 | degrees, -30 to +30 |
| `BANK` | Attitude bank | Servo .102 | degrees, -90 to +90 |
| `IAS` | Airspeed | Stepper .105 | knots, one decimal allowed |
| `VSI` | Vertical speed | Stepper .105 | ft/min |
| `AGL` | Radar altitude | Stepper .105 | feet, 0 to 2500 |
| `TQ` | Engine torque | Stepper .105 | percent, 0 to 120 |
| `N1` | Gas producer | Stepper .105 | percent, 0 to 103 |
| `ITT` | Turbine out temperature | Stepper .105 or Dual .106 | degrees C |
| `FUEL` | Fuel quantity | Stepper .105 or Dual .106 | US gallons |
| `OILT` | Engine oil temperature | Dual .106 (or Stepper) | degrees C |
| `OILP` | Engine oil pressure | Dual .106 (or Stepper) | psi |
| `XMSNT` | Transmission oil temperature | Dual .106 (or Stepper) | degrees C |
| `XMSNP` | Transmission oil pressure | Dual .106 (or Stepper) | psi |
| `RPME` | Engine speed (N2) | Dual .106 | percent, one decimal |
| `RPMR` | Rotor speed | Dual .106 | percent, one decimal |
| `ALT` | Altimeter | Dual .106 | feet |
| `FUELLOAD` | Fuel load | Dual .106 | psi |
| `ELECTRICALLOAD` | Electrical load | Dual .106 | percent |

## Display and status tokens

| Token | Board | Meaning |
|---|---|---|
| `ZULU` | Dual .106 | Zulu time as HHMM, for example `1430` |
| `TEMP` | Dual .106 | Temperature text up to 8 characters, for example `25C` |
| `PWR` | Dual .106 | 1 = all four OLEDs on, 0 = blank (main bus below 24 V). Sent first in the packet |
| `C1A`, `C1S`, `C2A`, `C2S` | Radio .101 | COM1 and COM2 active and standby frequency, MHz |
| `MAINBUS` | Radio .101 | Main bus voltage |
| `NAVCOM1` | Radio .101 | 0 = avionics bus below 24 V (COM and NAV displays blank), 1 = on |
| `BPWR` | Servo .102, Stepper .105, Dual .106 | Backlight power: 1 = main bus 20 V or more, 0 = backlight off |

## Lamp tokens (Servo .102)

`EOUT`, `RLOW`, `TOPW`, `TOTW`, `BHOT`, `BTMP`, `EC`, `TRC`, `TC`, `BD`, `FFLTR`, `FPMP`, `GENF`, `LOWF`, `SCF` on pins A1 to A15 in that order (see the Servo controller chapter).

## Control tokens (`C,<COMMAND>:<value>`)

| Command | Meaning |
|---|---|
| `SWING<CODE>` | Run one gauge's start-up swing on demand, for example `C,SWINGTQ:1`; the value is ignored |
| `OLEDTEST` | 1 fills the four Dual Stepper OLEDs solid white, 0 restores them |
| `KEEPALIVE` | Ignored command that still counts as received data and restarts the 30 second no-data timer |
| `RESET` | Reboot the board about 2 seconds later (port 13136; USB HID board on 4210) |
| `MATRIX` | Radio and ADF controllers: print the raw keypad matrix to the debug stream |
| `POWER` | ADF controller: send a power status (12 V and 5 V rails) now |
| `B` | Backlight brightness 0 to 15 (0 off, 15 full): Servo, Stepper and Dual Stepper; the Radio parses it but does nothing, the Upper ignores it. The ADF controller's pot sends it to the other panels |

## Messages from the ADF controller to the PC (UDP 27003)

`ADF_FREQ:<kHz>` (190 to 1799, sent 250 ms after the knobs stop), `RADAR_FOLLOW_ID:<digits>` (`RADAR_FOLLOW_ID:0` = stop following) and `POWER,V12:<volts>,V5:<volts>,STATUS:<OK, or 5V_LOW, 5V_HIGH, 12V_LOW, 12V_HIGH joined with +>` every 30 seconds. No bridge reads this port yet.

## Aircraft information to the GPS interface (UDP 27002)

`D,LAT:<deg>,LON:<deg>,ALT:<feet>,IAS:<knots>,HDG:<degrees true>,UTC:<yyyyMMddHHmmss[.fff]>`, every 500 ms from a bridge or the bench tester. Any subset of keys is accepted; a CSV form `lat,lon,altitude_ft,airspeed_kt,heading_deg[,utc]` also works. South and west are negative.

## Messages from the Radio controller to the bridge (UDP 27001)

`COM1_RADIO_SWAP`, `COM2_RADIO_SWAP`, `NAV1_RADIO_SWAP`, `NAV2_RADIO_SWAP`, `AVIONICS_MASTER_SET`, `MASTER_BATTERY_ON`, `MASTER_BATTERY_OFF`, `ALTERNATOR_ON`, `ALTERNATOR_OFF`, `COM2_STANDBY:<MHz>`, `NAV1_STANDBY:<MHz>`, `NAV2_STANDBY:<MHz>` and a bare number for the COM1 standby frequency.

# Appendix B: Building and flashing {-}

## Libraries

Ethernet 2.0.2 with SPI, AccelStepper 1.64.0 (steppers), Servo and Ewma (Servo controller), U8g2 and Wire (OLEDs), DCS-BIOS 0.3.13 (sketches that still carry DCS-BIOS code), and the Joystick and Keyboard libraries for the HID board. Install them from the Arduino Library Manager.

## Compile and upload

![Flashing a board](diagrams/flash_workflow.svg)

```
arduino-cli compile --fqbn arduino:avr:mega "Jet Ranger Arduino Sketches\JET_RANGER_STEPPER_CONTROLLER"
arduino-cli upload -p COM4 --fqbn arduino:avr:mega "Jet Ranger Arduino Sketches\JET_RANGER_STEPPER_CONTROLLER"
```

Use `--fqbn arduino:avr:leonardo` for the USB HID board. Close the Arduino IDE serial monitor first: it holds the port ("Access is denied"). Check the port with Device Manager or `arduino-cli board list`.

## COM port to board mapping

![Which board is on which COM port](diagrams/com_ports.svg)

| Board | IP | Hardware | COM port |
|---|---|---|---|
| Radio controller | .101 | Mega 2560 (replacement board) | COM20 |
| Servo controller | .102 | Mega 2560 clone | COM16 |
| Upper controller | .103 | Mega 2560 | not recorded |
| Stepper controller | .105 | Mega 2560 | COM4 |
| Dual stepper controller | .106 | Mega 2560 | COM5 |
| ADF controller | .107 | Mega 2560 | not recorded |
| USB HID board | .11 | Arduino Leonardo | COM8 |

The Lowrance GPS cable is not one of these boards: the GPS interface asks for its serial port the first time it runs and remembers it. These are the ports used at the last flash of each board. They follow the USB socket and the board: the Servo clone has no USB serial number, so its COM number is tied to the socket it is plugged into, and a replaced Mega gets a new number. Check before every upload.

## Sketch settings to know about

- Per-gauge `SwingXXX` gates and `SwingLoops` near the top of the Stepper and Dual Stepper sketches control which swings run at boot.
- Zero trims (`<CODE>ZeroOffset`, `TSoffset`, `RSoffset`) are set near the top of the same sketches.
- Every sketch has `reflectorIP` (172.16.1.10) for debug and heartbeat traffic.

# Appendix C: Calibration {-}

![The calibration loop](diagrams/calibration_flow.svg)

Between table points the boards interpolate in a straight line and hold the end value beyond the ends.

1. Send a **raw** value (`D,TQRAW:300`) to move the needle to a raw step or servo position.
2. Read the dial against its printed scale.
3. Repeat at three or more points (zero, mid scale, full scale).
4. Edit the calibration table in the sketch.
5. Compile and upload.
6. Send **real** values (`D,TQ:60`) and confirm the dial.

The StepperVSITester sends every raw and real token without a simulator. Do not drive a gauge hard against its stop: a stalled stepper or servo draws high current and gets hot. After any mechanical change to a gauge, recheck its table and zero trim.

# Appendix D: How this document is built {-}

This document and the User Guide are generated, not typeset by hand. The sources are Markdown files and vector diagrams in `Jet Ranger Documentation/DocBuild`:

| Item | Purpose |
|---|---|
| `src/master/*.md`, `src/guide/*.md` | Text of this document and the User Guide |
| `diagrams_a.py` to `diagrams_d.py`, `svgdraw.py`, `make_diagrams.py` | The 34 diagrams, written as Python functions that draw SVG; `python make_diagrams.py` regenerates them |
| `build_docs.py`, `run_lo.ps1` | Builds the `.odt` and `.pdf` through LibreOffice's Python interface (`run_lo.ps1 build_docs.py master`) |
| `pdf2png.ps1`, `montage.ps1`, `svg2png.py` | Helpers for checking pages and diagrams visually |

To change a diagram edit its function and rebuild; to change text edit the Markdown and rebuild. Keep `PROGRAM_SUMMARY.md` files and these sources in step after a code change.
