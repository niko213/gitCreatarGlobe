#include "StdAfx.h"
#include "GlbGlobeLineArrowSymbol.h"
#include "GlbLine.h"
#include <osg/Geode>
#include <osg/Geometry>
#include "CGlbGlobe.h"
#include <osg/LineStipple>
#include <osgDB/ReadFile>
#include <osg/TexMat>
#include <osg/BlendFunc>
#include <osg/PolygonOffset>
#include <osg/BoundingBox>
#include <osg/TexGen>
#include <osg/TexEnv>
#include <osg/ShapeDrawable>
#include <osg/PolygonMode>
#include <osg/Material>
#include <osg/TextureCubeMap>
#include <osg/LineWidth>

//AGG
#include "agg_scanline_u.h"
#include "agg_pixfmt_rgb.h"
#include "agg_span_allocator.h"
#include <agg_span_image_filter_rgba.h>
#include "agg_span_interpolator_linear.h"
#include "agg_span_gradient_alpha.h"
#include "agg_span_image_filter_rgb.h"
#include "agg_span_converter.h"
#include "agg_image_accessors.h"
#include <agg_trans_warp_magnifier.h>
#include "agg_span_interpolator_trans.h"
#include  <agg_conv_dash.h>  //  conv_dash

//ceshi
#include "platform/win32/agg_win32_bmp.h"
#include "GlbConvert.h"
#include "comutil.h"
#include "GlbLog.h"

#include "util/agg_color_conv.h"
#include "util/agg_color_conv_rgb8.h"
#include "platform/agg_platform_support.h"
using namespace GlbGlobe;

CGlbGlobeLineArrowSymbol::CGlbGlobeLineArrowSymbol(void)
{
}

CGlbGlobeLineArrowSymbol::~CGlbGlobeLineArrowSymbol(void)
{
}

osg::Node* CGlbGlobeLineArrowSymbol::Draw(CGlbGlobeRObject *obj,IGlbGeometry *geo)
{
	IGlbGeometry *geometry = geo;
	CGlbLine *line3D = dynamic_cast<CGlbLine*>(geometry);
	if (line3D == NULL)
		return NULL;
	CGlbFeature *feature = obj->GetFeature();
	GlbRenderInfo *renderInfo = obj->GetRenderInfo();
	glbref_ptr<GlbLineArrowSymbolInfo> lineArrowInfo = dynamic_cast<GlbLineArrowSymbolInfo*>(renderInfo);

	//
	osg::Geode *geode = new osg::Geode;
	osg::ref_ptr<osg::Geometry> bordergeometry = new osg::Geometry;	//外边界线
	osg::ref_ptr<osg::Geometry> fillgeometry = new osg::Geometry;	//填充面
	osg::ref_ptr<osg::Vec3Array> vertexes = new osg::Vec3Array;
	//箭头包含2个点
	glbInt32 count = line3D->GetCount();
	if (count != 2)
		return NULL;
	osg::Vec3d pointfirst,pointlast;
	line3D->GetPoint(0,&pointfirst.x(),&pointfirst.y(),&pointfirst.z());
	line3D->GetPoint(1,&pointlast.x(),&pointlast.y(),&pointlast.z());
	osg::Vec3d pointdelt = pointlast - pointfirst;	//用来计算箭头姿态
	glbDouble Arrowlength = pointdelt.length();
	if (obj->GetGlobe()->GetType() == GLB_GLOBETYPE_GLOBE)
	{
		g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(pointfirst.y()),osg::DegreesToRadians(pointfirst.x()),pointfirst.z(),pointfirst.x(),pointfirst.y(),pointfirst.z());
		g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(pointlast.y()),osg::DegreesToRadians(pointlast.x()),pointlast.z(),pointlast.x(),pointlast.y(),pointlast.z());
		pointdelt = pointlast - pointfirst;
		Arrowlength = pointdelt.length();
	}
	glbDouble Arrowwidth = 1.0;	//宽度
	glbDouble Arrowheight = 1.0;	//厚度
	glbInt32 fillcolor = 0;		//填充颜色
	glbInt32 linecolor = 0;		//线颜色
	glbInt32 lineOpacity = 0;	//先透明度
	glbInt32 fillOpacity = 0;	//填充透明度
	CGlbWString textureData = L"";	//纹理路径
	GlbGlobeTexRepeatModeEnum texRepeatMode = GLB_TEXTUREREPEAT_TIMES;	//纹理平铺方式
	glbDouble tilingU = 1.0;	
	glbDouble tilingV = 1.0;
	glbDouble textureRotation = 0.0;
	GlbGlobeLinePatternEnum lineStyle = GLB_LINE_SOLID;	//线形
	glbDouble linewith = 1.0;
	if (lineArrowInfo->linePixelInfo)
	{
		if (lineArrowInfo->linePixelInfo->lineColor)
			linecolor = lineArrowInfo->linePixelInfo->lineColor->GetValue(feature);
		if (lineArrowInfo->linePixelInfo->lineOpacity)
			lineOpacity = lineArrowInfo->linePixelInfo->lineOpacity->GetValue(feature);
		if (lineArrowInfo->linePixelInfo->lineWidth)
			linewith = lineArrowInfo->linePixelInfo->lineWidth->GetValue(feature);
		if (lineArrowInfo->linePixelInfo->linePattern)
			lineStyle = (GlbGlobeLinePatternEnum)lineArrowInfo->linePixelInfo->linePattern->GetValue(feature);
	}
	if (lineArrowInfo->width)
		Arrowwidth = lineArrowInfo->width->GetValue(feature);
	if (lineArrowInfo->height)
		Arrowheight = lineArrowInfo->height->GetValue(feature);
	if (lineArrowInfo->fillColor)
		fillcolor = lineArrowInfo->fillColor->GetValue(feature);
	if (lineArrowInfo->fillOpacity)
		fillOpacity = lineArrowInfo->fillOpacity->GetValue(feature);
	if (lineArrowInfo->textureData)
	{
		textureData = lineArrowInfo->textureData->GetValue(feature);
		if (lineArrowInfo->texRepeatMode)
			texRepeatMode = (GlbGlobeTexRepeatModeEnum)lineArrowInfo->texRepeatMode->GetValue(feature);
		if (lineArrowInfo->tilingU)
			tilingU = lineArrowInfo->tilingU->GetValue(feature);
		if (lineArrowInfo->tilingV)
			tilingV = lineArrowInfo->tilingV->GetValue(feature);
		if (lineArrowInfo->textureRotation)
			textureRotation = lineArrowInfo->textureRotation->GetValue(feature);
	}
	//确定几何体顶点数组
	switch(lineArrowInfo->pattern->GetValue(feature))
	{
	case GLB_ARROWPATTERN_D4:	//7个点，一条闭合折线
		{
			if (Arrowheight <= 0.0)
			{
				glbDouble Arrow_y = min(Arrowlength*0.66,abs(abs(Arrowlength-Arrowwidth)));
				vertexes->push_back(osg::Vec3d(0.0,Arrowlength,0.0));
				vertexes->push_back(osg::Vec3d(Arrowwidth/2.0,Arrow_y,0.0));
				vertexes->push_back(osg::Vec3d(Arrowwidth/6.0,Arrow_y,0.0));
				vertexes->push_back(osg::Vec3d(Arrowwidth/6.0,0.0,0.0));
				vertexes->push_back(osg::Vec3d(-Arrowwidth/6.0,0.0,0.0));			
				vertexes->push_back(osg::Vec3d(-Arrowwidth/6.0,Arrow_y,0.0));			
				vertexes->push_back(osg::Vec3d(-Arrowwidth/2.0,Arrow_y,0.0));		
				bordergeometry->setVertexArray(vertexes.get());
				fillgeometry->setVertexArray(vertexes.get());

				osg::ref_ptr<osg::Vec3Array> nomalvertex = new osg::Vec3Array;
				nomalvertex->push_back(osg::Vec3(0.0,0.0,1.0));
				bordergeometry->setNormalArray(nomalvertex.get());
				bordergeometry->setNormalBinding(osg::Geometry::BIND_OVERALL);
				fillgeometry->setNormalArray(nomalvertex.get());
				fillgeometry->setNormalBinding(osg::Geometry::BIND_OVERALL);

				osg::ref_ptr<osg::Vec4Array> linecolorArray = new osg::Vec4Array;
				linecolorArray->push_back(osg::Vec4(GetBValue(linecolor)/255.0,LOBYTE(((linecolor & 0xFFFF)) >> 8)/255.0,GetRValue(linecolor)/255.0,(glbFloat)lineOpacity/100.0));
				bordergeometry->setColorArray(linecolorArray);
				bordergeometry->setColorBinding(osg::Geometry::BIND_OVERALL);
				osg::ref_ptr<osg::Vec4Array> fillcolorArray = new osg::Vec4Array;
				fillcolorArray->push_back(osg::Vec4(GetBValue(fillcolor)/255.0,LOBYTE(((fillcolor & 0xFFFF)) >> 8)/255.0,GetRValue(fillcolor)/255.0,(glbFloat)fillOpacity/100.0));
				fillgeometry->setColorArray(fillcolorArray);
				fillgeometry->setColorBinding(osg::Geometry::BIND_OVERALL);

				osg::ref_ptr<osg::DrawElementsUInt> indexArray = new osg::DrawElementsUInt(osg::DrawElementsUInt::LINE_LOOP,0);
				indexArray->push_back(0);
				indexArray->push_back(1);
				indexArray->push_back(2);
				indexArray->push_back(3);
				indexArray->push_back(4);
				indexArray->push_back(5);
				indexArray->push_back(6);
				bordergeometry->addPrimitiveSet(indexArray.get());

				osg::ref_ptr<osg::DrawElementsUInt> indexArray2 = new osg::DrawElementsUInt(osg::DrawElementsUInt::QUADS,0);
				indexArray2->push_back(4);
				indexArray2->push_back(3);
				indexArray2->push_back(2);
				indexArray2->push_back(5);
				osg::ref_ptr<osg::DrawElementsUInt> indexArray3 = new osg::DrawElementsUInt(osg::DrawElementsUInt::TRIANGLES,0);
				indexArray3->push_back(0);
				indexArray3->push_back(6);
				indexArray3->push_back(1);
				fillgeometry->addPrimitiveSet(indexArray2.get());
				fillgeometry->addPrimitiveSet(indexArray3.get());
			}else{
				glbDouble Arrow_y = min(Arrowlength*0.66,abs(Arrowlength-Arrowwidth));
				vertexes->push_back(osg::Vec3d(0.0,Arrowlength,0.0));
				vertexes->push_back(osg::Vec3d(Arrowwidth/2,Arrow_y,0.0));
				vertexes->push_back(osg::Vec3d(Arrowwidth/6,Arrow_y,0.0));
				vertexes->push_back(osg::Vec3d(Arrowwidth/6,0.0,0.0));
				vertexes->push_back(osg::Vec3d(-Arrowwidth/6,0.0,0.0));			
				vertexes->push_back(osg::Vec3d(-Arrowwidth/6,Arrow_y,0.0));			
				vertexes->push_back(osg::Vec3d(-Arrowwidth/2,Arrow_y,0.0));	
				vertexes->push_back(osg::Vec3d(0.0,Arrowlength,Arrowheight));
				vertexes->push_back(osg::Vec3d(Arrowwidth/2,Arrow_y,Arrowheight));
				vertexes->push_back(osg::Vec3d(Arrowwidth/6,Arrow_y,Arrowheight));
				vertexes->push_back(osg::Vec3d(Arrowwidth/6,0.0,Arrowheight));
				vertexes->push_back(osg::Vec3d(-Arrowwidth/6,0.0,Arrowheight));			
				vertexes->push_back(osg::Vec3d(-Arrowwidth/6,Arrow_y,Arrowheight));			
				vertexes->push_back(osg::Vec3d(-Arrowwidth/2,Arrow_y,Arrowheight));
				bordergeometry->setVertexArray(vertexes.get());
				fillgeometry->setVertexArray(vertexes.get());

				osg::ref_ptr<osg::Vec3Array> nomalvertex = new osg::Vec3Array;
				nomalvertex->push_back(osg::Vec3(0.0,1.0,-1.0));
				nomalvertex->push_back(osg::Vec3(0.707,0.293,-1.0));
				nomalvertex->push_back(osg::Vec3(1.0,-1.0,-1.0));
				nomalvertex->push_back(osg::Vec3(1.0,-1.0,-1.0));
				nomalvertex->push_back(osg::Vec3(-1.0,-1.0,-1.0));
				nomalvertex->push_back(osg::Vec3(-1.0,-1.0,-1.0));
				nomalvertex->push_back(osg::Vec3(-0.707,0.293,-1.0));
				nomalvertex->push_back(osg::Vec3(0.0,1.0,1.0));
				nomalvertex->push_back(osg::Vec3(0.707,0.293,1.0));
				nomalvertex->push_back(osg::Vec3(1.0,-1.0,1.0));
				nomalvertex->push_back(osg::Vec3(1.0,-1.0,1.0));
				nomalvertex->push_back(osg::Vec3(-1.0,-1.0,1.0));
				nomalvertex->push_back(osg::Vec3(-1.0,-1.0,1.0));
				nomalvertex->push_back(osg::Vec3(-0.707,0.293,1.0));
				bordergeometry->setNormalArray(nomalvertex.get());
				bordergeometry->setNormalBinding(osg::Geometry::BIND_PER_VERTEX);
				fillgeometry->setNormalArray(nomalvertex.get());
				fillgeometry->setNormalBinding(osg::Geometry::BIND_PER_VERTEX);
				osg::ref_ptr<osg::Vec4Array> linecolorArray = new osg::Vec4Array;
				linecolorArray->push_back(osg::Vec4(GetBValue(linecolor)/255.0,LOBYTE(((linecolor & 0xFFFF)) >> 8)/255.0,GetRValue(linecolor)/255.0,(glbFloat)lineOpacity/100.0));
				bordergeometry->setColorArray(linecolorArray);
				bordergeometry->setColorBinding(osg::Geometry::BIND_OVERALL);
				osg::ref_ptr<osg::Vec4Array> fillcolorArray = new osg::Vec4Array;
				fillcolorArray->push_back(osg::Vec4(GetBValue(fillcolor)/255.0,LOBYTE(((fillcolor & 0xFFFF)) >> 8)/255.0,GetRValue(fillcolor)/255.0,(glbFloat)fillOpacity/100.0));
				fillgeometry->setColorArray(fillcolorArray);
				fillgeometry->setColorBinding(osg::Geometry::BIND_OVERALL);

				osg::ref_ptr<osg::DrawElementsUInt> indexArray = new osg::DrawElementsUInt(osg::DrawElementsUInt::LINE_LOOP,0);
				indexArray->push_back(0);
				indexArray->push_back(1);
				indexArray->push_back(2);
				indexArray->push_back(3);
				indexArray->push_back(4);
				indexArray->push_back(5);
				indexArray->push_back(6);
				osg::ref_ptr<osg::DrawElementsUInt> indexArray1 = new osg::DrawElementsUInt(osg::DrawElementsUInt::LINE_LOOP,0);
				indexArray1->push_back(7);
				indexArray1->push_back(8);
				indexArray1->push_back(9);
				indexArray1->push_back(10);
				indexArray1->push_back(11);
				indexArray1->push_back(12);
				indexArray1->push_back(13);
				osg::ref_ptr<osg::DrawElementsUInt> indexArray2 = new osg::DrawElementsUInt(osg::DrawElementsUInt::LINES,0);
				indexArray2->push_back(0);indexArray2->push_back(7);			
				indexArray2->push_back(1);indexArray2->push_back(8);
				indexArray2->push_back(2);indexArray2->push_back(9);
				indexArray2->push_back(3);indexArray2->push_back(10);
				indexArray2->push_back(4);indexArray2->push_back(11);
				indexArray2->push_back(5);indexArray2->push_back(12);
				indexArray2->push_back(6);indexArray2->push_back(13);			
				bordergeometry->addPrimitiveSet(indexArray.get());
				bordergeometry->addPrimitiveSet(indexArray1.get());
				bordergeometry->addPrimitiveSet(indexArray2.get());

				osg::ref_ptr<osg::DrawElementsUInt> indexArray3 = new osg::DrawElementsUInt(osg::DrawElementsUInt::QUADS,0);
				//face1
				indexArray3->push_back(4);
				indexArray3->push_back(3);
				indexArray3->push_back(2);
				indexArray3->push_back(5);
				//face2
				indexArray3->push_back(11);
				indexArray3->push_back(12);
				indexArray3->push_back(9);
				indexArray3->push_back(10);
				osg::ref_ptr<osg::DrawElementsUInt> indexArray4 = new osg::DrawElementsUInt(osg::DrawElementsUInt::TRIANGLES,0);
				//face1
				indexArray4->push_back(0);
				indexArray4->push_back(6);
				indexArray4->push_back(1);
				//face2
				indexArray4->push_back(7);
				indexArray4->push_back(8);
				indexArray4->push_back(13);
				osg::ref_ptr<osg::DrawElementsUInt> indexArray5 = new osg::DrawElementsUInt(osg::DrawElementsUInt::QUAD_STRIP,0);
				indexArray5->push_back(7);indexArray5->push_back(0);			
				indexArray5->push_back(8);indexArray5->push_back(1);
				indexArray5->push_back(9);indexArray5->push_back(2);
				indexArray5->push_back(10);indexArray5->push_back(3);
				indexArray5->push_back(11);indexArray5->push_back(4);
				indexArray5->push_back(12);indexArray5->push_back(5);
				indexArray5->push_back(13);indexArray5->push_back(6);
				indexArray5->push_back(7);indexArray5->push_back(0);
				fillgeometry->addPrimitiveSet(indexArray3.get());
				fillgeometry->addPrimitiveSet(indexArray4.get());
				fillgeometry->addPrimitiveSet(indexArray5.get());
			}
		}
		break;
	case GLB_ARROWPATTERN_D5:	//8个点
		{
			if (Arrowheight <= 0.0)
			{
				//确定8个顶点坐标
				glbDouble Arrow_y = min(Arrowlength*0.66,abs(Arrowlength-Arrowwidth));
				vertexes->push_back(osg::Vec3d(0.0,0.0,0.0));
				vertexes->push_back(osg::Vec3d(-Arrowwidth/4,-Arrowwidth/4,0.0));
				vertexes->push_back(osg::Vec3d(-Arrowwidth/16,Arrowlength-(Arrowlength-Arrow_y)/2,0.0));
				vertexes->push_back(osg::Vec3d(-Arrowwidth/2,Arrow_y,0.0));
				vertexes->push_back(osg::Vec3d(0.0,Arrowlength,0.0));				
				vertexes->push_back(osg::Vec3d(Arrowwidth/2,Arrow_y,0.0));		
				vertexes->push_back(osg::Vec3d(Arrowwidth/16,Arrowlength-(Arrowlength-Arrow_y)/2,0.0));
				vertexes->push_back(osg::Vec3d(Arrowwidth/4,-Arrowwidth/4,0.0));
				bordergeometry->setVertexArray(vertexes.get());
				fillgeometry->setVertexArray(vertexes.get());

				osg::ref_ptr<osg::Vec3Array> nomalvertex = new osg::Vec3Array;
				nomalvertex->push_back(osg::Vec3(0.0,0.0,1.0));
				bordergeometry->setNormalArray(nomalvertex.get());
				bordergeometry->setNormalBinding(osg::Geometry::BIND_OVERALL);
				fillgeometry->setNormalArray(nomalvertex.get());
				fillgeometry->setNormalBinding(osg::Geometry::BIND_OVERALL);
				osg::ref_ptr<osg::Vec4Array> linecolorArray = new osg::Vec4Array;
				linecolorArray->push_back(osg::Vec4(GetBValue(linecolor)/255.0,LOBYTE(((linecolor & 0xFFFF)) >> 8)/255.0,GetRValue(linecolor)/255.0,(glbFloat)lineOpacity/100.0));
				bordergeometry->setColorArray(linecolorArray);
				bordergeometry->setColorBinding(osg::Geometry::BIND_OVERALL);
				osg::ref_ptr<osg::Vec4Array> fillcolorArray = new osg::Vec4Array;
				fillcolorArray->push_back(osg::Vec4(GetBValue(fillcolor)/255.0,LOBYTE(((fillcolor & 0xFFFF)) >> 8)/255.0,GetRValue(fillcolor)/255.0,(glbFloat)fillOpacity/100.0));
				fillgeometry->setColorArray(fillcolorArray);
				fillgeometry->setColorBinding(osg::Geometry::BIND_OVERALL);

				osg::ref_ptr<osg::DrawElementsUInt> indexArray = new osg::DrawElementsUInt(osg::DrawElementsUInt::LINE_LOOP,0);
				indexArray->push_back(0);
				indexArray->push_back(1);
				indexArray->push_back(2);
				indexArray->push_back(3);
				indexArray->push_back(4);
				indexArray->push_back(5);
				indexArray->push_back(6);
				indexArray->push_back(7);
				bordergeometry->addPrimitiveSet(indexArray.get());

				osg::ref_ptr<osg::DrawElementsUInt> indexArray2 = new osg::DrawElementsUInt(osg::DrawElementsUInt::TRIANGLES,0);	//6个三角形
				indexArray2->push_back(0);			indexArray2->push_back(2);			indexArray2->push_back(1);
				indexArray2->push_back(0);			indexArray2->push_back(6);			indexArray2->push_back(2);
				indexArray2->push_back(0);			indexArray2->push_back(7);			indexArray2->push_back(6);
				indexArray2->push_back(4);			indexArray2->push_back(3);			indexArray2->push_back(2);
				indexArray2->push_back(4);			indexArray2->push_back(2);			indexArray2->push_back(6);
				indexArray2->push_back(4);			indexArray2->push_back(6);			indexArray2->push_back(5);
				fillgeometry->addPrimitiveSet(indexArray2.get());
			}else{
				glbDouble Arrow_y = min(Arrowlength*0.66,abs(Arrowlength-Arrowwidth));
				vertexes->push_back(osg::Vec3d(0.0,0.0,0.0));
				vertexes->push_back(osg::Vec3d(-Arrowwidth/4,-Arrowwidth/4,0.0));
				vertexes->push_back(osg::Vec3d(-Arrowwidth/16,Arrowlength-(Arrowlength-Arrow_y)/2,0.0));
				vertexes->push_back(osg::Vec3d(-Arrowwidth/2,Arrow_y,0.0));
				vertexes->push_back(osg::Vec3d(0.0,Arrowlength,0.0));				
				vertexes->push_back(osg::Vec3d(Arrowwidth/2,Arrow_y,0.0));		
				vertexes->push_back(osg::Vec3d(Arrowwidth/16,Arrowlength-(Arrowlength-Arrow_y)/2,0.0));
				vertexes->push_back(osg::Vec3d(Arrowwidth/4,-Arrowwidth/4,0.0));
				vertexes->push_back(osg::Vec3d(0.0,0.0,Arrowheight));
				vertexes->push_back(osg::Vec3d(-Arrowwidth/4,-Arrowwidth/4,Arrowheight));
				vertexes->push_back(osg::Vec3d(-Arrowwidth/16,Arrowlength-(Arrowlength-Arrow_y)/2,Arrowheight));
				vertexes->push_back(osg::Vec3d(-Arrowwidth/2,Arrow_y,Arrowheight));
				vertexes->push_back(osg::Vec3d(0.0,Arrowlength,Arrowheight));				
				vertexes->push_back(osg::Vec3d(Arrowwidth/2,Arrow_y,Arrowheight));		
				vertexes->push_back(osg::Vec3d(Arrowwidth/16,Arrowlength-(Arrowlength-Arrow_y)/2,Arrowheight));
				vertexes->push_back(osg::Vec3d(Arrowwidth/4,-Arrowwidth/4,Arrowheight));
				bordergeometry->setVertexArray(vertexes.get());
				fillgeometry->setVertexArray(vertexes.get());

				osg::ref_ptr<osg::Vec3Array> nomalvertex = new osg::Vec3Array;
				nomalvertex->push_back(osg::Vec3(0.0,-1.0,-1.0));
				nomalvertex->push_back(osg::Vec3(-0.293,-0.707,-1.0));
				nomalvertex->push_back(osg::Vec3(-0.293,-0.707,-1.0));
				nomalvertex->push_back(osg::Vec3(0.0,0.0,-1.0));
				nomalvertex->push_back(osg::Vec3(0.0,1.0,-1.0));
				nomalvertex->push_back(osg::Vec3(0.0,0.0,-1.0));
				nomalvertex->push_back(osg::Vec3(0.293,-0.707,-1.0));
				nomalvertex->push_back(osg::Vec3(0.293,-0.707,-1.0));
				nomalvertex->push_back(osg::Vec3(0.0,-1.0,1.0));
				nomalvertex->push_back(osg::Vec3(-0.293,-0.707,1.0));
				nomalvertex->push_back(osg::Vec3(-0.293,-0.707,1.0));
				nomalvertex->push_back(osg::Vec3(0.0,0.0,1.0));
				nomalvertex->push_back(osg::Vec3(0.0,1.0,1.0));
				nomalvertex->push_back(osg::Vec3(0.0,0.0,1.0));
				nomalvertex->push_back(osg::Vec3(0.293,-0.707,1.0));
				nomalvertex->push_back(osg::Vec3(0.293,-0.707,1.0));
				bordergeometry->setNormalArray(nomalvertex.get());
				bordergeometry->setNormalBinding(osg::Geometry::BIND_PER_VERTEX);
				fillgeometry->setNormalArray(nomalvertex.get());
				fillgeometry->setNormalBinding(osg::Geometry::BIND_PER_VERTEX);

				osg::ref_ptr<osg::Vec4Array> linecolorArray = new osg::Vec4Array;
				linecolorArray->push_back(osg::Vec4(GetBValue(linecolor)/255.0,LOBYTE(((linecolor & 0xFFFF)) >> 8)/255.0,GetRValue(linecolor)/255.0,(glbFloat)lineOpacity/100.0));
				bordergeometry->setColorArray(linecolorArray);
				bordergeometry->setColorBinding(osg::Geometry::BIND_OVERALL);
				osg::ref_ptr<osg::Vec4Array> fillcolorArray = new osg::Vec4Array;
				fillcolorArray->push_back(osg::Vec4(GetBValue(fillcolor)/255.0,LOBYTE(((fillcolor & 0xFFFF)) >> 8)/255.0,GetRValue(fillcolor)/255.0,(glbFloat)fillOpacity/100.0));
				fillgeometry->setColorArray(fillcolorArray);
				fillgeometry->setColorBinding(osg::Geometry::BIND_OVERALL);

				osg::ref_ptr<osg::DrawElementsUInt> indexArray = new osg::DrawElementsUInt(osg::DrawElementsUInt::LINE_LOOP,0);
				indexArray->push_back(0);
				indexArray->push_back(1);
				indexArray->push_back(2);
				indexArray->push_back(3);
				indexArray->push_back(4);
				indexArray->push_back(5);
				indexArray->push_back(6);
				indexArray->push_back(7);
				osg::ref_ptr<osg::DrawElementsUInt> indexArray1 = new osg::DrawElementsUInt(osg::DrawElementsUInt::LINE_LOOP,0);
				indexArray1->push_back(8);
				indexArray1->push_back(9);
				indexArray1->push_back(10);
				indexArray1->push_back(11);
				indexArray1->push_back(12);
				indexArray1->push_back(13);
				indexArray1->push_back(14);
				indexArray1->push_back(15);
				osg::ref_ptr<osg::DrawElementsUInt> indexArray2 = new osg::DrawElementsUInt(osg::DrawElementsUInt::LINES,0);
				indexArray2->push_back(0);indexArray2->push_back(8);
				indexArray2->push_back(1);indexArray2->push_back(9);
				indexArray2->push_back(2);indexArray2->push_back(10);
				indexArray2->push_back(3);indexArray2->push_back(11);
				indexArray2->push_back(4);indexArray2->push_back(12);
				indexArray2->push_back(5);indexArray2->push_back(13);
				indexArray2->push_back(6);indexArray2->push_back(14);
				indexArray2->push_back(7);indexArray2->push_back(15);
				bordergeometry->addPrimitiveSet(indexArray.get());
				bordergeometry->addPrimitiveSet(indexArray1.get());
				bordergeometry->addPrimitiveSet(indexArray2.get());

				osg::ref_ptr<osg::DrawElementsUInt> indexArray3 = new osg::DrawElementsUInt(osg::DrawElementsUInt::TRIANGLES,0);	//6+6个三角形
				indexArray3->push_back(0);			indexArray3->push_back(2);			indexArray3->push_back(1);
				indexArray3->push_back(0);			indexArray3->push_back(6);			indexArray3->push_back(2);
				indexArray3->push_back(0);			indexArray3->push_back(7);			indexArray3->push_back(6);
				indexArray3->push_back(4);			indexArray3->push_back(3);			indexArray3->push_back(2);
				indexArray3->push_back(4);			indexArray3->push_back(2);			indexArray3->push_back(6);
				indexArray3->push_back(4);			indexArray3->push_back(6);			indexArray3->push_back(5);
				indexArray3->push_back(8);			indexArray3->push_back(9);			indexArray3->push_back(10);
				indexArray3->push_back(8);			indexArray3->push_back(10);			indexArray3->push_back(14);
				indexArray3->push_back(8);			indexArray3->push_back(14);			indexArray3->push_back(15);
				indexArray3->push_back(12);			indexArray3->push_back(10);			indexArray3->push_back(11);
				indexArray3->push_back(12);			indexArray3->push_back(14);			indexArray3->push_back(10);
				indexArray3->push_back(12);			indexArray3->push_back(13);			indexArray3->push_back(14);
				osg::ref_ptr<osg::DrawElementsUInt> indexArray4 = new osg::DrawElementsUInt(osg::DrawElementsUInt::QUAD_STRIP,0);
				indexArray4->push_back(0);indexArray4->push_back(8);
				indexArray4->push_back(1);indexArray4->push_back(9);
				indexArray4->push_back(2);indexArray4->push_back(10);
				indexArray4->push_back(3);indexArray4->push_back(11);
				indexArray4->push_back(4);indexArray4->push_back(12);
				indexArray4->push_back(5);indexArray4->push_back(13);
				indexArray4->push_back(6);indexArray4->push_back(14);
				indexArray4->push_back(7);indexArray4->push_back(15);
				indexArray4->push_back(0);indexArray4->push_back(8);
				fillgeometry->addPrimitiveSet(indexArray3.get());
				fillgeometry->addPrimitiveSet(indexArray4.get());
			}
		}
		break;
	case GLB_ARROWPATTERN_D1:	//4个点，3条线
		{
			if (Arrowheight <= 0.0)
			{
				glbDouble Arrow_y = min(Arrowlength*0.66,abs(Arrowlength-Arrowwidth));
				vertexes->push_back(osg::Vec3d(0.0,0.0,0.0));
				vertexes->push_back(osg::Vec3d(0.0,Arrowlength,0.0));
				vertexes->push_back(osg::Vec3d(-Arrowwidth/2,Arrow_y,0.0));
				vertexes->push_back(osg::Vec3d(Arrowwidth/2,Arrow_y,0.0));
				bordergeometry->setVertexArray(vertexes.get());

				osg::ref_ptr<osg::Vec3Array> nomalvertex = new osg::Vec3Array;
				nomalvertex->push_back(osg::Vec3(0.0,0.0,1.0));
				bordergeometry->setNormalArray(nomalvertex.get());
				bordergeometry->setNormalBinding(osg::Geometry::BIND_OVERALL);
				fillgeometry->setNormalArray(nomalvertex.get());
				fillgeometry->setNormalBinding(osg::Geometry::BIND_OVERALL);

				osg::ref_ptr<osg::Vec4Array> colorArray = new osg::Vec4Array;
				colorArray->push_back(osg::Vec4(GetBValue(linecolor)/255.0,LOBYTE(((linecolor & 0xFFFF)) >> 8)/255.0,GetRValue(linecolor)/255.0,(glbFloat)lineOpacity/100.0));
				bordergeometry->setColorArray(colorArray);
				bordergeometry->setColorBinding(osg::Geometry::BIND_OVERALL);
				osg::ref_ptr<osg::Vec4Array> fillcolorArray = new osg::Vec4Array;
				fillcolorArray->push_back(osg::Vec4(GetBValue(fillcolor)/255.0,LOBYTE(((fillcolor & 0xFFFF)) >> 8)/255.0,GetRValue(fillcolor)/255.0,(glbFloat)fillOpacity/100.0));
				fillgeometry->setColorArray(fillcolorArray);
				fillgeometry->setColorBinding(osg::Geometry::BIND_OVERALL);
				osg::ref_ptr<osg::DrawElementsUInt> indexArray=new osg::DrawElementsUInt(osg::DrawElementsUInt::LINE_STRIP,0);
				indexArray->push_back(0);
				indexArray->push_back(1);
				indexArray->push_back(2);
				indexArray->push_back(1);
				indexArray->push_back(3);
				indexArray->push_back(1);
				bordergeometry->addPrimitiveSet(indexArray.get());				
			}else{	//8个点，3个面线
				glbDouble Arrow_y = min(Arrowlength*0.66,abs(Arrowlength-Arrowwidth));
				vertexes->push_back(osg::Vec3d(0.0,0.0,0.0));
				vertexes->push_back(osg::Vec3d(0.0,Arrowlength,0.0));
				vertexes->push_back(osg::Vec3d(-Arrowwidth/2,Arrow_y,0.0));
				vertexes->push_back(osg::Vec3d(Arrowwidth/2,Arrow_y,0.0));
				vertexes->push_back(osg::Vec3d(0.0,0.0,Arrowheight));
				vertexes->push_back(osg::Vec3d(0.0,Arrowlength,Arrowheight));
				vertexes->push_back(osg::Vec3d(-Arrowwidth/2,Arrow_y,Arrowheight));
				vertexes->push_back(osg::Vec3d(Arrowwidth/2,Arrow_y,Arrowheight));
				bordergeometry->setVertexArray(vertexes.get());
				fillgeometry->setVertexArray(vertexes.get());

				osg::ref_ptr<osg::Vec3Array> nomalvertex = new osg::Vec3Array;
				nomalvertex->push_back(osg::Vec3(0.0,0.0,1.0));
				bordergeometry->setNormalArray(nomalvertex.get());
				bordergeometry->setNormalBinding(osg::Geometry::BIND_OVERALL);
				fillgeometry->setNormalArray(nomalvertex.get());
				fillgeometry->setNormalBinding(osg::Geometry::BIND_OVERALL);

				osg::ref_ptr<osg::Vec4Array> colorArray = new osg::Vec4Array;
				colorArray->push_back(osg::Vec4(GetBValue(linecolor)/255.0,LOBYTE(((linecolor & 0xFFFF)) >> 8)/255.0,GetRValue(linecolor)/255.0,(glbFloat)lineOpacity/100.0));
				bordergeometry->setColorArray(colorArray);
				bordergeometry->setColorBinding(osg::Geometry::BIND_OVERALL);
				osg::ref_ptr<osg::Vec4Array> fillcolorArray = new osg::Vec4Array;
				fillcolorArray->push_back(osg::Vec4(GetBValue(fillcolor)/255.0,LOBYTE(((fillcolor & 0xFFFF)) >> 8)/255.0,GetRValue(fillcolor)/255.0,(glbFloat)fillOpacity/100.0));
				fillgeometry->setColorArray(fillcolorArray);
				fillgeometry->setColorBinding(osg::Geometry::BIND_OVERALL);

				osg::ref_ptr<osg::DrawElementsUInt> indexArray=new osg::DrawElementsUInt(osg::DrawElementsUInt::LINES,0);
				indexArray->push_back(0);
				indexArray->push_back(1);
				indexArray->push_back(2);
				indexArray->push_back(1);
				indexArray->push_back(3);
				indexArray->push_back(1);
				indexArray->push_back(4);
				indexArray->push_back(5);
				indexArray->push_back(6);
				indexArray->push_back(5);
				indexArray->push_back(7);
				indexArray->push_back(5);
				indexArray->push_back(0);
				indexArray->push_back(4);
				indexArray->push_back(1);
				indexArray->push_back(5);
				indexArray->push_back(2);
				indexArray->push_back(6);
				indexArray->push_back(3);
				indexArray->push_back(7);
				bordergeometry->addPrimitiveSet(indexArray.get());
				osg::ref_ptr<osg::DrawElementsUInt> indexArray1=new osg::DrawElementsUInt(osg::DrawElementsUInt::QUADS,0);
				indexArray1->push_back(0);indexArray1->push_back(1);indexArray1->push_back(5);indexArray1->push_back(4);
				indexArray1->push_back(1);indexArray1->push_back(2);indexArray1->push_back(6);indexArray1->push_back(5);
				indexArray1->push_back(3);indexArray1->push_back(1);indexArray1->push_back(5);indexArray1->push_back(7);
				fillgeometry->addPrimitiveSet(indexArray1.get());
			}
		};
		break;
	case GLB_ARROWPATTERN_D2:	//5个点，一条线，一个三角形
		{
			if (Arrowheight <= 0.0)
			{
				glbDouble Arrow_y = min(Arrowlength*0.66,abs(Arrowlength-Arrowwidth));
				vertexes->push_back(osg::Vec3d(0.0,0.0,0.0));
				vertexes->push_back(osg::Vec3d(0.0,Arrow_y,0.0));
				vertexes->push_back(osg::Vec3d(0.0,Arrowlength,0.0));
				vertexes->push_back(osg::Vec3d(-Arrowwidth/2,Arrow_y,0.0));
				vertexes->push_back(osg::Vec3d(Arrowwidth/2,Arrow_y,0.0));
				bordergeometry->setVertexArray(vertexes.get());
				fillgeometry->setVertexArray(vertexes.get());

				osg::ref_ptr<osg::Vec3Array> nomalvertex = new osg::Vec3Array;
				nomalvertex->push_back(osg::Vec3(0.0,0.0,1.0));
				bordergeometry->setNormalArray(nomalvertex.get());
				bordergeometry->setNormalBinding(osg::Geometry::BIND_OVERALL);
				fillgeometry->setNormalArray(nomalvertex.get());
				fillgeometry->setNormalBinding(osg::Geometry::BIND_OVERALL);

				osg::ref_ptr<osg::Vec4Array> linecolorArray = new osg::Vec4Array;
				linecolorArray->push_back(osg::Vec4(GetBValue(linecolor)/255.0,LOBYTE(((linecolor & 0xFFFF)) >> 8)/255.0,GetRValue(linecolor)/255.0,(glbFloat)lineOpacity/100.0));
				bordergeometry->setColorArray(linecolorArray);
				bordergeometry->setColorBinding(osg::Geometry::BIND_OVERALL);
				osg::ref_ptr<osg::Vec4Array> fillcolorArray = new osg::Vec4Array;
				fillcolorArray->push_back(osg::Vec4(GetBValue(fillcolor)/255.0,LOBYTE(((fillcolor & 0xFFFF)) >> 8)/255.0,GetRValue(fillcolor)/255.0,(glbFloat)fillOpacity/100.0));
				fillgeometry->setColorArray(fillcolorArray);
				fillgeometry->setColorBinding(osg::Geometry::BIND_OVERALL);

				osg::ref_ptr<osg::DrawElementsUInt> indexArray = new osg::DrawElementsUInt(osg::DrawElementsUInt::LINES,0);
				indexArray->push_back(0);
				indexArray->push_back(1);
				indexArray->push_back(2);
				indexArray->push_back(3);
				indexArray->push_back(3);
				indexArray->push_back(4);
				indexArray->push_back(4);
				indexArray->push_back(2);
				osg::ref_ptr<osg::DrawElementsUInt> indexArray2 = new osg::DrawElementsUInt(osg::DrawElementsUInt::TRIANGLES,0);
				indexArray2->push_back(2);
				indexArray2->push_back(3);
				indexArray2->push_back(4);
				bordergeometry->addPrimitiveSet(indexArray.get());
				fillgeometry->addPrimitiveSet(indexArray2.get());
			}else{	
				glbDouble Arrow_y = min(Arrowlength*0.66,abs(Arrowlength-Arrowwidth));
				vertexes->push_back(osg::Vec3d(0.0,0.0,0.0));
				vertexes->push_back(osg::Vec3d(0.0,Arrow_y,0.0));
				vertexes->push_back(osg::Vec3d(0.0,Arrowlength,0.0));
				vertexes->push_back(osg::Vec3d(-Arrowwidth/2,Arrow_y,0.0));
				vertexes->push_back(osg::Vec3d(Arrowwidth/2,Arrow_y,0.0));
				vertexes->push_back(osg::Vec3d(0.0,0.0,Arrowheight));
				vertexes->push_back(osg::Vec3d(0.0,Arrow_y,Arrowheight));
				vertexes->push_back(osg::Vec3d(0.0,Arrowlength,Arrowheight));
				vertexes->push_back(osg::Vec3d(-Arrowwidth/2,Arrow_y,Arrowheight));
				vertexes->push_back(osg::Vec3d(Arrowwidth/2,Arrow_y,Arrowheight));
				bordergeometry->setVertexArray(vertexes.get());
				fillgeometry->setVertexArray(vertexes.get());

				osg::ref_ptr<osg::Vec3Array> nomalvertex = new osg::Vec3Array;
				nomalvertex->push_back(osg::Vec3(0.0,0.0,-1.0));
				nomalvertex->push_back(osg::Vec3(0.0,0.0,-1.0));
				nomalvertex->push_back(osg::Vec3(0.0,1.0,-1.0));
				nomalvertex->push_back(osg::Vec3(-0.707,-0.293,-1.0));
				nomalvertex->push_back(osg::Vec3(0.707,-0.293,-1.0));
				nomalvertex->push_back(osg::Vec3(0.0,0.0,1.0));
				nomalvertex->push_back(osg::Vec3(0.0,0.0,1.0));
				nomalvertex->push_back(osg::Vec3(0.0,1.0,1.0));
				nomalvertex->push_back(osg::Vec3(-0.707,-0.293,1.0));
				nomalvertex->push_back(osg::Vec3(0.707,-0.293,1.0));
				
				bordergeometry->setNormalArray(nomalvertex.get());
				bordergeometry->setNormalBinding(osg::Geometry::BIND_PER_VERTEX);
				fillgeometry->setNormalArray(nomalvertex.get());
				fillgeometry->setNormalBinding(osg::Geometry::BIND_PER_VERTEX);

				osg::ref_ptr<osg::DrawElementsUInt> indexArray = new osg::DrawElementsUInt(osg::DrawElementsUInt::LINES,0);
				indexArray->push_back(0);
				indexArray->push_back(1);
				indexArray->push_back(2);
				indexArray->push_back(3);
				indexArray->push_back(3);
				indexArray->push_back(4);
				indexArray->push_back(4);
				indexArray->push_back(2);
				indexArray->push_back(5);
				indexArray->push_back(6);
				indexArray->push_back(7);
				indexArray->push_back(8);
				indexArray->push_back(8);
				indexArray->push_back(9);
				indexArray->push_back(9);
				indexArray->push_back(7);
				indexArray->push_back(0);
				indexArray->push_back(5);
				indexArray->push_back(1);
				indexArray->push_back(6);
				indexArray->push_back(2);
				indexArray->push_back(7);
				indexArray->push_back(3);
				indexArray->push_back(8);
				indexArray->push_back(4);
				indexArray->push_back(9);
				bordergeometry->addPrimitiveSet(indexArray.get());
				osg::ref_ptr<osg::DrawElementsUInt> indexArray1 = new osg::DrawElementsUInt(osg::DrawElementsUInt::TRIANGLES,0);
				indexArray1->push_back(2);
				indexArray1->push_back(3);
				indexArray1->push_back(4);
				indexArray1->push_back(7);
				indexArray1->push_back(9);
				indexArray1->push_back(8);
				osg::ref_ptr<osg::DrawElementsUInt> indexArray2 = new osg::DrawElementsUInt(osg::DrawElementsUInt::QUADS,0);
				indexArray2->push_back(0);indexArray2->push_back(1);indexArray2->push_back(6);indexArray2->push_back(5);
				indexArray2->push_back(3);indexArray2->push_back(4);indexArray2->push_back(9);indexArray2->push_back(8);
				indexArray2->push_back(4);indexArray2->push_back(2);indexArray2->push_back(7);indexArray2->push_back(9);
				indexArray2->push_back(2);indexArray2->push_back(3);indexArray2->push_back(8);indexArray2->push_back(7);
				fillgeometry->addPrimitiveSet(indexArray1.get());
				fillgeometry->addPrimitiveSet(indexArray2.get());
			}
		}
		break;
	case GLB_ARROWPATTERN_D3:	//7个顶点，1个quat,1个triangle
		{
			if (Arrowheight <= 0.0)
			{
				glbDouble Arrow_y = min(Arrowlength*0.66,abs(Arrowlength-Arrowwidth));
				vertexes->push_back(osg::Vec3d(-Arrowwidth/6,0.0,0.0));
				vertexes->push_back(osg::Vec3d(Arrowwidth/6,0.0,0.0));
				vertexes->push_back(osg::Vec3d(Arrowwidth/6,Arrow_y,0.0));
				vertexes->push_back(osg::Vec3d(-Arrowwidth/6,Arrow_y,0.0));

				vertexes->push_back(osg::Vec3d(0.0,Arrowlength,0.0));
				vertexes->push_back(osg::Vec3d(-Arrowwidth/2,Arrow_y,0.0));
				vertexes->push_back(osg::Vec3d(Arrowwidth/2,Arrow_y,0.0));
				bordergeometry->setVertexArray(vertexes.get());
				fillgeometry->setVertexArray(vertexes.get());

				osg::ref_ptr<osg::Vec3Array> nomalvertex = new osg::Vec3Array;
				nomalvertex->push_back(osg::Vec3(0.0,0.0,1.0));
				bordergeometry->setNormalArray(nomalvertex.get());
				bordergeometry->setNormalBinding(osg::Geometry::BIND_OVERALL);
				fillgeometry->setNormalArray(nomalvertex.get());
				fillgeometry->setNormalBinding(osg::Geometry::BIND_OVERALL);

				osg::ref_ptr<osg::Vec4Array> linecolorArray = new osg::Vec4Array;
				linecolorArray->push_back(osg::Vec4(GetBValue(linecolor)/255.0,LOBYTE(((linecolor & 0xFFFF)) >> 8)/255.0,GetRValue(linecolor)/255.0,(glbFloat)lineOpacity/100.0));
				bordergeometry->setColorArray(linecolorArray);
				bordergeometry->setColorBinding(osg::Geometry::BIND_OVERALL);
				osg::ref_ptr<osg::Vec4Array> fillcolorArray = new osg::Vec4Array;
				fillcolorArray->push_back(osg::Vec4(GetBValue(fillcolor)/255.0,LOBYTE(((fillcolor & 0xFFFF)) >> 8)/255.0,GetRValue(fillcolor)/255.0,(glbFloat)fillOpacity/100.0));
				fillgeometry->setColorArray(fillcolorArray);
				fillgeometry->setColorBinding(osg::Geometry::BIND_OVERALL);

				osg::ref_ptr<osg::DrawElementsUInt> indexArray0 = new osg::DrawElementsUInt(osg::DrawElementsUInt::LINE_LOOP,0);
				indexArray0->push_back(0);
				indexArray0->push_back(1);
				indexArray0->push_back(2);
				indexArray0->push_back(6);
				indexArray0->push_back(4);
				indexArray0->push_back(5);
				indexArray0->push_back(3);
				osg::ref_ptr<osg::DrawElementsUInt> indexArray = new osg::DrawElementsUInt(osg::DrawElementsUInt::LINES,0);
				indexArray->push_back(2);
				indexArray->push_back(3);
				bordergeometry->addPrimitiveSet(indexArray0.get());
				bordergeometry->addPrimitiveSet(indexArray.get());
 
				osg::ref_ptr<osg::DrawElementsUInt> indexArray2 = new osg::DrawElementsUInt(osg::DrawElementsUInt::QUADS,0);
				indexArray2->push_back(0);
				indexArray2->push_back(1);
				indexArray2->push_back(2);
				indexArray2->push_back(3);
				osg::ref_ptr<osg::DrawElementsUInt> indexArray3 = new osg::DrawElementsUInt(osg::DrawElementsUInt::TRIANGLES,0);
				indexArray3->push_back(4);
				indexArray3->push_back(5);
				indexArray3->push_back(6);
				fillgeometry->addPrimitiveSet(indexArray2.get());
				fillgeometry->addPrimitiveSet(indexArray3.get());
			}else{
				glbDouble Arrow_y = min(Arrowlength*0.66,abs(Arrowlength-Arrowwidth));
				//一个圆柱，一个圆锥
				osg::ref_ptr<osg::TessellationHints> hins = new osg::TessellationHints;
				hins->setDetailRatio(0.5f);

				osg::ref_ptr<osg::ShapeDrawable> shapeFillCylinderDrawable = new osg::ShapeDrawable(new osg::Cylinder(osg::Vec3d(0.0,0.0,0.0),Arrowwidth/4,Arrow_y),hins.get());
				osg::ref_ptr<osg::ShapeDrawable> shapeOutCylinderDrawable = new osg::ShapeDrawable(new osg::Cylinder(osg::Vec3d(0.0,0.0,0.0),Arrowwidth/4,Arrow_y),hins.get());
				osg::ref_ptr<osg::ShapeDrawable> shapeFillConeDrawable = new osg::ShapeDrawable(new osg::Cone(osg::Vec3d(0.0,Arrow_y,0.0),Arrowwidth/2,Arrowlength-Arrow_y),hins.get());
				osg::ref_ptr<osg::ShapeDrawable> shapeOutConeDrawable = new osg::ShapeDrawable(new osg::Cone(osg::Vec3d(0.0,Arrow_y,0.0),Arrowwidth/2,Arrowlength-Arrow_y),hins.get());

				osg::ref_ptr<osg::PolygonMode> polygonmodefill = new osg::PolygonMode;
				polygonmodefill->setMode(osg::PolygonMode::FRONT_AND_BACK,osg::PolygonMode::FILL);
				osg::ref_ptr<osg::PolygonMode> polygonmodeline = new osg::PolygonMode;
				polygonmodeline->setMode(osg::PolygonMode::FRONT_AND_BACK,osg::PolygonMode::LINE);
				shapeFillCylinderDrawable->getOrCreateStateSet()->setAttribute(polygonmodefill.get(),osg::StateAttribute::PROTECTED|osg::StateAttribute::ON);
				shapeFillConeDrawable->getOrCreateStateSet()->setAttribute(polygonmodefill.get(),osg::StateAttribute::PROTECTED|osg::StateAttribute::ON);
				shapeOutCylinderDrawable->getOrCreateStateSet()->setAttribute(polygonmodeline.get(),osg::StateAttribute::PROTECTED|osg::StateAttribute::ON);
				shapeOutConeDrawable->getOrCreateStateSet()->setAttribute(polygonmodeline.get(),osg::StateAttribute::PROTECTED|osg::StateAttribute::ON);
				
				//单独设置材质
				osg::ref_ptr<osg::Material> fillMaterial = dynamic_cast<osg::Material*>(shapeFillCylinderDrawable->getOrCreateStateSet()->getAttribute(osg::StateAttribute::MATERIAL));
				if (!fillMaterial.valid())
					fillMaterial = new osg::Material;
				shapeFillCylinderDrawable->getOrCreateStateSet()->setAttribute(fillMaterial,osg::StateAttribute::ON);	
				shapeFillCylinderDrawable->getOrCreateStateSet()->setMode(GL_DEPTH_TEST,osg::StateAttribute::ON);
				shapeFillConeDrawable->getOrCreateStateSet()->setAttribute(fillMaterial,osg::StateAttribute::ON);	
				shapeFillConeDrawable->getOrCreateStateSet()->setMode(GL_DEPTH_TEST,osg::StateAttribute::ON);

				//fillMaterial->setDiffuse(osg::Material::FRONT_AND_BACK,osg::Vec4(0.0,0.0,0.0,(glbFloat)lineOpacity/100.0));
				fillMaterial->setDiffuse(osg::Material::FRONT_AND_BACK,osg::Vec4(GetBValue(fillcolor)/255.0,LOBYTE(((fillcolor & 0xFFFF)) >> 8)/255.0,GetRValue(fillcolor)/255.0,(glbFloat)fillOpacity/100.0));
				if (fillOpacity < 100)
				{
					shapeFillCylinderDrawable->getOrCreateStateSet()->setMode(GL_BLEND,osg::StateAttribute::ON);
					if (shapeFillCylinderDrawable->getOrCreateStateSet()->getRenderingHint() != osg::StateSet::TRANSPARENT_BIN)
						shapeFillCylinderDrawable->getOrCreateStateSet()->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
					shapeFillConeDrawable->getOrCreateStateSet()->setMode(GL_BLEND,osg::StateAttribute::ON);
					if (shapeFillConeDrawable->getOrCreateStateSet()->getRenderingHint() != osg::StateSet::TRANSPARENT_BIN)
						shapeFillConeDrawable->getOrCreateStateSet()->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
				}else{
					shapeFillCylinderDrawable->getOrCreateStateSet()->setMode(GL_BLEND,osg::StateAttribute::OFF);
					shapeFillCylinderDrawable->getOrCreateStateSet()->setRenderingHint(osg::StateSet::OPAQUE_BIN);
					shapeFillConeDrawable->getOrCreateStateSet()->setMode(GL_BLEND,osg::StateAttribute::OFF);
					shapeFillConeDrawable->getOrCreateStateSet()->setRenderingHint(osg::StateSet::OPAQUE_BIN);
				}

				osg::ref_ptr<osg::Material> outMaterial = dynamic_cast<osg::Material*>(shapeOutCylinderDrawable->getOrCreateStateSet()->getAttribute(osg::StateAttribute::MATERIAL));
				if (!outMaterial.valid())
					outMaterial = new osg::Material;
				shapeOutCylinderDrawable->getOrCreateStateSet()->setAttribute(outMaterial,osg::StateAttribute::ON);	
				shapeOutCylinderDrawable->getOrCreateStateSet()->setMode(GL_DEPTH_TEST,osg::StateAttribute::ON);
				shapeOutConeDrawable->getOrCreateStateSet()->setAttribute(outMaterial,osg::StateAttribute::ON);	
				shapeOutConeDrawable->getOrCreateStateSet()->setMode(GL_DEPTH_TEST,osg::StateAttribute::ON);

				//outMaterial->setDiffuse(osg::Material::FRONT_AND_BACK,osg::Vec4(0.0,0.0,0.0,(glbFloat)lineOpacity/100.0));
				outMaterial->setDiffuse(osg::Material::FRONT_AND_BACK,osg::Vec4(GetBValue(fillcolor)/255.0,LOBYTE(((fillcolor & 0xFFFF)) >> 8)/255.0,GetRValue(fillcolor)/255.0,(glbFloat)fillOpacity/100.0));
				if (lineOpacity < 100)
				{
					shapeOutCylinderDrawable->getOrCreateStateSet()->setMode(GL_BLEND,osg::StateAttribute::ON);
					if (shapeOutCylinderDrawable->getOrCreateStateSet()->getRenderingHint() != osg::StateSet::TRANSPARENT_BIN)
						shapeOutCylinderDrawable->getOrCreateStateSet()->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
					shapeOutConeDrawable->getOrCreateStateSet()->setMode(GL_BLEND,osg::StateAttribute::ON);
					if (shapeOutConeDrawable->getOrCreateStateSet()->getRenderingHint() != osg::StateSet::TRANSPARENT_BIN)
						shapeOutConeDrawable->getOrCreateStateSet()->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
				}else{
					shapeOutCylinderDrawable->getOrCreateStateSet()->setMode(GL_BLEND,osg::StateAttribute::OFF);
					shapeOutCylinderDrawable->getOrCreateStateSet()->setRenderingHint(osg::StateSet::OPAQUE_BIN);
					shapeOutConeDrawable->getOrCreateStateSet()->setMode(GL_BLEND,osg::StateAttribute::OFF);
					shapeOutConeDrawable->getOrCreateStateSet()->setRenderingHint(osg::StateSet::OPAQUE_BIN);
				}
			}
		}
		break;
	default:
		break;
	}
	geode->addDrawable(bordergeometry.get());	//0
	geode->addDrawable(fillgeometry.get());		//1

	//纹理
	if (lineArrowInfo->textureData)
	{		
		osg::ref_ptr<osg::Image> image = new osg::Image;
		glbInt32 index = textureData.find_first_of(L'.');
		if(index == 0)
		{
			CGlbWString execDir = CGlbPath::GetExecDir();
			textureData = execDir + textureData.substr(1,textureData.size());
		}
		image = osgDB::readImageFile(textureData.ToString());
		if (image.valid())
		{
			//if (Arrowheight <= 0.0)
			//{
				osg::ref_ptr<osg::Texture2D> texture2d = new osg::Texture2D;
				osg::ref_ptr<osg::TexMat> texmat = new osg::TexMat;
				osg::BoundingBox bb = geode->getBoundingBox();
				glbDouble max_x = bb.xMax();
				glbDouble min_x = bb.xMin();
				glbDouble max_y = bb.yMax();
				glbDouble min_y = bb.yMin();
				glbDouble max_z = bb.zMax();
				glbDouble min_z = bb.zMin();
				//设置纹理对象
				texture2d->setDataVariance(osg::Object::DYNAMIC);
				texture2d->setImage(image.get());
				//纹理滤波
				texture2d->setFilter(osg::Texture::MIN_FILTER,osg::Texture::LINEAR);
				texture2d->setFilter(osg::Texture::MAG_FILTER,osg::Texture::LINEAR);
				//纹理边界
				texture2d->setWrap(osg::Texture::WRAP_S,osg::Texture::REPEAT);
				texture2d->setWrap(osg::Texture::WRAP_T,osg::Texture::REPEAT);
				//自动生成纹理坐标
				osg::ref_ptr<osg::TexGen> texgen = new osg::TexGen;
				texgen->setMode(osg::TexGen::OBJECT_LINEAR);
				texgen->setPlane(osg::TexGen::S,osg::Plane(1.0/(max_x-min_x),0.0,0.0,-min_x/(max_x-min_x)));
				texgen->setPlane(osg::TexGen::T,osg::Plane(0.0,1.0/(max_y-min_y),0.0,-min_y/(max_y-min_y)));
				//texgen->setPlane(osg::TexGen::R,osg::Plane(0.0,0.0,1.0/(max_z-min_z),-min_z/(max_z-min_z)));
				fillgeometry->getOrCreateStateSet()->setTextureAttributeAndModes(0,texgen.get(),osg::StateAttribute::ON);
				//纹理环境
				osg::ref_ptr<osg::TexEnv> texenv = new osg::TexEnv;
				texenv->setMode(osg::TexEnv::MODULATE);
				fillgeometry->getOrCreateStateSet()->setTextureAttribute(0,texenv.get());
				fillgeometry->getOrCreateStateSet()->setTextureAttributeAndModes(0,texture2d.get(),osg::StateAttribute::ON);	
				texture2d->setUnRefImageDataAfterApply(true);
				//texmat
				osg::Matrix mat;
				glbDouble xtexScale = 1.0,ytexScale = 1.0,ztexScale = 1.0;
				if (texRepeatMode == GLB_TEXTUREREPEAT_TIMES)
				{
					xtexScale = tilingU;
					ytexScale = tilingV;
				}else if (texRepeatMode == GLB_TEXTUREREPEAT_SIZE)
				{
					xtexScale = Arrowwidth/tilingU;
					ytexScale = Arrowlength/tilingV;
				}
				glbDouble angle = 0.0;
				angle = osg::DegreesToRadians(textureRotation);
				osg::Vec2 pt(Arrowwidth/2,Arrowlength/2);
				osg::Vec2 pt2(-Arrowwidth/2,Arrowlength/2);
				glbDouble ra = 2*pt.length();
				glbDouble ang = atan2(Arrowlength,Arrowwidth);
				glbDouble ang2 = atan2(Arrowlength,-Arrowwidth);
				glbDouble xm = ra*max(abs(cos(ang+angle)),abs(cos(ang2+angle)));
				glbDouble ym = ra*max(abs(sin(ang+angle)),abs(sin(ang2+angle)));			
				glbDouble sal =	ra*ra/(xm*xm+ym*ym);
				xtexScale *= sal;
				ytexScale *= sal;			
				texmat->setMatrix(osg::Matrix::translate(-0.5,-0.5,0.0) * osg::Matrix::rotate(angle,0.0,0.0,1.0) *osg::Matrix::scale(xtexScale,ytexScale,ztexScale) * osg::Matrix::translate(0.5*tilingU,0.5*tilingV,0.0));

				fillgeometry->getOrCreateStateSet()->setTextureAttributeAndModes(0,texmat.get(),osg::StateAttribute::ON);
			//}else{
				//if (lineArrowInfo->pattern->GetValue(feature) != GLB_ARROWPATTERN_D5)	//当做cube贴纹理,此处有些问题
				//{
				//	osg::BoundingBox bb = geode->getBoundingBox();
				//	glbDouble max_x = bb.xMax();
				//	glbDouble min_x = bb.xMin();
				//	glbDouble max_y = bb.yMax();
				//	glbDouble min_y = bb.yMin();
				//	glbDouble max_z = bb.zMax();
				//	glbDouble min_z = bb.zMin();
				//	osg::ref_ptr<osg::TextureCubeMap> cubemap = new osg::TextureCubeMap;
				//	cubemap->setImage(osg::TextureCubeMap::POSITIVE_X,image.get());
				//	cubemap->setImage(osg::TextureCubeMap::NEGATIVE_X,image.get());
				//	cubemap->setImage(osg::TextureCubeMap::POSITIVE_Y,image.get());
				//	cubemap->setImage(osg::TextureCubeMap::NEGATIVE_Y,image.get());
				//	cubemap->setImage(osg::TextureCubeMap::POSITIVE_Z,image.get());
				//	cubemap->setImage(osg::TextureCubeMap::NEGATIVE_Z,image.get());
				//	//纹理环绕
				//	cubemap->setWrap(osg::Texture::WRAP_S,osg::Texture::REPEAT);
				//	cubemap->setWrap(osg::Texture::WRAP_T,osg::Texture::REPEAT);
				//	cubemap->setWrap(osg::Texture::WRAP_R,osg::Texture::REPEAT);
				//	//纹理滤波
				//	cubemap->setFilter(osg::Texture::MIN_FILTER,osg::Texture::LINEAR);
				//	cubemap->setFilter(osg::Texture::MAG_FILTER,osg::Texture::LINEAR);
				//	//自动生成纹理坐标
				//	osg::ref_ptr<osg::TexGen> texgen = new osg::TexGen;
				//	texgen->setMode(osg::TexGen::NORMAL_MAP);
				//	fillgeometry->getOrCreateStateSet()->setTextureAttributeAndModes(0,texgen.get(),osg::StateAttribute::ON|osg::StateAttribute::PROTECTED);
				//	//纹理环境
				//	osg::ref_ptr<osg::TexEnv> texenv = new osg::TexEnv;
				//	texenv->setMode(osg::TexEnv::MODULATE);
				//	fillgeometry->getOrCreateStateSet()->setTextureAttribute(0,texenv.get());
				//	fillgeometry->getOrCreateStateSet()->setTextureAttributeAndModes(0,cubemap.get(),osg::StateAttribute::ON);	
				//	cubemap->setUnRefImageDataAfterApply(true);
				//	//设置纹理矩阵
				//	osg::ref_ptr<osg::TexMat> texmat = new osg::TexMat;
				//	osg::Matrix mat;
				//	//计算rote
				//	mat.makeTranslate(-0.5,-0.5,0.0);
				//	mat.postMultRotate(osg::Quat(osg::DegreesToRadians(textureRotation),0.0,0.0,1.0));
				//	mat.postMultTranslate(osg::Vec3(0.5,0.5,0.0));
				//	//计算scale
				//	if (texRepeatMode == GLB_TEXTUREREPEAT_TIMES)
				//	{
				//		mat.makeScale(tilingU,tilingV,tilingV);
				//	}else if (texRepeatMode == GLB_TEXTUREREPEAT_TIMES)
				//	{
				//		mat.makeScale((max_x-min_x)/tilingU,(max_y-min_y)/tilingV,(max_z-min_z)/tilingV);
				//	}
				//	texmat->setMatrix(mat);
				//	fillgeometry->getOrCreateStateSet()->setTextureAttributeAndModes(0,texmat.get(),osg::StateAttribute::ON);
				//}else{	//当圆柱贴

				//}
			//}
		}
		else if (textureData.length()>0)
		{
			GlbLogWOutput(GLB_LOGTYPE_ERR,L"读取线箭头图像文件(%s)失败. \r\n",textureData.ToString().c_str());
		}
	}
	//需要设置一定的PolygonOffset,让其更靠近视点,避免与其它面重叠
	osg::ref_ptr<osg::PolygonOffset> poffset = new osg::PolygonOffset(-0.1,-0.1);
	bordergeometry->getOrCreateStateSet()->setAttributeAndModes(poffset,osg::StateAttribute::ON);
	//线宽
	osg::ref_ptr<osg::LineWidth> lineWidth = new osg::LineWidth;
	lineWidth->setWidth(linewith);
	bordergeometry->getOrCreateStateSet()->setAttributeAndModes(lineWidth,osg::StateAttribute::ON);
	//线样式
	osg::ref_ptr<osg::LineStipple> lineStipple = new osg::LineStipple;
	if (lineStyle == GLB_LINE_DOTTED)
	{		
		lineStipple->setFactor(1);
		lineStipple->setPattern(0x1C47);
	}else if (lineStyle == GLB_LINE_SOLID)
	{
		lineStipple->setFactor(0);
	}
	bordergeometry->getOrCreateStateSet()->setAttributeAndModes(lineStipple.get(),osg::StateAttribute::ON);
	//设置材质
	osg::ref_ptr<osg::Material> fillMaterial = dynamic_cast<osg::Material*>(fillgeometry->getOrCreateStateSet()->getAttribute(osg::StateAttribute::MATERIAL));
	if (!fillMaterial.valid())
		fillMaterial = new osg::Material;
	fillgeometry->getOrCreateStateSet()->setAttribute(fillMaterial,osg::StateAttribute::ON);	
	fillgeometry->getOrCreateStateSet()->setMode(GL_DEPTH_TEST,osg::StateAttribute::ON);
	//fillMaterial->setDiffuse(osg::Material::FRONT_AND_BACK,osg::Vec4(GetBValue(fillcolor)/255.0,GetGValue(fillcolor)/255.0,GetRValue(fillcolor)/255.0,(glbFloat)fillOpacity/100.0));
	//fillMaterial->setDiffuse(osg::Material::FRONT_AND_BACK,osg::Vec4(0.0,0.0,0.0,(glbFloat)fillOpacity/100.0));
	fillMaterial->setDiffuse(osg::Material::FRONT_AND_BACK,osg::Vec4(GetBValue(fillcolor)/255.0,LOBYTE(((fillcolor & 0xFFFF)) >> 8)/255.0,GetRValue(fillcolor)/255.0,(glbFloat)fillOpacity/100.0));
	if (fillOpacity < 100)
	{
		fillgeometry->getOrCreateStateSet()->setMode(GL_BLEND,osg::StateAttribute::ON);
		if (fillgeometry->getOrCreateStateSet()->getRenderingHint() != osg::StateSet::TRANSPARENT_BIN)
			fillgeometry->getOrCreateStateSet()->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
	}else{
		fillgeometry->getOrCreateStateSet()->setMode(GL_BLEND,osg::StateAttribute::OFF);
		fillgeometry->getOrCreateStateSet()->setRenderingHint(osg::StateSet::OPAQUE_BIN);
	}

	osg::ref_ptr<osg::Material> outMaterial = dynamic_cast<osg::Material*>(bordergeometry->getOrCreateStateSet()->getAttribute(osg::StateAttribute::MATERIAL));
	if (!outMaterial.valid())
		outMaterial = new osg::Material;
	bordergeometry->getOrCreateStateSet()->setAttribute(outMaterial,osg::StateAttribute::ON);	
	bordergeometry->getOrCreateStateSet()->setMode(GL_DEPTH_TEST,osg::StateAttribute::ON);
	//outMaterial->setDiffuse(osg::Material::FRONT_AND_BACK,osg::Vec4(GetBValue(linecolor)/255.0,GetGValue(linecolor)/255.0,GetRValue(linecolor)/255.0,(glbFloat)lineOpacity/100.0));
	//outMaterial->setDiffuse(osg::Material::FRONT_AND_BACK,osg::Vec4(0.0,0.0,0.0,(glbFloat)lineOpacity/100.0));
	outMaterial->setDiffuse(osg::Material::FRONT_AND_BACK,osg::Vec4(GetBValue(linecolor)/255.0,LOBYTE(((linecolor & 0xFFFF)) >> 8)/255.0,GetRValue(linecolor)/255.0,(glbFloat)lineOpacity/100.0));
	if (lineOpacity < 100)
	{
		bordergeometry->getOrCreateStateSet()->setMode(GL_BLEND,osg::StateAttribute::ON);
		if (bordergeometry->getOrCreateStateSet()->getRenderingHint() != osg::StateSet::TRANSPARENT_BIN)
			bordergeometry->getOrCreateStateSet()->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
	}else{
		bordergeometry->getOrCreateStateSet()->setMode(GL_BLEND,osg::StateAttribute::OFF);
		bordergeometry->getOrCreateStateSet()->setRenderingHint(osg::StateSet::OPAQUE_BIN);
	}
	osg::ref_ptr<osg::BlendFunc> pBlendFun = new osg::BlendFunc(osg::BlendFunc::SRC_ALPHA,osg::BlendFunc::ONE_MINUS_SRC_ALPHA );	//设置混合方程
	geode->getOrCreateStateSet()->setAttributeAndModes(pBlendFun.get(),osg::StateAttribute::ON|osg::StateAttribute::OVERRIDE);
	geode->getOrCreateStateSet()->setRenderBinDetails(5,"RenderBin");
	return geode;
}

void CGlbGlobeLineArrowSymbol::DrawToImage(CGlbGlobeRObject *obj, IGlbGeometry* geom,glbByte *image,glbInt32 imageW,glbInt32 imageH,CGlbExtent &ext)
{
	if (!obj || !geom)
		return;
	//改由上层地形块tile逻辑中判断是否需要绘制==符号认为只要调用DrawToImage就是要绘制 2014.10.17 马林
	//CGlbFeature *feature = obj->GetFeature();
	//glbref_ptr<IGlbGeometry> outline = obj->GetOutLine();
	//if(outline == NULL) return;

	//// 判断ext是否与outline范围相交，不相交则不需要处理
	//CGlbExtent* outln_ext = outline->GetExtent();
	//glbDouble outln_mx,outln_Mx,outln_my,outln_My;
	//outln_ext->GetMin(&outln_mx,&outln_my);
	//outln_ext->GetMax(&outln_Mx,&outln_My);
	//glbDouble mx,my,Mx,My;
	//ext.GetMin(&mx,&my);
	//ext.GetMax(&Mx,&My);
	//glbDouble minx = max(mx,outln_mx);
	//glbDouble miny = max(my,outln_my);
	//glbDouble maxx = min(Mx,outln_Mx);
	//glbDouble maxy = min(My,outln_My);
	//if (minx<=maxx && miny<=maxy)
	{// outline的外包与ext范围有交集

		agg::rendering_buffer rbuf(image, imageW, imageH, -imageW * 4);
		agg::pixfmt_rgba32 pixf(rbuf);
		renb_type renb(pixf);//底层渲染器 

		DealAggRender(renb,obj,geom,imageW,imageH,ext);		
	}
}

IGlbGeometry* CGlbGlobeLineArrowSymbol::GetOutline(CGlbGlobeRObject *obj,IGlbGeometry *geo)
{
	if(obj->GetAltitudeMode() == GLB_ALTITUDEMODE_ONTERRAIN)
	{
		GlbRenderInfo *renderInfo = obj->GetRenderInfo();
		if(renderInfo == NULL)
			return NULL;
		GlbLineArrowSymbolInfo *lineArrowInfo = static_cast<GlbLineArrowSymbolInfo *>(renderInfo);
		CGlbFeature *feature = obj->GetFeature();

		CGlbLine *line = dynamic_cast<CGlbLine *>(geo);
		CGlbLine *line3d = dynamic_cast<CGlbLine*>(geo);
		if(line == NULL && line3d==NULL)    
			return NULL;
		osg::Vec3d pt0,pt1;	
		osg::Vec3d point0,point1;
		if (line)
		{
			if (line->GetCount() != 2)
				return NULL;

			pt0.set(osg::Vec3d(0,0,0));
			pt1.set(osg::Vec3d(0,0,0));
			line->GetPoint(0,&pt0.x(),&pt0.y());
			line->GetPoint(1,&pt1.x(),&pt1.y());	
		}
		else if (line3d)
		{
			if (line3d->GetCount() != 2)
				return NULL;
			line3d->GetPoint(0,&pt0.x(),&pt0.y(),&pt0.z());
			line3d->GetPoint(1,&pt1.x(),&pt1.y(),&pt1.z());		
		}		

		
		CGlbMultiLine* multiline = new CGlbMultiLine;
		glbDouble Arrowlength = (pt1 - pt0).length();
		if (obj->GetGlobe()->GetType() == GLB_GLOBETYPE_GLOBE)
		{
			g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(pt0.y()),osg::DegreesToRadians(pt0.x()),pt0.z(),point0.x(),point0.y(),point0.z());
			g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(pt1.y()),osg::DegreesToRadians(pt1.x()),pt1.z(),point1.x(),point1.y(),point1.z());
			Arrowlength = (point1 - point0).length();
		}
		glbDouble Arrowwidth = 10.0;
		if (lineArrowInfo->width)
			Arrowwidth = lineArrowInfo->width->GetValue(feature);
		//获取geomery，获取3维顶点，转化为2维经纬度
		switch(lineArrowInfo->pattern->GetValue(feature))
		{
		case GLB_ARROWPATTERN_D1:
			{
				glbDouble Arrow_y = min(Arrowlength*0.66,abs(abs(Arrowlength-Arrowwidth)));
				osg::ref_ptr<osg::Vec3dArray> vertexes = new osg::Vec3dArray;
				vertexes->push_back(osg::Vec3d(0.0,Arrowlength,0.0));
				vertexes->push_back(osg::Vec3d(Arrowwidth/2.0,Arrow_y,0.0));
				vertexes->push_back(osg::Vec3d(Arrowwidth/6.0,Arrow_y,0.0));
				vertexes->push_back(osg::Vec3d(Arrowwidth/6.0,0.0,0.0));
				vertexes->push_back(osg::Vec3d(-Arrowwidth/6.0,0.0,0.0));			
				vertexes->push_back(osg::Vec3d(-Arrowwidth/6.0,Arrow_y,0.0));			
				vertexes->push_back(osg::Vec3d(-Arrowwidth/2.0,Arrow_y,0.0));
				vertexes->push_back(osg::Vec3d(0.0,Arrowlength,0.0));
				//将vertexes变换到地球上
				osg::Matrixd mat;
				if(obj->GetGlobe()->GetType() == GLB_GLOBETYPE_GLOBE)
					g_ellipsoidModel->computeLocalToWorldTransformFromXYZ(point0.x(),point0.y(),point0.z(),mat);
				else if (obj->GetGlobe()->GetType() == GLB_GLOBETYPE_FLAT)
					mat.makeTranslate(point0);
				glbDouble yaw = 0.0;
				glbDouble pitch = 0.0;
				glbDouble roll = 0.0;
				if (lineArrowInfo->roll)
					roll = lineArrowInfo->roll->GetValue(feature);
				osg::Vec3d point1_0;
				if (obj->GetGlobe()->GetType() == GLB_GLOBETYPE_GLOBE)
				{
					osg::Matrix locTowor;
					g_ellipsoidModel->computeLocalToWorldTransformFromXYZ(point0.x(),point0.y(),point0.z(),locTowor);
					point1_0 = point1 * osg::Matrix::inverse(locTowor);
					
				}else if (obj->GetGlobe()->GetType() == GLB_GLOBETYPE_FLAT)
					point1_0 = point1-point0;
				point1_0.normalize();
				if (point1_0.z() > 1.0)
					point1_0.z() = 1.0;
				else if (point1_0.z() < -1.0)
					point1_0.z() = -1.0;
				pitch = asin(point1_0.z());
				yaw = atan2(-point1_0.x(),point1_0.y());
				roll = osg::DegreesToRadians(roll);
				osg::Vec3d yaw_vec(0.0,0.0,1.0);
				osg::Vec3d pitch_vec(cos(yaw),sin(yaw),0.0);
				osg::Vec3d roll_vec(-sin(yaw)*cos(pitch),cos(pitch)*cos(yaw),sin(pitch));
				osg::Quat quat;
				quat.makeRotate(yaw,yaw_vec,pitch,pitch_vec,roll,roll_vec);
				mat.preMultRotate(quat);

				CGlbLine* ln = new CGlbLine;
				for (size_t i = 0;i < vertexes->size();i++)
				{
					osg::Vec3d point = (*vertexes)[i] * mat;
					g_ellipsoidModel->convertXYZToLatLongHeight(point.x(),point.y(),point.z(),point.y(),point.x(),point.z());
					point.x() = osg::RadiansToDegrees(point.x());
					point.y() = osg::RadiansToDegrees(point.y());
					ln->AddPoint(point.x(),point.y());
				}
				multiline->AddLine(ln);
			}
			break;
		case GLB_ARROWPATTERN_D2:
			{
				glbDouble Arrow_y = min(Arrowlength*0.66,abs(abs(Arrowlength-Arrowwidth)));
				osg::ref_ptr<osg::Vec3dArray> vertexes = new osg::Vec3dArray;
				vertexes->push_back(osg::Vec3d(0.0,0.0,0.0));
				vertexes->push_back(osg::Vec3d(-Arrowwidth/4,-Arrowwidth/4,0.0));
				vertexes->push_back(osg::Vec3d(-Arrowwidth/16,Arrowlength-(Arrowlength-Arrow_y)/2,0.0));
				vertexes->push_back(osg::Vec3d(-Arrowwidth/2,Arrow_y,0.0));
				vertexes->push_back(osg::Vec3d(0.0,Arrowlength,0.0));				
				vertexes->push_back(osg::Vec3d(Arrowwidth/2,Arrow_y,0.0));		
				vertexes->push_back(osg::Vec3d(Arrowwidth/16,Arrowlength-(Arrowlength-Arrow_y)/2,0.0));
				vertexes->push_back(osg::Vec3d(Arrowwidth/4,-Arrowwidth/4,0.0));
				vertexes->push_back(osg::Vec3d(0.0,0.0,0.0));
				//将vertexes变换到地球上
				osg::Matrixd mat;
				if(obj->GetGlobe()->GetType() == GLB_GLOBETYPE_GLOBE)
					g_ellipsoidModel->computeLocalToWorldTransformFromXYZ(point0.x(),point0.y(),point0.z(),mat);
				else if (obj->GetGlobe()->GetType() == GLB_GLOBETYPE_FLAT)
					mat.makeTranslate(point0);
				glbDouble yaw = 0.0;
				glbDouble pitch = 0.0;
				glbDouble roll = 0.0;
				if (lineArrowInfo->roll)
					roll = lineArrowInfo->roll->GetValue(feature);
				osg::Vec3d point1_0;
				if (obj->GetGlobe()->GetType() == GLB_GLOBETYPE_GLOBE)
				{
					osg::Matrix locTowor;
					g_ellipsoidModel->computeLocalToWorldTransformFromXYZ(point0.x(),point0.y(),point0.z(),locTowor);
					point1_0 = point1 * osg::Matrix::inverse(locTowor);

				}else if (obj->GetGlobe()->GetType() == GLB_GLOBETYPE_FLAT)
					point1_0 = point1-point0;
				point1_0.normalize();
				if (point1_0.z() > 1.0)
					point1_0.z() = 1.0;
				else if (point1_0.z() < -1.0)
					point1_0.z() = -1.0;
				pitch = asin(point1_0.z());
				yaw = atan2(-point1_0.x(),point1_0.y());
				roll = osg::DegreesToRadians(roll);
				osg::Vec3d yaw_vec(0.0,0.0,1.0);
				osg::Vec3d pitch_vec(cos(yaw),sin(yaw),0.0);
				osg::Vec3d roll_vec(-sin(yaw)*cos(pitch),cos(pitch)*cos(yaw),sin(pitch));
				osg::Quat quat;
				quat.makeRotate(yaw,yaw_vec,pitch,pitch_vec,roll,roll_vec);
				mat.preMultRotate(quat);

				CGlbLine* ln = new CGlbLine;
				for (size_t i = 0;i < vertexes->size();i++)
				{
					osg::Vec3d point = (*vertexes)[i] * mat;
					g_ellipsoidModel->convertXYZToLatLongHeight(point.x(),point.y(),point.z(),point.y(),point.x(),point.z());
					point.x() = osg::RadiansToDegrees(point.x());
					point.y() = osg::RadiansToDegrees(point.y());
					ln->AddPoint(point.x(),point.y());
				}
				multiline->AddLine(ln);
			}
			break;
		case GLB_ARROWPATTERN_D3:
			{
				glbDouble Arrow_y = min(Arrowlength*0.66,abs(abs(Arrowlength-Arrowwidth)));
				osg::ref_ptr<osg::Vec3dArray> vertexes = new osg::Vec3dArray;
				vertexes->push_back(osg::Vec3d(0.0,0.0,0.0));
				vertexes->push_back(osg::Vec3d(0.0,Arrowlength,0.0));

				vertexes->push_back(osg::Vec3d(-Arrowwidth/2,Arrow_y,0.0));
				vertexes->push_back(osg::Vec3d(0.0,Arrowlength,0.0));
				vertexes->push_back(osg::Vec3d(Arrowwidth/2,Arrow_y,0.0));
				//将vertexes变换到地球上
				osg::Matrixd mat;
				if(obj->GetGlobe()->GetType() == GLB_GLOBETYPE_GLOBE)
					g_ellipsoidModel->computeLocalToWorldTransformFromXYZ(point0.x(),point0.y(),point0.z(),mat);
				else if (obj->GetGlobe()->GetType() == GLB_GLOBETYPE_FLAT)
					mat.makeTranslate(point0);
				glbDouble yaw = 0.0;
				glbDouble pitch = 0.0;
				glbDouble roll = 0.0;
				if (lineArrowInfo->roll)
					roll = lineArrowInfo->roll->GetValue(feature);
				osg::Vec3d point1_0;
				if (obj->GetGlobe()->GetType() == GLB_GLOBETYPE_GLOBE)
				{
					osg::Matrix locTowor;
					g_ellipsoidModel->computeLocalToWorldTransformFromXYZ(point0.x(),point0.y(),point0.z(),locTowor);
					point1_0 = point1 * osg::Matrix::inverse(locTowor);

				}else if (obj->GetGlobe()->GetType() == GLB_GLOBETYPE_FLAT)
					point1_0 = point1-point0;
				point1_0.normalize();
				if (point1_0.z() > 1.0)
					point1_0.z() = 1.0;
				else if (point1_0.z() < -1.0)
					point1_0.z() = -1.0;
				pitch = asin(point1_0.z());
				yaw = atan2(-point1_0.x(),point1_0.y());
				roll = osg::DegreesToRadians(roll);
				osg::Vec3d yaw_vec(0.0,0.0,1.0);
				osg::Vec3d pitch_vec(cos(yaw),sin(yaw),0.0);
				osg::Vec3d roll_vec(-sin(yaw)*cos(pitch),cos(pitch)*cos(yaw),sin(pitch));
				osg::Quat quat;
				quat.makeRotate(yaw,yaw_vec,pitch,pitch_vec,roll,roll_vec);
				mat.preMultRotate(quat);

				CGlbLine* ln = new CGlbLine;
				for (glbInt32 i = 0;i < 2;i++)
				{
					osg::Vec3d point = (*vertexes)[i] * mat;
					g_ellipsoidModel->convertXYZToLatLongHeight(point.x(),point.y(),point.z(),point.y(),point.x(),point.z());
					point.x() = osg::RadiansToDegrees(point.x());
					point.y() = osg::RadiansToDegrees(point.y());
					ln->AddPoint(point.x(),point.y());
				}
				multiline->AddLine(ln);
				CGlbLine* ln2 = new CGlbLine;
				for (size_t i = 2;i < vertexes->size();i++)
				{
					osg::Vec3d point = (*vertexes)[i] * mat;
					g_ellipsoidModel->convertXYZToLatLongHeight(point.x(),point.y(),point.z(),point.y(),point.x(),point.z());
					point.x() = osg::RadiansToDegrees(point.x());
					point.y() = osg::RadiansToDegrees(point.y());
					ln2->AddPoint(point.x(),point.y());
				}
				multiline->AddLine(ln2);
			}
			break;
		case GLB_ARROWPATTERN_D4:
			{
				glbDouble Arrow_y = min(Arrowlength*0.66,abs(abs(Arrowlength-Arrowwidth)));
				osg::ref_ptr<osg::Vec3dArray> vertexes = new osg::Vec3dArray;
				vertexes->push_back(osg::Vec3d(0.0,0.0,0.0));
				vertexes->push_back(osg::Vec3d(0.0,Arrow_y,0.0));

				vertexes->push_back(osg::Vec3d(0.0,Arrowlength,0.0));
				vertexes->push_back(osg::Vec3d(-Arrowwidth/2,Arrow_y,0.0));
				vertexes->push_back(osg::Vec3d(Arrowwidth/2,Arrow_y,0.0));
				vertexes->push_back(osg::Vec3d(0.0,Arrowlength,0.0));
				//将vertexes变换到地球上
				osg::Matrixd mat;
				if(obj->GetGlobe()->GetType() == GLB_GLOBETYPE_GLOBE)
					g_ellipsoidModel->computeLocalToWorldTransformFromXYZ(point0.x(),point0.y(),point0.z(),mat);
				else if (obj->GetGlobe()->GetType() == GLB_GLOBETYPE_FLAT)
					mat.makeTranslate(point0);
				glbDouble yaw = 0.0;
				glbDouble pitch = 0.0;
				glbDouble roll = 0.0;
				if (lineArrowInfo->roll)
					roll = lineArrowInfo->roll->GetValue(feature);
				osg::Vec3d point1_0;
				if (obj->GetGlobe()->GetType() == GLB_GLOBETYPE_GLOBE)
				{
					osg::Matrix locTowor;
					g_ellipsoidModel->computeLocalToWorldTransformFromXYZ(point0.x(),point0.y(),point0.z(),locTowor);
					point1_0 = point1 * osg::Matrix::inverse(locTowor);

				}else if (obj->GetGlobe()->GetType() == GLB_GLOBETYPE_FLAT)
					point1_0 = point1-point0;
				point1_0.normalize();
				if (point1_0.z() > 1.0)
					point1_0.z() = 1.0;
				else if (point1_0.z() < -1.0)
					point1_0.z() = -1.0;
				pitch = asin(point1_0.z());
				yaw = atan2(-point1_0.x(),point1_0.y());
				roll = osg::DegreesToRadians(roll);
				osg::Vec3d yaw_vec(0.0,0.0,1.0);
				osg::Vec3d pitch_vec(cos(yaw),sin(yaw),0.0);
				osg::Vec3d roll_vec(-sin(yaw)*cos(pitch),cos(pitch)*cos(yaw),sin(pitch));
				osg::Quat quat;
				quat.makeRotate(yaw,yaw_vec,pitch,pitch_vec,roll,roll_vec);
				mat.preMultRotate(quat);

				CGlbLine* ln = new CGlbLine;
				for (glbInt32 i = 0;i < 2;i++)
				{
					osg::Vec3d point = (*vertexes)[i] * mat;
					g_ellipsoidModel->convertXYZToLatLongHeight(point.x(),point.y(),point.z(),point.y(),point.x(),point.z());
					point.x() = osg::RadiansToDegrees(point.x());
					point.y() = osg::RadiansToDegrees(point.y());
					ln->AddPoint(point.x(),point.y());
				}
				multiline->AddLine(ln);
				CGlbLine* ln2 = new CGlbLine;
				for (size_t i = 2;i < vertexes->size();i++)
				{
					osg::Vec3d point = (*vertexes)[i] * mat;
					g_ellipsoidModel->convertXYZToLatLongHeight(point.x(),point.y(),point.z(),point.y(),point.x(),point.z());
					point.x() = osg::RadiansToDegrees(point.x());
					point.y() = osg::RadiansToDegrees(point.y());
					ln2->AddPoint(point.x(),point.y());
				}
				multiline->AddLine(ln2);
			}
			break;
		case GLB_ARROWPATTERN_D5:
			{
				glbDouble Arrow_y = min(Arrowlength*0.66,abs(abs(Arrowlength-Arrowwidth)));
				osg::ref_ptr<osg::Vec3dArray> vertexes = new osg::Vec3dArray;
				vertexes->push_back(osg::Vec3d(-Arrowwidth/6,0.0,0.0));
				vertexes->push_back(osg::Vec3d(Arrowwidth/6,0.0,0.0));
				vertexes->push_back(osg::Vec3d(Arrowwidth/6,Arrow_y,0.0));
				vertexes->push_back(osg::Vec3d(-Arrowwidth/6,Arrow_y,0.0));
				vertexes->push_back(osg::Vec3d(-Arrowwidth/6,0.0,0.0));

				vertexes->push_back(osg::Vec3d(0.0,Arrowlength,0.0));
				vertexes->push_back(osg::Vec3d(-Arrowwidth/2,Arrow_y,0.0));
				vertexes->push_back(osg::Vec3d(Arrowwidth/2,Arrow_y,0.0));
				vertexes->push_back(osg::Vec3d(0.0,Arrowlength,0.0));
				//将vertexes变换到地球上
				osg::Matrixd mat;
				if(obj->GetGlobe()->GetType() == GLB_GLOBETYPE_GLOBE)
					g_ellipsoidModel->computeLocalToWorldTransformFromXYZ(point0.x(),point0.y(),point0.z(),mat);
				else if (obj->GetGlobe()->GetType() == GLB_GLOBETYPE_FLAT)
					mat.makeTranslate(point0);
				glbDouble yaw = 0.0;
				glbDouble pitch = 0.0;
				glbDouble roll = 0.0;
				if (lineArrowInfo->roll)
					roll = lineArrowInfo->roll->GetValue(feature);
				osg::Vec3d point1_0;
				if (obj->GetGlobe()->GetType() == GLB_GLOBETYPE_GLOBE)
				{
					osg::Matrix locTowor;
					g_ellipsoidModel->computeLocalToWorldTransformFromXYZ(point0.x(),point0.y(),point0.z(),locTowor);
					point1_0 = point1 * osg::Matrix::inverse(locTowor);

				}else if (obj->GetGlobe()->GetType() == GLB_GLOBETYPE_FLAT)
					point1_0 = point1-point0;
				point1_0.normalize();
				if (point1_0.z() > 1.0)
					point1_0.z() = 1.0;
				else if (point1_0.z() < -1.0)
					point1_0.z() = -1.0;
				pitch = asin(point1_0.z());
				yaw = atan2(-point1_0.x(),point1_0.y());
				roll = osg::DegreesToRadians(roll);
				osg::Vec3d yaw_vec(0.0,0.0,1.0);
				osg::Vec3d pitch_vec(cos(yaw),sin(yaw),0.0);
				osg::Vec3d roll_vec(-sin(yaw)*cos(pitch),cos(pitch)*cos(yaw),sin(pitch));
				osg::Quat quat;
				quat.makeRotate(yaw,yaw_vec,pitch,pitch_vec,roll,roll_vec);
				mat.preMultRotate(quat);

				CGlbLine* ln = new CGlbLine;
				for (glbInt32 i = 0;i < 5;i++)
				{
					osg::Vec3d point = (*vertexes)[i] * mat;
					g_ellipsoidModel->convertXYZToLatLongHeight(point.x(),point.y(),point.z(),point.y(),point.x(),point.z());
					point.x() = osg::RadiansToDegrees(point.x());
					point.y() = osg::RadiansToDegrees(point.y());
					ln->AddPoint(point.x(),point.y());
				}
				multiline->AddLine(ln);
				CGlbLine* ln2 = new CGlbLine;
				for (size_t i = 5;i < vertexes->size();i++)
				{
					osg::Vec3d point = (*vertexes)[i] * mat;
					g_ellipsoidModel->convertXYZToLatLongHeight(point.x(),point.y(),point.z(),point.y(),point.x(),point.z());
					point.x() = osg::RadiansToDegrees(point.x());
					point.y() = osg::RadiansToDegrees(point.y());
					ln2->AddPoint(point.x(),point.y());
				}
				multiline->AddLine(ln2);
			}
			break;
		}
		if (multiline->GetCount()>0)
			mpt_outline = multiline;
	}
	return mpt_outline.get();
}

void CGlbGlobeLineArrowSymbol::DealAggRender(renb_type &renb,CGlbGlobeRObject *obj,IGlbGeometry* geom,glbInt32 imageW,glbInt32 imageH,CGlbExtent &ext)
{
	CGlbMultiLine* multiLine = dynamic_cast<CGlbMultiLine*>(geom);
	if (multiLine==NULL)
		return;

	agg::path_storage ps;	//顶点源
	agg::trans_affine mtx; //变换矩阵
	tempExtent = ext;	
	tempImageW = imageW;
	tempImageH = imageH;

	typedef agg::conv_transform<agg::path_storage> ell_ct_type;
	ell_ct_type ctell(ps,mtx);	//矩阵变换

	typedef agg::conv_contour<ell_ct_type> ell_cc_type;
	ell_cc_type ccell(ctell);	//轮廓变换

	typedef  agg::conv_dash<ell_cc_type>  ell_cd_type;
	ell_cd_type  cdccell(ccell);

	typedef agg::conv_stroke<ell_cd_type> ell_ct_cs_type;
	ell_ct_cs_type csell(cdccell);	//转换成多义线

	agg::rasterizer_scanline_aa<> ras;//处理纹理
	agg::rasterizer_scanline_aa<> rasOutline;	//绘制轮廓线
	agg::rasterizer_scanline_aa<> rasFillpolygon;//绘制填充面
	agg::scanline_p8 sl;

	GlbRenderInfo *renderInfo = obj->GetRenderInfo();
	GlbLineArrowSymbolInfo *lineArrowInfo = 
		static_cast<GlbLineArrowSymbolInfo *>(renderInfo);
	if(renderInfo == NULL)
		return;
	CGlbFeature *feature = obj->GetFeature();		

	ras.add_path(ccell);
	glbBool isRenderTex = false;

	glbInt32 lnCnt = multiLine->GetCount();
	for (glbInt32 i =0 ; i < lnCnt; i++)
	{
		CGlbLine* ln = const_cast<CGlbLine*>(multiLine->GetLine(i));
		glbref_ptr<IGlbGeometry> outline = NULL;
		if (!geom)
			outline = GetOutline(obj,ln);
		else
			outline = geom;

		if (!outline)
			continue;

		// 矢量绘制到renb
		FillDrawVertexArray(ps,outline.get());

		// 图像绘制到renb
		DealAggTexture(ras,renb,outline.get(),lineArrowInfo,feature,isRenderTex);
	}	
	//线颜色、透明度
	glbInt32 lineColor = 0xFFFFFFFF;
	glbInt32 lineOpacity = 100;
	agg::rgba8 rgbaLine(255,255,255,255);
	if (lineArrowInfo->linePixelInfo)
	{
		//轮廓线型
		if (lineArrowInfo->linePixelInfo->linePattern)
		{
			if (lineArrowInfo->linePixelInfo->linePattern->GetValue(feature) == GLB_LINE_SOLID)
				cdccell.add_dash(1,0);
			else if(lineArrowInfo->linePixelInfo->linePattern->GetValue(feature) == GLB_LINE_DOTTED)
				cdccell.add_dash(5,5);
		}
		//线宽
		if (lineArrowInfo->linePixelInfo->lineWidth)
			csell.width(lineArrowInfo->linePixelInfo->lineWidth->GetValue(feature));		
		rasFillpolygon.add_path(ccell);
		rasOutline.add_path(csell);
		
		if (lineArrowInfo->linePixelInfo->lineColor)
			lineColor = lineArrowInfo->linePixelInfo->lineColor->GetValue(feature);
		if (lineArrowInfo->linePixelInfo->lineOpacity)
			lineOpacity = lineArrowInfo->linePixelInfo->lineOpacity->GetValue(feature);
		rgbaLine = agg::rgba8(GetBValue(lineColor),LOBYTE(((lineColor & 0xFFFF)) >> 8),GetRValue(lineColor),lineOpacity*255/100.0);
	}
	agg::render_scanlines_aa_solid(rasOutline,sl,renb,rgbaLine);	
	glbInt32 fillColor = 0xFFFFFFFF;
	glbInt32 fillOpacity = 100;
	agg::rgba8 rgbafill(255,255,255,255);
	if (lineArrowInfo->fillColor)
		fillColor = lineArrowInfo->fillColor->GetValue(feature);
	if (lineArrowInfo->fillOpacity)
		fillOpacity = lineArrowInfo->fillOpacity->GetValue(feature);
	rgbafill = agg::rgba8(GetBValue(fillColor),LOBYTE(((fillColor & 0xFFFF)) >> 8),GetRValue(fillColor),fillOpacity*255/100.0);
	agg::render_scanlines_aa_solid(rasFillpolygon,sl,renb,rgbafill);
}

void CGlbGlobeLineArrowSymbol::DealAggTexture(agg::rasterizer_scanline_aa<> &ras, renb_type &renb,
											IGlbGeometry* geom,GlbLineArrowSymbolInfo *lineArrowInfo,CGlbFeature *feature,glbBool &isRenderTex)
{
	if (!lineArrowInfo)
		return;	
	GlbGlobeTexRepeatModeEnum repeatMode = 
		(GlbGlobeTexRepeatModeEnum)lineArrowInfo->texRepeatMode->GetValue(feature);
	glbFloat tilingU = 1,tilingV = 1;
	if (repeatMode==GLB_TEXTUREREPEAT_TIMES)
	{
		if (lineArrowInfo->tilingU)
			tilingU = lineArrowInfo->tilingU->GetValue(feature);
		if(lineArrowInfo->tilingV)
			tilingV = lineArrowInfo->tilingV->GetValue(feature);
	}
	else
	{
		glbInt32 tilingU_size, tilingV_size;
		if (lineArrowInfo->tilingU)
			tilingU_size = lineArrowInfo->tilingU->GetValue(feature);
		if(lineArrowInfo->tilingV)
			tilingV_size = lineArrowInfo->tilingV->GetValue(feature);

		CGlbExtent* cglbOrginExtent = const_cast<CGlbExtent*>(geom->GetExtent());

		tilingU = cglbOrginExtent->GetXWidth() / tilingU_size;
		tilingV = cglbOrginExtent->GetYHeight() / tilingV_size;
	}

	if (tilingU==1 && tilingV==1)
	{// 非重复纹理
		DealAggWrapTexture(ras,renb,geom,lineArrowInfo,feature,isRenderTex);	
	}
	else if (tilingU>=1 && tilingV>=1)
	{
		DealAggRepeatTexture(ras,renb,geom,lineArrowInfo,feature,isRenderTex,tilingU,tilingV);		
	}
}

void CGlbGlobeLineArrowSymbol::DealAggRepeatTexture(agg::rasterizer_scanline_aa<> &ras,renb_type &renb,IGlbGeometry* geom,GlbLineArrowSymbolInfo *lineArrowInfo,CGlbFeature *feature,glbBool &isRenderTex,glbFloat tilingU,glbFloat tilingV)
{
	CGlbMultiLine* multiline = dynamic_cast<CGlbMultiLine*>(geom);
	if (!multiline)
		return;

	osg::ref_ptr<osg::Image> image = NULL;
	if(lineArrowInfo && lineArrowInfo->textureData)
	{
		CGlbWString filePath = lineArrowInfo->textureData->GetValue(feature);
		//CGlbWString extname = CGlbPath::GetExtentname(filePath);
		glbInt32 index = filePath.find_first_of(L'.');
		if(index == 0)
		{
			CGlbWString execDir = CGlbPath::GetExecDir();
			filePath = execDir + filePath.substr(1,filePath.size());
		}
		image = osgDB::readImageFile(filePath.ToString());
		if (image==NULL && filePath.length()>0)
			GlbLogWOutput(GLB_LOGTYPE_ERR,L"读取线箭头图像文件(%s)失败. \r\n",filePath.ToString().c_str());
	}

	if(image == NULL)
		return;

	CGlbExtent* cglbExtent = const_cast<CGlbExtent*>(multiline->GetExtent());

	double cx,cy;
	cglbExtent->GetCenter(&cx,&cy);

	// 需要纹理渲染
	isRenderTex = true;
	double rotation = 0.0;
	if(lineArrowInfo->textureRotation)
		rotation = lineArrowInfo->textureRotation->GetValue(feature);	

	agg::scanline_p8 sl;
	agg::int8u alpha(255);//图片透明度
	if(lineArrowInfo->fillOpacity)
		alpha = agg::int8u(lineArrowInfo->fillOpacity->GetValue(feature) * 255 / 100.0);
	typedef agg::renderer_base<agg::pixfmt_rgba32> renderer_base_type;

	//typedef agg::renderer_scanline_aa_solid<renderer_base_type> renderer_scanline_type;
	//renderer_scanline_type rensl(renb);
	agg::rendering_buffer rbuf_img(NULL,0,0,0);
	GLenum dataType = (GLenum)image->getPixelFormat();

	unsigned char* pdata=NULL;
	if(dataType == GL_RGB)
		rbuf_img = agg::rendering_buffer(image->data(),image->s(),image->t(), 3 * image->s());
	else if(dataType == GL_RGBA)
	{
		int width = image->s();
		int height = image->t();
		int cnt = image->s() * image->t() * 3;
		pdata = new unsigned char[cnt];
		unsigned char *data = image->data();
		for(int i = 0; i < image->t();i++)
		{
			for(int j = 0; j < image->s();j++)
			{
				unsigned char* r = data + ( i * image->s() + j) * 4;
				unsigned char* g = data + ( i * image->s() + j) * 4 + 1;
				unsigned char* b = data + ( i * image->s() + j) * 4 + 2;
				unsigned char* a = data + ( i * image->s() + j) * 4 + 3;

				pdata[(i * image->s() + j) * 3] = *r;
				pdata[(i * image->s() + j) * 3 + 1] = *g;
				pdata[(i * image->s() + j) * 3 + 2] = *b;
			}
		}
		// image里的图案作为填充来源
		rbuf_img = agg::rendering_buffer(pdata,image->s(),image->t(),  3 * image->s());
	}	

	// 像素格式 我用的bmp是24位的 
	agg::pixfmt_rgb24 pixf_img(rbuf_img);

	// 线段分配器 
	typedef agg::span_allocator<agg::rgba8> span_allocator_type;//分配器类型
	span_allocator_type span_alloc;	// span_allocator

	// 插值器
	typedef agg::span_interpolator_linear<> interpolator_type;//插值器类型 
	// 插值器的变换矩阵
	agg::trans_affine img_mtx; 
	interpolator_type ip_img(img_mtx); // 插值器

	// Alpha处理
	agg::trans_affine alpha_mtx;
	interpolator_type ip_alpha(alpha_mtx);
	typedef agg::gradient_x gradientF_type;
	gradientF_type grF;

	typedef std::vector<agg::int8u> alphaF_type;
	alphaF_type alphaF(image->s());
	for (int i = 0; i < image->s();i++)
		alphaF[i] = alpha;

	typedef agg::span_gradient_alpha<agg::rgba8,
		interpolator_type,gradientF_type,alphaF_type> alpha_span_gen_type;
	alpha_span_gen_type alpha_span_gen(ip_alpha,grF,alphaF,0,150);

	// 图像访问器Image Accessor	
	typedef agg::image_accessor_wrap<agg::pixfmt_rgb24,	agg::wrap_mode_repeat,agg::wrap_mode_repeat> image_accessor_type;
	image_accessor_type    accessor(pixf_img);

	// 使用span_image_filter_rgb_bilinear 
	typedef agg::span_image_filter_rgb_bilinear< image_accessor_type, interpolator_type > pic_span_gen_type;
	pic_span_gen_type pic_span_gen(accessor, ip_img);

	// 这个就是Span Generator 
	typedef agg::span_converter<pic_span_gen_type,
		alpha_span_gen_type> span_gen_type;
	span_gen_type span_gen(pic_span_gen,alpha_span_gen);

	// 组合成渲染器 可以按指定的图案或不同的颜色（如渐变）填充顶点源里的多边形
	agg::renderer_scanline_aa<renderer_base_type,
		span_allocator_type,span_gen_type> my_renderer(renb,span_alloc,span_gen);

	// 1. 首先实现旋转
	img_mtx.translate(-image->s()*0.5,-image->t()*0.5);
	img_mtx.rotate(agg::deg2rad(rotation));
	img_mtx.translate(image->s()*0.5,image->t()*0.5);
	// 2. 图像缩放
	double imgscalex = double(tempImageW) / image->s() * (cglbExtent->GetXWidth() / tempExtent.GetXWidth());
	double imgscaley = double(tempImageH) / image->t() * (cglbExtent->GetYHeight() / tempExtent.GetYHeight());	
	img_mtx.scale(imgscalex / tilingU,imgscaley / tilingV);	

	// 3. 先把图像中心点偏移到(cglbExtent->GetLeft(),cglbExtent->GetBottom())处,以实现中心旋转
	double new_basex = cglbExtent->GetLeft();
	double new_basey = cglbExtent->GetBottom();
	double transx_first = (new_basex-tempExtent.GetLeft()) * tempImageW / tempExtent.GetXWidth();
	double transy_first = (new_basey-tempExtent.GetBottom()) * tempImageH / tempExtent.GetYHeight();
	img_mtx.translate(transx_first,transy_first);		

	// 4. 矩阵取反
	img_mtx.invert();//注意这里


	// Rasterizer & scanline 栅格扫描线
	// renderer_scanline_aa （还有一个兄弟版本renderer_scanline_bin）可以按指定的图案或不同的颜色（如渐变）填充顶点源里的多边形。其中的模板参数 SpanAllocator用于准
	// 备span，我们直接使用agg::span_allocator就行。这里的SpanGenerator就是本节要说的线段生成器，它决定了最终用什么东西填到rendering_buffer里
	agg::rasterizer_scanline_aa<> texRas;//处理纹理

	glbDouble ptx,pty;
	glbInt32 lnCnt = multiline->GetCount();
	for(glbInt32 i = 0; i < lnCnt; i++)
	{
		CGlbLine* ln = const_cast<CGlbLine*>(multiline->GetLine(i));
		glbInt32 ptCnt = ln->GetCount();
		for (glbInt32 j = 0; j < ptCnt; j++)
		{
			ln->GetPoint(j,&ptx,&pty);
			double new_x = (ptx - tempExtent.GetLeft()) * (tempImageW / (tempExtent.GetRight() - tempExtent.GetLeft()));
			double new_y = (pty - tempExtent.GetBottom()) * (tempImageH / (tempExtent.GetTop() - tempExtent.GetBottom()));			

			if(j == 0)
			{
				texRas.move_to_d(new_x,new_y);
				continue;
			}
			texRas.line_to_d(new_x,new_y);		
		}
		agg::render_scanlines(texRas,sl,my_renderer);
	}	

	if(pdata)
	{
		delete[] pdata;
		pdata = NULL;
	}
}
void CGlbGlobeLineArrowSymbol::DealAggWrapTexture(agg::rasterizer_scanline_aa<> &ras,renb_type &renb,IGlbGeometry* geom,GlbLineArrowSymbolInfo *lineArrowInfo,CGlbFeature *feature,glbBool &isRenderTex)
{
	CGlbMultiLine* multiline = dynamic_cast<CGlbMultiLine*>(geom);
	if (!multiline)
		return;

	osg::ref_ptr<osg::Image> image = NULL;
	if(lineArrowInfo && lineArrowInfo->textureData)
	{
		CGlbWString filePath = lineArrowInfo->textureData->GetValue(feature);
		//CGlbWString extname = CGlbPath::GetExtentname(filePath);
		glbInt32 index = filePath.find_first_of(L'.');
		if(index == 0)
		{
			CGlbWString execDir = CGlbPath::GetExecDir();
			filePath = execDir + filePath.substr(1,filePath.size());
		}
		image = osgDB::readImageFile(filePath.ToString());
		if (image==NULL && filePath.length()>0)
			GlbLogWOutput(GLB_LOGTYPE_ERR,L"读取线箭头图像文件(%s)失败. \r\n",filePath.ToString().c_str());
	}

	if(image == NULL)
		return;

	CGlbExtent* cglbExtent = const_cast<CGlbExtent*>(multiline->GetExtent());

	double cx,cy;
	cglbExtent->GetCenter(&cx,&cy);

	// 需要纹理渲染
	isRenderTex = true;
	double rotation = 0.0;
	if(lineArrowInfo->textureRotation)
		rotation = lineArrowInfo->textureRotation->GetValue(feature);	

	agg::scanline_p8 sl;
	agg::int8u alpha(255);//图片透明度
	if(lineArrowInfo->fillOpacity)
		alpha = agg::int8u(lineArrowInfo->fillOpacity->GetValue(feature) * 255 / 100.0);
	typedef agg::renderer_base<agg::pixfmt_rgba32> renderer_base_type;

	//typedef agg::renderer_scanline_aa_solid<renderer_base_type> renderer_scanline_type;
	//renderer_scanline_type rensl(renb);
	agg::rendering_buffer rbuf_img(NULL,0,0,0);
	GLenum dataType = (GLenum)image->getPixelFormat();

	unsigned char* pdata=NULL;
	if(dataType == GL_RGB)
		rbuf_img = agg::rendering_buffer(image->data(),image->s(),image->t(), 3 * image->s());
	else if(dataType == GL_RGBA)
	{
		int width = image->s();
		int height = image->t();
		int cnt = image->s() * image->t() * 3;
		pdata = new unsigned char[cnt];
		unsigned char *data = image->data();
		for(int i = 0; i < image->t();i++)
		{
			for(int j = 0; j < image->s();j++)
			{
				unsigned char* r = data + ( i * image->s() + j) * 4;
				unsigned char* g = data + ( i * image->s() + j) * 4 + 1;
				unsigned char* b = data + ( i * image->s() + j) * 4 + 2;
				unsigned char* a = data + ( i * image->s() + j) * 4 + 3;

				pdata[(i * image->s() + j) * 3] = *r;
				pdata[(i * image->s() + j) * 3 + 1] = *g;
				pdata[(i * image->s() + j) * 3 + 2] = *b;
			}
		}
		// image里的图案作为填充来源
		rbuf_img = agg::rendering_buffer(pdata,image->s(),image->t(),  3 * image->s());
	}	

	// 像素格式 我用的bmp是24位的 
	agg::pixfmt_rgba32 pixf_img(rbuf_img);

	// 线段分配器 
	typedef agg::span_allocator<agg::rgba8> span_allocator_type;//分配器类型
	span_allocator_type span_alloc;	// span_allocator

	// 插值器
	typedef agg::span_interpolator_linear<> interpolator_type;//插值器类型 
	// 插值器的变换矩阵
	agg::trans_affine img_mtx; 
	interpolator_type ip_img(img_mtx); // 插值器

	// 使用trans_warp_magnifier
	//typedef agg::span_interpolator_trans<agg::trans_warp_magnifier> interpolator_type; //插值器类型
	//agg::trans_warp_magnifier mag;
	//interpolator_type ip_imag(mag);
	//mag.magnification(0.5);
	//mag.center(100,100);
	//mag.radius(50);

	// Alpha处理
	agg::trans_affine alpha_mtx;
	interpolator_type ip_alpha(alpha_mtx);
	typedef agg::gradient_x gradientF_type;
	gradientF_type grF;

	typedef std::vector<agg::int8u> alphaF_type;
	alphaF_type alphaF(image->s());
	for (int i = 0; i < image->s();i++)
		alphaF[i] = alpha;

	typedef agg::span_gradient_alpha<agg::rgba8,
		interpolator_type,gradientF_type,alphaF_type> alpha_span_gen_type;
	alpha_span_gen_type alpha_span_gen(ip_alpha,grF,alphaF,0,150);

	// 图像类线段生成器
	typedef agg::span_image_filter_rgb_bilinear_clip<agg::pixfmt_rgba32,
		interpolator_type> pic_span_gen_type;
	pic_span_gen_type pic_span_gen(pixf_img,agg::rgba(1,1,1,1),ip_img);

	// 这个就是Span Generator 
	typedef agg::span_converter<pic_span_gen_type,
		alpha_span_gen_type> span_gen_type;
	span_gen_type span_gen(pic_span_gen,alpha_span_gen);


	// 组合成渲染器 可以按指定的图案或不同的颜色（如渐变）填充顶点源里的多边形
	agg::renderer_scanline_aa<renderer_base_type,
		span_allocator_type,span_gen_type> my_renderer(renb,span_alloc,span_gen);

	// Rasterizer & scanline 栅格扫描线
	// renderer_scanline_aa （还有一个兄弟版本renderer_scanline_bin）可以按指定的图案或不同的颜色（如渐变）填充顶点源里的多边形。其中的模板参数 SpanAllocator用于准
	// 备span，我们直接使用agg::span_allocator就行。这里的SpanGenerator就是本节要说的线段生成器，它决定了最终用什么东西填到rendering_buffer里
	//agg::rasterizer_scanline_aa<> texRas;//处理纹理

	// 1. 首先实现旋转
	img_mtx.translate(-image->s()*0.5,-image->t()*0.5);
	img_mtx.rotate(agg::deg2rad(rotation));
	img_mtx.translate(image->s()*0.5,image->t()*0.5);
	// 2. 图像缩放
	double imgscalex = double(tempImageW) / image->s() * (cglbExtent->GetXWidth() / tempExtent.GetXWidth());
	double imgscaley = double(tempImageH) / image->t() * (cglbExtent->GetYHeight() / tempExtent.GetYHeight());	
	img_mtx.scale(imgscalex,imgscaley);	

	// 3. 先把图像中心点偏移到(cglbExtent->GetLeft(),cglbExtent->GetBottom())处,以实现中心旋转
	double new_basex = cglbExtent->GetLeft();
	double new_basey = cglbExtent->GetBottom();
	double transx_first = (new_basex-tempExtent.GetLeft()) * tempImageW / tempExtent.GetXWidth();
	double transy_first = (new_basey-tempExtent.GetBottom()) * tempImageH / tempExtent.GetYHeight();
	img_mtx.translate(transx_first,transy_first);		

	// 4. 矩阵取反
	img_mtx.invert();//注意这里

	// Rasterizer & scanline 栅格扫描线
	// renderer_scanline_aa （还有一个兄弟版本renderer_scanline_bin）可以按指定的图案或不同的颜色（如渐变）填充顶点源里的多边形。其中的模板参数 SpanAllocator用于准
	// 备span，我们直接使用agg::span_allocator就行。这里的SpanGenerator就是本节要说的线段生成器，它决定了最终用什么东西填到rendering_buffer里
	agg::rasterizer_scanline_aa<> texRas;//处理纹理

	glbDouble ptx,pty;
	glbInt32 lnCnt = multiline->GetCount();
	for(glbInt32 i = 0; i < lnCnt; i++)
	{
		CGlbLine* ln = const_cast<CGlbLine*>(multiline->GetLine(i));
		glbInt32 ptCnt = ln->GetCount();
		for (glbInt32 j = 0; j < ptCnt; j++)
		{
			ln->GetPoint(j,&ptx,&pty);
			double new_x = (ptx - tempExtent.GetLeft()) * (tempImageW / (tempExtent.GetRight() - tempExtent.GetLeft()));
			double new_y = (pty - tempExtent.GetBottom()) * (tempImageH / (tempExtent.GetTop() - tempExtent.GetBottom()));			

			if(j == 0)
			{
				texRas.move_to_d(new_x,new_y);
				continue;
			}
			texRas.line_to_d(new_x,new_y);		
		}
		
	}	

	agg::render_scanlines(texRas,sl,my_renderer);

	if(pdata)
	{
		delete[] pdata;
		pdata = NULL;
	}
}

void CGlbGlobeLineArrowSymbol::FillDrawVertexArray(agg::path_storage &ps,IGlbGeometry *outline)
{
	CGlbMultiLine *multiline = dynamic_cast<CGlbMultiLine *>(outline);
	if(multiline == NULL)
		return;
	for (glbInt32 ii=0; ii < multiline->GetCount();ii++)
	{
		CGlbLine* line = const_cast<CGlbLine*>(multiline->GetLine(ii));
		const glbDouble *points = line->GetPoints();
		glbInt32 count = line->GetCount();
		for (glbInt32 i = 0; i < count; ++i)
		{
			//不做矩阵变换，转换坐标用这个
			osg::Vec2d point(points[2 * i],points[2 * i + 1]);

			double new_x = (point.x() - tempExtent.GetLeft()) * (tempImageW / (tempExtent.GetRight() - tempExtent.GetLeft()));
			double new_y = (point.y() - tempExtent.GetBottom()) * (tempImageH / (tempExtent.GetTop() - tempExtent.GetBottom()));
			if(i == 0)
			{
				new_x += 1e-7; 
				ps.move_to(new_x,new_y);
				continue;
			}
			ps.line_to(new_x,new_y);
		}
	}
}