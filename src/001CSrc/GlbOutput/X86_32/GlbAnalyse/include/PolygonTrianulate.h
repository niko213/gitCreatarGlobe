#pragma once

#include "GlbTin.h"
#include "GlbPolygon.h"

class CPolygonTriangulater
{
public:
	struct Control_Point
	{
		double x2d;
		double y2d;
		double x3d;
		double y3d;
		double z3d;
	};

	virtual bool Initialize(Control_Point* points, long count) = 0;
	virtual CGlbMultiTin* Triangulate(CGlbMultiPolygon* poly) = 0;
};

class CPolygonTriangulate
{
public:
	CPolygonTriangulate(void);
	~CPolygonTriangulate(void);

	bool Initialize(CPolygonTriangulater::Control_Point* points,long count);
	CGlbMultiTin* Triangulate(CGlbMultiPolygon* poly);

private:
	CPolygonTriangulater* triangulater;
};

