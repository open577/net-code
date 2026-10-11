#include <iostream>
#include <string>
#include <cstring>
#include <cerrno>
#include <pthread.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include "ThreadPool.hpp"
#include "TestPthread.hpp"
using namespace ThreadModlue;
// using namespace ThreadPoolMoudle;

int sockfd;
std::string server_ip;
uint16_t server_port;
pthread_t recv_thread_id;

void Recv()
{
    while (true)
    {
        char buffer[1024];
        struct sockaddr_in local;
        socklen_t len = sizeof(local);
        ssize_t s = recvfrom(sockfd, buffer, sizeof(buffer) - 1, 0, (struct sockaddr *)&local, &len);
        if (s > 0)
        {
            buffer[s] = 0;
            std::cout << buffer << std::endl;
        }
        else if (s < 0 && errno != EINTR)
        {
            std::cerr << "接收消息失败: " << std::strerror(errno) << std::endl;
            return;
        }
    }
}
void Send()
{
    struct sockaddr_in local;
    memset(&local, 0, sizeof(local));
    local.sin_family = AF_INET;
    local.sin_port = htons(server_port);
    if (inet_pton(AF_INET, server_ip.c_str(), &local.sin_addr) != 1)
    {
        std::cerr << "无效的服务器 IPv4 地址: " << server_ip << std::endl;
        pthread_cancel(recv_thread_id);
        return;
    }

    while (true)
    {
        std::cout << "please enter# ";
        std::string input;
        if (!std::getline(std::cin, input))
        {
            pthread_cancel(recv_thread_id);
            return;
        }

        ssize_t n = sendto(sockfd, input.c_str(), input.size(), 0, (struct sockaddr *)&local, sizeof(local));
        if (n < 0)
        {
            std::cerr << "发送消息失败: " << std::strerror(errno) << std::endl;
            continue;
        }

        if (input == "QUIT")
        {
            pthread_cancel(recv_thread_id);
            return;
        }
    }
}
//   ./client ip port  三个参数
int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        std::cerr << " 输入无效 请输入正确的参数 ./client ip port" << std::endl;
        exit(1);
    }

    server_ip = argv[1];
    server_port = std::stoi(argv[2]);
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0)
    {
        std::cerr << "套接字创建失败" << std::endl;
        return 2;
    }

    Thread recv(Recv);
    Thread send(Send);

    if (!recv.Start())
    {
        close(sockfd);
        return 3;
    }
    recv_thread_id = recv.ID();

    if (!send.Start())
    {
        pthread_cancel(recv_thread_id);
        recv.Join();
        close(sockfd);
        return 3;
    }

    recv.Join();
    send.Join();
    close(sockfd);
    return 0;
}