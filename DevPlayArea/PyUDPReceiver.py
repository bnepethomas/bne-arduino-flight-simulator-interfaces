import socket

UDP_IP_ADDRESS = "192.168.3.100"
UDP_PORT_NO = 15151
serverSock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
serverSock.settimeout(0.0001)
serverSock.bind((UDP_IP_ADDRESS, UDP_PORT_NO))

while True:
    
    try:
        data, addr = serverSock.recvfrom(1024)
        print ("Message: ", data)
                                          
    except socket.timeout:
        a=0
