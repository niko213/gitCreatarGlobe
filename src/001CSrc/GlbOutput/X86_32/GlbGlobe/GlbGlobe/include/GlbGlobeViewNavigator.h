/********************************************************************
* Copyright (c) 2014 北京超维创想信息技术有限公司
* All rights reserved.
*
* @file    GlbGlobeViewNavigator.h
* @brief   视图相关元素导航盘
*
* @version 1.0
* @author  敖建
* @date    2014-5-30 14:20
*********************************************************************/
#pragma once
#include "GlbGlobeView.h"
#include <osg/Camera>
#include <osgViewer/View>
namespace GlbGlobe
{
	/** 
	*枚举 导航盘位置枚举类型
	*/
	enum GlbGlobeNavigatorPositionType
	{
		NAVIGATOR_NONE_BTN = 0,		//其他位置
		NAVIGATOR_LEFT_BTN = 1,		//向左Move键
		NAVIGATOR_RIGHT_BTN = 2,	//向右Move键
		NAVIGATOR_UP_BTN = 3,		//向上Move键
		NAVIGATOR_DOWN_BTN = 4,		//向下Move键
		NAVIGATOR_CENTER_BTN = 5,	//导航盘中心
		NAVIGATOR_YAW_BTN = 6,		//Yaw圈
		NAVIGATOR_PITCH_BTN = 7,	//Pitch圈
		NAVIGATOR_OUT_BTN = 8		//导航盘外
	};
	class CGlbGlobe;
	class CGlbGlobeView;
	/**
	* @brief 视图相关元素导航盘类
	* @version 1.0
	* @author  敖建
	* @date    2014-5-30 14:46
	*/
	class GLB_DLLCLASS_EXPORT CGlbGlobeViewNavigator : public osg::Camera
	{
		/**
		* @brief 私有类，导航盘事件回调
		* @version 1.0
		* @author  敖建
		* @date    2014-6-3 10:10
		*/
		class CGlbGlobeViewNavigatorEventHandler : public osgGA::GUIEventHandler
		{
		public:
			/**
			* @brief 构造函数
			*/
			CGlbGlobeViewNavigatorEventHandler(CGlbGlobeView* pview,osg::Geode* pgeode)
			{
				mpr_p_view = pview;
				_ga_t0 = NULL;
				_ga_t1 = NULL;
				moveflag = 0;				
				push_pitch = -90;
				_geode = pgeode;
			}
			/**
			* @brief 析构函数
			*/
			~CGlbGlobeViewNavigatorEventHandler(){}
			/**
			* @brief 事件回调
			*/
			glbBool handle(const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter& aa);
			/**
			* @brief 触控事件处理
			* @return 返回true ：view需重绘
			*		-	返回false ：不需重绘
			*/
			glbBool handleTouch( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa );
		private:
			/**
			* @brief 获取鼠标在导航盘上的枚举位置
			* @param [in] ea 鼠标事件
			* @return GlbGlobeNavigatorPositionType 鼠标在导航盘上的位置的枚举类型
			*/
			GlbGlobeNavigatorPositionType getNavigatorPositionType(const osgGA::GUIEventAdapter& ea);
			/**
			* @brief 获取鼠标在导航盘上的枚举位置
			* @param [in] ea 鼠标事件
			* @return GlbGlobeNavigatorPositionType 鼠标在导航盘上的位置的枚举类型
			*/
			GlbGlobeNavigatorPositionType getTouchNavigatorPositionType(const osgGA::GUIEventAdapter& ea);
		private:
			CGlbGlobeView* mpr_p_view;
			osg::ref_ptr<const osgGA::GUIEventAdapter> _ga_t1;	//当前的前一个鼠标事件(主要用来存鼠标按下的那个事件)
			osg::ref_ptr<const osgGA::GUIEventAdapter> _ga_t0;	//当前鼠标事件
			glbInt8 moveflag;	//鼠标状态
			glbDouble push_pitch;
			osg::ref_ptr<osg::Geode> _geode;
		};
		/**
		* @brief 私有类，导航盘回调
		* @version 1.0
		* @author  敖建
		* @date    2014-5-30 14:21
		*/
		class CGlbGlobeViewNavigatorUpdateCallback : public osg::NodeCallback  
		{
		public:
			/**
			* @brief 构造函数
			*/
			CGlbGlobeViewNavigatorUpdateCallback(CGlbGlobeView* pview,CGlbGlobe* pglobe)
			{
				mpr_p_glbView = pview;
				mpr_p_globe = pglobe;
			}
			/**
			* @brief 析构函数
			*/
			~CGlbGlobeViewNavigatorUpdateCallback(){}
			/**
			* @brief 回调函数
			*/
			void operator()(osg::Node* node, osg::NodeVisitor* nv);
		private:
			/*glbref_ptr<CGlbGlobeView>*/CGlbGlobeView*	mpr_p_glbView;	//视图
			/*glbref_ptr<CGlbGlobe>*/    CGlbGlobe*		mpr_p_globe;		//场景
		};
	public:
		/**
		* @brief 构造函数
		*/
		CGlbGlobeViewNavigator(CGlbGlobeView* pview,int width,int height,CGlbGlobe* pglobe);
		/**
		* @brief 析构函数
		*/
		~CGlbGlobeViewNavigator(void);
		/**
		* @brief 设置导航盘位置
		* @param [in] width : 屏幕宽度
		*		  [in] height : 屏幕高度
		*/
		void SetPosition(int left,int top,int width,int height);
	private:
		/**
		* @brief 创建导航盘节点
		*/
		osg::ref_ptr<osg::Geode> createViewNavigator();
	};
}