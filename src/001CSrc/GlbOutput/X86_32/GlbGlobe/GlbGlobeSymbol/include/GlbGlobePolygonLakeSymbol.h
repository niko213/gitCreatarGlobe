/********************************************************************
* Copyright (c) 2014 北京超维创想信息技术有限公司
* All rights reserved.
* @file    GlbGlobePolygonLakeSymbol.h
* @brief   湖泊水符号头文件，文档定义GlbGlobePolygonLakeSymbol类
* @version 1.0
* @author  ChenPeng
* @date    2016-8-15 15:20
*********************************************************************/
#pragma once

#include "GlbGlobeSymbolExport.h"
#include "GlbGlobeSymbol.h"
#include "GlbPolygon.h"
#include "StdAfx.h"

namespace GlbGlobe
{
	/**
	* @brief 湖泊水符号类
	* @version 1.0
	* @author  ChenPeng
	* @date    2016-8-15 15:20
	*/
	class GLB_SYMBOLDLL_CLASSEXPORT CGlbGlobePolygonLakeSymbol : public CGlbGlobeSymbol
	{
	public:
		CGlbGlobePolygonLakeSymbol(void);
		~CGlbGlobePolygonLakeSymbol(void);

		virtual osg::Node *Draw( CGlbGlobeRObject *obj,IGlbGeometry *geo);
		virtual IGlbGeometry * GetOutline(CGlbGlobeRObject *obj,IGlbGeometry *geo);
	private:
		osg::Node *buildDrawable(CGlbGlobeRObject *obj,CGlbPolygon *geo);
		osg::Vec2dArray *buildTexCoords(osg::Vec3dArray *points);
	};
}