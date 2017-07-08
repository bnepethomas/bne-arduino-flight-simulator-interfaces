#!/usr/bin/python

import socket

UDP_IP = "127.0.0.1"
UDP_PORT = 7785

MESSAGE = "999=1:301=1:302=1:303=0:304=1"

print "UDP target IP:", UDP_IP
print "UDP target port:", UDP_PORT
print "message:", MESSAGE

sock = socket.socket(socket.AF_INET, # Internet
                     socket.SOCK_DGRAM) # UDP
sock.sendto(MESSAGE, (UDP_IP, UDP_PORT))
