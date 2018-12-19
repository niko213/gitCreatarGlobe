/********************************************************************
* Copyright (c) 2014 北京超维创想信息技术有限公司
* All rights reserved.
* @file    CGlbGlobeMarkerLabelSymbol.h
* @brief   文本符号头文件，文档定义CGlbGlobeMarkerLabelSymbol类
* @version 1.0
* @author  ChenPeng
* @date    2014-5-20 11:15
*********************************************************************/
#pragma once
#include "glbglobesymbol.h"
#include "GlbGlobeMarker2DShapeSymbol.h"
#include "GlbGlobeAutoTransform.h"
#include "../font_win32_tt/agg_font_win32_tt.h"

namespace GlbGlobe
{
	/**
	* @brief 文本符号类				 
	* @version 1.0
	* @author  ChenPeng
	* @date    2014-5-30 17:40
	*/
	class GLB_SYMBOLDLL_CLASSEXPORT CGlbGlobeMarkerLabelSymbol : public CGlbGlobeSymbol
	{
	public:
		/**
		* @brief 构造函数
		*/
		CGlbGlobeMarkerLabelSymbol(void);
		/**
		* @brief 析构函数
		*/
		~CGlbGlobeMarkerLabelSymbol(void);
		/**
		* @brief 符号渲染
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
		void DrawToImage(CGlbGlobeRObject *obj,IGlbGeometry* geom,
			glbByte *image/*RGBA*/,glbInt32 imageW,
			glbInt32 imageH,CGlbExtent &ext,glbBool isDirectDraw);
		/**
		* @brief 贴地形绘制对象的二维轮廓线 单位：地理单位
		* @return  对象的2D轮廓
		*/
		IGlbGeometry *GetOutline(CGlbGlobeRObject *obj,IGlbGeometry *geo);

		//提成公有的，供object计算贴地extent
		IGlbGeometry *DealOutline(osg::Vec2d centerPosition,
			GlbMarkerLabelSymbolInfo *markerInfo,CGlbGlobeRObject *obj);
	private:
		void DealImage(osg::Vec2d leftUp,osg::Vec2d rightDown,
			GlbGlobeLabelRelationEnum labelRelation,osg::Image *image,
			renb_type &renb,unsigned char * buffer,GlbRenderImageInfo *imageInfo,CGlbFeature *feature);
		void DealText(osg::Vec2d leftUp,CGlbWString content,renb_type &renb,
			GlbRenderTextInfo *textInfo,CGlbFeature *feature);
		std::vector<CGlbWString> DealMultilineAlignText(GlbRenderTextInfo *textRenderInfo,
			CGlbFeature *feature,glbDouble &bytes);
		void DealComputeBufferSize(glbInt32 multilineSize,glbInt32 textSize,CGlbWString textFont,
			CGlbWString content,glbInt32 bytes,GlbGlobeLabelRelationEnum labelRelation,
			GlbGlobeLabelAlignTypeEnum labelAlign,glbBool isItalic,
			osg::Image *tempImage,glbInt32 &sizeWidth,glbInt32 &sizeHeight,
			glbDouble &simplelineX,glbDouble &simplelineY,glbInt32 imageMaxSize);
		void DealRenderImageAndText(glbBool isEmpty,std::vector<CGlbWString> wsRenderContainer,
			CGlbWString content,glbDouble &simplelineX,glbInt32 bytes,glbInt32 textSize,
			osg::Image *tempImage,GlbRenderImageInfo *imageInfo,GlbRenderTextInfo *textInfo,
			GlbGlobeLabelRelationEnum labelRelation,GlbGlobeLabelAlignTypeEnum labelAlign,
			glbInt32 sizeWidth,glbInt32 sizeHeight,renb_type &renb,unsigned char * buffer,CGlbFeature *feature);
		osg::Geometry *CreateOutlineQuad(osg::Vec3 cornerVec,osg::Vec3 widthVec,osg::Vec3 heightVec);
	private:
		void DealAggRender(agg::rendering_buffer rbuf,CGlbGlobeRObject *obj,IGlbGeometry *geom,
			glbInt32 imageW,glbInt32 imageH,CGlbExtent &ext);
		osg::ref_ptr<osg::Vec2dArray> GetPointFromGeom(IGlbGeometry *geom);
		void FillDrawVertexArray(agg::path_storage &ps,IGlbGeometry *outline);
		void DealAggWrapTexture(agg::rendering_buffer rbuf,CGlbGlobeRObject *obj,
			GlbMarkerLabelSymbolInfo *markerInfo,CGlbFeature *feature,IGlbGeometry *outline);
	private:
		CGlbExtent tempExtent;
		glbInt32 tempImageW;
		glbInt32 tempImageH;
		glbref_ptr<CGlbExtent> cglbExtent;
		//处理label实现贴地
		//osg::ref_ptr<osg::Image> mpr_clampImage;
		glbBool mpr_isComputeOutline;
		glbDouble mpr_halfWidth;
		glbDouble mpr_halfHeight;

		//记录文字字体，减少create_font的次数
		CGlbWString mpr_textFont;
		glbBool mpr_isDirectDraw;
		agg::font_engine_win32_tt_int16 *mpr_font;
	};
}