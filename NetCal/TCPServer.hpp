#pragma once
#include "Socket.hpp"
#include "Log.hpp"
#include <iostream>
#include <memory>
#include <sys/wait.h>
#include <functional>

using namespace LogMoudle;
using namespace SocketModule;

using ioservice_t = std::function<void(std::shared_ptr<Socket> &sock, InetAddr &client)>;

class TCPServer
{
public:
    TCPServer(uint16_t port, ioservice_t service)
        : _port(port),
          _service(service),
          _isrunning(false),
          _listensockptr(std::make_unique<TcpSocket>())
    {
        _listensockptr->BuildTcpSocketMethod(port);
    }

    void Start()
    {
        _isrunning = true;
        while (_isrunning)
        {
            InetAddr client;
            auto acc = _listensockptr->Accept(&client);
            if (acc == nullptr)
            {
                continue;
            }
            LOG(LogLevel::INFO) << "Accept success" << client.FullName();

            pid_t pid = fork();
            if (pid < 0)
            {
                LOG(LogLevel::FATAL) << "子进程创建失败";
                exit(FORK_ERR);
            }

            else if (pid == 0)
            {
                // 子进程退出 让孙子进程执行
                _listensockptr->Close();
                if (fork() > 0)
                {
                    exit(OK);
                }

                // 孙子进程
                _service(acc, client);
                acc->Close();
                exit(OK);
            }

            else
            {
                // 父进程
                acc->Close();
                pid_t rid = ::waitpid(pid, nullptr, 0);
                (void)rid;
            }
        }
        _isrunning = false;
    }

private:
    uint16_t _port;
    std::unique_ptr<Socket> _listensockptr;
    bool _isrunning;
    ioservice_t _service;
};