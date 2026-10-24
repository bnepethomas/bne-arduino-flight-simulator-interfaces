# Appendix C: PCB schematics and layouts {-}

The schematic (the top sheet of each project) and the board layout of every PCB project in `Jet Ranger/PCBs/`, exported from KiCad 9. The layout shows the front and back copper (red and blue), the silkscreen (yellow) and the board outline, viewed from the front. The images are bitmaps: zoom in on the PDF or the document to read the labels, or open the KiCad project for the full detail. The schematic and layout images are regenerated from the KiCad files, so they show the boards as they were on the day the document was built, including any changes not yet committed.

<!-- pagebreak -->

## Jet Ranger Radio Auto Pilot

Revision 1.3 (change log; the title block shows 1.2, 2026-10-17). Mega shield for the Radio controller (172.16.1.101). Carries the USR-ES1 Ethernet module, the TCA9548A I2C multiplexer for the OLEDs, the MAX7219 that lights the 19 Auto Pilot and Audio panel LEDs, the backlight MOSFET (D5), the COM and NAV panel connector (`COMM_NAV`), the `AUTOPILOT` and `AUDIO` panel connectors, five matrix columns and the Auto Pilot knob inputs.

![Jet Ranger Radio Auto Pilot schematic](pcb/radio_auto_pilot_sch.jpg)

![Jet Ranger Radio Auto Pilot board layout](pcb/radio_auto_pilot_pcb.jpg)

<!-- pagebreak -->

## Jet Ranger COM and NAV

No revision set (title block dated 2024-08-10). Radio panel board behind the COM and NAV faceplate. Eight rotary encoders (a major and a minor knob for COM1, COM2, NAV1 and NAV2), the four frequency-swap switches in a diode matrix on column 0, four volume pots, and sockets for the two character OLEDs.

![Jet Ranger COM and NAV schematic](pcb/com_and_nav_sch.jpg)

![Jet Ranger COM and NAV board layout](pcb/com_and_nav_pcb.jpg)

<!-- pagebreak -->

## Jet Ranger Front Panel Servo

Revision 1.4 (2026-10-18). Mega shield for the Servo controller (172.16.1.102). ULN2803A lamp drivers for the 15 warning lamps, headers for the pitch and bank servos, three front-panel switch inputs, the USR-ES1 Ethernet module and the backlight MOSFET. Version 1.4 corrected the MOSFET pin-out.

![Jet Ranger Front Panel Servo schematic](pcb/front_panel_servo_sch.jpg)

![Jet Ranger Front Panel Servo board layout](pcb/front_panel_servo_pcb.jpg)

<!-- pagebreak -->

## Jet Ranger Upper Controller

Revision 1.4 (2026-10-18). Mega shield for the Upper controller (172.16.1.103). The overhead switch matrix and keypad inputs, the backlight MOSFET (D5), a brightness input on A0 (`GUAGE_BACKLIGHT_BRIGHT`) and the USR-ES1 Ethernet module. Version 1.4 corrected the MOSFET pin-out.

![Jet Ranger Upper Controller schematic](pcb/upper_controller_sch.jpg)

![Jet Ranger Upper Controller board layout](pcb/upper_controller_pcb.jpg)

<!-- pagebreak -->

## Jet Ranger 13 Stepper

Title block revision 1.6 (2026-10-18); the change log lists changes up to 1.5. Mega shield for the Stepper controller (172.16.1.105). Fourteen six-pin gauge sockets for the X27 steppers, a 12 V lamp supply with polyfuses, the USR-ES1 Ethernet module and the backlight MOSFET. 1.4 moved the LEDs away from the USB connectors and 1.5 selected the IRF540N.

![Jet Ranger 13 Stepper schematic](pcb/stepper_13_sch.jpg)

![Jet Ranger 13 Stepper board layout](pcb/stepper_13_pcb.jpg)

<!-- pagebreak -->

## Jet Ranger OLED Dual Stepper

Revision 1.5 (2026-10-18). Mega shield for the Dual Stepper controller (172.16.1.106). Six-pin gauge sockets for the oil, load, RPM and altimeter gauges, TCA9548A sockets for the four OLEDs (barometer, altimeter, clock, temperature), the USR-ES1 Ethernet module and the backlight MOSFET. 1.3 swapped power and ground on the four-pin OLED connections, 1.4 removed the short between Mega pins 2 and 32 and 1.5 corrected the MOSFET.

![Jet Ranger OLED Dual Stepper schematic](pcb/oled_dual_stepper_sch.jpg)

![Jet Ranger OLED Dual Stepper board layout](pcb/oled_dual_stepper_pcb.jpg)

<!-- pagebreak -->

## Jet Ranger X27 Small Stepper

Title block revision 2 (2022-08-04). One board for each stepper gauge. One X27 or VID29 stepper motor, two backlight LEDs with a 470 ohm resistor (the schematic value, see the backlight resistor table) and the six-pin connector to the controller.

![Jet Ranger X27 Small Stepper schematic](pcb/x27_small_stepper_sch.jpg)

![Jet Ranger X27 Small Stepper board layout](pcb/x27_small_stepper_pcb.jpg)

<!-- pagebreak -->

## Jet Ranger ADF Controller

Revision 1.4 (2026-10-19). Mega shield for the ADF controller (172.16.1.107). The ADF major and minor knobs (D6 to D9), the 7-pin keypad (D22 to D28), the landing gear, flaps, speed brake and brake inputs (D29 to D32), three landing gear lamps (D2 to D4), the I2C OLED, the backlight MOSFET, a panel brightness pot on A0 and a 12 V monitor on A15 (47 k and 10 k divider). 1.2 added the audio panel, 1.3 corrected the MOSFET and 1.4 added the 12 V monitor and the brightness pot.

![Jet Ranger ADF Controller schematic](pcb/adf_controller_sch.jpg)

![Jet Ranger ADF Controller board layout](pcb/adf_controller_pcb.jpg)

<!-- pagebreak -->

## Jet Ranger Audio Panel

Revision 1.1 (2026-10-17). Switch panel for the Radio Auto Pilot controller's `AUDIO` connector. Eight switches in a diode matrix on columns 3 and 4 (COM1, BOTH, NAV2, DME, COM2, NAV1, MKR, ADF), eleven LEDs on the MAX7219 digits 2 and 3, a connector for the Audio panel OLED and the backlight connector.

![Jet Ranger Audio Panel schematic](pcb/audio_panel_sch.jpg)

![Jet Ranger Audio Panel board layout](pcb/audio_panel_pcb.jpg)

<!-- pagebreak -->

## Jet Ranger Auto Pilot Panel

Revision 1.1 (2026-10-17). Switch panel for the Radio Auto Pilot controller's `AUTOPILOT` connector. Eight switches in a diode matrix on columns 1 and 2 (AP ENGAGE, HDG, CRS, ALT, APPR, FD, BC, IAS), five rotary encoders (CRS, ALT, VS, IAS, HDG), eight LEDs on the MAX7219 digits 0 and 1, the AP OLED connector, the autopilot magnet coil connection and the backlight connector.

![Jet Ranger Auto Pilot Panel schematic](pcb/auto_pilot_panel_sch.jpg)

![Jet Ranger Auto Pilot Panel board layout](pcb/auto_pilot_panel_pcb.jpg)

<!-- pagebreak -->

## Jet Ranger ADF Panel

Revision 1.1 (2026-10-17). Panel for the ADF controller's `ADF` connector. The ADF major and minor encoders, the landing gear, flaps, speed brake and brakes switches, three landing gear lamps with 330 ohm resistors, the keypad connector, the ADF OLED connector and the backlight connector.

![Jet Ranger ADF Panel schematic](pcb/adf_panel_sch.jpg)

![Jet Ranger ADF Panel board layout](pcb/adf_panel_pcb.jpg)

<!-- pagebreak -->

## Annunciator

Schematic sheet only: the project has no board layout. The same sheet is used inside the Servo, 13 Stepper and Dual Stepper schematics. Warning lamp annunciator circuit.

![Annunciator schematic](pcb/annunciator_sch.jpg)

<!-- pagebreak -->

## Jet Ranger Altimeter

Title block revision 2 (2022-08-04). Not covered by the sketches. Altimeter gauge board.

![Jet Ranger Altimeter schematic](pcb/altimeter_sch.jpg)

![Jet Ranger Altimeter board layout](pcb/altimeter_pcb.jpg)

<!-- pagebreak -->

## Large Guage (Simmeters Large Guage Replacement)

Title block revision 2 (2022-08-04). Not covered by the sketches. Replacement board for a large Simmeters gauge.

![Large Guage (Simmeters Large Guage Replacement) schematic](pcb/large_gauge_sch.jpg)

![Large Guage (Simmeters Large Guage Replacement) board layout](pcb/large_gauge_pcb.jpg)

<!-- pagebreak -->

## Simkits Large Dual Gauge

Title block revision 2 (2022-08-04). Not covered by the sketches. Replacement board for a large dual Simkits gauge.

![Simkits Large Dual Gauge schematic](pcb/simkits_large_dual_sch.jpg)

![Simkits Large Dual Gauge board layout](pcb/simkits_large_dual_pcb.jpg)

<!-- pagebreak -->

## Simkits Large Guage

Title block revision 2 (2022-08-04). Not covered by the sketches. Replacement board for a large Simkits gauge.

![Simkits Large Guage schematic](pcb/simkits_large_sch.jpg)

![Simkits Large Guage board layout](pcb/simkits_large_pcb.jpg)

<!-- pagebreak -->

## Simkits Small Dual Gauge

Title block revision 2 (2022-08-04). Not covered by the sketches. Replacement board for a small dual Simkits gauge.

![Simkits Small Dual Gauge schematic](pcb/simkits_small_dual_sch.jpg)

![Simkits Small Dual Gauge board layout](pcb/simkits_small_dual_pcb.jpg)

<!-- pagebreak -->

## Simkits Small Guage

Title block revision 2 (2022-08-04). Not covered by the sketches. Replacement board for a small Simkits gauge.

![Simkits Small Guage schematic](pcb/simkits_small_sch.jpg)

![Simkits Small Guage board layout](pcb/simkits_small_pcb.jpg)
