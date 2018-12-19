#include "StdAfx.h"
#include "GlbGlobeLinePixelSymbol.h"
#include "GlbLine.h"
#include "osg/Geode"
#include "osg/Geometry"
#include "osg/LineWidth"
#include "osg/BlendColor"
#include "osg/LineStipple"
#include "CGlbGlobe.h"
#include "agg_rendering_buffer.h"
#include <osg/Material>

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

#include "util/agg_color_conv.h"
#include "util/agg_color_conv_rgb8.h"
#include "platform/agg_platform_support.h"

#include "osg/BlendFunc"
#include "osg/Depth"

using namespace GlbGlobe;

CGlbGlobeLinePixelSymbol::CGlbGlobeLinePixelSymbol(void)
{
}

CGlbGlobeLinePixelSymbol::~CGlbGlobeLinePixelSymbol(void)
{
}

osg::Node * CGlbGlobeLinePixelSymbol::Draw( CGlbGlobeRObject *obj,IGlbGeometry *geo )
{
	IGlbGeometry *geometry = geo;
	glbref_ptr<CGlbLine> line3D = dynamic_cast<CGlbLine*>(geometry);
	if (line3D.get() == NULL || line3D->GetCount() < 2)
		return NULL;

	CGlbFeature *feature = obj->GetFeature();
	GlbRenderInfo *renderInfo = obj->GetRenderInfo();
	GlbLinePixelSymbolInfo *linePixelInfo = 
		static_cast<GlbLinePixelSymbolInfo*>(renderInfo);
	osg::Geode *geode = new osg::Geode;
	osg::Geometry *lineGeom = new osg::Geometry;
	osg::Vec3dArray *vertexes = new osg::Vec3dArray;
	const glbDouble *points = line3D->GetPoints();
	if (obj->GetGlobe()->GetType() == GLB_GLOBETYPE_GLOBE)
	{
		glbDouble xLon = 0.0,yLat = 0.0;
		for (glbInt32 i = 0; i < line3D->GetCount();++i)
		{
			osg::Vec3d point(points[i * 3],points[i * 3 + 1],points[i * 3 + 2]);
			xLon = point.x();
			yLat = point.y();			
			if(obj->GetAltitudeMode() == GLB_ALTITUDEMODE_RELATIVETERRAIN)
				point.z() += obj->GetGlobe()->GetElevationAt(xLon,yLat);
			g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(point.y()),osg::DegreesToRadians(point.x()),point.z(),point.x(),point.y(),point.z());

			vertexes->push_back(point);
		}
	}
	else if (obj->GetGlobe()->GetType() == GLB_GLOBETYPE_FLAT)
	{
		for (glbInt32 i = 0; i < line3D->GetCount();++i)
		{
			vertexes->push_back(
				osg::Vec3d(points[i * 3],points[i * 3 + 1],points[i * 3 + 2]));
		}
	}

	osg::Vec3Array *renderVertexes = new osg::Vec3Array;
	osg::ref_ptr<osg::Vec3dArray> nomalver = new osg::Vec3dArray;
	nomalver->push_back(osg::Vec3d(0.0,0.0,1.0));
	lineGeom->setNormalArray(nomalver.get());
	lineGeom->setNormalBinding(osg::Geometry::BIND_OVERALL);

	osg::Matrixd localToworld;
	osg::Matrixd worldTolacal;
	if(obj->GetGlobe()->GetType() == GLB_GLOBETYPE_GLOBE)
	{
		g_ellipsoidModel->computeLocalToWorldTransformFromXYZ(vertexes->at(0).x(),vertexes->at(0).y(),
			                                                  vertexes->at(0).z(),localToworld);
		worldTolacal = osg::Matrixd::inverse(localToworld);
	}
	else
	{
		localToworld.makeTranslate(vertexes->at(0).x(),vertexes->at(0).y(),vertexes->at(0).z());
		worldTolacal = osg::Matrixd::inverse(localToworld);
	}
	for (size_t i = 0; i < vertexes->size(); ++i)
	{
		//if(obj->GetGlobe()->GetType() == GLB_GLOBETYPE_GLOBE)
			renderVertexes->push_back(vertexes->at(i)*worldTolacal);
		//else
		//	renderVertexes->push_back(vertexes->at(i));
	}
	lineGeom->setVertexArray(renderVertexes);
	osg::Vec4Array *colorArray = new osg::Vec4Array;
	colorArray->push_back(GetColor(linePixelInfo->lineColor->GetValue(feature)));
	//lineGeom->setColorArray(colorArray);
	//lineGeom->setColorBinding(osg::Geometry::BIND_OVERALL);
	lineGeom->addPrimitiveSet(
		new osg::DrawArrays(osg::PrimitiveSet::LINE_STRIP,0,renderVertexes->size()));

	osg::StateSet *stateset = geode->getOrCreateStateSet();

	//设置材质
	osg::ref_ptr<osg::Material> material = dynamic_cast<osg::Material*>(stateset->getAttribute(osg::StateAttribute::MATERIAL));
	if (!material.valid())
		material = new osg::Material;
	stateset->setAttribute(material.get(),osg::StateAttribute::ON);	
	stateset->setMode(GL_DEPTH_TEST,osg::StateAttribute::ON);
	material->setDiffuse(osg::Material::FRONT_AND_BACK,osg::Vec4(0.0,0.0,0.0,(glbFloat)linePixelInfo->lineOpacity->GetValue(feature)/100.0));
	material->setEmission(osg::Material::FRONT_AND_BACK,osg::Vec4(GetBValue(linePixelInfo->lineColor->GetValue(feature))/255.0,LOBYTE(((linePixelInfo->lineColor->GetValue(feature) & 0xFFFF)) >> 8)/255.0,GetRValue(linePixelInfo->lineColor->GetValue(feature))/255.0,(glbFloat)linePixelInfo->lineOpacity->GetValue(feature)/100.0));
	if (linePixelInfo->lineOpacity->GetValue(feature) < 100)
	{
		stateset->setMode(GL_BLEND,osg::StateAttribute::ON);
		if (stateset->getRenderingHint() != osg::StateSet::TRANSPARENT_BIN)
			stateset->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
	}else{
		stateset->setMode(GL_BLEND,osg::StateAttribute::OFF);
		stateset->setRenderingHint(osg::StateSet::OPAQUE_BIN);
	}

	//线宽
	osg::LineWidth *lineWidth = new osg::LineWidth;
	lineWidth->setWidth(linePixelInfo->lineWidth->GetValue(feature));
	stateset->setAttributeAndModes(lineWidth,osg::StateAttribute::ON);
	//混合透明度
	osg::BlendColor *blendColor = new osg::BlendColor(
		osg::Vec4(1.0,1.0,1.0,(float)linePixelInfo->lineOpacity->GetValue(feature)/100.0));
	stateset->setMode(GL_BLEND,osg::StateAttribute::ON);
	stateset->setAttributeAndModes(blendColor,osg::StateAttribute::ON);
	if (linePixelInfo->linePattern->GetValue(feature) == GLB_LINE_DOTTED)
	{
		//线样式
		osg::LineStipple *lineStipple = new osg::LineStipple;
		lineStipple->setFactor(1);
		lineStipple->setPattern(0x1C47);
		stateset->setAttributeAndModes(lineStipple,osg::StateAttribute::ON);
	}

	//stateset->setAttributeAndModes( new osg::Depth( osg::Depth::LESS, 0, 1, false ) ); 
	//stateset->setAttributeAndModes( new osg::Depth(osg::Depth::ALWAYS, 0, 1, false), osg::StateAttribute::ON );
	osg::ref_ptr<osg::BlendFunc> pBlendFun = new osg::BlendFunc(osg::BlendFunc::SRC_ALPHA,osg::BlendFunc::ONE_MINUS_SRC_ALPHA );	//设置混合方程 
	stateset->setAttributeAndModes(pBlendFun.get(),osg::StateAttribute::ON);
	//stateset->setRenderBinDetails( obj->GetRenderOrder()+1, "RenderBin"); 

	geode->addDrawable(lineGeom);
	return geode;
}

osg::Node * CGlbGlobeLinePixelSymbol::DrawBorderline( CGlbGlobeRObject *obj,IGlbGeometry *geo,GlbRenderInfo *renderInfo )
{
	IGlbGeometry *geometry = geo;
	CGlbLine *line3D = dynamic_cast<CGlbLine*>(geometry);
	if (line3D == NULL)
		return NULL;
	CGlbFeature *feature = obj->GetFeature();
	GlbLinePixelSymbolInfo *linePixelInfo = 
		static_cast<GlbLinePixelSymbolInfo*>(renderInfo);
	osg::Geode *geode = new osg::Geode;
	osg::Geometry *lineGeom = new osg::Geometry;
	osg::Vec3dArray *vertexes = new osg::Vec3dArray;
	const glbDouble *points = line3D->GetPoints();
	if (obj->GetGlobe()->GetType() == GLB_GLOBETYPE_GLOBE)
	{
		glbDouble xLon = 0.0,yLat = 0.0;
		for (glbInt32 i = 0; i < line3D->GetCount();++i)
		{
			osg::Vec3d point(points[i * 3],points[i * 3 + 1],points[i * 3 + 2]);
			xLon = point.x();
			yLat = point.y();		
			if(obj->GetAltitudeMode() == GLB_ALTITUDEMODE_RELATIVETERRAIN)
				point.z() += point.z() + obj->GetGlobe()->GetElevationAt(xLon,yLat);
			g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(point.y()),osg::DegreesToRadians(point.x()),point.z(),point.x(),point.y(),point.z());	
			vertexes->push_back(point);
		}
	}
	else if (obj->GetGlobe()->GetType() == GLB_GLOBETYPE_FLAT)
	{
		for (glbInt32 i = 0; i < line3D->GetCount();++i)
		{
			vertexes->push_back(
				osg::Vec3d(points[i * 3],points[i * 3 + 1],points[i * 3 + 2]));
		}
	}

	osg::Vec3Array *renderVertexes = new osg::Vec3Array;
	osg::ref_ptr<osg::Vec3dArray> nomalver = new osg::Vec3dArray;
	nomalver->push_back(osg::Vec3d(0.0,0.0,1.0));
	lineGeom->setNormalArray(nomalver.get());
	lineGeom->setNormalBinding(osg::Geometry::BIND_OVERALL);
	if(GLB_GLOBETYPE_GLOBE == obj->GetGlobe()->GetType())
	{
		osg::Matrixd localToworld;
		g_ellipsoidModel->computeLocalToWorldTransformFromXYZ(vertexes->at(0).x(),vertexes->at(0).y(),vertexes->at(0).z(),localToworld);
		osg::Matrixd worldTolocal = osg::Matrixd::inverse(localToworld);
		for (size_t i = 0; i < vertexes->size(); ++i)
		{
			renderVertexes->push_back(vertexes->at(i)*worldTolocal);
		}
	}
	else if(GLB_GLOBETYPE_FLAT == obj->GetGlobe()->GetType())
	{
		osg::Matrixd trans;
		trans.makeTranslate(vertexes->at(0).x(),vertexes->at(0).y(),vertexes->at(0).z());
		osg::Matrixd tempTrans = osg::Matrixd::inverse(trans);
		for (size_t i = 0; i < vertexes->size(); ++i)
		{
			renderVertexes->push_back(vertexes->at(i) * tempTrans);
		}
	}
	lineGeom->setVertexArray(renderVertexes);
	osg::Vec4Array *colorArray = new osg::Vec4Array;
	colorArray->push_back(GetColor(linePixelInfo->lineColor->GetValue(feature)));
	lineGeom->setColorArray(colorArray);
	lineGeom->setColorBinding(osg::Geometry::BIND_OVERALL);
	lineGeom->addPrimitiveSet(
		new osg::DrawArrays(osg::PrimitiveSet::LINE_STRIP,0,renderVertexes->size()));

	osg::StateSet *stateset = geode->getOrCreateStateSet();

	//设置材质
	osg::ref_ptr<osg::Material> material = dynamic_cast<osg::Material*>(stateset->getAttribute(osg::StateAttribute::MATERIAL));
	if (!material.valid())
		material = new osg::Material;
	stateset->setAttribute(material.get(),osg::StateAttribute::ON);	
	stateset->setMode(GL_DEPTH_TEST,osg::StateAttribute::ON);
	material->setDiffuse(osg::Material::FRONT_AND_BACK,osg::Vec4(0.0,0.0,0.0,(glbFloat)linePixelInfo->lineOpacity->GetValue(feature)/100.0));
	material->setEmission(osg::Material::FRONT_AND_BACK,osg::Vec4(GetBValue(linePixelInfo->lineColor->GetValue(feature))/255.0,LOBYTE(((linePixelInfo->lineColor->GetValue(feature) & 0xFFFF)) >> 8)/255.0,GetRValue(linePixelInfo->lineColor->GetValue(feature))/255.0,(glbFloat)linePixelInfo->lineOpacity->GetValue(feature)/100.0));
	if (linePixelInfo->lineOpacity->GetValue(feature) < 100)
	{
		stateset->setMode(GL_BLEND,osg::StateAttribute::ON);
		if (stateset->getRenderingHint() != osg::StateSet::TRANSPARENT_BIN)
			stateset->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
	}else{
		stateset->setMode(GL_BLEND,osg::StateAttribute::OFF);
		stateset->setRenderingHint(osg::StateSet::OPAQUE_BIN);
	}

	//线宽
	osg::LineWidth *lineWidth = new osg::LineWidth;
	lineWidth->setWidth(linePixelInfo->lineWidth->GetValue(feature));
	stateset->setAttributeAndModes(lineWidth,osg::StateAttribute::ON);
	//混合透明度
	osg::BlendColor *blendColor = new osg::BlendColor(
		osg::Vec4(1.0,1.0,1.0,(float)linePixelInfo->lineOpacity->GetValue(feature)/100.0));
	stateset->setMode(GL_BLEND,osg::StateAttribute::ON);
	stateset->setAttributeAndModes(blendColor,osg::StateAttribute::ON);
	if (linePixelInfo->linePattern->GetValue(feature) == GLB_LINE_DOTTED)
	{
		//线样式
		osg::LineStipple *lineStipple = new osg::LineStipple;
		lineStipple->setFactor(1);
		lineStipple->setPattern(0x1C47);
		stateset->setAttributeAndModes(lineStipple,osg::StateAttribute::ON);
	}

	geode->addDrawable(lineGeom);
	return geode;
}

void CGlbGlobeLinePixelSymbol::DrawToImage(CGlbGlobeRObject *obj, IGlbGeometry* geom,glbByte *image/*RGBA*/,glbInt32 imageW, glbInt32 imageH,CGlbExtent &ext )
{	
	//改由上层地形块tile逻辑中判断是否需要绘制==符号认为只要调用DrawToImage就是要绘制 2014.10.17 马林
	//CGlbFeature *feature = obj->GetFeature();
	//IGlbGeometry *outline = obj->GetOutLine();
	//if(outline == NULL) 
	//	return;

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
	//if (minx<=maxx && miny<=maxy)	// outline的外包与ext范围有交集
	{
		agg::rendering_buffer rbuf(image,imageW,imageH,-imageW * 4);
		agg::pixfmt_rgba32 pixf(rbuf);
		renb_type renb(pixf);

		DealAggRender(renb,obj,geom,imageW,imageH,ext);		
	}
}

typedef Coordinate PT;
geos::geom::Geometry* CGlbGlobeLinePixelSymbol::Interection(IGlbGeometry *geom, CGlbExtent &ext)
{// 计算ext与geom的相交区域
	// CGlbLine  --->  Geometry(GEOS_MULTILINE/GEOS_LINE)
	CGlbLine* ln = dynamic_cast<CGlbLine*>(geom);	
	if (ln==NULL) return NULL;
	glbDouble minX,minY,maxX,maxY;
	ext.Get(&minX,&maxX,&minY,&maxY);	

	geos::geom::GeometryFactory factory;
	geos::geom::CoordinateArraySequenceFactory csf;

	// 1. 由ext生成rectPoly
	geos::geom::CoordinateSequence* cs1 = csf.create(5,2);
	cs1->setAt(PT(minX,minY,0),0);
	cs1->setAt(PT(maxX,minY,0),1);
	cs1->setAt(PT(maxX,maxY,0),2);
	cs1->setAt(PT(minX,maxY,0),3);
	cs1->setAt(PT(minX,minY,0),4);
	geos::geom::LinearRing* ring1 = factory.createLinearRing(cs1);
	geos::geom::Geometry* rectPoly = factory.createPolygon(ring1,NULL);

	// 2. 由geom生成Line	
	glbInt32 ptCnt = ln->GetCount();
	const glbDouble* pts = ln->GetPoints();
	geos::geom::CoordinateSequence* cs2 = csf.create(ptCnt,2);
	for(glbInt32 k = 0; k < ptCnt; k++)
	{				
		cs2->setAt(PT(pts[2*k],pts[2*k+1],0),k);
	}
	geos::geom::LineString* shell = factory.createLineString(cs2);	

	geos::geom::Geometry* outGeom=NULL;
	//3. 求交集
	if (shell && rectPoly)
	{
		//outGeom = shell->intersection(rectPoly);
		outGeom = rectPoly->intersection(shell);
		if (outGeom)
		{
			geos::geom::GeometryTypeId typeId = outGeom->getGeometryTypeId();
			std::string type = outGeom->getGeometryType();			
		}
	}	

	//4. 清除创建的对象
	if (rectPoly) delete rectPoly;
	if (shell) delete shell;

	return outGeom;
}

void CGlbGlobeLinePixelSymbol::FillDrawVertexArray(agg::path_storage &ps,geos::geom::Geometry* outline)
{
	PT c;
	geos::geom::GeometryTypeId typeId = outline->getGeometryTypeId();
	switch(typeId)
	{
	case GEOS_LINESTRING:
	case GEOS_LINEARRING:
		{
			//geos::geom::LineString* ln = dynamic_cast<geos::geom::LineString*>(outline);
			geos::geom::CoordinateSequence* coors = outline->getCoordinates();
			size_t count = coors->getSize();
			for (size_t k = 0; k < count; k++)
			{
				coors->getAt(k,c);
				double new_x = (c.x - tempExtent.GetLeft()) * (tempImageW / (tempExtent.GetRight() - tempExtent.GetLeft()));
				double new_y = (c.y - tempExtent.GetBottom()) * (tempImageH / (tempExtent.GetTop() - tempExtent.GetBottom()));
				if (k==0)
				{
					ps.move_to(new_x,new_y);
					continue;
				}
				ps.line_to(new_x,new_y);
			}			
		}
		break;
	case GEOS_MULTILINESTRING:
		{
			geos::geom::MultiLineString* mulLn = dynamic_cast<geos::geom::MultiLineString*>(outline);
			size_t numLn = mulLn->getNumGeometries();
			for (size_t j = 0; j < numLn; j++)
			{
				geos::geom::Geometry* geo = (geos::geom::Geometry*)(mulLn->getGeometryN(j));
				geos::geom::LineString* ln = dynamic_cast<geos::geom::LineString*>(geo);
				if (ln)
				{
					geos::geom::CoordinateSequence* coors = ln->getCoordinates();
					size_t count = coors->getSize();
					for (size_t k = 0; k < count; k++)
					{
						coors->getAt(k,c);
						double new_x = (c.x - tempExtent.GetLeft()) * (tempImageW / (tempExtent.GetRight() - tempExtent.GetLeft()));
						double new_y = (c.y - tempExtent.GetBottom()) * (tempImageH / (tempExtent.GetTop() - tempExtent.GetBottom()));
						if (k==0)
						{
							ps.move_to(new_x,new_y);
							continue;
						}
						ps.line_to(new_x,new_y);
					}			
				}
			}
		}
		break;
	}
}
void CGlbGlobeLinePixelSymbol::DealAggRender( renb_type &renb,CGlbGlobeRObject *obj,IGlbGeometry* geom,glbInt32 imageW,glbInt32 imageH,CGlbExtent &ext )
{
	glbref_ptr<CGlbMultiLine> multiLine = dynamic_cast<CGlbMultiLine*>(geom);
	if (multiLine==NULL)
		return;

	agg::path_storage ps;
	agg::trans_affine mtx;

	tempExtent = ext;
	tempImageW = imageW;
	tempImageH = imageH;

	typedef agg::conv_transform<agg::path_storage> ell_ct_type;
	ell_ct_type ctell(ps,mtx);//矩阵变换

	//typedef agg::conv_contour<ell_ct_type> ell_cc_type;
	//ell_cc_type ccell(ctell);
	typedef  agg::conv_dash<ell_ct_type>  ell_cd_type;
	ell_cd_type  cdccell(ctell);

	typedef agg::conv_stroke<ell_cd_type> ell_ct_cs_type;
	ell_ct_cs_type csell(cdccell);

	//agg::rasterizer_scanline_aa<> ras;//处理纹理
	agg::rasterizer_scanline_aa<> rasOutline;
	//agg::rasterizer_scanline_aa<> rasFillpolygon;
	agg::scanline_p8 sl;

	GlbRenderInfo *renderInfo = obj->GetRenderInfo();

	GlbLinePixelSymbolInfo* linePixelInfo = static_cast<GlbLinePixelSymbolInfo*>(renderInfo);
	if(renderInfo == NULL)
		return;
	CGlbFeature *feature = obj->GetFeature();
	
	//glbDouble area = ext.GetXWidth()*ext.GetYHeight();
	//CGlbExtent expandExt;// 扩大0.05倍的ext范围
	//osg::Vec3d _center;
	//ext.GetCenter(&_center.x(),&_center.y(),&_center.z());
	//expandExt.SetMin(_center.x()-ext.GetXWidth()*0.55,_center.y()-ext.GetYHeight()*0.55);
	//expandExt.SetMax(_center.x()+ext.GetXWidth()*0.55,_center.y()+ext.GetYHeight()*0.55);

	glbInt32 lnCnt = multiLine->GetCount();
	for(glbInt32 i = 0; i < lnCnt; i++)
	{
		CGlbLine* ln = const_cast<CGlbLine*>(multiLine->GetLine(i));
		glbref_ptr<IGlbGeometry> outline = static_cast<IGlbGeometry*>(ln);//GetOutline(obj,ln);

		// 2014.12.22 测试发现用geos进行剪裁的效率比我们自己实现的算法渲染速度要慢？？？？
		// 测试数据: 国界.shp
		//CGlbExtent mergeExt;
		//CGlbExtent* lnExt = outline->GetExtent();
		// 注意此处不能直接用lnExt->Merge(ext)因为这样会改变outline的外包的！！2014.4.17 马林
		//mergeExt.Merge(*lnExt);
		//mergeExt.Merge(ext);		
		//glbDouble area1 = mergeExt.GetXWidth()*mergeExt.GetYHeight();
		//if (area1 / area > OverlayFazhi)
		//{// 当要绘制的对象范围的面积是ext范围面积的2倍以上时，需要做overlay计算以加速栅格化
		//	geos::geom::Geometry* geom = Interection(ln,ext);
		//	if (geom)
		//	{
		//		FillDrawVertexArray(ps,geom);
		//		// 删除生成的geom
		//		delete geom;
		//	}			
		//}
		//else
		{// 矢量绘制到renb
			FillDrawVertexArray(ps,outline.get());
		}
	}	

	//轮廓线型
	if (linePixelInfo->linePattern)
	{
		if (linePixelInfo->linePattern->GetValue(feature) == GLB_LINE_SOLID)
			cdccell.add_dash(1,0);
		else if(linePixelInfo->linePattern->GetValue(feature) == GLB_LINE_DOTTED)
			cdccell.add_dash(5,5);
	}
	//线宽
	if (linePixelInfo->lineWidth)
		csell.width(linePixelInfo->lineWidth->GetValue(feature));
	rasOutline.add_path(csell);
	glbInt32 lineColor = 0xFFFFFFFF;
	agg::rgba8 rgbaLine(255,255,255,255);
	if (linePixelInfo->lineColor)
		lineColor = linePixelInfo->lineColor->GetValue(feature);
	if (linePixelInfo->lineOpacity)
		rgbaLine = agg::rgba8(GetBValue(lineColor),LOBYTE(((lineColor & 0xFFFF)) >> 8),GetRValue(lineColor), linePixelInfo->lineOpacity->GetValue(feature) * 255 / 100.0);//设置线的颜色【透明度】
	else
		rgbaLine = agg::rgba8(GetBValue(lineColor),LOBYTE(((lineColor & 0xFFFF)) >> 8),GetRValue(lineColor),GetAValue(lineColor));

	if(obj->IsSelected())
		agg::render_scanlines_aa_solid(rasOutline,sl,renb,agg::rgba8(255,0,0,128));
	else
		agg::render_scanlines_aa_solid(rasOutline,sl,renb,rgbaLine);
}

double CGlbGlobeLinePixelSymbol::Cross(osg::Vec2d v0,osg::Vec2d v1)
{
	//P × Q = x1*y2 - x2*y1
	return v0.x()*v1.y() - v1.x()*v0.y();
}

bool CGlbGlobeLinePixelSymbol::Intersect(osg::Vec2d v0,osg::Vec2d v1,osg::Vec2d v2,osg::Vec2d v3,osg::Vec2d& inter)
{
#if 0
	if (intersect(v0,v1,v2,v3)==true)
	{
		osg::Vec2d dir1 = v1 - v0;
		osg::Vec2d dir2 = v3 - v2;
		//求两向量外积，平行时外积为0
		double Corss = (dir1.x() * dir2.y() - dir1.y() * dir2.x());
		//计算二阶行列式的两个常数项
		double ConA = v0.x() * dir1.y() - v0.y() * dir1.x();//p1.x() * v1.y - p1.y() * v1.x;
		double ConB = v2.x() * dir2.y() - v2.y() * dir2.x();//p3.x() * v2.y - p3.y() * v2.x;
		//计算行列式D1和D2的值，除以系数行列式的值，得到交点坐标
		double x = (ConB * dir1.x() - ConA * dir2.x()) / Corss;
		double y = (ConB * dir1.y() - ConA * dir2.y()) / Corss;
		inter.set(x,y);
		return true;
	}
	return false;
#else
	// 1.  设以线段 P1P2 为对角线的矩形为R1， 设以线段 Q1Q2 为对角线的矩形为R2，如果R1和R2不相交，则两线段不会有交点
	double seg1_minx = min(v0.x(),v1.x());
	double seg1_maxx = max(v0.x(),v1.x());
	double seg1_miny = min(v0.y(),v1.y());
	double seg1_maxy = max(v0.y(),v1.y());

	double seg2_minx = min(v2.x(),v3.x());
	double seg2_maxx = max(v2.x(),v3.x());
	double seg2_miny = min(v2.y(),v3.y());
	double seg2_maxy = max(v2.y(),v3.y());

	double minx = max(seg1_minx,seg2_minx);
	double miny = max(seg1_miny,seg2_miny);
	double maxx = min(seg1_maxx,seg2_maxx);
	double maxy = min(seg1_maxy,seg2_maxy);
	if (minx>maxx || miny>maxy)
		return false;

	// 2.  跨立试验
	//如果两线段相交，则两线段必然相互跨立对方，所谓跨立，指的是一条线段的两端点分别位于另一线段所在直线的两边。
	//判断是否跨立，还是要用到矢量叉积的几何意义。若P1P2跨立Q1Q2 ，则矢量 ( P1 - Q1 ) 和( P2 - Q1 )位于矢量( Q2 - Q1 ) 的两侧，即：
	//( P1 - Q1 ) × ( Q2 - Q1 ) * ( P2 - Q1 ) × ( Q2 - Q1 ) < 0
	//	上式可改写成：
	//	( P1 - Q1 ) × ( Q2 - Q1 ) * ( Q2 - Q1 ) × ( P2 - Q1 ) > 0
	//	当 ( P1 - Q1 ) × ( Q2 - Q1 ) = 0 时，说明线段P1P2和Q1Q2共线（但是不一定有交点）。同理判断Q1Q2跨立P1P2的依据是：
	//	( Q1 - P1 ) × ( P2 - P1 ) * ( Q2 - P1 ) × ( P2 - P1 ) < 0
	osg::Vec2d P1 = v0;
	osg::Vec2d P2 = v1;
	osg::Vec2d Q1 = v2;
	osg::Vec2d Q2 = v3;	
	// P × Q = x1*y2 - x2*y1
	// 叉积的一个非常重要性质是可以通过它的符号判断两矢量相互之间的顺逆时针关系：
	//	　　若 P × Q > 0 , 则P在Q的顺时针方向。
	//  　　若 P × Q < 0 , 则P在Q的逆时针方向。
	//	　　若 P × Q = 0 , 则P与Q共线，但可能同向也可能反向。
	double a = Cross(P1 - Q1,Q2 - Q1);//(P1 - Q1) * (Q2 - Q1);
	double b = Cross(P2 - Q1,Q2 - Q1);//(P2 - Q1) * (Q2 - Q1);

	double c = Cross(Q1 - P1,P2 - P1);//(Q1 - P1) * (P2 - P1);
	double d = Cross(Q2 - P1,P2 - P1);// (Q2 - P1) * (P2 - P1);

	if (a*b<0 && c*d<0)
	{// 有交点
		osg::Vec2d dir1 = v1 - v0;
		osg::Vec2d dir2 = v3 - v2;
		//求两向量外积，平行时外积为0
		double Corss = (dir1.x() * dir2.y() - dir1.y() * dir2.x());
		//计算二阶行列式的两个常数项
		double ConA = v0.x() * dir1.y() - v0.y() * dir1.x();//p1.x() * v1.y - p1.y() * v1.x;
		double ConB = v2.x() * dir2.y() - v2.y() * dir2.x();//p3.x() * v2.y - p3.y() * v2.x;
		//计算行列式D1和D2的值，除以系数行列式的值，得到交点坐标
		double x = (ConB * dir1.x() - ConA * dir2.x()) / Corss;
		double y = (ConB * dir1.y() - ConA * dir2.y()) / Corss;
		inter.set(x,y);
		return true;
	}

	return false;
#endif
}
bool CGlbGlobeLinePixelSymbol::ReCalPoint(agg::path_storage &ps,osg::Vec2d& point, osg::Vec2d& nextpoint, CGlbExtent& ext)
{
	glbDouble minX,maxX,minY,maxY;
	ext.Get(&minX,&maxX,&minY,&maxY);

	glbDouble c1 = (point.x()-minX)*(point.x()-maxX);
	glbDouble c2 = (point.y()-minY)*(point.y()-maxY);
	glbDouble c3 = (nextpoint.x()-minX)*(nextpoint.x()-maxX);
	glbDouble c4 = (nextpoint.y()-minY)*(nextpoint.y()-maxY);

	if (c1<=0 && c2<=0 && c3<=0 && c4<=0)
	{// 两个点都在ext中
		double new_x = (point.x() - tempExtent.GetLeft()) * (tempImageW / (tempExtent.GetRight() - tempExtent.GetLeft()));
		double new_y = (point.y() - tempExtent.GetBottom()) * (tempImageH / (tempExtent.GetTop() - tempExtent.GetBottom()));
		ps.move_to(new_x,new_y);
		new_x = (nextpoint.x() - tempExtent.GetLeft()) * (tempImageW / (tempExtent.GetRight() - tempExtent.GetLeft()));
		new_y = (nextpoint.y() - tempExtent.GetBottom()) * (tempImageH / (tempExtent.GetTop() - tempExtent.GetBottom()));
		ps.line_to(new_x,new_y);

		return true;
	}
	// 剩下的就是 一个点在ext内，另一个点在ext外； 或者 两个点都在ext外 这两种情况
	osg::ref_ptr<osg::Vec2dArray> vcArray = new osg::Vec2dArray;
	osg::Vec2d inter;
	bool res = Intersect(point,nextpoint,osg::Vec2d(minX,minY),osg::Vec2d(maxX,minY),inter);
	if (res==true)
		vcArray->push_back(inter);		
	res = Intersect(point,nextpoint,osg::Vec2d(maxX,minY),osg::Vec2d(maxX,maxY),inter);
	if (res==true)
		vcArray->push_back(inter);
	res = Intersect(point,nextpoint,osg::Vec2d(maxX,maxY),osg::Vec2d(minX,maxY),inter);
	if (res==true)
		vcArray->push_back(inter);
	res = Intersect(point,nextpoint,osg::Vec2d(minX,maxY),osg::Vec2d(minX,minY),inter);
	if (res==true)
		vcArray->push_back(inter);

	if (vcArray->size()>0)
	{// 取距离point点近的交点,并且距离不能为0
		if (vcArray->size()==1)
		{// 只有一个交点
			if (c1<=0 && c2<=0)
			{
				double new_x = (point.x() - tempExtent.GetLeft()) * (tempImageW / (ext.GetRight() - tempExtent.GetLeft()));
				double new_y = (point.y() - tempExtent.GetBottom()) * (tempImageH / (ext.GetTop() - tempExtent.GetBottom()));
				ps.move_to(new_x,new_y);
				osg::Vec2d interpt = vcArray->at(0);
				new_x = (interpt.x() - tempExtent.GetLeft()) * (tempImageW / (tempExtent.GetRight() - tempExtent.GetLeft()));
				new_y = (interpt.y() - tempExtent.GetBottom()) * (tempImageH / (tempExtent.GetTop() - tempExtent.GetBottom()));
				ps.line_to(new_x,new_y);
			}	
			else if (c3<=0 && c4<=0)
			{
				osg::Vec2d interpt = vcArray->at(0);				
				double new_x = (interpt.x() - tempExtent.GetLeft()) * (tempImageW / (tempExtent.GetRight() - tempExtent.GetLeft()));
				double new_y = (interpt.y() - tempExtent.GetBottom()) * (tempImageH / (tempExtent.GetTop() - tempExtent.GetBottom()));
				ps.move_to(new_x,new_y);
				new_x = (nextpoint.x() - tempExtent.GetLeft()) * (tempImageW / (tempExtent.GetRight() - tempExtent.GetLeft()));
				new_y = (nextpoint.y() - tempExtent.GetBottom()) * (tempImageH / (tempExtent.GetTop() - tempExtent.GetBottom()));
				ps.line_to(new_x,new_y);
			}
		}
		else
		{
			if (vcArray->size()>2)
				int _error = 1;
			for (size_t k = 0; k < vcArray->size(); k++)
			{
				osg::Vec2d interpt = vcArray->at(k);
				double new_x = (interpt.x() - tempExtent.GetLeft()) * (tempImageW / (tempExtent.GetRight() - tempExtent.GetLeft()));
				double new_y = (interpt.y() - tempExtent.GetBottom()) * (tempImageH / (tempExtent.GetTop() - tempExtent.GetBottom()));
				if (k==0)
					ps.move_to(new_x,new_y);
				else
					ps.line_to(new_x,new_y);
			}		
		}		
		return true;
	}
	return false;
}

void CGlbGlobeLinePixelSymbol::FillDrawVertexArray(agg::path_storage &ps,IGlbGeometry *outline)
{
	CGlbLine *line = dynamic_cast<CGlbLine *>(outline);
	if(line == NULL)
		return;
	const glbDouble *points = line->GetPoints();
	glbInt32 count = line->GetCount();
#if 0
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
#else
	for (glbInt32 i = 0; i < count-1; ++i)
	{
		osg::Vec2d point(points[2 * i],points[2 * i + 1]);	
		osg::Vec2d nextpoint(points[2 * i + 2],points[2 * i + 3]);	
		ReCalPoint(ps,point, nextpoint, tempExtent);
	}	
#endif
}

IGlbGeometry* CGlbGlobeLinePixelSymbol::GetOutline(CGlbGlobeRObject *obj,IGlbGeometry *geo)
{
	if(obj->GetAltitudeMode() == GLB_ALTITUDEMODE_ONTERRAIN)
	{
		GlbRenderInfo *renderInfo = obj->GetRenderInfo();
		if(renderInfo == NULL)
			return NULL;
		GlbLinePixelSymbolInfo *linePixelInfo = static_cast<GlbLinePixelSymbolInfo *>(renderInfo);
		CGlbFeature *feature = obj->GetFeature();
		CGlbLine* ln3d = dynamic_cast<CGlbLine *>(geo);
		CGlbLine *line = new CGlbLine;
		for (glbInt32 i = 0; i < ln3d->GetCount(); i++)
		{
			osg::Vec3d point;
			ln3d->GetPoint(i,&point.x(),&point.y(),&point.z());
			line->AddPoint(point.x(),point.y());
		}
		if(line == NULL)
			return NULL;
		mpt_outline = line;
	}
	return mpt_outline.get();
}

double CGlbGlobeLinePixelSymbol::mult(osg::Vec2d a, osg::Vec2d b, osg::Vec2d c)  
{  
	return (a.x()-c.x())*(b.y()-c.y())-(b.x()-c.x())*(a.y()-c.y());  
}  

//aa, bb为一条线段两端点 cc, dd为另一条线段的两端点 相交返回true, 不相交返回false  
bool CGlbGlobeLinePixelSymbol::intersect(osg::Vec2d aa, osg::Vec2d bb, osg::Vec2d cc, osg::Vec2d dd)  
{  
	if ( max(aa.x(), bb.x())<min(cc.x(), dd.x()) )  
	{  
		return false;  
	}  
	if ( max(aa.y(), bb.y())<min(cc.y(), dd.y()) )  
	{  
		return false;  
	}  
	if ( max(cc.x(), dd.x())<min(aa.x(), bb.x()) )  
	{  
		return false;  
	}  
	if ( max(cc.y(), dd.y())<min(aa.y(), bb.y()) )  
	{  
		return false;  
	}  
	if ( mult(cc, bb, aa)*mult(bb, dd, aa)<0 )  
	{  
		return false;  
	}  
	if ( mult(aa, dd, cc)*mult(dd, bb, cc)<0 )  
	{  
		return false;  
	}  
	return true;  
}  