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
  * @brief CGlbObserverSet观察者集合类
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
	  * @brief 构造函数
	  *
	  * @param observeredObj 观察者对象
	  * @return 无
	  */
	CGlbObserverSet(const CGlbReference* observeredObj);
	/**
	  * @brief 析构函数
	  *
	  * @return 无
	  */
	virtual ~CGlbObserverSet();
	/**
	  * @brief 添加一个观察者
	  *
	  *
	  * @param observer 添加的观察者对象
	  * @return 无
	  */
	void AddObserver(CGlbObserver* observer);
	/**
	  * @brief 删除一个观察者
	  *
	  *
	  * @param observer 要删除观察者对象
	  * @return 无
	  */
	void RemoveObserver(CGlbObserver* observer);
	CGlbReference* AddRefLock();
	void SignalObjectDeleted(void* ptr);
	/**
	  * @brief 获取一个观察者
	  *
	  * @return 观察者对象
	  */
	const CGlbReference* GetObserverdObject() const;
private:	
	std::set<CGlbObserver*> mpr_observers;          ///<观察者集合
	CGlbReference*          mpr_observeredObject;   ///<被观察对象
};


