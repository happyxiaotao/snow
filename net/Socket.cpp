#include "Socket.h"

#include <cerrno>
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string.h>
#include "NetAddress.h"

int CSocket::CreateSocket(int domain, int type, int protocol)
{
    int fd = socket(domain, type, protocol);
    if (fd == -1)
    {
        fprintf(stderr, "create socket failed! socket:%d, error:%s\n", m_socket, strerror(errno));
        return -1;
    }
    return fd;
}
void CSocket::CloseSocket()
{
    if (m_socket != -1)
        close(m_socket);
    m_state = STATE::CLOSED;
}
int CSocket::GetSocket()
{
    return m_socket;
}
int CSocket::Bind(const CNetAddress &addr, bool bReuseAddr)
{
    if (bReuseAddr && (SetReuseAddr() == -1))
        return -1;
    if (bind(m_socket, addr.GetAddrPtr(), addr.GetAddrLen()) == -1)
    {
        fprintf(stderr, "bind socket failed! socket:%d, error:%s\n", m_socket, strerror(errno));
        return -1;
    }
    return 0;
}
int CSocket::Listen(int backlog)
{
    if (listen(m_socket, backlog) == -1)
    {
        fprintf(stderr, "listen socket failed! socket:%d, error:%s\n", m_socket, strerror(errno));
        return -1;
    }
    return 0;
}
int CSocket::Connect(const CNetAddress &addr)
{
    if (connect(m_socket, addr.GetAddrPtr(), addr.GetAddrLen()) == -1)
    {
        if (errno == EINPROGRESS)
            return 0;
        fprintf(stderr, "connect socket failed! socket:%d, error:%s\n", m_socket, strerror(errno));
        return -1;
    }
    return 0;
}
int CSocket::Accept(const CNetAddress &addr, bool bNonBlock)
{
    socklen_t len = addr.GetAddrLen();
    int fd = accept(m_socket, const_cast<sockaddr *>(addr.GetAddrPtr()), &len);
    if (fd == -1)
    {
        fprintf(stderr, "accept socket failed! listen socket:%d, error%s\n", m_socket, strerror(errno));
        return -1;
    }
    if (bNonBlock && (-1 == SetNonBlock()))
        return -1;
    return fd;
}

// 设置socket为非阻塞
int CSocket::SetNonBlock()
{
    int flag = fcntl(m_socket, F_GETFL);
    flag |= O_NONBLOCK;
    if (fcntl(m_socket, F_SETFL, flag) == -1)
    {
        fprintf(stderr, "fcntl(m_socket, F_SETFL, flag) return -1, m_socket:%d, flag:%d\n", m_socket, flag);
        return -1;
    }
    return 0;
}

int CSocket::SetReuseAddr()
{
    int bReuseaddr = 1;
    if (setsockopt(m_socket, SOL_SOCKET, SO_REUSEADDR, &bReuseaddr, sizeof(bReuseaddr)) == -1)
    {
        fprintf(stderr, "setsockopt SO_REUSEADDR failed, m_socket:%d\n", m_socket);
        return -1;
    }
    return 0;
}
