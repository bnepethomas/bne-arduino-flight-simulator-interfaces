import tkinter as tk
import time
from tkinter import Label
from PIL import ImageTk
from PIL import Image
#from tkinter import *

class SimpleApp(object):
    def __init__(self, master, filename, **kwargs):
        self.master = master
        self.filename = filename
        self.canvas = tk.Canvas(master, width=1000, height=1000)

        self.canvas.pack()

        self.update = self.draw().__next__
        master.after(100, self.update)

    def draw(self):
        image = Image.open(self.filename)

        angle = 0
        while True:
            canvas_obj = self.canvas.create_rectangle(0,0,20,20)
          
            tkimage = ImageTk.PhotoImage(image.rotate(angle))
            canvas_obj = self.canvas.create_image(500, 500, image=tkimage)

            self.master.after_idle(self.update)
            
            yield
            time.sleep(0.1)
            self.canvas.delete(canvas_obj)
            angle += 1
            angle %= 360

root = tk.Tk()
app = SimpleApp(root, 'HSI copy 1.png')
root.mainloop()
