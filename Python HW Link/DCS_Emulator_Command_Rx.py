import socket

UDP_IP_ADDRESS = "127.0.0.1"
UDP_PORT_NO = 26027
serverSock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
serverSock.settimeout(0.0001)
serverSock.bind((UDP_IP_ADDRESS, UDP_PORT_NO))

#sanity

a=0
while True:
    
    try:
        data, addr = serverSock.recvfrom(1024)
        print ("Message: ", data)
	print(a)
	a=0

                                          
    except socket.timeout:
        a=a+1
        continue