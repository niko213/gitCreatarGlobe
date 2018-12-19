#include "StdAfx.h"
#include "GlbGlobePolygonLakeSymbol.h"
#include "GlbGlobeSymbolCommon.h"
//#include "GlbGlobeDelaunayTriangulatorEx.h"
#include "GlbGlobeTessellatorEx.h"

#include "GlbPolygon.h"
#include "CGlbGlobe.h"
#include "GlbWString.h"
#include "GlbPath.h"
#include "GlbString.h"
#include "GlbConvert.h"

#include "osgDB/ReadFile"
#include "osg/CoordinateSystemNode"
#include "osg/Geode"
#include "osg/TexEnv"
#include "osg/BlendFunc"
#include "osg/PolygonMode"
#include "osg/LineWidth"
#include "osg/Depth"
#include "osg/Point"
#include "osg/LineStipple"
#include "GlbLog.h"
//#include "comutil.h"

#include "osg/ClipPlane"
#include "osg/ClipNode"
#include "osg/PolygonOffset"
#include "osg/CullFace"
#include "osg/TextureCubeMap"
#include "GlbGlobeView.h"
#include "osg/Material"

#ifdef min
#undef min
#endif

#ifdef max
#undef max
#endif

using namespace  GlbGlobe;

CGlbGlobePolygonLakeSymbol::CGlbGlobePolygonLakeSymbol(void)
{
}

CGlbGlobePolygonLakeSymbol::~CGlbGlobePolygonLakeSymbol(void)
{
}

osg::Node * CGlbGlobePolygonLakeSymbol::Draw(CGlbGlobeRObject *obj,IGlbGeometry *geo)
{
	if (obj==NULL||geo==NULL||obj->GetGlobe()==NULL)
		return NULL;

	GlbPolygonLakeSymbolInfo *polygonLakeInfo= dynamic_cast<GlbPolygonLakeSymbolInfo *>(obj->GetRenderInfo());
	if(polygonLakeInfo == NULL)
		return NULL;

	osg::Node *polygonNode=NULL;
	GlbGeometryTypeEnum geoType = geo->GetType();
	switch(geoType)
	{
	case GLB_GEO_POLYGON:
		{
			glbref_ptr<CGlbPolygon> geoTmp=dynamic_cast<CGlbPolygon *>(geo);
			if (geoTmp == NULL || geoTmp.get()==NULL)
				break;
			polygonNode = buildDrawable(obj,geoTmp.get());
			break;
		}
	case GLB_GEO_MULTIPOLYGON:
		{
			polygonNode = new osg::Group;
			glbref_ptr<CGlbMultiPolygon> multiPoly = dynamic_cast<CGlbMultiPolygon*>(geo);
			if (multiPoly == NULL || multiPoly.get()==NULL)
				break;
			glbInt32 polyCnt = multiPoly->GetCount();
			osg::Node *lakeNode = NULL;
			for (int k = 0; k < polyCnt; k++)
			{
				CGlbPolygon *polygon = const_cast<CGlbPolygon *>(multiPoly->GetPolygon(k));
				lakeNode = buildDrawable(obj,polygon);
				if(lakeNode)
					polygonNode->asGroup()->addChild(lakeNode);
			}
			break;
		}
	default:
		break;		
	}

	return polygonNode;
}

/** geo 不能是collect */
/*
	返回CGlbMultiPolygon 或 CGlbMultiPolygon类型
*/
IGlbGeometry * CGlbGlobePolygonLakeSymbol::GetOutline(CGlbGlobeRObject *obj,IGlbGeometry *geo)
{
	//return CGlbGlobeSymbol::GetOutline(obj,geo);
	if (geo==NULL)
		return NULL;

	// 马林 2014.8.18
	glbInt32 i;

	GlbGeometryTypeEnum geoType=geo->GetType();
	switch(geoType)
	{
	case GLB_GEO_POLYGON:
		{
			glbref_ptr<CGlbMultiPolygon> _outlines = new CGlbMultiPolygon();	
			glbref_ptr<CGlbPolygon> _outline = new CGlbPolygon();

			CGlbPolygon* poly = dynamic_cast<CGlbPolygon*>(geo);
			CGlbLine* extRing = const_cast<CGlbLine *>(poly->GetExtRing());
			glbInt32 ptCnt = extRing->GetCount();
			glbDouble ptx,pty;

			CGlbLine* ln = new CGlbLine;
			_outline->SetExtRing(ln);
			for (i = 0; i < ptCnt; i++)
			{
				extRing->GetPoint(i,&ptx,&pty);
				ln->AddPoint(ptx,pty);
			}

			glbInt32 inRingCnt = poly->GetInRingCount();
			for (i = 0; i < inRingCnt; i++)
			{
				CGlbLine* inRing = const_cast<CGlbLine *>(poly->GetInRing(i));
				ln = new CGlbLine;
				_outline->AddInRing(ln);
				ptCnt = inRing->GetCount();
				for (glbInt32 j = 0; j < ptCnt; j++)
				{
					inRing->GetPoint(i,&ptx,&pty);
					ln->AddPoint(ptx,pty);
				}
			}

			_outlines->AddPolygon(_outline.get());
			mpt_outline = _outlines;
		}
		break;
	case GLB_GEO_MULTIPOLYGON:
		{
			glbref_ptr<CGlbMultiPolygon> _outlines = new CGlbMultiPolygon();	

			CGlbMultiPolygon* multipoly = dynamic_cast<CGlbMultiPolygon*>(geo);
			glbInt32 polyCnt = multipoly->GetCount();
			for(i =0; i < polyCnt; i++)
			{				
				CGlbPolygon* _outline = new CGlbPolygon();
				_outlines->AddPolygon(_outline);

				CGlbPolygon* poly = const_cast<CGlbPolygon *>(multipoly->GetPolygon(i));
				CGlbLine* extRing = const_cast<CGlbLine *>(poly->GetExtRing());
				glbInt32 ptCnt = extRing->GetCount();
				glbDouble ptx,pty;

				CGlbLine* ln = new CGlbLine;
				_outline->SetExtRing(ln);
				for (i = 0; i < ptCnt; i++)
				{
					extRing->GetPoint(i,&ptx,&pty);
					ln->AddPoint(ptx,pty);
				}

				glbInt32 inRingCnt = poly->GetInRingCount();
				for (i = 0; i < inRingCnt; i++)
				{
					CGlbLine* inRing = const_cast<CGlbLine *>(poly->GetInRing(i));

					ln = new CGlbLine;
					_outline->AddInRing(ln);
					ptCnt = inRing->GetCount();
					for (glbInt32 j = 0; j < ptCnt; j++)
					{
						inRing->GetPoint(i,&ptx,&pty);
						ln->AddPoint(ptx,pty);
					}
				}				
			}

			mpt_outline = _outlines;
		}
		break;
	}

	return mpt_outline.get();
}

osg::Camera *CreateOverlayCamera(int texWidth,int texHeight,osg::StateSet *ownerStateSet,unsigned int unit)
{
	osg::ref_ptr<osg::Texture2D> texture = new osg::Texture2D;
	texture->setTextureSize(texWidth,texHeight);
	texture->setInternalFormat(GL_RGBA16F_ARB);
	texture->setSourceFormat(GL_RGBA);
	texture->setSourceFormat(GL_FLOAT);
	texture->setWrap(osg::Texture::WRAP_S,osg::Texture::CLAMP);
	texture->setWrap(osg::Texture::WRAP_T,osg::Texture::CLAMP);
	texture->setFilter(osg::Texture::MIN_FILTER,osg::Texture::LINEAR);
	texture->setFilter(osg::Texture::MAG_FILTER,osg::Texture::LINEAR);

	osg::ref_ptr<osg::Camera> overlayCamera = new osg::Camera;
	overlayCamera->setClearColor(osg::Vec4(0.1f,0.1f,0.1f,1.0f));
	overlayCamera->setClearMask(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	overlayCamera->setRenderOrder(osg::Camera::PRE_RENDER);
	overlayCamera->setRenderTargetImplementation(osg::Camera::FRAME_BUFFER_OBJECT);
	overlayCamera->setViewport(0,0,texWidth,texHeight);

	overlayCamera->attach(osg::Camera::COLOR_BUFFER,texture.get());
	ownerStateSet->setTextureAttributeAndModes(unit,texture.get());
	return overlayCamera.release();
}

osg::Texture *CreateTexture(const std::string &fileName)
{
	osg::ref_ptr<osg::Texture2D> texture = new osg::Texture2D;
	texture->setImage(osgDB::readImageFile(fileName));
	texture->setWrap( osg::Texture2D::WRAP_S, osg::Texture2D::REPEAT );
	texture->setWrap( osg::Texture2D::WRAP_T, osg::Texture2D::REPEAT );
	texture->setFilter( osg::Texture::MIN_FILTER, osg::Texture::LINEAR_MIPMAP_LINEAR );
	texture->setFilter( osg::Texture::MAG_FILTER, osg::Texture::LINEAR );
	return texture.release();
}

osg::Program *CreateProgram(/*const char* vertSrc, const char* fragSrc*/)
{
	//osg::ref_ptr<osg::Program> program = new osg::Program;
	//program->addShader( new osg::Shader(osg::Shader::VERTEX, vertSrc) );
	//program->addShader( new osg::Shader(osg::Shader::FRAGMENT, fragSrc) );
	HMODULE hmd = GetModuleHandle(L"GlbGlobe.dll");
	CGlbWString workdir = CGlbPath::GetModuleDir(hmd);
	CGlbWString lakeVertexShader =  workdir +L"\\res\\lake.vert";
	CGlbWString lakeFragmentShader = workdir +L"\\res\\lake.frag";
	osg::ref_ptr<osg::Program> program = createShaderProgram("lakeWaterShader", lakeVertexShader.ToString(), lakeFragmentShader.ToString(),true);
	return program.release();
}

osg::Vec2dArray *CGlbGlobePolygonLakeSymbol::buildTexCoords(osg::Vec3dArray *points)
{
	osg::Vec3dArray::size_type pntCnt = points->size();
	osg::Vec2dArray *texCoords = new osg::Vec2dArray(pntCnt);
	double dmin_lon=std::numeric_limits<double>::max();
	double dmax_lon=std::numeric_limits<double>::min();
	double dmin_lat=std::numeric_limits<double>::max();
	double dmax_lat=std::numeric_limits<double>::min();
	osg::Vec3dArray::const_iterator iterBegin=points->begin();
	osg::Vec3dArray::const_iterator iterEnd=points->end();
	for (osg::Vec3dArray::const_iterator iter=iterBegin;iter!=iterEnd;++iter)
	{
		if (iter->x()<dmin_lon)
			dmin_lon=iter->x();
		if (iter->x()>dmax_lon)
			dmax_lon=iter->x();
		if (iter->y()<dmin_lat)
			dmin_lat=iter->y();
		if (iter->y()>dmax_lat)
			dmax_lat=iter->y();
	}
	double centX=(dmax_lon+dmin_lon)/2.0;
	double centY=(dmax_lat+dmin_lat)/2.0;//中心
	double sizeX = dmax_lon - dmin_lon;
	double sizeY = dmax_lat - dmin_lat;
	pntCnt = 0;
	for (osg::Vec3dArray::const_iterator iter=iterBegin;iter!=iterEnd;++iter)
	{
		float u = (iter->x() - centX) / sizeX + 0.5;
		float v = (iter->y() - centY) / sizeY + 0.5;
		texCoords->at(pntCnt).set(u,v);
		++pntCnt;
	}
	return texCoords;
}

class ReflectionUpdateCallback : public osg::NodeCallback
{
public:
	ReflectionUpdateCallback(CGlbGlobeView* pview,osg::Matrixd reflectionMatrix)
	{
		mpr_view = pview;
		mpr_reflectionMatrix = reflectionMatrix;
	}
	~ReflectionUpdateCallback()
	{

	}
	void operator()(osg::Node *node,osg::NodeVisitor *nv)
	{
		osg::Matrixd masterMatrix = mpr_view->GetOsgCamera()->getViewMatrix();
		osg::Camera *reflectCamera = dynamic_cast<osg::Camera *>(node);
		reflectCamera->setViewMatrix(mpr_reflectionMatrix * masterMatrix);
		reflectCamera->setProjectionMatrix(mpr_view->GetOsgCamera()->getProjectionMatrix());
	}
private:
	//glbref_ptr<CGlbGlobeView> mpr_view;
	CGlbGlobeView* mpr_view;
	osg::Matrixd                              mpr_reflectionMatrix;
};

osg::Node * CGlbGlobePolygonLakeSymbol::buildDrawable( CGlbGlobeRObject *obj,CGlbPolygon *geo )
{
	GlbRenderInfo *polygonInfo = obj->GetRenderInfo();
	GlbPolygonLakeSymbolInfo *polygonLakeInfo = dynamic_cast<GlbPolygonLakeSymbolInfo *>(polygonInfo);

	CGlbPolygon *pPolyGon = geo;
	if (NULL == pPolyGon || pPolyGon->IsEmpty())
		return NULL;
	glbInt32 dimension = geo->GetCoordDimension();
	CGlbLine *pExtRing = const_cast<CGlbLine *>(pPolyGon->GetExtRing());//多边形外环
	if (NULL==pExtRing||pExtRing->IsEmpty())
		return NULL;

	double height = 0.0;
	// 0提取多边形原始顶点
	osg::ref_ptr<osg::Vec3dArray> points=new osg::Vec3dArray();
	const glbDouble* pCoords=pExtRing->GetPoints();
	glbInt32 pntCnt=pExtRing->GetCount();
	for (glbInt32 i=0;i<pntCnt;++i)
	{
		if(dimension == 3)
			height = pCoords[i*dimension+2];
		osg::Vec3d pnt(pCoords[i*dimension],pCoords[i*dimension+1],height);
		points->push_back(pnt);
	}
	glbUInt32 uniqueCnt=DelaunayTriangulator_uniqueifyPoints(points);// 判断多边形是否有效
	if (uniqueCnt<3)
		return NULL;
	osg::ref_ptr<osg::Geometry> geom=new osg::Geometry();
	geom->setVertexArray(points);
	geom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::POLYGON,0,points->size()));

	// 1计算多边形中心点位置和高程
	GlbGlobeTypeEnum globeTypeE=obj->GetGlobe()->GetType();
	glbBool isGlobe=GLB_GLOBETYPE_GLOBE==globeTypeE;
	CGlbExtent *pExt = const_cast<CGlbExtent*>(pPolyGon->GetExtent());
	glbDouble cetX,cetY,cetZ;
	pExt->GetCenter(&cetX,&cetY,&cetZ);
	double elevation = 0.0;
	GlbAltitudeModeEnum altitudeMode=obj->GetAltitudeMode();
	if (altitudeMode == GLB_ALTITUDEMODE_RELATIVETERRAIN)
	{
		elevation=obj->GetGlobe()->GetElevationAt(cetX,cetY);
	}

	// 2计算多边形朝向
	osg::Matrixd polygonMT;
	osg::EllipsoidModel myEllipsoidModel(g_ellipsoidModel->getRadiusEquator(),g_ellipsoidModel->getRadiusPolar());
	double worldCetX,worldCetY,worldCetZ;
	if (isGlobe)
	{
		myEllipsoidModel.computeLocalToWorldTransformFromLatLongHeight(
			osg::DegreesToRadians(cetY),osg::DegreesToRadians(cetX),cetZ + elevation,polygonMT);
		myEllipsoidModel.convertLatLongHeightToXYZ(
			osg::DegreesToRadians(cetY),osg::DegreesToRadians(cetX),cetZ + elevation,worldCetX,worldCetY,worldCetZ);
	}
	else
	{
		polygonMT.makeTranslate(cetX,cetY,cetZ + elevation);
		worldCetX = cetX;
		worldCetY = cetY;
		worldCetZ = cetZ + elevation;
	}
	osg::Matrixd polygonMT_inv=osg::Matrixd::inverse(polygonMT);// 计算多边形偏移矩阵

	// 3多边形分格化	
	osg::ref_ptr<GlbGlobe::TessellatorEx> tscx=new GlbGlobe::TessellatorEx();
	tscx->setTessellationType(GlbGlobe::TessellatorEx::TESS_TYPE_GEOMETRY);
	tscx->setBoundaryOnly(false);
	tscx->setWindingType(GlbGlobe::TessellatorEx::TESS_WINDING_ODD);
	osg::Vec3d pointpos=osg::Vec3d(0.0,0.0,1.0);
	tscx->setTessellationNormal(pointpos);
	tscx->retessellatePolygons(*geom);
	osg::Vec3dArray *newPoints=(osg::Vec3dArray *)geom->getVertexArray();
	osg::Vec3dArray::size_type newPntCnt= newPoints->size();
	if (newPntCnt<3)
		return NULL;

	osg::Vec2dArray *texcoords = buildTexCoords(newPoints);
	geom->setTexCoordArray(0,texcoords);
	// 4计算多边形顶点	
	osg::ref_ptr<osg::Vec3Array> vertexPoints=new osg::Vec3Array(newPoints->size());
	glbDouble newX,newY,newZ;
	if (isGlobe)
	{
		if (altitudeMode == GLB_ALTITUDEMODE_ABSOLUTE)
		{
			for (osg::Vec3Array::size_type i=0;i<newPntCnt;++i)
			{
				const osg::Vec3d &iPnt=newPoints->at(i);
				myEllipsoidModel.convertLatLongHeightToXYZ(osg::DegreesToRadians(iPnt.y()),osg::DegreesToRadians(iPnt.x()),iPnt.z(),
					newX,newY,newZ);
				osg::Vec3d pos(newX,newY,newZ);
				vertexPoints->at(i)=pos*polygonMT_inv;
			}
		}
		else if (altitudeMode == GLB_ALTITUDEMODE_RELATIVETERRAIN)
		{
			for (osg::Vec3Array::size_type i=0;i<newPntCnt;++i)
			{
				const osg::Vec3d &iPnt=newPoints->at(i);
				myEllipsoidModel.convertLatLongHeightToXYZ(osg::DegreesToRadians(iPnt.y()),osg::DegreesToRadians(iPnt.x()),iPnt.z()+elevation,
					newX,newY,newZ);
				osg::Vec3d pos(newX,newY,newZ);
				vertexPoints->at(i)=pos*polygonMT_inv;
			}
		}
	}
	else
	{
		if (altitudeMode == GLB_ALTITUDEMODE_ABSOLUTE)
		{
			for (osg::Vec3Array::size_type i=0;i<newPntCnt;++i)
			{
				const osg::Vec3d &iPnt = newPoints->at(i);
				vertexPoints->at(i) = iPnt * polygonMT_inv;
			}
		}
		else if (altitudeMode == GLB_ALTITUDEMODE_RELATIVETERRAIN)
		{
			elevation=obj->GetGlobe()->GetElevationAt(cetX,cetY);
			for (osg::Vec3Array::size_type i=0;i<newPntCnt;++i)
			{
				const osg::Vec3d &iPnt = newPoints->at(i);
				osg::Vec3d pos = iPnt;
				pos.z() += elevation;
				vertexPoints->at(i) = pos * polygonMT_inv;
			}
		}
	}	
	geom->setVertexArray(vertexPoints);

	// 5设置材质
	osg::StateSet *sset = geom->getOrCreateStateSet();
	//sset->setAttributeAndModes(new osg::PolygonOffset(1.0,1.0),osg::StateAttribute::ON);
	HMODULE hmd = GetModuleHandle(L"GlbGlobe.dll");
	CGlbWString workdir = CGlbPath::GetModuleDir(hmd);
	CGlbWString UVTexturePath =  workdir +L"\\res\\perlin_noise_water_DUDV.jpg";
	CGlbWString NMTexturePath = workdir +L"\\res\\perlin_noise_water_NM.jpg";
	sset->setTextureAttributeAndModes( 1, CreateTexture(UVTexturePath.ToString()) );
	sset->setTextureAttributeAndModes( 2, CreateTexture(NMTexturePath.ToString()) );
	sset->setAttributeAndModes( CreateProgram(/*waterVert, waterFrag*/) );
	sset->addUniform( new osg::Uniform("reflection", 0) ); //rtt反射 0
	sset->addUniform( new osg::Uniform("refraction", 1) );//折射 1
	sset->addUniform( new osg::Uniform("perlin", 2) );//柏林噪音 2
	glbFloat windStrong = 0.0;
	if(polygonLakeInfo->windStrong)
		windStrong = polygonLakeInfo->windStrong->GetValue(obj->GetFeature());
	sset->addUniform(new osg::Uniform("windStrong",windStrong));
	sset->setMode(GL_LIGHTING,osg::StateAttribute::OFF | osg::StateAttribute::PROTECTED);

	//先把水效添加此处

	float waterLevel = polygonLakeInfo->waterLevel->GetValue(obj->GetFeature());
	//osg::ref_ptr<osg::ClipNode> clipNode = NULL;
	osg::Node *skyNode = obj->GetGlobe()->GetView()->GetGlobeElementNode();

	// 6构建节点层次
	osg::Geode *polygonGeode = new osg::Geode();
	polygonGeode->addDrawable(geom);
	osg::Switch *pPolyGonSW=new osg::Switch();
	pPolyGonSW->addChild(polygonGeode);
	osg::MatrixTransform *mt = new osg::MatrixTransform();
	mt->setMatrix(polygonMT);
	osg::MatrixTransform *pPolyGonMT=mt;
	pPolyGonMT->addChild(pPolyGonSW);

	osg::ref_ptr<osg::Camera> camera = CreateOverlayCamera(1024,1024,sset,0);
	osg::ref_ptr<osg::Geode> pointGeode = NULL;
	osg::Vec3d geodeCenter = osg::Vec3d(worldCetX,worldCetY,worldCetZ);
	osg::BoundingSphere groundNodeBB = obj->GetGlobe()->mpr_p_objects_groundnode->computeBound();
	osg::Vec3d groundNodeBBCenter = groundNodeBB.center();

	osg::Vec3d temp1,temp2;
	if(isGlobe)
	{
		osg::Vec3d iPnt=newPoints->at(0);
		myEllipsoidModel.convertLatLongHeightToXYZ(osg::DegreesToRadians(iPnt.y()),osg::DegreesToRadians(iPnt.x()),iPnt.z(),
			temp1.x(),temp1.y(),temp1.z());
		iPnt = newPoints->at(1);
		myEllipsoidModel.convertLatLongHeightToXYZ(osg::DegreesToRadians(iPnt.y()),osg::DegreesToRadians(iPnt.x()),iPnt.z(),
			temp2.x(),temp2.y(),temp2.z());
	}
	else
	{
		temp1 = newPoints->at(0);
		temp2 = newPoints->at(1);
	}

	//相机不受父节点的任何变换的影响
	camera->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
	//从属相机正面剔除，保证水下相机拍不到地平面
	camera->getOrCreateStateSet()->setAttributeAndModes(
		new osg::CullFace(osg::CullFace::FRONT),osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);

	//参考：组建镜面反射矩阵 http://www.cppblog.com/Leaf/archive/2013/06/15/201017.html
	//组建镜面反射矩阵
	osg::Matrixd _reflectionMatrix;
	osg::Plane lakePlane(osg::Vec3d(worldCetX,worldCetY,worldCetZ),temp1,temp2);
	osg::Vec3d nor = lakePlane.getNormal();
	osg::Vec3d eye,center,up;
	obj->GetGlobe()->GetView()->GetOsgCamera()->getViewMatrixAsLookAt(eye,center,up);
	osg::Vec3d eyeDir = eye - center;
	eyeDir.normalize();
	//求向量夹角
	glbDouble temp = (eyeDir * nor) / (eyeDir.length() * nor.length());
	glbDouble angle = acos(temp);
	temp = osg::RadiansToDegrees(angle);
	double nd = lakePlane[3];
	//调整水面上下场景出现间隙的问题
	if(temp > 90.0)
		nd += 0.3;
	else
		nd -= 0.3;
	_reflectionMatrix.set(1-2*nor.x()*nor.x(),-2*nor.x()*nor.y(), -2*nor.x()*nor.z(), 0,
		-2*nor.y()*nor.x(), 1-2*nor.y()*nor.y(),-2*nor.y()*nor.z(), 0,
		-2*nor.z()*nor.x(), -2*nor.z()*nor.y(), 1-2*nor.z()*nor.z(),0,
		-2*nd*nor.x(),  -2*nd*nor.y(),  -2*nd*nor.z(),  1);
	camera->addUpdateCallback(new ReflectionUpdateCallback(obj->GetGlobe()->GetView(),_reflectionMatrix));

	//为了只拍水面以上设置裁剪面
	osg::ref_ptr<osg::ClipPlane> clipPlane = new osg::ClipPlane();
	//处理裁剪面正面裁剪背面裁剪问题
	if(temp > 90.0)
		nor = -nor;
	osg::Plane cullPlane(nor,temp1);
	clipPlane->setClipPlane(cullPlane);
	osg::ref_ptr<osg::ClipNode> clipNode = new osg::ClipNode();
	clipNode->addClipPlane(clipPlane.get());
	clipNode->addChild(skyNode);
	clipNode->addChild(obj->GetGlobe()->mpr_p_objects_groundnode);
	clipNode->addChild(obj->GetGlobe()->GetTerrain()->GetNode());
	camera->addChild(clipNode.get());

	osg::ref_ptr<osg::Group> polygonGroup=new osg::Group();
	polygonGroup->addChild(pPolyGonMT);
	polygonGroup->addChild(camera.get());
	return polygonGroup.release();
}