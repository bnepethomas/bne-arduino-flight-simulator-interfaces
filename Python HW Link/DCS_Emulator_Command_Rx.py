#!/usr/local/bin/python3

# Test script loading LUA file (which contains joystick commands) into an Dictionary (actually a dicutionary of dictionaries)
# Only loads entries which have a name tag and are on a single line enclosed by { and } 

import socket
import os
import sys
import time
import json
import sys

# See if input configuration file exists
# Parameters are either specified in this file or passed via command line
# 
if not (os.path.isfile('input_config.py')):
    
    print('Unable to find ' + input_file)
    
else:
    try:
        from input_config import *
        
        print('Learning Mode: ' + str(learning))
        print('Aircraft is: ' + AircraftType)
            
    except:
        print('Unable to open input_config.py')
        print('Or variable assignment incorrect - forgot quotes for string?')
        print('Defaults used')
    




# See if value is assigned.  First we checked config file and then
#   command line arguments

try:
    test = AircraftType
except:
    print('AircraftType not assigned - using defaults')
    AircraftType = 'default'


UDP_IP_ADDRESS = "127.0.0.1"
UDP_PORT_NO = 26027
serverSock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
serverSock.settimeout(0.0001)
serverSock.bind((UDP_IP_ADDRESS, UDP_PORT_NO))


debugging = False


#sanity
def test():
    a=0
    while True:
        
        try:
            data, addr = serverSock.recvfrom(1500)
            print ("Message: ", data)
            print(a)
            a=0

                                              
        except socket.timeout:
            a=a+1
            if (a > 1000):
                print("Long Receive Timeout - ", time.asctime())
                a=0
            continue

def RemoveUnwantedCharacters(stringToBeCleaned):
    stringToBeCleaned= stringToBeCleaned.replace('"', '')
    stringToBeCleaned = stringToBeCleaned.strip(' ')
    return(stringToBeCleaned)


if debugging:        
    print("hello world")
    print(os.name)
    print(os.getcwd())
    print(sys.platform)
    x = os.listdir(os.curdir)
    print(x)


input_file = 'testinputdata.csv'


#deviceToFind = 'breaker'
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
        

    except:
        print('Unable to read record of interest')
        print('Record name is: "' + itemOfInterest + '", Field is: "' + fieldOfInterest + '"')

        

    print('Developing I/O Blocks')

    print('Build out input blocks 1,2,3')
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
    

    print('Build out output block')
    # Map values from LUA eg GearLightLeft to ip adress, device number, value
    # Led bocks of Max7219 , State 0/1, 3 off?
    # Guage blocks analog values
    # Text blocks with sanity checkings

