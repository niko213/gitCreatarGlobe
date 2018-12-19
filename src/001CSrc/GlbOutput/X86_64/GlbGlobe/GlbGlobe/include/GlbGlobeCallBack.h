/********************************************************************
  * Copyright (c) 2014 北京超维创想信息技术有限公司
  * All rights reserved.
  *
  * @file    CGlbGlobeCallBack.h
  * @brief   Globe场景节点回调类头文件
  *
  * 这个档案定义CGlbGlobeCallBack这个class,负责修改场景节点树中的节点
  *
  * @version 1.0
  * @author  马林
  * @date    2014-5-12 16:40
*********************************************************************/
#pragma once
#include "GlbGlobeRObject.h"
#include "GlbGlobeLayer.h"
#include <osg\Node>
#include <osg\NodeCallback>
#include <osg\Geode>

namespace GlbGlobe
{

class GLB_DLLCLASS_EXPORT CGlbGlobeCallBack : public osg::NodeCallback
{
public:
	/**
	 * @brief Globe场景节点修改任务类
	 * @author 马林
	 * @date    2014-5-12 17:16
	 * @note Globe场景节点修改类的内部类
	*/
	class GLB_DLLCLASS_EXPORT CGlbGlobeCallBackTask : public CGlbReference
	{
		friend class CGlbGlobeCallBack;
	public:	
		 /**
		  * @brief 节点修改任务构造函数
		  * @param [in] robj 可绘制对象
		  * @param [in] node 对象osg节点
		  * @param [in] size 对象占用的内存大小
		  * @param [in] compileTime node编译为显示列表需要的时间估算值
		  * @return 无
		  */
		CGlbGlobeCallBackTask(CGlbGlobeRObject* robj, osg::Node* node=NULL, glbInt32 size=0, glbDouble compileTime=0.0);
		 /**
		  * @brief 节点替换任务构造函数
		  * @param [in] robj 可绘制对象
		  * @param [in] origChild 原节点
		  * @param [in] newChild  新osg节点		
		   @param [in] size 对象占用的内存大小差
		  * @param [in] compileTime node编译为显示列表需要的时间估算值
		  * @return 无
		  */
		CGlbGlobeCallBackTask(CGlbGlobeRObject* robj,osg::Node* origChild, osg::Node* newChild, glbInt32 size=0, glbDouble compileTime=0.0);
		 /**
		  * @brief 节点修改任务构造函数
		  * @param [in] lyr 对象图层FeatureLayer
		  * @param [in] node 对象osg节点
		  * @param [in] size 对象占用的内存大小
		  * @param [in] compileTime node编译为显示列表需要的时间估算值
		  * @return 无
		  */
		CGlbGlobeCallBackTask(osg::Node* parent_node, osg::Node* node=NULL, glbInt32 size=0, glbDouble compileTime=0.0);	
		 /**
		  * @brief 节点修改任务析构函数
		  * @return 无
		  */
		~CGlbGlobeCallBackTask();

		glbref_ptr<CGlbGlobeRObject> mpr_obj;		
		osg::ref_ptr<osg::Node>      mpr_NotObjParentNode;   // 代表父节点或origChild
		osg::ref_ptr<osg::Node>      mpr_node;				 // 代表需要新加的节点或newChild
		glbInt32                     mpr_size;
		glbDouble					 mpr_compileTime;
	private:
		glbBool                      mpr_isRemoveTask;		
		glbBool						 mpr_isReplaceTask;
	};

	class GLB_DLLCLASS_EXPORT CGlbGlobeFadeCallBackTask : public CGlbReference
	{
	public:
		CGlbGlobeFadeCallBackTask(osg::Node *node,osg::NodeCallback *callBack = NULL);
		~CGlbGlobeFadeCallBackTask();
		osg::ref_ptr<osg::Node> mpr_node;
		osg::ref_ptr<osg::NodeCallback> mpr_callBack;
	};
	
public:
	/**
	* @brief 构造函数
	* @return 无
	*/
	CGlbGlobeCallBack(void);
	/**
	* @brief 析构函数
	* @return 无
	*/
	~CGlbGlobeCallBack(void);

	/**
	* @brief 添加一个挂节点任务
	* @param [in] task 挂节点任务
	* @return 无
	*/
	 void AddHangTask(CGlbGlobeCallBackTask *task);
	/**
	* @brief 添加一个摘节点任务
	* @param [in] task 摘节点任务
	* @return 无
	*/
	 void AddRemoveTask(CGlbGlobeCallBackTask *task);
	/**
	* @brief 添加一个替换节点任务
	* @param [in] task 替换节点任务
	* @return 无
	*/
	 void AddReplaceTask(CGlbGlobeCallBackTask *task);
	/**
	* @brief 清空所有任务队列（包括挂队列和摘队列）
	* @return 无
	*/
	 void CleanQueue();
	/**
	* @brief 获取当前已使用的显存数量
	* @return 已使用的显存数量
	*/
	 glbInt32 GetUsedDislayMemory(){return mpr_useddisplaymemory;}
    /**
	 * @brief 获取单帧流量.
	*/
	 glbInt32 GetHangSize(){return mpr_hangSize;}
	 /**
	* @brief globe节点回调实现
	* @param [in] node 场景的osg节点
	* @param [in] nv osg节点访问器	  
	* @return  无
	*/
	  virtual void operator() (osg::Node* node, osg::NodeVisitor* nv);

	  void AddFadeTask(CGlbGlobeFadeCallBackTask *task);
	  
private:
	/*
	 *    交换队列
	 */
	 void SwapQueue();

	 // ceshi
	 // 计算当前场景中渲染的对象 占用的内存数量
	 glbInt64 ComputeTotalUsedMemory(std::map<int, glbref_ptr<CGlbGlobeRObject>> &objs);
private:
	glbInt32                                     mpr_useddisplaymemory;		///<已经使用的显存总量
	glbInt32                                     mpr_hangSize;              ///<单帧流量.
	std::list<glbref_ptr<CGlbGlobeCallBackTask>> mpr_prepareHangtasks;		///<缓冲队列	
	std::list<glbref_ptr<CGlbGlobeCallBackTask>> mpr_hangtasks;		        ///<队列
	std::list<glbref_ptr<CGlbGlobeFadeCallBackTask>> mpr_prepareFadetasks;			///<渐变缓冲队列
	std::list<glbref_ptr<CGlbGlobeFadeCallBackTask>> mpr_fadetasks;			///<渐变队列
	GlbCriticalSection                           mpr_critical;	            ///<临界区

	// ceshi
	glbDouble									 mpr_maximumTimeAvailableForGLCompile;	///<最长编译时长毫秒【默认5ms】

	//double										 mpr_previous;
	std::map<int, glbref_ptr<CGlbGlobeRObject>>  mpr_loadedObjs; ///记录当前内存中的对象
};
}


