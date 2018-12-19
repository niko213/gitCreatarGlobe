#include "StdAfx.h"
#include "GlbGlobeTinSymbol.h"

#include "CGlbGlobe.h"
#include "GlbGlobeObjectRenderInfos.h"
#include "GlbString.h"

#include "GlbTin.h"
#include "GlbLine.h"
#include "GlbPoint.h"

#include "osgDB/ReadFile"
#include "osg/Geode"
#include "osg/Geometry"
#include "osg/MatrixTransform"
#include "osg/Material"
#include "osg/CullFace"
#include "osg/Depth"
//#include "osgDB/WriteFile"
#include "osgUtil/SmoothingVisitor"
#include "osg/LightModel"
#include "GlbTinZScaleVisitor.h"
#include "GlbLog.h"

//--matrixtransform
//		|
//		|
//	  geode
//		|
//		|
//		geomery
namespace GlbGlobe
{
	CGlbGlobeTinSymbol::CGlbGlobeTinSymbol(void)
	{
	}

	CGlbGlobeTinSymbol::~CGlbGlobeTinSymbol(void)
	{
	}

	osg::Node * CGlbGlobeTinSymbol::Draw( CGlbGlobeRObject *obj,IGlbGeometry *geo )
	{
		if (obj==NULL||geo==NULL||obj->GetGlobe()==NULL)
			return NULL;

		GlbRenderInfo *renderInfo = obj->GetRenderInfo();
		GlbTinSymbolInfo *tinSymInfo = dynamic_cast<GlbTinSymbolInfo *>(renderInfo);
		if(NULL == tinSymInfo)
			return NULL;

		osg::Node *tinNode=NULL;
		GlbGeometryTypeEnum geoType = geo->GetType();
		switch(geoType)
		{
		case GLB_GEO_TIN:
			{
				CGlbTin *geoTmp=dynamic_cast<CGlbTin *>(geo);
				if (geoTmp==NULL)
					break;
				tinNode = buildDrawable(obj,geoTmp);
				break;
			}
		case GLB_GEO_LINE:
			{
				CGlbLine *geoTmp=dynamic_cast<CGlbLine *>(geo);
				if (geoTmp==NULL)
					break;
				tinNode = buildDrawable(obj,geoTmp);
				break;
			}
		case GLB_GEO_POINT:
			{
				CGlbPoint *geoTmp=dynamic_cast<CGlbPoint *>(geo);
				if (geoTmp==NULL)
					break;
				tinNode = buildDrawable(obj,geoTmp);
				break;
			}
		case GLB_GEO_MULTITIN:
			{
				CGlbMultiTin *geoTmp = dynamic_cast<CGlbMultiTin*>(geo);
				if(geoTmp == NULL)
					break;
				tinNode = buildDrawable(obj,geoTmp);
				break;
			}
		default:
			{
				tinNode=NULL;
				break;
			}			
		}
		return tinNode;
	}

	void CGlbGlobeTinSymbol::DrawToImage( CGlbGlobeRObject *obj,IGlbGeometry* geom, glbByte *image,glbInt32 imageW, glbInt32 imageH,CGlbExtent &ext )
	{
		return ;
		throw std::exception("The method or operation is not implemented.");
	}

	IGlbGeometry * CGlbGlobeTinSymbol::GetOutline( CGlbGlobeRObject *obj,IGlbGeometry *geo )
	{
		GlbRenderInfo *renderInfo = obj->GetRenderInfo();
		GlbTinSymbolInfo *tinSymInfo = dynamic_cast<GlbTinSymbolInfo *>(renderInfo);
		if(NULL == tinSymInfo)
			return NULL;
		CGlbFeature *mpt_feature = obj->GetFeature();
		IGlbGeometry *mpr_tinGeo=geo;
		if(mpt_feature!=NULL)
		{
			CGlbExtent *pExt= const_cast<CGlbExtent*>(mpt_feature->GetExtent());
			if (pExt)
			{
				glbDouble xx,yy,zz;
				pExt->GetCenter(&xx,&yy,&zz);
				CGlbPoint *outLint=new CGlbPoint(xx,yy,zz);
				return outLint;
			}
		}
		else if (mpr_tinGeo!=NULL)
		{
			CGlbExtent *pExt= const_cast<CGlbExtent*>(mpr_tinGeo->GetExtent());
			if (pExt)
			{
				glbDouble xx,yy,zz;
				pExt->GetCenter(&xx,&yy,&zz);
				CGlbPoint *outLint=new CGlbPoint(xx,yy,zz);
				return outLint;
			}
		}
		return NULL;
	}

	osg::Node * CGlbGlobeTinSymbol::buildDrawable( CGlbGlobeRObject *obj,CGlbPoint *geo )
	{
		GlbRenderInfo *renderInfo = obj->GetRenderInfo();
		GlbTinSymbolInfo *tinSymInfo = dynamic_cast<GlbTinSymbolInfo *>(renderInfo);
		if(NULL == tinSymInfo)
			return NULL;
		
		CGlbGlobe *pGlobe=obj->GetGlobe();
		CGlbFeature *feature = obj->GetFeature();
		CGlbWString datalocate = L"";
		if(tinSymInfo->locate != NULL)
			datalocate = tinSymInfo->locate->GetValue(feature);

		osg::Group *tinGroup = new osg::Group;
		osg::ref_ptr<osg::MatrixTransform> mt = new osg::MatrixTransform;
		double xx,yy,zz;
		geo->GetXYZ(&xx,&yy,&zz);
		if (datalocate!=L"")
		{
			osg::ref_ptr<osg::Node> tinNode = NULL;
			if (obj->IsUseInstance())
			{
				//modelNode  = 从实例化管理器Find(datalocate )
				//CGlbGlobeInstanceManager::findInstance(datalocate)
			}
			else
			{
				glbInt32 index = datalocate.find_first_of(L'.');
				if(index == 0)
				{// 处理当前执行文件的相对路径情况 ./AAA/....
					CGlbWString execDir = CGlbPath::GetExecDir();
					datalocate = execDir + datalocate.substr(1,datalocate.size());
				}
				tinNode = osgDB::readNodeFile(datalocate.ToString());
				if (tinNode==NULL)
				{
					GlbLogWOutput(GLB_LOGTYPE_ERR,L"读取Tin(Oid%d)模型文件(%s)失败. \r\n",feature->GetOid(),datalocate.c_str());	
					return NULL;
				}

				mt->addChild(tinNode.get());
				tinGroup->addChild(mt.get());						
			}
		}
		else//point geo
		{
			osg::ref_ptr<osg::Geode> geode = new osg::Geode;
			osg::ref_ptr<osg::Geometry> pixelGeom = new osg::Geometry;
			osg::ref_ptr<osg::Vec3dArray> renderVertexes = new osg::Vec3dArray;
			osg::Vec3d position(xx,yy,zz);
			if(pGlobe->GetType() == GLB_GLOBETYPE_GLOBE)
			{
				g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(position.y()),osg::DegreesToRadians(position.x()),position.z(),
					position.x(),position.y(),position.z());
			}
			renderVertexes->push_back(position);
			pixelGeom->setVertexArray(renderVertexes);
			pixelGeom->addPrimitiveSet(
				new osg::DrawArrays(osg::PrimitiveSet::POINTS,0,renderVertexes->size()));
			geode->addDrawable(pixelGeom);
			mt->addChild(geode.get());
			tinGroup->addChild(mt.get());
		}

		glbInt32 Id = -1;
		if(feature)
			Id = feature->GetOid();
		char partName[32];
		sprintf_s(partName,"Part%d",Id);
		mt->setName(partName);
		return tinGroup;
	}

	osg::Node * CGlbGlobeTinSymbol::buildDrawable( CGlbGlobeRObject *obj,CGlbLine *geo )
	{
		return NULL;
	}

	osg::Node * CGlbGlobeTinSymbol::buildDrawable( CGlbGlobeRObject *obj,CGlbTin *geo )
	{
		GlbRenderInfo *renderInfo    = obj->GetRenderInfo();
		GlbTinSymbolInfo *tinSymInfo = dynamic_cast<GlbTinSymbolInfo *>(renderInfo);
		if(NULL == tinSymInfo)
			return NULL;
		
		CGlbGlobe *pGlobe=obj->GetGlobe();
		CGlbFeature *feature   = obj->GetFeature();
		CGlbWString datalocate = L"";
		if(tinSymInfo->locate != NULL)
			datalocate = tinSymInfo->locate->GetValue(feature);

		glbInt32 Id = feature->GetOid();
		char partName[32];
		sprintf_s(partName,"Part%d",Id);
		osg::Group *tinGroup = new osg::Group;
		osg::ref_ptr<osg::MatrixTransform> mt = new osg::MatrixTransform;
		osg::ref_ptr<osg::Node> tinNode = NULL;
		if (datalocate!=L"")
		{
			if (obj->IsUseInstance())
			{
				//modelNode  = 从实例化管理器Find(datalocate )
				//CGlbGlobeInstanceManager::findInstance(datalocate)
			}
			else
			{
				// 注意此处一定要去除文件catch，以防止数据内存区域公用问题!!!! 马林 20160112
				osg::ref_ptr<osgDB::Options> op = new osgDB::Options();
				op->setObjectCacheHint(osgDB::Options::CACHE_NONE);
				glbInt32 index = datalocate.find_first_of(L'.');
				if(index == 0)
				{// 处理当前执行文件的相对路径情况 ./AAA/....
					CGlbWString execDir = CGlbPath::GetExecDir();
					datalocate = execDir + datalocate.substr(1,datalocate.size());
				}
				tinNode = osgDB::readNodeFile(datalocate.ToString(),op.get());
				if (tinNode==NULL)
				{			
					GlbLogWOutput(GLB_LOGTYPE_ERR,L"读取Tin(Oid%d)模型文件(%s)失败. \r\n",feature->GetOid(),datalocate.c_str());
					return NULL;
				}

				glbDouble zScale = 1.0;
				if (pGlobe->GetType() == GLB_GLOBETYPE_GLOBE)
				{// 球面模式下的z缩放
					if (tinSymInfo->zScale)
						zScale = tinSymInfo->zScale->GetValue(feature);
					if (fabs(zScale-1.0) > 0.00001)
					{// 处理z轴放缩
						CGlbTinZScaleVisitor tz(zScale);
						tinNode->accept(tz);
					}
				}

				mt->addChild(tinNode.get());
				mt->setName(partName);
				tinGroup->addChild(mt.get());
			}
		}
		else
		{
			tinNode = buildDrawable(tinSymInfo,geo,obj->GetGlobe()->GetType(),feature);
			tinNode->setName(partName);
			tinGroup->addChild(tinNode);
		}
		return tinGroup;
	}

	osg::Node * CGlbGlobeTinSymbol::buildDrawable( CGlbGlobeRObject *obj,CGlbMultiTin *geo )
	{
		GlbRenderInfo *renderInfo    = obj->GetRenderInfo();
		GlbTinSymbolInfo *tinSymInfo = dynamic_cast<GlbTinSymbolInfo *>(renderInfo);
		if(NULL == tinSymInfo || geo == NULL)
			return NULL;

		CGlbFeature *feature = obj->GetFeature();
		GlbGlobeTypeEnum type = obj->GetGlobe()->GetType();
		CGlbWString datalocate = L"";
		if(tinSymInfo->locate)
			datalocate = tinSymInfo->locate->GetValue(feature);

		glbInt32 Id = feature->GetOid();
		char partName[32];
		sprintf_s(partName,"Part%d",Id);
		osg::Group *tinGroup = new osg::Group;
		osg::Node *tinNode = NULL;
		if(datalocate == L"")
		{
			glbInt32 count = geo->GetCount();
			for(glbInt32 i = 0; i < count; i++)
			{
				CGlbTin *tin = const_cast<CGlbTin *>(geo->GetTin(i));
				tinNode = buildDrawable(tinSymInfo,tin,type,feature);
				if (tinNode)
				{
					tinNode->setName(partName);
					tinGroup->addChild(tinNode);
				}
			}
		}
		else
		{
			glbDouble x,y,z;
			geo->GetExtent()->GetCenter(&x,&y,&z);
			osg::ref_ptr<osg::MatrixTransform> mt = new osg::MatrixTransform;
			/*osg::Matrix m;
			if(type == GLB_GLOBETYPE_GLOBE)
			{
				g_ellipsoidModel->computeLocalToWorldTransformFromLatLongHeight(
					osg::DegreesToRadians(y),osg::DegreesToRadians(x),z,m);
			}
			else
				m.makeTranslate(x,y,z);
			mt->setMatrix(m);*/

			// 注意此处一定要去除文件catch，以防止数据内存区域公用问题!!!! 马林 20160112
			osg::ref_ptr<osgDB::Options> op = new osgDB::Options();
			op->setObjectCacheHint(osgDB::Options::CACHE_NONE);
			glbInt32 index = datalocate.find_first_of(L'.');
			if(index == 0)
			{// 处理当前执行文件的相对路径情况 ./AAA/....
				CGlbWString execDir = CGlbPath::GetExecDir();
				datalocate = execDir + datalocate.substr(1,datalocate.size());
			}
			tinNode = osgDB::readNodeFile(datalocate.ToString(),op.get());
			if (tinNode==NULL)
			{
				GlbLogWOutput(GLB_LOGTYPE_ERR,L"读取Tin(Oid%d)模型文件(%s)失败. \r\n",feature->GetOid(),datalocate.c_str());
				return NULL;
			}

			glbDouble zScale = 1.0;
			if (type == GLB_GLOBETYPE_GLOBE)
			{// 球面模式下的z缩放
				if (tinSymInfo->zScale)
					zScale = tinSymInfo->zScale->GetValue(feature);
				if (fabs(zScale-1.0) > 0.00001)
				{// 处理z轴放缩
					CGlbTinZScaleVisitor tz(zScale);
					tinNode->accept(tz);
				}
			}

			mt->addChild(tinNode);
			mt->setName(partName);
			tinGroup->addChild(mt.get());
		}

		return tinGroup;
	}

	osg::Node * CGlbGlobeTinSymbol::buildDrawable( GlbTinSymbolInfo *info,CGlbTin *geo,GlbGlobeTypeEnum type,CGlbFeature *feature )
	{
		if(geo == NULL)
			return NULL;
		glbInt32 vertexCount = geo->GetVertexCount();
		glbInt32 triangleCount = geo->GetTriangleCount();
		if(vertexCount < 1 || triangleCount < 1)
			return NULL;
		// 是否需要显示法线 - 测试用 ml 2017.5.18
		bool bDrawNormal = false;
		float lineLen = 10.0f;
		osg::ref_ptr<osg::Geometry> normalGeom = NULL;
		osg::ref_ptr<osg::Vec3Array> normaldrawPoints = NULL;
		osg::ref_ptr<osg::Vec4Array> normalclrs = NULL;
		if (bDrawNormal)
		{
			normalGeom = new osg::Geometry;
			normaldrawPoints = new osg::Vec3Array();
			normalclrs = new osg::Vec4Array();
			normalGeom->setVertexArray(normaldrawPoints.get());
			normalGeom->setColorArray(normalclrs.get());
			normalGeom->setColorBinding(osg::Geometry::BIND_OVERALL);
			normalclrs->push_back(osg::Vec4(0,1,0,1)); //绿色线显示法向
		}

		// 是否需要平滑显示
		glbBool bSmoothing = true;
		if (info->bSmoothing)
			bSmoothing = info->bSmoothing->GetValue(feature);
		//glbBool bIsBody = false;
		//if (info->bIsBody)
		//	bIsBody = info->bIsBody->GetValue(feature);

		glbDouble ptx,pty,ptz;
		glbInt32 first_ptidx,second_ptidx,third_ptidx;
		//osg::ref_ptr<osg::DrawElementsUInt> indexArray = new osg::DrawElementsUInt(osg::DrawElementsUInt::TRIANGLES,0);
		osg::ref_ptr<osg::Vec3Array> drawPoints = new osg::Vec3Array();
		osg::ref_ptr<osg::Vec3Array> normalArray = NULL;
		// 申请法线数组
		if (bSmoothing)
			normalArray = new osg::Vec3Array(vertexCount);
		else
			normalArray = new osg::Vec3Array(triangleCount*3);

		osg::ref_ptr<osg::Geometry> geometry = new osg::Geometry;
		geometry->setVertexArray(drawPoints.get());
		geometry->setNormalArray(normalArray.get());
		geometry->setNormalBinding(osg::Geometry::BIND_PER_VERTEX);

		osg::Vec3 normal;
		osg::Vec3d drawPoint;
		osg::Vec3d centerPoint;
		geo->GetExtent()->GetCenter(&ptx,&pty,&ptz);

		glbDouble zScale = 1.0;
		if (type == GLB_GLOBETYPE_GLOBE)
		{// 球面模式下的z缩放
			zScale = info->zScale->GetValue(feature);
			ptz *= zScale;
		}		
		if(type == GLB_GLOBETYPE_GLOBE)
		{
			g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(pty),osg::DegreesToRadians(ptx),ptz,
				centerPoint.x(),centerPoint.y(),centerPoint.z());
		}
		else
			centerPoint.set(ptx,pty,ptz);		

		if (bSmoothing)
		{// 平滑处理
			osg::ref_ptr<osg::UIntArray> countArray = new osg::UIntArray(vertexCount);	
			for(glbInt32 i = 0; i < vertexCount;i++)
			{
				geo->GetVertex(i,&ptx,&pty,&ptz);
				if(type == GLB_GLOBETYPE_GLOBE)
				{// 球面模式下的z缩放 ptz *= zScale;
					g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(pty),osg::DegreesToRadians(ptx),ptz*zScale,
						drawPoint.x(),drawPoint.y(),drawPoint.z());
				}
				else				
					drawPoint.set(ptx,pty,ptz);					
				
				drawPoints->push_back(drawPoint - centerPoint);			
			}

			osg::ref_ptr<osg::UIntArray> indexArray = new osg::UIntArray();
			for(glbInt32 i = 0; i < triangleCount;i++)
			{
				geo->GetTriangle(i,&first_ptidx,&second_ptidx,&third_ptidx);
				indexArray->push_back(first_ptidx);
				indexArray->push_back(second_ptidx);
				indexArray->push_back(third_ptidx);

				//计算三角形的法线
				osg::Vec3 vec0 = drawPoints->at(first_ptidx);
				osg::Vec3 vec1 = drawPoints->at(second_ptidx);
				osg::Vec3 vec2 = drawPoints->at(third_ptidx);
				normal = (vec1-vec0)^(vec2-vec0);
				normal.normalize();
				{// 法线求平均
					normalArray->at(first_ptidx) += normal;
					normalArray->at(second_ptidx) += normal;
					normalArray->at(third_ptidx) += normal;

					countArray->at(first_ptidx)++;
					countArray->at(second_ptidx)++;
					countArray->at(third_ptidx)++;
				}	

				if (bDrawNormal)
				{// 绘制法线
					normaldrawPoints->push_back(vec0);
					normaldrawPoints->push_back(vec1);
					normaldrawPoints->push_back(vec1);
					normaldrawPoints->push_back(vec2);
					normaldrawPoints->push_back(vec2);
					normaldrawPoints->push_back(vec0);
					osg::Vec3 pos = (vec0+vec1+vec2)/3.0;
					normaldrawPoints->push_back(pos);
					normaldrawPoints->push_back(pos+normal*lineLen);
				}
			}

			for(glbInt32 k = 0; k < vertexCount; k++)	
				normalArray->at(k) = normalArray->at(k) / countArray->at(k);
						
			osg::ref_ptr<osg::DrawElementsUInt> primitiveSet = new osg::DrawElementsUInt(osg::DrawElementsUInt::TRIANGLES,indexArray->size(),&indexArray->front());
			geometry->addPrimitiveSet(primitiveSet.get());
		}
		else
		{			
			for(glbInt32 i = 0; i < triangleCount;i++)
			{
				geo->GetTriangle(i,&first_ptidx,&second_ptidx,&third_ptidx);				

				geo->GetVertex(first_ptidx,&ptx,&pty,&ptz);		
				if(type == GLB_GLOBETYPE_GLOBE)
				{
					g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(pty),osg::DegreesToRadians(ptx),ptz*zScale,
						drawPoint.x(),drawPoint.y(),drawPoint.z());
				}
				else		
					drawPoint.set(ptx,pty,ptz);	

				osg::Vec3d vec0(drawPoint.x(),drawPoint.y(),drawPoint.z());
				drawPoints->push_back(vec0 - centerPoint);

				geo->GetVertex(second_ptidx,&ptx,&pty,&ptz);		
				if(type == GLB_GLOBETYPE_GLOBE)
				{
					g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(pty),osg::DegreesToRadians(ptx),ptz*zScale,
						drawPoint.x(),drawPoint.y(),drawPoint.z());
				}
				else		
					drawPoint.set(ptx,pty,ptz);	

				osg::Vec3d vec1(drawPoint.x(),drawPoint.y(),drawPoint.z());
				drawPoints->push_back(vec1 - centerPoint);

				geo->GetVertex(third_ptidx,&ptx,&pty,&ptz);		
				if(type == GLB_GLOBETYPE_GLOBE)
				{
					g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(pty),osg::DegreesToRadians(ptx),ptz*zScale,
						drawPoint.x(),drawPoint.y(),drawPoint.z());
				}
				else		
					drawPoint.set(ptx,pty,ptz);	
				osg::Vec3d vec2(drawPoint.x(),drawPoint.y(),drawPoint.z());
				drawPoints->push_back(vec2 - centerPoint);

				//计算三角形的法线
				normal = (vec1-vec0)^(vec2-vec0);
				normal.normalize(); 

				normalArray->at(i*3) = normal;
				normalArray->at(i*3+1) = normal;
				normalArray->at(i*3+2) = normal;	

				if (bDrawNormal)
				{// 绘制法线
					normaldrawPoints->push_back(vec0 - centerPoint);
					normaldrawPoints->push_back(vec1 - centerPoint);
					normaldrawPoints->push_back(vec1 - centerPoint);
					normaldrawPoints->push_back(vec2 - centerPoint);
					normaldrawPoints->push_back(vec2 - centerPoint);
					normaldrawPoints->push_back(vec0 - centerPoint);
					osg::Vec3 pos = (vec0+vec1+vec2)/3.0 - centerPoint;
					normaldrawPoints->push_back(pos);
					normaldrawPoints->push_back(pos+normal*lineLen);
				}
			}
			geometry->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::TRIANGLES,0,drawPoints->size()));		
		}	
		osg::ref_ptr<osg::StateSet> ss = geometry->getOrCreateStateSet();
		if (zScale != 1.0)
			ss->setMode( GL_RESCALE_NORMAL/*GL_NORMALIZE*/, osg::StateAttribute::ON );

		osg::ref_ptr<osg::LightModel> lm = new osg::LightModel;
		lm->setTwoSided(true); 
		ss->setAttribute(lm.get(),osg::StateAttribute::ON);
		osg::ref_ptr<osg::Material> material = new osg::Material;
		osg::Vec4 color = GetColor(info->color->GetValue(feature));
		glbInt32 opacity = info->opacity->GetValue(feature);
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
		
		//// 由于多tin时上下顶底面重合，为避免z-buffer，需要启动背面剔除 ml 2016.8.29 
		//// 闭合体时需要打开背面剔除，面状tin则需要关闭背面剔除以避免从背面看时tin面被剔除掉。 2017.5.22-- 移到了tinobject中
		//if (bIsBody)
		//	ss->setMode(GL_CULL_FACE,osg::StateAttribute::ON);
		//else
		//	ss->setMode(GL_CULL_FACE,osg::StateAttribute::OFF);

		//osgUtil::SmoothingVisitor::smooth(*(geometry.get()));
		osg::ref_ptr<osg::Geode> geode = new osg::Geode;
		geode->addDrawable(geometry.get());

		if (bDrawNormal)
		{// 绘制方向线
			normalGeom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINES,0,normaldrawPoints->size()));
			osg::ref_ptr<osg::StateSet> _ss = normalGeom->getOrCreateStateSet();
			_ss->setMode(GL_LIGHTING,osg::StateAttribute::OFF);
			geode->addDrawable(normalGeom.get());
		}

		osg::ref_ptr<osg::MatrixTransform> mt = new osg::MatrixTransform;
		osg::Matrix m;
		m.makeTranslate(centerPoint);
		mt->setMatrix(m);
		mt->addChild(geode.get());
		//osgDB::writeNodeFile(*geode.get(),"d:/test.osg");
		return mt.release();
	}
}