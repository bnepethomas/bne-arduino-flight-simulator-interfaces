# Currently cleaning up transition with hatches leaving trails


# Altimeter OLED code - used in conjunction with Arduino for driving stepper  
# https://github.com/bnepethomas/bne-arduino-flight-simulator-interfaces/tree/master/Hornet%20Altimeter

# This code is based of the SSD1306 code developed by the adafruit team
#   Suggest start with same code form Adafruit team to ensure hardware
#   is correctly configured. The adafruit library will need to be grabbed from Github
#   https://learn.adafruit.com/ssd1306-oled-displays-with-raspberry-pi-and-beaglebone-black/overview
#   Then use this code.





# Font used in Arduino build is FreeMonoBold  which is basically Courier
#   As the same font wasn't easily found, used 'monofonto.ttf' from
#       http://www.dafont.com/bitmap.php
#   This font is fixed width, and digits are centered which make the dial look more natural
#   The file needs to be copied to the directory where the python will execute from

# This code assumes used on the SPI interface, which must be enabled in the pi
#   To validate the SPI interface is active 'ls /dev/*spi*'. This should
#   return 'spidev0.0  spidev0.1'.
# On Pi3B+ despite SI enabled in UI, unable to see any SPI devices. Unsure why - enabled through raspi-config
# Reloaded and ultimately it started 

# pip install Adafruit_GPIO
# pip3 install Adafruit_SSD1306



# Currently only the Altitude is used, received as a string in a UDP packet
#   In future should


# The current build environment uses the 192.168.3.0/24 network with
#   192.168.3.101 - Wired Port of pi, address statically assigned - see below
#   192.168.3.107 - Wired interface of Arduino



# Statically assigning an address to the Rapsberry pi wired port (eth0). Assuming
#   wireless is used for general access a
#   edit (nano is easiest) /etc/dhcpcd.conf
#   uncomment and update the following
#       interface eth0
#       static ip_address=192.168.3.101/24

# Once complete the code will need to auto-run - so update rc.local - details to follow

# Recommend using git to keep things synchronised across different platforms. On Mac the desktop
#   client is simple to use.  For the Raspberry pi, the cli commands are available, and require
#   a little more knowledge, three useful commands to know
#       git pull            - grab/sync the repository from github
#       git commit .        - commit local changes
#       git push            - synch local changes to github. Need a git account (which is free for public projects)

# When working with the pi - I usually use VNC Viewer (after updating the 1280 * 720) with IDLE, and
#   ssh to the pi for cli work. (default username for the pi is... 'pi')


import socket

# Do not need short timeout for UDP packet as display is refreshed in one hit
#   (unlike stepper motors which need to move to new position
UDP_IP_ADDRESS = "192.168.3.101"

# Using local addresses while testing
UDP_IP_ADDRESS = "192.168.1.107"
UDP_PORT_NO = 15151
serverSock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
serverSock.settimeout(1)
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

column_Spacing = 23

ten_thousand_Column_Pos = 0
thousand_Column_Pos = ten_thousand_Column_Pos + column_Spacing
hundred_Column_Pos = thousand_Column_Pos + column_Spacing
ten_Column_Pos = hundred_Column_Pos + column_Spacing
one_Column_Pos = ten_Column_Pos + column_Spacing


top_hidden_row = -60

top_row = -22

middle_row = 15
bottom_row = 76
bottom_hidden_row = 80

hatch_width = 19
hatch_height = 30
character_width = 12


vertical_offset = 37
cursor_Multiplier = 3.6

Altitude_List_Entry = 0
Kollsman_List_Entry = 1


def DrawHatch(alt_TenThousandsValue,alt_ThousandsValue,alt_HundredsValue, alt_TensValue, alt_OnesValue,vertical_character_offset):
    # Only called if below 10,000 ft
  
    # Check to see if ready to roll the hatch between 90900 and 10,000 
    if ((alt_HundredsValue==9) & (alt_TenThousandsValue==0) & (alt_ThousandsValue==9)):
        hatch_Top = 2 - hatch_height + vertical_character_offset
    else:
        hatch_Top = 16
   
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
    for i in range(5,9):
        draw.line((ten_thousand_Column_Pos, hatch_Top + 28 + i, ten_thousand_Column_Pos + hatch_width, i + hatch_Top  + 28 + 10), fill=colour_black)


    #As hatch drawing is a little rough - need to clean up for rolling text
    if ((alt_HundredsValue==9)& (alt_TenThousandsValue==0) & (alt_ThousandsValue==9)):
        draw.rectangle((ten_thousand_Column_Pos, bottom_row + hatch_Top, ten_thousand_Column_Pos + column_Spacing, bottom_row + hatch_Top + hatch_height ), outline=colour_black, fill=colour_black)
        draw.text((ten_thousand_Column_Pos, middle_row + vertical_character_offset),str((alt_TenThousandsValue+1) % 10),  font=font, fill=colour_white)
 
    
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


    draw.rectangle((ten_thousand_Column_Pos, top_row, ten_thousand_Column_Pos + column_Spacing, top_row + hatch_height + 8 ), outline=colour_black, fill=colour_black)
    draw.rectangle((ten_thousand_Column_Pos, bottom_row + 20, ten_thousand_Column_Pos + column_Spacing, bottom_row + hatch_height + 10 ), outline=colour_black, fill=colour_black)

    # end DrawTenThousands



def DrawThousands(alt_TenThousandsValue,alt_ThousandsValue,alt_HundredsValue, alt_TensValue, alt_OnesValue,vertical_character_offset):



    draw.rectangle((thousand_Column_Pos, 0, thousand_Column_Pos + column_Spacing, 64), outline=colour_black, fill=colour_black)



    if ((alt_HundredsValue==9) ):
        draw.text((thousand_Column_Pos, top_row + vertical_character_offset),str(int(alt_ThousandsValue % 10)),  font=font, fill=colour_white)
        draw.text((thousand_Column_Pos, middle_row + vertical_character_offset),str(int(alt_ThousandsValue+1) % 10),  font=font, fill=colour_white)
        draw.rectangle((thousand_Column_Pos, top_row, thousand_Column_Pos + column_Spacing, top_row + hatch_height ), outline=colour_black, fill=colour_black)
        draw.rectangle((thousand_Column_Pos, bottom_row + 10, thousand_Column_Pos + column_Spacing, bottom_row + hatch_height + 10 ), outline=colour_black, fill=colour_black)

    else:
        draw.text((thousand_Column_Pos, middle_row), str(alt_ThousandsValue), font=font, fill=colour_white)


    draw.rectangle((thousand_Column_Pos, top_row, thousand_Column_Pos + column_Spacing, top_row + hatch_height + 8 ), outline=colour_black, fill=colour_black)

    #end Draw Thousands


def DrawHundreds(alt_TenThousandsValue,alt_ThousandsValue,alt_HundredsValue, alt_TensValue, alt_OnesValue,vertical_character_offset):

 
    # Large Rectangle to cover all three rows
    draw.rectangle((hundred_Column_Pos, 0, hundred_Column_Pos + column_Spacing, 64), outline=colour_black, fill=colour_black)

    draw.text((hundred_Column_Pos, top_hidden_row + vertical_character_offset ), str(int(alt_HundredsValue+9) % 10), font=font, fill=colour_white)
    draw.text((hundred_Column_Pos, top_row + vertical_character_offset),str(int(alt_HundredsValue % 10)),  font=font, fill=colour_white)
    draw.text((hundred_Column_Pos, middle_row + vertical_character_offset), str(int(alt_HundredsValue+1) % 10), font=font, fill=colour_white)
    # the following rows aren't needed with large 
    #draw.text((hundred_Column_Pos, bottom_row + vertical_character_offset ), str((alt_HundredsValue+2) % 10), font=font, fill=colour_white)
    #draw.text((hundred_Column_Pos, bottom_hidden_row + vertical_character_offset ), str((alt_HundredsValue+3) % 10), font=font, fill=colour_white)

    # end DrawHundreds


def DrawAltitude(altitude):

    alt_TenThousandsValue = int((altitude/10000) % 10)
    alt_ThousandsValue = int((altitude/1000) % 10)
    alt_HundredsValue = int((altitude/100) % 10)
    alt_TensValue = int((altitude/10) % 10)
    alt_OnesValue = int(altitude % 10)


    #vertical_character_offset is used to determine the offset for rolling characters
    vertical_character_offset = ((alt_OnesValue * cursor_Multiplier * -0.1) + (alt_TensValue * cursor_Multiplier * -1)) + vertical_offset


    #DrawHatch(alt_TenThousandsValue,alt_ThousandsValue,alt_HundredsValue, alt_TensValue, alt_OnesValue, vertical_character_offset )

    if (alt_TenThousandsValue == 0):
        DrawHatch(alt_TenThousandsValue,alt_ThousandsValue,alt_HundredsValue, alt_TensValue, alt_OnesValue, vertical_character_offset )
    else:
        DrawTenThousands(alt_TenThousandsValue,alt_ThousandsValue,alt_HundredsValue, alt_TensValue, alt_OnesValue, vertical_character_offset )

    DrawThousands(alt_TenThousandsValue,alt_ThousandsValue,alt_HundredsValue, alt_TensValue, alt_OnesValue, vertical_character_offset)
    DrawHundreds(alt_TenThousandsValue,alt_ThousandsValue,alt_HundredsValue, alt_TensValue, alt_OnesValue, vertical_character_offset)


    disp.image(image)
    disp.display()
    
    #end DrawAltitide
    

def DrawPressure(Pressure):
    draw.rectangle((70, 0, 128, 20 ), outline=colour_black, fill=colour_black)
    font = ImageFont.truetype('monofonto.ttf', 20)
    draw.text((80, -5),str(Pressure),  font=font, fill=colour_white)

    
print("init")
# Raspberry Pi pin configuration:
RST = 24
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
font = ImageFont.truetype('monofonto.ttf', 45)



# Draw the Ones and Tens which never change
draw.text((ten_Column_Pos, middle_row),'0', font=font, fill=colour_white)
draw.text((one_Column_Pos, middle_row),'0', font=font, fill=colour_white)

# Start with a zero display for a second
#DrawAltitude(0)

time.sleep(1)
print("init done")
while True:


    for k in range(9800, 10100,10):
        
        
        DrawAltitude(k)
        time.sleep(1)


    for k in range(10100,9800,-10):
        DrawAltitude(k)
        time.sleep(1)




    print("Finished")

    
##    try:
##        data, addr = serverSock.recvfrom(1024)
##        s = data.decode("utf-8")
##        receivedValues = s.split(",")
##
####        if (receivedValues[Altitude_List_Entry ].isdigit):
####            w = int(receivedValues[Altitude_List_Entry])
####            font = ImageFont.truetype('monofonto.ttf', 45)
####            DrawAltitude(w)
####
####        if (receivedValues[Kollsman_List_Entry].isdigit):
####            w = int(receivedValues[Kollsman_List_Entry])            
####            DrawPressure(w)
##                                          
##    except socket.timeout:
##        # Don't panic that packet hasn't arrived
##        a=0




