"""Physical connectivity of each controller board: which connector goes to which panel, gauge or lamp group."""
from dgcommon import diagram, Svg, INK, INK2, BLUE, GREEN, AMBER, RED

SIG, BACK, PWR, NET = BLUE, AMBER, RED, GREEN


def _connectivity(name, ip, hw, infra, panels, note=None):
    """infra: [(title, lines, colour, to_board)]; panels: [dict(title, lines, conns=[(label, colour)], theme)]."""
    W = 880
    px, pw = 540, 326
    # card geometry for the panels
    y = 20
    cards = []
    for p in panels:
        n, m = len(p["conns"]), len(p["lines"])
        h = max(18 + 26 * n, 44 + 18 * m) + 8
        cards.append((y, h))
        y += h + 8
    top, bottom = 20, y - 8
    board_h = bottom - top
    H = max(bottom + 20 + (24 if note else 0), 260)
    s = Svg(W, H)
    bx, bw = 214, 130
    s.rect(bx, top, bw, board_h, "#1F6B43", "#0A2A1A", rx=10)
    cy = top + board_h / 2
    for i, t in enumerate((name, ip, hw[0], hw[1])):
        size, wt, fill = ((15, 700, "#FFFFFF"), (12.5, 700, "#DDFBE8"), (11, 400, "#BFE9CE"), (11, 400, "#BFE9CE"))[i]
        s.text(bx + bw / 2, min(cy - 40, top + 80) + i * 20 + (0 if i < 2 else 4), t, size=size, weight=wt, fill=fill, anchor="middle")
    # infrastructure on the left
    ix, iw = 14, 140
    ih = 62
    gap = 12
    total = len(infra) * ih + (len(infra) - 1) * gap
    iy = cy - total / 2
    for t, lines, col, inward in infra:
        s.card(ix, iy, iw, ih, t, lines, "white", title_size=13, line_size=11.5)
        yy = iy + ih / 2
        pts = [(ix + iw, yy), (bx, yy)]
        s.arrow(pts if inward else pts[::-1], None, color=col, both=(col == NET))
        iy += ih + gap
    # panels
    for p, (cy0, h) in zip(panels, cards):
        s.card(px, cy0, pw, h, p["title"], p["lines"], p.get("theme", "white"), title_size=14, line_size=12)
        for k, (label, col) in enumerate(p["conns"]):
            yy = cy0 + 24 + k * 26
            s.arrow([(bx + bw, yy), (px, yy)], label, color=col, label_dy=-4)
    if note:
        s.text(14, H - 12, note, size=11.5, fill=INK2)
    return s


LEG = "Cable colours: blue = signal, amber = backlight, red = power, green = network and USB."


@diagram
def radio_connectivity():
    infra = [("Ethernet switch", ["W5500, 172.16.1.101"], NET, True),
             ("USB hub", ["COM20 (programming)"], NET, True),
             ("5 V supply", ["J4 5V IN"], PWR, True),
             ("12 to 20 V supply", ["J7 STEPPER IN"], PWR, True)]
    panels = [
        dict(title="COM and NAV panel", lines=["8 encoders, 4 swap switches,", "COM and NAV frequency OLEDs"], conns=[("J8 COMM_NAV  IDC 34", SIG)]),
        dict(title="Auto Pilot panel", lines=["CRS, ALT, VS, IAS, HDG knobs,", "8 LEDs, display"], conns=[("J12 AUTOPILOT  IDC 34", SIG)]),
        dict(title="Audio panel", lines=["audio switches, 11 LEDs, DME display"], conns=[("J3 AUDIO  IDC 20", SIG)]),
        dict(title="Huey Warning Panel", lines=["20 caution LEDs, 3 switches (dim, test, reset)"],
             conns=[("J22 HUEY ANODES  (LED rows)", SIG), ("J23 HUEY CATHODES  (LED columns)", SIG), ("J20 HUEY SWITCHES  (digital in)", SIG), ("J21 HUEY BK LIGHT", BACK)], theme="amber"),
    ]
    return _connectivity("Radio", "172.16.1.101", ("Radio Auto Pilot", "PCB 1.4"), infra, panels,
                         LEG + " The three main panel cables also carry their backlight (D5).")


@diagram
def servo_connectivity():
    infra = [("Ethernet switch", ["W5500, 172.16.1.102"], NET, True),
             ("USB hub", ["COM16 (programming)"], NET, True),
             ("5 V supply", ["J2 5V IN"], PWR, True),
             ("12 to 20 V supply", ["J3 STEPPER IN"], PWR, True)]
    panels = [
        dict(title="Attitude indicator", lines=["pitch servo (D26), bank servo (D27)"], conns=[("J42 PITCH  3-pin", SIG), ("J43 ROLL  3-pin", SIG)]),
        dict(title="Annunciator lamp panels", lines=["15 warning lamps on A1 to A15", "(the split between the two panels is not recorded)"],
             conns=[("J4 LHS ANNUC  IDC 16", SIG), ("J5 RHS ANNUC  IDC 16", SIG)]),
        dict(title="Front panel switches", lines=["TOT clear (D43), fuel valve (D48),", "warning mute (D47): to ground when on"],
             conns=[("SW1 TOT CLEAR", SIG), ("SW2 FUEL VALVE", SIG), ("SW3 WARNING MUTE", SIG)]),
        dict(title="Panel backlights", lines=["D5 PWM through the IRF540N"], conns=[("J6-J10, J15-J19  BK LIGHT x10", BACK)], theme="amber"),
        dict(title="Spare servo headers", lines=["J20 to J33, J44, J45: fitted, not used"], conns=[("not connected", "#C9C9C4")], theme="gray"),
    ]
    return _connectivity("Servo", "172.16.1.102", ("Front Panel Servo", "PCB 1.4"), infra, panels, LEG)


@diagram
def stepper_connectivity():
    infra = [("Ethernet switch", ["W5500, 172.16.1.105"], NET, True),
             ("USB hub", ["COM4 (programming)"], NET, True),
             ("5 V supply", ["J2 5V IN"], PWR, True),
             ("12 to 20 V supply", ["J3 STEPPER IN"], PWR, True)]
    g = [("Airspeed", "J26 AIRSPEED", "coils D12, D13, D22, D23"), ("Vertical speed", "J5 VVI", "coils D7, D8, D9, D11"),
         ("Radar altitude", "J23 RADAR ALT", "coils D32 to D35"), ("Torque", "J25 ENG TORQUE", "coils D36 to D39"),
         ("Gas producer (N1)", "J24 GAS PRODUCER", "coils D40 to D43"), ("Turbine out temperature", "J27 EGT", "coils A11 to A14"),
         ("Fuel quantity", "J4 FUEL LEVEL", "coils D2, D3, D4, D6"), ("Engine oil temperature (placeholder)", "J41 ENG OIL TEMP", "coils D48, A0 to A2"),
         ("Engine oil pressure (placeholder)", "J40 ENG OIL PRES", "coils D44 to D47"), ("Transmission oil temperature (placeholder)", "J30 XMSN OIL TEMP", "coils A3 to A6"),
         ("Transmission oil pressure (placeholder)", "J39 XMSN OIL PRESS", "coils A7 to A10")]
    panels = [dict(title=t, lines=["X27 stepper gauge, " + c], conns=[(j + "  6-pin", SIG)]) for t, j, c in g]
    panels.append(dict(title="Backlights", lines=["each gauge's pin 6, switched by D5 (IRF540N)"], conns=[("in the six-pin gauge cables", BACK)], theme="amber"))
    panels.append(dict(title="Turbine speed, rotor speed", lines=["J20 TURBINE SPEED, J22 ROTOR SPEED: not used", "(these gauges moved to the Dual Stepper)"], conns=[("not connected", "#C9C9C4")], theme="gray"))
    return _connectivity("Stepper", "172.16.1.105", ("13 Stepper", "PCB 1.6"), infra, panels, LEG)


@diagram
def dual_stepper_connectivity():
    infra = [("Ethernet switch", ["W5500, 172.16.1.106"], NET, True),
             ("USB hub", ["COM5 (programming)"], NET, True),
             ("5 V supply", ["J2 5V IN"], PWR, True),
             ("12 to 20 V supply", ["J3 STEPPER IN"], PWR, True)]
    panels = [
        dict(title="Engine oil gauge", lines=["dual instrument: pressure and temperature"], conns=[("J8 ENGINE  10-pin", SIG)]),
        dict(title="Transmission oil gauge", lines=["dual instrument: pressure and temperature"], conns=[("J6 XMSN  10-pin", SIG)]),
        dict(title="Rotor and engine speed gauge", lines=["dual instrument: rotor RPM and engine N2"], conns=[("J7 ROTOR ENG SPEED  10-pin", SIG)]),
        dict(title="Electrical and fuel load gauge", lines=["dual instrument: electrical load and fuel load"], conns=[("J10 ELEC FUEL LOAD  10-pin", SIG)]),
        dict(title="Altimeter", lines=["stepper dial and its zero sensor"], conns=[("J33 ALT  6-pin", SIG), ("J9 ALTIMETER ZERO  3-pin", SIG)]),
        dict(title="Barometer and altimeter OLEDs", lines=["I2C through the TCA9548A multiplexer"], conns=[("J55 BARO-OLED  4-pin", SIG), ("J17 ALTIMETER-OLED  4-pin", SIG)]),
        dict(title="Clock and temperature OLEDs", lines=["I2C through the TCA9548A multiplexer"], conns=[("J18 CLOCK-OLED  4-pin", SIG), ("J20 TEMPERATURE-OLED  4-pin", SIG)]),
        dict(title="Backlights", lines=["each gauge's pin 6, switched by D5 (IRF540N)"], conns=[("in the gauge cables", BACK)], theme="amber"),
        dict(title="Other gauge sockets", lines=["J4 FUEL LEVEL, J5 VVI, J19 ALTIMETER OLED,", "J26 AIRSPEED, J27 EGT: on the PCB, not drawn in the", "instrument map"], conns=[("not shown", "#C9C9C4")], theme="gray"),
    ]
    return _connectivity("Dual Stepper", "172.16.1.106", ("OLED Dual Stepper", "PCB 1.5"), infra, panels, LEG)


@diagram
def adf_connectivity():
    infra = [("Ethernet switch", ["W5500, 172.16.1.107"], NET, True),
             ("USB hub", ["COM26 (programming)"], NET, True),
             ("USB HID board", ["buttons and keys, UDP 4210"], NET, False),
             ("5 V supply", ["J4 5V IN"], PWR, True),
             ("12 to 20 V supply", ["J7 STEPPER IN"], PWR, True)]
    panels = [
        dict(title="ADF panel", lines=["ADF knobs, keypad, switches, OLED,", "3 landing gear lamps"], conns=[("J5 ADF  IDC 26", SIG)]),
        dict(title="Lower panel switches", lines=["hyd boost (D35), anti-ice (D36), caution dim (D37)"], conns=[("J20 HYD BOOST  6-pin", SIG)]),
        dict(title="Upper panel", lines=["72-switch matrix, numeric keypad,", "brightness pot (analog, switched off for now)"],
             conns=[("J30 ROWS  8-pin", SIG), ("J31 COLS  8-pin", SIG), ("J32 KEYPAD  8-pin", SIG), ("J16 GUAGE BRIGHT  3-pin", SIG)], theme="teal"),
        dict(title="Warning mute and TOT test switches", lines=["on the matrix ROW8 (indexes 88 and 89)"], conns=[("J17 WARNING MUTE", SIG), ("J18 TOT TEST", SIG)]),
        dict(title="Panel backlight", lines=["D5 PWM through the IRF540N"], conns=[("J33 BACKLIGHT", BACK)], theme="amber"),
    ]
    return _connectivity("ADF + Upper", "172.16.1.107", ("Upper ADF Controller", "PCB 1.0"), infra, panels, LEG + " J5 also carries the ADF panel backlight.")
