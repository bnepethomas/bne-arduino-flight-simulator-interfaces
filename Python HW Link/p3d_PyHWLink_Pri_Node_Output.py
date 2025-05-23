
#!/usr/bin/python

# PyHWLink_Pri_Node_Output.py

# Receives updates from DCS and distributes to output nodes

# On receive a packet, it builds out an array of AV pairs per destination
# and then throws the assembled array at the target once the packet is parsed

# By default it simple appends the received value from the sim, ie no range
#  checking or anything like that which can work find for binary indicators
#  but doesn't work so value for analog ranges or where special tasks need to
#  be carried out such as a bar display for Spoilers or spoiler position where
#  multiple outputs need to be coordinated. This sort of can be forced, but
#  now to add a value in the JSON file to inform code the parameter should
#  not be passed/
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

# Todos
# 20190609  Add field to JSON file to tell code not to append passed value from Sim


# Used for Gauge translation
airspeeds_input_array = [260,250,240,230,220,210,200,190,180,170,160,150,140,130,120,110,100,90 ,80 ,70 ,60 ,50 ,40 ,30 ,0]
airspeeds_calibrated_array  = [29 ,35 ,40 ,45 ,50 ,55 ,60 ,67 ,72 ,78 ,86 ,95 ,102,109,118,125,131,143,150,157,161,171,173,180,180]

import json
import os
import socket
import sys
import time
import logging


from optparse import OptionParser


debugging = True

target = {}
output_assignments_file = 'output_assignments.json'


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
        logging.critical("Error while opening configuration file :" + str(other))         
        print('Unable to open ' + output_file)
        print('Or variable assignment incorrect - forgot quotes for string?')
        print('Defaults used')
    




# See if value is assigned.  First we checked config file and then
#   command line arguments

logging.basicConfig(format='%(asctime)s:%(levelname)s:%(message)s',level=logging.INFO)
#logging.basicConfig(format='%(asctime)s:%(levelname)s:%(message)s',level=logging.DEBUG)
#logging.basicConfig(format='%(asctime)s:%(levelname)s:%(message)s')
 
logging.debug("Checking Command Line parameters")


parser = OptionParser()
parser.add_option("-d","--debug", dest="optiondebug",action="store_true",
                  default=False,
                  help="enable debug",metavar="DEBUGLEVEL")
parser.add_option("-l","--learning", dest="optionLearning",action="store_true",
                  default=False,
                  help="learning mode, assign actions to switch movements")
(options, args) = parser.parse_args()

logging.debug("options:" + str(options))
logging.debug("arguments:" + str(args))

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


UDP_IP_ADDRESS = "0"
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
            data, (Source_IP, Source_Port)  = serverSock.recvfrom(1500)


            logging.debug("From: " + str(Source_IP) + " " + str(Source_Port)) 
            
            ReceivedPacket = data.decode()
            logging.debug ("Message: " + ReceivedPacket)
            ProcessReceivedString(str(ReceivedPacket))
            
            logging.debug(a)
            a=0

            # We now have processed the entire packet - time to spool it out to the different targets

            logging.debug(' Sending to targets')

            
            
            for device in target:
                logging.debug(device + ' : ' + target[device]['IP'])

  

                # Extract the target IP Address and Port
                workingFields = ''
                workingFields = target[device]['IP'].split(':')

                
                if len(workingFields) != 2:
                    print('')
                    logging.warning('While attempt to extract Target IP and Port - incorrect number of fields in: ' + str(workingFields))
                    print('')
                  

                TARGET_IP_ADDRESS = workingFields[0]
                TARGET_PORT_NO = int(workingFields[1])


                # 20190615 manually adding prefix - possibly should do this more elegantly, but then everything coming from this should
                #    be a Data packet
                target[device]['Outputstring'] = 'D,' + target[device]['Outputstring']


                send_string = target[device]['Outputstring']
                logging.debug('Sending - ' + str(target[device]['IP']) + ' ' + str(target[device]['Outputstring']))

                
                serverSock.sendto(send_string.encode(), (TARGET_IP_ADDRESS, TARGET_PORT_NO))
                
                serverSock.sendto(send_string.encode(), (UDP_Reflector_IP, UDP_Reflector_Port))
            

            

                                              
        except socket.timeout:
            a=a+1
            if (a > 100000):
                logging.info("Long Receive Timeout")
                a=0
            continue

        except Exception as other:
            logging.critical("Error in ReceivePacket: " + str(other))                




def save_and_reload_assignments():
    # Save out to a temporary file and reload to ensure it is in shape
    global output_assignments

    
    try:
        temp_output_assignments_file = 'temp_output_assignments.json'

        json.dump(output_assignments, fp=open(temp_output_assignments_file,'w'),indent=4,sort_keys=True)

        output_assignments = None

        output_assignments = json.load(open(temp_output_assignments_file))

        print('Setting have been saved to "' + temp_output_assignments_file +
              '". Copy this file to "' + output_assignments_file + '" to permanently activate')


    except Exception as other:
        logging.critical('Error in save_and_reload_assignments: ' + str(other))                


    

# Currently not being used - unsure if needed for outputs
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
            logging.critical('Error in updateDescription: ' + str(other))                


               
        



def ValidateDataType(ExpectedDataType, ValuePassed):



    ValidationComplete = False

    
    try:
        logging.debug('ValidateDataType')
        logging.debug('Expected Data Type :' + ExpectedDataType)
        logging.debug('Value Passed : ' + str(ValuePassed))
        logging.debug('Value is of Type :' + str(type(ValuePassed)))

        if ExpectedDataType == 'int':
            try:
                a = int(ValuePassed)
                ValidationComplete = True
                logging.debug('int Validated')
            except:
                logging.warning('int Validation Failed')

                
        elif ExpectedDataType == 'float':
            logging.debug('Validating if float')
            try:
                a = float(ValuePassed)
                ValidationComplete = True
                logging.debug('float Validated')
                
            except:
                logging.warning('float Validation Failed')

                
        elif ExpectedDataType == 'bool':
            logging.debug('Validating if bool')
            try:
                ValuePassed = int(ValuePassed)
                if ValuePassed == True or ValuePassed == False:
                    ValidationComplete = True
                    logging.debug('bool Validated')               
            except:
                logging.warning('bool Validation Failed')

                
        elif ExpectedDataType == 'str':
            #For the moment just accept all is good
            ValidationComplete = True

                
        if ValidationComplete:
            logging.debug('Validation Completed Succesfully')
            return True
        else:
            logging.warning('Validation Failed')
            return False
                
                

        logging.debug('')

    except Exception as other:
        logging.critical('Error in ValidateDataType: ' + str(other))
        return False
    


# Is passed the IPAddress:Port pair of the target device and the action to add
# If a record doesn't exist - create the record and create field for action
# otherwise find record and append action to add
#
def FindTarget(targetIP, stringToAdd):


    debugging = False 
    global send_string, target
    logging.debug('')
    logging.debug('Hunting for :' + targetIP)



    try:
        # find matching key 
        if not targetIP in target:

            # Don't yet have have a record for this target for this iteration - create one
            # Assign action to 'Outputstring' field
            
            if debugging: print ('Adding target record :' + targetIP)
            targetinner = {}
            targetinner['Outputstring'] = stringToAdd
            targetinner['IP'] = targetIP           
            target[targetIP] = targetinner
        else:

            # Already have a record for the target - just append action to 'Outputstring' field
            
            logging.debug('Appending target record :' + targetIP)          
            target[targetIP]['Outputstring'] = target[targetIP]['Outputstring'] + ',' + stringToAdd
            logging.debug('Added target record :' + targetIP) 



        if debugging:
            logging.debug('Data Structures thus far')
            for IP_Port_Pair in target:
                logging.debug(IP_Port_Pair + '-' + target[IP_Port_Pair]['Outputstring'])

        logging.debug('')



    except Exception as other:
        logging.critical('Error in FindTarget: ' + str(other))
        
        
def ProcessReceivedString(ReceivedUDPString):
    global output_assignments
    global send_string, target
    global learning

    debugging = False
    
    logging.debug('Processing UDP String')

    send_string = ""
    target = {}
    payloadOk = True
    
    
    try:
        if len(ReceivedUDPString) > 0:

            debugging = False
            
            logging.debug('Stage 1 Processing: ' + str(ReceivedUDPString))
            logging.debug('Checking for correct format :')

            

            workingSets =''
            workingSets = ReceivedUDPString.split(',')
            logging.debug('There are ' + str(len(workingSets)) + ' records')
            counter = 0
            for workingRecords in workingSets:


                debugging = False
                
                logging.debug('Record workingRecord number ' + str(counter) + ' ' +
                      workingRecords)
                counter = counter + 1
                

                workingFields = ''
                workingFields = workingRecords.split(':')

                
                if len(workingFields) != 2:
                    print('')
                    print('WARNING - There are an incorrect number of fields in: ' + str(workingFields))
                    print('')
                  
                else:
                    logging.debug('Stage 2 Processing: ' + str(workingFields))

                    try:
                        workingkey = workingFields[0]
                        logging.debug('Working key is: ' + workingkey)

                        if not workingkey in output_assignments:

                            logging.warn( '' )
                            logging.warn( 'WARNING NO MAPPING FOR '+ workingkey)
                            logging.warn( '' )

                        else:

                            logging.debug('Working Fields for working key are: ' +
                                  str(output_assignments[workingkey]))

                            logging.debug('The value is: ' +
                                  str(output_assignments[workingkey]['Description']))


                            if learning and output_assignments[workingkey]['Description'] == None:
                                    updateDescription(workingkey)
                            logging.debug('Value for Description is : ' +
                                  str (output_assignments[workingkey]['Description']))


                            # Perform basic sanity check only if datatype is described
                            # If not ok the set flag (payloadOk) to stop further processing
                            if 'Datatype' in output_assignments[workingkey]:
                                logging.debug('Value for Datatype is : ' +
                                      str (output_assignments[workingkey]['Datatype']))
                                logging.debug('Value of payload is: ' + workingFields[1])

                                # Validation Failed
                                if not ValidateDataType(output_assignments[workingkey]['Datatype'], workingFields[1]):
                                    print('Warning Validation Failed for ' + str(output_assignments[workingkey]['Datatype'])
                                          + ' ' + str(workingFields[1]))
                                    
                                    payloadOk = False
                                


                            # Only generate a payload for AVs that have a target IP Address and a valid field
                            if 'IP' in output_assignments[workingkey] and 'Field' in output_assignments[workingkey] and payloadOk:

                                logging.debug('Value for IP is : ' +
                                      str (output_assignments[workingkey]['IP']))
                                logging.debug('Value for Field is : ' +
                                      str (output_assignments[workingkey]['Field']))



                                if (workingkey == "TRAILING_EDGE_FLAPS_LEFT_ANGLE"):
                                    # print( "******************flaps treatment ******************")
                                    # print('flaps is: ' + str(workingFields[1]))

                                    # Allocating Leds
                                    FlapLed1 = 24
                                    FlapLed2 = 25
                                    FlapLed3 = 26
                                    FlapLed4 = 27
                                    FlapLed5 = 28
                                    FlapLed6 = 29
                                    FlapLed7 = 30
                                    FlapLed8 = 31
                                    FlapLed9 = 32
                                    FlapLed10 = 32
                               
                                    
                                    if (int(workingFields[1]) > 0 and int(workingFields[1]) < 10):
                                        workingFields[1] = '1,' + \
                                        str(FlapLed1) + ':1,' + \
                                        str(FlapLed2) + ':0,' + \
                                        str(FlapLed3) + ':0,' + \
                                        str(FlapLed4) + ':0,' + \
                                        str(FlapLed5) + ':0,' + \
                                        str(FlapLed6) + ':0,' + \
                                        str(FlapLed7) + ':0,' + \
                                        str(FlapLed8) + ':0,' + \
                                        str(FlapLed9) + ':0,' + \
                                        str(FlapLed10) + ':0' 

                                    elif (int(workingFields[1]) >= 10 and int(workingFields[1]) < 20):
                                        workingFields[1] = '1,' + \
                                        str(FlapLed1) + ':1,' + \
                                        str(FlapLed2) + ':1,' + \
                                        str(FlapLed3) + ':0,' + \
                                        str(FlapLed4) + ':0,' + \
                                        str(FlapLed5) + ':0,' + \
                                        str(FlapLed6) + ':0,' + \
                                        str(FlapLed7) + ':0,' + \
                                        str(FlapLed8) + ':0,' + \
                                        str(FlapLed9) + ':0,' + \
                                        str(FlapLed10) + ':0'
                                    elif (int(workingFields[1]) >= 20 and int(workingFields[1]) < 30):
                                        workingFields[1] = '1,' + \
                                        str(FlapLed1) + ':1,' + \
                                        str(FlapLed2) + ':1,' + \
                                        str(FlapLed3) + ':1,' + \
                                        str(FlapLed4) + ':0,' + \
                                        str(FlapLed5) + ':0,' + \
                                        str(FlapLed6) + ':0,' + \
                                        str(FlapLed7) + ':0,' + \
                                        str(FlapLed8) + ':0,' + \
                                        str(FlapLed9) + ':0,' + \
                                        str(FlapLed10) + ':0' 
                                    elif (int(workingFields[1]) >= 30 and int(workingFields[1]) < 40):
                                        workingFields[1] = '1,' + \
                                        str(FlapLed1) + ':1,' + \
                                        str(FlapLed2) + ':1,' + \
                                        str(FlapLed3) + ':1,' + \
                                        str(FlapLed4) + ':1,' + \
                                        str(FlapLed5) + ':0,' + \
                                        str(FlapLed6) + ':0,' + \
                                        str(FlapLed7) + ':0,' + \
                                        str(FlapLed8) + ':0,' + \
                                        str(FlapLed9) + ':0,' + \
                                        str(FlapLed10) + ':0'
                                    elif (int(workingFields[1]) >= 40 and int(workingFields[1]) < 50):
                                        workingFields[1] = '1,' + \
                                        str(FlapLed1) + ':1,' + \
                                        str(FlapLed2) + ':1,' + \
                                        str(FlapLed3) + ':1,' + \
                                        str(FlapLed4) + ':1,' + \
                                        str(FlapLed5) + ':1,' + \
                                        str(FlapLed6) + ':0,' + \
                                        str(FlapLed7) + ':0,' + \
                                        str(FlapLed8) + ':0,' + \
                                        str(FlapLed9) + ':0,' + \
                                        str(FlapLed10) + ':0'
                                    elif (int(workingFields[1]) >= 50 and int(workingFields[1]) < 60):
                                        workingFields[1] = '1,' + \
                                        str(FlapLed1) + ':1,' + \
                                        str(FlapLed2) + ':1,' + \
                                        str(FlapLed3) + ':1,' + \
                                        str(FlapLed4) + ':1,' + \
                                        str(FlapLed5) + ':1,' + \
                                        str(FlapLed6) + ':1,' + \
                                        str(FlapLed7) + ':0,' + \
                                        str(FlapLed8) + ':0,' + \
                                        str(FlapLed9) + ':0,' + \
                                        str(FlapLed10) + ':0'
                                    elif (int(workingFields[1]) >= 60 and int(workingFields[1]) < 70):
                                        workingFields[1] = '1,' + \
                                        str(FlapLed1) + ':1,' + \
                                        str(FlapLed2) + ':1,' + \
                                        str(FlapLed3) + ':1,' + \
                                        str(FlapLed4) + ':1,' + \
                                        str(FlapLed5) + ':1,' + \
                                        str(FlapLed6) + ':1,' + \
                                        str(FlapLed7) + ':1,' + \
                                        str(FlapLed8) + ':0,' + \
                                        str(FlapLed9) + ':0,' + \
                                        str(FlapLed10) + ':0' 
                                    elif (int(workingFields[1]) >= 70 and int(workingFields[1]) < 80):
                                        workingFields[1] = '1, ' + \
                                        str(FlapLed1) + ':1,' + \
                                        str(FlapLed2) + ':1,' + \
                                        str(FlapLed3) + ':1,' + \
                                        str(FlapLed4) + ':1,' + \
                                        str(FlapLed5) + ':1,' + \
                                        str(FlapLed6) + ':1,' + \
                                        str(FlapLed7) + ':1,' + \
                                        str(FlapLed8) + ':1,' + \
                                        str(FlapLed9) + ':0,' + \
                                        str(FlapLed10) + ':0'
                                    elif (int(workingFields[1]) >= 80 and int(workingFields[1]) < 90):
                                        workingFields[1] = '1,' + \
                                        str(FlapLed1) + ':1,' + \
                                        str(FlapLed2) + ':1,' + \
                                        str(FlapLed3) + ':1,' + \
                                        str(FlapLed4) + ':1,' + \
                                        str(FlapLed5) + ':1,' + \
                                        str(FlapLed6) + ':1,' + \
                                        str(FlapLed7) + ':1,' + \
                                        str(FlapLed8) + ':1,' + \
                                        str(FlapLed9) + ':1,' + \
                                        str(FlapLed10) + ':0'
                                    elif (int(workingFields[1]) >= 90):
                                        workingFields[1] = '1, ' + \
                                        str(FlapLed1) + ':1,' + \
                                        str(FlapLed2) + ':1,' + \
                                        str(FlapLed3) + ':1,' + \
                                        str(FlapLed4) + ':1,' + \
                                        str(FlapLed5) + ':1,' + \
                                        str(FlapLed6) + ':1,' + \
                                        str(FlapLed7) + ':1,' + \
                                        str(FlapLed8) + ':1,' + \
                                        str(FlapLed9) + ':1,' + \
                                        str(FlapLed10) + ':1'
                                    else:
                                        workingFields[1] = '0,' + \
                                        str(FlapLed1) + ':0,' + \
                                        str(FlapLed2) + ':0,' + \
                                        str(FlapLed3) + ':0,' + \
                                        str(FlapLed4) + ':0,' + \
                                        str(FlapLed5) + ':0,' + \
                                        str(FlapLed6) + ':0,' + \
                                        str(FlapLed7) + ':0,' + \
                                        str(FlapLed8) + ':0,' + \
                                        str(FlapLed9) + ':0,' + \
                                        str(FlapLed10) + ':0'

                                        

                                elif (workingkey == "SPOILERS_LEFT_POSITION"):
                                    # print( "******************spoiler treatment ******************")
                                    # print('spoiler position is: ' + str(workingFields[1]))

                                    # Allocating Leds
                                    SpoilerLed1 = 40
                                    SpoilerLed2 = 41
                                    SpoilerLed3 = 42
                                    SpoilerLed4 = 43
                                    SpoilerLed5 = 44
                                    SpoilerLed6 = 45
                                    SpoilerLed7 = 46
                                    SpoilerLed8 = 47
                                    SpoilerLed9 = 48
                                    SpoilerLed10 = 49
                               
                                    
                                    if (int(workingFields[1]) > 0 and int(workingFields[1]) < 10):
                                        workingFields[1] = '1,' + \
                                        str(SpoilerLed1) + ':1,' + \
                                        str(SpoilerLed2) + ':0,' + \
                                        str(SpoilerLed3) + ':0,' + \
                                        str(SpoilerLed4) + ':0,' + \
                                        str(SpoilerLed5) + ':0,' + \
                                        str(SpoilerLed6) + ':0,' + \
                                        str(SpoilerLed7) + ':0,' + \
                                        str(SpoilerLed8) + ':0,' + \
                                        str(SpoilerLed9) + ':0,' + \
                                        str(SpoilerLed10) + ':0' 

                                    elif (int(workingFields[1]) >= 10 and int(workingFields[1]) < 20):
                                        workingFields[1] = '1,' + \
                                        str(SpoilerLed1) + ':1,' + \
                                        str(SpoilerLed2) + ':1,' + \
                                        str(SpoilerLed3) + ':0,' + \
                                        str(SpoilerLed4) + ':0,' + \
                                        str(SpoilerLed5) + ':0,' + \
                                        str(SpoilerLed6) + ':0,' + \
                                        str(SpoilerLed7) + ':0,' + \
                                        str(SpoilerLed8) + ':0,' + \
                                        str(SpoilerLed9) + ':0,' + \
                                        str(SpoilerLed10) + ':0'
                                    elif (int(workingFields[1]) >= 20 and int(workingFields[1]) < 30):
                                        workingFields[1] = '1,' + \
                                        str(SpoilerLed1) + ':1,' + \
                                        str(SpoilerLed2) + ':1,' + \
                                        str(SpoilerLed3) + ':1,' + \
                                        str(SpoilerLed4) + ':0,' + \
                                        str(SpoilerLed5) + ':0,' + \
                                        str(SpoilerLed6) + ':0,' + \
                                        str(SpoilerLed7) + ':0,' + \
                                        str(SpoilerLed8) + ':0,' + \
                                        str(SpoilerLed9) + ':0,' + \
                                        str(SpoilerLed10) + ':0' 
                                    elif (int(workingFields[1]) >= 30 and int(workingFields[1]) < 40):
                                        workingFields[1] = '1,' + \
                                        str(SpoilerLed1) + ':1,' + \
                                        str(SpoilerLed2) + ':1,' + \
                                        str(SpoilerLed3) + ':1,' + \
                                        str(SpoilerLed4) + ':1,' + \
                                        str(SpoilerLed5) + ':0,' + \
                                        str(SpoilerLed6) + ':0,' + \
                                        str(SpoilerLed7) + ':0,' + \
                                        str(SpoilerLed8) + ':0,' + \
                                        str(SpoilerLed9) + ':0,' + \
                                        str(SpoilerLed10) + ':0'
                                    elif (int(workingFields[1]) >= 40 and int(workingFields[1]) < 50):
                                        workingFields[1] = '1,' + \
                                        str(SpoilerLed1) + ':1,' + \
                                        str(SpoilerLed2) + ':1,' + \
                                        str(SpoilerLed3) + ':1,' + \
                                        str(SpoilerLed4) + ':1,' + \
                                        str(SpoilerLed5) + ':1,' + \
                                        str(SpoilerLed6) + ':0,' + \
                                        str(SpoilerLed7) + ':0,' + \
                                        str(SpoilerLed8) + ':0,' + \
                                        str(SpoilerLed9) + ':0,' + \
                                        str(SpoilerLed10) + ':0'
                                    elif (int(workingFields[1]) >= 50 and int(workingFields[1]) < 60):
                                        workingFields[1] = '1,' + \
                                        str(SpoilerLed1) + ':1,' + \
                                        str(SpoilerLed2) + ':1,' + \
                                        str(SpoilerLed3) + ':1,' + \
                                        str(SpoilerLed4) + ':1,' + \
                                        str(SpoilerLed5) + ':1,' + \
                                        str(SpoilerLed6) + ':1,' + \
                                        str(SpoilerLed7) + ':0,' + \
                                        str(SpoilerLed8) + ':0,' + \
                                        str(SpoilerLed9) + ':0,' + \
                                        str(SpoilerLed10) + ':0'
                                    elif (int(workingFields[1]) >= 60 and int(workingFields[1]) < 70):
                                        workingFields[1] = '1,' + \
                                        str(SpoilerLed1) + ':1,' + \
                                        str(SpoilerLed2) + ':1,' + \
                                        str(SpoilerLed3) + ':1,' + \
                                        str(SpoilerLed4) + ':1,' + \
                                        str(SpoilerLed5) + ':1,' + \
                                        str(SpoilerLed6) + ':1,' + \
                                        str(SpoilerLed7) + ':1,' + \
                                        str(SpoilerLed8) + ':0,' + \
                                        str(SpoilerLed9) + ':0,' + \
                                        str(SpoilerLed10) + ':0' 
                                    elif (int(workingFields[1]) >= 70 and int(workingFields[1]) < 80):
                                        workingFields[1] = '1, ' + \
                                        str(SpoilerLed1) + ':1,' + \
                                        str(SpoilerLed2) + ':1,' + \
                                        str(SpoilerLed3) + ':1,' + \
                                        str(SpoilerLed4) + ':1,' + \
                                        str(SpoilerLed5) + ':1,' + \
                                        str(SpoilerLed6) + ':1,' + \
                                        str(SpoilerLed7) + ':1,' + \
                                        str(SpoilerLed8) + ':1,' + \
                                        str(SpoilerLed9) + ':0,' + \
                                        str(SpoilerLed10) + ':0'
                                    elif (int(workingFields[1]) >= 80 and int(workingFields[1]) < 90):
                                        workingFields[1] = '1,' + \
                                        str(SpoilerLed1) + ':1,' + \
                                        str(SpoilerLed2) + ':1,' + \
                                        str(SpoilerLed3) + ':1,' + \
                                        str(SpoilerLed4) + ':1,' + \
                                        str(SpoilerLed5) + ':1,' + \
                                        str(SpoilerLed6) + ':1,' + \
                                        str(SpoilerLed7) + ':1,' + \
                                        str(SpoilerLed8) + ':1,' + \
                                        str(SpoilerLed9) + ':1,' + \
                                        str(SpoilerLed10) + ':0'
                                    elif (int(workingFields[1]) >= 90):
                                        workingFields[1] = '1, ' + \
                                        str(SpoilerLed1) + ':1,' + \
                                        str(SpoilerLed2) + ':1,' + \
                                        str(SpoilerLed3) + ':1,' + \
                                        str(SpoilerLed4) + ':1,' + \
                                        str(SpoilerLed5) + ':1,' + \
                                        str(SpoilerLed6) + ':1,' + \
                                        str(SpoilerLed7) + ':1,' + \
                                        str(SpoilerLed8) + ':1,' + \
                                        str(SpoilerLed9) + ':1,' + \
                                        str(SpoilerLed10) + ':1'
                                    else:
                                        workingFields[1] = '0,' + \
                                        str(SpoilerLed1) + ':0,' + \
                                        str(SpoilerLed2) + ':0,' + \
                                        str(SpoilerLed3) + ':0,' + \
                                        str(SpoilerLed4) + ':0,' + \
                                        str(SpoilerLed5) + ':0,' + \
                                        str(SpoilerLed6) + ':0,' + \
                                        str(SpoilerLed7) + ':0,' + \
                                        str(SpoilerLed8) + ':0,' + \
                                        str(SpoilerLed9) + ':0,' + \
                                        str(SpoilerLed10) + ':0'     
                                    
                                    
                                
                                if output_assignments[workingkey]['IP'] != None and output_assignments[workingkey]['Field'] != None:
                                    FindTarget(str (output_assignments[workingkey]['IP']),
                                               str (output_assignments[workingkey]['Field']) + ':' +
                                               workingFields[1])


                           
                        
    
                    except Exception as other:
                        print('')
                        print('WARNING - Unable to read record of interest in ProcessReceivedString')
                        print('WARNING - Record name is: "' + workingkey + '"')
                        print('')
                        logging.critical("Error in ProcessReceivedString: " + str(other))
                


            logging.debug('Continuing on')
            


    except Exception as other:
        logging.critical("Error in ProcessReceivedString: " + str(other))


def Translate_Value(value_to_process, input_array, calibration_array):

    # This basically provides a lookup table to translate an input
    # value to something that is calibrated on a servo
    #
    # May initially just replicate this module
    # for each guage or should do something like pass the arrays

    # Sample arrys which will be passeod from calling module
    # speeds = [260,250,240,230,220,210,200,190,180,170,160,150,140,130,120,110,100,90 ,80 ,70 ,60 ,50 ,40 ,30 ,0]
    # pos    = [29 ,35 ,40 ,45 ,50 ,55 ,60 ,67 ,72 ,78 ,86 ,95 ,102,109,118,125,131,143,150,157,161,171,173,180,180]



    local_value = 0
    
    logging.info('value_to_process at start of translate is :' + value_to_process)

    # Convert to int for airspeed
    try:
        local_value = int(value_to_process)
    except ValueError:
        logging.critical("None numeric value passed to Translate Value. Value is :" + value_to_process)
        #Center pointer - hopefully lowest risk
        return('180')
    
    # Have integer - no do range check
    
    if (len(input_array) != len(calibration_array)):
        logging.critical("Lookup arrays have different lengths. input has " + str(len(input_array)) + \
                   " calibration has " + str(len(calibration_array)))
        


    if (local_value > input_array[0]):
        # Nice easy one already at max, so just return that
        value_to_process = calibration_array[0]
    else:
        try:
            for i in range( len(input_array)):
                logging.debug(str(i) + " " + str(input_array[i]) + " " + str(calibration_array[i]))
                if (local_value >= input_array[i]):
                    value_to_process = calibration_array[i]

                    # Calc difference between and last step
                    # Note values in array must start at max and decrement to 0
                    # Determine if incremental calculation is needed
                    if (local_value != input_array[i]):
                        input_difference_percent = (local_value - input_array[i]) * 100/(input_array[i-1] - input_array[i])
                        output_difference = calibration_array[i] - calibration_array[i-1]

                        positional_change = int((input_difference_percent/100) * output_difference)
                        
                        logging.debug("Input Difference is " + str(input_difference_percent))
                        logging.debug("Output Difference is " + str(output_difference))                        
                        logging.debug("Positional change is " + str(positional_change))

                        value_to_process = value_to_process - positional_change
                    break
        except Exception as other:
            logging.critical('[e] Error in Translate_Value: ' + str(other))
            





    logging.info('value_to_process at end of translate is :' + str(value_to_process))
    return(str(value_to_process))





  
def RemoveUnwantedCharacters(stringToBeCleaned):
    stringToBeCleaned= stringToBeCleaned.replace('"', '')
    stringToBeCleaned = stringToBeCleaned.strip(' ')
    return(stringToBeCleaned)



# Sample File
##{ 
##    
##    "15": {
##        "IP": "127.0.0.1:2000",
##        "Field": "25", 
##        "Description": "Bulb 15" 
##        
##    }, 
##    "16": {
##        "IP": "127.0.0.1:2000",
##        "Field": "25", 
##        "Description": "Bulb 16" 
##    },
##    "17": {   
##	"Description": ""       
##    }, 
##
##    "ALT": {
##        "IP": "127.0.0.1:2003",
##        "Field": "2", 
##        "Description": "Altimeter" 
##        
##    }, 
##    "Airspeed": {
##        "IP": "127.0.0.1:2004",
##        "Field": "1", 
##        "Description": "Airspeed" 
##        
##    }, 
##    "VS": {
##        "IP": "127.0.0.1:2006",
##	  "Datatype": "str",
##        "Field": "1", 
##        "Description": "Vertical Speed" 
##        
##    }
##}
def CheckifAssignmentFileExists():
    try:
        if not (os.path.isfile(output_assignments_file)):

            print('Unable to find "' + output_assignments_file + '"')
            print('Creating default Ouput Assignments in: ' + output_assignments_file)

            dictOuter = {}
            dictInner = {}
            outercounter = 0
            while outercounter < 3:
                counter = 0
                while counter < 1:
                    dictInner = {}
                    dictInner['IP'] = None
                    dictInner['Description'] = None
                    dictInner['Field'] = None
                    dictInner['Datatype'] = 'str'
                    dictInnter['ManuallyCalcValue'] = 'False'

                    dictOuter[ '%.2d' % (outercounter) + ":" + '%.3d' % (counter)] = dictInner
                    counter = counter + 1

                    
                outercounter = outercounter + 1

            json.dump(dictOuter, fp=open(output_assignments_file,'w'),indent=4)

            print('Created Output Assignments file: "' + output_assignments_file + '"')
        
    except Exception as other:
        logging.critical("Error in CheckifAssignmentFileExists: " + str(other))


def main():



    global output_assignments, serverSock
    
    dictInputAssignments = {}


    CheckifAssignmentFileExists()


    # If unable to successfully load assignments we've hit a fatal error - so exit           
    print('Loading Output Assignments from: "' + output_assignments_file +'"')              
    try:
        output_assignments = json.load(open(output_assignments_file))

    except Exception as other:
        logging.critical("Unexpected error while reading file: '" + output_assignments_file + "'" + str(other))                             
        serverSock.close()
        sys.exit(0)


    
    ### Check Configuration file for basic health
    ### If not in shape correct records in memory and export a copy
    ### Not automaitcally overwriting current version just in case something silly occurs    
    try:
        CorrectionMade = False
        logging.debug("Checking output assignments file " + output_assignments_file + " for correctness ")
        for i in output_assignments:
            logging.debug("Checking entry " + i)

            
            try:
                if output_assignments[i]['Datatype'] == None:
                    ManuallyCalcValue("No datatype assigned for :" + i)
            except KeyError:
                logging.critical("Adding field datatype to record :" + i)
                output_assignments[i]['Datatype'] = "str"
                CorrectionMade = True


            try:
                if output_assignments[i]['ManuallyCalcValue'] == None:
                    logging.critical("No Manual Calc value assigned for :" + i)
            except KeyError:
                logging.critical("Adding field ManuallyCalcValue to  :" + i)             
                output_assignments[i]['ManuallyCalcValue'] = "False"
                CorrectionMade = True


            

        if (CorrectionMade == True):
            CopyOfAssignmentsFile = "copyof_output_assignments"
            logging.critical("Making a copy of the output file to " + CopyOfAssignmentsFile)  
            json.dump(output_assignments, fp=open( CopyOfAssignmentsFile + '.json','w'),indent=4)


        
    except:
        logging.critical("Unexpected error while walking dictinary: '" + output_assignments_file + "'" + str(other))                             
        serverSock.close()
        sys.exit(0)

    logging.debug("Completed checking output assignments file ")

    ### End Check Configuration file for basic health

        
    try:
        print('Waiting for packet')
        ReceivePacket()




    except KeyboardInterrupt:
        # Catch Ctl-C and quit
        print('Exiting')
        serverSock.close()
        sys.exit(0)


    except Exception as other:
        logging.critical("Unhandled error:" + str(other))
    

main()



