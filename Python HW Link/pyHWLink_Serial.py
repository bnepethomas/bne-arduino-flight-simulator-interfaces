#!/usr/bin/env python
      
import binascii
import time
import codecs
import serial
import string


ser = serial.Serial(

    port='/dev/ttyS0',
    baudrate = 9600,
    parity=serial.PARITY_NONE,
    stopbits=serial.STOPBITS_ONE,
    bytesize=serial.EIGHTBITS,
    timeout=1
)
counter=0

def CalcChecksum(strToCalc):
    print(strToCalc)
    packet = strToCalc.encode() 
    checksum = 0
    for el in packet:
        checksum ^= el

    #print (checksum, hex(checksum), chr(checksum))
    #Need to convert integer/byte into 2 characters
    mychecksum = hex(checksum)
    # Remove leading '0x'
    mychecksum = mychecksum[2:4]
    mychecksum = mychecksum.upper()
    print ('Checksum is ' +  mychecksum)
    
    return(mychecksum)
    #ser.write(str.encode('$GPRMC,160533.00,A,1002.3552,N,01045.51552,E,400,0,010413,0,E*73\r\n'))


workstring = "GPRMC,160533.00,A,1002.3552,N,01045.51552,E,400,0,010413,0,E"
workstring = "GPRMC,160533.00,A,2732.2500,S,15313.31000,E,299,0,010413,0,E"
#workstring = "GPGGA,053302.00,2732.2500,S,15313.31000,E,1,05,0.0,131.9,M,,,,"

outputstring = '$' + workstring + '*' + CalcChecksum(workstring) + '\r\n'
print(outputstring)
ser.write(str.encode(outputstring))

## Sucessfully calculated check sum by stripping leading $ and all characters post *
# To output will need to prepend $, append * - the ASCII chars of the hex checksum and then CRLF

#### From c#

#  Out Satellite status
outputstring = "GPGSA,A,3,01,02,03,,,,,,,,,,3.0,3.0,3.0,*"
outputstring = '$' + outputstring + '*' +  CalcChecksum(outputstring) + '\r\n'
ser.write(str.encode(outputstring))

outStartOfString = "GPGGA";
outUTC = "085823.00";
outGPSFix = "2";
outNoofSatellites = "05";
outPrecision = "0.0";
outEndOfString = "*";

outcompletestring = outStartOfString + "," + outUTC + ",";
outcompletestring = outcompletestring + '2732.2500' + "," + 'S' + ",";
outcompletestring = outcompletestring + '15313.31000' + "," + 'E' + ",";
outcompletestring = outcompletestring + outGPSFix + ",";
outcompletestring = outcompletestring + outNoofSatellites + ",";
outcompletestring = outcompletestring + outPrecision + ",";
outcompletestring = outcompletestring + '6999' + ".0,M,,,,";
outcompletestring = outcompletestring + outEndOfString;


outputstring = '$' + outcompletestring + '*' + CalcChecksum(outcompletestring) + '\r\n'
print(outputstring)
#outputstring = '$GPGGA,092750.000,5321.6802,N,00630.3372,W,1,8,1.03,61.7,M,55.2,M,,*76' + '\r\n'
#ser.write(str.encode(outputstring))


#  Out Satellite status
#outputstring = "GPGSA,A,3,01,02,03,,,,,,,,,,3.0,3.0,3.0,*"
#outputstring = '$' + outputstring + '*' +  CalcChecksum(outputstring) + '\r\n'
#ser.write(str.encode(outputstring))



