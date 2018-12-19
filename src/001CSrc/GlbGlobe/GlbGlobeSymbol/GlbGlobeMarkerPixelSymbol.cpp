#include "StdAfx.h"
#include "GlbGlobeMarkerPixelSymbol.h"
#include "osg/Geode"
#include "osg/Geometry"
#include "osg/BlendColor"
#include "osg/Point"
#include "GlbPoint.h"
#include "osg/Material"
#include "GlbGlobeRenderSimpleLabelSymbol.h"

using namespace GlbGlobe;
using namespace osg;

CGlbGlobeMarkerPixelSymbol::CGlbGlobeMarkerPixelSymbol(void)
{
}

CGlbGlobeMarkerPixelSymbol::~CGlbGlobeMarkerPixelSymbol(void)
{
}

osg::Node * CGlbGlobeMarkerPixelSymbol::Draw( CGlbGlobeRObject *obj,IGlbGeometry *geo )
{
	GlbRenderInfo *renderInfo = obj->GetRenderInfo();
	GlbMarkerPixelSymbolInfo *markerPixelInfo = 
		dynamic_cast<GlbMarkerPixelSymbolInfo *>(renderInfo);

	if(markerPixelInfo == NULL)
		return NULL;

	osg::Group *group = new osg::Group;
	CGlbFeature *feature = obj->GetFeature();
	osg::Geode *geode = new osg::Geode;
	ref_ptr<osg::Geometry> pixelGeom = new osg::Geometry;
	osg::ref_ptr<osg::StateSet> stateset = geode->getOrCreateStateSet();

	osg::ref_ptr<osg::Material> material = dynamic_cast<osg::Material*>(stateset->getAttribute(osg::StateAttribute::MATERIAL));
	if(!material)
		material = new osg::Material;

	stateset->setAttribute(material,osg::StateAttribute::ON);
	stateset->setMode(GL_DEPTH_TEST,osg::StateAttribute::ON);
	//stateset->setRenderBinDetails( 0, "RenderBin");

	if(markerPixelInfo->color)
	{
		material->setEmission(osg::Material::FRONT_AND_BACK,
			GetColor(markerPixelInfo->color->GetValue(feature)));
		material->setDiffuse(osg::Material::FRONT_AND_BACK,osg::Vec4(0,0,0,0));
	}
	if(markerPixelInfo->opacity)
	{
		glbInt32 opacity = markerPixelInfo->opacity->GetValue(feature);
		material->setAlpha(osg::Material::FRONT_AND_BACK,opacity / 100.0);

		if (opacity<100)
		{
			stateset->setMode(GL_BLEND,osg::StateAttribute::ON);
			if (stateset->getRenderingHint() != osg::StateSet::TRANSPARENT_BIN)
				stateset->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
		}
		else
		{
			stateset->setMode(GL_BLEND,osg::StateAttribute::OFF);
			stateset->setRenderingHint(osg::StateSet::OPAQUE_BIN);
		}
	}
	else
	{
		stateset->setMode(GL_BLEND,osg::StateAttribute::OFF);
		stateset->setRenderingHint(osg::StateSet::OPAQUE_BIN);
	}

	if(markerPixelInfo->size)
	{
		ref_ptr<osg::Point> point = new osg::Point;
		double ptsz = markerPixelInfo->size->GetValue(feature);
		point->setSize(ptsz);
		stateset->setAttributeAndModes(point,osg::StateAttribute::ON);
	}

	stateset->setMode(GL_LIGHTING,osg::StateAttribute::ON);
	ref_ptr<osg::Vec3dArray> renderVertexes = new osg::Vec3dArray;
	renderVertexes->push_back(osg::Vec3d(0,0,0));
	pixelGeom->setVertexArray(renderVertexes);
	pixelGeom->addPrimitiveSet(
		new osg::DrawArrays(osg::PrimitiveSet::POINTS,0,renderVertexes->size()));
	geode->addDrawable(pixelGeom);
	group->addChild(geode);

	if(markerPixelInfo->label && markerPixelInfo->label->bShow 
		&& markerPixelInfo->label->bShow->GetValue(feature))
	{
		CGlbGlobeRenderSimpleLabelSymbol sl;
		glbDouble xx = 0.0,yy = 0.0,zz = 0.0;
		glbref_ptr<CGlbPoint> point3D = new CGlbPoint(0.0,0.0,0.0);
		osg::ref_ptr<osg::Node> labelNode = sl.Draw(markerPixelInfo->label,point3D.get(),feature,obj->GetGlobe());
		if(labelNode.valid())
			group->addChild(labelNode.get());
	}

	return group;
}
