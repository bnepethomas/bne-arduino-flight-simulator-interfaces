
# Font used in Arduino build is FreeMonoBold  which is basically Courier


import time

import Adafruit_GPIO.SPI as SPI
import Adafruit_SSD1306

from PIL import Image
from PIL import ImageDraw
from PIL import ImageFont

colour_black = 0
colour_white = 255


column_Spacing = 17

ten_thousand_Column_Pos = 5
thousand_Column_Pos = ten_thousand_Column_Pos + column_Spacing
hundred_Column_Pos = thousand_Column_Pos + column_Spacing
ten_Column_Pos = hundred_Column_Pos + column_Spacing
one_Column_Pos = ten_Column_Pos + column_Spacing

hatch_width = 12
hatch_height = 20

character_width = 12

cursor_Multiplier = 2.1

top = 0
x = 23
top_hidden_row = -27
top_row = -6
middle_row = 15
bottom_row = 36
bottom_hidden_row = 57

def DrawHatch():
   
    # Draw White box
    draw.rectangle((ten_thousand_Column_Pos, 20, ten_thousand_Column_Pos + hatch_width, 20 + hatch_height), outline=colour_white, fill=colour_white)

    # Add diagonal black lines to create hatch
    for i in range(5,9): 
        draw.line((ten_thousand_Column_Pos, 10 + i, ten_thousand_Column_Pos +hatch_width, i  + 10 + 10), fill=colour_black)

    for i in range(5,9):
        draw.line((ten_thousand_Column_Pos, 17 + i, ten_thousand_Column_Pos + hatch_width, i  + 17 + 10), fill=colour_black)
        
    for i in range(5,9):
        draw.line((ten_thousand_Column_Pos, 24 + i, ten_thousand_Column_Pos + hatch_width, i  + 24 + 10), fill=colour_black)       

    for i in range(5,9):
        draw.line((ten_thousand_Column_Pos, 31 + i, ten_thousand_Column_Pos + hatch_width, i  + 31 + 10), fill=colour_black)

    # end DrawHatch

def DrawTenThousands(altitude):
    
    alt_TenThousandsValue = (altitude/10000) % 10
    if (alt_TenThousandsValue == 0):
        DrawHatch()
    else:
        draw.rectangle((ten_thousand_Column_Pos, 0, ten_thousand_Column_Pos + column_Spacing, 64), outline=colour_black, fill=colour_black)
        draw.text((ten_thousand_Column_Pos, middle_row), str(alt_TenThousandsValue), font=font, fill=colour_white)
    # end DrawTenThousands


def DrawThousands(altitude):


    alt_ThousandsValue = (altitude/1000) % 10
    draw.rectangle((thousand_Column_Pos, 0, thousand_Column_Pos + column_Spacing, 64), outline=colour_black, fill=colour_black)

    
### work area
    alt_HundredsValue = (altitude/100) % 10
    alt_TensValue = (altitude/10) % 10
    alt_OnesValue = altitude % 10   
    vertical_offset = 21
    tens_Corrected = ((alt_OnesValue * cursor_Multiplier * -0.1) + (alt_TensValue * cursor_Multiplier * -1)) + vertical_offset

    if ((alt_HundredsValue==9) ):
        draw.text((thousand_Column_Pos, top_row + tens_Corrected),str((alt_ThousandsValue) % 10),  font=font, fill=colour_white)
        draw.text((thousand_Column_Pos, middle_row + tens_Corrected),str((alt_ThousandsValue+1) % 10),  font=font, fill=colour_white)
        draw.rectangle((thousand_Column_Pos, top_row, thousand_Column_Pos + column_Spacing, top_row + hatch_height ), outline=colour_black, fill=colour_black)
        draw.rectangle((thousand_Column_Pos, bottom_row + 10, thousand_Column_Pos + column_Spacing, bottom_row + hatch_height + 10 ), outline=colour_black, fill=colour_black)

    else:
        draw.text((thousand_Column_Pos, middle_row), str(alt_ThousandsValue), font=font, fill=colour_white)


    #end Draw Thousands


def DrawHundreds(altitude):


    alt_HundredsValue = (altitude/100) % 10
    alt_TensValue = (altitude/10) % 10
    alt_OnesValue = altitude % 10
    
    vertical_offset = 21
    tens_Corrected = ((alt_OnesValue * cursor_Multiplier * -0.1) + (alt_TensValue * cursor_Multiplier * -1)) + vertical_offset

    
    # Large Rectangle to cover all three rows
    draw.rectangle((hundred_Column_Pos, 0, hundred_Column_Pos + column_Spacing, 64), outline=colour_black, fill=colour_black)

    draw.text((hundred_Column_Pos, top_hidden_row + tens_Corrected ), str((alt_HundredsValue+9) % 10), font=font, fill=colour_white)
    draw.text((hundred_Column_Pos, top_row + tens_Corrected),str((alt_HundredsValue) % 10),  font=font, fill=colour_white)
    draw.text((hundred_Column_Pos, middle_row + tens_Corrected), str((alt_HundredsValue+1) % 10), font=font, fill=colour_white)
    draw.text((hundred_Column_Pos, bottom_row + tens_Corrected ), str((alt_HundredsValue+2) % 10), font=font, fill=colour_white)
    draw.text((hundred_Column_Pos, bottom_hidden_row + tens_Corrected ), str((alt_HundredsValue+3) % 10), font=font, fill=colour_white)

    # end DrawHundreds


def DrawAltitude(altitude):
    DrawTenThousands(altitude)
    DrawThousands(altitude)
    DrawHundreds(altitude)

    #end DrawAltitide
    

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


# Load default font.
font = ImageFont.load_default()

# Alternatively load a TTF font.  Make sure the .ttf font file is in the same directory as the python script!
# Some other nice fonts to try: http://www.dafont.com/bitmap.php
font = ImageFont.truetype('monofonto.ttf', 26)



# Draw the Ones and Tens which never change
draw.text((ten_Column_Pos, middle_row),'0', font=font, fill=colour_white)
draw.text((one_Column_Pos, middle_row),'0', font=font, fill=colour_white)

DrawTenThousands(0)
DrawThousands(0)
DrawHundreds(0)

disp.image(image)
disp.display()
time.sleep(1)


for k in range(1900,2110,1):
    DrawAltitude(k)
    disp.image(image)
    disp.display()
##    print(k)
##    time.sleep(0.3)


for k in range(2110,1900,-1):
    DrawAltitude(k)
    disp.image(image)
    disp.display()
##    print(k)



DrawTenThousands(0)
DrawThousands(0)
DrawHundreds(0)
disp.image(image)
disp.display()
print("Finished")


