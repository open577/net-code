#pragma once
#include <iostream>
#include <memory>
#include <sys/select.h>
#include <sys/time.h>

#include "Log.hpp"
#include "Socket.hpp"
#include "InetAddr.hpp"
using namespace LogMoudle;
using namespace SocketModule;

class SelectServer
{
    const static int defaultmaxfd = sizeof(fd_set) * 8;
    const static int defaultfd = -1;
    const static int buffersize = 1024;

public:
    SelectServer(int port) : _listensocket(std::make_unique<TcpSocket>(port)), _isrunning(false)
    {
        _listensocket->BuildTcpSocketMethod(port);
        for (int i = 0; i < defaultmaxfd; i++)
        {
            _fd_arry[i] = defaultfd;
        }

        _fd_arry[0] = _listensocket->FD();
    }

    void Start()
    {
        _isrunning = true;
        while (_isrunning)
        {
            fd_set fds;
            FD_ZERO(&fds); // 清零

            int maxfd = 0;
            for (int i = 0; i < defaultmaxfd; i++) // 每一次select前都要更新fd_set
            {
                if (_fd_arry[i] == defaultfd)
                    continue;

                FD_SET(_fd_arry[i], &fds);
                if (maxfd < _fd_arry[i]) // 更新最大fd
                {
                    maxfd = _fd_arry[i];
                }
            }

            int n = select(maxfd + 1, &fds, nullptr, nullptr, nullptr);//等待
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
                Dispatcher(fds);
                break;
            }
            }
        }
        _isrunning = false;
    }

    void Dispatcher(fd_set &fds)
    {
        std::cout << "派发事件" << std::endl;
        for (int i = 0; i < defaultmaxfd; i++)
        {
            if (_fd_arry[i] == defaultfd)
                continue;

            else if (FD_ISSET(_fd_arry[i], &fds))
            {
                if (_fd_arry[i] == _listensocket->FD()) // 监听事件就绪
                    Accept();

                else // 读事件就绪
                {
                    Recv(_fd_arry[i],i);
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
            if (_fd_arry[pos] == defaultfd)
            {
                _fd_arry[pos] = sockfd;
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

    void Recv(int fd,int pos)
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
            _fd_arry[pos]=defaultfd;
            close(fd);
        }

        else
        {
            LOG(LogLevel::FATAL) << "读取出错";
            _fd_arry[pos]=defaultfd;
            close(fd);
        }
    }
    void PrintFD()
    {
        std::cout << "_fd_arry:";

        for (int i = 0; i < defaultmaxfd; i++)
        {
            if (_fd_arry[i] == defaultfd)
                continue;
            std::cout << _fd_arry[i] << " ";
        }
        std::cout << "\r\n";
    }

    void Stop()
    {
        _isrunning = false;
    }

    ~SelectServer()
    {
    }

private:
    std::unique_ptr<Socket> _listensocket;
    bool _isrunning;
    int _fd_arry[defaultmaxfd];
};