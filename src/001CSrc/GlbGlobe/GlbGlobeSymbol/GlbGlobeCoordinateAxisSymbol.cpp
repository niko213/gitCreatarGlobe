#include "StdAfx.h"
#include "GlbGlobeCoordinateAxisSymbol.h"
#include "CGlbGlobe.h"
#include "GlbPoint.h"
#include <osgText/Text>
#include <osg/LineStipple>
#include <osg/LineWidth>

using namespace GlbGlobe;

CGlbGlobeCoordinateAxisSymbol::CGlbGlobeCoordinateAxisSymbol(void)
{
	mpr_globeType = GLB_GLOBETYPE_UNKNOWN;
}


CGlbGlobeCoordinateAxisSymbol::~CGlbGlobeCoordinateAxisSymbol(void)
{
}

/* 生成的场景树结构图
osg::Switch
	|
osg::MatrixTransform
	|
	|
	|osg::geode  ---  (Axis)
	|		| 
	|		-----------------------osg::Geometry
	|		|
	|		-----------------------osgText::Text...
	|		
	|osg::geode  ---  (AxisGrid)
	|		| 
	|		------------------------osg::Geometry
	|		
	|osg::geode  ---  (AxisLabel)
			| 
			------------------------osgText::Text
			| 
			-------------------------osgText::Text.....

	|.........
*/
osg::Node *CGlbGlobeCoordinateAxisSymbol::Draw(CGlbGlobeRObject *obj,IGlbGeometry *geo)
{
	if (obj == NULL)
		return NULL;

	GlbRenderInfo *renderInfo = obj->GetRenderInfo();
	GlbCoordinateAxisSymbolInfo *coordinateAxisRenderInfo = 
		dynamic_cast<GlbCoordinateAxisSymbolInfo *>(renderInfo);
	if(NULL == coordinateAxisRenderInfo)
		return NULL;

	mpr_globeType = obj->GetGlobe()->GetType();

	osg::ref_ptr<osg::Switch> swNode = new osg::Switch;	
	osg::ref_ptr<osg::MatrixTransform> mtNode = new osg::MatrixTransform;
	swNode->addChild(mtNode);
	// geo中存两个点 ，第一个点表示坐标轴的原点位置，第二个点存三个轴的轴长	, 第三个点存三个轴向上的步长
	glbDouble x,y,z;
	x = coordinateAxisRenderInfo->originX;
	y = coordinateAxisRenderInfo->originY;
	z = coordinateAxisRenderInfo->originZ;
	osg::Matrixd localToWorld;
	if (mpr_globeType==GLB_GLOBETYPE_GLOBE)		
		g_ellipsoidModel->computeLocalToWorldTransformFromLatLongHeight(osg::DegreesToRadians(y),osg::DegreesToRadians(x),z,localToWorld);
	else	
		localToWorld.makeTranslate(x,y,z);	

	osg::Vec3d origin(x,y,z);
	osg::Vec3d axisLen(coordinateAxisRenderInfo->axisLengthX,coordinateAxisRenderInfo->axisLengthY,coordinateAxisRenderInfo->axisLengthZ);
	osg::Vec3d axisStep(coordinateAxisRenderInfo->axisStepX,coordinateAxisRenderInfo->axisStepY,coordinateAxisRenderInfo->axisStepZ);

	osg::Matrixd mOffset;
	{
		double xOffset, yOffset,zOffset;		
		xOffset = coordinateAxisRenderInfo->xOffset;		
		yOffset = coordinateAxisRenderInfo->yOffset;		
		zOffset = coordinateAxisRenderInfo->zOffset;	

		if (mpr_globeType==GLB_GLOBETYPE_GLOBE)		
		{
			double longitude = osg::DegreesToRadians(coordinateAxisRenderInfo->originX);
			double latitude = osg::DegreesToRadians(coordinateAxisRenderInfo->originY);
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
		else
			mOffset.makeTranslate(xOffset,yOffset,zOffset);
	}
	// 设置偏移矩阵
	mtNode->setMatrix(localToWorld * mOffset);

	// 放缩控制
	osg::Vec3d axiscale(coordinateAxisRenderInfo->xScale,coordinateAxisRenderInfo->yScale,coordinateAxisRenderInfo->zScale);		 

	// 绘制x,y,z三个坐标轴
	osg::Node* axisNode = buildCoordinateAxis(axisLen,axisStep,axiscale,coordinateAxisRenderInfo->bInvertZ,coordinateAxisRenderInfo->labelSize);
	// 绘制网格线
	osg::Node* gridNode = buildCoordinateAxisGrid(axisLen,axisStep,axiscale,coordinateAxisRenderInfo->bInvertZ);
	// 绘制坐标轴上的刻度
	osg::Node* labelNode = buildCoordinateAxisLabel(origin,axisLen,axisStep,axiscale,coordinateAxisRenderInfo->bInvertZ,coordinateAxisRenderInfo->labelSize);

	if (axisNode)
		mtNode->addChild(axisNode);
	if (gridNode)
	{
		mtNode->addChild(gridNode);
		if (coordinateAxisRenderInfo->bShowGrid==false)
			gridNode->setNodeMask(0x0);
	}
	if (labelNode)
	{
		mtNode->addChild(labelNode);
		if (coordinateAxisRenderInfo->bShowLabel==false)
			labelNode->setNodeMask(0x0);
	}

	return swNode.release();
}

// 绘制x,y,z三个坐标轴 - 像素线，像素点，文字
osg::Node* CGlbGlobeCoordinateAxisSymbol::buildCoordinateAxis(osg::Vec3d axislen, osg::Vec3d axistep, osg::Vec3d axiscale, bool isInvertZ, double labelSize)
{
	osg::Vec3d _scale = axiscale;

	osg::ref_ptr<osg::Geode> geode = new osg::Geode;
	geode->setName("Axis");
	osg::ref_ptr<osg::Geometry> geom = new osg::Geometry;
	osg::ref_ptr<osg::Vec3Array> vertexs = new osg::Vec3Array;
	osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array;
	geom->setVertexArray(vertexs);
	geom->setColorArray(colors);
	geom->setColorBinding(osg::Geometry::BIND_PER_VERTEX);
	geode->addDrawable(geom.get());

	// 绘制3个轴线
	osg::Vec3 xDir(1,0,0);
	osg::Vec3 yDir(0,1,0);
	osg::Vec3 zDir(0,0,1);
	if (axislen.x() > 0)
	{		
		osg::Vec3 xAxisPt = xDir*axislen.x()*_scale.x();
		vertexs->push_back(osg::Vec3(0,0,0));
		vertexs->push_back(xAxisPt);
		colors->push_back(osg::Vec4(1,0,0,1));
		colors->push_back(osg::Vec4(1,0,0,1));
	}
	if (axislen.y() > 0)
	{		
		osg::Vec3 yAxisPt = yDir*axislen.y()*_scale.y();
		vertexs->push_back(osg::Vec3(0,0,0));
		vertexs->push_back(yAxisPt);
		colors->push_back(osg::Vec4(0,1,0,1));
		colors->push_back(osg::Vec4(0,1,0,1));
	}
	if (axislen.z() > 0)
	{		
		osg::Vec3 zAxisPt = zDir*axislen.z()*_scale.z();
		vertexs->push_back(osg::Vec3(0,0,0));
		vertexs->push_back(isInvertZ ? -zAxisPt : zAxisPt);	
		colors->push_back(osg::Vec4(0,0,1,1));
		colors->push_back(osg::Vec4(0,0,1,1));	
	}		

	osg::ref_ptr<osg::UIntArray> pIndexArray = new osg::UIntArray;
	for (int k = 0; k < vertexs->size(); k++)
			pIndexArray->push_back(k);

	//osg::ref_ptr<osg::DrawArrays> da = new osg::DrawArrays(osg::PrimitiveSet::LINES,0,vertexs->size());
	osg::ref_ptr<osg::DrawElementsUInt> deu = new osg::DrawElementsUInt(osg::PrimitiveSet::LINES,pIndexArray->size(),&pIndexArray->front());
	geom->addPrimitiveSet(deu.get());	

	osg::StateSet *stateset = geom->getOrCreateStateSet();
	//stateset->setMode(GL_LIGHTING,osg::StateAttribute::OFF | osg::StateAttribute::PROTECTED);
	osg::ref_ptr<osg::LineWidth> lineWidth = new osg::LineWidth;
	lineWidth->setWidth(3);	//坐标轴线宽默认为3(个像素)
	stateset->setAttributeAndModes(lineWidth,osg::StateAttribute::ON);

	// 绘制x,y,z三个文字
	osg::ref_ptr<osgText::Font> font = new osgText::Font();
	font = osgText::readFontFile("fonts/simkai.ttf");
	if (axislen.x() > 0)
	{
		osg::ref_ptr<osgText::Text> xLabel = new osgText::Text;
		osg::Vec3 xLabelPt = xDir*axislen.x()*_scale.x()*1.05;
		xLabel->setPosition(xLabelPt);
		xLabel->setColor(osg::Vec4(1,0,0,1));
		xLabel->setText("X");
		xLabel->setFont(font.get());
		xLabel->setAlignment(osgText::Text::LEFT_CENTER);
		xLabel->setAxisAlignment(osgText::Text::XY_PLANE);
		xLabel->setName("xAxis");
		xLabel->setCharacterSize(labelSize,0.5f); 
		geode->addDrawable(xLabel);
	}

	if (axislen.y() > 0)
	{
		osg::ref_ptr<osgText::Text> yLabel = new osgText::Text;
		osg::Vec3 yLabelPt = yDir*axislen.y()*_scale.y()*1.05;
		yLabel->setPosition(yLabelPt);
		yLabel->setColor(osg::Vec4(0,1,0,1));
		yLabel->setText("Y");
		yLabel->setFont(font.get());
		yLabel->setAlignment(osgText::Text::LEFT_CENTER);
		yLabel->setAxisAlignment(osgText::Text::XY_PLANE);
		yLabel->setName("yAxis");
		yLabel->setCharacterSize(labelSize,0.5f); 

		geode->addDrawable(yLabel);
	}

	if (axislen.z() > 0)
	{
		osg::ref_ptr<osgText::Text> zLabel = new osgText::Text;
		osg::Vec3 zLabelPt = zDir*axislen.z()*_scale.z()*1.05;
		zLabel->setPosition(isInvertZ ? -zLabelPt : zLabelPt);
		zLabel->setColor(osg::Vec4(0,0,1,1));
		zLabel->setText("Z");
		zLabel->setFont(font.get());
		zLabel->setAlignment(osgText::Text::LEFT_CENTER);
		zLabel->setAxisAlignment(osgText::Text::XZ_PLANE);
		zLabel->setName("zAxis");
		zLabel->setCharacterSize(labelSize,0.5f);

		geode->addDrawable(zLabel);
	}
	geode->getOrCreateStateSet()->setMode(GL_LIGHTING,osg::StateAttribute::OFF | osg::StateAttribute::PROTECTED);
	return geode.release();
}

// 绘制网格线
osg::Node* CGlbGlobeCoordinateAxisSymbol::buildCoordinateAxisGrid(osg::Vec3d axislen, osg::Vec3d axistep, osg::Vec3d axiscale, bool isInvertZ)
{
	osg::Vec3d _scale = axiscale;

	osg::ref_ptr<osg::Geode> geode = new osg::Geode;
	geode->setName("AxisGrid");
	osg::ref_ptr<osg::Geometry> geom = new osg::Geometry;
	geode->addDrawable(geom);
	osg::ref_ptr<osg::Vec3Array> vertexs = new osg::Vec3Array;
	geom->setVertexArray(vertexs);
	osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array;
	colors->push_back(osg::Vec4(0.8,0.8,0.8,1)); // 白色网格线
	geom->setColorArray(colors);
	geom->setColorBinding(osg::Geometry::BIND_OVERALL);
	// 绘制xy平面网格线
	double x,y,z;
	if (axislen.x() > 0)
	{
		//for (x = 0; x <=axislen.x(); x+=axistep.x())
		for (x = 0; x <=axislen.x()*_scale.x(); x+=axistep.x()*_scale.x())
		{
			osg::Vec3 xp_s(x,0,0);
			osg::Vec3 xp_e(x,axislen.y()*_scale.y(),0);
			vertexs->push_back(xp_s);
			vertexs->push_back(xp_e);
		}
	}

	if (axislen.y() > 0)
	{
		for (y = 0; y <= axislen.y()*_scale.y(); y+=axistep.y()*_scale.y())
		{
			osg::Vec3 yp_s(0,y,0);
			osg::Vec3 yp_e(axislen.x()*_scale.x(),y,0);
			vertexs->push_back(yp_s);
			vertexs->push_back(yp_e);
		}
	}

	// 绘制yz平面网格线
	if (axislen.y() > 0)
	{
		for (y = 0; y <= axislen.y()*_scale.y(); y+=axistep.y()*_scale.y())
		{
			osg::Vec3 yp_s(0,y,0);
			osg::Vec3 yp_e(0,y,isInvertZ? -axislen.z()*_scale.z() : axislen.z()*_scale.z());
			vertexs->push_back(yp_s);
			vertexs->push_back(yp_e);
		}
	}

	if (axislen.z() > 0)
	{
		for (z = 0; z <= axislen.z()*_scale.z(); z+=axistep.z()*_scale.z())
		{
			double zz = isInvertZ ? -z : z;
			osg::Vec3 zp_s(0,0,zz);
			osg::Vec3 zp_e(0,axislen.y()*_scale.y(),zz);
			vertexs->push_back(zp_s);
			vertexs->push_back(zp_e);
		}
	}

	// 绘制xz平面网格线
	if (axislen.x() > 0)
	{
		for (x = 0; x <= axislen.x()*_scale.x(); x+=axistep.x()*_scale.x())
		{
			osg::Vec3 xp_s(x,axislen.y()*_scale.y(),0);
			osg::Vec3 xp_e(x,axislen.y()*_scale.y(),isInvertZ? -axislen.z()*_scale.z() : axislen.z()*_scale.z());
			vertexs->push_back(xp_s);
			vertexs->push_back(xp_e);
		}
	}

	if (axislen.z() > 0)
	{
		for (z = 0; z <= axislen.z()*_scale.z(); z+=axistep.z()*_scale.z())
		{
			double zz = isInvertZ ? -z : z;
			osg::Vec3 zp_s(0,						axislen.y()*_scale.y(),	zz);
			osg::Vec3 zp_e(axislen.x()*_scale.x(),	axislen.y()*_scale.y(),	zz);
			vertexs->push_back(zp_s);
			vertexs->push_back(zp_e);
		}
	}

	// 补充一种情况当只有z轴时，z轴上的刻度线位置要绘制出来
	if (axislen.x()<=0 && axislen.y()<=0)
	{
		for (z = 0; z <= axislen.z()*_scale.z(); z+=axistep.z()*_scale.z())
		{
			double zz = isInvertZ ? -z : z;
			osg::Vec3 zp_s(-2.5,0,zz);
			osg::Vec3 zp_e(2.5,0,zz);
			vertexs->push_back(zp_s);
			vertexs->push_back(zp_e);
		}
	}

	osg::ref_ptr<osg::DrawArrays> da = new osg::DrawArrays(osg::PrimitiveSet::LINES,0,vertexs->size());
	geom->addPrimitiveSet(da.get());	

	osg::StateSet *stateset = geom->getOrCreateStateSet();
	//线样式- 点划线 DOT_LINE
	osg::LineStipple *lineStipple = new osg::LineStipple;
	lineStipple->setFactor(1);
	lineStipple->setPattern(0x1C47);
	stateset->setAttributeAndModes(lineStipple,osg::StateAttribute::ON);
	// 禁用光照
	stateset->setMode(GL_LIGHTING,osg::StateAttribute::OFF | osg::StateAttribute::PROTECTED);

	return geode.release();
}

// 绘制坐标轴上的刻度标注
osg::Node* CGlbGlobeCoordinateAxisSymbol::buildCoordinateAxisLabel(osg::Vec3d orig,osg::Vec3d axislen, osg::Vec3d axistep, osg::Vec3d axiscale, bool isInvertZ, double labelSize)
{
	osg::Vec3d _scale = axiscale;

	osg::ref_ptr<osg::Geode> geode = new osg::Geode;
	geode->setName("AxisLabel");
	osg::ref_ptr<osgText::Font> font = new osgText::Font();
	font = osgText::readFontFile("fonts/simkai.ttf");
	char buff[128];
	// 绘制X轴上的刻度标注
	double x,y,z;
	if (axislen.x() > 0)
	{// X轴长度 > 0
		for (x = 0; x <= axislen.x()*_scale.x(); x+=axistep.x()*_scale.x())
		{
			osg::Vec3 pt(x,-5,0);
			osg::ref_ptr<osgText::Text> label = new osgText::Text;
			label->setPosition(pt);
			label->setColor(osg::Vec4(1,1,1,1));
			sprintf_s(buff,"%.0lf",x/_scale.x()+orig.x());
			label->setText(buff);
			label->setFont(font.get());
			label->setAlignment(osgText::Text::CENTER_TOP);
			label->setAxisAlignment(osgText::Text::SCREEN);
			label->setCharacterSize(labelSize,0.5f); 

			geode->addDrawable(label);
		}
	}
	// 绘制Y轴上的刻度标注
	if (axislen.y() > 0)
	{
		for (y = 0; y <= axislen.y()*_scale.y(); y+=axistep.y()*_scale.y())
		{
			osg::Vec3 pt(axislen.x()*_scale.x()+5,y,0);
			osg::ref_ptr<osgText::Text> label = new osgText::Text;
			label->setPosition(pt);
			label->setColor(osg::Vec4(1,1,1,1));
			sprintf_s(buff,"%.0lf",y/_scale.y()+orig.y());
			label->setText(buff);
			label->setFont(font.get());
			label->setAlignment(osgText::Text::LEFT_CENTER);
			label->setAxisAlignment(osgText::Text::SCREEN);
			label->setCharacterSize(labelSize,0.5f); 

			geode->addDrawable(label);
		}
	}	
	// 绘制Z轴上的刻度标注
	if (axislen.z() > 0)
	{
		for (z = 0; z <= axislen.z()*_scale.z(); z+=axistep.z()*_scale.z())
		{
			double zz = isInvertZ ? -z : z;
			osg::Vec3 pt(0,0,zz);

			osg::ref_ptr<osgText::Text> label = new osgText::Text;
			label->setPosition(pt);
			label->setColor(osg::Vec4(1,1,1,1));
			sprintf_s(buff,"%.0lf",zz/_scale.z()+orig.z());
			label->setText(buff);
			label->setFont(font.get());
			label->setAlignment(osgText::Text::RIGHT_CENTER);
			label->setAxisAlignment(osgText::Text::SCREEN);
			label->setCharacterSize(labelSize,0.5f); 

			geode->addDrawable(label);
		}
	}

	osg::StateSet *stateset = geode->getOrCreateStateSet();
	// 禁用光照
	stateset->setMode(GL_LIGHTING,osg::StateAttribute::OFF | osg::StateAttribute::PROTECTED);

	return geode.release();
}