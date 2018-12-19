/********************************************************************
* Copyright (c) 2014 北京超维创想信息技术有限公司
* All rights reserved.
* @file    GlbGlobeDrillSymbol.h
* @brief   钻孔符号头文件，文档定义CGlbGlobeDrillSymbol类
* @version 1.0
* @author  马林
* @date    2014-11-19 10:40
*********************************************************************/
#pragma once

#include "glbglobesymbol.h"
namespace GlbGlobe
{
	/**
	* @brief CGlbGlobeDrillSymbol钻孔符号类				 
	* @version 1.0
	* @author  马林
	* @date    2014-11-19 10:40
	*/
	class GLB_SYMBOLDLL_CLASSEXPORT CGlbGlobeDrillSymbol : public CGlbGlobeSymbol
	{
	public:
		/**
		* @brief 构造函数
		*/
		CGlbGlobeDrillSymbol(void);

		/**
		* @brief 析构函数
		*/
		~CGlbGlobeDrillSymbol(void);

		/**
		* @brief 模型符号渲染
		* @param[in] obj 渲染对象
		* @return 对象节点
		*/
		virtual osg::Node *Draw(CGlbGlobeRObject *obj,IGlbGeometry *geo);

	private:
		// 生成圆柱形的地层(地层线)
		osg::Node* buildStratumCylinder(IGlbGeometry * _ln, bool isGlobe, double cylinderRadius,osg::Vec3d originPos);
		// 根据线上的起点和终点pt0,pt1,计算形成的圆柱点和法向
		void computeCylinderVertexs(osg::Vec3d pt0, osg::Vec3d pt1, double radius, osg::Vec3dArray* vts, osg::Vec3Array* nors);
	};
}

