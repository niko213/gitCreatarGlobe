/********************************************************************
* Copyright (c) 2014 北京超维创想信息技术有限公司
* All rights reserved.
*
* @file    GlbGlobeViewScale.h
* @brief   视图相关元素联动视图
*
* @version 1.0
* @author  ChenPeng
* @date    2016-5-13 10:00
*********************************************************************/
#pragma once
#include "GlbGlobeView.h"
namespace GlbGlobe
{
	class CGlbGlobeViewSlaveUpdateCallback : public osg::NodeCallback
	{
	public:
		CGlbGlobeViewSlaveUpdateCallback(CGlbGlobeView* pview);
		~CGlbGlobeViewSlaveUpdateCallback();
		void operator()(osg::Node* node, osg::NodeVisitor* nv);
	private:
		//glbref_ptr<CGlbGlobeView> mpr_view;
		CGlbGlobeView* mpr_view;
	};
	class GLB_DLLCLASS_EXPORT CGlbGlobeViewSlave : public osg::Camera
	{
	public:
		CGlbGlobeViewSlave(CGlbGlobeView* pview,int x,int y,int width,int height);
		~CGlbGlobeViewSlave(void);
	};
}