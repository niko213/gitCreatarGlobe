/********************************************************************
* Copyright (c) 2014 北京超维创想信息技术有限公司
* All rights reserved.
* @file    GlbGlobeObjectFactory.h
* @brief   对象工厂头文件，文档定义CGlbGlobeObject类
* @version 1.0
* @author  ChenPeng
* @date    2014-5-5 14:20
*********************************************************************/

#pragma once
#include "GlbGlobeObject.h"
#include "GlbWString.h"

using namespace GlbGlobe;

class GLB_DLLCLASS_EXPORT IGlbGlobeObjectProvider : public CGlbReference
{
public:
	IGlbGlobeObjectProvider();
	virtual ~IGlbGlobeObjectProvider();
	virtual CGlbGlobeObject * Create()=0;
};

class GLB_DLLCLASS_EXPORT CGlbGlobeObjectFactory  : public CGlbReference
{
public:
	CGlbGlobeObjectFactory(void);
	~CGlbGlobeObjectFactory(void);
	
	static CGlbGlobeObjectFactory* GetInstance();

	void Register(IGlbGlobeObjectProvider* creator,glbWChar* objclassname);
	void UnRegister(glbWChar* objclassname);
	//IGlbGlobeObjectProvider* GetObjectCreator(int type);
	CGlbGlobeObject* GetObject(glbWChar* objclassname);
public:
	std::map<CGlbWString,IGlbGlobeObjectProvider*> mpr_providers;
};

//extern CGlbGlobeObjectFactory* g_objectFactory;
extern CGlbGlobeObjectFactory g_objectFactory;


