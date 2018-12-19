/********************************************************************
* Copyright (c) 2014 北京超维创想信息技术有限公司
* All rights reserved.
* @file    CGlbGlobeMarkerDynamicLabelSymbol.h
* @brief   动态文本符号头文件，文档定义CGlbGlobeMarkerDynamicLabelSymbol类
* @version 1.0
* @author  ChenPeng
* @date    2016-5-24 14:10
*********************************************************************/
#pragma once
#include "glbglobesymbol.h"
#include "GlbGlobeMarker2DShapeSymbol.h"
#include "GlbGlobeAutoTransform.h"
#include "../font_win32_tt/agg_font_win32_tt.h"

namespace GlbGlobe
{
	/**
	* @brief 动态文本符号类				 
	* @version 1.0
	* @author  ChenPeng
	* @date    2014-5-30 17:40
	*/
	class GLB_SYMBOLDLL_CLASSEXPORT CGlbGlobeMarkerDynamicLabelSymbol : public CGlbGlobeSymbol
	{
	public:
		/**
		* @brief 构造函数
		*/
		CGlbGlobeMarkerDynamicLabelSymbol(void);
		/**
		* @brief 析构函数
		*/
		~CGlbGlobeMarkerDynamicLabelSymbol(void);
		/**
		* @brief 符号渲染
		* @param[in] obj 渲染对象
		* @return 对象节点
		*/
		osg::Node *Draw(CGlbGlobeRObject *obj,IGlbGeometry *geo);
		/**
		* @brief 获取绘制文字的buffer
		* @return 文字buffer
		*/
		unsigned char *GetAggBuffer();
		/**
		* @brief 获取背景图片的buffer
		* @return 背景图片buffer
		*/
		osg::Image *GetTexImage();
		/**
		* @brief 获取文字buffer的宽高
		* @return 文字buffer的宽高
		*/
		osg::Vec2i GetAggBufferWidthAndHeight();
	private:
		void DealImage(osg::Vec2d leftUp,osg::Vec2d rightDown,osg::Image *image,
			renb_type &renb,unsigned char * buffer,GlbRenderImageInfo *imageInfo,CGlbFeature *feature);

		void DealText(osg::Vec2d leftUp,CGlbWString content,renb_type &renb,
			GlbRenderTextInfo *textInfo,CGlbFeature *feature);
		
		std::vector<CGlbWString> DealMultilineAlignText(CGlbWString content,
			GlbGlobeMultilineAlignTypeEnum multilineAlign,CGlbWString textFont,glbInt32 textSize,glbDouble &bytes);

		void DealRenderImageAndText(std::vector<CGlbWString> wsRenderContainer,
			glbInt32 customMultilineSize,CGlbWString content,glbInt32 bytes,glbInt32 textSize,
			osg::Image *tempImage,GlbRenderImageInfo *imageInfo,GlbRenderTextInfo *textInfo,
			glbInt32 sizeWidth,glbInt32 sizeHeight,renb_type &renb,unsigned char * buffer,CGlbFeature *feature);
		
		osg::Geometry *CreateOutlineQuad(osg::Vec3 cornerVec,osg::Vec3 widthVec,osg::Vec3 heightVec);
		
		void DealBufferSizeExtend(glbInt32 textSize,glbBool isBold,glbBool isItalic,
			glbBool isUnderline,osg::Image *image,glbDouble simplelineX,
			glbDouble simplelineY,glbInt32 &sizeWidth,glbInt32 &sizeHeight);

		void DealExtendImageByMaxSize(GlbRenderImageInfo *imageInfo,CGlbFeature *feature,glbInt32 &sizeWidth,glbInt32 &sizeHeight);
	private:
		glbref_ptr<CGlbGlobeRObject> mpr_rObj;
		GlbGlobeDynamicLabelFadeTextEnum mpr_fadeMode;
		unsigned char *mpr_aggBuffer;
		osg::ref_ptr<osg::Image> mpr_texImage;
		osg::Vec2i                             mpr_bufferSize;
	};
}