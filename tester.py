import socket

host = "localhost"
port = 31337

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect((host, port))

s.send("GET / HTTP/1.1\r\nHost: localhost\r\nConnection: close\r\n\r\n".encode("utf-8"))

data = s.recv(1024).decode()
print(repr(data))

s.close()
