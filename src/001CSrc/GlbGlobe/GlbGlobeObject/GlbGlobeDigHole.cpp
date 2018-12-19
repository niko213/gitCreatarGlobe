#include "StdAfx.h"
#include "GlbGlobeDigHole.h"
#include <osg/Stencil>
#include <osg/Material>
#include <osgUtil/Tessellator>
#include <osgShadow/ShadowedScene>
#include <osgShadow/ShadowMap>
#include <osgShadow/ShadowVolume>
#include <osg/LightSource>
#include <osg/Depth>
#include <osg/FrontFace>
#include <osg/CullFace>
#include <osg/BlendFunc>
#include <osg/Texture2D>
#include <osgDB/ReadFile>
#include <osg/TexEnv>

#include "GlbCalculateNodeSizeVisitor.h"
#include "GlbCalculateBoundBoxVisitor.h"
#include "GlbGlobeFeatureLayer.h"
#include "GlbGlobeDigHoleSymbol.h"
#include "GlbCompareValue.h"
#include "GlbWString.h"
#include "GlbGlobeMemCtrl.h"
#include "GlbConvert.h"

using namespace GlbGlobe;

const int ReceivesShadowTraversalMask = 0x1;	//标识阴影接收对象（例如地面）的掩码
const int CastsShadowTraversalMask  = 0x2;		//标识阴影投射对象（例如飞机）的掩码。

CGlbGlobeDigHole::CGlbGlobeDigHoleTask::CGlbGlobeDigHoleTask( CGlbGlobeDigHole *obj,glbInt32 level )
{
	mpr_obj = obj;
	mpr_level = level;
}
CGlbGlobeDigHole::CGlbGlobeDigHoleTask::~CGlbGlobeDigHoleTask( )
{
	mpr_obj = NULL;
}
glbBool CGlbGlobeDigHole::CGlbGlobeDigHoleTask::doRequest()
{
	if(mpr_obj)
	{
		mpr_obj->ReadData(mpr_level);
		return true;
	}
	return false;
}
CGlbGlobeObject* CGlbGlobeDigHole::CGlbGlobeDigHoleTask::getObject()
{
	return mpr_obj.get();
}

CGlbGlobeDigHole::CGlbGlobeDigHole(void)
{
	mpr_readData_critical.SetName(L"dighole_readdata");
	mpr_addToScene_critical.SetName(L"dighole_addscene");
	mpr_holeUpHei = 1000.0;
	mpr_holeDownHei = -1000.0;
	mpr_distance = 0.0;
	mpr_renderInfo = NULL;
	mpr_holeRegion = NULL;
	mpr_holeDepth = 0.0;
	mpr_isShowWall = false;
	mpr_outline = NULL;
	mpr_needReReadData = false;
	mpr_objSize = 0;
	mpr_isCanDrawImage = true;
}

CGlbGlobeDigHole::~CGlbGlobeDigHole(void)
{

}

GlbGlobeObjectTypeEnum CGlbGlobeDigHole::GetType()
{
	return GLB_OBJECTTYPE_DIGHOLE;
}

glbBool CGlbGlobeDigHole::Load(xmlNodePtr *node,const glbWChar* prjPath)
{
	CGlbGlobeRObject::Load(node,prjPath);
	xmlChar *szKey = NULL;
	xmlNodePtr pnode = *node;

	glbInt32 pointsCount = 0;
	if(pnode && (!xmlStrcmp(pnode->name,(const xmlChar *)"OutRingPointsCount")))
	{
		szKey = xmlNodeGetContent(pnode);
		sscanf((char*)szKey,"%d",&pointsCount);	
		xmlFree(szKey);

		pnode = pnode->next;
	}

	std::string index = "outRingPointsIndex";
	glbDouble tempData0,tempData1,tempData2;

	mpr_holeRegion = new CGlbPolygon(3,false);
	glbref_ptr<CGlbLine> outRing = new CGlbLine(3,false);
	for(glbInt32 i = 0; i < pointsCount * 3;i++)
	{
		index += CGlbConvert::Int32ToStr(i);
		if(pnode && (!xmlStrcmp(pnode->name,(const xmlChar *)index.c_str())))
		{
			szKey = xmlNodeGetContent(pnode);
			sscanf((char *)szKey,"%lf",&tempData0);
			xmlFree(szKey);

			pnode = pnode->next;
			index = "outRingPointsIndex";
		}

		i++;
		index += CGlbConvert::Int32ToStr(i);
		if(pnode && (!xmlStrcmp(pnode->name,(const xmlChar *)index.c_str())))
		{
			szKey = xmlNodeGetContent(pnode);
			sscanf((char *)szKey,"%lf",&tempData1);
			xmlFree(szKey);

			pnode = pnode->next;
			index = "outRingPointsIndex";
		}

		i++;
		index += CGlbConvert::Int32ToStr(i);
		if(pnode && (!xmlStrcmp(pnode->name,(const xmlChar *)index.c_str())))
		{
			szKey = xmlNodeGetContent(pnode);
			sscanf((char *)szKey,"%lf",&tempData2);
			xmlFree(szKey);

			pnode = pnode->next;
			index = "outRingPointsIndex";
		}

		outRing->AddPoint(tempData0,tempData1,tempData2);
	}
	mpr_holeRegion->SetExtRing(outRing.get());

	if(pnode && (!xmlStrcmp(pnode->name,(const xmlChar *)"RenderInfo")))
	{
		GlbGlobeObjectTypeEnum rdtype;
		xmlNodePtr rdchild = pnode->xmlChildrenNode;

		if(rdchild && (!xmlStrcmp(rdchild->name, (const xmlChar *)"RenderType")))
		{
			szKey = xmlNodeGetContent(rdchild);
			sscanf((char*)szKey,"%d",&rdtype);	
			xmlFree(szKey);

			rdchild = rdchild->next;
		}
		GlbDigHoleSymbolInfo* digHoleInfo = new GlbDigHoleSymbolInfo();
		digHoleInfo->Load(rdchild,prjPath);
		mpr_renderInfo = (GlbRenderInfo*)digHoleInfo;
	}
	return true;
}

glbBool CGlbGlobeDigHole::Save(xmlNodePtr node,const glbWChar* prjPath)
{
	CGlbGlobeRObject::Save(node,prjPath);

	char str[32];
	CGlbLine *ring = const_cast<CGlbLine *>(mpr_holeRegion->GetExtRing());
	glbInt32 pointsCount = ring->GetCount();
	sprintf(str,"%d",pointsCount);
	xmlNewTextChild(node,NULL,BAD_CAST "OutRingPointsCount",BAD_CAST str);

	const glbDouble *points = ring->GetPoints();
	std::string index = "outRingPointsIndex";
	for (glbInt32 i = 0; i < pointsCount * 3; i++)
	{
		index = "outRingPointsIndex";
		index += CGlbConvert::Int32ToStr(i);
		sprintf_s(str,"%8lf",points[i]);
		xmlNewTextChild(node,NULL,(unsigned char *)index.c_str(),BAD_CAST str);
	}

	xmlNodePtr rdnode = xmlNewNode(NULL,BAD_CAST "RenderInfo");
	xmlAddChild(node,rdnode);
	if(mpr_renderInfo)
	{
		sprintf_s(str,"%d",mpr_renderInfo->type);
		xmlNewTextChild(rdnode,NULL,BAD_CAST "RenderType",BAD_CAST str);

		GlbDigHoleSymbolInfo *digHoleInfo = (GlbDigHoleSymbolInfo *)mpr_renderInfo.get();
		digHoleInfo->Save(rdnode,prjPath);
	}
	return true;
}

glbDouble CGlbGlobeDigHole::GetDistance(osg::Vec3d &cameraPos,glbBool isCompute)
{
	if (!mpt_globe) return DBL_MAX;
	if (isCompute == false)
		return mpr_distance;
	if (mpt_isCameraAltitudeAsDistance)	
		mpr_distance = cameraPos.z();	
	else
	{
		osg::Vec3d centerPos;
		if (mpt_feature)
			mpt_feature->GetExtent()->GetCenter(&centerPos.x(),&centerPos.y(),&centerPos.z());
		else{
			if (!mpr_holeRegion)
				return DBL_MAX;
			mpr_holeRegion->GetExtent()->GetCenter(&centerPos.x(),&centerPos.y(),&centerPos.z());
		}
		if (mpt_globe->GetType() == GLB_GLOBETYPE_GLOBE)
		{
			osg::Vec3d cameraPos_w;
			g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(cameraPos.y()),osg::DegreesToRadians(cameraPos.x()),cameraPos.z(),cameraPos_w.x(),cameraPos_w.y(),cameraPos_w.z());
			g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(centerPos.y()),osg::DegreesToRadians(centerPos.x()),centerPos.z(),centerPos.x(),centerPos.y(),centerPos.z());
			mpr_distance = (centerPos - cameraPos_w).length();
		}else if (mpt_globe->GetType() == GLB_GLOBETYPE_FLAT)
			mpr_distance = (centerPos - cameraPos).length();
	}
	return mpr_distance;
}

void CGlbGlobeDigHole::LoadData(glbInt32 level)
{
	if(mpt_currLevel == level) return;
	if(mpt_preNode  !=  NULL)
	{//旧对象还未压入摘队列
		/*
		*   mpt_node = node1,preNode = NULL    -----> node1_h
		*   不做控制，且LoadData快
		*   {
		*       mpt_node = node2,preNode = node1
		*       mpt_node = node3,preNode = node2   -----> node1_h,node2_r,node2_h
		*       造成node1 永远不会摘除
		*   }
		*/
		return;
	}	
	{//与DirectDraw互斥.
		GlbScopedLock<GlbCriticalSection> lock(mpr_readData_critical);
		if (mpt_loadState == true)return;
		mpt_loadState = true;
	}		
	if(mpt_currLevel != level)
	{
		if(mpt_currLevel < level)
			level = mpt_currLevel + 1;
	}
	glbref_ptr<CGlbGlobeDigHoleTask> task = new CGlbGlobeDigHoleTask(this,level);
	mpt_globe->mpr_taskmanager->AddTask(task.get());
}

void CGlbGlobeDigHole::ReadData(glbInt32 level ,glbBool isDirect )
{
	if (!mpr_renderInfo)
	{
		GlbDigHoleSymbolInfo* info = new GlbDigHoleSymbolInfo;
		info->lineInfo = new GlbLinePixelSymbolInfo;
		info->lineInfo->lineColor = new GlbRenderColor;
		info->lineInfo->lineColor->value = 0xffff00ff;
		info->lineInfo->lineOpacity = new GlbRenderInt32;
		info->lineInfo->lineOpacity->value = 100;
		info->lineInfo->lineWidth = new GlbRenderDouble;
		info->lineInfo->lineWidth->value = 3;
		info->lineInfo->linePattern = new GlbRenderLinePattern;
		info->lineInfo->linePattern->value = GLB_LINE_DOTTED;
		info->belowTextureData = new GlbRenderString;
		info->belowTextureData->value = L"";
		info->sideTextureData = new GlbRenderString;
		info->sideTextureData->value = L"";
		mpr_renderInfo = info;
	}
	CGlbMultiPolygon* mutiPolygon3d = NULL;
	if (mpt_feature)
	{
		IGlbGeometry *geo = NULL;
		bool result = false;
		result = mpt_feature->GetGeometry(&geo,level);
		if(!result || geo == NULL) 
		{mpt_loadState=false;return;}

		// 使用智能指针防止多线程访问导致geo在Draw过程中被卸载
		glbref_ptr<IGlbGeometry> tempGeo = geo;

		GlbGeometryTypeEnum geoType = geo->GetType();
		switch(geoType)
		{
		case GLB_GEO_POLYGON:
			{
				CGlbPolygon* poly = dynamic_cast<CGlbPolygon*>(geo);
				if (poly != NULL)
				{
					if (mutiPolygon3d== NULL)					
						mutiPolygon3d = new CGlbMultiPolygon(poly->GetCoordDimension(),poly->HasM());
					
					mutiPolygon3d->AddPolygon(poly);
				}
			}					
			break;
		case GLB_GEO_MULTIPOLYGON:
			{
				CGlbMultiPolygon* multiPoly = dynamic_cast<CGlbMultiPolygon*>(geo);
				if (multiPoly != NULL)
				{
					if (mutiPolygon3d== NULL)				
						mutiPolygon3d = new CGlbMultiPolygon(multiPoly->GetCoordDimension(),multiPoly->HasM());

					glbInt32 polyCnt = multiPoly->GetCount();
					for (int k = 0; k < polyCnt; k++)
					{
						CGlbPolygon *polygon = const_cast<CGlbPolygon *>(multiPoly->GetPolygon(k));
						if (polygon==NULL) continue;
						mutiPolygon3d->AddPolygon(polygon);
					}
				}				
			}
			break;
		}
	}else{
		// 使用智能指针防止多线程访问导致mpr_holeRegion在Draw过程中被卸载
		glbref_ptr<CGlbPolygon> tempGeo = mpr_holeRegion;

		mutiPolygon3d = new CGlbMultiPolygon(mpr_holeRegion->GetCoordDimension(),mpr_holeRegion->HasM());
		mutiPolygon3d->AddPolygon(mpr_holeRegion.get());
	}
	//将旧的区域刷新
	DirtyOnTerrainObject();

	// 刷新新的区域
	mpr_outline = GetOutLine();
	DirtyOnTerrainObject();
	
	//非贴地部分
	osg::Switch* swiNode = new osg::Switch;
	for (glbInt32 i = 0;i < mutiPolygon3d->GetCount(); ++i)
	{
		CGlbPolygon* polygon3d = const_cast<CGlbPolygon *>(mutiPolygon3d->GetPolygon(i));
		osg::Node* node = NULL;
		glbref_ptr<CGlbGlobeDigHoleSymbol> digHoleSymbol = new CGlbGlobeDigHoleSymbol;
		node = digHoleSymbol->Draw(this,GetRegion(),GetDepth(),IsShowWall());
		//node = DrawDigHole3(GetRegion(),GetDepth());

		osg::MatrixTransform* MT = new osg::MatrixTransform;
		osg::Vec3d center;
		osg::Matrix mat;
		polygon3d->GetExtent()->GetCenter(&center.x(),&center.y(),&center.z());
		if (mpt_globe->GetType() == GLB_GLOBETYPE_GLOBE)
		{
			g_ellipsoidModel->computeLocalToWorldTransformFromLatLongHeight(osg::DegreesToRadians(center.y()),osg::DegreesToRadians(center.x()),center.z(),mat);
		}else if (mpt_globe->GetType() == GLB_GLOBETYPE_FLAT)
		{
			mat.makeIdentity();
			mat.makeTranslate(center.x(),center.y(),center.z());
		}
		MT->setMatrix(mat);
		MT->addChild(node);
		swiNode->addChild(MT);
	}

	if (mutiPolygon3d)
		delete mutiPolygon3d;

	if(mpr_needReReadData)
	{
		/*
		*   在检测mpr_needReReadData时,其它线程设置mpr_needReReadData=true还没执行！！！.
		*/
		mpr_needReReadData = false;
		return ReadData(level,true);
	}
	
	//增加使用内存
	glbInt32 objsize = this->ComputeNodeSize(swiNode);
	glbBool isOK = CGlbGlobeMemCtrl::GetInstance()->IncrementUsedMem(objsize);
	if(isOK||isDirect)
	{
		mpt_preNode   = mpt_node;
		mpt_node      = swiNode;
		mpt_currLevel = level;		
		mpr_objSize   = objsize;
	}else{
		swiNode = NULL;
	}
	mpt_loadState = false;	
}

void CGlbGlobeDigHole::DirectDraw(glbInt32 level)
{
	if (mpt_parent    == NULL) return;	
	{//与LoadData互斥.
		GlbScopedLock<GlbCriticalSection> lock(mpr_readData_critical);
		if (mpt_loadState == true)
		{
			/*
			    LoadData->ReadData，选择退出，但是会产生如下问题：
				        LoadData->ReadData 使用的是旧的渲染相关的属性，不能反映新设置的渲染属性.
				还必须直接退出，如果选择等待LoadData->ReadData的话，会堵塞界面线程！！！！！

				所以设置一个标志，ReadData 执行到末尾时，检查该标志,如果标示是true，ReadData在执行一次.
			*/
			mpr_needReReadData = true;
			return;
		}
		mpt_loadState = true;
	}
	if(mpt_preNode != NULL)
	{
	/*
	*    mpr_node = node2,preNode = node1  ----A线程调用AddToScene，
	*                                         还没有preNode Remove还没执行完
	*    下面ReadData 已经做完：
	*    mpr_ndoe = node3,preNode = node2   造成node1永远不会摘除.
	*/
		AddToScene();
	}
	/*
	*    因为在LoadData->ReadData时，ReadData 装载对象后发现超过内存限额
	*        会抛弃新加载的osg节点.
	*    但对DirectDraw而言：如果抛弃新的osg节点，就不能反映最新的变化.
	*    所以：
	*         ReadData方法新增一个参数，标记是DirectDraw调用的,不管是否超过
	*         内存限额,都不抛弃新的osg节点.
	*/
	ReadData(level,true);
	AddToScene();
	mpt_loadState = false;
}

void CGlbGlobeDigHole::AddToScene()
{
	if(mpt_node == NULL)return;
	GlbScopedLock<GlbCriticalSection> lock(mpr_addToScene_critical);
	bool needUpdate = false;
	if(mpt_preNode == NULL)
	{//没有产生新osg节点
		if(mpt_HangTaskNum == 0
			&& mpt_node->getNumParents() == 0
			/*&& mpt_altitudeMode != GLB_ALTITUDEMODE_ONTERRAIN*/)
		{//贴地模式,节点不需要挂.
			//挂mpt_node任务
			CGlbGlobeCallBack::CGlbGlobeCallBackTask* task = 
				new CGlbGlobeCallBack::CGlbGlobeCallBackTask(this,mpt_node);
			task->mpr_size = this->GetOsgNodeSize();				
			mpt_globe->mpr_p_callback->AddHangTask(task);
			mpt_HangTaskNum ++;
			needUpdate = true;
		}			
	}
	else
	{//产生了新osg节点						
		{//旧节点已经挂到场景树
			//摘旧节点mpt_preNode的任务
			CGlbGlobeCallBack::CGlbGlobeCallBackTask* task = 
				new CGlbGlobeCallBack::CGlbGlobeCallBackTask(this,mpt_preNode);
			task->mpr_size = this->ComputeNodeSize(mpt_preNode);
			mpt_globe->mpr_p_callback->AddRemoveTask(task);
			mpt_preNode = NULL;
			CGlbGlobeMemCtrl::GetInstance()->DecrementUsedMem(task->mpr_size);
		}
		//if(mpt_altitudeMode != GLB_ALTITUDEMODE_ONTERRAIN)
		{//挂新节点mpt_node任务
			CGlbGlobeCallBack::CGlbGlobeCallBackTask* task = 
				new CGlbGlobeCallBack::CGlbGlobeCallBackTask(this,mpt_node);
			task->mpr_size = this->GetOsgNodeSize();				
			mpt_globe->mpr_p_callback->AddHangTask(task);
			mpt_HangTaskNum ++;	
		}
		needUpdate = true;
	}
	if(needUpdate)
	{
		//UpdateWorldExtent();
		if (mpt_globe)
			mpt_globe->UpdateObject(this);
		glbBool isnew = mpt_isDispShow && mpt_isShow;
		if (isnew == false)			
			mpt_node->asSwitch()->setValue(0,false);
		if (mpt_isSelected)
			DealSelected();
		if(mpt_isBlink)
			DealBlink();
	}
}

glbInt32 CGlbGlobeDigHole::RemoveFromScene(glbBool isClean)
{
	{//与LoadData互斥.
		GlbScopedLock<GlbCriticalSection> lock(mpr_readData_critical);
		if (mpt_loadState == true)return 0;	
		mpt_loadState = true;
		/*
		*    mpt_node = node1,pre=NULL
		*    读:mpt_node = node2,pre=node1		
		*/
	}
	GlbScopedLock<GlbCriticalSection> lock(mpr_addToScene_critical);
	//处理贴地面	
	//if(    mpt_altitudeMode == GLB_ALTITUDEMODE_ONTERRAIN )
	{		
		DirtyOnTerrainObject();
	}
	glbInt32 tsize = 0;
	if(isClean == false)
	{//从显存卸载对象，节约显存.
		if ((mpt_node != NULL))// && (mpt_node->getNumParents() != 0))
		{//obj在显存	
			/*
			     mpt_node 已经在挂队列，但是还没挂到场景树上
				 这时判断getNumParents() != 0 是不可以的.
			*/
			CGlbGlobeCallBack::CGlbGlobeCallBackTask *task = 
				new CGlbGlobeCallBack::CGlbGlobeCallBackTask(this,mpt_node);
			task->mpr_size = this->GetOsgNodeSize();
			mpt_globe->mpr_p_callback->AddRemoveTask(task);
			tsize = task->mpr_size;
		}
	}
	else
	{//从内存中卸载对象，节约内存
		//删除 上一次装载的节点		
		if (mpt_preNode != NULL)
		{
			//if(mpt_preNode->getNumParents() > 0 )
			{				
				CGlbGlobeCallBack::CGlbGlobeCallBackTask* task = 
					new CGlbGlobeCallBack::CGlbGlobeCallBackTask(this,mpt_preNode);
				task->mpr_size = ComputeNodeSize(mpt_preNode);
				mpt_globe->mpr_p_callback->AddRemoveTask(task);
			}
			tsize += ComputeNodeSize(mpt_preNode);
			mpt_preNode = NULL;
		}
		//删除当前节点
		if (mpt_node != NULL) 
		{
			//if(mpt_node->getNumParents() > 0)
			{		
				CGlbGlobeCallBack::CGlbGlobeCallBackTask* task = 
					new CGlbGlobeCallBack::CGlbGlobeCallBackTask(this,mpt_node);
				task->mpr_size = this->GetOsgNodeSize();
				mpt_globe->mpr_p_callback->AddRemoveTask(task);
			}
			tsize       += this->GetOsgNodeSize();			
			mpt_node    =  NULL;
			mpr_objSize =  0;
		}		
		if (mpt_feature != NULL && mpt_featureLayer != NULL)
		{
			CGlbGlobeFeatureLayer* fl = dynamic_cast<CGlbGlobeFeatureLayer*>(mpt_featureLayer);
			if (fl)
				fl->NotifyFeatureDelete(mpt_feature->GetOid());
		}
		mpt_currLevel = -1;					
	}
	mpt_loadState = false;
	//减少使用内存
	if(tsize>0 && isClean)
	{
		CGlbGlobeMemCtrl::GetInstance()->DecrementUsedMem(tsize);		
	}
	return tsize;	
}

void CGlbGlobeDigHole::DrawToImage(glbByte *image,glbInt32 imageW,glbInt32 imageH,CGlbExtent &ext)
{
	/*
	*	调度控制对象隐藏时，不能渲染，直接返回
	*/
	if (!mpr_isCanDrawImage)return;
	if (mpr_renderInfo==NULL) return;
	glbref_ptr<GlbDigHoleSymbolInfo> digHoleSymbolInfo = dynamic_cast<GlbDigHoleSymbolInfo *>(mpr_renderInfo.get());
	if (digHoleSymbolInfo==NULL) return;

	glbref_ptr<IGlbGeometry> polys = AnalysisPolyGeomertry(mpt_feature.get(),mpr_holeRegion.get());
	if (!polys) return;

	CGlbGlobeDigHoleSymbol digHoleSymbol;
	digHoleSymbol.DrawToImage(this,polys.get(),image,imageW,imageH,ext);
}

glbref_ptr<CGlbExtent> CGlbGlobeDigHole::GetBound(glbBool isWorld /* = true */)
{
	if (mpt_globe == NULL && isWorld) return NULL;
	glbref_ptr<CGlbExtent> geoExt = NULL;
	if(    mpt_altitudeMode==GLB_ALTITUDEMODE_ONTERRAIN
			&& mpr_outline != NULL)
	{//贴地形绘制模式
		/*
		*   索引、查询【qtree】才能准确. z == 0.
		*/
		geoExt = const_cast<CGlbExtent *>(mpr_outline->GetExtent());
	}else{
		if(mpt_feature)
		{
			geoExt = const_cast<CGlbExtent *>(mpt_feature->GetExtent());
		}
		else if(mpr_holeRegion)
		{
			geoExt = const_cast<CGlbExtent *>(mpr_holeRegion->GetExtent());	
		}
	}
	if(isWorld == false)
	{//地理坐标		
		/*
		   对象添加时，对贴地形对象,在场景树中是用的地理坐标!!!!
		   贴地形绘制，GetDistance会用到mpr_elevate.
		   所以：
		        调用UpdateElevate().
		*/
		/*if( mpr_elevation == DBL_MAX
			&& mpt_altitudeMode==GLB_ALTITUDEMODE_ONTERRAIN
			&& mpr_holeRegion
		  )
			   UpdateElevate();*/
		/*
		*   地理坐标，不反应相对地形绘制的地形海拔.
		*/
		return geoExt;
	}
	else
	{//世界坐标		
		if(geoExt == NULL)return NULL;
		glbref_ptr<CGlbExtent> worldExt = new CGlbExtent();
		GlbGlobeTypeEnum globeType = mpt_globe->GetType();
		if(mpt_altitudeMode==GLB_ALTITUDEMODE_ONTERRAIN)
		{//贴地形绘制
			double x,y,z;
			geoExt->GetMin(&x,&y,&z);
			z = mpt_globe->GetElevationAt(x,y);
			if(globeType == GLB_GLOBETYPE_GLOBE)
			{
				g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(y),
					osg::DegreesToRadians(x),z,x,y,z);
			}
			worldExt->Merge(x,y,z);
			geoExt->GetMax(&x,&y,&z);
			z = mpt_globe->GetElevationAt(x,y);
			if(globeType == GLB_GLOBETYPE_GLOBE)
			{
				g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(y),
					osg::DegreesToRadians(x),z,x,y,z);
			}
			worldExt->Merge(x,y,z);			
		}
		else
		{//非贴地形.
			osg::ref_ptr<osg::Node> node = mpt_node;
			if(   node != NULL
				&&node->getBound().valid() )
			{				
				CGlbCalculateBoundBoxVisitor bboxV;
				node->accept(bboxV);
				osg::BoundingBoxd bb =bboxV.getBoundBox();
				worldExt->SetMin(bb.xMin(),bb.yMin(),bb.zMin());
				worldExt->SetMax(bb.xMax(),bb.yMax(),bb.zMax());
			}
			else
			{//osg 节点还没生成.	
				/*if( mpr_elevation == DBL_MAX)
				   UpdateElevate();*/
				double x,y,z;
				geoExt->GetMin(&x,&y,&z);				
				if(globeType == GLB_GLOBETYPE_GLOBE)
				{
					g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(y),
						osg::DegreesToRadians(x),z,x,y,z);					
				}
				worldExt->Merge(x,y,z);

				geoExt->GetMax(&x,&y,&z);				
				if(globeType == GLB_GLOBETYPE_GLOBE)
				{
					g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(y),
						osg::DegreesToRadians(x),z,x,y,z);
				}
				worldExt->Merge(x,y,z);
			}
		}//非贴地模式
		return worldExt;
	}//世界坐标	
	return NULL;
}

IGlbGeometry* CGlbGlobeDigHole::GetOutLine()
{
	CGlbMultiPolygon* multiPolygon = new CGlbMultiPolygon;
	if (mpt_feature)
	{
		IGlbGeometry *geo = NULL;
		glbBool result = false;
		result = mpt_feature->GetGeometry(&geo,mpt_currLevel);
		if (result && geo)
		{
			GlbGeometryTypeEnum geoType = geo->GetType();
			switch(geoType)
			{
			case GLB_GEO_POLYGON:
				{
					CGlbPolygon *polygon3d = static_cast<CGlbPolygon*>(geo);
					CGlbPolygon *polygon = new CGlbPolygon;
					CGlbLine *line3d = const_cast<CGlbLine *>(polygon3d->GetExtRing());
					glbInt32 dimension = line3d->GetCoordDimension();
					CGlbLine *ln = new CGlbLine;
					for (glbInt32 j = 0 ;j < line3d->GetCount();j++)
					{
						osg::Vec3d point;
						if (dimension==3)
							line3d->GetPoint(j,&point.x(),&point.y(),&point.z());
						else if (dimension == 2)
							line3d->GetPoint(j,&point.x(),&point.y());						
						ln->AddPoint(point.x(),point.y());
					}
					polygon->SetExtRing(ln);
					multiPolygon->AddPolygon(polygon);
				}					
				break;
			case GLB_GEO_MULTIPOLYGON:
				{
					CGlbMultiPolygon* multiPoly = dynamic_cast<CGlbMultiPolygon*>(geo);
					glbInt32 polyCnt = multiPoly->GetCount();
					for (int k = 0; k < polyCnt; k++)
					{
						CGlbPolygon *polygon3d = const_cast<CGlbPolygon *>(multiPoly->GetPolygon(k));
						CGlbPolygon *polygon = new CGlbPolygon;
						CGlbLine *line3d = const_cast<CGlbLine *>(polygon3d->GetExtRing());
						glbInt32 dimension = line3d->GetCoordDimension();
						CGlbLine *ln = new CGlbLine;
						for (glbInt32 j = 0 ;j < line3d->GetCount();j++)
						{
							osg::Vec3d point;
							if (dimension==3)
								line3d->GetPoint(j,&point.x(),&point.y(),&point.z());
							else if (dimension == 2)
								line3d->GetPoint(j,&point.x(),&point.y());		
							
							ln->AddPoint(point.x(),point.y());
						}
						polygon->SetExtRing(ln);
						multiPolygon->AddPolygon(polygon);
					}
				}
				break;
			}
		}
	}else{
		CGlbLine* line3d = const_cast<CGlbLine *>(mpr_holeRegion->GetExtRing());	//只有外环，无内环
		CGlbLine* ln = new CGlbLine;
		glbInt32 dimension = line3d->GetCoordDimension();
		glbDouble ptx,pty,ptz;
		for (glbInt32 i = 0; i < line3d->GetCount(); i++)
		{	
			if (dimension==3)
				line3d->GetPoint(i,&ptx,&pty,&ptz);
			else if (dimension == 2)
				line3d->GetPoint(i,&ptx,&pty);
			ln->AddPoint(ptx,pty);
		}
		CGlbPolygon* polygon = new CGlbPolygon;
		polygon->SetExtRing(ln);
		multiPolygon->AddPolygon(polygon);
	}	
	return multiPolygon;
}

glbInt32 CGlbGlobeDigHole::GetOsgNodeSize()
{
	if (mpt_node == NULL) return 0;
	if (mpr_objSize==0)
		mpr_objSize = ComputeNodeSize(mpt_node);
	return mpr_objSize;
}

void CGlbGlobeDigHole::SetRegion(CGlbPolygon *region ,glbBool &isNeedDirectDraw)
{
	isNeedDirectDraw = false;
	if(mpt_feature)
	{// 如果有要素，位置信息来源于要素.
		mpr_holeRegion = region;
		return;
	}

	{   		
		GlbScopedLock<GlbCriticalSection> lock(mpr_readData_critical);
		if (mpt_loadState == true)
		{
			/*
			 *  如果正在ReadData，ReadData可能在新的地形模式之前使用了旧的位置.
			 *  所以：
					需要告知调用者，调用DirectDraw.
			*/
			mpr_holeRegion = region;
			isNeedDirectDraw = true;						
			return;
		}
		mpt_loadState = true;
	}

	mpr_holeRegion = region;
	if(mpt_globe)
	{
		/*
		    如果mpr_lineGeo==NULL,对象是不在场景树上的.
			如果不UpdateObject，对象永远不在场景树上，这样永远不会调度该对象.
		*/
		mpt_globe->UpdateObject(this);
	}
	if (mpt_node       != NULL)
	{// lineGeo改变---->重绘	
		isNeedDirectDraw = true;	
		/*
		    因为： 如果外部调用没有理会：isNeedDirectDraw
			    且：对象的level也没有发生变化的情况,调度也不会更新osgNode
			所以：mpt_currLevel = -1，使调度可以LoadData.
			        
		*/
		mpt_currLevel = -1;
	}
	mpt_loadState = false;	
}

CGlbPolygon* CGlbGlobeDigHole::GetRegion()
{
	return mpr_holeRegion.get();
}

void CGlbGlobeDigHole::SetDepth(double depth ,glbBool &isNeedDirectDraw)
{	
	isNeedDirectDraw = false;
	if(mpt_feature)
	{// 如果有要素，位置信息来源于要素.
		mpr_holeDepth = depth;
		return;
	}

	{   		
		GlbScopedLock<GlbCriticalSection> lock(mpr_readData_critical);
		if (mpt_loadState == true)
		{
			/*
			 *  如果正在ReadData，ReadData可能在新的地形模式之前使用了旧的位置.
			 *  所以：
					需要告知调用者，调用DirectDraw.
			*/
			mpr_holeDepth = depth;
			isNeedDirectDraw = true;						
			return;
		}
		mpt_loadState = true;
	}

	mpr_holeDepth = depth;

	if (mpt_node       != NULL)
	{// lineGeo改变---->重绘
		isNeedDirectDraw = true;	
	}
	mpt_loadState = false;
}

glbDouble CGlbGlobeDigHole::GetDepth()
{
	return mpr_holeDepth;;
}

void CGlbGlobeDigHole::ShowWall(glbBool isshow)
{		
	if (mpr_isShowWall == isshow) 
		return;
	mpr_isShowWall = isshow;
	// 没有绘制则强制绘制
	if (mpr_isShowWall && mpt_node && mpt_node->asGroup()->getNumChildren()<=0)
		DirectDraw(0);

	if (mpt_node && mpt_node->asGroup()->getNumChildren()>0)
	{		
		osg::Node * node = mpt_node->asGroup()->getChild(0);
		if (mpr_isShowWall)
			node->setNodeMask(0xffffffff);
		else
			node->setNodeMask(0x0);
		//// 这也写的太让人晕了吧！！！！！！！！！
		//osg::Switch* sw = mpt_node->asSwitch()->getChild(0)->asTransform()->getChild(0)->asGroup()->getChild(1)->asSwitch();
		//if (sw)
		//{
		//	sw->setValue(0,mpr_isShowWall);
		//	sw->setValue(1,mpr_isShowWall);
		//}
	}
	return;
}

glbBool CGlbGlobeDigHole::IsShowWall()
{
	return mpr_isShowWall;
}

glbBool CGlbGlobeDigHole::SetRenderInfo(GlbRenderInfo *renderInfo ,glbBool &isNeedDirectDraw)
{
	if (renderInfo == NULL || renderInfo->type != GLB_OBJECTTYPE_DIGHOLE)
		return false;
	GlbDigHoleSymbolInfo* digholeInfo = dynamic_cast<GlbDigHoleSymbolInfo*>(renderInfo);
	if (!digholeInfo)	return false;
	isNeedDirectDraw = false;
	{//与LoadData互斥.
		/*
		*   这是因为： ReadData执行在中途时，生成的结果可能不会反应 新设置的属性.
		*/
		GlbScopedLock<GlbCriticalSection> lock(mpr_readData_critical);
		if (mpt_loadState == true)
		{
			isNeedDirectDraw = true;
			mpr_renderInfo   = renderInfo;
			return true;			
		}
		mpt_loadState = true;
	}
	if(mpt_node == NULL)
	{		
		mpr_renderInfo   = renderInfo;
		mpt_loadState    = false;
		return true;
	}	

	glbref_ptr<GlbDigHoleSymbolInfo> olddigholeInfo = dynamic_cast<GlbDigHoleSymbolInfo*>(mpr_renderInfo.get());		
	mpr_renderInfo = renderInfo;
	if (olddigholeInfo && digholeInfo && olddigholeInfo->lineInfo && digholeInfo->lineInfo)
	{
		if (   !CGlbCompareValue::CompareValueColorEqual(olddigholeInfo->lineInfo->lineColor,digholeInfo->lineInfo->lineColor,mpt_feature.get()) 
			|| !CGlbCompareValue::CompareValueIntEqual(olddigholeInfo->lineInfo->lineOpacity,digholeInfo->lineInfo->lineOpacity,mpt_feature.get()) 
			|| !CGlbCompareValue::CompareValueDoubleEqual(olddigholeInfo->lineInfo->lineWidth,digholeInfo->lineInfo->lineWidth,mpt_feature.get()) 
			|| !CGlbCompareValue::CompareValueLinePatternEnumEqual(olddigholeInfo->lineInfo->linePattern,digholeInfo->lineInfo->linePattern,mpt_feature.get()))
		{
			isNeedDirectDraw = true;
			mpt_loadState  = false;
			return true;
		}
	}
	
	if (olddigholeInfo && digholeInfo)
	{
		if (!CGlbCompareValue::CompareValueStringEqual(olddigholeInfo->belowTextureData,digholeInfo->belowTextureData,mpt_feature.get()))
		{
			if (mpt_node->asGroup()->getNumChildren()>0 && mpt_node->asGroup()->getChild(0)->asGroup()->getNumChildren()>0)
			{
				osg::Switch* sw = mpt_node->asSwitch()->getChild(0)->asTransform()->getChild(0)->asGroup()->getChild(1)->asSwitch();

				osg::ref_ptr<osg::Texture2D> Belowtex2d = new osg::Texture2D;
				CGlbWString texdata = digholeInfo->belowTextureData->GetValue(mpt_feature.get());
				glbInt32 index = texdata.find_first_of(L'.');
				if(index == 0)
				{
					CGlbWString execDir = CGlbPath::GetExecDir();
					texdata = execDir + texdata.substr(1,texdata.size());
				}
				osg::ref_ptr<osg::Image> image0 = osgDB::readImageFile(texdata.ToString());
				if (image0.valid())
					Belowtex2d->setImage(0,image0.get());
				Belowtex2d->setWrap(osg::Texture::WRAP_S,osg::Texture2D::CLAMP);
				Belowtex2d->setWrap(osg::Texture::WRAP_T,osg::Texture2D::CLAMP);
				//设置纹理环境
				osg::ref_ptr<osg::TexEnv> texenv=new osg::TexEnv;
				texenv->setMode(osg::TexEnv::REPLACE);
				//启用纹理单元0
				sw->getChild(0)->getOrCreateStateSet()->setTextureAttributeAndModes(0,Belowtex2d.get(),osg::StateAttribute::ON);
				//设置纹理环境
				sw->getChild(0)->getOrCreateStateSet()->setTextureAttribute(0,texenv.get());
			}
		}
		else
			isNeedDirectDraw = true;

		if (!CGlbCompareValue::CompareValueStringEqual(olddigholeInfo->sideTextureData,digholeInfo->sideTextureData,mpt_feature.get()))
		{
			if (mpt_node->asGroup()->getNumChildren()>0 && mpt_node->asGroup()->getChild(0)->asGroup()->getNumChildren()>0)
			{
				osg::Switch* sw = mpt_node->asSwitch()->getChild(0)->asTransform()->getChild(0)->asGroup()->getChild(1)->asSwitch();

				osg::ref_ptr<osg::Texture2D> Sidetex2d = new osg::Texture2D;
				CGlbWString texdata = digholeInfo->sideTextureData->GetValue(mpt_feature.get());
				glbInt32 index = texdata.find_first_of(L'.');
				if(index == 0)
				{
					CGlbWString execDir = CGlbPath::GetExecDir();
					texdata = execDir + texdata.substr(1,texdata.size());
				}
				osg::ref_ptr<osg::Image> image0 = osgDB::readImageFile(texdata.ToString());
				if (image0.valid())
					Sidetex2d->setImage(0,image0.get());
				Sidetex2d->setWrap(osg::Texture::WRAP_S,osg::Texture2D::CLAMP);
				Sidetex2d->setWrap(osg::Texture::WRAP_T,osg::Texture2D::CLAMP);
				//设置纹理环境
				osg::ref_ptr<osg::TexEnv> texenv=new osg::TexEnv;
				texenv->setMode(osg::TexEnv::REPLACE);
				//启用纹理单元0
				sw->getChild(1)->getOrCreateStateSet()->setTextureAttributeAndModes(0,Sidetex2d.get(),osg::StateAttribute::ON);
				//设置纹理环境
				sw->getChild(1)->getOrCreateStateSet()->setTextureAttribute(0,texenv.get());
			}
		}
		else
			isNeedDirectDraw = true;
	}

	mpt_loadState  = false;
	return false;
}

GlbRenderInfo* CGlbGlobeDigHole::GetRenderInfo()
{
	return mpr_renderInfo.get();
}

glbBool CGlbGlobeDigHole::SetShow(glbBool isShow,glbBool isOnState)
{	
	if(mpt_node == NULL && isOnState==true)
		return false;

	if (mpt_isEdit && isOnState)// 编辑状态不再由调度控制显隐
		return true;
	/*
	* 调度器根据可见范围控制对象显示和隐藏此时isOnState==true,isShow==true或false
	*/
	
	glbBool isold = mpt_isDispShow && mpt_isShow;

	if(isOnState)
	{// 调度器控制显隐
		mpt_isDispShow = isShow;
	}
	else
	{// 用户控制显隐
		mpt_isShow = isShow;
	}

	glbBool isnew = mpt_isDispShow && mpt_isShow;
	
	if(isold == isnew)
		return true;

	glbref_ptr<GlbDigHoleSymbolInfo> dhInfo = static_cast<GlbDigHoleSymbolInfo *>(mpr_renderInfo.get());
	if (!dhInfo)	
		return false;	

	//if( mpt_altitudeMode == GLB_ALTITUDEMODE_ONTERRAIN )
	{// 如果是贴地绘制对象，需要刷新贴地区域
		/*
		* mpr_isCanDrawImage控制在DrawToImage时是否需要绘制
		*/
		if (mpr_isCanDrawImage != isnew)
		{
			mpr_isCanDrawImage = isnew;
			// 刷新贴地区域
			DirtyOnTerrainObject();		
		}
	}
	//else
	{
		if (mpt_node)
		{
			if(isnew)
			{
				mpt_node->asSwitch()->setAllChildrenOn();
			}
			else
				mpt_node->asSwitch()->setAllChildrenOff();
		}
	}
	return true;
}

glbBool CGlbGlobeDigHole::SetSelected(glbBool isSelected)
{
	return false;
}

glbBool CGlbGlobeDigHole::SetBlink(glbBool isBlink)
{
	return false;
}

glbInt32 CGlbGlobeDigHole::GetDiggedCount()
{
	return mpr_diggedobjs.size();
}

glbInt32 CGlbGlobeDigHole::ComputeNodeSize( osg::Node *node )
{
	CGlbCalculateNodeSizeVisitor cnsv;
	node->accept(cnsv);
	return cnsv.getTextureSize() + cnsv.getNodeMemSize();
}

CGlbGlobeObject* CGlbGlobeDigHole::GetDiggedObject(glbInt32 objId)
{
	glbInt32 siz = mpr_diggedobjs.size();
	if (siz > objId)
		return mpr_diggedobjs[objId];
	return NULL;
}

glbBool CGlbGlobeDigHole::AddDiggedObject(CGlbGlobeRObject* obj)
{
	//设置对象被蒙版所蒙掉
	osg::ref_ptr<osg::Stencil> stencil = new osg::Stencil;
	stencil->setFunction(osg::Stencil::EQUAL,0,0xff);
	stencil->setOperation(osg::Stencil::KEEP,osg::Stencil::KEEP,osg::Stencil::KEEP);
	osg::Node* node = obj->GetOsgNode();
	node->getOrCreateStateSet()->setAttributeAndModes(stencil,osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);	
	osg::Node* pTerrianNode = obj->GetGlobe()->GetTerrain()->GetNode();
	glbInt32 terrianRenderBin = pTerrianNode->getOrCreateStateSet()->getRenderingHint();
	node->getOrCreateStateSet()->setRenderBinDetails(terrianRenderBin + 1,"RenderBin");
	mpr_diggedobjs.push_back(obj);
	return true;
}

glbBool CGlbGlobeDigHole::RemoveDiggedObject(glbInt32 objId)
{
	glbInt32 siz = mpr_diggedobjs.size();
	if (siz > objId)
	{
		//设置对象不被蒙版所蒙掉
		CGlbGlobeRObject* obj = mpr_diggedobjs[objId];
		osg::ref_ptr<osg::Stencil> stencil = new osg::Stencil;
		stencil->setFunction(osg::Stencil::EQUAL,0,0xff);
		stencil->setOperation(osg::Stencil::KEEP,osg::Stencil::KEEP,osg::Stencil::KEEP);
		osg::Node* node = obj->GetOsgNode();
		node->getOrCreateStateSet()->setAttributeAndModes(stencil,osg::StateAttribute::OFF | osg::StateAttribute::OVERRIDE);	

		mpr_diggedobjs.erase(mpr_diggedobjs.begin()+objId);
		return true;
	}	
	return false;
}

//直接蒙版,弃用
osg::Node* CGlbGlobeDigHole::DrawDigHole(CGlbPolygon* polygon3d,glbDouble depth)
{
	CGlbLine* line3d = NULL;
	//获取上下底面地理坐标
	osg::ref_ptr<osg::Vec3dArray> AboveFaceVertexs = new osg::Vec3dArray;
	osg::ref_ptr<osg::Vec3dArray> BelowFaceVertexs = new osg::Vec3dArray;
	osg::ref_ptr<osg::Vec3dArray> SideFaceVertexs = new osg::Vec3dArray;

	line3d = const_cast<CGlbLine*>(polygon3d->GetExtRing());
	for (glbInt32 i = 0;i < line3d->GetCount();++i)
	{
		osg::Vec3d point;
		line3d->GetPoint(i,&point.x(),&point.y(),&point.z());
		if (mpt_globe->GetType() == GLB_GLOBETYPE_GLOBE)
		{
			osg::Vec3d point2 = osg::Vec3d(point.x(),point.y(),point.z()-depth);
			osg::Vec3d pointup = osg::Vec3d(point.x(),point.y(),mpr_holeUpHei+point.z());
			osg::Vec3d pointdown = osg::Vec3d(point.x(),point.y(),mpr_holeDownHei+point.z());
			g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(point.y()),osg::DegreesToRadians(point.x()),point.z(),point.x(),point.y(),point.z());
			g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(point2.y()),osg::DegreesToRadians(point2.x()),point2.z(),point2.x(),point2.y(),point2.z());
			g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(pointup.y()),osg::DegreesToRadians(pointup.x()),pointup.z(),pointup.x(),pointup.y(),pointup.z());
			g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(pointdown.y()),osg::DegreesToRadians(pointdown.x()),pointdown.z(),pointdown.x(),pointdown.y(),pointdown.z());
			AboveFaceVertexs->push_back(pointup);
			AboveFaceVertexs->push_back(pointdown);
			BelowFaceVertexs->push_back(point2);
			SideFaceVertexs->push_back(point);
			SideFaceVertexs->push_back(point2);
		}else if (mpt_globe->GetType() == GLB_GLOBETYPE_FLAT)
		{
			osg::Vec3d point2 = osg::Vec3d(point.x(),point.y(),point.z()-depth);
			osg::Vec3d pointup = osg::Vec3d(point.x(),point.y(),mpr_holeUpHei+point.z());
			osg::Vec3d pointdown = osg::Vec3d(point.x(),point.y(),mpr_holeDownHei+point.z());
			AboveFaceVertexs->push_back(pointup);
			AboveFaceVertexs->push_back(pointdown);
			BelowFaceVertexs->push_back(point2);
			SideFaceVertexs->push_back(point);
			SideFaceVertexs->push_back(point2);
		}
	}

	//顶点偏移，以对象中心为原点地理坐标系，世界坐标转换成区域坐标
	osg::Vec3d center;
	osg::Matrixd worldTocenter;
	polygon3d->GetExtent()->GetCenter(&center.x(),&center.y(),&center.z());
	if (mpt_globe->GetType() == GLB_GLOBETYPE_GLOBE)
	{	
		osg::Matrixd centerToworld;
		g_ellipsoidModel->computeLocalToWorldTransformFromLatLongHeight(osg::DegreesToRadians(center.y()),osg::DegreesToRadians(center.x()),center.z(),centerToworld);
		worldTocenter = osg::Matrixd::inverse(centerToworld);
	}else if (mpt_globe->GetType() == GLB_GLOBETYPE_FLAT)
	{
		worldTocenter.makeIdentity();
		worldTocenter.makeTranslate(-center.x(),-center.y(),-center.z());
	}
	osg::ref_ptr<osg::Vec3dArray> StencilAboveFaceVertexs = new osg::Vec3dArray;
	osg::ref_ptr<osg::Vec3dArray> StencilBelowFaceVertexs = new osg::Vec3dArray;
	for (glbInt32 i = 0;i < line3d->GetCount();++i)
	{
		(*AboveFaceVertexs)[2*i] = (*AboveFaceVertexs)[2*i] * worldTocenter;
		(*AboveFaceVertexs)[2*i+1] = (*AboveFaceVertexs)[2*i+1] * worldTocenter;
		(*BelowFaceVertexs)[i] = (*BelowFaceVertexs)[i] * worldTocenter;
		(*SideFaceVertexs)[2*i] = (*SideFaceVertexs)[2*i] * worldTocenter;
		(*SideFaceVertexs)[2*i+1] = (*SideFaceVertexs)[2*i+1] * worldTocenter;
		StencilAboveFaceVertexs->push_back((*AboveFaceVertexs)[2*i]);
		StencilBelowFaceVertexs->push_back((*AboveFaceVertexs)[2*i+1]);
	}
	AboveFaceVertexs->push_back((*AboveFaceVertexs)[0]);
	AboveFaceVertexs->push_back((*AboveFaceVertexs)[1]);
	SideFaceVertexs->push_back((*SideFaceVertexs)[0]);
	SideFaceVertexs->push_back((*SideFaceVertexs)[1]);

	osg::Group* group = new osg::Group;
	osg::ref_ptr<osg::Geode> geodeAbove = new osg::Geode;
	osg::ref_ptr<osg::Geode> geodeBelow = new osg::Geode;
	osg::ref_ptr<osg::Geode> geodeSide = new osg::Geode;
	osg::ref_ptr<osg::Geometry> geomStencilAbove = new osg::Geometry;	//蒙版地上面
	osg::ref_ptr<osg::Geometry> geomStencilBelow = new osg::Geometry;	//蒙版地下面
	osg::ref_ptr<osg::Geometry> geomStencilSide = new osg::Geometry;	//蒙版侧面
	osg::ref_ptr<osg::Geometry> geomBelow = new osg::Geometry;	//下底面
	osg::ref_ptr<osg::Geometry> geomSide = new osg::Geometry;	//侧面

	//蒙版几何体,此几何体全透明
	geomStencilAbove->setVertexArray(StencilAboveFaceVertexs.get());
	geomStencilAbove->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::POLYGON,0,StencilAboveFaceVertexs->size()));
	geomStencilBelow->setVertexArray(StencilBelowFaceVertexs.get());
	geomStencilBelow->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::POLYGON,0,StencilBelowFaceVertexs->size()));
	geomStencilSide->setVertexArray(AboveFaceVertexs.get());
	geomStencilSide->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUAD_STRIP,0,AboveFaceVertexs->size()));

	//下底面
	geomBelow->setVertexArray(BelowFaceVertexs.get());
	geomBelow->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::POLYGON,0,BelowFaceVertexs->size()));
	//侧面
	geomSide->setVertexArray(SideFaceVertexs.get());
	geomSide->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUAD_STRIP,0,SideFaceVertexs->size()));

	osg::ref_ptr<osgUtil::Tessellator> tscx = new osgUtil::Tessellator(); 
	tscx->setTessellationType(osgUtil::Tessellator::TESS_TYPE_GEOMETRY);
	tscx->setBoundaryOnly(false);
	tscx->setWindingType( osgUtil::Tessellator::TESS_WINDING_ODD);
	tscx->setTessellationNormal(osg::Vec3(0.0,0.0,1.0));
	tscx->retessellatePolygons(*geomStencilAbove.get());
	geodeAbove->addDrawable(geomStencilAbove.get());
	tscx->setTessellationNormal(osg::Vec3(0.0,0.0,1.0));
	tscx->retessellatePolygons(*geomStencilBelow.get());
	geodeAbove->addDrawable(geomStencilBelow.get());	
	//侧面
	tscx->setTessellationNormal(osg::Vec3(0.0,0.0,1.0));
	tscx->retessellatePolygons(*geomStencilSide.get());
	geodeAbove->addDrawable(geomStencilSide.get());

	geodeBelow->addDrawable(geomBelow.get());
	geodeSide->addDrawable(geomSide.get());

	group->addChild(geodeAbove.get());
	osg::ref_ptr<osg::Switch> wallnode = new osg::Switch;
	wallnode->addChild(geodeBelow.get());
	wallnode->addChild(geodeSide.get());
	wallnode->setValue(0,mpr_isShowWall);
	wallnode->setValue(1,mpr_isShowWall);
	group->addChild(wallnode.get());	//1

	////地形 stencil 设置
	//osg::ref_ptr<osg::Stencil> terrianStencil = new osg::Stencil;
	//terrianStencil->setFunction(osg::Stencil::EQUAL,0,0xff);
	//terrianStencil->setOperation(osg::Stencil::ZERO,osg::Stencil::KEEP,osg::Stencil::KEEP);
	osg::Node* pTerrianNode = mpt_globe->GetTerrain()->GetNode();
	//osg::ref_ptr<osg::StateSet> statesetBin2 = pTerrianNode->getOrCreateStateSet();
	glbInt32 terrianRenderBin = pTerrianNode->getOrCreateStateSet()->getRenderingHint();
	//statesetBin2->setAttributeAndModes(terrianStencil,osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);	

	//面片 stencil 设置
	osg::ref_ptr<osg::Stencil> groupStencil = new osg::Stencil;
	groupStencil->setFunction(osg::Stencil::ALWAYS,1,0xff);
	groupStencil->setOperation(osg::Stencil::KEEP,osg::Stencil::REPLACE,osg::Stencil::REPLACE);
	osg::ref_ptr<osg::StateSet> statesetBin1 = group->getChild(0)->getOrCreateStateSet();
	statesetBin1->setRenderBinDetails(terrianRenderBin - 1,"RenderBin");
	statesetBin1->setAttributeAndModes(groupStencil,osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
	osg::ref_ptr<osg::ColorMask> colorMask = new osg::ColorMask();
	colorMask->setMask(false,false,false,false);
	statesetBin1->setAttribute(colorMask.get());

	//蒙版材质透明度
	osg::ref_ptr<osg::Material> aboveMaterial = new osg::Material;
	aboveMaterial->setAmbient(osg::Material::FRONT_AND_BACK,osg::Vec4(0.0f,0.0f,1.0f,1.0f));
	aboveMaterial->setDiffuse(osg::Material::FRONT_AND_BACK,osg::Vec4(0.5f,0.5f,0.5f,0.8f));
	aboveMaterial->setAlpha(osg::Material::FRONT_AND_BACK,0.0f);
	geodeAbove->getOrCreateStateSet()->setAttribute(aboveMaterial.get(),osg::StateAttribute::ON | osg::StateAttribute::PROTECTED);
	//下底面材质，纹理
	osg::ref_ptr<osg::Material> belowMaterial = new osg::Material;
	belowMaterial->setAmbient(osg::Material::FRONT_AND_BACK,osg::Vec4(1.0f,0.0f,0.0f,1.0f));
	belowMaterial->setDiffuse(osg::Material::FRONT_AND_BACK,osg::Vec4(0.5f,0.5f,0.5f,0.8f));
	geodeBelow->getOrCreateStateSet()->setAttribute(belowMaterial.get(),osg::StateAttribute::ON);
	//侧面材质，纹理
	osg::ref_ptr<osg::Material> sideMaterial = new osg::Material;
	sideMaterial->setAmbient(osg::Material::FRONT_AND_BACK,osg::Vec4(0.0f,1.0f,0.0f,1.0f));
	sideMaterial->setDiffuse(osg::Material::FRONT_AND_BACK,osg::Vec4(0.5f,0.5f,0.5f,0.8f));
	geodeSide->getOrCreateStateSet()->setAttribute(sideMaterial.get(),osg::StateAttribute::ON);

	return group;
}
//弃用
osg::Node* CGlbGlobeDigHole::DrawDigHole2(CGlbPolygon* polygon3d,glbDouble depth)
{
	CGlbLine* line3d = NULL;
	//获取上下底面地理坐标
	osg::ref_ptr<osg::Vec3dArray> AboveFaceVertexs = new osg::Vec3dArray;
	osg::ref_ptr<osg::Vec3dArray> BelowFaceVertexs = new osg::Vec3dArray;
	osg::ref_ptr<osg::Vec3dArray> SideFaceVertexs = new osg::Vec3dArray;

	line3d = const_cast<CGlbLine*>(polygon3d->GetExtRing());
	for (glbInt32 i = 0;i < line3d->GetCount();++i)
	{
		osg::Vec3d point;
		line3d->GetPoint(i,&point.x(),&point.y(),&point.z());
		if (mpt_globe->GetType() == GLB_GLOBETYPE_GLOBE)
		{
			osg::Vec3d point2 = osg::Vec3d(point.x(),point.y(),depth);
			g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(point.y()),osg::DegreesToRadians(point.x()),point.z(),point.x(),point.y(),point.z());
			g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(point2.y()),osg::DegreesToRadians(point2.x()),point2.z(),point2.x(),point2.y(),point2.z());
			AboveFaceVertexs->push_back(point);
			BelowFaceVertexs->push_back(point2);
		}else if (mpt_globe->GetType() == GLB_GLOBETYPE_FLAT)
		{
			osg::Vec3d point2 = osg::Vec3d(point.x(),point.y(),point.z()-depth);
			AboveFaceVertexs->push_back(point);
			BelowFaceVertexs->push_back(point2);
		}
	}

	//顶点偏移，以对象中心为原点地理坐标系，世界坐标转换成区域坐标
	osg::Vec3d center;
	osg::Matrixd worldTocenter;
	polygon3d->GetExtent()->GetCenter(&center.x(),&center.y(),&center.z());
	if (mpt_globe->GetType() == GLB_GLOBETYPE_GLOBE)
	{	
		osg::Matrixd centerToworld;
		g_ellipsoidModel->computeLocalToWorldTransformFromLatLongHeight(osg::DegreesToRadians(center.y()),osg::DegreesToRadians(center.x()),center.z(),centerToworld);
		worldTocenter = osg::Matrixd::inverse(centerToworld);
	}else if (mpt_globe->GetType() == GLB_GLOBETYPE_FLAT)
	{
		worldTocenter.makeIdentity();
		worldTocenter.makeTranslate(-center.x(),-center.y(),-center.z());
	}
	for (glbInt32 i = 0;i < line3d->GetCount();++i)
	{
		(*AboveFaceVertexs)[i] = (*AboveFaceVertexs)[i] * worldTocenter;
		(*BelowFaceVertexs)[i] = (*BelowFaceVertexs)[i] * worldTocenter;
		SideFaceVertexs->push_back((*AboveFaceVertexs)[i]);
		SideFaceVertexs->push_back((*BelowFaceVertexs)[i]);
	}
	SideFaceVertexs->push_back((*AboveFaceVertexs)[0]);
	SideFaceVertexs->push_back((*BelowFaceVertexs)[0]);

	osg::Group* group = new osg::Group;
	osg::ref_ptr<osg::Geode> geodeStencil = new osg::Geode;
	osg::ref_ptr<osg::Geode> geodeBelow = new osg::Geode;
	osg::ref_ptr<osg::Geode> geodeSide = new osg::Geode;
	osg::ref_ptr<osg::Geometry> geomStencil = new osg::Geometry;	//蒙版
	osg::ref_ptr<osg::Geometry> geomBelow = new osg::Geometry;	//下底面
	osg::ref_ptr<osg::Geometry> geomSide = new osg::Geometry;	//侧面

	//蒙版几何体,此几何体全透明
	geomStencil->setVertexArray(AboveFaceVertexs.get());
	geomStencil->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::POLYGON,0,AboveFaceVertexs->size()));

	//下底面
	geomBelow->setVertexArray(BelowFaceVertexs.get());
	geomBelow->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::POLYGON,0,BelowFaceVertexs->size()));
	//侧面
	geomSide->setVertexArray(SideFaceVertexs.get());
	geomSide->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUAD_STRIP,0,SideFaceVertexs->size()));

	osg::ref_ptr<osgUtil::Tessellator> tscx = new osgUtil::Tessellator(); 
	tscx->setTessellationType(osgUtil::Tessellator::TESS_TYPE_GEOMETRY);
	tscx->setBoundaryOnly(false);
	tscx->setWindingType( osgUtil::Tessellator::TESS_WINDING_ODD);
	tscx->setTessellationNormal(osg::Vec3(0.0,0.0,1.0));
	tscx->retessellatePolygons(*geomStencil.get());
	geodeStencil->addDrawable(geomStencil.get());	

	geodeBelow->addDrawable(geomBelow.get());
	geodeSide->addDrawable(geomSide.get());
	////阴影方法
	////定义新的阴影节点类，并设置接收和投射者标志
	//osg::ref_ptr<osgShadow::ShadowedScene> shadowedscene = new osgShadow::ShadowedScene;
	//shadowedscene->setCastsShadowTraversalMask(CastsShadowTraversalMask);
	//shadowedscene->setReceivesShadowTraversalMask(ReceivesShadowTraversalMask);
	////定义一个阴影技法类，并指定给阴影节点
	//osg::ref_ptr<osgShadow::ShadowVolume> shadowvolume = new osgShadow::ShadowVolume;
	//shadowedscene->setShadowTechnique(shadowvolume.get());
	////定义一个新的光源，并设置其参数
	//osg::ref_ptr<osg::LightSource> lightsource = new osg::LightSource;	 
	//osg::Vec3 lightpos(0.0,0.0,1.0);
	// 
	//lightsource->getLight()->setDirection(lightpos);
	//lightsource->getLight()->setAmbient(osg::Vec4(1.0,1.0,1.0,1.0));
	//lightsource->getLight()->setDiffuse(osg::Vec4(0.0,1.0,0.0,1.0));
	////lightsource->setLocalStateSetModes(osg::StateAttribute::ON); 
	////将光源和模型作为阴影节点的子节点加入，并设置视口的场景树
	//geodeStencil->setNodeMask(CastsShadowTraversalMask);
	//shadowedscene->addChild(geodeStencil.get());
	//shadowedscene->addChild(lightsource.get());

	//////////////////////////////////////////////////////////////////////////

	osg::ref_ptr<osg::LightSource> lightsource = new osg::LightSource;	 
	osg::Vec3 lightpos(0.0,0.0,1.0);

	lightsource->getLight()->setDirection(lightpos);
	lightsource->getLight()->setAmbient(osg::Vec4(1.0,1.0,1.0,1.0));
	lightsource->getLight()->setDiffuse(osg::Vec4(0.0,1.0,0.0,1.0));
	lightsource->setLocalStateSetModes(osg::StateAttribute::ON); 
	
	//Z-pass算法
	//Pass1
	osg::ref_ptr<osg::Depth> depth1 = new osg::Depth(osg::Depth::LEQUAL);  
	depth1->setWriteMask(false);  
	osg::ref_ptr<osg::Stencil> passStencil1 = new osg::Stencil();  
	passStencil1->setFunction(osg::Stencil::ALWAYS, 1, 0xff);  
	passStencil1->setOperation(osg::Stencil::KEEP, osg::Stencil::KEEP, osg::Stencil::INCR);  
	osg::ref_ptr<osg::ColorMask> colorMask1 = new osg::ColorMask;  
	colorMask1->setMask(false, false, false, false);  
	osg::ref_ptr<osg::FrontFace> frontFace1 = new osg::FrontFace(osg::FrontFace::COUNTER_CLOCKWISE);  
	osg::ref_ptr<osg::CullFace> cullFace1 = new osg::CullFace(osg::CullFace::BACK);  

	geodeStencil->getOrCreateStateSet()->setAttributeAndModes(depth1.get(), osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);  
	geodeStencil->getOrCreateStateSet()->setAttributeAndModes(passStencil1.get(), osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);  
	geodeStencil->getOrCreateStateSet()->setAttributeAndModes(colorMask1.get(), osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);  
	geodeStencil->getOrCreateStateSet()->setAttributeAndModes(frontFace1.get(), osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);  
	geodeStencil->getOrCreateStateSet()->setAttributeAndModes(cullFace1.get(), osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);  
	geodeStencil->getOrCreateStateSet()->setMode(GL_LIGHTING, false);  

	//Pass2  
	osg::ref_ptr<osg::Geode> geodeStencil2 = new osg::Geode;
	geodeStencil2->addDrawable(geomStencil.get());

	osg::ref_ptr<osg::Depth> depth2 = new osg::Depth(osg::Depth::LEQUAL);  
	depth2->setWriteMask(false);  
	osg::ref_ptr<osg::Stencil> passStencil2 = new osg::Stencil;  
	passStencil2->setFunction(osg::Stencil::ALWAYS, 1, 0xff);  
	passStencil2->setOperation(osg::Stencil::KEEP, osg::Stencil::KEEP, osg::Stencil::DECR);  
	osg::ref_ptr<osg::ColorMask> colorMask2 = new osg::ColorMask;  
	colorMask2->setMask(false, false, false, false);  
	osg::ref_ptr<osg::FrontFace> frontFace2 = new osg::FrontFace(osg::FrontFace::CLOCKWISE);  
	osg::ref_ptr<osg::CullFace> cullFace2 = new osg::CullFace(osg::CullFace::BACK);  

	geodeStencil2->getOrCreateStateSet()->setAttributeAndModes(depth2.get(), osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);  
	geodeStencil2->getOrCreateStateSet()->setAttributeAndModes(passStencil2.get(), osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);  
	geodeStencil2->getOrCreateStateSet()->setAttributeAndModes(colorMask2.get(), osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);  
	geodeStencil2->getOrCreateStateSet()->setAttributeAndModes(frontFace2.get(), osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);  
	geodeStencil2->getOrCreateStateSet()->setAttributeAndModes(cullFace2.get(), osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);  
	geodeStencil2->getOrCreateStateSet()->setMode(GL_LIGHTING, false);  

	osg::MatrixTransform *mt = new osg::MatrixTransform;  
	mt->setMatrix(osg::Matrix::identity());  
	mt->setReferenceFrame(osg::Transform::ABSOLUTE_RF);  
	osg::Geode *geode = new osg::Geode;  
	osg::Geometry *geometry = new osg::Geometry;  
	osg::Vec3Array *vertexArray = new osg::Vec3Array;  
	osg::Vec4Array *colorArray = new osg::Vec4Array;  
	colorArray->push_back(osg::Vec4(0.0f, 0.0f, 0.0f, 0.4f));  
	vertexArray->push_back(osg::Vec3(-0.1f, 0.1f,-0.10f));  
	vertexArray->push_back(osg::Vec3(-0.1f,-0.1f,-0.10f));  
	vertexArray->push_back(osg::Vec3(0.1f, 0.1f,-0.10f));  
	vertexArray->push_back(osg::Vec3(0.1f,-0.1f,-0.10f));  

	geometry->setVertexArray(vertexArray);  
	geometry->setColorArray(colorArray, osg::Array::BIND_OVERALL);  
	geometry->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::TRIANGLE_STRIP, 0, vertexArray->size()));  

	osg::BlendFunc *blendFunc = new osg::BlendFunc(osg::BlendFunc::SRC_ALPHA, osg::BlendFunc::ONE_MINUS_SRC_ALPHA);  
	osg::Stencil *stencil = new osg::Stencil;  
	stencil->setFunction(osg::Stencil::NOTEQUAL, 0, 0xff);  
	stencil->setOperation(osg::Stencil::KEEP, osg::Stencil::KEEP, osg::Stencil::KEEP);  
	osg::Depth *depth3 = new osg::Depth(osg::Depth::LEQUAL);  
	depth3->setWriteMask(false);  

	geometry->getOrCreateStateSet()->setAttributeAndModes(depth3, osg::StateAttribute::ON | osg::StateAttribute::PROTECTED);  
	geometry->getOrCreateStateSet()->setAttributeAndModes(blendFunc, osg::StateAttribute::ON | osg::StateAttribute::PROTECTED);  
	geometry->getOrCreateStateSet()->setAttributeAndModes(stencil, osg::StateAttribute::ON | osg::StateAttribute::PROTECTED);  
	geometry->getOrCreateStateSet()->setMode(GL_LIGHTING, false);  
	geometry->getOrCreateStateSet()->setMode(GL_DEPTH_TEST, false);  
	geode->addDrawable(geometry);
	mt->addChild(geode);

	osg::ref_ptr<osg::Group> gp = new osg::Group;
	gp->addChild(lightsource.get());
	gp->addChild(geodeStencil.get());
	gp->addChild(geodeStencil2.get());
	gp->addChild(mt);
	//////////////////////////////////////////////////////////////////////////

	group->addChild(gp.get());
	osg::ref_ptr<osg::Switch> wallnode = new osg::Switch;
	wallnode->addChild(geodeBelow.get());
	wallnode->addChild(geodeSide.get());
	wallnode->setValue(0,mpr_isShowWall);
	wallnode->setValue(1,mpr_isShowWall);
	group->addChild(wallnode.get());	//1

 	//地形 stencil 设置
 	osg::ref_ptr<osg::Stencil> terrianStencil = new osg::Stencil;
 	terrianStencil->setFunction(osg::Stencil::EQUAL,0,0xff);
 	terrianStencil->setOperation(osg::Stencil::ZERO,osg::Stencil::KEEP,osg::Stencil::KEEP);
 	osg::Node* pTerrianNode = mpt_globe->GetTerrain()->GetNode();
 	osg::ref_ptr<osg::StateSet> statesetBin2 = pTerrianNode->getOrCreateStateSet();
 	glbInt32 terrianRenderBin = pTerrianNode->getOrCreateStateSet()->getRenderingHint();
 	statesetBin2->setAttributeAndModes(terrianStencil,osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);	
	pTerrianNode->setNodeMask(ReceivesShadowTraversalMask);

 	//面片 stencil 设置
 	osg::ref_ptr<osg::Stencil> groupStencil = new osg::Stencil;
 	groupStencil->setFunction(osg::Stencil::ALWAYS,1,0xff);
 	groupStencil->setOperation(osg::Stencil::KEEP,osg::Stencil::REPLACE,osg::Stencil::REPLACE);
 	osg::ref_ptr<osg::StateSet> statesetBin1 = group->getChild(0)->getOrCreateStateSet();
 	statesetBin1->setRenderBinDetails(terrianRenderBin - 1,"RenderBin");
 	statesetBin1->setAttributeAndModes(groupStencil,osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
 	osg::ref_ptr<osg::ColorMask> colorMask = new osg::ColorMask();
 	colorMask->setMask(false,false,false,false);
 	statesetBin1->setAttribute(colorMask.get());

	//蒙版材质透明度
	osg::ref_ptr<osg::Material> aboveMaterial = new osg::Material;
	aboveMaterial->setAmbient(osg::Material::FRONT_AND_BACK,osg::Vec4(1.0f,0.0f,1.0f,1.0f));
	aboveMaterial->setDiffuse(osg::Material::FRONT_AND_BACK,osg::Vec4(0.5f,0.5f,0.5f,0.8f));
	aboveMaterial->setAlpha(osg::Material::FRONT_AND_BACK,1.0f);
	geodeStencil->getOrCreateStateSet()->setAttribute(aboveMaterial.get(),osg::StateAttribute::ON | osg::StateAttribute::PROTECTED);
	//下底面材质，纹理
	osg::ref_ptr<osg::Material> belowMaterial = new osg::Material;
	belowMaterial->setAmbient(osg::Material::FRONT_AND_BACK,osg::Vec4(1.0f,0.0f,0.0f,1.0f));
	belowMaterial->setDiffuse(osg::Material::FRONT_AND_BACK,osg::Vec4(0.5f,0.5f,0.5f,0.8f));
	geodeBelow->getOrCreateStateSet()->setAttribute(belowMaterial.get(),osg::StateAttribute::ON);
	//侧面材质，纹理
	osg::ref_ptr<osg::Material> sideMaterial = new osg::Material;
	sideMaterial->setAmbient(osg::Material::FRONT_AND_BACK,osg::Vec4(0.0f,1.0f,0.0f,1.0f));
	sideMaterial->setDiffuse(osg::Material::FRONT_AND_BACK,osg::Vec4(0.5f,0.5f,0.5f,0.8f));
	geodeSide->getOrCreateStateSet()->setAttribute(sideMaterial.get(),osg::StateAttribute::ON);

	return group;
}

//盒子蒙版,弃用
osg::Node* CGlbGlobeDigHole::DrawDigHole3(CGlbPolygon* polygon3d,glbDouble depth)
{
	CGlbLine* line3d = NULL;
	//获取上下底面地理坐标
	osg::ref_ptr<osg::Vec3dArray> AboveFaceVertexs = new osg::Vec3dArray;
	osg::ref_ptr<osg::Vec3dArray> BelowFaceVertexs = new osg::Vec3dArray;
	osg::ref_ptr<osg::Vec3dArray> SideFaceVertexs = new osg::Vec3dArray;

	line3d = const_cast<CGlbLine*>(polygon3d->GetExtRing());
	for (glbInt32 i = 0;i < line3d->GetCount();++i)
	{
		osg::Vec3d point;
		line3d->GetPoint(i,&point.x(),&point.y(),&point.z());
		if (mpt_globe->GetType() == GLB_GLOBETYPE_GLOBE)
		{
			osg::Vec3d point2 = osg::Vec3d(point.x(),point.y(),point.z()-depth);
			osg::Vec3d pointup = osg::Vec3d(point.x(),point.y(),mpr_holeUpHei+point.z());
			osg::Vec3d pointdown = osg::Vec3d(point.x(),point.y(),mpr_holeDownHei+point.z());
			g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(point.y()),osg::DegreesToRadians(point.x()),point.z(),point.x(),point.y(),point.z());
			g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(point2.y()),osg::DegreesToRadians(point2.x()),point2.z(),point2.x(),point2.y(),point2.z());
			g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(pointup.y()),osg::DegreesToRadians(pointup.x()),pointup.z(),pointup.x(),pointup.y(),pointup.z());
			g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(pointdown.y()),osg::DegreesToRadians(pointdown.x()),pointdown.z(),pointdown.x(),pointdown.y(),pointdown.z());
			AboveFaceVertexs->push_back(pointup);
			AboveFaceVertexs->push_back(pointdown);
			BelowFaceVertexs->push_back(point2);
			SideFaceVertexs->push_back(point);
			SideFaceVertexs->push_back(point2);
		}else if (mpt_globe->GetType() == GLB_GLOBETYPE_FLAT)
		{
			osg::Vec3d point2 = osg::Vec3d(point.x(),point.y(),point.z()-depth);
			osg::Vec3d pointup = osg::Vec3d(point.x(),point.y(),mpr_holeUpHei+point.z());
			osg::Vec3d pointdown = osg::Vec3d(point.x(),point.y(),mpr_holeDownHei+point.z());
			AboveFaceVertexs->push_back(pointup);
			AboveFaceVertexs->push_back(pointdown);
			BelowFaceVertexs->push_back(point2);
			SideFaceVertexs->push_back(point);
			SideFaceVertexs->push_back(point2);
		}
	}

	//顶点偏移，以对象中心为原点地理坐标系，世界坐标转换成区域坐标
	osg::Vec3d center;
	osg::Matrixd worldTocenter;
	polygon3d->GetExtent()->GetCenter(&center.x(),&center.y(),&center.z());
	if (mpt_globe->GetType() == GLB_GLOBETYPE_GLOBE)
	{	
		osg::Matrixd centerToworld;
		g_ellipsoidModel->computeLocalToWorldTransformFromLatLongHeight(osg::DegreesToRadians(center.y()),osg::DegreesToRadians(center.x()),center.z(),centerToworld);
		worldTocenter = osg::Matrixd::inverse(centerToworld);
	}else if (mpt_globe->GetType() == GLB_GLOBETYPE_FLAT)
	{
		worldTocenter.makeIdentity();
		worldTocenter.makeTranslate(-center.x(),-center.y(),-center.z());
	}
	osg::ref_ptr<osg::Vec3dArray> StencilAboveFaceVertexs = new osg::Vec3dArray;
	osg::ref_ptr<osg::Vec3dArray> StencilBelowFaceVertexs = new osg::Vec3dArray;
	for (glbInt32 i = 0;i < line3d->GetCount();++i)
	{
		(*AboveFaceVertexs)[2*i] = (*AboveFaceVertexs)[2*i] * worldTocenter;
		(*AboveFaceVertexs)[2*i+1] = (*AboveFaceVertexs)[2*i+1] * worldTocenter;
		(*BelowFaceVertexs)[i] = (*BelowFaceVertexs)[i] * worldTocenter;
		(*SideFaceVertexs)[2*i] = (*SideFaceVertexs)[2*i] * worldTocenter;
		(*SideFaceVertexs)[2*i+1] = (*SideFaceVertexs)[2*i+1] * worldTocenter;
		StencilAboveFaceVertexs->push_back((*AboveFaceVertexs)[2*i]);
		StencilBelowFaceVertexs->push_back((*AboveFaceVertexs)[2*i+1]);
	}
	AboveFaceVertexs->push_back((*AboveFaceVertexs)[0]);
	AboveFaceVertexs->push_back((*AboveFaceVertexs)[1]);
	SideFaceVertexs->push_back((*SideFaceVertexs)[0]);
	SideFaceVertexs->push_back((*SideFaceVertexs)[1]);

	osg::Group* group = new osg::Group;
	osg::ref_ptr<osg::Geode> geodeAboveFront = new osg::Geode;
	osg::ref_ptr<osg::Geode> geodeAboveBack = new osg::Geode;
	osg::ref_ptr<osg::Geode> geodeBelow = new osg::Geode;
	osg::ref_ptr<osg::Geode> geodeSide = new osg::Geode;
	osg::ref_ptr<osg::Geometry> geomStencilAbove = new osg::Geometry;	//蒙版地上面
	osg::ref_ptr<osg::Geometry> geomStencilBelow = new osg::Geometry;	//蒙版地下面
	osg::ref_ptr<osg::Geometry> geomStencilSide = new osg::Geometry;	//蒙版侧面
	osg::ref_ptr<osg::Geometry> geomBelow = new osg::Geometry;	//下底面
	osg::ref_ptr<osg::Geometry> geomSide = new osg::Geometry;	//侧面

	//蒙版几何体,此几何体全透明
	geomStencilAbove->setVertexArray(StencilAboveFaceVertexs.get());
	geomStencilAbove->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::POLYGON,0,StencilAboveFaceVertexs->size()));
	geomStencilBelow->setVertexArray(StencilBelowFaceVertexs.get());
	geomStencilBelow->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::POLYGON,0,StencilBelowFaceVertexs->size()));
	geomStencilSide->setVertexArray(AboveFaceVertexs.get());
	geomStencilSide->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUAD_STRIP,0,AboveFaceVertexs->size()));

	//下底面
	geomBelow->setVertexArray(BelowFaceVertexs.get());
	geomBelow->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::POLYGON,0,BelowFaceVertexs->size()));
	//侧面
	geomSide->setVertexArray(SideFaceVertexs.get());
	geomSide->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUAD_STRIP,0,SideFaceVertexs->size()));

	osg::ref_ptr<osgUtil::Tessellator> tscx = new osgUtil::Tessellator(); 
	tscx->setTessellationType(osgUtil::Tessellator::TESS_TYPE_GEOMETRY);
	tscx->setBoundaryOnly(false);
	tscx->setWindingType( osgUtil::Tessellator::TESS_WINDING_ODD);
	tscx->setTessellationNormal(osg::Vec3(0.0,0.0,1.0));
	tscx->retessellatePolygons(*geomStencilAbove.get());
	geodeAboveFront->addDrawable(geomStencilAbove.get());
	geodeAboveBack->addDrawable(geomStencilAbove.get());

	osg::ref_ptr<osgUtil::Tessellator> tscx2 = new osgUtil::Tessellator(); 
	tscx2->setTessellationType(osgUtil::Tessellator::TESS_TYPE_GEOMETRY);
	tscx2->setBoundaryOnly(false);
	tscx2->setWindingType( osgUtil::Tessellator::TESS_WINDING_ODD);
	tscx2->setTessellationNormal(osg::Vec3(0.0,0.0,-1.0));
	tscx2->retessellatePolygons(*geomStencilBelow.get());
	geodeAboveFront->addDrawable(geomStencilBelow.get());
	geodeAboveBack->addDrawable(geomStencilBelow.get());
	//侧面
	geodeAboveFront->addDrawable(geomStencilSide.get());
	geodeAboveBack->addDrawable(geomStencilSide.get());

	geodeBelow->addDrawable(geomBelow.get());
	geodeSide->addDrawable(geomSide.get());
	
	geodeAboveFront->getOrCreateStateSet()->setAttributeAndModes(new osg::CullFace(osg::CullFace::BACK));
	geodeAboveBack->getOrCreateStateSet()->setAttributeAndModes(new osg::CullFace(osg::CullFace::FRONT));
	group->addChild(geodeAboveFront.get());
	group->addChild(geodeAboveBack.get());
	osg::ref_ptr<osg::Switch> wallnode = new osg::Switch;
	wallnode->addChild(geodeBelow.get());
	wallnode->addChild(geodeSide.get());
	wallnode->setValue(0,mpr_isShowWall);
	wallnode->setValue(1,mpr_isShowWall);
	group->addChild(wallnode.get());	//1	


	//地形 stencil 设置
	osg::ref_ptr<osg::Stencil> terrianStencil = new osg::Stencil;
	terrianStencil->setFunction(osg::Stencil::EQUAL,0,0xff);
	terrianStencil->setOperation(osg::Stencil::ZERO,osg::Stencil::KEEP,osg::Stencil::KEEP);
	osg::Node* pTerrianNode = mpt_globe->GetTerrain()->GetNode();
	osg::ref_ptr<osg::StateSet> statesetBin1 = pTerrianNode->getOrCreateStateSet();
	glbInt32 terrianRenderBin = pTerrianNode->getOrCreateStateSet()->getRenderingHint();
	statesetBin1->setAttributeAndModes(terrianStencil,osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);	

	//////////////////////////////////////////////////////////////////////////
	//盒子正面 stencil 设置,
	osg::ref_ptr<osg::Stencil> groupStencil = new osg::Stencil;
	groupStencil->setFunction(osg::Stencil::ALWAYS,1,0xff);
	groupStencil->setOperation(osg::Stencil::DECR,osg::Stencil::INCR,osg::Stencil::DECR);
	osg::ref_ptr<osg::StateSet> statesetBin = geodeAboveFront->getOrCreateStateSet();
	statesetBin->setRenderBinDetails(terrianRenderBin - 1,"RenderBin");
	statesetBin->setAttributeAndModes(groupStencil,osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
	osg::ref_ptr<osg::ColorMask> colorMask = new osg::ColorMask();
	colorMask->setMask(false,false,false,false);
	statesetBin->setAttribute(colorMask.get());
	//盒子背面 stencil 设置,
	osg::ref_ptr<osg::Stencil> groupStencil2 = new osg::Stencil;
	groupStencil2->setFunction(osg::Stencil::ALWAYS,1,0xff);
	groupStencil2->setOperation(osg::Stencil::DECR,osg::Stencil::DECR,osg::Stencil::INCR);
	osg::ref_ptr<osg::StateSet> statesetBin2 = geodeAboveBack->getOrCreateStateSet();
	statesetBin2->setRenderBinDetails(terrianRenderBin - 1,"RenderBin");
	statesetBin2->setAttributeAndModes(groupStencil2,osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
	osg::ref_ptr<osg::ColorMask> colorMask2 = new osg::ColorMask();
	colorMask2->setMask(false,false,false,false);
	statesetBin2->setAttribute(colorMask2.get());
	//////////////////////////////////////////////////////////////////////////

	//蒙版材质透明度
	osg::ref_ptr<osg::Material> aboveMaterial = new osg::Material;
	aboveMaterial->setAmbient(osg::Material::FRONT_AND_BACK,osg::Vec4(0.0f,0.0f,1.0f,1.0f));
	aboveMaterial->setDiffuse(osg::Material::FRONT_AND_BACK,osg::Vec4(0.5f,0.5f,0.5f,0.8f));
	aboveMaterial->setAlpha(osg::Material::FRONT_AND_BACK,0.0f);
	geodeAboveFront->getOrCreateStateSet()->setAttribute(aboveMaterial.get(),osg::StateAttribute::ON | osg::StateAttribute::PROTECTED);
	geodeAboveBack->getOrCreateStateSet()->setAttribute(aboveMaterial.get(),osg::StateAttribute::ON | osg::StateAttribute::PROTECTED);


	//下底面材质，纹理
	osg::ref_ptr<osg::Material> belowMaterial = new osg::Material;
	belowMaterial->setAmbient(osg::Material::FRONT_AND_BACK,osg::Vec4(1.0f,0.0f,0.0f,1.0f));
	belowMaterial->setDiffuse(osg::Material::FRONT_AND_BACK,osg::Vec4(0.5f,0.5f,0.5f,0.8f));
	geodeBelow->getOrCreateStateSet()->setAttribute(belowMaterial.get(),osg::StateAttribute::ON);
	//侧面材质，纹理
	osg::ref_ptr<osg::Material> sideMaterial = new osg::Material;
	sideMaterial->setAmbient(osg::Material::FRONT_AND_BACK,osg::Vec4(0.0f,1.0f,0.0f,1.0f));
	sideMaterial->setDiffuse(osg::Material::FRONT_AND_BACK,osg::Vec4(0.5f,0.5f,0.5f,0.8f));
	geodeSide->getOrCreateStateSet()->setAttribute(sideMaterial.get(),osg::StateAttribute::ON);

	return group;
}

IGlbGeometry* CGlbGlobeDigHole::AnalysisPolyGeomertry(CGlbFeature* feature, CGlbPolygon* polyGeo)
{
	CGlbMultiPolygon* multiPolygon = new CGlbMultiPolygon();
	if (feature != NULL)
	{
		glbUInt32 level = 0;
		IGlbGeometry *geo = NULL;
		bool result = false;
		result = mpt_feature->GetGeometry(&geo,level);
		if(!result || geo == NULL) 
			return NULL;
		
		GlbGeometryTypeEnum geoType = geo->GetType();
		switch(geoType)
		{
		case GLB_GEO_POLYGON:
			{
				CGlbPolygon* poly3d =  static_cast<CGlbPolygon*>(geo);
				if (!poly3d)
					return NULL;
				CGlbLine* line3d = const_cast<CGlbLine*>(poly3d->GetExtRing());	//只有外环，无内环
				CGlbLine* ln = new CGlbLine;
				glbDouble ptx,pty,ptz;
				for (glbInt32 j = 0; j < line3d->GetCount(); j++)
				{					
					line3d->GetPoint(j,&ptx,&pty,&ptz);
					ln->AddPoint(ptx,pty);
				}
				CGlbPolygon* polygon = new CGlbPolygon;
				polygon->SetExtRing(ln);
				multiPolygon->AddPolygon(polygon);
			}					
			break;
		case GLB_GEO_MULTIPOLYGON:
			{
				CGlbMultiPolygon* multiPoly = dynamic_cast<CGlbMultiPolygon*>(geo);
				glbInt32 polyCnt = multiPoly->GetCount();
				for (int k = 0; k < polyCnt; k++)
				{
					CGlbPolygon *poly3d = const_cast<CGlbPolygon *>(multiPoly->GetPolygon(k));
					if (!poly3d)
						return NULL;
					CGlbLine* line3d = const_cast<CGlbLine*>(poly3d->GetExtRing());	//只有外环，无内环
					CGlbLine* ln = new CGlbLine;
					glbDouble ptx,pty,ptz;
					for (glbInt32 j = 0; j < line3d->GetCount(); j++)
					{					
						line3d->GetPoint(j,&ptx,&pty,&ptz);
						ln->AddPoint(ptx,pty);
					}
					CGlbPolygon* polygon = new CGlbPolygon;
					polygon->SetExtRing(ln);
					multiPolygon->AddPolygon(polygon);
				}
			}
			break;
		}
	}else{
		if (!polyGeo)
			return NULL;
		CGlbLine* line3d = const_cast<CGlbLine*>(polyGeo->GetExtRing());	//只有外环，无内环
		CGlbLine* ln = new CGlbLine;
		glbDouble ptx,pty,ptz;
		for (glbInt32 i = 0; i < line3d->GetCount(); i++)
		{					
			line3d->GetPoint(i,&ptx,&pty,&ptz);
			ln->AddPoint(ptx,pty);
		}
		CGlbPolygon* polygon = new CGlbPolygon;
		polygon->SetExtRing(ln);
		multiPolygon->AddPolygon(polygon);
	}
	return multiPolygon;
}

void CGlbGlobeDigHole::DealSelected()
{

}

void CGlbGlobeDigHole::DealBlink()
{

}

void CGlbGlobeDigHole::DirtyOnTerrainObject()
{
	if (!mpr_outline || !mpt_globe)return;
	CGlbExtent* cglbExtent = const_cast<CGlbExtent*>(mpr_outline->GetExtent());
	mpt_globe->AddDomDirtyExtent(*cglbExtent);	
}

void CGlbGlobeDigHole::testDigHoleObj()
{
	CGlbPolygon* polygon3d = new CGlbPolygon;
	CGlbLine* line3d = new CGlbLine;	
	line3d->AddPoint(116.301,39.900,115.0);
	line3d->AddPoint(116.300,39.901,115.0);
	line3d->AddPoint(116.299,39.900,115.0);
	line3d->AddPoint(116.300,39.9005,115.0);
	line3d->AddPoint(116.301,39.900,115.0);
	polygon3d->SetExtRing(line3d);
	glbBool isNeedDirectDraw = true;

	glbDouble depth = 100.0;
	SetDepth(depth,isNeedDirectDraw);
	ShowWall(true);
	SetMaxVisibleDistance(100000000000.0);
	SetAltitudeMode(GLB_ALTITUDEMODE_ONTERRAIN,isNeedDirectDraw);
	GlbDigHoleSymbolInfo* info = new GlbDigHoleSymbolInfo;
	info->lineInfo = new GlbLinePixelSymbolInfo;
	info->lineInfo->lineColor = new GlbRenderColor;
	info->lineInfo->lineColor->value = 0xffff00ff;
	info->lineInfo->lineOpacity = new GlbRenderInt32;
	info->lineInfo->lineOpacity->value = 100;
	info->lineInfo->lineWidth = new GlbRenderDouble;
	info->lineInfo->lineWidth->value = 2;
	info->lineInfo->linePattern = new GlbRenderLinePattern;
	info->lineInfo->linePattern->value = GLB_LINE_DOTTED;
	info->belowTextureData = new GlbRenderString;
	info->belowTextureData->value = L"d:\\b.bmp";
	info->sideTextureData = new GlbRenderString;
	info->sideTextureData->value = L"d:\\b.bmp";
	SetRenderInfo(info,isNeedDirectDraw);	
	SetRegion(polygon3d,isNeedDirectDraw);
	mpt_globe->AddObject(this);
}

glbBool CGlbGlobeDigHole::Load2( xmlNodePtr *node,const glbWChar* prjPath )
{
	xmlNodePtr pnode = *node;
	CGlbGlobeRObject::Load2(node,prjPath);
	xmlChar *szKey = NULL;
	xmlNodePtr groupNode = pnode->parent;
	xmlNodePtr childNode;
	CGlbString bUse;

	if (groupNode && (!xmlStrcmp(groupNode->name, (const xmlChar *)"GetRenderInfo")))
	{
		GlbGlobeObjectTypeEnum rdtype;
		xmlNodePtr rdchild = groupNode->xmlChildrenNode;

		GlbDigHoleSymbolInfo* digHoleInfo = new GlbDigHoleSymbolInfo();
		digHoleInfo->Load2(rdchild,prjPath);
		mpr_renderInfo = (GlbRenderInfo*)digHoleInfo;
	}

	groupNode = groupNode->next;//text
	groupNode = groupNode->next;//GetRenderOrder
	if (groupNode->xmlChildrenNode && (!xmlStrcmp(groupNode->xmlChildrenNode->name, (const xmlChar *)"text")))
	{
		szKey = xmlNodeGetContent(groupNode->xmlChildrenNode);	
		sscanf_s((char*)szKey,"%d",&mpt_renderOrder);
		xmlFree(szKey);
	}

	groupNode = groupNode->next;//text
	groupNode = groupNode->next;//GetRObject
	groupNode = groupNode->next;//text
	groupNode = groupNode->next;//GetTooltip


	groupNode = groupNode->next;//text
	groupNode = groupNode->next;//IsBlink
	if (groupNode->xmlChildrenNode && (!xmlStrcmp(groupNode->xmlChildrenNode->name, (const xmlChar *)"text")))
	{
		szKey = xmlNodeGetContent(groupNode->xmlChildrenNode);	
		char* str = u2g_obj((char*)szKey);
		bUse = (char*)str;
		if(bUse == "false")
			mpt_isBlink = false;
		else
			mpt_isBlink = true;
		xmlFree(szKey);
	}


	groupNode = groupNode->next;//text
	groupNode = groupNode->next;//IsEdit
	if (groupNode->xmlChildrenNode && (!xmlStrcmp(groupNode->xmlChildrenNode->name, (const xmlChar *)"text")))
	{
		szKey = xmlNodeGetContent(groupNode->xmlChildrenNode);	
		char* str = u2g_obj((char*)szKey);
		bUse = (char*)str;
		if(bUse == "false")
			mpt_isEdit = false;
		else
			mpt_isEdit = true;
		xmlFree(szKey);
	}


	groupNode = groupNode->next;//text
	groupNode = groupNode->next;//IsGround
	if (groupNode->xmlChildrenNode && (!xmlStrcmp(groupNode->xmlChildrenNode->name, (const xmlChar *)"text")))
	{
		szKey = xmlNodeGetContent(groupNode->xmlChildrenNode);	
		char* str = u2g_obj((char*)szKey);
		bUse = (char*)str;
		if(bUse == "false")
			mpt_isGround = false;
		else
			mpt_isGround = true;
		xmlFree(szKey);
	}


	groupNode = groupNode->next;//text
	groupNode = groupNode->next;//IsSelected
	if (groupNode->xmlChildrenNode && (!xmlStrcmp(groupNode->xmlChildrenNode->name, (const xmlChar *)"text")))
	{
		szKey = xmlNodeGetContent(groupNode->xmlChildrenNode);	
		char* str = u2g_obj((char*)szKey);
		bUse = (char*)str;
		if(bUse == "false")
			mpt_isSelected = false;
		else
			mpt_isSelected = true;
		xmlFree(szKey);
	}


	groupNode = groupNode->next;//text
	groupNode = groupNode->next;//IsShow
	if (groupNode->xmlChildrenNode && (!xmlStrcmp(groupNode->xmlChildrenNode->name, (const xmlChar *)"text")))
	{
		szKey = xmlNodeGetContent(groupNode->xmlChildrenNode);	
		char* str = u2g_obj((char*)szKey);
		bUse = (char*)str;
		if(bUse == "false")
			mpt_isShow = false;
		else
			mpt_isShow = true;
		xmlFree(szKey);
	}

	groupNode = groupNode->next;//text
	groupNode = groupNode->next;//IsUseInstance
	if (groupNode->xmlChildrenNode && (!xmlStrcmp(groupNode->xmlChildrenNode->name, (const xmlChar *)"text")))
	{
		szKey = xmlNodeGetContent(groupNode->xmlChildrenNode);	
		char* str = u2g_obj((char*)szKey);
		bUse = (char*)str;
		if(bUse == "false")
			mpt_isUseInstance = false;
		else
			mpt_isUseInstance = true;
		xmlFree(szKey);
	}

	groupNode = groupNode->next;//text
	groupNode = groupNode->next;//DiggedObjects
	groupNode = groupNode->next;//text
	groupNode = groupNode->next;//GetDepth
	if (groupNode && (!xmlStrcmp(groupNode->name, (const xmlChar *)"GetDepth")))
	{
		szKey = xmlNodeGetContent(groupNode);	
		sscanf_s((char*)szKey,"%d",&mpr_holeDepth);
		xmlFree(szKey);
	}
	groupNode = groupNode->next;//text
	groupNode = groupNode->next;//IsShowWall
	if (groupNode && (!xmlStrcmp(groupNode->name, (const xmlChar *)"IsShowWall")))
	{
		szKey = xmlNodeGetContent(groupNode);	
		char* str = u2g_obj((char*)szKey);
		bUse = (char*)str;
		if(bUse == "false")
			mpr_isShowWall = false;
		else
			mpr_isShowWall = true;
		xmlFree(szKey);
	}

	groupNode = groupNode->next;//text
	groupNode = groupNode->next;//GetRegion
	childNode = groupNode->xmlChildrenNode;//text
	childNode = childNode->next;//coordDimension
	glbInt32 coordDimension = -1;
	if (childNode->xmlChildrenNode && (!xmlStrcmp(childNode->xmlChildrenNode->name, (const xmlChar *)"text")))
	{
		szKey = xmlNodeGetContent(childNode->xmlChildrenNode);	
		sscanf_s((char*)szKey,"%d",&coordDimension);
		xmlFree(szKey);
	}
	childNode = childNode->next;//text
	childNode = childNode->next;//hasM
	glbBool ishasM = false;
	if (childNode)
	{
		szKey = xmlNodeGetContent(childNode);	
		char* str = u2g_obj((char*)szKey);
		bUse = (char*)str;
		if(bUse == "false")
			ishasM = false;
		else
			ishasM = true;
		xmlFree(szKey);
	}
	childNode = childNode->next;//text
	childNode = childNode->next;//ExtRing
	childNode = childNode->xmlChildrenNode;//text
	childNode = childNode->next;//coordDimension
	if (childNode && (!xmlStrcmp(childNode->name, (const xmlChar *)"coordDimension")))
	{
		szKey = xmlNodeGetContent(childNode);	
		sscanf_s((char*)szKey,"%d",&coordDimension);
		xmlFree(szKey);
	}
	childNode = childNode->next;//text
	childNode = childNode->next;//hasM
	ishasM = false;
	if (childNode)
	{
		szKey = xmlNodeGetContent(childNode);	
		char* str = u2g_obj((char*)szKey);
		bUse = (char*)str;
		if(bUse == "false")
			ishasM = false;
		else
			ishasM = true;
		xmlFree(szKey);
	}

	childNode = childNode->next;//text
	childNode = childNode->next;//points
	childNode = childNode->xmlChildrenNode;//text
	childNode = childNode->next;//tagPoint3
	mpr_holeRegion = new CGlbPolygon(3,false);
	CGlbLine *extRing = new CGlbLine(3,false);
	glbDouble tempX,tempY,tempZ;
	while(!xmlStrcmp(childNode->name, (const xmlChar *)"tagPoint3"))
	{
		childNode = childNode->xmlChildrenNode;//text
		childNode = childNode->next;//x
		szKey = xmlNodeGetContent(childNode);	
		sscanf_s((char*)szKey,"%lf",&tempX);

		childNode = childNode->next;//text
		childNode = childNode->next;//y
		szKey = xmlNodeGetContent(childNode);	
		sscanf_s((char*)szKey,"%lf",&tempY);

		childNode = childNode->next;//text
		childNode = childNode->next;//z
		szKey = xmlNodeGetContent(childNode);	
		sscanf_s((char*)szKey,"%lf",&tempZ);
		xmlFree(szKey);
		extRing->AddPoint(tempX,tempY,tempZ);
		childNode = childNode->parent;
		childNode = childNode->next;//text
		childNode = childNode->next;
		if(!childNode)
			break;
	}
	mpr_holeRegion->SetExtRing(extRing);
	return true;
}