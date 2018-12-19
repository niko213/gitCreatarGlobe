/********************************************************************
  * Copyright (c) 2013 北京超维创想信息技术有限公司
  * All rights reserved.
  *
  * @file    GlbReference.h
  * @brief   引用计数头文件
  *
  * 这个档案定义CGlbReference这个类
  *
  * @version 1.0
  * @author  HJZ
  * @date    2014-3-10 17:40
*********************************************************************/
#pragma once
#include "GlbCommExport.h"
#include "GlbCommTypes.h"
#include "GlbCriticalSection.h"
#include <set>
using namespace std;


class CGlbObserverSet;
class CGlbObserver;

/**
  * @brief CGlbReference引用计数类
  *
  * 类的引用次数，配合智能指针，不用显示是否指针
  * 					 
  * @version 1.0
  * @author  HJZ
  * @date    2014-3-10 14:40
  */
class GLBCOMM_API CGlbReference
{
public:
/**
  * @brief 构造函数
  *
  * 初始化引用次数为0
  *
  * @return 无
  */
	CGlbReference(void);
	//拷贝构造
	CGlbReference(const CGlbReference&);
	inline CGlbReference& operator = (const CGlbReference&) { return *this; }
/**
  * @brief 析构函数
  *
  * @return 无
  */
	virtual ~CGlbReference(void);
/**
  * @brief 增加一次引用
  *
  * @return 新的引用次数
  */
	virtual glbUInt32 Ref();
/**
  * @brief 减少一次引用
  *
  * @return 新的引用次数
  */
	virtual glbUInt32 UnRef();
/**
  * @brief 减少一次引用
  *
  * 减少一次引用
  *
  * @return 无
  */
	glbUInt32 UnRef_Nodelete();
/**
  * @brief 添加观察者
  *
  */
	void AddObserver(CGlbObserver* observer);
/**
  * @brief 移除观察者
  *
  */
	void RemoveObserver(CGlbObserver* observer);
/*
 * @brief 获取观察者集合
 *
 */
	CGlbObserverSet* GetObserverSet();
private:
	glbUInt32                  mpr_refcount;    ///<引用次数			
	CGlbObserverSet*           mpr_observerSet; ///<观察者集合
protected:
	GlbCriticalSection         mpr_critical;    ///<临界区
};