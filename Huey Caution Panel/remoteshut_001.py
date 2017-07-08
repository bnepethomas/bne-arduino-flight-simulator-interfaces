#!/usr/bin/python

# Simple and unsecure remote shutdown
# https://github.com/bnepethomas/bne-arduino-flight-simulator-interfaces

# git commit -a
# git push


import argparse
import numbers
import os
import re
import socket
import sys
import time

from random import randint


# 0.0.0.0 will listen on all ports, other specify desired source address
UDP_IP = "0.0.0.0"
UDP_PORT = 7785


 


sock = socket.socket(socket.AF_INET, # Internet
                     socket.SOCK_DGRAM) # UDP
sock.bind((UDP_IP, UDP_PORT))



while True:
    try:
       while True:
          sock.settimeout(2)
          data, addr = sock.recvfrom(1024) # buffer size is 1024 bytes
          #print "received message:", data
          words = data.split(":")
          #print words

          for current_word in words:
              print(current_word)
              #print(len(current_word))

              # Basic sanity check to catch values that are too short
              if len(current_word) >= 3:
                  values = current_word.split("=")
                  print values
                  print values[0] + "-" + values[1]

                  if values[0] == '91':
                        print "Handling 91-lamp_ENGINE_OIL_PRESS"
                        if values[1] == "1":
                            print "Received a one shutting down"
                            os.system("shutdown now -h")
                        else:
                            print "Received a zero"


            


    except socket.timeout:
	print("timeout - still waiting for shutdown")
        continue

    except KeyboardInterrupt:
        # Catch Ctl-C and quit
        sys.exit(0)

    except:
        print("Unexpected error:", sys.exc_info() [0])
        continue
