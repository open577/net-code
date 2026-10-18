#include "Http.hpp"



int main(int argc,char *argv[])
{
    if(argc!=2)
    {
        LOG(LogLevel::FATAL)<<"请输入正确格式"<<"./main.cc port";
        exit(USGE_ERR);
    }

    uint16_t port = std::stoi(argv[1]);

    std::unique_ptr<Http> httpsvr = std::make_unique<Http>(port);
    httpsvr->Start();
    return 0;
}