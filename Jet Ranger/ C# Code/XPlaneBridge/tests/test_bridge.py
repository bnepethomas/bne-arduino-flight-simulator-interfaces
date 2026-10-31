"""End-to-end test of XPlaneBridge without X-Plane: a fake X-Plane answers the bridge's RREF dataref subscriptions with known values and the script checks the packets
the bridge sends to the boards.

    python tests\\test_bridge.py [path to XPlaneBridge.exe]

The fake X-Plane listens on UDP 127.0.0.1:49100 (the bridge is started with --xplane-port 49100, so a real X-Plane on 49000 is not disturbed) and the bridge with
--board-host 127.0.0.1, so every board packet arrives on UDP 13136 here and the GPS packet on 27002. The real profiles (profiles\\_common.json and Robinson R22.json) are used.
Run 1 is the generic profile (any aircraft); run 2 is the Robinson R22 profile, whose bus voltage is then switched off. Only the bridge processes this script starts are stopped.
"""
import os, socket, struct, subprocess, sys, threading, time

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.dirname(HERE)
EXE = sys.argv[1] if len(sys.argv) > 1 else os.path.join(ROOT, "bin", "Release", "net10.0-windows", "XPlaneBridge.exe")
XPORT = 49100

VALUES = {
    "sim/cockpit2/gauges/indicators/airspeed_kts_pilot": 80.0,
    "sim/cockpit2/gauges/indicators/altitude_ft_pilot": 3200.4,
    "sim/cockpit2/gauges/indicators/vvi_fpm_pilot": 500.0,
    "sim/cockpit2/gauges/indicators/radio_altimeter_height_ft_pilot": 150.0,
    "sim/cockpit2/gauges/indicators/pitch_AHARS_deg_pilot": 4.5,
    "sim/cockpit2/gauges/indicators/roll_AHARS_deg_pilot": -12.0,
    "sim/flightmodel/position/true_psi": 90.0,
    "sim/flightmodel/position/latitude": -27.4,
    "sim/flightmodel/position/longitude": 153.0,
    "sim/cockpit2/engine/indicators/N1_percent[0]": 91.0,
    "sim/cockpit2/engine/indicators/N2_percent[0]": 100.0,
    "sim/cockpit2/engine/indicators/ITT_deg_C[0]": 610.0,
    "sim/cockpit2/engine/indicators/torque_n_mtr[0]": 6000.0,
    "sim/flightmodel/engine/POINT_max_TRQ[0]": 12000.0,
    "sim/cockpit2/engine/indicators/oil_pressure_psi[0]": 60.0,
    "sim/cockpit2/engine/indicators/oil_temperature_deg_C[0]": 70.0,
    "sim/cockpit2/engine/indicators/fuel_pressure_psi[0]": 14.0,
    "sim/flightmodel/weight/m_fuel_total": 100.0,
    "sim/cockpit2/clock_timer/zulu_time_hours": 14.0,
    "sim/cockpit2/clock_timer/zulu_time_minutes": 30.0,
    "sim/cockpit2/temperature/outside_air_temp_degc": 21.0,
    "sim/cockpit2/annunciators/low_rotor": 1.0,
    "sim/cockpit2/annunciators/fuel_quantity": 1.0,
    "sim/flightmodel/engine/ENGN_running[0]": 1.0,
    "sim/cockpit2/engine/indicators/prop_speed_rpm[0]": 530.0,
    "sim/cockpit2/electrical/bus_volts[0]": 14.0,
}

received = []
gps = []
stop = threading.Event()
subs = {}            # (address) -> {index: path}
subs_lock = threading.Lock()


def listen(port, sink):
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    s.bind(("127.0.0.1", port))
    s.settimeout(0.2)
    while not stop.is_set():
        try:
            data, _ = s.recvfrom(2000)
            sink.append((time.time(), data.decode("ascii", "replace")))
        except socket.timeout:
            pass
    s.close()


def fake_xplane():
    """Answers RREF requests (header RREF + NUL, int32 Hz, int32 index, 400 byte path) with RREF, then pairs of int32 index and float32 value."""
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    s.bind(("127.0.0.1", XPORT))
    s.settimeout(0.05)
    last = 0.0
    while not stop.is_set():
        try:
            data, addr = s.recvfrom(2000)
            if data[:5] == b"RREF\0" and len(data) >= 413:
                freq, index = struct.unpack("<ii", data[5:13])
                path = data[13:413].split(b"\0")[0].decode()
                with subs_lock:
                    if freq == 0:
                        subs.get(addr, {}).pop(index, None)
                    else:
                        subs.setdefault(addr, {})[index] = path
        except (socket.timeout, ConnectionResetError):
            pass
        if time.time() - last >= 0.1:
            last = time.time()
            with subs_lock:
                for addr, table in list(subs.items()):
                    if not table:
                        continue
                    body = b"RREF,"
                    for index, path in table.items():
                        body += struct.pack("<if", index, float(VALUES.get(path, 0.0)))
                    try:
                        s.sendto(body, addr)
                    except OSError:
                        pass
    s.close()


def run_bridge(extra, seconds, after=None):
    """Starts the bridge, lets it run `seconds` (calling after(t) once half way) and returns the time window."""
    exe = os.path.abspath(EXE)
    proc = subprocess.Popen([exe, "--board-host", "127.0.0.1", "--xplane-host", "127.0.0.1", "--xplane-port", str(XPORT), "--profile-dir", os.path.join(ROOT, "profiles")] + extra)
    start = time.time()
    try:
        time.sleep(seconds / 2)
        mid = time.time()
        if after:
            after()
        time.sleep(seconds / 2)
        return start, mid, time.time()
    finally:
        proc.terminate()
        try:
            proc.wait(5)
        except subprocess.TimeoutExpired:
            proc.kill()


def main():
    for t in (threading.Thread(target=listen, args=(13136, received), daemon=True), threading.Thread(target=listen, args=(27002, gps), daemon=True),
              threading.Thread(target=fake_xplane, daemon=True)):
        t.start()
    time.sleep(0.5)
    a0, _, a1 = run_bridge([], 4.0)
    with subs_lock:
        subs.clear()
    b0, bmid, b1 = run_bridge(["--module", "Robinson R22"], 5.0, lambda: VALUES.__setitem__("sim/cockpit2/electrical/bus_volts[0]", 0.0))
    time.sleep(0.3)
    stop.set()

    def win(a, b, sink=None):
        return [d for ts, d in (sink if sink is not None else received) if a <= ts <= b]

    ok = True

    def check(name, cond):
        nonlocal ok
        print(("PASS " if cond else "FAIL ") + name)
        ok = ok and cond

    run1 = win(a0 + 1.0, a1)
    print("packets received:", len(received), "gps:", len(gps))
    first = [d for d in run1 if "IAS:" in d]
    check("generic: stepper IAS 80.0, AGL 150, VSI 500, N1 91, torque 50 percent of the maximum, fuel 33 gal, ITT 610",
          any(all(s in d for s in ("IAS:80.0", "AGL:150", "VSI:500", "N1:91", "TQ:50", "FUEL:33", "ITT:610")) for d in run1))
    check("generic: dual stepper ALT 3200, oil 60 psi and 70 C, fuel pressure 14, engine speed 100.0, clock 1430, temperature 21C",
          any(all(s in d for s in ("D,PWR:1", "ALT:3200", "OILP:60", "OILT:70", "FUELLOAD:14", "RPME:100.0", "ZULU:1430", "TEMP:21C")) for d in run1))
    check("generic: servo pitch 4.5, bank -12.0, rotor-low and low-fuel lamps on, generator and engine-out lamps off",
          any(all(s in d for s in ("PITCH:4.5", "BANK:-12.0", "RLOW:1", "LOWF:1", "GENF:0", "EOUT:0")) for d in run1))
    check("generic: radio packet with NAVCOM1:1 and the Huey 20 minute fuel LED on (fuel quantity annunciator)",
          any(d.startswith("D,NAVCOM1:1") and "LED_H_20_MINUTE_FUEL:1" in d and "LED_H_ENG_OIL:0" in d for d in win(a0, a1)))
    check("generic: no rotor speed or bus voltage (no aircraft profile): no RPMR token, backlight BPWR:1", not any("RPMR:" in d for d in run1) and "D,BPWR:1" in run1)
    check("generic: GPS packet -27.4, 153.0, 3200 ft, 80.0 kt, heading 90", any("LAT:-27.400000" in d and "LON:153.000000" in d and "ALT:3200" in d and "IAS:80.0" in d and "HDG:90" in d for d in win(a0 + 1.0, a1, gps)))

    run2a = win(b0 + 1.0, bmid)
    run2b = win(bmid + 1.0, b1)
    run2a_radio = [d for d in win(b0, bmid) if d.startswith("D,NAVCOM1")]
    check("R22: rotor speed 530 rpm = RPMR:100.0", any("RPMR:100.0" in d for d in run2a))
    check("R22: bus 14 V doubled to 28 V: radio NAVCOM1:1 and BPWR:1", any("NAVCOM1:1" in d and "BPWR:1" in d for d in run2a_radio))
    radio_b = [d for d in win(bmid, b1) if d.startswith("D,NAVCOM1")]
    check("R22: bus 0 V: radio NAVCOM1:0, Huey LEDs dark, BPWR:0", bool(radio_b) and "NAVCOM1:0" in radio_b[-1] and "LED_H_20_MINUTE_FUEL:0" in radio_b[-1] and "BPWR:0" in radio_b[-1])
    check("R22: bus 0 V: radar altimeter reads zero (AGL:0) and the ADF/Upper backlight is off", any("AGL:0" in d for d in run2b) and "D,BPWR:0" in run2b)
    print("ALL PASS" if ok else "SOME FAILED")
    if not ok:
        for ts, d in received[-10:]:
            print("   ", d)
    return 0 if ok else 1


if __name__ == "__main__":
    sys.exit(main())
