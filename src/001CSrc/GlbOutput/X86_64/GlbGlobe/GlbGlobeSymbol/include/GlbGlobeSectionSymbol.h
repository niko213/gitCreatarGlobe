/********************************************************************
* Copyright (c) 2014 北京超维创想信息技术有限公司
* All rights reserved.
* @file    GlbGlobeSectionSymbol.h
* @brief   剖面符号头文件，文档定义CGlbGlobeSectionSymbol类
* @version 1.0
* @author  马林
* @date    2014-11-20 15:40
*********************************************************************/
#pragma once
#include "glbglobesymbol.h"
#include "GlbPolygon.h"
#include "GlbTin.h"
namespace GlbGlobe
{
	/**
	* @brief 剖面符号类				 
	* @version 1.0
	* @author  马林
	* @date    2014-11-20 15:40
	*/
	class GLB_SYMBOLDLL_CLASSEXPORT CGlbGlobeSectionSymbol : public CGlbGlobeSymbol
	{
	public:
		enum GlbProjPlaneEnum
		{// 投影面枚举变量  x轴(经度) y轴(纬度) z轴(海拔高度)
			GLB_PLANE_UNKNOWN	= 0,	// 未知投影平面
			GLB_PLANE_XY		= 1,	// XY平面
			GLB_PLANE_XZ		= 2,	// XZ平面
			GLB_PLANE_YZ		= 3		// YZ平面		
		};
	public:
		/**
		* @brief 构造函数
		*/
		CGlbGlobeSectionSymbol(void);
		/**
		* @brief 析构函数
		*/
		~CGlbGlobeSectionSymbol(void);
		/**
		* @brief 模型符号渲染
		* @param[in] obj 渲染对象
		* @return 对象节点
		*/
		virtual osg::Node *Draw(CGlbGlobeRObject *obj,IGlbGeometry *geo);

	private:
		osg::Vec2 CaculateTexCoord(glbDouble ptx,glbDouble pty,glbDouble ptz, CGlbExtent* ext,
						GlbGlobeTexRepeatModeEnum texRMode,glbInt32 tilingU	,glbInt32 tilingV);

		// 根据数据空间区域判断合适的投影平面
		GlbProjPlaneEnum CaculateProjPlane(CGlbExtent* ext);

		// 计算多边形的法向
		osg::Vec3 CaculatePolyNormal(CGlbMultiPolygon *mulPoly, GlbGlobeTypeEnum type);

		// 绘制Section中的polygon
		osg::Node* drawPolyGeom(CGlbMultiTin* geo, CGlbGlobeRObject *obj, CGlbFeature* polyFeature);
	private:
		// 坐标投影面  x轴(经度) y轴(纬度) z轴(海拔高度)
		GlbProjPlaneEnum mpr_prjPlaneEnum;		
	};
}

