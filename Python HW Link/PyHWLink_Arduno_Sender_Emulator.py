#!/usr/bin/python

# Arduino Sendor Emulator
# https://github.com/bnepethomas/bne-arduino-flight-simulator-interfaces


# Emulates a Arduino running OverPros Joystick interface with a Ethernet shield
# As we aren't exposed to an any Joystick button limits, there are 256 buttons supported
# Instead of using the stub to provide a Joystick via USB, delta are sent to a
# process running on a Raspberry Pi which maps these deltas to commands for the Sim
#
# The Arduino code is independant of the aircraft and simulator that is running,
# which simplifies its operation.
#
# There is a receive operation where the Arduino will report the state of all
# 256 inputs.  This will be spread across several packets with a 300mS delay between
# the packets.  The trigger packet is a simple 'CQ'




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
total_entries = 256

max_packet_size = 150

likelihood_of_change = 0.5005
#likelihood_of_change = 0
# 0.0.0.0 will listen on all addresses, other specify desired source address

command_string = ''

UDP_IP = "0.0.0.0"
UDP_PORT = 7788
UDP_Reflector_IP = "127.0.0.1"
UDP_Reflector_Port = 27000

Source_IP = 0
Source_Port = 0
Last_Source_IP = "127.0.0.1"

# Initialise array to 0 
switch_array = []
counter = 0
while counter < total_entries:
    switch_array.append(0)
    counter = counter + 1
print('There are ' + str(len(switch_array)) + ' switches')
if debugging: print(switch_array)


# Randonmise initial values
counter = 0
while counter < total_entries:
    
    if random.random() > 0.5:
        switch_array[counter] = 1
    else:
        switch_array[counter] = 0
    counter = counter + 1
if debugging: print(switch_array)


def Send_UDP_Command(command_to_send):
    UDP_IP = "127.0.0.1"
    TX_UDP_PORT = 26027

    global UDP_Reflector_IP, UDP_Reflector_Port

    if debugging: print ("UDP target port:" + str(TX_UDP_PORT))


    txsock = socket.socket(socket.AF_INET, # Internet
                     socket.SOCK_DGRAM) # UDP


    txsock.sendto(command_to_send, (UDP_IP, TX_UDP_PORT))
    txsock.sendto(command_to_send, (UDP_Reflector_IP, UDP_Reflector_Port))

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
    
    global command_string

    if command_string != '':
        Send_UDP_Command('D' + command_string)
    


def SendAllSwitchStates():

    global command_string
    
    print ("Sending Switch States")

    command_string = ''
    counter = 0
    while counter < total_entries:                         
        Add_UDP_Command('%.2d' % (Input_Module_Numer) + ':' + '%.3d' % (counter) + ':' + str(switch_array[counter]))

        counter = counter + 1

    # Send out buffered commands    
    Send_Remaining_Commands()


# Setup inputs and outputs


sock = socket.socket(socket.AF_INET, # Internet
                     socket.SOCK_DGRAM) # UDP
sock.bind((UDP_IP, UDP_PORT))

# Act on global var

while True:
    try:
       while True:
          if debugging: print(time.asctime()) 


          counter = 0
          while counter < total_entries:           
            if (random.random() * likelihood_of_change) > 0.5:
                print('Changing Switch '  + str(counter))
                if switch_array[counter] == 0:
                    switch_array[counter] = 1
                else:
                    switch_array[counter] = 0
                Send_UDP_Command('D' + '%.2d' % (Input_Module_Numer) + ':' + '%.3d' % (counter) + ':' + str(switch_array[counter]))
            counter = counter + 1
          if debugging:  print(switch_array)

          
          sock.settimeout(0.25)
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
