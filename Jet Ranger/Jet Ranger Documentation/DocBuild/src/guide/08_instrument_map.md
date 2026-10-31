# Instrument map

This appendix draws every gauge, lamp and display of the cockpit as the real thing (dial gauges with needles, the attitude indicator, annunciator and legend lamps, gear lights, OLED displays) and joins each one by a cable to the connector on the controller board that drives it. The connector references and names are the ones printed on the PCBs (see the installation views in Appendix F). Needles and lamps are drawn at illustrative readings and the lens colours are illustrative. Switches, knobs, keypads and the USB HID board are inputs and are not drawn. The full-size drawing is on one A3 landscape page, `Jet Ranger Instrument Map A3.pdf` (also an SVG and a PNG) in the documentation folder; it is made by `DocBuild/make_instrument_map.py`.

![The whole map (the A3 original is Jet Ranger Instrument Map A3.pdf; zoom in the PDF to read it)](diagrams/instrument_map_full.png)

<!-- pagebreak -->

## Stepper gauges and the Stepper controller

![Stepper gauges and the Stepper controller](diagrams/instrument_map_stepper.png)

<!-- pagebreak -->

## Dual Stepper gauges and the Dual Stepper controller

![Dual Stepper gauges and the Dual Stepper controller](diagrams/instrument_map_dual.png)

<!-- pagebreak -->

## Attitude indicator and the Servo controller

![Attitude indicator and the Servo controller](diagrams/instrument_map_servo.png)

<!-- pagebreak -->

## Huey Warning Panel, key, and the top of the Radio and ADF boards

![Huey Warning Panel, key, and the top of the Radio and ADF boards](diagrams/instrument_map_radio_top.png)

<!-- pagebreak -->

## Dual Stepper displays and the Servo annunciator lamps

![Dual Stepper displays and the Servo annunciator lamps](diagrams/instrument_map_lower_left.png)

<!-- pagebreak -->

## Radio panels (COM and NAV, Auto Pilot, Audio) and the ADF panel

![Radio panels (COM and NAV, Auto Pilot, Audio) and the ADF panel](diagrams/instrument_map_lower_right.png)

<!-- pagebreak -->

## Gauges and the connectors they use

| Instrument | Token | Connector on the board | Range |
|---|---|---|---|
| Airspeed | `IAS` | Stepper .105: J26 AIRSPEED | 0 to 140 knots |
| Vertical speed | `VSI` | Stepper .105: J5 VVI | -1750 to +1750 ft/min |
| Radar altitude | `AGL` | Stepper .105: J23 RADAR_ALT | 0 to 2500 ft |
| Torque | `TQ` | Stepper .105: J25 ENG_TORQUE | 0 to 120 percent |
| Gas producer (N1) | `N1` | Stepper .105: J24 GAS_PRODUCER | 0 to 103 percent |
| Turbine out temperature | `ITT` | Stepper .105: J27 EGT | 100 to 900 C |
| Fuel quantity | `FUEL` | Stepper .105: J4 FUEL_LEVEL | 0 to 75 US gallons |
| Engine oil pressure and temperature (dual instrument) | `OILP`, `OILT` | Dual Stepper .106: J8 ENGINE | 0 to 150 psi, 0 to 150 C |
| Transmission oil pressure and temperature (dual instrument) | `XMSNP`, `XMSNT` | Dual Stepper .106: J6 XMSN | 0 to 150 psi, 0 to 150 C |
| Rotor speed and engine speed N2 (dual instrument) | `RPMR`, `RPME` | Dual Stepper .106: J7 ROTOR ENG SPEED | 0 to 110 percent |
| Electrical load and fuel load (dual instrument) | `ELECTRICALLOAD`, `FUELLOAD` | Dual Stepper .106: J10 ELEC FUEL LOAD | 0 to 100 percent, 0 to 30 psi |
| Altimeter (and its zero sensor) | `ALT` | Dual Stepper .106: J33 ALT, J9 ALTIMETER_ZERO | one turn per 1000 ft |
| Attitude indicator pitch | `PITCH` | Servo .102: J42 PITCH | -30 to +30 degrees |
| Attitude indicator bank | `BANK` | Servo .102: J43 ROLL | -90 to +90 degrees |

## Lamps and displays and the connectors they use

| Lamps and displays | Tokens | Connector on the board |
|---|---|---|
| 15 annunciator lamps (engine out, rotor low, transmission oil pressure and temperature, battery hot and temperature, engine chip, tail rotor chip, transmission chip, baggage door, fuel pump, aft fuel filter, generator fail, low fuel, SC fail) | `EOUT`, `RLOW`, `TOPW`, `TOTW`, `BHOT`, `BTMP`, `EC`, `TRC`, `TC`, `BD`, `FPMP`, `FFLTR`, `GENF`, `LOWF`, `SCF` on Mega pins A1 to A15 | Servo .102: J4 LHS_ANNUC and J5 RHS_ANNUC (how the lamps divide between the two is not recorded) |
| COM and NAV frequency OLEDs | `C1A`, `C1S`, `C2A`, `C2S` | Radio .101: J8 COMM_NAV |
| Auto Pilot display (OLED) and 8 LEDs | `APHDG`, `APCRS`, `APIAS`, `APVS`, `APALT`; `LED_CRS`, `LED_HDG`, `LED_IAS`, `LED_ALT`, `LED_APPR`, `LED_FD`, `LED_BC`, `LED_APENG` | Radio .101: J12 AUTOPILOT |
| DME display (OLED) and 11 audio panel LEDs | `LED_COM1`, `LED_BOTH`, `LED_NAV2`, `LED_DME`, `LED_OUTER`, `LED_INNER`, `LED_COM2`, `LED_NAV1`, `LED_MKR`, `LED_ADF`, `LED_MID` | Radio .101: J3 AUDIO |
| 20 Huey Warning Panel lamps | `LED_H_*` (MAX7219 digits 4 to 7) | Radio .101: J22 HUEY_ANODES and J23 HUEY_CATHODES (rev 1.4 layout; the panel is hand wired today) |
| 4 Dual Stepper OLEDs (barometer, altimeter digits, clock, temperature) | `ALT`, `ZULU`, `TEMP` | Dual Stepper .106: J55 BARO-OLED, J17 ALTIMETER-OLED, J18 CLOCK-OLED, J20 TEMPERATURE-OLED |
| ADF display (OLED) and 3 landing gear lamps | `LDGN`, `LDGL`, `LDGR` (Mega pins D2, D3, D4) | ADF and Upper .107: J5 ADF |

Which dual instrument uses which socket is taken from the names on the PCBs (J8 ENGINE, J6 XMSN, J7 ROTOR ENG SPEED, J10 ELEC FUEL LOAD); the Stepper's turbine out temperature gauge is shown on J27 EGT; and how the 15 annunciator lamps divide between J4 and J5 is not recorded, so the drawing joins the whole lamp panel to both.
