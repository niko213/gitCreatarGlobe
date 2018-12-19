#include "StdAfx.h"
#include "GlbGlobeFeatureLayer.h"
#include "GlbGlobePoint.h"
#include "osg/MatrixTransform"
#include "osg/Switch"
#include "GlbGlobeMarkerModelSymbol.h"
#include "GlbGlobeMarker2DShapeSymbol.h"
#include "GlbGlobeMarker3DShapeSymbol.h"
#include "GlbGlobeMarkerPixelSymbol.h"
#include "GlbGlobeMarkerLabelSymbol.h"
#include "GlbGlobeMarkerDynamicLabelSymbol.h"
#include "GlbGlobeMarkerImageSymbol.h"
#include "GlbGlobeMarkerFireSymbol.h"
#include "GlbGlobeMarkerSmokeSymbol.h"
#include "GlbGlobeMarkerEpicentreSymbol.h"
#include "osg/BlendColor"
#include "osg/BlendFunc"
#include "osg/Material"
#include "osg/LineStipple"
#include "osg/LineWidth"
#include "osg/Point"
#include "osgDB/ReadFile"
#include "osg/Texture2D"
#include "osg/TexMat"
#include "osg/Depth"
#include "osg/Billboard"
#include <osg/ImageStream>
#include "GlbString.h"
#include "GlbCalculateBoundBoxVisitor.h"
#include "GlbCalculateNodeSizeVisitor.h"
#include "GlbPoint.h"
#include "GlbLine.h"
#include "GlbGlobeCallBack.h"
#include "GlbGlobeMemCtrl.h"
#include <fstream>

#include "osg/TexEnv"
#include "osgGA/GUIEventHandler"
#include "osgViewer/Viewer"
#include "GlbGlobeView.h"

#include "osg/LineSegment"
#include "GlbGlobeMath.h"
#include "osg/PolygonMode"

#include "GlbGlobeSymbolCommon.h"
#include "GlbCompareValue.h"
#include "GlbOpacityVisitor.h"
#include <osg/GraphicsCostEstimator>
#include "GlbNodeIsCompiledVisitor.h"
#include "GlbFadeInOutCallback.h"
#include <math.h>
#include <osg/ImageStream>

using namespace GlbGlobe;

class PointObjectCallback : public osg::NodeCallback
{
public:
	PointObjectCallback(CGlbGlobeRObject *obj)
	{
		mpr_isChangeFrame = false;
		mpr_markNum = 0;
		mpr_obj = obj;
	}

	~PointObjectCallback()
	{
		if(mpr_obj && mpr_obj->GetOsgNode())
		{
			//osg::StateSet *stateset = mpr_obj->GetOsgNode()->getOrCreateStateSet();
			//osg::Material *material = 
			//	dynamic_cast<osg::Material *>(stateset->getAttribute(osg::StateAttribute::MATERIAL));
			//if(material)
			//	stateset->removeAttribute(material);
			mpr_obj->GetOsgNode()->setStateSet(NULL);
		}
		mpr_obj = NULL;
	}

	void operator()(osg::Node *node,osg::NodeVisitor *nv)
	{
		if(mpr_obj)
		{
			GlbRenderInfo *renderInfo = mpr_obj->GetRenderInfo();
			if(renderInfo)
			{
				GlbMarkerSymbolInfo *markerInfo = (GlbMarkerSymbolInfo *)renderInfo;
				if(markerInfo)
				{
					unsigned int frameNum = 60;
					unsigned int count = nv->getFrameStamp()->getFrameNumber() - mpr_markNum;

					if (count > frameNum - 1)
					{
						mpr_markNum = nv->getFrameStamp()->getFrameNumber();
						mpr_isChangeFrame = !mpr_isChangeFrame;
						count = 0;
					} 

					if(!mpr_isChangeFrame)
						count = frameNum - count;

					float a = count * 1 / (float)frameNum;
					if(a < 0.2) a = 0.2;

					if(markerInfo->symType == GLB_MARKERSYMBOL_2DSHAPE
						|| markerInfo->symType == GLB_MARKERSYMBOL_3DSHAPE)
					{
						osg::MatrixTransform *mt = 
							dynamic_cast<osg::MatrixTransform *>(node->asSwitch()->getChild(0));
						if(mt)
						{
							if(markerInfo->symType == GLB_MARKERSYMBOL_2DSHAPE)
								Deal2DShapeBlink(mt,a);
							else if(markerInfo->symType == GLB_MARKERSYMBOL_3DSHAPE)
								Deal3DShapeBlink(mt,a);
						}
					}
					else
					{
						DealOtherPointObjectBlink(node,a);
					}
				}
			}
		}
		traverse(node,nv);
	}
private:
	void Deal2DShapeBlink(osg::Node *node,float a)
	{
		//osg::ref_ptr<osg::StateSet> ss = node->getOrCreateStateSet();
		//osg::ref_ptr<osg::Material> material = dynamic_cast<osg::Material*>(ss->getAttribute(osg::StateAttribute::MATERIAL));
		//if(!material.valid())
		//{
		//	material = new osg::Material;
		//	ss->setAttribute(material.get(),osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
		//}
		//ss->setMode(GL_BLEND,osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
		//ss->setRenderingHint(osg::StateSet::TRANSPARENT_BIN | osg::StateAttribute::OVERRIDE);
		//material->setAlpha(osg::Material::FRONT_AND_BACK,a);

		osg::Group *group = dynamic_cast<osg::Group *>(node->asGroup()->getChild(0));
		if(NULL == group) return;
		osg::Geode *geode = dynamic_cast<Geode *>(group->getChild(0));
		if(geode == NULL) return;
		osg::MatrixTransform *outlineMt = dynamic_cast<osg::MatrixTransform *>(group->getChild(1));
		if(NULL == outlineMt) return;
		osg::Node *outlineNode = outlineMt->getChild(0);
		if(NULL == outlineNode) return;
		osg::ref_ptr<osg::Drawable> fillDrawable = geode->getDrawable(0);
		osg::ref_ptr<osg::StateSet> fillStateSet = fillDrawable->getOrCreateStateSet();
		fillStateSet->setMode(GL_BLEND,osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
		fillStateSet->setRenderingHint(osg::StateSet::TRANSPARENT_BIN | osg::StateAttribute::OVERRIDE);
		osg::ref_ptr<osg::StateSet> outStateSet = outlineNode->getOrCreateStateSet();
		outStateSet->setMode(GL_BLEND,osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
		outStateSet->setRenderingHint(osg::StateSet::TRANSPARENT_BIN | osg::StateAttribute::OVERRIDE);
		osg::Material *fillMaterial = 
			dynamic_cast<osg::Material *>(fillStateSet->getAttribute(osg::StateAttribute::MATERIAL));
		osg::Material *outMaterial = 
			dynamic_cast<osg::Material *>(outStateSet->getAttribute(osg::StateAttribute::MATERIAL));
		
		if (fillMaterial)
			fillMaterial->setAlpha(osg::Material::FRONT_AND_BACK,a);
		if(outMaterial)
		{
			const osg::Vec4 diffuse = outMaterial->getDiffuse(osg::Material::FRONT_AND_BACK);
			const osg::Vec4 emission = outMaterial->getEmission(osg::Material::FRONT_AND_BACK);
			if (emission.a() > 0.1 || diffuse.a() > 0.1)
				outMaterial->setAlpha(osg::Material::FRONT_AND_BACK,a);
		}
	}
	void Deal3DShapeBlink(osg::Node *node,float a)
	{
		//osg::ref_ptr<osg::StateSet> ss = node->getOrCreateStateSet();
		//osg::ref_ptr<osg::Material> material = dynamic_cast<osg::Material*>(ss->getAttribute(osg::StateAttribute::MATERIAL));
		//if(!material.valid())
		//{
		//	material = new osg::Material;
		//	ss->setAttribute(material.get(),osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
		//}
		//ss->setMode(GL_BLEND,osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
		//ss->setRenderingHint(osg::StateSet::TRANSPARENT_BIN | osg::StateAttribute::OVERRIDE);
		//material->setAlpha(osg::Material::FRONT_AND_BACK,a);

		osg::Geode *geode = dynamic_cast<Geode *>(node->asGroup()->getChild(0));
		if(geode == NULL) return;
		osg::ref_ptr<osg::Drawable> fillDrawable = geode->getDrawable(0);
		osg::ref_ptr<osg::Drawable> outDrawable = geode->getDrawable(1);
		osg::ref_ptr<osg::StateSet> fillStateSet = fillDrawable->getOrCreateStateSet();
		fillStateSet->setMode(GL_BLEND,osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
		fillStateSet->setRenderingHint(osg::StateSet::TRANSPARENT_BIN | osg::StateAttribute::OVERRIDE);
		osg::ref_ptr<osg::StateSet> outStateSet = outDrawable->getOrCreateStateSet();
		outStateSet->setMode(GL_BLEND,osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
		outStateSet->setRenderingHint(osg::StateSet::TRANSPARENT_BIN | osg::StateAttribute::OVERRIDE);
		osg::Material *fillMaterial = 
			dynamic_cast<osg::Material *>(fillStateSet->getAttribute(osg::StateAttribute::MATERIAL));
		osg::Material *outMaterial = 
			dynamic_cast<osg::Material *>(outStateSet->getAttribute(osg::StateAttribute::MATERIAL));

		if (fillMaterial)
			fillMaterial->setAlpha(osg::Material::FRONT_AND_BACK,a);
		if(outMaterial)
		{
			const osg::Vec4 diffuse = outMaterial->getDiffuse(osg::Material::FRONT_AND_BACK);
			const osg::Vec4 emission = outMaterial->getEmission(osg::Material::FRONT_AND_BACK);
			if (emission.a() > 0.1 || diffuse.a() > 0.1)
				outMaterial->setAlpha(osg::Material::FRONT_AND_BACK,a);
		}
	}
	void DealOtherPointObjectBlink(osg::Node *node,float a)
	{
		osg::StateSet *stateset = node->getOrCreateStateSet();
		stateset->setMode(GL_BLEND,osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
		stateset->setRenderingHint(osg::StateSet::TRANSPARENT_BIN | osg::StateAttribute::OVERRIDE);
		osg::Material *material = 
			dynamic_cast<osg::Material *>(stateset->getAttribute(osg::StateAttribute::MATERIAL));
		if(material == NULL)
		{
			material = new osg::Material;
			stateset->setAttribute(material,osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
		}
		material->setDiffuse(osg::Material::FRONT_AND_BACK,osg::Vec4f(0,0,0,a));
		material->setEmission(osg::Material::FRONT_AND_BACK,osg::Vec4f(1,0,0,a));
		stateset->setMode(GL_LIGHTING,osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
	}
private:
	glbBool mpr_isChangeFrame;
	unsigned int mpr_markNum;
	glbref_ptr<CGlbGlobeRObject> mpr_obj;
};

//动态文字的显示
class DynamicLabelFadeCallback : public osg::NodeCallback
{
public:
	DynamicLabelFadeCallback(osg::Image *texImage,unsigned char *aggBuffer,osg::Vec2i bufferSize,CGlbGlobePoint *obj)
	{
		mpr_pointObj = obj;
		mpr_textIndex = 0.0;
		mpr_aggBuffer = aggBuffer;
		mpr_bufferSize = bufferSize;
		mpr_texImage = texImage;
		parseFadeSpeed();
	}

	~DynamicLabelFadeCallback()
	{
		if(mpr_aggBuffer)
		{
			delete mpr_aggBuffer;
			mpr_aggBuffer = NULL;
		}
	}

	void operator()(osg::Node *node,osg::NodeVisitor *nv)
	{
		if(mpr_pointObj->IsFadingDynamicLabel())
		{
			GlbMarkerDynamicLabelSymbolInfo *markerLabelInfo = 
				(GlbMarkerDynamicLabelSymbolInfo *)mpr_pointObj->GetRenderInfo();
			mpr_textIndex++;

			int bits = 0;
			unsigned char * buffer  = NULL;
			bits = ((mpr_bufferSize.x() * 32) + 31) / 32 * 4;
			bits *= mpr_bufferSize.y();
			buffer = new unsigned char[bits];
			memcpy(buffer,mpr_aggBuffer,sizeof(unsigned char) * mpr_bufferSize.x()* mpr_bufferSize.y() * 4);

			DealOpacityByPercent(buffer,mpr_bufferSize.x(),mpr_bufferSize.y(),1.0 * mpr_textIndex / mpr_fadeSpeedTime);

			ref_ptr<osg::Image> image = new osg::Image;
			if(mpr_texImage.valid())
			{// mpr_texImage地图buffer与文字buffer混合--替换
				unsigned char *pdata = mpr_texImage->data();
				int width = mpr_texImage->s();
				int height = mpr_texImage->t();
				for(int i = 0; i < height ;i++)
					for(int j =0; j < width; j++)
					{
						unsigned char* r = buffer + ( i * width + j) * 4;
						unsigned char* g = buffer + ( i * width + j) * 4 + 1;
						unsigned char* b = buffer + ( i * width + j) * 4 + 2;
						unsigned char* a = buffer + ( i * width + j) * 4 + 3;

						// 把文字接近透明区域<200过滤掉
						if(*a < 200)
							continue;
						// 把文字不透明区域>200（只有文字）替换掉底图上颜色，压花效果！
						pdata[(i * width + j) * 4] = *r;
						pdata[(i * width + j) * 4 + 1] = *g;
						pdata[(i * width + j) * 4 + 2] = *b;
						//pdata[(i * width + j) * 4 + 3] = *a;
					}
			}
			else
			{
				image->allocateImage(mpr_bufferSize.x(),mpr_bufferSize.y(),1,GL_RGBA, GL_UNSIGNED_BYTE);
				unsigned char *data = image->data();
				memcpy(data,buffer,sizeof(unsigned char) * mpr_bufferSize.x()* mpr_bufferSize.y() * 4);
			}

			osg::Switch *swith = dynamic_cast<osg::Switch *>(node);
			osg::Node *geode = swith->getChild(0)->asGroup()->getChild(0)->asGroup()->getChild(0)->asGroup()->getChild(0);
			osg::Geometry *geometry = dynamic_cast<osg::Geometry *>(geode->asGeode()->getDrawable(0));
			osg::StateSet *ss = geometry->getOrCreateStateSet();
			osg::ref_ptr<osg::Texture2D> texture = dynamic_cast<osg::Texture2D *>(ss->getAttribute(osg::StateAttribute::TEXTURE));
			if (!texture.valid())
				texture = new osg::Texture2D;

			if (mpr_texImage.valid())
			{
				texture->setImage(mpr_texImage);
				ss->setTextureAttributeAndModes(0,texture,osg::StateAttribute::ON);
			}
			else
			{
				texture->setImage(image);
				ss->setTextureAttributeAndModes(0,texture,osg::StateAttribute::ON);
			}

			if(mpr_textIndex > mpr_fadeSpeedTime)
			{
				mpr_pointObj->SetFadingDynamicLabel(false);
				glbref_ptr<CGlbGlobeCallBack::CGlbGlobeFadeCallBackTask> task = new CGlbGlobeCallBack::CGlbGlobeFadeCallBackTask(node,this);
				mpr_pointObj->GetGlobe()->mpr_p_callback->AddFadeTask(task.get());
			}

			if(buffer)
			{
				delete buffer;
				buffer = NULL;
			}
		}
		else
		{
			glbref_ptr<CGlbGlobeCallBack::CGlbGlobeFadeCallBackTask> task = new CGlbGlobeCallBack::CGlbGlobeFadeCallBackTask(node,this);
			mpr_pointObj->GetGlobe()->mpr_p_callback->AddFadeTask(task.get());
		}
		traverse(node,nv);
	}
private:
	void parseFadeSpeed()
	{
		GlbMarkerDynamicLabelSymbolInfo *markerLabelInfo = 
			(GlbMarkerDynamicLabelSymbolInfo *)mpr_pointObj->GetRenderInfo();
		GlbGlobeDynamicLabelFadeSpeedEnum fadeSpeedEnum = (GlbGlobeDynamicLabelFadeSpeedEnum)(markerLabelInfo->fadeSpeed->GetValue(mpr_pointObj->GetFeature()));
		if(fadeSpeedEnum == GLB_FADETEXT_FAST)
			mpr_fadeSpeedTime = 30;
		else if(fadeSpeedEnum == GLB_FADETEXT_MEDIUM)
			mpr_fadeSpeedTime = 60;
		else
			mpr_fadeSpeedTime = 90;

		mpr_fadeMode = (GlbGlobeDynamicLabelFadeTextEnum)(markerLabelInfo->fadeMode->GetValue(mpr_pointObj->GetFeature()));
	}
	void DealOpacityByPercent( unsigned char * buffer,glbInt32 sizeWidth,glbInt32 sizeHeight,double percent )
	{
		if(mpr_fadeMode == GLB_FADETEXT_TOPTOBOTTOM)
		{
			double height = sizeHeight - sizeHeight * percent;
			for(int i = 0; i < height ;i++)
				for(int j = 0 ; j < sizeWidth ; j++)
				{
					buffer[(i * sizeWidth + j) * 4 + 3] = 0;
				}
		}
		else
		{
			double width = sizeWidth * percent;
			for(int i = 0; i < sizeHeight ;i++)
			{
				for(int j = width ; j < sizeWidth ; j++)
				{
					buffer[(i * sizeWidth + j) * 4 + 3] = 0;
				}
			}
		}
	}
private:
	glbref_ptr<CGlbGlobePoint> mpr_pointObj;
	glbFloat                                             mpr_textIndex;
	glbInt32                                             mpr_fadeSpeedTime;
	unsigned char *mpr_aggBuffer;
	osg::ref_ptr<osg::Image> mpr_texImage;
	osg::Vec2i                             mpr_bufferSize;
	GlbGlobeDynamicLabelFadeTextEnum mpr_fadeMode;
};

//编辑
class PointEditEventHandler : public osgGA::GUIEventHandler
{
public:
	PointEditEventHandler(CGlbGlobePoint *obj):mpr_pointObj(obj)
	{
		mpr_globe = mpr_pointObj->GetGlobe();
		mpr_boundGeode = NULL;
		mpr_boundExtent = NULL;
	}
	bool handle(const osgGA::GUIEventAdapter &ea,osgGA::GUIActionAdapter &aa)
	{
		osgViewer::Viewer *viewer = dynamic_cast<osgViewer::Viewer*>(&aa);
		if(!viewer) return false;
		//unsigned int buttonMask = ea.getButtonMask();
		static glbBool isIntersect = false;
		bool isGlobe = mpr_globe->GetType() == GLB_GLOBETYPE_GLOBE;
		//callback代码____begin
		if(mpr_boundGeode == NULL)
		{
			mpr_boundExtent = mpr_pointObj->GetBound(true);
			osg::Vec3d center(0.0,0.0,0.0);
			mpr_boundExtent->GetCenter(&center.x(),&center.y(),&center.z());
			osg::Matrixd localToWorld;
			if(isGlobe)
				g_ellipsoidModel->computeLocalToWorldTransformFromXYZ(center.x(),center.y(),center.z(),localToWorld);
			else
				localToWorld.makeTranslate(center.x(),center.y(),center.z());
			mpr_boundGeode = CreateBoundingBox(*mpr_boundExtent,true);
			osg::ref_ptr<osg::MatrixTransform> mt = new osg::MatrixTransform;
			mt->setMatrix(localToWorld);
			mt->addChild(mpr_boundGeode);
			mpr_pointObj->GetEditNode()->addChild(mt);
		}

		glbref_ptr<CGlbExtent> geoExt = mpr_pointObj->GetBound(true);
		if(*geoExt != *mpr_boundExtent)
		{
			mpr_boundExtent = geoExt;
			osg::Matrixd localToWorld;
			osg::Vec3d center(0.0,0.0,0.0);
			geoExt->GetCenter(&center.x(),&center.y(),&center.z());
			if(isGlobe)
				g_ellipsoidModel->computeLocalToWorldTransformFromXYZ(center.x(),center.y(),center.z(),localToWorld);
			else
				localToWorld.makeTranslate(center.x(),center.y(),center.z());
			osg::ref_ptr<osg::MatrixTransform> mt = dynamic_cast<osg::MatrixTransform *>(mpr_boundGeode->getParent(0));
			if (mt != NULL)
				mt->setMatrix(localToWorld);
		}
		//callback代码____end

		switch(ea.getEventType())
		{
		case (osgGA::GUIEventAdapter::PUSH):
			{
				//osg::Node *node = mpr_pointObj->GetOsgNode();
				//if(!node)
				//	return false;
				CGlbGlobeView *view =  mpr_globe->GetView();
				osg::Vec3d start(ea.getX(),ea.getY(),0.0);
				osg::Vec3d end(ea.getX(),ea.getY(),1.0);
				view->ScreenToWorld(start.x(),start.y(),start.z());
				view->ScreenToWorld(end.x(),end.y(),end.z());
				glbref_ptr<CGlbExtent> boundExtent = mpr_pointObj->GetBound(true);
				glbDouble minX,minY,minZ,maxX,maxY,maxZ;
				boundExtent->GetMin(&minX,&minY,&minZ);
				boundExtent->GetMax(&maxX,&maxY,&maxZ);
				osg::BoundingBox bb(minX,minY,minZ,maxX,maxY,maxZ);
				isIntersect = intersectRayBoundingBox(start,end,bb);
				if(isIntersect == false)
					return false;
				return true;
			}
		case(osgGA::GUIEventAdapter::RELEASE):
			{
				if(isIntersect)
					isIntersect = false;
				return true;
			}
		case(osgGA::GUIEventAdapter::DRAG):
			{
				if(isIntersect && mpr_pointObj->GetEditMode() == GLB_ROBJECTEDIT_HORIZONTAL)
				{// 水平拖拽
					CGlbGlobeView *view =  mpr_globe->GetView();
					osg::Vec3d start(ea.getX(),ea.getY(),0.0);
					osg::Vec3d end(ea.getX(),ea.getY(),1.0);
					view->ScreenToWorld(start.x(),start.y(),start.z());
					view->ScreenToWorld(end.x(),end.y(),end.z());

					glbref_ptr<CGlbExtent> extent = mpr_pointObj->GetBound(true);
					osg::Vec3d center;
					extent->GetCenter(&center.x(),&center.y(),&center.z());
					glbDouble radius = (center - osg::Vec3d(0.0,0.0,0.0)).length();

					std::vector<osg::Vec3d> IntersectPos;
					glbInt32 pointNum = 0;
					if(isGlobe)// 球面模式与 中心点为（0,0,0）半径为radius的球求交
						pointNum = IntersectRaySphere(start,end,osg::Vec3d(0.0,0.0,0.0),radius,IntersectPos);
					else
					{// 平面模式与平行于xy平面的平面（0,0,center.z）求交
						osg::Vec3d IntersectPoint;
						//射线与平面地形相交点
						//glbBool isChoose = mpr_globe->ScreenToTerrainCoordinate(ea.getX(),ea.getY(),IntersectPoint.x(),IntersectPoint.y(),IntersectPoint.z());
						//if(isChoose)
						//	IntersectPos.push_back(IntersectPoint);
						osg::ref_ptr<osg::LineSegment> line = new osg::LineSegment(start,end);
						osg::Vec3d dir(0,0,1);
						osg::Plane *plane = new osg::Plane(dir,center);
						bool Intersected = intersect3D_SegmentPlane(line,plane,center,IntersectPoint);
						if(Intersected)
							IntersectPos.push_back(IntersectPoint);
					}

					if(IntersectPos.size() > 0)
					{
						osg::Vec3d oldPosition;
						osg::Vec3d newPosition;
						mpr_pointObj->GetPosition(&oldPosition.x(),&oldPosition.y(),&oldPosition.z());
						glbBool isNULL = false;
						if(isGlobe)
						{
							g_ellipsoidModel->convertXYZToLatLongHeight(
								IntersectPos.at(0).x(),IntersectPos.at(0).y(),IntersectPos.at(0).z(),
								newPosition.y(),newPosition.x(),newPosition.z());
							mpr_pointObj->SetPosition(osg::RadiansToDegrees(newPosition.x()),
								osg::RadiansToDegrees(newPosition.y()),oldPosition.z(),isNULL);
						}
						else
						{
							mpr_pointObj->SetPosition(IntersectPos.at(0).x(),IntersectPos.at(0).y(),oldPosition.z(),isNULL);
						}
						if (isNULL)
							mpr_pointObj->DirectDraw(0);
						return true;
					}
					return false;
				}
				else if(isIntersect && mpr_pointObj->GetEditMode() == GLB_ROBJECTEDIT_VERTICAL)
				{// 垂直拖拽
					osg::Vec3d position;
					mpr_pointObj->GetPosition(&position.x(),&position.y(),&position.z());

					osg::Vec3d focusPoint;
					osg::Vec3d cameraPoint;
					mpr_globe->GetView()->GetFocusPos(focusPoint);
					if(isGlobe)
						g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(focusPoint.y()),
						osg::DegreesToRadians(focusPoint.x()),focusPoint.z(),focusPoint.x(),focusPoint.y(),focusPoint.z());
					mpr_globe->GetView()->GetCameraPos(cameraPoint);
					if(isGlobe)
						g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(cameraPoint.y()),
						osg::DegreesToRadians(cameraPoint.x()),cameraPoint.z(),cameraPoint.x(),cameraPoint.y(),cameraPoint.z());
					osg::Vec3d dir = focusPoint - cameraPoint;
					dir.normalize();
					osg::Vec3d objPoint;
					if(isGlobe)
						g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(position.y()),
						osg::DegreesToRadians(position.x()),position.z(),objPoint.x(),objPoint.y(),objPoint.z());
					else
						objPoint = position;

					osg::Plane *plane = new osg::Plane(dir,objPoint);
					osg::Vec3d outPoint;

					CGlbGlobeView *view =  mpr_globe->GetView();
					osg::Vec3d start(ea.getX(),ea.getY(),0.0);
					osg::Vec3d end(ea.getX(),ea.getY(),1.0);
					view->ScreenToWorld(start.x(),start.y(),start.z());
					view->ScreenToWorld(end.x(),end.y(),end.z());

					osg::ref_ptr<osg::LineSegment> line = new osg::LineSegment(start,end);
					bool intersected = intersect3D_SegmentPlane(line,plane,objPoint,outPoint);
					if(intersected == false)
						return false;
					osg::Vec3d newPosition;
					if(isGlobe)
						g_ellipsoidModel->convertXYZToLatLongHeight(outPoint.x(),outPoint.y(),outPoint.z(),
						newPosition.y(),newPosition.x(),newPosition.z());
					else
						newPosition.z() = outPoint.z();
					glbBool isNULL = false;
					mpr_pointObj->SetPosition(position.x(),position.y(),newPosition.z(),isNULL);
					if (isNULL)
						mpr_pointObj->DirectDraw(0);
					return true;
				}
				return false;
			}
			default:
				return false;
		}
		return false;//return false继续调用其他事件，return true不会再调用其他事件
	}
private:
	glbref_ptr<CGlbGlobe> mpr_globe;
	glbref_ptr<CGlbGlobePoint> mpr_pointObj;
	osg::ref_ptr<osg::Geode> mpr_boundGeode;
	glbref_ptr<CGlbExtent> mpr_boundExtent;
};

CGlbGlobePoint::CGlbGlobePoint(void)
{
	mpr_readData_critical.SetName(L"point_readdata");
	mpr_addToScene_critical.SetName(L"point_addscene");
	mpr_outline_critical.SetName(L"point_outline");
	mpr_distance          = 0.0;
	mpr_outline           = NULL;
	mpr_renderInfo        = NULL;
	mpr_xOrLon            = 0.0;
	mpr_yOrLat            = 0.0;
	mpr_zOrAlt            = 0.0;
	mpr_elevation         = DBL_MAX;// 设置无效值
	mpr_isCanDrawImage    = true;	
	mpr_objSize           = 0;
	mpr_needReReadData    = false;
	mpr_noRotOutline = NULL;
	mpr_isDirectDraw = false;
	mpr_zerolevelObjNodeSize = -1;
	//SetIsNeedToPreCompile(true);	
	mpr_preNodeSize		  = -1;

	mpt_isFaded = true;			//启用淡入显示效果
	mpt_isLODEnable = false;	//关闭lod模式
	mpr_isFadingDynamicLabel = false;
	
	mpr_preDistance = 0.0;
}

CGlbGlobePoint::~CGlbGlobePoint(void)
{
	mpr_renderInfo = NULL;
	mpr_outline    = NULL;
	mpr_noRotOutline = NULL;
}

GlbGlobeObjectTypeEnum CGlbGlobePoint::GetType()
{
	return GLB_OBJECTTYPE_POINT;
}

glbBool CGlbGlobePoint::Load( xmlNodePtr* node,const glbWChar* prjPath )
{
	CGlbGlobeRObject::Load(node,prjPath);

	xmlChar* szKey = NULL;
	xmlNodePtr pnode = *node;
	if (pnode && (!xmlStrcmp(pnode->name, (const xmlChar *)"Lon")))
	{
		szKey = xmlNodeGetContent(pnode);
		sscanf((char*)szKey,"%lf",&mpr_xOrLon);	
		xmlFree(szKey);

		pnode = pnode->next;
	}
	if (pnode && (!xmlStrcmp(pnode->name, (const xmlChar *)"Lat")))
	{
		szKey = xmlNodeGetContent(pnode);
		sscanf((char*)szKey,"%lf",&mpr_yOrLat);	
		xmlFree(szKey);

		pnode = pnode->next;
	}
	if (pnode && (!xmlStrcmp(pnode->name, (const xmlChar *)"Alt")))
	{
		szKey = xmlNodeGetContent(pnode);
		sscanf((char*)szKey,"%lf",&mpr_zOrAlt);	
		xmlFree(szKey);

		pnode = pnode->next;
	}
	if (pnode && (!xmlStrcmp(pnode->name, (const xmlChar *)"RenderInfo")))
	{
		GlbGlobeObjectTypeEnum rdtype;
		GlbMarkerSymbolTypeEnum symtype;
		xmlNodePtr rdchild = pnode->xmlChildrenNode;

		if (rdchild && (!xmlStrcmp(rdchild->name, (const xmlChar *)"RenderType")))
		{
			szKey = xmlNodeGetContent(rdchild);
			sscanf((char*)szKey,"%d",&rdtype);	
			xmlFree(szKey);

			rdchild = rdchild->next;
		}
		if (rdchild && (!xmlStrcmp(rdchild->name, (const xmlChar *)"SymbolType")))
		{
			szKey = xmlNodeGetContent(rdchild);
			sscanf((char*)szKey,"%d",&symtype);	
			xmlFree(szKey);

			rdchild = rdchild->next;
			switch(symtype)
			{
			case GLB_MARKERSYMBOL_MODEL:
				{
					GlbMarkerModelSymbolInfo* modeinfo = new GlbMarkerModelSymbolInfo();
					modeinfo->Load(rdchild,prjPath);
					mpr_renderInfo = (GlbRenderInfo*)modeinfo;
				}break;
			case GLB_MARKERSYMBOL_2DSHAPE:
				{
					GlbMarker2DShapeSymbolInfo* shape2dinfo = new GlbMarker2DShapeSymbolInfo();
					shape2dinfo->Load(rdchild,prjPath);
					mpr_renderInfo = (GlbRenderInfo*)shape2dinfo;
				}break;
			case GLB_MARKERSYMBOL_3DSHAPE:
				{
					GlbMarker3DShapeSymbolInfo* shape3dinfo = new GlbMarker3DShapeSymbolInfo();
					shape3dinfo->Load(rdchild,prjPath);
					mpr_renderInfo = (GlbRenderInfo*)shape3dinfo;
				}break;
			case GLB_MARKERSYMBOL_PIXEL:
				{
					GlbMarkerPixelSymbolInfo* pixelinfo = new GlbMarkerPixelSymbolInfo();
					pixelinfo->Load(rdchild,prjPath);
					mpr_renderInfo = (GlbRenderInfo*)pixelinfo;
				}break;
			case GLB_MARKERSYMBOL_LABEL:
				{
					GlbMarkerLabelSymbolInfo* labelinfo = new GlbMarkerLabelSymbolInfo();
					labelinfo->Load(rdchild,prjPath);
					mpr_renderInfo = (GlbRenderInfo*)labelinfo;
				}break;
			case GLB_MARKERSYMBOL_DYNAMICLABEL:
				{
					GlbMarkerDynamicLabelSymbolInfo* labelinfo = new GlbMarkerDynamicLabelSymbolInfo();
					labelinfo->Load(rdchild,prjPath);
					mpr_renderInfo = (GlbRenderInfo*)labelinfo;
				}break;
			case GLB_MARKERSYMBOL_IMAGE:
				{
					GlbMarkerImageSymbolInfo* imageinfo = new GlbMarkerImageSymbolInfo();
					imageinfo->Load(rdchild,prjPath);
					mpr_renderInfo = (GlbRenderInfo*)imageinfo;
				}break;
			case GLB_MARKERSYMBOL_FIRE:
				{
					GlbMarkerFireSymbolInfo* fireinfo = new GlbMarkerFireSymbolInfo();
					fireinfo->Load(rdchild,prjPath);
					mpr_renderInfo = (GlbRenderInfo*)fireinfo;
				}break;
			case GLB_MARKERSYMBOL_SMOKE:
				{
					GlbMarkerSmokeSymbolInfo* smokeinfo = new GlbMarkerSmokeSymbolInfo();
					smokeinfo->Load(rdchild,prjPath);
					mpr_renderInfo = (GlbRenderInfo*)smokeinfo;
				}break;
			case GLB_MARKERSYMBOL_EPICENTRE:
				{
					GlbMarkerEpicentreSymbolInfo* epicentreinfo = new GlbMarkerEpicentreSymbolInfo();
					epicentreinfo->Load(rdchild,prjPath);
					mpr_renderInfo = (GlbRenderInfo*)epicentreinfo;
				}break;
			}
		}
	}
	ParseObjectFadeColor();
	return true;
}

glbBool CGlbGlobePoint::Save( xmlNodePtr node,const glbWChar* prjPath )
{
	CGlbGlobeRObject::Save(node,prjPath);
	//位置
	char str[32];

	sprintf_s(str,"%8lf",mpr_xOrLon);
	xmlNewTextChild(node, NULL, BAD_CAST "Lon", BAD_CAST str);

	sprintf_s(str,"%8lf",mpr_yOrLat);
	xmlNewTextChild(node, NULL, BAD_CAST "Lat", BAD_CAST str);

	sprintf_s(str,"%8lf",mpr_zOrAlt);
	xmlNewTextChild(node, NULL, BAD_CAST "Alt", BAD_CAST str);

	xmlNodePtr rdnode = xmlNewNode(NULL,BAD_CAST "RenderInfo");
	xmlAddChild(node,rdnode);
	if(mpr_renderInfo)
	{
		sprintf_s(str,"%d",mpr_renderInfo->type);
		xmlNewTextChild(rdnode, NULL, BAD_CAST "RenderType", BAD_CAST str);

		GlbMarkerSymbolInfo* markerinfo = (GlbMarkerSymbolInfo*)mpr_renderInfo.get();
		sprintf_s(str,"%d",markerinfo->symType);
		xmlNewTextChild(rdnode, NULL, BAD_CAST "SymbolType", BAD_CAST str);
		switch(markerinfo->symType)
		{
		case GLB_MARKERSYMBOL_MODEL:
			{
				((GlbMarkerModelSymbolInfo*)mpr_renderInfo.get())->Save(rdnode,prjPath);				
			}break;
		case GLB_MARKERSYMBOL_2DSHAPE:
			{
				((GlbMarker2DShapeSymbolInfo*)mpr_renderInfo.get())->Save(rdnode,prjPath);				
			}break;
		case GLB_MARKERSYMBOL_3DSHAPE:
			{
				((GlbMarker3DShapeSymbolInfo*)mpr_renderInfo.get())->Save(rdnode,prjPath);				
			}break;
		case GLB_MARKERSYMBOL_PIXEL:
			{
				((GlbMarkerPixelSymbolInfo*)mpr_renderInfo.get())->Save(rdnode,prjPath);				
			}break;
		case GLB_MARKERSYMBOL_LABEL:
			{
				((GlbMarkerLabelSymbolInfo*)mpr_renderInfo.get())->Save(rdnode,prjPath);				
			}break;
		case GLB_MARKERSYMBOL_DYNAMICLABEL:
			{
				((GlbMarkerDynamicLabelSymbolInfo*)mpr_renderInfo.get())->Save(rdnode,prjPath);				
			}break;
		case GLB_MARKERSYMBOL_IMAGE:
			{
				((GlbMarkerImageSymbolInfo*)mpr_renderInfo.get())->Save(rdnode,prjPath);				
			}break;
		case GLB_MARKERSYMBOL_FIRE:
			{
				((GlbMarkerFireSymbolInfo*)mpr_renderInfo.get())->Save(rdnode,prjPath);	
			}break;
		case GLB_MARKERSYMBOL_SMOKE:
			{
				((GlbMarkerSmokeSymbolInfo*)mpr_renderInfo.get())->Save(rdnode,prjPath);	
			}break;
		case GLB_MARKERSYMBOL_EPICENTRE:
			{
				((GlbMarkerEpicentreSymbolInfo*)mpr_renderInfo.get())->Save(rdnode,prjPath);	
			}break;
		}
	}
	return true;
}

glbDouble CGlbGlobePoint::GetDistance( osg::Vec3d &cameraPos,glbBool isCompute )
{
	if(isCompute == false)
		return mpr_distance;

	if (!mpt_globe)	return DBL_MAX;

	glbBool isShow = mpt_isDispShow && mpt_isShow;
	if (mpt_isCameraAltitudeAsDistance)	
		mpr_distance = cameraPos.z();	
	else if (mpt_node.valid() && isShow)
	{// 如果节点已经生成，直接用节点中心点作为计算参考[注意如果mpt_node（switch）为switchoff状态时::getBound()获得的值不是有效值（中心（0,0,0）radius=-1）malin, 所以isShow=true时才能用这种方法计算 2016.5.4]
		osg::Vec3d position = mpt_node->getBound().center();
		osg::Vec3d cameraPoint = cameraPos;
		if(mpt_globe->GetType() == GLB_GLOBETYPE_GLOBE)	
			g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(cameraPos.y()),
			osg::DegreesToRadians(cameraPos.x()),cameraPos.z(),cameraPoint.x(),cameraPoint.y(),cameraPoint.z());	
		mpr_distance = (position - cameraPoint).length();
	}
	else
	{
		//mpr_xOrLon,mpr_yOrLat,mpr_zOrAlt 初始化时0.0
		osg::Vec3d position(mpr_xOrLon,mpr_yOrLat,mpr_zOrAlt);
		//if(mpr_geo && mpr_geo->GetExtent())
		//{
		//	mpr_geo->GetExtent()->GetCenter(&position.x(),&position.y(),&position.z());
		//}
		if(mpt_feature.get())
		{		
			glbref_ptr<CGlbExtent> ext = const_cast<CGlbExtent *>(mpt_feature->GetExtent());
			if(ext)
			{//有可能是NULL
				ext->GetCenter(&position.x(),&position.y(),&position.z());
			}
		}
		//{//因为对象在加入Globe时,首先会调用GetBound，GetBound中会-->UpdateElevate
			//if(mpt_altitudeMode != GLB_ALTITUDEMODE_ABSOLUTE)
			//	UpdateElevate();
		//}
		ComputePosByAltitudeAndGloleType(position);

		osg::Vec3d cameraPoint = cameraPos;
		if(mpt_globe->GetType() == GLB_GLOBETYPE_GLOBE)	
			g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(cameraPos.y()),
				osg::DegreesToRadians(cameraPos.x()),cameraPos.z(),cameraPoint.x(),cameraPoint.y(),cameraPoint.z());	
		mpr_distance = (position - cameraPoint).length();
	}

	return mpr_distance;
}
/*
*   mpt_node、mpt_preNode 生成、改为NULL 在不同线程
*   LoadData 只有 Dispatcher 线程调用
*   
*   修改renderinfo,position 会在其他线程
*/
void CGlbGlobePoint::LoadData( glbInt32 level )
{
	if(mpt_currLevel == level) return;
	if(mpt_preNode  !=  NULL)
	{//旧对象还未压入摘队列
		/*
		*   mpt_node = node1,preNode = NULL    -----> node1_h
		*   不做控制，且LoadData快
		*   {
		*       mpt_node = node2,preNode = node1
		*       mpt_node = node3,preNode = node2   -----> node1_h,node2_r,node2_h
		*       造成node1 永远不会摘除
		*   }
		*/
		return;
	}
	glbref_ptr<GlbMarkerSymbolInfo>markerSymbol = dynamic_cast<GlbMarkerSymbolInfo *>(mpr_renderInfo.get());
	if(markerSymbol == NULL)
		return;
	// 非3D模型类型，且已经load过，就不用再次load
	if (markerSymbol->symType != GLB_MARKERSYMBOL_MODEL && mpt_currLevel != -1) 	
		return;
	{//与DirectDraw互斥.
		GlbScopedLock<GlbCriticalSection> lock(mpr_readData_critical);
		if (mpt_loadState == true)return;
		mpt_loadState = true;
	}		
	//if(mpt_currLevel != level)
	//{
	//	if(mpt_currLevel < level)
	//		level = mpt_currLevel + 1;
	//}
	glbref_ptr<CGlbGlobePointTask> task = new CGlbGlobePointTask(this,level);
	mpt_globe->mpr_taskmanager->AddTask(task.get());
}
/*
*     Dispatcher->AddToScene()->CGlbGlobePoint::AddToScene()  //频繁调用
*     外部线程-->DirectDraw()->AddToScene() //频繁调用
*/
void CGlbGlobePoint::AddToScene()
{	
	/*
	*   mpt_isInHangTasks = true ------ 调用 AddToScene的线程 (A)
	*   mpt_isInHangTasks = false ----- frame 线程            (B)
	*   
	*/
	if(mpt_node == NULL)return;
	//if(mpt_isFaded)
	//{		
	//	glbref_ptr<GlbMarkerSymbolInfo> markerInfo = dynamic_cast<GlbMarkerSymbolInfo*>(mpr_renderInfo.get());

	//	if (!(markerInfo->symType == GLB_MARKERSYMBOL_LABEL ||
	//		markerInfo->symType == GLB_MARKERSYMBOL_IMAGE ||
	//		markerInfo->symType == GLB_MARKERSYMBOL_DYNAMICLABEL))
	//	{// 对注记文字和图片以及动态文字 不能使用渐隐渐现!!因为里面都设置了透明显示
	//		osg::ref_ptr<CGlbFadeInOutCallback> fadeCb = 
	//			new CGlbFadeInOutCallback(mpt_fadeStartAlpha,mpt_fadeEndAlpha,this,mpt_fadeDurationTime,mpt_fadeColor);
	//		osg::NodeCallback* nc = mpt_node->getUpdateCallback();

	//		if(nc != NULL)
	//			mpt_node->removeUpdateCallback(nc);

	//		mpt_node->addUpdateCallback(fadeCb.get());
	//	}		
	//}

	GlbScopedLock<GlbCriticalSection> lock(mpr_addToScene_critical);
	bool needUpdate = false;
	if(mpt_preNode == NULL)
	{//没有产生新osg节点
		if(mpt_HangTaskNum == 0
		   && mpt_node->getNumParents() == 0
		   && mpt_altitudeMode != GLB_ALTITUDEMODE_ONTERRAIN)
		{//贴地模式,节点不需要挂.
			glbDouble compileNodeTime = 0.0;
			//CGlbNodeIsCompiledVisitor nicv;
			//mpt_node->accept(nicv);
			//if (nicv.IsCompiled()==false)
			{// 计算draw过程中编译mpt_node中的gemery和texture生成显示列表需要的时间(毫秒)
				//osg::ref_ptr<osg::GraphicsCostEstimator> gce = new osg::GraphicsCostEstimator;
				//osg::CostPair compileCost = gce->estimateCompileCost(mpt_node.get());
				compileNodeTime = 0.001;//compileCost.first;//-1帧允许处理5个
			}

			
			if ( IsNeedToPreCompile()==false ||
					(IsNeedToPreCompile()==true && IsPreCompiled() == true))
			{
				if(mpt_isFaded)
				{		
					glbref_ptr<GlbMarkerSymbolInfo> markerInfo = dynamic_cast<GlbMarkerSymbolInfo*>(mpr_renderInfo.get());

					if (!(markerInfo->symType == GLB_MARKERSYMBOL_LABEL ||
						markerInfo->symType == GLB_MARKERSYMBOL_IMAGE ||
						markerInfo->symType == GLB_MARKERSYMBOL_DYNAMICLABEL))
					{// 对注记文字和图片以及动态文字 不能使用渐隐渐现!!因为里面都设置了透明显示
						osg::ref_ptr<CGlbFadeInOutCallback> fadeCb = 
							new CGlbFadeInOutCallback(mpt_fadeStartAlpha,mpt_fadeEndAlpha,this,mpt_fadeDurationTime,mpt_fadeColor);
						/*osg::NodeCallback* nc = mpt_node->getUpdateCallback();

						if(nc != NULL)
							mpt_node->removeUpdateCallback(nc);*/

						mpt_node->addUpdateCallback(fadeCb.get());
					}		
				}
				//挂mpt_node任务
				CGlbGlobeCallBack::CGlbGlobeCallBackTask* task = 
					new CGlbGlobeCallBack::CGlbGlobeCallBackTask(this,mpt_node,0,compileNodeTime);
				task->mpr_size = this->GetOsgNodeSize();			
				task->mpr_compileTime = compileNodeTime;
				mpt_globe->mpr_p_callback->AddHangTask(task);
				mpt_HangTaskNum ++;
				needUpdate = true;
			}

		}			
	}
	else
	{//产生了新osg节点		
#if 0
		{//旧节点已经挂到场景树
			//摘旧节点mpt_preNode的任务
			CGlbGlobeCallBack::CGlbGlobeCallBackTask* task = 
				new CGlbGlobeCallBack::CGlbGlobeCallBackTask(this,mpt_preNode);
			task->mpr_size = ComputeNodeSize(mpt_preNode);
			mpt_globe->mpr_p_callback->AddRemoveTask(task);
			mpt_preNode = NULL;
			//CGlbGlobeMemCtrl::GetInstance()->DecrementUsedMem(task->mpr_size);
			CGlbGlobeMemCtrl::GetInstance()->DecrementUsedMem(mpr_preNodeSize);			
		}
		if(mpt_altitudeMode != GLB_ALTITUDEMODE_ONTERRAIN)
		{//挂新节点mpt_node任务
			glbDouble compileNodeTime = 0.0;
			//CGlbNodeIsCompiledVisitor nicv;
			//mpt_node->accept(nicv);
			//if (nicv.IsCompiled()==false)
			//{// 计算draw过程中编译mpt_node中的gemery和texture生成显示列表需要的时间(毫秒)
				osg::ref_ptr<osg::GraphicsCostEstimator> gce = new osg::GraphicsCostEstimator;
				osg::CostPair compileCost = gce->estimateCompileCost(mpt_node.get());
				compileNodeTime = compileCost.first;
			//}

			CGlbGlobeCallBack::CGlbGlobeCallBackTask* task = 
				new CGlbGlobeCallBack::CGlbGlobeCallBackTask(this,mpt_node,0,compileNodeTime);
			task->mpr_size = this->GetOsgNodeSize();		
			task->mpr_compileTime = compileNodeTime;
			mpt_globe->mpr_p_callback->AddHangTask(task);
			mpt_HangTaskNum ++;	
		}
#else
		if(mpt_altitudeMode != GLB_ALTITUDEMODE_ONTERRAIN)
		{// 替换节点
			if ( IsNeedToPreCompile()==false ||
				(IsNeedToPreCompile()==true && IsPreCompiled() == true))
			{
				if(mpt_isFaded)
				{		
					glbref_ptr<GlbMarkerSymbolInfo> markerInfo = dynamic_cast<GlbMarkerSymbolInfo*>(mpr_renderInfo.get());

					if (!(markerInfo->symType == GLB_MARKERSYMBOL_LABEL ||
						markerInfo->symType == GLB_MARKERSYMBOL_IMAGE ||
						markerInfo->symType == GLB_MARKERSYMBOL_DYNAMICLABEL))
					{// 对注记文字和图片以及动态文字 不能使用渐隐渐现!!因为里面都设置了透明显示
						osg::ref_ptr<CGlbFadeInOutCallback> fadeCb = 
							new CGlbFadeInOutCallback(mpt_fadeStartAlpha,mpt_fadeEndAlpha,this,mpt_fadeDurationTime,mpt_fadeColor);
						//osg::NodeCallback* nc = mpt_node->getUpdateCallback();

						//if(nc != NULL)
						//	mpt_node->removeUpdateCallback(nc);

						mpt_node->addUpdateCallback(fadeCb.get());
					}		
				}

				CGlbGlobeCallBack::CGlbGlobeCallBackTask* task = 
					new CGlbGlobeCallBack::CGlbGlobeCallBackTask(this,mpt_preNode,mpt_node,0,0.0);
				mpt_globe->mpr_p_callback->AddReplaceTask(task);
				mpt_HangTaskNum ++;
			}
		}		
		
		//摘旧节点mpt_preNode的任务
		CGlbGlobeCallBack::CGlbGlobeCallBackTask* task = 
			new CGlbGlobeCallBack::CGlbGlobeCallBackTask(this,mpt_preNode);
		task->mpr_size = ComputeNodeSize(mpt_preNode);
		mpt_globe->mpr_p_callback->AddRemoveTask(task);		

		mpt_preNode = NULL;
		CGlbGlobeMemCtrl::GetInstance()->DecrementUsedMem(mpr_preNodeSize/*task->size*/);		
#endif
		needUpdate = true;
	}
	if(needUpdate)
	{
		//UpdateWorldExtent();
		if (mpt_globe)
			mpt_globe->UpdateObject(this);
		glbBool isnew = mpt_isDispShow && mpt_isShow;
		if (isnew == false)			
			mpt_node->asSwitch()->setValue(0,false);
		if (mpt_isSelected)
			DealSelected();
		if(mpt_isBlink)
			DealBlink();
	}
}
/*
*   update线程 --->Dispatcher--->RemoveFromScene(true)         从内存中卸载对象
*                      |
*                      |---->CallBack->RemoveFromeScene(false) 从显存中卸载对象
*
*   只有update线程会 调用该方法，所以不需要【加锁】
*/
glbInt32 CGlbGlobePoint::RemoveFromScene( glbBool isClean )
{
	SetEdit(false);
	if(mpr_isFadingDynamicLabel)
	{
		osg::ref_ptr<osg::NodeCallback> fadingLabelCallback = mpt_node->getUpdateCallback();
		if(fadingLabelCallback.valid())
			mpt_node->removeUpdateCallback(fadingLabelCallback.get());
		mpr_isFadingDynamicLabel = false;
	}
	{//与LoadData互斥.
		GlbScopedLock<GlbCriticalSection> lock(mpr_readData_critical);
		if (mpt_loadState == true)return 0;	
		mpt_loadState = true;
		/*
		*    mpt_node = node1,pre=NULL
		*    读:mpt_node = node2,pre=node1		
		*/
	}
	GlbScopedLock<GlbCriticalSection> lock(mpr_addToScene_critical);
	//处理贴地面
	glbref_ptr<GlbMarkerSymbolInfo> markerInfo = dynamic_cast<GlbMarkerSymbolInfo*>(mpr_renderInfo.get());
	if(    mpt_altitudeMode == GLB_ALTITUDEMODE_ONTERRAIN 
		&& markerInfo->symType == GLB_MARKERSYMBOL_2DSHAPE)
	{		
		DirtyOnTerrainObject();
	}
	glbInt32 tsize = 0;
	if(isClean == false)
	{//从显存卸载对象，节约显存.
		if ((mpt_node != NULL))// && (mpt_node->getNumParents() != 0))
		{//obj在显存	
			/*
			     mpt_node 已经在挂队列，但是还没挂到场景树上
				 这时判断getNumParents() != 0 是不可以的.
			*/
			CGlbGlobeCallBack::CGlbGlobeCallBackTask *task = 
				new CGlbGlobeCallBack::CGlbGlobeCallBackTask(this,mpt_node);
			task->mpr_size = this->GetOsgNodeSize();
			mpt_globe->mpr_p_callback->AddRemoveTask(task);
			tsize = mpr_zerolevelObjNodeSize;//task->mpr_size;
		}
	}
	else
	{//从内存中卸载对象，节约内存
		//删除 上一次装载的节点		
		if (mpt_preNode != NULL)
		{
			//if(mpt_preNode->getNumParents() > 0 )
			{				
				CGlbGlobeCallBack::CGlbGlobeCallBackTask* task = 
					new CGlbGlobeCallBack::CGlbGlobeCallBackTask(this,mpt_preNode);
				task->mpr_size = ComputeNodeSize(mpt_preNode);
				mpt_globe->mpr_p_callback->AddRemoveTask(task);
			}
			tsize += mpr_preNodeSize;//ComputeNodeSize(mpt_preNode);
			mpt_preNode = NULL;
		}
		//删除当前节点
		if (mpt_node != NULL) 
		{
			if ( IsNeedToPreCompile()==false ||
				(IsNeedToPreCompile()==true && IsPreCompiled() == true))
			{
				// 恢复isPrecompiled为false状态
				SetIsPreCompiled(false);	

				//if(mpt_node->getNumParents() > 0)
				{		
					CGlbGlobeCallBack::CGlbGlobeCallBackTask* task = 
						new CGlbGlobeCallBack::CGlbGlobeCallBackTask(this,mpt_node);
					task->mpr_size = this->GetOsgNodeSize();
					mpt_globe->mpr_p_callback->AddRemoveTask(task);
				}
				tsize       += mpr_zerolevelObjNodeSize;//this->GetOsgNodeSize();			
				mpt_node    =  NULL;
				mpr_objSize =  0;						
			}
		}		
		// 错误，不能加这种逻辑!!!2017.2.13 ml
		//if (mpt_feature != NULL && mpt_featureLayer != NULL)
		//{
		//	CGlbGlobeFeatureLayer* fl = dynamic_cast<CGlbGlobeFeatureLayer*>(mpt_featureLayer);
		//	if (fl)
		//		fl->NotifyFeatureDelete(mpt_feature->GetOid());
		//}
		mpt_currLevel = -1;					
	}
	mpt_loadState = false;
	//减少使用内存
	if(tsize>0 && isClean)
	{
		CGlbGlobeMemCtrl::GetInstance()->DecrementUsedMem(/*mpr_zerolevelObjNodeSize+mpr_preNodeSize*/tsize);			
	}
	return tsize;	
}
#include <osg/ComputeBoundsVisitor>
/*
    返回的外包有可能是NULL
	！！！！非常关键的方法！！！！ 索引，拾取准确性！！！
*/
glbref_ptr<CGlbExtent> CGlbGlobePoint::GetBound( glbBool isWorld /*= true*/ )
{
	if (mpt_globe == NULL && isWorld) return NULL;
	glbref_ptr<CGlbExtent> geoExt = NULL;
	if(    mpt_altitudeMode==GLB_ALTITUDEMODE_ONTERRAIN
			&& mpr_outline != NULL)
	{//贴地形绘制模式
		/*
		*   索引、查询【qtree】才能准确. z == 0.
		*/
		geoExt = const_cast<CGlbExtent *>(mpr_outline->GetExtent());
	}else{
		if(mpt_feature.get())
		{
			geoExt = const_cast<CGlbExtent *>(mpt_feature->GetExtent());
		}
		else
		{
			geoExt = new CGlbExtent();
			//if (mpr_geo && mpr_geo->GetExtent())
			//	geoExt = const_cast<CGlbExtent *>(mpr_geo->GetExtent());
			//else
			{
				geoExt->SetMin(mpr_xOrLon,mpr_yOrLat,mpr_zOrAlt);
				geoExt->SetMax(mpr_xOrLon,mpr_yOrLat,mpr_zOrAlt);
			}			
		}
	}
	if(isWorld == false)
	{//地理坐标
		/*
		   对象添加时，对贴地形对象,在场景树中是用的地理坐标!!!!
		   贴地形绘制，GetDistance会用到mpr_elevate.
		   所以：
		        调用UpdateElevate().
		*/
		if( mpr_elevation == DBL_MAX
			&& mpt_altitudeMode==GLB_ALTITUDEMODE_ONTERRAIN)
			   UpdateElevate();
		/*
		*   地理坐标，不反应相对地形绘制的地形海拔.
		*/
		return geoExt;
	}
	else
	{//世界坐标		
		if(geoExt == NULL)return NULL;
		glbref_ptr<CGlbExtent> worldExt = new CGlbExtent();
		GlbGlobeTypeEnum globeType = mpt_globe->GetType();
		if(mpt_altitudeMode==GLB_ALTITUDEMODE_ONTERRAIN)
		{//贴地形绘制
			double x,y,z;
			geoExt->GetMin(&x,&y,&z);
			z = mpt_globe->GetElevationAt(x,y);
			if(globeType == GLB_GLOBETYPE_GLOBE)
			{
				g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(y),
					osg::DegreesToRadians(x),z,x,y,z);
			}
			worldExt->Merge(x,y,z);
			geoExt->GetMax(&x,&y,&z);
			z = mpt_globe->GetElevationAt(x,y);
			if(globeType == GLB_GLOBETYPE_GLOBE)
			{
				g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(y),
					osg::DegreesToRadians(x),z,x,y,z);
			}
			worldExt->Merge(x,y,z);
			/*
			   贴地形绘制，GetDistance会用到mpr_elevate.
			   所以：
			        调用UpdateElevate().
			*/
			if( mpr_elevation == DBL_MAX)
				   UpdateElevate();
		}
		else
		{//非贴地形.						
			osg::ref_ptr<osg::Node> node = mpt_node;
			if(   node != NULL
				&&node->getBound().valid() )
			{
				//方案1
				//CGlbCalculateBoundBoxVisitor bboxV;
				//mpt_node->accept(bboxV);
				//osg::BoundingBoxd bb =bboxV.getBoundBox();
				osg::ComputeBoundsVisitor bboxV;
				mpt_node->accept(bboxV);
				osg::BoundingBox bb = bboxV.getBoundingBox();

				//方案2 - 此方案在调度时会放大点模型的显示误差，导致相机后的很多模型明明不可见却被场景管理器筛选为可见 2016.3.30 malin
				//osg::BoundingSphere bs = node->getBound();
				//osg::BoundingBoxd bb;
				//bb.expandBy(bs._center - Vec3d(bs._radius,bs._radius,bs._radius));
				//bb.expandBy(bs._center + Vec3d(bs._radius,bs._radius,bs._radius));

				worldExt->SetMin(bb.xMin(),bb.yMin(),bb.zMin());
				worldExt->SetMax(bb.xMax(),bb.yMax(),bb.zMax());
			}
			else
			{//osg 节点还没生成.				
				double x,y,z;
				geoExt->GetMin(&x,&y,&z);
				if( mpr_elevation == DBL_MAX)
				   UpdateElevate();
				if(mpt_altitudeMode == GLB_ALTITUDEMODE_RELATIVETERRAIN)
				{					
					z += mpr_elevation;
				}
				if(globeType == GLB_GLOBETYPE_GLOBE)
				{
					g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(y),
						osg::DegreesToRadians(x),z,x,y,z);					
				}
				worldExt->Merge(x,y,z);

				geoExt->GetMax(&x,&y,&z);
				if(mpt_altitudeMode == GLB_ALTITUDEMODE_RELATIVETERRAIN)
				{					
					z += mpr_elevation;
				}
				if(globeType == GLB_GLOBETYPE_GLOBE)
				{
					g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(y),
						osg::DegreesToRadians(x),z,x,y,z);
				}
				worldExt->Merge(x,y,z);
			}
		}//非贴地模式
		return worldExt;
	}//世界坐标	
	return NULL;
}
/*
 *   多点、线、多边形 所有点相对于中心统一 升降！！！.
 */
void CGlbGlobePoint::UpdateElevate()
{//只有相对地形绘制,才需要.
	if (!mpt_globe) return;
	if(mpt_altitudeMode == GLB_ALTITUDEMODE_ABSOLUTE)
	{
		//贴地形绘制,GetDistance()是会用到mpr_elevate.

		// 图片对象需要特殊处理 2016.6.7 malin
		if (mpt_node && mpt_node->getBound().valid())
		{
			osg::Vec3d cameraPos;
			mpt_globe->GetView()->GetCameraPos(cameraPos);
			osg::Vec3d position = mpt_node->getBound().center();
			osg::Vec3d cameraPoint = cameraPos;
			if(mpt_globe->GetType() == GLB_GLOBETYPE_GLOBE)	
				g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(cameraPos.y()),
				osg::DegreesToRadians(cameraPos.x()),cameraPos.z(),cameraPoint.x(),cameraPoint.y(),cameraPoint.z());	
			glbDouble dis = (position - cameraPoint).length();

			if (dis == mpr_preDistance)// 相机不动，就不必更新图片对象外包 --优化减少更新次数
				return;
			mpr_preDistance = dis;

			glbref_ptr<GlbMarkerSymbolInfo> markerInfo = dynamic_cast<GlbMarkerSymbolInfo*>(mpr_renderInfo.get());
			if (markerInfo->symType == GLB_MARKERSYMBOL_IMAGE)		
			{
				mpt_globe->UpdateObject(this);
			}			
		}			
		return;
	}

	glbDouble elevation = 0.0;
	if (mpt_feature.get())
	{
		CGlbExtent* ext = const_cast<CGlbExtent *>(mpt_feature->GetExtent());
		if(ext == NULL) return;
		glbDouble xOrLon,yOrLat;
		ext->GetCenter(&xOrLon,&yOrLat);
		elevation = mpt_globe->GetElevationAt(xOrLon,yOrLat);		
	}
	else
	{
		//glbDouble xOrLon,yOrLat;
		//if (mpr_geo && mpr_geo->GetExtent())
		//	mpr_geo->GetExtent()->GetCenter(&xOrLon,&yOrLat);
		//else
			elevation = mpt_globe->GetElevationAt(mpr_xOrLon,mpr_yOrLat);
	}
	
	if(fabs(elevation - mpr_elevation) < 0.0001)
	{// 从绝对模式 改为相对模式，elevation - mpr_elevation == 0.0,怎么办?
		/*
		    修改模式出必须调用DealModePosByChangeAltitudeOrChangePos
		*/
		return;
	}
	mpr_elevation = elevation;
	if(mpt_altitudeMode == GLB_ALTITUDEMODE_RELATIVETERRAIN)
	{//更新Maxtrixform
		DealModePosByChangeAltitudeOrChangePos();
	}
}
/*
*   获取2D绘制时的外围轮廓.
*   轮廓 ------>多点或多线.
*/
IGlbGeometry * CGlbGlobePoint::GetOutLine()
{
	if(mpr_outline != NULL)
		return mpr_outline.get();
	if(mpt_feature.get())
	{
		//CGlbExtent* ext = mpt_feature->GetExtent();
		//if(ext == NULL)return NULL;
		//double x,y,z;
		//ext->GetCenter(&x,&y,&z);
		//return new CGlbPoint3D(x,y,z);
	}else{
		//if (mpr_geo)	return mpr_geo.get();
		CGlbMultiPoint * multiPts = new CGlbMultiPoint;
		multiPts->AddPoint(mpr_xOrLon,mpr_yOrLat);
		return multiPts;
	}
	return NULL;
}

glbBool CGlbGlobePoint::SetRenderInfo( GlbRenderInfo *renderInfo,glbBool &isNeedDirectDraw )
{
	isNeedDirectDraw = false;
	if (renderInfo == NULL || renderInfo->type != GLB_OBJECTTYPE_POINT)  return false;
	GlbMarkerSymbolInfo *markerInfo = dynamic_cast<GlbMarkerSymbolInfo *>(renderInfo);
	if (markerInfo == NULL) return false;		
	{//与LoadData互斥.
		/*
		*   这是因为： ReadData执行在中途时，生成的结果可能不会反应 新设置的属性.
		*/
		GlbScopedLock<GlbCriticalSection> lock(mpr_readData_critical);
		if (mpt_loadState == true)
		{
			isNeedDirectDraw = true;
			//动态文字正在显示期间重绘的处理
			{
				if(mpr_isFadingDynamicLabel)
				{
					mpr_isFadingDynamicLabel = false;
					glbref_ptr<CGlbGlobeCallBack::CGlbGlobeFadeCallBackTask> task = new CGlbGlobeCallBack::CGlbGlobeFadeCallBackTask(mpt_node);
					mpt_globe->mpr_p_callback->AddFadeTask(task.get());
				}
			}
			mpr_renderInfo   = renderInfo;
			ParseObjectFadeColor();
			return true;			
		}
		mpt_loadState = true;
	}
	if(mpt_node == NULL)
	{		
		mpr_renderInfo   = renderInfo;
		//解析FillColor,开启渐进的时候需要
		ParseObjectFadeColor();
		mpt_loadState    = false;
		/*
			如果isNeedDirectDraw=true， 则图层渲染器renderer调用SetRenderInfo接口会导致
			图层中所有对象都要加载，如果图层对象量很大会直接导致崩溃！！！！
		*/
		isNeedDirectDraw = true;
		return true;
	}
	GlbMarkerSymbolInfo *tempMarkerInfo = dynamic_cast<GlbMarkerSymbolInfo *>(mpr_renderInfo.get());	
	if(markerInfo->symType != tempMarkerInfo->symType)
	{
		isNeedDirectDraw = true;
		mpr_renderInfo   = renderInfo;
		ParseObjectFadeColor();
		mpt_loadState    = false;
		return true;
	}
	glbBool rt = false;
	switch (markerInfo->symType)
	{
	case GLB_MARKERSYMBOL_MODEL:
		{
			GlbMarkerModelSymbolInfo *markerModelInfo = dynamic_cast<GlbMarkerModelSymbolInfo *>(markerInfo);
			if(markerModelInfo != NULL)
			{
				rt = DealModelSymbol(markerModelInfo,isNeedDirectDraw);		
				// 当是模型点符号时 默认 打开渐变效果 和lod模式
				mpt_isFaded = true;
				mpt_isLODEnable = true;
			}
		}break;
	case GLB_MARKERSYMBOL_2DSHAPE:
		{
			GlbMarker2DShapeSymbolInfo *marker2DInfo = dynamic_cast<GlbMarker2DShapeSymbolInfo *>(markerInfo);
			if(marker2DInfo != NULL)
				rt = Deal2DShapeSymbol(marker2DInfo,isNeedDirectDraw);
		}break;
	case GLB_MARKERSYMBOL_3DSHAPE:
		{
			GlbMarker3DShapeSymbolInfo *marker3DInfo = dynamic_cast<GlbMarker3DShapeSymbolInfo *>(markerInfo);
			if(marker3DInfo != NULL)
				rt = Deal3DShapeSymbol(marker3DInfo,isNeedDirectDraw);
		}break;
	case GLB_MARKERSYMBOL_PIXEL:
		{
			GlbMarkerPixelSymbolInfo *pixelInfo = dynamic_cast<GlbMarkerPixelSymbolInfo *>(markerInfo);
			if(pixelInfo != NULL)
				rt = DealPixelSymbol(pixelInfo,isNeedDirectDraw);
		}break;
	case  GLB_MARKERSYMBOL_LABEL:
		{
			GlbMarkerLabelSymbolInfo *labelInfo = dynamic_cast<GlbMarkerLabelSymbolInfo *> (markerInfo);				
			if (labelInfo != NULL)
				rt = DealLabelSymbol(labelInfo,isNeedDirectDraw);
		}break;
	case GLB_MARKERSYMBOL_DYNAMICLABEL:
		{
			GlbMarkerDynamicLabelSymbolInfo *dynamicLabelInfo = dynamic_cast<GlbMarkerDynamicLabelSymbolInfo *>(markerInfo);
			if(dynamicLabelInfo != NULL)
				rt = DealDynamicLabelSymbol(dynamicLabelInfo,isNeedDirectDraw);
		}break;
	case  GLB_MARKERSYMBOL_IMAGE:
		{
			GlbMarkerImageSymbolInfo *imageInfo = dynamic_cast<GlbMarkerImageSymbolInfo *>(markerInfo);
			if (imageInfo != NULL)
				rt = DealImageSymbol(imageInfo,isNeedDirectDraw);
		}break;
	case GLB_MARKERSYMBOL_FIRE:
	case GLB_MARKERSYMBOL_SMOKE://重绘
	case GLB_MARKERSYMBOL_EPICENTRE:
		{
			rt = true;
			isNeedDirectDraw = true;
		}
		break;
	}
	if(rt)
	{
		mpr_renderInfo = renderInfo;
		ParseObjectFadeColor();
	}
	mpt_loadState  = false;
	return rt;
}

GlbRenderInfo * CGlbGlobePoint::GetRenderInfo()
{
	return mpr_renderInfo.get();
}

glbBool CGlbGlobePoint::SetShow( glbBool isShow,glbBool isOnState )
{
	glbBool isold = mpt_isDispShow && mpt_isShow;

	if(isOnState)
	{// 调度器控制显隐
		mpt_isDispShow = isShow;
	}
	else
	{// 用户控制显隐
		mpt_isShow = isShow;
	}

	glbBool isnew = mpt_isDispShow && mpt_isShow;

	if(isold == isnew)
		return true;
	mpr_isCanDrawImage = isnew;
	if (mpt_isEdit && isOnState)// 编辑状态不再由调度控制显隐
		return true;

	glbref_ptr<GlbMarkerSymbolInfo> markerInfo = dynamic_cast<GlbMarkerSymbolInfo *>(mpr_renderInfo.get());
	if (!markerInfo) return false;

	if( mpt_altitudeMode == GLB_ALTITUDEMODE_ONTERRAIN
		&& markerInfo->symType == GLB_MARKERSYMBOL_2DSHAPE)
	{// 如果是贴地绘制对象，需要刷新贴地区域
		/*
		* mpr_isCanDrawImage控制在DrawToImage时是否需要绘制
		*/
		//if (mpr_isCanDrawImage != isnew)
		{
			// 刷新贴地区域
			DirtyOnTerrainObject();		
		}
	}
	else
	{
		if (mpt_node)
		{
			// 2016.3.29 改为从场景树摘除和挂上方式，以提高渲染桢速  malin 改为原来模式，帧速没有明显调高
			if(isnew)
			{
				glbref_ptr<GlbMarkerSymbolInfo> markerInfo = dynamic_cast<GlbMarkerSymbolInfo*>(mpr_renderInfo.get());
				if(markerInfo->symType == GLB_MARKERSYMBOL_DYNAMICLABEL)
					DirectDraw(0);
				else
					mpt_node->asSwitch()->setAllChildrenOn();				
				{ 		
					//CGlbGlobeCallBack::CGlbGlobeCallBackTask* task = 
					//	new CGlbGlobeCallBack::CGlbGlobeCallBackTask(this,mpt_node);
					//task->mpr_size = this->GetOsgNodeSize();
					//mpt_globe->mpr_p_callback->AddHangTask(task);
				}
			}
			else
			{
				mpt_node->asSwitch()->setAllChildrenOff();
				{		
					//osg::ref_ptr<CGlbFadeInOutCallback> fadeCb = dynamic_cast<CGlbFadeInOutCallback*> (mpt_node->getUpdateCallback());
					//if (fadeCb)
					//	mpt_node->removeUpdateCallback(fadeCb.get());

					//CGlbGlobeCallBack::CGlbGlobeCallBackTask* task = 
					//	new CGlbGlobeCallBack::CGlbGlobeCallBackTask(this,mpt_node);
					//task->mpr_size = this->GetOsgNodeSize();
					//mpt_globe->mpr_p_callback->AddRemoveTask(task);
				}
			}
		}		
	}

	return true;
}

glbBool CGlbGlobePoint::SetSelected( glbBool isSelected )
{
	if(mpt_isSelected == isSelected) return true;
	mpt_isSelected = isSelected;
	if (mpt_node != NULL)
	{
		if(GLB_ALTITUDEMODE_ONTERRAIN == mpt_altitudeMode)
			DirtyOnTerrainObject();		
		else
			DealSelected();
	}
	return true;
}

glbBool CGlbGlobePoint::SetBlink( glbBool isBlink )
{
	if (mpt_isBlink == isBlink) return true;
	mpt_isBlink = isBlink;
	if (mpt_node != NULL && mpt_altitudeMode!=GLB_ALTITUDEMODE_ONTERRAIN)
		DealBlink();
	return true;
}
/*
*    DirectDraw 与 LoadData  要互斥分:生产数据
*    mpr_loadState【glbBool】作为互斥分开.
*    Dispatcher线程---在 AddToScene  互斥.
*/
void CGlbGlobePoint::DirectDraw(glbInt32 level)
{
	if (mpt_parent    == NULL) return;		
	{//与LoadData互斥.
		GlbScopedLock<GlbCriticalSection> lock(mpr_readData_critical);
		if (mpt_loadState == true)
		{
			/*
			    LoadData->ReadData，选择退出，但是会产生如下问题：
				        LoadData->ReadData 使用的是旧的渲染相关的属性，不能反映新设置的渲染属性.
				还必须直接退出，如果选择等待LoadData->ReadData的话，会堵塞界面线程！！！！！

				所以设置一个标志，ReadData 执行到末尾时，检查该标志,如果标示是true，ReadData在执行一次.
			*/
			mpr_needReReadData = true;
			return;
		}
		mpt_loadState = true;
	}
	if(mpt_preNode != NULL)
	{
	/*
	*    mpr_node = node2,preNode = node1  ----A线程调用AddToScene，
	*                                         还没有preNode Remove还没执行完
	*    下面ReadData 已经做完：
	*    mpr_ndoe = node3,preNode = node2   造成node1永远不会摘除.
	*/
		AddToScene();
	}
	/*
	*    因为在LoadData->ReadData时，ReadData 装载对象后发现超过内存限额
	*        会抛弃新加载的osg节点.
	*    但对DirectDraw而言：如果抛弃新的osg节点，就不能反映最新的变化.
	*    所以：
	*         ReadData方法新增一个参数，标记是DirectDraw调用的,不管是否超过
	*         内存限额,都不抛弃新的osg节点.
	*/
	ReadData(level,true);
	//if (mpt_isNeedToPreCompile)
	//{// 如果需要预编译显示列表
	//	SetIsPreCompiled(false);
	//	osg::Node* objNode = GetOsgNode();
	//	if (objNode)
	//	{	
	//		GlbScopedLock<GlbCriticalSection> lock(mpt_globe->mpr_taskmanager->mpr_compile_critical);	
	//		// 必须用osg的makeCurrent,用opengl的wglMakeCurrent会有问题的！！！！2016.12.5
	//		mpt_globe->mpr_taskmanager->mpr_compileContext->makeCurrent();		

	//		osgUtil::GLObjectsVisitor glov;
	//		glov.setState(mpt_globe->mpr_taskmanager->mpr_compileContext->getState());
	//		objNode->accept(glov);		

	//		mpt_globe->mpr_taskmanager->mpr_compileContext->releaseContext();						
	//	}
	//	// 设置编译完成标志
	//	SetIsPreCompiled(true);
	//}
	AddToScene();

}

glbBool CGlbGlobePoint::SetAltitudeMode( GlbAltitudeModeEnum mode,glbBool &isNeedDirectDraw)
{
	if (mpt_altitudeMode == mode) return true;
	isNeedDirectDraw = false;
	if(mpt_globe == NULL)
	{
		mpt_altitudeMode = mode;
		return true;
	}
	{   		
		GlbScopedLock<GlbCriticalSection> lock(mpr_readData_critical);
		if (mpt_loadState == true)
		{
			/*
			 *  如果正在ReadData，ReadData可能在新的地形模式之前使用了旧的模式.
			 *  所以：
					需要告知调用者，调用DirectDraw.
			*/
			isNeedDirectDraw = true;			
			mpt_altitudeMode = mode;						
			return true;
		}
		mpt_loadState = true;
	}
	if (mpt_node == NULL)
	{
		mpt_altitudeMode = mode;
		/*
			如果isNeedDirectDraw=true， 则图层渲染器renderer调用SetAltitudeMode接口会导致
			图层中所有对象都要加载，如果图层对象量很大会直接导致崩溃！！！！
		*/
		//isNeedDirectDraw = true;
	}
	else
	{
		if(   mpt_altitudeMode == GLB_ALTITUDEMODE_ONTERRAIN
		||mode             == GLB_ALTITUDEMODE_ONTERRAIN)
		{//贴地模式 --->非贴地模式
		 //非贴地模式-->贴地模式 
			isNeedDirectDraw = true;			
			mpt_globe->mpr_sceneobjIdxManager->RemoveObject(this);	
			if (mpt_altitudeMode == GLB_ALTITUDEMODE_ONTERRAIN)
			{//旧模式是贴地模式,新模式不是贴地模式.			
				mpr_isCanDrawImage = false;			
				DirtyOnTerrainObject();		
				GlbScopedLock<GlbCriticalSection> lock(mpr_outline_critical);
				mpr_outline = NULL;
			}else{//旧模式不是贴地，新模式是贴地			
				RemoveNodeFromScene(true);//从场景树上移除节点
				mpr_isCanDrawImage = true;
			}
			mpt_altitudeMode = mode;
			/*
			贴地模式、相对模式会用到mpr_elevate.
			所以：
				 调用UpdateElevate();
			*/
			if(mpt_altitudeMode != GLB_ALTITUDEMODE_ABSOLUTE)
				UpdateElevate();
			mpt_globe->mpr_sceneobjIdxManager->AddObject(this);			
		}	
		else
		{//绝对模式 --->相对地形模式
			//相对地形模式-->绝对模式 
			mpt_altitudeMode = mode;
			if (mpt_altitudeMode == GLB_ALTITUDEMODE_RELATIVETERRAIN)
			{
				UpdateElevate();
				/*
					UpdateElevate()中有一条件：if(fabs(elevation - mpr_elevation) < 0.0001)
					在模式变换时,很有可能不满足上述条件。
					所以：
						 必须调用DealModePosByChangeAltitudeOrChangePos().
				*/
			}
			DealModePosByChangeAltitudeOrChangePos();			
		}	
	}
	mpt_loadState = false;
	return true;
}
/*
    只对贴地形绘制有作用.
*/
void CGlbGlobePoint::SetRenderOrder( glbInt32 order )
{
	if (mpt_renderOrder == order)return;
	CGlbGlobeREObject::SetRenderOrder(order);
	if (mpt_altitudeMode==GLB_ALTITUDEMODE_ONTERRAIN)
	{
		DirtyOnTerrainObject();				
	}		
}

void CGlbGlobePoint::SetPosition( glbDouble xOrLon,glbDouble yOrLat,glbDouble zOrAlt,glbBool &isNeedDirectDraw )
{
	isNeedDirectDraw = false;
	if(mpt_feature.get())
	{// 如果有要素，位置信息来源于要素.
		return;
	}

	if (CGlbCompareValue::CompareDoubleEqual(mpr_xOrLon,xOrLon) &&
		CGlbCompareValue::CompareDoubleEqual(mpr_yOrLat,yOrLat) &&
		CGlbCompareValue::CompareDoubleEqual(mpr_zOrAlt,zOrAlt))
		return;

	{   		
		GlbScopedLock<GlbCriticalSection> lock(mpr_readData_critical);
		if (mpt_loadState == true)
		{
			/*
			 *  如果正在ReadData，ReadData可能在新的地形模式之前使用了旧的位置.
			 *  所以：
					需要告知调用者，调用DirectDraw.
			*/
			mpr_xOrLon       = xOrLon;
			mpr_yOrLat       = yOrLat;
			mpr_zOrAlt       = zOrAlt;
			isNeedDirectDraw = true;						
			return ;
		}
		mpt_loadState = true;
	}		
	mpr_xOrLon    = xOrLon;
	mpr_yOrLat    = yOrLat;
	mpr_zOrAlt    = zOrAlt;	

	if (mpt_globe)// 更新空间索引树中位置
		mpt_globe->UpdateObject(this);
	if (mpt_node != NULL)
	{
		glbref_ptr<GlbMarkerSymbolInfo> markerInfo = dynamic_cast<GlbMarkerSymbolInfo *>(mpr_renderInfo.get());
		if(markerInfo == NULL)return;
		if ((markerInfo->symType == GLB_MARKERSYMBOL_2DSHAPE ||
			markerInfo->symType == GLB_MARKERSYMBOL_LABEL) &&
			mpt_altitudeMode == GLB_ALTITUDEMODE_ONTERRAIN)
		{
			DirtyOnTerrainObject();
			isNeedDirectDraw = true;
		}
		else if (mpt_altitudeMode != GLB_ALTITUDEMODE_ONTERRAIN)
			DealModePosByChangeAltitudeOrChangePos();		
	}else{//节点还没生成,如果需要立即看到，需要调用DirectDraw 
			// 补充 ： -- 不能这样做，因为有时是不需要立即看到的.应该由上层应用来自行决定是否需要调用DirectDraw		
		//isNeedDirectDraw = true;
	}
	mpt_loadState = false;
	return ;
}

void CGlbGlobePoint::GetPosition( glbDouble *xOrLon,glbDouble *yOrLat,glbDouble *zOrAlt )
{
	if(xOrLon == NULL
		|| yOrLat == NULL
		|| zOrAlt == NULL)return;
	*xOrLon = mpr_xOrLon;
	*yOrLat = mpr_yOrLat;
	*zOrAlt = mpr_zOrAlt;
}

//void CGlbGlobePoint::SetGeo(IGlbGeometry* geo,glbBool &isNeedDirectDraw)
//{
//	isNeedDirectDraw = false;
//	if(mpt_feature.get())
//	{// 如果有要素，位置信息来源于要素.
//		return;
//	}
//
//	mpr_geo = geo;
//
//	if (mpt_globe)// 更新空间索引树中位置
//		mpt_globe->UpdateObject(this);
//	if (mpt_node != NULL)
//	{
//		glbref_ptr<GlbMarkerSymbolInfo> markerInfo = dynamic_cast<GlbMarkerSymbolInfo *>(mpr_renderInfo.get());
//		if(markerInfo == NULL)return;
//		if ((markerInfo->symType == GLB_MARKERSYMBOL_2DSHAPE ||
//			markerInfo->symType == GLB_MARKERSYMBOL_LABEL) &&
//			mpt_altitudeMode == GLB_ALTITUDEMODE_ONTERRAIN)
//		{
//			DirtyOnTerrainObject();
//			isNeedDirectDraw = true;
//		}
//		else if (mpt_altitudeMode != GLB_ALTITUDEMODE_ONTERRAIN)
//			DealModePosByChangeAltitudeOrChangePos();		
//	}else{//节点还没生成,如果需要立即看到，需要调用DirectDraw 
//		// 补充 ： -- 不能这样做，因为有时是不需要立即看到的.应该由上层应用来自行决定是否需要调用DirectDraw		
//		//isNeedDirectDraw = true;
//	}
//	mpt_loadState = false;
//}
//
//IGlbGeometry* CGlbGlobePoint::GetGeo()
//{
//	return mpr_geo.get();
//}
/*
*    LoadData、DirectDraw 会调用.
*    应为LoadData、DirectDraw已经互斥，所以ReadData不用互斥了.
*
*    1. 非贴地形： 装载数据、生成osg节点、生成outline
*                  为什么生成outline?? 因为三位对象的区域选择---->投影的2D选择.
*    2. 贴地形  ： 装载数据、生成outline、刷新地形污染区域.
*
*       outline ----->多点 或 多线.
*/

#include "osgDB/WriteFile"

void CGlbGlobePoint::ReadData( glbInt32 level,glbBool isDirect )
{
	glbref_ptr<GlbMarkerSymbolInfo> markerInfo = dynamic_cast<GlbMarkerSymbolInfo*>(mpr_renderInfo.get());
	if( markerInfo == NULL || ( mpt_altitudeMode == GLB_ALTITUDEMODE_ONTERRAIN 
		&& !(markerInfo->symType == GLB_MARKERSYMBOL_2DSHAPE || 
		markerInfo->symType == GLB_MARKERSYMBOL_LABEL)))
	{mpt_loadState=false;return;}

	if (markerInfo->symType == GLB_MARKERSYMBOL_MODEL)
	{// 当是模型点符号时 默认 打开渐变效果 2017.11.21 ml
		mpt_isFaded = true;
		mpt_isLODEnable = true;
	}

	osg::ref_ptr<osg::NodeCallback> fadeCb = NULL;
	if (markerInfo == NULL) {mpt_loadState=false;return;}
	glbref_ptr<CGlbMultiPoint> points = new CGlbMultiPoint(3,false);
	if (mpt_feature != NULL)
	{
		IGlbGeometry* geo = NULL;
		bool result = false;
		//mpt_geoField.ToUpper();
		//if(mpt_geoField != L"" && mpt_geoField != L"GLBGEO")
		//	result = mpt_feature->GetSubGeometry(mpt_geoField.c_str(),&geoCollect,level);
		//else
			result = mpt_feature->GetGeometry(&geo,0/*level*/);
		if(!result || geo==NULL)  {mpt_loadState=false;return;}

		// 使用智能指针防止多线程访问导致geo被卸载
		glbref_ptr<IGlbGeometry> tempGeo = geo;

		glbDouble height=mpr_zOrAlt;//0.0 ！！！！！！！！测试用，如果是二维点，高度用此作为默认高度
		glbDouble xOrLon,yOrLat/*,zOrAlt,M*/;
				
		glbInt32 dimension = geo->GetCoordDimension();
		GlbGeometryTypeEnum geoType = geo->GetType();
		switch(geoType)
		{
		case GLB_GEO_POINT:				///<二维点
			{						
				CGlbPoint* pt = dynamic_cast<CGlbPoint *>(geo);
				if (dimension==2)
					pt->GetXY(&xOrLon,&yOrLat);
				else if(dimension==3)
					pt->GetXYZ(&xOrLon,&yOrLat,&height);
				points->AddPoint(xOrLon,yOrLat,height);						
			}
			break;
		case GLB_GEO_MULTIPOINT:		///<二维多点
			{				
				CGlbMultiPoint* multipt = dynamic_cast<CGlbMultiPoint*>(geo);
				glbInt32 ptCnt = multipt->GetCount();
				for (glbInt32 k = 0; k < ptCnt; k++)
				{
					if (dimension == 2)
						multipt->GetPoint(k,&xOrLon,&yOrLat);
					else if (dimension == 3)
						multipt->GetPoint(k,&xOrLon,&yOrLat,&height);
					points->AddPoint(xOrLon,yOrLat,height);	
				}
			}
			break;	
		}		
	}
	else 
	{		
		points->AddPoint(mpr_xOrLon,mpr_yOrLat,mpr_zOrAlt);	
	}

	ref_ptr<osg::Switch> swiNode = new osg::Switch;
	ref_ptr<osg::Node> node = NULL;
	glbDouble xLon = 0.0 , yLat = 0.0 , zAlt = 0.0;
	glbDouble yaw = 0.0 , pitch = 0.0 , roll = 0.0;
	glbDouble xScale = 1 , yScale = 1 , zScale = 1;
	
	if(mpt_altitudeMode == GLB_ALTITUDEMODE_ONTERRAIN)
	{//贴地面刷新旧区域
		DirtyOnTerrainObject();
	}

	if (mpt_altitudeMode == GLB_ALTITUDEMODE_ONTERRAIN &&
		markerInfo->symType == GLB_MARKERSYMBOL_2DSHAPE)
	{
		glbref_ptr<CGlbGlobeMarker2DShapeSymbol> marker2DSymbol = new CGlbGlobeMarker2DShapeSymbol;
		glbref_ptr<CGlbMultiLine> outlines = NULL;
		glbref_ptr<CGlbMultiLine> noRotOutlines = NULL;
		glbref_ptr<CGlbLine> outline = NULL;
		if (points->GetCount()>=1)
		{
			outlines = new CGlbMultiLine;
			noRotOutlines = new CGlbMultiLine;
			for (glbInt32 i = 0; i < points->GetCount();i++)
			{
				if(points->GetPoint(i,&xLon,&yLat,&zAlt))
				{
					glbref_ptr<CGlbPoint> point = new CGlbPoint(xLon,yLat,zAlt);
					outline = dynamic_cast<CGlbLine *>(marker2DSymbol->GetOutline(this,point.get()));
					if(outline)
						outlines->AddLine(outline.get());
					outline = dynamic_cast<CGlbLine *>(marker2DSymbol->GetNoRotOutline(this,point.get()));
					if(outline)
						noRotOutlines->AddLine(outline.get());
				}
			}	
			{
				GlbScopedLock<GlbCriticalSection> lock(mpr_outline_critical);
				mpr_outline = outlines;
			}
			mpr_noRotOutline = noRotOutlines;
			DirtyOnTerrainObject();
			// 更新场景树中的范围
			mpt_globe->mpr_sceneobjIdxManager->UpdateObject(this);
		}		
	}
	else if(mpt_altitudeMode == GLB_ALTITUDEMODE_ONTERRAIN && 
		markerInfo->symType == GLB_MARKERSYMBOL_LABEL)
	{
		glbref_ptr<CGlbGlobeMarkerLabelSymbol> markerLabel = new CGlbGlobeMarkerLabelSymbol;
		glbref_ptr<CGlbMultiLine> outlines = NULL;
		glbref_ptr<CGlbLine> outline = NULL;
		if(points->GetCount() >= 1)
		{
			outlines = new CGlbMultiLine;
			for(glbInt32 i = 0;i < points->GetCount();i++)
			{
				if(points->GetPoint(i,&xLon,&yLat,&zAlt))
				{
					glbref_ptr<CGlbPoint> point = new CGlbPoint(xLon,yLat,zAlt);
					outline = dynamic_cast<CGlbLine *>(markerLabel->GetOutline(this,point.get()));
					outlines->AddLine(outline.get());
				}
			}
			{
				GlbScopedLock<GlbCriticalSection> lock(mpr_outline_critical);
				mpr_outline = outlines;
			}
			DirtyOnTerrainObject();
			mpt_globe->mpr_sceneobjIdxManager->UpdateObject(this);
			mpr_isDirectDraw = true;
		}
	}
	else
	{
		glbDouble yaw = 0.0 , pitch = 0.0 , roll = 0.0;
		glbDouble xScale = 1 , yScale = 1 , zScale = 1;
		glbDouble xOffset = 0.0,yOffset = 0.0,zOffset = 0.0;
		
		//added by lkx to debug
		{
			glbref_ptr<CGlbPoint> point = new CGlbPoint(mpr_xOrLon,mpr_yOrLat,mpr_zOrAlt);
			if (markerInfo->symType == GLB_MARKERSYMBOL_MODEL)
			{
				GlbMarkerModelSymbolInfo *markerModelInfo = 
					(GlbMarkerModelSymbolInfo *)markerInfo.get();
				if (markerModelInfo == NULL) return;			
				CGlbGlobeMarkerModelSymbol markerModelSymbol;
				node = markerModelSymbol.Draw(this,point.get(),level);	
				/*{
					CGlbWString datalocate = markerModelInfo->locate->value;
					node = osgDB::readNodeFile(datalocate.ToString());				
				}*/
				//this->ComputeNodeSize(node);
				if(markerModelInfo->xScale)
					xScale = markerModelInfo->xScale->GetValue(mpt_feature.get());
				if(markerModelInfo->yScale)
					yScale = markerModelInfo->yScale->GetValue(mpt_feature.get());
				if(markerModelInfo->zScale)
					zScale = markerModelInfo->zScale->GetValue(mpt_feature.get());
				if(markerModelInfo->yaw)
					yaw = markerModelInfo->yaw->GetValue(mpt_feature.get());
				if(markerModelInfo->pitch)
					pitch = markerModelInfo->pitch->GetValue(mpt_feature.get());
				if(markerModelInfo->roll)
					roll = markerModelInfo->roll->GetValue(mpt_feature.get());
				if(markerModelInfo->xOffset)
					xOffset = markerModelInfo->xOffset->GetValue(mpt_feature.get());
				if(markerModelInfo->yOffset)
					yOffset = markerModelInfo->yOffset->GetValue(mpt_feature.get());
				if(markerModelInfo->zOffset)
					zOffset = markerModelInfo->zOffset->GetValue(mpt_feature.get());
			}
			else if (markerInfo->symType == GLB_MARKERSYMBOL_2DSHAPE)
			{
				GlbMarker2DShapeSymbolInfo *marker2DShapeInfo = 
					(GlbMarker2DShapeSymbolInfo *)markerInfo.get();
				if(marker2DShapeInfo == NULL)return;
				CGlbGlobeMarker2DShapeSymbol marker2DSymbol;
				node = marker2DSymbol.Draw(this,point.get());
				if(marker2DShapeInfo->yaw)
					yaw = marker2DShapeInfo->yaw->GetValue(mpt_feature.get());
				if(marker2DShapeInfo->xScale)
					xScale = marker2DShapeInfo->xScale->GetValue(mpt_feature.get());
				if(marker2DShapeInfo->yScale)
					yScale = marker2DShapeInfo->yScale->GetValue(mpt_feature.get());
				if(marker2DShapeInfo->zScale)
					zScale = marker2DShapeInfo->zScale->GetValue(mpt_feature.get());
				if(marker2DShapeInfo->xOffset)
					xOffset = marker2DShapeInfo->xOffset->GetValue(mpt_feature.get());
				if(marker2DShapeInfo->yOffset)
					yOffset = marker2DShapeInfo->yOffset->GetValue(mpt_feature.get());
				if(marker2DShapeInfo->zOffset)
					zOffset = marker2DShapeInfo->zOffset->GetValue(mpt_feature.get());
			}
			else if(markerInfo->symType == GLB_MARKERSYMBOL_3DSHAPE)
			{
				GlbMarker3DShapeSymbolInfo *marker3DShapeInfo = 
					(GlbMarker3DShapeSymbolInfo *)markerInfo.get();
				if(marker3DShapeInfo == NULL)return;
				CGlbGlobeMarker3DShapeSymbol marker3DSymbol;
				node = marker3DSymbol.Draw(this,point.get());
				node->getOrCreateStateSet()->setMode(GL_CULL_FACE,osg::StateAttribute::OFF | osg::StateAttribute::PROTECTED);
				if(marker3DShapeInfo->pitch)
					pitch = marker3DShapeInfo->pitch->GetValue(mpt_feature.get());
				if(marker3DShapeInfo->yaw)
					yaw = marker3DShapeInfo->yaw->GetValue(mpt_feature.get());
				if(marker3DShapeInfo->roll)
					roll = marker3DShapeInfo->roll->GetValue(mpt_feature.get());
				if(marker3DShapeInfo->xScale)
					xScale = marker3DShapeInfo->xScale->GetValue(mpt_feature.get());
				if(marker3DShapeInfo->yScale)
					yScale = marker3DShapeInfo->yScale->GetValue(mpt_feature.get());
				if(marker3DShapeInfo->zScale)
					zScale = marker3DShapeInfo->zScale->GetValue(mpt_feature.get());
				if(marker3DShapeInfo->xOffset)
					xOffset = marker3DShapeInfo->xOffset->GetValue(mpt_feature.get());
				if(marker3DShapeInfo->yOffset)
					yOffset = marker3DShapeInfo->yOffset->GetValue(mpt_feature.get());
				if(marker3DShapeInfo->zOffset)
					zOffset = marker3DShapeInfo->zOffset->GetValue(mpt_feature.get());
			}
			else if (markerInfo->symType == GLB_MARKERSYMBOL_PIXEL)
			{
				GlbMarkerPixelSymbolInfo *markerPixelInfo = 
					(GlbMarkerPixelSymbolInfo *)markerInfo.get();
				if(markerPixelInfo == NULL)return;
				CGlbGlobeMarkerPixelSymbol markerPixelSymbol;
				node = markerPixelSymbol.Draw(this,point.get());
			}
			else if (markerInfo->symType == GLB_MARKERSYMBOL_FIRE 
				|| markerInfo->symType == GLB_MARKERSYMBOL_SMOKE)
			{
				node  = new osg::Node;
			}		
			else if (markerInfo->symType == GLB_MARKERSYMBOL_EPICENTRE)
			{
				node = new osg::Node;
			}
			else if (markerInfo->symType == GLB_MARKERSYMBOL_LABEL ||
				markerInfo->symType == GLB_MARKERSYMBOL_IMAGE ||
				markerInfo->symType == GLB_MARKERSYMBOL_DYNAMICLABEL)
				node = new osg::Node;
		}
		if (node == NULL)
		{mpt_loadState=false;return;}

		// 处理 GLB_MARKERSYMBOL_LABEL或GLB_MARKERSYMBOL_IMAGE
		for (glbInt32 i = 0; i < points->GetCount();i++)
		{
			ref_ptr<osg::MatrixTransform> mt = new osg::MatrixTransform;
			osg::Matrix m2;
			osg::Matrix m3;
			osg::Matrix mOffset;
			osg::Vec3d position;
			if(points->GetPoint(i,&position.x(),&position.y(),&position.z()))			
				ComputePosByAltitudeAndGloleType(position);
			glbref_ptr<CGlbPoint> point = new CGlbPoint(position.x(),position.y(),position.z());

			if (markerInfo->symType == GLB_MARKERSYMBOL_LABEL)
			{
				GlbMarkerLabelSymbolInfo *markerLabelInfo = 
					(GlbMarkerLabelSymbolInfo *)markerInfo.get();
				if(markerLabelInfo == NULL)return;
				CGlbGlobeMarkerLabelSymbol markerLabelSymbol;
				node = markerLabelSymbol.Draw(this,point.get());
				if(node == NULL) {mpt_loadState=false;continue;}
			}
			if (markerInfo->symType == GLB_MARKERSYMBOL_DYNAMICLABEL)
			{
				GlbMarkerDynamicLabelSymbolInfo *markerLabelInfo = 
					(GlbMarkerDynamicLabelSymbolInfo *)markerInfo.get();
				if(markerLabelInfo == NULL)return;
				CGlbGlobeMarkerDynamicLabelSymbol markerDynamicLabelSymbol;
				CGlbWString content = L"";
				node = markerDynamicLabelSymbol.Draw(this,point.get());
				if(node == NULL) {mpt_loadState=false;continue;}
				osg::ref_ptr<osg::Image> texImage = markerDynamicLabelSymbol.GetTexImage();
				unsigned char *aggBuffer = markerDynamicLabelSymbol.GetAggBuffer();
				osg::Vec2i buffersize = markerDynamicLabelSymbol.GetAggBufferWidthAndHeight();
				fadeCb = new DynamicLabelFadeCallback(texImage,aggBuffer,buffersize,this);
			}
			else if (markerInfo->symType == GLB_MARKERSYMBOL_IMAGE)
			{
				GlbMarkerImageSymbolInfo *markerImageInfo = 
					(GlbMarkerImageSymbolInfo *)markerInfo.get();
				if(markerImageInfo == NULL)return;
				CGlbGlobeMarkerImageSymbol markerImageSymbol;
				node = markerImageSymbol.Draw(this,point.get());
				if(node == NULL) {mpt_loadState=false;continue;}
			}
			else if (markerInfo->symType == GLB_MARKERSYMBOL_FIRE)
			{
				GlbMarkerFireSymbolInfo *markerFireInfo = 
					(GlbMarkerFireSymbolInfo *)markerInfo.get();
				if(markerFireInfo == NULL)return;
				CGlbGlobeMarkerFireSymbol markerFireSymbol;
				node = markerFireSymbol.Draw(this,point.get());	
				if(node == NULL) {mpt_loadState=false;continue;}
				//swiNode->setNodeMask(0x01);// 设置nodemask为0x01不允许选择
			}
			else if (markerInfo->symType == GLB_MARKERSYMBOL_SMOKE)
			{
				GlbMarkerSmokeSymbolInfo *markerSmokeInfo = 
					(GlbMarkerSmokeSymbolInfo *)markerInfo.get();
				if(markerSmokeInfo == NULL)return;
				CGlbGlobeMarkerSmokeSymbol markerSmokeSymbol;
				node = markerSmokeSymbol.Draw(this,point.get());	
				if(node == NULL) {mpt_loadState=false;continue;}
				swiNode->setNodeMask(0x01);// 设置nodemask为0x01不允许选择
			}
			else if (markerInfo->symType == GLB_MARKERSYMBOL_EPICENTRE)
			{// 震源点符号
				GlbMarkerEpicentreSymbolInfo *markerEpicentreInfo = 
					(GlbMarkerEpicentreSymbolInfo *)markerInfo.get();
				if(markerEpicentreInfo == NULL)return;
				CGlbGlobeMarkerEpicentreSymbol markerEpicentreSymbol;
				node = markerEpicentreSymbol.Draw(this,point.get());
				if(node == NULL) {mpt_loadState=false;continue;}
			}

			osg::Matrixd localToWorld;

			m2.makeScale(xScale,yScale,zScale);
			yaw = osg::DegreesToRadians(yaw);
			pitch = osg::DegreesToRadians(pitch);
			roll = osg::DegreesToRadians(roll);
			osg::Vec3d yaw_vec(0.0,0.0,1.0);
			osg::Vec3d pitch_vec(cos(yaw),sin(yaw),0.0);
			osg::Vec3d roll_vec(-sin(yaw)*cos(pitch),cos(pitch)*cos(yaw),sin(pitch));
			m3.makeRotate(yaw,yaw_vec,pitch,pitch_vec,roll,roll_vec);

			if(mpt_globe->GetType() == GLB_GLOBETYPE_GLOBE)
			{
				//
				double longitude = osg::DegreesToRadians(mpr_xOrLon);
				double latitude = osg::DegreesToRadians(mpr_yOrLat);

				// Compute up vector
				osg::Vec3d    up      ( cos(longitude)*cos(latitude), sin(longitude)*cos(latitude), sin(latitude));
				// Compute east vector
				osg::Vec3d    east    (-sin(longitude), cos(longitude), 0);
				// Compute north  vector = outer product up x east
				osg::Vec3d    north   = up ^ east;

				north.normalize();
				osg::Vec3 _Offset = east*xOffset + north*yOffset + up*zOffset;
				mOffset.makeTranslate(_Offset);
				//
				g_ellipsoidModel->computeLocalToWorldTransformFromXYZ(position.x(),position.y(),position.z(),localToWorld);
			}
			else if(mpt_globe->GetType() == GLB_GLOBETYPE_FLAT)
			{
				//
				mOffset.makeTranslate(xOffset,yOffset,zOffset);
				//
				localToWorld.makeTranslate(position.x(),position.y(),position.z());
			}
			mt->setMatrix(m2 * m3 * localToWorld * mOffset);

			// 烟和火不需要矩阵变换
			if (markerInfo->symType == GLB_MARKERSYMBOL_SMOKE || markerInfo->symType == GLB_MARKERSYMBOL_SMOKE)
				mt->setMatrix(osg::Matrix::identity());
				
			//this->ComputeNodeSize(node);
			mt->addChild(node);
			//this->ComputeNodeSize(mt);
			swiNode->addChild(mt);
			//this->ComputeNodeSize(swiNode);
		}
		{
			GlbScopedLock<GlbCriticalSection> lock(mpr_outline_critical);
			mpr_outline = points;
		}		

		//处理模型的透明度，因为模型需要实例化共享，
		//所以不能在CGlbGlobeMarkerModelSymbol::Draw里处理模型的透明度
		if(markerInfo->symType == GLB_MARKERSYMBOL_MODEL)
		{
			GlbMarkerModelSymbolInfo *modelInfo = 
				(GlbMarkerModelSymbolInfo *)markerInfo.get();
			if (modelInfo != NULL)
				DealModelOpacity(swiNode,modelInfo);				
		}
	}	
	if(mpr_needReReadData)
	{
		/*
		*   在检测mpr_needReReadData时,其它线程设置mpr_needReReadData=true还没执行！！！.
		*/
		mpr_needReReadData = false;
		return ReadData(level,true);
	}
	//if (mpt_altitudeMode != GLB_ALTITUDEMODE_ONTERRAIN)
	//{// 非贴地模式需要控制内存增量
		//增加使用内存		
		glbInt32 objsize = ComputeNodeSize(swiNode);		
		//glbBool isOK = CGlbGlobeMemCtrl::GetInstance()->IncrementUsedMem(objsize);	
		glbInt32 nodeSize = objsize * pow(3.0f,level);		
		glbBool isOK = CGlbGlobeMemCtrl::GetInstance()->IncrementUsedMem(nodeSize);
		if(isOK||isDirect)
		{
			if (mpt_node!=NULL)
			{
				mpr_preNodeSize = mpr_zerolevelObjNodeSize;
				NodeCallback *ncb = mpt_node->getUpdateCallback();
				if(ncb)
					mpt_node->removeUpdateCallback(ncb);
			}
			mpr_zerolevelObjNodeSize = nodeSize;

			// 设置预编译标志为false
			//if (IsNeedToPreCompile())	SetIsPreCompiled(false);

			// 预先计算bound，节省时间
			swiNode->getBound();

			//// 共享			
			//if (osgDB::Registry::instance()->getSharedStateManager())
			//	osgDB::Registry::instance()->getSharedStateManager()->share(swiNode.get());

			mpt_preNode   = mpt_node;
			if(this->IsNeedToPreCompile()== true )
			{
				mpt_node = NULL;
				mpt_precompileNode = swiNode;
				if(fadeCb.valid())
				{
					mpt_precompileNode->addUpdateCallback(fadeCb.get());
					mpr_isFadingDynamicLabel = true;
				}
			}
			else
			{
				mpt_node = swiNode;

				if(fadeCb.valid())
				{
					mpt_node->addUpdateCallback(fadeCb.get());
					mpr_isFadingDynamicLabel = true;
				}
			}
			mpt_currLevel = level;
			mpr_objSize   = objsize;

			//if (true)
			//{// 测试
			//	std::string levelNm = (level==0) ? "-0" : "-1";
			//	osg::Node* node = mpt_node->asGroup()->getChild(0)->asGroup()->getChild(0);
			//	std::string newName = node->getName() + levelNm;				
			//	swiNode->setName(newName.c_str());

			//	WCHAR wBuff[128];
			//	CGlbString nodeName = swiNode->getName();
			//	CGlbString _time = GetSystemTime();
			//	wsprintf(wBuff,L"%s %s readData level: %d .\n",_time.ToWString().c_str(),nodeName.ToWString().c_str(), level);
			//	OutputDebugString(wBuff);
			//}
		}else{
			swiNode = NULL;
		}
	//}
	mpt_loadState = false;	
}

CGlbGlobePoint::CGlbGlobePointTask::CGlbGlobePointTask( CGlbGlobePoint *obj,glbInt32 level )
{
	mpr_obj = obj;
	mpr_level = level;
}

CGlbGlobePoint::CGlbGlobePointTask::~CGlbGlobePointTask( )
{
	mpr_obj = NULL;
}

glbBool CGlbGlobePoint::CGlbGlobePointTask::doRequest()
{
	if(mpr_obj)
	{
		mpr_obj->ReadData(mpr_level);
		return true;
	}
	return false;
}

CGlbGlobeObject * CGlbGlobePoint::CGlbGlobePointTask::getObject()
{
	return mpr_obj.get();
}

void CGlbGlobePoint::DealModelOpacity( osg::Node *node,GlbMarkerModelSymbolInfo *modelInfo )
{
	osg::ref_ptr<osg::StateSet> stateset = node->getOrCreateStateSet();
	stateset->setMode( GL_RESCALE_NORMAL/*GL_NORMALIZE*/, osg::StateAttribute::ON );
	osg::ref_ptr<osg::Material> material = dynamic_cast<osg::Material *>(stateset->getAttribute(osg::StateAttribute::MATERIAL));
	//osg::ref_ptr<osg::Depth> depth = NULL;
	//osg::ref_ptr<osg::BlendFunc> pBlendFun = new osg::BlendFunc(osg::BlendFunc::SRC_ALPHA,osg::BlendFunc::ONE_MINUS_SRC_ALPHA);
	//stateset->setAttributeAndModes(pBlendFun.get(),osg::StateAttribute::ON);
	if(modelInfo && modelInfo->opacity)
	{
		glbInt32 opacity = modelInfo->opacity->GetValue(mpt_feature.get());
		
		if (opacity<100)
		{
			if(material == NULL)
				material = new osg::Material;
			//material->setAlpha(osg::Material::FRONT_AND_BACK,opacity / 100.0);
			//stateset->setAttribute(material,osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);

			//stateset->setMode(GL_LIGHTING,osg::StateAttribute::ON);
			stateset->setMode(GL_BLEND,osg::StateAttribute::ON);
			if (stateset->getRenderingHint() != osg::StateSet::TRANSPARENT_BIN)
			{
				stateset->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
				stateset->setRenderBinMode(osg::StateSet::OVERRIDE_RENDERBIN_DETAILS);
			}

			//depth = dynamic_cast<osg::Depth *>(stateset->getAttribute(osg::StateAttribute::DEPTH));
			//if(!depth.valid())
			//{
			//	depth = new osg::Depth;
			//	depth->setWriteMask(false);
			//	stateset->setAttributeAndModes(depth.get(), osg::StateAttribute::ON);				
			//}
			//else
			//{
			//	if(depth->getWriteMask())
			//		depth->setWriteMask(false);
			//}
			CGlbOpacityVisitor nodeVisiter(opacity);
			node->accept(nodeVisiter);
		}
		else
		{
			if((!mpt_isSelected) && (!mpt_isBlink))
			{
				//stateset->setMode(GL_LIGHTING,osg::StateAttribute::OFF/* | osg::StateAttribute::OVERRIDE*/);
				if (material)
					stateset->removeAttribute(material);
			}
			stateset->setMode(GL_BLEND,osg::StateAttribute::OFF);
			stateset->setRenderingHint(osg::StateSet::OPAQUE_BIN);
			//depth = dynamic_cast<osg::Depth *>(stateset->getAttribute(osg::StateAttribute::DEPTH));
			//if(!depth.valid())
			//{
			//	depth = new osg::Depth;
			//	depth->setWriteMask(true);
			//	ss->setAttributeAndModes(depth.get(), osg::StateAttribute::ON);
			//}
			//else
			//{
			//	if(!depth->getWriteMask())
			//		depth->setWriteMask(true);
			//}
		}
	}
	else
	{
		stateset->setMode(GL_BLEND,osg::StateAttribute::OFF);
		stateset->setRenderingHint(osg::StateSet::OPAQUE_BIN);
		//if(!mpt_isSelected || !mpt_isBlink)
		//	stateset->setMode(GL_LIGHTING,osg::StateAttribute::OFF/*|osg::StateAttribute::OVERRIDE*/);
	}
}
/*
*                  Switch
*            /                \     
*           /                  \
*MatrixTransform1(point1)       MatrixTransform2(point1)
*/
void CGlbGlobePoint::DealModePosByChangeAltitudeOrChangePos()
{
	if(mpt_node == NULL)return;
	unsigned int numChild = mpt_node->asSwitch()->getNumChildren();
	osg::Matrixd offsetMatrix = ComputePosOffsetMatrix();
	for (unsigned int i = 0; i < numChild; i++)
	{
		osg::MatrixTransform *mt = 
			dynamic_cast<osg::MatrixTransform *>(mpt_node->asSwitch()->getChild(i));
        if(mt == NULL)continue;
		if (mpt_feature.get())
		{
			glbDouble xOrLon,yOrLat,zOrAlt,M;
			zOrAlt = 0.0;
			M      = 0.0;
			if(GetFeaturePoint(i,&xOrLon,&yOrLat,&zOrAlt,&M) == false)
				return;
			/*
			*   m 包含了 trans,scale,rotate三个矩阵.
			*   此处只修改trans.
			*/
			osg::Matrix m = mt->getMatrix();
			osg::Vec3d position(xOrLon,yOrLat,zOrAlt);
			ComputePosByAltitudeAndGloleType(position);
			m.setTrans(position * offsetMatrix);	
			mt->setMatrix(m);
		}
		else
		{
			osg::Matrix m = mt->getMatrix();
			osg::Vec3d position(mpr_xOrLon,mpr_yOrLat,mpr_zOrAlt);
			ComputePosByAltitudeAndGloleType(position);
			m.setTrans(position * offsetMatrix);
			mt->setMatrix(m);			
		}
	}
	//更新对象在场景索引位置.
	mpt_globe->UpdateObject(this);
}

void CGlbGlobePoint::ComputePosByAltitudeAndGloleType( osg::Vec3d &position )
{	
	if (mpt_globe->GetType() == GLB_GLOBETYPE_GLOBE)
	{
		if (mpt_altitudeMode == GLB_ALTITUDEMODE_ABSOLUTE)
		{
			//把xOrLon、yOrLat、zOrAlt的值转换成世界坐标赋值给position的x、y、z
			g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(position.y()),
				osg::DegreesToRadians(position.x()),position.z(),position.x(),position.y(),position.z());
		}
		else if (mpt_altitudeMode == GLB_ALTITUDEMODE_RELATIVETERRAIN)
		{
			//把xOrLon、yOrLat、zOrAlt的值转换成世界坐标赋值给position的x、y、z
			g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(position.y()),
				osg::DegreesToRadians(position.x()),position.z()+mpr_elevation,position.x(),position.y(),position.z());
			//position.z() = position.z() + mpt_globe->GetElevationAt(mpr_xOrLon,mpr_yOrLat);			 
		}
		else if (mpt_altitudeMode == GLB_ALTITUDEMODE_ONTERRAIN)
		{
			g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(position.y()),
				osg::DegreesToRadians(position.x()),mpr_elevation,position.x(),position.y(),position.z());
		}
	}
	else if (mpt_globe->GetType() == GLB_GLOBETYPE_FLAT)
	{
		if(mpt_altitudeMode == GLB_ALTITUDEMODE_ABSOLUTE)
			position = osg::Vec3d(position.x(),position.y(),position.z());
		else if (mpt_altitudeMode == GLB_ALTITUDEMODE_RELATIVETERRAIN)
		{
			position = osg::Vec3d(position.x(),position.y(),position.z()+mpr_elevation);			
		}
		else if (mpt_altitudeMode == GLB_ALTITUDEMODE_ONTERRAIN)
		{
			position = osg::Vec3d(position.x(),position.y(),mpr_elevation);
		}
	}
}

void CGlbGlobePoint::DealSelected()
{
	glbref_ptr<GlbMarkerSymbolInfo> markerInfo = dynamic_cast<GlbMarkerSymbolInfo *>(mpr_renderInfo.get());
	osg::ref_ptr<osg::StateSet> stateset = mpt_node->getOrCreateStateSet();	
	osg::ref_ptr<osg::Material> material = dynamic_cast<osg::Material *>(stateset->getAttribute(osg::StateAttribute::MATERIAL));
	if(material == NULL)
		material = new osg::Material;
	if (mpt_isSelected)
	{
		if(markerInfo && markerInfo->symType == GLB_MARKERSYMBOL_MODEL)
		{
			material->setDiffuse(osg::Material::FRONT_AND_BACK,osg::Vec4(0,0,0,1));
			material->setEmission(osg::Material::FRONT_AND_BACK,osg::Vec4(1,0,0,1));
			stateset->setAttribute(material,osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
			stateset->setMode(GL_LIGHTING,osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
			// 启用材质需要禁用纹理，这样材质才会有效 2014.11.21 马林
			//stateset->setMode(GL_TEXTURE_2D,osg::StateAttribute::OFF | osg::StateAttribute::OVERRIDE);
		}
		else
		{
			material->setDiffuse(osg::Material::FRONT_AND_BACK,osg::Vec4(1,0,0,1));
			stateset->setAttribute(material,osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
		}
	}
	else
	{
		if (material)
		{
			mpt_node->setStateSet(NULL);
			//stateset->removeAttribute(material);
			//if(!mpt_isBlink && markerInfo && markerInfo->symType == GLB_MARKERSYMBOL_MODEL)
				//stateset->setMode(GL_LIGHTING,osg::StateAttribute::OFF/* | osg::StateAttribute::OVERRIDE*/);
		}
		if(markerInfo && markerInfo->symType == GLB_MARKERSYMBOL_MODEL)
		{
			GlbMarkerModelSymbolInfo *modelInfo = dynamic_cast<GlbMarkerModelSymbolInfo *>(mpr_renderInfo.get());
			DealModelOpacity(mpt_node,modelInfo);
		}
	}
}

void CGlbGlobePoint::DealBlink()
{
	if(mpt_isBlink)
	{
		ref_ptr<PointObjectCallback> poCallback = new PointObjectCallback(this);
		mpt_node->getOrCreateStateSet()->setMode(GL_LIGHTING,osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
		mpt_node->addUpdateCallback(poCallback);
	}
	else
	{
		osg::NodeCallback *callback = mpt_node->getUpdateCallback();
		if (callback)
		{
			mpt_node->removeUpdateCallback(callback);
			//取消闪烁之后，处理对象的显示

			//需要根据renderInfo重置渲染属性20150420
			GlbMarkerSymbolInfo *markerInfo = dynamic_cast<GlbMarkerSymbolInfo *>(mpr_renderInfo.get());
			if(markerInfo)
			{
				if(markerInfo->symType == GLB_MARKERSYMBOL_2DSHAPE
					|| markerInfo->symType == GLB_MARKERSYMBOL_3DSHAPE)
				{
					DirectDraw(0);
				}
			}
			
			DealSelected();
		}
	}
}

void CGlbGlobePoint::DrawToImage( glbByte *image/*RGBA*/, glbInt32 imageW,glbInt32 imageH,CGlbExtent &ext )
{
	if(NULL == image || mpr_outline == NULL)
		return;
	/*
	*	调度控制对象隐藏时，不能渲染，直接返回
	*/
	if (!mpr_isCanDrawImage )return;

	if(mpr_renderInfo == NULL)	return;

	glbref_ptr<GlbMarkerSymbolInfo> markerInfo = dynamic_cast<GlbMarkerSymbolInfo*>(mpr_renderInfo.get());
	if (markerInfo == NULL) return;//其实不会把mpr_renderInfo转换为NULL，这句可以不写

	if(mpt_altitudeMode != GLB_ALTITUDEMODE_ONTERRAIN ||
		!(markerInfo->symType == GLB_MARKERSYMBOL_2DSHAPE ||
		markerInfo->symType == GLB_MARKERSYMBOL_LABEL))
		return;
	glbref_ptr<IGlbGeometry> _outline = NULL;
	{
		GlbScopedLock<GlbCriticalSection> lock(mpr_outline_critical);
		_outline = mpr_outline;
	}

	CGlbGlobeMarker2DShapeSymbol marker2DShapeSymbol;	
	CGlbGlobeMarkerLabelSymbol markerLabelSymbol;

	if(markerInfo->symType == GLB_MARKERSYMBOL_2DSHAPE)
		marker2DShapeSymbol.DrawToImage(this,image,imageW,imageH,ext,_outline.get(),mpr_noRotOutline.get());
	else
	{
		markerLabelSymbol.DrawToImage(this,_outline.get(),image,imageW,imageH,ext,mpr_isDirectDraw);
		if(true == mpr_isDirectDraw)
			mpr_isDirectDraw = false;
	}
}

glbBool CGlbGlobePoint::DealModelSymbol(GlbMarkerModelSymbolInfo *modelInfo,glbBool &isNeedDirectDraw )
{
	glbBool isUpdateExtent = false;
	GlbMarkerModelSymbolInfo *tempInfo = dynamic_cast<GlbMarkerModelSymbolInfo *>(mpr_renderInfo.get());
	if (tempInfo == NULL) return false;
	//tempInfo是自身现有的渲染信息
	//modelInfo是新设置的渲染信息
	if (!CGlbCompareValue::CompareValueStringEqual(tempInfo->locate,modelInfo->locate,mpt_feature.get()) ||
		tempInfo->locate->bUseField != modelInfo->locate->bUseField ||
		tempInfo->locate->field != modelInfo->locate->field)
	{//模型地址被修改.
		isNeedDirectDraw = true;
		return true;
	}

	if (!CGlbCompareValue::CompareValueIntEqual(tempInfo->opacity,modelInfo->opacity,mpt_feature.get()))
	{//透明度被修改
		DealModelOpacity(mpt_node,modelInfo);
	}

	if (!CGlbCompareValue::CompareValueDoubleEqual(tempInfo->xScale,modelInfo->xScale,mpt_feature.get()) ||
		!CGlbCompareValue::CompareValueDoubleEqual(tempInfo->yScale,modelInfo->yScale,mpt_feature.get()) ||
		!CGlbCompareValue::CompareValueDoubleEqual(tempInfo->zScale,modelInfo->zScale,mpt_feature.get()) ||
		!CGlbCompareValue::CompareValueDoubleEqual(tempInfo->xOffset,modelInfo->xOffset,mpt_feature.get()) ||
		!CGlbCompareValue::CompareValueDoubleEqual(tempInfo->yOffset,modelInfo->yOffset,mpt_feature.get()) ||
		!CGlbCompareValue::CompareValueDoubleEqual(tempInfo->zOffset,modelInfo->zOffset,mpt_feature.get()) ||
		!CGlbCompareValue::CompareValueDoubleEqual(tempInfo->yaw,modelInfo->yaw,mpt_feature.get()) ||
		!CGlbCompareValue::CompareValueDoubleEqual(tempInfo->pitch,modelInfo->pitch,mpt_feature.get()) ||
		!CGlbCompareValue::CompareValueDoubleEqual(tempInfo->roll,modelInfo->roll,mpt_feature.get()))
	{
		glbInt32 numChild = mpt_node->asSwitch()->getNumChildren();
		for (glbInt32 i = 0; i < numChild;i++)
		{
			osg::MatrixTransform *mt = dynamic_cast<osg::MatrixTransform *>(mpt_node->asSwitch()->getChild(i));
			if(mt == NULL)continue;
			osg::Vec3d position(0.0,0.0,0.0);
			if (mpt_feature.get())
			{
				glbDouble xOrLon,yOrLat,zOrAlt,M;
				zOrAlt = 0.0;

				if(GetFeaturePoint(i,&xOrLon,&yOrLat,&zOrAlt,&M) == false)
					return false;
				position = osg::Vec3d(xOrLon,yOrLat,zOrAlt);
			}
			else
			{
				position = osg::Vec3d(mpr_xOrLon,mpr_yOrLat,mpr_zOrAlt);
			}

			osg::Matrix m2;
			osg::Matrix m3;
			osg::Matrixd localToWorld;

			osg::Matrix mOffset;
			glbDouble xOffset = 0;
			if(modelInfo->xOffset)
				xOffset = modelInfo->xOffset->GetValue(mpt_feature.get());
			glbDouble yOffset = 0;
			if (modelInfo->yOffset)
				yOffset = modelInfo->yOffset->GetValue(mpt_feature.get());
			glbDouble zOffset = 0;
			if (modelInfo->zOffset)
				zOffset = modelInfo->zOffset->GetValue(mpt_feature.get());

			if(mpt_globe->GetType() == GLB_GLOBETYPE_GLOBE)
			{
				//处理offset
				double longitude = osg::DegreesToRadians(mpr_xOrLon);
				double latitude = osg::DegreesToRadians(mpr_yOrLat);

				// Compute up vector
				osg::Vec3d    up      ( cos(longitude)*cos(latitude), sin(longitude)*cos(latitude), sin(latitude));
				// Compute east vector
				osg::Vec3d    east    (-sin(longitude), cos(longitude), 0);
				// Compute north  vector = outer product up x east
				osg::Vec3d    north   = up ^ east;

				north.normalize();
				osg::Vec3 _Offset = east*xOffset + north*yOffset + up*zOffset;
				mOffset.makeTranslate(_Offset);		
				//处理位置
				ComputePosByAltitudeAndGloleType(position);
				g_ellipsoidModel->computeLocalToWorldTransformFromXYZ(position.x(),position.y(),position.z(),localToWorld);
			}
			else if(mpt_globe->GetType() == GLB_GLOBETYPE_FLAT)
			{
				//处理offset
				mOffset.makeTranslate(xOffset,yOffset,zOffset);
				//处理位置
				localToWorld.makeTranslate(position.x(),position.y(),position.z());
			}
			if(modelInfo->xScale && modelInfo->yScale && modelInfo->zScale)
				m2.makeScale(modelInfo->xScale->GetValue(mpt_feature.get()),
				modelInfo->yScale->GetValue(mpt_feature.get()),modelInfo->zScale->GetValue(mpt_feature.get()));
			if(modelInfo->pitch && modelInfo->yaw && modelInfo->roll)
			{
				glbDouble yaw = osg::DegreesToRadians(modelInfo->yaw->GetValue(mpt_feature.get()));
				glbDouble pitch = osg::DegreesToRadians(modelInfo->pitch->GetValue(mpt_feature.get()));
				glbDouble roll = osg::DegreesToRadians(modelInfo->roll->GetValue(mpt_feature.get()));
				osg::Vec3d yaw_vec(0.0,0.0,1.0);
				osg::Vec3d pitch_vec(cos(yaw),sin(yaw),0.0);
				osg::Vec3d roll_vec(-sin(yaw)*cos(pitch),cos(pitch)*cos(yaw),sin(pitch));
				m3.makeRotate(yaw,yaw_vec,pitch,pitch_vec,roll,roll_vec);
			}
			mt->setMatrix(m2 * m3 * localToWorld * mOffset);		
			// 自动放缩法线向量比例以适应模型放缩比例 2016.4.21 malin
			mt->getOrCreateStateSet()->setMode( GL_RESCALE_NORMAL, osg::StateAttribute::ON );
			isUpdateExtent = true;
		}	
	}

	if (isUpdateExtent)mpt_globe->UpdateObject(this);
	return true;
}
/*
    感觉有问题！！！
*/
glbBool CGlbGlobePoint::Deal2DShapeSymbol( GlbMarker2DShapeSymbolInfo *marker2DInfo,glbBool &isNeedDirectDraw )
{
	glbBool isUpdateExtent = false;
	GlbMarker2DShapeSymbolInfo *tempInfo = (GlbMarker2DShapeSymbolInfo *)mpr_renderInfo.get();
	if (tempInfo == NULL) return false;
	//tempInfo是自身现有的渲染信息
	//modelInfo是新设置的渲染信息
	if (  marker2DInfo->shapeInfo->shapeType != tempInfo->shapeInfo->shapeType 
		||mpt_altitudeMode == GLB_ALTITUDEMODE_ONTERRAIN
		)
	{// 重新生成
		isNeedDirectDraw = true;
		return true;
	}

	GlbCircleInfo    *circleInfo = NULL;
	GlbRectangleInfo *rectangleInfo = NULL;
	GlbEllipseInfo   *ellipseInfo = NULL;
	GlbArcInfo       *arcInfo = NULL;

	if (marker2DInfo->shapeInfo->shapeType == GLB_MARKERSHAPE_CIRCLE)
	{
		circleInfo = (GlbCircleInfo *)(marker2DInfo->shapeInfo);

		if (circleInfo)
		{
			GlbCircleInfo *tempCircleInfo = 
				(GlbCircleInfo *)(tempInfo->shapeInfo);
			if(tempCircleInfo == NULL) return false;
			if (!CGlbCompareValue::CompareValueIntEqual(tempCircleInfo->edges,circleInfo->edges,mpt_feature.get()) ||
				!CGlbCompareValue::CompareValueDoubleEqual(tempCircleInfo->radius,circleInfo->radius,mpt_feature.get()))
				isNeedDirectDraw = true;
		}
	}
	else if (marker2DInfo->shapeInfo->shapeType == GLB_MARKERSHAPE_RECTANGLE)
	{
		rectangleInfo = (GlbRectangleInfo *)(marker2DInfo->shapeInfo);

		if (rectangleInfo)
		{
			GlbRectangleInfo *tempRectangleInfo = 
				(GlbRectangleInfo *)(tempInfo->shapeInfo);
			if(tempRectangleInfo == NULL) return false;
			if (!CGlbCompareValue::CompareValueDoubleEqual(tempRectangleInfo->width,rectangleInfo->width,mpt_feature.get()) ||
				!CGlbCompareValue::CompareValueDoubleEqual(tempRectangleInfo->height,rectangleInfo->height,mpt_feature.get()))
				isNeedDirectDraw = true;
		}
	}
	else if(marker2DInfo->shapeInfo->shapeType == GLB_MARKERSHAPE_ELLIPSE)
	{
		ellipseInfo = (GlbEllipseInfo *)(marker2DInfo->shapeInfo);

		if(ellipseInfo)
		{
			GlbEllipseInfo *tempEllipseInfo = 
				(GlbEllipseInfo *)(tempInfo->shapeInfo);
			if(tempEllipseInfo == NULL) return false;
			if (!CGlbCompareValue::CompareValueIntEqual(tempEllipseInfo->edges,ellipseInfo->edges,mpt_feature.get()) ||
				!CGlbCompareValue::CompareValueDoubleEqual(tempEllipseInfo->xRadius,ellipseInfo->xRadius,mpt_feature.get()) ||
				!CGlbCompareValue::CompareValueDoubleEqual(tempEllipseInfo->yRadius,ellipseInfo->yRadius,mpt_feature.get()))
				isNeedDirectDraw = true;
		}
	}
	else if (marker2DInfo->shapeInfo->shapeType == GLB_MARKERSHAPE_ARC)
	{
		arcInfo = (GlbArcInfo *)(marker2DInfo->shapeInfo);

		if (arcInfo)
		{
			GlbArcInfo *tempArcInfo = 
				(GlbArcInfo *)(tempInfo->shapeInfo);
			if(tempArcInfo == NULL) return false;
			if (!CGlbCompareValue::CompareValueDoubleEqual(tempArcInfo->eAngle,arcInfo->eAngle,mpt_feature.get()) ||
				!CGlbCompareValue::CompareValueIntEqual(tempArcInfo->edges,arcInfo->edges,mpt_feature.get()) ||
				!CGlbCompareValue::CompareValueBoolEqual(tempArcInfo->isFan,arcInfo->isFan,mpt_feature.get()) ||
				!CGlbCompareValue::CompareValueDoubleEqual(tempArcInfo->sAngle,arcInfo->sAngle,mpt_feature.get()) ||
				!CGlbCompareValue::CompareValueDoubleEqual(tempArcInfo->xRadius,arcInfo->xRadius,mpt_feature.get()) ||
				!CGlbCompareValue::CompareValueDoubleEqual(tempArcInfo->yRadius,arcInfo->yRadius,mpt_feature.get()))
				isNeedDirectDraw = true;
		}
	}
	else
		return false;

	if(isNeedDirectDraw == true)return true;

	glbInt32 numChild = mpt_node->asSwitch()->getNumChildren();
	for (glbInt32 i = 0 ; i < numChild; i++)
	{
		osg::MatrixTransform *mt = 
			dynamic_cast<osg::MatrixTransform *>(mpt_node->asSwitch()->getChild(i));
		if(mt == NULL)
			continue;
		osg::Group *group = dynamic_cast<osg::Group *>(mt->getChild(0));
		if(NULL == group) continue;
		osg::Geode *geode = dynamic_cast<Geode *>(group->getChild(0));
		if(geode == NULL) continue;
		osg::MatrixTransform *outlineMt = dynamic_cast<osg::MatrixTransform *>(group->getChild(1));
		if(NULL == outlineMt) continue;
		osg::Node *outlineNode = outlineMt->getChild(0);
		if(NULL == outlineNode) continue;
		osg::ref_ptr<osg::Drawable> fillDrawable = geode->getDrawable(0);
		osg::ref_ptr<osg::StateSet> fillStateSet = fillDrawable->getOrCreateStateSet();
		osg::ref_ptr<osg::StateSet> outStateSet = outlineNode->getOrCreateStateSet();
		osg::Material *fillMaterial = 
			dynamic_cast<osg::Material *>(fillStateSet->getAttribute(osg::StateAttribute::MATERIAL));
		osg::Material *outMaterial = 
			dynamic_cast<osg::Material *>(outStateSet->getAttribute(osg::StateAttribute::MATERIAL));

		if (!CGlbCompareValue::CompareValueColorEqual(tempInfo->fillInfo->color,marker2DInfo->fillInfo->color,mpt_feature.get()))
		{
			if (fillMaterial)
			{
				osg::Vec4 color = GetColor(marker2DInfo->fillInfo->color->GetValue(mpt_feature.get()));
				//color.a() = tempInfo->fillInfo->opacity->GetValue(mpt_feature.get()) / 100.0;
				fillMaterial->setEmission(osg::Material::FRONT_AND_BACK,color);
				fillMaterial->setDiffuse(osg::Material::FRONT_AND_BACK,osg::Vec4(0,0,0,0));
				fillMaterial->setAlpha(osg::Material::FRONT_AND_BACK,
					tempInfo->fillInfo->opacity->GetValue(mpt_feature.get()) / 100.0);
			}
		}

		if (!CGlbCompareValue::CompareValueIntEqual(tempInfo->fillInfo->opacity,marker2DInfo->fillInfo->opacity,mpt_feature.get()))
		{
			if(fillMaterial)
			{
				glbInt32 opacity = marker2DInfo->fillInfo->opacity->GetValue(mpt_feature.get());
				fillMaterial->setAlpha(osg::Material::FRONT_AND_BACK, opacity / 100.0);
				osg::ref_ptr<osg::Depth> depth = NULL;
				if (opacity<100)
				{
					fillStateSet->setMode(GL_BLEND,osg::StateAttribute::ON);
					if (fillStateSet->getRenderingHint() != osg::StateSet::TRANSPARENT_BIN)
						fillStateSet->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
					depth = dynamic_cast<osg::Depth *>(fillStateSet->getAttribute(osg::StateAttribute::DEPTH));
					if(!depth.valid())
					{
						depth = new osg::Depth;
						depth->setWriteMask(false);
						fillStateSet->setAttributeAndModes(depth.get(), osg::StateAttribute::ON);
					}
					else
					{
						if(depth->getWriteMask())
							depth->setWriteMask(false);
					}
				}
				else
				{
					fillStateSet->setMode(GL_BLEND,osg::StateAttribute::OFF);
					fillStateSet->setRenderingHint(osg::StateSet::OPAQUE_BIN);
					depth = dynamic_cast<osg::Depth *>(fillStateSet->getAttribute(osg::StateAttribute::DEPTH));
					if(!depth.valid())
					{
						depth = new osg::Depth;
						depth->setWriteMask(true);
						fillStateSet->setAttributeAndModes(depth.get(), osg::StateAttribute::ON);
					}
					else
					{
						if(!depth->getWriteMask())
							depth->setWriteMask(true);
					}
				}
			}
		}
		
		if (!CGlbCompareValue::CompareValueStringEqual(tempInfo->fillInfo->textureData,marker2DInfo->fillInfo->textureData,mpt_feature.get()))
		{
			osg::ref_ptr<osg::Texture2D> texture = 
				dynamic_cast<osg::Texture2D*>(fillStateSet->getTextureAttribute(0,osg::StateAttribute::TEXTURE));
			if(!texture){
				texture = new osg::Texture2D; fillStateSet->setTextureAttribute(0,texture);
			}
			osg::ref_ptr<osg::TexMat> texMat = 
				dynamic_cast<osg::TexMat*>(fillStateSet->getTextureAttribute(0,osg::StateAttribute::TEXMAT));
			if(texMat == NULL)
				texMat = new osg::TexMat;
			CGlbWString filepath(marker2DInfo->fillInfo->textureData->GetValue(mpt_feature.get()));
			glbInt32 index = filepath.find_first_of(L'.');
			if(index == 0)
			{
				CGlbWString execDir = CGlbPath::GetExecDir();
				filepath = execDir + filepath.substr(1,filepath.size());
			}
			osg::Image *image = osgDB::readImageFile(filepath.ToString());
			if (image)
			{
				texture->setImage(image);
				osg::ImageStream* imagestream = dynamic_cast<osg::ImageStream*>(image);
				if (imagestream) 
					imagestream->play();
			}

			osg::Matrix mScale;
			osg::Matrix mRotate;
			double tilingU = 1.0;
			double tilingV = 1.0;
			if (tempInfo->fillInfo->texRepeatMode->GetValue(mpt_feature.get())
				== GLB_TEXTUREREPEAT_TIMES)
			{
				tilingU = tempInfo->fillInfo->tilingU->GetValue(mpt_feature.get());
				tilingV = tempInfo->fillInfo->tilingV->GetValue(mpt_feature.get());
			}
			else if (tempInfo->fillInfo->texRepeatMode->GetValue(mpt_feature.get())
				== GLB_TEXTUREREPEAT_SIZE)
			{
				if (circleInfo)
				{
					tilingU = 2 * circleInfo->radius->GetValue(mpt_feature.get()) / texture->getTextureWidth();
					tilingV = 2 * circleInfo->radius->GetValue(mpt_feature.get()) / texture->getTextureHeight();
				}
				else if (rectangleInfo)
				{
					tilingU = 2 * rectangleInfo->width->GetValue(mpt_feature.get()) / texture->getTextureWidth();
					tilingV = 2 * rectangleInfo->height->GetValue(mpt_feature.get()) / texture->getTextureHeight();
				}
				else if (ellipseInfo)
				{
					tilingU = 2 * ellipseInfo->xRadius->GetValue(mpt_feature.get()) / texture->getTextureWidth();
					tilingV = 2 * ellipseInfo->yRadius->GetValue(mpt_feature.get()) / texture->getTextureHeight();
				}
				else if (arcInfo)
				{
					tilingU = 2 * arcInfo->xRadius->GetValue(mpt_feature.get()) / texture->getTextureWidth();
					tilingV = 2 * arcInfo->yRadius->GetValue(mpt_feature.get()) / texture->getTextureHeight();
				}
			}

			double textureRotation = osg::DegreesToRadians(tempInfo->fillInfo->textureRotation->GetValue(mpt_feature.get()));
			double bWidth = fillDrawable->getBound().xMax() - fillDrawable->getBound().xMin();
			double bHeight = fillDrawable->getBound().yMax() - fillDrawable->getBound().yMin();
			glbDouble ra = sqrt(bWidth * bWidth + bHeight * bHeight);
			glbDouble ang = atan2(bHeight,bWidth);
			glbDouble xm = ra*max(abs(cos(ang-textureRotation)),abs(cos(ang+textureRotation)));
			glbDouble ym = ra*max(abs(sin(ang-textureRotation)),abs(sin(ang+textureRotation)));
			double scalem = ra * ra/(xm*xm+ym*ym);
			mScale.makeScale(scalem * tilingU,scalem * tilingV,1.0);
			//mScale.makeScale(tilingU,tilingV,0.0);
			mRotate.makeRotate(textureRotation,osg::Vec3d(0,0,1));
			//texMat->setMatrix(mScale * mRotate);
			texMat->setMatrix(osg::Matrix::translate(-0.5,-0.5,0) * mRotate * mScale * osg::Matrix::translate(0.5 * tilingU ,0.5 * tilingV,0));
			//fillStateSet->setTextureAttributeAndModes(0,texture,osg::StateAttribute::ON);
			//fillStateSet->setTextureAttributeAndModes(0,texMat,osg::StateAttribute::ON);
		}		

		if (!CGlbCompareValue::CompareValueTexRepeatEnumEqual(tempInfo->fillInfo->texRepeatMode,marker2DInfo->fillInfo->texRepeatMode,mpt_feature.get()) ||
			!CGlbCompareValue::CompareValueIntEqual(tempInfo->fillInfo->tilingU,marker2DInfo->fillInfo->tilingU,mpt_feature.get()) ||
			!CGlbCompareValue::CompareValueIntEqual(tempInfo->fillInfo->tilingV,marker2DInfo->fillInfo->tilingV,mpt_feature.get()) ||
			!CGlbCompareValue::CompareValueDoubleEqual(tempInfo->fillInfo->textureRotation,marker2DInfo->fillInfo->textureRotation,mpt_feature.get()))
		{
			osg::ref_ptr<osg::Texture2D> texture = 
				dynamic_cast<osg::Texture2D*>(fillStateSet->getTextureAttribute(0,osg::StateAttribute::TEXTURE));
			if(texture == NULL)
			{
				texture = new osg::Texture2D;
				texture->setWrap(osg::Texture2D::WRAP_S,osg::Texture2D::REPEAT);
				texture->setWrap(osg::Texture2D::WRAP_T,osg::Texture2D::REPEAT);
			}
			osg::ref_ptr<osg::TexMat> texMat = 
				dynamic_cast<osg::TexMat*>(fillStateSet->getTextureAttribute(0,osg::StateAttribute::TEXMAT));
			if(texMat == NULL)
				texMat = new osg::TexMat;
			osg::Matrix mScale;
			osg::Matrix mRotate;
			double tilingU = 1.0;
			double tilingV = 1.0;

			if (marker2DInfo->fillInfo->texRepeatMode->GetValue(mpt_feature.get())
				== GLB_TEXTUREREPEAT_TIMES)
			{
				tilingU = marker2DInfo->fillInfo->tilingU->GetValue(mpt_feature.get());
				tilingV = marker2DInfo->fillInfo->tilingV->GetValue(mpt_feature.get());
			}
			else if (marker2DInfo->fillInfo->texRepeatMode->GetValue(mpt_feature.get())
				== GLB_TEXTUREREPEAT_SIZE)
			{
				if (circleInfo)
				{
					tilingU = 2 * circleInfo->radius->GetValue(mpt_feature.get()) / texture->getTextureWidth();
					tilingV = 2 * circleInfo->radius->GetValue(mpt_feature.get()) / texture->getTextureHeight();
				}
				else if (rectangleInfo)
				{
					tilingU = 2 * rectangleInfo->width->GetValue(mpt_feature.get()) / texture->getTextureWidth();
					tilingV = 2 * rectangleInfo->height->GetValue(mpt_feature.get()) / texture->getTextureHeight();
				}
				else if (ellipseInfo)
				{
					tilingU = 2 * ellipseInfo->xRadius->GetValue(mpt_feature.get()) / texture->getTextureWidth();
					tilingV = 2 * ellipseInfo->yRadius->GetValue(mpt_feature.get()) / texture->getTextureHeight();
				}
				else if (arcInfo)
				{
					tilingU = 2 * arcInfo->xRadius->GetValue(mpt_feature.get()) / texture->getTextureWidth();
					tilingV = 2 * arcInfo->yRadius->GetValue(mpt_feature.get()) / texture->getTextureHeight();
				}
			}
			double textureRotation = osg::DegreesToRadians(marker2DInfo->fillInfo->textureRotation->GetValue(mpt_feature.get()));
			double bWidth = fillDrawable->getBound().xMax() - fillDrawable->getBound().xMin();
			double bHeight = fillDrawable->getBound().yMax() - fillDrawable->getBound().yMin();
			glbDouble ra = sqrt(bWidth * bWidth + bHeight * bHeight);
			glbDouble ang = atan2(bHeight,bWidth);
			glbDouble xm = ra*max(abs(cos(ang-textureRotation)),abs(cos(ang+textureRotation)));
			glbDouble ym = ra*max(abs(sin(ang-textureRotation)),abs(sin(ang+textureRotation)));
			double scalem = ra * ra/(xm*xm+ym*ym);
			mScale.makeScale(scalem * tilingU,scalem * tilingV,1.0);
			//mScale.makeScale(tilingU,tilingV,0.0);
			mRotate.makeRotate(textureRotation,osg::Vec3d(0,0,1));
			texMat->setMatrix(osg::Matrix::translate(-0.5,-0.5,0) * mRotate  *mScale  * osg::Matrix::translate(0.5 * tilingU ,0.5 * tilingV,0));
			//fillStateSet->setTextureAttributeAndModes(0,texture,osg::StateAttribute::ON);
			//fillStateSet->setTextureAttributeAndModes(0,texMat,osg::StateAttribute::ON);
		}
		
		if (!CGlbCompareValue::CompareValueColorEqual(tempInfo->fillInfo->outlineInfo->lineColor,marker2DInfo->fillInfo->outlineInfo->lineColor,mpt_feature.get()))
		{
			if(outMaterial)
			{
				osg::Vec4 color = GetColor(marker2DInfo->fillInfo->outlineInfo->lineColor->GetValue(mpt_feature.get()));
				//color.a() = tempInfo->fillInfo->outlineInfo->lineOpacity->GetValue(mpt_feature.get()) / 100.0;
				outMaterial->setEmission(osg::Material::FRONT_AND_BACK,color);
				outMaterial->setDiffuse(osg::Material::FRONT_AND_BACK,osg::Vec4(0,0,0,0));
				outMaterial->setAlpha(osg::Material::FRONT_AND_BACK,
					tempInfo->fillInfo->outlineInfo->lineOpacity->GetValue(mpt_feature.get()) / 100.0);
			}			
		}

		if (!CGlbCompareValue::CompareValueIntEqual(tempInfo->fillInfo->outlineInfo->lineOpacity,
			marker2DInfo->fillInfo->outlineInfo->lineOpacity,mpt_feature.get()))
		{
			if(outMaterial)
				outMaterial->setAlpha(osg::Material::FRONT_AND_BACK,
				marker2DInfo->fillInfo->outlineInfo->lineOpacity->GetValue(mpt_feature.get()) / 100.0);			
		}

		if (!CGlbCompareValue::CompareValueLinePatternEnumEqual(tempInfo->fillInfo->outlineInfo->linePattern,
			marker2DInfo->fillInfo->outlineInfo->linePattern,mpt_feature.get()))
		{
			if (tempInfo->fillInfo->outlineInfo->linePattern->GetValue(mpt_feature.get())
					== GLB_LINE_DOTTED)
			{
				osg::LineStipple *lineStipple = 
					dynamic_cast<osg::LineStipple *>(outStateSet->getAttribute(osg::StateAttribute::LINESTIPPLE));
				outStateSet->setAttributeAndModes(lineStipple,osg::StateAttribute::OFF);
			}
			else
			{
				osg::LineStipple *lineStipple = 
					dynamic_cast<osg::LineStipple *>(outStateSet->getAttribute(osg::StateAttribute::LINESTIPPLE));
				if(lineStipple)
					outStateSet->setAttributeAndModes(lineStipple,osg::StateAttribute::ON);
				else
				{
					ref_ptr<osg::LineStipple> lineStipple = new osg::LineStipple;
					lineStipple->setFactor(1);
					lineStipple->setPattern(0x1C47);
					outStateSet->setAttributeAndModes(lineStipple,osg::StateAttribute::ON);
				}
			}
		}
		
		if (!CGlbCompareValue::CompareValueDoubleEqual(tempInfo->fillInfo->outlineInfo->lineWidth,
			marker2DInfo->fillInfo->outlineInfo->lineWidth,mpt_feature.get()))
		{
			osg::LineWidth *lineWidth = 
					dynamic_cast<osg::LineWidth *>(outStateSet->getAttribute(osg::StateAttribute::LINEWIDTH));
			if(lineWidth)
				lineWidth->setWidth(marker2DInfo->fillInfo->outlineInfo->lineWidth->GetValue(mpt_feature.get()));			
		}

		if (!CGlbCompareValue::CompareValueDoubleEqual(tempInfo->xScale,marker2DInfo->xScale,mpt_feature.get()) ||
			!CGlbCompareValue::CompareValueDoubleEqual(tempInfo->yScale,marker2DInfo->yScale,mpt_feature.get()) ||
			!CGlbCompareValue::CompareValueDoubleEqual(tempInfo->zScale,marker2DInfo->zScale,mpt_feature.get()) ||
			!CGlbCompareValue::CompareValueDoubleEqual(tempInfo->yaw,marker2DInfo->yaw,mpt_feature.get()) ||
			!CGlbCompareValue::CompareValueDoubleEqual(tempInfo->xOffset,marker2DInfo->xOffset,mpt_feature.get()) ||
			!CGlbCompareValue::CompareValueDoubleEqual(tempInfo->yOffset,marker2DInfo->yOffset,mpt_feature.get()) ||
			!CGlbCompareValue::CompareValueDoubleEqual(tempInfo->zOffset,marker2DInfo->zOffset,mpt_feature.get()))
		{//offset值被修改
			glbInt32 numChild = mpt_node->asSwitch()->getNumChildren();
			for (glbInt32 i = 0; i < numChild;i++)
			{
				osg::MatrixTransform *mt = dynamic_cast<osg::MatrixTransform *>(mpt_node->asSwitch()->getChild(i));
				if(mt == NULL)continue;
				osg::Vec3d position(0.0,0.0,0.0);
				if (mpt_feature.get())
				{
					glbDouble xOrLon,yOrLat,zOrAlt,M;
					zOrAlt = 0.0;

					if(GetFeaturePoint(i,&xOrLon,&yOrLat,&zOrAlt,&M) == false)
						return false;
					position = osg::Vec3d(xOrLon,yOrLat,zOrAlt);
				}
				else
				{
					position = osg::Vec3d(mpr_xOrLon,mpr_yOrLat,mpr_zOrAlt);
				}

				osg::Matrix m2;
				osg::Matrix m3;
				osg::Matrixd localToWorld;

				osg::Matrix mOffset;
				glbDouble xOffset = 0;
				if(marker2DInfo->xOffset)
					xOffset = marker2DInfo->xOffset->GetValue(mpt_feature.get());
				glbDouble yOffset = 0;
				if (marker2DInfo->yOffset)
					yOffset = marker2DInfo->yOffset->GetValue(mpt_feature.get());
				glbDouble zOffset = 0;
				if (marker2DInfo->zOffset)
					zOffset = marker2DInfo->zOffset->GetValue(mpt_feature.get());

				if(mpt_globe->GetType() == GLB_GLOBETYPE_GLOBE)
				{
					//
					double longitude = osg::DegreesToRadians(mpr_xOrLon);
					double latitude = osg::DegreesToRadians(mpr_yOrLat);

					// Compute up vector
					osg::Vec3d    up      ( cos(longitude)*cos(latitude), sin(longitude)*cos(latitude), sin(latitude));
					// Compute east vector
					osg::Vec3d    east    (-sin(longitude), cos(longitude), 0);
					// Compute north  vector = outer product up x east
					osg::Vec3d    north   = up ^ east;

					north.normalize();
					osg::Vec3 _Offset = east*xOffset + north*yOffset + up*zOffset;
					mOffset.makeTranslate(_Offset);		
					//
					ComputePosByAltitudeAndGloleType(position);
					g_ellipsoidModel->computeLocalToWorldTransformFromXYZ(position.x(),position.y(),position.z(),localToWorld);
				}
				else if(mpt_globe->GetType() == GLB_GLOBETYPE_FLAT)
				{
					mOffset.makeTranslate(xOffset,yOffset,zOffset);
					localToWorld.makeTranslate(position.x(),position.y(),position.z());
				}
				if(marker2DInfo->xScale && marker2DInfo->yScale && marker2DInfo->zScale)
					m2.makeScale(marker2DInfo->xScale->GetValue(mpt_feature.get()),
					marker2DInfo->yScale->GetValue(mpt_feature.get()),marker2DInfo->zScale->GetValue(mpt_feature.get()));
				if(marker2DInfo->yaw)
				{
					glbDouble yaw = osg::DegreesToRadians(marker2DInfo->yaw->GetValue(mpt_feature.get()));
					glbDouble pitch = 0.0;
					glbDouble roll = 0.0;
					osg::Vec3d yaw_vec(0.0,0.0,1.0);
					osg::Vec3d pitch_vec(cos(yaw),sin(yaw),0.0);
					osg::Vec3d roll_vec(-sin(yaw)*cos(pitch),cos(pitch)*cos(yaw),sin(pitch));
					m3.makeRotate(yaw,yaw_vec,pitch,pitch_vec,roll,roll_vec);
				}
				mt->setMatrix(m2 * m3 * localToWorld * mOffset);		
				// 自动放缩法线向量比例以适应模型放缩比例 2016.4.21 malin
				mt->getOrCreateStateSet()->setMode( GL_RESCALE_NORMAL, osg::StateAttribute::ON );
				isUpdateExtent = true;
			}		
		}
	}

	if (isUpdateExtent)mpt_globe->UpdateObject(this);
	
	return true;
}

glbBool CGlbGlobePoint::Deal3DShapeSymbol( GlbMarker3DShapeSymbolInfo *marker3DInfo,glbBool &isNeedDirectDraw )
{
	glbBool isUpdateExtent = false;
	GlbMarker3DShapeSymbolInfo *tempInfo = (GlbMarker3DShapeSymbolInfo *)(mpr_renderInfo.get());
	if (tempInfo == NULL) return false;
	//tempInfo是自身现有的渲染信息
	if (marker3DInfo->shapeInfo->shapeType != tempInfo->shapeInfo->shapeType)
	{// 重新生成
		isNeedDirectDraw = true;
		return true;
	}
	//modelInfo是新设置的渲染信息

	GlbSphereInfo *sphereInfo = NULL;
	GlbBoxInfo *boxInfo = NULL;
	GlbConeInfo *coneInfo = NULL;
	GlbCylinderInfo *cylinderInfo = NULL;
	GlbPyramidInfo *pyramidInfo = NULL;
	GlbPieInfo *pieInfo = NULL;

	if (marker3DInfo->shapeInfo->shapeType == GLB_MARKERSHAPE_SPHERE)
	{
		sphereInfo = (GlbSphereInfo *)(marker3DInfo->shapeInfo);

		if (sphereInfo)
		{
			GlbSphereInfo *tempSphereInfo = 
				(GlbSphereInfo *)(tempInfo->shapeInfo);
			if(tempSphereInfo == NULL) return false;
			if (!CGlbCompareValue::CompareValueDoubleEqual(tempSphereInfo->radius,sphereInfo->radius,mpt_feature.get()))
				isNeedDirectDraw = true;
		}
	}
	else if (marker3DInfo->shapeInfo->shapeType == GLB_MARKERSHAPE_BOX)
	{
		boxInfo = (GlbBoxInfo *)(marker3DInfo->shapeInfo);

		if (boxInfo)
		{
			GlbBoxInfo *tempBoxInfo = 
				(GlbBoxInfo *)(tempInfo->shapeInfo);
			if(tempBoxInfo == NULL) return false;
			if (!CGlbCompareValue::CompareValueDoubleEqual(tempBoxInfo->length,boxInfo->length,mpt_feature.get()) ||
				!CGlbCompareValue::CompareValueDoubleEqual(tempBoxInfo->width,boxInfo->width,mpt_feature.get()) ||
				!CGlbCompareValue::CompareValueDoubleEqual(tempBoxInfo->height,boxInfo->height,mpt_feature.get()))
				isNeedDirectDraw = true;
		}
	}
	else if (marker3DInfo->shapeInfo->shapeType == GLB_MARKERSHAPE_CONE)
	{
		coneInfo = (GlbConeInfo *)(marker3DInfo->shapeInfo);

		if(coneInfo)
		{
			GlbConeInfo *tempConeInfo = 
				(GlbConeInfo *)(tempInfo->shapeInfo);
			if(tempConeInfo == NULL) return false;
			if (!CGlbCompareValue::CompareValueIntEqual(tempConeInfo->edges,coneInfo->edges,mpt_feature.get()) ||
				!CGlbCompareValue::CompareValueDoubleEqual(tempConeInfo->radius,coneInfo->radius,mpt_feature.get()) ||
				!CGlbCompareValue::CompareValueDoubleEqual(tempConeInfo->height,coneInfo->height,mpt_feature.get()))
				isNeedDirectDraw = true;
		}
	}
	else if (marker3DInfo->shapeInfo->shapeType == GLB_MARKERSHAPE_CYLINDER)
	{
		cylinderInfo = (GlbCylinderInfo *)(marker3DInfo->shapeInfo);

		if (cylinderInfo)
		{
			GlbCylinderInfo *tempCylinderInfo = 
				(GlbCylinderInfo *)(tempInfo->shapeInfo);
			if(tempCylinderInfo == NULL) return false;
			if (!CGlbCompareValue::CompareValueIntEqual(tempCylinderInfo->edges,cylinderInfo->edges,mpt_feature.get()) ||
				!CGlbCompareValue::CompareValueDoubleEqual(tempCylinderInfo->radius,cylinderInfo->radius,mpt_feature.get()) ||
				!CGlbCompareValue::CompareValueDoubleEqual(tempCylinderInfo->height,cylinderInfo->height,mpt_feature.get()))
				isNeedDirectDraw = true;
		}
	}
	else if(marker3DInfo->shapeInfo->shapeType == GLB_MARKERSHAPE_PYRAMID)
	{
		pyramidInfo = (GlbPyramidInfo *)(marker3DInfo->shapeInfo);

		if (pyramidInfo)
		{
			GlbPyramidInfo *tempPyramidInfo = 
				(GlbPyramidInfo *)(tempInfo->shapeInfo);
			if(tempPyramidInfo == NULL) return false;
			if(!CGlbCompareValue::CompareValueDoubleEqual(tempPyramidInfo->length,pyramidInfo->length,mpt_feature.get()) ||
				!CGlbCompareValue::CompareValueDoubleEqual(tempPyramidInfo->width,pyramidInfo->width,mpt_feature.get()) ||
				!CGlbCompareValue::CompareValueDoubleEqual(tempPyramidInfo->height,pyramidInfo->height,mpt_feature.get()))
				isNeedDirectDraw = true;
		}
	}
	else if (marker3DInfo->shapeInfo->shapeType == GLB_MARKERSHAPE_PIE)
	{
		pieInfo = (GlbPieInfo *)(marker3DInfo->shapeInfo);

		if (pieInfo)
		{
			GlbPieInfo *tempPieInfo = 
				(GlbPieInfo *)(tempInfo->shapeInfo);
			if(tempPieInfo == NULL) return false;
			if (!CGlbCompareValue::CompareValueIntEqual(tempPieInfo->edges,pieInfo->edges,mpt_feature.get()) ||
				!CGlbCompareValue::CompareValueDoubleEqual(tempPieInfo->eAngle,pieInfo->eAngle,mpt_feature.get()) ||
				!CGlbCompareValue::CompareValueDoubleEqual(tempPieInfo->height,pieInfo->height,mpt_feature.get()) ||
				!CGlbCompareValue::CompareValueDoubleEqual(tempPieInfo->radius,pieInfo->radius,mpt_feature.get()) ||
				!CGlbCompareValue::CompareValueDoubleEqual(tempPieInfo->sAngle,pieInfo->sAngle,mpt_feature.get()))
				isNeedDirectDraw = true;
		}
	}
	else
		return false;

	if(true == isNeedDirectDraw)
		return true;

	glbInt32 numChild = mpt_node->asSwitch()->getNumChildren();
	for(glbInt32 i = 0; i < numChild; i++)
	{
		osg::MatrixTransform *mt = 
			dynamic_cast<osg::MatrixTransform *>(mpt_node->asSwitch()->getChild(i));
		if(mt == NULL)
			continue;
		osg::Geode *geode = dynamic_cast<Geode *>(mt->getChild(0));
		if(geode == NULL) continue;
		osg::ref_ptr<osg::Drawable> fillDrawable = geode->getDrawable(0);
		osg::ref_ptr<osg::Drawable> outDrawable = geode->getDrawable(1);
		osg::ref_ptr<osg::StateSet> fillStateSet = fillDrawable->getOrCreateStateSet();
		osg::ref_ptr<osg::StateSet> outStateSet = outDrawable->getOrCreateStateSet();
		osg::Material *fillMaterial = 
			dynamic_cast<osg::Material *>(fillStateSet->getAttribute(osg::StateAttribute::MATERIAL));
		osg::Material *outMaterial = 
			dynamic_cast<osg::Material *>(outStateSet->getAttribute(osg::StateAttribute::MATERIAL));

		if (!CGlbCompareValue::CompareValueColorEqual(tempInfo->fillInfo->color,marker3DInfo->fillInfo->color,mpt_feature.get()))
		{		
			if (fillMaterial)
			{
				osg::Vec4 color = GetColor(marker3DInfo->fillInfo->color->GetValue(mpt_feature.get()));
				fillMaterial->setDiffuse(osg::Material::FRONT_AND_BACK,color);
				fillMaterial->setAlpha(osg::Material::FRONT_AND_BACK,
					tempInfo->fillInfo->opacity->GetValue(mpt_feature.get()) / 100.0);
				//下面的效果是一致的
				//fillMaterial->setDiffuse(osg::Material::FRONT_AND_BACK,
				//	osg::Vec4(0,0,0,tempInfo->fillInfo->opacity->GetValue(mpt_feature.get()) / 100.0));
			}			
		}
		if (!CGlbCompareValue::CompareValueIntEqual(tempInfo->fillInfo->opacity,marker3DInfo->fillInfo->opacity,mpt_feature.get()))
		{			
			if(fillMaterial)
			{
				glbInt32 opacity = marker3DInfo->fillInfo->opacity->GetValue(mpt_feature.get());
				fillMaterial->setAlpha(osg::Material::FRONT_AND_BACK,
					opacity / 100.0);
				if (opacity<100)
				{
					fillStateSet->setMode(GL_BLEND,osg::StateAttribute::ON);
					if (fillStateSet->getRenderingHint() != osg::StateSet::TRANSPARENT_BIN)
						fillStateSet->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
				}
				else
				{
					fillStateSet->setMode(GL_BLEND,osg::StateAttribute::OFF);
					fillStateSet->setRenderingHint(osg::StateSet::OPAQUE_BIN);
				}
			}
		}
		if (!CGlbCompareValue::CompareValueStringEqual(tempInfo->fillInfo->textureData,marker3DInfo->fillInfo->textureData,mpt_feature.get()))
		{			
			osg::ref_ptr<osg::Texture2D> texture = 
				dynamic_cast<osg::Texture2D*>(fillStateSet->getTextureAttribute(0,osg::StateAttribute::TEXTURE));
			if(texture == NULL)
				texture = new osg::Texture2D;

			osg::ref_ptr<osg::TexMat> texMat = 
				dynamic_cast<osg::TexMat*>(fillStateSet->getTextureAttribute(0,osg::StateAttribute::TEXMAT));
			if(texMat == NULL)
				texMat = new osg::TexMat;

			CGlbWString filepath(marker3DInfo->fillInfo->textureData->GetValue(mpt_feature.get()));	
			glbInt32 index = filepath.find_first_of(L'.');
			if(index == 0)
			{
				CGlbWString execDir = CGlbPath::GetExecDir();
				filepath = execDir + filepath.substr(1,filepath.size());
			}
			osg::Image *image = 
				osgDB::readImageFile(filepath.ToString());
			if (image)
			{
				texture->setImage(image);
				osg::ImageStream* imagestream = dynamic_cast<osg::ImageStream*>(image);
				if (imagestream) 
					imagestream->play();
			}

			osg::Matrix mScale;
			osg::Matrix mRotate;
			double tilingU = 1.0;
			double tilingV = 1.0;
			if (tempInfo->fillInfo->texRepeatMode->GetValue(mpt_feature.get())
				== GLB_TEXTUREREPEAT_TIMES)
			{
				tilingU = tempInfo->fillInfo->tilingU->GetValue(mpt_feature.get());
				tilingV = tempInfo->fillInfo->tilingV->GetValue(mpt_feature.get());
			}
			else if (tempInfo->fillInfo->texRepeatMode->GetValue(mpt_feature.get())
				== GLB_TEXTUREREPEAT_SIZE)
			{
				if (sphereInfo)
				{
					tilingU = 2 * sphereInfo->radius->GetValue(mpt_feature.get()) / texture->getTextureWidth();
					tilingV = 2 * sphereInfo->radius->GetValue(mpt_feature.get()) / texture->getTextureHeight();
				}
				else if (boxInfo)
				{
					tilingU = boxInfo->width->GetValue(mpt_feature.get()) / texture->getTextureWidth();
					tilingV = boxInfo->length->GetValue(mpt_feature.get()) / texture->getTextureHeight();
				}
				else if (coneInfo)
				{
					tilingU = 2 * coneInfo->radius->GetValue(mpt_feature.get()) / texture->getTextureWidth();
					tilingV = 2 * coneInfo->radius->GetValue(mpt_feature.get()) / texture->getTextureHeight();
				}
				else if (cylinderInfo)
				{
					tilingU = 2 * cylinderInfo->radius->GetValue(mpt_feature.get()) / texture->getTextureWidth();
					tilingV = 2 * cylinderInfo->radius->GetValue(mpt_feature.get()) / texture->getTextureHeight();
				}
				else if (pyramidInfo)
				{
					tilingU = pyramidInfo->width->GetValue(mpt_feature.get()) / texture->getTextureWidth();
					tilingV = pyramidInfo->length->GetValue(mpt_feature.get()) / texture->getTextureHeight();
				}
				else if (pieInfo)
				{
					tilingU = 2 * pieInfo->radius->GetValue(mpt_feature.get()) / texture->getTextureWidth();
					tilingV = 2 * pieInfo->radius->GetValue(mpt_feature.get()) / texture->getTextureHeight();
				}
			}

			glbDouble textureRotation = osg::DegreesToRadians(tempInfo->fillInfo->textureRotation->GetValue(mpt_feature.get()));
			double bWidth = fillDrawable->getBound().xMax() - fillDrawable->getBound().xMin();
			double bHeight = fillDrawable->getBound().yMax() - fillDrawable->getBound().yMin();
			glbDouble ra = sqrt(bWidth * bWidth + bHeight * bHeight);
			glbDouble ang = atan2(bHeight,bWidth);
			glbDouble xm = ra * max(abs(cos(ang - textureRotation)),abs(cos(ang + textureRotation)));
			glbDouble ym = ra * max(abs(sin(ang-textureRotation)),abs(sin(ang + textureRotation)));
			glbDouble scalem = ra * ra / (xm * xm + ym * ym);
			mScale.makeScale(scalem * tilingU,scalem * tilingV,1.0);
			mRotate.makeRotate(textureRotation,osg::Vec3d(0,0,1));

			//texMat->setMatrix(mScale * mRotate);
			texMat->setMatrix(osg::Matrix::translate(-0.5,-0.5,0) * mRotate * mScale * 
				osg::Matrix::translate(0.5 * tilingU , 0.5 * tilingV , 0));
			//fillStateSet->setTextureAttributeAndModes(0,texture,osg::StateAttribute::ON);
			//fillStateSet->setTextureAttributeAndModes(0,texMat,osg::StateAttribute::ON);
			
		}
		if (!CGlbCompareValue::CompareValueTexRepeatEnumEqual(tempInfo->fillInfo->texRepeatMode,marker3DInfo->fillInfo->texRepeatMode,mpt_feature.get()) ||
			!CGlbCompareValue::CompareValueIntEqual(tempInfo->fillInfo->tilingU,marker3DInfo->fillInfo->tilingU,mpt_feature.get()) ||
			!CGlbCompareValue::CompareValueIntEqual(tempInfo->fillInfo->tilingV,marker3DInfo->fillInfo->tilingV,mpt_feature.get()) ||
			!CGlbCompareValue::CompareValueDoubleEqual(tempInfo->fillInfo->textureRotation,marker3DInfo->fillInfo->textureRotation,mpt_feature.get()))
		{			
			osg::Texture2D *texture = 
				dynamic_cast<osg::Texture2D*>(fillStateSet->getTextureAttribute(0,osg::StateAttribute::TEXTURE));

			osg::TexMat *texMat = 
				dynamic_cast<osg::TexMat*>(fillStateSet->getTextureAttribute(0,osg::StateAttribute::TEXMAT));

			if (texture && texMat)
			{
				osg::Matrix mScale;
				osg::Matrix mRotate;
				double tilingU = 1.0;
				double tilingV = 1.0;

				if (marker3DInfo->fillInfo->texRepeatMode->GetValue(mpt_feature.get())
					== GLB_TEXTUREREPEAT_TIMES)
				{
					tilingU = marker3DInfo->fillInfo->tilingU->GetValue(mpt_feature.get());
					tilingV = marker3DInfo->fillInfo->tilingV->GetValue(mpt_feature.get());
				}
				else if (marker3DInfo->fillInfo->texRepeatMode->GetValue(mpt_feature.get())
					== GLB_TEXTUREREPEAT_SIZE)
				{
					if (sphereInfo)
					{
						tilingU = 2 * sphereInfo->radius->GetValue(mpt_feature.get()) / texture->getTextureWidth();
						tilingV = 2 * sphereInfo->radius->GetValue(mpt_feature.get()) / texture->getTextureHeight();
					}
					else if (boxInfo)
					{
						tilingU = boxInfo->width->GetValue(mpt_feature.get()) / texture->getTextureWidth();
						tilingV = boxInfo->length->GetValue(mpt_feature.get()) / texture->getTextureHeight();
					}
					else if (coneInfo)
					{
						tilingU = 2 * coneInfo->radius->GetValue(mpt_feature.get()) / texture->getTextureWidth();
						tilingV = 2 * coneInfo->radius->GetValue(mpt_feature.get()) / texture->getTextureHeight();
					}
					else if (cylinderInfo)
					{
						tilingU = 2 * cylinderInfo->radius->GetValue(mpt_feature.get()) / texture->getTextureWidth();
						tilingV = 2 * cylinderInfo->radius->GetValue(mpt_feature.get()) / texture->getTextureHeight();
					}
					else if (pyramidInfo)
					{
						tilingU = pyramidInfo->width->GetValue(mpt_feature.get()) / texture->getTextureWidth();
						tilingV = pyramidInfo->length->GetValue(mpt_feature.get()) / texture->getTextureHeight();
					}
					else if (pieInfo)
					{
						tilingU = 2 * pieInfo->radius->GetValue(mpt_feature.get()) / texture->getTextureWidth();
						tilingV = 2 * pieInfo->radius->GetValue(mpt_feature.get()) / texture->getTextureHeight();
					}
				}
				//mScale.makeScale(tilingU,tilingV,0.0);

				glbDouble textureRotation = osg::DegreesToRadians(marker3DInfo->fillInfo->textureRotation->GetValue(mpt_feature.get()));
				double bWidth = fillDrawable->getBound().xMax() - fillDrawable->getBound().xMin();
				double bHeight = fillDrawable->getBound().yMax() - fillDrawable->getBound().yMin();
				glbDouble ra = sqrt(bWidth * bWidth + bHeight * bHeight);
				glbDouble ang = atan2(bHeight,bWidth);
				glbDouble xm = ra * max(abs(cos(ang - textureRotation)),abs(cos(ang + textureRotation)));
				glbDouble ym = ra * max(abs(sin(ang-textureRotation)),abs(sin(ang + textureRotation)));
				glbDouble scalem = ra * ra / (xm * xm + ym * ym);
				mScale.makeScale(scalem * tilingU,scalem * tilingV,1.0);
				mRotate.makeRotate(textureRotation,osg::Vec3d(0,0,1));
				texMat->setMatrix(osg::Matrix::translate(-0.5,-0.5,0) * mRotate * mScale * osg::Matrix::translate(0.5 * tilingU ,0.5 * tilingV,0));
			}
			
		}
		if (!CGlbCompareValue::CompareValueColorEqual(tempInfo->fillInfo->outlineInfo->lineColor,marker3DInfo->fillInfo->outlineInfo->lineColor,mpt_feature.get()))
		{			
			osg::Vec4 color = GetColor(marker3DInfo->fillInfo->outlineInfo->lineColor->GetValue(mpt_feature.get()));
			outMaterial->setDiffuse(osg::Material::FRONT_AND_BACK,color);
			outMaterial->setAlpha(osg::Material::FRONT_AND_BACK,
				tempInfo->fillInfo->outlineInfo->lineOpacity->GetValue(mpt_feature.get()) / 100.0);
			//下面的效果是一致的
			//outMaterial->setDiffuse(osg::Material::FRONT_AND_BACK,
			//	osg::Vec4(0,0,0,tempInfo->fillInfo->outlineInfo->lineOpacity->GetValue(mpt_feature.get()) / 100.0));
			
		}
		if (!CGlbCompareValue::CompareValueIntEqual(tempInfo->fillInfo->outlineInfo->lineOpacity,marker3DInfo->fillInfo->outlineInfo->lineOpacity,mpt_feature.get()))
		{
			outMaterial->setAlpha(osg::Material::FRONT_AND_BACK,
				marker3DInfo->fillInfo->outlineInfo->lineOpacity->GetValue(mpt_feature.get()) / 100.0);
		}
		if (!CGlbCompareValue::CompareValueLinePatternEnumEqual(tempInfo->fillInfo->outlineInfo->linePattern,marker3DInfo->fillInfo->outlineInfo->linePattern,mpt_feature.get()))
		{			
			if (tempInfo->fillInfo->outlineInfo->linePattern->GetValue(mpt_feature.get())	== GLB_LINE_DOTTED)
			{
				osg::LineStipple *lineStipple = 
					dynamic_cast<osg::LineStipple *>(outStateSet->getAttribute(osg::StateAttribute::LINESTIPPLE));
				outStateSet->setAttributeAndModes(lineStipple,osg::StateAttribute::OFF);
			}
			else
			{
				osg::LineStipple *lineStipple = 
					dynamic_cast<osg::LineStipple *>(outStateSet->getAttribute(osg::StateAttribute::LINESTIPPLE));
				if(lineStipple)
					outStateSet->setAttributeAndModes(lineStipple,osg::StateAttribute::ON);
				else
				{
					ref_ptr<osg::LineStipple> lineStipple = new osg::LineStipple;
					lineStipple->setFactor(1);
					lineStipple->setPattern(0x1C47);
					outStateSet->setAttributeAndModes(lineStipple,osg::StateAttribute::ON);
				}
			}
			
		}

		if (!CGlbCompareValue::CompareValueDoubleEqual(tempInfo->fillInfo->outlineInfo->lineWidth,marker3DInfo->fillInfo->outlineInfo->lineWidth,mpt_feature.get()))
		{			
			osg::LineWidth *lineWidth = 
				dynamic_cast<osg::LineWidth *>(outStateSet->getAttribute(osg::StateAttribute::LINEWIDTH));
			if(lineWidth)
				lineWidth->setWidth(marker3DInfo->fillInfo->outlineInfo->lineWidth->GetValue(mpt_feature.get()));			
		}

		if (!CGlbCompareValue::CompareValueDoubleEqual(tempInfo->pitch,marker3DInfo->pitch,mpt_feature.get()) ||
			!CGlbCompareValue::CompareValueDoubleEqual(tempInfo->yaw,marker3DInfo->yaw,mpt_feature.get()) ||
			!CGlbCompareValue::CompareValueDoubleEqual(tempInfo->roll,marker3DInfo->roll,mpt_feature.get()) ||
			!CGlbCompareValue::CompareValueDoubleEqual(tempInfo->xScale,marker3DInfo->xScale,mpt_feature.get()) ||
			!CGlbCompareValue::CompareValueDoubleEqual(tempInfo->yScale,marker3DInfo->yScale,mpt_feature.get()) ||
			!CGlbCompareValue::CompareValueDoubleEqual(tempInfo->zScale,marker3DInfo->zScale,mpt_feature.get()) ||
			!CGlbCompareValue::CompareValueDoubleEqual(tempInfo->xOffset,marker3DInfo->xOffset,mpt_feature.get()) ||
			!CGlbCompareValue::CompareValueDoubleEqual(tempInfo->yOffset,marker3DInfo->yOffset,mpt_feature.get()) ||
			!CGlbCompareValue::CompareValueDoubleEqual(tempInfo->zOffset,marker3DInfo->zOffset,mpt_feature.get()))
		{//offset值被修改
			glbInt32 numChild = mpt_node->asSwitch()->getNumChildren();
			for (glbInt32 i = 0; i < numChild;i++)
			{
				osg::MatrixTransform *mt = dynamic_cast<osg::MatrixTransform *>(mpt_node->asSwitch()->getChild(i));
				if(mt == NULL)continue;
				osg::Vec3d position(0.0,0.0,0.0);
				if (mpt_feature.get())
				{
					glbDouble xOrLon,yOrLat,zOrAlt,M;
					zOrAlt = 0.0;

					if(GetFeaturePoint(i,&xOrLon,&yOrLat,&zOrAlt,&M) == false)
						return false;
					position = osg::Vec3d(xOrLon,yOrLat,zOrAlt);
				}
				else
				{
					position = osg::Vec3d(mpr_xOrLon,mpr_yOrLat,mpr_zOrAlt);
				}

				osg::Matrix m2;
				osg::Matrix m3;
				osg::Matrixd localToWorld;

				osg::Matrix mOffset;
				glbDouble xOffset = 0;
				if(marker3DInfo->xOffset)
					xOffset = marker3DInfo->xOffset->GetValue(mpt_feature.get());
				glbDouble yOffset = 0;
				if (marker3DInfo->yOffset)
					yOffset = marker3DInfo->yOffset->GetValue(mpt_feature.get());
				glbDouble zOffset = 0;
				if (marker3DInfo->zOffset)
					zOffset = marker3DInfo->zOffset->GetValue(mpt_feature.get());

				if(mpt_globe->GetType() == GLB_GLOBETYPE_GLOBE)
				{
					//
					double longitude = osg::DegreesToRadians(mpr_xOrLon);
					double latitude = osg::DegreesToRadians(mpr_yOrLat);

					// Compute up vector
					osg::Vec3d    up      ( cos(longitude)*cos(latitude), sin(longitude)*cos(latitude), sin(latitude));
					// Compute east vector
					osg::Vec3d    east    (-sin(longitude), cos(longitude), 0);
					// Compute north  vector = outer product up x east
					osg::Vec3d    north   = up ^ east;

					north.normalize();
					osg::Vec3 _Offset = east*xOffset + north*yOffset + up*zOffset;
					mOffset.makeTranslate(_Offset);
					//
					ComputePosByAltitudeAndGloleType(position);
					g_ellipsoidModel->computeLocalToWorldTransformFromXYZ(position.x(),position.y(),position.z(),localToWorld);
				}
				else if(mpt_globe->GetType() == GLB_GLOBETYPE_FLAT)
				{
					//
					mOffset.makeTranslate(xOffset,yOffset,zOffset);
					//
					localToWorld.makeTranslate(position.x(),position.y(),position.z());
				}
				if(marker3DInfo->xScale && marker3DInfo->yScale && marker3DInfo->zScale)
					m2.makeScale(marker3DInfo->xScale->GetValue(mpt_feature.get()),
					marker3DInfo->yScale->GetValue(mpt_feature.get()),marker3DInfo->zScale->GetValue(mpt_feature.get()));
				if(marker3DInfo->pitch && marker3DInfo->yaw && marker3DInfo->roll)
				{
					glbDouble yaw = osg::DegreesToRadians(marker3DInfo->yaw->GetValue(mpt_feature.get()));
					glbDouble pitch = osg::DegreesToRadians(marker3DInfo->pitch->GetValue(mpt_feature.get()));
					glbDouble roll = osg::DegreesToRadians(marker3DInfo->roll->GetValue(mpt_feature.get()));
					osg::Vec3d yaw_vec(0.0,0.0,1.0);
					osg::Vec3d pitch_vec(cos(yaw),sin(yaw),0.0);
					osg::Vec3d roll_vec(-sin(yaw)*cos(pitch),cos(pitch)*cos(yaw),sin(pitch));
					m3.makeRotate(yaw,yaw_vec,pitch,pitch_vec,roll,roll_vec);
				}
				mt->setMatrix(m2 * m3 * localToWorld * mOffset);		
				// 自动放缩法线向量比例以适应模型放缩比例 2016.4.21 malin
				mt->getOrCreateStateSet()->setMode( GL_RESCALE_NORMAL, osg::StateAttribute::ON );
				isUpdateExtent = true;
			}		
		}

		if (isUpdateExtent)	mpt_globe->UpdateObject(this);		
	}

	return true;
}

glbBool CGlbGlobePoint::DealPixelSymbol( GlbMarkerPixelSymbolInfo *pixelInfo,glbBool &isNeedDirectDraw )
{
	glbBool isUpdateExtent = false;
	GlbMarkerPixelSymbolInfo *tempInfo = (GlbMarkerPixelSymbolInfo *)(mpr_renderInfo.get());
	if (tempInfo == NULL) return false;

	glbInt32 numChild = mpt_node->asSwitch()->getNumChildren();
	for(glbInt32 i = 0; i < numChild; i++)
	{
		osg::MatrixTransform *mt = 
			dynamic_cast<osg::MatrixTransform *>(mpt_node->asSwitch()->getChild(i));
		if(mt == NULL) continue;

		osg::Group *group = dynamic_cast<osg::Group *>(mt->getChild(0));
		if(group == NULL) continue;

		osg::Geode *geode = dynamic_cast<osg::Geode *>(group->getChild(0));
		if(geode == NULL) continue;

		osg::Switch *swiNode = NULL;
		if(group->getNumChildren() > 1)
			swiNode = dynamic_cast<osg::Switch *>(group->getChild(1));

		if(swiNode)
		{
			if(!pixelInfo->label)
			{
				isNeedDirectDraw = true; return true;
			}
			else
			{
				if(!CGlbCompareValue::CompareValueBoolEqual(tempInfo->label->bShow,pixelInfo->label->bShow,mpt_feature.get()))
				{
					if(pixelInfo->label->bShow->GetValue(mpt_feature.get()))
						swiNode->setAllChildrenOn();
					else
						swiNode->setAllChildrenOff();
				}

				if (!CGlbCompareValue::CompareValueBillboardEnumEqual(tempInfo->label->mode,pixelInfo->label->mode,mpt_feature.get()))
				{
					osg::Group *mtGroup = dynamic_cast<osg::Group *>(swiNode->getChild(0));
					if(mtGroup == NULL) continue;
					GlbGlobeAutoTransform *autoTransform = dynamic_cast<GlbGlobeAutoTransform *>(mtGroup->getChild(0));
					if(autoTransform == NULL) continue;
					if (pixelInfo->label->mode->GetValue(mpt_feature.get()) == GLB_BILLBOARD_SCREEN)
					{
						autoTransform->setAutoRotateMode(osg::AutoTransform::ROTATE_TO_SCREEN);
					}
					else if (pixelInfo->label->mode->GetValue(mpt_feature.get()) == GLB_BILLBOARD_AXIS)
					{
						autoTransform->setAutoRotateMode(osg::AutoTransform::ROTATE_TO_AXIS);
						autoTransform->setAxis(osg::Vec3(0.0f,0.0f,1.0f));
						autoTransform->setNormal(osg::Vec3(0.0f,-1.0f,0.0f));
					}
					else if(pixelInfo->label->mode->GetValue(mpt_feature.get()) == GLB_BILLBOARD_AUTOSLOPE)
					{
						autoTransform->setAutoRotateMode(osg::AutoTransform::ROTATE_TO_AXIS);
						autoTransform->setAxis(osg::Vec3(1.0f,0.0f,0.0f));
						autoTransform->setNormal(osg::Vec3(0.0f,-1.0f,0.0f));
					}			
				}

				if (!CGlbCompareValue::CompareValueColorEqual(tempInfo->label->textInfo->color,pixelInfo->label->textInfo->color,mpt_feature.get()) ||
					!CGlbCompareValue::CompareValueIntEqual(tempInfo->label->textInfo->size,pixelInfo->label->textInfo->size,mpt_feature.get()) ||
					!CGlbCompareValue::CompareValueStringEqual(tempInfo->label->textInfo->content,pixelInfo->label->textInfo->content,mpt_feature.get()) ||
					!CGlbCompareValue::CompareValueStringEqual(tempInfo->label->textInfo->font,pixelInfo->label->textInfo->font,mpt_feature.get()) ||
					!CGlbCompareValue::CompareValueBoolEqual(tempInfo->label->textInfo->isBold,pixelInfo->label->textInfo->isBold,mpt_feature.get()) ||
					!CGlbCompareValue::CompareValueBoolEqual(tempInfo->label->textInfo->isItalic,pixelInfo->label->textInfo->isItalic,mpt_feature.get()) ||
					!CGlbCompareValue::CompareValueBoolEqual(tempInfo->label->textInfo->isUnderline,pixelInfo->label->textInfo->isUnderline,mpt_feature.get()) ||
					!CGlbCompareValue::CompareValueMultilineAlignEnumEqual(tempInfo->label->textInfo->multilineAlign,pixelInfo->label->textInfo->multilineAlign,mpt_feature.get()) ||
					!CGlbCompareValue::CompareValueColorEqual(tempInfo->label->textInfo->backColor,pixelInfo->label->textInfo->backColor,mpt_feature.get()) ||
					!CGlbCompareValue::CompareValueIntEqual(tempInfo->label->textInfo->backOpacity,pixelInfo->label->textInfo->backOpacity,mpt_feature.get()) ||
					!CGlbCompareValue::CompareValueBoolEqual(tempInfo->label->geoInfo->isLimit,pixelInfo->label->geoInfo->isLimit,mpt_feature.get()) ||
					!CGlbCompareValue::CompareValueDoubleEqual(tempInfo->label->geoInfo->ratio,pixelInfo->label->geoInfo->ratio,mpt_feature.get()) ||
					!CGlbCompareValue::CompareValueLabelAlignEnumEqual(tempInfo->label->labelAlign,pixelInfo->label->labelAlign,mpt_feature.get()))
				{
					isNeedDirectDraw = true;
					return true;
				}

				if(!CGlbCompareValue::CompareValueDoubleEqual(tempInfo->label->xOffset,pixelInfo->label->xOffset,mpt_feature.get()) ||
					!CGlbCompareValue::CompareValueDoubleEqual(tempInfo->label->yOffset,pixelInfo->label->yOffset,mpt_feature.get()) ||
					!CGlbCompareValue::CompareValueDoubleEqual(tempInfo->label->zOffset,pixelInfo->label->zOffset,mpt_feature.get()))
				{
					osg::MatrixTransform *mt = dynamic_cast<osg::MatrixTransform *>(swiNode->getChild(0));
					osg::Matrix m;
					m.makeTranslate(pixelInfo->label->xOffset->GetValue(mpt_feature.get())/* - tempInfo->label->xOffset->GetValue(mpt_feature.get())*/,
						pixelInfo->label->yOffset->GetValue(mpt_feature.get())/* - tempInfo->label->yOffset->GetValue(mpt_feature.get())*/,
						pixelInfo->label->zOffset->GetValue(mpt_feature.get())/* - tempInfo->label->zOffset->GetValue(mpt_feature.get())*/);
					mt->setMatrix(m);
				}
			}
		}
		else
		{
			if(pixelInfo->label && pixelInfo->label->bShow && pixelInfo->label->bShow->GetValue(mpt_feature.get()))
			{
				isNeedDirectDraw = true;
				return true;
			}
		}

		osg::ref_ptr<osg::StateSet> stateset = geode->getOrCreateStateSet();
		osg::Material *material = dynamic_cast<osg::Material*>(stateset->getAttribute(osg::StateAttribute::MATERIAL));
		if (!CGlbCompareValue::CompareValueIntEqual(tempInfo->size,pixelInfo->size,mpt_feature.get()))
		{
			osg::Point *point = 
				dynamic_cast<osg::Point*>(stateset->getAttribute(osg::StateAttribute::POINT));
			point->setSize(pixelInfo->size->GetValue(mpt_feature.get()));
			isUpdateExtent = true;
		}

		if (!CGlbCompareValue::CompareValueIntEqual(tempInfo->opacity,pixelInfo->opacity,mpt_feature.get()))
		{
			if(material && pixelInfo->opacity)
			{
				glbInt32 opacity = pixelInfo->opacity->GetValue(mpt_feature.get());
				material->setAlpha(osg::Material::FRONT_AND_BACK,
					opacity / 100.0);
				if (opacity<100)
				{
					stateset->setMode(GL_BLEND,osg::StateAttribute::ON);
					if (stateset->getRenderingHint() != osg::StateSet::TRANSPARENT_BIN)
						stateset->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
				}
				else
				{
					stateset->setMode(GL_BLEND,osg::StateAttribute::OFF);
					stateset->setRenderingHint(osg::StateSet::OPAQUE_BIN);
				}
			}
		}

		if (!CGlbCompareValue::CompareValueColorEqual(tempInfo->color,pixelInfo->color,mpt_feature.get()))
		{
			if(material && pixelInfo->color)
			{
				osg::Vec4 color = GetColor(pixelInfo->color->GetValue(mpt_feature.get()));
				material->setEmission(osg::Material::FRONT_AND_BACK,color);
				material->setAlpha(osg::Material::FRONT_AND_BACK,
					tempInfo->opacity->GetValue(mpt_feature.get()) / 100.0);
			}
		}
	}

	if (isUpdateExtent)	mpt_globe->UpdateObject(this);
	return true;
}

glbInt32 CGlbGlobePoint::ComputeNodeSize( osg::Node *node )
{	
	if (node == NULL) return 0;
	CGlbCalculateNodeSizeVisitor cnsv;
	node->accept(cnsv);
	return cnsv.getTextureSize() + cnsv.getNodeMemSize();
}

glbInt32 CGlbGlobePoint::GetOsgNodeSize()
{
	if (mpt_node == NULL) return 0;
	if (mpr_objSize==0)
		mpr_objSize = ComputeNodeSize(mpt_node);
	return mpr_objSize;
		 
}

glbBool CGlbGlobePoint::DealLabelSymbol( GlbMarkerLabelSymbolInfo *labelInfo,glbBool &isNeedDirectDraw )
{
	glbBool isUpdateExtent = false;
	GlbMarkerLabelSymbolInfo *tempInfo = (GlbMarkerLabelSymbolInfo *)(mpr_renderInfo.get());
	if (tempInfo == NULL) return false;

	if (!CGlbCompareValue::CompareValueDoubleEqual(tempInfo->geoInfo->ratio,labelInfo->geoInfo->ratio,mpt_feature.get()) ||
		!CGlbCompareValue::CompareValueBoolEqual(tempInfo->geoInfo->isLimit,labelInfo->geoInfo->isLimit,mpt_feature.get()) ||
		!CGlbCompareValue::CompareValueLabelAlignEnumEqual(tempInfo->labelAlign,labelInfo->labelAlign,mpt_feature.get()) ||
		!CGlbCompareValue::CompareValueColorEqual(tempInfo->textInfo->color,labelInfo->textInfo->color,mpt_feature.get()) ||
		!CGlbCompareValue::CompareValueStringEqual(tempInfo->textInfo->content,labelInfo->textInfo->content,mpt_feature.get()) ||
		!CGlbCompareValue::CompareValueStringEqual(tempInfo->textInfo->font,labelInfo->textInfo->font,mpt_feature.get()) ||
		!CGlbCompareValue::CompareValueBoolEqual(tempInfo->textInfo->isBold,labelInfo->textInfo->isBold,mpt_feature.get()) ||
		!CGlbCompareValue::CompareValueBoolEqual(tempInfo->textInfo->isItalic,labelInfo->textInfo->isItalic,mpt_feature.get()) ||
		!CGlbCompareValue::CompareValueBoolEqual(tempInfo->textInfo->isUnderline,labelInfo->textInfo->isUnderline,mpt_feature.get()) ||
		!CGlbCompareValue::CompareValueMultilineAlignEnumEqual(tempInfo->textInfo->multilineAlign,labelInfo->textInfo->multilineAlign,mpt_feature.get()) ||
		!CGlbCompareValue::CompareValueIntEqual(tempInfo->textInfo->size,labelInfo->textInfo->size,mpt_feature.get()) ||
		!CGlbCompareValue::CompareValueColorEqual(tempInfo->imageInfo->color,labelInfo->imageInfo->color,mpt_feature.get()) ||
		!CGlbCompareValue::CompareValueStringEqual(tempInfo->imageInfo->dataSource,labelInfo->imageInfo->dataSource,mpt_feature.get()) ||
		!CGlbCompareValue::CompareValueIntEqual(tempInfo->imageInfo->maxSize,labelInfo->imageInfo->maxSize,mpt_feature.get()) ||
		!CGlbCompareValue::CompareValueIntEqual(tempInfo->imageInfo->opacity,labelInfo->imageInfo->opacity,mpt_feature.get()) ||
		!CGlbCompareValue::CompareValueLabelAlignEnumEqual(tempInfo->labelImageRelationInfo->align,labelInfo->labelImageRelationInfo->align,mpt_feature.get()) ||
		!CGlbCompareValue::CompareValueLabelRelationEnumEqual(tempInfo->labelImageRelationInfo->relation,labelInfo->labelImageRelationInfo->relation,mpt_feature.get()) ||
		!CGlbCompareValue::CompareValueColorEqual(tempInfo->textInfo->backColor,labelInfo->textInfo->backColor,mpt_feature.get()) ||
		!CGlbCompareValue::CompareValueIntEqual(tempInfo->textInfo->backOpacity,labelInfo->textInfo->backOpacity,mpt_feature.get()))
		isNeedDirectDraw = true;

	if(true == isNeedDirectDraw)
		return true;

	glbInt32 numChild = mpt_node->asSwitch()->getNumChildren();
	for(glbInt32 i = 0; i < numChild; i++)
	{
		osg::MatrixTransform *mt = 
			dynamic_cast<osg::MatrixTransform *>(mpt_node->asSwitch()->getChild(i));
		if(NULL == mt) continue;
		//osg::Group *group = dynamic_cast<osg::Group*>(mt->getChild(0));
		osg::MatrixTransform *group = dynamic_cast<osg::MatrixTransform *>(mt->getChild(0));
		if(NULL == group) continue;
		GlbGlobeAutoTransform *autoTransform = dynamic_cast<GlbGlobeAutoTransform *>(group->getChild(0));
		if(NULL == autoTransform) continue;
		GlbGlobeAutoTransform *outAutoTransform = NULL;
		if(group->getNumChildren() > 1)
			outAutoTransform = dynamic_cast<GlbGlobeAutoTransform *>(group->getChild(1));
		osg::Geode * geode = dynamic_cast<osg::Geode *>(autoTransform->getChild(0));
		if(NULL == geode) continue;
		osg::Geometry *labelGeom = dynamic_cast<osg::Geometry *>(geode->getDrawable(0));
		if(NULL == labelGeom) continue;

		osg::ref_ptr<osg::StateSet> stateset = labelGeom->getOrCreateStateSet();
		if (!CGlbCompareValue::CompareValueBillboardEnumEqual(tempInfo->mode,labelInfo->mode,mpt_feature.get()))
		{		
			if (labelInfo->mode->GetValue(mpt_feature.get()) == GLB_BILLBOARD_SCREEN)
			{
				autoTransform->setAutoRotateMode(osg::AutoTransform::ROTATE_TO_SCREEN);
				if(outAutoTransform)
					outAutoTransform->setAutoRotateMode(osg::AutoTransform::ROTATE_TO_SCREEN);
			}
			else if (labelInfo->mode->GetValue(mpt_feature.get()) == GLB_BILLBOARD_AXIS)
			{
				autoTransform->setAutoRotateMode(osg::AutoTransform::ROTATE_TO_AXIS);
				autoTransform->setAxis(osg::Vec3(0.0f,0.0f,1.0f));
				autoTransform->setNormal(osg::Vec3(0.0f,-1.0f,0.0f));
				if(outAutoTransform)
				{
					outAutoTransform->setAutoRotateMode(osg::AutoTransform::ROTATE_TO_AXIS);
					outAutoTransform->setAxis(osg::Vec3(0.0f,0.0f,1.0f));
					outAutoTransform->setNormal(osg::Vec3(0.0f,-1.0f,0.0f));
				}
			}
			else if(labelInfo->mode->GetValue(mpt_feature.get()) == GLB_BILLBOARD_AUTOSLOPE)
			{
				autoTransform->setAutoRotateMode(osg::AutoTransform::ROTATE_TO_AXIS);
				autoTransform->setAxis(osg::Vec3(1.0f,0.0f,0.0f));
				autoTransform->setNormal(osg::Vec3(0.0f,-1.0f,0.0f));
				if(outAutoTransform)
				{
					outAutoTransform->setAutoRotateMode(osg::AutoTransform::ROTATE_TO_AXIS);
					outAutoTransform->setAxis(osg::Vec3(1.0f,0.0f,0.0f));
					outAutoTransform->setNormal(osg::Vec3(0.0f,-1.0f,0.0f));
				}
			}			
		}

		if (!CGlbCompareValue::CompareValueDoubleEqual(tempInfo->pitch,labelInfo->pitch,mpt_feature.get()) ||
			!CGlbCompareValue::CompareValueDoubleEqual(tempInfo->yaw,labelInfo->yaw,mpt_feature.get()) ||
			!CGlbCompareValue::CompareValueDoubleEqual(tempInfo->roll,labelInfo->roll,mpt_feature.get()))
		{
			osg::Vec3d position(0.0,0.0,0.0);
			if (mpt_feature.get())
			{
				glbDouble xOrLon,yOrLat,zOrAlt,M;
				zOrAlt = 0.0;

				if(GetFeaturePoint(i,&xOrLon,&yOrLat,&zOrAlt,&M) == false)
					return false;

				position = osg::Vec3d(xOrLon,yOrLat,zOrAlt);
			}
			else
				position = osg::Vec3d(mpr_xOrLon,mpr_yOrLat,mpr_zOrAlt);

			osg::Matrix m3;
			osg::Matrixd localToWorld;

			if(mpt_globe->GetType() == GLB_GLOBETYPE_GLOBE)
			{
				ComputePosByAltitudeAndGloleType(position);
				g_ellipsoidModel->computeLocalToWorldTransformFromXYZ(position.x(),position.y(),position.z(),localToWorld);
			}
			else if(mpt_globe->GetType() == GLB_GLOBETYPE_FLAT)
				localToWorld.makeTranslate(position.x(),position.y(),position.z());

			if(labelInfo->pitch && labelInfo->yaw && labelInfo->roll)
			{
				glbDouble yaw = osg::DegreesToRadians(labelInfo->yaw->GetValue(mpt_feature.get()));
				glbDouble pitch = osg::DegreesToRadians(labelInfo->pitch->GetValue(mpt_feature.get()));
				glbDouble roll = osg::DegreesToRadians(labelInfo->roll->GetValue(mpt_feature.get()));
				osg::Vec3d yaw_vec(0.0,0.0,1.0);
				osg::Vec3d pitch_vec(cos(yaw),sin(yaw),0.0);
				osg::Vec3d roll_vec(-sin(yaw)*cos(pitch),cos(pitch)*cos(yaw),sin(pitch));
				m3.makeRotate(yaw,yaw_vec,pitch,pitch_vec,roll,roll_vec);
			}

			mt->setMatrix(m3 * localToWorld);
			isUpdateExtent = true;			
		}

		if(!CGlbCompareValue::CompareValueDoubleEqual(tempInfo->xOffset,labelInfo->xOffset,mpt_feature.get()) ||
			!CGlbCompareValue::CompareValueDoubleEqual(tempInfo->yOffset,labelInfo->yOffset,mpt_feature.get()) ||
			!CGlbCompareValue::CompareValueDoubleEqual(tempInfo->zOffset,labelInfo->zOffset,mpt_feature.get()))
		{
			osg::Matrix m;
			m.makeTranslate(labelInfo->xOffset->GetValue(mpt_feature.get())/* - tempInfo->xOffset->GetValue(mpt_feature.get())*/,
				labelInfo->yOffset->GetValue(mpt_feature.get())/* - tempInfo->yOffset->GetValue(mpt_feature.get())*/,
				labelInfo->zOffset->GetValue(mpt_feature.get())/* - tempInfo->zOffset->GetValue(mpt_feature.get())*/);
			group->setMatrix(m);
		}
	}

	if (isUpdateExtent) mpt_globe->UpdateObject(this);
	return true;
}

glbBool GlbGlobe::CGlbGlobePoint::DealDynamicLabelSymbol( GlbMarkerDynamicLabelSymbolInfo *dynamicLabelInfo,glbBool &isNeedDirectDraw )
{
	glbBool isUpdateExtent = false;
	GlbMarkerDynamicLabelSymbolInfo *tempInfo = (GlbMarkerDynamicLabelSymbolInfo *)(mpr_renderInfo.get());
	if (tempInfo == NULL) return false;

	if (!CGlbCompareValue::CompareValueDoubleEqual(tempInfo->geoInfo->ratio,dynamicLabelInfo->geoInfo->ratio,mpt_feature.get()) ||
		!CGlbCompareValue::CompareValueBoolEqual(tempInfo->geoInfo->isLimit,dynamicLabelInfo->geoInfo->isLimit,mpt_feature.get()) ||
		!CGlbCompareValue::CompareValueLabelAlignEnumEqual(tempInfo->labelAlign,dynamicLabelInfo->labelAlign,mpt_feature.get()) ||
		!CGlbCompareValue::CompareValueColorEqual(tempInfo->textInfo->color,dynamicLabelInfo->textInfo->color,mpt_feature.get()) ||
		!CGlbCompareValue::CompareValueStringEqual(tempInfo->textInfo->content,dynamicLabelInfo->textInfo->content,mpt_feature.get()) ||
		!CGlbCompareValue::CompareValueStringEqual(tempInfo->textInfo->font,dynamicLabelInfo->textInfo->font,mpt_feature.get()) ||
		!CGlbCompareValue::CompareValueBoolEqual(tempInfo->textInfo->isBold,dynamicLabelInfo->textInfo->isBold,mpt_feature.get()) ||
		!CGlbCompareValue::CompareValueBoolEqual(tempInfo->textInfo->isItalic,dynamicLabelInfo->textInfo->isItalic,mpt_feature.get()) ||
		!CGlbCompareValue::CompareValueBoolEqual(tempInfo->textInfo->isUnderline,dynamicLabelInfo->textInfo->isUnderline,mpt_feature.get()) ||
		!CGlbCompareValue::CompareValueMultilineAlignEnumEqual(tempInfo->textInfo->multilineAlign,dynamicLabelInfo->textInfo->multilineAlign,mpt_feature.get()) ||
		!CGlbCompareValue::CompareValueIntEqual(tempInfo->textInfo->size,dynamicLabelInfo->textInfo->size,mpt_feature.get()) ||
		!CGlbCompareValue::CompareValueColorEqual(tempInfo->imageInfo->color,dynamicLabelInfo->imageInfo->color,mpt_feature.get()) ||
		!CGlbCompareValue::CompareValueStringEqual(tempInfo->imageInfo->dataSource,dynamicLabelInfo->imageInfo->dataSource,mpt_feature.get()) ||
		!CGlbCompareValue::CompareValueIntEqual(tempInfo->imageInfo->maxSize,dynamicLabelInfo->imageInfo->maxSize,mpt_feature.get()) ||
		!CGlbCompareValue::CompareValueIntEqual(tempInfo->imageInfo->opacity,dynamicLabelInfo->imageInfo->opacity,mpt_feature.get()) ||
		!CGlbCompareValue::CompareValueColorEqual(tempInfo->textInfo->backColor,dynamicLabelInfo->textInfo->backColor,mpt_feature.get()) ||
		!CGlbCompareValue::CompareValueIntEqual(tempInfo->textInfo->backOpacity,dynamicLabelInfo->textInfo->backOpacity,mpt_feature.get()) ||
		!CGlbCompareValue::CompareValueDynamicLabelFadeEnumEqual(tempInfo->fadeMode,dynamicLabelInfo->fadeMode,mpt_feature.get()) ||
		!CGlbCompareValue::CompareValueDynamicLabelFadeSpeedEnumEqual(tempInfo->fadeSpeed,dynamicLabelInfo->fadeSpeed,mpt_feature.get()))
		isNeedDirectDraw = true;

	if(true == isNeedDirectDraw)
	{
		if(mpr_isFadingDynamicLabel)
		{
			mpr_isFadingDynamicLabel = false;
			glbref_ptr<CGlbGlobeCallBack::CGlbGlobeFadeCallBackTask> task = new CGlbGlobeCallBack::CGlbGlobeFadeCallBackTask(mpt_node);
			mpt_globe->mpr_p_callback->AddFadeTask(task.get());
		}
		return true;
	}

	glbInt32 numChild = mpt_node->asSwitch()->getNumChildren();
	for(glbInt32 i = 0; i < numChild; i++)
	{
		osg::MatrixTransform *mt = 
			dynamic_cast<osg::MatrixTransform *>(mpt_node->asSwitch()->getChild(i));
		if(NULL == mt) continue;
		//osg::Group *group = dynamic_cast<osg::Group*>(mt->getChild(0));
		osg::MatrixTransform *group = dynamic_cast<osg::MatrixTransform *>(mt->getChild(0));
		if(NULL == group) continue;
		GlbGlobeAutoTransform *autoTransform = dynamic_cast<GlbGlobeAutoTransform *>(group->getChild(0));
		if(NULL == autoTransform) continue;
		GlbGlobeAutoTransform *outAutoTransform = NULL;
		if(group->getNumChildren() > 1)
			outAutoTransform = dynamic_cast<GlbGlobeAutoTransform *>(group->getChild(1));
		osg::Geode * geode = dynamic_cast<osg::Geode *>(autoTransform->getChild(0));
		if(NULL == geode) continue;
		osg::Geometry *labelGeom = dynamic_cast<osg::Geometry *>(geode->getDrawable(0));
		if(NULL == labelGeom) continue;

		osg::ref_ptr<osg::StateSet> stateset = labelGeom->getOrCreateStateSet();
		if (!CGlbCompareValue::CompareValueBillboardEnumEqual(tempInfo->mode,dynamicLabelInfo->mode,mpt_feature.get()))
		{		
			if (dynamicLabelInfo->mode->GetValue(mpt_feature.get()) == GLB_BILLBOARD_SCREEN)
			{
				autoTransform->setAutoRotateMode(osg::AutoTransform::ROTATE_TO_SCREEN);
				if(outAutoTransform)
					outAutoTransform->setAutoRotateMode(osg::AutoTransform::ROTATE_TO_SCREEN);
			}
			else if (dynamicLabelInfo->mode->GetValue(mpt_feature.get()) == GLB_BILLBOARD_AXIS)
			{
				autoTransform->setAutoRotateMode(osg::AutoTransform::ROTATE_TO_AXIS);
				autoTransform->setAxis(osg::Vec3(0.0f,0.0f,1.0f));
				autoTransform->setNormal(osg::Vec3(0.0f,-1.0f,0.0f));
				if(outAutoTransform)
				{
					outAutoTransform->setAutoRotateMode(osg::AutoTransform::ROTATE_TO_AXIS);
					outAutoTransform->setAxis(osg::Vec3(0.0f,0.0f,1.0f));
					outAutoTransform->setNormal(osg::Vec3(0.0f,-1.0f,0.0f));
				}
			}
			else if(dynamicLabelInfo->mode->GetValue(mpt_feature.get()) == GLB_BILLBOARD_AUTOSLOPE)
			{
				autoTransform->setAutoRotateMode(osg::AutoTransform::ROTATE_TO_AXIS);
				autoTransform->setAxis(osg::Vec3(1.0f,0.0f,0.0f));
				autoTransform->setNormal(osg::Vec3(0.0f,-1.0f,0.0f));
				if(outAutoTransform)
				{
					outAutoTransform->setAutoRotateMode(osg::AutoTransform::ROTATE_TO_AXIS);
					outAutoTransform->setAxis(osg::Vec3(1.0f,0.0f,0.0f));
					outAutoTransform->setNormal(osg::Vec3(0.0f,-1.0f,0.0f));
				}
			}			
		}

		if (!CGlbCompareValue::CompareValueDoubleEqual(tempInfo->pitch,dynamicLabelInfo->pitch,mpt_feature.get()) ||
			!CGlbCompareValue::CompareValueDoubleEqual(tempInfo->yaw,dynamicLabelInfo->yaw,mpt_feature.get()) ||
			!CGlbCompareValue::CompareValueDoubleEqual(tempInfo->roll,dynamicLabelInfo->roll,mpt_feature.get()))
		{
			osg::Vec3d position(0.0,0.0,0.0);
			if (mpt_feature.get())
			{
				glbDouble xOrLon,yOrLat,zOrAlt,M;
				zOrAlt = 0.0;

				if(GetFeaturePoint(i,&xOrLon,&yOrLat,&zOrAlt,&M) == false)
					return false;

				position = osg::Vec3d(xOrLon,yOrLat,zOrAlt);
			}
			else
				position = osg::Vec3d(mpr_xOrLon,mpr_yOrLat,mpr_zOrAlt);

			osg::Matrix m3;
			osg::Matrixd localToWorld;

			if(mpt_globe->GetType() == GLB_GLOBETYPE_GLOBE)
			{
				ComputePosByAltitudeAndGloleType(position);
				g_ellipsoidModel->computeLocalToWorldTransformFromXYZ(position.x(),position.y(),position.z(),localToWorld);
			}
			else if(mpt_globe->GetType() == GLB_GLOBETYPE_FLAT)
				localToWorld.makeTranslate(position.x(),position.y(),position.z());

			if(dynamicLabelInfo->pitch && dynamicLabelInfo->yaw && dynamicLabelInfo->roll)
			{
				glbDouble yaw = osg::DegreesToRadians(dynamicLabelInfo->yaw->GetValue(mpt_feature.get()));
				glbDouble pitch = osg::DegreesToRadians(dynamicLabelInfo->pitch->GetValue(mpt_feature.get()));
				glbDouble roll = osg::DegreesToRadians(dynamicLabelInfo->roll->GetValue(mpt_feature.get()));
				osg::Vec3d yaw_vec(0.0,0.0,1.0);
				osg::Vec3d pitch_vec(cos(yaw),sin(yaw),0.0);
				osg::Vec3d roll_vec(-sin(yaw)*cos(pitch),cos(pitch)*cos(yaw),sin(pitch));
				m3.makeRotate(yaw,yaw_vec,pitch,pitch_vec,roll,roll_vec);
			}

			mt->setMatrix(m3 * localToWorld);
			isUpdateExtent = true;			
		}

		if(!CGlbCompareValue::CompareValueDoubleEqual(tempInfo->xOffset,dynamicLabelInfo->xOffset,mpt_feature.get()) ||
			!CGlbCompareValue::CompareValueDoubleEqual(tempInfo->yOffset,dynamicLabelInfo->yOffset,mpt_feature.get()) ||
			!CGlbCompareValue::CompareValueDoubleEqual(tempInfo->zOffset,dynamicLabelInfo->zOffset,mpt_feature.get()))
		{
			osg::Matrix m;
			m.makeTranslate(dynamicLabelInfo->xOffset->GetValue(mpt_feature.get()),
				dynamicLabelInfo->yOffset->GetValue(mpt_feature.get()),
				dynamicLabelInfo->zOffset->GetValue(mpt_feature.get()));
			group->setMatrix(m);
		}
	}

	if (isUpdateExtent) mpt_globe->UpdateObject(this);
	return true;
}


glbBool CGlbGlobePoint::DealImageSymbol( GlbMarkerImageSymbolInfo *imageInfo,glbBool &isNeedDirectDraw )
{
	glbBool isUpdateExtent = false;
	GlbMarkerImageSymbolInfo *tempInfo = (GlbMarkerImageSymbolInfo *)(mpr_renderInfo.get());
	if (tempInfo == NULL) return false;

	if (!CGlbCompareValue::CompareValueDoubleEqual(tempInfo->geoInfo->ratio,imageInfo->geoInfo->ratio,mpt_feature.get()) ||
		!CGlbCompareValue::CompareValueBoolEqual(tempInfo->geoInfo->isLimit,imageInfo->geoInfo->isLimit,mpt_feature.get()) ||
		!CGlbCompareValue::CompareValueImageAlignEnumEqual(tempInfo->imageAlign,imageInfo->imageAlign,mpt_feature.get()) ||
		!CGlbCompareValue::CompareValueIntEqual(tempInfo->imgInfo->maxSize,imageInfo->imgInfo->maxSize,mpt_feature.get()) ||
		!CGlbCompareValue::CompareValueBoolEqual(tempInfo->is360DegreePanorama, imageInfo->is360DegreePanorama, mpt_feature.get()))
		isNeedDirectDraw = true;

	if(true == isNeedDirectDraw)
		return true;

	if (imageInfo->is360DegreePanorama != NULL && imageInfo->is360DegreePanorama->GetValue(mpt_feature.get()))
	{// 360度全景图 - 不处理以下几种参数变化
		if (!CGlbCompareValue::CompareValueBillboardEnumEqual(tempInfo->imgBillboard,imageInfo->imgBillboard,mpt_feature.get()))
			return false;
		//if (!CGlbCompareValue::CompareValueDoubleEqual(tempInfo->pitch,imageInfo->pitch,mpt_feature.get()) ||
		//	!CGlbCompareValue::CompareValueDoubleEqual(tempInfo->yaw,imageInfo->yaw,mpt_feature.get()) ||
		//	!CGlbCompareValue::CompareValueDoubleEqual(tempInfo->roll,imageInfo->roll,mpt_feature.get()))
			//return false;
	}

	glbInt32 numChild = mpt_node->asSwitch()->getNumChildren();
	for(glbInt32 i = 0; i < numChild; i++)
	{
		osg::MatrixTransform *mt = 
			dynamic_cast<osg::MatrixTransform *>(mpt_node->asSwitch()->getChild(i));
		if(mt == NULL) continue;
		//osg::Group *group = dynamic_cast<osg::Group *>(mt->getChild(0));
		osg::MatrixTransform *group = dynamic_cast<osg::MatrixTransform *>(mt->getChild(0));
		if(NULL == group) continue;
		GlbGlobeAutoTransform *autoTransform = dynamic_cast<GlbGlobeAutoTransform *>(group->getChild(0));
		if(autoTransform == NULL) continue;
		GlbGlobeAutoTransform *outAutoTransform = NULL;
		if(group->getNumChildren() > 1)
			outAutoTransform = dynamic_cast<GlbGlobeAutoTransform *>(group->getChild(1));
		osg::Geode *geode = dynamic_cast<osg::Geode *>(autoTransform->getChild(0));
		if(geode == NULL) continue;
		osg::Geometry *geometry = dynamic_cast<osg::Geometry*>(geode->getDrawable(0));
		if(NULL == geometry) continue;
		osg::ref_ptr<osg::StateSet> stateset = geometry->getOrCreateStateSet();
		if (!CGlbCompareValue::CompareValueBillboardEnumEqual(tempInfo->imgBillboard,imageInfo->imgBillboard,mpt_feature.get()))
		{
			if (imageInfo->imgBillboard->GetValue(mpt_feature.get()) == GLB_BILLBOARD_SCREEN)
			{
				autoTransform->setAutoRotateMode(osg::AutoTransform::ROTATE_TO_SCREEN);
				if(outAutoTransform)
					outAutoTransform->setAutoRotateMode(osg::AutoTransform::ROTATE_TO_SCREEN);
			}
			else if (imageInfo->imgBillboard->GetValue(mpt_feature.get()) == GLB_BILLBOARD_AXIS)
			{
				autoTransform->setAutoRotateMode(osg::AutoTransform::ROTATE_TO_AXIS);
				autoTransform->setAxis(osg::Vec3(0.0f,0.0f,1.0f));
				autoTransform->setNormal(osg::Vec3(0.0f,-1.0f,0.0f));
				if(outAutoTransform)
				{
					outAutoTransform->setAutoRotateMode(osg::AutoTransform::ROTATE_TO_AXIS);
					outAutoTransform->setAxis(osg::Vec3(0.0f,0.0f,1.0f));
					outAutoTransform->setNormal(osg::Vec3(0.0f,-1.0f,0.0f));
				}
			}
			else if(imageInfo->imgBillboard->GetValue(mpt_feature.get()) == GLB_BILLBOARD_AUTOSLOPE)
			{
				autoTransform->setAutoRotateMode(osg::AutoTransform::ROTATE_TO_AXIS);
				autoTransform->setAxis(osg::Vec3(1.0f,0.0f,0.0f));
				autoTransform->setNormal(osg::Vec3(0.0f,-1.0f,0.0f));
				if(outAutoTransform)
				{
					outAutoTransform->setAutoRotateMode(osg::AutoTransform::ROTATE_TO_AXIS);
					outAutoTransform->setAxis(osg::Vec3(1.0f,0.0f,0.0f));
					outAutoTransform->setNormal(osg::Vec3(0.0f,-1.0f,0.0f));
				}
			}
		}

		if (!CGlbCompareValue::CompareValueStringEqual(tempInfo->imgInfo->dataSource,imageInfo->imgInfo->dataSource,mpt_feature.get()))
		{
			if(stateset->getNumTextureAttributeLists() > 0)
			{
				osg::Texture2D* texture = 
					dynamic_cast<osg::Texture2D*>(stateset->getTextureAttribute(0,osg::StateAttribute::TEXTURE));
				if(texture)
				{
					CGlbWString filepath(imageInfo->imgInfo->dataSource->GetValue(mpt_feature.get()));
					glbInt32 index = filepath.find_first_of(L'.');
					if(index == 0)
					{
						CGlbWString execDir = CGlbPath::GetExecDir();
						filepath = execDir + filepath.substr(1,filepath.size());
					}
					osg::Image *image = 
						osgDB::readImageFile(filepath.ToString());
					if (image)
						texture->setImage(image);
				}
			}
			else
			{
				osg::ref_ptr<osg::Texture2D> texture = new osg::Texture2D;
				CGlbWString filepath(imageInfo->imgInfo->dataSource->GetValue(mpt_feature.get()));
				glbInt32 index = filepath.find_first_of(L'.');
				if(index == 0)
				{
					CGlbWString execDir = CGlbPath::GetExecDir();
					filepath = execDir + filepath.substr(1,filepath.size());
				}
				osg::Image *image = osgDB::readImageFile(filepath.ToString());
				if (image)
				{
					texture->setImage(image);
					stateset->setTextureAttributeAndModes(0,texture,osg::StateAttribute::ON);
				}
			}
		}

		if (!CGlbCompareValue::CompareValueColorEqual(tempInfo->imgInfo->color,imageInfo->imgInfo->color,mpt_feature.get()))
		{
			osg::ref_ptr<osg::Material> material = dynamic_cast<osg::Material*>(stateset->getAttribute(osg::StateAttribute::MATERIAL));
			if(material == NULL)
				material = new osg::Material;
			material->setEmission(osg::Material::FRONT_AND_BACK,GetColor(imageInfo->imgInfo->color->GetValue(mpt_feature.get())));
			material->setAlpha(osg::Material::FRONT_AND_BACK,tempInfo->imgInfo->opacity->GetValue(mpt_feature.get()) / 100.0);
		}

		if (!CGlbCompareValue::CompareValueIntEqual(tempInfo->imgInfo->opacity,imageInfo->imgInfo->opacity,mpt_feature.get()))
		{
			osg::ref_ptr<osg::Material> material = dynamic_cast<osg::Material*>(stateset->getAttribute(osg::StateAttribute::MATERIAL));
			if(material == NULL)
				material = new osg::Material;
			material->setAlpha(osg::Material::FRONT_AND_BACK,imageInfo->imgInfo->opacity->GetValue(mpt_feature.get()) / 100.0);

			if (imageInfo->imgInfo->opacity->GetValue(mpt_feature.get()) >= 100)
			{
				stateset->setRenderingHint(osg::StateSet::OPAQUE_BIN);
				stateset->setMode(GL_BLEND,osg::StateAttribute::OFF);
			}
			else
			{
				stateset->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
				stateset->setMode(GL_BLEND,osg::StateAttribute::ON);
				osg::ref_ptr<osg::BlendFunc> pBlendFun = new osg::BlendFunc(osg::BlendFunc::/*ONE*/SRC_ALPHA,osg::BlendFunc::ONE_MINUS_SRC_ALPHA);	//设置混合方程
				stateset->setAttributeAndModes(pBlendFun.get(),osg::StateAttribute::ON);
			}
		}

		if (!CGlbCompareValue::CompareValueDoubleEqual(tempInfo->pitch,imageInfo->pitch,mpt_feature.get()) ||
			!CGlbCompareValue::CompareValueDoubleEqual(tempInfo->yaw,imageInfo->yaw,mpt_feature.get()) ||
			!CGlbCompareValue::CompareValueDoubleEqual(tempInfo->roll,imageInfo->roll,mpt_feature.get()))
		{
			osg::Vec3d position(0.0,0.0,0.0);
			if (mpt_feature.get())
			{
				glbDouble xOrLon,yOrLat,zOrAlt,M;
				zOrAlt = 0.0;

				if(GetFeaturePoint(i,&xOrLon,&yOrLat,&zOrAlt,&M) == false)
					return false;

				position = osg::Vec3d(xOrLon,yOrLat,zOrAlt);
			}
			else
				position = osg::Vec3d(mpr_xOrLon,mpr_yOrLat,mpr_zOrAlt);

			osg::Matrix m3;
			osg::Matrixd localToWorld;

			if(mpt_globe->GetType() == GLB_GLOBETYPE_GLOBE)
			{
				ComputePosByAltitudeAndGloleType(position);
				g_ellipsoidModel->computeLocalToWorldTransformFromXYZ(position.x(),position.y(),position.z(),localToWorld);
			}
			else if(mpt_globe->GetType() == GLB_GLOBETYPE_FLAT)
				localToWorld.makeTranslate(position.x(),position.y(),position.z());

			if(imageInfo->pitch && imageInfo->yaw && imageInfo->roll)
			{
				glbDouble yaw = osg::DegreesToRadians(imageInfo->yaw->GetValue(mpt_feature.get()));
				glbDouble pitch = osg::DegreesToRadians(imageInfo->pitch->GetValue(mpt_feature.get()));
				glbDouble roll = osg::DegreesToRadians(imageInfo->roll->GetValue(mpt_feature.get()));
				osg::Vec3d yaw_vec(0.0,0.0,1.0);
				osg::Vec3d pitch_vec(cos(yaw),sin(yaw),0.0);
				osg::Vec3d roll_vec(-sin(yaw)*cos(pitch),cos(pitch)*cos(yaw),sin(pitch));
				m3.makeRotate(yaw,yaw_vec,pitch,pitch_vec,roll,roll_vec);
			}

			mt->setMatrix(m3 * localToWorld);

			isUpdateExtent = true;
		}

		if(!CGlbCompareValue::CompareValueDoubleEqual(tempInfo->xOffset,imageInfo->xOffset,mpt_feature.get()) ||
			!CGlbCompareValue::CompareValueDoubleEqual(tempInfo->yOffset,imageInfo->yOffset,mpt_feature.get()) ||
			!CGlbCompareValue::CompareValueDoubleEqual(tempInfo->zOffset,imageInfo->zOffset,mpt_feature.get()))
		{
			osg::Matrix m;
			m.makeTranslate(imageInfo->xOffset->GetValue(mpt_feature.get())/* - tempInfo->xOffset->GetValue(mpt_feature.get())*/,
				imageInfo->yOffset->GetValue(mpt_feature.get())/* - tempInfo->yOffset->GetValue(mpt_feature.get())*/,
				imageInfo->zOffset->GetValue(mpt_feature.get())/* - tempInfo->zOffset->GetValue(mpt_feature.get())*/);
			group->setMatrix(m);
		}
	}

	if (isUpdateExtent)	mpt_globe->UpdateObject(this);	

	return true;
}

glbBool CGlbGlobePoint::GetFeaturePoint( glbInt32 idx,glbDouble *ptx,glbDouble *pty,glbDouble *ptz,glbDouble *ptm )
{
	IGlbGeometry *geo = NULL;
	bool result = false;
	result = mpt_feature->GetGeometry(&geo,mpt_currLevel);
	if(!result || geo == NULL)  return false;

	glbInt32 dimension = geo->GetCoordDimension();
	GlbGeometryTypeEnum geoType = geo->GetType();
	switch(geoType)
	{
	case GLB_GEO_POINT:				///<二维点
		{						
			CGlbPoint* pt = dynamic_cast<CGlbPoint *>(geo);
			if(dimension == 2)
				pt->GetXY(ptx,pty);
			else if(dimension == 3)
				pt->GetXYZ(ptx,pty,ptz);
		}
		break;
	case GLB_GEO_MULTIPOINT:		///<二维多点
		{
			CGlbMultiPoint* multipt = dynamic_cast<CGlbMultiPoint*>(geo);
			if(dimension == 2)
				multipt->GetPoint(idx,ptx,pty);
			else if(dimension == 3)
				multipt->GetPoint(idx,ptx,pty,ptz);
		}
		break;		
	}				
	return true;
}

glbBool CGlbGlobePoint::SetEdit( glbBool isEdit )
{
	if (mpt_isEdit == isEdit) return true;
	mpt_isEdit = isEdit;
	if(mpt_isEdit == true && mpr_editNode == NULL)
	{
		mpr_editNode = new osg::Group;
		osg::ref_ptr<PointEditEventHandler> editEventHandler = new PointEditEventHandler(this);
		mpr_editNode->addEventCallback(editEventHandler);
		glbref_ptr<CGlbGlobeCallBack::CGlbGlobeCallBackTask> task = 
			new CGlbGlobeCallBack::CGlbGlobeCallBackTask(mpt_parent,mpr_editNode);
		task->mpr_size = ComputeNodeSize(mpr_editNode);
		mpt_globe->mpr_p_callback->AddHangTask(task.get());
	}
	else if(mpt_isEdit == false && mpr_editNode != NULL)
	{
		glbref_ptr<CGlbGlobeCallBack::CGlbGlobeCallBackTask> task = 
			new CGlbGlobeCallBack::CGlbGlobeCallBackTask(mpt_parent,mpr_editNode);
		task->mpr_size = ComputeNodeSize(mpr_editNode);
		mpt_globe->mpr_p_callback->AddRemoveTask(task.get());
		mpr_editNode = NULL;
	}
	return true;
}

#include <osgDB/FileUtils>
// 重载ComputeRefLevel接口处理 模型类型
glbInt32 CGlbGlobePoint::ComputeRefLevel(glbDouble distance)
{
	glbInt32 level = CGlbGlobeRObject::ComputeRefLevel(distance);

	glbref_ptr<GlbMarkerModelSymbolInfo>markerSymbol = dynamic_cast<GlbMarkerModelSymbolInfo *>(mpr_renderInfo.get());
	// 3D模型类型markerSymbol->symType == GLB_MARKERSYMBOL_MODEL
	if (markerSymbol.valid() && level > 0)
	{
		glbWChar* loc = markerSymbol->locate->GetValue(mpt_feature.get());
		CGlbWString file = loc;		
		bool fe = CGlbPath::FileExist(file.c_str()); // osgDB::fileExists(file.ToString());
		if (fe==false)
		{// 查找相对exe的路径
			glbInt32 index = file.find_first_of(L'.');
			if(index == 0)
			{// 处理当前执行文件的相对路径情况 ./AAA/....
				CGlbWString execDir = CGlbPath::GetExecDir();
				CGlbWString relativeFile = execDir + file.substr(1,file.size());
				// 根据相对执行文件再次查找文件
				fe = CGlbPath::FileExist(relativeFile.c_str()); //osgDB::fileExists(lodFile.ToString());
			}
		}
		int idx = file.find(L':');
		if (fe==false)
		{// 判断是否是网络路径			
			if (idx>1)// 网络路径,无法判断lod文件是否存在，直接返回0
			{
				return 0;
			}
		}
		/*
		       level:3,2,1,0.  3是最精细的
			   3: modepath = dir/mdname
			   2:modepath = dir/2/mdname
			   1:modepath = dir/1/mdname
			   0:modepath = dir/0/mdname
			   
			   datalocate ------> level3 modepath
		*/
		CGlbWString dir   = CGlbPath::GetDir(file);
		if(dir.size() == 0)
			return 0;
		CGlbWString fname = CGlbPath::GetFileName(file);
		CGlbWString lodFile = dir;
		if (dir.at(dir.size()-1)=='\\')
			lodFile +=L"%ld\\%s";
		else
			lodFile +=L"\\%ld\\%s";
		lodFile = CGlbWString::FormatString((glbWChar*)lodFile.c_str(),level,fname.c_str());	

		//CGlbWString dir = CGlbPath::GetDir(file);		
		//wchar_t buffer[10];
		//_itow_s(level,buffer,10);
		//CGlbWString lodext = buffer;
		//dir += L"-" + lodext;
		//CGlbWString lodFile = dir + L"\\" + CGlbPath::GetFileName(file);
		bool lodfe = CGlbPath::FileExist(lodFile.c_str()); //osgDB::fileExists(lodFile.ToString());
		if (lodfe==false)
		{// 文件不存在,提示		
			//CGlbWString errorInfo = L"Error : 找不到模型文件:" + lodFile;
			//MessageBox(NULL,errorInfo.c_str(),L"Error",MB_OK);			
			if (fe)
				level = 0;
			else
				level = mpt_currLevel;
		}
	}
	else
	{
		level = 0;
	}
	return level;
}

void CGlbGlobePoint::DealEdit()
{
	/*osg::ref_ptr<osg::Node> node = g_editObject->GetOsgNode();
	if(node)
	{
		osg::ref_ptr<PointObjectEventHandler> poEventHandle = new PointObjectEventHandler(mpt_globe);
		node->setEventCallback(poEventHandle);
	}*/
}

void CGlbGlobePoint::DirtyOnTerrainObject()
{
	if (!mpt_globe) return;
	CGlbExtent* cglbExtent = GetBound(false).get();	
	if (cglbExtent)
		mpt_globe->AddDomDirtyExtent(*cglbExtent);
}

void CGlbGlobePoint::RemoveNodeFromScene(glbBool isClean)
{
	SetEdit(false);
	if(mpr_isFadingDynamicLabel)
	{
		osg::ref_ptr<osg::NodeCallback> fadingLabelCallback = mpt_node->getUpdateCallback();
		if(fadingLabelCallback.valid())
			mpt_node->removeUpdateCallback(fadingLabelCallback.get());
		mpr_isFadingDynamicLabel = false;
	}
	GlbScopedLock<GlbCriticalSection> lock(mpr_addToScene_critical);
	if(isClean == false)
	{//从显存卸载对象，节约显存.
		if ((mpt_node != NULL))// && (mpt_node->getNumParents() != 0))
		{//obj在显存	
			/*
			     mpt_node 已经在挂队列，但是还没挂到场景树上
				 这时判断getNumParents() != 0 是不可以的.
			*/
			CGlbGlobeCallBack::CGlbGlobeCallBackTask *task = 
				new CGlbGlobeCallBack::CGlbGlobeCallBackTask(this,mpt_node);
			task->mpr_size = this->GetOsgNodeSize();
			mpt_globe->mpr_p_callback->AddRemoveTask(task);
		}
	}
	else
	{//从内存中卸载对象，节约内存
		//删除 上一次装载的节点		
		if (mpt_preNode != NULL)
		{
			//if(mpt_preNode->getNumParents() > 0 )
			{				
				CGlbGlobeCallBack::CGlbGlobeCallBackTask* task = 
					new CGlbGlobeCallBack::CGlbGlobeCallBackTask(this,mpt_preNode);
				task->mpr_size = ComputeNodeSize(mpt_preNode);
				mpt_globe->mpr_p_callback->AddRemoveTask(task);
			}
			mpt_preNode = NULL;
		}
		//删除当前节点
		if (mpt_node != NULL) 
		{
			//if(mpt_node->getNumParents() > 0)
			{		
				CGlbGlobeCallBack::CGlbGlobeCallBackTask* task = 
					new CGlbGlobeCallBack::CGlbGlobeCallBackTask(this,mpt_node);
				task->mpr_size = this->GetOsgNodeSize();
				mpt_globe->mpr_p_callback->AddRemoveTask(task);
			}					
			mpt_node    =  NULL;
			mpr_objSize =  0;
		}		
		//if (mpt_feature != NULL && mpt_featureLayer != NULL)
		//{
		//	CGlbGlobeFeatureLayer* fl = dynamic_cast<CGlbGlobeFeatureLayer*>(mpt_featureLayer.get());
		//	if (fl)
		//		fl->NotifyFeatureDelete(mpt_feature->GetOid());
		//}
		mpt_currLevel = -1;					
	}
}

//现在直接用OperatorVisitor，此函数现在解析endAlpha的值
void GlbGlobe::CGlbGlobePoint::ParseObjectFadeColor()
{
	if(mpr_renderInfo == NULL) return;

	GlbMarkerSymbolInfo *markerInfo = dynamic_cast<GlbMarkerSymbolInfo *>(mpr_renderInfo.get());
	if(markerInfo == NULL) return;
	switch (markerInfo->symType)
	{
	case GLB_MARKERSYMBOL_2DSHAPE:
		{
			GlbMarker2DShapeSymbolInfo *marker2DInfo = dynamic_cast<GlbMarker2DShapeSymbolInfo *>(markerInfo);
			if(marker2DInfo != NULL)
			{
				mpt_fadeEndAlpha = marker2DInfo->fillInfo->opacity->GetValue(mpt_feature.get());
				//mpt_fadeColor = GetColor(marker2DInfo->fillInfo->color->GetValue(mpt_feature.get()));
			}
		}break;
	case GLB_MARKERSYMBOL_3DSHAPE:
		{
			GlbMarker3DShapeSymbolInfo *marker3DInfo = dynamic_cast<GlbMarker3DShapeSymbolInfo *>(markerInfo);
			if(marker3DInfo != NULL)
			{
				mpt_fadeEndAlpha = marker3DInfo->fillInfo->opacity->GetValue(mpt_feature.get());
				//mpt_fadeColor = GetColor(marker3DInfo->fillInfo->color->GetValue(mpt_feature.get()));
			}
		}break;
	case GLB_MARKERSYMBOL_PIXEL:
		{
			GlbMarkerPixelSymbolInfo *pixelInfo = dynamic_cast<GlbMarkerPixelSymbolInfo *>(markerInfo);
			if(pixelInfo != NULL)
			{
				mpt_fadeEndAlpha = pixelInfo->opacity->GetValue(mpt_feature.get());
				//mpt_fadeColor = GetColor(pixelInfo->color->GetValue(mpt_feature.get()));
			}
		}break;
	case GLB_MARKERSYMBOL_IMAGE:
		{
			GlbMarkerImageSymbolInfo *imageInfo = dynamic_cast<GlbMarkerImageSymbolInfo *>(markerInfo);
			if(imageInfo != NULL)
			{
				mpt_fadeEndAlpha = imageInfo->imgInfo->opacity->GetValue(mpt_feature.get());
				//mpt_fadeColor = GetColor(pixelInfo->color->GetValue(mpt_feature.get()));
			}
		}break;
	default:
		break;
	}
}

glbBool GlbGlobe::CGlbGlobePoint::IsFadingDynamicLabel()
{
	return mpr_isFadingDynamicLabel;
}

void GlbGlobe::CGlbGlobePoint::SetFadingDynamicLabel( glbBool isFading )
{
	mpr_isFadingDynamicLabel = isFading;
}

osg::Matrixd GlbGlobe::CGlbGlobePoint::ComputePosOffsetMatrix()
{
	glbref_ptr<GlbMarkerSymbolInfo> markerInfo = dynamic_cast<GlbMarkerSymbolInfo*>(mpr_renderInfo.get());
	if(!markerInfo.valid())
		return osg::Matrix::identity();
	osg::Vec3d offset;
	osg::Matrixd mOffset;
	if (markerInfo->symType == GLB_MARKERSYMBOL_MODEL)
		{
			GlbMarkerModelSymbolInfo *markerModelInfo = 
				(GlbMarkerModelSymbolInfo *)markerInfo.get();
			if (markerModelInfo == NULL) return osg::Matrix::identity();
			if(markerModelInfo->xOffset)
				offset.x() = markerModelInfo->xOffset->GetValue(mpt_feature.get());
			if(markerModelInfo->yOffset)
				offset.y() = markerModelInfo->yOffset->GetValue(mpt_feature.get());
			if(markerModelInfo->zOffset)
				offset.z() = markerModelInfo->zOffset->GetValue(mpt_feature.get());
		}
		else if (markerInfo->symType == GLB_MARKERSYMBOL_2DSHAPE)
		{
			GlbMarker2DShapeSymbolInfo *marker2DShapeInfo = 
				(GlbMarker2DShapeSymbolInfo *)markerInfo.get();
			if(marker2DShapeInfo == NULL) return osg::Matrix::identity();
			if(marker2DShapeInfo->xOffset)
				offset.x() = marker2DShapeInfo->xOffset->GetValue(mpt_feature.get());
			if(marker2DShapeInfo->yOffset)
				offset.y() = marker2DShapeInfo->yOffset->GetValue(mpt_feature.get());
			if(marker2DShapeInfo->zOffset)
				offset.z() = marker2DShapeInfo->zOffset->GetValue(mpt_feature.get());
		}
		else if(markerInfo->symType == GLB_MARKERSYMBOL_3DSHAPE)
		{
			GlbMarker3DShapeSymbolInfo *marker3DShapeInfo = 
				(GlbMarker3DShapeSymbolInfo *)markerInfo.get();
			if(marker3DShapeInfo == NULL) return osg::Matrix::identity();
			if(marker3DShapeInfo->xOffset)
				offset.x() = marker3DShapeInfo->xOffset->GetValue(mpt_feature.get());
			if(marker3DShapeInfo->yOffset)
				offset.y() = marker3DShapeInfo->yOffset->GetValue(mpt_feature.get());
			if(marker3DShapeInfo->zOffset)
				offset.z() = marker3DShapeInfo->zOffset->GetValue(mpt_feature.get());
		}

		if(mpt_globe->GetType() == GLB_GLOBETYPE_GLOBE)
		{
			double longitude = osg::DegreesToRadians(mpr_xOrLon);
			double latitude = osg::DegreesToRadians(mpr_yOrLat);

			// Compute up vector
			osg::Vec3d    up      ( cos(longitude)*cos(latitude), sin(longitude)*cos(latitude), sin(latitude));
			// Compute east vector
			osg::Vec3d    east    (-sin(longitude), cos(longitude), 0);
			// Compute north  vector = outer product up x east
			osg::Vec3d    north   = up ^ east;

			north.normalize();
			osg::Vec3 _Offset = east * offset.x() + north * offset.y() + up * offset.z();
			mOffset.makeTranslate(_Offset);
		}
		else if(mpt_globe->GetType() == GLB_GLOBETYPE_FLAT)
		{
			mOffset.makeTranslate(offset.x(),offset.y(),offset.z());
		}
		return mOffset;
}

glbBool GlbGlobe::CGlbGlobePoint::Load2( xmlNodePtr* node,const glbWChar* prjPath )
{
	xmlChar* szKey = NULL;
	xmlNodePtr pnode = *node;
	CGlbGlobeRObject::Load2(node,prjPath);
	xmlNodePtr childNode = NULL;
	xmlNodePtr groupNode = pnode->parent;
	pnode = pnode->next;//symType;

	szKey = pnode->xmlChildrenNode->content;
	char* str = u2g_obj((char*)szKey);
	CGlbString symType = (char*)str;

	CGlbString bUse;
	CGlbWString field;
	CGlbString value;

	if(symType == "NGLB_MARKERSYMBOL_MODEL")
	{
		GlbMarkerModelSymbolInfo* modeinfo = new GlbMarkerModelSymbolInfo();
		pnode = pnode->next;//text;
		modeinfo->Load2(pnode);
		mpr_renderInfo = modeinfo;
	}
	else if(symType == "NGLB_MARKERSYMBOL_2DSHAPE")
	{
		GlbMarker2DShapeSymbolInfo* Shape2DSymbolInfo = new GlbMarker2DShapeSymbolInfo();
		pnode = pnode->next;//text;
		Shape2DSymbolInfo->Load2(pnode);
		mpr_renderInfo = Shape2DSymbolInfo;
	}
	else if(symType == "NGLB_MARKERSYMBOL_3DSHAPE")
	{
		GlbMarker3DShapeSymbolInfo* Shape3DSymbolInfo = new GlbMarker3DShapeSymbolInfo();
		pnode = pnode->next;//text;
		Shape3DSymbolInfo->Load2(pnode);
		mpr_renderInfo = Shape3DSymbolInfo;
	}
	else if(symType == "NGLB_MARKERSYMBOL_PIXEL")
	{
		GlbMarkerPixelSymbolInfo* pixelinfo = new GlbMarkerPixelSymbolInfo();
		pnode = pnode->next;//text;
		pixelinfo->Load2(pnode,prjPath);
		mpr_renderInfo = (GlbRenderInfo*)pixelinfo;
	}
	else if(symType == "NGLB_MARKERSYMBOL_LABEL")
	{
		GlbMarkerLabelSymbolInfo* labelinfo = new GlbMarkerLabelSymbolInfo();
		pnode = pnode->next;//text;
		labelinfo->Load2(pnode,prjPath);
		mpr_renderInfo = (GlbRenderInfo*)labelinfo;
	}
	else if(symType == "NGLB_MARKERSYMBOL_IMAGE")
	{
		GlbMarkerImageSymbolInfo* imageinfo = new GlbMarkerImageSymbolInfo();
		pnode = pnode->next;//text;
		imageinfo->Load2(pnode,prjPath);
		mpr_renderInfo = (GlbRenderInfo*)imageinfo;
	}
	else if(symType == "NGLB_MARKERSYMBOL_FIRE")
	{
		GlbMarkerFireSymbolInfo* fireinfo = new GlbMarkerFireSymbolInfo();
		pnode = pnode->next;//text;
		fireinfo->Load2(pnode,prjPath);
		mpr_renderInfo = (GlbRenderInfo*)fireinfo;
	}
	else if(symType == "NGLB_MARKERSYMBOL_SMOKE")
	{
		GlbMarkerSmokeSymbolInfo* smokeinfo = new GlbMarkerSmokeSymbolInfo();
		pnode = pnode->next;//text;
		smokeinfo->Load2(pnode,prjPath);
		mpr_renderInfo = (GlbRenderInfo*)smokeinfo;
	}
	else if(symType == "NGLB_MARKERSYMBOL_EPICENTRE")
	{
		GlbMarkerEpicentreSymbolInfo *epicentreinfo = new GlbMarkerEpicentreSymbolInfo();
		pnode = pnode->next;//text
		epicentreinfo->Load2(pnode,prjPath);
		mpr_renderInfo = (GlbRenderInfo*)epicentreinfo;
	}
	else if(symType == "NGLB_MARKERSYMBOL_DYNAMICLABEL")
	{
		GlbMarkerDynamicLabelSymbolInfo* labelinfo = new GlbMarkerDynamicLabelSymbolInfo();
		pnode = pnode->next;//text;
		labelinfo->Load2(pnode,prjPath);
		mpr_renderInfo = (GlbRenderInfo*)labelinfo;
	}
	ParseObjectFadeColor();

	groupNode = groupNode->next;//text
	groupNode = groupNode->next;//GetRenderOrder
	if (groupNode->xmlChildrenNode && (!xmlStrcmp(groupNode->xmlChildrenNode->name, (const xmlChar *)"text")))
	{
		szKey = xmlNodeGetContent(groupNode->xmlChildrenNode);	
		sscanf_s((char*)szKey,"%d",&mpt_renderOrder);
		xmlFree(szKey);
	}

	groupNode = groupNode->next;//text
	groupNode = groupNode->next;//GetRObject
	groupNode = groupNode->next;//text
	groupNode = groupNode->next;//GetTooltip

	groupNode = groupNode->next;//text
	groupNode = groupNode->next;//IsBlink
	if (groupNode->xmlChildrenNode && (!xmlStrcmp(groupNode->xmlChildrenNode->name, (const xmlChar *)"text")))
	{
		szKey = xmlNodeGetContent(groupNode->xmlChildrenNode);	
		char* str = u2g_obj((char*)szKey);
		bUse = (char*)str;
		if(bUse == "false")
			mpt_isBlink = false;
		else
			mpt_isBlink = true;
		xmlFree(szKey);
	}

	groupNode = groupNode->next;//text
	groupNode = groupNode->next;//IsEdit
	if (groupNode->xmlChildrenNode && (!xmlStrcmp(groupNode->xmlChildrenNode->name, (const xmlChar *)"text")))
	{
		szKey = xmlNodeGetContent(groupNode->xmlChildrenNode);	
		char* str = u2g_obj((char*)szKey);
		bUse = (char*)str;
		if(bUse == "false")
			mpt_isEdit = false;
		else
			mpt_isEdit = true;
		xmlFree(szKey);
	}

	groupNode = groupNode->next;//text
	groupNode = groupNode->next;//IsGround
	if (groupNode->xmlChildrenNode && (!xmlStrcmp(groupNode->xmlChildrenNode->name, (const xmlChar *)"text")))
	{
		szKey = xmlNodeGetContent(groupNode->xmlChildrenNode);	
		char* str = u2g_obj((char*)szKey);
		bUse = (char*)str;
		if(bUse == "false")
			mpt_isGround = false;
		else
			mpt_isGround = true;
		xmlFree(szKey);
	}

	groupNode = groupNode->next;//text
	groupNode = groupNode->next;//IsSelected
	if (groupNode->xmlChildrenNode && (!xmlStrcmp(groupNode->xmlChildrenNode->name, (const xmlChar *)"text")))
	{
		szKey = xmlNodeGetContent(groupNode->xmlChildrenNode);	
		char* str = u2g_obj((char*)szKey);
		bUse = (char*)str;
		if(bUse == "false")
			mpt_isSelected = false;
		else
			mpt_isSelected = true;
		xmlFree(szKey);
	}

	groupNode = groupNode->next;//text
	groupNode = groupNode->next;//IsShow
	if (groupNode->xmlChildrenNode && (!xmlStrcmp(groupNode->xmlChildrenNode->name, (const xmlChar *)"text")))
	{
		szKey = xmlNodeGetContent(groupNode->xmlChildrenNode);	
		char* str = u2g_obj((char*)szKey);
		bUse = (char*)str;
		if(bUse == "false")
			mpt_isShow = false;
		else
			mpt_isShow = true;
		xmlFree(szKey);
	}

	groupNode = groupNode->next;//text
	groupNode = groupNode->next;//IsUseInstance
	if (groupNode->xmlChildrenNode && (!xmlStrcmp(groupNode->xmlChildrenNode->name, (const xmlChar *)"text")))
	{
		szKey = xmlNodeGetContent(groupNode->xmlChildrenNode);	
		char* str = u2g_obj((char*)szKey);
		bUse = (char*)str;
		if(bUse == "false")
			mpt_isUseInstance = false;
		else
			mpt_isUseInstance = true;
		xmlFree(szKey);
	}

	groupNode = groupNode->next;//text
	groupNode = groupNode->next;//xOrLon
	if (groupNode->xmlChildrenNode && (!xmlStrcmp(groupNode->xmlChildrenNode->name, (const xmlChar *)"text")))
	{
		szKey = xmlNodeGetContent(groupNode->xmlChildrenNode);	
		sscanf_s((char*)szKey,"%lf",&mpr_xOrLon);
		xmlFree(szKey);
	}

	groupNode = groupNode->next;//text
	groupNode = groupNode->next;//yOrLon
	if (groupNode->xmlChildrenNode && (!xmlStrcmp(groupNode->xmlChildrenNode->name, (const xmlChar *)"text")))
	{
		szKey = xmlNodeGetContent(groupNode->xmlChildrenNode);	
		sscanf_s((char*)szKey,"%lf",&mpr_yOrLat);
		xmlFree(szKey);
	}

	groupNode = groupNode->next;//text
	groupNode = groupNode->next;//zOrAlt
	if (groupNode->xmlChildrenNode && (!xmlStrcmp(groupNode->xmlChildrenNode->name, (const xmlChar *)"text")))
	{
		szKey = xmlNodeGetContent(groupNode->xmlChildrenNode);	
		sscanf_s((char*)szKey,"%lf",&mpr_zOrAlt);
		xmlFree(szKey);
	}
	return true;
}