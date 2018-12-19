// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#include <comutil.h>
#include <comdef.h>

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>

#include "agg_pixfmt_rgba.h"
#include "agg_renderer_base.h"
typedef agg::renderer_base<agg::pixfmt_rgba32> renb_type; 

#include "osg/Vec4"
#include "GlbCommTypes.h"
#define GetAValue(rgba)      (LOBYTE((rgba)>>24))
extern osg::Vec4 GetColor(glbInt32 color);
extern bool WriteBmpFile(int w, int h, unsigned char *pdata, BSTR filename);
// TODO: reference additional headers your program requires here

#define GetRValueInRGBA(rgba)      (LOBYTE(rgba))
#define GetGValueInRGBA(rgba)      (LOBYTE(((WORD)(rgba)) >> 8))
#define GetBValueInRGBA(rgba)      (LOBYTE((rgba)>>16))
#define GetRValueInBGRA(bgra)      (LOBYTE((bgra)>>16))
#define GetGValueInBGRA(bgra)      (LOBYTE(((bgra & 0xFFFF)) >> 8))//(LOBYTE(((WORD)(bgra)) >> 8))
#define GetBValueInBGRA(bgra)      (LOBYTE(bgra))

#define OverlayFazhi 4//10//2	//定义overlay倍数阀值
//全局变量
#include <osg/CoordinateSystemNode>
extern osg::ref_ptr<osg::EllipsoidModel> g_ellipsoidModel;//全局椭球体

#include "GlbGlobeObjectRenderInfos.h"
extern osg::Vec3 ComputeCornerByAlign(glbDouble width,glbDouble height,
	glbDouble ratio,GlbGlobe::GlbGlobeLabelAlignTypeEnum labelAlign);