/********************************************************************
* Copyright (c) 2014 北京超维创想信息技术有限公司
* All rights reserved.
* @file    GlbGlobeDynamicObject.h
* @brief   动态对象头文件，文档定义CGlbGlobeDynamicObject类
* @version 1.0
* @author  敖建
* @date    2014-7-1 15:07
*********************************************************************/ 
#pragma once

#include "GlbGlobeObjectExport.h"
#include "GlbGlobeREObject.h"
#include "GlbGlobeDynamic.h"
#include "CGlbGlobe.h"

namespace GlbGlobe
{ 
	class CGlbGlobePoint;
	class IGlbGlobeTask;
	/**
	* @brief 动态对象类	 
	* @version 1.0
	* @author  敖建
	* @date    2014-7-1 15:07
	*/
	class GLB_OBJECTDLL_CLASSEXPORT CGlbGlobeDynamicObject : public CGlbGlobeREObject,public CGlbGlobeDynamic
	{
		/**
		* @brief KeyPoint关键点,动态对象中记录关键点的结构体	 
		* @version 1.0
		* @author  敖建
		* @date    2014-7-10 10:42
		*/
		struct KeyPoint
		{
			glbDouble xorlon;
			glbDouble yorlat;
			glbDouble zoralt;
			glbDouble yaw;
			glbDouble pitch;
			glbDouble roll;
			glbInt32 action;			//事件ID 1-无  2-媒体播放
			std::string mediafilepath;	//媒体文件路径
			glbBool	 mediaMarker;		//媒体是否已播放标志 - 默认值为false
		};

	private:
		/**
		* @brief 动态对象动画回调，私有类	 
		* @version 1.0
		* @author  敖建
		* @date    2014-7-5 11:22
		*/
		class CGlbGlobeDynamicObjCallback : public osg::NodeCallback
		{
		public:
			/**
			* @brief 构造函数
			*/
			CGlbGlobeDynamicObjCallback(CGlbGlobeDynamicObject* dynamicObject,osg::AnimationPath::TimeControlPointMap tcpmap,glbBool isLoop):
			  _dynamicObject(dynamicObject),
				  _tcpmap(tcpmap),
				  _isLoop(isLoop),
				  _firstTime(DBL_MAX),
				  _latestTime(0.0),
				  _pause(false),
				  _pauseTime(0.0),	
				  _timeOffset(0.0),
				  _timeMultiplier(1.0),
				  _lastFrametime(DBL_MAX)
			  { 
				  _lastFrameyaw = 0.0;
				  _speed = 0.0;
				  _lastFramePosition = osg::Vec3d(0.0,0.0,0.0);
				  _second = _tcpmap.begin()->first;
				  osg::AnimationPath::ControlPoint cp;
				  osg::AnimationPath::TimeControlPointMap::const_iterator second = _tcpmap.lower_bound(_second);
				  _dynamicObject->CGlbGlobeDynamicObject::ControlPointToPosition(cp,&_xorlon,&_yorlat,&_zoralt,&_yaw,&_pitch,&_roll);
			  }
			  /**
			  * @brief 析构函数
			  */
			  virtual ~CGlbGlobeDynamicObjCallback(){}
			  /**
			  * @brief 回调函数
			  */
			  virtual void operator()(osg::Node* node, osg::NodeVisitor* nv);
			  /**
			  * @brief 获取动态对象当前运动了多长时间
			  */
			  glbDouble getAnimationTime() const;
			  /**
			  * @brief 设置是否暂停
			  **/
			  void setPause(glbBool pause);
			  /**
			  * @brief 获取是否暂停
			  */
			  inline glbBool getPause() const { return _pause; }
			  /**
			  * @brief 获取相对暂停时间
			  */
			  inline glbDouble getoppositetimeOffset(){ return _pauseTime - _firstTime; }	
			  /**
			  * @brief 设置偏移时间
			  */
			  inline void settimeOffset(glbDouble timeoffset){ _timeOffset = timeoffset; }
			  /**
			  * @brief 设置偏移时间
			  */
			  inline osg::AnimationPath::TimeControlPointMap getTimeControlPointMap(){ return _tcpmap;}
			  /*
				*@brief 获取当前时间，位置，姿态
			  */
			  glbBool GetCurrentPosition(glbDouble* xOrlon,glbDouble* yOrlat,glbDouble* zOrAlt,glbDouble* yaw,glbDouble* pitch,glbDouble* roll,glbDouble* seconds);
		private:
			CGlbGlobeDynamicObject* _dynamicObject;
			osg::AnimationPath::TimeControlPointMap _tcpmap;
			glbBool _isLoop;
			glbDouble _firstTime;
			glbDouble _latestTime; 
			glbBool _pause;
			glbDouble _pauseTime;			
			glbDouble _timeOffset;
			glbDouble _timeMultiplier;

			glbDouble _lastFrametime;		//上一帧时间
			glbDouble _lastFrameyaw;		//上一帧yaw角
			osg::Vec3d _lastFramePosition;	//上一帧位置
			glbDouble _speed;				//飞行速度			

			GlbCriticalSection _critical;	// 临界区对象，读写互斥
			glbDouble _xorlon,_yorlat,_zoralt,_yaw,_pitch,_roll,_second; // 当前相机时刻位置姿态
		};

		class CGlbGlobeDynamicObjectTask : public IGlbGlobeTask
		{
		public:
			CGlbGlobeDynamicObjectTask(CGlbGlobeDynamicObject *obj,glbInt32 level);
			~CGlbGlobeDynamicObjectTask();
			glbBool doRequest();
			CGlbGlobeObject *getObject();
		private:
			glbref_ptr<CGlbGlobeRObject> mpr_obj; //CGlbGlobeDynamicObject
			glbInt32 mpr_level;
		};

	public:
		/**
		* @brief 构造函数
		*/
		CGlbGlobeDynamicObject();
		/**
		* @brief 析构函数
		*/
		virtual ~CGlbGlobeDynamicObject();

		//CGlbGlobeREObject method
	public:
		/**
		* @brief 反序列化对象，解析xml获取对象属性值
		* @param[in]  node libXML2节点
		* @param[in]  prjPath 工程文件目录
		* @return 反序列化成功返回true、反之返回false
		*/
		glbBool Load(xmlNodePtr *node,const glbWChar* prjPath);
		/**
		* @brief 序列化对象，序列化对象属性到xml工程文件
		* @param[in]  os 工程文件输出流
		* @param[in]  prjPath 工程文件目录
		* @return 序列化成功返回true、反之返回false
		*/
		glbBool Save(xmlNodePtr node,const glbWChar* prjPath);
		/**
		* @brief 获取对象类型
		* @return 对象枚举类型
		*/
		GlbGlobeObjectTypeEnum GetType();
		/**
		* @brief 获取相机与对象的距离
		* @param[in]  cameraPos 相机在场景中的位置
		* @param[in]  isCompute 距离是否需要重新计算，重新计算设置true，反之设置false
		* @return 相机与对象的距离
		*/
		glbDouble GetDistance(osg::Vec3d &cameraPos,glbBool isCompute);
		/**
		* @brief 装载数据，调度器会频繁的调用该方法
		* @param[in]  level 需要装载数据的等级
		*/
		void LoadData(glbInt32 level);
		/**
		* @brief 挂osg节点或者交换osg节点到场景树
		*/
		void AddToScene();
		/**
		* @brief 从场景树摘除osg节点
		* @param[in]  isClean true表示同时删除所有osg节点以节省内存
		* @return 返回节省的内存大小
		*/
		glbInt32 RemoveFromScene(glbBool isClean);
		/**
		* @brief 获取对象节点包围盒
		* @param[in]  isWorld true表示获取世界坐标包围盒(单位：米)
		*  -                                    false表示获取地理坐标包围盒（单位：度）
		* @return 节点包围盒
		*/
		glbref_ptr<CGlbExtent> GetBound(glbBool isWorld = true);
		/**
		* @brief 相对地形绘制时，更新对象对应点的地形高度
		*/
		void UpdateElevate();
		/**
		* @brief 对象立即渲染
		* @param[in]  level 对象渲染等级
		*/
		void DirectDraw(glbInt32 level);
		/**
		* @brief 设置对象海拔模式
		* @param[in]  mode 海拔模式枚举
		* @param[out] isNeedDirectDraw 是否需要调用DirectDraw才能立即看到修改效果：true需要,false不需要.
		* @return 设置成功返回true，设置失败返回false
		*/
		glbBool SetAltitudeMode(GlbAltitudeModeEnum mode,glbBool &isNeedDirectDraw);	
		/**
		* @brief 设置对象的渲染信息
		* @param[in]  renderInfo 对象的渲染信息
		* @param[out] isNeedDirectDraw 是否需要调用DirectDraw才能立即看到修改效果：true需要,false不需要.
		* @return 设置成功返回true，反之返回false
		*/
		glbBool SetRenderInfo(GlbRenderInfo *renderInfo,glbBool &isNeedDirectDraw);
		/**
		* @brief 获取对象的渲染信息
		* @return 对象的渲染信息
		*/
		GlbRenderInfo *GetRenderInfo();
		/**
		* @brief 设置对象显隐
		* @param[in]  isShow 对象显示设置为true，隐藏设置为false
		* @return 设置成功返回true，反之返回false
		*/
		glbBool SetShow(glbBool isShow,glbBool isOnState = false);
		/**
		* @brief 设置对象的选中状态
		* @param[in]  isSelected 被拣选中设置为true，反之设置为false
		* @return 设置成功返回true，反之返回false
		*/
		glbBool SetSelected(glbBool isSelected);
		/**
		* @brief 获取当前LOD节点的大小，单位“字节”
		* @return 当前LOD级的osg节点大小
		*/
		glbInt32 GetOsgNodeSize();
		/**
		* @brief 设置对象进入或退出"编辑"状态
		* @param[in]  isEdit 进入编辑状态为true，退出为false
		* @return 成功返回true，失败返回false
		*/
		glbBool SetEdit(glbBool isEdit);
		//CGlbGlobeDynamic method 
	public:
		/**
		* @brief 添加控制点
		* @param [in] 控制点位置xOrlon,yOrlat,zOrAlt（球模式为经纬高，经度纬度单位为°，海拔高度单位为米；平面模式单位为米）
		*		  [in] 控制点姿态yaw，pitch,roll(单位为°)
		*		  [in] 控制点时间 seconds（单位为秒）
		* @return 成功返回true
		*		-	失败返回false
		*/
		virtual glbBool AddPosition(glbDouble xOrlon,glbDouble yOrlat,glbDouble zOrAlt,glbDouble yaw,glbDouble pitch,glbDouble roll,glbDouble seconds);
		/**
		* @brief 插入控制点
		* @param [in] 控制点编号 nId
		*		  [in] 控制点位置xOrlon,yOrlat,zOrAlt（球模式为经纬高，经度纬度单位为°，海拔高度单位为米；平面模式单位为米）
		*		  [in] 控制点姿态yaw，pitch,roll(单位为°)
		*		  [in] 控制点时间 seconds（单位为秒）
		* @return 成功返回true
		*		-	失败返回false
		*/
		virtual glbBool InsertPosition(glbInt32 nId,glbDouble xOrlon,glbDouble yOrlat,glbDouble zOrAlt,glbDouble yaw,glbDouble pitch,glbDouble roll,glbDouble seconds);
		/**
		* @brief 替换控制点
		* @param [in] 控制点编号 nId
		*		  [in] 控制点位置xOrlon,yOrlat,zOrAlt（球模式为经纬高，经度纬度单位为°，海拔高度单位为米；平面模式单位为米）
		*		  [in] 控制点姿态yaw，pitch,roll(单位为°)
		*		  [in] 控制点时间 seconds（单位为秒）
		* @return 成功返回true
		*		-	失败返回false
		*/
		virtual glbBool ReplacePosition(glbInt32 nId,glbDouble xOrlon,glbDouble yOrlat,glbDouble zOrAlt,glbDouble yaw,glbDouble pitch,glbDouble roll,glbDouble seconds);
		/**
		* @brief 移除控制点
		* @param [in] 控制点编号 nId
		* @return 成功返回true
		*		-	失败返回false
		*/
		virtual glbBool RemovePosition(glbInt32 nId);
		/**
		* @brief 获取控制点个数
		* @return 控制点个数
		*/
		virtual glbInt32 GetCount();
		/**
		* @brief 获取控制点信息
		* @param [in] 控制点编号 nId
		*		  [out] 控制点位置*xOrlon,*yOrlat,*zOrAlt（球模式为经纬高，经度纬度单位为°，海拔高度单位为米；平面模式单位为米）
		*		  [out] 控制点姿态*yaw,*pitch,*roll(单位为°)
		*		  [out] 控制点时间 *seconds（单位为秒）
		* @return 成功返回true
		*		-	失败返回false
		*/
		virtual glbBool GetPosition(glbInt32 nId,glbDouble* xOrlon,glbDouble* yOrlat,glbDouble* zOrAlt,glbDouble* yaw,glbDouble* pitch,glbDouble* roll,glbDouble* seconds);
		/**
		* @brief 获取当前时刻控制点信息
		* @param [out] 控制点*cp
		* @return 成功返回true
		*		-	失败返回false
		*/
		virtual glbBool GetControlPoint(osg::AnimationPath::ControlPoint *cp);
		/**
		* @brief 获取获取当前漫游位置点信息
		* @param  [out] 控制点位置*xOrlon,*yOrlat,*zOrAlt（球模式为经纬高，经度纬度单位为°，海拔高度单位为米；平面模式单位为米）
		*		  [out] 控制点姿态*yaw,*pitch,*roll(单位为°)
		*		  [out] 控制点时间 *seconds（单位为秒）
		* @return 成功返回true
		*		-	失败返回false
		*/
		virtual glbBool GetCurrentPosition(glbDouble* xOrlon,glbDouble* yOrlat,glbDouble* zOrAlt,glbDouble* yaw,glbDouble* pitch,glbDouble* roll,glbDouble* seconds);

		//编辑表达相关 -- 编辑要重新做SetEdit()
	public:
		/**
		* @brief 设置临时编辑点颜色
		* @param [in] 点ID nId
		*		  [in] 点颜色 color
		* @return 成功返回true
		*		-	失败返回false
		*/
		glbBool SetPointColor(glbInt32 nId,glbInt32 color);		 
		/**
		* @brief 添加绘制点
		* @param [in] 点坐标 xorlon,yorlat,zoralt
		*		  [in] 点颜色 color
		* @return 成功返回true
		*		-	失败返回false
		*/
		glbBool AddPoint(glbDouble xorlon,glbDouble yorlat,glbDouble zoralt,glbInt32 color);
		/**
		* @brief 替换(移动)绘制点
		* @param [in] 点ID nId
		*		  [in] 点坐标 xorlon,yorlat,zoralt
		*		  [in] 点颜色 color
		* @return 成功返回true
		*		-	失败返回false
		*/
		glbBool ReplacePoint(glbInt32 nId,glbDouble xorlon,glbDouble yorlat,glbDouble zoralt,glbInt32 color);
		/**
		* @brief 插入绘制点
		* @param [in] 点ID nId
		*		  [in] 点坐标 xorlon,yorlat,zoralt
		*		  [in] 点颜色 color
		* @return 成功返回true
		*		-	失败返回false
		*/		
		glbBool InsertPoint(glbInt32 nId,glbDouble xorlon,glbDouble yorlat,glbDouble zoralt,glbInt32 color);
		/**
		* @brief 删除点
		* @param [in] 点ID nId
		* @return 成功返回true
		*		-	失败返回false
		*/
		glbBool RemovePoint(glbInt32 nId);
		/**
		* @brief 清除所有点
		* @return 成功返回true
		*		-	失败返回false
		*/
		glbBool RemoveAllPoints();
		/**
		* @brief 获取是否显示控制点
		* @return 是返回true
		*		-	否返回false
		*/
		glbBool IsShowPoints();
		/**
		* @brief 设置是否显示控制点
		*/
		void SetShowPoints(glbBool isShow);
		/**
		* @brief 获取是否显示路径
		* @return 是返回true
		*		-	否返回false
		*/
		glbBool IsShowPath();
		/**
		* @brief 设置是否显示路径
		*/
		void SetShowPath(glbBool isShow);
		//动态对象激活回调
	public:
		/**
		* @brief 获取对象是否激活
		* @return 是返回true
		*		-	否返回false
		*/
		glbBool IsRunning();
		/**
		* @brief 激活动态对象
		*/
		glbBool Run();
		/**
		* @brief 停止动态对象
		*/
		glbBool Stop();
		/**
		* @brief 暂停动态对象
		*/
		glbBool Pause();
		/**
		* @brief 读取动态对象路径文件3dmax导出的
		*/	
		glbBool read_3dmaxpath(CGlbWString fliepath);
		/**
		* @brief 读取动态对象路径文件creatar导出的
		*/	
		glbBool read_creatarpath(CGlbWString fliepath);
		/**
		* @brief 写入动态对象路径文件3dmax格式
		*/	
		void write_3dmaxpath(CGlbWString fliepath);
		/**
		* @brief 写入动态对象路径文件creatar格式
		*/
		void write_creatarpath(CGlbWString fliepath);
		void writepath(CGlbWString fliepath,osg::AnimationPath::ControlPoint cp);
		//////////////////////////////////////////////////////////////////////////
		void testDynamicObject(CGlbWString filepath);
		/**
		 * @brief 在动态对象节点回调中处理用户自定义的事件
		 * @param [in] time 当前时刻
		**/	
		void callUserFunc(glbDouble time);
		/**
		 * @brief 初始化媒体播放map
		 * @note 在每次开始漫游结束时调用
		**/
		void initMediaMap();
		/**
		* @brief 设置动态运动的起始时间
		* @param [in] time 运动的起始时间
		**/
		void SetStrartTime(glbDouble startTime);
		/**
		* @brief 设置动态运动的时相机的附加旋转角度（三个方向yaw,pitch,roll）单位：度
		* @param [in] yaw是围绕Y轴旋转,也叫偏航角, pitch是围绕X轴旋转,也叫做俯仰角,roll是围绕Z轴旋转，也叫翻滚角
		*				globe中其实与传统的定义是有区别的Z，Y轴交换了
		**/
		void SetAdditionalRotate(glbDouble yaw, glbDouble pitch, glbDouble roll);
		/**
		* @brief 获取动态运动的时相机的附加旋转角度（三个方向yaw,pitch,roll）
		* @param [in] yaw是围绕Y轴旋转,也叫偏航角, pitch是围绕X轴旋转,也叫做俯仰角,roll是围绕Z轴旋转，也叫翻滚角
		*				globe中其实与传统的定义是有区别的Z，Y轴交换了
		**/		
		void GetAdditionalRotate(glbDouble& yaw, glbDouble& pitch,glbDouble& roll);
	//私有函数成员
	private:
		/**
		* @brief 装载数据
		*/
		void ReadData(glbInt32 level ,glbBool isDirect=false);

		void DealModePosByChangeAltitudeOrChangePos();

		glbInt32 ComputeNodeSize(osg::Node *node);
		/**
		* @brief 由Position获取ControlPoint
		* @param [in] globe 场景指针
		*		  [in] 控制点位置xOrlon,yOrlat,zOrAlt（球模式为经纬高，经度纬度单位为°，海拔高度单位为米；平面模式单位为米）
		*		  [in] 控制点姿态yaw,pitch,roll(单位为°)
		* @return osg::AnimationPath::ControlPoint
		*/
		osg::AnimationPath::ControlPoint PositionToControlPoint(glbDouble xOrlon,glbDouble yOrlat,glbDouble zOrAlt,glbDouble yaw,glbDouble pitch,glbDouble roll);
		/**
		* @brief 由ControlPoint获取Position
		* @param [in] globe 场景指针
		*		  [in] 	osg::AnimationPath::ControlPoint& cp
		*		  [out] 控制点位置xOrlon,yOrlat,zOrAlt（球模式为经纬高，经度纬度单位为°，海拔高度单位为米；平面模式单位为米）
		*		  [out] 控制点姿态yaw,pitch,roll(单位为°)
		*/
		void ControlPointToPosition(osg::AnimationPath::ControlPoint& cp,glbDouble* xOrlon,glbDouble* yOrlat,glbDouble* zOrAlt,glbDouble* yaw,glbDouble* pitch,glbDouble* roll);
		osg::AnimationPath::TimeControlPointMap TkpToTcp(std::map<glbDouble,KeyPoint> tkp_map);
		/**
		* @brief 将glbInt32转化为osg::vec4
		* @return osg::vec4
		*/
		inline osg::Vec4 getColorFromglbInt32(glbInt32 color)
		{
			return osg::Vec4(GetRValue(color),LOBYTE(((color & 0xFFFF)) >> 8),GetBValue(color),1.0);
		} 
		/**
		 * @brief 播放媒体文件 ---废弃!!!改用包装层C#实现媒体播放功能 
		 * @param [in] filepath
		 * @param [in] isNeedPause  是否需要暂停
		 * @return 成功返回true
					失败返回false
		 * @note 支持视频和音频文件,如avi, mpeg,wav, mp3等格式的多媒体文件
		**/
		glbBool MediaPlay(const char* filepath, glbBool isNeedPause = true);
		/**
		 * @brief 停止播放媒体文件,关闭多媒体播放窗口(视频)
		 * @return 成功返回true
					失败返回false
		**/
		glbBool MediaStop();
		/**
		 * @brief 暂停播放媒体文件
		 * @param [in] isPause 暂停还是继续
		 * @return 成功返回true
					失败返回false
		**/
		glbBool MeidaPause(glbBool isPause=true);
	private:
		HWND MakeWindow();
		void DisplayMCIErrorMsg(DWORD dwError);
	private:
		//osg::ref_ptr<osg::AnimationPath> mpr_dynamicObj_ap;
		std::vector<glbDouble> mpr_Tim;										//控制点时间的vector
		osg::AnimationPath::TimeControlPointMap mpr_tcp_map;				//控制点map
		std::map<glbDouble,KeyPoint> mpr_TKp_map;							//Time_KeyPoint map
		std::map<glbDouble,KeyPoint> mpr_KeyMeida_map;						//媒体播放控制点信息map
		glbBool mpr_isMediaPlaying;											//标志当前是否有媒体正在播放状态
		DWORD	mpr_uDeviceID;												//媒体播放设备
		//UINT	mpr_uAudioDeviceID;											//音频播放设备

		osg::ref_ptr<osg::Vec3dArray> mpr_PointsPosition;					//顶点相对坐标
		osg::Vec3d					 mpr_FirstPointPosition;				//动态对象顶点第一个坐标(世界坐标)
		osg::ref_ptr<osg::Vec4Array> mpr_PointsColor;						//顶点颜色
		glbBool mpr_isRunning;												//该对象是否激活
		glbBool mpr_isshowpoints;											//是否显示控制点
		glbBool mpr_isshowpath;												//是否显示路径
		//glbBool mpr_isshowDynamicObj;										//是否显示动态对象模型
		osg::ref_ptr<CGlbGlobeDynamicObjCallback> mpr_DynamicObjCallback;	//节点回调	

		//CGlbWString	mpr_fliepath;											//动态对象模型的文件名称（三维模型和图像注记时，需载入文件名称）

		glbBool mpr_isDirty;												//动态对象是否改变了
		//osg::ref_ptr<osg::Switch> mpr_switch;								//动态对象组节点
		glbDouble mpr_distance;												//对象与相机距离
		glbDouble mpr_elevation;				//当前地形高

		glbref_ptr<IGlbGeometry> mpr_outline;	///<对象的2D轮廓
		glbInt32 mpr_objSize;					///<临时测试，obj的size
		glbref_ptr<GlbRenderInfo> mpr_renderInfo;			///<对象渲染信息

		GlbCriticalSection mpr_readData_critical;//创建节点临界区
		GlbCriticalSection mpr_addToScene_critical;//节点添加场景临界区
		GlbCriticalSection mpr_stop_critical;
		glbBool                   mpr_needReReadData;		///<重新读取数据标志
		osg::ref_ptr<osg::Group>  mpr_editNode;				///<编辑对象.

		bool mpr_isFirstAddPosition;	///<标识第一次添加运动点
		HWND					  mpr_hWnd;					///<视频窗口句柄
	protected:
		glbDouble mpr_xOrLon;					///<经度（单位：度）或x坐标
		glbDouble mpr_yOrLat;					///<纬度（单位：度）或y坐标
		glbDouble mpr_zOrAlt;					///<高程坐标(绝对)		

		/// <附加的三个角度
		glbDouble mpr_additionalYaw;
		glbDouble mpr_additionalPitch;
		glbDouble mpr_additionalRoll;
	};
}
