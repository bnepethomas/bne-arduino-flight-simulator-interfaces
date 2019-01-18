import tkinter as tk

root = tk.Tk()
# width x height + x_offset + y_offset:
root.geometry("170x400+30+30")

v = tk.IntVar()
v.set(1)  # initializing the choice, i.e. Python

p = tk.IntVar()
p.set(1)  # initializing the choice, i.e. Python

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
         text="""Choose your favourite 
programming language:""",
         justify = tk.LEFT,
         padx = 20).place(x = 10, y = 10, width=120, height=25)

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
