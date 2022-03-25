#!/usr/bin/python

# pip install ping3

#pip3 install pyusb
# For Mac
#brew install libusb


import os


import usb.core
import usb.backend.libusb1

busses = usb.busses()
for bus in busses:

    devices = bus.devices
    for dev in devices:
        if dev != None:
            try:
         
                
                xdev = usb.core.find(idVendor=dev.idVendor, idProduct=dev.idProduct)
                if xdev._manufacturer is None:
                    xdev._manufacturer = usb.util.get_string(xdev, xdev.iManufacturer)
                if xdev._product is None:
                    xdev._product = usb.util.get_string(xdev, xdev.iProduct)
                stx = '%6d %6d: '+str(xdev._manufacturer).strip()+' = '+str(xdev._product).strip()
                print (stx % (dev.idVendor,dev.idProduct))

                
            except:
                pass




devices = usb.core.find(find_all=True)

for dev in devices:

    try:
        xdev = usb.core.find(idVendor=dev.idVendor, idProduct=dev.idProduct)
        if xdev._manufacturer is None:
            xdev._manufacturer = usb.util.get_string(xdev, xdev.iManufacturer)
        if xdev._product is None:
            xdev._product = usb.util.get_string(xdev, xdev.iProduct)
        stx = '%6d %6d: '+str(xdev._manufacturer).strip()+' = '+str(xdev._product).strip()
        print (stx % (dev.idVendor,dev.idProduct))
    
    except:
            pass
    print("device bus:", dev.bus)
    print("device address:", dev.address)
    print("device port:", dev.port_number)
    print("device speed:", dev.speed)

def myping(host):
    response = os.system("ping -c 1 -t 1 " + host)
    
    if response == 0:
        return True
    else:
        return False
        
print("Google " + str(myping("www.google.com")))
print("Default Gateway " + str(myping("192.168.2.1")))
print("Fail " + str(myping("192.168.4.32")))


