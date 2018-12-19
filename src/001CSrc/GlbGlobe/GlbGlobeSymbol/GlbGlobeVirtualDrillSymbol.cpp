#include "StdAfx.h"
#include "GlbGlobeVirtualDrillSymbol.h"
#include "GlbLine.h"
#include "CGlbGlobe.h"
#include <osg/Switch>
#include <osg/LineWidth>
#include <osg/BlendColor>
#include <osg/BlendFunc>
#include <osg/Material>
#include <osg/LightModel>

using namespace GlbGlobe;

CGlbGlobeVirtualDrillSymbol::CGlbGlobeVirtualDrillSymbol(void)
{
}

CGlbGlobeVirtualDrillSymbol::~CGlbGlobeVirtualDrillSymbol(void)
{
}

osg::Node *CGlbGlobeVirtualDrillSymbol::Draw(CGlbGlobeRObject *obj,IGlbGeometry *geo)
{
	return NULL;
}