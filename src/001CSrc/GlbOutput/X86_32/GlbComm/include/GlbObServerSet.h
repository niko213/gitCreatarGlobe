#pragma once
#include "GlbReference.h"
#include "GlbCriticalSection.h"
class GLBCOMM_API CGlbObserver
{
public:
	CGlbObserver(){}
	virtual ~CGlbObserver(){}
	virtual void ObjectDeleted(void*){}
};

/**
  * @brief CGlbObserverSet�۲��߼�����
  *
  * 					 
  * @version 1.0
  * @author  HJZ
  * @date    2014-3-7 14:40 
  */
class GLBCOMM_API CGlbObserverSet : public CGlbReference
{
public:
	/**
	  * @brief ���캯��
	  *
	  * @param observeredObj �۲��߶���
	  * @return ��
	  */
	CGlbObserverSet(const CGlbReference* observeredObj);
	/**
	  * @brief ��������
	  *
	  * @return ��
	  */
	virtual ~CGlbObserverSet();
	/**
	  * @brief ���һ���۲���
	  *
	  *
	  * @param observer ��ӵĹ۲��߶���
	  * @return ��
	  */
	void AddObserver(CGlbObserver* observer);
	/**
	  * @brief ɾ��һ���۲���
	  *
	  *
	  * @param observer Ҫɾ���۲��߶���
	  * @return ��
	  */
	void RemoveObserver(CGlbObserver* observer);
	CGlbReference* AddRefLock();
	void SignalObjectDeleted(void* ptr);
	/**
	  * @brief ��ȡһ���۲���
	  *
	  * @return �۲��߶���
	  */
	const CGlbReference* GetObserverdObject() const;
private:	
	std::set<CGlbObserver*> mpr_observers;          ///<�۲��߼���
	CGlbReference*          mpr_observeredObject;   ///<���۲����
};


