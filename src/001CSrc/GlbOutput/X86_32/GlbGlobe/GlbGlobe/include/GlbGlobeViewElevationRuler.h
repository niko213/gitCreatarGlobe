/********************************************************************
* Copyright (c) 2014 北京超维创想信息技术有限公司
* All rights reserved.
*
* @file    GlbGlobeViewElevationRuler.h
* @brief   视图相关元素导航盘标尺
*
* @version 1.0
* @author  敖建
* @date    2014-6-6 16:40
*********************************************************************/
#pragma once
#include "GlbGlobeView.h"
#include <osg/Camera>
#include <osgViewer/View>
namespace GlbGlobe
{
	/** 
	*枚举 导航标尺位置枚举类型
	*/
	enum GlbGlobeElevationRulerPositionType
	{
		ELEVATIONRULER_NONE_BTN		= 0,		//其他位置
		ELEVATIONRULER_CURSOR_BTN	= 1,		//游标 
		ELEVATIONRULER_UP_BTN		= 2,		//往上
		ELEVATIONRULER_DOWN_BTN		= 3,		//往下
		ELEVATIONRULER_EARTH_BTN	= 4,		//全球
		ELEVATIONRULER_COUNTRY_BTN	= 5,		//国家
		ELEVATIONRULER_PROVINCE_BTN	= 6,		//省
		ELEVATIONRULER_CITY_BTN		= 7,		//城市
		ELEVATIONRULER_STREET_BTN	= 8,		//街道
		ELEVATIONRULER_HOUSE_BTN	= 9,		//房屋
		ELEVATIONRULER_UG_BTN		= 10,		//地下
		ELEVATIONRULER_OUT_BTN		= 11		//导航盘外
	};
	class CGlbGlobe;
	class CGlbGlobeView;
	/**
	* @brief 视图相关元素导航标尺类
	* @version 1.0
	* @author  敖建
	* @date    2014-6-6 16:46
	*/
	class GLB_DLLCLASS_EXPORT CGlbGlobeViewElevationRuler : public osg::Camera
	{
		/**
		* @brief 私有类，导航标尺事件回调
		* @version 1.0
		* @author  敖建
		* @date    2014-6-9 11:17
		*/
		class CGlbGlobeViewElevationRulerEventHandler : public osgGA::GUIEventHandler
		{
		public:
			/**
			* @brief 构造函数
			*/
			CGlbGlobeViewElevationRulerEventHandler(CGlbGlobeView* pview,CGlbGlobe* pglobe,osg::Geode* pgeode)
			{
				mpr_p_view = pview;
				mpr_p_globe = pglobe;
				_ga_t0 = NULL;
				_ga_t1 = NULL;
				_geode = pgeode;
			}
			/**
			* @brief 析构函数
			*/
			~CGlbGlobeViewElevationRulerEventHandler(){}
			/**
			* @brief 事件回调
			*/
			glbBool handle(const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter& aa);
		private:
			/**
			* @brief 获取鼠标在导航盘上的枚举位置
			* @param [in] ea 鼠标事件
			* @return GlbGlobeNavigatorPositionType 鼠标在导航盘上的位置的枚举类型
			*/
			GlbGlobeElevationRulerPositionType getElevationRulerPositionType(const osgGA::GUIEventAdapter& ea);
			/**
			* @brief 获取鼠标在导航盘上的枚举位置
			* @param [in] ea 鼠标事件
			* @return GlbGlobeNavigatorPositionType 鼠标在导航盘上的位置的枚举类型
			*/
			GlbGlobeElevationRulerPositionType getTouchElevationRulerPositionType(const osgGA::GUIEventAdapter& ea);
			/**
			* @brief 飞行到distan_target高度附近
			*/
			glbBool FlyTo(glbDouble distan_target); 
			/**
			* @brief 将相机位置转化为游标量
			*/
			glbDouble DeltToDistance(glbDouble delt);
			/**
			* @brief 将游标量转化为相机位置
			*/
			glbDouble DistanceToDelt(glbDouble distan);
			/**
			* @brief 触控事件处理
			* @return 返回true ：view需重绘
			*		-	返回false ：不需重绘
			*/
			glbBool handleTouch( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa );
		private:
			CGlbGlobeView* mpr_p_view;
			CGlbGlobe* mpr_p_globe;
			osg::ref_ptr<const osgGA::GUIEventAdapter> _ga_t1;	//当前的前一个鼠标事件(主要用来存鼠标按下的那个事件)
			osg::ref_ptr<const osgGA::GUIEventAdapter> _ga_t0;	//当前鼠标事件
			osg::ref_ptr<osg::Geode> _geode;
		};

		/**
		* @brief 私有类，导航标尺回调
		* @version 1.0
		* @author  敖建
		* @date    2014-6-9 14:36
		*/
		class CGlbGlobeViewElevationRulerUpdateCallback : public osg::NodeCallback  
		{
		public:
			/**
			* @brief 构造函数
			*/
			CGlbGlobeViewElevationRulerUpdateCallback(CGlbGlobeView* pview,CGlbGlobe* pglobe)
			{
				mpr_p_glbView = pview;
				mpr_p_globe = pglobe;
			}
			/**
			* @brief 析构函数
			*/
			~CGlbGlobeViewElevationRulerUpdateCallback(){}
			/**
			* @brief 回调函数
			*/
			void operator()(osg::Node* node, osg::NodeVisitor* nv);
		private:
			/*glbref_ptr<CGlbGlobeView>*/CGlbGlobeView*	mpr_p_glbView;		//视图
			/*glbref_ptr<CGlbGlobe>*/	 CGlbGlobe*	    mpr_p_globe;		//场景
			/**
			* @brief 将游标量转化为相机位置
			*/
			glbDouble DistanceToDelt(glbDouble distan);
		};
	public:
		/**
		* @brief 构造函数
		*/
		CGlbGlobeViewElevationRuler(CGlbGlobeView* pview,int width,int height,CGlbGlobe* pglobe);
		/**
		* @brief 析构函数
		*/
		~CGlbGlobeViewElevationRuler(void);
		/**
		* @brief 设置导航盘标尺位置
		* @param [in] width : 屏幕宽度
		*		  [in] height : 屏幕高度
		*/
		void SetPosition(int left,int top,int width,int height);
	private:
		/**
		* @brief 创建导航盘标尺标尺部分节点
		*/
		osg::ref_ptr<osg::Geode> createViewElevationRuler();
		/**
		* @brief 创建导航盘标尺文字部分节点
		*/
		osg::ref_ptr<osg::Geode> createViewElevationRulerText();
	};
}

