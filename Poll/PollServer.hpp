#pragma once
#include <iostream>
#include <memory>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/poll.h>

#include "Log.hpp"
#include "Socket.hpp"
#include "InetAddr.hpp"
using namespace LogMoudle;
using namespace SocketModule;

class PollServer
{
    const static int defaultmaxfd = sizeof(fd_set) * 8;
    const static int defaultfd = -1;
    const static int buffersize = 1024;

public:
    PollServer(int port) : _listensocket(std::make_unique<TcpSocket>(port)), _isrunning(false)
    {
        _listensocket->BuildTcpSocketMethod(port);

        for (int i = 0; i < defaultmaxfd; i++)
        {
            _fds[i].fd = defaultfd;
            _fds[i].events = 0;
            _fds[i].revents = 0;
        }

        _fds[0].fd = _listensocket->FD();
        _fds[0].events = POLLIN;
        _fds[0].revents = 0;
    }

    void Start()
    {
        _isrunning = true;
        while (_isrunning)
        {

            // int timeout = -1;
            int n = poll(_fds, defaultmaxfd, 1000);
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
                PrintFD();
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
            if (_fds[i].fd == defaultfd)
                continue;

            else if (_fds[i].revents & POLLIN)
            {
                if (_fds[i].fd == _listensocket->FD()) // 监听事件就绪
                    Accept();

                else // 读事件就绪
                {
                    Recv(_fds[i].fd, i);
                }
            }
        }
    }
    void Accept()
    {
        std::cout << "Accept" << std::endl;
        InetAddr client;
        int sockfd = _listensocket->Accept(&client);
        int pos = 0;
        for (; pos < defaultmaxfd; pos++)
        {
            if (_fds[pos].fd == defaultfd)
            {
                _fds[pos].fd = sockfd;
                _fds[pos].events = POLLIN;
                _fds[pos].revents = 0;

                break;
                // continue;
            }

            else if (pos == defaultmaxfd)
            {
                LOG(LogLevel::WARNING) << "文件超载";
                close(sockfd);
                break;
            }

            // else
            // {
            // }
        }
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
            close(fd);

            _fds[pos].fd = defaultfd;
            _fds[pos].events = 0;
            _fds[pos].revents = 0;

        }

        else
        {
            if(errno==EINTR)
            {
                return;
            }

            if(errno==EAGAIN||errno==EWOULDBLOCK)
            {
                LOG(LogLevel::INFO)<<"被信号打断";
            }
            LOG(LogLevel::FATAL) << "读取出错";
            close(fd);

            _fds[pos].fd = defaultfd;
            _fds[pos].events = 0;
            _fds[pos].revents = 0;

        }
    }
    void PrintFD()
    {
        std::cout << "_fd_arry:";

        for (int i = 0; i < defaultmaxfd; i++)
        {
            if (_fds[i].fd == defaultfd)
                continue;
            std::cout << _fds[i].fd << " ";
        }
        std::cout << "\r\n";
    }

    void Stop()
    {
        _isrunning = false;
    }

    ~PollServer()
    {
    }

private:
    std::unique_ptr<Socket> _listensocket;
    bool _isrunning;
    struct pollfd _fds[defaultmaxfd];
};