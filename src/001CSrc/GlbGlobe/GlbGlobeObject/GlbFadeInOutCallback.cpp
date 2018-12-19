#include "StdAfx.h"
#include "GlbFadeInOutCallback.h"
#include "CGlbGlobe.h"
#include "GlbGlobeCallBack.h"
#include <osg/Material>
#include <osg/MatrixTransform>
#include "GlbOpacityVisitor.h"

using namespace GlbGlobe;

CGlbFadeInOutCallback::CGlbFadeInOutCallback( glbFloat startAlpha,glbFloat endAlpha,
	CGlbGlobeRObject *rObject,glbDouble durationTime,osg::Vec4 fadeColor )
{
	mpr_startAlpha = startAlpha;
	mpr_endAlpha = endAlpha;
	mpr_simulationTime = 0.0;
	mpr_isFadeIn = true;
	mpr_currentAlpha = startAlpha;
	mpr_rObject = rObject;
	mpr_durationTime = durationTime;
	mpr_fadeColor = fadeColor;
	if(endAlpha < startAlpha)
	{
		mpr_isFadeIn = false;
		mpr_currentAlpha = startAlpha;
	}
	//mpr_isThreeDimensional = isThreeDimensionalObject();
}


CGlbFadeInOutCallback::~CGlbFadeInOutCallback(void)
{
}

void CGlbFadeInOutCallback::changeMaterialAlpha( osg::StateSet* ss )
{
	if (ss==NULL) return;
	osg::ref_ptr<osg::Material> material = dynamic_cast<osg::Material *>(ss->getAttribute(osg::StateAttribute::MATERIAL));
	if(material == NULL)
	{
		material = new osg::Material;
		ss->setAttribute(material,osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
	}
	//之所以这样是因为像素线的时候渐进会出现变黑的效果，使用了自发光之后效果就正常了
	//但是使用了自发光又会使变身法线参与光照计算的三维物体没有立体效果，故做区分
	if(mpr_isThreeDimensional == false)
		material->setEmission(osg::Material::FRONT_AND_BACK,mpr_fadeColor);
	material->setDiffuse(osg::Material::FRONT_AND_BACK,mpr_fadeColor);
	material->setAlpha(osg::Material::FRONT_AND_BACK,mpr_currentAlpha / 100.0);
	ss->setMode(GL_LIGHTING,osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
	if(mpr_currentAlpha < 100)
	{
		if(osg::StateSet::TRANSPARENT_BIN != ss->getRenderingHint())
		{
			ss->setMode(GL_BLEND,osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
			ss->setRenderingHint(osg::StateSet::TRANSPARENT_BIN | osg::StateAttribute::OVERRIDE);
		}
	}
}

void GlbGlobe::CGlbFadeInOutCallback::operator()( osg::Node *node,osg::NodeVisitor *nv )
{
	glbref_ptr<GlbRenderInfo> renderInfo = mpr_rObject->GetRenderInfo();
	if(renderInfo.valid())
	{
		glbFloat alpha = 0.0;
		glbDouble simulationTime = nv->getFrameStamp()->getSimulationTime() - mpr_simulationTime;
		if(mpr_currentAlpha <= 100)
		{
			if(mpr_simulationTime != 0.0)
				alpha = ::fabs(mpr_endAlpha - mpr_startAlpha) * simulationTime / mpr_durationTime;

			if(mpr_isFadeIn)
			{
				mpr_currentAlpha += alpha;
				if(mpr_currentAlpha > mpr_endAlpha)
					mpr_currentAlpha = mpr_endAlpha;
			}
			else
			{
				mpr_currentAlpha -= alpha;
				if(mpr_currentAlpha < mpr_endAlpha)
					mpr_currentAlpha = mpr_endAlpha;
			}

			mpr_simulationTime = nv->getFrameStamp()->getSimulationTime();
			//changeMaterialAlpha(node->getOrCreateStateSet());
			CGlbOpacityVisitor nodeVisiter(mpr_currentAlpha);
			node->accept(nodeVisiter);
		}
	}

	if(mpr_currentAlpha >= mpr_endAlpha)
	{
		//node->setStateSet(NULL);
		glbref_ptr<CGlbGlobeCallBack::CGlbGlobeFadeCallBackTask> task = new CGlbGlobeCallBack::CGlbGlobeFadeCallBackTask(node);
		mpr_rObject->GetGlobe()->mpr_p_callback->AddFadeTask(task.get());
	}
	traverse(node,nv);
}

glbBool GlbGlobe::CGlbFadeInOutCallback::isThreeDimensionalObject()
{
	if(mpr_rObject == NULL)
		return false;
	GlbRenderInfo *renderInfo = mpr_rObject->GetRenderInfo();
	if(renderInfo->type == GLB_OBJECTTYPE_POINT)
	{
		GlbMarkerSymbolInfo *markerInfo = dynamic_cast<GlbMarkerSymbolInfo *>(renderInfo);
		if(markerInfo->symType == GLB_MARKERSYMBOL_3DSHAPE ||
			markerInfo->symType == GLB_MARKERSYMBOL_MODEL)
			return true;
	}
	else if(renderInfo->type == GLB_OBJECTTYPE_TIN)
		return true;

	return false;
}
