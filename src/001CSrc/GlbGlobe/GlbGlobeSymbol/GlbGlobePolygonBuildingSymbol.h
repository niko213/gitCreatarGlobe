/********************************************************************
* Copyright (c) 2014 北京超维创想信息技术有限公司
* All rights reserved.
* @file    GlbGlobePolygonBuildingSymbol.h
* @brief   多边形符号头文件，文档定义CGlbGlobePolygonBuildingSymbol类
* @version 1.0
* @author  龙庭友
* @date    2014-6-18 15:30
*********************************************************************/
#pragma once


#include "GlbGlobeSymbolExport.h"

#include "GlbGlobeSymbol.h"


namespace GlbGlobe
{
	/**
	* @brief 多边形建筑符号类
	* @version 1.0
	* @author  龙庭友
	* @date    2014-6-18 15:30
	*/
	class GLB_SYMBOLDLL_CLASSEXPORT CGlbGlobePolygonBuildingSymbol : public CGlbGlobeSymbol
	{
	public:
		CGlbGlobePolygonBuildingSymbol(void);
		~CGlbGlobePolygonBuildingSymbol(void);

		virtual osg::Node * Draw( CGlbGlobeRObject *obj,IGlbGeometry *geo );

		virtual void DrawToImage( CGlbGlobeRObject *obj, IGlbGeometry* geom,glbByte *image,glbInt32 imageW, glbInt32 imageH,CGlbExtent &ext );

		virtual IGlbGeometry * GetOutline(CGlbGlobeRObject *obj,IGlbGeometry *geo);

	};

}
