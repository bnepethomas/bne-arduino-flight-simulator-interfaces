#!/usr/bin/python

# pyHWLink_Lamp_Output_Emulator.py

# Simple framework which provides a GUI to emulate an output module
# Given the Max7219 supports 64 Leds, the program will support 64 outputs

# Uses same protocol as most of the pyHWLink family,
#    Data payload is preceeded by a D

# 20190916 Added check to catch CR on single value updates - was reporting error. Added Text to Canvas



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
import datetime




logging.basicConfig(format='%(asctime)s:%(levelname)s:%(message)s',level=logging.INFO)
#logging.basicConfig(format='%(asctime)s:%(levelname)s:%(message)s',level=logging.DEBUG)
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


UDP_IP_ADDRESS = "192.168.1.138"
UDP_IP_ADDRESS = "192.168.1.102"
UDP_PORT_NO = 7792

ValuesChanged = False

serverSock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
serverSock.settimeout(0.3)
serverSock.bind((UDP_IP_ADDRESS, UDP_PORT_NO))


OutputState = []
# Initialise output array over dimensioning it a little
for OutputPtr in range(0,65):
    OutputState.append(0)

  


root = Tk()
# width x height + x_offset + y_offset:
#root.geometry("700x200+30+30")
root.wm_title("pyHWLink Lamp Output Emulator")

canvas = Canvas(root, width=400, height=270)
canvas.pack()
    
timer = 0

def ProcessReceivedString(ReceivedUDPString):
    global input_assignments
    global send_string
    global learning
    global ValuesChanged
    
    logging.debug('Processing UDP String')

    send_string = ""
    # If a value has changed then update the screen other move on
    ValuesChanged = False
    
    try:
        if len(ReceivedUDPString) > 0 and ReceivedUDPString[0] == 'D':

            timer = 0
 
            logging.debug('Stage 1 Processing: ' + str(ReceivedUDPString))
            # Remove leading D,
            ReceivedUDPString = str(ReceivedUDPString[1:])
            logging.debug('Checking for correct format :')

            

            workingSets =''
            workingSets = ReceivedUDPString.split(',')
            logging.debug('There are ' + str(len(workingSets)) + ' records')
            counter = 0
            for workingRecords in workingSets:
                logging.debug('Record workingRecord number ' + str(counter) + ' ' +
                      workingRecords)
                counter = counter + 1
                

                workingFields = ''
                workingFields = workingRecords.split(':')

                # Try and cleanup trailing CRLF
                if len(workingFields) == 2:
                    workingFields[1] = workingFields[1][0]
                        
                if len(workingFields) != 2:
                    logging.debug('')
                    logging.debug('WARNING - There are an incorrect number of fields in: ' + str(workingFields))
                    logging.debug('')
                elif str(workingFields[1]) != '0' and str(workingFields[1]) != '1':
                    logging.warn('')
                    logging.warn('WARNING - Invalid 2nd parameter: ' + str(workingFields[1]))
                    logging.warn('')                   
                else:
                    logging.debug('Stage 2 Processing: ' + str(workingFields))

                    try:
                        workingKey = workingFields[0]
                        logging.debug('Working key is: ' + workingKey)
                        

                        if OutputState[int(workingKey)] != int(workingFields[1]):
                            ValuesChanged = True
                            OutputState[int(workingKey)] = int(workingFields[1]  )  
                                
    
                    except Exception as other:
                        logging.critical("Error in ProcessReceivedString: " + str(other))
                

 

            if (ValuesChanged == True):
                logging.debug('Updating Canvas')
                
                canvas.delete(ALL)
                for OutputPtr in range(0,64):
                    x = OutputPtr % 8
                    y = OutputPtr // 8
                    if (OutputState[OutputPtr+1] == 1):
                        canvas.create_rectangle(50 * x, 30 + y * 30, 52 + 50 * x, 62 + y * 30, fill='red')
                        canvas.create_text(26 + 50 * x, 45 + y * 30, text=OutputPtr + 1)
                        
                    else:
                        canvas.create_rectangle(50 * x, 30 + y * 30, 52 + 50 * x, 62 + y * 30, fill='black')
                        canvas.create_text(26 + 50 * x, 45 + y * 30, text=OutputPtr + 1,fill="yellow")
                
                    
                    
            


    except Exception as other:
        logging.critical("Error in ProcessReceivedString: " + str(other))




def tick():

    

    # Count down timer - probably not needed to final code
    global timer
    timer += 1

    # display timer value
    canvas.create_rectangle(1, 1, 40, 18, fill='white',outline='white')
    canvas.create_text(20, 10, text=timer)

    try:
        data, addr = serverSock.recvfrom(1500)
        ReceivedPacket = data.decode('utf-8')
        logging.debug("Message: " + ReceivedPacket)
        ProcessReceivedString(ReceivedPacket)


    except socket.timeout:
        print('timeout')

    except Exception as other:
        logging.critical("Error in tick: " + str(other))   


    if time == 0:
        canvas.destroy()
        root.destroy()
        root.quit()    
    else:
        canvas.after(1, tick)
        
canvas.after(1, tick)   

root.mainloop()


