#pragma once

#include <functional>
#include <iostream>
#include <string>
#include <stdint.h>
#include <fcntl.h>

#include "Socket.hpp"
#include "Reactor.hpp"


class Reactor;
using namespace SocketModule;
void SetNonBlock(int fd)
{
    int fl = fcntl(fd, F_GETFL);
    if (fl < 0)
    {
        perror("fcntl file\n");
        return;
    }

    fcntl(fd, F_SETFL, fl | O_NONBLOCK);
}
using func_t = std::function<std::string(std::string &)>;


class Connaction
{
public:
    Connaction():_event(0),_react(nullptr)
    {
    }

    uint32_t GetEnent()
    {
        return _event;
    }

    void SetEvent(uint32_t event)
    {
        _event=event;
    }
    
    void SetOnwer(Reactor *ower)
    {
        _react=ower;
    }

    Reactor *GetOwer()
    {
        return _react;
    }

    void RegisterFunc(func_t func)
    {
        _func=func;
    }

    virtual void Recver()=0;
    virtual void Sender()=0;
    virtual void Excepter()=0;
    virtual int GetSockFd()=0;
    func_t _func;

private:
    Reactor *_react;
    uint32_t _event;

};