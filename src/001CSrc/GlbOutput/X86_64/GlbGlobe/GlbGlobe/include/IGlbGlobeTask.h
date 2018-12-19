/********************************************************************
* Copyright (c) 2014 北京超维创想信息技术有限公司
* All rights reserved.
*
* @file    IGlbGlobeTask.h
* @brief   任务处理类 头文件
*
* 这个档案定义IGlbGlobeTask这个class ,任务类
*
* @version 1.0
* @author  马林
* @date    2014-5-12 14:18
*********************************************************************/

#pragma once

#include "GlbReference.h"
//#include "GlbGlobeTypes.h"

namespace GlbGlobe
{
	class CGlbGlobeObject;
	class GLB_DLLCLASS_EXPORT IGlbGlobeTask : public CGlbReference
	{
	public:
		IGlbGlobeTask(void);
		~IGlbGlobeTask(void);

		/**
		* @brief 执行任务
		* @note 纯虚接口，需要在子类中实现
		* @return 成功返回true
		-	失败返回false
		*/
		virtual glbBool doRequest()=0;
		/**
		* @brief 获取任务相关的对象
		* @note 纯虚接口，需要在子类中实现
		* @return 成功返回对象指针
		-	失败返回NULL
		*/
		virtual CGlbGlobeObject* getObject()=0;
		/**
		* @brief 任务是否正在执行（数据是否正在加载中）
		* @return 是返回true
		-	否返回false
		*/
		glbBool isDoing();
		/**
		* @brief 任务是否执行完毕（数据是否加载完毕）
		* @return 是返回true
		-	否返回false
		*/
		glbBool isFinished();

	protected:
		glbBool mpr_isDoing;
		glbBool mpr_isFinished;
	};

}


