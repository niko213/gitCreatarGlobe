/********************************************************************
* Copyright (c) 2014 北京超维创想信息技术有限公司
* All rights reserved.
*
* @file    GlbGlobeTerrainTileRequestTask.h
* @brief   地形块数据请求任务
*
* 这个档案定义CGlbGlobeTerrainTileRequestTask,CGlbGlobeRefreshTileTextureTask这2个class
*
* @version 1.0
* @author  马林
* @date    2014-5-27 16:10
*********************************************************************/
#pragma once

#include "IGlbGlobeTask.h"
#include "glbref_ptr.h"
#include "GlbGlobeTerrainTile.h"
#include "GlbGlobeWMTSTile.h"
#include "GlbGlobeGridTile.h"

namespace GlbGlobe
{
	
	/**
	*@brief 地形块初始化任务类
	*/
	class CGlbGlobeTerrainTileRequestTask :	public IGlbGlobeTask
	{
	public:
		/**
		* @brief 构造函数
		* @param [in] tile 地形块对象	  
		* @param [in] domlayers dom图层集合
		* @param [in] demlayers dem图层集合
		* @param [in] terrainobjs 贴地绘制对象集合
		* @param [in] mterrainobjs 地形修改对象集合
		*/
		CGlbGlobeTerrainTileRequestTask(CGlbGlobeTerrainTile* tile,
			std::vector<glbref_ptr<CGlbGlobeDomLayer>> domlayers,
			std::vector<glbref_ptr<CGlbGlobeDemLayer>> demlayers,
			std::vector<glbref_ptr<CGlbGlobeRObject>> terrainobjs, 
			std::vector<glbref_ptr<CGlbGlobeRObject>> mterrainobjs);
		~CGlbGlobeTerrainTileRequestTask(void);
		/**
		* @brief 执行任务
		* @return 成功返回true
		-	失败返回false
		*/
		glbBool doRequest();
		/**
		* @brief 获取任务相关的对象
		* @return 成功返回对象指针
		-	失败返回NULL
		*/
		CGlbGlobeObject* getObject();
	private:
		glbref_ptr<CGlbGlobeTerrainTile> mpr_terraintile;
		std::vector<glbref_ptr<CGlbGlobeDomLayer>> mpr_domlayers;
		std::vector<glbref_ptr<CGlbGlobeDemLayer>> mpr_demlayers;
		std::vector<glbref_ptr<CGlbGlobeRObject>> mpr_terrainobjs; 
		std::vector<glbref_ptr<CGlbGlobeRObject>> mpr_mterrainobjs;
	};

	/**
	*@brief 地形块纹理更新任务类
	*/
	class CGlbGlobeRefreshTileTextureTask :	public IGlbGlobeTask
	{
	public:
		/**
		* @brief 构造函数
		* @param [in] tile 地形块对象	  
		* @param [in] domlayers dom图层集合
		* @param [in] terrainobjs 贴地绘制对象集合
		* @param [in] isdomrefresh 是否是dom图层刷新
		*/
		CGlbGlobeRefreshTileTextureTask(CGlbGlobeTerrainTile* tile,									
			std::vector<glbref_ptr<CGlbGlobeDomLayer>>   domlayers,                   
			std::vector<glbref_ptr<CGlbGlobeRObject>>  terrainObjs,
			glbBool isdomrefresh = false);

		~CGlbGlobeRefreshTileTextureTask(void);
		/**
		* @brief 执行任务
		* @return 成功返回true
		-	失败返回false
		*/
		glbBool doRequest();
		/**
		* @brief 获取任务相关的对象
		* @return 成功返回对象指针
		-	失败返回NULL
		*/
		CGlbGlobeObject* getObject();
	private:
		glbref_ptr<CGlbGlobeTerrainTile> mpr_terraintile;
		std::vector<glbref_ptr<CGlbGlobeDomLayer>> mpr_domlayers;
		std::vector<glbref_ptr<CGlbGlobeRObject>> mpr_terrainobjs; 
		glbBool mpr_isDomrefesh;
	};


	/**
	*@brief 地形块dem更新任务类
	*/
	class CGlbGlobeRefreshTileDemTask :	public IGlbGlobeTask
	{
	public:
		/*
		* @brief 构造函数
		* @param [in] tile 地形块对象	  
		* @param [in] demlayers dem图层集合
		* @param [in] mterrainobjs 地形修改对象集合	  
		*/
		CGlbGlobeRefreshTileDemTask(CGlbGlobeTerrainTile* tile,									
			std::vector<glbref_ptr<CGlbGlobeDemLayer>>   demlayers,                   
			std::vector<glbref_ptr<CGlbGlobeRObject>>  mterrainObjs);

		~CGlbGlobeRefreshTileDemTask(void);
		/**
		* @brief 执行任务
		* @return 成功返回true
		-	失败返回false
		*/
		glbBool doRequest();
		/**
		* @brief 获取任务相关的对象
		* @return 成功返回对象指针
		-	失败返回NULL
		*/
		CGlbGlobeObject* getObject();
	private:
		glbref_ptr<CGlbGlobeTerrainTile> mpr_terraintile;
		std::vector<glbref_ptr<CGlbGlobeDemLayer>> mpr_demlayers;
		std::vector<glbref_ptr<CGlbGlobeRObject>> mpr_mterrainobjs; 
	};

	/**
	*@brief 瓦片初始化任务类 2017.6.9
	*/
	class CGlbGlobeWMTSTileRequestTask : public IGlbGlobeTask
	{
	public:
		CGlbGlobeWMTSTileRequestTask(CGlbGlobeWMTSTile* ptile);
		~CGlbGlobeWMTSTileRequestTask();
		/**
		* @brief 执行任务
		* @return 成功返回true
		-	失败返回false
		*/
		glbBool doRequest();
		/**
		* @brief 获取任务相关的对象
		* @return 成功返回对象指针
		-	失败返回NULL
		*/
		CGlbGlobeObject* getObject();
	private:
		glbref_ptr<CGlbGlobeWMTSTile> mpr_ptile;
	};

	/**
	*@brief 瓦片初始化任务类 2018.8.6
	*/
	class CGlbGlobeGridTileRequestTask : public IGlbGlobeTask
	{
	public:
		CGlbGlobeGridTileRequestTask(CGlbGlobeGridTile* ptile);
		~CGlbGlobeGridTileRequestTask();
		/**
		* @brief 执行任务
		* @return 成功返回true
		-	失败返回false
		*/
		glbBool doRequest();
		/**
		* @brief 获取任务相关的对象
		* @return 成功返回对象指针
		-	失败返回NULL
		*/
		CGlbGlobeObject* getObject();
	private:
		glbref_ptr<CGlbGlobeGridTile> mpr_ptile;
	};
}