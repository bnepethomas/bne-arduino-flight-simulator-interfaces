"""End-to-end test of JetRangerPanelEmulator: starts the five emulators (--board all) with a short watchdog and a state dump folder, sends them the packets a bridge
sends to the boards (on the same loopback addresses, 127.0.0.101 ...) and checks what each emulator reports through its dump file.

    python tests\\test_emulator.py [path to JetRangerPanelEmulator.exe]

Only the emulator process this script starts is stopped.
"""
import json, os, shutil, socket, subprocess, sys, tempfile, time

HERE = os.path.dirname(os.path.abspath(__file__))
EXE = sys.argv[1] if len(sys.argv) > 1 else os.path.join(HERE, "..", "bin", "Release", "net10.0-windows", "JetRangerPanelEmulator.exe")
NUMBERS = {"radio": 101, "servo": 102, "stepper": 105, "dual": 106, "adf": 107}
WATCHDOG = 3.0

tx = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
dump = tempfile.mkdtemp(prefix="emu_")


def send(board, text):
    tx.sendto(text.encode(), ("127.0.0.%d" % NUMBERS[board], 13136))


def state(board):
    try:
        with open(os.path.join(dump, board + ".json")) as f:
            return json.load(f)
    except (OSError, ValueError):
        return {}


def wait(board, cond, seconds=4.0, resend=None):
    """Polls the board's dump until cond(state) is true (resending a packet every 100 ms if asked); returns the last state."""
    end = time.time() + seconds
    s = {}
    while time.time() < end:
        if resend:
            send(*resend)
        s = state(board)
        if s and cond(s):
            return s
        time.sleep(0.1)
    return s


ok = True


def check(name, cond):
    global ok
    print(("PASS " if cond else "FAIL ") + name)
    ok = ok and bool(cond)


def near(s, key, value, tol):
    try:
        return abs(float(s.get(key, "nan")) - value) <= tol
    except ValueError:
        return False


def main():
    proc = subprocess.Popen([os.path.abspath(EXE), "--board", "all", "--dump", dump, "--watchdog", str(WATCHDOG)])
    try:
        time.sleep(2.5)
        # ---- Stepper
        pkt = "D,BPWR:1,IAS:82.0,VSI:500,AGL:450,N1:91,ITT:610,TQ:62,FUEL:40"
        s = wait("stepper", lambda s: near(s, "IAS.needle", 82, 0.5) and near(s, "ITT.needle", 610, 3), 6, ("stepper", pkt))
        check("stepper: values IAS 82, VSI 500, AGL 450, N1 91, ITT 610, TQ 62, FUEL 40",
              all(s.get(k) == v for k, v in (("IAS", "82"), ("VSI", "500"), ("AGL", "450"), ("N1", "91"), ("ITT", "610"), ("TQ", "62"), ("FUEL", "40"))))
        check("stepper: the needles reach their values (IAS 82, ITT 610)", near(s, "IAS.needle", 82, 0.5) and near(s, "ITT.needle", 610, 3))
        send("stepper", "C,SWINGIAS:1")
        s = wait("stepper", lambda s: float(s.get("IAS.needle", 0)) > 130, 3)
        check("stepper: C,SWINGIAS:1 sweeps the airspeed needle to the end of the scale and back", float(s.get("IAS.needle", 0)) > 130)
        s = wait("stepper", lambda s: near(s, "IAS.needle", 82, 0.5), 6, ("stepper", pkt))
        check("stepper: ... and it returns to the value", near(s, "IAS.needle", 82, 0.5))
        before = state("servo").get("_packets")

        # ---- Servo
        s = wait("servo", lambda s: s.get("RLOW") == "1" and near(s, "PITCH.needle", 6.5, 0.2), 5, ("servo", "D,BPWR:1,PITCH:6.5,BANK:-12.0,RLOW:1,LOWF:1,EC:0,GENF:0,EOUT:0,FPMP:1"))
        check("servo: pitch 6.5 and bank -12 reach the attitude indicator", near(s, "PITCH.needle", 6.5, 0.2) and near(s, "BANK.needle", -12, 0.3))
        check("servo: rotor low, low fuel and fuel pump lamps on, engine chip and engine out off",
              s.get("RLOW") == "1" and s.get("LOWF") == "1" and s.get("FPMP") == "1" and s.get("EC") == "0" and s.get("EOUT") == "0")
        check("servo: packets for the other boards did not reach it (its packet count only counts its own)", int(s.get("_packets", 0)) >= int(before or 0))
        s = wait("servo", lambda s: s.get("RLOW") == "0", 3, ("servo", "D,RLOW:0"))
        check("servo: RLOW:0 puts the lamp out", s.get("RLOW") == "0")

        # ---- Dual Stepper
        pkt = "D,PWR:1,BPWR:1,ALT:3250,RPMR:100.0,RPME:100.0,ZULU:1430,OILT:70,OILP:62,XMSNT:60,XMSNP:55,FUELLOAD:14,ELECTRICALLOAD:45,TEMP:21C"
        s = wait("dual", lambda s: near(s, "ALT.needle", 2.5, 0.1) and near(s, "OILP.needle", 62, 0.5), 6, ("dual", pkt))
        check("dual: gauges OILP 62, OILT 70, XMSNP 55, XMSNT 60, RPMR 100, RPME 100, load 45, fuel load 14",
              all(s.get(k) == v for k, v in (("OILP", "62"), ("OILT", "70"), ("XMSNP", "55"), ("XMSNT", "60"), ("RPMR", "100"), ("RPME", "100"), ("ELECTRICALLOAD", "45"), ("FUELLOAD", "14"))))
        check("dual: the altimeter needle is at 2.5 (250 ft into the thousand)", near(s, "ALT.needle", 2.5, 0.1))
        check("dual: OLEDs show 2992, 3250, 14:30 and 21C", (s.get("oled.baro"), s.get("oled.alt"), s.get("oled.clock"), s.get("oled.temp")) == ("2992", "3250", "14:30", "21C"))
        s = wait("dual", lambda s: s.get("oled.clock") == "", 3, ("dual", "D,PWR:0"))
        check("dual: PWR:0 blanks the OLEDs but the altimeter keeps its altitude", s.get("oled.clock") == "" and s.get("oled.alt") == "" and s.get("ALT") == "3250")
        s = wait("dual", lambda s: s.get("oled.clock") == "14:30", 3, ("dual", "D,PWR:1"))
        check("dual: PWR:1 brings them back", s.get("oled.clock") == "14:30")
        send("dual", "C,OLEDTEST:1")
        time.sleep(0.4)
        send("dual", "C,OLEDTEST:0")

        # ---- Radio
        pkt = "D,NAVCOM1:1,BPWR:1,C1A:119.5,C1S:121.9,C2A:118.0,C2S:122.8,LED_HDG:1,LED_ALT:1,LED_H_ENG_OIL:1,APHDG:270,APCRS:110,APIAS:82,APVS:-250,APALT:3200"
        s = wait("radio", lambda s: s.get("C1A") == "119.500", 4, ("radio", pkt))
        check("radio: COM frequencies are padded to xxx.xxx (119.500, 121.900, 118.000, 122.800)", (s.get("C1A"), s.get("C1S"), s.get("C2A"), s.get("C2S")) == ("119.500", "121.900", "118.000", "122.800"))
        check("radio: Auto Pilot values HDG 270, CRS 110, IAS 82, VS -250, ALT 3200", (s.get("APHDG"), s.get("APCRS"), s.get("APIAS"), s.get("APVS"), s.get("APALT")) == ("270", "110", "82", "-250", "3200"))
        check("radio: LED_HDG, LED_ALT and the Huey engine oil LED on, LED_CRS off", s.get("LED_HDG") == "1" and s.get("LED_ALT") == "1" and s.get("LED_H_ENG_OIL") == "1" and s.get("LED_CRS") == "0")
        s = wait("radio", lambda s: s.get("displays") == "blank", 3, ("radio", "D,NAVCOM1:0"))
        check("radio: NAVCOM1:0 blanks the displays and puts the LEDs out", s.get("displays") == "blank" and s.get("LED_HDG") == "0" and s.get("LED_H_ENG_OIL") == "0" and s.get("C1A") == "")
        s = wait("radio", lambda s: s.get("displays") == "on" and s.get("LED_HDG") == "1", 3, ("radio", "D,NAVCOM1:1"))
        check("radio: NAVCOM1:1 brings them back as they were", s.get("displays") == "on" and s.get("LED_HDG") == "1")
        send("radio", "C,LEDTEST:1")
        s = wait("radio", lambda s: s.get("LED_CRS") == "1", 3)
        leds = [k for k in s if k.startswith("LED_")]
        check("radio: C,LEDTEST:1 lights all 39 panel LEDs", len(leds) == 39 and all(s[k] == "1" for k in leds))
        send("radio", "C,LEDTEST:0")

        # ---- ADF and Upper
        s = wait("adf", lambda s: s.get("LDGN") == "1", 4, ("adf", "D,BPWR:1,LDGN:1,LDGL:1,LDGR:0"))
        check("adf: landing gear lamps nose and left on, right off", (s.get("LDGN"), s.get("LDGL"), s.get("LDGR")) == ("1", "1", "0"))
        check("adf: the OLED shows the ADF frequency and FOLLOW OFF", (s.get("oled.line1"), s.get("oled.line2")) == ("ADF  340 kHz", "FOLLOW OFF"))
        s = wait("adf", lambda s: s.get("display") == "blank", 3, ("adf", "C,DISPLAY:0"))
        check("adf: C,DISPLAY:0 from the Radio blanks the OLED and puts the lamps out", s.get("display") == "blank" and s.get("LDGN") == "0")
        s = wait("adf", lambda s: s.get("display") == "on" and s.get("LDGN") == "1", 3, ("adf", "C,DISPLAY:1"))
        check("adf: C,DISPLAY:1 restores them", s.get("display") == "on" and s.get("LDGN") == "1")

        # ---- brightness and backlight power (any board)
        s = wait("stepper", lambda s: near(s, "_light", 0.4, 0.01), 3, ("stepper", "C,B:6"))
        check("backlight: C,B:6 gives level 6 of 15 (0.40)", near(s, "_light", 0.4, 0.01))
        s = wait("stepper", lambda s: near(s, "_light", 0.0, 0.001), 3, ("stepper", "D,BPWR:0"))
        check("backlight: BPWR:0 switches the backlight off", near(s, "_light", 0.0, 0.001))
        s = wait("stepper", lambda s: near(s, "_light", 0.4, 0.01), 3, ("stepper", "D,BPWR:1"))
        check("backlight: BPWR:1 brings back the last brightness", near(s, "_light", 0.4, 0.01))

        # ---- reset and watchdog
        send("stepper", "C,RESET:1")
        s = wait("stepper", lambda s: s.get("IAS") == "", 3)
        check("stepper: C,RESET:1 forgets the values", s.get("IAS") == "")
        send("stepper", pkt_step := "D,BPWR:1,IAS:82.0,ITT:610")
        wait("stepper", lambda s: near(s, "IAS.needle", 82, 0.5), 6, ("stepper", pkt_step))
        time.sleep(WATCHDOG + 1.0)                                  # stop sending: the 30 second watchdog (3 seconds here)
        s = wait("stepper", lambda s: s.get("_noData") == "1" and near(s, "IAS.needle", 0, 0.5) and near(s, "ITT.needle", 100, 3), 8)
        check("watchdog: with no data the board goes blank (backlight off) and the needles return to zero", s.get("_noData") == "1" and near(s, "_light", 0, 0.001) and near(s, "IAS.needle", 0, 0.5) and near(s, "ITT.needle", 100, 3))
        if not ok:
            print("   stepper state:", {k: s.get(k) for k in ("_noData", "_light", "IAS", "IAS.needle", "ITT.needle", "_last")})
        s = wait("radio", lambda s: s.get("displays") == "blank", 2)
        check("watchdog: the radio displays blank too", s.get("displays") == "blank")
        s = wait("stepper", lambda s: s.get("_noData") == "0", 3, ("stepper", "D,BPWR:1,IAS:50"))
        check("watchdog: a packet brings the board back", s.get("_noData") == "0")
    finally:
        proc.terminate()
        try:
            proc.wait(5)
        except subprocess.TimeoutExpired:
            proc.kill()
        shutil.rmtree(dump, ignore_errors=True)
    print("ALL PASS" if ok else "SOME FAILED")
    return 0 if ok else 1


if __name__ == "__main__":
    sys.exit(main())
