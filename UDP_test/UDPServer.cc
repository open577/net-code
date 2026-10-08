#include "UDPServer.hpp"
#include <memory>

std::string defaultheader(const std::string mes)
{
    std::string result = "hello ,";
    result += mes;
    return result;
}

//   ./server port  两个参数
int main(int argc,char *argv[])
{
    if(argc!=2)
    {
        std::cerr<<"输入无效 ./server port"<<std::endl;
        return 1;
    }

    int port=std::stoi(argv[1]);
    std::unique_ptr<Udpserver> mm =std::make_unique<Udpserver>(port,defaultheader);
    mm->Init();
    mm->Start();
    return 0;
}