OBJS	= echo_server.o authorization.o base64.o hash.o helpers.o request.o response.o string.o tests.o
SOURCE	= echo_server.c authorization.c base64.c hash.c helpers.c request.c response.c string.c tests.c
HEADER	= authorization.h base64.h hash.h helpers.h request.h response.h string.h tests.h
OUT	= http-server
CC	 = gcc
FLAGS	 = -g -c -Wall
LFLAGS	 = -lcrypto

all: $(OBJS)
	$(CC) -g $(OBJS) -o $(OUT) $(LFLAGS)

echo_server.o: echo_server.c
	$(CC) $(FLAGS) echo_server.c 

authorization.o: authorization.c
	$(CC) $(FLAGS) authorization.c 

base64.o: base64.c
	$(CC) $(FLAGS) base64.c 

hash.o: hash.c
	$(CC) $(FLAGS) hash.c 

helpers.o: helpers.c
	$(CC) $(FLAGS) helpers.c 

request.o: request.c
	$(CC) $(FLAGS) request.c 

response.o: response.c
	$(CC) $(FLAGS) response.c 

string.o: string.c
	$(CC) $(FLAGS) string.c 

tests.o: tests.c
	$(CC) $(FLAGS) tests.c 


clean:
	rm -f $(OBJS) $(OUT)
