/********************************************************************
* Copyright (c) 2014 北京超维创想信息技术有限公司
* All rights reserved.
* @file    GlbGlobeDigHoleSymbol.h
* @brief   挖洞符号头文件，文档定义CGlbGlobeDigHoleSymbol类
* @version 1.0
* @author  敖建
* @date    2014-8-29 16:55
*********************************************************************/
#pragma once
#include "stdafx.h"
#include "glbglobesymbol.h"
//AGG
#include "agg_rendering_buffer.h"
#include "agg_rasterizer_scanline_aa.h"
#include "agg_scanline_p.h"
#include "agg_path_storage.h"
#include "agg_conv_stroke.h"
#include "agg_vcgen_markers_term.h"
#include "agg_renderer_scanline.h"
#include "agg_conv_contour.h"

namespace GlbGlobe
{
	/**
	* @brief CGlbGlobeDigHoleSymbol挖洞符号类				 
	* @version 1.0
	* @author  敖建
	* @date    2014-8-29 16:55
	*/
	class GLB_SYMBOLDLL_CLASSEXPORT CGlbGlobeDigHoleSymbol : public CGlbGlobeSymbol
	{
	public:
		/**
		* @ brief 构造函数
		*/
		CGlbGlobeDigHoleSymbol(void);
		/**
		* @ brief 析构函数
		*/
		virtual ~CGlbGlobeDigHoleSymbol(void);
		/**
		* @brief 模型符号渲染
		* @param[in] obj 渲染对象
		* @return 对象节点
		*/
		osg::Node *Draw(CGlbGlobeRObject *obj,IGlbGeometry *geo,glbDouble depth,glbBool isShowWall);
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
		void DealAggRender(renb_type &renb,CGlbGlobeRObject *obj,IGlbGeometry* geom,glbInt32 imageW,glbInt32 imageH,CGlbExtent &ext);
		void FillDrawVertexArray(agg::path_storage &ps,IGlbGeometry *outline);
	private:
		//判断点（px,py）是否在多边形polygon内 【polygon_pts 多边形点集， ptCnt 多边形点数】 目前只支持单个环的多边形
		bool PtInPolygon(double px, double py, double* polygon_pts, int ptCnt);
	private:
		CGlbExtent tempExtent;
		glbInt32 tempImageW;
		glbInt32 tempImageH;
		agg::renderer_base<agg::pixfmt_rgba32> dd;
	};
}