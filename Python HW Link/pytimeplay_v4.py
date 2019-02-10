

from PIL import Image, ImageTk
from tkinter import Tk, Label
import time

root = Tk()

def RBGAImage(path):
    return Image.open(path).convert("RGBA")

class SimpleApp(object):
    def __init__(self, master, **kwargs):
        self.master = master

        
        image = Image.open("HSI copy 2.png")
        eyes = image.convert("RGBA")
        

        newface = face.copy()
        newface.paste(eyes, (175, 167), eyes)
        facepic = ImageTk.PhotoImage(newface)
        self.label1 = Label(image=facepic)
        self.label1.grid(row = 0, column = 0)
        
        master.after(3000, self.update_image)

    def update_image(self):

        global angle  
        global face
        global newface



        angle += 9
        angle %= 360      
        image = Image.open("HSI copy 2.png")
        image = image.rotate(angle)
        eyes = image.convert("RGBA")
        

        newface = face.copy()
        newface.paste(eyes, (175, 167), eyes)
        facepic = ImageTk.PhotoImage(newface)
        print('width :' + str(eyes.width))
        print('height :' + str(eyes.height))
        print(eyes)
        print(face)
        print(newface)
        print(facepic)
        
##        if angle > 130:
##            newface.save('image_400.png')
##            face.save('image_401.png')
##            eyes.save('image_402.png')
##            image.save('image_403.png')
##            time.sleep(5)



        self.label1 = Label(image=facepic)
        
        #self.after_idle(self.update_image) 

        self.label1.after(100, self.update_image)

             

        print('Update Complete Angle is:' + str(angle))


angle = 66    
image = Image.open("HSI copy 2.png")
image = image.rotate(angle)



face = RBGAImage("HSI Outline.png")
eyes = image.convert("RGBA")
#eyes = RBGAImage("HSI copy 2.png")

eyes.rotate(angle)

newface = face.copy()

newface.paste(eyes, (175, 167), eyes)

facepic = ImageTk.PhotoImage(newface)



app = SimpleApp(root)


root.mainloop()
