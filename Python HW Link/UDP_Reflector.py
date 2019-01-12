#!/usr/bin/python

# UDP Reflector
#
# Displays and optionally forwards packets to a wireshark receiver
# Listens on a well known port 27000 (ie fixed)
# By default displays all packets but will filter using simple character matching (no wild carding)
#
#
# Command line options
#
# filterstring - display packets containing this string
# IP Address and Port to send to wireshark


import logging
import os
import socket
import sys
import time
import threading

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
debugging = False
config_file = 'UDP_Reflector_config.py'
filterString = ''
secondsBetweenKeepAlives = 5

# Initialise keepalive indicator
last_time_display = time.time()
packets_processed = 0

# Address and Port to listen on
UDP_IP_Address = ""
UDP_Port = 27000

# Wireshark target address and Port
wireshark_IP_Address = None
wireshark_Port = 27001

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

    
    parser.add_option("-w","--wh", dest="opt_W_Host",
                      help="Wireshark Target IP Address",metavar="opt_W_Host")    
    parser.add_option("-u","--wp", dest="opt_W_Port",
                      help="Wireshark Target Port. 27001 is used if not explicitly specified",
                      metavar="opt_W_Port") 
    (options, args) = parser.parse_args()


    
    logging.debug("options:" + str(options))
    logging.debug("arguments:" +  str(args))


    if options.opt_W_Host != None:
        wireshark_IP_Address = str(options.opt_W_Host)

    if options.opt_W_Port != None:
        wireshark_Port = str(options.opt_W_Port)


    
    if wireshark_IP_Address != None:
        logging.info("Wireshark host is : " + wireshark_IP_Address)
        logging.info("Wireshark UDP port is : " + str(wireshark_Port))

    if len(args) != 0:
        filterString = args[0]
        logging.info("Display Filter is :" + str(args[0]))            


   

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

    global wireshark_Sock, wireshark_IP_Address, wireshark_Port

    
    try:
        if len(ReceivedUDPString) > 0:
            
            ReceivedUDPString = str(ReceivedUDPString)
            logging.debug("From: " + Source_IP + " " + Source_Port)
            logging.debug('Payload: ' + ReceivedUDPString)
            Send_String = Source_IP + ':' + Source_Port + '---' + ReceivedUDPString
            
            # Is Wireshark target address set - if so throw a copy of the packet in its direction
            if wireshark_IP_Address != None:
                wireshark_Sock.sendto(Send_String, (wireshark_IP_Address, int(wireshark_Port)))

                
            # If we have passed a filter string via the command line - then display
            #   packets that contain the string (this includes the IP address of the sender)
            if filterString !="":
                if filterString in Send_String:
                    print(Send_String)

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
