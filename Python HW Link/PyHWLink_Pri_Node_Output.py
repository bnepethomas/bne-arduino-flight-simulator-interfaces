#!/usr/bin/python

# PyHWLink_Pri_Node_Output.py

# Receives updates from DCS and distributes to output nodes
# Should perform basic sanity checks on values
# eg Data Type, Min Max
# Operates in headless mode (where errors are skipped and not reported)
#   or in interactive mode where errors are flagged


#  Dict Structure
#
#   Record eg Altitude
#       Data Type eg Integer
#       Target IP eg 172.17.1.10
#       Target Device Number
import json
import os
import socket
import sys
import time



from optparse import OptionParser



debugging = False

target = {}


# See if input configuration file exists
# Parameters are either specified in this file or passed via command line
# Command line parameters override any settings in the config file
#
output_file = 'output_config.py'
if not (os.path.isfile(output_file)):
    
    print('Unable to find ' + output_file)
    
else:
    try:
        from output_config import *
        
        print('Learning Mode: ' + str(learning))
        print('Aircraft is: ' + AircraftType)

    except Exception as other:
        print(time.asctime() + "[e] Error while opening configuration file :" + str(other))         
        print('Unable to open ' + output_file)
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
UDP_PORT_NO = 26028
DCS_IP_ADDRESS = "127.0.0.1"
DCS_PORT_NO = 26026

UDP_Reflector_IP = "127.0.0.1"
UDP_Reflector_Port = 27000

serverSock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
serverSock.settimeout(0.0001)
serverSock.bind((UDP_IP_ADDRESS, UDP_PORT_NO))



output_assignments = None
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

            # We now have processed the entire packet - time to spool it out to the different targets

            print('Sending to targets')
            for device in target:
                print(device + ' : ' + target[device]['IP'])


                workingFields = ''
                workingFields = target[device]['IP'].split(':')

                
                if len(workingFields) != 2:
                    print('')
                    print('WARNING - There are an incorrect number of fields in: ' + str(workingFields))
                    print('')
                  

                TARGET_IP_ADDRESS = workingFields[0]
                TARGET_PORT_NO = int(workingFields[1])


                send_string = target[device]['Outputstring']
                serverSock.sendto(send_string, (TARGET_IP_ADDRESS, TARGET_PORT_NO))
                
                serverSock.sendto(send_string, (UDP_Reflector_IP, UDP_Reflector_Port))
            

            

                                              
        except socket.timeout:
            a=a+1
            if (a > 100000):
                print("Long Receive Timeout - ", time.asctime())
                a=0
            continue

        except Exception as other:
            print(time.asctime() + "[e] Error in ReceivePacket: " + str(other))                




def save_and_reload_assignments():
    # Save out to a temporary file and reload to ensure it is in shape
    global output_assignments

    
    try:
        temp_output_assignments_file = 'temp_output_assignments.json'

        json.dump(output_assignments, fp=open(temp_output_assignments_file,'w'),indent=4,sort_keys=True)

        output_assignments = None

        output_assignments = json.load(open(temp_output_assignments_file))


    except Exception as other:
        print(time.asctime() + "[e] Error in save_and_reload_assignments: " + str(other))                


    

def updateDescription(workingkey):
    global output_assignments

    
    print('In learning mode - time to update the description for: '  + str(workingkey))
    updaterecord = raw_input('Update Description? [y/n]: ')
    if updaterecord.upper() == 'Y':

        try:
            wrkstring = raw_input('Please provide a description for: "' + str(workingkey) +  '" ')
            if wrkstring != '':

                output_assignments[workingkey]['Description'] = wrkstring

                save_and_reload_assignments()
                

        except Exception as other:
            print(time.asctime() + "[e] Error in updateDescription: " + str(other))                


def updateOpenAction(workingkey):
    global output_assignments

    
    print('In learning mode - time to update the Open Action for: ' + str(workingkey) + ' / '
          + output_assignments[workingkey]['Description'] + ' ' )
    updaterecord = raw_input('Update Action? [y/n]: ')
    if updaterecord.upper() == 'Y':

        try:
            wrkstring = raw_input('Please provide a Open Action for: "' + str(workingkey) +  '" "'
                                  + output_assignments[workingkey]['Description'] + '" :')

            output_assignments[workingkey]['Open'] = wrkstring

            save_and_reload_assignments()

        except Exception as other:
            print(time.asctime() + "[e] Error in updateOpenAction: " + str(other))                


def updateCloseAction(workingkey):
    global output_assignments

    
    print('In learning mode - time to update the Close Action for: ' + str(workingkey) + ' '
          + output_assignments[workingkey]['Description'])
    updaterecord = raw_input('Update Action? [y/n]: ')
    if updaterecord.upper() == 'Y':
        
        try:
            wrkstring = raw_input('Please provide a Close Action for: "' + str(workingkey) +  '" "'
                                  + output_assignments[workingkey]['Description'] + '" :')

            output_assignments[workingkey]['Close'] = wrkstring

            save_and_reload_assignments()
                
        except Exception as other:
            print(time.asctime() + "[e] Error in updateCloseAction: " + str(other))

 
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
        print(time.asctime() + "[e] Error in Send_Value: " + str(other))

        
              

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
        print(time.asctime() + "[e] Error in addValueToSend: " + str(other))



def Send_Remaining_Commands():
    
    global send_string

    if send_string != '':
         Send_Value()       
    send_string = ''


def ValidateDataType(ExpectedDataType, ValuePassed):


    #debugging = True
    ValidationComplete = False

    
    try:
        if debugging: print('ValidateDataType')
        if debugging: print('Expected Data Type :' + ExpectedDataType)
        if debugging: print('Value Passed : ' + str(ValuePassed))
        if debugging: print('Value is of Type :' + str(type(ValuePassed)))

        if ExpectedDataType == 'int':
            try:
                a = int(ValuePassed)
                ValidationComplete = True
                if debugging: print('int Validated')
            except:
                if debugging: print('int Validation Failed')

                
        elif ExpectedDataType == 'float':
            if debugging: print('Validating if float')
            try:
                a = float(ValuePassed)
                ValidationComplete = True
                if debugging: print('float Validated')
                
            except:
                if debugging: print('float Validation Failed')

                
        elif ExpectedDataType == 'bool':
            if debugging: print('Validating if bool')
            try:
                ValuePassed = int(ValuePassed)
                if ValuePassed == True or ValuePassed == False:
                    ValidationComplete = True
                    if debugging: print('bool Validated')               
            except:
                if debugging: print('bool Validation Failed')

                
        elif ExpectedDataType == 'str':
            #For the moment just accept all is good
            ValidationComplete = True

                
        if ValidationComplete:
            if debugging: print('Validation Completed Succesfully')
            return True
        else:
            if debugging: print('Validation Failed')
            return False
                
                

        if debugging: print('')

    except Exception as other:
        print(time.asctime() + "[e] Error in ValidateDataType: " + str(other))
        return False
    

def FindTarget(targetIP, stringToAdd):

    global send_string, target
    if debugging: print('')
    if debugging: print('Hunting for :' + targetIP)




    try:
        # find matching key
        if not targetIP in target:
            print ('Adding target record :' + targetIP)
            targetinner = {}
            targetinner['Outputstring'] = stringToAdd
            targetinner['IP'] = targetIP           
            target[targetIP] = targetinner
        else:
            print ('Appending target record :' + targetIP)          
            target[targetIP]['Outputstring'] = target[targetIP]['Outputstring'] + ' , ' + stringToAdd
            if debugging: print ('Added target record :' + targetIP) 


        if debugging:
            for toys in target:
                print(toys + ':' + target[toys]['Outputstring'])

        if debugging: print('')



    except Exception as other:
        print(time.asctime() + "[e] Error in FindTarget: " + str(other))
        
        
def ProcessReceivedString(ReceivedUDPString):
    global output_assignments
    global send_string, target
    global learning

    debugging = False
    
    if debugging: print('Processing UDP String')

    send_string = ""
    target = {}
    payloadOk = True
    
    
    try:
        if len(ReceivedUDPString) > 0:

            debugging = False
            
            if debugging: print('Stage 1 Processing: ' + str(ReceivedUDPString))
            if debugging: print('Checking for correct format :')

            

            workingSets =''
            workingSets = ReceivedUDPString.split(',')
            if debugging: print('There are ' + str(len(workingSets)) + ' records')
            counter = 0
            for workingRecords in workingSets:


                debugging = False
                
                if debugging: print('Record workingRecord number ' + str(counter) + ' ' +
                      workingRecords)
                counter = counter + 1
                

                workingFields = ''
                workingFields = workingRecords.split(':')

                
                if len(workingFields) != 2:
                    print('')
                    print('WARNING - There are an incorrect number of fields in: ' + str(workingFields))
                    print('')
                  
                else:
                    if debugging: print('Stage 2 Processing: ' + str(workingFields))

                    try:
                        workingkey = workingFields[0]
                        if debugging: print('Working key is: ' + workingkey)

                        if not workingkey in output_assignments:

                            print( '' )
                            print( 'WARNING NO MAPPING FOR '+ workingkey)
                            print( '' )

                        else:

                            if debugging: print('Working Fields for working key are: ' +
                                  str(output_assignments[workingkey]))

                            if debugging: print('The value is: ' +
                                  str(output_assignments[workingkey]['Description']))


                            if learning and output_assignments[workingkey]['Description'] == None:
                                    updateDescription(workingkey)
                            if debugging: print('Value for Description is : ' +
                                  str (output_assignments[workingkey]['Description']))


                            # Perform basic sanity check only if datatype is described
                            # If not ok the set flag (payloadOk) to stop further processing
                            if 'Datatype' in output_assignments[workingkey]:
                                if debugging: print('Value for Datatype is : ' +
                                      str (output_assignments[workingkey]['Datatype']))
                                if debugging: print('Value of payload is: ' + workingFields[1])

                                # Validation Failed
                                if not ValidateDataType(output_assignments[workingkey]['Datatype'], workingFields[1]):
                                    print('Warning Validation Failed for ' + str(output_assignments[workingkey]['Datatype'])
                                          + ' ' + str(workingFields[1]))
                                    
                                    payloadOk = False
                                


                            # Only generate a payload for AVs that have a target IP Address and a valid field
                            if 'IP' in output_assignments[workingkey] and 'Field' in output_assignments[workingkey] and payloadOk:
                                debugging = True
                                if debugging: print('Value for IP is : ' +
                                      str (output_assignments[workingkey]['IP']))
                                if debugging: print('Value for Field is : ' +
                                      str (output_assignments[workingkey]['Field']))
                                if output_assignments[workingkey]['IP'] != None and output_assignments[workingkey]['Field'] != None:
                                    FindTarget(str (output_assignments[workingkey]['IP']),
                                               str (output_assignments[workingkey]['Field']) + ':' +
                                               workingFields[1])




                            # Switch is Closed
##
##                            
##                            if str(workingFields[1]) == '1':
##                                if learning and output_assignments[workingkey]['Close'] == None:
##                                    updateCloseAction(workingkey)
##                                print('Value for Close is : ' +
##                                  str (output_assignments[workingkey]['Close']))
##                                if output_assignments[workingkey]['Close'] != None:
##                                    addValueToSend(str (output_assignments[workingkey]['Close']))
##
##                            # Switch is Opened
##                            if str(workingFields[1]) == '0':
##                                if learning and output_assignments[workingkey]['Open'] == None:
##                                    updateOpenAction(workingkey)
##                                print('Value for Open is : ' +
##                                      str (output_assignments[workingkey]['Open']))
##                                if output_assignments[workingkey]['Open'] != None:
##                                    addValueToSend(str (output_assignments[workingkey]['Open']))
                            
                        
    
                    except Exception as other:
                        print('')
                        print('WARNING - Unable to read record of interest in ProcessReceivedString')
                        print('WARNING - Record name is: "' + workingkey + '"')
                        print('')
                        print(time.asctime() + "[e] Error in ProcessReceivedString: " + str(other))
                

            Send_Remaining_Commands()
            if debugging: print('Continuing on')
            


    except Exception as other:
        print(time.asctime() + "[e] Error in ProcessReceivedString: " + str(other))


  
def RemoveUnwantedCharacters(stringToBeCleaned):
    stringToBeCleaned= stringToBeCleaned.replace('"', '')
    stringToBeCleaned = stringToBeCleaned.strip(' ')
    return(stringToBeCleaned)


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


# Empty dictionary
dictInputAssignments = {}



# Check to see if input assignment files exists - if not create it
output_assignments_file = 'output_assignments.json'
if not (os.path.isfile(output_assignments_file)):
    
    print('Unable to find "' + output_assignments_file + '"')
    print('Creating default Input Assignments in: ' + output_assignments_file)
    
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

    json.dump(dictOuter, fp=open(output_assignments_file,'w'),indent=4)

    print('Created Output Assignments file: "' + output_assignments_file + '"')


#        
print('Loading Output Assignments from: "' + output_assignments_file +'"')              

try:
    output_assignments = json.load(open(output_assignments_file))



except Exception as other:
    print(time.asctime() + "Unexpected error while reading file: " + str(other))                             
    serverSock.close()
    sys.exit(0)


try:

    itemOfInterest = '2:73'
    fieldOfInterest = 'Close'

    
    print('Complete Record for: ' + itemOfInterest)
    print(output_assignments[itemOfInterest])
    print('Specific Field (' + fieldOfInterest + ')' )
    
    print(output_assignments[itemOfInterest][fieldOfInterest])
    
except:
    print('Unable to read record of interest')
    print('Record name is: "' + itemOfInterest + '", Field is: "' + fieldOfInterest + '"')

            


print('Build out output block')
# Map values from LUA eg GearLightLeft to ip adress, device number, value
# Led bocks of Max7219 , State 0/1, 3 off?
# Guage blocks analog values
# Text blocks with sanity checkings



try:
    print('Waiting for packet')
    ReceivePacket()

except KeyboardInterrupt:
    # Catch Ctl-C and quit
    print('Exiting')
    serverSock.close()
    sys.exit(0)


except Exception as other:
    print(time.asctime() + "Unhandled error:" + str(other))
    




serverSock.close()
