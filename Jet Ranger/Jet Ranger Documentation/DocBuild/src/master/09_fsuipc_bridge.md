# FSUIPC bridge (FSUIPCWinformsAutoCS)

`FSUIPCWinformsAutoCS` is the bridge for Prepar3D and FSX. It reads flight data through **FSUIPC** (raw memory offsets), sends each panel board its own packet, and listens on UDP 27001 for radio, battery and alternator commands from the Radio controller, acting on them through the FSUIPC API. It is a Windows Forms application (internally named `FSUIPCTest`); its window lists the values being sent and the 21 annunciator bits.

![FSUIPC bridge data flow](diagrams/fsuipc_bridge.svg)

## Program flow

1. **Constructor:** build the live annunciator list (one row per bit of the 0x2F28 field), connect the UDP clients and start `timerConnection`, which polls once a second for FSUIPC. Start the UDP listener on port 27001 in a background task. (The UDP client `.Connect` calls live in `Form1.Designer.cs`.)
2. **`timerConnection_Tick`:** keep trying `FSUIPCConnection.Open()`. Once it succeeds, stop and start `timerMain`.
3. **`timerMain_Tick`** (about 100 ms, so roughly 10 packets per second per board):
   - `FSUIPCConnection.Process()` refreshes every declared offset and the radio-stack group;
   - format the on-screen readout and update the annunciator list;
   - build and send each board's packet (below);
   - send the radio packet when a frequency or the bus voltage changed (at most every 200 ms) or every 5 seconds regardless.
4. **`UpdateRadios(command)`** handles port 27001 (below).
5. **`chkAvionicsMaster_CheckedChanged`** writes the avionics master offset when the operator toggles the checkbox, and closing the window stops both timers and the FSUIPC connection.

## FSUIPC offsets

| Offset | Meaning |
|---|---|
| `0x02BC` | Airspeed (knots x 128, so finer than 0.1 knot) |
| `0x02C8` | Vertical speed |
| `0x0238`, `0x0239` | Zulu hours and minutes (combined into the HHMM `ZULU`) |
| `0x0E8C` | Outside air temperature, degrees C x 256 (sent as `TEMP`, for example `21C`; a standard FSUIPC offset, not verified against DodoSim) |
| `0x08F0` | Turbine-out percent and temperature |
| `0x08BA`, `0x08B8` | Engine oil pressure and temperature |
| `0x0900`, `0x0904` | Transmission oil pressure and temperature |
| `0x0B74`, `0x0B78` | Fuel percent quantity and fuel capacity |
| `0x08F8` | Fuel pressure |
| `0x08F4` | Torque percent |
| `0x07BC`, `0x07C4`, `0x07C8`, `0x07D0`, `0x07DC`, `0x0800`, `0x0804` | Autopilot master, NAV1 lock (CRS), heading lock, altitude lock, airspeed hold, approach hold and back course hold: the Auto Pilot panel LEDs |
| `0x2EE0` | Flight director active: the FD LED |
| `0x3122` | Radio audio switches (bit 7 COM1 transmit, 6 COM2 transmit, 5 COM receive both, 4 NAV1 sound, 3 NAV2 sound, 2 marker sound, 1 DME sound, 0 ADF1 sound): the Audio panel LEDs |
| `0x0BAC`, `0x0BAE`, `0x0BB0` | Inner, middle and outer marker: the INNER, MIDDLE and OUTER LEDs |
| `0x0898` | Gas producer |
| `0x0908`, `0x0896` | Rotor RPM and engine N2 |
| `0x282C` | Battery load amps |
| `0x3324` | Altitude |
| `0x31E4` | Radio altimeter |
| `0x2F70`, `0x2F78` | Attitude pitch and bank |
| `0x2F28` | 21-bit annunciator (warning lamp) bit field |
| `0x2840`, `0x2850` | Main bus and avionics bus voltage |
| `0x2E80` | Avionics master (read and write) |
| `0x0560`, `0x0568` | Latitude and longitude for the GPS interface (90 degrees = 10001750 x 65536 x 65536; 360 degrees = 65536^4) |
| `0x0570` | Altitude in metres for the GPS interface (64-bit with 32 bits of fraction, sent in feet) |
| `0x0580` | True heading for the GPS interface (360 degrees = 65536^2) |
| `RadioStack` group | COM1 and COM2, NAV1 and NAV2 active and standby, transponder, ADF |

## Packets sent

The Servo, Stepper and Dual Stepper boards each get their **own** packet in real units; the boards convert with their own calibration tables. Whole numbers are rounded.

| Board | Packet contents |
|---|---|
| Servo .102 | `BPWR` (1 while the main bus is 20 V or more), `PITCH` (degrees, one decimal, negated because FSUIPC pitch is nose-down positive), `BANK` (degrees, one decimal), the 15 lamp codes `EOUT`, `TOPW`, `BHOT`, `EC`, `BD`, `GENF`, `TOTW`, `RLOW`, `FFLTR`, `BTMP`, `TC`, `TRC`, `FPMP`, `LOWF`, `SCF` as `:1` or `:0` |
| Stepper .105 | `BPWR`, `IAS` (knots, one decimal), `VSI` (ft/min), `AGL` (feet, power-gated), `FUEL` (US gal), `TQ` (%), `ITT` (degrees C), `N1` (%) |
| Dual Stepper .106 | `PWR` (first: 1, or 0 while the main bus is below 24 V), `BPWR`, `ALT`, `RPMR`, `RPME` (one decimal), `ZULU`, `OILT`, `OILP`, `XMSNT`, `XMSNP`, `FUELLOAD` (psi, from fuel pressure), `ELECTRICALLOAD` (%), `TEMP` |
| Radio .101 | `C1A`, `C1S`, `C2A`, `C2S`, `MAINBUS`, `NAVCOM1` (1 when the avionics bus, offset `0x2850`, is 24 V or more) `BPWR` (backlight power, 1 while the main bus is 20 V or more) and the 19 `LED_xxx` tokens for the Auto Pilot and Audio panel LEDs (all 0 while `NAVCOM1` is 0). The offsets above are standard FSUIPC offsets; the DodoSim aircraft may not drive all of them |

## Aircraft information for the GPS interface

Every 500 ms `SendGpsPacket()` sends `D,LAT:..,LON:..,ALT:..,IAS:..,HDG:..,UTC:yyyyMMddHHmmss` to the Lowrance GPS interface (`127.0.0.1:27002`; change `GpsHost` and `GpsPort` in `Form1.cs` if it runs on another PC). Latitude and longitude are in degrees, altitude in feet, airspeed in knots (the value already read from `0x02BC`) and heading in degrees true. `UTC` is the simulator's UTC date and time from the FSUIPC client library's `FSUIPCConnection.UTCDateTime`; if that read fails it is left out and the GPS interface uses the PC clock. Send errors, for example the GPS interface not running, are ignored. Built clean; **not yet run against Prepar3D**, so the offset conversions are unchecked against the simulator.

## Commands from the Radio controller (UDP 27001)

| Command | Action |
|---|---|
| `COM1_RADIO_SWAP`, `COM2_RADIO_SWAP` | FSUIPC controls `COM_STBY_RADIO_SWAP` and `COM2_RADIO_SWAP` |
| `NAV1_RADIO_SWAP`, `NAV2_RADIO_SWAP` | FSUIPC controls `NAV1_RADIO_SWAP` and `NAV2_RADIO_SWAP` |
| `AVIONICS_MASTER_SET` | Writes the avionics master offset `0x2E80` |
| `MASTER_BATTERY_ON`, `MASTER_BATTERY_OFF` | FSUIPC only has a toggle, so the bridge reads the main bus voltage and toggles only when that would move the switch the requested way |
| `ALTERNATOR_ON`, `ALTERNATOR_OFF` | `TOGGLE_MASTER_ALTERNATOR` unconditionally (no alternator state is read) |
| `COM2_STANDBY:`, `NAV1_STANDBY:`, `NAV2_STANDBY:` plus a frequency | Tunes that radio's standby frequency |
| Auto Pilot, Audio and ADF panel controls: `AP_MASTER`, `AP_HDG_HOLD`, `AP_NAV1_HOLD`, `AP_ALT_HOLD`, `AP_APR_HOLD`, `TOGGLE_FLIGHT_DIRECTOR`, `AP_BC_HOLD`, `AP_PANEL_SPEED_HOLD`, `VOR1_OBI_INC` / `_DEC`, `AP_ALT_VAR_INC` / `_DEC`, `AP_VS_VAR_INC` / `_DEC`, `AP_SPD_VAR_INC` / `_DEC`, `HEADING_BUG_INC` / `_DEC`, `COM1_TRANSMIT_SELECT`, `COM2_TRANSMIT_SELECT`, `COM_RECEIVE_ALL_TOGGLE`, `RADIO_VOR1_IDENT_TOGGLE`, `RADIO_VOR2_IDENT_TOGGLE`, `RADIO_DME1_IDENT_TOGGLE`, `RADIO_ADF_IDENT_TOGGLE`, `MARKER_SOUND_TOGGLE`, `GEAR_UP`, `GEAR_DOWN`, `FLAPS_UP`, `FLAPS_DOWN`, `SPOILERS_ON`, `SPOILERS_OFF`, `PARKING_BRAKES` | The FSUIPC control of the same name, sent with parameter 0 (`TryPanelCommand()`, checked first so none of these can fall through to the COM1 parser). A `:N` suffix repeats it N times (1 to 50) for a knob turned quickly |
| A bare number from 118.00 to 136.975 | COM1 standby: BCD-encoded and sent with `COM_STBY_RADIO_SET`. Any other unrecognised string ends up in this parser too |

## Notes and quirks

- Several unused annunciator bits (`SPARE_*`, `SIMCONNECT_FAIL`, `LOW_INLET_PRESSURE`) all share the short code `SCF`, so the last one in the packet wins for that lamp.
- `TURB_ENG_ITT_1` is assigned twice per tick (percent, then temperature), so the temperature wins.
- Run only one bridge at a time; the MSFS bridge also listens on UDP 27001.
- Built clean and run against Prepar3D v6.1 sending about 10 packets per second per board.
