# pyHWLink_USB_Reader
# https://www.raspberrypi.org/forums/viewtopic.php?t=13680
# importing modules. Note that this program requires PyUSB.
# Reference https://github.com/pyusb/pyusb/blob/master/docs/tutorial.rst




# sudo apt install python3-usb
# sudo python3 pyHWLink_USB_Reader.py
#
# If program is interrupted, the next time it is run - it errors
#    raise USBError(_strerror(ret), ret, _libusb_errno[ret])
#    usb.core.USBError: [Errno 2] Entity not found
#
# If the target devices is unplugged and then reconnected code runs ok
# Isolated to Detach Kernel Driver - so just used separate Try block to step over it

# Reading a 100 times a second witout sending anything to stdout has CPU @ 1-3%
# If outputings a line CPU utilisation rises to 10-11%

# to list attached USB devices
#   lsusb 
#   Bus 001 Device 015: ID 16c0:05b5 Van Ooijen Technische Informatica
# The -t displays tree and max supported speeds
# Port 2: Dev 15, If 0, Class=Human Interface Device, Driver=, 12M
# At BU0836X returns
# Bus 001 Device 019: ID 1dd2:1001
# The Device Number increments for each connect event





# https://www.raspberrypi.org/forums/viewtopic.php?t=13680
# importing modules. Note that this program requires PyUSB.
# Reference https://github.com/pyusb/pyusb/blob/master/docs/tutorial.rst
import usb.core
import time

# creating the object representing the BU0836
#ub = usb.core.find(idVendor=0x16c0, idProduct=0x05b5)
# creating the object representing the BU0836X
ub = usb.core.find(idVendor=0x1dd2, idProduct=0x1001)

# checking if the UB0836 was found
if ub is None:
    raise ValueError("Neither BU0836 or BU0836X are connected")
try:
    # existing kernel drivers must be detached for this to work
    print('Detaching Kernel')
    ub.detach_kernel_driver(0)
except:
    print('unable to Detach Kernel Driver')
try:

    # setting configuration
    print('Set Configuration')
    ub.set_configuration()
    # we arere now ready to show the output of first analogue port of BU0836
    # the endPointAdress and maxPacketSize values are required for this
    start = time.time()
    i = 0
    while (i < 10):
        ubdata = ub.read(0x81, 0x0020, timeout=0)

        print(ubdata[0] + 256 * ubdata[1])
        print(ubdata)
        
        # With no buttons pressed
        # array('B', [0, 0, 0, 0, 15])
        # Button 1
        # array('B', [1, 0, 0, 0, 15])
        # Button 2
        # array('B', [2, 0, 0, 0, 15])
        # Button 3
        # array('B', [4, 0, 0, 0, 15])
        # Button 4
        # 8,0,0,0
        # Button 5
        # 16,0,0,0
        # Button 6
        # 32,0,0,0
        # Button 7
        # 64,0,0,0
        # Button 8
        # 128,0,0,0
        # Button 9
        # 0,1,0,0
        # button 10
        # 0,2,0,0
        # Button 20
        # 0,0,8,0
        # Button 32
        # 0,0,0,128
        
        
        
        
        
        





        
        
        time.sleep(0.01)
        i = i + 1
    end = time.time()
    print(end - start)

    
except Exception as other:
    print('Error in USB Reader: ' + str(other))
    