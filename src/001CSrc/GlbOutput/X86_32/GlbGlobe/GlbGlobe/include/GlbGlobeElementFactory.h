/********************************************************************
* Copyright (c) 2014 北京超维创想信息技术有限公司
* All rights reserved.
*
* @file    GlbGlobeElementFactory.h
* @brief   Globe 元素管理
*
*
* @version 1.0
* @author  刘建辉
* @date    2014-5-29 15:40
*********************************************************************/
#pragma once
#include "GlbGlobeExport.h"
#include "GlbCommTypes.h"
#include "GlbWString.h"
#include "GlbReference.h"
#include <osg/Node>
#include <map>
using namespace std;
namespace GlbGlobe
{
	class CGlbGlobeView;
	/***************************************************************
	* 元素接口
	*
	****************************************************************/
	class GLB_DLLCLASS_EXPORT IGlbGlobeElement :public CGlbReference
	{
	public:
		/**
		* @brief 元素初始化
		* @param [in] view 场景视图
		*				    
		* @return true  代表成功
		*         false 代表失败
		*/
		virtual bool Initialize(CGlbGlobeView *view)=0;
		/**
		* @brief 获取元素的osg节点
		* @
		*				    
		* @return 元素的osg节点
		*         
		*/
		virtual osg::Node* GetOsgNode()=0;
		/**
		* @brief 控制元素的显隐
		* @param isShow true 代表需要显示
		*               false 代表需要隐藏
		*				    
		* @return 无
		*/
		virtual void Show(bool isShow)=0;
		/**
		* @brief 控制元素的子要素显隐(要素存在子要素的情况)
		* @param isShow true 代表需要显示
		*               false 代表需要隐藏
		* @param subEleName 子要素名称
		*				    
		* @return 无
		*/
		virtual void Show(bool isShow,CGlbWString const &subEleName/*=L""*/){}

		virtual osg::Node *GetNode(CGlbWString const &subEleName){return NULL;}
	};
	/**********************************************************************
	*
	*   元素提供者接口
	*
	***********************************************************************/
	class GLB_DLLCLASS_EXPORT IGlbGlobeElementProvider
	{
	public:
		/**
		* @brief 获取元素提供者名称
		*				    
		* @return 提供者名称
		*/
		virtual glbWChar* GetClassName()=0;
		/**
		* @brief 创建一个元素	    
		*				    
		* @return 成功 返回创建的元素
		*         失败，返回NULL
		*/
		virtual IGlbGlobeElement* Create()=0;
	};
	/************************************************
	*
	*   元素提供者管理器
	*
	*************************************************/
	class GLB_DLLCLASS_EXPORT CGlbGlobeElementFactory
	{
	public:
		static CGlbGlobeElementFactory* GetInstance();
	public:
		CGlbGlobeElementFactory(void);
		~CGlbGlobeElementFactory(void);
	public:
		void RegisterProvider(IGlbGlobeElementProvider* provider);
		void UnRegisterProvider(const glbWChar* className);
		IGlbGlobeElement* CreateElement(const glbWChar* className);
	private:		
		map<CGlbWString,IGlbGlobeElementProvider*> mpr_providers;
	};	
}
