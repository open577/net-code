#pragma once
#include <iostream>
#include <sys/types.h>  // 基础类型
#include <sys/socket.h> // socket()、bind、listen、accept、connect，sockaddr结构体
#include <netinet/in.h> // sockaddr_in、htons、AF_INET、SOCK_STREAM
#include <arpa/inet.h>  // inet_addr、inet_ntoa、inet_pton/inet_ntop
#include <netdb.h>      // gethostbyname、getaddrinfo
#include <unistd.h>     // close()关闭套接字
#include <memory>

#include "Log.hpp"
#include "InetAddr.hpp"
#include "Common.hpp"

namespace SocketModule
{
    using namespace LogMoudle;
    const static int gbacklog = 16;
    // 模版方法模式
    // 基类socket, 大部分方法，都是纯虚方法
    class Socket
    {
    public:
        virtual ~Socket() {}
        virtual void SocketOrDie() = 0;
        virtual void BindOrDie(uint16_t port) = 0;
        virtual void ListenOrDie(int backlog) = 0;
        virtual std::shared_ptr<Socket> Accept(InetAddr *client) = 0;
        virtual void Close() = 0;
        virtual int Recv(std::string *out) = 0;
        virtual int Send(const std::string &message) = 0;
        virtual int Connect(const std::string &server_ip, uint16_t port) = 0;

        void BuildTcpSocketMethod(uint16_t port, int backlog = gbacklog)
        {
            SocketOrDie();
            BindOrDie(port);
            ListenOrDie(backlog);
        }
        void BuildTcpClientSocketMethod()
        {
            SocketOrDie();
        }
    };

    const static int defaultsocket = -1;
    class TcpSocket : public Socket
    {
    public:
        TcpSocket() : _sockfd(defaultsocket) {}

        TcpSocket(int fd) : _sockfd(fd) {}

        ~TcpSocket() {}

        void SocketOrDie() override
        {
            _sockfd = socket(AF_INET, SOCK_STREAM, 0);
            if (_sockfd < 0)
            {
                LOG(LogLevel::FATAL) << "创建套接字失败";
                exit(SOCK_ERR);
            }
            LOG(LogLevel::INFO) << "套接字创建成功";
        }

        void BindOrDie(uint16_t port) override
        {
            InetAddr local(port);
            int n = bind(_sockfd, local.NetAddrPtr(), local.SockLen());
            if (n < 0)
            {
                LOG(LogLevel::FATAL) << "绑定失败";
                exit(BIND_ERR);
            }
            LOG(LogLevel::FATAL) << "绑定成功";
        }

        void ListenOrDie(int backlog) override
        {
            int n = listen(_sockfd, backlog);
            if (n < 0)
            {
                LOG(LogLevel::FATAL) << "监听失败";
                exit(LISTEN_ERR);
            }
            LOG(LogLevel::FATAL) << "监听成功";
        }

        std::shared_ptr<Socket> Accept(InetAddr *client) override
        {
            struct sockaddr_in peer;
            socklen_t len = sizeof(peer);
            int n = ::accept(_sockfd, CONV(peer), &len);
            if (n < 0)
            {
                LOG(LogLevel::FATAL) << "连接失败";
                return nullptr;
            }
            client->SetAddr(peer);
            return std::make_shared<TcpSocket>(n);
        }

        void Close() override
        {
            if (_sockfd > 0)
                close(_sockfd);
        }

        int Recv(std::string *out) override
        {
            char buffer[1024];
            ssize_t n = recv(_sockfd, buffer, sizeof(buffer) - 1, 0);
            if (n > 0)
            {
                buffer[n] = 0;
                *out += buffer;
            }
            return n;
        }
        int Send(const std::string &message) override
        {
            return send(_sockfd, message.c_str(), message.size(), 0);
        }
        int Connect(const std::string &server_ip, uint16_t port) override
        {
            InetAddr peer(server_ip, port);
            return connect(_sockfd, CONV(peer), peer.SockLen());
        }

    private:
        int _sockfd;
    };

}