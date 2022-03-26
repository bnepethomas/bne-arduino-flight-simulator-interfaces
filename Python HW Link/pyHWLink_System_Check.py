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


def myping(host):
    response = os.system("ping -n 1 " + host)
    
    if response == 0:
        return True
    else:
        return False

       
print("Google " + str(myping("www.google.com")))
print("Default Gateway " + str(myping("192.168.2.1")))

from pythonping import ping

response_list = ping('192.168.4.45', count=1,timeout=1,verbose=True)
if response_list.success == True:
    print("Pass")
else:
    print("Fail")


