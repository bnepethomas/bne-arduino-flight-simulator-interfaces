# MSFSWinformsAutoCS — Program Summary

WinForms bridge for **Microsoft Flight Simulator (2020 / 2024)**. It is the SimConnect
counterpart of `FSUIPCWinformsAutoCS`: same window, same timers, same UDP endpoints and the
same real-unit packet per board, with the data read through SimConnect instead of FSUIPC
offsets. Written 2026-10-17; **built and the packet builder tested with sample values, but not
yet run against MSFS** (the simulator was not running when it was written).

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
| Dual Stepper `.106` | `PWR` (`1`, or `0` while the main bus is below 24 V: blanks that board's four OLEDs), `ALT`, `RPMR`, `RPME`, `ZULU` (HHMM), `OILT`, `OILP`, `XMSNT`, `XMSNP`, `FUELLOAD` (fuel pressure, psi), `ELECTRICALLOAD` (amps, as the FSUIPC bridge sends it), `TEMP` (outside air, e.g. `21C`) |
| OLED `.104` | `ALT`, `BARO` (inHg x 100 from the Kohlsman setting in millibars). Only these two - the FSUIPC bridge sends this board the whole shared packet, but it reads only `ALT` and `BARO` |
| Radio `.101` | `C1A`, `C1S`, `C2A`, `C2S` (MHz, three decimals), `MAINBUS` (volts), `NAVCOM1` (`1` when the **avionics bus is 24 V or more**, else `0`: below that the Radio blanks its COM/NAV displays) |

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

## Warning lamps

Only three can be derived from standard SimVars and are implemented, with the thresholds
`SimConnect_to_UDP` used: `EOUT` (turbine speed below 60 %), `RLOW` (rotor at or below
95 %) and `LOWF` (fuel 17 US gal or less). The other twelve are aircraft specific
(`DodoSim` supplies them through an FSUIPC offset, which MSFS aircraft do not have) and
stay off; add rules to `Payloads.LampState()` once the aircraft's variables are known.

## Known gaps / to check against MSFS

- Not yet run against MSFS: check the status bar goes green, the readout fills in, and a
  packet capture (or the boards' debug output) shows 10 packets per second per board.
- Several MSFS Jet Ranger add-ons do not report every SimVar (see the notes at the top of
  `SimConnect_to_UDP/Form1.cs`: transmission pressure / temperature read 0 on two of them;
  one reports a fixed rotor speed). A missing `ELECTRICAL AVIONICS BUS VOLTAGE` makes the
  Radio displays go off, as the 24 V rule reads 0.
- Pitch sign, the units of `ELECTRICAL TOTAL LOAD AMPS` against what the Dual Stepper
  expects for `ELECTRICALLOAD`, and the 24 V thresholds are carried over from the FSUIPC
  bridge and are not verified for MSFS aircraft.
- No hysteresis on the 20 V / 24 V thresholds.
- The FSUIPC bridge also listens on 27001; run only one of the two bridges at a time.
