#include "StdAfx.h"
#include "GlbGlobeDrillSymbol.h"
#include "osgDB/ReadFile"
#include "GlbString.h"
#include "GlbDrill.h"
#include "GlbLine.h"
#include "CGlbGlobe.h"
#include <osg/Switch>
#include <osg/LineWidth>
#include <osg/BlendColor>
#include <osg/BlendFunc>
#include <osg/Material>
#include <osg/LightModel>
#include "GlbLog.h"

using namespace GlbGlobe;

CGlbGlobeDrillSymbol::CGlbGlobeDrillSymbol(void)
{
}


CGlbGlobeDrillSymbol::~CGlbGlobeDrillSymbol(void)
{
}
/* 生成的场景树结构图
osg::Switch
	|
osg::MatrixTransform
	|
	|
	|osg::switch  ---  (BaseLine)
	|		| 无模型文件
	|		-----------osg::MatrixTransform-------osg::Geode-------------osg::Geometry
	|		| 模型文件
	|		-----------模型节点
	|		
	|osg::switch  ---  (stratum0)
			| 无模型文件
			------------osg::MatrixTransform------osg::Geode-------------osg::Geometry
			|模型文件
			____________模型节点
	|osg::switch  ---  (stratum1)
			| 无模型文件
			------------osg::MatrixTransform------osg::Geode-------------osg::Geometry
			| 模型文件
			——————模型节点
	|.........
*/
osg::Node *CGlbGlobeDrillSymbol::Draw(CGlbGlobeRObject *obj,IGlbGeometry *geo)
{
	GlbRenderInfo *renderInfo = obj->GetRenderInfo();
	GlbDrillRenderInfo *drillRenderInfo = 
		dynamic_cast<GlbDrillRenderInfo *>(renderInfo);
	if(NULL == drillRenderInfo)
		return NULL;
	
	CGlbFeature *feature = obj->GetFeature();	
	GlbGlobeTypeEnum globeType = obj->GetGlobe()->GetType();
	CGlbWString datalocate = L"";	
	osg::ref_ptr<osg::Switch> swNode = new osg::Switch;	
	osg::ref_ptr<osg::MatrixTransform> swMt = new osg::MatrixTransform;
	swNode->addChild(swMt.get());
	{
		glbDouble xCenter,yCenter;
		feature->GetExtent()->GetCenter(&xCenter,&yCenter);

		osg::Matrix mOffset;

		glbDouble xOffset = 0;
		if (drillRenderInfo->xOffset)
			xOffset = drillRenderInfo->xOffset->GetValue(feature);
		glbDouble yOffset = 0;
		if (drillRenderInfo->yOffset)
			yOffset = drillRenderInfo->yOffset->GetValue(feature);
		glbDouble zOffset = 0;
		if (drillRenderInfo->zOffset)
			zOffset = drillRenderInfo->zOffset->GetValue(feature);			

		if (globeType == GLB_GLOBETYPE_FLAT)
		{
			mOffset.makeTranslate(xOffset,yOffset,zOffset);
		}
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
	osg::ref_ptr<osg::Switch> blSwiNode = NULL;
	//if (drillRenderInfo->isRenderBaseLine)
	{// 绘制中心线
		glbInt32 blColor = RGB(128,25,32);
		if (drillRenderInfo->baselineColor)
			blColor = drillRenderInfo->baselineColor->GetValue(feature);
		glbInt32 blOpacity = 255;
		if (drillRenderInfo->baselineOpacity)
			blOpacity = drillRenderInfo->baselineOpacity->GetValue(feature);
		glbInt32 blWidth = 2;
		if (drillRenderInfo->baselineWidth)
			blWidth = drillRenderInfo->baselineWidth->GetValue(feature);
		if (drillRenderInfo->baselineModelLocate)
			datalocate = drillRenderInfo->baselineModelLocate->GetValue(feature);
		if (drillRenderInfo->baselineModelLocate && datalocate.length()>0)
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
				if (blSwiNode==NULL) blSwiNode = new osg::Switch();
				blSwiNode->setName("BaseLine");
				blSwiNode->addChild(modelNode);
				swMt->addChild(blSwiNode);

				//blSwiNode->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
			}
			else //if (datalocate.length()>0)
			{
				GlbLogWOutput(GLB_LOGTYPE_ERR,L"读取钻孔(Oid:%d) 中心线模型文件(%s)失败. \r\n",feature->GetOid(),datalocate.c_str());
			}
		}
		else
		{// 读取几何数据
			//CGlbGeometryCollect* geo=NULL;
			CGlbMultiLine    * mline=NULL;
			feature->GetGeometry((IGlbGeometry**)&geo,0);
			//if(geo)geo->GetGeometry(0,(IGlbGeometry**)&mline);
			IGlbGeometry* geo=NULL;
			feature->GetGeometry(&geo,0);
			CGlbLine* line = NULL;
			if(geo->GetType()==GlbGeometryTypeEnum::GLB_GEO_MULTILINE)
			{
				mline = dynamic_cast<CGlbMultiLine*>(geo);
				if (mline)
					line = const_cast<CGlbLine*>(mline->GetLine(0));
			}
			else if(geo->GetType()==GlbGeometryTypeEnum::GLB_GEO_LINE)
			{
				line = dynamic_cast<CGlbLine*>(geo);
			}
				
			if (line)
			{//绘制中心线	
				glbInt32 ptCnt = line->GetCount();
				if (ptCnt>1)
				{// 由2个及多个点组成的线
					if (blSwiNode==NULL) blSwiNode = new osg::Switch();
					blSwiNode->setName("BaseLine");
					swMt->addChild(blSwiNode);

					osg::ref_ptr<osg::MatrixTransform> blTrans = new osg::MatrixTransform;
					blSwiNode->addChild(blTrans);
					osg::ref_ptr<osg::Geode> blGeode = new osg::Geode;
					blTrans->addChild(blGeode.get());
					osg::ref_ptr<osg::Geometry> blGeom = new osg::Geometry;
					blGeode->addDrawable(blGeom.get());					

					glbDouble ptx,pty,ptz;
					line->GetPoint(0,&ptx,&pty,&ptz);
					if (GLB_GLOBETYPE_GLOBE==globeType)						
						g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(pty),osg::DegreesToRadians(ptx),ptz,ptx,pty,ptz);

					osg::Matrix m;
					osg::Vec3d origVec(ptx,pty,ptz);
					m.setTrans(origVec);
					blTrans->setMatrix(m);

					osg::ref_ptr<osg::Vec3Array> vtArray = new osg::Vec3Array;
					osg::ref_ptr<osg::Vec4Array> clrArray = new osg::Vec4Array;
					for (glbInt32 k = 0; k < ptCnt; k++)
					{
						line->GetPoint(k,&ptx,&pty,&ptz);						
						if (GLB_GLOBETYPE_GLOBE==globeType)						
							g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(pty),osg::DegreesToRadians(ptx),ptz,ptx,pty,ptz);

						vtArray->push_back(osg::Vec3(ptx,pty,ptz)-origVec);
					}

					blGeom->setVertexArray(vtArray.get());		
					osg::Vec4 cc = GetColor(blColor);
					cc.set(cc.r(),cc.g(),cc.b(),blOpacity/100.0f);

					//clrArray->push_back(cc);
					//blGeom->setColorArray(clrArray);
					//blGeom->setColorBinding(osg::Geometry::BIND_OVERALL);

					blGeom->addPrimitiveSet(
						new osg::DrawArrays(osg::PrimitiveSet::LINE_STRIP,0,vtArray->size()));
						
					//线宽
					osg::LineWidth *lineWidth = new osg::LineWidth;
					lineWidth->setWidth(blWidth);
					osg::StateSet* ss = blSwiNode->getOrCreateStateSet();
					ss->setAttributeAndModes(lineWidth,osg::StateAttribute::ON);

					osg::ref_ptr<osg::Material> material = dynamic_cast<osg::Material*>(ss->getAttribute(osg::StateAttribute::MATERIAL));
					if (!material.valid())
						material = new osg::Material;
					ss->setAttribute(material.get(),osg::StateAttribute::ON);	
					ss->setMode(GL_DEPTH_TEST,osg::StateAttribute::ON);
					material->setDiffuse(osg::Material::FRONT_AND_BACK,cc);
					material->setEmission(osg::Material::FRONT_AND_BACK,cc);

					// 透明度								
					//混合是否启动
					if (blOpacity<100)
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
			}// end if(line)	
		}
	
		if (blSwiNode)
		{
			osg::StateSet* ss = blSwiNode->getOrCreateStateSet();
			//混合是否启动
			if (blOpacity<100)
			{// 启用混合
				ss->setMode(GL_BLEND, osg::StateAttribute::ON);			
				if (osg::StateSet::TRANSPARENT_BIN != ss->getRenderingHint())
					ss->setRenderingHint( osg::StateSet::TRANSPARENT_BIN );
				//osg::BlendColor* bc = dynamic_cast<osg::BlendColor*>(ss->getAttribute(osg::StateAttribute::BLENDCOLOR));
				//osg::BlendFunc* bf = dynamic_cast<osg::BlendFunc*>(ss->getAttribute(osg::StateAttribute::BLENDFUNC));
				//if (!bc)
				//{
				//	bc=new osg::BlendColor(osg::Vec4(1.0f,1.0f,1.0f,(float)blOpacity/ 255));  
				//	ss->setAttributeAndModes(bc,osg::StateAttribute::ON);
				//}
				//else
				//{
				//	bc->setConstantColor(osg::Vec4(1.0f,1.0f,1.0f,(float)blOpacity/ 255));
				//}

				//if (!bf)
				//{
				//	bf = new osg::BlendFunc();			
				//	ss->setAttributeAndModes(bf,osg::StateAttribute::ON);
				//	bf->setSource(osg::BlendFunc::CONSTANT_ALPHA);
				//	bf->setDestination(osg::BlendFunc::ONE_MINUS_CONSTANT_ALPHA);
				//}
			}
			else
			{// 禁用混合
				ss->setMode(GL_BLEND, osg::StateAttribute::OFF  | osg::StateAttribute::OVERRIDE);		
				//osg::BlendColor* bc = dynamic_cast<osg::BlendColor*>(ss->getAttribute(osg::StateAttribute::BLENDCOLOR));
				//if (bc)
				//{
				//	bc=new osg::BlendColor(osg::Vec4(1.0f,1.0f,1.0f,1.0f));  
				//	ss->setAttributeAndModes(bc,osg::StateAttribute::ON);
				//}
				if (osg::StateSet::TRANSPARENT_BIN == ss->getRenderingHint())
					ss->setRenderingHint( osg::StateSet::OPAQUE_BIN );		
			}
		}			
	}

	if (drillRenderInfo->isRenderBaseLine)
		blSwiNode->setAllChildrenOn();
	else
		blSwiNode->setAllChildrenOff();

	CGlbDrill   *drill   = dynamic_cast<CGlbDrill*>(feature);
	// 计算地层基准偏移点origin_vec
	glbDouble cx,cy,cz;
	drill->GetExtent()->GetCenter(&cx,&cy,&cz);
	if (GLB_GLOBETYPE_GLOBE==globeType)						
		g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(cy),osg::DegreesToRadians(cx),cz,cx,cy,cz);
	osg::Vec3d origin_vec(cx,cy,cz);

	//if (drillRenderInfo->isRenderStratum)
	{// 绘制地层		
		glbInt32 stColor = RGB(12,200,62);
		glbInt32 stOpacity = 100;
		glbDouble stWidth = 2;

		glbref_ptr<IGlbFeatureCursor> cursor=NULL;
		cursor = const_cast<IGlbFeatureCursor *>(drill->QueryStratum(NULL));
		if(cursor)
		{
			do
			{
				osg::ref_ptr<osg::Switch> stSwiNode = NULL;
				CGlbFeature * stFeature = cursor->GetFeature();
				if (drillRenderInfo->stratumColor)
					stColor = drillRenderInfo->stratumColor->GetValue(stFeature);

				if (drillRenderInfo->stratumOpacity)
					stOpacity = drillRenderInfo->stratumOpacity->GetValue(stFeature);

				if (drillRenderInfo->stratumWidth)
					stWidth = drillRenderInfo->stratumWidth->GetValue(stFeature);

				// 地层ID号
				//GLBVARIANT val;
				//stFeature->GetValueByName(L"GLBSTRATUMID",&val);
				glbInt32 stId = /*val.iVal*/stFeature->GetOid();

				datalocate.clear();
				if (drillRenderInfo->stratumModelLocate)
					datalocate = drillRenderInfo->stratumModelLocate->GetValue(stFeature);
				if (drillRenderInfo->stratumModelLocate && datalocate.length()>0)
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
						if (stSwiNode==NULL)stSwiNode = new osg::Switch();

						char stname[32];
						sprintf_s(stname,"Stratum%d",stId);
						stSwiNode->setName(stname);
						stSwiNode->addChild(modelNode);
						swMt->addChild(stSwiNode);

						//stSwiNode->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
					}	
					else //if (datalocate.length()>0)
					{						
						GlbLogWOutput(GLB_LOGTYPE_ERR,L"读取钻孔(Oid:%d) 地层(Oid:%d)模型文件(%s)失败. \r\n",feature->GetOid(),stFeature->GetOid(),datalocate.c_str());
					}
				}
				else{
					//CGlbGeometryCollect* geo  = NULL;
					IGlbGeometry* geo = NULL;
					CGlbMultiLine    * mline = NULL;
					stFeature->GetGeometry((IGlbGeometry**)&geo,0);
					GlbGeometryTypeEnum type = geo->GetType();
					CGlbLine* line = NULL;
					if(type==GLB_GEO_MULTILINE)
					{
						mline = dynamic_cast<CGlbMultiLine*>(geo);
						if(mline)line = const_cast<CGlbLine*>(mline->GetLine(0));	
					}
					else if (type==GLB_GEO_LINE)
						line = dynamic_cast<CGlbLine*>(geo);				
								
					if (line)
					{//绘制地层线			
						glbInt32 ptCnt = line->GetCount();
						if (ptCnt>1)
						{// 由2个及多个点组成的线
#if 0// 绘制成像素线
							if (stSwiNode==NULL)stSwiNode = new osg::Switch();
							char stname[32];
							sprintf_s(stname,"Stratum%d",stId);
							stSwiNode->setName(stname);

							swMt->addChild(stSwiNode);

							osg::ref_ptr<osg::MatrixTransform> stTrans = new osg::MatrixTransform;
							stSwiNode->addChild(stTrans);

							osg::ref_ptr<osg::Geode> stGeode = new osg::Geode;
							stTrans->addChild(stGeode.get());
							osg::ref_ptr<osg::Geometry> stGeom = new osg::Geometry;
							stGeode->addDrawable(stGeom.get());
							glbDouble ptx,pty,ptz;
							line->GetPoint(0,&ptx,&pty,&ptz);							
							if (GLB_GLOBETYPE_GLOBE==globeType)						
								g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(pty),osg::DegreesToRadians(ptx),ptz,ptx,pty,ptz);

							osg::Matrix m;
							osg::Vec3d origVec(ptx,pty,ptz);
							m.setTrans(origVec);
							stTrans->setMatrix(m);

							osg::ref_ptr<osg::Vec3Array> vtArray = new osg::Vec3Array;
							osg::ref_ptr<osg::Vec4Array> clrArray = new osg::Vec4Array;
							for (glbInt32 k = 0; k < ptCnt; k++)
							{
								line->GetPoint(k,&ptx,&pty,&ptz);				
								if (GLB_GLOBETYPE_GLOBE==globeType)						
									g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(pty),osg::DegreesToRadians(ptx),ptz,ptx,pty,ptz);

								vtArray->push_back(osg::Vec3(ptx,pty,ptz)-origVec);
							}

							stGeom->setVertexArray(vtArray.get());						
							//clrArray->push_back(GetColor(stColor));
							//stGeom->setColorArray(clrArray);
							//stGeom->setColorBinding(osg::Geometry::BIND_OVERALL);

							stGeom->addPrimitiveSet(
								new osg::DrawArrays(osg::PrimitiveSet::LINE_STRIP,0,vtArray->size()));

							//线宽
							osg::LineWidth *lineWidth = new osg::LineWidth;
							lineWidth->setWidth(stWidth);
							osg::StateSet* ss = stSwiNode->getOrCreateStateSet();
							ss->setAttributeAndModes(lineWidth,osg::StateAttribute::ON | osg::StateAttribute::PROTECTED);

							osg::ref_ptr<osg::Material> material = dynamic_cast<osg::Material*>(ss->getAttribute(osg::StateAttribute::MATERIAL));
							if (!material.valid())
								material = new osg::Material;
							ss->setAttribute(material.get(),osg::StateAttribute::ON);	
							ss->setMode(GL_DEPTH_TEST,osg::StateAttribute::ON);
							osg::Vec4 rColor = GetColor(stColor);
							rColor.set(rColor.r(),rColor.g(),rColor.b(),stOpacity/100.0f);
							material->setDiffuse(osg::Material::FRONT_AND_BACK,rColor);
							//material->setEmission(osg::Material::FRONT_AND_BACK,rColor);
#else // 绘制成圆柱体+中心像素线 2016/7/1 malin							
							bool isGlobe = GLB_GLOBETYPE_GLOBE==globeType?true:false;
							double cylinderRadius = stWidth*0.5;
							osg::Node* node = buildStratumCylinder(line,isGlobe,cylinderRadius,origin_vec);
							stSwiNode = node->asSwitch();
							char stname[32];
							sprintf_s(stname,"Stratum%d",stId);
							stSwiNode->setName(stname);

							swMt->addChild(stSwiNode);

							osg::Node* mtNode = stSwiNode->getChild(0);// 在地层节点下一个matrixtransform节点上设置材质颜色
							osg::StateSet* ss = mtNode->getOrCreateStateSet();
							osg::ref_ptr<osg::Material> material = dynamic_cast<osg::Material*>(ss->getAttribute(osg::StateAttribute::MATERIAL));
							if (!material.valid())
								material = new osg::Material;
							ss->setAttribute(material.get(),osg::StateAttribute::ON);	
							ss->setMode(GL_DEPTH_TEST,osg::StateAttribute::ON);
							ss->setMode(GL_LIGHTING,osg::StateAttribute::ON);
							osg::Vec4 rColor = GetColor(stColor);
							rColor.set(rColor.r(),rColor.g(),rColor.b(),stOpacity/100.0f);
							material->setDiffuse(osg::Material::FRONT_AND_BACK,rColor);
							//material->setAmbient(osg::Material::FRONT_AND_BACK,rColor);
							//material->setSpecular(osg::Material::FRONT_AND_BACK,rColor);
							//material->setEmission(osg::Material::FRONT_AND_BACK,rColor);

							// 双面光照
							osg::ref_ptr<osg::LightModel> lm = new osg::LightModel;
							lm->setTwoSided(true);
							ss->setAttribute(lm.get(), osg::StateAttribute::ON);
#endif
						}
					}// end if(line)	
				}

				if (stSwiNode)
				{
					osg::StateSet* ss = stSwiNode->getOrCreateStateSet();							
				
					//混合是否启动
					if (stOpacity<100)
					{// 启用混合
						ss->setMode(GL_BLEND, osg::StateAttribute::ON);			
						if (osg::StateSet::TRANSPARENT_BIN != ss->getRenderingHint())
							ss->setRenderingHint( osg::StateSet::TRANSPARENT_BIN );
						//osg::BlendColor* bc = dynamic_cast<osg::BlendColor*>(ss->getAttribute(osg::StateAttribute::BLENDCOLOR));
						//osg::BlendFunc* bf = dynamic_cast<osg::BlendFunc*>(ss->getAttribute(osg::StateAttribute::BLENDFUNC));
						//if (!bc)
						//{
						//	bc=new osg::BlendColor(osg::Vec4(1.0f,1.0f,1.0f,(float)stOpacity/ 255));  
						//	ss->setAttributeAndModes(bc,osg::StateAttribute::ON);
						//}
						//else
						//{
						//	bc->setConstantColor(osg::Vec4(1.0f,1.0f,1.0f,(float)stOpacity/ 255));
						//}

						//if (!bf)
						//{
						//	bf = new osg::BlendFunc();			
						//	ss->setAttributeAndModes(bf,osg::StateAttribute::ON);
						//	bf->setSource(osg::BlendFunc::CONSTANT_ALPHA);
						//	bf->setDestination(osg::BlendFunc::ONE_MINUS_CONSTANT_ALPHA);
						//}
					}
					else
					{// 禁用混合						
						ss->setMode(GL_BLEND, osg::StateAttribute::OFF  | osg::StateAttribute::OVERRIDE);	
						//osg::BlendColor* bc = dynamic_cast<osg::BlendColor*>(ss->getAttribute(osg::StateAttribute::BLENDCOLOR));
						//if (bc)
						//{
						//	bc=new osg::BlendColor(osg::Vec4(1.0f,1.0f,1.0f,1.0f));  
						//	ss->setAttributeAndModes(bc,osg::StateAttribute::ON);
						//}
						if (osg::StateSet::TRANSPARENT_BIN == ss->getRenderingHint())
							ss->setRenderingHint( osg::StateSet::OPAQUE_BIN );		
					}
				}

				// 设置开关
				if(stSwiNode)
				{
					if (drillRenderInfo->isRenderStratum)
						stSwiNode->setAllChildrenOn();
					else
						stSwiNode->setAllChildrenOff();
				}

			}while(cursor->MoveNext());
		}
	}	

	if (swNode)	
	{		
		return swNode.release();
	}

	return NULL;
}

osg::Node* CGlbGlobeDrillSymbol::buildStratumCylinder(IGlbGeometry * _ln, bool isGlobe, double cylinderRadius,osg::Vec3d originPos)
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
		computeCylinderVertexs(pt0,pt1,cylinderRadius,cylinderVertexs.get(),cylinderNormals.get());		

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

		geom->setVertexArray(_newcylinderVertexs);

		geom->setNormalArray(cylinderNormals);
		geom->setNormalBinding(osg::Geometry::BIND_PER_VERTEX);
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

void CGlbGlobeDrillSymbol::computeCylinderVertexs(osg::Vec3d pt0, osg::Vec3d pt1, double radius, osg::Vec3dArray* vts, osg::Vec3Array* nors)
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
	}

	vts->push_back(vts->at(0));
	nors->push_back(nors->at(0));
	vts->push_back(vts->at(1));
	nors->push_back(nors->at(1));
}