#!/usr/bin/python
#the oled controller supports the HD44780 command set
import smbus
import time
bus = smbus.SMBus(1)
address = 0x3C

bus.write_byte_data(address,0x80,0x2A)	# // **** Set "RE"=1<--->00101010B
bus.write_byte_data(address,0x80,0x71) 	# Function Selection A  [71h] (IS = X, RE = 1, SD=0), 2bajty
bus.write_byte_data(address,0x80,0x5C)  # 0x5C set Vdd
bus.write_byte_data(address,0x80,0x28) 

bus.write_byte_data(address,0x80,0x08) 	#// **** Set Sleep Mode On
bus.write_byte_data(address,0x80,0x2A) 	#// **** Set "RE"=1	00101010B
bus.write_byte_data(address,0x80,0x79) 	#// **** Set "SD"=1	01111001B

bus.write_byte_data(address,0x80,0xD5)  # Set Display Clock Divide Ratio/ Oscillator Frequency (D5h
bus.write_byte_data(address,0x80,0x70) 
bus.write_byte_data(address,0x80,0x78) 	#// **** Set "SD"=0

bus.write_byte_data(address,0x80,0x08) 	#// **** Set 5-dot, 3 or 4 line(0x09), 1 or 2 line(0x08)

bus.write_byte_data(address,0x80,0x06) 	#// **** Set Com31-->Com0  Seg0-->Seg99

#**** Set OLED Characterization
bus.write_byte_data(address,0x80,0x2A)   	#// **** Set "RE"=1 
bus.write_byte_data(address,0x80,0x79)   	#// **** Set "SD"=1

#// **** CGROM/CGRAM Management *** //
bus.write_byte_data(address,0x80,0x72)   	#// **** Set ROM
bus.write_byte_data(address,0x80,0x00)   	#// **** Set ROM A and 8 CGRAM


bus.write_byte_data(address,0x80,0xDA)  	#// **** Set Seg Pins HW Config
bus.write_byte_data(address,0x80,0x10)    

bus.write_byte_data(address,0x80,0x81)   	#// **** Set Contrast
bus.write_byte_data(address,0x80,0x50)

bus.write_byte_data(address,0x80,0xDB)   	#// **** Set VCOM deselect level
bus.write_byte_data(address,0x80,0x30)   	#// **** VCC x 0.83

bus.write_byte_data(address,0x80,0xDC)   	#// **** Set gpio - turn EN for 15V generator on.
bus.write_byte_data(address,0x80,0x03) 

bus.write_byte_data(address,0x80,0x78)   	#// **** Exiting Set OLED Characterization
bus.write_byte_data(address,0x80,0x28)  
bus.write_byte_data(address,0x80,0x2A)  
# sendCommand 0x05  	#// **** Set Entry Mode
bus.write_byte_data(address,0x80,6)  	#// **** Set Entry Mode
#bus.write_byte_data(address,0x80,0x06)  	#// **** Set Entry Mode
# entry mode chyba tu nie dziala?
bus.write_byte_data(address,0x80,0x08)   
bus.write_byte_data(address,0x80,0x28)  	#// **** Set "IS"=0 , "RE" =0 //28
bus.write_byte_data(address,0x80,0x01)  
bus.write_byte_data(address,0x80,0x80)  	#// **** Set DDRAM Address to 0x80 (line 1 start)

time.sleep (0.1)
bus.write_byte_data(address,0x80,0x0C)   	#// **** Turn on Display
bus.write_i2c_block_data(address,0x40,[32,32,ord('b'),ord('o'),ord('o'),ord('t'),ord('.'),ord('.')])
bus.write_i2c_block_data(address,0x40,[32,32,ord('b'),ord('o'),ord('o'),ord('t'),ord('.'),ord('.')])
time.sleep (1)

LCD_CLEARDISPLAY = 0x01
LCD_RETURNHOME = 0x02

bus.write_byte_data(address,0x80,LCD_CLEARDISPLAY)
bus.write_byte_data(address,0x80,0x0C)
bus.write_byte_data(address,0x80,LCD_RETURNHOME)
bus.write_i2c_block_data(address,0x40,[32,32,ord('h'),ord('h'),ord('i'),ord('i'),ord('.'),ord('.')])
time.sleep (1)
bus.write_byte_data(address,0x80,LCD_RETURNHOME)
for y in range(1, 3):
    for x in range(48, 58):
        bus.write_i2c_block_data(address,0x40,[x])
        time.sleep(0.02)

bus.write_byte_data(address,0x80,LCD_CLEARDISPLAY)
bus.write_byte_data(address,0x80,0xC0)
for y in range(1, 3):
    for x in range(48, 58):
        bus.write_i2c_block_data(address,0x40,[x])
        time.sleep(0.02)
