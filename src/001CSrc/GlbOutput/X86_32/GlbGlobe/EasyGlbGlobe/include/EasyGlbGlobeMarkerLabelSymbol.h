#pragma once
#include "EasyGlbGlobeSymbol.h"
#include "EasyGlbGlobeAutoTransform.h"
#include "../font_win32_tt/agg_font_win32_tt.h"
//AGG
#include "agg_rendering_buffer.h"
#include "agg_rasterizer_scanline_aa.h"
#include "agg_scanline_p.h"
#include "agg_path_storage.h"
#include "agg_conv_stroke.h"
#include "agg_vcgen_markers_term.h"
#include "agg_renderer_scanline.h"
#include "agg_conv_contour.h"

#include "osg/Geometry"

namespace GlbGlobe
{
	class GLB_DLLCLASS_EXPORT CEasyGlbGlobeMarkerLabelSymbol : public CEasyGlbGlobeSymbol
	{
	public:
		CEasyGlbGlobeMarkerLabelSymbol(void);
		~CEasyGlbGlobeMarkerLabelSymbol(void);
/**
		* @brief 符号渲染
		* @param[in] obj 渲染对象
		* @return 对象节点
		*/
		osg::Node *Draw(CEasyGlbGlobeRObject *obj,IGlbGeometry *geo);
		/**
		* @brief AGG二维绘制到像素
		* @param[in] obj 渲染对象
		* @param[in] geom 要渲染的几何体
		* @param[in] image 底图缓冲区
		* @param[in] imageW 底图的宽
		* @param[in] imageH 底图的高
		* @param[in] ext 地形块对应的范围
		*/
		void DrawToImage(CEasyGlbGlobeRObject *obj,IGlbGeometry* geom,
			glbByte *image/*RGBA*/,glbInt32 imageW,
			glbInt32 imageH,CGlbExtent &ext,glbBool isDirectDraw);
		/**
		* @brief 贴地形绘制对象的二维轮廓线 单位：地理单位
		* @return  对象的2D轮廓
		*/
		IGlbGeometry *GetOutline(CEasyGlbGlobeRObject *obj,IGlbGeometry *geo);

		//提成公有的，供object计算贴地extent
		IGlbGeometry *DealOutline(osg::Vec2d centerPosition,
			GlbMarkerLabelSymbolInfo *markerInfo,CEasyGlbGlobeRObject *obj);
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
		void DealAggRender(agg::rendering_buffer rbuf,CEasyGlbGlobeRObject *obj,IGlbGeometry *geom,
			glbInt32 imageW,glbInt32 imageH,CGlbExtent &ext);
		osg::ref_ptr<osg::Vec2dArray> GetPointFromGeom(IGlbGeometry *geom);
		void FillDrawVertexArray(agg::path_storage &ps,IGlbGeometry *outline);
		void DealAggWrapTexture(agg::rendering_buffer rbuf,CEasyGlbGlobeRObject *obj,
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


