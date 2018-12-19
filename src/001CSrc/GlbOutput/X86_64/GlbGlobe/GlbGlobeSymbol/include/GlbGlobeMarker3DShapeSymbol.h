/********************************************************************
* Copyright (c) 2014 北京超维创想信息技术有限公司
* All rights reserved.
* @file    GlbGlobeMarker3DShapesSymbol.h
* @brief   3D形状符号头文件，文档定义CGlbGlobeMarker3DShapeSymbol类
* @version 1.0
* @author  ChenPeng
* @date    2014-5-20 11:15
*********************************************************************/
#pragma once
#include "glbglobesymbol.h"
#include "osg/ShapeDrawable"

namespace GlbGlobe
{
	/**
	* @brief 3D形状符号类				 
	* @version 1.0
	* @author  ChenPeng
	* @date    2014-5-20 11:15
	*/
	class GLB_SYMBOLDLL_CLASSEXPORT CGlbGlobeMarker3DShapeSymbol : public CGlbGlobeSymbol
	{
	public:
		/**
		* @brief 构造函数
		*/
		CGlbGlobeMarker3DShapeSymbol(void);
		/**
		* @brief 析构函数
		*/
		~CGlbGlobeMarker3DShapeSymbol(void);
		/**
		* @brief 符号渲染
		* @param[in] obj 渲染对象
		* @return 对象节点
		*/
		osg::Node *Draw(CGlbGlobeRObject *obj,IGlbGeometry *geo);
	private:
		void DealDraw(osg::Shape *shape,
			osg::ShapeDrawable *shapeFillDrawable,
			osg::ShapeDrawable *shapeOutDrawable,
			GlbMarker3DShapeSymbolInfo *marker3DInfo,
			CGlbFeature *feature);
	};
}