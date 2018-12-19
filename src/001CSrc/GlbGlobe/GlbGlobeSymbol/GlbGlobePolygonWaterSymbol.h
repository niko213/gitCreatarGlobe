/********************************************************************
* Copyright (c) 2014 北京超维创想信息技术有限公司
* All rights reserved.
* @file    GlbGlobePolygonWaterSymbol.h
* @brief   多边形水面符号头文件，文档定义CGlbGlobePolygonWaterSymbol类
* @version 1.0
* @author  龙庭友
* @date    2014-6-18 15:30
*********************************************************************/
#pragma once

//  2016/10/11 备注： 水效改用 CGlbGlobePolygonLakeSymbol来实现 ,本类废弃   马林

#include "GlbGlobeSymbolExport.h"

#include "GlbGlobeSymbol.h"

namespace GlbGlobe
{
	class IWaterScene
	{
	public:
		virtual void enableReflections( bool enable )=0;
		virtual void enableRefractions( bool enable )=0;
	};

	/**
	* @brief 多边形水面符号类
	* @version 1.0
	* @author  龙庭友
	* @date    2014-6-18 15:30
	*/
	class GLB_SYMBOLDLL_CLASSEXPORT CGlbGlobePolygonWaterSymbol : public CGlbGlobeSymbol
	{
	public:
		CGlbGlobePolygonWaterSymbol(void);
		~CGlbGlobePolygonWaterSymbol(void);

		virtual osg::Node * Draw( CGlbGlobeRObject *obj,IGlbGeometry *geo );

		virtual void DrawToImage( CGlbGlobeRObject *obj, IGlbGeometry* geom,glbByte *image,glbInt32 imageW, glbInt32 imageH,CGlbExtent &ext );

		virtual IGlbGeometry * GetOutline(CGlbGlobeRObject *obj,IGlbGeometry *geo);
	private:
		osg::Node *buildPolygonWaterSurfaceDrawable(CGlbGlobeRObject *obj,IGlbGeometry *geo);

	};

}
