import sys
import logging


MAX_CARD_NUMBER = 3
received_string = 'D0,123:1'

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


    
    match int(switch_number):
          case 0:   # ILS - ROTARY NO RELEASE
              Do_Nothing()
          case 1:   # ILS - ROTARY NO RELEASE
              Do_Nothing()
          case 2:   # COM_COMM_RELAY_SW", "1");
              Do_Nothing()
          case 3:
              Do_Nothing()
          case 4:
              Do_Nothing()
          case 5:
              Do_Nothing()
          case 6:
              Do_Nothing()
          case 7:
              Do_Nothing()
          case 8:
              Do_Nothing()
          case 9:   # STROBE_SW", "1"); #STROBE "BRT"
              Do_Nothing()
          case 10:  # INT_WNG_TANK_SW", "0"); #INHIBIT - PRESSED
              Do_Nothing()
          case 11:  # ILS - ROTARY NO RELEASE
              Do_Nothing()
          case 12:  # ILS - ROTARY NO RELEASE
              Do_Nothing()
          case 13:  # COM_COMM_RELAY_SW", "1");
              Do_Nothing()
          case 14:
              Do_Nothing()
          case 15:
              Do_Nothing()
          case 16:
              Do_Nothing()
          case 17:
              Do_Nothing()
          case 18:
              Do_Nothing()
          case 19:
              Do_Nothing()
          case 20:  # STROBE_SW", "1"); # STROBE "NORMAL"
              Do_Nothing()
          case 21:  # Gen Tie
              Do_Nothing()
          case 22:  # ILS - ROTARY NO RELEASE
              Do_Nothing()
          case 23:  # ILS - ROTARY NO RELEASE
              Do_Nothing()
          case 24:  # COM_CRYPTO_SW", "1");
              Do_Nothing()
          case 25:  # SEL_JETT_BTN", "0"); # "JETT" BUTTON
              Do_Nothing()
          case 26:  # LDG_TAXI_SW", "1"); # LIGHTS "ON"
              Do_Nothing()
          case 27:  # "SELECT JETT KNOB"
              Do_Nothing()
          case 28:  # CMSD_DISPENSE_BTN", "0");
              Do_Nothing()
          case 29:  # CB_FCS_CHAN2", "1");
              Do_Nothing()
          case 30:  # CB_LAUNCH_BAR", "1");
              Do_Nothing()
          case 31:  # FCS_RESET_BTN", "0");
              Do_Nothing()
          case 32:  # TO_TRIM_BTN", "0");
              Do_Nothing()
          case 33:  # ILS - ROTARY NO RELEASE
              Do_Nothing()
          case 34:  # ILS - ROTARY NO RELEASE
              Do_Nothing()
          case 35:  # COM_CRYPTO_SW", "1");
              Do_Nothing()
          case 36:  # LAUNCH_BAR_SW", "0");
              Do_Nothing()
          case 37:
              Do_Nothing()
          case 38:  # "SELECT JETT KNOB"d
              Do_Nothing()
          case 39:  # CB_FCS_CHAN1", "1");
              Do_Nothing()
          case 40:  # CB_SPD_BRK", "1");
              Do_Nothing()
          case 41:
              Do_Nothing()
          case 42:  # GAIN_SWITCH", "0");
              Do_Nothing()
          case 43:
              Do_Nothing()
          case 44:  #ILS - ROTARY NO RELEASE
              Do_Nothing()
          case 45:  #ILS - ROTARY NO RELEASE
              Do_Nothing()
          case 46:  # COM_COMM_G_XMT_SW", "1");
              Do_Nothing()
          case 47:  # EMERGENCY_PARKING_BRAKE_PULL", "1");
              Do_Nothing()
          case 48:  # HOOK_BYPASS_SW", "0"); # HOOK "FIELD"
              Do_Nothing()
          case 49:  # "SELECT JETT KNOB"
              Do_Nothing()
          case 50:  # ENGINE_CRANK_SW", "1"); # ENG CRANK "RIGHT"
              Do_Nothing()
          case 51:  # APU_CONTROL_SW", "0"); # APU "ON"
              Do_Nothing()
          case 52:  # MC_SW", "1");
              Do_Nothing()
          case 53:
              Do_Nothing()
          case 54:  # OBOGS_SW", "0"); # OBOGS "ON"
              Do_Nothing()
          case 55:  # ILS - ROTARY NO RELEASE
              Do_Nothing()
          case 56:  # ILS - ROTARY NO RELEASE
              Do_Nothing()
          case 57:  # COM_COMM_G_XMT_SW", "1");
              Do_Nothing()
          case 58:  # FLAP_SW", "1"); # FLAPS "AUTO"
              Do_Nothing()
          case 59:  # EMERGENCY_PARKING_BRAKE_ROTATE", "1");
              Do_Nothing()
          case 60:  # "SELECT JETT KNOB"
              Do_Nothing()
          case 61:  # ENGINE_CRANK_SW", "1"); # ENG CRANK "LEFT
              Do_Nothing()
          case 62:
              Do_Nothing()
          case 63:  # MC_SW", "1");
              Do_Nothing()
          case 64:  # HYD_ISOLATE_OVERRIDE_SW", "0");
              Do_Nothing()
          case 65:  # OXY_FLOW", "0"); # OXY FLOW "ON"
              Do_Nothing()
          case 66:  # ILS - ROTARY NO RELEASE
              Do_Nothing()
          case 67:  # ILS - ROTARY NO RELEASE
              Do_Nothing()
          case 68:  # COM_IFF_MASTER_SW", "1");
              Do_Nothing()
          case 69:  # FLAP_SW", "1"); # FLAPS "FULL"
              Do_Nothing()
          case 70:  # ANTI_SKID_SW", "0"); #X0--
              Do_Nothing()
          case 71:  # "SELECT JETT KNOB"
              Do_Nothing()
          case 72:
              Do_Nothing()
          case 73:
              Do_Nothing()
          case 74:
              Do_Nothing()
          case 75:
              Do_Nothing()
          case 76:
              Do_Nothing()
          case 77:  # ILS - ROTARY NO RELEASE
              Do_Nothing()
          case 78:  # ILS - ROTARY NO RELEASE
              Do_Nothing()
          case 79:  # COM_IFF_MODE4_SW", "1");
              Do_Nothing()
          case 80:  # SEAT_HEIGHT_SW", "1");
              Do_Nothing()
          case 81:  # EJECTION_HANDLE_SW", "0");
              Do_Nothing()
          case 82:  # FIRE_TEST_SW", "1");
              Do_Nothing()
          case 83:
              Do_Nothing()
          case 84:
              Do_Nothing()
          case 85:
              Do_Nothing()
          case 86:
              Do_Nothing()
          case 87:
              Do_Nothing()
          case 88:  # ILS - ROTARY NO RELEASE
              Do_Nothing()
          case 89:  # ILS - ROTARY NO RELEASE
              Do_Nothing()
          case 90:  # COM_ILS_UFC_MAN_SW", "0");
              Do_Nothing()
          case 91:  # SEAT_HEIGHT_SW", "1");
              Do_Nothing()
          case 92:  # EJECTION_SEAT_ARMED", "1");
              Do_Nothing()
          case 93:  # FIRE_TEST_SW", "1");
              Do_Nothing()
          case 94:
              Do_Nothing()
          case 95:
              Do_Nothing()
          case 96:
              Do_Nothing()
          case 97:
              Do_Nothing()
          case 98:
              Do_Nothing()
          case 99:  # ILS - ROTARY NO RELEASE
              Do_Nothing()
          case 100: # ILS - ROTARY NO RELEASE
              Do_Nothing()
          case 101: # COM_IFF_MODE4_SW", "1");
              Do_Nothing()
          case 102: # SHLDR_HARNESS_SW", "1");  
              Do_Nothing()
          case 103: # EJECTION_SEAT_MNL_OVRD", "1");
              Do_Nothing()
          case 104:
              Do_Nothing()
          case 105:
              Do_Nothing()
          case 106:
              Do_Nothing()
          case 107:
              Do_Nothing()
          case 108:
              Do_Nothing()
          case 109:
              Do_Nothing()
          case 110: # PROBE_SW", "1"); # PROBE "EXTEND"
              Do_Nothing()
          case 111: # EXT_WNG_TANK_SW", "1"); # WING "ORIDE"
              Do_Nothing()
          case 112: # EXT_CNT_TANK_SW", "1"); # TANKS CTR "ORIDE"
              Do_Nothing()
          case 113: # FUEL_DUMP_SW", "0"); # DUMP "ON"
              Do_Nothing()
          case 114: # GEAR_LEVER", "1");
              Do_Nothing()
          case 115:  # GEAR_SILENCE_BTN", "0");
              Do_Nothing()
          case 116:
              Do_Nothing()
          case 117:
              Do_Nothing()
          case 118:
              Do_Nothing()
          case 119:
              Do_Nothing()
          case 120:
              Do_Nothing()
          case 121: # PROBE_SW", "1"); # PROBE "EMERG EXTD"
              Do_Nothing()
          case 122: # EXT_WNG_TANK_SW", "1"); #WING "STOP"
              Do_Nothing()
          case 123: # EXT_CNT_TANK_SW", "1"); # CTR "STOP"
              print("WHOOT doing something")
              Do_Nothing()
          case 125: # GEAR_DOWNLOCK_OVERRIDE_BTN", "0");
              Do_Nothing()
          case 126: # EMERGENCY_GEAR_ROTATE", "1");
              Do_Nothing()
          case 127:
              Do_Nothing()
          case 128:
              Do_Nothing()
          case 129:
              Do_Nothing()
          case 130:
              Do_Nothing()
          case 131:
              Do_Nothing()
          case 132: # COMM1_ANT_SELECT_SW", "1");
              Do_Nothing()
          case 133: # IFF_ANT_SELECT_SW", "1");
              Do_Nothing()
          case 134:
              Do_Nothing()
          case 135:
              Do_Nothing()
          case 136:
              Do_Nothing()
          case 137:
              Do_Nothing()
          case 138:
              Do_Nothing()
          case 139:
              Do_Nothing()
          case 140:
              Do_Nothing()
          case 141:
              Do_Nothing()
          case 142:
              Do_Nothing()
          case 143: # COMM1_ANT_SELECT_SW", "1");
              Do_Nothing()
          case 144: # IFF_ANT_SELECT_SW", "1");
              Do_Nothing()
          case 145:
              Do_Nothing()
          case 146:
              Do_Nothing()
          case 147:
              Do_Nothing()
          case 148:
              Do_Nothing()
          case 149:
              Do_Nothing()
          case 150:
              Do_Nothing()
          case 151:
              Do_Nothing()
          case 152:
              Do_Nothing()
          case 153:
              Do_Nothing()
          case 154:
              Do_Nothing()
          case 155:
              Do_Nothing()
          case 156:
              Do_Nothing()
          case 157:
              Do_Nothing()
          case 158:
              Do_Nothing()
          case 159:
              Do_Nothing()
          case 160:
              Do_Nothing()
          case 161:
              Do_Nothing()
          case 162:
              Do_Nothing()
          case 163:
              Do_Nothing()
          case 164:
              Do_Nothing()
          case 165:
              Do_Nothing()
          case 166:
              Do_Nothing()
          case 167:
              Do_Nothing()
          case 168:
              Do_Nothing()
          case 169:
              Do_Nothing()
          case 170:
              Do_Nothing()
          case 171:
              Do_Nothing()
          case 172:
              Do_Nothing()
          case 173:
              Do_Nothing()
          case 174:
              Do_Nothing()
          case 175:
              Do_Nothing()
          case 176:
              Do_Nothing()
          case 177:
              Do_Nothing()
          case 178:
              Do_Nothing()
          case 179:
              Do_Nothing()
          case _:
           Throw_Warning('Unhandled Switch Number :' + str(switch_number))
      


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
                Throw_Warning('Currently do have code to handle Unit ' + str(input_card_number))



def Do_Nothing():
    # Take a break
    i = 0

def Throw_Warning(warning_string):
    logging.critical(warning_string)
    #print(warning_string)

     
def Throw_Debug(debug_string):
    logging.debug(debug_string)
    #print(debug_string)

def Main():

    Check_String(received_string)
    
Main()
