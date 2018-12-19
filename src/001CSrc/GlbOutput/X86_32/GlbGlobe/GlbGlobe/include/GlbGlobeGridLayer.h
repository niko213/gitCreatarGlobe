/********************************************************************
* Copyright (c) 2014 北京超维创想信息技术有限公司
* All rights reserved.
*
* @file    GlbGlobeGridLayer.h
* @brief   格网图层头文件
*
* 这个档案定义CGlbGlobeGridLayer这个class
*
* @version 1.0
* @author  马林
* @date    2018-1-9 13:40
*********************************************************************/
#pragma once
#include "GlbGlobeExport.h"
#include "glbglobelayer.h"
#include "GlbGlobeRObject.h"
#include "GlbGlobeObjectRenderInfos.h"
#include "GlbGlobeGridTile.h"

namespace GlbGlobe
{
	class CGlbGlobe;
	class CGlbGlobeView;
	class GLB_DLLCLASS_EXPORT CGlbGlobeGridLayer :	public CGlbGlobeLayer
	{
	public:
		enum GLBOperationEnum
		{// 定义操作类型
			GLB_OPENERAION_ADD = 0,
			GLB_OPENERAION_REMOVE = 1
			//GLB_OPENERAION_MODIFY = 2
		};

		enum GLBOperationObjectEnum
		{// 定义操作对象
			GLB_NODE = 0,
			GLB_DRAWABLE = 2
			//GLB_VECARRAY = 3,
			//GLB_TEXTURE = 4
		};
		/** 
		* @brief 格网块修改信息结构体
		* @note 格网内部结构,供格网块使用
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
			GlbTileOperation(	GLBOperationEnum addordel, 
				GLBOperationObjectEnum operation, 
				osg::Group* p_tile_node,
				double compiletime=0.0);

			GlbTileOperation(	GLBOperationEnum addordel, 
				GLBOperationObjectEnum operation, 
				osg::Geode* p_tile_geode_node,
				osg::Drawable* p_drawable=NULL);
			virtual ~GlbTileOperation();
		public:
			GLBOperationEnum _addordel;				///< 0 - add; 1 - delete	; 2 - modify
			GLBOperationObjectEnum _nodeordrawable;		///< 0 - node ; 1 - drawable	; 2 - vec3array ; 3 - texture
			osg::ref_ptr<osg::Group> _p_tile_node;			// 节点
			osg::ref_ptr<osg::Geode> _p_tile_geode_node;	///< 叶子节点
			osg::ref_ptr<osg::Drawable> _p_drawable; ///< 可绘制对象
			//osg::ref_ptr<osg::Vec3Array> _p_vecarray; ///< 顶点数组
			double _node_compiletime;						///< 编译_p_tile_node需要的时间
			//osg::ref_ptr<osg::Texture>	_p_texture;
			//osg::ref_ptr<osg::Image>	_p_image;
		};
	private:
		/** 
		* @brief 格网osg节点更新回调类
		* @author 马林
		* @date    2018-7-30 11:10
		* @note 格网内部类,负责摘挂节点,drawable
		*/
		class CGlbGlobeGridCallBack : public osg::NodeCallback
		{
		public:
			/**
			* @brief 地形osg节点回调类构造函数
			* @param [in] pGlobeTerrain 地形对象	  
			* @return  无
			*/
			CGlbGlobeGridCallBack(CGlbGlobeGridLayer* pGridLayer); 	
			/**
			* @brief 地形osg节点回调实现
			* @param [in] node 地形的osg节点
			* @param [in] nv osg节点访问器	  
			* @return  无
			*/
			virtual void operator() (osg::Node* node,osg::NodeVisitor* nv);
		public:
			CGlbGlobeGridLayer* m_p_globe_gridlayer;
		};
	public:
		/**
		* @brief grid图层构造函数
		* @return 无
		*/
		CGlbGlobeGridLayer(void);
		/**
		* @brief grid图层析构函数
		* @return 无
		*/
		~CGlbGlobeGridLayer(void);
		/*基类接口*/
		/**
		* @brief 获取图层名称
		* @return 图层名称 
		*/
		const glbWChar* GetName();
		/**
		* @brief 设置图层名称
		* @param [in] name 图层名称
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
		glbBool   Load( xmlNodePtr node, glbWChar* relativepath );
		glbBool   Load2( xmlNodePtr node, glbWChar* relativepath );
		/**
		* @brief 图层序列化（保存）
		* @param [in]node xml写入节点
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
		glbBool Show(glbBool show,glbBool isOnState=false);
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

		/*类接口*/
		/**
		* @brief 设置渲染描述信息
		* @param [in] rinfo 渲染描述信息
		* @return  成功返回true
		-	 失败返回false
		*/
		glbBool SetRenderInfo(GlbRenderInfo* rinfo);
		/**
		* @brief 获取渲染描述信息
		* @return 渲染描述信息
		*/
		GlbRenderInfo* GetRenderInfo();
		// 切片接口
		/**
		* @brief 对格网进行线切片
		* @param [in] geo 折线		
		*/
		void ClipMap(IGlbGeometry* geo);
		/**
		* @brief 对格网进行I，J方向（X,Y方向）切片
		* @param [in] I,J 切片所在位置		
		*/
		void ClipMap(glbInt32 I,glbInt32 J);
		/**
		* @brief 对格网进行栅栏切片
		* @param [in] INum,JNum --I,J 方向切片数量		
		*/
		void ClipBarrierMap(glbInt32 INum, glbInt32 JNum);
		/**
		* @brief 关闭切片
		*/
		void Restore();

		// 图层更新
		void Update();
		/**
		* @brief 获取格网范围
		* @param [out] west 西边界	  
		* @param [out] east 东边界
		* @param [out] south 南边界
		* @param [out] north 北边界
		* @return  成功返回true
		-	 失败返回false
		*/
		glbBool GetExtent(glbDouble &west, glbDouble &east, glbDouble &south , glbDouble &north);
		/**
		* @brief 销毁  
		* @return  无
		*/
		void Dispose();
		/**
		* @brief 获取当前场景视图 
		* @return  成功返回视图对象指针
		-	 失败返回NULL
		*/
		CGlbGlobeView* GetView();
		/**
		* @brief 添加地形修改操作
		* @param [in] opera 地形修改操作	  
		* @return 无
		*/
		void AddOperation(GlbTileOperation opera);
		/**
		* @brief 计算射线是否与格网相交      
		*	@param [in] ln_pt1射线上一点(端点)
		* @param [in] ln_dir射线方向
		* @param [out] InterPos 如果相交，交点坐标	
		* @return 相交返回true
		*		-	不相交返回false
		*/
		glbBool IsRayInterGrid(osg::Vec3d ln_pt1, osg::Vec3d ln_dir, osg::Vec3d &InterPos);
		/**
		* @brief 地形osg节点的更新回调
		* @param [in] node osg节点
		* @param [in] nv osg节点访问器
		* @return  成功返回S_OK,S_FALSE
		-	 失败返回E_FAIL,E_POINTER
		*/
		HRESULT UpdateOsgCallBack(osg::Node* node,osg::NodeVisitor* nv);

		/**
		* @brief 获取tile最大的level
		* @return  最大的level
		*/
		glbInt32 GetMaxLevel();

		/**
		* @brief 单元格拾取
		* @param [in] mousex	鼠标点x坐标
		*		 [in] mousey	鼠标点y坐标
		* @param [out] tilelevel, tileRow, tileColumn, tileLayer 瓦块的等级，行列层号
		* @param [out] cell_I, cell_J, cell_K 单元格的I,J,K值
		* @return  成功返回true
		-	 失败返回false
		*/
		//glbBool Pick(glbInt32 mousex,glbInt32 mousey,
		//			glbInt32 &tilelevel, glbInt32 &tileRow, glbInt32 &tileColumn, glbInt32 &tileLayer,
		//			glbInt32& cell_I, glbInt32& cell_J, glbInt32& cell_K);

		glbBool Pick(glbInt32 mousex,glbInt32 mousey,glbDouble &interX,glbDouble &interY,glbDouble &interZ);

		// 是否需要暂停
		glbBool IsUpdateNeedTerminate();
		
		// 获取某级Tile的行列数
		glbBool GetRowColumnCount(glbInt32 level, glbInt32 &startRow,glbInt32 &startColumn,glbInt32 &rowNum, glbInt32 columnNum);
	private:
		// 初始化
		glbBool init();	
		// 平面模式下的grid图层更新
		void UpdateFlatGrid();
		// 球面模式下的grid图层更新
		void UpdateGlobeGrid();
		// 移除所有不可见瓦块
		void RemoveInvisibleTiles();
		// 判断相机射线与grid的数据区域的交点，返回最近交点和交点所在的面enum（上表面,下表面，前表面，后表面，左表面，右表面）
		//glbBool IsRayInterGrid(osg::Vec3d ln_pt1, osg::Vec3d ln_dir, osg::Vec3d &InterPos, BYTE& faceType);

		// 计算视锥与垂线(钻孔)的相交区域 --> 接口要移到globeView中
		//glbBool computeViewFrustumInterVerticalLine(glbDouble x,glbDouble y, glbDouble minz, glbDouble maxz,glbDouble& outminz, glbDouble& outmaxz);
		
		// 计算视锥与垂(剖)面的相交区域 [此相交区域需要规范为多个（x,y）坐标数组+minz+maxz的格式输出] --> 接口要移到globeView中
		//glbBool computeViewFrustumInterVerticalPlane(			
		//				glbDouble x0,glbDouble y0,glbDouble x1, glbDouble y1, glbDouble minz, glbDouble maxz,
		//				glbDouble& outx0,glbDouble& outy0,glbDouble& outx1, glbDouble& outy1, glbDouble& outminz, glbDouble& outmaxz);

		
	private:
		glbBool mpr_isInitialized;									///<是否初始化
		glbInt32 mpr_minLevel;										///<最小等级
		glbInt32 mpr_maxLevel;										///<最大等级
		glbDouble mpr_south;										///<南边界
		glbDouble mpr_north;										///<北边界
		glbDouble mpr_west;											///<西边界
		glbDouble mpr_east;											///<东边界
		glbref_ptr<CGlbGlobeRObject> mpr_gridObject;				///<对应的grid对象
		glbref_ptr<GlbRenderInfo> mpr_renderInfo;					///<渲染描述信息

		glbInt32 mpr_minlevel_tiles_columns;								///<最小级分块列数
		glbInt32 mpr_minlevel_tiles_rows;									///<最小级分块行数
		glbInt32 mpr_minlevel_tiles_layers;									///<最小级分块层数
		osg::ref_ptr<osg::Switch> mpr_node;									///<格网图层osg节点
		std::map<glbInt32,glbref_ptr<CGlbGlobeGridTile>> mpr_topmost_tiles;	///<0级格网块集合mpr_lztiles , 0级块索引	

		CRITICAL_SECTION mpr_osgcritical;									///<osg节点回调、添加修改任务、取高程 互斥
		CRITICAL_SECTION mpr_tilebuffer_exchange_criticalsection;			///<数据交换临界区
		std::vector<GlbTileOperation> mpr_operations;						///<操作集合
		std::vector<GlbTileOperation> mpr_operations_buffer;				///<操作集合缓冲	

		glbInt32 mpr_addOperationCount;										///<需要处理的添加节点操作数量

		glbBool	mpr_isNeedTerminate;										///<暂停



	};
}

