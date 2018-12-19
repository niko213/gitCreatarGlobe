/********************************************************************
* Copyright (c) 2014 北京超维创想信息技术有限公司
* All rights reserved.
*
* @file    GlbGlobeTaskManager.h
* @brief   任务管理类 头文件
*
* 这个档案定义CGlbGlobeTaskManager这个class ,任务管理类
*
* @version 1.0
* @author  马林
* @date    2014-5-12 14:18
*********************************************************************/
#include "glbref_ptr.h"
#include "IGlbGlobeTask.h"
#include <list>
#include <OpenThreads/Thread>
#include <OpenThreads/Block>
#include <osg/GraphicsContext>
#include <osgUtil/GLObjectsVisitor>
#include <osgUtil/IncrementalCompileOperation>
#include "GlbGlobeRObject.h"

#pragma once
namespace GlbGlobe
{
	class CGlbNodeCompileVisitor : public osg::NodeVisitor
	{
	public:

		CGlbNodeCompileVisitor();
		~CGlbNodeCompileVisitor();

		virtual void apply(osg::Node& node);
		virtual void apply(osg::Geode& node);

		virtual void apply(osg::Drawable& drawable);
		virtual void apply(osg::StateSet& stateset);
	public:
		osg::RenderInfo _renderInfo;
		osg::ref_ptr<osg::PixelBufferObject> _pbo;

	};

	class GLB_DLLCLASS_EXPORT CGlbGlobeTaskManager : public CGlbReference
	{
	private:
		struct CGlbGlobeTaskCompileCompletedCallback;
		friend struct CGlbGlobeTaskCompileCompletedCallback;
		/** 
		* @brief 对象任务处理线程类
		* @author 马林
		* @date    2014-5-12 15:10
		* @note 任务管理内部类,负责从对象任务列表中取任务并执行
		*/
		class CGlbGlobeTaskThread : public OpenThreads::Thread
		{
		public:
			/**
			* @brief 对象任务处理线程构造函数
			* @return 无
			*/
			CGlbGlobeTaskThread (CGlbGlobeTaskManager* taskManager);
			virtual ~CGlbGlobeTaskThread (void);
			/**
			* @brief 线程运行入口
			* @return 无
			*/
			virtual void run();
			/**
			* @brief 线程取消入口
			* @return 无
			*/
			virtual int cancel();		
			/**
			* @brief 线程暂停入口
			* @return 无
			*/
			void pause();
			/**
			* @brief 线程取消暂停入口
			* @return 无
			*/
			void resume();
		private:
			CGlbGlobeTaskManager* mpr_taskmanager;	
			glbBool mpr_bDone;
			glbBool mpr_pause;
			glbBool mpr_ispaused;
		};
		/** 
		* @brief 地形块任务处理线程类
		* @author 马林
		* @date    2014-5-12 15:10
		* @note 任务管理内部类,负责从地形块任务列表中取任务并执行
		*/
		class CGlbGlobeTerrainTaskThread : public OpenThreads::Thread
		{
		public:
			/**
			* @brief 地形块任务处理线程构造函数
			* @return 无
			*/
			CGlbGlobeTerrainTaskThread (CGlbGlobeTaskManager* taskManager);
			virtual ~CGlbGlobeTerrainTaskThread (void);
			/**
			* @brief 线程运行入口
			* @return 无
			*/
			virtual void run();
			/**
			* @brief 线程取消入口
			* @return 无
			*/
			virtual int cancel();
			/**
			* @brief 线程暂停入口
			* @return 无
			*/
			void pause();
			/**
			* @brief 线程取消暂停入口
			* @return 无
			*/
			void resume();
		private:
			CGlbGlobeTaskManager* mpr_taskmanager;	
			glbBool mpr_bDone;
			glbBool mpr_pause;
			glbBool mpr_ispaused;
		};
	public:
		/**
		* @brief globe任务管理器类构造函数	
		* @return 无
		*/
		CGlbGlobeTaskManager();
		/**
		* @brief globe任务管理器类析构函数
		* @return 无
		*/
		~CGlbGlobeTaskManager(void);

		/**
		* @brief globe任务管理器类初始化函数	
		* @param gc 编译显示列表使用的渲染描述信息
		* @return 无
		*/	  
		void init(osg::GraphicsContext* compileContext);
		osgUtil::IncrementalCompileOperation* GetIncrementalCompileOperation(){return mpr_ico.get();};
		void setIncrementalCompileOperation(osgUtil::IncrementalCompileOperation* ico){ mpr_ico = ico;}

		/**
		* @brief 添加一个对象处理任务
		* @return 无
		*/
		void  AddTask(IGlbGlobeTask* task);

		/**
		* @brief 添加一个地形块处理任务
		* @return 无
		*/	
		void  AddTerrainTask(IGlbGlobeTask* task);
		/**
		* @brief 获取一个对象处理任务
		* @return 如果有任务未处理返回任务
		-   没有任务则返回NULL
		*/
		glbref_ptr<IGlbGlobeTask>  GetTask();
		/**
		* @brief 获取一个地形块处理任务
		* @return 如果有任务未处理返回任务
		-   没有任务则返回NULL
		*/	
		glbref_ptr<IGlbGlobeTask>  GetTerrainTask();
		/**
		* @brief 清空所有对象处理任务
		* @return 无
		*/
		void  CleanObjectTasks();
		/**
		* @brief 获取对象任务数目
		* @return 任务数目
		*/
		glbInt32 GetObjectTaskNum();
		/**
		* @brief 删除地形加载任务
		* @return  成功返回true ，失败返回false
		*/
		glbBool	ReomveTerrainTask(IGlbGlobeTask* task);	
		/**
		* @brief 销毁所有的任务线程
		* @return  
		*/
		void DestoryAllTaskThreads();
		/**
		* @brief 暂停（恢复）所有任务线程
		* @return  
		*/
		void Pause(glbBool bPause);
	private:
		std::list<glbref_ptr<IGlbGlobeTask>> mpr_objtasks;					//对象加载任务队列
		std::list<glbref_ptr<IGlbGlobeTask>> mpr_terraintasks;				//地形块加载任务队列
		GlbCriticalSection		mpr_objtask_critical;						//对象任务临界区
		GlbCriticalSection		mpr_terraintask_critical;					//地形块任务临界区

		CGlbGlobeTerrainTaskThread* mpr_tt_thread;							//地形块加载请求线程
		//std::vector<CGlbGlobeTerrainTaskThread*> mpr_tt_thread;			//地形块加载请求线程
		std::vector<CGlbGlobeTaskThread*> mpr_tasks_thread;					//业务请求处理线程
		osg::ref_ptr<osgUtil::IncrementalCompileOperation> mpr_ico;

		glbBool					mpr_isInit;									//标志已经初始化
	public:
		//osg::ref_ptr<osgUtil::IncrementalCompileOperation> mpr_iCO;		//增量编译
		osg::ref_ptr<osg::GraphicsContext> mpr_compileContext;				//编译用context
		GlbCriticalSection		mpr_compile_critical;						//编译临界区

		OpenThreads::Block      mpr_tt_thread_block;				///<地形块加载线程阻塞变量
		OpenThreads::Block		mpr_tasks_thread_block;				///<对象加载线程阻塞变量
	};
}