#include "StdAfx.h"
#include "GlbGlobeCustomShapeDrawable.h"
#include "GlbGlobeCustomShape.h"
using namespace GlbGlobe;

const unsigned int MIN_NUM_ROWS = 3;
const unsigned int MIN_NUM_SEGMENTS = 5;

class CustomDrawShape
{
public:
	CustomDrawShape(State& state,const TessellationHints* hints):
	  _state(state),
		  _hints(hints)
	  {
	  }
	  void DrawCircleShape(const CustomCircle&);
	  void DrawRectangleShape(const CustomRectangle&);
	  void DrawEllipseShape(const CustomEllipse&);
	  void DrawArcShape(const CustomArc&);
	  void DrawConeShape(const CustomCone&);
	  void DrawCylinderShape(const CustomCylinder&);
	  void DrawPyramidShape(const CustomPyramid&);
	  void DrawPieShape(const CustomPie&);
	  void DrawBoxShape(const CustomBox&);
	  void DrawSphereShape(const CustomSphere&);
	  State&                      _state;
	  const TessellationHints*    _hints;
protected:
	CustomDrawShape& operator = (const CustomDrawShape&) { return *this; }
	void drawCylinderBody( unsigned int numSegments, glbFloat radius, glbFloat height );
	void drawPieBody(unsigned int numSegments, glbFloat radius, glbFloat height,glbFloat sAngle,glbFloat eAngle);
};

void CustomDrawShape::DrawCircleShape( const CustomCircle& circle)
{
	GLBeginEndAdapter& gl = _state.getGLBeginEndAdapter();
	gl.PushMatrix();
	gl.Translated(circle.getCenter().x(),circle.getCenter().y(),circle.getCenter().z());
	gl.Begin(GL_TRIANGLE_FAN);
	gl.Normal3f(0.0f,0.0f,1.0f);
	gl.TexCoord2f(0.5f,0.5f);
	gl.Vertex3f(0.0f,0.0f,0.0f);
	glbFloat angle = 0.0f;
	glbFloat texCoord = 0.0f;
	glbFloat angleDelta = 2.0f * osg::PI / (glbFloat)circle.getEdges();
	glbFloat texCoordDelta = 1.0f / (glbFloat)circle.getEdges();
	glbFloat r = circle.getRadius();
	for (glbInt32 i = 0;
		i < circle.getEdges();
		++i,angle += angleDelta,texCoord += texCoordDelta)
	{
		glbFloat c = cosf(angle);
		glbFloat s = sinf(angle);

		gl.TexCoord2f(c * 0.5f + 0.5f,s * 0.5f + 0.5f);
		gl.Vertex3f(c * r, s * r,0.0);
	}
	gl.TexCoord2f(1.0f,0.5f);
	gl.Vertex3f(r,0.0,0.0);
	gl.End();
	gl.PopMatrix();
}

void CustomDrawShape::DrawRectangleShape( const CustomRectangle& rectangle)
{
	GLBeginEndAdapter& gl = _state.getGLBeginEndAdapter();
	glbFloat dx = rectangle.getWidth() / 2;
	glbFloat dy = rectangle.getHeight() / 2;
	gl.PushMatrix();
	gl.Translated(rectangle.getCenter().x(),rectangle.getCenter().y(),rectangle.getCenter().z());
	gl.Begin(GL_QUADS);
	gl.Normal3f(0.0f,0.0f,1.0f);

	gl.TexCoord2f(0.0f,1.0f);
	gl.Vertex3f(-dx,dy,0.0f);

	gl.TexCoord2f(0.0f,0.0f);
	gl.Vertex3f(-dx,-dy,0.0f);

	gl.TexCoord2f(1.0f,0.0f);
	gl.Vertex3f(dx,-dy,0.0f);

	gl.TexCoord2f(1.0f,1.0f);
	gl.Vertex3f(dx,dy,0.0f);

	gl.End();
	gl.PopMatrix();
}

void CustomDrawShape::DrawEllipseShape( const CustomEllipse& ellipse)
{
	GLBeginEndAdapter& gl = _state.getGLBeginEndAdapter();
	gl.PushMatrix();
	gl.Translated(ellipse.getCenter().x(),ellipse.getCenter().y(),ellipse.getCenter().z());
	gl.Begin(GL_TRIANGLE_FAN);
	gl.Normal3f(0.0f,0.0f,1.0f);
	gl.TexCoord2f(0.5f,0.5f);
	gl.Vertex3f(0.0f,0.0f,0.0f);
	glbFloat angle = 0.0f;
	glbFloat texCoord = 0.0f;
	glbFloat angleDelta = 2.0f * osg::PI / (glbFloat)ellipse.getEdges();
	glbFloat texCoordDelta = 1.0f / (glbFloat)ellipse.getEdges();
	glbFloat xr = ellipse.getXRadius();
	glbFloat yr = ellipse.getYRadius();
	for (glbInt32 i = 0;
		i < ellipse.getEdges();
		++i,angle += angleDelta,texCoord += texCoordDelta)
	{
		glbFloat c = cosf(angle);
		glbFloat s = sinf(angle);

		gl.TexCoord2f(c * 0.5f + 0.5f,s * 0.5f + 0.5f);
		gl.Vertex3f(c * xr, s * yr,0.0);
	}
	gl.TexCoord2f(1.0f,0.5f);
	gl.Vertex3f(xr,0.0,0.0);
	gl.End();
	gl.PopMatrix();
}

void CustomDrawShape::DrawArcShape( const CustomArc& arc)
{
	GLBeginEndAdapter& gl = _state.getGLBeginEndAdapter();
	gl.PushMatrix();
	gl.Translated(arc.getCenter().x(),arc.getCenter().y(),arc.getCenter().z());
	if (arc.getIsFan())
	{
		gl.Begin(GL_TRIANGLE_FAN);
		gl.Normal3f(0.0f,0.0f,1.0f);
		gl.TexCoord2f(0.5f,0.5f);
		gl.Vertex3f(0.0f,0.0f,0.0f);
	}
	else
	{
		gl.Begin(GL_TRIANGLE_FAN);
		gl.Normal3f(0.0f,0.0f,1.0f);
	}
	glbFloat angle = arc.getSAngle();
	glbFloat texCoord = 0.0f;
	glbFloat angleDelta = (arc.getEAngle() - arc.getSAngle()) / (glbFloat)arc.getEdges();
	glbFloat texCoordDelta = 1.0f / (glbFloat)arc.getEdges();
	glbFloat xr = arc.getXRadius();
	glbFloat yr = arc.getYRadius();
	for (glbInt32 i = 0;
		i <= arc.getEdges();
		++i,angle += angleDelta,texCoord += texCoordDelta)
	{
		glbFloat c = cosf(angle);
		glbFloat s = sinf(angle);

		gl.TexCoord2f(c * 0.5f + 0.5f,s * 0.5f + 0.5f);
		gl.Vertex3f(c * xr, s * yr,0.0);
	}
	gl.End();
	gl.PopMatrix();
}

void CustomDrawShape::DrawConeShape( const CustomCone& cone)
{
	GLBeginEndAdapter& gl = _state.getGLBeginEndAdapter();

	gl.PushMatrix();

	gl.Translated(cone.getCenter().x(),cone.getCenter().y(),(cone.getHeight() + cone.getBaseOffset()) * 0.5 + cone.getCenter().z());

	if (!cone.zeroRotation())
	{
		Matrixd rotation(cone.computeRotationMatrix());
		gl.MultMatrixd(rotation.ptr());
	}

	// evaluate hints
	bool createBody = (_hints ? _hints->getCreateBody() : true);
	bool createBottom = (_hints ? _hints->getCreateBottom() : true);

	unsigned int numSegments = cone.getEdges();
	unsigned int numRows = 1;
	glbFloat ratio = (_hints ? _hints->getDetailRatio() : 1.0f);
	if (ratio > 0.0f && ratio != 1.0f) {
		numRows = (unsigned int) (numRows * ratio);
		if (numRows < MIN_NUM_ROWS)
			numRows = MIN_NUM_ROWS;
		numSegments = (unsigned int) (numSegments * ratio);
		if (numSegments < MIN_NUM_SEGMENTS)
			numSegments = MIN_NUM_SEGMENTS;
	}

	glbFloat r = cone.getRadius();
	glbFloat h = cone.getHeight();

	glbFloat normalz = r/(sqrtf(r*r+h*h));
	glbFloat normalRatio = 1.0f/(sqrtf(1.0f+normalz*normalz));
	normalz *= normalRatio;

	glbFloat angleDelta = 2.0f*osg::PI/(glbFloat)numSegments;
	glbFloat texCoordHorzDelta = 1.0/(glbFloat)numSegments;
	glbFloat texCoordRowDelta = 1.0/(glbFloat)numRows;
	glbFloat hDelta = cone.getHeight()/(glbFloat)numRows;
	glbFloat rDelta = cone.getRadius()/(glbFloat)numRows;

	glbFloat topz=cone.getHeight()+cone.getBaseOffset();
	glbFloat topr=0.0f;
	glbFloat topv=1.0f;
	glbFloat basez=topz-hDelta;
	glbFloat baser=rDelta;
	glbFloat basev=topv-texCoordRowDelta;
	glbFloat angle;
	glbFloat texCoord;

	if (createBody) {
		for(unsigned int rowi=0; rowi<numRows;
			++rowi,topz=basez, basez-=hDelta, topr=baser, baser+=rDelta, topv=basev, basev-=texCoordRowDelta) {
				// we can't use a fan for the cone top
				// since we need different normals at the top
				// for each face..
				gl.Begin(GL_QUAD_STRIP);

				angle = 0.0f;
				texCoord = 0.0f;
				for(unsigned int topi=0; topi<numSegments;
					++topi,angle+=angleDelta,texCoord+=texCoordHorzDelta) {

						glbFloat c = cosf(angle);
						glbFloat s = sinf(angle);

						gl.Normal3f(c*normalRatio,s*normalRatio,normalz);

						gl.TexCoord2f(texCoord,topv);
						gl.Vertex3f(c*topr,s*topr,topz);

						gl.TexCoord2f(texCoord,basev);
						gl.Vertex3f(c*baser,s*baser,basez);
				}

				// do last point by hand to ensure no round off errors.
				gl.Normal3f(normalRatio,0.0f,normalz);

				gl.TexCoord2f(1.0f,topv);
				gl.Vertex3f(topr,0.0f,topz);

				gl.TexCoord2f(1.0f,basev);
				gl.Vertex3f(baser,0.0f,basez);

				gl.End();
		}
	}

	if (createBottom) {
		gl.Begin(GL_TRIANGLE_FAN);

		angle = osg::PI*2.0f;
		texCoord = 1.0f;
		basez = cone.getBaseOffset();

		gl.Normal3f(0.0f,0.0f,-1.0f);
		gl.TexCoord2f(0.5f,0.5f);
		gl.Vertex3f(0.0f,0.0f,basez);

		for(unsigned int bottomi=0;bottomi<numSegments;
			++bottomi,angle-=angleDelta,texCoord-=texCoordHorzDelta) {

				glbFloat c = cosf(angle);
				glbFloat s = sinf(angle);

				gl.TexCoord2f(c*0.5f+0.5f,s*0.5f+0.5f);
				gl.Vertex3f(c*r,s*r,basez);
		}

		gl.TexCoord2f(1.0f,0.5f);
		gl.Vertex3f(r,0.0f,basez);

		gl.End();
	}

	gl.PopMatrix();
}

void CustomDrawShape::DrawCylinderShape( const CustomCylinder& cylinder)
{
	GLBeginEndAdapter& gl = _state.getGLBeginEndAdapter();

	gl.PushMatrix();

	gl.Translated(cylinder.getCenter().x(),cylinder.getCenter().y(),cylinder.getCenter().z());

	if (!cylinder.zeroRotation())
	{
		Matrixd rotation(cylinder.computeRotationMatrix());
		gl.MultMatrixd(rotation.ptr());
	}

	// evaluate hints
	bool createBody = (_hints ? _hints->getCreateBody() : true);
	bool createTop = (_hints ? _hints->getCreateTop() : true);
	bool createBottom = (_hints ? _hints->getCreateBottom() : true);

	unsigned int numSegments = cylinder.getEdges();
	glbFloat ratio = (_hints ? _hints->getDetailRatio() : 1.0f);
	if (ratio > 0.0f && ratio != 1.0f) {
		numSegments = (unsigned int) (numSegments * ratio);
		if (numSegments < MIN_NUM_SEGMENTS)
			numSegments = MIN_NUM_SEGMENTS;
	}

	// cylinder body
	if (createBody) 
		drawCylinderBody(numSegments, cylinder.getRadius(), cylinder.getHeight());

	glbFloat angleDelta = 2.0f*osg::PI/(glbFloat)numSegments;
	glbFloat texCoordDelta = 1.0f/(glbFloat)numSegments;

	glbFloat r = cylinder.getRadius();
	glbFloat h = cylinder.getHeight();

	glbFloat basez = 0/*-h*0.5f*/;
	glbFloat topz = h/**0.5f*/;

	glbFloat angle = 0.0f;
	glbFloat texCoord = 0.0f;

	// cylinder top
	if (createTop) {
		gl.Begin(GL_TRIANGLE_FAN);

		gl.Normal3f(0.0f,0.0f,1.0f);
		gl.TexCoord2f(0.5f,0.5f);
		gl.Vertex3f(0.0f,0.0f,topz);

		angle = 0.0f;
		texCoord = 0.0f;
		for(unsigned int topi=0;
			topi<numSegments;
			++topi,angle+=angleDelta,texCoord+=texCoordDelta)
		{
			glbFloat c = cosf(angle);
			glbFloat s = sinf(angle);

			gl.TexCoord2f(c*0.5f+0.5f,s*0.5f+0.5f);
			gl.Vertex3f(c*r,s*r,topz);
		}

		gl.TexCoord2f(1.0f,0.5f);
		gl.Vertex3f(r,0.0f,topz);

		gl.End();
	}

	// cylinder bottom
	if (createBottom)
	{
		gl.Begin(GL_TRIANGLE_FAN);

		gl.Normal3f(0.0f,0.0f,-1.0f);
		gl.TexCoord2f(0.5f,0.5f);
		gl.Vertex3f(0.0f,0.0f,basez);

		angle = osg::PI*2.0f;
		texCoord = 1.0f;
		for(unsigned int bottomi=0;
			bottomi<numSegments;
			++bottomi,angle-=angleDelta,texCoord-=texCoordDelta)
		{
			glbFloat c = cosf(angle);
			glbFloat s = sinf(angle);

			gl.TexCoord2f(c*0.5f+0.5f,s*0.5f+0.5f);
			gl.Vertex3f(c*r,s*r,basez);
		}

		gl.TexCoord2f(1.0f,0.5f);
		gl.Vertex3f(r,0.0f,basez);

		gl.End();
	}

	gl.PopMatrix();
}

void CustomDrawShape::drawCylinderBody( unsigned int numSegments, glbFloat radius, glbFloat height )
{
	const glbFloat angleDelta = 2.0f*osg::PI/(glbFloat)numSegments;
	const glbFloat texCoordDelta = 1.0f/(glbFloat)numSegments;

	const glbFloat r = radius;
	const glbFloat h = height;

	glbFloat basez = 0/*-h*0.5f*/;
	glbFloat topz = h/**0.5f*/;

	glbFloat angle = 0.0f;
	glbFloat texCoord = 0.0f;

	bool drawFrontFace = _hints ? _hints->getCreateFrontFace() : true;
	bool drawBackFace = _hints ? _hints->getCreateBackFace() : false;

	// The only difference between the font & back face loops is that the
	//  normals are inverted and the order of the vertex pairs is reversed.
	//  The code is mostly duplicated in order to hoist the back/front face 
	//  test out of the loop for efficiency

	GLBeginEndAdapter& gl = _state.getGLBeginEndAdapter();

	gl.Begin(GL_QUAD_STRIP);

	if (drawFrontFace) {

		for(unsigned int bodyi=0;
			bodyi<numSegments;
			++bodyi,angle+=angleDelta,texCoord+=texCoordDelta)
		{
			glbFloat c = cosf(angle);
			glbFloat s = sinf(angle);

			gl.Normal3f(c,s,0.0f);

			gl.TexCoord2f(texCoord,1.0f);
			gl.Vertex3f(c*r,s*r,topz);

			gl.TexCoord2f(texCoord,0.0f);
			gl.Vertex3f(c*r,s*r,basez);
		}

		// do last point by hand to ensure no round off errors.
		gl.Normal3f(1.0f,0.0f,0.0f);

		gl.TexCoord2f(1.0f,1.0f);
		gl.Vertex3f(r,0.0f,topz);

		gl.TexCoord2f(1.0f,0.0f);
		gl.Vertex3f(r,0.0f,basez);
	}

	if (drawBackFace) {
		for(unsigned int bodyi=0;
			bodyi<numSegments;
			++bodyi,angle+=angleDelta,texCoord+=texCoordDelta)
		{
			glbFloat c = cosf(angle);
			glbFloat s = sinf(angle);

			gl.Normal3f(-c,-s,0.0f);

			gl.TexCoord2f(texCoord,0.0f);
			gl.Vertex3f(c*r,s*r,basez);

			gl.TexCoord2f(texCoord,1.0f);
			gl.Vertex3f(c*r,s*r,topz);
		}

		// do last point by hand to ensure no round off errors.
		gl.Normal3f(-1.0f,0.0f,0.0f);

		gl.TexCoord2f(1.0f,0.0f);
		gl.Vertex3f(r,0.0f,basez);

		gl.TexCoord2f(1.0f,1.0f);
		gl.Vertex3f(r,0.0f,topz);
	}

	gl.End();
}

void CustomDrawShape::DrawPyramidShape( const CustomPyramid& pyramid)
{
	GLBeginEndAdapter& gl = _state.getGLBeginEndAdapter();
	glbFloat halfLength = pyramid.getLength() / 2;
	glbFloat halfWidth = pyramid.getWidth() / 2;
	glbFloat height = pyramid.getHeight();
	gl.PushMatrix();
	gl.Translated(pyramid.getCenter().x(),pyramid.getCenter().y(),pyramid.getCenter().z());
	gl.Begin(GL_TRIANGLES);

	//-  y   plane
	Vec3d dir = (Vec3d(halfWidth,-halfLength,0) - Vec3d(-halfWidth,-halfLength,0))
		^ (Vec3d(0.0,0.0,height) - Vec3d(-halfWidth,-halfLength,0));
	gl.Normal3f(dir.x(),dir.y(),dir.z());
	gl.TexCoord2f(0.5,1);
	gl.Vertex3f(0.0,0.0,height);

	gl.TexCoord2f(0.0,0.0);
	gl.Vertex3f(-halfWidth,-halfLength,0);

	gl.TexCoord2f(1.0,0.0);
	gl.Vertex3f(halfWidth,-halfLength,0);

	// + y plane
	Vec3d dir2 = (Vec3d(-halfWidth,halfLength,0) - Vec3d(halfWidth,halfLength,0))
		^ (Vec3d(0.0,0.0,height) - Vec3d(halfWidth,halfLength,0));
	gl.Normal3f(dir2.x(),dir2.y(),dir2.z());
	gl.TexCoord2f(0.5,1);
	gl.Vertex3f(0.0,0.0,height);

	gl.TexCoord2f(0.0,0.0);
	gl.Vertex3f(halfWidth,halfLength,0);

	gl.TexCoord2f(1.0,0.0);
	gl.Vertex3f(-halfWidth,halfLength,0);

	// + x plane
	Vec3d dir3 = (Vec3d(halfWidth,halfLength,0) - Vec3d(halfWidth,-halfLength,0))
		^ (Vec3d(0.0,0.0,height) - Vec3d(halfWidth,-halfLength,0));
	gl.Normal3f(dir3.x(),dir3.y(),dir3.z());
	gl.TexCoord2f(0.5,1);
	gl.Vertex3f(0.0,0.0,height);

	gl.TexCoord2f(0.0,0.0);
	gl.Vertex3f(halfWidth,-halfLength,0);

	gl.TexCoord2f(1.0,0.0);
	gl.Vertex3f(halfWidth,halfLength,0);

	// - x plane
	Vec3d dir4 = (Vec3d(-halfWidth,-halfLength,0) - Vec3d(-halfWidth,halfLength,0))
		^ (Vec3d(0.0,0.0,height) - Vec3d(-halfWidth,halfLength,0));
	gl.Normal3f(dir4.x(),dir4.y(),dir4.z());
	gl.TexCoord2f(0.5,1);
	gl.Vertex3f(0.0,0.0,height);

	gl.TexCoord2f(0.0,0.0);
	gl.Vertex3f(-halfWidth,halfLength,0);

	gl.TexCoord2f(1.0,0.0);
	gl.Vertex3f(-halfWidth,-halfLength,0);

	gl.End();
	gl.Begin(GL_QUADS);

	//createBottom
	gl.Normal3f(0.0,0.0,-1.0);

	gl.TexCoord2f(0.0,1.0);
	gl.Vertex3f(-halfWidth,halfLength,0);

	gl.TexCoord2f(0.0,0.0);
	gl.Vertex3f(-halfWidth,-halfLength,0);

	gl.TexCoord2f(1.0,0.0);
	gl.Vertex3f(halfWidth,-halfLength,0);

	gl.TexCoord2f(1.0,1.0);
	gl.Vertex3f(halfWidth,halfLength,0);

	gl.End();
	gl.PopMatrix();
}

void CustomDrawShape::DrawPieShape( const CustomPie& pie)
{
	GLBeginEndAdapter& gl = _state.getGLBeginEndAdapter();

	gl.PushMatrix();

	gl.Translated(pie.getCenter().x(),pie.getCenter().y(),pie.getCenter().z());

	if (!pie.zeroRotation())
	{
		Matrixd rotation(pie.computeRotationMatrix());
		gl.MultMatrixd(rotation.ptr());
	}

	// evaluate hints
	bool createBody = (_hints ? _hints->getCreateBody() : true);
	bool createTop = (_hints ? _hints->getCreateTop() : true);
	bool createBottom = (_hints ? _hints->getCreateBottom() : true);

	unsigned int numSegments = pie.getEdges();
	glbFloat ratio = (_hints ? _hints->getDetailRatio() : 1.0f);
	if (ratio > 0.0f && ratio != 1.0f) {
		numSegments = (unsigned int) (numSegments * ratio);
		if (numSegments < MIN_NUM_SEGMENTS)
			numSegments = MIN_NUM_SEGMENTS;
	}

	glbFloat sAngle = pie.getSAngle();
	glbFloat eAngle = pie.getEAngle();
	glbFloat firstX = 0.0,firstY = 0.0,lastX = 0.0,lastY = 0.0;
	// pie body
	if (createBody)
		drawPieBody(numSegments, pie.getRadius(), pie.getHeight(),sAngle,eAngle);

	glbFloat angleDelta = (eAngle - sAngle)/(glbFloat)numSegments;
	glbFloat texCoordDelta = 1.0f/(glbFloat)numSegments;

	glbFloat r = pie.getRadius();
	glbFloat h = pie.getHeight();

	glbFloat basez = 0/*-h*0.5f*/;
	glbFloat topz = h/**0.5f*/;

	glbFloat angle = 0.0f;
	glbFloat texCoord = 0.0f;

	// pie top
	if (createTop) {
		gl.Begin(GL_TRIANGLE_FAN);

		gl.Normal3f(0.0f,0.0f,1.0f);
		gl.TexCoord2f(0.5f,0.5f);
		gl.Vertex3f(0.0f,0.0f,topz);

		angle = sAngle;
		texCoord = 0.0f;
		for(unsigned int topi=0;
			topi<numSegments + 1;
			++topi,angle+=angleDelta,texCoord+=texCoordDelta)
		{
			glbFloat c = cosf(angle);
			glbFloat s = sinf(angle);

			gl.TexCoord2f(c*0.5f+0.5f,s*0.5f+0.5f);
			gl.Vertex3f(c*r,s*r,topz);

			if (topi == 0)
			{
				firstX = c * r;
				firstY = s * r;
			}
			if (topi == numSegments)
			{
				lastX = c * r;
				lastY = s * r;
			}
		}

		gl.End();
	}

	// pie bottom
	if (createBottom)
	{
		gl.Begin(GL_TRIANGLE_FAN);

		gl.Normal3f(0.0f,0.0f,-1.0f);
		gl.TexCoord2f(0.5f,0.5f);
		gl.Vertex3f(0.0f,0.0f,basez);

		angle = eAngle;
		texCoord = 1.0f;
		for(unsigned int bottomi=0;
			bottomi<numSegments + 1;
			++bottomi,angle-=angleDelta,texCoord-=texCoordDelta)
		{
			glbFloat c = cosf(angle);
			glbFloat s = sinf(angle);

			gl.TexCoord2f(c*0.5f+0.5f,s*0.5f+0.5f);
			gl.Vertex3f(c*r,s*r,basez);
		}

		gl.End();
	}

	gl.Begin(GL_QUADS);
	//pie body

	//computeNormal
	osg::Vec3d point1(0.0,0.0,topz);
	osg::Vec3d point2(0.0,0.0,0.0);

	osg::Vec3d point3(0.0,0.0,basez);
	osg::Vec3d point4(firstX,firstY,basez);

	osg::Vec3d dir = (point1 - point2) ^ (point3 - point4);

	gl.Normal3f(dir.x(),dir.y(),dir.z());

	gl.TexCoord2f(1.0f,1.0f);
	gl.Vertex3f(0.0,0.0,topz);

	gl.TexCoord2f(1.0f,0.0f);
	gl.Vertex3f(0.0,0.0,basez);

	gl.TexCoord2f(0.0f,0.0f);
	gl.Vertex3f(firstX,firstY,basez);

	gl.TexCoord2f(0.0f,1.0f);
	gl.Vertex3f(firstX,firstY,topz);

	osg::Vec3d point5(lastX,lastY,basez);
	osg::Vec3d dir2 = (point1 - point2) ^ (point3 - point5);

	gl.Normal3f(-dir2.x(),-dir2.y(),-dir2.z());

	gl.TexCoord2f(0.0f,1.0f);
	gl.Vertex3f(lastX,lastY,topz);

	gl.TexCoord2f(0.0f,0.0f);
	gl.Vertex3f(lastX,lastY,basez);

	gl.TexCoord2f(1.0f,0.0f);
	gl.Vertex3f(0.0,0.0,basez);

	gl.TexCoord2f(1.0f,1.0f);
	gl.Vertex3f(0.0,0.0,topz);

	gl.End();

	gl.PopMatrix();
}

void CustomDrawShape::drawPieBody( unsigned int numSegments, glbFloat radius, glbFloat height,glbFloat sAngle,glbFloat eAngle )
{
	const glbFloat angleDelta = (eAngle - sAngle)/(glbFloat)numSegments;
	const glbFloat texCoordDelta = 1.0f/(glbFloat)numSegments;

	const glbFloat r = radius;
	const glbFloat h = height;

	glbFloat basez = 0/*-h*0.5f*/;
	glbFloat topz = h/**0.5f*/;

	glbFloat angle = sAngle;
	glbFloat texCoord = 0.0f;

	bool drawFrontFace = _hints ? _hints->getCreateFrontFace() : true;
	bool drawBackFace = _hints ? _hints->getCreateBackFace() : false;

	GLBeginEndAdapter& gl = _state.getGLBeginEndAdapter();

	gl.Begin(GL_QUAD_STRIP);

	if (drawFrontFace) {

		for(unsigned int bodyi=0;
			bodyi<numSegments + 1;
			++bodyi,angle+=angleDelta,texCoord+=texCoordDelta)
		{
			glbFloat c = cosf(angle);
			glbFloat s = sinf(angle);

			gl.Normal3f(c,s,0.0f);

			gl.TexCoord2f(texCoord,1.0f);
			gl.Vertex3f(c*r,s*r,topz);

			gl.TexCoord2f(texCoord,0.0f);
			gl.Vertex3f(c*r,s*r,basez);
		}
	}

	if (drawBackFace) {
		for(unsigned int bodyi=0;
			bodyi<numSegments + 1;
			++bodyi,angle+=angleDelta,texCoord+=texCoordDelta)
		{
			glbFloat c = cosf(angle);
			glbFloat s = sinf(angle);

			gl.Normal3f(-c,-s,0.0f);

			gl.TexCoord2f(texCoord,0.0f);
			gl.Vertex3f(c*r,s*r,basez);

			gl.TexCoord2f(texCoord,1.0f);
			gl.Vertex3f(c*r,s*r,topz);
		}
	}

	gl.End();
}

void CustomDrawShape::DrawBoxShape( const CustomBox& box)
{
	GLBeginEndAdapter& gl = _state.getGLBeginEndAdapter();

	// evaluate hints
	bool createBody = (_hints ? _hints->getCreateBody() : true);
	bool createTop = (_hints ? _hints->getCreateTop() : true);
	bool createBottom = (_hints ? _hints->getCreateBottom() : true);

	float dx = box.getHalfLengths().x();
	float dy = box.getHalfLengths().y();
	float dz = box.getHalfLengths().z();

	gl.PushMatrix();

	gl.Translated(box.getCenter().x(),box.getCenter().y(),box.getHalfLengths().z()/*box.getCenter().z()*/);

	if (!box.zeroRotation())
	{
		Matrixd rotation(box.computeRotationMatrix());
		gl.MultMatrixd(rotation.ptr());
	}

	gl.Begin(GL_QUADS);

	if (createBody) {
		// -ve y plane
		gl.Normal3f(0.0f,-1.0f,0.0f);

		gl.TexCoord2f(0.0f,1.0f);
		gl.Vertex3f(-dy,-dx,dz);

		gl.TexCoord2f(0.0f,0.0f);
		gl.Vertex3f(-dy,-dx,-dz);

		gl.TexCoord2f(1.0f,0.0f);
		gl.Vertex3f(dy,-dx,-dz);

		gl.TexCoord2f(1.0f,1.0f);
		gl.Vertex3f(dy,-dx,dz);

		// +ve y plane
		gl.Normal3f(0.0f,1.0f,0.0f);

		gl.TexCoord2f(0.0f,1.0f);
		gl.Vertex3f(dy,dx,dz);

		gl.TexCoord2f(0.0f,0.0f);
		gl.Vertex3f(dy,dx,-dz);

		gl.TexCoord2f(1.0f,0.0f);
		gl.Vertex3f(-dy,dx,-dz);

		gl.TexCoord2f(1.0f,1.0f);
		gl.Vertex3f(-dy,dx,dz);

		// +ve x plane
		gl.Normal3f(1.0f,0.0f,0.0f);

		gl.TexCoord2f(0.0f,1.0f);
		gl.Vertex3f(dy,-dx,dz);

		gl.TexCoord2f(0.0f,0.0f);
		gl.Vertex3f(dy,-dx,-dz);

		gl.TexCoord2f(1.0f,0.0f);
		gl.Vertex3f(dy,dx,-dz);

		gl.TexCoord2f(1.0f,1.0f);
		gl.Vertex3f(dy,dx,dz);

		// -ve x plane
		gl.Normal3f(-1.0f,0.0f,0.0f);

		gl.TexCoord2f(0.0f,1.0f);
		gl.Vertex3f(-dy,dx,dz);

		gl.TexCoord2f(0.0f,0.0f);
		gl.Vertex3f(-dy,dx,-dz);

		gl.TexCoord2f(1.0f,0.0f);
		gl.Vertex3f(-dy,-dx,-dz);

		gl.TexCoord2f(1.0f,1.0f);
		gl.Vertex3f(-dy,-dx,dz);
	}

	if (createTop) {
		// +ve z plane
		gl.Normal3f(0.0f,0.0f,1.0f);

		gl.TexCoord2f(0.0f,1.0f);
		gl.Vertex3f(-dy,dx,dz);

		gl.TexCoord2f(0.0f,0.0f);
		gl.Vertex3f(-dy,-dx,dz);

		gl.TexCoord2f(1.0f,0.0f);
		gl.Vertex3f(dy,-dx,dz);

		gl.TexCoord2f(1.0f,1.0f);
		gl.Vertex3f(dy,dx,dz);
	}

	if (createBottom) {
		// -ve z plane
		gl.Normal3f(0.0f,0.0f,-1.0f);

		gl.TexCoord2f(0.0f,1.0f);
		gl.Vertex3f(dy,dx,-dz);

		gl.TexCoord2f(0.0f,0.0f);
		gl.Vertex3f(dy,-dx,-dz);

		gl.TexCoord2f(1.0f,0.0f);
		gl.Vertex3f(-dy,-dx,-dz);

		gl.TexCoord2f(1.0f,1.0f);
		gl.Vertex3f(-dy,dx,-dz);
	}

	gl.End();

	gl.PopMatrix();
}

void CustomDrawShape::DrawSphereShape( const CustomSphere& sphere)
{
	GLBeginEndAdapter& gl = _state.getGLBeginEndAdapter();

	gl.PushMatrix();

	gl.Translated(sphere.getCenter().x(),sphere.getCenter().y(),sphere.getRadius()/*sphere.getCenter().z()*/);

	bool drawFrontFace = _hints ? _hints->getCreateFrontFace() : true;
	bool drawBackFace = _hints ? _hints->getCreateBackFace() : false;

	unsigned int numSegments = 40;
	unsigned int numRows = 20;
	float ratio = (_hints ? _hints->getDetailRatio() : 1.0f);
	if (ratio > 0.0f && ratio != 1.0f) {
		numRows = (unsigned int) (numRows * ratio);
		if (numRows < MIN_NUM_ROWS)
			numRows = MIN_NUM_ROWS;
		numSegments = (unsigned int) (numSegments * ratio);
		if (numSegments < MIN_NUM_SEGMENTS)
			numSegments = MIN_NUM_SEGMENTS;
	}

	float lDelta = osg::PI/(float)numRows;
	float vDelta = 1.0f/(float)numRows;

	float angleDelta = osg::PI*2.0f/(float)numSegments;
	float texCoordHorzDelta = 1.0f/(float)numSegments;

	if (drawBackFace)
	{
		float lBase=-osg::PI*0.5f;
		float rBase=0.0f;
		float zBase=-sphere.getRadius();
		float vBase=0.0f;
		float nzBase=-1.0f;
		float nRatioBase=0.0f;

		for(unsigned int rowi=0; rowi<numRows; ++rowi)
		{

			float lTop = lBase+lDelta;
			float rTop = cosf(lTop)*sphere.getRadius();
			float zTop = sinf(lTop)*sphere.getRadius();
			float vTop = vBase+vDelta;
			float nzTop= sinf(lTop);
			float nRatioTop= cosf(lTop);

			gl.Begin(GL_QUAD_STRIP);

			float angle = 0.0f;
			float texCoord = 0.0f;

			for(unsigned int topi=0; topi<numSegments;
				++topi,angle+=angleDelta,texCoord+=texCoordHorzDelta)
			{

				float c = cosf(angle);
				float s = sinf(angle);

				gl.Normal3f(-c*nRatioBase,-s*nRatioBase,-nzBase);

				gl.TexCoord2f(texCoord,vBase);
				gl.Vertex3f(c*rBase,s*rBase,zBase);

				gl.Normal3f(-c*nRatioTop,-s*nRatioTop,-nzTop);

				gl.TexCoord2f(texCoord,vTop);
				gl.Vertex3f(c*rTop,s*rTop,zTop);


			}


			// do last point by hand to ensure no round off errors.
			gl.Normal3f(-nRatioBase,0.0f,-nzBase);

			gl.TexCoord2f(1.0f,vBase);
			gl.Vertex3f(rBase,0.0f,zBase);

			gl.Normal3f(-nRatioTop,0.0f,-nzTop);

			gl.TexCoord2f(1.0f,vTop);
			gl.Vertex3f(rTop,0.0f,zTop);

			gl.End();


			lBase=lTop;
			rBase=rTop;
			zBase=zTop;
			vBase=vTop;
			nzBase=nzTop;
			nRatioBase=nRatioTop;

		}
	}


	if (drawFrontFace)
	{
		float lBase=-osg::PI*0.5f;
		float rBase=0.0f;
		float zBase=-sphere.getRadius();
		float vBase=0.0f;
		float nzBase=-1.0f;
		float nRatioBase=0.0f;

		for(unsigned int rowi=0; rowi<numRows; ++rowi)
		{

			float lTop = lBase+lDelta;
			float rTop = cosf(lTop)*sphere.getRadius();
			float zTop = sinf(lTop)*sphere.getRadius();
			float vTop = vBase+vDelta;
			float nzTop= sinf(lTop);
			float nRatioTop= cosf(lTop);

			gl.Begin(GL_QUAD_STRIP);

			float angle = 0.0f;
			float texCoord = 0.0f;

			for(unsigned int topi=0; topi<numSegments;
				++topi,angle+=angleDelta,texCoord+=texCoordHorzDelta)
			{

				float c = cosf(angle);
				float s = sinf(angle);

				gl.Normal3f(c*nRatioTop,s*nRatioTop,nzTop);

				gl.TexCoord2f(texCoord,vTop);
				gl.Vertex3f(c*rTop,s*rTop,zTop);

				gl.Normal3f(c*nRatioBase,s*nRatioBase,nzBase);

				gl.TexCoord2f(texCoord,vBase);
				gl.Vertex3f(c*rBase,s*rBase,zBase);

			}

			// do last point by hand to ensure no round off errors.
			gl.Normal3f(nRatioTop,0.0f,nzTop);

			gl.TexCoord2f(1.0f,vTop);
			gl.Vertex3f(rTop,0.0f,zTop);

			gl.Normal3f(nRatioBase,0.0f,nzBase);

			gl.TexCoord2f(1.0f,vBase);
			gl.Vertex3f(rBase,0.0f,zBase);

			gl.End();


			lBase=lTop;
			rBase=rTop;
			zBase=zTop;
			vBase=vTop;
			nzBase=nzTop;
			nRatioBase=nRatioTop;

		}
	}

	gl.PopMatrix();
}

class CustomComputeBoundShape
{
public:

	CustomComputeBoundShape(BoundingBox& bb):_bb(bb) {}
	void ComputeCircleBound(const CustomCircle&);
	void ComputeRectangleBound(const CustomRectangle&);
	void ComputeEllipseBound(const CustomEllipse&);
	void ComputeArcBound(const CustomArc&);
	void ComputeConeBound(const CustomCone&);
	void ComputeCylinderBound(const CustomCylinder&);
	void ComputePyramidBound(const CustomPyramid&);
	void ComputePieBound(const CustomPie&);
	void ComputeBoxBound(const CustomBox&);
	void ComputeSphereBound(const CustomSphere&);
	BoundingBox&    _bb;
protected:
	CustomComputeBoundShape& operator = (const CustomComputeBoundShape&) { return *this; }
};

void CustomComputeBoundShape::ComputeCircleBound( const CustomCircle& circle)
{
	Vec3 halfLenghts(circle.getRadius(),circle.getRadius(),0.0);
	_bb.expandBy(circle.getCenter() - halfLenghts);
	_bb.expandBy(circle.getCenter() + halfLenghts);
}

void CustomComputeBoundShape::ComputeRectangleBound( const CustomRectangle& rectangle)
{
	Vec3 halfLengths(rectangle.getWidth(),rectangle.getHeight(),0.0f);
	_bb.expandBy(rectangle.getCenter() - halfLengths);
	_bb.expandBy(rectangle.getCenter() + halfLengths);
}

void CustomComputeBoundShape::ComputeEllipseBound( const CustomEllipse& ellipse)
{
	Vec3 halfLengths(ellipse.getXRadius(),ellipse.getYRadius(),0.0f);
	_bb.expandBy(ellipse.getCenter() - halfLengths);
	_bb.expandBy(ellipse.getCenter() + halfLengths);
}

void CustomComputeBoundShape::ComputeArcBound( const CustomArc& arc)
{
	Vec3 halfLenghts(arc.getXRadius(),arc.getYRadius(),0.0);
	_bb.expandBy(arc.getCenter() - halfLenghts);
	_bb.expandBy(arc.getCenter() + halfLenghts);
}

void CustomComputeBoundShape::ComputeConeBound( const CustomCone& cone)
{
	if (cone.zeroRotation())
	{
		_bb.expandBy(cone.getCenter() + Vec3(0.0,0.0,(cone.getHeight() + cone.getBaseOffset()) * 0.5f) +Vec3(-cone.getRadius(),-cone.getRadius(),cone.getBaseOffset()));
		_bb.expandBy(cone.getCenter() + Vec3(0.0,0.0,(cone.getHeight() + cone.getBaseOffset()) * 0.5f) + Vec3(cone.getRadius(),cone.getRadius(),cone.getHeight()+cone.getBaseOffset()));

	}
	else
	{
		Vec3 top(Vec3(cone.getRadius(),cone.getRadius(),cone.getHeight()+cone.getBaseOffset()));
		Vec3 base_1(Vec3(-cone.getRadius(),-cone.getRadius(),cone.getBaseOffset()));
		Vec3 base_2(Vec3(cone.getRadius(),-cone.getRadius(),cone.getBaseOffset()));
		Vec3 base_3(Vec3(cone.getRadius(),cone.getRadius(),cone.getBaseOffset()));
		Vec3 base_4(Vec3(-cone.getRadius(),cone.getRadius(),cone.getBaseOffset()));

		Matrix matrix = cone.computeRotationMatrix();
		_bb.expandBy(cone.getCenter()+base_1*matrix);
		_bb.expandBy(cone.getCenter()+base_2*matrix);
		_bb.expandBy(cone.getCenter()+base_3*matrix);
		_bb.expandBy(cone.getCenter()+base_4*matrix);
		_bb.expandBy(cone.getCenter()+top*matrix);
	}
}

void CustomComputeBoundShape::ComputeCylinderBound( const CustomCylinder& cylinder)
{
	if (cylinder.zeroRotation())
	{
		Vec3 halfLengths(cylinder.getRadius(),cylinder.getRadius(),cylinder.getHeight()*0.5f);
		_bb.expandBy(cylinder.getCenter() + Vec3(0.0,0.0,cylinder.getHeight()*0.5f) -halfLengths);
		_bb.expandBy(cylinder.getCenter()+ Vec3(0.0,0.0,cylinder.getHeight()*0.5f) + halfLengths);
	}
	else
	{
		glbFloat r = cylinder.getRadius();
		glbFloat z = cylinder.getHeight()*0.5f;

		Vec3 base_1(Vec3(-r,-r,-z));
		Vec3 base_2(Vec3(r,-r,-z));
		Vec3 base_3(Vec3(r,r,-z));
		Vec3 base_4(Vec3(-r,r,-z));

		Vec3 top_1(Vec3(-r,-r,z));
		Vec3 top_2(Vec3(r,-r,z));
		Vec3 top_3(Vec3(r,r,z));
		Vec3 top_4(Vec3(-r,r,z));

		Matrix matrix = cylinder.computeRotationMatrix();
		_bb.expandBy(cylinder.getCenter()+base_1*matrix);
		_bb.expandBy(cylinder.getCenter()+base_2*matrix);
		_bb.expandBy(cylinder.getCenter()+base_3*matrix);
		_bb.expandBy(cylinder.getCenter()+base_4*matrix);

		_bb.expandBy(cylinder.getCenter()+top_1*matrix);
		_bb.expandBy(cylinder.getCenter()+top_2*matrix);
		_bb.expandBy(cylinder.getCenter()+top_3*matrix);
		_bb.expandBy(cylinder.getCenter()+top_4*matrix);
	}
}

void CustomComputeBoundShape::ComputePyramidBound( const CustomPyramid& pyramid)
{
	if (pyramid.zeroRotation())
	{
		_bb.expandBy(pyramid.getCenter() + Vec3(0.0,0.0,pyramid.getHeight() * 0.5f) + Vec3(-pyramid.getWidth(),-pyramid.getLength(),pyramid.getBaseOffset()));
		_bb.expandBy(pyramid.getCenter() + Vec3(0.0,0.0,pyramid.getHeight() * 0.5f) + Vec3(pyramid.getWidth(),pyramid.getLength(),pyramid.getHeight()+pyramid.getBaseOffset()));
	}
	else
	{
		Vec3 top(Vec3(pyramid.getLength(),pyramid.getWidth(),pyramid.getHeight()+pyramid.getBaseOffset()));
		Vec3 base_1(Vec3(-pyramid.getLength(),-pyramid.getWidth(),pyramid.getBaseOffset()));
		Vec3 base_2(Vec3(pyramid.getLength(),-pyramid.getWidth(),pyramid.getBaseOffset()));
		Vec3 base_3(Vec3(pyramid.getLength(),pyramid.getWidth(),pyramid.getBaseOffset()));
		Vec3 base_4(Vec3(-pyramid.getLength(),pyramid.getWidth(),pyramid.getBaseOffset()));

		Matrix matrix = pyramid.computeRotationMatrix();
		_bb.expandBy(pyramid.getCenter()+base_1*matrix);
		_bb.expandBy(pyramid.getCenter()+base_2*matrix);
		_bb.expandBy(pyramid.getCenter()+base_3*matrix);
		_bb.expandBy(pyramid.getCenter()+base_4*matrix);
		_bb.expandBy(pyramid.getCenter()+top*matrix);
	}
}

void CustomComputeBoundShape::ComputePieBound( const CustomPie& pie)
{
	if (pie.zeroRotation())
	{
		Vec3 halfLengths(pie.getRadius(),pie.getRadius(),pie.getHeight()*0.5f);
		_bb.expandBy(pie.getCenter() + Vec3(0.0,0.0,pie.getHeight() * 0.5f) - halfLengths);
		_bb.expandBy(pie.getCenter() + Vec3(0.0,0.0,pie.getHeight() * 0.5f) + halfLengths);
	}
	else
	{
		glbFloat r = pie.getRadius();
		glbFloat z = pie.getHeight()*0.5f;

		Vec3 base_1(Vec3(-r,-r,-z));
		Vec3 base_2(Vec3(r,-r,-z));
		Vec3 base_3(Vec3(r,r,-z));
		Vec3 base_4(Vec3(-r,r,-z));

		Vec3 top_1(Vec3(-r,-r,z));
		Vec3 top_2(Vec3(r,-r,z));
		Vec3 top_3(Vec3(r,r,z));
		Vec3 top_4(Vec3(-r,r,z));

		Matrix matrix = pie.computeRotationMatrix();
		_bb.expandBy(pie.getCenter()+base_1*matrix);
		_bb.expandBy(pie.getCenter()+base_2*matrix);
		_bb.expandBy(pie.getCenter()+base_3*matrix);
		_bb.expandBy(pie.getCenter()+base_4*matrix);

		_bb.expandBy(pie.getCenter()+top_1*matrix);
		_bb.expandBy(pie.getCenter()+top_2*matrix);
		_bb.expandBy(pie.getCenter()+top_3*matrix);
		_bb.expandBy(pie.getCenter()+top_4*matrix);
	}
}

void CustomComputeBoundShape::ComputeBoxBound( const CustomBox& box)
{
	if (box.zeroRotation())
	{
		_bb.expandBy(box.getCenter() + Vec3(0.0,0.0,box.getHalfLengths().z()) - Vec3(box.getHalfLengths().y(),box.getHalfLengths().x(),box.getHalfLengths().z()));
		_bb.expandBy(box.getCenter() + Vec3(0.0,0.0,box.getHalfLengths().z()) + Vec3(box.getHalfLengths().y(),box.getHalfLengths().x(),box.getHalfLengths().z()));
		//_bb.expandBy(box.getCenter() + Vec3(0.0,0.0,box.getHalfLengths().z()) - box.getHalfLengths());
		//_bb.expandBy(box.getCenter() + Vec3(0.0,0.0,box.getHalfLengths().z()) + box.getHalfLengths());
	}
	else
	{
		float x = box.getHalfLengths().x();
		float y = box.getHalfLengths().y();
		float z = box.getHalfLengths().z();

		Vec3 base_1(Vec3(-x,-y,-z));
		Vec3 base_2(Vec3(x,-y,-z));
		Vec3 base_3(Vec3(x,y,-z));
		Vec3 base_4(Vec3(-x,y,-z));

		Vec3 top_1(Vec3(-x,-y,z));
		Vec3 top_2(Vec3(x,-y,z));
		Vec3 top_3(Vec3(x,y,z));
		Vec3 top_4(Vec3(-x,y,z));

		Matrix matrix = box.computeRotationMatrix();
		_bb.expandBy(box.getCenter()+base_1*matrix);
		_bb.expandBy(box.getCenter()+base_2*matrix);
		_bb.expandBy(box.getCenter()+base_3*matrix);
		_bb.expandBy(box.getCenter()+base_4*matrix);

		_bb.expandBy(box.getCenter()+top_1*matrix);
		_bb.expandBy(box.getCenter()+top_2*matrix);
		_bb.expandBy(box.getCenter()+top_3*matrix);
		_bb.expandBy(box.getCenter()+top_4*matrix);
	}
}

void CustomComputeBoundShape::ComputeSphereBound( const CustomSphere& sphere)
{
	Vec3 halfLengths(sphere.getRadius(),sphere.getRadius(),sphere.getRadius());
	_bb.expandBy(sphere.getCenter() + Vec3(0.0,0.0,sphere.getRadius()) - halfLengths);
	_bb.expandBy(sphere.getCenter()+ Vec3(0.0,0.0,sphere.getRadius()) + halfLengths);
}

class CustomPrimitiveShape
{
public:
	CustomPrimitiveShape(PrimitiveFunctor& functor,const TessellationHints* hints):
	  _functor(functor),
		  _hints(hints) {}
	  void PrimitiveCircleShape(const CustomCircle&);
	  void PrimitiveRectangleShape(const CustomRectangle&);
	  void PrimitiveEllipseShape(const CustomEllipse&);
	  void PrimitiveArcShape(const CustomArc&);
	  void PrimitiveConeShape(const CustomCone&);
	  void PrimitiveCylinderShape(const CustomCylinder&);
	  void PrimitivePyramidShape(const CustomPyramid&);
	  void PrimitivePieShape(const CustomPie&);
	  void PrimitiveBoxShape(const CustomBox&);
	  void PrimitiveSphereShape(const CustomSphere&);
	  PrimitiveFunctor& _functor;
	  const TessellationHints*  _hints;
private:
	CustomPrimitiveShape& operator = (const CustomPrimitiveShape&) { return *this; }
	void createCylinderBody( unsigned int numSegments, glbFloat radius, glbFloat height, const osg::Matrix& matrix );
	void createPieBody(unsigned int numSegments, glbFloat radius, glbFloat height,glbFloat sAngle,glbFloat eAngle, const osg::Matrix& matrix);
};

void CustomPrimitiveShape::PrimitiveCircleShape( const CustomCircle& circle)
{
	Matrix matrix;
	matrix.setTrans(circle.getCenter());

	unsigned int numSegments = circle.getEdges();
	glbFloat r = circle.getRadius();
	glbFloat angleDelta = 2.0f*osg::PI/(glbFloat)numSegments;
	glbFloat basez = 0.0;
	glbFloat angle;

	_functor.begin(GL_TRIANGLE_FAN);
	angle = osg::PI*2.0f;
	_functor.vertex(Vec3(0.0f,0.0f,basez)*matrix);
	for(unsigned int bottomi=0;
		bottomi<numSegments;
		++bottomi,angle-=angleDelta)
	{
		glbFloat c = cosf(angle);
		glbFloat s = sinf(angle);
		_functor.vertex(Vec3(c*r,s*r,basez)*matrix);
	}
	_functor.vertex(Vec3(r,0.0f,basez)*matrix);
	_functor.end();
}

void CustomPrimitiveShape::PrimitiveRectangleShape( const CustomRectangle& rectangle)
{
	float x = rectangle.getWidth() / 2;
	float y = rectangle.getHeight() / 2;
	float z = 0.0;

	Vec3 top_1(-x,-y,z);
	Vec3 top_2(x,-y,z);
	Vec3 top_3(x,y,z);
	Vec3 top_4(-x,y,z);

	top_1 += rectangle.getCenter();
	top_2 += rectangle.getCenter();
	top_3 += rectangle.getCenter();
	top_4 += rectangle.getCenter();

	_functor.begin(GL_QUADS);
	_functor.vertex(top_4);
	_functor.vertex(top_1);
	_functor.vertex(top_2);
	_functor.vertex(top_3);
	_functor.end();
}

void CustomPrimitiveShape::PrimitiveEllipseShape( const CustomEllipse& ellipse)
{
	Matrix matrix;
	matrix.setTrans(ellipse.getCenter());

	unsigned int numSegments = ellipse.getEdges();
	glbFloat xr = ellipse.getXRadius();
	glbFloat yr = ellipse.getYRadius();
	glbFloat angleDelta = 2.0f*osg::PI/(glbFloat)numSegments;
	glbFloat basez = 0.0;
	glbFloat angle;

	_functor.begin(GL_TRIANGLE_FAN);
	angle = osg::PI*2.0f;
	_functor.vertex(Vec3(0.0f,0.0f,basez)*matrix);
	for(unsigned int bottomi=0;
		bottomi<numSegments;
		++bottomi,angle-=angleDelta)
	{
		glbFloat c = cosf(angle);
		glbFloat s = sinf(angle);
		_functor.vertex(Vec3(c * xr,s * yr,basez)*matrix);
	}
	_functor.vertex(Vec3(xr,yr,basez)*matrix);
	_functor.end();
}

void CustomPrimitiveShape::PrimitiveArcShape( const CustomArc& arc)
{
	Matrix matrix;
	matrix.setTrans(arc.getCenter());

	unsigned int numSegments = arc.getEdges();
	glbFloat xr = arc.getXRadius();
	glbFloat yr = arc.getYRadius();
	glbFloat angleDelta = (arc.getEAngle() - arc.getSAngle()) / (glbFloat)numSegments;
	glbFloat basez = 0.0;

	_functor.begin(GL_TRIANGLE_FAN);

	if(arc.getIsFan())
		_functor.vertex(Vec3(0.0,0.0,0.0) * matrix);

	glbFloat angle = arc.getSAngle();
	for(unsigned int bottomi=0;
		bottomi<numSegments;
		++bottomi,angle += angleDelta)
	{
		glbFloat c = cosf(angle);
		glbFloat s = sinf(angle);
		_functor.vertex(Vec3(c * xr,s * yr,basez)*matrix);
	}
	_functor.end();
}

void CustomPrimitiveShape::PrimitiveConeShape( const CustomCone& cone)
{
	// evaluate hints
	bool createBody = (_hints ? _hints->getCreateBody() : true);
	bool createBottom = (_hints ? _hints->getCreateBottom() : true);

	Matrix matrix = cone.computeRotationMatrix();
	matrix.setTrans(Vec3(cone.getCenter().x(),
		cone.getCenter().y(),(cone.getHeight() + cone.getBaseOffset()) * 0.5));

	unsigned int numSegments = cone.getEdges();
	unsigned int numRows = 20;
	glbFloat ratio = (_hints ? _hints->getDetailRatio() : 1.0f);
	if (ratio > 0.0f && ratio != 1.0f) {
		numRows = (unsigned int) (numRows * ratio);
		if (numRows < MIN_NUM_ROWS)
			numRows = MIN_NUM_ROWS;
		numSegments = (unsigned int) (numSegments * ratio);
		if (numSegments < MIN_NUM_SEGMENTS)
			numSegments = MIN_NUM_SEGMENTS;
	}

	glbFloat r = cone.getRadius();
	glbFloat h = cone.getHeight();

	glbFloat normalz = r/(sqrtf(r*r+h*h));
	glbFloat normalRatio = 1.0f/(sqrtf(1.0f+normalz*normalz));
	normalz *= normalRatio;

	glbFloat angleDelta = 2.0f*osg::PI/(glbFloat)numSegments;
	glbFloat hDelta = cone.getHeight()/(glbFloat)numRows;
	glbFloat rDelta = cone.getRadius()/(glbFloat)numRows;

	glbFloat topz=cone.getHeight()+cone.getBaseOffset();
	glbFloat topr=0.0f;
	glbFloat basez=topz-hDelta;
	glbFloat baser=rDelta;
	glbFloat angle;

	if (createBody)
	{
		for(unsigned int rowi=0;
			rowi<numRows;
			++rowi,topz=basez, basez-=hDelta, topr=baser, baser+=rDelta)
		{
			// we can't use a fan for the cone top
			// since we need different normals at the top
			// for each face..
			_functor.begin(GL_QUAD_STRIP);

			angle = 0.0f;
			for(unsigned int topi=0;
				topi<numSegments;
				++topi,angle+=angleDelta)
			{

				glbFloat c = cosf(angle);
				glbFloat s = sinf(angle);

				_functor.vertex(Vec3(c*topr,s*topr,topz)*matrix);
				_functor.vertex(Vec3(c*baser,s*baser,basez)*matrix);

			}

			// do last point by hand to ensure no round off errors.
			_functor.vertex(Vec3(topr,0.0f,topz)*matrix);
			_functor.vertex(Vec3(baser,0.0f,basez)*matrix);

			_functor.end();

		}
	}

	if (createBottom)
	{
		// we can't use a fan for the cone top
		// since we need different normals at the top
		// for each face..
		_functor.begin(GL_TRIANGLE_FAN);

		angle = osg::PI*2.0f;
		basez = cone.getBaseOffset();

		_functor.vertex(Vec3(0.0f,0.0f,basez)*matrix);

		for(unsigned int bottomi=0;
			bottomi<numSegments;
			++bottomi,angle-=angleDelta)
		{

			glbFloat c = cosf(angle);
			glbFloat s = sinf(angle);

			_functor.vertex(Vec3(c*r,s*r,basez)*matrix);

		}

		_functor.vertex(Vec3(r,0.0f,basez)*matrix);

		_functor.end();
	}
}

void CustomPrimitiveShape::PrimitiveCylinderShape( const CustomCylinder& cylinder)
{
	// evaluate hints
	bool createBody = (_hints ? _hints->getCreateBody() : true);
	bool createTop = (_hints ? _hints->getCreateTop() : true);
	bool createBottom = (_hints ? _hints->getCreateBottom() : true);

	Matrix matrix = cylinder.computeRotationMatrix();
	matrix.setTrans(cylinder.getCenter());

	unsigned int numSegments = cylinder.getEdges();
	glbFloat ratio = (_hints ? _hints->getDetailRatio() : 1.0f);
	if (ratio > 0.0f && ratio != 1.0f) {
		numSegments = (unsigned int) (numSegments * ratio);
		if (numSegments < MIN_NUM_SEGMENTS)
			numSegments = MIN_NUM_SEGMENTS;
	}

	glbFloat angleDelta = 2.0f*osg::PI/(glbFloat)numSegments;

	glbFloat r = cylinder.getRadius();
	glbFloat h = cylinder.getHeight();

	glbFloat basez = 0/*-h*0.5f*/;
	glbFloat topz = h/*h*0.5f*/;
	glbFloat angle;

	// cylinder body
	if (createBody)
		createCylinderBody(numSegments, cylinder.getRadius(), cylinder.getHeight(), matrix);

	// cylinder top
	if (createTop)
	{
		_functor.begin(GL_TRIANGLE_FAN);

		_functor.vertex(Vec3(0.0f,0.0f,topz)*matrix);

		angle = 0.0f;
		for(unsigned int topi=0;
			topi<numSegments;
			++topi,angle+=angleDelta)
		{

			glbFloat c = cosf(angle);
			glbFloat s = sinf(angle);

			_functor.vertex(Vec3(c*r,s*r,topz)*matrix);

		}

		_functor.vertex(Vec3(r,0.0f,topz)*matrix);

		_functor.end();
	}

	// cylinder bottom
	if (createBottom)
	{
		_functor.begin(GL_TRIANGLE_FAN);

		_functor.vertex(Vec3(0.0f,0.0f,basez)*matrix);

		angle = osg::PI*2.0f;
		for(unsigned int bottomi=0;
			bottomi<numSegments;
			++bottomi,angle-=angleDelta)
		{

			glbFloat c = cosf(angle);
			glbFloat s = sinf(angle);

			_functor.vertex(Vec3(c*r,s*r,basez)*matrix);

		}

		_functor.vertex(Vec3(r,0.0f,basez)*matrix);

		_functor.end();
	}
}

void CustomPrimitiveShape::createCylinderBody( unsigned int numSegments, glbFloat radius, glbFloat height, const osg::Matrix& matrix )
{
	const glbFloat angleDelta = 2.0f*osg::PI/(glbFloat)numSegments;

	const glbFloat r = radius;
	const glbFloat h = height;

	glbFloat basez = 0/*-h*0.5f*/;
	glbFloat topz = h/*h*0.5f*/;

	glbFloat angle = 0.0f;

	_functor.begin(GL_QUAD_STRIP);

	for(unsigned int bodyi=0;
		bodyi<numSegments;
		++bodyi,angle+=angleDelta)
	{
		glbFloat c = cosf(angle);
		glbFloat s = sinf(angle);

		_functor.vertex(osg::Vec3(c*r,s*r,topz) * matrix);
		_functor.vertex(osg::Vec3(c*r,s*r,basez) * matrix);
	}

	// do last point by hand to ensure no round off errors.
	_functor.vertex(osg::Vec3(r,0.0f,topz) * matrix);
	_functor.vertex(osg::Vec3(r,0.0f,basez) * matrix);

	_functor.end();
}

void CustomPrimitiveShape::PrimitivePyramidShape( const CustomPyramid& pyramid)
{
	// evaluate hints
	bool createBody = (_hints ? _hints->getCreateBody() : true);
	bool createBottom = (_hints ? _hints->getCreateBottom() : true);

	Vec3 center = pyramid.getCenter();
	float x = pyramid.getWidth();
	float y = pyramid.getLength();
	float z = pyramid.getHeight();


	Vec3 base_1(-x,y,0);
	Vec3 base_2(-x,-y,0);
	Vec3 base_3(x,-y,0);
	Vec3 base_4(x,y,0);

	Vec3 top_1(0.0,0.0,z);
	Vec3 top_2(0.0,0.0,z);
	Vec3 top_3(0.0,0.0,z);
	Vec3 top_4(0.0,0.0,z);

	if (pyramid.zeroRotation())
	{
		base_1 += center;
		base_2 += center;
		base_3 += center;
		base_4 += center;

		top_1 += center;
		top_2 += center;
		top_3 += center;
		top_4 += center;
	}
	else
	{
		Matrix matrix = pyramid.computeRotationMatrix();
		matrix.setTrans(center);

		base_1 = base_1*matrix;
		base_2 = base_2*matrix;
		base_3 = base_3*matrix;
		base_4 = base_4*matrix;

		top_1 = top_1*matrix;
		top_2 = top_2*matrix;
		top_3 = top_3*matrix;
		top_4 = top_4*matrix;
	}

	_functor.begin(GL_QUADS);
	if (createBody)
	{
		_functor.vertex(top_1);
		_functor.vertex(base_1);
		_functor.vertex(base_2);
		_functor.vertex(top_2);

		_functor.vertex(top_2);
		_functor.vertex(base_2);
		_functor.vertex(base_3);
		_functor.vertex(top_3);

		_functor.vertex(top_3);
		_functor.vertex(base_3);
		_functor.vertex(base_4);
		_functor.vertex(top_4);

		_functor.vertex(top_4);
		_functor.vertex(base_4);
		_functor.vertex(base_1);
		_functor.vertex(top_1);
	}

	if (createBottom)
	{
		_functor.vertex(base_4);
		_functor.vertex(base_3);
		_functor.vertex(base_2);
		_functor.vertex(base_1);
	}

	_functor.end();
}

void CustomPrimitiveShape::PrimitivePieShape( const CustomPie& pie)
{
	// evaluate hints
	bool createBody = (_hints ? _hints->getCreateBody() : true);
	bool createTop = (_hints ? _hints->getCreateTop() : true);
	bool createBottom = (_hints ? _hints->getCreateBottom() : true);

	Matrix matrix = pie.computeRotationMatrix();
	matrix.setTrans(pie.getCenter());

	unsigned int numSegments = pie.getEdges();
	glbFloat ratio = (_hints ? _hints->getDetailRatio() : 1.0f);
	if (ratio > 0.0f && ratio != 1.0f) {
		numSegments = (unsigned int) (numSegments * ratio);
		if (numSegments < MIN_NUM_SEGMENTS)
			numSegments = MIN_NUM_SEGMENTS;
	}

	glbFloat sAngle = pie.getSAngle();
	glbFloat eAngle = pie.getEAngle();
	glbFloat firstX = 0.0,firstY = 0.0,lastX = 0.0,lastY = 0.0;
	glbFloat angleDelta = (eAngle - sAngle)/(glbFloat)numSegments;
	//glbFloat angleDelta = 2.0f*osg::PI/(glbFloat)numSegments;

	glbFloat r = pie.getRadius();
	glbFloat h = pie.getHeight();

	glbFloat basez = 0/*-h*0.5f*/;
	glbFloat topz = h/*h*0.5f*/;
	glbFloat angle = 0.0;

	// pie body
	if (createBody)
		createPieBody(numSegments, pie.getRadius(), pie.getHeight(), sAngle,eAngle,matrix);

	// pie top
	if (createTop)
	{
		_functor.begin(GL_TRIANGLE_FAN);

		_functor.vertex(Vec3(0.0f,0.0f,topz)*matrix);

		angle = sAngle;
		for(unsigned int topi=0;
			topi<numSegments + 1;
			++topi,angle+=angleDelta)
		{

			glbFloat c = cosf(angle);
			glbFloat s = sinf(angle);

			_functor.vertex(Vec3(c*r,s*r,topz)*matrix);

			if(topi == 0)
			{
				firstX = c * r;
				firstY = s * r;
			}
			else if(topi == numSegments)
			{
				lastX = c * r;
				lastY = s * r;
			}
		}

		_functor.end();
	}

	// pie bottom
	if (createBottom)
	{
		_functor.begin(GL_TRIANGLE_FAN);

		_functor.vertex(Vec3(0.0f,0.0f,basez)*matrix);

		angle = eAngle;
		for(unsigned int bottomi=0;
			bottomi<numSegments + 1;
			++bottomi,angle-=angleDelta)
		{

			glbFloat c = cosf(angle);
			glbFloat s = sinf(angle);

			_functor.vertex(Vec3(c*r,s*r,basez)*matrix);

		}

		_functor.end();

		_functor.begin(GL_QUADS);
		_functor.vertex(Vec3(0.0,0.0,topz)*matrix);
		_functor.vertex(Vec3(0.0,0.0,basez)*matrix);
		_functor.vertex(Vec3(firstX,firstY,basez)*matrix);
		_functor.vertex(Vec3(firstX,firstY,topz)*matrix);

		_functor.vertex(Vec3(lastX,lastY,topz)*matrix);
		_functor.vertex(Vec3(lastX,lastY,basez)*matrix);
		_functor.vertex(Vec3(0.0,0.0,basez)*matrix);
		_functor.vertex(Vec3(0.0,0.0,topz)*matrix);
		_functor.end();
	}
}

void CustomPrimitiveShape::createPieBody( unsigned int numSegments, glbFloat radius, glbFloat height,glbFloat sAngle,glbFloat eAngle, const osg::Matrix& matrix )
{
	const glbFloat angleDelta = (eAngle - sAngle)/(glbFloat)numSegments;
	//const glbFloat angleDelta = 2.0f*osg::PI/(glbFloat)numSegments;

	const glbFloat r = radius;
	const glbFloat h = height;

	glbFloat basez = 0/*-h*0.5f*/;
	glbFloat topz = h/*h*0.5f*/;

	glbFloat angle = sAngle;

	_functor.begin(GL_QUAD_STRIP);

	for(unsigned int bodyi=0;
		bodyi<numSegments + 1;
		++bodyi,angle+=angleDelta)
	{
		glbFloat c = cosf(angle);
		glbFloat s = sinf(angle);

		_functor.vertex(osg::Vec3(c*r,s*r,topz) * matrix);
		_functor.vertex(osg::Vec3(c*r,s*r,basez) * matrix);
	}

	_functor.end();
}

void CustomPrimitiveShape::PrimitiveBoxShape( const CustomBox& box)
{
	// evaluate hints
	bool createBody = (_hints ? _hints->getCreateBody() : true);
	bool createTop = (_hints ? _hints->getCreateTop() : true);
	bool createBottom = (_hints ? _hints->getCreateBottom() : true);

	Vec3 center = box.getCenter();
	float x = box.getHalfLengths().x();
	float y = box.getHalfLengths().y();
	float z = box.getHalfLengths().z();


	Vec3 base_1(y,x,-z);
	Vec3 base_2(y,-x,-z);
	Vec3 base_3(-y,-x,-z);
	Vec3 base_4(-y,x,-z);

	Vec3 top_1(-y,x,z);
	Vec3 top_2(-y,-x,z);
	Vec3 top_3(y,-x,z);
	Vec3 top_4(y,x,z);

	if (box.zeroRotation())
	{
		base_1 += osg::Vec3(center.x(),center.y(),center.z() + z);
		base_2 += osg::Vec3(center.x(),center.y(),center.z() + z);
		base_3 += osg::Vec3(center.x(),center.y(),center.z() + z);
		base_4 += osg::Vec3(center.x(),center.y(),center.z() + z);

		top_1 += osg::Vec3(center.x(),center.y(),center.z() + z);
		top_2 += osg::Vec3(center.x(),center.y(),center.z() + z);
		top_3 += osg::Vec3(center.x(),center.y(),center.z() + z);
		top_4 += osg::Vec3(center.x(),center.y(),center.z() + z);
	}
	else
	{
		Matrix matrix = box.computeRotationMatrix();
		matrix.setTrans(osg::Vec3(center.x(),center.y(),center.z() + z));

		base_1 = base_1*matrix;
		base_2 = base_2*matrix;
		base_3 = base_3*matrix;
		base_4 = base_4*matrix;

		top_1 = top_1*matrix;
		top_2 = top_2*matrix;
		top_3 = top_3*matrix;
		top_4 = top_4*matrix;
	}

	_functor.begin(GL_QUADS);
	if (createBody)
	{
		_functor.vertex(top_1);
		_functor.vertex(base_1);
		_functor.vertex(base_2);
		_functor.vertex(top_2);

		_functor.vertex(top_2);
		_functor.vertex(base_2);
		_functor.vertex(base_3);
		_functor.vertex(top_3);

		_functor.vertex(top_3);
		_functor.vertex(base_3);
		_functor.vertex(base_4);
		_functor.vertex(top_4);

		_functor.vertex(top_4);
		_functor.vertex(base_4);
		_functor.vertex(base_1);
		_functor.vertex(top_1);
	}

	if (createTop)
	{
		_functor.vertex(top_4);
		_functor.vertex(top_1);
		_functor.vertex(top_2);
		_functor.vertex(top_3);
	}

	if (createBottom)
	{
		_functor.vertex(base_4);
		_functor.vertex(base_3);
		_functor.vertex(base_2);
		_functor.vertex(base_1);
	}

	_functor.end();
}

void CustomPrimitiveShape::PrimitiveSphereShape( const CustomSphere& sphere)
{
	float tx = sphere.getCenter().x();
	float ty = sphere.getCenter().y();
	float tz = /*sphere.getCenter().z()*/sphere.getRadius();

	unsigned int numSegments = 40;
	unsigned int numRows = 20;
	float ratio = (_hints ? _hints->getDetailRatio() : 1.0f);
	if (ratio > 0.0f && ratio != 1.0f) {
		numRows = (unsigned int) (numRows * ratio);
		if (numRows < MIN_NUM_ROWS)
			numRows = MIN_NUM_ROWS;
		numSegments = (unsigned int) (numSegments * ratio);
		if (numSegments < MIN_NUM_SEGMENTS)
			numSegments = MIN_NUM_SEGMENTS;
	}

	float lDelta = osg::PI/(float)numRows;
	float vDelta = 1.0f/(float)numRows;

	float angleDelta = osg::PI*2.0f/(float)numSegments;

	float lBase=-osg::PI*0.5f;
	float rBase=0.0f;
	float zBase=-sphere.getRadius();
	float vBase=0.0f;

	for(unsigned int rowi=0;
		rowi<numRows;
		++rowi)
	{

		float lTop = lBase+lDelta;
		float rTop = cosf(lTop)*sphere.getRadius();
		float zTop = sinf(lTop)*sphere.getRadius();
		float vTop = vBase+vDelta;

		_functor.begin(GL_QUAD_STRIP);

		float angle = 0.0f;

		for(unsigned int topi=0;
			topi<numSegments;
			++topi,angle+=angleDelta)
		{

			float c = cosf(angle);
			float s = sinf(angle);

			_functor.vertex(tx+c*rTop,ty+s*rTop,tz+zTop);
			_functor.vertex(tx+c*rBase,ty+s*rBase,tz+zBase);

		}

		// do last point by hand to ensure no round off errors.
		_functor.vertex(tx+rTop,ty,tz+zTop);
		_functor.vertex(tx+rBase,ty,tz+zBase);

		_functor.end();

		lBase=lTop;
		rBase=rTop;
		zBase=zTop;
		vBase=vTop;

	}
}

GlbGlobe::GlobeShapeDrawable::GlobeShapeDrawable( Shape* shape, TessellationHints* hints/*=0*/ )
{
	setColor(Vec4(1.0f,1.0f,1.0f,1.0f));
	setTessellationHints(hints);
	setShape(shape);
}

GlbGlobe::GlobeShapeDrawable::~GlobeShapeDrawable()
{
}

void GlbGlobe::GlobeShapeDrawable::drawImplementation( RenderInfo& renderInfo ) const
{
	osg::State& state = *renderInfo.getState();
	GLBeginEndAdapter& gl = state.getGLBeginEndAdapter();

	if (_shape.valid())
	{
		const char *className = _shape->className();
		if (strcmp(className,"Sphere") == 0 ||
			strcmp(className,"Box") == 0 ||
			strcmp(className,"Cone") == 0 ||
			strcmp(className,"Cylinder") == 0 ||
			strcmp(className,"Sphere") == 0 ||
			strcmp(className,"InfinitePlane") == 0 ||
			strcmp(className,"TriangleMesh") == 0 ||
			strcmp(className,"ConvexHull") == 0 ||
			strcmp(className,"HeightField") == 0 ||
			strcmp(className,"CompositeShape") == 0 )
		{
			ShapeDrawable::drawImplementation(renderInfo);
		}
		else
		{
			gl.Color4fv(_color.ptr());
			CustomDrawShape cds(state,_tessellationHints.get());
			if (strcmp(className,"CustomCircle") == 0)
			{
				CustomCircle *customCircle = dynamic_cast<CustomCircle *>(_shape.get());
				if (customCircle)
					cds.DrawCircleShape(*customCircle);
			}
			else if (strcmp(className,"CustomRectangle") == 0)
			{
				CustomRectangle *customRectangle = dynamic_cast<CustomRectangle *>(_shape.get());
				if (customRectangle)
					cds.DrawRectangleShape(*customRectangle);
			}
			else if (strcmp(className,"CustomEllipse") == 0)
			{
				CustomEllipse *customEllipse = dynamic_cast<CustomEllipse *>(_shape.get());
				if (customEllipse)
					cds.DrawEllipseShape(*customEllipse);
			}
			else if (strcmp(className,"CustomArc") == 0)
			{
				CustomArc *customArc = dynamic_cast<CustomArc *>(_shape.get());
				if (customArc)
					cds.DrawArcShape(*customArc);
			}
			else if (strcmp(className,"CustomCone") == 0)
			{
				CustomCone *customCone = dynamic_cast<CustomCone *>(_shape.get());
				if (customCone)
					cds.DrawConeShape(*customCone);
			}
			else if (strcmp(className,"CustomCylinder") == 0)
			{
				CustomCylinder *customCylinder = dynamic_cast<CustomCylinder *>(_shape.get());
				if (customCylinder)
					cds.DrawCylinderShape(*customCylinder);
			}
			else if (strcmp(className,"CustomPyramid") == 0)
			{
				CustomPyramid *customPyramid = dynamic_cast<CustomPyramid *>(_shape.get());
				if(customPyramid)
					cds.DrawPyramidShape(*customPyramid);
			}
			else if (strcmp(className,"CustomPie") == 0)
			{
				CustomPie *customPie = dynamic_cast<CustomPie *>(_shape.get());
				if(customPie)
					cds.DrawPieShape(*customPie);
			}
			else if(strcmp(className,"CustomBox") == 0)
			{
				CustomBox *customBox = dynamic_cast<CustomBox *>(_shape.get());
				if(customBox)
					cds.DrawBoxShape(*customBox);
			}
			else if(strcmp(className,"CustomSphere") == 0)
			{
				CustomSphere *customSphere = dynamic_cast<CustomSphere *>(_shape.get());
				if(customSphere)
					cds.DrawSphereShape(*customSphere);
			}
		}
	}
}

void GlbGlobe::GlobeShapeDrawable::accept( PrimitiveFunctor& pf ) const
{
	if (_shape.valid())
	{
		const char *className = _shape->className();
		if (strcmp(className,"Sphere") == 0 ||
			strcmp(className,"Box") == 0 ||
			strcmp(className,"Cone") == 0 ||
			strcmp(className,"Cylinder") == 0 ||
			strcmp(className,"Sphere") == 0 ||
			strcmp(className,"InfinitePlane") == 0 ||
			strcmp(className,"TriangleMesh") == 0 ||
			strcmp(className,"ConvexHull") == 0 ||
			strcmp(className,"HeightField") == 0 ||
			strcmp(className,"CompositeShape") == 0 )
		{
			ShapeDrawable::accept(pf);
		}
		else
		{
			CustomPrimitiveShape cps(pf,_tessellationHints.get());
			if (strcmp(className,"CustomCircle") == 0)
			{
				CustomCircle *customCircle = dynamic_cast<CustomCircle *>(_shape.get());
				if (customCircle)
					cps.PrimitiveCircleShape(*customCircle);
			}
			else if (strcmp(className,"CustomRectangle") == 0)
			{
				CustomRectangle *customRectangle = dynamic_cast<CustomRectangle *>(_shape.get());
				if (customRectangle)
					cps.PrimitiveRectangleShape(*customRectangle);
			}
			else if (strcmp(className,"CustomEllipse") == 0)
			{
				CustomEllipse *customEllipse = dynamic_cast<CustomEllipse *>(_shape.get());
				if (customEllipse)
					cps.PrimitiveEllipseShape(*customEllipse);
			}
			else if (strcmp(className,"CustomArc") == 0)
			{
				CustomArc *customArc = dynamic_cast<CustomArc *>(_shape.get());
				if (customArc)
					cps.PrimitiveArcShape(*customArc);
			}
			else if (strcmp(className,"CustomCone") == 0)
			{
				CustomCone *customCone = dynamic_cast<CustomCone *>(_shape.get());
				if (customCone)
					cps.PrimitiveConeShape(*customCone);
			}
			else if (strcmp(className,"CustomCylinder") == 0)
			{
				CustomCylinder *customCylinder = dynamic_cast<CustomCylinder *>(_shape.get());
				if (customCylinder)
					cps.PrimitiveCylinderShape(*customCylinder);
			}
			else if (strcmp(className,"CustomPyramid") == 0)
			{
				CustomPyramid *customPyramid = dynamic_cast<CustomPyramid *>(_shape.get());
				if(customPyramid)
					cps.PrimitivePyramidShape(*customPyramid);
			}
			else if (strcmp(className,"CustomPie") == 0)
			{
				CustomPie *customPie = dynamic_cast<CustomPie *>(_shape.get());
				if(customPie)
					cps.PrimitivePieShape(*customPie);
			}
			else if(strcmp(className,"CustomBox") == 0)
			{
				CustomBox *customBox = dynamic_cast<CustomBox *>(_shape.get());
				if(customBox)
					cps.PrimitiveBoxShape(*customBox);
			}
			else if(strcmp(className,"CustomSphere") == 0)
			{
				CustomSphere *customSphere = dynamic_cast<CustomSphere *>(_shape.get());
				if(customSphere)
					cps.PrimitiveSphereShape(*customSphere);
			}
		}
	}
}

osg::BoundingBox GlbGlobe::GlobeShapeDrawable::computeBound() const
{
	BoundingBox bbox;
	if (_shape.valid())
	{
		const char *className = _shape->className();
		if (strcmp(className,"Sphere") == 0 ||
			strcmp(className,"Box") == 0 ||
			strcmp(className,"Cone") == 0 ||
			strcmp(className,"Cylinder") == 0 ||
			strcmp(className,"Sphere") == 0 ||
			strcmp(className,"InfinitePlane") == 0 ||
			strcmp(className,"TriangleMesh") == 0 ||
			strcmp(className,"ConvexHull") == 0 ||
			strcmp(className,"HeightField") == 0 ||
			strcmp(className,"CompositeShape") == 0 )
		{
			return ShapeDrawable::computeBound();
		}
		else
		{
			CustomComputeBoundShape ccbs(bbox);
			if (strcmp(className,"CustomCircle") == 0)
			{
				CustomCircle *customCircle = dynamic_cast<CustomCircle *>(_shape.get());
				if (customCircle)
					ccbs.ComputeCircleBound(*customCircle);
			}
			else if (strcmp(className,"CustomRectangle") == 0)
			{
				CustomRectangle *customRectangle = dynamic_cast<CustomRectangle *>(_shape.get());
				if (customRectangle)
					ccbs.ComputeRectangleBound(*customRectangle);
			}
			else if (strcmp(className,"CustomEllipse") == 0)
			{
				CustomEllipse *customEllipse = dynamic_cast<CustomEllipse *>(_shape.get());
				if (customEllipse)
					ccbs.ComputeEllipseBound(*customEllipse);
			}
			else if (strcmp(className,"CustomArc") == 0)
			{
				CustomArc *customArc = dynamic_cast<CustomArc *>(_shape.get());
				if (customArc)
					ccbs.ComputeArcBound(*customArc);
			}
			else if (strcmp(className,"CustomCone") == 0)
			{
				CustomCone *customCone = dynamic_cast<CustomCone*>(_shape.get());
				if (customCone)
					ccbs.ComputeConeBound(*customCone);
			}
			else if (strcmp(className,"CustomCylinder") == 0)
			{
				CustomCylinder *customCylinder = dynamic_cast<CustomCylinder*>(_shape.get());
				if(customCylinder)
					ccbs.ComputeCylinderBound(*customCylinder);
			}
			else if (strcmp(className,"CustomPyramid") == 0)
			{
				CustomPyramid *customPyramid = dynamic_cast<CustomPyramid*>(_shape.get());
				if(customPyramid)
					ccbs.ComputePyramidBound(*customPyramid);
			}
			else if (strcmp(className,"CustomPie") == 0)
			{
				CustomPie *customPie = dynamic_cast<CustomPie*>(_shape.get());
				if(customPie)
					ccbs.ComputePieBound(*customPie);
			}
			else if(strcmp(className,"CustomBox") == 0)
			{
				CustomBox *customBox = dynamic_cast<CustomBox*>(_shape.get());
				if(customBox)
					ccbs.ComputeBoxBound(*customBox);
			}
			else if(strcmp(className,"CustomSphere") == 0)
			{
				CustomSphere *customSphere = dynamic_cast<CustomSphere*>(_shape.get());
				if(customSphere)
					ccbs.ComputeSphereBound(*customSphere);
			}
		}
	}
	return bbox;
}
