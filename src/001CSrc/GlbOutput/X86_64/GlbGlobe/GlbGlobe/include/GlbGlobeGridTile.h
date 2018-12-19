/********************************************************************
* Copyright (c) 2014 北京超维创想信息技术有限公司
* All rights reserved.
*
* @file    GlbGlobeGridTile.h
* @brief   格网块 头文件
*
* 这个档案定义CGlbGlobeGridTile这个class
*
* @version 1.0
* @author  马林
* @date    2018-7-17 16:10
*********************************************************************/
#pragma once
#include <osg/BoundingBox>
#include <osg/Polytope>
#include "IGlbDataEngine.h"
#include "GlbReference.h"
#include "GlbGlobeTypes.h"
#include "GlbExtent.h"
#include "glbref_ptr.h"
#include "IGlbGlobeTask.h"
#include <osg/Geometry>
#include <osg/MatrixTransform>
#include <osg/Geode>

namespace GlbGlobe
{
	class CGlbGlobeGridLayer;
	class CGlbGlobeGridTile : public CGlbReference
	{
	public:
		/**
		* @brief 网格块构造函数
		* @param [in] grid 网格对象
		* @param [in] level 网格块等级
		* @param [in] south,north,west,east  网格块 东西南北 边界
		* @param [in] minAlt,maxAlt 网格块的最低和最高 高程 
		* @param [in] parent 父块	
		* @return  无	
		*/
		CGlbGlobeGridTile(CGlbGlobeGridLayer* grid,glbInt32 level,
			glbDouble south,glbDouble north,glbDouble west,glbDouble east,
			glbDouble minAlt=0,glbDouble maxAlt=0,
			CGlbGlobeGridTile* parent=NULL);
		/**
		* @brief 网格块析构函数
		* @return 无
		*/
		~CGlbGlobeGridTile(void);
		/**
		* @brief 网格块初始化
		* @return  无
		*/
		void Initialize(glbBool	load_direct=false);
		/**
		* @brief 获取网格块是否初始化
		* @return  已初始化返回true
		-	 没有初始化返回false
		*/
		glbBool IsInitialized();
		/**
		* @brief 网格块更新
		* @return  无
		*/
		void Update();
		/**
		* @brief  网格块数据加载
		* @note  网格块初始化时的加载线程调用此接口
		* @return  无
		*/
		void LoadData();
		/**
		* @brief 网格块销毁
		* @param [in] dispos_direct 是否直接销毁 似乎不需要此参数？？？？
		* @param [in] dispose_level_zero_tile 销毁0级网格块
		* @return 无
		*/
		void Dispose(glbBool dispos_direct=false ,glbBool dispose_level_zero_tile = false);
		/**
		* @brief 获取网格块等级
		* @return 网格块等级
		*/
		glbInt32 GetLevel();
		/**
		* @brief 获取网格块的行列号
		* @param [out] row 行号
		* @param [out] column 列号
		* @return 无
		*/
		void GetPosition(glbInt32& row, glbInt32& column,glbInt32& layer);
		/**
		* @brief 设置网格块的行列号
		* @param [in] row 行号
		* @param [in] column 列号
		* @return 无
		*/
		glbBool SetPosition(glbInt32 row, glbInt32 column,glbInt32 layer);
		/**
		* @brief 获取网格块内的XYZ方向的格子数量
		* @param [out] xCellNum x方向的格子数量
		* @param [out] yCellNum y方向的格子数量
		* @param [out] zCellNum z方向的格子数量
		* @return 无
		*/
		void GetXYZCellNum(glbInt32& xCellNum, glbInt32& yCellNum,glbInt32& zCellNum);
		/**
		* @brief 设置网格块内的XYZ方向的格子数量
		* @param [in] xCellNum x方向的格子数量
		* @param [in] yCellNum y方向的格子数量
		* @param [in] zCellNum z方向的格子数量
		* @return true
		*/
		glbBool SetXYZCellNum(glbInt32 xCellNum, glbInt32 yCellNum,glbInt32 zCellNum);
		/**
		* @brief 获取网格块的范围
		* @param [out] south,north,west,east  网格块 东西南北 边界
		* @param [out] minAlt,maxAlt 网格块的最低和最高 高程 
		* @return 无
		*/
		void GetRange(glbDouble& east, glbDouble& west, glbDouble& south, glbDouble& north, glbDouble& minAlt, glbDouble& maxAlt);

		/**
		* @brief 获取网格块的范围
		* @return 网格块范围
		*/
		CGlbExtent* GetExtent();
		/**
		* @brief 获取网格块的上一级父亲块
		* @return 父块
		*/
		CGlbGlobeGridTile* GetParentTile();
		/**
		* @brief 获取网格块是否可见
		* @return 可见返回true
		-	不可见返回false
		*/
		glbBool IsVisible(); 
		/**
		* @brief 获取网格块在父块中的方位
		* @return 方位
		- GlB_SOUTHWEST	西南
		- GLB_SOUTHEAST 东南
		- GLB_NORTHWEST 西北
		- GLB_NORTHEAST 东北
		*/
		GlbGlobeChildLocationEnum GetTileLocation();
		/**
		* @brief 获取网格块的东北子块
		* @return 如果东北子块存在返回子块指针
		-   如果东北子块不存在返回NULL
		*/
		CGlbGlobeGridTile* GetNorthEastChild();
		/**
		* @brief 获取网格块的西北子块
		* @return 如果西北子块存在返回子块指针
		-   如果西北子块不存在返回NULL
		*/
		CGlbGlobeGridTile* GetNorthWestChild();
		/**
		* @brief 获取网格块的东南子块
		* @return 如果东南子块存在返回子块指针
		-   如果东南子块不存在返回NULL
		*/
		CGlbGlobeGridTile* GetSouthEastChild();
		/**
		* @brief 获取网格块的西南子块
		* @return 如果西南子块存在返回子块指针
		-   如果西南子块不存在返回NULL
		*/
		CGlbGlobeGridTile* GetSouthWestChild();
	private:
		// 计算块外包
		void ComputeBoundBox();
		// 判断块是否需要分裂
		glbBool IsTileSplit();
		// 生成四个子块
		void ComputeChildren();
		// 加载格网块数据 
		glbBool LoadTileData();
		// 创建格网块绘制网格节点(包含geometry)
		osg::ref_ptr<osg::Group> CreateElevatedMesh();
		// 世界坐标转屏幕坐标
		void WorldToScreen(glbDouble &x, glbDouble &y, glbDouble &z);
		// 计算获取离相机点距离由近到远的块排序
		glbBool GetSortedChildTiles(std::map<glbDouble/*与相机距离*/,glbref_ptr<CGlbGlobeGridTile>>& tiles);	
		// 析构格网块surface
		void DestructorTileSurface(GlbGridTileSurface* pTileSurface);
		// 判断是否是内部块 -- 是内部块返回true
		glbBool IsInsideTile();
		GlbTileLocationEnum ComputeTileLocation();
		// 判断内部tile是否分裂
		glbBool isInsideTileSplit();
		// 判断边缘tile是否分裂
		glbBool isEdgeTileSplit(GlbTileLocationEnum lc);
		glbBool testEdgeTileWestEdge(glbDouble xmin,glbDouble xmax,glbDouble ymin,glbDouble ymax,glbDouble zmin,glbDouble zmax,osg::Vec3d cameraPos);
		glbBool testEdgeTileEastEdge(glbDouble xmin,glbDouble xmax,glbDouble ymin,glbDouble ymax,glbDouble zmin,glbDouble zmax,osg::Vec3d cameraPos);
		glbBool testEdgeTileSouthEdge(glbDouble xmin,glbDouble xmax,glbDouble ymin,glbDouble ymax,glbDouble zmin,glbDouble zmax,osg::Vec3d cameraPos);
		glbBool testEdgeTileNorthEdge(glbDouble xmin,glbDouble xmax,glbDouble ymin,glbDouble ymax,glbDouble zmin,glbDouble zmax,osg::Vec3d cameraPos);
	private:
		CGlbGlobeGridLayer*	mpr_grid;				//网格对象
		glbref_ptr<CGlbGlobeGridTile> mpr_nwchild;	//西北子块
		glbref_ptr<CGlbGlobeGridTile> mpr_nechild;	//东北子块
		glbref_ptr<CGlbGlobeGridTile> mpr_swchild;	//西南子块
		glbref_ptr<CGlbGlobeGridTile> mpr_sechild;	//东南子块
		glbref_ptr<CGlbGlobeGridTile> mpr_parent_tile;	//父块
		GlbGlobeChildLocationEnum mpr_location_enum;	//本块在父块中的位置
		//osg::ref_ptr<osg::MatrixTransform> mpr_node;	//osg节点
		osg::ref_ptr<osg::Group> mpr_node;				//osg节点 - switch
		glbDouble mpr_south;							//南边界
		glbDouble mpr_north;							//北边界
		glbDouble mpr_east;								//东边界
		glbDouble mpr_west;								//西边界
		glbDouble mpr_minalt;							//最小高度
		glbDouble mpr_maxalt;							//最大高度
		glbDouble mpr_center_longitude;					//块中心点的经度
		glbDouble mpr_center_latitude;					//块中心点的纬度
		glbref_ptr<CGlbExtent>	mpr_extent;				//网格块范围		
		osg::BoundingBox mpr_boundbox;					//网格块范围
		//std::vector<osg::Vec3> mpr_surrond_pointvec;
		osg::Vec3d		mpr_local_origin;				//网格块基准点[默认为网格块的中心点]
		glbBool mpr_isinitialized;						//初始化标志
		glbBool	mpr_isLoadingData;						//正在加载数据标志
		glbBool mpr_isDataLoaded;						//标志数据是否已经加载完成
		glbInt32 mpr_level;								//等级
		glbInt32 mpr_row;								//行号
		glbInt32 mpr_column;							//列号
		glbInt32 mpr_layer;								//层号
		GlbGlobeTypeEnum mpr_globe_type;				//globe场景类型
		glbDouble* mpr_data;							//原始网格块dom数据
		glbInt32	mpr_xCellNum;						//网格块x方向的格子数量
		glbInt32	mpr_yCellNum;						//网格块y方向的格子数量
		glbInt32	mpr_zCellNum;						//网格块z方向的格子数量	
		//glbInt32	mpr_vertex_count;					

		glbDouble	mpr_longitude_span;
		glbDouble	mpr_latitude_span;	

		GlbGridTileSurface* mpr_pTileSurface;			//网格外表面数据

		glbref_ptr<IGlbGlobeTask>	mpr_gridTask;		//网格数据加载任务

		osg::ref_ptr<osg::Geode> mpr_p_geode;
		osg::ref_ptr<osg::Geometry> mpr_p_northWestgeom;
		osg::ref_ptr<osg::Geometry> mpr_p_northEastgeom;
		osg::ref_ptr<osg::Geometry> mpr_p_southEastgeom;
		osg::ref_ptr<osg::Geometry> mpr_p_southWestgeom;	
	};
}

