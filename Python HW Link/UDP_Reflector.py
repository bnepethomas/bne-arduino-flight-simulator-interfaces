#!/usr/bin/python

# UDP Reflector
#
# Displays and optionally forwards packets to a wireshark receiver
# Listens on a well known port 27000 (ie fixed)
# By default displays all packets but will filter using wireshark filter (ie ip.addr==X.X.X.X)
#
#
# Interactive options
#
# ip.addr==X.X.X.X (no spaces)
#   only display packets from given address (X.X.X.X)
#   display packets from all sources (*)
# f==X.X.X.X
#   forward UDP packets to fixed port


#a = raw_input("need to insert leading zeros so sort is correct enter value and press enter to continue: ")




import os
import socket
import sys
import time

# Used for command line parsing
from optparse import OptionParser



# Global Variables
debugging = True
config_file = 'UDP_Reflector_config.py'

# Address and Port to listen on
UDP_IP_Address = ""
UDP_Port = 27000

# Wireshark target address and Port
wireshark_IP_Address = "127.0.0.1"
wireshark_Port = 27001

# See if input configuration file exists
# Parameters are either specified in this file or passed via command line
# Command line parameters override any settings in the config file
#

if not (os.path.isfile(config_file)):
    
    print('Unable to find ' + config_file)
    
else:
    try:
        from config_file import *
        
            
    except:
        print('Unable to open "' + config_file + '"' )
        print('Or variable assignment incorrect - forgot quotes for string?')
        print('Defaults used')
    




# See if value is assigned.  First we checked config file and then
#   command line arguments

if debugging: print("Checking Command Line parameters")


parser = OptionParser()
parser.add_option("-d","--debug", dest="optiondebug",action="store_true",
                  default=False,
                  help="enable debug",metavar="DEBUGLEVEL")
parser.add_option("-l","--learning", dest="optionLearning",action="store_true",
                  default=False,
                  help="learning mode, assign actions to switch movements")
(options, args) = parser.parse_args()

if debugging: print("options:", str(options))
if debugging: print("arguments:", args)

# Override Learning mode if passed via command line
if options.optionLearning == True:
    learning = True
    print('Learning Mode enabled through command line')
# Override Debug mode if passed via command line    
if options.optiondebug == True:
    debugging = True
    print('Debugging Mode enabled through command line')    



# The timeout is aggressive
serverSock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
serverSock.settimeout(0.0001)
serverSock.bind((UDP_IP_Address, UDP_Port))






def ReceivePacket():

    # 'a' keeps track of how long it has been since packet was received
    a=0


    while True:
        
        try:

            data, (Source_IP, Source_Port)  = serverSock.recvfrom(1500)
            ReceivedPacket = data


            Source_IP = str(Source_IP)
            Source_Port = str(Source_Port)
            ReceivedPacket = str(ReceivedPacket)
            
            if debugging: print ("From: " + Source_IP + " " + Source_Port)
            if debugging: print ("Message: ", ReceivedPacket)

            ProcessReceivedString( str(ReceivedPacket), Source_IP , str(Source_Port))
            
            if debugging: print(a)
            a=0

                                              
        except socket.timeout:
            a=a+1
            if (a > 10000):
                print("Mid Receive Timeout - ", time.asctime())
                a=0
            continue

        except:
            print('Error in ReceivePacket. Error is: ' + sys.exc_info() [0])





def ProcessReceivedString(ReceivedUDPString, Source_IP, Source_Port):

    if debugging: print('Processing UDP String')

    global wireshark_Sock, wireshark_IP_Address, wireshark_Port

    
    try:
        if len(ReceivedUDPString) > 0:
            
            ReceivedUDPString = str(ReceivedUDPString)
            if debugging: print ("From: " + Source_IP + " " + Source_Port)
            if debugging: print('Stage 1 Processing: ' + ReceivedUDPString)
            Send_string = Source_IP + ':' + Source_Port + '---' + ReceivedUDPString
            

            wireshark_Sock = socket.socket(socket.AF_INET, # Internet
                 socket.SOCK_DGRAM) # UDP
            wireshark_Sock.bind(('127.0.0.1', 23456 ))
            print('hi')
            try:
 #               wireshark_Sock.sendto(Send_string, (wireshark_IP_Address, wireshark_Port))
                wireshark_Sock.sendto("pop", ("127.0.0.1", 27005))
            except:
                print('oopps Error in ProcessReceivedString. Error is: ')
                print('doh')
            print('bye')
          
            
    except:
        print('Error in ProcessReceivedString. Error is: ' + sys.exc_info() [0])


def RemoveUnwantedCharacters(stringToBeCleaned):
    stringToBeCleaned= stringToBeCleaned.replace('"', '')
    stringToBeCleaned = stringToBeCleaned.strip(' ')
    return(stringToBeCleaned)




def Main():

    print('Listening on port ' + str(UDP_Port))
    try:
        print('Waiting for packet')
        ReceivePacket()

    except KeyboardInterrupt:
        # Catch Ctl-C and quit
        print('Exiting')
        
        serverSock.close()

        
        sys.exit(0)

    except:
        print(time.asctime(), "Unexpected error:", sys.exc_info() [0])







Main()
