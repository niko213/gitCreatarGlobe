/********************************************************************
  * Copyright (c) 2013 ������ά������Ϣ�������޹�˾
  * All rights reserved.
  *
  * @file    GlbReference.h
  * @brief   ���ü���ͷ�ļ�
  *
  * �����������CGlbReference�����
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
  * @brief CGlbReference���ü�����
  *
  * ������ô������������ָ�룬������ʾ�Ƿ�ָ��
  * 					 
  * @version 1.0
  * @author  HJZ
  * @date    2014-3-10 14:40
  */
class GLBCOMM_API CGlbReference
{
public:
/**
  * @brief ���캯��
  *
  * ��ʼ�����ô���Ϊ0
  *
  * @return ��
  */
	CGlbReference(void);
	//��������
	CGlbReference(const CGlbReference&);
	inline CGlbReference& operator = (const CGlbReference&) { return *this; }
/**
  * @brief ��������
  *
  * @return ��
  */
	virtual ~CGlbReference(void);
/**
  * @brief ����һ������
  *
  * @return �µ����ô���
  */
	virtual glbUInt32 Ref();
/**
  * @brief ����һ������
  *
  * @return �µ����ô���
  */
	virtual glbUInt32 UnRef();
/**
  * @brief ����һ������
  *
  * ����һ������
  *
  * @return ��
  */
	glbUInt32 UnRef_Nodelete();
/**
  * @brief ��ӹ۲���
  *
  */
	void AddObserver(CGlbObserver* observer);
/**
  * @brief �Ƴ��۲���
  *
  */
	void RemoveObserver(CGlbObserver* observer);
/*
 * @brief ��ȡ�۲��߼���
 *
 */
	CGlbObserverSet* GetObserverSet();
private:
	glbUInt32                  mpr_refcount;    ///<���ô���			
	CGlbObserverSet*           mpr_observerSet; ///<�۲��߼���
protected:
	GlbCriticalSection         mpr_critical;    ///<�ٽ���
};