

from PIL import Image, ImageTk
from tkinter import Tk, Label
import time

root = Tk()

def RBGAImage(path):
    return Image.open(path).convert("RGBA")

class SimpleApp(object):
    def __init__(self, master, **kwargs):
        self.master = master

        img = ImageTk.PhotoImage(Image.open("HSI copy 2.png"))
        self.panel = Label(root, image = img)
        self.panel.pack(side = "bottom", fill = "both", expand = "yes")
        


##
##        
##
##        
##        image2 = Image.open("HSI copy 2.png")
##        eyes = image2.convert("RGBA")
##        
##
##        # When working with images largely pointers so need to explicity copy image
##        # as opposed to just assignment
##        
##        newface = face.copy()
##        newface.paste(eyes, (500, 500), eyes)
##        facepic = ImageTk.PhotoImage(image2)
##        #self.label1 = Label(image=facepic)
##        #self.label1 = Label(image=eyes)
##        #self.label1.grid(row = 0, column = 0)
##        
        master.after(10000, self.update_image)

    def update_image(self):

        global angle  
        global face
        global newface

        #img = ImageTk.PhotoImage(Image.open("HSI copy 2.png"))
        #self.panel = Label(root, image = img)
        
        #self.panel.pack(side = "bottom", fill = "both", expand = "yes")

##        angle += 9
##        angle %= 360      
##        image = Image.open("HSI copy 2.png")
##        image = image.rotate(angle)
##        eyes = image.convert("RGBA")
##        
##
##        newface = face.copy()
##        newface.paste(eyes, (175, 167), eyes)
##        facepic = ImageTk.PhotoImage(newface)
##        print('angle :' + str(angle))
##        print('width :' + str(eyes.width))
##        print('height :' + str(eyes.height))
##        print(eyes)
##        print(face)
##        print(newface)
##        print(facepic)
        
##        if angle > 130:
##            newface.save('image_400.png')
##            face.save('image_401.png')
##            eyes.save('image_402.png')
##            image.save('image_403.png')
##            time.sleep(1)



##        self.label1 = Label(image=facepic)
##        
##        #self.after_idle(self.update_image) 

##        self.label1.after(100, self.update_image)
        self.panel.after(100, self.update_image)

             

        print('Update Complete Angle is:' + str(angle))


angle = 66    
image = Image.open("HSI copy 2.png")
image = image.rotate(angle)



face = RBGAImage("HSI Outline.png")
eyes = image.convert("RGBA")
eyes = RBGAImage("HSI copy 2.png")

eyes.rotate(angle)

newface = face.copy()

newface.paste(eyes, (175, 167), eyes)

#facepic = ImageTk.PhotoImage(newface)
facepic = ImageTk.PhotoImage(face)


######


######



app = SimpleApp(root)


root.mainloop()
