/********************************************************************
* Copyright (c) 2014 北京超维创想信息技术有限公司
* All rights reserved.
* @file    GlbGlobeLineDynamicArrowSymbol.h
* @brief   动态箭头线符号头文件，文档定义CGlbGlobeLineDynamicArrowSymbol类
* @version 1.0
* @author  ChenPeng
* @date    2016-6-20 11:40
*********************************************************************/

#pragma once
#include "GlbGlobeSymbol.h"
#include <osg/AnimationPath>
namespace GlbGlobe
{
	/**
	* @brief 动态箭头线符号类				 
	* @version 1.0
	* @author  ChenPeng
	* @date    2016-6-20 11:40
	*/
	class GLB_SYMBOLDLL_CLASSEXPORT CGlbGlobeLineDynamicArrowSymbol : public CGlbGlobeSymbol
	{
	private:
		struct PathMark
		{
			int lineIndedx;
			osg::Vec3d startPoint;
		};
	public:
		/**
		* @brief 构造函数
		*/
		CGlbGlobeLineDynamicArrowSymbol(void);
		/**
		* @brief 析构函数
		*/
		~CGlbGlobeLineDynamicArrowSymbol(void);
		/**
		* @brief 动态箭头线符号渲染
		* @param[in] obj 渲染对象
		* @return 对象节点
		*/
		osg::Node *Draw(CGlbGlobeRObject *obj,IGlbGeometry *geo);
		/**
		* @brief AGG二维绘制到像素
		* @param[in] obj 渲染对象
		* @param[in] geom 要渲染的几何体
		* @param[in] image 底图缓冲区
		* @param[in] imageW 底图的宽
		* @param[in] imageH 底图的高
		* @param[in] ext 地形块对应的范围
		*/
		IGlbGeometry *GetOutline(CGlbGlobeRObject *obj,IGlbGeometry *geo);
	private:
		osg::AnimationPathCallback *CreateAnimationPathCallback(osg::Vec3dArray *points, glbDouble startTime,glbDouble endTime,glbBool isModel = false);
		osg::Geode *Draw3DArrow(glbDouble radius,glbDouble length,osg::Vec4f color);
	};
}