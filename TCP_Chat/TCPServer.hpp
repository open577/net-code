#pragma once
#include <sys/wait.h>
#include "Log.hpp"
#include "InetAddr.hpp"
#include "Common.hpp"
#include "ThreadPool.hpp"

using namespace ThreadPoolMoudle;
using namespace LogMoudle;

int defaultlistensockfd = -1;
int backlog = 5;

using func_t = std::function<std::string(std::string, InetAddr &)>;
using task_t = std::function<void()>;

class TCPServer : public noCopy
{
public:
    TCPServer(uint16_t port, func_t func)
        : _port(port),
          _listensockfd(defaultlistensockfd),
          _isrunning(false),
          _func(func)
    {
    }

    void Init()
    {
        _listensockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (_listensockfd < 0)
        {
            LOG(LogLevel::FATAL) << "套接字创建失败";
            exit(SOCK_ERR);
        }
        LOG(LogLevel::DEBUG) << "套接字创建成功 " << _listensockfd;

        InetAddr local(_port);
        int n = bind(_listensockfd, local.NetAddrPtr(), local.SockLen());
        if (n < 0)
        {
            LOG(LogLevel::FATAL) << "绑定失败";
            exit(BIND_ERR);
        }
        LOG(LogLevel::DEBUG) << "绑定成功 " << _listensockfd;

        n = listen(_listensockfd, backlog);
        if (n < 0)
        {
            LOG(LogLevel::ERROR) << "监听失败";
            exit(LISTEN_ERR);
        }
        LOG(LogLevel::INFO) << "监听成功 " << _listensockfd;
    }

    class ThreadData
    {
    public:
        ThreadData(int fd, InetAddr &ar, TCPServer *s) : sockfd(fd), addr(ar), tsvr(s)
        {
        }

    public:
        int sockfd;
        InetAddr addr;
        TCPServer *tsvr;
    };

    void Serves(const int &sockfd, InetAddr &peer)
    {
        char buff[1024];
        while (true)
        {
            size_t n = read(sockfd, buff, sizeof(buff) - 1);
            if (n > 0)
            {
                buff[n] = 0;
                LOG(LogLevel::DEBUG) << peer.FullName() << " " << buff;
            }

            else if (n == 0)
            {
                LOG(LogLevel::DEBUG) << peer.FullName() << " 已退出";
                close(sockfd);
                break;
            }

            else
            {
                LOG(LogLevel::DEBUG) << peer.FullName() << " 读取异常";
                close(sockfd);
                break;
            }
        }
    }

    static void *Routine(void *mes)
    {
        pthread_detach(pthread_self());
        ThreadData *td = static_cast<ThreadData *>(mes);
        td->tsvr->Serves(td->sockfd, td->addr);
        delete td;
        return nullptr;
    }
    void Run()
    {
        _isrunning = true;
        while (_isrunning)
        {
            struct sockaddr_in peer;
            socklen_t len = sizeof(peer);
            int sockfd = accept(_listensockfd, CONV(peer), &len);
            if (sockfd < 0)
            {
                LOG(LogLevel::INFO) << " 绑定失败 ";
                continue;
            }
            InetAddr local(peer);
            LOG(LogLevel::INFO) << " 绑定成功 " << local.FullName();

            ThreadData *td = new ThreadData(sockfd, local, this);
            pthread_t tid;
            pthread_create(&tid, nullptr, Routine, td);
            // 1
            // Serves(_listensockfd, local);

            // 2
            // int f = fork();
            // if (f < 0)
            // {
            //     LOG(LogLevel::ERROR) << "子进程创建失败";
            //     exit(FORK_ERR);
            // }
            // else if (f == 0)
            // {
            //     父进程
            //     if (fork() > 0) // 创建孙子进程 子进程退出 让孙子进程执行任务
            //     {
            //         close(_listensockfd);
            //         exit(OK);
            //     }
            //     Serves(_listensockfd, local);
            //     exit(OK);
            // }

            // else
            // {
            //     close(sockfd);
            //     pid_t pt = waitpid(f, nullptr, 0);
            // }

            // 3
            // version2: 多线程版本
            // ThreadData *td = new ThreadData(sockfd, local, this);
            // pthread_t tid;
            // pthread_create(&tid, nullptr, Routine, td);

            // // version3：线程池版本，线程池一般比较适合处理短服务
            // // 将新链接和客户端构建一个新的任务，push线程池中
            // ThreadPool<task_t>::GetInstance()->Enqueue([this, sockfd, &local]()
            //                                            { this->Serves(sockfd, local); });
        }
        _isrunning = false;
    }

    ~TCPServer() {}

private:
    uint16_t _port;
    int _listensockfd;
    bool _isrunning;
    func_t _func;
};