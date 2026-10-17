# MSFSWinformsAutoCS — Program Summary

WinForms bridge for **Microsoft Flight Simulator (2020 / 2024)**. It is the SimConnect
counterpart of `FSUIPCWinformsAutoCS`: same window, same timers, same UDP endpoints and the
same real-unit packet per board, with the data read through SimConnect instead of FSUIPC
offsets. Written 2026-10-17 and since run against MSFS 2024.

## Files

| File | Role |
|---|---|
| `Form1.cs` | SimConnect connection, 100 ms main timer, on-screen readout, lamp list, UDP listener and the command handler |
| `Payloads.cs` | `SimSnapshot` (one sample of real-unit values) and `Payloads` (builds the packet for each board, the lamp rules and the power thresholds). No SimConnect or UI dependency, so it can be tested on its own |
| `Form1.Designer.cs`, `Program.cs` | Window layout (identical to the FSUIPC bridge) and entry point |
| `lib\Microsoft.FlightSimulator.SimConnect.dll`, `lib\SimConnect.dll` | Managed wrapper and native library copied from the MSFS 2024 SDK (`SimConnect SDK\lib`) so the project builds without the SDK path; `SimConnect.dll` is copied next to the exe |
| `MSFSWinformsAutoCS.csproj`, `.slnx` | `net10.0-windows10.0.19041.0`, WinForms, nullable enabled |

## Program flow

1. **Constructor** connects five UDP clients (port 13136) to the boards, builds the lamp
   list, starts the 1 s `timerConnection` and the UDP listener on 27001.
2. **`timerConnection_Tick`** tries `new SimConnect("MSFS Winforms Bridge", Handle, WM_USER_SIMCONNECT, ...)`.
   While MSFS is not running this throws and the timer simply tries again. On success it
   registers the data definition (`PanelVars` + the `PanelData` struct), requests it every
   2nd sim frame, maps the client events, then starts `timerMain` and stops itself.
3. **`DefWndProc`** passes `WM_USER_SIMCONNECT` to `simconnect.ReceiveMessage()`; the data
   callback only stores the newest `PanelData` sample. If MSFS quits (`OnRecvQuit`) or the
   connection throws, the app closes SimConnect, stops `timerMain` and goes back to step 2.
4. **`timerMain_Tick`** (100 ms) converts the newest sample to a `SimSnapshot`, updates the
   readout, lamp list and Avionics Master box, then sends:
   - the **Radio** packet when a frequency, the main bus voltage or the radio power flag
     changed (at most every 200 ms), or every 5 s regardless;
   - one packet each to the **Servo, Stepper, Dual Stepper and OLED** boards about every
     100 ms (about 10 per second per board).
   If no new data has arrived for 3 s (simulator paused or loading) it stops sending, so
   the boards' 30 s no-data watchdogs return the gauges to zero; the status bar says so.
5. **`UpdateRadios`** (UDP 27001, from the Radio / Upper controller) is the same command set
   as the FSUIPC bridge, sent as SimConnect client events.

## Packets (`Payloads.cs`)

| Board | Packet |
|---|---|
| Servo `.102` | `PITCH` (degrees, one decimal, negated: the sim reports nose-down positive), `BANK`, and the 15 lamp codes as `:1`/`:0` (all `0` while the main bus is below 20 V) |
| Stepper `.105` | `IAS` (knots, one decimal), `VSI` (fpm), `AGL` (feet, `0` below 20 V), `FUEL` (US gal), `TQ` (%), `ITT` (deg C), `N1` (%) |
| Dual Stepper `.106` | `PWR` (`1`, or `0` while the main bus is below 24 V: blanks that board's four OLEDs), `ALT`, `RPMR`, `RPME`, `ZULU` (HHMM), `OILT`, `OILP`, `XMSNT`, `XMSNP`, `FUELLOAD` (fuel pressure, psi), `ELECTRICALLOAD` (percent: the magnitude of `ELECTRICAL TOTAL LOAD AMPS` / 300 A - the sign goes negative while the battery supplies the bus, e.g. with the engine stopped - `ElectricalLoadFullScaleAmps`; a guess from one reading, the FlyInside B206's 143 A with its cockpit LOAD gauge at about 50 %), `TEMP` (outside air, e.g. `21C`) |
| OLED `.104` | `ALT`, `BARO` (inHg x 100 from the Kohlsman setting in millibars). Only these two - the FSUIPC bridge sends this board the whole shared packet, but it reads only `ALT` and `BARO` |
| Radio `.101` | `C1A`, `C1S`, `C2A`, `C2S` (MHz, three decimals), `MAINBUS` (volts), `NAVCOM1` (`1` when the **avionics bus is 24 V or more**, else `0`: below that the Radio blanks its COM/NAV displays; if the aircraft reports the avionics bus as exactly 0, as the FlyInside B206 does, the **main bus** is used instead) |

## SimVars requested

`PLANE ALTITUDE`, `AIRSPEED INDICATED`, `VERTICAL SPEED`, `PLANE ALT ABOVE GROUND`,
`ATTITUDE INDICATOR PITCH/BANK DEGREES`, `ROTOR RPM PCT:1`, `GENERAL ENG PCT MAX RPM:1`,
`ENG TORQUE PERCENT:1`, `ELECTRICAL TOTAL LOAD AMPS`, `TURB ENG ITT:1`,
`ENG OIL TEMPERATURE:1`, `FUEL TOTAL QUANTITY`, `TURB ENG CORRECTED N1:1`,
`ENG OIL PRESSURE:1`, `ENG TRANSMISSION PRESSURE:1`, `ENG TRANSMISSION TEMPERATURE:1`,
`GENERAL ENG FUEL PRESSURE:1`, `ELECTRICAL MAIN BUS VOLTAGE`, `ELECTRICAL AVIONICS BUS VOLTAGE`,
`AVIONICS MASTER SWITCH`, `AMBIENT TEMPERATURE`, `KOHLSMAN SETTING MB`, `ZULU TIME`, and the
COM 1/2 and NAV 1/2 active and standby frequencies. `PanelVars` and the `PanelData` struct
must list them in the same order; `CheckDefinitionMatchesStruct()` checks the count at connect.

## Commands from the panel (UDP 27001)

`COM1_RADIO_SWAP`, `COM2_RADIO_SWAP`, `NAV1_RADIO_SWAP`, `NAV2_RADIO_SWAP`;
`COM2_STANDBY:`, `NAV1_STANDBY:`, `NAV2_STANDBY:` plus a frequency (range-checked, sent as
`COM2_STBY_RADIO_SET_HZ` / `NAV1_STBY_SET_HZ` / `NAV2_STBY_SET_HZ`); a bare number is the
COM1 standby frequency (`COM_STBY_RADIO_SET_HZ`; out-of-range or unreadable becomes 121.50);
`AVIONICS_MASTER_SET`, `MASTER_BATTERY_ON/OFF` (`MASTER_BATTERY_SET` 1/0) and
`ALTERNATOR_ON/OFF` (`ALTERNATOR_SET` 1/0). Unlike FSUIPC, SimConnect has direct set events
for battery and alternator, so there is no voltage guard or blind toggle.

## Aircraft-specific L:vars

The FlyInside B206 does not feed the standard rotor / engine speed SimVars (they read 0 and a
slowly decaying idle while the cockpit shows 100 %). Found with Developer Mode > Tools >
Behaviors > Variables (Global tab, filter `Aircraft.`), the cockpit uses
`L:Aircraft.Rotor.RPM.Percent` (rotor, percent) and `L:Aircraft.Engine.1.Turbine.N2.Smooth`
(turbine, percent). The aircraft is recognised by the turbine L:var being above zero, or by `FlyInside` in the aircraft title (so
it is still recognised with the engine stopped). The bridge reads both L:vars and uses them when they are above zero, otherwise
`ROTOR RPM PCT:1` and `GENERAL ENG PCT MAX RPM:1`, so other aircraft are unaffected. They feed
`RPMR` / `RPME` and the `RLOW` / `EOUT` lamps. No gas producer variable could be found
(the standard N1 reads about 52 % while the cockpit shows 92 %), so while the turbine L:var is
available `N1` is that value x 0.92 (`FlyInsideGasProducerScale`, from one reading: N2 100 gives
92; not checked at other powers). The turbine out temperature (`ITT`) is estimated too: the standard `TURB ENG ITT:1` reads about 540 C while
the cockpit dial shows 600 at idle, and the dial rises with power (about 650-660 at torque 74 %, about 700 at
115 %, read from screenshots). So `ITT` is turbine N2 x 6 plus 1.05 C per torque percent above the idle torque
of 21 (`FlyInsideTurbineOutTemp()`); three rough readings only.
Fuel load (`FUELLOAD`, sent as psi from the fuel pressure): the cockpit FUEL dial is a 0-3 scale marked x10 with
the needle at about 1.7-2 while `GENERAL ENG FUEL PRESSURE:1` reads 1.7, so the SimVar is in dial units and
`FUELLOAD` is 10 x it (`FlyInsideFuelPressureScale`; the Dual Stepper's table runs to 30 psi). One dial reading.

Transmission oil (`XMSNP`, `XMSNT`): no variable exists (the standard SimVars read 0; the XMSN OIL dial
showed about 35 psi and about 10-20 C), so both are estimates. Pressure is 35 psi x the rotor speed fraction
(`FlyInsideTransmissionPressure`). Temperature starts at the outside air temperature and moves slowly (5 min warming,
10 min cooling time constants) toward ambient + 0.8 C per torque percent while the rotor is above 50 % (about 33 C at
idle, 75 at 74 % torque; `UpdateTransmissionTemp`); it restarts from ambient when the bridge restarts. Neither is
read from the aircraft: replace them if an L:var is found (search the Behaviors Global tab for `xmsn`, `trans`, `gear`).

Torque (`TQ`): `ENG TORQUE PERCENT:1` reads 0. The cockpit dial follows the collective immediately, while
the sim's torque variables lag it by several seconds, and `COLLECTIVE POSITION` reads 0 too. With the
turbine L:var at 95 or more (rotor at normal speed) `TQ` is `21 + 0.72 x` `GENERAL ENG THROTTLE LEVER
POSITION:1` (on the test setup the collective axis arrives as that variable; dial about 21 % at 0 and about 74 %
steady at 73.6) up to lever 73.6, then a steeper line (about 1.74 per lever percent) to 120 % at lever 100
(`FlyInsideTorqueLeverKnee`; the dial read about 100 % at lever 89; clamped to 0-120, `FlyInsideTorqueMax`). Below that speed it is estimated from `TURB ENG MAX TORQUE PERCENT:1`
through two readings (raw 5 gives 20, raw 27.5 gives 70; `FlyInsideTorque()`). Full collective for more than
about 2 s over-torques and breaks the engine in this aircraft (the dial swung to about 115 %). The lever
mapping depends on the controller; only two steady readings back the 0.72 slope.
Transmission and the other lamp variables have not been found yet.

## CowaSim Jet Ranger estimates (2026-10-18)

The CowaSim Jet Ranger (aircraft title `206B3 ...`) reports the standard SimVars, but three of its cockpit dials
do not read them, so the bridge applies these estimates to it (`cowaSim` in `ToSnapshot`). They were fitted to dial
readings taken from screenshots during a fast collective sweep (the aircraft then crashed), so they are rough:

- **Torque (`TQ`):** the dial is a smoothed copy of `ENG TORQUE PERCENT:1`: about 0.42 x the SimVar (dial about 38 at the
  steady idle value of 83, about 120 at a peak of about 260; 0.48 from the sweep fit, lowered to 0.42 after a flight check
  read about 15 % high) with a smoothing time constant of about 2.5 s (`CowaSimTorqueScale`,
  `CowaSimTorqueSmoothSeconds`, `SmoothCowaSimTorque()`); RMS error about 18 points on a signal swinging between 0 and 120.
- **Turbine out temperature (`ITT`):** the dial follows `ENG EXHAUST GAS TEMPERATURE:1`, not ITT (which reads over 2,000):
  EGT - 200 C (`CowaSimTurbineOutTempOffset`), within about 30 C over nine readings.
- **Electrical load (`ELECTRICALLOAD`):** the LOAD dial sat at about 50 % while the total load was 69.5 A, so 139 A is
  100 % for this aircraft (`CowaSimElectricalLoadFullScaleAmps`; 300 A for others). One reading.
- **Transmission oil:** no variable; pressure is 48 psi x the rotor speed fraction (the dial read about 45-50 psi,
  `CowaSimTransmissionPressureNormal`) and the temperature is 0.92 x the engine oil temperature (`CowaSimTransmissionTempOfOilTemp`): the dial read about
  55 C with the engine oil at 60 C shortly after start and did not follow a slow warm-up. One reading.
- Gas producer, engine oil, fuel pressure, rotor and engine speed matched the dials without any change.

The warning lamp for transmission oil pressure uses the estimate for this aircraft as for the FlyInside.

## Warning lamps

Five are implemented. Three can be derived from standard SimVars, with these thresholds: `EOUT` (turbine speed below 60 %), `RLOW` (rotor at or below
95 %) and `LOWF` (fuel 17 US gal or less). `TOPW` (transmission oil pressure) comes on below 40 psi
(`TransmissionPressureWarningPsi`), only when a pressure is available: the FlyInside B206's estimate (35 psi at
normal rotor speed, so the lamp is on all the time with the current estimate, and when the rotor stops) or an
aircraft whose SimVar reads above 0 (most read 0, which would keep it on). `GENF` (generator fail) comes on when the
engine speed is below 10 % (`GeneratorFailEngineRpmPct`). The other ten are aircraft specific
(`DodoSim` supplies them through an FSUIPC offset, which MSFS aircraft do not have) and
stay off; add rules to `Payloads.LampState()` once the aircraft's variables are known.

## Known gaps / to check against MSFS

- Run against MSFS 2024 with the FlyInside B206 and the CowaSim Jet Ranger; the estimates
  above come from those sessions and have not been checked at other power settings.
- Several MSFS Jet Ranger add-ons do not report every SimVar (transmission pressure / temperature read 0 on
  two of them; one reports a fixed rotor speed). A missing `ELECTRICAL AVIONICS BUS VOLTAGE` (reads 0) falls back
  to the main bus for the Radio power rule.
- Pitch sign, and the 24 V thresholds are carried over from the FSUIPC
  bridge and are not verified for MSFS aircraft.
- No hysteresis on the 20 V / 24 V thresholds.
- The FSUIPC bridge also listens on 27001; run only one of the two bridges at a time.
