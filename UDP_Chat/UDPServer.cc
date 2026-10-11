#include <functional>
#include "UDPServer.hpp"
#include "Route.hpp"
#include <memory>
#include "ThreadPool.hpp"
using namespace ThreadPoolMoudle;

std::string defaultheader(const std::string mes)
{
    std::string result = "hello ,";
    result += mes;
    return result;
}
using task_t = std::function<void()>;
//   ./server port  两个参数
int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        std::cerr << "输入无效 ./server port" << std::endl;
        return 1;
    }

    int port = std::stoi(argv[1]);
    Enable_Console_Log_Strategy();
    auto tp = ThreadPool<task_t>::GetInstance();
    // std::unique_ptr<Udpserver> mm =std::make_unique<Udpserver>(port,defaultheader);
    // mm->Init();
    // mm->Start();

    Route r;
    // std::unique_ptr<Udpserver> usvr = std::make_unique<Udpserver>(
    //     port, [&r,tp](int sockfd, const std::string &message, InetAddr &peer)
    //     { r.MessageRoute(sockfd, message, peer);
    //     tp->Enqueue()});

    std::unique_ptr<Udpserver> usvr = std::make_unique<Udpserver>(port, [&r, &tp](int sockfd, const std::string &message, InetAddr &peer)
                                                                  {
        task_t t = std::bind(&Route::MessageRoute, &r, sockfd, message, peer);
        tp->Enqueue(t); });

    usvr->Init();
    usvr->Start();

    return 0;
}