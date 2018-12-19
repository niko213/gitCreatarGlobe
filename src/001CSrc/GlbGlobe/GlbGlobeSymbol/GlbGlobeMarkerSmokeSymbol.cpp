#include "StdAfx.h"
#include "GlbGlobeMarkerSmokeSymbol.h"
#include "GlbPoint.h"
#include "CGlbGlobe.h"
#include "GlbGlobeView.h"
#include "GlbGlobeObjectRenderInfos.h"
#include <osgParticle/ExplosionEffect>

using namespace GlbGlobe;

CGlbGlobeMarkerSmokeSymbol::CGlbGlobeMarkerSmokeSymbol(void)
{
}


CGlbGlobeMarkerSmokeSymbol::~CGlbGlobeMarkerSmokeSymbol(void)
{
}


osg::Node *CGlbGlobeMarkerSmokeSymbol::Draw(CGlbGlobeRObject *obj,IGlbGeometry *geo)
{
	GlbRenderInfo *renderInfo = obj->GetRenderInfo();
	GlbMarkerSmokeSymbolInfo *markerSmokeInfo = dynamic_cast<GlbMarkerSmokeSymbolInfo *>(renderInfo);

	if(markerSmokeInfo == NULL)
		return NULL;

	GlbGlobeTypeEnum globeType = obj->GetGlobe()->GetType();

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

	osg::Group* smokeGroupNode = new osg::Group();

	float scale = 1.0f;
	float intensity = 1.0f;

	CGlbFeature* feature = obj->GetFeature();
	glbInt32 startColor = RGB(25,25,25);
	if (markerSmokeInfo->startColor)
		startColor = markerSmokeInfo->startColor->GetValue(feature);
	glbInt32 endColor = RGB(100,100,100);
	if (markerSmokeInfo->endColor)
		endColor = markerSmokeInfo->endColor->GetValue(feature);
	if (markerSmokeInfo->scale)
		scale = markerSmokeInfo->scale->GetValue(feature);
	if (markerSmokeInfo->intensity)
		intensity = markerSmokeInfo->intensity->GetValue(feature);

	osg::Vec3 _c1,_c2;
	osg::Vec4 cc = GetColor(startColor);
	_c1.set(cc.r(),cc.g(),cc.b());
	cc = GetColor(endColor);
	_c2.set(cc.r(),cc.g(),cc.b());

	// 风向
	osg::Vec3 wind(0.0f,0.0f,1.0f);	
	if (globeType == GLB_GLOBETYPE_GLOBE)
	{		
		wind = positions.at(0);
		wind.normalize();
		//wind.set(wind.x(),wind.y(),wind.z()+0.1);
		//wind.normalize();
	}

	for (size_t k = 0; k < positions.size(); k++)
	{
		osg::Vec3 pos = positions.at(k);			

		// 爆炸模拟， 10.0f为缩放比，默认为1.0f，不缩放
		osg::ref_ptr<osgParticle::ExplosionEffect> explosion = new osgParticle::ExplosionEffect(pos,scale,1);

		// 自定义烟雾对象
		osg::ref_ptr<osg::Group> smoke = CreateMySmokeParticle(pos,_c1,_c2,scale,intensity);

		explosion->setWind(wind);

		smokeGroupNode->addChild(explosion.get());
		smokeGroupNode->addChild(smoke.get());

	}
	return smokeGroupNode;	
}

#include <osgParticle/Particle>
#include <osgParticle/ParticleSystem>
#include <osgParticle/ParticleSystemUpdater>
#include <osgParticle/ModularEmitter>
#include <osgParticle/ModularProgram>
#include <osgParticle/RandomRateCounter>
#include <osgParticle/SectorPlacer>
#include <osgParticle/RadialShooter>
#include <osgParticle/AccelOperator>
#include <osgParticle/FluidFrictionOperator>
#include <osgParticle/BoxPlacer>

// 创建自定义烟雾粒子系统
osg::ref_ptr<osg::Group> CGlbGlobeMarkerSmokeSymbol::CreateMySmokeParticle(osg::Vec3 pos, osg::Vec3 startColor, osg::Vec3 endColor,float scale, float intensity)
{
	osg::ref_ptr<osg::Group> smokeNode = new osg::Group();

	// 创建粒子系统模板
	osgParticle::Particle ptemplate;
	// 设置生命周期
	ptemplate.setLifeTime(1.5*scale);
	// 设置粒子大小变化范围
	ptemplate.setSizeRange(osgParticle::rangef(0.75f*scale,3.0f*scale)); // 0.75-3.0
	// 设置粒子alpha变化范围
	ptemplate.setAlphaRange(osgParticle::rangef(0.0f,1.0f));
	float _rclr = 0.4f;//0.2f + rand()/double(RAND_MAX);//暗黑色烟雾
	// 设置粒子颜色变化范围
	//ptemplate.setColorRange(osgParticle::rangev4(osg::Vec4(0.1f,0.1f,0.1f,1.0f), osg::Vec4(_rclr,_rclr,_rclr,0.0f)));
	ptemplate.setColorRange(osgParticle::rangev4(osg::Vec4(startColor.x(),startColor.y(),startColor.z(),1.0f), osg::Vec4(endColor.x(),endColor.y(),endColor.z(),0.0f)));
	// 设置半径
	ptemplate.setRadius(0.05f); //0.05
	// 设置重量
	ptemplate.setMass(0.05f); //0.05

	// 创建粒子系统
	osg::ref_ptr<osgParticle::ParticleSystem> ps = new osgParticle::ParticleSystem();
	// 设置材质， 是否放射粒子，是否添加光照
	ps->setDefaultAttributes("res/smoke.rgb",false,false);

	// 加入模板
	ps->setDefaultParticleTemplate(ptemplate);

	// 创建粒子发射器(包括计数器，放置器和发射器）
	osg::ref_ptr<osgParticle::ModularEmitter> emitter = new osgParticle::ModularEmitter();
	// 关联粒子系统
	emitter->setParticleSystem(ps.get());

	// 创建发射器和计数器， 调整每一帧增加的粒子数目
	osg::ref_ptr<osgParticle::RandomRateCounter> counter = new osgParticle::RandomRateCounter();
	// 设置每秒添加的粒子的个数
	counter->setRateRange(100.00f*intensity,100.0f*intensity);
	// 关联计数器
	emitter->setCounter(counter.get());
	// 设置一个点放置器
	osg::ref_ptr<osgParticle::PointPlacer> placer = new osgParticle::PointPlacer();
	// 设置位置
	placer->setCenter(pos);
	// 关联点放置器
	emitter->setPlacer(placer.get());

	// box放置器
	//osg::ref_ptr<osgParticle::BoxPlacer> placer = new osgParticle::BoxPlacer();
	//placer->setXRange(-100,100);
	//placer->setYRange(-100,100);
	//placer->setZRange(-100,100);
	//emitter->setPlacer(placer.get());

	// 创建弧度发射器
	osg::ref_ptr<osgParticle::RadialShooter> shooter = new osgParticle::RadialShooter();
	// 设置发射速度变化范围
	shooter->setInitialSpeedRange(0,30*scale); // 100-0
	// 关联发射器
	emitter->setShooter(shooter.get());

	// 加入到场景中
	smokeNode->addChild(emitter.get());

	// 创建标准编程器对象，控制粒子在生命周期中的更新
	osg::ref_ptr<osgParticle::ModularProgram> program = new osgParticle::ModularProgram();
	// 关联粒子系统
	program->setParticleSystem(ps.get());

	// 创建重力模拟对象
	osg::ref_ptr<osgParticle::AccelOperator> ap = new osgParticle::AccelOperator();
	// 设置重力加速的，默认为9.80665f
	ap->setToGravity(-1.0f/scale);
	// 关联重力
	program->addOperator(ap.get());

	// 创建空气阻力模拟
	osg::ref_ptr<osgParticle::FluidFrictionOperator> ffo = new osgParticle::FluidFrictionOperator();
	// 设置空气属性
	// FluidViscosity 为1.8e-5f,FluidDensity 为1.2929f
	ffo->setFluidToAir();
	// 关联空气阻力
	program->addOperator(ffo.get());

	// 添加到场景
	smokeNode->addChild(program.get());

	// 添加更新器，实现每帧粒子的管理
	osg::ref_ptr<osgParticle::ParticleSystemUpdater> psu = new osgParticle::ParticleSystemUpdater();
	// 关联粒子系统
	psu->addParticleSystem(ps.get());

	// 加入场景
	osg::ref_ptr<osg::Geode> geode = new osg::Geode();
	geode->addDrawable(ps.get());

	smokeNode->addChild(geode.get());
	smokeNode->addChild(psu.get());
	return smokeNode.get();
}