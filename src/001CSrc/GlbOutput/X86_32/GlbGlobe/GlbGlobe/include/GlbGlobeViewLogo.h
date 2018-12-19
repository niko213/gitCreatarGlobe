/********************************************************************
* Copyright (c) 2014 北京超维创想信息技术有限公司
* All rights reserved.
*
* @file    GlbGlobeViewLogo.h
* @brief   视图相关元素LOGO
*
* @version 1.0
* @author  敖建
* @date    2014-6-9 10:29
*********************************************************************/
#pragma once
#include "GlbGlobeView.h"
#include "GlbWString.h"
#include <osg/Camera>
#include <osgViewer/View>
namespace GlbGlobe
{
	class CGlbGlobe;
	class CGlbGlobeView;
	/**
	* @brief 视图相关元素LOGO类
	* @version 1.0
	* @author  敖建
	* @date    2014-6-9 10:43
	*/
	class GLB_DLLCLASS_EXPORT CGlbGlobeViewLogo : public osg::Camera
	{
		/**
		* @brief 私有类，状态栏回调
		* @version 1.0
		* @author  敖建
		* @date    2014-5-27 10:01
		*/
		class CGlbGlobeLogoUpdateCallback : public osg::NodeCallback  
		{
		public:
			/**
			* @brief 构造函数
			*/
			CGlbGlobeLogoUpdateCallback()
			{
				positive = true;
			}
			/**
			* @brief 析构函数
			*/
			~CGlbGlobeLogoUpdateCallback(){}
			/**
			* @brief 回调函数
			*/
			void operator()(osg::Node* node, osg::NodeVisitor* nv);			
		private:
			bool positive;
		};

	public:
		/**
		* @brief 构造函数
		*/	
		//CGlbGlobeViewLogo(CGlbGlobeView* pview,int width,int height,CGlbGlobe* pglobe);

		/**
		* @brief构造函数                                                      
		* @param [in] width : 屏幕宽度
		*		  [in] height : 屏幕高度
		*		  [in] label : logo 文字
		*		  [in] labelColor : logo文字颜色
		*		  [in] image	  : 图片
		*		  [in] bDynamic   : 是否左右循环运动
		*/
		CGlbGlobeViewLogo(glbInt32 width,glbInt32 height,const glbWChar* label,glbInt32 labelColor,const glbWChar* image, glbBool bDynamic);
		/**
		* @brief 析构函数
		*/
		~CGlbGlobeViewLogo(void);
		/**
		* @brief 设置LOGO位置
		* @param [in] width : 屏幕宽度
		*		  [in] height : 屏幕高度
		*/
		void SetPosition(glbInt32 left,glbInt32 top,glbInt32 width,glbInt32 height);
		/**
		* @brief 设置LOGO纹理
		* @param [in] imageData : 图片全路径
		*/
		void SetImageLogo(CGlbWString path);
		/**
		* @brief 设置LOGO文字
		* @param [in] label : 文字
		*/
		void SetLabel(const glbWChar* label,glbInt32 labelColor);

	private:
		/**
		* @brief 创建LOGO节点
		*/
		osg::ref_ptr<osg::Geode> createViewLogo();
	private:
		osg::ref_ptr<osg::Geode> mpr_geode;
		glbInt32				 mpr_posLeft;
		glbInt32				 mpr_posTop;
		glbInt32				 mpr_width;
		glbInt32				 mpr_height;
		CGlbWString				 mpr_label;
		glbInt32				 mpr_labelColor;
		CGlbWString				 mpr_imagePath;
		glbBool					 mpr_bInitialized;
	};
}
