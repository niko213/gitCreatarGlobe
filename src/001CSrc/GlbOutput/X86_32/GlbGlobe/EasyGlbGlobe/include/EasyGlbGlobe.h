/********************************************************************
  * Copyright (c) 2014 北京超维创想信息技术有限公司
  * All rights reserved.
  *
  * @file    GlbEasyGlobe.h
  * @brief   WVS（网络视图服务）的Globe场景类头文件
  *
  * 这个档案定义CGlbEasyGlobe这个class,
  *
  * @version 1.0
  * @author  马林
  * @date    2016-5-19 13:40
*********************************************************************/
// EasyGlbGlobe.h
#pragma once
#include "GlbCommTypes.h"
#include "GlbReference.h"
#include "GlbCriticalSection.h"
#include "glbref_ptr.h"
#include "GlbWString.h"
#include "EasyGlbGlobeTypes.h"
#include "EasyGlbGlobeObject.h"
#include "EasyGlbGlobeLayer.h"
#include "osg/Switch"

namespace GlbGlobe
{
class GLB_DLLCLASS_EXPORT CEasyGlbGlobe : public CGlbReference
{
public:
	 /**
	  * @brief CGlbGlobe构造函数
	  * @param [in] globeType 场景类型
	  *			-	GLB_GLOBETYPE_UNKNOWN	未知类型
	  *			-	GLB_GLOBETYPE_GLOBE		球类型
	  *			-	GLB_GLOBETYPE_FLAT		平面类型
	  * @return 无
	  */
	CEasyGlbGlobe(GlbGlobeTypeEnum globeType);
	~CEasyGlbGlobe(void);

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
	glbBool AddObject(CEasyGlbGlobeObject* obj,glbBool needsetId=true);

	 /**
	  * @brief 删除对象
	  * @param [in] id 对象的id号
	  * @return 成功返回true
	  *		-	失败返回false
	  */
	glbBool RemoveObject(glbInt32 id);
	// 清空所有对象
	void RemoveAllObjects();

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
	CEasyGlbGlobeObject* GetObjectByIndex(glbInt32 idx);

	 /**
	  * @brief 根据对象id号获取对象
	  * @param [in] id  对象id号	  
	  *
	  * @return 对应id号的对象指针     
	  *   -  成功则返回idx索引位置的对象指针
	  *	  -  失败返回 NULL
	  */
	CEasyGlbGlobeObject* GetObjectById(glbInt32 id);

	/* 图层相关接口 */
	 /**
	  * @brief 更新对象
	  * @param [in] obj  对象指针	  
	  *	
	  * @return 无     
	  */
	glbBool AddLayer(CEasyGlbGlobeLayer* layer);

	 /**
	  * @brief 删除图层
	  * @param [in] id  图层的id号	  
	  *
	  * @return 成功返回true
	  *		-	失败返回false    
	  */
	glbBool RemoveLayer(glbInt32 id);

	// 删除所有图层
	glbBool RemoveAllLayers();

	 /**
	  * @brief 根据位置索引号获取图层
	  * @param [in] layer_idx  图层的位置索引号[0....]	  
	  *
	  * @return 成功返回图层指针
	  *		-	失败返回NULL    
	  */
	CEasyGlbGlobeLayer* GetLayer(glbInt32 layer_idx);

	 /**
	  * @brief 根据图层的id号获取图层
	  * @param [in] layer_id  图层id号	  
	  *
	  * @return 成功返回图层指针
	  *		-	失败返回NULL    
	  */
	CEasyGlbGlobeLayer* GetLayerById(glbInt32 layer_id);

	 /**
	  * @brief 获取场景中图层数量
	  *
	  * @return 图层数量
	  */	
	glbInt32 GetLayerCount();

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
	void AddDomDirtyExtent(CGlbExtent &ext,glbBool isDomLayerDirty = false);

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

	/* 地形相关接口 */
	 /**
	  * @brief 获取地形对象 
	  * @return 成功返回 地形对象指针
	  *		-	失败返回NULL    
	  */
	//CGlbGlobeTerrain* GetTerrain();
	 /**
	  * @brief 设置地形数据集  
	  * @param [in] dataset  地形数据集
	  * @return 成功返回true
	  *		-	失败返回false    
	  */
	//glbBool SetTerrainDataset(IGlbTerrainDataset* dataset);
	 /**
	  * @brief 获取地形数据集  
	  * @return 成功返回 地形数据集指针
	  *		-	失败返回NULL    
	  */
	//IGlbTerrainDataset* GetTerrainDataset();

	 /**
	  * @brief 设置地形不透明度  
	  * @param [in] opacity  不透明度[0-100]
	  *			-			0 表示全透明
	  *			-			100 表示不透明
	  * @return 成功返回true
	  *		-	失败返回false    
	  */
	//glbBool SetTerrainOpacity(glbInt32 opacity);

	 /**
	  * @brief 获取地形的不透明度   
	  * 
	  * @return 地形的不透明度值[0-100]
	  *			-			0 表示全透明
	  *			-			100 表示不透明
	  */
	//glbInt32 GetTerrainOpacity();

	 /**
	  * @brief 设置地形 显隐   
	  * @param [in] visible 显隐 : 
	  *          -           true为显示
	  *			 -			false为隐藏	 
	  * @return 成功返回true
	  *			失败返回false    
	  */
	//glbBool SetTerrainVisbile(glbBool visible);

	 /**
	  * @brief 获取地形显隐   
	  *
	  * @return 显示返回true
	  *		-	隐藏返回false    
	  */
	//glbBool IsTerrainVisbile();

	 /**
	  * @brief 设置地形 夸张系数   
	  * @param [in] exaggrate 夸张系数 
	  *     -       1.0 表示按实际高度渲染
	  *		-		< 1.0 表示高度进行了缩小
	  *		-		> 1.0 表示高度进行了放大
	  * @return 成功返回true
	  *		-	失败返回false    
	  */
	//glbBool SetTerrainExaggrate(glbDouble exaggrate);

	 /**
	  * @brief 获取地形 夸张系数   
	  *             
	  * @return  夸张系数 
	  */
	//glbDouble  GetTerrainExaggrate();

	/* 地下模式相关接口 */
	 /**
	  * @brief 设置地下模式开启或关闭   
	  * @param [in] mode 地下模式开启或关闭 : 
	  *         -            true为开启地下模式
	  *			-			false为关闭地下模式	 
	  * @return 成功返回true
	  *		-	失败返回false    
	  */
	//glbBool SetUnderGroundMode(glbBool mode);

	 /**
	  * @brief 获取当前是否地下模式是否开启   
	  *
	  * @return 地下模式开启返回true
	  *		-	地下模式关闭返回false    
	  */
	//glbBool IsUnderGroundMode();

	 /**
	  * @brief 设置地下参考面的海拔高度   
	  * @param [in] altitude 海拔高
	  *
	  * @return 成功返回true
	  *		-	失败返回false    
	  */
	//glbBool SetUnderGroundAltitude(glbDouble altitude);

	 /**
	  * @brief 获取地下参考面的海拔高度
	  *
	  * @return 地下参考面海拔高度   
	  */
	//glbDouble GetUnderGroundAltitude();

	 /**
	  * @brief 场景初始化 
	  * 
	  * @return 初始化成功返回true
		-		失败返回false
	  */
	//glbBool	Initialize();

	/** 序列化 */
	 /**
	  * @brief 打开工程文件   
	  * @param [in] prjFile 工程文件全路径名称
	  *			
	  * @return 成功返回true
	  *		-	失败返回false    
	  */
	//glbBool Open(const glbWChar* prjFile);

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
	//glbBool ScreenToTerrainCoordinate(glbDouble x,glbDouble y,glbDouble&  lonOrX, glbDouble& latOrY, glbDouble& altOrZ); 

	 /**
	  * @brief 计算射线是否与地形相交      [都是世界坐标系]
	  *	@param [in] ln_pt1射线上一点(端点)
	  * @param [in] ln_dir射线方向
	  * @param [out] InterPos 如果相交，交点坐标	
	  * @return 相交返回true
	  *		-	不相交返回false
	  */
	//glbBool IsRayInterTerrain(osg::Vec3d ln_pt1, osg::Vec3d ln_dir, osg::Vec3d &InterPos);

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

private:
	std::map<glbInt32/*id*/,glbref_ptr<CEasyGlbGlobeObject>> mpr_objects;	    //对象集合
	std::vector<glbInt32>                                mpr_objids;	    //对象顺序
	glbInt32	                                         mpr_objectid_base;	//对象id起始值 默认为0
	std::vector<glbref_ptr<CEasyGlbGlobeLayer>>          mpr_layers;		//图层集合
	glbInt32	                                         mpr_layerid_base;	//图层id起始值 默认为0
	//std::vector<GlbGroup>                              mpr_groups;		//分组集合
	//glbInt32	                                         mpr_groupid_base;	//组id起始值，默认为0
	std::vector<CGlbExtent> mpr_objDirtyExts;							//贴地形对象刷新区域
	std::vector<CGlbExtent> mpr_domDirtyExts;							//Dom图层刷新区域
	std::vector<CGlbExtent> mpr_mtobjDirtyExts;							//地形修改对象刷新区域
	std::vector<CGlbExtent> mpr_demDirtyExts;							//Dem图层刷新区域

	GlbCriticalSection	                                 mpr_critical;		//对象、图层 修改操作 临界区
	osg::ref_ptr<osg::Switch>	                         mpr_root;			//场景根节点
	osg::ref_ptr<osg::Switch>							 mpr_p_objects_groundnode;		//地面上对象osg节点
	osg::ref_ptr<osg::Switch>                            mpr_p_objects_undergroundnode;	//地面下对象osg节点
	//glbref_ptr<CGlbGlobeTerrain>  mpr_terrain;							//地形对象
	//glbref_ptr<CGlbGlobeUGPlane>  mpr_ugplane;							//地下参考面
	CGlbWString                   mpr_name;                             //场景名称           
	GlbGlobeTypeEnum			  mpr_type;								//场景类型
	glbBool		mpr_undergroundmode;									//是否是地下模式
	glbDouble	mpr_undergroundaltitude;								//地下参考面海拔高
	CGlbWString	mpr_prjFile;											//工程文件
	glbBool		mpr_is_initialized;										//场景是否初始化
};
	
}
