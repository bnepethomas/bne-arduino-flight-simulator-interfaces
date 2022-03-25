#!/usr/bin/python

# pip install ping3

#pip3 install pyusb
# For Mac
#brew install libusb


import os

import sys
import usb.core

# find USB devices
dev = usb.core.find(find_all=True)
# loop through devices, printing vendor and product ids in decimal and hex
for cfg in dev:
  sys.stdout.write('Decimal VendorID=' + str(cfg.idVendor) + ' & ProductID=' + str(cfg.idProduct) + '\n')
  sys.stdout.write('Hexadecimal VendorID=' + hex(cfg.idVendor) + ' & ProductID=' + hex(cfg.idProduct) + '\n\n')



import re
import subprocess
device_re = re.compile("Bus\s+(?P<bus>\d+)\s+Device\s+(?P<device>\d+).+ID\s(?P<id>\w+:\w+)\s(?P<tag>.+)$", re.I)
df = subprocess.check_output("lsusb")
devices = []
for i in df.split('\n'):
    if i:
        info = device_re.match(i)
        if info:
            dinfo = info.groupdict()
            dinfo['device'] = '/dev/bus/usb/%s/%s' % (dinfo.pop('bus'), dinfo.pop('device'))
            devices.append(dinfo)
print devices

def myping(host):
    response = os.system("ping -c 1 -t 1 " + host)
    
    if response == 0:
        return True
    else:
        return False
        
print("Google " + str(myping("www.google.com")))
print("Default Gateway " + str(myping("192.168.2.1")))
print("Fail " + str(myping("192.168.4.32")))


