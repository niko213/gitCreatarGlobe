#include "StdAfx.h"
#include "GlbGlobeNetworkSymbol.h"
#include "osgDB/ReadFile"
#include "GlbString.h"
//#include "GlbGlobeNetworkObject.h"
#include "GlbLine.h"
#include "GlbPoint.h"
#include "CGlbGlobe.h"
#include <osg/Switch>
#include <osg/LineWidth>
#include <osg/BlendColor>
#include <osg/BlendFunc>
#include <osg/Material>
#include <osg/Point>
#include "IGlbDataEngine.h"
#include "GlbLog.h"
#include <osg/LightModel>

using namespace GlbGlobe;

CGlbGlobeNetworkSymbol::CGlbGlobeNetworkSymbol(void)
{
}


CGlbGlobeNetworkSymbol::~CGlbGlobeNetworkSymbol(void)
{
}

osg::Node *CGlbGlobeNetworkSymbol::Draw(CGlbGlobeRObject *obj,IGlbGeometry *geo)
{
	if (obj==NULL)
		return NULL;
	GlbRenderInfo *renderInfo = obj->GetRenderInfo();
	GlbNetworkRenderInfo *networkRenderInfo = 
		dynamic_cast<GlbNetworkRenderInfo *>(renderInfo);
	if(NULL == networkRenderInfo)
		return NULL;	

	osg::ref_ptr<osg::Switch> swNode = new osg::Switch;

	CGlbFeature *feature = obj->GetFeature();	
	if (feature==NULL || feature->GetExtent()==NULL)
	{
		OutputDebugString(L"Error! network feature not exist or extent empty! ");
		return NULL;
	}

	glbInt32 edgeOId = feature->GetOid();
	char buff[32];
	sprintf_s(buff,"network_edge%d",edgeOId);

	glbDouble cx,cy,cz;
#if 1
	if (feature->GetClass())
	{
		IGlbFeatureClass* fc= const_cast<IGlbFeatureClass*>(feature->GetClass());
		fc->GetExtent()->GetCenter(&cx,&cy,&cz);
	}
	else
	{
		//feature->GetExtent()->GetCenter(&cx,&cy,&cz);
		OutputDebugString(L"Error! network featureClass not exist ,why ? ");
		return NULL;
	}
#else
	feature->GetExtent()->GetCenter(&cx,&cy,&cz);
#endif
	GlbGlobeTypeEnum globeType = obj->GetGlobe()->GetType();
	if (GLB_GLOBETYPE_GLOBE==globeType)						
		g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(cy),osg::DegreesToRadians(cx),cz,cx,cy,cz);

	//osg::Vec3d lnPt1,lnPt2;

	CGlbWString datalocate = L"";	

	glbInt32 edgeColor = RGB(128,25,32);
	if (networkRenderInfo->edgeColor)
		edgeColor = networkRenderInfo->edgeColor->GetValue(feature);
	glbInt32 edgeOpacity = 100;
	if (networkRenderInfo->edgeOpacity)
		edgeOpacity = networkRenderInfo->edgeOpacity->GetValue(feature);
	glbDouble edgeLineWidth = 3;
	if (networkRenderInfo->edgeLineWidth)
		edgeLineWidth = networkRenderInfo->edgeLineWidth->GetValue(feature);
	if (networkRenderInfo->edgeModelLocate)
		datalocate = networkRenderInfo->edgeModelLocate->GetValue(feature);	

	if (networkRenderInfo->edgeModelLocate && datalocate.length()>0)
	{// 从文件中读取模型	
		glbInt32 index = datalocate.find_first_of(L'.');
		if(index == 0)
		{// 处理当前执行文件的相对路径情况 ./AAA/....
			CGlbWString execDir = CGlbPath::GetExecDir();
			datalocate = execDir + datalocate.substr(1,datalocate.size());
		}
		osg::Node* modelNode = osgDB::readNodeFile(datalocate.ToString());
		if (modelNode)
		{			
			swNode->addChild(modelNode);
			modelNode->setName(buff);
			//blSwiNode->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
		}		
		else //if (datalocate.length()>0)
			GlbLogWOutput(GLB_LOGTYPE_ERR,L"读取网络数据模型文件(%s)失败. \r\n",datalocate.c_str());
	}
	else
	{// 读取几何数据
		IGlbGeometry* geo = NULL;
		feature->GetGeometry(&geo);
		CGlbLine* line = NULL;
		if(geo->GetType()==GLB_GEO_LINE)
			line = dynamic_cast<CGlbLine*>(geo);
		if (line)
		{// 线
			glbInt32 ptCnt = line->GetCount();
			if (ptCnt>1)
			{// 由2个及多个点组成的线				
				if (1)
				{// 测试DrawEdgeByCylinderShape(...) 马林 2017.2.16
					glbDouble ptx,pty,ptz;					
					osg::Matrixd m;
					osg::Vec3d origVec(cx,cy,cz);
					m.setTrans(origVec);
					osg::ref_ptr<osg::MatrixTransform> grp = new osg::MatrixTransform;
					grp->setName(buff);	
					grp->setMatrix(m);
					swNode->addChild(grp);

					osg::Vec3d v1, v2;
					osg::Vec4 cc = GetColor(edgeColor);
					cc.set(cc.r(),cc.g(),cc.b(),edgeOpacity/100.0f);

					for (glbInt32 k = 0; k < ptCnt-1; k++)
					{
						line->GetPoint(k,&ptx,&pty,&ptz);						
						if (GLB_GLOBETYPE_GLOBE==globeType)						
							g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(pty),osg::DegreesToRadians(ptx),ptz,ptx,pty,ptz);
						v1.set(ptx,pty,ptz);						

						line->GetPoint(k+1,&ptx,&pty,&ptz);						
						if (GLB_GLOBETYPE_GLOBE==globeType)						
							g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(pty),osg::DegreesToRadians(ptx),ptz,ptx,pty,ptz);
						v2.set(ptx,pty,ptz);						

						osg::ref_ptr<osg::Node> node = DrawEdgeByCylinderShape(v1-origVec,v2-origVec,edgeLineWidth*0.5,cc);
						if (node)
							grp->addChild(node);
					}

					osg::StateSet* ss = grp->getOrCreateStateSet();
					ss->setMode( GL_RESCALE_NORMAL/*GL_NORMALIZE*/, osg::StateAttribute::ON );

					osg::ref_ptr<osg::LightModel> lm = new osg::LightModel;
					lm->setTwoSided(true);
					ss->setAttribute(lm.get(),osg::StateAttribute::ON);
					osg::ref_ptr<osg::Material> material = new osg::Material;
					osg::Vec4 color = GetColor(edgeColor);
					glbInt32 opacity = edgeOpacity;
					material->setDiffuse(osg::Material::FRONT_AND_BACK,color);
					material->setAlpha(osg::Material::FRONT_AND_BACK,opacity / 100.0);
					ss->setAttribute(material,osg::StateAttribute::ON);
					if(opacity < 100)
					{
						ss->setMode(GL_BLEND,osg::StateAttribute::ON);
						if(ss->getRenderingHint() != osg::StateSet::TRANSPARENT_BIN)
							ss->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);			
					}
					else
					{
						ss->setMode(GL_BLEND,osg::StateAttribute::OFF);
						if(ss->getRenderingHint() != osg::StateSet::OPAQUE_BIN)
							ss->setRenderingHint(osg::StateSet::OPAQUE_BIN);
					}
					ss->setMode(GL_LIGHTING,osg::StateAttribute::ON);					
				}
				//else 中心实线--线对接没有问题了，不再有偏移,为了远处仍然能看到管线，一直绘制
				{
					osg::ref_ptr<osg::MatrixTransform> edgeTrans = new osg::MatrixTransform;
					edgeTrans->setName(buff);
					swNode->addChild(edgeTrans);

					osg::ref_ptr<osg::Geode> edgeGeode = new osg::Geode;
					edgeTrans->addChild(edgeGeode.get());
					osg::ref_ptr<osg::Geometry> edgeGeom = new osg::Geometry;
					edgeGeode->addDrawable(edgeGeom.get());

					glbDouble ptx,pty,ptz;
					osg::Matrixd m;
					osg::Vec3d origVec(cx,cy,cz);
					m.setTrans(origVec);
					edgeTrans->setMatrix(m);

					osg::ref_ptr<osg::Vec3Array> vtArray = new osg::Vec3Array;
					osg::ref_ptr<osg::Vec4Array> clrArray = new osg::Vec4Array;
					for (glbInt32 k = 0; k < ptCnt; k++)
					{
						line->GetPoint(k,&ptx,&pty,&ptz);						
						if (GLB_GLOBETYPE_GLOBE==globeType)						
							g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(pty),osg::DegreesToRadians(ptx),ptz,ptx,pty,ptz);

						vtArray->push_back(osg::Vec3d(ptx,pty,ptz)-origVec);
					}

					edgeGeom->setVertexArray(vtArray.get());		
					//osg::Vec4 cc(1,0,0,1); //= GetColor(edgeColor);
					osg::Vec4 cc = GetColor(edgeColor);
					//cc.set(cc.r(),cc.g(),cc.b(),edgeOpacity/100.0f);

					clrArray->push_back(cc);
					edgeGeom->setColorArray(clrArray);
					edgeGeom->setColorBinding(osg::Geometry::BIND_OVERALL);

					edgeGeom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINE_STRIP,0,vtArray->size()));

					//线宽
					osg::LineWidth *lineWidth = new osg::LineWidth;
					lineWidth->setWidth(edgeLineWidth);
					osg::StateSet* ss = edgeTrans->getOrCreateStateSet();
					ss->setAttributeAndModes(lineWidth,osg::StateAttribute::ON);

					//// 透明度								
					////混合是否启动
					//if (edgeOpacity<100)
					//{// 启用混合
					//	ss->setMode(GL_BLEND, osg::StateAttribute::ON);			
					//	if (osg::StateSet::TRANSPARENT_BIN != ss->getRenderingHint())
					//		ss->setRenderingHint( osg::StateSet::TRANSPARENT_BIN );						
					//}
					//else
					//{// 禁用混合						
						ss->setMode(GL_BLEND, osg::StateAttribute::OFF  | osg::StateAttribute::OVERRIDE);							
						if (osg::StateSet::TRANSPARENT_BIN == ss->getRenderingHint())
							ss->setRenderingHint( osg::StateSet::OPAQUE_BIN );		
					//}
				}
			}
		}// end if(line)			
	}	

	// node
	glbInt32 nodeColor = RGB(255,125,0);
	if (networkRenderInfo->nodeColor)
		nodeColor = networkRenderInfo->nodeColor->GetValue(feature);
	glbInt32 nodeOpacity = 255;
	if (networkRenderInfo->nodeOpacity)
		nodeOpacity = networkRenderInfo->nodeOpacity->GetValue(feature);
	// 是否渲染fromNode或 toNode
	glbBool bRenderFromNode = false;
	if (networkRenderInfo->isRenderFromNode)
		bRenderFromNode = networkRenderInfo->isRenderFromNode->GetValue(feature);
	glbBool bRenderToNode = false;
	if (networkRenderInfo->isRenderToNode)
		bRenderToNode = networkRenderInfo->isRenderToNode->GetValue(feature);
	glbDouble nodeSize = 6;
	if (networkRenderInfo->nodeSize)
		nodeSize = networkRenderInfo->nodeSize->GetValue(feature);

	IGlbFeatureClass* fclss = const_cast<IGlbFeatureClass*>(feature->GetClass());
	IGlbNetworkDataset* p_networkDs = dynamic_cast<IGlbNetworkDataset*>(fclss);
	IGlbFeatureClass* p_networkNodeFtCls = NULL; 
	if (p_networkDs)
		p_networkNodeFtCls = const_cast<IGlbFeatureClass*>(p_networkDs->GetNodeClass());	

	// 网络数据集，绘制edge时同时需要绘制边的两个端点 移到CGlbNetworkObject::ReadData中
	glbInt32 edgeID,fromNodeID,toNodeID;
	edgeID = fromNodeID = toNodeID = -1;
	CGlbFeature* fromNodeFeature = NULL;
	CGlbFeature* toNodeFeature = NULL;
	if (p_networkNodeFtCls)
	{	
		const GLBVARIANT* va = feature->GetValueByName(L"GlbEdgeID");
		if (va && va->isnull == false && va->vt == GLB_DATATYPE_INT32)
			edgeID = va->lVal;
		
		va = feature->GetValueByName(L"GlbFNodeID");
		if (va && va->isnull == false && va->vt == GLB_DATATYPE_INT32)
			fromNodeID = va->lVal;
		
		va = feature->GetValueByName(L"GlbTNodeID");
		if (va && va->isnull == false && va->vt == GLB_DATATYPE_INT32)
			toNodeID = va->lVal;	
	}

	//bool bRenderFromNodeGeometry = false;
	//osg::ref_ptr<osg::Geometry> _fromNodeGeom = NULL;
	if (bRenderFromNode)
	{
		if (p_networkNodeFtCls)
			fromNodeFeature = p_networkNodeFtCls->GetFeature(fromNodeID);
		sprintf_s(buff,"network_node%d",fromNodeID);

		if (networkRenderInfo->fromNodeModelLocate)
			datalocate = networkRenderInfo->fromNodeModelLocate->GetValue(fromNodeFeature);
		if (networkRenderInfo->fromNodeModelLocate && datalocate.length()>0)
		{
			glbInt32 index = datalocate.find_first_of(L'.');
			if(index == 0)
			{// 处理当前执行文件的相对路径情况 ./AAA/....
				CGlbWString execDir = CGlbPath::GetExecDir();
				datalocate = execDir + datalocate.substr(1,datalocate.size());
			}
			osg::Node* modelNode = osgDB::readNodeFile(datalocate.ToString());
			if (modelNode)
			{
				swNode->addChild(modelNode);
				modelNode->setName("network_fromnode");
			}
			else //if (datalocate.length()>0)
				GlbLogWOutput(GLB_LOGTYPE_ERR,L"读取网络数据模型文件(%s)失败. \r\n",datalocate.c_str());
		}
		else if (fromNodeFeature)
		{// 读取几何数据			
			IGlbGeometry* geo = NULL;
			fromNodeFeature->GetGeometry(&geo);
			CGlbPoint* pt = NULL;
			if(geo->GetType()==GLB_GEO_POINT)
				pt = dynamic_cast<CGlbPoint*>(geo);

			if (pt)
			{
				osg::ref_ptr<osg::MatrixTransform> nodeTrans = new osg::MatrixTransform;
				nodeTrans->setName(buff);
				swNode->addChild(nodeTrans);

				osg::ref_ptr<osg::Geode> nodeGeode = new osg::Geode;
				nodeGeode->setName("network_fromnode");
				nodeTrans->addChild(nodeGeode.get());
				osg::ref_ptr<osg::Geometry> nodeGeom = new osg::Geometry;
				nodeGeode->addDrawable(nodeGeom.get());

				glbDouble ptx,pty,ptz;
				pt->GetXYZ(&ptx,&pty,&ptz);
				if (GLB_GLOBETYPE_GLOBE==globeType)						
					g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(pty),osg::DegreesToRadians(ptx),ptz,ptx,pty,ptz);

				osg::Matrix m;
				osg::Vec3d origVec(cx,cy,cz);
				m.setTrans(origVec);
				nodeTrans->setMatrix(m);

				osg::ref_ptr<osg::Vec3Array> vtArray = new osg::Vec3Array;
				osg::ref_ptr<osg::Vec4Array> clrArray = new osg::Vec4Array;				
				vtArray->push_back(osg::Vec3(ptx,pty,ptz));				

				nodeGeom->setVertexArray(vtArray.get());		
				osg::Vec4 cc = GetColor(nodeColor);
				cc.set(cc.r(),cc.g(),cc.b(),edgeOpacity/100.0f);

				clrArray->push_back(cc);
				nodeGeom->setColorArray(clrArray);
				nodeGeom->setColorBinding(osg::Geometry::BIND_OVERALL);

				nodeGeom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::POINTS,0,vtArray->size()));

				//点大小
				osg::Point *osgpoint = new osg::Point;
				osgpoint->setSize(nodeSize);
				osg::StateSet* ss = nodeTrans->getOrCreateStateSet();
				ss->setAttributeAndModes(osgpoint,osg::StateAttribute::ON);

				// 透明度								
				//混合是否启动
				if (nodeOpacity<100)
				{// 启用混合
					ss->setMode(GL_BLEND, osg::StateAttribute::ON);			
					if (osg::StateSet::TRANSPARENT_BIN != ss->getRenderingHint())
						ss->setRenderingHint( osg::StateSet::TRANSPARENT_BIN );						
				}
				else
				{// 禁用混合						
					ss->setMode(GL_BLEND, osg::StateAttribute::OFF  | osg::StateAttribute::OVERRIDE);							
					if (osg::StateSet::TRANSPARENT_BIN == ss->getRenderingHint())
						ss->setRenderingHint( osg::StateSet::OPAQUE_BIN );		
				}

				//bRenderFromNodeGeometry = true;
				//_fromNodeGeom = nodeGeom;
			}// end of pt
		}
	}
	if (bRenderToNode)
	{
		if (p_networkNodeFtCls)
			toNodeFeature = p_networkNodeFtCls->GetFeature(toNodeID);
		sprintf_s(buff,"network_node%d",toNodeID);

		if (networkRenderInfo->toNodeModelLocate)
			datalocate = networkRenderInfo->toNodeModelLocate->GetValue(toNodeFeature);
		if (networkRenderInfo->toNodeModelLocate && datalocate.length()>0)
		{
			glbInt32 index = datalocate.find_first_of(L'.');
			if(index == 0)
			{// 处理当前执行文件的相对路径情况 ./AAA/....
				CGlbWString execDir = CGlbPath::GetExecDir();
				datalocate = execDir + datalocate.substr(1,datalocate.size());
			}
			osg::Node* modelNode = osgDB::readNodeFile(datalocate.ToString());
			if (modelNode)
			{
				swNode->addChild(modelNode);
				modelNode->setName("network_tonode");
			}
			else //if (datalocate.length()>0)
				GlbLogWOutput(GLB_LOGTYPE_ERR,L"读取网络数据模型文件(%s)失败. \r\n",datalocate.c_str());
		}
		else if (toNodeFeature)
		{// 读取几何数据
			IGlbGeometry* geo = NULL;
			toNodeFeature->GetGeometry(&geo);
			CGlbPoint* pt = NULL;
			if(geo->GetType()==GLB_GEO_POINT)
				pt = dynamic_cast<CGlbPoint*>(geo);
			if (pt )
			{
				osg::ref_ptr<osg::MatrixTransform> nodeTrans = new osg::MatrixTransform;
				nodeTrans->setName(buff);
				swNode->addChild(nodeTrans);

				osg::ref_ptr<osg::Geode> nodeGeode = new osg::Geode;
				nodeGeode->setName("network_tonode");
				nodeTrans->addChild(nodeGeode.get());
				osg::ref_ptr<osg::Geometry> nodeGeom = new osg::Geometry;
				nodeGeode->addDrawable(nodeGeom.get());

				glbDouble ptx,pty,ptz;
				pt->GetXYZ(&ptx,&pty,&ptz);
				if (GLB_GLOBETYPE_GLOBE==globeType)						
					g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(pty),osg::DegreesToRadians(ptx),ptz,ptx,pty,ptz);

				osg::Matrix m;
				osg::Vec3d origVec(cx,cy,cz);
				m.setTrans(origVec);
				nodeTrans->setMatrix(m);

				osg::ref_ptr<osg::Vec3Array> vtArray = new osg::Vec3Array;
				osg::ref_ptr<osg::Vec4Array> clrArray = new osg::Vec4Array;				
				vtArray->push_back(osg::Vec3(ptx,pty,ptz));				

				nodeGeom->setVertexArray(vtArray.get());		
				osg::Vec4 cc = GetColor(nodeColor);
				cc.set(cc.r(),cc.g(),cc.b(),edgeOpacity/100.0f);

				clrArray->push_back(cc);
				nodeGeom->setColorArray(clrArray);
				nodeGeom->setColorBinding(osg::Geometry::BIND_OVERALL);

				nodeGeom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::POINTS,0,vtArray->size()));

				//点大小
				osg::Point *osgpoint = new osg::Point;
				osgpoint->setSize(nodeSize);
				osg::StateSet* ss = nodeTrans->getOrCreateStateSet();
				ss->setAttributeAndModes(osgpoint,osg::StateAttribute::ON);

				// 透明度								
				//混合是否启动
				if (nodeOpacity<100)
				{// 启用混合
					ss->setMode(GL_BLEND, osg::StateAttribute::ON);			
					if (osg::StateSet::TRANSPARENT_BIN != ss->getRenderingHint())
						ss->setRenderingHint( osg::StateSet::TRANSPARENT_BIN );						
				}
				else
				{// 禁用混合						
					ss->setMode(GL_BLEND, osg::StateAttribute::OFF  | osg::StateAttribute::OVERRIDE);							
					if (osg::StateSet::TRANSPARENT_BIN == ss->getRenderingHint())
						ss->setRenderingHint( osg::StateSet::OPAQUE_BIN );		
				}				
			}			
		}
	}

	if (swNode->getNumChildren()>0)	
	{		
		return swNode.release();
	}

	return NULL;
}

#include <osg/Geometry>
osg::ref_ptr<osg::Drawable> CGlbGlobeNetworkSymbol::buildCylinder(unsigned int numSegments, float radius, float height, osg::Vec4 clr)
{
	osg::ref_ptr<osg::Geometry> nodeGeom = new osg::Geometry;
	osg::ref_ptr<osg::Vec3Array> vtArray = new osg::Vec3Array;
	osg::ref_ptr<osg::Vec3Array> norArray = new osg::Vec3Array;
	//osg::ref_ptr<osg::Vec4Array> clrArray = new osg::Vec4Array;		
	osg::ref_ptr<osg::Vec2Array> texcoordArray = new osg::Vec2Array;
			
	nodeGeom->setVertexArray(vtArray.get());			
	//clrArray->push_back(clr);
	//nodeGeom->setColorArray(clrArray);
	nodeGeom->setColorBinding(osg::Geometry::BIND_OVERALL);
	nodeGeom->setNormalArray(norArray);
	nodeGeom->setNormalBinding(osg::Geometry::BIND_PER_VERTEX);
	nodeGeom->setTexCoordArray(0,texcoordArray.get());

	const float angleDelta = 2.0f*osg::PI/(float)numSegments;
	const float texCoordDelta = 1.0f/(float)numSegments;

	const float r = radius;
	const float h = height;

	float basez = -h*0.5f;
	float topz = h*0.5f;

	float angle = 0.0f;
	float texCoord = 0.0f;

	for(unsigned int bodyi=0;
		bodyi<numSegments;
		++bodyi,angle+=angleDelta,texCoord+=texCoordDelta)
	{
		float c = cosf(angle);
		float s = sinf(angle);

		//gl.Normal3f(c,s,0.0f);
		norArray->push_back(osg::Vec3(c,s,0.0f));

		//gl.TexCoord2f(texCoord,1.0f);
		texcoordArray->push_back(osg::Vec2(texCoord,1.0f));

		//gl.Vertex3f(c*r,s*r,topz);
		vtArray->push_back(osg::Vec3f(c*r,s*r,topz));

		//gl.TexCoord2f(texCoord,0.0f);
		//gl.Vertex3f(c*r,s*r,basez);

		norArray->push_back(osg::Vec3(c,s,0.0f));
		vtArray->push_back(osg::Vec3f(c*r,s*r,basez));
		texcoordArray->push_back(osg::Vec2(texCoord,0.0f));
	}

	//// do last point by hand to ensure no round off errors.
	//gl.Normal3f(1.0f,0.0f,0.0f);

	//gl.TexCoord2f(1.0f,1.0f);
	//gl.Vertex3f(r,0.0f,topz);

	//gl.TexCoord2f(1.0f,0.0f);
	//gl.Vertex3f(r,0.0f,basez);

	norArray->push_back(osg::Vec3(1.0f,0.0f,0.0f));
	vtArray->push_back(osg::Vec3f(r,0.0f,topz));
	texcoordArray->push_back(osg::Vec2(1.0f,1.0f));

	norArray->push_back(osg::Vec3(1.0f,0.0f,0.0f));
	vtArray->push_back(osg::Vec3f(r,0.0f,basez));
	texcoordArray->push_back(osg::Vec2(1.0f,0.0f));

	nodeGeom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUAD_STRIP,0,vtArray->size()));

	return nodeGeom;
}

#include "osg/ShapeDrawable"
osg::ref_ptr<osg::Node> CGlbGlobeNetworkSymbol::DrawEdgeByCylinderShape(osg::Vec3d v1, osg::Vec3d v2, float radius, osg::Vec4 clr)
{
	osg::Vec3d v = v2 - v1;
	float len = v.length();
	v.normalize();

	osg::Matrixd mat;
	osg::Quat q;
	q.makeRotate(osg::Vec3d(0,0,1.0),v);
	//mat.makeRotate(q);
	//mat.preMultTranslate((v1+v2)*0.5);

	mat = osg::Matrixd::rotate(q) * osg::Matrixd::translate((v1+v2)*0.5);
#if 0
	// 俩边堵口的管子
	//osg::ref_ptr<osg::TessellationHints> hints = new osg::TessellationHints();
	//hints->setDetailRatio(0.5);
	//osg::ref_ptr<osg::ShapeDrawable> sd = new osg::ShapeDrawable(new osg::Cylinder(osg::Vec3f(0,0,0),radius,len),hints);
	//sd->setColor(clr);
#else
	// 两边不堵口的管子
	int numSegments = 12;
	osg::ref_ptr<osg::Drawable> sd = buildCylinder(numSegments,radius,len,clr);
#endif

	osg::ref_ptr<osg::MatrixTransform> mt = new osg::MatrixTransform;
	mt->setMatrix(mat);

	osg::ref_ptr<osg::Geode> geode = new osg::Geode;
	mt->addChild(geode);
	geode->addDrawable(sd);

	return mt;
}

osg::ref_ptr<osg::Node> CGlbGlobeNetworkSymbol::DrawEdgeByCylinderShape(IGlbGeometry* geo, float radius, osg::Vec4 clr)
{
	CGlbLine* ln = dynamic_cast<CGlbLine*>(geo);


	return NULL;
}

osg::ref_ptr<osg::Node> CGlbGlobeNetworkSymbol::DrawEdgeByCylinderShape(osg::Vec3 v1, osg::Vec3 v2, float radius, const char* textureLocate)
{

	return NULL;
}

