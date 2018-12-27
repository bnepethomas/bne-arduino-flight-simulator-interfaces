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

#logging.basicConfig(format='%(asctime)s:%(levelname)s:%(message)s',level=logging.INFO)
logging.basicConfig(format='%(asctime)s:%(levelname)s:%(message)s',level=logging.DEBUG)
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


    while True:
        
        try:

            data, (Source_IP, Source_Port)  = serverSock.recvfrom(1500)

            # Need to decode the payload to convert from bytes object to a string
            ReceivedPacket = data.decode()
            packets_processed = packets_processed + 1

            Source_IP = str(Source_IP)
            Source_Port = str(Source_Port)
            ReceivedPacket = str(ReceivedPacket)
            
            logging.debug("From: " + Source_IP + " " + Source_Port)
            logging.debug("Message: " + ReceivedPacket)

            ProcessReceivedString( str(ReceivedPacket), Source_IP , str(Source_Port) )
            
            logging.debug("Iterations since last packet " + str(iterations_Since_Last_Packet))
            
            iterations_Since_Last_Packet=0

                                              
        except socket.timeout:
            iterations_Since_Last_Packet = iterations_Since_Last_Packet +  1
            if debugging == True and (iterations_Since_Last_Packet > 3000):
                print("[i] Mid Receive Timeout - " + time.asctime())
                                
                # Timeout in dara from Flight Sim - locate GPS in Brisbane
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
                outAltitude = 1
                                        
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



def ProcessReceivedString(ReceivedUDPString, Source_IP, Source_Port):

    logging.debug('Processing UDP String')

    global wireshark_Sock, wireshark_IP_Address, wireshark_Port

    
    try:
        if len(ReceivedUDPString) > 0:
            

            logging.debug("From: " + Source_IP + " " + Source_Port)         
            logging.debug('Payload: ' + ReceivedUDPString)
            

            ParsePayload(ReceivedUDPString)
            
            Send_GPRMC()
            Send_GPGGA()
            Send_GPGSA()
            
            

    except Exception as other:
        logging.critical('Error in ProcessReceivedString. Error is: ' + str(other))          
            

def ParsePayload(Payload):

    global outAltitude
 #   global outUTC 
 #   global outDate 
 #   global xoutputstr 
 #   global outNorS 
 
 #global youtputstr 
 #   global outEorW 
 #   global outSpeed 
    global outTrackMadeGood
 #   global outMagVar
 #   global outMagEorW 


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
                            outAltitude = workingFields[1]
                            
                        if (workingkey=='magheading'):                            
                            outTrackMadeGood = workingFields[1]




                           
                        
    
                    except Exception as other:
                        print('')
                        print('WARNING - Unable to read record of interest in ProcessPayload')
                        print('WARNING - Record name is: "' + workingkey + '"')
                        print('')
                        logging.critical("Error in ProcessPayload: " + str(other))
                


            logging.debug('Continuing on')
            


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

    localDebugging = False

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
    localDebugging = False
    outStartOfString = "GPGGA"
    outGPSFix = "6";
    outNoofSatellites = "03"
    outPrecision = "0.0"
    outEndOfString = "*"

    outcompletestring = outStartOfString + "," + outUTC + ","
    outcompletestring = outcompletestring + xoutputstr + "," + outNorS + ","
    outcompletestring = outcompletestring + youtputstr + "," + outEorW + ","
    outcompletestring = outcompletestring + outGPSFix + ","
    outcompletestring = outcompletestring + outNoofSatellites + ","
    outcompletestring = outcompletestring + outPrecision + ","
    outcompletestring = outcompletestring + str(outAltitude) + ".0,M,,,,"
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
outAltitude = 1


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