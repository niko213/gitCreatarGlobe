/********************************************************************
* Copyright (c) 2014 北京超维创想信息技术有限公司
* All rights reserved.
*
* @file    GlbGlobeSceneObjIdxManager.h
* @brief   Globe场景对象索引管理类头文件
*
* 这个档案定义CGlbGlobeSceneObjIdxManager这个class,包括八叉树和四叉树索引机制，分别用来管理空间对象和贴地形绘制的对象
*	功能业务说明：1. 场景objects管理器，包含“离散八叉树”数组+“离散四叉树”数组，八叉树
*					用来管理按空间绝对位置绘制的场景可绘制对象RenderObject，四叉树则负责管理按
*					贴地形模式绘制的场景可绘制对象RenderObject。
*				  2. 快速的对象选择机制。支持点选和框选
* @version 1.0
* @author  马林
* @date    2014-5-13 11:20
*********************************************************************/
#pragma once
#include <osg/Polytope>
#include "GlbExtent.h"
#include "glbref_ptr.h"
#include "GlbGlobeRObject.h"

namespace GlbGlobe
{
	class CGlbAxisAlignedBox;
	class CGlbGlobeOctree;
	class CGlbGlobeQtree;
	/**
	* @brief Globe场景对象索引管理类			 
	* @version 1.0
	* @author  马林
	* @date    2014-5-13 11:20
	*/
	class GLB_DLLCLASS_EXPORT CGlbGlobeSceneObjIdxManager : public CGlbReference
	{
	public:
		/**
		* @brief 构造函数
		* @param [in] name 场景对象索引管理器名称
		* @param [in] max_depth 最大树深度，即最大层级数 （默认为20级）
		* @param [in] bound 初始范围(平面场景模式下需要设置)
		* @return  无
		*/
		CGlbGlobeSceneObjIdxManager(const WCHAR* name=NULL, int max_depth=20, CGlbExtent* bound=NULL);
		/**
		* @brief 析构函数
		* @return  无
		*/
		~CGlbGlobeSceneObjIdxManager(void);
	public:
		/**
		* @brief 从场景管理器中添加对象
		* @param [in] obj 可绘制对象 
		* @return  无
		*/
		void AddObject(CGlbGlobeRObject* obj);
		/**
		* @brief 从场景管理器中删除对象
		* @param [in] obj 可绘制对象 
		* @return  无
		*/
		void RemoveObject(CGlbGlobeRObject* obj);	
		/**
		* @brief 更新场景管理器中对象
		* @param [in] obj 可绘制对象 
		* @return  无
		*/
		void UpdateObject(CGlbGlobeRObject* obj); 
		/**
		* @brief 射线选择-非贴地面绘制对象
		* @param [in] ln_pt1,ln_pt2 射线的起点pt1和射线延伸方向上的一点pt2 
		* @param [out] result 选中的对象集合,按距离pt1点由近得远排列
		* @param [out] inter_pts 交点集合，按距离pt1点由近得远排列 ， 一个对象取一个交点
		* @return  有选中对象返回true，没有选中对象返回false
		*/
		bool Query(osg::Vec3d ln_pt1, osg::Vec3d ln_pt2, std::vector<glbref_ptr<CGlbGlobeObject>>& result, std::vector<osg::Vec3d>& inter_pts,bool query_crudeAndquickly=false/*粗选*/);
		/**
		* @brief 射线选择-贴地面绘制对象
		* @param [in] on_terrain_point 射线的与地面的交点（地理坐标 ： 经度，纬度） 
		* @param [out] result 选中的贴地面绘制对象集合
		* @param [in] nearest_distance 距离阀值 ，离on_terrain_point得距离小于此阀值的被选中
		* @param [in] globeTye 场景类型
		* @return  有选中对象返回true，没有选中对象返回false
		*/
		bool QueryOnTerrainObjects(osg::Vec3d on_terrain_point, std::vector<glbref_ptr<CGlbGlobeObject>>& result, double nearest_distance=0.1, GlbGlobeTypeEnum globeTye=GLB_GLOBETYPE_GLOBE);

		/**
		* @brief 视锥选择-非贴地面绘制对象
		* @param [in] polytope 视锥体 
		* @param [out] result 选中的对象集合
		* @param [in] query_crudeAndquickly 是否使用快速粗选方式 true则只用对象外包盒与视锥体求交，false则精细求交
		* @return 有选中对象返回true，没有选中对象返回false
		*/
		// 视锥选择-绝对坐标物体
		bool Query(osg::Polytope& polytope,std::vector<glbref_ptr<CGlbGlobeObject>>& result,bool query_crudeAndquickly=false);

		/**
		* @brief 矩形区域选择-贴地面绘制对象
		* @param [in] t_extent 选择区域（地理坐标 ：经度，纬度） 
		* @param [out] result 选中的贴地面绘制对象集合
		* @return  有选中对象返回true，没有选中对象返回false
		*/
		bool QueryOnTerrainObjects(CGlbExtent* t_extent,std::vector<glbref_ptr<CGlbGlobeObject>>& result);

		/**
		* @brief 矩形区域选择-非贴地面绘制对象
		* @param [in] t_extent 选择区域（世界坐标） 
		* @param [out] result 选中的非贴地面绘制对象集合
		* @return  有选中对象返回true，没有选中对象返回false
		*/
		bool QueryObjects(CGlbExtent* t_extent,std::vector<glbref_ptr<CGlbGlobeObject>>& result);

		/**
		* @brief 获取管理器中贴地形绘制对象的数量
		* @return  贴地形绘制对象的数量
		*/	
		glbInt32 GetOnTerrainObjectCount();
		/**
		* @brief 获取管理器中非贴地形绘制对象的数量
		* @return  贴地形绘制对象的数量
		*/	
		glbInt32 GetNoOnTerrainObjectCount();

	private:
		// 根据经纬度计算根octree的key, latitude,longitude : 单位:度 
		long GetKeyFromPosition(long latitude, long longitude);		
		CGlbAxisAlignedBox GetRootOctreeBoundBoxFromPosition(long col, long row, BOOL is_on_terrain=FALSE);
		void _addOctreeObject(CGlbGlobeObject* n,CGlbGlobeOctree* octant,long depth=0);
		void _removeObject(CGlbGlobeObject * n ,  BOOL is_on_terrain = FALSE);		
		void _addQtreeObject(CGlbGlobeObject* n,CGlbGlobeQtree* qtant,long depth=0);
		//void _removeQtreeObject(CGlbGlobeObject * n );
		bool _isObjectHaveSelected(CGlbGlobeObject* obj, std::vector<glbref_ptr<CGlbGlobeObject>>& result);
		// 去除result中的重复对象，只保留一个. 输入输出数量发生变化【有重复对象】时返回true， 没有变化时返回false
		bool _makeobjectsUnique(std::vector<glbref_ptr<CGlbGlobeObject>>& result);
	private:
		CGlbGlobeOctree* mpr_globe_top_octree;
		std::map<long,CGlbGlobeOctree*> mpr_map_octrees;
		long mpr_max_depth;
		std::wstring mpr_name;
		CGlbGlobeQtree* mpr_globe_top_qtree;
		std::map<long,CGlbGlobeQtree*> mpr_map_qtrees;
		CGlbExtent*		mpr_p_extent;
		bool	mpr_b_globe_type;
		CRITICAL_SECTION	mpr_objects_criticalsection;//对象操作临界区
		glbInt32			mpr_onterrain_objects_count;//贴地形绘制对象的数量
		glbInt32			mpr_notonterr_objects_count;//场景管理树中所有非贴地绘制对象的数量
	};
}


