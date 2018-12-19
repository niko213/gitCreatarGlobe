#include "StdAfx.h"
#include "GlbGlobeLineDynamicArrowSymbol.h"
#include "GlbLine.h"
#include <osg/Geode>
#include "osg/Geometry"
#include "CGlbGlobe.h"
#include <osg/Material>
#include <osg/LineWidth>
#include <osgDB/ReadFile>
#include "GlbGlobeCustomShape.h"
#include "GlbGlobeCustomShapeDrawable.h"

using namespace GlbGlobe;

CGlbGlobeLineDynamicArrowSymbol::CGlbGlobeLineDynamicArrowSymbol(void)
{
}

CGlbGlobeLineDynamicArrowSymbol::~CGlbGlobeLineDynamicArrowSymbol(void)
{
}

osg::Node * GlbGlobe::CGlbGlobeLineDynamicArrowSymbol::Draw( CGlbGlobeRObject *obj,IGlbGeometry *geo )
{
	IGlbGeometry *geometry = geo;
	CGlbLine *line3D = dynamic_cast<CGlbLine*>(geometry);
	if(line3D == NULL)
		return NULL;

	osg::Group *group = new osg::Group;
	CGlbFeature *feature = obj->GetFeature();
	GlbRenderInfo *renderInfo = obj->GetRenderInfo();
	GlbLineDynamicArrowSymbolInfo *dynamicArrowInfo = 
		static_cast<GlbLineDynamicArrowSymbolInfo*>(renderInfo);
	osg::Geode *geode = new osg::Geode;
	osg::ref_ptr<osg::Geometry> lineGeom = new osg::Geometry;
	osg::ref_ptr<osg::Vec3Array> vertexes = new osg::Vec3Array;
	const glbDouble *points = line3D->GetPoints();
	if(obj->GetGlobe()->GetType() == GLB_GLOBETYPE_GLOBE)
	{
		glbDouble xLon = 0.0,yLat = 0.0;
		for(glbInt32 i = 0; i < line3D->GetCount();++i)
		{
			osg::Vec3d point(points[i * 3],points[i * 3 + 1],points[i * 3 + 2]);
			xLon = point.x();
			yLat = point.y();
			if(obj->GetAltitudeMode() == GLB_ALTITUDEMODE_RELATIVETERRAIN)
				point.z() += obj->GetGlobe()->GetElevationAt(xLon,yLat);
			g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(point.y()),
				osg::DegreesToRadians(point.x()),point.z(),point.x(),point.y(),point.z());
			vertexes->push_back(point);
		}
	}
	else if(obj->GetGlobe()->GetType() == GLB_GLOBETYPE_FLAT)
	{
		for (glbInt32 i = 0; i < line3D->GetCount();++i)
		{
			vertexes->push_back(
				osg::Vec3d(points[i * 3],points[i * 3 + 1],points[i * 3 + 2]));
		}
	}

	osg::Vec3dArray *renderVertexes = new osg::Vec3dArray;
	osg::Vec3dArray *normalver = new osg::Vec3dArray;
	normalver->push_back(osg::Vec3d(0.0,0.0,1.0));
	lineGeom->setNormalArray(normalver);
	lineGeom->setNormalBinding(osg::Geometry::BIND_OVERALL);

	osg::Matrixd localToworld;
	osg::Matrixd worldTolocal;
	if(obj->GetGlobe()->GetType() == GLB_GLOBETYPE_GLOBE)
	{
		g_ellipsoidModel->computeLocalToWorldTransformFromXYZ(vertexes->at(0).x(),vertexes->at(0).y(),
			vertexes->at(0).z(),localToworld);
		worldTolocal = osg::Matrixd::inverse(localToworld);
	}
	else
	{
		localToworld.makeTranslate(vertexes->at(0).x(),vertexes->at(0).y(),vertexes->at(0).z());
		worldTolocal = osg::Matrixd::inverse(localToworld);
	}
	glbDouble xOffset = 0.0,yOffset = 0.0,zOffset = 0.0;
	if(dynamicArrowInfo->xOffset)
		xOffset = dynamicArrowInfo->xOffset->GetValue(feature);
	if(dynamicArrowInfo->yOffset)
		yOffset = dynamicArrowInfo->yOffset->GetValue(feature);
	if(dynamicArrowInfo->zOffset)
		zOffset = dynamicArrowInfo->zOffset->GetValue(feature);

	osg::Matrixd offsetMatrix;
	offsetMatrix.makeTranslate(xOffset,yOffset,zOffset);
	for (size_t i = 0; i < vertexes->size(); ++i)
	{
		renderVertexes->push_back(vertexes->at(i)*worldTolocal*offsetMatrix);
	}
	lineGeom->setVertexArray(renderVertexes);
	lineGeom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINE_STRIP,0,renderVertexes->size()));

	osg::StateSet *stateset = geode->getOrCreateStateSet();
	osg::Vec4f dynamicArrowColor = GetColor(dynamicArrowInfo->color->GetValue(feature));
	osg::Material *material = dynamic_cast<osg::Material*>(stateset->getAttribute(osg::StateAttribute::MATERIAL));
	if(!material)
		material = new osg::Material;
	stateset->setAttribute(material,osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
	material->setDiffuse(osg::Material::FRONT_AND_BACK,dynamicArrowColor);
	//material->setEmission(osg::Material::FRONT_AND_BACK,GetColor(dynamicArrowInfo->color->GetValue(feature)));
	//线宽
	osg::LineWidth *lineWidth = new osg::LineWidth;
	lineWidth->setWidth(2.0);
	stateset->setAttributeAndModes(lineWidth,osg::StateAttribute::ON);
	stateset->setMode(GL_DEPTH_TEST,osg::StateAttribute::ON);

	geode->addDrawable(lineGeom);
	if(dynamicArrowInfo->isShowLine->GetValue(feature) == false)
		geode->setNodeMask(false);
	group->addChild(geode);
	CGlbWString modelPath = L"";
	if(dynamicArrowInfo->modelPath != NULL)
		modelPath = dynamicArrowInfo->modelPath->GetValue(feature);
	osg::Node *model = osgDB::readNodeFile(modelPath.ToString());
	//反转顶点
	if(dynamicArrowInfo->isForwardDirection->GetValue(feature) == false)
	{
		reverse(renderVertexes->begin(),renderVertexes->end());
	}
	glbDouble time = dynamicArrowInfo->time->GetValue(feature);
	glbInt32 arrowNum = dynamicArrowInfo->number->GetValue(feature);
	glbBool isModel = false;
	if(model)
	{
		isModel = true;
		osg::ref_ptr<osg::StateSet> ss = model->getOrCreateStateSet();
		ss->setMode( GL_RESCALE_NORMAL/*GL_NORMALIZE*/, osg::StateAttribute::ON );
		glbDouble yaw = 0.0,pitch = 0.0,roll = 0.0,xScale = 1.0,yScale = 1.0,zScale = 1.0;
		if(dynamicArrowInfo->xScale)
			xScale = dynamicArrowInfo->xScale->GetValue(feature);
		if(dynamicArrowInfo->yScale)
			yScale = dynamicArrowInfo->yScale->GetValue(feature);
		if(dynamicArrowInfo->zScale)
			zScale = dynamicArrowInfo->zScale->GetValue(feature);
		if(dynamicArrowInfo->yaw)
			yaw = dynamicArrowInfo->yaw->GetValue(feature);
		if(dynamicArrowInfo->pitch)
			pitch = dynamicArrowInfo->pitch->GetValue(feature);
		if(dynamicArrowInfo->roll)
			roll = dynamicArrowInfo->roll->GetValue(feature);
		osg::Matrixd mScale,mRotate;
		mScale.makeScale(xScale,yScale,zScale);
		yaw = osg::DegreesToRadians(yaw);
		pitch = osg::DegreesToRadians(pitch);
		roll = osg::DegreesToRadians(roll);
		osg::Vec3d yaw_vec(0.0,0.0,1.0);
		osg::Vec3d pitch_vec(cos(yaw),sin(yaw),0.0);
		osg::Vec3d roll_vec(-sin(yaw)*cos(pitch),cos(pitch)*cos(yaw),sin(pitch));
		mRotate.makeRotate(yaw,yaw_vec,pitch,pitch_vec,roll,roll_vec);

		for(glbInt32 i = 0; i < arrowNum;i++)
		{
			double delta_time = time / (arrowNum);
			osg::MatrixTransform *selfMt = new osg::MatrixTransform;
			selfMt->addChild(model);
			selfMt->setMatrix(mScale * mRotate);
			osg::MatrixTransform *pathMt = new osg::MatrixTransform;
			pathMt->addChild(selfMt);
			pathMt->addUpdateCallback(
				CreateAnimationPathCallback(renderVertexes,delta_time * i,delta_time * i + time,isModel));
			pathMt->setName("model");//做标识，修改颜色的时候做区分，因为是模型的时候修改颜色不起作用
			group->addChild(pathMt);
		}
	}
	else
	{
		osg::Vec3d dir;
		glbDouble distance = 0.0;
		glbDouble diameter = dynamicArrowInfo->diameter->GetValue(feature);
		osg::Group *arrowGroup = new osg::Group;
		for(glbInt32 i = 0; i < arrowNum; i++)
		{
			double delta_time = time / (arrowNum);
			osg::MatrixTransform *mt = new osg::MatrixTransform;
			mt->addChild(Draw3DArrow(diameter,diameter * 3,dynamicArrowColor));
			mt->addUpdateCallback(
				CreateAnimationPathCallback(renderVertexes,delta_time * i,delta_time * i + time));
			arrowGroup->addChild(mt);
		}
		group->addChild(arrowGroup);
	}
	return group;
}

IGlbGeometry * GlbGlobe::CGlbGlobeLineDynamicArrowSymbol::GetOutline( CGlbGlobeRObject *obj,IGlbGeometry *geo )
{
	if(obj->GetAltitudeMode() == GLB_ALTITUDEMODE_ONTERRAIN)
	{
		GlbRenderInfo *renderInfo = obj->GetRenderInfo();
		if(renderInfo == NULL)
			return NULL;
		CGlbLine *line3d = dynamic_cast<CGlbLine *>(geo);
		CGlbLine *line = new CGlbLine;
		for(glbInt32 i = 0; i < line3d->GetCount();i++)
		{
			osg::Vec3d point;
			line3d->GetPoint(i,&point.x(),&point.y(),&point.z());
			line->AddPoint(point.x(),point.y());
		}
		mpt_outline = line;
	}
	return mpt_outline.get();
}

osg::Geode * GlbGlobe::CGlbGlobeLineDynamicArrowSymbol::Draw3DArrow(glbDouble radius,glbDouble length,osg::Vec4f color)
{
	glbDouble cylinderRadius = radius / 2;
	glbDouble cylinderHeight = length;
	osg::Geode *geode = new osg::Geode;
	//圆柱
	CustomCylinder *cylinder = new CustomCylinder(osg::Vec3d(0.0,0.0,-cylinderHeight / 2.0),cylinderRadius,cylinderHeight,24);
	osg::ShapeDrawable *shapeCylinderDrawable = new GlobeShapeDrawable(cylinder);
	geode->addDrawable(shapeCylinderDrawable);

	//圆锥
	glbDouble coneRadius = cylinderRadius * 1.5;
	glbDouble coneHeight = cylinderHeight * 0.4;
	osg::ref_ptr<CustomCone> cone = new CustomCone();
	glbDouble offsetFactor = cone->getBaseOffsetFactor();
	glbDouble zTrans = (1 - offsetFactor) * coneHeight * 0.5;//这个式子是根据绘制圆锥的规则推算的
	cone->setCenter(osg::Vec3d(0.0,0.0,-cylinderHeight / 2.0 + cylinderHeight - zTrans));
	cone->setRadius(coneRadius);
	cone->setHeight(coneHeight);
	cone->setEdges(24);
	osg::ref_ptr<osg::TessellationHints> hints = new osg::TessellationHints();
	hints->setDetailRatio(1.0);
	osg::ShapeDrawable *shapeConeDrawable = new GlobeShapeDrawable(cone,hints);
	geode->addDrawable(shapeConeDrawable);

	osg::StateSet *stateset = geode->getOrCreateStateSet();
	osg::Material *material = dynamic_cast<osg::Material*>(stateset->getAttribute(osg::StateAttribute::MATERIAL));
	if(!material)
		material = new osg::Material;
	stateset->setAttribute(material,osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
	material->setDiffuse(osg::Material::FRONT_AND_BACK,color);
	return geode;
}

osg::AnimationPathCallback * GlbGlobe::CGlbGlobeLineDynamicArrowSymbol::CreateAnimationPathCallback( 
	osg::Vec3dArray *points, glbDouble startTime,glbDouble endTime,glbBool isModel )
{
	osg::ref_ptr<osg::AnimationPath> path = new osg::AnimationPath;
	path->setLoopMode( osg::AnimationPath::LOOP );

	unsigned int numSamples = points->size();
	if (numSamples<2)	return NULL;
	glbDouble sumDistance = 0.0;
	osg::DoubleArray *deltaTimeArray = new osg::DoubleArray();
	for(unsigned int i = 0; i < numSamples - 1; ++i)
	{
		sumDistance += ((*points)[i + 1] - (*points)[i]).length();
	}
	//glbDouble delta_time = (endTime - startTime) / (numSamples - 1);
	glbDouble delta_time = 0.0;
	for(unsigned int i = 0; i < numSamples; ++i)
	{
		if(i == 0)
			delta_time = startTime;
		else if(i == numSamples - 1)
			delta_time = endTime;
		else
		{
			delta_time = ((*points)[i] - (*points)[i - 1]).length() / sumDistance * (endTime -startTime) + (*deltaTimeArray)[i - 1];
		}
		deltaTimeArray->push_back(delta_time);
	}

	for ( unsigned int i=0; i<numSamples; ++i )
	{
		osg::Quat rot;osg::Vec3 dir;
		if(i < points->size() - 1)
		{
			dir = (*points)[i + 1] - (*points)[i];dir.normalize();
			if(isModel)
				rot.makeRotate(osg::Y_AXIS,dir);
			else
				rot.makeRotate(osg::Z_AXIS,dir);
			if(i != 0)
			{
				dir = (*points)[i] - (*points)[i - 1];dir.normalize();
				if(isModel)
					rot.makeRotate(osg::Y_AXIS,dir);
				else
					rot.makeRotate(osg::Z_AXIS,dir);
				path->insert( (*deltaTimeArray)[i] - 0.0000001, osg::AnimationPath::ControlPoint(points->at(i),rot,osg::Vec3d(1.0,1.0,1.0)) );
			}
		}
		else
		{
			dir = (*points)[i] - (*points)[i-1]; dir.normalize();
			if(isModel)
				rot.makeRotate(osg::Y_AXIS,dir);
			else
				rot.makeRotate(osg::Z_AXIS,dir);
		}
		path->insert( (*deltaTimeArray)[i], osg::AnimationPath::ControlPoint(points->at(i),rot,osg::Vec3d(1.0,1.0,1.0)) );
		if(i != 0)
		{
			dir = (*points)[i + 1] - (*points)[i];dir.normalize();
			if(isModel)
				rot.makeRotate(osg::Y_AXIS,dir);
			else
				rot.makeRotate(osg::Z_AXIS,dir);
			path->insert( (*deltaTimeArray)[i] + 0.0000001, osg::AnimationPath::ControlPoint(points->at(i),rot,osg::Vec3d(1.0,1.0,1.0)) );
		}
	}

	osg::ref_ptr<osg::AnimationPathCallback> apcb = new osg::AnimationPathCallback;
	apcb->setAnimationPath( path.get() );
	return apcb.release();    
}