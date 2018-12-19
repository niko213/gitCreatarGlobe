#include "StdAfx.h"
#include "GlbGlobeCompositeObject.h"

#include "CGlbGlobe.h"
#include "GlbGlobeFeatureLayer.h"
#include "GlbGlobeTypes.h"

#include "GlbCalculateNodeSizeVisitor.h"


#ifdef min
#undef min
#endif

#ifdef max
#undef max
#endif

using namespace GlbGlobe;

glbInt32 ComputeNodeSize( osg::Node *node )
{
	glbInt32 mpr_objSize=0;
		CGlbCalculateNodeSizeVisitor cnsv;
		node->accept(cnsv);
		mpr_objSize = cnsv.getTextureSize() + cnsv.getNodeMemSize();	

	return mpr_objSize;
}
//////////////////////////////////////////////////////////////////////////
CGlbGlobeCompositeObject::CGlbGlobeCompositeObject(void)
{
	
}

CGlbGlobeCompositeObject::~CGlbGlobeCompositeObject(void)
{
	CGlbGlobeRObjectList::iterator iterBegin=mpr_subogjs.begin();
	CGlbGlobeRObjectList::iterator iterEnd=mpr_subogjs.end();
	for (CGlbGlobeRObjectList::iterator iter=iterBegin;iter!=iterEnd;++iter)
	{
		CGlbGlobeRObject* obj = (*iter).get();
		obj->RemoveFromScene(true);
		obj->SetDestroy();
		obj->SetParentObject(NULL);
		if(mpt_globe)
		{//从场景索引中删除对象
			mpt_globe->mpr_sceneobjIdxManager->RemoveObject(obj);
		}
	}
}
/*
*   设置高程模式
*   子对象 的高程模式可以不是一致的!!!!
*/
glbBool CGlbGlobeCompositeObject::SetAltitudeMode( GlbAltitudeModeEnum mode ,glbBool &isNeedDirectDraw)
{
	glbBool dd = false;
	isNeedDirectDraw = false;
	CGlbGlobeRObjectList::iterator iterBegin=mpr_subogjs.begin();
	CGlbGlobeRObjectList::iterator iterEnd=mpr_subogjs.end();
	for (CGlbGlobeRObjectList::iterator iter=iterBegin;iter!=iterEnd;++iter)
	{
		CGlbGlobeREObject* obj = dynamic_cast<CGlbGlobeREObject*>((*iter).get());
		if(obj)
		{
			/*
			    因为子对象的SetAltitudeMode，在地形时会更新污染区域.
				所以：符号对象可以不用更新污染区域.
			*/
			obj->SetAltitudeMode(mode,dd);
			/*
				因为： 子对象，有的需要DirectDraw，有的不需要
				所以：
				     在此直接调用子对象的DirectDraw.
			*/
			if(dd)obj->DirectDraw(obj->GetCurrLevel());
		}		
	}
	mpt_altitudeMode = mode;
	return true;
}

/*
*  设置渲染顺序
*  渲染顺序只对 贴地形模式起作用.
*  
*   !!!!! 不要求 所有子对象的渲染顺序 必须一致.
*/
void CGlbGlobeCompositeObject::SetRenderOrder( glbInt32 order )
{	
	CGlbGlobeRObjectList::iterator iterBegin=mpr_subogjs.begin();
	CGlbGlobeRObjectList::iterator iterEnd=mpr_subogjs.end();
	for (CGlbGlobeRObjectList::iterator iter=iterBegin;iter!=iterEnd;++iter)
	{
		CGlbGlobeREObject* obj = dynamic_cast<CGlbGlobeREObject*>((*iter).get());
		if(obj)
		{
			/*
			    因为子对象的SetAltitudeMode，在地形时会更新污染区域.
				所以：符号对象可以不用更新污染区域.
			*/
			obj->SetRenderOrder(order);
		}		
	}
	mpt_renderOrder = order;	
}
/*
	设置装载优先级
	因为：装载时是以【复合对象】为单位装载的
	所以：所有子对象的装载顺序 必须是一致的.
*/
void CGlbGlobeCompositeObject::SetLoadOrder(glbInt32 order)
{
	CGlbGlobeRObjectList::iterator iterBegin=mpr_subogjs.begin();
	CGlbGlobeRObjectList::iterator iterEnd=mpr_subogjs.end();
	for (CGlbGlobeRObjectList::iterator iter=iterBegin;iter!=iterEnd;++iter)
	{
		CGlbGlobeREObject* obj = dynamic_cast<CGlbGlobeREObject*>((*iter).get());
		if(obj)
			obj->SetLoadOrder(order);
	}
	mpt_loadOrder = order;
}
/*
	设置是否 实例化
	！！！ 不要求所有子对象都需要实例化
*/
void CGlbGlobeCompositeObject::SetUseInstance(glbBool useInstance)
{
	CGlbGlobeRObjectList::iterator iterBegin=mpr_subogjs.begin();
	CGlbGlobeRObjectList::iterator iterEnd=mpr_subogjs.end();
	for (CGlbGlobeRObjectList::iterator iter=iterBegin;iter!=iterEnd;++iter)
	{
		CGlbGlobeRObject* obj = (*iter).get();
		if(obj)
			obj->SetUseInstance(useInstance);
	}
	mpt_isUseInstance = useInstance;
}
/*
*   复合对象 是作为一个整体调度！！！
*   所以：
*        距离 = min(子对象的距离)
*/
glbDouble CGlbGlobeCompositeObject::GetDistance( osg::Vec3d &cameraPos,glbBool isCompute )
{
	glbDouble minDistance=std::numeric_limits<double>::max();
	CGlbGlobeRObjectList::iterator iterBegin=mpr_subogjs.begin();
	CGlbGlobeRObjectList::iterator iterEnd=mpr_subogjs.end();
	for (CGlbGlobeRObjectList::iterator iter=iterBegin;iter!=iterEnd;++iter)
	{
		double tmp=(*iter)->GetDistance(cameraPos, isCompute);
		if (tmp<minDistance)
			minDistance=tmp;
	}
	return minDistance;
}
/*
*   复合对象 是作为一个整体调度！！！
*   
*   子对象在场景索引中是不存在的!!!!
*    所以：
*         复合对象的LoadData必须 一次调用子对象的LoadData.
*/
void CGlbGlobeCompositeObject::LoadData( glbInt32 level )
{
	CGlbGlobeRObjectList::iterator iterBegin=mpr_subogjs.begin();
	CGlbGlobeRObjectList::iterator iterEnd=mpr_subogjs.end();
	for (CGlbGlobeRObjectList::iterator iter=iterBegin;iter!=iterEnd;++iter)
	{
		(*iter)->LoadData(level);
	}
	return CGlbGlobeREObject::LoadData(level);
}
/*
*   复合对象 是作为一个整体调度！！！
*   
*   子对象在场景索引中是不存在的!!!!
*    所以：
*         复合对象的AddToScene必须 一次调用子对象的AddToScene.
*/
void CGlbGlobeCompositeObject::AddToScene()
{
	CGlbGlobeRObjectList::iterator iterBegin=mpr_subogjs.begin();
	CGlbGlobeRObjectList::iterator iterEnd=mpr_subogjs.end();
	for (CGlbGlobeRObjectList::iterator iter=iterBegin;iter!=iterEnd;++iter)
	{
		(*iter)->AddToScene();
	}
}
/*
*   复合对象 是作为一个整体调度！！！
*   
*   子对象在场景索引中是不存在的!!!!
*    所以：
*         复合对象的RemoveFromScene必须 一次调用子对象的RemoveFromScene.
*/
glbInt32 CGlbGlobeCompositeObject::RemoveFromScene( glbBool isClean )
{
	glbInt32 objsize=0;
	CGlbGlobeRObjectList::iterator iterBegin=mpr_subogjs.begin();
	CGlbGlobeRObjectList::iterator iterEnd=mpr_subogjs.end();
	for (CGlbGlobeRObjectList::iterator iter=iterBegin;iter!=iterEnd;++iter)
	{
		objsize+=(*iter)->RemoveFromScene(isClean);
	}
	return objsize;
}

void CGlbGlobeCompositeObject::SetParentNode( osg::Node *parent )
{
	CGlbGlobeRObjectList::iterator iterBegin=mpr_subogjs.begin();
	CGlbGlobeRObjectList::iterator iterEnd=mpr_subogjs.end();
	for (CGlbGlobeRObjectList::iterator iter=iterBegin;iter!=iterEnd;++iter)
	{
		(*iter)->SetParentNode(parent);
	}
	return CGlbGlobeREObject::SetParentNode(parent);
}
/*
    复合对象，不放到场景索引中
	所以：
	     GetBound返回NULL
*/
glbref_ptr<CGlbExtent> CGlbGlobeCompositeObject::GetBound( glbBool isWorld /*= true */ )
{
	return NULL;
}

void CGlbGlobeCompositeObject::UpdateElevate()
{
	CGlbGlobeRObjectList::iterator iterBegin=mpr_subogjs.begin();
	CGlbGlobeRObjectList::iterator iterEnd=mpr_subogjs.end();
	for (CGlbGlobeRObjectList::iterator iter=iterBegin;iter!=iterEnd;++iter)
	{
		(*iter)->UpdateElevate();
	}
	return CGlbGlobeREObject::UpdateElevate();
}
/*
    复合对象，不放到场景索引中，不拾取
	所以：
	     GetOutLine返回NULL
*/
IGlbGeometry * CGlbGlobeCompositeObject::GetOutLine()
{
	return NULL;
}
/*
    复合对象，不设置渲染属性
*/
glbBool CGlbGlobeCompositeObject::SetRenderInfo( GlbRenderInfo *renderInfo ,glbBool &isNeedDirectDraw)
{
	return false;
}

GlbRenderInfo * CGlbGlobeCompositeObject::GetRenderInfo()
{
	return NULL;
}

glbBool CGlbGlobeCompositeObject::SetShow( glbBool isShow,glbBool isOnState/*=false */ )
{
	CGlbGlobeRObjectList::iterator iterBegin=mpr_subogjs.begin();
	CGlbGlobeRObjectList::iterator iterEnd=mpr_subogjs.end();
	for (CGlbGlobeRObjectList::iterator iter=iterBegin;iter!=iterEnd;++iter)
	{
		(*iter)->SetShow(isShow,isOnState);
	}
	return CGlbGlobeREObject::SetShow(isShow,isOnState);
}

glbBool CGlbGlobeCompositeObject::SetSelected( glbBool isSelected )
{
	CGlbGlobeRObjectList::iterator iterBegin=mpr_subogjs.begin();
	CGlbGlobeRObjectList::iterator iterEnd=mpr_subogjs.end();
	for (CGlbGlobeRObjectList::iterator iter=iterBegin;iter!=iterEnd;++iter)
	{
		(*iter)->SetSelected(isSelected);
	}
	return CGlbGlobeREObject::SetSelected(isSelected);
}

glbBool CGlbGlobeCompositeObject::SetBlink( glbBool isBlink )
{
	CGlbGlobeRObjectList::iterator iterBegin=mpr_subogjs.begin();
	CGlbGlobeRObjectList::iterator iterEnd=mpr_subogjs.end();
	for (CGlbGlobeRObjectList::iterator iter=iterBegin;iter!=iterEnd;++iter)
	{
		(*iter)->SetBlink(isBlink);
	}
	return CGlbGlobeREObject::SetBlink(isBlink);
}

void CGlbGlobeCompositeObject::DirectDraw( glbInt32 level )
{
	CGlbGlobeRObjectList::iterator iterBegin=mpr_subogjs.begin();
	CGlbGlobeRObjectList::iterator iterEnd=mpr_subogjs.end();
	for (CGlbGlobeRObjectList::iterator iter=iterBegin;iter!=iterEnd;++iter)
	{
		(*iter)->DirectDraw(level);
	}
	return CGlbGlobeREObject::DirectDraw(level);
}

void CGlbGlobeCompositeObject::DrawToImage( glbByte *image, glbInt32 imageW,glbInt32 imageH,CGlbExtent &ext )
{
	CGlbGlobeRObjectList::iterator iterBegin=mpr_subogjs.begin();
	CGlbGlobeRObjectList::iterator iterEnd=mpr_subogjs.end();
	for (CGlbGlobeRObjectList::iterator iter=iterBegin;iter!=iterEnd;++iter)
	{
		(*iter)->DrawToImage(image, imageW,imageH,ext );
	}
	return CGlbGlobeREObject::DrawToImage(image, imageW,imageH,ext );
}

glbInt32 CGlbGlobeCompositeObject::GetOsgNodeSize()
{
	glbInt32 osgNodeSize=0;
	CGlbGlobeRObjectList::iterator iterBegin=mpr_subogjs.begin();
	CGlbGlobeRObjectList::iterator iterEnd=mpr_subogjs.end();
	for (CGlbGlobeRObjectList::iterator iter=iterBegin;iter!=iterEnd;++iter)
	{
		osgNodeSize+=(*iter)->GetOsgNodeSize();
	}
	return osgNodeSize;
}

glbBool CGlbGlobeCompositeObject::Load( xmlNodePtr* node,glbWChar* prjPath )
{
	CGlbGlobeRObjectList::iterator iterBegin=mpr_subogjs.begin();
	CGlbGlobeRObjectList::iterator iterEnd=mpr_subogjs.end();
	for (CGlbGlobeRObjectList::iterator iter=iterBegin;iter!=iterEnd;++iter)
	{
		(*iter)->Load(node,prjPath);
	}
	return true;
}

glbBool CGlbGlobeCompositeObject::Save( xmlNodePtr node,const glbWChar* prjPath )
{
	CGlbGlobeRObjectList::iterator iterBegin=mpr_subogjs.begin();
	CGlbGlobeRObjectList::iterator iterEnd=mpr_subogjs.end();
	for (CGlbGlobeRObjectList::iterator iter=iterBegin;iter!=iterEnd;++iter)
	{
		(*iter)->Save(node,prjPath);
	}
	return true;
}

GlbGlobe::GlbGlobeObjectTypeEnum CGlbGlobeCompositeObject::GetType()
{
	return GLB_OBJECTTYPE_COMPLEX;
}

void CGlbGlobeCompositeObject::SetGlobe( CGlbGlobe *globe )
{
	CGlbGlobeRObjectList::iterator iterBegin=mpr_subogjs.begin();
	CGlbGlobeRObjectList::iterator iterEnd=mpr_subogjs.end();
	for (CGlbGlobeRObjectList::iterator iter=iterBegin;iter!=iterEnd;++iter)
	{
		(*iter)->SetGlobe(globe);
	}
	return CGlbGlobeREObject::SetGlobe(globe);
}
glbInt32 CGlbGlobeCompositeObject::GetCount()
{
	return mpr_subogjs.size();
}
/*
*   添加子对象.
*   复合对象：
*          1. 子对象 需要添加到场景索引，已利于拾取.
*          2. 
*/
glbBool CGlbGlobeCompositeObject::AddObject( CGlbGlobeRObject *obj )
{
	if (obj==NULL)return false;

	CGlbGlobeRObjectList::iterator iterBegin=mpr_subogjs.begin();
	CGlbGlobeRObjectList::iterator iterEnd=mpr_subogjs.end();
	for (CGlbGlobeRObjectList::iterator iter=iterBegin;iter!=iterEnd;++iter)
	{
		CGlbGlobeRObject* objTmp=(*iter).get();
		if (obj==objTmp)
		{//已经添加
			return false;
		}
	}
	mpr_subogjs.push_back(obj);

	if (mpt_globe!=NULL)
		obj->SetGlobe(mpt_globe);

	if (mpt_parent)
		obj->SetParentNode(mpt_parent);
	//1. 子对象添加到场景索引
	if (mpt_globe!=NULL)mpt_globe->mpr_sceneobjIdxManager->AddObject(obj);
	//2. 设置子对象的Parent
	obj->SetParentObject(this);
	return true;
}

glbBool CGlbGlobeCompositeObject::RemoveObject( glbInt32 idx )
{
	if (idx<0)return false;

	glbInt32 objCnt =mpr_subogjs.size();
	if (idx>=objCnt)return false;

	CGlbGlobeRObjectList::iterator iterBegin=mpr_subogjs.begin();
	CGlbGlobeRObjectList::iterator iterEnd=mpr_subogjs.end();
	glbInt32 idxTmp=0;
	CGlbGlobeRObject* objTmp;
	for (CGlbGlobeRObjectList::iterator iter=iterBegin;iter!=iterEnd;++iter)
	{
		if (idxTmp==idx)
		{
			objTmp=(*iter).get();
			objTmp->RemoveFromScene(true);
			objTmp->SetParentObject(NULL);			
			if(mpt_globe)
			{//从场景索引中删除对象
				mpt_globe->mpr_sceneobjIdxManager->RemoveObject(objTmp);	
			}
			/*
			//if (mpt_altitudeMode==GLB_ALTITUDEMODE_ONTERRAIN)
			子对象在RemoveFromeScene方法中会更新污染区域.
			所以这里就不用:
				DirtyOnTerrainObject()
			*/
			mpr_subogjs.erase(iter);
			
			break;
		}
		++idxTmp;
	}
	return true;
}

CGlbGlobeRObject *CGlbGlobeCompositeObject::GetRObject( glbInt32 idx )
{
	if (idx<0)return NULL;

	glbInt32 objCnt =mpr_subogjs.size();
	if (idx>=objCnt)return NULL;

	CGlbGlobeRObject* objTmp = mpr_subogjs[idx].get();
	return objTmp;
}


