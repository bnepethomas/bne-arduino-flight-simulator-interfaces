#!/usr/bin/env python
      
# XPlane Controller
# Sends Control Packets over UDP to Xplane
# XPlane by default  listens on 49000

# Very useful reference http://www.nuclearprojects.com/xplane/



import binascii
import time
import codecs
import string
import logging
import os
import socket
import sys
import time
import threading

import struct 
from collections import namedtuple

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



# Global Variables
debugging = True
config_file = 'Control_XPlane_config.py'
secondsBetweenKeepAlives = 5

# Initialise keepalive indicator
last_time_display = time.time()
packets_processed = 0

# Address and Port to send traffic to
UDP_IP_Address = "192.168.1.138"
UDP_Port = 49000


try:

    if not (os.path.isfile(config_file)):
        
        logging.info('Unable to find ' + config_file)

        
    else:
        try:
            from config_file import *
            
                
        except Exception as other:
            logging.critical("Error in Initialisation: " + str(other))

            print('Unable to open "' + config_file + '"' )
            print('Or variable assignment incorrect - forgot quotes for string?')
            print('Defaults used')


#    # The timeout is aggressive
#    serverSock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
#    serverSock.settimeout(0.0001)
#    serverSock.bind((UDP_IP_Address, UDP_Port))
#
#    wireshark_Sock = socket.socket(socket.AF_INET, # Internet
#        socket.SOCK_DGRAM) # UDP




    # See if value is assigned.  First we checked config file and then
    #   command line arguments

    logging.debug("Checking Command Line parameters")


    parser = OptionParser()

    (options, args) = parser.parse_args()


    
    logging.debug("options:" + str(options))
    logging.debug("arguments:" +  str(args))

   

except KeyboardInterrupt:
    CleanUpAndExit()

except Exception as other:
    logging.critical('Error in Setup: ' + str(other))


def SendTestPacket():
    # XPlane expects the 5 byte header ('DATA' + Reserved Char (which can be 0)
    # Then is 4 byes for the Index number eg Decimal 11 for flight controls
    # And then 32 bytes which the series of eight 4 byte numbers
    # If you want to see what values are associated with a given index go to
    #    Settings -> Data Output -> General Data Output, and Select Show in Cockpit for the Index of Interest
    
    # For Values you don't want to change set them to -999 which maps to 192, 121, 196, 0 (this isn't an IP Address!)
    
    # From sample c# code @ http://www.nuclearprojects.com/xplane/senddata.shtml
    # --- 11: Flight Controls ---
    # Pitch: 0.20
    # byte[] XFData = { 68, 65, 84, 65, 0, 11, 0, 0, 0, 205, 204, 76, 62, 0, 192, 121, 196, 0, 192, 121, 196, 0, 192, 121, 196, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    # Pitch: 0.00
    # byte[] XFData = { 68, 65, 84, 65, 0, 11, 0, 0, 0, 0, 0, 0, 0, 0, 192, 121, 196, 0, 192, 121, 196, 0, 192, 121, 196, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };


    try:

        # Currently XPlane is receiving the data but not acting on it
        # Of interest if the byte after data is 
        #   1 - reported at a Multiplayer machine flying Boeing 737-800
        #   * - reported as an X-Plane machine sending us its data output
        
        values = ('DATA'.encode('utf-8'), 0, 11, -0.20, -999, -999, -999, -999, -999, -999, -999)
        packer = struct.Struct('5s B B 8f')

        packed_data = packer.pack(*values)

        
        print('UDP target IP:', UDP_IP_Address)
        print('UDP target port:', UDP_Port)
        print('sending "%s"' % binascii.hexlify(packed_data), values)

        sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM) # UDP
        sock.sendto((packed_data), (UDP_IP_Address, UDP_Port))
    
    except Exception as other:
        logging.critical('Error in SendTestPacket: ' + str(other))
   

 


def ProcessXPlaneString(ReceivedUDPBytes):
    # Reference http://www.nuclearprojects.com/xplane/
    # The first 5 bytes are header - whcih includes DATA and then an internal use only byte
    # Following that is the message - 36 bytes - the first 4 bytes indicate the index number of the data element
    #    the last 32 bytes is the data, with up to 8 single precision floating point numbers
    #    which maps to 4 bytes per value
    
    # Index 03 (x03) - Speeds
    # Index 20 (x14) - Lat Long Altitude
    #
    # For this setup the data length is 77 bytes (206-282 in UDP packet inclusive)
    # Observed - single dataset 41 bytes, two data sets 77 bytes, three datasets 113
    # So payload is 36 bytes per datset, with a 5 byte header (Xplane 11)
    
    # Now what do the individual datsets maps to
    
    # Speeds - If shown in cockpit has Vindicated (knots as first value eg 216.61
    
    
    global outAltitude
 #   global outUTC 
 #   global outDate 
    global xoutputstr 
    global outNorS 
 
    global youtputstr 
    global outEorW 
    global outSpeed 
    global outTrackMadeGood
    
    # Expecting a packet length of 77 bytes
    UDP_Header_Length = 5      # 'DATA + 1 reserved byte
    Record_Length = 36         # Record_Header of 4 bytes + 8 records of 4 bytes
    
    Number_of_Records = 3
    Expected_Packet_Length = UDP_Header_Length + (Record_Length * Number_of_Records)
    
    try:
    
        if (len(ReceivedUDPBytes) != Expected_Packet_Length):
            logging.critical("Received Packet Length of " + str(len(ReceivedUDPBytes)) + "  - expected " + str(Expected_Packet_Length))
            
#        print("hello xplane - " + str(len(ReceivedUDPBytes)))
#        print(ReceivedUDPBytes)
#        dh = codecs.encode(ReceivedUDPBytes, 'hex')
#        print(dh)
        
        # Remove the 5 Byte UDP Header
        NewReceivedUDPBytes = ReceivedUDPBytes[5:]
        
#        print("hello xplane short- " + str(len(NewReceivedUDPBytes)))
#        dh = codecs.encode(NewReceivedUDPBytes, 'hex')
#        print(dh)
        
        # First value is vind
        #print(unpack('i8fi8f',NewReceivedUDPBytes))

        
        XPlaneRecord = namedtuple('XPlaneRecord','Idx1 kias Rec2 Rec3 Rec4 Rec5 Rec6 Rec7 Rec8 \
                                  Idx2 RecA RecB heading RecD RedE RecF RecG RecH \
                                  Idx3 lat long alt Rec14 Rec15 Rec16 Rec17 Rec18')
        
        logging.debug('Starting unpack to values')
        XPlaneStatus = XPlaneRecord._make(unpack('i8fi8fi8f',NewReceivedUDPBytes))
        
     
        if (XPlaneStatus.Idx1 == 3):
            # Have Correct Index, so assign AirSpeed
            # print('Xplane Airspeed is : ' + str(XPlaneStatus.kias))
            outSpeed = "{:.1f}".format(XPlaneStatus.kias)
            
        if (XPlaneStatus.Idx2 == 17):
            # print('XPlane Heading is : ' + str(XPlaneStatus.heading))
            outTrackMadeGood = "{:.2f}".format(XPlaneStatus.heading)            
            
        if (XPlaneStatus.Idx3 == 20):
            # Have Correct Index, so assign Altitude - but it is returned in feet so convert to meters
            # print('Xplane Altitude is : ' + str(XPlaneStatus.alt))
            wrkfloat = float(XPlaneStatus.alt) * 0.3048
            outAltitude = "{:.1f}".format(wrkfloat)
            print('XPlane Lat is : ' + str(XPlaneStatus.lat))
            print('XPlane Long is : ' + str(XPlaneStatus.long))


            if (XPlaneStatus.lat>0):
                outNorS = 'N'
            else:
                outNorS = 'S'
                
            if (XPlaneStatus.long>0):
                outEorW = 'E'
            else:
                outEorW = 'W'     

            # As Sim returns negative values for Southern Hemisphere - remove negative
            wrkfloat = abs(XPlaneStatus.lat)
            latDegrees = int(wrkfloat)
            wrkfloat = wrkfloat - latDegrees
            latMinutes = int(wrkfloat * 60)
            
            
            print( "workfloat : " + str(wrkfloat))
            
            # Dealing with the Seconds issue
            #      On P3d display is reports S27 deg 23.98
            #      It is passed through SimConnect as as decimal -27.39960
            #      But GPS whats it as slightly different decimal ie 2723.98 (which maps to 27.23.58.8
            #      the decimal peice looks like 0.01627, which needs to map to 98
            #      50 maps to 30.0"
            
            latSeconds = (wrkfloat - (latMinutes/60)) 
            print( "lat seconds: " + str(latSeconds))
            latSeconds = int(latSeconds * 6000)
            print( "lat seconds processed: " + str(latSeconds))
            #latSeconds = 99
            
            xoutputstr= "{:02}".format(latDegrees) +  "{:02}".format(latMinutes)  + "." +  "{:02}".format(latSeconds)


                      

            wrkfloat = abs(XPlaneStatus.long)                         
            longDegrees = int(wrkfloat)
            wrkfloat = wrkfloat - longDegrees
            longMinutes = int(wrkfloat * 60)

            longSeconds = (wrkfloat - (longMinutes/60)) 
            print( "long seconds: " + str(longSeconds))
            longSeconds = int(longSeconds * 6000)
            print( "long seconds processed: " + str(longSeconds))
            
                              
            
            youtputstr= "{:03}".format(longDegrees) +  "{:02}".format(longMinutes) + '.' + "{:02}".format(longSeconds)
    
        # Timeout in dara from Flight Sim - locate GPS in Brisbane
        outUTC = '160533.00'
        outDate = "010418"
        #xoutputstr = '2724.00'
        #outNorS = 'S'
        #youtputstr = '15307.000'
        #outEorW = 'E'
        #outSpeed = '299'
        #outTrackMadeGood = '0'
        outMagVar = '0'
        #outMagEorW = 'E'
        #outAltitude = '10'                                        
        Send_GPRMC()
        Send_GPGGA()
        Send_GPGSA()
        
    except Exception as other:
        logging.critical('Error in ProcessXPlaneString: ' + str(other))

def Main():

    print('Sending to ' + UDP_IP_Address + ' onport ' + str(UDP_Port))
    try:
        SendTestPacket()
        print('Hello World')

    except KeyboardInterrupt:
        # Catch Ctl-C and quit
        CleanUpAndExit()
        
    except Exception as other:
        logging.critical('Error in Main: ' + str(other))
 
 
 
Main()        
