#include <iostream>
#include <string>
#include <cstring>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>

//   ./client ip port  三个参数
int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        std::cerr << " 输入无效 请输入正确的参数 ./client ip port" << std::endl;
        exit(1);
    }

    std::string server_ip = argv[1];
    uint16_t server_port = std::stoi(argv[2]);
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0)
    {
        std::cerr << "套接字创建失败" << std::endl;
        return 2;
    }

    struct sockaddr_in local;
    memset(&local, 0, sizeof(local));
    local.sin_addr.s_addr = inet_addr(server_ip.c_str());
    local.sin_family = AF_INET;
    local.sin_port = htons(server_port);
    while (true)
    {
        std::cout << "please enter";
        std::string input;
        std::getline(std::cin, input);

        ssize_t n = sendto(sockfd, input.c_str(), input.size(), 0, (struct sockaddr *)&local, sizeof(local));

        char buffer[1024];
        struct sockaddr_in local;
        socklen_t len = sizeof(local);
        ssize_t s = recvfrom(sockfd, buffer, sizeof(buffer) - 1, 0, (struct sockaddr *)&local, &len);
        if(s>0)
        {
            buffer[s]=0;
            std::cout<<buffer<<std::endl;
        }
    }
    return 0;
}