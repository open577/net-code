#pragma once
#include <iostream>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>

class InetAddr
{
public:

    bool operator==(InetAddr client)
    {
        return client._ip==this->_ip&&client._port==this->_port;
    }

    std::string FullName()
    {
        return _ip+std::to_string(_port);
    }

    InetAddr(struct sockaddr_in &addr) : _addr(addr)
    {
        _port = ntohs(_addr.sin_port);
        _ip = inet_ntoa(_addr.sin_addr);
    }

    std::string GetIP()
    {
        return _ip;
    }

    uint16_t GetPort()
    {
        return _port;
    }

    struct sockaddr_in &Addr()
    {
        return _addr;
    }
private:
    struct sockaddr_in _addr;
    uint16_t _port;
    std::string _ip;
};