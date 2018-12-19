/********************************************************************
* Copyright (c) 2014 北京超维创想信息技术有限公司
* All rights reserved.
* @file    GlbGlobeViewSymbol.h
* @brief   可视区域分析符号头文件，文档定义CGlbGlobeViewSymbol类
* @version 1.0
* @author  龙庭友
* @date    2014-9-22 15:30
*********************************************************************/
#pragma once

#include "GlbGlobeSymbolExport.h"

#include "GlbGlobeSymbol.h"

#include "GlbPoint.h"

namespace GlbGlobe
{
	/**
	* @brief 可视区域分析符号类
	* @version 1.0
	* @author  龙庭友
	* @date    2014-9-22 15:30
	*/
	class GLB_SYMBOLDLL_CLASSEXPORT CGlbGlobeViewSymbol : public CGlbGlobeSymbol
	{
	public:
		CGlbGlobeViewSymbol(void);
		~CGlbGlobeViewSymbol(void);

		virtual osg::Node * Draw( CGlbGlobeRObject *obj,IGlbGeometry *geo );

		virtual void DrawToImage( CGlbGlobeRObject *obj,IGlbGeometry* geom, glbByte *image,glbInt32 imageW, glbInt32 imageH,CGlbExtent &ext );

		virtual IGlbGeometry * GetOutline( CGlbGlobeRObject *obj,IGlbGeometry *geo );

	};

}
