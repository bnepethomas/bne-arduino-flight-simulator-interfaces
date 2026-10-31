# DCS bridge (DCSBiosBridge)

`DCSBiosBridge` is the bridge for **DCS World**. It reads the **DCS-BIOS** export stream, converts the values an aircraft **profile** names into the panel's real units, and sends them to the boards in the same `D,TOKEN:value` packets (UDP 13136) as the other bridges, so the board firmware does not change. It is a Windows Forms program (`net10.0-windows`) and is **output only**: it sends nothing to the simulator (the panel's switches already reach DCS as joystick buttons through the USB HID board). The first aircraft is the UH-1H Huey, but a profile is a data file, so another aircraft needs no new code, and an aircraft with no profile of its own still drives the real-unit gauges from DCS-BIOS's common data.

Run only one bridge at a time. The bridge was run against live DCS for the Huey caution lamps (confirmed working); the gauge values added afterwards are tested with a synthetic stream and the real UH-1H and CommonData module files but have **not been flown against live DCS**.

## DCS-BIOS

DCS-BIOS v0.11.8 was installed on this PC on 31 October 2026 from the official release (`DCS-BIOS_v0.11.8.zip`, GitHub DCS-Skunkworks/dcs-bios) into `Saved Games\DCS\Scripts`: the `DCS-BIOS` folder and a new `Export.lua` containing one line, `dofile(lfs.writedir() .. [[Scripts\DCS-BIOS\BIOS.lua]])`. The release also ships the generated module files (`DCS-BIOS\doc\json\*.json`, 56 of them, including `CommonData.json`).

## How it works

1. **Export stream.** `ExportListener` joins UDP multicast 239.255.50.10 port 5010 on the loopback interface (the DCS-BIOS default, with address reuse on so other tools can listen too). `ExportParser` is the DCS-BIOS stream state machine (sync `0x55 0x55 0x55 0x55`, then blocks of start address, byte count and data, little endian) and keeps the 64 KB memory image in `ExportState`. A block may span datagrams.
2. **Aircraft.** The name of the aircraft is the 24 character string at address 0x0000 (`_ACFT_NAME`). `profiles\*.json` are matched on their `aircraft` names; the Profile box can also pick one by hand.
3. **Module definition.** The profile's `jsonFile` (for example `UH-1H.json`) is read from `Saved Games\DCS\Scripts\DCS-BIOS\doc\json\` (or `DCS.openbeta`, or `--json-dir`). `ModuleDoc` finds every control with an `identifier` and integer `outputs` (address, mask, shift_by, max_value); a value is `(word at address & mask) >> shift_by`.
4. **Profile values.** Each row maps a token to a control. `type` is `lamp` (1 when the value is at or above `threshold` of its maximum, optionally `invert`), `analog` (the control as a fraction of its maximum, or its raw value with `raw`, times `scale` plus `offset`, with `decimals`, or the calibration `points`), `digits` (drum digits, most significant first: the radar altimeter) or `coordinate` (degrees, minutes and fraction of a minute plus the N/S/E/W letter). A profile file whose name starts with an underscore (`_common.json`) is added to every aircraft's profile; the aircraft's own value for a token wins.
5. **Boards.** `Payloads.Build` sorts the tokens by board and builds one packet each: Radio (.101) `D,NAVCOM1:x,BPWR:x,LED_...`, Servo (.102), Stepper (.105), Dual Stepper (.106, with `PWR`), the GPS interface (127.0.0.1:27002) and `D,BPWR:x` to the ADF and Upper board (.107) always. The retired Upper address .103 is not used.
6. **Power rules** as in the other bridges: no bus voltage mapped (`MAINBUS`) means powered while data flows; `MAINBUS` at or above 20 V switches the backlights and the Servo lamps on, `AVIONICSBUS` (or `MAINBUS`) at or above 24 V powers the radio, and the Huey LEDs go dark with the radio.
7. **Cadence.** Radio packet on change (at most every 200 ms) or every 5 s; the other boards about every 90 ms; GPS twice a second. When the stream goes quiet for 3 seconds nothing is sent, so the boards' 30 second no-data watchdogs return the gauges to zero.

## Window and command line

![DCSBiosBridge with a synthetic UH-1H stream: three caution lights on, the common values, the mission clock with its time zone box and the Huey LED rows (scroll for the gauges)](screenshots/dcs_bridge.png)

The window shows the stream status, the aircraft name and the profile in use; the **Profile** box (Automatic or one chosen by hand); **Reload profiles**; **Suggest controls** (lists the module's controls whose identifier or description match each unmapped token's `hint`, best first, in the log); **Calibrate selected...** and **Clear its calibration** (below); **Send to the boards**; the **Mission time zone** box (above); a table of tokens with their control, raw value, value sent, board and status; and a log. Options: `--board-host <ip>` (send every board packet there, for testing), `--json-dir`, `--profile-dir`, `--module <name>`, `--utc-offset <hours>`, `--no-send` and `--logfile`.

## The Huey profile

`profiles\UH-1H.json` maps the 20 `LED_H_*` tokens to the UH-1H *Caution Lights Panel* indicator lights `CL_A0` to `CL_J1` (DCS cockpit arguments 91 to 110), in the order ENG_OIL, ENG_ICE, ENG_ICE_DETECT, ENG_CHIP, LEFT_FUEL_BOOST, RIGHT_FUEL_BOOST, ENG_FUEL_PUMP, 20_MINUTE_FUEL, FUEL_FILTER, GOV_EMER, AUX_FUEL_LOW, XMSN_OIL_PRESS, XMSN_OIL_HOT, HYD_PRESS, ENG_INLET_AIR, INST_INVERTER, DC_GENERATOR, EXT_POWER, CHIP_DETECTOR, IFF. The aircraft names are `UH-1H` and `Bell47_2`. The LEDs light only while the Radio is powered (`NAVCOM1:1`). No bus voltage is mapped: `DC_VOLTAGE` and `DC_V` are 0 to 1 needle positions with no known voltage scale, so the aircraft counts as powered while data flows.

## Gauges

**Every aircraft (`profiles\_common.json`, DCS-BIOS CommonData).** `IAS` (knots), `ALT` (feet above sea level, no barometric correction), `HDG` (true degrees) and `LAT` and `LON` (degrees, built from the latitude and longitude pieces; DCS-BIOS names the minutes `LAT_SEC`, south and west are negative). They go to the Stepper (`IAS`), the Dual Stepper (`ALT`) and the GPS interface (`LAT`, `LON`, `ALT`, `IAS`, `HDG` and `UTC`).

**GPS date and time.** The Lowrance GPS follows the simulator's clock: the bridge sends `UTC:yyyyMMddHHmmss.fff` in the 500 ms GPS packet. DCS-BIOS gives the mission start time (`TIME_START_HIGH` and `LOW`, seconds) and the model time (`TIME_MODEL_HIGH` and `LOW`, hundredths of a second since the start); their sum is the mission clock, which is the **theatre's local time**, so it follows the time of day set in the mission and time acceleration. DCS-BIOS has no time zone and no date, so the window has a **Mission time zone (hours from UTC)** box (also `--utc-offset <hours>`) that is taken off the clock, and the PC's UTC date is used. Set it to 4 for the Caucasus and the Persian Gulf, 3 for Syria, -8 for Nevada. The box starts at 0 and is not remembered between runs. That the start time is seconds since midnight of the mission's local day has not been checked against live DCS.

**UH-1H.** `AGL` (radar altitude, feet) is exact: it is read from the radar altimeter's four drum digits `RALT_DIGIT_1` to `4`. The other 16 gauge values are the cockpit needles' **positions** (0 to 1 of the dial, all DCS-BIOS exports for them) and run on a **first-guess scale**: the dial's whole sweep is mapped onto the panel gauge's own range.

| Token | DCS-BIOS control | Provisional mapping of the dial |
|---|---|---|
| `VSI` | `VVI_P` | -1 to +1 = -1750 to +1750 ft/min |
| `TQ` | `TORQUE_PRESS` | 0 to 1 = 0 to 120 percent |
| `N1` | `GAS_RPM` | 0 to 1 = 0 to 103 percent |
| `ITT` | `EXHAUST_TEMP` | 0 to 1 = 100 to 900 C |
| `FUEL` | `FUEL_QTY` | 0 to 1 = 0 to 75 gallons |
| `OILP`, `OILT` | `ENG_OIL_PRESS`, `ENG_OIL_TEMP` | 0 to 1 = 0 to 150 psi, 0 to 150 C |
| `XMSNP`, `XMSNT` | `TRANS_OIL_PRESS`, `TRANS_OIL_TEMP` | 0 to 1 = 0 to 150 psi, 0 to 150 C |
| `RPMR`, `RPME` | `ROTOR_RPM`, `ENG_RPM` | 0 to 1 = 0 to 110 percent |
| `ELECTRICALLOAD` | `MAIN_GEN_LOAD` | 0 to 1 = 0 to 100 percent |
| `FUELLOAD` | `FUEL_PRESS` | 0 to 1 = 0 to 30 psi |
| `PITCH`, `BANK` | `PADI_PITCH`, `PADI_ROLL` | -1 to +1 = -90 to +90 and -180 to +180 degrees (scale and sign unchecked) |

They show **uncalibrated** in the Status column. To calibrate one, select its row, fly to a reading, press **Calibrate selected...** and type what the cockpit gauge reads (in the units the panel board uses); the bridge stores the needle position DCS-BIOS reports at that moment as a point of that value. Two or more points replace the guess (straight lines between the points, held beyond the ends) and the Status shows *calibrated (n points)*. The points are kept in `profiles\UH-1H.calibration.json` next to the profile; **Clear its calibration** removes a token's points. The airspeed, altitude and clock gauges of the Huey are not mapped (the real airspeed and altitude come from CommonData).

## Testing without DCS

`python tests\test_bridge.py` starts the bridge with `--board-host 127.0.0.1`, sends a fake DCS-BIOS stream for an aircraft called TestModule (each frame split over two datagrams) and checks the board packets: lamp decoding with mask and shift, the radio power rule, `BPWR` following the bus, the aircraft's value beating the common one, `AGL` from the drum digits, calibration points against the plain scale, the GPS packet, and an aircraft with no profile. All 15 checks pass (including the GPS packet's `UTC:` for a mission clock of 11:30:00 at +4 hours). It stops only the bridge process it started.

## Known gaps

- The 16 needle-position gauges are not calibrated against the live cockpit.
- No bus voltage for the Huey, and no profile for any other aircraft yet.
- No inputs: DCS-BIOS commands (UDP 7778) would set exact switch positions, and the radio commands on port 27001 and the autopilot values are not handled; port 27003 (ADF frequency, radar follow ID) is not read.
- The landing gear lamps are not sent.
