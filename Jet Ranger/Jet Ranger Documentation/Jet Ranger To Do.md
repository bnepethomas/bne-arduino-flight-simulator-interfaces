# Jet Ranger To Do

Open jobs for the cockpit. Tick an item (`[x]`) when it is done and note the date.

## Hardware

- [ ] **Laser-cut a spacer plate for the Auto Pilot panel so the backlighting shines through for HDG, CRS, ALT, VS and IAS.**
  - The plate sits between the Jet Ranger Auto Pilot Panel PCB and the faceplate, with an opening at each of the five knob positions so the backlight reaches the HDG, CRS, ALT, VS and IAS labels (the five rotary encoders on the panel).
  - Where to take the positions from: the KiCad board layout `Jet Ranger/PCBs/Jet Ranger Auto Pilot Panel/Jet Ranger Auto Pilot Panel.kicad_pcb` (encoders `SW1` CRS, `SW2` ALT, `SW3` VS, `SW4` IAS and `SW5` HDG, and the backlight connector `J2`), or the 3D model `Jet Ranger Auto Pilot Panel.step` in the same folder.
  - To decide before cutting: the plate material and thickness, the opening sizes and shapes, whether the same plate is wanted for the Audio panel and the ADF panel, and the mounting holes (take them from the PCB).

- [ ] **Paint the gauge needles black.**

- [ ] **Mask the unwanted backlight.** Block the light that leaks where it should not shine (around the panels and gauges), so only the lettering and markings are backlit.

## Firmware and software

- [x] **Merge the Upper controller sketch into the ADF controller sketch** (done 2026-10-31; flashed to COM26). The combined Upper ADF Controller PCB is now the reference PCB for the ADF controller and the old Upper board (172.16.1.103) is retired. Also fixed that day: the 12 V monitor is on A8 (it read 5.33 V from A15 with 12.3 V on the board) and the ADF keypad lines arrive in reverse order (rows A7 to A4, columns A3 to A1; every key now reports correctly).
- [x] **Flash the Radio controller (COM20) with the Huey build** (done 2026-10-31): Huey Warning Panel switches on A8 to A10 and the 20 LEDs on MAX7219 digits 4 to 7; 40,552 bytes flash and 5,949 bytes RAM (72 percent). The panel is hand wired for now. The Auto Pilot and Audio LEDs will be dimmer than before (scan limit 7 instead of 3).
- [ ] **Remove the retired Upper address 172.16.1.103** from the bridges (`upperClient` in both), the SwingTester (the Upper IP box, brightness, backlight power, keep-alive and *Reset boards...* lists) and the Health Monitor's Upper light, and add the ADF and Upper board (.107) to the SwingTester's *Reset boards...* list.
- [ ] **Decide where the Huey LED states come from** (which simulator state drives each of the 20 `LED_H_xxx` tokens) and make the bridges send them. Neither bridge sends them yet.
- [ ] **The ADF sketch has no pause for a missing USB HID board** (the Radio has one), so with the HID board unplugged each switch or key change can stall the scan for up to about 80 ms.
- [ ] **Map the three new ADF switches in FSUIPC for the Dodo.** Joystick 4 buttons 17, 18 and 19 (indexes 112, 113 and 114) go to the Dodo events `ANTI_ICE_SWITCH_SET`, `HYDRAULICS_SWITCH_SET` and `CAUTION_LIGHT_SWITCH_SET`, with 1 on press and 0 on release. Use either this mapping or the bridge's `ANTI_ICE_SET` command for anti-ice, not both. The Dodo may ignore the plain `ANTI_ICE_SET` event.
- [ ] **Bridge support for port 27003.** Neither bridge reads the ADF frequency and radar follow ID the ADF controller sends, and neither sends the landing gear lamp states (the gear position SimVars and offsets are not read).
- [ ] **DCS bridge (`C# Code/DCSBiosBridge`, DCS-BIOS, output only):** the Huey caution lamps work against live DCS; gauge values added 2026-10-31 (airspeed, altitude, heading and position for every aircraft from CommonData; radar altitude and 16 needle-position gauges for the UH-1H) and tested with a synthetic stream. To do: fly the Huey and **calibrate the 16 needle-position gauges** (they run on first-guess scales and show "uncalibrated": select a row, press Calibrate selected..., type what the cockpit gauge reads, repeat at two or more readings); check the pitch and bank sign and scale; add a bus voltage if a volts scale is found; then inputs (DCS-BIOS commands on UDP 7778), the radio commands on port 27001, and profiles for other aircraft. Documentation chapters: done 2026-10-31 (master document and User Guide).
- [ ] **GPS time from DCS and X-Plane:** both bridges now send `UTC:` to the Lowrance GPS (added 2026-10-31). To check live: the DCS mission clock is assumed to be seconds since midnight of the mission's local day (set the bridge's Mission time zone box for the theatre); X-Plane uses its zulu clock with the PC's year. Not yet tried against the real AirMap.
- [ ] **X-Plane bridge (`C# Code/XPlaneBridge`, UDP datarefs, no plugin, output only):** built 2026-10-31 and tested against a fake X-Plane (generic profile plus Robinson R22). To do: run it against a real X-Plane 12 (check the dataref values, units and the rotor speed scales of the R22 and S-76 profiles, and the Huey LED approximations), add profiles for the aircraft you fly, then inputs/port 27001/27003 as for the other bridges. Documentation chapters: done 2026-10-31 (master document and User Guide).
- [ ] **Reflash any board that was flashed before its 5 V heartbeat reading was added**, so the Health Monitor's voltage table shows a reading for it. Check the table for "no reading".
- [ ] **Restart the bridge you use** after the latest build, so it sends `ANTI_ICE_SET` and the backlight power packets to the ADF and Upper board.

## Testing on the bench

- [ ] **ADF panel on the combined board:** all seven switches at their new pins (gear D31, flaps D32, speed brakes D33, brakes D34, hyd boost D35, anti-ice D36, caution dim D37; debug lines such as `ANTI-ICE closed (joystick button index 112)`), the display and lamps blanking and returning with the Radio, and the backlight following `BPWR`, `C,B` and the 30 s no-data timeout. The 12 V reading and the keypad are done (2026-10-31).
- [ ] **Upper panel on the combined board:** every matrix switch (debug lines `Front Input - <index>:<state>`, joystick buttons on Joystick 1 to 3), the Warning Mute and TOT Test connectors (indexes 88 and 89), the Upper keypad (`Upper key pressed: <key>`, keys typed on the HID board), and that the ADF knobs still tune smoothly while the matrix is scanned.
- [ ] **Huey Warning Panel:** the switches (Joystick 4 buttons 20 to 22), the lamp test while TEST is closed, and each LED from the SwingTester, (the Radio was flashed with the Huey build on 2026-10-31).
- [ ] **Radio:** the DME display blanking and returning with the other Radio displays.
- [ ] **Servo:** the fuel pump and fuel filter outputs after the pin swap (fuel pump now A11, aft fuel filter A12).
- [ ] **SwingTester COM ports window:** the list was checked with six boards plugged in on 2026-10-25 (board type, serial numbers, "open elsewhere"); still to try **Restart selected USB device** on a board.

## Documentation and source control

- [x] **Update the documentation** (done 2026-10-25, including the ADF Controller PCB revision 1.5 images and notes) for the changes: the three new ADF switches and `ANTI_ICE_SET`, the DME and ADF blanking, the ADF and Upper backlight and timeout rules, the bridges' new ADF and Upper packets, the SwingTester COM ports window, and the Servo fuel pin swap. Then regenerate the documents.
- [x] **Commit and push** (done 2026-10-25).
- [x] **Update the documentation again** (done 2026-10-31): the ADF and Upper merge (chapters, pin tables, joystick indexes, heartbeat names, COM26, the Upper ADF Controller PCB rev 1.0 images and notes, diagrams), the 12 V and keypad fixes, and the Huey Warning Panel switches, LEDs and SwingTester tests (firmware only: the Huey panel is hand wired, so its PCB is deliberately left out of the documentation). Then regenerate the documents.
- [x] **Update the to do list** (done 2026-10-31).
- [x] **Commit and push** the 2026-10-31 documentation (done 2026-10-31).
- [x] **Add an installation appendix** (done 2026-10-31): top-side pictures of the five controller PCBs with only the external connectors labelled (Appendix F of the master document, chapter 19 of the User Guide), drawn by `DocBuild/export_install_views.py`. The Huey Warning Panel connectors are left out.
- [x] **Commit and push** the installation appendix. (done 2026-10-31)
- [x] **Add the instrument map appendix** (done 2026-10-31): every gauge, lamp and display with the connector that drives it (Appendix G of the master document, chapter 20 of the User Guide), plus the A3 drawing as `Jet Ranger Instrument Map A3` (SVG, PDF, PNG), made by `DocBuild/make_instrument_map.py`.
- [x] **Commit and push** the installation appendix, the DCS bridge, the X-Plane bridge and the instrument map. (done 2026-10-31)
