/********************************************************************
  * Copyright (c) 2014 北京超维创想信息技术有限公司
  * All rights reserved.
  *
  * @file    CGlbGlobe.h
  * @brief   Globe场景类头文件
  *
  * 这个档案定义CGlbGlobe这个class,
  *
  * @version 1.0
  * @author  马林
  * @date    2014-5-5 10:40
*********************************************************************/
#pragma once

#include "GlbGlobeExport.h"
#include "GlbCommTypes.h"
#include "GlbWString.h"
#include "GlbGlobeTypes.h"
//#include "GlbDataSources.h"
#include "GlbGlobeObject.h"
#include "GlbGlobeLayer.h"
#include "GlbGlobeSceneObjIdxManager.h"
#include "GlbGlobeTaskManager.h"
#include "GlbGlobeCallBack.h"
#include "GlbGlobeTerrain.h"
#include "GlbGlobeUGPlane.h"
#include "GlbGlobeSettings.h"
#include "GlbGlobeLegend.h"

namespace GlbGlobe
{
class CGlbGlobeView;
class CGlbGlobeDispatcher;
//class CGlbGlobeSettings;
class GLB_DLLCLASS_EXPORT  CGlbGlobe : public CGlbReference
{
	friend class CGlbGlobeView;
	friend class CGlbGlobeDispatcher;
private:
	/** 组结构  */
	typedef struct _GlbGroup 
	{
		long id;			///<组id
		long parentId;		///<父组id -1表示没有父亲组
		CGlbWString name;	///<组名
		bool visible;		///<显示或隐藏		
	}GlbGroup;

	/**
	* @brief CGlbGlobeFLUpdateThread 场景类的私有类 （绝对高程模式）要素图层更新类
	* @version 1.0
	* @author  敖建
	* @date    2017-2-10 14:29
	*/
	class CGlbGlobeFLUpdateThread : public OpenThreads::Thread, public osg::Referenced
	{
	public:
		CGlbGlobeFLUpdateThread(CGlbGlobe* globe)
		{
			mpr_globe    = globe;
			mpr_done     = false;
			mpr_pause    = false;
			mpr_ispaused = false;
		}
		~CGlbGlobeFLUpdateThread()
		{		
			mpr_globe = NULL;								
		}
		void run();
		void pause();
		void resume();
		int  cancel();
	private:
		CGlbGlobe* mpr_globe;
		glbBool mpr_done;
		glbBool mpr_pause;
		glbBool mpr_ispaused;
	};
public:
	 /**
	  * @brief CGlbGlobe构造函数
	  * @param [in] globeType 场景类型
	  *			-	GLB_GLOBETYPE_UNKNOWN	未知类型
	  *			-	GLB_GLOBETYPE_GLOBE		球类型
	  *			-	GLB_GLOBETYPE_FLAT		平面类型
	  * @return 无
	  */
	CGlbGlobe(GlbGlobeTypeEnum globeType);

	 /**
	  * @brief CGlbGlobe析构函数
	  * @param 无
	  * @return 无
	  */
	~CGlbGlobe(void);
	/**
	  * @brief 获取场景名称

	  * @return	场景名称
	  */
	const glbWChar* GetName();
	/**
	  * @brief 设置场景名称

	  * @return	场景名称
	  */
	void SetName(const glbWChar* name);
	 /**
	  * @brief 获取场景类型

	  * @return		GLB_GLOBETYPE_GLOBE		球类型
	  			-	GLB_GLOBETYPE_FLAT		平面类型
	  			-	GLB_GLOBETYPE_UNKNOWN	未知类型
	  */
	GlbGlobeTypeEnum GetType();

	/* 对象相关接口 */
	 /**
	  * @brief 添加对象
	  * @param [in] obj 对象
	  * @return 成功返回true
	  *		-	失败返回false
	  */
	glbBool AddObject(CGlbGlobeObject* obj,glbBool needsetId=true);

	 /**
	  * @brief 删除对象
	  * @param [in] id 对象的id号
	  * @return 成功返回true
	  *		-	失败返回false
	  */
	glbBool RemoveObject(glbInt32 id);

	 /**
	  * @brief 移动对象位置
	  * @param [in] src_id  要移动的源对象id号
	  * @param [in] target_id  目标对象id号
	  *		-	将id号为src_id的对象移动到id号为taget_id的对象前面
	  * @return 成功返回true
	  *		-	失败返回false
	  */
	glbBool MoveObject(glbInt32 src_id,glbInt32 target_id);

	 /**
	  * @brief 获取场景中的对象数量
	  *
	  * @return 对象数量
	  */
	glbInt32 GetObjectCount();

	 /**
	  * @brief 根据索引位置获取对象
	  * @param [in] idx  索引位置号	  
	  *
	  * @return idx索引位置的对象指针   
	  *   -  成功则返回idx索引位置的对象指针
	  *	  -  失败返回 NULL
	  */
	CGlbGlobeObject* GetObjectByIndex(glbInt32 idx);

	 /**
	  * @brief 根据对象id号获取对象
	  * @param [in] id  对象id号	  
	  *
	  * @return 对应id号的对象指针     
	  *   -  成功则返回idx索引位置的对象指针
	  *	  -  失败返回 NULL
	  */
	CGlbGlobeObject* GetObjectById(glbInt32 id);

	 /**
	  * @brief 更新对象
	  * @param [in] obj  对象指针	  
	  *   -	 当对象位置，范围发生变化后，调用此函数
	  *	
	  * @return 无     
	  */
	void UpdateObject(CGlbGlobeObject* obj);

	/* 图层相关接口 */
	 /**
	  * @brief 更新对象
	  * @param [in] obj  对象指针	  
	  *	
	  * @return 无     
	  */
	glbBool AddLayer(CGlbGlobeLayer* layer);

	 /**
	  * @brief 删除图层
	  * @param [in] id  图层的id号	  
	  *
	  * @return 成功返回true
	  *		-	失败返回false    
	  */
	glbBool RemoveLayer(glbInt32 id);

	 /**
	  * @brief 移动图层位置
	  * @param [in] src_id   要移动的源图层id号
	  * @param [in] target_id  目标图层id号
	  *		-	将id号为src_id的图层移动到id号为target_id的图层前面
	  * @return 成功返回true
	  *		-	失败返回false
	  */
	glbBool MoveLayer(glbInt32 src_id, glbInt32 target_id);

	 /**
	  * @brief 根据位置索引号获取图层
	  * @param [in] layer_idx  图层的位置索引号[0....]	  
	  *
	  * @return 成功返回图层指针
	  *		-	失败返回NULL    
	  */
	CGlbGlobeLayer* GetLayer(glbInt32 layer_idx);

	 /**
	  * @brief 根据图层的id号获取图层
	  * @param [in] layer_id  图层id号	  
	  *
	  * @return 成功返回图层指针
	  *		-	失败返回NULL    
	  */
	CGlbGlobeLayer* GetLayerById(glbInt32 layer_id);

	 /**
	  * @brief 获取场景中图层数量
	  *
	  * @return 图层数量
	  */	
	glbInt32 GetLayerCount();

	/* 分组相关接口 */
	 /**
	  * @brief 创建一个组
	  * @param [in] parent_grpid  所属父组的id号	  
	  * @param [in] group_name	 组名
	  * @return 新建组id号
	  */
	glbInt32 CreateGroup(glbInt32 parent_grpid, glbWChar* group_name);  // 返回分组id

	 /**
	  * @brief 删除一个组
	  * @param [in] grp_id  组的id号	  
	  *
	  * @return 成功返回true
	  *			失败返回false    
	  */
	glbBool RemoveGroup(glbInt32 grp_id);

	 /**
	  * @brief 移动图层位置
	  * @param [in] src_id   要移动的源图层id号
	  * @param [in] target_id  目标图层id号
	  *		-	将id号为src_id的图层移动到id号为target_id的图层前面
	  * @return 成功返回true
	  *		-	失败返回false
	  */
	glbBool MoveGroup(glbInt32 src_id, glbInt32 target_id);

	 /**
	  * @brief 获取场景中组的数量
	  *
	  * @return 组数量
	  */	
	glbInt32 GetGroupCount();

	 /**
	  * @brief 根据位置索引号获取组信息
	  * @param [in] grp_idx  组的位置索引号[0....]	  
	  * @param [out] grp_id  组的id号
	  * @param [out] grp_name 组名
	  * @param [out] parent_id  组的父组id号
	  * @return 成功返回true
	  *		-	失败返回false    
	  */
	glbBool GetGroup(glbInt32 grp_idx, glbInt32* grp_id, const glbWChar** grp_name,  glbInt32* parent_id );

	 /**
	  * @brief 根据组id获取组信息	   
	  * @param [in] grp_id  组的id号
	  * @param [out] grp_name 组名
	  * @param [out] parent_id  组的父组id号
	  * @return 成功返回true
	  *		-	失败返回false    
	  */
	glbBool GetGroupById(glbInt32 grp_id, const glbWChar** grp_name,  glbInt32* parent_id);

	 /**
	  * @brief 设置组的名称   
	  * @param [in] grp_id  组的id号
	  * @param [in] grp_name 组名	 
	  * @return 成功返回true
	  *		-	失败返回false    
	  */
	glbBool SetGroupNameById(glbInt32 grp_id, glbWChar* group_name);

	 /**
	  * @brief 设置组 显隐   
	  * @param [in] grp_id  组的id号
	  * @param [in] visible 显隐 : 
	  *      -              true为显示
	  *		 -				false为隐藏	 
	  * @return 成功返回true
	  *		-	失败返回false    
	  */
	glbBool SetGroupVisible(glbInt32 grp_id , glbBool visible);

	 /**
	  * @brief 获取组的显隐属性   
	  * @param [in] grp_id  组的id号
	  * @return 显示返回true
	  *		-	隐藏返回false    
	  */
	glbBool GetGroupVisible(glbInt32 grp_id);

	 /**
	  * @brief 场景初始化 
	  * 
	  * @return 初始化成功返回true
		-		失败返回false
	  */
	glbBool	Initialize();
	/* 地形相关接口 */
	 /**
	  * @brief 获取地形对象 
	  * @return 成功返回 地形对象指针
	  *		-	失败返回NULL    
	  */
	CGlbGlobeTerrain* GetTerrain();
	 /**
	  * @brief 设置地形数据集  
	  * @param [in] dataset  地形数据集
	  * @return 成功返回true
	  *		-	失败返回false    
	  */
	glbBool SetTerrainDataset(IGlbTerrainDataset* dataset);
	 /**
	  * @brief 获取地形数据集  
	  * @return 成功返回 地形数据集指针
	  *		-	失败返回NULL    
	  */
	IGlbTerrainDataset* GetTerrainDataset();

	 /**
	  * @brief 设置地形不透明度  
	  * @param [in] opacity  不透明度[0-100]
	  *			-			0 表示全透明
	  *			-			100 表示不透明
	  * @return 成功返回true
	  *		-	失败返回false    
	  */
	glbBool SetTerrainOpacity(glbInt32 opacity);

	 /**
	  * @brief 获取地形的不透明度   
	  * 
	  * @return 地形的不透明度值[0-100]
	  *			-			0 表示全透明
	  *			-			100 表示不透明
	  */
	glbInt32 GetTerrainOpacity();

	 /**
	  * @brief 设置地形 显隐   
	  * @param [in] visible 显隐 : 
	  *          -           true为显示
	  *			 -			false为隐藏	 
	  * @return 成功返回true
	  *			失败返回false    
	  */
	glbBool SetTerrainVisbile(glbBool visible);

	 /**
	  * @brief 获取地形显隐   
	  *
	  * @return 显示返回true
	  *		-	隐藏返回false    
	  */
	glbBool IsTerrainVisbile();

	 /**
	  * @brief 设置地形 夸张系数   
	  * @param [in] exaggrate 夸张系数 
	  *     -       1.0 表示按实际高度渲染
	  *		-		< 1.0 表示高度进行了缩小
	  *		-		> 1.0 表示高度进行了放大
	  * @return 成功返回true
	  *		-	失败返回false    
	  */
	glbBool SetTerrainExaggrate(glbDouble exaggrate);

	 /**
	  * @brief 获取地形 夸张系数   
	  *             
	  * @return  夸张系数 
	  */
	glbDouble  GetTerrainExaggrate();

	 /**
	  * @brief 启动/停止 地形更新   
	  * @param [in] enable 启动/停止 : 
	  *         -           true为启动地形更新
	  *			-			false为停止地形更新	 
	  * @return 无
	  */
	void  EnableTerrainUpdate(glbBool enable);

	 /**
	  * @brief 地形更新是否启动	 	  
	  * @return  启动中返回true
			-	 停止了返回false
	  */
	glbBool IsTerrainUpdateEnable();

	 /**
	  * @brief 启动/关闭 线框模式   
	  * @param [in] wiremode 启动/关闭 : 
	  *         -           true为启动地形线框绘制模式
	  *			-			false为关闭地形线框绘制模式	 
	  * @return 无
	  */
	void EnableTerrainWireMode(glbBool wiremode);

	 /**
	  * @brief 获取地形是否使用线框绘制模式	  
	  * @return true 表示线框模式
				false 表示非线框模式
	  */
	glbBool IsTerrainWireMode();

	/* 地下模式相关接口 */
	 /**
	  * @brief 设置地下模式开启或关闭   
	  * @param [in] mode 地下模式开启或关闭 : 
	  *         -            true为开启地下模式
	  *			-			false为关闭地下模式	 
	  * @return 成功返回true
	  *		-	失败返回false    
	  */
	glbBool SetUnderGroundMode(glbBool mode);

	/**
	  * @brief 设置碰撞检测开启或关闭   
	  * @param [in] mode 碰撞检测开启或关闭 : 
	  *         -            true为开启 
	  *			-			false为关闭 	 
	  * @return 成功返回true
	  *		-	失败返回false    
	  */
	glbBool	SetCullisionMode(glbBool mode);
	/**
		* @brief 检测是否开启碰撞检测
		* @return 成功返回true
		*		-	失败返回false
	*/
	glbBool CheckCullisionModel()const;
	 /**
	  * @brief 获取当前是否地下模式是否开启   
	  *
	  * @return 地下模式开启返回true
	  *		-	地下模式关闭返回false    
	  */
	glbBool IsUnderGroundMode();

	 /**
	  * @brief 设置地下参考面的海拔高度   
	  * @param [in] altitude 海拔高
	  *
	  * @return 成功返回true
	  *		-	失败返回false    
	  */
	glbBool SetUnderGroundAltitude(glbDouble altitude);

	 /**
	  * @brief 获取地下参考面的海拔高度
	  *
	  * @return 地下参考面海拔高度   
	  */
	glbDouble GetUnderGroundAltitude();

	/** 序列化 */
	 /**
	  * @brief 打开工程文件   
	  * @param [in] prjFile 工程文件全路径名称
	  *			
	  * @return 成功返回true
	  *		-	失败返回false    
	  */
	glbBool Open(const glbWChar* prjFile);

	 /**
	  * @brief 保存当前场景到默认工程文件   
	  *			
	  * @return 无
	  */
	glbBool Save();

	 /**
	  * @brief 另存当前场景到指定的工程文件      
	  *	@param [in] newPrjFile 	要保存的工程文件全路径名
	  *
	  * @return 无
	  */
	glbBool SaveAs(glbWChar* newPrjFile);

	 /**
	  * @brief 获取当前场景默认工程文件的全路径名   
	  *			
	  * @return 工程文件的全路径名
	  */
	const glbWChar* GetPrjFile();

	 /**
	  * @brief 获取场景主视图   
	  *			
	  * @return 场景主视图
	  */
	CGlbGlobeView* GetView();

	 /**
	  * @brief 获取场景使用的参考坐标系描述   
	  *			
	  * @return 参考坐标系描述 
	  */
	const glbWChar* GetSRS();

	/* 其它 */
	 /**
	  * @brief 获取(lonOrX,latOrY)坐标位置处的高程   
	  *	@param [in] lonOrX 经度或X  参数说明:
	  		-		GLB_GLOBETYPE_GLOBE球面模式下，为经度(单位：度)
	  		-		GLB_GLOBETYPE_FLAT平面模式下，为X(单位：米)
	  *	@param [in] latOrY 纬度或Y 参数说明:
	  		-		GLB_GLOBETYPE_GLOBE球面模式下，为纬度(单位：度)
	  		-		GLB_GLOBETYPE_FLAT平面模式下，为Y(单位：米)
	  *
	  * @return 高程(单位:米)
	  */
	glbDouble GetElevationAt(glbDouble lonOrX, glbDouble latOrY);

	 /**
	  * @brief 添加地面纹理需要刷新的区域   
	  *	@param [in] ext 刷新区域 	  
	  * @param [in] isDomLayerDirty 是否是dom图层需要刷新	 
	  * @note	功能：收集需要刷新的地面区域
	  *			ext 参数说明:
	  			-	GLB_GLOBETYPE_GLOBE球面模式下，ext中的minx,maxx代表经度(单位：度)
	  			-										  miny,maxy代表纬度(单位：度)
	  			-										  minz,maxz不用
	  			-	GLB_GLOBETYPE_FLAT平面模式下，ext中的minx,maxx代表X(单位：米)
	  			-										  miny,maxy代表Y(单位：米)
	  			-										  minz,maxz不用
	  			isDomLayerDirty 参数说明:
	 			-	如果为true，表示是dom图层需要刷新
				-	如果为false，表示是贴地形绘制对象需要刷新(默认)
	  * @return 无
	  */
	//void AddDomDirtyExtent(CGlbExtent ext,glbBool isDomLayerDirty = false);
	void AddDomDirtyExtent(CGlbExtent &ext,glbBool isDomLayerDirty = false);

	 /**
	  * @brief 立即刷新地形纹理   
	  *	@param [in] objs 贴地形绘制对象集合
	  *	
	  *@note	功能：收集需要刷新的地面区域
	  *
	  * @return 无
	  */
	void  RefreshTerrainTexture(std::vector<glbref_ptr<CGlbGlobeRObject >> objs );

	 /**
	  * @brief 添加地面高程数据需要刷新的区域   
	  *	@param [in] ext 刷新区域 	  
	  * @param [in] isDemLayerDirty 是否是dem图层需要刷新	 
	  * @note	功能：收集需要刷新的地面区域
	  *			ext 参数说明:
	  			-	GLB_GLOBETYPE_GLOBE球面模式下，ext中的minx,maxx代表经度(单位：度)
	  			-										  miny,maxy代表纬度(单位：度)
	  			-										  minz,maxz不用
	  			-	GLB_GLOBETYPE_FLAT平面模式下，ext中的minx,maxx代表X(单位：米)
	  			-										  miny,maxy代表Y(单位：米)
	  			-										  minz,maxz不用
	  			isDemLayerDirty 参数说明:
	 			-	如果为true，表示是dem图层需要刷新
				-	如果为false，表示是地形修改对象需要刷新(默认)
	  * @return 无
	  */
	void AddDemDirtyExtent(CGlbExtent &ext,glbBool  isDemLayerDirty = false);
	 /**
	  * @brief 立即刷新地形的高度  
	  *	@param [in] mtobjs 地形修改对象集合
	  *	
	  *@note	功能：收集需要刷新的地面区域
	  *
	  * @return 无
	  */
	void RefreshTerrainDem(std::vector<glbref_ptr<CGlbGlobeRObject >> mtobjs  );

	 /**
	  * @brief 设置地面对象的不透明度      
	  *	@param [in] opacity 不透明度(0-100)
	  *			-	0 为全透明 
	  *			-	100为不透明
	  *
	  * @return 成功返回true
	  *		-	失败返回false
	  */
	glbBool SetGroundObjectsOpacity(glbInt32 opacity);

	 /**
	  * @brief 获取地面对象的不透明度
	  *
	  * @return 地面对象的不透明度(0-100)
	  *		-		0 为全透明 
	  *		-		100为不透明
	  */
	glbInt32 GetGroundObjectsOpacity();

	 /**
	  * @brief 设置地下对象的不透明度      
	  *	@param [in] opacity 不透明度(0-100)
	  *		-		0 为全透明 
	  *		-		100为不透明
	  *
	  * @return 成功返回true
	  *		-	失败返回false
	  */
	glbBool SetUnderGroundObjectsOpacity(glbInt32 opacity);

	 /**
	  * @brief 获取地下对象的不透明度
	  *
	  * @return 地下对象的不透明度(0-100)
	  *		-		0 为全透明 
	  *		-		100为不透明
	  */
	glbInt32 GetUnderGroundObjectsOpacity();

	 /**
	  * @brief 通知相机在地面上或地面下
	  * @param [in] underground 地面上或地面下. 地面上为false，地面下为true
	  * @return 无
	  */
	void NotifyCameraIsUnderground(glbBool underground);

	 /**
	  * @brief 计算相机位置的地形高程
	  *	
	  * @note 循环计算相机点处的地形高程并通知当前操控器,以纠正相机落在地面下的问题
	  *
	  * @return 无
	  */
	void ComputeElevation();

	 /**
	  * @brief 计算屏幕上某点(x,y)处的地面坐标      
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
	glbBool ScreenToTerrainCoordinate(glbDouble x,glbDouble y,glbDouble&  lonOrX, glbDouble& latOrY, glbDouble& altOrZ); 
	
	 /**
	  * @brief 计算射线是否与地形相交      [都是世界坐标系]
	  *	@param [in] ln_pt1射线上一点(端点)
	  * @param [in] ln_dir射线方向
	  * @param [out] InterPos 如果相交，交点坐标	
	  * @return 相交返回true
	  *		-	不相交返回false
	  */
	glbBool IsRayInterTerrain(osg::Vec3d ln_pt1, osg::Vec3d ln_dir, osg::Vec3d &InterPos);

	/**
	* @brief 计算屏幕上某点(x,y)处的地下参考面坐标      
	* @param [in] x 屏幕点的x坐标
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
	  * @brief 计算射线是否与地下参考面相交      [都是世界坐标系]
	  *	@param [in] ln_pt1射线上一点(端点)
	  * @param [in] ln_dir射线方向
	  * @param [out] InterPos 如果相交，交点坐标	
	  * @return 相交返回true
	  *		-	不相交返回false
	  */
	glbBool IsRayInterUGPlane(osg::Vec3d ln_pt1, osg::Vec3d ln_dir, osg::Vec3d &InterPos);

	 /**
	  * @brief 设置默认定位对象     
	  *	@param [in] lctID 定位对象id

	  * @note 设置定位对象后，启动系统时会飞向定位对象的位置，形成连续的动画效果

	  * @return 成功返回true
	  *		-	失败返回false
	  */
	glbBool SetDefaultLocation(glbInt32 lctID); 

	 /**
	  * @brief 获取默认定位对象
	  *
	  * @return 定位对象指针 
	  *			- NULL 表示没有默认的定位对象 	  
	  */
	CGlbGlobeObject* GetDefaultLocation() ;

	 /**
	  * @brief 设置工程口令     
	  *	@param [in] oldPassword 旧口令
	  * @param [in] newPassword 新口令

	  * @note 如果工程需要保密，调用此方法设置密码保护。

	  * @return 成功返回true
	  *		-	失败返回false
	  */
	glbBool SetPrjPassword(glbWChar* oldPassword,glbWChar* newPassword);

	 /**
	  * @brief 获取最后的错误信息	  
	  * @return 最后的错误信息 	  		  
	  */
	glbWChar* GetLastError();

	/**
	  * @brief 获取球配置信息
	  * @return 球配置信息
	  */
	CGlbGlobeSettings *GetGlobeSettings();

	 /**
	  * @brief 场景对象调度	  
	  * @return  	  		  
	  */
	void Dispatch();
	 /**
	  * @brief 导出obj为3ds文件
	  * @param [in] obj 对象
	  * @param [in] filePath 3ds文件全路径
	  * @return 成功返回true
	  *			- 失败返回false 	  
	  * @note 注意3ds纹理文件名称只能8个字符长
	  */
	bool Export3dsFile(CGlbGlobeObject* obj, CGlbWString filePath);
	/*
	* @brief  计算两点之间的地面距离
	* @param [in] xOrLonStart, yOrLatStart 起始点x,y或经纬度
				xOrLonEnd,yOrLatEnd 终止点x,y或经纬度
				step 计算精度(步长)
	* @return  两点之间的地面距离
	*/
	glbDouble GetOnTerrainDistance(glbDouble xOrLonStart, glbDouble yOrLatStart, glbDouble xOrLonEnd, glbDouble yOrLatEnd, glbDouble step);

	/*
	* @brief 绝对高程模式的要素图层更新	  (采用独立线程更新绝对模式的要素图层)
	* @return  	 	
	*/
	glbBool UpdateAbsoluteModeFeatureLayers();

	// 暂停/继续 所有Globe管理的线程
	glbBool Pause(glbBool ispause);

	// 停止所有Globe管理的线程
	glbBool Stop();

	// 重启所有Globe管理的线程
	glbBool Run();

	/**
	* @brief 点选
	* @param  [in] ln_pt1,ln_pt2 射线的起点pt1和射线延伸方向上的一点pt2 	
	*		  [out]	results 选中的倾斜摄影图层集合
	*		  [out] InterPoints 选中倾斜摄影图层的交点集合
	*		  [out] featureOids 如果有关联多边形数据集,选中的对象id
	* @return 有对象选中返回 true
	*		   没有对象选中返回false
	*/
	glbBool PickTiltPhotographyLayer( osg::Vec3d ln_pt1, osg::Vec3d ln_pt2, std::vector<glbref_ptr<CGlbGlobeLayer>>&results,std::vector<osg::Vec3d>&InterPoints,std::vector<glbInt32>&featureOids);
	
	 /**
	  * @brief 获取osg::Node	  
	  * @return  场景根节点	  		  
	  */
	osg::Node* GetOsgNode();

	/**
	  * @brief 获取场景裁剪管理器对象	  
	  * @return  裁剪对象  		  
	  */
	CGlbGlobeObject* GetClipManagerObject();
	/**
	  * @brief 设置图例	  
	  * @param [in] legend 图例对象
	  * @param [in] isShow 是否显示
	  */
	void SetLegend(CGlbGlobeLegend* legend, glbBool isShow=true);
	/**
	  * @brief 获取场景图例对象	  
	  * @return  图例对象  		  
	  */
	CGlbGlobeLegend* GetLegend();
	/**
	  * @brief 设置图例	显隐  	  
	  * @param [in] isShow 是否显示
	  */
	void ShowLegend(glbBool isShow);
	/**
	  * @brief 获取场景图例对象显隐状态  
	  * @return  图例对象显隐状态  		  
	  */
	glbBool IsLegendShow();
	/**
	  * @brief 将经纬高转为XYZ	 经纬单位：度
	  * @param [in] latitude 纬度 longitude 经度 height 高度
				[out] X,Y,Z 世界坐标系下的坐标
	  */	
	//inline void convertLatLongHeightToXYZ(glbDouble latitude, glbDouble longitude, glbDouble height, glbDouble& X, glbDouble& Y, glbDouble& Z)const
	//	g_ellipsmodel->convertLatLongHeightToXYZ(osg::DegreesToRadians(latitude),osg::DegreesToRadians(longitude),height,X,Y,Z);
	void convertLatLongHeightToXYZ(glbDouble latitude, glbDouble longitude, glbDouble height, glbDouble& X, glbDouble& Y, glbDouble& Z);
	/**
	  * @brief 将XYZ转为经纬高	 经纬单位：度
	  * @param [in] X,Y,Z 世界坐标系下的坐标
				[out] latitude 纬度 longitude 经度 height 高度
	  */
	void convertXYZToLatLongHeight(glbDouble X, glbDouble Y, glbDouble Z, glbDouble& latitude, glbDouble& longitude, glbDouble& height);	
	/**
	  * @brief 将经纬高转为局部坐标XYZ,参考转换的原点为（refLatitude，refLongitude，refHeight）	 经纬单位：度
	  * @param [in] latitude 纬度 longitude 经度 height 高度
				[in] refLatitude 参考原点纬度 refLongitude 参考原点经度 refHeight 参考原点高度
				[out] localX,localY,localZ 局部坐标系xyz坐标
	  */
	void convertLatLongHeightToLocalXYZFromReferenceLatLongHeight(glbDouble latitude, glbDouble longitude, glbDouble height,glbDouble refLatitude, glbDouble refLongitude, glbDouble refHeight, glbDouble& localX, glbDouble& localY, glbDouble& localZ);
	/**
	  * @brief 将经纬高转为局部坐标XYZ,参考转换的原点为世界坐标系下的点（X，Y，Z）	 经纬单位：度
	  * @param [in] latitude 纬度 longitude 经度 height 高度
				[in] refLatitude 参考原点纬度 refLongitude 参考原点经度 refHeight 参考原点高度
				[out] localX,localY,localZ 局部坐标系xyz坐标
	  */
	void convertLatLongHeightToLocalXYZFromReferenceXYZ(glbDouble latitude, glbDouble longitude, glbDouble height,glbDouble refX, glbDouble refY, glbDouble refZ, glbDouble& localX, glbDouble& localY, glbDouble& localZ);
	/**
	  * @brief 将局部坐标XYZ转为世界坐标系下坐标,参考转换的原点为（refLatitude，refLongitude，refHeight）	 经纬单位：度
	  * @param [in] localX localY localZ 局部坐标系XYZ 坐标
				[in] refLatitude 参考原点纬度 refLongitude 参考原点经度 refHeight 参考原点高度
				[out] X,Y,Z 世界坐标系下xyz坐标
	  */
	void convertLocalXYZToXYZFromReferenceLatLongHeight(glbDouble localX, glbDouble localY, glbDouble localZ, glbDouble refLatitude, glbDouble refLongitude, glbDouble refHeight, glbDouble& X, glbDouble& Y, glbDouble& Z ); 	
	/**
	  * @brief 将局部坐标XYZ转为WGS84经纬度坐标,参考转换的原点为（refLatitude，refLongitude，refHeight）	 经纬单位：度
	  * @param [in] localX localY localZ 局部坐标系XYZ 坐标
				[in] refLatitude 参考原点纬度 refLongitude 参考原点经度 refHeight 参考原点高度
				[out] latitude 纬度 longitude 经度 height 高度
	  */
	void convertLocalXYZToLatLongHeightFromReferenceLatLongHeight(glbDouble localX, glbDouble localY, glbDouble localZ, glbDouble refLatitude, glbDouble refLongitude, glbDouble refHeight, glbDouble& latitude, glbDouble& longitude, glbDouble& height );
	/**
	  * @brief 将局部坐标XYZ转为世界坐标系下坐标,参考转换的原点为世界坐标系下（X，Y，Z）	 经纬单位：度
	  * @param [in] localX localY localZ 局部坐标系XYZ 坐标
				[in] refX  refY  refZ 参考原点世界坐标系下坐标
				[out] X,Y,Z 世界坐标系下xyz坐标
	  */
	void convertLocalXYZToXYZFromReferenceXYZ(glbDouble localX, glbDouble localY, glbDouble localZ, glbDouble refX, glbDouble refY, glbDouble refZ, glbDouble& X, glbDouble& Y, glbDouble& Z );
	/**
	  * @brief 将局部坐标XYZ转为WGS84经纬高,参考转换的原点为世界坐标系下（X，Y，Z）	 经纬单位：度
	  * @param [in] localX localY localZ 局部坐标系XYZ 坐标
				[in] refX  refY  refZ 参考原点世界坐标系下坐标
				[out] latitude 纬度 longitude 经度 height 高度
	  */
	void convertLocalXYZToLatLongHeightFromReferenceXYZ(glbDouble localX, glbDouble localY, glbDouble localZ, glbDouble refX, glbDouble refY, glbDouble refZ, glbDouble& latitude, glbDouble& longitude, glbDouble& height );



public:
	glbref_ptr<CGlbGlobeUGPlane>  mpr_ugplane;							//地下参考面
private: 
	// 场景更新
	void Update();

	// 获取当前可见图层，返回true表示有可见图层，false表示没有可见图层
	glbBool GetVisibleLayers(std::vector<glbref_ptr<CGlbGlobeDomLayer>>& domlayers,
							std::vector<glbref_ptr<CGlbGlobeDemLayer>>& demlayers);

	// 根据renderorder排序贴地形绘制对象
	glbBool SortRObjsByRenderOrder(std::vector<glbref_ptr<CGlbGlobeRObject>>& rObjs);

	// 快速排序
	void quickSort(std::vector<glbref_ptr<CGlbGlobeRObject>>* objs, glbInt32 l, glbInt32 r);

	glbInt8 compareTo(CGlbGlobeRObject *objA, CGlbGlobeRObject *objB);

	glbBool OpenGlobeSaveFile(const glbWChar* prjFile);

	glbBool OpenProjectSaveFile(const glbWChar* prjFile);

	glbBool ParseProjectNodes(xmlNodePtr groupNode,glbInt32 &groupID,glbInt32 parentID,const CGlbWString rpath);

private:	
	//bool download_jpeg(char* url);
	//bool download_jpeg2(char* url);
	//void testCurlHttpTianDiTuImage();
	void testLegend();//测试图例
private:
	std::map<glbInt32/*id*/,glbref_ptr<CGlbGlobeObject>> mpr_objects;	    //对象集合
	std::vector<glbInt32>                                mpr_objids;	    //对象顺序
	glbInt32	                                         mpr_objectid_base;	//对象id起始值 默认为0
	std::vector<glbref_ptr<CGlbGlobeLayer>>              mpr_layers;		//图层集合
	glbInt32	                                         mpr_layerid_base;	//图层id起始值 默认为0
	std::vector<GlbGroup>                                mpr_groups;		//分组集合
	glbInt32	                                         mpr_groupid_base;	//组id起始值，默认为0
	GlbCriticalSection	                                 mpr_critical;		//对象、图层 修改操作 临界区
	osg::ref_ptr<osg::Switch>	                         mpr_root;			//场景根节点
	//osg::ref_ptr<osg::Switch>	mpr_p_objects_groundnode;				//地面上对象osg节点
	osg::ref_ptr<osg::Switch>                            mpr_p_objects_undergroundnode;	//地面下对象osg节点
	glbref_ptr<CGlbGlobeTerrain>  mpr_terrain;							//地形对象
	CGlbWString                   mpr_name;                             //场景名称           
	GlbGlobeTypeEnum	mpr_type;										//场景类型
	glbBool		mpr_undergroundmode;									//是否是地下模式
	glbBool		mpr_cullisionMode;										//漫游开启碰撞检测
	glbDouble	mpr_undergroundaltitude;								//地下参考面海拔高
	CGlbWString	mpr_prjFile;											//工程文件
	CGlbGlobeView*	mpr_globeview;										//场景主视图
	CGlbWString mpr_spatialreference;									//场景空间参考坐标系
	std::vector<CGlbExtent> mpr_objDirtyExts;							//贴地形对象刷新区域
	std::vector<CGlbExtent> mpr_domDirtyExts;							//Dom图层刷新区域
	std::vector<CGlbExtent> mpr_mtobjDirtyExts;							//地形修改对象刷新区域
	std::vector<CGlbExtent> mpr_demDirtyExts;							//Dem图层刷新区域
	glbInt32	mpr_undergroundobjsOpactity;							//地下对象不透明度
	glbInt32	mpr_groundobjsOpactity;									//地面对象不透明度
	glbInt32	mpr_defaultLocationID;									//工程缺省定位对象ID
	glbref_ptr<CGlbGlobeObject> mpr_defaultlocationobj;					//工程默认定位对象
	CGlbWString	mpr_prjPassword;										//工程文件访问口令
	glbBool	mpr_is_initialized;											//场景是否初始化
	CGlbGlobeDispatcher* mpr_dispatcher;                                //场景调度器

	glbref_ptr<CGlbGlobeSettings> mpr_settings;                         //配置信息

	std::vector<CGlbExtent> mpr_visibleExt;								//可见区域
	GlbCriticalSection		mpr_updateAndDispatchCritical;				//更新与调度临界区	
public:
	glbref_ptr<CGlbGlobeSceneObjIdxManager> mpr_sceneobjIdxManager;		//场景对象索引管理器
	osg::ref_ptr<CGlbGlobeCallBack> mpr_p_callback;						//场景节点修改回调
	glbref_ptr<CGlbGlobeTaskManager> mpr_taskmanager;					//任务管理器

	glbBool				mpr_TerrainNodeUpdate;							// 标志地形节点是否正要进行场景树的挂摘动作

	osg::ref_ptr<osg::Switch>	mpr_p_objects_groundnode;				//地面上对象osg节点
	osg::ref_ptr<osg::Switch> mpr_p_objects_waternode;                  //水效对象osg节点

	osg::Vec4f											mpr_backColor;
	osg::Vec4f											mpr_undergroundBackColor;
private:
	osg::ref_ptr<CGlbGlobeFLUpdateThread>				mpr_pAbsolubeFLthread;  //绝对高程模式要素图层更新类
	glbref_ptr<CGlbGlobeObject>							mpr_clipManagerObject;	//场景裁剪管理器对象

	glbref_ptr<CGlbGlobeLegend>							mpr_pLegend;	//图例数组
};

}