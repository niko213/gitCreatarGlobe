#include "StdAfx.h"
#include "GlbGlobeMarker2DShapeSymbol.h"
#include "GlbPoint.h"
#include "GlbLine.h"
#include "osg/Geode"
#include "GlbGlobeCustomShape.h"
#include "osg/Material"
#include "osg/LineWidth"
#include "osg/LineStipple"
#include "osg/Texture2D"
#include "osg/TexMat"
#include "osg/PolygonMode"
#include "osg/PolygonOffset"
#include "osgDB/ReadFile"
#include "GlbString.h"
#include "CGlbGlobe.h"
#include <osg/ImageStream>

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

//ceshi
#include "platform/win32/agg_win32_bmp.h"
#include "GlbConvert.h"
#include "comutil.h"

#include "util/agg_color_conv.h"
#include "util/agg_color_conv_rgb8.h"
#include "platform/agg_platform_support.h"

#include "GlbGlobeLinePixelSymbol.h"

#include <agg_conv_dash.h> // conv_dash

using namespace GlbGlobe;

CGlbGlobeMarker2DShapeSymbol::CGlbGlobeMarker2DShapeSymbol(void)
{
	cglbExtent = NULL;
	mpr_globeType = GLB_GLOBETYPE_GLOBE;
}

CGlbGlobeMarker2DShapeSymbol::~CGlbGlobeMarker2DShapeSymbol(void)
{
	cglbExtent = NULL;
	cglbOrginExtent = NULL;
}

osg::Node * CGlbGlobeMarker2DShapeSymbol::Draw( CGlbGlobeRObject *obj,IGlbGeometry *geo )
{
	GlbRenderInfo *renderInfo = obj->GetRenderInfo();
	GlbMarker2DShapeSymbolInfo *marker2DInfo = 
		static_cast<GlbMarker2DShapeSymbolInfo *>(renderInfo);
	
	if(marker2DInfo == NULL)
		return NULL;

	osg::Vec3d centerPosition;
	
	IGlbGeometry *geometry = geo;
	CGlbPoint *point3D = dynamic_cast<CGlbPoint*>(geometry);
	if(point3D)
		point3D->GetXYZ(&centerPosition.x(),&centerPosition.y(),&centerPosition.z());
	
	osg::Vec3d centerPoint(0.0,0.0,0.0);
	CGlbFeature *feature = obj->GetFeature();

	osg::Group *group = new osg::Group;
	osg::ref_ptr<osg::Geode> geode = new osg::Geode();
	IGlbGeometry *outGeometry = DealOutline3DVertexes(centerPosition,marker2DInfo,feature,obj->GetGlobe()->GetType());
	osg::ref_ptr<osg::MatrixTransform> outlineMt = new osg::MatrixTransform;
	osg::Node *outlineNode = NULL;
	if(outGeometry)
	{
		CGlbLine *line3D = dynamic_cast<CGlbLine *>(outGeometry);
		const glbDouble *line3DPoints = line3D->GetPoints();
		osg::Vec3d firstPoint(line3DPoints[0],line3DPoints[1],line3DPoints[2]);
		if(obj->GetAltitudeMode() == GLB_ALTITUDEMODE_RELATIVETERRAIN)
		{
			centerPosition.z() = centerPosition.z() + obj->GetGlobe()->GetElevationAt(centerPosition.x(),centerPosition.y());
			firstPoint.z() = firstPoint.z() + obj->GetGlobe()->GetElevationAt(firstPoint.x(),firstPoint.y());
		}
		outlineNode = CGlbGlobeLinePixelSymbol::DrawBorderline(obj,outGeometry,marker2DInfo->fillInfo->outlineInfo);
		if(outlineNode == NULL) return NULL;
		outlineMt->addChild(outlineNode);
		if(GLB_GLOBETYPE_GLOBE == obj->GetGlobe()->GetType())
		{
			osg::Matrixd cenToWord;
			g_ellipsoidModel->computeLocalToWorldTransformFromLatLongHeight(
				osg::DegreesToRadians(centerPosition.y()),osg::DegreesToRadians(centerPosition.x()),
				centerPosition.z(),cenToWord);
			osg::Matrixd firstToWord;
			g_ellipsoidModel->computeLocalToWorldTransformFromLatLongHeight(
				osg::DegreesToRadians(firstPoint.y()),osg::DegreesToRadians(firstPoint.x()),
				firstPoint.z(),firstToWord);
			osg::Matrixd firstTocen = firstToWord * osg::Matrixd::inverse(cenToWord)  ;
			outlineMt->setMatrix(firstTocen);
		}
		else if(GLB_GLOBETYPE_FLAT == obj->GetGlobe()->GetType())
		{
			osg::Matrixd trans;
			osg::Matrixd trans2;
			trans2.makeTranslate(firstPoint.x(),firstPoint.y(),firstPoint.z());
			trans.makeTranslate(centerPosition.x(),centerPosition.y(),centerPosition.z());
			osg::Matrixd tempTrans = trans2 * osg::Matrixd::inverse(trans);
			outlineMt->setMatrix(tempTrans);
		}
	}

	if (obj->GetAltitudeMode() != GLB_ALTITUDEMODE_ONTERRAIN)
	{
		if(marker2DInfo->shapeInfo == NULL)
			return NULL;

		ref_ptr<GlobeShapeDrawable> shapeFillDrawable = NULL;
		ref_ptr<GlobeShapeDrawable> shapeOutDrawable = NULL;
		if (marker2DInfo->shapeInfo->shapeType == GLB_MARKERSHAPE_CIRCLE)
		{
			GlbCircleInfo *circleInfo = 
				static_cast<GlbCircleInfo *>(marker2DInfo->shapeInfo);
			if(circleInfo)
			{
				ref_ptr<CustomCircle> circle = new CustomCircle;
				if(circleInfo->radius != NULL)
					circle->set(centerPoint,circleInfo->radius->GetValue(feature),
					circleInfo->edges->GetValue(feature));
				shapeFillDrawable = new GlobeShapeDrawable(circle);
				//shapeOutDrawable = new GlobeShapeDrawable(circle);
				DealDraw(circle,shapeFillDrawable,outlineNode,marker2DInfo,feature);
				geode->addDrawable(shapeFillDrawable);
				//geode->addDrawable(shapeOutDrawable);
			}
		}
		else if (marker2DInfo->shapeInfo->shapeType == GLB_MARKERSHAPE_RECTANGLE)
		{
			GlbRectangleInfo *rectangleInfo = 
				static_cast<GlbRectangleInfo *>(marker2DInfo->shapeInfo);
			if (rectangleInfo)
			{
				ref_ptr<CustomRectangle> rectangle = new CustomRectangle;
				if(rectangleInfo->width != NULL && rectangleInfo->height != NULL)
					rectangle->set(centerPoint,rectangleInfo->width->GetValue(feature),
					rectangleInfo->height->GetValue(feature));
				shapeFillDrawable = new GlobeShapeDrawable(rectangle);
				//shapeOutDrawable = new GlobeShapeDrawable(rectangle);
				DealDraw(rectangle,shapeFillDrawable,outlineNode,marker2DInfo,feature);
				geode->addDrawable(shapeFillDrawable);
				//geode->addDrawable(shapeOutDrawable);
			}
		}
		else if (marker2DInfo->shapeInfo->shapeType == GLB_MARKERSHAPE_ELLIPSE)
		{
			GlbEllipseInfo *ellipseInfo = 
				static_cast<GlbEllipseInfo *>(marker2DInfo->shapeInfo);
			if (ellipseInfo)
			{
				ref_ptr<CustomEllipse> ellipse = new CustomEllipse;
				if(ellipseInfo->xRadius != NULL && ellipseInfo->yRadius != NULL && ellipseInfo->edges != NULL)
					ellipse->set(centerPoint,ellipseInfo->xRadius->GetValue(feature),
					ellipseInfo->yRadius->GetValue(feature),ellipseInfo->edges->GetValue(feature));
				shapeFillDrawable = new GlobeShapeDrawable(ellipse);
				//shapeOutDrawable = new GlobeShapeDrawable(ellipse);
				DealDraw(ellipse,shapeFillDrawable,outlineNode,marker2DInfo,feature);
				geode->addDrawable(shapeFillDrawable);
				//geode->addDrawable(shapeOutDrawable);
			}
		}
		else if (marker2DInfo->shapeInfo->shapeType == GLB_MARKERSHAPE_ARC)
		{
			GlbArcInfo *arcInfo = 
				static_cast<GlbArcInfo *>(marker2DInfo->shapeInfo);
			if (arcInfo)
			{
				ref_ptr<CustomArc> arc = new CustomArc;
				if(arcInfo->isFan != NULL && arcInfo->xRadius != NULL && arcInfo->yRadius != NULL &&
					arcInfo->sAngle != NULL && arcInfo->eAngle && arcInfo->edges != NULL)
					arc->set(centerPoint,arcInfo->isFan->GetValue(feature),arcInfo->xRadius->GetValue(feature),
					arcInfo->yRadius->GetValue(feature),osg::DegreesToRadians(arcInfo->sAngle->GetValue(feature)),
					osg::DegreesToRadians(arcInfo->eAngle->GetValue(feature)),arcInfo->edges->GetValue(feature));
				shapeFillDrawable = new GlobeShapeDrawable(arc);
				//shapeOutDrawable = new GlobeShapeDrawable(arc);
				DealDraw(arc,shapeFillDrawable,outlineNode,marker2DInfo,feature);
				geode->addDrawable(shapeFillDrawable);
				//geode->addDrawable(shapeOutDrawable);
			}
		}

		if (outlineNode)
		{
			osg::ref_ptr<osg::StateSet> stateset = outlineNode->getOrCreateStateSet();
			osg::ref_ptr<osg::PolygonMode> polygonMode = new osg::PolygonMode();
			polygonMode->setMode( osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::LINE);
			stateset->setAttribute( polygonMode.get(), osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON );
			osg::ref_ptr<osg::PolygonOffset> poffset = new osg::PolygonOffset(-1,-1);
			stateset->setAttributeAndModes(poffset,osg::StateAttribute::ON);
		}
	}

	mpt_outline = DealOutline(osg::Vec2d(centerPosition.x(),centerPosition.y()),marker2DInfo,feature,obj->GetGlobe()->GetType());
	group->addChild(geode);
	group->addChild(outlineMt);
	return group;
}

void CGlbGlobeMarker2DShapeSymbol::FillDrawVertexArray(agg::path_storage &ps,IGlbGeometry *outline)
{
	CGlbLine *line = dynamic_cast<CGlbLine *>(outline);
	if(line == NULL)
		return;

	//glbDouble cx,cy;
	//CGlbExtent* outlineExt = outline->GetExtent();
	//outlineExt->GetCenter(&cx,&cy);
	//double len ;	

	//// 考虑经纬形变！！！
	//double meterlonByDegree = 1.0,meterlatByDegree = 1.0;
	//if(mpr_globeType == GLB_GLOBETYPE_GLOBE)
	//{// 球模式时，需要纠正变形，否则会出现画圆变成椭圆的现象
	//	// 纬度1度相当于距离多少米
	//	meterlatByDegree = osg::PI * g_ellipsoidModel->getRadiusPolar() / 180;
	//	double loncircle = cos(osg::DegreesToRadians(cy)) * g_ellipsoidModel->getRadiusEquator();
	//	// 经度1度相当于距离多少米
	//	meterlonByDegree =  osg::PI * loncircle / 180; 
	//}

	const glbDouble *points = line->GetPoints();
	glbInt32 count = line->GetCount();
	for (glbInt32 i = 0; i < count; ++i)
	{
		//不做矩阵变换，转换坐标用这个
		osg::Vec2d point(points[2 * i],points[2 * i + 1]);

		double new_x = (point.x() - tempExtent.GetLeft()) * (tempImageW / (tempExtent.GetRight() - tempExtent.GetLeft()));
		double new_y = (point.y() - tempExtent.GetBottom()) * (tempImageH / (tempExtent.GetTop() - tempExtent.GetBottom()));

		// 测试旋转30度
		//double xx = point.x() - cx;
		//double yy = point.y() - cy;
		//double theta = atan2(yy,xx);
		//len = sqrt(xx*xx + yy*yy);
		//double new_x = cx + len*cos(theta + agg::deg2rad(rotation));
		//double new_y = cy + len*sin(theta + agg::deg2rad(rotation)) * meterlonByDegree / meterlatByDegree;
		//new_x = (new_x - tempExtent.GetLeft()) * (tempImageW / (tempExtent.GetRight() - tempExtent.GetLeft()));
		//new_y = (new_y - tempExtent.GetBottom()) * (tempImageH / (tempExtent.GetTop() - tempExtent.GetBottom()));
		if(i == 0)
		{
			ps.move_to(new_x,new_y);
			continue;
		}
		ps.line_to(new_x,new_y);
	}
	ps.close_polygon();
}

typedef Coordinate PT;
geos::geom::Geometry* CGlbGlobeMarker2DShapeSymbol::Interection(CGlbLine *outline, CGlbExtent &ext)
{// 计算ext与geom的相交区域
	// CGlbLine  --->  Geometry(GEOS_LINESTRING/GEOS_MULTILINESTING)	
	if (outline==NULL) return NULL;
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

	// 2. 由outline生成Poly
	geos::geom::Geometry* otherPoly = NULL;
	geos::geom::LinearRing* shell = NULL;	
	glbInt32 ptCnt = outline->GetCount();
	const glbDouble* pts = outline->GetPoints();
	geos::geom::CoordinateSequence* cs2 = csf.create(ptCnt+1,2);
	for(glbInt32 k = 0; k < ptCnt; k++)
	{				
		cs2->setAt(PT(pts[2*k],pts[2*k+1],0),k);
	}
	// 首尾相连
	cs2->setAt(PT(pts[0],pts[1],0),ptCnt);
	shell = factory.createLinearRing(cs2);	
	otherPoly = factory.createPolygon(shell,NULL);	

	//3. 求交集
	geos::geom::Geometry* outGeom=NULL;
	if (otherPoly && rectPoly)
	{
		//outGeom = rectPoly->intersection(otherPoly);  -- 不能用这种，否则轮廓线显示会有问题!!2014.12.25  马林
		outGeom = otherPoly->intersection(rectPoly);
		if (outGeom)
		{
			geos::geom::GeometryTypeId typeId = outGeom->getGeometryTypeId();
			std::string type = outGeom->getGeometryType();			
		}
	}	

	//4. 清除创建的对象
	if (rectPoly) delete rectPoly;	
	if (otherPoly) delete otherPoly;
	return outGeom;
}

void CGlbGlobeMarker2DShapeSymbol::FillDrawVertexArray(agg::path_storage &ps,geos::geom::Polygon* poly)
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
	ps.close_polygon();
	
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

void CGlbGlobeMarker2DShapeSymbol::DealAggTexture(agg::rasterizer_scanline_aa<> &ras,agg::rendering_buffer rbuf,
													IGlbGeometry *geom,GlbMarker2DShapeSymbolInfo *markerInfo,
													CGlbFeature *feature,glbBool &isRenderTex,geos::geom::Polygon* poly,IGlbGeometry *outline)
{
	if (NULL == markerInfo)
		return;
	CGlbLine *noRotOutline = dynamic_cast<CGlbLine *>(geom);
	if (NULL == noRotOutline)
		return;

	GlbPolygon2DSymbolInfo* fillInfo = markerInfo->fillInfo;
	if (NULL == fillInfo)
		return;

	CGlbWString filePath = fillInfo->textureData->GetValue(feature);
	if (filePath.empty())
		return;	

	
	GlbGlobeTexRepeatModeEnum repeatMode = 
		(GlbGlobeTexRepeatModeEnum)fillInfo->texRepeatMode->GetValue(feature);
	glbFloat tilingU = 1,tilingV = 1;
	if (repeatMode==GLB_TEXTUREREPEAT_TIMES)
	{
		if (fillInfo->tilingU)
			tilingU = fillInfo->tilingU->GetValue(feature);
		if(fillInfo->tilingV)
			tilingV = fillInfo->tilingV->GetValue(feature);
	}
	else
	{
		glbInt32 tilingU_size, tilingV_size;
		if (fillInfo->tilingU)
			tilingU_size = fillInfo->tilingU->GetValue(feature);
		if(fillInfo->tilingV)
			tilingV_size = fillInfo->tilingV->GetValue(feature);

		cglbOrginExtent = const_cast<CGlbExtent*>(noRotOutline->GetExtent());

		tilingU = cglbOrginExtent->GetXWidth() / tilingU_size;
		tilingV = cglbOrginExtent->GetYHeight() / tilingV_size;
	}

	if (tilingU==1 && tilingV==1)
	{// 非重复纹理
		DealAggWrapTexture(ras,rbuf,markerInfo,feature,isRenderTex,poly,noRotOutline);	
	}
	else if (tilingU>=1 && tilingV>=1)
	{
		DealAggRepeatTexture(ras,rbuf,geom,markerInfo,feature,isRenderTex,tilingU,tilingV,poly,noRotOutline);
	}
}

void CGlbGlobeMarker2DShapeSymbol::DealAggWrapTexture(agg::rasterizer_scanline_aa<> &ras,
														agg::rendering_buffer rbuf,GlbMarker2DShapeSymbolInfo *markerInfo,
														CGlbFeature *feature,glbBool &isRenderTex,geos::geom::Polygon* poly,IGlbGeometry *outline)
{
	GlbPolygon2DSymbolInfo* fillInfo = markerInfo->fillInfo;
	osg::ref_ptr<osg::Image> image = NULL;
	CGlbWString filePath = L"";
	if(fillInfo && fillInfo->textureData)
	{
		filePath = fillInfo->textureData->GetValue(feature);
		glbInt32 index = filePath.find_first_of(L'.');
		if(index == 0)
		{
			CGlbWString execDir = CGlbPath::GetExecDir();
			filePath = execDir + filePath.substr(1,filePath.size());
		}
		image = osgDB::readImageFile(filePath.ToString());		
	}

	if(image == NULL)
		return;	

	if (poly==NULL) return;

	double rotation = 0.0;
	if(fillInfo->textureRotation)
		rotation = fillInfo->textureRotation->GetValue(feature);	

	cglbExtent = const_cast<CGlbExtent*>(outline->GetExtent());

	// 需要纹理渲染
	isRenderTex = true;	
	double yaw = 0.0;
	if (markerInfo->yaw)
		yaw = markerInfo->yaw->GetValue(feature);

	agg::scanline_p8 sl;
	glbInt32 imageOpacity = 100;//图片透明度
	if(fillInfo->opacity)
		imageOpacity = fillInfo->opacity->GetValue(feature);

	agg::rendering_buffer rbuf_img(NULL,0,0,0);
	GLenum dataType = (GLenum)image->getPixelFormat();

	int stride = 0,s = image->s(),t = image->t();
	switch(dataType)
	{
	case GL_RGB: stride = s * 3;break;
	case GL_RGBA: stride = s * 4;break;
	default:break;
	}

	unsigned char *data = image->data();
	if(dataType == GL_RGBA)
	{
		static osg::ref_ptr<osg::Image> recImage = dynamic_cast<osg::Image *>(image->clone(osg::CopyOp::DEEP_COPY_IMAGES));
		static CGlbWString cFilePath = filePath;
		if(cFilePath != filePath)
		{
			recImage = dynamic_cast<osg::Image *>(image->clone(osg::CopyOp::DEEP_COPY_IMAGES));
			cFilePath = filePath;
		}
		if(NULL == recImage)
			return;
		unsigned char *recData = recImage->data();
		for(glbInt32 i = 0; i < image->t();i++)
			for(glbInt32 j = 0; j < image->s();j++)
			{
				unsigned char* a = recData + ( i * image->s() + j) * 4 + 3;
				data[(i * image->s() + j) * 4 + 3] = *a * imageOpacity  / 100.0;
			}
	}

	rbuf_img = agg::rendering_buffer(data,s,t,stride);

	agg::path_storage canvas;
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
			canvas.move_to(new_x,new_y);
			continue;
		}
		canvas.line_to(new_x,new_y);		
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
				canvas.move_to(new_x,new_y);
				continue;
			}
			canvas.line_to(new_x,new_y);		
		}
	}			

	agg::trans_affine imageMatrix;
	// 1. 首先实现旋转
	imageMatrix.multiply(agg::trans_affine_translation(-image->s()*0.5,-image->t()*0.5));
	imageMatrix.multiply(agg::trans_affine_rotation(agg::deg2rad(rotation+yaw)));
	imageMatrix.multiply(agg::trans_affine_translation(image->s()*0.5,image->t()*0.5));

	// 2. 图像缩放
	double imgscalex = double(tempImageW) / image->s() * (cglbExtent->GetXWidth() / tempExtent.GetXWidth());
	double imgscaley = double(tempImageH) / image->t() * (cglbExtent->GetYHeight() / tempExtent.GetYHeight());	
	imageMatrix.multiply(agg::trans_affine_scaling(imgscalex ,imgscaley));

	// 3. 先把图像中心点偏移到(cglbExtent->GetLeft(),cglbExtent->GetBottom())处,以实现中心旋转
	double new_basex = cglbExtent->GetLeft(); 
	double new_basey = cglbExtent->GetBottom();
	double transx_first = (new_basex-tempExtent.GetLeft()) * tempImageW / tempExtent.GetXWidth();
	double transy_first = (new_basey-tempExtent.GetBottom()) * tempImageH / tempExtent.GetYHeight();
	imageMatrix.multiply(agg::trans_affine_translation(transx_first,transy_first));
	// 矩阵取反
	imageMatrix.invert();

	if(dataType == GL_RGB)
	{
		agg::int8u *newData = new agg::int8u[t * s * 4];
		agg::rendering_buffer newBuffer(newData,s,t,s * 4);
		agg::color_conv(&newBuffer,&rbuf_img,agg::color_conv_rgb24_to_rgba32());
		rbuf_img = newBuffer;
	}

	if(dataType == GL_RGB)
	{
		unsigned char * rbufData = rbuf_img.buf();
		for(int i = 0; i < t;i++)
		{
			for(int j =0; j < s; j++)
			{
				rbufData[(i * s + j) * 4 + 3] *= imageOpacity  / 100.0;
			}
		}
	}

	typedef agg::renderer_base<agg::pixfmt_rgba32> RendererBaseType;
	agg::pixfmt_rgba32 pixelFormat(rbuf);
	RendererBaseType rendererBase(pixelFormat);

	typedef agg::span_allocator<RendererBaseType::color_type> AllocatorType;
	AllocatorType allocator;

	agg::pixfmt_rgba32 pixelFormatOfData( rbuf_img );
	typedef agg::span_interpolator_linear<> InterpolatorType;
	InterpolatorType interpolator(imageMatrix);
	typedef agg::image_accessor_clone<agg::pixfmt_rgba32> ImageAccessorType;
	ImageAccessorType imageAccessor( pixelFormatOfData );
	typedef agg::image_accessor_clone<agg::pixfmt_rgba32> RGBAImageAccessorType;
	typedef agg::span_image_filter_rgba_bilinear<RGBAImageAccessorType, InterpolatorType> RGBASpanType;
	RGBASpanType generator(imageAccessor,interpolator);

	agg::rasterizer_scanline_aa<> rasterizer;
	rasterizer.reset();
	agg::conv_contour<agg::path_storage> converter(canvas);
	rasterizer.add_path(converter);
	agg::scanline_p8 scanline;
	agg::render_scanlines_aa(rasterizer,scanline,rendererBase,allocator,generator);

	unsigned char * rbufData = rbuf_img.buf();
	if (rbufData)
	{
		delete [] rbufData;
	}
}

void CGlbGlobeMarker2DShapeSymbol::DealAggRepeatTexture(agg::rasterizer_scanline_aa<> &ras,
						agg::rendering_buffer rbuf,IGlbGeometry *geom,GlbMarker2DShapeSymbolInfo *markerInfo,
						CGlbFeature *feature,glbBool &isRenderTex,glbFloat tilingU,glbFloat tilingV,geos::geom::Polygon* poly,IGlbGeometry *outline)
{
	GlbPolygon2DSymbolInfo* fillInfo = markerInfo->fillInfo;
	osg::ref_ptr<osg::Image> image = NULL;
	CGlbWString filePath = L"";
	if(fillInfo && fillInfo->textureData)
	{
		filePath = fillInfo->textureData->GetValue(feature);
		glbInt32 index = filePath.find_first_of(L'.');
		if(index == 0)
		{
			CGlbWString execDir = CGlbPath::GetExecDir();
			filePath = execDir + filePath.substr(1,filePath.size());
		}
		image = osgDB::readImageFile(filePath.ToString());
	}

	if (image == NULL)
		return;	

	if (poly == NULL) return;

	double rotation = 0.0;
	if(fillInfo->textureRotation)
		rotation = fillInfo->textureRotation->GetValue(feature);	

	cglbExtent = const_cast<CGlbExtent*>(outline->GetExtent());
	double cx,cy;
	cglbExtent->GetCenter(&cx,&cy);

	// 需要纹理渲染
	isRenderTex = true;	
	double yaw = 0.0;
	if (markerInfo->yaw)
		yaw = markerInfo->yaw->GetValue(feature);

	glbInt32 imageOpacity = 100;//图片透明度
	if(fillInfo->opacity)
		imageOpacity = fillInfo->opacity->GetValue(feature);
	agg::rendering_buffer rbuf_img(NULL,0,0,0);
	GLenum dataType = (GLenum)image->getPixelFormat();

	int stride = 0,s = image->s(),t = image->t();
	switch(dataType)
	{
	case GL_RGB: stride = s * 3;break;
	case GL_RGBA: stride = s * 4;break;
	default:break;
	}

	unsigned char *data = image->data();

	//处理32位设置透明度
	if(dataType == GL_RGBA)
	{
		static osg::ref_ptr<osg::Image> recImage = dynamic_cast<osg::Image *>(image->clone(osg::CopyOp::DEEP_COPY_IMAGES));
		static CGlbWString cFilePath = filePath;
		if(cFilePath != filePath)
		{
			recImage = dynamic_cast<osg::Image *>(image->clone(osg::CopyOp::DEEP_COPY_IMAGES));
			cFilePath = filePath;
		}
		if(NULL == recImage)
			return;
		unsigned char *recData = recImage->data();
		for(glbInt32 i = 0; i < image->t();i++)
			for(glbInt32 j = 0; j < image->s();j++)
			{
				unsigned char* a = recData + ( i * image->s() + j) * 4 + 3;
				data[(i * image->s() + j) * 4 + 3] = *a * imageOpacity  / 100.0;
			}
	}

	rbuf_img = agg::rendering_buffer(data,s,t,stride);

	agg::path_storage canvas;
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
			canvas.move_to(new_x,new_y);
			continue;
		}
		canvas.line_to(new_x,new_y);		
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
				canvas.move_to(new_x,new_y);
				continue;
			}
			canvas.line_to(new_x,new_y);		
		}
	}			

	agg::trans_affine imageMatrix;
	// 1. 首先实现旋转
	imageMatrix.multiply(agg::trans_affine_translation(-image->s()*0.5,-image->t()*0.5));
	imageMatrix.multiply(agg::trans_affine_rotation(agg::deg2rad(rotation+yaw)));
	imageMatrix.multiply(agg::trans_affine_translation(image->s()*0.5,image->t()*0.5));

	// 2. 图像缩放
	double imgscalex = double(tempImageW) / image->s() * (cglbExtent->GetXWidth() / tempExtent.GetXWidth());
	double imgscaley = double(tempImageH) / image->t() * (cglbExtent->GetYHeight() / tempExtent.GetYHeight());	
	imageMatrix.multiply(agg::trans_affine_scaling(imgscalex / tilingU ,imgscaley / tilingV));

	// 3. 先把图像中心点偏移到(cglbExtent->GetLeft(),cglbExtent->GetBottom())处,以实现中心旋转
	double new_basex = cglbExtent->GetLeft(); 
	double new_basey = cglbExtent->GetBottom();
	double transx_first = (new_basex-tempExtent.GetLeft()) * tempImageW / tempExtent.GetXWidth();
	double transy_first = (new_basey-tempExtent.GetBottom()) * tempImageH / tempExtent.GetYHeight();
	imageMatrix.multiply(agg::trans_affine_translation(transx_first,transy_first));
	// 矩阵取反
	imageMatrix.invert();

	if(dataType == GL_RGB)
	{
		agg::int8u *newData = new agg::int8u[t * s * 4];
		agg::rendering_buffer newBuffer(newData,s,t,s * 4);
		agg::color_conv(&newBuffer,&rbuf_img,agg::color_conv_rgb24_to_rgba32());
		rbuf_img = newBuffer;
	}

	if(dataType == GL_RGB)
	{
		unsigned char * rbufData = rbuf_img.buf();
		for(int i = 0; i < t;i++)
		{
			for(int j =0; j < s; j++)
			{
				rbufData[(i * s + j) * 4 + 3] *= imageOpacity  / 100.0;
			}
		}
	}

	typedef agg::renderer_base<agg::pixfmt_rgba32> RendererBaseType;
	agg::pixfmt_rgba32 pixelFormat(rbuf);
	RendererBaseType rendererBase(pixelFormat);

	typedef agg::span_allocator<RendererBaseType::color_type> AllocatorType;
	AllocatorType allocator;

	agg::pixfmt_rgba32 pixelFormatOfData( rbuf_img );
	typedef agg::span_interpolator_linear<> InterpolatorType;
	InterpolatorType interpolator(imageMatrix);
	//typedef agg::image_accessor_clone<agg::pixfmt_rgba32> ImageAccessorType;
	typedef agg::image_accessor_wrap<agg::pixfmt_rgba32,agg::wrap_mode_repeat,agg::wrap_mode_repeat> ImageAccessorType;
	ImageAccessorType imageAccessor( pixelFormatOfData );
	typedef agg::image_accessor_wrap<agg::pixfmt_rgba32,agg::wrap_mode_repeat,agg::wrap_mode_repeat> RGBAImageAccessorType;
	typedef agg::span_image_filter_rgba_bilinear<RGBAImageAccessorType, InterpolatorType> RGBASpanType;
	RGBASpanType generator(imageAccessor,interpolator);

	agg::rasterizer_scanline_aa<> rasterizer;
	rasterizer.reset();
	agg::conv_contour<agg::path_storage> converter(canvas);
	rasterizer.add_path(converter);
	agg::scanline_p8 scanline;
	agg::render_scanlines_aa(rasterizer,scanline,rendererBase,allocator,generator);

	unsigned char * rbufData = rbuf_img.buf();
	if (rbufData)
	{
		delete [] rbufData;
	}
}

void CGlbGlobeMarker2DShapeSymbol::DealAggRender( agg::rendering_buffer rbuf,renb_type &renb,CGlbGlobeRObject *obj,
															glbInt32 imageW,glbInt32 imageH,CGlbExtent &ext,IGlbGeometry* outline)
{
	agg::path_storage ps;
	agg::trans_affine mtx;

	tempExtent = ext;
	tempImageW = imageW;
	tempImageH = imageH;

	typedef agg::conv_transform<agg::path_storage> ell_ct_type;
	ell_ct_type ctell(ps,mtx);//矩阵变换

	typedef agg::conv_contour<ell_ct_type> ell_cc_type;
	ell_cc_type ccell(ctell);

	typedef agg::conv_dash<ell_cc_type> ell_cd_type; 
	ell_cd_type cdccell(ccell);

	GlbRenderInfo *renderInfo = obj->GetRenderInfo();
	if(renderInfo == NULL)
		return;

	GlbMarker2DShapeSymbolInfo *marker2DInfo = 
		static_cast<GlbMarker2DShapeSymbolInfo *>(renderInfo);
	CGlbFeature *feature = obj->GetFeature();		

	if(marker2DInfo->fillInfo->outlineInfo->linePattern->GetValue(feature) == GLB_LINE_SOLID)
		cdccell.add_dash(1,0);
	else if(marker2DInfo->fillInfo->outlineInfo->linePattern->GetValue(feature) == GLB_LINE_DOTTED)
		cdccell.add_dash(5,5);

	typedef agg::conv_stroke<ell_cd_type> ell_ct_cs_type;
	ell_ct_cs_type csell(cdccell);

	agg::rasterizer_scanline_aa<> ras;//处理纹理
	agg::rasterizer_scanline_aa<> rasOutline;
	agg::rasterizer_scanline_aa<> rasFillpolygon;
	agg::scanline_p8 sl;

	ras.add_path(ccell);
	glbBool isRenderTex = false;	

	glbDouble area = ext.GetXWidth()*ext.GetYHeight();
	CGlbExtent expandExt;// 扩大0.05倍的ext范围
	osg::Vec3d _center;
	ext.GetCenter(&_center.x(),&_center.y(),&_center.z());
	expandExt.SetMin(_center.x()-ext.GetXWidth()*0.52,_center.y()-ext.GetYHeight()*0.52);
	expandExt.SetMax(_center.x()+ext.GetXWidth()*0.52,_center.y()+ext.GetYHeight()*0.52);

	// 矢量绘制到renb
	glbref_ptr<CGlbMultiLine> multiline = dynamic_cast<CGlbMultiLine *>(outline);
	glbref_ptr<CGlbLine> line = dynamic_cast<CGlbLine *>(outline);

	CGlbMultiLine *noRotMultiline = dynamic_cast<CGlbMultiLine *>(mpr_noRotOutline.get());
	glbref_ptr<CGlbLine> noRotline = dynamic_cast<CGlbLine *>(mpr_noRotOutline.get());
	
	if(line && noRotline)
	{
		CGlbExtent* lineExt = const_cast<CGlbExtent*>(line->GetExtent());
		// 注意此处不能直接用lineExt->Merge(ext)因为这样会改变line的外包的！！2014.4.17 马林
		CGlbExtent mergeExt;
		mergeExt.Merge(*lineExt);
		mergeExt.Merge(ext);		
		glbDouble area1 = mergeExt.GetXWidth()*mergeExt.GetYHeight();
		if (area1 / area > OverlayFazhi)
		{// 当要绘制的对象范围的面积是ext范围面积的2倍以上时，需要做overlay计算以加速栅格化
			geos::geom::Geometry* outGeom = Interection(line.get(),expandExt);
			if (outGeom)
			{
				geos::geom::GeometryTypeId typeId = outGeom->getGeometryTypeId();
				switch(typeId)
				{
				case GEOS_POLYGON:
					{
						geos::geom::Polygon* poly = dynamic_cast<geos::geom::Polygon*>(outGeom);
						// 图像绘制到renb
						DealAggTexture(ras,rbuf,noRotline.get(),marker2DInfo,feature,isRenderTex,poly,line.get());
						// 矢量绘制到renb
						FillDrawVertexArray(ps,poly);						
					}
					break;
				case GEOS_MULTIPOLYGON:
					{
						geos::geom::MultiPolygon* multiPoly = dynamic_cast<geos::geom::MultiPolygon*>(outGeom);
						size_t numPoly = multiPoly->getNumGeometries();
						for (size_t k = 0; k < numPoly; k++)
						{
							geos::geom::Geometry* geo = (geos::geom::Geometry*)(multiPoly->getGeometryN(k));
							geos::geom::Polygon* poly = dynamic_cast<geos::geom::Polygon*>(geo);
							// 图像绘制到renb
							DealAggTexture(ras,rbuf,noRotline.get(),marker2DInfo,feature,isRenderTex,poly,line.get());
							// 矢量绘制到renb
							FillDrawVertexArray(ps,poly);							
						}
					}
					break;
				}
				// 删除geometry
				delete outGeom;
			}
		}
		else
		{
			// 图像绘制到renb
			DealAggTexture(ras,rbuf,noRotline.get(),marker2DInfo,feature,isRenderTex,line.get());
			// 矢量绘制到renb
			FillDrawVertexArray(ps,line.get());
		}	
	}
	else if(multiline && noRotMultiline)
	{
		for(glbInt32 i = 0; i < multiline->GetCount(); i++)
		{
			line = const_cast<CGlbLine*>(multiline->GetLine(i));
			noRotline = const_cast<CGlbLine*>(noRotMultiline->GetLine(i));

			CGlbExtent* lineExt = const_cast<CGlbExtent*>(line->GetExtent());
			// 注意此处不能直接用lineExt->Merge(ext)因为这样会改变line的外包的！！2014.4.17 马林
			CGlbExtent mergeExt;
			mergeExt.Merge(*lineExt);
			mergeExt.Merge(ext);		
			glbDouble area1 = mergeExt.GetXWidth()*mergeExt.GetYHeight();			
			if (area1 / area > OverlayFazhi)
			{// 当要绘制的对象范围的面积是ext范围面积的2倍以上时，需要做overlay计算以加速栅格化
				geos::geom::Geometry* outGeom = Interection(line.get(),expandExt);
				if (outGeom)
				{
					geos::geom::GeometryTypeId typeId = outGeom->getGeometryTypeId();
					switch(typeId)
					{
					case GEOS_POLYGON:
						{
							geos::geom::Polygon* poly = dynamic_cast<geos::geom::Polygon*>(outGeom);
							// 图像绘制到renb
							DealAggTexture(ras,rbuf,noRotline.get(),marker2DInfo,feature,isRenderTex,poly,line.get());
							FillDrawVertexArray(ps,poly);							
						}
						break;
					case GEOS_MULTIPOLYGON:
						{
							geos::geom::MultiPolygon* multiPoly = dynamic_cast<geos::geom::MultiPolygon*>(outGeom);
							size_t numPoly = multiPoly->getNumGeometries();
							for (size_t k = 0; k < numPoly; k++)
							{
								geos::geom::Geometry* geo = (geos::geom::Geometry*)(multiPoly->getGeometryN(k));
								geos::geom::Polygon* poly = dynamic_cast<geos::geom::Polygon*>(geo);
								// 图像绘制到renb
								DealAggTexture(ras,rbuf,noRotline.get(),marker2DInfo,feature,isRenderTex,poly,line.get());
								FillDrawVertexArray(ps,poly);								
							}
						}
						break;
					}
					// 删除geometry
					delete outGeom;
				}
			}
			else
			{
				// 图像绘制到renb
				DealAggTexture(ras,rbuf,noRotline.get(),marker2DInfo,feature,isRenderTex,line.get());
				// 矢量绘制到renb
				FillDrawVertexArray(ps,line.get());
			}
		}
	}

	if(marker2DInfo->fillInfo && marker2DInfo->fillInfo->outlineInfo && marker2DInfo->fillInfo->outlineInfo->lineWidth)
		csell.width(marker2DInfo->fillInfo->outlineInfo->lineWidth->GetValue(feature)/* * (ext.GetRight() - ext.GetLeft())  /  imageW*/);//线宽
	rasFillpolygon.add_path(ccell);
	rasOutline.add_path(csell);
	glbInt32 lineColor = 0xFFFFFFFF,polygonColor = 0xFFFFFFFF;
	agg::rgba8 rgbaLine(255,255,255,255),rgbaPolygon(255,255,255,255);
	
	if(marker2DInfo->fillInfo && marker2DInfo->fillInfo->outlineInfo && marker2DInfo->fillInfo->outlineInfo->lineColor)
		lineColor = marker2DInfo->fillInfo->outlineInfo->lineColor->GetValue(feature);
	if(marker2DInfo->fillInfo && marker2DInfo->fillInfo->outlineInfo && marker2DInfo->fillInfo->outlineInfo->lineOpacity)
	{
		//rgbaLine = agg::rgba8(GetRValue(lineColor),GetGValue(lineColor),GetBValue(lineColor),
		//marker2DInfo->fillInfo->outlineInfo->lineOpacity->GetValue(feature) * 255 / 100.0);//设置线的颜色【透明度】
		rgbaLine = agg::rgba8(GetBValue(lineColor),LOBYTE(((lineColor & 0xFFFF)) >> 8),GetRValue(lineColor),
		marker2DInfo->fillInfo->outlineInfo->lineOpacity->GetValue(feature) * 255 / 100.0);//设置线的颜色【透明度】
	}
	else if(marker2DInfo->fillInfo && marker2DInfo->fillInfo->outlineInfo && marker2DInfo->fillInfo->outlineInfo->lineOpacity == NULL)
	{
		//rgbaLine = agg::rgba8(GetRValue(lineColor),GetGValue(lineColor),GetBValue(lineColor),GetAValue(lineColor));
		rgbaLine = agg::rgba8(GetBValue(lineColor),LOBYTE(((lineColor & 0xFFFF)) >> 8),GetRValue(lineColor),GetAValue(lineColor));
	}
	
	if(marker2DInfo->fillInfo && marker2DInfo->fillInfo->color)
		polygonColor = marker2DInfo->fillInfo->color->GetValue(feature);

	rgbaPolygon = agg::rgba8(GetBValue(polygonColor),LOBYTE(((polygonColor & 0xFFFF)) >> 8),GetRValue(polygonColor),GetAValue(polygonColor));
	if(marker2DInfo->fillInfo && marker2DInfo->fillInfo->opacity)
	{
		if(isRenderTex)
			rgbaPolygon = agg::rgba8(GetBValue(polygonColor),LOBYTE(((polygonColor & 0xFFFF)) >> 8),GetRValue(polygonColor),
			marker2DInfo->fillInfo->opacity->GetValue(feature) * 255 / 100.0 * 0.5);
		else
			rgbaPolygon = agg::rgba8(GetBValue(polygonColor),LOBYTE(((polygonColor & 0xFFFF)) >> 8),GetRValue(polygonColor),
			marker2DInfo->fillInfo->opacity->GetValue(feature) * 255 / 100.0);
	}	
	
	agg::render_scanlines_aa_solid(rasOutline,sl,renb,rgbaLine);

	if(obj->IsSelected())
	{
		agg::render_scanlines_aa_solid(rasFillpolygon,sl,renb,agg::rgba8(255,0,0,128));
		return;
	}

	if(polygonColor == 0xFFFFFFFF && isRenderTex == true)
		return;
	agg::render_scanlines_aa_solid(rasFillpolygon,sl,renb,rgbaPolygon);	
}

void CGlbGlobeMarker2DShapeSymbol::DealAggTexture( agg::rasterizer_scanline_aa<> &ras, agg::rendering_buffer rbuf, IGlbGeometry *geom,
																GlbMarker2DShapeSymbolInfo *markerInfo, CGlbFeature *feature,glbBool &isRenderTex,IGlbGeometry *outline)
{
	CGlbLine *noRotOutline = dynamic_cast<CGlbLine *>(geom);
	if (NULL == noRotOutline || NULL == markerInfo)
		return;

	GlbPolygon2DSymbolInfo* fillInfo = markerInfo->fillInfo;
	if(fillInfo == NULL)
		return;

	CGlbWString filePath = fillInfo->textureData->GetValue(feature);
	if (filePath.empty())
		return;	

	GlbGlobeTexRepeatModeEnum repeatMode = 
		(GlbGlobeTexRepeatModeEnum)fillInfo->texRepeatMode->GetValue(feature);
	glbFloat tilingU = 1,tilingV = 1;
	if (repeatMode==GLB_TEXTUREREPEAT_TIMES)
	{
		if (fillInfo->tilingU)
			tilingU = fillInfo->tilingU->GetValue(feature);
		if(fillInfo->tilingV)
			tilingV = fillInfo->tilingV->GetValue(feature);
	}
	else
	{
		glbInt32 tilingU_size, tilingV_size;
		if (fillInfo->tilingU)
			tilingU_size = fillInfo->tilingU->GetValue(feature);
		if(fillInfo->tilingV)
			tilingV_size = fillInfo->tilingV->GetValue(feature);

		cglbOrginExtent = const_cast<CGlbExtent*>(noRotOutline->GetExtent());

		tilingU = cglbOrginExtent->GetXWidth() / tilingU_size;
		tilingV = cglbOrginExtent->GetYHeight() / tilingV_size;
	}

	if (tilingU==1 && tilingV==1)
	{// 非重复纹理
		DealAggWrapTexture(ras,rbuf,markerInfo,feature,isRenderTex,outline);	
		//DealAggWrapTexture(ras,rbuf,markerInfo,feature,isRenderTex,noRotOutline);	
	}
	else if (tilingU>=1 && tilingV>=1)
	{
		DealAggRepeatTexture(ras,rbuf,geom,markerInfo,feature,isRenderTex,tilingU,tilingV,outline);
		//DealAggRepeatTexture(ras,rbuf,geom,markerInfo,feature,isRenderTex,tilingU,tilingV,noRotOutline);
	}
}


void CGlbGlobeMarker2DShapeSymbol::DealAggRepeatTexture(agg::rasterizer_scanline_aa<> &ras,agg::rendering_buffer rbuf,
											IGlbGeometry *geom,GlbMarker2DShapeSymbolInfo *markerInfo,
											CGlbFeature *feature,glbBool &isRenderTex,glbFloat tilingU,glbFloat tilingV,IGlbGeometry *outline)
{
	GlbPolygon2DSymbolInfo* fillInfo = markerInfo->fillInfo;
	osg::ref_ptr<osg::Image> image = NULL;
	CGlbWString filePath = L"";
	if(fillInfo && fillInfo->textureData)
	{
		filePath = fillInfo->textureData->GetValue(feature);
		glbInt32 index = filePath.find_first_of(L'.');
		if(index == 0)
		{
			CGlbWString execDir = CGlbPath::GetExecDir();
			filePath = execDir + filePath.substr(1,filePath.size());
		}
		image = osgDB::readImageFile(filePath.ToString());
	}

	if(image == NULL)
		return;	

	double rotation = 0.0;
	if(fillInfo->textureRotation)
		rotation = fillInfo->textureRotation->GetValue(feature);	

	cglbExtent = const_cast<CGlbExtent*>(outline->GetExtent());
	double cx,cy;
	cglbExtent->GetCenter(&cx,&cy);

	// 需要纹理渲染
	isRenderTex = true;	
	double yaw = 0.0;
	if (markerInfo->yaw)
		yaw = markerInfo->yaw->GetValue(feature);

	glbInt32 imageOpacity = 100;//图片透明度
	if(fillInfo->opacity)
		imageOpacity = fillInfo->opacity->GetValue(feature);
	agg::rendering_buffer rbuf_img(NULL,0,0,0);
	GLenum dataType = (GLenum)image->getPixelFormat();

	int stride = 0,s = image->s(),t = image->t();
	switch(dataType)
	{
	case GL_RGB: stride = s * 3;break;
	case GL_RGBA: stride = s * 4;break;
	default:break;
	}

	unsigned char *data = image->data();

	//处理32位设置透明度
	if(dataType == GL_RGBA)
	{
		static osg::ref_ptr<osg::Image> recImage = dynamic_cast<osg::Image *>(image->clone(osg::CopyOp::DEEP_COPY_IMAGES));
		static CGlbWString cFilePath = filePath;
		if(cFilePath != filePath)
		{
			recImage = dynamic_cast<osg::Image *>(image->clone(osg::CopyOp::DEEP_COPY_IMAGES));
			cFilePath = filePath;
		}
		if(NULL == recImage)
			return;
		unsigned char *recData = recImage->data();
		for(glbInt32 i = 0; i < image->t();i++)
			for(glbInt32 j = 0; j < image->s();j++)
			{
				unsigned char* a = recData + ( i * image->s() + j) * 4 + 3;
				data[(i * image->s() + j) * 4 + 3] = *a * imageOpacity  / 100.0;
			}
	}

	rbuf_img = agg::rendering_buffer(data,s,t,stride);

	agg::path_storage canvas;
	CGlbLine *line = dynamic_cast<CGlbLine *>(outline); //(noROutline.get()) //outline
	if(line == NULL)
		return;

	const glbDouble *points = line->GetPoints();
	glbInt32 count = line->GetCount();
	for (glbInt32 i = 0; i <count; i++)
	{
		//不做矩阵变换，转换坐标用这个
		osg::Vec2d point(points[2 * i],points[2 * i + 1]);		

		double new_x = (point.x() - tempExtent.GetLeft()) * (tempImageW / (tempExtent.GetRight() - tempExtent.GetLeft()));
		double new_y = (point.y() - tempExtent.GetBottom()) * (tempImageH / (tempExtent.GetTop() - tempExtent.GetBottom()));			

		if(i == 0)
		{
			canvas.move_to(new_x,new_y);
			continue;
		}
		canvas.line_to(new_x,new_y);		
	}
	canvas.close_polygon();

	agg::trans_affine imageMatrix;
	// 1. 首先实现旋转
	imageMatrix.multiply(agg::trans_affine_translation(-image->s()*0.5,-image->t()*0.5));
	imageMatrix.multiply(agg::trans_affine_rotation(agg::deg2rad(rotation+yaw)));
	imageMatrix.multiply(agg::trans_affine_translation(image->s()*0.5,image->t()*0.5));

	// 2. 图像缩放
	double imgscalex = double(tempImageW) / image->s() * (cglbExtent->GetXWidth() / tempExtent.GetXWidth());
	double imgscaley = double(tempImageH) / image->t() * (cglbExtent->GetYHeight() / tempExtent.GetYHeight());	
	imageMatrix.multiply(agg::trans_affine_scaling(imgscalex / tilingU ,imgscaley / tilingV));

	// 3. 先把图像中心点偏移到(cglbExtent->GetLeft(),cglbExtent->GetBottom())处,以实现中心旋转
	double new_basex = cglbExtent->GetLeft(); 
	double new_basey = cglbExtent->GetBottom();
	double transx_first = (new_basex-tempExtent.GetLeft()) * tempImageW / tempExtent.GetXWidth();
	double transy_first = (new_basey-tempExtent.GetBottom()) * tempImageH / tempExtent.GetYHeight();
	imageMatrix.multiply(agg::trans_affine_translation(transx_first,transy_first));
	// 矩阵取反
	imageMatrix.invert();

	if(dataType == GL_RGB)
	{
		agg::int8u *newData = new agg::int8u[t * s * 4];
		agg::rendering_buffer newBuffer(newData,s,t,s * 4);
		agg::color_conv(&newBuffer,&rbuf_img,agg::color_conv_rgb24_to_rgba32());
		rbuf_img = newBuffer;
	}

	if(dataType == GL_RGB)
	{
		unsigned char * rbufData = rbuf_img.buf();
		for(int i = 0; i < t;i++)
		{
			for(int j =0; j < s; j++)
			{
				rbufData[(i * s + j) * 4 + 3] *= imageOpacity  / 100.0;
			}
		}
	}

	typedef agg::renderer_base<agg::pixfmt_rgba32> RendererBaseType;
	agg::pixfmt_rgba32 pixelFormat(rbuf);
	RendererBaseType rendererBase(pixelFormat);

	typedef agg::span_allocator<RendererBaseType::color_type> AllocatorType;
	AllocatorType allocator;

	agg::pixfmt_rgba32 pixelFormatOfData( rbuf_img );
	typedef agg::span_interpolator_linear<> InterpolatorType;
	InterpolatorType interpolator(imageMatrix);
	//typedef agg::image_accessor_clone<agg::pixfmt_rgba32> ImageAccessorType;
	typedef agg::image_accessor_wrap<agg::pixfmt_rgba32,agg::wrap_mode_repeat,agg::wrap_mode_repeat> ImageAccessorType;
	ImageAccessorType imageAccessor( pixelFormatOfData );
	typedef agg::image_accessor_wrap<agg::pixfmt_rgba32,agg::wrap_mode_repeat,agg::wrap_mode_repeat> RGBAImageAccessorType;
	typedef agg::span_image_filter_rgba_bilinear<RGBAImageAccessorType, InterpolatorType> RGBASpanType;
	RGBASpanType generator(imageAccessor,interpolator);

	agg::rasterizer_scanline_aa<> rasterizer;
	rasterizer.reset();
	agg::conv_contour<agg::path_storage> converter(canvas);
	rasterizer.add_path(converter);
	agg::scanline_p8 scanline;
	agg::render_scanlines_aa(rasterizer,scanline,rendererBase,allocator,generator);

	unsigned char * rbufData = rbuf_img.buf();
	if (rbufData)
	{
		delete [] rbufData;
	}
}

void CGlbGlobeMarker2DShapeSymbol::DealAggWrapTexture(agg::rasterizer_scanline_aa<> &ras,agg::rendering_buffer rbuf,
											GlbMarker2DShapeSymbolInfo *markerInfo,CGlbFeature *feature,glbBool &isRenderTex,IGlbGeometry *outline)
{
	GlbPolygon2DSymbolInfo* fillInfo = markerInfo->fillInfo;
	if(fillInfo == NULL)
		return;
	
	osg::ref_ptr<osg::Image> image = NULL;
	CGlbWString filePath = L"";
	if(fillInfo && fillInfo->textureData)
	{
		filePath = fillInfo->textureData->GetValue(feature);
		glbInt32 index = filePath.find_first_of(L'.');
		if(index == 0)
		{
			CGlbWString execDir = CGlbPath::GetExecDir();
			filePath = execDir + filePath.substr(1,filePath.size());
		}
		image = osgDB::readImageFile(filePath.ToString());
	}

	if(image == NULL)
		return;	

	double rotation = 0.0;
	if(fillInfo->textureRotation)
		rotation = fillInfo->textureRotation->GetValue(feature);	

	cglbExtent = const_cast<CGlbExtent*>(outline->GetExtent());
	//double cx,cy;
	//cglbExtent->GetCenter(&cx,&cy);


	// 需要纹理渲染
	isRenderTex = true;	
	double yaw = 0.0;
	if (markerInfo->yaw)
		yaw = markerInfo->yaw->GetValue(feature);

	agg::scanline_p8 sl;
	glbInt32 imageOpacity = 100;//图片透明度
	if(fillInfo->opacity)
		imageOpacity = fillInfo->opacity->GetValue(feature);

	agg::rendering_buffer rbuf_img(NULL,0,0,0);
	GLenum dataType = (GLenum)image->getPixelFormat();

	int stride = 0,s = image->s(),t = image->t();
	switch(dataType)
	{
	case GL_RGB: stride = s * 3;break;
	case GL_RGBA: stride = s * 4;break;
	default:break;
	}

	unsigned char *data = image->data();
	if(dataType == GL_RGBA)
	{
		static osg::ref_ptr<osg::Image> recImage = dynamic_cast<osg::Image *>(image->clone(osg::CopyOp::DEEP_COPY_IMAGES));
		static CGlbWString cFilePath = filePath;
		if(cFilePath != filePath)
		{
			recImage = dynamic_cast<osg::Image *>(image->clone(osg::CopyOp::DEEP_COPY_IMAGES));
			cFilePath = filePath;
		}
		if(NULL == recImage)
			return;
		unsigned char *recData = recImage->data();
		for(glbInt32 i = 0; i < image->t();i++)
			for(glbInt32 j = 0; j < image->s();j++)
			{
				unsigned char* a = recData + ( i * image->s() + j) * 4 + 3;	
				data[(i * image->s() + j) * 4 + 3] = (*a) * imageOpacity  / 100.0;
			}
	}

	rbuf_img = agg::rendering_buffer(data,s,t,stride);

	agg::path_storage canvas;
	CGlbLine *line = dynamic_cast<CGlbLine *>(outline); //(noROutline.get()) //outline
	if(line == NULL)
		return;

	const glbDouble *points = line->GetPoints();
	glbInt32 count = line->GetCount();
	for (glbInt32 i = 0; i <count; i++)
	{
		//不做矩阵变换，转换坐标用这个
		osg::Vec2d point(points[2 * i],points[2 * i + 1]);		

		double new_x = (point.x() - tempExtent.GetLeft()) * (tempImageW / (tempExtent.GetRight() - tempExtent.GetLeft()));
		double new_y = (point.y() - tempExtent.GetBottom()) * (tempImageH / (tempExtent.GetTop() - tempExtent.GetBottom()));			

		if(i == 0)
		{
			canvas.move_to(new_x,new_y);
			continue;
		}
		canvas.line_to(new_x,new_y);		
	}
	canvas.close_polygon();

	agg::trans_affine imageMatrix;
	// 1. 首先实现旋转
	imageMatrix.multiply(agg::trans_affine_translation(-image->s()*0.5,-image->t()*0.5));
	imageMatrix.multiply(agg::trans_affine_rotation(agg::deg2rad(rotation+yaw)));
	imageMatrix.multiply(agg::trans_affine_translation(image->s()*0.5,image->t()*0.5));

	// 2. 图像缩放
	double imgscalex = double(tempImageW) / image->s() * (cglbExtent->GetXWidth() / tempExtent.GetXWidth());
	double imgscaley = double(tempImageH) / image->t() * (cglbExtent->GetYHeight() / tempExtent.GetYHeight());	
	imageMatrix.multiply(agg::trans_affine_scaling(imgscalex ,imgscaley));

	// 3. 先把图像中心点偏移到(cglbExtent->GetLeft(),cglbExtent->GetBottom())处,以实现中心旋转
	double new_basex = cglbExtent->GetLeft(); 
	double new_basey = cglbExtent->GetBottom();
	double transx_first = (new_basex-tempExtent.GetLeft()) * tempImageW / tempExtent.GetXWidth();
	double transy_first = (new_basey-tempExtent.GetBottom()) * tempImageH / tempExtent.GetYHeight();
	imageMatrix.multiply(agg::trans_affine_translation(transx_first,transy_first));
	// 矩阵取反
	imageMatrix.invert();

	if(dataType == GL_RGB)
	{
		agg::int8u *newData = new agg::int8u[t * s * 4];
		agg::rendering_buffer newBuffer(newData,s,t,s * 4);
		agg::color_conv(&newBuffer,&rbuf_img,agg::color_conv_rgb24_to_rgba32());
		rbuf_img = newBuffer;
	}

	if(dataType == GL_RGB)
	{
		unsigned char * rbufData = rbuf_img.buf();
		for(int i = 0; i < t;i++)
		{
			for(int j =0; j < s; j++)
			{
				rbufData[(i * s + j) * 4 + 3] *= imageOpacity  / 100.0;
			}
		}
	}

	typedef agg::renderer_base<agg::pixfmt_rgba32> RendererBaseType;
	agg::pixfmt_rgba32 pixelFormat(rbuf);
	RendererBaseType rendererBase(pixelFormat);

	typedef agg::span_allocator<RendererBaseType::color_type> AllocatorType;
	AllocatorType allocator;

	agg::pixfmt_rgba32 pixelFormatOfData( rbuf_img );
	typedef agg::span_interpolator_linear<> InterpolatorType;
	InterpolatorType interpolator(imageMatrix);
	typedef agg::image_accessor_clone<agg::pixfmt_rgba32> ImageAccessorType;
	ImageAccessorType imageAccessor( pixelFormatOfData );
	typedef agg::image_accessor_clone<agg::pixfmt_rgba32> RGBAImageAccessorType;
	typedef agg::span_image_filter_rgba_bilinear<RGBAImageAccessorType, InterpolatorType> RGBASpanType;
	RGBASpanType generator(imageAccessor,interpolator);

	agg::rasterizer_scanline_aa<> rasterizer;
	rasterizer.reset();
	agg::conv_contour<agg::path_storage> converter(canvas);
	rasterizer.add_path(converter);
	agg::scanline_p8 scanline;
	agg::render_scanlines_aa(rasterizer,scanline,rendererBase,allocator,generator);

	unsigned char * rbufData = rbuf_img.buf();
	if (rbufData)
	{
		delete [] rbufData;
	}
}

IGlbGeometry *CGlbGlobeMarker2DShapeSymbol::DealNoRotOutline(osg::Vec2d centerPosition, double rotation,
															GlbMarker2DShapeSymbolInfo *markerInfo,CGlbFeature *feature,GlbGlobeTypeEnum globeType)
{
	if(markerInfo == NULL)
		return NULL;

	CGlbLine *line = new CGlbLine;	

	rotation = agg::deg2rad(rotation);
	GlbShapeInfo *shapeInfo = markerInfo->shapeInfo;
	if(shapeInfo == NULL)
		return NULL;

	osg::Vec3d centerPos(centerPosition.x(),centerPosition.y(),0);
	osg::Matrixd localToWorld;
	if (GLB_GLOBETYPE_GLOBE==mpr_globeType)
	{
		g_ellipsoidModel->computeLocalToWorldTransformFromLatLongHeight(osg::DegreesToRadians(centerPos.y()),osg::DegreesToRadians(centerPos.x()),centerPos.z(),localToWorld);
		g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(centerPos.y()),osg::DegreesToRadians(centerPos.x()),centerPos.z(),centerPos.x(),centerPos.y(),centerPos.z());				
		//g_ellipsoidModel->computeLocalToWorldTransformFromXYZ(centerPos.x(),centerPos.y(),centerPos.z(),localToWorld);
	}

	if (shapeInfo->shapeType == GLB_MARKERSHAPE_CIRCLE)
	{
		GlbCircleInfo *circleInfo = 
			static_cast<GlbCircleInfo *>(shapeInfo);

		if (circleInfo)
		{
			if(circleInfo->edges == NULL || circleInfo->radius == NULL)
				return NULL;
			glbInt32 edges = circleInfo->edges->GetValue(feature);
			glbFloat xRadius = circleInfo->radius->GetValue(feature);// / meterlonByDegree;
			glbFloat yRadius = circleInfo->radius->GetValue(feature);// / meterlatByDegree;
			glbFloat angle = 0.0f;
			glbFloat texCoord = 0.0f;
			double firstX = 0.0;
			double firstY = 0.0;
			glbFloat angleDelta = 2.0f * osg::PI / (glbFloat)edges;
			for(glbInt32 i = 0;i <= edges;++i,angle += angleDelta)
			{
				glbDouble xx = xRadius * cosf(angle);
				glbDouble yy = yRadius * sinf(angle);

				double theta = atan2(yy,xx);
				double len = sqrt(xx*xx+yy*yy);
				xx = len * cos(theta+rotation);
				yy = len * sin(theta+rotation);
				osg::Vec3d pt(xx,yy,0);	
				if (GLB_GLOBETYPE_GLOBE==globeType)
				{
					pt = localToWorld.preMult(pt);  // pt * localToWorld
					g_ellipsoidModel->convertXYZToLatLongHeight(pt.x(),pt.y(),pt.z(),pt.y(),pt.x(),pt.z());
					line->AddPoint(osg::RadiansToDegrees(pt.x()),osg::RadiansToDegrees(pt.y()));
				}
				else
					line->AddPoint(centerPosition.x()+pt.x(),centerPosition.y()+pt.y());				
			}
		}
	}
	else if (shapeInfo->shapeType == GLB_MARKERSHAPE_RECTANGLE)
	{
		GlbRectangleInfo *rectangleInfo = 
			static_cast<GlbRectangleInfo *>(shapeInfo);

		if (rectangleInfo)
		{
			if(rectangleInfo->width == NULL || rectangleInfo->height == NULL)
				return NULL;

			glbDouble halfWidth = rectangleInfo->width->GetValue(feature) / 2;
			glbDouble halfHeight = rectangleInfo->height->GetValue(feature) / 2;					

			osg::Vec2d pts[5];
			pts[0].set(-halfWidth, halfHeight);
			pts[1].set(-halfWidth, -halfHeight);
			pts[2].set(halfWidth, -halfHeight);
			pts[3].set(halfWidth, halfHeight);
			pts[4].set(-halfWidth,halfHeight);

			for (int k = 0; k <5; k++)
			{			
				double theta = atan2(pts[k].y(),pts[k].x());
				double len = pts[k].length();//sqrt(pts[k].x()*pts[k].x()+pts[k].y()*pts[k].y());
				double xx = len * cos(theta+rotation);
				double yy = len * sin(theta+rotation);

				osg::Vec3d pt(xx,yy,0);
				if (GLB_GLOBETYPE_GLOBE==globeType)
				{
					pt = localToWorld.preMult(pt);  // pt * localToWorld
					g_ellipsoidModel->convertXYZToLatLongHeight(pt.x(),pt.y(),pt.z(),pt.y(),pt.x(),pt.z());
					line->AddPoint(osg::RadiansToDegrees(pt.x()),osg::RadiansToDegrees(pt.y()));
				}
				else
					line->AddPoint(centerPosition.x()+pt.x(),centerPosition.y()+pt.y());

			}
		}
	}
	else if (shapeInfo->shapeType == GLB_MARKERSHAPE_ELLIPSE)
	{
		GlbEllipseInfo *ellipseInfo = 
			static_cast<GlbEllipseInfo *>(shapeInfo);

		if (ellipseInfo)
		{
			if(ellipseInfo->edges == NULL || ellipseInfo->xRadius == NULL || ellipseInfo->yRadius == NULL)
				return NULL;
			glbFloat angle = 0.0;
			glbInt32 edges = ellipseInfo->edges->GetValue(feature);
			glbFloat angleDelta = 2.0f * osg::PI / (glbFloat) edges;

			glbDouble xRadius = ellipseInfo->xRadius->GetValue(feature);
			glbDouble yRadius = ellipseInfo->yRadius->GetValue(feature);

			for (glbInt32 i = 0; i <= edges; ++i,angle += angleDelta)
			{				
				double xx = xRadius*cosf(angle);
				double yy = yRadius*sinf(angle);

				double theta = atan2(yy,xx);
				double len = sqrt(xx*xx+yy*yy);
				xx = len * cos(theta+rotation);
				yy = len * sin(theta+rotation);
				osg::Vec3d pt(xx,yy,0);	
				if (GLB_GLOBETYPE_GLOBE==globeType)
				{
					pt = localToWorld.preMult(pt);  // pt * localToWorld
					g_ellipsoidModel->convertXYZToLatLongHeight(pt.x(),pt.y(),pt.z(),pt.y(),pt.x(),pt.z());
					line->AddPoint(osg::RadiansToDegrees(pt.x()),osg::RadiansToDegrees(pt.y()));
				}
				else
					line->AddPoint(centerPosition.x()+pt.x(),centerPosition.y()+pt.y());
			}
		}
	}
	else if (shapeInfo->shapeType == GLB_MARKERSHAPE_ARC)
	{
		GlbArcInfo *arcInfo = 
			static_cast<GlbArcInfo *>(shapeInfo);

		if (arcInfo)
		{
			if(arcInfo->edges == NULL || arcInfo->xRadius == NULL || 
				arcInfo->yRadius == NULL || arcInfo->sAngle == NULL || arcInfo->eAngle == NULL)
				return NULL;
			glbInt32 edges = arcInfo->edges->GetValue(feature);

			glbFloat sAngle = osg::DegreesToRadians(arcInfo->sAngle->GetValue(feature));
			glbFloat eAngle = osg::DegreesToRadians(arcInfo->eAngle->GetValue(feature));
			glbFloat angle = sAngle;
			glbFloat angleDelta = (eAngle - sAngle) / (glbFloat)edges;
			double firstX = 0.0;
			double firstY = 0.0;

			glbDouble xRadius = arcInfo->xRadius->GetValue(feature);
			glbDouble yRadius = arcInfo->yRadius->GetValue(feature);			

			for (glbInt32 i = 0; i < edges; ++i,angle += angleDelta)
			{
				double xx = xRadius * cosf(angle);
				double yy = yRadius * sinf(angle);

				double theta = atan2(yy,xx);
				double len = sqrt(xx*xx+yy*yy);
				xx = len * cos(theta+rotation);
				yy = len * sin(theta+rotation);

				double x2,y2;
				osg::Vec3d pt(xx,yy,0);	
				if (GLB_GLOBETYPE_GLOBE==globeType)
				{
					pt = localToWorld.preMult(pt);  // pt * localToWorld
					g_ellipsoidModel->convertXYZToLatLongHeight(pt.x(),pt.y(),pt.z(),pt.y(),pt.x(),pt.z());
					x2 = osg::RadiansToDegrees(pt.x());
					y2 = osg::RadiansToDegrees(pt.y());					
				}
				else
				{
					x2 = centerPosition.x()+pt.x();
					y2 = centerPosition.y()+pt.y();
				}

				if (i == 0)
				{
					if(arcInfo->isFan)
					{
						if (arcInfo->isFan->GetValue(feature))
						{
							firstX = centerPosition.x();
							firstY = centerPosition.y();							
							line->AddPoint(centerPosition.x(),centerPosition.y());
							line->AddPoint(x2,y2);
						}
						else
						{
							firstX = x2;
							firstY = y2;							
							line->AddPoint(x2,y2);
						}
						continue;
					}
				}				
				line->AddPoint(x2,y2);
			}
			line->AddPoint(firstX,firstY);
		}
	}
	return line;
}

IGlbGeometry * CGlbGlobeMarker2DShapeSymbol::DealOutline(	osg::Vec2d centerPosition,GlbMarker2DShapeSymbolInfo *markerInfo,
															CGlbFeature *feature,GlbGlobeTypeEnum globeType )
{
	if(markerInfo == NULL)
		return NULL;

	CGlbLine *line = new CGlbLine;

	double rotation = 0.0;
	if (markerInfo->yaw)
		rotation = markerInfo->yaw->GetValue(feature);
	rotation = agg::deg2rad(rotation);

	GlbShapeInfo *shapeInfo = markerInfo->shapeInfo;
	if(shapeInfo == NULL)
		return NULL;

	osg::Vec3d centerPos(centerPosition.x(),centerPosition.y(),0);
	osg::Matrixd localToWorld;
	if (GLB_GLOBETYPE_GLOBE==globeType)
	{
		g_ellipsoidModel->computeLocalToWorldTransformFromLatLongHeight(
			osg::DegreesToRadians(centerPos.y()),osg::DegreesToRadians(centerPos.x()),centerPos.z(),localToWorld);
		g_ellipsoidModel->convertLatLongHeightToXYZ(
			osg::DegreesToRadians(centerPos.y()),osg::DegreesToRadians(centerPos.x()),centerPos.z(),
			centerPos.x(),centerPos.y(),centerPos.z());				
		//g_ellipsoidModel->computeLocalToWorldTransformFromXYZ(centerPos.x(),centerPos.y(),centerPos.z(),localToWorld);
	}

	if (shapeInfo->shapeType == GLB_MARKERSHAPE_CIRCLE)
	{
		GlbCircleInfo *circleInfo = 
			static_cast<GlbCircleInfo *>(shapeInfo);

		if (circleInfo)
		{
			if(circleInfo->edges == NULL || circleInfo->radius == NULL)
				return NULL;
			glbInt32 edges = circleInfo->edges->GetValue(feature);
			glbFloat xRadius = circleInfo->radius->GetValue(feature);// / meterlonByDegree;
			glbFloat yRadius = circleInfo->radius->GetValue(feature);// / meterlatByDegree;
			glbFloat angle = 0.0f;
			glbFloat texCoord = 0.0f;
			double firstX = 0.0;
			double firstY = 0.0;
			glbFloat angleDelta = 2.0f * osg::PI / (glbFloat)edges;
			for(glbInt32 i = 0;i < edges;++i,angle += angleDelta)
			{
				glbDouble xx = xRadius * cosf(angle);
				glbDouble yy = yRadius * sinf(angle);

				double theta = atan2(yy,xx);
				double len = sqrt(xx*xx+yy*yy);
				xx = len * cos(theta+rotation);
				yy = len * sin(theta+rotation);
				osg::Vec3d pt(xx,yy,0);	
				if (GLB_GLOBETYPE_GLOBE==globeType)
				{
					pt = localToWorld.preMult(pt);  // pt * localToWorld
					g_ellipsoidModel->convertXYZToLatLongHeight(pt.x(),pt.y(),pt.z(),pt.y(),pt.x(),pt.z());
					line->AddPoint(osg::RadiansToDegrees(pt.x()),osg::RadiansToDegrees(pt.y()));
				}
				else
					line->AddPoint(centerPosition.x()+pt.x(),centerPosition.y()+pt.y());				
			}
		}
	}
	else if (shapeInfo->shapeType == GLB_MARKERSHAPE_RECTANGLE)
	{
		GlbRectangleInfo *rectangleInfo = 
			static_cast<GlbRectangleInfo *>(shapeInfo);

		if (rectangleInfo)
		{
			if(rectangleInfo->width == NULL || rectangleInfo->height == NULL)
				return NULL;

			glbDouble halfWidth = rectangleInfo->width->GetValue(feature) / 2;
			glbDouble halfHeight = rectangleInfo->height->GetValue(feature) / 2;					

			osg::Vec2d pts[4];
			pts[0].set(-halfWidth, halfHeight);
			pts[1].set(-halfWidth, -halfHeight);
			pts[2].set(halfWidth, -halfHeight);
			pts[3].set(halfWidth, halfHeight);
			//pts[4].set(-halfWidth + 1.0e-7,halfHeight);

			for (int k = 0; k < 4; k++)
			{			
				double theta = atan2(pts[k].y(),pts[k].x());
				double len = pts[k].length();//sqrt(pts[k].x()*pts[k].x()+pts[k].y()*pts[k].y());
				double xx = len * cos(theta+rotation);
				double yy = len * sin(theta+rotation);

				osg::Vec3d pt(xx,yy,0);
				if (GLB_GLOBETYPE_GLOBE==globeType)
				{
					pt = localToWorld.preMult(pt);  // pt * localToWorld
					g_ellipsoidModel->convertXYZToLatLongHeight(pt.x(),pt.y(),pt.z(),pt.y(),pt.x(),pt.z());
					line->AddPoint(osg::RadiansToDegrees(pt.x()),osg::RadiansToDegrees(pt.y()));
				}
				else
					line->AddPoint(centerPosition.x()+pt.x(),centerPosition.y()+pt.y());	
			}
		}
	}
	else if (shapeInfo->shapeType == GLB_MARKERSHAPE_ELLIPSE)
	{
		GlbEllipseInfo *ellipseInfo = 
			static_cast<GlbEllipseInfo *>(shapeInfo);

		if (ellipseInfo)
		{
			if(ellipseInfo->edges == NULL || ellipseInfo->xRadius == NULL || ellipseInfo->yRadius == NULL)
				return NULL;
			glbFloat angle = 0.0;
			glbInt32 edges = ellipseInfo->edges->GetValue(feature);
			glbFloat angleDelta = 2.0f * osg::PI / (glbFloat) edges;

			glbDouble xRadius = ellipseInfo->xRadius->GetValue(feature);
			glbDouble yRadius = ellipseInfo->yRadius->GetValue(feature);
			
			for (glbInt32 i = 0; i < edges; ++i,angle += angleDelta)
			{				
				double xx = xRadius*cosf(angle);
				double yy = yRadius*sinf(angle);
				
				double theta = atan2(yy,xx);
				double len = sqrt(xx*xx+yy*yy);
				xx = len * cos(theta+rotation);
				yy = len * sin(theta+rotation);
				osg::Vec3d pt(xx,yy,0);	
				if (GLB_GLOBETYPE_GLOBE==globeType)
				{
					pt = localToWorld.preMult(pt);  // pt * localToWorld
					g_ellipsoidModel->convertXYZToLatLongHeight(pt.x(),pt.y(),pt.z(),pt.y(),pt.x(),pt.z());
					line->AddPoint(osg::RadiansToDegrees(pt.x()),osg::RadiansToDegrees(pt.y()));
				}
				else
					line->AddPoint(centerPosition.x()+pt.x(),centerPosition.y()+pt.y());
			}
		}
	}
	else if (shapeInfo->shapeType == GLB_MARKERSHAPE_ARC)
	{
		GlbArcInfo *arcInfo = 
			static_cast<GlbArcInfo *>(shapeInfo);

		if (arcInfo)
		{
			if(arcInfo->edges == NULL || arcInfo->xRadius == NULL || 
				arcInfo->yRadius == NULL || arcInfo->sAngle == NULL || arcInfo->eAngle == NULL)
				return NULL;
			glbInt32 edges = arcInfo->edges->GetValue(feature);

			glbFloat sAngle = osg::DegreesToRadians(arcInfo->sAngle->GetValue(feature));
			glbFloat eAngle = osg::DegreesToRadians(arcInfo->eAngle->GetValue(feature));
			glbFloat angle = sAngle;
			glbFloat angleDelta = (eAngle - sAngle) / (glbFloat)edges;
			double firstX = 0.0;
			double firstY = 0.0;

			glbDouble xRadius = arcInfo->xRadius->GetValue(feature);
			glbDouble yRadius = arcInfo->yRadius->GetValue(feature);			

			for (glbInt32 i = 0; i <= edges; ++i,angle += angleDelta)
			{
				double xx = xRadius * cosf(angle);
				double yy = yRadius * sinf(angle);

				double theta = atan2(yy,xx);
				double len = sqrt(xx*xx+yy*yy);
				xx = len * cos(theta+rotation);
				yy = len * sin(theta+rotation);

				double x2,y2;
				osg::Vec3d pt(xx,yy,0);	
				if (GLB_GLOBETYPE_GLOBE==globeType)
				{
					pt = localToWorld.preMult(pt);  // pt * localToWorld
					g_ellipsoidModel->convertXYZToLatLongHeight(pt.x(),pt.y(),pt.z(),pt.y(),pt.x(),pt.z());
					x2 = osg::RadiansToDegrees(pt.x());
					y2 = osg::RadiansToDegrees(pt.y());					
				}
				else
				{
					x2 = centerPosition.x()+pt.x();
					y2 = centerPosition.y()+pt.y();
				}

				if (i == 0)
				{
					if(arcInfo->isFan)
					{
						if (arcInfo->isFan->GetValue(feature))
						{
							firstX = centerPosition.x();
							firstY = centerPosition.y();							
							line->AddPoint(centerPosition.x(),centerPosition.y());
							line->AddPoint(x2,y2);
						}
						else
						{
							firstX = x2;
							firstY = y2;							
							line->AddPoint(x2,y2);
						}
						continue;
					}
				}				
				line->AddPoint(x2,y2);
			}
			//line->AddPoint(firstX + 1.0e-7,firstY);
		}
	}
	return line;
}

IGlbGeometry * CGlbGlobeMarker2DShapeSymbol::GetOutline( CGlbGlobeRObject *obj,IGlbGeometry *geo )
{
	if(obj->GetAltitudeMode() == GLB_ALTITUDEMODE_ONTERRAIN)
	{
		GlbRenderInfo *renderInfo = obj->GetRenderInfo();
		if(renderInfo == NULL)
			return NULL;
		GlbMarker2DShapeSymbolInfo *marker2DInfo = 
			static_cast<GlbMarker2DShapeSymbolInfo *>(renderInfo);
		CGlbFeature *feature = obj->GetFeature(); 

		osg::ref_ptr<osg::Vec2dArray> geoPoints = new osg::Vec2dArray();
		GlbGeometryTypeEnum geoType = geo->GetType();
		glbDouble ptx,pty;
		switch(geoType)
		{
		case GLB_GEO_POINT:
			{
				CGlbPoint* pt = dynamic_cast<CGlbPoint *>(geo);
				pt->GetXY(&ptx,&pty);
				geoPoints->push_back(osg::Vec2d(ptx,pty));
			}
			break;
		case GLB_GEO_MULTIPOINT:
			{
				CGlbMultiPoint* pts = dynamic_cast<CGlbMultiPoint*>(geo);
				glbInt32 ptCnt = pts->GetCount();
				for (glbInt32 i = 0; i < ptCnt; i++)
				{
					pts->GetPoint(i,&ptx,&pty);
					geoPoints->push_back(osg::Vec2d(ptx,pty));
				}				
			}
			break;	
		}

		int ptCnt = geoPoints->size();
		if (ptCnt==1)
		{
			mpt_outline = DealOutline(geoPoints->at(0),marker2DInfo,feature,obj->GetGlobe()->GetType());
		}
		else if (ptCnt>1)
		{
			CGlbMultiLine* multiln = NULL;
			for (glbInt32 k = 0; k < ptCnt; k++)
			{
				glbref_ptr<IGlbGeometry> geom = DealOutline(geoPoints->at(k),marker2DInfo,feature,obj->GetGlobe()->GetType());
				if (geom)
				{
					geoType = geom->GetType();
					if (geoType==GLB_GEO_LINE)
					{
						CGlbLine* ln = dynamic_cast<CGlbLine*>(geom.get());
						if (ln)
						{
							if (!multiln) multiln = new CGlbMultiLine();
							multiln->AddLine(ln);
						}
					}					
				}
			}
			mpt_outline = multiln;
		}
	}
	return mpt_outline.get();
}

void CGlbGlobeMarker2DShapeSymbol::DrawToImage( CGlbGlobeRObject *obj, glbByte *image/*RGBA*/,
	glbInt32 imageW, glbInt32 imageH,CGlbExtent &ext,IGlbGeometry* outline,IGlbGeometry* noRotOutline )
{
	agg::rendering_buffer rbuf(image, imageW, imageH, -imageW * 4);
	agg::pixfmt_rgba32 pixf(rbuf);
	renb_type renb(pixf);//底层渲染器 
	mpr_noRotOutline = noRotOutline;
	DealAggRender(rbuf,renb,obj,imageW,imageH,ext,outline);	
}

void CGlbGlobeMarker2DShapeSymbol::DealDraw(	osg::Shape *shape,
												osg::ShapeDrawable *shapeFillDrawable, 
												osg::Node *outlineNode, 
												GlbMarker2DShapeSymbolInfo *marker2DInfo,
												CGlbFeature *feature )
{
	osg::ref_ptr<osg::StateSet> fillStateSet = shapeFillDrawable->getOrCreateStateSet();
	osg::ref_ptr<osg::Material> fillMaterial = 
		dynamic_cast<osg::Material*>(fillStateSet->getAttribute(osg::StateAttribute::MATERIAL));
	if (!fillMaterial)
		fillMaterial = new osg::Material;

	fillStateSet->setAttribute(fillMaterial,osg::StateAttribute::ON);
	fillStateSet->setMode(GL_DEPTH_TEST,osg::StateAttribute::ON);

	if(marker2DInfo->fillInfo == NULL)
		return;

	if(marker2DInfo->fillInfo->color)
	{
		fillMaterial->setEmission(osg::Material::FRONT_AND_BACK,
			GetColor(marker2DInfo->fillInfo->color->GetValue(feature)));
		fillMaterial->setDiffuse(osg::Material::FRONT_AND_BACK,osg::Vec4(0,0,0,0));
	}

	if(marker2DInfo->fillInfo->opacity)
	{
		glbInt32 opacity = marker2DInfo->fillInfo->opacity->GetValue(feature);
		fillMaterial->setAlpha(osg::Material::FRONT_AND_BACK,opacity / 100.0);

		if (opacity<100)
		{
			fillStateSet->setMode(GL_BLEND,osg::StateAttribute::ON);
			if (fillStateSet->getRenderingHint() != osg::StateSet::TRANSPARENT_BIN)
				fillStateSet->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
		}
		else
		{
			fillStateSet->setMode(GL_BLEND,osg::StateAttribute::OFF);
			fillStateSet->setRenderingHint(osg::StateSet::OPAQUE_BIN);
		}
	}
	else
	{
		fillStateSet->setMode(GL_BLEND,osg::StateAttribute::OFF);
		fillStateSet->setRenderingHint(osg::StateSet::OPAQUE_BIN);
	}	

	ref_ptr<osg::Texture2D> texture = new osg::Texture2D;
	ref_ptr<osg::TexMat> texMat = new osg::TexMat;
	//纹理重复以及旋转
	osg::Matrix mScale;
	osg::Matrix mRotate;
	double tilingU = 1.0;
	double tilingV = 1.0;
	//贴图
	osg::Image *image = NULL;
	if(marker2DInfo->fillInfo->textureData)
	{
		CGlbWString ws(marker2DInfo->fillInfo->textureData->GetValue(feature));
		glbInt32 index = ws.find_first_of(L'.');
		if(index == 0)
		{
			CGlbWString execDir = CGlbPath::GetExecDir();
			ws = execDir + ws.substr(1,ws.size());
		}
		image = osgDB::readImageFile(ws.ToString());

		osg::ImageStream* imagestream = dynamic_cast<osg::ImageStream*>(image);
		if (imagestream) 
			imagestream->play();
	}

	double scalem = 1.0;
	if (image)
	{
		texture->setImage(image);

		if(marker2DInfo->fillInfo->texRepeatMode)
		{
			if (marker2DInfo->fillInfo->texRepeatMode->GetValue(feature)
				== GLB_TEXTUREREPEAT_TIMES)
			{
				if(marker2DInfo->fillInfo->tilingU)
					tilingU = marker2DInfo->fillInfo->tilingU->GetValue(feature);
				if(marker2DInfo->fillInfo->tilingV)
					tilingV = marker2DInfo->fillInfo->tilingV->GetValue(feature);
			}
			else if (marker2DInfo->fillInfo->texRepeatMode->GetValue(feature)
				== GLB_TEXTUREREPEAT_SIZE)
			{
				CustomCircle *circle = dynamic_cast<CustomCircle *>(shape);
				CustomRectangle *rectangle = dynamic_cast<CustomRectangle *>(shape);
				CustomEllipse *ellipse = dynamic_cast<CustomEllipse *>(shape);
				CustomArc *arc = dynamic_cast<CustomArc *>(shape);
				if (circle)
				{
					tilingU = 2 * circle->getRadius() / image->s();
					tilingV = 2 * circle->getRadius() / image->t();
				}
				else if (rectangle)
				{
					tilingU = rectangle->getWidth() / image->s();
					tilingV = rectangle->getHeight() / image->t();
				}
				else if (ellipse)
				{
					tilingU = 2 * ellipse->getXRadius() / image->s();
					tilingV = 2 * ellipse->getYRadius() / image->t();
				}
				else if (arc)
				{
					tilingU = 2 * arc->getXRadius() / image->s();
					tilingV = 2 * arc->getYRadius() / image->t();
				}
			}
		}

		//mScale.makeScale(tilingU,tilingV,0.0);
		//if(marker2DInfo->fillInfo->textureRotation)
		//	mRotate.makeRotate(
		//	osg::DegreesToRadians(marker2DInfo->fillInfo->textureRotation->GetValue(feature)),
		//	osg::Vec3d(0,0,1));
		glbDouble textureRotation = 0.0;
		if(marker2DInfo->fillInfo->textureRotation)
			textureRotation = osg::DegreesToRadians(marker2DInfo->fillInfo->textureRotation->GetValue(feature));
		glbDouble bWidth = shapeFillDrawable->getBound().xMax() - shapeFillDrawable->getBound().xMin();
		glbDouble bHeight = shapeFillDrawable->getBound().yMax() - shapeFillDrawable->getBound().yMin();
		glbDouble ra = sqrt(bWidth * bWidth + bHeight * bHeight);
		glbDouble ang = atan2(bHeight,bWidth);
		glbDouble xm = ra * max(abs(cos(ang - textureRotation)),abs(cos(ang + textureRotation)));
		glbDouble ym = ra * max(abs(sin(ang-textureRotation)),abs(sin(ang + textureRotation)));
		scalem = ra * ra / (xm * xm + ym * ym);
		mScale.makeScale(scalem * tilingU,scalem * tilingV,1.0);
		mRotate.makeRotate(textureRotation,osg::Vec3d(0,0,1));
	}

	texture->setWrap(osg::Texture2D::WRAP_S,osg::Texture2D::REPEAT);
	texture->setWrap(osg::Texture2D::WRAP_T,osg::Texture2D::REPEAT);
	fillStateSet->setTextureAttributeAndModes(0,texture,osg::StateAttribute::ON);
	//texMat->setMatrix(osg::Matrix::translate(-0.5,-0.5,0)
	//	* mRotate * osg::Matrix::translate(0.5,0.5,0) * mScale);
	texMat->setMatrix(osg::Matrix::translate(-0.5,-0.5,0) * mRotate * mScale * osg::Matrix::translate(0.5 * tilingU ,0.5 * tilingV,0));
	fillStateSet->setTextureAttributeAndModes(0,texMat,osg::StateAttribute::ON);

	osg::ref_ptr<osg::StateSet> outStateSet = outlineNode->getOrCreateStateSet();
	osg::ref_ptr<osg::Material> outMaterial = 
		dynamic_cast<osg::Material*>(outStateSet->getAttribute(osg::StateAttribute::MATERIAL));
	if (!outMaterial)
		outMaterial = new osg::Material;

	outStateSet->setAttribute(outMaterial,osg::StateAttribute::ON);
	outStateSet->setMode(GL_BLEND,osg::StateAttribute::ON);
	outStateSet->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
	outStateSet->setMode(GL_DEPTH_TEST,osg::StateAttribute::ON);

	if(marker2DInfo->fillInfo->outlineInfo && marker2DInfo->fillInfo->outlineInfo->lineColor)
	{
		outMaterial->setEmission(osg::Material::FRONT_AND_BACK,
			GetColor(marker2DInfo->fillInfo->outlineInfo->lineColor->GetValue(feature)));
		outMaterial->setDiffuse(osg::Material::FRONT_AND_BACK,osg::Vec4(0,0,0,0));
	}
	if(marker2DInfo->fillInfo->outlineInfo && marker2DInfo->fillInfo->outlineInfo->lineOpacity)
		outMaterial->setAlpha(osg::Material::FRONT_AND_BACK,
		marker2DInfo->fillInfo->outlineInfo->lineOpacity->GetValue(feature) / 100.0);
	//线宽
	if(marker2DInfo->fillInfo->outlineInfo && marker2DInfo->fillInfo->outlineInfo->lineWidth)
	{
		ref_ptr<osg::LineWidth> lineWidth = new osg::LineWidth;
		lineWidth->setWidth(marker2DInfo->fillInfo->outlineInfo->lineWidth->GetValue(feature));
		outStateSet->setAttributeAndModes(lineWidth,osg::StateAttribute::ON);
	}

	if(marker2DInfo->fillInfo->outlineInfo && marker2DInfo->fillInfo->outlineInfo->linePattern)
	{
		if(marker2DInfo->fillInfo->outlineInfo->linePattern->GetValue(feature) == GLB_LINE_DOTTED)
		{
			//线样式
			ref_ptr<osg::LineStipple> lineStipple = new osg::LineStipple;
			lineStipple->setFactor(1);
			lineStipple->setPattern(0x1C47);
			outStateSet->setAttributeAndModes(lineStipple,osg::StateAttribute::ON);
		}
	}	
}

osg::ref_ptr<osg::Vec2dArray> CGlbGlobeMarker2DShapeSymbol::GetPointFromGeom(IGlbGeometry *geom)
{
	osg::ref_ptr<osg::Vec2dArray> pts = new osg::Vec2dArray;
	glbDouble ptx,pty;
	GlbGeometryTypeEnum geomType = geom->GetType();
	switch(geomType)	
	{
	case GLB_GEO_POINT:
		{			
			CGlbPoint* pt = dynamic_cast<CGlbPoint*>(geom);
			pt->GetXY(&ptx,&pty);
			pts->push_back(osg::Vec2d(ptx,pty));				
		}
		break;	
	case GLB_GEO_MULTIPOINT:
		{
			CGlbMultiPoint* multPt = dynamic_cast<CGlbMultiPoint*>(geom);
			glbInt32 ptCnt = multPt->GetCount();
			for (glbInt32 k = 0; k < ptCnt; k++)
			{
				multPt->GetPoint(k,&ptx,&pty);
				pts->push_back(osg::Vec2d(ptx,pty));
			}
		}
		break;	
	}

	return pts;
}

IGlbGeometry * CGlbGlobeMarker2DShapeSymbol::DealOutline3DVertexes( osg::Vec3d centerPosition, GlbMarker2DShapeSymbolInfo *markerInfo,CGlbFeature *feature,GlbGlobeTypeEnum globeType )
{
	if(markerInfo == NULL)
		return NULL;

	CGlbLine *line = new CGlbLine(3,false);
	GlbShapeInfo *shapeInfo = markerInfo->shapeInfo;
	osg::Vec3d tempPoint;
	osg::Matrixd localToWorld;//Globe模式
	osg::Matrixd trans;//Flat模式
	
	if(GLB_GLOBETYPE_GLOBE == globeType)
		g_ellipsoidModel->computeLocalToWorldTransformFromLatLongHeight(osg::DegreesToRadians(centerPosition.y()),
		osg::DegreesToRadians(centerPosition.x()),centerPosition.z(),localToWorld);
	else if(GLB_GLOBETYPE_FLAT == globeType)
		trans.makeTranslate(centerPosition.x(),centerPosition.y(),centerPosition.z());

	if (shapeInfo->shapeType == GLB_MARKERSHAPE_CIRCLE)
	{
		GlbCircleInfo *circleInfo = 
			static_cast<GlbCircleInfo *>(shapeInfo);

		if (circleInfo)
		{
			if(circleInfo->edges == NULL || circleInfo->radius == NULL)
				return NULL;
			glbInt32 edges = circleInfo->edges->GetValue(feature);
			glbFloat xRadius = circleInfo->radius->GetValue(feature);
			glbFloat yRadius = circleInfo->radius->GetValue(feature);
			glbFloat angle = 0.0f;
			double firstX = 0.0;
			double firstY = 0.0;
			glbFloat angleDelta = 2.0f * osg::PI / (glbFloat)edges;
			for(glbInt32 i = 0;i < edges;++i,angle += angleDelta)
			{
				glbFloat c = cosf(angle);
				glbFloat s = sinf(angle);
				if (i == 0)
				{
					firstX = c * xRadius;
					firstY = s * yRadius;
				}
				tempPoint = osg::Vec3d(c * xRadius,s * yRadius,0.0);
				if(GLB_GLOBETYPE_GLOBE == globeType)
				{
					tempPoint = localToWorld.preMult(tempPoint);
					g_ellipsoidModel->convertXYZToLatLongHeight(tempPoint.x(),
						tempPoint.y(),tempPoint.z(),tempPoint.y(),tempPoint.x(),tempPoint.z());
					line->AddPoint(osg::RadiansToDegrees(tempPoint.x()),
						osg::RadiansToDegrees(tempPoint.y()),tempPoint.z());
				}
				else if(GLB_GLOBETYPE_FLAT == globeType)
				{
					tempPoint = trans.preMult(tempPoint);
					line->AddPoint(tempPoint.x(),tempPoint.y(),tempPoint.z());
				}
			}
			tempPoint = osg::Vec3d(firstX,firstY,0.0);
			if(GLB_GLOBETYPE_GLOBE == globeType)
			{
				tempPoint = localToWorld.preMult(tempPoint);
				g_ellipsoidModel->convertXYZToLatLongHeight(tempPoint.x(),
					tempPoint.y(),tempPoint.z(),tempPoint.y(),tempPoint.x(),tempPoint.z());
				line->AddPoint(osg::RadiansToDegrees(tempPoint.x()),
					osg::RadiansToDegrees(tempPoint.y()),tempPoint.z());
			}
			else if(GLB_GLOBETYPE_FLAT == globeType)
			{
				tempPoint = trans.preMult(tempPoint);
				line->AddPoint(tempPoint.x(),tempPoint.y(),tempPoint.z());
			}
		}
	}
	else if (shapeInfo->shapeType == GLB_MARKERSHAPE_RECTANGLE)
	{
		GlbRectangleInfo *rectangleInfo = 
			static_cast<GlbRectangleInfo *>(shapeInfo);

		if (rectangleInfo)
		{
			if(rectangleInfo->width == NULL || rectangleInfo->height == NULL)
				return NULL;
			glbFloat halfWidth = rectangleInfo->width->GetValue(feature) / 2;
			glbFloat halfHeight = rectangleInfo->height->GetValue(feature) / 2;

			osg::Vec3d pts[5];
			osg::Vec3d tempPoint;
			pts[0].set(-halfWidth,halfHeight,0.0);
			pts[1].set(-halfWidth,-halfHeight,0.0);
			pts[2].set(halfWidth,-halfHeight,0.0);
			pts[3].set(halfWidth,halfHeight,0.0);
			pts[4].set(-halfWidth,halfHeight,0.0);

			for(glbInt32 i = 0; i < 5;i++)
			{
				tempPoint = osg::Vec3d(pts[i].x(),pts[i].y(),pts[i].z());
				if(GLB_GLOBETYPE_GLOBE == globeType)
				{
					tempPoint = localToWorld.preMult(tempPoint);
					g_ellipsoidModel->convertXYZToLatLongHeight(
						tempPoint.x(),tempPoint.y(),tempPoint.z(),tempPoint.y(),tempPoint.x(),tempPoint.z());
					line->AddPoint(osg::RadiansToDegrees(tempPoint.x()),osg::RadiansToDegrees(tempPoint.y()),tempPoint.z());
				}
				else if(GLB_GLOBETYPE_FLAT == globeType)
				{
					tempPoint = trans.preMult(tempPoint);
					line->AddPoint(tempPoint.x(),tempPoint.y(),tempPoint.z());
				}
			}
		}
	}
	else if (shapeInfo->shapeType == GLB_MARKERSHAPE_ELLIPSE)
	{
		GlbEllipseInfo *ellipseInfo = 
			static_cast<GlbEllipseInfo *>(shapeInfo);

		if (ellipseInfo)
		{
			if(ellipseInfo->edges == NULL || ellipseInfo->xRadius == NULL || ellipseInfo->yRadius == NULL)
				return NULL;
			glbFloat angle = 0.0;
			glbInt32 edges = ellipseInfo->edges->GetValue(feature);
			glbFloat xRadius = ellipseInfo->xRadius->GetValue(feature);
			glbFloat yRadius = ellipseInfo->yRadius->GetValue(feature);
			glbFloat angleDelta = 2.0f * osg::PI / (glbFloat) edges;
			double firstX = 0.0;
			double firstY = 0.0;
			for (glbInt32 i = 0; i < edges; ++i,angle += angleDelta)
			{
				glbFloat c = cosf(angle);
				glbFloat s = sinf(angle);
				if (i == 0)
				{
					firstX = c * xRadius;
					firstY = s * yRadius;
				}
				tempPoint = osg::Vec3d(c * xRadius,s * yRadius,0.0);
				if(GLB_GLOBETYPE_GLOBE == globeType)
				{
					tempPoint = localToWorld.preMult(tempPoint);
					g_ellipsoidModel->convertXYZToLatLongHeight(tempPoint.x(),
						tempPoint.y(),tempPoint.z(),tempPoint.y(),tempPoint.x(),tempPoint.z());
					line->AddPoint(osg::RadiansToDegrees(tempPoint.x()),
						osg::RadiansToDegrees(tempPoint.y()),tempPoint.z());
				}
				else if(GLB_GLOBETYPE_FLAT == globeType)
				{
					tempPoint = trans.preMult(tempPoint);
					line->AddPoint(tempPoint.x(),tempPoint.y(),tempPoint.z());
				}
			}
			tempPoint = osg::Vec3d(firstX,firstY,0.0);
			if(GLB_GLOBETYPE_GLOBE == globeType)
			{
				tempPoint = localToWorld.preMult(tempPoint);
				g_ellipsoidModel->convertXYZToLatLongHeight(tempPoint.x(),
					tempPoint.y(),tempPoint.z(),tempPoint.y(),tempPoint.x(),tempPoint.z());
				line->AddPoint(osg::RadiansToDegrees(tempPoint.x()),
					osg::RadiansToDegrees(tempPoint.y()),tempPoint.z());
			}
			else if(GLB_GLOBETYPE_FLAT == globeType)
			{
				tempPoint = trans.preMult(tempPoint);
				line->AddPoint(tempPoint.x(),tempPoint.y(),tempPoint.z());
			}
		}
	}
	else if (shapeInfo->shapeType == GLB_MARKERSHAPE_ARC)
	{
		GlbArcInfo *arcInfo = 
			static_cast<GlbArcInfo *>(shapeInfo);

		if (arcInfo)
		{
			if(arcInfo->edges == NULL || arcInfo->xRadius == NULL || 
				arcInfo->yRadius == NULL || arcInfo->sAngle == NULL || arcInfo->eAngle == NULL)
				return NULL;
			glbInt32 edges = arcInfo->edges->GetValue(feature);
			glbFloat xRadius = arcInfo->xRadius->GetValue(feature);
			glbFloat yRadius = arcInfo->yRadius->GetValue(feature);
			glbFloat sAngle = osg::DegreesToRadians(arcInfo->sAngle->GetValue(feature));
			glbFloat eAngle = osg::DegreesToRadians(arcInfo->eAngle->GetValue(feature));
			glbFloat angle = sAngle;
			glbFloat angleDelta = (eAngle - sAngle) / (glbFloat)edges;
			double firstX = 0.0;
			double firstY = 0.0;
			for (glbInt32 i = 0; i <= edges; ++i,angle += angleDelta)
			{
				glbFloat c = cosf(angle);
				glbFloat s = sinf(angle);
				if (i == 0)
				{
					if(arcInfo->isFan)
					{
						if (arcInfo->isFan->GetValue(feature))
						{
							firstX = 0.0;
							firstY = 0.0;
							tempPoint = osg::Vec3d(firstX,firstY,0.0);
							if(GLB_GLOBETYPE_GLOBE == globeType)
							{
								tempPoint = localToWorld.preMult(tempPoint);
								g_ellipsoidModel->convertXYZToLatLongHeight(tempPoint.x(),
									tempPoint.y(),tempPoint.z(),tempPoint.y(),tempPoint.x(),tempPoint.z());
								line->AddPoint(osg::RadiansToDegrees(tempPoint.x()),
									osg::RadiansToDegrees(tempPoint.y()),tempPoint.z());
							}
							else if(GLB_GLOBETYPE_FLAT == globeType)
							{
								tempPoint = trans.preMult(tempPoint);
								line->AddPoint(tempPoint.x(),tempPoint.y(),tempPoint.z());
							}
						}
						else
						{
							firstX = c * xRadius;
							firstY = s * yRadius;
						}
					}
				}
				tempPoint = osg::Vec3d(c * xRadius,s * yRadius,0.0);
				if(GLB_GLOBETYPE_GLOBE == globeType)
				{
					tempPoint = localToWorld.preMult(tempPoint);
					g_ellipsoidModel->convertXYZToLatLongHeight(tempPoint.x(),
						tempPoint.y(),tempPoint.z(),tempPoint.y(),tempPoint.x(),tempPoint.z());
					line->AddPoint(osg::RadiansToDegrees(tempPoint.x()),
						osg::RadiansToDegrees(tempPoint.y()),tempPoint.z());
				}
				else if(GLB_GLOBETYPE_FLAT == globeType)
				{
					tempPoint = trans.preMult(tempPoint);
					line->AddPoint(tempPoint.x(),tempPoint.y(),tempPoint.z());
				}
			}
			tempPoint = osg::Vec3d(firstX,firstY,0.0);
			if(GLB_GLOBETYPE_GLOBE == globeType)
			{
				tempPoint = localToWorld.preMult(tempPoint);
				g_ellipsoidModel->convertXYZToLatLongHeight(tempPoint.x(),
					tempPoint.y(),tempPoint.z(),tempPoint.y(),tempPoint.x(),tempPoint.z());
				line->AddPoint(osg::RadiansToDegrees(tempPoint.x()),
					osg::RadiansToDegrees(tempPoint.y()),tempPoint.z());
			}
			else if(GLB_GLOBETYPE_FLAT == globeType)
			{
				tempPoint = trans.preMult(tempPoint);
				line->AddPoint(tempPoint.x(),tempPoint.y(),tempPoint.z());
			}
		}
	}

	return line;
}

IGlbGeometry * GlbGlobe::CGlbGlobeMarker2DShapeSymbol::GetNoRotOutline( CGlbGlobeRObject *obj,IGlbGeometry *geo )
{
	if(obj->GetAltitudeMode() != GLB_ALTITUDEMODE_ONTERRAIN)
		return NULL;

	GlbRenderInfo *renderInfo = obj->GetRenderInfo();
	if(renderInfo == NULL)
		return NULL;
	GlbMarker2DShapeSymbolInfo *marker2DInfo = 
		static_cast<GlbMarker2DShapeSymbolInfo *>(renderInfo);
	CGlbFeature *feature = obj->GetFeature(); 

	IGlbGeometry *noRotGeom = NULL;
	osg::ref_ptr<osg::Vec2dArray> geoPoints = new osg::Vec2dArray();
	GlbGeometryTypeEnum geoType = geo->GetType();
	glbDouble ptx,pty;
	switch(geoType)
	{
	case GLB_GEO_POINT:	
		{
			CGlbPoint* pt = dynamic_cast<CGlbPoint *>(geo);
			pt->GetXY(&ptx,&pty);
			geoPoints->push_back(osg::Vec2d(ptx,pty));
		}
		break;
	case GLB_GEO_MULTIPOINT:
		{
			CGlbMultiPoint* pts = dynamic_cast<CGlbMultiPoint*>(geo);
			glbInt32 ptCnt = pts->GetCount();
			for (glbInt32 i = 0; i < ptCnt; i++)
			{
				pts->GetPoint(i,&ptx,&pty);
				geoPoints->push_back(osg::Vec2d(ptx,pty));
			}				
		}
		break;	
	}

	int ptCnt = geoPoints->size();
	if (ptCnt==1)
	{
		noRotGeom = DealNoRotOutline(geoPoints->at(0),0.0,marker2DInfo,feature,obj->GetGlobe()->GetType());
	}
	else if (ptCnt>1)
	{
		CGlbMultiLine* multiln = NULL;
		for (glbInt32 k = 0; k < ptCnt; k++)
		{
			glbref_ptr<IGlbGeometry> geom = DealNoRotOutline(geoPoints->at(k),0.0,marker2DInfo,feature,obj->GetGlobe()->GetType());
			if (geom)
			{
				geoType = geom->GetType();
				if (geoType==GLB_GEO_LINE)
				{
					CGlbLine* ln = dynamic_cast<CGlbLine*>(geom.get());
					if (ln)
					{
						if (!multiln) multiln = new CGlbMultiLine();
						multiln->AddLine(ln);
					}
				}					
			}
		}
		noRotGeom = multiln;
	}

	return noRotGeom;
}
