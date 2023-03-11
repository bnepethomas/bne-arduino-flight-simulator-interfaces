#!/usr/bin/env python
      
# NEMA convertor - takes Flight Sim coordinates from a UDP stream and outputs via serial port NEMA Sentences      
 
# The Lowarance 2000 supports the following NEMA Sentences
#     GLL, RMC, RMB, GGA, GSA, GSV, APB
# Currently Exporting- RMC, GGA and GSA

#   $GPGGA - Global Positioning System Fix Data
#   $GPGSA - GPS DOP and active satellites 
#   $GPRMC - Recommended minimum specific GPS/Transit data

#   $GPRMB - Recommended minimum navigation info
#   $GPGSV - GPS Satellites in view
#   $GPAPB - Auto Pilot

# After been driven nuts trying to understand why the Lowrance GPS seemed to be displaying
    # incorrect data went to the Digital Data screen - it also mismatched the data I was sending to it.
    # discovered previous owen on has DM enabled, not DMS and Datum selection was set yo NA 1983
    # set Datum to WGS 84 and coordinate system using DMS, and now things align
# Also worthy of note is Seconds should be sent as Decmial value and the GPS will convert to Degrees
    # ie 50 beings 30

# For P3d, SimConnect is used to subscribe to a dataflow - whcih means that code must either be installed
#  on the PC running P3d or on a second PC with SimConnect installed and configured to point to primary PC
#  As the code is ligthweight the plan is to run on Primary PC

# For Xplane - it has a native UDP exporter.  In Version 11 - goto Settings,
#  Select the Values of Interest,  (eg Speeds, Lat,Long, Altitude)
#  A warning will be presented that the Network Data Output hasn't been configured
#  Enter IP Address and target port (as of 20181229 using common port 13136 - but this is likely to change)


import binascii
import time
import codecs
import serial
import string
import logging
import os
import socket
import sys
import time
import threading
import datetime


logging.basicConfig(format='%(asctime)s:%(levelname)s:%(message)s',level=logging.INFO)
#logging.basicConfig(format='%(asctime)s:%(levelname)s:%(message)s',level=logging.DEBUG)
#logging.basicConfig(format='%(asctime)s:%(levelname)s:%(message)s')



# Used for command line parsing
from optparse import OptionParser


def CleanUpAndExit():
    try:
        # Catch Ctl-C and quit
        print('')
        print('Exiting')
        print('')
        try:
            serverSock.close()
        except:
            logging.critical('Unable to close server socket')
        sys.exit(0)

    except Exception as other:
        logging.critical('Error in CleanUpAndExit: ' + str(other))
        sys.exit(0)

      
  


ser = serial.Serial(

    port='/dev/ttyS0',
    baudrate = 115200,
    parity=serial.PARITY_NONE,
    stopbits=serial.STOPBITS_ONE,
    bytesize=serial.EIGHTBITS,
    timeout=1
)
    


def Main():


    print('Sending Command to Projector')
    try:
        outputstring =  '\r*pow=off#\r'

        ser.write(str.encode(outputstring))


    except KeyboardInterrupt:
        # Catch Ctl-C and quit
        CleanUpAndExit()
        
    except Exception as other:
        logging.critical('Error in Main: ' + str(other))
 
 
 
Main()        
