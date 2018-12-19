/********************************************************************
* Copyright (c) 2014 北京超维创想信息技术有限公司
* All rights reserved.
*
* @file    GlbPlanePathManipulator.h
* @brief   平面模式下路径漫游操控器类头文件
*
* @version 1.0
* @author  敖建
* @date    2014-7-9 10:26
*********************************************************************/
#pragma once
#include "GlbGlobeDynamic.h"
#include "GlbGlobeManipulatorManager.h"

namespace GlbGlobe
{ 
	/**
	* @brief 平面模式下路径漫游操控器类				 
	* @version 1.0
	* @author  敖建
	* @date    2014-7-9 10:28
	*/
	class GLB_DLLCLASS_EXPORT CGlbPlanePathManipulator : public osgGA::CameraManipulator,
		public IGlbGlobeManipulator					   
	{	
	public:
		/**
		* @brief 构造函数
		*/
		CGlbPlanePathManipulator(CGlbGlobe* globe,CGlbGlobeDynamic *dynamicObj);
		/**
		* @brief 析构函数
		*/
		~CGlbPlanePathManipulator(void);

		//osgGA::CameraManipulator method
	public:
		/**
		* @brief 获取相机矩阵
		*/
		virtual osg::Matrixd getMatrix() const;

		/**
		* @brief 设置相机矩阵
		*/
		virtual void setByMatrix(const osg::Matrixd& matrix);

		/**
		* @brief 获取相机逆矩阵
		*/
		virtual osg::Matrixd getInverseMatrix() const;

		/**
		* @brief 设置相机逆矩阵
		*/
		virtual void setByInverseMatrix(const osg::Matrixd& matrix);

		/**
		* @brief 事件回调
		*/
		virtual glbBool handle(const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter& aa);

		/**
		* @brief 获取操控器类型
		*/
		GlbGlobeManipulatorTypeEnum GetType();
		/**
		* @brief 飞向
		* @param [in] 目标点状态：焦点位置xOrlon,yOrlat,zOrAlt
		*		  [in] 目标点状态：相机焦点距离distance,焦点处yaw,pitch
		*		  [in] 飞行时长 seconds
		* @return 成功返回true
		*		-	失败返回false
		*/
		glbBool FlyTo(glbDouble xOrlon, glbDouble yOrlat, glbDouble zOrAlt, glbDouble distance, glbDouble yaw, glbDouble pitch, glbDouble seconds);

		/**
		* @brief 环绕飞行
		* @param [in] 环绕飞行中心点坐标：xOrlon,yOrlat,zOrAlt
		*		  [in] 飞行时长 seconds
		* @return 成功返回true
		*		-	失败返回false
		*/
		glbBool FlyAround(glbDouble xOrlon, glbDouble yOrlat, glbDouble zOrAlt, glbDouble seconds, bool repeat);

		/**
		* @brief 判断是否正在飞行状态中
		* @return 是返回true
		*		-	否返回false
		*/
		glbBool IsFlying();

		/**
		* @brief 停止飞行
		* @return 成功返回true
		*		-	失败返回false
		*/
		glbBool StopFlying();

		/**
		* @brief 跳向
		* @param [in] 目标点状态：焦点位置xOrlon,yOrlat,zOrAlt
		*		  [in] 目标点状态：相机焦点距离distance,焦点处yaw,pitch
		* @return 成功返回true
		*		-	失败返回false
		*/
		glbBool JumpTo(glbDouble xOrlon, glbDouble yOrlat, glbDouble zOrAlt, glbDouble distance, glbDouble yaw, glbDouble pitch);

		/**
		* @brief 设置在焦点处偏航角
		* @param [in] 旧焦点位置old_focus
		*		  [in] 旧相机焦点距离distance
		*		  [in] 焦点处偏航角yaw
		* @return 成功返回true
		*		-	失败返回false
		*/
		glbBool SetYaw(glbDouble yaw,osg::Vec3d old_focus, glbDouble olddistance);

		/**
		* @brief 获取焦点处偏航角
		* @return 焦点处偏航角
		*/
		glbDouble GetYaw();

		/**
		* @brief 设置在焦点处俯仰角
		* @param [in] 旧焦点位置old_focus
		*		  [in] 旧相机焦点距离distance
		*		  [in] 焦点处俯仰角pitch
		* @return 成功返回true
		*		-	失败返回false
		*/
		glbBool SetPitch(glbDouble pitch,osg::Vec3d old_focus,glbDouble olddistance);

		/**
		* @brief 获取焦点处俯仰角
		* @return 焦点处俯仰角
		*/
		glbDouble GetPitch();

		/**
		* @brief 获取相机位置
		* @param [out] 相机位置 cameraPos
		* @return 成功返回true
		*		-	失败返回false
		*/
		glbBool GetCameraPos(osg::Vec3d& cameraPos);
		/**
		* @brief 设置相机偏航角
		*/
		void SetCameraYaw(glbDouble yaw);
		/**
		* @brief 获取相机偏航角
		* @return 相机偏航角
		*/
		glbDouble GetCameraYaw();
		/**
		* @brief 设置相机俯仰角
		*/
		void SetCameraPitch(glbDouble pitch);
		/**
		* @brief 获取相机俯仰角
		* @return 相机俯仰角
		*/
		glbDouble GetCameraPitch();
		/**
		* @brief 获取焦点位置
		* @param [out] 焦点位置 cameraPos
		* @return 成功返回true
		*		-	失败返回false
		*/
		glbBool GetFocusPos(osg::Vec3d& focusPos);

		/**
		* @brief 获取焦点相机距离
		* @return 焦点相机距离，单位米
		*/
		glbDouble GetDistance();

		/**
		* @brief 相机在地形上平移
		* @param [in] 2个垂直方向上的平移量：dangleX，dangleY
		* @return 成功返回true
		*		-	失败返回false
		*/
		glbBool Move(glbDouble dangleX,glbDouble dangleY);

		//2014.11.24 aj添加，给导航条drag调用
		/**
		* @brief 相机平移
		* @param [in] x,y,z相机平移向量，世界坐标系下
		* @return 成功返回 true
		*			失败返回false
		*/
		glbBool TranslateCamera(glbDouble x,glbDouble y,glbDouble z);

		/**
		* @brief 开启关闭碰撞检测
		* @param [in] true为开启碰撞检测
		*			   false为关闭碰撞检测
		*/
		glbBool DetectCollision(glbBool mode);

		/**
		* @brief 是否开启碰撞检测
		* @return 是返回true
		*		-	否返回false
		*/
		//virtual glbBool IsDetectCollision();

		/**
		* @brief 开启关闭地下模式
		* @param [in] true为开启地下模式
		*			   false为关闭地下模式
		*/
		glbBool SetUnderGroundMode(glbBool mode);

		/**
		* @brief 是否开启地下模式
		* @return 是返回true
		*		-	否返回false
		*/
		//virtual glbBool IsUnderGroundMode();

		/**
		* @brief 设置地下参考面深度
		* @param [in] zOrAlt地下参考面深度
		* @return 成功返回true
		*		-	失败返回false
		*/
		glbBool SetUndergroundDepth(glbDouble zOrAlt);

		/**
		* @brief 纠正相机海拔/高度
		* @return 成功返回true
		*		-	失败返回false
		*/
		glbBool UpdateCameraElevate(glbDouble elevation);

		/**
		* @brief 焦点纠偏
		* @return 成功返回true
		*		-	失败返回false
		*/
		glbBool UpdateFocusElevate(glbDouble elevation);
		// 相机抖动
		/**
		* @brief 获取是否启用了相机抖动
		* @return	启动相机抖动 返回true
					没有相机抖动 返回false
		*/
		void Shake(glbBool isShake);
		/*
		* @brief 设置 相机抖动幅度
		* @param [in] horizontalDegree 水平方向抖动幅度 球面模式 单位是【度】 默认为2度  平面模式单位是【米】 
	    		 [in] verticalDegree 水平方向抖动幅度 球面模式 单位是【度】 默认为2度  平面模式单位是【米】 
		*/
		void SetShakeSope(glbDouble horizontalDegree, glbDouble verticalDegree);
		/*
		* @brief 设置 相机抖动 时间
 		* @param [in] seconds 相机抖动时间 单位： 秒 
		*/
		void SetShakeTime(glbDouble seconds);

		// 虚拟参考面
		/*
		* @brief 启用/禁止 虚拟参考面
 		* @param [in] bEnable    启用 ： true， 禁止 ： false 
		*/
		void EnalbeVirtualReferencePlane(glbBool bEnable);
		/*
		* @brief 设置 虚拟参考面高度
 		* @param [in] zOrAltitude 虚拟参考面高度  
		*/
		void SetVirtualReferencePlane(glbDouble zOrAltitude);
		/*
		* @brief 获取 虚拟参考面高度
 		* @return 虚拟参考面高度  
		*/
		glbDouble GetVirtualReferencePlane();
	private:
		CGlbGlobe* mpr_globe;
		CGlbGlobeDynamic* mpr_dynamicObj;			//动态对象
		glbDouble mpr_pitch;							//俯仰角
		glbDouble mpr_yaw;								//偏航角
		glbDouble mpr_roll;								//滚转角
		osg::Vec3d mpr_focusPos;						//焦点位置
		glbDouble mpr_distance;							//焦点相机距离
		osg::AnimationPath::ControlPoint mpr_cp;
	};
}