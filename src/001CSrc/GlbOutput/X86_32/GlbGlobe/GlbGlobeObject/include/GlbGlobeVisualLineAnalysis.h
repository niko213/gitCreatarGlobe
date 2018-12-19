/********************************************************************
* Copyright (c) 2014 北京超维创想信息技术有限公司
* All rights reserved.
* @file    GlbGlobeVisualLineAnalysis.h
* @brief   视线分析头文件，文档定义CGlbGlobeVisualLineAnalysis类
* @version 1.0
* @author  龙庭友
* @date    2014-9-22 15:30
*********************************************************************/
#pragma once

#include "GlbGlobeObjectExport.h"

#include "GlbGlobeREObject.h"
#include "IGlbGlobeTask.h"
#include "GlbGlobeCallBack.h"


#include "GlbLine.h"
#include "GlbPoint.h"

namespace GlbGlobe
{
	/**
	* @brief 视线分析类
	* @version 1.0
	* @author  龙庭友
	* @date    2014-9-22 15:30
	*/
	class GLB_OBJECTDLL_CLASSEXPORT CGlbGlobeVisualLineAnalysis : public CGlbGlobeREObject
	{
		class CGlbGlobeVisualLineAnalysisLoadDataTask : public IGlbGlobeTask
		{
		public:
			CGlbGlobeVisualLineAnalysisLoadDataTask(CGlbGlobeVisualLineAnalysis *obj,glbInt32 level);
			~CGlbGlobeVisualLineAnalysisLoadDataTask();
			glbBool doRequest();
			CGlbGlobeObject *getObject(){return mpr_obj.get();}
		private:
			glbref_ptr<CGlbGlobeVisualLineAnalysis>    mpr_obj;
			glbInt32                                   mpr_level;
		};
	public:
		CGlbGlobeVisualLineAnalysis(void);
		~CGlbGlobeVisualLineAnalysis(void);
		
		virtual glbBool Load( xmlNodePtr* node,glbWChar* prjPath );

		virtual glbBool Save( xmlNodePtr node,const glbWChar* prjPath );

		virtual GlbGlobeObjectTypeEnum GetType();

		virtual glbDouble GetDistance( osg::Vec3d &cameraPos,glbBool isCompute );

		virtual void LoadData( glbInt32 level );

		virtual void AddToScene();

		virtual glbInt32 RemoveFromScene( glbBool isClean );

		virtual glbref_ptr<CGlbExtent> GetBound( glbBool isWorld = true );

		virtual IGlbGeometry * GetOutLine();

		virtual glbBool SetRenderInfo(GlbRenderInfo *renderInfo,glbBool &isNeedDirectDraw);

		virtual GlbRenderInfo * GetRenderInfo();

		virtual glbBool SetShow( glbBool isShow,glbBool isOnState=false );

		virtual void DirectDraw( glbInt32 level );		

		virtual glbInt32 GetOsgNodeSize();		
		//////////////////////////////////////////////////////////////////////////	

		/** 设置视线 */
		glbBool SetSightLine(CGlbPoint *watchPoint,CGlbPoint *targetPoint,glbBool &isNeedDirectDraw );
		/** 观察点 */
		CGlbPoint *GetWatchPoint()const{return mpr_watchPoint.get();}
		/** 目标点 */
		CGlbPoint *GetTargetPoint()const{return mpr_targetPoint.get();}
	private:
		void ReadData(glbInt32 level,glbBool isDirect=false);

		glbInt32 ComputeNodeSize( osg::Node *node );

		glbref_ptr<CGlbPoint>                           mpr_watchPoint;		  ///<相机位置(经纬高)
		glbref_ptr<CGlbPoint>                           mpr_targetPoint;		  ///<焦点位置
		glbref_ptr<CGlbLine>							  mpr_visualLine;		
		
		glbDouble                                         mpr_distance;           ///<相机到对象的距离
		glbref_ptr<GlbRenderInfo>                         mpr_renderInfo;         ///<对象渲染信息
		glbInt32                                          mpr_objSize;			  ///<节点大小
		GlbCriticalSection                                mpr_readData_critical;  ///<创建节点临界区
		GlbCriticalSection                                mpr_addToScene_critical;///<节点添加场景临界区
		glbBool											  mpr_needReReadData;	  ///<重新读取数据标志
	};

}
