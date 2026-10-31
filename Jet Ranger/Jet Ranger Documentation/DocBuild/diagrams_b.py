"""Per-board diagrams: Radio, Servo, lamps, Stepper, Dual Stepper, ADF and Upper, HID path."""
from dgcommon import diagram, Svg, INK, INK2, LINE, text_width, BLUE, GREEN, AMBER, RED


def mega(s, x, y, w, h, title, sub, theme="blue"):
    s.card(x, y, w, h, title, sub, theme, title_size=15, line_size=12.5, center=True)


@diagram
def radio_blocks():
    s = Svg(880, 600)
    mega(s, 310, 150, 260, 190, "Arduino Mega 2560", ["JET_RANGER_RADIO_CONTROLLER", "172.16.1.101", "status LEDs 14 (green), 15 (red)"])
    s.card(16, 40, 250, 200, "8 rotary encoders", ["COM1 major 31/30, minor 33/32", "COM2 major 34/35, minor 37/36", "NAV1 major 38/39, minor 41/40", "NAV2 major 42/43, minor 45/44", "polled, no interrupts needed"], "white", title_size=14.5, line_size=12.5)
    s.card(16, 270, 250, 130, "4 x 5 button matrix", ["rows 22 - 25, columns 26 - 29, 48", "col 0 = the four swap keys,", "cols 1 - 4 = Auto Pilot and Audio", "panel switches (+ 5 AP knobs)"], "white", title_size=14.5, line_size=12.5)
    s.arrow([(266, 140), (310, 200)], None, color=BLUE)
    s.arrow([(266, 335), (310, 290)], None, color=BLUE)
    s.card(614, 40, 250, 92, "TCA9548A I2C mux", ["address 0x70", "I2C SDA 20 / SCL 21"], "white", title_size=14.5, line_size=12.5)
    s.card(614, 156, 250, 74, "NAV OLED  (mux port 0)", ["NAV1 | NAV2, active / standby"], "gray", title_size=13.5, line_size=12.5)
    s.card(614, 246, 250, 74, "COMM OLED  (mux port 1)", ["COM1 | COM2, active / standby"], "gray", title_size=13.5, line_size=12.5)
    s.arrow([(570, 200), (614, 86)], "I2C", color=BLUE, label_dx=-8, label_dy=-8)
    s.arrow([(739, 132), (739, 156)], None)
    s.arrow([(700, 230), (700, 246)], None)
    s.card(614, 336, 250, 60, "DME OLED  (mux port 3)", ["Audio panel: 10M / 0M at start-up"], "gray", title_size=13.5, line_size=12.5)
    s.arrow([(700, 320), (700, 336)], None)
    s.card(310, 400, 260, 74, "W5500 Ethernet shield", ["SPI 50 - 52, reset pin 53", "UDP to and from the PC"], "white", title_size=14.5, line_size=12.5)
    s.arrow([(440, 340), (440, 400)], None, color=BLUE, both=True)
    s.card(16, 440, 250, 140, "Swap keys (column 0)", ["row 0: NAV1_RADIO_SWAP", "row 1: NAV2_RADIO_SWAP", "row 2: COM1_RADIO_SWAP", "row 3: COM2_RADIO_SWAP", "        + AVIONICS_MASTER_SET"], "white", title_size=14.5, line_size=12)
    s.card(614, 416, 250, 164, "Power and blanking", ["COM and NAV OLEDs go dark when", "the PC sends NAVCOM1:0 (avionics", "bus below 24 V) or after 30 s with", "no data and no knob or key use.", "Any knob or key wakes them."], "amber", title_size=14.5, line_size=12.5)
    s.arrow([(570, 470), (614, 470)], None, dash="4 3")
    return s


@diagram
def servo_blocks():
    s = Svg(880, 640)
    mega(s, 300, 190, 280, 200, "Arduino Mega 2560", ["JET_RANGER_SERVO_CONTROLLER", "172.16.1.102", "status LEDs 14 / 15, W5500 reset 53"])
    s.card(16, 40, 250, 100, "Attitude servos", ["pin 26  pitch   -30..+30 deg", "pin 27  bank    -90..+90 deg", "eased 1 step per 5 ms"], "white", title_size=14.5, line_size=12.5)
    s.arrow([(266, 100), (300, 230)], None, color=BLUE)
    s.card(16, 170, 250, 120, "Servo power (separate)", ["5 - 6 V supply for the servos,", "ground tied to the Mega.", "Never power servos from the", "Mega 5 V pin."], "amber", title_size=14.5, line_size=12.5)
    s.arrow([(266, 230), (300, 250)], None, dash="4 3", color=AMBER)
    s.card(614, 40, 250, 130, "15 warning lamps", ["pins A1 - A15, two ULN2803A", "drivers; all on at power-up", "(lamp test), then off or", "following the packet"], "white", title_size=14.5, line_size=12.5)
    s.arrow([(580, 250), (614, 120)], "lamps", color=BLUE, label_dx=18, label_dy=0)
    s.card(614, 200, 250, 126, "3 front-panel switches", ["D43 TOT CLEAR", "D47 WARNING MUTE", "D48 FUEL VALVE", "to GND, 30 ms debounce"], "white", title_size=14.5, line_size=12)
    s.arrow([(614, 255), (580, 290)], None, color=BLUE)
    s.card(300, 440, 280, 74, "UDP 13136 in", ["D,PITCH:..,BANK:..,<lamp>:1/0", "C,SWINGPITCH / SWINGBANK / RESET"], "gray", title_size=14, line_size=12.5, center=True)
    s.arrow([(440, 440), (440, 390)], None, color=BLUE)
    s.card(614, 380, 250, 134, "UDP 4210 out to the HID board", ["\"<index>,<state>\" on every change:", "WARNING MUTE  ->  index 7", "TOT CLEAR  ->  index 8", "FUEL VALVE  ->  index 9"], "amber", title_size=13.5, line_size=12.5)
    s.arrow([(580, 340), (614, 420)], None, color=AMBER)
    s.card(16, 380, 250, 134, "Safety nets", ["no packet for 30 s: servos to", "calibrated zero, lamps off", "C,RESET:1: reboot via the", "AVR watchdog (2 s)"], "white", title_size=14.5, line_size=12.5)
    s.text(16, 560, "Joystick 1 buttons 8, 9 and 10 in Windows (the firmware counts from 0). FSUIPC needs them bound to DS206 events.", size=12.5, fill=INK2)
    s.text(16, 582, "Calibration: PITCH_DEG_TABLE  -30 deg 179, 0 deg 113, +30 deg 70;  BANK_DEG_TABLE  -90 deg 5, 0 deg 93, +90 deg 179 (servo positions).", size=12.5, fill=INK2)
    return s


@diagram
def lamp_map():
    s = Svg(880, 566)
    s.text(16, 26, "The 15 warning lamps on the Servo board", size=15, weight=700)
    s.text(16, 46, "Green: the bridge drives it from the simulator. Grey: needs aircraft-specific variables, so it stays off.", size=12.5, fill=INK2)
    lamps = [
        ("A1", "ENGINE OUT", "EOUT", "engine speed < 60 %", True),
        ("A2", "ROTOR RPM LOW", "RLOW", "rotor <= 95 %", True),
        ("A3", "XMSN OIL PRESS", "TOPW", "pressure < 40 psi", True),
        ("A4", "XMSN OIL TEMP", "TOTW", "", False),
        ("A5", "BATTERY HOT", "BHOT", "", False),
        ("A6", "BATTERY TEMP", "BTMP", "", False),
        ("A7", "ENGINE CHIP", "EC", "", False),
        ("A8", "TAIL ROTOR CHIP", "TRC", "", False),
        ("A9", "XMSN CHIP", "TC", "", False),
        ("A10", "BAGGAGE DOOR", "BD", "", False),
        ("A11", "AFT FUEL FILTER", "FFLTR", "", False),
        ("A12", "FUEL PUMP", "FPMP", "", False),
        ("A13", "GENERATOR FAIL", "GENF", "engine speed < 10 %", True),
        ("A14", "LOW FUEL", "LOWF", "fuel <= 17 gal", True),
        ("A15", "SC FAIL", "SCF", "", False),
    ]
    cw, ch, gx, gy = 276, 78, 10, 10
    for i, (pin, name, code, rule, on) in enumerate(lamps):
        col, row = i % 3, i // 3
        x = 16 + col * (cw + gx)
        y = 70 + row * (ch + gy)
        theme = "green" if on else "gray"
        s.card(x, y, cw, ch, name, [f"pin {pin}   token {code}", rule if on else "always off"], theme, title_size=13.5, line_size=12.5)
    s.text(16, 534, "All lamps go out when the main bus is below 20 V. Sent as <token>:1 / <token>:0 in every Servo packet.", size=12.5, fill=INK2)
    s.text(16, 554, "The lamp test at power-up lights all 15; a no-data timeout turns them all off.", size=12.5, fill=INK2)
    return s


@diagram
def adf_blocks():
    s = Svg(880, 700)
    mega(s, 300, 170, 280, 190, "Arduino Mega 2560", ["JET_RANGER_ADF_CONTROLLER", "172.16.1.107  (ADF + Upper)", "status LEDs 14 / 15, reset 53"])
    s.card(16, 20, 250, 90, "ADF knobs", ["major D6 / D7: 100 kHz per click", "minor D8 / D9: 1 kHz per click", "range 190 to 1799 kHz"], "white", title_size=14, line_size=12.5)
    s.arrow([(266, 65), (300, 200)], None, color=BLUE)
    s.card(16, 122, 250, 96, "ADF keypad 4 x 3", ["rows A7 - A4, columns A3 - A1", "0 - 9 type the aircraft ID", "# sends it, * deletes a digit"], "white", title_size=14, line_size=12.5)
    s.arrow([(266, 170), (300, 235)], None, color=BLUE)
    s.card(16, 230, 250, 104, "Upper switch matrix 9 x 8", ["rows D22 - D30, columns D38 - D45", "one row per pass of the loop", "index = row x 11 + column (0 - 95)"], "white", title_size=14, line_size=12.5)
    s.arrow([(266, 282), (300, 270)], None, color=BLUE)
    s.card(16, 346, 250, 80, "Upper keypad 4 x 3", ["rows A10, A12, A14, A15", "columns A9, A11, A13"], "white", title_size=14, line_size=12.5)
    s.arrow([(266, 386), (300, 310)], None, color=BLUE)
    s.card(16, 438, 250, 80, "Brightness pot (switched off)", ["wiper to A0 (5 V to ground)", "not read for the moment"], "gray", title_size=14, line_size=12.5)
    s.arrow([(266, 478), (330, 360)], None, color=BLUE)
    s.card(16, 530, 250, 104, "Power monitors", ["12 V: A8, 47k / 10k divider", "5 V: AVcc against the 1.1 V", "band-gap (AREF left at AVcc)"], "white", title_size=14, line_size=12.5)
    s.arrow([(266, 582), (360, 360)], None, color=BLUE)
    s.card(614, 20, 250, 80, "Character OLED", ["I2C 0x3C, SDA 20 / SCL 21", "ADF kHz and aircraft ID"], "gray", title_size=14, line_size=12.5)
    s.arrow([(580, 210), (614, 70)], None, color=LINE)
    s.card(614, 116, 250, 140, "UDP 27003 to the PC", ["ADF_FREQ:<kHz>", "RADAR_FOLLOW_ID:<digits>", "POWER,V12:..,V5:..,STATUS:OK", "(power status every 30 s)"], "amber", title_size=14, line_size=12.5)
    s.arrow([(580, 250), (614, 200)], None, color=AMBER)
    s.card(614, 272, 250, 130, "Switches to the simulator", ["D31-D37 to the bridge (27001):", "GEAR_SET, FLAPS_SET,", "SPOILERS_SET, ANTI_ICE_SET (1/0)", "buttons 104-107, 112-114 to HID"], "green", title_size=14, line_size=12.5)
    s.arrow([(580, 300), (614, 330)], None, color=GREEN)
    s.card(614, 418, 250, 100, "Upper panel to the HID board", ["UDP 4210: \"<index>,<state>\" (0 - 95)", "keypad: \"KEY,<key>,<state>\"", "heartbeats ADF_PANEL + UPPER_INPUT"], "amber", title_size=13.5, line_size=12.5)
    s.arrow([(580, 330), (614, 460)], None, color=AMBER)
    s.card(614, 534, 250, 100, "Own backlight and lamps", ["D5 PWM: BPWR, C,B, 30 s no-data;", "OLED and lamps D2 - D4 blank with", "the Radio displays (C,DISPLAY)"], "white", title_size=14, line_size=12.5)
    s.arrow([(580, 350), (614, 570)], None, color=LINE)
    s.card(300, 420, 280, 100, "UDP 13136 in", ["C,RESET:1  C,MATRIX:1  C,POWER:1", "C,B:0-15  C,DISPLAY:0/1  C,KEEPALIVE:1", "D,LDGN/LDGL/LDGR (lamps), D,BPWR:1/0"], "white", title_size=14, line_size=12.5, center=True)
    s.arrow([(440, 420), (440, 360)], None, color=BLUE)
    s.text(16, 668, "The toggle switches send their position as a value (every change and every 5 s); the brakes push button sends PARKING_BRAKES.", size=12.5, fill=INK2)
    s.text(16, 688, "The Upper panel (switch matrix and keypad) was the separate Upper controller, 172.16.1.103, until it was merged into this sketch.", size=12.5, fill=INK2)
    return s


@diagram
def hid_path():
    s = Svg(880, 600)
    s.card(16, 20, 250, 96, "ADF + Upper controller  .107", ["9 x 8 switch matrix + keypad", "index = row x 11 + column", "heartbeats ADF_PANEL, UPPER_INPUT"], "white", title_size=14, line_size=12.5)
    s.card(16, 140, 250, 96, "Servo controller  .102", ["WARNING MUTE  index 7", "TOT CLEAR     index 8", "FUEL VALVE    index 9"], "white", title_size=14, line_size=12.5)
    s.card(330, 70, 220, 120, "USB HID board  .11", ["Arduino Leonardo", "UDP 4210 in", "heartbeat JOYSTICK"], "amber", title_size=14, line_size=12.5, center=True)
    s.arrow([(266, 68), (330, 110)], "idx,state", color=AMBER, label_dy=-8)
    s.arrow([(266, 188), (330, 150)], "idx,state", color=AMBER, label_dy=14)
    s.card(614, 20, 250, 220, "Windows sees", ["4 joysticks x 32 buttons", "index 0 - 31    Joystick 1", "index 32 - 63   Joystick 2", "index 64 - 95   Joystick 3", "index 96 - 127  Joystick 4", "plus a keyboard (keypad keys)"], "white", title_size=14, line_size=12.5)
    s.arrow([(550, 130), (614, 130)], "USB", color=AMBER, label_dy=-8)
    s.card(614, 270, 250, 90, "FSUIPC or the simulator", ["binds each button to an event", "(DS206 events on P3D / FSX)"], "blue", title_size=14, line_size=12.5)
    s.arrow([(739, 240), (739, 270)], None)
    s.text(16, 300, "Index map for the front-panel switches", size=15, weight=700)
    s.card(16, 316, 400, 112, "Windows button numbers count from 1", ["index 7  ->  Joystick 1, button 8", "index 8  ->  Joystick 1, button 9", "index 9  ->  Joystick 1, button 10", "FSUIPC counts from 0 (button 7, 8, 9)"], "white", title_size=14, line_size=12.5)
    s.card(436, 380, 428, 90, "Boot", ["the Servo board sends each switch's starting", "state once at the end of power-up, so the", "virtual joystick matches the switches"], "white", title_size=14, line_size=12.5)
    s.card(16, 450, 400, 120, "Remote reset", ["C,RESET:1 to UDP 4210 reboots the HID", "board (watchdog, 2 s). Windows briefly", "loses the joysticks and keyboard, then", "detects them again."], "white", title_size=14, line_size=12.5)
    s.card(436, 490, 428, 80, "Local button", ["pin 5 on the HID board itself drives", "Joystick 1 button 0"], "gray", title_size=14, line_size=12.5)
    return s


@diagram
def radio_routing():
    s = Svg(880, 640)
    s.text(16, 26, "Knob turned: radio board to simulator", size=15, weight=700)
    s.card(16, 40, 150, 84, "Encoder", ["COM1 / COM2 /", "NAV1 / NAV2"], "white", center=True)
    s.card(204, 40, 230, 84, "Radio board", ["updates its own display,", "sends a labelled standby", "to UDP 27001"], "gray", center=True)
    s.card(472, 40, 190, 84, "Bridge", ["UpdateRadios()", "range-checks the value"], "blue", center=True)
    s.card(700, 40, 164, 84, "Simulator", ["sets that radio's", "standby frequency"], "white", center=True)
    for a, b in ((166, 204), (434, 472), (662, 700)):
        s.arrow([(a, 82), (b, 82)], None, color=GREEN if a == 434 else LINE)
    s.card(204, 140, 458, 112, "What the radio sends on 27001", [
        "COM1   118.295                (a bare number)",
        "COM2   COM2_STANDBY:118.295",
        "NAV1   NAV1_STANDBY:108.000",
        "NAV2   NAV2_STANDBY:108.000"], "white", title_size=13.5, line_size=12.5, mono_lines=True)
    s.text(16, 286, "Swap key: radio board to simulator", size=15, weight=700)
    s.card(16, 300, 150, 74, "Swap key", ["column 0"], "white", center=True)
    s.card(204, 300, 230, 74, "Radio board", ["sends the swap command", "to UDP 27001"], "gray", center=True)
    s.card(472, 300, 190, 74, "Bridge", ["swap event"], "blue", center=True)
    s.card(700, 300, 164, 74, "Simulator", ["swaps active /", "standby"], "white", center=True)
    for a, b in ((166, 204), (434, 472), (662, 700)):
        s.arrow([(a, 337), (b, 337)], None, color=GREEN if a == 434 else LINE)
    s.text(204, 398, "NAV1 and NAV2 also swap on the NAV display at once, because the PC does not send NAV frequencies back.", size=12.5, fill=INK2)
    s.text(16, 440, "Simulator values: bridge to radio board", size=15, weight=700)
    s.card(16, 454, 190, 84, "Simulator", ["COM 1 and 2 active", "and standby, bus", "voltages"], "white", center=True)
    s.card(244, 454, 190, 84, "Bridge", ["builds the radio packet", "on a change, and at", "least every 5 s"], "blue", center=True)
    s.card(472, 454, 190, 84, "Radio board", ["UDP 13136: COM line", "redrawn; NAV redrawn", "on a power change"], "gray", center=True)
    s.card(700, 454, 164, 84, "Displays", ["padded to", "128.300  127.800"], "white", center=True)
    for a, b in ((206, 244), (434, 472), (662, 700)):
        s.arrow([(a, 496), (b, 496)], None, color=BLUE if a == 434 else LINE)
    s.text(16, 570, "D,C1A:128.300,C1S:127.800,C2A:118.000,C2S:121.500,MAINBUS:28.0,NAVCOM1:1", size=12.5, mono=True, fill=INK)
    s.text(16, 594, "Tuning steps: COM 25 kHz channels 118.000 - 136.975 MHz; NAV 50 kHz steps 108.00 - 117.95 MHz.", size=12.5, fill=INK2)
    s.text(16, 616, "The minor knob wraps within the same MHz; the major knob stops at the ends.", size=12.5, fill=INK2)
    return s


@diagram
def stepper_blocks():
    s = Svg(880, 600)
    s.frame(16, 12, 848, 332, "Gauges in use (7)", "each is a stepper with its own calibration table", theme="gray")
    left = [
        ("Airspeed  IAS", "pins 12, 13, 22, 23", "knots 0 - 140, 10 points"),
        ("Vertical speed  VSI", "pins 7, 8, 9, 11", "fpm -1750 to +1750, 11 pts"),
        ("Radar altitude  AGL", "pins 32 - 35", "feet 0 - 2500, 11 points"),
        ("Fuel quantity  FUEL", "pins 2, 3, 4, 6", "gallons 0 - 75, 2 points"),
    ]
    right = [
        ("Torque  TQ", "pins 36 - 39", "percent 0 - 120, 2 points"),
        ("Gas producer  N1", "pins 40 - 43", "percent 0 - 103, 3 points"),
        ("Turbine out temperature  ITT", "pins A11 - A14", "deg C 100 - 900, 7 points"),
    ]
    y = 62
    for t, a, b in left:
        s.card(30, y, 400, 62, t, [f"{a}   |   {b}"], "white", title_size=14, line_size=12.5)
        y += 68
    y = 62
    for t, a, b in right:
        s.card(450, y, 400, 62, t, [f"{a}   |   {b}"], "white", title_size=14, line_size=12.5)
        y += 68
    s.arrow([(440, 344), (440, 372)], None, color=BLUE)
    mega(s, 160, 372, 560, 84, "Arduino Mega 2560", ["JET_RANGER_STEPPER_CONTROLLER  |  172.16.1.105  |  UDP 13136"])
    s.card(16, 480, 280, 100, "Safety nets", ["no packet for 30 s: gauges to zero", "C,RESET:1: watchdog reboot", "C,SWING<code>:1: one swing"], "white", title_size=14, line_size=12.5)
    s.card(310, 480, 260, 100, "Each stepper", ["FULL4WIRE via AccelStepper:", "homes against its stop,", "then eases to the target"], "white", title_size=14, line_size=12.5)
    s.card(584, 480, 280, 100, "Present but unused here", ["EOT, XOT, XOP, EOP steppers: the", "oil gauges live on the Dual", "Stepper board"], "gray", title_size=14, line_size=12.5)
    return s


@diagram
def dual_stepper_blocks():
    s = Svg(880, 760)
    g = [
        ("Engine oil pressure", "OILP", "pins 44 - 47"),
        ("Engine oil temp", "OILT", "pins 48, A0 - A2"),
        ("Transmission pressure", "XMSNP", "pins A7 - A10"),
        ("Transmission temp", "XMSNT", "pins A3 - A6"),
        ("Electrical load %", "ELECTRICALLOAD", "pins 36 - 39"),
        ("Fuel load psi", "FUELLOAD", "pins 32 - 35"),
        ("Turbine speed %", "RPME", "pins 24 - 27"),
        ("Rotor speed %", "RPMR", "pins 28 - 31"),
        ("Altimeter feet", "ALT", "pins 40 - 43, zero A15"),
    ]
    s.frame(16, 12, 848, 276, "Nine stepper gauges", "all FULL4WIRE, real-unit tables, start-up swing in a fixed order", theme="gray")
    cw, chh = 270, 62
    for i, (t, code, pins) in enumerate(g):
        col, row = i % 3, i // 3
        s.card(30 + col * (cw + 9), 60 + row * (chh + 8), cw, chh, f"{t}", [f"{code}   |   {pins}"], "white", title_size=13.5, line_size=12)
    s.arrow([(440, 288), (440, 316)], None, color=BLUE)
    mega(s, 190, 316, 500, 84, "Arduino Mega 2560", ["JET_RANGER_OLED_DUAL_STEPPER_CONTROLLER", "172.16.1.106  |  UDP 13136"])
    s.arrow([(440, 400), (440, 428)], "I2C", color=BLUE, label_dx=22, label_dy=4)
    s.frame(16, 428, 848, 214, "Four OLEDs on a TCA9548A I2C multiplexer", "address 0x70, reset pin 16, SDA 20 / SCL 21", theme="gray")
    oleds = [("Channel 1  Baro", "29.92 inHg setting"), ("Channel 2  Altimeter", "rolling drum digits (ALT)"), ("Channel 3  Clock", "ZULU time, HHMM"), ("Channel 4  Temperature", "TEMP text, e.g. 21C")]
    for i, (t, d) in enumerate(oleds):
        col, row = i % 2, i // 2
        s.card(30 + col * 424, 480 + row * 74, 410, 64, t, [d], "white", title_size=13.5, line_size=12.5)
    s.card(16, 660, 420, 84, "PWR code", ["PWR:0 (main bus below 24 V) blanks all", "four OLEDs; PWR:1 brings them back"], "amber", title_size=13.5, line_size=12.5)
    s.card(450, 660, 414, 84, "Safety nets", ["no packet for 30 s: gauges to zero, OLEDs blank", "C,RESET:1 reboot; C,OLEDTEST:1/0 lamp test"], "white", title_size=13.5, line_size=12.5)
    return s
