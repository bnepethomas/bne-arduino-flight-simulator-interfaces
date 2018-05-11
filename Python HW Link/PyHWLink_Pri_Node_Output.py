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

from random import randint


# 0.0.0.0 will listen on all addresses, other specify desired source address
UDP_IP = "0.0.0.0"
UDP_PORT = 7784

Source_IP = 0
Source_Port = 0
Last_Source_IP = "127.0.0.1"




def Send_UDP_Command(command_to_send):
    # Send Command to IKARUS in DCS
    # The target port is found in config.lua - ExportScript.Config.ListenerPort
    # We determine where to send the update based on the last data packet we received
    UDP_IP = Last_Source_IP
    UDP_PORT = 26027   


    print ("UDP target IP:"), UDP_IP
    print ("UDP target port:"), UDP_PORT

    sock = socket.socket(socket.AF_INET, # Internet
                     socket.SOCK_DGRAM) # UDP
    sock.sendto(command_to_send, (UDP_IP, UDP_PORT))







def SendAllSwitchStates():
    print ("Sending Switch States")


def Reboot():
    print ("Received a Reboot - rebooting")

    time.sleep(1)
    GPIO.cleanup()       # clean up GPIO on CTRL+C exit
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
          sock.settimeout(2)
          data, (Source_IP, Source_Port) = sock.recvfrom(1024) # buffer size is 1024 bytes
          if (Source_IP != Last_Source_IP):
            Last_Source_IP = Source_IP  
            print ("New Source Address Found")
            SendAllSwitchStates()
          
          #print "received message:", data
          words = data.split(":")
          #print words


          with canvas(device) as draw:
            for current_word in words:
                  #print(current_word)
                  #print(len(current_word))

                  # Basic sanity check to catch values that are too short
                  if len(current_word) >= 3:
                      values = current_word.split("=")
                      #print values
                      #print values[0] + "+" + values[1]

                      if values[0] == '91':
                            #print "Handling 91-lamp_ENGINE_OIL_PRESS"
                            if values[1] == "1":
                                draw.point((0, 7 ), 1)
                            else:
                                draw.point((0, 7 ), 0)


                      if values[0] == '92':
                            #print "Handling 92-lamp_ENGINE_ICING"
                            if values[1] == "1":
                                draw.point((0, 6), 1)
                            else:
                                draw.point((0, 6), 0)


                      if values[0] == '93':
                            #print "Handling 93-lamp_ENGINE_ICE_DET"
                            if values[1] == "1":
                                draw.point((0, 5 ), 1)
                            else:
                                draw.point((0, 5 ), 0)                             
                      if values[0] == '94':
                            #print "Handling 94-lamp_ENGINE_CHIP_DET"
                            if values[1] == "1":
                                draw.point((0, 4 ), 1)
                            else:
                                draw.point((0, 4 ), 0)
                      if values[0] == '95':
                            #print "Handling 95-lamp_LEFT_FUEL_BOOST"
                            if values[1] == "1":
                                draw.point((0, 3 ), 1)
                            else:
                                draw.point((0, 3 ), 0)                                
                      if values[0] == '96':
                            #print "Handling 96-lamp_RIGHT_FUEL_BOOST"
                            if values[1] == "1":
                                draw.point((0, 1 ), 1)
                            else:
                                draw.point((0, 1 ), 0)                                 

                      if values[0] == '97':
                            #print "Handling 97-lamp_ENG_FUEL_PUMP"
                            if values[1] == "1":
                                draw.point((0, 2 ), 1)
                            else:
                                draw.point((0, 2 ), 0)
                      if values[0] == '98':
                            #print "Handling 98-lamp_20_MINUTE"
                            if values[1] == "1":
                                draw.point((0, 0 ), 1)
                            else:
                                draw.point((0, 0 ), 0)
                      if values[0] == '99':
                            #print "Handling 99-lamp_FUEL_FILTER"
                            if values[1] == "1":
                                draw.point((2, 1 ), 1)
                            else:
                                draw.point((2, 1 ), 0)                                
                      if values[0] == '100':
                            #print "Handling 100-lamp_GOV_EMERG"
                            if values[1] == "1":
                                draw.point((2, 0 ), 1)
                            else:
                                draw.point((2, 0 ), 0)                                
                      if values[0] == '101':
                            #print "Handling 101-lamp_AUX_FUEL_LOW"
                            if values[1] == "1":
                                draw.point((1, 7 ), 1)
                            else:
                                draw.point((1, 7 ), 0)  
                      if values[0] == '102':
                            #print "Handling 102-lamp_XMSN_OIL_PRESS"
                            if values[1] == "1":
                                draw.point((1, 6 ), 1)
                            else:
                                draw.point((1, 6 ), 0)
                      if values[0] == '103':
                            #print "Handling 103-lamp_XMSN_OIL_HOT"
                            if values[1] == "1":
                                draw.point((1, 5 ), 1)
                            else:
                                draw.point((1, 5 ), 0)
                      if values[0] == '104':
                            #print "Handling 104-lamp_HYD_PRESSURE"
                            if values[1] == "1":
                                draw.point((1, 4 ), 1)
                            else:
                                draw.point((1, 4 ), 0)
                      if values[0] == '105':
                            #print "Handling 105-lamp_ENGINE_INLET_AIR"
                            if values[1] == "1":
                                draw.point((1, 3 ), 1)
                            else:
                                draw.point((1, 3 ), 0)
                      if values[0] == '106':
                            #print "Handling 106-lamp_INST_INVERTER"
                            if values[1] == "1":
                                draw.point((1, 2 ), 1)
                            else:
                                draw.point((1, 2 ), 0)
                      if values[0] == '107':
                            #print "Handling 107-lamp_DC_GENERATOR"
                            if values[1] == "1":
                                draw.point((1, 1 ), 1)
                            else:
                                draw.point((1, 1 ), 0)
                      if values[0] == '108':
                            #print "Handling 108-lamp_EXTERNAL_POWER"
                            if values[1] == "1":
                                draw.point((1, 0 ), 1)
                            else:
                                draw.point((1, 0 ), 0)
                      if values[0] == '109':
                            #print "Handling 109-lamp_CHIP_DETECTOR"
                            if values[1] == "1":
                                draw.point((3, 1 ), 1)
                            else:
                                draw.point((3, 1 ), 0)
                      if values[0] == '110':
                            #print "Handling 110-lamp_IFF"
                            if values[1] == "1":
                                draw.point((3, 0 ), 1)
                            else:
                                draw.point((3, 0 ), 0)
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
        print("timeout")
        continue

    except KeyboardInterrupt:
        # Catch Ctl-C and quit

        sys.exit(0)

    except:
        print("Unexpected error:", sys.exc_info() [0])
        continue
