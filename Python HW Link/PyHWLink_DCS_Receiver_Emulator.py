#!/usr/bin/python

# DCS Receiver Emulator
# https://github.com/bnepethomas/bne-arduino-flight-simulator-interfaces


# Listens on 26026 and printers whatever it receives.




import argparse
import numbers
import logging
import os
import random
import re
import socket
import sys
import time




#logging.basicConfig(format='%(asctime)s:%(levelname)s:%(message)s',level=logging.INFO)
logging.basicConfig(format='%(asctime)s:%(levelname)s:%(message)s',level=logging.DEBUG)
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


# Global Variables

debugging = False



# 0.0.0.0 will listen on all addresses, other specify desired source address

command_string = ''


UDP_PORT = 26026
UDP_IP = "127.0.0.1"


Source_IP = 0
Source_Port = 0
Last_Source_IP = "127.0.0.1"

# Setup inputs 


sock = socket.socket(socket.AF_INET, # Internet
                     socket.SOCK_DGRAM) # UDP
sock.bind((UDP_IP, UDP_PORT))



while True:
    try:
        while True:
            logging.debug ('') 

            try:
                sock.settimeout(5)
                data, (Source_IP, Source_Port) = sock.recvfrom(1500) # buffer size is 1024 bytes
              

                print('received message:'), data

              
            except KeyboardInterrupt:
                # Catch Ctl-C and quit
                print('')
                print('Exiting')
                print('')
                sock.close()
                sys.exit(0)   


    except socket.timeout:
        logging.debug ('Timeout')       
        continue
        

    except KeyboardInterrupt:
        # Catch Ctl-C and quit
        print('')
        print('Exiting')
        print('')
        sock.close()
        sys.exit(0)
        
    except Exception as other:
        logging.critical('Error in Main: ' + str(other))
        continue
