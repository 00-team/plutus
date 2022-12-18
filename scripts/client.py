# import socket
# import sys

# # Create a UDS socket
# sock = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)

# # Connect the socket to the port where the server is listening
# server_address = './uds_socket'
# print >>sys.stderr, 'connecting to %s' % server_address
# try:
#     sock.connect(server_address)
# except socket.error, msg:
#     print >>sys.stderr, msg
#     sys.exit(1)

import socket

sock = socket.socket(socket.AF_UNIX, socket.SOCK_DGRAM)

client_addr = '/tmp/client_plutus.sock'
server_addr = '/tmp/plutus.sock'

sock.bind(client_addr)
sock.connect(server_addr)


while (True):
    inp = input('inp: ')
    if inp == 'q':
        break
    sock.send(inp.encode('utf-8'))
    print(sock.recv(256, 0))
