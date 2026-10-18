#pragma once

#include "Log.hpp"
#include "TCPServer.hpp"
#include "Socket.hpp"
#include "InetAddr.hpp"
#include <string>
#include <sstream>
using namespace SocketModule;
using namespace LogMoudle;

const std::string webroot = "./wwwroot";
const std::string homepage = "index.html";
const std::string space = " ";
const std::string httpstep = "\r\n";
const std::string separation = ": ";
class HttpRequest
{
public:
    HttpRequest() {}
    ~HttpRequest() {}

    void RequestOneLine(std::string &requeststr, std::string *line)
    {
        auto pos = requeststr.find(httpstep);
        if (pos == std::string::npos)
        {
            return;
        }

        std::string paseline = requeststr.substr(0, pos);
        requeststr.erase(0, paseline.size() + httpstep.size());
        *line = paseline;
    }

    void PraseRequesrLine(std::string &line)
    {
        std::stringstream ss(line);
        ss >> _req_methord >> _uri >> _http_version;
    }
    bool DeSerialize(std::string &requeststr)
    {
        std::string request_linel;
        RequestOneLine(requeststr, &request_linel);
        PraseRequesrLine(request_linel);
        LOG(LogLevel::INFO) << _req_methord;
        LOG(LogLevel::INFO) << _uri;
        LOG(LogLevel::INFO) << _http_version;
        if (_uri == "/")
        {
            _uri = webroot + "/" + homepage;
        }

        else
        {
            _uri = webroot + _uri;
        }

        return true;
    }

    std::string URI()
    {
        return _uri;
    }

    std::string Version()
    {
        return _http_version;
    }

private:
    std::string _req_methord;
    std::string _uri;
    std::string _http_version;
    std::unordered_map<std::string, std::string> _req_package;
    std::string _req_text;
};

class HttpResponse
{
public:
    HttpResponse(std::string version, std::string state_code, std::string state_des, std::string &webstring)
        : _http_version(version), _state_code(state_code), _state_des(state_des), _res_text(webstring)
    {
    }
    ~HttpResponse() {}

    std::string Serialize()
    {

        std::string statu_line = _http_version + space + _state_code + space + _state_des + httpstep;

        std::string res_package;
        for (auto &oneline : _res_package)
        {
            res_package += oneline.first + separation + oneline.second + httpstep;
        }

        return statu_line + res_package + httpstep + _res_text;
    }

public:
    std::string _http_version;
    std::string _state_code;
    std::string _state_des;
    std::unordered_map<std::string, std::string> _res_package;
    std::string _res_text;
};

class Http
{
public:
    Http(uint16_t port) : _tcps(std::make_unique<TCPServer>(port))
    {
    }

    bool OpenFile(std::string filepash, std::string &webstring)
    {
        std::ifstream in(filepash);
        if (!in.is_open())
        {
            return false;
        }

        else
        {
            std::string line;
            while (getline(in, line))
            {
                webstring += line + httpstep;
            }
        }
        return true;
    }
    void GetWeb(std::string filepash, std::string &webstring)
    {
        OpenFile(filepash, webstring);
    }
    void HttpRequest_Response(std::shared_ptr<Socket> &socket, InetAddr &client)
    {
        std::string request;
        socket->Recv(&request);

        HttpRequest req;
        req.DeSerialize(request);

        std::string webstring;
        std::string filepash = req.URI();
        GetWeb(filepash, webstring);
        HttpResponse res(req.Version(), "200", "OK", webstring);

        std::string resstr = res.Serialize();

        socket->Send(resstr);
    }

    void Start()
    {
        _tcps->Start([this](std::shared_ptr<Socket> &sock, InetAddr &client)
                     { this->HttpRequest_Response(sock, client); });
    }

private:
    std::unique_ptr<TCPServer> _tcps;
};