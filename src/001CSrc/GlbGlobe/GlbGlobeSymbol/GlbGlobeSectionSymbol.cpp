#include "StdAfx.h"
#include "GlbGlobeSectionSymbol.h"
#include "osgDB/ReadFile"
#include "GlbString.h"
#include "GlbSection.h"
#include "GlbLine.h"
#include "CGlbGlobe.h"
#include "GlbObject.h"
#include <osg/Switch>
#include <osg/LineWidth>
#include <osg/BlendColor>
#include <osg/BlendFunc>
#include <osg/TexMat>
#include <osg/LightModel>
#include <osg/PolygonOffset>
#include <osgUtil/Tessellator>
#include <osg/Material>
#include "GlbLog.h"
// 多边形转为tin
#include "PolygonTrianulate.h"

using namespace GlbGlobe;

CGlbGlobeSectionSymbol::CGlbGlobeSectionSymbol(void)
{
	mpr_prjPlaneEnum = GLB_PLANE_UNKNOWN;
}


CGlbGlobeSectionSymbol::~CGlbGlobeSectionSymbol(void)
{
}

/* 生成的节点树结构
swNode -->osg::Switch
	 |
osg::MatrixTransform
	 |
	 |_________swSecNode ("Sec")
	 |				|	 \有模型
	 |				|	  \______________modelNode
	 |				|  无模型只有几何形体
	 |				|______________MatrixTransform--------geode------geometry
	 |
	 |
	 | ________swArcNode ("Arc")
	 |				|				
	 |				|________arcNode("Arc0")_____MatrixTransform-------geode------geometry
	 |				|
	 |				|________arcNode("Arc1")_____MatrixTransform-------geode------geometry
	 |				|
	 |				|...........
	 |
	 |
	 |________swPolyNode ("Poly")
					|
					|_____________polyNode("Poly0")
					|					|			\有模型
					|					|			 \__________modelNode
					|					| 无模型只有几何形体
					|					|______________MatrixTransform-------geode------geometry
					|_____________polyNode("Poly1")
										|			\有模型
										|			 \__________modelNode
										| 无模型只有几何形体
										|______________MatrixTransform-------geode------geometry
					.......

*/
osg::Node *CGlbGlobeSectionSymbol::Draw(CGlbGlobeRObject *obj,IGlbGeometry *geo)
{
	GlbRenderInfo *renderInfo = obj->GetRenderInfo();
	GlbSectionRenderInfo *sectionRenderInfo = 
		dynamic_cast<GlbSectionRenderInfo *>(renderInfo);
	if(NULL == sectionRenderInfo)
		return NULL;

	CGlbFeature *feature = obj->GetFeature();
	CGlbSection* section = dynamic_cast<CGlbSection*>(feature);
	if (section==NULL) return NULL;

	CGlbExtent *featureExtent = const_cast<CGlbExtent *>(feature->GetExtent());
	mpr_prjPlaneEnum = CaculateProjPlane(featureExtent);
	if (mpr_prjPlaneEnum == GLB_PLANE_UNKNOWN)
		return NULL;

	CGlbWString datalocate = L"";		
	GlbGlobeTypeEnum globeType = obj->GetGlobe()->GetType();
	glbDouble ptx,pty,ptz;
	osg::ref_ptr<osg::Switch> swNode = new osg::Switch;
	osg::ref_ptr<osg::MatrixTransform> swMt = new osg::MatrixTransform;
	swNode->addChild(swMt.get());
	{
		glbDouble xCenter,yCenter;
		feature->GetExtent()->GetCenter(&xCenter,&yCenter);

		osg::Matrix mOffset;

		glbDouble xOffset = 0;
		if(sectionRenderInfo->xOffset)
			xOffset = sectionRenderInfo->xOffset->GetValue(feature);
		glbDouble yOffset = 0;
		if (sectionRenderInfo->yOffset)
			yOffset = sectionRenderInfo->yOffset->GetValue(feature);
		glbDouble zOffset = 0;
		if (sectionRenderInfo->zOffset)
			zOffset = sectionRenderInfo->zOffset->GetValue(feature);

		if(globeType == GLB_GLOBETYPE_FLAT)
			mOffset.makeTranslate(xOffset,yOffset,zOffset);
		else
		{
			double longitude = osg::DegreesToRadians(xCenter);
			double latitude = osg::DegreesToRadians(yCenter);

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
		swMt->setMatrix(mOffset);
	}
	osg::ref_ptr<osg::Switch> swSecNode = new osg::Switch;
	swSecNode->setName("Sec");
	osg::ref_ptr<osg::Switch> swArcNode = new osg::Switch;
	swArcNode->setName("Arc");
	osg::ref_ptr<osg::Switch> swPolyNode = new osg::Switch;
	swPolyNode->setName("Poly");
	// 绘制剖面整体
	if (sectionRenderInfo->isRenderSec)
	{						
		if (sectionRenderInfo->secModelLocate)
			datalocate = sectionRenderInfo->secModelLocate->GetValue(feature);
		if(sectionRenderInfo->secModelLocate && datalocate.length()>0)
		{// 读模型
			glbInt32 index = datalocate.find_first_of(L'.');
			if(index == 0)
			{// 处理当前执行文件的相对路径情况 ./AAA/....
				CGlbWString execDir = CGlbPath::GetExecDir();
				datalocate = execDir + datalocate.substr(1,datalocate.size());
			}
			osg::Node* modelNode = osgDB::readNodeFile(datalocate.ToString());
			if (modelNode)
			{
				swSecNode->addChild(modelNode);
				//swNode->addChild(swSecNode);
				swMt->addChild(swSecNode);
				swSecNode->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
			}
			else //if (datalocate.length()>0)
			{
				GlbLogWOutput(GLB_LOGTYPE_ERR,L"读取剖面(Oid%d)中心线模型文件(%s)失败. \r\n",feature->GetOid(),datalocate.c_str());
			}
		}		
		else
		{// 读几何数据
			//CGlbGeometryCollect		*geo=NULL;
			IGlbGeometry			*geo=NULL;
			CGlbMultiPolygon		*mulPoly=NULL;
			section->GetGeometry((IGlbGeometry**)&geo,0);
			if(geo)			
				mulPoly = dynamic_cast<CGlbMultiPolygon*>(geo);
			
			if (mulPoly)
			{				
				glbInt32 secColor = sectionRenderInfo->secColor->GetValue(feature);
				glbInt32 secOpacity = sectionRenderInfo->secOpacity->GetValue(feature);
				datalocate = sectionRenderInfo->secTextureData->GetValue(feature);
				GlbGlobeTexRepeatModeEnum secTexRepeatMode = (GlbGlobeTexRepeatModeEnum)(sectionRenderInfo->secTexRepeatMode->GetValue(feature));
				glbDouble secTexRotation = sectionRenderInfo->secTextureRotation->GetValue(feature);
				glbInt32 secTilingU = sectionRenderInfo->secTilingU->GetValue(feature);
				glbInt32 secTilingV = sectionRenderInfo->secTilingV->GetValue(feature);

				CGlbExtent* ext = const_cast<CGlbExtent *>(mulPoly->GetExtent());
				ext->GetCenter(&ptx,&pty,&ptz);				
				if (GLB_GLOBETYPE_GLOBE==globeType)						
					g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(pty),osg::DegreesToRadians(ptx),ptz,ptx,pty,ptz);
				osg::ref_ptr<osg::MatrixTransform> secTrans = new osg::MatrixTransform;
				osg::Matrix m;
				osg::Vec3d origVec(ptx,pty,ptz);
				m.setTrans(origVec);				
				secTrans->setMatrix(m);
				
				osg::ref_ptr<osg::Geode> geode = new osg::Geode;
				secTrans->addChild(geode);
				osg::ref_ptr<osg::Geometry> geom = new osg::Geometry;
				geode->addDrawable(geom);
				osg::ref_ptr<osg::Vec3Array> vtArray = new osg::Vec3Array;
				geom->setVertexArray(vtArray);
				osg::ref_ptr<osg::Vec4Array> clrArray = new osg::Vec4Array;				
				osg::ref_ptr<osg::Vec2Array> texArray = new osg::Vec2Array;
				geom->setTexCoordArray(0,texArray);
				osg::ref_ptr<osg::Vec3Array> normArray = new osg::Vec3Array;
				geom->setNormalArray(normArray);
				geom->setNormalBinding(osg::Geometry::BIND_OVERALL);
				osg::Vec3 nor = CaculatePolyNormal(mulPoly,globeType);
				normArray->push_back(nor);

				glbInt32 polyCnt = mulPoly->GetCount();
				unsigned int startIndex = 0;
				for (glbInt32 idx = 0; idx < polyCnt; idx++)
				{					
					CGlbPolygon* poly = const_cast<CGlbPolygon *>(mulPoly->GetPolygon(idx));
					CGlbLine* extRing = const_cast<CGlbLine *>(poly->GetExtRing());
					glbInt32 ptCnt = extRing->GetCount();
					if (ptCnt<3) continue;

					CGlbExtent* polyExt = const_cast<CGlbExtent*>(poly->GetExtent());
					for (glbInt32 i = 0; i < ptCnt; i++)
					{
						extRing->GetPoint(i,&ptx,&pty,&ptz);
						osg::Vec2 texCoord = CaculateTexCoord(ptx,pty,ptz,polyExt,secTexRepeatMode,secTilingU,secTilingV);
						texArray->push_back(texCoord);
						if (GLB_GLOBETYPE_GLOBE==globeType)						
							g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(pty),osg::DegreesToRadians(ptx),ptz,ptx,pty,ptz);
						vtArray->push_back(osg::Vec3(ptx,pty,ptz)-origVec);
					}

					geom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::POLYGON,startIndex,ptCnt));
					startIndex += ptCnt;
				}
				if (datalocate.length()>0)
				{// 纹理文件
					glbInt32 index = datalocate.find_first_of(L'.');
					if(index == 0)
					{// 处理当前执行文件的相对路径情况 ./AAA/....
						CGlbWString execDir = CGlbPath::GetExecDir();
						datalocate = execDir + datalocate.substr(1,datalocate.size());
					}
					osg::Image* texImg = osgDB::readImageFile(datalocate.ToString().c_str());
					if(texImg)
					{
						osg::ref_ptr<osg::Texture2D> tex2d = new osg::Texture2D;
						tex2d->setImage(texImg);				
						tex2d->setWrap(osg::Texture2D::WRAP_S,osg::Texture2D::REPEAT);
						tex2d->setWrap(osg::Texture2D::WRAP_T,osg::Texture2D::REPEAT);
						tex2d->setFilter(osg::Texture::MIN_FILTER,osg::Texture::LINEAR_MIPMAP_LINEAR);
						tex2d->setFilter(osg::Texture::MAG_FILTER,osg::Texture::LINEAR);
						tex2d->setMaxAnisotropy(8.0);
						tex2d->setResizeNonPowerOfTwoHint(true);
						geom->getOrCreateStateSet()->setTextureAttributeAndModes(0, tex2d, osg::StateAttribute::ON );

						// 纹理旋转
						osg::TexMat* _texmat = new osg::TexMat;
						_texmat->setMatrix(osg::Matrix::rotate(osg::DegreesToRadians(secTexRotation),0.0f,0.0f,1.0f));					
						geom->getOrCreateStateSet()->setTextureAttributeAndModes(0,_texmat,osg::StateAttribute::ON);						
					}
					else if (datalocate.length()>0)
						GlbLogWOutput(GLB_LOGTYPE_ERR,L"读取剖面(Oid%d)剖面图像文件(%s)失败. \r\n",feature->GetOid(),datalocate.c_str());
				}
				else
				{// 颜色					
					geom->setColorArray(clrArray);
					osg::Vec4 clr = GetColor(secColor);
					clr.set(clr.r(),clr.g(),clr.b(),secOpacity/100.0f);
					clrArray->push_back(clr);
					geom->setColorBinding(osg::Geometry::BIND_OVERALL);

					osg::StateSet* ss = geom->getOrCreateStateSet();
					ss->setMode(GL_LIGHTING,osg::StateAttribute::OFF  | osg::StateAttribute::PROTECTED);


					// 透明效果
					if (secOpacity<100)
					{// 半透明
						ss->setMode(GL_BLEND, osg::StateAttribute::ON);			
						if (osg::StateSet::OPAQUE_BIN == ss->getRenderingHint())
							ss->setRenderingHint( osg::StateSet::TRANSPARENT_BIN );
					}
					else
					{// 不透明
						ss->setMode(GL_BLEND, osg::StateAttribute::OFF  | osg::StateAttribute::OVERRIDE);								
						if (osg::StateSet::TRANSPARENT_BIN == ss->getRenderingHint())
							ss->setRenderingHint( osg::StateSet::OPAQUE_BIN );		
					}
				}			

				// 多边形网格化
				if (geom->getNumPrimitiveSets()>0)
				{					
					swSecNode->addChild(secTrans);
					//swNode->addChild(swSecNode);
					swMt->addChild(swSecNode);
					osg::ref_ptr<osgUtil::Tessellator> tscx=new osgUtil::Tessellator();
					tscx->setTessellationType(osgUtil::Tessellator::TESS_TYPE_GEOMETRY);
					tscx->setBoundaryOnly(false);
					tscx->setWindingType(osgUtil::Tessellator::TESS_WINDING_ODD);
					//osg::Vec3d pointpos(0.0,0.0,1.0);
					tscx->setTessellationNormal(nor);
					tscx->retessellatePolygons(*geom);
				}				
			}			
		}		

		if (swSecNode->getNumChildren()>0)
		{
			// 打开双面光照
			osg::LightModel* lm = new osg::LightModel;
			lm->setTwoSided(true);
			swSecNode->getOrCreateStateSet()->setAttributeAndModes(lm,osg::StateAttribute::ON);
		}
	}
	// 绘制弧线		
	if (sectionRenderInfo->isRenderArc)
	{
		glbref_ptr<IGlbFeatureCursor> cursor=NULL;
		cursor = const_cast<IGlbFeatureCursor *>(section->QueryArc(NULL));
		if (cursor)
		{
			char arcName[16];
			do 
			{
				CGlbFeature * arcFeature = cursor->GetFeature();
				glbInt32 arcColor = sectionRenderInfo->arcColor->GetValue(arcFeature);
				glbInt32 arcOpacity = sectionRenderInfo->arcOpacity->GetValue(arcFeature);
				glbInt32 arcWidth = sectionRenderInfo->arcWidth->GetValue(arcFeature);
				//GLBVARIANT val;
				//arcFeature->GetValueByName(L"GLBARCID",&val);
				glbInt32 oid = arcFeature->GetOid();

				osg::ref_ptr<osg::Switch> arcNode=new osg::Switch;
				sprintf(arcName,"Arc%d",/*val.lVal*/oid);
				arcNode->setName(arcName);

				IGlbGeometry		  *geo=NULL;				
				CGlbLine              *line=NULL;             
				arcFeature->GetGeometry((IGlbGeometry**)&geo,0);
				if(geo)
				{
					line = dynamic_cast<CGlbLine*>(geo);
				}
				if (line)
				{
					//CGlbExtent* ext = mulLine->GetExtent();
					const CGlbExtent* ext = line->GetExtent();
					ext->GetCenter(&ptx,&pty,&ptz);				
					if (GLB_GLOBETYPE_GLOBE==globeType)						
						g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(pty),osg::DegreesToRadians(ptx),ptz,ptx,pty,ptz);					
					osg::ref_ptr<osg::MatrixTransform> arcTrans = new osg::MatrixTransform;					
					osg::Matrix m;
					osg::Vec3d origVec(ptx,pty,ptz);
					m.setTrans(origVec);			
					arcTrans->setMatrix(m);

					arcNode->addChild(arcTrans);

					osg::ref_ptr<osg::Geode> geode = new osg::Geode;
					arcTrans->addChild(geode);
					osg::ref_ptr<osg::Geometry> geom = new osg::Geometry;
					geode->addDrawable(geom);
					osg::ref_ptr<osg::Vec3Array> vtArray = new osg::Vec3Array;
					geom->setVertexArray(vtArray);
					//glbInt32 lnCnt = mulLine->GetCount();
					unsigned int startIndex=0;
					//for (glbInt32 i = 0; i < lnCnt; i++)
					{
						//CGlbLine3D* line = mulLine->GetLine(i);
						//if (line == NULL) continue;
						glbInt32 ptCnt = line->GetCount();
						for (glbInt32 idx = 0; idx < ptCnt; idx++)
						{
							line->GetPoint(idx,&ptx,&pty,&ptz);

							if (GLB_GLOBETYPE_GLOBE==globeType)						
								g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(pty),osg::DegreesToRadians(ptx),ptz,ptx,pty,ptz);

							vtArray->push_back(osg::Vec3(ptx,pty,ptz) - origVec);
						}						
						geom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINE_STRIP,startIndex,ptCnt));
						startIndex+=ptCnt;
					}
					osg::Vec4 clr = GetColor(arcColor);
					clr.set(clr.r(),clr.g(),clr.b(),arcOpacity/100.0f);

					//osg::ref_ptr<osg::Vec4Array> clrArray = new osg::Vec4Array;
					//geom->setColorArray(clrArray);
					//clrArray->push_back(clr);				
					//geom->setColorBinding(osg::Geometry::BIND_OVERALL);

					// 有可绘制对象时添加节点
					if (geom->getNumPrimitiveSets()>0)
					{
						swArcNode->addChild(arcNode);
						//线宽
						osg::LineWidth *lineWidth = new osg::LineWidth;
						lineWidth->setWidth(arcWidth);

						osg::StateSet* ss = arcTrans->getOrCreateStateSet();
						ss->setAttributeAndModes(lineWidth,osg::StateAttribute::ON);

						osg::ref_ptr<osg::Material> material = dynamic_cast<osg::Material*>(ss->getAttribute(osg::StateAttribute::MATERIAL));
						if (!material.valid())
							material = new osg::Material;
						ss->setAttribute(material.get(),osg::StateAttribute::ON);	
						ss->setMode(GL_DEPTH_TEST,osg::StateAttribute::ON);
						material->setDiffuse(osg::Material::FRONT_AND_BACK,clr);

						// 透明效果
						if (arcOpacity<100)
						{// 半透明
							ss->setMode(GL_BLEND, osg::StateAttribute::ON);			
							if (osg::StateSet::TRANSPARENT_BIN != ss->getRenderingHint())
								ss->setRenderingHint( osg::StateSet::TRANSPARENT_BIN );
						}
						else
						{// 不透明
							ss->setMode(GL_BLEND, osg::StateAttribute::OFF);								
							if (osg::StateSet::OPAQUE_BIN != ss->getRenderingHint())
								ss->setRenderingHint( osg::StateSet::OPAQUE_BIN );		
						}
						ss->setMode(GL_LIGHTING,osg::StateAttribute::OFF);
					}
				}
			} while (cursor->MoveNext());
		}

		if (swArcNode->getNumChildren()>0)		
		{
			swArcNode->getOrCreateStateSet()->setMode(GL_LIGHTING,osg::StateAttribute::OFF  | osg::StateAttribute::PROTECTED);
			//swNode->addChild(swArcNode);
			swMt->addChild(swArcNode);
		}

	}
	CPolygonTriangulater::Control_Point *cp = NULL;
#if 0 // 暂时取消三角化，问题太多
	// 获取section的控制点 初始化CPolygonTriangulate
	std::vector<CPolygonTriangulater::Control_Point> ctrlPts;
	glbref_ptr<IGlbObjectCursor> objCursor = const_cast<IGlbObjectCursor *>(section->QueryCtrlPt(NULL));
	if (objCursor)
	{
		do 
		{
			//double x3d,y3d,z3d, x2d,y2d;
			CPolygonTriangulater::Control_Point pt;
			CGlbObject* _obj = objCursor->GetObject();
			if (_obj)
			{
				const GLBVARIANT* var = _obj->GetValueByName(L"GLB3DX");
				pt.x3d = var->dblVal;			

				var = _obj->GetValueByName(L"GLB3DY");
				pt.y3d = var->dblVal;

				var = _obj->GetValueByName(L"GLB3DZ");
				pt.z3d = var->dblVal;

				var = _obj->GetValueByName(L"GLB2DX");
				pt.x2d = var->dblVal;

				var = _obj->GetValueByName(L"GLB2DY");
				pt.y2d = var->dblVal;
				ctrlPts.push_back(pt);
			}			
		}
		while(objCursor->MoveNext());
	}
	long secpntnum = ctrlPts.size();
	cp = new CPolygonTriangulater::Control_Point[secpntnum];
	for( long ctrlpnt = 0; ctrlpnt < secpntnum; ctrlpnt ++ )
	{
		cp[ctrlpnt].x2d = ctrlPts[ctrlpnt].x2d;
		cp[ctrlpnt].y2d = ctrlPts[ctrlpnt].y2d;
		cp[ctrlpnt].x3d = ctrlPts[ctrlpnt].x3d;
		cp[ctrlpnt].y3d = ctrlPts[ctrlpnt].y3d;
		cp[ctrlpnt].z3d = ctrlPts[ctrlpnt].z3d;
	}
	CPolygonTriangulate ptr;
	// 初始化ptr
	bool isOk = ptr.Initialize(cp, secpntnum);	
#endif
	///////////////////////////////////

	int cnt = 0;
	std::vector<glbInt32> polyOid;
	// 剖面中的多边形
	if (sectionRenderInfo->isRenderPoly)
	{
		glbref_ptr<IGlbFeatureCursor> cursor = NULL;
		cursor = const_cast<IGlbFeatureCursor *>(section->QueryPolygon(NULL));
		if (cursor)
		{
			char polyName[16];
			do 
			{
				CGlbFeature * polyFeature = cursor->GetFeature();
				//GLBVARIANT val;
				//polyFeature->GetValueByName(L"GLBPOLYGONID",&val);
				glbInt32 oid = polyFeature->GetOid();
				polyOid.push_back(oid);
				
				osg::ref_ptr<osg::Switch> polyNode = new osg::Switch;
				sprintf(polyName,"Poly%d",/*val.lVal*/oid);
				polyNode->setName(polyName);				
				
				datalocate.clear();
				if (sectionRenderInfo->polyModelLocate)
					datalocate = sectionRenderInfo->polyModelLocate->GetValue(polyFeature);
				if (sectionRenderInfo->polyModelLocate && datalocate.length()>0)
				{// 读模型数据
					glbInt32 index = datalocate.find_first_of(L'.');
					if(index == 0)
					{// 处理当前执行文件的相对路径情况 ./AAA/....
						CGlbWString execDir = CGlbPath::GetExecDir();
						datalocate = execDir + datalocate.substr(1,datalocate.size());
					}
					osg::Node* modelNode = osgDB::readNodeFile(datalocate.ToString());
					if (modelNode)	
					{
						polyNode->addChild(modelNode);	
						swPolyNode->addChild(polyNode);						
					}
					else //if (datalocate.length()>0)
					{
						GlbLogWOutput(GLB_LOGTYPE_ERR,L"读取剖面(Oid%d)多边形(Oid%%d)模型文件(%s)失败. \r\n",feature->GetOid(),polyFeature->GetOid(),datalocate.c_str());						
					}
				}
				else
				{// 读几何数据
					IGlbGeometry			*geo=NULL;
					CGlbMultiPolygon		*mulPoly=NULL;
					polyFeature->GetGeometry((IGlbGeometry**)&geo,0);
					if(geo)					
						mulPoly = dynamic_cast<CGlbMultiPolygon*>(geo);
#if 1 // 原始polygon显示					
					if (mulPoly)
					{
						glbInt32 polyColor = sectionRenderInfo->polyColor->GetValue(polyFeature);
						glbInt32 polyOpacity = sectionRenderInfo->polyOpacity->GetValue(polyFeature);
						datalocate = sectionRenderInfo->polyTextureData->GetValue(polyFeature);
						GlbGlobeTexRepeatModeEnum polyTexRMode = (GlbGlobeTexRepeatModeEnum)sectionRenderInfo->polyTexRepeatMode->GetValue(polyFeature);
						glbDouble polyTexRotation = sectionRenderInfo->polyTextureRotation->GetValue(polyFeature);
						glbInt32 polyTilingU = sectionRenderInfo->polyTilingU->GetValue(polyFeature);
						glbInt32 polyTilingV = sectionRenderInfo->polyTilingV->GetValue(polyFeature);

						CGlbExtent* ext = const_cast<CGlbExtent *>(mulPoly->GetExtent());
						ext->GetCenter(&ptx,&pty,&ptz);				
						if (GLB_GLOBETYPE_GLOBE==globeType)						
							g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(pty),osg::DegreesToRadians(ptx),ptz,ptx,pty,ptz);

						osg::ref_ptr<osg::MatrixTransform> polyTrans = new osg::MatrixTransform;
						osg::Matrix m;
						osg::Vec3d origVec(ptx,pty,ptz);
						m.setTrans(origVec);		
						polyTrans->setMatrix(m);

						polyNode->addChild(polyTrans);

						osg::ref_ptr<osg::Geode> geode = new osg::Geode;
						polyTrans->addChild(geode);
						osg::ref_ptr<osg::Geometry> geom = new osg::Geometry;
						geode->addDrawable(geom);
						osg::ref_ptr<osg::Vec3Array> vtArray = new osg::Vec3Array;
						geom->setVertexArray(vtArray);
						osg::ref_ptr<osg::Vec4Array> clrArray = new osg::Vec4Array;				
						osg::ref_ptr<osg::Vec2Array> texArray = new osg::Vec2Array;
						geom->setTexCoordArray(0,texArray);
						osg::ref_ptr<osg::Vec3Array> normArray = new osg::Vec3Array;
						geom->setNormalArray(normArray);
						geom->setNormalBinding(osg::Geometry::BIND_OVERALL);
						osg::Vec3 nor = CaculatePolyNormal(mulPoly,globeType);
						normArray->push_back(nor);

						glbInt32 polyCnt = mulPoly->GetCount();
						unsigned int startIndex = 0;
						for (glbInt32 idx = 0; idx < polyCnt; idx++)
						{					
							CGlbPolygon* poly = const_cast<CGlbPolygon *>(mulPoly->GetPolygon(idx));
							CGlbLine* extRing = const_cast<CGlbLine *>(poly->GetExtRing());
							glbInt32 ptCnt = extRing->GetCount();
							if (ptCnt<3) continue;

							CGlbExtent* polyExt = const_cast<CGlbExtent*>(poly->GetExtent());

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

							geom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::POLYGON,startIndex,ptCnt));						
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
								geom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::POLYGON,startIndex,ptCnt));									
								startIndex += ptCnt;
							}
						}
						if (datalocate.length()>0)
						{// 纹理文件
							glbInt32 index = datalocate.find_first_of(L'.');
							if(index == 0)
							{// 处理当前执行文件的相对路径情况 ./AAA/....
								CGlbWString execDir = CGlbPath::GetExecDir();
								datalocate = execDir + datalocate.substr(1,datalocate.size());
							}
							osg::Image* texImg = osgDB::readImageFile(datalocate.ToString().c_str());
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
								geom->getOrCreateStateSet()->setTextureAttributeAndModes(0, tex2d, osg::StateAttribute::ON );

								// 纹理旋转
								osg::TexMat* _texmat = new osg::TexMat;
								_texmat->setMatrix(osg::Matrix::rotate(osg::DegreesToRadians(polyTexRotation),0.0f,0.0f,1.0f));					
								geom->getOrCreateStateSet()->setTextureAttributeAndModes(0,_texmat,osg::StateAttribute::ON);								
							}
							else
							{
								GlbLogWOutput(GLB_LOGTYPE_ERR,L"读取剖面(Oid%d)多边形(Oid%d)图像文件(%s)失败. \r\n",feature->GetOid(),polyFeature->GetOid(),datalocate.c_str());	
							}
						}
						else
						{// 颜色
						/*	geom->setColorArray(clrArray);
							osg::Vec4 clr = GetColor(polyColor);
							clr.set(clr.r(),clr.g(),clr.b(),polyOpacity/100.f);
							clrArray->push_back(clr);
							geom->setColorBinding(osg::Geometry::BIND_OVERALL);*/						
						}

						{// 用颜色材质表示颜色
							osg::StateSet* stateset = geom->getOrCreateStateSet();
							osg::Vec4 clr = GetColor(polyColor);
							clr.set(clr.r(),clr.g(),clr.b(),polyOpacity/100.f);							
							osg::ref_ptr<osg::Material> material = new osg::Material;
							material->setDiffuse(osg::Material::FRONT_AND_BACK,clr);
							material->setAlpha(osg::Material::FRONT_AND_BACK,polyOpacity/100.f);
							stateset->setAttribute(material,osg::StateAttribute::ON);
							stateset->setMode(GL_LIGHTING,osg::StateAttribute::ON);

							// 透明效果
							if (polyOpacity<100)
							{// 半透明
								stateset->setMode(GL_BLEND, osg::StateAttribute::ON);			
								if (osg::StateSet::TRANSPARENT_BIN != stateset->getRenderingHint())
									stateset->setRenderingHint( osg::StateSet::TRANSPARENT_BIN );
							}
							else
							{// 不透明
								stateset->setMode(GL_BLEND, osg::StateAttribute::OFF);								
								if (osg::StateSet::OPAQUE_BIN != stateset->getRenderingHint())
									stateset->setRenderingHint( osg::StateSet::OPAQUE_BIN );		
							}							
							stateset->setMode(GL_LIGHTING,osg::StateAttribute::OFF);
							// 打开双面光照
							osg::LightModel* lm = new osg::LightModel;
							lm->setTwoSided(true);
							stateset->setAttributeAndModes(lm,osg::StateAttribute::ON);
						}			

						// 多边形网格化
						if (geom->getNumPrimitiveSets()>0)
						{					
							swPolyNode->addChild(polyNode);

							osg::ref_ptr<osgUtil::Tessellator> tscx=new osgUtil::Tessellator();
							tscx->setTessellationType(osgUtil::Tessellator::TESS_TYPE_GEOMETRY);
							tscx->setBoundaryOnly(false);
							tscx->setWindingType(osgUtil::Tessellator::TESS_WINDING_ODD);
							//osg::Vec3d pointpos(0.0,0.0,1.0);
							tscx->setTessellationNormal(nor);
							tscx->retessellatePolygons(*(geom.get()));
						}				
					}
#else // 三角化显示
					CGlbMultiTin* mtin = NULL;
					try
					{
						mtin = ptr.Triangulate(mulPoly);
						
					}					
					catch (...)
					{
						MessageBox(NULL,L"剖面三角化出错",L"错误",MB_OK);
					}	

					if (mtin)
					{
						osg::Node* node = drawPolyGeom(mtin,obj,polyFeature);
						if (node)
						{
							polyNode->addChild(node);	
							swPolyNode->addChild(polyNode);
						}
					}
#endif
				}				
				cnt++;
			} while (cursor->MoveNext());

			if (swPolyNode->getNumChildren()>0)
			{
				if (swArcNode->getNumChildren()>0)
				{// 如果有弧段,则多边形偏移减少共面
					osg::ref_ptr<osg::PolygonOffset> poffset = new osg::PolygonOffset(-1,-1);
					swPolyNode->getOrCreateStateSet()->setAttributeAndModes(poffset,osg::StateAttribute::ON);
				}				
				//swNode->addChild(swPolyNode);
				swMt->addChild(swPolyNode);
			}
		}	
	}
	// 释放资源
	if (cp!=NULL)
	{
		delete cp;
		cp = NULL;
	}

	if (swNode->getNumChildren()>0)	
	{
		return swNode.release();
	}

	return NULL;
}

osg::Vec2 CGlbGlobeSectionSymbol::CaculateTexCoord(glbDouble ptx,glbDouble pty,glbDouble ptz, CGlbExtent* ext,
									GlbGlobeTexRepeatModeEnum texRMode,glbInt32 tilingU	,glbInt32 tilingV)
{
	osg::Vec2 vt;
	vt.set(0,0);
	glbDouble xMin,xMax,yMin,yMax,zMin,zMax;
	ext->Get(&xMin,&xMax,&yMin,&yMax,&zMin,&zMax);

	float u,v;
	switch (mpr_prjPlaneEnum)
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

CGlbGlobeSectionSymbol::GlbProjPlaneEnum CGlbGlobeSectionSymbol::CaculateProjPlane(CGlbExtent* ext)
{// 投影面积 - XZ平面或YZ平面  不会是XY平面 否则就不叫剖面了	
	glbDouble xLen = ext->GetXWidth();
	glbDouble yLen = ext->GetYHeight();

	if (xLen >= yLen)
		return GLB_PLANE_XZ;
	else
		return GLB_PLANE_YZ;

	return GLB_PLANE_UNKNOWN;
}

osg::Vec3 CGlbGlobeSectionSymbol::CaculatePolyNormal(CGlbMultiPolygon *mulPoly, GlbGlobeTypeEnum type)
{// 取第1,2，3点构成的面计算法线
	if (mulPoly==NULL || mulPoly->GetExtent()==NULL)
		return osg::Vec3(0,0,1);
	glbDouble ptx,pty,ptz;
	glbInt32 polyCnt = mulPoly->GetCount();
	osg::ref_ptr<osg::Vec3Array> vtArray = new osg::Vec3Array;

	CGlbExtent* ext = const_cast<CGlbExtent*>(mulPoly->GetExtent());
	
	glbDouble minx,miny,minz,maxx,maxy,maxz;	
	ext->GetMin(&minx,&miny,&minz);
	ext->GetMax(&maxx,&maxy,&maxz);

	ptx = minx; pty = miny; ptz = minz;
	if (GLB_GLOBETYPE_GLOBE==type)						
		g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(pty),osg::DegreesToRadians(ptx),ptz,ptx,pty,ptz);
	vtArray->push_back(osg::Vec3(ptx,pty,ptz));

	ptx = maxx; pty = maxy; ptz = minz;
	if (GLB_GLOBETYPE_GLOBE==type)						
		g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(pty),osg::DegreesToRadians(ptx),ptz,ptx,pty,ptz);
	vtArray->push_back(osg::Vec3(ptx,pty,ptz));

	ptx = maxx; pty = maxy; ptz = maxz;
	if (GLB_GLOBETYPE_GLOBE==type)						
		g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(pty),osg::DegreesToRadians(ptx),ptz,ptx,pty,ptz);
	vtArray->push_back(osg::Vec3(ptx,pty,ptz));

	//unsigned int startIndex = 0;
	//for (glbInt32 idx = 0; idx < polyCnt; idx++)
	//{					
	//	CGlbPolygon* poly = const_cast<CGlbPolygon *>(mulPoly->GetPolygon(idx));
	//	CGlbLine* extRing = const_cast<CGlbLine *>(poly->GetExtRing());
	//	glbInt32 ptCnt = extRing->GetCount();
	//	if (ptCnt<3) continue;

	//	for (glbInt32 i = 0; i < 3; i++)
	//	{
	//		extRing->GetPoint(i,&ptx,&pty,&ptz);
	//		
	//		if (GLB_GLOBETYPE_GLOBE==type)						
	//			g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(pty),osg::DegreesToRadians(ptx),ptz,ptx,pty,ptz);

	//		vtArray->push_back(osg::Vec3(ptx,pty,ptz));
	//	}
	//}
	
	osg::Vec3 v0 = vtArray->at(0);
	osg::Vec3 v1 = vtArray->at(1);
	osg::Vec3 v2 = vtArray->at(2);
	osg::Vec3 dir0 = v1 - v0;
	osg::Vec3 dir1 = v2 - v1;
	osg::Vec3 nor = dir0^dir1;
	nor.normalize();
	return nor;
}

osg::Node* CGlbGlobeSectionSymbol::drawPolyGeom(CGlbMultiTin* geo, CGlbGlobeRObject *obj, CGlbFeature* polyFeature)
{
	if(geo == NULL || obj == NULL)
		return NULL;
	glbInt32 nTinCnt = geo->GetCount();
	if (nTinCnt < 1)	return NULL;
	GlbGlobeTypeEnum type = obj->GetGlobe()->GetType();

	glbDouble ptx,pty,ptz;
	osg::Vec3d centerPoint;
	geo->GetExtent()->GetCenter(&ptx,&pty,&ptz);
	if(type == GLB_GLOBETYPE_GLOBE)
	{
		g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(pty),osg::DegreesToRadians(ptx),ptz,
			centerPoint.x(),centerPoint.y(),centerPoint.z());
	}
	else
	{
		centerPoint.x() = ptx;
		centerPoint.y() = pty;
		centerPoint.z() = ptz;
	}
	
	GlbSectionRenderInfo*sectionRenderInfo = dynamic_cast<GlbSectionRenderInfo*>(obj->GetRenderInfo());

	glbInt32 polyColor = sectionRenderInfo->polyColor->GetValue(polyFeature);
	glbInt32 polyOpacity = sectionRenderInfo->polyOpacity->GetValue(polyFeature);	
	
	glbInt32 first_ptidx,second_ptidx,third_ptidx;
	//osg::ref_ptr<osg::DrawElementsUInt> indexArray = new osg::DrawElementsUInt(osg::DrawElementsUInt::TRIANGLES,0);
	osg::ref_ptr<osg::Vec3Array> drawPoints = new osg::Vec3Array();
	osg::ref_ptr<osg::Vec3Array> normalArray = new osg::Vec3Array();
	//osg::ref_ptr<osg::UIntArray> indexArray = new osg::UIntArray();
	//osg::ref_ptr<osg::UIntArray> countArray = new osg::UIntArray();	

	osg::Vec3 normal;
	osg::Vec3d drawPoint;
	for (glbInt32 idx = 0; idx < nTinCnt; idx++)
	{
		CGlbTin* pTin = const_cast<CGlbTin*>(geo->GetTin(idx));
		if (pTin==NULL) continue;
		glbInt32 vertexCount = pTin->GetVertexCount();
		glbInt32 triangleCount = pTin->GetTriangleCount();
		if(vertexCount < 1 || triangleCount < 1)
			continue;

		//for(glbInt32 i = 0; i < vertexCount;i++)
		//{
		//	pTin->GetVertex(i,&ptx,&pty,&ptz);		
		//	if(type == GLB_GLOBETYPE_GLOBE)
		//	{
		//		g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(pty),osg::DegreesToRadians(ptx),ptz,
		//			drawPoint.x(),drawPoint.y(),drawPoint.z());
		//	}
		//	else
		//		drawPoint.set(ptx,pty,ptz);
		//	drawPoints->push_back(drawPoint - centerPoint);

		//	//normalArray->push_back(osg::Vec3(0,0,0));
		//	//countArray->push_back(0);
		//}
		
		for(glbInt32 i = 0; i < triangleCount;i++)
		{
			pTin->GetTriangle(i,&first_ptidx,&second_ptidx,&third_ptidx);
			//indexArray->push_back(first_ptidx);
			//indexArray->push_back(second_ptidx);
			//indexArray->push_back(third_ptidx);

			pTin->GetVertex(first_ptidx,&ptx,&pty,&ptz);		
			if(type == GLB_GLOBETYPE_GLOBE)
			{
				g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(pty),osg::DegreesToRadians(ptx),ptz,
					drawPoint.x(),drawPoint.y(),drawPoint.z());
			}
			else		
				drawPoint.set(ptx,pty,ptz);	

			osg::Vec3 vec0(drawPoint.x(),drawPoint.y(),drawPoint.z());
			drawPoints->push_back(vec0 - centerPoint);

			pTin->GetVertex(second_ptidx,&ptx,&pty,&ptz);		
			if(type == GLB_GLOBETYPE_GLOBE)
			{
				g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(pty),osg::DegreesToRadians(ptx),ptz,
					drawPoint.x(),drawPoint.y(),drawPoint.z());
			}
			else		
				drawPoint.set(ptx,pty,ptz);	

			osg::Vec3 vec1(drawPoint.x(),drawPoint.y(),drawPoint.z());
			drawPoints->push_back(vec1 - centerPoint);

			pTin->GetVertex(third_ptidx,&ptx,&pty,&ptz);		
			if(type == GLB_GLOBETYPE_GLOBE)
			{
				g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(pty),osg::DegreesToRadians(ptx),ptz,
					drawPoint.x(),drawPoint.y(),drawPoint.z());
			}
			else		
				drawPoint.set(ptx,pty,ptz);	
			osg::Vec3 vec2(drawPoint.x(),drawPoint.y(),drawPoint.z());
			drawPoints->push_back(vec2 - centerPoint);

			//计算三角形的法线
			normal = (vec1-vec0)^(vec2-vec0);
			normal.normalize(); 

			normalArray->push_back(normal);
			normalArray->push_back(normal);
			normalArray->push_back(normal);

			//{// 法线求平均
			//	normalArray->at(first_ptidx) += normal;
			//	normalArray->at(second_ptidx) += normal;
			//	normalArray->at(third_ptidx) += normal;

			//	countArray->at(first_ptidx)++;
			//	countArray->at(second_ptidx)++;
			//	countArray->at(third_ptidx)++;
			//}		
		}

		//for(glbInt32 k = 0; k < vertexCount; k++)
		//	//normalArray->at(k).normalize();
		//	normalArray->at(k) = normalArray->at(k) / countArray->at(k);
	}	

	//osg::ref_ptr<osg::DrawElementsUInt> primitiveSet = new osg::DrawElementsUInt(osg::DrawElementsUInt::TRIANGLES,indexArray->size(),&indexArray->front());
	osg::ref_ptr<osg::MatrixTransform> mt = new osg::MatrixTransform;
	osg::Matrix m;
	m.makeTranslate(centerPoint);
	mt->setMatrix(m);
	osg::ref_ptr<osg::Geode> geode = new osg::Geode;

	osg::ref_ptr<osg::Geometry> geometry = new osg::Geometry;	
	geometry->setVertexArray(drawPoints.get());
	geometry->setNormalArray(normalArray.get());
	geometry->setNormalBinding(osg::Geometry::BIND_PER_VERTEX);

	osg::Vec4 color = GetColor(polyColor);
	geometry->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::TRIANGLES,0,drawPoints->size()));

	osg::ref_ptr<osg::StateSet> ss = geometry->getOrCreateStateSet();
	osg::ref_ptr<osg::LightModel> lm = new osg::LightModel;
	lm->setTwoSided(true);
	ss->setAttribute(lm.get(),osg::StateAttribute::ON);
	osg::ref_ptr<osg::Material> material = new osg::Material;

	material->setDiffuse(osg::Material::FRONT_AND_BACK,color);
	material->setAlpha(osg::Material::FRONT_AND_BACK,polyOpacity / 100.0);
	ss->setAttribute(material,osg::StateAttribute::ON);
	if(polyOpacity < 100)
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

	//osgUtil::SmoothingVisitor::smooth(*(geometry.get()));
	geode->addDrawable(geometry.get());

	mt->addChild(geode.get());
	//osgDB::writeNodeFile(*geode.get(),"d:/test.osg");
	return mt.release();
}