#pragma once
#include <string>
#include <functional>
#include <jsoncpp/json/json.h>

#include "InetAddr.hpp"
#include "Socket.hpp"

using namespace SocketModule;
class Request
{
public:
    Request() {}
    Request(int x, int y, char oper) : _x(x), _y(y), _oper(oper) {}

    std::string Serialize()
    {
        Json::Value root;
        root["x"] = _x;
        root["y"] = _y;
        root["oper"] = _oper;
        Json::FastWriter writer;
        std::string s = writer.write(root);
        return s;
    }

    bool Deserialize(std::string &in)
    {
        Json::Value root;
        Json::Reader reader;

        bool ok = reader.parse(in, root);
        if (ok)
        {
            _x = root["x"].asInt();
            _y = root["y"].asInt();
            _oper = root["oper"].asInt();
            return ok;
        }
        return false;
    }

    ~Request() {}

    int GetX()
    {
        return _x;
    }

    int GetY()
    {
        return _y;
    }

    char GetOper()
    {
        return _oper;
    }

private:
    int _x;
    int _y;
    char _oper;
};

class Response
{
public:
    Response() {}
    Response(int result, int code) : _result(result), _code(code)
    {
    }

    std::string Serialize()
    {
        Json::Value root;
        root["result"] = _result;
        root["code"] = _code;
        Json::FastWriter writer;
        std::string s = writer.write(root);
        return s;
    }

    bool Deserialize(std::string &in)
    {
        Json::Value root;
        Json::Reader reader;

        bool ok = reader.parse(in, root);
        if (ok)
        {
            _result = root["result"].asInt();
            _code = root["code"].asInt();
            return ok;
        }
        return false;
    }

    void SetResult(int result)
    {
        _result = result;
    }
    void SetCode(int code)
    {
        _code = code;
    }
    void ShowResult()
    {
        std::cout << "计算结果是: " << _result << "[" << _code << "]" << std::endl;
    }
    ~Response() {}

private:
    int _result;
    int _code;
};

// const std::string step="\r\n";
using func_t = std::function<Response(Request &res)>;
class Protocol
{
public:
    Protocol(){}
    Protocol(func_t func) : _func(func) {}

    std::string EnCode(const std::string &jsonstr)
    {
        std::string len = std::to_string(jsonstr.size());
        return len + step + jsonstr + step;
    }

    bool DeCode(std::string &buffer, std::string *package)
    {
        ssize_t pos = buffer.find(step);
        if (pos == std::string::npos)
            return false;

        std::string package_len_str = buffer.substr(0, pos);
        int package_len_int = std::stoi(package_len_str);
        int target_len = package_len_str.size() + package_len_int + 2 * step.size();

        if (buffer.size() < target_len)
        {
            return false;
        }

        *package = buffer.substr(static_cast<size_t>(pos) + step.size(), static_cast<size_t>(package_len_int));

        buffer.erase(0);
        return true;
    }
    std::string BuildRequestString(int x, int y, char oper)
    {
        // 1. 构建一个完整的请求
        Request req(x, y, oper);

        // 2. 序列化
        std::string json_req = req.Serialize();

        // 3. 添加长度报头
        return EnCode(json_req);
    }
    void GetRequest(std::shared_ptr<Socket> &sock, InetAddr &client)
    {
        std::string buffer;
        while (true)
        {
            int n = sock->Recv(&buffer);
            if (n > 0)
            {
                std::string package;
                bool res = DeCode(buffer, &package);
                if (!res)
                {
                    continue;
                }

                Request req;
                bool ret = req.Deserialize(package);
                if (!ret)
                    continue;

                Response resp = _func(req);

                // 4. 序列化
                std::string json_str = resp.Serialize();

                // 5. 添加自定义长度
                std::string send_str = EnCode(json_str); // 携带长度的应答报文了"len\r\n{result:XXX, code:XX}\r\n"

                // 6. 直接发送
                sock->Send(send_str);
            }
            else if (n == 0)
            {
                LOG(LogLevel::INFO) << "client:" << client.FullName() << "Quit!";
                break;
            }
            else
            {
                LOG(LogLevel::WARNING) << "client:" << client.FullName() << ", recv error";
                break;
            }
        }
    }

    bool GetResponse(std::shared_ptr<Socket> &client, std::string &resp_buff, Response *resp)
    {
        while (true)
        {
            int n = client->Recv(&resp_buff);
            if (n > 0)
            {
                // 成功
                std::string json_package;
                // 1. 解析报文，提取完整的json请求，如果不完整，就让服务器继续读取
                // bool ret = Decode(resp_buff, &json_package);
                // if (!ret)
                //     continue;

                while (DeCode(resp_buff, &json_package))
                {
                    resp->Deserialize(json_package);
                }
                return true;
            }
            else if (n == 0)
            {
                std::cout << "server quit " << std::endl;
                return false;
            }
            else
            {
                std::cout << "recv error" << std::endl;
                return false;
            }
        }
    }

private:
    func_t _func;
};
