/********************************************************************
* Copyright (c) 2014 北京超维创想信息技术有限公司
* All rights reserved.
* @file    GlbGlobeVirtualSectionSymbol.h
* @brief   虚拟剖面符号头文件，文档定义CGlbGlobeVirtualSectionSymbol类
* @version 1.0
* @author  马林
* @date    2018-8-21 10:40
*********************************************************************/
#pragma once
#include "glbglobesymbol.h"
namespace GlbGlobe
{
	/**
	* @brief CGlbGlobeVirtualSectionSymbol虚拟剖面符号类				 
	* @version 1.0
	* @author  马林
	* @date    2018-8-21 10:40
	*/
	class GLB_SYMBOLDLL_CLASSEXPORT CGlbGlobeVirtualSectionSymbol : public CGlbGlobeSymbol
	{
	private:
		enum GlbProjPlaneEnum
		{// 投影面枚举变量  x轴(经度) y轴(纬度) z轴(海拔高度)
			GLB_PLANE_UNKNOWN	= 0,	// 未知投影平面
			GLB_PLANE_XY		= 1,	// XY平面
			GLB_PLANE_XZ		= 2,	// XZ平面
			GLB_PLANE_YZ		= 3		// YZ平面		
		};
	public:
		CGlbGlobeVirtualSectionSymbol(void);
		~CGlbGlobeVirtualSectionSymbol(void);

		/**
		* @brief 模型符号渲染
		* @param[in] obj 渲染对象
		* @return 对象节点
		*/
		virtual osg::Node *Draw(CGlbGlobeRObject *obj,IGlbGeometry *geo);
	private:
		// 坐标投影面  x轴(经度) y轴(纬度) z轴(海拔高度)
		GlbProjPlaneEnum mpr_prjPlaneEnum;		
	};
}
