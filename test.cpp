#include "net/Socket.h"
#include "EventLoop.h"

CEventLoop loop;

void OnRead(int fd, int event, void *arg)
{
    char buf[5];
    memset(buf, 0, sizeof(buf));
    int n = read(fd, buf, sizeof(buf) - 1);
    if (n == 0)
    {
        close(fd);
        return;
    }
    buf[sizeof(buf) - 1] = '\0';
    printf("read buf:%s\n", buf);
}

void OnAccept(int fd, int event, void *arg)
{
    CSocket *pSocket = (CSocket *)arg;

    // accept a new connect
    CNetAddress addr;
    int conn = pSocket->Accept(addr, true);
    loop.AddFileEvent(conn, EV_READABLE, OnRead, NULL);
}

int main()
{
    CNetAddress addr(AF_INET, "127.0.0.1", 8080);
    CSocket socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
    socket.Bind(addr);
    socket.Listen(100);

    loop.AddFileEvent(socket.GetSocket(), EV_READABLE, OnAccept, &socket);
    loop.Dispatch();
}