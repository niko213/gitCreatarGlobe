/********************************************************************
* Copyright (c) 2014 北京超维创想信息技术有限公司
* All rights reserved.
*
* @file    CGlbClipObject.h
* @brief   裁剪对象 头文件
*
* 这个档案定义CGlbClipObject这个class
*
* @version 1.0
* @author  ChenPeng
* @date    2016-10-27 13:30
* @note	   功能拓展为场景裁剪管理器 支持对地面、对象、图层同时裁剪 2017-11-6 malin
*********************************************************************/
#pragma once
#include "GlbGlobeExport.h"
#include "GlbGlobeObject.h"
#include "GlbGlobeLayer.h"
#include "GlbExtent.h"
#include <osg/ClipNode>

namespace GlbGlobe
{
	class GLB_DLLCLASS_EXPORT CGlbClipObject : public CGlbGlobeObject
	{
	public:
		struct clipObjInfo
		{
			glbref_ptr<CGlbGlobeObject> obj;
			osg::ref_ptr<osg::Node> objOriginParentNode;
		};

		struct clipLayerInfo
		{
			glbref_ptr<CGlbGlobeLayer> lyr;
			osg::ref_ptr<osg::Node> lyrOriginParentNode;
		};
	public:
		CGlbClipObject(void);
		~CGlbClipObject(void);
		/**
		* @brief 获取对象类型
		* @return 对象枚举类型
		*/
		GlbGlobeObjectTypeEnum GetType();
		// 添加一个裁剪平面 （planeA,planeB,planeC,planeD）为定义平面的4个参数
		glbBool AddClipPlane(glbDouble planeA,glbDouble planeB,glbDouble planeC,glbDouble planeD);
		// 删除一个裁剪平面
		glbBool RemoveClipPlane(glbUInt32 pos);
		// 删除所有裁剪平面
		glbBool	RemoveAllClipPlane();
		// 获取第pos个裁剪平面方程的参数（planeA,planeB,planeC,planeD）
		void GetClipPlane(glbUInt32 pos,glbDouble &planeA, glbDouble &planeB, glbDouble &planeC, glbDouble &planeD);
		// 获取裁剪平面数量
		glbInt32 GetClipPlaneCount();
		// 设置裁剪包围盒
		void SetClipBox(CGlbExtent* ext);
		// 设置第pos个裁剪平面方程反向（法线相反）
		void SetOpposeSide(unsigned int pos);
		//如果封闭的是上下的话,需要节点上下面的下沉高度，需要相对高程
		void ConvertLatLongHeightToXYZ(glbDouble lat,glbDouble lon,glbDouble ele,glbDouble &posX, glbDouble &posY, glbDouble &posZ);
		void ComputeClipPlane(osg::Vec3d v1, osg::Vec3d v2, osg::Vec3d v3, glbDouble &planeA, glbDouble &planeB, glbDouble &planeC, glbDouble &planeD);
		// 裁剪对象	
		void Clip(CGlbGlobeObject* obj, glbBool enableClip = true);		
		// 裁剪要素图层
		void Clip(CGlbGlobeLayer* layer, glbBool enableClip = true);
		//////////////////////////////////////////////////////////////////////////
		// 扩展 裁剪多个对象和图层+地形
		void AddObject(CGlbGlobeObject* obj);
		void RemoveAllObjects();
		glbInt32 GetObjectCount();		
		glbBool RemoveObject(glbInt32 idx);
		//CGlbGlobeObject* GetObject(glbInt32 idx);
		CGlbGlobeObject* GetObjectByIndex(glbInt32 idx);
		void AddLayer(CGlbGlobeLayer* layer);
		void RemoveAllLayers();
		glbInt32 GetLayerCount();
		//CGlbGlobeLayer* GetLayer(glbInt32 idx);
		CGlbGlobeLayer* GetLayerByIndex(glbInt32 idx);
		glbBool RemoveLayer(glbInt32 idx);
		void SetTerrainToClip(glbBool enableClip);
		glbBool IsTerrainNeedClip();
		void Clip(glbBool enableClip = true);
	public:
		glbBool doClip(osg::Node* rootNode,glbBool enableClip,osg::Vec3d offsetPos);
	private:
		osg::ref_ptr<osg::ClipNode> mpr_clipNode;
		osg::Vec3d                  mpr_offsetPos;
		glbInt32                    mpr_clipID;				//不设置id,clipNode与节点直接设置clipPlane有区别
		bool                        mpr_markOffsetPos;		///< 偏移点
		glbBool						mpr_bTerrainNeedClip;	///< 是否裁剪地面	
		std::vector<clipObjInfo> mpr_objectsNeedClip;		///< 需要裁剪的对象集合
		std::vector<clipLayerInfo> mpr_layersNeedClip;		///< 需要裁剪的（要素）图层或(倾斜摄影)图层...集合			
		osg::ref_ptr<osg::Node> mpr_terrainParentNode;		///< 存储地形原始的父节点
		glbBool						mpr_IsEnableClip;		///< 是否启用了裁剪
		osg::ref_ptr<osg::NodeCallback> mpr_clipCallback;		
	};
}