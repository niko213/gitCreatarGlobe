/********************************************************************
* Copyright (c) 2014 北京超维创想信息技术有限公司
* All rights reserved.
*
* @file    GlbPlaneManipulator.h
* @brief   平面模式下自由操控器类头文件
*
* @version 1.0
* @author  敖建
* @date    2014-5-13 10:37
*********************************************************************/

#pragma once
#include "GlbGlobeManipulatorManager.h"
#include "GlbCommTypes.h"
#include "IGlbGlobeViewEventHandle.h"

#include <osgGA/GUIEventAdapter>
#include <osgGA/GUIActionAdapter>
#include <osg/AnimationPath>

//////////////////////////////////////////////////////////////////////////
//坐标系定义：
//
//1.相机坐标系(lookat坐标系)：以相机为原点，z轴正方向指向相机焦点反方向，y轴正方向指向相机up方向，x轴与Oz,Oy轴垂直，右手系；此坐标系由初始camera的Lookat矩阵设置
//
//2.平面惯性系(场景中心原点系)：以大地块中心为原点，z轴正方向垂直场景参考平面向上，x轴正方向向东，y轴正方向向北，右手系
////////////////////////////////////////////////////////////////////////// 

namespace GlbGlobe
{
	class CGlbGlobe;
	class CGlbGlobeView;
	class GlbGlobeManipulatorManager;
	class IGlbGlobeManipulator;
	class IGlbGlobeViewEventHandle;
	class GLB_DLLCLASS_EXPORT CGlbPlaneManipulator :
		public IGlbGlobeManipulator
		,public osgGA::CameraManipulator
	{	
		/**
		* @brief KeyPoint关键点,FlyCallback中记录关键点的结构体	 
		* @version 1.0
		* @author  敖建
		* @date    2014-6-25 11:22
		*/
		struct KeyPoint
		{
			osg::Vec3d focusPos;
			glbDouble distan;
			glbDouble yaw;
			glbDouble pitch;
		};
	private:
		/**
		* @brief 平面模式自由操控器飞行回调，私有类	 
		* @version 1.0
		* @author  敖建
		* @date    2014-6-24 13:22
		*/
		class CGlbPlaneFlyCallback : public osg::NodeCallback
		{
		public:	
			/**
			* @brief 构造函数
			*/
			CGlbPlaneFlyCallback(KeyPoint pt0,KeyPoint pt1,glbDouble loopTime):
			  _manipulator(NULL),
				  _firstPoint(pt0),
				  _lastPoint(pt1),
				  _loopTime(loopTime),
				  _firstTime(DBL_MAX),
				  _latestTime(DBL_MAX),
				  _pauseTime(DBL_MAX),
				  _pause(false),
				  _isfar(false)
			  {  
				  _matrix.makeIdentity();
			  }
			  CGlbPlaneFlyCallback(CGlbPlaneManipulator* manipulator,glbDouble loopTime):
			  _manipulator(manipulator),
				  _loopTime(loopTime),
				  _firstTime(DBL_MAX),
				  _latestTime(DBL_MAX),
				  _pauseTime(DBL_MAX),
				  _pause(false),
				  _isfar(false)
			  {  
				  _matrix.makeIdentity();
			  }
			  /**
			  * @brief 析构函数
			  */
			  ~CGlbPlaneFlyCallback(){}
			  /**
			  * @brief 回调函数
			  */
			  virtual void operator()(osg::Node* node, osg::NodeVisitor* nv);
			  /**
			  * @brief 设置是否暂停
			  */
			  void setPause(glbBool pause);
			  /**
			  * @brief 获取是否暂停
			  */
			  glbBool getPause() const { return _pause; }
			  /**
			  * @brief 获取当前飞行了多长时间
			  */
			  glbDouble getAnimationTime() const { return _latestTime-_firstTime; }
			  /**
			  * @brief 获取维护的矩阵
			  */
			  osg::Matrixd getMatrix() const { return _matrix; }
		private:
			KeyPoint _firstPoint;
			KeyPoint _lastPoint;
			glbDouble _loopTime;

			glbDouble _firstTime;
			glbDouble _latestTime;
			glbDouble _pauseTime;
			glbBool _pause;
			osg::Matrixd _matrix;
			glbDouble _isfar;

			CGlbPlaneManipulator* _manipulator;
		};
	public:
		/**
		* @brief 构造函数									 
		*/
		CGlbPlaneManipulator(CGlbGlobe *globe);
		/**
		* @brief 析构函数
		*/
		~CGlbPlaneManipulator(void);

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
		* @brief 初始化
		*/
		virtual void home(const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter& aa);

		//IGlbGlobeManipulator method
	public:
		/**
		* @brief 获取操控器类型
		*/
		virtual GlbGlobeManipulatorTypeEnum GetType();
		/**
		* @brief 飞向
		* @param [in] 目标点状态：焦点位置xOrlon,yOrlat,zOrAlt
		*		  [in] 目标点状态：相机焦点距离distance,焦点处yaw,pitch
		*		  [in] 飞行时长 seconds
		* @return 成功返回true
		*		-	失败返回false
		*/
		virtual glbBool FlyTo(glbDouble xOrlon, glbDouble yOrlat, glbDouble zOrAlt, glbDouble distance, glbDouble yaw, glbDouble pitch, glbDouble seconds);

		/**
		* @brief 环绕飞行
		* @param [in] 环绕飞行中心点坐标：xOrlon,yOrlat,zOrAlt
		*		  [in] 飞行时长 seconds
		* @return 成功返回true
		*		-	失败返回false
		*/
		virtual glbBool FlyAround(glbDouble xOrlon, glbDouble yOrlat, glbDouble zOrAlt, glbDouble seconds, bool repeat);

		/**
		* @brief 判断是否正在飞行状态中
		* @return 是返回true
		*		-	否返回false
		*/
		virtual glbBool IsFlying();

		/**
		* @brief 停止飞行
		* @return 成功返回true
		*		-	失败返回false
		*/
		virtual glbBool StopFlying();

		/**
		* @brief 跳向
		* @param [in] 目标点状态：焦点位置xOrlon,yOrlat,zOrAlt
		*		  [in] 目标点状态：相机焦点距离distance,焦点处yaw,pitch
		* @return 成功返回true
		*		-	失败返回false
		*/
		virtual glbBool JumpTo(glbDouble xOrlon, glbDouble yOrlat, glbDouble zOrAlt, glbDouble distance, glbDouble yaw, glbDouble pitch);

		/**
		* @brief 设置在焦点处偏航角
		* @param [in] 旧焦点位置old_focus
		*		  [in] 旧相机焦点距离distance
		*		  [in] 焦点处偏航角yaw
		* @return 成功返回true
		*		-	失败返回false
		*/
		virtual glbBool SetYaw(glbDouble yaw,osg::Vec3d old_focus, glbDouble olddistance);

		/**
		* @brief 获取焦点处偏航角
		* @return 焦点处偏航角
		*/
		virtual glbDouble GetYaw();

		/**
		* @brief 设置在焦点处俯仰角
		* @param [in] 旧焦点位置old_focus
		*		  [in] 旧相机焦点距离distance
		*		  [in] 焦点处俯仰角pitch
		* @return 成功返回true
		*		-	失败返回false
		*/
		virtual glbBool SetPitch(glbDouble pitch,osg::Vec3d old_focus,glbDouble olddistance);

		/**
		* @brief 获取焦点处俯仰角
		* @return 焦点处俯仰角
		*/
		virtual glbDouble GetPitch();

		/**
		* @brief 获取相机位置
		* @param [out] 相机位置 cameraPos
		* @return 成功返回true
		*		-	失败返回false
		*/
		virtual glbBool GetCameraPos(osg::Vec3d& cameraPos);

		/**
		* @brief 获取焦点位置
		* @param [out] 焦点位置 cameraPos
		* @return 成功返回true
		*		-	失败返回false
		*/
		virtual glbBool GetFocusPos(osg::Vec3d& focusPos);

		/**
		* @brief 获取焦点相机距离
		* @return 焦点相机距离，单位米
		*/
		virtual glbDouble GetDistance();

		//////////////////////////////////////////////////////////////////////////
		// 以下4个接口和Drag接口,为外部WVS被动调用实现客户端 拖拽，旋转，放缩和还原相机位置操作所用
		/*
		* 鼠标按下
		*/
		void Push(glbInt32 ptx,glbInt32 pty);
		/*
		* 旋转
		*/
		void Rotate(glbInt32 ptx1,glbInt32 pty1,glbInt32 ptx2,glbInt32 pty2);
		/*
		* 缩放 
		*/
		void Zoom(glbBool isScrollUp);		
		/**
		* @brief 设置相机参数
		* @param yaw   绕向上方向旋转的角度
				 pitch 绕东方向旋转的角度
				 focusPos  焦点坐标
				 distance  焦点与相机之间的距离
		*/
		void SetCameraParam(glbDouble yaw, glbDouble pitch, osg::Vec3d focusPos, glbDouble distance);
		//////////////////////////////////////////////////////////////////////////

		/**
		* @brief 相机在地形上平移
		* @param [in] 2个垂直方向上的平移量：dangleX，dangleY
		* @return 成功返回true
		*		-	失败返回false
		*/
		virtual glbBool Move(glbDouble dangleX,glbDouble dangleY);

		//2014.11.24 aj添加，给导航条drag调用
		/**
		* @brief 相机平移
		* @param [in] x,y,z相机平移向量，世界坐标系下
		* @return 成功返回 true
		*			失败返回false
		*/
		virtual glbBool TranslateCamera(glbDouble x,glbDouble y,glbDouble z);

		/**
		* @brief 开启关闭碰撞检测
		* @param [in] true为开启碰撞检测
		*			   false为关闭碰撞检测
		*/
		virtual glbBool DetectCollision(glbBool mode);

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
		virtual glbBool SetUnderGroundMode(glbBool mode);

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
		virtual glbBool SetUndergroundDepth(glbDouble zOrAlt);

		/**
		* @brief 纠正相机海拔/高度
		* @return 成功返回true
		*		-	失败返回false
		*/
		virtual glbBool UpdateCameraElevate(glbDouble elevation);

		/**
		* @brief 焦点纠偏
		* @return 成功返回true
		*		-	失败返回false
		*/
		virtual glbBool UpdateFocusElevate(glbDouble elevation);

		/**
		* @brief 自由模式操控器下，设置相机锁定模式
		*/
		virtual glbBool SetLockMode(GlbGlobeLockModeEnum glbType);

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
		void SetShakeSope(glbDouble horizontalMeter, glbDouble verticalMeter);
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
	public:
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
		* @brief 旋转相机俯仰角
		*/
		void RotateCameraPitch(glbDouble pitch);

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
		* @brief 旋转相机偏航角
		*/
		void RotateCameraYaw(glbDouble yaw);

		/**
		* @brief 放缩
		*/
		void Zoom(glbDouble delt);
		/**
		* @brief 定点放缩
		*/
		void ZoomCursor(glbDouble delt, glbDouble x, glbDouble y);

		/**
		* @brief 绕选中点垂线旋转
		*/
		void Rotate(glbDouble lonOrX,glbDouble latOrY,glbDouble altOrZ,glbDouble pitch,glbDouble yaw);

		/**
		* @brief 鼠标拖拽，由pt1点拖拽到pt2点
		* @param [in] ptx1,pty1：起始点
		*		  [in] ptx2,pty2：目标点
		*/
		void Drag(glbInt32 ptx1,glbInt32 pty1,glbInt32 ptx2,glbInt32 pty2);

	private:
		/**
		* @brief 按键事件处理
		* @return 返回true ：view需重绘
		*		-	返回false ：不需重绘
		*/
		glbBool handleKeyDown(const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter& aa);

		/**
		* @brief 清空事件缓存
		*/
		void flushMouseEventStack();

		/**
		* @brief 根据相机位置与焦点位置计算其他参数mpr_distance,mpr_pitch;mpr_yaw不变
		* @param [in] cameraPos 相机位置经纬高
		[in] focusPos 焦点位置经纬高
		*/
		void UpdataMatrix(osg::Vec3d& cameraPos,osg::Vec3d& focusPos); 

		/**
		* @brief 通过相机海拔计算Move操作时的平移系数
		* param [in] Alt 海拔高度
		* @return 返回平移系数
		*/
		glbDouble getMoveCoefficient(glbDouble Alt);
		/**
		* @brief 相机纠偏前，修正相机边界
		*/
		void UpdateCameraElevateExt();
		/**
		* @brief 保持相机朝向不变,纠偏方式1,基于相机的纠偏 - 允许边界约束
		*/
		void UpdateCameraElevateExt1();
		/**
		* @brief 保持相机看向的焦点不变,纠偏方式2，基于焦点的纠偏
		*/
		void UpdateCameraElevateExt2();
		/**
		* @brief 线段与面求交
		* param [in] startPos 线段起始点
		[in] endPos 线段终止点
		[in] planePos 面上的一个点
		[in] nomalvec 面法线向量
		[in] IntersectPos 交点
		* @return 交点个数
		*/
		glbInt32 IntersectRayPlane(osg::Vec3d startPos,osg::Vec3d endPos,osg::Vec3d planePos,osg::Vec3d nomalvec,std::vector<osg::Vec3d>& IntersectPos);

		/**
		* @brief 飞向
		* @param [in] 目标点状态：焦点位置xOrlon,yOrlat,zOrAlt
		*		  [in] 目标点状态：相机焦点距离distance,焦点处yaw,pitch
		*		  [in] 飞行时长 seconds
		* @return 成功返回true
		*		-	失败返回false
		*/
		virtual glbBool FlyTo2(glbDouble xOrlon, glbDouble yOrlat, glbDouble zOrAlt, glbDouble distance, glbDouble yaw, glbDouble pitch, glbDouble seconds);
		/**
		* @brief 计算当前相机与物体相交为焦点时的位置姿态
		*/
		glbBool computeObjMatrix();

		/**
		* @brief 判断相机位置是否在平面允许范围内 ， 在的话返回true， 不在的话返回false
		*/
		glbBool IsCameraPosInRange(osg::Matrixd& _mat);
	private:
		osg::Vec3d	mpr_focusPos;		//焦点位置
		glbDouble	mpr_distance;		//相机到焦点距离
		glbDouble	mpr_yaw;			//相机偏航角(相对于相机)
		glbDouble	mpr_pitch;			//相机俯仰角(相对于相机)
		osg::Vec2d	mpr_speed;			//相机自动移动速度

		CGlbGlobe*	mpr_globe;			//场景指针
		glbDouble	mpr_minx;			//场景范围
		glbDouble	mpr_maxx;			//场景范围
		glbDouble	mpr_miny;			//场景范围
		glbDouble	mpr_maxy;			//场景范围
		glbDouble	mpr_minz;			//场景范围
		glbDouble	mpr_maxz;			//场景范围
		glbDouble	mpr_maxCameraAlt;	//相机最大海拔高度
		glbDouble	mpr_angel;
		glbDouble	mpr_camera_minx;	//相机范围
		glbDouble	mpr_camera_maxx;	//相机范围
		glbDouble	mpr_camera_miny;	//相机范围
		glbDouble	mpr_camera_maxy;	//相机范围
		osg::Vec3d	mpr_oldfocusPos;

		GlbGlobeLockModeEnum mpr_lockmode;					//相机锁定模式

		osg::ref_ptr<const osgGA::GUIEventAdapter> _ga_t1;	//当前的前一个鼠标事件(主要用来存鼠标按下的那个事件) 		
		osg::ref_ptr<const osgGA::GUIEventAdapter> _ga_t0;	//当前鼠标事件
		osg::Vec3d _pushPos;								//按下点世界坐标
		osg::Matrixd _pushMatrix;							//鼠标按下时刻，相机状态矩阵
		osg::Matrixd _pushVPW;
		osg::ref_ptr<CGlbPlaneFlyCallback> _FlyCallback;	//飞行回调

		osg::Vec3d mpr_dragDir;	//记录拖拽方向
		bool	   mpr_bFirstDrag;	//是否是第一次拖拽

		// 相机抖动相关变量
		glbBool			mpr_isShake;		// 控制相机是否抖动
		osg::Vec3d		mpr_shakefocusPos;	// 相机抖动 时的焦点位置
		glbDouble		mpr_shakeDistance;	// 相机抖动 时的距离		
		glbDouble		mpr_orignPitch;		// 相机抖动的原始pitch角
		glbDouble		mpr_orignyaw;		// 相机抖动时的原始yaw角

		glbDouble		mpr_shakeTime;
		glbDouble		mpr_shakeStartTime;
		glbDouble		mpr_shakeHorDegree;
		glbDouble		mpr_shakeVerDegree;

		// 虚拟参考面相关变量
		glbBool			mpr_bUseReferencePlane;	//是否启用虚拟参考面
		glbDouble		mpr_referencePlaneZ;  //虚拟参考面的z    虚拟参考面为（0,0,z）

	};

}