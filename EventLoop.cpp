#include "EventLoop.h"

int Epoll::AddEvent(int nFd, int nMask, bool bEt)
{
	struct epoll_event ev;
	ev.data.fd = nFd;
	ev.events = 0;
	int nOp = m_pEventLoop->m_mapFile[nFd].m_nMask == EV_NONE ? EPOLL_CTL_ADD : EPOLL_CTL_MOD;
	nMask |= m_pEventLoop->m_mapFile[nFd].m_nMask;
	if (nMask & EV_READABLE)
		ev.events |= EPOLLIN;
	if (nMask & EV_WRITEABLE)
		ev.events |= EPOLLOUT;
	if (epoll_ctl(m_nEpFd, nOp, nFd, &ev) == -1)
	{
		printf("epoll_ctl error:%s\n", strerror(errno));
		printf("m_nEpFd=%d\n", m_nEpFd);
		printf("noP=%d\n", nOp);
		printf("nFd=%d\n", nFd);
		printf("ev.data.fd=%d\n", ev.data.fd);
		printf("ev.events=%d\n", ev.events);

		return -1;
	}
	return 0;
}

int Epoll::DelEvent(int nFd, int nDelMask, bool bEt)
{
	struct epoll_event ev;
	ev.data.fd = nFd;
	ev.events = 0;
	int nMask = m_pEventLoop->m_mapFile[nFd].m_nMask & (~nDelMask);
	int nOp = nMask == EV_NONE ? EPOLL_CTL_DEL : EPOLL_CTL_MOD;
	if (nMask & EV_READABLE)
		ev.events |= EPOLLIN;
	if (nMask & EV_WRITEABLE)
		ev.events |= EPOLLOUT;
	if (epoll_ctl(m_nEpFd, nOp, nFd, &ev) == -1)
		return -1;
	return 0;
}
// ��ѯ
// ���nTimeout����Ϊ0���ᵼ��epoll_wait�������أ�ռ�ô�����CPU
// nTimeout����Ϊ-1����ʾһֱ��������δ����
int Epoll::Poll(int nTimeout)
{
	int num = epoll_wait(m_nEpFd, &m_vecEv[0], static_cast<int>(m_vecEv.size()), nTimeout);
	for (int i = 0; i < num; i++)
	{
		int nMask = EV_NONE;
		struct epoll_event &ev = m_vecEv[i];
		if (ev.events & EPOLLIN)
			nMask |= EV_READABLE;
		if (ev.events & EPOLLOUT)
			nMask |= EV_WRITEABLE;
		if (ev.events & EPOLLERR || ev.events & EPOLLHUP)
			nMask |= EV_WRITEABLE;
		m_pEventLoop->m_vecFired[i].m_nFd = ev.data.fd;
		m_pEventLoop->m_vecFired[i].m_nMask = nMask;
	}
	// ���epoll_wait���صľ��������ļ����������Ѿ��������ˣ�����������Ӧ�¼��Ľ��ո���
	if (num > 0 && num == m_vecEv.size())
	{
		int nNewSize = (num * 2 > EPOLL_MAX_NUM) ? EPOLL_MAX_NUM : num * 2;
		struct epoll_event tmp;
		tmp.data.fd = -1;
		tmp.events = 0;
		m_vecEv.resize(nNewSize, tmp);

		m_pEventLoop->m_vecFired.resize(nNewSize); // ����ҪĬ�ϵģ���ΪFileFired�ṹ����Ĭ�ϵ�ֵ
	}
	return num;
}

int Epoll::Init(CEventLoop *pEventLoop)
{
	if ((m_nEpFd = epoll_create(1024)) == -1)
	{
		printf("epoll_create error:%s\n", strerror(errno));
		return -1;
	}
	printf("m_nEpFd=%d\n", m_nEpFd);
	struct epoll_event tmp;
	tmp.data.fd = -1;
	tmp.events = 0;
	m_vecEv.resize(EPOLL_INIT_NUM, tmp);

	m_pEventLoop = pEventLoop;
}
void Epoll::Clear()
{
	if (m_nEpFd != -1)
	{
		close(m_nEpFd);
		m_nEpFd = -1;
	}
}

int CEventLoop::AddFileEvent(int nFd, int nMask, FileProc cbProc, void *pArg)
{
	if (m_epoll.AddEvent(nFd, nMask) < 0)
		return -1;
	FileEvent &file = m_mapFile[nFd];
	file.m_nMask |= nMask;
	if (file.m_nMask & EV_READABLE)
	{
		file.m_cbReadProc = cbProc;
		file.m_pReadArg = pArg;
	}
	if (file.m_nMask & EV_WRITEABLE)
	{
		file.m_cbWriteProc = cbProc;
		file.m_pWriteArg = pArg;
	}
	return 0;
}
int CEventLoop::DelFileEvent(int nFd, int nDelMask)
{
	FileEvent &file = m_mapFile[nFd];
	if (file.m_nMask == EV_NONE)
		return 0;
	if (m_epoll.DelEvent(nFd, nDelMask) < 0)
		return -1;
	file.m_nMask &= ~nDelMask;
	return 0;
}
uint64_t CEventLoop::Dispatch(int nTimeout)
{
	uint64_t nProcessed = 0;
	while (true)
	{
		int num = m_epoll.Poll(nTimeout);
		//if (num < 0)
		//	return 0;
		for (int i = 0; i < num; i++)
		{
			int nMask = m_vecFired[i].m_nMask;
			int nFd = m_vecFired[i].m_nFd;
			FileEvent &file = m_mapFile[nFd];
			bool bFired = false;

			if (file.m_nMask & nMask & EV_READABLE)
			{
				file.m_cbReadProc(nFd, nMask, file.m_pReadArg);
				bFired = true;
			}
			if (!bFired && file.m_nMask & nMask & EV_WRITEABLE)
			{
				file.m_cbWriteProc(nFd, nMask, file.m_pWriteArg);
				bFired = true;
			}
			nProcessed++;
		}
	}
	return nProcessed;
}
