/********************************************************************
* Copyright (c) 2014 北京超维创想信息技术有限公司
* All rights reserved.
* @file    GlbGlobeMarkerSmokeSymbol.h
* @brief   烟雾符号头文件，文档定义CGlbGlobeMarkerSmokeSymbol类
* @version 1.0
* @author  malin
* @date    2015-12-17 13:40
*********************************************************************/
#pragma once
#include "glbglobesymbol.h"

namespace GlbGlobe
{
	/**
	* @brief 烟雾符号类				 
	* @version 1.0
	* @author  malin
	* @date   2015-12-17 13:40
	*/
	class GLB_SYMBOLDLL_CLASSEXPORT CGlbGlobeMarkerSmokeSymbol : public CGlbGlobeSymbol
	{
	public:
		CGlbGlobeMarkerSmokeSymbol(void);
		~CGlbGlobeMarkerSmokeSymbol(void);
		/**
		* @brief 符号渲染
		* @param[in] obj 渲染对象
		* @return 对象节点
		*/
		osg::Node *Draw(CGlbGlobeRObject *obj,IGlbGeometry *geo);	
	private:
		osg::ref_ptr<osg::Group> CreateMySmokeParticle(osg::Vec3 pos,osg::Vec3 startColor, osg::Vec3 endColor, float scale, float intensity);
	};
}


