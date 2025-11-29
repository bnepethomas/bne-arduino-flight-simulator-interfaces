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

from struct import *
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
config_file = 'pyHWLink_Serial_config.py'
secondsBetweenKeepAlives = 5

# Initialise keepalive indicator
last_time_display = time.time()
packets_processed = 0

# Address and Port to listen on
UDP_IP_Address = ""
UDP_Port = 13136

# Last time a packet was received - used to just a default location to GPS to keep it alive
last_time_packet_received = datetime.datetime.now()


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


    # The timeout is aggressive
    serverSock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    serverSock.settimeout(0.0001)
    serverSock.bind((UDP_IP_Address, UDP_Port))

    wireshark_Sock = socket.socket(socket.AF_INET, # Internet
        socket.SOCK_DGRAM) # UDP




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




def ReceivePacket():

    
    global last_time_display
    global packets_processed
    global last_time_packet_received
    
    
    global outUTC 
    global outDate 
    global xoutputstr 
    global outNorS 
    global youtputstr 
    global outEorW 
    global outSpeed 
    global outTrackMadeGood
    global outMagVar
    global outMagEorW 
    global outAltitude 
    iterations_Since_Last_Packet = 0

    SendingSim = 'SimConnect'
    while True:
        
        try:

            data, (Source_IP, Source_Port)  = serverSock.recvfrom(1500)
            
            logging.debug("Packet Received " + str(Source_Port))

            # Need to decode the payload to convert from bytes object to a string
            # Don't do this is the packet is from XPlane Sourced from 49001
            if (Source_Port != 49001):
                # SimConnect P3d
                SendingSim = 'SimConnect'
                ReceivedPacket = data.decode()
                ReceivedPacket = str(ReceivedPacket)
            else:
                SendingSim = 'XPlane'
                ReceivedPacket = data
                
            packets_processed = packets_processed + 1

            Source_IP = str(Source_IP)
            Source_Port = str(Source_Port)
            #ReceivedPacket = str(ReceivedPacket)
            
            logging.debug("From: " + Source_IP + " " + Source_Port)
            logging.debug("Message: " + str(ReceivedPacket))

            if (SendingSim == 'SimConnect'):             
                ProcessReceivedString( str(ReceivedPacket), Source_IP , str(Source_Port) )
            elif (SendingSim == 'XPlane'):
                ProcessXPlaneString(ReceivedPacket)
            else:
                logging.critical("Unknown Sim")
                
            
            logging.debug("Iterations since last packet " + str(iterations_Since_Last_Packet))
            
            iterations_Since_Last_Packet=0
            last_time_packet_received = datetime.now()
            

                                              
        except socket.timeout:
            iterations_Since_Last_Packet = iterations_Since_Last_Packet +  1

            timesincelastpacket = datetime.datetime.now() - last_time_packet_received
            
            if (timesincelastpacket.microseconds > 500000):
                print("[i] Mid Receive Timeout - " + time.asctime())
                last_time_packet_received = datetime.datetime.now()
                                                
                # Timeout in dara from Flight Sim - locate GPS in Brisbane
                outUTC = '160533.00'
                outDate = "010418"
                xoutputstr = '2725.00'
                outNorS = 'S'
                youtputstr = '15306.000'
                outEorW = 'E'
                outSpeed = '299'
                outTrackMadeGood = '10'
                outMagVar = '0'
                outMagEorW = 'E'
                outAltitude = '10'                                        
                Send_GPRMC()
                Send_GPGGA()
                Send_GPGSA()
                iterations_Since_Last_Packet=0
                
            # Throw something on console to show we haven't died
            if time.time() - last_time_display > secondsBetweenKeepAlives:
                
                #Calculate Packets per Second
                if packets_processed != 0:
                    packets_per_Second = packets_processed / secondsBetweenKeepAlives
                else:
                    packets_per_Second = 0
                
                pps_string = ". " + str(packets_per_Second) + " packets per second."
                
                logging.info('Keepalive check ' + str(packets_processed)
                             + ' Packets Processed' + pps_string)
                
                last_time_display = time.time()
                packets_processed = 0

                
            continue

        except Exception as other:
            logging.critical('Error in ReceivePacket: ' + str(other))
            

        if time.time() - last_time_display > 5:
            print('Keepalive ' + time.asctime())
            last_time_display = time.time()

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
        logging.critical('Error in ProcessReceivedString. Error is: ' + str(other))   

def ProcessReceivedString(ReceivedUDPString, Source_IP, Source_Port):

    logging.debug('Processing UDP String')

    global wireshark_Sock, wireshark_IP_Address, wireshark_Port

    
    try:
        if len(ReceivedUDPString) > 0:
            

            logging.debug("From: " + Source_IP + " " + Source_Port)         
            logging.critical('Payload: ' + ReceivedUDPString)
            
            print('parsing')
            
            ParsePayload(ReceivedUDPString)
            
            print ('back from parsing')
            
#            outUTC = '160533.00'
#            outDate = "010418"
#            xoutputstr = '2724.00'
#            outNorS = 'S'
#            youtputstr = '15307.000'
#            outEorW = 'E'
#            outSpeed = '298'
#            outTrackMadeGood = '0'
#            outMagVar = '0'
#            outMagEorW = 'E'
#            outAltitude = '10.0'   
            
            Send_GPRMC()
            Send_GPGGA()
            Send_GPGSA()
            
            

    except Exception as other:
        logging.critical('Error in ProcessReceivedString. Error is: ' + str(other))          
            

def ParsePayload(Payload):

    global outAltitude
 #   global outUTC 
 #   global outDate 
    global xoutputstr 
    global outNorS 
 
    global youtputstr 
    global outEorW 
    global outSpeed 
    global outTrackMadeGood
 #   global outMagVar
 #   global outMagEorW
 
 
 
    global latDegrees
    global latMinutes 
    global latSeconds 

    global longDegrees
    global longMinutes 
    global longSeconds


    logging.info('Payload: ' + Payload)

    
    logging.basicConfig(level=logging.DEBUG)
    
    logging.debug('Processing Payload')

    send_string = ""
    target = {}
    payloadOk = False
    

    
    try:
        if len(Payload) > 0:

            
            logging.debug('Stage 1 Processing: ' + str(Payload))
            logging.debug('Checking for correct format :')

            

            workingSets =''
            workingSets = Payload.split(',')
            logging.debug('There are ' + str(len(workingSets)) + ' records')
            counter = 0
            for workingRecords in workingSets:


                debugging = True
                
                logging.debug('Record workingRecord number ' + str(counter) + ' ' +
                      workingRecords)
                counter = counter + 1
                

                workingFields = ''
                workingFields = workingRecords.split(':')

                
                if len(workingFields) != 2:
                    print('')
                    print('WARNING - There are an incorrect number of fields in: ' + str(workingFields))
                    print('')
                  
                else:
                    logging.debug('Stage 2 Processing: ' + str(workingFields))

                    try:
                        workingkey = workingFields[0]
                        logging.debug('Working key is: ' + workingkey)
                        
                         
                        if (workingkey=='altitude'):
                            # As NEMA works in Meters ensure data request from P3d is in Meters not feet
                            wrkfloat = float(workingFields[1])
                            outAltitude = "{:.0f}".format(wrkfloat)
                            
                        if (workingkey=='magheading'):
                            wrkfloat = float(workingFields[1])
                            outTrackMadeGood = "{:.2f}".format(wrkfloat)
                            
                        if (workingkey=='airspeed'):
                            # Note if Ground speed is 0 then GPS will display a Northery heading as not display altitude alterts
                            wrkfloat = float(workingFields[1])
                            outSpeed = "{:.1f}".format(wrkfloat)
                            
                        if (workingkey=='latitude'):
                            print( workingFields[1])
                            wrkfloat = float(workingFields[1])
                            if (wrkfloat>0):
                                outNorS = 'N'
                            else:
                                outNorS = 'S'
                             
                            
                            # As Sim returns negative values for Southern Hemisphere - remove negative
                            wrkfloat = abs(wrkfloat)
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
                            #print( "lat seconds: " + str(latSeconds))
                            latSeconds = int(latSeconds * 6000)
                            #print( "lat seconds processed: " + str(latSeconds))
                            #latSeconds = 99
                            
                            xoutputstr=  "{:02}".format(latDegrees) +  "{:02}".format(latMinutes)  + "." +  "{:02}".format(latSeconds)
                            #print(xoutputstr)
                            
                                
                            
                        if (workingkey=='longitude'):
                            
                            wrkfloat = float(workingFields[1])
                            if (wrkfloat>0):
                                outEorW = 'E'
                            else:
                                outEorW = 'W'                         
 
                            wrkfloat = abs(wrkfloat)                         
                            longDegrees = int(wrkfloat)
                            wrkfloat = wrkfloat - longDegrees
                            longMinutes = int(wrkfloat * 60)

                            longSeconds = (wrkfloat - (longMinutes/60)) 
                            print( "long seconds: " + str(longSeconds))
                            longSeconds = int(longSeconds * 6000)
                            print( "long seconds processed: " + str(longSeconds))
                            
                                              
                            
                            youtputstr= "{:03}".format(longDegrees) +  "{:02}".format(longMinutes) + '.' + "{:02}".format(longSeconds)
                            print("Here" + youtputstr)
          
                         
                        
    
                    except Exception as other:
                        print('')
                        print('WARNING - Unable to process record of interest in ProcessPayload')
                        print('WARNING - Record name is: "' + workingkey + '"')
                        print('WARNING - Error is: "' + str(other) + '"')    
                        print('')
                        logging.critical("Error in ProcessPayload: " + str(other))
                


            logging.debug('Continuing on')
            print('finihsed parsing')


    except Exception as other:
        
        logging.critical("Error in ProcessPayload: " + str(other))


    
    

localDebugging = False

ser = serial.Serial(

    port='/dev/ttyS0',
    baudrate = 115200,
    parity=serial.PARITY_NONE,
    stopbits=serial.STOPBITS_ONE,
    bytesize=serial.EIGHTBITS,
    timeout=1
)
counter=0

def CalcChecksum(strToCalc):
    if localDebugging:
        print(strToCalc)
    packet = strToCalc.encode() 
    checksum = 0
    for el in packet:
        checksum ^= el

    #print (checksum, hex(checksum), chr(checksum))
    #Need to convert integer/byte into 2 characters
    mychecksum = hex(checksum)
    # Remove leading '0x'
    mychecksum = mychecksum[2:4]
    # NEMA Senstences like Caps - so ensure it is capitalised
    mychecksum = mychecksum.upper()
    if localDebugging:
        print ('Checksum is ' +  mychecksum)
    
    return(mychecksum)
    #ser.write(str.encode('$GPRMC,160533.00,A,1002.3552,N,01045.51552,E,400,0,010413,0,E*73\r\n'))

def Send_GPRMC():

    localDebugging = True

    outStartOfString = 'GPRMC'
    outStatus = 'A'
    outEndOfString = '*'

    # Assemble string
    outcompletestring = outStartOfString + "," + outUTC + ","
    outcompletestring = outcompletestring + outStatus + ","
    outcompletestring = outcompletestring + xoutputstr + "," + outNorS + ","
    outcompletestring = outcompletestring + youtputstr + "," + outEorW + ","
    outcompletestring = outcompletestring + outSpeed + ","
    outcompletestring = outcompletestring + outTrackMadeGood + ","
    outcompletestring = outcompletestring + outDate + ","
    outcompletestring = outcompletestring + outMagVar + "," + outMagEorW 

    outputstring = '$' + outcompletestring + '*' + CalcChecksum(outcompletestring) + '\r\n'
    if localDebugging:
        print(outputstring)
    ser.write(str.encode(outputstring))

def Send_GPGGA():
    localDebugging = True
    outStartOfString = "GPGGA"
    outGPSFix = "6";
    outNoofSatellites = "03"
    outPrecision = "0.00"
    outEndOfString = "*"

    outcompletestring = outStartOfString + "," + outUTC + ","
    outcompletestring = outcompletestring + xoutputstr + "," + outNorS + ","
    outcompletestring = outcompletestring + youtputstr + "," + outEorW + ","
    outcompletestring = outcompletestring + outGPSFix + ","
    outcompletestring = outcompletestring + outNoofSatellites + ","
    outcompletestring = outcompletestring + outPrecision + ","
    outcompletestring = outcompletestring + outAltitude + ",M,,,,"
    outcompletestring = outcompletestring 


    outputstring = '$' + outcompletestring + '*' + CalcChecksum(outcompletestring) + '\r\n'
    
  
    if localDebugging:
        print(outputstring)
    ser.write(str.encode(outputstring))

def Send_GPGSA():
    #  Out Satellite status
    outputstring = "GPGSA,A,3,01,02,03,,,,,,,,,,3.0,3.0,3.0"
    outputstring = '$' + outputstring + '*' +  CalcChecksum(outputstring) + '\r\n'

    if localDebugging:
        print (outputstring)
    ser.write(str.encode(outputstring))



def Test_Cycle():
    i = 0
    j = 0

    global latDegrees
    global latMinutes 
    global latSeconds 

    global xoutputval 

    global longDegrees
    global longMinutes 
    global longSeconds
    global outAltitude
  
    while ( j < 4600):
        
        youtputstr = str(longDegrees).zfill(2) + str(longMinutes).zfill(2) + '.' + str(longSeconds)
        
        time.sleep(1)
        #longMinutes = longMinutes + 1
        longDegrees = longDegrees + 1
        if (longMinutes > 59):
            longMinutes = 0
            longDegrees = longDegrees + 1
        if (longDegrees > 179):
            longDegrees = 0
                
        Send_GPRMC()
        Send_GPGGA()
        Send_GPGSA()
            

        j = j + 1
        i = 0
        while (i < 100):
            xoutputval = xoutputval + 1
            xoutputstr = str(latDegrees).zfill(2) + str(latMinutes).zfill(2) + '.' + str(latSeconds)
            
            
            Send_GPRMC()
            Send_GPGGA()
            Send_GPGSA()
            
            time.sleep(2)
            latMinutes = latMinutes + 1
            #latDegrees = latDegrees + 1
            if (latMinutes > 59):
                latMinutes = 0
                latDegrees = latDegrees + 1
            if (latDegrees > 74):
                latDegrees = 0
             
             
             
            i = i + 1
            print('Count is ' + str(i) + ' ' + xoutputstr)
            
            outAltitude = outAltitude + 10
            if (outAltitude > 15000):
                outAltitude = 1

    print('Finished')



# Reference - teststring
if False:
    workstring = "GPRMC,160533.00,A,2723.4120,S,15307.72900,E,299,0,010413,0,E"
    # Checksum should be 63
    outputstring = '$' + workstring + '*' + CalcChecksum(workstring) + '\r\n'
    print(outputstring)
    ser.write(str.encode(outputstring))

outUTC = '160533.00'
outDate = "010418"
xoutputstr = '2723.4120'
outNorS = 'S'
youtputstr = '15307.72900'
outEorW = 'E'
outSpeed = '299'
outTrackMadeGood = '0'
outMagVar = '0'
outMagEorW = 'E'
outAltitude = '1'

## Sucessfully calculated check sum by stripping leading $ and all characters post *
# To output will need to prepend $, append * - the ASCII chars of the hex checksum and then CRLF

Send_GPRMC()
Send_GPGGA()
Send_GPGSA()

latDegrees = 27
latMinutes = 23
latSeconds = 4120

xoutputval = 2723.4120

longDegrees = 153
longMinutes = 7
longSeconds = 72900




def Main():


    print('Listening on port ' + str(UDP_Port))
    try:

        ReceivePacket()

    except KeyboardInterrupt:
        # Catch Ctl-C and quit
        CleanUpAndExit()
        
    except Exception as other:
        logging.critical('Error in Main: ' + str(other))
 
 
 
Main()        
