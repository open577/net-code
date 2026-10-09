#pragma once
#include "Log.hpp"
#include <functional>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>

using namespace LogMoudle;
const int defaultfd = -1;
using func_t = std::function<std::string(const std::string)>;

class Udpserver
{
public:
    Udpserver(uint32_t port, func_t func)
    {
        _sockfd = defaultfd;
        _port = port;
        _isrunning = false;
        _func = func;
    }

    void Init()
    {
        // 1 创建套接字
        _sockfd = socket(AF_INET, SOCK_DGRAM, 0);
        if (_sockfd < 0)
        {
            LOG(LogLevel::ERROR) << "创建套接字失败";
            exit(1);
        }
        LOG(LogLevel::ERROR) << "创建套接字成功";

        // 2 绑定
        // 2.1 先初始化数据
        struct sockaddr_in local;
        bzero(&local, sizeof(local));
        local.sin_family = AF_INET;
        local.sin_port = htons(_port);
        local.sin_addr.s_addr = INADDR_ANY;
        // 2.2 绑定
        int n = bind(_sockfd, (struct sockaddr *)&local, sizeof(local));
        if (n < 0)
        {
            LOG(LogLevel::INFO) << "绑定失败";
            exit(2);
        }
        LOG(LogLevel::INFO) << "绑定成功";
    }

    void Start()
    {
        _isrunning = true;
        while (_isrunning)
        {
            char buffer[1024];
            struct sockaddr_in local;
            socklen_t len = sizeof(local);
            ssize_t s = recvfrom(_sockfd, buffer, sizeof(buffer) - 1, 0, (struct sockaddr *)&local, &len);
            if (s > 0)
            {
                buffer[s] = 0;
                int peer_port = ntohs(local.sin_port);           // 从网络中拿到的！网络序列
                std::string peer_ip = inet_ntoa(local.sin_addr); // 4字节网络风格的IP -> 点分十进制的字符串风格的IP
                std::string result = _func(buffer);
                std::cout<<"["<<peer_ip<<" "<<peer_port<<"] #"<<buffer<<std::endl;
                ssize_t n = sendto(_sockfd, result.c_str(), result.size(), 0, (struct sockaddr *)&local, len);
            }
        }
    }

private:
    int _sockfd;
    uint32_t _port;
    bool _isrunning;
    func_t _func;
};