#pragma once
#include "EasyGlbGlobeSymbol.h"
//AGG
#include "agg_rendering_buffer.h"
#include "agg_rasterizer_scanline_aa.h"
#include "agg_scanline_p.h"
#include "agg_path_storage.h"
#include "agg_conv_stroke.h"
#include "agg_vcgen_markers_term.h"
#include "agg_renderer_scanline.h"
#include "agg_conv_contour.h"

#include "geos.h"

namespace GlbGlobe
{
	class GLB_DLLCLASS_EXPORT CEasyGlbGlobeLinePixelSymbol : public CEasyGlbGlobeSymbol
	{
	public:
		CEasyGlbGlobeLinePixelSymbol(void);
		~CEasyGlbGlobeLinePixelSymbol(void);
		/**
		* @brief ģ�ͷ�����Ⱦ
		* @param[in] obj ��Ⱦ����
		* @return ����ڵ�
		*/
		osg::Node *Draw(CEasyGlbGlobeRObject *obj,IGlbGeometry *geo);
		/**
		* @brief AGG��ά���Ƶ�����
		* @param[in] obj ��Ⱦ����
		* @param[in] geom ��Ⱦ������
		* @param[in] image ��ͼ������
		* @param[in] imageW ��ͼ�Ŀ�
		* @param[in] imageH ��ͼ�ĸ�
		* @param[in] ext ���ο��Ӧ�ķ�Χ
		*/
		void DrawToImage(CEasyGlbGlobeRObject *obj,IGlbGeometry* geom,	glbByte *image/*RGBA*/,glbInt32 imageW,glbInt32 imageH,CGlbExtent &ext);

		static osg::Node *DrawBorderline(CEasyGlbGlobeRObject *obj,IGlbGeometry *geo,GlbRenderInfo *renderInfo);

		/**
		* @brief �����λ��ƶ���Ķ�ά������ ��λ������λ
		* @return  �����2D����
		*/
		IGlbGeometry *GetOutline(CEasyGlbGlobeRObject *obj,IGlbGeometry *geo);

	private:
		void DealAggRender(renb_type &renb,CEasyGlbGlobeRObject *obj,IGlbGeometry* geom, glbInt32 imageW,glbInt32 imageH,CGlbExtent &ext);
		void FillDrawVertexArray(agg::path_storage &ps,IGlbGeometry *outline);
		bool ReCalPoint(agg::path_storage &ps,osg::Vec2d& point, osg::Vec2d& nextpoint, CGlbExtent& ext);

		bool Intersect(osg::Vec2d v0,osg::Vec2d v1,osg::Vec2d v2,osg::Vec2d v3,osg::Vec2d& inter);
		double Cross(osg::Vec2d v0,osg::Vec2d v1);

		double mult(osg::Vec2d a, osg::Vec2d b, osg::Vec2d c);
		bool intersect(osg::Vec2d aa, osg::Vec2d bb, osg::Vec2d cc, osg::Vec2d dd);  

		geos::geom::Geometry* Interection(IGlbGeometry *geom, CGlbExtent &ext);
		void FillDrawVertexArray(agg::path_storage &ps,geos::geom::Geometry* outline);
	private:
		CGlbExtent tempExtent;
		glbInt32 tempImageW;
		glbInt32 tempImageH;
	};
}


