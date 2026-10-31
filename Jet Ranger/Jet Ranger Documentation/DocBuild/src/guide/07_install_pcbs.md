# Installing the controller PCBs

These pictures are an aid for installing the controller boards. Each shows the **top (component) side** of a controller PCB as it is seen when the board is fitted, with only the board outline, the mounting holes, the pads and silkscreen outlines of the **external connectors**, the unlabelled round pads in rows and a double column around the edge (the Arduino Mega headers) and the outline of the Ethernet module. Every external connector is labelled with its reference (J5, SW2) and, where there is room, its name. **Pin 1 of each connector is the square pad**; on a ribbon-cable (IDC) header the small triangle also marks pin 1. Nothing else is printed, so the pictures are easy to read at the bench. They are drawn from the KiCad layout files by `export_install_views.py`, so they show the boards as they were on the day the document was built.

## Jet Ranger Radio Auto Pilot

Revision 1.4 (31 October 2026, with the Huey Warning Panel connectors J20 to J23), for the Radio controller (172.16.1.101). Connectors with the name left off in the picture (a row of gauge sockets, for example) are listed in the table.

![Jet Ranger Radio Auto Pilot, top side with the external connectors labelled](pcb/radio_auto_pilot_install.jpg)

| Label | Name | Type | Pins | Notes |
|---|---|---|---|---|
| J3 | AUDIO | IDC header | 20 (2 x 10) | Audio panel |
| J4 | 5V IN | DC jack | 3 | 5 V supply input (DC jack) |
| J7 | STEPPER IN 12-20VDC | DC jack | 3 | 12 to 20 V supply input (DC jack) |
| J8 | COMM NAV | IDC header | 34 (2 x 17) | COM and NAV panel |
| J12 | AUTOPILOT | IDC header | 34 (2 x 17) | Auto Pilot panel |
| J20 | HUEY SWITCHES | Molex KK-254 | 4 | Huey Warning Panel switches: GND, brightness / dim (A8), test (A9), reset (A10) |
| J21 | HUEY BK LIGHT | Molex KK-254 | 2 | Huey Warning Panel backlight: +12 V and the switched backlight |
| J22 | HUEY ANODES | Molex KK-254 | 8 | Huey Warning Panel LED anodes ANNODE_00 to ANNODE_07 |
| J23 | HUEY CATHODES | Molex KK-254 | 4 | Huey Warning Panel LED cathodes CATHODE_04 to CATHODE_07 (MAX7219 digits 4 to 7) |

<!-- pagebreak -->

## Jet Ranger Front Panel Servo

Revision 1.4, for the Servo controller (172.16.1.102). Connectors with the name left off in the picture (a row of gauge sockets, for example) are listed in the table.

![Jet Ranger Front Panel Servo, top side with the external connectors labelled](pcb/front_panel_servo_install.jpg)

| Label | Name | Type | Pins | Notes |
|---|---|---|---|---|
| J2 | 5V IN | DC jack | 3 | 5 V supply input (DC jack) |
| J3 | STEPPER IN 12-20VDC | DC jack | 3 | 12 to 20 V supply input (DC jack) |
| J4 | LHS ANNUC | IDC header | 16 (2 x 8) | Left hand annunciator panel |
| J5 | RHS ANNUC | IDC header | 16 (2 x 8) | Right hand annunciator panel |
| J6 | BK LIGHT | Molex KK-254 | 2 |  |
| J7 | BK LIGHT | Molex KK-254 | 2 |  |
| J8 | BK LIGHT | Molex KK-254 | 2 |  |
| J9 | BK LIGHT | Molex KK-254 | 2 |  |
| J10 | BK LIGHT | Molex KK-254 | 2 |  |
| J15 | BK LIGHT | Molex KK-254 | 2 |  |
| J16 | BK LIGHT | Molex KK-254 | 2 |  |
| J17 | BK LIGHT | Molex KK-254 | 2 |  |
| J18 | BK LIGHT | Molex KK-254 | 2 |  |
| J19 | BK LIGHT | Molex KK-254 | 2 |  |
| J20 | AIRSPEED | Pin header | 3 |  |
| J21 | EGT | Pin header | 3 |  |
| J22 | TURBINE SPEED | Pin header | 3 |  |
| J23 | RADAR ALT | Pin header | 3 |  |
| J24 | ENG TORQUE | Pin header | 3 |  |
| J25 | ROTOR SPEED | Pin header | 3 |  |
| J26 | VVI | Pin header | 3 |  |
| J27 | ENG OIL TEMP | Pin header | 3 |  |
| J28 | FUEL LEVEL | Pin header | 3 |  |
| J30 | GAS PRODUCER | Pin header | 3 |  |
| J31 | ENG OIL PRES | Pin header | 3 |  |
| J32 | XMSN OIL TEMP | Pin header | 3 |  |
| J33 | XMSN OIL PRESS | Pin header | 3 |  |
| J42 | PITCH | Pin header | 3 | Attitude indicator pitch servo |
| J43 | ROLL | Pin header | 3 | Attitude indicator bank (roll) servo |
| J44 | ELEC LOAD | Pin header | 3 |  |
| J45 | FUEL LOAD | Pin header | 3 |  |
| SW1 | TOT CLEAR | Molex KK-254 | 2 | TOT clear switch (D43), to ground when pressed |
| SW2 | FUEL VALVE | Molex KK-254 | 2 | Fuel valve switch (D48), to ground when on |
| SW3 | WARNING MUTE | Molex KK-254 | 2 | Warning mute switch (D47), to ground when pressed |

<!-- pagebreak -->

## Jet Ranger 13 Stepper

Revision 1.6, for the Stepper controller (172.16.1.105). Connectors with the name left off in the picture (a row of gauge sockets, for example) are listed in the table.

![Jet Ranger 13 Stepper, top side with the external connectors labelled](pcb/stepper_13_install.jpg)

| Label | Name | Type | Pins | Notes |
|---|---|---|---|---|
| J2 | 5V IN | DC jack | 3 | 5 V supply input (DC jack) |
| J3 | STEPPER IN 12-20VDC | DC jack | 3 | 12 to 20 V supply input (DC jack) |
| J4 | FUEL LEVEL | Molex KK-254 | 6 |  |
| J5 | VVI | Molex KK-254 | 6 |  |
| J20 | TURBINE SPEED | Molex KK-254 | 6 |  |
| J22 | ROTOR SPEED | Molex KK-254 | 6 |  |
| J23 | RADAR ALT | Molex KK-254 | 6 |  |
| J24 | GAS PRODUCER | Molex KK-254 | 6 |  |
| J25 | ENG TORQUE | Molex KK-254 | 6 |  |
| J26 | AIRSPEED | Molex KK-254 | 6 |  |
| J27 | EGT | Molex KK-254 | 6 |  |
| J30 | XMSN OIL TEMP | Molex KK-254 | 6 |  |
| J39 | XMSN OIL PRESS | Molex KK-254 | 6 |  |
| J40 | ENG OIL PRES | Molex KK-254 | 6 |  |
| J41 | ENG OIL TEMP | Molex KK-254 | 6 |  |

<!-- pagebreak -->

## Jet Ranger OLED Dual Stepper

Revision 1.5, for the Dual Stepper controller (172.16.1.106). Connectors with the name left off in the picture (a row of gauge sockets, for example) are listed in the table.

![Jet Ranger OLED Dual Stepper, top side with the external connectors labelled](pcb/oled_dual_stepper_install.jpg)

| Label | Name | Type | Pins | Notes |
|---|---|---|---|---|
| J2 | 5V IN | DC jack | 3 | 5 V supply input (DC jack) |
| J3 | STEPPER IN 12-20VDC | DC jack | 3 | 12 to 20 V supply input (DC jack) |
| J4 | FUEL LEVEL | Molex KK-254 | 6 |  |
| J5 | VVI | Molex KK-254 | 6 |  |
| J6 | XMSN | Molex KK-254 | 10 |  |
| J7 | ROTOR ENG SPEED | Molex KK-254 | 10 |  |
| J8 | ENGINE | Molex KK-254 | 10 |  |
| J9 | ALTIMETER ZERO | Molex KK-254 | 3 | Altimeter zero sensor |
| J10 | ELEC FUEL LOAD | Molex KK-254 | 10 |  |
| J17 | ALTIMETER-OLED | Molex KK-254 | 4 | Altimeter OLED |
| J18 | CLOCK-OLED | Molex KK-254 | 4 | Clock OLED |
| J19 | ALTIMETER OLED | Molex KK-254 | 6 |  |
| J20 | TEMPERATURE-OLED | Molex KK-254 | 4 | Temperature OLED |
| J26 | AIRSPEED | Molex KK-254 | 6 |  |
| J27 | EGT | Molex KK-254 | 6 |  |
| J33 | ALT | Molex KK-254 | 6 |  |
| J55 | BARO-OLED | Molex KK-254 | 4 | Barometer OLED |

<!-- pagebreak -->

## Jet Ranger Upper ADF Controller

Revision 1.0, for the ADF and Upper controller (172.16.1.107). Connectors with the name left off in the picture (a row of gauge sockets, for example) are listed in the table.

![Jet Ranger Upper ADF Controller, top side with the external connectors labelled](pcb/upper_adf_controller_install.jpg)

| Label | Name | Type | Pins | Notes |
|---|---|---|---|---|
| J4 | 5V IN | DC jack | 3 | 5 V supply input (DC jack) |
| J5 | ADF | IDC header | 26 (2 x 13) | ADF panel (knobs, keypad, switches, lamps) |
| J7 | STEPPER IN 12-20VDC | DC jack | 3 | 12 to 20 V supply input (DC jack) |
| J16 | GUAGE BRIGHT | Molex KK-254 | 3 | Panel brightness pot |
| J17 | WARNING MUTE | Molex KK-254 | 2 | Warning mute switch (matrix ROW8) |
| J18 | TOT TEST | Molex KK-254 | 2 | TOT test switch (matrix ROW8) |
| J20 | HYD BOOST | Molex KK-254 | 6 | Hyd boost, anti-ice and caution dim switches |
| J30 | ROWS | Molex KK-254 | 8 | Upper switch matrix rows ROW0 to ROW7 |
| J31 | COLS | Molex KK-254 | 8 | Upper switch matrix columns COL0 to COL7 |
| J32 | KEYPAD | Molex KK-254 | 8 | Upper numeric keypad |
| J33 | BACKLIGHT | Molex KK-254 | 2 | Panel backlight |

<!-- pagebreak -->
