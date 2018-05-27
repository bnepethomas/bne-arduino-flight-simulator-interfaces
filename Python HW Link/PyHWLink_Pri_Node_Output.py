#!/usr/bin/python

# Primary Node HW interface for DCS
# Includes insecure remote shutdown 
# https://github.com/bnepethomas/bne-arduino-flight-simulator-interfaces

# git commit -a
# git push


# Added Shutdown and reboot options
# Variable '999':
# values[1] == "ShutdownAndHalt":
# values[1] == "Reboot":


# If Sending Commands to DCS with Ikarus installed
# The values to be sent can be found
# C:\Program Files\Eagle Dynamics\DCS World\Mods\aircraft\Uh-1H\Input\UH-1H\joystick
# Structure is
# C - COmmand
# 15 - Cockpit Device Id
# 3003 - Unknown but seen in multiple places
# Switch Position
# Send to Port UDP_PORT = 26027
# MESSAGE = "C15,3003,-1" - Turns Test Switch on - All warning lights
# MESSAGE = "C15,3003,0" - Turns Test Switch to centre
# MESSAGE = "C15,3003,1" - Turns Test Switch to Reset - clears caution on front panel
# Bright/Dim 15,3004


# Change History
#
# 20180511 Duplicated from receiver_004 - Huey Project

import argparse
import numbers
import os
import re
import socket
import sys
import time

from credentials import *

from random import randint


# 0.0.0.0 will listen on all addresses, other specify desired source address
UDP_IP = "0.0.0.0"
UDP_PORT = 7784
UDP_Reflector_IP = "127.0.0.1"
UDP_Reflector_Port = 27000

Source_IP = 0
Source_Port = 0
Last_Source_IP = "127.0.0.1"





def Send_UDP_Command(command_to_send):
    # Send Command to IKARUS in DCS
    # The target port is found in config.lua - ExportScript.Config.ListenerPort
    # We determine where to send the update based on the last data packet we received
    UDP_IP = "127.0.0.1"
    TX_UDP_PORT = 26027

    global UDP_Reflector_IP, UDP_Reflector_Port


    print ("UDP target IP:", UDP_IP)
    print ("UDP target port:", TX_UDP_PORT)

    txsock = socket.socket(socket.AF_INET, # Internet
                     socket.SOCK_DGRAM) # UDP

    reflector_sock = socket.socket(socket.AF_INET, # Internet
                     socket.SOCK_DGRAM) # UDP

    
    print ("step2") 
    # imported from credentials 
    print (whatami)               
    #sock.sendto(command_to_send, (UDP_IP, TX_UDP_PORT))
    txsock.sendto("00:098:1,02:003:0", (UDP_IP, TX_UDP_PORT))
    reflector_sock.sendto("00:098:1,02:003:0", (UDP_Reflector_IP, UDP_Reflector_Port))






def SendAllSwitchStates():
    print ("Sending Switch States")


def Reboot():
    print ("Received a Reboot - rebooting")
    print ("Here's what would have happened - os.system('shutdown now -r')")
    #os.system("shutdown now -r")
    sys.exit(0)

def ShutdownAndHalt():
    print ("Received a Halt - shutting down")
    print ("Here's what would have happened - os.system('shutdown now -h')")
    #os.system("shutdown now -h")
    sys.exit(0)
    



# Setup inputs and outputs


sock = socket.socket(socket.AF_INET, # Internet
                     socket.SOCK_DGRAM) # UDP
sock.bind((UDP_IP, UDP_PORT))

# Act on global var

while True:
    try:
       while True:
          print(time.asctime()) 
          Send_UDP_Command("C15,3004,-1")
          
          sock.settimeout(0.25)
          data, (Source_IP, Source_Port) = sock.recvfrom(1500) # buffer size is 1024 bytes
          if (Source_IP != Last_Source_IP):
            Last_Source_IP = Source_IP  
            print ("New Source Address Found")
            SendAllSwitchStates()
          
          
          
          #print "received message:", data
          words = data.split(":")
          #print words



          for current_word in words:
                #print(current_word)
                #print(len(current_word))

                # Basic sanity check to catch values that are too short
                if len(current_word) >= 3:
                    values = current_word.split("=")
                    #print values
                    #print values[0] + "+" + values[1]

                    if values[0] == '91':
                        print ("Handling 91-lamp_ENGINE_OIL_PRESS")


                    if values[0] == '999':
                        print ("Handling SHUTDOWN")
                        if values[1] == "ShutdownAndHalt":
                            # Stop listening on original port
                            sock.close()
                            ShutdownAndHalt()
                        elif values[1] == "Reboot":
                            # Stop listening on original port
                            sock.close()
                            Reboot()                                      
                        else:
                            print ("Received a Invalid Shutdown Request")


    


    except socket.timeout:
        print(time.asctime(), "timeout")
        
        continue
        

    except KeyboardInterrupt:
        # Catch Ctl-C and quit

        sys.exit(0)

    except:
        print(time.asctime(), "Unexpected error:", sys.exc_info() [0])
        continue
