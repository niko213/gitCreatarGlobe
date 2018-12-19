/********************************************************************
* Copyright (c) 2014 北京超维创想信息技术有限公司
* All rights reserved.
* @file    GlbGlobeCompositeObject.h
* @brief   复合对象头文件，文档定义CGlbGlobeCompositeObject类
* @version 1.0
* @author  龙庭友
* @date    2014-9-4 15:30
*********************************************************************/
#pragma once

#include "GlbGlobeObjectExport.h"

#include "GlbGlobeREObject.h"
#include "CGlbGlobe.h"


#include <list>

namespace GlbGlobe
{
	/**
	* @brief CGlbGlobeCompositeObject复合对象类
	* @version 1.0
	* @author  龙庭友
	* @date    2014-9-4 15:30
	*/
	class GLB_OBJECTDLL_CLASSEXPORT CGlbGlobeCompositeObject : public CGlbGlobeREObject
	{
	public:
		CGlbGlobeCompositeObject(void);
		~CGlbGlobeCompositeObject(void);

		glbBool   SetAltitudeMode( GlbAltitudeModeEnum mode ,glbBool &isNeedDirectDraw);
		void      SetRenderOrder( glbInt32 order );
		void      SetLoadOrder(glbInt32 order);
		void      SetUseInstance(glbBool useInstance);
		glbDouble GetDistance( osg::Vec3d &cameraPos,glbBool isCompute );
		void      LoadData( glbInt32 level );
		void      AddToScene();
		glbInt32  RemoveFromScene( glbBool isClean );
		void      SetParentNode( osg::Node *parent );
		glbref_ptr<CGlbExtent> GetBound( glbBool isWorld = true );
		void            UpdateElevate();
		IGlbGeometry *  GetOutLine();
		glbBool         SetRenderInfo( GlbRenderInfo *renderInfo ,glbBool &isNeedDirectDraw);
		GlbRenderInfo * GetRenderInfo();
		glbBool SetShow( glbBool isShow,glbBool isOnState=false );
		glbBool SetSelected( glbBool isSelected );
		glbBool SetBlink( glbBool isBlink );
		void    DirectDraw( glbInt32 level );
		void    DrawToImage( glbByte *image, glbInt32 imageW,glbInt32 imageH,CGlbExtent &ext );
		glbInt32 GetOsgNodeSize();
		glbBool  Load( xmlNodePtr* node,glbWChar* prjPath );
		glbBool  Save( xmlNodePtr node,const glbWChar* prjPath );
		GlbGlobeObjectTypeEnum GetType();
		void SetGlobe( CGlbGlobe *globe );

		//////////////////////////////////////////////////////////////////////////
		glbInt32 GetCount();
		/* 添加复合子对象*/
		glbBool AddObject(CGlbGlobeRObject *obj);
		/* 移除复合子对象 下标从0开始*/
		glbBool RemoveObject(glbInt32 idx);
		/* 获取复合子对象 下标从0开始*/
		CGlbGlobeRObject *GetRObject(glbInt32 idx);
	private:
		typedef std::vector<glbref_ptr<CGlbGlobeRObject> >    CGlbGlobeRObjectList;
		CGlbGlobeRObjectList                                 mpr_subogjs;

		glbInt32                                             mpr_objSize;		
	};

}
