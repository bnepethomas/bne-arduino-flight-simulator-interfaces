from SimConnect import *
import time
import logging
import os
import socket
import sys

Radians = 57.295779513
FeetInMeters = 3.28084

sm = None
aq = None
altitude = None
HEADING_DEGREES_TRUE = None
AIRSPEED_TRUE = None
LATITUDE = None
LONGITUDE = None


command_string = ''
prefix_with_D = ''
target_IP  = '192.168.2.41'
target_Port = 13136


UDP_IP = "0.0.0.0"
UDP_PORT = 7791
UDP_Reflector_IP = "127.0.0.1"
UDP_Reflector_Port = 27000

Source_IP = 0
Source_Port = 0
Last_Source_IP = "127.0.0.1"


logging.basicConfig(format='%(asctime)s:%(levelname)s:%(message)s',level=logging.DEBUG)


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
        # sock.sendto(command_to_send.encode('utf-8'),
        #            (UDP_Reflector_IP, UDP_Reflector_Port))

    

    except Exception as other:
        logging.critical('Error in Send_UDP_Command: ' + str(other))




def CleanUpAndExit():
    try:
        # Catch Ctl-C and quit
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


        print("Grabbing HEADING_DEGREES_TRUE")
        HEADING_DEGREES_TRUE = aq.find("PLANE_HEADING_DEGREES_MAGNETIC")
        HEADING_DEGREES_TRUE.time = 200
        print("HEADING_DEGREES_TRUE is: " + str(HEADING_DEGREES_TRUE.value * Radians))

        print("Grabbing AIRSPEED_TRUE")
        AIRSPEED_TRUE = aq.find("AIRSPEED_TRUE")
        AIRSPEED_TRUE.time = 200
        print("AIRSPEED_TRUE is: " + str(AIRSPEED_TRUE.value))

        print("Grabbing LATITUDE")
        LATITUDE = aq.find("PLANE_LATITUDE")
        LATITUDE.time = 200
        print("LATITUDE is: " + str(LATITUDE.value))

        print("Grabbing LONGITUDE")
        LONGITUDE = aq.find("PLANE_LONGITUDE")
        LONGITUDE.time = 200
        print("LONGITUDE is: " + str(LONGITUDE.value))


       
       
    except Exception as error:
       error_string = str(error)
       print(error_string)
       print("Sim not running?")

def DisplaySimVariables():
    global sm
    global aq
    global altitude 
    global HEADING_DEGREES_TRUE 
    global AIRSPEED_TRUE
    global LATITUDE 
    global LONGITUDE

    while True:
        print("Altitude is: " + str(altitude.value))
        print("AIRSPEED_TRUE is: " + str(AIRSPEED_TRUE.value))
        print("HEADING_DEGREES_TRUE is: " + str(HEADING_DEGREES_TRUE.value * Radians))
        print("LATITUDE is: " + str(LATITUDE.value))
        print("LONGITUDE is: " + str(LONGITUDE.value))
        Send_UDP_Command("altitude:" + str(altitude.value/FeetInMeters) +
             ",magheading:" + str(HEADING_DEGREES_TRUE.value * Radians) +
             ",airspeed:" + str(AIRSPEED_TRUE.value) +
             ",latitude:" + str(LATITUDE.value) +
             ",longitude:" + str(LONGITUDE.value) )
        time.sleep(0.25)

def Main():
    print("Starting SimConnect")

    
    try:
        StartSimConnect()

        DisplaySimVariables()

    except KeyboardInterrupt:
        # Catch Ctl-C and quit
        CleanUpAndExit()
        
    except Exception as other:
        logging.critical('Error in Main: ' + str(other))



Main()
