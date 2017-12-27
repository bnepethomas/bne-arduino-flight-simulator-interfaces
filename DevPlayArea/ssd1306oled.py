
# Font used in Arduino build is FreeMonoBold  which is basically Courier
# This code is based of the SSD1306 code developed by the adafruit team

import socket

UDP_IP_ADDRESS = "192.168.3.101"
UDP_PORT_NO = 15151
serverSock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
serverSock.settimeout(0.0001)
serverSock.bind((UDP_IP_ADDRESS, UDP_PORT_NO))


import time

import Adafruit_GPIO.SPI as SPI
import Adafruit_SSD1306

from PIL import Image
from PIL import ImageDraw
from PIL import ImageFont

colour_black = 0
colour_white = 255
top = 0

column_Spacing = 17

ten_thousand_Column_Pos = 5
thousand_Column_Pos = ten_thousand_Column_Pos + column_Spacing
hundred_Column_Pos = thousand_Column_Pos + column_Spacing
ten_Column_Pos = hundred_Column_Pos + column_Spacing
one_Column_Pos = ten_Column_Pos + column_Spacing

top_hidden_row = -27
top_row = -6
middle_row = 15
bottom_row = 36
bottom_hidden_row = 57

hatch_width = 12
hatch_height = 20
character_width = 12

vertical_offset = 21
cursor_Multiplier = 2.1





def DrawHatch(alt_TenThousandsValue,alt_ThousandsValue,alt_HundredsValue, alt_TensValue, alt_OnesValue,vertical_character_offset):
    # Only called if below 10,000 ft
  
    # Check to see if ready to roll the hatch between 90900 and 10,000 
    if ((alt_HundredsValue==9) & (alt_TenThousandsValue==0) & (alt_ThousandsValue==9)):
        hatch_Top = 10 - hatch_height + vertical_character_offset
    else:
        hatch_Top = 10
   
    # Draw White box
    draw.rectangle((ten_thousand_Column_Pos, hatch_Top + 10, ten_thousand_Column_Pos + hatch_width, hatch_Top + 10 + hatch_height), outline=colour_white, fill=colour_white)

    # Add diagonal black lines to create hatch
    for i in range(5,9): 
        draw.line((ten_thousand_Column_Pos, hatch_Top + i, ten_thousand_Column_Pos +hatch_width, i  + hatch_Top + 10), fill=colour_black)
    for i in range(5,9):
        draw.line((ten_thousand_Column_Pos, hatch_Top + 7 + i, ten_thousand_Column_Pos + hatch_width, i + hatch_Top + 7 + 10), fill=colour_black)       
    for i in range(5,9):
        draw.line((ten_thousand_Column_Pos, hatch_Top + 14 + i, ten_thousand_Column_Pos + hatch_width, i + hatch_Top + 14 + 10), fill=colour_black)       
    for i in range(5,9):
        draw.line((ten_thousand_Column_Pos, hatch_Top + 21 + i, ten_thousand_Column_Pos + hatch_width, i + hatch_Top  + 21 + 10), fill=colour_black)


    #As hatch drawing is a little rough - need to clean up for rolling text
    if ((alt_HundredsValue==9)& (alt_TenThousandsValue==0) & (alt_ThousandsValue==9)):
        draw.rectangle((ten_thousand_Column_Pos, bottom_row + hatch_Top, ten_thousand_Column_Pos + column_Spacing, bottom_row + hatch_Top + hatch_height ), outline=colour_black, fill=colour_black)
        draw.text((ten_thousand_Column_Pos, middle_row + vertical_character_offset),str((alt_TenThousandsValue+1) % 10),  font=font, fill=colour_white)
 

    draw.rectangle((ten_thousand_Column_Pos, top_row, ten_thousand_Column_Pos + column_Spacing, top_row + hatch_height ), outline=colour_black, fill=colour_black)
    draw.rectangle((ten_thousand_Column_Pos, bottom_row + 10, ten_thousand_Column_Pos + column_Spacing, bottom_row + hatch_height + 10 ), outline=colour_black, fill=colour_black)


    
    # end DrawHatch


def DrawTenThousands(alt_TenThousandsValue,alt_ThousandsValue,alt_HundredsValue, alt_TensValue, alt_OnesValue,vertical_character_offset):

    draw.rectangle((ten_thousand_Column_Pos, 0, ten_thousand_Column_Pos + column_Spacing, 64), outline=colour_black, fill=colour_black)
    

    if (alt_TenThousandsValue == 0):
        DrawHatch(alt_TenThousandsValue,alt_ThousandsValue,alt_HundredsValue, alt_TensValue, alt_OnesValue,vertical_character_offset)
    else:


        if ((alt_HundredsValue==9) & (alt_ThousandsValue==9)):
            draw.text((ten_thousand_Column_Pos, top_row + vertical_character_offset),str((alt_TenThousandsValue) % 10),  font=font, fill=colour_white)
            draw.text((ten_thousand_Column_Pos, middle_row + vertical_character_offset),str((alt_TenThousandsValue+1) % 10),  font=font, fill=colour_white)
        else:
            draw.text((ten_thousand_Column_Pos, middle_row), str(alt_TenThousandsValue), font=font, fill=colour_white)

    # end DrawTenThousands



def DrawThousands(alt_TenThousandsValue,alt_ThousandsValue,alt_HundredsValue, alt_TensValue, alt_OnesValue,vertical_character_offset):



    draw.rectangle((thousand_Column_Pos, 0, thousand_Column_Pos + column_Spacing, 64), outline=colour_black, fill=colour_black)


    if ((alt_HundredsValue==9) ):
        draw.text((thousand_Column_Pos, top_row + vertical_character_offset),str((alt_ThousandsValue) % 10),  font=font, fill=colour_white)
        draw.text((thousand_Column_Pos, middle_row + vertical_character_offset),str((alt_ThousandsValue+1) % 10),  font=font, fill=colour_white)
        draw.rectangle((thousand_Column_Pos, top_row, thousand_Column_Pos + column_Spacing, top_row + hatch_height ), outline=colour_black, fill=colour_black)
        draw.rectangle((thousand_Column_Pos, bottom_row + 10, thousand_Column_Pos + column_Spacing, bottom_row + hatch_height + 10 ), outline=colour_black, fill=colour_black)

    else:
        draw.text((thousand_Column_Pos, middle_row), str(alt_ThousandsValue), font=font, fill=colour_white)


    #end Draw Thousands


def DrawHundreds(alt_TenThousandsValue,alt_ThousandsValue,alt_HundredsValue, alt_TensValue, alt_OnesValue,vertical_character_offset):

 
    # Large Rectangle to cover all three rows
    draw.rectangle((hundred_Column_Pos, 0, hundred_Column_Pos + column_Spacing, 64), outline=colour_black, fill=colour_black)

    draw.text((hundred_Column_Pos, top_hidden_row + vertical_character_offset ), str((alt_HundredsValue+9) % 10), font=font, fill=colour_white)
    draw.text((hundred_Column_Pos, top_row + vertical_character_offset),str((alt_HundredsValue) % 10),  font=font, fill=colour_white)
    draw.text((hundred_Column_Pos, middle_row + vertical_character_offset), str((alt_HundredsValue+1) % 10), font=font, fill=colour_white)
    draw.text((hundred_Column_Pos, bottom_row + vertical_character_offset ), str((alt_HundredsValue+2) % 10), font=font, fill=colour_white)
    draw.text((hundred_Column_Pos, bottom_hidden_row + vertical_character_offset ), str((alt_HundredsValue+3) % 10), font=font, fill=colour_white)

    # end DrawHundreds


def DrawAltitude(altitude):

    alt_TenThousandsValue = (altitude/10000) % 10
    alt_ThousandsValue = (altitude/1000) % 10
    alt_HundredsValue = (altitude/100) % 10
    alt_TensValue = (altitude/10) % 10
    alt_OnesValue = altitude % 10


    #vertical_character_offset is used to determine the offset for rolling characters
    vertical_character_offset = ((alt_OnesValue * cursor_Multiplier * -0.1) + (alt_TensValue * cursor_Multiplier * -1)) + vertical_offset
    
    DrawTenThousands(alt_TenThousandsValue,alt_ThousandsValue,alt_HundredsValue, alt_TensValue, alt_OnesValue, vertical_character_offset )
    DrawThousands(alt_TenThousandsValue,alt_ThousandsValue,alt_HundredsValue, alt_TensValue, alt_OnesValue, vertical_character_offset)
    DrawHundreds(alt_TenThousandsValue,alt_ThousandsValue,alt_HundredsValue, alt_TensValue, alt_OnesValue, vertical_character_offset)


    disp.image(image)
    disp.display()
    
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

# Start with a zero display for a second
DrawAltitude(0)
time.sleep(1)

while True:
    
    try:
        data, addr = serverSock.recvfrom(1024)
        print ("Message: ", data)
        s = data.decode("utf-8") 
        if (s.isdigit):
            w = int(s)
            DrawAltitude(w)    
                                          
    except socket.timeout:
        a=0


##for k in range(0,30050,10):
##    DrawAltitude(k)
##
##for k in range(10050,0,-1):
##    DrawAltitude(k)
##
##
##DrawAltitude(0)
##
##print("Finished")


