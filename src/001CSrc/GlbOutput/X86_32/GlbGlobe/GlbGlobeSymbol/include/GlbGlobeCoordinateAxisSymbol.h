/********************************************************************
* Copyright (c) 2014 北京超维创想信息技术有限公司
* All rights reserved.
* @file    GlbGlobeCoordinateAxisSymbol.h
* @brief   坐标轴符号头文件，文档定义CGlbGlobeCoordinateAxisSymbol类
* @version 1.0
* @author  马林
* @date    2016-7-6 15:40
*********************************************************************/
#pragma once

#include "glbglobesymbol.h"

namespace GlbGlobe
{
	/**
	* @brief CGlbGlobeCoordinateAxisSymbol 坐标轴符号类				 
	* @version 1.0
	* @author  马林
	* @date    2016-7-6 15:40
	*/
	class GLB_SYMBOLDLL_CLASSEXPORT CGlbGlobeCoordinateAxisSymbol : public CGlbGlobeSymbol
	{
	public:
		/**
		* @brief 构造函数
		*/
		CGlbGlobeCoordinateAxisSymbol(void);
		/**
		* @brief 析构函数
		*/
		~CGlbGlobeCoordinateAxisSymbol(void);

		/**
		* @brief 模型符号渲染
		* @param[in] obj 渲染对象
		* @return 对象节点
		*/
		virtual osg::Node *Draw(CGlbGlobeRObject *obj,IGlbGeometry *geo);
	private:
		osg::Node* buildCoordinateAxis(osg::Vec3d axislen, osg::Vec3d axistep, osg::Vec3d axiscale, bool isInvertZ,double labelSize=40.0);
		osg::Node* buildCoordinateAxisGrid(osg::Vec3d axislen, osg::Vec3d axistep, osg::Vec3d axiscale, bool isInvertZ);
		osg::Node* buildCoordinateAxisLabel(osg::Vec3d orig,osg::Vec3d axislen, osg::Vec3d axistep, osg::Vec3d axiscale, bool isInvertZ, double labelSize=40.0);

	private:
		GlbGlobeTypeEnum mpr_globeType;
	};

}