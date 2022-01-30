import sys
import socket
import random
import time


sndMsg = b'./' + sys.argv[1].encode('utf-8') + b'\n'

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect(("localhost", 8989))

start_time = time.time()

s.send(sndMsg)
data = s.recv(4096)

s.close()
