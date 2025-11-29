#!/usr/bin/python

# UDP_Gauge_Test_Sender - Interactive sender of UDP Test Data
# https://github.com/bnepethomas/bne-arduino-flight-simulator-interfaces

# Samrs

import logging
import os
import socket
import sys
import time


import random



# Global Variables
debugging = True
command_string = ''
prefix_with_D = ''
target_IP  = ''
target_Port = ''


UDP_IP = "0.0.0.0"
UDP_PORT = 7791
UDP_Reflector_IP = "127.0.0.1"
UDP_Reflector_Port = 27000

Source_IP = 0
Source_Port = 0
Last_Source_IP = "127.0.0.1"


#logging.basicConfig(format='%(asctime)s:%(levelname)s:%(message)s',level=logging.DEBUG)
logging.basicConfig(format='%(asctime)s:%(levelname)s:%(message)s')



sock = socket.socket(socket.AF_INET, # Internet
                     socket.SOCK_DGRAM) # UDP
sock.bind((UDP_IP, UDP_PORT))

def New_Translate_Value(value_to_process, input_array, calibration_array):

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



def Translate_Value(value_to_process):

    # This basically provides a lookup table to translate an input
    # value to something that is calibrated on a servo
    #
    # May initially just replicate this module
    # for each guage or should do something like pass the arrays
    

    speeds = [260,250,240,230,220,210,200,190,180,170,160,150,140,130,120,110,100,90 ,80 ,70 ,60 ,50 ,40 ,30 ,0]
    pos    = [29 ,35 ,40 ,45 ,50 ,55 ,60 ,67 ,72 ,78 ,86 ,95 ,102,109,118,125,131,143,150,157,161,171,173,180,180]


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



    
    if (len(speeds) != len(pos)):
        logging.critical("Lookup arrays have different lengths. speed has " + str(len(speeds)) + \
                   " pos has " + str(len(pos)))
        


    if (local_value > speeds[0]):
        # Nice easy one already at max, so just return that
        value_to_process = pos[0]
    else:
        try:
            for i in range( len(speeds)):
                logging.debug(str(i) + " " + str(speeds[i]) + " " + str(pos[i]))
                if (local_value >= speeds[i]):
                    value_to_process = pos[i]

                    # Calc difference between and last step
                    # Note values in array must start at max and decrement to 0
                    # Determine if incremental calculation is needed
                    if (local_value != speeds[i]):
                        input_difference_percent = (local_value - speeds[i]) * 100/(speeds[i-1] - speeds[i])
                        output_difference = pos[i] - pos[i-1]

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



def Send_UDP_Command(command_to_send):


    global sock



    try:

        logging.debug('UDP target IP: ' + str(target_IP)
                             + '  UDP target port: ' + str(target_Port))
        logging.debug('Sending: "' + command_to_send + '"')
        
        sock.sendto(command_to_send.encode('utf-8'),
                    (target_IP , target_Port))
        sock.sendto(command_to_send.encode('utf-8'),
                    (UDP_Reflector_IP, UDP_Reflector_Port))

    

    except Exception as other:
        logging.critical('Error in Send_UDP_Command: ' + str(other))




def main():

    speeds = [260,250,240,230,220,210,200,190,180,170,160,150,140,130,120,110,100,90 ,80 ,70 ,60 ,50 ,40 ,30 ,0]
    pos    = [29 ,35 ,40 ,45 ,50 ,55 ,60 ,67 ,72 ,78 ,86 ,95 ,102,109,118,125,131,143,150,157,161,171,173,180,180]

    global prefix_with_D
    global target_IP, target_Port

    try:
        print('UDP_Test_Sender - ctl-c at anytime to exit')
        print('')
        target_IP = input('Enter Target IP Address [172.16.1.21]: ')
        if len(target_IP) == 0:
            target_IP = '172.16.1.21'



        target_Port = ''
        while not isinstance(target_Port, int):
            target_Port = input('Enter Target Port [13135]: ')
            if len(target_Port) == 0:
                target_Port = 13135
            try:
                target_Port = int(target_Port)
                
            except:
                target_Port = ''
                print('Target Port needs to be an integer between 1 and 65535')



        logging.debug('UDP target IP: ' + str(target_IP)
                             + '  UDP target Port: ' + str(target_Port))       
        


        while True:


            command_string = input('Enter Value String to Send: ')


            command_string = New_Translate_Value(command_string, speeds, pos )
        
            command_string = 'D,150:' + command_string


            command_string = command_string + chr(10) 
            Send_UDP_Command(command_string)

            

    except KeyboardInterrupt:
        # Catch Ctl-C and quit
        print('Exiting')
        print('')
        sock.close()
        sys.exit(0)
        
    except Exception as other:
        logging.critical('[e] Error in Main: ' + str(other))




main()
