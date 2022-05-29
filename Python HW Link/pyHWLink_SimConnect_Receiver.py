# pyHWLink_SimConnect_Receiver
# Receives UDP from Arudino and Sends Sim COnnect COmmands

from SimConnect import *
import time
import logging
import os
import socket
import sys

Radians = 57.295779513
FeetInMeters = 3.28084

# Sim Connect Variables
sm = None
aq = None
altitude = None
HEADING_DEGREES_TRUE = None
AIRSPEED_TRUE = None
LATITUDE = None
LONGITUDE = None
GEAR_LEFT_POSITION = None
GEAR_RIGHT_POSITION = None
GEAR_CENTER_POSITION = None
TRAILING_EDGE_FLAPS_LEFT_ANGLE = None

# Local State Variables
# Setting to -1 means the first update should trigger changes which in turn triggers leds
last_center_gear_pos = -1
last_left_gear_pos =  -1 
last_right_gear_pos = -1
last_flaps_pos = -1

command_string = ''
prefix_with_D = ''
target_IP  = '172.16.1.112'
target_Port = 13136

Led_target_IP = '172.16.1.106'



UDP_IP = "0.0.0.0"
UDP_PORT = 7792
UDP_Reflector_IP = "127.0.0.1"
UDP_Reflector_Port = 27000

Source_IP = 0
Source_Port = 0
Last_Source_IP = "127.0.0.1"


nowexiting = False

logging.basicConfig(format='%(asctime)s:%(levelname)s:%(message)s',level=logging.DEBUG)


sock = socket.socket(socket.AF_INET, # Internet
                     socket.SOCK_DGRAM) # UDP
sock.bind((UDP_IP, UDP_PORT))



def Send_UDP_Command(command_to_send):

    global sock
    global target_IP



    try:

##        logging.debug('UDP target IP: ' + str(target_IP)
##                             + '  UDP target port: ' + str(target_Port))
##        logging.debug('Sending: "' + command_to_send + '"')
        
        sock.sendto(command_to_send.encode('utf-8'),
                    (target_IP , target_Port))
        # sock.sendto(command_to_send.encode('utf-8'),
        #            (UDP_Reflector_IP, UDP_Reflector_Port))

    

    except Exception as other:
        logging.critical('Error in Send_UDP_Command: ' + str(other))



def Send_UDP_Led_Command(command_to_send):

    # Prepends the command to be sent with a 'D,' and then sends in a UDP packet

    # This could be optimised to keep adding values to a string
    # and then either sned the string when it has got to a preset length
    # or send the string at the end of the loop
    
    global sock
    global Led_target_IP

    command_to_send = "D," + command_to_send

    try:

        logging.debug('UDP target IP: ' + str(Led_target_IP)
                             + '  UDP target port: ' + str(target_Port))
        logging.debug('Sending: "' + command_to_send + '"')
        
        sock.sendto(command_to_send.encode('utf-8'),
                    (Led_target_IP , target_Port))
        # sock.sendto(command_to_send.encode('utf-8'),
        #            (UDP_Reflector_IP, UDP_Reflector_Port))

    

    except Exception as other:
        logging.critical('Error in Send_UDP_Led_Command: ' + str(other))


def CleanUpAndExit():
    global nowexiting
    
    try:
        # Catch Ctl-C and quit
        nowexiting = True
        print('')
        print('Exiting')
        print('')


    except Exception as other:
        logging.critical('Error in CleanUpAndExit: ' + str(other))
        sys.exit(0)




def StartSimConnect():
    # Create SimConnect link

    global sm
    global aq
    global altitude 
    global HEADING_DEGREES_TRUE 
    global AIRSPEED_TRUE
    global LATITUDE 
    global LONGITUDE
    global GEAR_CENTER_POSITION
    global GEAR_LEFT_POSITION
    global GEAR_RIGHT_POSITION
    global TRAILING_EDGE_FLAPS_LEFT_ANGLE

    
    try: 
        sm = SimConnect()
        print("Sim Connected")    
        # Note the default _time is 2000 to be refreshed every 2 seconds
        print("Asking for aircraft info")
        aq = AircraftRequests(sm, _time=2000)
        print("Configured Aircraft Requests")
        # Use _time=ms where ms is the time in milliseconds to cache the data.
        # Setting ms to 0 will disable data caching and always pull new data from the sim.
        # There is still a timeout of 4 tries with a 10ms delay between checks.
        # If no data is received in 40ms the value will be set to None
        # Each request can be fine tuned by setting the time param.

        # To find and set timeout of cached data to 200ms:
        print("Grabbing Altitude")
        altitude = aq.find("PLANE_ALTITUDE")
        altitude.time = 200
        print("Altitude is: " + str(altitude.value))

        time.sleep(1)
        print("Setting Lights")
        landing = aq.find("LIGHT_LANDING")
        landing.time = 200
        print("Landing Light is: " + str(landing.value))
        landingvar = 1.0
        aq.set("LIGHT_LANDING",landingvar)
        print("Landing Light is: " + str(landing.value))
        print("Lights Set")


        
        aq.set("PLANE_ALTITUDE", altitude.value + 0.5)


        ae = AircraftEvents(sm)
        # Trigger a simple event
        event_to_trigger = ae.find("AP_MASTER")  # Toggles autopilot on or off
        event_to_trigger()
        time.sleep(2)


       
       
    except Exception as error:
       error_string = str(error)
       print(error_string)
       print("Sim not running?")
       time.sleep(2)
       
       

def Update_Sim_Variables():
    global sm
    global aq
    global altitude 
    global HEADING_DEGREES_TRUE 
    global AIRSPEED_TRUE
    global LATITUDE 
    global LONGITUDE


    while True:
        print("Altitude is: " + str(altitude.value))

        
        time.sleep(0.25)

def Main():
    print("Starting SimConnect Receiver")



    
    while not nowexiting:
        try:
            StartSimConnect()

            Update_Sim_Variables()

        except KeyboardInterrupt:
            # Catch Ctl-C and quit
            CleanUpAndExit()
            
        except Exception as other:
            logging.critical('Error in Main: ' + str(other))



Main()
