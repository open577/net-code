#pragma once
#include "Connaction.hpp"
#include "InetAddr.hpp"
class Reactor;
#define SIZE 1024

Reactor *EnableReadWrite();
class Channel : public Connaction
{
public:
    Channel(int sockfd, InetAddr &client) : _sockfd(sockfd), _client(client)
    {
        SetNonBlock(_sockfd);
    }

    void Recver() override
    {
        char buff[SIZE];
        while (true)
        {
            int n = recv(_sockfd, buff, sizeof(buff), 0);

            if (n > 0)
            {
                _inbuffer += buff;
            }

            else if (n == 0)
            {
                Excepter();
            }

            else
            {
                if (errno == EAGAIN || errno == EWOULDBLOCK)
                    break;

                else if (errno == EINTR)
                    continue;

                else
                {
                    Excepter();
                    return;
                }
            }
        }

        LOG(LogLevel::DEBUG)<<"client say : "<<buff;
        if(!_inbuffer.empty())
        {
            _outbuffer+=_func(_inbuffer);
        }

        if(!_outbuffer.empty())
        {
            Sender();
        }
    }

    void Sender() override
    {
        while (true)
        {
            int n = send(_sockfd, _outbuffer.c_str(), sizeof(_outbuffer), 0);

            if (n > 0)
            {
                _outbuffer.erase(0, n);
                if(_outbuffer.empty())
                    break;
            }

            else if (n == 0)
            {
                Excepter();
            }

            else
            {
                if (errno == EAGAIN || errno == EWOULDBLOCK)
                    break;

                else if (errno == EINTR)
                    continue;

                else
                {
                    Excepter();
                    return;
                }
            }
        }
        GetOwer()->EnableReadWrite(_sockfd,true,true);
    }

    void Excepter() override
    {}

    int GetSockFd()override
    {
        return _sockfd;
    }
private:
    int _sockfd;
    std::string _inbuffer;
    std::string _outbuffer;
    InetAddr _client;
};