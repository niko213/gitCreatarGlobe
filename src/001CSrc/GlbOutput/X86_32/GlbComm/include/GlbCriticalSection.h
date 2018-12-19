#pragma once
#include "GlbWString.h"
/**
  * @brief GlbCriticalSection关键段类
  *
  * 用于多线程控制线程互斥
  * 					 
  * @version 1.0
  * @author  GWB
  * @date    2014-3-10 14:40
  */
class GLBCOMM_API GlbCriticalSection
{
public:
	/**
	  * @brief 构造函数
	  *
	  *
	  * @return 无
	  */
	GlbCriticalSection()
	{
		InitializeCriticalSection(&_critical);
		_name = L"";
	}

	/**
	  * @brief 析构函数
	  *
	  *
	  * @return 无
	  */
	~GlbCriticalSection(void)
	{
		DeleteCriticalSection(&_critical);
	}
public:
	/**
	  * @brief 进入关键段
	  *
	  *
	  * @return 无
	  */
	void Lock()
	{
		EnterCriticalSection(&_critical);
	}
	/**
	  * @brief 离开关键段
	  *
	  *
	  * @return 无
	  */
	void UnLock()
	{
		LeaveCriticalSection(&_critical);
	}
	void SetName(const glbWChar* crtName)
	{
		if(crtName)
			_name = crtName;
		else
			_name = L"";
	}
	const glbWChar* GetName()
	{
		return _name.c_str();
	}
private:
	CRITICAL_SECTION _critical; ///<关键段
	CGlbWString      _name;
};

template <class M> 
class GlbScopedLock
{
    private:
        M& m_lock;
        GlbScopedLock(const GlbScopedLock&);            // 阻止拷贝
        GlbScopedLock& operator=(const GlbScopedLock&); // 阻止赋值
    public:
        explicit GlbScopedLock(M& m):m_lock(m) {m_lock.Lock();}
        ~GlbScopedLock(){m_lock.UnLock();}
};

