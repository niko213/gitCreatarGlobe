/********************************************************************
* Copyright (c) 2014 北京超维创想信息技术有限公司
* All rights reserved.
* @file    CGlbGlobeMarkerPixelSymbol.h
* @brief   像素点符号头文件，文档定义CGlbGlobeMarkerPixelSymbol类
* @version 1.0
* @author  ChenPeng
* @date    2014-5-8 13:40
*********************************************************************/
#pragma once
#include "glbglobesymbol.h"

namespace GlbGlobe
{
	/**
	* @brief 像素点符号类				 
	* @version 1.0
	* @author  ChenPeng
	* @date    2014-5-8 13:40
	*/
	class GLB_SYMBOLDLL_CLASSEXPORT CGlbGlobeMarkerPixelSymbol :public CGlbGlobeSymbol
	{
	public:
		/**
		* @brief 构造函数
		*/
		CGlbGlobeMarkerPixelSymbol(void);
		/**
		* @brief 析构函数
		*/
		~CGlbGlobeMarkerPixelSymbol(void);
		/**
		* @brief 模型符号渲染
		* @param[in] obj 渲染对象
		* @return 对象节点
		*/
		osg::Node *Draw(CGlbGlobeRObject *obj,IGlbGeometry *geo);
	};
}