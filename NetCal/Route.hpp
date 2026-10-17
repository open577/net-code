#pragma once
#include <iostream>
#include <vector>
#include <cerrno>
#include <cstring>
#include "Log.hpp"
#include "InetAddr.hpp"
using namespace LogMoudle;

class Route
{

    bool IsExist(InetAddr &client)
    {
        for (auto user : _online_user)
        {
            if (client == user)
            {
                return true;
            }
        }
        return false;
    }
    void UserAdd(InetAddr &client)
    {
        LOG(LogLevel::INFO) << "新增一个在线用户: " << client.FullName();
        _online_user.push_back(client);
    }
    void DeleteUser(InetAddr &client)
    {
        for (auto it = _online_user.begin(); it != _online_user.end(); it++)
        {
            if (*it == client)
            {
                LOG(LogLevel::INFO) << "删除一个在线用户:" << client.FullName() << "成功";
                _online_user.erase(it);
                break;
            }
        }
    }

public:
    void MessageRoute(int sockfd, const std::string &mes, InetAddr &client)
    {
        if (!IsExist(client))
        {
            LOG(LogLevel::INFO) << "正在新增一个用户: " << client.FullName();
            UserAdd(client);
        }

        std::string prevname = client.FullName()+" " + mes;

        // 发送消息
        for (auto &user : _online_user)
        {
            if (sendto(sockfd, prevname.c_str(), prevname.size(), 0,
                       (sockaddr *)&user.Addr(), sizeof(user.Addr())) < 0)
            {
                LOG(LogLevel::ERROR) << "发送消息失败: " << std::strerror(errno);
            }
        }

        if (mes == "QUIT")
        {
            LOG(LogLevel::INFO) << "删除一个在线用户: " << client.FullName();
            DeleteUser(client);
        }
    }

private:
    std::vector<InetAddr> _online_user;
};