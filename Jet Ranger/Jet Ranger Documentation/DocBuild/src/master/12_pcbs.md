# PCB notes

Notes from reading the KiCad projects in `Jet Ranger/PCBs` (schematics, PCB layouts and netlists), brought up to date on 18 October 2026. Matching boards to sketches is by name. The checks read the files only; no board was measured.

![Active PCBs, their revision and the sketches they belong to](diagrams/pcb_map.svg)

## Boards in use

| PCB | Revision (title block) | Used by | Notes |
|---|---|---|---|
| Jet Ranger Front Panel Servo | 1.4 (2026-10-18) | Servo controller | Mega shield, two ULN2803A lamp drivers, servo headers (16, 2 used), 15 lamp outputs, three switch inputs, backlight MOSFET. Change log: 1.3 had the MOSFET pinned wrongly (source and drain need swapping), 1.4 corrects it |
| Jet Ranger 13 Stepper | 1.6 (2026-10-18) | Stepper controller | Mega shield, USR-ES1 Ethernet, 14 six-pin gauge sockets, 12 V lamp supply with polyfuses, backlight MOSFET. The change log lists up to 1.5 (1.4 moved the LEDs away from the USB connectors, 1.5 selected the IRF540N); the title block now says 1.6 |
| Jet Ranger OLED Dual Stepper | 1.5 (2026-10-18) | Dual stepper controller | Mega shield, USR-ES1, gauge sockets, TCA9548A sockets for the OLEDs, backlight MOSFET. No change log text; the schematic now uses the IRF540N |
| Jet Ranger X27 Small Stepper | not set | Each stepper gauge | One X27 or VID29 stepper, backlight LED, six-pin connector to the controller |
| Jet Ranger Radio Auto Pilot | 1.3 (2026-10-18) | Radio controller | Radio and autopilot controller. Change log: 1.2 added the audio panel, 1.3 corrected the MOSFET mis-pinout |
| Jet Ranger COM and NAV | not set | Radio panel | Eight rotary encoders, push buttons and the four frequency-swap switches in a diode matrix |
| Jet Ranger Upper Controller | 1.4 (2026-10-18) | Upper controller | Mega shield for the switch matrix, backlight MOSFET, brightness input on A0. Change log: 1.4 corrected the MOSFET mis-pinout |
| Jet Ranger ADF Controller | 1.4 (2026-10-19) | ADF controller (`JET_RANGER_ADF_CONTROLLER`, .107) | Mega shield with the ADF major and minor knobs (D6 to D9), an I2C OLED, a 7-pin keypad (D22 to D28), landing-gear, flaps, speed-brake and brake inputs (D29 to D32), three landing-gear lamps (D2 to D4), the backlight MOSFET, a panel brightness pot (A0) and a 12 V monitor (A15, 47 k / 10 k divider). Change log: 1.2 added the audio panel, 1.3 corrected the MOSFET mis-pinout (IRF540 selected), 1.4 added the 12 V power rail monitors and the pot for panel brightness |
| Jet Ranger Audio Panel, Auto Pilot Panel, ADF Panel | 1.1 (2026-10-17) | Switch panels | Diode-matrix switch boards: Audio (10 switches, 21 diodes), Auto Pilot (16 switch and encoder parts, 18 diodes, 2 LEDs), ADF (9 switch and encoder parts, 4 diodes) |
| Annunciator, Altimeter, Large Guage, Simkits gauge boards | not set | Other panels and gauges | Not covered by the sketches in this document |

The X27 steppers run straight from the Mega's pins, so the stepper boards need no driver chips; the 12 V input feeds lamps and backlights. The schematic and board layout of every PCB are in Appendix C.

## Backlight driver

![The backlight MOSFET circuit](diagrams/backlight_circuit.svg)

All six controller boards (Servo, 13 Stepper, Dual Stepper, Radio Auto Pilot, Upper and ADF Controller) have the same backlight circuit, traced pad to pad in the PCB files: Mega pin **D5** is the net `BACKLIGHT_PWM`, which goes through a 330 ohm resistor to the gate of an **IRF540N** in a TO-220 package, with a 10 k resistor from the gate to ground. The drain is the backlight net (`/BACKLIGHT_PANELS` or `BACKLIGHT_-V`) that the panel or gauge backlight connectors share, and the source is ground, so the MOSFET switches the backlight low side and D5 PWM sets the brightness. `BACKLIGHT_PWM` is also brought out on pin 6 of an 8-pin header on each board.

| Board | Backlight connectors on the MOSFET drain net |
|---|---|
| Front Panel Servo | Ten `BK_LIGHT` connectors (J6 to J10, J15 to J19) |
| 13 Stepper | Pin 6 of 13 gauge sockets (airspeed, radar altitude, gas producer, transmission oil pressure, VSI, engine torque, turbine speed, rotor speed, fuel level, engine oil pressure and temperature, transmission oil temperature, EGT) |
| Dual Stepper | Gauge sockets for airspeed, VSI, fuel level, EGT, altimeter, engine, transmission, rotor and engine speed, and electrical and fuel load |
| Radio Auto Pilot | The AUDIO, AUTOPILOT and COMM_NAV connectors |
| Upper Controller | The BACKLIGHT connector (J5) |
| ADF Controller | The ADF connector (J5) |

The earlier revisions used a generic N-channel MOSFET symbol (`Q_NMOS_GSD`) whose pin-out had source and drain swapped; the revisions dated 18 October 2026 (1.4 Servo, 1.3 Radio Auto Pilot and ADF Controller, 1.4 Upper, 1.5 Stepper and Dual Stepper) use the IRF540N with the correct pin-out. A board made from an earlier revision should have the transistor checked.

The Radio, Servo, Stepper, Dual Stepper and ADF sketches drive D5 (see the network, Radio and ADF controller chapters). The Upper sketch does not. The Upper PCB has a brightness input on A0 (`GUAGE_BACKLIGHT_BRIGHT`, connector J16) that no sketch reads; the ADF PCB has the same pot input, and the ADF sketch reads it and sends the level to the other panels.

## Warning Mute, Fuel Valve and TOT Clear inputs

All KiCad schematic, PCB and netlist files under `Jet Ranger/PCBs` were searched for a Warning Mute (horn mute) input. Two boards have one.

- **Jet Ranger Front Panel Servo:** SW3 (`WARNING_MUTE`, a 2-pin Molex KK-254 connector) connects net `/SW_WARNING_MUTE` to Arduino Mega pin D47 and GND. The same board has SW1 `TOT_CLEAR` on D43 and SW2 `FUEL_VALVE` on D48, each also wired to GND. The Servo sketch reads these three pins as debounced inputs (internal pull-up, closed = LOW), logs every transition and sends it to the USB HID board.
- **Jet Ranger Upper Controller:** J17 (`WARNING MUTE`, a 2-pin Molex KK-254 connector) connects `/ROW8` (Arduino D30, shared with J18 TOT TEST) on pin 2 and, on pin 1, a 1N4148 diode D5 whose other side goes to `/COL0`, a switch in the board's diode-isolated key matrix. The Upper sketch scans a 4 by 3 matrix on analog pins A9 to A15, which does not include D30, so this input may not be read by the current firmware.

## Pin and copper check

The KiCad boards were traced pad to pad through tracks and vias on 12 October 2026 and the Mega pin tables were compared again on 18 October 2026 against that check. This checks connections only; it is not a design rule check (clearances, pours and footprints are not checked).

- **No Mega pin assignment changed** between the two checks on the Servo, 13 Stepper, Dual Stepper and Upper boards, apart from the 13 Stepper D2 net being renamed (below). The Radio Auto Pilot and ADF Controller were first checked on 18 October.
- **Jet Ranger OLED Dual Stepper:** up to version 1.3, D2 and D32 were one net (`STEPPER_FL_A`), joining fuel quantity and fuel load coil A. Fixed in 1.4: D2 is `STEPPER_FA_A` and D32 `STEPPER_FL_A` in 1.5. Boards older than 1.4 still have the short: fit only one of those two gauges or cut the trace.
- **Jet Ranger 13 Stepper:** D2 carried the same copied name but nothing else used it; it is `STEPPER_FA_A` (label only). D32 is the radar altitude coil `STEPPER_RA_A`.
- **Both stepper PCBs:** `BACKLIGHT_PWM` is D5 and VSI coil B is D8. Both stepper sketches use D5 for `BACK_LIGHTS` (pin 8 used to be both the backlight and VSI coil B).
- **Jet Ranger Front Panel Servo:** the lamp outputs are still wired to different Mega pins from the sketch for 10 of the 15 lamps: the PCB has A1 Rotor RPM low, A2 Engine out, A5 Battery temp, A6 Battery hot, A7 Transmission chip, A8 Baggage door, A9 Engine chip, A10 Tail rotor chip, A11 Fuel pump and A12 Aft fuel filter; the sketch has them the other way round for A1/A2, A5/A6 and A11/A12 and in a different order for A7 to A10 (A3, A4 and A13 to A15 agree). The power-up lamp test cannot show this. Test each lamp with a single `D,<lamp>:1` and fix whichever side is wrong.
- **Jet Ranger Radio Auto Pilot:** the COM1 major, COM1 minor, COM2 minor and NAV1 minor encoders have A and B the other way round from the sketch (see the Radio controller chapter); the other four match. The volume pots and mux address pins are wired but not used by the sketch (the autopilot knobs, MAX7219 lines and fifth matrix column are now used).
- **Servo, 13 Stepper and Upper Controller:** a net `GPS_TX` joins Mega pin D18 (TX1) to pin 5 of an 8-pin header (J11 on the Servo and 13 Stepper, J43 on the Upper). No sketch uses Serial1 on those boards. The Dual Stepper uses D16 to D19 for the OLED multiplexer instead.
- **Upper Controller, 13 Stepper and OLED Dual Stepper:** pins otherwise match their sketches. The TCA9548A address pins (D17 to D19 on the Dual Stepper, D2 to D4 on the Radio Auto Pilot) are not driven; the module's pull-downs set address 0x70.

## KiCad: changing the revision number

In the PCB Editor use *File, Page Settings*, set the *Title Block* revision field, then save the board. To show it on the board, place a text item on the `F.Silkscreen` layer (press T) containing `Rev ${REVISION}`; other variables such as `${TITLE}`, `${ISSUE_DATE}` and `${COMPANY}` work the same way. The schematic and the PCB each keep their own title block, so set both if they should match. Make a fresh Gerber zip in `MANUFACTURING` before ordering.
