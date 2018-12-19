/********************************************************************
* Copyright (c) 2014 北京超维创想信息技术有限公司
* All rights reserved.
*
* @file    GlbGlobeManipulator.h
* @brief   球模式下自由操控器类头文件
*
* @version 1.0
* @author  敖建
* @date    2014-5-13 10:35
*********************************************************************/

#pragma once
#include "GlbGlobeManipulatorManager.h"
#include "GlbCommTypes.h"
#include "IGlbGlobeViewEventHandle.h"

#include <osgGA/GUIEventAdapter>
#include <osgGA/GUIActionAdapter>
#include <osg/AnimationPath>

#define ElevateThreshold 2.0	//相机在地上时，位于地形之上的阈值，单位m
#define MinElevateThreshold 1.0//0.2
#define ZERO_E7 0.0000001
#define MaxCameraPosZ 100000000

//////////////////////////////////////////////////////////////////////////
//坐标系定义：
//
//1.相机坐标系(lookat坐标系)：以相机为原点，z轴正方向指向相机焦点反方向，y轴正方向指向相机up方向，x轴与Oz,Oy轴垂直，右手系
//
//2.地球惯性坐标系(地球球心坐标系)：以地心为原点，z轴正方向指向北极点，x轴正方向指向经纬度为(0,0)点(格林尼治)，y轴与Oz,Ox轴垂直，指向经纬度为(90E,0)点，,右手系
//
//3.地理坐标系：以当地点为原点，z轴正方向垂直向上，y轴指向正北方向，x轴指向正东方向
//////////////////////////////////////////////////////////////////////////

namespace GlbGlobe
{
	class CGlbGlobe;
	class CGlbGlobeView;
	class GlbGlobeManipulatorManager;
	class IGlbGlobeManipulator;
	class IGlbGlobeViewEventHandle;
	/**
	* @brief 球形模式下自由操控器类				 
	* @version 1.0
	* @author  敖建
	* @date    2014-5-13 10:40
	*/
	class GLB_DLLCLASS_EXPORT CGlbGlobeManipulator : 
		public IGlbGlobeManipulator
		,public osgGA::CameraManipulator
	{
		/**
		* @brief KeyPoint 关键点,FlyCallback中记录关键点的结构体	 
		* @version 1.0
		* @author  敖建
		* @date    2014-6-14 11:22
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
		* @brief 自由操控器飞行回调，私有类	 
		* @version 1.0
		* @author  敖建
		* @date    2014-6-14 11:22
		*/
		class CGlbGlobeFlyCallback : public osg::NodeCallback
		{ 
		public:
			/**
			* @brief 构造函数
			*/
			CGlbGlobeFlyCallback();
			/**
			* @brief 构造函数
			*/
			CGlbGlobeFlyCallback(/*CGlbGlobeManipulator* manipulator,*/KeyPoint pt0,KeyPoint pt1,glbDouble loopTime,glbBool isUnderMode);
			/**
			* @brief 构造函数
			*/
			CGlbGlobeFlyCallback(KeyPoint pt0,KeyPoint pt1,glbDouble distan,glbDouble loopTime,glbBool isUnderMode);
			/**
			* @brief 构造函数
			*/
			CGlbGlobeFlyCallback(CGlbGlobeManipulator* manipulator,glbDouble loopTime,osg::Vec3d centerPos,glbDouble distan);
			/**
			* @brief 回调函数
			*/
			virtual void operator()(osg::Node* node, osg::NodeVisitor* nv);
			/**
			* @brief 获取当前飞行了多长时间
			*/
			glbDouble getAnimationTime() const;
			/**
			* @brief 设置是否暂停
			*/
			void setPause(glbBool pause);
			/**
			* @brief 获取是否暂停
			*/
			glbBool getPause() const { return _pause; }
			/**
			* @brief 获取维护的矩阵
			*/
			osg::Matrixd getMatrix() const;

			inline glbBool IslastFrame(){ return _islastFrame;}
			inline void resetlastFrame(){ _islastFrame = false;}
			inline osg::Vec3d getfocus(){ return _focus_temp;}
		protected:
			/**
			* @brief 由相机焦点获取相机位置姿态矩阵
			* [in] cameraPos,focusPos：相机、焦点地理坐标系
			* 返回位置姿态矩阵
			*/
			void UpdateMatrix(osg::Vec3d cameraPos,osg::Vec3d focusPos);
			/**
			* @brief 球面点插值
			*/
			osg::Vec3d interpolate(glbDouble ratio,osg::Vec3d& pt0,osg::Vec3d& pt1);
			/**
			* @brief quat插值
			*/
			osg::Quat interpolate(glbDouble ratio,osg::Quat& q0,osg::Quat& q1);
			/**
			* @brief 析构函数
			*/
			~CGlbGlobeFlyCallback(){}
		private:			
			glbDouble _firstTime;
			glbDouble _latestTime;
			glbDouble _pauseTime;
			glbBool _pause;
			glbDouble _timeMultiplier;
			glbDouble _timeOffset;
			osg::Matrixd _matrix;

			KeyPoint _firstPt;
			KeyPoint _lastPt;
			osg::Vec3d _firstcameraPos;
			osg::Vec3d _lastcameraPos;
			osg::Vec3d _firstLookat;
			osg::Vec3d _lastLookat;
			osg::Quat _firstQuat;
			osg::Quat _lastQuat;
			glbDouble _First_Last_distance;
			glbDouble _distance;

			CGlbGlobeManipulator* _manipulator;
			glbDouble _loopTime;
			glbDouble _isfar;
			glbBool _islastFrame;
			osg::Vec3d _focus_temp;
			unsigned char mpr_flag;
			glbBool _isUnder;
		};
	public:
		/**
		* @brief 构造函数
		*/
		CGlbGlobeManipulator(CGlbGlobe *globe);

		/**
		* @brief 析构函数
		*/
		~CGlbGlobeManipulator(void);

		/**
		* @brief 初始化
		*/
		virtual void home(const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter& aa);

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
		* @brief 绕选中点垂线旋转
		*/
		void Rotate(glbDouble lonOrX,glbDouble latOrY,glbDouble altOrZ,glbDouble pitch,glbDouble yaw);

		/**
		* @brief 鼠标拖拽，由pt1点拖拽到pt2点
		* @param [in] ptx1,pty1：起始点
		*		  [in] ptx2,pty2：目标点
		*/
		void Drag(glbInt32 ptx1,glbInt32 pty1,glbInt32 ptx2,glbInt32 pty2);


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
		virtual glbBool FlyAround(glbDouble xOrlon, glbDouble yOrlat, glbDouble zOrAlt, glbDouble seconds, glbBool repeat);

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
		// 以下4个接口和Drag接口为外部WVS被动调用实现客户端 拖拽，旋转，放缩和还原相机姿态位置操作所用
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
		* @brief 相机在地形上平移.
		相机在当前海拔高度上不变，绕地球球心转动，转动轴为过地球球心，方向为相机朝向焦点方向与相机向上方向叉乘所形成的向量方向的直线。转动角度为angel = L(线位移)/R(相机球心距离)。
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
		* @brief 纠正相机海拔/高度，相机姿态不变，相机抬高
		* @return 成功返回true
		*		-	失败返回false
		*/
		virtual glbBool UpdateCameraElevate(glbDouble elevation);

		/**
		* @brief 焦点纠偏
		* @return 焦点无变化或虚焦点返回false
		*		-	焦点有变化返回true
		*/
		virtual glbBool UpdateFocusElevate(glbDouble elevation);

		/**
		* @brief 事件回调
		*/
		virtual glbBool handle(const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter& us);

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
		
	protected:
		/**
		* @brief 计算相机位置
		* @param [in] focusPos 焦点位置经纬高
		[in] distance 相机焦点距离
		[in] yaw 焦点处偏航角
		[in] pitch 焦点处俯仰角
		* @return 相机位置经纬高
		*/
		osg::Vec3d computeCameraPosition(osg::Vec3d& focusPos,glbDouble distance,glbDouble yaw,glbDouble pitch);

		/**
		* @brief 计算焦点位置
		* @param [in] cameraPos 相机位置经纬高
		[in] distance 相机焦点距离
		[in] yaw 焦点处偏航角
		[in] pitch 焦点处俯仰角
		* @return 焦点位置经纬高
		*/
		//osg::Vec3d computeFocusPosition(osg::Vec3d& cameraPos,glbDouble distance,glbDouble yaw,glbDouble pitch);

		/**
		* @brief 根据相机位置与焦点位置计算其他参数mpr_distance,mpr_yaw,mpr_pitch
		* @param [in] cameraPos 相机位置经纬高
		[in] focusPos 焦点位置经纬高
		*/
		void UpdataMatrix(osg::Vec3d& camPos,osg::Vec3d& focPos);	

		/**
		* @brief 清空事件缓存
		*/
		void flushMouseEventStack();

		/**
		* @brief 保存当前行为事件和前一个行为事件
		* @param [in] ea 行为事件
		*/
		void addMouseEvent(const osgGA::GUIEventAdapter& ea);

		/**
		* @brief 鼠标是否移动
		* @return 是返回true
		*		-	否返回false
		*/
		glbBool isMouseMoving();

		/**
		* @brief 按键事件处理
		* @return 返回true ：view需重绘
		*		-	返回false ：不需重绘
		*/
		glbBool handleKeyDown( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us );

		/**
		* @brief 触控事件处理
		* @return 返回true ：view需重绘
		*		-	返回false ：不需重绘
		*/
		glbBool handleTouch( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us );
		/**
		* @brief 判断2个3维向量是否相等
		* @return 是返回true
		*		-	否返回false 
		*/
		static glbBool IsSamePos(osg::Vec3d vec1,osg::Vec3d vec2);

		
	private:
		/**
		* @brief 飞向,私有调用，鼠标双击飞行
		* @param [in] 目标点状态：焦点位置xOrlon,yOrlat,zOrAlt
		*		  [in] 目标点状态：相机焦点距离distance,焦点处yaw,pitch
		*		  [in] 飞行时长 seconds
		* @return 成功返回true
		*		-	失败返回false
		*/
		virtual glbBool FlyTo2(glbDouble xOrlon, glbDouble yOrlat, glbDouble zOrAlt, glbDouble distance, glbDouble yaw, glbDouble pitch, glbDouble seconds);
		/**
		* @brief 线段与球求交
		* param [in] startPos 线段起始点
		[in] endPos 线段终止点
		[in] centerPos 球中心点
		[in] radius 球半径
		[in] IntersectPos 交点
		* @return 交点个数
		*/
		//glbInt32 IntersectRaySphere(osg::Vec3d startPos,osg::Vec3d endPos,osg::Vec3d centerPos,glbDouble radius,std::vector<osg::Vec3d>& IntersectPos);
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
		* @brief 通过相机海拔计算Move操作时的转动系数
		* param [in] Alt 海拔高度
		* @return 返回转动系数
		*/
		glbDouble getMoveCoefficient(glbDouble Alt);

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

		/**
		* @brief 经纬高，角度制转弧度制
		*/
		inline void DegToRad(osg::Vec3d& vec)
		{
			vec.x() = osg::DegreesToRadians(vec.x());
			vec.y() = osg::DegreesToRadians(vec.y());
		}

		/**
		* @brief 经纬高，弧度制转角度制
		*/
		inline void RadToDeg(osg::Vec3d& vec)
		{
			vec.x() = osg::RadiansToDegrees(vec.x());
			vec.y() = osg::RadiansToDegrees(vec.y());
		}
		/**
		* @brief 相机纠偏，焦点不变，相机抬高，pitch变大
		*/
		glbBool UpdateCameraElevate2(glbDouble cameraElevate);

		/**
		* @brief 屏幕坐标转化为场景内经纬高
		*/
		glbBool ScenePtToGePt(glbDouble x,glbDouble y,osg::Vec3d& resultPos); 

		/**
		* @brief 计算当前相机与物体相交为焦点时的位置姿态
		*/
		glbBool computeObjMatrix();
		///**
		// * @brief 是否按下control键
		// * return 是返回true，否返回false
		//**/
		//glbBool IsKeyCtrlDown();
		///**
		// * @brief 设置是否按下control键
		//**/
		//void SetKeyCtrlDown(glbBool isdown);
		void setByMatrix2(const osg::Matrixd& matrix);
		
	private:
		osg::Vec3d mpr_focusPos;	//焦点位置(经纬高)
		glbDouble mpr_distance;		//相机到正向焦点距离
		glbDouble mpr_yaw;			//相机偏航角(相对于焦点),定义指北为0，北偏东为0~90，即顺时针旋转.
		glbDouble mpr_pitch;		//相机俯仰角(相对于焦点),定义竖直向下看为-90，向上为90.
		glbDouble mpr_roll;			//相机滚转角(相对于焦点)正常情况下，保持地面不倾斜，mpr_roll一直都为0;

		glbBool	mpr_isDetectCollision;		//是否开启碰撞检测
		CGlbGlobe* mpr_globe;				//场景对象指针
		osg::ref_ptr<const osgGA::GUIEventAdapter> _ga_t1;	//当前的前一个鼠标事件(主要用来存鼠标按下的那个事件) 		
		osg::ref_ptr<const osgGA::GUIEventAdapter> _ga_t0;	//当前鼠标事件
		osg::ref_ptr<const osgGA::GUIEventAdapter> _ga_touch;	
		osg::ref_ptr<const osgGA::GUIEventAdapter> _ga_release;	
		glbBool _isTouchDelay;
		osg::Vec3d _pushPos;								//鼠标按下点经纬高 || 触控第1个点按下的经纬高
		osg::Vec3d _pushPos2;								//触控第2个点按下的经纬高
		osg::Matrixd _pushMatrix;							//鼠标按下时刻，相机状态矩阵
		osg::Matrixd _pushVPW;
		glbDouble _pushpitch;
		glbDouble _pushyaw;
		osg::ref_ptr<CGlbGlobeFlyCallback> _FlyCallback;	//飞行回调
		glbBool isVirtualFocus;								//当前是否为虚焦点
		osg::Vec3d mpr_virtualFocusPos;						//维护一个虚焦点，flyTo时应用
		GlbGlobeLockModeEnum mpr_lockmode;					//相机锁定模式
		osg::Vec3d mpr_focus_temp;							//飞行目标点
		glbBool mpr_isUnderGround;							//相机是否在地下
		glbBool mpr_isctrldown;				//Ctrl键是否按下
		glbBool mpr_isshiftdown;				//Alt键是否按下

		glbBool	mpr_isPushPicked;			//鼠标按下是否选中了地面或对象

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