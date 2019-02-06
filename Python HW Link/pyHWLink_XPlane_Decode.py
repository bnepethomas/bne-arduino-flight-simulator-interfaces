#!/usr/bin/env python

# pyHWLink_XPlane_Decode.py


# For Xplane - it has a native UDP exporter.  In Version 11 - goto Settings,
#  Select the Values of Interest,  (eg Speeds, Lat,Long, Altitude)
#  A warning will be presented that the Network Data Output hasn't been configured
#  Enter IP Address and target port (as of 20190203 using port 7793)

# Description of fields https://www.x-plane.com/kb/data-set-output-table/
# Also stored locally X-Plane_Export_Table.odt

# In Xplane 9 - go to inet 3 page to set target port and disable/enable sending of trafic

# If adding or removing fields - need to set the of the expect packet - which is a multiplier of record length
#   Number_of_Records in Update ProcessXPlaneString and
#
#   Update this block.  Each line has a index number, and then name the records of interest, otherwise increment Rec No as a placeholder
#       XPlaneRecord = namedtuple('XPlaneRecord','Idx1 kias Rec2 Rec3 Rec4 Rec5 Rec6 Rec7 Rec8 \
#                                 Idx2 RecA RecB heading RecD RedE RecF RecG RecH \
#                                 Idx3 lat long alt Rec14 Rec15 Rec16 Rec17 Rec18')
#       
#       XPlaneStatus = XPlaneRecord._make(unpack('i8fi8fi8f',NewReceivedUDPBytes))

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



# Used for Command line parsing
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
config_file = ' pyHWLink_XPlane_Decode_config.py'
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
    
    # Update this when new fields are being sent from X-Plane
    Number_of_Records = 11

    
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
        


        # VSI Engine RPM
        # When adding new records - ensure they are added in order for this record
        # To determine what the likely values are for fields, also display values in X-Plane
        XPlaneRecord = namedtuple('XPlaneRecord', 
                                  'IdxSpeed kias Rec2 Rec3 Rec4 Rec5 Rec6 Rec7 Rec8 \
                                  IdxMachGLoad Mach Rec2a GLoad Rec4a Rec5a Rec6a Rec7a Rec8a \
                                  IdxTrimFlaps TrimPosition Rec10 Rec11 FlapsDesiredPos FlapsActualPos Rec13a Rec13b Rec13d \
                                  IdxGearBrakes Gear WheelBrakes Rec3b Rec4b Rec5b Rec6b Rec7b Rec8b \
                                  IdxHeading Pitch Roll Heading RecD RedE RecF RecG RecH \
                                  IdxLatLong lat long alt Rec14 Rec15 Rec16 Rec17 Rec18 \
                                  IdxEngineN1 Engine1N1 Engine2N1 Rec3j Rec4j Rec5j Rec6j Rec7j Rec8j \
                                  IdxCOM COM1_Active COM1_Standby Rec19 COM2_Active COM2_Standby  Rec20 Rec21 Rec22 \
                                  IdxNAV NAV1_Active NAV1_Standby Rec23 Rec24 NAV2_Active NAV2_Standby Rec25 Rec26 \
                                  IdxMarker OuterMarker MiddleMarker InnerMarker Rec4t Rec5t Rec6t Rec7t Rec8t \
                                  IdxAPValues APAirSpeed APHeading APVS APAltitude Rec5y Rec6y Rec7y Rec8y')
        
        logging.debug('Starting unpack to values')

        # X-Plane exports records in groups of 8, the recordset beings with an integer index, and the remainder are floats
        # unpackstring string contains a i8f for each recordset.
        # Not every one of the 8 floats are assigned values
        unpackstring = ""
        for i in range(0, Number_of_Records):
            unpackstring=  unpackstring + 'i8f'
            
        # Now take the received data, and unpack it into a named tuple.
        XPlaneStatus = XPlaneRecord._make(unpack(unpackstring,NewReceivedUDPBytes))


       
        
     
        # Its worth noting that indexes may change between versions of X-Plane. So may need to validate these for X-Plane 11
        # Currently testing with X-Plane 9.  Note Index 17 definitely changed between versions

        SpeedIndex = 3
        MachGLoadIndex = 4
        TrimFlapsIndex = 13
        GearBrakesIndex = 14
        HeadingIndex = 17
        LatLongIndex = 20
        EngineN1Index = 41
        COM1COM2Index = 96
        NAV1NAV2Index = 97
        MarkerIndex = 104
        APValuesIndex = 116

        # Speeds, Vertical Speed
        if (XPlaneStatus.IdxSpeed == SpeedIndex):
            # Have Correct Index, so assign AirSpeed
            #print('Xplane Airspeed is : ' + str(XPlaneStatus.kias))
            outSpeed = "{:.1f}".format(XPlaneStatus.kias)
        else:
            logging.critical('Error in ProcessReceivedString. Speeds IdxSpeed != ' + str(SpeedIndex) + '. Value is: ' + str(XPlaneStatus.IdxSpeed))                            


        # Mach G-Load
        if (XPlaneStatus.IdxMachGLoad == MachGLoadIndex):
            # print( XPlaneStatus.Rec2a, XPlaneStatus.Rec4a, XPlaneStatus.Rec5a, XPlaneStatus.Rec6a, XPlaneStatus.Rec7a, XPlaneStatus.Rec8a)
            #print("Mach :" + str( XPlaneStatus.Mach))
            #print("G Load :" + str( XPlaneStatus.GLoad))
            x = 0
        else:
            logging.critical('Error in ProcessReceivedString. Mach G-Load IdxMachGLoad != ' + str(MachGLoadIndex) + '. Value is: ' + str(XPlaneStatus.IdxMachGLoad))   

        # Trim Flaps
        if (XPlaneStatus.IdxTrimFlaps == TrimFlapsIndex):
            x = 0
            # print(XPlaneStatus.Rec10, XPlaneStatus.Rec11, XPlaneStatus.Rec13a, XPlaneStatus.Rec13b, XPlaneStatus.Rec13d)
            # Trim -0.5 to 0.5
            # print("Trim Position " + str(XPlaneStatus.TrimPosition))           
            # print("Flaps Desired Position " + str(XPlaneStatus.FlapsDesiredPos))
            # print("Flaps Actual Position " + str(XPlaneStatus.FlapsActualPos))
        else:
            logging.critical('Error in ProcessReceivedString. Trim IdxTrimFlaps != ' + str(TrimFlapsIndex) + '. Value is: ' + str(XPlaneStatus.IdxTrimFlaps))


        # Gear Brakes
        if (XPlaneStatus.IdxGearBrakes == GearBrakesIndex):
            #print("Gear Values",  XPlaneStatus.Rec3b, XPlaneStatus.Rec4b, XPlaneStatus.Rec5b, XPlaneStatus.Rec6b, XPlaneStatus.Rec7b, XPlaneStatus.Rec8b)
            #print("Gear :" + str( XPlaneStatus.Gear))
            #print("Wheel Brakes :" + str( XPlaneStatus.WheelBrakes))
            x = 0
        else:
            logging.critical('Error in ProcessReceivedString. Gear Brakes IdxGearBrakes != ' + str(GearBrakesIndex) + '. Value is: ' + str(XPlaneStatus.IdxGearBrakes))  
            
            
        # Angular Velocity in X-Plane 9 - perhaps Pitch, Roll Heading in XPlane 11. Magnetic Compass was added
        if (XPlaneStatus.IdxHeading == HeadingIndex):
            # print('XPlane Heading is : ' + str(XPlaneStatus.Heading))
            outTrackMadeGood = "{:.2f}".format(XPlaneStatus.Heading)
            print("Pitch :", XPlaneStatus.Pitch)
            print("Roll :", XPlaneStatus.Roll)
        else:
            logging.critical('Error in ProcessReceivedString. Heading IdxHeading != ' + str(HeadingIndex) + '. Value is: ' + str(XPlaneStatus.IdxHeading))                            
   
            
        # Latitiude Longitude Altitude
        if (XPlaneStatus.IdxLatLong == LatLongIndex):
            # Have Correct Index, so assign Altitude - but it is returned in feet so convert to meters
            # print('Xplane Altitude is : ' + str(XPlaneStatus.alt))
            wrkfloat = float(XPlaneStatus.alt) * 0.3048
            outAltitude = "{:.1f}".format(wrkfloat)
            #print('XPlane Lat is : ' + str(XPlaneStatus.lat))
            #print('XPlane Long is : ' + str(XPlaneStatus.long))
        else:
            logging.critical('Error in ProcessReceivedString. Latitude, Longitude IdxLatLong != ' + str(LatLongIndex) + '. Value is: ' + str(XPlaneStatus.IdxLatLong))


         # Engine N1 
        if (XPlaneStatus.IdxEngineN1 == EngineN1Index):
            #print( "Engine N1 ",  XPlaneStatus.Rec3j, XPlaneStatus.Rec4j, XPlaneStatus.Rec5j, XPlaneStatus.Rec6j, XPlaneStatus.Rec7j, XPlaneStatus.Rec8j )
            #print("Engine 1 N1: " + str( XPlaneStatus.Engine1N1 ))
            #print("Engine 2 N1: " + str( XPlaneStatus.Engine2N1))
            x = 0
        else:
            logging.critical('Error in ProcessReceivedString. Engine N1 IdxEngineN1 != ' + str(EngineN1Index) + '. Value is: ' + str(XPlaneStatus.IdxEngineN1))      

            
        # COM 1/2 Frequency
        if (XPlaneStatus.IdxCOM == COM1COM2Index):
            # Note Frequencies are stored as integer ie 128.15 is 12815          
            #print('COM1 Active: ' + str(int(XPlaneStatus.COM1_Active)))
            #print('COM1 Standby: ' + str(int(XPlaneStatus.COM1_Standby)))
            #print('COM2 Active: ' + str(int(XPlaneStatus.COM2_Active)))
            #print('COM2 Standby: ' + str(int(XPlaneStatus.COM2_Standby)))
            x=0
            
        else:
            logging.critical('Error in ProcessReceivedString. COM 1/2 Frequency IdxCOM != ' + str(COM1COM2Index) + '. Value is: ' + str(XPlaneStatus.IdxCOM))



        # NAV 1/2 Frequency
        if (XPlaneStatus.IdxNAV == NAV1NAV2Index):
            # Note Frequencies are stored as integer ie 128.15 is 12815
            x=0
            #print('NAV1 Active: ' + str(int(XPlaneStatus.NAV1_Active)))
            #print('NAV1 Standby: ' + str(int(XPlaneStatus.NAV1_Standby)))
            #print('NAV2 Active: ' + str(int(XPlaneStatus.NAV2_Active)))
            #print('NAV2 Standby: ' + str(int(XPlaneStatus.NAV2_Standby)))
           
        else:
            logging.critical('Error in ProcessReceivedString. NAV 1/2 Frequency Idx4 != ' + str(NAV1NAV2Index) + '. Value is: ' + str(XPlaneStatus.IdxNAV))


        # Marker
        if (XPlaneStatus.IdxMarker == MarkerIndex):
            #print("Marker ",  XPlaneStatus.OuterMarker, XPlaneStatus.MiddleMarker, XPlaneStatus.InnerMarker, XPlaneStatus.Rec4t, XPlaneStatus.Rec5t, XPlaneStatus.Rec6t, XPlaneStatus.Rec7t, XPlaneStatus.Rec8t)
            #print("Outer Marker :" + str( XPlaneStatus.OuterMarker))
            #print("Middle Marker :" + str( XPlaneStatus.MiddleMarker))
            #print("Inner Marker :" + str( XPlaneStatus.InnerMarker))
            #print("G Load :" + str( XPlaneStatus.GLoad))
            x = 0
        else:
            logging.critical('Error in ProcessReceivedString. Marker IdxMarker != ' + str(MarkerIndex) + '. Value is: ' + str(XPlaneStatus.IdxMarker))   


         # Auto Pilot Values
        if (XPlaneStatus.IdxAPValues == APValuesIndex):
            #print( XPlaneStatus.Rec5y, XPlaneStatus.Rec6y, XPlaneStatus.Rec7y, XPlaneStatus.Rec8y)
            #print("Autopilot Airspeed :" + str( XPlaneStatus.APAirSpeed))
            #print("Autopilot Altitude :" + str( XPlaneStatus.APAltitude))
            #print("Autopilot Heading :" + str( XPlaneStatus.APHeading))
            #print("Autopilot V/S  :" + str( XPlaneStatus.APVS))
            x = 0
        else:
            logging.critical('Error in ProcessReceivedString. Autopilot Values IdxAPValues != ' + str(APValuesIndex) + '. Value is: ' + str(XPlaneStatus.IdxAPValues))      

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
