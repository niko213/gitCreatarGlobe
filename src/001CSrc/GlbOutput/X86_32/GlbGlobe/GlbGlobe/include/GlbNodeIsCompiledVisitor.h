/********************************************************************
* Copyright (c) 2014 北京超维创想信息技术有限公司
* All rights reserved.
* @file    GlbNodeIsCompiledVisitor.h
* @brief   判断节点内Geometry和Texture是否已经编译类头文件，文档定义CGlbNodeIsCompiledVisitor类
* @version 1.0
* @author  Malin
* @date    2015-3-19 15:00
*********************************************************************/
#pragma once

#include <osg/NodeVisitor>
#include <osg/Geode>

namespace GlbGlobe
{
	/**
	* @brief  判断节点内Geometry和Texture是否已经编译类
	* @version 1.0
	* @author  Malin
	* @date    2015-3-19 15:00
	*/
	class GLB_DLLCLASS_EXPORT CGlbNodeIsCompiledVisitor : public osg::NodeVisitor
	{
	public:
		CGlbNodeIsCompiledVisitor(void);
		~CGlbNodeIsCompiledVisitor(void);
		/**
		* @brief 实现虚接口
		* @return  无
		*/
		virtual void apply(osg::Geode &node);		
		/**
		* @brief 返回是否已经编译	
		* @return  true表示已经编译，
		false表示未编译
		*/
		glbBool IsCompiled();
	private:
		glbBool mpr_isCompiled;
	};
}