#!/usr/bin/python

import socket

UDP_IP = "192.168.1.124"
UDP_PORT = 26027

MESSAGE = "C15,3003,-1"

print "UDP target IP:", UDP_IP
print "UDP target port:", UDP_PORT
print "message:", MESSAGE

sock = socket.socket(socket.AF_INET, # Internet
                     socket.SOCK_DGRAM) # UDP
sock.sendto(MESSAGE, (UDP_IP, UDP_PORT))
