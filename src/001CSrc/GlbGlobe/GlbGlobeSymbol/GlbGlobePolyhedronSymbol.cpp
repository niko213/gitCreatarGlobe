#include "StdAfx.h"
#include "GlbGlobePolyhedronSymbol.h"


namespace GlbGlobe
{
	CGlbGlobePolyhedronSymbol::CGlbGlobePolyhedronSymbol(void)
	{
	}

	CGlbGlobePolyhedronSymbol::~CGlbGlobePolyhedronSymbol(void)
	{
	}

	osg::Node * CGlbGlobePolyhedronSymbol::Draw( CGlbGlobeRObject *obj,IGlbGeometry *geo )
	{
		throw std::exception("The method or operation is not implemented.");
	}

	void CGlbGlobePolyhedronSymbol::DrawToImage( CGlbGlobeRObject *obj,IGlbGeometry* geom, glbByte *image,glbInt32 imageW, glbInt32 imageH,CGlbExtent &ext )
	{
		throw std::exception("The method or operation is not implemented.");
	}

	IGlbGeometry * CGlbGlobePolyhedronSymbol::GetOutline(CGlbGlobeRObject *obj,IGlbGeometry *geo)
	{
		throw std::exception("The method or operation is not implemented.");
	}

}
