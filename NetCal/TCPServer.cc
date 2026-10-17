#include "NetCal.hpp"
#include "Protocol.hpp"
#include "TCPServer.hpp"
#include "Daemon.hpp"
#include <memory>

void Usage(std::string proc)
{
    std::cerr << "Usage: " << proc << " port" << std::endl;
}

// ./server port
int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        Usage(argv[0]);
        exit(USGE_ERR);
    }

    Daemon(0, 0);
    std::cout << "服务器已启动 现在已经是一个守护进行了" << std::endl;

    // 顶层请求
    std::unique_ptr<Cal> cal = std::make_unique<Cal>();

    // 协议层
    std::unique_ptr<Protocol> protol = std::make_unique<Protocol>([&cal](Request &req) -> Response
                                                                  { return cal->Enque(req); });

    std::unique_ptr<TCPServer> tcps = std::make_unique<TCPServer>(std::stoi(argv[1]), [&protol](std::shared_ptr<Socket> &sock, InetAddr &client) {
        protol->GetRequest(sock,client);
    });

    tcps->Start();
    return 0;
}
