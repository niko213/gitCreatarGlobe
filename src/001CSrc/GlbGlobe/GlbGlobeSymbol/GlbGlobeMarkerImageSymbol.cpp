#include "StdAfx.h"
#include "GlbGlobeMarkerImageSymbol.h"
#include "osg/Billboard"
#include "osg/BlendColor"
#include "osg/BlendFunc"
#include "osg/Texture2D"
#include "osgDB/ReadFile"
#include "GlbString.h"
#include "CGlbGlobe.h"
#include "GlbGlobeView.h"
#include "GlbPoint.h"
#include <osg/Material>
#include <osg/Depth>
#include <osg/ImageStream>

//#include "osg/AutoTransform"
#include <osg/PolygonMode>
#include "GlbLog.h"

using namespace GlbGlobe;
using namespace osg;

CGlbGlobeMarkerImageSymbol::CGlbGlobeMarkerImageSymbol(void)
{
}

CGlbGlobeMarkerImageSymbol::~CGlbGlobeMarkerImageSymbol(void)
{
}

osg::Node * CGlbGlobeMarkerImageSymbol::Draw( CGlbGlobeRObject *obj,IGlbGeometry *geo )
{
	GlbRenderInfo *renderInfo = obj->GetRenderInfo();
	GlbMarkerImageSymbolInfo *imageInfo = 
		static_cast<GlbMarkerImageSymbolInfo *>(renderInfo);
	if(imageInfo == NULL)
		return NULL;
	CGlbFeature *feature = obj->GetFeature();
	CGlbGlobe *globe = obj->GetGlobe();
	if(globe == NULL) return NULL;

	if (imageInfo->is360DegreePanorama != NULL)
	{
		glbBool is360DegreePanorama = imageInfo->is360DegreePanorama->GetValue(feature);
		if (is360DegreePanorama)
		{
			return Draw360DegreePanorama(obj,geo);
		}
	}

	//osg::Group *group = new osg::Group;
	osg::MatrixTransform *group = new osg::MatrixTransform;
	//像素尺寸 默认值为-1
	glbInt32 maxSize = 0;
	if(imageInfo->imgInfo && imageInfo->imgInfo->maxSize)
		maxSize = imageInfo->imgInfo->maxSize->GetValue(feature);
	if(0 == maxSize)
		return group;
	osg::ref_ptr<osg::Image> image = NULL;
	CGlbWString datalocate = L"";
	if(imageInfo->imgInfo && imageInfo->imgInfo->dataSource)
	{
		datalocate = imageInfo->imgInfo->dataSource->GetValue(feature);
		glbInt32 index = datalocate.find_first_of(L'.');
		if(index == 0)
		{
			CGlbWString execDir = CGlbPath::GetExecDir();
			datalocate = execDir + datalocate.substr(1,datalocate.size());
		}
		image = osgDB::readImageFile(datalocate.ToString());
		osg::ImageStream* imagestream = dynamic_cast<osg::ImageStream*>(image.get());
		if (imagestream) 
			imagestream->play();
	}
	if(!image)
	{
		GlbLogWOutput(GLB_LOGTYPE_ERR,L"读取图像文件(%s)失败. \r\n",datalocate.c_str());
		return NULL;
	}

	osg::ref_ptr<GlbGlobeAutoTransform> autoTransform = new GlbGlobeAutoTransform(globe);
	osg::ref_ptr<GlbGlobeAutoTransform> outAutoTransform = new GlbGlobeAutoTransform(globe);
	GlbGlobeLabelAlignTypeEnum labelAlign = GLB_LABELALG_LEFTBOTTOM;
	if(imageInfo->imageAlign)
		labelAlign = (GlbGlobeLabelAlignTypeEnum)imageInfo->imageAlign->GetValue(feature);

	GlbGlobeBillboardModeEnum bbMode = GLB_BILLBOARD_SCREEN;
	if(imageInfo->imgBillboard)
	{
		bbMode = (GlbGlobeBillboardModeEnum)imageInfo->imgBillboard->GetValue(feature);

		if (bbMode == GLB_BILLBOARD_SCREEN)
			autoTransform->setAutoRotateMode(osg::AutoTransform::ROTATE_TO_SCREEN);
		else if (bbMode == GLB_BILLBOARD_AXIS)
		{
			autoTransform->setAutoRotateMode(osg::AutoTransform::ROTATE_TO_AXIS);
			autoTransform->setAxis(osg::Vec3(0.0f,0.0f,1.0f));
			autoTransform->setNormal(osg::Vec3(0.0f,-1.0f,0.0f));
		}
		else if(GLB_BILLBOARD_AUTOSLOPE == bbMode)
		{
			autoTransform->setAutoRotateMode(osg::AutoTransform::ROTATE_TO_AXIS);
			autoTransform->setAxis(osg::Vec3(1.0f,0.0f,0.0f));
			autoTransform->setNormal(osg::Vec3(0.0f,-1.0f,0.0f));
		}
	}

	glbInt32 imageWidth = image->s();
	glbInt32 imageHeight = image->t();
	double pixelRatioW = 1.0;
	double pixelRatioH = 1.0;

	if(maxSize >= 0 && (maxSize != imageWidth || maxSize != imageHeight))
	{
		if(imageWidth >= imageHeight)
		{
			pixelRatioW = (double)maxSize / imageWidth;
			pixelRatioH = (double)maxSize / imageWidth;
		}
		else
		{
			pixelRatioW = (double)maxSize / imageHeight;
			pixelRatioH = (double)maxSize / imageHeight;
		}
	}

	ref_ptr<osg::StateSet> stateset = new osg::StateSet;
	ref_ptr<osg::Texture2D> texture = new osg::Texture2D;
	if (image)
	{
		texture->setImage(image);
		stateset->setTextureAttributeAndModes(0,texture,osg::StateAttribute::ON);
	}

	glbBool isLimit = false;
	glbDouble ratio = 1.0;
	if(imageInfo->geoInfo->isLimit)
		isLimit = imageInfo->geoInfo->isLimit->GetValue(feature);
	if(imageInfo->geoInfo->ratio)
		ratio = imageInfo->geoInfo->ratio->GetValue(feature);

	//创建几何体
	osg::ref_ptr<osg::Geometry> labelGeom = NULL;
	osg::Vec3 corner(0.0,0.0,0.0);//GLB_LABELALG_LEFTBOTTOM
	osg::Vec3 width,height;

	imageWidth = imageWidth * pixelRatioW;
	imageHeight = imageHeight * pixelRatioH;

	if(!isLimit)
	{
		width = osg::Vec3(imageWidth * ratio,0.0,0.0);
		height = osg::Vec3(0.0,imageHeight * ratio,0.0);
		corner = ComputeCornerByAlign(imageWidth,imageHeight,ratio,labelAlign);
		labelGeom = osg::createTexturedQuadGeometry(corner,width,height);
	}
	else
	{
		width = osg::Vec3(imageWidth/* * ratio*/,0.0,0.0);
		height = osg::Vec3(0.0,imageHeight/* * ratio*/,0.0);
		corner = ComputeCornerByAlign(imageWidth,imageHeight,1.0,labelAlign);
		labelGeom = osg::createTexturedQuadGeometry(corner,width,height);
		autoTransform->setAutoScaleToScreen(true);
		autoTransform->setMinimumScale(0.0);
		//if(-1 == maxSize)
		//{
		//	autoTransform->setMaximumScale(DBL_MAX);
		//}
		//else
			autoTransform->setMaximumScale(/*maxSize * */ratio);
	}

	glbInt32 opacity = 100;
	if(imageInfo->imgInfo->opacity)
		opacity = imageInfo->imgInfo->opacity->GetValue(feature);
	glbInt32 color = 0xFFFFFFFF;
	if(imageInfo->imgInfo->color)
		color = imageInfo->imgInfo->color->GetValue(feature);

	osg::ref_ptr<osg::Material> material = dynamic_cast<osg::Material *>(stateset->getAttribute(osg::StateAttribute::MATERIAL));
	if(material == NULL)
		material = new osg::Material;
	material->setEmission(osg::Material::FRONT_AND_BACK,GetColor(color));
	//material->setDiffuse(osg::Material::FRONT_AND_BACK,GetColor(color));
	material->setDiffuse(osg::Material::FRONT_AND_BACK,osg::Vec4(0,0,0,0));
	material->setAlpha(osg::Material::FRONT_AND_BACK,opacity / 100.0);
	stateset->setMode(GL_DEPTH_TEST,osg::StateAttribute::ON);

	osg::ref_ptr<osg::Depth> depth = dynamic_cast<osg::Depth *>(stateset->getAttribute(osg::StateAttribute::DEPTH));
	if(!depth.valid())
	{
		depth = new osg::Depth;
		depth->setWriteMask(false);
		stateset->setAttributeAndModes(depth.get(), osg::StateAttribute::ON);
	}
	else
	{
		if(depth->getWriteMask())
			depth->setWriteMask(false);
	}
	if (opacity<100)
	{
		stateset->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
		stateset->setMode(GL_BLEND,osg::StateAttribute::ON);
		osg::ref_ptr<osg::BlendFunc> pBlendFun = new osg::BlendFunc(osg::BlendFunc::/*ONE*/SRC_ALPHA,osg::BlendFunc::ONE_MINUS_SRC_ALPHA);	//设置混合方程
		stateset->setAttributeAndModes(pBlendFun.get(),osg::StateAttribute::ON);
	}	
	else
	{
		stateset->setRenderingHint(osg::StateSet::OPAQUE_BIN);
		stateset->setMode(GL_BLEND,osg::StateAttribute::OFF);
	}

	stateset->setAttribute(material,osg::StateAttribute::ON);
	//stateset->setMode(GL_LIGHTING,osg::StateAttribute::OFF);
	labelGeom->setStateSet(stateset);
	osg::ref_ptr<osg::Geode> geode = new osg::Geode;
	osg::ref_ptr<osg::Geode> outGeode = new osg::Geode;
	//内框
	osg::ref_ptr<osg::Geometry> lineGeometry = CreateOutlineQuad(corner,width,height);
	geode->addDrawable(labelGeom);
	//geode->addDrawable(lineGeometry);
	autoTransform->addChild(geode);
	group->addChild(autoTransform);

	////添加外框
	if(isLimit)
	{
		osg::ref_ptr<osg::Geometry> outlineGeometry = CreateOutlineQuad(corner,width,height);
		if(NULL == outlineGeometry) return group;
		//outGeode->addDrawable(outlineGeometry);
		outAutoTransform->addChild(outGeode);
		if(GLB_BILLBOARD_SCREEN == bbMode)
			outAutoTransform->setAutoRotateMode(osg::AutoTransform::ROTATE_TO_SCREEN);
		else if(GLB_BILLBOARD_AXIS == bbMode)
		{
			outAutoTransform->setAutoRotateMode(osg::AutoTransform::ROTATE_TO_AXIS);
			outAutoTransform->setAxis(osg::Vec3(0.0f,0.0f,1.0f));
			outAutoTransform->setNormal(osg::Vec3(0.0f,-1.0f,0.0f));
		}
		else if(GLB_BILLBOARD_AUTOSLOPE == bbMode)
		{
			outAutoTransform->setAutoRotateMode(osg::AutoTransform::ROTATE_TO_AXIS);
			outAutoTransform->setAxis(osg::Vec3(1.0f,0.0f,0.0f));
			outAutoTransform->setNormal(osg::Vec3(0.0f,-1.0f,0.0f));
		}
		outAutoTransform->setAutoScaleToScreen(true);
		outAutoTransform->setMinimumScale(DBL_MAX);
		outAutoTransform->setMaximumScale(/*maxSize * */ratio);
		group->addChild(outAutoTransform);
	}

	glbDouble xOffset = 0.0,yOffset = 0.0,zOffset = 0.0;
	if(imageInfo->xOffset && imageInfo->yOffset && imageInfo->zOffset)
	{
		xOffset = imageInfo->xOffset->GetValue(feature);
		yOffset = imageInfo->yOffset->GetValue(feature);
		zOffset = imageInfo->zOffset->GetValue(feature);
		osg::Matrix m;
		m.makeTranslate(xOffset,yOffset,zOffset);
		group->setMatrix(m);
	}

	return group;
}

osg::Geometry * GlbGlobe::CGlbGlobeMarkerImageSymbol::CreateOutlineQuad( osg::Vec3 cornerVec,osg::Vec3 widthVec,osg::Vec3 heightVec )
{
	osg::Geometry *geom = new osg::Geometry;
	osg::ref_ptr<osg::StateSet> geomState = geom->getOrCreateStateSet();
	//geomState->setMode(GL_LIGHTING,osg::StateAttribute::ON | osg::StateAttribute::PROTECTED);
	osg::ref_ptr<osg::Material> geomMaterial = 
		dynamic_cast<osg::Material *>(geomState->getAttribute(osg::StateAttribute::MATERIAL));
	if(geomMaterial == NULL)
		geomMaterial = new osg::Material;
	geomMaterial->setEmission(osg::Material::FRONT_AND_BACK,osg::Vec4(1,1,0,1));
	geomMaterial->setDiffuse(osg::Material::FRONT_AND_BACK,osg::Vec4(0,0,0,0));
	geomState->setAttribute(geomMaterial,osg::StateAttribute::ON | osg::StateAttribute::PROTECTED);
	osg::ref_ptr<osg::Vec3dArray> vertexArray = new osg::Vec3dArray(4);
	(*vertexArray)[0] = cornerVec + heightVec;
	(*vertexArray)[1] = cornerVec;
	(*vertexArray)[2] = cornerVec + widthVec;
	(*vertexArray)[3] = cornerVec + widthVec + heightVec;
	geom->setVertexArray(vertexArray);
	geom->setColorBinding(osg::Geometry::BIND_OVERALL);
	osg::ref_ptr<osg::Vec3dArray> normals = new osg::Vec3dArray(1);
	(*normals)[0] = widthVec ^ heightVec;
	(*normals)[0].normalize();
	geom->setNormalArray(normals);
	geom->setNormalBinding(Geometry::BIND_OVERALL);
	geom->addPrimitiveSet(new DrawArrays(PrimitiveSet::LINE_LOOP,0,4));
	return geom;
}

osg::Node* CGlbGlobeMarkerImageSymbol::Draw360DegreePanorama(CGlbGlobeRObject *obj,IGlbGeometry *geo)
{
	GlbRenderInfo *renderInfo = obj->GetRenderInfo();
	GlbMarkerImageSymbolInfo *imageInfo = 
		static_cast<GlbMarkerImageSymbolInfo *>(renderInfo);
	if(imageInfo == NULL)
		return NULL;
	CGlbFeature *feature = obj->GetFeature();
	CGlbGlobe *globe = obj->GetGlobe();
	if(globe == NULL) return NULL;

	//像素尺寸 默认值为-1
	glbInt32 maxSize = 0;
	if(imageInfo->imgInfo && imageInfo->imgInfo->maxSize)
		maxSize = imageInfo->imgInfo->maxSize->GetValue(feature);
	if (maxSize<=0)	return NULL;

	osg::ref_ptr<osg::Image> image = NULL;
	CGlbWString datalocate = L"";
	if(imageInfo->imgInfo && imageInfo->imgInfo->dataSource)
	{
		datalocate = imageInfo->imgInfo->dataSource->GetValue(feature);
		glbInt32 index = datalocate.find_first_of(L'.');
		if(index == 0)
		{
			CGlbWString execDir = CGlbPath::GetExecDir();
			datalocate = execDir + datalocate.substr(1,datalocate.size());
		}
		image = osgDB::readImageFile(datalocate.ToString());
		osg::ImageStream* imagestream = dynamic_cast<osg::ImageStream*>(image.get());
		if (imagestream) 
			imagestream->play();
	}
	if(!image)
	{
		GlbLogWOutput(GLB_LOGTYPE_ERR,L"读取图像文件(%s)失败. \r\n",datalocate.c_str());
		return NULL;
	}
	if (image==NULL) return NULL;

	glbInt32 imageWidth = image->s();
	glbInt32 imageHeight = image->t();
	double pixelRatioW = 1.0;
	double pixelRatioH = 1.0;

	if(maxSize >= 0 && (maxSize != imageWidth || maxSize != imageHeight))
	{
		if(imageWidth >= imageHeight)
		{
			pixelRatioW = (double)maxSize / imageWidth;
			pixelRatioH = (double)maxSize / imageWidth;
		}
		else
		{
			pixelRatioW = (double)maxSize / imageHeight;
			pixelRatioH = (double)maxSize / imageHeight;
		}
	}
	glbBool isLimit = false;
	glbDouble ratio = 1.0;
	if(imageInfo->geoInfo->isLimit)
		isLimit = imageInfo->geoInfo->isLimit->GetValue(feature);
	if(imageInfo->geoInfo->ratio)
		ratio = imageInfo->geoInfo->ratio->GetValue(feature);

	double width,height;
	
	imageWidth = imageWidth * pixelRatioW;
	imageHeight = imageHeight * pixelRatioH;
	osg::ref_ptr<GlbGlobeAutoTransform> autoTransform = new GlbGlobeAutoTransform(globe);
	autoTransform->setAutoRotateMode(osg::AutoTransform::AutoRotateMode::NO_ROTATION);
	if(!isLimit)
	{
		width = imageWidth * ratio;
		height = imageHeight * ratio;		
	}
	else
	{
		width = imageWidth;
		height = imageHeight;

		autoTransform->setAutoScaleToScreen(true);
		autoTransform->setMinimumScale(0.0);
		autoTransform->setMaximumScale(/*maxSize * */ratio);
	}

	// 全景图半径 : 计算原理： 推测全景图长度的一半正好是周长的一半，周长perimeter=2*PI*Radius
	double radius = width * 0.5 / osg::PI;

	//int width = _img->s();
	//int height = _img->t();
	osg::Geometry* geom = new osg::Geometry;

	osg::ref_ptr<osg::Vec3Array> points = new osg::Vec3Array;
	osg::ref_ptr<osg::Vec2Array> coords = new osg::Vec2Array;
	osg::ref_ptr<osg::Vec3Array> norms = new osg::Vec3Array;
	double segDegrees = 5;
	double theta;
	double phi,phi2;
	double x,y,z,x2,y2,z2;
	double u,v,u2,v2;
	int startidx = 0;
	int cnt = 0;
	for (int i = 0; i < 36; i++)
	{// i 纬度
		cnt = 0;
		phi = osg::DegreesToRadians(90 - segDegrees*i);
		for (int j = 0; j <= 72; j++)
		{// j 经度
			theta = osg::DegreesToRadians(-180 + j * segDegrees);

			x = cos(phi) * sin(theta);  
			y = cos(phi) * cos(theta);  
			z = sin(phi); 

			// 计算点在panoramic image的u,v坐标
			u = theta / (2 * osg::PI) + 0.5;
			v = phi / osg::PI + 0.5;

			phi2 = osg::DegreesToRadians(90 - segDegrees*(i+1));			
			x2 = cos(phi2) * sin(theta);  
			y2 = cos(phi2) * cos(theta);  
			z2 = sin(phi2); 

			// 计算点在panoramic image的u,v坐标
			u2 = theta / (2 * osg::PI) + 0.5;
			v2 = phi2 / osg::PI + 0.5;

			points->push_back(osg::Vec3(radius*x,radius*y,radius*z));
			points->push_back(osg::Vec3(radius*x2,radius*y2,radius*z2));

			norms->push_back(osg::Vec3(x,y,z));
			norms->push_back(osg::Vec3(x2,y2,z2));			

			coords->push_back(osg::Vec2(u,v));
			coords->push_back(osg::Vec2(u2,v2));
			cnt += 2;
		}
		//geom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUAD_STRIP,startidx,cnt));
		//startidx += cnt;
	}

	geom->setVertexArray(points.get());
	geom->setTexCoordArray(0,coords.get());
	geom->setNormalArray(norms.get());
	geom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUAD_STRIP,0,points->size()));

	osg::ref_ptr<osg::Texture2D> _tex = new osg::Texture2D;			
	_tex->setImage(image.get());

	_tex->setFilter(osg::Texture::MIN_FILTER,osg::Texture::LINEAR_MIPMAP_LINEAR);
	_tex->setFilter(osg::Texture::MAG_FILTER,osg::Texture::LINEAR);
	//mpr_p_texture->setFilter(osg::Texture::MAG_FILTER,osg::Texture::LINEAR);
	_tex->setWrap(osg::Texture::WRAP_S,osg::Texture::CLAMP_TO_EDGE);
	_tex->setWrap(osg::Texture::WRAP_T,osg::Texture::CLAMP_TO_EDGE);

	_tex->setMaxAnisotropy(8.0);
	// 自动释放image占用的内存数据 2017.5.15
	// mpr_texture->setUnRefImageDataAfterApply(true);

	_tex->setResizeNonPowerOfTwoHint(false);
	geom->getOrCreateStateSet()->setTextureAttributeAndModes(0,_tex.get());	

	glbInt32 opacity = 100;
	if(imageInfo->imgInfo->opacity)
		opacity = imageInfo->imgInfo->opacity->GetValue(feature);
	glbInt32 color = 0xFFFFFFFF;
	if(imageInfo->imgInfo->color)
		color = imageInfo->imgInfo->color->GetValue(feature);

	ref_ptr<osg::StateSet> stateset = geom->getOrCreateStateSet();
	osg::ref_ptr<osg::Material> material = dynamic_cast<osg::Material *>(stateset->getAttribute(osg::StateAttribute::MATERIAL));
	if(material == NULL)
		material = new osg::Material;
	material->setEmission(osg::Material::FRONT_AND_BACK,GetColor(color));	
	material->setDiffuse(osg::Material::FRONT_AND_BACK,osg::Vec4(0,0,0,0));
	material->setAlpha(osg::Material::FRONT_AND_BACK,opacity / 100.0);
	if (opacity<100)
	{
		stateset->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
		stateset->setMode(GL_BLEND,osg::StateAttribute::ON);
		osg::ref_ptr<osg::BlendFunc> pBlendFun = new osg::BlendFunc(osg::BlendFunc::/*ONE*/SRC_ALPHA,osg::BlendFunc::ONE_MINUS_SRC_ALPHA);	//设置混合方程
		stateset->setAttributeAndModes(pBlendFun.get(),osg::StateAttribute::ON);
	}	
	else
	{
		stateset->setRenderingHint(osg::StateSet::OPAQUE_BIN);
		stateset->setMode(GL_BLEND,osg::StateAttribute::OFF);
	}
	stateset->setAttribute(material,osg::StateAttribute::ON);

	//stateset->setMode(GL_LIGHTING, osg::StateAttribute::OFF);

	osg::Geode* geode = new osg::Geode;
	geode->addDrawable(geom);	

	osg::MatrixTransform *group = new osg::MatrixTransform;
	group->addChild(autoTransform);
	autoTransform->addChild(geode);
	return group;
}
