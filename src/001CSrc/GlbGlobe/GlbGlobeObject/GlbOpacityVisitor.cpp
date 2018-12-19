#include "StdAfx.h"
#include "GlbOpacityVisitor.h"
#include <osg/NodeVisitor>
#include <osg/Material>
#include <osg/BlendFunc>

using namespace GlbGlobe;

CGlbOpacityVisitor::CGlbOpacityVisitor(glbInt32 opacity)
	: NodeVisitor(NodeVisitor::TRAVERSE_ALL_CHILDREN)
{
	mpr_opacity = opacity;
}

CGlbOpacityVisitor::~CGlbOpacityVisitor(void)
{
}

void CGlbOpacityVisitor::changeMaterialAlpha(osg::StateSet* ss, glbInt32 opacity)
{
	if (ss==NULL) return;
	osg::ref_ptr<osg::Material> material = dynamic_cast<osg::Material *>(ss->getAttribute(osg::StateAttribute::MATERIAL));
	if(material == NULL)
		return;		
	float opa = opacity / 100.0;
	osg::Material::Face dif_face = material->getDiffuseFrontAndBack()? osg::Material::FRONT_AND_BACK : osg::Material::FRONT;
    osg::Material::Face amb_face = material->getAmbientFrontAndBack()? osg::Material::FRONT_AND_BACK : osg::Material::FRONT;
	osg::Material::Face spe_face = material->getSpecularFrontAndBack()? osg::Material::FRONT_AND_BACK : osg::Material::FRONT;
	osg::Vec4 diffuse = (osg::Vec4) material->getDiffuse(dif_face);
	osg::Vec4 ambient = (osg::Vec4) material->getAmbient(amb_face);
	osg::Vec4 specular = (osg::Vec4) material->getSpecular(spe_face);
	material->setDiffuse(amb_face,osg::Vec4(diffuse.r(), diffuse.g() ,diffuse.b() ,opa));
	material->setAmbient(dif_face,osg::Vec4(ambient.r(), ambient.g() ,ambient.b() ,opa));
	material->setSpecular(spe_face,osg::Vec4(specular.r(), specular.g(), specular.b() ,opa));

	// 透明效果
	if (opacity<100)
	{// 半透明			
		if (osg::StateSet::TRANSPARENT_BIN != ss->getRenderingHint())
		{
			ss->setMode(GL_BLEND, osg::StateAttribute::ON);		
			ss->setRenderingHint( osg::StateSet::TRANSPARENT_BIN );
			osg::ref_ptr<osg::BlendFunc> bf = new osg::BlendFunc;
			ss->setAttributeAndModes(bf.get(),osg::StateAttribute::ON);
		}
	}
	else
	{// 不透明										
		if (osg::StateSet::OPAQUE_BIN != ss->getRenderingHint())
		{
			ss->setMode(GL_BLEND, osg::StateAttribute::OFF);
			ss->setRenderingHint( osg::StateSet::OPAQUE_BIN );	
		}
	}
	//ss->setMode(GL_LIGHTING, osg::StateAttribute::ON);	
}

