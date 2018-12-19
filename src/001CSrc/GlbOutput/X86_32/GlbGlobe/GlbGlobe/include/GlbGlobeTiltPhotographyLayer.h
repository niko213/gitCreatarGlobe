/********************************************************************
  * Copyright (c) 2014 北京超维创想信息技术有限公司
  * All rights reserved.
  *
  * @file    GlbGlobeTiltPhotographyLayer.h
  * @brief   倾斜摄影数据图层头文件
  *
  * 这个档案定义CGlbGlobeTiltPhotographyLayer这个class
  *
  * @version 1.0
  * @author  马林
  * @date    2015-8-4 10:40
*********************************************************************/
#pragma once 

#include "GlbGlobeExport.h"
#include "GlbGlobeDomLayer.h"
#include <osg/Texture2D>
#include "GlbGlobeRObject.h"
#include "GlbPolygon.h"
#include "GlbFeature.h"
#include <GlbTin.h>

namespace GlbGlobe
{
	class GLB_DLLCLASS_EXPORT CGlbGlobeTiltPhotographyLayer : public CGlbGlobeLayer
	{
	private:
		// 块信息结构体
		struct GlbTileInfo
		{	// 名称
			std::string name;
			//  块node
			osg::ref_ptr<osg::Node>  node;
			// 范围
			double xmin;
			double xmax;
			double ymin;
			double ymax;
			double zmin;
			double zmax;
			// 子块数
			int	childCnt;
			// 纹理数据
			osg::ref_ptr<osg::Texture>  texture;
			unsigned char* orignData; // 原始的纹理 
		}_GlbTileInfo;

		// 定义操作类型，用于界面与osg更新回调之间通讯
		enum GlbOperationType
		{// 操作类型
			Opera_None = 0,						//无
			Opera_SelectionRegionChanged = 1,	//选择区域变化
			Opera_DigHoleRegionChanged = 2,		//挖洞区域变化
			Opera_OnTerrainObjChanged = 3		//贴地绘制对象变化
		};

		enum GlbShaderType
		{// 找色器 类型
			Shader_Unknown = 0,
			Shader_projectTexture = 1,
			Shader_digHole = 2
		};

		// @brief 倾斜摄影图层osg节点更新回调类
		class CGlbGlobeTiltPhotographyCallback : public osg::NodeCallback
		{
		public:
			CGlbGlobeTiltPhotographyCallback(CGlbGlobeTiltPhotographyLayer* pLayer);
			/**
			* @brief 倾斜摄影图层osg节点回调实现
			* @param [in] node 地形的osg节点
			* @param [in] nv osg节点访问器	  
			* @return  无
			*/
			virtual void operator() (osg::Node* node,osg::NodeVisitor* nv);
		private:
			CGlbGlobeTiltPhotographyLayer* m_pLayer;
		};
	public:
		/*
		* @brief 倾斜摄影数据图层构造函数
		* @param [in] tpFile creatarGlobe定义的倾斜摄影数据索引文件*.tpf
		* @return 无
		*/
		CGlbGlobeTiltPhotographyLayer(CGlbWString tpfFile,glbBool isGlobe=true);
		/*
		* @brief 倾斜摄影数据图层析构函数
		* @return 无
		*/
		~CGlbGlobeTiltPhotographyLayer(void);
		/*
		* @brief 获取相机与图层的距离
		* @param[in]  cameraPos 相机在场景中的位置
		* @return 相机与图层的距离
		*/
		glbDouble GetDistance( osg::Vec3d &cameraPos);
		/* 基类接口 */
		 /*
		  * @brief 获取图层名称
		  * @return 图层名称 
		  */
		const glbWChar* GetName();
		 /*
		  * @brief 设置图层名称
		  * @param name 图层名称
		  * @return  成功返回true
				-	 失败返回false
		  */
		glbBool SetName(glbWChar* name);
		 /*
		  * @brief 获取图层类型
		  * @return  图层类型
		  */
		GlbGlobeLayerTypeEnum GetType();

		 /*
		  * @brief 图层序列化（加载）
		  * @param [in] node xml读取节点
		  * @param [in] relativepath 相对路径
		  * @return  成功返回true
				-	 失败返回false
		  */
		glbBool   Load(xmlNodePtr node, glbWChar* relativepath );
		 /*
		  * @brief 图层序列化（保存）
		  * @param [in] node xml写入节点
		  * @param [in] relativepath 相对路径
		  * @return  成功返回true
				-	 失败返回false
		  */
		glbBool   Save(xmlNodePtr node, glbWChar* relativepath );
		/*
		* @brief 设置图层显示或隐藏
		* @param [in] show 显隐  显示为true，隐藏为false
		* @param [in] isOnState true表示在可见区域  false表示不在可见区域
		* @return  成功返回true
		-	 失败返回false
		*/
		glbBool Show(glbBool show,glbBool isOnState=false);
		 /*
		  * @brief 设置图层不透明度
		  * @param [in] opacity 不透明度(0-100)
						-	0表示全透明
						-	100表示不透明
		  * @return  成功返回true
				-	 失败返回false
		  */
		glbBool SetOpacity(glbInt32 opacity);
		/*
		* @brief 获取图层范围
		* @param [in] isWorld 是否是世界坐标，如果是世界坐标，以米为单位;如果不是世界坐标，则使用地理坐标，以度为单位			
		* @return  图层范围
		*/
		glbref_ptr<CGlbExtent> GetBound(glbBool isWorld);

		 /*
		  * @brief 获取图层对应的tpf文件全路径
		  * @return tpf文件全路径 
		  */
		const glbWChar* GetTPFFilePath();
		 /*
		  * @brief 设置图层关联的挖洞对象
		  * @param [in] dighole 挖洞对象
		  */		
		void SetAssociateObject(CGlbGlobeObject* dighole);
		/*
		* @brief 获取图层关联的挖洞对象
		* @return 无
		*/
		CGlbGlobeObject* GetAssociateObject();
		/*
		* @brief 图层更新
		* @param [in] domlayers 栅格图层集合
		* @param [in] objs		贴地绘制和挖洞对象集合
		* @return 无
		* @note 适合小范围内对象集中的区域，因为默认的纹理尺寸为1024*1024，分辨率不是很高！2017.11.14
		*/	
		void Update(	/*std::vector<glbref_ptr<CGlbGlobeDomLayer>> domlayers,*/
						std::vector<glbref_ptr<CGlbGlobeRObject>> terrainobjs	);

		/*
		* @brief 刷新图层区域内的纹理
		* @param [in] objDirtyExts 需要刷新的贴地形对象区域
		* @param [in] domDirtyExts 需要刷新的dom图层区域
		* @param [in] domlayers		dom图层集合
		* @param [in] terrainObjs	贴地形绘制的对象集合
		* @return 无
		*/		
		void RefreshTexture(std::vector<CGlbExtent>                    &objDirtyExts,
							std::vector<CGlbExtent>                    &domDirtyExts,
							std::vector<glbref_ptr<CGlbGlobeDomLayer>> &domlayers, 	
							std::vector<glbref_ptr<CGlbGlobeRObject>>  &terrainobjs	);
		/*
		* @brief 添加选中区域
		* @param poly : 多边形
				 clr  : 颜色
		* @return 添加的选中区域索引id
		*/
		glbBool AddSelectedRegion(CGlbPolygon* poly);
		/*
		* @brief 获取已添加选中区域的数量		
		* @return 已添加选中区域的数量	
		*/
		glbInt32 GetSelectedRegionCount();

		/*
		* @brief 获取第idx个已添加选中区域		
		* @return 已添加选中区域
		*/
		CGlbPolygon* GetSelectedRegion(glbInt32 idx);
		/*
		* @brief 删除第idx个已添加选中区域		
		* @return 成功返回true
				失败返回false
		*/
		glbBool	RemoveSelectedRegion(glbInt32 idx);
		/*
		* @brief 清除所有已添加选中区域		
		* @return 无
		*/
		void ClearSelectedRegions();
		/**
		* @brief 射线选择-非贴地面绘制对象
		* @param [in] ln_pt1,ln_pt2 射线的起点pt1和射线延伸方向上的一点pt2 		
		* @param [out] inter_pts 交点集合，按距离pt1点由近得远排列 ， 一个对象取一个交点
		* @param [out] featureOid 选中对象id
		* @return  有选中对象返回true，没有选中对象返回false
		*/
		glbBool Query(osg::Vec3d ln_pt1, osg::Vec3d ln_pt2, std::vector<osg::Vec3d>& inter_pts, glbInt32 &featureOid);
		/**
		* @brief 倾斜摄影osg节点的更新回调
		* @param [in] node osg节点
		* @param [in] nv osg节点访问器
		* @return  成功返回S_OK,S_FALSE
		-	 失败返回E_FAIL,E_POINTER
		*/
		//HRESULT UpdateOsgCallBack(osg::Node* node,osg::NodeVisitor* nv);
		 /*
		  * @brief 设置选中所需关联查询的多边形对象数据集
		  * @param [in] fclss 多边形对象数据集
					[in] querybufferSize 查询区域大小（半径）默认50米
		  * @return  成功返回true
				-	 失败返回false
		  */
		glbBool SetPolyFeatureClassForQuery(IGlbFeatureClass* fclss, glbDouble querybufferSize=50.0);
		 /*
		  * @brief 获取选择所关联查询的多边形对象数据集		 
		  * @return   多边形对象数据集
		  */
		IGlbFeatureClass* GetPolyFeatureClassForQuery();
		 /*
		  * @brief 获取选择所关联查询区域半径 
		  * @return   查询区域半径 
		  */
		glbDouble GetQueryBufferSize();		
		 /*
		  * @brief 设置多边形绘制外延缓冲尺寸(单位：米)
		  * @param [in] buffersize 多边形绘制外延缓冲尺寸
		  */
		void SetPolyDrawBufferHint(glbDouble buffersize=0.0);
		 /*
		  * @brief 获取多边形绘制外延缓冲尺寸(单位：米)
		  * @return   多边形绘制外延缓冲尺寸
		  */
		glbDouble GetPolyDrawBufferHint();
		 /*
		  * @brief 图层节点的更新回调函数	
		 */
		void UpdateOsgCallback (osg::Node* node,osg::NodeVisitor* nv);			

		/*
		 * @brief 添加挖洞区域	
		 * @param poly : 多边形
		*/
		glbBool AddDigHoleRegion(CGlbPolygon* poly);
		/*
		* @brief 获取已添加挖洞区域的数量		
		* @return 已添加选中区域的数量	
		*/
		glbInt32 GetDigHoleCount();

		/*
		* @brief 获取第idx个已添加挖洞区域		
		* @return 已添加挖洞区域
		*/
		CGlbPolygon* GetDigHole(glbInt32 idx);
		/*
		* @brief 删除第idx个已添加挖洞区域		
		* @return 成功返回true
				失败返回false
		*/
		glbBool	RemoveDigHole(glbInt32 idx);
		/*
		* @brief 清除所有已添加挖洞区域		
		* @return 无
		*/
		void ClearDigHole();
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
		/*
		* @brief 获取倾斜数据锚点坐标
		* @param [out] anchorX x
		*		 [out] anchorY y
		*		 [out] anchorZ z
		* @return 无
		*/
		void GetAnchor(glbDouble& anchorX, glbDouble& anchorY, glbDouble& anchorZ);
		/*
		* @brief 计算坡度 单位：度
		* @param [in] x1,y1,z1 起始点 【单位：球面：度，平面：米】
		*		 [in] x2,y2,z2 终止点 【单位：度】
		* @return 坡度值
		*/
		glbDouble Slope(glbDouble x1, glbDouble y1, glbDouble z1, glbDouble x2, glbDouble y2, glbDouble z2);

		/**
		* @brief  释放内存
		*/
		void ReleaseMemory();	

		//测试接口 启动（关闭）线框模式
		void EnableWireMode(glbBool wiremode);
	private:
		// 加载tpf文件，生成图层顶级块
		glbBool loadTPFfile(CGlbWString tpfFile);
		// 创建下挂需要投射对象的正投相机生成rtt纹理
		// 将此camera挂到倾斜摄影图层根节点下
		osg::Camera* createOverlayCamera(osg::Node* overlayNode, GlbGlobeTypeEnum type);
		// 创建shader  -- 选中或叠加shader
		// ss ：倾斜图层节点属性
		// camera : overlay相机
		// textureUnit : 绑定的纹理单元
		glbBool createShaders(osg::StateSet *ss,osg::Camera *camera, glbInt32 textureUnit);

		osg::Texture2D* createRTTexture();

		glbBool buildOverlaySubgraph();

		// 创建多边形节点
		osg::Node* createRegionGeometry(CGlbPolygon* polygon, osg::Vec4 clr);

		// 创建多多边形节点
		osg::Node* createRegionsGeometry(std::vector<glbref_ptr<CGlbPolygon>> polygons,osg::Vec4 clr);

		// 将包围盒转为Tin
		CGlbTin* ExtentToTin(CGlbExtent* p_extent);

		// 处理Operation
		void DealOperation(GlbOperationType opera);
		// 创建shader  -- 挖洞的shader
		// ss ：倾斜图层节点属性
		// camera : overlay相机
		// textureUnit : 绑定的纹理单元
		glbBool createDigHoleShaders(osg::StateSet *ss,osg::Camera *camera, glbInt32 textureUnit);

		glbBool buildDigHoleSubgraph();
		// 创建贴地绘制对象组节点
		osg::Node* createOnTerrainObjectsNode(std::vector<glbref_ptr<CGlbGlobeRObject>>  &terrainobjs);

		// 判断node节点中名称为name的UpdateCallback是否存在
		glbBool isUpdateCallbackExist(osg::Node* node, std::string name);

		// 创建贴地和选中标识的节点
		osg::Node* buildOverlayNode();
		// 创建挖洞节点
		osg::Node* buildDigHoleOverlayNode();
	private://测试rtt纹理
		void testRTTCameraTexture(osg::Group* parentNode,CGlbPolygon* poly,osg::Texture2D* tex);
		glbBool isPolyInterLayer(CGlbPolygon* poly);
		// 更新相机矩阵
		void updateCamera(osg::StateSet *ss,osg::Camera *camera,osg::Node* overlayNode);
	private:
		CGlbWString			mpr_tpfFilePath;					///< tpf文件路径
		glbBool				mpr_isGlobe;						///< 是否是球面方式			
		osg::Vec3d			mpr_anchor;							///< 锚点【球面模式单位：度】
		CGlbExtent			mpr_bound;							///< 外包
		CGlbString			mpr_DataPath;						///< 数据路径
		GlbCriticalSection	mpr_critical;						///< 临界区
		glbBool				mpr_isLinemode;						///<线框模式	
		std::map<std::string, GlbTileInfo> _tilesRenderingMap;	///< 现在渲染的tile的map数组
		std::map<std::string, GlbTileInfo> _topmostTilesMap;	///< 顶级tile的map数组
		glbref_ptr<CGlbGlobeObject>		mpr_digHoleObj;			///< 与之关联的挖洞对象
		std::vector<glbref_ptr<CGlbPolygon>> mpr_selectedRegions;///< 选中区域
		osg::ref_ptr<osg::NodeCallback>	mpr_nodeCallback;		///< 图层节点回调
		//glbBool				mpr_bDirty;						///< 刷新标志
		osg::ref_ptr<osg::Group>	mpr_tiltDataNodeRoot;		///< 倾斜数据根节点	
		glbDouble			mpr_overlayBaseHeight;				
		glbref_ptr<IGlbFeatureClass>   mpr_pPolyFeatureClass;	///< 选择相关的多边形数据集
		glbDouble			mpr_queryBufferSize;				///< 选择查询半径
		glbDouble			mpr_polyDrawBufferSize;				///< 多边形渲染缓冲尺寸

		glbInt32			mpr_textureUnit;					///< 纹理号 默认为1，第一重纹理不占用0级纹理
		glbInt32			mpr_tex_width;
		glbInt32			mpr_tex_height;
		osg::ref_ptr<osg::Texture2D> mpr_rttTexture;
		GlbOperationType		mpr_curOperation;				///< 当前操作类型
		osg::ref_ptr<osg::Node>		mpr_overlayNode;			///< 投影纹理节点
		//std::vector<glbref_ptr<CGlbGlobeDomLayer>> mpr_domlayers; 	///< dom图层
		std::vector<glbref_ptr<CGlbGlobeRObject>>  mpr_terrainobjs;	///< 贴地绘制对象

		std::vector<glbref_ptr<CGlbPolygon>> mpr_digHoleRegions;///< 挖洞区域  2017.11.9
		glbInt32			mpr_digHolTextureUnit;				///< 纹理号 默认为2，第一重纹理不占用0级纹理
		osg::ref_ptr<osg::Node>		mpr_digHoleOverlayNode;		///< 投影纹理节点
		osg::ref_ptr<osg::Texture2D> mpr_digHolerttTexture;		///< 挖洞的rtt纹理

		glbDouble			mpr_xOffset;
		glbDouble			mpr_yOffset;
		glbDouble			mpr_zOffset;
	};
}


