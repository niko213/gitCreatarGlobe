#include "StdAfx.h"
#include "GlbGlobeVirtualSectionSymbol.h"
#include "CGlbGlobe.h"
#include "GlbObject.h"
#include <osg/Switch>
#include <osg/LineWidth>
#include <osg/BlendColor>
#include <osg/BlendFunc>
#include <osg/TexMat>
#include <osg/LightModel>
#include <osg/PolygonOffset>
#include <osgUtil/Tessellator>
#include <osg/Material>

using namespace GlbGlobe;

CGlbGlobeVirtualSectionSymbol::CGlbGlobeVirtualSectionSymbol(void)
{
	mpr_prjPlaneEnum = GLB_PLANE_UNKNOWN;
}


CGlbGlobeVirtualSectionSymbol::~CGlbGlobeVirtualSectionSymbol(void)
{
}

osg::Node *CGlbGlobeVirtualSectionSymbol::Draw(CGlbGlobeRObject *obj,IGlbGeometry *geo)
{
	GlbRenderInfo *renderInfo = obj->GetRenderInfo();
	GlbSectionRenderInfo *sectionRenderInfo = 
		dynamic_cast<GlbSectionRenderInfo *>(renderInfo);
	if(NULL == sectionRenderInfo)
		return NULL;

	return NULL;
}