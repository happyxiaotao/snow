all:server client

server:net/NetAddress.cpp net/Socket.cpp Buffer.cpp EventLoop.cpp simple_server.cpp
	g++ $^ -o $@
client:net/NetAddress.cpp net/Socket.cpp Buffer.cpp EventLoop.cpp simple_client.cpp
	g++ $^ -o $@

.PHONY:
clean:
	rm server client
