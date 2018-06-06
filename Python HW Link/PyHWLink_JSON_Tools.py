#!/usr/bin/python

# PyHWLink_JSON_Tools.py

# Tools for working with JSON files, always saves modifications in separate file
# 1: Validates Mandatory Fields are in all records
# 2: Adds/Removes field from all records


from collections import OrderedDict
import json
import logging
import os
import socket
import sys
import time



from optparse import OptionParser

logging.basicConfig(format='%(asctime)s:%(levelname)s:%(message)s',level=logging.INFO)
#logging.basicConfig(format='%(asctime)s:%(levelname)s:%(message)s',level=logging.DEBUG)
#logging.basicConfig(format='%(asctime)s:%(levelname)s:%(message)s')




debugging = False


input_assignments_file = 'a_input_assignments.json'
temp_input_assignments_file = 'a_temp_input_assignments.json'


input_assignments = {}
send_string = None

def playtime():

    print('playtime')
    try:

        ks = list(input_assignments.keys())
        ks.sort()
        print('len =' + str(len(ks)))
        for plaything in ks:

           logging.debug(str( plaything))
           if input_assignments[plaything]['Description'] != None:
                print(str(input_assignments[plaything]['Description']))
               


##        json.dump(input_assignments, fp=open(str('b' + input_assignments_file),'w'),indent=4,sort_keys=True)
##        logging.info('Created Input Assignments file: "' + str('b' + input_assignments_file) + '"')

    except Exception as other:
        logging.critical("Error in playtime': " + str(other))

    print('playtime over')

def addfield(field_to_add):
    try:
        print('Adding Field: ' + str(field_to_add))

        ks = list(input_assignments.keys())
        ks.sort()
        
        for plaything in ks:
            try:
                input_assignments[plaything][field_to_add] = None
                if input_assignments[plaything]['Description'] != None:
                    print( input_assignments[plaything]['Description'] )

            except:
                print('Unable to Add ' + field_to_add + ' to ' + str(plaything))

                   


        json.dump(input_assignments, fp=open(str('b' + input_assignments_file),'w'),indent=4,sort_keys=True)

        logging.info('Created new file in addfield: "' + str('b' + input_assignments_file) + '"')

    except Exception as other:
        logging.critical("Error in addfield': " + str(other))       
        


def deletefield(field_to_delete):
    try:
        print('Deleting Field: ' + str(field_to_delete))


        ks = list(input_assignments.keys())
        ks.sort()
        
        for plaything in ks:
            try:
                input_assignments[plaything].pop(field_to_delete) 
                if input_assignments[plaything]['Description'] != None:
                    print( input_assignments[plaything]['Description'] )

            except:
                print('Unable to Add ' + field_to_delete + ' to ' + str(plaything))

                   


        json.dump(input_assignments, fp=open(str('b' + input_assignments_file),'w'),indent=4,sort_keys=True)

        logging.info('Created new file in deletefield: "' + str('b' + input_assignments_file) + '"')

    except Exception as other:
        logging.critical("Error in deletefield': " + str(other))       
       


def CreateEmptyAssignmentsFile():
    # Check to see if input assignment files exists - if not create it

    if not (os.path.isfile(input_assignments_file)):
        
        logging.critical('Unable to find "' + input_assignments_file + '"')
        logging.info('Creating default Input Assignments in: ' + input_assignments_file)
        
        dictOuter = {}
        dictInner = {}
        outercounter = 0
        while outercounter < 1:
            counter = 0
            while counter < 20:
                dictInner = {}
                dictInner['Description'] = None
                dictInner['Open'] = None
                dictInner['Close'] = None


                dictOuter[ '%.2d' % (outercounter) + ":" + '%.3d' % (counter)] = dictInner
                counter = counter + 1

                
            outercounter = outercounter + 1

        json.dump(dictOuter, fp=open(input_assignments_file,'w'),indent=4,sort_keys=True)

        logging.info('Created Input Assignments file: "' + input_assignments_file + '"')


def load_assignments():

    global input_assignments
    try:
        input_assignments = None

        input_assignments = json.load(open(temp_input_assignments_file))
        print('input_assignments loaded from: "' + str(temp_input_assignments_file) + '"')
        print('')

        
    except Exception as other:
        logging.critical("Error in load_assignments': " + str(other))

        
      



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
            logging.critical("Error in updateOpenAction: " + str(other))



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
            logging.critical("Error in updateCloseAction: " + str(other))



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
                    logging.warn('WARNING - Invlaid 3rd parameter: ' + str(workingFields[2]))
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
                        logging.critical('')
                        logging.critical('WARNING - Unable to read record of interest in ProcessReceivedString')
                        logging.critical('WARNING - Record name is: "' + workingkey + '"')
                        logging.critical('')
                        logging.critical("Error in ProcessReceivedString: " + str(other))
                

            Send_Remaining_Commands()
            logging.debug('Continuing on')
            


    except Exception as other:
        logging.critical("Error in ProcessReceivedString: " + str(other))




def LoadDCSParameterFile():
    # reads in a json like file from DCS which holds commands
    # this is a pre DCS 1.5 format

    # Test script loading LUA file (which contains joystick commands) into an Dictionary (actually a dictionary of dictionaries)
    # Only loads entries which have a name tag and are on a single line enclosed by { and } 


    
    input_file = 'testinputdata.csv'


    
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




dictInputAssignments = {}


def main():
    try:
        print('Starting Tools')
        load_assignments()
        playtime()
        
        deletefield('Open')
        
        print('Exiting Tools')
 #       sys.exit(0)

    except KeyboardInterrupt:
        # Catch Ctl-C and quit
        print('Exiting Tools')
        sys.exit(0)

    except Exception as other:
        logging.critical("Error in main:" + str(other))         

 

main()
