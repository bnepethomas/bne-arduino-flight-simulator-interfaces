# COM ports and flashing

Each Arduino is programmed over its own USB cable from the primary PC. The cockpit itself runs over Ethernet, so USB is only needed to upload a sketch.

## COM port to Arduino mapping

![Which board is on which COM port](diagrams/com_ports.svg)

| Board | Address | Hardware | COM port | Sketch |
|---|---|---|---|---|
| Radio controller | 172.16.1.101 | Arduino Mega 2560 (replacement board) | **COM20** | `JET_RANGER_RADIO_CONTROLLER` |
| Servo controller | 172.16.1.102 | Mega 2560 clone | **COM16** | `JET_RANGER_SERVO_CONTROLLER` |
| Upper controller | 172.16.1.103 | Mega 2560 | not recorded | `JET_RANGER_UPPER_CONTROLLER` |
| OLED controller | 172.16.1.104 | Mega 2560 | not recorded | `JET_RANGER_OLED_CONTROLLER` |
| Stepper controller | 172.16.1.105 | Mega 2560 | **COM4** | `JET_RANGER_STEPPER_CONTROLLER` |
| Dual stepper controller | 172.16.1.106 | Mega 2560 | **COM5** | `JET_RANGER_OLED_DUAL_STEPPER_CONTROLLER` |
| USB HID board | 172.16.1.11 | Arduino Leonardo | **COM8** | `UDP_TO_HID` |

These are the ports used the last time each board was flashed. A COM number belongs to a USB socket and a board together, so it **can change**:

- The Servo controller is a clone Mega that reports no USB serial number, so Windows ties its COM number to the USB socket it is plugged into. Plug it into a different socket and it appears as a new port.
- Replacing a Mega (the Radio controller was replaced once) gives a new port number.
- The Upper and OLED controllers were not connected when the ports were recorded.

Always check the port before an upload: open Device Manager, Ports (COM and LPT), or run `arduino-cli board list`. If you are not sure which board is which, unplug one USB cable and see which port disappears. Add the confirmed port to the table above (and to the sketch header comment) once you have uploaded to a board.

## Flashing a board

![Flashing a board](diagrams/flash_workflow.svg)

1. Close the Arduino IDE **serial monitor**. It holds the port, and an upload then fails with *Access is denied*.
2. Check the COM port (above).
3. Compile the sketch.
4. Upload to that port.
5. Watch the debug listener (UDP 27000) and the Health Monitor: the board prints *Ethernet Started* and *Setup Complete* and its light goes green within about 10 seconds.

```
arduino-cli compile --fqbn arduino:avr:mega "Jet Ranger Arduino Sketches\JET_RANGER_STEPPER_CONTROLLER"
arduino-cli upload -p COM4 --fqbn arduino:avr:mega "Jet Ranger Arduino Sketches\JET_RANGER_STEPPER_CONTROLLER"
```

For the Leonardo (USB HID board) use `--fqbn arduino:avr:leonardo` and its port, COM8. Flash only the board you are working on. If a board ever reboot-loops after a reset packet, power-cycle it and flash it again.

# PCBs and wiring

The Mega sits on a carrier PCB that breaks its pins out to servo headers, lamp outputs, gauge sockets and the Ethernet module. KiCad projects for every board are in `Jet Ranger/PCBs` in the repository, with Gerbers in each `MANUFACTURING` folder.

![Active PCBs](diagrams/pcb_map.svg)

| PCB | Used by | Main parts |
|---|---|---|
| Jet Ranger Front Panel Servo | Servo board | Mega shield, ULN2803A lamp drivers, servo headers, lamp outputs |
| Jet Ranger 13 Stepper | Stepper board | Mega shield, USR-ES1 Ethernet, 14 six-pin gauge sockets, 12 V lamp supply with polyfuses |
| Jet Ranger OLED Dual Stepper | Dual stepper board | Mega shield, USR-ES1, six-pin gauge sockets, TCA9548A sockets for the OLEDs |
| Jet Ranger X27 Small Stepper | Each stepper gauge | One X27 or VID29 stepper, backlight LED, six-pin connector to the controller |
| Jet Ranger Radio Auto Pilot | Radio controller | Radio controller board |
| Jet Ranger COM and NAV | Radio panel | 8 rotary encoders, push buttons, four frequency-swap switches |
| Jet Ranger Upper Controller | Upper controller | Mega shield for the switch matrix |

The X27 steppers run straight from the Mega's pins, so the stepper boards need no driver chips; the 12 V input feeds lamps and backlights.

**Version 1.4 of the OLED Dual Stepper PCB (12 October 2026)** separates Mega pins 2 and 32. Earlier versions joined them in copper because of a copy-and-paste net name, so the fuel quantity and fuel load gauges shared coil A. On a board older than 1.4 fit only one of those two gauges or cut the trace between the pins.

## Connections on the servo carrier board

| Connector | Count | Pins | Wiring |
|---|---|---|---|
| Servo headers | 16 on the PCB, 2 used | 26 (pitch), 27 (bank) | Signal, +V, GND; +V from the servo rail |
| Lamp outputs | 15 | A1 to A15 | Pin HIGH = lamp on; through a driver for lamps above 20 mA |
| Spare outputs | 3 | not assigned | Reserved for future lights |
| Ethernet module | 1 | SPI 50 to 52, chip select, reset 53 | Reset is pulsed by the firmware at start-up |
| Status LEDs | 2 | 14 green, 15 red | With series resistors to GND |
| Servo power input | 1 | | 5 to 6 V; ground joined to Mega GND |
| Front-panel switch inputs | 3 | D43, D47, D48 | Each to GND through a switch (SW1 TOT clear, SW3 warning mute, SW2 fuel valve); internal pull-up, debounced in firmware |

The rule that matters most: **never power servos from the Mega's 5 V pin.** Servos draw current spikes when they move, which can reset the Mega and drop the Ethernet link.

## Wiring checklist

- Servo rail fed from its own 5 to 6 V supply, not the Mega.
- Supply ground joined to Mega GND (signals need a shared reference).
- Each servo plugged in signal-side matching the header silkscreen.
- Lamps checked during the power-up lamp test.
- Ethernet cable to the same switch as the primary PC.
- Board serial number noted in the sketch header.

# Calibration

The servo board turns attitude degrees into a servo position (0 to 180) with two small tables in the sketch. The stepper boards do the same for steps. Between table points the boards interpolate in a straight line; beyond the ends they hold the end value.

| Instrument | Attitude | Servo position |
|---|---|---|
| Pitch (`PITCH_DEG_TABLE`) | -30 degrees | 179 |
| Pitch | 0 (level) | 113 |
| Pitch | +30 degrees | 70 |
| Bank (`BANK_DEG_TABLE`) | -90 degrees | 5 |
| Bank | 0 (level) | 93 |
| Bank | +90 degrees | 179 |

The self-test sweep uses `aServMinPosition[]` and `aServMaxPosition[]` (bank 5 to 179, pitch 166 to 70) and parks at `aServZeroPosition[]` (bank 93, pitch 113).

![The calibration loop](diagrams/calibration_flow.svg)

## Calibrating a servo

1. Send raw positions with `BANKRAW` or `PITCHRAW` (for example `D,PITCHRAW:113`) until the instrument shows a known attitude.
2. Note the position for each attitude you check: at least both ends and level.
3. Enter them in `PITCH_DEG_TABLE` or `BANK_DEG_TABLE`, and set `aServZeroPosition[]` to the level position so the self-test parks there.
4. Upload, then check with real values, for example `D,PITCH:0` and `D,BANK:30`.

## Calibrating a stepper gauge

1. Send raw steps with the `RAW` token (for example `D,TQRAW:300`) and read the dial.
2. Repeat at zero, mid scale and full scale (more points where the scale is not linear).
3. Edit the gauge's table in the sketch (`TQ_PCT_TABLE`, `IAS_KT_TABLE` and so on) and its `...ZeroOffset` trim if the zero is off.
4. Upload and check with real values (`D,TQ:60`).

The StepperVSITester sends every token and raw token without a simulator. Do not drive a gauge hard against its mechanical stop: a stalled servo or stepper draws high current and gets hot.
