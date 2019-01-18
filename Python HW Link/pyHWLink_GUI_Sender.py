import tkinter as tk

root = tk.Tk()
# width x height + x_offset + y_offset:
root.geometry("700x200+30+30")
root.wm_title("pyHWLink GUI Sender")

button_height = 25
button_width = 90



# Begin Set Needed to add Radio Button
#   Copy and Paste to add
#   Buttons are raanged vertically

# Variables that are used
# suggest just increment SwitchX to Switch32
#   SwitchXNum
#   SwitchXs
#   SwitchXval
#   SwitchX_xpos
#   SwitchX_ypos

# Need to also reposition buttons
# Update
SwitchX_xpos = 500
# Update
SwitchX_ypos = 30


# Update
SwitchXNum = tk.IntVar()
# Update
SwitchXNum.set(1)

# Update
SwitchXs = [
    ("1",1),
    ("2",2),
    ("3",3),
    ("4",4),
    ("5",5)
]

# Update
def ShowModules():
    print(SwitchXNum.get())

# Update
for SwitchXval, language in enumerate(SwitchXs):
     tk.Radiobutton(root, 
                  text=language,
                  indicatoron = 0,
                  width = 20,
                  padx = 20,
                  # Update  
                  variable=SwitchXNum,
                  #Update  
                  command=ShowModules,
                  value=SwitchXval).place(x = SwitchX_xpos, y = SwitchX_ypos + SwitchXval*30, width=button_width, height=button_height)

# End Set Needed to add Radio Button   






v = tk.IntVar()
v.set(1)  # initializing the choice, i.e. Python

p = tk.IntVar()
p.set(1)  # initializing the choice, i.e. Python




# Update
moduleNum = tk.IntVar()
# Update
moduleNum.set(1)

# Update
Modules = [
    ("1",1),
    ("2",2),
    ("3",3),
    ("4",4),
    ("5",5)
]

# Update
def ShowModules():
    print(moduleNum.get())

for mval, language in enumerate(Modules):
     tk.Radiobutton(root, 
                  text=language,
                  indicatoron = 0,
                  width = 20,
                  padx = 20,
                  # Update  
                  variable=moduleNum,
                  #Update  
                  command=ShowModules,
                  value=mval).place(x = 300, y = 30 + mval*30, width=button_width, height=button_height)


     

languages = [
    ("Python",1),
    ("Perl",2),
    ("Java",3),
    ("C++",4),
    ("C",5)
]

planes = [
    ("A10",1),
    ("737",2),
    ("JetRanger",3),
]



def ShowChoice():
    print(v.get())

def ShowPlane():
    print(p.get())

tk.Label(root, 
         text="Send Commands to Sim",
         justify = tk.LEFT,
         padx = 20).place(x = 5, y = 5, width=150, height=button_height)

for mval, language in enumerate(Modules):
     tk.Radiobutton(root, 
                  text=language,
                  indicatoron = 0,
                  width = 20,
                  padx = 20, 
                  variable=moduleNum, 
                  command=ShowModules,
                  value=mval).place(x = 300, y = 30 + mval*30, width=button_width, height=button_height)

for val, language in enumerate(languages):
     tk.Radiobutton(root, 
                  text=language,
                  indicatoron = 0,
                  width = 20,
                  padx = 20, 
                  variable=v, 
                  command=ShowChoice,
                  value=val).place(x = 20, y = 30 + val*30, width=button_width, height=button_height)

for pval, language in enumerate(planes):
     tk.Radiobutton(root, 
                  text=language,
                  indicatoron = 0,
                  width = 20,
                  padx = 20, 
                  variable=p, 
                  command=ShowPlane,
                  value=pval).place(x = 150, y = 30 + pval*30, width=button_width, height=button_height)
root.mainloop()
