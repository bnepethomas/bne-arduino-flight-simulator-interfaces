#!/usr/bin/env python3
import tkinter as tk
from PIL import Image
from PIL import ImageTk
 
def update_image():
        global tkimg1
        global angle
        
        #self.canvas.delete(all)
        
        tkimg1 = ImageTk.PhotoImage(image.rotate(angle))

##        image = Image.open("HSI copy 2.png")
##        image2 = Image.open("HSI Outline.png")
##        tkimage2 = ImageTk.PhotoImage(image2)
        
        canvas_obj = root.canvas.delete(all)
        canvas_obj = root.canvas.create_image(500, 526, image=tkimg1)
        canvas_obj = root.canvas.create_image(500,500, image=tkimage2)
        #label.config( image = tkimg1)
        root.canvas.after(100, update_image)
        angle += 9
        angle %= 360
        #self.canvas.delete(canvas_obj)
        #print ("Updated")
 
 
angle = 0 
root = tk.Tk()
root.canvas = tk.Canvas(root, width=1000, height=1000)
root.canvas.pack()
im = Image.open("HSI copy 2.png")
tkimg1 = ImageTk.PhotoImage(im)

image = Image.open("HSI copy 2.png")
image2 = Image.open("HSI Outline.png")
tkimage2 = ImageTk.PhotoImage(image2)
print ("Loaded")

root.after(100, update_image)
root.mainloop()


