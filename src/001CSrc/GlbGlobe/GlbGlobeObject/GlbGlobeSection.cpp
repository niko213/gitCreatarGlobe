#include "StdAfx.h"
#include "GlbGlobeSection.h"
#include "CGlbGlobe.h"
#include "GlbGlobeView.h"
#include "GlbGlobeTypes.h"
#include "GlbPolygon.h"
#include "GlbCalculateNodeSizeVisitor.h"
#include "GlbGlobeMemCtrl.h"
#include "GlbGlobeFeatureLayer.h"
#include "GlbGlobeSectionSymbol.h"
#include "GlbSection.h"
#include <osg/Material>
#include "GlbCompareValue.h"
#include "GlbColorVisitor.h"
#include "GlbOpacityVisitor.h"
#include "osg/Depth"
#include "GlbFadeInOutCallback.h"

using namespace GlbGlobe;

class SectionObjectCallback : public osg::NodeCallback
{
public:
	SectionObjectCallback(CGlbGlobeRObject *obj, osg::Node* node=NULL)
	{
		mpr_isChangeFrame = false;
		mpr_markNum = 0;	
		CGlbGlobeSection *secObject = dynamic_cast<CGlbGlobeSection *>(obj);
		mpr_stateset = NULL;
		mpr_node = node;
		if (mpr_node==NULL)
		{
			if(secObject)				
				mpr_stateset = secObject->GetOsgNode()->getOrCreateStateSet();					
		}
		else
		{
			mpr_stateset = mpr_node->getOrCreateStateSet();
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
		if (nv->getFrameStamp()->getFrameNumber() - mpr_markNum > 10)
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
	osg::ref_ptr<osg::Node> mpr_node;
};

// CGlbGlobeSection接口
CGlbGlobeSection::CGlbGlobeSectionTask::CGlbGlobeSectionTask( CGlbGlobeSection *obj,glbInt32 level )
{
	mpr_obj = obj;
	mpr_level = level;
}

CGlbGlobeSection::CGlbGlobeSectionTask::~CGlbGlobeSectionTask( )
{
	mpr_obj = NULL;
}

glbBool CGlbGlobeSection::CGlbGlobeSectionTask::doRequest()
{
	if(mpr_obj)
	{
		mpr_obj->ReadData(mpr_level);
		return true;
	}
	return false;
}

CGlbGlobeObject * CGlbGlobeSection::CGlbGlobeSectionTask::getObject()
{
	return mpr_obj.get();
}

CGlbGlobeSection::CGlbGlobeSection(void)
{
	mpr_readData_critical.SetName(L"section_readdata");
	mpr_addToScene_critical.SetName(L"section_addscene");
	mpt_altitudeMode = GLB_ALTITUDEMODE_ABSOLUTE; //剖面都是绝对坐标方式
	mpr_needReReadData = false;
}


CGlbGlobeSection::~CGlbGlobeSection(void)
{
}

GlbGlobeObjectTypeEnum CGlbGlobeSection::GetType()
{
	return GLB_OBJECTTYPE_SECTION;
}

glbBool CGlbGlobeSection::Load(xmlNodePtr* node,glbWChar* prjPath)
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

		GlbSectionRenderInfo *renderInfo = new GlbSectionRenderInfo();
		renderInfo->Load(rdchild,prjPath);
		mpr_renderInfo = renderInfo;
		ParseObjectFadeColor();
	}
	return true;
}

glbBool CGlbGlobeSection::Save(xmlNodePtr node,const glbWChar* prjPath)
{
	CGlbGlobeRObject::Save(node,prjPath);

	xmlNodePtr rdnode = xmlNewNode(NULL,BAD_CAST "RenderInfo");
	xmlAddChild(node,rdnode);
	if(mpr_renderInfo)
	{
		char str[32];
		sprintf_s(str,"%d",mpr_renderInfo->type);
		xmlNewTextChild(rdnode,NULL,BAD_CAST "RenderType",BAD_CAST str);

		GlbSectionRenderInfo *renderInfo = dynamic_cast<GlbSectionRenderInfo*>(mpr_renderInfo.get());
		if(renderInfo)
			renderInfo->Save(rdnode,prjPath);
	}
	return true;
}

glbref_ptr<CGlbExtent> CGlbGlobeSection::GetBound(glbBool isWorld)
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

glbDouble CGlbGlobeSection::GetDistance(osg::Vec3d &cameraPos,glbBool isCompute)
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

IGlbGeometry *CGlbGlobeSection::GetOutLine()
{
	if (mpt_feature==NULL || mpt_globe==NULL)
		return NULL;

	if (mpr_outline==NULL)
	{
		glbref_ptr<CGlbExtent> geoExt = const_cast<CGlbExtent *>(mpt_feature->GetExtent());
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

void CGlbGlobeSection::ReadData(glbInt32 level,glbBool isDirect)
{
	glbref_ptr<GlbSectionRenderInfo> renderInfo = dynamic_cast<GlbSectionRenderInfo*>(mpr_renderInfo.get());
	if( renderInfo == NULL || mpt_altitudeMode != GLB_ALTITUDEMODE_ABSOLUTE )
	{mpt_loadState=false;return;}

	if (renderInfo == NULL) {mpt_loadState=false;return;}
	if (mpt_feature == NULL){mpt_loadState=false;return;}	
	CGlbSection* section = dynamic_cast<CGlbSection*>(mpt_feature.get());
	if (section==NULL) {mpt_loadState=false;return;}

	CGlbGlobeSectionSymbol symbol;
	osg::ref_ptr<osg::Node> node = symbol.Draw(this,NULL);
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

void CGlbGlobeSection::LoadData(glbInt32 level)
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
	glbref_ptr<GlbSectionRenderInfo> renderInfo = dynamic_cast<GlbSectionRenderInfo *>(mpr_renderInfo.get());
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
	glbref_ptr<CGlbGlobeSectionTask> task = new CGlbGlobeSectionTask(this,level);
	mpt_globe->mpr_taskmanager->AddTask(task.get());
}

void CGlbGlobeSection::AddToScene()
{
	/*
	*   mpt_isInHangTasks = true ------ 调用 AddToScene的线程 (A)
	*   mpt_isInHangTasks = false ----- frame 线程            (B)
	*   
	*/
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

glbInt32 CGlbGlobeSection::RemoveFromScene(glbBool isClean)
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
void CGlbGlobeSection::DirectDraw(glbInt32 level)
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

glbBool CGlbGlobeSection::SetAltitudeMode(GlbAltitudeModeEnum mode,glbBool &isNeedDirectDraw)
{
	isNeedDirectDraw = false;
	if (mode!=GLB_ALTITUDEMODE_ABSOLUTE)
		return false;
	return true;
}

glbBool CGlbGlobeSection::SetRenderInfo(GlbRenderInfo *renderInfo,glbBool &isNeedDirectDraw)
{
	isNeedDirectDraw = false;
	if (renderInfo == NULL || renderInfo->type != GLB_OBJECTTYPE_SECTION)  return false;
	GlbSectionRenderInfo *sectionRenderInfo = dynamic_cast<GlbSectionRenderInfo *>(renderInfo);
	if (sectionRenderInfo == NULL) return false;		
	{//与LoadData互斥.
		/*
		*   这是因为： ReadData执行在中途时，生成的结果可能不会反应 新设置的属性.
		*/
		GlbScopedLock<GlbCriticalSection> lock(mpr_readData_critical);
		if (mpt_loadState == true)
		{
			isNeedDirectDraw = true;
			mpr_renderInfo   = renderInfo;
			ParseObjectFadeColor();
			return true;			
		}
		mpt_loadState = true;
	}
	if(mpt_node == NULL)
	{		
		mpr_renderInfo   = renderInfo;
		ParseObjectFadeColor();
		mpt_loadState    = false;
		/*
			如果isNeedDirectDraw=true， 则图层渲染器renderer调用SetRenderInfo接口会导致
			图层中所有对象都要加载，如果图层对象量很大会直接导致崩溃！！！！
		*/
		//isNeedDirectDraw = true;
		return true;
	}
	
	glbBool rt = false;
	rt = DealSectionRenderInfo(sectionRenderInfo,isNeedDirectDraw);
	if (rt)
	{
		mpr_renderInfo   = renderInfo;
		ParseObjectFadeColor();
	}
	mpt_loadState    = false;
	return rt;
}

GlbRenderInfo *CGlbGlobeSection::GetRenderInfo()
{
	return mpr_renderInfo.get();
}

glbBool CGlbGlobeSection::SetShow(glbBool isShow,glbBool isOnState)
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

	glbref_ptr<GlbSectionRenderInfo> renderInfo = dynamic_cast<GlbSectionRenderInfo *>(mpr_renderInfo.get());
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

glbBool CGlbGlobeSection::SetSelected(glbBool isSelected)
{
	if(mpt_isSelected == isSelected) return true;
	mpt_isSelected = isSelected;
	if (mpt_node != NULL && mpt_altitudeMode!=GLB_ALTITUDEMODE_ONTERRAIN)
	{
		DealSelected();
	}
	return true;
}

glbBool CGlbGlobeSection::SetBlink(glbBool isBlink)
{
	if (mpt_isBlink == isBlink) return true;
	mpt_isBlink = isBlink;
	if (mpt_node != NULL && mpt_altitudeMode!=GLB_ALTITUDEMODE_ONTERRAIN)
		DealBlink();
	return true;
}

glbInt32 CGlbGlobeSection::GetOsgNodeSize()
{
	if (mpt_node == NULL) return 0;
	if (mpr_objSize==0)
		mpr_objSize = ComputeNodeSize(mpt_node);
	return mpr_objSize;
}

void CGlbGlobeSection::DealSelected()
{
	glbref_ptr<GlbSectionRenderInfo> renderInfo = dynamic_cast<GlbSectionRenderInfo *>(mpr_renderInfo.get());
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

void CGlbGlobeSection::DealBlink()
{
	if(mpt_isBlink)
	{
		osg::NodeCallback *callback = mpt_node->getUpdateCallback();
		if (callback)
		{
			mpt_node->removeUpdateCallback(callback);
		}

		osg::ref_ptr<SectionObjectCallback> secCallback = new SectionObjectCallback(this);
		mpt_node->addUpdateCallback(secCallback);
	}
	else
	{
		// 关闭光照
		//mpt_node->getOrCreateStateSet()->setMode(GL_LIGHTING,osg::StateAttribute::OFF);
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

glbInt32 CGlbGlobeSection::ComputeNodeSize(osg::Node *node)
{
	CGlbCalculateNodeSizeVisitor cnsv;
	node->accept(cnsv);
	return cnsv.getTextureSize() + cnsv.getNodeMemSize();
}

glbBool CGlbGlobeSection::DealSectionRenderInfo(GlbSectionRenderInfo* newSectionRenderInfo, glbBool &isNeedDirectDraw)
{// 全部要求重绘
	GlbSectionRenderInfo *tempInfo = dynamic_cast<GlbSectionRenderInfo *>(mpr_renderInfo.get());
	if (tempInfo == NULL) return false;
	//tempInfo是自身现有的渲染信息
	//newDrillRenderInfo是新设置的渲染信息
	if (tempInfo->isRenderSec != newSectionRenderInfo->isRenderSec || 
		tempInfo->isRenderArc != newSectionRenderInfo->isRenderArc ||
		tempInfo->isRenderPoly != newSectionRenderInfo->isRenderPoly )
	{// 剖面 弧段 或 多边形 开关被修改
		isNeedDirectDraw = DealSectionArcPolyONOFF(newSectionRenderInfo->isRenderSec,newSectionRenderInfo->isRenderArc,newSectionRenderInfo->isRenderPoly);	
		return true;
	}

	//-----Section
	if (/*tempInfo->isRenderSec==true &&*/
		!CGlbCompareValue::CompareValueStringEqual(tempInfo->secModelLocate,newSectionRenderInfo->secModelLocate,mpt_feature.get()))
	{//sec模型地址被修改.
		isNeedDirectDraw = true;
		return true;
	}
	if ( /*tempInfo->isRenderSec==true  &&*/		
		!CGlbCompareValue::CompareValueColorEqual(tempInfo->secColor,newSectionRenderInfo->secColor,mpt_feature.get()))
	{// sec颜色变化
		DealSectionColor(newSectionRenderInfo->secColor);
		isNeedDirectDraw=false;
		return true;
	}

	if (/*tempInfo->isRenderSec==true  &&*/
		!CGlbCompareValue::CompareValueIntEqual(tempInfo->secOpacity,newSectionRenderInfo->secOpacity,mpt_feature.get()))
	{// sec不透明度变化
		DealSectionOpacity(newSectionRenderInfo->secOpacity);
		isNeedDirectDraw = false;
		return true;
	}

	//tempInfo->secTextureData
	//tempInfo->secTexRepeatMode
	//tempInfo->secTextureRotation
	//tempInfo->secTilingU
	//tempInfo->secTilingV
	
	//-----Arc
	if ( /*tempInfo->isRenderArc==true  &&	*/	
		!CGlbCompareValue::CompareValueColorEqual(tempInfo->arcColor,newSectionRenderInfo->arcColor,mpt_feature.get()))
	{// arc颜色变化
		if (newSectionRenderInfo->arcColor->bUseField && newSectionRenderInfo->arcColor->field.length()>0)
			isNeedDirectDraw = true;
		else
		{
			DealArcColor(newSectionRenderInfo->arcColor);
			isNeedDirectDraw=false;
		}
		return true;
	}

	if ( /*tempInfo->isRenderArc==true  &&*/		
		!CGlbCompareValue::CompareValueIntEqual(tempInfo->arcOpacity,newSectionRenderInfo->arcOpacity,mpt_feature.get()))
	{// arc透明度变化
		if (newSectionRenderInfo->arcOpacity->bUseField && newSectionRenderInfo->arcOpacity->field.length()>0)
			isNeedDirectDraw = true;
		else
		{
			DealArcOpacity(newSectionRenderInfo->arcOpacity);
			isNeedDirectDraw=false;
		}
		return true;
	}

	if (/* tempInfo->isRenderArc==true  &&	*/	
		!CGlbCompareValue::CompareValueDoubleEqual(tempInfo->arcWidth,newSectionRenderInfo->arcWidth,mpt_feature.get()))
	{// arc线宽变化
		if (newSectionRenderInfo->arcWidth->bUseField && newSectionRenderInfo->arcWidth->field.length()>0)
			isNeedDirectDraw = true;
		else
		{
			DealArcWidth(newSectionRenderInfo->arcWidth);
			isNeedDirectDraw=false;
		}
		return true;
	}

	//-----Poly
	if ( /*tempInfo->isRenderPoly==true  &&	*/	
		!CGlbCompareValue::CompareValueColorEqual(tempInfo->polyColor,newSectionRenderInfo->polyColor,mpt_feature.get()))
	{// poly颜色变化
		if (newSectionRenderInfo->polyColor->bUseField && newSectionRenderInfo->polyColor->field.length()>0)
			isNeedDirectDraw = true;
		else
		{
			DealPolyColor(newSectionRenderInfo->polyColor);
			isNeedDirectDraw=false;
		}
		return true;
	}

	if ( /*tempInfo->isRenderPoly==true  &&	*/	
		!CGlbCompareValue::CompareValueIntEqual(tempInfo->polyOpacity,newSectionRenderInfo->polyOpacity,mpt_feature.get()))
	{// poly透明度变化
		if (newSectionRenderInfo->polyOpacity->bUseField && newSectionRenderInfo->polyOpacity->field.length()>0)
			isNeedDirectDraw = true;
		else
		{
			DealPolyOpacity(newSectionRenderInfo->polyOpacity);
			isNeedDirectDraw=false;
		}
		return true;
	}
	if (/*tempInfo->isRenderPoly==true &&*/
		!CGlbCompareValue::CompareValueStringEqual(tempInfo->polyModelLocate,newSectionRenderInfo->polyModelLocate,mpt_feature.get()))
	{//poly模型地址被修改.
		isNeedDirectDraw = true;
		return true;
	}

	if(!CGlbCompareValue::CompareValueDoubleEqual(tempInfo->xOffset,newSectionRenderInfo->xOffset,mpt_feature.get()) ||
		!CGlbCompareValue::CompareValueDoubleEqual(tempInfo->yOffset,newSectionRenderInfo->yOffset,mpt_feature.get()) ||
		!CGlbCompareValue::CompareValueDoubleEqual(tempInfo->zOffset,newSectionRenderInfo->zOffset,mpt_feature.get())||
		!CGlbCompareValue::CompareValueDoubleEqual(tempInfo->xScale, newSectionRenderInfo->xScale, mpt_feature.get()) ||
		!CGlbCompareValue::CompareValueDoubleEqual(tempInfo->yScale, newSectionRenderInfo->yScale, mpt_feature.get()) ||
		!CGlbCompareValue::CompareValueDoubleEqual(tempInfo->zScale, newSectionRenderInfo->zScale, mpt_feature.get()) )
	{
		osg::MatrixTransform *mt = dynamic_cast<osg::MatrixTransform *>(mpt_node->asGroup()->getChild(0));

		//平移
		osg::Matrix mOffset;
		glbDouble xOffset = 0;
		if(newSectionRenderInfo->xOffset)
			xOffset = newSectionRenderInfo->xOffset->GetValue(mpt_feature.get());
		glbDouble yOffset = 0;
		if (newSectionRenderInfo->yOffset)
			yOffset = newSectionRenderInfo->yOffset->GetValue(mpt_feature.get());
		glbDouble zOffset = 0;
		if (newSectionRenderInfo->zOffset)
			zOffset = newSectionRenderInfo->zOffset->GetValue(mpt_feature.get());

		//缩放
		osg::Matrix scaleMt;
		glbDouble xScale = (newSectionRenderInfo->xScale)?(newSectionRenderInfo->xScale->GetValue(mpt_feature.get())):1.0;
		glbDouble yScale = (newSectionRenderInfo->yScale)?(newSectionRenderInfo->yScale->GetValue(mpt_feature.get())) : 1.0;
		glbDouble zScale = (newSectionRenderInfo->zScale)?(newSectionRenderInfo->zScale->GetValue(mpt_feature.get())) : 1.0;

		if (mpt_globe->GetType() == GLB_GLOBETYPE_FLAT)
		{
			glbDouble xCenter, yCenter, zCenter;
			mpt_feature->GetExtent()->GetCenter(&xCenter, &yCenter, &zCenter);

			osg::Matrix localToWorld;
			localToWorld.makeTranslate(xCenter, yCenter, zCenter);

			scaleMt.makeScale(xScale, yScale, zScale);
			mOffset.makeTranslate(xOffset, yOffset, zOffset);
			mt->setMatrix(osg::Matrix::inverse(localToWorld)* scaleMt *  mOffset * localToWorld);
		}
		
		else
		{

			glbDouble xCenter, yCenter, zCenter;
			mpt_feature->GetExtent()->GetCenter(&xCenter, &yCenter,&zCenter);

			double longitude = osg::DegreesToRadians(xCenter);
			double latitude = osg::DegreesToRadians(yCenter);

			osg::Vec3d localOrigin;//局部原点
			g_ellipsoidModel->convertLatLongHeightToXYZ(latitude, longitude, zCenter, localOrigin.x(), localOrigin.y(), localOrigin.z());

			osg::Matrix localToWorld;
			g_ellipsoidModel->computeLocalToWorldTransformFromXYZ(localOrigin.x(), localOrigin.y(), localOrigin.z(), localToWorld);
			
			scaleMt.makeScale(xScale, yScale, zScale);
			mOffset.makeTranslate(xOffset, yOffset, zOffset);
			//局部平移缩放 再回世界坐标系下
			mt->setMatrix(osg::Matrix::inverse(localToWorld)* scaleMt *  mOffset * localToWorld );

		}

		
		return true;
	}

	//tempInfo->polyTextureData
	//tempInfo->polyTexRepeatMode
	//tempInfo->polyTextureRotation
	//tempInfo->polyTilingU
	//tempInfo->polyTilingV

	isNeedDirectDraw = true;
	return false;
}

bool CGlbGlobeSection::DealSectionArcPolyONOFF(glbBool isSecShow, glbBool isArcShow, glbBool isPolyShow)
{
	bool isNeedDrectDraw = false;
	osg::Switch* secNode = FindSectionNode();	
	if (secNode)
	{
		if (isSecShow)
		{
			if (secNode->getNumChildren()<=0)
				isNeedDrectDraw = true;
			secNode->setAllChildrenOn();
		}
		else
			secNode->setAllChildrenOff();
	}

	osg::Switch* arcNode =FindArcNode();
	if (arcNode)
	{
		if (isArcShow)
		{
			if (arcNode->getNumChildren()<=0)
				isNeedDrectDraw = true;
			arcNode->setAllChildrenOn();
		}
		else
			arcNode->setAllChildrenOff();
	}

	osg::Switch* polyNode = FindPolyNode();
	if (polyNode)
	{
		if (isPolyShow)
		{
			if (polyNode->getNumChildren()<=0)
				isNeedDrectDraw = true;
			polyNode->setAllChildrenOn();
		}
		else
			polyNode->setAllChildrenOff();
	}		
	return isNeedDrectDraw;
}

void CGlbGlobeSection::DealSectionColor(GlbRenderColor* clr)
{
	osg::Node* secNode = FindSectionNode();
	if (secNode==NULL || clr==NULL) return;
	glbInt32 color = clr->GetValue(mpt_feature.get());

	glbInt32 red = LOBYTE((color)>>16);//GetBValue(color);
	glbInt32 green = LOBYTE(((color & 0xFFFF)) >> 8);//GetGValue(color);
	glbInt32 blue = LOBYTE(color);//GetRValue(color);
	CGlbColorVisitor nodeVisiter(red,green,blue);
	secNode->accept(nodeVisiter);	
}
void CGlbGlobeSection::DealSectionOpacity(GlbRenderInt32* opacity)
{
	osg::Node* secNode = FindSectionNode();
	if (secNode==NULL || opacity==NULL) return;
	glbInt32 opt = opacity->GetValue(mpt_feature.get());

	CGlbOpacityVisitor nodeVisiter(opt);
	secNode->accept(nodeVisiter);

	osg::StateSet* ss = secNode->getOrCreateStateSet();
	//混合是否启动
	if (opt<100)
	{// 启用混合
		ss->setMode(GL_BLEND, osg::StateAttribute::ON);
		ss->setRenderBinMode(osg::StateSet::OVERRIDE_RENDERBIN_DETAILS);
		if (osg::StateSet::TRANSPARENT_BIN != ss->getRenderingHint())
			ss->setRenderingHint( osg::StateSet::TRANSPARENT_BIN );
		//ss->setMode(GL_LIGHTING,osg::StateAttribute::OFF);
	}
	else
	{// 禁用混合						
		ss->setMode(GL_BLEND, osg::StateAttribute::OFF  | osg::StateAttribute::OVERRIDE);			
		if (osg::StateSet::TRANSPARENT_BIN == ss->getRenderingHint())
			ss->setRenderingHint( osg::StateSet::OPAQUE_BIN );		
	}	
}

void CGlbGlobeSection::DealArcColor(GlbRenderColor* clr)
{
	osg::Node* arcNode = FindArcNode();
	if (arcNode==NULL || clr==NULL) return;
	glbInt32 color = clr->GetValue(mpt_feature.get());

	glbInt32 red = LOBYTE((color)>>16);//GetBValue(color);
	glbInt32 green = LOBYTE(((color & 0xFFFF)) >> 8);//GetGValue(color);
	glbInt32 blue = LOBYTE(color);//GetRValue(color);
	CGlbColorVisitor nodeVisiter(red,green,blue);
	arcNode->accept(nodeVisiter);	
}
void CGlbGlobeSection::DealArcOpacity(GlbRenderInt32* opacity)
{
	osg::Node* arcNode = FindArcNode();
	if (arcNode==NULL || opacity==NULL) return;
	glbInt32 opt = opacity->GetValue(mpt_feature.get());

	CGlbOpacityVisitor nodeVisiter(opt);
	arcNode->accept(nodeVisiter);

	osg::StateSet* ss = arcNode->getOrCreateStateSet();
	//混合是否启动
	if (opt<100)
	{// 启用混合
		ss->setMode(GL_BLEND, osg::StateAttribute::ON);
		ss->setRenderBinMode(osg::StateSet::OVERRIDE_RENDERBIN_DETAILS);
		if (osg::StateSet::TRANSPARENT_BIN != ss->getRenderingHint())
			ss->setRenderingHint( osg::StateSet::TRANSPARENT_BIN );
		//ss->setMode(GL_LIGHTING,osg::StateAttribute::OFF);
	}
	else
	{// 禁用混合						
		ss->setMode(GL_BLEND, osg::StateAttribute::OFF  | osg::StateAttribute::OVERRIDE);			
		if (osg::StateSet::TRANSPARENT_BIN == ss->getRenderingHint())
			ss->setRenderingHint( osg::StateSet::OPAQUE_BIN );		
	}	
}
#include <osg/LineWidth>
void CGlbGlobeSection::DealArcWidth(GlbRenderDouble* width)
{
	osg::Node* arcNode = FindArcNode();
	if (arcNode==NULL || width==NULL)	return;

	CGlbFeature* feature = GetFeature();
	glbDouble blWidth = width->GetValue(feature);

	osg::StateSet* ss = arcNode->getOrCreateStateSet();
	//线宽
	if(width->bUseField && width->field.length()>0)
	{// 使用字段渲染线宽时 从节点中取消此线宽渲染属性
		osg::StateAttribute* lw = ss->getAttribute(osg::StateAttribute::LINEWIDTH);
		if (lw)	ss->removeAttribute(lw);
	}
	else
	{	
		osg::LineWidth *lineWidth = new osg::LineWidth;
		lineWidth->setWidth(blWidth);
		ss->setAttributeAndModes(lineWidth,osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
	}
}

void CGlbGlobeSection::DealPolyColor(GlbRenderColor* clr)
{
	osg::Node* polyNode = FindPolyNode();
	if (polyNode==NULL || clr==NULL) return;
	glbInt32 color = clr->GetValue(mpt_feature.get());

	glbInt32 red = LOBYTE((color)>>16);//GetBValue(color);
	glbInt32 green = LOBYTE(((color & 0xFFFF)) >> 8);//GetGValue(color);
	glbInt32 blue = LOBYTE(color);//GetRValue(color);
	CGlbColorVisitor nodeVisiter(red,green,blue);
	polyNode->accept(nodeVisiter);	
}
void CGlbGlobeSection::DealPolyOpacity(GlbRenderInt32* opacity)
{
	osg::Node* polyNode = FindPolyNode();
	if (polyNode==NULL || opacity==NULL) return;
	glbInt32 opt = opacity->GetValue(mpt_feature.get());

	CGlbOpacityVisitor nodeVisiter(opt);
	polyNode->accept(nodeVisiter);

	osg::StateSet* ss = polyNode->getOrCreateStateSet();
	//osg::ref_ptr<osg::Depth> depth = NULL;
	//混合是否启动
	if (opt<100)
	{// 启用混合
		ss->setMode(GL_BLEND, osg::StateAttribute::ON);
		ss->setRenderBinMode(osg::StateSet::OVERRIDE_RENDERBIN_DETAILS);
		if (osg::StateSet::TRANSPARENT_BIN != ss->getRenderingHint())
			ss->setRenderingHint( osg::StateSet::TRANSPARENT_BIN );
		//ss->setMode(GL_LIGHTING,osg::StateAttribute::ON);
		//ss->setMode(GL_LIGHTING,osg::StateAttribute::OFF);
		//depth = dynamic_cast<osg::Depth *>(ss->getAttribute(osg::StateAttribute::DEPTH));
		//if(!depth.valid())
		//{
		//	depth = new osg::Depth;
		//	depth->setWriteMask(false);
		//	ss->setAttributeAndModes(depth.get(), osg::StateAttribute::ON);
		//}
		//else
		//{
		//	if(depth->getWriteMask())
		//		depth->setWriteMask(false);
		//}
	}
	else
	{// 禁用混合						
		ss->setMode(GL_BLEND, osg::StateAttribute::OFF);			
		if (osg::StateSet::OPAQUE_BIN != ss->getRenderingHint())
			ss->setRenderingHint( osg::StateSet::OPAQUE_BIN );
		//depth = dynamic_cast<osg::Depth *>(ss->getAttribute(osg::StateAttribute::DEPTH));
		//if(!depth.valid())
		//{
		//	depth = new osg::Depth;
		//	depth->setWriteMask(true);
		//	ss->setAttributeAndModes(depth.get(), osg::StateAttribute::ON);
		//}
		//else
		//{
		//	if(!depth->getWriteMask())
		//		depth->setWriteMask(true);
		//}
	}	
}

glbBool CGlbGlobeSection::PickArc(int mx,int my,glbInt32& arcId)
{// 像素线
	if (mpt_node==NULL) return false;
	if (mpt_node->asGroup()->getNumChildren()<1) return false;
	osg::ref_ptr<osg::Node> arcNode=NULL;
	osg::MatrixTransform* trans = dynamic_cast<osg::MatrixTransform*>(mpt_node->asGroup()->getChild(0));
	if (trans == NULL) return false;
	unsigned int numChild = trans->getNumChildren();
	for (unsigned int i = 0; i < numChild; i++)
	{
		osg::Node * childNode = trans->getChild(i);
		if (childNode->getName()=="Arc")
		{
			arcNode = childNode;
			break;
		}
	}
	if (arcNode==NULL)
		return false;

	glbBool bFindArc=false;

	glbDouble _x = mx;
	glbDouble _y = my;

	CGlbGlobeView* globeView = GetGlobe()->GetView();
	globeView->WindowToScreen(_x,_y);
	
	//用polytope求交[像素线]
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

	osg::ref_ptr<osgUtil::PolytopeIntersector > picker = new osgUtil::PolytopeIntersector(transformedPolytope);
	osgUtil::IntersectionVisitor iv(picker.get());

	// 需要保留上面的偏移缩放矩阵信息
	osg::ref_ptr<osg::MatrixTransform> tempMt = new osg::MatrixTransform();
	tempMt->setMatrix(trans->getMatrix());
	tempMt->addChild(arcNode);

	tempMt->accept(iv);

	if (picker->containsIntersections())
	{// 不需要考虑距离
		const osg::NodePath& nodePath = picker->getFirstIntersection().nodePath;	
		for(osg::NodePath::const_iterator nitr=nodePath.begin();
			nitr!=nodePath.end();
			++nitr)
		{
			osg::Switch* _switch = dynamic_cast<osg::Switch*>(*nitr);
			if (_switch){
				std::string ndName = _switch->getName();
				if (ndName.size()>2 && ndName != "Arc")
				{//"Arc"
					std::string headStr = ndName.substr(0,3);
					if (headStr=="Arc")
					{
						std::string numStr = ndName.substr(3,ndName.length()-3);
						arcId = atoi(numStr.c_str());						
						bFindArc = true;
						break;
					}
				}
			}
		}		
	}		

	return bFindArc;
}
glbBool CGlbGlobeSection::PickPoly(int mx,int my,glbInt32& polyId)
{
	if (mpt_node==NULL) return false;

	glbBool bFindPoly=false;
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
	bool bFindStratum = false;
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
			osg::Switch* _switch = dynamic_cast<osg::Switch*>(*nitr);
			if (_switch){
				std::string ndName = _switch->getName();
				if (ndName.size()>3 && ndName != "Poly")
				{//"Poly"
					std::string headStr = ndName.substr(0,4);					
					if (headStr=="Poly")
					{
						std::string numStr = ndName.substr(4,ndName.length()-4);
						polyId = atoi(numStr.c_str());						
						bFindPoly = true;
						break;
					}
				}
			}
		}				
	}	

	return bFindPoly;
}
void CGlbGlobeSection::SetArcSelected(glbInt32 arcId,glbBool isSelected)
{
	glbBool curSelected = false;
	std::map<glbInt32,glbBool>::iterator itr = mpr_arcIsSeclected.find(arcId);
	if (itr==mpr_arcIsSeclected.end())
	{// 没有找到
		mpr_arcIsSeclected[arcId] = isSelected;
	}
	else
		curSelected = itr->second;
	// 选中状态没有变化
	if (curSelected==isSelected)
		return;

	osg::Node* arcNode = FindArcNodeById(arcId);
	if (arcNode==NULL) return;

	itr->second = isSelected;

	osg::ref_ptr<osg::StateSet> stateset = arcNode->getOrCreateStateSet();	
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
		//stateset->setMode(GL_LIGHTING,osg::StateAttribute::OFF);
		stateset->setMode(GL_TEXTURE_2D,osg::StateAttribute::ON);
	}
}

void CGlbGlobeSection::SetArcBlink(glbInt32 arcId,glbBool isBlink)
{
	osg::Node* node = FindArcNodeById(arcId);
	if (node == NULL) return;

	if(isBlink)
	{				
		osg::NodeCallback *callback = node->getUpdateCallback();
		if (callback)
		{
			node->removeUpdateCallback(callback);
		}

		osg::ref_ptr<SectionObjectCallback> secCallback = new SectionObjectCallback(NULL,node);		
		node->addUpdateCallback(secCallback);

	}
	else
	{
		// 关闭光照
		//node->getOrCreateStateSet()->setMode(GL_LIGHTING,osg::StateAttribute::OFF);
		osg::Material *material  = 
			dynamic_cast<osg::Material *>(node->getOrCreateStateSet()->getAttribute(osg::StateAttribute::MATERIAL));
		node->getOrCreateStateSet()->removeAttribute(material);
		//node->getOrCreateStateSet()->setMode(GL_TEXTURE_2D,osg::StateAttribute::OFF);

		osg::NodeCallback *callback = node->getUpdateCallback();
		if (callback)
		{
			node->removeUpdateCallback(callback);			
		}
	}
}

void CGlbGlobeSection::SetArcShow(glbInt32 arcId,glbBool isShow)
{
	if (mpt_node==NULL) return;
	unsigned int childNum = mpt_node->asGroup()->getNumChildren();
	if(childNum < 1) return;
	osg::Group *mtNode = mpt_node->asGroup()->getChild(0)->asGroup();
	if(mtNode == NULL) return;
	childNum = mtNode->getNumChildren();
	std::string arcName = "Arc";
	char temp[16];
	sprintf(temp,"Arc%d",arcId);
	std::string ArcIdName(temp);

	for (unsigned int i = 0; i < childNum ; i++)
	{
		osg::Node* node = mtNode->getChild(i);
		if (node->getName()==arcName)			
		{
			unsigned int arcCnt = node->asGroup()->getNumChildren();
			for (unsigned int j = 0 ; j < arcCnt; j++)
			{
				osg::Node* arcNode = node->asGroup()->getChild(j);
				if (ArcIdName == arcNode->getName())
				{
					if (isShow)
						arcNode->asSwitch()->setAllChildrenOn();
					else
						arcNode->asSwitch()->setAllChildrenOff();
					//if(isShow != node->asSwitch()->getValue(j))		
					//	node->asSwitch()->setValue(j,isShow);	
					break;
				}
			}			
		}
	}	
}

void CGlbGlobeSection::SetPolySelected(glbInt32 polyId,glbBool isSelected)
{
	glbBool curSelected = false;
	std::map<glbInt32,glbBool>::iterator itr = mpr_polyIsSelected.find(polyId);
	if (itr==mpr_polyIsSelected.end())
	{// 没有找到
		mpr_polyIsSelected[polyId] = isSelected;
	}
	else
		curSelected = itr->second;
	// 选中状态没有变化
	if (curSelected==isSelected)
		return;

	itr->second = isSelected;

	osg::Node* polyNode = FindPolyNodeById(polyId);
	if (polyNode==NULL) return;

	osg::ref_ptr<osg::StateSet> stateset = polyNode->getOrCreateStateSet();	
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
		//stateset->setMode(GL_LIGHTING,osg::StateAttribute::OFF);
		stateset->setMode(GL_TEXTURE_2D,osg::StateAttribute::ON);
	}
}

void CGlbGlobeSection::SetPolyBlink(glbInt32 polyId,glbBool isBlink)
{
	osg::Node* node = FindPolyNodeById(polyId);
	if (node == NULL) return;

	if(isBlink)
	{				
		osg::NodeCallback *callback = node->getUpdateCallback();
		if (callback)
		{
			node->removeUpdateCallback(callback);
		}
		osg::ref_ptr<SectionObjectCallback> secCallback = new SectionObjectCallback(NULL,node);		
		node->addUpdateCallback(secCallback);
	}
	else
	{
		// 关闭光照
		//node->getOrCreateStateSet()->setMode(GL_LIGHTING,osg::StateAttribute::OFF);
		osg::Material *material  = 
			dynamic_cast<osg::Material *>(node->getOrCreateStateSet()->getAttribute(osg::StateAttribute::MATERIAL));
		node->getOrCreateStateSet()->removeAttribute(material);
		node->getOrCreateStateSet()->setMode(GL_TEXTURE_2D,osg::StateAttribute::ON);

		osg::NodeCallback *callback = node->getUpdateCallback();
		if (callback)
		{
			node->removeUpdateCallback(callback);
			////取消闪烁之后，处理对象的显示
			//std::map<glbInt32,glbBool>::iterator itr = mpr_stratumSelect.find(stratumId);
			//if (itr!=mpr_stratumSelect.end())			
			//	SetStratumSelected(stratumId,itr->second);			
		}
	}
}

void CGlbGlobeSection::SetPolyShow(glbInt32 polyId,glbBool isShow)
{
	if (mpt_node==NULL) return;
	unsigned int childNum = mpt_node->asGroup()->getNumChildren();
	if(childNum < 1) return;
	osg::Group *mtNode = mpt_node->asGroup()->getChild(0)->asGroup();
	if(mtNode == NULL) return;
	childNum = mtNode->getNumChildren();
	std::string polyName = "Poly";
	char temp[16];
	sprintf(temp,"Poly%d",polyId);
	std::string PolyIdName(temp);
	for (unsigned int i = 0; i < childNum ; i++)
	{
		osg::Node* node = mtNode->getChild(i);
		if (node->getName()==polyName)			
		{
			unsigned int polyCnt = node->asGroup()->getNumChildren();
			for (unsigned int j = 0 ; j < polyCnt; j++)
			{
				osg::Node* polyNode = node->asGroup()->getChild(j);				
				if (polyNode->getName() == PolyIdName)	
				{
					if (isShow)
						polyNode->asSwitch()->setAllChildrenOn();
					else
						polyNode->asSwitch()->setAllChildrenOff();

/*					if(isShow != node->asSwitch()->getValue(j))			
						node->asSwitch()->setValue(j,isShow);*/	
					break;
				}
			}			
		}
	}	
}

osg::Switch* CGlbGlobeSection::FindSectionNode()
{
	if (mpt_node==NULL) return NULL;
	unsigned int childNum = mpt_node->asGroup()->getNumChildren();
	if(childNum < 1) return NULL;
	osg::Group *mtNode = mpt_node->asGroup()->getChild(0)->asGroup();
	std::string secName = "Sec";
	for (unsigned int i = 0; i < mtNode->getNumChildren() ; i++)
	{
		osg::Node* node = mtNode->getChild(i);
		if (node->getName()==secName)
			return node->asSwitch();
	}
	return NULL;
}

osg::Switch* CGlbGlobeSection::FindArcNode()
{
	if (mpt_node==NULL) return NULL;
	unsigned int childNum = mpt_node->asGroup()->getNumChildren();
	if(childNum < 1) return NULL;
	osg::Group *mtNode = mpt_node->asGroup()->getChild(0)->asGroup();
	if(mtNode == NULL) return NULL;
	std::string arcName = "Arc";
	for (unsigned int i = 0; i < mtNode->getNumChildren() ; i++)
	{
		osg::Node* node = mtNode->getChild(i);
		if (node->getName()==arcName)
			return node->asSwitch();
	}
	return NULL;
}

osg::Switch* CGlbGlobeSection::FindPolyNode()
{
	if (mpt_node==NULL) return NULL;
	unsigned int childNum = mpt_node->asGroup()->getNumChildren();
	if(childNum < 1) return NULL;
	osg::Group *mtNode = mpt_node->asGroup()->getChild(0)->asGroup();
	std::string polyName = "Poly";
	for (unsigned int i = 0; i < mtNode->getNumChildren() ; i++)
	{
		osg::Node* node = mtNode->getChild(i);
		if (node->getName()==polyName)
			return node->asSwitch();
	}
	return NULL;
}

osg::Node* CGlbGlobeSection::FindArcNodeById(glbInt32 arcId)
{
	osg::Switch* arcNode = FindArcNode();
	if (arcNode==NULL) return NULL;	
	char temp[16];
	sprintf(temp,"Arc%d",arcId);
	std::string ArcIdName(temp);	
	unsigned int arcCnt = arcNode->getNumChildren();
	for (unsigned int i = 0 ; i < arcCnt; i++)
	{
		osg::Node* childArcNode = arcNode->getChild(i);
		if (ArcIdName == arcNode->getName())
			return childArcNode;					
	}			
	return NULL;
}

osg::Node* CGlbGlobeSection::FindPolyNodeById(glbInt32 polyId)
{
	osg::Switch* polyNode = FindPolyNode();
	if (polyNode==NULL) return NULL;	
	char temp[16];
	sprintf(temp,"Poly%d",polyId);
	std::string PolyIdName(temp);
	
	unsigned int polyCnt = polyNode->getNumChildren();
	for (unsigned int i = 0 ; i < polyCnt; i++)
	{
		osg::Node* childPolyNode = polyNode->getChild(i);				
		if (childPolyNode->getName() == PolyIdName)				
			return childPolyNode;
	}	
	return NULL;
}

//现在直接用OperatorVisitor，此函数现在解析endAlpha的值
void GlbGlobe::CGlbGlobeSection::ParseObjectFadeColor()
{
	if(!mpr_renderInfo.valid())
		return;

	GlbSectionRenderInfo *sectionRenderInfo = dynamic_cast<GlbSectionRenderInfo *>(mpr_renderInfo.get());
	if (sectionRenderInfo == NULL) return;

	if (sectionRenderInfo->isRenderSec)
	{
		//mpt_fadeColor = GetColor(sectionRenderInfo->secColor->GetValue(mpt_feature.get()));
		mpt_fadeEndAlpha = sectionRenderInfo->secOpacity->GetValue(mpt_feature.get());
	}
	else if(sectionRenderInfo->isRenderPoly)
	{
		//mpt_fadeColor = GetColor(sectionRenderInfo->polyColor->GetValue(mpt_feature.get()));
		mpt_fadeEndAlpha = sectionRenderInfo->polyOpacity->GetValue(mpt_feature.get());
	}
	else if(sectionRenderInfo->isRenderArc)
	{
		//mpt_fadeColor = GetColor(sectionRenderInfo->arcColor->GetValue(mpt_feature.get()));
		mpt_fadeEndAlpha = sectionRenderInfo->arcOpacity->GetValue(mpt_feature.get());
	}
}

glbBool GlbGlobe::CGlbGlobeSection::Load2( xmlNodePtr* node,glbWChar* prjPath )
{
	return true;
}
