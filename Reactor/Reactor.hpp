#pragma once
#include "Log.hpp"
#include "Connaction.hpp"
#include "Epoller.hpp"
#include <unordered_map>

#include <memory>

class Reactor
{
    static const int res_num = 128;

private:
    bool IsConnectionExistsHelper(int fd)
    {
        auto item = _connection.find(fd);
        if (item == _connection.end())
        {
            return false;
        }

        return true;
    }

    bool IsConnectionExists(std::unique_ptr<Connaction> &con)
    {
        return IsConnectionExistsHelper(con->GetSockFd());
    }
    bool IsConnectionExists(int sockfd)
    {
        return IsConnectionExistsHelper(sockfd);
    }

    bool IsConnectionEmpty()
    {
        return _connection.empty();
    }

    void LopOnce(int timeout)
    {
        _epol->WaitEvent(_res, res_num, timeout);
    }

    void Dispatcher(int n)
    {
        for (int i = 0; i < n; i++)
        {
            int sockfd = _res[i].data.fd;
            uint32_t event = _res->events;

            if (event & EPOLLERR)
                event |= (EPOLLIN | EPOLLOUT);

            if (event & EPOLLHUP)
                event |= (EPOLLIN | EPOLLOUT);

            if (event & EPOLLIN)
            {
                if (IsConnectionExists(sockfd))
                    _connection[sockfd]->Recver();
            }

            if (event & EPOLLOUT)
            {
                if (IsConnectionExists(sockfd))
                    _connection[sockfd]->Sender();
            }
        }
    }

public:
    Reactor() : _isrunning(false), _epol(std::make_unique<Epoller>())
    {
    }
    void Loop()
    {
        if (IsConnectionEmpty())
        {
            return;
        }

        _isrunning = true;
        while (_isrunning)
        {
            int timeout = -1;
            LopOnce(timeout);
            Dispatcher(timeout);
        }
        _isrunning = false;
    }

    void AddConnection(std::shared_ptr<Connaction> &conn)
    {
        if (IsConnectionExists(conn->GetSockFd()))
        {
            LOG(LogLevel::INFO) << "connection exits " << conn->GetSockFd();
            return;
        }

        int sockfd = conn->GetSockFd();
        uint32_t event = conn->GetEnent();
        _epol->AddEvent(sockfd, event);

        conn->SetOnwer(this);

        _connection[sockfd] = conn;
    }
    void EnableReadWrite(int fd, bool enableread, bool enablewrite)
    {
        if (IsConnectionExists(fd)) // 不能添加新的已经有的链接
        {
            LOG(LogLevel::INFO) << "connection exits " << fd;
            return;
        }

        uint32_t new_event = (EPOLLET | (enableread ? EPOLLIN : 0) | (enablewrite ? EPOLLOUT : 0));
        _epol->AddEvent(fd, new_event);
        _connection[fd]->SetEvent(new_event);
    }

    void DelConnection(int sockfd)
    {
        if (!IsConnectionExists(sockfd))
        {
            LOG(LogLevel::ERROR) << "connection not exits" << sockfd;
            return;
        }

        // 从内核中删除
        _epol->DelEvent(sockfd);
        // 从connection中删除
        _connection.erase(sockfd);

        LOG(LogLevel::DEBUG) << "DelConnection success " << sockfd;
    }
    void Stop()
    {
        _isrunning = false;
    }
    void PrintConnection()
    {
        std::cout << "当前Reactor正在进行管理的fd List:";
        for (auto &conn : _connection)
        {
            std::cout << conn.second->GetSockFd() << " ";
        }
        std::cout << "\r\n";
    }
    ~Reactor()
    {
    }

private:
    bool _isrunning;
    std::unique_ptr<Epoller> _epol;
    std::unordered_map<int, std::shared_ptr<Connaction>> _connection;
    struct epoll_event _res[res_num];
};