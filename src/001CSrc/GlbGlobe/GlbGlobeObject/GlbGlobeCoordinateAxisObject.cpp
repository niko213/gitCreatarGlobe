#include "StdAfx.h"
#include "GlbGlobeCoordinateAxisObject.h"
#include "CGlbGlobe.h"
#include "GlbGlobeView.h"
#include "GlbGlobeTypes.h"
#include "GlbGlobeMemCtrl.h"
#include "GlbCalculateNodeSizeVisitor.h"
#include "GlbColorVisitor.h"
#include "GlbFadeInOutCallback.h"
#include "GlbGlobeObjectRenderInfos.h"
#include "GlbGlobeCoordinateAxisSymbol.h"

using namespace GlbGlobe;

// CGlbGlobeCoordinateAxisObject 接口实现
CGlbGlobeCoordinateAxisObject::CGlbGlobeCoordinateAxisTask::CGlbGlobeCoordinateAxisTask( CGlbGlobeCoordinateAxisObject *obj, glbInt32 level )
{
	mpr_obj = obj;	
	mpr_level = level;
}

CGlbGlobeCoordinateAxisObject::CGlbGlobeCoordinateAxisTask::~CGlbGlobeCoordinateAxisTask( )
{
	mpr_obj = NULL;
}

glbBool CGlbGlobeCoordinateAxisObject::CGlbGlobeCoordinateAxisTask::doRequest()
{
	if(mpr_obj)
	{
		mpr_obj->ReadData(mpr_level);
		return true;
	}
	return false;
}

CGlbGlobeObject * CGlbGlobeCoordinateAxisObject::CGlbGlobeCoordinateAxisTask::getObject()
{
	return mpr_obj.get();
}

//////////////////////////////////////////////////////////////////////////
CGlbGlobeCoordinateAxisObject::CGlbGlobeCoordinateAxisObject(void)
{	
	mpr_readData_critical.SetName(L"coord_axis_readdata");
	mpr_addToScene_critical.SetName(L"coord_axis_addscene");
	mpt_altitudeMode = GLB_ALTITUDEMODE_ABSOLUTE; //坐标轴都是绝对坐标方式
	mpr_needReReadData = false;
}

CGlbGlobeCoordinateAxisObject::~CGlbGlobeCoordinateAxisObject(void)
{
	mpr_outline = NULL;
	mpr_renderInfo = NULL;
}

GlbGlobeObjectTypeEnum CGlbGlobeCoordinateAxisObject::GetType()
{
	return GLB_OBJECTTYPE_COORDINAT_AXIS;
}

glbBool CGlbGlobeCoordinateAxisObject::Load(xmlNodePtr* node,const glbWChar* prjPath)
{
	CGlbGlobeRObject::Load(node,prjPath);

	xmlNodePtr pnode = *node;
	xmlChar *szKey = NULL;
	if(pnode && (!xmlStrcmp(pnode->name,(const xmlChar *)"RenderInfo")))
	{
		GlbGlobeObjectTypeEnum rdtype;
		GlbGlobeLineSymbolTypeEnum symtype;
		xmlNodePtr rdchild = pnode->xmlChildrenNode;

		if(rdchild && (!xmlStrcmp(rdchild->name, (const xmlChar *)"RenderType")))
		{
			szKey = xmlNodeGetContent(rdchild);
			sscanf((char*)szKey,"%d",&rdtype);	
			xmlFree(szKey);

			rdchild = rdchild->next;
		}

		GlbCoordinateAxisSymbolInfo *axisInfo = new GlbCoordinateAxisSymbolInfo();
		axisInfo->Load(rdchild,prjPath);
		mpr_renderInfo = (GlbRenderInfo*)axisInfo;
	}
	return true;
}

glbBool CGlbGlobeCoordinateAxisObject::Load2(xmlNodePtr* node,const glbWChar* prjPath)
{
	return false;
}

glbBool CGlbGlobeCoordinateAxisObject::Save(xmlNodePtr node,const glbWChar* prjPath)
{
	CGlbGlobeRObject::Save(node,prjPath);

	char str[32];
	xmlNodePtr rdnode = xmlNewNode(NULL,BAD_CAST "RenderInfo");
	xmlAddChild(node,rdnode);
	if(mpr_renderInfo)
	{
		sprintf_s(str,"%d",mpr_renderInfo->type);
		xmlNewTextChild(rdnode,NULL,BAD_CAST "RenderType",BAD_CAST str);

		GlbCoordinateAxisSymbolInfo *axisInfo = (GlbCoordinateAxisSymbolInfo *)mpr_renderInfo.get();
		if (axisInfo)
		{
			axisInfo->Save(rdnode,prjPath);
		}		
	}
	return true;
}

glbDouble CGlbGlobeCoordinateAxisObject::GetDistance(osg::Vec3d &cameraPos,glbBool isCompute)
{
	if(isCompute == false)
		return mpr_distance;

	if (mpt_isCameraAltitudeAsDistance)	
	{
		mpr_distance = cameraPos.z();
	}	
	else
	{
		if (mpt_globe==NULL || mpr_renderInfo==NULL)
			return DBL_MAX;		
		GlbCoordinateAxisSymbolInfo *axisInfo = dynamic_cast<GlbCoordinateAxisSymbolInfo *>(mpr_renderInfo.get());

		glbDouble xOrLon,yOrLat,zOrAlt;		
		xOrLon = axisInfo->originX;
		yOrLat = axisInfo->originY;
		zOrAlt = axisInfo->originZ;
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

glbBool CGlbGlobeCoordinateAxisObject::SetAltitudeMode(GlbAltitudeModeEnum mode,glbBool &isNeedDirectDraw)
{
	isNeedDirectDraw = false;
	if (mode != GLB_ALTITUDEMODE_ABSOLUTE)
		return false;
	return true;
}

/*
*   mpt_node、mpt_preNode 生成、改为NULL 在不同线程
*   LoadData 只有 Dispatcher 线程调用
*   
*   修改renderinfo,position 会在其他线程
*/
 void CGlbGlobeCoordinateAxisObject::LoadData(glbInt32 level)
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
	glbref_ptr<CGlbGlobeCoordinateAxisTask> task = new CGlbGlobeCoordinateAxisTask(this,level);
	mpt_globe->mpr_taskmanager->AddTask(task.get());
 }

 void CGlbGlobeCoordinateAxisObject::ReadData(glbInt32 level,glbBool isDirect)
 {
	glbref_ptr<GlbCoordinateAxisSymbolInfo> renderInfo = dynamic_cast<GlbCoordinateAxisSymbolInfo*>(mpr_renderInfo.get());
	if( renderInfo == NULL || mpt_altitudeMode != GLB_ALTITUDEMODE_ABSOLUTE )
	{mpt_loadState=false;return;}

	if (renderInfo == NULL) {mpt_loadState=false;return;}
	CGlbGlobeCoordinateAxisSymbol symbol;
	osg::ref_ptr<osg::Node> node = symbol.Draw(this,NULL);
	if (node==NULL){mpt_loadState=false;return;}

	osg::ref_ptr<osg::Switch> swiNode = node->asSwitch();
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

 /*
 *     Dispatcher->AddToScene()->CGlbGlobePoint::AddToScene()  //频繁调用
 *     外部线程-->DirectDraw()->AddToScene() //频繁调用
 */
void CGlbGlobeCoordinateAxisObject::AddToScene()
{
	if(mpt_node == NULL)return;
	if(mpt_isFaded)
	{
		osg::ref_ptr<CGlbFadeInOutCallback> fadeCb = 
			new CGlbFadeInOutCallback(mpt_fadeStartAlpha,mpt_fadeEndAlpha,this,mpt_fadeDurationTime,mpt_fadeColor);
		mpt_node->addUpdateCallback(fadeCb.get());
	}
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
		//UpdateWorldExtent();
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
/*
*   update线程 --->Dispatcher--->RemoveFromScene(true)         从内存中卸载对象
*                      |
*                      |---->CallBack->RemoveFromeScene(false) 从显存中卸载对象
*
*   只有update线程会 调用该方法，所以不需要【加锁】
*/
glbInt32 CGlbGlobeCoordinateAxisObject::RemoveFromScene(glbBool isClean)
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
void CGlbGlobeCoordinateAxisObject::DirectDraw(glbInt32 level)
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

glbref_ptr<CGlbExtent> CGlbGlobeCoordinateAxisObject::GetBound(glbBool isWorld)
{
	if (mpt_globe == NULL && isWorld) return NULL;

	if (mpr_renderInfo==NULL) return NULL;

	GlbCoordinateAxisSymbolInfo *axisInfo = dynamic_cast<GlbCoordinateAxisSymbolInfo *>(mpr_renderInfo.get());
	osg::Vec3d	origin(axisInfo->originX,axisInfo->originY,axisInfo->originZ);					// 坐标轴原点
	osg::Vec3d	axisLen(axisInfo->axisLengthX,axisInfo->axisLengthY,axisInfo->axisLengthZ);		// x,y,z三个轴的长度

	glbref_ptr<CGlbExtent> geoExt = new CGlbExtent;

	GlbGlobeTypeEnum globeType = mpt_globe->GetType();
	if (globeType!=GLB_GLOBETYPE_GLOBE)	
	{
		geoExt->Merge(origin.x(),origin.y(),origin.z());
		geoExt->Merge(origin.x()+axisLen.x(),origin.y()+axisLen.y(),axisInfo->bInvertZ ? (origin.z()-axisLen.z()) : (origin.z()+axisLen.z()));
	}
	else
		geoExt->Merge(origin.x(),origin.y(),origin.z());

	if (isWorld == false)
	{//地理坐标
		return geoExt;	
	}
	else // 世界坐标
	{// 非贴地形
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
			
			if (globeType==GLB_GLOBETYPE_GLOBE)
			{
				double x,y,z;
				x = origin.x(); y = origin.y(); z = origin.z();	
				g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(y),osg::DegreesToRadians(x),z,x,y,z);	
				worldExt->Merge(x,y,z);			
					
				worldExt->Merge(x+axisLen.x(),y+axisLen.y(),z+axisLen.z());				
			}
			else if (globeType==GLB_GLOBETYPE_FLAT)		
				worldExt = geoExt;	
		}
		return worldExt;
	}
	return NULL;
}

IGlbGeometry *CGlbGlobeCoordinateAxisObject::GetOutLine()
{
	return NULL;
}

glbBool CGlbGlobeCoordinateAxisObject::SetRenderInfo(GlbRenderInfo *renderInfo,glbBool &isNeedDirectDraw)
{
	isNeedDirectDraw = false;
	if (renderInfo == NULL || renderInfo->type != GLB_OBJECTTYPE_COORDINAT_AXIS)  return false;
	GlbCoordinateAxisSymbolInfo *axisInfo = dynamic_cast<GlbCoordinateAxisSymbolInfo *>(renderInfo);
	if (axisInfo == NULL) return false;		
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
	rt = DealCoordinateAxisRenderInfo(axisInfo,isNeedDirectDraw);
	if (rt) mpr_renderInfo   = renderInfo;
	mpt_loadState    = false;
	return rt;	
}

GlbRenderInfo *CGlbGlobeCoordinateAxisObject::GetRenderInfo()
{
	return mpr_renderInfo.get();
}

glbBool CGlbGlobeCoordinateAxisObject::SetShow(glbBool isShow,glbBool isOnState)
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

	if (mpt_node)
	{
		if(isnew)
		{
			mpt_node->asSwitch()->setAllChildrenOn();
		}
		else
			mpt_node->asSwitch()->setAllChildrenOff();
	}

	return true;
}

glbBool CGlbGlobeCoordinateAxisObject::SetSelected(glbBool isSelected)
{
	if(mpt_isSelected == isSelected) return true;
	mpt_isSelected = isSelected;
	if (mpt_node != NULL && mpt_altitudeMode!=GLB_ALTITUDEMODE_ONTERRAIN)
	{
		DealSelected();
	}
	return true;
}

glbBool CGlbGlobeCoordinateAxisObject::SetBlink(glbBool isBlink)
{
	if (mpt_isBlink == isBlink) return true;
	mpt_isBlink = isBlink;
	if (mpt_node != NULL && mpt_altitudeMode!=GLB_ALTITUDEMODE_ONTERRAIN)
		DealBlink();
	return true;
}

glbInt32 CGlbGlobeCoordinateAxisObject::GetOsgNodeSize()
{
	if (mpt_node == NULL) return 0;
	if (mpr_objSize==0)
		mpr_objSize = ComputeNodeSize(mpt_node);
	return mpr_objSize;
}

glbInt32 CGlbGlobeCoordinateAxisObject::ComputeNodeSize(osg::Node *node)
{
	CGlbCalculateNodeSizeVisitor cnsv;
	node->accept(cnsv);
	return cnsv.getTextureSize() + cnsv.getNodeMemSize();
}

void CGlbGlobeCoordinateAxisObject::DealSelected()
{

}
void CGlbGlobeCoordinateAxisObject::DealBlink()
{

}

bool CGlbGlobeCoordinateAxisObject::DealCoordinateAxisRenderInfo(GlbCoordinateAxisSymbolInfo *renderInfo,glbBool &isNeedDirectDraw)
{
	GlbCoordinateAxisSymbolInfo *tempInfo = dynamic_cast<GlbCoordinateAxisSymbolInfo *>(mpr_renderInfo.get());
	if (tempInfo == NULL) return false;

	if (tempInfo->bShowGrid != renderInfo->bShowGrid)
	{
		DealCoordinateAxisShowGrid(renderInfo->bShowGrid);
		isNeedDirectDraw = false;
		return true;
	}

	if (tempInfo->bShowLabel != renderInfo->bShowLabel)
	{
		DealCoordinateAxisShowLabel(renderInfo->bShowLabel);
		isNeedDirectDraw = false;
		return true;
	}

	if (tempInfo->xOffset != renderInfo->xOffset || 
		tempInfo->yOffset != renderInfo->yOffset || 
		tempInfo->zOffset != renderInfo->zOffset )
	{
		osg::Vec3d orign(renderInfo->originX,renderInfo->originY,renderInfo->originZ);
		osg::Vec3d offset(renderInfo->xOffset,renderInfo->yOffset,renderInfo->zOffset);
		DealCoordinateAxisOffset(orign,offset);
		isNeedDirectDraw = false;
		return true;
	}

	isNeedDirectDraw = true;
	return true;
}

void CGlbGlobeCoordinateAxisObject::DealCoordinateAxisShowGrid(glbBool bShowGrid)
{
	if (mpt_node)
	{// 找到grids节点，设为不可见。
		osg::Group* grp = mpt_node->asSwitch()->getChild(0)->asGroup();
		if (grp)
		{
			for (size_t k = 0; k < grp->getNumChildren(); k++)
			{
				if ("AxisGrid" == grp->getChild(k)->getName())
				{	
					if (bShowGrid)
						grp->getChild(k)->setNodeMask(0xffffffff);
					else
						grp->getChild(k)->setNodeMask(0x0);

					break;
				}
			}
		}
	}
}

void CGlbGlobeCoordinateAxisObject::DealCoordinateAxisShowLabel(glbBool bShowLabel)
{
	if (mpt_node)
	{// 找到grids节点，设为不可见。
		osg::Group* grp = mpt_node->asSwitch()->getChild(0)->asGroup();
		if (grp)
		{
			for (size_t k = 0; k < grp->getNumChildren(); k++)
			{
				if ("AxisLabel" == grp->getChild(k)->getName())
				{	
					if (bShowLabel)
						grp->getChild(k)->setNodeMask(0xffffffff);
					else
						grp->getChild(k)->setNodeMask(0x0);

					break;
				}
			}
		}
	}
}

void CGlbGlobeCoordinateAxisObject::DealCoordinateAxisOffset(osg::Vec3d origin, osg::Vec3d offset)
{
	if (mpt_node && mpt_node->asGroup()->getNumChildren()>0)
	{// 找到grids节点，设为不可见。
		osg::MatrixTransform* mtNode = dynamic_cast<osg::MatrixTransform*>(mpt_node->asSwitch()->getChild(0));
		if (mtNode)
		{
			GlbGlobeTypeEnum globeType = GetGlobe()->GetType();
			glbDouble x,y,z;
			x = origin.x();
			y = origin.y();
			z = origin.z();
			osg::Matrixd localToWorld;
			if (globeType==GLB_GLOBETYPE_GLOBE)		
				g_ellipsoidModel->computeLocalToWorldTransformFromLatLongHeight(osg::DegreesToRadians(y),osg::DegreesToRadians(x),z,localToWorld);
			else	
				localToWorld.makeTranslate(x,y,z);	
			
			osg::Matrixd mOffset;
			{
				double xOffset = offset.x();
				double yOffset = offset.y();
				double zOffset = offset.z();

				if (globeType==GLB_GLOBETYPE_GLOBE)		
				{
					double longitude = osg::DegreesToRadians(origin.x());
					double latitude = osg::DegreesToRadians(origin.y());
					// Compute up vector
					osg::Vec3d    up      ( cos(longitude)*cos(latitude), sin(longitude)*cos(latitude), sin(latitude));
					// Compute east vector
					osg::Vec3d    east    (-sin(longitude), cos(longitude), 0);
					// Compute north  vector = outer product up x east
					osg::Vec3d    north   = up ^ east;

					north.normalize();
					osg::Vec3 _Offset = east*xOffset + north*yOffset + up*zOffset;
					mOffset.makeTranslate(_Offset);
				}
				else
					mOffset.makeTranslate(xOffset,yOffset,zOffset);
			}
			// 设置偏移矩阵
			mtNode->setMatrix(localToWorld * mOffset);
		}
	}
}