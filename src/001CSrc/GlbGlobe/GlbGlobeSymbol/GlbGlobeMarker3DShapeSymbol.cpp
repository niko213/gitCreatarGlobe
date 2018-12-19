#include "StdAfx.h"
#include "GlbGlobeMarker3DShapeSymbol.h"
#include "GlbPoint.h"
#include "osg/Geode"
#include "osgDB/ReadFile"
#include "osg/Material"
#include "osg/Texture2D"
#include "osg/TexMat"
#include "osg/LineWidth"
#include "osg/LineStipple"
#include "osg/PolygonMode"
#include "osg/PolygonOffset"
#include "osg/CullFace"
#include "GlbString.h"
#include "GlbGlobeCustomShape.h"
#include "GlbGlobeCustomShapeDrawable.h"
#include <osg/ImageStream>

using namespace GlbGlobe;

CGlbGlobeMarker3DShapeSymbol::CGlbGlobeMarker3DShapeSymbol(void)
{
}

CGlbGlobeMarker3DShapeSymbol::~CGlbGlobeMarker3DShapeSymbol(void)
{
}

osg::Node * GlbGlobe::CGlbGlobeMarker3DShapeSymbol::Draw( CGlbGlobeRObject *obj,IGlbGeometry *geo )
{
	GlbRenderInfo *renderInfo = obj->GetRenderInfo();
	GlbMarker3DShapeSymbolInfo *marker3DInfo = 
		static_cast<GlbMarker3DShapeSymbolInfo *>(renderInfo);

	if(marker3DInfo == NULL)
		return NULL;

	double xOrLon = 0.0,yOrLat = 0.0,zOrAlt = 0.0;
	//IGlbGeometry *geometry = obj->GetGeometry();
	//CGlbPoint3D *point3D = dynamic_cast<CGlbPoint3D*>(geometry);
	//if (point3D)
	//	point3D->GetXYZ(&xOrLon,&yOrLat,&zOrAlt);
	osg::Vec3d centerPoint(0.0,0.0,0.0);
	CGlbFeature *feature = obj->GetFeature();
	osg::Geode *geode = new osg::Geode;

	if(marker3DInfo->shapeInfo == NULL)
		return NULL;

	ref_ptr<osg::ShapeDrawable> shapeFillDrawable = NULL;
	ref_ptr<osg::ShapeDrawable> shapeOutDrawable = NULL;
	if (marker3DInfo->shapeInfo->shapeType == GLB_MARKERSHAPE_SPHERE)
	{
		GlbSphereInfo *sphereInfo = 
			static_cast<GlbSphereInfo *>(marker3DInfo->shapeInfo);
		if (sphereInfo)
		{
			ref_ptr<CustomSphere> sphere = new CustomSphere;
			if(sphereInfo->radius)
				sphere->set(centerPoint,sphereInfo->radius->GetValue(feature));
			osg::ref_ptr<osg::TessellationHints> hints = new osg::TessellationHints();
			hints->setDetailRatio(0.3);
			shapeFillDrawable = new GlobeShapeDrawable(sphere,hints);
			shapeOutDrawable = 	new GlobeShapeDrawable(sphere,hints);
			DealDraw(sphere,shapeFillDrawable,shapeOutDrawable,marker3DInfo,feature);
			geode->addDrawable(shapeFillDrawable);
			geode->addDrawable(shapeOutDrawable);
		}
	}
	else if (marker3DInfo->shapeInfo->shapeType == GLB_MARKERSHAPE_BOX)
	{
		GlbBoxInfo *boxInfo = 
			static_cast<GlbBoxInfo *>(marker3DInfo->shapeInfo);
		if (boxInfo)
		{
			ref_ptr<CustomBox/*osg::Box*/> box = NULL;
			if(boxInfo->length && boxInfo->width && boxInfo->height)
				box = new CustomBox/*osg::Box*/(centerPoint,boxInfo->length->GetValue(feature),
				boxInfo->width->GetValue(feature),boxInfo->height->GetValue(feature));
			if(box)
			{
				shapeFillDrawable = new GlobeShapeDrawable/*osg::ShapeDrawable*/(box);
				shapeOutDrawable = new GlobeShapeDrawable/*osg::ShapeDrawable*/(box);
				DealDraw(box,shapeFillDrawable,shapeOutDrawable,marker3DInfo,feature);
				geode->addDrawable(shapeFillDrawable);
				geode->addDrawable(shapeOutDrawable);
			}
		}
	}
	else if (marker3DInfo->shapeInfo->shapeType == GLB_MARKERSHAPE_CONE)
	{
		GlbConeInfo *coneInfo = 
			static_cast<GlbConeInfo *>(marker3DInfo->shapeInfo);
		if (coneInfo)
		{
			ref_ptr<CustomCone> cone = NULL;
			if(coneInfo->edges && coneInfo->radius && coneInfo->height)
				cone = new CustomCone(centerPoint,coneInfo->radius->GetValue(feature),
				coneInfo->height->GetValue(feature),coneInfo->edges->GetValue(feature));
			if(cone)
			{
				osg::ref_ptr<osg::TessellationHints> hints = new osg::TessellationHints();
				hints->setDetailRatio(1.0);
				shapeFillDrawable = new GlobeShapeDrawable(cone,hints);
				shapeOutDrawable = new GlobeShapeDrawable(cone,hints);
				DealDraw(cone,shapeFillDrawable,shapeOutDrawable,marker3DInfo,feature);
				geode->addDrawable(shapeFillDrawable);
				geode->addDrawable(shapeOutDrawable);
			}
		}
	}
	else if (marker3DInfo->shapeInfo->shapeType == GLB_MARKERSHAPE_CYLINDER)
	{
		GlbCylinderInfo *cylinderInfo = 
			static_cast<GlbCylinderInfo *>(marker3DInfo->shapeInfo);
		if (cylinderInfo)
		{
			ref_ptr<CustomCylinder> cylinder = NULL;
			if(cylinderInfo->edges && cylinderInfo->radius && cylinderInfo->height)
				cylinder = new CustomCylinder(centerPoint,cylinderInfo->radius->GetValue(feature),
				cylinderInfo->height->GetValue(feature),cylinderInfo->edges->GetValue(feature));
			if(cylinder)
			{
				shapeFillDrawable = new GlobeShapeDrawable(cylinder);
				shapeOutDrawable = new GlobeShapeDrawable(cylinder);
				DealDraw(cylinder,shapeFillDrawable,shapeOutDrawable,marker3DInfo,feature);
				geode->addDrawable(shapeFillDrawable);
				geode->addDrawable(shapeOutDrawable);
			}
		}
	}
	else if (marker3DInfo->shapeInfo->shapeType == GLB_MARKERSHAPE_PYRAMID)
	{
		GlbPyramidInfo *pyramidInfo = 
			static_cast<GlbPyramidInfo *>(marker3DInfo->shapeInfo);
		if (pyramidInfo)
		{
			ref_ptr<CustomPyramid> pyramid = NULL;
			if(pyramidInfo->length && pyramidInfo->width && pyramidInfo->height)
				pyramid = new CustomPyramid(centerPoint,pyramidInfo->length->GetValue(feature),
				pyramidInfo->width->GetValue(feature),pyramidInfo->height->GetValue(feature));
			if(pyramid)
			{
				shapeFillDrawable = new GlobeShapeDrawable(pyramid);
				shapeOutDrawable = new GlobeShapeDrawable(pyramid);
				DealDraw(pyramid,shapeFillDrawable,shapeOutDrawable,marker3DInfo,feature);
				geode->addDrawable(shapeFillDrawable);
				geode->addDrawable(shapeOutDrawable);
			}
		}
	}
	else if (marker3DInfo->shapeInfo->shapeType == GLB_MARKERSHAPE_PIE)
	{
		GlbPieInfo *pieInfo = 
			static_cast<GlbPieInfo *>(marker3DInfo->shapeInfo);

		if (pieInfo)
		{
			ref_ptr<CustomPie> pie = NULL;
			if(pieInfo->sAngle && pieInfo->eAngle && pieInfo->radius && pieInfo->edges && pieInfo->height)
				pie = new CustomPie(centerPoint,pieInfo->radius->GetValue(feature),
				pieInfo->height->GetValue(feature),pieInfo->edges->GetValue(feature),
				osg::DegreesToRadians(pieInfo->sAngle->GetValue(feature)),
				osg::DegreesToRadians(pieInfo->eAngle->GetValue(feature)));
			if(pie)
			{
				shapeFillDrawable = new GlobeShapeDrawable(pie);
				shapeOutDrawable = new GlobeShapeDrawable(pie);
				DealDraw(pie,shapeFillDrawable,shapeOutDrawable,marker3DInfo,feature);
				geode->addDrawable(shapeFillDrawable);
				geode->addDrawable(shapeOutDrawable);
			}
		}
	}

	//设置背部剔除看不见背面东西
	osg::ref_ptr<osg::CullFace> cullface=new osg::CullFace(osg::CullFace::BACK);
	geode->getOrCreateStateSet()->setAttribute(cullface.get());
	geode->getOrCreateStateSet()->setMode(GL_CULL_FACE,osg::StateAttribute::ON);

	return geode;
}

void GlbGlobe::CGlbGlobeMarker3DShapeSymbol::DealDraw( osg::Shape *shape, 
	osg::ShapeDrawable *shapeFillDrawable, osg::ShapeDrawable *shapeOutDrawable,
	GlbMarker3DShapeSymbolInfo *marker3DInfo, CGlbFeature *feature )
{
	if(shape == NULL)
		return;
	osg::ref_ptr<osg::StateSet> fillStateSet = shapeFillDrawable->getOrCreateStateSet();
	osg::ref_ptr<osg::Material> fillMaterial = 
		dynamic_cast<osg::Material*>(fillStateSet->getAttribute(osg::StateAttribute::MATERIAL));
	if (!fillMaterial)
		fillMaterial = new osg::Material;

	fillStateSet->setAttribute(fillMaterial,osg::StateAttribute::ON);	
	fillStateSet->setMode(GL_DEPTH_TEST,osg::StateAttribute::ON);
	
	if(marker3DInfo->fillInfo && marker3DInfo->fillInfo->color)
	{
		fillMaterial->setDiffuse(osg::Material::FRONT_AND_BACK,
			GetColor(marker3DInfo->fillInfo->color->GetValue(feature)));
	}
	if(marker3DInfo->fillInfo && marker3DInfo->fillInfo->opacity)
	{
		glbInt32 opacity = marker3DInfo->fillInfo->opacity->GetValue(feature);
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

	osg::Matrix mScale;
	osg::Matrix mRotate;
	double tilingU = 1.0;
	double tilingV = 1.0;

	osg::ref_ptr<osg::Image> image = NULL;
	if(marker3DInfo->fillInfo && marker3DInfo->fillInfo->textureData)
	{
		CGlbWString ws(marker3DInfo->fillInfo->textureData->GetValue(feature));
		glbInt32 index = ws.find_first_of(L'.');
		if(index == 0)
		{
			CGlbWString execDir = CGlbPath::GetExecDir();
			ws = execDir + ws.substr(1,ws.size());
		}
		image = osgDB::readImageFile(ws.ToString());
		//image->setAllocationMode(osg::Image::AllocationMode::NO_DELETE);

		osg::ImageStream* imagestream = dynamic_cast<osg::ImageStream*>(image.get());
		if (imagestream) 
			imagestream->play();
	}

	glbDouble scalem = 1.0;
	if (image)
	{
		texture->setImage(image);
		//texture->setDataVariance(osg::Object::DataVariance::STATIC);
		//texture->setUnRefImageDataAfterApply(true);
		//texture->setClientStorageHint(true);

		if(marker3DInfo->fillInfo && marker3DInfo->fillInfo->texRepeatMode)
		{
			if (marker3DInfo->fillInfo->texRepeatMode->GetValue(feature)
				== GLB_TEXTUREREPEAT_TIMES)
			{
				if(marker3DInfo->fillInfo->tilingU)
					tilingU = marker3DInfo->fillInfo->tilingU->GetValue(feature);
				if(marker3DInfo->fillInfo->tilingV)
					tilingV = marker3DInfo->fillInfo->tilingV->GetValue(feature);
			}
			else if (marker3DInfo->fillInfo->texRepeatMode->GetValue(feature)
				== GLB_TEXTUREREPEAT_SIZE)
			{
				osg::Sphere *sphere = dynamic_cast<osg::Sphere *>(shape);
				osg::Box *box = dynamic_cast<osg::Box *>(shape);
				CustomCone *cone = dynamic_cast<CustomCone *>(shape);
				CustomCylinder *cylinder = dynamic_cast<CustomCylinder *>(shape);
				CustomPyramid *pyramid = dynamic_cast<CustomPyramid *>(shape);
				CustomPie *pie = dynamic_cast<CustomPie *>(shape);
				if (sphere)
				{
					tilingU = 2 * sphere->getRadius() / image->s();
					tilingV = 2 * sphere->getRadius() / image->t();
				}
				else if (box)
				{
					Vec3d halfLengths = box->getHalfLengths();
					tilingU = 2 * halfLengths.x() / image->s();
					tilingV = 2 * halfLengths.y() / image->t();
				}
				else if (cone)
				{
					tilingU = 2 * cone->getRadius() / image->s();
					tilingV = 2 * cone->getRadius() / image->t();
				}
				else if (cylinder)
				{
					tilingU = 2 * cylinder->getRadius() / image->s();
					tilingV = 2 * cylinder->getRadius() / image->t();
				}
				else if (pyramid)
				{
					tilingU = pyramid->getWidth() / image->s();
					tilingV = pyramid->getLength() / image->t();
				}
				else if (pie)
				{
					tilingU = 2 * pie->getRadius() / image->s();
					tilingV = 2 * pie->getRadius() / image->t();
				}
			}
		}
		//mScale.makeScale(tilingU,tilingV,0.0);
		//if(marker3DInfo->fillInfo->textureRotation)
		//	mRotate.makeRotate(
		//	osg::DegreesToRadians(marker3DInfo->fillInfo->textureRotation->GetValue(feature)),
		//	osg::Vec3d(0,0,1));
		glbDouble textureRotation = 0.0;
		if(marker3DInfo->fillInfo && marker3DInfo->fillInfo->textureRotation)
			textureRotation = osg::DegreesToRadians(marker3DInfo->fillInfo->textureRotation->GetValue(feature));
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
	//texMat->setMatrix(osg::Matrix::translate(-0.5,-0.5,0) * mRotate * 
	//	osg::Matrix::translate(0.5,0.5,0) * mScale);
	texMat->setMatrix(osg::Matrix::translate(-0.5,-0.5,0) * mRotate * mScale * osg::Matrix::translate(0.5 * tilingU ,0.5 * tilingV,0));
	fillStateSet->setTextureAttributeAndModes(0,texMat,osg::StateAttribute::ON);

	osg::ref_ptr<osg::StateSet> outStateSet = shapeOutDrawable->getOrCreateStateSet();
	osg::ref_ptr<osg::Material> outMaterial = 
		dynamic_cast<osg::Material*>(outStateSet->getAttribute(osg::StateAttribute::MATERIAL));
	if(!outMaterial)
		outMaterial = new osg::Material;
	outStateSet->setAttribute(outMaterial,osg::StateAttribute::ON);
	outStateSet->setMode(GL_BLEND,osg::StateAttribute::ON);
	outStateSet->setMode(GL_DEPTH_TEST,osg::StateAttribute::ON);
	outStateSet->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
	osg::ref_ptr<osg::PolygonMode> polygonMode = new osg::PolygonMode();
	polygonMode->setMode( osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::LINE);
	outStateSet->setAttribute( polygonMode.get(), osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON );
	osg::ref_ptr<osg::PolygonOffset> poffset = new osg::PolygonOffset(-1,-1);
	outStateSet->setAttributeAndModes(poffset,osg::StateAttribute::ON);

	if(marker3DInfo->fillInfo && marker3DInfo->fillInfo->outlineInfo)
	{
		if(marker3DInfo->fillInfo->outlineInfo->lineColor)
		{
			outMaterial->setDiffuse(osg::Material::FRONT_AND_BACK,
				GetColor(marker3DInfo->fillInfo->outlineInfo->lineColor->GetValue(feature)));
		}
		if(marker3DInfo->fillInfo->outlineInfo->lineOpacity)
			outMaterial->setAlpha(osg::Material::FRONT_AND_BACK,
			marker3DInfo->fillInfo->outlineInfo->lineOpacity->GetValue(feature) / 100.0);
		if(marker3DInfo->fillInfo->outlineInfo->lineWidth)
		{
			ref_ptr<osg::LineWidth> lineWidth = new osg::LineWidth;
			lineWidth->setWidth(marker3DInfo->fillInfo->outlineInfo->lineWidth->GetValue(feature));
			outStateSet->setAttributeAndModes(lineWidth,osg::StateAttribute::ON);
		}
		if(marker3DInfo->fillInfo->outlineInfo->linePattern)
		{
			if (marker3DInfo->fillInfo->outlineInfo->linePattern->GetValue(feature) == GLB_LINE_DOTTED)
			{
				ref_ptr<osg::LineStipple> lineStipple = new osg::LineStipple;
				lineStipple->setFactor(1);
				lineStipple->setPattern(0x1C47);
				outStateSet->setAttributeAndModes(lineStipple,osg::StateAttribute::ON);
			}
		}
	}
}
