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


# Local State Variables
# Setting to -1 means the first update should trigger changes which in turn triggers leds
last_center_gear_pos = -1
last_left_gear_pos =  -1 
last_right_gear_pos = -1

command_string = ''
prefix_with_D = ''
target_IP  = '172.16.1.112'
target_Port = 13136

Led_target_IP = '172.16.1.106'



UDP_IP = "0.0.0.0"
UDP_PORT = 7791
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


# ######################## CONSTANTS IMPORTED FROM ARDUINO

# Master
# CHIP U1 (C)
SELECT_JET_PANEL = 2
AOA = 2 # NOT DONE
MASTER_ARM =  2

# CHIP U2 (A)
LEFT_EWI = 0
UFC_PANEL = 0
BIT_LED = 0
LEFT_DIS = 0 # CHECK IF NEEDED 
Bit_led = 0 # Bit Test Ledt HUD BOX   

# CHIP U3 (D)
CAUTION_PANEL = 3
LOCK_SHOOT = 3
RIGHT_DIS = 3 #CHECK IF NEEDED

# CHIP U5 (B)
RIGHT_EWI = 1

# CHIP U4 (E)
AOA_DIM = 4

# CHIP U6 (F)
RWR_DIM = 5

#  LEFT EWI 
# NO GO  - LEFT EWI - ORANGE
NO_GO_A_ROW = 0
NO_GO_A_COL = 0
NO_GO_B_ROW = 1
NO_GO_B_COL = 0

# GO  - LEFT EWI - GREEN
GO_A_ROW = 2
GO_A_COL = 0
GO_B_ROW = 3
GO_B_COL = 0

# RIGHT BLEED - LEFT EWI - RED
R_BLEED_A_ROW = 0
R_BLEED_A_COL = 1
R_BLEED_B_ROW = 1
R_BLEED_B_COL = 1

# LEFT BLEED - LEFT EWI - RED
L_BLEED_A_ROW = 2
L_BLEED_A_COL = 1
L_BLEED_B_ROW = 3
L_BLEED_B_COL = 1

# STBY - LEFT EWI - ORANGE
STBY_A_ROW = 0
STBY_A_COL = 2
STBY_B_ROW = 1
STBY_B_COL = 2

# SPD BRK- LEFT EWI - GREEN
SPD_BRK_A_ROW = 2
SPD_BRK_A_COL = 2
SPD_BRK_B_ROW = 3
SPD_BRK_B_COL = 2

# REC - LEFT EWI - ORANGE
REC_A_ROW = 0
REC_A_COL = 3
REC_B_ROW = 1
REC_B_COL = 3

# LAUNCH BAR RED - LEFT EWI - RED
L_BAR_RED_A_ROW = 2
L_BAR_RED_A_COL = 3
L_BAR_RED_B_ROW = 3
L_BAR_RED_B_COL = 3

# XMIT - LEFT EWI - GREEN
XMIT_A_ROW = 0
XMIT_A_COL = 4
XMIT_B_ROW = 1
XMIT_B_COL = 4

# LAUNCH BAR GREEN - LEFT EWI - GREEN
L_BAR_GREEN_A_ROW = 2
L_BAR_GREEN_A_COL = 4
L_BAR_GREEN_B_ROW = 3
L_BAR_GREEN_B_COL = 4

# ASPJ OH - LEFT EWI - ORANGE
ASPJ_OH_A_ROW = 0
ASPJ_OH_A_COL = 5
ASPJ_OH_B_ROW = 1
ASPJ_OH_B_COL = 5

# MASTER CAUTION - LEFT EWI - ORANGE
MSTR_CAUT_A_ROW = 4
MSTR_CAUT_A_COL = 1
MSTR_CAUT_B_ROW = 4
MSTR_CAUT_B_COL = 2
MSTR_CAUT_C_ROW = 5
MSTR_CAUT_C_COL = 1
MSTR_CAUT_D_ROW = 5
MSTR_CAUT_D_COL = 2

# LEFT FIRE - LEFT EWI - RED
LEFT_FIRE_A_ROW = 6
LEFT_FIRE_A_COL = 1
LEFT_FIRE_B_ROW = 6
LEFT_FIRE_B_COL = 2
LEFT_FIRE_C_ROW = 7
LEFT_FIRE_C_COL = 1
LEFT_FIRE_D_ROW = 7
LEFT_FIRE_D_COL = 2

#  RIGHT EWI 
# RCDR ON  - RIGHT EWI - GREEN
RCDR_ON_A_ROW = 0
RCDR_ON_A_COL = 0
RCDR_ON_B_ROW = 1
RCDR_ON_B_COL = 0

# DISP  - RIGHT EWI - GREEN
DISP_A_ROW = 2
DISP_A_COL = 0
DISP_B_ROW = 3
DISP_B_COL = 0

# SPARE 1 - RIGHT EWI - GREEN
SPARE_1_A_ROW = 0
SPARE_1_A_COL = 1
SPARE_1_B_ROW = 1
SPARE_1_B_COL = 1

# SPARE 2 - RIGHT EWI - GREEN
SPARE_2_A_ROW = 2
SPARE_2_A_COL = 1
SPARE_2_B_ROW = 3
SPARE_2_B_COL = 1

# SPARE 3 - RIGHT EWI - GREEN
SPARE_3_A_ROW = 0
SPARE_3_A_COL = 2
SPARE_3_B_ROW = 1
SPARE_3_B_COL = 2

# SPARE 4 - RIGHT EWI - GREEN
SPARE_4_A_ROW = 2
SPARE_4_A_COL = 2
SPARE_4_B_ROW = 3
SPARE_4_B_COL = 2

# SPARE 5 - RIGHT EWI - GREEN
SPARE_5_A_ROW = 0
SPARE_5_A_COL = 3
SPARE_5_B_ROW = 1
SPARE_5_B_COL = 3

# SAM - RIGHT EWI - RED
SAM_A_ROW = 2
SAM_A_COL = 3
SAM_B_ROW = 3
SAM_B_COL = 3

# AI - RIGHT EWI - RED
AI_A_ROW = 0
AI_A_COL = 4
AI_B_ROW = 1
AI_B_COL = 4

# AAA - RIGHT EWI - RED
AAA_A_ROW = 2
AAA_A_COL = 4
AAA_B_ROW = 3
AAA_B_COL = 4

# CW - RIGHT EWI - RED
CW_A_ROW = 0
CW_A_COL = 5
CW_B_ROW = 1
CW_B_COL = 5

#  FIRE 
# APU FIRE - RIGHT EWI - RED
APU_FIRE_A_ROW = 4
APU_FIRE_A_COL = 1
APU_FIRE_B_ROW = 4
APU_FIRE_B_COL = 2
APU_FIRE_C_ROW = 5
APU_FIRE_C_COL = 1
APU_FIRE_D_ROW = 5
APU_FIRE_D_COL = 2

# RIGHT FIRE - RIGHT EWI - RED
RIGHT_FIRE_A_ROW = 6
RIGHT_FIRE_A_COL = 1
RIGHT_FIRE_B_ROW = 6
RIGHT_FIRE_B_COL = 2
RIGHT_FIRE_C_ROW = 7
RIGHT_FIRE_C_COL = 1
RIGHT_FIRE_D_ROW = 7
RIGHT_FIRE_D_COL = 2

#  CAUTION PANEL 
CK_SEAT_COL_A = 0
CK_SEAT_ROW_A = 0
CK_SEAT_COL_B = 0
CK_SEAT_ROW_B = 1

FCS_HOT_COL_A = 1
FCS_HOT_ROW_A = 0
FCS_HOT_COL_B = 1
FCS_HOT_ROW_B = 1

FUEL_LO_COL_A = 2
FUEL_LO_ROW_A = 0
FUEL_LO_COL_B = 2
FUEL_LO_ROW_B = 1

L_GEN_COL_A = 3
L_GEN_ROW_A = 0
L_GEN_COL_B = 3
L_GEN_ROW_B = 1

APU_ACC_COL_A = 0
APU_ACC_ROW_A = 2
APU_ACC_COL_B = 0
APU_ACC_ROW_B = 3

GEN_TIE_COL_A = 1
GEN_TIE_ROW_A = 2
GEN_TIE_COL_B = 1
GEN_TIE_ROW_B = 3

FCES_COL_A = 2
FCES_ROW_A = 2
FCES_COL_B = 2
FCES_ROW_B = 3

R_GEN_COL_A = 3
R_GEN_ROW_A = 2
R_GEN_COL_B = 3
R_GEN_ROW_B = 3

BATT_SW_COL_A = 0
BATT_SW_ROW_A = 4
BATT_SW_COL_B = 0
BATT_SW_ROW_B = 5

C_SPARE_1_COL_A = 1
C_SPARE_1_ROW_A = 4
C_SPARE_1_COL_B = 1
C_SPARE_1_ROW_B = 5

C_SPARE_2_COL_A = 2
C_SPARE_2_ROW_A = 4
C_SPARE_2_COL_B = 2
C_SPARE_2_ROW_B = 5

C_SPARE_3_COL_A = 3
C_SPARE_3_ROW_A = 4
C_SPARE_3_COL_B = 3
C_SPARE_3_ROW_B = 5

#  UFC 
UFC_OPT1_COL_A = 4
UFC_OPT1_ROW_A = 4
UFC_OPT1_COL_B = 3
UFC_OPT1_ROW_B = 4

UFC_OPT2_COL_A = 4
UFC_OPT2_ROW_A = 5
UFC_OPT2_COL_B = 3
UFC_OPT2_ROW_B = 5

UFC_OPT3_COL_A = 4
UFC_OPT3_ROW_A = 6
UFC_OPT3_COL_B = 3
UFC_OPT3_ROW_B = 6

UFC_OPT4_COL_A = 3
UFC_OPT4_ROW_A = 7
UFC_OPT4_COL_B = 4
UFC_OPT4_ROW_B = 7

UFC_OPT5_COL_A = 5
UFC_OPT5_ROW_A = 4

#  JETT STATION SELECT 
SEL_CENTER_COL_A = 0
SEL_CENTER_ROW_A = 0

SEL_LEFT_INNER_COL_A = 0
SEL_LEFT_INNER_ROW_A = 1

SEL_LEFT_OUTER_COL_A = 1
SEL_LEFT_OUTER_ROW_A = 1

SEL_RIGHT_INNER_COL_A = 1
SEL_RIGHT_INNER_ROW_A = 0

SEL_RIGHT_OUTER_COL_A = 2
SEL_RIGHT_OUTER_ROW_A = 0

#  GEAR FLAPS 
NOSE_GEAR_COL_A = 3
NOSE_GEAR_ROW_A = 0
NOSE_GEAR_COL_B = 3
NOSE_GEAR_ROW_B = 1

LEFT_GEAR_COL_A = 2
LEFT_GEAR_ROW_A = 1
LEFT_GEAR_COL_B = 1
LEFT_GEAR_ROW_B = 2

RIGHT_GEAR_COL_A = 4
RIGHT_GEAR_ROW_A = 0
RIGHT_GEAR_COL_B = 4
RIGHT_GEAR_ROW_B = 1

HALF_FLAPS_COL_A = 0
HALF_FLAPS_ROW_A = 2
HALF_FLAPS_COL_B = 2
HALF_FLAPS_ROW_B = 2

FULL_FLAPS_COL_A = 5
FULL_FLAPS_ROW_A = 0
FULL_FLAPS_COL_B = 5
FULL_FLAPS_ROW_B = 1

AMBER_FLAPS_COL_A = 3
AMBER_FLAPS_ROW_A = 2
AMBER_FLAPS_COL_B = 4
AMBER_FLAPS_ROW_B = 2

#  RWR - ALR_67 
RWR_BIT_COL_A = 0
RWR_BIT_ROW_A = 0

RWR_ENABLE_OFFSET_COL_A = 0
RWR_ENABLE_OFFSET_ROW_A = 1

RWR_ENABLE_SPECIAL_COL_A = 0
RWR_ENABLE_SPECIAL_ROW_A = 2

RWR_LIMIT_COL_A = 0
RWR_LIMIT_ROW_A = 3

RWR_ON_COL_A = 0
RWR_ON_ROW_A = 4

RWR_FAIL_RED_COL_A = 1
RWR_FAIL_RED_ROW_A = 0

RWR_OFFSET_COL_A = 1
RWR_OFFSET_ROW_A = 1

RWR_SPECIAL_COL_A = 1
RWR_SPECIAL_ROW_A = 2

RWR_DISPLAY_COL_A = 1
RWR_DISPLAY_ROW_A = 3

RWR_ALR_67_COL_A = 1
RWR_ALR_67_ROW_A = 4

#  LEFT DIS 
GEAR_KNOB_COL_A = 5
GEAR_KNOB_ROW_A = 3

APU_LT_COL_A = 5
APU_LT_ROW_A = 2

#  RIGHT  DIS 
HOOK_COL_A = 5
HOOK_ROW_A = 1
HOOK_COL_B = 5 #6
HOOK_ROW_B = 0 #1

RAD_ALT_R_COL_A = 6 #5
RAD_ALT_R_ROW_A = 1 #0
RAD_ALT_G_COL_A = 6
RAD_ALT_G_ROW_A = 0

#  BIT LIGHT HUD 
# *** not found in DSC FP as yet ***
BIT_GREEN_COL_A = 5
BIT_GREEN_ROW_A = 5

BIT_RED_COL_A = 6
BIT_RED_ROW_A = 5


#  LOCK SHOOT 
LOCK_COL_A = 5
LOCK_ROW_A = 2
LOCK_COL_B = 5
LOCK_ROW_B = 4

AOA_DIM = 4

SHOOT_COL_A = 6
SHOOT_ROW_A = 3
SHOOT_COL_B = 6
SHOOT_ROW_B = 4

SHOOT_FLASH_COL_A = 5
SHOOT_FLASH_ROW_A = 3
SHOOT_FLASH_COL_B = 6
SHOOT_FLASH_ROW_B = 2


#  AOA 
AOA_ABOVE_COL = 0
AOA_ABOVE_ROW = 1

AOA_ON_COL = 1
AOA_ON_ROW = 1

AOA_BELOW_COL = 1
AOA_BELOW_ROW = 0

# ##################### END CONSTANTS IMPORTED FROM ARDUINO

def Set_Nose_Gear_Led(state):

    # LedNumber which has XRRCC where X = Max7219 Unit, RR Row, CC Column
    global SELECT_JET_PANEL
    global NOSE_GEAR_COL_A 
    global NOSE_GEAR_ROW_A 
    global NOSE_GEAR_COL_B 
    global NOSE_GEAR_ROW_B         

    Command = str(SELECT_JET_PANEL) + str(NOSE_GEAR_COL_A).zfill(2) \
                      + str(NOSE_GEAR_ROW_A).zfill(2) + ":" + str(state) + ","
    Command = Command + str(SELECT_JET_PANEL) + str(NOSE_GEAR_COL_B).zfill(2) \
                      + str(NOSE_GEAR_ROW_B).zfill(2) + ":" + str(state)
    Send_UDP_Led_Command(Command)
    


def Set_Left_Gear_Led(state):

    # LedNumber which has XRRCC where X = Max7219 Unit, RR Row, CC Column
    global SELECT_JET_PANEL  
    global LEFT_GEAR_COL_A 
    global LEFT_GEAR_ROW_A 
    global LEFT_GEAR_COL_B 
    global LEFT_GEAR_ROW_B         

    Command = str(SELECT_JET_PANEL) + str(LEFT_GEAR_COL_A).zfill(2) \
                      + str(LEFT_GEAR_ROW_A).zfill(2) + ":" + str(state) + ","
    Command = Command + str(SELECT_JET_PANEL) + str(LEFT_GEAR_COL_B).zfill(2) \
                      + str(LEFT_GEAR_ROW_B).zfill(2) + ":" + str(state)
    Send_UDP_Led_Command(Command)



def Set_Right_Gear_Led(state):

    # LedNumber which has XRRCC where X = Max7219 Unit, RR Row, CC Column
    global SELECT_JET_PANEL
    global RIGHT_GEAR_COL_A 
    global RIGHT_GEAR_ROW_A 
    global RIGHT_GEAR_COL_B 
    global RIGHT_GEAR_ROW_B        

    Command = str(SELECT_JET_PANEL) + str(RIGHT_GEAR_COL_A).zfill(2) \
                      + str(RIGHT_GEAR_ROW_A).zfill(2) + ":" + str(state) + ","
    Command = Command + str(SELECT_JET_PANEL) + str(RIGHT_GEAR_COL_B).zfill(2) \
                      + str(RIGHT_GEAR_ROW_B).zfill(2) + ":" + str(state)
    Send_UDP_Led_Command(Command)



def Set_Full_Flaps_Led(state):

    # LedNumber which has XRRCC where X = Max7219 Unit, RR Row, CC Column
    global SELECT_JET_PANEL
    global FULL_FLAPS_COL_A 
    global FULL_FLAPS_ROW_A 
    global FULL_FLAPS_COL_B 
    global FULL_FLAPS_ROW_B       

    Command = str(SELECT_JET_PANEL) + str(FULL_FLAPS_COL_A).zfill(2) \
                      + str(FULL_FLAPS_ROW_A).zfill(2) + ":" + str(state) + ","
    Command = Command + str(SELECT_JET_PANEL) + str(FULL_FLAPS_COL_B).zfill(2) \
                      + str(FULL_FLAPS_ROW_B).zfill(2) + ":" + str(state)
    Send_UDP_Led_Command(Command)




def Set_Half_Flaps_Led(state):

    # LedNumber which has XRRCC where X = Max7219 Unit, RR Row, CC Column
    global SELECT_JET_PANEL
    global HALF_FLAPS_COL_A
    global HALF_FLAPS_ROW_A
    global HALF_FLAPS_COL_B
    global HALF_FLAPS_ROW_B 

    Command = str(SELECT_JET_PANEL) + str(HALF_FLAPS_COL_A).zfill(2) \
                      + str(HALF_FLAPS_ROW_A).zfill(2) + ":" + str(state) + ","
    Command = Command + str(SELECT_JET_PANEL) + str(HALF_FLAPS_COL_B).zfill(2) \
                      + str(HALF_FLAPS_ROW_B).zfill(2) + ":" + str(state)
    Send_UDP_Led_Command(Command)




def Set_Fuel_Low_Led(state):

    # LedNumber which has XRRCC where X = Max7219 Unit, RR Row, CC Column
    global CAUTION_PANEL
    global FUEL_LO_COL_A
    global FUEL_LO_ROW_A
    global FUEL_LO_COL_B
    global FUEL_LO_ROW_B
    
    Command = str(CAUTION_PANEL) + str(FUEL_LO_COL_A).zfill(2) \
                      + str(FUEL_LO_ROW_A).zfill(2) + ":" + str(state) + ","
    Command = Command + str(CAUTION_PANEL) + str(FUEL_LO_COL_B).zfill(2) \
                      + str(FUEL_LO_ROW_B).zfill(2) + ":" + str(state)
    Send_UDP_Led_Command(Command)

# ##################################### END SETTING LED POSITIONS

# ##################################### BEGIN UPDATE AIRCRAFT VARIABLES

def Update_Gear_Pos():
    global GEAR_CENTER_POSITION
    global GEAR_LEFT_POSITION
    global GEAR_RIGHT_POSITION

    global last_center_gear_pos
    global last_left_gear_pos
    global last_right_gear_pos


    print("GEAR_CENTER_POSITION is: " + str(GEAR_CENTER_POSITION.value))
    print("GEAR LEFT POSITION is: " + str(GEAR_LEFT_POSITION.value))
    print("GEAR_RIGHT_POSITION is: " + str(GEAR_RIGHT_POSITION.value))

##    if last_center_gear_pos != GEAR_CENTER_POSITION.value :
##       last_center_gear_pos = GEAR_CENTER_POSITION.value
       
        

# ##################################### END UPDATE AIRCRAFT VARIABLES

def Send_UDP_Command(command_to_send):

    global sock
    global target_IP



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

        print("Grabbing GEAR_CENTER_POSITION")
        GEAR_CENTER_POSITION = aq.find("GEAR_CENTER_POSITION")
        GEAR_CENTER_POSITION.time = 200
        print("GEAR_CENTER_POSITION is: " + str(GEAR_CENTER_POSITION.value))

        print("Grabbing GEAR_LEFT_POSITION")
        GEAR_LEFT_POSITION = aq.find("GEAR_LEFT_POSITION")
        GEAR_LEFT_POSITION.time = 200
        print("GEAR LEFT POSITION is: " + str(GEAR_LEFT_POSITION.value))

        print("Grabbing GEAR_RIGHT_POSITION")
        GEAR_RIGHT_POSITION = aq.find("GEAR_RIGHT_POSITION")
        GEAR_RIGHT_POSITION.time = 200
        print("GEAR_RIGHT_POSITION is: " + str(GEAR_RIGHT_POSITION.value))



        
       
       
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
        print("AIRSPEED_TRUE is: " + str(AIRSPEED_TRUE.value))
        print("HEADING_DEGREES_TRUE is: " + str(HEADING_DEGREES_TRUE.value * Radians))
        print("LATITUDE is: " + str(LATITUDE.value))
        print("LONGITUDE is: " + str(LONGITUDE.value))
        Send_UDP_Command("altitude:" + str(altitude.value/FeetInMeters) +
             ",magheading:" + str(int(HEADING_DEGREES_TRUE.value * Radians)) +
             ",airspeed:" + str(AIRSPEED_TRUE.value) +
             ",latitude:" + str(LATITUDE.value) +
             ",longitude:" + str(LONGITUDE.value) )

        Update_Gear_Pos()
        
        time.sleep(0.25)

def Main():
    print("Starting SimConnect")

    # Clear all Leds
    Set_Nose_Gear_Led(0)
    Set_Left_Gear_Led(0)
    Set_Right_Gear_Led(0)
    Set_Half_Flaps_Led(0)
    Set_Full_Flaps_Led(0)
    Set_Fuel_Low_Led(0)

    
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
