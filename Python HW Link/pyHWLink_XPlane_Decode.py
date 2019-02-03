#!/usr/bin/env python

# pyHWLink_XPlane_Decode.py


# For Xplane - it has a native UDP exporter.  In Version 11 - goto Settings,
#  Select the Values of Interest,  (eg Speeds, Lat,Long, Altitude)
#  A warning will be presented that the Network Data Output hasn't been configured
#  Enter IP Address and target port (as of 20190203 using port 7793)


# In Xplane 9 - go to inet 3 page to set target port and disable/enable sending of trafic

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
UDP_Port = 7793


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

                                              
        except socket.timeout:
            iterations_Since_Last_Packet = iterations_Since_Last_Packet +  1
            if debugging == True and (iterations_Since_Last_Packet > 9000):
                print("[i] Mid Receive Timeout - " + time.asctime())
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
            
        print("hello xplane - " + str(len(ReceivedUDPBytes)))
        print(ReceivedUDPBytes)
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
