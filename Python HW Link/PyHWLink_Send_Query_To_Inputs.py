#!/usr/bin/python

# Send_Query_To_inputs
# https://github.com/bnepethomas/bne-arduino-flight-simulator-interfaces


# Sends a query to Arduino Input modules to trigger the sending of all switch states
#  The trigger packet is a simple 'CQ'

import logging
import socket
import sys
import time


logging.basicConfig(format='%(asctime)s:%(levelname)s:%(message)s',level=logging.INFO)
#logging.basicConfig(format='%(asctime)s:%(levelname)s:%(message)s',level=logging.DEBUG)
#logging.basicConfig(format='%(asctime)s:%(levelname)s:%(message)s')



# Global Variables
debugging = False

UDP_IP = "127.0.0.1"
UDP_PORT = 7787
TX_UDP_PORT = 7788
UDP_Reflector_IP = "127.0.0.1"
UDP_Reflector_Port = 27000

txsock = socket.socket(socket.AF_INET,socket.SOCK_DGRAM) # UDP
txsock.bind((UDP_IP, UDP_PORT))


def Send_UDP_Command(command_to_send):


    logging.debug("UDP target IP:" + UDP_IP)
    logging.debug("UDP target port:" + str(TX_UDP_PORT))

    

    txsock.sendto(command_to_send, (UDP_IP, TX_UDP_PORT))
    # Copy of packet to reflector
    txsock.sendto(command_to_send, (UDP_Reflector_IP, UDP_Reflector_Port))

    print( 'Query Sent to ' + UDP_IP + ':' + str(TX_UDP_PORT))





# Act on global var
def main():
    print("Sends a query to Arduino Input modules to trigger the sending of all switch states")
    print("The trigger packet is a simple 'CQ'")
    print("")
    while True:
        try:
           while True:
              a = raw_input('Press Enter to send trigger packet:')
              print(time.asctime()) 
              Send_UDP_Command('CQ')        

        except KeyboardInterrupt:
            # Catch Ctl-C and quit
            sys.exit(0)
            
        except Exception as other:
            logging.critical('Error in Main: ' + str(other))
            continue

main()
