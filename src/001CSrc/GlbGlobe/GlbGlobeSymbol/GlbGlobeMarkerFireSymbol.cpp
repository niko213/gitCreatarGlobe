#include "StdAfx.h"
#include "GlbGlobeMarkerFireSymbol.h"
#include "GlbPoint.h"
#include "GlbGlobeObjectRenderInfos.h"

#include <osgParticle/ExplosionDebrisEffect>
#include <osgParticle/ExplosionEffect>
#include <osgParticle/FireEffect>

using namespace GlbGlobe;

CGlbGlobeMarkerFireSymbol::CGlbGlobeMarkerFireSymbol(void)
{
}


CGlbGlobeMarkerFireSymbol::~CGlbGlobeMarkerFireSymbol(void)
{
}

osg::Node *CGlbGlobeMarkerFireSymbol::Draw(CGlbGlobeRObject *obj,IGlbGeometry *geo)
{
	GlbRenderInfo *renderInfo = obj->GetRenderInfo();
	GlbMarkerFireSymbolInfo *markerFireInfo = dynamic_cast<GlbMarkerFireSymbolInfo *>(renderInfo);

	if(markerFireInfo == NULL)
		return NULL;

	CGlbFeature *feature = obj->GetFeature();
	glbDouble scale = 1.0;
	if (markerFireInfo->scale)
		scale = markerFireInfo->scale->GetValue(feature);
	glbDouble intensity = 1.0;
	if (markerFireInfo->intensity)
		intensity = markerFireInfo->intensity->GetValue(feature);

	std::vector<osg::Vec3> positions;
	GlbGeometryTypeEnum geoType = geo->GetType();
	switch(geoType)
	{
	case GLB_GEO_POINT:
		{
			CGlbPoint* pt = dynamic_cast<CGlbPoint*>(geo);
			if(pt)
			{
				osg::Vec3d pos;
				pt->GetXYZ(&pos.x(),&pos.y(),&pos.z());				
				positions.push_back(pos);
			}
		}
		break;
	case GLB_GEO_MULTIPOINT:
		{
			CGlbMultiPoint* mulPt = dynamic_cast<CGlbMultiPoint*>(geo);
			if (mulPt)
			{
				osg::Vec3d pos;
				glbInt32 cnt = mulPt->GetCount();
				for (glbInt32 idx = 0; idx < cnt; idx++)
				{
					mulPt->GetPoint(idx,&pos.x(),&pos.y(),&pos.z());
					positions.push_back(pos);
				}
			}
		}
		break;
	}	
	
	if (positions.size()<=0) return NULL;

	osg::Group* fireGroupNode = new osg::Group();

	for (size_t k = 0; k < positions.size(); k++)
	{
		osg::Vec3 pos = positions.at(k);

		// 爆炸模拟， 10.0f为缩放比，默认为1.0f，不缩放
		osg::ref_ptr<osgParticle::ExplosionEffect> explosion = new osgParticle::ExplosionEffect(pos,scale,intensity);		

		// 碎片模拟
		osg::ref_ptr<osgParticle::ExplosionDebrisEffect> explosionDebris = new osgParticle::ExplosionDebrisEffect(pos,intensity);

		// 火焰模拟
		osg::ref_ptr<osgParticle::FireEffect> fire = new osgParticle::FireEffect(pos,scale,intensity);

		// 火焰不熄灭
		fire->getEmitter()->setEndless(true);

		fire->setEmitterDuration(10000000);
		// 设置风向		
		fire->setTextureFileName("res/smoke.rgb");
		//fire->setWind(wind);

		// 添加子节点
		fireGroupNode->addChild(fire.get());		
		fireGroupNode->addChild(explosion.get());
		fireGroupNode->addChild(explosionDebris.get());
	}
	return fireGroupNode;	
}
