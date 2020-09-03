
#include "NetAddress.h"

class CSocket
{
public:
    enum STATE
    {
        OPENED,
        CLOSED,
    };

public:
    CSocket(int domain, int type, int protocol) : m_socket(-1), m_state(STATE::CLOSED)
    {
        if ((m_socket = CreateSocket(domain, type, protocol)) != -1)
            m_state = STATE::OPENED;
    }
    ~CSocket()
    {
        CloseSocket();
    }

public:
    int CreateSocket(int domain, int type, int protocol);
    void CloseSocket();
    int GetSocket();
    int Bind(const CNetAddress &addr, bool bReuseAddr = true); // 默认是可复用的
    int Listen(int backlog);
    int Connect(const CNetAddress &addr);
    int Accept(const CNetAddress &addr, bool bNonBlcok = true); // 默认设置非阻塞的

public:
    // 设置socket为非阻塞
    int SetNonBlock();
    // 设置socket复用
    int SetReuseAddr();

private:
    int m_socket;
    enum STATE m_state;
};
