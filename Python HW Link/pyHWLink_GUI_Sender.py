import tkinter as tk

root = tk.Tk()
# width x height + x_offset + y_offset:
root.geometry("400x200+30+30")
root.wm_title("pyHWLink GUI Sender")



v = tk.IntVar()
v.set(1)  # initializing the choice, i.e. Python

p = tk.IntVar()
p.set(1)  # initializing the choice, i.e. Python


# Begin Set Needed to add Radio Button
# Copy and Paste to add

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
                  value=mval).place(x = 300, y = 30 + mval*30, width=120, height=25)


# End Set Needed to add Radio Button   
     

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
         padx = 20).place(x = 5, y = 5, width=150, height=25)

for mval, language in enumerate(Modules):
     tk.Radiobutton(root, 
                  text=language,
                  indicatoron = 0,
                  width = 20,
                  padx = 20, 
                  variable=moduleNum, 
                  command=ShowModules,
                  value=mval).place(x = 300, y = 30 + mval*30, width=120, height=25)

for val, language in enumerate(languages):
     tk.Radiobutton(root, 
                  text=language,
                  indicatoron = 0,
                  width = 20,
                  padx = 20, 
                  variable=v, 
                  command=ShowChoice,
                  value=val).place(x = 20, y = 30 + val*30, width=120, height=25)

for pval, language in enumerate(planes):
     tk.Radiobutton(root, 
                  text=language,
                  indicatoron = 0,
                  width = 20,
                  padx = 20, 
                  variable=p, 
                  command=ShowPlane,
                  value=pval).place(x = 150, y = 30 + pval*30, width=120, height=25)
root.mainloop()
