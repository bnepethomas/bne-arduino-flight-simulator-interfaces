#!/usr/bin/python

# pyHWLink_Lamp_Output_Emulator.py

# Simple framework which provides a GUI to emulate an output module
# Given the Max7219 supports 64 Leds, the program will support 64 outputs

# Uses same protocol as most of the pyHWLink family,
#    Data payload is preceeded by a D



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


OutputState = []
# Initialise output array over dimensioning it a little
for OutputPtr in range(0,65):
    OutputState.append(0)

  


root = Tk()
# width x height + x_offset + y_offset:
#root.geometry("700x200+30+30")
root.wm_title("pyHWLink Lamp Output Emulator")

canvas = Canvas(root, width=420, height=260)
canvas.pack()
    
timer = 5

def ProcessReceivedString(ReceivedUDPString):
    global input_assignments
    global send_string
    global learning
    
    logging.debug('Processing UDP String')

    send_string = ""
    
    try:
        if len(ReceivedUDPString) > 0 and ReceivedUDPString[0] == 'D':
            
            logging.debug('Stage 1 Processing: ' + str(ReceivedUDPString))
            # Remove leading D
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

                
                if len(workingFields) != 3:
                    logging.warn('')
                    logging.warn('WARNING - There are an incorrect number of fields in: ' + str(workingFields))
                    logging.warn('')
                elif str(workingFields[2]) != '0' and str(workingFields[2]) != '1':
                    logging.warn('')
                    logging.warn('WARNING - Invalid 3rd parameter: ' + str(workingFields[2]))
                    logging.warn('')                   
                else:
                    logging.debug('Stage 2 Processing: ' + str(workingFields))

                    try:
                        workingkey = workingFields[0] + ':' + workingFields[1]
                        logging.debug('Working key is: ' + workingkey)
                        
                        logging.debug('Working Fields for working key are: ' +
                              str(input_assignments[workingkey]))

                        logging.debug('The value is: ' +
                              str(input_assignments[workingkey]['Description']))


                        # Lamp is On
                        if str(workingFields[2]) == '1':

                                                          

                        # Lamp is Off
                        if str(workingFields[2]) == '0':

                                
    
                    except Exception as other:
                        logging.critical('')
                        logging.critical('WARNING - Unable to read record of interest in ProcessReceivedString')
                        logging.critical('WARNING - Record name is: "' + workingkey + '"')
                        logging.critical('')
                        logging.critical("Error in ProcessReceivedString: " + str(other))
                

            Send_Remaining_Commands()
            logging.debug('Continuing on')
            


    except Exception as other:
        logging.critical("Error in ProcessReceivedString: " + str(other))




def tick():

    canvas.delete(ALL)

    # Count down timer - probably not needed to final code
    global timer
    timer -= 1

    # display timer value    
    canvas.create_text(10, 10, text=timer)

    print(timer)

    try:
        data, addr = serverSock.recvfrom(1500)
        print('Received Something')
        ReceivedPacket = data.decode('utf-8')
        logging.debug("Message: " + ReceivedPacket)
        print('Packet Received')
        time.sleep(5)


    except socket.timeout:
        x=0
        print('timeout')

    except Exception as other:
        logging.critical("Error in tick: " + str(other))   

    for x in range(0,8):
        for y in range(0,8):
            if (random.randint(0,1) == 1):   
                canvas.create_rectangle(50 * x, 30 + y * 30, 52 + 50 * x, 62 + y * 30, fill='red')
            else:
                canvas.create_rectangle(50 * x, 30 + y * 30, 52 + 50 * x, 62 + y * 30, fill='black')
    if time == 0:
        canvas.destroy()
        root.destroy()
        root.quit()    
    else:
        canvas.after(100, tick)
        
canvas.after(1, tick)   

root.mainloop()


