/********************************************************************
* Copyright (c) 2014 北京超维创想信息技术有限公司
* All rights reserved.
* @file    GlbGlobeLineArrowSymbol.h
* @brief   箭头线符号头文件，文档定义CGlbGlobeLineArrowSymbol类
* @version 1.0
* @author  敖建
* @date    2014-8-7 11:13
*********************************************************************/

#pragma once
#include "StdAfx.h"
#include "glbglobesymbol.h"
#include "GlbGlobeCustomShapeDrawable.h"
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
	* @brief 箭头线符号类				 
	* @version 1.0
	* @author  敖建
	* @date    2014-8-7 11:13
	*/
	class GLB_SYMBOLDLL_CLASSEXPORT CGlbGlobeLineArrowSymbol : public CGlbGlobeSymbol
	{
	public:
		/**
		* @ brief 构造函数
		*/
		CGlbGlobeLineArrowSymbol(void);
		/**
		* @ brief 析构函数
		*/
		virtual ~CGlbGlobeLineArrowSymbol(void);
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
		IGlbGeometry *DealOutline(osg::Vec3d centerPosition,
			GlbLineArrowSymbolInfo *lineArrowInfo,CGlbFeature *feature,GlbGlobeTypeEnum globeType);
		void DealDraw(osg::Shape *shape,osg::ShapeDrawable *shapeFillDrawable,osg::ShapeDrawable *shapeOutDrawable,GlbLineArrowSymbolInfo *lineArrowInfo,CGlbFeature *feature);
		void DealAggRender(renb_type &renb,CGlbGlobeRObject *obj,IGlbGeometry* geom,glbInt32 imageW,glbInt32 imageH,CGlbExtent &ext);
		void FillDrawVertexArray(agg::path_storage &ps,IGlbGeometry *outline);
		void DealAggTexture(agg::rasterizer_scanline_aa<> &ras,	renb_type &renb,IGlbGeometry* geom,GlbLineArrowSymbolInfo *lineArrowInfo,CGlbFeature *feature,glbBool &isRenderTex);

		IGlbGeometry *DealNoRotOutline(osg::Vec3d centerPosition,GlbLineArrowSymbolInfo *lineArrowInfo,CGlbFeature *feature,GlbGlobeTypeEnum globeType);

		void DealAggRepeatTexture(agg::rasterizer_scanline_aa<> &ras,renb_type &renb,IGlbGeometry* geom,GlbLineArrowSymbolInfo *lineArrowInfo,CGlbFeature *feature,glbBool &isRenderTex,glbFloat tilingU,glbFloat tilingV);
		void DealAggWrapTexture(agg::rasterizer_scanline_aa<> &ras,renb_type &renb,IGlbGeometry* geom,GlbLineArrowSymbolInfo *lineArrowInfo,CGlbFeature *feature,glbBool &isRenderTex);

	private:
		CGlbExtent tempExtent;
		glbInt32 tempImageW;
		glbInt32 tempImageH;
	};
}