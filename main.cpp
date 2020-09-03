/*************************************************************************
	> File Name: main.cpp
	> Author: xsfh
	> Note:
	> Created Time: Tue 01 Sep 2020 10:42:22 AM PDT
 ************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "net/Socket.h"

int main()
{
	NetAddress address(AF_INET, "127.0.0.1", 8080);
	CSocket socket(AF_INET, SOCK_STREAM, 0);
	socket.Bind(address, true);
	int ret = socket.Listen(100);

	for (;;)
	{
		int fd = socket.Accept(address);	// 当监听socket设置为非阻塞的时候，通过for循环中accept的方法就不好，会一直报错accept失败，资源不可用
		if (fd != -1)
		{
			char buf[1024];
			memset(buf, 0, sizeof(buf));
			read(fd, buf, sizeof(buf));
			printf("get data:%s\n", buf);
			write(fd, "nihao", 5);
			close(fd);
		}
	}

	return 0;
}
