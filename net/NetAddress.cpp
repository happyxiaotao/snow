
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "NetAddress.h"

CNetAddress::CNetAddress(unsigned short family, const char *ip, unsigned short port) : m_family(family)
{
    if (m_family == AF_INET)
    {
        memset(&m_addr, 0, sizeof(struct sockaddr_in));
        m_addr.sin_family = m_family;
        m_addr.sin_port = htons(port);
        inet_pton(AF_INET, ip, &m_addr.sin_addr);
    }
    else if (m_family == AF_INET6)
    {
        memset(&m_addr6, 0, sizeof(struct sockaddr_in6));
        m_addr6.sin6_family = m_family;
        m_addr6.sin6_port = htons(port);
        inet_pton(AF_INET6, ip, &m_addr6.sin6_addr);
    }
    else
    {
        fprintf(stderr, "m_family unknown");
        exit(-1);
    }
}
CNetAddress::~CNetAddress()
{
}

struct sockaddr *CNetAddress::GetAddrPtr()
{
    if (m_family == AF_INET)
        return (struct sockaddr *)&m_addr;
    return (struct sockaddr *)(void *)&m_addr6; // 默认ipv6
}
const struct sockaddr *CNetAddress::GetAddrPtr() const
{
    if (m_family == AF_INET)
        return (struct sockaddr *)&m_addr;
    return (struct sockaddr *)(void *)&m_addr6; // 默认ipv6
}

socklen_t CNetAddress::GetAddrLen()
{
    if (m_family == AF_INET)
        return sizeof(m_addr);
    return sizeof(m_addr6); // 默认ipv4
}
socklen_t CNetAddress::GetAddrLen() const
{
    if (m_family == AF_INET)
        return sizeof(m_addr);
    return sizeof(m_addr6); // 默认ipv4
}