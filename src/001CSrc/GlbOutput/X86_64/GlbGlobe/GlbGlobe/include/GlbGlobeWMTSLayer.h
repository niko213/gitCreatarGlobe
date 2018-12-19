/********************************************************************
* Copyright (c) 2014 北京超维创想信息技术有限公司
* All rights reserved.
*
* @file    GlbGlobeWMTSLayer.h
* @brief   网络瓦片地图图层头文件
*
* 这个档案定义CGlbGlobeWMTSLayer这个class
*
* @version 1.0
* @author  马林
* @date    2017-6-1 10:40
*********************************************************************/
#pragma once
#include "GlbGlobeExport.h"
#include "GlbGlobeLayer.h"
#include "GlbGlobeWMTSTile.h"
#include "libxml/tree.h"

namespace GlbGlobe
{

	class CGlbGlobe;
	class CGlbGlobeView;
	/**
	* @brief 网络瓦片地图图层类
	* @note 描述和渲染WMTS网络瓦片数据
	*/
	class GLB_DLLCLASS_EXPORT CGlbGlobeWMTSLayer : public CGlbGlobeLayer
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
			GLB_DRAWABLE = 1,
			GLB_VECARRAY = 2,
			GLB_TEXTURE = 3
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
		class CGlbGlobeWMTSCallBack : public osg::NodeCallback
		{
		public:
			/**
			* @brief 地形osg节点回调类构造函数
			* @param [in] pGlobeLayer wmts图层对象  
			* @return  无
			*/
			CGlbGlobeWMTSCallBack(CGlbGlobeWMTSLayer* pGlobeLayer); 	
			/**
			* @brief 地形osg节点回调实现
			* @param [in] node 地形的osg节点
			* @param [in] nv osg节点访问器	  
			* @return  无
			*/
			virtual void operator() (osg::Node* node,osg::NodeVisitor* nv);
		public:
			CGlbGlobeWMTSLayer* p_wmtslayer;
		};
	public:
		/**
		* @brief WMTS图层构造函数
		* @return 无
		*/
		CGlbGlobeWMTSLayer(void);
		/**
		* @brief WMTS图层析构函数
		* @return 无
		*/
		~CGlbGlobeWMTSLayer(void);

		 /* 基类接口 */
		/**
		* @brief 获取图层名称
		* @return 图层名称 
		*/
		const glbWChar* GetName();
		/**
		* @brief 设置图层名称
		* @param name 图层名称
		* @return  成功返回true
		-	 失败返回false
		*/
		glbBool SetName(glbWChar* name);
		/**
		* @brief 获取图层类型
		* @return  图层类型
		*/
		GlbGlobeLayerTypeEnum GetType();
		/**
		* @brief 图层序列化（加载）
		* @param [in] node xml读取节点
		* @param [in] relativepath 相对路径
		* @return  成功返回true
		-	 失败返回false
		*/
		glbBool   Load(xmlNodePtr node, glbWChar* relativepath );
		glbBool   Load2(xmlNodePtr node, glbWChar* relativepath );
		/**
		* @brief 图层序列化（保存）
		* @param [in] node xml写入节点
		* @param [in] relativepath 相对路径
		* @return  成功返回true
		-	 失败返回false
		*/
		glbBool   Save(xmlNodePtr node, glbWChar* relativepath );
		/**
		* @brief 设置图层显示或隐藏
		* @param [in] show 显隐  显示为true，隐藏为false
		* @param [in] isOnState true表示在可见区域  false表示不在可见区域
		* @return  成功返回true
		-	 失败返回false
		*/
		glbBool Show(glbBool isShow,glbBool isOnState=false);
		/**
		* @brief 设置图层不透明度
		* @param [in] opacity 不透明度(0-100)
		-	0表示全透明
		-	100表示不透明
		* @return  成功返回true
		-	 失败返回false
		*/
		glbBool SetOpacity(glbInt32 opacity);
		/**
		* @brief 获取图层范围
		* @param [in] isWorld 是否是世界坐标，如果是世界坐标，以米为单位;如果不是世界坐标，则使用地理坐标，以度为单位				
		* @return  图层范围
		*/
		glbref_ptr<CGlbExtent> GetBound(glbBool isWorld);

		glbBool GetExtent(glbDouble &west, glbDouble &east, glbDouble &south, glbDouble &north);

		/* WMTSLayer 接口 */
		/* 
		* @brief 设置高程模式
		* @param [in] mode 高程模式
		* @return 成功返回true
		-	 失败返回false
		*/
		glbBool SetAltitudeMode(GlbAltitudeModeEnum mode); 
		/**
		* @brief 获取高程模式					
		* @return  高程模式
		*/
		GlbAltitudeModeEnum GetAltitudeMode();

		/* 
		* @brief 设置绝对模式下的高程Z值
		* @param [in] altitude 高程Z值
		* @return 成功返回true
		-	 失败返回false
		*/
		glbBool SetAltitudeOrZ(glbDouble altitude);
		/**
		* @brief 获取高程Z值					
		* @return  高程Z值
		*/
		glbDouble GetAltitudeOrZ();
		/**
		* @brief 图层更新		 
		*/
		void Update();

		/**
		* @brief 地形osg节点的更新回调
		* @param [in] node osg节点
		* @param [in] nv osg节点访问器
		* @return  成功返回S_OK,S_FALSE
		-	 失败返回E_FAIL,E_POINTER
		*/
		HRESULT UpdateOsgCallBack(osg::Node* node,osg::NodeVisitor* nv);

		/**
		* @brief 图层初始化	   
		* @return  成功返回true
		-	 失败返回false
		*/
		glbBool Initialize();

		glbBool IsInitialized(){return mpr_isInitialized;}
		/**
		* @brief 图层销毁  
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
		CGlbGlobeWMTSTile* GetWMTSLevelZeroTile(glbInt32 key);
		/**
		* @brief 获取p_tile所属的顶级块的北边相邻块
		* @param [in] p_tile 地形块	 
		* @return  WMTS瓦片对象指针
		*/
		//CGlbGlobeWMTSTile* GetTopmostNorthNeibourTile(CGlbGlobeWMTSTile* p_tile);
		/**
		* @brief 获取p_tile所属的顶级块的南边相邻块
		* @param [in] p_tile 地形块	 
		* @return  WMTS瓦片对象指针
		*/
		//CGlbGlobeWMTSTile* GetTopmostSouthNeibourTile(CGlbGlobeWMTSTile* p_tile);
		/**
		* @brief 获取p_tile所属的顶级块的西边相邻块
		* @param [in] p_tile 地形块	 
		* @return  WMTS瓦片对象指针
		*/
		//CGlbGlobeWMTSTile* GetTopmostWestNeibourTile(CGlbGlobeWMTSTile* p_tile);
		/**
		* @brief 获取p_tile所属的顶级块的东边相邻块
		* @param [in] p_tile 地形块	 
		* @return  WMTS瓦片对象指针
		*/
		//CGlbGlobeWMTSTile* GetTopmostEastNeibourTile(CGlbGlobeWMTSTile* p_tile);

		/**
		* @brief 获取某级瓦片的行数
		* @param [in] level 级数	 
		* @return  此级WMTS瓦片列数
		*/
		glbInt32 GetTileRowsOfLevel(glbInt32 level);
		/**
		* @brief 获取某级瓦片的列数
		* @param [in] level 级数	 
		* @return  此级WMTS瓦片列数
		*/
		glbInt32 GetTileColumnsOfLevel(glbInt32 level);		

		/**
		* @brief 获取瓦片key值
		* @param [in]   level 级数
				-		row 行号
				-		column 列号
		* @return  key值
		*/
		glbInt32 ComputeTileKey(glbInt32 level, glbInt32 row, glbInt32 column);

		/**
		* @brief 通知有新加瓦片
		* @param [in] pTile 瓦片对象	 
		* @return  成功返回true
		-	 失败返回false
		*/		
		glbBool NotifyTileAdd(CGlbGlobeWMTSTile* pTile);
		
		/**
		* @brief 通知有瓦片被移除
		* @param [in] pTile 瓦片对象	 
		* @return  成功返回true
		-	 失败返回false
		*/	
		glbBool NotifyTileRemove(CGlbGlobeWMTSTile* pTile);

		/**
		* @brief 根据level，row，column查找tile
		* @param [in] level 瓦片等级
					- row	瓦片行号
					- column瓦片列号
		* @return  WMTS瓦片对象指针
		*/
		CGlbGlobeWMTSTile* FindTile(glbInt32 level, glbInt32 row, glbInt32 column);
		/**
		* @brief 获取WMTS服务的瓦片数据最小级数		
		* @return  瓦片最小级数
		*/
		glbInt32	GetMinLevel();
		/**
		* @brief 获取WMTS服务的瓦片数据最大级数		
		* @return  瓦片最大级数
		*/
		glbInt32	GetMaxLevel();
		/*
		* @brief 设置图层偏移
		* @param [in] xOffset x偏移
		*		 [in] yOffset y偏移
		*		 [in] zOffset z偏移
		* @return 无
		*/
		void SetLayerOffset(glbDouble xOffset, glbDouble yOffset, glbDouble zOffset);
		/*
		* @brief 获取图层偏移
		* @param [out] xOffset x偏移
		*		 [out] yOffset y偏移
		*		 [out] zOffset z偏移
		* @return 无
		*/
		void GetLayerOffset(glbDouble& xOffset, glbDouble& yOffset, glbDouble& zOffset);
	private:
		void UpdateGlobe();
		void UpdateFlat();
		void RemoveInvisibleTiles();
		glbBool GetSortedTopmostTiles(std::map<glbInt32,glbref_ptr<CGlbGlobeWMTSTile>> tiles, osg::Vec3d camerapos, std::map<glbDouble,glbref_ptr<CGlbGlobeWMTSTile>>& sorted_tiles);
		void UpdateRelativeElevationPos();
	private:
		glbref_ptr<CGlbExtent>	mpr_extent;				///<图层外包
		GlbAltitudeModeEnum     mpr_altitudeMode;		///<高程模式
		glbDouble				mpr_altitudeOrZ;		///<绝对模式时的高程
		glbBool					mpr_isInitialized;		///<是否初始化
		glbDouble				mpr_south;				///<南边界
		glbDouble				mpr_north;				///<北边界
		glbDouble				mpr_west;				///<西边界
		glbDouble				mpr_east;				///<东边界
		glbDouble				mpr_lzts;				///<顶级的尺寸
		CRITICAL_SECTION		mpr_osgcritical;		///<osg节点回调、添加修改任务、取高程 互斥
		std::map<glbInt32,glbref_ptr<CGlbGlobeWMTSTile>> mpr_topmost_tiles;	///<0级地形块集合mpr_lztiles , 0级块索引	
		CRITICAL_SECTION mpr_tilebuffer_exchange_criticalsection;			///<数据交换临界区
		std::vector<GlbTileOperation> mpr_operations;						///<操作集合
		std::vector<GlbTileOperation> mpr_operations_buffer;				///<操作集合缓冲		
		glbBool					mpr_enableUpdate;							///<图层更新
		glbInt32				mpr_levelzero_tiles_columns;				///<0级分块列数
		glbInt32				mpr_levelzero_tiles_rows;					///<0级分块行数

		glbInt32				mpr_minLevel;					///<最小等级
		glbInt32				mpr_maxLevel;					///<最大等级
		
		///<所有瓦片集合，只加不删，瓦片Dispose（顶级瓦片除外）后，itr->second->second=NULL,key的值为瓦片row*此级瓦片列数+瓦片column
		std::map<glbInt32/*level*/,std::map<glbInt32/*key=row*cols+col*/, glbref_ptr<CGlbGlobeWMTSTile>>> mpr_all_tiles;

		glbBool					mpr_isDispShow;

		glbDouble				mpr_xOffset;
		glbDouble				mpr_yOffset;
		glbDouble				mpr_zOffset;
	public:
		GlbGlobeTypeEnum		mpr_globe_type;								///<场景类型
	};
}

