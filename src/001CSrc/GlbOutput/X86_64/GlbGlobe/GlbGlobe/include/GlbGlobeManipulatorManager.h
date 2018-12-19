/********************************************************************
* Copyright (c) 2014 北京超维创想信息技术有限公司
* All rights reserved.
*
* @file    GlbGlobeManipulatorManager.h
* @brief   操控器管理类头文件
*
* @version 1.0
* @author  敖建
* @date    2014-5-12 14:14
*********************************************************************/

#pragma once
#include "CGlbGlobe.h"
#include "GlbGlobeExport.h"
#include <osgGA/CameraManipulator>

namespace GlbGlobe
{
	/**
	*枚举 操控器枚举类型
	*/
	enum GlbGlobeManipulatorTypeEnum
	{
		GLB_MANIPULATOR_UNKNOWN = -1,		//未知操控器
		GLB_MANIPULATOR_FREE = 0,			//自由操控器
		GLB_MANIPULATOR_DRIVE = 1,			//驾驶操控器
		GLB_MANIPULATOR_ANIMATIONPATH = 2	//路径操控器【第一人称操控器】
	};

	/**
	*枚举 自由操控器模式下锁定Type
	*/
	enum GlbGlobeLockModeEnum
	{
		GLB_LOCKMODE_FREE = 0,/**3D,无锁定 */
		GLB_LOCKMODE_2D   = 1,/**2D,锁定pitch角为-90°*/
		GLB_LOCKMODEL_2DN = 2/**2D,锁定North，yaw角为0°*/
	};

	/**
	* @brief 操控器接口定义类	 
	* @version 1.0
	* @author  敖建
	* @date    2014-5-12 14:32
	*/	
	class GLB_DLLCLASS_EXPORT IGlbGlobeManipulator/* : public CGlbReference*/
	{
	public:
		/**
		* @brief IGlbGeometry析构函数
		* 没有任何参数及传回值
		*/
		virtual ~IGlbGlobeManipulator(){};

		/**
		* @brief 获取操控器类型
		*/
		virtual GlbGlobeManipulatorTypeEnum GetType() = 0;
		/**
		* @brief 飞向
		* @param [in] 目标点状态：焦点位置xOrlon,yOrlat,zOrAlt
		*		  [in] 目标点状态：相机焦点距离distance,焦点处yaw,pitch
		*		  [in] 飞行时长 seconds
		* @return 成功返回true
		*		-	失败返回false
		*/
		virtual glbBool FlyTo(glbDouble xOrlon, glbDouble yOrlat, glbDouble zOrAlt, glbDouble distance, glbDouble yaw, glbDouble pitch, glbDouble seconds) = 0;

		/**
		* @brief 环绕飞行
		* @param [in] 环绕飞行中心点坐标：xOrlon,yOrlat,zOrAlt
		*		  [in] 飞行时长 seconds
		* @return 成功返回true
		*		-	失败返回false
		*/
		virtual glbBool FlyAround(glbDouble xOrlon, glbDouble yOrlat, glbDouble zOrAlt, glbDouble seconds, bool repeat) = 0;

		/**
		* @brief 判断是否正在飞行状态中
		* @return 是返回true
		*		-	否返回false
		*/
		virtual glbBool IsFlying() = 0;

		/**
		* @brief 停止飞行
		* @return 成功返回true
		*		-	失败返回false
		*/
		virtual glbBool StopFlying() = 0;

		/**
		* @brief 跳向
		* @param [in] 目标点状态：焦点位置xOrlon,yOrlat,zOrAlt
		*		  [in] 目标点状态：相机焦点距离distance,焦点处yaw,pitch
		* @return 成功返回true
		*		-	失败返回false
		*/
		virtual glbBool JumpTo(glbDouble xOrlon, glbDouble yOrlat, glbDouble zOrAlt, glbDouble distance, glbDouble yaw, glbDouble pitch) = 0;

		/**
		* @brief 设置在焦点处偏航角
		* @param [in] 旧焦点位置old_focus
		*		  [in] 旧相机焦点距离distance
		*		  [in] 焦点处偏航角yaw
		* @return 成功返回true
		*		-	失败返回false
		*/
		virtual glbBool SetYaw(glbDouble yaw,osg::Vec3d old_focus, glbDouble olddistance) = 0;

		/**
		* @brief 获取焦点处偏航角
		* @return 焦点处偏航角
		*/
		virtual glbDouble GetYaw() = 0;

		/**
		* @brief 设置在焦点处俯仰角
		* @param [in] 旧焦点位置old_focus
		*		  [in] 旧相机焦点距离distance
		*		  [in] 焦点处俯仰角pitch
		* @return 成功返回true
		*		-	失败返回false
		*/
		virtual glbBool SetPitch(glbDouble pitch,osg::Vec3d old_focus,glbDouble olddistance) = 0;

		/**
		* @brief 获取焦点处俯仰角
		* @return 焦点处俯仰角
		*/
		virtual glbDouble GetPitch() = 0;

		/**
		* @brief 获取相机位置
		* @param [out] 相机位置 cameraPos
		* @return 成功返回true
		*		-	失败返回false
		*/
		virtual glbBool GetCameraPos(osg::Vec3d& cameraPos) = 0;	
		/**
		* @brief 设置相机偏航角
		*/
		virtual void SetCameraYaw(glbDouble yaw)=0;
		/**
		* @brief 获取相机偏航角
		* @return 相机偏航角
		*/
		virtual glbDouble GetCameraYaw()=0;
		/**
		* @brief 设置相机俯仰角
		*/
		virtual void SetCameraPitch(glbDouble pitch)=0;
		/**
		* @brief 获取相机俯仰角
		* @return 相机俯仰角
		*/
		virtual glbDouble GetCameraPitch()=0;
		/**
		* @brief 获取焦点位置
		* @param [out] 焦点位置 cameraPos
		* @return 成功返回true
		*		-	失败返回false
		*/
		virtual glbBool GetFocusPos(osg::Vec3d& focusPos) = 0;

		/**
		* @brief 获取焦点相机距离
		* @return 焦点相机距离，单位米
		*/
		virtual glbDouble GetDistance() = 0;

		/**
		* @brief 相机在地形上平移
		* @param [in] 2个垂直方向上的平移量：dangleX，dangleY
		* @return 成功返回true
		*		-	失败返回false
		*/
		virtual glbBool Move(glbDouble dangleX,glbDouble dangleY) = 0;

		//2014.11.24 aj添加，给导航条drag调用
		/**
		* @brief 相机平移
		* @param [in] x,y,z相机平移向量，世界坐标系下
		* @return 成功返回 true
		*			失败返回false
		*/
		virtual glbBool TranslateCamera(glbDouble x,glbDouble y,glbDouble z) = 0;

		/**
		* @brief 是否开启碰撞检测
		* @return 是返回true
		*		-	否返回false
		*/
		//virtual glbBool IsDetectCollision() = 0;

		/**
		* @brief 开启关闭地下模式
		* @param [in] true为开启地下模式
		*			   false为关闭地下模式
		*/
		virtual glbBool SetUnderGroundMode(glbBool mode) = 0;

		/**
		* @brief 是否开启地下模式
		* @return 是返回true
		*		-	否返回false
		*/
		//virtual glbBool IsUnderGroundMode() = 0;

		/**
		* @brief 设置地下参考面深度
		* @param [in] zOrAlt地下参考面深度
		* @return 成功返回true
		*		-	失败返回false
		*/
		virtual glbBool SetUndergroundDepth(glbDouble zOrAlt) = 0;

		/**
		* @brief 纠正相机海拔/高度
		* @return 成功返回true
		*		-	失败返回false
		*/
		virtual glbBool UpdateCameraElevate(glbDouble elevation) = 0;

		/**
		* @brief 焦点纠偏
		* @return 成功返回true
		*		-	失败返回false
		*/
		virtual glbBool UpdateFocusElevate(glbDouble elevation) = 0;

		// 相机抖动
		/**
		* @brief 获取是否启用了相机抖动
		* @return	启动相机抖动 返回true
					没有相机抖动 返回false
		*/
		virtual void Shake(glbBool isShake) = 0;
		/*
		* @brief 设置 相机抖动幅度
		* @param [in] horizontalDegree 水平方向抖动幅度 球面模式 单位是【度】 默认为2度  平面模式单位是【米】 
	    		 [in] verticalDegree 水平方向抖动幅度 球面模式 单位是【度】 默认为2度  平面模式单位是【米】 
		*/
		virtual void SetShakeSope(glbDouble horizontalDegree, glbDouble verticalDegree) = 0;
		/*
		* @brief 设置 相机抖动 时间
 		* @param [in] seconds 相机抖动时间 单位： 秒 
		*/
		virtual void SetShakeTime(glbDouble seconds) = 0;

		// 虚拟参考面
		/*
		* @brief 启用/禁止 虚拟参考面
 		* @param [in] bEnable    启用 ： true， 禁止 ： false 
		*/
		virtual void EnalbeVirtualReferencePlane(glbBool bEnable) = 0;
		/*
		* @brief 设置 虚拟参考面高度
 		* @param [in] zOrAltitude 虚拟参考面高度  
		*/
		virtual void SetVirtualReferencePlane(glbDouble zOrAltitude) = 0;
		/*
		* @brief 获取 虚拟参考面高度
 		* @return 虚拟参考面高度  
		*/
		virtual glbDouble GetVirtualReferencePlane() = 0;


	};


	/**
	* @brief 操控器管理类				 
	* @version 1.0
	* @author  敖建
	* @date    2014-5-12 14:50
	*/
	class CGlbGlobe;
	class CGlbGlobeDynamic;
	class GLB_DLLCLASS_EXPORT CGlbGlobeManipulatorManager :
		public IGlbGlobeManipulator,
		public osgGA::CameraManipulator
	{
	public:
		/**
		* @brief 构造函数
		* @param [in] CGlbGlobe* 场景指针
		* @return 无
		*/
		CGlbGlobeManipulatorManager(CGlbGlobe *globe);
		/**
		* @brief 构造函数
		*/
		virtual ~CGlbGlobeManipulatorManager(void);
		/**
		* @brief 启动自由操控器
		* @return 成功返回true
		*		-	失败返回false
		*/		
		glbBool SetDragMode();		
		/**
		* @brief 设置自由操控器锁定模式
		* @param [in] GlbGlobeLockModeType锁定模式枚举类型
		* @return 成功返回true
		*		-	失败返回false
		*/
		glbBool SetLockMode(GlbGlobeLockModeEnum glbType);
		/**
		* @brief 启动第一人称驾驶操控器
		* @return 成功返回true
		*		-	失败返回false
		*/
		glbBool SetDriveMode();
		/**
		* @brief 启动路径漫游操控器
		* @param [in] IGlbGlobeDynamicObject漫游路径动态对象
		* @return 成功返回true
		*		-	失败返回false
		*/
		glbBool SetPathMode(CGlbGlobeDynamic *obj);

		void setByMatrix(const osg::Matrixd& matrix);
		void setByInverseMatrix(const osg::Matrixd& matrix);
		osg::Matrixd getMatrix() const;
		osg::Matrixd getInverseMatrix() const;
		void home(const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter& aa);
		glbBool handle(const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter& us);

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
		/*
		*  @brief 设置相机参数
		**/
		void SetCameraParam(glbDouble yaw, glbDouble pitch, osg::Vec3d focusPos, glbDouble distance);

		/*
		* @brief定义 拖拽，旋转，缩放 三个动作
		*/
		void Drag(glbInt32 ptx1,glbInt32 pty1,glbInt32 ptx2,glbInt32 pty2);
		void Rotate(glbInt32 ptx1,glbInt32 pty1,glbInt32 ptx2,glbInt32 pty2);
		void Zoom(glbBool isScrollUp);

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
		//////////////////////////////////////////////
		// 目前以下接口只在第一人称中实现,后续其他操控器进行添加//
		//////////////////////////////////////////////
		/**
		* @brief 设置移动速度
		* @param [in] speed 速度
		*/
		void	SetDriveMoveSpeed(glbDouble speed);
		/**
		* @brief 返回移动速度
		* @return 
		*/
		glbDouble GetDriveMoveSpeed();

		/**
		* @brief 添加碰撞检测的模型对象
		* @param [in] obj :添加的模型对象
		* @return true：添加成功,false:添加失败
		*/
		glbBool AddCollisionObject(CGlbGlobeRObject *obj);

		/**
		* @brief 添加图层下的碰撞对象
		* @param [in] id :图层id
		* @param [in] obj：碰撞对象
		*/
		glbBool	AddCollisionLaysObject(glbInt32 layId,CGlbGlobeRObject* obj);
		/**
		* @brief 返回已经添加碰撞检测的模型对象或者图层的id
		* @param [out] objIds :已添加的模型对象id
		* @param [out] layIds :已添加的图层对象id
		*/
		void  GetHaveAddObjsAndLays(std::vector<glbInt32>& objIds,std::vector<glbInt32>& layIds);
		/**
		* @brief 删除碰撞检测的对象或图层
		* @param [in] id :被删除模型对象id
		* @param [in] lyrOrObj:删除是对象还是图层,true-图层 ，false-对象
		* @return true：删除成功,false:删除失败
		*/
		glbBool	RemoveCollisionObjectOrLayerById(int id,glbBool isLyr = false);

		/**
		* @brief 获取碰撞检测的模型对象的数目
		* @return 
		*/
		glbInt32 GetCollisionObjectCount();

		/**
		* @brief 依据对象ID获取碰撞对象
		* @param [in] idx：碰撞对象的index
		* @return 返回检索的碰撞对象,不存在返回null
		*/
		CGlbGlobeRObject* getCollisionObjectByObjId(int idx);

		/**
		* @brief 删除所有碰撞检测的模型对象
		* @return true:删除成功,false:删除失败
		*/
		glbBool	RemoveAllCollsionObject();	

	protected:
		CGlbGlobe* mpr_globe;								//场景指针
		IGlbGlobeManipulator* mpr_manipulator;				//操控器接口指针		
		GlbGlobeManipulatorTypeEnum mpr_glbManiType;		//操控器类型
	};
}
