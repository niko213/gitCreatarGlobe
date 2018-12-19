#include "StdAfx.h"
#include "GlbGlobePolygonBuildingSymbol.h"

namespace GlbGlobe
{
	CGlbGlobePolygonBuildingSymbol::CGlbGlobePolygonBuildingSymbol(void)
	{
	}


	CGlbGlobePolygonBuildingSymbol::~CGlbGlobePolygonBuildingSymbol(void)
	{
	}

	osg::Node * CGlbGlobePolygonBuildingSymbol::Draw( CGlbGlobeRObject *obj,IGlbGeometry *geo )
	{
		throw std::exception("The method or operation is not implemented.");
	}

	void CGlbGlobePolygonBuildingSymbol::DrawToImage( CGlbGlobeRObject *obj,IGlbGeometry* geom, glbByte *image,glbInt32 imageW, glbInt32 imageH,CGlbExtent &ext )
	{
		throw std::exception("The method or operation is not implemented.");
	}

	IGlbGeometry * CGlbGlobePolygonBuildingSymbol::GetOutline(CGlbGlobeRObject *obj,IGlbGeometry *geo)
	{
		throw std::exception("The method or operation is not implemented.");
	}

}
