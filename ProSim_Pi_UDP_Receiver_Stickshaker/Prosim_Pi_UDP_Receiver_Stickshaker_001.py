#!/usr/bin/python


# In addition to statically assigning IP address - will try wireless which means statically
#   assign SSID - Boeing737


# ProSim_Pi_UDP_Receiver_Stickshaker
# Generic HW Interface whivh receives UDP AV Pairs
# Based on Huey Caution Panel - pulled 20170828
# Includes insecure remote shutdown 
# https://github.com/bnepethomas/bne-arduino-flight-simulator-interfaces

# git commit -a
# git push


# Added Shutdown and reboot options
# Variable '999':
# values[1] == "ShutdownAndHalt":
# values[1] == "Reboot":


# Need to enable SPI as it is not enabled by default on Pi

# Board Pin Name    Remarks   RPi Pin RPi Function
#   1        VCC     +5V Power   2  5V0
#   2        GND     Ground   6  GND
#   3        DIN     Data In  19    GPIO 10 (MOSI)
#   4        CS       Chip Select 24   GPIO 8 (SPI CE0)
#   5        CLK     Clock    23    GPIO 11 (SPI CLK)

# Using pins for input
# Pin   Name
# 14    Ground
# 16    GPIO23
# 18    GPIO24
# 22    GPIO25

# Use interrupts for switch inputs
# Need recent GPIO library using 6.3

# Pinmap
# Reference https://www.itead.cc/wiki/RPI_Screws_Prototype_Add-on_V2.0
#PI Pin N.O.   Name    Arduino Description
#1    3.3V  3.3V  
#2    5V    5V    
#3    GPIO02   A4    SDA
#4    5V    5V    
#5    GPIO03   A5    SCL
#6    GND   GND   
#   7    GPIO04   A0    
#8    GPIO14   D1    TXD
#9    GND   GND   
#10   GPIO15   D0    RXD
#11   GPIO17   D2    
#12   GPIO18   D3    
#13   GPIO27   D5    
#   14   GND   GND   
#15   GPIO22   D6    
#   16   GPIO23   D7    
#17   3.3V  3.3V  
#   18   GPIO24   D8    
#   19   GPIO10   D11   SPI_MOSI
#20   GND   GND   
#21   GPIO09   D12   SPI_MISO
#   22   GPIO25   D9    
#   23   GPIO11   D13   SPI_SCK
#   24   GPIO08   D10   SPI_CE0
#25   GND   GND   
#26   GPIO07   D4    SPI_CE1
#27   ID_SD       
#28   ID_SC       
#29   GPIO05   A1    
#30   GND   GND   
#31   GPIO06   A2    
#32   GPIO12   A3    
#33   GPIO13   D14   
#34   GND   GND   
#35   GPIO19   D16   
#36   GPIO16   D15   
#37   GPIO26   D19   
#38   GPIO20   D17   
#39   GND   GND   
#40   GPIO21   D18






# Change History
#
# 20170828 Forked from Huey Caution Panel - Removed References for Max7219 - attempt to use
#          unused pins so both can coexist in test environment
# 20170902 Changed listen port to 9920 to match Arduino


import argparse
import RPi.GPIO as GPIO
import numbers
import os
import re
import socket
import sys
import time

from random import randint



# 0.0.0.0 will listen on all addresses, other specify desired source address
UDP_IP = "0.0.0.0"
UDP_PORT = 9920

Source_IP = 0
Source_Port = 0
Last_Source_IP = "127.0.0.1"

Debounce_Delay = 0.04


StickShakerPin = 04
SparePin = 17




def ShakerStartup():
   GPIO.setup(StickShakerPin,GPIO.OUT)
   print "Shaker on"
   GPIO.output(StickShakerPin,GPIO.LOW)
   time.sleep(2)
   print "Shaker off"
   GPIO.output(StickShakerPin,GPIO.HIGH)
   
   GPIO.setup(SparePin,GPIO.OUT)
   print "Spare on"
   GPIO.output(SparePin,GPIO.LOW)
   time.sleep(2)
   print "Spare off"
   GPIO.output(SparePin,GPIO.HIGH)

   


def Reboot():
   print "Received a Reboot - rebooting"
   time.sleep(1)
   GPIO.cleanup()       # clean up GPIO on CTRL+C exit
   os.system("shutdown now -r")
   sys.exit(0)

def ShutdownAndHalt():
   print "Received a Halt - shutting down"
   time.sleep(1)
   GPIO.cleanup()       # clean up GPIO on CTRL+C exit
   os.system("shutdown now -h")
   sys.exit(0)
   



# Setup inputs and outputs


GPIO.setmode(GPIO.BCM)
GPIO.setwarnings(False)

ShakerStartup()


sock = socket.socket(socket.AF_INET, # Internet
                socket.SOCK_DGRAM) # UDP
sock.bind((UDP_IP, UDP_PORT))

#
   
# Act on global var

while True:
   try:
      while True:
        sock.settimeout(2)
        data, (Source_IP, Source_Port) = sock.recvfrom(1024) # buffer size is 1024 bytes

        
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
              #print values[0] + "+" + values[1]

              if values[0] == 'I01':
                  #print "Stick Shaker"
                  if values[1] == "1":
                     # Inverting values are relay input is pulled low to trigger
                     GPIO.output(StickShakerPin,GPIO.LOW)
                  else:
                     GPIO.output(StickShakerPin,GPIO.HIGH)

   
              if values[0] == '999':
                  print "Handling SHUTDOWN"
                  if values[1] == "ShutdownAndHalt":
                     # Stop listening on original port
                     sock.close()
                     ShutdownAndHalt()
                  elif values[1] == "Reboot":
                     # Stop listening on original port
                     sock.close()
                     Reboot()                                      
                  else:
                     print "Received a Invalid Shutdown Request"

  


   except socket.timeout:
      print("timeout")
      continue

   except KeyboardInterrupt:
      # Catch Ctl-C and quit
      GPIO.cleanup()       # clean up GPIO on CTRL+C exit
      sys.exit(0)

   except:
      print("Unexpected error:", sys.exc_info() [0])
      continue
