/********************************************************************
* Copyright (c) 2014 ������ά������Ϣ�������޹�˾
* All rights reserved.
*
* @file    CGlbClipObject.h
* @brief   �ü����� ͷ�ļ�
*
* �����������CGlbClipObject���class
*
* @version 1.0
* @author  ChenPeng
* @date    2016-10-27 13:30
* @note	   ������չΪ�����ü������� ֧�ֶԵ��桢����ͼ��ͬʱ�ü� 2017-11-6 malin
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
		* @brief ��ȡ��������
		* @return ����ö������
		*/
		GlbGlobeObjectTypeEnum GetType();
		// ���һ���ü�ƽ�� ��planeA,planeB,planeC,planeD��Ϊ����ƽ���4������
		glbBool AddClipPlane(glbDouble planeA,glbDouble planeB,glbDouble planeC,glbDouble planeD);
		// ɾ��һ���ü�ƽ��
		glbBool RemoveClipPlane(glbUInt32 pos);
		// ɾ�����вü�ƽ��
		glbBool	RemoveAllClipPlane();
		// ��ȡ��pos���ü�ƽ�淽�̵Ĳ�����planeA,planeB,planeC,planeD��
		void GetClipPlane(glbUInt32 pos,glbDouble &planeA, glbDouble &planeB, glbDouble &planeC, glbDouble &planeD);
		// ��ȡ�ü�ƽ������
		glbInt32 GetClipPlaneCount();
		// ���òü���Χ��
		void SetClipBox(CGlbExtent* ext);
		// ���õ�pos���ü�ƽ�淽�̷��򣨷����෴��
		void SetOpposeSide(unsigned int pos);
		//�����յ������µĻ�,��Ҫ�ڵ���������³��߶ȣ���Ҫ��Ը߳�
		void ConvertLatLongHeightToXYZ(glbDouble lat,glbDouble lon,glbDouble ele,glbDouble &posX, glbDouble &posY, glbDouble &posZ);
		void ComputeClipPlane(osg::Vec3d v1, osg::Vec3d v2, osg::Vec3d v3, glbDouble &planeA, glbDouble &planeB, glbDouble &planeC, glbDouble &planeD);
		// �ü�����	
		void Clip(CGlbGlobeObject* obj, glbBool enableClip = true);		
		// �ü�Ҫ��ͼ��
		void Clip(CGlbGlobeLayer* layer, glbBool enableClip = true);
		//////////////////////////////////////////////////////////////////////////
		// ��չ �ü���������ͼ��+����
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
		glbInt32                    mpr_clipID;				//������id,clipNode��ڵ�ֱ������clipPlane������
		bool                        mpr_markOffsetPos;		///< ƫ�Ƶ�
		glbBool						mpr_bTerrainNeedClip;	///< �Ƿ�ü�����	
		std::vector<clipObjInfo> mpr_objectsNeedClip;		///< ��Ҫ�ü��Ķ��󼯺�
		std::vector<clipLayerInfo> mpr_layersNeedClip;		///< ��Ҫ�ü��ģ�Ҫ�أ�ͼ���(��б��Ӱ)ͼ��...����			
		osg::ref_ptr<osg::Node> mpr_terrainParentNode;		///< �洢����ԭʼ�ĸ��ڵ�
		glbBool						mpr_IsEnableClip;		///< �Ƿ������˲ü�
		osg::ref_ptr<osg::NodeCallback> mpr_clipCallback;		
	};
}