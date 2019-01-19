import tkinter as tk
import time

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
    ("1",1),
    ("2",2),
    ("3",3),
    ("4",4),
    ("5",5)
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


Switch04_xpos = 310
Switch04_ypos = 40


def ShowSwitch04_1():
    print("Pause")
    print("Button Down")
    time.sleep(0.1)
    print("Button Up")

def ShowSwitch04_2():
    print("Exit")
    print("Button Down")
    time.sleep(0.1)
    print("Button Up")


tk.Button(root, 
    text="Pause",
    width = 20,
    padx = 20,
    command=ShowSwitch04_1,
    ).place(x = Switch04_xpos, y = Switch04_ypos + 0 *30, width=button_width, height=button_height)

tk.Button(root, 
    text="Exit",
    width = 20,
    padx = 20,
    command=ShowSwitch04_2,
    ).place(x = Switch04_xpos, y = Switch04_ypos + 1 *30, width=button_width, height=button_height)

####################################################################################################
# End Pause Exit
####################################################################################################



root.mainloop()
