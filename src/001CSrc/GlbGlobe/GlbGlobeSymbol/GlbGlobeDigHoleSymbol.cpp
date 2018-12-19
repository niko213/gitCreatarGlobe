#include "stdafx.h"
#include "GlbGlobeDigHoleSymbol.h"
#include "GlbPolygon.h"
#include "CGlbGlobe.h"
#include "GlbGlobeTessellatorEx.h"
#include "GlbWString.h"

#include <osg/Stencil>
#include <osg/Material>
#include <osgUtil/Tessellator>
#include <osg/Texture2D>
#include <osgDB/ReadFile>
#include <osg/TexEnv>

//AGG
#include "agg_scanline_u.h"
#include "agg_pixfmt_rgb.h"
#include "agg_pixfmt_rgba.h"
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

CGlbGlobeDigHoleSymbol::CGlbGlobeDigHoleSymbol(void)
{
}


CGlbGlobeDigHoleSymbol::~CGlbGlobeDigHoleSymbol(void)
{
}

osg::Node* CGlbGlobeDigHoleSymbol::Draw(CGlbGlobeRObject *obj,IGlbGeometry *geo,glbDouble depth,glbBool isShowWall)
{
	//绘制侧面与底面，和蒙版
	CGlbLine* line3d = NULL;
	//获取上下底面地理坐标
	osg::ref_ptr<osg::Vec3Array> AboveFaceVertexs = new osg::Vec3Array;	//顶点坐标
	osg::ref_ptr<osg::Vec3Array> BelowFaceVertexs = new osg::Vec3Array;
	osg::ref_ptr<osg::Vec3Array> SideFaceVertexs = new osg::Vec3Array;
	osg::ref_ptr<osg::Vec2Array> BelowCroodVertexs = new osg::Vec2Array;	//纹理坐标
	osg::ref_ptr<osg::Vec2Array> SideCroodVertexs = new osg::Vec2Array;
	CGlbPolygon *polygon3d = static_cast<CGlbPolygon*>(geo);
	glbDouble xmin,ymin,zmin,xmax,ymax,zmax; 
	geo->GetExtent()->GetMax(&xmax,&ymax,&zmax);
	geo->GetExtent()->GetMin(&xmin,&ymin,&zmin);

	line3d = const_cast<CGlbLine*>(polygon3d->GetExtRing());	//外环
	for (glbInt32 i = 1;i < line3d->GetCount();++i)
	{
		osg::Vec3d point;
		line3d->GetPoint(i,&point.x(),&point.y(),&point.z());
		BelowCroodVertexs->push_back(osg::Vec2((point.x()-xmin)/(xmax-xmin),(point.y()-ymin)/(ymax-ymin)));
		if (obj->GetGlobe()->GetType() == GLB_GLOBETYPE_GLOBE)
		{			
			osg::Vec3d point2 = osg::Vec3d(point.x(),point.y(),point.z()-depth);
			g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(point.y()),osg::DegreesToRadians(point.x()),point.z(),point.x(),point.y(),point.z());
			g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(point2.y()),osg::DegreesToRadians(point2.x()),point2.z(),point2.x(),point2.y(),point2.z());
			AboveFaceVertexs->push_back(point);
			BelowFaceVertexs->push_back(point2);
		}else if (obj->GetGlobe()->GetType() == GLB_GLOBETYPE_FLAT)
		{
			osg::Vec3d point2 = osg::Vec3d(point.x(),point.y(),point.z()-depth);
			AboveFaceVertexs->push_back(point);
			BelowFaceVertexs->push_back(point2);
		}
	}

	//顶点偏移，以对象中心为原点地理坐标系，世界坐标转换成区域坐标
	osg::Vec3d center;
	osg::Matrixd worldTocenter;
	polygon3d->GetExtent()->GetCenter(&center.x(),&center.y(),&center.z());
	if (obj->GetGlobe()->GetType() == GLB_GLOBETYPE_GLOBE)
	{	
		osg::Matrixd centerToworld;
		g_ellipsoidModel->computeLocalToWorldTransformFromLatLongHeight(osg::DegreesToRadians(center.y()),osg::DegreesToRadians(center.x()),center.z(),centerToworld);
		worldTocenter = osg::Matrixd::inverse(centerToworld);
	}else if (obj->GetGlobe()->GetType() == GLB_GLOBETYPE_FLAT)
	{
		worldTocenter.makeIdentity();
		worldTocenter.makeTranslate(-center.x(),-center.y(),-center.z());
	}
	for (glbInt32 i = 0;i < line3d->GetCount()-1;++i)
	{
		(*AboveFaceVertexs)[i] = (*AboveFaceVertexs)[i] * worldTocenter;
		(*BelowFaceVertexs)[i] = (*BelowFaceVertexs)[i] * worldTocenter;
		SideFaceVertexs->push_back((*AboveFaceVertexs)[i]);
		SideFaceVertexs->push_back((*BelowFaceVertexs)[i]);
	}
	SideFaceVertexs->push_back((*AboveFaceVertexs)[0]);
	SideFaceVertexs->push_back((*BelowFaceVertexs)[0]);

	glbDouble linelength = 0.0;	//外环线长
	for (glbInt32 i = 0;i < BelowFaceVertexs->size()-1;++i)
	{
		linelength += ((*BelowFaceVertexs)[i+1]-(*BelowFaceVertexs)[i]).length();
	}
	linelength += ((*BelowFaceVertexs)[0]-(*BelowFaceVertexs)[BelowFaceVertexs->size()-1]).length();
	//侧面纹理坐标
	glbDouble lnlg = 0.0;
	SideCroodVertexs->push_back(osg::Vec2(0.0,1.0));
	SideCroodVertexs->push_back(osg::Vec2(0.0,0.0));
	for (glbInt32 i = 0;i < line3d->GetCount() - 2;++i)
	{
		lnlg += ((*BelowFaceVertexs)[i+1]-(*BelowFaceVertexs)[i]).length();
		SideCroodVertexs->push_back(osg::Vec2(lnlg/linelength,1.0));
		SideCroodVertexs->push_back(osg::Vec2(lnlg/linelength,0.0));		
	}
	SideCroodVertexs->push_back(osg::Vec2(1.0,1.0));
	SideCroodVertexs->push_back(osg::Vec2(1.0,0.0));

	osg::Group* group = new osg::Group;
	osg::ref_ptr<osg::Geode> geodeAbove = new osg::Geode;
	osg::ref_ptr<osg::Geode> geodeBelow = new osg::Geode;
	osg::ref_ptr<osg::Geode> geodeSide = new osg::Geode;
	osg::ref_ptr<osg::Geometry> geomStencil = new osg::Geometry;	//蒙版
	osg::ref_ptr<osg::Geometry> geomBelow = new osg::Geometry;	//下底面
	osg::ref_ptr<osg::Geometry> geomSide = new osg::Geometry;	//侧面	

	//蒙版几何体,此几何体全透明
	//AboveFaceVertexs->pop_back();
	geomStencil->setVertexArray(AboveFaceVertexs.get());
	geomStencil->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::POLYGON,0,AboveFaceVertexs->size()));

	//下底面
	//BelowFaceVertexs->pop_back();
	geomBelow->setVertexArray(BelowFaceVertexs.get());
	//BelowCroodVertexs->pop_back();
	geomBelow->setTexCoordArray(0,BelowCroodVertexs.get());
	geomBelow->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::POLYGON,0,BelowFaceVertexs->size()));
	//侧面
	geomSide->setVertexArray(SideFaceVertexs.get());
	geomSide->setTexCoordArray(0,SideCroodVertexs.get());
	geomSide->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUAD_STRIP,0,SideFaceVertexs->size()));

	osg::ref_ptr<GlbGlobe::TessellatorEx> tscx = new GlbGlobe::TessellatorEx();
	tscx->setTessellationType(GlbGlobe::TessellatorEx::TESS_TYPE_GEOMETRY);
	tscx->setBoundaryOnly(false);
	tscx->setWindingType(GlbGlobe::TessellatorEx::TESS_WINDING_ODD);
	tscx->setTessellationNormal(osg::Vec3d(0.0,0.0,1.0));
	tscx->retessellatePolygons(*geomStencil.get());
	geodeAbove->addDrawable(geomStencil.get());	

	osg::ref_ptr<GlbGlobe::TessellatorEx> tscx2 = new GlbGlobe::TessellatorEx();
	tscx2->setTessellationType(GlbGlobe::TessellatorEx::TESS_TYPE_GEOMETRY);
	tscx2->setBoundaryOnly(false);
	tscx2->setWindingType(GlbGlobe::TessellatorEx::TESS_WINDING_ODD);
	tscx2->setTessellationNormal(osg::Vec3d(0.0,0.0,1.0));
	tscx2->retessellatePolygons(*geomBelow.get());
	//osg::ref_ptr<osgUtil::Tessellator> tscx2 = new osgUtil::Tessellator(); 
	//tscx2->setTessellationType(osgUtil::Tessellator::TESS_TYPE_GEOMETRY);
	//tscx2->setBoundaryOnly(false);
	//tscx2->setWindingType( osgUtil::Tessellator::TESS_WINDING_ODD);
	//tscx2->setTessellationNormal(osg::Vec3(0.0,0.0,1.0));
	//tscx2->retessellatePolygons(*geomBelow.get());
	geodeBelow->addDrawable(geomBelow.get());
	geodeSide->addDrawable(geomSide.get());

	group->addChild(geodeAbove.get());
	osg::ref_ptr<osg::Switch> wallnode = new osg::Switch;
	wallnode->setName("wall");
	wallnode->addChild(geodeBelow.get());
	wallnode->addChild(geodeSide.get());	
	wallnode->setValue(0,isShowWall);
	wallnode->setValue(1,isShowWall);
	group->addChild(wallnode.get());	//1
	

	////地形 stencil 设置
	//osg::ref_ptr<osg::Stencil> terrianStencil = new osg::Stencil;
	//terrianStencil->setFunction(osg::Stencil::EQUAL,0,0xff);
	//terrianStencil->setOperation(osg::Stencil::ZERO,osg::Stencil::KEEP,osg::Stencil::KEEP);
	osg::Node* pTerrianNode = obj->GetGlobe()->GetTerrain()->GetNode();
	//osg::ref_ptr<osg::StateSet> statesetBin2 = pTerrianNode->getOrCreateStateSet();
	glbInt32 terrianRenderBin = pTerrianNode->getOrCreateStateSet()->getRenderingHint();
	//statesetBin2->setAttributeAndModes(terrianStencil,osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);	

	//面片 stencil 设置
	osg::ref_ptr<osg::Stencil> groupStencil = new osg::Stencil;
	groupStencil->setFunction(osg::Stencil::ALWAYS,1,0xff);
	groupStencil->setOperation(osg::Stencil::KEEP,osg::Stencil::KEEP,osg::Stencil::REPLACE);
	osg::ref_ptr<osg::StateSet> statesetBin1 = group->getChild(0)->getOrCreateStateSet();
	statesetBin1->setRenderBinDetails(terrianRenderBin - 1,"RenderBin");
	statesetBin1->setAttributeAndModes(groupStencil,osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
	osg::ref_ptr<osg::ColorMask> colorMask = new osg::ColorMask();
	colorMask->setMask(false,false,false,false);
	statesetBin1->setAttribute(colorMask.get());

	//蒙版材质透明度
	osg::ref_ptr<osg::Material> aboveMaterial = new osg::Material;
	aboveMaterial->setAmbient(osg::Material::FRONT_AND_BACK,osg::Vec4(0.0f,0.0f,1.0f,1.0f));
	aboveMaterial->setDiffuse(osg::Material::FRONT_AND_BACK,osg::Vec4(0.5f,0.5f,0.5f,0.8f));
	aboveMaterial->setAlpha(osg::Material::FRONT_AND_BACK,0.0f);
	geodeAbove->getOrCreateStateSet()->setAttribute(aboveMaterial.get(),osg::StateAttribute::ON | osg::StateAttribute::PROTECTED);
	//下底面材质，纹理
	osg::ref_ptr<osg::Material> belowMaterial = new osg::Material;
	belowMaterial->setAmbient(osg::Material::FRONT_AND_BACK,osg::Vec4(1.0f,0.0f,0.0f,1.0f));
	belowMaterial->setDiffuse(osg::Material::FRONT_AND_BACK,osg::Vec4(0.5f,0.5f,0.5f,1.0f));
	geodeBelow->getOrCreateStateSet()->setAttribute(belowMaterial.get(),osg::StateAttribute::ON | osg::StateAttribute::PROTECTED);
	geodeBelow->getOrCreateStateSet()->setMode(GL_LIGHTING,osg::StateAttribute::OFF);
	//底面纹理
	CGlbFeature* feature = obj->GetFeature();
	GlbDigHoleSymbolInfo* digholeInfo = static_cast<GlbDigHoleSymbolInfo*>(obj->GetRenderInfo());
	CGlbWString dpath0;
	if (digholeInfo && digholeInfo->belowTextureData)
		dpath0 = digholeInfo->belowTextureData->GetValue(feature);
	//CGlbWString dpath = CGlbPath::GetExecDir();
	//CGlbWString dpath0 = dpath + L"\\res\\006.png";
	osg::ref_ptr<osg::Texture2D> Belowtex2d = new osg::Texture2D;
	glbInt32 index = dpath0.find_first_of(L'.');
	if(index == 0)
	{
		CGlbWString execDir = CGlbPath::GetExecDir();
		dpath0 = execDir + dpath0.substr(1,dpath0.size());
	}
	osg::ref_ptr<osg::Image> image0 = osgDB::readImageFile(dpath0.ToString().c_str());
	if (image0.valid())
		Belowtex2d->setImage(0,image0.get());
	else if (dpath0.length()>0)
	{
		GlbLogWOutput(GLB_LOGTYPE_ERR,L"读取挖洞对象图像文件(%s)失败. \r\n",dpath0.ToString().c_str());
	}
	Belowtex2d->setWrap(osg::Texture::WRAP_S,osg::Texture2D::CLAMP);
	Belowtex2d->setWrap(osg::Texture::WRAP_T,osg::Texture2D::CLAMP);
	//设置纹理环境
	osg::ref_ptr<osg::TexEnv> texenv=new osg::TexEnv;
	texenv->setMode(osg::TexEnv::REPLACE);
	//启用纹理单元0
	geodeBelow->getOrCreateStateSet()->setTextureAttributeAndModes(0,Belowtex2d.get(),osg::StateAttribute::ON);
	//设置纹理环境
	geodeBelow->getOrCreateStateSet()->setTextureAttribute(0,texenv.get());

	//侧面材质，纹理
	osg::ref_ptr<osg::Material> sideMaterial = new osg::Material;
	sideMaterial->setAmbient(osg::Material::FRONT_AND_BACK,osg::Vec4(0.0f,0.0f,1.0f,1.0f));
	sideMaterial->setDiffuse(osg::Material::FRONT_AND_BACK,osg::Vec4(0.5f,0.5f,0.5f,1.0f));
	geodeSide->getOrCreateStateSet()->setAttribute(sideMaterial.get(),osg::StateAttribute::ON | osg::StateAttribute::PROTECTED);
	//geodeSide->getOrCreateStateSet()->setMode(GL_LIGHTING,osg::StateAttribute::OFF);
	//侧面纹理
	CGlbWString dpath1;
	if (digholeInfo && digholeInfo->sideTextureData)
		dpath1 = digholeInfo->sideTextureData->GetValue(feature);
	//CGlbWString dpath1 = dpath + L"\\res\\006.png";
	osg::ref_ptr<osg::Texture2D> Sidetex2d = new osg::Texture2D;
	index = dpath1.find_first_of(L'.');
	if(index == 0)
	{
		CGlbWString execDir = CGlbPath::GetExecDir();
		dpath1 = execDir + dpath1.substr(1,dpath1.size());
	}
	osg::ref_ptr<osg::Image> image1 = osgDB::readImageFile(dpath1.ToString().c_str());
	if (image1.valid())
		Sidetex2d->setImage(0,image1.get());
	Sidetex2d->setWrap(osg::Texture::WRAP_S,osg::Texture2D::CLAMP);
	Sidetex2d->setWrap(osg::Texture::WRAP_T,osg::Texture2D::CLAMP);
	//启用纹理单元0
	geodeSide->getOrCreateStateSet()->setTextureAttributeAndModes(0,Sidetex2d.get(),osg::StateAttribute::ON);
	//设置纹理环境
	geodeSide->getOrCreateStateSet()->setTextureAttribute(0,texenv.get());

	return group;
}

void CGlbGlobeDigHoleSymbol::DrawToImage(CGlbGlobeRObject *obj,IGlbGeometry* geom, glbByte *image,glbInt32 imageW,glbInt32 imageH,CGlbExtent &ext)
{
	glbref_ptr<IGlbGeometry> outline = obj->GetOutLine();
	if(outline == NULL) return;

	// 判断ext是否与outline范围相交，不相交则不需要处理
	CGlbExtent* outln_ext = const_cast<CGlbExtent*>(outline->GetExtent());
	glbDouble outln_mx,outln_Mx,outln_my,outln_My;
	outln_ext->GetMin(&outln_mx,&outln_my);
	outln_ext->GetMax(&outln_Mx,&outln_My);
	glbDouble mx,my,Mx,My;
	ext.GetMin(&mx,&my);
	ext.GetMax(&Mx,&My);
	glbDouble minx = max(mx,outln_mx);
	glbDouble miny = max(my,outln_my);
	glbDouble maxx = min(Mx,outln_Mx);
	glbDouble maxy = min(My,outln_My);
	if (minx<=maxx && miny<=maxy)
	{// outline的外包与ext范围有交集
		agg::rendering_buffer rbuf(image, imageW, imageH, -imageW * 4);
		agg::pixfmt_rgba32 pixf(rbuf);
		renb_type renb(pixf);//底层渲染器 

		DealAggRender(renb,obj,geom,imageW,imageH,ext);
		////////////////////////////////////////////////////////////////////////
		glbDouble xStep = (Mx-mx) / imageW;
		glbDouble yStep = (My-my) / imageH;

		// 列
		glbInt32 starti = floor(outln_mx - mx)/xStep;	
		starti = max(starti,0);							
		starti = min(starti,imageW);
		glbInt32 endi = ceil(outln_Mx - mx)/xStep;
		//endi = imageW - endi;
		endi = max(endi,0);
		endi = min(endi,imageW);
		// 行
		glbInt32 startj = floor(outln_my - my)/yStep;
		startj = max(startj,0);
		startj = min(startj,imageH);
		glbInt32 endj = ceil(outln_My - my)/yStep;
		//endj = imageH - endj;
		endj = max(endj,0);
		endj = min(endj,imageH);		

		CGlbMultiPolygon* multiPolygon = static_cast<CGlbMultiPolygon*>(outline.get());
		glbInt32 num = multiPolygon->GetCount();
		for (glbInt32 k = 0;k < num;k++)
		{
			CGlbPolygon* polygon = const_cast<CGlbPolygon*>(multiPolygon->GetPolygon(k));
			CGlbLine* line = const_cast<CGlbLine*>(polygon->GetExtRing());
			glbInt32 pointnum = line->GetCount();
			glbDouble* polygonPoint = (glbDouble*)line->GetPoints();
			glbDouble lon,lat;
			for (glbInt32 i = starti;i < endi;i++)
			{// 列
				for (glbInt32 j = startj;j < endj;j++)
				{// 行	
					//判断点(lon,lat)是否在多边形内
					lon = mx + i*xStep + 0.5*xStep;
					lat = my + j*yStep + 0.5*yStep;
					if(PtInPolygon(lon,lat,polygonPoint,pointnum))
					{						
						*(image+4*((imageH-1-j)*imageW+i)+3) = 0x00;
					}
				}
			}
		}		
	}
}

bool CGlbGlobeDigHoleSymbol::PtInPolygon(double px,double py, double* polygon_pts, int ptCnt)
{
	int nCross = 0;
	for(int i=0;i<ptCnt-1;i++)
	{
		double p1_x = polygon_pts[i*2];
		double p1_y = polygon_pts[i*2+1];
		double p2_x = polygon_pts[(i+1)*2];
		double p2_y = polygon_pts[(i+1)*2+1];
		// 求解 y=p.y 与 p1p2 的交点
		if(p1_y == p2_y)// p1p2 与 y=p0.y平行
			continue;
		if ( py < min(p1_y, p2_y) ) // 交点在p1p2延长线上
			continue;
		if ( py >= max(p1_y, p2_y) ) // 交点在p1p2延长线上
			continue;
		// 求交点的 X 坐标 --------------------------------------------------------------
		double x = (double)(py - p1_y) * (double)(p2_x - p1_x) / (double)(p2_y - p1_y) + p1_x;
		if(x > px)
			nCross++; // 只统计单边交点
	}
	return (nCross%2==1);
}

IGlbGeometry* CGlbGlobeDigHoleSymbol::GetOutline(CGlbGlobeRObject *obj,IGlbGeometry *geo)
{
	CGlbMultiPolygon* multiPolygon = new CGlbMultiPolygon;
	CGlbPolygon* polygon3d =  dynamic_cast<CGlbPolygon*>(geo);
	if (!polygon3d)
		return NULL;
	CGlbLine* line3d = const_cast<CGlbLine*>(polygon3d->GetExtRing());	//只有外环，无内环
	CGlbLine* ln = new CGlbLine;
	glbDouble ptx,pty,ptz;
	for (glbInt32 i = 0; i < line3d->GetCount(); i++)
	{					
		line3d->GetPoint(i,&ptx,&pty,&ptz);
		ln->AddPoint(ptx,pty);
	}
	CGlbPolygon* polygon = new CGlbPolygon;
	polygon->SetExtRing(ln);
	multiPolygon->AddPolygon(polygon);
	return multiPolygon;
}

void CGlbGlobeDigHoleSymbol::DealAggRender(renb_type &renb,CGlbGlobeRObject *obj,IGlbGeometry* geom,glbInt32 imageW,glbInt32 imageH,CGlbExtent &ext)
{
	// 绘制对象几何类型必须是 CGlbMultiPolygon类型
	CGlbMultiPolygon* multPoly = dynamic_cast<CGlbMultiPolygon*>(geom);
	if (!multPoly)
		return;

	agg::path_storage ps;
	agg::trans_affine mtx;		 

	tempExtent = ext;
	tempImageW = imageW;
	tempImageH = imageH;

	typedef agg::conv_transform<agg::path_storage> ell_ct_type;
	ell_ct_type ctell(ps,mtx);//矩阵变换

	typedef agg::conv_contour<ell_ct_type> ell_cc_type;	//
	ell_cc_type ccell(ctell);

	typedef  agg::conv_dash<ell_cc_type>  ell_cd_type;
	ell_cd_type  cdccell(ccell);		

	typedef agg::conv_stroke<ell_cd_type> ell_ct_cs_type;
	ell_ct_cs_type csell(cdccell);

	agg::rasterizer_scanline_aa<> ras;//处理纹理
	agg::rasterizer_scanline_aa<> rasOutline;
	//agg::rasterizer_scanline_aa<> rasFillpolygon;
	// 修改scanline_p8   ---> scanline32_p8 避免agg中出现int32->int16的类型转换，会导致系统崩溃 malin  2016.1.13
	agg::scanline32_p8 sl;

	// 图像绘制到renb
	ras.add_path(ccell);
	glbInt32 polyCnt = multPoly->GetCount();
	for(glbInt32 i = 0 ; i <polyCnt; i++)
	{
		CGlbPolygon* poly = const_cast<CGlbPolygon*>(multPoly->GetPolygon(i));		
		// 矢量绘制到renb
		FillDrawVertexArray(ps,poly);	
	}	
	GlbRenderInfo *renderInfo = obj->GetRenderInfo();
	GlbDigHoleSymbolInfo *digHoleInfo= dynamic_cast<GlbDigHoleSymbolInfo *>(renderInfo);

	CGlbFeature *feature = obj->GetFeature();

	if(digHoleInfo->lineInfo && digHoleInfo->lineInfo->lineWidth)
		csell.width(digHoleInfo->lineInfo->lineWidth->GetValue(feature)/* * (ext.GetRight() - ext.GetLeft())  /  imageW*/);//线宽
	//rasFillpolygon.add_path(ccell);
	glbInt32 lineColor = 0xFFFFFFFF,polygonColor = 0xFFFFFFFF;
	agg::rgba8 rgbaLine(255,255,255,255),rgbaPolygon(255,255,255,255);

	//轮廓线型
	if (digHoleInfo->lineInfo && digHoleInfo->lineInfo->linePattern)
	{
		if (digHoleInfo->lineInfo->linePattern->GetValue(feature) == GLB_LINE_SOLID)
			cdccell.add_dash(1,0);
		else if(digHoleInfo->lineInfo->linePattern->GetValue(feature) == GLB_LINE_DOTTED)
			cdccell.add_dash(5,5);
	}
	rasOutline.add_path(csell);
	if(digHoleInfo->lineInfo && digHoleInfo->lineInfo->lineColor)
		lineColor = digHoleInfo->lineInfo->lineColor->GetValue(feature);
	if(digHoleInfo->lineInfo && digHoleInfo->lineInfo->lineOpacity)
	{
		rgbaLine = agg::rgba8(GetBValue(lineColor),LOBYTE(((lineColor & 0xFFFF)) >> 8),GetRValue(lineColor),digHoleInfo->lineInfo->lineOpacity->GetValue(feature) * 255 / 100.0);//设置线的颜色【透明度】
	}
	else if(digHoleInfo->lineInfo && digHoleInfo->lineInfo->lineOpacity == NULL)
	{
		rgbaLine = agg::rgba8(GetBValue(lineColor),LOBYTE(((lineColor & 0xFFFF)) >> 8),GetRValue(lineColor),GetAValue(lineColor));
	}
	//rgbaPolygon = agg::rgba8(255,255,255,255);

	agg::render_scanlines_aa_solid(rasOutline,sl,renb,rgbaLine);
	//agg::render_scanlines_aa_solid(rasFillpolygon,sl,renb,rgbaPolygon);
}

void CGlbGlobeDigHoleSymbol::FillDrawVertexArray(agg::path_storage &ps,IGlbGeometry *outline)
{
	CGlbPolygon* poly = dynamic_cast<CGlbPolygon*>(outline);
	if (!poly)
		return;

	glbDouble ptx,pty;			
	CGlbLine* extRing = const_cast<CGlbLine*>(poly->GetExtRing());
	glbInt32 ptCnt = extRing->GetCount();
	for (glbInt32 i = 0; i < ptCnt; i++)
	{
		extRing->GetPoint(i,&ptx,&pty);
		double new_x = (ptx - tempExtent.GetLeft()) * (tempImageW / (tempExtent.GetRight() - tempExtent.GetLeft()));
		double new_y = (pty - tempExtent.GetBottom()) * (tempImageH / (tempExtent.GetTop() - tempExtent.GetBottom()));			

		if(i == 0)
		{
			new_x += 1e-7;
			ps.move_to(new_x,new_y);
			continue;
		}
		ps.line_to(new_x,new_y);		
	}

	glbInt32 inRingCnt = poly->GetInRingCount();
	for (glbInt32 j = 0; j < inRingCnt; j++)
	{
		CGlbLine* inRing = const_cast<CGlbLine*>(poly->GetInRing(j));
		glbInt32 ptCnt = inRing->GetCount();
		for (glbInt32 k = 0; k < ptCnt; k++)
			//for (glbInt32 k = ptCnt-1; k <= 0; k--)
		{
			inRing->GetPoint(k,&ptx,&pty);
			double new_x = (ptx - tempExtent.GetLeft()) * (tempImageW / (tempExtent.GetRight() - tempExtent.GetLeft()));
			double new_y = (pty - tempExtent.GetBottom()) * (tempImageH / (tempExtent.GetTop() - tempExtent.GetBottom()));			

			if(k == 0)
			{
				ps.move_to(new_x,new_y);
				continue;
			}
			ps.line_to(new_x,new_y);		
		}
	}	
}