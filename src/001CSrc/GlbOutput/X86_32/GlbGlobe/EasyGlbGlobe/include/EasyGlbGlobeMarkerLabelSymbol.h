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
		* @brief ������Ⱦ
		* @param[in] obj ��Ⱦ����
		* @return ����ڵ�
		*/
		osg::Node *Draw(CEasyGlbGlobeRObject *obj,IGlbGeometry *geo);
		/**
		* @brief AGG��ά���Ƶ�����
		* @param[in] obj ��Ⱦ����
		* @param[in] geom Ҫ��Ⱦ�ļ�����
		* @param[in] image ��ͼ������
		* @param[in] imageW ��ͼ�Ŀ�
		* @param[in] imageH ��ͼ�ĸ�
		* @param[in] ext ���ο��Ӧ�ķ�Χ
		*/
		void DrawToImage(CEasyGlbGlobeRObject *obj,IGlbGeometry* geom,
			glbByte *image/*RGBA*/,glbInt32 imageW,
			glbInt32 imageH,CGlbExtent &ext,glbBool isDirectDraw);
		/**
		* @brief �����λ��ƶ���Ķ�ά������ ��λ������λ
		* @return  �����2D����
		*/
		IGlbGeometry *GetOutline(CEasyGlbGlobeRObject *obj,IGlbGeometry *geo);

		//��ɹ��еģ���object��������extent
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
		//����labelʵ������
		//osg::ref_ptr<osg::Image> mpr_clampImage;
		glbBool mpr_isComputeOutline;
		glbDouble mpr_halfWidth;
		glbDouble mpr_halfHeight;

		//��¼�������壬����create_font�Ĵ���
		CGlbWString mpr_textFont;
		glbBool mpr_isDirectDraw;
		agg::font_engine_win32_tt_int16 *mpr_font;

	};
}


