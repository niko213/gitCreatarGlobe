/********************************************************************
* Copyright (c) 2014 北京超维创想信息技术有限公司
* All rights reserved.
* @file    GlbGlobeLine3DSymbol.h
* @brief   3d线符号头文件，文档定义CGlbGlobeLine3DSymbol类
* @version 1.0
* @author  敖建
* @date    2014-8-12 11:13
*********************************************************************/
#pragma once
#include "StdAfx.h"
#include "glbglobesymbol.h"
namespace GlbGlobe
{
	/**
	* @brief 3d线符号类				 
	* @version 1.0
	* @author  敖建
	* @date    2014-8-7 11:13
	*/
	class GLB_SYMBOLDLL_CLASSEXPORT CGlbGlobeLine3DSymbol
	{
	public:
		/**
		* @ brief 构造函数
		*/
		CGlbGlobeLine3DSymbol(void);
		/**
		* @ brief 析构函数
		*/
		~CGlbGlobeLine3DSymbol(void);
		/**
		* @brief 模型符号渲染
		* @param[in] obj 渲染对象
		* @return 对象节点
		*/
		osg::Node *Draw(CGlbGlobeRObject *obj,IGlbGeometry *geo);
	};
}
