// stdafx.cpp : source file that includes just the standard includes
// GlbGlobeSymbol.pch will be the pre-compiled header
// stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"

//宏定义 用于固定的偏移   
#define COLORTABLE 54   
#define RGBA_MODE 3//4
#define RGB_MODE 3

// TODO: reference any additional headers you need in STDAFX.H
// and not in this file

osg::ref_ptr<osg::EllipsoidModel> g_ellipsoidModel = new osg::EllipsoidModel(osg::WGS_84_RADIUS_EQUATOR,osg::WGS_84_RADIUS_POLAR);

extern osg::Vec4 GetColor( glbInt32 color )
{
	glbInt32 blue = LOBYTE(color);//GetRValue(color);
	glbInt32 green = LOBYTE(((color & 0xFFFF)) >> 8);//GetGValue(color);
	glbInt32 red = LOBYTE((color)>>16);//GetBValue(color);
	glbInt32 alpha = LOBYTE((color)>>24);//GetAValue(color);
	return osg::Vec4(red / 255.0,green / 255.0,blue / 255.0,alpha / 255.0);
}

extern bool WriteBmpFile( int w, int h, unsigned char *pdata, BSTR filename )
{
		unsigned char header[COLORTABLE] = {

		0x42, 0x4d, 0, 0, 0, 0, 0, 0, 0, 0,

		54, 0, 0, 0, 40, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, RGBA_MODE * 8, 0, 

		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 

		0, 0, 0, 0

	};

	long file_size = (long)w * (long)h * RGBA_MODE + 54;

	header[2] = (unsigned char)(file_size &0x000000ff);

	header[3] = (file_size >> 8) & 0x000000ff;

	header[4] = (file_size >> 16) & 0x000000ff;

	header[5] = (file_size >> 24) & 0x000000ff;

	long width = w;

	header[18] = width & 0x000000ff;

	header[19] = (width >> 8) &0x000000ff;

	header[20] = (width >> 16) &0x000000ff;

	header[21] = (width >> 24) &0x000000ff;

	long height = h;

	header[22] = height &0x000000ff;

	header[23] = (height >> 8) &0x000000ff;

	header[24] = (height >> 16) &0x000000ff;

	header[25] = (height >> 24) &0x000000ff;

	FILE *pWritingFile = NULL;

	_bstr_t bFileName = filename;
	char* pBmpFileName = bFileName; 
	pWritingFile = fopen(pBmpFileName, "wb"); 

	if( pWritingFile == NULL )
		return false;

	fwrite(header, sizeof(unsigned char), 54, pWritingFile);

	int BytesPerPixel = RGBA_MODE;
	int LineLength, TotalLength; 
	LineLength = w * BytesPerPixel; // 每行数据长度大致为图象宽度乘以 
	// 每像素的字节数 
	while( LineLength % 4 != 0 ) // 修正LineLength使其为4的倍数 
		++LineLength; 
	TotalLength = LineLength * h; // 数据总长 = 每行长度 * 图象高度 

	fwrite(pdata, sizeof(unsigned char), (size_t)(long)TotalLength,  pWritingFile); 

	// 释放内存和关闭文件 	
	fclose(pWritingFile); 	

	return true;
}

extern osg::Vec3 ComputeCornerByAlign( glbDouble width,glbDouble height, 
	glbDouble ratio,GlbGlobe::GlbGlobeLabelAlignTypeEnum labelAlign )
{
	osg::Vec3 corner(0.0,0.0,0.0);
	if(labelAlign == GlbGlobe::GLB_LABELALG_LEFTTOP)
		corner = osg::Vec3(0.0,-height * ratio,0.0);
	else if(labelAlign == GlbGlobe::GLB_LABELALG_LEFTCENTER)
		corner = osg::Vec3(0.0,-height / 2 * ratio,0.0);
	else if(labelAlign == GlbGlobe::GLB_LABELALG_CENTERTOP)
		corner = osg::Vec3(-width / 2 * ratio,-height * ratio,0.0);
	else if(labelAlign == GlbGlobe::GLB_LABELALG_CENTERCENTER)
		corner = osg::Vec3(-width / 2 * ratio,-height / 2 * ratio,0.0);
	else if(labelAlign == GlbGlobe::GLB_LABELALG_CENTERBOTTOM)
		corner = osg::Vec3(-width / 2 * ratio,0.0,0.0);
	else if(labelAlign == GlbGlobe::GLB_LABELALG_RIGHTTOP)
		corner = osg::Vec3(-width * ratio,-height * ratio,0.0);
	else if(labelAlign == GlbGlobe::GLB_LABELALG_RIGHTCENTER)
		corner = osg::Vec3(-width * ratio,-height / 2 * ratio,0.0);
	else if(labelAlign == GlbGlobe::GLB_LABELALG_RIGHTBOTTOM)
		corner = osg::Vec3(-width * ratio,0.0,0.0);
	return corner;
}
