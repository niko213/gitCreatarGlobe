/********************************************************************
* Copyright (c) 2014 北京超维创想信息技术有限公司
* All rights reserved.
*
* @file    GlbGlobeViewStatusBar.h
* @brief   视图相关元素状态栏
*
* @version 1.0
* @author  敖建
* @date    2014-5-26 16:00
*********************************************************************/

#pragma once
#include "GlbGlobeView.h"
#include <osg/Camera>
#include <osgViewer/View>
namespace GlbGlobe
{
	class CGlbGlobe;
	class CGlbGlobeView;
	/**
	* @brief 视图相关元素状态栏类
	* @version 1.0
	* @author  敖建
	* @date    2014-5-26 16:01
	*/
	class GLB_DLLCLASS_EXPORT CGlbGlobeViewStatusBar : public osg::Camera
	{
		/**
		* @brief 私有类，状态栏回调
		* @version 1.0
		* @author  敖建
		* @date    2014-5-27 10:01
		*/
		class CGlbGlobeViewStatusBarUpdateCallback : public osg::NodeCallback  
		{
		public:
			/**
			* @brief 构造函数
			*/
			CGlbGlobeViewStatusBarUpdateCallback()
			{
				mpr_p_glbView = NULL;
				mpr_p_globe = NULL;
			}
			/**
			* @brief 析构函数
			*/
			~CGlbGlobeViewStatusBarUpdateCallback(){}
			/**
			* @brief 回调函数
			*/
			void operator()(osg::Node* node, osg::NodeVisitor* nv);
			/**
			* @brief 设置视图
			*/
			void Put_GlbGlobeView(CGlbGlobeView* pView);
			/**
			* @brief 设置场景
			*/
			void Put_GlbGlobe(CGlbGlobe* pGlobe);
		private:
			/*glbref_ptr<CGlbGlobeView>*/CGlbGlobeView*	mpr_p_glbView;	//视图
			/*glbref_ptr<CGlbGlobe>*/	 CGlbGlobe*	    mpr_p_globe;		//场景

			osg::Vec3d mpr_cameraPos;
			osg::Vec3d mpr_center;
		};
	public:
		/**
		* @brief 构造函数
		*/
		CGlbGlobeViewStatusBar(CGlbGlobeView* pview,int width,int height,CGlbGlobe* pglobe);
		/**
		* @brief 析构函数
		*/
		~CGlbGlobeViewStatusBar(void);
		/**
		* @brief 设置状态条位置
		* @param [in] width : 屏幕宽度
		*		  [in] height : 屏幕高度
		*/
		void SetPosition(int left,int top,int width,int height);
	private:
		/**
		* @brief 创建状态栏节点
		*/
		osg::ref_ptr<osg::Geode> createViewStatusBar();
	};
};