"""System-level diagrams: overview, network, packets, data pipeline, life cycle, reset, power rules."""
from dgcommon import diagram, Svg, INK, INK2, LINE, text_width, BLUE, GREEN, AMBER, RED


@diagram
def system_overview():
    s = Svg(880, 620)
    s.frame(16, 16, 320, 548, "Windows PC (primary)", "172.16.1.10", theme="blue")
    x, w = 30, 292
    y = 66
    cards = [
        ("Flight simulator", ["MSFS 2020/2024 or Prepar3D / FSX"], "white", 62),
        ("Bridge application", ["MSFSWinformsAutoCS or", "FSUIPCWinformsAutoCS"], "blue", 80),
        ("FSUIPC + DS206 events", ["binds joystick buttons (P3D / FSX)"], "white", 62),
        ("Python UDP reflector", ["board debug text, UDP 27000"], "white", 62),
        ("Health Monitor", ["board heartbeats, UDP 13137"], "white", 62),
        ("Bench tools", ["SwingTester, StepperVSITester,", "radio / upper emulators"], "white", 80),
    ]
    ys = {}
    for t, ln, th, h in cards:
        s.card(x, y, w, h, t, ln, th)
        ys[t] = (y, h)
        y += h + 12
    by, bh = ys["Bridge application"]
    s.arrow([(x + w / 2, ys["Flight simulator"][0] + ys["Flight simulator"][1]), (x + w / 2, by)], None)
    s.rect(432, 66, 20, 440, "#2B2F36", "#2B2F36", rx=6)
    s.text(442, 44, "Ethernet switch", size=14, weight=700, anchor="middle")
    s.text(442, 60, "172.16.1.0/24", size=12.5, fill=INK2, anchor="middle")
    s.arrow([(322, by + 22), (432, by + 22)], "UDP 13136", color=BLUE, label_dy=-8)
    s.arrow([(432, by + 56), (322, by + 56)], "UDP 27001", color=GREEN, label_dy=-8)
    ry = ys["Python UDP reflector"][0]
    s.arrow([(432, ry + 31), (322, ry + 31)], "debug 27000", label_dy=-8)
    hy = ys["Health Monitor"][0]
    s.arrow([(432, hy + 31), (322, hy + 31)], "heartbeat 13137", label_dy=-8)
    s.frame(548, 16, 316, 548, "Panel boards", "Arduino Mega 2560 + W5500", theme="gray")
    boards = [
        ("Radio controller  .101", ["COM/NAV OLEDs, knobs, buttons"], "white"),
        ("Servo controller  .102", ["attitude servos, 15 lamps"], "white"),
        ("Stepper controller  .105", ["IAS, VSI, torque, N1, ITT ..."], "white"),
        ("OLED dual stepper  .106", ["oil, load, RPM, altimeter, OLEDs"], "white"),
        ("ADF + Upper controller  .107", ["ADF knobs, ID keypad, overhead switches"], "white"),
        ("USB HID board  .11", ["UDP 4210  ->  USB joysticks"], "amber"),
    ]
    y = 66
    for t, ln, th in boards:
        s.card(562, y, 288, 56, t, ln, th)
        s.arrow([(452, y + 28), (562, y + 28)], None, color=AMBER if th == "amber" else BLUE, both=True)
        y += 66
    hid_y = y - 66
    s.path(f"M 706 {hid_y + 56} L 706 590 L 176 590 L 176 564", stroke=AMBER, sw=1.8, dash="6 4")
    s.rect(330, 580, 250, 20, "#FFFFFF", "#FFFFFF", rx=3, sw=0)
    s.text(455, 595, "USB: joystick buttons into Windows", size=12.5, fill=AMBER, anchor="middle")
    return s


@diagram
def network_map():
    s = Svg(880, 608)
    s.card(250, 14, 380, 92, "Windows PC  172.16.1.10",
           ["listens: 27001 bridge commands, 27000 debug,", "13137 heartbeats, 27003 ADF events  |  sends:", "13136 to boards, 4210 to the HID board"], "blue", line_size=12.5)
    s.rect(30, 148, 820, 22, "#2B2F36", "#2B2F36", rx=6)
    s.text(46, 164, "Ethernet switch  172.16.1.0/24", size=13.5, weight=700, fill="#FFFFFF")
    s.arrow([(440, 106), (440, 148)], None, color=BLUE, both=True)
    boards = [
        (".101", "Radio", ["in: 13136", "out: 27001", "27000, 13137"], "white"),
        (".102", "Servo", ["in: 13136", "out: 4210", "27000, 13137"], "white"),
        (".105", "Stepper", ["in: 13136", "out: 27000", "13137"], "white"),
        (".106", "Dual stepper", ["in: 13136", "out: 27000", "13137"], "white"),
        (".107", "ADF + Upper", ["in: 13136*", "out: 27003, 4210", "27000, 13137"], "white"),
        (".11", "USB HID", ["in: 4210", "out: 27000", "13137"], "amber"),
    ]
    w, gap = 136, 7.2
    x = 14
    for ip, name, ln, th in boards:
        s.card(x, 214, w, 126, name, [f"172.16.1{ip}"] + ln, th, title_size=14.5, line_size=12.5, center=True)
        s.arrow([(x + w / 2, 170), (x + w / 2, 214)], None, color=BLUE if th == "white" else AMBER, both=True)
        x += w + gap
    s.text(440, 372, "* the ADF + Upper board acts on control packets (C,RESET, C,B, C,DISPLAY ...) and BPWR; the old Upper (.103) is retired", size=12.5, fill=INK2, anchor="middle")
    s.frame(14, 392, 852, 204, "What the ports carry", theme="gray")
    rows = [
        ("13136", "data (D,...) and control (C,...) packets from the PC to a board"),
        ("27001", "radio, battery and alternator commands from the Radio board to the bridge"),
        ("4210", "switch events (index,state) and key events to the USB HID board; also C,RESET:1 for it"),
        ("27002", "aircraft information from a bridge (or the bench tester) to the Lowrance GPS interface"),
        ("27003", "ADF frequency, radar follow ID and power status from the ADF controller to the PC (no bridge reads it yet)"),
        ("27000", "debug text from every board to the UDP reflector"),
        ("13137", "10 second heartbeat text from every board to the Health Monitor"),
    ]
    yy = 440
    for port, desc in rows:
        s.pill(28, yy - 14, port, theme="gray", size=12.5)
        s.text(110, yy + 2, desc, size=13, fill=INK)
        yy += 22
    return s


@diagram
def packet_format():
    s = Svg(880, 410)
    s.text(16, 30, "Data packet  (PC to board, UDP 13136)", size=15, weight=700)
    tiles = [("D", "type"), (",", ""), ("PITCH:-3.3", "gauge, real units"), (",", ""), ("BANK:-12.0", "gauge, real units"), (",", ""), ("EOUT:1", "lamp 1 on / 0 off")]
    x = 16
    for t, note in tiles:
        if t == ",":
            s.text(x + 5, 80, ",", size=24, fill=INK2)
            x += 24
            continue
        w = text_width(t, 20) + 26
        s.rect(x, 52, w, 44, "#F1F1EE", "#C9C9C4", rx=9)
        s.text(x + w / 2, 82, t, size=20, mono=True, anchor="middle")
        if note:
            s.line(x + w / 2, 96, x + w / 2, 118)
            s.text(x + w / 2, 136, note, size=12.5, fill=INK2, anchor="middle")
        x += w + 4
    s.text(16, 190, "Control packet  (UDP 13136, or 4210 for the HID board)", size=15, weight=700)
    x = 16
    for t, note in [("C", "type"), (",", ""), ("RESET:1", "command : value"), (",", ""), ("SWINGTQ:1", "a second command")]:
        if t == ",":
            s.text(x + 5, 238, ",", size=24, fill=INK2)
            x += 24
            continue
        w = text_width(t, 20) + 26
        s.rect(x, 210, w, 44, "#E8F0FB", BLUE, rx=9)
        s.text(x + w / 2, 240, t, size=20, mono=True, anchor="middle")
        s.line(x + w / 2, 254, x + w / 2, 276)
        s.text(x + w / 2, 294, note, size=12.5, fill=INK2, anchor="middle")
        x += w + 4
    s.card(16, 322, 410, 76, "Rules", ["pairs in any order, up to 1000 bytes", "unknown tokens are ignored"], "white", line_size=13)
    s.card(442, 322, 422, 76, "Values", ["real units: degrees, knots, psi, percent, C", "<CODE>RAW sends raw steps / servo position"], "white", line_size=13)
    return s


@diagram
def data_pipeline():
    s = Svg(880, 470)
    cols = [
        (16, "1  Simulator", ["SimVars (MSFS) or", "FSUIPC offsets (P3D / FSX)"], "white"),
        (234, "2  Bridge app", ["converts to real units", "applies aircraft estimates", "builds one packet per board"], "blue"),
        (452, "3  UDP packet", ["D,IAS:93.4,VSI:-250,...", "about 10 per second", "to each board"], "white"),
        (670, "4  Board firmware", ["parses token:value", "calibration table:", "real units to steps"], "gray"),
    ]
    for x, t, ln, th in cols:
        s.card(x, 40, 194, 118, t, ln, th, line_size=12.5)
    for x in (210, 428, 646):
        s.arrow([(x, 99), (x + 24, 99)], None, color=BLUE)
    s.arrow([(767, 158), (767, 196)], None, color=BLUE)
    s.card(670, 196, 194, 74, "Output", ["stepper needle, servo,", "lamp or OLED"], "green", line_size=12.5)
    s.frame(16, 190, 628, 196, "Send rates", theme="gray")
    rows = [
        ("Servo, Stepper, Dual Stepper", "every 100 ms (about 10 per second)"),
        ("Radio", "on a change (at most every 200 ms),"),
        ("", "and at least every 5 seconds"),
        ("No simulator data for 3 s", "the MSFS bridge stops sending"),
        ("No packet for 30 s", "the board returns its gauges to zero"),
    ]
    y = 236
    for a, b in rows:
        if a:
            s.text(30, y, a, size=13, weight=700)
        s.text(300, y, b, size=13, fill=INK)
        y += 28
    s.card(16, 404, 848, 52, "Why real units?", ["each board owns its calibration, so a gauge can be re-measured and fixed without touching the PC software"], "white", line_size=12.5)
    return s


@diagram
def board_lifecycle():
    s = Svg(880, 520)

    def st(x, y, w, h, t, ln, th):
        s.card(x, y, w, h, t, ln, th, line_size=12.5, center=True)
    st(30, 30, 190, 84, "Power-up / reset", ["status LEDs on", "W5500 reset (pin 53)"], "gray")
    st(260, 30, 190, 84, "Ethernet start", ["static IP, UDP sockets", "red LED flashes ~3 s"], "gray")
    st(490, 30, 180, 84, "Start-up swings", ["lamp test, servo sweep,", "stepper homing + swings"], "gray")
    st(710, 30, 150, 84, "Setup complete", ["30 s timer armed", "debug line sent"], "blue")
    s.arrow([(220, 72), (260, 72)], None)
    s.arrow([(450, 72), (490, 72)], None)
    s.arrow([(670, 72), (710, 72)], None)
    st(250, 210, 380, 110, "Running", ["packet in: update targets", "servos / steppers ease to target", "10 s heartbeat to the Health Monitor", "debug text to the reflector"], "green")
    s.arrow([(785, 114), (785, 265), (630, 265)], "first packet", label_dy=-8, label_at=0.35)
    st(560, 410, 300, 90, "No data for 30 s", ["gauges to calibrated zero,", "lamps off, OLEDs blank"], "amber")
    s.arrow([(600, 320), (600, 410)], "30 s silence", label_anchor="end", label_dx=-8, label_dy=0)
    s.arrow([(800, 410), (800, 300), (630, 300)], "data resumes", label_at=0.55, label_dy=-8)
    st(30, 410, 300, 90, "Reboot  (C,RESET:1)", ["debug acknowledgement,", "watchdog or jump to 0, ~2 s"], "red")
    s.arrow([(280, 320), (200, 410)], "C,RESET:1", label_anchor="end", label_dx=-6, label_dy=0)
    s.arrow([(30, 455), (12, 455), (12, 72), (30, 72)], None, color=LINE, dash="5 4")
    s.text(440, 396, "after a reset the board starts again from power-up", size=12.5, fill=INK2, anchor="middle")
    return s


@diagram
def remote_reset():
    s = Svg(880, 420)
    lanes = [("SwingTester", "(or any UDP sender)", 40), ("Board", "", 166), ("Debug stream", "(UDP reflector)", 296)]
    for t, t2, y in lanes:
        s.text(16, y + 24, t, size=13.5, weight=700)
        if t2:
            s.text(16, y + 42, t2, size=12.5, fill=INK2)
    s.card(190, 40, 230, 56, "Reset boards...", ["C,RESET:1 to each ticked board"], "blue", title_size=13.5, line_size=12, center=True)
    s.arrow([(305, 96), (305, 166)], "UDP 13136 (HID: 4210)", label_dx=82, label_dy=0)
    s.card(190, 166, 200, 56, "Acknowledge", ["debug: RESET received"], "gray", title_size=13.5, line_size=12, center=True)
    s.card(420, 166, 200, 56, "Arm watchdog 2 s", ["Radio, ADF/Upper: jump to 0"], "amber", title_size=13.5, line_size=12, center=True)
    s.card(650, 166, 210, 56, "Reboot", ["setup() runs again"], "red", title_size=13.5, line_size=12, center=True)
    s.arrow([(390, 194), (420, 194)], None)
    s.arrow([(620, 194), (650, 194)], None)
    s.card(190, 296, 200, 50, "RESET received", ["rebooting in 2 s"], "white", title_size=13, line_size=12, center=True)
    s.card(420, 296, 200, 50, "Ethernet Started", ["about 4 to 7 s later"], "white", title_size=13, line_size=12, center=True)
    s.card(650, 296, 210, 50, "Setup Complete", ["Servo ~13 s, Dual ~65 s"], "green", title_size=13, line_size=12, center=True)
    s.arrow([(290, 222), (290, 296)], None, dash="4 3")
    s.arrow([(520, 222), (520, 296)], None, dash="4 3")
    s.arrow([(755, 222), (755, 296)], None, dash="4 3")
    s.card(16, 360, 848, 50, "Notes", ["The HID board drops its USB joysticks while it restarts. A board that loops after a reset needs a power cycle."], "white", line_size=12.5)
    return s


@diagram
def power_rules():
    s = Svg(880, 490)
    x0, x1, y = 40, 840, 150

    def vx(v):
        return x0 + (x1 - x0) * v / 30.0
    s.text(20, 34, "Bus voltage decides what is powered", size=16, weight=700)
    s.text(20, 56, "The bridge reads the voltages; the boards only act on the packets they receive.", size=12.5, fill=INK2)
    s.line(x0, y, x1, y, color="#555555", sw=2)
    for v in range(0, 31, 5):
        s.line(vx(v), y - 6, vx(v), y + 6, color="#555555", sw=1.5)
        s.text(vx(v), y + 24, f"{v} V", size=12.5, fill=INK2, anchor="middle")
    s.rect(vx(0), y - 32, vx(20) - vx(0), 26, "#FDECEA", RED, rx=3)
    s.rect(vx(20), y - 32, vx(24) - vx(20), 26, "#FFF3DC", AMBER, rx=3)
    s.rect(vx(24), y - 32, vx(30) - vx(24), 26, "#E7F4EA", GREEN, rx=3)
    s.text(vx(10), y - 14, "everything off", size=12.5, anchor="middle")
    s.text(vx(22), y - 14, "gauges", size=12, anchor="middle")
    s.text(vx(27), y - 14, "gauges + displays", size=12.5, anchor="middle")
    rows = [
        ("Main bus 20 V or more", "Servo lamps, radar altitude (AGL) and the backlights (BPWR:1) are on, otherwise off", "amber"),
        ("Main bus 24 V or more", "Dual Stepper packet carries PWR:1: Baro, Altimeter, Clock and Temperature OLEDs on", "green"),
        ("Avionics bus 24 V or more", "Radio packet carries NAVCOM1:1: COM and NAV displays on", "green"),
        ("Avionics bus reads exactly 0", "the main bus is used instead (FlyInside B206 and CowaSim report 0)", "gray"),
    ]
    yy = 210
    for a, b, th in rows:
        s.card(20, yy, 262, 50, a, [], th, title_size=13.5)
        s.text(296, yy + 30, b, size=13, fill=INK)
        yy += 62
    s.text(20, 478, "There is no hysteresis: a bus hovering at a threshold can make the displays flicker.", size=12.5, fill=INK2)
    return s
