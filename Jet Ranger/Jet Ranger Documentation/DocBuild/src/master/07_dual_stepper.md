# Dual stepper controller (172.16.1.106)

`JET_RANGER_OLED_DUAL_STEPPER_CONTROLLER` runs on a third Mega 2560, on the Jet Ranger OLED Dual Stepper PCB. It drives stepper gauges for engine and transmission oil, engine and rotor RPM, fuel load, electrical load and the altimeter, plus **four small OLED displays**. Like the Stepper board it takes real units. It started as a copy of the Stepper sketch, which is why the two share their structure, their zero-trim approach and most of their code.

![Dual stepper controller blocks](diagrams/dual_stepper_blocks.svg)

## Gauges and calibration

| Gauge | Token | Mega pins | Unit | Calibration |
|---|---|---|---|---|
| Engine oil pressure | `OILP` | 44 to 47 | psi | `EOP_PSI_TABLE`: 0, 50, 100, 150 psi = 0, 74, 148, 220 steps |
| Engine oil temperature | `OILT` | 48, A0 to A2 | degrees C | `EOT_C_TABLE`: 0, 50, 100, 150 C = 0, 79, 156, 231 steps |
| Transmission oil pressure | `XMSNP` | A7 to A10 | psi | `XOP_PSI_TABLE`: 0, 50, 70, 100, 150 psi = 0, 74, 104, 149, 220 steps |
| Transmission oil temperature | `XMSNT` | A3 to A6 | degrees C | `XOT_C_TABLE`: 0, 50, 100, 150 C = 0, 75, 150, 225 steps |
| Electrical load | `ELECTRICALLOAD` | 36 to 39 | percent | `ELECTRICAL_LOAD_PCT_TABLE`: 0 percent = step 22, 100 percent = step 200 |
| Fuel load | `FUELLOAD` | 32 to 35 | psi | `FUEL_LOAD_PSI_TABLE`: 0 psi = 0, 30 psi = 160 steps |
| Engine speed (N2) | `RPME` | 24 to 27 | percent, one decimal | `TS_PCT_TABLE`: 0, 55, 100, 110 percent = 0, 300, 535, 600 steps |
| Rotor speed | `RPMR` | 28 to 31 | percent, one decimal | `RS_PCT_TABLE`: the same values |
| Altimeter | `ALT` | 40 to 43, zero sensor A15 | feet | 720 steps per 1000 ft |

Turbine out temperature (`ITT`) and fuel quantity (`FUEL`) are also accepted, with the same tables as the Stepper board; wire each physical gauge to one board only. `VSI`, `IAS`, `AGL`, `TQ` and `N1` are not handled here.

- Every token has a raw twin (`FUELLOADRAW`, `RPMERAW`, `ALTRAW` and so on).
- The 0 percent point of the RPM tables is assumed, and the 0 point of the electrical-load table is step 22, not zero.
- **Zero trims (steps):** `TSoffset` 10, `RSoffset` 0, `EOTZeroOffset` 24, `EOPZeroOffset` 0, `XOTZeroOffset` 16, `XOPZeroOffset` 5, `FuelLoadZeroOffset` 55, `ElectricalLoadZeroOffset` 24. The altimeter has none.
- The altimeter seeks its zero sensor on A15 by moving up to 10,080 steps (about 14 turns) before giving up, then sets position -25 and sweeps one 720 step revolution. The altimeter's own speed is 600 steps per second.

## OLED displays

The diagram above shows the four displays on the multiplexer. Four 128 by 32 SSD1306 OLEDs share the Mega's I2C pins (20 SDA, 21 SCL) through a TCA9548A multiplexer at address 0x70. The firmware pulses the mux reset (pin 16) at start-up, calls `Wire.begin()` with a 25 ms timeout and probes the mux; if it does not answer the sketch logs "TCA mux NOT found - OLED init skipped" and starts the swings at once, then retries once after the swings.

| Display | Mux channel | Token | Shows |
|---|---|---|---|
| Barometric setting | 1 | none yet | `2992` at boot, not updated |
| Altimeter digits | 2 | `ALT` | Rolling-drum altitude, raw MSL (barometric correction ignored) |
| Clock | 3 | `ZULU` | Zulu time from `HHMM`, for example `ZULU:1430` shows `14:30` |
| Temperature | 4 | `TEMP` | The text as sent, for example `TEMP:25C` (up to 8 characters) |

Updates are gated on change and throttled to every 300 ms. A boot-time I2C scan (`OLED_I2C_SCAN`) logs one line per channel to the debug stream, for example `I2C scan ch1: 0x3c` or `I2C scan ch3: none`; the latest scan found OLEDs on channels 1 to 4. The TCA9548A address pins (D17 to D19) are not driven by the sketch: the module's own pull-downs set address 0x70, so keep a module with pull-downs.

**Power:** `PWR:0` (sent first in the bridge's packet while the main bus is below 24 V) blanks all four OLEDs and `PWR:1` redraws them. The altimeter needle keeps following altitude. The lamp test ignores `PWR`.

**Lamp test:** `C,OLEDTEST:1` fills all four OLEDs solid white and `C,OLEDTEST:0` redraws their default content and clears the change gates so live data repaints them. It is ignored (with a debug line) if the mux was not found.

## Backlight

Pin 5 (the PCB's `BACKLIGHT_PWM`) drives the panel backlights through the board's IRF540N MOSFET (`BACK_LIGHTS`). `applyBacklight()` sets the level from `BPWR`, the last `C,B:0-15` brightness command and the 30 second no-data watchdog (see the network chapter): the default is PWM 65; once a brightness command has been received the last commanded level returns whenever power returns; `BPWR:0` or 30 seconds with no packet (including after power-up with no PC feed) switches it off, and the next packet switches it back on. The backlight still starts at 128 during setup. The old DCS-BIOS brightness handlers still call `analogWrite()` directly but never run. Each change is logged (`Brightness 15 -> PWM 255`, `Backlight power off`, `Backlight restored - UDP data resumed`). Compiled at 58,312 bytes of flash (22%) and 79% RAM (1,720 bytes free); flashed to the Dual Stepper on COM5.

## Program flow

1. **Setup:** initialise the OLEDs first so they are up while the needles swing, then run each gauge's swing in this order, each behind its own `SwingXXX` gate (all currently `true`, `SwingLoops` 1): engine oil pressure, engine oil temperature, transmission oil pressure, transmission oil temperature, electrical load, fuel load, engine speed, rotor speed, altimeter. Start-up takes about 65 seconds. The 30 second watchdog is armed at the end of `setup()`.
2. **Main loop:** run the steppers, send the `DUAL_STEPPER` heartbeat every 10 seconds and check UDP 13136 every 5 ms.
3. **Watchdog:** after 30 seconds without a packet every needle returns to its calibrated zero and all OLEDs blank.

## Packets

Bridge packet: `D,PWR:1,BPWR:1,ALT:1200,RPMR:100.0,RPME:100.0,ZULU:1430,OILT:60,OILP:55,XMSNT:50,XMSNP:35,FUELLOAD:17,ELECTRICALLOAD:50,TEMP:21C`. Rotor and engine speed carry one decimal so the fraction reaches the table interpolation.

## On-demand commands

| Command | Action |
|---|---|
| `SWINGOILP`, `SWINGOILT`, `SWINGXMSNP`, `SWINGXMSNT` | Oil and transmission gauges (about 5 to 6 seconds each) |
| `SWINGELECTRICALLOAD`, `SWINGFUELLOAD`, `SWINGRPME`, `SWINGRPMR` | Load and RPM gauges (about 6 to 7 seconds each) |
| `SWINGALT` | Altimeter (re-finds the zero sensor) |
| `OLEDTEST:1` / `OLEDTEST:0` | OLED lamp test on and off |
| `B:0` to `B:15` | Backlight brightness |
| `RESET:1` | Watchdog reboot (bench-tested: "Ethernet Started" 6.7 seconds later and a normal boot in about 56 seconds) |

All are sent as `C,<COMMAND>:1`. The oil, transmission, load and RPM swings wind to the stop and return to the zero offset; the electrical-load swing returns to its 22 step zero.

## Pin usage

| Pin | Function |
|---|---|
| 14, 15 | Status LEDs |
| 16 | TCA9548A reset |
| 20, 21 | I2C SDA, SCL |
| 53 | W5500 reset |
| 5 | Backlight PWM (PCB `BACKLIGHT_PWM`) |
| 24 to 27 | Engine speed coils |
| 28 to 31 | Rotor speed coils |
| 32 to 35 | Fuel load coils |
| 36 to 39 | Electrical load coils |
| 40 to 43, A15 | Altimeter coils and zero sensor |
| 44 to 47, 48, A0 to A2 | Oil pressure and temperature coils |
| A3 to A10 | Transmission temperature and pressure coils |
| 2, 3, 4, 6, A11 to A14 | Fuel quantity and turbine out temperature coils |

## Network

Static IP `172.16.1.106`, MAC `A8:61:0A:67:83:6A`, data port 13136, heartbeat `DUAL_STEPPER` (a prefix distinct from the Stepper's `STEPPER`, so the Health Monitor can tell them apart) to 172.16.1.10:13137, debug to 172.16.1.10:27000 including the I2C scan.

## Known issues

- **RAM use is 79 percent** and the compiler warns "Low memory available". Avoid adding OLEDs or string-heavy code.
- The barometric OLED is initialised but never updated; nothing feeds it.
- The altimeter digits ignore barometric offset.
- Engine and rotor RPM tables assume 0 percent is 0 steps; `RSoffset` is not measured.
- The Clock OLED (channel 3) answered on the I2C scan but its display has not been confirmed by eye.
- **PCB version 1.4 (12 Oct 2026)** removes a short between Mega pins 2 and 32 (a copied net name joined fuel quantity coil A and fuel load coil A in copper). On a board older than 1.4 fit only one of those two gauges or cut the trace.
