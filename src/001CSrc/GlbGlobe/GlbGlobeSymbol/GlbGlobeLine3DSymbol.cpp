#include "StdAfx.h"
#include "GlbGlobeLine3DSymbol.h"
#include "GlbLine.h"
#include <osg/Geode>
using namespace GlbGlobe;

CGlbGlobeLine3DSymbol::CGlbGlobeLine3DSymbol(void)
{
}


CGlbGlobeLine3DSymbol::~CGlbGlobeLine3DSymbol(void)
{
}

osg::Node* CGlbGlobeLine3DSymbol::Draw(CGlbGlobeRObject *obj,IGlbGeometry *geo)
{
	osg::Geode* geode = new osg::Geode;
	return geode;
}
