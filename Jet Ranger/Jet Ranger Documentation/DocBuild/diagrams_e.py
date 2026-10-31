"""GPS interface and backlight diagrams."""
from dgcommon import diagram, Svg, INK, INK2, LINE, BLUE, GREEN, AMBER, RED


@diagram
def gps_flow():
    s = Svg(880, 520)
    s.card(16, 20, 170, 84, "Simulator", ["MSFS 2020 / 2024 or", "Prepar3D / FSX"], "white", center=True)
    s.card(214, 20, 214, 84, "Bridge", ["MSFSWinformsAutoCS or", "FSUIPCWinformsAutoCS"], "blue", center=True)
    s.arrow([(186, 62), (214, 62)], None)
    s.card(214, 150, 214, 96, "Bench tester", ["typed values, 8-way cursor,", "Country / City / Airport,", "Fly, date and time"], "amber", title_size=14, line_size=12.5, center=True)
    s.card(500, 20, 364, 226, "LowranceAirMapGps", [
        "listens on UDP 27002",
        "aircraft data in use; no packet for",
        "30 s: Brisbane Airport (runway start)",
        "time and date: simulator UTC when",
        "the packet has it, else the PC clock",
        "builds NMEA, sends every 500 ms",
    ], "blue", title_size=15, line_size=13)
    s.arrow([(428, 62), (500, 62)], "UDP 27002", color=BLUE, label_dy=-10)
    s.arrow([(428, 198), (500, 198)], "or", color=AMBER, label_dy=-10)
    s.arrow([(682, 246), (682, 288)], None, color=GREEN)
    s.card(500, 288, 364, 84, "Serial port", ["115200 bps, 8 data bits, no parity, 1 stop bit", "GGA, RMC, GLL, VTG, GSA, HDT sentences"], "gray", title_size=14, line_size=12.5)
    s.arrow([(682, 372), (682, 408)], None, color=GREEN)
    s.card(500, 408, 364, 70, "Lowrance AirMap 2000C", ["receives the position as a GPS"], "white", title_size=14, line_size=12.5, center=True)
    s.card(16, 288, 412, 190, "UDP packet (every 500 ms)", [
        "D,LAT:-27.383000,LON:153.106800,",
        "ALT:13,IAS:0.0,HDG:27.0,",
        "UTC:20261018031205.250",
        "",
        "degrees (south and west negative), feet,",
        "knots, degrees true, simulator UTC time",
    ], "white", title_size=14, line_size=12.5, mono_lines=False)
    return s


@diagram
def gps_states():
    s = Svg(880, 360)
    s.card(16, 30, 340, 170, "Live: aircraft data over UDP", [
        "position, altitude, airspeed, heading",
        "from the latest packet",
        "GPS time and date: the simulator's",
        "UTC, ticking between packets",
    ], "green", title_size=15, line_size=13)
    s.card(524, 30, 340, 170, "Default: Brisbane Airport", [
        "-27.3830, 153.1068, 13 ft, speed 0,",
        "heading 027 true (runway 01L threshold)",
        "GPS time and date: the PC clock",
        "values can be changed in settings.json",
    ], "amber", title_size=15, line_size=13)
    s.arrow([(356, 85), (524, 85)], "no packet for 30 s", color=AMBER, label_dy=-10)
    s.arrow([(524, 150), (356, 150)], "packet arrives", color=GREEN, label_dy=16)
    s.card(16, 232, 848, 96, "What is ignored", [
        "Packets that are not aircraft data (for example other D, packets on the network) or are out of range do not reset the 30 s timer.",
        "A packet without UTC: switches the clock back to the PC's; a packet with only some keys keeps the previous values for the rest.",
    ], "gray", title_size=14, line_size=12.5)
    return s


@diagram
def backlight_logic():
    s = Svg(880, 600)
    s.card(16, 20, 290, 112, "BPWR from the bridge", ["1 = main bus 20 V or more", "0 = below 20 V: backlight off", "assumed 1 until the first one arrives"], "white", title_size=14, line_size=12.5)
    s.card(16, 150, 290, 112, "C,B:0-15 brightness", ["remembered; 0 = off, 15 = full", "none received: default PWM 65", "once received: the last level"], "white", title_size=14, line_size=12.5)
    s.card(16, 280, 290, 112, "No-data watchdog", ["30 s with no UDP packet (also after", "power-up with no PC): backlight off", "the next packet turns it back on"], "white", title_size=14, line_size=12.5)
    s.card(350, 120, 220, 170, "applyBacklight()", ["off if BPWR is 0", "off if the watchdog fired", "otherwise the last C,B", "level, or 65 by default"], "blue", title_size=14.5, line_size=12.5, center=True)
    for y in (76, 206, 336):
        s.arrow([(306, y), (328, y), (328, 205), (350, 205)], None, color=LINE)
    s.arrow([(570, 205), (620, 205)], None, color=GREEN)
    s.card(620, 130, 244, 150, "Mega pin D5 (PWM)", ["0 to 255", "-> 330 ohm -> MOSFET gate", "-> panel backlights"], "green", title_size=14, line_size=12.5, center=True)
    s.frame(16, 424, 848, 160, "Example", "Servo, Stepper and Dual Stepper boards", theme="gray")
    rows = [
        "power-up, no PC feed: 65 for 30 s, then off",
        "packets arrive with BPWR:1: back on at 65     |     C,B:15: full",
        "BPWR:0 (main bus down): off     |     C,B:3 while it is off: stays off",
        "BPWR:1 again: on at 3, the last commanded level, not 65",
    ]
    y = 488
    for r in rows:
        s.text(32, y, r, size=13, fill=INK)
        y += 22
    return s


@diagram
def backlight_circuit():
    s = Svg(880, 400)
    s.text(16, 26, "Backlight driver on the controller PCBs (one MOSFET per board)", size=15, weight=700)
    # Mega pin
    s.card(16, 120, 150, 70, "Mega pin D5", ["BACKLIGHT_PWM"], "blue", title_size=13.5, line_size=12, center=True)
    # series resistor
    s.line(166, 155, 250, 155, color=INK, sw=2)
    s.rect(250, 143, 70, 24, "#FFFFFF", INK, rx=2, sw=2)
    s.text(285, 160, "330 ohm", size=12, anchor="middle")
    s.line(320, 155, 420, 155, color=INK, sw=2)
    # gate node + pull down
    s.circle(380, 155, 4, fill=INK, stroke=INK)
    s.line(380, 155, 380, 232, color=INK, sw=2)
    s.rect(368, 232, 24, 56, "#FFFFFF", INK, rx=2, sw=2)
    s.text(402, 264, "10 k", size=12)
    s.line(380, 288, 380, 320, color=INK, sw=2)
    s.line(364, 320, 396, 320, color=INK, sw=2)
    s.line(370, 326, 390, 326, color=INK, sw=2)
    s.line(376, 332, 384, 332, color=INK, sw=2)
    # MOSFET
    s.rect(420, 100, 90, 110, "#E7F0FD", BLUE, rx=6, sw=2)
    s.text(465, 140, "IRF540N", size=14, weight=700, anchor="middle")
    s.text(465, 160, "N-channel", size=12, anchor="middle", fill=INK2)
    s.text(465, 178, "MOSFET", size=12, anchor="middle", fill=INK2)
    s.text(424, 160, "G", size=12, weight=700)
    s.text(496, 118, "D", size=12, weight=700)
    s.text(496, 204, "S", size=12, weight=700)
    # drain
    s.line(465, 100, 465, 60, color=INK, sw=2)
    s.line(465, 60, 600, 60, color=INK, sw=2)
    s.card(600, 28, 264, 64, "BACKLIGHT_-V net", ["panel backlight connectors (J...)"], "green", title_size=13.5, line_size=12, center=True)
    # source
    s.line(465, 210, 465, 260, color=INK, sw=2)
    s.line(449, 260, 481, 260, color=INK, sw=2)
    s.line(455, 266, 475, 266, color=INK, sw=2)
    s.line(461, 272, 469, 272, color=INK, sw=2)
    s.text(490, 270, "GND", size=12, fill=INK2)
    s.card(600, 140, 264, 150, "On these boards", [
        "Servo (J6-J10, J15-J19)",
        "13 Stepper (12 gauge sockets)",
        "Dual Stepper (gauge sockets)",
        "Radio Auto Pilot, Upper ADF controller",
    ], "gray", title_size=13.5, line_size=12, center=False)
    s.text(16, 366, "Revisions 1.3 to 1.5 (18 October 2026) replaced the generic MOSFET symbol, which had source and drain swapped, with the IRF540N.", size=12.5, fill=INK2)
    s.text(16, 386, "A board built from an earlier revision has the transistor pinned wrongly: check it, or fit it the right way round.", size=12.5, fill=INK2)
    return s


@diagram
def oled_connectors():
    s = Svg(880, 330)
    s.text(16, 26, "The two OLED connection types (pin 1 is on the left)", size=15, weight=700)

    def row(y, title, pins, note):
        s.text(16, y, title, size=14, weight=700)
        x = 16
        for i, (label, fill, stroke) in enumerate(pins):
            s.rect(x, y + 12, 150, 64, fill, stroke, rx=8, sw=1.6)
            s.text(x + 75, y + 36, f"pin {i + 1}", size=12.5, fill=INK2, anchor="middle")
            s.text(x + 75, y + 62, label, size=15, weight=700, anchor="middle")
            x += 160
        s.text(16, y + 98, note, size=12.5, fill=INK2)

    row(60, "Traditional OLED connection", [("+5 V", "#FDECEA", "#C62828"), ("GND", "#EAEAEA", "#555555"), ("other pins", "#FFFFFF", "#C9C9C4"), ("not listed here", "#FFFFFF", "#C9C9C4")],
        "Supply is 5 V on pin 1.")
    row(190, "Directly connected SSD1306 OLED (Temperature and Time)", [("GND", "#EAEAEA", "#555555"), ("+3.3 V", "#FFF3DC", "#C77700"), ("SD / SC", "#FFFFFF", "#C9C9C4"), ("SC / SD", "#FFFFFF", "#C9C9C4")],
        "Supply is 3.3 V on pin 2. The SD and SC pin positions have not been validated.")
    return s
