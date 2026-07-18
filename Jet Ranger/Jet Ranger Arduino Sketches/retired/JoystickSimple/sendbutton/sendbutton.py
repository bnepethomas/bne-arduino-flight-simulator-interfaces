#!/usr/bin/env python3
"""
UDP Joystick Button Controller
Sends button press/release commands to an Arduino Leonardo + Ethernet Shield
running the joystick sketch.

Packet format expected by the Arduino: "idx,state\n"
  idx   = button index (0-63)
  state = 1 (press) or 0 (release)
  
python sendbutton.py 20  trigger --hold 2000  
"""

import socket
import time
import sys
import argparse

# --- Configuration ---
ARDUINO_IP = "172.16.1.11"    # Arduino's static IP
ARDUINO_PORT = 4210

# Optional: map friendly names to button indices.
# Extend this as you assign more buttons in FSUIPC.
BUTTON_MAP = {
    "PHYSICAL_BTN": 0,       # the hardwired pin 5 button (read-only from sim side, don't send this one)
    "ROTOR_BRAKE": 1,
    "ANNUNCIATOR_TEST": 2,
    "WARNING_MUTE": 3,
    "AI_CAGE": 4,
    "TOT_TEST": 5,
    "RAD_ALT_MUTE": 6,
    "RAD_ALT_TEST": 7,
    "STARTER": 8,
    "SAS_MASTER": 9,
    "SAS_HDG": 10,
    "SAS_NAV": 11,
    "SAS_SPD": 12,
    "SAS_ALT": 13,
    "SAS_VRT": 14,
    "COM1_TX_SELECT": 15,
    "COM2_TX_SELECT": 16,
    "COM_RX_ALL": 17,
    "COM1_FREQ_SWAP": 18,
    "NAV1_FREQ_SWAP": 19,
    "COM2_FREQ_SWAP": 20,
    "NAV2_FREQ_SWAP": 21,
    "XPNDR_IDT": 22,
    "CABLE_CUT": 23,
    "SAS_DISCONNECT": 24,
    # ... add more as needed, up to index 63
}


def send_packet(sock, idx: int, state: int):
    """Send a single "idx,state" UDP packet."""
    msg = f"{idx},{state}"
    sock.sendto(msg.encode(), (ARDUINO_IP, ARDUINO_PORT))
    print(f"Sent -> {msg}")


def press(sock, idx: int):
    send_packet(sock, idx, 1)


def release(sock, idx: int):
    send_packet(sock, idx, 0)


def trigger(sock, idx: int, hold_ms: int = 100):
    """Momentary press+release, for _TRIGGER-style events."""
    press(sock, idx)
    time.sleep(hold_ms / 1000.0)
    release(sock, idx)


def resolve_index(button):
    """Accept either a raw integer index or a name from BUTTON_MAP."""
    if button.isdigit():
        idx = int(button)
    else:
        key = button.upper()
        if key not in BUTTON_MAP:
            print(f"Unknown button name: {button}")
            print(f"Available names: {', '.join(BUTTON_MAP.keys())}")
            sys.exit(1)
        idx = BUTTON_MAP[key]

    if not (0 <= idx < 64):
        print(f"Button index {idx} out of range (0-63)")
        sys.exit(1)

    return idx


def main():
    global ARDUINO_IP, ARDUINO_PORT

    parser = argparse.ArgumentParser(description="Send UDP joystick button commands to Arduino")
    parser.add_argument("button", help="Button index (0-63) or name (e.g. ROTOR_BRAKE)")
    parser.add_argument(
        "action",
        choices=["press", "release", "trigger"],
        help="press = hold down, release = let go, trigger = momentary press+release",
    )
    parser.add_argument(
        "--hold", type=int, default=100,
        help="Hold time in ms for 'trigger' action (default: 100)"
    )
    parser.add_argument(
        "--ip", default=ARDUINO_IP,
        help=f"Arduino IP address (default: {ARDUINO_IP})"
    )
    parser.add_argument(
        "--port", type=int, default=ARDUINO_PORT,
        help=f"Arduino UDP port (default: {ARDUINO_PORT})"
    )

    args = parser.parse_args()

    ARDUINO_IP = args.ip
    ARDUINO_PORT = args.port

    idx = resolve_index(args.button)

    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

    if args.action == "press":
        press(sock, idx)
    elif args.action == "release":
        release(sock, idx)
    elif args.action == "trigger":
        trigger(sock, idx, args.hold)

    sock.close()


if __name__ == "__main__":
    main()