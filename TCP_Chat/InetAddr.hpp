#pragma once
#include <iostream>
#include "Common.hpp"

class InetAddr
{
public:
    InetAddr(struct sockaddr_in &addr) : _addr(addr)
    {
        _port = ntohs(_addr.sin_port);
        char buf[64];
        inet_ntop(AF_INET, &_addr.sin_addr, buf, INET_ADDRSTRLEN);
        _ip = std::string(buf);
    }

    InetAddr() {}

    InetAddr(const std::string ip, uint16_t port) : _ip(ip), _port(port)
    {
        memset(&_addr, 0, sizeof(_addr));
        _addr.sin_family = AF_INET;
        inet_pton(AF_INET, _ip.c_str(), &_addr.sin_addr);
        _addr.sin_port = _port;
    }

    InetAddr(uint16_t port) :_port(port)
    {
        memset(&_addr, 0, sizeof(_addr));
        _addr.sin_family = AF_INET;
        _addr.sin_port = _port;
    }

    
    bool operator==(InetAddr client)
    {
        return client._ip == this->_ip && client._port == this->_port;
    }

    std::string FullName()
    {
        return _ip + std::to_string(_port);
    }

    std::string GetIP()
    {
        return _ip;
    }

    struct sockaddr *NetAddrPtr()
    {
        return CONV(_addr);
    }
    uint16_t GetPort()
    {
        return _port;
    }

    socklen_t SockLen()
    {
        return sizeof(_addr);
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