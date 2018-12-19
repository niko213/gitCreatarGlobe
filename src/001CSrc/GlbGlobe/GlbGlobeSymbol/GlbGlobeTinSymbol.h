/********************************************************************
* Copyright (c) 2014 北京超维创想信息技术有限公司
* All rights reserved.
* @file    GlbGlobeTinSymbol.h
* @brief   地质体Tin符号头文件，文档定义CGlbGlobeTinSymbol类
* @version 1.0
* @author  龙庭友
* @date    2014-11-13 15:30
*********************************************************************/
#pragma once

#include "GlbGlobeSymbolExport.h"
#include "GlbGlobeSymbol.h"

class CGlbTin;
class CGlbLine;
class CGlbPoint;
class CGlbMultiTin;

namespace GlbGlobe
{
	/**
	* @brief 地质体Tin符号类
	* @version 1.0
	* @author  龙庭友
	* @date    2014-11-13 15:30
	*/
	class GLB_SYMBOLDLL_CLASSEXPORT CGlbGlobeTinSymbol : public CGlbGlobeSymbol
	{
	public:
		CGlbGlobeTinSymbol(void);
		~CGlbGlobeTinSymbol(void);

		virtual osg::Node * Draw( CGlbGlobeRObject *obj,IGlbGeometry *geo );

		virtual void DrawToImage( CGlbGlobeRObject *obj,IGlbGeometry* geom, glbByte *image,glbInt32 imageW, glbInt32 imageH,CGlbExtent &ext );

		virtual IGlbGeometry * GetOutline( CGlbGlobeRObject *obj,IGlbGeometry *geo );
	private:
		osg::Node *buildDrawable(CGlbGlobeRObject *obj,CGlbPoint *geo );
		osg::Node *buildDrawable(CGlbGlobeRObject *obj,CGlbLine *geo );
		osg::Node *buildDrawable(CGlbGlobeRObject *obj,CGlbTin *geo );
		osg::Node *buildDrawable(CGlbGlobeRObject *obj,CGlbMultiTin *geo );
		osg::Node *buildDrawable(GlbTinSymbolInfo *info,CGlbTin *geo,GlbGlobeTypeEnum type,CGlbFeature *feature);
	};

}
