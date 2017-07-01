#!/usr/bin/python

import socket

import re
import time
import argparse
import numbers

from random import randint

from luma.led_matrix.device import max7219
from luma.core.interface.serial import spi, noop
from luma.core.render import canvas
from luma.core.virtual import viewport
from luma.core.legacy import text, show_message
from luma.core.legacy.font import proportional, CP437_FONT, TINY_FONT, SINCLAIR_FONT, LCD_FONT

UDP_IP = "192.168.1.109"
UDP_PORT = 7784

def checkandassign(attribname, attribvalue):
    print "checking value"
    if attribname == "300":
        print "found it 300"
    elif attribname == "301":
        print "foundit 301"
    else:
        print "no hit"
        

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
              print values[0] + "-" + values[1]
              checkandassign( values[0], values[1])
              for current_value in values:
                  if current_value == "300":
                      print "Handling 300"
                      demo(1, 0)

    except socket.timeout:
	print("timeout")
        continue


