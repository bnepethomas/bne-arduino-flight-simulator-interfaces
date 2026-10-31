"""Exports an installation view of each controller PCB: the top silkscreen side only, with just the external connectors labelled.

For each board a cut-down copy of the .kicad_pcb is written to %TEMP%\\install_views and rendered with kicad-cli:
  - every footprint is dropped except the external connectors (the panel, gauge and power connectors; the Arduino Mega pin headers,
    breakout headers, jumpers and other generic "Conn_..." parts are left out), the mounting holes, the Arduino Mega socket and the
    Ethernet module (outlines and pads only, no labels);
  - all tracks, vias, zones and board text are dropped, so the copper layer shows only the pads (pin 1 is the square pad);
  - each external connector gets a label with its reference and name ("J32" over "KEYPAD"), placed beside it where nothing else is,
    with a thin line back to the connector when it had to move away.
Output: DocBuild/pcb/<slug>_install.jpg (needs Edge for the rasterising, like export_pcb_images.py; pcb_to_jpg.ps1-style conversion
is done here with System.Drawing through PowerShell).

    python export_install_views.py [slug ...]
"""
import math, os, re, subprocess, sys, shutil, tempfile, time, uuid

K = r"C:\Program Files\KiCad\9.0\bin\kicad-cli.exe"
EDGE = r"C:\Program Files (x86)\Microsoft\Edge\Application\msedge.exe"
PCBS = r"C:\Users\admin\bne-arduino-flight-simulator-interfaces\Jet Ranger\PCBs"
HERE = os.path.dirname(os.path.abspath(__file__))
OUT = os.path.join(HERE, "pcb")
TMP = os.path.join(tempfile.gettempdir(), "install_views")

# (folder, project name, slug)
BOARDS = [
    ("Jet Ranger Radio Auto Pilot", "Jet Ranger Radio Auto Pilot", "radio_auto_pilot"),
    ("Jet Ranger Front Panel Servo", "Jet Ranger Front Panel Servo", "front_panel_servo"),
    ("Jet Ranger 13 Stepper", "Jet Ranger 13 Stepper", "stepper_13"),
    ("Jet Ranger OLED Dual Stepper", "Jet Ranger OLED Dual Stepper", "oled_dual_stepper"),
    ("JET RANGER UPPER ADF CONTROLLER", "JET RANGER UPPER ADF CONTROLLER", "upper_adf_controller"),
]

# Named connectors that are not external (internal links and patch headers).
NOT_EXTERNAL_VALUES = ("Utility", "PATCH")
NOT_EXTERNAL_PREFIXES = ()
LABEL_SIZE = 1.3


def children(text):
    """(start, end, name) of every top-level child of the root list."""
    out = []
    depth, i, n = 0, 0, len(text)
    start = None
    in_str = False
    while i < n:
        c = text[i]
        if in_str:
            if c == "\\":
                i += 1
            elif c == '"':
                in_str = False
        elif c == '"':
            in_str = True
        elif c == "(":
            depth += 1
            if depth == 2:
                start = i
        elif c == ")":
            if depth == 2:
                m = re.match(r"\(([A-Za-z_0-9]+)", text[start:start + 40])
                out.append((start, i + 1, m.group(1)))
            depth -= 1
        i += 1
    return out


def prop(block, name):
    m = re.search(r'\(property "%s" "((?:[^"\\]|\\.)*)"' % name, block)
    return m.group(1) if m else None


def is_external(block):
    ref = prop(block, "Reference") or ""
    val = prop(block, "Value") or ""
    if not re.match(r"(J|SW)\d+$", ref):
        return False
    if val.startswith("Conn_") or val in NOT_EXTERNAL_VALUES or val.startswith(NOT_EXTERNAL_PREFIXES):
        return False
    return True


def silence(block):
    """Every property text of a footprint (reference, value, ...) goes off the silkscreen, so only the labels added below show."""
    def fix(m):
        return re.sub(r'\(layer "[^"]*"\)', '(layer "F.Fab")', m.group(0))
    out, i = [], 0
    for m in re.finditer(r"\((?:property|fp_text) ", block):
        if m.start() < i:
            continue
        depth, j, in_str = 0, m.start(), False
        while True:
            c = block[j]
            if in_str:
                if c == "\\":
                    j += 1
                elif c == '"':
                    in_str = False
            elif c == '"':
                in_str = True
            elif c == "(":
                depth += 1
            elif c == ")":
                depth -= 1
                if depth == 0:
                    break
            j += 1
        out.append(block[i:m.start()])
        out.append(re.sub(r'\(layer "[^"]*"\)', '(layer "F.Fab")', block[m.start():j + 1]))
        i = j + 1
    out.append(block[i:])
    return "".join(out)


def nums(s):
    return [float(x) for x in re.findall(r"-?\d+\.?\d*", s)]


def footprint_geometry(block):
    """(pads, outline_points): absolute pad rectangles (cx, cy, half-size) and absolute silkscreen / courtyard points."""
    m = re.search(r"\(at ([-\d.]+) ([-\d.]+)(?: ([-\d.]+))?\)", block)
    fx, fy, rot = float(m.group(1)), float(m.group(2)), float(m.group(3) or 0)
    c, s = math.cos(math.radians(rot)), math.sin(math.radians(rot))

    def absolute(px, py):
        return fx + px * c + py * s, fy - px * s + py * c
    pads = []
    for pm in re.finditer(r'\(pad "[^"]*" \w+ \w+\s+\(at ([-\d.]+) ([-\d.]+)(?: [-\d.]+)?\)\s+\(size ([-\d.]+) ([-\d.]+)\)', block):
        x, y = absolute(float(pm.group(1)), float(pm.group(2)))
        pads.append((x, y, max(float(pm.group(3)), float(pm.group(4))) / 2))
    pts = []
    for lm in re.finditer(r'\((?:fp_line|fp_rect)\s+\(start ([-\d.]+) ([-\d.]+)\)\s+\(end ([-\d.]+) ([-\d.]+)\)(?:(?!\(layer).)*\(layer "(F\.SilkS|F\.CrtYd)"\)', block, re.S):
        pts.append(absolute(float(lm.group(1)), float(lm.group(2))))
        pts.append(absolute(float(lm.group(3)), float(lm.group(4))))
    return pads, pts


def bbox(points):
    xs = [p[0] for p in points]
    ys = [p[1] for p in points]
    return min(xs), min(ys), max(xs), max(ys)


def board_box(text):
    pts = []
    for s, e, name in children(text):
        if name.startswith("gr_") and '(layer "Edge.Cuts")' in text[s:e]:
            pts += [(float(a), float(b)) for a, b in re.findall(r"\((?:start|end|center|mid) ([-\d.]+) ([-\d.]+)\)", text[s:e])]
    return bbox(pts) if pts else (0, 0, 160, 160)


def overlaps(a, b, pad=0.0):
    return not (a[2] + pad <= b[0] or b[2] + pad <= a[0] or a[3] + pad <= b[1] or b[3] + pad <= a[1])


def wrap(value, width):
    """Splits a connector name into lines of at most `width` characters (never inside a word of 14 characters or less)."""
    parts = [x for x in re.split(r"[_ ]+", value) if x]
    width = max(width, min(14, max(len(x) for x in parts)))
    tokens = []
    for part in parts:
        while len(part) > width:
            cut = part.rfind("-", 0, width)
            cut = cut + 1 if cut > 2 else width
            tokens.append(part[:cut])
            part = part[cut:]
        if part:
            tokens.append(part)
    lines = []
    for tok in tokens:
        if lines and len(lines[-1]) + 1 + len(tok) <= width:
            lines[-1] += " " + tok
        else:
            lines.append(tok)
    return lines


def full_variants(ref, val, narrow):
    out = []
    for width in ((6, 9, 14, 30) if narrow else (30, 14, 9, 6)):
        lines = [ref] + wrap(val, width)
        if lines not in out:
            out.append(lines)
    return out


SIDES = ("above", "below", "right", "left")


def candidate(side, bb, w, h, gap, shift):
    cx0, cy0 = (bb[0] + bb[2]) / 2, (bb[1] + bb[3]) / 2
    if side == "above":
        return (cx0 + shift - w / 2, bb[1] - gap - h, cx0 + shift + w / 2, bb[1] - gap)
    if side == "below":
        return (cx0 + shift - w / 2, bb[3] + gap, cx0 + shift + w / 2, bb[3] + gap + h)
    if side == "right":
        return (bb[2] + gap, cy0 + shift - h / 2, bb[2] + gap + w, cy0 + shift + h / 2)
    return (bb[0] - gap - w, cy0 + shift - h / 2, bb[0] - gap, cy0 + shift + h / 2)


def size_of(lines):
    return max(len(l) for l in lines) * LABEL_SIZE * 0.95 + 1.0, len(lines) * LABEL_SIZE * 1.5 + 0.6


def search(lines, bb, taken, board, gaps, shifts):
    w, h = size_of(lines)
    for gap in gaps:
        for shift in shifts:
            for side in SIDES:
                r = candidate(side, bb, w, h, gap, shift)
                if r[0] < board[0] + 0.5 or r[1] < board[1] + 0.5 or r[2] > board[2] - 0.5 or r[3] > board[3] - 0.5:
                    continue
                if any(overlaps(r, o, 0.3) for o in taken):
                    continue
                return r, gap, shift
    return None


def place_labels(items, obstacles, board):
    """items: [(ref, value, bbox)]. Returns [(lines, centre, rect, anchor_bbox, moved)].

    Each label is tried right beside its connector first, with the name wrapped onto as many lines as it takes; where the connectors are too
    close together for that (the row of gauge sockets on the Servo and Stepper boards) the label is just the reference (J28), and only if
    even that does not fit is a full label put further away and joined to its connector by a line."""
    placed = []
    taken = list(obstacles)
    near_gaps, near_shifts = (1.0, 2.5), (0, 2, -2, 3, -3)
    far_gaps = (1.0, 2.5, 5.0, 8.0, 12.0)
    far_shifts = (0, 2, -2, 4, -4, 6, -6, 8, -8, 10, -10, 13, -13, 16, -16, 20, -20, 25, -25)
    centres = [((it[2][0] + it[2][2]) / 2, (it[2][1] + it[2][3]) / 2) for it in items]
    def is_dense(bb):
        cx, cy = (bb[0] + bb[2]) / 2, (bb[1] + bb[3]) / 2
        return sum(1 for (x, y) in centres if (x, y) != (cx, cy) and abs(x - cx) < 7.5 and abs(y - cy) < 3.0) > 0

    # a connector with a close neighbour in the same row (the rows of gauge sockets) gets just its reference, directly above or below it, and
    # those are placed first so nothing else takes their spots; the names of those connectors are in the table beside the picture
    for ref, val, bb in sorted(items, key=lambda it: (not is_dense(it[2]), round(it[2][1] / 8), it[2][0])):
        narrow = (bb[2] - bb[0]) < 14 and (bb[3] - bb[1]) < 14
        fulls = full_variants(ref, val, narrow)
        best = None
        dense = is_dense(bb)
        if dense:
            found = search([ref], bb, taken, board, (0.8, 1.6), (0,))
            if found:
                best = ([ref],) + found + (False,)
        for lines in ([] if best else fulls):
            found = search(lines, bb, taken, board, near_gaps, near_shifts)
            if found:
                best = (lines,) + found + (False,)
                break
        if not best:
            found = search([ref], bb, taken, board, near_gaps, near_shifts + (4, -4, 6, -6))
            if found:
                best = ([ref],) + found + (False,)
        if not best:
            for lines in fulls + [[ref]]:
                found = search(lines, bb, taken, board, far_gaps, far_shifts)
                if found:
                    best = (lines,) + found + (True,)
                    break
        if not best:
            w, h = size_of([ref])
            r = candidate("above", bb, w, h, 1.0, 0)
            best = ([ref], r, 1.0, 0, True)
            print("  no free place for", ref, val)
        lines, r, gap, shift, moved = best
        taken.append(r)
        placed.append((lines, ((r[0] + r[2]) / 2, (r[1] + r[3]) / 2), r, bb, moved))
    return placed


def label_items(placed):
    out = []
    for lines, (cx, cy), r, bb, moved in placed:
        text = "\\n".join(lines)
        out.append('\t(gr_text "%s"\n\t\t(at %.3f %.3f 0)\n\t\t(layer "F.SilkS")\n\t\t(uuid "%s")\n\t\t(effects\n\t\t\t(font\n\t\t\t\t(size %s %s)\n\t\t\t\t(thickness 0.2)\n\t\t\t\t(bold yes)\n\t\t\t)\n\t\t)\n\t)\n'
                   % (text, cx, cy, uuid.uuid4(), LABEL_SIZE, LABEL_SIZE))
        if moved:
            ax, ay = (bb[0] + bb[2]) / 2, (bb[1] + bb[3]) / 2
            # leader from the nearest point of the label rectangle to the connector box
            sx = min(max(ax, r[0]), r[2])
            sy = min(max(ay, r[1]), r[3])
            ex = min(max(sx, bb[0]), bb[2])
            ey = min(max(sy, bb[1]), bb[3])
            out.append('\t(gr_line\n\t\t(start %.3f %.3f)\n\t\t(end %.3f %.3f)\n\t\t(stroke\n\t\t\t(width 0.15)\n\t\t\t(type default)\n\t\t)\n\t\t(layer "F.SilkS")\n\t\t(uuid "%s")\n\t)\n'
                       % (sx, sy, ex, ey, uuid.uuid4()))
    return "".join(out)


def cut_down(src, dst):
    text = open(src, encoding="utf-8").read()
    board = board_box(text)
    pieces, pos = [], 0
    items, obstacles = [], []
    details = []
    for s, e, name in children(text):
        pieces.append(text[pos:s])
        pos = e
        block = text[s:e]
        if name == "footprint":
            ref = prop(block, "Reference") or ""
            pads, pts = footprint_geometry(block)
            if is_external(block):
                val = prop(block, "Value")
                box = bbox([(p[0] - p[2], p[1] - p[2]) for p in pads] + [(p[0] + p[2], p[1] + p[2]) for p in pads] + pts)
                items.append((ref, val, box))
                lib = re.search(r'\(footprint "([^"]*)"', block).group(1).split(":")[-1]
                details.append({"ref": ref, "name": val, "pins": len(pads), "lib": lib})
                obstacles.append(box)
                pieces.append(silence(block))
            elif re.match(r"H\d+$", ref):
                pieces.append(silence(block))
                obstacles.append(bbox([(p[0] - p[2], p[1] - p[2]) for p in pads] + [(p[0] + p[2], p[1] + p[2]) for p in pads]))
            elif ref == "U1":
                pieces.append(silence(block))
                box = bbox(pts) if pts else bbox([(p[0], p[1]) for p in pads])
                obstacles.append(box)
                items.append(("ETHERNET", "MODULE", box))
            elif ref == "A1":
                pieces.append(silence(block))
                obstacles += [(p[0] - 1.3, p[1] - 1.3, p[0] + 1.3, p[1] + 1.3) for p in pads]
        elif name.startswith("gr_"):
            if '(layer "Edge.Cuts")' in block:
                pieces.append(block)
        elif name in ("segment", "via", "arc", "zone", "image", "group", "dimension", "target", "generated", "table"):
            pass
        else:
            pieces.append(block)
    pieces.append(text[pos:])
    body = "".join(pieces)
    ethernet = [it for it in items if it[0] == "ETHERNET"]
    placed = place_labels([it for it in items if it[0] != "ETHERNET"], obstacles, board)
    # the Ethernet module label sits inside its own outline
    for ref, val, box in ethernet:
        placed.append((["ETHERNET", "MODULE"], ((box[0] + box[2]) / 2, (box[1] + box[3]) / 2), box, box, False))
    last = body.rstrip().rfind(")")
    body = body[:last] + label_items(placed) + ")\n"
    os.makedirs(os.path.dirname(dst), exist_ok=True)
    open(dst, "w", encoding="utf-8").write(body)
    return details


def svg_size(path):
    head = open(path, encoding="utf-8", errors="replace").read(3000)
    m = re.search(r'viewBox="([\d.\-]+) ([\d.\-]+) ([\d.]+) ([\d.]+)"', head)
    return float(m.group(3)), float(m.group(4))


def shot(svg, png, width=2000):
    w, h = svg_size(svg)
    height = int(round(width * h / w))
    html = png + ".html"
    uri = "file:///" + svg.replace("\\", "/")
    open(html, "w", encoding="utf-8").write(
        '<!doctype html><html><body style="margin:0;background:#ffffff"><img src="%s" style="width:%dpx;height:%dpx;display:block"></body></html>'
        % (uri, width, height))
    cmd = [EDGE, "--headless=new", "--disable-gpu", "--hide-scrollbars", "--force-device-scale-factor=1",
           "--default-background-color=FFFFFFFF", "--window-size=%d,%d" % (width, height), "--screenshot=" + png,
           "file:///" + html.replace("\\", "/")]
    if os.path.exists(png):
        os.remove(png)
    subprocess.run(cmd, check=True, capture_output=True, timeout=120)
    # headless Edge can exit before the screenshot is written: wait for the file to appear and stop growing
    last, waited = -1, 0.0
    while waited < 40:
        size = os.path.getsize(png) if os.path.exists(png) else -1
        if size > 0 and size == last:
            break
        last = size
        time.sleep(1.0)
        waited += 1.0


def to_jpg(png, jpg):
    """Trims the white border and saves a JPEG (trim_to_jpg.ps1, System.Drawing through PowerShell)."""
    r = subprocess.run(["powershell", "-NoProfile", "-ExecutionPolicy", "Bypass", "-File", os.path.join(HERE, "trim_to_jpg.ps1"), "-Png", png, "-Jpg", jpg],
                       capture_output=True, text=True, timeout=180)
    if r.returncode != 0:
        raise RuntimeError(r.stderr[:500])


BOARD_INFO = {
    "radio_auto_pilot": ("Jet Ranger Radio Auto Pilot", "revision 1.4 (31 October 2026, with the Huey Warning Panel connectors J20 to J23)", "Radio controller (172.16.1.101)"),
    "front_panel_servo": ("Jet Ranger Front Panel Servo", "revision 1.4", "Servo controller (172.16.1.102)"),
    "stepper_13": ("Jet Ranger 13 Stepper", "revision 1.6", "Stepper controller (172.16.1.105)"),
    "oled_dual_stepper": ("Jet Ranger OLED Dual Stepper", "revision 1.5", "Dual Stepper controller (172.16.1.106)"),
    "upper_adf_controller": ("Jet Ranger Upper ADF Controller", "revision 1.0", "ADF and Upper controller (172.16.1.107)"),
}

# What each connector is for, where the PCB files and the sketches make it certain (the others show only their name, pin count and type).
NOTES = {
    "radio_auto_pilot": {"J3": "Audio panel", "J12": "Auto Pilot panel", "J8": "COM and NAV panel", "J4": "5 V supply input (DC jack)",
                         "J7": "12 to 20 V supply input (DC jack)",
                         "J20": "Huey Warning Panel switches: GND, brightness / dim (A8), test (A9), reset (A10)",
                         "J21": "Huey Warning Panel backlight: +12 V and the switched backlight",
                         "J22": "Huey Warning Panel LED anodes ANNODE_00 to ANNODE_07",
                         "J23": "Huey Warning Panel LED cathodes CATHODE_04 to CATHODE_07 (MAX7219 digits 4 to 7)"},
    "front_panel_servo": {"J2": "5 V supply input (DC jack)", "J3": "12 to 20 V supply input (DC jack)", "J4": "Left hand annunciator panel",
                          "J5": "Right hand annunciator panel", "SW1": "TOT clear switch (D43), to ground when pressed",
                          "SW2": "Fuel valve switch (D48), to ground when on", "SW3": "Warning mute switch (D47), to ground when pressed",
                          "J42": "Attitude indicator pitch servo", "J43": "Attitude indicator bank (roll) servo"},
    "stepper_13": {"J2": "5 V supply input (DC jack)", "J3": "12 to 20 V supply input (DC jack)"},
    "oled_dual_stepper": {"J2": "5 V supply input (DC jack)", "J3": "12 to 20 V supply input (DC jack)", "J9": "Altimeter zero sensor",
                          "J55": "Barometer OLED", "J17": "Altimeter OLED", "J18": "Clock OLED", "J20": "Temperature OLED"},
    "upper_adf_controller": {"J4": "5 V supply input (DC jack)", "J7": "12 to 20 V supply input (DC jack)", "J5": "ADF panel (knobs, keypad, switches, lamps)",
                             "J30": "Upper switch matrix rows ROW0 to ROW7", "J31": "Upper switch matrix columns COL0 to COL7",
                             "J32": "Upper numeric keypad", "J20": "Hyd boost, anti-ice and caution dim switches",
                             "J16": "Panel brightness pot", "J17": "Warning mute switch (matrix ROW8)", "J18": "TOT test switch (matrix ROW8)",
                             "J33": "Panel backlight"},
}


def kind_of(d):
    lib = d["lib"]
    m = re.search(r"_(\d)x(\d+)_", lib)
    if lib.startswith("BarrelJack"):
        return "DC jack", "3"
    if lib.startswith("IDC-Header") and m:
        return "IDC header", "%d (2 x %d)" % (int(m.group(1)) * int(m.group(2)), int(m.group(2)))
    if lib.startswith("Molex_KK") and m:
        return "Molex KK-254", str(int(m.group(2)))
    if lib.startswith("PinHeader") and m:
        return "Pin header", str(int(m.group(2)))
    return lib, str(d["pins"])


def board_markdown(slug, details, heading="##"):
    title, rev, board = BOARD_INFO[slug]
    out = ["%s %s" % (heading, title), "",
           "%s, for the %s. Connectors with the name left off in the picture (a row of gauge sockets, for example) are listed in the table." % (rev[0].upper() + rev[1:], board),
           "", "![%s, top side with the external connectors labelled](pcb/%s_install.jpg)" % (title, slug), "",
           "| Label | Name | Type | Pins | Notes |", "|---|---|---|---|---|"]

    def order(d):
        m = re.match(r"([A-Z]+)(\d+)", d["ref"])
        return (m.group(1), int(m.group(2)))
    for d in sorted(details, key=order):
        kind, pins = kind_of(d)
        out.append("| %s | %s | %s | %s | %s |" % (d["ref"], d["name"].replace("_", " "), kind, pins, NOTES.get(slug, {}).get(d["ref"], "")))
    out.append("")
    out.append("<!-- pagebreak -->")
    out.append("")
    return "\n".join(out)


INTRO = ("These pictures are an aid for installing the controller boards. Each shows the **top (component) side** of a controller PCB as it is seen "
         "when the board is fitted, with only the board outline, the mounting holes, the pads and silkscreen outlines of the **external connectors**, "
         "the unlabelled round pads in rows and a double column around the edge (the Arduino Mega headers) and the outline of the Ethernet module. "
         "Every external connector is labelled with its reference (J5, SW2) and, where there is room, its name. **Pin 1 of each connector is the square pad**; "
         "on a ribbon-cable (IDC) header the small triangle also marks pin 1. Nothing else is printed, so the pictures are easy to read at the bench. "
         "They are drawn from the KiCad layout files by `export_install_views.py`, so they show the boards as they were on the day the document was built.")


def write_markdown(data):
    master = ["# Appendix F: Installing the controller PCBs {-}", "", INTRO, ""]
    guide = ["# Installing the controller PCBs", "", INTRO, ""]
    for slug, _ in [(b[2], b) for b in BOARDS]:
        master.append(board_markdown(slug, data[slug], "##"))
        guide.append(board_markdown(slug, data[slug], "##"))
    src = os.path.join(HERE, "src")
    open(os.path.join(src, "master", "15_install_pcbs.md"), "w", encoding="utf-8", newline="").write("\n".join(master).rstrip() + "\n")
    open(os.path.join(src, "guide", "07_install_pcbs.md"), "w", encoding="utf-8", newline="").write("\n".join(guide).rstrip() + "\n")
    print("wrote src/master/15_install_pcbs.md and src/guide/07_install_pcbs.md")


if __name__ == "__main__":
    args = sys.argv[1:]
    md_only = "--md" in args
    only = [a for a in args if not a.startswith("--")]
    os.makedirs(TMP, exist_ok=True)
    os.makedirs(OUT, exist_ok=True)
    data = {}
    for folder, name, slug in BOARDS:
        if only and slug not in only and not md_only:
            continue
        if md_only:
            data[slug] = cut_down(os.path.join(PCBS, folder, name + ".kicad_pcb"), os.path.join(TMP, slug, slug + ".kicad_pcb"))
            continue
        src = os.path.join(PCBS, folder, name + ".kicad_pcb")
        d = os.path.join(TMP, slug)
        shutil.rmtree(d, ignore_errors=True)
        dst = os.path.join(d, slug + ".kicad_pcb")
        labelled = cut_down(src, dst)
        data[slug] = labelled
        svg = os.path.join(TMP, slug + "_install.svg")
        subprocess.run([K, "pcb", "export", "svg", "--mode-single", "--black-and-white", "--exclude-drawing-sheet",
                        "-l", "F.Cu,F.Silkscreen,Edge.Cuts", "--page-size-mode", "2", "-o", svg, dst],
                       check=True, capture_output=True, timeout=180)
        png = os.path.join(TMP, slug + "_install.png")
        shot(svg, png)
        to_jpg(png, os.path.join(OUT, slug + "_install.jpg"))
        print("done", slug, len(labelled), "labelled:", ", ".join(d["ref"] + " " + d["name"] for d in labelled))
    if md_only or (not only and len(data) == len(BOARDS)):
        write_markdown(data)
