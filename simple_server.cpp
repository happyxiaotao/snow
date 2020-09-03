#include "net/Socket.h"
#include "EventLoop.h"

CEventLoop loop;

void OnRead(int fd, int event, void *arg)
{
    char buf[100];
    memset(buf, 0, sizeof(buf));
    int n = read(fd, buf, sizeof(buf) - 1);
    if (n == 0)
    {
        close(fd);
        loop.DelFileEvent(fd);
        return;
    }
    buf[sizeof(buf) - 1] = '\0';
    static int nId = 0;
    sscanf(buf, "%d", &nId);
    printf("read data from client:%d\n", nId);
    nId++;
    memset(buf, 0, sizeof(buf));
    sprintf(buf, "%d", nId);
    send(fd, buf, sizeof(buf), 0);
    printf("send to client:%d\n", nId);
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