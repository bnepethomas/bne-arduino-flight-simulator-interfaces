import tkinter as tk
import argparse
import numbers
import logging
import os
import random
import re
import socket
import sys
import time




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


UDP_IP_ADDRESS = "127.0.0.1"
UDP_PORT_NO = 7789
TX_UDP_IP = "127.0.0.1"
TX_UDP_PORT = 26027
UDP_Reflector_IP = "127.0.0.1"
UDP_Reflector_Port = 27000

serverSock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
serverSock.settimeout(0.0001)
serverSock.bind((UDP_IP_ADDRESS, UDP_PORT_NO))



def Send_UDP_Command(command_to_send):
    
    global TX_UDP_IP
    global TX_UDP_PORT
    global UDP_Reflector_IP, UDP_Reflector_Port, SOCK

    logging.debug ("IP target address:" + str(TX_UDP_IP))
    logging.debug ("UDP target port:" + str(TX_UDP_PORT))

    serverSock.sendto(command_to_send.encode('utf-8'), (TX_UDP_IP, TX_UDP_PORT))
    serverSock.sendto(command_to_send.encode('utf-8'), (UDP_Reflector_IP, UDP_Reflector_Port))



def Send_Button_Up_Down(button_Number):
    logging.debug ("Button Down/Up " + str(button_Number))
    strModuleNum = '%.2d' % (Switch00Num.get() + 1)
    Send_Button_Down(button_Number)
    time.sleep(0.1)
    Send_Button_Up(button_Number)


    

def Send_Button_Down(button_Number):
    logging.debug ("Button Down " + str(button_Number))
    strModuleNum = '%.2d' % (Switch00Num.get() + 1)
    Send_UDP_Command('D' + strModuleNum + ':' + str(button_Number) + ':1' )


def Send_Button_Up(button_Number):
    logging.debug ("Button Up " + str(button_Number))
    strModuleNum = '%.2d' % (Switch00Num.get() + 1)
    Send_UDP_Command('D' + strModuleNum + ':' + str(button_Number) + ':0' )
    

root = tk.Tk()
# width x height + x_offset + y_offset:
root.geometry("700x200+30+30")
root.wm_title("pyHWLink GUI Sender")

button_height = 25
button_width = 90


tk.Label(root, 
     text="Send Commands to Sim",
     justify = tk.LEFT,
     padx = 20).place(x = 5, y = 5, width=150, height=button_height)

# DO NOT REPLACE 'X' HERE!!!!!!!

# Begin Set Needed to add Radio Button
#   Copy and Paste to add
#   Then select new text and replace X (case sentitive with new button number be careful to only replace in the new text
#   Then search and replace '# Update' with nothing
#   Buttons are arranged vertically

# Variables that are used
# suggest just increment SwitchX to Switch32
#   SwitchXNum
#   SwitchXs
#   SwitchXval
#   SwitchX_xpos
#   SwitchX_ypos


####################################################################################################
# Start SwitchX                                                                                    #
####################################################################################################


# Need to also reposition buttons
# Note the base buttons here are hiiden as it has a negative x_pos value - need to make positive
SwitchX_xpos = -100
SwitchX_ypos = 10

SwitchXNum = tk.IntVar()
SwitchXNum.set(0)

SwitchXs = [
    ("001",1),
    ("002",2),
    ("003",3),
    ("004",4),
    ("005",5)
]


def ShowSwitchX():
    print(SwitchXNum.get())


for SwitchXval, SwitchXChoices in enumerate(SwitchXs):
    tk.Radiobutton(root, 
        text=SwitchXChoices[0],
        indicatoron = 0,
        width = 20,
        padx = 20,
        variable=SwitchXNum,
        command=ShowSwitchX,
        value=SwitchXval).place(x = SwitchX_xpos, y = SwitchX_ypos + SwitchXval*30, width=button_width, height=button_height)


####################################################################################################
# End SwitchX                                                                                      #
####################################################################################################


####################################################################################################
# Start Select Module Number                                                                       #
####################################################################################################


# Selects which input module it currently being emulated
Switch00_xpos = 150
Switch00_ypos = 10


Switch00Num = tk.IntVar()
Switch00Num.set(0)


Switch00s = [
    ("1",1),
    ("2",2),
    ("3",3),
    ("4",4),
    ("5",5)
]


def ShowSwitch00():
    print("Module " + str(Switch00Num.get() + 1))


for Switch00val, Switch00Choices in enumerate(Switch00s):
    tk.Radiobutton(root, 
        text=Switch00Choices[0],
        indicatoron = 0,
        width = 20,
        padx = 20,
        variable=Switch00Num,
        command=ShowSwitch00,
        value=Switch00val).place(x = Switch00_xpos + Switch00val * 15, y = Switch00_ypos, width=10, height=15)


####################################################################################################
# End Select Module Number                                                                         #
####################################################################################################




####################################################################################################
# Start Switch01                                                                                   #
####################################################################################################



Switch01_xpos = 10
Switch01_ypos = 40
Switch01Num = tk.IntVar()
Switch01Num.set(0)


Switch01s = [
    ("Gear Up",1),
    ("Gar Down",2),
]


def ShowSwitch01():
    print(Switch01Num.get())


for Switch01val, Switch01Choices in enumerate(Switch01s):
     tk.Radiobutton(root, 
                  text=Switch01Choices[0],
                  indicatoron = 0,
                  width = 20,
                  padx = 20,                 
                  variable=Switch01Num,                   
                  command=ShowSwitch01,
                  value=Switch01val).place(x = Switch01_xpos, y = Switch01_ypos + Switch01val*30, width=button_width, height=button_height)


####################################################################################################
# End Switch01          
####################################################################################################


####################################################################################################
# Start Landing Lights
####################################################################################################



Switch02_xpos = 110
Switch02_ypos = 40

Switch02Num = tk.IntVar()
Switch02Num.set(0)

Switch02s = [
    ("Land_Lghts_On",1),
    ("Land_Lghts_Off",2),
]


def ShowSwitch02():
    print(Switch02Num.get())

for Switch02val, Switch02Choices in enumerate(Switch02s):
    tk.Radiobutton(root, 
        text=Switch02Choices[0],
        indicatoron = 0,
        width = 20,
        padx = 20,
        variable=Switch02Num,
        command=ShowSwitch02,
        value=Switch02val).place(x = Switch02_xpos, y = Switch02_ypos + Switch02val*30, width=button_width, height=button_height)


####################################################################################################
# End Flaps
####################################################################################################


####################################################################################################
# Start Incremental Flaps
####################################################################################################


Switch03_xpos = 210
Switch03_ypos = 40


Switch03s = [
    ("Flaps_Inc_Up",1),
    ("Flaps_Inc_Down",2),
]


def ShowSwitch03_1():
    print("Flaps_Inc_Up")
    print("Button Down")
    time.sleep(0.1)
    print("Button Up")

def ShowSwitch03_2():
    print("Flaps_Inc_Down")
    print("Button Down")
    time.sleep(0.1)
    print("Button Up")


tk.Button(root, 
    text="Flaps_Inc_Up",
    width = 20,
    padx = 20,
    command=ShowSwitch03_1,
    ).place(x = Switch03_xpos, y = Switch03_ypos + 0 *30, width=button_width, height=button_height)

tk.Button(root, 
    text="Flaps_Inc_Down",
    width = 20,
    padx = 20,
    command=ShowSwitch03_2,
    ).place(x = Switch03_xpos, y = Switch03_ypos + 1 *30, width=button_width, height=button_height)

####################################################################################################
# End Incremental Flaps
####################################################################################################


####################################################################################################
# Start Pause Exit
####################################################################################################

# Consumes both 4 & 5
Switch04_xpos = 310
Switch04_ypos = 40


def ShowSwitch04():
    print("Pause")
    Send_Button_Up_Down("004")

def ShowSwitch05():
    print("Exit")
    Send_Button_Up_Down("005")



tk.Button(root, 
    text="Pause",
    width = 20,
    padx = 20,
    command=ShowSwitch04,
    ).place(x = Switch04_xpos, y = Switch04_ypos + 0 *30, width=button_width, height=button_height)

tk.Button(root, 
    text="Exit",
    width = 20,
    padx = 20,
    command=ShowSwitch05,
    ).place(x = Switch04_xpos, y = Switch04_ypos + 1 *30, width=button_width, height=button_height)

####################################################################################################
# End Pause Exit
####################################################################################################


####################################################################################################
# Start Wheel Brakes                                                                                    #
####################################################################################################


# Need to also reposition buttons
# Note the base buttons here are hiiden as it has a negative x_pos value - need to make positive
Switch06_xpos = 410
Switch06_ypos = 40

Switch06Num = tk.IntVar()
Switch06Num.set(0)

Switch06s = [
    ("Wheel_Brks_On",1),
    ("Wheel_Brks_Off",2),
]


def ShowSwitch06():
    print(Switch06Num.get())


for Switch06val, Switch06Choices in enumerate(Switch06s):
    tk.Radiobutton(root, 
        text=Switch06Choices[0],
        indicatoron = 0,
        width = 20,
        padx = 20,
        variable=Switch06Num,
        command=ShowSwitch06,
        value=Switch06val).place(x = Switch06_xpos, y = Switch06_ypos + Switch06val*30, width=button_width, height=button_height)


####################################################################################################
# End Wheel Brakes                                                                                     #
####################################################################################################



root.mainloop()
