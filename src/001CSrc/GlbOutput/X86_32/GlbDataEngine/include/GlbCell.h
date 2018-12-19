#pragma once
#include "GlbReference.h"
#include "GlbCommTypes.h"
#include "GlbDataEngineExport.h"

class GLBDATAENGINE_API GlbPoint3D 
{
public:
	GlbPoint3D()
	{

	};
	GlbPoint3D(glbDouble _x,glbDouble _y, glbDouble _z)
	{
		X = _x;
		Y = _y;
		Z = _z;
	};
public:
	glbDouble X;
	glbDouble Y;
	glbDouble Z;
};

class GLBDATAENGINE_API cwSGridCell
{
public:
	cwSGridCell()
	{

	};
public:
	glbInt32 I;
	glbInt32 J;
	glbInt32 K;

	GlbPoint3D Node[8];
};

class GLBDATAENGINE_API cwSGridFacet
{
public:
	cwSGridFacet()
	{
	};
public:
	glbInt32 CellIndex;
	glbByte Orientation;
};




///**
//  * @brief µ¥Ôª¸ñ
//  * 
//  * 
//  * @version 1.0
//  * @date    2018-01-02 15:27
//  * @author  ML
//  */
//class GLBDATAENGINE_API CGlbCell : public CGlbReference
//{
//public:
//	CGlbCell(void);
//	~CGlbCell(void);
//};

