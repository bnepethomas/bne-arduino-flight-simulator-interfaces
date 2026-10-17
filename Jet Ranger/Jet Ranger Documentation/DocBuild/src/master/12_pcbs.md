# PCB notes

Notes from reading the KiCad projects in `Jet Ranger/PCBs` (schematics, PCB layouts and netlists). Matching boards to sketches is by name.

![Active PCBs and the sketches they belong to](diagrams/pcb_map.svg)

## Boards in use

| PCB | Used by | Main parts |
|---|---|---|
| Jet Ranger Front Panel Servo | Servo controller | Mega shield, two ULN2803A lamp drivers, servo headers (16 on the PCB, 2 used), 15 annunciator outputs, three switch inputs |
| Jet Ranger 13 Stepper | Stepper controller | Mega shield, USR-ES1 Ethernet module, 14 six-pin gauge sockets, 12 V annunciator supply with polyfuses |
| Jet Ranger OLED Dual Stepper | Dual stepper controller | Mega shield, USR-ES1, six-pin gauge sockets, TCA9548A sockets for the OLEDs |
| Jet Ranger X27 Small Stepper | Each stepper gauge | One X27 or VID29 stepper, backlight LED, six-pin connector to the controller |
| Jet Ranger Radio Auto Pilot | Radio controller | Radio controller board |
| Jet Ranger COM and NAV | Radio controller | Front panel board: eight rotary encoders, push buttons and the four frequency-swap switches in a diode matrix |
| Jet Ranger Upper Controller | Upper controller | Mega shield for the switch matrix |
| Annunciator, Altimeter, Audio Panel, Auto Pilot Panel, ADF Panel, ADF Controller, Large Guage, Simkits gauge boards | Other panels and gauges | Not covered by the sketches in this document |

The X27 steppers run straight from the Mega's pins, so the stepper boards need no driver chips; the 12 V input feeds annunciators and backlights.

## Warning Mute, Fuel Valve and TOT Clear inputs

All KiCad schematic, PCB and netlist files under `Jet Ranger/PCBs` were searched for a Warning Mute (horn mute) input. Two boards have one.

- **Jet Ranger Front Panel Servo:** SW3 (`WARNING_MUTE`, a 2-pin Molex KK-254 connector) connects net `/SW_WARNING_MUTE` to Arduino Mega pin D47 and GND. The same board has SW1 `TOT_CLEAR` on D43 and SW2 `FUEL_VALVE` on D48, each also wired to GND. The Servo sketch reads these three pins as debounced inputs (internal pull-up, closed = LOW), logs every transition and sends it to the USB HID board.
- **Jet Ranger Upper Controller:** J17 (`WARNING MUTE`, a 2-pin Molex KK-254 connector) connects `/ROW8` (Arduino D30, shared with J18 TOT TEST) on pin 2 and, on pin 1, a 1N4148 diode D5 whose other side goes to `/COL0`, a switch in the board's diode-isolated key matrix. The Upper sketch scans a 4 by 3 matrix on analog pins A9 to A15, which does not include D30, so this input may not be read by the current firmware.

## Pin and copper check (12 October 2026)

The KiCad boards were traced pad to pad through tracks and vias. No board had copper joining two controller pins in the final state, and every production board was fully routed. This checks connections only; it is not a design rule check (clearances, pours and footprints are not checked).

- **Jet Ranger OLED Dual Stepper:** up to version 1.3, D2 and D32 were one net (`STEPPER_FL_A`), joining fuel quantity and fuel load coil A. Fixed in version 1.4 (D2 is now `STEPPER_FA_A`). Boards older than 1.4 still have the short: fit only one of those two gauges or cut the trace. Make a fresh Gerber zip before ordering; the newest zip in `MANUFACTURING` was from 10 October.
- **Jet Ranger 13 Stepper:** D2 carried the same copied name but nothing else used it; it is renamed `STEPPER_FA_A`, label only, so no new board is needed.
- **Both stepper PCBs:** `BACKLIGHT_PWM` is D5 and VSI coil B is D8. Both stepper sketches use D5 for `BACK_LIGHTS` (pin 8 used to be both the backlight and VSI coil B).
- **Jet Ranger Front Panel Servo:** 10 of the 15 annunciator nets sit on different Mega pins from the sketch (A1/A2, A5/A6, A7 to A10, A11/A12). To be bench-tested lamp by lamp.
- **Upper Controller, 13 Stepper and OLED Dual Stepper:** pins otherwise match their sketches. The TCA9548A address pins (D17 to D19 on the Dual Stepper) are not driven; the module's pull-downs set address 0x70.

## KiCad: changing the revision number

In the PCB Editor use *File, Page Settings*, set the *Title Block* revision field, then save the board. To show it on the board, place a text item on the `F.Silkscreen` layer (press T) containing `Rev ${REVISION}`; other variables such as `${TITLE}`, `${ISSUE_DATE}` and `${COMPANY}` work the same way. The schematic and the PCB each keep their own title block, so set both if they should match.
