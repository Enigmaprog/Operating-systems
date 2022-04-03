all : client server

client : client.c params.h
	gcc client.c -o client.out

server : server.c params.h
	gcc server.c -o server.out

clean :
	rm client.out server.out