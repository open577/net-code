# .PHONY:all
# 	all:tcpserver tcpclient
# tcpserver:TCPServer.cc
# 	g++ TCPServer.cc -o tcpserver -g
# tcpclient:TCPClient.cc
# 	g++ TCPClient.cc -o tcpclient -g

# .PHONY:clean
# clean:
# 	rm -rf tcpserver tcpclient
.PHONY: all clean
all: tcpserver tcpclient

tcpserver: TCPServer.cc
	g++ TCPServer.cc -o tcpserver -g

tcpclient: TCPClient.cc
	g++ TCPClient.cc -o tcpclient -g

clean:
	rm -f tcpserver tcpclient
