/********************************************************************
* Copyright (c) 2014 北京超维创想信息技术有限公司
* All rights reserved.
* @file    GlbGlobeVirtualDrillSymbol.h
* @brief   虚拟钻孔符号头文件，文档定义CGlbGlobeVirtualDrillSymbol类
* @version 1.0
* @author  马林
* @date    2018-8-21 10:40
*********************************************************************/
#pragma once
#include "glbglobesymbol.h"

namespace GlbGlobe
{
	/**
	* @brief CGlbGlobeVirtualDrillSymbol虚拟钻孔符号类				 
	* @version 1.0
	* @author  马林
	* @date    2018-8-21 10:40
	*/
	class GLB_SYMBOLDLL_CLASSEXPORT CGlbGlobeVirtualDrillSymbol : public CGlbGlobeSymbol
	{
	public:
		CGlbGlobeVirtualDrillSymbol(void);
		~CGlbGlobeVirtualDrillSymbol(void);

		/**
		* @brief 模型符号渲染
		* @param[in] obj 渲染对象
		* @return 对象节点
		*/
		virtual osg::Node *Draw(CGlbGlobeRObject *obj,IGlbGeometry *geo);
	};
}

