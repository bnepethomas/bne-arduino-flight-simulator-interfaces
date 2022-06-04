# pyHWLink_SimConnect_Receiver
# Receives UDP from Arudino and Sends Sim Connect Commands

# It looks like a number of events either aren't natively supported in the Simconnect python SDK
# or not supported in MSFS. eg NAV_LIGHTS_ON
# The good news is these are easily added
# Edit (after saving a copy) EventList.py
# C:\Users\admin\AppData\Local\Programs\Python\Python37\Lib\site-packages\SimConnect
# Added turn on Taxi Lights
#(b'TOGGLE_NAV_LIGHTS', "Toggle navigation lights", "All aircraft"),
#(b'NAV_LIGHTS_ON', "Turn on navigation lights", "All aircraft"),

# It would be nice to also pass a parameter to use the Set commands PARKING_BRAKE_SET

from SimConnect import *
import time
import datetime
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


nowexiting = False

logging.basicConfig(format='%(asctime)s:%(levelname)s:%(message)s',level=logging.DEBUG)



# Global Variables
debugging = True
config_file = 'pyHWLink_SimConnect_Receiver.py'
secondsBetweenKeepAlives = 5

# Initialise keepalive indicator
last_time_display = time.time()
packets_processed = 0

# Address and Port to listen on
UDP_IP_Address = ""
UDP_Port = 7791
serverSock = None

# Last time a packet was received - used to just a default location to GPS to keep it alive
last_time_packet_received = datetime.datetime.now()



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
        aq = AircraftRequests(sm, _time=500)
        ae = AircraftEvents(sm)
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

##        time.sleep(1)
##        print("Setting Lights")
##        landing = aq.find("LIGHT_NAV_ON")
##        landing.time = 200
##        print("LIGHT_NAV_ON Light is: " + str(landing.value))
##        landingvar = 0
##        aq.set("FLAP_POSITION_SET",1)
##        time.sleep(1)
##        print("LIGHT_NAV_ON Light is: " + str(landing.value))
##        print("Lights Set")


        
        #aq.set("PLANE_ALTITUDE", altitude.value + 1)



        
        # Trigger a simple event
        
        event_to_trigger = ae.find("NAV_LIGHTS_ON")
        event_to_trigger()       
##        time.sleep(3)
##        event_to_trigger = ae.find("FLAPS_DECR")
##        event_to_trigger()
##        time.sleep(3)

        ### Working Events
        #event_to_trigger = ae.find("FLAPS_INCR")  
        #event_to_trigger = ae.find("FLAPS_DECR")  
        #event_to_trigger = ae.find("AP_MASTER")
        #event_to_trigger = ae.find("GEAR_UP")
        #event_to_trigger = ae.find("PARKING_BRAKES")
        #event_to_trigger = ae.find("LANDING_LIGHTS_OFF")

       
       
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


def ReceivePacket():

    global serverSock
    
    global last_time_display
    global packets_processed
    global last_time_packet_received

    iterations_Since_Last_Packet = 0


    while True:
        
        try:

            data, (Source_IP, Source_Port)  = serverSock.recvfrom(1500)
            
            logging.debug("Packet Received " + str(Source_Port))

            ReceivedPacket = data.decode()
                
            packets_processed = packets_processed + 1

            Source_IP = str(Source_IP)
            Source_Port = str(Source_Port)
            #ReceivedPacket = str(ReceivedPacket)
            
            logging.debug("From: " + Source_IP + " " + Source_Port)
            logging.debug("Message: " + str(ReceivedPacket))

            ProcessReceivedString( str(ReceivedPacket), Source_IP , str(Source_Port) )
            
            logging.debug("Iterations since last packet " + str(iterations_Since_Last_Packet))
            
            iterations_Since_Last_Packet=0
            last_time_packet_received = datetime.datetime.now()
            

                                              
        except socket.timeout:
            iterations_Since_Last_Packet = iterations_Since_Last_Packet +  1

            timesincelastpacket = datetime.datetime.now() - last_time_packet_received
            
            if (timesincelastpacket.seconds > 5):
                print("[i] Mid Receive Timeout - " + time.asctime())
                last_time_packet_received = datetime.datetime.now()
                iterations_Since_Last_Packet=0


                
            # Throw something on console to show we haven't died
            if time.time() - last_time_display > secondsBetweenKeepAlives:
                
                #Calculate Packets per Second
                if packets_processed != 0:
                    packets_per_Second = packets_processed / secondsBetweenKeepAlives
                else:
                    packets_per_Second = 0
                
                pps_string = ". " + str(packets_per_Second) + " packets per second."
                
                logging.info('Keepalive check ' + str(packets_processed)
                             + ' Packets Processed' + pps_string)
                
                last_time_display = time.time()
                packets_processed = 0

                
            continue

        except Exception as other:
            logging.critical('Error in ReceivePacket: ' + str(other))
            

        if time.time() - last_time_display > 5:
            print('Keepalive ' + time.asctime())
            last_time_display = time.time()




def ProcessReceivedString(ReceivedUDPString, Source_IP, Source_Port):

        logging.debug('Processing Switch String')        


def Main():
    print("Starting SimConnect Receiver")

    global serverSock
    
    # Start UDP Listening Port
    # The timeout is aggressive
    serverSock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    serverSock.settimeout(0.0001)
    serverSock.bind((UDP_IP_Address, UDP_Port))



    
    while not nowexiting:
        try:

            ReceivePacket()
##            StartSimConnect()
##
##            Update_Sim_Variables()

        except KeyboardInterrupt:
            # Catch Ctl-C and quit
            CleanUpAndExit()
            
        except Exception as other:
            logging.critical('Error in Main: ' + str(other))



Main()
