"""Draws the A3 instrument map: every gauge, lamp and display of the Jet Ranger cockpit, drawn as the real thing (dial gauges with needles, attitude
indicator, annunciator lamps, panel legend lamps, gear lights, OLED displays) and joined by cables to the connector on the controller board that drives it.

    python make_instrument_map.py

Writes "../Jet Ranger Instrument Map A3.svg" (vector, 420 x 297 mm), ".pdf" (A3 landscape) and ".png" (200 dpi preview). The PDF and PNG are made with
headless Edge. The connector references (J26, J42 ...) and names are the ones printed on the PCBs (see Appendix F of the master document).
"""
import math, os, subprocess, time, sys

HERE = os.path.dirname(os.path.abspath(__file__))
DOCS = os.path.dirname(HERE)
BASE = os.path.join(DOCS, "Jet Ranger Instrument Map A3")
EDGE = r"C:\Program Files (x86)\Microsoft\Edge\Application\msedge.exe"

W, H = 1680, 1188            # 1 unit = 0.25 mm: 420 x 297 mm, A3 landscape
FONT = "Liberation Sans, Arial, Helvetica, sans-serif"
MONO = "Liberation Mono, Consolas, Courier New, monospace"

STEPPER_C, DUAL_C, SERVO_C, RADIO_C, ADF_C = "#C77700", "#6A3FC7", "#2E7D32", "#2F6FEB", "#00796B"

out = []
ADI_COUNT = 0


def add(s):
    out.append(s)


def esc(s):
    return s.replace("&", "&amp;").replace("<", "&lt;").replace(">", "&gt;")


def text(x, y, s, size=10, fill="#1F1F1F", weight=400, anchor="middle", family=FONT, rot=None, spacing=None, extra="", halo=False):
    t = ' transform="rotate(%s %.1f %.1f)"' % (rot, x, y) if rot is not None else ""
    ls = ' letter-spacing="%s"' % spacing if spacing else ""
    if halo:
        extra += ' stroke="#fff" stroke-width="2.6" stroke-linejoin="round" paint-order="stroke"'
    add('<text x="%.1f" y="%.1f" font-family="%s" font-size="%s" font-weight="%s" fill="%s" text-anchor="%s"%s%s %s>%s</text>'
        % (x, y, family, size, weight, fill, anchor, t, ls, extra, esc(s)))


def polar(cx, cy, r, deg):
    a = math.radians(deg)
    return cx + r * math.sin(a), cy - r * math.cos(a)


# ----------------------------------------------------------------------------------------------------------------------------- definitions
def defs():
    add('<defs>')
    add('<linearGradient id="bezel" x1="0" y1="0" x2="1" y2="1"><stop offset="0" stop-color="#F4F4F2"/><stop offset="0.35" stop-color="#9A9A98"/>'
        '<stop offset="0.7" stop-color="#E3E3E0"/><stop offset="1" stop-color="#6E6E6C"/></linearGradient>')
    add('<linearGradient id="bezelR" x1="1" y1="0" x2="0" y2="1"><stop offset="0" stop-color="#F4F4F2"/><stop offset="0.4" stop-color="#8C8C8A"/>'
        '<stop offset="1" stop-color="#DADAD7"/></linearGradient>')
    add('<radialGradient id="face" cx="0.4" cy="0.35" r="0.9"><stop offset="0" stop-color="#2B2B2B"/><stop offset="1" stop-color="#0B0B0B"/></radialGradient>')
    add('<radialGradient id="glass" cx="0.3" cy="0.25" r="0.8"><stop offset="0" stop-color="#FFFFFF" stop-opacity="0.35"/><stop offset="0.5" stop-color="#FFFFFF" stop-opacity="0.05"/>'
        '<stop offset="1" stop-color="#FFFFFF" stop-opacity="0"/></radialGradient>')
    add('<radialGradient id="hub" cx="0.35" cy="0.3" r="0.8"><stop offset="0" stop-color="#EDEDED"/><stop offset="1" stop-color="#4A4A4A"/></radialGradient>')
    for name, c0, c1, c2 in (("amber", "#FFF1B0", "#FFB300", "#B96A00"), ("red", "#FFC4B8", "#F2331F", "#8F1208"),
                             ("green", "#D5FFC9", "#3FD13F", "#14721C"), ("blue", "#CFE6FF", "#3C8CFF", "#1448A8"),
                             ("white", "#FFFFFF", "#EDEDED", "#9A9A9A")):
        add('<radialGradient id="lens_%s" cx="0.5" cy="0.45" r="0.75"><stop offset="0" stop-color="%s"/><stop offset="0.55" stop-color="%s"/><stop offset="1" stop-color="%s"/></radialGradient>' % (name, c0, c1, c2))
    add('<linearGradient id="housing" x1="0" y1="0" x2="0" y2="1"><stop offset="0" stop-color="#4A4D52"/><stop offset="1" stop-color="#26282B"/></linearGradient>')
    add('<linearGradient id="pcb" x1="0" y1="0" x2="1" y2="1"><stop offset="0" stop-color="#1F6B45"/><stop offset="1" stop-color="#134A2F"/></linearGradient>')
    add('<linearGradient id="sky" x1="0" y1="0" x2="0" y2="1"><stop offset="0" stop-color="#5BA8E8"/><stop offset="1" stop-color="#9ACBF2"/></linearGradient>')
    add('<linearGradient id="ground" x1="0" y1="0" x2="0" y2="1"><stop offset="0" stop-color="#A5733E"/><stop offset="1" stop-color="#6B4423"/></linearGradient>')
    add('<filter id="shadow" x="-20%" y="-20%" width="140%" height="140%"><feDropShadow dx="1.2" dy="1.6" stdDeviation="1.6" flood-color="#000" flood-opacity="0.35"/></filter>')
    add('<filter id="glow" x="-40%" y="-40%" width="180%" height="180%"><feGaussianBlur stdDeviation="2.2" result="b"/><feMerge><feMergeNode in="b"/><feMergeNode in="SourceGraphic"/></feMerge></filter>')
    add('</defs>')


# ----------------------------------------------------------------------------------------------------------------------------- gauges
def dial(cx, cy, r, g, uid):
    """One round dial: bezel, black face, scale with numerals, legend, needle. g is a dict (see GAUGES)."""
    add('<g filter="url(#shadow)">')
    add('<circle cx="%.1f" cy="%.1f" r="%.1f" fill="url(#bezel)" stroke="#4a4a4a" stroke-width="0.6"/>' % (cx, cy, r))
    add('<circle cx="%.1f" cy="%.1f" r="%.1f" fill="#222" stroke="#000" stroke-width="0.5"/>' % (cx, cy, r * 0.9))
    add('</g>')
    add('<circle cx="%.1f" cy="%.1f" r="%.1f" fill="url(#face)"/>' % (cx, cy, r * 0.86))
    v0, v1, a0, a1 = g["range"][0], g["range"][1], g["angles"][0], g["angles"][1]

    def ang(v):
        return a0 + (a1 - a0) * (v - v0) / (v1 - v0)
    if g.get("arc"):
        for (s, e, col) in g["arc"]:
            p1 = polar(cx, cy, r * 0.8, ang(s))
            p2 = polar(cx, cy, r * 0.8, ang(e))
            large = 1 if abs(ang(e) - ang(s)) > 180 else 0
            add('<path d="M %.1f %.1f A %.1f %.1f 0 %d 1 %.1f %.1f" fill="none" stroke="%s" stroke-width="%.1f"/>' % (p1[0], p1[1], r * 0.8, r * 0.8, large, p2[0], p2[1], col, r * 0.07))
    major, minors = g["major"], g.get("minor", 1)
    n = int(round((v1 - v0) / major))
    for i in range(n + 1):
        v = v0 + i * major
        p_out, p_in = polar(cx, cy, r * 0.78, ang(v)), polar(cx, cy, r * 0.64, ang(v))
        add('<line x1="%.1f" y1="%.1f" x2="%.1f" y2="%.1f" stroke="#F2F2F2" stroke-width="%.2f"/>' % (p_out[0], p_out[1], p_in[0], p_in[1], max(0.8, r * 0.04)))
        if i < n:
            for k in range(1, minors):
                vm = v + k * major / minors
                q1, q2 = polar(cx, cy, r * 0.78, ang(vm)), polar(cx, cy, r * 0.71, ang(vm))
                add('<line x1="%.1f" y1="%.1f" x2="%.1f" y2="%.1f" stroke="#D8D8D8" stroke-width="%.2f"/>' % (q1[0], q1[1], q2[0], q2[1], max(0.5, r * 0.02)))
        lab = g["label"](v) if g.get("label") else None
        if lab is not None and lab != "":
            lx, ly = polar(cx, cy, r * 0.52, ang(v))
            text(lx, ly + r * 0.06, str(lab), size=round(r * 0.18, 1), fill="#F5F5F5", weight=700)
    # legend
    ly0, lstep = g.get("legend_pos", (0.56, 0.17))
    for i, line in enumerate(g["legend"]):
        text(cx, cy + r * (ly0 + lstep * i), line, size=round(r * (0.13 if i else 0.14), 1), fill="#E8E8E8" if i == 0 else "#BDBDBD", weight=700 if i == 0 else 400)
    # needle
    na = ang(g["value"])
    tip = polar(cx, cy, r * 0.74, na)
    tail = polar(cx, cy, r * 0.16, na + 180)
    left = polar(cx, cy, r * 0.045, na - 90)
    right = polar(cx, cy, r * 0.045, na + 90)
    add('<polygon points="%.1f,%.1f %.1f,%.1f %.1f,%.1f %.1f,%.1f" fill="#F7F7F7" stroke="#222" stroke-width="0.4"/>'
        % (tip[0], tip[1], right[0], right[1], tail[0], tail[1], left[0], left[1]))
    add('<circle cx="%.1f" cy="%.1f" r="%.1f" fill="url(#hub)" stroke="#222" stroke-width="0.4"/>' % (cx, cy, r * 0.09))
    add('<ellipse cx="%.1f" cy="%.1f" rx="%.1f" ry="%.1f" fill="url(#glass)"/>' % (cx - r * 0.18, cy - r * 0.32, r * 0.55, r * 0.4))


def lab_div(d):
    return lambda v: "%g" % (v / d)


def lab_every(step, div=1):
    return lambda v: ("%g" % (v / div)) if abs((v / step) - round(v / step)) < 1e-6 else ""


def dual(cx, cy, r, left, right):
    w = r * 4.5
    add('<g filter="url(#shadow)"><rect x="%.1f" y="%.1f" width="%.1f" height="%.1f" rx="%.1f" fill="url(#housing)" stroke="#111" stroke-width="0.8"/></g>' % (cx - w / 2, cy - r * 1.25, w, r * 2.5, r * 0.3))
    dial(cx - r * 1.08, cy, r, left, "l")
    dial(cx + r * 1.08, cy, r, right, "r")


def altimeter(cx, cy, r):
    g = dict(range=(0, 10), angles=(0, 360), major=1, minor=5, label=lambda v: "" if v == 10 else "%d" % v, legend=["ALT"], value=3.2, legend_pos=(-0.2, 0.2))
    dial(cx, cy, r, g, "alt")


def attitude(cx, cy, r, bank=12, pitch=6):
    add('<g filter="url(#shadow)"><circle cx="%.1f" cy="%.1f" r="%.1f" fill="url(#bezel)" stroke="#4a4a4a" stroke-width="0.6"/></g>' % (cx, cy, r))
    add('<circle cx="%.1f" cy="%.1f" r="%.1f" fill="#111"/>' % (cx, cy, r * 0.9))
    global ADI_COUNT
    ADI_COUNT += 1
    cid = "adiclip%d" % ADI_COUNT
    add('<clipPath id="%s"><circle cx="%.1f" cy="%.1f" r="%.1f"/></clipPath>' % (cid, cx, cy, r * 0.84))
    add('<g clip-path="url(#%s)"><g transform="rotate(%s %.1f %.1f) translate(0 %.1f)">' % (cid, bank, cx, cy, pitch * r * 0.02))
    add('<rect x="%.1f" y="%.1f" width="%.1f" height="%.1f" fill="url(#sky)"/>' % (cx - r * 2, cy - r * 2, r * 4, r * 2))
    add('<rect x="%.1f" y="%.1f" width="%.1f" height="%.1f" fill="url(#ground)"/>' % (cx - r * 2, cy, r * 4, r * 2))
    add('<line x1="%.1f" y1="%.1f" x2="%.1f" y2="%.1f" stroke="#fff" stroke-width="1.4"/>' % (cx - r * 2, cy, cx + r * 2, cy))
    for k in (-20, -10, 10, 20):
        y = cy - k * r * 0.02 * 1.6
        wd = r * (0.28 if abs(k) == 10 else 0.4)
        add('<line x1="%.1f" y1="%.1f" x2="%.1f" y2="%.1f" stroke="#fff" stroke-width="0.9"/>' % (cx - wd, y, cx + wd, y))
    add('</g></g>')
    for d in (-60, -45, -30, -20, -10, 0, 10, 20, 30, 45, 60):
        p1, p2 = polar(cx, cy, r * 0.84, d), polar(cx, cy, r * (0.74 if d % 30 == 0 or d == 0 else 0.78), d)
        add('<line x1="%.1f" y1="%.1f" x2="%.1f" y2="%.1f" stroke="#fff" stroke-width="%.1f"/>' % (p1[0], p1[1], p2[0], p2[1], 1.4 if d % 30 == 0 else 0.8))
    tri = [polar(cx, cy, r * 0.74, 0 + bank), polar(cx, cy, r * 0.86, -5 + bank), polar(cx, cy, r * 0.86, 5 + bank)]
    add('<polygon points="%.1f,%.1f %.1f,%.1f %.1f,%.1f" fill="#FFD43B" stroke="#222" stroke-width="0.4"/>' % (tri[0][0], tri[0][1], tri[1][0], tri[1][1], tri[2][0], tri[2][1]))
    add('<path d="M %.1f %.1f h %.1f M %.1f %.1f h %.1f" stroke="#FFC400" stroke-width="2.6" stroke-linecap="round"/>' % (cx - r * 0.62, cy, r * 0.34, cx + r * 0.28, cy, r * 0.34))
    add('<path d="M %.1f %.1f v %.1f" stroke="#FFC400" stroke-width="2.6" stroke-linecap="round"/>' % (cx, cy - r * 0.04, r * 0.12))
    add('<circle cx="%.1f" cy="%.1f" r="%.1f" fill="#FFC400" stroke="#222" stroke-width="0.4"/>' % (cx, cy, r * 0.05))
    add('<ellipse cx="%.1f" cy="%.1f" rx="%.1f" ry="%.1f" fill="url(#glass)"/>' % (cx - r * 0.2, cy - r * 0.35, r * 0.55, r * 0.4))


# ----------------------------------------------------------------------------------------------------------------------------- lamps and displays
def lamp(x, y, w, h, lines, colour="amber", code=None, size=None, dark=False):
    """An annunciator lamp: metal bezel, glowing lens with the legend engraved in it."""
    size = size or min(7.6, w / 9.5)
    add('<g filter="url(#shadow)"><rect x="%.1f" y="%.1f" width="%.1f" height="%.1f" rx="3" fill="url(#bezel)" stroke="#444" stroke-width="0.5"/></g>' % (x, y, w, h))
    add('<rect x="%.1f" y="%.1f" width="%.1f" height="%.1f" rx="2" fill="url(#lens_%s)" stroke="#5a3a00" stroke-width="0.4"/>' % (x + 2.2, y + 2.2, w - 4.4, h - 4.4, colour))
    add('<rect x="%.1f" y="%.1f" width="%.1f" height="%.1f" rx="2" fill="#fff" opacity="0.28"/>' % (x + 3.2, y + 3.0, w - 6.4, (h - 4.4) * 0.38))
    n = len([l for l in lines if l])
    top = y + h / 2 - (n - 1) * size * 0.58 + size * 0.34
    col = "#FFFFFF" if colour in ("red", "blue", "green") else "#2B1A00"
    for i, ln in enumerate([l for l in lines if l]):
        text(x + w / 2, top + i * size * 1.16, ln, size=size, fill=col, weight=700)
    if code:
        text(x + w / 2, y + h + 7.5, code, size=5.8, fill="#666", weight=400)


def gear_lamp(cx, cy, r, letter):
    add('<g filter="url(#shadow)"><circle cx="%.1f" cy="%.1f" r="%.1f" fill="url(#bezelR)" stroke="#444" stroke-width="0.5"/></g>' % (cx, cy, r))
    add('<circle cx="%.1f" cy="%.1f" r="%.1f" fill="url(#lens_green)" stroke="#0b3d12" stroke-width="0.5"/>' % (cx, cy, r * 0.78))
    add('<ellipse cx="%.1f" cy="%.1f" rx="%.1f" ry="%.1f" fill="#fff" opacity="0.35"/>' % (cx - r * 0.12, cy - r * 0.32, r * 0.45, r * 0.25))
    text(cx, cy + r * 0.24, letter, size=round(r * 0.8, 1), fill="#FFFFFF", weight=700)


def oled(x, y, w, h, lines, size=None):
    size = size or min(9, h / (len(lines) * 1.25 + 0.4))
    add('<g filter="url(#shadow)"><rect x="%.1f" y="%.1f" width="%.1f" height="%.1f" rx="3" fill="#2E3034" stroke="#111" stroke-width="0.6"/></g>' % (x, y, w, h))
    add('<rect x="%.1f" y="%.1f" width="%.1f" height="%.1f" rx="1.5" fill="#02060A"/>' % (x + 3, y + 3, w - 6, h - 6))
    n = len(lines)
    for i, ln in enumerate(lines):
        ty = y + h / 2 + (i - (n - 1) / 2) * size * 1.22 + size * 0.33
        text(x + w / 2, ty, ln, size=size, fill="#9FF0FF", family=MONO, weight=700, extra='filter="url(#glow)"')


def panel_frame(x, y, w, h, title, sub=None, colour="#888", bottom=False):
    add('<rect x="%.1f" y="%.1f" width="%.1f" height="%.1f" rx="6" fill="#FAFAF8" stroke="%s" stroke-width="1.1"/>' % (x, y, w, h, colour))
    ty = y + h - 8 if bottom else y + 13
    text(x + 8, ty, title, size=10, fill="#222", weight=700, anchor="start")
    if sub:
        text(x + w - 8, ty, sub, size=8, fill="#777", anchor="end")


# ----------------------------------------------------------------------------------------------------------------------------- boards and cables
class Board:
    def __init__(self, x, y, w, h, title, ip, sketch, pcb, colour):
        self.x, self.y, self.w, self.h, self.colour = x, y, w, h, colour
        self.title, self.ip, self.sketch, self.pcb = title, ip, sketch, pcb
        self.top, self.bottom = [], []         # port lists, each (key, label)

    def draw(self):
        add('<g filter="url(#shadow)"><rect x="%.1f" y="%.1f" width="%.1f" height="%.1f" rx="7" fill="url(#pcb)" stroke="#0A2A1A" stroke-width="1.4"/></g>' % (self.x, self.y, self.w, self.h))
        add('<rect x="%.1f" y="%.1f" width="%.1f" height="5" rx="2" fill="%s"/>' % (self.x + 6, self.y + 5, self.w - 12, self.colour))
        cx = self.x + self.w / 2
        text(cx, self.y + 36, self.title, size=15, fill="#FFFFFF", weight=700)
        text(cx, self.y + 52, self.ip, size=11, fill="#DDFBE8", weight=700)
        text(cx, self.y + 67, self.sketch, size=7.6, fill="#BFE9CE", family=MONO)
        text(cx, self.y + 80, "Arduino Mega 2560 + W5500   |   " + self.pcb, size=7.2, fill="#9FD7B3")
        # silk "Mega" outline and ethernet module hint
        add('<rect x="%.1f" y="%.1f" width="%.1f" height="%.1f" rx="3" fill="none" stroke="#7FC79B" stroke-width="0.6" stroke-dasharray="3 2"/>' % (self.x + 14, self.y + 90, self.w - 28, self.h - 100))
        text(cx, self.y + self.h - 18, "ETHERNET  172.16.1.0/24", size=7, fill="#8CCFA7", spacing="1")

    def port_x(self, side, i, n):
        pad = 22
        span = self.w - 2 * pad
        return self.x + pad + span * (i + 0.5) / n

    def ports(self, side):
        return self.top if side == "top" else self.bottom


ports_drawn = []
cables = []


def cable(p1, p2, colour, vertical=True):
    """A cable between two points, drawn as a dark sleeve with a coloured core."""
    (x1, y1), (x2, y2) = p1, p2
    k = abs(y2 - y1) * 0.55
    d = "M %.1f %.1f C %.1f %.1f %.1f %.1f %.1f %.1f" % (x1, y1, x1, y1 + (k if y2 > y1 else -k), x2, y2 - (k if y2 > y1 else -k), x2, y2)
    cables.append((d, colour))


def draw_cables():
    for d, colour in cables:
        add('<path d="%s" fill="none" stroke="#1d1d1d" stroke-width="4.6" stroke-linecap="round"/>' % d)
    for d, colour in cables:
        add('<path d="%s" fill="none" stroke="%s" stroke-width="2.6" stroke-linecap="round"/>' % (d, colour))


def connector(board, side, x, label, pins=6, colour="#FFFFFF"):
    """A header on the board edge with its reference and name beside the cable."""
    y = board.y if side == "top" else board.y + board.h
    pw = max(18, min(34, pins * 4.2))
    yy = y - 3 if side == "top" else y - 7
    add('<rect x="%.1f" y="%.1f" width="%.1f" height="10" rx="1.5" fill="#EDE8D6" stroke="#6B6650" stroke-width="0.6"/>' % (x - pw / 2, yy, pw))
    for i in range(min(pins, 12)):
        px = x - pw / 2 + 3 + i * (pw - 6) / max(1, min(pins, 12) - 1) if pins > 1 else x
        add('<circle cx="%.1f" cy="%.1f" r="1.2" fill="#444"/>' % (px, yy + 5))
    add('<rect x="%.1f" y="%.1f" width="3" height="10" fill="#C0392B"/>' % (x - pw / 2, yy))        # pin 1 mark
    ref, _, name = label.partition(" ")
    if side == "top":
        text(x + 7.5, y - 10, label, size=8.4, fill="#111", weight=700, anchor="start", rot=-90, halo=True)
    else:
        text(x + 7.5, y + 10, label, size=8.4, fill="#111", weight=700, anchor="start", rot=90, halo=True)


def tag(x, y, s, colour):
    w = max(18, len(s) * 4.6 + 8)
    add('<rect x="%.1f" y="%.1f" width="%.1f" height="11" rx="5.5" fill="%s" stroke="#222" stroke-width="0.5"/>' % (x - w / 2, y - 5.5, w, colour))
    text(x, y + 2.8, s, size=7, fill="#FFFFFF", weight=700)


# ----------------------------------------------------------------------------------------------------------------------------- data
def G(legend, token, rng, angles, major, minor, label, value, arc=None):
    return dict(legend=legend, token=token, range=rng, angles=angles, major=major, minor=minor, label=label, value=value, arc=arc)


S270 = (-135, 135)
GAUGES_STEPPER = [
    ("J26 AIRSPEED", G(["AIRSPEED", "KNOTS"], "IAS", (0, 140), S270, 20, 2, lab_every(20), 80)),
    ("J5 VVI", G(["VERT SPEED", "x100 FT/MIN"], "VSI", (-1750, 1750), (-150, 150), 250, 1, lambda v: {0: "0", 500: "5", -500: "5", 1000: "10", -1000: "10", 1500: "15", -1500: "15"}.get(v, ""), 500)),
    ("J23 RADAR_ALT", G(["RADAR ALT", "x100 FT"], "AGL", (0, 2500), S270, 500, 5, lab_every(500, 100), 800)),
    ("J25 ENG_TORQUE", G(["TORQUE", "PERCENT"], "TQ", (0, 120), S270, 20, 2, lab_every(20), 62)),
    ("J24 GAS_PRODUCER", G(["GAS PROD", "N1 %"], "N1", (0, 110), S270, 10, 2, lab_every(20), 91)),
    ("J27 EGT", G(["TURBINE OUT", "TEMP x100 C"], "ITT", (100, 900), S270, 100, 2, lab_every(200, 100), 610)),
    ("J4 FUEL_LEVEL", G(["FUEL", "US GAL"], "FUEL", (0, 75), S270, 25, 5, lab_every(25), 40)),
]


def psi(legend, token, value):
    return G(legend, token, (0, 150), S270, 50, 5, lab_every(50), value)


DUALS = [
    ("J8 ENGINE", psi(["ENG OIL", "PSI"], "OILP", 62), psi(["ENG OIL", "TEMP C"], "OILT", 70)),
    ("J6 XMSN", psi(["XMSN OIL", "PSI"], "XMSNP", 55), psi(["XMSN OIL", "TEMP C"], "XMSNT", 60)),
    ("J7 ROTOR ENG SPEED", G(["ROTOR", "RPM %"], "RPMR", (0, 110), S270, 10, 2, lab_every(50), 100), G(["ENGINE", "N2 %"], "RPME", (0, 110), S270, 10, 2, lab_every(50), 100)),
    ("J10 ELEC FUEL LOAD", G(["ELEC LOAD", "PERCENT"], "ELECTRICALLOAD", (0, 100), S270, 25, 5, lab_every(25), 45), G(["FUEL", "PRESS PSI"], "FUELLOAD", (0, 30), S270, 10, 2, lab_every(10), 14)),
]

SERVO_LAMPS = [
    (("ENGINE", "OUT"), "red", "EOUT A1"), (("ROTOR", "LOW"), "red", "RLOW A2"), (("XMSN OIL", "PRESS"), "amber", "TOPW A3"),
    (("XMSN OIL", "TEMP"), "amber", "TOTW A4"), (("BATTERY", "HOT"), "amber", "BHOT A5"), (("BATTERY", "TEMP"), "amber", "BTMP A6"),
    (("ENGINE", "CHIP"), "amber", "EC A7"), (("TAIL ROTOR", "CHIP"), "amber", "TRC A8"), (("XMSN", "CHIP"), "amber", "TC A9"),
    (("BAGGAGE", "DOOR"), "amber", "BD A10"), (("FUEL", "PUMP"), "amber", "FPMP A11"), (("AFT FUEL", "FILTER"), "amber", "FFLTR A12"),
    (("GEN", "FAIL"), "amber", "GENF A13"), (("LOW", "FUEL"), "amber", "LOWF A14"), (("SC", "FAIL"), "amber", "SCF A15"),
]

HUEY_LAMPS = [
    ("ENG OIL", "PRESS"), ("ENG", "ICING"), ("ENG ICE", "JET"), ("ENG CHIP", "DET"), ("L FUEL", "BOOST"), ("R FUEL", "BOOST"),
    ("ENG FUEL", "PUMP"), ("20 MIN", "FUEL"), ("FUEL", "FILTER"), ("GOV", "EMER"),
    ("AUX FUEL", "LOW"), ("XMSN OIL", "PRESS"), ("XMSN OIL", "HOT"), ("HYD", "PRESS"), ("INLET", "AIR"), ("INST", "INVERTER"),
    ("DC", "GENERATOR"), ("EXT", "POWER"), ("CHIP", "DETECTOR"), ("IFF", ""),
]
AP_LEDS = [("CRS", "green"), ("HDG", "green"), ("IAS", "green"), ("ALT", "green"), ("APPR", "green"), ("FD", "green"), ("BC", "green"), ("AP", "green")]
AUDIO_LEDS = [("COM1", "green"), ("BOTH", "green"), ("NAV2", "green"), ("DME", "green"), ("OUTER", "blue"), ("INNER", "white"),
              ("COM2", "green"), ("NAV1", "green"), ("MKR", "green"), ("ADF", "green"), ("MIDDLE", "amber")]


# ----------------------------------------------------------------------------------------------------------------------------- the drawing
def build():
    out.clear()
    cables.clear()
    add('<svg xmlns="http://www.w3.org/2000/svg" width="420mm" height="297mm" viewBox="0 0 %d %d">' % (W, H))
    defs()
    add('<rect width="%d" height="%d" fill="#FFFFFF"/>' % (W, H))
    add('<rect x="8" y="8" width="%d" height="%d" rx="10" fill="none" stroke="#BBB" stroke-width="1.2"/>' % (W - 16, H - 16))

    text(28, 46, "Jet Ranger cockpit: gauges, lamps and displays, and the controller board that drives each one", size=28, fill="#1A1A1A", weight=700, anchor="start")
    text(28, 68, "Instrument map, A3 landscape. Needles and lamps are drawn at illustrative readings. Cables end at the named connector on the board (references as printed on the PCBs).", size=12.5, fill="#555", anchor="start")

    X = dict(stepper=(20, 372), dual=(372, 726), servo=(726, 1026), radio=(1026, 1494), adf=(1494, 1662))
    BY, BH = 540, 122
    boards = {
        "stepper": Board(X["stepper"][0] + 10, BY, X["stepper"][1] - X["stepper"][0] - 20, BH, "STEPPER CONTROLLER", "172.16.1.105", "JET_RANGER_STEPPER_CONTROLLER", "Jet Ranger 13 Stepper v1.6", STEPPER_C),
        "dual": Board(X["dual"][0] + 10, BY, X["dual"][1] - X["dual"][0] - 20, BH, "DUAL STEPPER CONTROLLER", "172.16.1.106", "JET_RANGER_OLED_DUAL_STEPPER_CONTROLLER", "OLED Dual Stepper v1.5", DUAL_C),
        "servo": Board(X["servo"][0] + 10, BY, X["servo"][1] - X["servo"][0] - 20, BH, "SERVO CONTROLLER", "172.16.1.102", "JET_RANGER_SERVO_CONTROLLER", "Front Panel Servo v1.4", SERVO_C),
        "radio": Board(X["radio"][0] + 10, BY, X["radio"][1] - X["radio"][0] - 20, BH, "RADIO CONTROLLER", "172.16.1.101", "JET_RANGER_RADIO_CONTROLLER", "Radio Auto Pilot v1.4", RADIO_C),
        "adf": Board(X["adf"][0] + 8, BY, X["adf"][1] - X["adf"][0] - 12, BH, "ADF + UPPER", "172.16.1.107", "JET_RANGER_ADF_CONTROLLER", "Upper ADF v1.0", ADF_C),
    }
    inst_draw = []

    def section(key, s):
        b = boards[key]
        text(b.x + 2, 98, s, size=11, fill="#444", weight=700, anchor="start")

    # ------------------------------------------------ Stepper: 7 gauges in rows of 3, 2 and 2
    sb = boards["stepper"]
    section("stepper", "STEPPER GAUGES  (X27 steppers)")
    r = 47
    rows = [(0, 3), (3, 5), (5, 7)]
    ports = []
    for ri, (a, b) in enumerate(rows):
        n = b - a
        x0 = sb.x + (sb.w - (n * 2 * r + (n - 1) * 16)) / 2 + r
        for i in range(a, b):
            ports.append((x0 + (i - a) * (2 * r + 16), 158 + ri * 114, GAUGES_STEPPER[i]))
    ports.sort(key=lambda p: (p[0], p[1]))
    for k, (cx, cy, (label, g)) in enumerate(ports):
        px = sb.port_x("top", k, len(ports))
        cable((cx, cy + r * 0.85), (px, sb.y), STEPPER_C)
        inst_draw.append(lambda cx=cx, cy=cy, g=g, label=label, px=px: (dial(cx, cy, r, g, label), tag(cx, cy + r + 9, g["token"], STEPPER_C), connector(sb, "top", px, label, pins=6)))

    # ------------------------------------------------ Dual stepper: 4 dual gauges + altimeter
    db = boards["dual"]
    section("dual", "DUAL STEPPER GAUGES  (dual instruments + altimeter)")
    r2 = 34
    colx = [db.x + 86, db.x + db.w - 86]
    items = []
    for n, (label, gl, gr) in enumerate(DUALS):
        items.append((colx[n % 2], 158 + (n // 2) * 114, label, gl, gr))
    items.append((db.x + db.w / 2, 388, "J33 ALT", None, None))
    items.sort(key=lambda p: (p[0], p[1]))
    n_top = len(items) + 1
    idx = 0
    for (cx, cy, label, gl, gr) in items:
        px = db.port_x("top", idx, n_top)
        idx += 1
        if gl is not None:
            cable((cx, cy + r2 * 1.25), (px, db.y), DUAL_C)
            inst_draw.append(lambda cx=cx, cy=cy, gl=gl, gr=gr, label=label, px=px: (dual(cx, cy, r2, gl, gr), tag(cx, cy + r2 * 1.25 + 9, gl["token"] + " / " + gr["token"], DUAL_C), connector(db, "top", px, label, pins=10)))
        else:
            pz = db.port_x("top", idx, n_top)
            idx += 1
            cable((cx - 12, cy + 42), (px, db.y), DUAL_C)
            cable((cx + 14, cy + 40), (pz, db.y), DUAL_C)
            inst_draw.append(lambda cx=cx, cy=cy, px=px, pz=pz: (altimeter(cx, cy, 46), tag(cx, cy + 55, "ALT", DUAL_C), connector(db, "top", px, "J33 ALT", pins=6), connector(db, "top", pz, "J9 ALTIMETER_ZERO", pins=3)))

    # ------------------------------------------------ Servo: attitude indicator
    vb = boards["servo"]
    section("servo", "ATTITUDE INDICATOR  (pitch and bank servos)")
    acx, acy, ar = vb.x + vb.w / 2, 242, 108
    pitch_x, roll_x = vb.port_x("top", 1, 4), vb.port_x("top", 2, 4)
    cable((acx - 30, acy + ar - 4), (pitch_x, vb.y), SERVO_C)
    cable((acx + 30, acy + ar - 4), (roll_x, vb.y), SERVO_C)
    inst_draw.append(lambda: (attitude(acx, acy, ar), tag(acx - 46, acy + ar + 10, "PITCH servo", SERVO_C), tag(acx + 46, acy + ar + 10, "ROLL servo", SERVO_C),
                              connector(vb, "top", pitch_x, "J42 PITCH", pins=3), connector(vb, "top", roll_x, "J43 ROLL", pins=3)))

    # ------------------------------------------------ Radio board: Huey panel above, three panels below
    rb = boards["radio"]
    huey_y = 300
    section("radio", "HUEY WARNING PANEL")
    panel_frame(rb.x, huey_y, rb.w, 160, "HUEY WARNING PANEL  (20 caution lamps on MAX7219 digits 4 - 7)", "connectors J20 - J23", RADIO_C)
    hw = (rb.w - 20) / 10
    for i, lines in enumerate(HUEY_LAMPS):
        lx = rb.x + 10 + (i % 10) * hw
        ly = huey_y + 26 + (i // 10) * 64
        inst_draw.append(lambda lx=lx, ly=ly, lines=lines, i=i: lamp(lx, ly, hw - 4, 44, list(lines), "amber", "CL_%s%d" % ("ABCDEFGHIJ"[i % 10], i // 10), size=6.8))
    for k, label in enumerate(("J22 HUEY ANODES", "J23 HUEY CATHODES")):
        px = rb.port_x("top", k + 1, 4)
        cable((rb.x + rb.w * (0.35 + 0.3 * k), huey_y + 160), (px, rb.y), RADIO_C)
        inst_draw.append(lambda px=px, label=label, k=k: connector(rb, "top", px, label, pins=8 if k == 0 else 4))

    LY = 782
    # ------------------------------------------------ lower region: dual OLEDs
    ox = db.x + 6
    panel_frame(ox, LY, db.w - 12, 186, "DISPLAYS  (4 OLEDs via a TCA9548A multiplexer)", "I2C D20 / D21", DUAL_C, bottom=True)
    olds = [("J55 BARO-OLED", ["BARO", "2992"]), ("J17 ALTIMETER-OLED", ["ALT", "03200"]), ("J18 CLOCK-OLED", ["ZULU", "14:30"]), ("J20 TEMPERATURE-OLED", ["TEMP", "21C"])]
    pos = []
    ow = (db.w - 12 - 40) / 2
    for i, (label, lines) in enumerate(olds):
        pos.append((ox + 14 + (i % 2) * (ow + 12), LY + 44 + (i // 2) * 66, label, lines))
    srt = sorted(pos, key=lambda p: (p[0], p[1]))
    for i, (gx, gy, label, lines) in enumerate(srt):
        px = db.port_x("bottom", i, 4)
        cable((gx + ow / 2, gy - 14), (px, db.y + db.h), DUAL_C)
        inst_draw.append(lambda gx=gx, gy=gy, label=label, lines=lines, px=px: (oled(gx, gy - 14, ow, 62, lines, size=14), connector(db, "bottom", px, label, pins=4)))

    # ------------------------------------------------ servo annunciator lamps
    pw = vb.w - 4
    px0 = vb.x + 2
    panel_frame(px0, LY, pw, 5 * 58 + 42, "ANNUNCIATOR LAMPS  (Mega pins A1 - A15)", None, SERVO_C)
    lw, lh = 88, 42
    jx = [vb.port_x("bottom", 1, 4), vb.port_x("bottom", 2, 4)]
    cable((px0 + pw * 0.3, LY), (jx[0], vb.y + vb.h), SERVO_C)
    cable((px0 + pw * 0.7, LY), (jx[1], vb.y + vb.h), SERVO_C)
    inst_draw.append(lambda: (connector(vb, "bottom", jx[0], "J4 LHS ANNUC", pins=16), connector(vb, "bottom", jx[1], "J5 RHS ANNUC", pins=16)))
    for i, (lines, colour, code) in enumerate(SERVO_LAMPS):
        cx_ = px0 + 12 + (i % 3) * (lw + 6)
        cy_ = LY + 28 + (i // 3) * 58
        inst_draw.append(lambda cx_=cx_, cy_=cy_, lines=lines, colour=colour, code=code: lamp(cx_, cy_, lw, lh, list(lines), colour, code, size=8.4))

    # ------------------------------------------------ radio panels below the board
    rw = (rb.w - 12) / 3
    ry = LY
    cn_x, ap_x, au_x = rb.x, rb.x + rw + 6, rb.x + 2 * (rw + 6)
    PH = 300
    panel_frame(cn_x, ry, rw, PH, "COM AND NAV PANEL", None, RADIO_C)
    inst_draw.append(lambda: (oled(cn_x + 8, ry + 26, rw - 16, 66, ["119.500  121.900", "118.200  124.000"], size=10.4),
                              oled(cn_x + 8, ry + 104, rw - 16, 66, ["108.30   110.50", "109.90   113.20"], size=10.4)))
    for i, s in enumerate(("COM and NAV frequency OLEDs", "(8 encoders and 4 swap", "switches are inputs, not drawn)")):
        text(cn_x + rw / 2, ry + 196 + i * 14, s, size=8.6, fill="#666")
    panel_frame(ap_x, ry, rw, PH, "AUTO PILOT PANEL", "8 LEDs", RADIO_C)
    inst_draw.append(lambda: oled(ap_x + 8, ry + 26, rw - 16, 56, ["090            0", "110    80   3200"], size=10.2))
    cw = (rw - 16) / 3
    for i, (lab, colour) in enumerate(AP_LEDS):
        inst_draw.append(lambda i=i, lab=lab, colour=colour: lamp(ap_x + 8 + (i % 3) * cw, ry + 100 + (i // 3) * 58, cw - 5, 40, [lab], colour, None, size=9.4))
    panel_frame(au_x, ry, rw, PH, "AUDIO PANEL", "11 LEDs", RADIO_C)
    inst_draw.append(lambda: oled(au_x + 8, ry + 26, rw - 16, 44, ["10M", "0M"], size=11))
    for i, (lab, colour) in enumerate(AUDIO_LEDS):
        inst_draw.append(lambda i=i, lab=lab, colour=colour: lamp(au_x + 8 + (i % 3) * cw, ry + 84 + (i // 3) * 52, cw - 5, 38, [lab], colour, None, size=9.4))
    for i, (label, x_, pins) in enumerate((("J8 COMM_NAV", cn_x + rw / 2, 34), ("J12 AUTOPILOT", ap_x + rw / 2, 34), ("J3 AUDIO", au_x + rw / 2, 20))):
        px = rb.port_x("bottom", i, 3)
        cable((x_, ry), (px, rb.y + rb.h), RADIO_C)
        inst_draw.append(lambda px=px, label=label, pins=pins: connector(rb, "bottom", px, label, pins=min(pins, 12)))

    # ------------------------------------------------ ADF panel
    ab = boards["adf"]
    aw = ab.w
    panel_frame(ab.x, LY, aw, PH, "ADF PANEL", None, ADF_C)
    inst_draw.append(lambda: oled(ab.x + 8, LY + 28, aw - 16, 64, ["ADF  340 kHz", "FOLLOW OFF"], size=9.4))
    text(ab.x + aw / 2, LY + 114, "LANDING GEAR", size=9.4, fill="#444", weight=700)
    inst_draw.append(lambda: (gear_lamp(ab.x + aw / 2, LY + 148, 19, "N"), gear_lamp(ab.x + aw / 2 - 38, LY + 200, 19, "L"), gear_lamp(ab.x + aw / 2 + 38, LY + 200, 19, "R")))
    pa = ab.port_x("bottom", 0, 1)
    cable((ab.x + aw / 2, LY), (pa, ab.y + ab.h), ADF_C)
    inst_draw.append(lambda: connector(ab, "bottom", pa, "J5 ADF", pins=12))
    section("adf", "ADF PANEL")
    # inputs note in the free top right corner
    text(ab.x + 2, 128, "Switches, knobs,", size=10, fill="#666", anchor="start")
    text(ab.x + 2, 142, "keypads and the", size=10, fill="#666", anchor="start")
    text(ab.x + 2, 156, "USB HID board are", size=10, fill="#666", anchor="start")
    text(ab.x + 2, 170, "inputs: not drawn.", size=10, fill="#666", anchor="start")

    # ------------------------------------------------ how the values arrive (bottom left, under the Stepper board)
    bx, by, bw = sb.x, 710, sb.w
    panel_frame(bx, by, bw, 392, "HOW THE VALUES ARRIVE", None, "#AAA")
    steps = [("SIMULATOR", "MSFS 2020 / 2024, Prepar3D, DCS World", "#E8EEFB"),
             ("BRIDGE PROGRAM  (on the PC)", "FSUIPC, MSFS or DCS-BIOS bridge", "#E8EEFB"),
             ("ETHERNET  172.16.1.0/24", "UDP 13136:  D,IAS:93.4,PITCH:4.5,...", "#EDEDED"),
             ("CONTROLLER BOARD", "Mega 2560 + W5500, calibration tables", "#E3F2E8"),
             ("CABLE TO THE CONNECTOR", "stepper coils, servos, lamps, OLEDs", "#FFF3DC")]
    sy = by + 26
    for i, (head, sub, col) in enumerate(steps):
        add('<rect x="%.1f" y="%.1f" width="%.1f" height="52" rx="6" fill="%s" stroke="#888" stroke-width="0.9"/>' % (bx + 12, sy, bw - 24, col))
        text(bx + bw / 2, sy + 21, head, size=10.5, fill="#222", weight=700)
        text(bx + bw / 2, sy + 38, sub, size=8.6, fill="#555")
        if i < len(steps) - 1:
            add('<path d="M %.1f %.1f v 14" stroke="#555" stroke-width="2"/><polygon points="%.1f,%.1f %.1f,%.1f %.1f,%.1f" fill="#555"/>' % (bx + bw / 2, sy + 52, bx + bw / 2 - 5, sy + 62, bx + bw / 2 + 5, sy + 62, bx + bw / 2, sy + 70))
        sy += 72
    text(bx + bw / 2, sy + 2, "Each board takes real units and does its own calibration.", size=8.4, fill="#777")

    # ------------------------------------------------ key (top right)
    kx0, ky0, kw, kh = 1036, 86, 448, 100
    panel_frame(kx0, ky0, kw, kh + 80, "KEY", None, "#AAA")
    dial(kx0 + 36, ky0 + 56, 24, G(["", ""], "", (0, 10), S270, 5, 1, lab_every(5), 4), "k")
    text(kx0 + 68, ky0 + 50, "Dial gauge (X27 stepper)", size=9.4, anchor="start")
    text(kx0 + 68, ky0 + 63, "token in the coloured pill", size=9.4, anchor="start", fill="#666")
    attitude(kx0 + 226, ky0 + 56, 26)
    text(kx0 + 258, ky0 + 50, "Attitude indicator", size=9.4, anchor="start")
    text(kx0 + 258, ky0 + 63, "(2 servos)", size=9.4, anchor="start", fill="#666")
    lamp(kx0 + 14, ky0 + 96, 62, 32, ["CAUTION", "LAMP"], "amber", None, size=8)
    text(kx0 + 84, ky0 + 108, "Legend lamp on a Mega pin", size=9.4, anchor="start")
    text(kx0 + 84, ky0 + 121, "or a MAX7219 LED", size=9.4, anchor="start", fill="#666")
    oled(kx0 + 226, ky0 + 96, 64, 32, ["ADF 340", "OLED"], size=9)
    text(kx0 + 298, ky0 + 108, "OLED display", size=9.4, anchor="start")
    text(kx0 + 298, ky0 + 121, "(I2C, 2 lines)", size=9.4, anchor="start", fill="#666")
    kx = kx0 + 14
    for colour, name in ((STEPPER_C, "Stepper"), (DUAL_C, "Dual Stepper"), (SERVO_C, "Servo"), (RADIO_C, "Radio"), (ADF_C, "ADF + Upper")):
        add('<path d="M %.1f %.1f h 18" stroke="#1d1d1d" stroke-width="4.6" stroke-linecap="round"/><path d="M %.1f %.1f h 18" stroke="%s" stroke-width="2.6" stroke-linecap="round"/>' % (kx, ky0 + 156, kx, ky0 + 156, colour))
        text(kx + 24, ky0 + 159, name, size=9.2, anchor="start")
        kx += 88
    text(kx0 + kw - 8, ky0 + 172, "Red bar = pin 1.  J = connector, SW = switch.  Lens colours are illustrative.", size=8.2, fill="#777", anchor="end")
    # data path
    dy = 280 - 0
    text(kx0, dy - 12, "", size=1)

    for b in boards.values():
        b.draw()
    draw_cables()
    for f in inst_draw:
        f()
    text(W - 28, H - 18, "Jet Ranger cockpit instrument map - 31 October 2026 - from the sketches, PCB layouts and tables in the Program Summary", size=9, fill="#888", anchor="end")
    add('</svg>')
    return "\n".join(out)


def render(svg_path):
    """A3 PDF and a 200 dpi PNG with headless Edge."""
    html = BASE + ".html"
    uri = "file:///" + svg_path.replace("\\", "/")
    open(html, "w", encoding="utf-8").write(
        '<!doctype html><html><head><meta charset="utf-8"><style>@page{size:420mm 297mm;margin:0}html,body{margin:0;padding:0}img{display:block;width:420mm;height:297mm}</style></head>'
        '<body><img src="%s"></body></html>' % uri)
    pdf, png = BASE + ".pdf", BASE + ".png"
    for f in (pdf, png):
        if os.path.exists(f):
            os.remove(f)
    furi = "file:///" + html.replace("\\", "/")
    subprocess.run([EDGE, "--headless=new", "--disable-gpu", "--no-pdf-header-footer", "--print-to-pdf=" + pdf, furi], check=True, capture_output=True, timeout=180)
    html2 = BASE + "_png.html"
    open(html2, "w", encoding="utf-8").write('<!doctype html><html><body style="margin:0;background:#fff"><img src="%s" style="display:block;width:3307px;height:2339px"></body></html>' % uri)
    subprocess.run([EDGE, "--headless=new", "--disable-gpu", "--hide-scrollbars", "--force-device-scale-factor=1", "--window-size=3307,2339",
                    "--screenshot=" + png, "file:///" + html2.replace("\\", "/")], check=True, capture_output=True, timeout=180)
    for f in (pdf, png):
        last, waited = -1, 0
        while waited < 40:
            size = os.path.getsize(f) if os.path.exists(f) else -1
            if size > 0 and size == last:
                break
            last = size
            time.sleep(1)
            waited += 1
    os.remove(html)
    os.remove(html2)



# ----------------------------------------------------------------------------------------------------------------------------- pictures and text for the documents
# Crops of the A3 drawing that stay readable at page width: (file name, caption, x, y, width, height) in drawing units.
CROPS = [
    ("full", "The whole map (the A3 original is Jet Ranger Instrument Map A3.pdf; zoom in the PDF to read it)", 0, 0, W, H, 2.0),
    ("stepper", "Stepper gauges and the Stepper controller", 10, 84, 370, 590, 2.8),
    ("dual", "Dual Stepper gauges and the Dual Stepper controller", 372, 84, 354, 590, 2.8),
    ("servo", "Attitude indicator and the Servo controller", 726, 84, 300, 590, 2.8),
    ("radio_top", "Huey Warning Panel, key, and the top of the Radio and ADF boards", 1026, 84, 646, 590, 2.6),
    ("lower_left", "Dual Stepper displays and the Servo annunciator lamps", 372, 648, 654, 500, 2.6),
    ("lower_right", "Radio panels (COM and NAV, Auto Pilot, Audio) and the ADF panel", 1026, 648, 646, 472, 2.6),
]


def shot_png(svg_text, w_px, h_px, path):
    tmp_svg, tmp_html = path + ".tmp.svg", path + ".tmp.html"
    open(tmp_svg, "w", encoding="utf-8").write(svg_text)
    uri = "file:///" + tmp_svg.replace("\\", "/")
    open(tmp_html, "w", encoding="utf-8").write('<!doctype html><html><body style="margin:0;background:#fff"><img src="%s" style="display:block;width:%dpx;height:%dpx"></body></html>' % (uri, w_px, h_px))
    if os.path.exists(path):
        os.remove(path)
    subprocess.run([EDGE, "--headless=new", "--disable-gpu", "--hide-scrollbars", "--force-device-scale-factor=1", "--window-size=%d,%d" % (w_px, h_px),
                    "--screenshot=" + path, "file:///" + tmp_html.replace("\\", "/")], check=True, capture_output=True, timeout=180)
    last, waited = -1, 0
    while waited < 40:
        size = os.path.getsize(path) if os.path.exists(path) else -1
        if size > 0 and size == last:
            break
        last = size
        time.sleep(1)
        waited += 1
    os.remove(tmp_svg)
    os.remove(tmp_html)


def write_crops(svg):
    outdir = os.path.join(HERE, "diagrams")
    head = '<svg xmlns="http://www.w3.org/2000/svg" width="420mm" height="297mm" viewBox="0 0 %d %d">' % (W, H)
    assert head in svg
    for name, caption, x, y, w, h, scale in CROPS:
        wp, hp = int(w * scale), int(h * scale)
        cropped = svg.replace(head, '<svg xmlns="http://www.w3.org/2000/svg" width="%d" height="%d" viewBox="%d %d %d %d">' % (wp, hp, x, y, w, h))
        shot_png(cropped, wp, hp, os.path.join(outdir, "instrument_map_%s.png" % name))
        print("wrote diagrams/instrument_map_%s.png" % name)


def md_tables():
    rows = ["| Instrument | Token | Connector on the board | Range |", "|---|---|---|---|"]
    stepper = [("Airspeed", "IAS", "J26 AIRSPEED", "0 to 140 knots"), ("Vertical speed", "VSI", "J5 VVI", "-1750 to +1750 ft/min"),
               ("Radar altitude", "AGL", "J23 RADAR_ALT", "0 to 2500 ft"), ("Torque", "TQ", "J25 ENG_TORQUE", "0 to 120 percent"),
               ("Gas producer (N1)", "N1", "J24 GAS_PRODUCER", "0 to 103 percent"), ("Turbine out temperature", "ITT", "J27 EGT", "100 to 900 C"),
               ("Fuel quantity", "FUEL", "J4 FUEL_LEVEL", "0 to 75 US gallons")]
    for a, b, c, d in stepper:
        rows.append("| %s | `%s` | Stepper .105: %s | %s |" % (a, b, c, d))
    duals = [("Engine oil pressure and temperature (dual instrument)", "`OILP`, `OILT`", "J8 ENGINE", "0 to 150 psi, 0 to 150 C"),
             ("Transmission oil pressure and temperature (dual instrument)", "`XMSNP`, `XMSNT`", "J6 XMSN", "0 to 150 psi, 0 to 150 C"),
             ("Rotor speed and engine speed N2 (dual instrument)", "`RPMR`, `RPME`", "J7 ROTOR ENG SPEED", "0 to 110 percent"),
             ("Electrical load and fuel load (dual instrument)", "`ELECTRICALLOAD`, `FUELLOAD`", "J10 ELEC FUEL LOAD", "0 to 100 percent, 0 to 30 psi"),
             ("Altimeter (and its zero sensor)", "`ALT`", "J33 ALT, J9 ALTIMETER_ZERO", "one turn per 1000 ft")]
    for a, b, c, d in duals:
        rows.append("| %s | %s | Dual Stepper .106: %s | %s |" % (a, b, c, d))
    rows.append("| Attitude indicator pitch | `PITCH` | Servo .102: J42 PITCH | -30 to +30 degrees |")
    rows.append("| Attitude indicator bank | `BANK` | Servo .102: J43 ROLL | -90 to +90 degrees |")
    gauges = "\n".join(rows)
    lamps = ["| Lamps and displays | Tokens | Connector on the board |", "|---|---|---|",
             "| 15 annunciator lamps (engine out, rotor low, transmission oil pressure and temperature, battery hot and temperature, engine chip, tail rotor chip, transmission chip, baggage door, fuel pump, aft fuel filter, generator fail, low fuel, SC fail) | `EOUT`, `RLOW`, `TOPW`, `TOTW`, `BHOT`, `BTMP`, `EC`, `TRC`, `TC`, `BD`, `FPMP`, `FFLTR`, `GENF`, `LOWF`, `SCF` on Mega pins A1 to A15 | Servo .102: J4 LHS_ANNUC and J5 RHS_ANNUC (how the lamps divide between the two is not recorded) |",
             "| COM and NAV frequency OLEDs | `C1A`, `C1S`, `C2A`, `C2S` | Radio .101: J8 COMM_NAV |",
             "| Auto Pilot display (OLED) and 8 LEDs | `APHDG`, `APCRS`, `APIAS`, `APVS`, `APALT`; `LED_CRS`, `LED_HDG`, `LED_IAS`, `LED_ALT`, `LED_APPR`, `LED_FD`, `LED_BC`, `LED_APENG` | Radio .101: J12 AUTOPILOT |",
             "| DME display (OLED) and 11 audio panel LEDs | `LED_COM1`, `LED_BOTH`, `LED_NAV2`, `LED_DME`, `LED_OUTER`, `LED_INNER`, `LED_COM2`, `LED_NAV1`, `LED_MKR`, `LED_ADF`, `LED_MID` | Radio .101: J3 AUDIO |",
             "| 20 Huey Warning Panel lamps | `LED_H_*` (MAX7219 digits 4 to 7) | Radio .101: J22 HUEY_ANODES and J23 HUEY_CATHODES (rev 1.4 layout; the panel is hand wired today) |",
             "| 4 Dual Stepper OLEDs (barometer, altimeter digits, clock, temperature) | `ALT`, `ZULU`, `TEMP` | Dual Stepper .106: J55 BARO-OLED, J17 ALTIMETER-OLED, J18 CLOCK-OLED, J20 TEMPERATURE-OLED |",
             "| ADF display (OLED) and 3 landing gear lamps | `LDGN`, `LDGL`, `LDGR` (Mega pins D2, D3, D4) | ADF and Upper .107: J5 ADF |"]
    return gauges, "\n".join(lamps)


def write_markdown(heading_master):
    gauges, lamps = md_tables()
    intro = ("This appendix draws every gauge, lamp and display of the cockpit as the real thing (dial gauges with needles, the attitude indicator, annunciator and legend lamps, "
             "gear lights, OLED displays) and joins each one by a cable to the connector on the controller board that drives it. The connector references and names are "
             "the ones printed on the PCBs (see the installation views in Appendix F). Needles and lamps are drawn at illustrative readings and the lens colours are illustrative. "
             "Switches, knobs, keypads and the USB HID board are inputs and are not drawn. The full-size drawing is on one A3 landscape page, `Jet Ranger Instrument Map A3.pdf` "
             "(also an SVG and a PNG) in the documentation folder; it is made by `DocBuild/make_instrument_map.py`.")
    inference = ("Which dual instrument uses which socket is taken from the names on the PCBs (J8 ENGINE, J6 XMSN, J7 ROTOR ENG SPEED, J10 ELEC FUEL LOAD); the Stepper's turbine "
                 "out temperature gauge is shown on J27 EGT; and how the 15 annunciator lamps divide between J4 and J5 is not recorded, so the drawing joins the whole lamp panel to both.")
    body = [intro, "",
            "![%s](diagrams/instrument_map_full.png)" % CROPS[0][1], "", "<!-- pagebreak -->", ""]
    heading2 = "##"
    for name, caption, *_ in CROPS[1:]:
        body += ["%s %s" % (heading2, caption[0].upper() + caption[1:]), "", "![%s](diagrams/instrument_map_%s.png)" % (caption, name), "", "<!-- pagebreak -->", ""]
    body += ["%s Gauges and the connectors they use" % heading2, "", gauges, "", "%s Lamps and displays and the connectors they use" % heading2, "", lamps, "", inference, ""]
    master = ["# Appendix G: Instrument map {-}", ""] + body
    guide = ["# Instrument map", ""] + body
    src = os.path.join(HERE, "src")
    open(os.path.join(src, "master", "16_instrument_map.md"), "w", encoding="utf-8", newline="").write("\n".join(master).rstrip() + "\n")
    open(os.path.join(src, "guide", "08_instrument_map.md"), "w", encoding="utf-8", newline="").write("\n".join(guide).rstrip() + "\n")
    print("wrote src/master/16_instrument_map.md and src/guide/08_instrument_map.md")


if __name__ == "__main__":
    svg = build()
    svg_path = BASE + ".svg"
    open(svg_path, "w", encoding="utf-8").write(svg)
    print("wrote", svg_path, len(svg), "bytes")
    if "--no-render" not in sys.argv:
        render(svg_path)
        print("wrote", BASE + ".pdf", "and", BASE + ".png")
    if "--no-doc" not in sys.argv and "--no-render" not in sys.argv:
        write_crops(svg)
        write_markdown(True)
