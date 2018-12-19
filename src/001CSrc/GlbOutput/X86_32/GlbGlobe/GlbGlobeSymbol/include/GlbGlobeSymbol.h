/********************************************************************
* Copyright (c) 2014 北京超维创想信息技术有限公司
* All rights reserved.
* @file    GlbGlobeSymbol.h
* @brief   符号基类头文件，文档定义CGlbGlobeSymbol类
* @version 1.0
* @author  ChenPeng
* @date    2014-5-8 11:30
*********************************************************************/
#pragma once
#include "osg/Node"
#include "GlbGlobeSymbolExport.h"
#include "GlbGlobeRObject.h"

namespace GlbGlobe
{
	/**
	* @brief 符号基类				 
	* @version 1.0
	* @author  ChenPeng
	* @date    2014-5-8 11:30
	*/
	class GLB_SYMBOLDLL_CLASSEXPORT CGlbGlobeSymbol : public CGlbReference
	{
	public:
		/**
		* @brief 构造函数
		*/
		CGlbGlobeSymbol(void);
		/**
		* @brief 析构函数
		*/
		virtual ~CGlbGlobeSymbol(void);
		/**
		* @brief 符号渲染
		* @param[in] obj 渲染对象
		* @return 对象节点
		*/
		virtual osg::Node *Draw(CGlbGlobeRObject *obj,IGlbGeometry *geo);
		/**
		* @brief AGG二维绘制到像素
		* @param[in] obj 渲染对象
		* @param[in] geom 要渲染的几何体
		* @param[in] image 底图缓冲区
		* @param[in] imageW 底图的宽
		* @param[in] imageH 底图的高
		* @param[in] ext 地形块对应的范围
		*/
		virtual void DrawToImage(CGlbGlobeRObject *obj,IGlbGeometry* geom,
			glbByte *image/*RGBA*/,glbInt32 imageW,
			glbInt32 imageH,CGlbExtent &ext);
		/**
		* @brief 贴地形绘制对象的二维轮廓线 单位：地理单位
		* @return  对象的2D轮廓
		*/
		virtual IGlbGeometry *GetOutline(CGlbGlobeRObject *obj,IGlbGeometry *geo);
	protected:
		glbref_ptr<IGlbGeometry> mpt_outline;///<贴地形绘制对象的二维轮廓线 单位：地理单位
	};
}