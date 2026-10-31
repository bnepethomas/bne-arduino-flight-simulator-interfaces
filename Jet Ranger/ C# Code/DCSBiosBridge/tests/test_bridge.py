"""End-to-end test of DCSBiosBridge without DCS: sends a synthetic DCS-BIOS export stream and checks the board packets the bridge produces.

    python tests\\test_bridge.py [path to DCSBiosBridge.exe]

The bridge is started with --board-host 127.0.0.1, so every board packet arrives on UDP 13136 on this PC and the GPS packet on 27002. The fake aircraft "TestModule"
(tests\\TestModule.json, tests\\profiles\\TestModule.json and its .calibration.json) has three lamps, an airspeed, a bus voltage, four radar altimeter digits, a calibrated
torque needle and an uncalibrated vertical speed needle; tests\\CommonData.json and tests\\profiles\\_common.json are the values every aircraft gives (airspeed, altitude,
heading, position). A fourth phase flies an aircraft with no profile ("OtherPlane") to check that the common values alone still work. Only the bridge process this
script starts is stopped at the end.
"""
import os, socket, struct, subprocess, sys, threading, time

HERE = os.path.dirname(os.path.abspath(__file__))
EXE = sys.argv[1] if len(sys.argv) > 1 else os.path.join(HERE, "..", "bin", "Release", "net10.0-windows", "DCSBiosBridge.exe")

received = []
gps = []
stop = threading.Event()


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


def block(addr, data):
    return struct.pack("<HH", addr, len(data)) + data


def frame(name, lamps, ias, bus, ias_us, alt, digits, tq, vsi):
    """One DCS-BIOS frame: the aircraft name, the test module's words and CommonData's words (airspeed 123 kt etc. at 4136 onwards)."""
    f = b"\x55\x55\x55\x55"
    f += block(0x0000, name.encode().ljust(24, b"\0"))
    f += block(0x1000, struct.pack("<HHHH", lamps & 3, (lamps >> 2) & 1, ias, bus))
    f += block(4136, struct.pack("<HHHHHH", ias_us, alt, 90, 27, 24, 0) + b"S\0" + struct.pack("<HHH", 153, 0, 0) + b"E\0")
    f += block(4160, struct.pack("<HHHH", *[int(round(d / 10 * 65535)) for d in digits]))
    f += block(4168, struct.pack("<HH", tq, vsi))
    f += block(4176, struct.pack("<HHHH", 0, 36000, 8, 15712))      # mission start 10:00:00 (36000 s) plus model time 5400.00 s (540000 hundredths) = 11:30:00 local
    return f


def main():
    threading.Thread(target=listen, args=(13136, received), daemon=True).start()
    threading.Thread(target=listen, args=(27002, gps), daemon=True).start()
    exe = os.path.abspath(EXE)
    proc = subprocess.Popen([exe, "--board-host", "127.0.0.1", "--utc-offset", "4", "--json-dir", HERE, "--profile-dir", os.path.join(HERE, "profiles")])
    marks = []
    try:
        time.sleep(2.0)
        tx = socket.socket(socket.AF_INET, socket.SOCK_DGRAM, socket.IPPROTO_UDP)
        tx.setsockopt(socket.IPPROTO_IP, socket.IP_MULTICAST_IF, socket.inet_aton("127.0.0.1"))
        tx.setsockopt(socket.IPPROTO_IP, socket.IP_MULTICAST_TTL, 1)
        tx.setsockopt(socket.IPPROTO_IP, socket.IP_MULTICAST_LOOP, 1)
        dest = ("239.255.50.10", 5010)
        half = 32768
        v28 = int(28 / 30 * 65535)
        phases = [
            # label, aircraft, lamps, ias, bus, common ias_us, alt ft, drum digits, torque, vsi, seconds
            ("lamp A on, bus 28 V", "TestModule", 0b001, 0, v28, 123, 3200, (1, 2, 3, 4), half, half, 1.5),
            ("lamps B and C on, 100 kt, bus 28 V", "TestModule", 0b110, int(100 / 200 * 65535), v28, 123, 3200, (1, 2, 3, 4), half, half, 1.5),
            ("bus 10 V (main bus down)", "TestModule", 0b111, int(100 / 200 * 65535), int(10 / 30 * 65535), 123, 3200, (1, 2, 3, 4), half, half, 1.5),
            ("an aircraft with no profile: common values only", "OtherPlane", 0, 0, 0, 123, 3200, (0, 0, 0, 0), 0, 0, 1.5),
        ]
        for label, name, lamps, ias, bus, ias_us, alt, digits, tq, vsi, seconds in phases:
            marks.append((label, time.time()))
            end = time.time() + seconds
            while time.time() < end:
                f = frame(name, lamps, ias, bus, ias_us, alt, digits, tq, vsi)
                cut = len(f) // 2          # one frame split over two datagrams, as the stream may be
                tx.sendto(f[:cut], dest)
                tx.sendto(f[cut:], dest)
                time.sleep(1 / 30)
        marks.append(("end", time.time()))
    finally:
        time.sleep(0.5)
        stop.set()
        proc.terminate()
        try:
            proc.wait(5)
        except subprocess.TimeoutExpired:
            proc.kill()

    def during(i, skip=0.4, sink=None):
        a, b = marks[i][1] + skip, marks[i + 1][1]     # the other boards are resent every 90 ms, so skip the start of a phase
        return [d for ts, d in (sink if sink is not None else received) if a <= ts <= b]

    ok = True

    def check(name, cond):
        nonlocal ok
        print(("PASS " if cond else "FAIL ") + name)
        ok = ok and cond

    print("packets received:", len(received), "gps:", len(gps))
    p1, p2, p3, p4 = during(0), during(1), during(2), during(3)
    # the radio packet goes out on change (at most every 200 ms) or every 5 s, so it is looked for from the start of each phase
    radio1 = [d for d in during(0, 0.0) if d.startswith("D,NAVCOM1")]
    radio2 = [d for d in during(1, 0.0) if d.startswith("D,NAVCOM1")]
    radio3 = [d for d in during(2, 0.0) if d.startswith("D,NAVCOM1")]
    check("phase 1 radio packet", bool(radio1))
    check("phase 1: NAVCOM1:1, ENG_OIL on, ENG_ICE off", bool(radio1) and "NAVCOM1:1" in radio1[-1] and "LED_H_ENG_OIL:1" in radio1[-1] and "LED_H_ENG_ICE:0" in radio1[-1])
    check("phase 2: ENG_ICE and IFF on (decoded across split datagrams, mask and shift)", bool(radio2) and "LED_H_ENG_ICE:1" in radio2[-1] and "LED_H_IFF:1" in radio2[-1] and "LED_H_ENG_OIL:0" in radio2[-1])
    check("phase 2: aircraft IAS (100.0) wins over the common IAS (123)", any("IAS:100.0" in d for d in p2) and not any("IAS:123" in d for d in p2))
    check("phase 1: ADF/Upper backlight D,BPWR:1", "D,BPWR:1" in p1)
    check("phase 3: main bus down, radio LEDs off, NAVCOM1:0, BPWR:0", bool(radio3) and "NAVCOM1:0" in radio3[-1] and "LED_H_IFF:0" in radio3[-1] and "BPWR:0" in radio3[-1])
    check("phase 3: ADF/Upper backlight D,BPWR:0", "D,BPWR:0" in p3)
    check("radar altimeter drum digits 1,2,3,4 -> AGL:1234 (Stepper)", any("AGL:1234" in d for d in p1))
    check("calibrated torque: 0.5 of the dial -> TQ:20 (not the 60 of the plain scale)", any("TQ:20" in d for d in p1) and not any("TQ:60" in d for d in p1))
    check("uncalibrated vertical speed: 0.5 of the dial -> VSI:0 (scale and offset)", any("VSI:0" in d for d in p1))
    check("common altitude 3200 ft -> ALT:3200 (Dual Stepper)", any(d.startswith("D,PWR:1") and "ALT:3200" in d for d in p1))
    g1 = during(0, 0.4, gps)
    check("GPS packet: S 27 deg 24 min = -27.4, E 153 = 153.0, heading 90, 3200 ft", any("LAT:-27.400000" in d and "LON:153.000000" in d and "ALT:3200" in d and "HDG:90" in d for d in g1))
    check("GPS packet carries the simulator's UTC: mission clock 11:30:00 local at +4 h is 07:30:00 UTC (PC's date)", any(d.endswith(",UTC:" + time.strftime("%Y%m%d", time.gmtime()) + "073000.000") for d in g1))
    check("phase 4 (no profile): common IAS 123 and ALT 3200 are still sent", any("IAS:123" in d for d in p4) and any("ALT:3200" in d for d in p4))
    check("phase 4 (no profile): no lamp or gauge-needle tokens", not any(("TQ:" in d or "LED_H_" in d or "VSI:" in d) for d in p4))
    print("ALL PASS" if ok else "SOME FAILED")
    if not ok:
        for ts, d in received[-12:]:
            print("   ", d)
    return 0 if ok else 1


if __name__ == "__main__":
    sys.exit(main())
