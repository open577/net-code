#pragma once
#include "Log.hpp"
#include <iostream>
#include <fstream>
#include <unordered_map>
#include <string>
#include "InetAddr.hpp"

using namespace LogMoudle;
const std::string dicstep = ": ";
const std::string defaultdicfile = "./dictionary.txt";

class Dict
{
public:
    Dict(const std::string path = defaultdicfile)
    {
        _path = path;
    }

    bool LoadDict()
    {
        std::ifstream in(_path);
        if (!in.is_open())
        {
            LOG(LogLevel::INFO) << "打开文件失败";
            return false;
        }

        std::string input;
        while (getline(in, input))
        {
            auto pos = input.find(dicstep);
            if (pos == std::string::npos)
            {
                LOG(LogLevel::INFO) << "解析失败";
                continue;
            }

            std::string english = input.substr(0, pos);
            std::string chinese = input.substr(pos +dicstep.size());

            if (english.empty() || chinese.empty())
            {
                LOG(LogLevel::INFO) << "解析失败";
                continue;
            }

            _dir.insert(std::make_pair(english, chinese));
            LOG(LogLevel::DEBUG) << "加载成功";
            in.close();
        }
        return true;
    }

    std::string Translate(const std::string word, InetAddr &client)
    {
        auto pos = _dir.find(word);
        if (pos == _dir.end())
        {
            LOG(LogLevel::INFO) << "查找错误" << "[" << client.GetIP() << " " << client.GetPort() << " " << word << "->" << "None";
            return "None";
        }

        LOG(LogLevel::INFO) << "查找成功" << "[" << client.GetIP() << " " << client.GetPort() << " " << word << "->" <<pos->second;
        return pos->second;
    }

private:
    std::string _path;
    std::unordered_map<std::string, std::string> _dir;
};