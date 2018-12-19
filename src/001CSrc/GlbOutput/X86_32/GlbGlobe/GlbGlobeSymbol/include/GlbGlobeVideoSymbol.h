/********************************************************************
* Copyright (c) 2014 北京超维创想信息技术有限公司
* All rights reserved.
* @file    GlbGlobeVideoSymbol.h
* @brief   投影符号头文件，文档定义CGlbGlobeVideoSymbol类
* @version 1.0
* @author  敖建
* @date    2014-10-10 14:55
*********************************************************************/
#pragma once
#include "StdAfx.h"
#include "GlbPolygon.h"
#include "glbglobesymbol.h"
#include "GlbGlobeAutoTransform.h"

/* note
	目前支持 10个贴地视频同时绘制，如果是多个贴地视频有重合区域从1-10依次混合叠加
//*/
namespace GlbGlobe
{
	/**
	* @brief 投影符号类				 
	* @version 1.0
	* @author  敖建
	* @date    2014-10-10 14:55
	*/
	class GLB_SYMBOLDLL_CLASSEXPORT CGlbGlobeVideoSymbol : public CGlbGlobeSymbol
	{
	public:
		/**
		* @ brief 构造函数
		*/
		CGlbGlobeVideoSymbol(void);
		/**
		* @ brief 析构函数
		*/
		~CGlbGlobeVideoSymbol(void);
		/**
		* @brief 模型符号渲染
		* @param[in] obj 渲染对象
		* @return 对象节点
		*/
		osg::Node *Draw(CGlbGlobeRObject *obj,IGlbGeometry *geo);
		/**
		* @brief AGG二维绘制到像素
		* @param[in] obj 渲染对象
		* @param[in] geom 渲染几何体
		* @param[in] image 底图缓冲区
		* @param[in] imageW 底图的宽
		* @param[in] imageH 底图的高
		* @param[in] ext 地形块对应的范围
		*/
		void DrawToImage(CGlbGlobeRObject *obj,IGlbGeometry* geom,	glbByte *image/*RGBA*/,glbInt32 imageW,glbInt32 imageH,CGlbExtent &ext);
		/**
		* @brief 贴地形绘制对象的二维轮廓线 单位：地理单位
		* @return  对象的2D轮廓
		*/
		IGlbGeometry *GetOutline(CGlbGlobeRObject *obj,IGlbGeometry *geo);

	private:
		/**
		* @brief 视频投影到平面模型符号渲染
		* @param[in] obj 渲染对象
		* @return 对象节点
		*/
		osg::Node *DrawBillBoardVideo(CGlbGlobeRObject *obj,CGlbPolygon *polygon);
		/**
		* @brief 视频投影到地形模型符号渲染
		* @param[in] obj 渲染对象
		* @return 对象节点
		*/
		osg::Node *DrawTerrainVideo(CGlbGlobeRObject *obj,CGlbPolygon *polygon);

		osg::Camera *initOverLayData(osg::Node *overLayNode,CGlbGlobe *globe);

		glbBool createShaders(osg::StateSet *ss,osg::Camera *camera,glbInt32 videoIndex);
	};
}

