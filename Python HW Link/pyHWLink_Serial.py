#!/usr/bin/env python
      
import binascii
import time
import codecs
# import serial
# 
# 
# ser = serial.Serial(
#   
#    port='/dev/ttyS0',
#    baudrate = 9600,
#    parity=serial.PARITY_NONE,
#    stopbits=serial.STOPBITS_ONE,
#    bytesize=serial.EIGHTBITS,
#    timeout=1
# )
counter=0


while 0:
   ser.write(str.encode('$GPRMC,160533.00,A,1002.3552,N,01045.51552,E,400,0,010413,0,E*73\r\n'))

   time.sleep(1)
   print("hi")
   counter += 1
   
print("hello world") 
workstring = "GPRMC,160533.00,A,1002.3552,N,01045.51552,E,400,0,010413,0,E"
print(workstring)
packet = workstring.encode() 
checksum = 0
for el in packet:
    print(el)
    checksum ^= el

print (checksum, hex(checksum), chr(checksum))
#Need to convert integer/byte into 2 characters
mychecksum = hex(checksum)
print (mychecksum)
mychecksum = mychecksum + 'hi'
mychecksum = mychecksum[2:4]
print (mychecksum)

## Sucessfully calculated check sum by stripping leading $ and all characters post *
# To output will need to prepend $, append * - the ASCII chars of the hex checksum and then CRLF
outputstring = '$' + workstring + '*' + mychecksum
print(outputstring)

