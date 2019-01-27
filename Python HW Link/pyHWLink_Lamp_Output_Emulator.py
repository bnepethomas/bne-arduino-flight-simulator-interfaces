#!/usr/bin/python

# pyHWLink_Lamp_Output_Emulator.py

# Simple framework which provides a GUI to emulate an output module
# Given the Max7219 supports 64 Leds, the program will support 64 outputs



from tkinter import *
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


UDP_IP_ADDRESS = "127.0.0.1"
UDP_PORT_NO = 7791


serverSock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
serverSock.settimeout(0.0001)
serverSock.bind((UDP_IP_ADDRESS, UDP_PORT_NO))



root = Tk()
# width x height + x_offset + y_offset:
#root.geometry("700x200+30+30")
root.wm_title("pyHWLink Lamp Output Emulator")

canvas = Canvas(root, width=420, height=260)
canvas.pack()

for x in range(0,8):
    for y in range(0,8):
        canvas.create_rectangle(50 * x, y * 30, 52 + 50 * x, 32 + y * 30, fill='red')



root.mainloop()
