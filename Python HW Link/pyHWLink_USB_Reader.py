# pyHWLink_USB_Reader
# https://www.raspberrypi.org/forums/viewtopic.php?t=13680
# importing modules. Note that this program requires PyUSB.
# Reference https://github.com/pyusb/pyusb/blob/master/docs/tutorial.rst




# sudo apt install python3-usb
# sudo python3 pyHWLink_USB_Reader.py
#
# If program is interrupted, the next time it is run - it errors
#    raise USBError(_strerror(ret), ret, _libusb_errno[ret])
#    usb.core.USBError: [Errno 2] Entity not found
#
# If the target devices is unplugged and then reconnected code runs ok
# Isolated to Detach Kernel Driver - so just used separate Try block to step over it

# Reading a 100 times a second witout sending anything to stdout has CPU @ 1-3%
# If outputings a line CPU utilisation rises to 10-11%

# to list attached USB devices
#   lsusb 
#   Bus 001 Device 015: ID 16c0:05b5 Van Ooijen Technische Informatica
# The -t displays tree and max supported speeds
# Port 2: Dev 15, If 0, Class=Human Interface Device, Driver=, 12M
# At BU0836X returns
# Bus 001 Device 019: ID 1dd2:1001
# The Device Number increments for each connect event

# needed to do this on mac sudo python3 -mpip install pyusb



# https://www.raspberrypi.org/forums/viewtopic.php?t=13680
# importing modules. Note that this program requires PyUSB.
# Reference https://github.com/pyusb/pyusb/blob/master/docs/tutorial.rst
import usb.core
import time

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




UDP_PORT = 7795
UDP_IP = "127.0.0.1"
# Sends to Radio_Control
TX_UDP_PORT = 7794
UDP_Reflector_IP = "127.0.0.1"
UDP_Reflector_Port = 27000

Source_IP = 0
Source_Port = 0
Last_Source_IP = "127.0.0.1"


def Send_UDP_Command(command_to_send):
    
    global UDP_IP
    global TX_UDP_PORT
	

    global UDP_Reflector_IP, UDP_Reflector_Port, SOCK

    logging.debug ("IP target address:" + str(UDP_IP))
    logging.debug ("UDP target port:" + str(TX_UDP_PORT))


    sock.sendto(command_to_send.encode('utf-8'), (UDP_IP, TX_UDP_PORT))
    sock.sendto(command_to_send.encode('utf-8'), (UDP_Reflector_IP, UDP_Reflector_Port))


# Initialise the network

print ('Initialising Network')                                                  
sock = socket.socket(socket.AF_INET, # Internet
                     socket.SOCK_DGRAM) # UDP
sock.bind((UDP_IP, UDP_PORT))


# Currently geared for a single Leo Bodnar card. Just try in series
# Later code is liekly to specify which card is of interest
# creating the object representing the BU0836
ub = usb.core.find(idVendor=0x16c0, idProduct=0x05b5)

# creating the object representing the BU0836X
if ub is None:
    # Try the BU0836X
    ub = usb.core.find(idVendor=0x1dd2, idProduct=0x1001)

if ub is None:
    # Try the BBI-32
    ub = usb.core.find(idVendor=0x1dd2, idProduct=0x1150)

# checking if the UB0836 was found
if ub is None:
    raise ValueError("Neither BU0836, BU0836X, or BBI-32 are connected")
try:
    # existing kernel drivers must be detached for this to work
    print('Detaching Kernel')
    ub.detach_kernel_driver(0)
except:
    print('unable to Detach Kernel Driver')
    
lastubdata = 0
first_loop = True

# Initialise Button Array = noting this is a Base 1 not base 0!
# Initially using 32 buttons = but the BBI-32 supports rotary swiches when enables up to 132 inputs
Max_Buttons = 132
button_array = [0] * (Max_Buttons + 1)
i = 1
while( i < Max_Buttons):
    button_array[i] = 0
    i = i + 1
    
try:

    # setting configuration
    print('Set Configuration')
    ub.set_configuration()
    # we arere now ready to show the output of first analogue port of BU0836
    # the endPointAdress and maxPacketSize values are required for this
    start = time.time()
    i = 0
    while (True):
        ubdata = ub.read(0x81, 0x0020, timeout=0)

        # print(ubdata[0] + 256 * ubdata[1])
        # print(ubdata)
        
        if (ubdata != lastubdata):
            lastubdata = ubdata
            
            # Remember this array is zero based
            button_ptr = 0
            while (button_ptr < Max_Buttons):
                
                bit_pos  = button_ptr % 8
                # print('Bit Pos: ' + str(bit_pos))
                
                byte_pos = int(button_ptr / 8)
                # print('Byte Pos: ' + str(byte_pos))
                button_ptr = button_ptr + 1
                
                
                # print( 'Byte Pos ' + str(byte_pos) + ' Byte Value' + str(ubdata[byte_pos]) + ' Bit Pos' + str(bit_pos))
        
                localint = ubdata[byte_pos]
                bit_offset = bit_pos
                mask = 1 << bit_offset
                # print ('mask ' + str(mask))
                if (localint & mask != 0):
                    if (button_array[button_ptr] == 0):
                        print ('Button Pressed ' + str(button_ptr))
                        Send_UDP_Command('Button Pressed ' + str(button_ptr))
                        button_array[button_ptr] = 1
                    
                else:
                    if (button_array[button_ptr] == 1):
                        print ('Button Released ' + str(button_ptr))
                        Send_UDP_Command('Button Released ' + str(button_ptr))
                        button_array[button_ptr] = 0

        
        # With no buttons pressed
        # array('B', [0, 0, 0, 0, 15])
        # Button 1
        # array('B', [1, 0, 0, 0, 15])
        # Button 2
        # array('B', [2, 0, 0, 0, 15])
        # Button 3
        # array('B', [4, 0, 0, 0, 15])
        # Button 4
        # 8,0,0,0
        # Button 5
        # 16,0,0,0
        # Button 6
        # 32,0,0,0
        # Button 7
        # 64,0,0,0
        # Button 8
        # 128,0,0,0
        # Button 9
        # 0,1,0,0
        # button 10
        # 0,2,0,0
        # Button 20
        # 0,0,8,0
        # Button 32
        # 0,0,0,128
        
        
        first_loop = False
        time.sleep(0.01)
        i = i + 1
    end = time.time()
    print(end - start)

    
except Exception as other:
    print('Error in USB Reader: ' + str(other))
    if (str(other) == '[Errno 13] Access denied (insufficient permissions)'):
        print('Execute the following: sudo python3 pyHWLink_USB_Reader.py')
        
    
