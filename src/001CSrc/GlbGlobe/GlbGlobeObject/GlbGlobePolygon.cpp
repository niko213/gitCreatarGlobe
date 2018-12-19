#include "StdAfx.h"
#include "GlbGlobePolygon.h"

#include "CGlbGlobe.h"
#include "GlbGlobeFeatureLayer.h"
#include "GlbCalculateNodeSizeVisitor.h"
#include "GlbCalculateBoundBoxVisitor.h"

#include "GlbGlobeSymbolCommon.h"
#include "GlbGlobePolygonSymbol.h"
#include "GlbGlobePolyhedronSymbol.h"
#include "GlbGlobePolygonBuildingSymbol.h"
#include "GlbGlobePolygonWaterSymbol.h"
#include "GlbGlobePolygonLakeSymbol.h"

#include "GlbPolygon.h"
#include "IGlbGeometry.h"
#include "GlbExtent.h"
#include "GlbString.h"
#include "GlbGlobeMemCtrl.h"

#include "osg/Material"
#include "osg/LineWidth"
#include "osg/LineStipple"
#include "osg/Depth"
#include "osg/Point"

#include "osgGA/GUIEventHandler"
#include "GlbGlobeMath.h"
#include "GlbGlobeView.h"
#include "GlbConvert.h"
#include "GlbFadeInOutCallback.h"
#include "GlbCompareValue.h"

using namespace GlbGlobe;

//CGlbExtent * TransformExtentFromFlatToWorld( CGlbExtent *platExt )
//{
//	glbDouble minX,maxX,minY,maxY,minZ,maxZ;
//	platExt->Get(&minX,&maxX,&minY,&maxY,&minZ,&maxZ);
//	glbDouble minWorldX,minWorldY,minWorldZ,maxWorldX,maxWorldY,maxWorldZ;
//	g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(minX),
//		osg::DegreesToRadians(minY),minZ,minWorldX,minWorldY,minWorldZ);
//	g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(maxX),
//		osg::DegreesToRadians(maxY),maxZ,maxWorldX,maxWorldY,maxWorldZ);
//	CGlbExtent *worldExt=new CGlbExtent(minWorldX,maxWorldX,minWorldY,maxWorldY,minWorldZ,maxWorldZ);
//	return worldExt;
//}
//////////////////////////////////////////////////////////////////////////
class PolygonObjectBlinkCallback : public osg::NodeCallback
{
public:
	PolygonObjectBlinkCallback()
	{
		mpr_isChangeFrame = true;
		mpr_markNum = 0;
	}

	~PolygonObjectBlinkCallback()
	{
	}

	void operator()(osg::Node *node,osg::NodeVisitor *nv)
	{
		unsigned int frameNum = 60;
		glbInt32 count = nv->getFrameStamp()->getFrameNumber() - mpr_markNum;

		if (count > frameNum - 1)
		{
			mpr_markNum = nv->getFrameStamp()->getFrameNumber();
			mpr_isChangeFrame = !mpr_isChangeFrame;
			count = 0;
		}

		if(!mpr_isChangeFrame)
			count = frameNum - count;

		float a = count * 1 / (float)frameNum;
		if(a < 0.2) a = 0.2;

		DealPolygon2DBlink(node,a);
		traverse(node,nv);
	}
private:
	void DealPolygon2DBlink(osg::Node *node,float a)
	{
		osg::Group *pGroupTop=node->asSwitch()->getChild(0)->asGroup()->getChild(0)->asGroup();
		osg::Transform *pTransform=pGroupTop->getChild(0)->asTransform();
		osg::Switch *pSW=pTransform->getChild(0)->asSwitch();
		osg::Geode *pGeode=pSW->getChild(0)->asGeode();
		osg::Drawable *pDrawable=pGeode->getDrawable(0);
		osg::StateSet *sset = pDrawable->getOrCreateStateSet();
		if(pTransform->getNumChildren() < 2)
			return;
		osg::Switch *pSW_outline=pTransform->getChild(1)->asSwitch();
		osg::Geode *pGeode_outline=pSW_outline->getChild(0)->asGeode();
		osg::Drawable *pDrawable_outline=pGeode_outline->getDrawable(0);
		osg::StateSet *sset_outline = pDrawable_outline->getOrCreateStateSet();

		sset->setMode(GL_BLEND,osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
		sset->setRenderingHint(osg::StateSet::TRANSPARENT_BIN | osg::StateAttribute::OVERRIDE);
		sset_outline->setMode(GL_BLEND,osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
		sset_outline->setRenderingHint(osg::StateSet::TRANSPARENT_BIN | osg::StateAttribute::OVERRIDE);

		sset->getUniform("PolygonOpacity")->set(int(a * 100));
		sset_outline->getUniform("PolygonOutlineOpacity")->set(int(a * 100));
	}
private:
	glbBool mpr_isChangeFrame;
	unsigned int mpr_markNum;
};
//////////////////////////////////////////////////////////////////////////
CGlbGlobePolygon::CGlbGlobePolygonLoadDataTask::CGlbGlobePolygonLoadDataTask( CGlbGlobePolygon *obj,glbInt32 level )
{
	mpr_obj = obj;
	mpr_level = level;
}

CGlbGlobePolygon::CGlbGlobePolygonLoadDataTask::~CGlbGlobePolygonLoadDataTask( )
{
	mpr_obj = NULL;
}

glbBool CGlbGlobePolygon::CGlbGlobePolygonLoadDataTask::doRequest()
{
	if(mpr_obj)
	{
		mpr_obj->ReadData(mpr_level);
		return true;
	}
	return false;
}

CGlbGlobeObject * CGlbGlobePolygon::CGlbGlobePolygonLoadDataTask::getObject(){return mpr_obj.get();}
//////////////////////////////////////////////////////////////////////////
CGlbGlobePolygon::CGlbGlobePolygon(void)
{
	mpr_readData_critical.SetName(L"polygon_readdata");
	mpr_addToScene_critical.SetName(L"polygon_addscene");
	mpr_outline_critical.SetName(L"polygon_outline");
	mpr_distance = 0.0;
	mpr_outline = NULL;
	mpr_renderInfo = NULL;
	mpr_altitude = 0.0;

	mpr_elevation=DBL_MAX;
	mpt_maxVisibleDistance = 1500000.0;
	mpr_polygonGeo=NULL;
	mpr_objSize=0;
	mpr_TexRotation=0.0f;
	mpr_needReReadData=false;
	mpr_isCanDrawImage = true;
}

CGlbGlobePolygon::~CGlbGlobePolygon(void)
{	

}

GlbGlobe::GlbGlobeObjectTypeEnum CGlbGlobePolygon::GetType()
{
	return GLB_OBJECTTYPE_POLYGON;
}
//////////////////////////////////////////////////////////////////////////
glbBool CGlbGlobePolygon::Load( xmlNodePtr *node,const glbWChar* prjPath )
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

	mpr_polygonGeo = new CGlbPolygon(3,false);
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
	mpr_polygonGeo->SetExtRing(outRing.get());

	glbInt32 inRingCount;
	if(pnode && (!xmlStrcmp(pnode->name,(const xmlChar *)"InRingCount")))
	{
		szKey = xmlNodeGetContent(pnode);
		sscanf((char*)szKey,"%d",&inRingCount);	
		xmlFree(szKey);

		pnode = pnode->next;
	}

	index = "InRingIndex";
	for (glbInt32 i = 0; i < inRingCount; i++)
	{
		index = "InRingIndex";
		glbref_ptr<CGlbLine> inRing = new CGlbLine;
		index += CGlbConvert::Int32ToStr(i);
		if(pnode && (!xmlStrcmp(pnode->name,(const xmlChar *)index.c_str())))
		{
			szKey = xmlNodeGetContent(pnode);
			sscanf((char*)szKey,"%d",&pointsCount);	
			xmlFree(szKey);
			pnode = pnode->next;
		}

		index = "InRingPointsIndex";
		for(glbInt32 j = 0; j < pointsCount * 3; j++)
		{
			index += CGlbConvert::Int32ToStr(i);
			index += CGlbConvert::Int32ToStr(j);
			if(pnode && (!xmlStrcmp(pnode->name,(const xmlChar *)index.c_str())))
			{
				szKey = xmlNodeGetContent(pnode);
				sscanf((char *)szKey,"%lf",&tempData0);
				xmlFree(szKey);

				pnode = pnode->next;
				index = "InRingPointsIndex";
			}

			j++;
			index += CGlbConvert::Int32ToStr(i);
			index += CGlbConvert::Int32ToStr(j);
			if(pnode && (!xmlStrcmp(pnode->name,(const xmlChar *)index.c_str())))
			{
				szKey = xmlNodeGetContent(pnode);
				sscanf((char *)szKey,"%lf",&tempData1);
				xmlFree(szKey);

				pnode = pnode->next;
				index = "InRingPointsIndex";
			}

			j++;
			index += CGlbConvert::Int32ToStr(i);
			index += CGlbConvert::Int32ToStr(j);
			if(pnode && (!xmlStrcmp(pnode->name,(const xmlChar *)index.c_str())))
			{
				szKey = xmlNodeGetContent(pnode);
				sscanf((char *)szKey,"%lf",&tempData2);
				xmlFree(szKey);

				pnode = pnode->next;
				index = "InRingPointsIndex";
			}

			inRing->AddPoint(tempData0,tempData1,tempData2);
		}
		mpr_polygonGeo->AddInRing(inRing.get());
	}

	if(pnode && (!xmlStrcmp(pnode->name,(const xmlChar *)"RenderInfo")))
	{
		GlbGlobeObjectTypeEnum rdtype;
		GlbGlobePolygonSymbolTypeEnum symtype;
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
			case  GLB_POLYGONSYMBOL_2D:
				{
					GlbPolygon2DSymbolInfo* polygon2DInfo = new GlbPolygon2DSymbolInfo();
					polygon2DInfo->Load(rdchild,prjPath);
					mpr_renderInfo = (GlbRenderInfo*)polygon2DInfo;
				}
				break;
			case GLB_POLYGONSYMBOL_POLYHEDRON:
				{
					GlbPolyhedronSymbolInfo* polyhedronInfo = new GlbPolyhedronSymbolInfo();
					polyhedronInfo->Load(rdchild,prjPath);
					mpr_renderInfo = (GlbRenderInfo*)polyhedronInfo;
				}
				break;
			case GLB_POLYGONSYMBOL_BUILDING:
				{
					GlbPolygonBuildingSymbolInfo* polygonBuildingInfo = new GlbPolygonBuildingSymbolInfo();
					polygonBuildingInfo->Load(rdchild,prjPath);
					mpr_renderInfo = (GlbRenderInfo*)polygonBuildingInfo;
				}
				break;
			case GLB_POLYGONSYMBOL_WATER:
				{
					GlbPolygonWaterSymbolInfo* waterInfo = new GlbPolygonWaterSymbolInfo();
					waterInfo->Load(rdchild,prjPath);
					mpr_renderInfo = (GlbRenderInfo*)waterInfo;
				}
				break;
			case GLB_POLYGONSYMBOL_LAKE:
				{
					GlbPolygonLakeSymbolInfo* lakeInfo = new GlbPolygonLakeSymbolInfo();
					lakeInfo->Load(rdchild,prjPath);
					mpr_renderInfo = (GlbRenderInfo*)lakeInfo;
				}
				break;
			}
		}
	}
	return true;
}

glbBool CGlbGlobePolygon::Save( xmlNodePtr node,const glbWChar* prjPath )
{
	CGlbGlobeRObject::Save(node,prjPath);

	char str[64];
	CGlbLine *ring = const_cast<CGlbLine *>(mpr_polygonGeo->GetExtRing());
	glbInt32 pointsCount = ring->GetCount();
	sprintf(str,"%d",pointsCount);
	xmlNewTextChild(node,NULL,BAD_CAST "OutRingPointsCount",BAD_CAST str);

	const glbDouble *points = ring->GetPoints();
	std::string index = "outRingPointsIndex";
	for (glbInt32 i = 0; i < pointsCount * 3; i++)
	{
		std::string index = "outRingPointsIndex";
		index += CGlbConvert::Int32ToStr(i);
		sprintf_s(str,"%8lf",points[i]);
		xmlNewTextChild(node,NULL,(unsigned char *)index.c_str(),BAD_CAST str);
	}

	glbInt32 inRingCount = mpr_polygonGeo->GetInRingCount();
	sprintf(str,"%d",inRingCount);
	xmlNewTextChild(node,NULL,BAD_CAST "InRingCount",BAD_CAST str);
	for(glbInt32 i = 0; i < inRingCount; i++)
	{
		index = "InRingIndex";
		index += CGlbConvert::Int32ToStr(i);
		ring = const_cast<CGlbLine *>(mpr_polygonGeo->GetInRing(i));
		pointsCount = ring->GetCount();
		sprintf(str,"%d",pointsCount);
		xmlNewTextChild(node,NULL,(unsigned char *)index.c_str(),BAD_CAST str);
		points = ring->GetPoints();
		for(glbInt32 j = 0; j < pointsCount * 3;j++)
		{
			index = "InRingPointsIndex";
			index += CGlbConvert::Int32ToStr(i);
			index += CGlbConvert::Int32ToStr(j);
			sprintf_s(str,"%8lf",points[j]);
			xmlNewTextChild(node,NULL,(unsigned char *)index.c_str(),BAD_CAST str);
		}
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
		case GLB_POLYGONSYMBOL_2D:
			{
				((GlbPolygon2DSymbolInfo *)mpr_renderInfo.get())->Save(rdnode,prjPath);
			}break;
		case GLB_POLYGONSYMBOL_POLYHEDRON:
			{
				((GlbPolyhedronSymbolInfo *)mpr_renderInfo.get())->Save(rdnode,prjPath);
			}break;
		case GLB_POLYGONSYMBOL_BUILDING:
			{
				((GlbPolygonBuildingSymbolInfo *)mpr_renderInfo.get())->Save(rdnode,prjPath);
			}break;
		case GLB_POLYGONSYMBOL_WATER:
			{
				((GlbPolygonWaterSymbolInfo *)mpr_renderInfo.get())->Save(rdnode,prjPath);
			}break;
		case GLB_POLYGONSYMBOL_LAKE:
			{
				((GlbPolygonLakeSymbolInfo *)mpr_renderInfo.get())->Save(rdnode,prjPath);
			}break;
		default:
			break;
		}
	}
	return true;
}

glbref_ptr<CGlbExtent> CGlbGlobePolygon::GetBound( glbBool isWorld /*= true */ )
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
			geoExt = const_cast<CGlbExtent *>(mpt_feature->GetExtent());		
		else if(mpr_polygonGeo)		
			geoExt = const_cast<CGlbExtent *>(mpr_polygonGeo->GetExtent());				
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
			&& mpt_altitudeMode==GLB_ALTITUDEMODE_ONTERRAIN)
			UpdateElevate();
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
				osg::ref_ptr<osg::Node> node = mpt_node;
				CGlbCalculateBoundBoxVisitor bboxV;
				node->accept(bboxV);
				osg::BoundingBoxd bb =bboxV.getBoundBox();

				//osg::BoundingSphere bs = node->getBound();
				//osg::BoundingBoxd bb;
				//bb.expandBy(bs._center - osg::Vec3d(bs._radius,bs._radius,bs._radius));
				//bb.expandBy(bs._center + osg::Vec3d(bs._radius,bs._radius,bs._radius));
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

glbDouble CGlbGlobePolygon::GetDistance( osg::Vec3d &cameraPos,glbBool isCompute )
{
	if (!isCompute)
		return mpr_distance;

	if (!mpt_globe)	return DBL_MAX;

	if (mpt_isCameraAltitudeAsDistance)	
		mpr_distance = cameraPos.z();	
	else
	{
		osg::Vec3d position;
		if (mpt_feature)
		{
			CGlbExtent* ext = const_cast<CGlbExtent *>(mpt_feature->GetExtent());
			ext->GetCenter(&position.x(),&position.y(),&position.z());	
		}else{
			if (mpr_polygonGeo == NULL)
				return DBL_MAX;
			CGlbExtent *polyExtent = const_cast<CGlbExtent *>(mpr_polygonGeo->GetExtent());
			polyExtent->GetCenter(&position.x(),&position.y(),&position.z());	
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

glbBool CGlbGlobePolygon::SetGeo( CGlbPolygon *polygon ,glbBool &isNeedDirectDraw )
{
	isNeedDirectDraw = false;
	polygon = RemoveRepeatePoints(polygon);
	if(mpt_feature)
	{// 如果有要素，位置信息来源于要素.
		mpr_polygonGeo = polygon;
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
			mpr_polygonGeo = polygon;
			isNeedDirectDraw = true;						
			return true;
		}
		mpt_loadState = true;
	}

	mpr_polygonGeo = polygon;
	if(mpt_globe)
	{
		/*
		如果mpr_polygonGeo==NULL,对象是不在场景树上的.
		如果不UpdateObject，对象永远不在场景树上，这样永远不会调度该对象.
		*/
		mpt_globe->UpdateObject(this);
	}
	if (mpt_node       != NULL)
	{// lineGeo改变---->重绘
		/*
		mpt_node == NULL,也是需要重新绘制的!!!.
		*/
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

void CGlbGlobePolygon::DirectDraw( glbInt32 level )
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

void CGlbGlobePolygon::LoadData( glbInt32 level )
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
	glbref_ptr<GlbPolygonSymbolInfo> polygonSymbol = dynamic_cast<GlbPolygonSymbolInfo *>(mpr_renderInfo.get());
	if(polygonSymbol == NULL)
		return;
	// 非3D模型类型，且已经load过，就不用再次load  -------> 那种多边形有多level？？？
	if (/*polygonSymbol->type != GLB_OBJECTTYPE_POLYGON &&*/ mpt_currLevel != -1) 	
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
	glbref_ptr<CGlbGlobePolygonLoadDataTask> task = new CGlbGlobePolygonLoadDataTask(this,level);
	mpt_globe->mpr_taskmanager->AddTask(task.get());
}

void CGlbGlobePolygon::AddToScene()
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

glbInt32 CGlbGlobePolygon::RemoveFromScene( glbBool isClean )
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
	if(    mpt_altitudeMode == GLB_ALTITUDEMODE_ONTERRAIN )
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

void CGlbGlobePolygon::ReadData( glbInt32 level ,glbBool isDirect)
{
	glbref_ptr<GlbPolygonSymbolInfo> polygonSymbolInfo = dynamic_cast<GlbPolygonSymbolInfo *>(mpr_renderInfo.get());
	if (NULL==polygonSymbolInfo || (mpt_feature==NULL&&mpr_polygonGeo==NULL))	
	{mpt_loadState=false;return;}

	if(	mpt_altitudeMode == GLB_ALTITUDEMODE_ONTERRAIN 
		&&	polygonSymbolInfo->symType != GLB_POLYGONSYMBOL_2D)//普通多边形才能贴地
	{mpt_loadState=false;return;}

	// 刷新旧的区域
	if(	mpt_altitudeMode == GLB_ALTITUDEMODE_ONTERRAIN )
		DirtyOnTerrainObject();

	osg::Group *polygonContainer=NULL;
	if (mpt_feature)
	{// feature
		IGlbGeometry *geo = NULL;
		glbBool result = false;
		result = mpt_feature->GetGeometry(&geo,level);
		if(!result || geo==NULL) 
		{mpt_loadState=false;return;}

		// 使用智能指针防止多线程访问导致mpr_polygonGeo在Draw过程中被卸载
		glbref_ptr<IGlbGeometry> tempGeo = geo;

		CGlbGlobePolygonSymbol _polySymbol;
		// 计算outline				
		glbref_ptr<CGlbMultiPolygon> multiOutline = new CGlbMultiPolygon();
		glbref_ptr<IGlbGeometry> outline = _polySymbol.GetOutline(this,geo);
		if(outline == NULL){mpt_loadState=false;return;}
		GlbGeometryTypeEnum geoType = outline->GetType();
		switch(geoType)
		{
		case GLB_GEO_POLYGON:
			{
				CGlbPolygon* poly = dynamic_cast<CGlbPolygon*>(outline.get());
				multiOutline->AddPolygon(poly);
			}					
			break;
		case GLB_GEO_MULTIPOLYGON:
			{
				CGlbMultiPolygon* multiPoly = dynamic_cast<CGlbMultiPolygon*>(outline.get());
				glbInt32 polyCnt = multiPoly->GetCount();
				for (int k = 0; k < polyCnt; k++)
				{
					CGlbPolygon *polygon = const_cast<CGlbPolygon *>(multiPoly->GetPolygon(k));
					multiOutline->AddPolygon(polygon);
				}
			}
			break;
		}

		GlbScopedLock<GlbCriticalSection> lock(mpr_outline_critical);
		mpr_outline = multiOutline;
		if (mpt_altitudeMode == GLB_ALTITUDEMODE_ONTERRAIN)
		{// 贴地
			// 刷新新的区域
			if (polygonSymbolInfo->symType == GLB_POLYGONSYMBOL_2D)
				DirtyOnTerrainObject();
		} 
		else
		{// 非贴地
			osg::Node *polygonNode=NULL;
			switch(polygonSymbolInfo->symType)
			{
			case GLB_POLYGONSYMBOL_2D:
				{
					GlbPolygon2DSymbolInfo *polygon2DSymbolInfo = dynamic_cast<GlbPolygon2DSymbolInfo *>(polygonSymbolInfo.get());
					if (polygon2DSymbolInfo)
					{
						polygonNode = _polySymbol.Draw(this,geo);
						if (polygonContainer==NULL)	polygonContainer=new osg::Group();
						polygonContainer->addChild(polygonNode);
					}
				}
				break;
			case GLB_POLYGONSYMBOL_POLYHEDRON:
				break;
			case GLB_POLYGONSYMBOL_BUILDING:
				break;
			case GLB_POLYGONSYMBOL_WATER:
				{
					GlbPolygonWaterSymbolInfo *polygonWaterSymbolInfo = dynamic_cast<GlbPolygonWaterSymbolInfo *>(polygonSymbolInfo.get());
					if (polygonWaterSymbolInfo)
					{
						CGlbGlobePolygonWaterSymbol _polyWaterSymbol;
						polygonNode = _polyWaterSymbol.Draw(this,geo);
						if (polygonContainer==NULL)	polygonContainer=new osg::Group();
						polygonContainer->addChild(polygonNode);
					}
				}
				break;
			case GLB_POLYGONSYMBOL_LAKE:
				{
					GlbPolygonLakeSymbolInfo *polygonLakeSymbolInfo = dynamic_cast<GlbPolygonLakeSymbolInfo *>(polygonSymbolInfo.get());
					if (polygonLakeSymbolInfo)
					{
						CGlbGlobePolygonLakeSymbol _polyLakeSymbol;
						polygonNode = _polyLakeSymbol.Draw(this,geo);
						if (polygonContainer==NULL)	polygonContainer=new osg::Group();
						polygonContainer->addChild(polygonNode);
					}
				}
				break;
			}	
		}
	} 
	else 
	{// polygonGeo
		CGlbGlobePolygonSymbol symbol;
		IGlbGeometry* _outline = symbol.GetOutline(this,mpr_polygonGeo.get());
		{	
			GlbScopedLock<GlbCriticalSection> lock(mpr_outline_critical);
			mpr_outline = _outline;
		}
		osg::Node *node = NULL;
		if (mpt_altitudeMode == GLB_ALTITUDEMODE_ONTERRAIN)
		{//贴地
			if (polygonSymbolInfo->symType == GLB_POLYGONSYMBOL_2D)									
				DirtyOnTerrainObject();						
		} 
		else
		{//非贴地	
			// 使用智能指针防止多线程访问导致mpr_polygonGeo在Draw过程中被卸载
			glbref_ptr<CGlbPolygon> tempGeo = mpr_polygonGeo;

			switch(polygonSymbolInfo->symType)
			{
			case GLB_POLYGONSYMBOL_2D:
				{
					GlbPolygon2DSymbolInfo* polygon2DSymbolInfo = dynamic_cast<GlbPolygon2DSymbolInfo *>(polygonSymbolInfo.get());
					if (polygon2DSymbolInfo)
					{
						CGlbGlobePolygonSymbol polygonSymbol;
						node = polygonSymbol.Draw(this,mpr_polygonGeo.get());
					}
				}
				break;
			case GLB_POLYGONSYMBOL_POLYHEDRON:
				{
					GlbPolyhedronSymbolInfo *polyhedronSymbolInfo = dynamic_cast<GlbPolyhedronSymbolInfo *>(polygonSymbolInfo.get());
					if (polyhedronSymbolInfo)
					{
						CGlbGlobePolyhedronSymbol polygonSymbol;
						node = polygonSymbol.Draw(this,mpr_polygonGeo.get());
					}
				}
				break;
			case GLB_POLYGONSYMBOL_BUILDING:
				{
					GlbPolygonBuildingSymbolInfo *polyBuildingInfo = dynamic_cast<GlbPolygonBuildingSymbolInfo *>(polygonSymbolInfo.get());
					if(polyBuildingInfo) 
					{
						CGlbGlobePolygonBuildingSymbol polygonBuildindSymbol;
						node = polygonBuildindSymbol.Draw(this,mpr_polygonGeo.get());
					}
				}
				break;
			case GLB_POLYGONSYMBOL_WATER:
				{
					GlbPolygonWaterSymbolInfo *polygonWaterSymbolInfo = dynamic_cast<GlbPolygonWaterSymbolInfo *>(polygonSymbolInfo.get());
					if (polygonWaterSymbolInfo) 
					{
						CGlbGlobePolygonWaterSymbol polygonSymbol;
						node = polygonSymbol.Draw(this,mpr_polygonGeo.get());
					}					
				}
				break;
			case GLB_POLYGONSYMBOL_LAKE:
				{
					GlbPolygonLakeSymbolInfo *polygonLakeSymbolInfo = dynamic_cast<GlbPolygonLakeSymbolInfo *>(polygonSymbolInfo.get());
					if (polygonLakeSymbolInfo) 
					{
						CGlbGlobePolygonLakeSymbol lakeSymbol;
						node = lakeSymbol.Draw(this,mpr_polygonGeo.get());
					}					
				}
				break;
			}		
			
			if (NULL!=node)
			{
				polygonContainer = new osg::Group();
				polygonContainer->addChild(node);
				//polygonContainer=node->asGroup();
			}
			else{
				mpt_loadState=false;
				//为了界面添加点的时候可以看到polygon对象的实时更新
				mpt_node = new osg::Switch;
				return;
			}
		}			
	}		

	osg::ref_ptr<osg::Switch> swiNode = new osg::Switch();
	//swiNode->setValue(0,mpt_isShow);
	if (polygonContainer!=NULL)
		swiNode->addChild(polygonContainer);

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

glbBool CGlbGlobePolygon::SetAltitudeMode( GlbAltitudeModeEnum mode ,glbBool &isNeedDirectDraw )
{
	isNeedDirectDraw = false;
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
void CGlbGlobePolygon::SetRenderOrder( glbInt32 order )
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

// 相对地形渲染时候由globe update中调用
void CGlbGlobePolygon::UpdateElevate()
{
	if (mpt_globe==NULL)	return;

	glbDouble elevation = 0.0;
	CGlbExtent* ext = NULL;
	if (mpt_feature)	
		ext = const_cast<CGlbExtent *>(mpt_feature->GetExtent());	
	else if (mpr_polygonGeo)
		ext = const_cast<CGlbExtent *>(mpr_polygonGeo->GetExtent());

	if (!ext)
		return;

	glbDouble xOrLon,yOrLat;
	ext->GetCenter(&xOrLon,&yOrLat);
	elevation = mpt_globe->GetElevationAt(xOrLon,yOrLat);	

	if(fabs(elevation - mpr_elevation) < 0.0001)
		return;

	mpr_elevation=elevation;
	//更新Maxtrixform
	if(mpt_altitudeMode == GLB_ALTITUDEMODE_RELATIVETERRAIN)
		DealModePosByChangeAltitudeOrChangePos();
}

void CGlbGlobePolygon::DealModePosByChangeAltitudeOrChangePos()
{
	if(mpt_node == NULL || mpt_globe == NULL)
		return;	

	unsigned int numChild = mpt_node->asSwitch()->getNumChildren();
	if(numChild < 1)
	{
		mpt_globe->UpdateObject(this);
		return;
	}


	if (mpt_feature!=NULL)
	{
		IGlbGeometry *geo=NULL;
		bool result = false;
		result = mpt_feature->GetGeometry(&geo,mpt_currLevel);
		if(!result || geo == NULL) 
			return;
		osg::Group *pContainer=mpt_node->asSwitch()->getChild(0)->asGroup();

		GlbGeometryTypeEnum geoType = geo->GetType();
		switch(geoType)
		{
		case GLB_GEO_POLYGON:
			{
				CGlbExtent *pGeoExt = const_cast<CGlbExtent *>(geo->GetExtent());
				if (pGeoExt==NULL) return;
				glbDouble xOrLon,yOrLat,zOrAlt;
				pGeoExt->GetCenter(&xOrLon,&yOrLat,&zOrAlt);
				osg::Vec3d position(xOrLon,yOrLat,zOrAlt);
				ComputePosByAltitudeAndGloleType(position);

				osg::Group *pGroupTop=pContainer->getChild(0)->asGroup();
				osg::Transform *pTransform=pGroupTop->getChild(0)->asTransform();
				osg::MatrixTransform *mt = dynamic_cast<osg::MatrixTransform *>(pTransform);
				osg::Matrix &m = const_cast<osg::Matrix &>(mt->getMatrix());
				m.setTrans(position);
				mt->setMatrix(m);
			}					
			break;
		case GLB_GEO_MULTIPOLYGON:
			{
				CGlbMultiPolygon* multiPoly = dynamic_cast<CGlbMultiPolygon*>(geo);
				glbInt32 polyCnt = multiPoly->GetCount();
				//for (int k = 0; k < polyCnt; k++)
				{
					//CGlbPolygon *polygon = const_cast<CGlbPolygon *>(multiPoly->GetPolygon(k));
					CGlbExtent *pGeoExt = const_cast<CGlbExtent *>(multiPoly->GetExtent());
					if (pGeoExt==NULL)
						return;
					glbDouble xOrLon,yOrLat,zOrAlt;
					pGeoExt->GetCenter(&xOrLon,&yOrLat,&zOrAlt);
					osg::Vec3d position(xOrLon,yOrLat,zOrAlt);
					ComputePosByAltitudeAndGloleType(position);

					osg::Group *pGroupTop=pContainer->getChild(0)->asGroup();
					osg::Transform *pTransform=pGroupTop->getChild(0)->asTransform();
					osg::MatrixTransform *mt = dynamic_cast<osg::MatrixTransform *>(pTransform);
					osg::Matrix &m = const_cast<osg::Matrix &>(mt->getMatrix());
					m.setTrans(position);
					mt->setMatrix(m);
				}
			}
			break;
		}	
	} 
	else if (mpr_polygonGeo!=NULL)
	{
		CGlbExtent *pGeoExt = const_cast<CGlbExtent *>(mpr_polygonGeo->GetExtent());
		if (pGeoExt==NULL)
			return;
		glbDouble xOrLon,yOrLat,zOrAlt;
		pGeoExt->GetCenter(&xOrLon,&yOrLat,&zOrAlt);
		osg::Vec3d position(xOrLon,yOrLat,zOrAlt);
		ComputePosByAltitudeAndGloleType(position);

		osg::Group *pGroupTop=mpt_node->asSwitch()->getChild(0)->asGroup();
		osg::Transform *pTransform=pGroupTop->getChild(0)->asTransform();
		osg::MatrixTransform *mt = dynamic_cast<osg::MatrixTransform *>(pTransform);
		osg::Matrix &m = const_cast<osg::Matrix &>(mt->getMatrix());
		m.setTrans(position);
		mt->setMatrix(m);
	}

	mpt_globe->UpdateObject(this);
}

void CGlbGlobePolygon::ComputePosByAltitudeAndGloleType( osg::Vec3d &position )
{	
	if (mpt_globe==NULL) return;
	if (mpt_globe->GetType() == GLB_GLOBETYPE_GLOBE)
	{
		if (mpt_altitudeMode == GLB_ALTITUDEMODE_ABSOLUTE)
		{
			//把xOrLon、yOrLat、zOrAlt的值转换成世界坐标赋值给position的x、y、z
			g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(position.y()),
				osg::DegreesToRadians(position.x()),position.z(),position.x(),position.y(),position.z());
		}
		else if (mpt_altitudeMode == GLB_ALTITUDEMODE_RELATIVETERRAIN)
		{
			//把xOrLon、yOrLat、zOrAlt的值转换成世界坐标赋值给position的x、y、z
			g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(position.y()),
				osg::DegreesToRadians(position.x()),position.z()+mpr_elevation,position.x(),position.y(),position.z());
			//position.z() = position.z() + mpt_globe->GetElevationAt(mpr_xOrLon,mpr_yOrLat);			 
		}
		else if (mpt_altitudeMode == GLB_ALTITUDEMODE_ONTERRAIN)
		{
			g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(position.y()),
				osg::DegreesToRadians(position.x()),mpr_elevation,position.x(),position.y(),position.z());
		}
	}
	else if (mpt_globe->GetType() == GLB_GLOBETYPE_FLAT)
	{
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

/*
*   获取2D绘制时的外围轮廓.
*   轮廓 ------>多Poly - CGlbMultiPolygon
*/
IGlbGeometry * CGlbGlobePolygon::GetOutLine()
{
	if (mpr_outline!=NULL)
		return mpr_outline.get();	
	glbref_ptr<IGlbGeometry> geom = AnalysisPolyGeomertry(mpt_feature.get(),mpr_polygonGeo.get());
	{
		GlbScopedLock<GlbCriticalSection> lock(mpr_outline_critical);
		mpr_outline = geom;
	}
	if (mpr_outline)
		return mpr_outline.get();

	return NULL;
}

glbBool CGlbGlobePolygon::SetRenderInfo( GlbRenderInfo *renderInfo  ,glbBool &isNeedDirectDraw )
{
	if (renderInfo == NULL || renderInfo->type != GLB_OBJECTTYPE_POLYGON)
		return false;
	GlbPolygonSymbolInfo *polygonInfo = static_cast<GlbPolygonSymbolInfo *>(renderInfo);
	if (polygonInfo == NULL) return false;		
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
		ParseObjectFadeColor();
		mpt_loadState    = false;	
		return true;
	}
	/*GlbPolygon2DSymbolInfo *tempPolygonInfo = dynamic_cast<GlbPolygon2DSymbolInfo *>(mpr_renderInfo.get());	
	if(polygonInfo->symType != tempPolygonInfo->symType)
	{
	isNeedDirectDraw = true;
	mpr_renderInfo   = renderInfo;
	mpt_loadState    = false;
	return true;
	}*/
	glbBool rt = false;
	switch(polygonInfo->symType)
	{
	case GLB_POLYGONSYMBOL_2D:
		{
			GlbPolygon2DSymbolInfo *polygon2DInfo= dynamic_cast<GlbPolygon2DSymbolInfo *>(polygonInfo);
			if (polygon2DInfo != NULL)			
				rt = judgePolygon2DNeedDirectDraw(polygon2DInfo,isNeedDirectDraw);			
		}
		break;
	case GLB_POLYGONSYMBOL_POLYHEDRON:
		{
			GlbPolyhedronSymbolInfo *polyhedronInfo = dynamic_cast<GlbPolyhedronSymbolInfo *>(polygonInfo);
			if(polyhedronInfo != NULL) 
				rt = judgePolyhedronNeedDirectDraw(polyhedronInfo,isNeedDirectDraw);
		}
		break;
	case GLB_POLYGONSYMBOL_BUILDING:
		{
			GlbPolygonBuildingSymbolInfo *polyBuildingInfo = dynamic_cast<GlbPolygonBuildingSymbolInfo *>(polygonInfo);
			if(polyBuildingInfo != NULL)
				rt = judgePolygonBuildingNeedDirectDraw(polyBuildingInfo,isNeedDirectDraw);
		}
		break;
	case GLB_POLYGONSYMBOL_WATER:
		{
			GlbPolygonWaterSymbolInfo *polygonWaterInfo = dynamic_cast<GlbPolygonWaterSymbolInfo *>(polygonInfo);
			if(polygonWaterInfo != NULL)
				rt = judgePolygonWaterNeedDirectDraw(polygonWaterInfo,isNeedDirectDraw);
		}
		break;
	case GLB_POLYGONSYMBOL_LAKE:
		{
			GlbPolygonLakeSymbolInfo *polygonLakeInfo = dynamic_cast<GlbPolygonLakeSymbolInfo *>(polygonInfo);
			if(polygonLakeInfo != NULL)
				rt = judgePolygonLakeNeedDirectDraw(polygonLakeInfo,isNeedDirectDraw);
		}
		break;
	}		

	if(rt)mpr_renderInfo = renderInfo;
	mpt_loadState  = false;
	return rt;
}

GlbRenderInfo * CGlbGlobePolygon::GetRenderInfo(){return mpr_renderInfo.get();}

glbBool CGlbGlobePolygon::SetShow(glbBool isShow,glbBool isOnState/*= false*/)
{	
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
	mpr_isCanDrawImage = isnew;
	if (mpt_isEdit && isOnState)// 编辑状态不再由调度控制显隐
		return true;

	glbref_ptr<GlbPolygonSymbolInfo> polygonSymbolInfo = dynamic_cast<GlbPolygonSymbolInfo *>(mpr_renderInfo.get());
	if (polygonSymbolInfo==NULL)	return false;

	if (mpt_isEdit && isOnState)// 编辑状态不再由调度控制显隐
		return true;

	if( mpt_altitudeMode == GLB_ALTITUDEMODE_ONTERRAIN )
	{// 如果是贴地绘制对象，需要刷新贴地区域
		/*
		* mpr_isCanDrawImage控制在DrawToImage时是否需要绘制
		*/
		//if (mpr_isCanDrawImage != isnew)
		{
			//mpr_isCanDrawImage = isnew;
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

glbBool CGlbGlobePolygon::SetSelected( glbBool isSelected )
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

glbBool CGlbGlobePolygon::SetBlink( glbBool isBlink )
{
	if (mpt_isBlink == isBlink) 
		return true;
	mpt_isBlink = isBlink;
	if (mpt_node != NULL && mpt_altitudeMode!=GLB_ALTITUDEMODE_ONTERRAIN)
		DealBlink();
	return true;
}

IGlbGeometry* CGlbGlobePolygon::AnalysisPolyGeomertry(CGlbFeature* feature, CGlbPolygon* polyGeo)
{
	CGlbGlobePolygonSymbol symbol;	
	if (feature != NULL)
	{
		glbref_ptr<CGlbMultiPolygon> polys = new CGlbMultiPolygon();

		glbUInt32            level      = 0;
		IGlbGeometry *geo = NULL;
		bool                 result     = false;

		result = mpt_feature->GetGeometry(&geo,level);
		if(!result || geo==NULL)  return NULL;

		glbref_ptr<IGlbGeometry> outline = symbol.GetOutline(this,geo);
		if (outline == NULL) return NULL;
		GlbGeometryTypeEnum geoType = outline->GetType();
		switch(geoType)
		{
		case GLB_GEO_POLYGON:
			{
				CGlbPolygon* poly = dynamic_cast<CGlbPolygon*>(outline.get());
				polys->AddPolygon(poly);
			}					
			break;
		case GLB_GEO_MULTIPOLYGON:
			{
				CGlbMultiPolygon* multiPoly = dynamic_cast<CGlbMultiPolygon*>(outline.get());
				glbInt32 polyCnt = multiPoly->GetCount();
				for (int k = 0; k < polyCnt; k++)
				{
					CGlbPolygon *polygon = const_cast<CGlbPolygon *>(multiPoly->GetPolygon(k));
					polys->AddPolygon(polygon);
				}
			}
			break;
		}		

		if (polys->GetCount()>0)
			return polys.release();
	}
	else if (polyGeo)
	{		
		glbref_ptr<IGlbGeometry> geom = symbol.GetOutline(this,polyGeo);
		return geom.release();
	}
	return NULL;
}

void CGlbGlobePolygon::DrawToImage( glbByte *image, glbInt32 imageW,glbInt32 imageH,CGlbExtent &ext )
{
	if (!mpr_isCanDrawImage) return;	
	glbref_ptr<GlbPolygonSymbolInfo> polygonSymbolInfo = dynamic_cast<GlbPolygonSymbolInfo *>(mpr_renderInfo.get());
	if (polygonSymbolInfo==NULL)
		return;

	if(mpt_altitudeMode != GLB_ALTITUDEMODE_ONTERRAIN /*||
													  polygonSymbolInfo->symType != GLB_POLYGONSYMBOL_2D*/)
													  return;

	glbref_ptr<IGlbGeometry> polys = GetOutLine();			
	if (!polys)	return;

	//{// 过滤在image中只占小于1个像素的多边形
	//	glbref_ptr<CGlbExtent> geomExt = polys->GetExtent();
	//	glbDouble area = geomExt->GetXWidth() * geomExt->GetYHeight() * imageW * imageH / (ext.GetXWidth() * ext.GetYHeight());
	//	if (area < 1.0)
	//		return;
	//}	
	//*/
	CGlbGlobePolygonSymbol polygon2DSymbol;	
	polygon2DSymbol.DrawToImage(this,polys.get(),image,imageW,imageH,ext);
}

glbInt32 CGlbGlobePolygon::GetOsgNodeSize()
{
	if (mpt_node == NULL) return 0;
	if (mpr_objSize==0)
		mpr_objSize = ComputeNodeSize(mpt_node);
	return mpr_objSize;
}

//////////////////////////////////////////////////////////////////////////
void CGlbGlobePolygon::DealSelected()
{
	if (mpt_node==NULL)	return;

	if (mpt_altitudeMode==GLB_ALTITUDEMODE_ONTERRAIN)	return;

	if (mpt_feature!=NULL)
	{
		osg::Group *pContainer=mpt_node->asSwitch()->getChild(0)->asGroup();
		unsigned int childNum=pContainer->getNumChildren();
		for (unsigned int childId=0;childId<childNum;++childId)
		{
			osg::Group *pGroupTop=pContainer->getChild(childId)->asGroup();
			osg::StateSet *sset = pGroupTop->getOrCreateStateSet();
			sset->getUniform("PolygonSelected")->set(mpt_isSelected);
		}
	}
	else if (mpr_polygonGeo!=NULL)
	{
		osg::Group *pGroupTop=mpt_node->asSwitch()->getChild(0)->asGroup()->getChild(0)->asGroup();
		osg::StateSet *sset = pGroupTop->getOrCreateStateSet();
		if(sset->getUniform("PolygonSelected"))
			sset->getUniform("PolygonSelected")->set(mpt_isSelected);
	}		
}

void CGlbGlobePolygon::DealBlink()
{
	if (mpt_node==NULL)
		return;
	if (mpt_altitudeMode==GLB_ALTITUDEMODE_ONTERRAIN)
		return;

	if(mpt_isBlink)
	{
		PolygonObjectBlinkCallback *blinkCb = new PolygonObjectBlinkCallback();
		mpt_node->addUpdateCallback(blinkCb);
	}
	else
	{
		osg::NodeCallback *callback = mpt_node->getUpdateCallback();
		if (callback)
		{
			mpt_node->removeUpdateCallback(callback);
			DirectDraw(0);
		}
	}
	//if (mpt_feature!=NULL)
	//{
	//	osg::Group *pContainer=mpt_node->asSwitch()->getChild(0)->asGroup();
	//	unsigned int childNum=pContainer->getNumChildren();
	//	for (unsigned int childId=0;childId<childNum;++childId)
	//	{
	//		osg::Group *pGroupTop=pContainer->getChild(childId)->asGroup();
	//		osg::StateSet *sset = pGroupTop->getOrCreateStateSet();
	//		sset->getUniform("PolygonBlink")->set(mpt_isBlink);
	//	}
	//}
	//else if (mpr_polygonGeo!=NULL)
	//{
	//	osg::Group *pGroupTop=mpt_node->asSwitch()->getChild(0)->asGroup();
	//	osg::StateSet *sset = pGroupTop->getOrCreateStateSet();
	//	sset->getUniform("PolygonBlink")->set(mpt_isBlink);
	//}
}

class PolygonEditEventHandler : public osgGA::GUIEventHandler
{
public:
	PolygonEditEventHandler(CGlbGlobePolygon *obj):mpr_polygonObj(obj)
	{
		mpr_globe = mpr_polygonObj->GetGlobe();
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
		static glbBool isInRing = false;
		static glbInt32 inRingIndex = -1;

		static osg::ref_ptr<osg::MatrixTransform> moveMt = NULL;//鼠标move与线相交产生的点，记录下来，方便删除
		glbBool isGlobe = mpr_globe->GetType() == GLB_GLOBETYPE_GLOBE;

		//处理move与编辑点相交
		static glbBool isMoveIntersect = false;
		static glbInt32 moveIntersectIndex = -1;
		static glbBool isColorChanged = false;//记录因为鼠标滑动与编辑点相交修改过编辑点其中一点的颜色

		static glbBool isNeedUpdate = false;//编辑渲染橡皮筋节点，鼠标Release更新polygonObj
		static glbref_ptr<CGlbLine> updateOutRingline = NULL;//记录鼠标Release更新polygonObj需要的点集
		static glbref_ptr<CGlbLine> updateInRingline = NULL;
		static osg::ref_ptr<osg::MatrixTransform> bungeeMt = NULL;

		//计算相交阈值
		double pixelWidth = mpr_globe->GetView()->GetCurrentPixelWidth();

		//CallBack原始代码——————begin

		if(mpr_polygonObj->IsEdit())//线在编辑状态
		{
			GlbGlobeRObEditModeEnum editMode = mpr_polygonObj->GetEditMode();
			if(mpr_boundGeode == NULL)//如果包围盒为空，给线对象创建包围盒
			{
				if(isGlobe)
					mpr_boundExtent = mpr_polygonObj->GetBound(false);
				else
					mpr_boundExtent = mpr_polygonObj->GetBound(true);
				osg::Vec3d center(0.0,0.0,0.0);
				mpr_boundExtent->GetCenter(&center.x(),&center.y(),&center.z());
				osg::Matrixd localToWorld;
				if(isGlobe)
				{
					if(mpr_polygonObj->GetAltitudeMode() == GLB_ALTITUDEMODE_RELATIVETERRAIN)
						center.z() += mpr_polygonObj->GetGlobe()->GetElevationAt(center.x(),center.y());
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
				mpr_polygonObj->GetEditNode()->addChild(mt);
			}

			glbref_ptr<CGlbExtent> geoExt = NULL;
			if(isGlobe)
				geoExt = mpr_polygonObj->GetBound(false);
			else
				geoExt = mpr_polygonObj->GetBound(true);
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
					if(mpr_polygonObj->GetAltitudeMode() == GLB_ALTITUDEMODE_RELATIVETERRAIN)
						center.z() += mpr_polygonObj->GetGlobe()->GetElevationAt(center.x(),center.y());
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
					mpr_polygonObj->GetEditNode()->removeChild(mt);
					mt = NULL;
				}
				mpr_pointsGeode = NULL;
				if(mpr_isGeoChanged)
					mpr_isGeoChanged = false;
				if(isColorChanged)
					isColorChanged = false;
			}

			//补充平面编辑？？？？？？？
			if(mpr_pointsGeode == NULL && mpr_polygonObj->GetEditMode() == GLB_ROBJECTEDIT_SHAPE)
			{
				mpr_pointsGeode = new osg::Geode;
				osg::ref_ptr<osg::Geometry> pointsGeom = new osg::Geometry;
				CGlbPolygon *polygon3D = mpr_polygonObj->GetGeo();
				osg::ref_ptr<osg::Vec3dArray> vertexes = new osg::Vec3dArray;
				osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array;
				CGlbLine *extRingLine = const_cast<CGlbLine *>(polygon3D->GetExtRing());
				const glbDouble *points = extRingLine->GetPoints();

				osg::Matrixd localToWorld;
				if(isGlobe)
					g_ellipsoidModel->computeLocalToWorldTransformFromLatLongHeight(
					osg::DegreesToRadians(points[1]),osg::DegreesToRadians(points[0]),points[2],localToWorld);
				else
					localToWorld.makeTranslate(points[0],points[1],points[2]);
				osg::Matrixd worldTolocal = osg::Matrixd::inverse(localToWorld);

				for (glbInt32 i = 0; i < extRingLine->GetCount();i++)
				{
					osg::Vec3d point(points[3 * i],points[3 * i + 1],points[3 * i + 2]);
					if(isGlobe)
						g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(point.y()),
						osg::DegreesToRadians(point.x()),point.z(),point.x(),point.y(),point.z());
					vertexes->push_back(point * worldTolocal);
					colors->push_back(osg::Vec4(1.0,1.0,1.0,1.0));
				}

				glbInt32 inRingCount = polygon3D->GetInRingCount();
				for(glbInt32 i = 0; i < inRingCount;i++)
				{
					CGlbLine *inRingLine = const_cast<CGlbLine *>(polygon3D->GetInRing(i));
					if(inRingLine == NULL)
						continue;

					points = inRingLine->GetPoints();
					for (glbInt32 j = 0; j < inRingLine->GetCount();j++)
					{
						osg::Vec3d point(points[3 * j],points[3 * j + 1],points[3 * j + 2]);
						if(isGlobe)
							g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(point.y()),
							osg::DegreesToRadians(point.x()),point.z(),point.x(),point.y(),point.z());
						vertexes->push_back(point * worldTolocal);
						colors->push_back(osg::Vec4(1.0,1.0,1.0,1.0));
					}
				}

				pointsGeom->setVertexArray(vertexes);
				pointsGeom->setColorArray(colors);
				pointsGeom->setColorBinding(osg::Geometry::BIND_PER_VERTEX);
				pointsGeom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::POINTS,0,vertexes->size()));
				osg::ref_ptr<osg::StateSet> stateset = mpr_pointsGeode->getOrCreateStateSet();
				osg::ref_ptr<osg::Point> point = new osg::Point;
				point->setSize(7);
				stateset->setAttributeAndModes(point,osg::StateAttribute::ON);
				stateset->setMode(GL_LIGHTING,osg::StateAttribute::OFF);
				//stateset->setMode(GL_DEPTH_TEST,osg::StateAttribute::ON);
				stateset->setRenderBinDetails(2,"RenderBin");
				osg::ref_ptr<osg::Depth> depth = new osg::Depth;
				depth->setFunction(osg::Depth::ALWAYS);
				stateset->setAttributeAndModes(depth,osg::StateAttribute::ON);
				mpr_pointsGeode->addDrawable(pointsGeom);
				mt = new osg::MatrixTransform;
				mt->setMatrix(localToWorld);
				mt->addChild(mpr_pointsGeode);
				osg::Group *editNode = mpr_polygonObj->GetEditNode();
				if(editNode)
					editNode->addChild(mt);
			}

			if(mpr_pointsGeode != NULL && mpr_polygonObj->GetEditMode() != GLB_ROBJECTEDIT_SHAPE)
			{
				osg::Group *editNode = mpr_polygonObj->GetEditNode();
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
				if(isInRing)
				{
					CGlbPolygon *polygon3D = mpr_polygonObj->GetGeo();
					CGlbLine *extRingLine = const_cast<CGlbLine *>(polygon3D->GetExtRing());
					glbInt32 tempindex = moveIntersectIndex;
					//外圈所有点
					moveIntersectIndex = extRingLine->GetCount();
					for(glbInt32 i = 0; i < inRingIndex; i++)
					{
						CGlbLine *inRingLine = const_cast<CGlbLine *>(polygon3D->GetInRing(i));
						moveIntersectIndex += inRingLine->GetCount();
					}
					moveIntersectIndex += tempindex;
				}
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
				osg::Node *node = mpr_polygonObj->GetOsgNode();
				if(!node)
					return false;

				glbref_ptr<CGlbExtent> boundExtent = mpr_polygonObj->GetBound(true);
				glbDouble minX,minY,minZ,maxX,maxY,maxZ;
				boundExtent->GetMin(&minX,&minY,&minZ);
				boundExtent->GetMax(&maxX,&maxY,&maxZ);
				osg::BoundingBox bb(minX,minY,minZ,maxX,maxY,maxZ);
				isIntersect = intersectRayBoundingBox(start,end,bb);//判断射线与对象的包围盒相交
				if(isIntersect == false)
					return false;

				//先确定按下鼠标时是否与编辑点相交，相交则记录相交点位置
				CGlbPolygon *polygon3D = mpr_polygonObj->GetGeo();
				CGlbLine *extRingLine = const_cast<CGlbLine *>(polygon3D->GetExtRing());
				const glbDouble *points = extRingLine->GetPoints();
				osg::Vec3d point;
				for (glbInt32 i = 0 ; i < extRingLine->GetCount() ; i++)
				{
					point = osg::Vec3d(points[3 * i],points[3 * i + 1],points[3 * i + 2]);
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

				if(isIntersectPoint == false)
				{
					glbInt32 inRingCount = polygon3D->GetInRingCount();
					for(glbInt32 i = 0; i < inRingCount; i++)
					{
						CGlbLine *inRingLine = const_cast<CGlbLine *>(polygon3D->GetInRing(i));
						if(inRingLine == NULL)
							continue;
						points = inRingLine->GetPoints();
						for (glbInt32 j = 0 ; j < extRingLine->GetCount() ; j++)
						{
							point = osg::Vec3d(points[3 * j],points[3 * j + 1],points[3 * j + 2]);
							if(isGlobe)
								g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(point.y()),
								osg::DegreesToRadians(point.x()),point.z(),point.x(),point.y(),point.z());
							osg::ref_ptr<osg::LineSegment> seg = new osg::LineSegment(start,end);
							bool intersected = intersect3D_SegmentPoint(seg,point,1.5 * pixelWidth/*0.1*/);
							if(intersected == false)
								continue;

							isIntersectPoint = true;
							isInRing = true;
							inRingIndex = i;
							intersectIndex = j;
							break;
						}
					}
				}

				//如果没有与编辑点相交，在对象编辑几何形态，点下鼠标与线段相交要添加编辑点
				if(isIntersectPoint == false && mpr_polygonObj->GetEditMode() == GLB_ROBJECTEDIT_SHAPE)
				{
					CGlbPolygon *polygon3D = mpr_polygonObj->GetGeo();
					glbref_ptr<CGlbPolygon> newPolygonGeo = new CGlbPolygon(3,false);
					CGlbLine *extRingLine = const_cast<CGlbLine *>(polygon3D->GetExtRing());
					const glbDouble *points = extRingLine->GetPoints();
					osg::Vec3d startPoint,endPoint;
					glbBool isIntersectOutRing = false;//相交外圈
					glbBool isIntersectInRing = false;//相交内圈
					//判断鼠标与外圈线段相交添加编辑点
					for (glbInt32 i = 0 ; i < extRingLine->GetCount(); i++)
					{
						startPoint = osg::Vec3d(points[3 * i],points[3 * i + 1],points[3 * i + 2]);
						if(isGlobe)
							g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(startPoint.y()),
							osg::DegreesToRadians(startPoint.x()),startPoint.z(),startPoint.x(),startPoint.y(),startPoint.z());
						if(i == extRingLine->GetCount() - 1)
							endPoint = osg::Vec3d(points[0],points[1],points[2]);
						else
							endPoint = osg::Vec3d(points[3 * (i + 1)],points[3 * (i + 1) + 1],points[3 * (i + 1) + 2]);
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
						if(mpr_polygonObj->GetEditNode()->getNumChildren() < 2)
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

						glbref_ptr<CGlbLine> outRing = new CGlbLine(3,false);
						for (glbInt32 j = 0; j< extRingLine->GetCount();j++)
						{
							if(j == i + 1)
							{
								outRing->AddPoint(outPoint.x(),outPoint.y(),outPoint.z());
								intersectIndex = j;
							}
							outRing->AddPoint(points[3 * j],points[3 * j + 1],points[3 * j + 2]);
						}
						if(i == extRingLine->GetCount() - 1)
						{
							outRing->AddPoint(outPoint.x(),outPoint.y(),outPoint.z());
							intersectIndex = outRing->GetCount();
						}
						newPolygonGeo->SetExtRing(outRing.get());
						isIntersectOutRing = true;
						break;
					}

					if(isIntersectOutRing)
					{
						glbInt32 inRingCount = polygon3D->GetInRingCount();
						CGlbLine *inRingLine = NULL;
						for(glbInt32 i = 0; i < inRingCount; i++)
						{
							inRingLine = const_cast<CGlbLine *>(polygon3D->GetInRing(i));
							if(inRingLine == NULL)
								continue;
							newPolygonGeo->AddInRing(inRingLine);
						}
						glbBool isNULL = false;
						mpr_polygonObj->SetGeo(newPolygonGeo.get(),isNULL);
						mpr_isGeoChanged = true;
						mpr_polygonObj->DirectDraw(0);
						isIntersectPoint = true;
						osg::Group *group = mpr_polygonObj->GetEditNode();
						if(group && moveMt)
						{
							group->removeChild(moveMt);
							moveMt = NULL;
						}
						//return false;
						return true;
					}

					//判断鼠标与内圈线段相交添加编辑点
					newPolygonGeo->SetExtRing(extRingLine);//先添加外圈点
					glbInt32 inRingCount = polygon3D->GetInRingCount();
					CGlbLine *inRingLine = NULL;
					for(glbInt32 i = 0; i < inRingCount; i++)
					{
						inRingLine = const_cast<CGlbLine *>(polygon3D->GetInRing(i));
						newPolygonGeo->AddInRing(inRingLine);
						if(inRingLine == NULL)
							continue;
						for (glbInt32 j = 0 ; j < inRingLine->GetCount() - 1; j++)
						{
							points = inRingLine->GetPoints();
							startPoint = osg::Vec3d(points[3 * j],points[3 * j + 1],points[3 * j + 2]);
							if(isGlobe)
								g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(startPoint.y()),
								osg::DegreesToRadians(startPoint.x()),startPoint.z(),startPoint.x(),startPoint.y(),startPoint.z());
							endPoint = osg::Vec3d(points[3 * (j + 1)],points[3 * (j + 1) + 1],points[3 * (j + 1) + 2]);
							if(isGlobe)
								g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(endPoint.y()),
								osg::DegreesToRadians(endPoint.x()),endPoint.z(),endPoint.x(),endPoint.y(),endPoint.z());
							osg::ref_ptr<osg::LineSegment> seg1 = new osg::LineSegment(startPoint,endPoint);
							osg::ref_ptr<osg::LineSegment> seg2 = new osg::LineSegment(start,end);
							osg::Vec3d outPoint;
							bool intersected = intersect3D_SegmentSegment(seg1,seg2,pixelWidth / 2/*0.03*/,outPoint);//鼠标与射线相交
							if(intersected == false)
								continue;

							newPolygonGeo->DeleteInRing(inRingLine);
							//更新编辑点
							if(mpr_polygonObj->GetEditNode()->getNumChildren() < 2)
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
							glbref_ptr<CGlbLine> newInRing = new CGlbLine(3,false);
							for (glbInt32 k = 0; k < inRingLine->GetCount();k++)
							{
								if(k == j + 1)
								{
									newInRing->AddPoint(outPoint.x(),outPoint.y(),outPoint.z());
									intersectIndex = k;
								}
								newInRing->AddPoint(points[3 * k],points[3 * k + 1],points[3 * k + 2]);
							}
							newPolygonGeo->AddInRing(newInRing.get());
							for(glbInt32 m = i + 1; m < inRingCount; m++)
							{
								inRingLine = const_cast<CGlbLine *>(polygon3D->GetInRing(m));
								if(inRingLine)
									newPolygonGeo->AddInRing(inRingLine);
							}
							glbBool isNULL = false;
							mpr_polygonObj->SetGeo(newPolygonGeo.get(),isNULL);
							mpr_isGeoChanged = true;
							mpr_polygonObj->DirectDraw(0);
							osg::Group *group = mpr_polygonObj->GetEditNode();
							if(group && moveMt)
							{
								group->removeChild(moveMt);
								moveMt = NULL;
							}
							isIntersectPoint = true;
							return false;
						}
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
				if(isIntersect && mpr_polygonObj->GetEditMode() == GLB_ROBJECTEDIT_HORIZONTAL)//DRAG下水平平移对象
				{
					CGlbGlobeView *view =  mpr_globe->GetView();
					osg::Vec3d start(ea.getX(),ea.getY(),0.0);
					osg::Vec3d end(ea.getX(),ea.getY(),1.0);
					view->ScreenToWorld(start.x(),start.y(),start.z());
					view->ScreenToWorld(end.x(),end.y(),end.z());

					glbref_ptr<CGlbExtent> extent = mpr_polygonObj->GetBound(true);
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

						CGlbPolygon *polygonGeo = mpr_polygonObj->GetGeo();
						CGlbLine *extRingLine = const_cast<CGlbLine *>(polygonGeo->GetExtRing());
						const glbDouble *points = extRingLine->GetPoints();
						glbref_ptr<CGlbPolygon> tempPolygonGeo = new CGlbPolygon(3,false);
						glbref_ptr<CGlbLine> outRing = new CGlbLine(3,false);
						for (glbInt32 i = 0 ; i < extRingLine->GetCount(); i++)
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
							outRing->AddPoint(point.x(),point.y(),points[3 * i + 2]);
							//outRing->AddPoint(point.x(),point.y(),point.z());
						}
						tempPolygonGeo->SetExtRing(outRing.get());

						glbInt32 inRingCount = polygonGeo->GetInRingCount();
						CGlbLine *inRingLine = NULL;
						for(glbInt32 i = 0; i < inRingCount; i++)
						{
							glbref_ptr<CGlbLine> newInRing = new CGlbLine(3,false);
							inRingLine = const_cast<CGlbLine *>(polygonGeo->GetInRing(i));
							if(inRingLine == NULL)
								continue;

							points = inRingLine->GetPoints();
							for (glbInt32 j = 0; j < inRingLine->GetCount(); j++)
							{
								osg::Vec3d point = osg::Vec3d(points[3 * j],points[3 * j +1],points[3 * j + 2]);
								if(isGlobe)
									g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(point.y()),
									osg::DegreesToRadians(point.x()),point.z(),point.x(),point.y(),point.z());
								point = point * trans;
								if(isGlobe)
								{
									g_ellipsoidModel->convertXYZToLatLongHeight(point.x(),point.y(),point.z(),point.y(),point.x(),point.z());
									point.x() = osg::RadiansToDegrees(point.x());
									point.y() = osg::RadiansToDegrees(point.y());
								}
								newInRing->AddPoint(point.x(),point.y(),points[3 * j + 2]);
							}

							tempPolygonGeo->AddInRing(newInRing.get());
						}

						glbBool isDirectDraw = false;
						mpr_polygonObj->SetGeo(tempPolygonGeo.get(),isDirectDraw);
						mpr_polygonObj->DirectDraw(0);
						intersectPoint = newPoint;
						//return false;
						return true;
					}
				}
				else if(isIntersect && mpr_polygonObj->GetEditMode() == GLB_ROBJECTEDIT_VERTICAL)//DRAG下竖直平移对象
				{
					osg::Vec3d center;
					CGlbPolygon *polygonGeo = mpr_polygonObj->GetGeo();
					if(!polygonGeo)
						return false;
					CGlbExtent *lineExt = const_cast<CGlbExtent *>(polygonGeo->GetExtent());
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

					CGlbLine *extRingLine = const_cast<CGlbLine *>(polygonGeo->GetExtRing());
					const glbDouble *points = extRingLine->GetPoints();
					glbref_ptr<CGlbPolygon> newPolygonGeo = new CGlbPolygon(3,false);
					glbref_ptr<CGlbLine> outRing = new CGlbLine(3,false);
					for (glbInt32 i = 0; i < extRingLine->GetCount(); i++)
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
						//outRing->AddPoint(point.x(),point.y(),point.z());
						outRing->AddPoint(points[3 * i],points[3 * i +1],point.z());
					}
					newPolygonGeo->SetExtRing(outRing.get());

					glbInt32 inRingCount = polygonGeo->GetInRingCount();
					CGlbLine *inRingLine = NULL;
					for(glbInt32 i = 0; i < inRingCount; i++)
					{
						glbref_ptr<CGlbLine> newInRing = new CGlbLine(3,false);
						inRingLine = const_cast<CGlbLine *>(polygonGeo->GetInRing(i));
						if(inRingLine == NULL)
							continue;

						points = inRingLine->GetPoints();
						for (glbInt32 j = 0; j < inRingLine->GetCount(); j++)
						{
							osg::Vec3d point = osg::Vec3d(points[3 * j],points[3 * j +1],points[3 * j + 2]);
							if(isGlobe)
								g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(point.y()),
								osg::DegreesToRadians(point.x()),point.z(),point.x(),point.y(),point.z());
							point = point * trans;
							if(isGlobe)
								g_ellipsoidModel->convertXYZToLatLongHeight(point.x(),point.y(),point.z(),point.y(),point.x(),point.z());
							//point.x() = osg::RadiansToDegrees(point.x());
							//point.y() = osg::RadiansToDegrees(point.y());
							//outRing->AddPoint(point.x(),point.y(),point.z());
							newInRing->AddPoint(points[3 * j],points[3 * j +1],point.z());
						}

						newPolygonGeo->AddInRing(newInRing.get());
					}

					glbBool isNULL = false;
					mpr_polygonObj->SetGeo(newPolygonGeo.get(),isNULL);
					mpr_polygonObj->DirectDraw(0);
					intersectPoint = outPoint;
					//return false;
					return true;
				}
				else if(isIntersect && isIntersectPoint && mpr_polygonObj->GetEditMode() == GLB_ROBJECTEDIT_SHAPE)//DRAG下编辑形态
				{
					if(isIntersectPoint)
					{
						CGlbPolygon *polygon3D = mpr_polygonObj->GetGeo();
						CGlbLine *extRingLine = const_cast<CGlbLine *>(polygon3D->GetExtRing());
						glbInt32 inRingCount = polygon3D->GetInRingCount();
						const glbDouble *points = extRingLine->GetPoints();
						CGlbGlobeView *view =  mpr_globe->GetView();
						osg::Vec3d start(ea.getX(),ea.getY(),0.0);
						osg::Vec3d end(ea.getX(),ea.getY(),1.0);
						view->ScreenToWorld(start.x(),start.y(),start.z());
						view->ScreenToWorld(end.x(),end.y(),end.z());

						glbref_ptr<CGlbExtent> extent = mpr_polygonObj->GetBound(true);
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
						updateOutRingline = new CGlbLine(3,false);
						updateInRingline = new CGlbLine(3,false);
						osg::ref_ptr<osg::Vec3dArray> bungeeVers = new osg::Vec3dArray;
						if(isInRing == false)
						{
							glbInt32 extRingVerCnt = extRingLine->GetCount();
							for(glbInt32 j = 0; j < extRingVerCnt; j++)
							{
								if(j == intersectIndex)
								{
									updateOutRingline->AddPoint(tempIntersect.x(),tempIntersect.y(),tempIntersect.z());
									////组建bungeeGeom顶点
									if(j == 0)
									{
										bungeeVers->push_back(osg::Vec3d(points[3 * (extRingVerCnt -  1)],points[3 * (extRingVerCnt - 1)+ 1],points[3 * (extRingVerCnt - 1) + 2]));
										bungeeVers->push_back(osg::Vec3d(tempIntersect.x(),tempIntersect.y(),tempIntersect.z()));
										bungeeVers->push_back(osg::Vec3d(points[3 * (j + 1)],points[3 * (j + 1)+ 1],points[3 * (j + 1) + 2]));
									}
									else if(j == extRingVerCnt - 1)
									{
										bungeeVers->push_back(osg::Vec3d(points[3 * (j -  1)],points[3 * (j - 1)+ 1],points[3 * (j - 1) + 2]));
										bungeeVers->push_back(osg::Vec3d(tempIntersect.x(),tempIntersect.y(),tempIntersect.z()));
										bungeeVers->push_back(osg::Vec3d(points[0],points[1],points[2]));
									}
									else
									{
										bungeeVers->push_back(osg::Vec3d(points[3 * (j -  1)],points[3 * (j - 1)+ 1],points[3 * (j - 1) + 2]));
										bungeeVers->push_back(osg::Vec3d(tempIntersect.x(),tempIntersect.y(),tempIntersect.z()));
										bungeeVers->push_back(osg::Vec3d(points[3 * (j + 1)],points[3 * (j + 1)+ 1],points[3 * (j + 1) + 2]));
									}
								}
								else
									updateOutRingline->AddPoint(points[3 * j],points[3 * j + 1],points[3 * j + 2]);
							}
						}
						else
						{
							CGlbLine *inRingLine = const_cast<CGlbLine *>(polygon3D->GetInRing(inRingIndex));
							glbInt32 inRingVerCnt = inRingLine->GetCount();
							points = inRingLine->GetPoints();
							for(glbInt32 j = 0; j < inRingVerCnt; j++)
							{
								if(j == intersectIndex)
								{
									updateInRingline->AddPoint(tempIntersect.x(),tempIntersect.y(),tempIntersect.z());
									////组建bungeeGeom顶点
									if(j == 0)
									{
										bungeeVers->push_back(osg::Vec3d(points[3 * (inRingVerCnt -  1)],points[3 * (inRingVerCnt - 1)+ 1],points[3 * (inRingVerCnt - 1) + 2]));
										bungeeVers->push_back(osg::Vec3d(tempIntersect.x(),tempIntersect.y(),tempIntersect.z()));
										bungeeVers->push_back(osg::Vec3d(points[3 * (j + 1)],points[3 * (j + 1)+ 1],points[3 * (j + 1) + 2]));
									}
									else if(j == inRingVerCnt - 1)
									{
										bungeeVers->push_back(osg::Vec3d(points[3 * (j -  1)],points[3 * (j - 1)+ 1],points[3 * (j - 1) + 2]));
										bungeeVers->push_back(osg::Vec3d(tempIntersect.x(),tempIntersect.y(),tempIntersect.z()));
										bungeeVers->push_back(osg::Vec3d(points[0],points[1],points[2]));
									}
									else
									{
										bungeeVers->push_back(osg::Vec3d(points[3 * (j -  1)],points[3 * (j - 1)+ 1],points[3 * (j - 1) + 2]));
										bungeeVers->push_back(osg::Vec3d(tempIntersect.x(),tempIntersect.y(),tempIntersect.z()));
										bungeeVers->push_back(osg::Vec3d(points[3 * (j + 1)],points[3 * (j + 1)+ 1],points[3 * (j + 1) + 2]));
									}
								}
								else
									updateInRingline->AddPoint(points[3 * j],points[3 * j + 1],points[3 * j + 2]);
							}
						}

						//删除老的
						osg::Group *editNode = mpr_polygonObj->GetEditNode();
						if(bungeeMt && editNode)
						{
							editNode->removeChild(bungeeMt);
							bungeeMt = NULL;
						}

						//创建橡皮筋节点
						osg::ref_ptr<osg::Geode> bungeeGeode = new osg::Geode;
						osg::ref_ptr<osg::Geometry> bungeeGeom = new osg::Geometry;
						osg::ref_ptr<osg::Vec3dArray> vertexes = new osg::Vec3dArray;
						osg::ref_ptr<osg::Vec4Array> color = new osg::Vec4Array;

						osg::Matrixd localToWorld;
						if(isGlobe)
							g_ellipsoidModel->computeLocalToWorldTransformFromLatLongHeight(
							osg::DegreesToRadians(points[1]),osg::DegreesToRadians(points[0]),points[2],localToWorld);
						else
							localToWorld.makeTranslate(points[0],points[1],points[2]);
						osg::Matrixd worldTolocal = osg::Matrixd::inverse(localToWorld);

						for (glbInt32 i = 0; i < bungeeVers->size(); i++)
						{
							osg::Vec3d temPoint(0.0,0.0,0.0);
							if(isGlobe)
								g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(bungeeVers->at(i).y()),
								osg::DegreesToRadians(bungeeVers->at(i).x()),bungeeVers->at(i).z(),temPoint.x(),temPoint.y(),temPoint.z());
							else
								temPoint = osg::Vec3d(bungeeVers->at(i).x(),bungeeVers->at(i).y(),bungeeVers->at(i).z());
							vertexes->push_back(temPoint * worldTolocal);
						}
						color->push_back(osg::Vec4(1.0,0.0,0.0,1.0));

						bungeeGeom->setVertexArray(vertexes);
						bungeeGeom->setColorArray(color);
						bungeeGeom->setColorBinding(osg::Geometry::BIND_OVERALL);
						bungeeGeom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINE_STRIP,0,vertexes->size()));
						osg::ref_ptr<osg::StateSet> stateset = bungeeGeode->getOrCreateStateSet();
						osg::ref_ptr<osg::LineWidth> lineWidth = new osg::LineWidth;
						lineWidth->setWidth(3);//需要根据像素修改大小,根据osg案例测试，这个设置的就是屏幕像素大小
						stateset->setAttributeAndModes(lineWidth,osg::StateAttribute::ON);
						stateset->setMode(GL_LIGHTING,osg::StateAttribute::OFF);
						//stateset->setMode(GL_DEPTH_TEST,osg::StateAttribute::ON);
						bungeeGeode->addDrawable(bungeeGeom);
						bungeeMt = new osg::MatrixTransform;
						bungeeMt->setMatrix(localToWorld);
						bungeeMt->addChild(bungeeGeode);
						if(editNode)
							editNode->addChild(bungeeMt);

						isNeedUpdate = true;
						return true;
					}
				}
			}
		case(osgGA::GUIEventAdapter::MOVE):
			{
				//没有与编辑点相交，所以当与线相交时要添加编辑球，不相交的时候移除编辑球
				if(isIntersectPoint == false && mpr_polygonObj->GetEditMode() == GLB_ROBJECTEDIT_SHAPE)
				{
					CGlbGlobeView *view =  mpr_globe->GetView();
					osg::Vec3d start(ea.getX(),ea.getY(),0.0);
					osg::Vec3d end(ea.getX(),ea.getY(),1.0);
					view->ScreenToWorld(start.x(),start.y(),start.z());
					view->ScreenToWorld(end.x(),end.y(),end.z());
					osg::Node *node = mpr_polygonObj->GetOsgNode();
					if(!node)
						return false;

					glbref_ptr<CGlbExtent> boundExtent = mpr_polygonObj->GetBound(true);
					glbDouble minX,minY,minZ,maxX,maxY,maxZ;
					boundExtent->GetMin(&minX,&minY,&minZ);
					boundExtent->GetMax(&maxX,&maxY,&maxZ);
					osg::BoundingBox bb(minX,minY,minZ,maxX,maxY,maxZ);
					glbBool tempIsIntersect = intersectRayBoundingBox(start,end,bb);
					if(tempIsIntersect == false)
						return false;

					CGlbPolygon *polygon3D = mpr_polygonObj->GetGeo();
					CGlbLine *extRingLine = const_cast<CGlbLine *>(polygon3D->GetExtRing());
					const glbDouble *points = extRingLine->GetPoints();

					//处理move与编辑点相交时，更改相交编辑点颜色
					isMoveIntersect = false;
					moveIntersectIndex = -1;
					isInRing = false;
					inRingIndex = -1;
					osg::Vec3d point;
					for (glbInt32 i = 0 ; i < extRingLine->GetCount() ; i++)
					{
						point = osg::Vec3d(points[3 * i],points[3 * i + 1],points[3 * i + 2]);
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

					if(isMoveIntersect == false)
					{
						glbInt32 inRingCount = polygon3D->GetInRingCount();
						for(glbInt32 i = 0; i < inRingCount; i++)
						{
							CGlbLine *inRingLine = const_cast<CGlbLine *>(polygon3D->GetInRing(i));
							if(inRingLine == NULL)
								continue;
							points = inRingLine->GetPoints();
							for (glbInt32 j = 0 ; j < inRingLine->GetCount() ; j++)
							{
								point = osg::Vec3d(points[3 * j],points[3 * j + 1],points[3 * j + 2]);
								if(isGlobe)
									g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(point.y()),
									osg::DegreesToRadians(point.x()),point.z(),point.x(),point.y(),point.z());
								osg::ref_ptr<osg::LineSegment> seg = new osg::LineSegment(start,end);
								bool intersected = intersect3D_SegmentPoint(seg,point,1.5 * pixelWidth/*0.1*/);
								if(intersected == false)
									continue;

								isInRing = true;
								inRingIndex = i;
								moveIntersectIndex = j;
								isMoveIntersect = true;
								break;
							}
						}
					}

					//处理删除
					if(moveMt)
					{
						osg::Group *editNode = mpr_polygonObj->GetEditNode();
						if(editNode)
						{
							glbBool isMove = editNode->removeChild(moveMt);
							moveMt = NULL;
						}
					}

					if(isMoveIntersect == false)//滑动与编辑点相交，不处理与线相交
					{
						//处理与编辑线相交
						osg::Vec3d startPoint,endPoint;
						points = extRingLine->GetPoints();
						//move与外圈相交创建点Geo
						for (glbInt32 i = 0 ; i < extRingLine->GetCount(); i++)
						{
							startPoint = osg::Vec3d(points[3 * i],points[3 * i + 1],points[3 * i + 2]);
							if(isGlobe)
								g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(startPoint.y()),
								osg::DegreesToRadians(startPoint.x()),startPoint.z(),startPoint.x(),startPoint.y(),startPoint.z());
							if(i == extRingLine->GetCount() - 1)
								endPoint = osg::Vec3d(points[0],points[1],points[2]);
							else
								endPoint = osg::Vec3d(points[3 * (i + 1)],points[3 * (i + 1) + 1],points[3 * (i + 1) + 2]);
							if(isGlobe)
								g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(endPoint.y()),
								osg::DegreesToRadians(endPoint.x()),endPoint.z(),endPoint.x(),endPoint.y(),endPoint.z());
							osg::ref_ptr<osg::LineSegment> seg1 = new osg::LineSegment(startPoint,endPoint);
							osg::ref_ptr<osg::LineSegment> seg2 = new osg::LineSegment(start,end);
							osg::Vec3d outPoint;
							bool intersected = intersect3D_SegmentSegment(seg1,seg2,pixelWidth / 2/*0.03*/,outPoint);
							if(intersected == false)
								continue;

							osg::ref_ptr<osg::Geode> pointGeode = new osg::Geode;
							osg::ref_ptr<osg::Geometry> pointGeom = new osg::Geometry;
							CGlbPolygon *polygon3D = mpr_polygonObj->GetGeo();
							osg::ref_ptr<osg::Vec3dArray> vertexes = new osg::Vec3dArray;
							osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array;
							CGlbLine *extRingLine = const_cast<CGlbLine *>(polygon3D->GetExtRing());
							const glbDouble *points = extRingLine->GetPoints();

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
							osg::Group *editNode = mpr_polygonObj->GetEditNode();
							if(editNode)
								editNode->addChild(moveMt);
							//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
							//return false;
							return true;
						}

						//move与内圈相交创建点Geo
						glbInt32 inRingCount = polygon3D->GetInRingCount();
						for(glbInt32 i = 0; i < inRingCount;i++)
						{
							CGlbLine *inRingLine = const_cast<CGlbLine *>(polygon3D->GetInRing(i));
							if(inRingLine == NULL)
								continue;
							points = inRingLine->GetPoints();
							for (glbInt32 j = 0 ; j < inRingLine->GetCount() ; j++)
							{
								startPoint = osg::Vec3d(points[3 * j],points[3 * j + 1],points[3 * j + 2]);
								if(isGlobe)
									g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(startPoint.y()),
									osg::DegreesToRadians(startPoint.x()),startPoint.z(),startPoint.x(),startPoint.y(),startPoint.z());
								if(j == inRingLine->GetCount() - 1)
									endPoint = osg::Vec3d(points[0],points[1],points[2]);
								else
									endPoint = osg::Vec3d(points[3 * (j + 1)],points[3 * (j + 1) + 1],points[3 * (j + 1) + 2]);
								if(isGlobe)
									g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(endPoint.y()),
									osg::DegreesToRadians(endPoint.x()),endPoint.z(),endPoint.x(),endPoint.y(),endPoint.z());
								osg::ref_ptr<osg::LineSegment> seg1 = new osg::LineSegment(startPoint,endPoint);
								osg::ref_ptr<osg::LineSegment> seg2 = new osg::LineSegment(start,end);
								osg::Vec3d outPoint;
								bool intersected = intersect3D_SegmentSegment(seg1,seg2,pixelWidth / 2/*0.03*/,outPoint);
								if(intersected == false)
									continue;

								osg::ref_ptr<osg::Geode> pointGeode = new osg::Geode;
								osg::ref_ptr<osg::Geometry> pointGeom = new osg::Geometry;
								CGlbPolygon *polygon3D = mpr_polygonObj->GetGeo();
								osg::ref_ptr<osg::Vec3dArray> vertexes = new osg::Vec3dArray;
								osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array;
								CGlbLine *extRingLine = const_cast<CGlbLine *>(polygon3D->GetExtRing());
								const glbDouble *extPoints = extRingLine->GetPoints();

								osg::Matrixd localToWorld;
								if(isGlobe)
									g_ellipsoidModel->computeLocalToWorldTransformFromLatLongHeight(
									osg::DegreesToRadians(extPoints[1]),osg::DegreesToRadians(extPoints[0]),extPoints[2],localToWorld);
								else
									localToWorld.makeTranslate(extPoints[0],extPoints[1],extPoints[2]);
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
								osg::Group *editNode = mpr_polygonObj->GetEditNode();
								if(editNode)
									editNode->addChild(moveMt);
								//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
								//return false;
								return true;
							}
						}
					}
				}
				return false;
			}
		case(osgGA::GUIEventAdapter::RELEASE):
			{
				//更新polygonObject
				if(isInRing == false && true == isNeedUpdate)
				{
					glbref_ptr<CGlbPolygon> polygonGeo = new CGlbPolygon(3,false);
					if(updateOutRingline)
					{
						polygonGeo->SetExtRing(updateOutRingline.get());
						glbBool isNull = false;
						CGlbPolygon *oldPolygon3D = mpr_polygonObj->GetGeo();
						for(glbInt32 i = 0; i < oldPolygon3D->GetInRingCount(); i++)
						{
							CGlbLine *inRingLine = const_cast<CGlbLine *>(oldPolygon3D->GetInRing(i));
							polygonGeo->AddInRing(inRingLine);
						}
						mpr_polygonObj->SetGeo(polygonGeo.get(),isNull);
						mpr_isGeoChanged = true;
						mpr_polygonObj->DirectDraw(0);
						osg::Group *group = mpr_polygonObj->GetEditNode();
						if(group && moveMt)
						{
							group->removeChild(moveMt);
							moveMt = NULL;
						}

						if(group && bungeeMt)
						{
							group->removeChild(bungeeMt);
							bungeeMt = NULL;
						}

						isNeedUpdate = false;
					}
				}
				else if(isInRing == true && true == isNeedUpdate)
				{
					glbref_ptr<CGlbPolygon> polygonGeo = new CGlbPolygon(3,false);
					CGlbPolygon *oldPolygon3D = mpr_polygonObj->GetGeo();
					CGlbLine *extRingLine = const_cast<CGlbLine *>(oldPolygon3D->GetExtRing());
					polygonGeo->SetExtRing(extRingLine);

					if(updateInRingline)
					{
						glbBool isNull = false;

						for(glbInt32 i = 0; i < oldPolygon3D->GetInRingCount(); i++)
						{
							CGlbLine *inRingLine = const_cast<CGlbLine *>(oldPolygon3D->GetInRing(i));
							if(i == inRingIndex)
								polygonGeo->AddInRing(updateInRingline.get());
							else
								polygonGeo->AddInRing(inRingLine);
						}

						mpr_polygonObj->SetGeo(polygonGeo.get(),isNull);
						mpr_isGeoChanged = true;
						mpr_polygonObj->DirectDraw(0);
						osg::Group *group = mpr_polygonObj->GetEditNode();
						if(group && moveMt)
						{
							group->removeChild(moveMt);
							moveMt = NULL;
						}

						if(group && bungeeMt)
						{
							group->removeChild(bungeeMt);
							bungeeMt = NULL;
						}

						isNeedUpdate = false;
					}
				}

				isIntersectPoint = false;
				intersectIndex = -1;
				isInRing = false;
				inRingIndex = -1;

				return false;
			}
		default:
			return false;//return false继续调用其他事件，return true不会再调用其他事件
		}
		return false;
	}
private:
	glbref_ptr<CGlbGlobe> mpr_globe;
	glbref_ptr<CGlbGlobePolygon> mpr_polygonObj;
	osg::ref_ptr<osg::Geode> mpr_boundGeode;
	glbref_ptr<CGlbExtent> mpr_boundExtent;
	osg::ref_ptr<osg::Geode> mpr_pointsGeode;
	glbBool mpr_isGeoChanged;
};

glbBool CGlbGlobePolygon::SetEdit(glbBool isEdit)
{
	if(mpt_isEdit == isEdit) return true;
	mpt_isEdit = isEdit;
	if(mpt_isEdit == true && mpr_editNode == NULL)
	{
		mpr_editNode = new osg::Group;
		osg::ref_ptr<PolygonEditEventHandler> editEventHandler = new PolygonEditEventHandler(this);
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

void CGlbGlobePolygon::DealEdit()
{
	/*osg::ref_ptr<osg::Node> node = g_editObject->GetOsgNode();
	if(node)
	{
	osg::ref_ptr<PointObjectEventHandler> poEventHandle = new PointObjectEventHandler(mpt_globe);
	node->setEventCallback(poEventHandle);
	}*/
}

glbBool CGlbGlobePolygon::DrawPoints( glbInt32 color )
{
	return false;
	throw;
}

glbBool CGlbGlobePolygon::SetCurrPoint( glbInt32 idx,glbInt32 color )
{
	return false;
	throw;
}

glbBool CGlbGlobePolygon::SetSelectedPoint( glbInt32 idx,glbInt32 color )
{
	return false;
	throw;
}

glbBool CGlbGlobePolygon::DrawRubber( glbDouble *points,glbInt32 pcnt,glbInt32 color )
{
	return false;
	throw;
}

glbBool CGlbGlobePolygon::DrawPrepareAddPoint( glbDouble xOrLon,glbDouble yOrLat,glbDouble zOrAlt,glbInt32 color )
{
	return false;
	throw;
}

glbBool CGlbGlobePolygon::ClearDraw()
{
	return false;
	throw;
}

glbInt32 CGlbGlobePolygon::ComputeNodeSize( osg::Node *node )
{
	CGlbCalculateNodeSizeVisitor cnsv;
	node->accept(cnsv);
	return cnsv.getTextureSize() + cnsv.getNodeMemSize();
}

void CGlbGlobePolygon::DirtyOnTerrainObject()
{
	if (!mpt_globe) return;
	CGlbExtent* cglbExtent = GetBound(false).get();	
	if (cglbExtent)
		mpt_globe->AddDomDirtyExtent(*cglbExtent);
}

void CGlbGlobePolygon::RemoveNodeFromScene(glbBool isClean)
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
//////////////////////////////////////////////////////////////////////////
bool CGlbGlobePolygon::judgePolygon2DNeedDirectDraw( GlbPolygon2DSymbolInfo *polygon2DSmblInfo, bool &isNeedDirectDraw )
{
	if (mpt_altitudeMode == GLB_ALTITUDEMODE_ONTERRAIN &&polygon2DSmblInfo->symType == GLB_POLYGONSYMBOL_2D)//贴地模式 直接重绘
	{
		isNeedDirectDraw=true;
		return true;
	} 
	glbFloat texRotation=mpr_TexRotation;
	if (polygon2DSmblInfo->textureRotation)
		texRotation=polygon2DSmblInfo->textureRotation->value;
	if (!isFloatEqual(texRotation,mpr_TexRotation))
	{
		isNeedDirectDraw=true;//重新计算纹理坐标
		return true;
	}
	if (mpt_node==NULL)
		return false;
	GlbPolygonSymbolInfo *polygonInfo = static_cast<GlbPolygonSymbolInfo*>(mpr_renderInfo.get());
	GlbPolygon2DSymbolInfo *polygon2DInfo= dynamic_cast<GlbPolygon2DSymbolInfo *>(polygonInfo);
	glbBool isDrawEdgeWall = polygon2DSmblInfo->isEdgeVerticalExtend->GetValue(mpt_feature.get());
	glbBool isWallDirReserve = polygon2DSmblInfo->isExtendDirReserve->GetValue(mpt_feature.get());
	if (isDrawEdgeWall)
	{
		glbBool isDirReserveDirty = CGlbCompareValue::CompareValueBoolEqual(polygon2DSmblInfo->isExtendDirReserve,polygon2DInfo->isExtendDirReserve,mpt_feature.get());
		glbBool isDepthDirty = CGlbCompareValue::CompareValueDoubleEqual(polygon2DSmblInfo->extendDepth,polygon2DInfo->extendDepth,mpt_feature.get());
		if (!isDirReserveDirty || !isDepthDirty)
		{
			isNeedDirectDraw=true;
			return true;
		}
	}
	glbDouble zOffset = 0.0;
	if (polygon2DSmblInfo->zOrAltitude)
		zOffset = polygon2DSmblInfo->zOrAltitude->GetValue(mpt_feature.get());
	if (fabs(zOffset) > 0.001)
	{
		const CGlbExtent* pExtent = NULL;
		if (mpt_feature != NULL)
		{
			IGlbGeometry* geo = NULL;
			glbBool result = false;
			result = mpt_feature->GetGeometry(&geo,0);
			if (geo != NULL)
				pExtent = geo->GetExtent();
		}
		else if (mpr_polygonGeo != NULL)
			pExtent = mpr_polygonGeo->GetExtent();
		if (pExtent != NULL)
		{
			osg::Matrixd offsetZ = osg::Matrixd::identity();
			offsetZ.makeTranslate(0,0,zOffset);

			glbDouble cetX,cetY,cetZ;
			pExtent->GetCenter(&cetX,&cetY,&cetZ);
			osg::Matrixd polygonMT;
			osg::Group *pContainer=mpt_node->asSwitch()->getChild(0)->asGroup();
			unsigned int childNum=pContainer->getNumChildren();
			for (unsigned int childId=0;childId<childNum;++childId)
			{
				osg::Group *pGroupTop=pContainer->getChild(childId)->asGroup();
				osg::MatrixTransform *pTransform=dynamic_cast<osg::MatrixTransform *>(pGroupTop->getChild(0)->asTransform());
				
				if(mpt_globe->GetType() == GLB_GLOBETYPE_GLOBE)
					g_ellipsoidModel->computeLocalToWorldTransformFromLatLongHeight(
						osg::DegreesToRadians(cetY),osg::DegreesToRadians(cetX),cetZ,polygonMT);										
				else				
					polygonMT.makeTranslate(cetX,cetY,cetZ);			

				pTransform->setMatrix(polygonMT*offsetZ);
			}		
		}		
	}
	
	isNeedDirectDraw=false;

	if (mpt_feature!=NULL)//fea
	{
		osg::Group *pContainer=mpt_node->asSwitch()->getChild(0)->asGroup();
		unsigned int childNum=pContainer->getNumChildren();
		for (unsigned int childId=0;childId<childNum;++childId)
		{
			osg::Group *pGroupTop=pContainer->getChild(childId)->asGroup();
			osg::Transform *pTransform=pGroupTop->getChild(0)->asTransform();
			osg::Switch *pSW=pTransform->getChild(0)->asSwitch();
			osg::Geode *pGeode=pSW->getChild(0)->asGeode();
			unsigned int childCnt=pGeode->getNumDrawables();
			for (unsigned int i=0;i<childCnt;++i)
			{
				osg::Drawable *pDrawable=pGeode->getDrawable(i);
				osg::StateSet *sset = pDrawable->getOrCreateStateSet();
				updatePolygon2DSymbolInfo(sset,polygon2DSmblInfo);
			}

			osg::Switch *pSW_outline=pTransform->getChild(1)->asSwitch();
			osg::Geode *pGeode_outline=pSW_outline->getChild(0)->asGeode();
			childCnt=pGeode_outline->getNumDrawables();
			for(unsigned int i=0;i<childCnt;++i)
			{
				osg::Drawable *pDrawable_outline=pGeode_outline->getDrawable(i);
				osg::StateSet *sset_outline = pDrawable_outline->getOrCreateStateSet();
				updatePolygon2DOutlineSymbolInfo(sset_outline,polygon2DSmblInfo);
			}
			if (pTransform->getNumChildren()>2)
			{
				osg::Switch *pSW_edgewall=pTransform->getChild(2)->asSwitch();
				if (isDrawEdgeWall)
					pSW_edgewall->setAllChildrenOn();
				else
					pSW_edgewall->setAllChildrenOff();
			}			
		}
	}
	else if (mpr_polygonGeo!=NULL)// single polygon2d
	{
		/*osg::Group *pContainer=mpt_node->asSwitch()->getChild(0)->asGroup();
		osg::Group *pGroupTop=pContainer->getChild(0)->asGroup();*/
		osg::Group *pGroupTop=mpt_node->asSwitch()->getChild(0)->asGroup()->getChild(0)->asGroup();
		osg::Transform *pTransform=pGroupTop->getChild(0)->asTransform();
		osg::Switch *pSW=pTransform->getChild(0)->asSwitch();
		osg::Geode *pGeode=pSW->getChild(0)->asGeode();
		osg::Drawable *pDrawable=pGeode->getDrawable(0);
		osg::StateSet *sset = pDrawable->getOrCreateStateSet();
		updatePolygon2DSymbolInfo(sset,polygon2DSmblInfo);
		osg::Switch *pSW_outline=pTransform->getChild(1)->asSwitch();
		osg::Geode *pGeode_outline=pSW_outline->getChild(0)->asGeode();
		osg::Drawable *pDrawable_outline=pGeode_outline->getDrawable(0);
		osg::StateSet *sset_outline = pDrawable_outline->getOrCreateStateSet();
		updatePolygon2DOutlineSymbolInfo(sset_outline,polygon2DSmblInfo);

		if (pTransform->getNumChildren()>2)
		{
			osg::Switch *pSW_edgewall=pTransform->getChild(2)->asSwitch();
			if (isDrawEdgeWall)
				pSW_edgewall->setAllChildrenOn();
			else
				pSW_edgewall->setAllChildrenOff();
		}			
	}
	return true;

	//// 透明度
	//glbInt32 polygonOpacity=100;
	//if (polygon2DSmblInfo->opacity)
	//{
	//	polygonOpacity=polygon2DSmblInfo->opacity->value;
	//}
	//if (polygonOpacity!=100)//透明
	//	sset->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
	//else
	//	sset->setRenderingHint(osg::StateSet::OPAQUE_BIN);
	//sset->getUniform("PolygonOpacity")->set(polygonOpacity);//

	////2、纹理贴图/或者颜色
	//osg::Vec4f polygonColor(0.0,255.0,0.0,1.0);
	//if (polygon2DSmblInfo->color)
	//{
	//	glbInt32 plyClr=polygon2DSmblInfo->color->value;
	//	polygonColor.set(GetRValueInBGRA(plyClr),GetGValueInBGRA(plyClr),GetBValueInBGRA(plyClr),1.0);
	//}
	//sset->getUniform("PolygonColor")->set(polygonColor);
	//bool buseTexColor=false;
	//if (polygon2DSmblInfo->textureData)
	//{
	//	CGlbWString imageMapFilePath=polygon2DSmblInfo->textureData->value;
	//	/*osg::Texture2D* imageMap_tex = readTexture(imageMapFilePath, osg::Texture::REPEAT);
	//	if (imageMap_tex!=NULL)
	//	{
	//	sset->setTextureAttributeAndModes(0, imageMap_tex, osg::StateAttribute::ON );
	//	buseTexColor=true;
	//	}*/

	//	osg::Texture2D *tex2D = 
	//		dynamic_cast<osg::Texture2D *>(sset->getTextureAttribute(0,osg::StateAttribute::TEXTURE));

	//	osg::Image *pImg=loadImage(imageMapFilePath);
	//	if (pImg!=NULL)
	//	{
	//		tex2D->setImage(pImg);
	//		tex2D->dirtyTextureObject();
	//		buseTexColor=true;
	//	}			
	//}
	//sset->getUniform("PolygonUseTex")->set(buseTexColor);

	////3、纹理模式
	//float tilingU=1.0;
	//if (polygon2DSmblInfo->tilingU)
	//{
	//	glbInt32 u=polygon2DSmblInfo->tilingU->value;
	//	tilingU=u;
	//}
	//float tilingV=1.0;
	//if (polygon2DSmblInfo->tilingV)
	//{
	//	glbInt32 v=polygon2DSmblInfo->tilingV->value;
	//	tilingV=v;
	//}
	//int texRepeatMode=0;
	//if (polygon2DSmblInfo->texRepeatMode)
	//{
	//	if (polygon2DSmblInfo->texRepeatMode->value==GLB_TEXTUREREPEAT_TIMES)
	//		texRepeatMode=0;
	//	else if(polygon2DSmblInfo->texRepeatMode->value==GLB_TEXTUREREPEAT_SIZE)
	//		texRepeatMode=1;
	//}
	//sset->getUniform("tilingU")->set(tilingU);
	//sset->getUniform("tilingV")->set(tilingV);
	//sset->getUniform("PolygonTexRepeatMode")->set(texRepeatMode);

	//return true;
}
void CGlbGlobePolygon::updatePolygon2DSymbolInfo( osg::StateSet *sset,GlbPolygon2DSymbolInfo *polygon2DSmblInfo )
{
	//2、纹理贴图/或者颜色
	osg::Vec4f polygonColor(0.0,255.0,0.0,1.0);
	if (polygon2DSmblInfo->color)
	{
		glbInt32 plyClr=polygon2DSmblInfo->color->GetValue(mpt_feature.get());
		polygonColor.set(GetRValueInBGRA(plyClr),LOBYTE(((plyClr & 0xFFFF)) >> 8),GetBValueInBGRA(plyClr),1.0);
	}
	sset->getUniform("PolygonColor")->set(polygonColor);
	bool buseTexColor=false;
	if (polygon2DSmblInfo->textureData)
	{
		CGlbWString imageMapFilePath=polygon2DSmblInfo->textureData->value;
		/*osg::Texture2D* imageMap_tex = readTexture(imageMapFilePath, osg::Texture::REPEAT);
		if (imageMap_tex!=NULL)
		{
		sset->setTextureAttributeAndModes(0, imageMap_tex, osg::StateAttribute::ON );
		buseTexColor=true;
		}*/

		osg::Texture2D *tex2D = 
			dynamic_cast<osg::Texture2D *>(sset->getTextureAttribute(0,osg::StateAttribute::TEXTURE));

		glbInt32 index = imageMapFilePath.find_first_of(L'.');
		if(index == 0)
		{
			CGlbWString execDir = CGlbPath::GetExecDir();
			imageMapFilePath = execDir + imageMapFilePath.substr(1,imageMapFilePath.size());
		}
		osg::Image *pImg=loadImage(imageMapFilePath);
		if (pImg!=NULL)
		{
			tex2D->setImage(pImg);
			tex2D->dirtyTextureObject();
			buseTexColor=true;
		}			
	}
	sset->getUniform("PolygonUseTex")->set(buseTexColor);

	//3、纹理模式
	float tilingU=1.0;
	if (polygon2DSmblInfo->tilingU)
	{
		glbInt32 u=polygon2DSmblInfo->tilingU->value;
		tilingU=u;
	}
	float tilingV=1.0;
	if (polygon2DSmblInfo->tilingV)
	{
		glbInt32 v=polygon2DSmblInfo->tilingV->value;
		tilingV=v;
	}
	int texRepeatMode=0;
	if (polygon2DSmblInfo->texRepeatMode)
	{
		if (polygon2DSmblInfo->texRepeatMode->value==GLB_TEXTUREREPEAT_TIMES)
			texRepeatMode=0;
		else if(polygon2DSmblInfo->texRepeatMode->value==GLB_TEXTUREREPEAT_SIZE)
			texRepeatMode=1;
	}
	sset->getUniform("tilingU")->set(tilingU);
	sset->getUniform("tilingV")->set(tilingV);
	sset->getUniform("PolygonTexRepeatMode")->set(texRepeatMode);

	// 透明度
	glbInt32 polygonOpacity=100;
	if (polygon2DSmblInfo->opacity)
	{
		polygonOpacity=polygon2DSmblInfo->opacity->value;
	}
	osg::ref_ptr<osg::Depth> depth = NULL;
	if (polygonOpacity!=100)//透明
	{
		sset->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
		sset->setMode(GL_BLEND,osg::StateAttribute::ON);
		depth = dynamic_cast<osg::Depth *>(sset->getAttribute(osg::StateAttribute::DEPTH));
		if(!depth.valid())
		{
			depth = new osg::Depth;
			depth->setWriteMask(false);
			sset->setAttributeAndModes(depth.get(), osg::StateAttribute::ON);
		}
		else
		{
			if(depth->getWriteMask())
				depth->setWriteMask(false);
		}
	}
	else
	{
		sset->setRenderingHint(osg::StateSet::OPAQUE_BIN);
		sset->setMode(GL_BLEND,osg::StateAttribute::OFF);
		depth = dynamic_cast<osg::Depth *>(sset->getAttribute(osg::StateAttribute::DEPTH));
		if(!depth.valid())
		{
			depth = new osg::Depth;
			depth->setWriteMask(true);
			sset->setAttributeAndModes(depth.get(), osg::StateAttribute::ON);
		}
		else
		{
			if(!depth->getWriteMask())
				depth->setWriteMask(true);
		}
	}
	sset->getUniform("PolygonOpacity")->set(polygonOpacity);//
}
void CGlbGlobePolygon::updatePolygon2DOutlineSymbolInfo( osg::StateSet *sset,GlbPolygon2DSymbolInfo *polygon2DSmblInfo )
{
	//polygon2DSmblInfo!=NULL
	GlbLinePixelSymbolInfo *pOutline=polygon2DSmblInfo->outlineInfo;
	// 透明度
	glbInt32 polygonOutlineOpacity=100;
	if (pOutline!=NULL&&pOutline->lineOpacity)
	{
		polygonOutlineOpacity=pOutline->lineOpacity->value;
	}
	if (polygonOutlineOpacity!=100)//透明 
		sset->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
	else
		sset->setRenderingHint(osg::StateSet::OPAQUE_BIN);
	sset->getUniform("PolygonOutlineOpacity")->set(polygonOutlineOpacity);//

	//2、颜色
	osg::Vec4f polygonOutlineColor(0.0,255.0,0.0,255.0);
	if (pOutline!=NULL&&pOutline->lineColor)
	{
		glbInt32 plyClr=pOutline->lineColor->value;
		polygonOutlineColor.set(GetRValueInBGRA(plyClr),LOBYTE(((plyClr & 0xFFFF)) >> 8),GetBValueInBGRA(plyClr),255.0);
	}
	sset->getUniform("PolygonOutlineColor")->set(polygonOutlineColor);

	//3、线宽
	glbDouble plyOutlineW=1.0;
	if (pOutline!=NULL&&pOutline->lineWidth)
		plyOutlineW=pOutline->lineWidth->value;
	if (plyOutlineW<=0.0)
		plyOutlineW=1.0;
	osg::LineWidth *lineWidth = 
		dynamic_cast<osg::LineWidth *>(sset->getAttribute(osg::StateAttribute::LINEWIDTH));
	if(lineWidth!=NULL)
		lineWidth->setWidth((float)plyOutlineW);

	// 4、线样式
	bool solidLine=true;
	if (pOutline!=NULL&&pOutline->linePattern!=NULL)
	{
		if (pOutline->linePattern!=NULL)
			solidLine=pOutline->linePattern->value==GLB_LINE_SOLID;
	}
	osg::LineStipple *lineStipple = 
		dynamic_cast<osg::LineStipple *>(sset->getAttribute(osg::StateAttribute::LINESTIPPLE));
	if (lineStipple!=NULL)
	{
		if (solidLine)
			sset->setAttributeAndModes(lineStipple,osg::StateAttribute::OFF);
		else
			sset->setAttributeAndModes(lineStipple,osg::StateAttribute::ON);
	}
}
///
bool CGlbGlobePolygon::judgePolyhedronNeedDirectDraw( GlbPolyhedronSymbolInfo *polyhedronSmblInfo, bool &isNeedDirectDraw )
{
	isNeedDirectDraw=true;
	return true;
}
///
bool CGlbGlobePolygon::judgePolygonBuildingNeedDirectDraw(GlbPolygonBuildingSymbolInfo *polyhedronSmblInfo, bool &isNeedDirectDraw)
{
	isNeedDirectDraw=true;
	return true;
}
///
bool CGlbGlobePolygon::judgePolygonWaterNeedDirectDraw( GlbPolygonWaterSymbolInfo *polygonWaterSmblInfo, bool &isNeedDirectDraw )
{
	glbFloat texRotation=mpr_TexRotation;
	if (polygonWaterSmblInfo->refractTexRotation)
		texRotation=polygonWaterSmblInfo->refractTexRotation->value;
	if (!isFloatEqual(texRotation,mpr_TexRotation))
	{
		isNeedDirectDraw=true;//重新计算纹理坐标
		return true;
	}

	if (mpt_node==NULL)
		return false;
	osg::Group *pGroupTop=mpt_node->asSwitch()->getChild(0)->asGroup()->getChild(0)->asGroup();// is CWaterScene
	IWaterScene *pWaterScene=dynamic_cast<IWaterScene *>(mpt_node->asSwitch()->getChild(0));
	osg::Transform *pTransform=pGroupTop->asTransform();
	osg::Switch *pSW=pTransform->getChild(0)->asSwitch();
	osg::Geode *pGeode=pSW->getChild(0)->asGeode();
	osg::Drawable *pDrawable=pGeode->getDrawable(0);
	osg::StateSet *sset = pDrawable->getOrCreateStateSet();


	// 透明度
	glbInt32 polygonOpacity=100;
	if (polygonWaterSmblInfo->opacity)
	{
		polygonOpacity=polygonWaterSmblInfo->opacity->value;
	}
	if (polygonOpacity!=100)//透明
		sset->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
	else
		sset->setRenderingHint(osg::StateSet::OPAQUE_BIN);
	sset->getUniform("PolygonOpacity")->set(polygonOpacity);//

	// 水面对边形 颜色
	osg::Vec4f polygonColor(0.0,255.0,0.0,1.0);
	if (polygonWaterSmblInfo->color)
	{
		glbInt32 plyClr=polygonWaterSmblInfo->color->value;
		polygonColor.set(GetRValueInBGRA(plyClr),LOBYTE(((plyClr & 0xFFFF)) >> 8),GetBValueInBGRA(plyClr),1.0);
	}
	sset->getUniform("PolygonColor")->set(polygonColor);
	bool buseTexColor=false;
	if (polygonWaterSmblInfo->refractionImageData)// 折射纹理贴图
	{
		CGlbWString imageMapFilePath=polygonWaterSmblInfo->refractionImageData->value;
		osg::Texture2D *tex2D = 
			dynamic_cast<osg::Texture2D *>(sset->getTextureAttribute(3,osg::StateAttribute::TEXTURE));
		glbInt32 index = imageMapFilePath.find_first_of(L'.');
		if(index == 0)
		{
			CGlbWString execDir = CGlbPath::GetExecDir();
			imageMapFilePath = execDir + imageMapFilePath.substr(1,imageMapFilePath.size());
		}
		osg::Image *pImg=loadImage(imageMapFilePath);
		if (pImg!=NULL)
		{
			tex2D->setImage(pImg);
			tex2D->dirtyTextureObject();
			buseTexColor=true;
		}
	}
	sset->getUniform("PolygonUseTex")->set(buseTexColor);

	bool enableReflect=false;
	if (polygonWaterSmblInfo->enableReflections)
	{
		enableReflect=polygonWaterSmblInfo->enableReflections->value;
	}
	sset->getUniform("waterPolygon_EnableReflections")->set( enableReflect);
	pWaterScene->enableReflections(enableReflect);

	bool enableRefract=false;
	if (polygonWaterSmblInfo->enableRefractions)
	{
		enableRefract=polygonWaterSmblInfo->enableRefractions->value;
	}
	sset->getUniform("waterPolygon_EnableRefractions")->set( enableRefract);
	pWaterScene->enableRefractions(enableRefract);


	// 风速、风向
	float windSpeed=1.0f;
	if (polygonWaterSmblInfo->windStrong)
	{
		windSpeed=polygonWaterSmblInfo->windStrong->value;
	}
	sset->getUniform("windSpeed")->set(windSpeed);
	windSpeed=0.0f;
	if (polygonWaterSmblInfo->windDirection)
	{
		windSpeed=polygonWaterSmblInfo->windDirection->value;
	}
	sset->getUniform("windDirection")->set(windSpeed);


	isNeedDirectDraw=false;
	return true;
}

CGlbPolygon * GlbGlobe::CGlbGlobePolygon::RemoveRepeatePoints( CGlbPolygon *polygon )
{
	if(polygon == NULL)
		return NULL;

	CGlbLine *pExtRing = const_cast<CGlbLine *>(polygon->GetExtRing());
	if(pExtRing == NULL || pExtRing->IsEmpty())
		return NULL;

	CGlbPolygon *newPolygon = new CGlbPolygon(3);
	CGlbLine *extLine = new CGlbLine(3);
	extLine = RemoveRepeatePointsByLine(pExtRing);
	if(extLine == NULL) return NULL;

	for(glbInt32 i = 0; i < polygon->GetInRingCount(); i++)
	{
		CGlbLine *inRing = const_cast<CGlbLine *>(polygon->GetInRing(i));
		inRing = RemoveRepeatePointsByLine(inRing);
		if(inRing == NULL)
			continue;

		newPolygon->AddInRing(inRing);
	}

	newPolygon->SetExtRing(extLine);

	return newPolygon;
}

CGlbLine * GlbGlobe::CGlbGlobePolygon::RemoveRepeatePointsByLine( CGlbLine *line )
{
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
	//osg::Vec3dArray *tempPoints = new osg::Vec3dArray;
	//tempPoints->push_back(v);
	CGlbLine *extLine = new CGlbLine(3);
	extLine->AddPoint(v.x(),v.y(),v.z());
	for(; p != points->end();p++)
	{
		if(v[0] == (*p)[0] && v[1] == (*p)[1] && v[2] == (*p)[2])
			continue;

		v = *p;
		//tempPoints->push_back(v);
		extLine->AddPoint(v.x(),v.y(),v.z());
	}

	osg::Vec3d beginPoint = points->at(0);
	osg::Vec3d endPoint = points->at(points->size() - 1);
	if(isDoubleEqual(beginPoint.x(),endPoint.x()) && isDoubleEqual(beginPoint.y(),endPoint.y()) && isDoubleEqual(beginPoint.z(),endPoint.z()))
		extLine->DeletePoint(extLine->GetCount() - 1);//删除最后一个点，因为与第一个点相等

	return extLine;
}

void GlbGlobe::CGlbGlobePolygon::ParseObjectFadeColor()
{
	if(!mpr_renderInfo.valid()) return;
	GlbPolygon2DSymbolInfo *polygonInfo = dynamic_cast<GlbPolygon2DSymbolInfo *>(mpr_renderInfo.get());	
	if(polygonInfo == NULL) return;
	switch(polygonInfo->symType)
	{
	case GLB_POLYGONSYMBOL_2D:
		{
			GlbPolygon2DSymbolInfo *polygon2DInfo= dynamic_cast<GlbPolygon2DSymbolInfo *>(polygonInfo);
			if (polygon2DInfo != NULL)
				mpt_fadeColor = GetColor(polygon2DInfo->color->GetValue(mpt_feature.get()));
		}
		break;
	case GLB_POLYGONSYMBOL_POLYHEDRON:
		{
			GlbPolyhedronSymbolInfo *polyhedronInfo = dynamic_cast<GlbPolyhedronSymbolInfo *>(polygonInfo);
			if(polyhedronInfo != NULL) 
			{// 未写完，待实现 ！！！！！
			}
		}
		break;
	case GLB_POLYGONSYMBOL_BUILDING:
		{
			GlbPolygonBuildingSymbolInfo *polyBuildingInfo = dynamic_cast<GlbPolygonBuildingSymbolInfo *>(polygonInfo);
			if(polyBuildingInfo != NULL)
			{// 未写完，待实现 ！！！！！
			}
		}
		break;
	case GLB_POLYGONSYMBOL_WATER:
		{
			GlbPolygonWaterSymbolInfo *polygonWaterInfo = dynamic_cast<GlbPolygonWaterSymbolInfo *>(polygonInfo);
			if(polygonWaterInfo != NULL)
			{// 未写完，待实现 ！！！！！
			}
		}
		break;
	default:
		break;
	}	
}

glbBool GlbGlobe::CGlbGlobePolygon::Load2( xmlNodePtr *node,const glbWChar* prjPath )
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

	if(symType == "NGLB_POLYGONSYMBOL_2D")
	{
		GlbPolygon2DSymbolInfo* polygon2DInfo = new GlbPolygon2DSymbolInfo();
		pnode = pnode->next;//text;
		polygon2DInfo->Load2(pnode,prjPath);
		mpr_renderInfo = (GlbRenderInfo*)polygon2DInfo;
	}
	else if(symType == "NGLB_POLYGONSYMBOL_POLYHEDRON")
	{
		GlbPolyhedronSymbolInfo* polyhedronInfo = new GlbPolyhedronSymbolInfo();
		pnode = pnode->next;//text;
		polyhedronInfo->Load2(pnode,prjPath);
		mpr_renderInfo = (GlbRenderInfo*)polyhedronInfo;
	}
	else if(symType == "NGLB_POLYGONSYMBOL_BUILDING")
	{
		GlbPolygonBuildingSymbolInfo* polygonBuildingInfo = new GlbPolygonBuildingSymbolInfo();
		pnode = pnode->next;//text;
		polygonBuildingInfo->Load2(pnode,prjPath);
		mpr_renderInfo = (GlbRenderInfo*)polygonBuildingInfo;
	}
	else if(symType == "NGLB_POLYGONSYMBOL_WATER")
	{
		GlbPolygonWaterSymbolInfo* waterInfo = new GlbPolygonWaterSymbolInfo();
		pnode = pnode->next;//text;
		waterInfo->Load2(pnode,prjPath);
		mpr_renderInfo = (GlbRenderInfo*)waterInfo;
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
		szKey = xmlNodeGetContent(childNode->xmlChildrenNode);	
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
	groupNode = childNode;
	childNode = childNode->xmlChildrenNode;//text
	childNode = childNode->next;//coordDimension
	if (childNode->xmlChildrenNode && (!xmlStrcmp(childNode->xmlChildrenNode->name, (const xmlChar *)"text")))
	{
		szKey = xmlNodeGetContent(childNode->xmlChildrenNode);	
		sscanf_s((char*)szKey,"%d",&coordDimension);
		xmlFree(szKey);
	}
	childNode = childNode->next;//text
	childNode = childNode->next;//hasM
	if (childNode)
	{
		szKey = xmlNodeGetContent(childNode->xmlChildrenNode);	
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
	mpr_polygonGeo = new CGlbPolygon(3,false);
	glbref_ptr<CGlbLine> outRing = new CGlbLine(3,false);
	glbDouble tempX,tempY,tempZ;
	while(!xmlStrcmp(childNode->name, (const xmlChar *)"tagPoint3"))//组建ExtRing
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
		outRing->AddPoint(tempX,tempY,tempZ);
		childNode = childNode->parent;
		childNode = childNode->next;//text
		childNode = childNode->next;
		if(!childNode)
			break;
	}
	mpr_polygonGeo->SetExtRing(outRing.get());

	childNode = groupNode->next;//text
	childNode = childNode->next;//InRings
	if(childNode->xmlChildrenNode == NULL)
		return true;
	childNode = childNode->xmlChildrenNode;//text
	childNode = childNode->next;//SerialLine
	groupNode = childNode;//记录SerialLine
	while(!xmlStrcmp(childNode->name, (const xmlChar *)"SerialLine"))
	{
		childNode = childNode->xmlChildrenNode;//text
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
			szKey = xmlNodeGetContent(childNode->xmlChildrenNode);	
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
		glbref_ptr<CGlbLine> inRing = new CGlbLine(3,false);
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
			inRing->AddPoint(tempX,tempY,tempZ);
			childNode = childNode->parent;
			childNode = childNode->next;//text
			childNode = childNode->next;
			if(!childNode)
				break;
		}
		mpr_polygonGeo->AddInRing(inRing.get());

		childNode = groupNode->next;//text
		childNode = childNode->next;//SerialLine
		if(!childNode)
			break;
	}

	return true;
}

bool GlbGlobe::CGlbGlobePolygon::judgePolygonLakeNeedDirectDraw( GlbPolygonLakeSymbolInfo *polygonLakeSmblInfo, bool &isNeedDirectDraw )
{
	isNeedDirectDraw=true;
	return true;
}