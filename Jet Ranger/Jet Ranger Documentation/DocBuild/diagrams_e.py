"""GPS interface and backlight diagrams."""
from dgcommon import diagram, Svg, INK, INK2, LINE, BLUE, GREEN, AMBER, RED


@diagram
def gps_flow():
    s = Svg(880, 520)
    s.card(16, 20, 170, 84, "Simulator", ["MSFS, Prepar3D / FSX,", "DCS World or X-Plane"], "white", center=True)
    s.card(214, 20, 214, 84, "Bridge", ["FSUIPC, MSFS, DCSBios", "or XPlane bridge"], "blue", center=True)
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


@diagram
def power_supply():
    s = Svg(880, 672)
    C12, C5 = RED, "#7A7A75"          # 12 V red, 5 V gray
    s.text(16, 26, "Power supply", size=17, weight=700)

    # the ATX supply and its PCB
    s.frame(16, 44, 262, 372, "ATX power supply")
    s.card(32, 92, 230, 70, "ATX power supply", ["the supply unit"], "gray", center=True)
    s.arrow([(147, 162), (147, 192)], "rails", color="#2B2F36", sw=1.2, label_dx=22, label_dy=3)
    s.card(32, 192, 230, 200, "ATX power supply PCB", ["12 V and 5 V outputs"], "blue", center=True)
    # the six output connectors on the right edge of the PCB: (y, colour, label)
    outs = [(226, C12, "12 V"), (256, C5, "5 V"), (286, C5, "5 V"), (316, C5, "5 V"), (346, C12, "12 V"), (376, C12, "12 V")]
    for y, col, lab in outs:
        s.rect(256, y - 7, 14, 14, col, "#444444", rx=3, sw=1)
        s.text(246, y + 4.5, lab, size=12, weight=700, anchor="end", fill=INK2)

    # the front assembly: the power distribution PCB, the network switch and the USB hub
    s.frame(520, 44, 344, 350, "Front assembly", "front panel assembly")
    s.card(548, 98, 288, 96, "Power distribution PCB", ["receives 12 V and 5 V from the ATX supply"], "blue", center=True)
    s.card(548, 214, 288, 70, "Network switch", ["sits in the front panel assembly"], "white", center=True)
    s.card(548, 304, 288, 70, "USB hub", ["7 ports, powered directly"], "white", center=True)

    # the GPS and the 12 V lighting module, outside the front assembly
    s.card(548, 424, 288, 80, "GPS", ["Lowrance AirMap 2000C"], "white", center=True)
    s.card(548, 514, 288, 80, "12 V lighting module", [], "white", center=True)

    # cables from the PCB's connectors (the markers on its edge) to what they power
    s.arrow([(270, 226), (330, 226), (330, 128), (548, 128)], "12 V power cable", color=C12, sw=2.6, label_at=0.55, label_dy=-7)
    s.arrow([(270, 256), (400, 256), (400, 168), (548, 168)], "5 V power cable", color=C5, sw=2.6, label_at=0.55, label_dy=-7)
    s.arrow([(270, 286), (460, 286), (460, 249), (548, 249)], None, color=C5, sw=2.6)
    s.text(365, 279, "5 V cable (separate)", size=12, anchor="middle", fill=INK2)
    s.arrow([(270, 316), (500, 316), (500, 339), (548, 339)], None, color=C5, sw=2.6)
    s.text(385, 309, "5 V cable (separate)", size=12, anchor="middle", fill=INK2)
    s.arrow([(270, 346), (300, 346), (300, 464), (548, 464)], None, color=C12, sw=2.6)
    s.text(424, 457, "12 V cable (separate)", size=12, anchor="middle", fill=INK2)
    s.arrow([(270, 376), (286, 376), (286, 554), (548, 554)], None, color=C12, sw=2.6)
    s.text(424, 547, "12 V cable (separate)", size=12, anchor="middle", fill=INK2)

    # key
    s.text(16, 440, "Cables", size=13, weight=700)
    s.line(16, 460, 56, 460, C12, 3)
    s.text(64, 464, "12 V", size=12.5)
    s.line(110, 460, 150, 460, C5, 3)
    s.text(158, 464, "5 V", size=12.5)
    s.text(16, 620, "The ATX supply PCB feeds the power distribution PCB with its own 12 V and 5 V cables.", size=12.5, fill=INK2)
    s.text(16, 638, "The GPS, the 12 V lighting module, the network switch and the USB hub each have a separate cable of their own.", size=12.5, fill=INK2)
    s.text(16, 656, "The USB hub is powered directly, not through the power distribution PCB. Each cable is one line; ground returns are not drawn.", size=12.5, fill=INK2)
    return s


@diagram
def usb_wiring():
    s = Svg(880, 560)
    USB, SERIAL = BLUE, AMBER
    s.text(16, 26, "USB wiring", size=17, weight=700)
    # the key
    s.line(560, 22, 600, 22, USB, 3)
    s.text(608, 26, "USB cable", size=12.5)
    s.line(710, 22, 750, 22, SERIAL, 3)
    s.text(758, 26, "serial cable", size=12.5)

    # the PC, with the sound card above it and the flight controls below it: each has its own USB cable
    s.card(16, 60, 190, 70, "External sound card", ["Sound Blaster"], "white", title_size=14, center=True)
    s.card(16, 210, 230, 90, "PC", ["primary flight simulator PC"], "gray", center=True)
    s.card(16, 350, 190, 60, "Primary flight controls", [], "white", title_size=14, center=True)
    s.card(16, 440, 190, 70, "CKAS 6DOF platform", ["motion platform"], "white", title_size=14, center=True)
    s.arrow([(70, 210), (70, 130)], "USB", color=USB, sw=2.6, label_dx=22, label_dy=3)
    s.arrow([(70, 300), (70, 350)], "USB", color=USB, sw=2.6, label_dx=22, label_dy=3)
    s.arrow([(232, 300), (232, 475), (206, 475)], "USB", color=USB, sw=2.6, label_at=0.5, label_dx=0, label_dy=4)

    # the front assembly: the hub and everything on it
    s.frame(262, 44, 438, 474, "Front assembly")
    s.card(284, 80, 112, 410, "USB hub", ["7 ports"], "blue", center=True)
    s.arrow([(246, 255), (284, 255)], "USB", color=USB, sw=2.6, label_dy=-8)

    rows = [
        ("Radio controller", ".101", "blue"),
        ("Servo controller", ".102", "green"),
        ("Stepper controller", ".105", "amber"),
        ("Dual Stepper controller", ".106", "purple"),
        ("ADF and Upper controller", ".107", "teal"),
        ("USB HID board", ".11", "white"),
        ("USB to serial adapter", "", "gray"),
    ]
    y0, step = 104, 56
    for i, (name, addr, theme) in enumerate(rows):
        cy = y0 + i * step
        s.card(470, cy - 22, 208, 44, name + ("  " + addr if addr else ""), [], theme, title_size=13.5, center=True)
        s.arrow([(396, cy), (470, cy)], None, color=USB, sw=2.4)

    # the serial adapter to the GPS
    cy = y0 + 6 * step
    s.card(730, cy - 36, 134, 72, "GPS", ["Lowrance", "AirMap 2000C"], "white", title_size=14, line_size=12, center=True)
    s.arrow([(678, cy), (730, cy)], None, color=SERIAL, sw=2.4)
    s.text(704, cy - 9, "serial", size=12, anchor="middle", fill=INK2)

    s.text(262, 545, "The sound card, the flight controls and the CKAS platform connect to the PC directly, not through the hub.", size=12.5, fill=INK2)
    return s


@diagram
def ckas_platform():
    s = Svg(880, 436)
    USB, SAFE = BLUE, AMBER
    s.text(16, 26, "CKAS 6DOF platform connections", size=17, weight=700)
    s.line(540, 22, 580, 22, USB, 3)
    s.text(588, 26, "USB cable", size=12.5)
    s.line(690, 22, 730, 22, SAFE, 3)
    s.text(738, 26, "wiring to the platform", size=12.5)

    # the PC and the platform, joined by USB
    s.card(16, 200, 190, 110, "PC", ["primary flight simulator PC"], "gray", center=True)
    s.card(330, 90, 230, 270, "CKAS 6DOF platform", ["six degrees of freedom", "motion platform"], "blue", center=True)
    s.rect(323, 248, 14, 14, USB, "#444444", rx=3, sw=1)
    s.arrow([(206, 255), (323, 255)], "USB", color=USB, sw=2.8, both=True, label_dy=-9)

    # the emergency stop switch and the two door open sensors, wired to the platform
    inputs = [
        (190, "Emergency stop switch", "", "red", "emergency stop"),
        (255, "Door open sensor 1", "", "amber", "door sensor 1"),
        (320, "Door open sensor 2", "", "amber", "door sensor 2"),
    ]
    for y, title, _, theme, label in inputs:
        s.card(680, y - 28, 184, 56, title, [], theme, title_size=14, center=True)
        s.rect(553, y - 7, 14, 14, SAFE, "#444444", rx=3, sw=1)
        s.text(544, y + 4.5, label, size=12, weight=700, anchor="end", fill=INK2)
        s.arrow([(680, y), (567, y)], None, color=SAFE, sw=2.6)

    s.text(16, 392, "The emergency stop switch and the two door open sensors connect directly to the platform.", size=12.5, fill=INK2)
    s.text(16, 412, "Not drawn: the platform's own power, the cable types, and what the platform does when an input trips.", size=12.5, fill=INK2)
    return s


@diagram
def lighting():
    s = Svg(880, 628)
    WIRE = "#2B2F36"
    s.text(16, 26, "Lighting", size=17, weight=700)

    # the two sets of lights
    s.card(180, 50, 220, 90, "White flood lights", ["on in the left position"], "white", center=True)
    s.card(480, 50, 220, 90, "Red flight lights", ["on in the right position"], "red", center=True)

    # the three position switch: the supply goes to the common pole, the left and right contacts go to the lights, the middle contact goes nowhere
    s.rect(300, 200, 280, 160, "#F1F1EE", "#C9C9C4", rx=12, sw=1.2)
    s.text(292, 285, "Three position", size=13, weight=700, anchor="end")
    s.text(292, 303, "switch", size=13, weight=700, anchor="end")
    pivot, left, mid, right = (440, 330), (370, 262), (440, 262), (510, 262)
    s.line(pivot[0], pivot[1], left[0], left[1], "#B5B5B0", 2, dash="5 4")
    s.line(pivot[0], pivot[1], right[0], right[1], "#B5B5B0", 2, dash="5 4")
    s.line(pivot[0], pivot[1], mid[0], mid[1] + 4, WIRE, 4)
    for pt in (left, mid, right):
        s.circle(pt[0], pt[1], 6, "#FFFFFF", WIRE, 1.8)
    s.circle(pivot[0], pivot[1], 8, WIRE, WIRE, 1.2)
    s.text(left[0] - 14, left[1] + 5, "left", size=12.5, weight=700, anchor="end")
    s.text(mid[0], mid[1] - 14, "middle: off", size=12.5, weight=700, anchor="middle")
    s.text(right[0] + 14, right[1] + 5, "right", size=12.5, weight=700, anchor="start")

    # the wires
    s.arrow([(left[0], left[1] - 6), (left[0], 170), (290, 170), (290, 140)], None, color=WIRE, sw=2.6)
    s.arrow([(right[0], right[1] - 6), (right[0], 170), (590, 170), (590, 140)], None, color=WIRE, sw=2.6)
    s.text(346, 162, "left position", size=12, anchor="middle", fill=INK2)
    s.text(532, 162, "right position", size=12, anchor="middle", fill=INK2)

    # the dimming circuit between the switch and the ATX supply
    s.card(340, 402, 200, 58, "Dimming circuit", [], "amber", title_size=14, center=True)
    s.arrow([(440, 402), (440, 338)], None, color=WIRE, sw=2.6)
    s.text(452, 378, "dimmed power", size=12, anchor="start", fill=INK2)

    # the ATX supply
    s.card(340, 508, 200, 76, "ATX power supply", ["feeds the dimming circuit"], "gray", center=True)
    s.arrow([(440, 508), (440, 460)], None, color=WIRE, sw=2.6)
    s.text(452, 488, "power from the ATX supply", size=12, anchor="start", fill=INK2)

    s.text(16, 612, "The switch is drawn in the middle (off) position. The supply voltage, the dimming circuit's design and the wiring of the lights are not drawn.", size=12, fill=INK2)
    return s


# ---------------------------------------------------------------------------------------------------------------- small icons for the PC display diagram
DARK = "#2B2F36"
SCREEN = "#CFE3FB"


def icon_pc(s, cx, cy):
    """A PC tower, about 70 wide and 130 high, centred on cx, cy."""
    x, y, w, h = cx - 35, cy - 65, 70, 130
    s.rect(x, y, w, h, "#4A4F59", DARK, rx=6, sw=2)
    s.rect(x + 10, y + 14, w - 20, 8, "#2B2F36", "#1F1F1F", rx=2, sw=1)          # drive slots
    s.rect(x + 10, y + 30, w - 20, 8, "#2B2F36", "#1F1F1F", rx=2, sw=1)
    s.circle(cx, y + h - 34, 9, "#8FD18F", DARK, 2)                                # power button
    for k in range(4):
        s.line(x + 14, y + h - 14 + k * 0, x + w - 14, y + h - 14, "#2B2F36", 1.2) if k == 0 else None


def icon_screen(s, cx, cy, w, h, stand=True):
    """A flat screen (a television or a monitor) of width w and height h on a small stand, centred on cx, cy."""
    x, y = cx - w / 2, cy - h / 2 - (h * 0.12 if stand else 0)
    s.rect(x, y, w, h, DARK, DARK, rx=4, sw=2)
    s.rect(x + 4, y + 4, w - 8, h - 8, SCREEN, "#9DB8D8", rx=2, sw=1)
    s.line(x + w * 0.12, y + h * 0.78, x + w * 0.4, y + h * 0.25, "#FFFFFF", 2.2)  # a little glare
    if stand:
        s.line(cx, y + h, cx, y + h + h * 0.12, DARK, 3)
        s.line(cx - w * 0.2, y + h + h * 0.12, cx + w * 0.2, y + h + h * 0.12, DARK, 4)


def icon_projector(s, cx, cy, w=88, h=46):
    """A mini projector, lens to the right, with a little beam in front of it."""
    x, y = cx - w / 2 - 6, cy - h / 2
    s.path(f"M {x + w + 4:.1f} {cy - 6:.1f} L {x + w + 30:.1f} {cy - 20:.1f} L {x + w + 30:.1f} {cy + 20:.1f} L {x + w + 4:.1f} {cy + 6:.1f} Z", stroke="#E8C547", sw=1, fill="#FFF3C4")
    s.rect(x, y, w, h, "#E9E9E5", DARK, rx=8, sw=2)
    s.circle(x + w - 20, cy, 15, "#4A4F59", DARK, 2)
    s.circle(x + w - 20, cy, 8, "#9DB8D8", "#1F1F1F", 1.5)
    for k in range(3):
        s.line(x + 12, y + 12 + k * 9, x + 36, y + 12 + k * 9, "#9A9A95", 1.6)
    s.line(x + 14, y + h, x + 14, y + h + 6, DARK, 3)
    s.line(x + w - 14, y + h, x + w - 14, y + h + 6, DARK, 3)


@diagram
def pc_displays():
    s = Svg(880, 580)
    HDMI = "#00796B"
    s.text(16, 26, "PC displays", size=17, weight=700)
    s.line(600, 22, 640, 22, HDMI, 3)
    s.text(648, 26, "HDMI cable", size=12.5)

    # the PC
    s.rect(16, 50, 230, 500, "#F1F1EE", "#C9C9C4", rx=12, sw=1.2)
    icon_pc(s, 131, 270)
    s.text(131, 380, "PC", size=17, weight=700, anchor="middle")
    s.text(131, 400, "primary flight simulator PC", size=12.5, anchor="middle", fill=INK2)

    # the four displays, one row each, with the HDMI cable from the PC
    rows = [
        ("Primary forward display", "50 inch LCD", "tv"),
        ("Pilot footwell display", "18 inch LCD", "monitor"),
        ("Left mini projector", "", "projector"),
        ("Right mini projector", "", "projector"),
    ]
    for i, (title, line, kind) in enumerate(rows):
        cy = 110 + i * 120
        s.rect(480, cy - 52, 384, 104, "#FFFFFF", "#C9C9C4", rx=12, sw=1.2)
        icx = 556
        if kind == "tv":
            icon_screen(s, icx, cy + 2, 112, 66)
        elif kind == "monitor":
            icon_screen(s, icx, cy + 2, 62, 40)
        else:
            icon_projector(s, icx - 4, cy)
        s.text(630, cy - 2 if line else cy + 5, title, size=15, weight=700)
        if line:
            s.text(630, cy + 20, line, size=13.5, fill=INK2)
        # the HDMI port on the PC and the cable to the display
        s.rect(239, cy - 7, 14, 14, HDMI, "#444444", rx=3, sw=1)
        s.arrow([(253, cy), (480, cy)], "HDMI", color=HDMI, sw=2.8, label_at=0.5, label_dy=-8)

    s.text(16, 572, "All four displays are connected to the PC by HDMI.", size=12.5, fill=INK2)
    return s


# ---------------------------------------------------------------------------------------------------------------- icons for the sound system diagram
def icon_amp(s, cx, cy, w=120, h=52):
    """An audio amplifier: a flat box with a display strip and two knobs."""
    x, y = cx - w / 2, cy - h / 2
    s.rect(x, y, w, h, "#D9DBDF", DARK, rx=5, sw=2)
    s.rect(x + 8, y + 10, w * 0.34, h - 20, DARK, DARK, rx=3, sw=1)
    s.line(x + 14, cy, x + 8 + w * 0.34 - 6, cy, "#8FD18F", 2)
    for k in range(2):
        kx = x + w * 0.62 + k * w * 0.2
        s.circle(kx, cy, 9, "#F4F4F2", DARK, 2)
        s.line(kx, cy, kx, cy - 7, DARK, 2)


def icon_speaker(s, cx, cy, w=46, h=64):
    """A loudspeaker cabinet with a tweeter and a woofer."""
    x, y = cx - w / 2, cy - h / 2
    s.rect(x, y, w, h, "#4A4F59", DARK, rx=5, sw=2)
    s.circle(cx, y + h * 0.26, w * 0.15, "#2B2F36", "#B5B5B0", 1.6)
    s.circle(cx, y + h * 0.65, w * 0.3, "#2B2F36", "#B5B5B0", 2)
    s.circle(cx, y + h * 0.65, w * 0.11, "#6B707A", "#B5B5B0", 1.2)


def icon_headset(s, cx, cy, size=64):
    """An aviation headset: a headband, two ear cups and a microphone boom."""
    r = size * 0.42
    s.path(f"M {cx - r:.1f} {cy + 2:.1f} A {r:.1f} {r:.1f} 0 0 1 {cx + r:.1f} {cy + 2:.1f}", stroke=DARK, sw=4)
    for sx in (-1, 1):
        s.rect(cx + sx * r - 7, cy - 2, 14, size * 0.42, "#4A4F59", DARK, rx=5, sw=2)
    s.path(f"M {cx - r:.1f} {cy + size * 0.38:.1f} Q {cx - r:.1f} {cy + size * 0.62:.1f} {cx - r * 0.2:.1f} {cy + size * 0.6:.1f}", stroke=DARK, sw=2.4)
    s.circle(cx - r * 0.2, cy + size * 0.6, 4, DARK, DARK, 1)


def icon_soundcard(s, cx, cy, w=84, h=40):
    """An external USB sound card: a small box with jacks on the front."""
    x, y = cx - w / 2, cy - h / 2
    s.rect(x, y, w, h, "#2B6CB0", DARK, rx=6, sw=2)
    s.circle(x + 16, cy, 8, "#DDE8F5", DARK, 1.6)
    s.circle(x + 16, cy, 3, DARK, DARK, 1)
    for k in range(3):
        s.circle(x + 38 + k * 15, cy, 4.5, "#1F1F1F", "#DDE8F5", 1.4)


def icon_rotary(s, cx, cy, r=30):
    """A two way rotary switch: a knob with a pointer, and the two positions marked."""
    s.circle(cx, cy, r, "#E9E9E5", DARK, 2.4)
    s.circle(cx, cy, r * 0.62, "#B5B5B0", DARK, 1.6)
    s.line(cx, cy, cx + r * 0.52, cy - r * 0.52, DARK, 4)
    s.text(cx + r + 8, cy - r * 0.72, "1", size=13, weight=700, anchor="start")
    s.text(cx + r + 8, cy + r * 0.95, "2", size=13, weight=700, anchor="start")


@diagram
def sound_system():
    s = Svg(880, 736)
    ANALOG, USB = AMBER, BLUE
    s.text(16, 26, "Sound system", size=17, weight=700)
    s.line(540, 22, 580, 22, ANALOG, 3)
    s.text(588, 26, "audio cable", size=12.5)
    s.line(690, 22, 730, 22, USB, 3)
    s.text(738, 26, "USB cable", size=12.5)

    # the PC, with the analog outputs at the top and the USB socket at the bottom
    s.rect(16, 50, 150, 650, "#F1F1EE", "#C9C9C4", rx=12, sw=1.2)
    icon_pc(s, 91, 360)
    s.text(91, 460, "PC", size=17, weight=700, anchor="middle")
    s.rect(159, 138, 14, 14, ANALOG, "#444444", rx=3, sw=1)
    s.rect(159, 538, 14, 14, USB, "#444444", rx=3, sw=1)

    # the primary sound system: the Cambridge Audio amplifiers and four speakers
    s.text(214, 60, "Primary sound system", size=14, weight=700)
    s.rect(214, 74, 160, 142, "#FFFFFF", "#C9C9C4", rx=12, sw=1.2)
    icon_amp(s, 294, 120)
    s.text(294, 168, "Cambridge Audio", size=13.5, weight=700, anchor="middle")
    s.text(294, 187, "amplifiers", size=13.5, weight=700, anchor="middle")
    s.arrow([(173, 145), (214, 145)], "analog", color=ANALOG, sw=2.8, label_dy=-9)
    speakers = [("Front left", 460, 84), ("Front right", 670, 84), ("Rear left", 460, 160), ("Rear right", 670, 160)]
    for title, x, y in speakers:
        s.rect(x, y, 190, 66, "#FFFFFF", "#C9C9C4", rx=10, sw=1.2)
        icon_speaker(s, x + 34, y + 33, 36, 50)
        s.text(x + 66, y + 38, title + " speaker", size=13.5, weight=700, anchor="start")
    # cables from the amplifiers to the four speakers
    s.arrow([(374, 145), (420, 145), (420, 117), (460, 117)], None, color=ANALOG, sw=2.4)
    s.arrow([(420, 145), (420, 193), (460, 193)], None, color=ANALOG, sw=2.4)
    s.arrow([(420, 117), (420, 66), (765, 66), (765, 84)], None, color=ANALOG, sw=2.4)
    s.arrow([(420, 193), (420, 242), (765, 242), (765, 226)], None, color=ANALOG, sw=2.4)

    # the second sound system: the USB sound card, the rotary switch and the two places it can go
    s.text(214, 330, "Headset and power speakers", size=14, weight=700)
    s.rect(214, 480, 130, 80, "#FFFFFF", "#C9C9C4", rx=12, sw=1.2)
    icon_soundcard(s, 279, 508)
    s.text(279, 546, "Sound Blaster", size=13, weight=700, anchor="middle")
    s.arrow([(173, 545), (186, 545), (186, 520), (214, 520)], "USB", color=USB, sw=2.8, label_at=0.5, label_dy=-9, label_dx=-8)
    s.rect(376, 470, 152, 100, "#FFFFFF", "#C9C9C4", rx=12, sw=1.2)
    icon_rotary(s, 436, 514, 28)
    s.text(452, 557, "Two way rotary switch", size=12, weight=700, anchor="middle")
    s.arrow([(344, 520), (376, 520)], None, color=ANALOG, sw=2.6)

    # position 1: the aviation headset amplifier and the two headsets
    s.rect(590, 350, 140, 100, "#FFFFFF", "#C9C9C4", rx=12, sw=1.2)
    icon_amp(s, 660, 384, 96, 42)
    s.text(660, 424, "Aviation headset", size=12.5, weight=700, anchor="middle")
    s.text(660, 440, "amplifier", size=12.5, weight=700, anchor="middle")
    s.rect(764, 330, 100, 90, "#FFFFFF", "#C9C9C4", rx=10, sw=1.2)
    icon_headset(s, 814, 366, 52)
    s.text(814, 410, "Pilot headset", size=12, weight=700, anchor="middle")
    s.rect(764, 436, 100, 90, "#FFFFFF", "#C9C9C4", rx=10, sw=1.2)
    icon_headset(s, 814, 472, 52)
    s.text(814, 516, "Copilot headset", size=12, weight=700, anchor="middle")
    s.arrow([(528, 520), (558, 520), (558, 400), (590, 400)], "position 1", color=ANALOG, sw=2.6, label_at=0.5, label_dx=-34, label_dy=0)
    s.arrow([(730, 400), (747, 400), (747, 375), (764, 375)], None, color=ANALOG, sw=2.2)
    s.arrow([(747, 400), (747, 481), (764, 481)], None, color=ANALOG, sw=2.2)

    # position 2: the powered speakers at the upper rear of the pit
    s.rect(590, 586, 274, 120, "#FFFFFF", "#C9C9C4", rx=12, sw=1.2)
    s.text(727, 612, "Powered speakers", size=13, weight=700, anchor="middle")
    icon_speaker(s, 650, 653, 40, 56)
    icon_speaker(s, 800, 653, 40, 56)
    s.text(650, 696, "upper rear left", size=11.5, anchor="middle", fill=INK2)
    s.text(800, 696, "upper rear right", size=11.5, anchor="middle", fill=INK2)
    s.arrow([(558, 520), (558, 656), (590, 656)], "position 2", color=ANALOG, sw=2.6, label_at=0.5, label_dx=-34, label_dy=0)

    s.text(16, 728, "The sound card feeds either the headset amplifier or the powered speakers, never both at once.", size=12, fill=INK2)
    return s
