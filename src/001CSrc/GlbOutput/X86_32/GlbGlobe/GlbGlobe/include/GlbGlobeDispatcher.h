/********************************************************************
  * Copyright (c) 2014 北京超维创想信息技术有限公司
  * All rights reserved.
  *
  * @file    GlbGlobeDispatcher.h
  * @brief   GlbGlobeDispatcher调度类头文件
  *
  * 这个档案定义GlbGlobeDispatcher这个class,
  *
  * @version 1.0
  * @author  于勇
  * @date    2014-5-15 10:20
*********************************************************************/
#pragma once

#include "GlbCommTypes.h"
#include "glbref_ptr.h"
#include <osg/Node>
#include <osg/Timer>
#include "GlbExtent.h"

namespace GlbGlobe
{
class CGlbGlobe;
class CGlbGlobeRObject;
class CGlbGlobeDispatcher
{
public:
	 /**
	  * @brief GlbGlobeDispatcher构造函数
	  * @param 无
	  * @return 无
	  */
	CGlbGlobeDispatcher(CGlbGlobe* globe);

	 /**
	  * @brief GlbGlobeDispatcher析构函数
	  * @param 无
	  * @return 无
	  */
	~CGlbGlobeDispatcher(void);
	
	 /**
	  * @brief 更新对象
	  * @param [in] obj对象数组地址	  
	  * @return 无     
	  */ 
	void Update(std::vector<glbref_ptr<CGlbGlobeObject>>& objs);	
	 /**
	  * @brief 返回与地形相关的对象
	  * @param [out] tobj 贴地形对象数组地址	  
	  * @param [out] robj 相对形对象数组地址
	  * @param [out] mobj 修改地形对象数组地址
	  * @return 无     
	  */ 
	void GetTerrainObjects(std::vector<glbref_ptr<CGlbGlobeRObject>> &tobjs,
		                   std::vector<glbref_ptr<CGlbGlobeRObject>> &robjs,
						   std::vector<glbref_ptr<CGlbGlobeRObject>> &mobjs);

	 /**
	  * @brief 返回与地形相关的对象及“脏”区域集合
	  * @param [out] tobj 贴地形对象数组地址	  
	  * @param [out] robj 相对形对象数组地址
	  * @param [out] mobj 修改地形对象数组地址
	  * @param [out] objDirtyExts 贴地形对象"脏"区域集合	  
	  * @param [out] domlayerDirtyExts dom图层"脏"区域集合		 
	  * @param [out] mtobjDirtyExts 地面修改对象"脏"区域集合	  
	  * @param [out] demlayerDirtyExts dem图层"脏"区域集合		 
	  * @return 无     
	  */ 
	void GetTerrainObjectsAndDirtyExts(std::vector<glbref_ptr<CGlbGlobeRObject>> &tobjs,
										std::vector<glbref_ptr<CGlbGlobeRObject>> &robjs,
										std::vector<glbref_ptr<CGlbGlobeRObject>> &mobjs,
										std::vector<CGlbExtent>	&objDirtyExts,
										std::vector<CGlbExtent>	&domDirtyExts,
										std::vector<CGlbExtent>	&mtobjDirtyExts,
										std::vector<CGlbExtent>	&demDirtyExts);
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

	void UpdateState();
private:
	/**
	 *  @brief 内存不够，卸载对象
	 *  @return 返回卸载对象后空出来的内存
	 */
	glbInt64 UnLoadObject(CGlbGlobeRObject* obj);
	/**
	 * @brief 显存不够，从场景树上摘除对象osg节点
	 * @return 返回 摘除的osg节点的大小
	*/
	glbInt64 RemoveObjFromScene(CGlbGlobeRObject* obj);
	/**
	 * @brief 销毁删除的对象
	*/
	void DestoryObjects();
private:
	CGlbGlobe*														mpr_globe;
	std::map<glbInt32, std::vector<glbref_ptr<CGlbGlobeObject>>>    mpr_objlist;			//可见列表。注意：order越小，优先级越高；距离越小，优先级越高
	std::vector<glbref_ptr<CGlbGlobeRObject>>						mpr_terrainObjs;		//贴地形绘制对象
	std::vector<glbref_ptr<CGlbGlobeRObject>>						mpr_rTerrainObjs;		//相对地形绘制对象
	std::vector<glbref_ptr<CGlbGlobeRObject>>						mpr_mTerrainObjs;		//地形修改对象
	GlbCriticalSection                                              mpr_critical;

	std::vector<CGlbExtent>											mpr_objDirtyExts;		//贴地形对象刷新区域
	std::vector<CGlbExtent>											mpr_domDirtyExts;		//Dom图层刷新区域
	std::vector<CGlbExtent>											mpr_mtobjDirtyExts;		//地形修改对象刷新区域
	std::vector<CGlbExtent>											mpr_demDirtyExts;		//Dem图层刷新区域

	std::vector<CGlbExtent>											mpr_tempObjDirtyExts;	//贴地形对象刷新区域
	std::vector<CGlbExtent>											mpr_tempDomDirtyExts;	//Dom图层刷新区域
	std::vector<CGlbExtent>											mpr_tempMtobjDirtyExts;	//地形修改对象刷新区域
	std::vector<CGlbExtent>											mpr_tempDemDirtyExts;	//Dem图层刷新区域

	osg::Vec3d														mpr_eyePosition;
	glbDouble														mpr_pitch;
	glbDouble														mpr_yaw;
	glbUInt64														mpr_numOfObjs;	  
	glbref_ptr<CGlbGlobeRObject>                                    mpr_lastLoadObj;         //视点、数量没变化情况下最后一次装载的对象	
	glbref_ptr<CGlbGlobeRObject>                                    mpr_lastRemoveObj;      //最后一次卸载的可见区域对象  
	osg::Timer_t                                                    mpr_lastRemoveTime;     //最后一次卸载的可见区域对象的时间

    glbBool                                                         mpr_needTerminateUpdate;
	osg::Timer_t                                                    mpr_preUpateTime;

	void quickSort(std::vector<glbref_ptr<CGlbGlobeObject>> * objs, glbInt32 l, glbInt32 r);
	//1：A优先级高于B；
	//0：A优先级等于B；
	//-1：A优先级低于B
	glbInt8 compareTo(CGlbGlobeRObject *objA, CGlbGlobeRObject *objB, glbBool isComputeDistance = false);   
	//将可见列表objs合并到已有的列表objmap中。
	void combineTwoList(std::map<glbInt32, std::vector<glbref_ptr<CGlbGlobeObject>>>& objmap, vector<glbref_ptr<CGlbGlobeObject>> &objs);
	//获取剩余内存的数量
	//glbInt64 GetUnusedMemory();
	//决策loaddata
	void LoadData();
	//决策挂场景树
	void AddToScene();
	//大顶-堆排序算法,输出：距离从小到大.
	void bigHeapAdjust(std::vector<glbref_ptr<CGlbGlobeObject>>& objs,glbInt32 r,glbInt32 len);
	void bigHeapSort(std::vector<glbref_ptr<CGlbGlobeObject>>& objs);
};
}