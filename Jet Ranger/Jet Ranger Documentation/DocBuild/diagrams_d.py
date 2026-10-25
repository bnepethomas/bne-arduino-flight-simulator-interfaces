"""Guide and tool diagrams: COM ports, PCB map, debug tools, flashing, calibration, joystick binding, start-up."""
from dgcommon import diagram, Svg, INK, INK2, LINE, text_width, BLUE, GREEN, AMBER, RED


@diagram
def com_ports():
    s = Svg(880, 560)
    s.card(16, 20, 190, 100, "Windows PC", ["172.16.1.10", "Arduino IDE / arduino-cli", "uploads over USB"], "dark", title_size=15, line_size=12.5, center=True)
    rows = [
        ("Radio controller", "172.16.1.101", "Mega 2560 (replacement)", "COM20", "green"),
        ("Servo controller", "172.16.1.102", "Mega 2560 clone", "COM16", "green"),
        ("Upper controller", "172.16.1.103", "Mega 2560", "not recorded", "gray"),
        ("Stepper controller", "172.16.1.105", "Mega 2560", "COM4", "blue"),
        ("Dual stepper controller", "172.16.1.106", "Mega 2560", "COM5", "blue"),
        ("ADF controller", "172.16.1.107", "Mega 2560", "not recorded", "gray"),
        ("USB HID board", "172.16.1.11", "Arduino Leonardo", "COM8", "amber"),
    ]
    y = 20
    for name, ip, kind, com, th in rows:
        s.card(300, y, 400, 60, name, [f"{ip}   |   {kind}"], "white", title_size=13.5, line_size=12.5)
        s.pill(730, y + 18, com, th, size=13, pad=10, h=26)
        s.arrow([(206, 70), (250, 70), (250, y + 30), (300, y + 30)], None, color="#B0B0AB", sw=1.4) if com != "not recorded" else None
        y += 70
    s.text(16, 530, "COM numbers are the ones used at the last flash. They follow the USB socket and the board, so check them (see the guide) before an upload.", size=12.5, fill=INK2)
    s.text(16, 550, "Each board also has a network connection: USB is for programming only, the cockpit runs over UDP.", size=12.5, fill=INK2)
    return s


@diagram
def pcb_map():
    s = Svg(880, 700)
    s.text(16, 24, "Active PCBs, their revision and the sketches they belong to", size=15, weight=700)
    pcbs = [
        ("Jet Ranger Radio Auto Pilot  v1.3", "Radio controller (.101)", ["autopilot knobs, volume pots, MAX7219 display", "lines; backlight MOSFET on D5"], "blue"),
        ("Jet Ranger COM and NAV", "Radio controller (.101)", ["four swap switches in a diode matrix:", "column D26, rows D22-D25"], "blue"),
        ("Jet Ranger Front Panel Servo  v1.4", "Servo controller (.102)", ["switches D43, D47, D48; backlight D5", "MOSFET pin-out corrected in 1.4"], "green"),
        ("Jet Ranger Upper Controller  v1.4", "Upper controller (.103)", ["key matrix, brightness pot on A0,", "backlight MOSFET on D5"], "purple"),
        ("Jet Ranger 13 Stepper  v1.6", "Stepper controller (.105)", ["backlight D5, VSI coil B D8", "IRF540N backlight MOSFET"], "amber"),
        ("Jet Ranger OLED Dual Stepper  v1.5", "Dual stepper controller (.106)", ["D2 / D32 short removed in 1.4", "IRF540N backlight MOSFET"], "amber"),
        ("Jet Ranger ADF Controller  v1.5", "ADF controller (.107)", ["knobs, keypad, brightness pot, 12 V monitor,", "gear lamps, anti-ice / hyd boost / caution dim"], "teal"),
        ("Audio, Auto Pilot and ADF panels  v1.1", "switch panels on the controllers", ["diode-matrix switch boards, LEDs", "and rotary encoders"], "gray"),
    ]
    for i, (pcb, board, lines, th) in enumerate(pcbs):
        c, r = i % 2, i // 2
        x, y = 16 + c * 440, 50 + r * 140
        s.card(x, y, 408, 120, pcb, ["plugs into: " + board] + lines, th, title_size=14, line_size=12.5)
    s.text(16, 622, "COM and NAV (radio panel) has no revision set. Dual Stepper boards older than 1.4: fit only one of fuel quantity and fuel load.", size=12.5, fill=INK2)
    s.text(16, 644, "Check every pin against the sketch pin table and the KiCad files before ordering or wiring a new board.", size=12.5, fill=INK2)
    return s


@diagram
def debug_tools():
    s = Svg(880, 400)
    s.card(16, 30, 220, 130, "Arduino boards", ["debug text from every", "board, sent to the PC", "UDP 172.16.1.10:27000"], "white", title_size=14, line_size=12.5)
    s.card(300, 30, 270, 130, "UDP reflector", ["listens on 27000", "prints each line", "can forward to a second", "listener (analysis script)"], "blue", title_size=14, line_size=12.5)
    s.card(634, 30, 230, 130, "Other listeners", ["terminal window,", "log file, analysis script"], "white", title_size=14, line_size=12.5)
    s.arrow([(236, 95), (300, 95)], "udp", color=GREEN, label_dy=-10)
    s.arrow([(570, 95), (634, 95)], "forward", color=BLUE, label_dy=-10)
    s.card(16, 210, 270, 110, "Health Monitor", ["listens on 13137", "one light per board"], "gray", title_size=14, line_size=12.5)
    s.card(320, 210, 270, 110, "SwingTester", ["swings every gauge and", "lamp from the PC, uses", "ports 13136 and 4210"], "gray", title_size=14, line_size=12.5)
    s.card(624, 210, 240, 110, "StepperVSITester", ["VSI test sweep,", "sent to the Stepper"], "gray", title_size=14, line_size=12.5)
    s.text(16, 360, "Only one program can own a UDP port. If the reflector is running, do not start a second listener on 27000 - use its forwarding instead.", size=12.5, fill=INK2)
    s.text(16, 382, "The bridges listen on 27001 and the HID board on 4210, so run only one bridge and one HID sender at a time.", size=12.5, fill=INK2)
    return s


@diagram
def flash_workflow():
    s = Svg(880, 400)
    steps = [
        ("1", "Close monitor", "the serial monitor\nholds the COM port"),
        ("2", "Check port", "Device Manager or\narduino-cli board list"),
        ("3", "Compile", "arduino-cli compile\n--fqbn\narduino:avr:mega"),
        ("4", "Upload", "arduino-cli upload\n-p COMx --fqbn\narduino:avr:mega"),
        ("5", "Watch", "debug on 27000,\nheartbeat on 13137"),
    ]
    bw = 160
    for i, (n, t, d) in enumerate(steps):
        x = 16 + i * (bw + 12)
        s.card(x, 40, bw, 130, f"{n}  {t}", d.split("\n"), "blue" if i in (2, 3) else "white", title_size=13.5, line_size=12)
        if i < len(steps) - 1:
            s.arrow([(x + bw, 105), (x + bw + 12, 105)], None)
    s.card(16, 220, 420, 110, "Leonardo (USB HID board)", ["same flow with", "--fqbn arduino:avr:leonardo", "on COM8"], "amber", title_size=14, line_size=12.5)
    s.card(460, 220, 404, 110, "If the upload fails", ["'Access is denied': another program", "holds the port (serial monitor, IDE).", "Wrong port: re-run board list."], "red", title_size=14, line_size=12.5)
    s.text(16, 362, "Flash only the board you are working on. After a flash the board restarts and sends its first heartbeat within about 10 seconds.", size=12.5, fill=INK2)
    return s


@diagram
def calibration_flow():
    s = Svg(880, 360)
    steps = [("Send a raw value", "<CODE>RAW bypasses\nreal-unit scaling", "blue"),
             ("Read the dial", "note position against\nthe printed scale", "white"),
             ("Repeat at 3+ points", "zero, mid, full scale", "white"),
             ("Edit the table", "calibration table in the\nsketch (steps or degrees)", "amber"),
             ("Compile and flash", "see Flashing", "white"),
             ("Check with real units", "send a real value,\nconfirm the dial", "green")]
    for i, (t, d, th) in enumerate(steps):
        c, r = i % 3, i // 3
        x, y = 16 + c * 290, 30 + r * 140
        s.card(x, y, 266, 104, f"{i+1}  {t}", d.split("\n"), th, title_size=14, line_size=12.5)
        if c < 2:
            s.arrow([(x + 266, y + 52), (x + 290, y + 52)], None)
    s.arrow([(16 + 2 * 290 + 133, 134), (16 + 2 * 290 + 133, 150), (16 + 133, 150), (16 + 133, 170)], None, dash="4 3")
    s.text(16, 322, "Typical tables: pitch -30 / 0 / +30 degrees, bank -90 / 0 / +90 degrees, one row per stepper or servo gauge.", size=12.5, fill=INK2)
    s.text(16, 344, "Sending <CODE>RAW:n moves the motor to step n directly, so any dial can be calibrated without a simulator.", size=12.5, fill=INK2)
    return s


@diagram
def joystick_binding():
    s = Svg(880, 470)
    s.card(16, 20, 190, 94, "Cockpit switch", ["Warning Mute,", "Fuel Valve, TOT Clear..."], "white", center=True)
    s.card(240, 20, 190, 94, "Servo / Upper Mega", ["debounced change ->", "index,state"], "gray", center=True)
    s.card(464, 20, 190, 94, "USB HID board", ["joystick 1..4,", "button 0..31"], "amber", center=True)
    s.card(688, 20, 176, 94, "Windows", ["game controller", "(joy.cpl)"], "white", center=True)
    for a, b in ((206, 240), (430, 464), (654, 688)):
        s.arrow([(a, 67), (b, 67)], None)
    s.arrow([(776, 114), (776, 140), (230, 140), (230, 166)], None)
    s.card(16, 166, 424, 150, "FSUIPC (P3D / FSX)", ["Buttons + Switches tab", "1. Operate the switch: FSUIPC shows its", "    joystick letter and button number", "2. Select for FS control, pick the event", "3. Set the released side too, then OK"], "blue", title_size=14, line_size=12.5)
    s.card(464, 166, 400, 150, "Bindings for the servo board switches", ["Warning Mute: Joy 1 button 8", "  DS206.WARNING_MUTE_BUTTON_SET", "Fuel Valve: Joy 1 button 10", "  DS206.FUEL_VALVE_SWITCH_SET", "TOT Clear: Joy 1 button 9", "  DS206.TOT_TEST_BUTTON_SET"], "white", title_size=14, line_size=12.5)
    s.arrow([(440, 240), (464, 240)], None)
    s.card(16, 340, 848, 90, "MSFS", ["Bind the same joystick buttons in the MSFS controls menu. The DS206 events are an FSUIPC / DodoSim", "feature and are not available in MSFS."], "gray", title_size=14, line_size=12.5)
    s.text(16, 454, "FSUIPC counts buttons from 0 and Windows from 1, so Windows button 10 appears as button 9 in FSUIPC.", size=12.5, fill=INK2)
    return s


@diagram
def setup_flow():
    s = Svg(880, 360)
    steps = [("Power the cockpit", "main and avionics", "gray"),
             ("Start the reflector", "UDP 27000", "white"),
             ("Start the simulator", "P3D / FSX or MSFS", "white"),
             ("Start ONE bridge", "FSUIPC or MSFS", "blue"),
             ("Open Health Monitor", "all lights green", "green"),
             ("Fly", "gauges follow the sim", "green")]
    for i, (t, d, th) in enumerate(steps):
        c, r = i % 3, i // 3
        x, y = 16 + c * 290, 30 + r * 130
        s.card(x, y, 266, 96, f"{i+1}  {t}", [d], th, title_size=14, line_size=12.5)
        if c < 2:
            s.arrow([(x + 266, y + 48), (x + 290, y + 48)], None)
    s.arrow([(16 + 2 * 290 + 133, 126), (16 + 2 * 290 + 133, 142), (16 + 133, 142), (16 + 133, 160)], None, dash="4 3")
    s.text(16, 320, "Order matters little: every board waits for data, and the bridge reconnects to the simulator on its own every second.", size=12.5, fill=INK2)
    s.text(16, 342, "If a gauge stays at zero for 30 s without data, the board's watchdog parked it on purpose.", size=12.5, fill=INK2)
    return s
