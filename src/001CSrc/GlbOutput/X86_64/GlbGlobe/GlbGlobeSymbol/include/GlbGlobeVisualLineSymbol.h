/********************************************************************
* Copyright (c) 2014 北京超维创想信息技术有限公司
* All rights reserved.
* @file    GlbGlobeVisualLineSymbol.h
* @brief   可视线符号头文件，文档定义CGlbGlobeVisualLineSymbol类
* @version 1.0
* @author  龙庭友
* @date    2014-9-22 15:30
*********************************************************************/
#pragma once

#include "GlbGlobeSymbolExport.h"

#include "GlbGlobeSymbol.h"


namespace GlbGlobe
{
	/**
	* @brief 可视线符号类
	* @version 1.0
	* @author  龙庭友
	* @date    2014-9-22 15:30
	*/
	class GLB_SYMBOLDLL_CLASSEXPORT CGlbGlobeVisualLineSymbol :	public CGlbGlobeSymbol
	{
	public:
		CGlbGlobeVisualLineSymbol(void);
		~CGlbGlobeVisualLineSymbol(void);

		virtual osg::Node * Draw( CGlbGlobeRObject *obj,IGlbGeometry *geo );

		virtual void DrawToImage( CGlbGlobeRObject *obj,IGlbGeometry* geom, glbByte *image,glbInt32 imageW, glbInt32 imageH,CGlbExtent &ext );

		virtual IGlbGeometry * GetOutline( CGlbGlobeRObject *obj,IGlbGeometry *geo );

	};

}
