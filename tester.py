import socket

host = "localhost"
port = 31337

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect((host, port))

s.send("GET /images/ein%20leerzeichen.png HTTP/1.1\r\nHost: localhost\r\n\r\n".encode("utf-8"))

received = ""
while 1:
	data = s.recv(1024).decode("latin-1")
	if data:
		received += data
	else:
		break

print(repr(received))

s.close()
