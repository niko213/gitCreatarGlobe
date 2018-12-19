/********************************************************************
* Copyright (c) 2014 北京超维创想信息技术有限公司
* All rights reserved.
*
* @file    GlbGlobeTerrainTile.h
* @brief   地形块 头文件
*
* 这个档案定义CGlbGlobeTerrainTile这个class
*
* @version 1.0
* @author  马林
* @date    2014-5-8 16:10
*********************************************************************/
#pragma once
#include <osg/BoundingBox>
#include <osg/Polytope>
#include "GlbReference.h"
#include "GlbGlobeDomLayer.h"
#include "GlbGlobeDemLayer.h"
#include "GlbGlobeRObject.h"
#include "GlbGlobeTypes.h"
#include "IGlbGlobeTask.h"
#include <osg/Geometry>
#include <osg/MatrixTransform>
#include <osg/Geode>
#include <osg/Texture2D>

namespace GlbGlobe
{

	class CGlbGlobeTerrain;
	class CGlbGlobeTerrainTile : public CGlbReference
	{
	private:
		/** 
		* @brief 地形块节点裁剪回调类[废弃discard]
		* @author 马林
		* @date    2015-3-5 11:10
		* @note 地形块内部类,负责控制不可见裁剪
		*/
		class CGlbGlobeTerrainTileCullCallBack : public osg::NodeCallback
		{
		public:
			/**
			* @brief 地形块节点裁剪回调类构造函数
			* @param [in] bbox  地形块外包
			* @param [in] level 地形块等级	  
			* @return  无
			*/
			CGlbGlobeTerrainTileCullCallBack(const osg::BoundingBox& bbox,glbInt32 level); 			

			~CGlbGlobeTerrainTileCullCallBack();
			/**
			* @brief 地形osg节点回调实现
			* @param [in] node 地形块的osg节点
			* @param [in] nv osg节点访问器	  
			* @return  无
			*/
			virtual void operator() (osg::Node* node,osg::NodeVisitor* nv);
		private:
			osg::BoundingBox mpr_bbox;	
			//osg::Camera* mpr_p_osgCamera;
			//CGlbGlobeTerrainTile* mpr_p_terraintile;
			glbInt32 mpr_level;
		};
	public:
		/**
		* @brief 地形块构造函数
		* @param [in] terrain 地形对象
		* @param [in] level 地形块等级
		* @param [in] south,north,west,east  地形块 东西南北 边界
		* @param [in] minAlt,maxAlt 地形块的最低和最高 高程 
		* @param [in] parent 父块	
		* @return  无	
		*/
		CGlbGlobeTerrainTile(CGlbGlobeTerrain* terrain,glbInt32 level,
			glbDouble south,glbDouble north,glbDouble west,glbDouble east,
			glbDouble minAlt=0,glbDouble maxAlt=0,
			CGlbGlobeTerrainTile* parent=NULL);
		/**
		* @brief 地形块析构函数
		* @return 无
		*/
		~CGlbGlobeTerrainTile(void);
		/**
		* @brief 地形块初始化
		* @param [in] domlayers dom图层集合
		* @param [in] demlayers dem图层集合
		* @param [in] terrainobjs 贴地形绘制对象集合
		* @param [in] mTerrainobjs 地形修改对象集合
		* @return  无
		*/
		void Initialize(	std::vector<glbref_ptr<CGlbGlobeDomLayer>>& domlayers,
			std::vector<glbref_ptr<CGlbGlobeDemLayer>>& demlayers,
			std::vector<glbref_ptr<CGlbGlobeRObject>>& terrainobjs, 
			std::vector<glbref_ptr<CGlbGlobeRObject>>& mTerrainobjs,
			glbBool	load_direct=false);
		/**
		* @brief 获取地形块是否初始化
		* @return  已初始化返回true
		-	 没有初始化返回false
		*/
		glbBool IsInitialized();
		/**
		* @brief 地形块更新
		* @param [in] domlayers dom图层集合
		* @param [in] demlayers dem图层集合
		* @param [in] terrainobjs 贴地形绘制对象集合
		* @param [in] mTerrainobjs 地形修改对象集合
		* @return  无
		*/
		void Update(	std::vector<glbref_ptr<CGlbGlobeDomLayer>>& domlayers,
			std::vector<glbref_ptr<CGlbGlobeDemLayer>>& demlayers,
			std::vector<glbref_ptr<CGlbGlobeRObject>>& terrainobjs, 
			std::vector<glbref_ptr<CGlbGlobeRObject>>& mTerrainobjs);
		/**
		* @brief 地形块数据加载
		* @param [in] domlayers dom图层集合
		* @param [in] demlayers dem图层集合
		* @param [in] terrainobjs 贴地形绘制对象集合
		* @param [in] mTerrainobjs 地形修改对象集合
		* @note 地形块初始化时的加载线程调用此接口
		* @return  无
		*/
		void LoadData(	std::vector<glbref_ptr<CGlbGlobeDomLayer>>& domlayers,
			std::vector<glbref_ptr<CGlbGlobeDemLayer>>& demlayers,
			std::vector<glbref_ptr<CGlbGlobeRObject>>& terrainobjs, 
			std::vector<glbref_ptr<CGlbGlobeRObject>>& mTerrainobjs);
		/**
		* @brief 地形块纹理数据加载	  
		* @param [in] domlayers dom图层集合
		* @param [in] terrainobjs 贴地形绘制对象集合
		* @param [in] domrefresh 是否是dom图层刷新
		* @note  RefreshTexture时的加载线程调用此接口
		* @return  无
		*/
		void LoadDomData(	std::vector<glbref_ptr<CGlbGlobeDomLayer>>& domlayers,
			std::vector<glbref_ptr<CGlbGlobeRObject>>& terrainobjs,
			glbBool domrefresh);

		/**
		* @brief 地形块dem数据加载	  
		* @param [in] demlayers dem图层集合
		* @param [in] mterrainobjs 地形修改对象集合
		* @note  RefreshDem时的加载线程调用此接口
		* @return  无
		*/
		void LoadDemData(	std::vector<glbref_ptr<CGlbGlobeDemLayer>>& demlayers,
			std::vector<glbref_ptr<CGlbGlobeRObject>>& mterrainobjs);
		/**
		* @brief 地形块销毁
		* @param [in] dispos_direct 是否直接销毁 似乎不需要此参数？？？？
		* @param [in] dispose_level_zero_tile 销毁0级地形块
		* @return 无
		*/
		void Dispose(glbBool dispos_direct=false ,glbBool dispose_level_zero_tile = false);
		/**
		* @brief 刷新地形块纹理影像
		* @param [in] objDirtyExts 需要刷新的贴地形对象区域
		* @param [in] domDirtyExts 需要刷新的dom图层区域
		* @param [in] domlayers	dom图层集合
		* @param [in] terrainObjs 贴地形绘制的对象集合
		* @note 本接口与dom图层和贴地形绘制对象相关.
		- GLB_GLOBETYPE_GLOBE球模式下 为地理坐标 单位：度
		- GLB_GLOBETYPE_FLAT平面模式下 为世界坐标 单位：米
		* @return  无 			
		*/
		void RefreshTexture(std::vector<CGlbExtent>						&objDirtyExts,
			std::vector<CGlbExtent>						&domDirtyExts,
			std::vector<glbref_ptr<CGlbGlobeDomLayer>>  &domlayers,                   
			std::vector<glbref_ptr<CGlbGlobeRObject>>	&terrainObjs);
		/**
		* @brief 刷新地形块高程数据
		* @param [in] mtobjDirtyExts 需要刷新的地形修改对象区域
		* @param [in] demDirtyExts 需要刷新的dem图层区域
		* @param [in] demlayers	dem图层集合
		* @param [in] mterrainObjs 地形修改对象集合
		* @param [in] operationPackage 操作包
		* @note 本接口与dem图层和地形修改对象相关.
		- GLB_GLOBETYPE_GLOBE球模式下 为地理坐标 单位：度
		- GLB_GLOBETYPE_FLAT平面模式下 为世界坐标 单位：米
		* @return  无 			
		*/
		void RefreshDem(	std::vector<CGlbExtent>						&mtobjDirtyExts,
			std::vector<CGlbExtent>						&demDirtyExts,
			std::vector<glbref_ptr<CGlbGlobeDemLayer>>  &demlayers,                   
			std::vector<glbref_ptr<CGlbGlobeRObject>>   &mterrainObjs
			/*CGlbGlobeTerrain::GlbTileOperationPackage	&operationPackage*/ );
		/**
		* @brief 地形块接边
		* @return 无
		*/
		void Merge();
		/*
		* @brief 计算(lonOrX,latOrY)坐标位置处的高程   
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
		* @brief 计算地形块可见区域
		* @param [in] cv 视锥体
		* @param [in] min_southDegree 需要计算的南纬起始角度 (默认为-66.5度)
		* @param [in] max_norhtDegree 需要计算的北纬结束角度 (默认为66.5度)
		* @return  可见的范围boundingbox
		*/
		osg::BoundingBox GetVisibleExtent(osg::Polytope cv,glbDouble min_southDegree, glbDouble max_norhtDegree);
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
		* @brief 获取地形块的范围
		* @return 地形块范围
		*/
		CGlbExtent* GetExtent();
		/**
		* @brief 获取地形块的上一级父亲块
		* @return 父块
		*/
		CGlbGlobeTerrainTile* GetParentTile();
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
		* @brief 获取地形块纹理 [废弃]
		* @return 纹理对象			
		*/
		osg::Texture2D* GetTexture();
		/**
		* @brief 获取地形块的dem高程数据
		* @param [out] p_data 高程数据
		* @param [out] dem_columns 高程数据列数
		* @param [out] dem_rows 高程数据行数
		* @return 无
		*/
		void GetDemdata(glbFloat** p_data, glbInt32 &dem_columns, glbInt32 &dem_rows);
		/**
		* @brief 获取地形块的dom纹理数据
		* @param [out] p_data 纹理数据
		* @param [out] dom_columns  纹理数据列数
		* @param [out] dom_rows  纹理数据行数
		* @return 无
		*/
		void GetDomdata(glbByte** p_data, glbInt32 &dom_columns, glbInt32 &dom_rows);
		/**
		* @brief 获取地形块的东北子块
		* @return 如果东北子块存在返回子块指针
		-   如果东北子块不存在返回NULL
		*/
		CGlbGlobeTerrainTile* GetNorthEastChild();
		/**
		* @brief 获取地形块的西北子块
		* @return 如果西北子块存在返回子块指针
		-   如果西北子块不存在返回NULL
		*/
		CGlbGlobeTerrainTile* GetNorthWestChild();
		/**
		* @brief 获取地形块的东南子块
		* @return 如果东南子块存在返回子块指针
		-   如果东南子块不存在返回NULL
		*/
		CGlbGlobeTerrainTile* GetSouthEastChild();
		/**
		* @brief 获取地形块的西南子块
		* @return 如果西南子块存在返回子块指针
		-   如果西南子块不存在返回NULL
		*/
		CGlbGlobeTerrainTile* GetSouthWestChild();
		/**
		* @brief 获取地形块边缘的高程数据
		* @param [in] edge 地形块边缘
		* @return 高程数据数组
		*/
		osg::Vec3dArray* GetEdgeVertexArray(GlbTileEdgeEnum edge);
		/**
		* @brief 设置地形块不透明度
		* @param [in] domlayers	dom图层集合
		* @param [in] terrainObjs 贴地形绘制的对象集合
		* @param [in] opacity 不透明度
		* @return  成功返回true
		-	 失败返回false
		*/
		glbBool SetOpacity(	std::vector<glbref_ptr<CGlbGlobeDomLayer>> &domlayers,                   
			std::vector<glbref_ptr<CGlbGlobeRObject>>  &terrainObjs,
			glbInt32 opacity);

		/**
		* @brief 设置地形块夸张系数
		* @param [in] exaggrate 地形夸张系数	  
		* @return  成功返回true
		-	 失败返回false
		*/
		glbBool SetExaggrate(glbDouble exaggrate);

		/**
		* @brief 获取地形对象[废弃]
		* @return 地形对象
		*/
		CGlbGlobeTerrain* GetTerrain();

		/**
		* @brief 刷新地形块接边的geometry的顶点数组
		* @return 地形对象
		*/
		glbBool UpdateEdgeGeometryVertexArray();
		/** 
		* @brief 根据tile范围过滤出与此地块相交的贴地绘制对象
		* @param [in] terrainobjs 贴地形绘制对象集合
		* @param [out] outObjs  与地形块相交的对象集合
		* @return  有交集 返回true ，没有交集 返回 false
		*/
		glbBool FilterOnTerrainObjects(std::vector<glbref_ptr<CGlbGlobeRObject>>& terrainobjs, std::vector<glbref_ptr<CGlbGlobeRObject>>& outObjs);
		/** 
		* @brief 根据tile范围过滤出与此地块相交的"脏"区域集合
		* @param [in] objDirtyExts "脏"区域集合
		* @param [out] outObjDirtyExts  与地形块相交的"脏"区域集合
		* @return  有交集 返回true ，没有交集 返回 false
		*/
		glbBool FilterObjDirtyExt(std::vector<CGlbExtent>&objDirtyExts, std::vector<CGlbExtent>& outObjDirtyExts);

		// 测试【废弃】
		glbDouble computeSrcProjArea();
	private:
		void UpdateGlobe(	std::vector<glbref_ptr<CGlbGlobeDomLayer>> &domlayers,
			std::vector<glbref_ptr<CGlbGlobeDemLayer>> &demlayers,
			std::vector<glbref_ptr<CGlbGlobeRObject>>  &terrainobjs, 
			std::vector<glbref_ptr<CGlbGlobeRObject>>  &mTerrainobjs);
		void UpdateGlobe_Pro(std::vector<glbref_ptr<CGlbGlobeDomLayer>> &domlayers,
			std::vector<glbref_ptr<CGlbGlobeDemLayer>>	&demlayers,
			std::vector<glbref_ptr<CGlbGlobeRObject>>	&terrainobjs, 
			std::vector<glbref_ptr<CGlbGlobeRObject>>	&mTerrainobjs);

		void UpdateFlat(	std::vector<glbref_ptr<CGlbGlobeDomLayer>>	&domlayers,
			std::vector<glbref_ptr<CGlbGlobeDemLayer>>	&demlayers,
			std::vector<glbref_ptr<CGlbGlobeRObject>>	&terrainobjs, 
			std::vector<glbref_ptr<CGlbGlobeRObject>>	&mTerrainobjs);

		void MergeGlobe();
		void MergeFlat();

		glbDouble SphericalDistance(glbDouble latA, glbDouble lonA, glbDouble latB, glbDouble lonB);

		void ComputeChildren();
		glbInt32 GetRowFromLatitude(glbDouble latitude, glbDouble tileSize);
		glbInt32 GetColFromLongitude(glbDouble longitude, glbDouble tileSize);
		// 加载地形块dom和dem数据 
		glbBool LoadTileData();
		// 加载覆盖地形块的domlayer和terrainobjects数据->image形成纹理
		glbBool LoadOverlayTextureData(	std::vector<glbref_ptr<CGlbGlobeDomLayer>>	&domlayers,
			std::vector<glbref_ptr<CGlbGlobeRObject>>	&terrainobjs);
		// 加载覆盖此地形块的demlayer和mterrainobjects数据->直接修改地形块的dem数据17*17
		glbBool LoadOverlayDemData(		std::vector<glbref_ptr<CGlbGlobeDemLayer>>	&demlayers,
			std::vector<glbref_ptr<CGlbGlobeRObject>>	&mTerrainobjs);	
		// 插值加密部分dem数据
		glbFloat* MultifyDemSection(glbFloat* p_data, glbInt32 dem_columns, glbInt32 dem_rows, GlbGlobeChildLocationEnum location);
		// 插值加密部分dom数据
		glbByte* MultifyDomSection(glbByte* p_data, glbInt32 dom_columns, glbInt32 dom_rows, GlbGlobeChildLocationEnum location);
		// 创建地形块绘制网格节点(包含geometry)
		osg::ref_ptr<osg::Group> CreateElevatedMesh();
		// 获取（row,column）处高程值
		glbFloat GetHeightData(glbFloat* p_heightdatas, glbInt32 column, glbInt32 row, GlbGlobeChildLocationEnum location);
		// 根据等级进行配色
		osg::Vec4 AdjustTileColor(long level);
		// 判断块是否需要分裂
		glbBool IsTileSplit();
		// 世界坐标转屏幕坐标
		void WorldToScreen(glbDouble &x, glbDouble &y, glbDouble &z);
		// 删除边界geometry
		void RemoveEdgeGeometry(GlbTileEdgeEnum edge);
		// 生成边界geometry
		void BuildEdgeGeometry(osg::Vec3dArray* p_neighbour_edge_vertex_array,GlbTileEdgeEnum edge);
		// 修改边界geometry
		void ModifyEdgeGeometry(osg::Vec3dArray* p_neighbour_edge_vertex_array,GlbTileEdgeEnum edge);
		// 获取北面相邻tile对象
		CGlbGlobeTerrainTile* GetNorthDirectionTile();
		CGlbGlobeTerrainTile* GetNorthDirectionTileOnFlatTerrain();
		// 获取西面相邻tile对象
		CGlbGlobeTerrainTile* GetWestDirectionTile();
		CGlbGlobeTerrainTile* GetWestDirectionTileOnFlatTerrain();
		// 获取南面相邻tile对象
		CGlbGlobeTerrainTile* GetSouthDirectionTile();
		CGlbGlobeTerrainTile* GetSouthDirectionTileOnFlatTerrain();
		// 获取东面相邻tile对象
		CGlbGlobeTerrainTile* GetEastDirectionTile();
		CGlbGlobeTerrainTile* GetEastDirectionTileOnFlatTerrain();

		CGlbGlobeTerrainTile* GetNorthNeighbourInTileRange(glbDouble west, glbDouble east, glbDouble north, glbDouble south);
		CGlbGlobeTerrainTile* GetWestNeighbourInTileRange(glbDouble west, glbDouble east, glbDouble north, glbDouble south);
		CGlbGlobeTerrainTile* GetSouthNeighbourInTileRange(glbDouble west, glbDouble east, glbDouble north, glbDouble south);
		CGlbGlobeTerrainTile* GetEastNeighbourInTileRange(glbDouble west, glbDouble east, glbDouble north, glbDouble south);

		osg::ref_ptr<osg::Vec3dArray> GetNeighbourTileEdgeVecArray_Ext(CGlbGlobeTerrainTile* p_neighbour_tile,GlbTileEdgeEnum base_edge);

		BOOL IsEdgeNeedMerge(CGlbGlobeTerrainTile* p_base_tile, 
			CGlbGlobeTerrainTile* p_neighbout_tile, GlbTileEdgeEnum base_edge, osg::Vec3dArray* p_VertexArray);

		CGlbGlobeTerrainTile* GetTopMostTile(CGlbGlobeTerrainTile* p_tile);

		/*
		递归寻找精确可见区域tile
		loopdepth : 迭代深度 [默认为3]
		*/
		osg::BoundingBox AAA(glbDouble childSouth,glbDouble childNorth,glbDouble childWest,glbDouble childEast,
			glbDouble childMinHei, glbDouble childMaxHei,glbInt32 level, glbInt32 row, glbInt32 column, 
			osg::Polytope cv,glbDouble min_southDegree, glbDouble max_norhtDegree,glbInt32 loopdepth=2);

		GlbGlobeChildLocationEnum GetLocationFromLongitudeLatitude(glbDouble longOrX,glbDouble latOrY);

		// 判断domlayer与本tile是否有交集
		glbBool IsDomlayerInterTile(CGlbGlobeDomLayer* pDomlayer);
		// 判断demlayer与本tile是否有交集
		glbBool IsDemlayerInterTile(CGlbGlobeDemLayer* pDemlayer);
		// 从maxlevel插值出本tile的domlayer数据
		glbBool InterpolateDomLayerData(CGlbGlobeDomLayer* pLayer,glbInt32 maxLevel,CGlbPixelBlock* p_pixelblock);
		// 从maxlevel插值出本tile的demlayer数据
		glbBool InterpolateDemLayerData(CGlbGlobeDemLayer* pLayer,glbInt32 maxLevel,CGlbPixelBlock* p_pixelblock);
		// 递归插值生成当前块的demlayer的dem数据
		glbBool InterpolateDemDataFromDemLayer(glbFloat* demData, glbByte* dataMask,glbInt32 row, glbInt32 column, glbDouble &srcWest,glbDouble &srcEast, glbDouble &srcSouth,glbDouble &srcNorth);
		// 重新计算mpr_boundbox
		void ComputeBoundBox();

		// 递归更新edge边的顶点数组
		glbBool UpdateEdgeGeomVerttexArray(GlbTileEdgeEnum edge);

		// 重新生成四个geometry
		void RebuildElevatedMesh();

		// 计算获取离相机点距离由近到远的块排序
		glbBool GetSortedChildTiles(std::map<glbDouble,glbref_ptr<CGlbGlobeTerrainTile>>& tiles);	

	public:
		void RefreshTexture_Step1(std::vector<CGlbExtent>						&objDirtyExts,
								std::vector<CGlbExtent>						&domDirtyExts,
								std::vector<glbref_ptr<CGlbGlobeDomLayer>>  &domlayers,                   
								std::vector<glbref_ptr<CGlbGlobeRObject>>	&terrainObjs);
		void RefreshTexture_Step2(std::vector<CGlbExtent>						&objDirtyExts,
								std::vector<CGlbExtent>						&domDirtyExts,
								std::vector<glbref_ptr<CGlbGlobeDomLayer>>  &domlayers,                   
								std::vector<glbref_ptr<CGlbGlobeRObject>>	&terrainObjs);
	private:
		CGlbGlobeTerrain*	mpr_terrain;				//地形对象
		glbref_ptr<CGlbGlobeTerrainTile> mpr_nwchild;	//西北子块
		glbref_ptr<CGlbGlobeTerrainTile> mpr_nechild;	//东北子块
		glbref_ptr<CGlbGlobeTerrainTile> mpr_swchild;	//西南子块
		glbref_ptr<CGlbGlobeTerrainTile> mpr_sechild;	//东南子块
		glbref_ptr<CGlbGlobeTerrainTile> mpr_parent_tile;	//父块
		GlbGlobeChildLocationEnum mpr_location_enum;		//本块在父块中的位置
		//osg::ref_ptr<osg::MatrixTransform> mpr_node;		//osg节点
		osg::ref_ptr<osg::Group> mpr_node;				//osg节点 - switch
		osg::ref_ptr<osg::Texture2D> mpr_texture;		//地形块纹理
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
		//std::vector<osg::Vec3> mpr_surrond_pointvec;
		osg::Vec3d		mpr_local_origin;				//地形块基准点[默认为地形块的中心点]
		glbBool mpr_isinitialized;						//初始化标志
		glbBool	mpr_isLoadingData;						//正在加载数据标志
		glbBool mpr_isDataLoaded;						//标志数据是否已经加载完成
		glbInt32 mpr_level;								//等级
		glbInt32 mpr_row;								//行号
		glbInt32 mpr_column;							//列号
		//CRITICAL_SECTION mpr_critical;				//临界区
		glbByte* mpr_image;								//256*256*4 地形块dom数据
		glbInt32	mpr_imagesize;						//dom行列数
		glbByte* mpr_origin_image;						//原始地形块dom数据

		glbByte* mpr_overlayImage;						//地形dom 与DOM图层融合后的256*256*4
		osg::ref_ptr<osg::Image> mpr_p_osgImg;			//对应生成的osg::Image对象	

		GlbGlobeTypeEnum mpr_globe_type;				//globe场景类型
		glbFloat*		mpr_demdata;					//dem数据17*17
		glbFloat*		mpr_overlaydemdata;				//地形块dem 融合地形修改对象和DEM图层后的17*17
		glbInt32		mpr_demsize;					//dem行列数
		glbInt32		mpr_vertex_count;

		glbDouble		mpr_longitude_span;
		glbDouble		mpr_latitude_span;	

		// 缝边用geometry
		osg::ref_ptr<osg::Geometry> mpr_p_north_easthalf_geom;
		osg::ref_ptr<osg::Geometry> mpr_p_north_westhalf_geom;
		osg::ref_ptr<osg::Geometry> mpr_p_south_easthalf_geom;
		osg::ref_ptr<osg::Geometry> mpr_p_south_westhalf_geom;
		osg::ref_ptr<osg::Geometry> mpr_p_east_northhalf_geom;
		osg::ref_ptr<osg::Geometry> mpr_p_east_southhalf_geom;
		osg::ref_ptr<osg::Geometry> mpr_p_west_northhalf_geom;
		osg::ref_ptr<osg::Geometry> mpr_p_west_southhalf_geom;
		// 边界顶点集
		osg::ref_ptr<osg::Vec3dArray> mpr_p_north_easthalf_vertexarray;
		osg::ref_ptr<osg::Vec3dArray> mpr_p_north_westhalf_vertexarray;
		osg::ref_ptr<osg::Vec3dArray> mpr_p_south_easthalf_vertexarray;
		osg::ref_ptr<osg::Vec3dArray> mpr_p_south_westhalf_vertexarray;
		osg::ref_ptr<osg::Vec3dArray> mpr_p_east_northhalf_vertexarray;
		osg::ref_ptr<osg::Vec3dArray> mpr_p_east_southhalf_vertexarray;
		osg::ref_ptr<osg::Vec3dArray> mpr_p_west_northhalf_vertexarray;
		osg::ref_ptr<osg::Vec3dArray> mpr_p_west_southhalf_vertexarray;

		osg::ref_ptr<osg::Vec3dArray> mpr_p_center_westhalf_vertexarray;
		osg::ref_ptr<osg::Vec3dArray> mpr_p_center_easthalf_vertexarray;
		osg::ref_ptr<osg::Vec3dArray> mpr_p_center_northhalf_vertexarray;
		osg::ref_ptr<osg::Vec3dArray> mpr_p_center_southhalf_vertexarray;

		osg::ref_ptr<osg::Vec2Array> mpr_p_north_easthalf_texcoords;
		osg::ref_ptr<osg::Vec2Array> mpr_p_north_westhalf_texcoords;
		osg::ref_ptr<osg::Vec2Array> mpr_p_south_easthalf_texcoords;
		osg::ref_ptr<osg::Vec2Array> mpr_p_south_westhalf_texcoords;
		osg::ref_ptr<osg::Vec2Array> mpr_p_east_northhalf_texcoords;
		osg::ref_ptr<osg::Vec2Array> mpr_p_east_southhalf_texcoords;
		osg::ref_ptr<osg::Vec2Array> mpr_p_west_northhalf_texcoords;
		osg::ref_ptr<osg::Vec2Array> mpr_p_west_southhalf_texcoords;
		osg::ref_ptr<osg::Geode> mpr_p_geode;
		osg::ref_ptr<osg::Geometry> mpr_p_northWestgeom;
		osg::ref_ptr<osg::Geometry> mpr_p_northEastgeom;
		osg::ref_ptr<osg::Geometry> mpr_p_southEastgeom;
		osg::ref_ptr<osg::Geometry> mpr_p_southWestgeom;	

		glbref_ptr<IGlbGlobeTask>	mpr_terrainTask;

		// 测试代码
		glbInt32  mpr_opacity;

		glbBool						mpr_textureRefreshed;
	};
}