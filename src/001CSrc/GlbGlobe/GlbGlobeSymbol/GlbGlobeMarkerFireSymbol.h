/********************************************************************
* Copyright (c) 2014 北京超维创想信息技术有限公司
* All rights reserved.
* @file    GlbGlobeMarkerFireSymbol.h
* @brief   火符号头文件，文档定义CGlbGlobeMarkerFireSymbol类
* @version 1.0
* @author  malin
* @date    2015-12-17 13:40
*********************************************************************/
#pragma once
#include "glbglobesymbol.h"

namespace GlbGlobe
{
	/**
	* @brief 火符号类				 
	* @version 1.0
	* @author  malin
	* @date   2015-12-17 13:40
	*/
	class GLB_SYMBOLDLL_CLASSEXPORT CGlbGlobeMarkerFireSymbol : public CGlbGlobeSymbol
	{
	public:
		CGlbGlobeMarkerFireSymbol(void);
		~CGlbGlobeMarkerFireSymbol(void);

		/**
		* @brief 符号渲染
		* @param[in] obj 渲染对象
		* @return 对象节点
		*/
		osg::Node *Draw(CGlbGlobeRObject *obj,IGlbGeometry *geo);		
	};
}

