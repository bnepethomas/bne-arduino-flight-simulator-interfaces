# DCSBiosBridge - Program Summary

Windows Forms bridge (.NET 10) that feeds the Jet Ranger panel boards from **DCS World through DCS-BIOS**. It is the DCS counterpart of
`FSUIPCWinformsAutoCS` and `MSFSWinformsAutoCS`: it listens to the DCS-BIOS export stream, reads the values an aircraft **profile** names, and sends them to
the boards in the same `D,TOKEN:value` packets (UDP 13136) the other bridges use, so the board firmware does not change. Run only one bridge at a time.

**Status (31 October 2026): output only.** It builds (Release, 0 warnings) and passes `tests\test_bridge.py` (an end-to-end test with a synthetic DCS-BIOS stream, 14 checks).
The Huey caution lamps have been run against live DCS (confirmed working by the user); the gauge values added afterwards have been tested with the synthetic stream and the real
UH-1H and CommonData module files, but **not yet flown against live DCS**.
DCS-BIOS v0.11.8 was installed on this PC on 31 October 2026 from the official release (`DCS-BIOS_v0.11.8.zip`, GitHub DCS-Skunkworks/dcs-bios) into
`Saved Games\DCS\Scripts`: the `DCS-BIOS` folder and a new `Export.lua` (one line, `dofile(lfs.writedir() .. [[Scripts\DCS-BIOS\BIOS.lua]])`; there was
no `Scripts` folder before). It sends nothing to the simulator (no inputs, no port 27001 radio commands yet).

## How it works

1. **Export stream.** `ExportListener` joins UDP multicast 239.255.50.10 port 5010 on the loopback interface (the DCS-BIOS default, address reuse on so other
   tools can listen too). `ExportParser` is the DCS-BIOS stream state machine (sync `0x55 0x55 0x55 0x55`, then blocks of start address, byte count and data,
   little endian) and keeps the 64 KB memory image in `ExportState`. A block may span datagrams; the test splits every frame in two.
2. **Aircraft.** The name of the aircraft is read from the string at address 0x0000 (24 characters, the DCS-BIOS metadata `_ACFT_NAME`; this address is from
   memory of the DCS-BIOS library and unverified, so the Profile box lets you pick the profile by hand). `profiles\*.json` are matched on their `aircraft` names.
3. **Module definition.** The profile's `jsonFile` (for example `UH-1H.json`) is read from `Saved Games\DCS\Scripts\DCS-BIOS\doc\json\` (or `DCS.openbeta`, or
   `--json-dir`). DCS-BIOS writes these files itself when DCS runs with it installed. `ModuleDoc` finds every control with an `identifier` and integer `outputs`
   (address, mask, shift_by, max_value); a value is `(word at address & mask) >> shift_by`.
4. **Profile values.** Each row maps a generic token to a control: `type` `lamp` (1 when the value is at or above `threshold` of its maximum, optionally
   `invert`), `analog` (the control as a fraction of its maximum, or its raw value with `raw`, times `scale` plus `offset`, `decimals`; or the calibration `points`),
   `digits` (drum digits, most significant first: the radar altimeter) or `coordinate` (degrees, minutes and fraction of a minute plus the N/S/E/W letter). The tokens are the ones the boards already understand: `LED_H_*` (the Huey Warning Panel
   LEDs), `LED_*`, `PITCH`, `BANK`, `IAS`, `VSI`, `AGL`, `FUEL`, `TQ`, `ITT`, `N1`, `ALT`, `RPMR`, `RPME`, `OILT`, `OILP`, `XMSNT`, `XMSNP`, `FUELLOAD`,
   `ELECTRICALLOAD`, `TEMP`, `ZULU`, the servo lamp codes, `LAT`, `LON`, `HDG`, and the power inputs `MAINBUS` and `AVIONICSBUS` (volts). A new aircraft is a
   new profile file, not new code.
5. **Boards.** `Payloads.Build` sorts the tokens by board (`Boards.BoardFor`) and builds one packet each: Radio (.101) `D,NAVCOM1:x,BPWR:x,LED_...`, Servo
   (.102), Stepper (.105), Dual Stepper (.106, with `PWR`) and the GPS interface (127.0.0.1:27002), plus `D,BPWR:x` to the ADF and Upper board (.107) always.
   The old Upper address .103 is not used. Power rules as in the other bridges: no bus voltage mapped means powered while data is flowing; `MAINBUS` at or
   above 20 V switches the backlights and servo lamps on, `AVIONICSBUS` (or `MAINBUS`) at or above 24 V powers the radio, and the Huey LEDs go dark with the radio.
6. **Cadence.** Radio packet on change (at most every 200 ms) or every 5 s; the other boards about every 90 ms; GPS twice a second. When the stream goes quiet for
   3 seconds nothing is sent, so the boards' 30 second no-data watchdogs return the gauges to zero.

## Window and command line

Stream status, aircraft name and profile; the Profile box (Automatic or a profile by hand); **Reload profiles**; **Suggest controls** (lists the module's
controls whose identifier or description match each unmapped token's `hint`, best first, in the log); **Send to the boards**; a table of tokens with their
control, raw value, value sent and board; and a log. Options: `--board-host <ip>` (send every board packet there, for testing), `--json-dir`, `--profile-dir`,
`--module <name>`, `--no-send`.

## The Huey profile

`profiles\UH-1H.json` maps the 20 `LED_H_*` tokens to the UH-1H "Caution Lights Panel" indicator lights `CL_A0` to `CL_J1` (in the order ENG_OIL, ENG_ICE,
ENG_ICE_DETECT, ENG_CHIP, LEFT_FUEL_BOOST, RIGHT_FUEL_BOOST, ENG_FUEL_PUMP, 20_MINUTE_FUEL, FUEL_FILTER, GOV_EMER, AUX_FUEL_LOW, XMSN_OIL_PRESS, XMSN_OIL_HOT,
HYD_PRESS, ENG_INLET_AIR, INST_INVERTER, DC_GENERATOR, EXT_POWER, CHIP_DETECTOR, IFF). The identifiers come from DCS-BIOS's own `UH-1H.lua`, where they use the
DCS cockpit arguments 91 to 110, the same ones the 2017 export script read. The release also ships the generated module files (`doc\json\*.json`, 56 of them), and all
20 controls resolve there to integer outputs (address 0x1414 and 0x1416, masks and shifts as in the table of the file). The aircraft names are `UH-1H` and `Bell47_2`
(from `Module:new("UH-1H", 0x1400, { "UH-1H", "Bell47_2" })`), and the name string `_ACFT_NAME` is at address 0x0000, 24 characters (`MetadataStart.lua`).
No bus voltage is mapped: `DC_VOLTAGE` and `DC_V` are 0 to 1 needle positions with no known voltage scale, so the aircraft counts as powered while data flows (the
backlights then follow the stream, not the bus). The **Suggest controls** button helps with new profiles. Other controls of interest for later: `MASTER_CAUTION_IND`,
and the caution panel's `CLP_RESET_TEST_SW` and `CLP_BRIGHT_DIM_SW` rockers (inputs).

## Gauges

**Every aircraft (`profiles\_common.json`, DCS-BIOS CommonData).** A profile file whose name starts with an underscore is added to every aircraft's own profile (the aircraft's
value for the same token wins), and an aircraft with no profile of its own is read with just these, so **any aircraft** drives the real-unit values: `IAS` (knots, `IAS_US_INT`),
`ALT` (feet above sea level, `ALT_MSL_FT`, no barometric correction), `HDG` (true degrees, `HDG_DEG`) and `LAT` and `LON` (degrees, from `LAT_DEG`, `LAT_SEC` which is really
minutes, `LAT_SEC_FRAC` and the direction letter, south and west negative). These go to the Stepper (`IAS`), the Dual Stepper (`ALT`) and the GPS interface on 127.0.0.1:27002
(`LAT`, `LON`, `ALT`, `IAS`, `HDG`). `CommonData.json` is loaded with the aircraft's file.

**UH-1H (`profiles\UH-1H.json`).** `AGL` (radar altitude, feet) is exact: the radar altimeter's four drum digits `RALT_DIGIT_1` to `4` (thousands first, as DCS-BIOS's own
display string builds it). The other 16 gauge values are the cockpit gauges' **needle positions** (0 to 1 of the dial: all DCS-BIOS exports for them, `defineFloat` gauge positions)
and only have a **first-guess scale**: the dial's whole sweep is mapped onto the panel gauge's own range, so the panel needle follows the same fraction of its dial:

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

They show **"uncalibrated"** in the Status column. To calibrate one: select its row, fly to a reading, press **Calibrate selected...** and type what the cockpit gauge reads
(in the units the panel board uses); the bridge stores the needle position DCS-BIOS reports at that moment as a point of that value. Two or more points replace the guess (straight
lines between the points, held beyond the ends) and the Status shows "calibrated (n points)". The points are kept in `profiles\UH-1H.calibration.json` (token to a list of
[needle position, value]), next to the profile so its comments are not rewritten; **Clear its calibration** removes a token's points. Not mapped: the DC voltage (`DC_VOLTAGE`,
`DC_V`: a 0 to 1 needle with no known volts scale, so `MAINBUS` stays unmapped and the aircraft counts as powered while data flows) and the airspeed, altitude and clock gauges
(the real airspeed and altitude come from CommonData).

## Testing without DCS

`python tests\test_bridge.py` starts the bridge with `--board-host 127.0.0.1`, sends a fake DCS-BIOS stream for an aircraft "TestModule" (three lamps, an
airspeed, a bus voltage, four radar altimeter digits, a calibrated torque needle and an uncalibrated vertical speed needle, plus CommonData's airspeed, altitude,
heading and position; each frame split over two datagrams) and checks the board packets: lamp decoding with mask and shift, the radio power rule, `BPWR` following the bus,
the aircraft's value beating the common one, `AGL` from the drum digits, calibration points against the plain scale, the GPS packet, and a fourth phase with an aircraft that
has no profile (the common values alone still work). All 14 checks pass. It stops only the bridge process it started.

## Not done yet

Calibrating the 16 needle-position gauges against the live cockpit (they run on first-guess scales), a bus voltage for the Huey, a profile for any other aircraft, inputs (joystick buttons already work through the USB HID board; DCS-BIOS commands on UDP 7778 would set exact switch positions), the radio commands on port 27001,
radio frequencies and the autopilot values, and the landing gear lamps. (Documentation: master document and User Guide, done 31 October 2026.)
