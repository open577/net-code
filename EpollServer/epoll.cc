#include "EpollServer.hpp"

void ERROR()
{
    LOG(LogLevel::ERROR)<<"输入无效 IP+Port";
}
int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        ERROR();
    }

    int port=std::stoi(argv[1]);
    std::unique_ptr<EpollServer> sel=std::make_unique<EpollServer>(port);
    sel->Start();
    return 0;
}