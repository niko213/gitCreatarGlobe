#pragma once
#include "GlbReference.h"
#include "GlbWString.h"
#include "EasyGlbGlobeTypes.h"
#include "IGlbDataEngine.h"
#include <osg/Switch>
#include "libxml/tree.h"

namespace GlbGlobe
{
	class CEasyGlbGlobe;
	class GLB_DLLCLASS_EXPORT CEasyGlbGlobeTerrain : public CGlbReference
	{
	public:
		/**
		* @brief 地形构造函数
		* @param [in] globe 地形所属场景	  
		* @return  无
		*/
		CEasyGlbGlobeTerrain(CEasyGlbGlobe* globe);
		~CEasyGlbGlobeTerrain(void);
		/**
		* @brief 设置地形数据集
		* @param [in] tdataset 地形数据集	  
		* @return  成功返回true
		-	 失败返回false
		*/
		//glbBool SetTerrainDataset(IGlbTerrainDataset* tdataset);
		/**
		* @brief 获取地形数据集 
		* @return  成功返回地形数据集指针
		-	 失败返回NULL
		*/
		//IGlbTerrainDataset* GetTerrainDataset();
		/**
		* @brief 地形初始化	   
		* @return  成功返回true
		-	 失败返回false
		*/
		//glbBool Initialize();
		/**
		* @brief 获取地形是否初始化	   
		* @return  已初始化返回true
		-	 没有初始化返回false
		*/
		//glbBool IsInitialized();
		/**
		* @brief 地形更新
		* @param [in] domlayers dom图层集合
		* @param [in] demlayers dem图层集合
		* @param [in] terrainobjs 贴地形绘制对象集合
		* @param [in] mTerrainobjs 地形修改对象集合
		* @return  无
		*/
		//void Update(std::vector<glbref_ptr<CGlbGlobeDomLayer>> &domlayers,
		//	std::vector<glbref_ptr<CGlbGlobeDemLayer>> &demlayers,
		//	std::vector<glbref_ptr<CEasyGlbGlobeRObject>>  &terrainobjs, 
		//	std::vector<glbref_ptr<CEasyGlbGlobeRObject>>  &mTerrainobjs);
		/**
		* @brief 地形销毁  
		* @return  无
		*/
		//void Dispose();
	};

}

