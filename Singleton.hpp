#pragma once

#include <mutex>

template <typename T>
class CSingleton
{
public:
	static T *GetInstance()
	{
		if (m_pInstance == nullptr)
		{
			std::lock_guard<std::mutex> guard(m_mutex);
			if (m_pInstance == nullptr)
				m_pInstance = new T();
		}
		return m_pInstance;
	}

private:
	static std::mutex m_mutex;
	static T *m_pInstance;
};
template <typename T>
std::mutex CSingleton<T>::m_mutex;
template <typename T>
T *CSingleton<T>::m_pInstance = nullptr;