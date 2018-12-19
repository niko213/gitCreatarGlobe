/********************************************************************
* Copyright (c) 2014 北京超维创想信息技术有限公司
* All rights reserved.
*
* @file    GlbGlobeClipHandler.h
* @brief   球模式下，相机裁剪面回调类
*
* @version 1.0
* @author  敖建
* @date    2014-6-11 10:00
*********************************************************************/
#pragma once
#include <osgUtil/CullVisitor>
#include "CGlbGlobe.h"
#include "GlbGlobeView.h"

//#define XinJiangTaLiMuPenDiXiangMu   1  // 定义了新疆塔里木盆地项目[超大面积多层地质体结构]

namespace GlbGlobe
{
	class CGlbGlobe;
	class CGlbGlobeView;
	/**
	* @brief 球模式下，相机裁剪面回调类
	* @version 1.0
	* @author  敖建
	* @date    2014-5-30 14:46
	*/
	class GLB_DLLCLASS_EXPORT CGlbGlobeClipHandler : public osg::NodeCallback
	{
	public:
		struct CustomProjClamper : public osg::CullSettings::ClampProjectionMatrixCallback
		{
			glbDouble _minNear;
			glbDouble _maxFar;
			glbDouble _nearFarRatio;
			glbBool _isFlat;
			glbDouble _minDistance;

#ifdef XinJiangTaLiMuPenDiXiangMu
			glbBool _bUnderGround;
			glbDouble _refFar;
#endif

			CustomProjClamper() : _minNear( -DBL_MAX )
				,_maxFar( DBL_MAX )
				,_nearFarRatio( 0.00015 )
				,_isFlat(false)
				,_minDistance(0.05)
			{ 
			}

			//???
			template<class matrix_type, class value_type>
			bool _clampProjectionMatrix(matrix_type& projection, double& znear, double& zfar, value_type nearFarRatio) const
			{
				double epsilon = 1e-6;
				if (zfar<znear-epsilon || _isnan(zfar) == 1)
				{
					OSG_INFO<<"_clampProjectionMatrix not applied, invalid depth range, znear = "<<znear<<"  zfar = "<<zfar<<std::endl;
					return false;
				}

				if (zfar<znear+epsilon)
				{
					double average = (znear+zfar)*0.5;
					znear = average-epsilon;
					zfar = average+epsilon;
				}

				if (fabs(projection(0,3))<epsilon  && fabs(projection(1,3))<epsilon  && fabs(projection(2,3))<epsilon )
				{
					value_type delta_span = (zfar-znear)*0.02;
					if (delta_span<1.0) delta_span = 1.0;
					value_type desired_znear = znear - delta_span;
					value_type desired_zfar = zfar + delta_span;
					if (_isFlat)
						desired_zfar += 2.0*(desired_zfar-desired_znear);
					znear = desired_znear;
					zfar = desired_zfar;

					projection(2,2)=-2.0f/(desired_zfar-desired_znear);
					projection(3,2)=-(desired_zfar+desired_znear)/(desired_zfar-desired_znear);
				}
				else
				{
					value_type zfarPushRatio = 1.02;
					value_type znearPullRatio = 0.98;
#ifdef XinJiangTaLiMuPenDiXiangMu
					{
						if (_bUnderGround && zfar < _refFar)
							zfar = _refFar;
					}
#endif
					value_type desired_znear = znear * znearPullRatio;
					value_type desired_zfar = zfar * zfarPushRatio;

					double min_near_plane = zfar*nearFarRatio;
					if (desired_znear<min_near_plane) desired_znear=min_near_plane;

					//double desired_zlen = zfar - min_near_plane;

					if ( desired_znear < _minDistance )//注：由0.01改为0.35以解决喜马拉雅位置远处碎边问题ml 2014.9.2
						desired_znear = _minDistance;//0.01;			

					znear = desired_znear;
					zfar = desired_zfar;

					value_type trans_near_plane = (-desired_znear*projection(2,2)+projection(3,2))/(-desired_znear*projection(2,3)+projection(3,3));
					value_type trans_far_plane = (-desired_zfar*projection(2,2)+projection(3,2))/(-desired_zfar*projection(2,3)+projection(3,3));

					value_type ratio = fabs(2.0/(trans_near_plane-trans_far_plane));
					value_type center = -(trans_near_plane+trans_far_plane)/2.0;

					projection.postMult(osg::Matrix(1.0f,0.0f,0.0f,0.0f,
						0.0f,1.0f,0.0f,0.0f,
						0.0f,0.0f,ratio,0.0f,
						0.0f,0.0f,center*ratio,1.0f));

				}
				return true;
			}


			bool clampProjectionMatrixImplementation(osg::Matrixf& projection, double& znear, double& zfar) const
			{
				double n = max( znear, _minNear );
				double f = min( zfar, _maxFar );
				bool r = _clampProjectionMatrix( projection, n, f, _nearFarRatio );
				if ( r ) {
					znear = n;
					zfar = f;
				}
				return r;
			}

			bool clampProjectionMatrixImplementation(osg::Matrixd& projection, double& znear, double& zfar) const
			{

				double n = max( znear, _minNear );
				double f = min( zfar, _maxFar );
				bool r = _clampProjectionMatrix( projection, n, f, _nearFarRatio );
				if ( r ) {
					znear = n;
					zfar = f;
				}
				return r;
			}
		};
	public:
		/**
		* @brief 构造函数
		*/
		CGlbGlobeClipHandler(CGlbGlobe* pglobe);
		/**
		* @brief 析构函数
		*/
		~CGlbGlobeClipHandler(void);
		/**
		* @brief 回调函数
		*/
		virtual void operator()( osg::Node* node, osg::NodeVisitor* nv );

		/*
		* @bief 设置nearfarRadio, eye_distance之间的关系 
		*/
		//void SetNearfarRadio(double nearfarRadio);
	private:
		glbBool loadUserFile(std::string filepath);
		glbBool mpr_isUserFileExist;
		std::map<double/*Altitude*/,double/*nearfarRatio*/> mpr_cameraAltitudeNearfarRatio; //undergroundmode下的相机距地面高和远近剪切面比率
	public:
		std::map<osg::Camera*, osg::ref_ptr<osg::CullSettings::ClampProjectionMatrixCallback>> mpr_p_clampers;	//相机和裁剪面
		CGlbGlobe* mpr_p_globe;
	};
}


