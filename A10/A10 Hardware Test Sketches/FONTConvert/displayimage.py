
from tkinter import Tk, Canvas
from PIL import Image, ImageTk
import time

def display_image(image_path):
    root = Tk()
    root.title("BMP Image Viewer")
    
    # Load the BMP image
    image = Image.open(image_path)
    photo = ImageTk.PhotoImage(image)
    
    # Create a canvas to display the image
    canvas = Canvas(root, width=image.width, height=image.height)
    canvas.create_image(0, 0, anchor="nw", image=photo)
    canvas.pack()
    
    root.mainloop()

# Display the first BMP image
display_image("hashone.bmp")
time.sleep(2)
# Display the second BMP image
display_image("output90.bmp")

