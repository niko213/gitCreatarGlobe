/********************************************************************
* Copyright (c) 2014 北京超维创想信息技术有限公司
* All rights reserved.
* @file    GlbGlobeTinObject.h
* @brief   地质体对象头文件，文档定义CGlbGlobeTinObject类
* @version 1.0
* @author  龙庭友
* @date    2014-11-13 15:30
*********************************************************************/
#pragma once

#include "GlbGlobeObjectExport.h"

#include "GlbGlobeREObject.h"
#include "IGlbGlobeTask.h"
#include "GlbGlobeCallBack.h"

#include "GlbTin.h"
#include "GlbRobjectMeshBox.h"

namespace GlbGlobe
{
	/**
	* @brief CGlbGlobeTinObject地质体Tin对象类
	* @version 1.0
	* @author  龙庭友
	* @date    2014-11-13 15:30
	*/
	class GLB_OBJECTDLL_CLASSEXPORT CGlbGlobeTinObject : public CGlbGlobeREObject
	{
		class CGlbGlobeTinLoadDataTask : public IGlbGlobeTask
		{
		public:
			CGlbGlobeTinLoadDataTask(CGlbGlobeTinObject *obj,glbInt32 level);
			~CGlbGlobeTinLoadDataTask();
			glbBool doRequest();
			CGlbGlobeObject *getObject();
		private:
			glbref_ptr<CGlbGlobeTinObject>    mpr_obj;
			glbInt32                        mpr_level;
		};
	public:
		CGlbGlobeTinObject(void);
		~CGlbGlobeTinObject(void);

		virtual glbBool SetAltitudeMode( GlbAltitudeModeEnum mode,glbBool &isNeedDirectDraw );

		virtual void SetRenderOrder( glbInt32 order );

		virtual glbDouble GetDistance( osg::Vec3d &cameraPos,glbBool isCompute );

		virtual void LoadData( glbInt32 level );

		virtual void AddToScene();

		virtual glbInt32 RemoveFromScene( glbBool isClean );

		virtual glbref_ptr<CGlbExtent> GetBound( glbBool isWorld = true );

		virtual void UpdateElevate();

		virtual IGlbGeometry * GetOutLine();

		virtual glbBool SetRenderInfo( GlbRenderInfo *renderInfo,glbBool &isNeedDirectDraw );

		virtual GlbRenderInfo * GetRenderInfo();

		virtual glbBool SetShow( glbBool isShow,glbBool isOnState=false );

		virtual glbBool SetSelected( glbBool isSelected );

		void SetPartSelected(glbInt32 partId,glbBool isSelected);

		virtual glbBool SetBlink( glbBool isBlink );

		virtual void DirectDraw( glbInt32 level );

		virtual void DrawToImage( glbByte *image, glbInt32 imageW,glbInt32 imageH,CGlbExtent &ext );

		virtual glbInt32 GetOsgNodeSize();

		virtual glbBool Load( xmlNodePtr* node,const glbWChar* prjPath );

		virtual glbBool Load2( xmlNodePtr* node,const glbWChar* prjPath );

		virtual glbBool Save( xmlNodePtr node,const glbWChar* prjPath );

		virtual glbBool SetEdit( glbBool isEdit );

		virtual GlbGlobeObjectTypeEnum GetType();

		glbBool SetGeo(IGlbGeometry *tinGeo,glbBool &isNeedDirectDraw);
		IGlbGeometry *GetGeo()const{return mpr_tinGeo.get();}

		/**
		* @brief 线段与对象的交点
		* @param vtStart  起点
		* @param vtEnd  终点
		* @param interPoint 交点
		* @return 0:没有交点 1有交点
		*/
		virtual glbBool interset(osg::Vec3d vtStart, osg::Vec3d vtEnd, osg::Vec3d& interPoint);
	
		/**
		* @brief 判断当前生成的node是否是用纹理渲染的	
		* @return true纹理  false 颜色
		*/	
		glbBool IsTextureRenderMode();
	private:
		void ReadData(glbInt32 level ,glbBool isDirect=false);
		glbBool DealTinModelSymbol(osg::MatrixTransform *tinContainer,glbInt32 level,GlbTinSymbolInfo *tinInfo,glbBool &isNeedDirectDraw);
		void DealSelected();
		void DealBlink();
		void ComputePosByAltitudeAndGloleType(osg::Vec3d &position);
		void DealModePosByChangeAltitudeOrChangePos();
		glbInt32 ComputeNodeSize( osg::Node *node );
		void DirtyOnTerrainObject();
		osg::Node* FindPartNodeByID(glbInt32 partId);

		bool judgeTinWaterNeedDirectDraw( GlbTinWaterSymbolInfo *tinWaterSmblInfo, bool &isNeedDirectDraw );
		//分格子
		void MeshTinToGrids(glbInt32 level);

		//解析对象的颜色，渐进的时候需要
		void ParseObjectFadeColor();
		/**
		* @brief 更新格子
		* @ parentMatrix 最新tinContainer的矩阵
		* @globeMode 是否是球面模式
		*/
		void updateMeshBox(const osg::Matrix parentMatrix,const GlbTinSymbolInfo * tinSmblInfo, 
			glbref_ptr<CGlbFeature>  feature,GLboolean globeMode);

		glbDouble                                         mpr_distance;        ///<相机到对象的距离
		glbref_ptr<GlbRenderInfo>                         mpr_renderInfo;      ///<对象渲染信息
		glbref_ptr<IGlbGeometry>                          mpr_outline;         ///<对象的2D轮廓
		glbref_ptr<IGlbGeometry>                          mpr_tinGeo;      ///<对象的多边形数据

		glbDouble                                         mpr_elevation;		///<当前地形高

		glbBool                   mpr_isCanDrawImage;		///<控制贴地面的显隐
		glbInt32                  mpr_objSize;				///<obj的size

		GlbCriticalSection        mpr_addToScene_critical;	///<节点添加场景临界区
		GlbCriticalSection		  mpr_readData_critical;	///<ReadData创建节点临界区	
		glbBool                   mpr_needReReadData;		///<重新读取数据标志

		std::map<glbInt32,glbBool> mpr_partSelect;   ///<地质体id与是否选中标志数组

		glbFloat				 mpr_TexRotation;

		ClbRobjectMeshBox		mpr_MeshBox;
	};

}
