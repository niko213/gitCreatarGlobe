#include "StdAfx.h"
#include "GlbGlobeGridObject.h"
#include "GlbGlobeView.h"
#include "GlbGlobeTypes.h"
#include "GlbPolygon.h"
#include "GlbLine.h"
#include "GlbCalculateNodeSizeVisitor.h"
#include "GlbGlobeMemCtrl.h"
#include <osg/Material>
#include <osg/PolygonOffset>

using namespace GlbGlobe;

// CGlbGlobeGridTask接口
CGlbGlobeGridObject::CGlbGlobeGridTask::CGlbGlobeGridTask( CGlbGlobeGridObject *obj,glbInt32 level )
{
	mpr_obj = obj;
	mpr_level = level;
}

CGlbGlobeGridObject::CGlbGlobeGridTask::~CGlbGlobeGridTask( )
{
	mpr_obj = NULL;
}

glbBool CGlbGlobeGridObject::CGlbGlobeGridTask::doRequest()
{
	if(mpr_obj)
	{
		mpr_obj->ReadData(mpr_level);
		return true;
	}
	return false;
}

CGlbGlobeObject * CGlbGlobeGridObject::CGlbGlobeGridTask::getObject()
{
	return mpr_obj.get();
}
//////////////////////////////////////////////////////////////////////////

CGlbGlobeGridObject::CGlbGlobeGridObject(void)
{
	mpr_readData_critical.SetName(L"section_readdata");
	mpr_addToScene_critical.SetName(L"section_addscene");
	mpt_altitudeMode = GLB_ALTITUDEMODE_ABSOLUTE; //剖面都是绝对坐标方式
	mpr_needReReadData = false;
}


CGlbGlobeGridObject::~CGlbGlobeGridObject(void)
{
}

GlbGlobeObjectTypeEnum CGlbGlobeGridObject::GetType()
{
	return GLB_OBJECTTYPE_GRID;
}

glbBool CGlbGlobeGridObject::Load(xmlNodePtr* node,glbWChar* prjPath)
{
	CGlbGlobeRObject::Load(node,prjPath);

	xmlChar *szKey = NULL;
	xmlNodePtr pnode = *node;

	if(pnode && (!xmlStrcmp(pnode->name,(const xmlChar *)"RenderInfo")))
	{
		GlbGlobeObjectTypeEnum rdtype;
		xmlNodePtr rdchild = pnode->xmlChildrenNode;

		if(rdchild && (!xmlStrcmp(rdchild->name,(const xmlChar *)"RenderType")))
		{
			szKey = xmlNodeGetContent(rdchild);
			sscanf((char*)szKey,"%d",&rdtype);
			xmlFree(szKey);

			rdchild = rdchild->next;
		}

		GlbGridRenderInfo *renderInfo = new GlbGridRenderInfo();
		renderInfo->Load(rdchild,prjPath);
		mpr_renderInfo = renderInfo;
	}
	return true;
}

glbBool CGlbGlobeGridObject::Load2( xmlNodePtr* node,glbWChar* prjPath )
{
	return true;
}

glbBool CGlbGlobeGridObject::Save(xmlNodePtr node,const glbWChar* prjPath)
{
	CGlbGlobeRObject::Save(node,prjPath);

	xmlNodePtr rdnode = xmlNewNode(NULL,BAD_CAST "RenderInfo");
	xmlAddChild(node,rdnode);
	if(mpr_renderInfo)
	{
		char str[32];
		sprintf_s(str,"%d",mpr_renderInfo->type);
		xmlNewTextChild(rdnode,NULL,BAD_CAST "RenderType",BAD_CAST str);

		GlbGridRenderInfo *renderInfo = dynamic_cast<GlbGridRenderInfo*>(mpr_renderInfo.get());
		if(renderInfo)
			renderInfo->Save(rdnode,prjPath);
	}
	return true;
}

glbref_ptr<CGlbExtent> CGlbGlobeGridObject::GetBound(glbBool isWorld)
{
	if (mpt_globe == NULL || mpt_dataset == NULL) return NULL;

	IGlbGeoDataset* pdset = NULL;
	mpt_dataset->QueryInterface(L"IGlbGeoDataset",(void**)&pdset);
	if (pdset==NULL)
		return NULL;
	glbref_ptr<CGlbExtent> geoExt = const_cast<CGlbExtent*>(pdset->GetExtent());

	if (isWorld == false)
	{//地理坐标
		return geoExt;	
	}
	else
	{
		glbref_ptr<CGlbExtent> worldExt = new CGlbExtent();
		osg::ref_ptr<osg::Node> node = mpt_node;
		if(   node != NULL
			&&node->getBound().valid() )
		{
			//方案1
			//CGlbCalculateBoundBoxVisitor bboxV;
			//mpt_node->accept(bboxV);
			//osg::BoundingBoxd bb =bboxV.getBoundBox();
			//方案2
			osg::BoundingSphere bs = node->getBound();
			osg::BoundingBoxd bb;
			bb.expandBy(bs._center - osg::Vec3d(bs._radius,bs._radius,bs._radius));
			bb.expandBy(bs._center + osg::Vec3d(bs._radius,bs._radius,bs._radius));
			worldExt->SetMin(bb.xMin(),bb.yMin(),bb.zMin());
			worldExt->SetMax(bb.xMax(),bb.yMax(),bb.zMax());
		}
		else
		{//osg 节点还没生成.
			GlbGlobeTypeEnum globeType = mpt_globe->GetType();
			if (globeType==GLB_GLOBETYPE_GLOBE)
			{
				double x,y,z;
				double xMin,xMax,yMin,yMax,zMin,zMax;
				geoExt->Get(&xMin,&xMax,&yMin,&yMax,&zMin,&zMax);			
				g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(yMin),osg::DegreesToRadians(xMin),zMin,x,y,z);	
				worldExt->Merge(x,y,z);			
				g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(yMax),osg::DegreesToRadians(xMax),zMax,x,y,z);	
				worldExt->Merge(x,y,z);
				g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(yMin),osg::DegreesToRadians(xMax),zMin,x,y,z);	
				worldExt->Merge(x,y,z);
				g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(yMin),osg::DegreesToRadians(xMax),zMax,x,y,z);	
				worldExt->Merge(x,y,z);
				g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(yMin),osg::DegreesToRadians(xMin),zMax,x,y,z);	
				worldExt->Merge(x,y,z);
				g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(yMax),osg::DegreesToRadians(xMin),zMin,x,y,z);	
				worldExt->Merge(x,y,z);
				g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(yMax),osg::DegreesToRadians(xMin),zMax,x,y,z);	
				worldExt->Merge(x,y,z);
				g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(yMax),osg::DegreesToRadians(xMax),zMin,x,y,z);	
				worldExt->Merge(x,y,z);
			}
			else if (globeType==GLB_GLOBETYPE_FLAT)		
				worldExt = geoExt;	
		}
		return worldExt;
	}
	return NULL;
}

glbDouble CGlbGlobeGridObject::GetDistance(osg::Vec3d &cameraPos,glbBool isCompute)
{
	if(isCompute == false)
		return mpr_distance;

	if (mpt_dataset==NULL || mpt_globe==NULL)
		return DBL_MAX;
	IGlbGeoDataset* pdset = NULL;
	mpt_dataset->QueryInterface(L"IGlbGeoDataset",(void**)&pdset);
	if (pdset==NULL)
		return NULL;
	glbref_ptr<CGlbExtent> ext = const_cast<CGlbExtent*>(pdset->GetExtent());
	if (ext==NULL)
		return DBL_MAX;
	if (mpt_isCameraAltitudeAsDistance)	
		mpr_distance = cameraPos.z();	
	else
	{
		glbDouble xOrLon,yOrLat,zOrAlt;
		ext->GetCenter(&xOrLon,&yOrLat,&zOrAlt);
		GlbGlobeTypeEnum globeType = mpt_globe->GetType();	

		osg::Vec3d cameraPoint = cameraPos;
		if (globeType==GLB_GLOBETYPE_GLOBE)
		{
			g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(yOrLat),osg::DegreesToRadians(xOrLon),zOrAlt,xOrLon,yOrLat,zOrAlt);		
			g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(cameraPos.y()),
				osg::DegreesToRadians(cameraPos.x()),cameraPos.z(),cameraPoint.x(),cameraPoint.y(),cameraPoint.z());		
		}

		mpr_distance = (osg::Vec3d(xOrLon,yOrLat,zOrAlt)-cameraPoint).length();
	}

	return mpr_distance;
}

IGlbGeometry *CGlbGlobeGridObject::GetOutLine()
{
	if (mpt_dataset==NULL || mpt_globe==NULL)
		return NULL;

	if (mpr_outline==NULL)
	{
		IGlbGeoDataset* pdset = NULL;
		mpt_dataset->QueryInterface(L"IGlbGeoDataset",(void**)&pdset);
		if (pdset==NULL)
			return NULL;
		glbref_ptr<CGlbExtent> geoExt = const_cast<CGlbExtent*>(pdset->GetExtent());

		if (geoExt==NULL)
			return NULL;

		glbDouble minX,maxX,minY,maxY;
		geoExt->Get(&minX,&maxX,&minY,&maxY);

		CGlbPolygon* poly = new CGlbPolygon();
		CGlbLine* extRing = new CGlbLine();
		extRing->AddPoint(minX,minY);
		extRing->AddPoint(maxX,minY);
		extRing->AddPoint(maxX,maxY);
		extRing->AddPoint(minX,maxY);

		poly->SetExtRing(extRing);

		mpr_outline = poly;
	}
	return mpr_outline.get();
}

void CGlbGlobeGridObject::ReadData(glbInt32 level,glbBool isDirect)
{
	//MessageBox(NULL,L"ReadData begin.",L"ok",MB_OK);

	glbref_ptr<GlbGridRenderInfo> renderInfo = dynamic_cast<GlbGridRenderInfo*>(mpr_renderInfo.get());
	if( renderInfo == NULL || mpt_altitudeMode != GLB_ALTITUDEMODE_ABSOLUTE )
	{mpt_loadState=false;return;}

	if (renderInfo == NULL) {mpt_loadState=false;return;}
	if (mpt_dataset == NULL){mpt_loadState=false;return;}	
	IGlbSGridDataset* pdset = NULL;
	mpt_dataset->QueryInterface(L"IGlbSGridDataset",(void**)&pdset);
	if (pdset==NULL) {mpt_loadState=false;return;}

	//CGlbGlobeSectionSymbol symbol;
	osg::ref_ptr<osg::Node> node = Draw();//symbol.Draw(this,NULL);
	if (node==NULL){mpt_loadState=false;return;}
	
	osg::Switch* swiNode = node->asSwitch();
	//if (swiNode)
	//	DealModelOpacity(swiNode,renderInfo);
	if(mpr_needReReadData)
	{
		/*
		*   在检测mpr_needReReadData时,其它线程设置mpr_needReReadData=true还没执行！！！.
		*/
		mpr_needReReadData = false;
		return ReadData(level,true);
	}

	if (swiNode==NULL)
		return;
	//if (mpt_altitudeMode != GLB_ALTITUDEMODE_ONTERRAIN)
	//{// 非贴地模式需要控制内存增量
		//增加使用内存		
		glbInt32 objsize = ComputeNodeSize(swiNode);
		glbBool isOK = CGlbGlobeMemCtrl::GetInstance()->IncrementUsedMem(objsize);
		if(isOK||isDirect)
		{
			// 预先计算bound，节省时间
			swiNode->getBound();

			mpt_preNode   = mpt_node;
			mpt_node      = swiNode;
			mpt_currLevel = level;		
			mpr_objSize   = objsize;
		}else{
			swiNode = NULL;
		}
	//}
	mpt_loadState = false;	
}

void CGlbGlobeGridObject::LoadData(glbInt32 level)
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
	glbref_ptr<GlbGridRenderInfo> renderInfo = dynamic_cast<GlbGridRenderInfo *>(mpr_renderInfo.get());
	if(renderInfo == NULL)
		return;
	// 非3D模型类型，且已经load过，就不用再次load
	if (mpt_currLevel != -1) 	
		return;
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
	glbref_ptr<CGlbGlobeGridTask> task = new CGlbGlobeGridTask(this,level);
	mpt_globe->mpr_taskmanager->AddTask(task.get());
}

void CGlbGlobeGridObject::AddToScene()
{
	/*
	*   mpt_isInHangTasks = true ------ 调用 AddToScene的线程 (A)
	*   mpt_isInHangTasks = false ----- frame 线程            (B)
	*   
	*/
	if(mpt_node == NULL)return;
	//if(mpt_isFaded)
	//{
	//	osg::ref_ptr<CGlbFadeInOutCallback> fadeCb = 
	//		new CGlbFadeInOutCallback(mpt_fadeStartAlpha,mpt_fadeEndAlpha,this,mpt_fadeDurationTime,mpt_fadeColor);
	//	mpt_node->addUpdateCallback(fadeCb.get());
	//}
	GlbScopedLock<GlbCriticalSection> lock(mpr_addToScene_critical);
	bool needUpdate = false;
	if(mpt_preNode == NULL)
	{//没有产生新osg节点
		if(mpt_HangTaskNum == 0
		   && mpt_node->getNumParents() == 0
		   && mpt_altitudeMode != GLB_ALTITUDEMODE_ONTERRAIN)
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
			task->mpr_size = ComputeNodeSize(mpt_preNode);
			mpt_globe->mpr_p_callback->AddRemoveTask(task);
			mpt_preNode = NULL;
			CGlbGlobeMemCtrl::GetInstance()->DecrementUsedMem(task->mpr_size);
		}
		if(mpt_altitudeMode != GLB_ALTITUDEMODE_ONTERRAIN)
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
		mpt_globe->UpdateObject(this);
		glbBool isnew = mpt_isDispShow && mpt_isShow;
		if (isnew == false)			
			mpt_node->asSwitch()->setValue(0,false);
		//if (mpt_isSelected)
		//	DealSelected();
		//if(mpt_isBlink)
		//	DealBlink();
	}
}

glbInt32 CGlbGlobeGridObject::RemoveFromScene(glbBool isClean)
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
		// 错误，不能加这种逻辑!!!2017.2.13 ml
		//if (mpt_feature != NULL && mpt_featureLayer != NULL)
		//{
		//	CGlbGlobeFeatureLayer* fl = dynamic_cast<CGlbGlobeFeatureLayer*>(mpt_featureLayer);
		//	if (fl)
		//		fl->NotifyFeatureDelete(mpt_feature->GetOid());
		//}
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

/*
*    DirectDraw 与 LoadData  要互斥分:生产数据
*    mpr_loadState【glbBool】作为互斥分开.
*    Dispatcher线程---在 AddToScene  互斥.
*/
void CGlbGlobeGridObject::DirectDraw(glbInt32 level)
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
}

glbBool CGlbGlobeGridObject::SetAltitudeMode(GlbAltitudeModeEnum mode,glbBool &isNeedDirectDraw)
{
	isNeedDirectDraw = false;
	if (mode!=GLB_ALTITUDEMODE_ABSOLUTE)
		return false;
	return true;
}

glbBool CGlbGlobeGridObject::SetRenderInfo(GlbRenderInfo *renderInfo,glbBool &isNeedDirectDraw)
{
	isNeedDirectDraw = false;
	if (renderInfo == NULL || renderInfo->type != GLB_OBJECTTYPE_GRID)  return false;
	GlbGridRenderInfo *gridRenderInfo = dynamic_cast<GlbGridRenderInfo *>(renderInfo);
	if (gridRenderInfo == NULL) return false;		
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
		/*
			如果isNeedDirectDraw=true， 则图层渲染器renderer调用SetRenderInfo接口会导致
			图层中所有对象都要加载，如果图层对象量很大会直接导致崩溃！！！！
		*/
		//isNeedDirectDraw = true;
		return true;
	}
	
	glbBool rt = false;
	rt = DealGridRenderInfo(gridRenderInfo,isNeedDirectDraw);
	if (rt)
	{
		mpr_renderInfo   = renderInfo;
	}
	mpt_loadState    = false;
	return rt;
}

GlbRenderInfo *CGlbGlobeGridObject::GetRenderInfo()
{
	return mpr_renderInfo.get();
}

glbBool CGlbGlobeGridObject::SetShow(glbBool isShow,glbBool isOnState)
{
	if (mpt_node==NULL && isOnState==true)
		return false;

	if (mpt_isEdit && isOnState)// 编辑状态不再由调度控制显隐
		return true;

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

	glbref_ptr<GlbGridRenderInfo> renderInfo = dynamic_cast<GlbGridRenderInfo *>(mpr_renderInfo.get());
	if (!renderInfo) return false;

	//if( mpt_altitudeMode == GLB_ALTITUDEMODE_ONTERRAIN )
	//{// 如果是贴地绘制对象，需要刷新贴地区域
	//	/*
	//	* mpr_isCanDrawImage控制在DrawToImage时是否需要绘制
	//	*/
	//	if (mpr_isCanDrawImage != isnew)
	//	{
	//		mpr_isCanDrawImage = isnew;
	//		// 刷新贴地区域
	//		DirtyOnTerrainObject();		
	//	}
	//}
	//else
	//{
		if (mpt_node)
		{
			if(isnew)
			{
				mpt_node->asSwitch()->setAllChildrenOn();
			}
			else
				mpt_node->asSwitch()->setAllChildrenOff();
		}

	//}

	return true;
}

glbBool CGlbGlobeGridObject::SetSelected(glbBool isSelected)
{
	if(mpt_isSelected == isSelected) return true;
	mpt_isSelected = isSelected;
	if (mpt_node != NULL && mpt_altitudeMode!=GLB_ALTITUDEMODE_ONTERRAIN)
	{
		DealSelected();
	}
	return true;
}

glbBool CGlbGlobeGridObject::SetBlink(glbBool isBlink)
{
	if (mpt_isBlink == isBlink) return true;
	mpt_isBlink = isBlink;
	if (mpt_node != NULL && mpt_altitudeMode!=GLB_ALTITUDEMODE_ONTERRAIN)
		DealBlink();
	return true;
}

glbInt32 CGlbGlobeGridObject::GetOsgNodeSize()
{
	if (mpt_node == NULL) return 0;
	if (mpr_objSize==0)
		mpr_objSize = ComputeNodeSize(mpt_node);
	return mpr_objSize;
}

void CGlbGlobeGridObject::DealSelected()
{
	glbref_ptr<GlbGridRenderInfo> renderInfo = dynamic_cast<GlbGridRenderInfo *>(mpr_renderInfo.get());
	if (!renderInfo) return;

	osg::ref_ptr<osg::StateSet> stateset = mpt_node->getOrCreateStateSet();	
	osg::ref_ptr<osg::Material> material = dynamic_cast<osg::Material *>(stateset->getAttribute(osg::StateAttribute::MATERIAL));
	if(material == NULL)
		material = new osg::Material;
	if (mpt_isSelected)
	{
		material->setDiffuse(osg::Material::FRONT_AND_BACK,osg::Vec4(1,0,0,1));		
		//{
		//	material->setDiffuse(osg::Material::FRONT_AND_BACK,osg::Vec4(0,0,0,1));
		//	material->setAmbient(osg::Material::FRONT_AND_BACK,osg::Vec4(0,0,0,1));
		//	material->setSpecular(osg::Material::FRONT_AND_BACK,osg::Vec4(0,0,0,1));
		//	material->setEmission(osg::Material::FRONT_AND_BACK,osg::Vec4(1,0,0,0.1));
		//}
		stateset->setAttribute(material,osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
		stateset->setMode(GL_LIGHTING,osg::StateAttribute::ON);
		stateset->setMode(GL_TEXTURE_2D,osg::StateAttribute::OFF | osg::StateAttribute::OVERRIDE);
	}
	else
	{
		if (material)
			stateset->removeAttribute(material);
		//if(markerInfo && markerInfo->symType == GLB_MARKERSYMBOL_MODEL)
		//{
		//	GlbMarkerModelSymbolInfo *modelInfo = dynamic_cast<GlbMarkerModelSymbolInfo *>(mpr_renderInfo.get());
		//	DealModelOpacity(mpt_node,modelInfo);
		//}
		stateset->setMode(GL_TEXTURE_2D,osg::StateAttribute::ON);
	}
}

void CGlbGlobeGridObject::DealBlink()
{
	
}

glbInt32 CGlbGlobeGridObject::ComputeNodeSize(osg::Node *node)
{
	CGlbCalculateNodeSizeVisitor cnsv;
	node->accept(cnsv);
	return cnsv.getTextureSize() + cnsv.getNodeMemSize();
}

glbBool CGlbGlobeGridObject::DealGridRenderInfo(GlbRenderInfo* newSectionRenderInfo, glbBool &isNeedDirectDraw)
{// 全部要求重绘
	GlbGridRenderInfo *tempInfo = dynamic_cast<GlbGridRenderInfo *>(mpr_renderInfo.get());
	GlbGridRenderInfo *_newSectionRenderInfo = dynamic_cast<GlbGridRenderInfo *>(newSectionRenderInfo);
	if (tempInfo == NULL) return false;

	if (tempInfo->isShowEdge != _newSectionRenderInfo->isShowEdge)
	{
		if (mpr_edgeGeode)
		{
			if (_newSectionRenderInfo->isShowEdge == true)
				mpr_edgeGeode->setNodeMask(0xffffffff);
			else
				mpr_edgeGeode->setNodeMask(0x0);
		}
		isNeedDirectDraw = false;
		return true;
	}
	if (tempInfo->colorLocate != _newSectionRenderInfo->colorLocate || tempInfo->gridOpacity != _newSectionRenderInfo->gridOpacity)
	{
		DealGridColorOpacity(_newSectionRenderInfo->colorLocate, _newSectionRenderInfo->gridOpacity);
		isNeedDirectDraw = false;
		return true;
	}

	//if (tempInfo->mode != _newSectionRenderInfo->mode)
	//{
	//	isNeedDirectDraw = true;
	//	return true;
	//}
	isNeedDirectDraw = true;
	return true;
}

glbBool CGlbGlobeGridObject::DealGridColorOpacity(CGlbWString colorLocate, glbInt32 opacity)
{
	if (mpt_dataset==NULL) return false;
	IGlbSGridDataset* sdset = NULL;
	mpt_dataset->QueryInterface(L"IGlbSGridDataset",(void**)&sdset);
	if (sdset == NULL)
		return false;
	if (mpr_faceGeode == NULL || mpr_faceGeode->getNumDrawables() < 1)
		return false;

	sdset->SetColorScheme(const_cast<glbWChar*>(colorLocate.c_str()));

	osg::ref_ptr<osg::Vec4Array> clrArray = new osg::Vec4Array;

	cwSGridFacet* pSGridIndex = NULL;
	glbInt32 cnt; // index 长度
	sdset->GetBoundCells(&pSGridIndex,&cnt);

	cwSGridCell gridCell;	

	glbBool isvalid;
	glbInt32 color;
	for(glbInt32 idx = 0; idx < cnt; idx++)
	{			
		sdset->GetCellByIndex(pSGridIndex[idx].CellIndex,&gridCell, &isvalid);

		if( isvalid == false)
			continue;

		BYTE orientation = pSGridIndex[idx].Orientation;

		sdset->GetCellColorByIndex(pSGridIndex[idx].CellIndex,&color,&isvalid);
		osg::Vec4 clr(1,1,1,1);
		if (isvalid)
		{
			glbInt32 red = LOBYTE((color)>>16);//GetBValue(color);
			glbInt32 green = LOBYTE(((color & 0xFFFF)) >> 8);//GetGValue(color);
			glbInt32 blue = LOBYTE(color);//GetRValue(color);
			glbInt32 alpha = opacity;//LOBYTE((color)>>24);//GetAValue(color);
			clr = osg::Vec4(red / 255.0,green / 255.0,blue / 255.0,alpha / 100.0);
		}		

		//// 0-3-2-1  1-2-6-5 2-3-7-6 0-4-7-3 0-1-5-4 4-5-6-7
		osg::ref_ptr<osg::UIntArray> pIndexArray = new osg::UIntArray;	
		switch(orientation)
		{
		case 2:
			{
				pIndexArray->push_back(4);	pIndexArray->push_back(5);	pIndexArray->push_back(6);	pIndexArray->push_back(7);				
			}			
			break;
		case 1:
			{
				pIndexArray->push_back(0);	pIndexArray->push_back(3);	pIndexArray->push_back(2);	pIndexArray->push_back(1);				
			}
			break;
		case 3:
			{
				pIndexArray->push_back(0);	pIndexArray->push_back(4);	pIndexArray->push_back(7);	pIndexArray->push_back(3);				
			}
			break;
		case 4:
			{
				pIndexArray->push_back(1);	pIndexArray->push_back(2);	pIndexArray->push_back(6);	pIndexArray->push_back(5);
			}
			break;
		case 5:
			{
				pIndexArray->push_back(0);	pIndexArray->push_back(1);	pIndexArray->push_back(5);	pIndexArray->push_back(4);
			}
			break;
		case 6:
			{
				pIndexArray->push_back(2);	pIndexArray->push_back(3);	pIndexArray->push_back(7);	pIndexArray->push_back(6);
			}
			break;
		}

		//cwVector3D dpt;
		for (UINT kk = 0; kk < pIndexArray->size(); kk++)
		{			
			clrArray->push_back(clr);
		}
	}

	mpr_faceGeode->getDrawable(0)->asGeometry()->setColorArray(clrArray);
	mpr_faceGeode->getDrawable(0)->asGeometry()->setColorBinding(osg::Geometry::AttributeBinding::BIND_PER_VERTEX);

	if (opacity < 100)
	{// 半透明
		mpr_faceGeode->getDrawable(0)->asGeometry()->getOrCreateStateSet()->setRenderingHint( osg::StateSet::TRANSPARENT_BIN );	
		mpr_faceGeode->getDrawable(0)->asGeometry()->getOrCreateStateSet()->setMode(GL_BLEND, osg::StateAttribute::ON);
	}
	else
	{
		mpr_faceGeode->getDrawable(0)->asGeometry()->getOrCreateStateSet()->setRenderingHint( osg::StateSet::OPAQUE_BIN );
		mpr_faceGeode->getDrawable(0)->asGeometry()->getOrCreateStateSet()->setMode(GL_BLEND, osg::StateAttribute::OFF);
	}

	return false;
}

osg::Node* CGlbGlobeGridObject::Draw()
{
	if (mpt_dataset==NULL || mpt_globe==NULL)
		return NULL;
	IGlbSGridDataset* sdset = NULL;
	mpt_dataset->QueryInterface(L"IGlbSGridDataset",(void**)&sdset);
	if (sdset == NULL)
		return NULL;

	GlbGridRenderInfo *renderInfo = dynamic_cast<GlbGridRenderInfo *>(mpr_renderInfo.get());
	if (renderInfo == NULL)
		return NULL;

	//MessageBox(NULL,L"CGlbGlobeGridObject Draw begin!",L"Notify",MB_OK);

	osg::Node* node = NULL;
	switch(renderInfo->mode)
	{
	case GLB_RENDERMODE_GRID:
		{
			node = buildGrid();
			// ceshi 2018.8.3 4个sgrid给张所看
			//node = buildTempLargeCopyGrid();
		}
		break;
	case GLB_RENDERMODE_LINECLIP:
		node = buildLineClipMap(renderInfo->geo.get());
		break;
	case GLB_RENDERMODE_AXISCLIP:
		node = buildNINJClipMap(renderInfo->I,renderInfo->J);
		//node = buildBarrierClipMap(renderInfo->I,renderInfo->J);
		break;
	case GLB_RENDERMODE_BARRIERCLIP:
		node = buildBarrierClipMap(renderInfo->INum,renderInfo->JNum);
		//node = buildNINJClipMap(renderInfo->INum,renderInfo->JNum);
		break;
	}

	//MessageBox(NULL,L"CGlbGlobeGridObject Draw end!",L"Notify",MB_OK);
	return node;
}

osg::Node* CGlbGlobeGridObject::buildGrid()
{
	IGlbSGridDataset* sdset = NULL;
	mpt_dataset->QueryInterface(L"IGlbSGridDataset",(void**)&sdset);
	if (sdset == NULL)
		return NULL;

	GlbGridRenderInfo *renderInfo = dynamic_cast<GlbGridRenderInfo *>(mpr_renderInfo.get());
	if (renderInfo == NULL)
		return NULL;

	glbInt32 opacity = 100;
	if (renderInfo)
		opacity = renderInfo->gridOpacity;

	cwSGridFacet* pSGridIndex = NULL;
	glbInt32 cnt; // index 长度
	sdset->GetBoundCells(&pSGridIndex,&cnt);
	if( cnt == 0 )return NULL;

	if (mpr_faceGeode) mpr_faceGeode = NULL;
	if (mpr_edgeGeode) mpr_edgeGeode = NULL;

	IGlbGeoDataset* gdset = NULL;
	mpt_dataset->QueryInterface(L"IGlbGeoDataset",(void**)&gdset);
	const CGlbExtent* pB = gdset->GetExtent();
	glbDouble cx,cy,cz;
	pB->GetCenter(&cx,&cy,&cz);
	//// 偏移原点
	osg::Vec3 originVec(cx,cy,cz);

	//第二种渲染方式： 一个Geometry，一个批次全部渲染出来，使用顶点颜色和顶点法向
	osg::ref_ptr<osg::Geometry> geom = new osg::Geometry;	
	osg::ref_ptr<osg::Vec3Array> vtArray = new osg::Vec3Array;	
	osg::ref_ptr<osg::Vec3Array> norArray = new osg::Vec3Array;
	osg::ref_ptr<osg::Vec4Array> clrArray = new osg::Vec4Array;

	geom->setVertexArray(vtArray);
	geom->setNormalArray(norArray);
	geom->setNormalBinding(osg::Geometry::AttributeBinding::BIND_PER_VERTEX);
	geom->setColorArray(clrArray);
	geom->setColorBinding(osg::Geometry::AttributeBinding::BIND_PER_VERTEX);

	cwSGridCell gridCell;
	double nTransparency=0.0;	//透明度
	//glbInt32 NI,NJ,NK;
	//NI = sdset->get_NI();
	//NJ = sdset->get_NJ();
	//NK = sdset->get_NK();
	glbBool isvalid;
	glbInt32 color;

	//{
	//	FILE *fp = fopen("d:\\trace.txt","w");
	//	char szMsg[1024];
	//	for(int i = 0; i < 500; i ++ )
	//	{
	//		sdset->GetCellColorByIndex(i,&color,&isvalid);
	//		fprintf(fp,"%d\n",color);
	//	}
	//	fclose(fp);
	//}

	//MessageBoxA(NULL,"","",MB_OK);

	for(glbInt32 idx = 0; idx < cnt; idx++)
	{			
		sdset->GetCellByIndex(pSGridIndex[idx].CellIndex,&gridCell, &isvalid);

		if( isvalid == false)
			continue;

		BYTE orientation = pSGridIndex[idx].Orientation;

		sdset->GetCellColorByIndex(pSGridIndex[idx].CellIndex,&color,&isvalid);
		osg::Vec4 clr(1,1,1,1);
		if (isvalid)
		{
			glbInt32 blue = LOBYTE((color)>>16);//GetBValue(color);
			glbInt32 green = LOBYTE(((color & 0xFFFF)) >> 8);//GetGValue(color);
			glbInt32 red = LOBYTE(color);//GetRValue(color);
			glbInt32 alpha = opacity;//LOBYTE((color)>>24);//GetAValue(color);
			clr = osg::Vec4(red / 255.0,green / 255.0,blue / 255.0,alpha / 100.0);
		}		

		//// 0-3-2-1  1-2-6-5 2-3-7-6 0-4-7-3 0-1-5-4 4-5-6-7
		osg::ref_ptr<osg::UIntArray> pIndexArray = new osg::UIntArray;	
		switch(orientation)
		{
		case 2:
			{
				pIndexArray->push_back(4);	pIndexArray->push_back(5);	pIndexArray->push_back(6);	pIndexArray->push_back(7);
				for (int kk = 0; kk < 4; kk++)
					norArray->push_back(osg::Vec3(0,0,1));
			}			
			break;
		case 1:
			{
				pIndexArray->push_back(0);	pIndexArray->push_back(3);	pIndexArray->push_back(2);	pIndexArray->push_back(1);
				for (int kk = 0; kk < 4; kk++)
					norArray->push_back(osg::Vec3(0,0,-1));
			}
			break;
		case 3:
			{
				pIndexArray->push_back(0);	pIndexArray->push_back(4);	pIndexArray->push_back(7);	pIndexArray->push_back(3);
				for (int kk = 0; kk < 4; kk++)
					norArray->push_back(osg::Vec3(-1,0,0));
			}
			break;
		case 4:
			{
				pIndexArray->push_back(1);	pIndexArray->push_back(2);	pIndexArray->push_back(6);	pIndexArray->push_back(5);
				for (int kk = 0; kk < 4; kk++)
					norArray->push_back(osg::Vec3(1,0,0));
			}
			break;
		case 5:
			{
				pIndexArray->push_back(0);	pIndexArray->push_back(1);	pIndexArray->push_back(5);	pIndexArray->push_back(4);
				for (int kk = 0; kk < 4; kk++)
					norArray->push_back(osg::Vec3(0,-1,0));
			}
			break;
		case 6:
			{
				pIndexArray->push_back(2);	pIndexArray->push_back(3);	pIndexArray->push_back(7);	pIndexArray->push_back(6);
				for (int kk = 0; kk < 4; kk++)
					norArray->push_back(osg::Vec3(0,1,0));
			}
			break;
		}

		//cwVector3D dpt;
		for (UINT kk = 0; kk < pIndexArray->size(); kk++)
		{			
			//pSGrid->GetVertex(pIndexArray->at(kk),&vtIndex);
			double xx = gridCell.Node[pIndexArray->at(kk)].X;
			double yy = gridCell.Node[pIndexArray->at(kk)].Y;
			double zz = gridCell.Node[pIndexArray->at(kk)].Z;
			vtArray->push_back(osg::Vec3(xx,yy,zz) /*- originVec*/);

			//char szMsg[1024];
			//fprintf(fp, "%f,%f,%f,%f,%f,%f\n", clr.r(), clr.g(), clr.b(), xx, yy, zz);
			//MessageBoxA(NULL, szMsg, "MSG", MB_OK);
			clrArray->push_back(clr);
		}
	}

	//fclose(fp);

	geom->addPrimitiveSet(new osg::DrawArrays(GL_QUADS,0, vtArray->size()));
	//需要设置一定的PolygonOffset,让其更靠近视点,避免与其它面重叠
	osg::ref_ptr<osg::PolygonOffset> poffset = new osg::PolygonOffset(1.0,1.0);
	geom->getOrCreateStateSet()->setAttributeAndModes(poffset,osg::StateAttribute::ON);

	/*-----------end of commended for debug--------------------------*/

	// 如果是单色，则使用材质颜色并启动双面光照，如果是颜色数组则禁用光照
	//osg::ref_ptr<osg::LightModel> pLightModel = new osg::LightModel;
	//pLightModel->setTwoSided(true);
	//pLightModel->setAmbientIntensity(osg::Vec4(0.8, 0.8, 0.8, 1.0));
	//pStateSet->setAttributeAndModes(pLightModel.get(), osg::StateAttribute::ON);
	//pStateSet->setMode(GL_LIGHTING, osg::StateAttribute::ON);

	// end

	//设置背部剔除看不见背面东西
	//osg::ref_ptr<osg::CullFace> cullface=new osg::CullFace(osg::CullFace::BACK);
	//geom->getOrCreateStateSet()->setAttribute(cullface.get());
	//geom->getOrCreateStateSet()->setMode(GL_CULL_FACE,osg::StateAttribute::ON);

	geom->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);

	osg::ref_ptr<osg::Geode> faceGeode = new osg::Geode;
	faceGeode->addDrawable(geom);
	faceGeode->setName("gridFace");
	mpr_faceGeode = faceGeode;

	osg::ref_ptr<osg::Geode> edgeGeode = new osg::Geode;
	{// 边界线		
		osg::ref_ptr<osg::Geometry> geomEdge = new osg::Geometry;	
		geomEdge->setName("gridEdge");
		//osg::ref_ptr<osg::Vec3Array> vtArray = new osg::Vec3Array;
		//vtArray->reserve(8*cnt);
		osg::ref_ptr<osg::Vec4Array> clrArray = new osg::Vec4Array;

		geomEdge->setVertexArray(vtArray);
		geomEdge->setNormalBinding(osg::Geometry::AttributeBinding::BIND_PER_VERTEX);
		geomEdge->setColorArray(clrArray);
		clrArray->push_back(osg::Vec4(0.8,0,0,0.2));
		geomEdge->setColorBinding(osg::Geometry::AttributeBinding::BIND_OVERALL);

		osg::ref_ptr<osg::UIntArray> pIndexArray2 = new osg::UIntArray;		

		long cellcount = 0;
		for(long idx = 0; idx < cnt; idx++)
		{			
			sdset->GetCellByIndex(pSGridIndex[idx].CellIndex,&gridCell, &isvalid);	

			if( isvalid == false)
				continue;			

			BYTE orientation = pSGridIndex[idx].Orientation;
			if (orientation < 1 || orientation > 6)
				continue;

			int base = 4*idx;	
			for (int kk = 0; kk < 3; kk++)
			{
				pIndexArray2->push_back(base+kk);
				pIndexArray2->push_back(base+kk+1);
			}
			pIndexArray2->push_back(base+3);
			pIndexArray2->push_back(base);
		}
		geomEdge->addPrimitiveSet(new osg::DrawElementsUInt(GL_LINES,pIndexArray2->size(),&pIndexArray2->front()));

		geomEdge->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
		geomEdge->getOrCreateStateSet()->setRenderingHint(osg::StateSet::RenderingHint::TRANSPARENT_BIN);
		geomEdge->getOrCreateStateSet()->setMode(GL_BLEND, osg::StateAttribute::ON);

		edgeGeode->addDrawable(geomEdge);

		edgeGeode->setName("edgeFace");
		if (renderInfo->isShowEdge == false)
			edgeGeode->setNodeMask(0x0);

		mpr_edgeGeode = edgeGeode;
	}

	osg::ref_ptr<osg::Switch> grp = new osg::Switch;
	grp->addChild(faceGeode);
	grp->addChild(edgeGeode);
	return grp.release();
}

// 线切面
osg::Node* CGlbGlobeGridObject::buildLineClipMap(IGlbGeometry* geo)
{
	if ( geo->GetType() != GLB_GEO_LINE )
		return NULL;
	IGlbDataset* dset = GetDataset();
	IGlbSGridDataset* sdset = NULL;
	dset->QueryInterface(L"IGlbSGridDataset",(void**)&sdset);
	if (sdset == NULL)
		return NULL;

	GlbGridRenderInfo *renderInfo = dynamic_cast<GlbGridRenderInfo *>(mpr_renderInfo.get());
	glbInt32 opacity = 100;
	if (renderInfo)
		opacity = renderInfo->gridOpacity;

	osg::Switch* pActor = new osg::Switch();
	CGlbLine* lnGeo = dynamic_cast<CGlbLine*>(geo);

	glbInt32 NI,NJ,NK;
	NI = sdset->get_NI();
	NJ = sdset->get_NJ();
	NK = sdset->get_NK(); 

	glbDouble stepI,stepJ;
	glbDouble orgX,orgY;
	glbVector Ivec, Jvec;
	stepI = sdset->get_StepI();
	stepJ = sdset->get_StepJ();
	Ivec = sdset->get_StepVectorI();
	Jvec = sdset->get_StepVectorJ();
	sdset->GetOrigin(&orgX, &orgY);
	Ivec.z = 0;
	Jvec.z = 0;

	glbInt32 color;
	int p_num = lnGeo->GetCount();
	glbDouble ptx,pty,ptz;
	for(int i = 0; i < p_num - 1; i++)//线切割
	{
		//--切面--------------------
		osg::ref_ptr<osg::Geometry> geom = new osg::Geometry;
		osg::ref_ptr<osg::Vec3Array> vtArray = new osg::Vec3Array;	
		osg::ref_ptr<osg::Vec4Array> clrArray = new osg::Vec4Array;
		osg::ref_ptr<osg::Geode> geoNode = new osg::Geode;
		geoNode->addDrawable(geom);
		pActor->addChild(geoNode);

		geom->setVertexArray(vtArray);
		geom->setColorArray(clrArray);
		geom->setColorBinding(osg::Geometry::AttributeBinding::BIND_PER_VERTEX);
		//--切线--------------------
		osg::ref_ptr<osg::Geometry> geomLine = new osg::Geometry;
		osg::ref_ptr<osg::Vec3Array> vtArrayLine = new osg::Vec3Array;	
		osg::ref_ptr<osg::Vec4Array> clrArrayLine = new osg::Vec4Array;
		geoNode->addDrawable(geomLine);

		geomLine->setVertexArray(vtArrayLine);
		geomLine->setColorArray(clrArrayLine);
		clrArrayLine->push_back(osg::Vec4(0,0,0,0.2));
		geomLine->setColorBinding(osg::Geometry::AttributeBinding::BIND_OVERALL);
		geomLine->getOrCreateStateSet()->setMode(GL_BLEND,osg::StateAttribute::ON);
		//每段起止点的索引
		glbVector startVec, endVec;
		lnGeo->GetPoint(i,&ptx,&pty,&ptz);
		startVec.x = ptx - orgX;
		startVec.y = pty - orgY;
		startVec.z = 0;		

		int index_startI = floor((startVec*Ivec)/stepI/sqrt(Ivec.x*Ivec.x + Ivec.y*Ivec.y));
		int index_startJ = floor((startVec*Jvec)/stepJ/sqrt(Jvec.x*Jvec.x + Jvec.y*Jvec.y));
		lnGeo->GetPoint(i+1,&ptx,&pty,&ptz);
		endVec.x = ptx - orgX;
		endVec.y = pty - orgY;
		endVec.z = 0;

		int index_endI = floor((endVec*Ivec)/stepI/sqrt(Ivec.x*Ivec.x + Ivec.y*Ivec.y));
		int index_endJ = floor((endVec*Jvec)/stepJ/sqrt(Jvec.x*Jvec.x + Jvec.y*Jvec.y));

		int d_I = index_endI - index_startI;
		int d_J = index_endJ - index_startJ;
		std::vector<GlbPoint3D> clipIndex;//所有的穿过的格子索引

		GlbPoint3D p3d;
		p3d.X = index_startI; p3d.Y = index_startJ; p3d.Z = 0;
		clipIndex.push_back(p3d);
		if(abs(d_I) > abs(d_J))
		{
			double curJd = index_startJ;

			for(int curI = index_startI; curI != index_endI + d_I/abs(d_I); curI += d_I/abs(d_I))
			{
				curJd += d_J/double(abs(d_I));
				int curJ = curJd;
				if(curI<0||curI>=NI||curJ<0||curJ>=NJ)
					continue;
				GlbPoint3D p3d;
				p3d.X = curI; p3d.Y = curJ; p3d.Z = 0;
				clipIndex.push_back(p3d);
				if(curJ > 0)
				{
					p3d.Y = curJ - 1;
					clipIndex.push_back(p3d);
				}
				if(curJ < NJ - 1)
				{
					p3d.Y = curJ + 1;
					clipIndex.push_back(p3d);
				}
				if(curJ > 1)
				{
					p3d.Y = curJ - 2;
					clipIndex.push_back(p3d);
				}
				if(curJ < NJ - 2)
				{
					p3d.Y = curJ + 2;
					clipIndex.push_back(p3d);
				}
			}
		}
		else
		{
			double curId = index_startI;
			for(int curJ = index_startJ; curJ != index_endJ + d_J/abs(d_J); curJ += d_J/abs(d_J))
			{
				curId += d_I/double(abs(d_J));
				int curI = curId;
				if(curI<0||curI>=NI||curJ<0||curJ>=NJ)
					continue;
				GlbPoint3D p3d;
				p3d.X = curI; p3d.Y = curJ; p3d.Z = 0;
				clipIndex.push_back(p3d);
				if(curI > 0)
				{
					p3d.X = curI - 1;
					clipIndex.push_back(p3d);
				}
				if(curI < NI - 1)
				{
					p3d.X = curI + 1;
					clipIndex.push_back(p3d);
				}
				if(curI > 1)
				{
					p3d.X = curI - 2;
					clipIndex.push_back(p3d);
				}
				if(curI < NI - 2)
				{
					p3d.X = curI + 2;
					clipIndex.push_back(p3d);
				}
			}

		}
		cwSGridCell gridCell;
		glbBool isvalid;
		lnGeo->GetPoint(i,&ptx,&pty,&ptz);
		GlbPoint3D p1(ptx,pty,ptz);
		lnGeo->GetPoint(i+1,&ptx,&pty,&ptz);		
		GlbPoint3D p2(ptx,pty,ptz);
		for(int idx = 0; idx < clipIndex.size(); idx++)
		{
			sdset->GetCell(clipIndex.at(idx).X, clipIndex.at(idx).Y, clipIndex.at(idx).Z, &gridCell, &isvalid);
			if(!cellCrossPlane(gridCell, p1, p2)) continue;
			for(int idk = 0; idk < NK; idk++)
			{
				sdset->GetCell(clipIndex.at(idx).X, clipIndex.at(idx).Y, idk, &gridCell, &isvalid);
				if( isvalid == false)
					continue;
				GlbPoint3D d1 = gridCell.Node[0];
				GlbPoint3D d2 = gridCell.Node[1];
				GlbPoint3D d3 = gridCell.Node[2];
				GlbPoint3D d4 = gridCell.Node[3];
				GlbPoint3D d5 = gridCell.Node[4];
				GlbPoint3D d6 = gridCell.Node[5];
				GlbPoint3D d7 = gridCell.Node[6];
				GlbPoint3D d8 = gridCell.Node[7];
				std::vector<GlbPoint3D> quat;
				GlbPoint3D intr;

				if(segmentsIntr(d1, d2, p1, p2, intr))
				{
					quat.push_back(intr);
					segmentsIntr(d5, d6, p1, p2, intr);
					quat.push_back(intr);
				}
				if(segmentsIntr(d2, d3, p1, p2, intr))
				{
					quat.push_back(intr);
					segmentsIntr(d6, d7, p1, p2, intr);
					quat.push_back(intr);
				}
				if(segmentsIntr(d3, d4, p1, p2, intr))
				{
					quat.push_back(intr);
					segmentsIntr(d7, d8, p1, p2, intr);
					quat.push_back(intr);
				}
				if(segmentsIntr(d4, d1, p1, p2, intr))
				{
					quat.push_back(intr);
					segmentsIntr(d8, d5, p1, p2, intr);
					quat.push_back(intr);
				}
				//---------------------
				//if((idx == 0 || idx == clipIndex.size() - 1) && quat.size() == 2)
				if( quat.size() == 2)
				{
					double z1 = (d1.Z + d2.Z + d3.Z + d4.Z)/4;
					double z2 = (d5.Z + d6.Z + d7.Z + d8.Z)/4;
					//int clipInd = idx == 0 ? i:i+1;
					int clipInd = idx < clipIndex.size()/2? i:i+1;
					lnGeo->GetPoint(clipInd,&ptx,&pty,&ptz);
					double x = ptx;
					double y = pty;
					GlbPoint3D curcentP;
					curcentP.X = x; curcentP.Y = y; curcentP.Z = z1;
					quat.push_back(curcentP);
					curcentP.Z = z2;
					quat.push_back(curcentP);
				}
				if(quat.size() >= 4)
				{
					for(int curQ = 2; curQ < quat.size(); curQ += 2)
					{
						GlbPoint3D temp = quat[curQ];
						quat[curQ] = quat[curQ + 1];
						quat[curQ + 1] = temp;
					}

					osg::Vec4 clr(1,1,1,1);
					sdset->GetCellColor(clipIndex.at(idx).X, clipIndex.at(idx).Y, idk,&color,&isvalid);
					if (isvalid)
					{
						glbInt32 red = LOBYTE((color)>>16);//GetBValue(color);
						glbInt32 green = LOBYTE(((color & 0xFFFF)) >> 8);//GetGValue(color);
						glbInt32 blue = LOBYTE(color);//GetRValue(color);
						glbInt32 alpha = opacity;//LOBYTE((color)>>24);//GetAValue(color);
						clr = osg::Vec4(red / 255.0,green / 255.0,blue / 255.0,alpha / 100.0);
					}					
					for(int curInd = 0; curInd < quat.size(); curInd++)
					{
						//pDis->GeometryToLocal(&quat[curInd].X,&quat[curInd].Y,&quat[curInd].Z);
						vtArray->push_back(osg::Vec3(quat[curInd].X, quat[curInd].Y, quat[curInd].Z));
						clrArray->push_back(clr);
						vtArrayLine->push_back(osg::Vec3(quat[curInd].X, quat[curInd].Y, quat[curInd].Z));
						if(curInd > 0)
							vtArrayLine->push_back(osg::Vec3(quat[curInd].X, quat[curInd].Y, quat[curInd].Z));
					}
					vtArrayLine->push_back(osg::Vec3(quat[0].X, quat[0].Y, quat[0].Z));
				}
			}

		}
		geom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUADS, 0, vtArray->size()));
		geomLine->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINES, 0, vtArrayLine->size()));

		//需要设置一定的PolygonOffset,让其更靠近视点,避免与其它面重叠
		osg::ref_ptr<osg::PolygonOffset> poffset = new osg::PolygonOffset(1.0,1.0);
		geom->getOrCreateStateSet()->setAttributeAndModes(poffset,osg::StateAttribute::ON);
	}

	pActor->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
	
	return pActor;
}

// m*n 栅栏图 切面
osg::Node* CGlbGlobeGridObject::buildBarrierClipMap(glbInt32 INum, glbInt32 JNum)
{
	IGlbDataset* dset = GetDataset();
	IGlbSGridDataset* sdset = NULL;
	dset->QueryInterface(L"IGlbSGridDataset",(void**)&sdset);
	if (sdset == NULL)
		return NULL;
	osg::Switch* pActor = new osg::Switch();

	GlbGridRenderInfo *renderInfo = dynamic_cast<GlbGridRenderInfo *>(mpr_renderInfo.get());
	glbInt32 opacity = 100;
	if (renderInfo)
		opacity = renderInfo->gridOpacity;

	long p_num = 1;

	//------------------------------
	glbInt32 NI,NJ,NK;
	NI = sdset->get_NI();
	NJ = sdset->get_NJ();
	NK = sdset->get_NK(); 

	glbInt32 color;
	cwSGridCell gridCell;
	glbBool isvalid;
	int xNum = INum;
	int yNum = JNum;
	if(xNum > 0)
	{
		//--切面--------------------
		osg::ref_ptr<osg::Geometry> geom = new osg::Geometry;
		osg::ref_ptr<osg::Vec3Array> vtArray = new osg::Vec3Array;	
		osg::ref_ptr<osg::Vec4Array> clrArray = new osg::Vec4Array;
		osg::ref_ptr<osg::Geode> geoNode = new osg::Geode;
		geoNode->addDrawable(geom);
		pActor->addChild(geoNode);

		geom->setVertexArray(vtArray);
		geom->setColorArray(clrArray);
		geom->setColorBinding(osg::Geometry::AttributeBinding::BIND_PER_VERTEX);
		//--切线--------------------
		osg::ref_ptr<osg::Geometry> geomLine = new osg::Geometry;
		osg::ref_ptr<osg::Vec3Array> vtArrayLine = new osg::Vec3Array;	
		osg::ref_ptr<osg::Vec4Array> clrArrayLine = new osg::Vec4Array;
		geoNode->addDrawable(geomLine);

		geomLine->setVertexArray(vtArrayLine);
		geomLine->setColorArray(clrArrayLine);
		clrArrayLine->push_back(osg::Vec4(0,0,0,0.2));
		geomLine->setColorBinding(osg::Geometry::AttributeBinding::BIND_OVERALL);
		geomLine->getOrCreateStateSet()->setMode(GL_BLEND,osg::StateAttribute::ON);

		double xIndexStep = NI/double(xNum+1);
		for(int curIndex = 1; curIndex <= xNum; curIndex++)
		{
			int curI = xIndexStep*curIndex;
			for(int curJ = 0; curJ < NJ; curJ++)
				for(int curK = 0; curK < NK; curK++)
				{									
					sdset->GetCell(curI, curJ, curK, &gridCell, &isvalid);
					if( isvalid == false)
						continue;
					GlbPoint3D d1 = gridCell.Node[0];
					GlbPoint3D d2 = gridCell.Node[1];
					GlbPoint3D d3 = gridCell.Node[2];
					GlbPoint3D d4 = gridCell.Node[3];
					GlbPoint3D d5 = gridCell.Node[4];
					GlbPoint3D d6 = gridCell.Node[5];
					GlbPoint3D d7 = gridCell.Node[6];
					GlbPoint3D d8 = gridCell.Node[7];
					GlbPoint3D v[4];
					v[0].X = (d1.X + d2.X)/2; v[0].Y = (d1.Y + d2.Y)/2;v[0].Z = (d1.Z + d2.Z)/2;
					v[1].X = (d5.X + d6.X)/2; v[1].Y = (d5.Y + d6.Y)/2;v[1].Z = (d5.Z + d6.Z)/2;
					v[2].X = (d7.X + d8.X)/2; v[2].Y = (d7.Y + d8.Y)/2;v[2].Z = (d7.Z + d8.Z)/2;
					v[3].X = (d3.X + d4.X)/2; v[3].Y = (d3.Y + d4.Y)/2;v[3].Z = (d3.Z + d4.Z)/2;

					osg::Vec4 clr(1,1,1,1);
					sdset->GetCellColor(curI, curJ, curK,&color,&isvalid);
					if (isvalid)
					{
						glbInt32 red = LOBYTE((color)>>16);//GetBValue(color);
						glbInt32 green = LOBYTE(((color & 0xFFFF)) >> 8);//GetGValue(color);
						glbInt32 blue = LOBYTE(color);//GetRValue(color);
						glbInt32 alpha = opacity;//LOBYTE((color)>>24);//GetAValue(color);
						clr = osg::Vec4(red / 255.0,green / 255.0,blue / 255.0,alpha / 100.0);
					}		
					for(int curInd = 0; curInd < 4; curInd++)
					{
						//pDis->GeometryToLocal(&v[curInd].X,&v[curInd].Y,&v[curInd].Z);
						vtArray->push_back(osg::Vec3(v[curInd].X, v[curInd].Y, v[curInd].Z));
						clrArray->push_back(clr);
						vtArrayLine->push_back(osg::Vec3(v[curInd].X, v[curInd].Y, v[curInd].Z));
						if(curInd > 0)
							vtArrayLine->push_back(osg::Vec3(v[curInd].X, v[curInd].Y, v[curInd].Z));
					}
					vtArrayLine->push_back(osg::Vec3(v[0].X, v[0].Y, v[0].Z));
				}
		}
		geom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUADS, 0, vtArray->size()));
		geomLine->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINES, 0, vtArrayLine->size()));

		//需要设置一定的PolygonOffset,让其更靠近视点,避免与其它面重叠
		osg::ref_ptr<osg::PolygonOffset> poffset = new osg::PolygonOffset(1.0,1.0);
		geom->getOrCreateStateSet()->setAttributeAndModes(poffset,osg::StateAttribute::ON);
	}
	if(yNum > 0)
	{
		//--切面--------------------
		osg::ref_ptr<osg::Geometry> geom = new osg::Geometry;
		osg::ref_ptr<osg::Vec3Array> vtArray = new osg::Vec3Array;	
		osg::ref_ptr<osg::Vec4Array> clrArray = new osg::Vec4Array;
		osg::ref_ptr<osg::Geode> geoNode = new osg::Geode;
		geoNode->addDrawable(geom);
		pActor->addChild(geoNode);

		geom->setVertexArray(vtArray);
		geom->setColorArray(clrArray);
		geom->setColorBinding(osg::Geometry::AttributeBinding::BIND_PER_VERTEX);
		//--切线--------------------
		osg::ref_ptr<osg::Geometry> geomLine = new osg::Geometry;
		osg::ref_ptr<osg::Vec3Array> vtArrayLine = new osg::Vec3Array;	
		osg::ref_ptr<osg::Vec4Array> clrArrayLine = new osg::Vec4Array;
		geoNode->addDrawable(geomLine);

		geomLine->setVertexArray(vtArrayLine);
		geomLine->setColorArray(clrArrayLine);
		clrArrayLine->push_back(osg::Vec4(0,0,0,0.2));
		geomLine->setColorBinding(osg::Geometry::AttributeBinding::BIND_OVERALL);
		geomLine->getOrCreateStateSet()->setMode(GL_BLEND,osg::StateAttribute::ON);

		double yIndexStep = NJ/double(yNum+1);
		for(int curIndex = 1; curIndex <= yNum; curIndex++)
		{
			int curJ = yIndexStep*curIndex;
			for(int curI = 0; curI < NI; curI++)
				for(int curK = 0; curK < NK; curK++)
				{				
					sdset->GetCell(curI, curJ, curK, &gridCell, &isvalid);
					if( isvalid == false)
						continue;
					GlbPoint3D d1 = gridCell.Node[0];
					GlbPoint3D d2 = gridCell.Node[1];
					GlbPoint3D d3 = gridCell.Node[2];
					GlbPoint3D d4 = gridCell.Node[3];
					GlbPoint3D d5 = gridCell.Node[4];
					GlbPoint3D d6 = gridCell.Node[5];
					GlbPoint3D d7 = gridCell.Node[6];
					GlbPoint3D d8 = gridCell.Node[7];
					GlbPoint3D v[4];
					v[0].X = (d2.X + d3.X)/2; v[0].Y = (d2.Y + d3.Y)/2;v[0].Z = (d2.Z + d3.Z)/2;
					v[1].X = (d6.X + d7.X)/2; v[1].Y = (d6.Y + d7.Y)/2;v[1].Z = (d6.Z + d7.Z)/2;
					v[2].X = (d8.X + d5.X)/2; v[2].Y = (d8.Y + d5.Y)/2;v[2].Z = (d8.Z + d5.Z)/2;
					v[3].X = (d4.X + d1.X)/2; v[3].Y = (d4.Y + d1.Y)/2;v[3].Z = (d4.Z + d1.Z)/2;


					osg::Vec4 clr(1,1,1,1);
					sdset->GetCellColor(curI, curJ, curK, &color,&isvalid);
					if (isvalid)
					{
						glbInt32 red = LOBYTE((color)>>16);//GetBValue(color);
						glbInt32 green = LOBYTE(((color & 0xFFFF)) >> 8);//GetGValue(color);
						glbInt32 blue = LOBYTE(color);//GetRValue(color);
						glbInt32 alpha = opacity;//LOBYTE((color)>>24);//GetAValue(color);
						clr = osg::Vec4(red / 255.0,green / 255.0,blue / 255.0,alpha / 100.0);
					}		
					for(int curInd = 0; curInd < 4; curInd++)
					{
						//pDis->GeometryToLocal(&v[curInd].X,&v[curInd].Y,&v[curInd].Z);
						vtArray->push_back(osg::Vec3(v[curInd].X, v[curInd].Y, v[curInd].Z));
						clrArray->push_back(clr);
						vtArrayLine->push_back(osg::Vec3(v[curInd].X, v[curInd].Y, v[curInd].Z));
						if(curInd > 0)
							vtArrayLine->push_back(osg::Vec3(v[curInd].X, v[curInd].Y, v[curInd].Z));
					}
					vtArrayLine->push_back(osg::Vec3(v[0].X, v[0].Y, v[0].Z));
				}
		}
		geom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUADS, 0, vtArray->size()));
		geomLine->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINES, 0, vtArrayLine->size()));
		
		//需要设置一定的PolygonOffset,让其更靠近视点,避免与其它面重叠
		osg::ref_ptr<osg::PolygonOffset> poffset = new osg::PolygonOffset(1.0,1.0);
		geom->getOrCreateStateSet()->setAttributeAndModes(poffset,osg::StateAttribute::ON);
	}
	pActor->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
	return pActor;
}
// NI,NJ方向切面
osg::Node* CGlbGlobeGridObject::buildNINJClipMap(glbInt32 I,glbInt32 J)
{
	IGlbDataset* dset = GetDataset();
	IGlbSGridDataset* sdset = NULL;
	dset->QueryInterface(L"IGlbSGridDataset",(void**)&sdset);
	if (sdset == NULL)
		return NULL;
	osg::Switch* pActor = new osg::Switch();

	osg::ref_ptr<osg::Geometry> geom = new osg::Geometry;
	osg::ref_ptr<osg::Vec3Array> vtArray = new osg::Vec3Array;	
	osg::ref_ptr<osg::Vec3Array> norArray = new osg::Vec3Array;
	osg::ref_ptr<osg::Vec4Array> clrArray = new osg::Vec4Array;

	geom->setVertexArray(vtArray);
	geom->setNormalArray(norArray);
	geom->setNormalBinding(osg::Geometry::AttributeBinding::BIND_PER_VERTEX);
	geom->setColorArray(clrArray);
	geom->setColorBinding(osg::Geometry::AttributeBinding::BIND_PER_VERTEX);

	GlbGridRenderInfo *renderInfo = dynamic_cast<GlbGridRenderInfo *>(mpr_renderInfo.get());
	glbInt32 opacity = 100;
	if (renderInfo)
		opacity = renderInfo->gridOpacity;

	//------------------------------
	glbInt32 NI,NJ,NK;
	NI = sdset->get_NI();
	NJ = sdset->get_NJ();
	NK = sdset->get_NK(); 

	glbInt32 color;
	cwSGridCell gridCell;
	glbBool isvalid;

	std::vector<cwSGridCell> cellVec;
	std::vector<long> cellIndexVec;

	if(I != -1)
	{
		for(long j = 0; j < NJ; j++)
			for(long k = 0; k < NK; k++)
			{
				sdset->GetCell(I,j,k,&gridCell, &isvalid);
				if( isvalid == false)
					continue;
				cellVec.push_back(gridCell);
				//cellIndexVec.push_back(k*NJ*NI + j*NI + ii);
				cellIndexVec.push_back(j*NI*NK + I*NK +k);
			}
	}
	if(J != -1)
	{
		for(long i = 0; i < NI; i++)
			for(long k = 0; k < NK; k++)
			{
				sdset->GetCell(i,J,k,&gridCell, &isvalid);
				if( isvalid == false)
					continue;
				cellVec.push_back(gridCell);
				//cellIndexVec.push_back(k*NJ*NI + jj*NI + i);
				cellIndexVec.push_back(J*NI*NK + i*NK +k);
			}
	}

	// 边界线		
	osg::ref_ptr<osg::Geometry> geomEdge;	
	osg::ref_ptr<osg::Vec4Array> clrEdgeArray;
	osg::ref_ptr<osg::Vec3Array> vtEdgeArray;	
	if(renderInfo && renderInfo->isShowEdge)
	{
		// 边界线		
		geomEdge = new osg::Geometry;	
		clrEdgeArray = new osg::Vec4Array;
		vtEdgeArray = new osg::Vec3Array;	
		geomEdge->setVertexArray(vtEdgeArray);
		geomEdge->setNormalBinding(osg::Geometry::AttributeBinding::BIND_OVERALL);
		geomEdge->setColorArray(clrEdgeArray);
		clrEdgeArray->push_back(osg::Vec4(0,0,0,0.2));
		geomEdge->setColorBinding(osg::Geometry::AttributeBinding::BIND_OVERALL);
	}
	for(long idx = 0; idx < cellIndexVec.size(); idx++)
	{
		gridCell = cellVec[idx];
		osg::Vec4 clr(1,1,1,1);
		sdset->GetCellColorByIndex(cellIndexVec[idx],&color,&isvalid);
		if (isvalid)
		{
			glbInt32 red = LOBYTE((color)>>16);//GetBValue(color);
			glbInt32 green = LOBYTE(((color & 0xFFFF)) >> 8);//GetGValue(color);
			glbInt32 blue = LOBYTE(color);//GetRValue(color);
			glbInt32 alpha = opacity;//LOBYTE((color)>>24);//GetAValue(color);
			clr = osg::Vec4(red / 255.0,green / 255.0,blue / 255.0,alpha / 100.0);
		}
		osg::ref_ptr<osg::UIntArray> pIndexArray = new osg::UIntArray;
		pIndexArray->push_back(4);	pIndexArray->push_back(5);	pIndexArray->push_back(6);	pIndexArray->push_back(7);
		pIndexArray->push_back(0);	pIndexArray->push_back(4);	pIndexArray->push_back(7);	pIndexArray->push_back(3);
		pIndexArray->push_back(0);	pIndexArray->push_back(1);	pIndexArray->push_back(5);	pIndexArray->push_back(4);
		pIndexArray->push_back(0);	pIndexArray->push_back(3);	pIndexArray->push_back(2);	pIndexArray->push_back(1);
		pIndexArray->push_back(1);	pIndexArray->push_back(2);	pIndexArray->push_back(6);	pIndexArray->push_back(5);
		pIndexArray->push_back(2);	pIndexArray->push_back(3);	pIndexArray->push_back(7);	pIndexArray->push_back(6);
		GlbPoint3D dpt,dpt2;
		for (UINT idx = 0; idx < 6; idx++)	
		{

			for (UINT idy = 0; idy < 4; idy++)
			{
				if(idx == 0)
					norArray->push_back(osg::Vec3(0,0,1));
				else if(idx == 1)
					norArray->push_back(osg::Vec3(-1,0,0));
				else if(idx == 2)
					norArray->push_back(osg::Vec3(0,-1,0));
				else if(idx == 3)
					norArray->push_back(osg::Vec3(0,0,-1));
				else if(idx == 4)
					norArray->push_back(osg::Vec3(1,0,0));
				else if(idx == 5)
					norArray->push_back(osg::Vec3(0,1,0));
				dpt.X = gridCell.Node[pIndexArray->at(idx*4 + idy)].X;
				dpt.Y = gridCell.Node[pIndexArray->at(idx*4 + idy)].Y;
				dpt.Z = gridCell.Node[pIndexArray->at(idx*4 + idy)].Z;
				//pDis->GeometryToLocal(&dpt.X,&dpt.Y,&dpt.Z);
				vtArray->push_back(osg::Vec3(dpt.X,dpt.Y,dpt.Z) /*- originVec*/);
				if(idy == 0)
					dpt2 = dpt;
				clrArray->push_back(clr);
				if(renderInfo && renderInfo->isShowEdge)
				{
					if(idy != 0 && idy != 3)
						vtEdgeArray->push_back(osg::Vec3(dpt.X,dpt.Y,dpt.Z));
					vtEdgeArray->push_back(osg::Vec3(dpt.X,dpt.Y,dpt.Z));
					if(idy == 3)
					{
						vtEdgeArray->push_back(osg::Vec3(dpt2.X,dpt2.Y,dpt2.Z));
					}
				}

			}

		}
	}
	geom->addPrimitiveSet(new osg::DrawArrays(GL_QUADS,0, vtArray->size()));
	//需要设置一定的PolygonOffset,让其更靠近视点,避免与其它面重叠
	osg::ref_ptr<osg::PolygonOffset> poffset = new osg::PolygonOffset(1.0,1.0);
	geom->getOrCreateStateSet()->setAttributeAndModes(poffset,osg::StateAttribute::ON);

	osg::ref_ptr<osg::Geode> geode = new osg::Geode;
	geode->addDrawable(geom);

	pActor->addChild(geode);

	if(renderInfo && renderInfo->isShowEdge)
	{
		geomEdge->addPrimitiveSet(new osg::DrawArrays(GL_LINES,0, vtEdgeArray->size()));
		geomEdge->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
		geomEdge->getOrCreateStateSet()->setMode(GL_BLEND, osg::StateAttribute::ON);
		geode->addDrawable(geomEdge);
	}
	pActor->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
	return pActor;
}

bool CGlbGlobeGridObject::cellCrossPlane( const cwSGridCell& cell, const GlbPoint3D& planep1, const GlbPoint3D& planep2 )
{
	double A,B,C;

	if(planep1.X == planep2.X)
	{
		A = 1;
		B = 0;
		C = -planep1.X;
	}
	else
	{
		A = (planep2.Y - planep1.Y)/(planep2.X - planep1.X);
		B = -1;
		C = planep2.Y - A*planep2.X;
	}
	double d1 = A*cell.Node[0].X + B*cell.Node[0].Y + C;
	double d2 = A*cell.Node[1].X + B*cell.Node[1].Y + C;
	double d3 = A*cell.Node[2].X + B*cell.Node[2].Y + C;
	double d4 = A*cell.Node[3].X + B*cell.Node[3].Y + C;
	double d5 = A*cell.Node[4].X + B*cell.Node[4].Y + C;
	double d6 = A*cell.Node[5].X + B*cell.Node[5].Y + C;
	double d7 = A*cell.Node[6].X + B*cell.Node[6].Y + C;
	double d8 = A*cell.Node[7].X + B*cell.Node[7].Y + C;
	if(d1 >0 && d2 >0 && d3 >0 && d4 >0 && d5 >0 && d6 >0 && d7 >0 && d8 >0)
		return false;
	if(d1 <0 && d2 <0 && d3 <0 && d4 <0 && d5 <0 && d6 <0 && d7 <0 && d8 <0)
		return false;
	return true;
}

bool CGlbGlobeGridObject::segmentsIntr( const GlbPoint3D& a, const GlbPoint3D& b, const GlbPoint3D& c, const GlbPoint3D& d, GlbPoint3D& intrP)
{
	/** 1 解线性方程组, 求线段交点. **/  
	// 如果分母为0 则平行或共线, 不相交  
	double denominator = (b.Y - a.Y)*(d.X - c.X) - (a.X - b.X)*(c.Y - d.Y);  
	if (denominator==0) 
	{  
		return false;  
	}  
	// 线段所在直线的交点坐标 (x , y)      
	intrP.X = ( (b.X - a.X) * (d.X - c.X) * (c.Y - a.Y)   
		+ (b.Y - a.Y) * (d.X - c.X) * a.X   
		- (d.Y - c.Y) * (b.X - a.X) * c.X ) / denominator;  
	intrP.Y = -( (b.Y - a.Y) * (d.Y - c.Y) * (c.X - a.X)   
		+ (b.X - a.X) * (d.Y - c.Y) * a.Y   
		- (d.X - c.X) * (b.Y - a.Y) * c.Y ) / denominator;  

	/** 2 判断交点是否在两条线段上 **/  
	if (  
		// 交点在线段1上  
		(intrP.X - a.X) * (intrP.X - b.X) <= 0 && (intrP.Y - a.Y) * (intrP.Y - b.Y) <= 0  
		// 且交点也在线段2上  
		&& (intrP.X - c.X) * (intrP.X - d.X) <= 0 && (intrP.Y - c.Y) * (intrP.Y - d.Y) <= 0  
		)
	{ 
		double bili;
		if(a.X != b.X)
		{
			bili = (intrP.X - a.X)/(b.X - a.X);
		}
		else
		{
			bili = (intrP.Y - a.Y)/(b.Y - a.Y);
		}
		intrP.Z = a.Z + (b.Z - a.Z)*bili;

		return true;
	}  
	//否则不相交  
	return false ;
}

osg::Node* CGlbGlobeGridObject::buildTempLargeCopyGrid()
{// 生成2*2排列的4倍大小的格网数据 2018.8.4
	IGlbSGridDataset* sdset = NULL;
	mpt_dataset->QueryInterface(L"IGlbSGridDataset",(void**)&sdset);
	if (sdset == NULL)
		return NULL;

	GlbGridRenderInfo *renderInfo = dynamic_cast<GlbGridRenderInfo *>(mpr_renderInfo.get());
	if (renderInfo == NULL)
		return NULL;

	glbInt32 opacity = 100;
	if (renderInfo)
		opacity = renderInfo->gridOpacity;

	cwSGridFacet* pSGridIndex = NULL;
	glbInt32 cnt; // index 长度
	sdset->GetBoundCells(&pSGridIndex,&cnt);
	if( cnt == 0 )return NULL;

	if (mpr_faceGeode) mpr_faceGeode = NULL;
	if (mpr_edgeGeode) mpr_edgeGeode = NULL;

	IGlbGeoDataset* gdset = NULL;
	mpt_dataset->QueryInterface(L"IGlbGeoDataset",(void**)&gdset);
	const CGlbExtent* pB = gdset->GetExtent();
	glbDouble cx,cy,cz;
	pB->GetCenter(&cx,&cy,&cz);
	glbDouble xx = pB->GetXWidth();
	glbDouble yy = pB->GetYHeight();

	osg::Matrix m[4];
	osg::MatrixTransform* mt0 = new osg::MatrixTransform();
	m[0].setTrans(osg::Vec3d(-xx*0.5,-yy*0.5,0));
	mt0->setMatrix(m[0]);
	osg::MatrixTransform* mt1 = new osg::MatrixTransform();
	m[1].setTrans(osg::Vec3d(xx*0.5,-yy*0.5,0));
	mt1->setMatrix(m[1]);
	osg::MatrixTransform* mt2 = new osg::MatrixTransform();
	m[2].setTrans(osg::Vec3d(-xx*0.5,yy*0.5,0));
	mt2->setMatrix(m[2]);
	osg::MatrixTransform* mt3 = new osg::MatrixTransform();
	m[3].setTrans(osg::Vec3d(xx*0.5,yy*0.5,0));
	mt3->setMatrix(m[3]);

	//// 偏移原点
	osg::Vec3 originVec(cx,cy,cz);
	//第二种渲染方式： 一个Geometry，一个批次全部渲染出来，使用顶点颜色和顶点法向
	osg::ref_ptr<osg::Geometry> geom = new osg::Geometry;	
	osg::ref_ptr<osg::Vec3Array> vtArray = new osg::Vec3Array;	
	osg::ref_ptr<osg::Vec3Array> norArray = new osg::Vec3Array;
	osg::ref_ptr<osg::Vec4Array> clrArray = new osg::Vec4Array;

	geom->setVertexArray(vtArray);
	geom->setNormalArray(norArray);
	geom->setNormalBinding(osg::Geometry::AttributeBinding::BIND_PER_VERTEX);
	geom->setColorArray(clrArray);
	geom->setColorBinding(osg::Geometry::AttributeBinding::BIND_PER_VERTEX);

	cwSGridCell gridCell;
	double nTransparency=0.0;	//透明度
	//glbInt32 NI,NJ,NK;
	//NI = sdset->get_NI();
	//NJ = sdset->get_NJ();
	//NK = sdset->get_NK();
	glbBool isvalid;
	glbInt32 color;	

	for(glbInt32 idx = 0; idx < cnt; idx++)
	{			
		sdset->GetCellByIndex(pSGridIndex[idx].CellIndex,&gridCell, &isvalid);

		if( isvalid == false)
			continue;

		BYTE orientation = pSGridIndex[idx].Orientation;

		sdset->GetCellColorByIndex(pSGridIndex[idx].CellIndex,&color,&isvalid);
		osg::Vec4 clr(1,1,1,1);
		if (isvalid)
		{
			glbInt32 blue = LOBYTE((color)>>16);//GetBValue(color);
			glbInt32 green = LOBYTE(((color & 0xFFFF)) >> 8);//GetGValue(color);
			glbInt32 red = LOBYTE(color);//GetRValue(color);
			glbInt32 alpha = opacity;//LOBYTE((color)>>24);//GetAValue(color);
			clr = osg::Vec4(red / 255.0,green / 255.0,blue / 255.0,alpha / 100.0);
		}		

		//// 0-3-2-1  1-2-6-5 2-3-7-6 0-4-7-3 0-1-5-4 4-5-6-7
		osg::ref_ptr<osg::UIntArray> pIndexArray = new osg::UIntArray;	
		switch(orientation)
		{
		case 2:
			{
				pIndexArray->push_back(4);	pIndexArray->push_back(5);	pIndexArray->push_back(6);	pIndexArray->push_back(7);
				for (int kk = 0; kk < 4; kk++)
					norArray->push_back(osg::Vec3(0,0,1));
			}			
			break;
		case 1:
			{
				pIndexArray->push_back(0);	pIndexArray->push_back(3);	pIndexArray->push_back(2);	pIndexArray->push_back(1);
				for (int kk = 0; kk < 4; kk++)
					norArray->push_back(osg::Vec3(0,0,-1));
			}
			break;
		case 3:
			{
				pIndexArray->push_back(0);	pIndexArray->push_back(4);	pIndexArray->push_back(7);	pIndexArray->push_back(3);
				for (int kk = 0; kk < 4; kk++)
					norArray->push_back(osg::Vec3(-1,0,0));
			}
			break;
		case 4:
			{
				pIndexArray->push_back(1);	pIndexArray->push_back(2);	pIndexArray->push_back(6);	pIndexArray->push_back(5);
				for (int kk = 0; kk < 4; kk++)
					norArray->push_back(osg::Vec3(1,0,0));
			}
			break;
		case 5:
			{
				pIndexArray->push_back(0);	pIndexArray->push_back(1);	pIndexArray->push_back(5);	pIndexArray->push_back(4);
				for (int kk = 0; kk < 4; kk++)
					norArray->push_back(osg::Vec3(0,-1,0));
			}
			break;
		case 6:
			{
				pIndexArray->push_back(2);	pIndexArray->push_back(3);	pIndexArray->push_back(7);	pIndexArray->push_back(6);
				for (int kk = 0; kk < 4; kk++)
					norArray->push_back(osg::Vec3(0,1,0));
			}
			break;
		}

		//cwVector3D dpt;
		for (UINT kk = 0; kk < pIndexArray->size(); kk++)
		{			
			//pSGrid->GetVertex(pIndexArray->at(kk),&vtIndex);
			double xx = gridCell.Node[pIndexArray->at(kk)].X;
			double yy = gridCell.Node[pIndexArray->at(kk)].Y;
			double zz = gridCell.Node[pIndexArray->at(kk)].Z;
			vtArray->push_back(osg::Vec3(xx,yy,zz) /*- originVec*/);

			//char szMsg[1024];
			//fprintf(fp, "%f,%f,%f,%f,%f,%f\n", clr.r(), clr.g(), clr.b(), xx, yy, zz);
			//MessageBoxA(NULL, szMsg, "MSG", MB_OK);
			clrArray->push_back(clr);
		}
	}

	geom->addPrimitiveSet(new osg::DrawArrays(GL_QUADS,0, vtArray->size()));
	//需要设置一定的PolygonOffset,让其更靠近视点,避免与其它面重叠
	osg::ref_ptr<osg::PolygonOffset> poffset = new osg::PolygonOffset(1.0,1.0);
	geom->getOrCreateStateSet()->setAttributeAndModes(poffset,osg::StateAttribute::ON);	

	geom->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);

	osg::ref_ptr<osg::Geode> faceGeode = new osg::Geode;
	faceGeode->addDrawable(geom);
	faceGeode->setName("gridFace");
	mpr_faceGeode = faceGeode;

	osg::ref_ptr<osg::Geode> edgeGeode = new osg::Geode;
	{// 边界线		
		osg::ref_ptr<osg::Geometry> geomEdge = new osg::Geometry;	
		geomEdge->setName("gridEdge");
		//osg::ref_ptr<osg::Vec3Array> vtArray = new osg::Vec3Array;
		//vtArray->reserve(8*cnt);
		osg::ref_ptr<osg::Vec4Array> clrArray = new osg::Vec4Array;

		geomEdge->setVertexArray(vtArray);
		geomEdge->setNormalBinding(osg::Geometry::AttributeBinding::BIND_PER_VERTEX);
		geomEdge->setColorArray(clrArray);
		clrArray->push_back(osg::Vec4(0.8,0,0,0.2));
		geomEdge->setColorBinding(osg::Geometry::AttributeBinding::BIND_OVERALL);

		osg::ref_ptr<osg::UIntArray> pIndexArray2 = new osg::UIntArray;		

		long cellcount = 0;
		for(long idx = 0; idx < cnt; idx++)
		{			
			sdset->GetCellByIndex(pSGridIndex[idx].CellIndex,&gridCell, &isvalid);	

			if( isvalid == false)
				continue;			

			BYTE orientation = pSGridIndex[idx].Orientation;
			if (orientation < 1 || orientation > 6)
				continue;

			int base = 4*idx;	
			for (int kk = 0; kk < 3; kk++)
			{
				pIndexArray2->push_back(base+kk);
				pIndexArray2->push_back(base+kk+1);
			}
			pIndexArray2->push_back(base+3);
			pIndexArray2->push_back(base);
		}
		geomEdge->addPrimitiveSet(new osg::DrawElementsUInt(GL_LINES,pIndexArray2->size(),&pIndexArray2->front()));

		geomEdge->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
		geomEdge->getOrCreateStateSet()->setRenderingHint(osg::StateSet::RenderingHint::TRANSPARENT_BIN);
		geomEdge->getOrCreateStateSet()->setMode(GL_BLEND, osg::StateAttribute::ON);

		edgeGeode->addDrawable(geomEdge);

		edgeGeode->setName("edgeFace");
		if (renderInfo->isShowEdge == false)
			edgeGeode->setNodeMask(0x0);

		mpr_edgeGeode = edgeGeode;
	}

	osg::ref_ptr<osg::Group> grp = new osg::Group;
	grp->addChild(faceGeode);
	grp->addChild(edgeGeode);

	mt0->addChild(grp);
	mt1->addChild(grp);
	mt2->addChild(grp);
	mt3->addChild(grp);

	osg::ref_ptr<osg::Switch> sw = new osg::Switch;
	sw->addChild(mt0);
	sw->addChild(mt1);
	sw->addChild(mt2);
	sw->addChild(mt3);

	return sw.release();
}