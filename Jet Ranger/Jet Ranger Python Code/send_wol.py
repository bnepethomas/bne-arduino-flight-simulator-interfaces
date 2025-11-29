#!/usr/bin/env python3
"""
send_wol.py — send a Wake-on-LAN magic packet

Usage:
    python send_wol.py AA:BB:CC:DD:EE:FF
    python send_wol.py AA-BB-CC-DD-EE-FF --ip 192.168.1.255 --port 9 --repeat 5
"""

import argparse
import socket
import sys
import re

def normalize_mac(mac: str) -> bytes:
    """Return 6-byte MAC from common string formats or raise ValueError."""
    # Remove common separators and lower
    cleaned = re.sub(r'[^0-9A-Fa-f]', '', mac)
    if len(cleaned) != 12:
        raise ValueError(f"MAC '{mac}' is not valid (expected 12 hex digits after removing separators).")
    try:
        return bytes.fromhex(cleaned)
    except ValueError:
        raise ValueError(f"MAC '{mac}' contains invalid hex digits.")

def make_magic_packet(mac_bytes: bytes) -> bytes:
    """Construct the 6 x 0xFF header + 16 repetitions of MAC."""
    return b'\xff' * 6 + mac_bytes * 16

def send_magic_packet(mac: str, ip: str = '255.255.255.255', port: int = 9, repeat: int = 3, timeout: float = 1.0):
    """
    Send a Wake-on-LAN magic packet.

    mac: MAC address string (AA:BB:CC:DD:EE:FF or similar)
    ip: broadcast IP to send to (default broadcast 255.255.255.255).
        You can also use a subnet-directed broadcast like 192.168.1.255.
    port: UDP port (commonly 7 or 9).
    repeat: number of times to send the packet (useful for reliability).
    timeout: socket timeout in seconds (not strictly necessary; used for cleanup).
    """
    mac_bytes = normalize_mac(mac)
    packet = make_magic_packet(mac_bytes)

    # UDP broadcast socket
    with socket.socket(socket.AF_INET, socket.SOCK_DGRAM) as sock:
        sock.setsockopt(socket.SOL_SOCKET, socket.SO_BROADCAST, 1)
        sock.settimeout(timeout)
        for i in range(max(1, int(repeat))):
            try:
                sock.sendto(packet, (ip, int(port)))
            except Exception as e:
                raise RuntimeError(f"Failed to send magic packet (attempt {i+1}): {e}")

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description="Send a Wake-on-LAN magic packet.")
    parser.add_argument('mac', help='Target MAC address (e.g. AA:BB:CC:DD:EE:FF)')
    parser.add_argument('--ip', default='255.255.255.255', help='Broadcast IP (default 255.255.255.255)')
    parser.add_argument('--port', type=int, default=9, help='UDP port (default 9; some use 7)')
    parser.add_argument('--repeat', type=int, default=3, help='Number of times to send the packet (default 3)')
    args = parser.parse_args()

    try:
        send_magic_packet(args.mac, ip=args.ip, port=args.port, repeat=args.repeat)
        print(f"Magic packet sent to {args.mac} via {args.ip}:{args.port} (x{args.repeat})")
    except Exception as exc:
        print("Error:", exc, file=sys.stderr)
        sys.exit(1)
