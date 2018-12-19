/********************************************************************
* Copyright (c) 2014 北京超维创想信息技术有限公司
* All rights reserved.
*
* @file    GlbGlobeWMTSTile.h
* @brief   WMTS 地图瓦片 头文件
*
* 这个档案定义CGlbGlobeWMTSTile这个class
*
* @version 1.0
* @author  马林
* @date    2017-6-5 10:10
*********************************************************************/
#pragma once
#include <osg/BoundingBox>
#include <osg/Polytope>
#include "glbref_ptr.h"
#include "GlbReference.h"
#include "GlbGlobeTypes.h"
#include "IGlbGlobeTask.h"
#include "GlbExtent.h"
#include <osg/Geometry>
#include <osg/MatrixTransform>
#include <osg/Geode>
#include <osg/Texture2D>

namespace GlbGlobe
{
	class CGlbGlobeWMTSLayer;
	class CGlbGlobeWMTSTile : public CGlbReference
	{
	public:
		/**
		* @brief 瓦片块构造函数
		* @param [in] WMTSLayer wmts图层对象
		* @param [in] level 地形块等级
		* @param [in] south,north,west,east  瓦片 东西南北 边界
		* @param [in] minAlt,maxAlt 瓦片的最低和最高 高程 
		* @param [in] parent 父瓦片	
		* @return  无	
		*/
		CGlbGlobeWMTSTile(CGlbGlobeWMTSLayer* player,glbInt32 level,
			glbDouble south,glbDouble north,glbDouble west,glbDouble east,
			glbDouble minAlt=0,glbDouble maxAlt=0,
			CGlbGlobeWMTSTile* parent=NULL);
		/**
		* @brief 瓦片析构函数
		* @return 无
		*/
		~CGlbGlobeWMTSTile(void);

		/**
		* @brief 瓦片初始化
		* @return  无
		*/
		void Initialize(glbBool	load_direct=false);
		/**
		* @brief 获取瓦片是否初始化
		* @return  已初始化返回true
		-	 没有初始化返回false
		*/
		glbBool IsInitialized();
		/**
		* @brief 地形块更新		
		* @return  无
		*/
		void Update();
		/**
		* @brief 地形块数据加载		
		* @return  无
		*/
		void LoadData();
		/**
		* @brief 地形块销毁		
		* @param [in] dispose_level_zero_tile 销毁0级地形块
		* @return 无
		*/
		void Dispose(glbBool dispose_level_zero_tile = false);
		/**
		* @brief 地形块接边
		* @return 无
		*/
		void Merge();
		/**
		* @brief 获取地形块的唯一ID - key
		* @return 地形块key
		*/
		glbInt32 GetKey();
		/**
		* @brief 获取地形块等级
		* @return 地形块等级
		*/
		glbInt32 GetLevel();
		/**
		* @brief 获取地形块的行列号
		* @param [out] row 行号
		* @param [out] column 列号
		* @return 无
		*/
		void GetPosition(glbInt32& row, glbInt32& column);
		/**
		* @brief 设置地形块的行列号
		* @param [in] row 行号
		* @param [in] column 列号
		* @return 无
		*/
		glbBool SetPosition(glbInt32 row, glbInt32 column);
		/**
		* @brief 获取地形块的范围
		* @param [out] south,north,west,east  地形块 东西南北 边界
		* @param [out] minAlt,maxAlt 地形块的最低和最高 高程 
		* @return 无
		*/
		void GetRange(glbDouble& east, glbDouble& west, glbDouble& south, glbDouble& north, glbDouble& minAlt, glbDouble& maxAlt);
		/**
		* @brief 获取地形块的上一级父亲块
		* @return 父块
		*/
		CGlbGlobeWMTSTile* GetParentTile();
		/**
		* @brief 获取地形块是否可见
		* @return 可见返回true
		-	不可见返回false
		*/
		glbBool IsVisible();
		/**
		* @brief 获取地形块在父块中的方位
		* @return 方位
		- GlB_SOUTHWEST	西南
		- GLB_SOUTHEAST 东南
		- GLB_NORTHWEST 西北
		- GLB_NORTHEAST 东北
		*/
		GlbGlobeChildLocationEnum GetTileLocation();
		/**
		* @brief 获取地形块的东北子块
		* @return 如果东北子块存在返回子块指针
		-   如果东北子块不存在返回NULL
		*/
		CGlbGlobeWMTSTile* GetNorthEastChild();
		/**
		* @brief 获取地形块的西北子块
		* @return 如果西北子块存在返回子块指针
		-   如果西北子块不存在返回NULL
		*/
		CGlbGlobeWMTSTile* GetNorthWestChild();
		/**
		* @brief 获取地形块的东南子块
		* @return 如果东南子块存在返回子块指针
		-   如果东南子块不存在返回NULL
		*/
		CGlbGlobeWMTSTile* GetSouthEastChild();
		/**
		* @brief 获取地形块的西南子块
		* @return 如果西南子块存在返回子块指针
		-   如果西南子块不存在返回NULL
		*/
		CGlbGlobeWMTSTile* GetSouthWestChild();

		/**
		* @brief 获取指定位置的高程
		* @param [in] xOrLon, yOrLat 位置				 
				 [out] h 高程
		* @return 成功返回true，失败返回false
		*/
		glbBool	GetPosHeight(glbDouble xOrLon,glbDouble yOrLat, glbDouble& h);
		/**
		* @brief 获取指定位置的高程
		* @param [in] xOrLon, yOrLat 位置				 
					  zOrAlt 高程
		* @return 成功返回true，失败返回false
		* @note 废弃不用！！！！
		*/
		glbBool	ModifyPosHeight(glbDouble xOrLon,glbDouble yOrLat, glbDouble zOrAlt);
	private:
		void UpdateGlobe();		
		void UpdateFlat();

		void MergeGlobe();
		void MergeFlat();

		void ComputeChildren();
		// 加载地形块dom和dem数据 
		glbBool LoadTileData();
		// 创建地形块绘制网格节点(包含geometry)
		osg::ref_ptr<osg::Group> CreateElevatedMesh();
		// 获取（row,column）处高程值
		glbFloat GetHeightData(glbFloat* p_heightdatas, glbInt32 column, glbInt32 row, GlbGlobeChildLocationEnum location);
		// 重新计算mpr_boundbox
		void ComputeBoundBox();
		// 世界坐标转屏幕坐标
		void WorldToScreen(glbDouble &x, glbDouble &y, glbDouble &z);
		// 判断块是否需要分裂
		glbBool IsTileSplit();
		// 获取北面相邻tile对象
		CGlbGlobeWMTSTile* GetNorthDirectionTile();		
		// 获取西面相邻tile对象
		CGlbGlobeWMTSTile* GetWestDirectionTile();
		// 获取南面相邻tile对象
		CGlbGlobeWMTSTile* GetSouthDirectionTile();
		// 获取东面相邻tile对象
		CGlbGlobeWMTSTile* GetEastDirectionTile();
		// 获取按距离相机从近到远顺序排列的四个tile
		glbBool GetSortedChildTiles(std::map<glbDouble,glbref_ptr<CGlbGlobeWMTSTile>>& tiles);
		// 恢复默认edge顶点高程
		glbBool	RecoverEdgeVertexsAltitude(GlbTileEdgeEnum edge);

		glbBool isCoordValidate(double x, double y, double z);
	private:
		CGlbGlobeWMTSLayer*	mpr_wmtsLayer;				//wmts图层
		glbref_ptr<CGlbGlobeWMTSTile> mpr_nwchild;		//西北子块
		glbref_ptr<CGlbGlobeWMTSTile> mpr_nechild;		//东北子块
		glbref_ptr<CGlbGlobeWMTSTile> mpr_swchild;		//西南子块
		glbref_ptr<CGlbGlobeWMTSTile> mpr_sechild;		//东南子块
		glbref_ptr<CGlbGlobeWMTSTile> mpr_parent_tile;	//父块
		GlbGlobeChildLocationEnum	  mpr_location_enum;	//本块在父块中的位置
		osg::ref_ptr<osg::Group>	  mpr_node;				//osg节点 - switch
		osg::ref_ptr<osg::Texture2D> mpr_texture;		//地形块纹理
		osg::ref_ptr<osg::Image> mpr_p_osgImg;			//对应生成的osg::Image对象	
		glbDouble mpr_south;							//南边界
		glbDouble mpr_north;							//北边界
		glbDouble mpr_east;								//东边界
		glbDouble mpr_west;								//西边界
		glbDouble mpr_minalt;							//最小高度
		glbDouble mpr_maxalt;							//最大高度
		glbDouble mpr_center_longitude;					//块中心点的经度
		glbDouble mpr_center_latitude;					//块中心点的纬度		
		glbref_ptr<CGlbExtent>	mpr_extent;				//地形块范围		
		osg::BoundingBox mpr_boundbox;					//地形块范围
		osg::Vec3d		mpr_local_origin;				//地形块基准点[默认为地形块的中心点]
		glbBool mpr_isinitialized;						//初始化标志
		glbBool	mpr_isLoadingData;						//正在加载数据标志
		glbBool mpr_isDataLoaded;						//标志数据是否已经加载完成
		glbInt32 mpr_level;								//等级
		glbInt32 mpr_row;								//行号
		glbInt32 mpr_column;							//列号
		GlbGlobeTypeEnum mpr_globe_type;				//globe场景类型
		glbInt32		mpr_demsize;					//dem行列数
		glbFloat*		mpr_demdata;					//dem数据17*17
		glbInt32		mpr_vertex_count;

		glbDouble		mpr_longitude_span;
		glbDouble		mpr_latitude_span;	

		// 缝边用geometry
		osg::ref_ptr<osg::Geode>	mpr_p_geode;
		osg::ref_ptr<osg::Geometry> mpr_p_northWestgeom;
		osg::ref_ptr<osg::Geometry> mpr_p_northEastgeom;
		osg::ref_ptr<osg::Geometry> mpr_p_southEastgeom;
		osg::ref_ptr<osg::Geometry> mpr_p_southWestgeom;


		glbInt32	mpr_north_easthalf_merge_code;
		glbInt32	mpr_north_westhalf_merge_code;
		glbInt32	mpr_south_easthalf_merge_code;
		glbInt32	mpr_south_westhalf_merge_code;
		glbInt32	mpr_east_northhalf_merge_code;
		glbInt32	mpr_east_southhalf_merge_code;
		glbInt32	mpr_west_northhalf_merge_code;
		glbInt32	mpr_west_southhalf_merge_code;

		glbref_ptr<IGlbGlobeTask>	mpr_tileTask;
	};
}

