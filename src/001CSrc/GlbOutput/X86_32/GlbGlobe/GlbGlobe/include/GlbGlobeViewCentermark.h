/********************************************************************
* Copyright (c) 2014 北京超维创想信息技术有限公司
* All rights reserved.
*
* @file    GlbGlobeViewCentermark.h
* @brief   视图相关元素中心标记
*
* @version 1.0
* @author  敖建
* @date    2014-7-16 11:43
*********************************************************************/

#pragma once
#include "GlbGlobeView.h"
#include <osg/Camera>
#include <osgViewer/View>
namespace GlbGlobe
{
	class CGlbGlobe;
	class CGlbGlobeView;
	/**
	* @brief 视图相关元素中心标记类
	* @version 1.0
	* @author  敖建
	* @date    2014-7-16 11:43
	*/
	class GLB_DLLCLASS_EXPORT CGlbGlobeViewCentermark : public osg::Camera
	{
	public:
		/**
		* @brief 构造函数
		*/
		CGlbGlobeViewCentermark(CGlbGlobeView* pview,int width,int height,CGlbGlobe* pglobe);
		/**
		* @brief 析构函数
		*/
		~CGlbGlobeViewCentermark(void);
		/**
		* @brief 设置中心标记位置
		* @param [in] width : 屏幕宽度
		*		  [in] height : 屏幕高度
		*/
		void SetPosition(int left,int top,int width,int height);
	};
}

