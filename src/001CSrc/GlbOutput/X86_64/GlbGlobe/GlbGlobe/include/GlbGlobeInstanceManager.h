/********************************************************************
* Copyright (c) 2014 北京超维创想信息技术有限公司
* All rights reserved.
*
* @file    GlbGlobeInstanceManager.h
* @brief   实例化对象管理器头文件
*
* 这个档案定义CGlbGlobeInstanceManager这个class
*
* @version 1.0
* @author  马林
* @date    2014-7-4 10:10
*********************************************************************/
#pragma once

#include "GlbReference.h"
#include "GlbGlobeObject.h"
#include "GlbWString.h"
#include <osg\Node>
namespace GlbGlobe
{
	/**
	* @brief 实例化实现类
	*/
	class CGlbGlobeInstance : public CGlbReference
	{
	public:
		CGlbGlobeInstance();
		~CGlbGlobeInstance();

		glbBool addInstance(glbWChar* key,osg::Node *node);
		osg::Node *findInstance(glbWChar* key);	
		void clearInstance();
	private:
		std::map<CGlbWString,osg::ref_ptr<osg::Node>> mpr_instances;
		CRITICAL_SECTION mpr_critical;
	};

	class GLB_DLLCLASS_EXPORT CGlbGlobeInstanceManager
	{
	public:
		/**
		* @brief 实例化对象管理器构造函数
		* @return 无
		*/
		CGlbGlobeInstanceManager(void);
		/**
		* @brief 实例化对象管理器析构函数
		* @return 无
		*/
		~CGlbGlobeInstanceManager(void);

		/**
		* @brief 添加一个实例
		* @param key 索引值
		* @return 成功返回true，失败返回false
		*/
		static glbBool AddInstance(glbWChar* key,osg::Node *node);

		/**
		* @brief 根据实例对象索引值查找实例对象
		* @param key 索引值
		* @return 实例对象生成的节点osg::Node*
		*/
		static osg::Node *FindInstance(glbWChar* key);	

		/**
		* @brief 清理 实例化对象管理器中的实例
		* @note 【调度器，在内存不足时调用】
		* @return 无
		*/
		static void ClearInstance();
	};

}


