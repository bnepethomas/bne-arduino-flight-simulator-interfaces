"""End-to-end test of DCSBiosBridge without DCS: sends a synthetic DCS-BIOS export stream and checks the board packets the bridge produces.

    python tests\\test_bridge.py [path to DCSBiosBridge.exe]

The bridge is started with --board-host 127.0.0.1, so every board packet arrives on UDP 13136 on this PC. The fake aircraft is "TestModule" (tests\\TestModule.json
and tests\\profiles\\TestModule.json): three lamps, an airspeed and a bus voltage. Only the bridge process this script starts is stopped at the end.
"""
import os, socket, struct, subprocess, sys, threading, time

HERE = os.path.dirname(os.path.abspath(__file__))
EXE = sys.argv[1] if len(sys.argv) > 1 else os.path.join(HERE, "..", "bin", "Release", "net10.0-windows", "DCSBiosBridge.exe")

received = []
stop = threading.Event()


def listen():
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    s.bind(("127.0.0.1", 13136))
    s.settimeout(0.2)
    while not stop.is_set():
        try:
            data, _ = s.recvfrom(2000)
            received.append((time.time(), data.decode("ascii", "replace")))
        except socket.timeout:
            pass
    s.close()


def block(addr, data):
    return struct.pack("<HH", addr, len(data)) + data


def frame(lamps, ias, bus):
    name = b"TestModule".ljust(24, b"\0")
    words = struct.pack("<HHHH", lamps & 3, (lamps >> 2) & 1, ias, bus)
    return b"\x55\x55\x55\x55" + block(0x0000, name) + block(0x1000, words)


def main():
    t = threading.Thread(target=listen, daemon=True)
    t.start()
    exe = os.path.abspath(EXE)
    proc = subprocess.Popen([exe, "--board-host", "127.0.0.1", "--json-dir", HERE, "--profile-dir", os.path.join(HERE, "profiles")])
    try:
        time.sleep(2.0)
        tx = socket.socket(socket.AF_INET, socket.SOCK_DGRAM, socket.IPPROTO_UDP)
        tx.setsockopt(socket.IPPROTO_IP, socket.IP_MULTICAST_IF, socket.inet_aton("127.0.0.1"))
        tx.setsockopt(socket.IPPROTO_IP, socket.IP_MULTICAST_TTL, 1)
        tx.setsockopt(socket.IPPROTO_IP, socket.IP_MULTICAST_LOOP, 1)
        dest = ("239.255.50.10", 5010)
        phases = [
            ("lamp A on, bus 28 V", 0b001, 0, int(28 / 30 * 65535), 1.5),
            ("lamps B and C on, 100 kt, bus 28 V", 0b110, int(100 / 200 * 65535), int(28 / 30 * 65535), 1.5),
            ("bus 10 V (main bus down)", 0b111, int(100 / 200 * 65535), int(10 / 30 * 65535), 1.5),
        ]
        marks = []
        for label, lamps, ias, bus, seconds in phases:
            marks.append((label, time.time()))
            end = time.time() + seconds
            while time.time() < end:
                f = frame(lamps, ias, bus)
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

    def during(i, skip=0.4):
        a, b = marks[i][1] + skip, marks[i + 1][1]     # the other boards are resent every 90 ms, so skip the start of a phase
        return [d for ts, d in received if a <= ts <= b]

    ok = True

    def check(name, cond):
        nonlocal ok
        print(("PASS " if cond else "FAIL ") + name)
        ok = ok and cond

    print("packets received:", len(received))
    p1, p2, p3 = during(0), during(1), during(2)
    # the radio packet goes out on change (at most every 200 ms) or every 5 s, so it is looked for from the start of each phase
    radio1 = [d for d in during(0, 0.0) if d.startswith("D,NAVCOM1")]
    radio2 = [d for d in during(1, 0.0) if d.startswith("D,NAVCOM1")]
    radio3 = [d for d in during(2, 0.0) if d.startswith("D,NAVCOM1")]
    check("phase 1 radio packet", bool(radio1))
    check("phase 1: NAVCOM1:1, ENG_OIL on, ENG_ICE off", bool(radio1) and "NAVCOM1:1" in radio1[-1] and "LED_H_ENG_OIL:1" in radio1[-1] and "LED_H_ENG_ICE:0" in radio1[-1])
    check("phase 2: ENG_ICE and IFF on (decoded across split datagrams, mask and shift)", bool(radio2) and "LED_H_ENG_ICE:1" in radio2[-1] and "LED_H_IFF:1" in radio2[-1] and "LED_H_ENG_OIL:0" in radio2[-1])
    check("phase 2: IAS packet to the stepper board", any("IAS:100.0" in d for d in p2))
    check("phase 1: ADF/Upper backlight D,BPWR:1", "D,BPWR:1" in p1)
    check("phase 3: main bus down, radio LEDs off, NAVCOM1:0, BPWR:0", bool(radio3) and "NAVCOM1:0" in radio3[-1] and "LED_H_IFF:0" in radio3[-1] and "BPWR:0" in radio3[-1])
    check("phase 3: ADF/Upper backlight D,BPWR:0", "D,BPWR:0" in p3)
    print("ALL PASS" if ok else "SOME FAILED")
    if not ok:
        for ts, d in received[-12:]:
            print("   ", d)
    return 0 if ok else 1


if __name__ == "__main__":
    sys.exit(main())
