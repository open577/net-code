#include "Common.hpp"
#include "Log.hpp"
#include "InetAddr.hpp"

using namespace LogMoudle;
void USGE(std::string mes)
{
    std::cout << "USGE " << mes << "serverip + serverport" << std::endl;
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        USGE(argv[0]);
        exit(USGE_ERR);
    }

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0)
    {
        LOG(LogLevel::FATAL) << "创建套接字失败";
        exit(SOCK_ERR);
    }
    LOG(LogLevel::FATAL) << "创建套接字失败 "<<sock;

    std::string serverip=argv[1];
    uint16_t serverport=std::stoi(argv[2]);
    
    InetAddr local(serverip,serverport);
    int n=connect(sock,local.NetAddrPtr(),local.SockLen());
    if(n<0)
    {
        LOG(LogLevel::INFO)<<"连接失败";
        exit(CONNECT_ERR);
    }

    while(true)
    {
        std::string line;
        std::cout<<"请按照我的想法输入"<<std::endl;
        std::getline(std::cin,line);
        ssize_t n=write(sock,line.c_str(),line.size());
        if(n<0)
        {
            LOG(LogLevel::INFO)<<"读取出错";
            exit(WRITE_ERR);
        }

        char buff[1024];
        ssize_t r =read(sock,buff,sizeof(buff)-1);
        if(r<0)
        {
            buff[r]=0;
            std::cout<<" server echo # "<<buff;
        }
    }

    close(sock);
    return 0;
}