/********************************************************************
* Copyright (c) 2014 北京超维创想信息技术有限公司
* All rights reserved.
* @file    GlbCullActiveVisitor.h
* @brief   统一开关节点下所有子节点视锥筛选选项功能类的头文件，文档定义CGlbCullActiveVisitor类
* @version 1.0
* @author  Malin
* @date    2016-2-23 10:00
*********************************************************************/
#pragma once

#include <osg/NodeVisitor>
namespace GlbGlobe
{
	/**
	* @brief   统一开关节点下所有子节点视锥筛选选项功能类
	* @version 1.0
	* @author  Malin
	* @date    2016-2-23 10:00
	*/
	class GLB_DLLCLASS_EXPORT CGlbCullActiveVisitor :	public osg::NodeVisitor
	{
	public:
		CGlbCullActiveVisitor(bool isCullActive);
		~CGlbCullActiveVisitor(void);

		/**
		* @brief 实现虚接口
		* @return  无
		*/
		virtual void apply(osg::Node &node);		
	private:
		bool _isCullActive;
	};
}

