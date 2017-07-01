#!/usr/bin/python

# Huey Caution Panel Hardware interface for DCS


# Need to enable SPI as it is not enabled by default on Pi

# Board Pin Name    Remarks 	RPi Pin RPi Function
#   1 	    VCC     +5V Power 	2 	5V0
#   2 	    GND     Ground 	6 	GND
#   3 	    DIN     Data In 	19 	GPIO 10 (MOSI)
#   4 	    CS 	    Chip Select 24 	GPIO 8 (SPI CE0)
#   5 	    CLK     Clock 	23 	GPIO 11 (SPI CLK)

# Use interrupts for switch inputs
# Need recent GPIO library using 6.3

import socket

import re
import time
import argparse
import numbers
import sys

from random import randint

from luma.led_matrix.device import max7219
from luma.core.interface.serial import spi, noop
from luma.core.render import canvas
from luma.core.virtual import viewport
from luma.core.legacy import text, show_message
from luma.core.legacy.font import proportional, CP437_FONT, TINY_FONT, SINCLAIR_FONT, LCD_FONT

# 0.0.0.0 will listen on all ports, other specify desired source address
UDP_IP = "0.0.0.0"
UDP_PORT = 7784

def checkandassign(attribname, attribvalue):
    #print "checking value"
    if attribname == "300":
        print "found it 300"
    elif attribname == "301":
        print "foundit 301"
    #else:
    #    print "no hit"
        

def demo(n, block_orientation):
    # create matrix device
    serial = spi(port=0, device=0, gpio=noop())
    device = max7219(serial, cascaded=n or 1, block_orientation=block_orientation)
    print("Created device")

            
    print("Drawing on Canvas stage 1")


    #with canvas(device) as draw:
    for abc in range(1):
        with canvas(device) as draw:
            for y in range(8):
                for x in range(8):
                    #print("Point " + str(x) + " " + str(y))
                    draw.point((x, y ), randint(0,1))
                
        time.sleep(0.1)
                
    print("Finished Drawing on Canvas stage 2")        




sock = socket.socket(socket.AF_INET, # Internet
                     socket.SOCK_DGRAM) # UDP
sock.bind((UDP_IP, UDP_PORT))

# Init Led - All on all off afer 0.5 Sec

# Get Switch state and set global vars

# Act on global var

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
                  #print values
                  #print values[0] + "-" + values[1]
                  checkandassign( values[0], values[1])
                  for current_value in values:
                      if current_value == "300":
                          print "Handling 300"
                          demo(1, 0)

    except socket.timeout:
	print("timeout")
        continue

    except KeyboardInterrupt:
        # Catch Ctl-C and quit
        sys.exit(0)

    except:
        print("Unexpected error:", sys.exc_info() [0])
        continue
