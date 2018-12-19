/********************************************************************
* Copyright (c) 2014 北京超维创想信息技术有限公司
* All rights reserved.
*
* @file    GlbGlobeViewScale.h
* @brief   视图相关元素比例尺
*
* @version 1.0
* @author  敖建
* @date    2014-6-9 10:30
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
	* @brief 视图相关元素比例尺类
	* @version 1.0
	* @author  敖建
	* @date    2014-6-9 10:43
	*/
	class GLB_DLLCLASS_EXPORT CGlbGlobeViewScale : public osg::Camera
	{
		/**
		* @brief 私有类，比例尺回调
		* @version 1.0
		* @author  敖建
		* @date    2014-9-5 15:31
		*/
		class CGlbGlobeViewScaleUpdateCallback : public osg::NodeCallback  
		{
		public:
			/**
			* @brief 构造函数
			*/
			CGlbGlobeViewScaleUpdateCallback(CGlbGlobeViewScale *viewScale,int width,int height)
			{
				mpr_p_glbView = NULL;
				mpr_p_globe = NULL;
				mpr_width = width;
				mpr_height = height;
				mpr_viewScale = viewScale;
			}
			/**
			* @brief 析构函数
			*/
			~CGlbGlobeViewScaleUpdateCallback(){}
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
			/**
			* @brief 求球面上2点夹角
			* param [in] lat1,lat2 : 点的纬度
			*		 [in] lon1,lon2 : 点的经度 
			* @return p1-O-p2夹角
			*/
			inline glbDouble computeGlobeAngle(glbDouble lat1,glbDouble lon1,glbDouble lat2,glbDouble lon2)
			{
				//arc cos[cosβ1cosβcos（α1-α2）+sinβ1sinβ2]
				glbDouble temp = cos(osg::DegreesToRadians(lat1))*cos(osg::DegreesToRadians(lat2))*cos(osg::DegreesToRadians(lon1-lon2))+sin(osg::DegreesToRadians(lat1))*sin(osg::DegreesToRadians(lat2));
				if (temp > 1.0)
					temp = 1.0;
				else if (temp < -1.0)
					temp = -1.0;
				return acos(temp);
			}
		private:
			/*glbref_ptr<CGlbGlobeView>*/CGlbGlobeView*	mpr_p_glbView;	//视图
			/*glbref_ptr<CGlbGlobe>*/	 CGlbGlobe*	mpr_p_globe;	//场景
			glbInt32 mpr_width;
			glbInt32 mpr_height;
			CGlbGlobeViewScale *mpr_viewScale;
			osg::Vec3d mpr_cameraPos;
		};
	public:
		/**
		* @brief 构造函数
		*/
		CGlbGlobeViewScale(CGlbGlobeView* pview,int width,int height,CGlbGlobe* pglobe);
		/**
		* @brief 析构函数
		*/
		~CGlbGlobeViewScale(void);
		/**
		* @brief 设置比例尺位置
		* @param [in] width : 屏幕宽度
		*		  [in] height : 屏幕高度
		*/
		void SetPosition(int left,int top,int width,int height);

		void UpdateScale(glbDouble scale){ mpr_scale = scale;}

		glbDouble GetCurrentScale(){ return mpr_scale;}
	private:
		/**
		* @brief 创建比例尺节点
		*/
		osg::ref_ptr<osg::Geode> createViewScale();
	private:
		glbDouble mpr_scale;		
	};
}