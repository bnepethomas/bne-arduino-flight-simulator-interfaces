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
Input_Module_Numer = 0
debugging = False
total_entries = 10

max_packet_size = 150

likelihood_of_change = 0.5005
#likelihood_of_change = 0.8
#likelihood_of_change = 0
# 0.0.0.0 will listen on all addresses, other specify desired source address

command_string = ''


UDP_PORT = 7788
UDP_IP = "127.0.0.1"
TX_UDP_PORT = 26027
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
logging.debug(switch_array)


# Randonmise initial values
counter = 0
while counter < total_entries:
    
    if random.random() > 0.5:
        switch_array[counter] = 1
    else:
        switch_array[counter] = 0
    counter = counter + 1
logging.debug(switch_array)


def Send_UDP_Command(command_to_send):
    
    global UDP_IP
    global TX_UDP_PORT
	

    global UDP_Reflector_IP, UDP_Reflector_Port, SOCK

    logging.debug ("IP target address:" + str(UDP_IP))
    logging.debug ("UDP target port:" + str(TX_UDP_PORT))


    sock.sendto(command_to_send.encode('utf-8'), (UDP_IP, TX_UDP_PORT))
    sock.sendto(command_to_send.encode('utf-8'), (UDP_Reflector_IP, UDP_Reflector_Port))

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
    command_string = ''
    


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



while True:
    try:
        while True:
            logging.debug ('') 


            counter = 0
            while counter < total_entries:           
                if (random.random() * likelihood_of_change) > 0.5:
                    print('Changing Switch '  + str(counter))
                    if switch_array[counter] == 0:
                        switch_array[counter] = 1
                    else:
                        switch_array[counter] = 0
                    Add_UDP_Command('%.2d' % (Input_Module_Numer) + ':' + '%.3d' % (counter) + ':' + str(switch_array[counter]))
                counter = counter + 1
            Send_Remaining_Commands()

            logging.debug (switch_array)


            try:
                sock.settimeout(0.25)
                data, (Source_IP, Source_Port) = sock.recvfrom(1500) # buffer size is 1024 bytes

                        

                print('received message:'), data
                if data == 'CQ':
                   SendAllSwitchStates()
              
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
