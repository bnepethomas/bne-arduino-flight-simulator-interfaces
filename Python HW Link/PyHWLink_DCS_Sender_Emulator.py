#!/usr/bin/python

# Arduino Sendor Emulator
# https://github.com/bnepethomas/bne-arduino-flight-simulator-interfaces


# Emulates DCS


import argparse
import numbers
import os
import re
import socket
import sys
import time


import random


# Global Variables
Input_Module_Numer = 0
debugging = False
max_packet_size = 150


#likelihood_of_change = 0
# 0.0.0.0 will listen on all addresses, other specify desired source address

command_string = ''

UDP_IP = "0.0.0.0"
UDP_PORT = 7789
UDP_Reflector_IP = "127.0.0.1"
UDP_Reflector_Port = 27000

Source_IP = 0
Source_Port = 0
Last_Source_IP = "127.0.0.1"







def Send_UDP_Command(command_to_send):
    UDP_IP = "127.0.0.1"
    TX_UDP_PORT = 26028

    global UDP_Reflector_IP, UDP_Reflector_Port, sock

    if debugging: print ("UDP target port:" + str(TX_UDP_PORT))



    sock.sendto(command_to_send, (UDP_IP, TX_UDP_PORT))
    sock.sendto(command_to_send, (UDP_Reflector_IP, UDP_Reflector_Port))

# When doing a bulk send - group commands into packets with an approx size of 1000 bytes
def Add_UDP_Command(command_to_add):

    global command_string

    if command_string == '':
        command_string = command_to_add
    else:
        command_string = command_string + ',' + command_to_add

    if len(command_string) > max_packet_size:
        Send_UDP_Command('D' + command_string)
        command_string = ''
        


def Send_Remaining_Commands():

    # Don't expect a D to prefix packets from DCS
    # This is different to all other inter module packets which are prefixed
    global command_string

    print (time.asctime() + ' Sending DCS Codes')
    if command_string != '':
        Send_UDP_Command(command_string)
    command_string = ''
    




# Setup inputs and outputs


sock = socket.socket(socket.AF_INET, # Internet
                     socket.SOCK_DGRAM) # UDP
sock.bind((UDP_IP, UDP_PORT))

# Act on global var

while True:
    try:
       while True:
          if debugging: print(time.asctime()) 


          command_string = "VS:100,ALT:10500,15:1,Airspeed:351,Nose Gear:15"  
          Send_Remaining_Commands()
        
          if debugging:  print(switch_array)

          

          sock.settimeout(10)
          data, (Source_IP, Source_Port) = sock.recvfrom(1500) # buffer size is 1024 bytes

                    
          
          print "received message:", data
          if data == 'CQ':
              SendAllSwitchStates()
              
    


    except socket.timeout:
        if debugging: print(time.asctime() + " timeout")       
        continue
        

    except KeyboardInterrupt:
        # Catch Ctl-C and quit
        sys.exit(0)
        
    except Exception as other:
        print(time.asctime() + "[e] Error in Main: " + str(other))
        continue
