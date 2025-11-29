import tkinter as tk
from PIL import Image,ImageTk
import os


class ImageClassifyer(tk.Frame):


    def __init__(self, parent, *args, **kwargs):

        tk.Frame.__init__(self, parent, *args, **kwargs)

        self.root = parent
        self.root.wm_title("Classify Image")   
        src = "."

        self.list_images = []
        for d in os.listdir(src):
            self.list_images.append(d)

        self.frame1 = tk.Frame(self.root, width=500, height=400, bd=2)
        self.frame1.grid(row=1, column=0)
        self.frame2 = tk.Frame(self.root, width=500, height=400, bd=1)
        self.frame2.grid(row=1, column=1)

        self.cv1 = tk.Canvas(self.frame1, height=390, width=490, background="white", bd=1, relief=tk.RAISED)
        self.cv1.grid(row=1,column=0)
        self.cv2 = tk.Canvas(self.frame2, height=390, width=490, bd=2, relief=tk.SUNKEN)
        self.cv2.grid(row=1,column=0)

        claButton = tk.Button(self.root, text='Classify', height=2, width=10, command=self.classify_obj)
        claButton.grid(row=0, column=1, padx=2, pady=2)
        broButton = tk.Button(self.root, text='Next', height=2, width=8, command = self.next_image)
        broButton.grid(row=0, column=0, padx=2, pady=2)

        self.counter = 0
        self.max_count = len(self.list_images)-1
        self.next_image()

    def classify_obj(self):
        print("In Development")

    def next_image(self):

        if self.counter > self.max_count:
            print("No more images")
        else:
            im = Image.open("{}{}".format(".", self.list_images[self.counter]))
            if (490-im.size[0])<(390-im.size[1]):
                width = 490
                height = width*im.size[1]/im.size[0]
                self.next_step(height, width)
            else:
                height = 390
                width = height*im.size[0]/im.size[1]
                self.next_step(height, width)

    def next_step(self, height, width):
        self.im = Image.open("{}{}".format(".", self.list_images[self.counter]))
        self.im.thumbnail((width, height), Image.ANTIALIAS)
        self.root.photo = ImageTk.PhotoImage(self.im)
        self.photo = ImageTk.PhotoImage(self.im)

        if self.counter == 0:
            self.cv1.create_image(0, 0, anchor = 'nw', image = self.photo)

        else:
            self.im.thumbnail((width, height), Image.ANTIALIAS)
            self.cv1.delete("all")
            self.cv1.create_image(0, 0, anchor = 'nw', image = self.photo)
        self.counter += 1


if __name__ == "__main__":
    root = tk.Tk() 
    MyApp = ImageClassifyer(root)
    tk.mainloop()
