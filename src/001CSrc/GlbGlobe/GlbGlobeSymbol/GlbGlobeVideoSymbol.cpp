#include "StdAfx.h"
#include "GlbGlobeVideoSymbol.h"
#include "GlbGlobeSymbolCommon.h"
#include "GlbVideoPlayer.h"
#include "CGlbGlobe.h"
#include <osg/Geode>
#include <osg/Geometry>
#include <osg/MatrixTransform>
#include <osgDB/ReadFile>
#include <osg/TexEnv>
#include <osg/Material>
#include <osg/TexGen>
#include "GlbConvert.h"

using namespace GlbGlobe;

CGlbGlobeVideoSymbol::CGlbGlobeVideoSymbol(void)
{
}

CGlbGlobeVideoSymbol::~CGlbGlobeVideoSymbol(void)
{
}

osg::Node* CGlbGlobeVideoSymbol::Draw(CGlbGlobeRObject *obj,IGlbGeometry *geo)
{
	GlbRenderInfo* renderInfo = obj->GetRenderInfo();
	GlbVideoSymbolInfo* videoInfo = static_cast<GlbVideoSymbolInfo*>(renderInfo);
	if (videoInfo == NULL)
		return NULL;

	glbref_ptr<CGlbPolygon> polygon = dynamic_cast<CGlbPolygon *>(geo);
	if(polygon.get() == NULL)
		return NULL;

	switch(videoInfo->symType)
	{
	case GLB_VIDEOSYMBOL_TERRAIN:
		return DrawTerrainVideo(obj,polygon.get());
	case GLB_VIDEOSYMBOL_BILLBOARD:
		return DrawBillBoardVideo(obj,polygon.get());
	case GLB_VIDEOSYMBOL_FREE:
		return DrawBillBoardVideo(obj,polygon.get());
	}
	return NULL;
}

void CGlbGlobeVideoSymbol::DrawToImage(CGlbGlobeRObject *obj,IGlbGeometry* geom, glbByte *image,glbInt32 imageW,glbInt32 imageH,CGlbExtent &ext)
{

}

IGlbGeometry* CGlbGlobeVideoSymbol::GetOutline(CGlbGlobeRObject *obj,IGlbGeometry *geo)
{
	return NULL;
}

osg::Node * CGlbGlobeVideoSymbol::DrawBillBoardVideo( CGlbGlobeRObject *obj,CGlbPolygon *polygon )
{
	GlbRenderInfo* renderInfo = obj->GetRenderInfo();
	GlbVideoSymbolInfo* videoInfo = static_cast<GlbVideoSymbolInfo*>(renderInfo);
	if (!videoInfo)
		return NULL;

	if(polygon == NULL || polygon->IsEmpty() || polygon->GetExtent()==NULL)
		return NULL;

	CGlbLine *pExtRing = const_cast<CGlbLine *>(polygon->GetExtRing());
	if(pExtRing == NULL || pExtRing->IsEmpty())
		return NULL;
	
	glbInt32 dimension = polygon->GetCoordDimension();
	glbDouble height = 0.0;
	osg::ref_ptr<osg::Vec3dArray> points = new osg::Vec3dArray;
	const glbDouble *pCoords = pExtRing->GetPoints();
	glbInt32 pntCnt = pExtRing->GetCount();
	for(glbInt32 i = 0; i < pntCnt;++i)
	{
		if(dimension == 3)
			height = pCoords[i * dimension + 2];
		//if(videoInfo->symType == GLB_VIDEOSYMBOL_TERRAIN)
		//	height = 800.0;
		osg::Vec3d pnt(pCoords[i * dimension],pCoords[i * dimension + 1],height);
		points->push_back(pnt);
	}
	glbUInt32 uniqueCnt = DelaunayTriangulator_uniqueifyPoints(points);
	if(uniqueCnt < 3)
		return NULL;

	//points = orderExtRingPoints(points);
	//if(!points.valid())
	//	return NULL;

	pntCnt = points->size();
	osg::ref_ptr<osg::Geometry> videoGeom = new osg::Geometry();
	videoGeom->setVertexArray(points);
	videoGeom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::POLYGON,0,points->size()));
	//计算多边形中心点位置和高程
	GlbGlobeTypeEnum globeType = obj->GetGlobe()->GetType();
	glbBool isGlobe = GLB_GLOBETYPE_GLOBE == globeType;

	CGlbExtent *pExt = const_cast<CGlbExtent*>(polygon->GetExtent());
	glbDouble cetX,cetY,cetZ;
	pExt->GetCenter(&cetX,&cetY,&cetZ);
	//glbDouble cetX,cetY,cetZ;
	//cetX = points->at(0).x();
	//cetY = points->at(0).y();
	//cetZ = points->at(0).z();
	double elevation = 0.0;
	GlbAltitudeModeEnum altitudeMode = obj->GetAltitudeMode();
	if(altitudeMode == GLB_ALTITUDEMODE_RELATIVETERRAIN)
		elevation = obj->GetGlobe()->GetElevationAt(cetX,cetY);

	osg::Matrixd polygonMT;
	if(isGlobe)
		g_ellipsoidModel->computeLocalToWorldTransformFromLatLongHeight(
		osg::DegreesToRadians(cetY),osg::DegreesToRadians(cetX),cetZ + elevation,polygonMT);
	else
		polygonMT.makeTranslate(cetX,cetY,cetZ + elevation);
	osg::Matrixd polygonMT_inv = osg::Matrixd::inverse(polygonMT);

	osg::ref_ptr<osg::Vec3Array> renderPoints = new osg::Vec3Array;
	glbDouble newX,newY,newZ;
	if(isGlobe)
	{
		if(altitudeMode == GLB_ALTITUDEMODE_ABSOLUTE)
		{
			for(glbInt32 i = 0; i < pntCnt;i++)
			{
				const osg::Vec3d &iPnt = points->at(i);
				g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(iPnt.y()),osg::DegreesToRadians(iPnt.x()),iPnt.z(),
					newX,newY,newZ);
				osg::Vec3d pos(newX,newY,newZ);
				renderPoints->push_back(pos * polygonMT_inv);
			}
		}
		else if(altitudeMode == GLB_ALTITUDEMODE_RELATIVETERRAIN)
		{
			for(glbInt32 i = 0; i < pntCnt; i++)
			{
				const osg::Vec3d &iPnt = points->at(i);
				g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(iPnt.y()),osg::DegreesToRadians(iPnt.x()),iPnt.z()+elevation,
					newX,newY,newZ);
				osg::Vec3d pos(newX,newY,newZ);
				renderPoints->push_back(pos * polygonMT_inv);
			}
		}
	}
	else
	{
		if (altitudeMode == GLB_ALTITUDEMODE_ABSOLUTE)
		{
			for (glbInt32 i = 0; i < pntCnt; i++)
			{
				const osg::Vec3d &iPnt = points->at(i);
				renderPoints->push_back(iPnt * polygonMT_inv);
			}
		}
		else if (altitudeMode == GLB_ALTITUDEMODE_RELATIVETERRAIN)
		{
			for (glbInt32 i = 0; i < pntCnt; i++)
			{
				const osg::Vec3d &iPnt = points->at(i);
				osg::Vec3d pos = iPnt;
				pos.z() += elevation;
				renderPoints->push_back(pos * polygonMT_inv);
			}
		}
	}
	videoGeom->setVertexArray(renderPoints.get());

	osg::ref_ptr<osg::StateSet> geoStateSet = videoGeom->getOrCreateStateSet();

	//纹理坐标的设置，和顶点传进来的顺序以及读出的纹理都有关系。
	osg::ref_ptr<osg::Vec2Array> croodVertexs = new osg::Vec2Array;	//纹理坐标
	croodVertexs->push_back(osg::Vec2(0.0,1.0));
	croodVertexs->push_back(osg::Vec2(1.0,1.0));
	croodVertexs->push_back(osg::Vec2(1.0,0.0));
	croodVertexs->push_back(osg::Vec2(0.0,0.0));
	videoGeom->setTexCoordArray(0,croodVertexs.get());

	osg::ref_ptr<osg::Geode> videoGeode = new osg::Geode;
	videoGeode->addDrawable(videoGeom.get());

	CGlbFeature *feature = obj->GetFeature();
	glbDouble opacity = 100.0,volume = 100.0,
		referenceTime = 0.0,xOffset = 0.0,yOffset = 0.0,zOffset = 0.0,
		xScale = 1.0,yScale = 1.0;
	glbBool bvoice = true,bPause = false,bLoop = true,bRewid = false;
	if (videoInfo->bvoice)
		bvoice = videoInfo->bvoice->GetValue(feature);

	if(videoInfo->bPause)
		bPause = videoInfo->bPause->GetValue(feature);

	if(videoInfo->bLoop)
		bLoop = videoInfo->bLoop->GetValue(feature);

	if(videoInfo->bRewind)
		bRewid = videoInfo->bRewind->GetValue(feature);

	if (videoInfo->opacity)
		opacity = videoInfo->opacity->GetValue(feature);

	if(videoInfo->volume)
		volume = videoInfo->volume->GetValue(feature);

	if(videoInfo->referenceTime)
		referenceTime = videoInfo->referenceTime->GetValue(feature);

	if(videoInfo->xOffset)
		xOffset = videoInfo->xOffset->GetValue(feature);

	if(videoInfo->yOffset)
		yOffset = videoInfo->yOffset->GetValue(feature);

	if(videoInfo->zOffset)
		zOffset = videoInfo->zOffset->GetValue(feature);

	if(videoInfo->xScale)
		xScale = videoInfo->xScale->GetValue(feature);

	if(videoInfo->yScale)
		yScale = videoInfo->yScale->GetValue(feature);

	osg::ref_ptr<VideoPlayer> videoPlayer = dynamic_cast<VideoPlayer *>(obj->GetCustomData());
	if ( videoPlayer.valid() && videoPlayer->getStatus() != osg::ImageStream::PLAYING && videoInfo->videofile )
	{
		CGlbWString videofile = videoInfo->videofile->GetValue(feature);
		if (videofile != L"")
		{
			std::string filePath = videofile.ToString();
			if(videoPlayer.valid())
			{
				videoPlayer->open(filePath);
				videoPlayer->setReferenceTime(referenceTime);
				if(bvoice)
					videoPlayer->setVolume(volume);
				else
					videoPlayer->setVolume(0);

				videoPlayer->play();

				if(bLoop)
					videoPlayer->setLoopingMode(osg::ImageStream::LOOPING);
				else
					videoPlayer->setLoopingMode(osg::ImageStream::NO_LOOPING);
			}
		}
	}

	osg::ref_ptr<osg::Texture2D> tex2d = new osg::Texture2D(videoPlayer.get());
	tex2d->setResizeNonPowerOfTwoHint(false);//??
	tex2d->setFilter(osg::Texture::MIN_FILTER,osg::Texture::LINEAR);
	tex2d->setWrap(osg::Texture::WRAP_S,osg::Texture2D::CLAMP_TO_EDGE);
	tex2d->setWrap(osg::Texture::WRAP_T,osg::Texture2D::CLAMP_TO_EDGE);
	//启用纹理单元0
	geoStateSet->setTextureAttributeAndModes(0,tex2d.get(),osg::StateAttribute::ON);
	//设置纹理环境
	if(videoPlayer->isImageTranslucent())
	{
		geoStateSet->setMode(GL_BLEND,osg::StateAttribute::ON);
		geoStateSet->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
	}

	//材质
	osg::ref_ptr<osg::Material> material = new osg::Material;
	material->setEmission(osg::Material::FRONT_AND_BACK,osg::Vec4(1.0,1.0,1.0,(glbFloat)opacity/100.0));
	material->setDiffuse(osg::Material::FRONT_AND_BACK,osg::Vec4(0.0,0.0,0.0,(glbFloat)opacity/100.0));
	if (opacity < 100)
	{
		geoStateSet->setMode(GL_BLEND,osg::StateAttribute::ON);
		if (geoStateSet->getRenderingHint() != osg::StateSet::TRANSPARENT_BIN)
			geoStateSet->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
	}
	else
	{
		geoStateSet->setMode(GL_BLEND,osg::StateAttribute::OFF);
		geoStateSet->setRenderingHint(osg::StateSet::OPAQUE_BIN);
	}
	geoStateSet->setAttribute(material.get(),osg::StateAttribute::ON);


	osg::ref_ptr<GlbGlobeAutoTransform> autoTransform = NULL;
	if(videoInfo->symType != GLB_VIDEOSYMBOL_TERRAIN)
	{
		autoTransform = new GlbGlobeAutoTransform(obj->GetGlobe());
		autoTransform->addChild(videoGeode.get());
	}
	if(videoInfo->symType == GLB_VIDEOSYMBOL_BILLBOARD)
		autoTransform->setAutoRotateMode(osg::AutoTransform::ROTATE_TO_SCREEN);
	else if(videoInfo->symType == GLB_VIDEOSYMBOL_FREE)// || videoInfo->symType == GLB_VIDEOSYMBOL_TERRAIN)
		autoTransform->setAutoRotateMode(osg::AutoTransform::NO_ROTATION);

	osg::ref_ptr<osg::MatrixTransform> mt = new osg::MatrixTransform;

	osg::Matrix mScale;
	if(videoInfo->xScale && videoInfo->yScale)
		mScale.makeScale(videoInfo->xScale->GetValue(NULL),videoInfo->yScale->GetValue(NULL),1.0);

	osg::Matrix mTranslate;
	if(videoInfo->xOffset && videoInfo->yOffset && videoInfo->zOffset)
		mTranslate.makeTranslate(videoInfo->xOffset->GetValue(NULL),
			videoInfo->yOffset->GetValue(NULL),videoInfo->zOffset->GetValue(NULL));

	mt->setMatrix(mScale * mTranslate * polygonMT);
	if(videoInfo->symType == GLB_VIDEOSYMBOL_TERRAIN)
		mt->addChild(videoGeode.get());
	else
		mt->addChild(autoTransform.get());

	return mt.release();
}

osg::Camera * CGlbGlobeVideoSymbol::initOverLayData( osg::Node *overLayNode,CGlbGlobe *globe )
{
	if(overLayNode == NULL)
		return NULL;
	osg::Camera *camera = new osg::Camera;
	camera->setClearColor(osg::Vec4(0.0,0.0,0.0,0.0));
	camera->setClearMask( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	camera->setReferenceFrame(osg::Camera::ABSOLUTE_RF);//RELATIVE_RF
	camera->setViewport(0,0,1024,1024);
	camera->setComputeNearFarMode(osg::CullSettings::DO_NOT_COMPUTE_NEAR_FAR);
	camera->setRenderOrder(osg::Camera::PRE_RENDER);
	camera->setRenderTargetImplementation(osg::Camera::PIXEL_BUFFER_RTT);
	camera->setRenderTargetImplementation(osg::Camera::PIXEL_BUFFER);
	camera->addChild(overLayNode);

	//方案一:
	osg::BoundingSphere bs;// overLayNode的外包
	for(unsigned int i=0; i<camera->getNumChildren(); ++i)
	{
		bs.expandBy(camera->getChild(i)->getBound());
	}
	//方案二:
	//osg::Geode *geode = overLayNode->asGroup()->getChild(0)->asGeode();
	//osg::BoundingBox bb = geode->getBoundingBox();

	osg::Vec3d eyePoint;
	if(globe->GetType() == GLB_GLOBETYPE_GLOBE)
	{
		osg::Vec3d upDirection(0.0,0.0,1.0);
		osg::Vec3d lookVector(0.0,-1.0,0.0);

		osg::Vec3d center = bs.center();
		//osg::Vec3d center = bb.center() * testMatrix;
		lookVector = center;
		lookVector.normalize();

		double viewDistance = 2.0 * bs.radius();
		//double viewDistance = 2.0 * bb.radius();
		eyePoint = center + lookVector * viewDistance;

		double znear = viewDistance - bs.radius();
		double zfar  = viewDistance + bs.radius();
		float top   = bs.radius();
		//double znear = viewDistance - bb.radius();
		//double zfar  = viewDistance + bb.radius();
		//float top   = bb.radius();
		float right = top;

		camera->setProjectionMatrixAsOrtho(-right,right,-top,top,znear,zfar);
		camera->setViewMatrixAsLookAt(eyePoint,center,upDirection);
	}
	else
	{
		osg::Vec3d upDirection(0.0,1.0,0.0);
		osg::Vec3d viewDirection(0.0,0.0,1.0);

		double viewDistance = 1.5 * bs.radius();
		//double viewDistance = 1.5 * bb.radius();
		osg::Vec3d center = bs.center();
		//osg::Vec3d center = bb.center() * testMatrix;
		eyePoint = center + viewDirection*viewDistance;

		double znear = viewDistance - bs.radius();
		double zfar  = viewDistance + bs.radius();
		float top   = bs.radius();
		//double znear = viewDistance - bb.radius();
		//double zfar  = viewDistance + bb.radius();
		//float top   = bb.radius();
		float right = top;

		camera->setProjectionMatrixAsOrtho(-right,right,-top,top,znear,zfar);
		camera->setViewMatrixAsLookAt(eyePoint,center,upDirection);
	}

	//osg::MatrixTransform *mt = new osg::MatrixTransform;
	//osg::Matrix m;
	//g_ellipsoidModel->computeLocalToWorldTransformFromXYZ(eyePoint.x(),eyePoint.y(),eyePoint.z(),m);
	//mt->setMatrix(m);
	//mt->addChild(camera);

	return camera;
	//return mt;
}

glbBool CGlbGlobeVideoSymbol::createShaders( osg::StateSet *ss,osg::Camera *camera,glbInt32 videoIndex )
{
	osg::ref_ptr<osg::Program> program = dynamic_cast<osg::Program *>(ss->getAttribute(osg::StateAttribute::PROGRAM));
	CGlbString textureNum = "";
	if(!program.valid())
	{
		program = new osg::Program;
		ss->setAttributeAndModes(program.get(),osg::StateAttribute::ON);
		HMODULE hmd = GetModuleHandle(L"GlbGlobe.dll");
		CGlbWString workdir = CGlbPath::GetModuleDir(hmd);
		CGlbWString vertexShaderPath = workdir + L"\\res\\textureVideo.vert";
		CGlbWString fragShaderPath = workdir + L"\\res\\textureVideo.frag";
		osg::ref_ptr<osg::Shader> vertShader = 
			osg::Shader::readShaderFile(osg::Shader::VERTEX,vertexShaderPath.ToString());
		osg::ref_ptr<osg::Shader> fragShader = 
			osg::Shader::readShaderFile(osg::Shader::FRAGMENT,fragShaderPath.ToString());
		program->addShader(vertShader.get());
		program->addShader(fragShader.get());
	}
	osg::ref_ptr<osg::Texture2D> texture = new osg::Texture2D;
	texture->setTextureSize(1024,1024);
	texture->setInternalFormat(GL_RGBA);
	texture->setFilter(osg::Texture2D::MIN_FILTER,osg::Texture2D::LINEAR_MIPMAP_LINEAR);
	texture->setFilter(osg::Texture2D::MAG_FILTER,osg::Texture2D::LINEAR);
	texture->setWrap(osg::Texture2D::WRAP_S,osg::Texture2D::CLAMP_TO_EDGE);
	texture->setWrap(osg::Texture2D::WRAP_T,osg::Texture2D::CLAMP_TO_EDGE);
	texture->setMaxAnisotropy(8.0);
	//texture->setBorderColor(osg::Vec4(1.0,1.0,1.0,1.0));
	camera->attach(osg::Camera::COLOR_BUFFER,texture.get());
	ss->setTextureAttributeAndModes( videoIndex, texture.get(),osg::StateAttribute::ON );

	if (ss->getUniform("texture_0") == NULL)
		ss->addUniform(new osg::Uniform("texture_0",0));

	textureNum= CGlbConvert::Int32ToStr(videoIndex);
	textureNum = "texture_" + textureNum;
	ss->addUniform(new osg::Uniform(textureNum.c_str(),videoIndex));

	osg::Matrixd MVP = camera->getViewMatrix() * camera->getProjectionMatrix();
	osg::Matrixd MVPT = MVP *
		osg::Matrixd::translate(1.0,1.0,1.0) *
		osg::Matrixd::scale(0.5,0.5,0.5);

	osg::BoundingSphere bs;
	for(unsigned int i=0; i<camera->getNumChildren(); ++i)
	{
		bs.expandBy(camera->getChild(i)->getBound());
	}
	textureNum= CGlbConvert::Int32ToStr(videoIndex);
	textureNum = "coortMatrix" + textureNum;
	osg::Uniform* uniformMatrix = ss->getOrCreateUniform(textureNum.c_str(),osg::Uniform::FLOAT_MAT4);
	uniformMatrix->set(MVPT);

	//当前视图计算矩阵起作用，没有这个过滤shader里因为后面的计算会影响当前效果
	textureNum= CGlbConvert::Int32ToStr(videoIndex);
	textureNum = "isWork" + textureNum;
	ss->addUniform(new osg::Uniform(textureNum.c_str(),true));

	//恢复状态
	textureNum= CGlbConvert::Int32ToStr(videoIndex);
	textureNum = "recoverState" + textureNum;
	ss->addUniform(new osg::Uniform(textureNum.c_str(),false));

	return true;
}

osg::Node* CGlbGlobeVideoSymbol::DrawTerrainVideo(CGlbGlobeRObject *obj,CGlbPolygon *polygon)
{
	// 先绘制一个billboard视频以便用rtt相机拍照
	osg::Node *freeNode = DrawBillBoardVideo(obj,polygon);
	CGlbGlobe *globe = obj->GetGlobe();
	osg::Camera *camera = initOverLayData(freeNode,globe);
	osg::ref_ptr<osg::Node> terrainNode = obj->GetGlobe()->GetTerrain()->GetNode();
	glbBool isOK = createShaders(terrainNode->getOrCreateStateSet(),camera,obj->GetVideoIndex());
	//return camera->getParent(0);
	return camera;
}