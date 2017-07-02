#!/usr/bin/python

# Huey Caution Panel Hardware interface for DCS
# https://github.com/bnepethomas/bne-arduino-flight-simulator-interfaces

# git commit -a
# git push

# Need to enable SPI as it is not enabled by default on Pi

# Board Pin Name    Remarks 	RPi Pin RPi Function
#   1 	    VCC     +5V Power 	2 	5V0
#   2 	    GND     Ground 	6 	GND
#   3 	    DIN     Data In 	19 	GPIO 10 (MOSI)
#   4 	    CS 	    Chip Select 24 	GPIO 8 (SPI CE0)
#   5 	    CLK     Clock 	23 	GPIO 11 (SPI CLK)

# Using pins for input
# Pin   Name
# 14    Ground
# 16    GPIO23
# 18    GPIO24

# Use interrupts for switch inputs
# Need recent GPIO library using 6.3

# Pinmap
# Reference https://www.itead.cc/wiki/RPI_Screws_Prototype_Add-on_V2.0
#PI Pin N.O. 	Name    Arduino Description
#1 	3.3V 	3.3V 	
#2 	5V 	5V 	
#3 	GPIO02 	A4 	SDA
#4 	5V 	5V 	
#5 	GPIO03 	A5 	SCL
#6 	GND 	GND 	
#7 	GPIO04 	A0 	
#8 	GPIO14 	D1 	TXD
#9 	GND 	GND 	
#10 	GPIO15 	D0 	RXD
#11 	GPIO17 	D2 	
#12 	GPIO18 	D3 	
#13 	GPIO27 	D5 	
#   14 	GND 	GND 	
#15 	GPIO22 	D6 	
#   16 	GPIO23 	D7 	
#17 	3.3V 	3.3V 	
#   18 	GPIO24 	D8 	
#   19 	GPIO10 	D11 	SPI_MOSI
#20 	GND 	GND 	
#21 	GPIO09 	D12 	SPI_MISO
#22 	GPIO25 	D9 	
#   23 	GPIO11 	D13 	SPI_SCK
#   24 	GPIO08 	D10 	SPI_CE0
#25 	GND 	GND 	
#26 	GPIO07 	D4 	SPI_CE1
#27 	ID_SD 		
#28 	ID_SC 		
#29 	GPIO05 	A1 	
#30 	GND 	GND 	
#31 	GPIO06 	A2 	
#32 	GPIO12 	A3 	
#33 	GPIO13 	D14 	
#34 	GND 	GND 	
#35 	GPIO19 	D16 	
#36 	GPIO16 	D15 	
#37 	GPIO26 	D19 	
#38 	GPIO20 	D17 	
#39 	GND 	GND 	
#40 	GPIO21 	D18


# Led Mappings (x,y)
# 1,7   lamp_ENGINE_OIL_PRESS
# 1,6   lamp_ENGINE_ICING
# 1,5   lamp_ENGINE_ICE_DET
# 1,4   lamp_ENGINE_CHIP_DET
# 1,3   lamp_LEFT_FUEL_BOOST
# 1,1   lamp_RIGHT_FUEL_BOOST
# 1,2   lamp_ENG_FUEL_PUMP
# 1,0   lamp_20_MINUTE
# 3,1   lamp_FUEL_FILTER
# 3,0   lamp_GOV_EMERG
# 2,7   lamp_AUX_FUEL_LOW
# 2,6   lamp_XMSN_OIL_PRESS
# 2,5   lamp_XMSN_OIL_HOT
# 2,4   lamp_HYD_PRESSURE
# 2,3   lamp_ENGINE_INLET_AIR
# 2,2   lamp_INST_INVERTER
# 2,1   lamp_DC_GENERATOR
# 2,0   lamp_EXTERNAL_POWER
# 4,1   lamp_CHIP_DETECTOR
# 4,0   lamp_IFF



import argparse
import RPi.GPIO as GPIO
import numbers
import re
import socket
import sys
import time

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

def LedStartup():
    GPIO.setmode(GPIO.BCM)
    GPIO.setwarnings(False)
    GPIO.setup(26,GPIO.OUT)
    print "LED on"
    GPIO.output(26,GPIO.HIGH)
    time.sleep(1)
    print "LED off"
    GPIO.output(26,GPIO.LOW)


# Only used to map output numbers during configuration
def LedMap():
    print("Walking Led Array")
    for y in range(8):
        for x in range(8):              
             with canvas(device) as draw:
                print x, ' ' , y 
                draw.point((x, y ), 1)
                raw_input("Press Enter to continue:")
                    

def LedRandom():
    print "Starting Drawing Random" 

    for abc in range(1):
        with canvas(device) as draw:
            for y in range(8):
                for x in range(8):
                    draw.point((x, y ), randint(0,1))
                
        time.sleep(0.1)
                
    print "Finished Drawing Random"       


def Ledallon():
    print("Leds all on")

    for abc in range(1):
        with canvas(device) as draw:
            for y in range(8):
                for x in range(8):
                    draw.point((x, y ), 1)
                
        time.sleep(0.1)
                
    print("Finished Leds all on")   


def Ledalloff():
    print("Leds all off")

    for abc in range(1):
        with canvas(device) as draw:
            for y in range(8):
                for x in range(8):
                    draw.point((x, y ), 0)
                
        time.sleep(0.1)
                
    print("Finished Leds all off")  

def DensitySweep():
    for _ in range(4):
        for intensity in range(16):
            device.contrast(intensity * 16)
            time.sleep(0.1)

LedStartup()


# Creating Max7219 device
print "Creating Max7219 Device"
serial = spi(port=0, device=0, gpio=noop())
device = max7219(serial, cascaded=1 or 1, block_orientation=0)
print "Created Max7219 Device"

# Excerise Leds
# Random led pattern
LedRandom()
time.sleep(1)
# All on
Ledallon()
time.sleep(1)
# Density sweep
DensitySweep()
# All off
Ledalloff()
time.sleep(1)


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
                  print values
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
