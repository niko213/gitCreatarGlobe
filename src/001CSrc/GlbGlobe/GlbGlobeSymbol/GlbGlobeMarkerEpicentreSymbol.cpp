#include "StdAfx.h"
#include "GlbPoint.h"
#include "GlbGlobeMarkerEpicentreSymbol.h"
#include <osg/Material>
#include <osg/MatrixTransform>

using namespace GlbGlobe;

class EpicentreUpdateCallback : public osg::NodeCallback
{
public:
	EpicentreUpdateCallback(glbInt32 clr)
	{
		//step = 1.0f;
		scale = 1.0f;
		bInvert = false;		
		color = GetColor(clr);
		srand((unsigned) time(NULL));
	}
	virtual void operator()(osg::Node* node, osg::NodeVisitor* nv)
	{ 
		//std::cout<<"update callback - pre traverse"<<node<<std::endl;
		osg::MatrixTransform* mt = dynamic_cast<osg::MatrixTransform*>(node);
		if (mt)
		{
			osg::StateSet *stateset = mt->getOrCreateStateSet();
			osg::ref_ptr<osg::Material> material = dynamic_cast<osg::Material *>(stateset->getAttribute(osg::StateAttribute::MATERIAL));
			
			if (material)
			{
				float r = float(rand())/RAND_MAX;
				float g = float(rand())/RAND_MAX;
				float b = float(rand())/RAND_MAX;
				if (bInvert)
					material->setDiffuse(osg::Material::FRONT_AND_BACK,osg::Vec4(r,g,b,color.a()));
				else
					material->setDiffuse(osg::Material::FRONT_AND_BACK,color);

				bInvert = !bInvert;
/*				if (bInvert){
					step += 0.01f;					
					if (step >= 1.0)	bInvert = !bInvert;					
				}
				else{
					step -= 0.01f;
					if (step <= 0.3)	bInvert = !bInvert;
				}	*/			
			}			
			float ss = float(rand())/RAND_MAX - 0.5; //[-0.5 - 0.5]
			scale = 1 + ss;
			osg::Matrix m;
			m.makeScale(scale,scale,scale);
			mt->setMatrix(m);
		}
		traverse(node,nv);
		//std::cout<<"update callback - post traverse"<<node<<std::endl;
	}
	private:
		//float step;
		osg::Vec4 color;
		bool bInvert;
		float scale;
};


CGlbGlobeMarkerEpicentreSymbol::CGlbGlobeMarkerEpicentreSymbol(void)
{
}


CGlbGlobeMarkerEpicentreSymbol::~CGlbGlobeMarkerEpicentreSymbol(void)
{
}

osg::Node *CGlbGlobeMarkerEpicentreSymbol::Draw(CGlbGlobeRObject *obj,IGlbGeometry *geo)
{
	GlbRenderInfo *renderInfo = obj->GetRenderInfo();
	GlbMarkerEpicentreSymbolInfo *markerEpicentreInfo = dynamic_cast<GlbMarkerEpicentreSymbolInfo *>(renderInfo);

	if(markerEpicentreInfo == NULL)
		return NULL;

	CGlbFeature *feature = obj->GetFeature();
	glbDouble radius;
	if (markerEpicentreInfo->radius)
		radius = markerEpicentreInfo->radius->GetValue(feature);
	glbInt32 color;
	if (markerEpicentreInfo->color)
		color = markerEpicentreInfo->color->GetValue(feature);

/*	CGlbPoint* pt = dynamic_cast<CGlbPoint*>(geo);
	if (pt==NULL) return NULL;
	osg::Vec3d pos;
	pt->GetXYZ(&pos.x(),&pos.y(),&pos.z());		*/	
	
	osg::Node* epicentreNode = createEpicentreNode(radius,color);

	return epicentreNode;
}

#include <osg/ShapeDrawable>
#include <osgDB/WriteFile>
#include <osg/TextureCubeMap>
#include <osg/TexGen>
// 生成一个震源球,添加一个回调修改颜色
osg::Node* CGlbGlobeMarkerEpicentreSymbol::createEpicentreNode(glbDouble radius, glbInt32 clr)
{
	osg::ref_ptr<osg::MatrixTransform> epicentreNode = new osg::MatrixTransform();

	osg::ref_ptr<osg::Geode> geode = new osg::Geode();
	epicentreNode->addChild(geode.get());
	osg::ref_ptr<osg::ShapeDrawable> drawable = new osg::ShapeDrawable(new osg::Sphere(osg::Vec3(0,0,0), radius));
	geode->addDrawable(drawable.get());
	// material 红色
	osg::StateSet *stateset = epicentreNode->getOrCreateStateSet();
	osg::ref_ptr<osg::Material> material = new osg::Material();//dynamic_cast<osg::Material *>(stateset->getAttribute(osg::StateAttribute::MATERIAL));

	osg::Vec4 cc = GetColor(clr);
	material->setDiffuse(osg::Material::FRONT_AND_BACK,cc/*osg::Vec4(1,0,0,0.5)*/);
	//material->setAlpha(osg::Material::FRONT_AND_BACK,0.5);
	stateset->setAttribute(material.get(),osg::StateAttribute::ON);
	stateset->setMode(GL_LIGHTING,osg::StateAttribute::ON);

	//{
	//	unsigned int tex_width = 512;
	//	unsigned int tex_height = 512;

	//	osg::TextureCubeMap* texture = new osg::TextureCubeMap;
	//	texture->setTextureSize(tex_width, tex_height);
	//	texture->setInternalFormat(GL_RGBA);
	//	texture->setWrap(osg::Texture::WRAP_S, osg::Texture::CLAMP);
	//	texture->setWrap(osg::Texture::WRAP_T, osg::Texture::CLAMP);
	//	texture->setWrap(osg::Texture::WRAP_R, osg::Texture::CLAMP);
	//	texture->setFilter(osg::TextureCubeMap::MIN_FILTER,osg::TextureCubeMap::LINEAR);
	//	texture->setFilter(osg::TextureCubeMap::MAG_FILTER,osg::TextureCubeMap::LINEAR);

	//	stateset->setTextureMode(0, GL_TEXTURE_GEN_S, osg::StateAttribute::ON);
	//	stateset->setTextureMode(0, GL_TEXTURE_GEN_T, osg::StateAttribute::ON);
	//	stateset->setTextureAttributeAndModes(0,texture,osg::StateAttribute::ON);

	//	osg::TexGen *tg = new osg::TexGen;
	//	tg->setMode(osg::TexGen::NORMAL_MAP);
	//	stateset->setTextureAttributeAndModes(0, tg, osg::StateAttribute::ON);

	//	stateset->setMode(GL_LIGHTING,osg::StateAttribute::OFF);
	//}

	EpicentreUpdateCallback* _callback = new EpicentreUpdateCallback(clr);
	// 添加回调变换颜色 - 修改材质颜色，透明度
	epicentreNode->setUpdateCallback(_callback);

	//stateset->setMode(GL_BLEND,osg::StateAttribute::ON);
	//stateset->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);

	//bool res = osgDB::writeNodeFile(*geode.get(),"f:\\epicentre.osg");
	//osgDB::writeObjectFile(*_callback,"f:\\epicentre.osgt");

	return epicentreNode.release();
}
