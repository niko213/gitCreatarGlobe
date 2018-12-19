#include "StdAfx.h"
#include "GlbGlobeVisualLineSymbol.h"
#include "GlbGlobeSymbolCommon.h"

#include "CGlbGlobe.h"
#include "GlbGlobeView.h"


#include "GlbLine.h"


#include "osg/Geode"
#include "osg/Geometry"
#include "osgUtil/CullVisitor"
#include "osg/CullFace"
#include "osg/Camera"
#include "osg/MatrixTransform"
#include "osg/BlendFunc"
#include "osg/PolygonMode"
#include "osg/Matrixd"

namespace GlbGlobe
{
	class CVisualLineDepth;
	class CVisualLine;
	class CVisualLineScene:public osg::Group
	{
		friend class CVisualLineDepth;
		friend class CVisualLine;
	public:
		CVisualLineScene(CGlbGlobeRObject *obj,GlbVisualLineSymbolInfo *visualLineInfo,CGlbLine *visualLine);
		bool init();
	protected:
		void traverse(osg::NodeVisitor& nv);
		void preRenderCull( osgUtil::CullVisitor& cv);
		void cull( osgUtil::CullVisitor& cv);
		void postRenderCull( osgUtil::CullVisitor& cv){}
	private:
		bool                              mpr_inited;
		CGlbGlobeRObject                 *_obj;
		GlbVisualLineSymbolInfo          *_visualLineInfo;
		CGlbLine                       *_visualLine;
		osg::ref_ptr<osg::Camera>         mpr_depthCamera;
		osg::ref_ptr<osg::MatrixTransform> mpr_visualLineMT;

		unsigned int                      mpr_normalSceneMask;
		unsigned int                      mpr_visualLineMask;
		unsigned int                      mpr_visualLineDepthMask;

		osg::ref_ptr<osg::StateSet>       mpr_globalStateSet;
		osg::ref_ptr<osg::StateSet>       mpr_visualLineStateSet;

		osg::ref_ptr<CVisualLine>         mpr_visualLine;
		osg::ref_ptr<CVisualLineDepth>    mpr_visualLineDepth;
	};
	class CVisualLineDepth : public osg::Referenced
	{
		friend class CVisualLineScene;
		class CameraCullCallback : public osg::NodeCallback
		{
		public:
			CameraCullCallback(CVisualLineScene *visualLineScene):mpr_inited(false)
			{
				mpr_visualLineDepthRoot=NULL;
				mpr_visualLineScene=visualLineScene;
				mpr_visualLineDepthRoot=mpr_visualLineScene->_obj->GetGlobe()->GetView()->getNormalSceneRootNode();
				if (mpr_visualLineDepthRoot!=NULL)
					mpr_inited=true;
			}
			void operator()(osg::Node*, osg::NodeVisitor* nv)
			{
				if (!mpr_inited)
					return;

				osg::Group *normalRoot=mpr_visualLineScene->mpr_visualLineDepth->getSceneGlobeNode();   
				normalRoot->osg::Group::traverse(*nv); 
			}
		private:
			bool                           mpr_inited;
			CVisualLineScene              *mpr_visualLineScene;
			osg::Group                    *mpr_visualLineDepthRoot;
		};
	public:
		CVisualLineDepth(CVisualLineScene *visualLineScene)
		{
			mpr_visualLineScene=visualLineScene;
			mpr_depthCam=NULL;
			mpr_depthTexture=NULL;
			mpr_globeDepthSset=NULL;
			mpr_NormalSceneRootSset=NULL;
			mpr_NormalSceneRoot=NULL;
		}
		osg::Camera *getOrBuildDepthCamera()
		{
			if (mpr_depthCam.valid())
				return mpr_depthCam.get();

			createDepthCamera();
			
			return mpr_depthCam.get();
		}
		osg::Texture2D *getDepthTex()const{return mpr_depthTexture.get();}
		osg::Matrixf const &getViewM()const{return mpr_ViewMat_Depth;}
		osg::Matrixf const &getProjM()const{return mpr_ProjMat_Depth;}
		osg::StateSet *getSceneGlobeNodeSset()
		{
			if (mpr_NormalSceneRootSset.valid())
				return mpr_NormalSceneRootSset.get();
			
			mpr_NormalSceneRootSset=getSceneGlobeNode()->getOrCreateStateSet();
			return mpr_NormalSceneRootSset.get();
		}
		osg::StateSet *getGlobeDepthSset()
		{
			if (mpr_globeDepthSset.valid())
				return mpr_globeDepthSset.get();
			initGlobeDepthSset();
			return mpr_globeDepthSset.get();
		}
		osg::Group *getSceneGlobeNode()
		{
			if (mpr_NormalSceneRoot.valid())
				return mpr_NormalSceneRoot.get();

			mpr_NormalSceneRoot=mpr_visualLineScene->_obj->GetGlobe()->GetView()->getSceneGlobeNode();

			return mpr_NormalSceneRoot.get();
		}
	private:
		void initGlobeDepthSset()
		{
			osg::StateSet *sset=new osg::StateSet();
			sset->setMode( GL_CULL_FACE, osg::StateAttribute::ON |osg::StateAttribute::OVERRIDE);
			sset->setAttributeAndModes( new osg::CullFace( osg::CullFace::BACK ), osg::StateAttribute::ON|osg::StateAttribute::OVERRIDE);
			sset->setMode(GL_DEPTH_TEST,osg::StateAttribute::OFF| osg::StateAttribute::OVERRIDE);
			sset->setMode(GL_LIGHTING,osg::StateAttribute::OFF| osg::StateAttribute::OVERRIDE);
			sset->setMode(GL_BLEND,osg::StateAttribute::ON| osg::StateAttribute::OVERRIDE);
			sset->setAttributeAndModes( new osg::BlendFunc(GL_ONE, GL_ZERO), osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
			sset->addUniform( new osg::Uniform("WatchPoint", osg::Vec3f(mpr_watchPoint)) );
			osg::ref_ptr<osg::Program> program = createShaderProgram("visualLineDepthShader", getDepthVertexShaderSource(), getDepthFragmentShaderSource(), false);
			if(program.valid())
				sset->setAttributeAndModes(program.get(), osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);

			mpr_globeDepthSset=sset;
		}
		void calCameraPos()
		{
			glbDouble watchX,watchY,watchZ;
			mpr_visualLineScene->_visualLine->GetPoint(0,&watchX,&watchY,&watchZ);
			mpr_watchPoint=osg::Vec3d(watchX,watchY,watchZ);
			mpr_visualLineScene->_visualLine->GetPoint(1,&watchX,&watchY,&watchZ);
			mpr_targetPoint=osg::Vec3d(watchX,watchY,watchZ);

			GlbGlobeTypeEnum globeTypeE=mpr_visualLineScene->_obj->GetGlobe()->GetType();
			glbBool isGlobeMode=GLB_GLOBETYPE_GLOBE==globeTypeE;
			if (isGlobeMode)
			{
				osg::EllipsoidModel myEllipsoidModel(g_ellipsoidModel->getRadiusEquator(),g_ellipsoidModel->getRadiusPolar());
				myEllipsoidModel.convertLatLongHeightToXYZ(osg::DegreesToRadians(mpr_watchPoint.y()),osg::DegreesToRadians(mpr_watchPoint.x()),mpr_watchPoint.z(),
					mpr_watchPoint.x(),mpr_watchPoint.y(),mpr_watchPoint.z());
				myEllipsoidModel.convertLatLongHeightToXYZ(osg::DegreesToRadians(mpr_targetPoint.y()),osg::DegreesToRadians(mpr_targetPoint.x()),mpr_targetPoint.z(),
					mpr_targetPoint.x(),mpr_targetPoint.y(),mpr_targetPoint.z());
			}
			osg::Vec3d up(0.0,0.0,1.0);
			osg::Vec3d center=mpr_targetPoint;
			osg::Vec3d eyeDir=mpr_watchPoint-mpr_targetPoint;
			double depthLen=eyeDir.length();
			eyeDir.normalize();
			//osg::Vec3d eye=mpr_watchPoint+eyeDir*depthLen/10.0; 
			osg::Vec3d eye=mpr_watchPoint;  
			mpr_depthCam->setViewMatrixAsLookAt(eye,center,up);
			mpr_ViewMat_Depth=mpr_depthCam->getViewMatrix();     
			//mpr_depthCam->setProjectionMatrixAsOrtho(-0.1,0.1,-0.1,0.1,depthLen/10.0,depthLen*1.1);  
			mpr_depthCam->setProjectionMatrixAsOrtho(-0.1,0.1,-0.1,0.1,0.0,depthLen); 
			mpr_ProjMat_Depth=mpr_depthCam->getProjectionMatrix();
		}
		void createDepthCamera()
		{
			if (mpr_depthCam.valid())
				return;

			osg::Texture2D *tex=new osg::Texture2D();
			tex->setTextureSize(1, 1);
			tex->setShadowTextureMode(osg::Texture2D::LUMINANCE);
			tex->setInternalFormat( GL_RGBA32F_ARB );
			tex->setSourceFormat( GL_RGBA );
			tex->setSourceType(GL_FLOAT); 
			tex->setFilter( osg::Texture2D::MIN_FILTER, osg::Texture2D::NEAREST );
			tex->setFilter( osg::Texture2D::MAG_FILTER, osg::Texture2D::NEAREST );
			tex->setWrap(osg::Texture2D::WRAP_S,osg::Texture2D::REPEAT);
			tex->setWrap(osg::Texture2D::WRAP_T,osg::Texture2D::REPEAT);
			mpr_depthTexture=tex; 
	
			osg::Camera *cam=new osg::Camera();
			cam->setClearMask(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			cam->setClearColor(osg::Vec4(1.0,1.0,1.0,1.0));

			cam->setComputeNearFarMode(osg::Camera::DO_NOT_COMPUTE_NEAR_FAR); 
			cam->setCullMask( ~mpr_visualLineScene->mpr_visualLineMask ); 
			cam->setReferenceFrame( osg::Transform::ABSOLUTE_RF );
			cam->setViewport( 0, 0, tex->getTextureWidth(), tex->getTextureHeight() );
			cam->setRenderOrder(osg::Camera::PRE_RENDER);
			cam->setRenderTargetImplementation(osg::Camera::FRAME_BUFFER_OBJECT);
			cam->attach( osg::Camera::COLOR_BUFFER, mpr_depthTexture.get() );  

			

			mpr_depthCam=cam;


			calCameraPos();
			initGlobeDepthSset();


			osg::Group *pGlobeNode=mpr_visualLineScene->_obj->GetGlobe()->GetView()->getSceneGlobeNode();

			osg::MatrixTransform *depthNode=new osg::MatrixTransform(); 
			depthNode->setStateSet(mpr_globeDepthSset);
			depthNode->addChild(pGlobeNode);

			cam->addChild(depthNode);
		}
		void createDepthCamera0()
		{
			if (mpr_depthCam.valid())
				return;

			osg::Texture2D *tex=new osg::Texture2D();
			tex->setTextureSize(1, 1);
			tex->setShadowTextureMode(osg::Texture2D::LUMINANCE);
			tex->setInternalFormat( GL_DEPTH_COMPONENT24 );
			tex->setSourceFormat( GL_DEPTH_COMPONENT );
			tex->setSourceType(GL_FLOAT);
			tex->setFilter( osg::Texture2D::MIN_FILTER, osg::Texture2D::LINEAR );
			tex->setFilter( osg::Texture2D::MAG_FILTER, osg::Texture2D::LINEAR );

			mpr_depthTexture=tex; 

			osg::Camera *cam=new osg::Camera();
			cam->setClearMask(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			//cam->setClearDepth(1.0); 
			cam->setClearColor(osg::Vec4(1.0,1.0,1.0,1.0));

			//cam->setComputeNearFarMode(osg::Camera::DO_NOT_COMPUTE_NEAR_FAR);
			cam->setCullMask( ~mpr_visualLineScene->mpr_visualLineMask ); 
			//cam->setReferenceFrame( osg::Transform::ABSOLUTE_RF );	 	   
			cam->setReferenceFrame( osg::Transform::ABSOLUTE_RF_INHERIT_VIEWPOINT );
			cam->setViewport( 0, 0, 1, 1 );
			cam->setRenderOrder(osg::Camera::PRE_RENDER);
			cam->setRenderTargetImplementation(osg::Camera::FRAME_BUFFER_OBJECT);
			cam->attach( osg::Camera::DEPTH_BUFFER, mpr_depthTexture.get() );  



			mpr_depthCam=cam;


			calCameraPos();
			initGlobeDepthSset();


			osg::Group *pGlobeNode=mpr_visualLineScene->_obj->GetGlobe()->GetView()->getSceneGlobeNode();

			osg::MatrixTransform *depthNode=new osg::MatrixTransform(); 
			depthNode->setReferenceFrame(osg::Transform::ABSOLUTE_RF); 
			//depthNode->setStateSet(mpr_globeDepthSset);     
			depthNode->addChild(pGlobeNode);

			cam->addChild(depthNode);
		}
		CVisualLineScene                 *mpr_visualLineScene;

		osg::Vec3d                        mpr_watchPoint;
		osg::Vec3d                        mpr_targetPoint;
		osg::Matrixf                      mpr_ViewMat_Depth;
		osg::Matrixf                      mpr_ProjMat_Depth;
		osg::ref_ptr<osg::Camera>         mpr_depthCam;
		osg::ref_ptr<osg::Texture2D>      mpr_depthTexture;
		osg::ref_ptr<osg::StateSet>       mpr_globeDepthSset;
		osg::ref_ptr<osg::StateSet>       mpr_NormalSceneRootSset;
		osg::ref_ptr<osg::Group>          mpr_NormalSceneRoot;
	};
	class CVisualLine : public osg::Referenced
	{
	public:
		CVisualLine(CVisualLineScene *visualLineScene)
		{
			mpr_visualLineScene=visualLineScene;
		}
		osg::MatrixTransform *getOrBuildVisualLine()
		{
			if (mpr_visualLineMT.valid())
				return mpr_visualLineMT.get();

			createVisualLine();

			return mpr_visualLineMT.get();
		}
	private:
		void createVisualLineSymbolInfo(osg::StateSet *&sset,CGlbGlobeRObject *obj,GlbVisualLineSymbolInfo *visualLineInfo)
		{
			sset->setMode( GL_CULL_FACE, osg::StateAttribute::OFF);
			sset->setMode(GL_BLEND,osg::StateAttribute::ON);
			osg::ref_ptr<osg::BlendFunc> pBlendFun = new osg::BlendFunc(osg::BlendFunc::SRC_ALPHA,osg::BlendFunc::ONE_MINUS_SRC_ALPHA );	//设置混合方程
			sset->setAttributeAndModes(pBlendFun.get(),osg::StateAttribute::ON/*|osg::StateAttribute::OVERRIDE*/);
			sset->setRenderBinDetails( obj->GetRenderOrder()+1, "RenderBin"); 
			sset->setTextureAttributeAndModes(0, mpr_visualLineScene->mpr_visualLineDepth->getDepthTex(), osg::StateAttribute::ON );
			sset->addUniform( new osg::Uniform("VisualLine_DepthMap", 0) );
			sset->addUniform( new osg::Uniform("ViewMatrix_DepthCam", mpr_visualLineScene->mpr_visualLineDepth->getViewM()) );
			sset->addUniform( new osg::Uniform("ProjectionMatrix_DepthCam", mpr_visualLineScene->mpr_visualLineDepth->getProjM()) );
			

			/*sset->addUniform( new osg::Uniform("PolygonSelected", false) );
			sset->addUniform( new osg::Uniform("PolygonBlink", false) );*/
			// 1、透明
			glbInt32 visibleLineOpacity=100;
			if (visualLineInfo->opacity)
			{
				visibleLineOpacity=visualLineInfo->opacity->value;
			}
			if (visibleLineOpacity!=100)//透明
				sset->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
			else
				sset->setRenderingHint(osg::StateSet::OPAQUE_BIN);
			sset->addUniform( new osg::Uniform("visibleLineOpacity", visibleLineOpacity ) );

			//2、颜色
			osg::Vec4f visibleLineColor(0.0,255.0,0.0,255.0);
			if (visualLineInfo->visibleLineColor)
			{
				glbInt32 plyClr=visualLineInfo->visibleLineColor->value;
				visibleLineColor.set(GetRValueInBGRA(plyClr),LOBYTE(((plyClr & 0xFFFF)) >> 8),GetBValueInBGRA(plyClr),255.0);
			}
			sset->addUniform( new osg::Uniform("visibleLineColor", visibleLineColor) );
			osg::Vec4f invisibleLineColor(255.0,0.0,0.0,255.0);
			if (visualLineInfo->invisibleLineColor)
			{
				glbInt32 plyClr=visualLineInfo->invisibleLineColor->value;
				invisibleLineColor.set(GetRValueInBGRA(plyClr),LOBYTE(((plyClr & 0xFFFF)) >> 8),GetBValueInBGRA(plyClr),255.0);
			}
			sset->addUniform( new osg::Uniform("invisibleLineColor", invisibleLineColor) );
			
			HMODULE hmd = GetModuleHandle(L"GlbGlobe.dll");
			CGlbWString workdir = CGlbPath::GetModuleDir(hmd);
			CGlbWString visualLineVertexShader =  workdir +L"\\res\\visualLine.vert"; 
			CGlbWString visualLineFragmentShader = workdir +L"\\res\\visualLine.frag";
			osg::ref_ptr<osg::Program> program = createShaderProgram("visualLineShader", visualLineVertexShader.ToString(), visualLineFragmentShader.ToString(), true);
			if(program.valid())
				sset->setAttributeAndModes(program.get(), osg::StateAttribute::ON );	
		}
		void createVisualLine()
		{
			if (mpr_visualLineMT.valid())
				return;
			osg::ref_ptr<osg::Vec3dArray> points=new osg::Vec3dArray();
			glbDouble watchX,watchY,watchZ;
			mpr_visualLineScene->_visualLine->GetPoint(0,&watchX,&watchY,&watchZ);
			points->push_back(osg::Vec3d(watchX,watchY,watchZ));
			mpr_visualLineScene->_visualLine->GetPoint(1,&watchX,&watchY,&watchZ);
			points->push_back(osg::Vec3d(watchX,watchY,watchZ));

			glbUInt32 uniqueCnt=DelaunayTriangulator_uniqueifyPoints(points);// 判断多边形是否有效
			if (uniqueCnt!=2)
				return;

			// 计算多边形位置和方向矩阵
			GlbGlobeTypeEnum globeTypeE=mpr_visualLineScene->_obj->GetGlobe()->GetType();
			glbBool isGlobeMode=GLB_GLOBETYPE_GLOBE==globeTypeE;
			glbDouble cetX,cetY,cetZ;
			mpr_visualLineScene->_visualLine->GetPoint(0,&cetX,&cetY,&cetZ);
			osg::EllipsoidModel myEllipsoidModel(g_ellipsoidModel->getRadiusEquator(),g_ellipsoidModel->getRadiusPolar());
			osg::Matrixd polygonOffset;
			polygonOffset.makeIdentity();
			if (isGlobeMode)
				myEllipsoidModel.computeLocalToWorldTransformFromLatLongHeight(osg::DegreesToRadians(cetY),osg::DegreesToRadians(cetX),cetZ,polygonOffset);
			else
				polygonOffset.makeTranslate(cetX,cetY,cetZ);

			osg::Matrixd polygonTransOffset;//平移矩阵
			polygonTransOffset.makeTranslate(polygonOffset.getTrans());
			polygonTransOffset.makeIdentity();//禁止偏移
			osg::Matrixd polygonRotOffset=polygonOffset;//方向矩阵  
			polygonRotOffset.setTrans(0.0,0.0,0.0);
			polygonRotOffset.makeIdentity();//禁止旋转
			osg::Matrixd polygonRotOffset_inv=osg::Matrixd::inverse(polygonRotOffset);

			osg::Vec3d transOffset=polygonTransOffset.getTrans();
			glbDouble newX,newY,newZ;
			if (isGlobeMode)
			{
				for (osg::Vec3Array::size_type i=0;i<2;++i)
				{
					const osg::Vec3d &iPnt=points->at(i);
					myEllipsoidModel.convertLatLongHeightToXYZ(osg::DegreesToRadians(iPnt.y()),osg::DegreesToRadians(iPnt.x()),iPnt.z(),
						newX,newY,newZ);
					osg::Vec3d pos(newX,newY,newZ);
					pos-=transOffset;
					points->at(i)=pos*polygonRotOffset_inv;
				}
			}
			else
			{
				for (osg::Vec3Array::size_type i=0;i<2;++i)
				{
					const osg::Vec3d &iPnt=points->at(i);
					osg::Vec3d pos=iPnt-transOffset;
					points->at(i)=pos*polygonRotOffset_inv;
				}
			}

			osg::Geometry *geom=new osg::Geometry();
			geom->setVertexArray(points);
			geom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINES,0,2));
			osg::StateSet *sset=geom->getOrCreateStateSet();
			createVisualLineSymbolInfo(sset,mpr_visualLineScene->_obj,mpr_visualLineScene->_visualLineInfo);

			osg::Geode *polygonGeode = new osg::Geode();
			polygonGeode->addDrawable(geom);

			osg::Switch *pPolyGonSW=new osg::Switch();
			pPolyGonSW->setNodeMask(mpr_visualLineScene->mpr_visualLineMask);
			pPolyGonSW->addChild(polygonGeode);


			osg::MatrixTransform *mt = new osg::MatrixTransform();
			mt->setNodeMask(mpr_visualLineScene->mpr_normalSceneMask | mpr_visualLineScene->mpr_visualLineMask);
			mt->setMatrix(polygonRotOffset*polygonTransOffset);
			mt->addChild(pPolyGonSW);

			mpr_visualLineMT=mt;
		}

		CVisualLineScene                 *mpr_visualLineScene;

		osg::ref_ptr<osg::MatrixTransform> mpr_visualLineMT;
	};

	CVisualLineScene::CVisualLineScene(CGlbGlobeRObject *obj,GlbVisualLineSymbolInfo *visualLineInfo,CGlbLine *visualLine)
	{
		mpr_inited=false;
		_obj=obj;
		_visualLineInfo=visualLineInfo;
		_visualLine=visualLine;
		mpr_normalSceneMask=getNodeMask();
		mpr_visualLineDepthMask=16;
		mpr_normalSceneMask=32;
		mpr_visualLineMask=64;
		this->setNodeMask(mpr_visualLineMask);
		mpr_visualLine=new CVisualLine(this);
		mpr_visualLineDepth=new CVisualLineDepth(this);
	}
	bool CVisualLineScene::init()
	{
		if (mpr_inited)
			return true;

		
		osg::Camera *cam=mpr_visualLineDepth->getOrBuildDepthCamera();
		if (cam==NULL)
			return false;

		osg::MatrixTransform *mt=mpr_visualLine->getOrBuildVisualLine();
		if (mt==NULL)
			return false;
		mpr_visualLineMT=mt;



		mpr_globalStateSet=new osg::StateSet();

		mpr_visualLineStateSet=new osg::StateSet();
		mpr_visualLineStateSet->setMode( GL_CULL_FACE, osg::StateAttribute::OFF);


		addChild(mt);


		osg::Group *pSceneRootNode=_obj->GetGlobe()->GetView()->getSceneRootNode();  
		pSceneRootNode->addChild(cam);


		mpr_inited=true;
		return mpr_inited;
	}
	void CVisualLineScene::traverse(osg::NodeVisitor& nv)
		{
			if (!mpr_inited)
				return;

			return Group::traverse(nv);

			osg::NodeVisitor::VisitorType vtt=nv.getVisitorType();
			/*if (vtt==osg::NodeVisitor::UPDATE_VISITOR)
			{
			osg::Group::traverse(nv);
			} 
			else */if (vtt==osg::NodeVisitor::CULL_VISITOR)
			{
				osgUtil::CullVisitor* cv = dynamic_cast<osgUtil::CullVisitor*>(&nv);
				if (cv!=NULL) 
				{
					if (cv->getCurrentCamera()->getName() == "ShadowCamera" ||
						cv->getCurrentCamera()->getName() == "AnalysisCamera" )
						// Do not do reflections and everything if we're in a shadow pass. 
						osg::Group::traverse(nv);
					else
					{
						//preRenderCull(*cv);
						cull(*cv);
						postRenderCull(*cv);
					}
				}
				else
					osg::Group::traverse(nv);
			}
			else
				osg::Group::traverse(nv);
		}
	void CVisualLineScene::preRenderCull( osgUtil::CullVisitor& cv)
	{
		osg::StateSet *oriSset=mpr_visualLineDepth->getSceneGlobeNodeSset();
		osg::Group *normalRoot=mpr_visualLineDepth->getSceneGlobeNode();   
		normalRoot->setStateSet(mpr_visualLineDepth->getGlobeDepthSset());

		osg::Camera *depthCam=mpr_visualLineDepth->getOrBuildDepthCamera();
		cv.pushStateSet(mpr_globalStateSet.get());
		depthCam->accept( cv );
		cv.popStateSet();
		
		normalRoot->setStateSet(oriSset);
	}
	void CVisualLineScene::cull( osgUtil::CullVisitor& cv)
	{
		/*mpr_visualLineStateSet->getUniform("ViewMatrix_DepthCam")->set(mpr_visualLineDepth->getViewM());
		mpr_visualLineStateSet->getUniform("ProjectionMatrix_DepthCam")->set( mpr_visualLineDepth->getProjM() );*/

		unsigned int mask=cv.getTraversalMask(); 
		cv.pushStateSet(mpr_globalStateSet);

		{
			for (unsigned int i = 0; i < getNumChildren(); ++i)
			{
				osg::Node* child = getChild(i);
				if (child != mpr_visualLineMT.get())
					child->setNodeMask((child->getNodeMask() & ~mpr_visualLineMask) | mpr_normalSceneMask);
			}

			// render visualline 
			cv.pushStateSet( mpr_visualLineStateSet.get() );
			cv.setTraversalMask( mask & mpr_visualLineMask );
			osg::Group::traverse(cv);
			// pop mpr_visualLineStateSet
			cv.popStateSet();
		}

		// render rest of scene
		cv.setTraversalMask( mask & mpr_normalSceneMask );
		osg::Group::traverse(cv);

		// pop mpr_globalStateSet
		cv.popStateSet(); 

		// put original mask back
		cv.setTraversalMask( mask );
	}
	CGlbGlobeVisualLineSymbol::CGlbGlobeVisualLineSymbol(void){}

	CGlbGlobeVisualLineSymbol::~CGlbGlobeVisualLineSymbol(void){}

	osg::Node * CGlbGlobeVisualLineSymbol::Draw( CGlbGlobeRObject *obj,IGlbGeometry *geo )
	{
		if (obj==NULL||geo==NULL)
			return NULL;
		GlbRenderInfo *renderInfo = obj->GetRenderInfo(); 
		if (renderInfo==NULL)
			return NULL;
		GlbVisualLineSymbolInfo *visualLineInfo= dynamic_cast<GlbVisualLineSymbolInfo *>(renderInfo);
		if (visualLineInfo==NULL)
			return NULL;
		CGlbLine *pLine=dynamic_cast<CGlbLine *>(geo);
		if (pLine==NULL)
			return NULL;


		osg::ref_ptr<CVisualLineScene> visualLineScene=new CVisualLineScene(obj,visualLineInfo,pLine);
		if (visualLineScene->init())
			return visualLineScene.release();
		return NULL;
	}

	void CGlbGlobeVisualLineSymbol::DrawToImage( CGlbGlobeRObject *obj,IGlbGeometry* geom, glbByte *image,glbInt32 imageW, glbInt32 imageH,CGlbExtent &ext )
	{
		return;
	}

	IGlbGeometry * CGlbGlobeVisualLineSymbol::GetOutline( CGlbGlobeRObject *obj,IGlbGeometry *geo )
	{
		return NULL;
	}

}
