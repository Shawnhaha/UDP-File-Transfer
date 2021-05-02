all: server client

server: UDP_Server.c
	gcc UDP_Server.c -o userver

client: UDP_Client.c
	gcc UDP_Client.c -o uclient

clean:
	rm userver uclient
