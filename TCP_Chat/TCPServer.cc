#include "Common.hpp"
#include "TCPServer.hpp"
#include "Dic.hpp"

void USGE(std::string mes)
{
    std::cout << "USGE " << mes << "port" << std::endl;
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        USGE(argv[0]);
        exit(USGE_ERR);
    }

    Enable_Console_Log_Strategy();
    Dict dic;
    dic.LoadDict();

    uint16_t port = std::stoi(argv[1]);
    TCPServer ts(port,[&dic](const std::string &word, InetAddr &addr){
        return dic.Translate(word, addr);}
    );

    ts.Init();
    ts.Run();
    return 0;
}