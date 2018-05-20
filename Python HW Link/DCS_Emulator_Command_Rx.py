#!/usr/local/bin/python3

# Test script loading LUA file (which contains joystick commands) into an Dictionary (actually a dicutionary of dictionaries)
# Only loads entries which have a name tag and are on a single line enclosed by { and } 

import socket
import os
import sys
import time
import json

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

if os.path.isfile(input_file):
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

    except:
        print('Unable to read record of interest')
        print('Record name is: "' + itemOfInterest + '", Field is: "' + fieldOfInterest + '"')
else:
    print('Unable to find ' + input_file)
    
    


