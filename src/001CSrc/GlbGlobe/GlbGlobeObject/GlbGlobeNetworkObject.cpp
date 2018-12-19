#include "StdAfx.h"
#include "GlbGlobeNetworkObject.h"
#include "CGlbGlobe.h"
#include "GlbGlobeView.h"
#include "GlbGlobeTypes.h"
#include "GlbPolygon.h"
#include "GlbCalculateNodeSizeVisitor.h"
#include "GlbGlobeMemCtrl.h"
#include "GlbGlobeFeatureLayer.h"
#include "GlbGlobeNetworkSymbol.h"
//#include "GlbDrill.h"
#include <osgUtil/LineSegmentIntersector>
#include <osgUtil/PolytopeIntersector>
#include <osg/Material>
#include <osg/LineWidth>
#include <osg/Point>
#include "GlbColorVisitor.h"

using namespace GlbGlobe;

class NetworkObjectCallback : public osg::NodeCallback
{
public:
	NetworkObjectCallback(CGlbGlobeRObject *obj, osg::Node* nodeNode=NULL)
	{
		mpr_isChangeFrame = false;
		mpr_markNum = 0;	
		CGlbGlobeNetworkObject *networkObject = dynamic_cast<CGlbGlobeNetworkObject *>(obj);		
		mpr_stateset = NULL;
		if (nodeNode==NULL)
		{
			if(networkObject)			
				mpr_stateset = networkObject->GetOsgNode()->getOrCreateStateSet();					
		}
		else
		{
			mpr_stateset = nodeNode->getOrCreateStateSet();
		}

		if(mpr_stateset)
		{

			osg::Material *material  = 
				dynamic_cast<osg::Material *>(mpr_stateset->getAttribute(osg::StateAttribute::MATERIAL));
			if (!material)			
				mpr_material = new osg::Material;			
			else
				mpr_material = material;
			mpr_stateset->setAttribute(mpr_material,osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
		}
	}	

	void operator()(osg::Node *node,osg::NodeVisitor *nv)
	{
		if (nv->getFrameStamp()->getFrameNumber() - mpr_markNum > 20)
		{
			mpr_markNum = nv->getFrameStamp()->getFrameNumber();
			mpr_isChangeFrame = !mpr_isChangeFrame;
		} 

		if (mpr_isChangeFrame)
		{			
			mpr_stateset->setMode(GL_LIGHTING,osg::StateAttribute::ON);

			mpr_material->setDiffuse(osg::Material::FRONT_AND_BACK,osg::Vec4(1,0,0,1));					
			mpr_stateset->setAttribute(mpr_material,osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
			mpr_stateset->setMode(GL_TEXTURE_2D,osg::StateAttribute::OFF | osg::StateAttribute::OVERRIDE);
		}
		else
		{
			osg::Material *material  = 
				dynamic_cast<osg::Material *>(mpr_stateset->getAttribute(osg::StateAttribute::MATERIAL));
			mpr_stateset->removeAttribute(material);
			mpr_stateset->setMode(GL_TEXTURE_2D,osg::StateAttribute::ON);
			mpr_stateset->setMode(GL_LIGHTING,osg::StateAttribute::OFF);
		}
	}
private:
	osg::ref_ptr<osg::Material> mpr_material;
	osg::ref_ptr<osg::StateSet> mpr_stateset;
	glbBool mpr_isChangeFrame;
	unsigned int mpr_markNum;
	osg::ref_ptr<osg::Node> stratumNode;
};


// CGlbGlobeNetworkObject interface 
CGlbGlobeNetworkObject::CGlbGlobeNetworkTask::CGlbGlobeNetworkTask( CGlbGlobeNetworkObject *obj,glbInt32 level )
{
	mpr_obj = obj;
	mpr_level = level;
}

CGlbGlobeNetworkObject::CGlbGlobeNetworkTask::~CGlbGlobeNetworkTask( )
{
	mpr_obj = NULL;
}

glbBool CGlbGlobeNetworkObject::CGlbGlobeNetworkTask::doRequest()
{
	if(mpr_obj)
	{
		mpr_obj->ReadData(mpr_level);
		return true;
	}
	return false;
}

CGlbGlobeObject * CGlbGlobeNetworkObject::CGlbGlobeNetworkTask::getObject()
{
	return mpr_obj.get();
}

CGlbGlobeNetworkObject::CGlbGlobeNetworkObject(void)
{
	mpr_readData_critical.SetName(L"network_readdata");
	mpr_addToScene_critical.SetName(L"network_addscene");
	mpt_altitudeMode = GLB_ALTITUDEMODE_ABSOLUTE; //网络对象都是绝对坐标方式
	mpr_needReReadData = false;
}


CGlbGlobeNetworkObject::~CGlbGlobeNetworkObject(void)
{
	mpr_outline = NULL;
	mpr_renderInfo = NULL;
}

GlbGlobeObjectTypeEnum CGlbGlobeNetworkObject::GetType()
{
	return GLB_OBJECTTYPE_NETWORK;
}

glbBool CGlbGlobeNetworkObject::Load(xmlNodePtr* node,glbWChar* prjPath)
{
	CGlbGlobeRObject::Load(node,prjPath);

	xmlChar* szKey = NULL;
	xmlNodePtr pnode = *node;

	if (pnode && (!xmlStrcmp(pnode->name, (const xmlChar *)"RenderInfo")))
	{
		GlbGlobeObjectTypeEnum rdtype;
		xmlNodePtr rdchild = pnode->xmlChildrenNode;

		if (rdchild && (!xmlStrcmp(rdchild->name, (const xmlChar *)"RenderType")))
		{
			szKey = xmlNodeGetContent(rdchild);
			sscanf((char*)szKey,"%d",&rdtype);	
			xmlFree(szKey);

			rdchild = rdchild->next;
		}

		GlbNetworkRenderInfo* renderInfo = new GlbNetworkRenderInfo();
		renderInfo->Load(rdchild,prjPath);
	}
	return true;
}

glbBool CGlbGlobeNetworkObject::Save(xmlNodePtr node,const glbWChar* prjPath)
{
	CGlbGlobeRObject::Save(node,prjPath);	

	xmlNodePtr rdnode = xmlNewNode(NULL,BAD_CAST "RenderInfo");
	xmlAddChild(node,rdnode);
	if(mpr_renderInfo)
	{
		char str[32];
		sprintf_s(str,"%d",mpr_renderInfo->type);
		xmlNewTextChild(rdnode, NULL, BAD_CAST "RenderType", BAD_CAST str);

		GlbNetworkRenderInfo* renderInfo = dynamic_cast<GlbNetworkRenderInfo*>(mpr_renderInfo.get());
		//if (renderInfo)		
		renderInfo->Save(rdnode,prjPath);				
	}
	return true;
}

glbDouble CGlbGlobeNetworkObject::GetDistance(osg::Vec3d &cameraPos,glbBool isCompute)
{
	if(isCompute == false)
		return mpr_distance;

	if (mpt_feature==NULL || mpt_globe==NULL)
		return DBL_MAX;

	glbref_ptr<CGlbExtent> ext = const_cast<CGlbExtent *>(mpt_feature->GetExtent());
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

glbref_ptr<CGlbExtent> CGlbGlobeNetworkObject::GetBound(glbBool isWorld)
{
	if (mpt_globe == NULL && isWorld) return NULL;

	if (mpt_feature==NULL || mpt_globe==NULL)
		return NULL;
	glbref_ptr<CGlbExtent> geoExt = const_cast<CGlbExtent *>(mpt_feature->GetExtent());
	if (geoExt==NULL)
		return NULL;

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

IGlbGeometry* CGlbGlobeNetworkObject::GetOutLine()
{
	if (mpt_feature==NULL || mpt_globe==NULL)
		return NULL;

	if (mpr_outline==NULL)
	{
		IGlbGeometry* geo = NULL;
		mpt_feature->GetGeometry(&geo);
		//glbref_ptr<CGlbExtent> geoExt = const_cast<CGlbExtent *>(mpt_feature->GetExtent());
		//if (geoExt==NULL)
		//	return NULL;
		//glbDouble cx,cy,cz;
		//geoExt->GetCenter(&cx,&cy,&cz);
		//CGlbPoint* pt = new CGlbPoint(cx,cy,cz);
		mpr_outline = geo;
	}
	return mpr_outline.get();
}

/*
*   mpt_node、mpt_preNode 生成、改为NULL 在不同线程
*   LoadData 只有 Dispatcher 线程调用
*   
*   修改renderinfo,position 会在其他线程
*/
void CGlbGlobeNetworkObject::LoadData(glbInt32 level)
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
	glbref_ptr<GlbNetworkRenderInfo> renderInfo = dynamic_cast<GlbNetworkRenderInfo *>(mpr_renderInfo.get());
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
	glbref_ptr<CGlbGlobeNetworkTask> task = new CGlbGlobeNetworkTask(this,level);
	mpt_globe->mpr_taskmanager->AddTask(task.get());
}

void CGlbGlobeNetworkObject::ReadData(glbInt32 level,glbBool isDirect)
{
	glbref_ptr<GlbNetworkRenderInfo> renderInfo = dynamic_cast<GlbNetworkRenderInfo*>(mpr_renderInfo.get());
	if( renderInfo == NULL || mpt_altitudeMode != GLB_ALTITUDEMODE_ABSOLUTE )
	{mpt_loadState=false;return;}

	if (renderInfo == NULL) {mpt_loadState=false;return;}
	if (mpt_feature == NULL){mpt_loadState=false;return;}	

	CGlbGlobeNetworkSymbol symbol;
	osg::ref_ptr<osg::Node> node = symbol.Draw(this,NULL);
	if (node==NULL){mpt_loadState=false;return;}

	//{// 测试		
	//	glbInt32 color = renderInfo->edgeColor->GetValue(mpt_feature.get());

	//	glbInt32 red = LOBYTE((color)>>16);//GetBValue(color);
	//	glbInt32 green = LOBYTE(((color & 0xFFFF)) >> 8);//GetGValue(color);
	//	glbInt32 blue = LOBYTE(color);//GetRValue(color);
	//	CGlbColorVisitor nodeVisiter(red,green,blue);
	//	node->accept(nodeVisiter);	
	//	CGlbWString path = renderInfo->edgeModelLocate->GetValue(mpt_feature.get());
	//	osgDB::writeNodeFile(*(node.get()),"E:\\管线\\张家口不明管线\\aa.osg");
	//}

	osg::ref_ptr<osg::Switch> swiNode = NULL;	
	
	swiNode = node->asSwitch();
	if (swiNode==NULL){mpt_loadState=false;return;};	

	// 默认关闭光照，使用network本身纹理或颜色绘制
	swiNode->getOrCreateStateSet()->setMode(GL_LIGHTING,osg::StateAttribute::OFF);

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

			//{// 处理透明度<100的状况 2015.6.30 malin
			//	if (renderInfo->baselineOpacity)
			//	{
			//		glbInt32 blOpacity = renderInfo->baselineOpacity->GetValue(mpt_feature.get());
			//		if (blOpacity<100)
			//			DealDrillBaselineOpacity(renderInfo->baselineOpacity);
			//	}
			//	if (renderInfo->stratumOpacity)
			//	{
			//		glbInt32 stOpacity = renderInfo->stratumOpacity->GetValue(mpt_feature.get());
			//		if (stOpacity<100)
			//			DealDrillStratumOpacity(renderInfo->stratumOpacity);
			//	}	
			//}
		}else{
			swiNode = NULL;
		}
	//}
	mpt_loadState = false;	

	//mpt_globe->GetView()->JumpTo(point.x(),point.y(),point.z(),1000,0,-90);
	//first = false;
}

/*
*     Dispatcher->AddToScene()->CGlbGlobePoint::AddToScene()  //频繁调用
*     外部线程-->DirectDraw()->AddToScene() //频繁调用
*/
void CGlbGlobeNetworkObject::AddToScene()
{
	/*
	*   mpt_isInHangTasks = true ------ 调用 AddToScene的线程 (A)
	*   mpt_isInHangTasks = false ----- frame 线程            (B)
	*   
	*/
	if(mpt_node == NULL)return;
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
		if (mpt_isShow == false)
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
glbInt32 CGlbGlobeNetworkObject::RemoveFromScene(glbBool isClean)
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
/*
*    DirectDraw 与 LoadData  要互斥分:生产数据
*    mpr_loadState【glbBool】作为互斥分开.
*    Dispatcher线程---在 AddToScene  互斥.
*/
void CGlbGlobeNetworkObject::DirectDraw(glbInt32 level)
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

glbBool CGlbGlobeNetworkObject::SetRenderInfo(GlbRenderInfo *renderInfo,glbBool &isNeedDirectDraw)
{
	isNeedDirectDraw = false;
	if (renderInfo == NULL || renderInfo->type != GLB_OBJECTTYPE_NETWORK)  return false;
	GlbNetworkRenderInfo *networkRenderInfo = dynamic_cast<GlbNetworkRenderInfo *>(renderInfo);
	if (networkRenderInfo == NULL) return false;		
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
	rt = DealNetworkRenderInfo(networkRenderInfo,isNeedDirectDraw);
	if (rt) mpr_renderInfo   = renderInfo;
	mpt_loadState    = false;
	return rt;
}

glbBool CGlbGlobeNetworkObject::SetAltitudeMode(GlbAltitudeModeEnum mode,glbBool &isNeedDirectDraw)
{
	isNeedDirectDraw = false;
	if (mode!=GLB_ALTITUDEMODE_ABSOLUTE)
		return false;
	return true;
}

GlbRenderInfo *CGlbGlobeNetworkObject::GetRenderInfo()
{
	return mpr_renderInfo.get();
}

glbBool CGlbGlobeNetworkObject::SetShow(glbBool isShow,glbBool isOnState)
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

	glbref_ptr<GlbNetworkRenderInfo> networkRenderInfo = dynamic_cast<GlbNetworkRenderInfo *>(mpr_renderInfo.get());
	if (!networkRenderInfo) return false;

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

glbBool CGlbGlobeNetworkObject::SetSelected(glbBool isSelected)
{
	if(mpt_isSelected == isSelected) return true;
	mpt_isSelected = isSelected;
	if (mpt_node != NULL && mpt_altitudeMode!=GLB_ALTITUDEMODE_ONTERRAIN)
	{
		DealSelected();
	}
	return true;
}

glbBool CGlbGlobeNetworkObject::SetBlink(glbBool isBlink)
{
	if (mpt_isBlink == isBlink) return true;
	mpt_isBlink = isBlink;
	if (mpt_node != NULL && mpt_altitudeMode!=GLB_ALTITUDEMODE_ONTERRAIN)
		DealBlink();
	return true;
}

void CGlbGlobeNetworkObject::DealSelected()
{
	glbref_ptr<GlbNetworkRenderInfo> renderInfo = dynamic_cast<GlbNetworkRenderInfo *>(mpr_renderInfo.get());
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
		stateset->setMode(GL_LIGHTING,osg::StateAttribute::OFF);
		stateset->setMode(GL_TEXTURE_2D,osg::StateAttribute::ON);
	}
}
void CGlbGlobeNetworkObject::DealBlink()
{
	if(mpt_isBlink)
	{
		osg::NodeCallback *callback = mpt_node->getUpdateCallback();
		if (callback)
		{
			mpt_node->removeUpdateCallback(callback);
		}

		osg::ref_ptr<NetworkObjectCallback> networkCallback = new NetworkObjectCallback(this);
		mpt_node->addUpdateCallback(networkCallback);
	}
	else
	{
		// 关闭光照
		mpt_node->getOrCreateStateSet()->setMode(GL_LIGHTING,osg::StateAttribute::OFF);
		osg::Material *material  = 
			dynamic_cast<osg::Material *>(mpt_node->getOrCreateStateSet()->getAttribute(osg::StateAttribute::MATERIAL));
		mpt_node->getOrCreateStateSet()->removeAttribute(material);
		mpt_node->getOrCreateStateSet()->setMode(GL_TEXTURE_2D,osg::StateAttribute::ON);

		osg::NodeCallback *callback = mpt_node->getUpdateCallback();
		if (callback)
		{
			mpt_node->removeUpdateCallback(callback);
			//取消闪烁之后，处理对象的显示
			DealSelected();
		}		
	}
}

glbInt32 CGlbGlobeNetworkObject::GetOsgNodeSize()
{
	if (mpt_node == NULL) return 0;
	if (mpr_objSize==0)
		mpr_objSize = ComputeNodeSize(mpt_node);
	return mpr_objSize;
}

glbInt32 CGlbGlobeNetworkObject::ComputeNodeSize(osg::Node *node)
{
	CGlbCalculateNodeSizeVisitor cnsv;
	node->accept(cnsv);
	return cnsv.getTextureSize() + cnsv.getNodeMemSize();
}

#include "GlbCompareValue.h"
glbBool CGlbGlobeNetworkObject::DealNetworkRenderInfo(GlbNetworkRenderInfo* newNetworkRenderInfo, glbBool &isNeedDirectDraw)
{
	GlbNetworkRenderInfo *tempInfo = dynamic_cast<GlbNetworkRenderInfo *>(mpr_renderInfo.get());
	if (tempInfo == NULL) return false;
	//tempInfo是自身现有的渲染信息
	//newDrillRenderInfo是新设置的渲染信息
	
	if (!CGlbCompareValue::CompareValueColorEqual(tempInfo->edgeColor,newNetworkRenderInfo->edgeColor,mpt_feature.get()))
	{// edge线颜色变化
		DealNetworkEdgeColor(newNetworkRenderInfo->edgeColor);
		isNeedDirectDraw=false;
		return true;
	}

	if (!CGlbCompareValue::CompareValueIntEqual(tempInfo->edgeOpacity,newNetworkRenderInfo->edgeOpacity,mpt_feature.get()))
	{// edge线[模型]不透明度变化
		DealNetworkEdgeOpacity(newNetworkRenderInfo->edgeOpacity);
		isNeedDirectDraw = false;
		return true;
	}

	if (!CGlbCompareValue::CompareValueDoubleEqual(tempInfo->edgeLineWidth,newNetworkRenderInfo->edgeLineWidth,mpt_feature.get()))
	{
		//DealNetworkEdgeLineWidth(newNetworkRenderInfo->edgeLineWidth);
		isNeedDirectDraw = true;
		return true;
	}

	if ((!CGlbCompareValue::CompareValueStringEqual(tempInfo->edgeModelLocate,newNetworkRenderInfo->edgeModelLocate,NULL) ||
		tempInfo->edgeModelLocate->bUseField != newNetworkRenderInfo->edgeModelLocate->bUseField ||
		tempInfo->edgeModelLocate->field != newNetworkRenderInfo->edgeModelLocate->field))
	{//模型地址被修改.
		isNeedDirectDraw = true;
		return true;
	}

	if (!CGlbCompareValue::CompareValueColorEqual(tempInfo->nodeColor,newNetworkRenderInfo->nodeColor,mpt_feature.get()))
	{// node点颜色变化
		DealNetworkNodeColor(newNetworkRenderInfo->nodeColor);
		isNeedDirectDraw=false;
		return true;
	}

	if (!CGlbCompareValue::CompareValueIntEqual(tempInfo->nodeOpacity,newNetworkRenderInfo->nodeOpacity,mpt_feature.get()))
	{// node点[模型]不透明度变化
		DealNetworkNodeOpacity(newNetworkRenderInfo->nodeOpacity);
		isNeedDirectDraw = false;
		return true;
	}

	if (!CGlbCompareValue::CompareValueDoubleEqual(tempInfo->nodeSize,newNetworkRenderInfo->nodeSize,mpt_feature.get()))
	{
		DealNetworkNodeSize(newNetworkRenderInfo->nodeSize);
		isNeedDirectDraw = false;
		return true;
	}

	if (!CGlbCompareValue::CompareValueBoolEqual(tempInfo->isRenderFromNode,newNetworkRenderInfo->isRenderFromNode,mpt_feature.get()))
	{// node from节点绘制与否变换
		DealNetworkIsRenderFromNode(newNetworkRenderInfo->isRenderFromNode);
		isNeedDirectDraw = false;
		return true;
	}	

	if (!CGlbCompareValue::CompareValueBoolEqual(tempInfo->isRenderToNode,newNetworkRenderInfo->isRenderToNode,mpt_feature.get()))
	{// node to节点绘制与否变换
		DealNetworkIsRenderToNode(newNetworkRenderInfo->isRenderToNode);
		isNeedDirectDraw = false;
		return true;
	}	

	if ((!CGlbCompareValue::CompareValueStringEqual(tempInfo->fromNodeModelLocate,newNetworkRenderInfo->fromNodeModelLocate,NULL) ||
		tempInfo->fromNodeModelLocate->bUseField != newNetworkRenderInfo->fromNodeModelLocate->bUseField ||
		tempInfo->fromNodeModelLocate->field != newNetworkRenderInfo->fromNodeModelLocate->field))
	{//模型地址被修改.
		isNeedDirectDraw = true;
		return true;
	}

	if ((!CGlbCompareValue::CompareValueStringEqual(tempInfo->toNodeModelLocate,newNetworkRenderInfo->toNodeModelLocate,NULL) ||
		tempInfo->toNodeModelLocate->bUseField != newNetworkRenderInfo->toNodeModelLocate->bUseField ||
		tempInfo->toNodeModelLocate->field != newNetworkRenderInfo->toNodeModelLocate->field))
	{//模型地址被修改.
		isNeedDirectDraw = true;
		return true;
	}

	return true;
}

void CGlbGlobeNetworkObject::DealNetworkEdgeColor(GlbRenderColor* clr)
{
	if (mpt_node==NULL || clr==NULL)return;
	if (mpt_node->asGroup()->getNumChildren()<=0)return;
	osg::Node* edgeNode = mpt_node->asGroup()->getChild(0);

	glbInt32 color = clr->GetValue(mpt_feature.get());

	glbInt32 red = LOBYTE((color)>>16);//GetBValue(color);
	glbInt32 green = LOBYTE(((color & 0xFFFF)) >> 8);//GetGValue(color);
	glbInt32 blue = LOBYTE(color);//GetRValue(color);
	CGlbColorVisitor nodeVisiter(red,green,blue);
	edgeNode->accept(nodeVisiter);	
}
#include "GlbOpacityVisitor.h"
void CGlbGlobeNetworkObject::DealNetworkEdgeOpacity(GlbRenderInt32* opacity)
{
	if (mpt_node==NULL || opacity==NULL)return;
	if (mpt_node->asGroup()->getNumChildren()<=0)return;
	osg::Node* edgeNode = mpt_node->asGroup()->getChild(0);
	
	glbInt32 opt = opacity->GetValue(mpt_feature.get());

	CGlbOpacityVisitor nodeVisiter(opt);
	edgeNode->accept(nodeVisiter);

	osg::StateSet* ss = edgeNode->getOrCreateStateSet();
	//混合是否启动
	if (opt<100)
	{// 启用混合
		ss->setMode(GL_BLEND, osg::StateAttribute::ON);
		ss->setRenderBinMode(osg::StateSet::OVERRIDE_RENDERBIN_DETAILS);
		if (osg::StateSet::TRANSPARENT_BIN != ss->getRenderingHint())
			ss->setRenderingHint( osg::StateSet::TRANSPARENT_BIN );		
		ss->setMode(GL_LIGHTING,osg::StateAttribute::OFF);
	}
	else
	{// 禁用混合						
		ss->setMode(GL_BLEND, osg::StateAttribute::OFF  | osg::StateAttribute::OVERRIDE);			
		if (osg::StateSet::TRANSPARENT_BIN == ss->getRenderingHint())
			ss->setRenderingHint( osg::StateSet::OPAQUE_BIN );		
	}	

}
void CGlbGlobeNetworkObject::DealNetworkEdgeLineWidth(GlbRenderDouble* lineWidth)
{
	if (mpt_node==NULL || lineWidth==NULL)return;
	if (mpt_node->asGroup()->getNumChildren()<=0)return;
	osg::Node* edgeNode = mpt_node->asGroup()->getChild(0);

	glbDouble lw = lineWidth->GetValue(mpt_feature.get());

	osg::LineWidth *osglineWidth = new osg::LineWidth;
	osglineWidth->setWidth(lw);
	osg::StateSet* ss = edgeNode->getOrCreateStateSet();
	ss->setAttributeAndModes(osglineWidth,osg::StateAttribute::ON);
}
void CGlbGlobeNetworkObject::DealNetworkNodeColor(GlbRenderColor* clr)
{
	if (mpt_node==NULL || clr==NULL)return;
	if (mpt_node->asGroup()->getNumChildren()<=1)return;

	glbInt32 color = clr->GetValue(mpt_feature.get());

	glbInt32 red = LOBYTE((color)>>16);//GetBValue(color);
	glbInt32 green = LOBYTE(((color & 0xFFFF)) >> 8);//GetGValue(color);
	glbInt32 blue = LOBYTE(color);//GetRValue(color);

	for (unsigned int i = 1 ; i < mpt_node->asGroup()->getNumChildren(); i++)
	{
		osg::Node* nodeNode = mpt_node->asGroup()->getChild(i);
		if (nodeNode==NULL) continue;

		CGlbColorVisitor nodeVisiter(red,green,blue);
		nodeNode->accept(nodeVisiter);	
	}
}
void CGlbGlobeNetworkObject::DealNetworkNodeOpacity(GlbRenderInt32* opacity)
{
	if (mpt_node==NULL || opacity==NULL)return;
	if (mpt_node->asGroup()->getNumChildren()<=1)return;
	osg::Node* edgeNode = mpt_node->asGroup()->getChild(0);

	glbInt32 opt = opacity->GetValue(mpt_feature.get());

	for (unsigned int i = 1 ; i < mpt_node->asGroup()->getNumChildren(); i++)
	{
		osg::Node* nodeNode = mpt_node->asGroup()->getChild(i);
		if (nodeNode==NULL) continue;

		CGlbOpacityVisitor nodeVisiter(opt);
		nodeNode->accept(nodeVisiter);

		osg::StateSet* ss = nodeNode->getOrCreateStateSet();
		//混合是否启动
		if (opt<100)
		{// 启用混合
			ss->setMode(GL_BLEND, osg::StateAttribute::ON);
			ss->setRenderBinMode(osg::StateSet::OVERRIDE_RENDERBIN_DETAILS);
			if (osg::StateSet::TRANSPARENT_BIN != ss->getRenderingHint())
				ss->setRenderingHint( osg::StateSet::TRANSPARENT_BIN );		
			ss->setMode(GL_LIGHTING,osg::StateAttribute::OFF);
		}
		else
		{// 禁用混合						
			ss->setMode(GL_BLEND, osg::StateAttribute::OFF  | osg::StateAttribute::OVERRIDE);			
			if (osg::StateSet::TRANSPARENT_BIN == ss->getRenderingHint())
				ss->setRenderingHint( osg::StateSet::OPAQUE_BIN );		
		}	
	}
}
void CGlbGlobeNetworkObject::DealNetworkNodeSize(GlbRenderDouble* nodeSize)
{
	if (mpt_node==NULL || nodeSize==NULL)return;
	if (mpt_node->asGroup()->getNumChildren()<=1)return;
	osg::Node* edgeNode = mpt_node->asGroup()->getChild(0);

	glbDouble nz = nodeSize->GetValue(mpt_feature.get());

	osg::Point *osgPoint = new osg::Point;
	osgPoint->setSize(nz);

	for (unsigned int i = 1 ; i < mpt_node->asGroup()->getNumChildren(); i++)
	{
		osg::Node* nodeNode = mpt_node->asGroup()->getChild(i);
		if (nodeNode==NULL) continue;
		osg::StateSet* ss = nodeNode->getOrCreateStateSet();
		ss->setAttributeAndModes(osgPoint,osg::StateAttribute::ON);
	}
}
void CGlbGlobeNetworkObject::DealNetworkIsRenderFromNode(GlbRenderBool* bRenderNode)
{
	if (mpt_node==NULL || bRenderNode==NULL) return;
	if (mpt_node->asGroup()->getNumChildren()<0) return;

	glbBool bRender = bRenderNode->GetValue(mpt_feature.get());

	for (unsigned int i = mpt_node->asGroup()->getNumChildren()-1; i >= 0 ;i--)
	{
		if (mpt_node->asGroup()->getChild(i)->getName() == "network_fromnode")
		{
			if (bRender)
				mpt_node->asGroup()->getChild(i)->setNodeMask(0xffffffff);
			else
				mpt_node->asGroup()->getChild(i)->setNodeMask(0x0);
			return;
		}
	}
}
void CGlbGlobeNetworkObject::DealNetworkIsRenderToNode(GlbRenderBool* bRenderNode)
{
	if (mpt_node==NULL || bRenderNode==NULL) return;
	if (mpt_node->asGroup()->getNumChildren()<0) return;

	glbBool bRender = bRenderNode->GetValue(mpt_feature.get());

	for (unsigned int i = mpt_node->asGroup()->getNumChildren()-1; i >= 0 ;i--)
	{
		if (mpt_node->asGroup()->getChild(i)->getName() == "network_tonode")
		{
			if (bRender)
				mpt_node->asGroup()->getChild(i)->setNodeMask(0xffffffff);
			else
				mpt_node->asGroup()->getChild(i)->setNodeMask(0x0);
			return;
		}
	}
}

glbBool CGlbGlobeNetworkObject::Pick(glbInt32 mx,glbInt32 my,glbInt32& nodeId)
{
	if (mpt_node==NULL) return false;

	glbDouble _x = mx;
	glbDouble _y = my;

	CGlbGlobeView* globeView = GetGlobe()->GetView();
	globeView->WindowToScreen(_x,_y);

	osg::Vec3d vStart(_x,_y,0.0);
	osg::Vec3d vEnd(_x,_y,1.0);	
	globeView->ScreenToWorld(vStart.x(),vStart.y(),vStart.z());
	globeView->ScreenToWorld(vEnd.x(),vEnd.y(),vEnd.z());

	//先用射线求交
	osg::ref_ptr< osgUtil::LineSegmentIntersector > picker = new osgUtil::LineSegmentIntersector(vStart, vEnd);
	osgUtil::IntersectionVisitor iv(picker.get());
	bool bFindFromNode = false;
	bool bFindToNode = false;
	mpt_node->accept(iv);
	if (picker->containsIntersections())
	{
		osgUtil::LineSegmentIntersector::Intersections& intersections = picker->getIntersections();		
		const osgUtil::LineSegmentIntersector::Intersection& hit = *(intersections.begin());
		//osg::Vec3d hitPt = hit.getWorldIntersectPoint();
		//double dis = (ln_pt1-hitPt).length();
		const osg::NodePath& nodePath = hit.nodePath;
		for(osg::NodePath::const_iterator nitr=nodePath.begin();
			nitr!=nodePath.end();
			++nitr)
		{
			osg::Node* _node = dynamic_cast<osg::Node*>(*nitr);
			if (_node){
				std::string ndName = _node->getName();
				if (_node->getName() == "network_fromnode")
				{
					bFindFromNode = true;
					break;
				}
				else if (_node->getName() == "network_tonode")
				{
					bFindToNode = true;
					break;
				}
			}
		}				
	}	

	if (bFindFromNode==false && bFindToNode==false )
	{//如果没有交点，再用polytope求交[像素线]
		// 用棱锥体求交的方法来获取 像素线
		double xMin = _x - 3;
		double yMin = _y - 3;
		double xMax = _x + 3;
		double yMax = _y + 3;
		double zNear = 0.0;//window coord
		osg::Polytope _polytope;
		_polytope.add(osg::Plane(1.0, 0.0, 0.0, -xMin));
		_polytope.add(osg::Plane(-1.0,0.0 ,0.0, xMax));
		_polytope.add(osg::Plane(0.0, 1.0, 0.0,-yMin));
		_polytope.add(osg::Plane(0.0,-1.0,0.0, yMax));
		_polytope.add(osg::Plane(0.0,0.0,1.0, -zNear));

		osg::Polytope transformedPolytope;
		osg::Camera* p_camera = globeView->GetOsgCamera();
		osg::Matrix VPW = p_camera->getViewMatrix() *
			p_camera->getProjectionMatrix() *
			p_camera->getViewport()->computeWindowMatrix();	

		transformedPolytope.setAndTransformProvidingInverse(_polytope, VPW);
		// end 生成polytope	

		osg::ref_ptr<osgUtil::PolytopeIntersector > picker = new osgUtil::PolytopeIntersector(_polytope);
		osgUtil::IntersectionVisitor iv(picker.get());

		mpt_node->accept(iv);
		if (picker->containsIntersections())
		{// 不需要考虑距离
			const osg::NodePath& nodePath = picker->getFirstIntersection().nodePath;	
			for(osg::NodePath::const_iterator nitr=nodePath.begin();
				nitr!=nodePath.end();
				++nitr)
			{
				osg::Node* _node = dynamic_cast<osg::Node*>(*nitr);
				if (_node){
					std::string ndName = _node->getName();
					if (_node->getName() == "network_fromnode")
					{
						bFindFromNode = true;
						break;
					}
					else if (_node->getName() == "network_tonode")
					{
						bFindToNode = true;
						break;
					}
				}
			}		
		}		
	}
	if (bFindFromNode)
	{		
		const GLBVARIANT* va = mpt_feature->GetValueByName(L"GlbFNodeID");
		if (va && va->isnull == false && va->vt == GLB_DATATYPE_INT32)
			nodeId = va->lVal;
		else
			return false;
	}
	if (bFindToNode)
	{
		const GLBVARIANT* va = mpt_feature->GetValueByName(L"GlbTNodeID");
		if (va && va->isnull == false && va->vt == GLB_DATATYPE_INT32)
			nodeId = va->lVal;
		else
			return false;
	}

	if (bFindFromNode || bFindToNode)
		return true;
	return false;
}

void CGlbGlobeNetworkObject::SetNodeSelected(glbInt32 nodeId,glbBool isSelected)
{
	if (mpt_feature==NULL) return;
	if (mpt_node == NULL || mpt_altitudeMode!=GLB_ALTITUDEMODE_ONTERRAIN)
		return;

	std::map<glbInt32,glbBool>::iterator itr = mpr_NodeSelect.find(nodeId);
	if (itr!=mpr_NodeSelect.end())
	{
		glbBool isNodeSelect = itr->second;
		if (isNodeSelect == isSelected)
			return;
	}
	mpr_NodeSelect[nodeId] = isSelected;

	osg::Node* node = NULL;
	glbInt32 fromNodeID = -1;
	const GLBVARIANT* va = mpt_feature->GetValueByName(L"GlbFNodeID");
	if (va && va->isnull == false && va->vt == GLB_DATATYPE_INT32)
		fromNodeID = va->lVal;
	if (fromNodeID==nodeId)
	{
		for (unsigned int i = mpt_node->asGroup()->getNumChildren()-1; i >= 0 ;i--)
		{
			if (mpt_node->asGroup()->getChild(i)->getName() == "network_fromnode")
			{
				node = mpt_node->asGroup()->getChild(i);
				break;
			}
		}	
	}
	if (node==NULL)
	{
		glbInt32 toNodeID = -1;
		va = mpt_feature->GetValueByName(L"GlbTNodeID");
		if (va && va->isnull == false && va->vt == GLB_DATATYPE_INT32)
			toNodeID = va->lVal;
		if (toNodeID==nodeId)
		{
			for (unsigned int i = mpt_node->asGroup()->getNumChildren()-1; i >= 0 ;i--)
			{
				if (mpt_node->asGroup()->getChild(i)->getName() == "network_tonode")
				{
					node = mpt_node->asGroup()->getChild(i);
					break;
				}
			}	
		}
	}

	if (node==NULL) return;
	DealNodeSelect (node,isSelected );
}

void CGlbGlobeNetworkObject::DealNodeSelect(osg::Node* node,glbBool isSelected)
{
	osg::ref_ptr<osg::StateSet> stateset = node->getOrCreateStateSet();	
	osg::ref_ptr<osg::Material> material = dynamic_cast<osg::Material *>(stateset->getAttribute(osg::StateAttribute::MATERIAL));
	if(material == NULL)
		material = new osg::Material;
	if (isSelected)
	{
		material->setDiffuse(osg::Material::FRONT_AND_BACK,osg::Vec4(1,0,0,1));		
		stateset->setAttribute(material,osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
		stateset->setMode(GL_LIGHTING,osg::StateAttribute::ON);
		stateset->setMode(GL_TEXTURE_2D,osg::StateAttribute::OFF | osg::StateAttribute::OVERRIDE);
	}
	else
	{
		if (material)
			stateset->removeAttribute(material);
		stateset->setMode(GL_LIGHTING,osg::StateAttribute::OFF);
		stateset->setMode(GL_TEXTURE_2D,osg::StateAttribute::ON);
	}
}

void CGlbGlobeNetworkObject::SetNodeBlink(glbInt32 nodeId,glbBool isBlink)
{
	if (mpt_feature==NULL) return;
	if (mpt_node == NULL || mpt_altitudeMode!=GLB_ALTITUDEMODE_ONTERRAIN)
		return;

	std::map<glbInt32,glbBool>::iterator itr = mpr_NodeBlink.find(nodeId);
	if (itr!=mpr_NodeBlink.end())
	{
		glbBool isNodeSelect = itr->second;
		if (isNodeSelect == isBlink)
			return;
	}
	mpr_NodeBlink[nodeId] = isBlink;

	osg::Node* node = NULL;
	glbInt32 fromNodeID = -1;
	const GLBVARIANT* va = mpt_feature->GetValueByName(L"GlbFNodeID");
	if (va && va->isnull == false && va->vt == GLB_DATATYPE_INT32)
		fromNodeID = va->lVal;
	if (fromNodeID==nodeId)
	{
		for (unsigned int i = mpt_node->asGroup()->getNumChildren()-1; i >= 0 ;i--)
		{
			if (mpt_node->asGroup()->getChild(i)->getName() == "network_fromnode")
			{
				node = mpt_node->asGroup()->getChild(i);
				break;
			}
		}	
	}
	if (node==NULL)
	{
		glbInt32 toNodeID = -1;
		va = mpt_feature->GetValueByName(L"GlbTNodeID");
		if (va && va->isnull == false && va->vt == GLB_DATATYPE_INT32)
			toNodeID = va->lVal;
		if (toNodeID==nodeId)
		{
			for (unsigned int i = mpt_node->asGroup()->getNumChildren()-1; i >= 0 ;i--)
			{
				if (mpt_node->asGroup()->getChild(i)->getName() == "network_tonode")
				{
					node = mpt_node->asGroup()->getChild(i);
					break;
				}
			}	
		}
	}

	if (node==NULL) return;

	if(mpt_isBlink)
	{
		osg::NodeCallback *callback = node->getUpdateCallback();
		if (callback)
		{
			mpt_node->removeUpdateCallback(callback);
		}

		osg::ref_ptr<NetworkObjectCallback> networkCallback = new NetworkObjectCallback(this,node);
		node->addUpdateCallback(networkCallback);
	}
	else
	{
		// 关闭光照
		mpt_node->getOrCreateStateSet()->setMode(GL_LIGHTING,osg::StateAttribute::OFF);
		osg::Material *material  = 
			dynamic_cast<osg::Material *>(node->getOrCreateStateSet()->getAttribute(osg::StateAttribute::MATERIAL));
		node->getOrCreateStateSet()->removeAttribute(material);
		node->getOrCreateStateSet()->setMode(GL_TEXTURE_2D,osg::StateAttribute::ON);

		osg::NodeCallback *callback = node->getUpdateCallback();
		if (callback)
		{
			node->removeUpdateCallback(callback);
			glbBool isSelected = false;
			std::map<glbInt32,glbBool>::iterator itr = mpr_NodeSelect.find(nodeId);
			if (itr != mpr_NodeSelect.end())
				isSelected = itr->second;
			//取消闪烁之后，处理对象的显示
			DealNodeSelect(node,isSelected);
		}		
	}
}

void CGlbGlobeNetworkObject::SetNodeShow(glbInt32 nodeId,glbBool isShow)
{	
	if (mpt_feature==NULL) return;
	osg::Node* node = NULL;
	glbInt32 fromNodeID = -1;
	const GLBVARIANT* va = mpt_feature->GetValueByName(L"GlbFNodeID");
	if (va && va->isnull == false && va->vt == GLB_DATATYPE_INT32)
		fromNodeID = va->lVal;
	if (fromNodeID==nodeId)
	{
		for (unsigned int i = mpt_node->asGroup()->getNumChildren()-1; i >= 0 ;i--)
		{
			if (mpt_node->asGroup()->getChild(i)->getName() == "network_fromnode")
			{
				node = mpt_node->asGroup()->getChild(i);
				break;
			}
		}	
	}
	if (node==NULL)
	{
		glbInt32 toNodeID = -1;
		va = mpt_feature->GetValueByName(L"GlbTNodeID");
		if (va && va->isnull == false && va->vt == GLB_DATATYPE_INT32)
			toNodeID = va->lVal;
		if (toNodeID==nodeId)
		{
			for (unsigned int i = mpt_node->asGroup()->getNumChildren()-1; i >= 0 ;i--)
			{
				if (mpt_node->asGroup()->getChild(i)->getName() == "network_tonode")
				{
					node = mpt_node->asGroup()->getChild(i);
					break;
				}
			}	
		}
	}

	if (node==NULL) return;
	if (isShow)	
		node->setNodeMask(0xffffffff);	
	else
		node->setNodeMask(0x0);
}

