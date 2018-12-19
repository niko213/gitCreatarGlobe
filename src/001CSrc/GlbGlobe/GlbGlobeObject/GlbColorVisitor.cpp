#include "StdAfx.h"
#include "GlbColorVisitor.h"
#include <osg/NodeVisitor>
#include <osg/Material>

using namespace GlbGlobe;

CGlbColorVisitor::CGlbColorVisitor(glbInt32 r, glbInt32 g, glbInt32 b)
	: NodeVisitor(NodeVisitor::TRAVERSE_ALL_CHILDREN)
{
	mpr_red = r;
	mpr_green = g;
	mpr_blue = b;
}


CGlbColorVisitor::~CGlbColorVisitor(void)
{
}

void CGlbColorVisitor::changeMaterialColor(osg::StateSet* ss, glbInt32 red,glbInt32 green,glbInt32 blue)
{
	if (ss==NULL) return;
	osg::ref_ptr<osg::Material> material = dynamic_cast<osg::Material *>(ss->getAttribute(osg::StateAttribute::MATERIAL));
	if(material == NULL)
		return;		
	osg::ref_ptr<osg::Texture> texture = dynamic_cast<osg::Texture *>(ss->getAttribute(osg::StateAttribute::TEXTURE));
	if (texture != NULL)
		return;
	
	float r = red / 255.0f;
	float g = green / 255.0f;
	float b = blue / 255.0f;
	osg::Material::Face dif_face = material->getDiffuseFrontAndBack()? osg::Material::FRONT_AND_BACK : osg::Material::FRONT;
	osg::Material::Face amb_face = material->getAmbientFrontAndBack()? osg::Material::FRONT_AND_BACK : osg::Material::FRONT;
	osg::Material::Face spe_face = material->getSpecularFrontAndBack()? osg::Material::FRONT_AND_BACK : osg::Material::FRONT;
	osg::Vec4 diffuse = (osg::Vec4) material->getDiffuse(dif_face);
	osg::Vec4 ambient = (osg::Vec4) material->getAmbient(amb_face);
	osg::Vec4 specular = (osg::Vec4) material->getSpecular(spe_face);
	material->setDiffuse(dif_face,osg::Vec4(r, g ,b ,diffuse.a()));
	//material->setEmission(dif_face,osg::Vec4(r, g ,b ,diffuse.a()));
		
	//material->setAmbient(amb_face,osg::Vec4(r, g ,b ,ambient.a()));
	//material->setSpecular(spe_face,osg::Vec4(r, g, b ,specular.a()));
	ss->setAttributeAndModes(material,osg::StateAttribute::ON);

	ss->setMode(GL_LIGHTING,osg::StateAttribute::ON);
}