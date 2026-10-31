"""Exports the schematic (root sheet) and the board layout of each active PCB project to PNG, for the documents.

Uses kicad-cli (SVG export) and headless Edge to rasterise. Output: DocBuild/pcb/<slug>_sch.png and <slug>_pcb.png.
"""
import os, re, subprocess, sys, glob, shutil, tempfile

K = r"C:\Program Files\KiCad\9.0\bin\kicad-cli.exe"
EDGE = r"C:\Program Files (x86)\Microsoft\Edge\Application\msedge.exe"
PCBS = r"C:\Users\admin\bne-arduino-flight-simulator-interfaces\Jet Ranger\PCBs"
OUT = r"C:\Users\admin\bne-arduino-flight-simulator-interfaces\Jet Ranger\Jet Ranger Documentation\DocBuild\pcb"
TMP = os.path.join(tempfile.gettempdir(), "pcb_export")
os.makedirs(OUT, exist_ok=True)
os.makedirs(TMP, exist_ok=True)

# (folder, root name, slug)
BOARDS = [
    ("Jet Ranger Radio Auto Pilot", "Jet Ranger Radio Auto Pilot", "radio_auto_pilot"),
    ("Jet Ranger COM and NAV", "Jet Ranger COM and NAV", "com_and_nav"),
    ("Jet Ranger Front Panel Servo", "Jet Ranger Front Panel Servo", "front_panel_servo"),
    ("Jet Ranger 13 Stepper", "Jet Ranger 13 Stepper", "stepper_13"),
    ("Jet Ranger OLED Dual Stepper", "Jet Ranger OLED Dual Stepper", "oled_dual_stepper"),
    ("Jet Ranger X27 Small Stepper", "Jet Ranger X27 Small Stepper", "x27_small_stepper"),
    ("JET RANGER UPPER ADF CONTROLLER", "JET RANGER UPPER ADF CONTROLLER", "upper_adf_controller"),
    ("Jet Ranger Audio Panel", "Jet Ranger Audio Panel", "audio_panel"),
    ("Jet Ranger Auto Pilot Panel", "Jet Ranger Auto Pilot Panel", "auto_pilot_panel"),
    ("Jet Ranger ADF Panel", "Jet Ranger ADF Panel", "adf_panel"),
    ("Annunciator", "Annunciator", "annunciator"),
    ("Jet Ranger Altimeter", "Jet Ranger Altimeter", "altimeter"),
    ("Large Guage", "Simmeters Large Guage Replacement", "large_gauge"),
    ("Simkits Large Dual Gauge", "Simkits Large Dual Gauge", "simkits_large_dual"),
    ("Simkits Large Guage", "Simkits Large Guage", "simkits_large"),
    ("Simkits Small Dual Gauge", "Simkits Small Dual Gauge", "simkits_small_dual"),
    ("Simkits Small Guage", "Simkits Small Guage", "simkits_small"),
]


def svg_size(path):
    head = open(path, encoding="utf-8", errors="replace").read(3000)
    m = re.search(r'viewBox="([\d.\-]+) ([\d.\-]+) ([\d.]+) ([\d.]+)"', head)
    if m:
        return float(m.group(3)), float(m.group(4))
    w = re.search(r'width="([\d.]+)', head)
    h = re.search(r'height="([\d.]+)', head)
    return float(w.group(1)), float(h.group(1))


def shot(svg, png, background, width=2000):
    w, h = svg_size(svg)
    height = int(round(width * h / w))
    html = os.path.join(TMP, os.path.basename(png) + ".html")
    uri = "file:///" + svg.replace("\\", "/")
    open(html, "w", encoding="utf-8").write(
        '<!doctype html><html><body style="margin:0;background:%s"><img src="%s" style="width:%dpx;height:%dpx;display:block"></body></html>'
        % (background, uri, width, height))
    cmd = [EDGE, "--headless=new", "--disable-gpu", "--hide-scrollbars", "--force-device-scale-factor=1",
           "--default-background-color=FFFFFFFF" if background == "#ffffff" else "--default-background-color=FF101214",
           "--window-size=%d,%d" % (width, height), "--screenshot=" + png, "file:///" + html.replace("\\", "/")]
    subprocess.run(cmd, check=True, capture_output=True, timeout=120)


only = sys.argv[1:]
for folder, root, slug in BOARDS:
    if only and slug not in only:
        continue
    d = os.path.join(PCBS, folder)
    sch = os.path.join(d, root + ".kicad_sch")
    pcb = os.path.join(d, root + ".kicad_pcb")
    sdir = os.path.join(TMP, slug + "_sch")
    shutil.rmtree(sdir, ignore_errors=True)
    os.makedirs(sdir)
    subprocess.run([K, "sch", "export", "svg", "-o", sdir, sch], check=True, capture_output=True, timeout=180)
    root_svg = os.path.join(sdir, root + ".svg")
    if not os.path.exists(root_svg):
        root_svg = sorted(glob.glob(os.path.join(sdir, "*.svg")), key=len)[0]
    shot(root_svg, os.path.join(TMP, slug + "_sch.png"), "#ffffff", 2600)
    pcb_svg = os.path.join(TMP, slug + "_pcb.svg")
    subprocess.run([K, "pcb", "export", "svg", "--mode-single", "-l", "F.Cu,B.Cu,F.Silkscreen,B.Silkscreen,Edge.Cuts",
                    "--page-size-mode", "2", "-o", pcb_svg, pcb], check=True, capture_output=True, timeout=180)
    shot(pcb_svg, os.path.join(TMP, slug + "_pcb.png"), "#101214", 2000)
    print("done", slug)
