#pragma once

#include <string.h>

#define IOBUFFER_INIT_SIZE 0x20 // 32 bytes
//#define IOBUFFER_MAX_SIZE 0xffff	// 65535 bytes

class CBuffer
{
public:
	CBuffer(int nAlloc = IOBUFFER_INIT_SIZE)
	{
		char *pData = new char[nAlloc];
		memset(pData, 0, nAlloc);
		m_nAlloc = nAlloc;
		m_pData = pData;
		m_nSize = 0;
	}
	~CBuffer()
	{
		FreeMem();
		m_pData = nullptr;
	}

public:
	/* buffer�������	*/

	// ���ݳ���
	size_t Size()
	{
		return m_nSize;
	}
	// ��������
	bool Append(const char *pSrc, size_t nLen)
	{
		if (pSrc == nullptr || nLen == 0)
			return true;
		if (!IncreaseMem(nLen))
			return false;
		memcpy(m_pData + m_nSize, pSrc, nLen);
		m_nSize += nLen;
		return true;
	}
	// ��ȡ����
	const char *GetData()
	{
		return m_pData;
	}
	// �����п�
	bool Empty()
	{
		return m_nSize == 0;
	}
	// �������
	void Clear()
	{
		m_nSize = 0;
	}

	/* �ռ���ز��� */
	size_t MaxSize()
	{
		return m_nAlloc;
	}
	// ���ٿռ�
	bool IncreaseMem(size_t nAddLen)
	{
		if (m_nAlloc - m_nSize < nAddLen)
		{
			size_t nNewAlloc = m_nAlloc * 2;
			while (nNewAlloc - m_nSize < nAddLen)
				nNewAlloc *= 2;
			char *pData = new char[nNewAlloc];
			memcpy(pData, m_pData, m_nSize);
			delete[] m_pData;
			m_pData = pData;
			m_nAlloc = nNewAlloc;
		}
		return true;
	}
	//���ͷſռ�
	void FreeMem()
	{
		if (m_nAlloc > 0)
		{
			delete[] m_pData;
		}
		m_pData = nullptr;
		m_nAlloc = 0;
		m_nSize = 0;
	}
	// ʣ��ռ�
	size_t LeftMem()
	{
		return m_nAlloc - m_nSize;
	}

private:
	char *m_pData;	 // ָ��ռ��׵�ַ
	size_t m_nAlloc; // �ѿ��ٵĿռ�
	size_t m_nSize;	 // ��ʹ�õĿռ�
};
