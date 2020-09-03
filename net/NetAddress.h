#pragma once

#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>

class CNetAddress
{
public:
    CNetAddress(unsigned short family = AF_INET, const char *ip = "127.0.0.1", unsigned short port = 8008);
    ~CNetAddress();

public:
    struct sockaddr *GetAddrPtr();
    const struct sockaddr *GetAddrPtr() const;

    socklen_t GetAddrLen();
    socklen_t GetAddrLen() const;

private:
    unsigned short m_family;
    union
    {
        struct sockaddr_in m_addr;
        struct sockaddr_in6 m_addr6;
    };
};
