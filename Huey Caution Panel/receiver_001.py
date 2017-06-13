#!/usr/bin/python

import socket


UDP_IP = "127.0.0.1"
UDP_PORT = 5005

sock = socket.socket(socket.AF_INET, # Internet
                     socket.SOCK_DGRAM) # UDP
sock.bind((UDP_IP, UDP_PORT))

while True:
    try:
       while True:
          sock.settimeout(2)
          data, addr = sock.recvfrom(1024) # buffer size is 1024 bytes
          print "received message:", data
          words = data.split(":")
          print words
          for current_word in words:
              print(current_word)
              values = current_word.split("=")
              print values

    except socket.timeout:
	print("timeout")
        continue


