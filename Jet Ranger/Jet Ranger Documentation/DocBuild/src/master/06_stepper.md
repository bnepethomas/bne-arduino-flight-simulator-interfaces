# Stepper controller (172.16.1.105)

`JET_RANGER_STEPPER_CONTROLLER` runs on an Arduino Mega 2560 with a W5500 Ethernet module, on the Jet Ranger 13 Stepper PCB. It drives X27-style stepper gauges directly from the Mega's pins (no driver chips) and takes **real units**, converting them to needle steps with calibration tables in the sketch. The oil, transmission, RPM, load and altimeter gauges are on the Dual Stepper board (next chapter).

The sketch started as an A-10C front instrument board and still carries some DCS-BIOS code and an unused SARI roll stepper claim on pins 30 and 32; the Jet Ranger path is the UDP protocol.

![Stepper controller blocks](diagrams/stepper_blocks.svg)

## Gauges and calibration

| Gauge | Token | Mega pins | Unit and range | Calibration |
|---|---|---|---|---|
| Airspeed | `IAS` | 12, 13, 22, 23 | knots 0 to 140, one decimal allowed | `IAS_KT_TABLE`, 10 points (0 to 635 steps) |
| Vertical speed | `VSI` | 7, 8, 9, 11 | ft/min -1750 to +1750 | `VSI_FPM_TABLE`, 11 points (-311 to +315 steps) |
| Radar altitude | `AGL` | 32 to 35 | feet 0 to 2500 | `AGL_FT_TABLE`, 11 points (0 to 469 steps) |
| Torque | `TQ` | 36 to 39 | percent 0 to 120 | `TQ_PCT_TABLE`, 2 points (0 to 600 steps) |
| Gas producer (N1) | `N1` | 40 to 43 | percent 0 to 103 | `GP_PCT_TABLE`, 3 points (0 percent is step 15) |
| Turbine out temperature | `ITT` | A11 to A14 | degrees C 100 to 900 | `EGT_C_TABLE`, 7 points (100 C is step 0) |
| Fuel quantity | `FUEL` | 2, 3, 4, 6 | US gallons 0 to 75 | `FA_GAL_TABLE`, 2 points (0 gal is step 53) |

Notes:

- The first IAS row (0 kt = step 0) is an assumed zero. The VSI and IAS steppers are wired C, D, A, B.
- Every token has a raw twin (`IASRAW:300`) that moves the needle by raw steps and skips the table.
- Four more steppers are declared with placeholder linear scales and no start-up swing: engine oil temperature (`OILT`, pins 48, A0 to A2), transmission oil temperature (`XMSNT`, A3 to A6), transmission oil pressure (`XMSNP`, A7 to A10) and engine oil pressure (`OILP`, 44 to 47). The oil gauges are really driven by the Dual Stepper; wire each physical gauge to one board only.
- `RPME` and `RPMR` packets sent to this board are ignored.
- **Zero trims:** each gauge has a `<CODE>ZeroOffset` value (steps) to nudge its true zero without touching its table. Current values: `AGLZeroOffset` 10, `TQZeroOffset` 23, all others 0. The trim is applied in each gauge's real-value setter, so a `<CODE>:0` packet, the watchdog reset and the end of each swing all land on the same trimmed position. The `*RAW` codes do not apply it.

## Program flow

1. **Setup:** flash the status LEDs, bring up Ethernet (10 ms retransmission timeout), ramp the backlight, then run each gauge's boot swing in this order, each behind its own `SwingXXX` gate: fuel quantity, torque, turbine out temperature, gas producer, VSI, airspeed, radar altitude. All seven gates are currently `true` (`SwingLoops` is 2). The 30 second watchdog is armed at the end of `setup()`.
2. **Main loop:** toggle the status LEDs, call `.run()` on every stepper, send the `STEPPER` heartbeat every 10 seconds, and every 5 ms check UDP 13136 and store new targets. Steppers move with smooth acceleration (maximum speed 9000, acceleration 1000).
3. **Watchdog:** after 30 seconds with no packet every gauge is driven to its calibrated zero through its own setter (trim included).

## Packets

The bridges send this board its own packet: `D,IAS:93.4,VSI:500,AGL:850,FUEL:42,TQ:65,ITT:610,N1:92`. `AGL` is zero below 20 V on the main bus. The fractional part of `IAS` is interpolated between table rows, so the needle moves smoothly rather than in whole-knot jumps.

## On-demand swings

Send `C,<COMMAND>:1` (the value is ignored) to run one gauge's swing without rebooting. The swing blocks the loop for about 10 seconds and re-homes the stepper, so the needle then rests at its calibrated zero until the next packet.

| Command | Gauge |
|---|---|
| `SWINGFUEL` | Fuel quantity |
| `SWINGTQ` | Torque |
| `SWINGITT` | Turbine out temperature |
| `SWINGN1` | Gas producer |
| `SWINGVSI` | Vertical speed |
| `SWINGIAS` | Airspeed |
| `SWINGAGL` | Radar altitude |

The airspeed and radar altitude swings return to zero after the full-scale wind-up instead of 635 steps past it.

`C,RESET:1` reboots the board through the AVR watchdog (bench-tested: "Ethernet Started" 6.6 seconds later, a normal boot, no loop).

## Pin usage

| Pin | Function |
|---|---|
| 14, 15 | Green and red status LEDs |
| 53 | W5500 reset |
| 5 | Backlight PWM (`BACK_LIGHTS`; the PCB routes `BACKLIGHT_PWM` to D5 and VSI coil B to D8) |
| 7, 8, 9, 11 | VSI coils |
| 12, 13, 22, 23 | Airspeed coils |
| 32 to 35 | Radar altitude coils |
| 36 to 39 | Torque coils |
| 40 to 43 | Gas producer coils |
| A11 to A14 | Turbine out temperature coils |
| 2, 3, 4, 6 | Fuel quantity coils |
| 48, A0 to A2 | Engine oil temperature coils |
| A3 to A6 | Transmission oil temperature coils |
| A7 to A10 | Transmission oil pressure coils |
| 44 to 47 | Engine oil pressure coils |
| 24 to 31 | Unused (Turbine and Rotor speed moved to the Dual Stepper) |

## Network

Static IP `172.16.1.105`, MAC `A8:61:0A:67:83:69`, data port 13136, heartbeat `STEPPER` to 172.16.1.10:13137, debug to 172.16.1.10:27000. The StepperVSITester can send every token and raw token to this board without a simulator.

## Known issues

- Pin 32 is claimed by both the radar altitude coil and the unused SARI roll stepper; pin A2 (56) is claimed by the engine oil temperature coil and the shared stepper enable pin. Neither SARI nor the enable pin is used.
- `VSIstepper.runToNewPosition(-X27_FULLWIRE_HOMING_STEPS)` expands to -635 + 5 = -630, not -640, because the macro is not parenthesised (about 1.5 percent).
- The placeholder oil and transmission gauges use a shared linear scale, not bench-measured tables.
