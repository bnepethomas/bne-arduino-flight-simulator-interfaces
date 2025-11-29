#!/usr/bin/python

# NEED to CHECK IF Arduino Switch and led stack start from 0 !!!!

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
UDP_IP_ADDRESS = "172.16.1.2"
UDP_PORT_NO = 7792
# Input Test
UDP_IP_ADDRESS = "172.16.1.2"
UDP_PORT_NO = 26027

ValuesChanged = False

serverSock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
serverSock.settimeout(0.3)
serverSock.bind((UDP_IP_ADDRESS, UDP_PORT_NO))


OutputState = []
OutputState1 = []
OutputState2 = []
PreviouslyHighOutputState = []
PreviouslyHighOutputState1 = []
PreviouslyHighOutputState2 = []
# Initialise output array over dimensioning it a little

ArraySize = 177
for OutputPtr in range(0,ArraySize):
    OutputState.append(0)
    OutputState1.append(0)
    OutputState2.append(0)
    PreviouslyHighOutputState.append(0)
    PreviouslyHighOutputState1.append(0)
    PreviouslyHighOutputState2.append(0)

  


root = Tk()
# width x height + x_offset + y_offset:
#root.geometry("700x200+30+30")
root.wm_title("pyHWLink Lamp Output Emulator")

canvas = Canvas(root, width=1800, height=520)
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

            # If last character is a CR - trim it out
            if (ord( ReceivedUDPString[len(ReceivedUDPString) -1]) == 10):
                 logging.debug("Trimming off trailing CR")
                 ReceivedUDPString = ReceivedUDPString[:-1]   
            

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

                # For a lamp output we are looking for two values eg 02:1
                # For a switch output we are looking for three values 02:000:1


                        
                if len(workingFields) == 2:

                    if str(workingFields[1]) != '0' and str(workingFields[1]) != '1':
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

                # Work on outputs from Input Devices which pass three values
                elif len(workingFields) == 3:
                    logging.debug('Processing Input from Input Modules 3 Variable')
                    logging.debug('Stage 2 Processing: ' + str(workingFields))

                    try:
                        workingKey = workingFields[1]
                        logging.debug('Working key is: ' + workingKey)

                        majorarraypointer = int(workingFields[0])


                        if majorarraypointer == 0:
                            if OutputState[int(workingKey)] != int(workingFields[2]):
                                ValuesChanged = True
                                OutputState[int(workingKey)] = int(workingFields[2])

                                # Store spots where location has been high
                                if OutputState[int(workingKey)] == 1:
                                    PreviouslyHighOutputState[int(workingKey)] = 1


                        if majorarraypointer == 1:
                            if OutputState1[int(workingKey)] != int(workingFields[2]):
                                ValuesChanged = True
                                OutputState1[int(workingKey)] = int(workingFields[2])

                                # Store spots where location has been high
                                if OutputState1[int(workingKey)] == 1:
                                    PreviouslyHighOutputState1[int(workingKey)] = 1


                        if majorarraypointer == 2:
                            if OutputState2[int(workingKey)] != int(workingFields[2]):
                                ValuesChanged = True
                                OutputState2[int(workingKey)] = int(workingFields[2])

                                # Store spots where location has been high
                                if OutputState2[int(workingKey)] == 1:
                                    PreviouslyHighOutputState2[int(workingKey)] = 1
                                

                    except Exception as other:
                        logging.critical("Error in ProcessReceivedString: " + str(other))
                    
                else:
                    logging.debug('')
                    logging.debug('WARNING - There are an incorrect number of fields in: ' + str(workingFields))
                    logging.debug('')
                
                

 

            if (ValuesChanged == True):
                logging.debug('Updating Canvas')
                
                canvas.delete(ALL)
                xoffset = 0
                for OutputPtr in range(0,ArraySize - 1):
                    x = OutputPtr % 11
                    y = OutputPtr // 11
                    if (OutputState[OutputPtr+1] == 1):
                        canvas.create_rectangle(xoffset + 50 * x, 30 + y * 30, xoffset + 52 + 50 * x, 62 + y * 30, fill='red')
                        canvas.create_text(xoffset + 26 + 50 * x, 45 + y * 30, text=OutputPtr + 1,fill="black")
                        
                    elif (PreviouslyHighOutputState[OutputPtr+1] == 1):
                        canvas.create_rectangle(xoffset + 50 * x, 30 + y * 30, xoffset + 52 + 50 * x, 62 + y * 30, fill='orange')
                        canvas.create_text(xoffset + 26 + 50 * x, 45 + y * 30, text=OutputPtr + 1)
                    else:
                        canvas.create_rectangle(xoffset + 50 * x, 30 + y * 30, xoffset + 52 + 50 * x, 62 + y * 30, fill='black')
                        canvas.create_text(xoffset + 26 + 50 * x, 45 + y * 30, text=OutputPtr + 1,fill="yellow")
                

                xoffset = 600
                for OutputPtr in range(0,ArraySize - 1):
                    x = OutputPtr % 11
                    y = OutputPtr // 11
                    if (OutputState1[OutputPtr+1] == 1):
                        canvas.create_rectangle(xoffset + 50 * x, 30 + y * 30, xoffset + 52 + 50 * x, 62 + y * 30, fill='red')
                        canvas.create_text(xoffset + 26 + 50 * x, 45 + y * 30, text=OutputPtr + 1,fill="black")
                        
                    elif (PreviouslyHighOutputState1[OutputPtr+1] == 1):
                        canvas.create_rectangle(xoffset + 50 * x, 30 + y * 30, xoffset + 52 + 50 * x, 62 + y * 30, fill='orange')
                        canvas.create_text(xoffset + 26 + 50 * x, 45 + y * 30, text=OutputPtr + 1)
                    else:
                        canvas.create_rectangle(xoffset + 50 * x, 30 + y * 30, xoffset + 52 + 50 * x, 62 + y * 30, fill='black')
                        canvas.create_text(xoffset + 26 + 50 * x, 45 + y * 30, text=OutputPtr + 1,fill="yellow")         


                xoffset = 1200
                for OutputPtr in range(0,ArraySize - 1):
                    x = OutputPtr % 11
                    y = OutputPtr // 11
                    if (OutputState2[OutputPtr+1] == 1):
                        canvas.create_rectangle(xoffset + 50 * x, 30 + y * 30, xoffset + 52 + 50 * x, 62 + y * 30, fill='red')
                        canvas.create_text(xoffset + 26 + 50 * x, 45 + y * 30, text=OutputPtr + 1,fill="black")
                        
                    elif (PreviouslyHighOutputState2[OutputPtr+1] == 1):
                        canvas.create_rectangle(xoffset + 50 * x, 30 + y * 30, xoffset + 52 + 50 * x, 62 + y * 30, fill='orange')
                        canvas.create_text(xoffset + 26 + 50 * x, 45 + y * 30, text=OutputPtr + 1)
                    else:
                        canvas.create_rectangle(xoffset + 50 * x, 30 + y * 30, xoffset + 52 + 50 * x, 62 + y * 30, fill='black')
                        canvas.create_text(xoffset + 26 + 50 * x, 45 + y * 30, text=OutputPtr + 1,fill="yellow")         


    except Exception as other:
        logging.critical("Error in ProcessReceivedString: " + str(other))




def tick():

    

    # Count down timer - probably not needed to final code
    global timer
    timer += 1

    # display timer value
    canvas.create_rectangle(1, 1, 380, 18, fill='white',outline='white')
    canvas.create_text(120, 10, text=datetime.datetime.now().strftime("%H:%M:%S") + " " + UDP_IP_ADDRESS + ":" + str(UDP_PORT_NO) )

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


