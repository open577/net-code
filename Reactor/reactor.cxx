#include "Log.hpp"

#include "Protocol.hpp"
#include "NetCal.hpp"
#include "Reactor.hpp"

#include "Listener.hpp"

#include <string>

using namespace LogMoudle;
static void Usage(std::string proc)
{
    std::cerr << "Usage: " << proc << " port" << std::endl;
}

int main(int argc, char *argv[])
{

    if (argc != 2)
    {
        Usage(argv[0]);
        exit(USGE_ERR);
    }

    uint16_t port=std::stoi(argv[1]);
    std::shared_ptr<Cal> cal = std::make_shared<Cal>();

    std::unique_ptr<Protocol> proto = std::make_unique<Protocol>([&cal](Request &res) -> Response
                                                                 { return cal->Enque(res); });
    
    std::shared_ptr<Connaction> con = std::make_shared<Listener>(port);
    con->RegisterFunc([&proto](std::string &inbuffer)->std::string{
        LOG(LogLevel::DEBUG)<<"进入匿名函数模块";
        std::string response_str;
        while(true)
        {
        std::string package;

            if(proto->DeCode(inbuffer,&package))
                return "";
            
            response_str+=package;
        }

        LOG(LogLevel::DEBUG)<<"匿名函数模块结束";
        return response_str;
    });

    std::unique_ptr<Reactor> reactor=std::make_unique<Reactor>();

    reactor->AddConnection(con);
    reactor->Loop();
    return 0;
}