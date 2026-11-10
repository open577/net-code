#pragma once
#include "Log.hpp"
#include "Connaction.hpp"
#include "Epoller.hpp"
#include <unordered_map>

class Reactor
{
    static const int res_num = 128;

private:
    bool IsConnectionExistsHelper(int fd)
    {
        auto item=_connection.find(fd);
        if(item==_connection.end())
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
        _epol->WaitEvent(_res,res_num,timeout);
    }

public:
private:
    std::unique_ptr<Epoller> _epol;
    std::unordered_map<int, std::shared_ptr<Epoller>> _connection;
    bool _isrunning;
    struct epoll_event _res[res_num];
};