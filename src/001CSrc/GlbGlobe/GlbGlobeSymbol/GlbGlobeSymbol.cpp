// GlbGlobeSymbol.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "GlbGlobeSymbol.h"
using namespace GlbGlobe;

CGlbGlobeSymbol::CGlbGlobeSymbol(void)
{
	mpt_outline = NULL;
}


CGlbGlobeSymbol::~CGlbGlobeSymbol(void)
{
}

osg::Node * CGlbGlobeSymbol::Draw( CGlbGlobeRObject *obj,IGlbGeometry *geo)
{
	return NULL;
}

void CGlbGlobeSymbol::DrawToImage( CGlbGlobeRObject *obj,IGlbGeometry* geom,
	glbByte *image/*RGBA*/, glbInt32 imageW, 
	glbInt32 imageH, CGlbExtent &ext )
{

}

IGlbGeometry * CGlbGlobeSymbol::GetOutline(CGlbGlobeRObject *obj,IGlbGeometry *geo)
{
	return mpt_outline.get();
}
