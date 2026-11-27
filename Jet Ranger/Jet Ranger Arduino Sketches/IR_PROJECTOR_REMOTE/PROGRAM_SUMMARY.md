# IR_PROJECTOR_REMOTE — Program Summary

Replaces a projector's infrared remote: an Arduino sends the remote's codes through two IR LEDs when a push button is pressed,
the projector power switch is moved, or a command arrives on the serial port. Added 27 November 2026. It is driven over USB
serial, buttons and the switch; no network side is needed.

## Hardware

An Arduino Uno with the **IR Projector Remote** board (KiCad project in `Jet Ranger/PCBs/IR Projector Remote`), or the same
wiring by hand:

| Pin | Connection |
|---|---|
| D3 | 1 k to the base of a BC547; emitter to GND; collector to the cathodes of both IR LEDs. Each LED anode has its own 100 ohm resistor to 5 V (about 34 mA each, 68 mA through the transistor) |
| A5 | Projector power switch (SPST) to GND |
| D7 | Status LED (3 mm) through 330 ohm to GND |
| A0, A1, A2, A3, D5, D6 | Push buttons to GND: POWER, UP, DOWN, LEFT, OK, RIGHT |

All inputs use the internal pull-ups, so no resistors are needed on the buttons or the switch. A4 is free.

## The board

| Connector | Pins |
|---|---|
| J1, Molex KK 254, 3 way (IR LEDs) | 1 = LED 1 anode, 2 = both cathodes (collector), 3 = LED 2 anode |
| J2, Molex KK 254, 2 way (power switch) | 1 = A5, 2 = GND |
| J3, 2.54 mm header, 10 way (test) | 1 GND, 2 POWER (A0), 3 UP (A1), 4 DOWN (A2), 5 LEFT (A3), 6 OK (D5), 7 RIGHT (D6), 8 switch (A5), 9 D3 (drive to the BC547 base resistor), 10 GND |

D1 is the status LED. The push buttons connect to J3, between their pin and a GND pin.

The board sits **on top of the Uno's headers**, component side up, on male headers soldered to it and pointing down; the Uno
hangs underneath. Seen from above, the USB socket is at the left, the digital pins run along the top and the power and analogue
pins along the bottom. The board's left edge stops 3 mm inboard of the back of the USB socket (taken as 16 mm deep), so it does
not hang over the socket, and its right edge lines up with the back of the Uno's board, with a notch (7.8 x 10.6 mm) around the
Uno's 6 way ICSP header. The four 3.2 mm (M3) mounting holes are outside the Uno's outline, above and below it near each end:
the assembly is mounted by this board, so the standoffs must be taller than the headers and the Uno together (allow 20 mm or
more). The board is 55.9 x 71.3 mm, two layers, with no copper pours: ground is routed as tracks like the other nets, so the layout is easy to change by hand.

The footprint `IR_Remote:Arduino_UNO_Shield` is the project's own: KiCad's `Module:Arduino_UNO_R3` has the two header rows the
other way up (it only fits with the Uno on the component side), so the pads were mirrored. The first version of this board used
the stock rows and was wrong.

Checked in KiCad 9.0.7: electrical rules check and design rules check (with schematic parity) report no violations and no
unconnected pads. Routed with Freerouting 2.1.0. Gerbers have not been generated and the board has not been made.

## Power switch

Switch **closed** (on) sends `STARTUP`; switch **opened** (off) sends `SHUTDOWN`. A command is sent only when the switch
changes (debounced, 30 ms); its position at power-up sends nothing. A change made while a sequence is running waits for it to
finish, then follows it; if the switch is put back before then, nothing is sent. `CANCEL` also drops a waiting change.

## Push buttons

Debounced without blocking (30 ms). The code is sent once when a button goes down; holding it does not repeat it. A button
already held at start-up sends nothing.

## Status

Compiled for `arduino:avr:uno` (6,220 bytes flash, 704 bytes RAM) with the power switch and status LED, and flashed to an Uno (COM3) on 28 November 2026. Not yet tried against the projector or on the new board.
