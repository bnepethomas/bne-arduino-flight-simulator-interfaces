#!/usr/bin/env python
      
# NEMA convertor - takes Flight Sim coordinates from a UDP stream and outputs via serial port NEMA Sentences      
 
# The Lowarance 2000 supports the following NEMA Sentences
#     GLL, RMC, RMB, GGA, GSA, GSV, APB
# Currently Exporting- RMC, GGA and GSA

import binascii
import time
import codecs
import serial
import string


localDebugging = False

ser = serial.Serial(

    port='/dev/ttyS0',
    baudrate = 115200,
    parity=serial.PARITY_NONE,
    stopbits=serial.STOPBITS_ONE,
    bytesize=serial.EIGHTBITS,
    timeout=1
)
counter=0

def CalcChecksum(strToCalc):
    if localDebugging:
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
    if localDebugging:
        print ('Checksum is ' +  mychecksum)
    
    return(mychecksum)
    #ser.write(str.encode('$GPRMC,160533.00,A,1002.3552,N,01045.51552,E,400,0,010413,0,E*73\r\n'))

def Send_GPRMC():

    outStartOfString = 'GPRMC'
    outStatus = 'A'
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
    if localDebugging:
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
    outcompletestring = outcompletestring + '699' + ".0,M,,,,"
    outcompletestring = outcompletestring 


    outputstring = '$' + outcompletestring + '*' + CalcChecksum(outcompletestring) + '\r\n'
    if localDebugging:
        print(outputstring)
    ser.write(str.encode(outputstring))

def Send_GPGSA():
    #  Out Satellite status
    outputstring = "GPGSA,A,3,01,02,03,,,,,,,,,,3.0,3.0,3.0,*"
    outputstring = '$' + outputstring + '*' +  CalcChecksum(outputstring) + '\r\n'
    if localDebugging:
        print (outputstring)
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
outSpeed = '299'
outTrackMadeGood = '0'
outMagVar = '0'
outMagEorW = 'E'


## Sucessfully calculated check sum by stripping leading $ and all characters post *
# To output will need to prepend $, append * - the ASCII chars of the hex checksum and then CRLF

Send_GPRMC()
Send_GPGGA()
Send_GPGSA()

latDegrees = 27
latMinutes = 23
latSeconds = 4120
i = 0
j = 0

xoutputval = 2723.4120

longDegrees = 153
longMinutes = 7
longSeconds = 72900


while ( j < 4600):
    
    youtputstr = str(longDegrees).zfill(2) + str(longMinutes).zfill(2) + '.' + str(latSeconds)
    
    time.sleep(0.01)
    longMinutes = longMinutes + 1
    if (longMinutes > 59):
        longMinutes = 0
        longDegrees = longDegrees + 1
        if (longDegrees > 179):
            longDegrees = 0
            
    Send_GPRMC()
    Send_GPGGA()
    Send_GPGSA()
        
    time.sleep(0.01)

    j = j + 1
    i = 0
    while (i < 4600):
        xoutputval = xoutputval + 1
        xoutputstr = str(latDegrees).zfill(2) + str(latMinutes).zfill(2) + '.' + str(latSeconds)
        
        
        Send_GPRMC()
        Send_GPGGA()
        Send_GPGSA()
        
        time.sleep(0.01)
        latMinutes = latMinutes + 1
        if (latMinutes > 59):
            latMinutes = 0
            latDegrees = latDegrees + 1
            if (latDegrees > 74):
                latDegrees = 0
                
        i = i + 1
        print('Count is ' + str(i) + ' ' + xoutputstr)

print('Finished')

