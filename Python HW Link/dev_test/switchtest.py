import sys
import logging


MAX_CARD_NUMBER = 3
received_string = 'D0,123:1,3:1'

#logging.basicConfig(format='%(asctime)s:%(levelname)s:%(message)s',level=logging.INFO)
logging.basicConfig(format='%(asctime)s:%(levelname)s:%(message)s',level=logging.DEBUG)
#logging.basicConfig(format='%(asctime)s:%(levelname)s:%(message)s')






MIN_VERSION_PY3 = 10   # min. 3.x version
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
    

def Process_Unit_0(switch_number, switch_action):
    print('Need to do something with ' + str(switch_number) + ':' + str(switch_action))


def Check_String(received_string):
    #commandstring = input("Enter Command String")


    print(received_string)
    Throw_Debug('String to check is:' + received_string) 


    if received_string[:1] != 'D':
        Throw_Warning('Received command does not start with D')
        Throw_Warning("Received string is " + received_string)
        return()


    Throw_Debug('Checking Second Character')
    input_card_number = received_string[1:2]
    if input_card_number.isdigit():    
        Throw_Debug('Arduino Unit Number is: ' + input_card_number)       
        input_card_number = int(input_card_number)
        if input_card_number > MAX_CARD_NUMBER:
            Throw_Warning('Arduino Unit Number is larger than allowed (' + str(MAX_CARD_NUMBER) + ')')
            return()
    else:
        Throw_Warning('Arduino Unit number is not a number :' + input_card_number)
        sys.exit()


    passed_values = received_string.split(',')
    if len(passed_values) < 2:
        Throw_Warning('Insufficient number of value pairs passed, only ' + str(len(passed_values)) + ' passed')
        return()

    
    for av_pair in range(1, len(passed_values)):
        Throw_Debug(passed_values[av_pair])
        switch_av_pair = passed_values[av_pair].split(':')

        match input_card_number:
        
            case 0:
                Process_Unit_0(switch_av_pair[0], switch_av_pair[1])
            case _:
                Throw_Warning('Currently not handling Unit ' + str(input_card_number))






def Throw_Warning(warning_string):
    logging.critical(warning_string)
    #print(warning_string)

     
def Throw_Debug(debug_string):
    logging.debug(debug_string)
    #print(debug_string)

def Main():

    Check_String(received_string)
    
Main()
