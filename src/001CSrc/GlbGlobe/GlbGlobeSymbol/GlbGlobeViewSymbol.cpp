#include "StdAfx.h"
#include "GlbGlobeViewSymbol.h"
#include "GlbGlobeSymbolCommon.h"

#include "CGlbGlobe.h"
#include "GlbGlobeView.h"
#include "GlbGlobeSceneManager.h"
#include "GlbGlobeObjectRenderInfos.h"


#include "GlbLine.h"


#include "osg/Geode"
#include "osg/Geometry"
#include "osgUtil/CullVisitor"
#include "osg/CullFace"
#include "osg/Camera"
#include "osg/Material"
#include "osg/Uniform"
#include "osg/TexGen"
#include "osg/MatrixTransform"
#include "osg/BlendFunc"
#include "osg/Depth"
#include "osg/PolygonMode"
#include "osg/Matrixd"
#include "osg/PolygonOffset"


namespace GlbGlobe
{
	class CView: public osg::Group
	{
		class CameraCullCallback : public osg::NodeCallback
		{
		public:
			CameraCullCallback(CView* vs)
			{
				_view=vs;
				mpr_globeNode=_view->_obj->GetGlobe()->GetView()->getSceneGlobeNode();
			}
			virtual void operator()(osg::Node*, osg::NodeVisitor* nv)
			{
				mpr_globeNode->osg::Group::traverse(*nv);
			}
		protected:
			CView * _view;
			osg::Group *mpr_globeNode;
		};		
	public:
		CView(CGlbGlobeRObject *obj,GlbViewSymbolInfo *viewInfo,CGlbLine *visualLine)
		{
			mpr_inited=false;
			_obj=obj;
			_viewInfo=viewInfo;
			_visualLine=visualLine;

			mpr_sceneMgr=obj->GetGlobe()->GetView()->getSceneManager();

			mpr_normalSceneMask=getNodeMask();
			mpr_normalSceneMask=32;
			mpr_viewMask=64;
			this->setNodeMask(mpr_viewMask);

			mpr_shadowTextureUnit=1;
		}
		~CView()
		{
		}
		bool init()
		{
			if (mpr_inited)
				return true;
			
			createView();
			createVisualLine(); 


			mpr_texgen= new osg::TexGen;

			mpr_sceneMgr->setFunType(GlbGlobe::CGlbGlobeSceneManager::FT_ViewAnalysis);

			initDepthCamera();

			mpr_inited=true;
			return mpr_inited;
		}
	private:
		void createViewSymbolInfo(osg::StateSet *&sset,CGlbGlobeRObject *obj,GlbViewSymbolInfo *viewInfo)
		{
			//return;
			sset->setMode(GL_BLEND,osg::StateAttribute::ON);
			osg::ref_ptr<osg::BlendFunc> pBlendFun = new osg::BlendFunc(osg::BlendFunc::ONE,osg::BlendFunc::ZERO);
			sset->setAttributeAndModes(pBlendFun.get(),osg::StateAttribute::ON| osg::StateAttribute::PROTECTED);
			sset->setRenderBinDetails( obj->GetRenderOrder()+1, "RenderBin"); 

			sset->setMode( GL_LIGHTING, osg::StateAttribute::ON | osg::StateAttribute::PROTECTED);
			osg::ref_ptr<osg::Material> material = dynamic_cast<osg::Material *>(sset->getAttribute(osg::StateAttribute::MATERIAL));
			if(material == NULL)
				material = new osg::Material;
			material->setDiffuse(osg::Material::FRONT_AND_BACK,osg::Vec4(0,0,0,1));
			material->setAmbient(osg::Material::FRONT_AND_BACK,osg::Vec4(0,0,0,1));
			material->setSpecular(osg::Material::FRONT_AND_BACK,osg::Vec4(0,0,0,1));
			material->setEmission(osg::Material::FRONT_AND_BACK,osg::Vec4(1,1,0,1));
			sset->setAttribute(material,osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);

			osg::ref_ptr<osg::Program> program = new osg::Program();
			if(program.valid())
				sset->setAttributeAndModes(program.get(), osg::StateAttribute::ON  | osg::StateAttribute::PROTECTED);
		}
		void createView()
		{
			if (mpr_viewMT.valid())
				return;
			osg::ref_ptr<osg::Vec3dArray> points=new osg::Vec3dArray();
			glbDouble watchX,watchY,watchZ;
			_visualLine->GetPoint(0,&watchX,&watchY,&watchZ);
			points->push_back(osg::Vec3d(watchX,watchY,watchZ));
			_visualLine->GetPoint(1,&watchX,&watchY,&watchZ);
			points->push_back(osg::Vec3d(watchX,watchY,watchZ));

			glbUInt32 uniqueCnt=DelaunayTriangulator_uniqueifyPoints(points);// 判断多边形是否有效
			if (uniqueCnt!=2)
				return;			

			osg::Vec3d watchPos=points->at(0);
			osg::Vec3d targetPos=points->at(1);

			osg::EllipsoidModel myEllipsoidModel(g_ellipsoidModel->getRadiusEquator(),g_ellipsoidModel->getRadiusPolar());

			GlbGlobeTypeEnum globeTypeE=_obj->GetGlobe()->GetType();
			glbBool isGlobeMode=GLB_GLOBETYPE_GLOBE==globeTypeE;
			if (isGlobeMode)
			{
				myEllipsoidModel.convertLatLongHeightToXYZ(osg::DegreesToRadians(watchPos.y()),osg::DegreesToRadians(watchPos.x()),watchPos.z(),
					watchPos.x(),watchPos.y(),watchPos.z());	
				myEllipsoidModel.convertLatLongHeightToXYZ(osg::DegreesToRadians(targetPos.y()),osg::DegreesToRadians(targetPos.x()),targetPos.z(),
					targetPos.x(),targetPos.y(),targetPos.z());
				points->at(0)=watchPos;
				points->at(1)=targetPos;
			}		

			osg::Vec3d visualDir=targetPos-watchPos;
			double visulLineLen=visualDir.length();
			osg::Vec3d upDir=watchPos;
			upDir.normalize();
			visualDir.normalize();
			osg::Vec3d leftDir=upDir ^ visualDir; leftDir.normalize();
			upDir=visualDir ^ leftDir;upDir.normalize();

			double fovy_degree=30;			double fovx_degree=30;
			if (_viewInfo!=NULL&&_viewInfo->fovIndegree!=NULL)
			{
				fovy_degree=_viewInfo->fovIndegree->value;
				fovx_degree=fovy_degree;
			}
			double halfH=visulLineLen*tan(osg::DegreesToRadians(fovy_degree/2.0));
			double halfW=visulLineLen*tan(osg::DegreesToRadians(fovx_degree/2.0));

			osg::DrawElementsUShort* vtxIdx = new osg::DrawElementsUShort( GL_LINES );
			vtxIdx->reserve(16);
			{//左上角和右上角点
				osg::Vec3d upCenterPos=targetPos+upDir*halfH;
				osg::Vec3d upLeftPos=upCenterPos+leftDir*halfW;
				osg::Vec3d upRightPos=upCenterPos-leftDir*halfW;
				points->pop_back();
				points->push_back(upLeftPos);
				points->push_back(upRightPos);

				vtxIdx->push_back(0);vtxIdx->push_back(1);
				vtxIdx->push_back(0);vtxIdx->push_back(2);
			}
			{//右下角和左下角点
				osg::Vec3d downCenterPos=targetPos-upDir*halfH;
				osg::Vec3d downLeftPos=downCenterPos+leftDir*halfW;
				osg::Vec3d downRightPos=downCenterPos-leftDir*halfW;
				points->push_back(downRightPos);
				points->push_back(downLeftPos);
				vtxIdx->push_back(0);vtxIdx->push_back(3);
				vtxIdx->push_back(0);vtxIdx->push_back(4);
			}
			vtxIdx->push_back(1);vtxIdx->push_back(2);
			vtxIdx->push_back(2);vtxIdx->push_back(3);
			vtxIdx->push_back(3);vtxIdx->push_back(4);
			vtxIdx->push_back(4);vtxIdx->push_back(1);

			// 计算多边形位置和方向矩阵			
			glbDouble cetX,cetY,cetZ;
			_visualLine->GetPoint(0,&cetX,&cetY,&cetZ);			
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
				for (osg::Vec3Array::size_type i=0;i<5;++i)
				{
					const osg::Vec3d &iPnt=points->at(i);
					/*myEllipsoidModel.convertLatLongHeightToXYZ(osg::DegreesToRadians(iPnt.y()),osg::DegreesToRadians(iPnt.x()),iPnt.z(),
						newX,newY,newZ);
					osg::Vec3d pos(newX,newY,newZ);*/
					osg::Vec3d pos=iPnt;
					pos-=transOffset;
					points->at(i)=pos*polygonRotOffset_inv;
				}
			}
			else
			{
				for (osg::Vec3Array::size_type i=0;i<5;++i)
				{
					const osg::Vec3d &iPnt=points->at(i);
					osg::Vec3d pos=iPnt-transOffset;
					points->at(i)=pos*polygonRotOffset_inv;
				}
			}

			osg::Geometry *geom=new osg::Geometry();
			geom->setVertexArray(points);
			osg::ref_ptr<osg::Vec4Array> colorArray = new osg::Vec4Array();
			colorArray->push_back(osg::Vec4(1.0,1.0,0.0,1.0));
			geom->setColorArray(colorArray);
			geom->setColorBinding(osg::Geometry::BIND_OVERALL);
			//geom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINE_STRIP,0,6));
			geom->addPrimitiveSet(vtxIdx);
			osg::StateSet *sset=geom->getOrCreateStateSet();
			createViewSymbolInfo(sset,_obj,NULL);

			osg::Geode *polygonGeode = new osg::Geode();
			polygonGeode->addDrawable(geom);

			osg::Switch *pPolyGonSW=new osg::Switch();
			pPolyGonSW->addChild(polygonGeode);


			osg::MatrixTransform *mt = new osg::MatrixTransform();
			mt->setMatrix(polygonRotOffset*polygonTransOffset);
			mt->addChild(pPolyGonSW);

			mpr_viewMT=mt;

			this->addChild(mpr_viewMT);
		}
		void createVisualLineSymbolInfo(osg::StateSet *&sset,CGlbGlobeRObject *obj,GlbVisualLineSymbolInfo *visualLineInfo)
		{
			return;
			sset->setMode(GL_BLEND,osg::StateAttribute::ON| osg::StateAttribute::PROTECTED);
			osg::ref_ptr<osg::BlendFunc> pBlendFun = new osg::BlendFunc(osg::BlendFunc::ONE,osg::BlendFunc::ZERO);	//设置混合方程
			sset->setAttributeAndModes(pBlendFun.get(),osg::StateAttribute::ON|osg::StateAttribute::PROTECTED);
			sset->setRenderBinDetails( obj->GetRenderOrder()+1, "RenderBin"); 
			

			//sset->setMode( GL_CULL_FACE, osg::StateAttribute::OFF |osg::StateAttribute::PROTECTED);
			//sset->setAttributeAndModes( new osg::CullFace( osg::CullFace::BACK ), osg::StateAttribute::ON|osg::StateAttribute::PROTECTED);
			//sset->setMode(GL_DEPTH_TEST,osg::StateAttribute::OFF| osg::StateAttribute::PROTECTED);
			sset->setMode(GL_LIGHTING,osg::StateAttribute::ON| osg::StateAttribute::PROTECTED);		
			osg::ref_ptr<osg::Material> material = dynamic_cast<osg::Material *>(sset->getAttribute(osg::StateAttribute::MATERIAL));
			if(material == NULL)
				material = new osg::Material;
			material->setDiffuse(osg::Material::FRONT_AND_BACK,osg::Vec4(0,0,0,1));
			material->setAmbient(osg::Material::FRONT_AND_BACK,osg::Vec4(0,0,0,1));
			material->setSpecular(osg::Material::FRONT_AND_BACK,osg::Vec4(0,0,0,1));
			material->setEmission(osg::Material::FRONT_AND_BACK,osg::Vec4(1,1,0,1));
			sset->setAttribute(material,osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
		}
		void createVisualLine()
		{
			if (mpr_visualLineMT.valid())
				return;
			osg::ref_ptr<osg::Vec3dArray> points=new osg::Vec3dArray();
			glbDouble watchX,watchY,watchZ;
			_visualLine->GetPoint(0,&watchX,&watchY,&watchZ);
			points->push_back(osg::Vec3d(watchX,watchY,watchZ));
			_visualLine->GetPoint(1,&watchX,&watchY,&watchZ);
			points->push_back(osg::Vec3d(watchX,watchY,watchZ));

			glbUInt32 uniqueCnt=DelaunayTriangulator_uniqueifyPoints(points);// 判断多边形是否有效
			if (uniqueCnt!=2)
				return;

			// 计算多边形位置和方向矩阵
			GlbGlobeTypeEnum globeTypeE=_obj->GetGlobe()->GetType();
			glbBool isGlobeMode=GLB_GLOBETYPE_GLOBE==globeTypeE;
			glbDouble cetX,cetY,cetZ;
			_visualLine->GetPoint(0,&cetX,&cetY,&cetZ);
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
			createVisualLineSymbolInfo(sset,_obj,NULL);

			osg::Geode *polygonGeode = new osg::Geode();
			polygonGeode->addDrawable(geom);

			osg::Switch *pPolyGonSW=new osg::Switch();
			pPolyGonSW->addChild(polygonGeode);


			osg::MatrixTransform *mt = new osg::MatrixTransform();
			mt->setMatrix(polygonRotOffset*polygonTransOffset);
			mt->addChild(pPolyGonSW);

			mpr_visualLineMT=mt;

			this->addChild(mpr_visualLineMT);
		}
		bool initDepthCamera()
		{
			if (mpr_depthCamera.valid())
				return true;

			/*osg::Texture2D *tex=new osg::Texture2D();
			tex->setTextureSize(1024, 1024);
			tex->setShadowTextureMode(osg::Texture2D::LUMINANCE);
			tex->setShadowTextureMode(osg::Texture2D::LUMINANCE);
			tex->setInternalFormat( GL_RGBA32F_ARB );
			tex->setSourceFormat( GL_RGBA );
			tex->setSourceType(GL_FLOAT); 
			tex->setFilter( osg::Texture2D::MIN_FILTER, osg::Texture2D::NEAREST );
			tex->setFilter( osg::Texture2D::MAG_FILTER, osg::Texture2D::NEAREST );
			tex->setWrap(osg::Texture2D::WRAP_S,osg::Texture2D::CLAMP);
			tex->setWrap(osg::Texture2D::WRAP_T,osg::Texture2D::CLAMP);
			tex->setBorderColor(osg::Vec4(1.0f,1.0f,1.0f,1.0f));
			mpr_depthTexture=tex; 

			osg::Camera *cam=new osg::Camera();
			cam->setClearMask(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			cam->setClearColor(osg::Vec4(1.0,1.0,1.0,1.0));

			cam->setCullCallback(new CameraCullCallback(this));
			cam->setCullMask( ~mpr_viewMask ); 
			cam->setCullingMode(osg::CullSettings::VIEW_FRUSTUM_CULLING);
			cam->setComputeNearFarMode(osg::Camera::DO_NOT_COMPUTE_NEAR_FAR); 
			cam->setViewport( 0, 0, tex->getTextureWidth(), tex->getTextureHeight() );
			cam->setRenderOrder(osg::Camera::PRE_RENDER);   
			cam->setRenderTargetImplementation(osg::Camera::FRAME_BUFFER_OBJECT);
			cam->attach( osg::Camera::COLOR_BUFFER, mpr_depthTexture.get() );*/  

			osg::Texture2D *tex=mpr_sceneMgr->getViewDepthTexturn();

			osg::Camera *cam = new osg::Camera;

			cam->setReferenceFrame(osg::Camera::ABSOLUTE_RF_INHERIT_VIEWPOINT);

			cam->setCullCallback(new CameraCullCallback(this));
			cam->setCullMask( ~mpr_viewMask ); 
			cam->setClearMask(GL_DEPTH_BUFFER_BIT);
			//cam->setClearColor(osg::Vec4(1.0f,1.0f,1.0f,1.0f));
			cam->setComputeNearFarMode(osg::Camera::DO_NOT_COMPUTE_NEAR_FAR);
			cam->setViewport(0,0,tex->getTextureWidth(),tex->getTextureHeight());
			cam->setRenderOrder(osg::Camera::PRE_RENDER);
			cam->setRenderTargetImplementation(osg::Camera::FRAME_BUFFER_OBJECT);
			//cam->attach(osg::Camera::DEPTH_BUFFER, mpr_depthTexture.get());
			cam->attach(osg::Camera::DEPTH_BUFFER, tex);


			osg::StateSet* stateset = cam->getOrCreateStateSet();
			osg::ref_ptr<osg::CullFace> cull_face = new osg::CullFace;
			cull_face->setMode(osg::CullFace::FRONT);
			//cull_face->setMode(osg::CullFace::BACK);
			stateset->setAttribute(cull_face.get(), osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
			stateset->setMode(GL_CULL_FACE, osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);

			// negative polygonoffset - move the backface nearer to the eye point so that backfaces shadow themselves
			float factor =-2.0;// -_polyOffset[0];
			float units = -2.0;// -_polyOffset[1];

			osg::ref_ptr<osg::PolygonOffset> polygon_offset = new osg::PolygonOffset;
			polygon_offset->setFactor(factor);
			polygon_offset->setUnits(units);
			stateset->setAttribute(polygon_offset.get(), osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
			stateset->setMode(GL_POLYGON_OFFSET_FILL, osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);


			/*osg::StateSet *sset=cam->getOrCreateStateSet();
			sset->setMode( GL_CULL_FACE, osg::StateAttribute::ON |osg::StateAttribute::OVERRIDE);
			sset->setAttributeAndModes( new osg::CullFace( osg::CullFace::BACK ), osg::StateAttribute::ON|osg::StateAttribute::OVERRIDE);
			sset->setMode(GL_DEPTH_TEST,osg::StateAttribute::OFF| osg::StateAttribute::OVERRIDE);
			sset->setMode(GL_LIGHTING,osg::StateAttribute::OFF| osg::StateAttribute::OVERRIDE);
			sset->setMode(GL_BLEND,osg::StateAttribute::ON| osg::StateAttribute::OVERRIDE);
			sset->setAttributeAndModes( new osg::BlendFunc(GL_ONE, GL_ZERO), osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
			CGlbWString workdir= CGlbPath::GetExecDir();  
			CGlbWString visualLineDepthVertexShader =  workdir +L"\\res\\visualLineDepth.vert"; 
			CGlbWString visualLineDepthFragmentShader = workdir +L"\\res\\visualLineDepth.frag"; 
			osg::ref_ptr<osg::Program> program = createShaderProgram("visualLineDepthShader", visualLineDepthVertexShader.ToString(), visualLineDepthFragmentShader.ToString(), true);
			if(program.valid())
			sset->setAttributeAndModes(program.get(), osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);*/

			mpr_depthCamera=cam;
			calCameraPos();

			return true;
		}
		osg::Vec3 computeOrthogonalVector(const osg::Vec3& direction) const
		{
			float length = direction.length();
			osg::Vec3 orthogonalVector = direction ^ osg::Vec3(0.0f, 1.0f, 0.0f);
			if (orthogonalVector.normalize()<length*0.5f)
			{
				orthogonalVector = direction ^ osg::Vec3(0.0f, 0.0f, 1.0f);
				orthogonalVector.normalize();
			}
			return orthogonalVector;
		}
		osg::Vec3 computeOrthogonalVector(const osg::Vec3d& watchPoint,const osg::Vec3d& eyeDir) const
		{
			float length = eyeDir.length();
			osg::Vec3d eyeDirNor=eyeDir;eyeDirNor.normalize();
			osg::Vec3d watchPointNor=watchPoint;watchPointNor.normalize();
			osg::Vec3d rightNor = eyeDirNor ^ watchPointNor;
			rightNor.normalize();
			osg::Vec3 orthogonalVector=rightNor ^ eyeDirNor;
			
			return orthogonalVector;
		}
		void calCameraPos()
		{
			glbDouble watchX,watchY,watchZ;
			_visualLine->GetPoint(0,&watchX,&watchY,&watchZ);
			osg::Vec3d mpr_watchPoint=osg::Vec3d(watchX,watchY,watchZ);
			_visualLine->GetPoint(1,&watchX,&watchY,&watchZ);
			osg::Vec3d mpr_targetPoint=osg::Vec3d(watchX,watchY,watchZ);

			GlbGlobeTypeEnum globeTypeE=_obj->GetGlobe()->GetType();
			glbBool isGlobeMode=GLB_GLOBETYPE_GLOBE==globeTypeE;
			if (isGlobeMode)
			{
				osg::EllipsoidModel myEllipsoidModel(g_ellipsoidModel->getRadiusEquator(),g_ellipsoidModel->getRadiusPolar());
				myEllipsoidModel.convertLatLongHeightToXYZ(osg::DegreesToRadians(mpr_watchPoint.y()),osg::DegreesToRadians(mpr_watchPoint.x()),mpr_watchPoint.z(),
					mpr_watchPoint.x(),mpr_watchPoint.y(),mpr_watchPoint.z());
				myEllipsoidModel.convertLatLongHeightToXYZ(osg::DegreesToRadians(mpr_targetPoint.y()),osg::DegreesToRadians(mpr_targetPoint.x()),mpr_targetPoint.z(),
					mpr_targetPoint.x(),mpr_targetPoint.y(),mpr_targetPoint.z());
			}

			osg::Vec3d eye=mpr_watchPoint;
			osg::Vec3d up(0.0,0.0,1.0);
			osg::Vec3d center=mpr_targetPoint;
			osg::Vec3d eyeDir=mpr_watchPoint-mpr_targetPoint;
			double depthLen=eyeDir.length();
			//up=computeOrthogonalVector(eyeDir);
			//up=computeOrthogonalVector(mpr_watchPoint,eyeDir);
			up=mpr_watchPoint;up.normalize();
			mpr_depthCamera->setViewMatrixAsLookAt(eye,center,up);
			double fovy=30;
			if (_viewInfo!=NULL&&_viewInfo->fovIndegree!=NULL)
			{
				fovy=_viewInfo->fovIndegree->value;
			}
			double aspectRatio=1.0;
			double dNear=0.1;
			double dFar=depthLen;
			mpr_depthCamera->setProjectionMatrixAsPerspective(fovy,aspectRatio,dNear,dFar);

			mpr_ViewMat_Depth=mpr_depthCamera->getViewMatrix(); 
			mpr_ProjMat_Depth=mpr_depthCamera->getProjectionMatrix();
		}
		void traverse(osg::NodeVisitor& nv)
		{
			if (nv.getVisitorType() == osg::NodeVisitor::CULL_VISITOR)
			{
				osgUtil::CullVisitor* cv = dynamic_cast<osgUtil::CullVisitor*>(&nv);
				if (cv) 
					cull(*cv);
				else 
					osg::Group::traverse(nv);
			}
			else
			{
				osg::Group::traverse(nv);
			}
		}
		void cull(osgUtil::CullVisitor& cv)
		{
			osgUtil::RenderStage* orig_rs = cv.getRenderStage();

			// record the traversal mask on entry so we can reapply it later.
			unsigned int traversalMask = cv.getTraversalMask();
			// do traversal of shadow recieving scene which does need to be decorated by the shadow map
			{
				osg::StateSet *ss=getStateSet();
				if (ss)
					cv.pushStateSet(ss);
				osg::Group::traverse(cv);
				if (ss)
					cv.popStateSet();
			}
			// need to compute view frustum for RTT camera.
			// 1) get the light position
			// 2) get the center and extents of the view frustum			
			{
				cv.setTraversalMask( traversalMask &~mpr_viewMask);

				// do RTT camera traversal   
				mpr_depthCamera->accept(cv);  

				mpr_texgen->setMode(osg::TexGen::EYE_LINEAR);

				// compute the matrix which takes a vertex from local coords into tex coords
				// We actually use two matrices one used to define texgen and second that will be used as modelview when appling to Op en G L   
				mpr_texgen->setPlanesFromMatrix( mpr_depthCamera->getProjectionMatrix() *
					osg::Matrix::translate(1.0,1.0,1.0) * osg::Matrix::scale(0.5f,0.5f,0.5f) ); 

				// Place texgen with modelview which removes big offsets (making it float friendly)   
				osg::RefMatrix * refMatrix = new osg::RefMatrix (mpr_depthCamera->getInverseViewMatrix() * mpr_sceneMgr->getCvModelViewMat() );

				cv.getRenderStage()->getPositionalStateContainer()->
					addPositionedTextureAttribute( mpr_shadowTextureUnit, refMatrix, mpr_texgen.get() );				
			}

			// reapply the original traversal mask
			cv.setTraversalMask( traversalMask );
		}

		bool                              mpr_inited;

		CGlbGlobeRObject                 *_obj;
		GlbViewSymbolInfo                *_viewInfo;
		CGlbLine                       *_visualLine;
		
		
		osg::Matrixf                      mpr_ViewMat_Depth;
		osg::Matrixf                      mpr_ProjMat_Depth;
		osg::ref_ptr<osg::Camera>         mpr_depthCamera;
		CGlbGlobeSceneManager            *mpr_sceneMgr;

		unsigned int                      mpr_normalSceneMask;
		unsigned int                      mpr_viewMask;

		osg::ref_ptr<osg::TexGen>         mpr_texgen;
		unsigned int                      mpr_shadowTextureUnit;
		osg::Vec2                         mpr_ambientBias;


		osg::ref_ptr<osg::MatrixTransform> mpr_viewMT;
		osg::ref_ptr<osg::MatrixTransform> mpr_visualLineMT;
	};	
	CGlbGlobeViewSymbol::CGlbGlobeViewSymbol(void){}

	CGlbGlobeViewSymbol::~CGlbGlobeViewSymbol(void){}

	osg::Node * CGlbGlobeViewSymbol::Draw( CGlbGlobeRObject *obj,IGlbGeometry *geo )
	{
		if (obj==NULL||geo==NULL)
			return NULL;
		GlbRenderInfo *renderInfo = obj->GetRenderInfo(); 
		if (renderInfo==NULL)
			return NULL;
		GlbViewSymbolInfo *viewInfo= dynamic_cast<GlbViewSymbolInfo *>(renderInfo);
		if (viewInfo==NULL)
			return NULL;
		CGlbLine *pLine=dynamic_cast<CGlbLine *>(geo);
		if (pLine==NULL)
			return NULL;

		osg::ref_ptr<CView> view=new CView(obj,viewInfo,pLine);
		if (view->init())
			return view.release();
		/*osg::ref_ptr<CViewScene> viewScene=new CViewScene(obj,viewInfo,pLine);
		if (viewScene->init())
			return viewScene.release();*/
		return NULL;
	}

	void CGlbGlobeViewSymbol::DrawToImage( CGlbGlobeRObject *obj,IGlbGeometry* geom, glbByte *image,glbInt32 imageW, glbInt32 imageH,CGlbExtent &ext ){}

	IGlbGeometry * CGlbGlobeViewSymbol::GetOutline( CGlbGlobeRObject *obj,IGlbGeometry *geo ){return NULL;}
}

