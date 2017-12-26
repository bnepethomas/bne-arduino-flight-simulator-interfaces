
# Font used in Arduino build is FreeMonoBold  which is basically Courier


import time

import Adafruit_GPIO.SPI as SPI
import Adafruit_SSD1306

from PIL import Image
from PIL import ImageDraw
from PIL import ImageFont

colour_black = 0
colour_white = 255

ten_Column_Pos = 45
hundred_Column_Pos = 24
thousand_Column_Pos = 5
hatch_width = 12
hatch_height = 20

def DrawHatch():
   

    draw.rectangle((thousand_Column_Pos, 20, thousand_Column_Pos + hatch_width, 20 + hatch_height), outline=colour_white, fill=colour_white)

    for i in range(5,9): 
        draw.line((thousand_Column_Pos, 10 + i, thousand_Column_Pos +hatch_width, i  + 10 + 10), fill=colour_black)

    for i in range(5,9):
        draw.line((thousand_Column_Pos, 17 + i, thousand_Column_Pos + hatch_width, i  + 17 + 10), fill=colour_black)
        
    for i in range(5,9):
        draw.line((thousand_Column_Pos, 24 + i, thousand_Column_Pos + hatch_width, i  + 24 + 10), fill=colour_black)       

    for i in range(5,9):
        draw.line((thousand_Column_Pos, 31 + i, thousand_Column_Pos + hatch_width, i  + 31 + 10), fill=colour_black)








# Raspberry Pi pin configuration:
RST = 24
# Note the following are only used with SPI:
DC = 23
SPI_PORT = 0
SPI_DEVICE = 0


# 128x64 display with hardware SPI:
disp = Adafruit_SSD1306.SSD1306_128_64(rst=RST, dc=DC, spi=SPI.SpiDev(SPI_PORT, SPI_DEVICE, max_speed_hz=8000000))



# Initialize library.
disp.begin()

# Clear display.
disp.clear()
disp.display()

# Create blank image for drawing.
# Make sure to create image with mode '1' for 1-bit color.
width = disp.width
height = disp.height
image = Image.new('1', (width, height))

# Get drawing object to draw on image.
draw = ImageDraw.Draw(image)

# Draw a black filled box to clear the image.
draw.rectangle((0,0,width,height), outline=0, fill=0)

# Draw some shapes.
# First define some constants to allow easy resizing of shapes.
padding = 4
shape_width = 20
top = padding
bottom = height-padding
# Move left to right keeping track of the current x position for drawing shapes.
x = padding
# Draw a rectangle.

DrawHatch()
#draw.rectangle((x, top, x+shape_width, bottom), outline=255, fill=255)
x += shape_width+padding
# Draw an X.
draw.line((x, bottom, x+shape_width, top), fill=255)
draw.line((x, top, x+shape_width, bottom), fill=255)
x += shape_width+padding

# Load default font.
font = ImageFont.load_default()

# Alternatively load a TTF font.  Make sure the .ttf font file is in the same directory as the python script!
# Some other nice fonts to try: http://www.dafont.com/bitmap.php
font = ImageFont.truetype('monofonto.ttf', 24)

# Write two lines of text.
draw.text((x, top-6),'01234',  font=font, fill=255)
draw.text((x, top+14), '56789', font=font, fill=255)
draw.text((x, top+34), '01230', font=font, fill=255)
# Display image.
disp.image(image)
disp.display()
