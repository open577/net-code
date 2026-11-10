#pragma once

#include "Connaction.hpp"
#include "Socket.hpp"
#include "InetAddr.hpp"

#include <memory>
#include <sys/epoll.h>


class Listener : public Connaction
{
public:
    Listener(uint32_t port) : _port(port), _listener(std::make_shared<TcpSocket>())
    {
        _listener->BuildTcpSocketMethod(_port);
        SetEvent(EPOLLIN | EPOLLET);
        SetNonBlock(_listener->FD());
    }

    void Recver()
    {
        InetAddr client;
        int fd = _listener->Accept(&client);
        while (true)
        {
            if (fd == ACCEPT_DONE)
            {
                break;
            }
            else if (fd == ACCEPT_CONTINUE)
                continue;

            else if (fd == ACCEPT_ERR)
                break;

            else // 到这里 Accept得到的文件描述符是合法的
            {
                // TODO
            }
        }
    }

    void Sender() override
    {
    }

    void Excepter() override
    {
    }



    int GetSockFd()override
    {
        return _listener->FD();
    }

private:
    uint32_t _port;
    std::shared_ptr<Socket> _listener;
};