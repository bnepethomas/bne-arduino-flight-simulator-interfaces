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


import usb.core
import usb.backend.libusb1

def ScanUSB():
    backend = usb.backend.libusb1.get_backend(find_library=lambda x: "C:\libusb\MS64\dll\libusb-1.0.dll")

    dev = usb.core.find(backend=backend, find_all=True)


    devices = usb.core.find(find_all=True)

    for dev in devices:

        try:
            print(dev._get_full_descriptor_str())

        except:
            print("Unable to get descriptor")

        try:
            xdev = usb.core.find(idVendor=dev.idVendor, idProduct=dev.idProduct)
            if xdev._manufacturer is None:
                xdev._manufacturer = usb.util.get_string(xdev, xdev.iManufacturer)
            if xdev._product is None:
                xdev._product = usb.util.get_string(xdev, xdev.iProduct)
            stx = '%6d %6d: '+str(xdev._manufacturer).strip()+' = '+str(xdev._product).strip()
                   
            print (str(xdev._manufacturer).strip(),":",str(xdev._product).strip())
            
        except:
            print("Unknown devivce")
            pass
        print("Bus:", dev.bus, " Address:", dev.address, " Port:", dev.port_number," Speed:", dev.speed)
        print()



from pythonping import ping

def pingit(Target_Name, Target_IP):

    show_all_pings = False
    response_list = ping(Target_IP, count=1,timeout=1,verbose=show_all_pings)
    if response_list.rtt_avg_ms <  10:
        print(Target_Name + " : PASS")
    else:
        print("WARNING " + Target_Name + " : FAIL")
        input("Press Enter to continue")

def pingpitdevices():

    pingit("Forward Console Inputs", '172.16.1.101')
    pingit("Left Console Inputs", '172.16.1.100')
    pingit("Right Console Inputs", '172.16.1.103')
    pingit("UDP to Keystroke Converter", '172.16.1.110')

    pingit("Digital, Servo, Stepper Outputs", '172.16.1.111')
    pingit("Max7219, Netron, Power Control Outputs", '172.16.1.106')

    pingit("Pixel Leds Outputs", '172.16.1.105')
    


def checkipconnectivity():
    pingit("Local Loopback", '127.0.0.1')

    # Do a special check to ensure you have the correct ethernet port plugged in the PC
    response_list = ping('172.16.1.10', count=1,timeout=1,verbose=False)
    if response_list.rtt_avg_ms <  10:
        print("Local Physical IP (172.16.1.10) Pass")
        pingpitdevices()
    else:
        print("LOCAL ETHERNET PING (172.16.1.10) FAILED - IS IT CONFIGURED?")


checkipconnectivity()
