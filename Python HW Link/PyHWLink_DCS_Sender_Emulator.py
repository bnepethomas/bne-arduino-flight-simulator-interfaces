#!/usr/bin/python

# PyHWLink_DCS_Sender_Emulator
# https://github.com/bnepethomas/bne-arduino-flight-simulator-interfaces


# Emulates DCS sending telemetry over UDP - largely used to test PyHWLink_Pri_Node_Output.py

import os
import re
import socket
import sys
import time
from datetime import datetime

from pyHWLink_Tools import *




# Global Variables
delay_between_packets = 10
debugging = False


# The string to send - emulating DCS Packets
command_string = "VS:100,ALT:10500,15:1,16:0,17:1,Airspeed:351,Nose Gear:15"



Listen_On_All_IPs = "0.0.0.0"
UDP_PORT = 7789
UDP_Reflector_IP = "127.0.0.1"
UDP_Reflector_Port = 27000


# Date Time String used for logging to screen
def now_mS():
    return str(datetime.now().strftime('%Y-%m-%d %H:%M:%S.%f')[:-3] + ':')

def Send_UDP_Command(command_to_send):
    UDP_IP = "127.0.0.1"
    TX_UDP_PORT = 26028

    global UDP_Reflector_IP, UDP_Reflector_Port, sock

    if debugging: print ("UDP target port:" + str(TX_UDP_PORT))
    print('Target:  ' + UDP_IP + ':' + str(TX_UDP_PORT))
    print('Sending: "' + command_to_send + '"')



    sock.sendto(command_to_send, (UDP_IP, TX_UDP_PORT))
    sock.sendto(command_to_send, (UDP_Reflector_IP, UDP_Reflector_Port))



    

# Being lazy here - want to be able to exit code with ctl-C without waiting full delay
# could use interrupts, but this is nice and simple
def Snooze(Length_Of_Snooze):
    counter = 0
    # The two multiplier is due to sleeping for only 0.5 of a second
    while counter <= (Length_Of_Snooze * 2):
        time.sleep(0.5)

        counter =  counter + 1


def main():

    global sock

    # Setup socket - we aren't planning to explicitly listen
    # more so traffic comes from a known port

    try:
        sock = socket.socket(socket.AF_INET, # Internet
                             socket.SOCK_DGRAM) # UDP
        sock.bind((Listen_On_All_IPs, UDP_PORT))

        while True:
            if debugging: print(time.asctime()) 


            
            Send_UDP_Command(command_string)


            # Delay between sending packets
            print('')
            print(log_mS() + ' Sleeping ' + str(delay_between_packets) + ' seconds')
            print('')
            Snooze(delay_between_packets)
                

            

    except KeyboardInterrupt:
        # Catch Ctl-C and quit
        print('exiting')
        print('')
        sock.close()
        sys.exit(0)
        
    except Exception as other:
        print(time.asctime() + "[e] Error in Main: " + str(other))


main()
