#pragma once
#include <iostream>
#include <memory>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/epoll.h>

#include "Log.hpp"
#include "Socket.hpp"
#include "InetAddr.hpp"
using namespace LogMoudle;
using namespace SocketModule;

class EpollServer
{
    const static int defaultmaxfd = 1024;
    const static int defaultfd = -1;
    const static int buffersize = 1024;

public:
    EpollServer(int port) : _listensocket(std::make_unique<TcpSocket>(port)), _isrunning(false)
    {
        _listensocket->BuildTcpSocketMethod(port);
        _epfd = epoll_create(1024);
        if (_epfd < 0)
        {
            LOG(LogLevel::FATAL) << "epoll_create failed";
            exit(EPOLL_CREATE_ERROR);
        }

        LOG(LogLevel::INFO) << "epoll_create success " << _epfd;

        struct epoll_event en;
        en.data.fd = _listensocket->FD();
        en.events = EPOLLIN;

        int n = epoll_ctl(_epfd, EPOLL_CTL_ADD, en.data.fd, &en);
        if (n < 0)
        {
            LOG(LogLevel::ERROR) << "epoll_ctl add listenfd failed";
        }

        LOG(LogLevel::INFO) << "epoll_ctl add listenfd success " << n;
    }

    void Start()
    {
        _isrunning = true;
        while (_isrunning)
        {

            int timeout = 1000;
            int n = epoll_wait(_epfd, _res, defaultmaxfd, timeout);
            switch (n)
            {
            case 0:
            {
                LOG(LogLevel::INFO) << "等待时间超时";
                continue;
            }
            case -1:
            {
                LOG(LogLevel::ERROR) << "读取出错";
                break;
            }

            default:
            {
                LOG(LogLevel::INFO) << "等待成功";
                // TODO 事件派发
                Dispatcher();
                break;
            }
            }
        }
        _isrunning = false;
    }

    void Dispatcher()
    {
        std::cout << "派发事件" << std::endl;
        for (int i = 0; i < defaultmaxfd; i++)
        {
            if (_res[i].events & EPOLLIN)
            {
                if (_res[i].data.fd == _listensocket->FD()) // 监听事件就绪
                    Accept();

                else // 读事件就绪
                {
                    Recv(_res[i].data.fd, i);
                }
            }
        }
    }
    void Accept()
    {
        std::cout << "Accept" << std::endl;
        InetAddr client;
        struct epoll_event ev;
        ev.data.fd = _listensocket->Accept(&client);
        ev.events = EPOLLIN;

        int n = epoll_ctl(_epfd, EPOLL_CTL_ADD, ev.data.fd, &ev);
        if (n < 0)
        {
            LOG(LogLevel::ERROR) << "添加新fd失败";
        }

        LOG(LogLevel::ERROR) << "添加新fd 成功 " << ev.data.fd;

        std::cout << "Accept done" << std::endl;
    }

    void Recv(int fd, int pos)
    {
        std::cout << "Recv" << std::endl;
        char buff[buffersize];
        ssize_t n = recv(fd, buff, sizeof(buff) - 1, 0);
        if (n > 0)
        {
            buff[n] = 0;
            std::cout << buff << std::endl;
        }

        else if (n == 0)
        {
            LOG(LogLevel::INFO) << "发送端关闭";

            // epoll ctl 只能关闭有效文件描述符 所以只能先设置删除再close

            uint32_t events = _res[pos].events;

            int n = epoll_ctl(_epfd, EPOLL_CTL_DEL, fd, nullptr);
            close(fd);
        }

        else
        {
            if (errno == EINTR)
            {
                return;
            }

            if (errno == EAGAIN || errno == EWOULDBLOCK)
            {
                LOG(LogLevel::INFO) << "被信号打断";
            }
            LOG(LogLevel::FATAL) << "读取出错";

            uint32_t events = _res[pos].events;

            int n = epoll_ctl(_epfd, EPOLL_CTL_DEL, fd, nullptr);
            close(fd);
        }
    }


    void Stop()
    {
        _isrunning = false;
    }

    ~EpollServer()
    {
    }

private:
    std::unique_ptr<Socket> _listensocket;
    bool _isrunning;
    int _epfd;

    struct epoll_event _res[defaultmaxfd];
};