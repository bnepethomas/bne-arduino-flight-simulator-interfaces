#!/usr/bin/python

# KeyStroke_Sender.py
# Listens for Keycodes over UDP and delivers keystrokes to whatever code has focus (hopefully a Sim!)
#
# Largely intended for DCS default aircraft that don't have a LUA interface - although should see what DCS BIOS uses
# Website to test https://www.keyboardtester.com/tester.html

# 20190518 DCS differentiates between Left and Right Alt, Ctrl and Shift keys - added keyboards - note Alt may also appear as menu

# DCS and P3d not seeing even a siomple G - due to change in security in windows
# Ref https://www.tenforums.com/software-apps/49635-sendkeys-not-working-windows-10-a-3.html
#SendKeys are locked in Win 10 and 8.1 by the UAC.
#If you need to use Sendkeys you may want to try Disabling it in the Registry :
#HKLM>Software>Microsoft>Windows>CurrentVersion>Policies>System>EnableLUA=0.

import logging
import os
import socket
import sys
import time


import ctypes



from optparse import OptionParser

#logging.basicConfig(format='%(asctime)s:%(levelname)s:%(message)s',level=logging.INFO)
logging.basicConfig(format='%(asctime)s:%(levelname)s:%(message)s',level=logging.DEBUG)
#logging.basicConfig(format='%(asctime)s:%(levelname)s:%(message)s')



MIN_VERSION_PY3 = 5    # min. 3.x version
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

print("IF THIS ISN'T WORKING NOTE THE FOLLOWING!!!")
print('SendKeys are locked in Win 10 and 8.1 by the UAC.')
print('If you need to use Sendkeys you may want to try Disabling it in the Registry :')
print('HKLM>Software>Microsoft>Windows>CurrentVersion>Policies>System>EnableLUA=0.')

# Setup networking
try:
    UDP_IP_ADDRESS = "172.16.1.3"
    # Windows was unable tobind to 0 - checking firewall
    #UDP_IP_ADDRESS = "0"
    UDP_PORT_NO = 7790
    print('Listening on ' + UDP_IP_ADDRESS + ' Port ' + str(UDP_PORT_NO))
    
    serverSock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    serverSock.settimeout(0.0001)
    serverSock.bind((UDP_IP_ADDRESS, UDP_PORT_NO))
          
except Exception as other:
    logging.critical("Error in Network Setup: " + str(other)) 



# msdn.microsoft.com/en-us/library/dd375731
# https://docs.microsoft.com/en-us/windows/desktop/inputdev/virtual-key-codes
VK_TAB  = 0x09
VK_MENU = 0x12

SendInput = ctypes.windll.user32.SendInput

# C struct redefinitions 
PUL = ctypes.POINTER(ctypes.c_ulong)
class KeyBdInput(ctypes.Structure):
    _fields_ = [("wVk", ctypes.c_ushort),
                ("wScan", ctypes.c_ushort),
                ("dwFlags", ctypes.c_ulong),
                ("time", ctypes.c_ulong),
                ("dwExtraInfo", PUL)]

class HardwareInput(ctypes.Structure):
    _fields_ = [("uMsg", ctypes.c_ulong),
                ("wParamL", ctypes.c_short),
                ("wParamH", ctypes.c_ushort)]

class MouseInput(ctypes.Structure):
    _fields_ = [("dx", ctypes.c_long),
                ("dy", ctypes.c_long),
                ("mouseData", ctypes.c_ulong),
                ("dwFlags", ctypes.c_ulong),
                ("time",ctypes.c_ulong),
                ("dwExtraInfo", PUL)]

class Input_I(ctypes.Union):
    _fields_ = [("ki", KeyBdInput),
                 ("mi", MouseInput),
                 ("hi", HardwareInput)]

class Input(ctypes.Structure):
    _fields_ = [("type", ctypes.c_ulong),
                ("ii", Input_I)]


def PressKey(hexKeyCode):
    extra = ctypes.c_ulong(0)
    ii_ = Input_I()
    ii_.ki = KeyBdInput( 0, hexKeyCode, 0x0008, 0, ctypes.pointer(extra) )
    x = Input( ctypes.c_ulong(1), ii_ )
    ctypes.windll.user32.SendInput(1, ctypes.pointer(x), ctypes.sizeof(x))

def ReleaseKey(hexKeyCode):
    extra = ctypes.c_ulong(0)
    ii_ = Input_I()
    ii_.ki = KeyBdInput( 0, hexKeyCode, 0x0008 | 0x0002, 0, ctypes.pointer(extra) )
    x = Input( ctypes.c_ulong(1), ii_ )
    ctypes.windll.user32.SendInput(1, ctypes.pointer(x), ctypes.sizeof(x))





def AltTab():
    """Press Alt+Tab and hold Alt key for 2 seconds
    in order to see the overlay.
    """
    PressKey(VK_MENU)   # Alt
    PressKey(VK_TAB)    # Tab
    ReleaseKey(VK_TAB)  # Tab~
    time.sleep(2)
    ReleaseKey(VK_MENU) # Alt~

#if __name__ == "__main__":
#    AltTab()


# Classic Dictionary Ref
Classic_KeyStrokeDict = { 'A': [0x41],
        'B': [0x42],
        'C': [0x43],
        'D': [0x44],
        'E': [0x45],
        'F': [0x46],
        'G': [0x47],
        'H': [0x48],
        'I': [0x49],
        'J': [0x4A],
        'K': [0x4B],
        'L': [0x4C],
        'M': [0x4D],
        'N': [0x4E],
        'O': [0x4F],
        'P': [0x50],
        'Q': [0x51],
        'R': [0x52],
        'S': [0x53],
        'T': [0x54],
        'U': [0x55],
        'V': [0x56],
        'W': [0x57],
        'X': [0x58],
        'Y': [0x59],
        'Z': [0x5A],

        '0': [0x30],
        '1': [0x31],
        '2': [0x32],
        '3': [0x33],
        '4': [0x34],
        '5': [0x35],
        '6': [0x36],
        '7': [0x37],
        '8': [0x38],
        '9': [0x39],

        'F1': [0x70],
        'F2': [0x71],
        'F3': [0x72],
        'F4': [0x73],
        'F5': [0x74],
        'F6': [0x75],
        'F7': [0x76],
        'F8': [0x77],
        'F9': [0x78],
        'F10': [0x79],
        'F11': [0x7A],
        'F12': [0x7B],

        '-': [0xBD],
        '+': [0xBB],
        '`': [0xC0],                  
        ';': [0xBA],
        '[': [0xDB],                  
        ']': [0xDD],
        '\\': [0xDC],                  
        '=': [0xDD],
        ',': [0xBC],                  
        '.': [0xBE],                
        'FORWARDSLASH': [0xBF],
        'QUOTE': [0xDE],
        'ESC': [0x1B],
        'TAB': [0x09],
        'CAPSLOCK': [0x14],

        'SPACE': [0x20],
        'BACKSPACE': [0x08],          
        'PRNTSCRN': [0x2C],
        'SCROLLLOCK': [0x91],                  
        'PAUSE': [0x13],                
        'INSERT': [0x2D],
        'DELETE': [0x2E],
        'HOME': [0x24],                  
        'END': [0x23],                
        'PGUP': [0x21],                
        'PGDOWN': [0x22],
        'LEFTARROW': [0x25],                
        'RIGHTARROW': [0x27],
        'UPARROW': [0x26],                
        'DOWNARROW': [0x28],
        'NUM0': [0x60],                
        'NUM1': [0x61],
        'NUM2': [0x62],                
        'NUM3': [0x63],
        'NUM4': [0x64],                
        'NUM5': [0x65],
        'NUM6': [0x66],                
        'NUM7': [0x67],
        'NUM8': [0x68],                
        'NUM9': [0x69],
        'NUMPLUS': [0x6B],                
        'NUMMINUS': [0x6D],
        'NUM*': [0x6A],                
        'ENTER': [0x0D],
        'NUMDECIMAL': [0x6E],                
        'NUMDIVIDE': [0x6F],
        'NUMLOCK': [0x90],                
        'xx': [0x0],

        'MEDIAPLAYPAUSE': [0xB3],                
        'MUTE': [0xAD],
        'VOLUP': [0xAF],                
        'VOLDOWN': [0xAE],
        'xx': [0x0],                
        'xx': [0x0],
        'xx': [0x0],                
        'xx': [0x0],
        'xx': [0x0],                
        'xx': [0x0],
        'xx': [0x0],                
        'xx': [0x0],
        'xx': [0x0],                
        'xx': [0x0],
        'xx': [0x0],                
        'xx': [0x0],
        'xx': [0x0],                
        'xx': [0x0],
        'xx': [0x0],                
        'xx': [0x0],
        'xx': [0x0],                
        'xx': [0x0],
                  

                  
        'ALT': [0x12],
        'SHIFT': [0x10],
        'CTRL': [0x11]
                  
                  
                  }

# DX Dictionary Reference - note this works for DirectX - not classic
# May need to provide an option to select dictionarys

KeyStrokeDict = {
        'ESC': [0x01],
        'A': [0x1E],
        'B': [0x30],
        'C': [0x2E],
        'D': [0x20],
        'E': [0x12],
        'F': [0x21],
        'G': [0x22],                        
        'H': [0x23],
        'I': [0x17],
        'J': [0x24],
        'K': [0x25],
        'L': [0x26],
        'M': [0x32],
        'N': [0x31],
        'O': [0x18],
        'P': [0x19],
        'Q': [0x10],
        'R': [0x13],
        'S': [0x1F],
        'T': [0x14],
        'U': [0x16],
        'V': [0x2F],
        'W': [0x11],
        'X': [0x2D],
        'Y': [0x15],
        'Z': [0x2C],

        '0': [0x0B],
        '1': [0x02],
        '2': [0x03],
        '3': [0x04],
        '4': [0x05],
        '5': [0x06],
        '6': [0x07],
        '7': [0x08],
        '8': [0x09],
        '9': [0x0A],

        'F1': [0x3B],
        'F2': [0x3C],
        'F3': [0x3D],
        'F4': [0x3E],
        'F5': [0x3F],
        'F6': [0x40],
        'F7': [0x41],
        'F8': [0x42],
        'F9': [0x43],
        'F10': [0x44],
        'F11': [0x57],
        'F12': [0x58],

        '-': [0x0C],
        '=': [0x0D],
        'BACKSPACE': [0x0E],
        'TAB': [0x0F],
        '[': [0x1A],                  
        ']': [0x1B],
        '\\': [0x2B],
        'CAPSLOCK': [0x3A],
        ';': [0x27],
        'QUOTE': [0x28], # Used Quote instead of ' to remove need for escaping
        'ENTER' : [0X1C],
        ',': [0x33],
        '.': [0x34],
        '/': [0x35],
        
        'SPACE': [0x39],



##        DIK_LMENU = 0x38,
##        DIK_AT = 0x91,
##        DIK_COLON = 0x92,
##        DIK_UNDERLINE = 0x93,
##        DIK_AX = 0x96,
##        DIK_RMENU = 0xB8,



        '+': [0xBB],
        '`': [0x29],                  
                    

        'PRNTSCRN': [0xB7],
        'SCROLLLOCK': [0x46],                  
        'PAUSE': [0x95], # To be validated DIK_STOP
        
        'INSERT': [0xD2],
        'DELETE': [0xD3],
        'HOME': [0xC7],                  
        'END': [0xCF],                
        'PGUP': [0xC9],                
        'PGDOWN': [0xD1],
        'LEFTARROW': [0xCB],                
        'RIGHTARROW': [0xCD],
        'UPARROW': [0xC8],                
        'DOWNARROW': [0xD0],

      
        'NUM0': [0x52],                
        'NUM1': [0x4F],
        'NUM2': [0x50],                
        'NUM3': [0x51],
        'NUM4': [0x4B],                
        'NUM5': [0x4C],
        'NUM6': [0x4D],                
        'NUM7': [0x47],
        'NUM8': [0x48],                
        'NUM9': [0x49],
        
        'NUMPLUS': [0x4E],                
        'NUMMINUS': [0x4A],
        'NUMDECIMAL': [0x53],
        'NUMENTER': [0x9C],      
        'NUMLOCK' : [0x45],        
        'NUMMULTIPLY': [0x37],                
        'NUMDIVIDE': [0xB5],



        'MEDIAPLAYPAUSE': [0xB3],                
        'MUTE': [0xAD],
        'VOLUP': [0xB0],                
        'VOLDOWN': [0xAE],
        'APPS' : [0xDD],
        'POWER' : [0xDE],
        'SLEEP' : [0xDF],
        'WAKE' : [0xE3],
        'SEARCH' : [0xE5],
        'FAVORITE' : [0xE6],
        'WEBHOME' : [0xB2],
        'REFRESH' : [0xE7],
        'WEBSTOP' : [0xE8],
        'WEBFORWARD' : [0xE9],
        'WEBBACK' : [0xEA],
        'COMP' : [0xEB],
        'MAIL' : [0xEC],
        'MEDIASELECT' : [0xED],

##        DIK_LCONTROL = 0x1D,
##        DIK_RCONTROL = 0x9D,
                          
        'ALT':          [0x12],
        'SHIFT':        [0x10],
        'CTRL':         [0x11],
        'LSHIFT':       [0x2A],
        'RSHIFT':       [0x36],
        'LCTRL':        [0x1D],
        'RCTRL':        [0xA3],
        'LALT':         [0x38],
        'RALT':         [0xA5],
        'LWIN':         [0xDB],
        'RWIN':         [0xDC]
                               
}






def OriginalCode():
    print('wait a sec')
    time.sleep(5)
    PressKey(0x47)
    time.sleep(0.3)
    ReleaseKey(0x47)

    print('sleeping 2')
    time.sleep(2)
    print('going to outside view')
    PressKey(0x71)
    ReleaseKey(0x71)
    print('sleeping 2')
    time.sleep(2)
    print('returning to normal view')
    PressKey(0x70)
    ReleaseKey(0x70)

    print('done')

def ReceivePacket():

    # a is used to track the number of timeouts between packets
    # throws a keepalive message to indicate we are still alive
    a=0
    while True:
        
        try:
            data, addr = serverSock.recvfrom(1500)
            
            ReceivedPacket = data.decode('utf-8')
            logging.debug("Message: " + ReceivedPacket)
            ProcessReceivedString(str(ReceivedPacket))
            

            a=0

                                              
        except socket.timeout:
            a=a+1
            if (a > 10000):
                logging.info("Long Receive Timeout")
                a=0
            continue

        
        except Exception as other:
            logging.critical("Error in ReceivePacket: " + str(other)) 


def ProcessReceivedString(ReceivedUDPString):
    global input_assignments
    global send_string
    global learning
    
    logging.debug('Processing UDP String')

    ReceivedUDPString = ReceivedUDPString.upper()

    send_string = ""
    
    try:
        if len(ReceivedUDPString) > 0 :
          
            logging.debug('Stage 1 Processing: ' + str(ReceivedUDPString))

            # Expecting a set of strings that are space delimited
            # If unhandled character arrives log as critical
            # If string contains reserved modifiers ALT CTL SHFT provide special treatement
            # These are held down first and released last

            # Split String
          
            CommandsToProcess = ReceivedUDPString.split()
            print( CommandsToProcess)

            # Ensure list is empty    
            ModifierSet = []

            # Check for reserved modifiers - add to new array
            # and remove from original array
            ModifiersOfInterest = ['ALT', 'CTRL', 'SHIFT', 'LSHIFT', 'RSHIFT', 'LCTRL', 'RCTRL', 'LALT', 'RALT', 'LWIN', 'RWIN' ]
            for ModifierToCheck in ModifiersOfInterest:
                if ModifierToCheck in CommandsToProcess:
                    print('Found an ' + ModifierToCheck)
                    # Remove it from the set and add to ModifierSet
                    CommandsToProcess.remove(ModifierToCheck)
                    ModifierSet.insert(0,ModifierToCheck)



            # Now have twos sets, one contains strings to send and the other modifers           
            # In test mode so don't send modifers just yet
            
            print( CommandsToProcess)

            # Originally was supporting a text string without spaces
            # But that makes it difficult to deal with F1 ESC etc


            print('Modifier Down')
            for ModifierToPress in ModifierSet:
                print('Modifier Down')
                print( KeyStrokeDict.get(ModifierToPress))
                if KeyStrokeDict.get(ModifierToPress) != None:
                    PressKey( int(KeyStrokeDict.get(ModifierToPress)[0]))
                    time.sleep(0.02)
                else:
                    logging.critical('Unable to find :' + ModifierToPress  +
                         ' in KeyStrokeDict in module ProcessReceivedString')


            # Inner exception Management is to ensure that modifers are released
            try:
                    
                print('Command To Send')
                for CommandToSend in CommandsToProcess:
                    print('Entering loop')
                    print('Command To Send is: ' + CommandToSend)
                    
                    if KeyStrokeDict.get(CommandToSend.upper()) != None:


                        
                        print('GOING IN')

                        print(KeyStrokeDict.get(CommandToSend.upper()))
                        print( "Partial" + str(KeyStrokeDict.get(CommandToSend.upper()[0])))
                        
                        print("coming out")
                        print('Pressing Key')    
                        PressKey( int(KeyStrokeDict.get(CommandToSend.upper())[0]))
                        time.sleep(0.02)
                        print('Releasing Key')
                        ReleaseKey(int(KeyStrokeDict.get(CommandToSend.upper())[0]))
                        print('Key Released')
                    else:
                        logging.critical('Unable to find :' + CommandToSend  +
                         ' in KeyStrokeDict in module ProcessReceivedString')
            except Exception as other:
                logging.critical("Error in inner ProcessReceivedString: " + str(other))




            print('Modifier Up')
            for ModifierToRelease in ModifierSet:
                print( KeyStrokeDict.get(ModifierToRelease))
                if KeyStrokeDict.get(ModifierToRelease) != None:
                    time.sleep(0.02)
                    ReleaseKey(int(KeyStrokeDict.get(ModifierToRelease)[0]))
                else:
                    logging.critical('Unable to find :' + ModifierToPress  +
                         ' in KeyStrokeDict in module ProcessReceivedString')


 
    except Exception as other:
        logging.critical("Error in ProcessReceivedString: " + str(other))


def CycleThroughKeycodes():
# Test Harness to check what DirectX applications considers a keycode to be        
    for x in range (0x0,0xFF):

        input("Press a key")
        print("Going to send " + str(x) + " "  +  hex(x))
        
        print("Now set focus on application being tested")
        print('Sleeping for 2 seconds')
        time.sleep(2)
        print('Pressing')
        PressKey(x) # press our char
        time.sleep(0.5)
        ReleaseKey(x) #release our char
        print('Released')
        print("Sent " + str(x) + " "  +  hex(x))


try:
    print('Waiting for packet')
    ReceivePacket()

except KeyboardInterrupt:
    # Catch Ctl-C and quit
    print('Exiting')
    serverSock.close()
    sys.exit(0)

except Exception as other:
    logging.critical("Error in main:" + str(other)) 



##        DIK_ESCAPE = 0x01,
##        DIK_1 = 0x02,
##        DIK_2 = 0x03,
##        DIK_3 = 0x04,
##        DIK_4 = 0x05,
##        DIK_5 = 0x06,
##        DIK_6 = 0x07,
##        DIK_7 = 0x08,
##        DIK_8 = 0x09,
##        DIK_9 = 0x0A,
##        DIK_0 = 0x0B,
##        DIK_MINUS = 0x0C,
##        DIK_EQUALS = 0x0D,
##        DIK_BACK = 0x0E,
##        DIK_TAB = 0x0F,
##        DIK_Q = 0x10,
##        DIK_W = 0x11,
##        DIK_E = 0x12,
##        DIK_R = 0x13,
##        DIK_T = 0x14,
##        DIK_Y = 0x15,
##        DIK_U = 0x16,
##        DIK_I = 0x17,
##        DIK_O = 0x18,
##        DIK_P = 0x19,
##        DIK_LBRACKET = 0x1A,
##        DIK_RBRACKET = 0x1B,
##        DIK_RETURN = 0x1C,
##        DIK_LCONTROL = 0x1D,
##        DIK_A = 0x1E,
##        DIK_S = 0x1F,
##        DIK_D = 0x20,
##        DIK_F = 0x21,
##        DIK_G = 0x22,
##        DIK_H = 0x23,
##        DIK_J = 0x24,
##        DIK_K = 0x25,
##        DIK_L = 0x26,
##        DIK_SEMICOLON = 0x27,
##        DIK_APOSTROPHE = 0x28,
##        DIK_GRAVE = 0x29,
##        DIK_LSHIFT = 0x2A,
##        DIK_BACKSLASH = 0x2B,
##        DIK_Z = 0x2C,
##        DIK_X = 0x2D,
##        DIK_C = 0x2E,
##        DIK_V = 0x2F,
##        DIK_B = 0x30,
##        DIK_N = 0x31,
##        DIK_M = 0x32,
##        DIK_COMMA = 0x33,
##        DIK_PERIOD = 0x34,
##        DIK_SLASH = 0x35,
##        DIK_RSHIFT = 0x36,
##        DIK_MULTIPLY = 0x37,
##        DIK_LMENU = 0x38,
##        DIK_SPACE = 0x39,
##        DIK_CAPITAL = 0x3A,
##        DIK_F1 = 0x3B,
##        DIK_F2 = 0x3C,
##        DIK_F3 = 0x3D,
##        DIK_F4 = 0x3E,
##        DIK_F5 = 0x3F,
##        DIK_F6 = 0x40,
##        DIK_F7 = 0x41,
##        DIK_F8 = 0x42,
##        DIK_F9 = 0x43,
##        DIK_F10 = 0x44,
##        DIK_NUMLOCK = 0x45,
##        DIK_SCROLL = 0x46,
##        DIK_NUMPAD7 = 0x47,
##        DIK_NUMPAD8 = 0x48,
##        DIK_NUMPAD9 = 0x49,
##        DIK_SUBTRACT = 0x4A,
##        DIK_NUMPAD4 = 0x4B,
##        DIK_NUMPAD5 = 0x4C,
##        DIK_NUMPAD6 = 0x4D,
##        DIK_ADD = 0x4E,
##        DIK_NUMPAD1 = 0x4F,
##        DIK_NUMPAD2 = 0x50,
##        DIK_NUMPAD3 = 0x51,
##        DIK_NUMPAD0 = 0x52,
##        DIK_DECIMAL = 0x53,
##        DIK_F11 = 0x57,
##        DIK_F12 = 0x58,
##        DIK_F13 = 0x64,
##        DIK_F14 = 0x65,
##        DIK_F15 = 0x66,
##        DIK_KANA = 0x70,
##        DIK_CONVERT = 0x79,
##        DIK_NOCONVERT = 0x7B,
##        DIK_YEN = 0x7D,
##        DIK_NUMPADEQUALS = 0x8D,
##        DIK_CIRCUMFLEX = 0x90,
##        DIK_AT = 0x91,
##        DIK_COLON = 0x92,
##        DIK_UNDERLINE = 0x93,
##        DIK_KANJI = 0x94,
##        DIK_STOP = 0x95,
##        DIK_AX = 0x96,
##        DIK_UNLABELED = 0x97,
##        DIK_NUMPADENTER = 0x9C,
##        DIK_RCONTROL = 0x9D,
##        DIK_NUMPADCOMMA = 0xB3,
##        DIK_DIVIDE = 0xB5,
##        DIK_SYSRQ = 0xB7,
##        DIK_RMENU = 0xB8,
##        DIK_HOME = 0xC7,
##        DIK_UP = 0xC8,
##        DIK_PRIOR = 0xC9,
##        DIK_LEFT = 0xCB,
##        DIK_RIGHT = 0xCD,
##        DIK_END = 0xCF,
##        DIK_DOWN = 0xD0,
##        DIK_NEXT = 0xD1,
##        DIK_INSERT = 0xD2,
##        DIK_DELETE = 0xD3,
##        DIK_LWIN = 0xDB,
##        DIK_RWIN = 0xDC,
##        DIK_APPS = 0xDD,
##        DIK_BACKSPACE = DIK_BACK,
##        DIK_NUMPADSTAR = DIK_MULTIPLY,
##        DIK_LALT = DIK_LMENU,
##        DIK_CAPSLOCK = DIK_CAPITAL,
##        DIK_NUMPADMINUS = DIK_SUBTRACT,
##        DIK_NUMPADPLUS = DIK_ADD,
##        DIK_NUMPADPERIOD = DIK_DECIMAL,
##        DIK_NUMPADSLASH = DIK_DIVIDE,
##        DIK_RALT = DIK_RMENU,
##        DIK_UPARROW = DIK_UP,
##        DIK_PGUP = DIK_PRIOR,
##        DIK_LEFTARROW = DIK_LEFT,
##        DIK_RIGHTARROW = DIK_RIGHT,
##        DIK_DOWNARROW = DIK_DOWN,
##        DIK_PGDN = DIK_NEXT,
