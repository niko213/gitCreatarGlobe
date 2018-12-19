#include "StdAfx.h"
#include "GlbGlobeVirtualSectionObject.h"
#include "CGlbGlobe.h"
#include "GlbGlobeView.h"
#include "GlbGlobeMemCtrl.h"
#include "GlbGlobeVirtualSectionSymbol.h"
#include "osgDB/ReadFile"
#include "osgUtil/Tessellator"
#include "osg/LightModel"
#include <osg/Material>

using namespace GlbGlobe;

class VSectionObjectCallback : public osg::NodeCallback
{
public:
	VSectionObjectCallback(CGlbGlobeRObject *obj, osg::NodeList& nlist)
	{
		mpr_isChangeFrame = false;
		mpr_markNum = 0;	
		CGlbGlobeVirtualSectionObject *secObject = dynamic_cast<CGlbGlobeVirtualSectionObject *>(obj);		
		mpr_nodes = nlist;
		mpr_statesets.clear();
		mpr_materials.clear();
		if (mpr_nodes.size()<=0)
		{
			if(secObject)
			{
				osg::StateSet* ss = secObject->GetOsgNode()->getOrCreateStateSet();		
				mpr_statesets.push_back(ss);

				osg::Material *material  = 
					dynamic_cast<osg::Material *>(ss->getAttribute(osg::StateAttribute::MATERIAL));
				if (!material)			
					material = new osg::Material;		
				ss->setAttribute(material,osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);

				mpr_materials.push_back(material);
			}
		}
		else
		{
			for (size_t k = 0 ; k  < mpr_nodes.size(); k++)
			{
				if (mpr_nodes.at(k)==NULL) continue;
				osg::StateSet* ss = mpr_nodes.at(k)->getOrCreateStateSet();
				mpr_statesets.push_back(ss);

				osg::Material *material  = 
					dynamic_cast<osg::Material *>(ss->getAttribute(osg::StateAttribute::MATERIAL));
				if (!material)			
					material = new osg::Material;		
				ss->setAttribute(material,osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);

				mpr_materials.push_back(material);
			}		
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
			for (size_t k = 0 ; k  < mpr_statesets.size(); k++)
			{
				osg::ref_ptr<osg::StateSet> ss = mpr_statesets.at(k);
				osg::ref_ptr<osg::Material> material = mpr_materials.at(k); 
				ss->setMode(GL_LIGHTING,osg::StateAttribute::ON);
				material->setDiffuse(osg::Material::FRONT_AND_BACK,osg::Vec4(1,0,0,1));		
				ss->setAttribute(material,osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
				ss->setMode(GL_TEXTURE_2D,osg::StateAttribute::OFF | osg::StateAttribute::OVERRIDE);
			}		
		}
		else
		{
			for (size_t k = 0 ; k  < mpr_statesets.size(); k++)
			{
				osg::ref_ptr<osg::StateSet> ss = mpr_statesets.at(k);
				osg::Material *material  = 
					dynamic_cast<osg::Material *>(ss->getAttribute(osg::StateAttribute::MATERIAL));
				ss->removeAttribute(material);
				ss->setMode(GL_TEXTURE_2D,osg::StateAttribute::ON);
				ss->setMode(GL_LIGHTING,osg::StateAttribute::OFF);
			}
		}
	}
	~VSectionObjectCallback()
	{
		mpr_nodes.clear();
		mpr_materials.clear();
		mpr_statesets.clear();
	}
private:
	std::vector<osg::ref_ptr<osg::Material>> mpr_materials;
	std::vector<osg::ref_ptr<osg::StateSet>> mpr_statesets;
	glbBool mpr_isChangeFrame;
	unsigned int mpr_markNum;
	osg::NodeList mpr_nodes;
};


#pragma region VSectionTask
// CGlbGlobeVSectionTask
CGlbGlobeVirtualSectionObject::CGlbGlobeVSectionTask::CGlbGlobeVSectionTask( CGlbGlobeVirtualSectionObject *obj,glbInt32 level )
{
	mpr_obj = obj;
	mpr_level = level;
}

CGlbGlobeVirtualSectionObject::CGlbGlobeVSectionTask::~CGlbGlobeVSectionTask( )
{
	mpr_obj = NULL;
}

glbBool CGlbGlobeVirtualSectionObject::CGlbGlobeVSectionTask::doRequest()
{
	if(mpr_obj)
	{
		mpr_obj->ReadData(mpr_level);
		return true;
	}
	return false;
}

CGlbGlobeObject * CGlbGlobeVirtualSectionObject::CGlbGlobeVSectionTask::getObject()
{
	return mpr_obj.get();
}
#pragma endregion VSectionTask

//////////////////////////////////////////////////////////////////////////
// CGlbGlobeVirtualSectionObject�ӿ�
CGlbGlobeVirtualSectionObject::CGlbGlobeVirtualSectionObject(void)
{
	mpr_readData_critical.SetName(L"section_readdata");
	mpr_addToScene_critical.SetName(L"section_addscene");
	mpt_altitudeMode = GLB_ALTITUDEMODE_ABSOLUTE; //���涼�Ǿ������귽ʽ
	mpr_needReReadData = false;
	mpr_zOffset = 0.0;
	mpr_isFromConstructModel = true;
	mpr_isFromGridModel = false;
}

CGlbGlobeVirtualSectionObject::CGlbGlobeVirtualSectionObject(glbChar* jsonString)
{
	mpr_readData_critical.SetName(L"section_readdata");
	mpr_addToScene_critical.SetName(L"section_addscene");
	mpt_altitudeMode = GLB_ALTITUDEMODE_ABSOLUTE; //���涼�Ǿ������귽ʽ
	mpr_needReReadData = false;
	mpr_zOffset = 0.0;
	mpr_isFromConstructModel = false;
	mpr_isFromGridModel = true;
	// ����jsonString

}

CGlbGlobeVirtualSectionObject::~CGlbGlobeVirtualSectionObject(void)
{

}

GlbGlobeObjectTypeEnum CGlbGlobeVirtualSectionObject::GetType()
{
	return GLB_OBJECTTYPE_VSECTION;
}

glbBool CGlbGlobeVirtualSectionObject::Load(xmlNodePtr* node,const glbWChar* prjPath)
{
	return false;
}

glbBool CGlbGlobeVirtualSectionObject::Load2(xmlNodePtr* node,const glbWChar* prjPath)
{
	return false;
}

glbBool CGlbGlobeVirtualSectionObject::Save(xmlNodePtr node,const glbWChar* prjPath)
{
	return false;
}

glbDouble CGlbGlobeVirtualSectionObject::GetDistance(osg::Vec3d &cameraPos,glbBool isCompute)
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

 void CGlbGlobeVirtualSectionObject::LoadData(glbInt32 level)
 {
	if(mpt_currLevel == level) return;
	if(mpt_preNode  !=  NULL)
	{//�ɶ���δѹ��ժ����
		/*
		*   mpt_node = node1,preNode = NULL    -----> node1_h
		*   �������ƣ���LoadData��
		*   {
		*       mpt_node = node2,preNode = node1
		*       mpt_node = node3,preNode = node2   -----> node1_h,node2_r,node2_h
		*       ���node1 ��Զ����ժ��
		*   }
		*/
		return;
	}
	glbref_ptr<GlbSectionRenderInfo> renderInfo = dynamic_cast<GlbSectionRenderInfo *>(mpr_renderInfo.get());
	if(renderInfo == NULL)
		return;
	// ��3Dģ�����ͣ����Ѿ�load�����Ͳ����ٴ�load
	if (mpt_currLevel != -1) 	
		return;
	{//��DirectDraw����.
		GlbScopedLock<GlbCriticalSection> lock(mpr_readData_critical);
		if (mpt_loadState == true)return;
		mpt_loadState = true;
	}		
	if(mpt_currLevel != level)
	{
		if(mpt_currLevel < level)
			level = mpt_currLevel + 1;
	}
	glbref_ptr<CGlbGlobeVSectionTask> task = new CGlbGlobeVSectionTask(this,level);
	mpt_globe->mpr_taskmanager->AddTask(task.get());
 }


void CGlbGlobeVirtualSectionObject::AddToScene()
{
	/*
	*   mpt_isInHangTasks = true ------ ���� AddToScene���߳� (A)
	*   mpt_isInHangTasks = false ----- frame �߳�            (B)
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
	{//û�в�����osg�ڵ�
		if(mpt_HangTaskNum == 0
		   && mpt_node->getNumParents() == 0
		   && mpt_altitudeMode != GLB_ALTITUDEMODE_ONTERRAIN)
		{//����ģʽ,�ڵ㲻��Ҫ��.
			//��mpt_node����
			CGlbGlobeCallBack::CGlbGlobeCallBackTask* task = 
				new CGlbGlobeCallBack::CGlbGlobeCallBackTask(this,mpt_node);
			task->mpr_size = this->GetOsgNodeSize();				
			mpt_globe->mpr_p_callback->AddHangTask(task);
			mpt_HangTaskNum ++;
			needUpdate = true;
		}			
	}
	else
	{//��������osg�ڵ�						
		{//�ɽڵ��Ѿ��ҵ�������
			//ժ�ɽڵ�mpt_preNode������
			CGlbGlobeCallBack::CGlbGlobeCallBackTask* task = 
				new CGlbGlobeCallBack::CGlbGlobeCallBackTask(this,mpt_preNode);
			task->mpr_size = ComputeNodeSize(mpt_preNode);
			mpt_globe->mpr_p_callback->AddRemoveTask(task);
			mpt_preNode = NULL;
			CGlbGlobeMemCtrl::GetInstance()->DecrementUsedMem(task->mpr_size);
		}
		if(mpt_altitudeMode != GLB_ALTITUDEMODE_ONTERRAIN)
		{//���½ڵ�mpt_node����
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

glbInt32 CGlbGlobeVirtualSectionObject::RemoveFromScene(glbBool isClean)
{
	{//��LoadData����.
		GlbScopedLock<GlbCriticalSection> lock(mpr_readData_critical);
		if (mpt_loadState == true)return 0;	
		mpt_loadState = true;
		/*
		*    mpt_node = node1,pre=NULL
		*    ��:mpt_node = node2,pre=node1		
		*/
	}
	GlbScopedLock<GlbCriticalSection> lock(mpr_addToScene_critical);
	
	glbInt32 tsize = 0;
	if(isClean == false)
	{//���Դ�ж�ض��󣬽�Լ�Դ�.
		if ((mpt_node != NULL))// && (mpt_node->getNumParents() != 0))
		{//obj���Դ�	
			/*
			     mpt_node �Ѿ��ڹҶ��У����ǻ�û�ҵ���������
				 ��ʱ�ж�getNumParents() != 0 �ǲ����Ե�.
			*/
			CGlbGlobeCallBack::CGlbGlobeCallBackTask *task = 
				new CGlbGlobeCallBack::CGlbGlobeCallBackTask(this,mpt_node);
			task->mpr_size = this->GetOsgNodeSize();
			mpt_globe->mpr_p_callback->AddRemoveTask(task);
			tsize = task->mpr_size;
		}
	}
	else
	{//���ڴ���ж�ض��󣬽�Լ�ڴ�
		//ɾ�� ��һ��װ�صĽڵ�		
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
		//ɾ����ǰ�ڵ�
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
		// ���󣬲��ܼ������߼�!!!2017.2.13 ml
		//if (mpt_feature != NULL && mpt_featureLayer != NULL)
		//{
		//	CGlbGlobeFeatureLayer* fl = dynamic_cast<CGlbGlobeFeatureLayer*>(mpt_featureLayer);
		//	if (fl)
		//		fl->NotifyFeatureDelete(mpt_feature->GetOid());
		//}
		mpt_currLevel = -1;					
	}
	mpt_loadState = false;
	//����ʹ���ڴ�
	if(tsize>0 && isClean)
	{
		CGlbGlobeMemCtrl::GetInstance()->DecrementUsedMem(tsize);		
	}
	return tsize;	
}

glbref_ptr<CGlbExtent> CGlbGlobeVirtualSectionObject::GetBound(glbBool isWorld)
{
	if (mpt_globe == NULL && isWorld) return NULL;

	if (mpr_geometry==NULL || mpt_globe==NULL)
		return NULL;
	glbref_ptr<CGlbExtent> geoExt = const_cast<CGlbExtent *>(mpr_geometry->GetExtent());
	if (geoExt==NULL)
		return NULL;

	if (isWorld == false)
	{//��������
		return geoExt;	
	}
	else
	{
		glbref_ptr<CGlbExtent> worldExt = new CGlbExtent();
		osg::ref_ptr<osg::Node> node = mpt_node;
		if(   node != NULL
			&&node->getBound().valid() )
		{
			//����1
			//CGlbCalculateBoundBoxVisitor bboxV;
			//mpt_node->accept(bboxV);
			//osg::BoundingBoxd bb =bboxV.getBoundBox();
			//����2
			osg::BoundingSphere bs = node->getBound();
			osg::BoundingBoxd bb;
			bb.expandBy(bs._center - osg::Vec3d(bs._radius,bs._radius,bs._radius));
			bb.expandBy(bs._center + osg::Vec3d(bs._radius,bs._radius,bs._radius));
			worldExt->SetMin(bb.xMin(),bb.yMin(),bb.zMin());
			worldExt->SetMax(bb.xMax(),bb.yMax(),bb.zMax());
		}
		else
		{//osg �ڵ㻹û����.
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

void CGlbGlobeVirtualSectionObject::UpdateElevate()
{
}

IGlbGeometry *CGlbGlobeVirtualSectionObject::GetOutLine()
{
	return NULL;
}

glbBool CGlbGlobeVirtualSectionObject::SetRenderInfo(GlbRenderInfo *renderInfo,glbBool &isNeedDirectDraw)
{
	return false;
}

GlbRenderInfo *CGlbGlobeVirtualSectionObject::GetRenderInfo()
{
	return mpr_renderInfo.get();
}

glbBool CGlbGlobeVirtualSectionObject::SetShow(glbBool isShow,glbBool isOnState)
{
	if (mpt_node==NULL && isOnState==true)
		return false;

	if (mpt_isEdit && isOnState)// �༭״̬�����ɵ��ȿ�������
		return true;

	glbBool isold = mpt_isDispShow && mpt_isShow;

	if(isOnState)
	{// ��������������
		mpt_isDispShow = isShow;
	}
	else
	{// �û���������
		mpt_isShow = isShow;
	}

	glbBool isnew = mpt_isDispShow && mpt_isShow;

	if(isold == isnew)
		return true;

	glbref_ptr<GlbSectionRenderInfo> renderInfo = dynamic_cast<GlbSectionRenderInfo *>(mpr_renderInfo.get());
	if (!renderInfo) return false;

	//if( mpt_altitudeMode == GLB_ALTITUDEMODE_ONTERRAIN )
	//{// ��������ػ��ƶ�����Ҫˢ����������
	//	/*
	//	* mpr_isCanDrawImage������DrawToImageʱ�Ƿ���Ҫ����
	//	*/
	//	if (mpr_isCanDrawImage != isnew)
	//	{
	//		mpr_isCanDrawImage = isnew;
	//		// ˢ����������
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

glbBool CGlbGlobeVirtualSectionObject::SetSelected(glbBool isSelected)
{
	if(mpt_isSelected == isSelected) return true;
	mpt_isSelected = isSelected;
	if (mpt_node != NULL && mpt_altitudeMode!=GLB_ALTITUDEMODE_ONTERRAIN)
	{
		DealSelected();
	}
	return true;
}

glbBool CGlbGlobeVirtualSectionObject::SetBlink(glbBool isBlink)
{
	if (mpt_isBlink == isBlink) return true;
	mpt_isBlink = isBlink;
	if (mpt_node != NULL && mpt_altitudeMode!=GLB_ALTITUDEMODE_ONTERRAIN)
		DealBlink();
	return true;
}

/*
*    DirectDraw �� LoadData  Ҫ�����:��������
*    mpr_loadState��glbBool����Ϊ����ֿ�.
*    Dispatcher�߳�---�� AddToScene  ����.
*/
void CGlbGlobeVirtualSectionObject::DirectDraw(glbInt32 level)
{
	if (mpt_parent    == NULL) return;		
	{//��LoadData����.
		GlbScopedLock<GlbCriticalSection> lock(mpr_readData_critical);
		if (mpt_loadState == true)
		{
			/*
			    LoadData->ReadData��ѡ���˳������ǻ�����������⣺
				        LoadData->ReadData ʹ�õ��Ǿɵ���Ⱦ��ص����ԣ����ܷ�ӳ�����õ���Ⱦ����.
				������ֱ���˳������ѡ��ȴ�LoadData->ReadData�Ļ�������������̣߳���������

				��������һ����־��ReadData ִ�е�ĩβʱ�����ñ�־,�����ʾ��true��ReadData��ִ��һ��.
			*/
			mpr_needReReadData = true;
			return;
		}
		mpt_loadState = true;
	}
	//if (true)
	//{					
	//	char buff[256];
	//	sprintf(buff,"CGlbGlobeVirtualSectionObject::DirectDraw pre AddToScene. mpt_preNode is NULL? %d.\n",(mpt_preNode==NULL) ? 1 : 0);
	//	LOG(buff);			
	//}
	if(mpt_preNode != NULL)
	{
	/*
	*    mpr_node = node2,preNode = node1  ----A�̵߳���AddToScene��
	*                                         ��û��preNode Remove��ûִ����
	*    ����ReadData �Ѿ����꣺
	*    mpr_ndoe = node3,preNode = node2   ���node1��Զ����ժ��.
	*/
		AddToScene();
	}
	/*
	*    ��Ϊ��LoadData->ReadDataʱ��ReadData װ�ض�����ֳ����ڴ��޶�
	*        �������¼��ص�osg�ڵ�.
	*    ����DirectDraw���ԣ���������µ�osg�ڵ㣬�Ͳ��ܷ�ӳ���µı仯.
	*    ���ԣ�
	*         ReadData��������һ�������������DirectDraw���õ�,�����Ƿ񳬹�
	*         �ڴ��޶�,���������µ�osg�ڵ�.
	*/
	ReadData(level,true);
	AddToScene();
}

glbBool CGlbGlobeVirtualSectionObject::SetAltitudeMode(GlbAltitudeModeEnum mode,glbBool &isNeedDirectDraw)
{
	isNeedDirectDraw = false;
	if (mode!=GLB_ALTITUDEMODE_ABSOLUTE)
		return false;
	return true;
}

glbInt32 CGlbGlobeVirtualSectionObject::GetOsgNodeSize()
{
	if (mpt_node == NULL) return 0;
	if (mpr_objSize==0)
		mpr_objSize = ComputeNodeSize(mpt_node);
	return mpr_objSize;
}

void CGlbGlobeVirtualSectionObject::SetJsonString(char* jsonString)
{//����ģ�����ɵ���������
	mpr_isFromConstructModel = false;
	mpr_isFromGridModel = true;
}

void CGlbGlobeVirtualSectionObject::SetGeometry(IGlbGeometry* geo, glbChar* info, glbBool isUseTexture)
{//�ṹģ�����ɵ���������
	//if (true)
	//{		
	//	char buff[256];
	//	sprintf(buff,"CGlbGlobeVirtualSectionObject::SetGeometryobj , isUseTexture: %d.\n.",isUseTexture?1:0);
	//	LOG(buff);
	//	LOG(info);
	//}	

	mpr_geometry = geo;
	mpr_info = info;
	mpr_isFromConstructModel = true;
	mpr_isFromGridModel = false;

	// ����info 
	ParserInfo(isUseTexture);
}

glbBool CGlbGlobeVirtualSectionObject::ParserInfo(glbBool isUseTexture)
{
	if (mpr_info.length()<=0)
		return false;
	// ����info 
	//int len = strlen(info);
	mpr_polysInfo.clear();

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
		{// ����û��","��
			colorstr = mpr_info.substr(pos+1,mpr_info.length()-(pos+1));				
		}
		else
			colorstr = mpr_info.substr(pos+1,curpos-(pos+1));
		pos = curpos;

		GlbPolygonInfo info;
		glbInt32 id = atoi(idstr.c_str());
		glbInt32 clr = 0xFFFFFFFF;
		if (isUseTexture==false)
			clr = atoi(colorstr.c_str());		
		info.featureOid = id;
		info.color = clr;
		if (isUseTexture)
			info.texture_name = colorstr;

		mpr_polysInfo.push_back(info);

		if (curpos==std::string::npos)
			break;
	}

	//if(true)
	//{//�������
	//	CGlbMultiPolygon* mulPoly = dynamic_cast<CGlbMultiPolygon*>(mpr_geometry.get());
	//	char buff[256];
	//	sprintf(buff,"CGlbGlobeVirtualSectionObject::ParserInfo end,polyNum: %d.\n.",mulPoly->GetCount());
	//	LOG(buff);			
	//}
	return true;
}

void CGlbGlobeVirtualSectionObject::SetZOffset(glbDouble zoffset)
{	
	//if (true)
	//{		
	//	LOG("CGlbGlobeVirtualSectionObject::SetZOffset begin.\n");			
	//}	
	// ��ԭֵ����
	//if (fabs(mpr_zOffset - zoffset) < 0.0001) return;
	mpr_zOffset = zoffset;
	if (mpt_node && mpt_node->asSwitch()->getNumChildren()>0 && mpr_geometry!=NULL)
	{//mpr_isFromConstructModelģʽ�� �ڵ��Ѿ������ˣ����¾���
		CGlbExtent* ext = const_cast<CGlbExtent *>(mpr_geometry->GetExtent());
		if (ext==NULL) return;
		glbDouble ptx,pty,ptz;
		ext->GetCenter(&ptx,&pty,&ptz);
		double newptz = ptz+mpr_zOffset;
		if (GLB_GLOBETYPE_GLOBE==GetGlobe()->GetType())						
			g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(pty),osg::DegreesToRadians(ptx),newptz,ptx,pty,newptz);

		osg::MatrixTransform* polyTrans = dynamic_cast<osg::MatrixTransform*>(mpt_node->asSwitch()->getChild(0));
		if (polyTrans)
		{			
			osg::Matrix m;
			m.setTrans(osg::Vec3d(ptx,pty,newptz));		
			polyTrans->setMatrix(m);			
		}

		// ���³�����
		mpt_globe->mpr_sceneobjIdxManager->UpdateObject(this);
	}
	//if (true)
	//{		
	//	LOG("CGlbGlobeVirtualSectionObject::SetZOffset end.\n");			
	//}	
}

glbDouble CGlbGlobeVirtualSectionObject::GetZOffset()
{
	return mpr_zOffset;
}

void CGlbGlobeVirtualSectionObject::Update()
{
	if (mpr_isFromGridModel)
	{// �߾�����ģʽ�Ŵ���Update

	}
}

glbBool CGlbGlobeVirtualSectionObject::PickPoly(int mx,int my,glbInt32& polyId)
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

	//����������
	osg::ref_ptr< osgUtil::LineSegmentIntersector > picker = new osgUtil::LineSegmentIntersector(vStart, vEnd);
	osgUtil::IntersectionVisitor iv(picker.get());
	mpt_node->accept(iv);
	if (picker->containsIntersections())
	{
		osgUtil::LineSegmentIntersector::Intersections& intersections = picker->getIntersections();		
		const osgUtil::LineSegmentIntersector::Intersection& hit = *(intersections.begin());
		std::string name = hit.drawable->getName();
		if (name.length()>0)
		{
			glbInt32 id;
			sscanf_s(name.c_str(),"%d",&id);
			polyId = id;				
			bFindPoly = true;
		}		
	}	
	return bFindPoly;
}

void CGlbGlobeVirtualSectionObject::SetPolySelected(glbInt32 polyId,glbBool isSelected)
{
	if (mpt_node == NULL) return;
	if (mpr_oidNodeMap.size()<=0)return;
	std::map<glbInt32/*oid*/,osg::NodeList>::iterator iter = mpr_oidNodeMap.find(polyId);
	if (iter != mpr_oidNodeMap.end())
	{
		osg::NodeList nlist = iter->second;
		for(size_t k = 0; k < nlist.size(); k++)
		{
			osg::Node* node = nlist.at(k);
			if (node == NULL) continue;
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
				stateset->setMode(GL_TEXTURE_2D,osg::StateAttribute::ON);
			}
		}
	}
}

void CGlbGlobeVirtualSectionObject::SetPolyBlink(glbInt32 polyId,glbBool isBlink)
{
	if (mpt_node == NULL) return;
	if (mpr_oidNodeMap.size()<=0)return;
	std::map<glbInt32/*oid*/,osg::NodeList>::iterator iter = mpr_oidNodeMap.find(polyId);
	if (iter == mpr_oidNodeMap.end())
		return;

	if(isBlink)
	{				
		osg::NodeCallback *callback = mpt_node->getUpdateCallback();
		if (callback)
		{
			mpt_node->removeUpdateCallback(callback);
		}
		osg::ref_ptr<VSectionObjectCallback> secCallback = new VSectionObjectCallback(NULL,iter->second);		
		mpt_node->addUpdateCallback(secCallback);
	}
	else
	{
		// �رչ���
		//node->getOrCreateStateSet()->setMode(GL_LIGHTING,osg::StateAttribute::OFF);
		osg::NodeList nlist = iter->second;
		for(size_t k = 0; k < nlist.size(); k++)
		{
			osg::Node* node = nlist.at(k);
			if (node == NULL) continue;
			osg::Material *material  = 
				dynamic_cast<osg::Material *>(node->getOrCreateStateSet()->getAttribute(osg::StateAttribute::MATERIAL));
			node->getOrCreateStateSet()->removeAttribute(material);
			node->getOrCreateStateSet()->setMode(GL_TEXTURE_2D,osg::StateAttribute::ON);
		}

		osg::NodeCallback *callback = mpt_node->getUpdateCallback();
		if (callback)
		{
			mpt_node->removeUpdateCallback(callback);
			////ȡ����˸֮�󣬴���������ʾ
			//std::map<glbInt32,glbBool>::iterator itr = mpr_stratumSelect.find(stratumId);
			//if (itr!=mpr_stratumSelect.end())			
			//	SetStratumSelected(stratumId,itr->second);			
		}
	}
}

void CGlbGlobeVirtualSectionObject::SetPolyShow(glbInt32 polyId,glbBool isShow)
{
	if (mpr_oidNodeMap.size()<=0)return;
	std::map<glbInt32/*oid*/,osg::NodeList>::iterator iter = mpr_oidNodeMap.find(polyId);
	if (iter != mpr_oidNodeMap.end())
	{
		osg::NodeList nlist = iter->second;
		for(size_t k = 0; k < nlist.size(); k++)
		{
			osg::Node* node = nlist.at(k);
			if (node == NULL) continue;

			if (isShow)
				node->setNodeMask(0xffffffff);
			else
				node->setNodeMask(0x0);
		}
	}
}

void CGlbGlobeVirtualSectionObject::ReadData(glbInt32 level,glbBool isDirect)
{
	//glbref_ptr<GlbSectionRenderInfo> renderInfo = dynamic_cast<GlbSectionRenderInfo*>(mpr_renderInfo.get());
	//if( renderInfo == NULL || mpt_altitudeMode != GLB_ALTITUDEMODE_ABSOLUTE )
	//{mpt_loadState=false;return;}

	//if (renderInfo == NULL) {mpt_loadState=false;return;}
	if (mpt_altitudeMode != GLB_ALTITUDEMODE_ABSOLUTE) return;
	
	//CGlbGlobeVirtualSectionSymbol symbol;
	osg::ref_ptr<osg::Node> node = NULL;
	if (mpr_isFromConstructModel)
		node = DrawSection(mpr_geometry.get());//symbol.Draw(this,NULL);
	if (node==NULL){mpt_loadState=false;return;}
	
	osg::Switch* swiNode = node->asSwitch();
	//if (swiNode)
	//	DealModelOpacity(swiNode,renderInfo);
	if(mpr_needReReadData)
	{
		/*
		*   �ڼ��mpr_needReReadDataʱ,�����߳�����mpr_needReReadData=true��ûִ�У�����.
		*/
		mpr_needReReadData = false;
		return ReadData(level,true);
	}

	if (swiNode==NULL)
		return;
	//if (mpt_altitudeMode != GLB_ALTITUDEMODE_ONTERRAIN)
	//{// ������ģʽ��Ҫ�����ڴ�����
		//����ʹ���ڴ�		
		glbInt32 objsize = ComputeNodeSize(swiNode);
		glbBool isOK = CGlbGlobeMemCtrl::GetInstance()->IncrementUsedMem(objsize);
		if(isOK||isDirect)
		{
			// Ԥ�ȼ���bound����ʡʱ��
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
void CGlbGlobeVirtualSectionObject::DealSelected()
{
	if (mpt_node == NULL) return;

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

void CGlbGlobeVirtualSectionObject::DealBlink()
{
	if(mpt_isBlink)
	{
		osg::NodeCallback *callback = mpt_node->getUpdateCallback();
		if (callback)
		{
			mpt_node->removeUpdateCallback(callback);
		}
		osg::NodeList nlist;
		osg::ref_ptr<VSectionObjectCallback> secCallback = new VSectionObjectCallback(this,nlist);
		mpt_node->addUpdateCallback(secCallback);
	}
	else
	{
		// �رչ���
		//mpt_node->getOrCreateStateSet()->setMode(GL_LIGHTING,osg::StateAttribute::OFF);
		osg::Material *material  = 
			dynamic_cast<osg::Material *>(mpt_node->getOrCreateStateSet()->getAttribute(osg::StateAttribute::MATERIAL));
		mpt_node->getOrCreateStateSet()->removeAttribute(material);
		mpt_node->getOrCreateStateSet()->setMode(GL_TEXTURE_2D,osg::StateAttribute::ON);

		osg::NodeCallback *callback = mpt_node->getUpdateCallback();
		if (callback)
		{
			mpt_node->removeUpdateCallback(callback);
			//ȡ����˸֮�󣬴���������ʾ
			DealSelected();
		}
	}
}
glbInt32 CGlbGlobeVirtualSectionObject::ComputeNodeSize(osg::Node *node)
{
	return 0.0;
}

glbBool CGlbGlobeVirtualSectionObject::DealSectionRenderInfo(GlbSectionRenderInfo* newSectionRenderInfo, glbBool &isNeedDirectDraw)
{
	return false;
}

glbBool CGlbGlobeVirtualSectionObject::DealSectionArcPolyONOFF(glbBool isSecShow, glbBool isArcShow, glbBool isPolyShow)
{
	return false;
}

void CGlbGlobeVirtualSectionObject::DealSectionColor(GlbRenderColor* clr)
{

}
void CGlbGlobeVirtualSectionObject::DealSectionOpacity(GlbRenderInt32* opacity)
{

}

void CGlbGlobeVirtualSectionObject::DealArcColor(GlbRenderColor* clr)
{

}
void CGlbGlobeVirtualSectionObject::DealArcOpacity(GlbRenderInt32* opacity)
{

}
void CGlbGlobeVirtualSectionObject::DealArcWidth(GlbRenderDouble* width)
{

}

void CGlbGlobeVirtualSectionObject::DealPolyColor(GlbRenderColor* clr)
{

}
void CGlbGlobeVirtualSectionObject::DealPolyOpacity(GlbRenderInt32* opacity)
{

}

glbBool CGlbGlobeVirtualSectionObject::ParserJson(glbChar* jsonStr)
{
	return false;
}

osg::Node* CGlbGlobeVirtualSectionObject::DrawSection(IGlbGeometry* geo/*, const glbChar* info*/)
{	
	if (geo == NULL) return NULL;
	GlbGeometryTypeEnum geomType = geo->GetType();
	// �����Ƕ����μ���
	if (geomType != GLB_GEO_MULTIPOLYGON)
		return NULL;

	CGlbExtent* ext = const_cast<CGlbExtent *>(geo->GetExtent());
	if (ext==NULL)
		return NULL;

	double uLen = sqrt(ext->GetXWidth()*ext->GetXWidth()+ext->GetYHeight()*ext->GetYHeight());
	double vLen = ext->GetZLengh();
	// ���map
	mpr_oidNodeMap.clear();

	// Ĭ�����������
	//mpr_zOffset = ext->GetZLengh()*0.55;

	GlbGlobeTypeEnum globeType = GetGlobe()->GetType();
	double ptx,pty,ptz;
	std::vector<glbInt32> polyOid;
	osg::Switch* polyNode = new osg::Switch();
	// �����еĶ����	
	CGlbMultiPolygon		*mulPoly= dynamic_cast<CGlbMultiPolygon*>(geo);					
	if (mulPoly)
	{		
		// ����Ĭ�ϵ������ظ�ģʽ��UVֵ
		GlbGlobeTexRepeatModeEnum polyTexRMode = GLB_TEXTUREREPEAT_TIMES;
		glbInt32 mulpolyTilingU = 2;
		glbInt32 mulpolyTilingV = 2*vLen/uLen;

		ext->GetCenter(&ptx,&pty,&ptz);			
		double newptz = ptz+mpr_zOffset;
		if (GLB_GLOBETYPE_GLOBE==globeType)						
			g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(pty),osg::DegreesToRadians(ptx),newptz,ptx,pty,newptz);

		osg::ref_ptr<osg::MatrixTransform> polyTrans = new osg::MatrixTransform;
		osg::Matrix m;
		osg::Vec3d origVec(ptx,pty,ptz);
		m.setTrans(osg::Vec3d(ptx,pty,newptz));		
		polyTrans->setMatrix(m);

		polyNode->addChild(polyTrans);		

		glbInt32 polyCnt = mulPoly->GetCount();		
		glbInt32 featureOid;
		glbInt32 geomColor;
		char strfeatureOid[64];
		for (glbInt32 idx = 0; idx < polyCnt; idx++)
		{
			CGlbPolygon* poly = const_cast<CGlbPolygon *>(mulPoly->GetPolygon(idx));
			CGlbLine* extRing = const_cast<CGlbLine *>(poly->GetExtRing());
			glbInt32 ptCnt = extRing->GetCount();
			if (ptCnt<3) continue;

			CGlbExtent* polyExt = const_cast<CGlbExtent *>(poly->GetExtent());
			double _uLen = sqrt(polyExt->GetXWidth()*polyExt->GetXWidth()+polyExt->GetYHeight()*polyExt->GetYHeight());
			double _vLen = ext->GetZLengh();			
		
			glbInt32 polyTilingU = 5 * _uLen / uLen;
			glbInt32 polyTilingV = 5 * _vLen /vLen;
	
			osg::ref_ptr<osg::Geode> geode = new osg::Geode;
			polyTrans->addChild(geode);	

			osg::ref_ptr<osg::Geometry> geom = new osg::Geometry;
			geode->addDrawable(geom);
			osg::ref_ptr<osg::Vec3Array> vtArray = new osg::Vec3Array;
			geom->setVertexArray(vtArray);
			osg::ref_ptr<osg::Vec4Array> clrArray = new osg::Vec4Array;	
			geom->setColorArray(clrArray);
			geom->setColorBinding(osg::Geometry::BIND_OVERALL);
			osg::ref_ptr<osg::Vec2Array> texArray = new osg::Vec2Array;
			geom->setTexCoordArray(0,texArray);
			osg::ref_ptr<osg::Vec3Array> normArray = new osg::Vec3Array;
			geom->setNormalArray(normArray);
			geom->setNormalBinding(osg::Geometry::BIND_OVERALL);

			osg::Vec3 nor = CaculateTessellationNormal(poly,globeType);//CaculatePolyNormal(poly,globeType);
			normArray->push_back(nor);

			std::string texture_name;
			if (idx < (glbInt32)mpr_polysInfo.size())
			{
				featureOid = mpr_polysInfo.at(idx).featureOid;
				geomColor = mpr_polysInfo.at(idx).color;
				texture_name = mpr_polysInfo.at(idx).texture_name;
				sprintf_s(strfeatureOid,"%d",featureOid);
				mpr_polysInfo[idx].drawable = geom.get();
			}
			else
			{
				featureOid = -1;
				geomColor  = RGB(255,0,0);
				sprintf_s(strfeatureOid,"%d",featureOid);
				mpr_polysInfo[idx].drawable = NULL;
				//stringstream s;
				//s << featureOid;
				//s >> strfeatureOid;
			}
			geom->setName(strfeatureOid);
			geode->setName(strfeatureOid);

			osg::Vec4 clr = GetColor(geomColor);
			clrArray->push_back(clr);

			if (featureOid != -1)
			{
				std::map<glbInt32/*oid*/,osg::NodeList>::iterator iter =  mpr_oidNodeMap.find(featureOid);
				if (iter == mpr_oidNodeMap.end())
				{// û�ҵ�
					osg::NodeList nlist;
					nlist.push_back(geode);
					mpr_oidNodeMap[featureOid] = nlist;
				}
				else
				{// �ҵ��ˣ���geode�ӵ�NodeList����
					iter->second.push_back(geode);
				}
			}

			unsigned int startIndex = 0;			
			for (glbInt32 i = 0; i < ptCnt; i++)
			{
				extRing->GetPoint(i,&ptx,&pty,&ptz);
				osg::Vec2 texCoord = CaculateTexCoord(ptx,pty,ptz,polyExt,polyTexRMode,polyTilingU,polyTilingV);
				texArray->push_back(texCoord);
				if (GLB_GLOBETYPE_GLOBE==globeType)						
					g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(pty),osg::DegreesToRadians(ptx),ptz,ptx,pty,ptz);
				vtArray->push_back(osg::Vec3(ptx,pty,ptz)-origVec);
				//vtArray->push_back(osg::Vec3(ptx,pty,ptz));
			}
			osg::ref_ptr<osg::DrawArrays> extRingPrimitiveset = new osg::DrawArrays(osg::PrimitiveSet::POLYGON,startIndex,ptCnt);
				
			geom->addPrimitiveSet(extRingPrimitiveset);					
			startIndex += ptCnt;

			glbInt32 inRingCnt = poly->GetInRingCount();
			for (glbInt32 j = 0; j < inRingCnt; j++)
			{
				CGlbLine* inRing = const_cast<CGlbLine *>(poly->GetInRing(j));
				ptCnt = inRing->GetCount();
				if (ptCnt<3) continue;

				for (glbInt32 k = 0; k < ptCnt; k++)
				{
					inRing->GetPoint(k,&ptx,&pty,&ptz);
					osg::Vec2 texCoord = CaculateTexCoord(ptx,pty,ptz,polyExt,polyTexRMode,polyTilingU,polyTilingV);
					texArray->push_back(texCoord);
					if (GLB_GLOBETYPE_GLOBE==globeType)						
						g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(pty),osg::DegreesToRadians(ptx),ptz,ptx,pty,ptz);
					vtArray->push_back(osg::Vec3(ptx,pty,ptz)-origVec);
					//vtArray->push_back(osg::Vec3(ptx,pty,ptz));
				}
				osg::ref_ptr<osg::DrawArrays> inRingPrimitiveset = new osg::DrawArrays(osg::PrimitiveSet::POLYGON,startIndex,ptCnt);				
				geom->addPrimitiveSet(inRingPrimitiveset);					
				startIndex += ptCnt;
			}

			// ���������
			if (geom->getNumPrimitiveSets()>0)
			{
				//osg::Vec3 _nor = CaculateTessellationNormal(poly,globeType);				

				osg::ref_ptr<osgUtil::Tessellator> tscx=new osgUtil::Tessellator();
				tscx->setTessellationType(osgUtil::Tessellator::TESS_TYPE_GEOMETRY);
				tscx->setBoundaryOnly(false);
				tscx->setWindingType(osgUtil::Tessellator::TESS_WINDING_ODD);
				//osg::Vec3d pointpos(0.0,0.0,1.0);
				tscx->setTessellationNormal(nor);
				tscx->retessellatePolygons(*(geom.get()));
			}		

			if (texture_name.length()<=0)
			{// ����ɫ���ʱ�ʾ��ɫ
				osg::StateSet* stateset = geom->getOrCreateStateSet();					
				osg::ref_ptr<osg::Material> material = new osg::Material;
				material->setDiffuse(osg::Material::FRONT_AND_BACK,clr);
				//material->setAlpha(osg::Material::FRONT_AND_BACK,polyOpacity/100.f);
				stateset->setAttribute(material,osg::StateAttribute::ON);
				//// ͸��Ч��
				//if (polyOpacity<100)
				//{// ��͸��
				//	stateset->setMode(GL_BLEND, osg::StateAttribute::ON);			
				//	if (osg::StateSet::TRANSPARENT_BIN != stateset->getRenderingHint())
				//		stateset->setRenderingHint( osg::StateSet::TRANSPARENT_BIN );
				//}
				//else
				//{// ��͸��
				//	stateset->setMode(GL_BLEND, osg::StateAttribute::OFF);								
				//	if (osg::StateSet::OPAQUE_BIN != stateset->getRenderingHint())
				//		stateset->setRenderingHint( osg::StateSet::OPAQUE_BIN );		
				//}							
				stateset->setMode(GL_LIGHTING,osg::StateAttribute::ON);
				stateset->setMode(GL_DEPTH_TEST,osg::StateAttribute::ON);
				// ��˫�����
				osg::LightModel* lm = new osg::LightModel;
				lm->setTwoSided(true);
				stateset->setAttributeAndModes(lm,osg::StateAttribute::ON);
			}
			else
			{// ʹ�������ͼ
				osg::StateSet* stateset = geom->getOrCreateStateSet();
				osg::Image* texImg = osgDB::readImageFile(texture_name.c_str());
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
					stateset->setTextureAttributeAndModes(0, tex2d, osg::StateAttribute::ON );

					stateset->setMode(GL_LIGHTING,osg::StateAttribute::OFF);
					// ������ת
					//osg::TexMat* _texmat = new osg::TexMat;
					//_texmat->setMatrix(osg::Matrix::rotate(osg::DegreesToRadians(polyTexRotation),0.0f,0.0f,1.0f));					
					//geom->getOrCreateStateSet()->setTextureAttributeAndModes(0,_texmat,osg::StateAttribute::ON);								
				}
				//else
				//{
				//	GlbLogWOutput(GLB_LOGTYPE_ERR,L"��ȡ����(Oid%d)�����(Oid%d)ͼ���ļ�(%s)ʧ��. \r\n",feature->GetOid(),polyFeature->GetOid(),datalocate.c_str());	
				//}
			}			
		}		
	}					
	return polyNode;
}

osg::Vec3 CGlbGlobeVirtualSectionObject::CaculatePolyNormal(CGlbPolygon *poly, GlbGlobeTypeEnum type)
{// ȡ��1,2��3�㹹�ɵ����������ϸ�����淨��
	if (poly==NULL || poly->GetExtent()==NULL)
		return osg::Vec3(1,1,1);
	CGlbLine* extLn = const_cast<CGlbLine*>(poly->GetExtRing());
	if (extLn==NULL || extLn->GetCount() < 3)
		return osg::Vec3(1,1,1);
	CGlbExtent* ext = const_cast<CGlbExtent*>(poly->GetExtent());
	glbDouble zlen = ext->GetZLengh();

	std::vector<osg::Vec3d> pts;
	glbDouble ptx,pty,ptz;
	for (int idx = 0; idx < extLn->GetCount(); idx++)
	{
		extLn->GetPoint(idx,&ptx,&pty,&ptz);
		if (pts.size()<=0)
		{			
			if (GLB_GLOBETYPE_GLOBE==type)
				g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(pty),osg::DegreesToRadians(ptx),ptz,ptx,pty,ptz);
			pts.push_back(osg::Vec3d(ptx,pty,ptz));
		}
		else
		{
			if (GLB_GLOBETYPE_GLOBE==type)
				g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(pty),osg::DegreesToRadians(ptx),ptz,ptx,pty,ptz);

			// �ڶ����㲻�����һ���㹲�㣬�������㲻���ڵ�һ�͵ڶ������������
			size_t preIdx = pts.size()-1;
			osg::Vec3d nor = osg::Vec3d(ptx,pty,ptz) - pts.at(preIdx);
			nor.normalize();
			if (nor.length()>0.00001)
			{
				if (preIdx>0)
				{// ��������
					osg::Vec3d prenor =  pts.at(preIdx) - pts.at(preIdx-1);					
					prenor.normalize();
					double cc = prenor*nor;
					//if (fabs(fabs(cc)-1) < 0.00001)
					if (fabs(cc) > 0.99999)// ����
						continue;					
					pts.push_back(osg::Vec3d(ptx,pty,ptz));				
				}
				else
				{// �ڶ�����
					pts.push_back(osg::Vec3d(ptx,pty,ptz));					
				}
			}			
		}

		if (pts.size()>2)
			break;
	}		
	if (pts.size()<3)
		return  osg::Vec3(1,1,1);

	osg::Vec3 v0 = pts.at(0);
	osg::Vec3 v1 = pts.at(1);
	osg::Vec3 v2 = pts.at(2);
	osg::Vec3 dir0 = v1 - v0;
	osg::Vec3 dir1 = v2 - v0;
	dir0.normalize();
	dir1.normalize();
	osg::Vec3 nor = dir0^dir1;
	nor.normalize();
	if (nor.length()<0.001)
	{
		nor.set(0.1,0.1,0.1);
		nor.normalize();
	
		pts.clear();
		for (int idx = 0; idx < extLn->GetCount(); idx++)
		{
			extLn->GetPoint(idx,&ptx,&pty,&ptz);
			if (pts.size()<=0)
			{
				//orignPts.push_back(osg::Vec3d(ptx,pty,ptz));
				if (GLB_GLOBETYPE_GLOBE==type)
					g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(pty),osg::DegreesToRadians(ptx),ptz,ptx,pty,ptz);
				pts.push_back(osg::Vec3d(ptx,pty,ptz));
			}
			else
			{
				//osg::Vec3d orgVec(ptx,pty,ptz);
				if (GLB_GLOBETYPE_GLOBE==type)
					g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(pty),osg::DegreesToRadians(ptx),ptz,ptx,pty,ptz);

				size_t preIdx = pts.size()-1;
				osg::Vec3d nor = osg::Vec3d(ptx,pty,ptz) - pts.at(preIdx);
				if (nor.length()>0)
				{
					if (preIdx>0)
					{
						osg::Vec3d prenor =  pts.at(preIdx) - pts.at(preIdx-1);
						nor.normalize();
						prenor.normalize();
						double cc = prenor*nor;
						if (cc!= 1 && cc!=-1)
							pts.push_back(osg::Vec3d(ptx,pty,ptz));
					}
					else
					{
						pts.push_back(osg::Vec3d(ptx,pty,ptz));
					}
				}
			}

			if (pts.size()>2)
				break;
		}		
	}
	return nor;
}

osg::Vec3 CGlbGlobeVirtualSectionObject::CaculateTessellationNormal(CGlbPolygon *poly, GlbGlobeTypeEnum type)
{// ȡ��1,2��3�㹹�ɵ�����㷨��
	if (poly==NULL || poly->GetExtent()==NULL)
		return osg::Vec3(1,1,1);
	glbDouble ptx,pty,ptz;	
	osg::ref_ptr<osg::Vec3Array> vtArray = new osg::Vec3Array;

	CGlbExtent* ext = const_cast<CGlbExtent*>(poly->GetExtent());

	glbDouble minx,miny,minz,maxx,maxy,maxz;	
	ext->GetMin(&minx,&miny,&minz);
	ext->GetMax(&maxx,&maxy,&maxz);

	// ����(minx,miny)-->(maxx,maxy) ��������(minx,maxy) --> (maxx,miny) ������Ҫ�����ж�!2018.10.11
	CGlbLine* extLn = const_cast<CGlbLine*>(poly->GetExtRing());
	if (extLn==NULL || extLn->GetCount() < 3)
		return osg::Vec3(1,1,1);
	osg::Vec3d _v0,_v1;
	for (int idx = 0; idx < extLn->GetCount(); idx++)
	{
		extLn->GetPoint(idx,&ptx,&pty,&ptz);
		if (idx == 0)
			_v0.set(ptx,pty,0);
		else
		{// �ڶ�����������һ�����x��y���겻ͬ			
			if (fabs((ptx-_v0.x())*(pty-_v0.y()))<=0.00001)
				continue;
			_v1.set(ptx,pty,0);
			break;
		}
	}
	osg::Vec3d _n0 = _v1 - _v0;
	osg::Vec3d _n1(maxx-minx,maxy-miny,0);
	_n0.normalize();
	_n1.normalize();
	double cc = fabs(_n0*_n1);
	double cosQuatorPI = cos(osg::PI_4);
	if (cc <= cosQuatorPI)
	{// (minx,maxy) --> (maxx,miny)
		ptx = minx; pty = maxy; ptz = minz;
		if (GLB_GLOBETYPE_GLOBE==type)						
			g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(pty),osg::DegreesToRadians(ptx),ptz,ptx,pty,ptz);
		vtArray->push_back(osg::Vec3(ptx,pty,ptz));

		ptx = maxx; pty = miny; ptz = minz;
		if (GLB_GLOBETYPE_GLOBE==type)						
			g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(pty),osg::DegreesToRadians(ptx),ptz,ptx,pty,ptz);
		vtArray->push_back(osg::Vec3(ptx,pty,ptz));
	}
	else
	{// (minx,miny)-->(maxx,maxy)
		ptx = minx; pty = miny; ptz = minz;
		if (GLB_GLOBETYPE_GLOBE==type)						
			g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(pty),osg::DegreesToRadians(ptx),ptz,ptx,pty,ptz);
		vtArray->push_back(osg::Vec3(ptx,pty,ptz));

		ptx = maxx; pty = maxy; ptz = minz;
		if (GLB_GLOBETYPE_GLOBE==type)						
			g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(pty),osg::DegreesToRadians(ptx),ptz,ptx,pty,ptz);
		vtArray->push_back(osg::Vec3(ptx,pty,ptz));
	}	

	if (ext->GetZLengh()<0.00001)
	{
		if (cc <= cosQuatorPI)
		{
			ptx = maxx; pty = maxy; ptz = minz;
		}
		else
		{
			ptx = maxx; pty = miny; ptz = minz;
		}		
	}
	else
	{
		if (cc <= cosQuatorPI)
		{
			ptx = maxx; pty = miny; ptz = maxz;
		}
		else
		{
			ptx = maxx; pty = maxy; ptz = maxz;
		}
	}	
	if (GLB_GLOBETYPE_GLOBE==type)						
		g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(pty),osg::DegreesToRadians(ptx),ptz,ptx,pty,ptz);
	vtArray->push_back(osg::Vec3(ptx,pty,ptz));

	osg::Vec3 v0 = vtArray->at(0);
	osg::Vec3 v1 = vtArray->at(1);
	osg::Vec3 v2 = vtArray->at(2);
	osg::Vec3 dir0 = v1 - v0;
	osg::Vec3 dir1 = v2 - v1;
	osg::Vec3 nor = dir0^dir1;
	nor.normalize();
	return nor;
}
CGlbGlobeVirtualSectionObject::GlbProjPlaneEnum CGlbGlobeVirtualSectionObject::CaculateProjPlane(CGlbExtent* ext)
{// ͶӰ��� - XZƽ���YZƽ��  ������XYƽ�� ����Ͳ���������	
	glbDouble xLen = ext->GetXWidth();
	glbDouble yLen = ext->GetYHeight();

	if (xLen >= yLen)
		return GLB_PLANE_XZ;
	else
		return GLB_PLANE_YZ;

	return GLB_PLANE_UNKNOWN;
}

osg::Vec2 CGlbGlobeVirtualSectionObject::CaculateTexCoord(glbDouble ptx,glbDouble pty,glbDouble ptz, CGlbExtent* ext,
															GlbGlobeTexRepeatModeEnum texRMode,glbInt32 tilingU	,glbInt32 tilingV)
{
	osg::Vec2 vt;
	vt.set(0,0);
	if (ext==NULL) return vt;

	GlbProjPlaneEnum prjPlaneEnum = CaculateProjPlane(ext);
	glbDouble xMin,xMax,yMin,yMax,zMin,zMax;
	ext->Get(&xMin,&xMax,&yMin,&yMax,&zMin,&zMax);

	float u,v;
	switch (prjPlaneEnum)
	{
	case GLB_PLANE_XZ:
		{
			if (texRMode==GLB_TEXTUREREPEAT_TIMES) 
			{
				u = (ptx - xMin) / (xMax - xMin) * tilingU;
				v = (ptz - zMin) / (zMax - zMin) * tilingV;
			}
			else if (texRMode==GLB_TEXTUREREPEAT_SIZE)
			{
				u = (ptx - xMin) / tilingU;
				v = (ptz - zMin) / tilingV;
			}			
			vt.set(u,v);
		}
		break;
	case GLB_PLANE_YZ:
		{
			if (texRMode==GLB_TEXTUREREPEAT_TIMES)
			{
				u = (pty - yMin) / (yMax - yMin) * tilingU;
				v = (ptz - zMin) / (zMax - zMin) * tilingV;
			}
			else if (texRMode==GLB_TEXTUREREPEAT_SIZE)
			{
				u = (pty - yMin) / tilingU;
				v = (ptz - zMin) / tilingV;
			}
			vt.set(u,v);
		}
		break;
	}
	return vt;
}