import tkinter as tk

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

#########################
# Start SwitchX         #
#########################

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

# Need to also reposition buttons
# Update - note the base buttons here are hiiden as it has a negative x_pos value - need to make positive
SwitchX_xpos = -100
# Update
SwitchX_ypos = 10


# Update
SwitchXNum = tk.IntVar()
# Update
SwitchXNum.set(0)

# Update
SwitchXs = [
    ("1",1),
    ("2",2),
    ("3",3),
    ("4",4),
    ("5",5)
]

# Update
def ShowSwitchX():
    print(SwitchXNum.get())

# Update
for SwitchXval, SwitchXChoices in enumerate(SwitchXs):
     tk.Radiobutton(root, 
                  text=SwitchXChoices[0],
                  indicatoron = 0,
                  width = 20,
                  padx = 20,
                  # Update  
                  variable=SwitchXNum,
                  # Update  
                  command=ShowSwitchX,
                  value=SwitchXval).place(x = SwitchX_xpos, y = SwitchX_ypos + SwitchXval*30, width=button_width, height=button_height)

# End Set Needed to add Radio Button   
#########################
# End SwitchX           #
#########################



languages = [
    ("Python",1),
    ("Perl",2),
    ("Java",3),
    ("C++",4),
    ("C",5)
]

#########################
# Start Switch01        #
#########################


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


#########################
# End Switch01          #
#########################


planes = [
    ("A10",1),
    ("737",2),
    ("JetRanger",3),
]






root.mainloop()
