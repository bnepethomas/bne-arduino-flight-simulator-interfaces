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

    try:
  
        # http://blog.shahada.abubakar.net/post/linux-udp-network-client-library-for-x-plane-10-and-11
        


        values = ('CMND'.encode('utf-8'), 0, 'sim/flight_controls/flaps_down'.encode('utf-8'))
        packer = struct.Struct('4s B 32s')

        packed_data = packer.pack(*values)

        i = 0
        while (i < 4):
            print('UDP target IP:', UDP_IP_Address)
            print('UDP target port:', UDP_Port)
            print('sending "%s"' % binascii.hexlify(packed_data), values)
            

            sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM) # UDP
            sock.sendto((packed_data), (UDP_IP_Address, UDP_Port))
            time.sleep(0.1)
            i +=1

        timetosleep = 5
        print("Sleeping for " + str(timetosleep))
        time.sleep(timetosleep)


        values = ('CMND'.encode('utf-8'), 0, 'sim/flight_controls/flaps_up'.encode('utf-8'))
        packer = struct.Struct('4s B 30s')
        packed_data = packer.pack(*values)

        
        i = 0
        while (i < 4):
            print('UDP target IP:', UDP_IP_Address)
            print('UDP target port:', UDP_Port)
            print('sending "%s"' % binascii.hexlify(packed_data), values)
            

            sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM) # UDP
            sock.sendto((packed_data), (UDP_IP_Address, UDP_Port))
            time.sleep(0.1)
            i +=1
        
    
    except Exception as other:
        logging.critical('Error in SendTestPacket: ' + str(other))
   

 
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
