#!/usr/bin/python

# PyHWLink_Pri_Node_Input.py

# Test script loading LUA file (which contains joystick commands) into an Dictionary (actually a dictionary of dictionaries)
# Only loads entries which have a name tag and are on a single line enclosed by { and } 

from collections import OrderedDict
import json
import os
import socket
import sys
import time



from optparse import OptionParser

from pyHWLink_Tools import *


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
        print(log_mS() + "Unexpected error " + str(other))             
        print('Unable to open input_config.py')
        print('Or variable assignment incorrect - forgot quotes for string?')
        print('Defaults used')
    




# See if value is assigned.  First we checked config file and then
#   command line arguments

if debugging: print("Checking Command Line parameters")


parser = OptionParser()
parser.add_option("-d","--debug", dest="optiondebug",action="store_true",
                  default=False,
                  help="enable debug",metavar="DEBUGLEVEL")
parser.add_option("-l","--learning", dest="optionLearning",action="store_true",
                  default=False,
                  help="learning mode, assign actions to switch movements")
(options, args) = parser.parse_args()

if debugging: print("options:", str(options))
if debugging: print("arguments:", args)

# Override Learning mode if passed via command line
if options.optionLearning == True:
    learning = True
    print('Learning Mode enabled through command line')
# Override Debug mode if passed via command line    
if options.optiondebug == True:
    debugging = True
    print('Debugging Mode enabled through command line')    

    
    

try:
    test = AircraftType
except:
    print('AircraftType not assigned - using defaults')
    AircraftType = 'default'


UDP_IP_ADDRESS = "127.0.0.1"
UDP_PORT_NO = 26027
DCS_IP_ADDRESS = "127.0.0.1"
DCS_PORT_NO = 26026

UDP_Reflector_IP = "127.0.0.1"
UDP_Reflector_Port = 27000

serverSock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
serverSock.settimeout(0.0001)
serverSock.bind((UDP_IP_ADDRESS, UDP_PORT_NO))



input_assignments = None
send_string = None

def ReceivePacket():

    a=0
    while True:
        
        try:
            data, addr = serverSock.recvfrom(1500)
            
            if debugging: print ("Message: ", data)
            ReceivedPacket = data
            ProcessReceivedString(str(ReceivedPacket))
            
            if debugging: print(a)
            a=0

                                              
        except socket.timeout:
            a=a+1
            if (a > 100000):
                print(log_mS() + " Long Receive Timeout")
                a=0
            continue

        
        except Exception as other:
            print(log_mS() + "[e] Error in RecevePacket: " + str(other)) 

 

def save_and_reload_assignments():
    # Save out to a temporary file and reload to ensure it is in shape
    global input_assignments

    
    try:
        temp_input_assignments_file = 'temp_input_assignments.json'

        json.dump(input_assignments, fp=open(temp_input_assignments_file,'w'),indent=4,sort_keys=True)

        input_assignments = None

        input_assignments = json.load(open(temp_input_assignments_file))

    except Exception as other:
        print(log_mS() + "[e] Error in save_and_reload_assignments': " + str(other))

        
  
    
    

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
            print(log_mS() + "[e] Error in updateDescription': " + str(other))




def updateOpenAction(workingkey):
    global input_assignments

    
    print('In learning mode - time to update the Open Action for: ' + str(workingkey) + ' / '
          + input_assignments[workingkey]['Description'] + ' ' )
    updaterecord = raw_input('Update Action? [y/n]: ')
    if updaterecord.upper() == 'Y':

        try:
            wrkstring = raw_input('Please provide a Open Action for: "' + str(workingkey) +  '" "'
                                  + input_assignments[workingkey]['Description'] + '" :')

            input_assignments[workingkey]['Open'] = wrkstring

            save_and_reload_assignments()
                
        except Exception as other:
            print(log_mS() + "[e] Error in updateOpenAction: " + str(other))



def updateCloseAction(workingkey):
    global input_assignments

    
    print('In learning mode - time to update the Close Action for: ' + str(workingkey) + ' '
          + input_assignments[workingkey]['Description'])
    updaterecord = raw_input('Update Action? [y/n]: ')
    if updaterecord.upper() == 'Y':
        
        try:
            wrkstring = raw_input('Please provide a Close Action for: "' + str(workingkey) +  '" "'
                                  + input_assignments[workingkey]['Description'] + '" :')

            input_assignments[workingkey]['Close'] = wrkstring

            save_and_reload_assignments()
                
        except Exception as other:
            print(log_mS() + "[e] Error in updateCloseAction: " + str(other))
                

 

def Send_Value():

    global send_string
    global serverSock


    try:

        if True: print ("UDP target port:" + str(DCS_PORT_NO))
        if debugging: print ("UDP target port:" + str(DCS_PORT_NO))

        serverSock.sendto(send_string, (DCS_IP_ADDRESS, DCS_PORT_NO))
        serverSock.sendto(send_string, (UDP_Reflector_IP, UDP_Reflector_Port))

        send_string = ""

    except Exception as other:
        print(log_mS + "[e] Error in Send_Value: " + str(other))
        
              

def addValueToSend(valueToAdd):

    global send_string
              

    try:
        if debugging: print('Send String is ' + str(len(send_string)) + ' characters long')
        if debugging: print('Before ' + send_string)


        if len(send_string) == 0:
            send_string = valueToAdd
        else:
            send_string = send_string + ',' + valueToAdd
             
        if len(send_string) > 40:
            
            if debugging: print('Send String Now !!!!!')
            Send_Value()
            
        
        if debugging: print('After ' + send_string)

    except Exception as other:
        print(log_mS + "[e] Error in addValueToSend: " + str(other))


def Send_Remaining_Commands():
    
    global send_string

    if send_string != '':
         Send_Value()       
    send_string = ''
    


def ProcessReceivedString(ReceivedUDPString):
    global input_assignments
    global send_string
    global learning
    
    if debugging: print('Processing UDP String')

    send_string = ""
    
    try:
        if len(ReceivedUDPString) > 0 and ReceivedUDPString[0] == 'D':
            
            if debugging: print('Stage 1 Processing: ' + str(ReceivedUDPString))
            # Remove leading D
            ReceivedUDPString = str(ReceivedUDPString[1:])
            if debugging: print('Checking for correct format :')

            

            workingSets =''
            workingSets = ReceivedUDPString.split(',')
            if debugging: print('There are ' + str(len(workingSets)) + ' records')
            counter = 0
            for workingRecords in workingSets:
                if debugging: print('Record workingRecord number ' + str(counter) + ' ' +
                      workingRecords)
                counter = counter + 1
                

                workingFields = ''
                workingFields = workingRecords.split(':')

                
                if len(workingFields) != 3:
                    print('')
                    print('WARNING - There are an incorrect number of fields in: ' + str(workingFields))
                    print('')
                elif str(workingFields[2]) != '0' and str(workingFields[2]) != '1':
                    print('')
                    print('WARNING - Invlaid 3rd parameter: ' + str(workingFields[2]))
                    print('')                   
                else:
                    if debugging: print('Stage 2 Processing: ' + str(workingFields))

                    try:
                        workingkey = workingFields[0] + ':' + workingFields[1]
                        if debugging: print('Working key is: ' + workingkey)
                        
                        if debugging: print('Working Fields for working key are: ' +
                              str(input_assignments[workingkey]))

                        if debugging: print('The value is: ' +
                              str(input_assignments[workingkey]['Description']))


                        if learning and input_assignments[workingkey]['Description'] == None:
                                updateDescription(workingkey)
                        print('Value for Description is : ' +
                              str (input_assignments[workingkey]['Description']))

                        # Switch is Closed
                        if str(workingFields[2]) == '1':
                            if learning and input_assignments[workingkey]['Close'] == None:
                                updateCloseAction(workingkey)
                            print('Value for Close is : ' +
                              str (input_assignments[workingkey]['Close']))
                            if input_assignments[workingkey]['Close'] != None:
                                addValueToSend(str (input_assignments[workingkey]['Close']))

                        # Switch is Opened
                        if str(workingFields[2]) == '0':
                            if learning and input_assignments[workingkey]['Open'] == None:
                                updateOpenAction(workingkey)
                            print('Value for Open is : ' +
                                  str (input_assignments[workingkey]['Open']))
                            if input_assignments[workingkey]['Open'] != None:
                                addValueToSend(str (input_assignments[workingkey]['Open']))
                            
                        
    
                    except Exception as other:
                        print('')
                        print('WARNING - Unable to read record of interest in ProcessReceivedString')
                        print('WARNING - Record name is: "' + workingkey + '"')
                        print('')
                        print(log_mS + "[e] Error in ProcessReceivedString: " + str(other))
                

            Send_Remaining_Commands()
            if debugging: print('Continuing on')
            


    except Exception as other:
        print(log_mS + "[e] Error in ProcessReceivedString: " + str(other))


  
def RemoveUnwantedCharacters(stringToBeCleaned):
    stringToBeCleaned= stringToBeCleaned.replace('"', '')
    stringToBeCleaned = stringToBeCleaned.strip(' ')
    return(stringToBeCleaned)


def LoadDCSParameterFile():
    # reads in a json like file from DCS which holds commands
    # this is a pre DCS 1.5 format

    
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


                if debugging: print("Processing " +  myline)

                # Initialise list
                mylist = []
                
                
                # Strip out leading and trailing '#'
                myline = myline[ myline.find("{") + 1:]
                myline = myline[: myline.find("}")]
                
                if debugging: print("Removed # " + myline[:])

                # Split into attrib var pairs
                mysplit = myline.split(',')
                
                if debugging: print(mysplit)

                
                for i in mysplit:

                    if debugging: print("Cleaning up " + i)
                    mysplit1 = i.split('=')
                    if debugging: print (mysplit1)

                    if len(mysplit1) == 2:
                        if debugging:  print('Have correct number of attributes')

                        # Remove unwanted characters
                        s = RemoveUnwantedCharacters(mysplit1[0])
                        t = RemoveUnwantedCharacters(mysplit1[1])

                        mylist.append([s,t])

                        if debugging and s == 'name':
                            print("Found a name: " + t )


                # Now have AV pairs in a list - time to see if one of them is name

                innerDict = {}
                for s in mylist:
                    innerDict[s[0]] = s[1]
                if debugging: print('The end result is ')
                if debugging: print(mylist)

                
                for s in mylist:
                    if s[0] == 'name':
                        if debugging: print('We have a valid name - time to add a item to the dictionary')

                        for t in mylist:
                            if t[0] == 'down':
                        
                                #myDict[s[1]] = t[1]
                                #myDict[s[1]] = mylist
                                myDict[s[1]] = innerDict
                                
                                if debugging: print('myDict is now ' + str(len(myDict)))
                                #print('Dictionary is :' +  myDict['name'])
                                                                       
                    
                    
                
            myline = file_object.readline()

            
        print('Have read ' + str(count) + ' lines')
        print('Have loaded ' + str(len(myDict)) + ' entries')
        
        file_object.close()

        if debugging: print(myDict)

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
            print(log_mS() + "[e] Error in LoadDCSParameterFile" + str(other))         
            print('Unable to read record of interest')
            print('Record name is: "' + itemOfInterest + '", Field is: "' + fieldOfInterest + '"')

            

    

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
input_assignments_file = 'input_assignments.json'
if not (os.path.isfile(input_assignments_file)):
    
    print('Unable to find "' + input_assignments_file + '"')
    print('Creating default Input Assignments in: ' + input_assignments_file)
    
    dictOuter = {}
    dictInner = {}
    outercounter = 0
    while outercounter < 3:
        counter = 0
        while counter < 256:
            dictInner = {}
            dictInner['Description'] = None
            dictInner['Open'] = None
            dictInner['Close'] = None

            #dictOuter[str(outercounter) + ":" + str(counter)] = dictInner
            dictOuter[ '%.2d' % (outercounter) + ":" + '%.3d' % (counter)] = dictInner
            counter = counter + 1

            
        outercounter = outercounter + 1

    json.dump(dictOuter, fp=open(input_assignments_file,'w'),indent=4)

    print('Created Input Assignments file: "' + input_assignments_file + '"')


#        
print('Loading Input Assignments from: "' + input_assignments_file +'"')              

try:
    input_assignments = json.load(open(input_assignments_file))
                             
except Exception as other:
    print(log_mS() + "Unexpected error while reading file:" + str(other))         

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
    print(log_mS() + "[e] Error in main:" + str(other))         

 


serverSock.close()
