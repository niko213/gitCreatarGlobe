/********************************************************************
* Copyright (c) 2014 北京超维创想信息技术有限公司
* All rights reserved.
* @file    GlbGlobePolygon.h
* @brief   多边形对象头文件，文档定义CGlbGlobePolygon类
* @version 1.0
* @author  龙庭友
* @date    2014-6-18 15:30
*********************************************************************/
#pragma once

#include "GlbGlobeObjectExport.h"

#include "GlbGlobeREObject.h"
#include "IGlbGlobeTask.h"
#include "GlbGlobeCallBack.h"

#include "GlbPolygon.h"

namespace GlbGlobe
{
/**
* @brief CGlbGlobePolygon多边形对象类
* @version 1.0
* @author  龙庭友
* @date    2014-6-18 15:30
*/
class GLB_OBJECTDLL_CLASSEXPORT CGlbGlobePolygon : public CGlbGlobeREObject
{
	class CGlbGlobePolygonLoadDataTask : public IGlbGlobeTask
	{
	public:
		CGlbGlobePolygonLoadDataTask(CGlbGlobePolygon *obj,glbInt32 level);
		~CGlbGlobePolygonLoadDataTask();
		glbBool doRequest();
		CGlbGlobeObject *getObject();
	private:
		glbref_ptr<CGlbGlobePolygon>    mpr_obj;
		glbInt32                        mpr_level;
	};
public:
	CGlbGlobePolygon(void);
	~CGlbGlobePolygon(void);

	virtual GlbGlobeObjectTypeEnum GetType();
	glbBool Load( xmlNodePtr *node,const glbWChar* prjPath );

	glbBool Load2( xmlNodePtr *node,const glbWChar* prjPath );
	glbBool Save( xmlNodePtr node,const glbWChar* prjPath );

	virtual glbBool SetAltitudeMode( GlbAltitudeModeEnum mode ,glbBool &isNeedDirectDraw);

	virtual void SetRenderOrder( glbInt32 order );	
		
	virtual glbDouble GetDistance( osg::Vec3d &cameraPos,glbBool isCompute );

	virtual void LoadData( glbInt32 level );

	virtual void AddToScene();

	virtual glbInt32 RemoveFromScene( glbBool isClean );

	glbref_ptr<CGlbExtent> GetBound( glbBool isWorld = true );

	virtual void UpdateElevate();

	virtual IGlbGeometry * GetOutLine();

	virtual glbBool SetRenderInfo( GlbRenderInfo *renderInfo ,glbBool &isNeedDirectDraw );

	virtual GlbRenderInfo * GetRenderInfo();
		
	virtual glbBool SetShow(glbBool isShow,glbBool isOnState=false);

	virtual glbBool SetSelected( glbBool isSelected );

	virtual glbBool SetBlink( glbBool isBlink );

	virtual void DirectDraw( glbInt32 level );

	virtual void DrawToImage( glbByte *image, glbInt32 imageW,glbInt32 imageH,CGlbExtent &ext );

	virtual glbInt32 GetOsgNodeSize();

	/** 逆时针顺序为正 */
	glbBool SetGeo(CGlbPolygon *polygon,glbBool &isNeedDirectDraw);
	CGlbPolygon *GetGeo()const{return mpr_polygonGeo.get();}
	
	/**
	* @brief 设置对象进入或退出"编辑"状态
	* @param[in]  isEdit 进入编辑状态为true，退出为false
	* @return 成功返回true，失败返回false
	*/
	glbBool SetEdit(glbBool isEdit);

	//解析对象的颜色[填充]，渐进的时候需要
	void ParseObjectFadeColor();
private:
	glbBool DrawPoints(glbInt32 color);
	glbBool SetCurrPoint(glbInt32 idx,glbInt32 color);
	glbBool SetSelectedPoint(glbInt32 idx,glbInt32 color);
	glbBool DrawRubber(glbDouble *points,glbInt32 pcnt,glbInt32 color);
	glbBool DrawPrepareAddPoint(glbDouble xOrLon,glbDouble yOrLat,glbDouble zOrAlt,glbInt32 color);
	glbBool ClearDraw();
	CGlbPolygon *RemoveRepeatePoints(CGlbPolygon *polygon);
	CGlbLine *RemoveRepeatePointsByLine(CGlbLine *line);
private:		
	void ReadData(glbInt32 level ,glbBool isDirect=false);

	void DealSelected();
	void DealBlink();
	void ComputePosByAltitudeAndGloleType(osg::Vec3d &position);
	void DealModePosByChangeAltitudeOrChangePos();

	bool judgePolygon2DNeedDirectDraw(GlbPolygon2DSymbolInfo *polygon2DSmblInfo, bool &isNeedDirectDraw);
	void updatePolygon2DSymbolInfo(osg::StateSet *sset,GlbPolygon2DSymbolInfo *polygon2DSmblInfo);
	void updatePolygon2DOutlineSymbolInfo(osg::StateSet *sset,GlbPolygon2DSymbolInfo *polygon2DSmblInfo);
	bool judgePolyhedronNeedDirectDraw(GlbPolyhedronSymbolInfo *polyhedronSmblInfo, bool &isNeedDirectDraw);
	bool judgePolygonBuildingNeedDirectDraw(GlbPolygonBuildingSymbolInfo *polyhedronSmblInfo, bool &isNeedDirectDraw);
	bool judgePolygonWaterNeedDirectDraw(GlbPolygonWaterSymbolInfo *polygonWaterSmblInfo, bool &isNeedDirectDraw);
	bool judgePolygonLakeNeedDirectDraw(GlbPolygonLakeSymbolInfo *polygonLakeSmblInfo, bool &isNeedDirectDraw);
		
	// 解析多边形对象中几何体 - 返回CGlbMultiPolygon对象
	IGlbGeometry* AnalysisPolyGeomertry(CGlbFeature* feature, CGlbPolygon* polyGeo);

	glbInt32 ComputeNodeSize( osg::Node *node );

	void DirtyOnTerrainObject();

	//从场景中移除节点，用于非贴地形模式转换为贴地形模式时，将场景中的非贴地形对象节点移除
	void RemoveNodeFromScene(glbBool isClean=true);
	
	void DealEdit();
private:
	glbDouble                                         mpr_distance;        ///<相机到对象的距离
	glbref_ptr<GlbRenderInfo>                         mpr_renderInfo;      ///<对象渲染信息
	glbref_ptr<IGlbGeometry>                          mpr_outline;         ///<对象的2D轮廓
	glbref_ptr<CGlbPolygon>                         mpr_polygonGeo;      ///<对象的多边形数据
	glbDouble                                         mpr_altitude;	       ///<多边形海拔高度，skyline 为所有多边形点的平均海拔高度
	glbDouble                                         mpr_elevation;		///<当前地形高

	glbBool                                           mpr_isDrawPoints;    ///<是否渲染编辑点
	glbFloat                                          mpr_TexRotation;	

	glbBool                   mpr_isCanDrawImage;		///<控制贴地面的显隐
	glbInt32                  mpr_objSize;				///<obj的size
	GlbCriticalSection        mpr_addToScene_critical;	///<节点添加场景临界区
	GlbCriticalSection		  mpr_readData_critical;	///<ReadData创建节点临界区	
	glbBool                   mpr_needReReadData;		///<重新读取数据标志
	GlbCriticalSection		  mpr_outline_critical;		///<outline读写临界区		
};

}
