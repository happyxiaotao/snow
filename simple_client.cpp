#include "net/Socket.h"
#include "EventLoop.h"

CEventLoop loop;

void OnWrite(int fd, int mask, void *arg);

void OnRead(int fd, int mask, void *arg)
{
    char szbuf[1024];
    memset(szbuf, 0, sizeof(szbuf));
    int n = read(fd, szbuf, sizeof(szbuf));
    if (n == 0)
    {
        close(fd);
        loop.DelFileEvent(fd);
        return;
    }
    szbuf[sizeof(szbuf) - 1] = '\0';
    printf("get data from server:%s\n", szbuf);

    loop.DelFileEvent(fd, EV_READABLE);
    loop.AddFileEvent(fd, EV_WRITEABLE, OnWrite, NULL);
}

void OnWrite(int fd, int mask, void *arg)
{
    static int nId = 0;
    nId++;
    char szBuf[100];
    memset(szBuf, 0, sizeof(szBuf));
    sprintf(szBuf, "%d", nId);
    szBuf[sizeof(szBuf) - 1] = '\0';
    write(fd, szBuf, sizeof(szBuf));
    printf("send data to server:%s\n", szBuf);

    loop.DelFileEvent(fd, EV_WRITEABLE);
    loop.AddFileEvent(fd, EV_READABLE, OnRead, NULL);
}

int main()
{
    CSocket socket(AF_INET, SOCK_STREAM, 0);
    CNetAddress addr(AF_INET, "127.0.0.1", 8080);
    socket.Connect(addr);
    loop.AddFileEvent(socket.GetSocket(), EV_WRITEABLE, OnWrite, &socket); // 服务端可写
    loop.Dispatch();
}