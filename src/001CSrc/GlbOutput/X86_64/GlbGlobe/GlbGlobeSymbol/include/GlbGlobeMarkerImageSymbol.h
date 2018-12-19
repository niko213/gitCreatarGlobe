/********************************************************************
* Copyright (c) 2014 北京超维创想信息技术有限公司
* All rights reserved.
* @file    GlbGlobeMarkerImageSymbol.h
* @brief   图标符号头文件，文档定义GlbGlobeMarkerImageSymbol类
* @version 1.0
* @author  ChenPeng
* @date    2014-5-20 11:15
*********************************************************************/
#pragma once
#include "glbglobesymbol.h"
#include "osg/Geometry"
#include "GlbGlobeAutoTransform.h"

namespace GlbGlobe
{
	/**
	* @brief 图标符号类				 
	* @version 1.0
	* @author  ChenPeng
	* @date    2014-5-20 11:15
	*/
	class GLB_SYMBOLDLL_CLASSEXPORT CGlbGlobeMarkerImageSymbol : public CGlbGlobeSymbol
	{
	public:
		/**
		* @brief 构造函数
		*/
		CGlbGlobeMarkerImageSymbol(void);
		/**
		* @brief 析构函数
		*/
		~CGlbGlobeMarkerImageSymbol(void);
		/**
		* @brief 符号渲染
		* @param[in] obj 渲染对象
		* @return 对象节点
		*/
		osg::Node *Draw(CGlbGlobeRObject *obj,IGlbGeometry *geo);
	private:
		osg::Geometry *CreateOutlineQuad(osg::Vec3 cornerVec,osg::Vec3 widthVec,osg::Vec3 heightVec);
		osg::Node* Draw360DegreePanorama(CGlbGlobeRObject *obj,IGlbGeometry *geo);
	};
}