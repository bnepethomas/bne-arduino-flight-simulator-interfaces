# MSFS bridge (MSFSWinformsAutoCS)

`MSFSWinformsAutoCS` is the bridge for Microsoft Flight Simulator 2020 and 2024. It has the same window, the same timers, the same UDP endpoints and the same real-unit packet per board as the FSUIPC bridge, but reads the data through **SimConnect**. For aircraft whose cockpit dials do not read the standard simulator variables it applies estimates fitted to dial readings (a FlyInside B206 profile and a CowaSim Jet Ranger profile).

![MSFS bridge data flow](diagrams/msfs_bridge.svg)

## Files

| File | Role |
|---|---|
| `Form1.cs` | SimConnect connection, 100 ms main timer, on-screen readout, lamp list, UDP listener and command handler |
| `Payloads.cs` | `SimSnapshot` (one sample of real-unit values) and `Payloads` (packet builders, lamp rules, power thresholds); no SimConnect or window code, so it can be tested on its own |
| `Form1.Designer.cs`, `Program.cs` | Window layout (identical to the FSUIPC bridge) and entry point |
| `lib\Microsoft.FlightSimulator.SimConnect.dll`, `lib\SimConnect.dll` | Managed wrapper and native library copied from the MSFS 2024 SDK so the project builds without the SDK path |
| `MSFSWinformsAutoCS.csproj` | `net10.0-windows10.0.19041.0`, WinForms |

## Program flow

1. **Constructor:** connect the UDP clients (port 13136), build the lamp list, start the 1 second `timerConnection` and the UDP listener on 27001.
2. **`timerConnection_Tick`:** try `new SimConnect("MSFS Winforms Bridge", Handle, WM_USER_SIMCONNECT, ...)`. While MSFS is not running this throws and the timer tries again. On success it registers the data definition, requests it every second sim frame, maps the client events, starts `timerMain` and stops itself.
3. **`DefWndProc`** passes `WM_USER_SIMCONNECT` to `ReceiveMessage()`; the data callback only stores the newest sample. If MSFS quits or the connection throws, the app closes SimConnect, stops `timerMain` and goes back to step 2.
4. **`timerMain_Tick`** (100 ms) converts the newest sample to a `SimSnapshot`, updates the readout, lamp list and Avionics Master box, then sends the packets: Radio when a frequency, the main bus voltage or the radio power flag changed (at most every 200 ms) or every 5 seconds; one packet each to the Servo, Stepper and Dual Stepper boards about every 100 ms.
5. **`UpdateRadios`** (UDP 27001) handles the Radio controller's commands as SimConnect client events.

![Connection states](diagrams/msfs_states.svg)

If no new data has arrived for 3 seconds (paused or loading) the bridge stops sending, so the boards' 30 second watchdogs return the gauges to zero, and the status bar says so. The status bar also shows the aircraft title and follows it when a different aircraft is loaded.

## Packets

| Board | Packet |
|---|---|
| Servo .102 | `BPWR` (1 while the main bus is 20 V or more), `PITCH` (degrees, one decimal, negated: the simulator reports nose-down positive), `BANK`, and the 15 lamp codes (all `0` while the main bus is below 20 V) |
| Stepper .105 | `BPWR`, `IAS` (knots, one decimal), `VSI`, `AGL` (feet, 0 below 20 V), `FUEL` (US gal), `TQ`, `ITT` (degrees C), `N1` |
| Dual Stepper .106 | `PWR` (1, or 0 while the main bus is below 24 V), `BPWR`, `ALT`, `RPMR`, `RPME`, `ZULU`, `OILT`, `OILP`, `XMSNT`, `XMSNP`, `FUELLOAD` (fuel pressure, psi), `ELECTRICALLOAD` (percent, below), `TEMP` |
| ADF .107 and Upper .103 | `BPWR` only (`Payloads.Adf()`, `D,BPWR:1/0`, sent to `adfClient` and `upperClient` with the other boards' packets); it also feeds their 30 second no-data timeout |
| Radio .101 | `C1A`, `C1S`, `C2A`, `C2S`, `MAINBUS`, `NAVCOM1`, `BPWR` (backlight power, 1 while the main bus is 20 V or more), the 19 `LED_xxx` tokens and the five Auto Pilot display values `APHDG`, `APCRS`, `APIAS`, `APVS` and `APALT` (see above) (`NAVCOM1` is 1 when the avionics bus is 24 V or more; if the aircraft reports the avionics bus as exactly 0, as the FlyInside B206 does, the main bus is used instead) |

`ELECTRICALLOAD` is the magnitude of `ELECTRICAL TOTAL LOAD AMPS` divided by 300 A (the sign goes negative while the battery supplies the bus, for example with the engine stopped); the CowaSim profile uses 139 A.

## Aircraft information for the GPS interface

Every 500 ms the main timer sends `D,LAT:..,LON:..,ALT:..,IAS:..,HDG:..,UTC:yyyyMMddHHmmss.fff` (`Payloads.Gps()`) to the Lowrance GPS interface (`127.0.0.1:27002`; change `GpsHost` and `GpsPort` in `Form1.cs` if it runs on another PC). Three SimVars were added at the end of the data definition and the `PanelData` struct, in the same order: `PLANE LATITUDE`, `PLANE LONGITUDE` (degrees) and `PLANE HEADING DEGREES TRUE`, plus `ZULU YEAR`, `ZULU MONTH OF YEAR` and `ZULU DAY OF MONTH`. Altitude (`PLANE ALTITUDE`, feet MSL) and airspeed (`AIRSPEED INDICATED`) were already read. The simulator's UTC time is built from the already-read `ZULU TIME` (seconds since midnight) and the three date SimVars, and is left out if the date is invalid. The readout shows the position and heading. Nothing is sent while the data is stale, so the GPS interface falls back to Brisbane Airport after 30 seconds. The packet builder was checked with sample values and read back by the GPS interface's parser; **not yet run against MSFS**.

## SimVars requested

`PLANE ALTITUDE`, `AIRSPEED INDICATED`, `VERTICAL SPEED`, `PLANE ALT ABOVE GROUND`, `ATTITUDE INDICATOR PITCH` and `BANK DEGREES`, `ROTOR RPM PCT:1`, `GENERAL ENG PCT MAX RPM:1`, `ENG TORQUE PERCENT:1`, `TURB ENG MAX TORQUE PERCENT:1`, `ENG EXHAUST GAS TEMPERATURE:1`, `GENERAL ENG THROTTLE LEVER POSITION:1`, `ELECTRICAL TOTAL LOAD AMPS`, `TURB ENG ITT:1`, `ENG OIL TEMPERATURE:1`, `FUEL TOTAL QUANTITY`, `TURB ENG CORRECTED N1:1`, `ENG OIL PRESSURE:1`, `ENG TRANSMISSION PRESSURE:1`, `ENG TRANSMISSION TEMPERATURE:1`, `GENERAL ENG FUEL PRESSURE:1`, `ELECTRICAL MAIN BUS VOLTAGE`, `ELECTRICAL AVIONICS BUS VOLTAGE`, `AVIONICS MASTER SWITCH`, `AMBIENT TEMPERATURE`, `KOHLSMAN SETTING MB` (shown in the readout only), `ZULU TIME`, the COM 1, COM 2, NAV 1 and NAV 2 active and standby frequencies, the two FlyInside L:vars and the six GPS SimVars above. `PanelVars` and the `PanelData` struct must list them in the same order; `CheckDefinitionMatchesStruct()` checks the count at connect.

## Autopilot and audio panel LEDs

The 19 LEDs on the Auto Pilot and Audio panels are read through **two extra SimConnect requests** (`PanelAp` and `PanelAudio`, every 6th frame), separate from the main gauge data, so a SimVar the simulator does not know can only take these LEDs down, not the gauges. `PanelAp`: `AUTOPILOT MASTER`, `AUTOPILOT NAV1 LOCK`, `AUTOPILOT HEADING LOCK`, `AUTOPILOT ALTITUDE LOCK`, `AUTOPILOT AIRSPEED HOLD`, `AUTOPILOT APPROACH HOLD`, `AUTOPILOT BACKCOURSE HOLD`, `AUTOPILOT FLIGHT DIRECTOR ACTIVE`. `PanelAudio`: `COM TRANSMIT:1` and `:2`, `COM RECEIVE ALL`, `NAV SOUND:1` and `:2`, `DME SOUND`, `ADF SOUND`, `MARKER SOUND`, `OUTER MARKER`, `MIDDLE MARKER`, `INNER MARKER`. They become the `LED_xxx` tokens of the radio packet (`Payloads.LedTokens()`, all 0 while the radio bus is below 24 V). `ADF SOUND` could not be confirmed to exist in MSFS; if it does not, the audio LEDs stay off and the autopilot LEDs are unaffected.

## Auto Pilot display values

The five values on the Auto Pilot panel's OLED are read through a third extra SimConnect request (`PanelApValues`, every 6th frame), again so an unknown SimVar cannot take the gauge data down: `AUTOPILOT HEADING LOCK DIR`, `NAV OBS:1` (the course), `AUTOPILOT AIRSPEED HOLD VAR`, `AUTOPILOT VERTICAL HOLD VAR` and `AUTOPILOT ALTITUDE LOCK VAR`. They become the tokens `APHDG`, `APCRS`, `APIAS`, `APVS` and `APALT` of the radio packet (`Payloads.ApValueTokens()`: heading and course as three digits with north as 360, the others as whole numbers).

## Commands from the panel (UDP 27001)

The Auto Pilot, Audio and ADF panel controls (`AP_MASTER`, `AP_HDG_HOLD`, `HEADING_BUG_INC`, `COM1_TRANSMIT_SELECT`, `GEAR_DOWN`, `PARKING_BRAKES` and the others listed in the FSUIPC bridge chapter) are mapped to client events of the same name when SimConnect opens (`PanelEventNames`, event numbers from 1000) and sent by `TryPanelCommand()`, which is checked first; a `:N` suffix repeats the event N times. The ADF panel's toggle switches send their positions as `GEAR_SET:1/0`, `FLAPS_SET:1/0`, `SPOILERS_SET:1/0` and `ANTI_ICE_SET:1/0` (on every change and every 5 seconds), which `TryPositionCommand()` sends as the SimConnect events `GEAR_SET` (0 or 1), `FLAPS_SET` and `SPOILERS_SET` (0 or 16383) and `ANTI_ICE_SET` (0 or 1), event numbers from 2000, so the simulator follows the switch instead of toggling.

`COM1_RADIO_SWAP`, `COM2_RADIO_SWAP`, `NAV1_RADIO_SWAP`, `NAV2_RADIO_SWAP`; `COM2_STANDBY:`, `NAV1_STANDBY:`, `NAV2_STANDBY:` plus a range-checked frequency (sent as `COM2_STBY_RADIO_SET_HZ`, `NAV1_STBY_SET_HZ`, `NAV2_STBY_SET_HZ`); a bare number is the COM1 standby frequency (an out-of-range or unreadable value becomes 121.50); `AVIONICS_MASTER_SET`; `MASTER_BATTERY_ON` and `_OFF` (`MASTER_BATTERY_SET` 1 or 0); `ALTERNATOR_ON` and `_OFF` (`ALTERNATOR_SET`). Unlike FSUIPC, SimConnect has direct set events, so there is no voltage guard or blind toggle.

## Aircraft profiles

![Which values come from where](diagrams/aircraft_profiles.svg)

Other aircraft use the standard SimVars unchanged. Estimates were fitted to a few dial readings taken from screenshots and may be a few percent off at power settings that were not checked.

### FlyInside B206

The FlyInside B206 does not feed the standard rotor and engine speed SimVars (they read 0 and a slowly decaying idle while the cockpit shows 100 percent). Found with Developer Mode, Tools, Behaviors, Variables (Global tab, filter `Aircraft.`): the cockpit uses `L:Aircraft.Rotor.RPM.Percent` and `L:Aircraft.Engine.1.Turbine.N2.Smooth`. The aircraft is recognised by the turbine L:var being above zero or by `FlyInside` in the aircraft title. The bridge uses the L:vars when above zero, otherwise the standard SimVars. They feed `RPMR`, `RPME` and the `RLOW` and `EOUT` lamps.

| Value | Estimate |
|---|---|
| Gas producer (`N1`) | 0.92 x turbine N2 (no variable found; the standard N1 reads about 52 while the cockpit shows 92) |
| Torque (`TQ`) | `ENG TORQUE PERCENT:1` reads 0. With the turbine at 95 or more: 21 + 0.72 x collective lever up to lever 73.6 (dial 74), then about 1.74 per lever percent to 120 at lever 100, clamped 0 to 120. Below that speed it is estimated from `TURB ENG MAX TORQUE PERCENT:1` through two readings |
| Turbine out temperature (`ITT`) | 6 x turbine N2 + 1.05 per torque percent above the idle torque of 21 (the standard value reads about 540 while the dial shows 600) |
| Fuel load (`FUELLOAD`) | 10 x `GENERAL ENG FUEL PRESSURE:1` (the cockpit dial is a 0 to 3 scale marked x10) |
| Transmission pressure (`XMSNP`) | 35 psi x the rotor speed fraction |
| Transmission temperature (`XMSNT`) | Starts at outside air temperature and moves slowly (5 minute warm-up, 10 minute cool-down) toward ambient + 0.8 per torque percent while the rotor is above 50 percent; restarts from ambient when the bridge restarts |

![Torque curves](diagrams/torque_curves.svg)

Holding full collective for more than about 2 seconds over-torques and breaks the engine in this aircraft (the dial swings to about 115 percent). The lever mapping depends on the controller; only two steady readings back the 0.72 slope.

### CowaSim Jet Ranger

The CowaSim Jet Ranger (aircraft title `206B3 ...`, for example `206B3 C-GTOK`) reports the standard SimVars and most dials match them. Gas producer, engine oil, fuel pressure and rotor and engine speed need no change. Three dials do not read the standard values, and it has no transmission oil values:

- **Torque:** the dial is a smoothed copy of `ENG TORQUE PERCENT:1`, about 0.42 x the SimVar (dial about 38 at the steady idle value of 83, about 120 at a peak of about 260) with a smoothing time constant of about 2.5 seconds. Error is about 18 points on a signal swinging between 0 and 120.
- **Turbine out temperature:** the dial follows `ENG EXHAUST GAS TEMPERATURE:1`, not ITT (which reads over 2,000): EGT minus 200 C, within about 30 C over nine readings.
- **Electrical load:** the LOAD dial sat at about 50 percent while the total load was 69.5 A, so 139 A is 100 percent (one reading).
- **Transmission oil:** pressure is 48 psi x the rotor speed fraction (the dial read about 45 to 50 psi); temperature is 0.92 x the engine oil temperature (one reading, which did not follow a slow warm-up).

## Warning lamps

Five lamps are driven; the other ten are aircraft specific (DodoSim supplies them through an FSUIPC offset, which MSFS aircraft do not have) and stay off. Add rules to `Payloads.LampState()` once an aircraft's variables are known.

| Lamp | Rule |
|---|---|
| `EOUT` engine out | Turbine speed below 60 percent |
| `RLOW` rotor low | Rotor at or below 95 percent |
| `LOWF` low fuel | 17 US gal or less |
| `TOPW` transmission oil pressure | Below 40 psi, only when a pressure is available (the FlyInside and CowaSim estimates, or an aircraft whose SimVar reads above 0) |
| `GENF` generator fail | Engine speed below 10 percent |

## Known gaps

- The FlyInside and CowaSim estimates come from a few readings and have not been checked at other power settings.
- Some MSFS Jet Ranger add-ons do not report every SimVar (transmission pressure and temperature read 0 on two of them; one reports a fixed rotor speed). A missing avionics bus voltage (exactly 0) falls back to the main bus for the Radio power rule.
- Pitch sign and the 24 V thresholds are carried over from the FSUIPC bridge and are not verified for every MSFS aircraft.
- There is no hysteresis on the 20 V and 24 V thresholds.
- The FlyInside electrical load dial stays at about 50 percent whatever the load.
- The FlyInside transmission pressure estimate is 35 psi at normal rotor speed, which is below the 40 psi lamp threshold, so the `TOPW` lamp stays on with that aircraft.
