/********************************************************************
* Copyright (c) 2014 北京超维创想信息技术有限公司
* All rights reserved.
*
* @file    GlbGlobeView.h
* @brief   球三维视图类，显示CGlbGlobe的数据
*
* @version 1.0
* @author  敖建
* @date    2014-5-13 13:45
*********************************************************************/

#pragma once
#include "CGlbGlobe.h"
#include "GlbGlobeExport.h"
#include "GlbGlobeManipulator.h"
#include "GlbPlaneManipulator.h"
#include "GlbGlobeViewStatusBar.h"
#include "GlbGlobeViewNavigator.h"
#include "GlbGlobeViewElevationRuler.h"
#include "GlbGlobeViewScale.h"
#include "GlbGlobeViewSlave.h"
#include "GlbGlobeViewLogo.h"
#include "GlbGlobeViewCentermark.h"
#include "GlbGlobeElementFactory.h"
#include "GlbGlobeClipHandler.h"
#include "GlbPolygon.h"

#include "GlbGlobeSceneManager.h"

#include <osgViewer/Viewer>
#include <OpenThreads/Thread>
#include <GlbWString.h>
#include <GlbPath.h>
#include <GlbString.h>
#include <osgViewer/GraphicsWindow>
#include <osgDB/WriteFile>
#include <osg/Fog>

namespace GlbGlobe
{
	/**
	* @brief 用于注册、查询CGlbGlobeView的实例,单例模式的类,在全局只有一个实例
	* @version 1.0
	* @author  敖建
	* @date    2014-5-21 16:46
	*/
	class CGlbGlobeViewRegistry
	{
	public:
		/**
		* @brief 检索视图注册类的唯一实例
		* @return 视图注册类的实例
		*/
		static CGlbGlobeViewRegistry * GetViewRegistry()
		{
			/*if(mpr_self==NULL)mpr_self = new CGlbGlobeViewRegistry();
			return mpr_self;*/
			return &mpr_self;//modified by longtingyou
		}

		/**
		* @brief 注册CGlbGlobeView的实例
		* @param [in] hwnd : CGlbGlobeView实例的窗口句柄
		*		  [in] pview ：CGlbGlobeView的实例
		*/
		void RegistryView(HWND hwnd,void* pview)
		{
			mpr_globeviews.insert(std::pair<HWND,void *>(hwnd,pview));
		}

		/**
		* @brief 查询CGlbGlobeView的实例
		* @param [in] hwnd CGlbGlobeView的实例的窗口局部
		* @return CGlbGlobeView的实例指针
		*/
		void * FindView(HWND hwnd)
		{
			std::map<HWND,void *>::iterator itr = mpr_globeviews.find(hwnd);
			if(itr!=mpr_globeviews.end())
				return itr->second;
			else
				return NULL;
		}

		/**
		* @brief 移除CGlbGlobeView的实例
		* @param [in] hwnd CGlbGlobeView的窗口句柄
		*/
		void RemoveView(HWND hwnd)
		{
			std::map<HWND,void *>::iterator itr = mpr_globeviews.find(hwnd);
			if(itr!=mpr_globeviews.end())
				mpr_globeviews.erase(itr);
		}

		/**
		* @brief CGlbGlobeView的析构函数                                                             
		*/
		~CGlbGlobeViewRegistry(void)
		{
			//if(mpr_self)mpr_self=NULL;//modified by longtingyou
		}
	private:
		/**
		* @brief CGlbGlobeView的构造函数                                                             
		*/
		CGlbGlobeViewRegistry(void)
		{
		}
	private:
		//static CGlbGlobeViewRegistry * mpr_self;	//静态成员，全局唯一的窗口注册类实例
		static CGlbGlobeViewRegistry mpr_self;	//静态成员，全局唯一的窗口注册类实例,modified by longtingyou
		std::map<HWND,void *> mpr_globeviews;		//CGlbGlobeView的注册映射表
	};

	class CGlbGlobe;
	class CGlbGlobeManipulatorManager;
	class IGlbGlobeViewEventHandle;
	class CGlbGlobeViewRegistry;
	class CGlbGlobeViewStatusBar;
	class CGlbGlobeViewNavigator;
	class CGlbGlobeViewElevationRuler;
	class CGlbGlobeViewScale;
	class CGlbGlobeViewLogo;
	class CGlbGlobeViewCentermark;
	class CGlbGlobeClipHandler;	
	class CGlbGlobeUGPlane;
	class CGlbGlobeDynamic;
	class GLB_DLLCLASS_EXPORT CGlbGlobeView : public CGlbReference
	{
	private:
		/**
		* @brief myosgviewer 视图类私有类	 
		* @version 1.0
		* @author  敖建
		* @date    2014-5-19 15:28
		*/
		class myosgviewer : public osgViewer::Viewer
		{
		public:
			virtual void frame(double simulationTime=USE_REFERENCE_TIME);
			IGlbGlobeViewEventHandle* mpr_eventhandle;
		};

		/**
		*@ brief CGlbGlobeMousePickHandle 鼠标点拾取坐标类
		*/
		class CGlbGlobeMousePickHandle : public osgGA::GUIEventHandler
		{
		public:
			CGlbGlobeMousePickHandle(CGlbGlobe* globe)
			{
				mpr_globe = globe;
				_ga_t0 = NULL;
			}
			~CGlbGlobeMousePickHandle(){}
			virtual glbBool handle(const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter& aa);
		private:
			CGlbGlobe* mpr_globe;
			osg::ref_ptr<const osgGA::GUIEventAdapter> _ga_t0;
		};

		/**
		* @brief CGlbGlobeUpdateThread 视图类的私有类 数据更新线程类
		* @version 1.0
		* @author  敖建
		* @date    2014-5-19 14:29
		*/
		class CGlbGlobeUpdateThread : public OpenThreads::Thread
		{
		public:
			CGlbGlobeUpdateThread(CGlbGlobe* globe)
			{
				mpr_globe    = globe;
				mpr_done     = false;
				mpr_pause    = false;
				mpr_ispaused = false;
			}
			~CGlbGlobeUpdateThread()
			{		
				mpr_globe = NULL;								
			}
			void run();
			void pause();
			void resume();
			int  cancel();
		private:
			glbref_ptr<CGlbGlobe> mpr_globe;
			glbBool mpr_done;
			glbBool mpr_pause;
			glbBool mpr_ispaused;
		};

		/**
		* @brief CGlbGlobeRenderThread 视图类的私有类 渲染更新线程类，负责fame()场景
		* @version 1.0
		* @author  敖建
		* @date    2014-5-19 14:29
		*/
		class CGlbGlobeRenderThread : public OpenThreads::Thread
		{
		public:
			CGlbGlobeRenderThread(myosgviewer* osgviewer)
			{
				mpr_myosgviewer = osgviewer;
				mpr_done     = false;
				mpr_pause    = false;
				mpr_ispaused = false;
			}
			~CGlbGlobeRenderThread()
			{
				if (mpr_myosgviewer)
				{
					mpr_myosgviewer = NULL;
				}				
			}

			void run();
			void pause();
			void resume();
			int  cancel();
		private:
			myosgviewer* mpr_myosgviewer;
			glbBool mpr_done;
			glbBool mpr_pause;
			glbBool mpr_ispaused;
		};

		/**
		* @brief CGlbGlobeComputeElevationThread 视图类的私有类 场景Globe定时计算线程类，负责定时计算eye和center处地形高...
		* @version 1.0
		* @author  敖建
		* @date    2014-5-19 14:29
		*/
		class CGlbGlobeComputeElevationThread : public OpenThreads::Thread
		{
		public:
			CGlbGlobeComputeElevationThread(CGlbGlobe* p_globe)
			{
				mpr_globe = p_globe;
				mpr_done = false;
				mpr_pause = false;
				mpr_ispaused=false;
			}
			~CGlbGlobeComputeElevationThread(void)
			{
				mpr_globe = NULL;
			}
			void run();		
			void pause();
			void resume();
			int  cancel();		
		private:
			bool       mpr_done;	
			glbref_ptr<CGlbGlobe> mpr_globe;
			glbBool mpr_pause;
			glbBool mpr_ispaused;
		};

		/**
		* @brief CGlbGlobeDispatcherThread 视图类的私有类 场景Globe调度控制线程类，负责场景中对象的调度控制...
		* @version 1.0
		* @author  马林
		* @date    2014-9-11 13:29
		*/
		class CGlbGlobeDispatcherThread : public OpenThreads::Thread
		{
		public:
			CGlbGlobeDispatcherThread(CGlbGlobe* p_globe)
			{
				mpr_globe = p_globe;
				mpr_done = false;
				mpr_pause = false;
				mpr_ispaused = false;
			}
			~CGlbGlobeDispatcherThread(void)
			{
				mpr_globe = NULL;
			}
			void run();		
			void pause();
			void resume();
			int  cancel();		
		private:
			bool       mpr_done;	
			glbref_ptr<CGlbGlobe> mpr_globe;
			glbBool mpr_pause;
			glbBool mpr_ispaused;
		};
		// 抓图回调
		struct CGlbGlobeCaptureDrawCallback : public osg::Camera::DrawCallback
		{
			CGlbGlobeCaptureDrawCallback(std::string fileName,HANDLE _hEvent)
			{
				mpr_strFileName = fileName;
				mpr_isDone = false;
				mpr_hEvent = _hEvent;
			}
			~CGlbGlobeCaptureDrawCallback(){}

			virtual void operator() (const osg::Camera& camera) const				
			{
				if (mpr_isDone == false)
				{
					unsigned int width,height;
					//得到窗口系统接口
					//osg::GraphicsContext::WindowingSystemInterface* wsi = osg::GraphicsContext::getWindowingSystemInterface();
					//得到屏幕分辨率
					//wsi->getScreenResolution(osg::GraphicsContext::ScreenIdentifier(0),width,height);
				
					width = camera.getViewport()->width();
					height = camera.getViewport()->height();
					osg::ref_ptr<osg::Image> image_c = new osg::Image();
					image_c->allocateImage(width,height,1,GL_RGB,GL_UNSIGNED_BYTE);
					// 读取像素信息抓图
					image_c->readPixels(0,0,width,height,GL_RGB,GL_UNSIGNED_BYTE);
					//image_c->readPixels(0,0,width,height,GL_DEPTH_COMPONENT,GL_FLOAT);
					// 保存文件
					osgDB::writeImageFile(*(image_c.get()),mpr_strFileName);

					// 设置信号量为发信号状态,让SaveImage()函数中的WaitForSingleObject继续运行下去
					SetEvent(mpr_hEvent);
					
					//取消回调
					mpr_isDone = true;
					osg::Camera* cc = const_cast<osg::Camera*>(&camera);
					cc->setPostDrawCallback(NULL);
				}		
			}
		private:
			std::string mpr_strFileName;
			mutable glbBool mpr_isDone;	
			HANDLE mpr_hEvent;
		};

	public:
		/**
		* @brief 构造函数
		*/
		CGlbGlobeView(void);
		/**
		* @brief 析构函数
		*/
		virtual ~CGlbGlobeView(void);
	public:	
		/**
		* @brief 创建窗口
		* @param [in] hWndParent：父窗口句柄
		* @return 窗口句柄
		*/
		HWND Create(HWND hWndParent);

		/**
		* @brief 注册窗口类
		*/
		void registerWindowClasses();

		/**
		* @brief 设置窗口位置
		* @param [in] left：窗口左上角坐标
		[in] top : 窗口左上角坐标
		[in] width : 窗口宽度
		[in] height : 窗口高度
		*/
		void SetWinPosition(glbInt32 left,glbInt32 top,glbInt32 width,glbInt32 height);

		/**
		* @brief 设置焦点为当前窗口
		*/
		void SetFocus(void);

		/**
		* @brief 设置光标
		*/
		HCURSOR SetCursor(HCURSOR hcursor);

		/**
		* @brief 获取窗口句柄
		*/
		HWND GetHWnd(){return mpr_hwnd;}

		/**
		* @brief 设置背景
		*/
		void SetBackColor(glbFloat r,glbFloat g,glbFloat b,glbFloat a);
		void GetBackColor(glbFloat &r,glbFloat &g,glbFloat &b,glbFloat &a);
		/**
		* @brief 设置地下背景 默认为天蓝色(0, 128, 255)
		*/
		void SetUnderGroundBackColor(glbFloat r,glbFloat g,glbFloat b,glbFloat a);
		void GetUnderGroundBackColor(glbFloat &r,glbFloat &g,glbFloat &b,glbFloat &a);
		/**
		* @brief 视图事件处理
		*/
		glbBool HandEvents(UINT uMsg, WPARAM wParam, LPARAM lParam);

		/**
		* @brief 设置视图事件回调
		* @return 旧的IGlbGlobeViewEventHandle指针
		*/
		IGlbGlobeViewEventHandle*  SetEventHandle(IGlbGlobeViewEventHandle *event_handle);
		/**
		* @brief 获取视图事件回调指针
		* @return IGlbGlobeViewEventHandle指针
		*/
		IGlbGlobeViewEventHandle*  GetEventHandle();

		/**
		* @brief 设置场景对象
		* @param [in] p_globe：CGlbGlobe对象指针
		* @return 成功返回true
		*		-	失败返回false
		*/
		glbBool SetGlobe(CGlbGlobe* p_globe);

		/**
		* @brief 获取场景对象
		* @return 场景对象
		*/
		CGlbGlobe*   GetGlobe(){return mpr_globe.get();}

		/**
		* @brief 启动数据更新，渲染线程
		*/
		void Run();

		/**
		* @brief 停止数据更新，渲染线程
		*/
		void Stop();

		/**
		* @brief 暂停（继续）数据更新，渲染线程
		* @param [in] bPause 暂停为true，继续为false
		*/
		void Pause(glbBool bPause=true);

		/**
		* @brief 暂停（继续）渲染线程
		* @param [in] bPause 暂停为true，继续为false
		*/
		void PauseRenderThread(glbBool bPause);
		/**
		* @brief 获取相机操控器类型
		* @return 返回GlbGlobeManipulatorTypeEnum相机操控器类型
		*/
		GlbGlobeManipulatorTypeEnum GetManipulatorType();

		/**
		* @brief 设置自由操控模式
		* @return 成功返回true
		*		-	失败返回false
		*/	
		glbBool SetDragMode();

		/**
		* @brief 设置自由操控下的锁定模式
		* @return 成功返回true
		*		-	失败返回false
		*/
		glbBool SetLockMode(GlbGlobeLockModeEnum glbType);

		/**
		* @brief 设置第一人称驾驶操控模式
		* @return 成功返回true
		*		-	失败返回false
		*/
		glbBool SetDriveMode();

		/**
		* @brief 设置路径操控模式
		* @return 成功返回true
		*		-	失败返回false
		*/
		glbBool SetPathMode(CGlbGlobeDynamic *obj);
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
		* @brief 检测是否开启碰撞检测
		* @return 成功返回true
		*		-	失败返回false
		*/
		glbBool	CheckCollision()const;

		/**
		* @brief 添加碰撞检测的模型对象
		* @param [in] obj :添加的模型对象
		* @return true：添加成功,false:添加失败
		*/
		glbBool AddCollisionObject(CGlbGlobeRObject *obj);

		/**
		//* @brief 添加碰撞检测的（要素）图层
		//* @param [in] layer :添加的要素图层
		//* @return true：添加成功,false:添加失败
		//*/
		glbBool AddCollisionLayer(CGlbGlobeLayer *layer);

		/**
		* @brief 添加图层下的碰撞对象--(要素图层回调中调用此函数，内部函数，不用封装)
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
		*  包括图层下的所有对象
		* @return 
		*/
		glbInt32 GetCollisionObjectCount();

		/**
		* @brief 删除所有碰撞检测的模型对象
		* @return true:删除成功,false:删除失败
		*/
		glbBool	RemoveAllCollsionObject();	

		/**
		* @brief 依据对象ID获取碰撞对象
		* @param [in] idx：碰撞对象的index
		* @return 返回检索的碰撞对象,不存在返回null
		*/
		CGlbGlobeRObject* getCollisionObjectByObjId(int idx);

		/**
		* @brief 地心空间直角坐标系 到 地心大地坐标系的变换矩阵
		* @param [in/out] xorlon 地理经度
		*		  [in/out] yorlat 地理纬度
		*		  [in/out] zoralt 海拔高度
		*/
		void WorldToWGS84(glbDouble* xorlon,glbDouble* yorlat,glbDouble* zoralt);

		/**
		* @brief 地心大地坐标系的变换矩阵 到 地心空间直角坐标系
		* @param [in/out] xorlon 地理经度
		*		  [in/out] yorlat 地理纬度
		*		  [in/out] zoralt 海拔高度
		*/
		void WGS84ToWorld(glbDouble* xorlon,glbDouble* yorlat,glbDouble* zoralt);

		/**
		* @brief 窗口坐标转为屏幕坐标
		* @param [in/out] sx 
		*		  [in/out] sy 
		*/
		void WindowToScreen(glbDouble &sx,glbDouble &sy);

		/**
		* @brief 屏幕坐标转为窗口坐标
		* @param [in/out] sx 
		*		  [in/out] sy 
		*/
		void ScreenToWindow(glbDouble &sx,glbDouble &sy);

		/**
		* @brief 窗口坐标转为场景屏幕坐标
		* @param [in/out] sx 
		*		  [in/out] sy 
		*/
		void WindowToScene(glbDouble &sx,glbDouble &sy);

		/**
		* @brief 场景屏幕坐标转为窗口坐标
		* @param [in/out] sx 
		*		  [in/out] sy 
		*/
		void SceneToWindow(glbDouble &sx,glbDouble &sy);

		/**
		* @brief 世界坐标转为屏幕坐标
		* @param [in/out] sx 
		*		  [in/out] sy
		*		  [in]	   sz
		*/
		void WorldToScreen(glbDouble &sx,glbDouble &sy,glbDouble &sz);

		/**
		* @brief 屏幕坐标转为世界坐标
		* @param [in/out] sx 
		*		  [in/out] sy
		*		  [out]	   sz
		*/
		void ScreenToWorld(glbDouble &sx,glbDouble &sy,glbDouble &sz);

		/**
		* @brief 点选
		* @param [in] mousex	鼠标点x坐标
		*		  [in] mousey	鼠标点y坐标
		*		  [out]	results 选中的对象集合【按照距离相机距离由近到远排序】
		*		  [out] InterPoints 选中对象的交点集合
		* @return 有对象选中返回 true
		*		   没有对象选中返回false
		*/
		glbBool Pick( glbInt32 mousex,glbInt32 mousey, std::vector<glbref_ptr<CGlbGlobeObject>>&results,std::vector<osg::Vec3d>&InterPoints);
		/**
		* @brief 操控器求鼠标当前点位置的对象及交点
		* @param [in] mousex	鼠标点x坐标
		*		  [in] mousey	鼠标点y坐标		
		*		  [out] InterPoint 选中的最近对象的交点
		* @return 有对象选中返回 true
		*		   没有对象选中返回false
		*/
		glbBool PickNearestObject( glbInt32 mousex,glbInt32 mousey, osg::Vec3d& InterPoint);
		/**
		* @brief 多边形选
		* @param [in] region	多边形区域
		*		  [in] minAltitude	最小高度 【默认为-1000】
		*		  [in] maxAltitude  最大高度 【默认为10000】
		*		  [out] result 选中的对象集合
		* @note region在Globe模式下的坐标格式是(经度，纬度)，Flat模式下是(x,y)
		* @return 有对象选中返回 true
		*		   没有对象选中返回false
		*/
		glbBool Pick( CGlbPolygon * region,	std::vector<glbref_ptr<CGlbGlobeObject>>&result, glbDouble minAltitude=-1000.0, glbDouble maxAltitude=10000.0);
		/**
		* @brief 获取相机焦点的屏幕坐标,以窗口左下角为（0,0）
		* @param [out] x 
		*		  [out] y
		*/
		void getScreenFocusPt(glbDouble& x,glbDouble& y);
		/**
		* @brief 获取osg相机
		* @return osg相机
		*/
		osg::Camera* GetOsgCamera();

		/**
		* @brief 获取导航盘要显示的偏航角
		* @return 偏航角
		*/
		glbDouble GetYaw();

		/**
		* @brief 转动导航盘Yaw角(0~360)
		* @param [in] yaw 输入的yaw角
		*		  [in] old_focus 旧的焦点位置
		*		  [in] olddistance 旧的相机旧焦点之间的距离
		* @return 成功返回 true
		*			失败返回false
		*/
		glbBool SetYaw(glbDouble yaw,osg::Vec3d old_focus, glbDouble olddistance);

		/**
		* @brief 获取导航盘要显示的Pitch角
		* @return 俯仰角
		*/
		glbDouble GetPitch();

		/**
		* @brief 转动导航盘Pitch角(-90~0)
		* @param [in] yaw 输入的pitch角
		*		  [in] old_focus 旧的焦点位置
		*		  [in] olddistance 旧的相机旧焦点之间的距离
		* @return 成功返回 true
		*			失败返回false
		*/
		glbBool SetPitch(glbDouble pitch,osg::Vec3d old_focus, glbDouble olddistance);

		/**
		* @brief 获取相机位置
		* @param [out] cameraPos 相机位置，球面模式用经纬(单位°)高表示，平面模式用xyz表示
		* @return 成功返回 true
		*			失败返回false
		*/
		glbBool GetCameraPos(osg::Vec3d& cameraPos);
		/**
		* @brief 获取相机Yaw
		*
		*/
		glbDouble GetCameraYaw();
		/**
		* @brief 获取相机Pitch
		*
		*/
		glbDouble GetCameraPitch();

		/**
		* @brief 获取焦点位置
		* @param [out] focusPos 焦点位置，球面模式用经纬(单位°)高表示，平面模式用xyz表示
		* @return 成功返回 true
		*			失败返回false
		*/
		glbBool GetFocusPos(osg::Vec3d& focusPos);

		/**
		* @brief 获取焦点相机距离
		* @return 焦点相机距离，单位米
		*/
		glbDouble GetDistance();

		/**
		* @brief 设置相机参数
		* @param yaw   绕Z或向上方向旋转的角度
				 pitch 绕X或东方向旋转的角度
				 focusPos  焦点坐标
				 distance  焦点与相机之间的距离
		*/
		void SetCameraParam(glbDouble yaw, glbDouble pitch, osg::Vec3d focusPos, glbDouble distance);
		/*
		* @brief定义 拖拽，旋转，缩放 三个动作
		*/
		void Drag(glbInt32 ptx1,glbInt32 pty1,glbInt32 ptx2,glbInt32 pty2);
		void Rotate(glbInt32 ptx1,glbInt32 pty1,glbInt32 ptx2,glbInt32 pty2);
		void Zoom(glbBool isScrollUp);
		/**
		* @brief 飞向
		* @param [in] 目标点状态：焦点位置xOrlon,yOrlat,zOrAlt
		*		  [in] 目标点状态：相机焦点距离distance,焦点处yaw,pitch
		*		  [in] 飞行时长 seconds
		* @return 成功返回true
		*		-	失败返回false
		*/
		glbBool FlyTo(glbDouble xOrlon, glbDouble yOrlat, glbDouble zOrAlt,glbDouble distance,glbDouble yaw,glbDouble pitch, glbDouble seconds=1.0);

		/**
		* @brief 跳向
		* @param [in] 目标点状态：焦点位置xOrlon,yOrlat,zOrAlt
		*		  [in] 目标点状态：相机焦点距离distance,焦点处yaw,pitch
		* @return 成功返回true
		*		-	失败返回false
		*/
		glbBool JumpTo(glbDouble xOrlon, glbDouble yOrlat, glbDouble zOrAlt,glbDouble distance,glbDouble yaw, glbDouble pitch);

		/**
		* @brief 环绕飞行
		* @param [in] 环绕飞行中心点坐标：xOrlon,yOrlat,zOrAlt
		*		  [in] 飞行时长 seconds
		* @return 成功返回true
		*		-	失败返回false
		*/
		glbBool FlyAround(glbDouble lonORx,glbDouble latORy,glbDouble zOrAlt,glbDouble seconds,glbBool repeat);

		/**
		* @brief 判断是否正在飞行状态中
		* @return 是返回true
		*		-	否返回false
		*/
		glbBool IsFly();

		/**
		* @brief 停止飞行
		* @return 成功返回true
		*		-	失败返回false
		*/
		glbBool StopFly();

		/**
		* @brief 操作导航盘,平移相机
		* @param [in] anglex 绕过地心平行于屏幕竖直方向的轴转动角度
		*		  [in] angley 绕过地心平行于屏幕水平方向的轴转动角度
		* @return 成功返回 true
		*			失败返回false
		*/
		glbBool Move(glbDouble anglex,glbDouble angley);

		//2014.11.24 aj添加，给导航条drag调用
		/**
		* @brief 相机平移
		* @param [in] x,y,z相机平移向量，世界坐标系下
		* @return 成功返回 true
		*			失败返回false
		*/
		glbBool TranslateCamera(glbDouble x,glbDouble y,glbDouble z);

		// 		/**
		// 		 * @brief 操作导航盘,平移相机
		// 		 * @param [in] lonOrx1,latOry1 平移前相机的经度纬度
		// 		 *		  [in] lonOrx2,latOry2 平移后相机的经度纬度
		// 		 * @return 成功返回 true
		// 		 *			失败返回false
		// 		**/
		// 		glbBool Move(glbDouble lonOrx1,glbDouble latOry1,glbDouble lonOrx2,glbDouble latOry2);

		/**
		* @brief 更新相机所在经纬度的地形高度
		* @param [in] elevation ：相机所在经纬度的地形高度
		*/
		void UpdateTerrainElevationOfCameraPos(glbDouble elevation);

		/**
		* @brief 更新焦点所在经纬度的地形高度 - 废弃【2015.3.10 马林】
		* @param [in] elevation ：焦点所在经纬度的地形高度
		*/
		void UpdateTerrainElevationOfFoucsPos(glbDouble elevation);

		/**
		* @brief 获取相机所在经纬度的地形高度
		* @return  相机所在经纬度的地形高度
		*/
		glbDouble GetTerrainElevationOfCameraPos();

		/**
		* @brief 更新计算视线与地形交点
		*/
		void UpdateFocusElevate(/*glbBool bInter, osg::Vec3d interPos*/);

		/**
		* 接口应经弃用
		* @brief 获取计算视线与地形交点
		* @param [out] interPos ：交点
		* @return true有交点，false无交点 
		*/
		//glbBool getSightLineInterTerrain(osg::Vec3d& interPos);

		/**
		* @brief 获取焦点所在经纬度的地形高度
		* @return  焦点所在经纬度的地形高度
		*/
		glbDouble GetTerrainElevationOfFoucsPos();

		/**
		* @brief 显示导航盘
		*/
		void ShowNavigator(bool isShow);
		/**
		* @brief 显示导航标尺
		*/
		void ShowElevationRuler(bool isShow);
		/**
		* @brief 显示状态栏
		*/
		void ShowStatusBar(bool isShow);
		/**
		* @brief 显示LOGO
		*/
		void ShowLogo(bool isShow);
		/**
		* @brief 显示比例尺
		*/
		void ShowScaleRuler(bool isShow);
		/**
		* @brief 显示中心标记
		*/
		void ShowCentermark(bool isShow);
		/**
		* @brief 显示星空
		*/
		void ShowStars(bool isShow);
		/**
		* @brief 获取星空
		*/
		osg::Node *GetStars();
		/**
		* @brief 显示太阳
		*/
		void ShowSun(bool isShow);
		/**
		* @brief 显示月亮
		*/
		void ShowMoon(bool isShow);
		/**
		* @brief 显示大气
		*/
		void ShowAtmosphere(bool isShow);
		/**
		* @brief 显示天穹
		*/
		void ShowSkydom(bool isShow);
		/**
		* @brief 获取天穹
		*/
		osg::Node *GetSkydom();
		/**
		* @brief 获取天空系统节点
		*/
		osg::Node *GetGlobeElementNode();
		/**
		* @brief 设置许可服务
		*/
		bool SetLicSrv(CGlbWString ip,glbInt32 port);
		/**
		* @brief 获取场景默认光源模式
		* @return 光源模式
		*/
		GlbGlobeLightingModeEnum GetLightingMode();
		/**
		* @brief 设置场景默认光源模式
		* @param [in] mode 光源模式
		* @return 成功返回true
					失败返回false
		*/
		bool SetLightingMode(GlbGlobeLightingModeEnum mode);

		/**
		* @brief 获取场景中的光源数量
		* @return 光源数量
		*/
		glbInt32 GetLightsCount();
		/**
		* @brief 通过索引值获取场景中的光源
		* @param [in] idx 索引
				 [out] lgt 光源 
		* @return 成功返回true ，失败返回false
		*/
		glbBool GetLightByIndex(glbInt32 idx, GlbGlobeLight& lgt);
		/**
		* @brief 通过光源id	获取场景中的光源
		* @param [in] id 光源id
				 [out] lgt 光源 
		* @return 成功返回true ，失败返回false
		*/
		glbBool GetLightById(glbInt32 id, GlbGlobeLight& lgt);
		/**
		* @brief 添加光源
		* @param [in] light 光源
		* @return 成功返回true
				失败返回false
		*/
		glbBool AddLight(GlbGlobeLight light);
		/**
		* @brief 通过光源ID删除光源
		* @param [in] idx 光源序号
		* @return 成功返回true
				失败返回false
		*/
		glbBool RemoveLight(glbInt32 id);
		/**
		* @brief 更新光源
		* @param [in][out] light 光源
		* @return 成功返回true
				失败返回false
		*/
		glbBool	UpdateLight(GlbGlobeLight& light);
		/**
		* @brief 保存当前视图到文件
		* @param [in] strFileName 文件全路径
		* @return 成功返回true
				失败返回false
		*/
		glbBool SaveImage(CGlbWString strFileName);
		/**
		* @brief 保存当前视图到文件
		* @param [in] strFileName 文件全路径
				 [in] imgWidth 图像宽度
				 [in] imgHeight 图像高度
		* @return 成功返回true
				失败返回false
		*/
		glbBool SaveImage(CGlbWString strFileName, glbInt32 imgWidth, glbInt32 imgHeight);

		/**
		* @brief 根据相机位置和焦点位置计算相机位置的pitch和yaw
		* @param [in] cameraLonOrX，cameraLatOrY， cameraAltOrZ 相机位置 【经纬（单位：度）高或XYZ】
					[in] focusLonOrX，focusLatOrY，focusAltOrZ 焦点位置	【经纬（单位：度）高或XYZ】
					[out] pitch  视线方向与x轴夹角 （相机坐标系）
					[out] yaw    视线方向与z轴夹角	

		*/
		void ComputeCameraPitchAndYaw(glbDouble cameraLonOrX, glbDouble cameraLatOrY, glbDouble cameraAltOrZ,
										glbDouble focusLonOrX, glbDouble focusLatOrY, glbDouble focusAltOrZ ,
										glbDouble &pitch, glbDouble &yaw);

		//////////////////////////////////////////////////////////////////////////
		// 相机抖动相关接口
		/*
		* @brief 启动/停止 相机抖动
		* @param [in] isShake true-启动相机抖动 false-停止相机抖动  
		*/
		void	Shake(glbBool isShake);
		/**
		* @brief 获取是否启用了相机抖动
		* @return	启动相机抖动 返回true
					没有相机抖动 返回false
		*/
		glbBool IsShake();
		/*
		* @brief 设置 相机抖动幅度
		* @param [in] horizontalDegree 水平方向抖动幅度 单位是【度】 默认为2度 
	    		 [in] verticalDegree 水平方向抖动幅度 单位是【度】 默认为2度 
		*/
		void	SetShakeSope(glbDouble horizontalDegree = 2.0, glbDouble verticalDegree = 2.0);
		/*
		* @brief 获取 相机抖动幅度
		* @param [out] horizontalDegree 水平方向抖动幅度 单位是【度】 
	    		 [out] verticalDegree 水平方向抖动幅度 单位是【度】  
		* @return	 成功返回true
					 失败返回false
		*/
		glbBool GetShakeSope(glbDouble &horizontalDegree, glbDouble &verticalDegree);
		/*
		* @brief 设置 相机抖动 时间
 		* @param [in] seconds 相机抖动时间 单位： 秒 
		*/
		void	SetShakeTime(glbDouble seconds);
		/**
		* @brief 获取抖动时间
		* @return 抖动时间
		*/
		glbDouble GetShakeTime();
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

		osg::ref_ptr<osg::Geode> createEarth();	//测试代码球模式

		osg::ref_ptr<osg::Geode> createPlane();	//测试代码平面模式	

		/**
		* @brief 判断包围盒与视锥是否相交
		* @param [in] bb ：包围盒
		* @return 相交返回 true
		*			不想交返回false
		*/
		glbBool contains(const osg::BoundingBox& bb);


		osg::Group *getReflectNode()const{return mpr_p_root.get();}

		osg::Group *getSceneRootNode()const{return mpr_p_root.get();}

		osg::Group *getSceneGlobeNode()const{return mpr_globe->mpr_root.get();}

		osg::Group *getGroundNode()const{return mpr_globe->mpr_p_objects_groundnode;}

		osg::Group *getNormalSceneRootNode()const{return mpr_globe->mpr_root.get();}

		osg::Group *getViewSceneNode()const{return mpr_viewSceneNode.get();}
		void setViewSceneNode(osg::Group *viewSceneNode)
		{
			if (mpr_viewSceneNode.valid())
				return;

			mpr_p_root->removeChild(mpr_globe->mpr_root.get());
			viewSceneNode->addChild(mpr_globe->mpr_root.get());
			mpr_p_root->addChild(viewSceneNode);		

			mpr_viewSceneNode=viewSceneNode;
		}

		CGlbGlobeSceneManager *getSceneManager()const{return mpr_p_rootEx;}


		//获取当前场景中心点附近一个像素的宽度
		glbDouble GetCurrentPixelWidth();

		osg::Texture2D *getSceneDepthTex()const{return mpr_depthTexture.get();}
		osg::Texture2D *getSceneColorTex()const{return mpr_sceneTexture.get();}

		inline void setUpdateCameraMode(glbBool mode){mpr_updateCameraMode = mode;}
		inline glbBool getUpdateCameraMode(){return mpr_updateCameraMode;}
		/**
		* @brief 添加联动视口
		* @param [in] x  视口的水平起始位置
		* @param [in] y	  视口的竖直起始位置
		* @param [in] width    视口的宽
		* @param [in] height   视口的高
		* @return 成功返回true
		*		-	失败返回false
		**/
		glbBool AddSlaveCamera(int x,int y,int width,int height);
		/**
		* @brief 删除联动视口
		* @return 成功返回true
		*		-	失败返回false
		**/
		glbBool RemoveSlaveCamera();
		/**
		* @brief 开启地下参考面
		* @param [in] true表示强制打开地下参考面，
		*                        false表示使用默认的地下参考面方式
		**/
		void OpenGlobeUGPlane(glbBool isOpen);

	//public:
	//	// 释放编译context生成的资源
	//	void FreeCompileContextResource();
	private:
		// 判断obj是否在result中，是返回true，否则返回false
		bool _isObjectHaveSelected(CGlbGlobeObject* obj, std::vector<glbref_ptr<CGlbGlobeObject>>& result);
		//初始化视图元素
		void initElements();
		// 创建太阳光源
		osg::Node* createSunLight();
		glbBool osgLight2glbLight(osg::Light* lgt, GlbGlobeLight& glbLight);
		glbBool glbLight2osgLight(GlbGlobeLight& glbLight, osg::Light* lgt);

		// 创建对称太阳光源
		osg::Node* createSymmetricalSunLight();

		// 临时添加的烟雾效果，在focusPos位置起烟，配合相机震动，实现地震效果 2015.12.11
		osg::ref_ptr<osg::Node> CreateSmoke(); 
		osg::ref_ptr<osg::Fog> CreateFog(bool m_Linear);
		// 创建自定义烟雾粒子系统
		osg::ref_ptr<osg::Group> CreateMySmokeParticle(osg::Vec3 pos);
		// 测试 创建坐标轴对象
		//void TestCreateCoordinateAxisObject();
	public:
		WNDPROC _windowProcedure;	
	private:
		//私有成员变量
		CGlbWString					mpr_classname;	     //窗口类名
		HWND                        mpr_hwnd;            //窗口句柄
		HWND                        mpr_parent_hwnd;     //窗口父句柄
		HCURSOR                     mpr_cursor;			 //窗口光标

		osg::ref_ptr<myosgviewer>   mpr_osgviewer;					//osg 视图
		osg::ref_ptr<osg::Group>	mpr_p_root;						//osg根节点 
		osg::ref_ptr<CGlbGlobeSceneManager>	  mpr_p_rootEx;						//osg根节点 
		osg::ref_ptr<CGlbGlobeMousePickHandle> mpr_mousepick;
		osg::ref_ptr<CGlbGlobeManipulatorManager> mpr_manipulatormanager;		//操控器管理器
		glbref_ptr<CGlbGlobe> mpr_globe;							//场景对象
		IGlbGlobeViewEventHandle* mpr_eventhandle;					//窗口事件回调处理
		CGlbGlobeUpdateThread* mpr_updatethread;					//数据更新线程类
		CGlbGlobeRenderThread* mpr_renderthread;					//渲染更新线程类
		CGlbGlobeComputeElevationThread* mpr_computeElvthread;		//高程计算线程类
		CGlbGlobeDispatcherThread* mpr_dispatcherthread;			//调度线程
		osg::ref_ptr<CGlbGlobeClipHandler> mpr_cliphandler;			//裁剪回调

		glbBool mpr_isrun;											//是否已经启动渲染线程

		glbDouble mpr_UndergroundDepth;		//地下参考面高度
		glbDouble mpr_cameraElevation;		//相机经纬度处地形高度
		glbDouble mpr_focusElevation;		//焦点经纬度处地形高度

		//视图相关元素变量
		osg::ref_ptr<CGlbGlobeViewStatusBar>				mpr_statusbar;	//状态栏
		osg::ref_ptr<CGlbGlobeViewElevationRuler>			mpr_ruler;		//导航标尺
		osg::ref_ptr<CGlbGlobeViewNavigator>				mpr_navigator ;	//导航盘
		osg::ref_ptr<CGlbGlobeViewLogo>						mpr_logo;		//LOGO
		osg::ref_ptr<CGlbGlobeViewScale>					mpr_scale;		//比例尺
		osg::ref_ptr<CGlbGlobeViewCentermark>				mpr_centermark;	//中心标记
		glbref_ptr<IGlbGlobeElement>                        mpr_sky; //天空要素		

		osg::ref_ptr<osg::Texture2D>                        mpr_depthTexture;
		osg::ref_ptr<osg::Texture2D>                        mpr_sceneTexture;

		osg::ref_ptr<osg::Group>                            mpr_viewSceneNode;
		glbBool												mpr_updateCameraMode;	//相机纠偏计算模式，false为其他线程通知模式，true为预计算模式;F1键为false,F2键为true

		osg::ref_ptr<osg::GraphicsContext>					mpr_shareContext;
		//glbBool mpr_bInter;
		//osg::Vec3d mpr_interPos;

		std::map<glbInt32,GlbGlobeLight>					mpr_lightsManager;//光源管理器
		//std::vector<GlbGlobeLight>							mpr_lightsManager2;
		std::vector<glbInt32>								mpr_lightsId;
		osg::ref_ptr<osg::Group>							mpr_p_LightsRoot; //光源根节点

		glbBool												mpr_bShake;
		glbDouble											mpr_shakeTime;
		glbDouble											mpr_shakeHorDegree;
		glbDouble										    mpr_shakeVerDegree;

		// temp
		osg::ref_ptr<osg::Node>								mpr_smokeNode;
		osg::ref_ptr<osg::Fog>								mpr_fog; 

		osg::Vec4f											mpr_backColor;
		osg::Vec4f											mpr_undergroundBackColor;

		//记录鼠标位置
		long                                                mpr_nextXpos;
		long                                                mpr_nextYpos;
			
		//使用手动重置为无信号状态，初始化时无信号状态										
		HANDLE												mpr_hEvent_SaveImage;
	};
}