#include "StdAfx.h"
#include "GlbGlobeSymbolCommon.h"


#include <osgDB/ReadFile>
#include <osgDB/FileUtils>
#include <osg/Material>


#include <iterator>

bool isFloatEqual(glbFloat leftValue,glbFloat rightValue)
{
	if (fabs(leftValue - rightValue) > 0.000001)
		return false;
	return true;
}

bool isDoubleEqual(glbDouble leftValue,glbDouble rightValue)
{
	if (fabs(leftValue - rightValue) > 0.000000000001)
		return false;
	return true;
}

osg::Image *loadImage(const CGlbWString &name)
{
	if (name.empty()||name==L"")
	{
		return NULL;
	}

	CGlbWString nameTmp=name;
	osg::Image *pImg=osgDB::readImageFile(nameTmp.ToString().c_str());
	return pImg;

}

osg::Texture2D *loadTexture(osg::Image *pImg, osg::Texture::WrapMode wrap)
{
	//if (pImg!=NULL)
	{
		osg::Texture2D* tex = new osg::Texture2D();

		tex->setFilter(osg::Texture::MIN_FILTER, osg::Texture::LINEAR_MIPMAP_LINEAR);
		tex->setFilter(osg::Texture::MAG_FILTER, osg::Texture::LINEAR);
		tex->setWrap  (osg::Texture::WRAP_S,     wrap);
		tex->setWrap  (osg::Texture::WRAP_T,     wrap);
		tex->setImage (pImg);

		return tex;
	}
	return NULL;
}

osg::Texture2D *readTexture(const CGlbWString &name, osg::Texture::WrapMode wrap)
{
	CGlbWString nameTmp=name;
	osg::Image *pImg=loadImage(name);
	if (pImg!=NULL)
	{
		osg::Texture2D* tex = new osg::Texture2D();

		tex->setFilter(osg::Texture::MIN_FILTER, osg::Texture::LINEAR_MIPMAP_LINEAR);
		tex->setFilter(osg::Texture::MAG_FILTER, osg::Texture::LINEAR);
		tex->setWrap  (osg::Texture::WRAP_S,     wrap);
		tex->setWrap  (osg::Texture::WRAP_T,     wrap);
		tex->setImage (pImg);

		return tex;
	}
	return NULL;	
}

osg::Shader* readShaderFile(const std::string& filename)
{
	// The .vert and .frag extensions were added to the GLSL plugin in OSG 
	// 2.7.3, and the automatic setting of shader type depending on the 
	// extension was added in OSG 2.9.1. The code below lets us use OSG
	// 2.6 and still get the same behavior.
#if OPENSCENEGRAPH_MAJOR_VERSION > 2 || \
	(OPENSCENEGRAPH_MAJOR_VERSION == 2 && OPENSCENEGRAPH_MINOR_VERSION > 9) || \
	(OPENSCENEGRAPH_MAJOR_VERSION == 2 && OPENSCENEGRAPH_MINOR_VERSION == 9 && OPENSCENEGRAPH_PATCH_VERSION >= 1)
	// This will search the registry's file path.
	return osgDB::readShaderFile(filename);
#else
	// Determine shader type from the extension
	osg::Shader::Type type = osg::Shader::UNDEFINED;
	if (filename.find("vert") == filename.length() - 4)
		type = osg::Shader::VERTEX;
	else if (filename.find("frag") == filename.length() - 4)
		type = osg::Shader::FRAGMENT;
	else
		return NULL;

	// Find the shader file in the osgDB data path list.
	std::string fullpath = osgDB::findDataFile(filename);
	if (fullpath.empty())
		return NULL;

	// Read the shader file.
	osg::Shader* shader = osg::Shader::readShaderFile(type, fullpath);
	return shader;
#endif
}

/** Creates a shader program using either the given strings as shader 
*  source directly, or as filenames to load the shaders from disk, 
*  depending on the value of the \c loadFromFiles parameter.
*/
osg::Program* createShaderProgram( const std::string& name,const std::string& vertexSrc, const std::string& fragmentSrc, bool loadFromFiles )
{
	osg::ref_ptr<osg::Shader> vShader = NULL;
	osg::ref_ptr<osg::Shader> fShader = NULL;

	if (loadFromFiles)
	{
		vShader = readShaderFile(vertexSrc);
		if (!vShader)
		{
			//osg::notify(osg::WARN) << "Could not read shader from file " << vertexSrc << std::endl;
			//return NULL;
		}

		fShader = readShaderFile(fragmentSrc);
		if (!fShader)
		{
			//osg::notify(osg::WARN) << "Could not read shader from file " << fragmentSrc << std::endl;
			//return NULL;
		}
	}
	else
	{
		if (!vertexSrc.empty())
		{
			vShader = new osg::Shader( osg::Shader::VERTEX, vertexSrc );
		}
		if (!fragmentSrc.empty())
		{
			fShader = new osg::Shader( osg::Shader::FRAGMENT, fragmentSrc );
		}
	}

	osg::Program* program = new osg::Program;
	program->setName(name);

	//std::string globalDefinitionsList = buildGlobalDefinitionsList(name);
	if (vShader.valid())
	{
		//vShader->setShaderSource(globalDefinitionsList + vShader->getShaderSource());
		vShader->setName(name+"_vertex_shader");
		program->addShader( vShader.get() );
	}
	if (fShader.valid())
	{
		//fShader->setShaderSource(globalDefinitionsList + fShader->getShaderSource());
		fShader->setName(name+"_fragment_shader");
		program->addShader( fShader.get() );
	}

	return program;
}

std::string getDepthVertexShaderSource()
{
	return "varying vec4 sceneDepth;\n"
	"void main( void )\n"
	"{\n"
		"gl_Position = ftransform();\n"
		"sceneDepth.x = gl_Position.z;\n"
		"sceneDepth.y = gl_Position.w;\n"
	"}\n";
}
std::string getDepthFragmentShaderSource()
{
	return "varying vec4 sceneDepth;\n"
	"void main( void )\n"
	"{\n"
		"float finalDepth = sceneDepth.x / sceneDepth.y;\n"
		"gl_FragColor = vec4(finalDepth, finalDepth, finalDepth, 1.0);\n"
	"}\n";
}

glbUInt32 DelaunayTriangulator_uniqueifyPoints(osg::Vec3Array *points)
{
	if (points==NULL)
		return 0;
	
	glbUInt32 uniqueCount=points->size();
	if (uniqueCount<2)
		return uniqueCount;
	osg::ref_ptr<osg::Vec3Array> pointsTmp=new osg::Vec3Array();
	for (size_t i=0;i<uniqueCount;++i)// 不改变原来数据的值
	{
		pointsTmp->push_back(points->at(i));
	}
	
	std::sort( pointsTmp->begin(), pointsTmp->end() );

	//osg::ref_ptr<osg::Vec3Array> temppts = new osg::Vec3Array;
	// This won't work... must write our own unique() that compares only the first
	// two terms of a Vec3 for equivalency
	//std::insert_iterator< osg::Vec3Array > ti( *(temppts.get()), temppts->begin() );
	//std::unique_copy( points_->begin(), points_->end(), ti );

	osg::Vec3Array::iterator p = pointsTmp->begin();
	osg::Vec3 v = *p;
	// Always push back the first point
	//temppts->push_back( (v = *p));
	uniqueCount=1;
	for( ; p != pointsTmp->end(); p++ )
	{
		if( v[0] == (*p)[0] && v[1] == (*p)[1] )
			continue;

		//temppts->push_back( (v = *p));
		v=*p;
		++uniqueCount;
	}

	return uniqueCount;

	/*points_->clear();
	std::insert_iterator< osg::Vec3Array > ci(*(points_.get()),points_->begin());
	std::copy( temppts->begin(), temppts->end(), ci );*/
}
glbUInt32 DelaunayTriangulator_uniqueifyPoints(osg::Vec3dArray *points)
{
	if (points==NULL)
		return 0;
	
	glbUInt32 uniqueCount=points->size();
	if (uniqueCount<2)
		return uniqueCount;
	osg::ref_ptr<osg::Vec3dArray> pointsTmp=new osg::Vec3dArray();
	for (size_t i=0;i<uniqueCount;++i)// 不改变原来数据的值
	{
		pointsTmp->push_back(points->at(i));
	}
	
	std::sort( pointsTmp->begin(), pointsTmp->end() );

	//osg::ref_ptr<osg::Vec3Array> temppts = new osg::Vec3Array;
	// This won't work... must write our own unique() that compares only the first
	// two terms of a Vec3 for equivalency
	//std::insert_iterator< osg::Vec3Array > ti( *(temppts.get()), temppts->begin() );
	//std::unique_copy( points_->begin(), points_->end(), ti );

	osg::Vec3dArray::iterator p = pointsTmp->begin();
	osg::Vec3d v = *p;
	// Always push back the first point
	//temppts->push_back( (v = *p));
	uniqueCount=1;
	for( ; p != pointsTmp->end(); p++ )
	{
		if( v[0] == (*p)[0] && v[1] == (*p)[1] )
			continue;

		//temppts->push_back( (v = *p));
		v=*p;
		++uniqueCount;
	}

	return uniqueCount;

	/*points_->clear();
	std::insert_iterator< osg::Vec3Array > ci(*(points_.get()),points_->begin());
	std::copy( temppts->begin(), temppts->end(), ci );*/
}

osg::Geode * CreateBoundingBox( CGlbExtent extent,glbBool isWorld )
{
	osg::Vec3d center(0.0,0.0,0.0),extentMin(0.0,0.0,0.0),extentMax(0.0,0.0,0.0);
	extent.GetCenter(&center.x(),&center.y(),&center.z());
	osg::Matrixd localToworld,worldTolocal;
	if(!isWorld)
		g_ellipsoidModel->computeLocalToWorldTransformFromLatLongHeight(
		osg::DegreesToRadians(center.y()),osg::DegreesToRadians(center.x()),center.z(),localToworld);
	else
		g_ellipsoidModel->computeLocalToWorldTransformFromXYZ(center.x(),center.y(),center.z(),localToworld);
	worldTolocal = osg::Matrixd::inverse(localToworld);
	extent.GetMin(&extentMin.x(),&extentMin.y(),&extentMin.z());
	extent.GetMax(&extentMax.x(),&extentMax.y(),&extentMax.z());
	osg::ref_ptr<osg::Geometry> geom = new osg::Geometry();
	osg::ref_ptr<osg::Vec3dArray> coords = new osg::Vec3dArray();
	coords->push_back(osg::Vec3d(extentMin.x(),extentMin.y(),extentMax.z()));
	coords->push_back(osg::Vec3d(extentMax.x(),extentMin.y(),extentMax.z()));
	coords->push_back(osg::Vec3d(extentMax.x(),extentMin.y(),extentMin.z()));
	coords->push_back(osg::Vec3d(extentMin.x(),extentMin.y(),extentMin.z()));
	coords->push_back(osg::Vec3d(extentMin.x(),extentMax.y(),extentMax.z()));
	coords->push_back(osg::Vec3d(extentMax.x(),extentMax.y(),extentMax.z()));
	coords->push_back(osg::Vec3d(extentMax.x(),extentMax.y(),extentMin.z()));
	coords->push_back(osg::Vec3d(extentMin.x(),extentMax.y(),extentMin.z()));
	osg::ref_ptr<osg::Vec3dArray> renderVertexes = new osg::Vec3dArray;
	for (glbInt32 i = 0; i < coords->size(); i++)
	{
		osg::Vec3d point = coords->at(i);
		if(!isWorld)
		{
			g_ellipsoidModel->convertLatLongHeightToXYZ(
				osg::DegreesToRadians(point.y()),osg::DegreesToRadians(point.x()),point.z(),point.x(),point.y(),point.z());
			renderVertexes->push_back(point * worldTolocal);
		}
		else
			renderVertexes->push_back(point - center);
	}
	geom->setVertexArray(renderVertexes.get());
	osg::ref_ptr<osg::DrawElementsUShort> drawElements1 = new osg::DrawElementsUShort(osg::PrimitiveSet::LINE_LOOP);
	drawElements1->push_back(0);
	drawElements1->push_back(1);
	drawElements1->push_back(2);
	drawElements1->push_back(3);
	osg::ref_ptr<osg::DrawElementsUShort> drawElements2 = new osg::DrawElementsUShort(osg::PrimitiveSet::LINE_LOOP);
	drawElements2->push_back(4);
	drawElements2->push_back(5);
	drawElements2->push_back(6);
	drawElements2->push_back(7);
	osg::ref_ptr<osg::DrawElementsUShort> drawElements3 = new osg::DrawElementsUShort(osg::PrimitiveSet::LINES);
	drawElements3->push_back(0);
	drawElements3->push_back(4);
	drawElements3->push_back(1);
	drawElements3->push_back(5);
	drawElements3->push_back(2);
	drawElements3->push_back(6);
	drawElements3->push_back(3);
	drawElements3->push_back(7);
	geom->addPrimitiveSet(drawElements1.get());
	geom->addPrimitiveSet(drawElements2.get());
	geom->addPrimitiveSet(drawElements3.get());
	//osg::ref_ptr<osg::Vec4Array> vc = new osg::Vec4Array();
	//vc->push_back(osg::Vec4(1.0,1.0,1.0,1.0));
	//geom->setColorArray(vc.get());
	//geom->setColorBinding(osg::Geometry::BIND_OVERALL);
	osg::Geode *geode = new osg::Geode();
	osg::ref_ptr<osg::StateSet> stateset = geom->getOrCreateStateSet();
	osg::ref_ptr<osg::Material> material = dynamic_cast<osg::Material *>(stateset->getAttribute(osg::StateAttribute::MATERIAL));
	if(material == NULL)
		material = new osg::Material;
	stateset->setAttribute(material,osg::StateAttribute::ON | osg::StateAttribute::PROTECTED);
	material->setEmission(osg::Material::FRONT_AND_BACK,osg::Vec4(1,1,0,1));
	material->setDiffuse(osg::Material::FRONT_AND_BACK,osg::Vec4(0,0,0,0));
	geode->addDrawable(geom.get());
	return geode;
}
