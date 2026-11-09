#pragma once
#include <iostream>
#include <cstdint>
#include <string>
#include <functional>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <cstring>
#include <netinet/in.h>


enum EXIT_CODE
{
    OK,
    SUCC,
    SOCK_ERR,
    BIND_ERR,
    LISTEN_ERR,
    USGE_ERR,
    CONNECT_ERR,
    WRITE_ERR,
    FORK_ERR
};

class noCopy
{
public:
    noCopy() {}
    ~noCopy() {}
    void operator=(noCopy nocopy) {}
    noCopy(const noCopy &nocopy) {}
};

#define CONV(addr) ((struct sockaddr *)&addr)