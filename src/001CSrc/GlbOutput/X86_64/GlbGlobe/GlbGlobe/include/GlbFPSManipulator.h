/********************************************************************
* Copyright (c) 2014 北京超维创想信息技术有限公司
* All rights reserved.
*
* @file    GlbFPSManipulator.h
* @brief   第一人称漫游 类头文件
*
* @version 1.0
* @author  xt
* @date    2016-5-25 10:35
*********************************************************************/

#pragma once
#include "GlbGlobeManipulatorManager.h"
#include "GlbCommTypes.h"
#include "IGlbGlobeViewEventHandle.h"

#include <osgGA/GUIEventAdapter>
#include <osgGA/GUIActionAdapter>
#include <osgUtil/IntersectionVisitor>

#include <functional>
//////////////////////////////////////////////////////////////////////////
#ifndef __GLB_FPS_MANIPULATOR_H__
#define __GLB_FPS_MANIPULATOR_H__

#define _PITCHYAW_
//#define  FPSTEST
namespace GlbGlobe
{
	class CGlbGlobe;
	class CGlbGlobeView;
	class GlbGlobeManipulatorManager;
	class IGlbGlobeManipulator;
	class IGlbGlobeViewEventHandle;

	class GLB_DLLCLASS_EXPORT CGlbFPSManipulator :
		public IGlbGlobeManipulator
		,public osgGA::CameraManipulator
{
	public:
	/**
	* @brief 构造函数
	*/
		 CGlbFPSManipulator(CGlbGlobe* globe);
		 ~CGlbFPSManipulator();

	/************************************************************************/
	/* interface from osgGA::CameraManipulator     
	*  部分接口标示目前没有实现
	*/
	/************************************************************************/
	/**
	* @brief 获取相机矩阵
	*/
	virtual osg::Matrixd						getMatrix() const;
	/**
	* @brief 获取相机矩阵逆矩阵
	*/
	virtual osg::Matrixd						getInverseMatrix() const;
	/**
	* @brief 设置相机矩阵
	*/
	virtual void								setByMatrix(const osg::Matrixd& matrix);
	/**
	* @brief 获取相机逆矩阵
	*/
	virtual void								setByInverseMatrix(const osg::Matrixd& matrix);
	/************************************************************************/
	/* interface from IGlbGlobeManipulator                                                                     */
	/************************************************************************/
	/**
	* @brief 获取操控器类型
	*/
	virtual GlbGlobeManipulatorTypeEnum			GetType();
	/**
		* @brief 飞向
		* @param [in] 目标点状态：焦点位置xOrlon,yOrlat,zOrAlt
		*		  [in] 目标点状态：相机焦点距离distance,焦点处yaw,pitch
		*		  [in] 飞行时长 seconds(目前是帧数)
		* @return 成功返回true
		*		-	失败返回false
	*/
	virtual glbBool								FlyTo(glbDouble xOrlon, glbDouble yOrlat, glbDouble zOrAlt, glbDouble distance,
													glbDouble yaw, glbDouble pitch, glbDouble seconds);
	/**
		* @brief 环绕飞行(该接口在此操控器没有实现)
		* @param [in] 环绕飞行中心点坐标：xOrlon,yOrlat,zOrAlt
		*		  [in] 飞行时长 seconds
		* @return 成功返回true
		*		-	失败返回false
	*/
	virtual glbBool								FlyAround(glbDouble xOrlon, glbDouble yOrlat, glbDouble zOrAlt,
												glbDouble seconds, glbBool repeat);
	/**
		* @brief 判断是否正在飞行状态中(动画)
		* @return 是返回true
		*		-	否返回false
	*/
	virtual glbBool								IsFlying() ;

	/**
		* @brief 停止飞行
		* @return 成功返回true
		*		-	失败返回false
	*/
	virtual glbBool								StopFlying();
	/**
		* @brief 跳向 (该接口在此操控器没有实现)
		* @param [in] 目标点状态：焦点位置xOrlon,yOrlat,zOrAlt
		*		  [in] 目标点状态：相机焦点距离distance,焦点处yaw,pitch
		* @return 成功返回true
		*		-	失败返回false
	*/											
	virtual glbBool								JumpTo(glbDouble xOrlon, glbDouble yOrlat, glbDouble zOrAlt, glbDouble distance, glbDouble yaw, glbDouble pitch);

	/**
		* @brief 获取相机位置
		* @param [out] 相机位置 cameraPos
		* @return 成功返回true degree
		*		-	失败返回false
	*/
	virtual glbBool								GetCameraPos(osg::Vec3d& cameraPos);
	/**
		* @brief 设置相机偏航角
	*/
	virtual void								SetCameraYaw(glbDouble yaw){};
	/**
		* @brief 获取相机偏航角
		* @return 相机偏航角
	*/
	virtual glbDouble							GetCameraYaw();

	virtual void								SetCameraPitch(glbDouble pitch){};
	/**
		* @brief 获取相机俯仰角
		* @return 相机俯仰角
	*/
	virtual glbDouble							GetCameraPitch();
	/**
		* @brief 获取焦点位置(第一人称漫游返回的是lookat的位置)
		* @param [out] 焦点位置 cameraPos
		* @return 成功返回true
		*		-	失败返回false
	*/
	virtual glbBool								GetFocusPos(osg::Vec3d& focusPos);
	/**
		* @brief 获取焦点相机距离
		* @return 焦点相机距离
	*/
	virtual glbDouble							GetDistance();
	/**
		* @brief 相机在地形上平移.(该接口在此操控器没有实现)
		相机在当前海拔高度上不变，绕地球球心转动，转动轴为过地球球心，方向为相机朝向焦点方向与相机向上方向叉乘所形成的向量方向的直线。转动角度为angel = L(线位移)/R(相机球心距离)。
		* @param [in] 2个垂直方向上的平移量：dangleX，dangleY
		* @return 成功返回true
		*		-	失败返回false
	*/
	virtual glbBool								Move(glbDouble dangleX,glbDouble dangleY){return false;};
	/**
		* @brief 相机平移
		* @param [in] x,y,z相机平移向量，世界坐标系下
		* @return 成功返回 true
		*			失败返回false
	*/
	virtual glbBool								TranslateCamera(glbDouble x,glbDouble y,glbDouble z){return false;}
	/**
		* @brief 开启关闭地下模式(目前第一人称只在地上)
		* @param [in] true为开启地下模式
		*			   false为关闭地下模式
	*/
	virtual glbBool								SetUnderGroundMode(glbBool mode);
	/**
		* @brief 设置地下参考面深度
		* @param [in] zOrAlt地下参考面深度
		* @return 成功返回true
		*		-	失败返回false
	*/
	virtual glbBool								SetUndergroundDepth(glbDouble zOrAlt);
	/**
		* @brief 纠正相机海拔/高度，相机姿态不变，相机抬高
		* @return 成功返回true
		*		-	失败返回false
	*/
	virtual glbBool								UpdateCameraElevate(glbDouble elevation);
	/**
		* @brief 焦点纠偏 (该接口在此操控器没有实现)
		* @return 焦点无变化或虚焦点返回false
		*		-	焦点有变化返回true
	*/
	virtual glbBool								UpdateFocusElevate(glbDouble elevation){return false;};
	
	/**
		* @brief 获取是否启用了相机抖动(该接口在此操控器没有实现)
		* @return	启动相机抖动 返回true
					没有相机抖动 返回false
	*/
	virtual void								Shake(glbBool isShake){};
	/*
		* @brief 设置 相机抖动幅度(该接口在此操控器没有实现)
		* @param [in] horizontalDegree 水平方向抖动幅度 球面模式 单位是【度】 默认为2度  平面模式单位是【米】 
	    		 [in] verticalDegree 水平方向抖动幅度 球面模式 单位是【度】 默认为2度  平面模式单位是【米】 
	*/
	virtual void								SetShakeSope(glbDouble horizontalDegree, glbDouble verticalDegree){} ;
	/*
		* @brief 设置 相机抖动 时间(该接口在此操控器没有实现)
 		* @param [in] seconds 相机抖动时间 单位： 秒 
		*/
	virtual void								SetShakeTime(glbDouble seconds){};
	/*
		* @brief 启用/禁止 虚拟参考面(该接口在此操控器没有实现)
 		* @param [in] bEnable    启用 ： true， 禁止 ： false 
	*/
	virtual void								EnalbeVirtualReferencePlane(glbBool bEnable);
	/*
		* @brief 设置 虚拟参考面高度(该接口在此操控器没有实现)
 		* @param [in] zOrAltitude 虚拟参考面高度  
	*/
	virtual void								SetVirtualReferencePlane(glbDouble zOrAltitude);

	virtual glbDouble							GetVirtualReferencePlane();
	/**
		* @brief 设置在焦点处偏航角(该接口在此操控器没有实现)
		* @param [in] 旧焦点位置old_focus
		*		  [in] 旧相机焦点距离distance
		*		  [in] 焦点处偏航角yaw
		* @return 成功返回true
		*		-	失败返回false
	*/
	virtual glbBool								SetYaw(glbDouble yaw,osg::Vec3d old_focus, glbDouble olddistance){return false;};
	/**
		* @brief 获取焦点处偏航角
		* @return 焦点处偏航角
		*/
	virtual glbDouble							GetYaw();
	/**
		* @brief 设置在焦点处俯仰角
		* @param [in] 旧焦点位置old_focus
		*		  [in] 旧相机焦点距离distance
		*		  [in] 焦点处俯仰角pitch
		* @return 成功返回true
		*		-	失败返回false
		*/
	virtual glbBool								SetPitch(glbDouble pitch,osg::Vec3d old_focus,glbDouble olddistance){return false;};
	/**
		* @brief 获取焦点处俯仰角
		* @return 焦点处俯仰角
		*/
	virtual glbDouble							GetPitch();
	/**
		* @brief 事件回调
		*/
	virtual glbBool								handle(const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter& us);

	struct CameraLLHPPY 
	{
		CameraLLHPPY()
		{
			_X = _Y = _Z = 0.0;
			_pitch = _yaw = 0.0; //radin
		}

		void									setCameraPos(const osg::Vec3d&pos)	{_X = pos.x();_Y = pos.y();_Z = pos.z();}
		void									setCameraPY(const osg::Vec3d&py)	{_pitch = py.x();_yaw = py.z();}
		void									setPY(const osg::Vec2d&py)			{_pitch = py.x();_yaw = py.y();}
		osg::Vec3d								getCameraPos()const					{return osg::Vec3d(_X,_Y,_Z);}
		osg::Vec2d								getPY()const						{return osg::Vec2d(_pitch,_yaw);}
		const GLdouble&							getP()const							{return _pitch;}
		const GLdouble&							getY()const							{return _yaw;}
	private:
		glbDouble								_X;
		glbDouble								_Y;
		glbDouble								_Z;

		glbDouble								_pitch; //radin
		glbDouble								_yaw;   
	};

	typedef std::function<void (void)> ManipulatorOutCallBack ;
	typedef	std::function<void (double) > UpdateCameraAscendHighCallBack;
	// fly callback
	class CFPSManipulatorCallback :				public osg::NodeCallback
	{
	public:

		enum MoveType
		{
			XYZ = 0,
			LATITUDELONGITUDE,
			GLOBEMANIPULOAOR
		} ;

		CFPSManipulatorCallback(const CameraLLHPPY&b,const CameraLLHPPY&a,CGlbGlobe*glb,glbBool col = true);
		~CFPSManipulatorCallback(){}
		//初始化操作
		void									init();
		//设置动画帧数
		void									setAnimationTime(glbDouble times){looptime = times;}
		/**
		* @brief 停止动画
		* @removeUpdate 是否删除动画回调
		*/
		void									stopAnimation(glbBool removeUpdate = false,glbBool resetFunCallback = true);
		/**
		* @brief 设置动画帧数
		* @return 
		*/
		glbBool									checkAnimationStart()const;

		virtual void							operator()(osg::Node* node, osg::NodeVisitor* nv);

		const CameraLLHPPY&						getCurrerntCamera()const;
		const CameraLLHPPY&						getAfterCameraF()const;

		void									setStartCamera(const CameraLLHPPY&c) {currentCamera = c;afterCamera = currentCamera;}
		void									setTargetCamera(const CameraLLHPPY&c){targetCamera = c;}

		void									setMoveType(MoveType type) {moveType = type;}
		MoveType								getMoveType()const {return moveType;}
		glbBool									getCollision()const {return collision;}
		void									setRefLocation(glbBool r) {refLocation = r;}
		glbBool									getRefLocation()const {return refLocation;}
		void									setCollision(glbBool c) {collision = c;}
		void									setPFCallBack(ManipulatorOutCallBack pf) {pfunCallBack = pf;}
		void									setCameraAscendCallBack(UpdateCameraAscendHighCallBack pf) {ascendCallBack = pf;}
		void									setManipulator(CGlbFPSManipulator * p) {manipulator = p;}
	private:
		void									updateCamera(osg::Node*node,osg::NodeVisitor*nv);
		/**
		* @brief 开始动画前的一些操作
		* @return 
		*/
		glbBool									beforeLoop(osg::Node * n,GLdouble&,GLdouble&);
		/**
		* @brief std::erf //radin
		* @return 
		*/
		GLdouble								erf(GLdouble x);
		CameraLLHPPY		currentCamera;
		CameraLLHPPY		afterCamera;
		CameraLLHPPY		targetCamera;
		
		glbBool				willStop;		//is animationing ing
		glbBool				haveStart;
		glbBool				collision;
		glbBool				firstLoop;
		glbBool				refLocation;

		glbDouble			looptime;
		int					frameStamp;
		glbDouble			stepLen;
		osg::Vec2d			pitchYawDelta;

		osg::Node*			nodeForCallback;
		CGlbGlobe*			globe;

		MoveType			moveType;

		ManipulatorOutCallBack	pfunCallBack;
		UpdateCameraAscendHighCallBack ascendCallBack;
		CGlbFPSManipulator *			manipulator;
	};

	
	/**
		* @brief 切换到第一人称一些准备工作
	*/
	void										beforePFSManipulator();
	/**
		* @brief 退出第一人称后续操作
	*/
	void										outPFSManipulator();

	/************************************************************************/
	/* set                                                                     */
	/************************************************************************/
	/**
		* @brief 鼠标操作移动距离
		* @param [in] 鼠标操作移动速度 m/s
	*/
	void										SetDriveMoveSpeed(const glbDouble&speed);

	/**
		* @brief 返回移动速度
		* @return 
	*/
	glbDouble									GetDriveMoveSpeed();

	/**
		* @brief 设置鼠标双击动画时间
		* @param [in] 
	*/
	void										setFlyTimes(const glbDouble&);
	/**
		* @brief 添加碰撞对象
		* @param [in] obj：碰撞对象
	*/
	glbBool										AddCollisionObject(CGlbGlobeRObject* obj);

	/**
		* @brief 添加图层下的碰撞对象
		* @param [in] id :图层id
		* @param [in] obj：碰撞对象
	*/
	glbBool										AddCollisionLaysObject(glbInt32 layId,CGlbGlobeRObject* obj);
	/**
		* @brief 返回已经添加碰撞检测的模型对象或者图层的id
		* @param [out] objIds :已添加的模型对象id
		* @param [out] layIds :已添加的图层对象id
		*/
	void										GetHaveAddObjsAndLays(std::vector<glbInt32>& objIds,std::vector<glbInt32>& layIds);
	/**
		* @brief 删除碰撞检测的对象或图层
		* @param [in] id :被删除模型对象id
		* @param [in] lyrOrObj:删除是对象还是图层,true-图层 ，false-对象
		* @return true：删除成功,false:删除失败
		*/
	glbBool										RemoveCollisionObjectOrLayerById(int id,glbBool isLyr = false);
	/**
		* @brief 删除所有碰撞对象
		* @param [in]
		* @return 删除成功返回true
	*/
	glbBool										RemoveAllCollsionObject();
	/**
		* @brief 依据对象ID获取碰撞对象
		* @param [in] idx：碰撞对象的index
		* @return 返回检索的碰撞对象,不存在返回null
	*/
	CGlbGlobeRObject*							getCollisionObjectByObjId(int idx);
	/**
		* @brief  (为了平滑处理)由自由漫游器转到第一人称漫游器
		* @return 
	*/
	
	/**
		* @brief依据对象ID获取碰撞对象数目
		* @return 返回碰撞对象数目
	*/
	glbInt32									GetCollisionObjectCount();

	glbBool										globeManipulator2FPSManipulator();
	/**
		* @brief  (为了平滑处理)由第一人称漫游器转到自由漫游器
		* @return 
	*/
	glbBool										fPSManipulator2GlobeManipulator(ManipulatorOutCallBack pf);

	void										setCameraXYZ(const osg::Vec3d XYZ);
	void										setCameraLLH(const osg::Vec3d LLH);//degree
	void										setCameraHigh(const glbDouble ,glbBool add = true);
	void 										setCameraPitchYaw(const osg::Vec3d py);
	void 										setCameraPitchYawAlex(const osg::Vec2 py);
	void										ascendCamera(GLdouble high);
	void										setFlyFocus(const osg::Vec3d focusPoint);
	GLdouble									getCameraAscendHigh()const;
	osg::Vec3d									getCameraPitChYaw()const;
	osg::Vec3d									getCameraVector()const;
	osg::Vec3d									getCameraRightVector()const;
	osg::Vec3d									getCameraUp()const;
	osg::Vec3d									getCameraXYZ()const;
	osg::Vec3d									getCameraLLHRadin()const; 
	osg::Vec3d									getCameraLLHDegree()const;
	osg::Vec3d									getWorldUp() const;
	/**
		* @brief 获取鼠标点击点的位置，以及镜头到交点的距离
		* @param [out] pushPoint:交点位置
		* @param [out] dis:镜头到交点距离
		* @return true:有交点,false:无交点
	*/
	glbBool										getPushPoint(osg::Vec3d&pushPoint,GLdouble&dis)const;
private:
	/**
		* @brief 获取一点位置的高程
		* @param lat :维度
		* @param lon :经度
	*/
	glbDouble									getTerrianElv(const glbDouble&lat,const glbDouble&lon,glbBool nowCamerPos = false)const;
	/**
		* @brief 线段交点
		* @param start :开始点
		* @param end :结束点
		* @param d: 交点距离
	*/
	glbBool										intersect(const osg::Vec3d start, osg::Vec3d& end,glbDouble&d)const;
	/**
		* @brief 朝向某个方向distance距离是否有交点
		* @param moveDir : 朝向
		* @param distance :距离
	*/
	glbBool										cullisionDir(const osg::Vec3d moveDir, glbDouble& distance);
	/************************************************************************/
	/* events                                                                 */
	/************************************************************************/
	void										flushMouseEventStack();
	void										addMouseEvent( const osgGA::GUIEventAdapter& ea );
	glbBool										performMovement(const osgGA::GUIEventAdapter& ea);
	glbBool										performMovementLeftAndMiddlePush( );
	glbBool										performMovementLeftMouseButton( const glbDouble eventTimeDelta, const glbDouble dx, const glbDouble dy );
	glbBool										handleMousePush( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us );
	glbBool										handleMouseDrag( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us );
	glbBool										handleMouseRelease( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us );
	glbBool										handleMouseDoubleClick( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us );
	glbBool										handleScroll( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us );
	glbBool										processKeyEvent(const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter& us);
	glbBool										handleFrame( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us );
	
	/*						private			*/
	private:
	glbBool										processKeyUp(const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter& us);
	glbBool										processKeyDown(const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter& us);

	void										initParams(void);
	/**
		* @brief 纠正相机位置:防止在模型或者地形以下,主要用于调整相机高度
		* @param [in] newPos:新的相机位置
		* @return 
	*/
	void										rectifyCameraPos(osg::Vec3d&newPos,glbBool needFootCollision = true);
	/**
		* @brief 镜头朝向dir方向移动len长度
		* @param [in] dir:交点位置
		* @param [in] len:镜头到交点距离
		* @param [in] collision:移动过程中是否进行碰撞检测
		* @param [ref] ref:是否需要依据地形或者三维场景调整镜头脚下高度
		* @
	*/
	glbBool										moveCameraPos(const osg::Vec3d dir,glbDouble len,glbBool collision = true,glbBool ref = true);
	osg::Vec3d									moveCameraBySweptSphere(osg::Vec3d cameraXYZ,osg::Vec3d cameraVDir);
	void										rotatePitchYaw(glbDouble yaw,glbDouble pitch,glbBool disallowFlipOver);
	void										fixVerticalAxis(osg::Quat& rotation, const osg::Vec3d& localUp, glbBool disallowFlipOver );
	/*
		绕着一点旋转
	*/
	void										rotateAboutPoint(const osg::Vec3d aroundPoint,glbDouble pitch,glbDouble yaw);
	glbBool										intersectorWithNode(osgUtil::Intersector * itersector);
	void										cameraCallBack(glbDouble cameraZ);
	//当相机从空中降落时候，降落点位置的确定
	glbBool										findDescendCameraPosition(const osg::Vec3d currentXYZ,const osg::Vec3d viewDir,osg::Vec3d&XYZ);
	//											get mouse push point
	void										descendCamerAnimation(const osg::Vec3d currentXYZ,const osg::Vec2d py,const osg::Vec3d viewDir);
	void										moveCameraByKey();
	//镜头移动到地下或者地上
	void										moveCamerUnOrUpGround();
	
	void home(const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter& aa);

	//											计算目标位置相机的位置姿态
	CameraLLHPPY								computeTargetPointCameraPPY(const osg::Vec3d&targe);
	/**
		* @brief 飞向,私有调用，鼠标双击飞行(参照glbGlobeManipulator)
		* @param [in] 目标点状态：焦点位置xOrlon,yOrlat,zOrAlt
		*		  [in] 目标点状态：相机焦点距离distance,焦点处yaw,pitch
		*		  [in] 飞行时长 seconds
		* @return 成功返回true
		*		-	失败返回false
		*/
		virtual glbBool flyTo2(glbDouble xOrlon, glbDouble yOrlat, glbDouble zOrAlt, glbDouble distance, glbDouble yaw, glbDouble pitch, glbDouble seconds);
		
	/**
		* @brief 设置相机移动动画
		* @param s:开始相机
		  @param t:结束相机
		  @param loopTime :帧数
		  @param type :依据XYZ坐标还是经纬度计算
		  @param collision :是否开始碰撞检测
		  @param refl :在动画工程中是否调节相机相对地形的位置
		  @param pf :动画完毕后的回调
		  @param cpf :动画完毕后的回调
		* @return 
	*/
	void										animationCamer(const CameraLLHPPY s,const CameraLLHPPY t,glbDouble loopTime,
													unsigned int type,glbBool collision = true,glbBool refL = true,
													ManipulatorOutCallBack pf = nullptr,glbBool cpf = true);
	//有镜头矩阵推出 pitch yaw(弧度)是相对于交点的角度
	void										computeYawPitchDistanceFocusePararmFromMatrix(const osg::Matrixd mt,glbDouble& yaw, glbDouble& pitch, glbDouble& distance, osg::Vec3d& focuse);
	
	enum FLYTYPE
	{
		FLYTO2,
		FLYTO,
		FLYAROUND
	};
	struct KeyPoint
	{
		osg::Vec3d focusPos;
		glbDouble distan;
		glbDouble yaw;
		glbDouble pitch;
	};
	void										flyToFromCGlbGlobeManipulator(osg::NodeVisitor* nv);
	void										flyToFromCGlbGlobeManipulatorinit(FLYTYPE flyType,const KeyPoint&pt0,const KeyPoint&pt1,GLdouble distan,GLdouble loopTime);
	

	private:
		CGlbGlobe*								mpr_globe;
		// 虚拟参考面相关变量
		glbBool									mpr_bUseReferencePlane;	//是否启用虚拟参考面
		glbDouble								mpr_referencePlaneZ;  //虚拟参考面的z    虚拟参考面为（0,0,z）
		osg::ref_ptr<CFPSManipulatorCallback>	mpr_mouseCallBack;

	private:									//以下的经纬度 还有偏航 俯仰角 全部用角度

		glbBool*								mpr_wasd;		//同时按下前进和旋转键使用
		osg::Vec3d								mpr_cameraPos;	//焦点位置(经纬高) 在此漫游器中相当于眼睛的位置，此处是代表相机的经纬高(角度)
#ifdef  _PITCHYAW_
		/*  
			在局部坐标系下，相对于X轴(朝向东)的pitch旋转弧度，相对于Z轴(朝向)yaw旋转弧度
			旋转正负依据opengl的正负，该旋转与GlobeManipulator旋转的转化见函数convertFPSRotateToGlobeManipulatorRotate
		*/
		osg::Vec3d								mpr_rotateV; 
		glbDouble								mpr_rotateStep;
#else
		osg::Quat								mpr_rotation;   //相机的旋转
#endif

	private:
		glbDouble								mpr_moveSpeed;
		glbDouble								mpr_personHigh;			//身高
		glbDouble								mpr_ascendHith;

		glbDouble								mpr_animationTimes;    //快速移动的时间
		osg::Vec3d								mpr_focus_temp;			//飞行目标点
		osg::Vec3d								mpr_mousePushPoint;  //当前鼠标push位置

		glbBool									mpr_camera_is_underGround; //镜头是否处于地下状态
		osg::ref_ptr< const osgGA::GUIEventAdapter > _ga_t1;
		osg::ref_ptr< const osgGA::GUIEventAdapter > _ga_t0;
		osg::Matrixd							mpr_homeMatrix;
	public: //从globe自由操控器flyto需要的变量
		

		 KeyPoint								_firstPt;
		 KeyPoint								_lastPt;

		 glbDouble								Globe_firstTime;
		 GLdouble								Globe_latestTime;
		 glbDouble								Globe_timeMultiplier;
		 glbDouble								Globe_loopTime;
		 glbDouble								Globe_First_Last_distance;
		 glbBool								Globe_pause;
		 glbBool								Globe_isfar;
		 glbBool								Globe_isUnder;
		 osg::Matrixd							Globe_matrix;
		 FLYTYPE								Globe_Fly_flag;
		 osg::Vec3d								Globe_focus_temp;
	};

	extern osg::Vec3d	quatToEuler(osg::Quat q);
	extern glbBool		lineSegmentSphereIntersect(const osg::Vec3d&o,const osg::Vec3d&e,const osg::Vec3d&c,GLdouble r);
	extern osg::Vec3d	interpolate(glbDouble ratio,osg::Vec3d& pt0,osg::Vec3d& pt1);
	extern void			getLocalToWorldFromLatLong(const osg::Vec3d&focus,osg::Matrixd&focusToWorld);
	extern void			getLocalToWorldFromXYZMatrix(const osg::Vec3d&focus,osg::Matrixd&focusToWorld);
	extern void			convertXYZToLatLongHeight(const GLdouble&X,const GLdouble&Y,const GLdouble&Z,osg::Vec3d&llh);
	extern void			convertLatLongHeightToXYZ(const GLdouble&lat,const GLdouble&lon,const GLdouble&heigh,osg::Vec3d&xyz);
	extern osg::Vec2d	computeIntermediatePointBetweenToPoint(const osg::Vec2d&startLLH, 
									const osg::Vec2d&targetLLH,glbDouble fraction);
	extern glbDouble	computeTowPointGreatCircleDistance(const osg::Vec2d&startLLH,const osg::Vec2d&targetLLH);
}	
#endif