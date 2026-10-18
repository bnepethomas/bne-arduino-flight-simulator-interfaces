"""PC software diagrams: bridges, connection states, aircraft profiles, torque curves, Health Monitor, SwingTester."""
import math

from dgcommon import diagram, Svg, INK, INK2, LINE, text_width, BLUE, GREEN, AMBER, RED


@diagram
def fsuipc_bridge():
    s = Svg(880, 664)
    s.card(16, 20, 190, 84, "Prepar3D / FSX", ["with FSUIPC and the", "DodoSim 206 aircraft"], "white", center=True)
    s.card(240, 20, 190, 84, "FSUIPCConnection", ["Offset<T> values,", "Process() every tick"], "gray", center=True)
    s.card(464, 20, 190, 84, "timerMain", ["every 100 ms", "reads all offsets"], "blue", center=True)
    s.card(688, 20, 176, 84, "sFrontPanel", ["real-unit values,", "annunciator bits"], "white", center=True)
    for a, b in ((206, 240), (430, 464), (654, 688)):
        s.arrow([(a, 62), (b, 62)], None)
    s.arrow([(776, 104), (776, 150), (560, 150), (560, 176)], None)
    s.card(16, 176, 380, 172, "Offsets read", [
        "0x02BC airspeed    0x02C8 vertical speed",
        "0x3324 altitude    0x31E4 radar altitude",
        "0x2F70 / 0x2F78 pitch / bank",
        "0x2840 / 0x2850 main / avionics bus volts",
        "0x2F28 annunciator bits",
        "0x0238 / 0x0239 Zulu time, 0x0E8C air temp",
        "radio stack: COM, NAV standby frequencies"], "white", title_size=14, line_size=12, mono_lines=False)
    s.card(420, 176, 444, 172, "Payload builders", [
        "Servo      D,PITCH,BANK + 15 lamp codes",
        "Stepper    D,IAS,VSI,AGL,FUEL,TQ,ITT,N1",
        "Dual       D,PWR,ALT,RPMR,RPME,ZULU,OILT,OILP,",
        "                  XMSNT,XMSNP,FUELLOAD,ELECTRICALLOAD,TEMP",
        "Radio      D,C1A,C1S,C2A,C2S,MAINBUS,NAVCOM1"], "blue", title_size=14, line_size=12)
    s.arrow([(640, 348), (640, 380)], "UDP 13136", color=BLUE, label_dx=60, label_dy=4)
    s.card(420, 380, 444, 64, "UDP clients", ["172.16.1.101 / .102 / .105 / .106"], "white", title_size=14, line_size=12.5)
    s.card(16, 380, 380, 100, "timerConnection  (1 s)", ["tries FSUIPCConnection.Open() until", "the simulator is found, then starts", "timerMain and stops itself"], "gray", title_size=14, line_size=12.5)
    s.text(16, 524, "Radio, battery and alternator commands come back from the Radio board:", size=14, weight=700)
    s.card(16, 540, 190, 100, "UDP 27001", ["StartListener()", "background task"], "white", center=True)
    s.card(240, 540, 250, 100, "UpdateRadios(command)", ["swaps, labelled standby,", "avionics, battery, alternator"], "blue", center=True)
    s.card(524, 540, 340, 100, "FsControl events / offsets", ["COM_STBY_RADIO_SET, NAV1_STBY_SET,", "..., avionics master 0x2E80"], "white", center=True)
    s.arrow([(206, 590), (240, 590)], None, color=GREEN)
    s.arrow([(490, 590), (524, 590)], None)
    return s


@diagram
def msfs_bridge():
    s = Svg(880, 680)
    s.card(16, 20, 190, 84, "MSFS 2020 / 2024", ["any aircraft"], "white", center=True)
    s.card(240, 20, 190, 84, "SimConnect", ["data request every", "2nd sim frame"], "gray", center=True)
    s.card(464, 20, 190, 84, "PanelData", ["newest sample is", "kept (event driven)"], "white", center=True)
    s.card(688, 20, 176, 84, "timerMain", ["every 100 ms", "samples it"], "blue", center=True)
    for a, b in ((206, 240), (430, 464), (654, 688)):
        s.arrow([(a, 62), (b, 62)], None)
    s.arrow([(776, 104), (776, 140), (440, 140), (440, 170)], None)
    s.card(16, 170, 848, 94, "ToSnapshot(): one SimSnapshot in real units", [
        "standard SimVars, with aircraft profiles on top: FlyInside B206 (L:vars and estimates), CowaSim Jet Ranger",
        "(smoothed torque, EGT based turbine temperature, load scale), see the aircraft profile figure"], "blue", title_size=14, line_size=12.5)
    s.arrow([(440, 264), (440, 300)], None)
    s.card(16, 300, 848, 78, "Payloads.cs: pure packet builders (no SimConnect or window code)", [
        "Servo, Stepper, Dual Stepper, Radio packets; lamp rules; power rules (20 V, 24 V)"], "white", title_size=14, line_size=12.5)
    s.arrow([(440, 378), (440, 410)], "UDP 13136, about 10 per second", color=BLUE, label_dx=118, label_dy=2)
    s.card(16, 410, 848, 54, "UDP clients", ["Radio .101   Servo .102   Stepper .105   Dual Stepper .106"], "white", title_size=14, line_size=12.5)
    s.card(16, 490, 280, 94, "timerConnection  (1 s)", ["new SimConnect(...) until MSFS", "is found; on quit or a lost", "connection it starts again"], "gray", title_size=14, line_size=12)
    s.card(320, 490, 270, 94, "DefWndProc", ["WM_USER_SIMCONNECT ->", "ReceiveMessage(); data callback", "only stores the sample"], "white", title_size=14, line_size=12)
    s.card(614, 490, 250, 94, "UDP 27001 listener", ["UpdateRadios(): swaps, standby,", "avionics, battery, alternator", "-> TransmitClientEvent"], "white", title_size=14, line_size=12)
    s.text(16, 614, "No new data for 3 s (paused or loading): the bridge stops sending and the boards' 30 s watchdogs zero the gauges.", size=12.5, fill=INK2)
    s.text(16, 636, "Run only one bridge at a time: FSUIPCWinformsAutoCS and MSFSWinformsAutoCS both listen on UDP 27001.", size=12.5, fill=INK2)
    return s


@diagram
def msfs_states():
    s = Svg(880, 400)
    s.card(30, 30, 240, 90, "Disconnected", ["red status bar", "keeps looking every 1 s"], "red", center=True)
    s.card(320, 30, 240, 90, "Connected, waiting", ["orange status bar", "no aircraft data yet"], "amber", center=True)
    s.card(610, 30, 240, 90, "Connected: <aircraft>", ["green status bar", "readout and packets running"], "green", center=True)
    s.arrow([(270, 75), (320, 75)], None, color=BLUE)
    s.arrow([(560, 75), (610, 75)], None, color=BLUE)
    s.text(295, 20, "MSFS found", size=12, fill=INK2, anchor="middle")
    s.text(585, 20, "first data", size=12, fill=INK2, anchor="middle")
    s.card(610, 230, 240, 90, "No data (paused)", ["orange; stops sending", "gauges zero after 30 s"], "amber", center=True)
    s.arrow([(690, 120), (690, 230)], "no data for 3 s", label_anchor="end", label_dx=-8, label_dy=0)
    s.arrow([(770, 230), (770, 120)], "data returns", label_anchor="start", label_dx=8, label_dy=0)
    s.card(30, 230, 240, 90, "MSFS quits or the link drops", ["from any state"], "gray", title_size=13.5, center=True)
    s.arrow([(150, 230), (150, 120)], "start again", color=RED, dash="5 4", label_anchor="start", label_dx=8, label_dy=0)
    s.text(16, 356, "A new aircraft is picked up while connected: the name in the status bar changes and the estimate models restart.", size=12.5, fill=INK2)
    s.text(16, 378, "SimConnect errors are caught on every path, so a closed simulator never stops the bridge. FSUIPCWinformsAutoCS behaves the same way with FSUIPC.", size=12.5, fill=INK2)
    return s


@diagram
def aircraft_profiles():
    s = Svg(880, 700)
    cols = [(16, 230, "Value"), (254, 190, "Standard SimVars"), (452, 200, "FlyInside B206"), (660, 204, "CowaSim Jet Ranger")]
    for x, w, t in cols:
        s.card(x, 14, w, 40, t, [], "dark" if t != "Value" else "gray", title_size=14, center=True)
    rows = [
        ("Rotor, engine speed", "ROTOR RPM PCT,\nGENERAL ENG PCT MAX RPM", "L:Aircraft.Rotor.RPM.Percent,\nL:...Turbine.N2.Smooth", "standard"),
        ("Gas producer (N1)", "TURB ENG CORRECTED N1", "0.92 x turbine N2", "standard"),
        ("Torque (TQ)", "ENG TORQUE PERCENT", "collective lever curve,\nup to 120 %", "0.42 x SimVar,\nsmoothed 2.5 s"),
        ("Turbine out temp (ITT)", "TURB ENG ITT", "6 x N2 + 1.05 x\n(torque - 21)", "EGT - 200 C"),
        ("Fuel load (psi)", "ENG FUEL PRESSURE", "10 x SimVar", "standard"),
        ("Electrical load (%)", "|total amps| / 300 A", "|total amps| / 300 A", "|total amps| / 139 A"),
        ("Transmission pressure", "SimVar (usually 0)", "35 psi x rotor fraction", "48 psi x rotor fraction"),
        ("Transmission temp", "SimVar (usually 0)", "slow warm-up from air temp", "0.92 x engine oil temp"),
        ("Recognised by", "otherwise", "turbine L:var, or title\ncontains FlyInside", "title contains 206B3"),
    ]
    y = 64
    for r in rows:
        h = 56
        for (x, w, _), val in zip(cols, r):
            theme = "gray" if x == 16 else ("green" if (val == "standard") else "white")
            lines = val.split("\n")
            s.card(x, y, w, h, lines[0], lines[1:], theme, title_size=12.5, line_size=12) if x != 16 else s.card(x, y, w, h, lines[0], [], theme, title_size=13, line_size=12)
        y += h + 6
    s.text(16, 690 - 6, "Green = the standard SimVar is used unchanged. Estimates come from a few dial readings and may be a few percent off.", size=12.5, fill=INK2)
    return s


def _axes(s, x, y, w, h, xmax, ymax, xlabel, ylabel, xticks, yticks):
    s.rect(x, y, w, h, "#FFFFFF", "#C9C9C4", rx=2, sw=1)
    for v in xticks:
        px = x + w * v / xmax
        s.line(px, y + h, px, y + h + 5, color="#555555", sw=1)
        s.line(px, y, px, y + h, color="#ECECE8", sw=1)
        s.text(px, y + h + 20, str(v), size=12, fill=INK2, anchor="middle")
    for v in yticks:
        py = y + h - h * v / ymax
        s.line(x - 5, py, x, py, color="#555555", sw=1)
        s.line(x, py, x + w, py, color="#ECECE8", sw=1)
        s.text(x - 9, py + 4, str(v), size=12, fill=INK2, anchor="end")
    s.text(x + w / 2, y + h + 42, xlabel, size=12.5, anchor="middle")
    s.text(x - 36, y - 10, ylabel, size=12.5, fill=INK2)


@diagram
def torque_curves():
    s = Svg(880, 440)
    # left chart: FlyInside lever -> torque
    x, y, w, h = 70, 56, 330, 280
    s.text(16, 28, "FlyInside B206: torque from the collective lever", size=14.5, weight=700)
    _axes(s, x, y, w, h, 100, 130, "collective lever position (%)", "torque sent (%)", [0, 20, 40, 60, 80, 100], [0, 20, 40, 60, 80, 100, 120])
    pts = [(0, 21), (73.6, 74), (100, 120)]
    path = " ".join(("M" if i == 0 else "L") + f" {x + w * px / 100:.1f} {y + h - h * py / 130:.1f}" for i, (px, py) in enumerate(pts))
    s.path(path, stroke=BLUE, sw=2.6)
    for px, py in pts:
        s.circle(x + w * px / 100, y + h - h * py / 130, 5, fill="#FFFFFF", stroke=BLUE, sw=2)
    s.text(x + w * 0.74 + 8, y + h - h * 74 / 130 + 22, "73.6 / 74 %", size=12, fill=INK2)
    s.text(x + 12, y + h - h * 21 / 130 + 24, "idle 21 %", size=12, fill=INK2)
    s.text(x + w - 8, y + h - h * 120 / 130 - 10, "100 / 120 % (cap)", size=12, fill=INK2, anchor="end")
    # right chart: CowaSim smoothing
    x2, y2, w2, h2 = 500, 56, 340, 280
    s.text(470, 28, "CowaSim: dial follows a smoothed 0.42 x SimVar", size=14.5, weight=700)
    _axes(s, x2, y2, w2, h2, 20, 130, "time (seconds)", "torque (%)", [0, 5, 10, 15, 20], [0, 20, 40, 60, 80, 100, 120])
    raw, dial = [], []
    d = 0.42 * 83
    for i in range(0, 201):
        t = i / 10.0
        r = 83 if (t < 3 or t > 8) else 260
        target = 0.42 * r
        d += (target - d) * (1 - math.exp(-0.1 / 2.5))
        raw.append((t, min(target, 130)))
        dial.append((t, d))
    pr = " ".join(("M" if i == 0 else "L") + f" {x2 + w2 * t / 20:.1f} {y2 + h2 - h2 * v / 130:.1f}" for i, (t, v) in enumerate(raw))
    pd = " ".join(("M" if i == 0 else "L") + f" {x2 + w2 * t / 20:.1f} {y2 + h2 - h2 * v / 130:.1f}" for i, (t, v) in enumerate(dial))
    s.path(pr, stroke="#8A8A85", sw=1.8, dash="6 4")
    s.path(pd, stroke=GREEN, sw=2.6)
    s.text(x2 + 10, y2 + 24, "dashed: 0.42 x SimVar (SimVar jumps 83 -> 260 -> 83)", size=12, fill=INK2)
    s.text(x2 + 10, y2 + 42, "solid: what the bridge sends (smoothed, 2.5 s)", size=12, fill=GREEN)
    s.text(16, 410, "Both curves were fitted to dial readings from screenshots; check them against the cockpit at other power settings.", size=12.5, fill=INK2)
    s.text(16, 430, "FlyInside above lever 73.6 climbs at about 1.74 points per percent; the engine over-torques if full collective is held for 2 s.", size=12.5, fill=INK2)
    return s


@diagram
def health_monitor():
    s = Svg(880, 460)
    boards = [("Radio controller", "COMM_NAV"), ("Servo controller", "SERVO"), ("Upper controller", "UPPER_INPUT"),
              ("USB HID board", "JOYSTICK"), ("Stepper controller", "STEPPER"), ("Dual stepper", "DUAL_STEPPER")]
    y = 24
    for name, tok in boards:
        s.card(16, y, 250, 52, name, [], "white", title_size=13.5)
        s.text(284, y + 20, f'"{tok}"', size=13, mono=True)
        s.arrow([(282, y + 36), (400, y + 36)], None, color=BLUE)
        y += 62
    s.text(290, 14, "every 10 s", size=12, fill=INK2)
    s.card(400, 24, 200, 372, "Health Monitor", ["UDP 13137", "matches the message by", "its prefix (not the", "sender address)"], "blue", title_size=14.5, line_size=12.5, center=True)
    s.arrow([(600, 210), (640, 210)], None)
    s.frame(640, 24, 224, 372, "One light per board", theme="gray")
    s.rect(660, 76, 184, 84, "#E7F4EA", GREEN, rx=8)
    s.text(752, 106, "green", size=15, weight=700, anchor="middle")
    s.text(752, 128, "heard within 15 s", size=12.5, anchor="middle")
    s.rect(660, 176, 184, 84, "#FFF3DC", AMBER, rx=8)
    s.text(752, 206, "orange", size=15, weight=700, anchor="middle")
    s.text(752, 228, "15 s without a message", size=12.5, anchor="middle")
    s.rect(660, 276, 184, 84, "#FDECEA", RED, rx=8)
    s.text(752, 306, "red", size=15, weight=700, anchor="middle")
    s.text(752, 328, "30 s: connection lost", size=12.5, anchor="middle")
    s.text(16, 440, "The monitor only listens; it never sends anything. It does not care which bridge is running.", size=12.5, fill=INK2)
    return s


@diagram
def swing_sequence():
    s = Svg(880, 560)
    s.text(16, 26, "SwingTester: Swing ALL runs the gauges in this order", size=15, weight=700)
    steps = [
        ("1", "Engine oil pressure", "Dual", "6 s"), ("2", "Engine oil temp", "Dual", "6 s"), ("3", "Transmission pressure", "Dual", "6 s"),
        ("4", "Transmission temp", "Dual", "6 s"), ("5", "Fuel available", "Stepper", "11 s"), ("6", "Electrical load", "Dual", "7 s"),
        ("7", "Fuel load", "Dual", "7 s"), ("8", "Torque", "Stepper", "11 s"), ("9", "Turbine out", "Stepper", "11 s"),
        ("10", "Gas producer", "Stepper", "11 s"), ("11", "Pitch", "Servo", "3 s"), ("12", "Roll", "Servo", "3 s"),
        ("13", "Turbine RPM", "Dual", "7 s"), ("14", "Rotor RPM", "Dual", "7 s"), ("15", "VSI", "Stepper", "11 s"),
        ("16", "Airspeed", "Stepper", "11 s"), ("17", "Radar altitude", "Stepper", "11 s"), ("18", "Altimeter", "Dual", "6 s"),
    ]
    col = {"Dual": "purple", "Stepper": "blue", "Servo": "green"}
    cw, ch = 276, 56
    for i, (n, name, board, w) in enumerate(steps):
        c, r = i % 3, i // 3
        x, y = 16 + c * (cw + 10), 50 + r * (ch + 8)
        s.card(x, y, cw, ch, f"{n}  {name}", [f"{board} board  |  wait {w}"], col[board], title_size=13.5, line_size=12.5)
    s.text(16, 470, "Then the 15 warning lamps are switched off one by one. OLEDs ON / OFF light the Dual Stepper's displays for the run.", size=12.5, fill=INK2)
    s.text(16, 492, "The boards do not report when a swing ends, so each row has a wait; a full pass takes about 220 s plus the lamps.", size=12.5, fill=INK2)
    s.text(16, 514, "A keep-alive command every 10 s stops the boards' 30 s no-data timers from firing during the run.", size=12.5, fill=INK2)
    s.pill(16, 530, "Dual stepper", "purple")
    s.pill(130, 530, "Stepper", "blue")
    s.pill(210, 530, "Servo", "green")
    return s


@diagram
def emulators():
    s = Svg(880, 360)
    s.card(16, 30, 230, 92, "Radio controller emulator", ["clickable knobs, swap keys,", "master on / off keys"], "white", title_size=14, line_size=12.5)
    s.card(16, 190, 230, 92, "Upper controller emulator", ["one toggle per wired switch", "(22 switches)"], "white", title_size=14, line_size=12.5)
    s.card(380, 30, 200, 92, "Bridge application", ["UDP 27001 on the PC"], "blue", title_size=14, line_size=12.5, center=True)
    s.card(380, 190, 200, 92, "USB HID board", ["UDP 4210 on 172.16.1.11"], "amber", title_size=14, line_size=12.5, center=True)
    s.arrow([(246, 76), (380, 76)], "commands", color=GREEN, label_dy=-10)
    s.arrow([(246, 236), (380, 236)], "idx,state", color=AMBER, label_dy=-10)
    s.card(614, 30, 250, 252, "Use them when", ["the real board is disconnected", "or being worked on.", "", "Only controls that do something in", "the firmware are shown.", "", "Neither sends debug (27000) or", "heartbeat (13137) traffic."], "gray", title_size=14, line_size=12.5)
    s.text(16, 322, "The Radio emulator still sends a bare frequency for every radio, so only COM1 tunes from it.", size=12.5, fill=INK2)
    s.text(16, 344, "(The real board sends labelled COM2_STANDBY:, NAV1_STANDBY: and NAV2_STANDBY: values, which both bridges understand.)", size=12.5, fill=INK2)
    return s
