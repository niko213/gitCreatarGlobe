#include "StdAfx.h"
#include "GlbGlobeLine2DSymbol.h"
#include "GlbLine.h"
#include <osg/Geode>
using namespace GlbGlobe;

CGlbGlobeLine2DSymbol::CGlbGlobeLine2DSymbol(void)
{
}


CGlbGlobeLine2DSymbol::~CGlbGlobeLine2DSymbol(void)
{
}

osg::Node* CGlbGlobeLine2DSymbol::Draw(CGlbGlobeRObject *obj,IGlbGeometry *geo)
{
	osg::Geode* geode = new osg::Geode;
	return geode;
}
