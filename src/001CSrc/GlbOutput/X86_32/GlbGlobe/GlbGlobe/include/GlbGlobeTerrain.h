/********************************************************************
* Copyright (c) 2014 北京超维创想信息技术有限公司
* All rights reserved.
*
* @file    GlbGlobeTerrain.h
* @brief   地形 头文件
*
* 这个档案定义CGlbGlobeTerrain这个class
*
* @version 1.0
* @author  马林
* @date    2014-5-8 16:10
*********************************************************************/
#pragma once

#include "GlbReference.h"
#include "GlbGlobeObject.h"
#include "GlbWString.h"
#include "GlbGlobeTerrainTile.h"
#include <osg/NodeCallback>

namespace GlbGlobe
{
	class CGlbGlobe;
	class CGlbGlobeView;
	/**
	* @brief 地形类			 
	* @version 1.0
	* @author  马林
	* @date    2014-5-8 16:10
	*/
	class GLB_DLLCLASS_EXPORT CGlbGlobeTerrain : public CGlbReference
	{
	public:
		enum GLBOperationEnum
		{// 定义操作类型
			GLB_OPENERAION_ADD = 0,
			GLB_OPENERAION_REMOVE = 1,
			GLB_OPENERAION_MODIFY = 2
		};

		enum GLBOperationObjectEnum
		{// 定义操作对象
			GLB_NODE = 0,
			GLB_DRAWABLE = 2,
			GLB_VECARRAY = 3,
			GLB_TEXTURE = 4
		};
		/** 
		* @brief 地形块修改信息结构体
		* @note 地形内部结构,供地形块使用
		*/
		struct GlbTileOperation
		{
		public:	
			/**
			* @brief 结构体构造函数
			* @param addordelormodity 操作类型. 0删除, 1添加, 2修改
			* @param nodeORdrawableORvecarray 对象类型. 0节点, 1可绘制对象, 2顶点数组
			* @param p_tile_node 节点
			* @param p_drawable 可绘制对象
			* @param p_vecarray 顶点数组 
			* @return  无	
			*/
			GlbTileOperation(	GLBOperationEnum addordelormodity, 
				GLBOperationObjectEnum operation, 
				osg::Group* p_tile_node,
				double compiletime=0.0);

			GlbTileOperation(	GLBOperationEnum addordelormodity, 
				GLBOperationObjectEnum operation, 
				osg::Geode* p_tile_geode_node,
				osg::Drawable* p_drawable=NULL, 
				osg::Vec3Array* p_vecarray=NULL);

			GlbTileOperation ( GLBOperationEnum addordelormodity, 
				GLBOperationObjectEnum operation,
				osg::Texture* p_texture = NULL,
				osg::Image* p_image = NULL);
			virtual ~GlbTileOperation();
		public:
			GLBOperationEnum _addordelormodify;				///< 0 - add; 1 - delete	; 2 - modify
			GLBOperationObjectEnum _nodeordrawableorvecarrayortexture;		///< 0 - node ; 1 - drawable	; 2 - vec3array ; 3 - texture
			osg::ref_ptr<osg::Group> _p_tile_node;			// 节点
			osg::ref_ptr<osg::Geode> _p_tile_geode_node;	///< 叶子节点
			osg::ref_ptr<osg::Drawable> _p_drawable; ///< 可绘制对象
			osg::ref_ptr<osg::Vec3Array> _p_vecarray; ///< 顶点数组
			double _node_compiletime;						///< 编译_p_tile_node需要的时间
			osg::ref_ptr<osg::Texture>	_p_texture;
			osg::ref_ptr<osg::Image>	_p_image;
		};
	private:
		/** 
		* @brief 地形osg节点更新回调类
		* @author 马林
		* @date    2014-5-12 11:10
		* @note 地形内部类,负责摘挂节点,drawable
		*/
		class CGlbGlobeTerrainCallBack : public osg::NodeCallback
		{
		public:
			/**
			* @brief 地形osg节点回调类构造函数
			* @param [in] pGlobeTerrain 地形对象	  
			* @return  无
			*/
			CGlbGlobeTerrainCallBack(CGlbGlobeTerrain* pGlobeTerrain); 	
			/**
			* @brief 地形osg节点回调实现
			* @param [in] node 地形的osg节点
			* @param [in] nv osg节点访问器	  
			* @return  无
			*/
			virtual void operator() (osg::Node* node,osg::NodeVisitor* nv);
		public:
			CGlbGlobeTerrain* m_p_globe_terrian;
		};
	public:
		/**
		* @brief 地形构造函数
		* @param [in] globe 地形所属场景	  
		* @return  无
		*/
		CGlbGlobeTerrain(CGlbGlobe* globe);
		/**
		* @brief 地形析构函数	   
		* @return  无
		*/
		~CGlbGlobeTerrain(void);

		/**
		* @brief 设置地形数据集
		* @param [in] tdataset 地形数据集	  
		* @return  成功返回true
		-	 失败返回false
		*/
		glbBool SetTerrainDataset(IGlbTerrainDataset* tdataset);
		/**
		* @brief 获取地形数据集 
		* @return  成功返回地形数据集指针
		-	 失败返回NULL
		*/
		IGlbTerrainDataset* GetTerrainDataset();
		/**
		* @brief 地形初始化	   
		* @return  成功返回true
		-	 失败返回false
		*/
		glbBool Initialize();
		/**
		* @brief 获取地形是否初始化	   
		* @return  已初始化返回true
		-	 没有初始化返回false
		*/
		glbBool IsInitialized();
		/**
		* @brief 地形更新
		* @param [in] domlayers dom图层集合
		* @param [in] demlayers dem图层集合
		* @param [in] terrainobjs 贴地形绘制对象集合
		* @param [in] mTerrainobjs 地形修改对象集合
		* @return  无
		*/
		void Update(std::vector<glbref_ptr<CGlbGlobeDomLayer>> &domlayers,
			std::vector<glbref_ptr<CGlbGlobeDemLayer>> &demlayers,
			std::vector<glbref_ptr<CGlbGlobeRObject>>  &terrainobjs, 
			std::vector<glbref_ptr<CGlbGlobeRObject>>  &mTerrainobjs);
		/**
		* @brief 地形销毁  
		* @return  无
		*/
		void Dispose();
		/**
		* @brief 添加地形修改操作
		* @param [in] opera 地形修改操作	  
		* @return 无
		*/
		void AddOperation(GlbTileOperation opera);
		/**
		* @brief 添加需要打包处理的地形修改操作
		* @param [in] opera 地形修改操作	  
		* @return 无
		*/
		void AddPackageOperation(GlbTileOperation opera);
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
		glbBool ScreenToTerrainCoordinate(glbInt32 x,glbInt32 y,
			glbDouble &lonOrx,glbDouble &latOry,glbDouble &altOrz);
		/**
		* @brief 计算射线是否与地形相交      
		*	@param [in] ln_pt1射线上一点(端点)
		* @param [in] ln_dir射线方向
		* @param [out] InterPos 如果相交，交点坐标	
		* @return 相交返回true
		*		-	不相交返回false
		*/
		glbBool IsRayInterTerrain(osg::Vec3d ln_pt1, osg::Vec3d ln_dir, osg::Vec3d &InterPos);
		/**
		* @brief 地形osg节点的更新回调
		* @param [in] node osg节点
		* @param [in] nv osg节点访问器
		* @return  成功返回S_OK,S_FALSE
		-	 失败返回E_FAIL,E_POINTER
		*/
		HRESULT UpdateOsgCallBack(osg::Node* node,osg::NodeVisitor* nv);
		/**
		* @brief 刷新地形纹理影像
		* @param [in] objDirtyExts 需要刷新的贴地形对象区域
		* @param [in] domDirtyExts 需要刷新的dom图层区域
		* @param [in] domlayers	dom图层集合
		* @param [in] terrainObjs 贴地形绘制的对象集合
		* @note 本接口与dom图层和贴地形绘制对象相关.
		- GLB_GLOBETYPE_GLOBE球模式下 为地理坐标 单位：度
		- GLB_GLOBETYPE_FLAT平面模式下 为世界坐标 单位：米
		* @return  无 			
		*/
		void RefreshTexture(std::vector<CGlbExtent>                    &objDirtyExts,
			std::vector<CGlbExtent>                    &domDirtyExts,
			std::vector<glbref_ptr<CGlbGlobeDomLayer>> &domlayers,                   
			std::vector<glbref_ptr<CGlbGlobeRObject>>  &terrainObjs );
		/**
		* @brief 刷新地形高程数据
		* @param [in] mtobjDirtyExts 需要刷新的地形修改对象区域
		* @param [in] demDirtyExts 需要刷新的dem图层区域
		* @param [in] demlayers	dem图层集合
		* @param [in] mterrainObjs 地形修改对象集合
		* @note 本接口与dem图层和地形修改对象相关.
		- GLB_GLOBETYPE_GLOBE球模式下 为地理坐标 单位：度
		- GLB_GLOBETYPE_FLAT平面模式下 为世界坐标 单位：米
		* @return  无 			
		*/
		void RefreshDem(std::vector<CGlbExtent>                      &mtobjDirtyExts,
			std::vector<CGlbExtent>                      &demDirtyExts,
			std::vector<glbref_ptr<CGlbGlobeDemLayer>>   &demlayers,                   
			std::vector<glbref_ptr<CGlbGlobeRObject>>    &mterrainObjs );
		/**
		* @brief 设置地形不透明度
		* @param [in] domlayers	dom图层集合
		* @param [in] terrainObjs 贴地形绘制的对象集合
		* @param [in] opacity 地形不透明度	  
		* @return  成功返回true
		-	 失败返回false
		*/
		glbBool SetOpacity(	std::vector<glbref_ptr<CGlbGlobeDomLayer>> &domlayers,                   
			std::vector<glbref_ptr<CGlbGlobeRObject>>  &terrainObjs,
			glbInt32 opacity);
		/**
		* @brief 获取地形不透明度  
		* @return 不透明度
		*/
		glbInt32 GetOpacity();
		/**
		* @brief 设置地形夸张系数
		* @param [in] exaggrate 地形夸张系数	  
		* @return  成功返回true
		-	 失败返回false
		*/
		glbBool SetExaggrate(glbDouble exaggrate);
		/**
		* @brief 获取地形夸张系数
		* @return 地形夸张系数
		*/
		glbDouble  GetExaggrate();
		/**
		* @brief 设置地形显隐
		* @param [in] visible 地形显隐 显示为true,隐藏为false	  
		* @return  成功返回true
		-	 失败返回false
		*/
		glbBool SetVisible(glbBool visible);
		/**
		* @brief 获取地形显隐
		* @return 地形显隐 
		- 显示为true,隐藏为false	  
		*/
		glbBool GetVisible();
		/**
		* @brief 获取地形节点
		* @return 地形节点   
		*/
		osg::Node* GetNode();
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
		* @brief 通知相机在地面上或地面下
		* @param [in] underground 地面上或地面下. 地面上为false，地面下为true
		* @return 无
		*/
		void NotifyCameraIsUnderground(glbBool underground);
		/**
		* @brief 获取地形范围
		* @param [out] west 西边界	  
		* @param [out] east 东边界
		* @param [out] south 南边界
		* @param [out] north 北边界
		* @return  成功返回true
		-	 失败返回false
		*/
		glbBool GetExtent(glbDouble &west, glbDouble &east, glbDouble &south , glbDouble &north);
		/**
		* @brief 计算地面可见区域
		* @param [out] extent_one 可见区域1	  
		* @param [out] extent_two 可见区域2
		* @param [in] min_southDegree 需要计算的南纬起始角度 (默认为-66.5度)
		* @param [in] max_norhtDegree 需要计算的北纬结束角度 (默认为66.5度)
		* @return  成功返回true
		-	 失败返回false
		*/
		glbBool  ComputeVisibleExtents(CGlbExtent& extent_one, CGlbExtent& extent_two,
			glbDouble min_southDegree=-66.5, glbDouble max_norhtDegree=66.5);
		/**
		* @brief 地形启动或关闭背面剔除
		* @param [in] enablecull 背面剔除 启动为true,关闭为false	  
		* @return  成功返回true
		-	 失败返回false
		*/
		void EnableCullFace(glbBool enablecull);
		/**
		* @brief 地形更新启动或关闭
		* @param [in] enableupdate 地形更新 启动为true,关闭为false	  
		* @return  成功返回true
		-	 失败返回false
		*/
		void EnableUpdate(glbBool enableupdate);
		/**
		* @brief 地形更新是否启动	 	  
		* @return  启动中返回true
		-	 停止了返回false
		*/
		glbBool IsUpdateEnable();
		/**
		* @brief 获取当前场景视图 
		* @return  成功返回视图对象指针
		-	 失败返回NULL
		*/
		CGlbGlobeView* GetView();
		/**
		* @brief 获取GLOBE模式下0级的分块度数[默认为36度]
		* @return  0级分块度数
		*/
		glbInt32 GetLevelZeroTileSizeOfDegrees();
		/**
		* @brief 获取0级对应key的tile[GLOBE模式]
		* @return  0级地形块
		*/
		CGlbGlobeTerrainTile* GetTerrainLevelZeroTile(glbInt32 key);
		/**
		* @brief 获取p_tile所属的顶级块的北边相邻块
		* @param [in] p_tile 地形块	 
		* @return  地形块对象指针
		*/
		CGlbGlobeTerrainTile* GetTopmostNorthNeibourTile(CGlbGlobeTerrainTile* p_tile);
		/**
		* @brief 获取p_tile所属的顶级块的南边相邻块
		* @param [in] p_tile 地形块	 
		* @return  地形块对象指针
		*/
		CGlbGlobeTerrainTile* GetTopmostSouthNeibourTile(CGlbGlobeTerrainTile* p_tile);
		/**
		* @brief 获取p_tile所属的顶级块的西边相邻块
		* @param [in] p_tile 地形块	 
		* @return  地形块对象指针
		*/
		CGlbGlobeTerrainTile* GetTopmostWestNeibourTile(CGlbGlobeTerrainTile* p_tile);
		/**
		* @brief 获取p_tile所属的顶级块的东边相邻块
		* @param [in] p_tile 地形块	 
		* @return  地形块对象指针
		*/
		CGlbGlobeTerrainTile* GetTopmostEastNeibourTile(CGlbGlobeTerrainTile* p_tile);
		/**
		* @brief 反序列化对象，解析xml获取对象属性值
		* @param[in]  node libXML2节点
		* @param[in]  prjPath 工程文件目录
		* @return 反序列化成功返回true、反之返回false
		*/
		glbBool Load(xmlNodePtr node,const glbWChar* prjPath=NULL);
		/**
		* @brief 序列化对象，序列化对象属性到xml工程文件
		* @param[in]  node libXML2节点
		* @param[in]  prjPath 工程文件目录
		* @return 序列化成功返回true、反之返回false
		*/
		glbBool Save(xmlNodePtr node,const glbWChar* prjPath=NULL);
		 /*
		  * @brief 添加摄影测量图层对象
		  * @param [in] layer 摄影测量图层对象
		  * @return 无 
		  */
		void AddTiltPhotographLayer(CGlbGlobeLayer* layer);
		 /*
		  * @brief 删除摄影测量图层对象
		  * @param [in] layer 摄影测量图层对象
		  * @return 成功返回true，失败返回false 
		  */
		glbBool RemoveTiltPhotographLayer(CGlbGlobeLayer* layer);
		 /*
		  * @brief  计算两点之间的地面距离
		  * @param [in] xOrLonStart, yOrLatStart 起始点x,y或经纬度
						xOrLonEnd,yOrLatEnd 终止点x,y或经纬度
						step 计算精度(步长)
		  * @return  两点之间的地面距离
		  */
		glbDouble ComputeOnTerrainDistance(glbDouble xOrLonStart, glbDouble yOrLatStart, glbDouble xOrLonEnd, glbDouble yOrLatEnd, glbDouble step);

		/*
		  * @brief 判断是否需要停止Tile的Update		  
		  * @return 需要停止返回true，不需要返回false
		  * @note 目前平面模式下会使用，结束条件是Add node的数量超过阈值（5-10）
		  */
		glbBool IsUpdateNeedTerminate();

		//测试接口 启动（关闭）线框模式
		void EnableWireMode(glbBool wiremode);
		glbBool IsWireMode();
private:
	void UpdateGlobeTerrain(std::vector<glbref_ptr<CGlbGlobeDomLayer>> domlayers,
		std::vector<glbref_ptr<CGlbGlobeDemLayer>> demlayers,
		std::vector<glbref_ptr<CGlbGlobeRObject>> terrainobjs, 
		std::vector<glbref_ptr<CGlbGlobeRObject>> mTerrainobjs);
	void UpdateFlatTerrain(std::vector<glbref_ptr<CGlbGlobeDomLayer>> domlayers,
		std::vector<glbref_ptr<CGlbGlobeDemLayer>> demlayers,
		std::vector<glbref_ptr<CGlbGlobeRObject>> terrainobjs, 
		std::vector<glbref_ptr<CGlbGlobeRObject>> mTerrainobjs);
	void RemoveInvisibleTiles();
	glbInt32 GetRowFromLatitude(glbDouble latitude, glbDouble tileSize);
	glbInt32 GetColFromLongitude(glbDouble longitude, glbDouble tileSize);
	glbBool IsIntersect(CGlbExtent* src_extent, std::vector<CGlbExtent>& target_extents);
	glbBool GetSortedTopmostTiles(std::map<glbInt32,glbref_ptr<CGlbGlobeTerrainTile>> tiles, osg::Vec3d camerapos, std::map<glbDouble,glbref_ptr<CGlbGlobeTerrainTile>>& sorted_tiles);

	// 如果(lonOrX, latOrY）在倾斜摄影图层范围内，计算该点处的高度，成功返回true，失败返回false
	// 如果(lonOrX, latOrY）不在倾斜摄影图层范围内，返回false
	glbBool GetElevationAtFromTiltPhotographLayer(glbDouble lonOrX, glbDouble latOrY, glbDouble &elevtion);
private:	
	glbref_ptr<IGlbTerrainDataset> mpr_tdataset;						///<地形数据集
	glbBool	mpr_isInitialized;											///<地形初始化标志
	glbDouble mpr_south;												///<南边界
	glbDouble mpr_north;												///<北边界
	glbDouble mpr_west;													///<西边界
	glbDouble mpr_east;													///<东边界
	glbDouble mpr_lzts;													///<0级分块范围	球模式：单位是度
	glbBool mpr_enableUpdate;											///<开启/停止 地形更新
	glbBool	mpr_isLinemode;												///<线框模式	
	CRITICAL_SECTION mpr_osgcritical;									///<osg节点回调、添加修改任务、取高程 互斥
	osg::ref_ptr<osg::Switch> mpr_node;									///<地形osg节点
	std::map<glbInt32,glbref_ptr<CGlbGlobeTerrainTile>> mpr_topmost_tiles;	///<0级地形块集合mpr_lztiles , 0级块索引	
	glbInt32 mpr_opacity;												///<地形透明度
	glbDouble mpr_exaggrate;											///<地形夸张系数
	glbBool mpr_visible;												///<地形显隐
	glbBool mpr_enablecullface;											///<背面剔除【开启或关闭】
	
	CRITICAL_SECTION mpr_tilebuffer_exchange_criticalsection;			///<数据交换临界区
	std::vector<GlbTileOperation> mpr_operations;						///<操作集合
	std::vector<GlbTileOperation> mpr_operations_buffer;				///<操作集合缓冲		

	std::vector<GlbTileOperation> mpr_packageOperations;				///<需要一次性处理的操作集合
	std::vector<GlbTileOperation> mpr_packageOperations_buffer;			///<需要一次性处理的操作集合缓冲

	//glbDouble mpr_levelzero_tilesize_degrees;							///<0级分块大小
	glbInt32 mpr_levelzero_tiles_columns;								///<0级分块列数
	glbInt32 mpr_levelzero_tiles_rows;									///<0级分块行数
	glbBool	mpr_is_camera_underground;									///<记录相机是否在地面下

	std::vector<glbref_ptr<CGlbGlobeLayer>> mpr_tilt_photography_layers;///<倾斜摄影图层集合
	//osg::Timer_t mpr_prebufferchange_time;							///<上一次operationbuffer交换的时刻
	glbBool	mpr_bOpacityChanged;										///<废弃 discard

	glbInt32 mpr_addOperationCount;										///<需要处理的添加节点操作数量
public:
	//CRITICAL_SECTION mpr_critical;									//设置数据集/数据集读取数据互斥
	CRITICAL_SECTION mpr_tDatasetAccessCritical;						//设置地形数据集/读取地形数据集数据互斥

	GlbGlobeTypeEnum mpr_globe_type;									//场景类型
	CGlbGlobe* mpr_globe;												//globe场景对象
	};
}