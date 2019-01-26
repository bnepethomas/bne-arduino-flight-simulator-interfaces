#!/usr/bin/python

# KeyStroke_Sender.py
# Listens for Keycodes over UDP and delivers keystrokes to whatever code has focus (hopefully a Sim!)
#
# Largely intended for DCS default aircraft that don't have a LUA interface - although should see what DCS BIOS uses

import logging
import os
import socket
import sys
import time


import ctypes
from ctypes import wintypes


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


# Setup networking
try:
    UDP_IP_ADDRESS = "192.168.1.138"
    # Windows was unable tobind to 0 - checking firewall
    #UDP_IP_ADDRESS = "0"
    UDP_PORT_NO = 7790
    print('Listening on ' + UDP_IP_ADDRESS + ' Port ' + str(UDP_PORT_NO))
    
    serverSock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    serverSock.settimeout(0.0001)
    serverSock.bind((UDP_IP_ADDRESS, UDP_PORT_NO))
          
except Exception as other:
    logging.critical("Error in Netowrk Setup: " + str(other)) 


user32 = ctypes.WinDLL('user32', use_last_error=True)

INPUT_MOUSE    = 0
INPUT_KEYBOARD = 1
INPUT_HARDWARE = 2

KEYEVENTF_EXTENDEDKEY = 0x0001
KEYEVENTF_KEYUP       = 0x0002
KEYEVENTF_UNICODE     = 0x0004
KEYEVENTF_SCANCODE    = 0x0008

MAPVK_VK_TO_VSC = 0

# msdn.microsoft.com/en-us/library/dd375731
# https://docs.microsoft.com/en-us/windows/desktop/inputdev/virtual-key-codes
VK_TAB  = 0x09
VK_MENU = 0x12

# C struct definitions

wintypes.ULONG_PTR = wintypes.WPARAM

class MOUSEINPUT(ctypes.Structure):
    _fields_ = (("dx",          wintypes.LONG),
                ("dy",          wintypes.LONG),
                ("mouseData",   wintypes.DWORD),
                ("dwFlags",     wintypes.DWORD),
                ("time",        wintypes.DWORD),
                ("dwExtraInfo", wintypes.ULONG_PTR))

class KEYBDINPUT(ctypes.Structure):
    _fields_ = (("wVk",         wintypes.WORD),
                ("wScan",       wintypes.WORD),
                ("dwFlags",     wintypes.DWORD),
                ("time",        wintypes.DWORD),
                ("dwExtraInfo", wintypes.ULONG_PTR))

    def __init__(self, *args, **kwds):
        super(KEYBDINPUT, self).__init__(*args, **kwds)
        # some programs use the scan code even if KEYEVENTF_SCANCODE
        # isn't set in dwFflags, so attempt to map the correct code.
        if not self.dwFlags & KEYEVENTF_UNICODE:
            self.wScan = user32.MapVirtualKeyExW(self.wVk,
                                                 MAPVK_VK_TO_VSC, 0)

class HARDWAREINPUT(ctypes.Structure):
    _fields_ = (("uMsg",    wintypes.DWORD),
                ("wParamL", wintypes.WORD),
                ("wParamH", wintypes.WORD))

class INPUT(ctypes.Structure):
    class _INPUT(ctypes.Union):
        _fields_ = (("ki", KEYBDINPUT),
                    ("mi", MOUSEINPUT),
                    ("hi", HARDWAREINPUT))
    _anonymous_ = ("_input",)
    _fields_ = (("type",   wintypes.DWORD),
                ("_input", _INPUT))

LPINPUT = ctypes.POINTER(INPUT)

def _check_count(result, func, args):
    if result == 0:
        raise ctypes.WinError(ctypes.get_last_error())
    return args

user32.SendInput.errcheck = _check_count
user32.SendInput.argtypes = (wintypes.UINT, # nInputs
                             LPINPUT,       # pInputs
                             ctypes.c_int)  # cbSize

# Functions

def PressKey(hexKeyCode):
    x = INPUT(type=INPUT_KEYBOARD,
              ki=KEYBDINPUT(wVk=hexKeyCode))
    user32.SendInput(1, ctypes.byref(x), ctypes.sizeof(x))

def ReleaseKey(hexKeyCode):
    x = INPUT(type=INPUT_KEYBOARD,
              ki=KEYBDINPUT(wVk=hexKeyCode,
                            dwFlags=KEYEVENTF_KEYUP))
    user32.SendInput(1, ctypes.byref(x), ctypes.sizeof(x))

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


KeyStrokeDict = { 'A': [0x41],
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
        '\/': [0xBF],

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
                  

                  
        'Alt': [0x12],
        'Ctl': [0x11]
                  
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

            ModifiersOfInterest = ['Alt', 'Ctl', 'Shft']
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
                print( KeyStrokeDict.get(ModifierToPress))
                if KeyStrokeDict.get(ModifierToPress) != None:
                    PressKey( int(KeyStrokeDict.get(ModifierToPress)[0]))
                    time.sleep(0.02)
                else:
                    logging.critical('Unable to find :' + ModifierToPress  +
                         ' in KeyStrokeDict in module ProcessReceivedString')


            # Inner exception Management is to ensure that modifers are released
            try:
                    
                print('Command To Sender')
                for CommandToSend in CommandsToProcess:
                    print('Entering loop')
                    print('Command To Send is: ' + CommandToSend)
                    
                    if KeyStrokeDict.get(CommandToSend.upper()) != None:
                        print( KeyStrokeDict.get(CommandToSend.upper()[0]))
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


