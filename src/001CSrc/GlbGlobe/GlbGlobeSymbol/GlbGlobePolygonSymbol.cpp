#include "StdAfx.h"
#include "GlbGlobePolygonSymbol.h"
#include "GlbGlobeSymbolCommon.h"
//#include "GlbGlobeDelaunayTriangulatorEx.h"
#include "GlbGlobeTessellatorEx.h"

#include "GlbPolygon.h"
#include "CGlbGlobe.h"
#include "GlbWString.h"
#include "GlbPath.h"
#include "GlbString.h"
#include "GlbConvert.h"

#include "osgDB/ReadFile"
#include "osg/CoordinateSystemNode"
#include "osg/Geode"
#include "osg/TexEnv"
#include "osg/BlendFunc"
#include "osg/PolygonMode"
#include "osg/LineWidth"
#include "osg/Depth"
#include "osg/LineStipple"
#include "GlbLog.h"
#include <osg/LightModel>
#include <osg/Material>

//AGG
#include "agg_rendering_buffer.h"
#include "agg_rasterizer_scanline_aa.h"
#include "agg_scanline_p.h"
#include "agg_ellipse.h"
#include "agg_path_storage.h"
#include "agg_conv_stroke.h"
#include "agg_vcgen_markers_term.h"
#include "agg_renderer_scanline.h"
#include "agg_conv_contour.h"
#include "agg_scanline_u.h"
#include "agg_pixfmt_rgb.h"
#include "agg_span_allocator.h"
#include "agg_span_interpolator_linear.h"
#include "agg_span_gradient_alpha.h"
#include "agg_span_image_filter_rgb.h"
#include "agg_span_converter.h"
#include "agg_image_accessors.h"
#include "util/agg_color_conv.h"
#include "util/agg_color_conv_rgb8.h"
#include "platform/agg_platform_support.h"
#include "platform/win32/agg_win32_bmp.h"

#include "comutil.h"
#include "GlbGlobeMath.h"

#ifdef min
#undef min
#endif

#ifdef max
#undef max
#endif

using namespace  GlbGlobe;

geos::geom::GeometryFactory factory;

CGlbGlobePolygonSymbol::CGlbGlobePolygonSymbol(void)
{
}

CGlbGlobePolygonSymbol::~CGlbGlobePolygonSymbol(void)
{
}

osg::Node * CGlbGlobePolygonSymbol::Draw(CGlbGlobeRObject *obj,IGlbGeometry *geo)
{
	if (obj==NULL||geo==NULL||obj->GetGlobe()==NULL)
		return NULL;

	GlbPolygon2DSymbolInfo *polygon2DInfo= dynamic_cast<GlbPolygon2DSymbolInfo *>(obj->GetRenderInfo());
	if(polygon2DInfo == NULL)
		return NULL;

	osg::Node *polygonNode=NULL;
	GlbGeometryTypeEnum geoType = geo->GetType();
	switch(geoType)
	{
	case GLB_GEO_POLYGON:
		{
			glbref_ptr<CGlbPolygon> geoTmp=dynamic_cast<CGlbPolygon *>(geo);
			if (geoTmp==NULL || geoTmp.get()==NULL)
				break;
			polygonNode = buildDrawable(obj,geoTmp.get());
			break;
		}
	case GLB_GEO_MULTIPOLYGON:
		{
			glbref_ptr<CGlbMultiPolygon> geoTmp=dynamic_cast<CGlbMultiPolygon *>(geo);
			if (geoTmp==NULL || geoTmp.get()==NULL)
				break;
			polygonNode = buildDrawable(obj,geoTmp.get());
			break;
		}
	default:
		{
			//polygonNode=NULL;
			//break;
		}			
	}

	return polygonNode;
}

void CGlbGlobePolygonSymbol::FillDrawVertexArray(agg::path_storage &ps,IGlbGeometry *outline)
{
	CGlbPolygon* poly = dynamic_cast<CGlbPolygon*>(outline);
	if (!poly)
		return;

	glbDouble ptx,pty;			
	CGlbLine* extRing = const_cast<CGlbLine *>(poly->GetExtRing());
	glbInt32 ptCnt = extRing->GetCount();
	for (glbInt32 i = 0; i < ptCnt; i++)
	{
		extRing->GetPoint(i,&ptx,&pty);
		double new_x = (ptx - tempExtent.GetLeft()) * (tempImageW / (tempExtent.GetRight() - tempExtent.GetLeft()));
		double new_y = (pty - tempExtent.GetBottom()) * (tempImageH / (tempExtent.GetTop() - tempExtent.GetBottom()));			

		if(i == 0)
		{
			ps.move_to(new_x,new_y);
			continue;
		}
		ps.line_to(new_x,new_y);		
	}
	ps.close_polygon();

	glbInt32 inRingCnt = poly->GetInRingCount();
	for (glbInt32 j = 0; j < inRingCnt; j++)
	{
		CGlbLine* inRing = const_cast<CGlbLine *>(poly->GetInRing(j));
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
		ps.close_polygon();
	}			
	
}

typedef Coordinate PT;
geos::geom::Geometry* CGlbGlobePolygonSymbol::Interection(CGlbPolygon *geom, CGlbExtent &ext)
{// 计算ext与geom的相交区域
	// CGlbPolygon  --->  Geometry(GEOS_MULTIPOLYGON/GEOS_POLYGON)
	if (geom==NULL) return NULL;
	CGlbPolygon* poly = geom;		
	glbDouble minX,minY,maxX,maxY;
	ext.Get(&minX,&maxX,&minY,&maxY);	

	geos::geom::CoordinateArraySequenceFactory csf;

	// 1. 由ext生成rectPoly
	geos::geom::CoordinateSequence* cs1 = csf.create(5,2);
	if (cs1==NULL) return NULL;
	cs1->setAt(PT(minX,minY,0),0);
	cs1->setAt(PT(maxX,minY,0),1);
	cs1->setAt(PT(maxX,maxY,0),2);
	cs1->setAt(PT(minX,maxY,0),3);
	cs1->setAt(PT(minX,minY,0),4);
	geos::geom::LinearRing* ring1 = factory.createLinearRing(cs1);
	if (ring1==NULL) return NULL;
	geos::geom::Polygon* rectPoly = factory.createPolygon(ring1,NULL);
	if (rectPoly==NULL) return NULL;

	// 2. 由geom生成Poly
	geos::geom::Polygon* otherPoly = NULL;
	CGlbLine* extRing = const_cast<CGlbLine *>(poly->GetExtRing());
	geos::geom::LinearRing* shell = NULL;
	
	if (extRing)
	{
		glbInt32 ptCnt = extRing->GetCount();
		const glbDouble* pts = extRing->GetPoints();
		geos::geom::CoordinateSequence* cs2 = csf.create(ptCnt+1,2);
		if (cs2)
		{
			for(glbInt32 k = 0; k < ptCnt; k++)
			{				
				cs2->setAt(PT(pts[2*k],pts[2*k+1],0),k);
			}
			cs2->setAt(PT(pts[0],pts[1],0),ptCnt);
			shell = factory.createLinearRing(cs2);
		}
	}
	glbInt32 inRingCnt = poly->GetInRingCount();
	std::vector<geos::geom::Geometry *>* holes = NULL;
	if (inRingCnt>0)
		holes = new vector<Geometry *>(inRingCnt);
	for (glbInt32 idx = 0; idx < inRingCnt; idx++)
	{
		CGlbLine* inRing = const_cast<CGlbLine *>(poly->GetInRing(idx));
		if (inRing)
		{
			glbInt32 ptCnt = inRing->GetCount();
			const glbDouble* pts = inRing->GetPoints();
			geos::geom::CoordinateSequence* cs2 = csf.create(ptCnt+1,2);
			if (cs2)
			{
				for(glbInt32 k = 0; k < ptCnt; k++)
				{				
					cs2->setAt(PT(pts[2*k],pts[2*k+1],0),k);
				}
				cs2->setAt(PT(pts[0],pts[1],0),ptCnt);
				geos::geom::LinearRing* ring = factory.createLinearRing(cs2);
				if (ring)
					(*holes)[idx] = ring;
			}
		}
	}


	if (shell && holes==NULL)	
		otherPoly = factory.createPolygon(shell,NULL);
	else if (shell && holes)
		otherPoly = factory.createPolygon(shell,holes);
	
	geos::geom::Geometry* outGeom=NULL;
	//3. 求交集
	if (otherPoly && rectPoly)
	{
		try
		{
			// 判断多边形是否是合法多边形
			bool bValid = otherPoly->isValid();
			if (bValid)
				outGeom = rectPoly->intersection(otherPoly);
			else
			{// 非法多边形
				outGeom=NULL;
			}
		}		
		catch (std::exception* e)
		{
			outGeom = NULL;
		}		
	}	
	
	//4. 清除创建的对象
	if (rectPoly) 				
		factory.destroyGeometry(rectPoly);			

	if (otherPoly)		
		factory.destroyGeometry(otherPoly);
	
	return outGeom;
}

void CGlbGlobePolygonSymbol::FillDrawVertexArray(agg::path_storage &ps,geos::geom::Polygon* poly)
{
	if (poly==NULL) return;
	const geos::geom::LineString* extstring = poly->getExteriorRing();
	geos::geom::CoordinateSequence* coors = extstring->getCoordinates();
	size_t numPt = coors->size();

	PT c;
	for (size_t i = 0; i < numPt; i++)
	{
		coors->getAt(i,c);
		double new_x = (c.x - tempExtent.GetLeft()) * (tempImageW / (tempExtent.GetRight() - tempExtent.GetLeft()));
		double new_y = (c.y - tempExtent.GetBottom()) * (tempImageH / (tempExtent.GetTop() - tempExtent.GetBottom()));			

		if(i == 0)
		{
			ps.move_to(new_x,new_y);
			continue;
		}
		ps.line_to(new_x,new_y);		
	}
	
	glbInt32 inRingCnt = poly->getNumInteriorRing();
	for (glbInt32 j = 0; j < inRingCnt; j++)
	{
		const geos::geom::LineString* interstring = poly->getInteriorRingN(j);
		coors = interstring->getCoordinates();
		size_t numPt = coors->size();		
		for (size_t k = 0; k < numPt; k++)
		{
			coors->getAt(k,c);

			double new_x = (c.x - tempExtent.GetLeft()) * (tempImageW / (tempExtent.GetRight() - tempExtent.GetLeft()));
			double new_y = (c.y - tempExtent.GetBottom()) * (tempImageH / (tempExtent.GetTop() - tempExtent.GetBottom()));			

			if(k == 0)
			{
				ps.move_to(new_x,new_y);
				continue;
			}
			ps.line_to(new_x,new_y);		
		}
	}			
}

void CGlbGlobePolygonSymbol::DealAggTexture(agg::rasterizer_scanline_aa<> &ras,
											renb_type &renb, CGlbPolygon *geom, geos::geom::Polygon* poly,GlbPolygon2DSymbolInfo *polyInfo,
											CGlbFeature *feature,glbBool &isRenderTex)
{
	if (polyInfo==NULL || polyInfo->textureData==NULL)
		return;	

	CGlbWString filePath = polyInfo->textureData->GetValue(feature);
	if (filePath.empty())
		return;	

	GlbGlobeTexRepeatModeEnum repeatMode = (GlbGlobeTexRepeatModeEnum)(polyInfo->texRepeatMode->GetValue(feature));
	glbFloat tilingU = 1,tilingV = 1;
	if (repeatMode==GLB_TEXTUREREPEAT_TIMES)
	{
		if (polyInfo->tilingU)
			tilingU = polyInfo->tilingU->GetValue(feature);
		if(polyInfo->tilingV)
			tilingV = polyInfo->tilingV->GetValue(feature);
	}
	else if (repeatMode==GLB_TEXTUREREPEAT_SIZE)
	{
		glbInt32 tilingU_size, tilingV_size;
		if (polyInfo->tilingU)
			tilingU_size = polyInfo->tilingU->GetValue(feature);
		if(polyInfo->tilingV)
			tilingV_size = polyInfo->tilingV->GetValue(feature);

		glbref_ptr<CGlbExtent> polyExt = const_cast<CGlbExtent*>(geom->GetExtent());
		tilingU = polyExt->GetXWidth() / tilingU_size + 0.5;
		tilingV = polyExt->GetYHeight() / tilingV_size + 0.5;
	}

	if (tilingU==1 && tilingV==1)
	{// 非重复纹理
		DealAggWrapTexture(ras,renb,geom,poly,polyInfo,feature,isRenderTex);	
	}
	else if (tilingU>=1 && tilingV>=1)
	{
		DealAggRepeatTexture(ras,renb,geom,poly, polyInfo,feature,isRenderTex,tilingU,tilingV);		
	}
}

void CGlbGlobePolygonSymbol::DealAggRepeatTexture(agg::rasterizer_scanline_aa<> &ras,
										renb_type &renb, IGlbGeometry *geom, geos::geom::Polygon* poly, GlbPolygon2DSymbolInfo *polyInfo,
										CGlbFeature *feature,glbBool &isRenderTex,glbFloat tilingU,glbFloat tilingV)
{	
	if (poly==NULL || geom==NULL)	return;
	//// 得到外包
	//const geos::geom::Envelope* envelope = poly->getEnvelopeInternal();
	//if (envelope==NULL || envelope->isNull())
	//	return;

	osg::ref_ptr<osg::Image> image = NULL;
	if(polyInfo && polyInfo->textureData)
	{
		CGlbWString filePath = polyInfo->textureData->GetValue(feature);
		//CGlbWString extname = CGlbPath::GetExtentname(filePath);
		glbInt32 index = filePath.find_first_of(L'.');
		if(index == 0)
		{
			CGlbWString execDir = CGlbPath::GetExecDir();
			filePath = execDir + filePath.substr(1,filePath.size());
		}
		image = osgDB::readImageFile(filePath.ToString());
		if (image==NULL && filePath.length()>0)
			GlbLogWOutput(GLB_LOGTYPE_ERR,L"读取多边形图像文件(%s)失败. \r\n",filePath.ToString().c_str());
	}

	if(image == NULL)
	{		
		return;
	}

	glbref_ptr<CGlbExtent> cglbExtent = const_cast<CGlbExtent*>(geom->GetExtent());

	// 需要纹理渲染
	isRenderTex = true;
	double rotation = 0.0;
	if(polyInfo->textureRotation)
		rotation = polyInfo->textureRotation->GetValue(feature);		

	agg::scanline_p8 sl;
	agg::int8u alpha(255);//图片透明度
	if(polyInfo->opacity)
		alpha = agg::int8u(polyInfo->opacity->GetValue(feature) * 255 / 100.0);
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

	const geos::geom::LineString* extstring = poly->getExteriorRing();
	geos::geom::CoordinateSequence* coors = extstring->getCoordinates();
	size_t numPt = coors->size();

	PT c;
	for (size_t i = 0; i < numPt; i++)
	{
		coors->getAt(i,c);
		double new_x = (c.x - tempExtent.GetLeft()) * (tempImageW / (tempExtent.GetRight() - tempExtent.GetLeft()));
		double new_y = (c.y - tempExtent.GetBottom()) * (tempImageH / (tempExtent.GetTop() - tempExtent.GetBottom()));			

		if(i == 0)
		{
			texRas.move_to_d(new_x,new_y);
			continue;
		}
		texRas.line_to_d(new_x,new_y);		
	}

	size_t inRingCnt = poly->getNumInteriorRing();
	for (size_t j = 0; j < inRingCnt; j++)
	{
		const geos::geom::LineString* interstring = poly->getInteriorRingN(j);
		coors = interstring->getCoordinates();
		size_t numPt = coors->size();		
		for (size_t k = 0; k < numPt; k++)
		{
			coors->getAt(k,c);

			double new_x = (c.x - tempExtent.GetLeft()) * (tempImageW / (tempExtent.GetRight() - tempExtent.GetLeft()));
			double new_y = (c.y - tempExtent.GetBottom()) * (tempImageH / (tempExtent.GetTop() - tempExtent.GetBottom()));			

			if(k == 0)
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

void CGlbGlobePolygonSymbol::DealAggWrapTexture(agg::rasterizer_scanline_aa<> &ras,
										renb_type &renb, IGlbGeometry *geom, geos::geom::Polygon* poly,GlbPolygon2DSymbolInfo *polyInfo,
										CGlbFeature *feature,glbBool &isRenderTex)
{	
	if (poly==NULL || geom==NULL)		return;

	// 得到外包
	const geos::geom::Envelope* envelope = poly->getEnvelopeInternal();
	if (envelope==NULL || envelope->isNull())
		return;	

	osg::ref_ptr<osg::Image> image = NULL;
	if(polyInfo && polyInfo->textureData)
	{
		CGlbWString filePath = polyInfo->textureData->GetValue(feature);
		//CGlbWString extname = CGlbPath::GetExtentname(filePath);
		glbInt32 index = filePath.find_first_of(L'.');
		if(index == 0)
		{
			CGlbWString execDir = CGlbPath::GetExecDir();
			filePath = execDir + filePath.substr(1,filePath.size());
		}
		image = osgDB::readImageFile(filePath.ToString());
		if (image==NULL && filePath.length()>0)
			GlbLogWOutput(GLB_LOGTYPE_ERR,L"读取多边形图像文件(%s)失败. \r\n",filePath.ToString().c_str());
	}

	if(image == NULL)
		return;

	cglbExtent = const_cast<CGlbExtent *>(geom->GetExtent());

	double cx,cy;
	cglbExtent->GetCenter(&cx,&cy);

	// 需要纹理渲染
	isRenderTex = true;
	double rotation = 0.0;
	if(polyInfo->textureRotation)
		rotation = polyInfo->textureRotation->GetValue(feature);	

	agg::scanline_p8 sl;
	agg::int8u alpha(255);//图片透明度
	if(polyInfo->opacity)
		alpha = agg::int8u(polyInfo->opacity->GetValue(feature) * 255 / 100.0);
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

	// 1. 首先实现旋转
	img_mtx.multiply(agg::trans_affine_translation(-image->s()*0.5,-image->t()*0.5));
	img_mtx.multiply(agg::trans_affine_rotation(agg::deg2rad(rotation)));
	img_mtx.multiply(agg::trans_affine_translation(image->s()*0.5,image->t()*0.5));

	// 2. 图像缩放
	double imgscalex = double(tempImageW) / image->s() * (cglbExtent->GetXWidth() / tempExtent.GetXWidth());
	double imgscaley = double(tempImageH) / image->t() * (cglbExtent->GetYHeight() / tempExtent.GetYHeight());	
	img_mtx.multiply(agg::trans_affine_scaling(imgscalex ,imgscaley));

	// 3. 先把图像中心点偏移到(cglbExtent->GetLeft(),cglbExtent->GetBottom())处,以实现中心旋转
	double new_basex = cglbExtent->GetLeft();
	double new_basey = cglbExtent->GetBottom();
	double transx_first = (new_basex-tempExtent.GetLeft()) * tempImageW / tempExtent.GetXWidth();
	double transy_first = (new_basey-tempExtent.GetBottom()) * tempImageH / tempExtent.GetYHeight();
	img_mtx.multiply(agg::trans_affine_translation(transx_first,transy_first));

	// 4. 矩阵取反
	img_mtx.invert();//注意这里

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

	// Rasterizer & scanline 栅格扫描线
	// renderer_scanline_aa （还有一个兄弟版本renderer_scanline_bin）可以按指定的图案或不同的颜色（如渐变）填充顶点源里的多边形。其中的模板参数 SpanAllocator用于准
	// 备span，我们直接使用agg::span_allocator就行。这里的SpanGenerator就是本节要说的线段生成器，它决定了最终用什么东西填到rendering_buffer里
	agg::rasterizer_scanline_aa<> texRas;//处理纹理

	const geos::geom::LineString* extstring = poly->getExteriorRing();
	geos::geom::CoordinateSequence* coors = extstring->getCoordinates();
	size_t numPt = coors->size();
	// 测试代码 2014.12.24 ml
	//texRas.move_to_d(0,0);
	//texRas.line_to_d(255,0);
	//texRas.line_to_d(255,255);
	//texRas.line_to_d(0,255);
	//texRas.line_to_d(0,0);
	
	//agg::ellipse ell(128,128,127,127); //圆心在中间
	// 用我们的渲染器画圆
	//texRas.add_path(ell);
	
	PT c;
	for (size_t i = 0; i < numPt; i++)
	{
		coors->getAt(i,c);
		double new_x = (c.x - tempExtent.GetLeft()) * (tempImageW / (tempExtent.GetRight() - tempExtent.GetLeft()));
		double new_y = (c.y - tempExtent.GetBottom()) * (tempImageH / (tempExtent.GetTop() - tempExtent.GetBottom()));			

		if(i == 0)
		{
			texRas.move_to_d(new_x,new_y);
			continue;
		}
		texRas.line_to_d(new_x,new_y);		
	}

	size_t inRingCnt = poly->getNumInteriorRing();
	for (size_t j = 0; j < inRingCnt; j++)
	{
		const geos::geom::LineString* interstring = poly->getInteriorRingN(j);
		coors = interstring->getCoordinates();
		size_t numPt = coors->size();		
		for (size_t k = 0; k < numPt; k++)
		{
			coors->getAt(k,c);

			double new_x = (c.x - tempExtent.GetLeft()) * (tempImageW / (tempExtent.GetRight() - tempExtent.GetLeft()));
			double new_y = (c.y - tempExtent.GetBottom()) * (tempImageH / (tempExtent.GetTop() - tempExtent.GetBottom()));			

			if(k == 0)
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

void CGlbGlobePolygonSymbol::DealAggRender( renb_type &renb,CGlbGlobeRObject *obj,IGlbGeometry *geom,
											glbInt32 imageW,glbInt32 imageH,CGlbExtent &ext)
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

	typedef agg::conv_contour<ell_ct_type> ell_cc_type;
	ell_cc_type ccell(ctell);

	typedef agg::conv_stroke<ell_ct_type> ell_ct_cs_type;
	ell_ct_cs_type csell(ctell);

	agg::rasterizer_scanline_aa<> ras;//处理纹理
	agg::rasterizer_scanline_aa<> rasOutline;
	agg::rasterizer_scanline_aa<> rasFillpolygon;
	agg::scanline_p8 sl;

	GlbRenderInfo *renderInfo = obj->GetRenderInfo();
	GlbPolygon2DSymbolInfo *polygon2DInfo= dynamic_cast<GlbPolygon2DSymbolInfo *>(renderInfo);

	CGlbFeature *feature = obj->GetFeature();

	glbBool isRenderTex = false;

	glbDouble area = ext.GetXWidth()*ext.GetYHeight();
	CGlbExtent expandExt;// 扩大0.05倍的ext范围
	osg::Vec3d _center;
	ext.GetCenter(&_center.x(),&_center.y(),&_center.z());
	expandExt.SetMin(_center.x()-ext.GetXWidth()*0.55,_center.y()-ext.GetYHeight()*0.55);
	expandExt.SetMax(_center.x()+ext.GetXWidth()*0.55,_center.y()+ext.GetYHeight()*0.55);

	// 图像绘制到renb
	ras.add_path(ccell);
	glbInt32 polyCnt = multPoly->GetCount();
	
	for(glbInt32 i = 0 ; i <polyCnt; i++)
	{
		//osg::Timer_t startTime = osg::Timer::instance()->tick();		
		CGlbPolygon* poly = const_cast<CGlbPolygon *>(multPoly->GetPolygon(i));	
		if (poly==NULL) continue;
		const CGlbExtent* polyExt = poly->GetExtent();
		// 注意此处不能直接用polyExt->Merge(ext)因为这样会改变poly的外包的！！2014.4.17 马林
		CGlbExtent mergeExt;//合并poly+ext
		mergeExt.Merge(*polyExt);
		mergeExt.Merge(ext);
		glbDouble area1 = mergeExt.GetXWidth()*mergeExt.GetYHeight();
		if (area1 / area > OverlayFazhi)
		{// 当要绘制的对象范围的面积是ext范围面积的2倍以上时，需要做overlay计算以加速栅格化	
			geos::geom::Geometry* outGeom = Interection(poly,expandExt);
			if (outGeom)
			{
				geos::geom::GeometryTypeId typeId = outGeom->getGeometryTypeId();
				switch(typeId)
				{
				case GEOS_POLYGON:
					{
						geos::geom::Polygon* ply = dynamic_cast<geos::geom::Polygon*>(outGeom);
						FillDrawVertexArray(ps,ply);
						// 绘制纹理
						DealAggTexture(ras,renb,poly,ply,polygon2DInfo,feature,isRenderTex);							
					}
					break;
				case GEOS_MULTIPOLYGON:
					{
						geos::geom::MultiPolygon* multiPoly = dynamic_cast<geos::geom::MultiPolygon*>(outGeom);
						size_t numPoly = multiPoly->getNumGeometries();
						for (size_t k = 0; k < numPoly; k++)
						{
							geos::geom::Geometry* geo = (geos::geom::Geometry*)(multiPoly->getGeometryN(k));
							geos::geom::Polygon* ply = dynamic_cast<geos::geom::Polygon*>(geo);
							FillDrawVertexArray(ps,ply);							
							// 绘制纹理
							DealAggTexture(ras,renb,poly,ply,polygon2DInfo,feature,isRenderTex);							
						}
					}
					break;
				}

				// 删除outGeom				
				factory.destroyGeometry(outGeom);
			}
		}
		else
		{
			// 矢量绘制到renb
			FillDrawVertexArray(ps,poly);	
			// 绘制纹理
			DealAggTexture(ras,renb,poly,polygon2DInfo,feature,isRenderTex);			
		}			
	}	

	if(polygon2DInfo->outlineInfo && polygon2DInfo->outlineInfo->lineWidth)
		csell.width(polygon2DInfo->outlineInfo->lineWidth->GetValue(feature)/* * (ext.GetRight() - ext.GetLeft())  /  imageW*/);//线宽
	rasFillpolygon.add_path(ccell);
	rasOutline.add_path(csell);
	glbInt32 lineColor = 0xFFFFFFFF,polygonColor = 0xFFFFFFFF;
	agg::rgba8 rgbaLine(255,255,255,255),rgbaPolygon(255,255,255,255);

	if(polygon2DInfo->outlineInfo->lineColor)
		lineColor = polygon2DInfo->outlineInfo->lineColor->GetValue(feature);
	if(polygon2DInfo->outlineInfo && polygon2DInfo->outlineInfo->lineOpacity)
	{
		rgbaLine = agg::rgba8(GetBValue(lineColor),LOBYTE(((lineColor & 0xFFFF)) >> 8),GetRValue(lineColor),
			polygon2DInfo->outlineInfo->lineOpacity->GetValue(feature) * 255 / 100.0);//设置线的颜色【透明度】
	}
	else if(polygon2DInfo->outlineInfo && polygon2DInfo->outlineInfo->lineOpacity == NULL)
	{
		//rgbaLine = agg::rgba8(GetRValue(lineColor),GetGValue(lineColor),GetBValue(lineColor),GetAValue(lineColor));
		rgbaLine = agg::rgba8(GetBValue(lineColor),LOBYTE(((lineColor & 0xFFFF)) >> 8),GetRValue(lineColor),GetAValue(lineColor));
	}

	if(polygon2DInfo->color)
		polygonColor = polygon2DInfo->color->GetValue(feature);

	if (obj &&  obj->IsSelected()==true)
	{// 选中状态
		lineColor = RGB(0,0,255);
		polygonColor = RGB(0,0,255);
	}

	rgbaPolygon = agg::rgba8(GetBValue(polygonColor),LOBYTE(((polygonColor & 0xFFFF)) >> 8),GetRValue(polygonColor),GetAValue(polygonColor));
	if(polygon2DInfo->opacity)
	{
		if(isRenderTex)
			rgbaPolygon = agg::rgba8(GetBValue(polygonColor),LOBYTE(((polygonColor & 0xFFFF)) >> 8),GetRValue(polygonColor),
			polygon2DInfo->opacity->GetValue(feature) * 255 / 100.0 * 0.5);
		else
			rgbaPolygon = agg::rgba8(GetBValue(polygonColor),LOBYTE(((polygonColor & 0xFFFF)) >> 8),GetRValue(polygonColor),
			polygon2DInfo->opacity->GetValue(feature) * 255 / 100.0);
	}

	agg::render_scanlines_aa_solid(rasOutline,sl,renb,rgbaLine);
	if(polygonColor == 0xFFFFFFFF && isRenderTex == true)
		return;
	agg::render_scanlines_aa_solid(rasFillpolygon,sl,renb,rgbaPolygon);

	//if (true)
	//{// 测试
	//	osg::Timer_t currTime = osg::Timer::instance()->tick();
	//	double frameTime = osg::Timer::instance()->delta_m(preTime,currTime);		
	//	swprintf(wBuff,L"DealAggRender [w:%lf e:%lf s:%lf n:%lf] use time: %lf .\n",ext.GetLeft(),ext.GetRight(),ext.GetBottom(),ext.GetTop(),frameTime);
	//	OutputDebugString(wBuff);
	//}	
}
void CGlbGlobePolygonSymbol::DrawToImage(CGlbGlobeRObject *obj, IGlbGeometry* geom, glbByte *image/*RGBA*/,glbInt32 imageW, glbInt32 imageH,CGlbExtent &ext)
{
	if (obj==NULL || geom==NULL)
		return;

	//改由上层地形块tile逻辑中判断是否需要绘制==符号认为只要调用DrawToImage就是要绘制 2014.10.17 马林
	//glbref_ptr<IGlbGeometry> outline = obj->GetOutLine();
	//if(outline == NULL) 
	//	return;
	//GlbRenderInfo *polygonInfo = obj->GetRenderInfo();
	//GlbPolygon2DSymbolInfo *polygon2DInfo= dynamic_cast<GlbPolygon2DSymbolInfo *>(polygonInfo);
	//if (polygon2DInfo==NULL)
	//	return;	
	//mpr_globeType = obj->GetGlobe()->GetType();	
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
		renb_type renb(pixf);

		DealAggRender(renb,obj,geom,imageW,imageH,ext);		
	}
}
/** geo 不能是collect */
/*
	返回CGlbMultiPolygon 或 CGlbMultiPolygon类型
*/
IGlbGeometry * CGlbGlobePolygonSymbol::GetOutline(CGlbGlobeRObject *obj,IGlbGeometry *geo)
{
	////throw std::exception("The method or operation is not implemented.");
	if (geo==NULL)	return NULL;
	glbInt32 dimension = geo->GetCoordDimension();
	// 马林 2014.8.18
	glbInt32 i;	
	GlbGeometryTypeEnum geoType=geo->GetType();
	switch(geoType)
	{
	case GLB_GEO_POLYGON:
		{
			glbref_ptr<CGlbMultiPolygon> _outlines = new CGlbMultiPolygon(2,false);	
			glbref_ptr<CGlbPolygon> _outline = new CGlbPolygon(2,false);

			CGlbPolygon* poly = dynamic_cast<CGlbPolygon*>(geo);
			CGlbLine* extRing = const_cast<CGlbLine *>(poly->GetExtRing());
			glbInt32 ptCnt = extRing->GetCount();
			//glbDouble ptx,pty;
			CGlbLine* ln = new CGlbLine();
			const double*           pcoords    = extRing->GetPoints();
			//glbArray pvalues;
			//double*                 new_pcoords= new double[ptCnt*2];
			//pvalues.pDoubles = new double[ptCnt * 2];
			for (i = 0; i < ptCnt; i++)
			{
				//extRing->GetPoint(i,&ptx,&pty);
				//ln->AddPoint(ptx,pty);
				//pvalues.pDoubles[i*2]   = pcoords[i*2];
				//pvalues.pDoubles[i*2+1] = pcoords[i*2+1];
				ln->AddPoint(pcoords[i * dimension],pcoords[i * dimension + 1]);
			}
			_outline->SetExtRing(ln);
			//delete[] pvalues.pDoubles;

			glbInt32 inRingCnt = poly->GetInRingCount();
			for (i = 0; i < inRingCnt; i++)
			{
				CGlbLine* inRing = const_cast<CGlbLine *>(poly->GetInRing(i));
				ptCnt = inRing->GetCount();
				if (ptCnt<3) continue;

				ln = new CGlbLine;			

				pcoords    = inRing->GetPoints();
				//pvalues.pDoubles= new double[ptCnt*2];
				for (glbInt32 j = 0; j < ptCnt; j++)
				{
					//inRing->GetPoint(j,&ptx,&pty);
					//ln->AddPoint(ptx,pty);
					//pvalues.pDoubles[j*2]   = pcoords[j*2];
					//pvalues.pDoubles[j*2+1] = pcoords[j*2+1];
					ln->AddPoint(pcoords[j * dimension],pcoords[j * dimension + 1]);
				}
				_outline->AddInRing(ln);
				//delete[] pvalues.pDoubles;
			}

			_outlines->AddPolygon(_outline.get());
			mpt_outline = _outlines;
		}
		break;
	case GLB_GEO_MULTIPOLYGON:
		{
			glbref_ptr<CGlbMultiPolygon> _outlines = new CGlbMultiPolygon();	

			CGlbMultiPolygon* multipoly = dynamic_cast<CGlbMultiPolygon*>(geo);
			glbInt32 polyCnt = multipoly->GetCount();
			for(i =0; i < polyCnt; i++)
			{				
				CGlbPolygon* _outline = new CGlbPolygon();				

				CGlbPolygon* poly = const_cast<CGlbPolygon *>(multipoly->GetPolygon(i));
				CGlbLine* extRing = const_cast<CGlbLine *>(poly->GetExtRing());
				glbInt32 ptCnt = extRing->GetCount();
				//glbDouble ptx,pty;

				CGlbLine* ln = new CGlbLine;		
				const double*           pcoords    = extRing->GetPoints();
				//double*                 new_pcoords= new double[ptCnt*2];
				//glbArray pvalues;
				//pvalues.pDoubles = new double[ptCnt*2];
				for (glbInt32 k = 0; k < ptCnt; k++)
				{
					//extRing->GetPoint(k,&ptx,&pty);
					//ln->AddPoint(ptx,pty);
					//pvalues.pDoubles[k*2]   = pcoords[k*2];
					//pvalues.pDoubles[k*2+1] = pcoords[k*2+1];
					ln->AddPoint(pcoords[k * dimension],pcoords[k * dimension + 1]);
				}
				_outline->SetExtRing(ln);
				//delete[] pvalues.pDoubles;

				glbInt32 inRingCnt = poly->GetInRingCount();
				for (glbInt32 k = 0; k < inRingCnt; k++)
				{
					CGlbLine* inRing = const_cast<CGlbLine *>(poly->GetInRing(k));
					ptCnt = inRing->GetCount();
					if (ptCnt<3) continue;

					ln = new CGlbLine;				
					pcoords    = inRing->GetPoints();
					//pvalues.pDoubles = new double[ptCnt*2];
					for (glbInt32 j = 0; j < ptCnt; j++)
					{
						//inRing->GetPoint(j,&ptx,&pty);
						//ln->AddPoint(ptx,pty);
						//pvalues.pDoubles[j*2]   = pcoords[j*2];
						//pvalues.pDoubles[j*2+1] = pcoords[j*2+1];
						ln->AddPoint(pcoords[ j * dimension],pcoords[j * dimension + 1]);
					}
					_outline->AddInRing(ln);
					//delete[] pvalues.pDoubles;
				}				
				_outlines->AddPolygon(_outline);
			}
			mpt_outline = _outlines;
		}
		break;
	}

	return mpt_outline.release();
}
//////////////////////////////////////////////////////////////////////////
void createPolygon2DSymbolInfo(osg::StateSet *&sset,CGlbGlobeRObject *obj,GlbPolygon2DSymbolInfo *polygon2DInfo,glbDouble texCetX,glbDouble texCetY,glbDouble sizeX,glbDouble sizeY)
{
	sset->setMode( GL_CULL_FACE, osg::StateAttribute::OFF);
	//sset->setMode(GL_DEPTH_TEST,osg::StateAttribute::OFF);
	sset->setMode(GL_BLEND,osg::StateAttribute::ON);
	osg::ref_ptr<osg::BlendFunc> pBlendFun = new osg::BlendFunc(osg::BlendFunc::SRC_ALPHA,osg::BlendFunc::ONE_MINUS_SRC_ALPHA );	//设置混合方程
	//osg::ref_ptr<osg::BlendFunc> pBlendFun = new osg::BlendFunc(osg::BlendFunc::ONE,osg::BlendFunc::ZERO);	//设置混合方程
	sset->setAttributeAndModes(pBlendFun.get(),osg::StateAttribute::ON/*|osg::StateAttribute::OVERRIDE*/);
	//sset->setRenderBinDetails( obj->GetRenderOrder(), "RenderBin"); 
	osg::ref_ptr<osg::PolygonMode> polygonMode = new osg::PolygonMode();
	polygonMode->setMode( osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::FILL);
	sset->setAttribute( polygonMode.get(), osg::StateAttribute::ON );

	sset->addUniform( new osg::Uniform("PolygonDownLeft", osg::Vec2f(texCetX,texCetY) ) );
	sset->addUniform( new osg::Uniform("PolygonRotateCenter", osg::Vec2f(texCetX,texCetY) ) );
	sset->addUniform( new osg::Uniform("PolygonWeightAndHeight", osg::Vec2f(sizeX,sizeY) ) );

	/*sset->addUniform( new osg::Uniform("PolygonSelected", false) );
	sset->addUniform( new osg::Uniform("PolygonBlink", false) );*/
	// 1、透明
	glbInt32 polygonOpacity=100;
	osg::ref_ptr<osg::Depth> depth = NULL;
	if (polygon2DInfo->opacity)
	{
		polygonOpacity=polygon2DInfo->opacity->value;
	}
	if (polygonOpacity!=100)//透明
	{
		sset->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
		depth = dynamic_cast<osg::Depth *>(sset->getAttribute(osg::StateAttribute::DEPTH));
		if(!depth.valid())
		{
			depth = new osg::Depth;
			depth->setWriteMask(false);
			sset->setAttributeAndModes(depth.get(), osg::StateAttribute::ON);
		}
		else
		{
			if(depth->getWriteMask())
				depth->setWriteMask(false);
		}
	}
	else
	{
		sset->setRenderingHint(osg::StateSet::OPAQUE_BIN);
		sset->setMode(GL_BLEND,osg::StateAttribute::OFF);
		depth = dynamic_cast<osg::Depth *>(sset->getAttribute(osg::StateAttribute::DEPTH));
		if(!depth.valid())
		{
			depth = new osg::Depth;
			depth->setWriteMask(true);
			sset->setAttributeAndModes(depth.get(), osg::StateAttribute::ON);
		}
		else
		{
			if(!depth->getWriteMask())
				depth->setWriteMask(true);
		}
	}
	sset->addUniform( new osg::Uniform("PolygonOpacity", polygonOpacity ) );

	//2、纹理贴图/或者颜色
	osg::Vec4f polygonColor(0.0,255.0,0.0,1.0);
	if (polygon2DInfo->color)
	{
		glbInt32 plyClr=polygon2DInfo->color->GetValue(obj->GetFeature());
		polygonColor.set(GetRValueInBGRA(plyClr),LOBYTE(((plyClr & 0xFFFF)) >> 8),GetBValueInBGRA(plyClr),1.0);
	}
	sset->addUniform( new osg::Uniform("PolygonColor", polygonColor) );
	bool buseTexColor=false;
	osg::Image *pImg=NULL;
	if (polygon2DInfo->textureData)
	{
		CGlbWString imageMapFilePath=polygon2DInfo->textureData->value;
		glbInt32 index = imageMapFilePath.find_first_of(L'.');
		if(index == 0)
		{
			CGlbWString execDir = CGlbPath::GetExecDir();
			imageMapFilePath = execDir + imageMapFilePath.substr(1,imageMapFilePath.size());
		}
		pImg= loadImage(imageMapFilePath);
		if (pImg!=NULL)
			buseTexColor=true;
	}
	osg::Texture2D* imageMap_tex = loadTexture(pImg, osg::Texture::REPEAT);
	sset->setTextureAttributeAndModes(0, imageMap_tex, osg::StateAttribute::ON );
	sset->addUniform( new osg::Uniform("PolygonTextureMap", 0) );
	sset->addUniform( new osg::Uniform("PolygonUseTex", buseTexColor) );

	// 纹理模式
	float tilingU=1.0;
	if (polygon2DInfo->tilingU)
	{
		glbInt32 u=polygon2DInfo->tilingU->value;
		tilingU=u;
	}
	float tilingV=1.0;
	if (polygon2DInfo->tilingV)
	{
		glbInt32 v=polygon2DInfo->tilingV->value;
		tilingV=v;
	}
	int texRepeatMode=0;
	if (polygon2DInfo->texRepeatMode)
	{
		if (polygon2DInfo->texRepeatMode->value==GLB_TEXTUREREPEAT_TIMES)
			texRepeatMode=0;
		else if(polygon2DInfo->texRepeatMode->value==GLB_TEXTUREREPEAT_SIZE)
			texRepeatMode=1;
	}
	sset->addUniform( new osg::Uniform("tilingU", tilingU) );
	sset->addUniform( new osg::Uniform("tilingV", tilingV) );
	sset->addUniform( new osg::Uniform("PolygonTexRepeatMode", texRepeatMode) );
		
	HMODULE hmd = GetModuleHandle(L"GlbGlobe.dll");
	CGlbWString workdir = CGlbPath::GetModuleDir(hmd);
	CGlbWString polygonVertexShader =  workdir +L"\\res\\polygon.vert";
	CGlbWString polygonFragmentShader = workdir +L"\\res\\polygon.frag";
	osg::ref_ptr<osg::Program> program = createShaderProgram("polygonShader", polygonVertexShader.ToString(), polygonFragmentShader.ToString(), true);
	if(program.valid())
		sset->setAttributeAndModes(program.get(), osg::StateAttribute::ON  | osg::StateAttribute::PROTECTED);	
	//osg::TexGen *texGen=new osg::TexGen();
	//texGen->setMode((osg::TexGen::Mode)(/*osg::TexGen::OBJECT_LINEAR|*/osg::TexGen::OBJECT_LINEAR));
	//sset->setTextureAttributeAndModes(0,texGen,osg::StateAttribute::ON|osg::StateAttribute::OVERRIDE);
	//osg::TexMat *tm=new osg::TexMat();
	/*osg::Matrixd txtm =osg::Matrixd::rotate(osg::DegreesToRadians(45.0),osg::Vec3f(0.0,0.0,1.0));
	tm->setMatrix(txtm);*/
	//sset->setTextureAttribute(0,tm);
}
void createPolygon2DOutlineSymbolInfo(osg::StateSet *&sset,CGlbGlobeRObject *obj,GlbPolygon2DSymbolInfo *polygon2DInfo)
{
	sset->setMode( GL_CULL_FACE, osg::StateAttribute::ON);
	sset->setMode(GL_DEPTH_TEST,osg::StateAttribute::ON);
	//sset->setAttributeAndModes( new osg::Depth( osg::Depth::LESS, 0, 1, false ) ); // no depth write
	//sset->setAttributeAndModes( new osg::Depth(osg::Depth::ALWAYS, 0, 1, false), osg::StateAttribute::ON );
	sset->setMode(GL_BLEND,osg::StateAttribute::ON);
	osg::ref_ptr<osg::BlendFunc> pBlendFun = new osg::BlendFunc(osg::BlendFunc::SRC_ALPHA,osg::BlendFunc::ONE_MINUS_SRC_ALPHA );	//设置混合方程
	//osg::ref_ptr<osg::BlendFunc> pBlendFun = new osg::BlendFunc(osg::BlendFunc::ONE,osg::BlendFunc::ZERO);	//设置混合方程 
	sset->setAttributeAndModes(pBlendFun.get(),osg::StateAttribute::ON/*|osg::StateAttribute::OVERRIDE*/);
	sset->setRenderBinDetails( obj->GetRenderOrder()+1, "RenderBin"); 


	// 1、透明度
	glbInt32 polygonOutlineOpacity=100;
	if (polygon2DInfo->outlineInfo!=NULL)
	{
		if (polygon2DInfo->outlineInfo->lineOpacity!=NULL)
		{
			polygonOutlineOpacity=polygon2DInfo->outlineInfo->lineOpacity->value;
		}
	}
	if (polygonOutlineOpacity!=100)//透明
		sset->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
	else
		sset->setRenderingHint(osg::StateSet::OPAQUE_BIN);
	sset->addUniform( new osg::Uniform("PolygonOutlineOpacity", polygonOutlineOpacity ) );

	//2、颜色
	osg::Vec4f polygonOutlineColor(0.0,255.0,0.0,255.0);
	if (polygon2DInfo->outlineInfo!=NULL)
	{
		if (polygon2DInfo->outlineInfo->lineColor!=NULL)
		{
			glbInt32 plyClr=polygon2DInfo->outlineInfo->lineColor->GetValue(obj->GetFeature());
			polygonOutlineColor.set(GetRValueInBGRA(plyClr),LOBYTE(((plyClr & 0xFFFF)) >> 8),GetBValueInBGRA(plyClr),255.0);
		}
	}
	sset->addUniform( new osg::Uniform("PolygonOutlineColor", polygonOutlineColor) );
	// 3、线宽
	glbDouble plyOutlineW=1.0;
	if (polygon2DInfo->outlineInfo!=NULL)
	{
		if (polygon2DInfo->outlineInfo->lineWidth!=NULL)
			plyOutlineW=polygon2DInfo->outlineInfo->lineWidth->value;
	}
	if (plyOutlineW<=0.0)
		plyOutlineW=1.0;
	osg::LineWidth *lineWidth = new osg::LineWidth((float)plyOutlineW);
	sset->setAttributeAndModes(lineWidth,osg::StateAttribute::ON);
	// 4、线样式
	bool solidLine=true;
	if (polygon2DInfo->outlineInfo!=NULL)
	{
		if (polygon2DInfo->outlineInfo->linePattern!=NULL)
			solidLine=polygon2DInfo->outlineInfo->linePattern->value==GLB_LINE_SOLID;
	}
	osg::LineStipple *lineStipple = new osg::LineStipple();
	lineStipple->setFactor(1);
	lineStipple->setPattern(0x1C47);
	if (solidLine)
		sset->setAttributeAndModes(lineStipple,osg::StateAttribute::OFF);
	else
		sset->setAttributeAndModes(lineStipple,osg::StateAttribute::ON);
	
	/*bool buseTexColor=false;
	osg::Image *pImg=NULL;
	if (polygon2DInfo->textureData)
	{
		CGlbWString imageMapFilePath=polygon2DInfo->textureData->value;
		pImg= loadImage(imageMapFilePath);
		if (pImg!=NULL)
			buseTexColor=true;
	}
	osg::Texture2D* imageMap_tex = loadTexture(pImg, osg::Texture::REPEAT);
	sset->setTextureAttributeAndModes(0, imageMap_tex, osg::StateAttribute::ON );
	sset->addUniform( new osg::Uniform("PolygonTextureMap", 0) );
	sset->addUniform( new osg::Uniform("PolygonUseTex", buseTexColor) );*/

	// 纹理模式
	/*float tilingU=1.0;
	if (polygon2DInfo->tilingU)
	{
	glbInt32 u=polygon2DInfo->tilingU->value;
	tilingU=u;
	}
	float tilingV=1.0;
	if (polygon2DInfo->tilingV)
	{
	glbInt32 v=polygon2DInfo->tilingV->value;
	tilingV=v;
	}
	int texRepeatMode=0;
	if (polygon2DInfo->texRepeatMode)
	{
	if (polygon2DInfo->texRepeatMode->value==GLB_TEXTUREREPEAT_TIMES)
	texRepeatMode=0;
	else if(polygon2DInfo->texRepeatMode->value==GLB_TEXTUREREPEAT_SIZE)
	texRepeatMode=1;
	}
	sset->addUniform( new osg::Uniform("tilingU", tilingU) );
	sset->addUniform( new osg::Uniform("tilingV", tilingV) );
	sset->addUniform( new osg::Uniform("PolygonTexRepeatMode", texRepeatMode) );*/
		
	HMODULE hmd = GetModuleHandle(L"GlbGlobe.dll");
	CGlbWString workdir = CGlbPath::GetModuleDir(hmd);
	CGlbWString polygonOutlineVertexShader =  workdir +L"\\res\\polygonOutline.vert";
	CGlbWString polygonOutlineFragmentShader = workdir +L"\\res\\polygonOutline.frag";
	osg::ref_ptr<osg::Program> program = createShaderProgram("polygonOutlineShader", polygonOutlineVertexShader.ToString(), polygonOutlineFragmentShader.ToString(), true);
	if(program.valid())
		sset->setAttributeAndModes(program.get(), osg::StateAttribute::ON | osg::StateAttribute::PROTECTED);	
	//osg::TexGen *texGen=new osg::TexGen();
	//texGen->setMode((osg::TexGen::Mode)(/*osg::TexGen::OBJECT_LINEAR|*/osg::TexGen::OBJECT_LINEAR));
	//sset->setTextureAttributeAndModes(0,texGen,osg::StateAttribute::ON|osg::StateAttribute::OVERRIDE);
	//osg::TexMat *tm=new osg::TexMat();
	/*osg::Matrixd txtm =osg::Matrixd::rotate(osg::DegreesToRadians(45.0),osg::Vec3f(0.0,0.0,1.0));
	tm->setMatrix(txtm);*/
	//sset->setTextureAttribute(0,tm);
}

void createPolygon2DEdgeWallSymbolInfo(osg::StateSet *&sset,CGlbGlobeRObject *obj,GlbPolygon2DSymbolInfo *polygon2DInfo)
{
	sset->setMode( GL_CULL_FACE, osg::StateAttribute::OFF);
	// 1、透明
	glbInt32 polygonOpacity=100;
	osg::ref_ptr<osg::Depth> depth = NULL;
	if (polygon2DInfo->opacity)
	{
		polygonOpacity=polygon2DInfo->opacity->value;
	}	
	//2、颜色
	osg::Vec4f polygonColor(0.0,1.0,0.0,1.0);
	if (polygon2DInfo->color)
	{
		glbInt32 plyClr=polygon2DInfo->color->GetValue(obj->GetFeature());
		polygonColor = GetColor(plyClr);	
	}
	//3、使用材质颜色
	osg::ref_ptr<osg::LightModel> lm = new osg::LightModel;
	lm->setTwoSided(true); 
	sset->setAttribute(lm.get(),osg::StateAttribute::ON);
	osg::ref_ptr<osg::Material> material = new osg::Material;
	osg::Vec4 color = polygonColor;
	glbInt32 opacity = polygonOpacity;
	material->setDiffuse(osg::Material::FRONT_AND_BACK,color);
	material->setAlpha(osg::Material::FRONT_AND_BACK,opacity / 100.0);
	sset->setAttribute(material,osg::StateAttribute::ON);
	if(opacity < 100)
	{
		sset->setMode(GL_BLEND,osg::StateAttribute::ON);
		if(sset->getRenderingHint() != osg::StateSet::TRANSPARENT_BIN)
			sset->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);			
	}
	else
	{
		sset->setMode(GL_BLEND,osg::StateAttribute::OFF);
		if(sset->getRenderingHint() != osg::StateSet::OPAQUE_BIN)
			sset->setRenderingHint(osg::StateSet::OPAQUE_BIN);
	}
	sset->setMode(GL_LIGHTING,osg::StateAttribute::ON);
}

osg::Node * CGlbGlobePolygonSymbol::buildDrawable( CGlbGlobeRObject *obj,CGlbPolygon *geo )
{
	GlbRenderInfo *polygonInfo = obj->GetRenderInfo();
	GlbPolygon2DSymbolInfo *polygon2DInfo = dynamic_cast<GlbPolygon2DSymbolInfo *>(polygonInfo);

	CGlbPolygon *pPolyGon = geo;
	if (NULL == pPolyGon || pPolyGon->IsEmpty())
		return NULL;
	glbInt32 dimension = geo->GetCoordDimension();
	CGlbLine *pExtRing = const_cast<CGlbLine *>(pPolyGon->GetExtRing());//多边形外环
	if (NULL==pExtRing||pExtRing->IsEmpty())
		return NULL;
	glbInt32 pntCnt=pExtRing->GetCount();
	if (pntCnt<3)
		return NULL;
	GlbGlobeTypeEnum globeTypeE=obj->GetGlobe()->GetType();
	glbBool isGlobe=GLB_GLOBETYPE_GLOBE==globeTypeE;
	glbDouble newX,newY,newZ;
	osg::EllipsoidModel myEllipsoidModel(g_ellipsoidModel->getRadiusEquator(),g_ellipsoidModel->getRadiusPolar());

	double height = 0.0;
	osg::Matrixd offsetMt = osg::Matrix::identity();
	if (polygon2DInfo->zOrAltitude != NULL)
	{
		double offsetZ= polygon2DInfo->zOrAltitude->GetValue(obj->GetFeature());
		offsetMt.setTrans(osg::Vec3d(0,0,offsetZ));
	}
	// 0提取多边形原始顶点
	osg::ref_ptr<osg::Vec3dArray> points=new osg::Vec3dArray();
	osg::ref_ptr<osg::Vec3Array> points_outline=new osg::Vec3Array();
	const glbDouble* pCoords=pExtRing->GetPoints();
	for (glbInt32 i=0;i<pntCnt;++i)
	{
		if(dimension == 3)
			height = pCoords[i*dimension+2];
		osg::Vec3d pnt(pCoords[i*dimension],pCoords[i*dimension+1],height);
		points->push_back(pnt);
		points_outline->push_back(pnt);
	}
	glbUInt32 uniqueCnt=DelaunayTriangulator_uniqueifyPoints(points);// 判断多边形是否有效
	if (uniqueCnt<3)
		return NULL;
	
	pntCnt = points->size();
	// 首尾点强制闭合
	double tt;
	if (isGlobe)
	{
		myEllipsoidModel.convertLatLongHeightToXYZ(osg::DegreesToRadians(points->at(0).y()),osg::DegreesToRadians(points->at(0).x()),points->at(0).z(),
			newX,newY,newZ);
		osg::Vec3d pos1(newX,newY,newZ);
		myEllipsoidModel.convertLatLongHeightToXYZ(osg::DegreesToRadians(points->at(points->size()-1).y()),osg::DegreesToRadians(points->at(points->size()-1).x()),points->at(points->size()-1).z(),
			newX,newY,newZ);
		osg::Vec3d pos2(newX,newY,newZ);
		tt = (pos1-pos2).length();
	}
	else
		tt = (points->at(0)-points->at(points->size()-1)).length();
	if (tt > 0.001)
	{
		points->push_back(points->at(0));
		points_outline->push_back(points_outline->at(0));
		pntCnt++;
	}

	osg::ref_ptr<osg::Geometry> geom=new osg::Geometry();
	osg::ref_ptr<osg::Geometry> geom_outline=new osg::Geometry();
	// 边缘向下延伸
	osg::ref_ptr<osg::Geometry> geom_edgeWall=NULL;
	glbBool isDrawEdgeWall = false;
	glbBool isWallDirReserve = false;
	glbDouble wallHeight = 0.0;
	if (polygon2DInfo->isEdgeVerticalExtend != NULL)
		isDrawEdgeWall = polygon2DInfo->isEdgeVerticalExtend->GetValue(obj->GetFeature());
	if (polygon2DInfo->isExtendDirReserve != NULL)
		isWallDirReserve = polygon2DInfo->isExtendDirReserve->GetValue(obj->GetFeature());
	if (polygon2DInfo->extendDepth != NULL)
		wallHeight = polygon2DInfo->extendDepth->GetValue(obj->GetFeature());
	//if (isDrawEdgeWall)
		geom_edgeWall=new osg::Geometry();	
	
	geom->setVertexArray(points);
	geom_outline->setVertexArray(points_outline);
	GLint startindex=0;
	geom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::POLYGON,startindex,pntCnt));// 外环
	geom_outline->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINE_LOOP,startindex,pntCnt));// 外环
	//if (isDrawEdgeWall)
		geom_edgeWall->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUAD_STRIP,startindex*2,pntCnt*2));//外环下延

	startindex+=pntCnt;
	glbInt32 inRingCnt=pPolyGon->GetInRingCount();
	for (glbInt32 i=0;i<inRingCnt;++i)// 遍历内环
	{
		CGlbLine *pInRing = const_cast<CGlbLine *>(pPolyGon->GetInRing(i));
		pCoords=pInRing->GetPoints();
		glbInt32 InPntCnt=pInRing->GetCount();
		for (glbInt32 iPnt=0;iPnt<InPntCnt;++iPnt)
		{
			if(dimension == 3)
				height = pCoords[iPnt*dimension+2];
			osg::Vec3d pnt(pCoords[iPnt*dimension],pCoords[iPnt*dimension+1],height);
			points->push_back(pnt);
		}
		//内环首尾强制闭合
		if (isGlobe)
		{
			myEllipsoidModel.convertLatLongHeightToXYZ(osg::DegreesToRadians(points->at(startindex).y()),osg::DegreesToRadians(points->at(startindex).x()),points->at(startindex).z(),
				newX,newY,newZ);
			osg::Vec3d pos1(newX,newY,newZ);
			myEllipsoidModel.convertLatLongHeightToXYZ(osg::DegreesToRadians(points->at(points->size()-1).y()),osg::DegreesToRadians(points->at(points->size()-1).x()),points->at(points->size()-1).z(),
				newX,newY,newZ);
			osg::Vec3d pos2(newX,newY,newZ);
			tt = (pos1-pos2).length();
		}
		else
			tt = (points->at(startindex)-points->at(points->size()-1)).length();
		if (tt > 0.001)
		{
			points->push_back(points->at(startindex));
			InPntCnt++;
		}

		geom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::POLYGON,startindex,InPntCnt));// 内环
		geom_outline->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINE_LOOP,startindex,InPntCnt));// 内环
		//if (isDrawEdgeWall)
			geom_edgeWall->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUAD_STRIP,startindex*2,InPntCnt*2));//内环下延
		startindex+=InPntCnt;
	}

	// 1计算多边形中心点位置和高程

	CGlbExtent *pExt = const_cast<CGlbExtent*>(pPolyGon->GetExtent());
	glbDouble cetX,cetY,cetZ;
	pExt->GetCenter(&cetX,&cetY,&cetZ);
	double elevation = 0.0;
	GlbAltitudeModeEnum altitudeMode=obj->GetAltitudeMode();
	//if (altitudeMode == GLB_ALTITUDEMODE_RELATIVETERRAIN)
	//{
	//	elevation=obj->GetGlobe()->GetElevationAt(cetX,cetY);
	//}

	// 2计算多边形朝向
	osg::Matrixd polygonMT;	
	if (isGlobe)
	{
		myEllipsoidModel.computeLocalToWorldTransformFromLatLongHeight(
			osg::DegreesToRadians(cetY),osg::DegreesToRadians(cetX),cetZ,polygonMT);
	}
	else
		polygonMT.makeTranslate(cetX,cetY,cetZ);
	osg::Matrixd polygonMT_inv=osg::Matrixd::inverse(polygonMT);// 计算多边形偏移矩阵

	// 3多边形分格化	
	osg::ref_ptr<GlbGlobe::TessellatorEx> tscx=new GlbGlobe::TessellatorEx();
	tscx->setTessellationType(GlbGlobe::TessellatorEx::TESS_TYPE_GEOMETRY);
	tscx->setBoundaryOnly(false);
	tscx->setWindingType(GlbGlobe::TessellatorEx::TESS_WINDING_ODD);
	osg::Vec3d pointpos=osg::Vec3d(0.0,0.0,1.0);
	tscx->setTessellationNormal(pointpos);
	tscx->retessellatePolygons(*geom);
	osg::Vec3dArray *newPoints=(osg::Vec3dArray *)geom->getVertexArray();
	osg::Vec3dArray::size_type newPntCnt= newPoints->size();
	if (newPntCnt<3)
		return NULL;

	// 4计算多边形贴图纹理坐标	
	double texSizeU,texSizeV,texCetX,texCetY;//纹理大小和中心
	float texRotAngles=0.0f;
	if (polygon2DInfo->textureRotation)
		texRotAngles=polygon2DInfo->textureRotation->value;
	osg::Vec2dArray* texcoords = buildTexCoordsEx(newPoints,osg::DegreesToRadians(texRotAngles),texSizeU,texSizeV,texCetX,texCetY);//计算多边形纹理坐标
	geom->setTexCoordArray(0, texcoords);

	// 5计算多边形顶点	
	osg::ref_ptr<osg::Vec3Array> vertexPoints=new osg::Vec3Array(newPoints->size());
	osg::ref_ptr<osg::Vec3Array> edgeWallPoints = NULL;
	osg::ref_ptr<osg::Vec3Array> edgeWallNormals = NULL;
	//if (isDrawEdgeWall)// 边缘延伸
		edgeWallPoints = new osg::Vec3Array(newPoints->size()*2);
		edgeWallNormals = new osg::Vec3Array(newPoints->size()*2);
	if (isGlobe)
	{
		if (altitudeMode == GLB_ALTITUDEMODE_ABSOLUTE)
		{
			for (osg::Vec3Array::size_type i=0;i<newPntCnt;++i)
			{
				const osg::Vec3d &iPnt=newPoints->at(i);
				myEllipsoidModel.convertLatLongHeightToXYZ(osg::DegreesToRadians(iPnt.y()),osg::DegreesToRadians(iPnt.x()),iPnt.z(),
					newX,newY,newZ);
				osg::Vec3d pos(newX,newY,newZ);
				vertexPoints->at(i)=pos*polygonMT_inv;

				//if (isDrawEdgeWall)
				{// 边缘延伸
					edgeWallPoints->at(2*i)=pos*polygonMT_inv;
					double cc = isWallDirReserve ? 1.0 : -1.0;
					myEllipsoidModel.convertLatLongHeightToXYZ(osg::DegreesToRadians(iPnt.y()),osg::DegreesToRadians(iPnt.x()),iPnt.z()+cc*wallHeight,
						newX,newY,newZ);
					edgeWallPoints->at(2*i+1)=osg::Vec3d(newX,newY,newZ)*polygonMT_inv;
					if (i <newPntCnt-1)
					{// 非最后一个点用当前点和下一个点向下延伸形成面
						osg::Vec3 v0 = edgeWallPoints->at(2*i);
						osg::Vec3 v1 = edgeWallPoints->at(2*i+1);
						osg::Vec3d cc=newPoints->at(i+1);
						myEllipsoidModel.convertLatLongHeightToXYZ(osg::DegreesToRadians(cc.y()),osg::DegreesToRadians(cc.x()),cc.z(),
							newX,newY,newZ);
						osg::Vec3 v2 = osg::Vec3(newX,newY,newZ);
						osg::Vec3 n = ComputeNormal(v0,v1,v2);
						edgeWallNormals->at(2*i) = n;
						edgeWallNormals->at(2*i+1) = n;
					}
					else
					{
						edgeWallNormals->at(2*i) = edgeWallNormals->at(2*i-1);
						edgeWallNormals->at(2*i+1) = edgeWallNormals->at(2*i-1);
					}
				}
			}
		}
		else if (altitudeMode == GLB_ALTITUDEMODE_RELATIVETERRAIN)
		{
			for (osg::Vec3Array::size_type i=0;i<newPntCnt;++i)
			{
				const osg::Vec3d &iPnt=newPoints->at(i);
				myEllipsoidModel.convertLatLongHeightToXYZ(osg::DegreesToRadians(iPnt.y()),osg::DegreesToRadians(iPnt.x()),iPnt.z()+elevation,
					newX,newY,newZ);
				osg::Vec3d pos(newX,newY,newZ);
				vertexPoints->at(i)=pos*polygonMT_inv;

				//if (isDrawEdgeWall)
				{// 边缘延伸
					edgeWallPoints->at(2*i)=pos*polygonMT_inv;
					double cc = isWallDirReserve ? 1.0 : -1.0;
					myEllipsoidModel.convertLatLongHeightToXYZ(osg::DegreesToRadians(iPnt.y()),osg::DegreesToRadians(iPnt.x()),iPnt.z()+elevation+cc*wallHeight,
						newX,newY,newZ);
					edgeWallPoints->at(2*i+1)=osg::Vec3d(newX,newY,newZ)*polygonMT_inv;
					if (i <newPntCnt-1)
					{// 非最后一个点用当前点和下一个点向下延伸形成面
						osg::Vec3 v0 = edgeWallPoints->at(2*i);
						osg::Vec3 v1 = edgeWallPoints->at(2*i+1);
						osg::Vec3d cc=newPoints->at(i+1);
						myEllipsoidModel.convertLatLongHeightToXYZ(osg::DegreesToRadians(cc.y()),osg::DegreesToRadians(cc.x()),cc.z(),
							newX,newY,newZ);
						osg::Vec3 v2 = osg::Vec3(newX,newY,newZ);
						osg::Vec3 n = ComputeNormal(v0,v1,v2);
						edgeWallNormals->at(2*i) = n;
						edgeWallNormals->at(2*i+1) = n;
					}
					else
					{
						edgeWallNormals->at(2*i) = edgeWallNormals->at(2*i-1);
						edgeWallNormals->at(2*i+1) = edgeWallNormals->at(2*i-1);
					}
				}
			}
		}
		//else if (mpt_altitudeMode == GLB_ALTITUDEMODE_ONTERRAIN)
		//{
			//for (osg::Vec3Array::size_type i=0;i<newPntCnt;++i)
			//{
			//	const osg::Vec3d &iPnt=newPoints->at(i);
			//	myEllipsoidModel.convertLatLongHeightToXYZ(osg::DegreesToRadians(iPnt.y()),osg::DegreesToRadians(iPnt.x()),elevation,
			//		newX,newY,newZ);
			//	osg::Vec3d pos(newX,newY,newZ);
			//	pos-=transOffset;
			//	vertexPoints->at(i)=pos*polygonRotOffset_inv;
			//}
		//}
	}
	else
	{
		if (altitudeMode == GLB_ALTITUDEMODE_ABSOLUTE)
		{
			for (osg::Vec3Array::size_type i=0;i<newPntCnt;++i)
			{
				const osg::Vec3d &iPnt = newPoints->at(i);
				vertexPoints->at(i) = iPnt * polygonMT_inv;
				//if (isDrawEdgeWall)
				{// 边缘延伸
					edgeWallPoints->at(2*i)=iPnt*polygonMT_inv;
					double cc = isWallDirReserve ? 1.0 : -1.0;					
					edgeWallPoints->at(2*i+1)=osg::Vec3d(iPnt.x(),iPnt.y(),iPnt.z()+cc*wallHeight)*polygonMT_inv;

					if (i <newPntCnt-1)
					{// 非最后一个点用当前点和下一个点向下延伸形成面
						osg::Vec3 v0 = edgeWallPoints->at(2*i);
						osg::Vec3 v1 = edgeWallPoints->at(2*i+1);					
						osg::Vec3 v2 = newPoints->at(i+1)*polygonMT_inv;
						osg::Vec3 n = ComputeNormal(v0,v1,v2);
						edgeWallNormals->at(2*i) = n;
						edgeWallNormals->at(2*i+1) = n;
					}
					else
					{
						edgeWallNormals->at(2*i) = edgeWallNormals->at(2*i-1);
						edgeWallNormals->at(2*i+1) = edgeWallNormals->at(2*i-1);
					}
				}
			}
		}
		else if (altitudeMode == GLB_ALTITUDEMODE_RELATIVETERRAIN)
		{
			elevation=obj->GetGlobe()->GetElevationAt(cetX,cetY);
			for (osg::Vec3Array::size_type i=0;i<newPntCnt;++i)
			{
				const osg::Vec3d &iPnt = newPoints->at(i);
				osg::Vec3d pos = iPnt;
				pos.z() += elevation;
				vertexPoints->at(i) = pos * polygonMT_inv;

				//if (isDrawEdgeWall)
				{// 边缘延伸
					edgeWallPoints->at(2*i)=pos*polygonMT_inv;
					double cc = isWallDirReserve ? 1.0 : -1.0;					
					edgeWallPoints->at(2*i+1)=osg::Vec3d(pos.x(),pos.y(),pos.z()+cc*wallHeight)*polygonMT_inv;
					if (i <newPntCnt-1)
					{// 非最后一个点用当前点和下一个点向下延伸形成面
						osg::Vec3 v0 = edgeWallPoints->at(2*i);
						osg::Vec3 v1 = edgeWallPoints->at(2*i+1);					
						osg::Vec3 v2 = newPoints->at(i+1)*polygonMT_inv;
						osg::Vec3 n = ComputeNormal(v0,v1,v2);
						edgeWallNormals->at(2*i) = n;
						edgeWallNormals->at(2*i+1) = n;
					}
					else
					{
						edgeWallNormals->at(2*i) = edgeWallNormals->at(2*i-1);
						edgeWallNormals->at(2*i+1) = edgeWallNormals->at(2*i-1);
					}
				}
			}
		}
		//else if (mAltitudeMode == GLB_ALTITUDEMODE_ONTERRAIN)
		//{
		//	for (osg::Vec3Array::size_type i=0;i<newPntCnt;++i)
		//	{
		//		const osg::Vec3d &iPnt=newPoints->at(i);
		//		osg::Vec3d pos=iPnt;pos.z()+=elevation;pos-=transOffset;
		//		vertexPoints->at(i)=pos*polygonRotOffset_inv;
		//	}
		//}
	}	
	geom->setVertexArray(vertexPoints);
	geom_outline->setVertexArray(vertexPoints);

	// 6设置材质 
	osg::StateSet *sset=geom->getOrCreateStateSet();
	createPolygon2DSymbolInfo(sset,obj,polygon2DInfo,texCetX,texCetY,texSizeU,texSizeV);
	osg::StateSet *sset_outline=geom_outline->getOrCreateStateSet();
	createPolygon2DOutlineSymbolInfo(sset_outline,obj,polygon2DInfo);

	// 7构建节点层次
	osg::Geode *polygonGeode = new osg::Geode();
	polygonGeode->addDrawable(geom);

	osg::Switch *pPolyGonSW=new osg::Switch();
	pPolyGonSW->addChild(polygonGeode);

	osg::Geode *polygonGeode_outline = new osg::Geode();
	polygonGeode_outline->addDrawable(geom_outline);

	osg::Switch *pPolyGonSW_outline=new osg::Switch();
	pPolyGonSW_outline->addChild(polygonGeode_outline);

	osg::MatrixTransform *mt = new osg::MatrixTransform();
	mt->setMatrix(polygonMT*offsetMt);

	osg::MatrixTransform *pPolyGonMT=mt;
	pPolyGonMT->addChild(pPolyGonSW);
	pPolyGonMT->addChild(pPolyGonSW_outline);

	//if (isDrawEdgeWall)
	{
		geom_edgeWall->setVertexArray(edgeWallPoints);
		geom_edgeWall->setNormalArray(edgeWallNormals);
		geom_edgeWall->setNormalBinding(osg::Geometry::BIND_PER_VERTEX);
		// 设置材质 
		osg::StateSet *sset_wall=geom_edgeWall->getOrCreateStateSet();
		createPolygon2DEdgeWallSymbolInfo(sset_wall,obj,polygon2DInfo);
		// 构建层次节点
		osg::Geode *polygonGeode_edgewall = new osg::Geode();
		polygonGeode_edgewall->addDrawable(geom_edgeWall);

		osg::Switch *pPolygonSW_edgewall=new osg::Switch();
		pPolygonSW_edgewall->addChild(polygonGeode_edgewall);
		pPolygonSW_edgewall->setName("polygonEdgeWall");
		if (isDrawEdgeWall==false)
			pPolygonSW_edgewall->setAllChildrenOff();
		pPolyGonMT->addChild(pPolygonSW_edgewall);
	}

	osg::ref_ptr<osg::Group> polygonGroup=new osg::Group();
	sset=polygonGroup->getOrCreateStateSet();
	sset->addUniform( new osg::Uniform("PolygonSelected", false) );
	sset->addUniform( new osg::Uniform("PolygonBlink", false) );
	polygonGroup->addChild(pPolyGonMT);
	return polygonGroup.release();
}

///Multi
osg::Node * CGlbGlobePolygonSymbol::buildDrawable( CGlbGlobeRObject *obj,CGlbMultiPolygon *geo )
{
	GlbRenderInfo *polygonInfo = obj->GetRenderInfo();
	GlbPolygon2DSymbolInfo *polygon2DInfo= (GlbPolygon2DSymbolInfo *)polygonInfo;
	if(geo==NULL||geo->IsEmpty())
		return NULL;
	// 边缘向下延伸	
	glbBool isDrawEdgeWall = false;
	glbBool isWallDirReserve = false;
	glbDouble wallHeight = 0.0;
	if (polygon2DInfo->isEdgeVerticalExtend != NULL)
		isDrawEdgeWall = polygon2DInfo->isEdgeVerticalExtend->GetValue(obj->GetFeature());
	if (polygon2DInfo->isExtendDirReserve != NULL)
		isWallDirReserve = polygon2DInfo->isExtendDirReserve->GetValue(obj->GetFeature());
	if (polygon2DInfo->extendDepth != NULL)
		wallHeight = polygon2DInfo->extendDepth->GetValue(obj->GetFeature());

	osg::Matrixd offsetMt = osg::Matrix::identity();
	if (polygon2DInfo->zOrAltitude != NULL)
	{
		double offsetZ= polygon2DInfo->zOrAltitude->GetValue(obj->GetFeature());
		offsetMt.setTrans(osg::Vec3d(0,0,offsetZ));
	}

	GlbGlobeTypeEnum globeTypeE=obj->GetGlobe()->GetType();
	glbBool isGlobe=GLB_GLOBETYPE_GLOBE==globeTypeE;
	glbDouble newX,newY,newZ;

	// 1计算多边形中心点位置		
	const CGlbExtent *pExt=geo->GetExtent();
	glbDouble cetX,cetY,cetZ;
	pExt->GetCenter(&cetX,&cetY,&cetZ);
	double elevation=0.0;
	GlbAltitudeModeEnum altitudeMode = obj->GetAltitudeMode();
	//if(altitudeMode == GLB_ALTITUDEMODE_RELATIVETERRAIN)
	//	elevation = obj->GetGlobe()->GetElevationAt(cetX,cetY);

	// 2计算多边形朝向
	osg::EllipsoidModel myEllipsoidModel(g_ellipsoidModel->getRadiusEquator(),g_ellipsoidModel->getRadiusPolar());
	osg::Matrixd polygonOffset;
	if (isGlobe)
		myEllipsoidModel.computeLocalToWorldTransformFromLatLongHeight(
			osg::DegreesToRadians(cetY),osg::DegreesToRadians(cetX),cetZ,polygonOffset);
	else
		polygonOffset.makeTranslate(cetX,cetY,cetZ);
	osg::Matrixd polygonOffset_inv=osg::Matrixd::inverse(polygonOffset);// 计算多边形偏移矩阵

	// 3构建各个子多边形顶点
	osg::ref_ptr<osg::Geode> polygonGeode = new osg::Geode();
	osg::ref_ptr<osg::Geode> polygonGeode_outline = new osg::Geode();
	osg::ref_ptr<osg::Geode> polygonGeode_edgewall = NULL;
	//if (isDrawEdgeWall)
		polygonGeode_edgewall = new osg::Geode();

	glbInt32 polygonCnt=geo->GetCount();
	for (glbInt32 plgIdx=0;plgIdx<polygonCnt;++plgIdx)
	{
		CGlbPolygon *polygon = const_cast<CGlbPolygon *>(geo->GetPolygon(plgIdx));
		if (polygon==NULL||polygon->IsEmpty())
			continue;
		// 3.1提取多边形原始顶点
		CGlbLine *pExtRing = const_cast<CGlbLine *>(polygon->GetExtRing());//多边形外环
		glbInt32 pntCnt=pExtRing->GetCount();
		if(pntCnt < 3) continue;
		glbDouble heightZ=0.0;
		osg::ref_ptr<osg::Vec3dArray> points=new osg::Vec3dArray();
		osg::ref_ptr<osg::Vec3Array> points_outline=new osg::Vec3Array();		
		const glbDouble* pCoords=pExtRing->GetPoints();
		
		for (glbInt32 pntIdx=0;pntIdx<pntCnt;++pntIdx)
		{
			osg::Vec3d pnt(pCoords[pntIdx*2],pCoords[pntIdx*2+1],heightZ);
			points->push_back(pnt);
			points_outline->push_back(pnt);
		}
		glbUInt32 uniqueCnt=DelaunayTriangulator_uniqueifyPoints(points);// 判断多边形是否有效
		if (uniqueCnt<3)
			continue;
		pntCnt = points->size();
		// 首尾点强制闭合
		double tt;
		if (isGlobe)
		{
			myEllipsoidModel.convertLatLongHeightToXYZ(osg::DegreesToRadians(points->at(0).y()),osg::DegreesToRadians(points->at(0).x()),points->at(0).z(),
				newX,newY,newZ);
			osg::Vec3d pos1(newX,newY,newZ);
			myEllipsoidModel.convertLatLongHeightToXYZ(osg::DegreesToRadians(points->at(points->size()-1).y()),osg::DegreesToRadians(points->at(points->size()-1).x()),points->at(points->size()-1).z(),
				newX,newY,newZ);
			osg::Vec3d pos2(newX,newY,newZ);
			tt = (pos1-pos2).length();
		}
		else
			tt = (points->at(0)-points->at(points->size()-1)).length();
		if (tt > 0.001)
		{
			points->push_back(points->at(0));
			points_outline->push_back(points_outline->at(0));
			pntCnt++;
		}		

		osg::ref_ptr<osg::Geometry> geom=new osg::Geometry();
		osg::ref_ptr<osg::Geometry> geom_outline=new osg::Geometry();
		osg::ref_ptr<osg::Geometry> geom_edgeWall=NULL;
		//if (isDrawEdgeWall)
			geom_edgeWall=new osg::Geometry();	

		geom->setVertexArray(points);
		GLint startindex=0;
		geom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::POLYGON,startindex,pntCnt));// 外环
		geom_outline->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINE_LOOP,startindex,pntCnt));// 外环
		geom_edgeWall->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUAD_STRIP,startindex*2,pntCnt*2));
		startindex+=pntCnt;
		glbInt32 inRingCnt=polygon->GetInRingCount();// 内环
		for (glbInt32 inRingIdx=0;inRingIdx<inRingCnt;++inRingIdx)
		{
			CGlbLine *pInRing = const_cast<CGlbLine *>(polygon->GetInRing(inRingIdx));
			pCoords=pInRing->GetPoints();
			glbInt32 InPntCnt=pInRing->GetCount();
			for (glbInt32 inPntIdx=0;inPntIdx<InPntCnt;++inPntIdx)
			{
				osg::Vec3d pnt(pCoords[inPntIdx*2],pCoords[inPntIdx*2+1],heightZ);
				points->push_back(pnt);
			}		
			// 内环首尾闭合
			if (isGlobe)
			{
				myEllipsoidModel.convertLatLongHeightToXYZ(osg::DegreesToRadians(points->at(startindex).y()),osg::DegreesToRadians(points->at(startindex).x()),points->at(startindex).z(),
					newX,newY,newZ);
				osg::Vec3d pos1(newX,newY,newZ);
				myEllipsoidModel.convertLatLongHeightToXYZ(osg::DegreesToRadians(points->at(points->size()-1).y()),osg::DegreesToRadians(points->at(points->size()-1).x()),points->at(points->size()-1).z(),
					newX,newY,newZ);
				osg::Vec3d pos2(newX,newY,newZ);
				tt = (pos1-pos2).length();
			}
			else
				tt = (points->at(startindex)-points->at(points->size()-1)).length();
			if (tt < 0.001)
			{
				points->push_back(points->at(startindex));
				InPntCnt++;
			}
			geom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::POLYGON,startindex,InPntCnt));
			geom_outline->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINE_LOOP,startindex,InPntCnt));// 内环
			geom_edgeWall->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUAD_STRIP,startindex*2,InPntCnt*2));
			startindex+=InPntCnt;
		}

		// 3.2多边形分格化
		osg::ref_ptr<GlbGlobe::TessellatorEx> tscx=new GlbGlobe::TessellatorEx();
		tscx->setTessellationType(GlbGlobe::TessellatorEx::TESS_TYPE_GEOMETRY);
		tscx->setBoundaryOnly(false);
		tscx->setWindingType(GlbGlobe::TessellatorEx::TESS_WINDING_ODD);
		osg::Vec3d pointpos(0.0,0.0,1.0);
		tscx->setTessellationNormal(pointpos);
		tscx->retessellatePolygons(*geom);
		osg::Vec3dArray *newPoints=(osg::Vec3dArray *)geom->getVertexArray();
		osg::Vec3dArray::size_type newPntCnt= newPoints->size();
		if (newPntCnt<3)
			continue;

		// 3.3计算多边形贴图纹理坐标
		double sizeX,sizeY,texCetX,texCetY;
		float texRotAngles=0.0f;
		if (polygon2DInfo->textureRotation)
			texRotAngles=polygon2DInfo->textureRotation->value;
		osg::Vec2dArray* texcoords = buildTexCoordsEx(newPoints,osg::DegreesToRadians(texRotAngles),sizeX,sizeY,texCetX,texCetY);//计算多边形纹理坐标
		//osg::Vec2Array* texcoords = buildTexCoordsEx(newPoints,osg::DegreesToRadians(texRotAngles),sizeX,sizeY,centX,centY);//计算多边形纹理坐标
		geom->setTexCoordArray(0, texcoords);

		// 3.4计算多边形顶点			
		osg::ref_ptr<osg::Vec3Array> vertexPoints=new osg::Vec3Array(newPoints->size());
		osg::ref_ptr<osg::Vec3Array> edgeWallPoints = NULL;
		osg::ref_ptr<osg::Vec3Array> edgeWallNormals = NULL;
		//if (isDrawEdgeWall)// 边缘延伸
			edgeWallPoints = new osg::Vec3Array(newPoints->size()*2);
			edgeWallNormals = new osg::Vec3Array(newPoints->size()*2);

		if (isGlobe)
		{
			if (altitudeMode == GLB_ALTITUDEMODE_ABSOLUTE)
			{
				for (osg::Vec3Array::size_type inewPntIdx=0;inewPntIdx<newPntCnt;++inewPntIdx)
				{
					const osg::Vec3d &iPnt=newPoints->at(inewPntIdx);
					myEllipsoidModel.convertLatLongHeightToXYZ(osg::DegreesToRadians(iPnt.y()),osg::DegreesToRadians(iPnt.x()),iPnt.z(),
						newX,newY,newZ);
					osg::Vec3d pos(newX,newY,newZ);
					vertexPoints->at(inewPntIdx) = pos * polygonOffset_inv;

					//if (isDrawEdgeWall)
					{// 边缘延伸
						edgeWallPoints->at(2*inewPntIdx)=pos*polygonOffset_inv;
						double cc = isWallDirReserve ? 1.0 : -1.0;
						myEllipsoidModel.convertLatLongHeightToXYZ(osg::DegreesToRadians(iPnt.y()),osg::DegreesToRadians(iPnt.x()),iPnt.z()+cc*wallHeight,
							newX,newY,newZ);
						edgeWallPoints->at(2*inewPntIdx+1)=osg::Vec3d(newX,newY,newZ)*polygonOffset_inv;

						if (inewPntIdx <newPntCnt-1)
						{// 非最后一个点用当前点和下一个点向下延伸形成面
							osg::Vec3 v0 = edgeWallPoints->at(2*inewPntIdx);
							osg::Vec3 v1 = edgeWallPoints->at(2*inewPntIdx+1);
							osg::Vec3d cc=newPoints->at(inewPntIdx+1);
							myEllipsoidModel.convertLatLongHeightToXYZ(osg::DegreesToRadians(cc.y()),osg::DegreesToRadians(cc.x()),cc.z(),
								newX,newY,newZ);
							osg::Vec3 v2 = osg::Vec3(newX,newY,newZ);
							osg::Vec3 n = ComputeNormal(v0,v1,v2);
							edgeWallNormals->at(2*inewPntIdx) = n;
							edgeWallNormals->at(2*inewPntIdx+1) = n;
						}
						else
						{
							edgeWallNormals->at(2*inewPntIdx) = edgeWallNormals->at(2*inewPntIdx-1);
							edgeWallNormals->at(2*inewPntIdx+1) = edgeWallNormals->at(2*inewPntIdx-1);
						}
					}
				}
			}
			else if (altitudeMode == GLB_ALTITUDEMODE_RELATIVETERRAIN)
			{
				for (osg::Vec3Array::size_type inewPntIdx=0;inewPntIdx<newPntCnt;++inewPntIdx)
				{
					const osg::Vec3d &iPnt=newPoints->at(inewPntIdx);
					myEllipsoidModel.convertLatLongHeightToXYZ(osg::DegreesToRadians(iPnt.y()),osg::DegreesToRadians(iPnt.x()),iPnt.z()+elevation,
						newX,newY,newZ);
					osg::Vec3d pos(newX,newY,newZ);
					vertexPoints->at(inewPntIdx) = pos * polygonOffset_inv;
					//if (isDrawEdgeWall)
					{// 边缘延伸
						edgeWallPoints->at(2*inewPntIdx)=pos*polygonOffset_inv;
						double cc = isWallDirReserve ? 1.0 : -1.0;
						myEllipsoidModel.convertLatLongHeightToXYZ(osg::DegreesToRadians(iPnt.y()),osg::DegreesToRadians(iPnt.x()),iPnt.z()+elevation+cc*wallHeight,
							newX,newY,newZ);
						edgeWallPoints->at(2*inewPntIdx+1)=osg::Vec3d(newX,newY,newZ)*polygonOffset_inv;
						if (inewPntIdx <newPntCnt-1)
						{// 非最后一个点用当前点和下一个点向下延伸形成面
							osg::Vec3 v0 = edgeWallPoints->at(2*inewPntIdx);
							osg::Vec3 v1 = edgeWallPoints->at(2*inewPntIdx+1);
							osg::Vec3d cc=newPoints->at(inewPntIdx+1);
							myEllipsoidModel.convertLatLongHeightToXYZ(osg::DegreesToRadians(cc.y()),osg::DegreesToRadians(cc.x()),cc.z(),
								newX,newY,newZ);
							osg::Vec3 v2 = osg::Vec3(newX,newY,newZ);
							osg::Vec3 n = ComputeNormal(v0,v1,v2);
							edgeWallNormals->at(2*inewPntIdx) = n;
							edgeWallNormals->at(2*inewPntIdx+1) = n;
						}
						else
						{
							edgeWallNormals->at(2*inewPntIdx) = edgeWallNormals->at(2*inewPntIdx-1);
							edgeWallNormals->at(2*inewPntIdx+1) = edgeWallNormals->at(2*inewPntIdx-1);
						}
					}
				}
			}
			//else if (mpt_altitudeMode == GLB_ALTITUDEMODE_ONTERRAIN)
			//{
			//	for (osg::Vec3Array::size_type inewPntIdx=0;inewPntIdx<newPntCnt;++inewPntIdx)
			//	{
			//		const osg::Vec3d &iPnt=newPoints->at(inewPntIdx);
			//		myEllipsoidModel.convertLatLongHeightToXYZ(osg::DegreesToRadians(iPnt.y()),osg::DegreesToRadians(iPnt.x()),elevation,
			//			newX,newY,newZ);
			//		osg::Vec3d pos(newX,newY,newZ);
			//		pos-=transOffset;
			//		vertexPoints->at(inewPntIdx)=pos*polygonRotOffset_inv;
			//	}
			//}
		}
		else
		{
			if (altitudeMode == GLB_ALTITUDEMODE_ABSOLUTE)
			{
				for (osg::Vec3Array::size_type inewPntIdx=0;inewPntIdx<newPntCnt;++inewPntIdx)
				{
					const osg::Vec3d &iPnt = newPoints->at(inewPntIdx);
					vertexPoints->at(inewPntIdx) = iPnt * polygonOffset_inv;
					//if (isDrawEdgeWall)
					{// 边缘延伸
						edgeWallPoints->at(2*inewPntIdx)=iPnt*polygonOffset_inv;
						double cc = isWallDirReserve ? 1.0 : -1.0;					
						edgeWallPoints->at(2*inewPntIdx+1)=osg::Vec3d(iPnt.x(),iPnt.y(),iPnt.z()+cc*wallHeight)*polygonOffset_inv;
						if (inewPntIdx <newPntCnt-1)
						{// 非最后一个点用当前点和下一个点向下延伸形成面
							osg::Vec3 v0 = edgeWallPoints->at(2*inewPntIdx);
							osg::Vec3 v1 = edgeWallPoints->at(2*inewPntIdx+1);					
							osg::Vec3 v2 = newPoints->at(inewPntIdx+1)*polygonOffset_inv;
							osg::Vec3 n = ComputeNormal(v0,v1,v2);
							edgeWallNormals->at(2*inewPntIdx) = n;
							edgeWallNormals->at(2*inewPntIdx+1) = n;
						}
						else
						{
							edgeWallNormals->at(2*inewPntIdx) = edgeWallNormals->at(2*inewPntIdx-1);
							edgeWallNormals->at(2*inewPntIdx+1) = edgeWallNormals->at(2*inewPntIdx-1);
						}
					}
				}
			}
			else if (altitudeMode == GLB_ALTITUDEMODE_RELATIVETERRAIN)
			{
				for (osg::Vec3Array::size_type inewPntIdx=0;inewPntIdx<newPntCnt;++inewPntIdx)
				{
					const osg::Vec3d &iPnt = newPoints->at(inewPntIdx);
					osg::Vec3d pos = iPnt;pos.z() += elevation;
					vertexPoints->at(inewPntIdx) = pos * polygonOffset_inv;
					//if (isDrawEdgeWall)
					{// 边缘延伸
						edgeWallPoints->at(2*inewPntIdx)=iPnt*polygonOffset_inv;
						double cc = isWallDirReserve ? 1.0 : -1.0;					
						edgeWallPoints->at(2*inewPntIdx+1)=osg::Vec3d(pos.x(),pos.y(),pos.z()+cc*wallHeight)*polygonOffset_inv;
						if (inewPntIdx <newPntCnt-1)
						{// 非最后一个点用当前点和下一个点向下延伸形成面
							osg::Vec3 v0 = edgeWallPoints->at(2*inewPntIdx);
							osg::Vec3 v1 = edgeWallPoints->at(2*inewPntIdx+1);					
							osg::Vec3 v2 = newPoints->at(inewPntIdx+1)*polygonOffset_inv;
							osg::Vec3 n = ComputeNormal(v0,v1,v2);
							edgeWallNormals->at(2*inewPntIdx) = n;
							edgeWallNormals->at(2*inewPntIdx+1) = n;
						}
						else
						{
							edgeWallNormals->at(2*inewPntIdx) = edgeWallNormals->at(2*inewPntIdx-1);
							edgeWallNormals->at(2*inewPntIdx+1) = edgeWallNormals->at(2*inewPntIdx-1);
						}
					}
				}
			}
			//else if (altitudeMode == GLB_ALTITUDEMODE_ONTERRAIN)
			//{
			//	for (osg::Vec3Array::size_type inewPntIdx=0;inewPntIdx<newPntCnt;++inewPntIdx)
			//	{
			//		const osg::Vec3d &iPnt=newPoints->at(inewPntIdx);
			//		osg::Vec3d pos=iPnt;pos.z()+=elevation;pos-=transOffset;
			//		vertexPoints->at(inewPntIdx)=pos*polygonRotOffset_inv;
			//	}
			//}
		}
		geom->setVertexArray(vertexPoints);
		geom_outline->setVertexArray(vertexPoints);

		// 3.5构建材质
		osg::StateSet *sset=geom->getOrCreateStateSet();
		createPolygon2DSymbolInfo(sset,obj,polygon2DInfo,texCetX,texCetY,sizeX,sizeY);
		osg::StateSet *sset_outline=geom_outline->getOrCreateStateSet();
		createPolygon2DOutlineSymbolInfo(sset_outline,obj,polygon2DInfo);

		polygonGeode->addDrawable(geom);
		polygonGeode_outline->addDrawable(geom_outline);

		//if (isDrawEdgeWall)
		{// 边缘延伸
			geom_edgeWall->setVertexArray(edgeWallPoints);
			geom_edgeWall->setNormalArray(edgeWallNormals);
			geom_edgeWall->setNormalBinding(osg::Geometry::BIND_PER_VERTEX);
			sset=geom_edgeWall->getOrCreateStateSet();
			createPolygon2DEdgeWallSymbolInfo(sset,obj,polygon2DInfo);
			polygonGeode_edgewall->addDrawable(geom_edgeWall);
		}
	}

	// 4构建节点层次
	osg::Switch *pPolyGonSW=new osg::Switch();
	pPolyGonSW->addChild(polygonGeode);

	osg::Switch *pPolyGonSW_outline=new osg::Switch();
	pPolyGonSW_outline->addChild(polygonGeode_outline);

	osg::MatrixTransform *mt = new osg::MatrixTransform();
	mt->setMatrix(polygonOffset*offsetMt);

	osg::MatrixTransform *pPolyGonMT=mt;
	pPolyGonMT->addChild(pPolyGonSW);
	pPolyGonMT->addChild(pPolyGonSW_outline);

	//if (isDrawEdgeWall)
	{// 边缘延伸
		osg::Switch *pPolyGonSW_edgewall=new osg::Switch();
		pPolyGonSW_edgewall->setName("polygonEdgeWall");
		if (isDrawEdgeWall==false)
			pPolyGonSW_edgewall->setAllChildrenOff();
		pPolyGonSW_edgewall->addChild(polygonGeode_edgewall);
		pPolyGonMT->addChild(pPolyGonSW_edgewall);
	}

	osg::ref_ptr<osg::Group> polygonGroup=new osg::Group();
	osg::StateSet *ssetPly=polygonGroup->getOrCreateStateSet();
	ssetPly->addUniform( new osg::Uniform("PolygonSelected", false) );
	ssetPly->addUniform( new osg::Uniform("PolygonBlink", false) );
	polygonGroup->addChild(pPolyGonMT);
	return polygonGroup.release();
}

//////////////////////////////////////////////////////////////////////////
osg::Vec2Array * CGlbGlobePolygonSymbol::buildTexCoords(osg::Vec3Array *points,double &sizeX,double &sizeY,double &centX,double &centY)const
{
	double dmin_lon=std::numeric_limits<double>::max();
	double dmax_lon=std::numeric_limits<double>::min();
	double dmin_lat=std::numeric_limits<double>::max();
	double dmax_lat=std::numeric_limits<double>::min();
	osg::Vec3Array::const_iterator iterBegin=points->begin();
	osg::Vec3Array::const_iterator iterEnd=points->end();
	for (osg::Vec3Array::const_iterator iter=iterBegin;iter!=iterEnd;++iter)
	{
		if (iter->x()<dmin_lon)
			dmin_lon=iter->x();
		if (iter->x()>dmax_lon)
			dmax_lon=iter->x();
		if (iter->y()<dmin_lat)
			dmin_lat=iter->y();
		if (iter->y()>dmax_lat)
			dmax_lat=iter->y();
	}
	/*double */sizeX=dmax_lon-dmin_lon;
	/*double */sizeY=dmax_lat-dmin_lat;
	centX=(dmax_lon+dmin_lon)/2.0;
	centY=(dmax_lat+dmin_lat)/2.0;
		
	osg::Vec3Array::size_type pntCnt=points->size();
	osg::Vec2Array *texCoords=new osg::Vec2Array(pntCnt);
	pntCnt=0;
	for (osg::Vec3Array::const_iterator iter=iterBegin;iter!=iterEnd;++iter)
	{
		//texCoords->at(pntCnt).set((iter->x()-dmin_lon)/sizeX,(iter->y()-dmin_lat)/sizeY);
		texCoords->at(pntCnt).set(iter->x(),iter->y());
		++pntCnt;
	}
	return texCoords;
}
osg::Vec2Array * CGlbGlobePolygonSymbol::buildTexCoordsEx(osg::Vec3Array *points,float rotAnge_rad,double &sizeX,double &sizeY,double &centX,double &centY)const
{
	osg::Vec3Array::size_type pntCnt = points->size();		
	osg::Vec2Array *texCoords=new osg::Vec2Array(pntCnt);

	// 1、计算旋转中心
	double dmin_lon=std::numeric_limits<double>::max();
	double dmax_lon=std::numeric_limits<double>::min();
	double dmin_lat=std::numeric_limits<double>::max();
	double dmax_lat=std::numeric_limits<double>::min();
	osg::Vec3Array::const_iterator iterBegin=points->begin();
	osg::Vec3Array::const_iterator iterEnd=points->end();
	for (osg::Vec3Array::const_iterator iter=iterBegin;iter!=iterEnd;++iter)
	{
		if (iter->x()<dmin_lon)
			dmin_lon=iter->x();
		if (iter->x()>dmax_lon)
			dmax_lon=iter->x();
		if (iter->y()<dmin_lat)
			dmin_lat=iter->y();
		if (iter->y()>dmax_lat)
			dmax_lat=iter->y();
	}
	/*sizeX=dmax_lon-dmin_lon;
	sizeY=dmax_lat-dmin_lat;*/
	centX=(dmax_lon+dmin_lon)/2.0;
	centY=(dmax_lat+dmin_lat)/2.0;//旋转中心
		

	// 2、旋转
	osg::Vec3f rotCenter(centX,centY,0.0);
	osg::Matrixf rotMat=osg::Matrixf::rotate(rotAnge_rad,osg::Vec3f(0.0,0.0,1.0));		
	pntCnt=0;
	osg::Vec3d newUV;
	/*double */dmin_lon=std::numeric_limits<double>::max();
	/*double */dmax_lon=std::numeric_limits<double>::min();
	/*double */dmin_lat=std::numeric_limits<double>::max();
	/*double */dmax_lat=std::numeric_limits<double>::min();
	for (osg::Vec3Array::const_iterator iter=iterBegin;iter!=iterEnd;++iter)//旋转
	{
		//texCoords->at(pntCnt).set(iter->x(),iter->y());
		//newUV=(*iter/*-rotCenter*/)*rotMat;
		newUV=(*iter-rotCenter)*rotMat;
		texCoords->at(pntCnt).set(newUV.x(),newUV.y());
		/*newUV=(*iter-rotCenter)*rotMat;
		texCoords->at(pntCnt).set(newUV.x(),newUV.y());*/

		if (newUV.x()<dmin_lon)
			dmin_lon=newUV.x();
		if (newUV.x()>dmax_lon)
			dmax_lon=newUV.x();
		if (newUV.y()<dmin_lat)
			dmin_lat=newUV.y();
		if (newUV.y()>dmax_lat)
			dmax_lat=newUV.y();

		++pntCnt;
	}
	centX=(dmax_lon+dmin_lon)/2.0;
	centY=(dmax_lat+dmin_lat)/2.0;//旋转中心重新计算
	sizeX=dmax_lon-dmin_lon;
	sizeY=dmax_lat-dmin_lat;
	//// 3、计算旋转以后的uv
	//pntCnt=0;
	//osg::Vec2f downLeft(dmin_lon,dmin_lat);
	//osg::Vec2f rotCenter2(centX,centY);
	//osg::Vec2Array::iterator iterBegin_New=texCoords->begin();
	//osg::Vec2Array::iterator iterEnd_New=texCoords->end();
	//for (osg::Vec2Array::iterator iter=iterBegin_New;iter!=iterEnd_New;++iter)
	////for (osg::Vec3Array::const_iterator iter=iterBegin;iter!=iterEnd;++iter)
	//{
	//	//texCoords->at(pntCnt).set((iter->x()-dmin_lon)/sizeX,(iter->y()-dmin_lat)/sizeY);
	//	//texCoords->at(pntCnt).set((iter->x()+centX-dmin_lon)/sizeX,(iter->y()+centY-dmin_lat)/sizeY);
	//	/**iter=*iter+rotCenter2-downLeft;
	//	iter->set(iter->x()/sizeX,iter->y()/sizeY);*/


	//	++pntCnt;
	//}
	return texCoords;
}
osg::Vec2dArray * CGlbGlobePolygonSymbol::buildTexCoordsEx(osg::Vec3dArray *points,float rotAnge_rad,double &sizeX,double &sizeY,double &centX,double &centY)const
{
	osg::Vec3dArray::size_type pntCnt = points->size();		
	osg::Vec2dArray *texCoords=new osg::Vec2dArray(pntCnt);

	// 1、计算旋转中心
	double dmin_lon=std::numeric_limits<double>::max();
	double dmax_lon=std::numeric_limits<double>::min();
	double dmin_lat=std::numeric_limits<double>::max();
	double dmax_lat=std::numeric_limits<double>::min();
	osg::Vec3dArray::const_iterator iterBegin=points->begin();
	osg::Vec3dArray::const_iterator iterEnd=points->end();
	for (osg::Vec3dArray::const_iterator iter=iterBegin;iter!=iterEnd;++iter)
	{
		if (iter->x()<dmin_lon)
			dmin_lon=iter->x();
		if (iter->x()>dmax_lon)
			dmax_lon=iter->x();
		if (iter->y()<dmin_lat)
			dmin_lat=iter->y();
		if (iter->y()>dmax_lat)
			dmax_lat=iter->y();
	}
	/*sizeX=dmax_lon-dmin_lon;
	sizeY=dmax_lat-dmin_lat;*/
	centX=(dmax_lon+dmin_lon)/2.0;
	centY=(dmax_lat+dmin_lat)/2.0;//旋转中心
		

	// 2、旋转
	osg::Vec3d rotCenter(centX,centY,0.0);
	osg::Matrixd rotMat=osg::Matrixd::rotate(rotAnge_rad,osg::Vec3d(0.0,0.0,1.0));		
	pntCnt=0;
	osg::Vec3d newUV;
	/*double */dmin_lon=std::numeric_limits<double>::max();
	/*double */dmax_lon=std::numeric_limits<double>::min();
	/*double */dmin_lat=std::numeric_limits<double>::max();
	/*double */dmax_lat=std::numeric_limits<double>::min();
	for (osg::Vec3dArray::const_iterator iter=iterBegin;iter!=iterEnd;++iter)//旋转
	{
		//texCoords->at(pntCnt).set(iter->x(),iter->y());
		//newUV=(*iter/*-rotCenter*/)*rotMat;
		newUV=(*iter-rotCenter)*rotMat;
		texCoords->at(pntCnt).set(newUV.x(),newUV.y());
		/*newUV=(*iter-rotCenter)*rotMat;
		texCoords->at(pntCnt).set(newUV.x(),newUV.y());*/

		if (newUV.x()<dmin_lon)
			dmin_lon=newUV.x();
		if (newUV.x()>dmax_lon)
			dmax_lon=newUV.x();
		if (newUV.y()<dmin_lat)
			dmin_lat=newUV.y();
		if (newUV.y()>dmax_lat)
			dmax_lat=newUV.y();

		++pntCnt;
	}
	centX=(dmax_lon+dmin_lon)/2.0;
	centY=(dmax_lat+dmin_lat)/2.0;//旋转中心重新计算
	sizeX=dmax_lon-dmin_lon;
	sizeY=dmax_lat-dmin_lat;
	//// 3、计算旋转以后的uv
	//pntCnt=0;
	//osg::Vec2f downLeft(dmin_lon,dmin_lat);
	//osg::Vec2f rotCenter2(centX,centY);
	//osg::Vec2Array::iterator iterBegin_New=texCoords->begin();
	//osg::Vec2Array::iterator iterEnd_New=texCoords->end();
	//for (osg::Vec2Array::iterator iter=iterBegin_New;iter!=iterEnd_New;++iter)
	////for (osg::Vec3Array::const_iterator iter=iterBegin;iter!=iterEnd;++iter)
	//{
	//	//texCoords->at(pntCnt).set((iter->x()-dmin_lon)/sizeX,(iter->y()-dmin_lat)/sizeY);
	//	//texCoords->at(pntCnt).set((iter->x()+centX-dmin_lon)/sizeX,(iter->y()+centY-dmin_lat)/sizeY);
	//	/**iter=*iter+rotCenter2-downLeft;
	//	iter->set(iter->x()/sizeX,iter->y()/sizeY);*/


	//	++pntCnt;
	//}
	return texCoords;
}

void CGlbGlobePolygonSymbol::DealAggTexture(agg::rasterizer_scanline_aa<> &ras,renb_type &renb,
							IGlbGeometry *geom,GlbPolygon2DSymbolInfo *polyInfo,
							CGlbFeature *feature,glbBool &isRenderTex)
{
	if (polyInfo==NULL || polyInfo->textureData==NULL)
		return;	
	CGlbWString filePath = polyInfo->textureData->GetValue(feature);
	if (filePath.empty())
		return;	

	GlbGlobeTexRepeatModeEnum repeatMode = (GlbGlobeTexRepeatModeEnum)(polyInfo->texRepeatMode->GetValue(feature));
	glbFloat tilingU = 1,tilingV = 1;
	if (repeatMode==GLB_TEXTUREREPEAT_TIMES)
	{
		if (polyInfo->tilingU)
			tilingU = polyInfo->tilingU->GetValue(feature);
		if(polyInfo->tilingV)
			tilingV = polyInfo->tilingV->GetValue(feature);
	}
	else if (repeatMode==GLB_TEXTUREREPEAT_SIZE)
	{
		glbInt32 tilingU_size, tilingV_size;
		if (polyInfo->tilingU)
			tilingU_size = polyInfo->tilingU->GetValue(feature);
		if(polyInfo->tilingV)
			tilingV_size = polyInfo->tilingV->GetValue(feature);

		glbref_ptr<CGlbExtent> polyExt = const_cast<CGlbExtent *>(geom->GetExtent());
		tilingU = polyExt->GetXWidth() / tilingU_size + 0.5;
		tilingV = polyExt->GetYHeight() / tilingV_size + 0.5;
	}

	if (tilingU==1 && tilingV==1)
	{// 非重复纹理
		DealAggWrapTexture(ras,renb,geom,polyInfo,feature,isRenderTex);	
	}
	else if (tilingU>=1 && tilingV>=1)
	{
		DealAggRepeatTexture(ras,renb,geom, polyInfo,feature,isRenderTex,tilingU,tilingV);		
	}
}

void CGlbGlobePolygonSymbol::DealAggRepeatTexture(agg::rasterizer_scanline_aa<> &ras,renb_type &renb,
													IGlbGeometry *geom,GlbPolygon2DSymbolInfo *polyInfo,
													CGlbFeature *feature,glbBool &isRenderTex,glbFloat tilingU,glbFloat tilingV)
{
	CGlbPolygon* poly = dynamic_cast<CGlbPolygon*>(geom);
	if (!poly)
		return;

	osg::ref_ptr<osg::Image> image = NULL;
	if(polyInfo && polyInfo->textureData)
	{
		CGlbWString filePath = polyInfo->textureData->GetValue(feature);
		//CGlbWString extname = CGlbPath::GetExtentname(filePath);
		glbInt32 index = filePath.find_first_of(L'.');
		if(index == 0)
		{
			CGlbWString execDir = CGlbPath::GetExecDir();
			filePath = execDir + filePath.substr(1,filePath.size());
		}
		image = osgDB::readImageFile(filePath.ToString());
		if (image==NULL && filePath.length()>0)
			GlbLogWOutput(GLB_LOGTYPE_ERR,L"读取多边形图像文件(%s)失败. \r\n",filePath.ToString().c_str());
	}

	if(image == NULL)
		return;

	cglbExtent = const_cast<CGlbExtent *>(poly->GetExtent());
	double cx,cy;
	cglbExtent->GetCenter(&cx,&cy);
	
	// 需要纹理渲染
	isRenderTex = true;
	double rotation = 0.0;
	if(polyInfo->textureRotation)
		rotation = polyInfo->textureRotation->GetValue(feature);		

	agg::scanline_p8 sl;
	agg::int8u alpha(255);//图片透明度
	if(polyInfo->opacity)
		alpha = agg::int8u(polyInfo->opacity->GetValue(feature) * 255 / 100.0);
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
	CGlbLine* extRing = const_cast<CGlbLine *>(poly->GetExtRing());
	glbInt32 ptCnt = extRing->GetCount();
	for (glbInt32 i = 0; i < ptCnt; i++)
	{
		extRing->GetPoint(i,&ptx,&pty);
		double new_x = (ptx - tempExtent.GetLeft()) * (tempImageW / (tempExtent.GetRight() - tempExtent.GetLeft()));
		double new_y = (pty - tempExtent.GetBottom()) * (tempImageH / (tempExtent.GetTop() - tempExtent.GetBottom()));			

		if(i == 0)
		{
			texRas.move_to_d(new_x,new_y);
			continue;
		}
		texRas.line_to_d(new_x,new_y);		
	}	

	glbInt32 inRingCnt = poly->GetInRingCount();
	for (glbInt32 j = 0; j < inRingCnt; j++)
	{
		CGlbLine* inRing = const_cast<CGlbLine *>(poly->GetInRing(j));
		glbInt32 ptCnt = inRing->GetCount();
		for (glbInt32 k = 0; k < ptCnt; k++)
		{
			inRing->GetPoint(k,&ptx,&pty);
			double new_x = (ptx - tempExtent.GetLeft()) * (tempImageW / (tempExtent.GetRight() - tempExtent.GetLeft()));
			double new_y = (pty - tempExtent.GetBottom()) * (tempImageH / (tempExtent.GetTop() - tempExtent.GetBottom()));			

			if(k == 0)
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

void CGlbGlobePolygonSymbol::DealAggWrapTexture(agg::rasterizer_scanline_aa<> &ras,renb_type &renb,
													IGlbGeometry *geom,GlbPolygon2DSymbolInfo *polyInfo,
													CGlbFeature *feature,glbBool &isRenderTex)
{
	CGlbPolygon* poly = dynamic_cast<CGlbPolygon*>(geom);
	if (!poly)
		return;

	osg::ref_ptr<osg::Image> image = NULL;
	if(polyInfo && polyInfo->textureData)
	{
		CGlbWString filePath = polyInfo->textureData->GetValue(feature);
		glbInt32 index = filePath.find_first_of(L'.');
		if(index == 0)
		{
			CGlbWString execDir = CGlbPath::GetExecDir();
			filePath = execDir + filePath.substr(1,filePath.size());
		}
		image = osgDB::readImageFile(filePath.ToString());
		if (image==NULL && filePath.length()>0)
			GlbLogWOutput(GLB_LOGTYPE_ERR,L"读取多边形图像文件(%s)失败. \r\n",filePath.ToString().c_str());
	}

	if(image == NULL)
		return;

	cglbExtent = const_cast<CGlbExtent *>(poly->GetExtent());

	double cx,cy;
	cglbExtent->GetCenter(&cx,&cy);

	// 需要纹理渲染
	isRenderTex = true;
	double rotation = 0.0;
	if(polyInfo->textureRotation)
		rotation = polyInfo->textureRotation->GetValue(feature);	

	agg::scanline_p8 sl;
	agg::int8u alpha(255);//图片透明度
	if(polyInfo->opacity)
		alpha = agg::int8u(polyInfo->opacity->GetValue(feature) * 255 / 100.0);
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
	// 1. 首先实现旋转
	img_mtx.multiply(agg::trans_affine_translation(-image->s()*0.5,-image->t()*0.5));
	img_mtx.multiply(agg::trans_affine_rotation(agg::deg2rad(rotation)));
	img_mtx.multiply(agg::trans_affine_translation(image->s()*0.5,image->t()*0.5));
	//img_mtx.translate(-image->s()*0.5,-image->t()*0.5);
	//img_mtx.rotate(agg::deg2rad(rotation));
	//img_mtx.translate(image->s()*0.5,image->t()*0.5);
	// 2. 图像缩放
	double imgscalex = double(tempImageW) / image->s() * (cglbExtent->GetXWidth() / tempExtent.GetXWidth());
	double imgscaley = double(tempImageH) / image->t() * (cglbExtent->GetYHeight() / tempExtent.GetYHeight());	
	img_mtx.multiply(agg::trans_affine_scaling(imgscalex ,imgscaley));
	//img_mtx.scale(imgscalex,imgscaley);	

	// 3. 先把图像中心点偏移到(cglbExtent->GetLeft(),cglbExtent->GetBottom())处,以实现中心旋转
	double new_basex = cglbExtent->GetLeft();
	double new_basey = cglbExtent->GetBottom();
	double transx_first = (new_basex-tempExtent.GetLeft()) * tempImageW / tempExtent.GetXWidth();
	double transy_first = (new_basey-tempExtent.GetBottom()) * tempImageH / tempExtent.GetYHeight();
	img_mtx.multiply(agg::trans_affine_translation(transx_first,transy_first));
	//img_mtx.translate(transx_first,transy_first);		

	// 4. 矩阵取反
	img_mtx.invert();//注意这里

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
	agg::rasterizer_scanline_aa<> texRas;//处理纹理

	glbDouble ptx,pty;
	CGlbLine* extRing = const_cast<CGlbLine *>(poly->GetExtRing());
	glbInt32 ptCnt = extRing->GetCount();
	for (glbInt32 i = 0; i < ptCnt; i++)
	{
		extRing->GetPoint(i,&ptx,&pty);
		double new_x = (ptx - tempExtent.GetLeft()) * (tempImageW / (tempExtent.GetRight() - tempExtent.GetLeft()));
		double new_y = (pty - tempExtent.GetBottom()) * (tempImageH / (tempExtent.GetTop() - tempExtent.GetBottom()));			

		if(i == 0)
		{
			texRas.move_to_d(new_x,new_y);
			continue;
		}
		texRas.line_to_d(new_x,new_y);		
	}	

	glbInt32 inRingCnt = poly->GetInRingCount();
	for (glbInt32 j = 0; j < inRingCnt; j++)
	{
		CGlbLine* inRing = const_cast<CGlbLine *>(poly->GetInRing(j));
		glbInt32 ptCnt = inRing->GetCount();
		for (glbInt32 k = 0; k < ptCnt; k++)
		{
			inRing->GetPoint(k,&ptx,&pty);
			double new_x = (ptx - tempExtent.GetLeft()) * (tempImageW / (tempExtent.GetRight() - tempExtent.GetLeft()));
			double new_y = (pty - tempExtent.GetBottom()) * (tempImageH / (tempExtent.GetTop() - tempExtent.GetBottom()));			

			if(k == 0)
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

