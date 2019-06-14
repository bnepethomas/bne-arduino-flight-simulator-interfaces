
#!/usr/bin/python

# PyHWLink_Pri_Node_Output.py

# Receives updates from DCS and distributes to output nodes

# On receive a packet, it builds out an array of AV pairs per destination
# and then throws the assembled array at the target once the packet is parsed

# By default it simple appends the received value from the sim, ie no range
#  checking or anything like that which can work find for binary indicators
#  but doesn't work so value for analog ranges or where special tasks need to
#  be carried out such as a bar display for flaps or spoiler position where
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


import json
import os
import socket
import sys
import time
import logging


from optparse import OptionParser


debugging = False

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

            logging.info(' Sending to targets')

            
            
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


                # 20190615 manually adding prefix - should do this more elegantly! 
                target[device]['Outputstring'] = 'D, ' + target[device]['Outputstring']


                send_string = target[device]['Outputstring']
                logging.info('Sending - ' + str(target[device]['IP']) + ' ' + str(target[device]['Outputstring']))

                
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
            target[targetIP]['Outputstring'] = target[targetIP]['Outputstring'] + ' , ' + stringToAdd
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


##
##                                if (workingkey == "TRAILING_EDGE_FLAPS_LEFT_ANGLE"):
##                                    print( "******************special treatment ******************")
##                                    print('workingFields1 is: ' + str(workingFields[1]))
##                                    if (int(workingFields[1]) > 0):
##                                        workingFields[1] = '1'
##                                    else:
##                                        workingFields[1] = '0'
                                    
                                    
                                
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
                    dictInner['Datatype'] = None

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

### Play area begin
    try:
        for i in output_assignments:
            print(output_assignments[i]['IP'])
            try:
                if output_assignments[i]['Datatype'] == None:
                    print("No datatype assigned")
            except KeyError:
                print("Adding field datatype to record :" + i)
                output_assignments[i]['Datatype'] = "str"
                
            output_assignments[i]['ManuallyCalcValue'] = "False"
        json.dump(output_assignments, fp=open('201606091700.json','w'),indent=4)
##    "GEAR_CENTER_POSITION": {
##        "IP": "172.16.1.21:13135",
##	"Datatype": "str",
##        "Field": "9", 
##        "Description": "Centre Gear Position"  

        
    except:
        logging.critical("Unexpected error while walking dictinary: '" + output_assignments_file + "'" + str(other))                             
        serverSock.close()
        sys.exit(0)
        
    try:
        print('Waiting for packet')
        ReceivePacket()

### Play area end


    except KeyboardInterrupt:
        # Catch Ctl-C and quit
        print('Exiting')
        serverSock.close()
        sys.exit(0)


    except Exception as other:
        logging.critical("Unhandled error:" + str(other))
    

main()



