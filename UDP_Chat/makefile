.PHONY:all
all:udpserver udpclient
udpserver:UDPServer.cc
	g++ UDPServer.cc -o udpserver -g
udpclient:UDPClient.cc
	g++ UDPClient.cc -o udpclient -g

.PHONY:clean
clean:
	rm -rf udpclient udpserver