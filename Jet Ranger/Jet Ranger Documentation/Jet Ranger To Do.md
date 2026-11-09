# Jet Ranger To Do

Open jobs for the cockpit. Tick an item (`[x]`) when it is done and note the date.

## Hardware

- [ ] **Install a cable for the upper brightness pot from the upper panel to the Upper ADF controller,** following the same cable path as the upper switches cable. The pot input on the Upper ADF Controller PCB is A0 (5 V, ground and wiper); its code is switched off at the moment (`USE_BRIGHTNESS_POT` 0 in the ADF sketch). Once the cable is in, switch it on so the pot sets the ADF backlight and is sent as `C,B` to the Radio, Servo, Stepper and Dual Stepper boards, and check it against the start-up hold's levels (100 percent, then 70 percent).
- [ ] **Install the ATX PSU PCB.**

## Longer term

- [ ] **Redo the Auto Pilot panel as a SAS panel,** including the power LED and the red/green state.

## Firmware and software

- [ ] **Remove the retired Upper address 172.16.1.103** from the FSUIPC and MSFS bridges (`upperClient` in both; the DCS and X-Plane bridges never use it), the BenchTester (the Upper IP box, brightness, backlight power, keep-alive and *Reset boards...* lists) and the Health Monitor's Upper light, and add the ADF and Upper board (.107) to the BenchTester's *Reset boards...* list.
- [ ] **Huey LED states in the FSUIPC and MSFS bridges.** The DCS bridge now sends all 20 `LED_H_xxx` tokens from the UH-1H caution panel (tested live, 2026-10-31) and the X-Plane bridge sends ten approximations from generic annunciators. Still to decide for the Prepar3D / FSX and MSFS bridges: which simulator state drives each lamp, and make them send it.
- [ ] **The ADF sketch has no pause for a missing USB HID board** (the Radio has one), so with the HID board unplugged each switch or key change can stall the scan for up to about 80 ms.
- [ ] **Map the three new ADF switches in FSUIPC for the Dodo.** Joystick 4 buttons 17, 18 and 19 (indexes 112, 113 and 114) go to the Dodo events `ANTI_ICE_SWITCH_SET`, `HYDRAULICS_SWITCH_SET` and `CAUTION_LIGHT_SWITCH_SET`, with 1 on press and 0 on release. Use either this mapping or the bridge's `ANTI_ICE_SET` command for anti-ice, not both. The Dodo may ignore the plain `ANTI_ICE_SET` event.
- [ ] **Simulator-dependent keystrokes from the non-ADF (Upper) keypad.** The Upper numeric keypad (J32 on the Upper ADF Controller PCB) should send different keystrokes depending on the simulator being flown, mainly for radio interactions: **DCS F1 to F10**, **MSFS 1 to 10** (keys 0 to 9 of the keypad). **`*` should toggle the ATC windows** and **`#` should pause** the simulator. Today the keypad types plain keys through the USB HID board (`Upper key pressed: <key>`), which does not know the simulator. To decide: where the mapping lives (the bridge for the running simulator sending the keystrokes on the PC, or telling the HID board which table to use), the key for each of the 12 keys per simulator (and for Prepar3D / FSX and X-Plane), and the exact keys that toggle ATC and pause in each.
- [ ] **Bridge support for port 27003.** None of the four bridges (FSUIPC, MSFS, DCS, X-Plane) reads the ADF frequency and radar follow ID the ADF controller sends, and none sends the landing gear lamp states (the gear position SimVars, offsets, DCS-BIOS controls and datarefs are not read). The panel emulators show the lamps if a bridge sends `LDGN`, `LDGL` and `LDGR`.
- [ ] **DCS bridge (`C# Code/DCSBiosBridge`, DCS-BIOS, output only):** the bridge has run against live DCS (confirmed 2026-11-07); gauge values added 2026-10-31 (airspeed, altitude, heading and position for every aircraft from CommonData; radar altitude and 16 needle-position gauges for the UH-1H). To do: fly the Huey and **calibrate the 16 needle-position gauges** (they run on first-guess scales and show "uncalibrated": select a row, press Calibrate selected..., type what the cockpit gauge reads, repeat at two or more readings); check the pitch and bank sign and scale; add a bus voltage if a volts scale is found; then inputs (DCS-BIOS commands on UDP 7778), the radio commands on port 27001, and profiles for other aircraft. Documentation chapters: done 2026-10-31 (master document and User Guide).
- [ ] **Start-up script** (`Jet Ranger/Start Jet Ranger.bat` / `.ps1`, added 2026-10-31): tested with dry runs and with the real Health Monitor, GPS interface and bridges, but not with the simulators. To do: copy `Start Jet Ranger.sample.json` to `Start Jet Ranger.json` and set the `Reflector` command and the simulator programs (MSFS, P3D, FSX, DCS, X-Plane) if you want `-StartSimulator`; run it once per simulator. This PC blocks `.ps1` files, so use the `.bat`.
- [ ] **GPS time from DCS and X-Plane:** both bridges now send `UTC:` to the Lowrance GPS (added 2026-10-31). To check live: the DCS mission clock is assumed to be seconds since midnight of the mission's local day (set the bridge's Mission time zone box for the theatre); X-Plane uses its zulu clock with the PC's year. Not yet tried against the real AirMap.
- [ ] **X-Plane bridge (`C# Code/XPlaneBridge`, UDP datarefs, no plugin, output only):** built 2026-10-31 and tested against a fake X-Plane (generic profile plus Robinson R22). To do: add profiles for the aircraft you fly, then inputs/port 27001/27003 as for the other bridges. Documentation chapters: done 2026-10-31 (master document and User Guide).

## Testing on the bench

## Documentation and source control

- [ ] **Re-grab all the screenshots** (added 2026-11-08): they are out of date. They are in `DocBuild/screenshots` (the bridges, the emulators, the Health Monitor, the BenchTester and the other program windows; `capture_all.ps1` takes them), and the User Guide and master document use them. Re-take them from the current builds, then rebuild both documents.
