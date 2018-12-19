#include "StdAfx.h"
#include "GlbGlobeVirtualDrillObject.h"
#include "CGlbGlobe.h"
#include "GlbGlobeView.h"
#include "GlbGlobeMemCtrl.h"
#include "GlbGlobeAutoDrill.h"
#include "GlbGlobeVirtualDrillSymbol.h"
#include "GlbLine.h"
#include <osgUtil/LineSegmentIntersector>
#include <osgUtil/PolytopeIntersector>
#include <osg/Material>
#include <osg/LightModel>
#include "osgDB/ReadFile"

using namespace GlbGlobe;

class VDrillObjectCallback : public osg::NodeCallback
{
public:
	VDrillObjectCallback(CGlbGlobeRObject *obj, osg::Node* stratumNode=NULL)
	{
		mpr_isChangeFrame = false;
		mpr_markNum = 0;	
		CGlbGlobeVirtualDrillObject *drillObject = dynamic_cast<CGlbGlobeVirtualDrillObject *>(obj);		
		mpr_stateset = NULL;
		if (stratumNode==NULL)
		{
			if(drillObject)			
				mpr_stateset = drillObject->GetOsgNode()->getOrCreateStateSet();					
		}
		else
		{
			mpr_stateset = stratumNode->getOrCreateStateSet();
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
			//mpr_stateset->setMode(GL_LIGHTING,osg::StateAttribute::OFF);
		}
	}
private:
	osg::ref_ptr<osg::Material> mpr_material;
	osg::ref_ptr<osg::StateSet> mpr_stateset;
	glbBool mpr_isChangeFrame;
	unsigned int mpr_markNum;
	osg::ref_ptr<osg::Node> stratumNode;
};


#pragma region VSectionTask
// CGlbGlobeVSectionTask
CGlbGlobeVirtualDrillObject::CGlbGlobeVDrillTask::CGlbGlobeVDrillTask( CGlbGlobeVirtualDrillObject *obj,glbInt32 level )
{
	mpr_obj = obj;
	mpr_level = level;
}

CGlbGlobeVirtualDrillObject::CGlbGlobeVDrillTask::~CGlbGlobeVDrillTask( )
{
	mpr_obj = NULL;
}

glbBool CGlbGlobeVirtualDrillObject::CGlbGlobeVDrillTask::doRequest()
{
	if(mpr_obj)
	{
		mpr_obj->ReadData(mpr_level);
		return true;
	}
	return false;
}

CGlbGlobeObject * CGlbGlobeVirtualDrillObject::CGlbGlobeVDrillTask::getObject()
{
	return mpr_obj.get();
}
#pragma endregion VSectionTask

//////////////////////////////////////////////////////////////////////////
// CGlbGlobeVirtualDrillObject接口
CGlbGlobeVirtualDrillObject::CGlbGlobeVirtualDrillObject(void)
{
	mpr_readData_critical.SetName(L"section_readdata");
	mpr_addToScene_critical.SetName(L"section_addscene");
	mpt_altitudeMode = GLB_ALTITUDEMODE_ABSOLUTE; //剖面都是绝对坐标方式
	mpr_needReReadData = false;
	mpr_isFromConstructModel = false;
	mpr_isFromGridModel = false;
	mpr_radius = 100.0;
	mpr_isAuto = false;
	mpr_zScale = 1;
	mpr_zOffset = 0;
}

CGlbGlobeVirtualDrillObject::CGlbGlobeVirtualDrillObject(glbChar* jsonString)
{
	mpr_readData_critical.SetName(L"section_readdata");
	mpr_addToScene_critical.SetName(L"section_addscene");
	mpt_altitudeMode = GLB_ALTITUDEMODE_ABSOLUTE; //剖面都是绝对坐标方式
	mpr_needReReadData = false;
	mpr_isFromConstructModel = false;
	mpr_isFromGridModel = false;
	mpr_radius = 100.0;
	mpr_isAuto = false;
	// 解析jsonString
	mpr_zScale = 1;
	mpr_zOffset = 0;


}

CGlbGlobeVirtualDrillObject::~CGlbGlobeVirtualDrillObject(void)
{
}

GlbGlobeObjectTypeEnum CGlbGlobeVirtualDrillObject::GetType()
{
	return GLB_OBJECTTYPE_VDRILL;
}

glbBool CGlbGlobeVirtualDrillObject::Load(xmlNodePtr* node,const glbWChar* prjPath)
{
	return false;
}

glbBool CGlbGlobeVirtualDrillObject::Load2(xmlNodePtr* node,const glbWChar* prjPath)
{
	return false;
}

glbBool CGlbGlobeVirtualDrillObject::Save(xmlNodePtr node,const glbWChar* prjPath)
{
	return false;
}

glbDouble CGlbGlobeVirtualDrillObject::GetDistance(osg::Vec3d &cameraPos,glbBool isCompute)
{
	if(isCompute == false)
		return mpr_distance;

	if (mpt_globe==NULL)
		return DBL_MAX;

	glbref_ptr<CGlbExtent> ext = GetBound(false);
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

 void CGlbGlobeVirtualDrillObject::LoadData(glbInt32 level)
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
	//glbref_ptr<GlbDrillRenderInfo> renderInfo = dynamic_cast<GlbDrillRenderInfo *>(mpr_renderInfo.get());
	//if(renderInfo == NULL)
	//	return;
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
	glbref_ptr<CGlbGlobeVDrillTask> task = new CGlbGlobeVDrillTask(this,level);
	mpt_globe->mpr_taskmanager->AddTask(task.get());
 }


void CGlbGlobeVirtualDrillObject::AddToScene()
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

glbInt32 CGlbGlobeVirtualDrillObject::RemoveFromScene(glbBool isClean)
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

glbref_ptr<CGlbExtent> CGlbGlobeVirtualDrillObject::GetBound(glbBool isWorld)
{
	if (mpt_globe == NULL && isWorld) return NULL;

	if (mpr_geometry==NULL)
		return NULL;
	glbref_ptr<CGlbExtent> geoExt = const_cast<CGlbExtent *>(mpr_geometry->GetExtent());
	if (geoExt==NULL)
		return NULL;

	if (isWorld == false)
	{//地理坐标
		// minx，miny与maxx，maxy相同
		double minx,miny,minz,maxx,maxy,maxz;
		geoExt->GetMin(&minx,&miny,&minz);
		geoExt->GetMax(&maxx,&maxy,&maxz);
		glbref_ptr<CGlbExtent> newgeoExt = new CGlbExtent(); 
		newgeoExt->Set(minx-0.1,maxx+0.1,miny-0.1,maxy+0.1,minz-0.1,maxz+0.1);
		return newgeoExt;	
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

void CGlbGlobeVirtualDrillObject::UpdateElevate()
{
}

IGlbGeometry *CGlbGlobeVirtualDrillObject::GetOutLine()
{
	return NULL;
}

glbBool CGlbGlobeVirtualDrillObject::SetRenderInfo(GlbRenderInfo *renderInfo,glbBool &isNeedDirectDraw)
{
	return false;
}

GlbRenderInfo *CGlbGlobeVirtualDrillObject::GetRenderInfo()
{
	return mpr_renderInfo.get();
}

glbBool CGlbGlobeVirtualDrillObject::SetShow(glbBool isShow,glbBool isOnState)
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

glbBool CGlbGlobeVirtualDrillObject::SetSelected(glbBool isSelected)
{
	if(mpt_isSelected == isSelected) return true;
	mpt_isSelected = isSelected;
	if (mpt_node != NULL && mpt_altitudeMode!=GLB_ALTITUDEMODE_ONTERRAIN)
	{
		DealSelected();
	}
	return true;
}

glbBool CGlbGlobeVirtualDrillObject::SetBlink(glbBool isBlink)
{
	if (mpt_isBlink == isBlink) return true;
	mpt_isBlink = isBlink;
	if (mpt_node != NULL && mpt_altitudeMode!=GLB_ALTITUDEMODE_ONTERRAIN)
		DealBlink();
	return true;
}

/*
*    DirectDraw 与 LoadData  要互斥分:生产数据
*    mpr_loadState【glbBool】作为互斥分开.
*    Dispatcher线程---在 AddToScene  互斥.
*/
void CGlbGlobeVirtualDrillObject::DirectDraw(glbInt32 level)
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

glbBool CGlbGlobeVirtualDrillObject::SetAltitudeMode(GlbAltitudeModeEnum mode,glbBool &isNeedDirectDraw)
{
	isNeedDirectDraw = false;
	if (mode!=GLB_ALTITUDEMODE_ABSOLUTE)
		return false;
	return true;
}

glbInt32 CGlbGlobeVirtualDrillObject::GetOsgNodeSize()
{
	if (mpt_node == NULL) return 0;
	if (mpr_objSize==0)
		mpr_objSize = ComputeNodeSize(mpt_node);
	return mpr_objSize;
}

void CGlbGlobeVirtualDrillObject::SetJsonString(glbChar* jsonString)
{//网格模型生成的虚拟钻孔
	mpr_isFromConstructModel = false;
	mpr_isFromGridModel = true;
	ParserJson(jsonString);
}

void CGlbGlobeVirtualDrillObject::SetGeometry(IGlbGeometry* geo, glbChar* info, glbBool isUseTexture)
{//结构模型生成的虚拟钻孔
	//if (true)
	//{		
	//	char buff[256];
	//	sprintf(buff,"CGlbGlobeVirtualDrillObject::SetGeometryobj , isUseTexture: %d.\n.",isUseTexture?1:0);
	//	LOG(buff);
	//	LOG(info);
	//}	
	mpr_geometry = geo;
	// info 内数据格式为： 数据集名称（string）,id(32位整型),颜色(32位整型）或纹理路径（string),id(32位整型),颜色(32位整型）或纹理路径（string)......[循环输出geom包含的地层的id和颜色/纹理路径 ]
	mpr_info = info;
	mpr_isFromConstructModel = true;
	mpr_isFromGridModel = false;
	// 解析info
	ParserInfo(isUseTexture);
}

void CGlbGlobeVirtualDrillObject::SetZOffset(glbDouble zOffset)
{
	mpr_zOffset = zOffset;
	if (mpt_node && mpt_node->asSwitch()->getNumChildren()>0 && mpr_geometry!=NULL)
	{//mpr_isFromConstructModel模式： 节点已经生成了，更新矩阵
		CGlbExtent* ext = const_cast<CGlbExtent *>(mpr_geometry->GetExtent());
		if (ext==NULL) return;
		GlbGlobeTypeEnum globeType = mpt_globe->GetType();

		glbDouble cx,cy,cz;
		ext->GetCenter(&cx,&cy,&cz);
		//double newptz = mpr_zOffset;
		if (GLB_GLOBETYPE_GLOBE==GetGlobe()->GetType())						
			g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(cy),osg::DegreesToRadians(cx),cz,cx,cy,cz);
		osg::Vec3d origin_vec(cx,cy,cz);

		// 设置偏移矩阵
		osg::Matrix mOffset;
		osg::Matrix mScale;
		if (globeType == GLB_GLOBETYPE_FLAT)
		{
			mOffset.makeTranslate(0,0,mpr_zOffset);
			mScale.makeScale(1,1,mpr_zScale);
		}
		else
		{			
			double longitude = osg::DegreesToRadians(cx);
			double latitude = osg::DegreesToRadians(cy);
			// Compute up vector
			osg::Vec3d    up      ( cos(longitude)*cos(latitude), sin(longitude)*cos(latitude), sin(latitude));
			// Compute east vector
			osg::Vec3d    east    (-sin(longitude), cos(longitude), 0);
			// Compute north  vector = outer product up x east
			osg::Vec3d    north   = up ^ east;

			north.normalize();
			osg::Vec3 _Offset = east*0 + north*0 + up*mpr_zOffset;
			mOffset.makeTranslate(_Offset);	
			mScale.makeScale(1,1,mpr_zScale);
		}

		osg::MatrixTransform* polyTrans = dynamic_cast<osg::MatrixTransform*>(mpt_node->asSwitch()->getChild(0));
		if (polyTrans)
		{	
			polyTrans->setMatrix(mScale*mOffset);			
			// 自动放缩法线向量比例以适应模型放缩比例 2016.4.21 malin
			polyTrans->getOrCreateStateSet()->setMode( GL_RESCALE_NORMAL, osg::StateAttribute::ON );
		}		
		// 更新场景树
		mpt_globe->mpr_sceneobjIdxManager->UpdateObject(this);
	}
}

glbDouble CGlbGlobeVirtualDrillObject::GetZOffset()
{
	return mpr_zOffset;
}

void CGlbGlobeVirtualDrillObject::SetZScale(glbDouble zScale)
{
	mpr_zScale = zScale;
	if (mpt_node && mpt_node->asSwitch()->getNumChildren()>0 && mpr_geometry!=NULL)
	{//mpr_isFromConstructModel模式： 节点已经生成了，更新矩阵
		CGlbExtent* ext = const_cast<CGlbExtent *>(mpr_geometry->GetExtent());
		if (ext==NULL) return;
		GlbGlobeTypeEnum globeType = mpt_globe->GetType();

		glbDouble cx,cy,cz;
		ext->GetCenter(&cx,&cy,&cz);
		//double newptz = mpr_zOffset;
		if (GLB_GLOBETYPE_GLOBE==GetGlobe()->GetType())						
			g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(cy),osg::DegreesToRadians(cx),cz,cx,cy,cz);
		osg::Vec3d origin_vec(cx,cy,cz);

		// 设置偏移矩阵
		osg::Matrix mOffset;
		osg::Matrix mScale;
		if (globeType == GLB_GLOBETYPE_FLAT)
		{
			mOffset.makeTranslate(0,0,mpr_zOffset);
			mScale.makeScale(1,1,mpr_zScale);
		}
		else
		{			
			double longitude = osg::DegreesToRadians(cx);
			double latitude = osg::DegreesToRadians(cy);
			// Compute up vector
			osg::Vec3d    up      ( cos(longitude)*cos(latitude), sin(longitude)*cos(latitude), sin(latitude));
			// Compute east vector
			osg::Vec3d    east    (-sin(longitude), cos(longitude), 0);
			// Compute north  vector = outer product up x east
			osg::Vec3d    north   = up ^ east;

			north.normalize();
			osg::Vec3 _Offset = east*0 + north*0 + up*mpr_zOffset;
			mOffset.makeTranslate(_Offset);	

			mScale.makeScale(1,1,mpr_zScale);
			//处理位置
			//ComputePosByAltitudeAndGloleType(position);
			//g_ellipsoidModel->computeLocalToWorldTransformFromXYZ(position.x(),position.y(),position.z(),localToWorld);
		}

		osg::MatrixTransform* polyTrans = dynamic_cast<osg::MatrixTransform*>(mpt_node->asSwitch()->getChild(0));
		if (polyTrans)
		{	
			//polyTrans->setMatrix(mScale* localToWorld* mOffset);	
			polyTrans->setMatrix(mScale * mOffset);	
		}		
		// 更新场景树
		mpt_globe->mpr_sceneobjIdxManager->UpdateObject(this);
	}
}

glbDouble CGlbGlobeVirtualDrillObject::GetZScale()
{
	return mpr_zScale;
}

void CGlbGlobeVirtualDrillObject::SetRadius(glbDouble radius)
{
	mpr_radius = radius;
	if (mpt_node)
	{// 已经生成了，需要重绘！
		DirectDraw(0);
	}
}

glbDouble CGlbGlobeVirtualDrillObject::GetRadius()
{
	return mpr_radius;
}

void CGlbGlobeVirtualDrillObject::Update()
{//lod 级数是否需要改变，重新申请grid格网

}

glbBool CGlbGlobeVirtualDrillObject::Pick(glbInt32 mx,glbInt32 my,glbInt32& stratumId)
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
				if (ndName.size()>7)
				{//"Stratum"
					std::string headStr = ndName.substr(0,7);					
					if (headStr=="Stratum")
					{
						std::string numStr = ndName.substr(7,ndName.length()-7);
						stratumId = atoi(numStr.c_str());
						//sscanf_s(ndName.c_str(),"Stratum%d",&stratumId);
						bFindStratum = true;
						break;
					}
				}
			}
		}				
	}	

	if (bFindStratum==false)
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
				osg::Switch* _switch = dynamic_cast<osg::Switch*>(*nitr);
				if (_switch){
					std::string ndName = _switch->getName();
					if (ndName.size()>7)
					{//"Stratum"
						std::string headStr = ndName.substr(0,7);
						if (headStr=="Stratum")
						{
							std::string numStr = ndName.substr(7,ndName.length()-7);
							stratumId = atoi(numStr.c_str());
							//char n[7];
							//sscanf_s(ndName.c_str(),"%s%d",n,&stratumId);
							bFindStratum = true;
							break;
						}
					}
				}
			}		
		}		
	}
	return bFindStratum;
}

void CGlbGlobeVirtualDrillObject::SetStratumSelected(glbInt32 stratumId,glbBool isSelected)
{
	//glbBool curSelected = false;
	//std::map<glbInt32,glbBool>::iterator itr = mpr_stratumSelect.find(stratumId);
	//if (itr==mpr_stratumSelect.end())
	//{// 没有找到
	//	mpr_stratumSelect[stratumId] = isSelected;
	//}
	//else
	//	curSelected = itr->second;
	//// 选中状态没有变化
	//if (curSelected==isSelected)
	//	return;

	osg::Node* stratumNode = FindStratumNodeByID(stratumId);
	if (stratumNode==NULL) return;

	//itr->second = isSelected;

	osg::ref_ptr<osg::StateSet> stateset = stratumNode->getOrCreateStateSet();	
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
		//stratumNode->setStateSet(NULL);
	}
}

void CGlbGlobeVirtualDrillObject::SetStratumBlink(glbInt32 stratumId,glbBool isBlink)
{
	if (mpt_node==NULL) return;
	osg::Node* node = FindStratumNodeByID(stratumId);
	if (node==NULL)
		return;

	if(isBlink)
	{				
		osg::NodeCallback *callback = node->getUpdateCallback();
		if (callback)
		{
			node->removeUpdateCallback(callback);
		}

		osg::ref_ptr<VDrillObjectCallback> drillCallback = new VDrillObjectCallback(NULL,node);		
		node->addUpdateCallback(drillCallback);		
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
			//取消闪烁之后，处理对象的显示
			std::map<glbInt32,glbBool>::iterator itr = mpr_stratumSelect.find(stratumId);
			if (itr!=mpr_stratumSelect.end())			
				SetStratumSelected(stratumId,itr->second);			
		}
	}
}

void CGlbGlobeVirtualDrillObject::SetStratumShow(glbInt32 stratumId,glbBool isShow)
{
	if (mpt_node==NULL) return;	
	if (mpt_node->asGroup()->getNumChildren()<1)	return;	
	osg::Group* mtNode = mpt_node->asGroup()->getChild(0)->asGroup();
	if (mtNode==NULL) return;

	char strName[16];
	sprintf_s(strName,"Stratum%d",stratumId);
	std::string stratumName(strName);
	unsigned int numChild = mtNode->getNumChildren();
	for (unsigned int i = 0; i <numChild; i++)
	{
		osg::Node* node = mtNode->getChild(i);
		std::string name = node->getName();
		if (name == stratumName)
		{// 找到 
/*			if(isShow != node->asSwitch()->getValue(0))			
				node->asSwitch()->setValue(i,isShow);*/		
			if (isShow)
				node->asSwitch()->setAllChildrenOn();
			else
				node->asSwitch()->setAllChildrenOff();
			break;
		}
	}	
}

void CGlbGlobeVirtualDrillObject::SetStratumAuto(glbBool isAuto)
{
	if(mpr_isAuto == isAuto)
		return;
	mpr_isAuto = isAuto;
	//DirectDraw(0);
}

void CGlbGlobeVirtualDrillObject::ReadData(glbInt32 level,glbBool isDirect)
{
	//glbref_ptr<GlbDrillRenderInfo> renderInfo = dynamic_cast<GlbDrillRenderInfo*>(mpr_renderInfo.get());
	if( mpr_geometry == NULL || mpt_altitudeMode != GLB_ALTITUDEMODE_ABSOLUTE )
	{mpt_loadState=false;return;}	

	//CGlbGlobeVirtualDrillSymbol symbol;
	osg::ref_ptr<osg::Node> node = DrawDrill(mpr_geometry.get());//symbol.Draw(this,NULL);
	if (node==NULL){mpt_loadState=false;return;}

	osg::ref_ptr<osg::Switch> swiNode = NULL;
	if(mpr_isAuto)
	{
		osg::Vec3d point;
		glbref_ptr<CGlbExtent> extent = GetBound(false);
		extent->GetCenter(&point.x(),&point.y(),&point.z());

		osg::Matrix mTrans2;
		osg::Vec3d position;	
		if (mpt_globe->GetType()==GLB_GLOBETYPE_GLOBE)
		{
			g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(point.y()),
				osg::DegreesToRadians(point.x()),point.z(),position.x(),position.y(),position.z());			
			g_ellipsoidModel->computeLocalToWorldTransformFromXYZ(position.x(),position.y(),position.z(),mTrans2);
		}
		else// GLB_GLOBETYPE_FLAT
		{
			mTrans2.makeTranslate(point.x(),point.y(),point.z());
		}		

		swiNode = new osg::Switch;
		osg::ref_ptr<osg::MatrixTransform> mt = new osg::MatrixTransform;
		osg::Matrix mTrans=osg::Matrix::inverse(mTrans2);
		mt->setMatrix(mTrans);
		mt->addChild(node);

		osg::ref_ptr<GlbGlobeAutoDrill> autoTransform = new GlbGlobeAutoDrill;
		autoTransform->setAutoRotateMode(GlbGlobeAutoDrill::ROTATE_TO_SCREEN);
		autoTransform->setAutoScaleToScreen(true);
		autoTransform->setMinimumScale(0.0);
		autoTransform->setMaximumScale(FLT_MAX);
		autoTransform->addChild(mt);

		osg::ref_ptr<osg::MatrixTransform> mt2 = new osg::MatrixTransform;
		mt2->setMatrix(mTrans2);
		mt2->addChild(autoTransform);
		swiNode->addChild(mt2.get());
	}
	else
	{
		swiNode = node->asSwitch();
		if (swiNode==NULL){mpt_loadState=false;return;};
	}

	// 默认关闭光照，使用drill本身纹理或颜色绘制
	//swiNode->getOrCreateStateSet()->setMode(GL_LIGHTING,osg::StateAttribute::OFF);

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

void CGlbGlobeVirtualDrillObject::DealSelected()
{

}

void CGlbGlobeVirtualDrillObject::DealBlink()
{

}

glbInt32 CGlbGlobeVirtualDrillObject::ComputeNodeSize(osg::Node *node)
{
	return 0.0;
}

//glbBool CGlbGlobeVirtualDrillObject::DealDrillRenderInfo(GlbDrillRenderInfo* newDrillRenderInfo, glbBool &isNeedDirectDraw)
//{
//	return false;
//}

// 处理中心线或地层开关
void CGlbGlobeVirtualDrillObject::DealDrillBLOrStRenderOrNot(bool renderBaseline, bool renderStratum)
{

}

glbBool CGlbGlobeVirtualDrillObject::ParserJson(glbChar* jsonStr)
{// 解析grid
	return false;
}

glbBool CGlbGlobeVirtualDrillObject::ParserInfo(glbBool isUseTexture)
{
	if (mpr_info.length()<=0)
		return false;
	// 解析info 
	//int len = strlen(info);
	mpr_linesInfo.clear();

	std::string::size_type pos=mpr_info.find_first_of(",");
	if (pos==CGlbString::npos) return false;	
	mpr_tinName = mpr_info.substr(0,pos);
	std::string tempword=mpr_info.substr(pos+1,mpr_info.length()-pos-1); 
	std::string::size_type curpos = 0;	
	std::string idstr;
	std::string colorstr;	
	while((curpos=mpr_info.find_first_of(",",pos+1))!=std::string::npos)
	{
		idstr = mpr_info.substr(pos+1,curpos-(pos+1));
		pos = curpos;
		curpos=mpr_info.find_first_of(",",pos+1);	
		if (curpos==std::string::npos)
		{// 后面没有","了
			colorstr = mpr_info.substr(pos+1,mpr_info.length()-(pos+1));				
		}
		else
			colorstr = mpr_info.substr(pos+1,curpos-(pos+1));
		pos = curpos;

		GlbStratumInfo info;
		glbInt32 id = atoi(idstr.c_str());
		glbInt32 clr = 0xFFFFFFFF;
		if (isUseTexture==false)
			clr = atoi(colorstr.c_str());		
		info.featureOid = id;
		info.color = clr;
		if (isUseTexture)
			info.textureFullPath = colorstr;

		mpr_linesInfo.push_back(info);

		if (curpos==std::string::npos)
			break;
	}

	return true;
}

osg::Node* CGlbGlobeVirtualDrillObject::DrawDrill(IGlbGeometry* geo)
{// 绘制钻孔
	if (mpt_globe==NULL) return NULL;
	GlbGeometryTypeEnum type = geo->GetType();
	if (type != GLB_GEO_MULTILINE)//只接受多线几何
		return NULL;
	GlbGlobeTypeEnum globeType = mpt_globe->GetType();

	osg::ref_ptr<osg::Switch> swNode = new osg::Switch;	
	osg::ref_ptr<osg::MatrixTransform> swMt = new osg::MatrixTransform;
	swNode->addChild(swMt.get());	

	// 计算地层基准偏移点origin_vec
	glbDouble cx,cy,cz;
	geo->GetExtent()->GetCenter(&cx,&cy,&cz);
	if (GLB_GLOBETYPE_GLOBE==globeType)						
		g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(cy),osg::DegreesToRadians(cx),cz,cx,cy,cz);
	osg::Vec3d origin_vec(cx,cy,cz);

	// 设置偏移矩阵
	osg::Matrix mOffset;
	osg::Matrix mScale;
	mScale.makeScale(1,1,mpr_zScale);
	if (globeType == GLB_GLOBETYPE_FLAT)
	{
		mOffset.makeTranslate(0,0,mpr_zOffset);		
	}
	else
	{			
		double longitude = osg::DegreesToRadians(cx);
		double latitude = osg::DegreesToRadians(cy);
		// Compute up vector
		osg::Vec3d    up      ( cos(longitude)*cos(latitude), sin(longitude)*cos(latitude), sin(latitude));
		// Compute east vector
		osg::Vec3d    east    (-sin(longitude), cos(longitude), 0);
		// Compute north  vector = outer product up x east
		osg::Vec3d    north   = up ^ east;

		north.normalize();
		osg::Vec3 _Offset = east*0 + north*0 + up*mpr_zOffset;
		mOffset.makeTranslate(_Offset);			
	}
	swMt->setMatrix(mScale * mOffset);	

	//double ptx,pty,ptz;
	CGlbMultiLine* mln = dynamic_cast<CGlbMultiLine*>(geo);
	glbInt32 lnNum = mln->GetCount();
	for (int idx = 0; idx < lnNum; idx++)
	{
		CGlbLine* ln = const_cast<CGlbLine*>(mln->GetLine(idx));
		if (ln==NULL) continue;
		glbInt32 ptNum = ln->GetCount();
		if (ptNum < 2) continue;

		bool isGlobe = GLB_GLOBETYPE_GLOBE==globeType?true:false;
		double cylinderRadius = mpr_radius;
		osg::Node* node = buildStratumCylinder(ln,isGlobe,cylinderRadius,origin_vec);
		osg::ref_ptr<osg::Switch> stSwiNode = node->asSwitch();

		swMt->addChild(stSwiNode);

		osg::Node* mtNode = stSwiNode->getChild(0);// 在地层节点下一个matrixtransform节点上设置材质颜色
		osg::StateSet* ss = mtNode->getOrCreateStateSet();
		if (idx < mpr_linesInfo.size())
		{			
			char stname[32];
			sprintf_s(stname,"Stratum%d",mpr_linesInfo.at(idx).featureOid);
			stSwiNode->setName(stname);

			if (mpr_linesInfo.at(idx).textureFullPath.length()<=0)
			{// 使用颜色绘制
				osg::ref_ptr<osg::Material> material = dynamic_cast<osg::Material*>(ss->getAttribute(osg::StateAttribute::MATERIAL));
				if (!material.valid())
					material = new osg::Material;
				ss->setAttribute(material.get(),osg::StateAttribute::ON);	
				ss->setMode(GL_DEPTH_TEST,osg::StateAttribute::ON);
				ss->setMode(GL_LIGHTING,osg::StateAttribute::ON);
				osg::Vec4 rColor = GetColor(mpr_linesInfo.at(idx).color);
				//rColor.set(rColor.r(),rColor.g(),rColor.b(),stOpacity/100.0f);
				material->setDiffuse(osg::Material::FRONT_AND_BACK,rColor);
				//material->setAmbient(osg::Material::FRONT_AND_BACK,rColor);
				//material->setSpecular(osg::Material::FRONT_AND_BACK,rColor);
				//material->setEmission(osg::Material::FRONT_AND_BACK,rColor);

				// 双面光照
				osg::ref_ptr<osg::LightModel> lm = new osg::LightModel;
				lm->setTwoSided(true);
				ss->setAttribute(lm.get(), osg::StateAttribute::ON);
			}
			else
			{// 使用纹理绘制
				osg::Image* texImg = osgDB::readImageFile(mpr_linesInfo.at(idx).textureFullPath.c_str());
				if(texImg)
				{
					osg::ref_ptr<osg::Texture2D> tex2d = new osg::Texture2D;
					tex2d->setFilter(osg::Texture::MIN_FILTER,osg::Texture::LINEAR_MIPMAP_LINEAR);
					tex2d->setFilter(osg::Texture::MAG_FILTER,osg::Texture::LINEAR);
					tex2d->setWrap(osg::Texture2D::WRAP_S,osg::Texture2D::REPEAT);
					tex2d->setWrap(osg::Texture2D::WRAP_T,osg::Texture2D::REPEAT);
					tex2d->setMaxAnisotropy(8.0);
					tex2d->setResizeNonPowerOfTwoHint(true);
					tex2d->setImage(texImg);						
					ss->setTextureAttributeAndModes(0, tex2d, osg::StateAttribute::ON );

					ss->setMode(GL_LIGHTING,osg::StateAttribute::OFF);											
				}
			}
		}		
	}
	return swNode.release();
}

osg::Node* CGlbGlobeVirtualDrillObject::buildStratumCylinder(IGlbGeometry * _ln, bool isGlobe, double cylinderRadius,osg::Vec3d originPos)
{
	CGlbLine* ln = dynamic_cast<CGlbLine*>(_ln);
	if (ln==NULL) return NULL;

	// 偏移基准点
	osg::Vec3d local_origin = originPos;
	osg::ref_ptr<osg::MatrixTransform> mt = new osg::MatrixTransform();
	osg::Matrix m;
	m.setTrans(local_origin);
	mt->setMatrix(m);

	//// 保存地层柱体上的点  点的排列方式为quadStrip方式
	glbDouble ptx0,pty0,ptz0,ptx1,pty1,ptz1;
	glbInt32 ptCnt = ln->GetCount();
	for (glbInt32 k = 0; k < ptCnt-1; k++)
	{
		ln->GetPoint(k,&ptx0,&pty0,&ptz0);
		ln->GetPoint(k+1,&ptx1,&pty1,&ptz1);

		if (ptz0 < ptz1)
		{// 交换0-1
			ln->GetPoint(k,&ptx1,&pty1,&ptz1);
			ln->GetPoint(k+1,&ptx0,&pty0,&ptz0);
		}

		if (isGlobe)
		{
			g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(pty0),osg::DegreesToRadians(ptx0),ptz0,
				ptx0,pty0,ptz0);
			g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(pty1),osg::DegreesToRadians(ptx1),ptz1,
				ptx1,pty1,ptz1);
		}		

		osg::Vec3d pt0(ptx0,pty0,ptz0);
		osg::Vec3d pt1(ptx1,pty1,ptz1);		

		// 注意此处必须是osg::Vec3dArray而不能是osg::Vec3Array，否则当cylinderRadius<10时会有浮点误差出现导致圆柱顶点不规则 2016.7.28
		osg::ref_ptr<osg::Vec3dArray> cylinderVertexs = new osg::Vec3dArray();
		osg::ref_ptr<osg::Vec3Array> cylinderNormals = new osg::Vec3Array();
		osg::ref_ptr<osg::Vec2Array> cylinderTexcoords = new osg::Vec2Array();
		computeCylinderVertexs(pt0,pt1,cylinderRadius,cylinderVertexs.get(),cylinderNormals.get(),cylinderTexcoords.get());		

		osg::ref_ptr<osg::Geometry> geom = new osg::Geometry;

		// 重算顶点坐标
		osg::ref_ptr<osg::Vec3Array> _newcylinderVertexs = new osg::Vec3Array();
		_newcylinderVertexs->reserve(cylinderVertexs->size());
		for (size_t n = 0 ; n < cylinderVertexs->size(); n++)
		{
			osg::Vec3d newPt = cylinderVertexs->at(n) - local_origin;
			_newcylinderVertexs->push_back(newPt);
		}

		// 中心像素线
		int lineFirstIndex = cylinderVertexs->size();
		_newcylinderVertexs->push_back(osg::Vec3d(ptx0,pty0,ptz0)-local_origin);
		_newcylinderVertexs->push_back(osg::Vec3d(ptx1,pty1,ptz1)-local_origin);

		osg::Vec3 nor = pt0 - pt1;
		nor.normalize();
		cylinderNormals->push_back(nor);
		cylinderNormals->push_back(nor);

		cylinderTexcoords->push_back(osg::Vec2(0,1));
		cylinderTexcoords->push_back(osg::Vec2(0,0));

		geom->setVertexArray(_newcylinderVertexs);

		geom->setNormalArray(cylinderNormals);	
		geom->setNormalBinding(osg::Geometry::BIND_PER_VERTEX);

		geom->setTexCoordArray(0,cylinderTexcoords/*,osg::Array::BIND_PER_VERTEX*/);

		// 绘制圆柱体
		geom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUAD_STRIP,0,cylinderVertexs->size()));	

		// 只绘制圆柱体的上顶面，以免上下面重叠
		/*	int circlePtCnt = cylinderVertexs->size()/2;
		osg::ref_ptr<osg::UIntArray> pIndexArray = new osg::UIntArray;
		pIndexArray->push_back(lineFirstIndex);
		for (int kk = 0; kk < circlePtCnt; kk++)		
		pIndexArray->push_back(2*kk);
		geom->addPrimitiveSet(new osg::DrawElementsUInt(osg::PrimitiveSet::TRIANGLE_FAN,pIndexArray->size(),&pIndexArray->front()));	
		*/
		{// 绘制圆柱体上顶面
			int topCircleFirstIndex = _newcylinderVertexs->size();
			int circlePtCnt = cylinderVertexs->size()/2;
			
			_newcylinderVertexs->push_back(osg::Vec3d(ptx0,pty0,ptz0)-local_origin);
			cylinderNormals->push_back(nor);
			for (int kk = 0; kk < circlePtCnt; kk++)
			{				
				_newcylinderVertexs->push_back(_newcylinderVertexs->at(2*kk));
				cylinderNormals->push_back(nor);
			}
			int cnt = circlePtCnt+1;
			geom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::TRIANGLE_FAN,topCircleFirstIndex,cnt));
		}
		// 绘制中心线
		geom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINE_STRIP,lineFirstIndex,2));	

		osg::ref_ptr<osg::Geode> geode = new osg::Geode();
		geode->addDrawable(geom.get());
		mt->addChild(geode.get());
	}	

	osg::ref_ptr<osg::Switch> swStratum = new osg::Switch;
	swStratum->addChild(mt.get());
	return swStratum.release();
}

void CGlbGlobeVirtualDrillObject::computeCylinderVertexs(osg::Vec3d pt0, osg::Vec3d pt1, double radius, osg::Vec3dArray* vts, osg::Vec3Array* nors, osg::Vec2Array* texcoords)
{
	osg::Vec3d dir = pt0 - pt1;
	dir.normalize();

	// 计算平面（pt0,dir)上的一点ptPlane, (pt0,ptPlane)矢量planeDir与dir垂直	
	double xx,yy,zz;
	if (fabs(dir.z()) > 0.001)
	{// 接近垂直于xy平面
		xx = pt0.x() + 10;
		yy = pt0.y();
		zz = -(dir.x()*(xx-pt0.x())+dir.y()*(yy-pt0.y()))/dir.z() + pt0.z();
	}
	else if (dir.x() != 0)
	{ 
		yy = pt0.y() + 10;
		zz = pt0.z();
		xx = -(dir.y()*(yy-pt0.y())+dir.z()*(zz-pt0.z()))/dir.x() + pt0.x();
	}
	else
	{
		xx = pt0.x() +10;
		zz = pt0.z();
		yy = -(dir.x()*(xx-pt0.x())+dir.z()*(zz-pt0.z()))/dir.y() + pt0.y();
	}
	osg::Vec3d planeDir(xx-pt0.x(),yy-pt0.y(),zz-pt0.z());
	planeDir.normalize();
	// 按一定弧度实现矢量planeDir绕dir旋转得到矢量A
	osg::Matrixd mt;
	int edges = 18; //(360/20) 分为18段，每段20度
	double v = max(fabs(pt1.z()-pt0.z()) / (radius * 2),1);
	for (int k = 0; k < edges; k++)
	{
		mt.makeRotate(osg::DegreesToRadians(k*360.0/edges),dir);

		osg::Vec3d newrotDir = planeDir * mt;
		newrotDir.normalize();
		osg::Vec3d newPt0 = pt0 + newrotDir * radius; // 上顶面顶点
		osg::Vec3d newPt1 = pt1 + newrotDir * radius; // 下底面顶点
		
		osg::Vec3d nor0 = newPt0-pt0;
		nor0.normalize();
	
		vts->push_back(newPt0);
		nors->push_back(nor0);
		vts->push_back(newPt1);		
		nors->push_back(nor0);

		double u = (double)k / edges * 2*osg::PI;
		osg::Vec2 tex0 = osg::Vec2(u,v);
		osg::Vec2 tex1 = osg::Vec2(u,0);
		texcoords->push_back(tex0);
		texcoords->push_back(tex1);
	}

	vts->push_back(vts->at(0));
	nors->push_back(nors->at(0));
	vts->push_back(vts->at(1));
	nors->push_back(nors->at(1));

	texcoords->push_back(osg::Vec2(2*osg::PI,v));
	texcoords->push_back(osg::Vec2(2*osg::PI,0));
}

osg::Node* CGlbGlobeVirtualDrillObject::FindStratumNodeByID(glbInt32 stratumId)
{
	if (mpt_node==NULL || mpt_node->asGroup()->getNumChildren()<1) return NULL;
	osg::Group* mtNode = mpt_node->asGroup()->getChild(0)->asGroup();
	if (mtNode==NULL) return NULL;

	char strName[16];
	sprintf_s(strName,"Stratum%d",stratumId);
	std::string stratumName(strName);
	unsigned int numChild = mtNode->getNumChildren();
	for (unsigned int i = 0; i <numChild; i++)
	{
		osg::Node* node = mtNode->getChild(i);
		std::string name = node->getName();
		if (name == stratumName)
		{// 找到 				
			return node;
		}
	}
	return NULL;
}

IGlbGeometry* CGlbGlobeVirtualDrillObject::GetGeometry()
{
	return mpr_geometry.get();
}

const glbChar* CGlbGlobeVirtualDrillObject::GetGeometryInfo()
{
	return mpr_info.c_str();
}