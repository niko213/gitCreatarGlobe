/********************************************************************
* Copyright (c) 2014 北京超维创想信息技术有限公司
* All rights reserved.
* @file    GlbGlobeNetworkSymbol.h
* @brief   网络符号头文件，文档定义CGlbGlobeNetworkSymbol类
* @version 1.0
* @author  马林
* @date    2016-3-1 17:40
*********************************************************************/
#pragma once

#include "glbglobesymbol.h"
namespace GlbGlobe
{
	/**
	* @brief CGlbGlobeNetworkSymbol网络符号类				 
	* @version 1.0
	* @author  马林
	* @date    2016-3-1 17:40
	*/
	class GLB_SYMBOLDLL_CLASSEXPORT CGlbGlobeNetworkSymbol  : public CGlbGlobeSymbol
	{
	public:
		CGlbGlobeNetworkSymbol(void);
		~CGlbGlobeNetworkSymbol(void);

		/**
		* @brief 模型符号渲染
		* @param[in] obj 渲染对象
		* @return 对象节点
		*/
		virtual osg::Node *Draw(CGlbGlobeRObject *obj,IGlbGeometry *geo);

	private:
		// 生成两头空的圆柱体
		osg::ref_ptr<osg::Drawable> buildCylinder(unsigned int numSegments, float radius, float height, osg::Vec4 clr);

		// 单个网络线绘制成圆柱样式(使用材质绘制)
		osg::ref_ptr<osg::Node> DrawEdgeByCylinderShape(osg::Vec3d v1, osg::Vec3d v2, float radius, osg::Vec4 clr);
		// 整条网络线绘制成圆柱样式(使用材质绘制)
		osg::ref_ptr<osg::Node> DrawEdgeByCylinderShape(IGlbGeometry* geo, float radius, osg::Vec4 clr);
		// 网络线绘制成圆柱样式(使用纹理绘制)
		osg::ref_ptr<osg::Node> DrawEdgeByCylinderShape(osg::Vec3 v1, osg::Vec3 v2, float radius, const char* textureLocate);
	};
}

