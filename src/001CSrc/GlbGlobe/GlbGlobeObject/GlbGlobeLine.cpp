#include "StdAfx.h"
#include "GlbGlobeFeatureLayer.h"
#include "GlbGlobeLine.h"
#include "GlbCalculateNodeSizeVisitor.h"
#include "GlbCalculateBoundBoxVisitor.h"
#include "GlbGlobeLinePixelSymbol.h"
#include "GlbGlobeLineArrowSymbol.h"
#include "GlbGlobeLineDynamicArrowSymbol.h"
#include <osg/Material>
#include <osg/BlendColor>
#include <osg/LineWidth>
#include <osg/LineStipple>
#include <osg/Point>
#include <osgDB/ReadFile>
#include <osg/TexMat>
#include <osg/TexGen>
#include <osg/TexEnv>
#include "GlbCompareValue.h"

#include "osgGA/GUIEventHandler"
#include "GlbGlobeView.h"
#include "osg/LineSegment"
#include "GlbGlobeMath.h"
#include "GlbGlobePoint.h"
#include "GlbGlobeMemCtrl.h"
#include "GlbGlobeLineModelSymbol.h"

#include "GlbGlobeSymbolCommon.h"
#include "GlbConvert.h"
#include "GlbFadeInOutCallback.h"

#include "osg/Depth"

using namespace GlbGlobe;

class LineObjectCallback : public osg::NodeCallback
{
public:
	LineObjectCallback()
	{
		mpr_markNum = 0;
		mpr_isStateChanged = true;
	}

	~LineObjectCallback()
	{
	}

	void operator()(osg::Node *node,osg::NodeVisitor *nv)
	{
		unsigned int frameNum = 60;
		glbInt32 count = nv->getFrameStamp()->getFrameNumber() - mpr_markNum;

		if (count > frameNum - 1)
		{
			mpr_markNum = nv->getFrameStamp()->getFrameNumber();
			mpr_isStateChanged = !mpr_isStateChanged;
			count = 0;
		}

		if(!mpr_isStateChanged)
			count = frameNum - count;

		float a = count * 1 / (float)frameNum;
		if(a < 0.2) a = 0.2;

		DealPixelLineBlink(node,a);
		traverse(node,nv);	
	}
private:
	void DealPixelLineBlink(osg::Node *node,float a)
	{
		osg::MatrixTransform *mt = dynamic_cast<osg::MatrixTransform *>(node->asSwitch()->getChild(0));
		if(mt == NULL) return;
		osg::Geode *geode = dynamic_cast<osg::Geode *>(mt->getChild(0));
		if(geode == NULL) return;
		osg::Geometry *lineGeometry = dynamic_cast<osg::Geometry*>(geode->getDrawable(0));
		if(lineGeometry == NULL) return;
		osg::StateSet *stateset = geode->getOrCreateStateSet();
		osg::ref_ptr<osg::Material> material = dynamic_cast<osg::Material*>(stateset->getAttribute(osg::StateAttribute::MATERIAL));
		if (!material.valid()) return;
		material->setAlpha(osg::Material::FRONT_AND_BACK,a);
		stateset->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
		stateset->setMode(GL_BLEND,osg::StateAttribute::ON);
	}
private:
	glbInt32 mpr_markNum;
	glbBool mpr_isStateChanged;
};

CGlbGlobeLine::CGlbGlobeLine(void)
{
	mpr_readData_critical.SetName(L"line_readdata");
	mpr_addToScene_critical.SetName(L"line_addscene");
	mpr_outline_critical.SetName(L"line_outline");
	mpr_distance = 0.0;
	mpr_outline = NULL;
	mpr_renderInfo = NULL;
	mpr_lineGeo = NULL;
	mpr_elevation = DBL_MAX;// 设置无效值
	mpr_needReReadData = false;
	mpr_isCanDrawImage = true;
}

CGlbGlobeLine::~CGlbGlobeLine(void)
{

}

CGlbGlobeLine::CGlbGlobeLineTask::CGlbGlobeLineTask( CGlbGlobeLine *obj,glbInt32 level )
{
	mpr_obj = obj;
	mpr_level = level;
}

CGlbGlobeLine::CGlbGlobeLineTask::~CGlbGlobeLineTask( )
{
	mpr_obj = NULL;
}

glbBool CGlbGlobeLine::CGlbGlobeLineTask::doRequest()
{
	if(mpr_obj)
	{
		mpr_obj->ReadData(mpr_level,false);
		return true;
	}
	return false;
}

CGlbGlobeObject * CGlbGlobeLine::CGlbGlobeLineTask::getObject()
{
	return mpr_obj.get();
}

GlbGlobeObjectTypeEnum CGlbGlobeLine::GetType()
{
	return GLB_OBJECTTYPE_LINE;
}

glbBool CGlbGlobeLine::Load( xmlNodePtr *node,const glbWChar* prjPath )
{
	CGlbGlobeRObject::Load(node,prjPath);
	xmlChar *szKey = NULL;
	xmlNodePtr pnode = *node;
	glbInt32 pointsCount = 0;
	if(pnode && (!xmlStrcmp(pnode->name,(const xmlChar *)"PointsCount")))
	{
		szKey = xmlNodeGetContent(pnode);
		sscanf((char*)szKey,"%d",&pointsCount);	
		xmlFree(szKey);

		pnode = pnode->next;
	}

	std::string index = "pointsIndex";
	glbDouble tempData0,tempData1,tempData2;

	mpr_lineGeo = new CGlbLine(3,false);
	for(glbInt32 i = 0; i < pointsCount * 3;i++)
	{
		index += CGlbConvert::Int32ToStr(i);
		if(pnode && (!xmlStrcmp(pnode->name,(const xmlChar *)index.c_str())))
		{
			szKey = xmlNodeGetContent(pnode);
			sscanf((char *)szKey,"%lf",&tempData0);
			xmlFree(szKey);

			pnode = pnode->next;
			index = "pointsIndex";
		}

		i++;
		index += CGlbConvert::Int32ToStr(i);
		if(pnode && (!xmlStrcmp(pnode->name,(const xmlChar *)index.c_str())))
		{
			szKey = xmlNodeGetContent(pnode);
			sscanf((char *)szKey,"%lf",&tempData1);
			xmlFree(szKey);

			pnode = pnode->next;
			index = "pointsIndex";
		}

		i++;
		index += CGlbConvert::Int32ToStr(i);
		if(pnode && (!xmlStrcmp(pnode->name,(const xmlChar *)index.c_str())))
		{
			szKey = xmlNodeGetContent(pnode);
			sscanf((char *)szKey,"%lf",&tempData2);
			xmlFree(szKey);

			pnode = pnode->next;
			index = "pointsIndex";
		}

		mpr_lineGeo->AddPoint(tempData0,tempData1,tempData2);
	}
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
		if(rdchild && (!xmlStrcmp(rdchild->name, (const xmlChar *)"SymbolType")))
		{
			szKey = xmlNodeGetContent(rdchild);
			sscanf((char*)szKey,"%d",&symtype);	
			xmlFree(szKey);

			rdchild = rdchild->next;
			switch(symtype)
			{
			case  GLB_LINESYMBOL_PIXEL:
				{
					GlbLinePixelSymbolInfo* pixelinfo = new GlbLinePixelSymbolInfo();
					pixelinfo->Load(rdchild,prjPath);
					mpr_renderInfo = (GlbRenderInfo*)pixelinfo;
				}
				break;
			case GLB_LINESYMBOL_2D:
				{
					GlbLine2DSymbolInfo* line2DInfo = new GlbLine2DSymbolInfo();
					line2DInfo->Load(rdchild,prjPath);
					mpr_renderInfo = (GlbRenderInfo*)line2DInfo;
				}
				break;
			case GLB_LINESYMBOL_3D:
				{
					GlbLine3DSymbolInfo* line3DInfo = new GlbLine3DSymbolInfo();
					line3DInfo->Load(rdchild,prjPath);
					mpr_renderInfo = (GlbRenderInfo*)line3DInfo;
				}
				break;
			case GLB_LINESYMBOL_ARROW:
				{
					GlbLineArrowSymbolInfo* arrowInfo = new GlbLineArrowSymbolInfo();
					arrowInfo->Load(rdchild,prjPath);
					mpr_renderInfo = (GlbRenderInfo*)arrowInfo;
				}
				break;
			case GLB_LINESYMBOL_DYNAMICARROW:
				{
					GlbLineDynamicArrowSymbolInfo* arrowInfo = new GlbLineDynamicArrowSymbolInfo();
					arrowInfo->Load(rdchild,prjPath);
					mpr_renderInfo = (GlbRenderInfo*)arrowInfo;
				}
				break;
			}
		}
	}
	ParseObjectFadeColor();
	return true;
} 

glbBool CGlbGlobeLine::Save( xmlNodePtr node,const glbWChar* prjPath )
{
	CGlbGlobeRObject::Save(node,prjPath);

	char str[32];
	glbInt32 pointsCount = mpr_lineGeo->GetCount();
	sprintf(str,"%d",pointsCount);
	xmlNewTextChild(node,NULL,BAD_CAST "PointsCount",BAD_CAST str);

	const glbDouble *points = mpr_lineGeo->GetPoints();
	std::string index = "pointsIndex";
	for (glbInt32 i = 0; i < pointsCount * 3; i++)
	{
		index = "pointsIndex";
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

		GlbLineSymbolInfo *lineInfo = (GlbLineSymbolInfo *)mpr_renderInfo.get();
		sprintf_s(str,"%d",lineInfo->symType);
		xmlNewTextChild(rdnode,NULL,BAD_CAST "SymbolType",BAD_CAST str);
		switch(lineInfo->symType)
		{
		case GLB_LINESYMBOL_PIXEL:
			{
				((GlbLinePixelSymbolInfo *)mpr_renderInfo.get())->Save(rdnode,prjPath);
			}break;
		case GLB_LINESYMBOL_2D:
			{
				((GlbLine2DSymbolInfo *)mpr_renderInfo.get())->Save(rdnode,prjPath);
			}break;
		case GLB_LINESYMBOL_3D:
			{
				((GlbLine3DSymbolInfo *)mpr_renderInfo.get())->Save(rdnode,prjPath);
			}break;
		case GLB_LINESYMBOL_ARROW:
			{
				((GlbLineArrowSymbolInfo *)mpr_renderInfo.get())->Save(rdnode,prjPath);
			}break;
		case GLB_LINESYMBOL_DYNAMICARROW:
			{
				((GlbLineDynamicArrowSymbolInfo *)mpr_renderInfo.get())->Save(rdnode,prjPath);
			}break;
		default:
			break;
		}
	}
	return true;
}

glbDouble CGlbGlobeLine::GetDistance( osg::Vec3d &cameraPos,glbBool isCompute )
{
	if (isCompute == false)
		return mpr_distance;

	if (!mpt_globe)	return DBL_MAX;

	if (mpt_isCameraAltitudeAsDistance)	
		mpr_distance = cameraPos.z();	
	else
	{
		osg::Vec3d position;
		if (mpt_feature.get())
		{
			CGlbExtent* ext = const_cast<CGlbExtent *>(mpt_feature->GetExtent());
			ext->GetCenter(&position.x(),&position.y(),&position.z());	
		}else{
			if (mpr_lineGeo == NULL)
				return DBL_MAX;
			CGlbExtent *lineExtent = const_cast<CGlbExtent *>(mpr_lineGeo->GetExtent());
			lineExtent->GetCenter(&position.x(),&position.y(),&position.z());	
		}
		ComputePosByAltitudeAndGloleType(position);
		osg::Vec3d cameraPoint = cameraPos;
		if (mpt_globe->GetType() == GLB_GLOBETYPE_GLOBE)
			g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(cameraPos.y()),
			osg::DegreesToRadians(cameraPos.x()),cameraPos.z(),cameraPoint.x(),cameraPoint.y(),cameraPoint.z());	
		mpr_distance = (position - cameraPoint).length();
	}
	return mpr_distance;
}

void CGlbGlobeLine::LoadData( glbInt32 level )
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
	glbref_ptr<GlbLineSymbolInfo> lineSymbol = dynamic_cast<GlbLineSymbolInfo *>(mpr_renderInfo.get());
	if(lineSymbol == NULL)
		return;
	// 非3D模型类型，且已经load过，就不用再次load  ---> 那种线有多个level？？？
	if ( mpt_currLevel != -1) 	
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
	glbref_ptr<CGlbGlobeLineTask> task = new CGlbGlobeLineTask(this,level);
	mpt_globe->mpr_taskmanager->AddTask(task.get());
}

void CGlbGlobeLine::AddToScene()
{
	if(mpt_altitudeMode == GLB_ALTITUDEMODE_ONTERRAIN)return;
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
			&& mpt_node->getNumParents() == 0)
		{
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

glbInt32 CGlbGlobeLine::RemoveFromScene( glbBool isClean )
{
	SetEdit(false);
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
	if(  mpt_altitudeMode == GLB_ALTITUDEMODE_ONTERRAIN )
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

glbref_ptr<CGlbExtent> CGlbGlobeLine::GetBound( glbBool isWorld /*= true*/ )
{
	if (mpt_globe == NULL && isWorld) return NULL;
	/*
	Pick 时，会【其他线程中】调用GetBound
	当isWorld == true时，会用到mpt_node,
	此时，如果另有线程 调用RemoveFromScene时，会使mpt_node = NULL.
	这时：在GetBound中用到mpt_node会崩溃.
	所以：
	定义一个局部变量objNode.
	*/
	osg::ref_ptr<osg::Node> objNode = mpt_node;
	glbref_ptr<CGlbExtent> geoExt = NULL;
	if(    mpt_altitudeMode==GLB_ALTITUDEMODE_ONTERRAIN
		&& mpr_outline != NULL)
	{//贴地形绘制模式
		/*
		*   索引、查询【qtree】才能准确. z == 0.
		*/
		geoExt = const_cast<CGlbExtent *>(mpr_outline->GetExtent());
	}else{
		if(mpt_feature.get())		
			geoExt = const_cast<CGlbExtent *>(mpt_feature->GetExtent());		
		else if(mpr_lineGeo)		
			geoExt = const_cast<CGlbExtent *>(mpr_lineGeo->GetExtent());				
	}
	if(isWorld == false)
	{//地理坐标
		/*
		对象添加时，对贴地形对象,在场景树中是用的地理坐标!!!!
		贴地形绘制，GetDistance会用到mpr_elevate.
		所以：
		调用UpdateElevate().
		*/
		if( mpr_elevation == DBL_MAX
			&& mpt_altitudeMode==GLB_ALTITUDEMODE_ONTERRAIN )
			UpdateElevate();
		/*
		*   地理坐标，不反应相对地形绘制的地形海拔.
		*/
		return geoExt;
	}
	else
	{//世界坐标		
		if(geoExt == NULL)return NULL;
		CGlbExtent* worldExt = new CGlbExtent();
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
			if(   objNode != NULL
				&&objNode->getBound().valid() )
			{
				CGlbCalculateBoundBoxVisitor bboxV;
				objNode->accept(bboxV);
				osg::BoundingBoxd bb =bboxV.getBoundBox();

				//osg::BoundingSphere bs = objNode->getBound();
				//osg::BoundingBoxd bb;
				//bb.expandBy(bs._center - Vec3d(bs._radius,bs._radius,bs._radius));
				//bb.expandBy(bs._center + Vec3d(bs._radius,bs._radius,bs._radius));
				worldExt->SetMin(bb.xMin(),bb.yMin(),bb.zMin());
				worldExt->SetMax(bb.xMax(),bb.yMax(),bb.zMax());
			}
			else
			{//osg 节点还没生成.				
				double x,y,z;
				geoExt->GetMin(&x,&y,&z);
				if( mpr_elevation == DBL_MAX)
					UpdateElevate();
				if(mpt_altitudeMode == GLB_ALTITUDEMODE_RELATIVETERRAIN)
				{					
					z += mpr_elevation;
				}
				if(globeType == GLB_GLOBETYPE_GLOBE)
				{
					g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(y),
						osg::DegreesToRadians(x),z,x,y,z);					
				}
				worldExt->Merge(x,y,z);

				geoExt->GetMax(&x,&y,&z);
				if(mpt_altitudeMode == GLB_ALTITUDEMODE_RELATIVETERRAIN)
				{					
					z += mpr_elevation;
				}
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

void CGlbGlobeLine::UpdateElevate()
{
	if (!mpt_globe) 
		return;
	if(mpt_altitudeMode == GLB_ALTITUDEMODE_ABSOLUTE)
	{
		//贴地形绘制,GetDistance()是会用到mpr_elevate.
		return;
	}

	glbDouble elevation = 0.0;
	if (mpt_feature.get())
	{
		CGlbExtent* ext = const_cast<CGlbExtent *>(mpt_feature->GetExtent());
		if (ext)
		{
			glbDouble xOrLon,yOrLat;
			ext->GetCenter(&xOrLon,&yOrLat);
			elevation = mpt_globe->GetElevationAt(xOrLon,yOrLat);
		}
	}
	else
	{
		if(mpr_lineGeo == NULL)return;
		const glbDouble *points = mpr_lineGeo->GetPoints();
		elevation = mpt_globe->GetElevationAt(points[0],points[1]);
	}

	if(fabs(elevation - mpr_elevation) < 0.0001)
		return;	
	mpr_elevation = elevation;

	if(mpt_altitudeMode == GLB_ALTITUDEMODE_RELATIVETERRAIN)
	{//更新Maxtrixform
		DealModePosByChangeAltitudeOrChangePos();
	}
}

/*
*   获取2D绘制时的外围轮廓.
*   轮廓 ------>多线(闭合或不闭合)CGlbMultiLine
*/
IGlbGeometry * CGlbGlobeLine::GetOutLine()
{// 返回类型 CGlbMultiLine - 贴地形对象 
	//		 CGlbMultiLine3D - 非贴地形绘制 
	if(mpr_outline != NULL)
		return mpr_outline.get();

	glbref_ptr<IGlbGeometry> geom = AnalysisLineGeomertry(mpt_feature.get(),mpr_lineGeo.get());
	if (geom)
	{
		GlbScopedLock<GlbCriticalSection> lock(mpr_outline_critical);
		mpr_outline = geom;
		return mpr_outline.get();
	}
	//if (mpt_feature.get())
	//{// 要素对象的情况，返回NULL
	//	
	//}
	//else if (mpr_lineGeo)
	//{		
	//	CGlbMultiLine * multiline = new CGlbMultiLine();
	//	CGlbLine* line = new CGlbLine;
	//	glbInt32 num = mpr_lineGeo->GetCount();		
	//	glbDouble xOrLon,yOrLat,zOrAlt;
	//	for (glbInt32 i = 0;i < num;i++)
	//	{
	//		mpr_lineGeo->GetPoint(i,&xOrLon,&yOrLat,&zOrAlt);
	//		line->AddPoint(xOrLon,yOrLat);
	//	}
	//	multiline->AddLine(line);
	//	mpr_outline = multiline;
	//			
	//	return mpr_outline;
	//}
	return NULL;
}

glbBool CGlbGlobeLine::SetRenderInfo( GlbRenderInfo *renderInfo,glbBool &isNeedDirectDraw)
{
	isNeedDirectDraw = false;
	if (renderInfo == NULL || renderInfo->type != GLB_OBJECTTYPE_LINE)
		return false;
	GlbLineSymbolInfo *lineInfo = dynamic_cast<GlbLineSymbolInfo *>(renderInfo);
	if (lineInfo == NULL) return false;		
	// 动态箭头对象不允许被选中,以提高鼠标点选速度，避免操控器卡顿
	if (lineInfo->symType == GLB_LINESYMBOL_DYNAMICARROW)
		mpt_isEnableSelect = false;

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
		return true;
	}

	GlbLineSymbolInfo *templineInfo = static_cast<GlbLineSymbolInfo*>(mpr_renderInfo.get());	
	if(lineInfo->symType != templineInfo->symType)
	{
		isNeedDirectDraw = true;
		mpr_renderInfo   = renderInfo;
		ParseObjectFadeColor();
		mpt_loadState    = false;
		return true;
	}	

	glbBool rt = false;			
	switch(lineInfo->symType)
	{
	case GLB_LINESYMBOL_PIXEL:
		{
			GlbLinePixelSymbolInfo *linePixelInfo = static_cast<GlbLinePixelSymbolInfo*>(lineInfo);
			if (linePixelInfo != NULL) 		
				rt = DealPixelSymbol(linePixelInfo,isNeedDirectDraw);
		}
		break;
	case GLB_LINESYMBOL_2D:
		{
			GlbLine2DSymbolInfo *line2DInfo = static_cast<GlbLine2DSymbolInfo*>(lineInfo);
			if (line2DInfo != NULL)
			{// 未写完，待实现 ！！！！！
			}
		}
		break;
	case GLB_LINESYMBOL_3D:
		{
			GlbLine3DSymbolInfo *line3DInfo = static_cast<GlbLine3DSymbolInfo*>(lineInfo);
			if (line3DInfo != NULL)
			{// 未写完，待实现 ！！！！！
			}
		}
		break;
	case GLB_LINESYMBOL_ARROW:
		{
			GlbLineArrowSymbolInfo *lineArrowInfo = static_cast<GlbLineArrowSymbolInfo*>(lineInfo);
			if (lineArrowInfo != NULL) 
				rt = DealArrowSymbol(lineArrowInfo,isNeedDirectDraw);		
		}
		break;
	case GLB_LINESYMBOL_DYNAMICARROW:
		{
			GlbLineDynamicArrowSymbolInfo *dynamicArrowInfo = static_cast<GlbLineDynamicArrowSymbolInfo *>(lineInfo);
			if(dynamicArrowInfo != NULL)
				rt = DealDynamicArrowSymbol(dynamicArrowInfo,isNeedDirectDraw);
		}
		break;
	}

	//DealOpacity(mpt_node,renderInfo);
	if(rt)
	{
		mpr_renderInfo = renderInfo;
		ParseObjectFadeColor();
	}
	mpt_loadState  = false;
	return rt;
}

GlbRenderInfo * CGlbGlobeLine::GetRenderInfo()
{
	return mpr_renderInfo.get();
}

glbBool CGlbGlobeLine::SetShow( glbBool isShow ,glbBool isOnState)
{	
	/*
	* 调度器根据可见范围控制对象显示和隐藏此时isOnState==true,isShow==true或false
	*/

	glbBool isold = (mpt_isDispShow && mpt_isShow)?true:false;

	if(isOnState)
	{// 调度器控制显隐
		mpt_isDispShow = isShow;
	}
	else
	{// 用户控制显隐
		mpt_isShow = isShow;
	}

	glbBool isnew = (mpt_isDispShow && mpt_isShow)?true:false;

	if(isold == isnew)
		return true;
	mpr_isCanDrawImage = isnew;
	if (mpt_isEdit && isOnState)// 编辑状态不再由调度控制显隐
		return true;

	glbref_ptr<GlbLineSymbolInfo> lineInfo = static_cast<GlbLineSymbolInfo *>(mpr_renderInfo.get());
	if (!lineInfo)	return false;		

	if( mpt_altitudeMode == GLB_ALTITUDEMODE_ONTERRAIN )
	{// 如果是贴地绘制对象，需要刷新贴地区域
		/*
		* mpr_isCanDrawImage控制在DrawToImage时是否需要绘制
		*/
		//if (mpr_isCanDrawImage)
		{
			// 刷新贴地区域
			DirtyOnTerrainObject();		
		}
	}
	else
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

glbBool CGlbGlobeLine::SetSelected( glbBool isSelected )
{
	if (mpt_isSelected == isSelected) return true;
	mpt_isSelected = isSelected;
	if (mpt_node != NULL)
	{
		if(GLB_ALTITUDEMODE_ONTERRAIN == mpt_altitudeMode)
			DirtyOnTerrainObject();		
		else
			DealSelected();
	}
	return true;
}

glbBool CGlbGlobeLine::SetBlink( glbBool isBlink )
{
	if(mpt_isBlink == isBlink) return true;
	mpt_isBlink = isBlink;
	if(mpt_node != NULL && mpt_altitudeMode!=GLB_ALTITUDEMODE_ONTERRAIN)
		DealBlink();
	return true;
}

void CGlbGlobeLine::DirectDraw( glbInt32 level )
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

glbBool CGlbGlobeLine::SetAltitudeMode( GlbAltitudeModeEnum mode ,glbBool &isNeedDirectDraw)
{	
	if (mpt_altitudeMode == mode) return true;
	isNeedDirectDraw = false;
	if(mpt_globe == NULL)
	{
		mpt_altitudeMode = mode;
		return true;
	}	

	{   		
		GlbScopedLock<GlbCriticalSection> lock(mpr_readData_critical);
		if (mpt_loadState == true)
		{
			/*
			*  如果正在ReadData，ReadData可能在新的地形模式之前使用了旧的模式.
			*  所以：
			需要告知调用者，调用DirectDraw.
			*/
			isNeedDirectDraw = true;			
			mpt_altitudeMode = mode;						
			return true;
		}
		mpt_loadState = true;
	}
	if (mpt_node == NULL)
	{
		mpt_altitudeMode = mode;		
	}
	else
	{
		if(   mpt_altitudeMode == GLB_ALTITUDEMODE_ONTERRAIN
			||mode             == GLB_ALTITUDEMODE_ONTERRAIN)
		{//贴地模式 --->非贴地模式
			//非贴地模式-->贴地模式 
			isNeedDirectDraw = true;			
			mpt_globe->mpr_sceneobjIdxManager->RemoveObject(this);	
			if (mpt_altitudeMode == GLB_ALTITUDEMODE_ONTERRAIN)
			{//旧模式是贴地模式,新模式不是贴地模式.				
				mpr_isCanDrawImage = false;			
				DirtyOnTerrainObject();		
				GlbScopedLock<GlbCriticalSection> lock(mpr_outline_critical);
				mpr_outline = NULL;
			}
			else{//旧模式不是贴地，新模式是贴地
				RemoveNodeFromScene(true);//从场景树上移除节点
				mpr_isCanDrawImage = true;
			}
			mpt_altitudeMode = mode;
			/*
			贴地模式、相对模式会用到mpr_elevate.
			所以：
			调用UpdateElevate();
			*/
			if(mpt_altitudeMode != GLB_ALTITUDEMODE_ABSOLUTE)
				UpdateElevate();
			mpt_globe->mpr_sceneobjIdxManager->AddObject(this);			
		}	
		else
		{
			mpt_altitudeMode = mode;
			if (mpt_altitudeMode == GLB_ALTITUDEMODE_RELATIVETERRAIN)
			{
				UpdateElevate();
				/*
				UpdateElevate()中有一条件：if(fabs(elevation - mpr_elevation) < 0.0001)
				在模式变换时,很有可能不满足上述条件。
				所以：
				必须调用DealModePosByChangeAltitudeOrChangePos().
				*/
			}
			DealModePosByChangeAltitudeOrChangePos();			
		}
	}
	mpt_loadState = false;
	return true;
}

/*
只对贴地形绘制有作用.
*/
void CGlbGlobeLine::SetRenderOrder( glbInt32 order )
{
	if (mpt_renderOrder == order) 
		return;
	mpt_renderOrder = order;	
	if (mpt_altitudeMode==GLB_ALTITUDEMODE_ONTERRAIN)
	{
		DirtyOnTerrainObject();				
	}		
	CGlbGlobeREObject::SetRenderOrder(order);
}

CGlbLine * CGlbGlobeLine::GetGeo()
{
	return mpr_lineGeo.get();
}

void CGlbGlobeLine::ReadData( glbInt32 level , glbBool isDirect)
{
	glbref_ptr<GlbLineSymbolInfo> lineInfo = static_cast<GlbLineSymbolInfo *>(mpr_renderInfo.get());
	if (lineInfo == NULL)
	{mpt_loadState=false;return;}
	// 解析几何数据
	glbref_ptr<CGlbMultiLine> multiline3d = new CGlbMultiLine(3,false);
	if (mpt_feature.get())
	{	
		IGlbGeometry *geo = NULL;
		bool result = false;
		result = mpt_feature->GetGeometry(&geo,level);
		if(result==false || geo==NULL) 
		{mpt_loadState=false;return;}

		// 使用智能指针防止多线程访问导致geo在Draw过程中被卸载
		glbref_ptr<IGlbGeometry> tempGeo = geo;

		glbInt32 dimension = geo->GetCoordDimension();
		glbDouble height=0.0;//0.0 ！！！！！！！！测试用，如果是二维点，高度用此作为默认高度
		glbDouble xOrLon,yOrLat;
		switch(geo->GetType())
		{		
		case GLB_GEO_LINE:
			{
				CGlbLine* ln = dynamic_cast<CGlbLine*>(geo);
				glbInt32 num = ln->GetCount();
				CGlbLine* line3d = new CGlbLine(3,false);
				for (glbInt32 i = 0;i < num;i++)
				{
					if (dimension == 2)
						ln->GetPoint(i,&xOrLon,&yOrLat);
					else if (dimension == 3)
						ln->GetPoint(i,&xOrLon,&yOrLat,&height);
					line3d->AddPoint(xOrLon,yOrLat,height);
				}
				multiline3d->AddLine(line3d);
			}	
			break;
		case GLB_GEO_MULTILINE:
			{
				CGlbMultiLine* mulln = dynamic_cast<CGlbMultiLine*>(geo);
				glbInt32 linenum = mulln->GetCount();						
				for (glbInt32 i = 0;i < linenum;i++)
				{
					CGlbLine* ln = const_cast<CGlbLine *>(mulln->GetLine(i));
					glbInt32 num = ln->GetCount();
					CGlbLine* line3d = new CGlbLine(3,false);
					for (glbInt32 j = 0;j < num;j++)
					{
						if (dimension == 2)
							ln->GetPoint(j,&xOrLon,&yOrLat);
						else if (dimension == 3)
							ln->GetPoint(j,&xOrLon,&yOrLat,&height);				
						line3d->AddPoint(xOrLon,yOrLat,height);
					}
					multiline3d->AddLine(line3d);
				}
			}
			break;
		}
	}else{
		// 使用智能指针防止多线程访问导致mpr_lineGeo在Draw过程中被卸载
		multiline3d = new CGlbMultiLine(3,false);
		multiline3d->AddLine(mpr_lineGeo.get());
	}
	osg::Switch* swiNode = new osg::Switch; 
	if (mpt_altitudeMode == GLB_ALTITUDEMODE_ONTERRAIN)	//贴地
	{		
		//将旧的区域刷新			
		DirtyOnTerrainObject();			

		osg::Node *node = NULL;
		glbDouble yaw = 0.0 , pitch = 0.0 , roll = 0.0;
		glbDouble xScale = 1.0 , yScale = 1.0 , zScale = 1.0;		

		glbref_ptr<CGlbMultiLine> outlines =  new CGlbMultiLine;//dynamic_cast<CGlbMultiLine *>(mpr_outline.get());		
		if (lineInfo->symType == GLB_LINESYMBOL_PIXEL)
		{
			CGlbGlobeLinePixelSymbol linePixelSymbol;
			glbref_ptr<CGlbLine> outline = NULL;
			if (multiline3d)
			{
				for (glbInt32 i = 0; i < multiline3d->GetCount();i++)
				{
					CGlbLine* ln3d = const_cast<CGlbLine *>(multiline3d->GetLine(i));
					outline = dynamic_cast<CGlbLine*>(linePixelSymbol.GetOutline(this,ln3d));
					outlines->AddLine(outline.get());
				}				

				// 添加新的地形“脏”区域
				GlbScopedLock<GlbCriticalSection> lock(mpr_outline_critical);
				mpr_outline = outlines;
				DirtyOnTerrainObject();
			}			
		}
		else if (lineInfo->symType == GLB_LINESYMBOL_2D)
		{
		}
		else if (lineInfo->symType == GLB_LINESYMBOL_3D)
		{
		}
		else if (lineInfo->symType == GLB_LINESYMBOL_ARROW ||
			        lineInfo->symType == GLB_LINESYMBOL_DYNAMICARROW)
		{
			CGlbGlobeLineArrowSymbol lineArrowSymbol;
			if (multiline3d)
			{
				for (glbInt32 i = 0; i < multiline3d->GetCount();i++)
				{
					CGlbLine* ln3d = const_cast<CGlbLine *>(multiline3d->GetLine(i));
					CGlbMultiLine* outline = dynamic_cast<CGlbMultiLine*>(lineArrowSymbol.GetOutline(this,ln3d));
					for (glbInt32 j = 0;j < outline->GetCount();j++)
					{
						CGlbLine *line = const_cast<CGlbLine *>(outline->GetLine(j));
						outlines->AddLine(line);
					}					
				}

				GlbScopedLock<GlbCriticalSection> lock(mpr_outline_critical);
				mpr_outline = outlines;			
				// 添加地面刷新区域
				DirtyOnTerrainObject();
			}			
		}
	}
	else
	{	//非贴地	
		if (lineInfo->symType == GLB_LINESYMBOL_MODEL)
		{
			CGlbGlobeLineModelSymbol lineModelSymbol;
			if (multiline3d)
			{
				for (glbInt32 i = 0; i < multiline3d->GetCount();i++)
				{
					CGlbLine* ln3d = const_cast<CGlbLine *>(multiline3d->GetLine(i));
					osg::Node* node = lineModelSymbol.Draw(this,ln3d);
					if (node)
					{// 
						swiNode->addChild(node);
					}
				}		
				//mpr_outline = multiline3d;
			}			
		}
		else
		{
			for (glbInt32 i = 0; i < multiline3d->GetCount();i++)
			{
				CGlbLine* line3d = const_cast<CGlbLine *>(multiline3d->GetLine(i));
				swiNode->addChild(createline(line3d));		
			}	
		}			
	}
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
		// 预先计算bound，节省时间
		swiNode->getBound();

		mpt_preNode   = mpt_node;
		mpt_node      = swiNode;
		mpt_currLevel = level;		
		mpr_objSize   = objsize;
		//if(true)
		//{//测试输出
		//	double um = mpr_objSize;
		//	um = um/1024.0/1024.0;		
		//	GlbLogWOutput(GLB_LOGTYPE_INFO,L"obj %s: used %lf\n",GetName(),um);
		//}
	}else{
		swiNode = NULL;
	}
	mpt_loadState = false;	
}

osg::MatrixTransform* CGlbGlobeLine::createline(CGlbLine* line3d)
{
	osg::ref_ptr<osg::Node> node = NULL;
	glbDouble yaw = 0.0 , pitch = 0.0 , roll = 0.0;
	glbDouble xScale = 1.0 , yScale = 1.0 , zScale = 1.0;

	glbref_ptr<GlbLineSymbolInfo> lineInfo = dynamic_cast<GlbLineSymbolInfo *>(mpr_renderInfo.get());
	if (lineInfo == NULL) return NULL;
	if (lineInfo->symType == GLB_LINESYMBOL_PIXEL)
	{
		GlbLinePixelSymbolInfo *linePixelInfo = 
			dynamic_cast<GlbLinePixelSymbolInfo*>(lineInfo.get());
		if(linePixelInfo == NULL) return NULL;
		CGlbGlobeLinePixelSymbol linePixelSymbol;
		node = linePixelSymbol.Draw(this,line3d);
		mpr_outline = linePixelSymbol.GetOutline(this,line3d);
	}
	else if (lineInfo->symType == GLB_LINESYMBOL_2D)
	{
	}
	else if (lineInfo->symType == GLB_LINESYMBOL_3D)
	{
	}
	else if (lineInfo->symType == GLB_LINESYMBOL_ARROW)
	{
		GlbLineArrowSymbolInfo *lineArrowInfo = 
			dynamic_cast<GlbLineArrowSymbolInfo*>(lineInfo.get());
		if(lineArrowInfo == NULL) return NULL;
		CGlbGlobeLineArrowSymbol lineArrowSymbol;
		node = lineArrowSymbol.Draw(this,line3d);
		mpr_outline = lineArrowSymbol.GetOutline(this,line3d);
		roll = lineArrowInfo->roll->GetValue(mpt_feature.get());
		//根据箭头起始终止点计算yaw，pitch,(2个顶点)
		osg::Vec3d ptfirst,ptsecond;
		line3d->GetPoint(0,&ptfirst.x(),&ptfirst.y(),&ptfirst.z());	//经纬高
		line3d->GetPoint(1,&ptsecond.x(),&ptsecond.y(),&ptsecond.z());
		if (mpt_globe->GetType() == GLB_GLOBETYPE_GLOBE)
		{
			osg::Matrix locTowor;
			g_ellipsoidModel->computeLocalToWorldTransformFromLatLongHeight(osg::DegreesToRadians(ptfirst.y()),osg::DegreesToRadians(ptfirst.x()),ptfirst.z(),locTowor);
			g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(ptfirst.y()),osg::DegreesToRadians(ptfirst.x()),ptfirst.z(),ptfirst.x(),ptfirst.y(),ptfirst.z());
			g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(ptsecond.y()),osg::DegreesToRadians(ptsecond.x()),ptsecond.z(),ptsecond.x(),ptsecond.y(),ptsecond.z());
			//osg::Vec3d arrowVec = ptsecond-ptfirst;
			ptsecond = ptsecond * osg::Matrix::inverse(locTowor);
			ptsecond.normalize();
			if (ptsecond.z() > 1.0)
				ptsecond.z() = 1.0;
			else if (ptsecond.z() < -1.0)
				ptsecond.z() = -1.0;
			pitch = osg::RadiansToDegrees(asin(ptsecond.z()));
			yaw = osg::RadiansToDegrees(atan2(-ptsecond.x(),ptsecond.y()));
		}
	}
	else if(lineInfo->symType == GLB_LINESYMBOL_DYNAMICARROW)
	{
		GlbLineDynamicArrowSymbolInfo *lineDynamicArrowInfo = 
			dynamic_cast<GlbLineDynamicArrowSymbolInfo *>(lineInfo.get());
		if(lineDynamicArrowInfo == NULL) return NULL;
		CGlbGlobeLineDynamicArrowSymbol lineDynamicArrowSymbol;
		if(line3d->GetCount()>1)
		{
			node = lineDynamicArrowSymbol.Draw(this,line3d);
			mpr_outline = lineDynamicArrowSymbol.GetOutline(this,line3d);
		}
	}

	if (node == NULL)
		return NULL;
	osg::MatrixTransform *mt = new osg::MatrixTransform;
	osg::Matrix mat;
	if (line3d)
	{
		const glbDouble *points = line3d->GetPoints();
		osg::Vec3d point(points[0],points[1],points[2]);

		//if (lineInfo->symType == GLB_LINESYMBOL_PIXEL)
		//{
		//	if(mpt_globe->GetType() == GLB_GLOBETYPE_GLOBE)
		//		g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(point.y()),osg::DegreesToRadians(point.x()),point.z(),point.x(),point.y(),point.z());
		//	mat.makeTranslate(point);
		//}else if (lineInfo->symType == GLB_LINESYMBOL_ARROW)
		//{
		//if(mpt_globe->GetType() == GLB_GLOBETYPE_GLOBE)
		//	g_ellipsoidModel->computeLocalToWorldTransformFromLatLongHeight(osg::DegreesToRadians(point.y()),osg::DegreesToRadians(point.x()),point.z(),mat);
		//else if (mpt_globe->GetType() == GLB_GLOBETYPE_FLAT)
		//	mat.makeTranslate(point);
		//}

		ComputePosByAltitudeAndGloleType(point);
		if(mpt_globe->GetType() == GLB_GLOBETYPE_GLOBE)
			g_ellipsoidModel->computeLocalToWorldTransformFromXYZ(point.x(),point.y(),point.z(),mat);
		else if (mpt_globe->GetType() == GLB_GLOBETYPE_FLAT)
			mat.makeTranslate(point);

		yaw = osg::DegreesToRadians(yaw);
		pitch = osg::DegreesToRadians(pitch);
		roll = osg::DegreesToRadians(roll);
		osg::Vec3d yaw_vec(0.0,0.0,1.0);
		osg::Vec3d pitch_vec(cos(yaw),sin(yaw),0.0);
		osg::Vec3d roll_vec(-sin(yaw)*cos(pitch),cos(pitch)*cos(yaw),sin(pitch));
		osg::Quat quat;
		quat.makeRotate(yaw,yaw_vec,pitch,pitch_vec,roll,roll_vec);
		mat.preMultRotate(quat);
		mat.preMultScale(osg::Vec3d(xScale,yScale,zScale));

		mt->setMatrix(mat);
	}
	mt->addChild(node);
	return mt;
}

void CGlbGlobeLine::ComputePosByAltitudeAndGloleType( osg::Vec3d &position )
{
	if (mpt_globe->GetType() == GLB_GLOBETYPE_GLOBE)
	{
		if (mpt_altitudeMode == GLB_ALTITUDEMODE_ABSOLUTE)
		{
			//把xOrLon、yOrLat、zOrAlt的值转换成世界坐标赋值给position的x、y、z
			g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(position.y()),osg::DegreesToRadians(position.x()),position.z(),position.x(),position.y(),position.z());
		}
		else if (mpt_altitudeMode == GLB_ALTITUDEMODE_RELATIVETERRAIN)
		{
			//double xOrLon = position.x(),yOrLat = position.y();
			//position.z() = position.z() + mpt_globe->GetElevationAt(xOrLon,yOrLat);
			//把xOrLon、yOrLat、zOrAlt的值转换成世界坐标赋值给position的x、y、z
			g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(position.y()),osg::DegreesToRadians(position.x()),position.z() + mpr_elevation,position.x(),position.y(),position.z());			
		}
		else if (mpt_altitudeMode == GLB_ALTITUDEMODE_ONTERRAIN)
		{
			g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(position.y()),
				osg::DegreesToRadians(position.x()),mpr_elevation,position.x(),position.y(),position.z());
		}
	}
	else if (mpt_globe->GetType() == GLB_GLOBETYPE_FLAT)
	{
		//position.z() = position.z() + mpt_globe->GetElevationAt(position.x(),position.y());
		if(mpt_altitudeMode == GLB_ALTITUDEMODE_ABSOLUTE)
			position = osg::Vec3d(position.x(),position.y(),position.z());
		else if (mpt_altitudeMode == GLB_ALTITUDEMODE_RELATIVETERRAIN)
		{
			position = osg::Vec3d(position.x(),position.y(),position.z()+mpr_elevation);			
		}
		else if (mpt_altitudeMode == GLB_ALTITUDEMODE_ONTERRAIN)
		{
			position = osg::Vec3d(position.x(),position.y(),mpr_elevation);
		}
	}
}

glbInt32 CGlbGlobeLine::ComputeNodeSize( osg::Node *node )
{
	CGlbCalculateNodeSizeVisitor cnsv;
	node->accept(cnsv);
	return cnsv.getTextureSize() + cnsv.getNodeMemSize();
}

void CGlbGlobeLine::DealSelected()
{
	osg::StateSet *stateset = mpt_node->getOrCreateStateSet();
	osg::ref_ptr<osg::Material> material = dynamic_cast<osg::Material *>(stateset->getAttribute(osg::StateAttribute::MATERIAL));
	if(material == NULL)
		material = new osg::Material;
	if (mpt_isSelected)
	{
		material->setDiffuse(osg::Material::FRONT_AND_BACK,osg::Vec4(0,0,0,1));
		material->setEmission(osg::Material::FRONT_AND_BACK,osg::Vec4(1,0,0,1));//像素线受光照影像会变黑，使用了自发光之后颜色就正常了
		stateset->setAttribute(material.get(),osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
		stateset->setMode(GL_LIGHTING,osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
	}
	else
	{
		if(material)
		{
			//stateset->removeAttribute(material);
			mpt_node->setStateSet(NULL);
		}
		else
			DealOpacity(mpt_node,mpr_renderInfo.get());
	}
}

void CGlbGlobeLine::DealOpacity(osg::Node *node,GlbRenderInfo *renderInfo)
{
	if (mpt_isSelected)
		return;
	osg::ref_ptr<osg::StateSet> stateset = node->getOrCreateStateSet();
	stateset->setMode( GL_NORMALIZE, osg::StateAttribute::ON );
	osg::ref_ptr<osg::Material> material = dynamic_cast<osg::Material *>(stateset->getAttribute(osg::StateAttribute::MATERIAL));

	GlbLineSymbolInfo *lineInfo = dynamic_cast<GlbLineSymbolInfo *>(renderInfo);
	if (lineInfo == NULL)
		return;		
	switch(lineInfo->symType)
	{
	case GLB_LINESYMBOL_PIXEL:
		{
			GlbLinePixelSymbolInfo *linePixelInfo = static_cast<GlbLinePixelSymbolInfo*>(lineInfo);
			if (linePixelInfo && linePixelInfo->lineOpacity) 
			{
				glbInt32 opacity = linePixelInfo->lineOpacity->GetValue(mpt_feature.get());
				if (opacity<100)
				{
					if(material == NULL)
						material = new osg::Material;
					material->setAlpha(osg::Material::FRONT_AND_BACK,opacity / 100.0);
					stateset->setAttribute(material,osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
					stateset->setMode(GL_LIGHTING,osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
					stateset->setMode(GL_BLEND,osg::StateAttribute::ON);
					if (stateset->getRenderingHint() != osg::StateSet::TRANSPARENT_BIN)
						stateset->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
				}
				else
				{
					if((!mpt_isSelected) && (!mpt_isBlink))
					{
						stateset->setMode(GL_LIGHTING,osg::StateAttribute::OFF | osg::StateAttribute::OVERRIDE);
						if (material)
							stateset->removeAttribute(material);
					}
					stateset->setMode(GL_BLEND,osg::StateAttribute::OFF);
					stateset->setRenderingHint(osg::StateSet::OPAQUE_BIN);			
				}	
			}else{
				stateset->setMode(GL_BLEND,osg::StateAttribute::OFF);
				stateset->setRenderingHint(osg::StateSet::OPAQUE_BIN);
				if(!mpt_isSelected || !mpt_isBlink)
					stateset->setMode(GL_LIGHTING,osg::StateAttribute::OFF|osg::StateAttribute::OVERRIDE);
			}
		}
		break;
	case GLB_LINESYMBOL_2D:
		break;
	case GLB_LINESYMBOL_3D:
		break;
	case GLB_LINESYMBOL_ARROW:
		{
			GlbLineArrowSymbolInfo *lineArrowInfo = static_cast<GlbLineArrowSymbolInfo*>(lineInfo);
			if (lineArrowInfo && lineArrowInfo->fillOpacity)	//以箭头填充的Opacity为准
			{
				glbInt32 opacity = lineArrowInfo->fillOpacity->GetValue(mpt_feature.get());
				if (opacity<100)
				{
					if(material == NULL)
						material = new osg::Material;
					material->setAlpha(osg::Material::FRONT_AND_BACK,opacity / 100.0);
					stateset->setAttribute(material,osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);

					stateset->setMode(GL_LIGHTING,osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
					stateset->setMode(GL_BLEND,osg::StateAttribute::ON);
					if (stateset->getRenderingHint() != osg::StateSet::TRANSPARENT_BIN)
						stateset->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
				}
				else
				{
					if((!mpt_isSelected) && (!mpt_isBlink))
					{
						stateset->setMode(GL_LIGHTING,osg::StateAttribute::OFF | osg::StateAttribute::OVERRIDE);
						if (material)
							stateset->removeAttribute(material);
					}
					stateset->setMode(GL_BLEND,osg::StateAttribute::OFF);
					stateset->setRenderingHint(osg::StateSet::OPAQUE_BIN);			
				}	
			}else{
				stateset->setMode(GL_BLEND,osg::StateAttribute::OFF);
				stateset->setRenderingHint(osg::StateSet::OPAQUE_BIN);
				if(!mpt_isSelected || !mpt_isBlink)
					stateset->setMode(GL_LIGHTING,osg::StateAttribute::OFF|osg::StateAttribute::OVERRIDE);
			}
		}
		break;
	case GLB_LINESYMBOL_MODEL:
		{
			GlbLineModelSymbolInfo *lineModelInfo = static_cast<GlbLineModelSymbolInfo*>(lineInfo);
			if (lineModelInfo && lineModelInfo->opacity) 
			{
				glbInt32 opacity = lineModelInfo->opacity->GetValue(mpt_feature.get());
				if (opacity<100)
				{
					if(material == NULL)
						material = new osg::Material;
					material->setAlpha(osg::Material::FRONT_AND_BACK,opacity / 100.0);
					stateset->setAttribute(material,osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);

					stateset->setMode(GL_LIGHTING,osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
					stateset->setMode(GL_BLEND,osg::StateAttribute::ON);
					if (stateset->getRenderingHint() != osg::StateSet::TRANSPARENT_BIN)
						stateset->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
				}
				else
				{
					if((!mpt_isSelected) && (!mpt_isBlink))
					{
						stateset->setMode(GL_LIGHTING,osg::StateAttribute::OFF | osg::StateAttribute::OVERRIDE);
						if (material)
							stateset->removeAttribute(material);
					}
					stateset->setMode(GL_BLEND,osg::StateAttribute::OFF);
					stateset->setRenderingHint(osg::StateSet::OPAQUE_BIN);			
				}	
			}else{
				stateset->setMode(GL_BLEND,osg::StateAttribute::OFF);
				stateset->setRenderingHint(osg::StateSet::OPAQUE_BIN);
				if(!mpt_isSelected || !mpt_isBlink)
					stateset->setMode(GL_LIGHTING,osg::StateAttribute::OFF|osg::StateAttribute::OVERRIDE);
			}
		}
		break;
	default:
		break;
	}
}

void CGlbGlobeLine::DealBlink()
{
	if(mpt_isBlink)
	{
		LineObjectCallback *loCallback = new LineObjectCallback();
		mpt_node->addUpdateCallback(loCallback);
	}
	else
	{
		osg::NodeCallback *callback = mpt_node->getUpdateCallback();
		if (callback)
		{
			mpt_node->removeUpdateCallback(callback);
			DirectDraw(0);
		}
		DealSelected();
	}
}

glbBool CGlbGlobeLine::DealPixelSymbol( GlbLinePixelSymbolInfo *linePixelInfo,glbBool &isNeedDirectDraw )
{
	//考虑贴地模式
	if(mpt_altitudeMode == GLB_ALTITUDEMODE_ONTERRAIN)
	{
		isNeedDirectDraw = true;
		return true;
	}
	GlbLinePixelSymbolInfo *tempInfo = static_cast<GlbLinePixelSymbolInfo*>(mpr_renderInfo.get());
	if(tempInfo == NULL) return false;
	for(glbInt32 i = 0; i < mpt_node->asSwitch()->getNumChildren(); i++)
	{
		osg::MatrixTransform *mt = dynamic_cast<osg::MatrixTransform *>(mpt_node->asSwitch()->getChild(i));
		if(mt == NULL) 
			return false;
		osg::Geode *geode = dynamic_cast<osg::Geode *>(mt->getChild(0));
		if(geode == NULL) 
			return false;
		osg::Geometry *lineGeometry = dynamic_cast<osg::Geometry*>(geode->getDrawable(0));
		if(lineGeometry == NULL) 
			return false;
		osg::StateSet *stateset = geode->getOrCreateStateSet();
		//线宽
		osg::LineWidth *lineWidth = new osg::LineWidth;
		lineWidth->setWidth(linePixelInfo->lineWidth->GetValue(mpt_feature.get()));
		stateset->setAttributeAndModes(lineWidth,osg::StateAttribute::ON);
		//材质
		osg::ref_ptr<osg::Material> material = dynamic_cast<osg::Material*>(stateset->getAttribute(osg::StateAttribute::MATERIAL));
		if (!material.valid())
			material = new osg::Material;
		stateset->setAttribute(material.get(),osg::StateAttribute::ON);	
		stateset->setMode(GL_DEPTH_TEST,osg::StateAttribute::ON);
		material->setDiffuse(osg::Material::FRONT_AND_BACK,osg::Vec4(0.0,0.0,0.0,(glbFloat)linePixelInfo->lineOpacity->GetValue(mpt_feature.get())/100.0));
		material->setEmission(osg::Material::FRONT_AND_BACK,osg::Vec4(GetBValue(linePixelInfo->lineColor->GetValue(mpt_feature.get()))/255.0,LOBYTE(((linePixelInfo->lineColor->GetValue(mpt_feature.get()) & 0xFFFF)) >> 8)/255.0,GetRValue(linePixelInfo->lineColor->GetValue(mpt_feature.get()))/255.0,(glbFloat)linePixelInfo->lineOpacity->GetValue(mpt_feature.get())/100.0));
		if (linePixelInfo->lineOpacity->GetValue(mpt_feature.get()) < 100 || mpt_isBlink)
		{
			stateset->setMode(GL_BLEND,osg::StateAttribute::ON);
			if (stateset->getRenderingHint() != osg::StateSet::TRANSPARENT_BIN)
				stateset->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
		}else{
			stateset->setMode(GL_BLEND,osg::StateAttribute::OFF);
			stateset->setRenderingHint(osg::StateSet::OPAQUE_BIN);
		}
		if (linePixelInfo->linePattern->GetValue(mpt_feature.get()) == GLB_LINE_DOTTED)
		{
			//线样式
			osg::LineStipple *lineStipple = new osg::LineStipple;
			lineStipple->setFactor(1);
			lineStipple->setPattern(0x1C47);
			stateset->setAttributeAndModes(lineStipple,osg::StateAttribute::ON);
		}else{
			//线样式
			osg::LineStipple *lineStipple = new osg::LineStipple;
			lineStipple->setFactor(0);
			stateset->setAttributeAndModes(lineStipple,osg::StateAttribute::ON);
		}
	}
	return true;
}

glbBool CGlbGlobeLine::DealArrowSymbol(GlbLineArrowSymbolInfo *lineArrowInfo,glbBool &isNeedDirectDraw)
{
	GlbLineArrowSymbolInfo *oldInfo = static_cast<GlbLineArrowSymbolInfo*>(mpr_renderInfo.get());
	if(oldInfo == NULL)
		return false;
	// 考虑贴地模式
	if (mpt_altitudeMode == GLB_ALTITUDEMODE_ONTERRAIN)
	{
		isNeedDirectDraw = true;
		return true;
	}

	glbInt32 numChild = mpt_node->asSwitch()->getNumChildren();
	for(glbInt32 i = 0; i < numChild; i++)
	{
		osg::MatrixTransform *mt = dynamic_cast<osg::MatrixTransform *>(mpt_node->asSwitch()->getChild(i));
		if(mt == NULL) 
			continue;
		osg::Geode* geode = dynamic_cast<osg::Geode*>(mt->getChild(0));	//箭头叶节点
		if(geode == NULL) 
			continue;
		osg::Drawable *outline = dynamic_cast<osg::Drawable*>(geode->getDrawable(0));
		osg::Drawable *fillgeom = dynamic_cast<osg::Drawable*>(geode->getDrawable(1));
		if (outline == NULL)
			continue;
		if (fillgeom == NULL)
			continue;

		glbDouble yaw = 0.0 , pitch = 0.0 , roll = 0.0;
		glbDouble xScale = 1.0 , yScale = 1.0 , zScale = 1.0;

		//需要DirectDraw
		//箭头样式改变
		if (!CGlbCompareValue::CompareValueArrowPatternEnumEqual(oldInfo->pattern,lineArrowInfo->pattern,mpt_feature.get()))
		{
			isNeedDirectDraw = true;
			return true;
		}
		//箭头样式改变
		if (!CGlbCompareValue::CompareValueDoubleEqual(oldInfo->width,lineArrowInfo->width,mpt_feature.get()))
		{
			xScale = lineArrowInfo->height->GetValue(mpt_feature.get())/oldInfo->height->GetValue(mpt_feature.get());
			isNeedDirectDraw = true;
			return true;
		}
		//箭头厚度
		if (!CGlbCompareValue::CompareValueDoubleEqual(oldInfo->height,lineArrowInfo->height,mpt_feature.get()))
		{
			if ((oldInfo->height->GetValue(mpt_feature.get()))*(lineArrowInfo->height->GetValue(mpt_feature.get())) > 0)
			{	//直接拉伸z轴，zScale
				zScale = lineArrowInfo->height->GetValue(mpt_feature.get())/oldInfo->height->GetValue(mpt_feature.get());
				isNeedDirectDraw = true;
				return true;
			}
			else if(lineArrowInfo->height->GetValue(mpt_feature.get()) == 0 ||
				oldInfo->height ->GetValue(mpt_feature.get()) == 0)
			{
				isNeedDirectDraw = true;
				return true;
			}
		}
		//不需要DirectDraw
		//翻滚角
		if (!CGlbCompareValue::CompareValueDoubleEqual(oldInfo->roll,lineArrowInfo->roll,mpt_feature.get()))
		{
			roll = lineArrowInfo->roll->GetValue(mpt_feature.get());
		}
		//轮廓线
		if (oldInfo->linePixelInfo && lineArrowInfo->linePixelInfo)
		{
			//线样式
			if (!CGlbCompareValue::CompareValueLinePatternEnumEqual(oldInfo->linePixelInfo->linePattern,lineArrowInfo->linePixelInfo->linePattern,mpt_feature.get()))
			{
				if (lineArrowInfo->linePixelInfo->linePattern->GetValue(mpt_feature.get()) == GLB_LINE_DOTTED)
				{
					//点划线
					osg::LineStipple *lineStipple = new osg::LineStipple;
					lineStipple->setFactor(1);
					lineStipple->setPattern(0x1C47);
					outline->getOrCreateStateSet()->setAttributeAndModes(lineStipple,osg::StateAttribute::ON);
				}else{
					//实线
					osg::LineStipple *lineStipple = new osg::LineStipple;
					lineStipple->setFactor(0);
					outline->getOrCreateStateSet()->setAttributeAndModes(lineStipple,osg::StateAttribute::ON);
				}
			}
			//线像素宽
			if (!CGlbCompareValue::CompareValueDoubleEqual(oldInfo->linePixelInfo->lineWidth,lineArrowInfo->linePixelInfo->lineWidth,mpt_feature.get()))
			{
				//线宽
				osg::LineWidth *lineWidth = new osg::LineWidth;
				lineWidth->setWidth(lineArrowInfo->linePixelInfo->lineWidth->GetValue(mpt_feature.get()));
				outline->getOrCreateStateSet()->setAttributeAndModes(lineWidth,osg::StateAttribute::ON);
			}
			if (!CGlbCompareValue::CompareValueColorEqual(oldInfo->linePixelInfo->lineColor,lineArrowInfo->linePixelInfo->lineColor,mpt_feature.get())	   ||	!CGlbCompareValue::CompareValueIntEqual(oldInfo->linePixelInfo->lineOpacity,lineArrowInfo->linePixelInfo->lineOpacity,mpt_feature.get()))
			{
				//材质
				osg::ref_ptr<osg::Material> material = dynamic_cast<osg::Material*>(outline->getOrCreateStateSet()->getAttribute(osg::StateAttribute::MATERIAL));
				if (!material.valid())
					material = new osg::Material;
				outline->getOrCreateStateSet()->setAttribute(material.get(),osg::StateAttribute::ON);	
				outline->getOrCreateStateSet()->setMode(GL_DEPTH_TEST,osg::StateAttribute::ON);
				//material->setDiffuse(osg::Material::FRONT_AND_BACK,osg::Vec4(0.0,0.0,0.0,(glbFloat)lineArrowInfo->linePixelInfo->lineOpacity->GetValue(mpt_feature.get())/100.0));
				material->setDiffuse(osg::Material::FRONT_AND_BACK,osg::Vec4(GetBValue(lineArrowInfo->linePixelInfo->lineColor->GetValue(mpt_feature.get()))/255.0,LOBYTE(((lineArrowInfo->linePixelInfo->lineColor->GetValue(mpt_feature.get()) & 0xFFFF)) >> 8)/255.0,GetRValue(lineArrowInfo->linePixelInfo->lineColor->GetValue(mpt_feature.get()))/255.0,(glbFloat)lineArrowInfo->linePixelInfo->lineOpacity->GetValue(mpt_feature.get())/100.0));
				if (lineArrowInfo->linePixelInfo->lineOpacity->GetValue(mpt_feature.get()) < 100)
				{
					outline->getOrCreateStateSet()->setMode(GL_BLEND,osg::StateAttribute::ON);
					if (outline->getOrCreateStateSet()->getRenderingHint() != osg::StateSet::TRANSPARENT_BIN)
						outline->getOrCreateStateSet()->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
				}else{
					outline->getOrCreateStateSet()->setMode(GL_BLEND,osg::StateAttribute::OFF);
					outline->getOrCreateStateSet()->setRenderingHint(osg::StateSet::OPAQUE_BIN);
				}
			}
		}
		//填充
		if (!CGlbCompareValue::CompareValueDoubleEqual(oldInfo->width,lineArrowInfo->width,mpt_feature.get()))
		{
			xScale = lineArrowInfo->width->GetValue(mpt_feature.get())/oldInfo->width->GetValue(mpt_feature.get());
		}
		if (!CGlbCompareValue::CompareValueColorEqual(oldInfo->fillColor,lineArrowInfo->fillColor,mpt_feature.get()) || !CGlbCompareValue::CompareValueIntEqual(oldInfo->fillOpacity,lineArrowInfo->fillOpacity,mpt_feature.get()))
		{
			//材质
			osg::ref_ptr<osg::Material> material = dynamic_cast<osg::Material*>(fillgeom->getOrCreateStateSet()->getAttribute(osg::StateAttribute::MATERIAL));
			if (!material.valid())
				material = new osg::Material;
			fillgeom->getOrCreateStateSet()->setAttribute(material.get(),osg::StateAttribute::ON);	
			fillgeom->getOrCreateStateSet()->setMode(GL_DEPTH_TEST,osg::StateAttribute::ON);
			//material->setDiffuse(osg::Material::FRONT_AND_BACK,osg::Vec4(0.0,0.0,0.0,(glbFloat)lineArrowInfo->fillOpacity->GetValue(mpt_feature.get())/100.0));
			material->setDiffuse(osg::Material::FRONT_AND_BACK,osg::Vec4(GetBValue(lineArrowInfo->fillColor->GetValue(mpt_feature.get()))/255.0,LOBYTE(((lineArrowInfo->fillColor->GetValue(mpt_feature.get()) & 0xFFFF)) >> 8)/255.0,GetRValue(lineArrowInfo->fillColor->GetValue(mpt_feature.get()))/255.0,(glbFloat)lineArrowInfo->fillOpacity->GetValue(mpt_feature.get())/100.0));
			if (lineArrowInfo->fillOpacity->GetValue(mpt_feature.get()) < 100)
			{
				fillgeom->getOrCreateStateSet()->setMode(GL_BLEND,osg::StateAttribute::ON);
				if (fillgeom->getOrCreateStateSet()->getRenderingHint() != osg::StateSet::TRANSPARENT_BIN)
					fillgeom->getOrCreateStateSet()->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
			}else{
				fillgeom->getOrCreateStateSet()->setMode(GL_BLEND,osg::StateAttribute::OFF);
				fillgeom->getOrCreateStateSet()->setRenderingHint(osg::StateSet::OPAQUE_BIN);
			}
		}
		//纹理
		if (!CGlbCompareValue::CompareValueStringEqual(oldInfo->textureData,lineArrowInfo->textureData,mpt_feature.get()))
		{
			osg::ref_ptr<osg::Image> image = new osg::Image;
			CGlbWString texturePath = lineArrowInfo->textureData->GetValue(mpt_feature.get());
			glbInt32 index = texturePath.find_first_of(L'.');
			if(index == 0)
			{
				CGlbWString execDir = CGlbPath::GetExecDir();
				texturePath = execDir + texturePath.substr(1,texturePath.size());
			}
			image = osgDB::readImageFile(texturePath.ToString());
			if (image.valid())
			{
				osg::ref_ptr<osg::Texture2D> tex2d = 
					dynamic_cast<osg::Texture2D*>(fillgeom->getOrCreateStateSet()->getTextureAttribute(0,osg::StateAttribute::TEXTURE));
				if (!tex2d)
					tex2d = new osg::Texture2D;
				tex2d->setImage(0,image.get());
				osg::BoundingBox bb = geode->getBoundingBox();
				glbDouble max_x = bb.xMax();
				glbDouble min_x = bb.xMin();
				glbDouble max_y = bb.yMax();
				glbDouble min_y = bb.yMin();
				glbDouble max_z = bb.zMax();
				glbDouble min_z = bb.zMin();
				//设置纹理对象
				tex2d->setDataVariance(osg::Object::DYNAMIC);
				tex2d->setImage(image.get());
				//纹理滤波
				tex2d->setFilter(osg::Texture::MIN_FILTER,osg::Texture::LINEAR);
				tex2d->setFilter(osg::Texture::MAG_FILTER,osg::Texture::LINEAR);
				//纹理边界
				tex2d->setWrap(osg::Texture::WRAP_S,osg::Texture::REPEAT);
				tex2d->setWrap(osg::Texture::WRAP_T,osg::Texture::REPEAT);
				//自动生成纹理坐标
				osg::ref_ptr<osg::TexGen> texgen = new osg::TexGen;
				texgen->setMode(osg::TexGen::OBJECT_LINEAR);
				texgen->setPlane(osg::TexGen::S,osg::Plane(1.0/(max_x-min_x),0.0,0.0,-min_x/(max_x-min_x)));
				texgen->setPlane(osg::TexGen::T,osg::Plane(0.0,1.0/(max_y-min_y),0.0,-min_y/(max_y-min_y)));
				//texgen->setPlane(osg::TexGen::R,osg::Plane(0.0,0.0,1.0/(max_z-min_z),-min_z/(max_z-min_z)));
				fillgeom->getOrCreateStateSet()->setTextureAttributeAndModes(0,texgen.get(),osg::StateAttribute::ON);
				//纹理环境
				osg::ref_ptr<osg::TexEnv> texenv = new osg::TexEnv;
				texenv->setMode(osg::TexEnv::MODULATE);
				fillgeom->getOrCreateStateSet()->setTextureAttribute(0,texenv.get());
				fillgeom->getOrCreateStateSet()->setTextureAttributeAndModes(0,tex2d.get(),osg::StateAttribute::ON);	
				tex2d->setUnRefImageDataAfterApply(true);

				osg::TexMat *texMat = 
					dynamic_cast<osg::TexMat*>(fillgeom->getOrCreateStateSet()->getTextureAttribute(0,osg::StateAttribute::TEXMAT));
				if (!texMat)
					texMat = new osg::TexMat;
				fillgeom->getOrCreateStateSet()->setAttributeAndModes(texMat,osg::StateAttribute::ON);
				glbDouble xtexScale = 1.0,ytexScale = 1.0,ztexScale = 1.0;
				xtexScale = lineArrowInfo->tilingU->GetValue(mpt_feature.get());
				ytexScale = lineArrowInfo->tilingV->GetValue(mpt_feature.get());
				glbDouble angle = 0.0;
				angle = lineArrowInfo->textureRotation->GetValue(mpt_feature.get());
				glbDouble Arrowlength = 1.0;
				osg::Vec3d pt0,pt1;
				mpr_lineGeo->GetPoint(2*i,&pt0.x(),&pt0.y(),&pt0.z());
				mpr_lineGeo->GetPoint(2*i+1,&pt1.x(),&pt1.y(),&pt1.z());
				if (mpt_globe->GetType() == GLB_GLOBETYPE_GLOBE)
				{
					g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(pt0.y()),osg::DegreesToRadians(pt0.x()),pt0.z(),pt0.x(),pt0.y(),pt0.z());
					g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(pt1.y()),osg::DegreesToRadians(pt1.x()),pt1.z(),pt1.x(),pt1.y(),pt1.z());
				}
				Arrowlength = (pt1-pt0).length();
				glbDouble Arrowwidth = lineArrowInfo->width->GetValue(mpt_feature.get());
				//纹理平铺方式				
				if (lineArrowInfo->texRepeatMode->GetValue(mpt_feature.get()) == GLB_TEXTUREREPEAT_TIMES)
				{
					xtexScale = lineArrowInfo->tilingU->GetValue(mpt_feature.get());
					ytexScale = lineArrowInfo->tilingV->GetValue(mpt_feature.get());
				}else if (lineArrowInfo->texRepeatMode->GetValue(mpt_feature.get()) == GLB_TEXTUREREPEAT_SIZE)
				{
					xtexScale = lineArrowInfo->width->GetValue(mpt_feature.get())/lineArrowInfo->tilingU->GetValue(mpt_feature.get());
					ytexScale = Arrowlength/lineArrowInfo->tilingV->GetValue(mpt_feature.get());
				}
				angle = osg::DegreesToRadians(lineArrowInfo->textureRotation->GetValue(mpt_feature.get()));
				osg::Vec2 pt(Arrowwidth/2,Arrowlength/2);
				osg::Vec2 pt2(-Arrowwidth/2,Arrowlength/2);
				glbDouble ra = 2*pt.length();
				glbDouble ang = atan2(Arrowlength,Arrowwidth);
				glbDouble ang2 = atan2(Arrowlength,-Arrowwidth);
				glbDouble xm = ra*max(abs(cos(ang+angle)),abs(cos(ang2+angle)));
				glbDouble ym = ra*max(abs(sin(ang+angle)),abs(sin(ang2+angle)));			
				glbDouble sal =	ra*ra/(xm*xm+ym*ym);
				xtexScale *= sal;
				ytexScale *= sal;
				texMat->setMatrix(osg::Matrix::translate(-0.5,-0.5,0.0) * osg::Matrix::rotate(angle,0.0,0.0,1.0) * osg::Matrix::scale(xtexScale,ytexScale,ztexScale) * osg::Matrix::translate(0.5*lineArrowInfo->tilingU->GetValue(mpt_feature.get()),0.5*lineArrowInfo->tilingV->GetValue(mpt_feature.get()),0.0));
			}
		}else{
			osg::TexMat *texMat = 
				dynamic_cast<osg::TexMat*>(fillgeom->getOrCreateStateSet()->getTextureAttribute(0,osg::StateAttribute::TEXMAT));
			if (!texMat)
				texMat = new osg::TexMat;
			fillgeom->getOrCreateStateSet()->setAttributeAndModes(texMat,osg::StateAttribute::ON);
			glbDouble xtexScale = 1.0,ytexScale = 1.0,ztexScale = 1.0;
			xtexScale = lineArrowInfo->tilingU->GetValue(mpt_feature.get());
			ytexScale = lineArrowInfo->tilingV->GetValue(mpt_feature.get());
			glbDouble angle = 0.0;
			angle = lineArrowInfo->textureRotation->GetValue(mpt_feature.get());
			glbDouble Arrowlength = 1.0;
			osg::Vec3d pt0,pt1;
			mpr_lineGeo->GetPoint(2*i,&pt0.x(),&pt0.y(),&pt0.z());
			mpr_lineGeo->GetPoint(2*i+1,&pt1.x(),&pt1.y(),&pt1.z());
			if (mpt_globe->GetType() == GLB_GLOBETYPE_GLOBE)
			{
				g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(pt0.y()),osg::DegreesToRadians(pt0.x()),pt0.z(),pt0.x(),pt0.y(),pt0.z());
				g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(pt1.y()),osg::DegreesToRadians(pt1.x()),pt1.z(),pt1.x(),pt1.y(),pt1.z());
			}
			Arrowlength = (pt1-pt0).length();
			glbDouble Arrowwidth = lineArrowInfo->width->GetValue(mpt_feature.get());
			osg::Vec2 pt(Arrowwidth/2,Arrowlength/2);
			osg::Vec2 pt2(-Arrowwidth/2,Arrowlength/2);
			glbDouble ra = 2*pt.length();
			glbDouble ang = atan2(Arrowlength,Arrowwidth);
			glbDouble ang2 = atan2(Arrowlength,-Arrowwidth);
			glbDouble xm = ra*max(abs(cos(ang+angle)),abs(cos(ang2+angle)));
			glbDouble ym = ra*max(abs(sin(ang+angle)),abs(sin(ang2+angle)));			
			glbDouble sal =	ra*ra/(xm*xm+ym*ym);
			xtexScale *= sal;
			ytexScale *= sal;
			//纹理平铺方式
			if (!CGlbCompareValue::CompareValueTexRepeatEnumEqual(oldInfo->texRepeatMode,lineArrowInfo->texRepeatMode,mpt_feature.get()) || !CGlbCompareValue::CompareValueDoubleEqual(oldInfo->tilingU,lineArrowInfo->tilingU,mpt_feature.get()) || !CGlbCompareValue::CompareValueDoubleEqual(oldInfo->tilingV,lineArrowInfo->tilingV,mpt_feature.get()) || !CGlbCompareValue::CompareValueDoubleEqual(oldInfo->textureRotation,lineArrowInfo->textureRotation,mpt_feature.get()))
			{
				if (lineArrowInfo->texRepeatMode->GetValue(mpt_feature.get()) == GLB_TEXTUREREPEAT_TIMES)
				{
					xtexScale = lineArrowInfo->tilingU->GetValue(mpt_feature.get());
					ytexScale = lineArrowInfo->tilingV->GetValue(mpt_feature.get());
				}else if (lineArrowInfo->texRepeatMode->GetValue(mpt_feature.get()) == GLB_TEXTUREREPEAT_SIZE)
				{
					xtexScale = lineArrowInfo->width->GetValue(mpt_feature.get())/lineArrowInfo->tilingU->GetValue(mpt_feature.get());
					ytexScale = Arrowlength/lineArrowInfo->tilingV->GetValue(mpt_feature.get());
				}
				angle = osg::DegreesToRadians(lineArrowInfo->textureRotation->GetValue(mpt_feature.get()));
				osg::Vec2 pt(Arrowwidth/2,Arrowlength/2);
				osg::Vec2 pt2(-Arrowwidth/2,Arrowlength/2);
				glbDouble ra = 2*pt.length();
				glbDouble ang = atan2(Arrowlength,Arrowwidth);
				glbDouble ang2 = atan2(Arrowlength,-Arrowwidth);
				glbDouble xm = ra*max(abs(cos(ang+angle)),abs(cos(ang2+angle)));
				glbDouble ym = ra*max(abs(sin(ang+angle)),abs(sin(ang2+angle)));			
				glbDouble sal =	ra*ra/(xm*xm+ym*ym);
				xtexScale *= sal;
				ytexScale *= sal;
			}
			texMat->setMatrix(osg::Matrix::translate(-0.5,-0.5,0.0) * osg::Matrix::rotate(angle,0.0,0.0,1.0) * osg::Matrix::scale(xtexScale,ytexScale,ztexScale) * osg::Matrix::translate(0.5*lineArrowInfo->tilingU->GetValue(mpt_feature.get()),0.5*lineArrowInfo->tilingV->GetValue(mpt_feature.get()),0.0));
		}
		osg::Matrix mat;
		if (mpr_lineGeo)
		{
			//根据箭头起始终止点计算yaw，pitch,(2个顶点)
			osg::Vec3d ptfirst,ptsecond;
			mpr_lineGeo->GetPoint(0,&ptfirst.x(),&ptfirst.y(),&ptfirst.z());	//经纬高
			mpr_lineGeo->GetPoint(1,&ptsecond.x(),&ptsecond.y(),&ptsecond.z());
			if (mpt_globe->GetType() == GLB_GLOBETYPE_GLOBE)
			{
				osg::Matrix locTowor;
				g_ellipsoidModel->computeLocalToWorldTransformFromLatLongHeight(osg::DegreesToRadians(ptfirst.y()),osg::DegreesToRadians(ptfirst.x()),ptfirst.z(),locTowor);
				g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(ptfirst.y()),osg::DegreesToRadians(ptfirst.x()),ptfirst.z(),ptfirst.x(),ptfirst.y(),ptfirst.z());
				g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(ptsecond.y()),osg::DegreesToRadians(ptsecond.x()),ptsecond.z(),ptsecond.x(),ptsecond.y(),ptsecond.z());
				//osg::Vec3d arrowVec = ptsecond-ptfirst;
				ptsecond = ptsecond * osg::Matrix::inverse(locTowor);
				ptsecond.normalize();
				if (ptsecond.z() > 1.0)
					ptsecond.z() = 1.0;
				else if (ptsecond.z() < -1.0)
					ptsecond.z() = -1.0;
				pitch = osg::RadiansToDegrees(asin(ptsecond.z()));
				yaw = osg::RadiansToDegrees(atan2(-ptsecond.x(),ptsecond.y()));
			}
			osg::Vec3d point = ptfirst;
			if(mpt_globe->GetType() == GLB_GLOBETYPE_GLOBE)
				g_ellipsoidModel->computeLocalToWorldTransformFromXYZ(point.x(),point.y(),point.z(),mat);
			else if (mpt_globe->GetType() == GLB_GLOBETYPE_FLAT)
				mat.makeTranslate(point);

			yaw = osg::DegreesToRadians(yaw);
			pitch = osg::DegreesToRadians(pitch);
			roll = osg::DegreesToRadians(roll);
			osg::Vec3d yaw_vec(0.0,0.0,1.0);
			osg::Vec3d pitch_vec(cos(yaw),sin(yaw),0.0);
			osg::Vec3d roll_vec(-sin(yaw)*cos(pitch),cos(pitch)*cos(yaw),sin(pitch));
			osg::Quat quat;
			quat.makeRotate(yaw,yaw_vec,pitch,pitch_vec,roll,roll_vec);
			mat.preMultRotate(quat);
			mat.preMultScale(osg::Vec3d(xScale,yScale,zScale));

			mt->setMatrix(mat);
			if (mpt_altitudeMode == GLB_ALTITUDEMODE_RELATIVETERRAIN)
			{
				DealModePosByChangeAltitudeOrChangePos();
			}
		}		
	}
	return true;
}

void CGlbGlobeLine::DealModePosByChangeAltitudeOrChangePos()
{
	if(mpt_node == NULL)
		return;
	unsigned int numChild = mpt_node->asSwitch()->getNumChildren();
	for (unsigned int i = 0; i < numChild; i++)
	{
		osg::MatrixTransform *mt = 
			static_cast<osg::MatrixTransform *>(mpt_node->asSwitch()->getChild(i));

		if(mt == NULL)	continue;
		if (mpt_feature.get())
		{// 错误！！！
			glbDouble xOrLon,yOrLat,zOrAlt,M;
			zOrAlt = 0.0;
			//获取外包盒
			//mpt_feature->GetExtent()->GetCenter(&xOrLon,&yOrLat,&zOrAlt);
			if(GetFeaturePoint(i,&xOrLon,&yOrLat,&zOrAlt,&M) == false)
				return;

			osg::Matrix m = mt->getMatrix();
			osg::Vec3d position(xOrLon,yOrLat,zOrAlt);
			ComputePosByAltitudeAndGloleType(position);
			m.setTrans(position);	
			mt->setMatrix(m);
		}
		else
		{			
			osg::Matrix m = mt->getMatrix();
			//glbDouble xOrLon,yOrLat,zOrAlt;
			//chenpeng
			//mpr_lineGeo->GetExtent()->GetCenter(&xOrLon,&yOrLat,&zOrAlt);
			//osg::Vec3d position(xOrLon,yOrLat,zOrAlt);
			const glbDouble *points = mpr_lineGeo->GetPoints();
			osg::Vec3d position(points[0],points[1],points[2]);
			ComputePosByAltitudeAndGloleType(position);
			m.setTrans(position);	
			mt->setMatrix(m);
		}
	}	
	mpt_globe->UpdateObject(this);
}

glbBool CGlbGlobeLine::SetGeo( CGlbLine *line ,glbBool &isNeedDirectDraw )
{
	isNeedDirectDraw = false;
	line = RemoveRepeatePoints(line);
	if(mpt_feature.get())
	{// 如果有要素，位置信息来源于要素.
		mpr_lineGeo = line;
		return true;
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
			mpr_lineGeo = line;
			isNeedDirectDraw = true;						
			return true;
		}
		mpt_loadState = true;
	}

	mpr_lineGeo = line;
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
	return true;
}

glbInt32 CGlbGlobeLine::GetOsgNodeSize()
{
	if (mpt_node == NULL) return 0;
	if (mpr_objSize==0)
		return ComputeNodeSize(mpt_node);
	return mpr_objSize;
}

void CGlbGlobeLine::DirtyOnTerrainObject()
{
	if (!mpt_globe) return;
	CGlbExtent* cglbExtent = GetBound(false).get();	
	if (cglbExtent)
		mpt_globe->AddDomDirtyExtent(*cglbExtent);
}

void CGlbGlobeLine::RemoveNodeFromScene(glbBool isClean)
{
	SetEdit(false);
	GlbScopedLock<GlbCriticalSection> lock(mpr_addToScene_critical);
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
			mpt_node    =  NULL;
			mpr_objSize =  0;
		}		
		//if (mpt_feature != NULL && mpt_featureLayer != NULL)
		//{
		//	CGlbGlobeFeatureLayer* fl = dynamic_cast<CGlbGlobeFeatureLayer*>(mpt_featureLayer.get());
		//	if (fl)
		//		fl->NotifyFeatureDelete(mpt_feature->GetOid());
		//}
		mpt_currLevel = -1;					
	}
}


/*
*	DrawToImage中调用 ，只用于贴地形绘制
*/
IGlbGeometry* CGlbGlobeLine::AnalysisLineGeomertry(CGlbFeature* feature, CGlbLine* lineGeo)
{	
	glbref_ptr<CGlbMultiLine> multiline = new CGlbMultiLine();
	//glbDouble xOrLon,yOrLat,zOrAlt,M;
	if (mpt_feature.get())
	{			
		glbInt32 level = 0;
		IGlbGeometry *geo = NULL;
		bool result = false;
		result = mpt_feature->GetGeometry(&geo,level);
		if(!result || geo == NULL) 
			return NULL;
		switch(geo->GetType())
		{		
		case GLB_GEO_LINE:
			{
				CGlbLine* ln = dynamic_cast<CGlbLine*>(geo);
				glbInt32 ptCnt = ln->GetCount();
				//glbArray pvalues;
				const glbDouble* pcoords =  ln->GetPoints();
				CGlbLine* newLn = new CGlbLine();
				for (glbInt32 k = 0; k < ptCnt; k++)
				{
					newLn->AddPoint(pcoords[k*2],pcoords[k*2+1]);
				}						
				multiline->AddLine(newLn);
				//delete [] new_pcoords;
			}	
			break;
		case GLB_GEO_MULTILINE:
			{
				return geo;
				CGlbMultiLine* mulln = dynamic_cast<CGlbMultiLine*>(geo);
				glbInt32 linenum = mulln->GetCount();						
				for (glbInt32 j = 0;j < linenum; j++)
				{
					CGlbLine* ln = const_cast<CGlbLine *>(mulln->GetLine(j));
					glbInt32 ptCnt = ln->GetCount();
					//glbArray pvalues;
					const glbDouble* pcoords =  ln->GetPoints();
					CGlbLine* newLn = new CGlbLine();
					for (glbInt32 k = 0; k < ptCnt; k++)
					{
						newLn->AddPoint(pcoords[k*2],pcoords[k*2+1]);
					}
					multiline->AddLine(newLn);
					//delete [] new_pcoords;
				}						
			}
			break;
		}
	}
	else if (lineGeo)
	{
		glbInt32 ptCnt = lineGeo->GetCount();
		const glbDouble* pcoords =  lineGeo->GetPoints();
		glbref_ptr<CGlbLine> line = new CGlbLine();
		for (glbInt32 i = 0;i < ptCnt;i++)
		{
			line->AddPoint(pcoords[i*3],pcoords[i*3+1]);
		}
		multiline->AddLine(line.get());
		//delete [] new_pcoords;
	}

	if (multiline->GetCount()<=0)			
		return NULL;	

	return multiline.release();
}

/* 线程安全 mpr_outline会有 
数据ReadData线程
Globe::Update线程访问
*/
void CGlbGlobeLine::DrawToImage( glbByte *image,glbInt32 imageW, glbInt32 imageH,CGlbExtent &ext )
{
	/*
	*	调度控制对象隐藏时，不能渲染，直接返回
	*/
	if (!mpr_isCanDrawImage)return;

	glbref_ptr<GlbLineSymbolInfo> lineInfo = dynamic_cast<GlbLineSymbolInfo*>(mpr_renderInfo.get());
	if(lineInfo == NULL) return;

	/*
	// 过滤在image中只占小于1个像素的线
	glbref_ptr<CGlbExtent> geomExt = geom->GetExtent();
	glbDouble sc = geomExt->GetXWidth() * geomExt->GetYHeight() / (ext.GetXWidth() * ext.GetYHeight());
	glbDouble area = sc * imageW * imageH;
	if (area < 1)
	return;	
	*/	

	glbref_ptr<IGlbGeometry>  outline = GetOutLine();
	if (outline==NULL) return;

	//GlbScopedLock<GlbCriticalSection> lock(mpr_outline_critical);

	if (lineInfo->symType == GLB_LINESYMBOL_PIXEL)
	{
		CGlbGlobeLinePixelSymbol linePixelSymbol;
		linePixelSymbol.DrawToImage(this,outline.get(),image,imageW,imageH,ext);
	}else if (lineInfo->symType == GLB_LINESYMBOL_2D)
	{
	}else if (lineInfo->symType == GLB_LINESYMBOL_3D)
	{
	}else if (lineInfo->symType == GLB_LINESYMBOL_ARROW)
	{
		CGlbGlobeLineArrowSymbol lineArrowSymbol;
		lineArrowSymbol.DrawToImage(this,outline.get(),image,imageW,imageH,ext);
	}
}

class LineEditEventHandler : public osgGA::GUIEventHandler
{
public:
	LineEditEventHandler(CGlbGlobeLine *obj):mpr_lineObj(obj)
	{
		mpr_globe = mpr_lineObj->GetGlobe();
		mpr_boundGeode = NULL;
		mpr_boundExtent = NULL;
		mpr_pointsGeode = NULL;
		mpr_isGeoChanged = false;
	}
	bool handle(const osgGA::GUIEventAdapter &ea,osgGA::GUIActionAdapter &aa)
	{
		osgViewer::Viewer *viewer = dynamic_cast<osgViewer::Viewer*>(&aa);
		if(!viewer) return false;
		viewer->getCamera()->setCullingMode(
			viewer->getCamera()->getCullingMode() & ~osg::CullStack::SMALL_FEATURE_CULLING);
		unsigned int buttonMask = ea.getButtonMask();
		static glbBool isIntersect = false;//标识射线与对象的包围盒相交
		static osg::Vec3d intersectPoint(0.0,0.0,0.0);//记录射线与包围盒相交时 相交的第一个点方便求对象需要移动的向量
		static glbBool isIntersectPoint = false;//记录射线已经与编辑点相交，不用再判断射线是否再与线对象相交
		static glbInt32 intersectIndex = -1;

		static osg::ref_ptr<osg::MatrixTransform> moveMt = NULL;//鼠标move与线相交产生的点，记录下来，方便删除
		glbBool isGlobe = mpr_globe->GetType() == GLB_GLOBETYPE_GLOBE;

		//处理move与编辑点相交
		static glbBool isMoveIntersect = false;
		static glbInt32 moveIntersectIndex = -1;
		static glbBool isColorChanged = false;//记录因为鼠标滑动与编辑点相交修改过编辑点其中一点的颜色


		//计算相交阈值
		double pixelWidth = mpr_globe->GetView()->GetCurrentPixelWidth();

		//CallBack原始代码——————begin

		if(mpr_lineObj->IsEdit())//线在编辑状态
		{
			GlbGlobeRObEditModeEnum editMode = mpr_lineObj->GetEditMode();
			if(mpr_boundGeode == NULL)//如果包围盒为空，给线对象创建包围盒
			{
				if(isGlobe)
					mpr_boundExtent = mpr_lineObj->GetBound(false);
				else
					mpr_boundExtent = mpr_lineObj->GetBound(true);
				osg::Vec3d center(0.0,0.0,0.0);
				mpr_boundExtent->GetCenter(&center.x(),&center.y(),&center.z());
				osg::Matrixd localToWorld;
				if(isGlobe)
				{
					if(mpr_lineObj->GetAltitudeMode() == GLB_ALTITUDEMODE_RELATIVETERRAIN)
						center.z() += mpr_lineObj->GetGlobe()->GetElevationAt(center.x(),center.y());
					g_ellipsoidModel->computeLocalToWorldTransformFromLatLongHeight(
						osg::DegreesToRadians(center.y()),osg::DegreesToRadians(center.x()),center.z(),localToWorld);
					mpr_boundGeode = CreateBoundingBox(*mpr_boundExtent);
				}
				else
				{
					localToWorld.setTrans(center.x(),center.y(),center.z());
					mpr_boundGeode = CreateBoundingBox(*mpr_boundExtent,true);
				}
				osg::ref_ptr<osg::MatrixTransform> mt = new osg::MatrixTransform;
				mt->setMatrix(localToWorld);
				mt->addChild(mpr_boundGeode);
				mpr_lineObj->GetEditNode()->addChild(mt);
			}

			glbref_ptr<CGlbExtent> geoExt = NULL;
			if(isGlobe)
				geoExt = mpr_lineObj->GetBound(false);
			else
				geoExt = mpr_lineObj->GetBound(true);
			if(*geoExt != *mpr_boundExtent)//如果包围盒有变化，更新包围盒
			{
				mpr_boundExtent = geoExt;
				osg::ref_ptr<osg::MatrixTransform> mt = dynamic_cast<osg::MatrixTransform *>(mpr_boundGeode->getParent(0));
				if(mt)
				{
					mt->removeChild(mpr_boundGeode);
					mpr_boundGeode = NULL;
				}
				osg::Vec3d center(0.0,0.0,0.0);
				mpr_boundExtent->GetCenter(&center.x(),&center.y(),&center.z());
				osg::Matrixd localToWorld;
				if(isGlobe)
				{
					if(mpr_lineObj->GetAltitudeMode() == GLB_ALTITUDEMODE_RELATIVETERRAIN)
						center.z() += mpr_lineObj->GetGlobe()->GetElevationAt(center.x(),center.y());
					g_ellipsoidModel->computeLocalToWorldTransformFromLatLongHeight(
						osg::DegreesToRadians(center.y()),osg::DegreesToRadians(center.x()),center.z(),localToWorld);
					mpr_boundGeode = CreateBoundingBox(*mpr_boundExtent);
				}
				else
				{
					localToWorld.setTrans(center.x(),center.y(),center.z());
					mpr_boundGeode = CreateBoundingBox(*mpr_boundExtent,true);
				}
				mt->setMatrix(localToWorld);
				mt->addChild(mpr_boundGeode);
			}

			static osg::ref_ptr<osg::MatrixTransform> mt = NULL;
			//线的Geo发生变化更新编辑点节点
			//Geo的变化包括 1、与线相交点击添加点
			//                            2、拖动编辑点编辑线
			if(mpr_isGeoChanged || (isMoveIntersect == false && isColorChanged))
			{
				if(mt)
				{
					mpr_lineObj->GetEditNode()->removeChild(mt);
					mt = NULL;
				}
				mpr_pointsGeode = NULL;
				if(mpr_isGeoChanged)
					mpr_isGeoChanged = false;
				if(isColorChanged)
					isColorChanged = false;
			}

			//补充平面编辑？？？？？？？
			if(mpr_pointsGeode == NULL && mpr_lineObj->GetEditMode() == GLB_ROBJECTEDIT_SHAPE)
			{
				mpr_pointsGeode = new osg::Geode;
				osg::ref_ptr<osg::Geometry> pointsGeom = new osg::Geometry;
				CGlbLine *line3D = mpr_lineObj->GetGeo();
				osg::ref_ptr<osg::Vec3dArray> vertexes = new osg::Vec3dArray;
				osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array;
				const glbDouble *points = line3D->GetPoints();

				osg::Matrixd localToWorld;
				if(isGlobe)
					g_ellipsoidModel->computeLocalToWorldTransformFromLatLongHeight(
					osg::DegreesToRadians(points[1]),osg::DegreesToRadians(points[0]),points[2],localToWorld);
				else
					localToWorld.makeTranslate(points[0],points[1],points[2]);
				osg::Matrixd worldTolocal = osg::Matrixd::inverse(localToWorld);

				for (glbInt32 i = 0; i < line3D->GetCount();i++)
				{
					osg::Vec3d point(points[3 * i],points[3 * i + 1],points[3 * i + 2]);
					if(isGlobe)
						g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(point.y()),
						osg::DegreesToRadians(point.x()),point.z(),point.x(),point.y(),point.z());

					vertexes->push_back(point * worldTolocal);
					colors->push_back(osg::Vec4(1.0,1.0,1.0,1.0));
				}

				pointsGeom->setVertexArray(vertexes);
				pointsGeom->setColorArray(colors);
				pointsGeom->setColorBinding(osg::Geometry::BIND_PER_VERTEX);
				pointsGeom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::POINTS,0,vertexes->size()));
				osg::ref_ptr<osg::StateSet> stateset = mpr_pointsGeode->getOrCreateStateSet();
				osg::ref_ptr<osg::Point> point = new osg::Point;
				point->setSize(7);//需要根据像素修改大小,根据osg案例测试，这个设置的就是屏幕像素大小
				stateset->setAttributeAndModes(point,osg::StateAttribute::ON);
				stateset->setMode(GL_LIGHTING,osg::StateAttribute::OFF);
				//stateset->setMode(GL_DEPTH_TEST,osg::StateAttribute::OFF);

				//为了使编辑球不被遮挡
				stateset->setRenderBinDetails(2,"RenderBin");
				osg::ref_ptr<osg::Depth> depth = new osg::Depth;
				depth->setFunction(osg::Depth::ALWAYS);
				stateset->setAttributeAndModes(depth, osg::StateAttribute::ON);

				mpr_pointsGeode->addDrawable(pointsGeom);
				mt = new osg::MatrixTransform;
				mt->setMatrix(localToWorld);
				mt->addChild(mpr_pointsGeode);
				Group *editNode = mpr_lineObj->GetEditNode();
				if(editNode)
					editNode->addChild(mt);
			}

			if(mpr_pointsGeode != NULL && mpr_lineObj->GetEditMode() != GLB_ROBJECTEDIT_SHAPE)
			{
				Group *editNode = mpr_lineObj->GetEditNode();
				if(editNode->getNumChildren() > 1)
				{
					osg::MatrixTransform *mt = dynamic_cast<osg::MatrixTransform *>(editNode->getChild(1));
					if(mt)
					{
						mt->removeChild(mpr_pointsGeode);
						mpr_pointsGeode = NULL;
					}
				}
			}

			if(isMoveIntersect && mpr_pointsGeode)
			{
				osg::Geometry *pointsGeom = dynamic_cast<osg::Geometry *>(mpr_pointsGeode->getDrawable(0));
				osg::Vec4Array *colors = dynamic_cast<osg::Vec4Array *>(pointsGeom->getColorArray());
				osg::ref_ptr<osg::Vec4Array> newColors = new osg::Vec4Array;
				for(glbInt32 i = 0 ; i < colors->size(); i++)
				{
					if(moveIntersectIndex == i)
						newColors->push_back(osg::Vec4(1.0,1.0,0.0,1.0));
					else
						newColors->push_back(colors->at(i));
				}
				pointsGeom->setColorArray(newColors);
				pointsGeom->setColorBinding(osg::Geometry::BIND_PER_VERTEX);
				pointsGeom->dirtyDisplayList();
				isColorChanged = true;
			}
		}

		//CallBack原始代码——————end

		switch(ea.getEventType())
		{
		case(osgGA::GUIEventAdapter::PUSH):
			{
				CGlbGlobeView *view =  mpr_globe->GetView();
				osg::Vec3d start(ea.getX(),ea.getY(),0.0);
				osg::Vec3d end(ea.getX(),ea.getY(),1.0);
				view->ScreenToWorld(start.x(),start.y(),start.z());
				view->ScreenToWorld(end.x(),end.y(),end.z());
				osg::Node *node = mpr_lineObj->GetOsgNode();
				if(!node)
					return false;

				glbref_ptr<CGlbExtent> boundExtent = mpr_lineObj->GetBound(true);
				glbDouble minX,minY,minZ,maxX,maxY,maxZ;
				boundExtent->GetMin(&minX,&minY,&minZ);
				boundExtent->GetMax(&maxX,&maxY,&maxZ);
				osg::BoundingBox bb(minX,minY,minZ,maxX,maxY,maxZ);
				isIntersect = intersectRayBoundingBox(start,end,bb);//判断射线与对象的包围盒相交
				if(isIntersect == false)
					return false;

				//先确定按下鼠标时是否与编辑点相交，相交则记录相交点位置
				CGlbLine *line3D = mpr_lineObj->GetGeo();
				const glbDouble *points = line3D->GetPoints();
				Vec3d point;
				for (glbInt32 i = 0 ; i < line3D->GetCount() ; i++)
				{
					point = Vec3d(points[3 * i],points[3 * i + 1],points[3 * i + 2]);
					if(isGlobe)
						g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(point.y()),
						osg::DegreesToRadians(point.x()),point.z(),point.x(),point.y(),point.z());
					osg::ref_ptr<osg::LineSegment> seg = new osg::LineSegment(start,end);
					bool intersected = intersect3D_SegmentPoint(seg,point,1.5 * pixelWidth/*0.1*/);
					if(intersected == false)
						continue;

					isIntersectPoint = true;
					intersectIndex = i;
					break;
				}

				//如果没有与编辑点相交，在对象编辑几何形态，点下鼠标与线段相交要添加编辑点
				if(isIntersectPoint == false && mpr_lineObj->GetEditMode() == GLB_ROBJECTEDIT_SHAPE)
				{
					CGlbLine *line3D = mpr_lineObj->GetGeo();
					const glbDouble *points = line3D->GetPoints();
					Vec3d startPoint,endPoint;
					for (glbInt32 i = 0 ; i < line3D->GetCount() - 1; i++)
					{
						startPoint = Vec3d(points[3 * i],points[3 * i + 1],points[3 * i + 2]);
						if(isGlobe)
							g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(startPoint.y()),
							osg::DegreesToRadians(startPoint.x()),startPoint.z(),startPoint.x(),startPoint.y(),startPoint.z());
						endPoint = Vec3d(points[3 * (i + 1)],points[3 * (i + 1) + 1],points[3 * (i + 1) + 2]);
						if(isGlobe)
							g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(endPoint.y()),
							osg::DegreesToRadians(endPoint.x()),endPoint.z(),endPoint.x(),endPoint.y(),endPoint.z());
						osg::ref_ptr<osg::LineSegment> seg1 = new osg::LineSegment(startPoint,endPoint);
						osg::ref_ptr<osg::LineSegment> seg2 = new osg::LineSegment(start,end);
						osg::Vec3d outPoint;
						bool intersected = intersect3D_SegmentSegment(seg1,seg2,pixelWidth / 2/*0.03*/,outPoint);//鼠标与射线相交
						if(intersected == false)
							continue;
						//更新编辑点
						if(mpr_lineObj->GetEditNode()->getNumChildren() < 2)
							return false;

						//射线与线对象相交，给线对象添加几何点：给lineGeo添加点
						//此刻线对象的包围盒是没有没有发生变化的
						//要通过方法告诉EditCallback里对编辑点几何进行更新,添加成员mpr_isGeoChanged
						if(isGlobe)
						{
							g_ellipsoidModel->convertXYZToLatLongHeight(outPoint.x(),
								outPoint.y(),outPoint.z(),outPoint.y(),outPoint.x(),outPoint.z());
							outPoint.x() = osg::RadiansToDegrees(outPoint.x());
							outPoint.y() = osg::RadiansToDegrees(outPoint.y());
						}
						glbref_ptr<CGlbLine> newlineGeo = new CGlbLine(3,false);
						for (glbInt32 j = 0; j< line3D->GetCount();j++)
						{
							if(j == i + 1)
							{
								newlineGeo->AddPoint(outPoint.x(),outPoint.y(),outPoint.z());
								intersectIndex = j;
							}
							newlineGeo->AddPoint(points[3 * j],points[3 * j + 1],points[3 * j + 2]);
						}
						glbBool isNULL = false;
						mpr_lineObj->SetGeo(newlineGeo.get(),isNULL);
						mpr_isGeoChanged = true;
						mpr_lineObj->DirectDraw(0);
						Group *group = mpr_lineObj->GetEditNode();
						if(group)
						{
							group->removeChild(moveMt);
							moveMt = NULL;
						}
						isIntersectPoint = true;
						return true;
					}
				}

				//记录初始相交点，以便求线对象需要移动的矩阵
				osg::Vec3d center;
				boundExtent->GetCenter(&center.x(),&center.y(),&center.z());
				glbDouble radius = (center - osg::Vec3d(0.0,0.0,0.0)).length();

				std::vector<osg::Vec3d> IntersectPos;
				glbInt32 pointNum = IntersectRaySphere(start,end,osg::Vec3d(0.0,0.0,0.0),radius,IntersectPos);
				if(pointNum > 0)
					intersectPoint = IntersectPos.at(0);
				return false;
			}
		case(osgGA::GUIEventAdapter::DRAG):
			{
				if(isIntersect && mpr_lineObj->GetEditMode() == GLB_ROBJECTEDIT_HORIZONTAL)//DRAG下水平平移对象
				{
					CGlbGlobeView *view =  mpr_globe->GetView();
					osg::Vec3d start(ea.getX(),ea.getY(),0.0);
					osg::Vec3d end(ea.getX(),ea.getY(),1.0);
					view->ScreenToWorld(start.x(),start.y(),start.z());
					view->ScreenToWorld(end.x(),end.y(),end.z());

					glbref_ptr<CGlbExtent> extent = mpr_lineObj->GetBound(true);
					osg::Vec3d center;
					extent->GetCenter(&center.x(),&center.y(),&center.z());
					glbDouble radius = (center - osg::Vec3d(0.0,0.0,0.0)).length();

					std::vector<osg::Vec3d> IntersectPos;
					glbInt32 pointNum = 0;
					if(isGlobe)
						pointNum = IntersectRaySphere(start,end,osg::Vec3d(0.0,0.0,0.0),radius,IntersectPos);
					else
					{
						osg::Vec3d IntersectPoint;
						osg::ref_ptr<osg::LineSegment> line = new osg::LineSegment(start,end);
						osg::Vec3d dir(0,0,1);
						osg::Plane *plane = new osg::Plane(dir,center);
						bool Intersected = intersect3D_SegmentPlane(line,plane,center,IntersectPoint);
						if(Intersected)
							IntersectPos.push_back(IntersectPoint);
					}

					if(IntersectPos.size() > 0)
					{
						osg::Vec3d newPoint = IntersectPos.at(0);
						if(isGlobe)
						{
							osg::Vec3d tempPoint;
							g_ellipsoidModel->convertXYZToLatLongHeight(intersectPoint.x(),intersectPoint.y(),
								intersectPoint.z(),tempPoint.y(),tempPoint.x(),tempPoint.z());
							g_ellipsoidModel->convertXYZToLatLongHeight(newPoint.x(),newPoint.y(),
								newPoint.z(),newPoint.y(),newPoint.x(),newPoint.z());

							g_ellipsoidModel->convertLatLongHeightToXYZ(newPoint.y(),newPoint.x(),
								tempPoint.z(),newPoint.x(),newPoint.y(),newPoint.z());
						}

						osg::Matrixd trans;
						//trans.setTrans(osg::Vec3d(newPoint.x() - intersectPoint.x(),newPoint.y() - intersectPoint.y(),0));
						//osg::Vec3d test = newPoint - intersectPoint;
						trans.setTrans(newPoint - intersectPoint);

						CGlbLine *lineGeo = mpr_lineObj->GetGeo();
						const glbDouble *points = lineGeo->GetPoints();
						glbref_ptr<CGlbLine> tempLineGeo = new CGlbLine(3,false);
						for (glbInt32 i = 0 ; i < lineGeo->GetCount(); i++)
						{
							osg::Vec3d point = osg::Vec3d(points[3 * i],points[3 * i + 1],points[3 * i + 2]);
							if(isGlobe)
								g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(point.y()),
								osg::DegreesToRadians(point.x()),point.z(),point.x(),point.y(),point.z());
							point = point * trans;
							if(isGlobe)
							{
								g_ellipsoidModel->convertXYZToLatLongHeight(point.x(),point.y(),
									point.z(),point.y(),point.x(),point.z());
								point.x() = osg::RadiansToDegrees(point.x());
								point.y() = osg::RadiansToDegrees(point.y());
							}
							//tempLineGeo->AddPoint(point.x(),point.y(),point.z());
							tempLineGeo->AddPoint(point.x(),point.y(),points[3 * i + 2]);
						}
						glbBool isDirectDraw = false;
						mpr_lineObj->SetGeo(tempLineGeo.get(),isDirectDraw);
						mpr_lineObj->DirectDraw(0);
						intersectPoint = newPoint;
						return true;
					}
				}
				else if(isIntersect && mpr_lineObj->GetEditMode() == GLB_ROBJECTEDIT_VERTICAL)//DRAG下竖直平移对象
				{
					osg::Vec3d center;
					CGlbLine *lineGeo = mpr_lineObj->GetGeo();
					if(!lineGeo)
						return false;
					CGlbExtent *lineExt = const_cast<CGlbExtent *>(lineGeo->GetExtent());
					if(!lineExt)
						return false;
					lineExt->GetCenter(&center.x(),&center.y(),&center.z());

					osg::Vec3d focusPoint;
					osg::Vec3d cameraPoint;
					mpr_globe->GetView()->GetFocusPos(focusPoint);
					if(isGlobe)
						g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(focusPoint.y()),
						osg::DegreesToRadians(focusPoint.x()),focusPoint.z(),focusPoint.x(),focusPoint.y(),focusPoint.z());
					mpr_globe->GetView()->GetCameraPos(cameraPoint);
					if(isGlobe)
						g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(cameraPoint.y()),
						osg::DegreesToRadians(cameraPoint.x()),cameraPoint.z(),cameraPoint.x(),cameraPoint.y(),cameraPoint.z());
					osg::Vec3d dir = focusPoint - cameraPoint;
					dir.normalize();
					osg::Vec3d objPoint;
					if(isGlobe)
						g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(center.y()),
						osg::DegreesToRadians(center.x()),center.z(),objPoint.x(),objPoint.y(),objPoint.z());
					else
						objPoint = center;

					osg::Plane *plane = new osg::Plane(dir,objPoint);
					osg::Vec3d outPoint;

					CGlbGlobeView *view =  mpr_globe->GetView();
					osg::Vec3d start(ea.getX(),ea.getY(),0.0);
					osg::Vec3d end(ea.getX(),ea.getY(),1.0);
					view->ScreenToWorld(start.x(),start.y(),start.z());
					view->ScreenToWorld(end.x(),end.y(),end.z());

					osg::ref_ptr<osg::LineSegment> line = new osg::LineSegment(start,end);
					bool intersected = intersect3D_SegmentPlane(line,plane,objPoint,outPoint);
					if(intersected == false)
						return false;
					osg::Matrixd trans;
					//trans.setTrans(outPoint - intersectPoint);
					trans.setTrans(osg::Vec3d(0,0,outPoint.z() - intersectPoint.z()));

					const glbDouble *points = lineGeo->GetPoints();
					glbref_ptr<CGlbLine> newlineGeo = new CGlbLine(3,false);
					for (glbInt32 i = 0; i < lineGeo->GetCount(); i++)
					{
						osg::Vec3d point = osg::Vec3d(points[3 * i],points[3 * i +1],points[3 * i + 2]);
						if(isGlobe)
							g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(point.y()),
							osg::DegreesToRadians(point.x()),point.z(),point.x(),point.y(),point.z());
						point = point * trans;
						if(isGlobe)
							g_ellipsoidModel->convertXYZToLatLongHeight(point.x(),point.y(),point.z(),point.y(),point.x(),point.z());
						//point.x() = osg::RadiansToDegrees(point.x());
						//point.y() = osg::RadiansToDegrees(point.y());
						newlineGeo->AddPoint(points[3 * i],points[3 * i +1],point.z());
					}
					glbBool isNULL = false;
					mpr_lineObj->SetGeo(newlineGeo.get(),isNULL);
					mpr_lineObj->DirectDraw(0);
					intersectPoint = outPoint;
					return true;
				}
				else if(isIntersect && isIntersectPoint && mpr_lineObj->GetEditMode() == GLB_ROBJECTEDIT_SHAPE)//DRAG下编辑形态
				{
					if(isIntersectPoint)
					{
						CGlbLine *line3D = mpr_lineObj->GetGeo();
						const glbDouble *points = line3D->GetPoints();
						CGlbGlobeView *view =  mpr_globe->GetView();
						osg::Vec3d start(ea.getX(),ea.getY(),0.0);
						osg::Vec3d end(ea.getX(),ea.getY(),1.0);
						view->ScreenToWorld(start.x(),start.y(),start.z());
						view->ScreenToWorld(end.x(),end.y(),end.z());

						glbref_ptr<CGlbExtent> extent = mpr_lineObj->GetBound(true);
						osg::Vec3d center;
						extent->GetCenter(&center.x(),&center.y(),&center.z());
						glbDouble radius = (center - osg::Vec3d(0.0,0.0,0.0)).length();

						std::vector<osg::Vec3d> IntersectPos;
						glbInt32 pointNum = 0;

						if(isGlobe)
							pointNum = IntersectRaySphere(start,end,osg::Vec3d(0.0,0.0,0.0),radius,IntersectPos);
						else
						{
							osg::Vec3d IntersectPoint;
							osg::ref_ptr<osg::LineSegment> line = new osg::LineSegment(start,end);
							osg::Vec3d dir(0,0,1);
							osg::Plane *plane = new osg::Plane(dir,center);
							bool Intersected = intersect3D_SegmentPlane(line,plane,center,IntersectPoint);
							if(Intersected)
								IntersectPos.push_back(IntersectPoint);
						}
						
						if(IntersectPos.size() < 1)
							return false;

						//相交编辑点，编辑线对象的Geo
						osg::Vec3d tempIntersect = IntersectPos.at(0);
						if(isGlobe)
						{
							g_ellipsoidModel->convertXYZToLatLongHeight(tempIntersect.x(),tempIntersect.y(),
								tempIntersect.z(),tempIntersect.y(),tempIntersect.x(),tempIntersect.z());
							tempIntersect.x() = osg::RadiansToDegrees(tempIntersect.x());
							tempIntersect.y() = osg::RadiansToDegrees(tempIntersect.y());
						}
						glbref_ptr<CGlbLine> lineGeo = new CGlbLine(3,false);
						for(glbInt32 j = 0; j < line3D->GetCount(); j++)
						{
							if(j == intersectIndex)
								lineGeo->AddPoint(tempIntersect.x(),tempIntersect.y(),tempIntersect.z());
							else
								lineGeo->AddPoint(points[3 * j],points[3 * j + 1],points[3 * j + 2]);
						}
						glbBool isNull = false;
						mpr_lineObj->SetGeo(lineGeo.get(),isNull);
						mpr_isGeoChanged = true;
						mpr_lineObj->DirectDraw(0);
						Group *group = mpr_lineObj->GetEditNode();
						if(group)
						{
							group->removeChild(moveMt);
							moveMt = NULL;
						}
						return true;
					}
				}
			}
		case(osgGA::GUIEventAdapter::MOVE):
			{
				//没有与编辑点相交，所以当与线相交时要添加编辑球，不相交的时候移除编辑球
				if(isIntersectPoint == false && mpr_lineObj->GetEditMode() == GLB_ROBJECTEDIT_SHAPE)
				{
					CGlbGlobeView *view =  mpr_globe->GetView();
					osg::Vec3d start(ea.getX(),ea.getY(),0.0);
					osg::Vec3d end(ea.getX(),ea.getY(),1.0);
					view->ScreenToWorld(start.x(),start.y(),start.z());
					view->ScreenToWorld(end.x(),end.y(),end.z());
					osg::Node *node = mpr_lineObj->GetOsgNode();
					if(!node)
						return false;

					glbref_ptr<CGlbExtent> boundExtent = mpr_lineObj->GetBound(true);
					glbDouble minX,minY,minZ,maxX,maxY,maxZ;
					boundExtent->GetMin(&minX,&minY,&minZ);
					boundExtent->GetMax(&maxX,&maxY,&maxZ);
					osg::BoundingBox bb(minX,minY,minZ,maxX,maxY,maxZ);
					glbBool tempIsIntersect = intersectRayBoundingBox(start,end,bb);
					if(tempIsIntersect == false)
						return false;

					CGlbLine *line3D = mpr_lineObj->GetGeo();
					const glbDouble *points = line3D->GetPoints();

					//处理move与编辑点相交时，更改相交编辑点颜色
					isMoveIntersect = false;
					moveIntersectIndex = -1;
					Vec3d point;
					for (glbInt32 i = 0 ; i < line3D->GetCount() ; i++)
					{
						point = Vec3d(points[3 * i],points[3 * i + 1],points[3 * i + 2]);
						if(isGlobe)
							g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(point.y()),
							osg::DegreesToRadians(point.x()),point.z(),point.x(),point.y(),point.z());
						osg::ref_ptr<osg::LineSegment> seg = new osg::LineSegment(start,end);
						bool intersected = intersect3D_SegmentPoint(seg,point,1.5 * pixelWidth/*0.1*/);
						if(intersected == false)
							continue;
						//处理“i”位置编辑点的颜色
						isMoveIntersect = true;
						moveIntersectIndex = i;
						break;
					}

					//处理删除
					if(moveMt)
					{
						Group *editNode = mpr_lineObj->GetEditNode();
						if(editNode)
						{
							glbBool isMove = editNode->removeChild(moveMt);
							moveMt = NULL;
						}
					}

					if(isMoveIntersect == false)//滑动与编辑点相交，不处理与线相交
					{
						//处理与编辑线相交
						Vec3d startPoint,endPoint;
						for (glbInt32 i = 0 ; i < line3D->GetCount() - 1; i++)
						{
							startPoint = Vec3d(points[3 * i],points[3 * i + 1],points[3 * i + 2]);
							if(isGlobe)
								g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(startPoint.y()),
								osg::DegreesToRadians(startPoint.x()),startPoint.z(),startPoint.x(),startPoint.y(),startPoint.z());

							endPoint = Vec3d(points[3 * (i + 1)],points[3 * (i + 1) + 1],points[3 * (i + 1) + 2]);
							if(isGlobe)
								g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(endPoint.y()),
								osg::DegreesToRadians(endPoint.x()),endPoint.z(),endPoint.x(),endPoint.y(),endPoint.z());
							osg::ref_ptr<osg::LineSegment> seg1 = new osg::LineSegment(startPoint,endPoint);
							osg::ref_ptr<osg::LineSegment> seg2 = new osg::LineSegment(start,end);
							osg::Vec3d outPoint;
							bool intersected = intersect3D_SegmentSegment(seg1,seg2,pixelWidth / 2/*0.03*/,outPoint);
							if(intersected == false)
								continue;

							osg::ref_ptr<Geode> pointGeode = new osg::Geode;
							osg::ref_ptr<osg::Geometry> pointGeom = new osg::Geometry;
							CGlbLine *line3D = mpr_lineObj->GetGeo();
							osg::ref_ptr<osg::Vec3dArray> vertexes = new osg::Vec3dArray;
							osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array;
							const glbDouble *points = line3D->GetPoints();

							osg::Matrixd localToWorld;
							if(isGlobe)
								g_ellipsoidModel->computeLocalToWorldTransformFromLatLongHeight(
								osg::DegreesToRadians(points[1]),osg::DegreesToRadians(points[0]),points[2],localToWorld);
							else
								localToWorld.makeTranslate(points[0],points[1],points[2]);
							osg::Matrixd worldTolocal = osg::Matrixd::inverse(localToWorld);

							vertexes->push_back(outPoint * worldTolocal);
							colors->push_back(osg::Vec4(1.0,1.0,1.0,1.0));

							pointGeom->setVertexArray(vertexes);
							pointGeom->setColorArray(colors);
							pointGeom->setColorBinding(osg::Geometry::BIND_PER_VERTEX);
							pointGeom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::POINTS,0,vertexes->size()));
							osg::ref_ptr<osg::StateSet> stateset = pointGeode->getOrCreateStateSet();
							osg::ref_ptr<osg::Point> point = new osg::Point;
							point->setSize(7);//需要根据像素修改大小,根据osg案例测试，这个设置的就是屏幕像素大小
							stateset->setAttributeAndModes(point,osg::StateAttribute::ON);
							stateset->setMode(GL_LIGHTING,osg::StateAttribute::OFF);
							//stateset->setMode(GL_DEPTH_TEST,osg::StateAttribute::ON);
							stateset->setRenderBinDetails(2,"RenderBin");
							osg::ref_ptr<osg::Depth> depth = new osg::Depth;
							depth->setFunction(osg::Depth::ALWAYS);
							stateset->setAttributeAndModes(depth,osg::StateAttribute::ON);
							pointGeode->addDrawable(pointGeom);
							moveMt = new osg::MatrixTransform;
							moveMt->setMatrix(localToWorld);
							moveMt->addChild(pointGeode);
							Group *editNode = mpr_lineObj->GetEditNode();
							if(editNode)
								editNode->addChild(moveMt);
							//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
							return true;
						}
					}
				}
				return false;
			}
		case(osgGA::GUIEventAdapter::RELEASE):
			{
				isIntersectPoint = false;
				intersectIndex = -1;
				return false;
			}
		default:
			return false;//return false继续调用其他事件，return true不会再调用其他事件
		}
		return false;
	}
private:
	glbref_ptr<CGlbGlobe> mpr_globe;
	glbref_ptr<CGlbGlobeLine> mpr_lineObj;
	osg::ref_ptr<osg::Geode> mpr_boundGeode;
	glbref_ptr<CGlbExtent> mpr_boundExtent;
	osg::ref_ptr<osg::Geode> mpr_pointsGeode;
	glbBool mpr_isGeoChanged;
};

glbBool CGlbGlobeLine::SetEdit( glbBool isEdit )
{
	if (mpt_isEdit == isEdit) return true;
	mpt_isEdit = isEdit;	
	//DealEdit();
	if(mpt_isEdit == true && mpr_editNode == NULL)
	{
		mpr_editNode = new osg::Group;
		osg::ref_ptr<LineEditEventHandler> editEventHandler = new LineEditEventHandler(this);
		mpr_editNode->addEventCallback(editEventHandler);
		glbref_ptr<CGlbGlobeCallBack::CGlbGlobeCallBackTask> task = 
			new CGlbGlobeCallBack::CGlbGlobeCallBackTask(mpt_parent,mpr_editNode);
		task->mpr_size = ComputeNodeSize(mpr_editNode);
		mpt_globe->mpr_p_callback->AddHangTask(task.get());
	}
	else if(mpt_isEdit == false && mpr_editNode != NULL)
	{
		glbref_ptr<CGlbGlobeCallBack::CGlbGlobeCallBackTask> task = 
			new CGlbGlobeCallBack::CGlbGlobeCallBackTask(mpt_parent,mpr_editNode);
		task->mpr_size = ComputeNodeSize(mpr_editNode);
		mpt_globe->mpr_p_callback->AddRemoveTask(task.get());
		mpr_editNode = NULL;
	}
	return true;
}

void CGlbGlobeLine::DealEdit()
{
	//if (mpr_editNode)
	//{
	//	if (mpt_isEdit)
	//	{
	//		osg::ref_ptr<LineObjectEventHandler> poEventHandle = new LineObjectEventHandler(mpt_globe);
	//		mpr_editNode->setEventCallback(poEventHandle);
	//	}
	//	else
	//		mpr_editNode->removeEventCallback(mpr_editNode->getEventCallback());
	//}	
}

glbBool CGlbGlobeLine::GetFeaturePoint( glbInt32 idx,glbDouble *ptx,glbDouble *pty,glbDouble *ptz,glbDouble *ptm)
{
	IGlbGeometry *geo = NULL;
	bool result = false;
	result = mpt_feature->GetGeometry(&geo,mpt_currLevel);
	if(!result || geo == NULL)  return false;

	GlbGeometryTypeEnum geoType = geo->GetType();
	switch(geoType)
	{
	case GLB_GEO_LINE:				///<二维点
		{						
			CGlbLine* ln = dynamic_cast<CGlbLine *>(geo);
			ln->GetPoint(0,ptx,pty);							
		}
		break;
	case GLB_GEO_MULTILINE:		///<二维多点
		{
			CGlbMultiLine* multiln = dynamic_cast<CGlbMultiLine *>(geo);
			CGlbLine* ln = const_cast<CGlbLine *>(multiln->GetLine(idx));
			ln->GetPoint(0,ptx,pty);					
		}
		break;		
	}
	return true;
}

CGlbLine * GlbGlobe::CGlbGlobeLine::RemoveRepeatePoints( CGlbLine *line )
{
	if(line == NULL)
		return NULL;

	glbInt32 dimension = line->GetCoordDimension();
	glbDouble height = 0.0;
	osg::ref_ptr<osg::Vec3dArray> points = new osg::Vec3dArray;
	const glbDouble *pCoords = line->GetPoints();
	glbInt32 pntCnt = line->GetCount();
	for(glbInt32 i = 0; i < pntCnt;++i)
	{
		if(dimension == 3)
			height = pCoords[i * dimension + 2];
		osg::Vec3d pnt(pCoords[i * dimension],pCoords[i * dimension + 1],height);
		points->push_back(pnt);
	}

	glbInt32 pointCnt = points->size();
	osg::Vec3dArray::iterator p = points->begin();
	osg::Vec3d v = *p;
	CGlbLine *newLine = new CGlbLine(3);
	newLine->AddPoint(v.x(),v.y(),v.z());
	for(; p != points->end();p++)
	{
		if(v[0] == (*p)[0] && v[1] == (*p)[1] && v[2] == (*p)[2])
			continue;

		v = *p;
		//tempPoints->push_back(v);
		newLine->AddPoint(v.x(),v.y(),v.z());
	}

	osg::Vec3d beginPoint = points->at(0);
	osg::Vec3d endPoint = points->at(points->size() - 1);
	if(newLine->GetCount() > 1 && isDoubleEqual(beginPoint.x(),endPoint.x()) && isDoubleEqual(beginPoint.y(),endPoint.y()) && isDoubleEqual(beginPoint.z(),endPoint.z()))
		newLine->DeletePoint(newLine->GetCount() - 1);//删除最后一个点，因为与第一个点相等
	return newLine;
}

//现在直接用OperatorVisitor，此函数现在解析endAlpha的值
void GlbGlobe::CGlbGlobeLine::ParseObjectFadeColor()
{
	if(!mpr_renderInfo.valid()) return;

	GlbLineSymbolInfo *lineInfo = static_cast<GlbLineSymbolInfo*>(mpr_renderInfo.get());
	if(lineInfo == NULL) return;
	switch(lineInfo->symType)
	{
	case GLB_LINESYMBOL_PIXEL:
		{
			GlbLinePixelSymbolInfo *linePixelInfo = static_cast<GlbLinePixelSymbolInfo*>(lineInfo);
			if (linePixelInfo != NULL)
			{
				mpt_fadeEndAlpha = linePixelInfo->lineOpacity->GetValue(mpt_feature.get());
				//mpt_fadeColor = GetColor(linePixelInfo->lineColor->GetValue(mpt_feature.get()));
			}
		}
		break;
	case GLB_LINESYMBOL_2D:
		{
			GlbLine2DSymbolInfo *line2DInfo = static_cast<GlbLine2DSymbolInfo*>(lineInfo);
			if (line2DInfo != NULL)
			{// 未写完，待实现 ！！！！！
			}
		}
		break;
	case GLB_LINESYMBOL_3D:
		{
			GlbLine3DSymbolInfo *line3DInfo = static_cast<GlbLine3DSymbolInfo*>(lineInfo);
			if (line3DInfo != NULL)
			{// 未写完，待实现 ！！！！！
			}
		}
		break;
	case GLB_LINESYMBOL_ARROW:
		{
			GlbLineArrowSymbolInfo *lineArrowInfo = static_cast<GlbLineArrowSymbolInfo*>(lineInfo);
			if (lineArrowInfo != NULL) 
			{
				//mpt_fadeColor = GetColor(lineArrowInfo->fillColor->GetValue(mpt_feature.get()));
				mpt_fadeEndAlpha = lineArrowInfo->fillOpacity->GetValue(mpt_feature.get());
			}
		}
		break;
	default:
		break;
	}
}

glbBool GlbGlobe::CGlbGlobeLine::DealDynamicArrowSymbol( GlbLineDynamicArrowSymbolInfo *dynamicArrowInfo,glbBool &isNeedDirectDraw )
{
	if(mpt_altitudeMode == GLB_ALTITUDEMODE_ONTERRAIN)
	{
		isNeedDirectDraw = true;
		return true;
	}
	GlbLineDynamicArrowSymbolInfo *tempInfo = static_cast<GlbLineDynamicArrowSymbolInfo*>(mpr_renderInfo.get());
	if(tempInfo == NULL) return false;
	for(glbInt32 i = 0; i < mpt_node->asSwitch()->getNumChildren(); i++)
	{
		osg::MatrixTransform *mt = dynamic_cast<osg::MatrixTransform *>(mpt_node->asSwitch()->getChild(i));
		if(mt == NULL) 
			return false;
		osg::Geode *geode = dynamic_cast<osg::Geode *>(mt->getChild(0)->asGroup()->getChild(0));
		if(geode == NULL) 
			return false;
		osg::StateSet *linestateset = geode->getOrCreateStateSet();
		osg::StateSet *arrowStateset = mt->getChild(0)->asGroup()->getChild(1)->getOrCreateStateSet();
		//材质
		if(!CGlbCompareValue::CompareValueColorEqual(tempInfo->color,dynamicArrowInfo->color,mpt_feature.get()))
		{
			osg::ref_ptr<osg::Material> linematerial = dynamic_cast<osg::Material*>(linestateset->getAttribute(osg::StateAttribute::MATERIAL));
			if (!linematerial.valid())
				linematerial = new osg::Material;
			linestateset->setAttribute(linematerial.get(),osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);	
			linematerial->setDiffuse(osg::Material::FRONT_AND_BACK,GetColor(dynamicArrowInfo->color->GetValue(mpt_feature.get())));

			if(mt->getChild(0)->asGroup()->getChild(1)->getName() != "model")//不是模型说明是箭头，需要修改颜色
			{
				osg::ref_ptr<osg::Material> arrowmaterial = dynamic_cast<osg::Material*>(arrowStateset->getAttribute(osg::StateAttribute::MATERIAL));
				if (!arrowmaterial.valid())
					arrowmaterial = new osg::Material;
				arrowStateset->setAttribute(arrowmaterial.get(),osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);	
				arrowmaterial->setDiffuse(osg::Material::FRONT_AND_BACK,GetColor(dynamicArrowInfo->color->GetValue(mpt_feature.get())));
			}
			return true;
		}

		if(!CGlbCompareValue::CompareValueBoolEqual(tempInfo->isShowLine,dynamicArrowInfo->isShowLine,mpt_feature.get()))
		{
			geode->setNodeMask(dynamicArrowInfo->isShowLine->GetValue(mpt_feature.get()));
			return true;
		}
	}
	isNeedDirectDraw = true;
	return true;
}

glbBool GlbGlobe::CGlbGlobeLine::Load2( xmlNodePtr *node,const glbWChar* prjPath )
{
	xmlChar* szKey = NULL;
	xmlNodePtr pnode = *node;
	CGlbGlobeRObject::Load2(node,prjPath);
	xmlNodePtr childNode = NULL;
	xmlNodePtr groupNode = pnode->parent;
	pnode = pnode->next;//symType;

	szKey = pnode->xmlChildrenNode->content;
	char* str = u2g_obj((char*)szKey);
	CGlbString symType = (char*)str;

	CGlbString bUse;
	CGlbWString field;
	CGlbString value;

	if(symType == "NGLB_LINESYMBOL_PIXEL")
	{
		GlbLinePixelSymbolInfo* pixelinfo = new GlbLinePixelSymbolInfo();
		pnode = pnode->next;//text;
		pixelinfo->Load2(pnode,prjPath);
		mpr_renderInfo = (GlbRenderInfo*)pixelinfo;
	}
	else if(symType == "NGLB_LINESYMBOL_2D")
	{
		GlbLine2DSymbolInfo* line2DInfo = new GlbLine2DSymbolInfo();
		pnode = pnode->next;//text;
		line2DInfo->Load2(pnode,prjPath);
		mpr_renderInfo = (GlbRenderInfo*)line2DInfo;
	}
	else if(symType == "NGLB_LINESYMBOL_3D")
	{
		GlbLine3DSymbolInfo* line3DInfo = new GlbLine3DSymbolInfo();
		pnode = pnode->next;//text;
		line3DInfo->Load2(pnode,prjPath);
		mpr_renderInfo = (GlbRenderInfo*)line3DInfo;
	}
	else if(symType == "NGLB_LINESYMBOL_ARROW")
	{
		GlbLineArrowSymbolInfo* arrowInfo = new GlbLineArrowSymbolInfo();
		pnode = pnode->next;//text;
		arrowInfo->Load2(pnode,prjPath);
		mpr_renderInfo = (GlbRenderInfo*)arrowInfo;
	}
	else if(symType == "NGLB_LINESYMBOL_MODEL")
	{
	}
	else if(symType == "NGLB_LINESYMBOL_DYNAMICARROW")
	{
		GlbLineDynamicArrowSymbolInfo* arrowInfo = new GlbLineDynamicArrowSymbolInfo();
		pnode = pnode->next;//text;
		arrowInfo->Load2(pnode,prjPath);
		mpr_renderInfo = (GlbRenderInfo*)arrowInfo;
	}
	ParseObjectFadeColor();

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
	groupNode = groupNode->next;//GetGeo
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
	childNode = childNode->next;//points
	childNode = childNode->xmlChildrenNode;//text
	childNode = childNode->next;//tagPoint3
	mpr_lineGeo = new CGlbLine(3,false);
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
		mpr_lineGeo->AddPoint(tempX,tempY,tempZ);
		childNode = childNode->parent;
		childNode = childNode->next;//text
		childNode = childNode->next;
		if(!childNode)
			break;
	}
	return true;
}
