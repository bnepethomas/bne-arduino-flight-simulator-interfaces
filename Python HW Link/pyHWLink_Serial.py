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
    # NEMA Senstences like Caps - so ensure it is capitalised
    mychecksum = mychecksum.upper()
    print ('Checksum is ' +  mychecksum)
    
    return(mychecksum)
    #ser.write(str.encode('$GPRMC,160533.00,A,1002.3552,N,01045.51552,E,400,0,010413,0,E*73\r\n'))

def Send_GPRMC():

    outStartOfString = 'GPRMC'
    outStatus = 'A'

    outSpeed = '299'
    outTrackMadeGood = '30'
    outMagVar = '0'
    outMagEorW = 'E'
    outEndOfString = '*'

    # Assemble string
    outcompletestring = outStartOfString + "," + outUTC + ","
    outcompletestring = outcompletestring + outStatus + ","
    outcompletestring = outcompletestring + xoutputstr + "," + outNorS + ","
    outcompletestring = outcompletestring + youtputstr + "," + outEorW + ","
    outcompletestring = outcompletestring + outSpeed + ","
    outcompletestring = outcompletestring + outTrackMadeGood + ","
    outcompletestring = outcompletestring + outDate + ","
    outcompletestring = outcompletestring + outMagVar + "," + outMagEorW 

    outputstring = '$' + outcompletestring + '*' + CalcChecksum(outcompletestring) + '\r\n'
    print(outputstring)
    ser.write(str.encode(outputstring))

def Send_GPGGA():
    outStartOfString = "GPGGA"
    outGPSFix = "2";
    outNoofSatellites = "05"
    outPrecision = "0.0"
    outEndOfString = "*"

    outcompletestring = outStartOfString + "," + outUTC + ","
    outcompletestring = outcompletestring + xoutputstr + "," + outNorS + ","
    outcompletestring = outcompletestring + youtputstr + "," + outEorW + ","
    outcompletestring = outcompletestring + outGPSFix + ","
    outcompletestring = outcompletestring + outNoofSatellites + ","
    outcompletestring = outcompletestring + outPrecision + ","
    outcompletestring = outcompletestring + '6999' + ".0,M,,,,"
    outcompletestring = outcompletestring 


    outputstring = '$' + outcompletestring + '*' + CalcChecksum(outcompletestring) + '\r\n'
    print(outputstring)


def Send_GPGAS():
    #  Out Satellite status
    outputstring = "GPGSA,A,3,01,02,03,,,,,,,,,,3.0,3.0,3.0,*"
    outputstring = '$' + outputstring + '*' +  CalcChecksum(outputstring) + '\r\n'
    ser.write(str.encode(outputstring))


# Reference - teststring
if False:
    workstring = "GPRMC,160533.00,A,2723.4120,S,15307.72900,E,299,0,010413,0,E"
    # Checksum should be 63
    outputstring = '$' + workstring + '*' + CalcChecksum(workstring) + '\r\n'
    print(outputstring)
    ser.write(str.encode(outputstring))

outUTC = '160533.00'
outDate = "010413"
xoutputstr = '2723.4120'
outNorS = 'S'
youtputstr = '15307.72900'
outEorW = 'E'


## Sucessfully calculated check sum by stripping leading $ and all characters post *
# To output will need to prepend $, append * - the ASCII chars of the hex checksum and then CRLF

Send_GPRMC()
Send_GPGGA()
Send_GPGAS()




