"""Runs the DCS bridge and then the X-Plane bridge with --emulators-only against the five panel emulators and checks what the emulators show, so the whole path
simulator data -> bridge -> board packet -> emulator display is covered. Nothing is sent to the real boards (--emulators-only).

    python tests\\test_bridges_to_emulators.py

The DCS bridge is fed the synthetic DCS-BIOS stream of its own test (aircraft TestModule); the X-Plane bridge talks to the fake X-Plane of its own test on UDP 49100.
Only the processes this script starts are stopped.
"""
import json, os, shutil, socket, subprocess, sys, tempfile, threading, time

HERE = os.path.dirname(os.path.abspath(__file__))
CODE = os.path.dirname(os.path.dirname(HERE))
EMU = os.path.join(CODE, "JetRangerPanelEmulator", "bin", "Release", "net10.0-windows", "JetRangerPanelEmulator.exe")
DCS_DIR = os.path.join(CODE, "DCSBiosBridge")
DCS = os.path.join(DCS_DIR, "bin", "Release", "net10.0-windows", "DCSBiosBridge.exe")
XP_DIR = os.path.join(CODE, "XPlaneBridge")
XP = os.path.join(XP_DIR, "bin", "Release", "net10.0-windows", "XPlaneBridge.exe")

sys.argv = [sys.argv[0]]
sys.path.insert(0, os.path.join(DCS_DIR, "tests"))
sys.path.insert(0, os.path.join(XP_DIR, "tests"))
import importlib.util


def load(name, path):
    spec = importlib.util.spec_from_file_location(name, path)
    mod = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(mod)
    return mod


dcs_test = load("dcs_test", os.path.join(DCS_DIR, "tests", "test_bridge.py"))
xp_test = load("xp_test", os.path.join(XP_DIR, "tests", "test_bridge.py"))

dump = tempfile.mkdtemp(prefix="emu_")
ok = True


def state(board):
    try:
        with open(os.path.join(dump, board + ".json")) as f:
            return json.load(f)
    except (OSError, ValueError):
        return {}


def wait(board, cond, seconds=8.0):
    end = time.time() + seconds
    s = {}
    while time.time() < end:
        s = state(board)
        if s and cond(s):
            return s
        time.sleep(0.15)
    return s


def check(name, cond):
    global ok
    print(("PASS " if cond else "FAIL ") + name)
    ok = ok and bool(cond)


def stop(proc):
    proc.terminate()
    try:
        proc.wait(5)
    except subprocess.TimeoutExpired:
        proc.kill()


def run_dcs():
    proc = subprocess.Popen([DCS, "--emulators-only", "--json-dir", os.path.join(DCS_DIR, "tests"), "--profile-dir", os.path.join(DCS_DIR, "tests", "profiles")])
    stop_feed = threading.Event()

    def feed():
        tx = socket.socket(socket.AF_INET, socket.SOCK_DGRAM, socket.IPPROTO_UDP)
        tx.setsockopt(socket.IPPROTO_IP, socket.IP_MULTICAST_IF, socket.inet_aton("127.0.0.1"))
        tx.setsockopt(socket.IPPROTO_IP, socket.IP_MULTICAST_TTL, 1)
        tx.setsockopt(socket.IPPROTO_IP, socket.IP_MULTICAST_LOOP, 1)
        v28 = int(28 / 30 * 65535)
        while not stop_feed.is_set():
            f = dcs_test.frame("TestModule", 0b001, int(100 / 200 * 65535), v28, 123, 3200, (1, 2, 3, 4), 32768, 32768)
            cut = len(f) // 2
            tx.sendto(f[:cut], ("239.255.50.10", 5010))
            tx.sendto(f[cut:], ("239.255.50.10", 5010))
            time.sleep(1 / 30)

    t = threading.Thread(target=feed, daemon=True)
    t.start()
    try:
        s = wait("radio", lambda s: s.get("LED_H_ENG_OIL") == "1")
        check("DCS bridge -> radio emulator: the Huey engine oil LED is on, ENG ICE off", s.get("LED_H_ENG_OIL") == "1" and s.get("LED_H_ENG_ICE") == "0")
        s = wait("stepper", lambda s: s.get("AGL") == "1234" and s.get("IAS") == "100")
        check("DCS bridge -> stepper emulator: IAS 100", s.get("IAS") == "100")
        check("DCS bridge -> stepper emulator: AGL 1234", s.get("AGL") == "1234")
        s = wait("dual", lambda s: s.get("ALT") == "3200")
        check("DCS bridge -> dual stepper emulator: altitude 3200 and the OLED shows it", s.get("ALT") == "3200" and s.get("oled.alt") == "3200")
        s = wait("adf", lambda s: int(s.get("_packets", 0)) > 3)
        check("DCS bridge -> ADF emulator: the backlight power packets arrive", int(s.get("_packets", 0)) > 3)
    finally:
        stop_feed.set()
        stop(proc)


def run_xplane():
    server = threading.Thread(target=xp_test.fake_xplane, daemon=True)
    server.start()
    time.sleep(0.5)
    proc = subprocess.Popen([XP, "--emulators-only", "--xplane-host", "127.0.0.1", "--xplane-port", str(xp_test.XPORT), "--profile-dir", os.path.join(XP_DIR, "profiles"), "--module", "Robinson R22"])
    try:
        s = wait("stepper", lambda s: s.get("IAS") == "80")
        check("X-Plane bridge -> stepper emulator: IAS 80, N1 91, ITT 610, torque 50, fuel 33", (s.get("IAS"), s.get("N1"), s.get("ITT"), s.get("TQ"), s.get("FUEL")) == ("80", "91", "610", "50", "33"))
        s = wait("dual", lambda s: s.get("oled.clock") == "14:30")
        check("X-Plane bridge -> dual stepper emulator: altitude 3200, clock 14:30, temperature 21C, rotor 100 percent (R22 profile)",
              s.get("ALT") == "3200" and s.get("oled.clock") == "14:30" and s.get("oled.temp") == "21C" and s.get("RPMR") == "100")
        s = wait("servo", lambda s: s.get("RLOW") == "1")
        check("X-Plane bridge -> servo emulator: pitch 4.5, bank -12, rotor low and low fuel lamps on", s.get("PITCH") == "4.5" and s.get("BANK") == "-12" and s.get("RLOW") == "1" and s.get("LOWF") == "1")
        s = wait("radio", lambda s: s.get("LED_H_20_MINUTE_FUEL") == "1")
        check("X-Plane bridge -> radio emulator: the 20 minute fuel LED is on", s.get("LED_H_20_MINUTE_FUEL") == "1")
    finally:
        stop(proc)
        xp_test.stop.set()


def main():
    emu = subprocess.Popen([EMU, "--board", "all", "--dump", dump])
    try:
        time.sleep(2.5)
        run_dcs()
        run_xplane()
    finally:
        stop(emu)
        shutil.rmtree(dump, ignore_errors=True)
    print("ALL PASS" if ok else "SOME FAILED")
    return 0 if ok else 1


if __name__ == "__main__":
    sys.exit(main())
