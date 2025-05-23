#!/usr/bin/python

# pip install pythyonping

#pip3 install pyusb
# For Mac
#brew install libusb

# For Windows
# Download and unpack to C:\libusb the libusb-1.0.20 from download link
# https://sourceforge.net/projects/libusb/files/libusb-1.0/libusb-1.0.20/libusb-1.0.20.7z/download
# backend = usb.backend.libusb1.get_backend(find_library=lambda x: "C:\libusb\MS64\dll\libusb-1.0.dll")
# dev = usb.core.find(backend=backend, find_all=True)

#Also for validation GUI UsbTreeView_x64
# https://www.uwe-sieber.de/usbtreeview_e.html#download

import os
import subprocess
import sys 
# For some reason this desn't run after upgrade from Python 3.7 to 3.10 - commented out

##import usb
##import usb.core
##import usb.backend.libusb1
##
##def ScanUSB():
##    backend = usb.backend.libusb1.get_backend(find_library=lambda x: "C:\libusb\MS64\dll\libusb-1.0.dll")
##
##    dev = usb.core.find(backend=backend, find_all=True)
##
##
##    devices = usb.core.find(find_all=True)
##
##    for dev in devices:
##
##        try:
##            print(dev._get_full_descriptor_str())
##
##        except:
##            print("Unable to get descriptor")
##
##        try:
##            xdev = usb.core.find(idVendor=dev.idVendor, idProduct=dev.idProduct)
##            if xdev._manufacturer is None:
##                xdev._manufacturer = usb.util.get_string(xdev, xdev.iManufacturer)
##            if xdev._product is None:
##                xdev._product = usb.util.get_string(xdev, xdev.iProduct)
##            stx = '%6d %6d: '+str(xdev._manufacturer).strip()+' = '+str(xdev._product).strip()
##                   
##            print (str(xdev._manufacturer).strip(),":",str(xdev._product).strip())
##            
##        except:
##            print("Unknown devivce")
##            pass
##        print("Bus:", dev.bus, " Address:", dev.address, " Port:", dev.port_number," Speed:", dev.speed)
##        print()



from pythonping import ping

def pingit(Target_Name, Target_IP):

    show_all_pings = False
    response_list = ping(Target_IP, count=1,timeout=1,verbose=show_all_pings)
    if response_list.rtt_avg_ms <  10:
        print(Target_Name + " (" + Target_IP  + ") : PASS")
    else:
        print("WARNING " + Target_Name + " (" + Target_IP +  ") : FAIL")
        input("Press Enter to continue")

def pingpitdevices():

    pingit("Forward Console Inputs", '172.16.1.101')
    pingit("Left Console Inputs", '172.16.1.100')
    pingit("Right Console Inputs", '172.16.1.103')
    pingit("UDP to Keystroke Converter", '172.16.1.110')

    pingit("Digital, Servo, Stepper Outputs", '172.16.1.111')
    pingit("Max7219, Netron, Power Control Outputs", '172.16.1.106')

    pingit("Pixel Leds Outputs", '172.16.1.105')

    pingit("Left Console GPS Raspberry Pi", '172.16.1.112')
    


def checkipconnectivity():
    pingit("Local Loopback", '127.0.0.1')

    # Do a special check to ensure you have the correct ethernet port plugged in the PC
    response_list = ping('172.16.1.10', count=1,timeout=1,verbose=False)
    if response_list.rtt_avg_ms <  10:
        print("Local Physical IP (172.16.1.10) PASS")
        pingpitdevices()
    else:
        print("LOCAL ETHERNET PING (172.16.1.10) FAILED - IS IT CONFIGURED?")


def CheckPythonVersion():
    MIN_VERSION_PY3 = 10    # min. 3.x version
    if (sys.version_info[0] < 3):
            Warning_Message = "ERROR: This script requires a minimum of Python 3." + str(MIN_VERSION_PY3) 
            print('')
            logging.critical(Warning_Message)
            print('')
            print('Invalid Version of Python running')
            print('Running Python earlier than Python 3.0! ' + sys.version)
            sys.exit(Warning_Message)

    elif (sys.version_info[0] == 3 and sys.version_info[1] < MIN_VERSION_PY3):
            Warning_Message = "ERROR: This script requires a minimum of Python 3." + str(MIN_VERSION_PY3)           
            print('')
            logging.critical(Warning_Message)  
            print('')
            print('Invalid Version of Python running')
            print('Running Python ' + sys.version)
            sys.exit(Warning_Message)


def main():
    CheckPythonVersion()

    checkipconnectivity()
    print()
    print()
    print("Checking correct Wifi is in use")

    wifi = subprocess.check_output(['netsh', 'WLAN', 'show', 'interfaces'])
    data = wifi.decode('utf-8')
    if "Shed" in data:
        print("Connected to Shed")
    else:
        print("WARNING - not connected Shed SSID")
    print()
    print()      
    input("Tests Complete. Press Enter to Continue")


main()
