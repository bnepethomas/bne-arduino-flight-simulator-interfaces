
import time

import Adafruit_GPIO.SPI as SPI
import Adafruit_SSD1306

from PIL import Image
from PIL import ImageDraw
from PIL import ImageFont


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
padding = 2
shape_width = 20
top = padding
bottom = height-padding
# Move left to right keeping track of the current x position for drawing shapes.
x = padding
# Draw an ellipse.
draw.ellipse((x, top , x+shape_width, bottom), outline=255, fill=0)
x += shape_width+padding
# Draw a rectangle.
draw.rectangle((x, top, x+shape_width, bottom), outline=255, fill=0)
x += shape_width+padding
# Draw a triangle.
draw.polygon([(x, bottom), (x+shape_width/2, top), (x+shape_width, bottom)], outline=255, fill=255)
x += shape_width+padding
# Draw an X.
draw.line((x, bottom, x+shape_width, top), fill=255)
draw.line((x, top, x+shape_width, bottom), fill=255)
x += shape_width+padding

# Load default font.
font = ImageFont.load_default()

# Alternatively load a TTF font.  Make sure the .ttf font file is in the same directory as the python script!
# Some other nice fonts to try: http://www.dafont.com/bitmap.php
#font = ImageFont.truetype('Minecraftia.ttf', 8)

# Write two lines of text.
draw.text((x, top),    'Hello',  font=font, fill=255)
draw.text((x, top+20), 'World!', font=font, fill=255)

# Display image.
disp.image(image)
disp.display()
