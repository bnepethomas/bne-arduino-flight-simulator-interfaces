#!/usr/bin/python

# Remote Shutdown for the Huey Hardware
# Completely UNSECURE
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
# 0,7   91  lamp_ENGINE_OIL_PRESS
# 0,6   92  lamp_ENGINE_ICING
# 0,5   93  lamp_ENGINE_ICE_DET
# 0,4   94  lamp_ENGINE_CHIP_DET
# 0,3   95  lamp_LEFT_FUEL_BOOST
# 0,1   96  lamp_RIGHT_FUEL_BOOST
# 0,2   97  lamp_ENG_FUEL_PUMP
# 0,0   98  lamp_20_MINUTE
# 2,1   99  lamp_FUEL_FILTER
# 2,0   100 lamp_GOV_EMERG
# 1,7   101 lamp_AUX_FUEL_LOW
# 1,6   102 lamp_XMSN_OIL_PRESS
# 1,5   103 lamp_XMSN_OIL_HOT
# 1,4   104 lamp_HYD_PRESSURE
# 1,3   105 lamp_ENGINE_INLET_AIR
# 1,2   106 lamp_INST_INVERTER
# 1,1   107 lamp_DC_GENERATOR
# 1,0   108 lamp_EXTERNAL_POWER
# 3,1   109 lamp_CHIP_DETECTOR
# 3,0   110 lamp_IFF



import argparse
import RPi.GPIO as GPIO
import numbers
import os
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
UDP_PORT = 7785


global Last_Led_Test_Mode

LampTestPin = 24
BrightnessPin = 23





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
    for intensity in range(16):
        device.contrast(intensity * 16)
        time.sleep(0.1)




        


# Setup inputs and outputs

Last_Led_Test_Mode = "Off"
Last_Brightness = "Bright"



# Creating Max7219 device
print "Creating Max7219 Device"
serial = spi(port=0, device=0, gpio=noop())
device = max7219(serial, cascaded=1 or 1, block_orientation=0)
print "Created Max7219 Device"


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


          with canvas(device) as draw:
            for current_word in words:
                  print(current_word)
                  #print(len(current_word))

                  # Basic sanity check to catch values that are too short
                  if len(current_word) >= 3:
                      values = current_word.split("=")
                      print values
                      print values[0] + "-" + values[1]

                      if values[0] == '999':
                            print "Handling 91-lamp_ENGINE_OIL_PRESS"
                            if values[1] == "1":
                                draw.point((0, 7 ), 1)
                                print "Received a one shutting down"
                                os.system("shutdown now -h")
                            else:
                                print "Received a zero NOT shutting down"
                                draw.point((0, 7 ), 0)
                            
                      if values[0] == '92':
                            print "Handling 92-lamp_ENGINE_ICING"
                            if values[1] == "1":
                                draw.point((0, 6 ), 1)
                            else:
                                draw.point((0, 6 ), 0) 
                      if values[0] == '93':
                            print "Handling 93-lamp_ENGINE_ICE_DET"
                            if values[1] == "1":
                                draw.point((0, 5 ), 1)
                            else:
                                draw.point((0, 5 ), 0)                             
                      if values[0] == '94':
                            print "Handling 94-lamp_ENGINE_CHIP_DET"
                            if values[1] == "1":
                                draw.point((0, 4 ), 1)
                            else:
                                draw.point((0, 4 ), 0)
                      if values[0] == '95':
                            print "Handling 95-lamp_LEFT_FUEL_BOOST"
                            if values[1] == "1":
                                draw.point((0, 3 ), 1)
                            else:
                                draw.point((0, 3 ), 0)                                
                      if values[0] == '96':
                            print "Handling 96-lamp_RIGHT_FUEL_BOOST"
                            if values[1] == "1":
                                draw.point((0, 1 ), 1)
                            else:
                                draw.point((0, 1 ), 0)                                 

                      if values[0] == '97':
                            print "Handling 97-lamp_ENG_FUEL_PUMP"
                            if values[1] == "1":
                                draw.point((0, 2 ), 1)
                            else:
                                draw.point((0, 2 ), 0)
                      if values[0] == '98':
                            print "Handling 98-lamp_20_MINUTE"
                            if values[1] == "1":
                                draw.point((0, 0 ), 1)
                            else:
                                draw.point((0, 0 ), 0)
                      if values[0] == '99':
                            print "Handling 99-lamp_FUEL_FILTER"
                            if values[1] == "1":
                                draw.point((2, 1 ), 1)
                            else:
                                draw.point((2, 1 ), 0)                                
                      if values[0] == '100':
                            print "Handling 100-lamp_GOV_EMERG"
                            if values[1] == "1":
                                draw.point((2, 0 ), 1)
                            else:
                                draw.point((2, 0 ), 0)                                
                      if values[0] == '101':
                            print "Handling 101-lamp_AUX_FUEL_LOW"
                            if values[1] == "1":
                                draw.point((1, 7 ), 1)
                            else:
                                draw.point((1, 7 ), 0)  
                      if values[0] == '102':
                            print "Handling 102-lamp_XMSN_OIL_PRESS"
                            if values[1] == "1":
                                draw.point((1, 6 ), 1)
                            else:
                                draw.point((1, 6 ), 0)
                      if values[0] == '103':
                            print "Handling 103-lamp_XMSN_OIL_HOT"
                            if values[1] == "1":
                                draw.point((1, 5 ), 1)
                            else:
                                draw.point((1, 5 ), 0)
                      if values[0] == '104':
                            print "Handling 104-lamp_HYD_PRESSURE"
                            if values[1] == "1":
                                draw.point((1, 4 ), 1)
                            else:
                                draw.point((1, 4 ), 0)
                      if values[0] == '105':
                            print "Handling 105-lamp_ENGINE_INLET_AIR"
                            if values[1] == "1":
                                draw.point((1, 3 ), 1)
                            else:
                                draw.point((1, 3 ), 0)
                      if values[0] == '106':
                            print "Handling 106-lamp_INST_INVERTER"
                            if values[1] == "1":
                                draw.point((1, 2 ), 1)
                            else:
                                draw.point((1, 2 ), 0)
                      if values[0] == '107':
                            print "Handling 107-lamp_DC_GENERATOR"
                            if values[1] == "1":
                                draw.point((1, 1 ), 1)
                            else:
                                draw.point((1, 1 ), 0)
                      if values[0] == '108':
                            print "Handling 108-lamp_EXTERNAL_POWER"
                            if values[1] == "1":
                                draw.point((1, 0 ), 1)
                            else:
                                draw.point((1, 0 ), 0)
                      if values[0] == '109':
                            print "Handling 109-lamp_CHIP_DETECTOR"
                            if values[1] == "1":
                                draw.point((3, 1 ), 1)
                            else:
                                draw.point((3, 1 ), 0)
                      if values[0] == '110':
                            print "Handling 110-lamp_IFF"
                            if values[1] == "1":
                                draw.point((3, 0 ), 1)
                            else:
                                draw.point((3, 0 ), 0)

        


    except socket.timeout:
	print("timeout in shutdown continuing to watch")
        continue

    except KeyboardInterrupt:
        sys.exit(0)

    except:
        print("Unexpected error:", sys.exc_info() [0])
        continue
