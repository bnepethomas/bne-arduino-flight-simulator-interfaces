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

- [ ] **Flash the Upper controller** with the new backlight code (D5 backlight, `D,BPWR`, `C,B` brightness and the 30 s no-data timeout). It was not connected, so it has only been compiled. The Upper still has no pause for a missing USB HID board (the Radio has one), so with the HID board unplugged each switch change can stall the matrix scan for up to about 80 ms.
- [ ] **Map the three new ADF switches in FSUIPC for the Dodo.** Joystick 4 buttons 17, 18 and 19 (indexes 112, 113 and 114) go to the Dodo events `ANTI_ICE_SWITCH_SET`, `HYDRAULICS_SWITCH_SET` and `CAUTION_LIGHT_SWITCH_SET`, with 1 on press and 0 on release. Use either this mapping or the bridge's `ANTI_ICE_SET` command for anti-ice, not both. The Dodo may ignore the plain `ANTI_ICE_SET` event.
- [ ] **Bridge support for port 27003.** Neither bridge reads the ADF frequency and radar follow ID the ADF controller sends, and neither sends the landing gear lamp states (the gear position SimVars and offsets are not read).
- [ ] **Reflash any board that was flashed before its 5 V heartbeat reading was added**, so the Health Monitor's voltage table shows a reading for it. Check the table for "no reading".
- [ ] **Restart the bridge you use** after the latest build, so it sends `ANTI_ICE_SET` and the backlight power packets to the ADF and Upper.

## Testing on the bench

- [ ] **ADF panel:** the anti-ice, hyd boost and caution dim switches (debug lines such as `ANTI-ICE closed (joystick button index 112)`), the display and lamps blanking and returning with the Radio, and the backlight following `BPWR`, `C,B` and the 30 s no-data timeout.
- [ ] **Radio:** the DME display blanking and returning with the other Radio displays.
- [ ] **Servo:** the fuel pump and fuel filter outputs after the pin swap (fuel pump now A11, aft fuel filter A12).
- [ ] **SwingTester COM ports window:** the list was checked with six boards plugged in on 2026-10-25 (board type, serial numbers, "open elsewhere"); still to try **Restart selected USB device** on a board.

## Documentation and source control

- [x] **Update the documentation** (done 2026-10-25, including the ADF Controller PCB revision 1.5 images and notes) for the changes: the three new ADF switches and `ANTI_ICE_SET`, the DME and ADF blanking, the ADF and Upper backlight and timeout rules, the bridges' new ADF and Upper packets, the SwingTester COM ports window, and the Servo fuel pin swap. Then regenerate the documents.
- [x] **Commit and push** (done 2026-10-25).
