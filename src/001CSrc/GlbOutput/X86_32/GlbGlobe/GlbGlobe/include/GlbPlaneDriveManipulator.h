/********************************************************************
* Copyright (c) 2014 北京超维创想信息技术有限公司
* All rights reserved.
*
* @file    GlbPlaneDriveManipulator.h
* @brief   平面模式下第一人称驾驶操控器类头文件
*
* @version 1.0
* @author  敖建
* @date    2014-6-5 15:14
*********************************************************************/
#pragma once
#include "GlbGlobeManipulatorManager.h"
#include "GlbCommTypes.h"
namespace GlbGlobe
{
	class GlbGlobe;
	/**
	* @brief CGlbPlaneDriveManipulator 平面模式下驾驶操控器类				 
	* @version 1.0
	* @author  敖建
	* @date    2014-6-5 15:14
	*/
	class GLB_DLLCLASS_EXPORT CGlbPlaneDriveManipulator
	{
	public:
		/**
		* @brief 构造函数
		*/
		CGlbPlaneDriveManipulator(void);

		/**
		* @brief 析构函数
		*/
		~CGlbPlaneDriveManipulator(void);
	};
}