/********************************************************************
* Copyright (c) 2014 北京超维创想信息技术有限公司
* All rights reserved.
* @file    GlbGlobeLineModelSymbol.h
* @brief   模型线符号头文件，文档定义CGlbGlobeLineModelSymbol类
* @version 1.0
* @author  马林
* @date    2014-11-23 10:30
*********************************************************************/
#pragma once
#include "glbglobesymbol.h"

namespace GlbGlobe
{
	/**
	* @brief 模型线符号类				 
	* @version 1.0
	* @author  马林
	* @date    2014-11-23 10:30
	*/
	class GLB_SYMBOLDLL_CLASSEXPORT CGlbGlobeLineModelSymbol : public CGlbGlobeSymbol
	{
	public:
		/**
		* @brief 构造函数
		*/
		CGlbGlobeLineModelSymbol(void);
		/**
		* @brief 析构函数
		*/
		~CGlbGlobeLineModelSymbol(void);
		/**
		* @brief 模型符号渲染
		* @param[in] obj 渲染对象
		* @return 对象节点
		*/
		osg::Node *Draw(CGlbGlobeRObject *obj,IGlbGeometry *geo);
	private:
		// 当后缀名为.ini的图像列表文件时
		osg::Node *DrawLageImage(IGlbGeometry *geo, CGlbWString dataPath, bool isGlobe);
		
		osg::Vec3 CGlbGlobeLineModelSymbol::GetCoordByLinePercent(IGlbGeometry *geo, double per, std::vector<double>segLen, double totallength,int& prePointIdx, bool isGlobe);
	};
}

