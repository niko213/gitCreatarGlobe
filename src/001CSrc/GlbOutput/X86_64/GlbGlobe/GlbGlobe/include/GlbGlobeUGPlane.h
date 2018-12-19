/********************************************************************
* Copyright (c) 2014 北京超维创想信息技术有限公司
* All rights reserved.
*
* @file    GlbGlobeUGPlane.h
* @brief   地下参考面类头文件
*
* @version 1.0
* @author  敖建
* @date    2014-6-19 10:09
*********************************************************************/
#pragma once
#include <osg/Texture2D>

namespace GlbGlobe
{
	class CGlbGlobe;
	class CGlbGlobeView;
	/**
	* @brief 地下参考面类				 
	* @version 1.0
	* @author  敖建
	* @date    2014-6-19 10:09
	*/
	class GLB_DLLCLASS_EXPORT CGlbGlobeUGPlane : public CGlbReference
	{		
	public:
		/**
		* @brief 构造函数
		*/
		CGlbGlobeUGPlane(CGlbGlobe* pglobe);
		/**
		* @brief 析构函数
		*/
		~CGlbGlobeUGPlane(void); 
		/**
		* @brief 计算屏幕上某点(x,y)处的地下参考面坐标      
		*	@param [in] x 屏幕点的x坐标
		* @param [in] y 屏幕点的y坐标
		* @param [out] lonOrX 经度或X
		* @param [out] latOrY 纬度或Y
		* @param [out] altOrZ 高度或Z
		* @note 输入点(x,y)为屏幕坐标,即窗口左下角点处为原点（0,0）
		- GLB_GLOBETYPE_GLOBE球模式下 lonOrX,latOrY单位为度
		- GLB_GLOBETYPE_FLAT平面模式下 lonOrX,latOrY单位为米	
		* @return 成功返回true
		*		-	失败返回false
		*/
		glbBool ScreenToUGPlaneCoordinate(glbInt32 x,glbInt32 y,glbDouble &lonOrx,glbDouble &latOry,glbDouble &altOrz);
		/**
		* @brief 计算射线是否与地下参考面相交      
		*	@param [in] ln_pt1射线上一点(端点)
		* @param [in] ln_dir射线方向
		* @param [out] InterPos 如果相交，交点坐标	
		* @return 相交返回true
		*		-	不相交返回false
		*/
		glbBool IsRayInterUGPlane(osg::Vec3d ln_pt1, osg::Vec3d ln_dir, osg::Vec3d &InterPos);

		/**
		* @brief 获取地下参考面结点      
		*/
		osg::Node* getNode();
		/**
		* @brief 设置地下参考面高度
		*/
		void SetAltitude(glbDouble altitude);

		/**
		* @brief 设置过滤显示标识
		* @param [in] isFilter默认为false，设置为true当开启slaveCamera时显示地下参考面
		*/
		void SetShowFilter(glbBool isFilter);
	private: 
		CGlbGlobe* mpr_p_globe;
		osg::ref_ptr<osg::Group> mpr_UGPlane;
		/*CGlbUGPlaneCallBack**/osg::NodeCallback* mpr_p_ugPlaneCallback;
	};
}