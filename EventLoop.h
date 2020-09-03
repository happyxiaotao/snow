#pragma once

#include <vector>
#include <map>
#include <sys/epoll.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#define EV_NONE (0x00)
#define EV_READABLE (0x01)
#define EV_WRITEABLE (0x02)

#define EPOLL_INIT_NUM (32)	 // epoll��ʼ������fd����
#define EPOLL_MAX_NUM (4096) // epoll������fd����

class CEventLoop;

typedef void (*FileProc)(int nFd, int nMask, void *pArg);

struct FileEvent
{
	FileEvent()
	{
		m_nMask = EV_NONE;
		m_cbReadProc = nullptr;
		m_cbWriteProc = nullptr;
		m_pReadArg = nullptr;
		m_pWriteArg = nullptr;
	}
	int m_nMask; // EV_NONE|EV_READABLE|EV_WRITEABLE
	FileProc m_cbReadProc;
	FileProc m_cbWriteProc;
	void *m_pReadArg;
	void *m_pWriteArg;
};

struct FileFired
{
	FileFired()
	{
		m_nFd = -1;
		m_nMask = EV_NONE;
	}
	int m_nFd;	 // fd
	int m_nMask; //
};

/*
* epoll�¼��Ĺ�������Ա�е�ǰ���ĸ�����epoll�¼������fd���Լ�struct epoll������Ӧ
* m_vecEvent�����ĸ�����Ӧ����epoll_wait���ظ����ﵽ��ǰvec���ֵ��ʱ�����ݣ�ֱ���ﵽָ�������ֵ����
* */
class Epoll
{
public:
	Epoll(CEventLoop *pEventLoop)
	{
		Init(pEventLoop);
	}
	~Epoll()
	{
		Clear();
	}

public:
	// ����ʱ�������bEt��ʾ�Ƿ�ʹ��ˮƽ��������ʱû�������ô�
	// nMask��ֵʱǰ�涨���EV_��ͷ�ĺ궨��
	int AddEvent(int nFd, int nMask, bool bEt = false);

	int DelEvent(int nFd, int nDelMask, bool bEt = false);
	// ��ѯ
	int Poll(int nTimeout);

	int Init(CEventLoop *pEventLoop);
	void Clear();

public:
	int m_nMaxSize;
	int m_nEpFd;
	std::vector<struct epoll_event> m_vecEv;
	CEventLoop *m_pEventLoop;
};

class CEventLoop
{
public:
	friend Epoll;

	CEventLoop() : m_epoll(this)
	{
		m_vecFired.resize(EPOLL_INIT_NUM);
	}
	~CEventLoop() = default;

public:
	int AddFileEvent(int nFd, int nMask, FileProc cbProc, void *pArg);
	int DelFileEvent(int nFd, int nDelMask = ~EV_NONE);
	uint64_t Dispatch(int nTimeout = -1); // ms

public:
	int m_nMaxSize; // ���fd
	std::map<int, FileEvent> m_mapFile;
	std::vector<FileFired> m_vecFired;
	Epoll m_epoll;
};
