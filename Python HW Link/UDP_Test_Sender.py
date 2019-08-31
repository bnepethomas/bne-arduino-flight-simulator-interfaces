#!/usr/bin/python

# UDP_Test_Sender - Interactive sender of UDP Test Data
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


logging.basicConfig(format='%(asctime)s:%(levelname)s:%(message)s',level=logging.DEBUG)
#logging.basicConfig(format='%(asctime)s:%(levelname)s:%(message)s')



sock = socket.socket(socket.AF_INET, # Internet
                     socket.SOCK_DGRAM) # UDP
sock.bind((UDP_IP, UDP_PORT))







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

        run_LED_tests = input(
            'Would you like Run LEDs Test [N]: ')
        if run_LED_tests.upper() != 'Y':
            run_LED_tests = False
        else:
            run_LED_tests = True    
        

        run_DigitalOutput_tests = input(
            'Would you like Run Digital Outputs Test [N]: ')
        if run_DigitalOutput_tests.upper() != 'Y':
            run_DigitalOutput_tests = False
        else:
            run_DigitalOutput_tests = True


        run_Servo_tests = input(
            'Would you like Run Servos Test [N]: ')
        if run_Servo_tests.upper() != 'Y':
            run_Servo_tests = False
        else:
            run_Servo_tests = True
            

        if (run_LED_tests == False) and (run_DigitalOutput_tests == False) and (run_Servo_tests == False) :
            prefix_with_D = input(
                'Would you like all commands to be prefex with a D [Y]: ')
            if prefix_with_D.upper() != 'N':
                prefix_with_D = True
            else:
                prefix_with_D = False




                


        logging.debug('UDP target IP: ' + str(target_IP)
                             + '  UDP target Port: ' + str(target_Port))       
        


        while True:

            if run_LED_tests == True:

                no_of_leds = 64

                print('LED Ports run from 0 to 63')
                print('All LEDs on')
                command_string = "D,"
                for x in range(no_of_leds):
                    if x != (no_of_leds - 1):
                        command_string = command_string + str(x) + ":1,"
                    else:
                        command_string = command_string + str(x) + ":1"
                    
                command_string = command_string + chr(10) 
                Send_UDP_Command(command_string)
                

                input('All LEDs off - Press Enter to continue...')
                command_string = "D,"
                for x in range(no_of_leds):
                    if x != (no_of_leds - 1):
                        command_string = command_string + str(x) + ":0,"
                    else:
                        command_string = command_string + str(x) + ":0"
                command_string = command_string + chr(10) 
                Send_UDP_Command(command_string)
                

                input('Walk LEDS - Press Enter to continue...')
                for x in range(no_of_leds):
                    print('LED: ' + str(x))
                    command_string = "D,"
                    command_string = command_string + str(x) + ":1," 
                    if x != 0:
                        command_string = command_string + str(x-1) + ":0" + chr(10)                       
                    Send_UDP_Command(command_string)
                    input('Next LED - Press Enter to continue...')
                    if x == (no_of_leds - 1):
                        command_string = "D,"
                        command_string = command_string + str(x) + ":0" + chr(10)  
                        Send_UDP_Command(command_string)
                input('Starting Again - Press Enter to continue...')
                    

            if run_DigitalOutput_tests == True:

                no_of_digitaloutputs = 7
                digitialOutput_Base_Port = 202

                
                print('Starting at the 3rd Digital Output which maps to port ' + str(digitialOutput_Base_Port))
                print('Testing ' + str(no_of_digitaloutputs) + ' Digital Outputs')
                print('All Digital Outputs on')
                command_string = "D,"
                for x in range(no_of_digitaloutputs):
                    if x != (no_of_digitaloutputs - 1):
                        command_string = command_string + str(x + digitialOutput_Base_Port) + ":1,"
                    else:
                        command_string = command_string + str(x + digitialOutput_Base_Port) + ":1"
                    
                command_string = command_string + chr(10) 
                Send_UDP_Command(command_string)
                

                input('All Digital Outputs off - Press Enter to continue...')
                command_string = "D,"
                for x in range(no_of_digitaloutputs):
                    if x != (no_of_digitaloutputs - 1):
                        command_string = command_string + str(x + digitialOutput_Base_Port) + ":0,"
                    else:
                        command_string = command_string + str(x + digitialOutput_Base_Port) + ":0"
                command_string = command_string + chr(10) 
                Send_UDP_Command(command_string)
                

                input('Walk Digital Outputs - Press Enter to continue...')
                for x in range(no_of_digitaloutputs):
                    print('Digital Output: ' + str(x + digitialOutput_Base_Port))
                    command_string = "D,"
                    command_string = command_string + str(x + digitialOutput_Base_Port) + ":1," 
                    if x != 0:
                        command_string = command_string + str(x - 1 +  digitialOutput_Base_Port) + ":0" + chr(10)                       
                    Send_UDP_Command(command_string)
                    input('Next Digital Output - Press Enter to continue...')
                    if x == (no_of_digitaloutputs - 1):
                        command_string = "D,"
                        command_string = command_string + str(x + digitialOutput_Base_Port) + ":0" + chr(10)  
                        Send_UDP_Command(command_string)
                input('Starting Again - Press Enter to continue...')                


                
            if run_Servo_tests == True:
                no_of_Servos_per_port = 9

                first_base_port = 150

                for servo_no in range(no_of_Servos_per_port):
                    input('Setting to Servo Lower ' + str(servo_no))
                    command_string = "D," + str(servo_no + first_base_port) + ":40" + chr(10)
                    Send_UDP_Command(command_string)
                    input('Mid - Press Enter to continue...')
                    command_string = "D," + str(servo_no + first_base_port) + ":100" + chr(10)
                    Send_UDP_Command(command_string)
                    input('Upper - Press Enter to continue...')                    
                    command_string = "D," + str(servo_no + first_base_port) + ":160" + chr(10)
                    Send_UDP_Command(command_string)
                    input('Setting to Servo Lower ' + str(servo_no))
                    command_string = "D," + str(servo_no + first_base_port) + ":40" + chr(10)
                    Send_UDP_Command(command_string)
                    
                    



                
                
            if (run_LED_tests == False) and (run_DigitalOutput_tests == False) and (run_Servo_tests == False) :
                # Running Interactive tests
                command_string = input('Enter Command String to Send: ')

                if prefix_with_D:
                    command_string = 'D,' + command_string            

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
