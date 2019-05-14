#!/usr/bin/python

# PyHWLink_Pri_Node_Input.py

# Receives AV pairs from input devices and translates to AV pairs for the sim


from collections import OrderedDict
import json
import logging
import os
import socket
import sys
import time



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
if not (os.path.isfile('input_config.py')):
    
    print('Unable to find ' + input_file)
    
else:
    try:
        from input_config import *
        
        print('Learning Mode: ' + str(learning))
        print('Aircraft is: ' + AircraftType)


    except Exception as other:
        logging.critical("Unexpected error " + str(other))             
        print('Unable to open input_config.py')
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

# Windows was unable tobind to 0 - checking firewall
#UDP_IP_ADDRESS = "127.0.0.1"
#  When running Pi use address of 0 - as it listens to traffic from everything
UDP_IP_ADDRESS = "0"
UDP_PORT_NO = 26027
SIM_API_IP_ADDRESS = "172.16.1.3"
SIM_API_PORT_NO = 7791
SIM_KB_IP_ADDRESS = "172.16.1.3"
SIM_KB_PORT_NO = 7790

UDP_Reflector_IP = "127.0.0.1"
UDP_Reflector_Port = 27000

serverSock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
serverSock.settimeout(0.0001)
serverSock.bind((UDP_IP_ADDRESS, UDP_PORT_NO))

input_assignments_file = 'input_assignments.json'
temp_input_assignments_file = 'temp_input_assignments.json'


input_assignments = None
API_send_string = None
KB_send_string = None

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
    updaterecord = raw_input('Update Description? [y/n]: ')
    if updaterecord.upper() == 'Y':

        try:
            wrkstring = raw_input('Please provide a description for: "' + str(workingkey) +  '" ')
            if wrkstring != '':

                input_assignments[workingkey]['Description'] = wrkstring

                save_and_reload_assignments()
                
        except Exception as other:
            logging.critical("Error in updateDescription': " + str(other))




def updateAPIOpenAction(workingkey):
    global input_assignments

    
    print('In learning mode - time to update the API Open Action for: ' + str(workingkey) + ' / '
          + input_assignments[workingkey]['Description'] + ' ' )
    updaterecord = raw_input('Update API Action? [y/n]: ')
    if updaterecord.upper() == 'Y':

        try:
            wrkstring = raw_input('Please provide an API Open Action for: "' + str(workingkey) +  '" "'
                                  + input_assignments[workingkey]['Description'] + '" :')

            input_assignments[workingkey]['API_Open'] = wrkstring

            save_and_reload_assignments()
                
        except Exception as other:
            logging.critical("Error in updateAPIOpenAction: " + str(other))

def updateKBOpenAction(workingkey):
    global input_assignments

    
    print('In learning mode - time to update the KB Open Action for: ' + str(workingkey) + ' / '
          + input_assignments[workingkey]['Description'] + ' ' )
    updaterecord = raw_input('Update KB Action? [y/n]: ')
    if updaterecord.upper() == 'Y':

        try:
            wrkstring = raw_input('Please provide an KB Open Action for: "' + str(workingkey) +  '" "'
                                  + input_assignments[workingkey]['Description'] + '" :')

            input_assignments[workingkey]['KB_Open'] = wrkstring

            save_and_reload_assignments()
                
        except Exception as other:
            logging.critical("Error in updateKBOpenAction: " + str(other))

def updateAPICloseAction(workingkey):
    global input_assignments

    
    print('In learning mode - time to update the API Close Action for: ' + str(workingkey) + ' '
          + input_assignments[workingkey]['Description'])
    updaterecord = raw_input('Update API Action? [y/n]: ')
    if updaterecord.upper() == 'Y':
        
        try:
            wrkstring = raw_input('Please provide an API Close Action for: "' + str(workingkey) +  '" "'
                                  + input_assignments[workingkey]['Description'] + '" :')

            input_assignments[workingkey]['API_Close'] = wrkstring

            save_and_reload_assignments()
                
        except Exception as other:
            logging.critical("Error in updateAPICloseAction: " + str(other))
                

def updateKBCloseAction(workingkey):
    global input_assignments

    
    print('In learning mode - time to update the KB Close Action for: ' + str(workingkey) + ' '
          + input_assignments[workingkey]['Description'])
    updaterecord = raw_input('Update KB Action? [y/n]: ')
    if updaterecord.upper() == 'Y':
        
        try:
            wrkstring = raw_input('Please provide an KB Close Action for: "' + str(workingkey) +  '" "'
                                  + input_assignments[workingkey]['Description'] + '" :')

            input_assignments[workingkey]['KB_Close'] = wrkstring

            save_and_reload_assignments()
                
        except Exception as other:
            logging.critical("Error in updateKBCloseAction: " + str(other)) 

def API_Send_Value():

    global API_send_string
    global serverSock


    try:

        logging.debug("UDP target port:" + str(SIM_API_PORT_NO))

        serverSock.sendto(API_send_string.encode('utf-8'), (SIM_API_IP_ADDRESS, SIM_API_PORT_NO))
        serverSock.sendto(API_send_string.encode('utf-8'), (UDP_Reflector_IP, UDP_Reflector_Port))

        API_send_string = ""

    except Exception as other:
        logging.critical("Error in API_Send_Value: " + str(other))
        

def KB_Send_Value():

    global KB_send_string
    global serverSock


    try:

        logging.debug("UDP target port:" + str(SIM_KB_PORT_NO))

        serverSock.sendto(KB_send_string.encode('utf-8'), (SIM_KB_IP_ADDRESS, SIM_KB_PORT_NO))
        serverSock.sendto(KB_send_string.encode('utf-8'), (UDP_Reflector_IP, UDP_Reflector_Port))

        KB_send_string = ""

    except Exception as other:
        logging.critical("Error in KB_Send_Value: " + str(other))
              

def addAPIValueToSend(valueToAdd):

# Currently allowing building out of longer string
# This may be introducing latency will monitor and may use direct send

    global API_send_string
              

    try:
        logging.debug('Send String is ' + str(len(API_send_string)) + ' characters long')
        logging.debug('Before ' + API_send_string)


        if len(API_send_string) == 0:
            API_send_string = valueToAdd
        else:
            API_send_string = API_send_string + ',' + valueToAdd
             
        if len(API_send_string) > 40:
            
            logging.debug('Send String Now !!!!!')
            API_Send_Value()
            
        
        logging.debug('After ' + API_send_string)

    except Exception as other:
        logging.critical("Error in addAPIValueToSend: " + str(other))


def addKBValueToSend(valueToAdd):

# Currently allowing building out of longer string
# This may be introducing latency will monitor and may use direct send

    global KB_send_string
              

    try:
        logging.debug('Send String is ' + str(len(KB_send_string)) + ' characters long')
        logging.debug('Before ' + KB_send_string)


        if len(KB_send_string) == 0:
            KB_send_string = valueToAdd
        else:
            KB_send_string = KB_send_string + ',' + valueToAdd
             
        if len(KB_send_string) > 40:
            
            logging.debug('Send String Now !!!!!')
            KB_Send_Value()
            
        
        logging.debug('After ' + KB_send_string)

    except Exception as other:
        logging.critical("Error in addKBValueToSend: " + str(other))


def Send_Remaining_API_Commands():
    
    global API_send_string

    if API_send_string != '':
         API_Send_Value()       
    API_send_string = ''
    
def Send_Remaining_KB_Commands():
    
    global KB_send_string

    if KB_send_string != '':
         KB_Send_Value()       
    KB_send_string = ''

def ProcessReceivedString(ReceivedUDPString):
    global input_assignments
    global API_send_string
    global KB_send_string
    global learning
    
    logging.debug('Processing UDP String')

    API_send_string = ""
    KB_send_string = ""
    
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


                        if learning and input_assignments[workingkey]['Description'] == None:
                                updateDescription(workingkey)
                        print('Value for Description is : ' +
                              str (input_assignments[workingkey]['Description']))

                        # Switch is Closed
                        if str(workingFields[2]) == '1':
                            # API Action    
                            if learning and input_assignments[workingkey]['API_Close'] == None:
                                updateAPICloseAction(workingkey)
                            print('Value for API Close is : ' +
                              str (input_assignments[workingkey]['API_Close']))
                            if input_assignments[workingkey]['API_Close'] != None:
                                addAPIValueToSend(str (input_assignments[workingkey]['API_Close']))

                            # Keyboard action 
                            if learning and input_assignments[workingkey]['Keyboard_Close'] == None:
                                updateKBCloseAction(workingkey)
                            print('Value for Keyboard Close is : ' +
                              str (input_assignments[workingkey]['Keyboard_Close']))
                            if input_assignments[workingkey]['Keyboard_Close'] != None:
                                addKBValueToSend(str (input_assignments[workingkey]['Keyboard_Close']))


                        # Switch is Opened
                        if str(workingFields[2]) == '0':
                            # API Action
                            if learning and input_assignments[workingkey]['API_Open'] == None:
                                updateAPIOpenAction(workingkey)
                            print('Value for API Open is : ' +
                                  str (input_assignments[workingkey]['API_Open']))
                            if input_assignments[workingkey]['API_Open'] != None:
                                addAPIValueToSend(str (input_assignments[workingkey]['API_Open']))

                            # Keyboard action  
                            if learning and input_assignments[workingkey]['Keyboard_Open'] == None:
                                updateKBOpenAction(workingkey)
                            print('Value for Keyboard Open is : ' +
                                  str (input_assignments[workingkey]['Keyboard_Open']))
                            if input_assignments[workingkey]['Keyboard_Open'] != None:
                                addKBValueToSend(str (input_assignments[workingkey]['Keyboard_Open']))                           
                        
    
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
#   5: OnKeyboardAction
#   6: OffKeyboardAction


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
            dictInner['API_Open'] = None
            dictInner['API_Close'] = None
            dictInner['Keyboard_Open'] = None
            dictInner['Keyboard_Close'] = None

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
