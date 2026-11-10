#pragma once
#include <sys/epoll.h>

#include "Log.hpp"
#include "Common.hpp"

using namespace LogMoudle;
class Epoller
{
public:
    Epoller() : _epfd(-1)
    {
        int n = epoll_create1(128);
        if (n < 0)
        {
            LOG(LogLevel::ERROR) << "epoll_create1 error";
            exit(EPOLL_CREATE_ERROR);
        }

        LOG(LogLevel::ERROR) << "epoll_create1 success";
    }

    void ModEventHelper(int sockfd, uint32_t event, int oper)
    {
        struct epoll_event ev;
        ev.data.fd = sockfd;
        ev.events = event;
        int n = epoll_ctl(_epfd, oper, sockfd, &ev);
        if (n < 0)
        {
            LOG(LogLevel::ERROR) << sockfd << " epoll_ctl fiale ";
        }

        LOG(LogLevel::ERROR) << sockfd << " epoll_ctl success ";
    }

    void AddEvent(int sockfd,uint32_t event)
    {
        ModEventHelper(sockfd,event,EPOLL_CTL_ADD);
    }

    void DelEvent(int sockfd)
    {
        int n = epoll_ctl(_epfd, EPOLL_CTL_DEL, sockfd, nullptr);
                if (n < 0)
        {
            LOG(LogLevel::ERROR) << sockfd << " epoll_delete fiale ";
        }

        LOG(LogLevel::ERROR) << sockfd << " epoll_delete success ";
    }

    void MouEvent(int sockfd,uint32_t event)
    {
        ModEventHelper(sockfd,event,EPOLL_CTL_MOD);
    }

    int WaitEvent(struct epoll_event *res,int maxevent,int timeout)
    {
        int n=epoll_wait(_epfd,res,maxevent,timeout);
        if(n<0)
        {
            LOG(LogLevel::ERROR)<<"epoll_wait failed";
        }

        else if(n==0)
        {
            LOG(LogLevel::INFO)<<"timeout ";
        }

        else
        {
            //TODO
        }
        return n;
    }

private:
    int _epfd;
};