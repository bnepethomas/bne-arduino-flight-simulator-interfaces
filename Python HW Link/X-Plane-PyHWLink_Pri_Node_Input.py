#!/usr/bin/python

# X-Plane-PyHWLink_Pri_Node_Input.py
# Based on PyHWLink_Pri_Node_Input.py - split 20190102
#    Will keep the same receive ports as original code as there
#        should only be a single Sim running at a point in time

# Receives AV pairs from input devices and translates to datasets for the sim
#


from collections import OrderedDict
import json
import logging
import os
import socket
import sys
import time
import struct


from optparse import OptionParser

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




debugging = False

# See if input configuration file exists
# Parameters are either specified in this file or passed via command line
# Command line parameters override any settings in the config file
# 
if not (os.path.isfile('X-Plane-input_config.py')):
    
    print('Unable to find X-Plane-input_config.py')
    
else:
    try:
        from input_config import *
        
        print('Learning Mode: ' + str(learning))
        print('Aircraft is: ' + AircraftType)


    except Exception as other:
        logging.critical("Unexpected error " + str(other))             
        print('Unable to open X-Plane-input_config.py')
        print('Or variable assignment incorrect - forgot quotes for string?')
        print('Defaults used')
    




# See if value is assigned.  First we checked config file and then
#   command line arguments

logging.debug("Checking Command Line parameters")


parser = OptionParser()
parser.add_option("-l","--learning", dest="optionLearning",action="store_true",
                  default=False,
                  help="learning mode, assign actions to switch movements")
(options, args) = parser.parse_args()

logging.debug("options:" + str(options))
logging.debug("arguments:" +  str(args))

# Override Learning mode if passed via command line
if options.optionLearning == True:
    learning = True
    print('Learning Mode enabled through command line')
   

    
    

try:
    test = AircraftType
except:
    print('AircraftType not assigned - using defaults')
    AircraftType = 'default'


UDP_IP_ADDRESS = "127.0.0.1"
# Windows was unable tobind to 0 - checking firewall
#UDP_IP_ADDRESS = "0"
UDP_PORT_NO = 26027
#XPlane_IP_ADDRESS = "127.0.0.1"
XPlane_IP_ADDRESS = "192.168.1.138"
XPlane_Port_No = 49000
KeyStroke_Sender_Port_No = 7790

UDP_Reflector_IP = "127.0.0.1"
UDP_Reflector_Port = 27000

serverSock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
serverSock.settimeout(0.0001)
serverSock.bind((UDP_IP_ADDRESS, UDP_PORT_NO))

# Changed reflect Xplane - only a single set of bindings are used for all aircraft
#
input_assignments_file = 'X-Plane-input_assignments.json'
temp_input_assignments_file = 'X-Plane-temp_input_assignments.json'


input_assignments = None
send_string = None

def ReceivePacket():

    # a is used to track the number of timeouts between packets
    # throws a keepalive message to indicate we are still alive
    a=0
    while True:
        
        try:
            data, addr = serverSock.recvfrom(1500)
            
            ReceivedPacket = data.decode('utf-8')
            logging.debug("Message: " + ReceivedPacket)
            ProcessReceivedString(str(ReceivedPacket))
            

            a=0

                                              
        except socket.timeout:
            a=a+1
            if (a > 1000):
                logging.info("Long Receive Timeout")
                a=0
            continue

        
        except Exception as other:
            logging.critical("Error in ReceivePacket: " + str(other)) 

 

def save_and_reload_assignments():
    # Save out to a temporary file and reload to ensure it is in shape
    global input_assignments

    
    try:
        

        json.dump(input_assignments, fp=open(temp_input_assignments_file,'w'),indent=4,sort_keys=True)

        input_assignments = None

        input_assignments = json.load(open(temp_input_assignments_file))
        print('Configuration saves changes saves to ' + temp_input_assignments_file)
        print('To persist changes copy ' + temp_input_assignments_file + ' to ' +
              input_assignments_file)
        print('')

    except Exception as other:
        logging.critical("Error in save_and_reload_assignments': " + str(other))

        
  
    
    

def updateDescription(workingkey):
    global input_assignments

    
    print('In learning mode - time to update the description for: '  + str(workingkey))
    updaterecord = input('Update Description? [y/n]: ')
    if updaterecord.upper() == 'Y':

        try:
            wrkstring = input('Please provide a description for: "' + str(workingkey) +  '" ')
            if wrkstring != '':

                input_assignments[workingkey]['Description'] = wrkstring

                save_and_reload_assignments()
                
        except Exception as other:
            logging.critical("Error in updateDescription': " + str(other))




def updateOpenAction(workingkey):
    global input_assignments

    
    print('In learning mode - time to update the Open Action for: ' + str(workingkey) + ' / '
          + input_assignments[workingkey]['Description'] + ' ' )
    updaterecord = input('Update Action? [y/n]: ')
    if updaterecord.upper() == 'Y':

        try:
            wrkstring = input('Please provide a Open Action for: "' + str(workingkey) +  '" "'
                                  + input_assignments[workingkey]['Description'] + '" :')

            input_assignments[workingkey]['UDPOpen'] = wrkstring

            save_and_reload_assignments()
                
        except Exception as other:
            logging.critical("Error in updateOpenAction: " + str(other))



def updateCloseAction(workingkey):
    global input_assignments

    
    print('In learning mode - time to update the Close Action for: ' + str(workingkey) + ' '
          + input_assignments[workingkey]['Description'])
    updaterecord = input('Update Action? [y/n]: ')
    if updaterecord.upper() == 'Y':
        
        try:
            wrkstring = input('Please provide a Close Action for: "' + str(workingkey) +  '" "'
                                  + input_assignments[workingkey]['Description'] + '" :')

            input_assignments[workingkey]['UDPClose'] = wrkstring

            save_and_reload_assignments()
                
        except Exception as other:
            logging.critical("Error in updateCloseAction: " + str(other))
                

 

def Send_Value():

    global send_string
    global serverSock


    try:

 

        logging.debug("UDP target port:" + str(XPlane_Port_No))
        logging.debug('send_string is ' + send_string)
        logging.debug('Length of Send String is ' + str(len(send_string)))
        
        # X-Plane Specific
        
        values = ('CMND'.encode('utf-8'), 0, send_string.encode('utf-8'))
        packer = struct.Struct('4s B ' + str(len(send_string)) + 's')
        packed_data = packer.pack(*values)      


        sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM) # UDP
        sock.sendto((packed_data), (XPlane_IP_ADDRESS, XPlane_Port_No))


        serverSock.sendto((packed_data), (UDP_Reflector_IP, UDP_Reflector_Port))

        # X-Plane Specific

        send_string = ""

    except Exception as other:
        logging.critical("Error in Send_Value: " + str(other))
        

def SendKeyStroke(KeyStrokes_To_Send):

    try:

 

        logging.debug("UDP target port:" + str(KeyStroke_Sender_Port_No))
        logging.debug('send_string is ' + KeyStrokes_To_Send)
        logging.debug('Length of Send String is ' + str(len(KeyStrokes_To_Send)))
        

        
        values = (KeyStrokes_To_Send.encode('utf-8'))
    


        sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM) # UDP
        sock.sendto((values), (XPlane_IP_ADDRESS, KeyStroke_Sender_Port_No))


        serverSock.sendto((values), (UDP_Reflector_IP, UDP_Reflector_Port))





    except Exception as other:
        logging.critical("Error in SendKeyStroke: " + str(other))
  
  
  

def addValueToSend(valueToAdd):

    global send_string
              

    try:
        logging.debug('Send String is ' + str(len(send_string)) + ' characters long')
        logging.debug('Before ' + send_string)


        if len(send_string) == 0:
            send_string = valueToAdd
        else:
            send_string = send_string + ',' + valueToAdd
             
        if len(send_string) > 40:
            
            logging.debug('Send String Now !!!!!')
            Send_Value()
            
        
        logging.debug('After ' + send_string)

    except Exception as other:
        logging.critical("Error in addValueToSend: " + str(other))


def Send_Remaining_Commands():
    
    global send_string

    if send_string != '':
         Send_Value()       
    send_string = ''
    


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

                        # Check to see if learning and description is blank

                        if learning and input_assignments[workingkey]['Description'] == None:
                                updateDescription(workingkey)
                        print('Value for Description is : ' +
                              str (input_assignments[workingkey]['Description']))


                        # Switch is Closed
                        if str(workingFields[2]) == '1':
                            if learning and input_assignments[workingkey]['UDPClose'] == None:
                                updateCloseAction(workingkey)

                            # Check both UDP and Keyboard Fields    
                            print('Value for UDPClose is : ' +
                              str (input_assignments[workingkey]['UDPClose']))
                            if input_assignments[workingkey]['UDPClose'] != None:
                                addValueToSend(str (input_assignments[workingkey]['UDPClose']))
                                
                            print('Value for KeyboardClose is : ' +
                              str (input_assignments[workingkey]['KeyboardClose']))
                            if input_assignments[workingkey]['KeyboardClose'] != None:
                                print('*********************')
                                print('Add keyboard close send code here!')
                                SendKeyStroke(input_assignments[workingkey]['KeyboardClose'])
                                
                                

                        # Switch is Opened
                        if str(workingFields[2]) == '0':
                            if learning and input_assignments[workingkey]['UDPOpen'] == None:
                                updateOpenAction(workingkey)
                            print('Value for UDPOpen is : ' +
                                  str (input_assignments[workingkey]['UDPOpen']))
                            if input_assignments[workingkey]['UDPOpen'] != None:
                                addValueToSend(str (input_assignments[workingkey]['UDPOpen']))
                            
                            print('Value for KeyboardOpen is : ' +
                                str (input_assignments[workingkey]['KeyboardOpen']))
                            if input_assignments[workingkey]['KeyboardOpen'] != None:
                                print('*********************')
                                print('Add keyboard open send code here!')
                                SendKeyStroke(input_assignments[workingkey]['KeyboardOpen'])
                                

                                
    
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


  
def RemoveUnwantedCharacters(stringToBeCleaned):
    stringToBeCleaned= stringToBeCleaned.replace('"', '')
    stringToBeCleaned = stringToBeCleaned.strip(' ')
    return(stringToBeCleaned)


def LoadDCSParameterFile():
    # reads in a json like file from DCS which holds commands
    # this is a pre DCS 1.5 format

    # Test script loading LUA file (which contains joystick commands) into an Dictionary (actually a dictionary of dictionaries)
    # Only loads entries which have a name tag and are on a single line enclosed by { and } 


    
    input_file = 'testinputdata.csv'


    # Only load a small number of devices
    #deviceToFind = 'breaker'
    
    # Load all possible devices
    deviceToFind = ''  # Lets load everything that we can

    # Empty dictionary
    myDict = {}

    if not (os.path.isfile(input_file)):
        
        print('Unable to find ' + input_file)
        
    else:
        file_object = open(input_file, 'r')
        count = 0
        myline = file_object.readline() 
        while myline != "":
            count = count + 1
            if myline.find(deviceToFind) >= 0 and myline.find("{") >= 0 and myline.find('}') >= 0:


                logging.debug("Processing " +  myline)

                # Initialise list
                mylist = []
                
                
                # Strip out leading and trailing '#'
                myline = myline[ myline.find("{") + 1:]
                myline = myline[: myline.find("}")]
                
                logging.debug("Removed # " + myline[:])

                # Split into attrib var pairs
                mysplit = myline.split(',')
                
                logging.debug(mysplit)

                
                for i in mysplit:

                    logging.debug("Cleaning up " + i)
                    mysplit1 = i.split('=')
                    logging.debug(mysplit1)

                    if len(mysplit1) == 2:
                        logging.debug('Have correct number of attributes')

                        # Remove unwanted characters
                        s = RemoveUnwantedCharacters(mysplit1[0])
                        t = RemoveUnwantedCharacters(mysplit1[1])

                        mylist.append([s,t])

                        if debugging and s == 'name':
                            logging.debug('Found a name: ' + t )


                # Now have AV pairs in a list - time to see if one of them is name

                innerDict = {}
                for s in mylist:
                    innerDict[s[0]] = s[1]
                logging.debug('The end result is ')
                logging.debug(mylist)

                
                for s in mylist:
                    if s[0] == 'name':
                        logging.debug('We have a valid name - time to add a item to the dictionary')

                        for t in mylist:
                            if t[0] == 'down':
                        

                                myDict[s[1]] = innerDict
                                
                                logging.debug('myDict is now ' + str(len(myDict)))

                                                                       
                    
                    
                
            myline = file_object.readline()

            
        print('Have read ' + str(count) + ' lines')
        print('Have loaded ' + str(len(myDict)) + ' entries')
        
        file_object.close()

        logging.debug(myDict)

        try:

            itemOfInterest = 'CB Generator CONT Left'
            fieldOfInterest = 'down'

            
            print('Complete Record for: ' + itemOfInterest)
            print(myDict[itemOfInterest])
            print('Specific Field (' + fieldOfInterest + ')' )
            
            print(myDict[itemOfInterest][fieldOfInterest])

            S = json.dumps(myDict)
            json.dump(myDict, fp=open('testjson.txt','w'),indent=4)
            print('File Exported')
            
        except Exception as other:
            logging.critical("Error in LoadDCSParameterFile" + str(other))         
            logging.critical('Unable to read record of interest')
            logging.critical('Record name is: "' + itemOfInterest + '", Field is: "' + fieldOfInterest + '"')

            

    

# Here we receive packets containing switch transitions
# Maximum inputs per module is 256
# Unless explicitly asked via a command request, the sending unit only sends deltas
# The format is AV pairs indicating new switch status, eg a switch moving to the off position will reflect as switch_no:0
# Packets from input nodes will have the format of DX:A=V:A1=V1, where X is the input node number.
# The Node number is only indicated in the front of the packet, not at the individual AV pair.

# If an AV pair is not known it will be silently discarded unless the Primary node is in learning mode.
# If in learning mode it will ask the operator what task should be assigned to the unknown AV pair. Switch description will
#   also be validated.

# There will be a generic input file, and aircraft specific files.Still to be determined how aircraft type is selected, ideally
#   this would be dynamic - which may mean receiving a trigger, most likely from the output driver as it is receiving packets
#   from the simulator.  An approach may be to allow the output code to sense aircraft change and if aircraft type does change
#   then kill existing input process and launch new input process with command line parameters.

# Dict should contain
#   1: UniqueSwitchID (Key) (NodeNumber:SwitchID)
#   2: SwitchDescription
#   3: OnSwitchAction
#   4: OffSwitchAction


# Empty dictionary
dictInputAssignments = {}



# Check to see if input assignment files exists - if not create it

if not (os.path.isfile(input_assignments_file)):
    
    logging.critical('Unable to find "' + input_assignments_file + '"')
    logging.info('Creating default Input Assignments in: ' + input_assignments_file)
    
    dictOuter = {}
    dictInner = {}
    outercounter = 0
    while outercounter < 3:
        counter = 0
        while counter < 256:
            dictInner = {}
            dictInner['Description'] = None
            dictInner['UDPOpen'] = None
            dictInner['UDPClose'] = None
            dictInner['KeyboardOpen'] = None
            dictInner['KeyboardClose'] = None
            
            #dictOuter[str(outercounter) + ":" + str(counter)] = dictInner
            dictOuter[ '%.2d' % (outercounter) + ":" + '%.3d' % (counter)] = dictInner
            counter = counter + 1

            
        outercounter = outercounter + 1

    json.dump(dictOuter, fp=open(input_assignments_file,'w'),indent=4)

    logging.info('Created Input Assignments file: "' + input_assignments_file + '"')


#        
print('Loading Input Assignments from: "' + input_assignments_file +'"')              

try:
    input_assignments = json.load(open(input_assignments_file))
                             
except Exception as other:
    logging.critical("Unexpected error while reading file:" + str(other))         

    serverSock.close()
    sys.exit(0)


##try:
##    print('Adding new fields')
##    for key, value in input_assignments.items():
##        print(key, value)
##        value['KeyboardClose'] = None
##        value['KeyboardOpen'] = None
##        print(key, value)
##
##    save_and_reload_assignments()
                             
##except Exception as other:
##    logging.critical("Unexpected error while adding new fields to file:" + str(other))    

try:
    print('Waiting for packet')
    ReceivePacket()

except KeyboardInterrupt:
    # Catch Ctl-C and quit
    print('Exiting')
    serverSock.close()
    sys.exit(0)

except Exception as other:
    logging.critical("Error in main:" + str(other))         

 


serverSock.close()
