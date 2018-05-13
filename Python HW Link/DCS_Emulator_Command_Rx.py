import socket

import time

UDP_IP_ADDRESS = "127.0.0.1"
UDP_PORT_NO = 26027
serverSock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
serverSock.settimeout(0.0001)
serverSock.bind((UDP_IP_ADDRESS, UDP_PORT_NO))

#sanity

a=0
while True:
    
    try:
        data, addr = serverSock.recvfrom(1500)
        print ("Message: ", data)
        print(a)
        a=0

                                          
    except socket.timeout:
        a=a+1
        if (a > 1000):
            print("Long Receive Timeout - ", time.asctime())
            a=0
        continue