from tkinter import *
from PIL import ImageTk, Image
import os

root = Tk()


def RBGAImage(path):
    return Image.open(path).convert("RGBA")

class SimpleApp(object):
    def __init__(self, master, **kwargs):
        self.master = master



        master.after(1000, self.update_image)

    def update_image(self):

        img2 = ImageTk.PhotoImage(Image.open("HSI copy 2.png"))
        self.panel1 = Label(root, image = img2)
        self.panel1.pack(side = "top", fill = "both", expand = "yes")
        self.panel1.after(1000, self.update_image)
        print('Alive')


img = ImageTk.PhotoImage(Image.open("HSI copy 2.png"))
##panel = Label(root, image = img)
##panel.pack(side = "bottom", fill = "both", expand = "yes")


app = SimpleApp(root)

root.mainloop()
