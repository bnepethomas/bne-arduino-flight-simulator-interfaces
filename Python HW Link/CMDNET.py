# Sends Commands to Fly Elise Immersive Display Pro
# Full Documentation is in Display PRO User Guide
#
# By default the CMDNET interface is displayed - ie not listening on TCP 6600

# Useful Commands
# Help
# SETCONFIGSET
#
# All commands
# HELP, GETDESKTOPWARPING, SETDESKTOPWARPING, GETWINDOWWARPING, SETWINDOWWARPING,
# LOADPROCALIB, SETCONFIGSET, SHOW, HIDE, QUIT;

import socket

HOST = "127.0.0.1"  # The server's hostname or IP address
PORT = 6600         # Default Port for Immersive Display PRO

with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
    s.connect((HOST, PORT))
    #s.sendall(b"HELP;")
    #s.sendall(b"SETCONFIGSET 2;")
    #s.sendall(b"SETCONFIGSET 1;")
    s.sendall(b"SETDESKTOPWARPING ENABLE;")
    data = s.recv(1024)

print(f"Received {data!r}")
