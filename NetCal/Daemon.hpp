#pragma once
#include "Log.hpp"
#include "Common.hpp"
#include <iostream>
#include <string>
#include <unistd.h>
#include <fcntl.h>     // open()、O_RDWR 宏！【你报错的这个宏就在这里】
#include <sys/types.h> // pid_t 类型
#include <sys/stat.h>  // open配套头文件
#include <cstdlib>     // exit
#include <cstdio>

using namespace LogMoudle;
const static std::string repash = "/dev/null";
void Daemon(int nochdir, int noclose)
{
    signal(SIGPIPE, SIG_IGN);
    signal(SIGCHLD, SIG_IGN);

    if (fork() > 0) // 父进程退出，让子进程来完成任务
    {
        exit(0);
    }

    // 子进程执行的任务
    if (nochdir == 0)
    {
        chdir("/");
    }

    if (noclose == 0)
    {
        int n = ::open(repash.c_str(), O_RDWR);
        {
            if (n < 0)
            {
                LOG(LogLevel::FATAL) << "文件" << repash << "打开失败";
                exit(OPEN_ERR);
            }

            else
            {
                dup2(n,0);
                dup2(n,0);
                dup2(n,0);
                close(n);
            }
        }
    }
}