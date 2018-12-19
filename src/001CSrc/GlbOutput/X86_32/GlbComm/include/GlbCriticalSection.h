#pragma once
#include "GlbWString.h"
/**
  * @brief GlbCriticalSection�ؼ�����
  *
  * ���ڶ��߳̿����̻߳���
  * 					 
  * @version 1.0
  * @author  GWB
  * @date    2014-3-10 14:40
  */
class GLBCOMM_API GlbCriticalSection
{
public:
	/**
	  * @brief ���캯��
	  *
	  *
	  * @return ��
	  */
	GlbCriticalSection()
	{
		InitializeCriticalSection(&_critical);
		_name = L"";
	}

	/**
	  * @brief ��������
	  *
	  *
	  * @return ��
	  */
	~GlbCriticalSection(void)
	{
		DeleteCriticalSection(&_critical);
	}
public:
	/**
	  * @brief ����ؼ���
	  *
	  *
	  * @return ��
	  */
	void Lock()
	{
		EnterCriticalSection(&_critical);
	}
	/**
	  * @brief �뿪�ؼ���
	  *
	  *
	  * @return ��
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
	CRITICAL_SECTION _critical; ///<�ؼ���
	CGlbWString      _name;
};

template <class M> 
class GlbScopedLock
{
    private:
        M& m_lock;
        GlbScopedLock(const GlbScopedLock&);            // ��ֹ����
        GlbScopedLock& operator=(const GlbScopedLock&); // ��ֹ��ֵ
    public:
        explicit GlbScopedLock(M& m):m_lock(m) {m_lock.Lock();}
        ~GlbScopedLock(){m_lock.UnLock();}
};

