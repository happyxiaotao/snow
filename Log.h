/*
��־ϵͳ����ƣ�
1���ؼ�������ֵ
2�����е�λ�ã��ĸ��ļ�����һ�У�
3��ʱ��
4���̺߳ţ����̺�
*/
#if 0
#pragma once

#include <string>
#include <list>
#include <fstream>
#include <chrono>
#include <stdarg.h>
#include <string.h>
#include "Singleton.hpp"

// ��־�Ǽ�
enum class LOG_LEVEL :int
{
	LOG_NONE = 1,
	LOG_DEBUG = 2,
	LOG_INFO = 3,
	LOG_WARNING = 4,
	LOG_ERROR = 5,
};
// ��־���λ��
enum LOG_TARGET
{
	LOG_TARGET_NONE = 0x00,
	LOG_TARGET_CONSOLE = 0x01,
	LOG_TARGET_FILE = 0x02,
};

#define LOG_INFO(...) CSingleton<CLog>::GetInstance()->Log()
#define LOG_WARN(...) CLog::IN

class CLog
{
	friend class CSingleton<CLog>;
public:
	void Init(LOG_LEVEL logLevel, LOG_TARGET logTarget, const char *pFileName)
	{
		SetLogLevel(logLevel);
		SetLogTarget(logTarget);
		CreateFile(pFileName);
	}
	static int WriteLog(
		LOG_LEVEL loglevel,		// log�ȼ�
		char* filename,	// ���������ļ���
		char* function,	// ������
		int line,		// �к�
		char* format,	// ��ʽ������
		...)			// ��������
	{
		int nRet = 0;
		struct timeval tv;
		gettimeofday();
		localtime(&tv);
		
		// ��־����
		char szLog[256];
		memset(szLog, 0, sizeof(szLog));
		va_list ap;
		va_start(ap, format);
		nRet = vsnprintf(szLog, sizeof(szLog), format, ap);
		va_end(ap);

		OutputToTarget();

		return nRet;
	}

	// ���log
	static void OutputToTarget();
	
	void SetLogLevel(LOG_LEVEL logLevel)
	{
		m_logLevel = logLevel;
	}
	void SetLogTarget(LOG_TARGET logTarget)
	{
		m_logTarget = logTarget;
	}
	void CreateFile(const char* pFileName)
	{
		m_file.open(pFileName, std::ios::binary);
	}
private:
	CLog() = default;
	~CLog() = default;
	CLog(CLog&) = delete;
	CLog(CLog&&) = delete;
	CLog& operator=(CLog&) = delete;
private:

	// log�ȼ�
	LOG_LEVEL m_logLevel;
	// log���λ��
	LOG_TARGET m_logTarget;
	// ��־�ļ�
	static std::ifstream m_file;
	// �洢log��buffer
	static std::list<std::string> m_logList;
};

#endif