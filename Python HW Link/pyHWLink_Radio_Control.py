#!/usr/bin/python

# pyHWLink_Radio_Control
#

# Listens on 7794

# 


import logging
import os
import socket
import sys
import time
import threading

logging.basicConfig(format='%(asctime)s:%(levelname)s:%(message)s',level=logging.INFO)
#logging.basicConfig(format='%(asctime)s:%(levelname)s:%(message)s',level=logging.DEBUG)
#logging.basicConfig(format='%(asctime)s:%(levelname)s:%(message)s')
 
MIN_VERSION_PY3 = 5    # min. 3.x version
if (sys.version_info[0] < 3):
        Warning_Message = "ERROR: This script requires a minimum of Python 3." + str(MIN_VERSION_PY3) 
        print('')
        logging.critical(Warning_Message)
        print('')
        print('Invalid Version of Python running')
        print('Running Python earlier than Python 3.0! ' + sys.version)
        sys.exit(Warning_Message)

elif (sys.version_info[0] == 3 and sys.version_info[1] < MIN_VERSION_PY3):
        Warning_Message = "ERROR: This script requires a minimum of Python 3." + str(MIN_VERSION_PY3)           
        print('')
        logging.critical(Warning_Message)  
        print('')
        print('Invalid Version of Python running')
        print('Running Python ' + sys.version)
        sys.exit(Warning_Message)
        

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
debugging = False
config_file = 'Radio_Control_config.py'
filterString = ''
secondsBetweenKeepAlives = 5

# Initialise keepalive indicator
last_time_display = time.time()
last_packet_received = time.time()
packets_processed = 0

# Address and Port to listen on
UDP_IP_Address = ""
UDP_Port = 7794

radio_channel = 0


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
    iterations_Since_Last_Packet = 0


    while True:
        
        try:

            data, (Source_IP, Source_Port)  = serverSock.recvfrom(1500)

            ReceivedPacket = data
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
            if debugging == True and (iterations_Since_Last_Packet > 10000):
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



def ProcessReceivedString(ReceivedUDPString, Source_IP, Source_Port):

    logging.debug('Processing UDP String')

    global last_packet_received
    global radio_channel
    
    
    try:
        if len(ReceivedUDPString) > 0:

            # When dealing with encoder need to only catch button press for timing
            # As we are getting Press and then release at the end of the pulse width
            # So seeing two events for each transition
            if (ReceivedUDPString.find('Pressed') != -1):
                #print(ReceivedUDPString)                
                packet_time = time.time()
                packet_delta = packet_time - last_packet_received
                print('Packet TimeDelta : ' + str(packet_delta))
                last_packet_received = packet_time

                # If there is a small delay between steps, suggests operater is in a hurry
                # Make steps larger
                if (packet_delta <= 0.1):
                    delta_size = 5
                else:
                    delta_size = 1

                # Working on buttons 11 and 12
                if (ReceivedUDPString.find('11') != -1):
                    
                    radio_channel = radio_channel + delta_size
                elif (ReceivedUDPString.find('12') != -1):
                    radio_channel = radio_channel - delta_size
                print('Radio Channel : ' + str(radio_channel))
                  
                  
            ReceivedUDPString = str(ReceivedUDPString)
            logging.debug("From: " + Source_IP + " " + Source_Port)
            logging.debug('Payload: ' + ReceivedUDPString)
            Send_String = Source_IP + ':' + Source_Port + '---' + ReceivedUDPString
            
  
                
 
    except Exception as other:
        logging.critical('Error in ProcessReceivedString. Error is: ' + str(other))          
             



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
