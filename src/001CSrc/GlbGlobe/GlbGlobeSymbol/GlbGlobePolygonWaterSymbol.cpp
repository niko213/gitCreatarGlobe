#include "StdAfx.h"
#include "GlbGlobePolygonWaterSymbol.h"
#include "GlbGlobeSymbolCommon.h"

#include "CGlbGlobe.h"
#include "GlbGlobeView.h"
//#include "GlbGlobeObjectRenderInfos.h"
#include "GlbPolygon.h"
#include "GlbWString.h"
#include "GlbPath.h"
#include "GlbString.h"
#include "GlbTin.h"

#include "osgUtil/DelaunayTriangulator"
#include "osgDB/ReadFile"
#include "osgDB/FileUtils"

#include "osg/CoordinateSystemNode"
#include "osg/Version"
#include "osg/Camera"
#include "osg/MatrixTransform"
#include "osg/Geode"
#include "osgUtil/CullVisitor"
#include "osg/ClipNode"
#include "osg/Timer"
#include "osg/Texture2D"
#include "osg/BlendFunc"
#include "osg/CullFace"
//#include "osg/TexGen"
//#include "osg/TexMat"
#include "osg/PolygonMode"
#include "GlbLog.h"

#ifdef min
#undef min
#endif

#ifdef max
#undef max
#endif

#define USE_LOCAL_SHADERS 0

osg::Texture2D *createTexture2D( const osg::Vec2s& size, GLint format )
{
	osg::Texture2D* texture = new osg::Texture2D;
	texture->setTextureSize(size.x(), size.y());
	texture->setInternalFormat(format);
	texture->setFilter(osg::Texture2D::MIN_FILTER,osg::Texture2D::LINEAR);
	texture->setFilter(osg::Texture2D::MAG_FILTER,osg::Texture2D::LINEAR);
	texture->setWrap(osg::Texture::WRAP_S, osg::Texture::CLAMP );
	texture->setWrap(osg::Texture::WRAP_T, osg::Texture::CLAMP );
	texture->setDataVariance(osg::Object::DYNAMIC);
	return texture;
}

osg::Camera *renderToTexturePass( osg::Texture* textureBuffer )
{
	osg::Camera* camera = new osg::Camera;

	camera->setClearMask( GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT );
	camera->setClearColor( osg::Vec4f(0.f, 0.f, 0.f, 1.f) );
	camera->setReferenceFrame( osg::Transform::ABSOLUTE_RF_INHERIT_VIEWPOINT );
	camera->setViewport( 0,0, textureBuffer->getTextureWidth(), textureBuffer->getTextureHeight() );
	camera->setRenderOrder(osg::Camera::PRE_RENDER);
	camera->setRenderTargetImplementation(osg::Camera::FRAME_BUFFER_OBJECT);
	camera->attach( osg::Camera::COLOR_BUFFER, textureBuffer );

	return camera;
}

namespace GlbGlobe
{
	class CWaterScene;
	class CWaterSurface : public osg::Geode
	{
		friend class CWaterScene;
	private:
		osg::Vec3 mpr_normal;
	public:
		CWaterSurface(CGlbGlobeRObject *obj,IGlbGeometry *geometry)
			:_reflectionMirrorMatrix(
			1,  0,  0,  0,
			0,  1,  0,  0,
			0,  0, -1,  0,
			0,  0,  0,  1 )
		{
			_isDirty =  true;
			_isStateDirty=true;
			polygonWaterPlane=NULL;
			_objRadius=1.0;
			_buildSuccess=false;

			pEyePosUniform=NULL;
			_obj=obj;
			_glbgeo=geometry;
			polygonWaterOffset=new osg::MatrixTransform();

			build();

			if (_buildSuccess)
			{
				setUserData( new WaterSurfaceData(*this, 256, 25) );//setUserData( new WaterSurfaceData(*this, _NUMFRAMES, 25) );
				setUpdateCallback( new WaterAnimationCallback );
				setCullCallback( new WaterAnimationCallback );
			}
		}
					

		~CWaterSurface()
		{
			delete polygonWaterPlane;
		}

		 void update( unsigned int frame, const double& dt, const osg::Vec3f& eye )
		 {
			 if(_isDirty)
				 build();
			 if (_isStateDirty)
				 initStateSet();

			 //getStateSet()->getUniform("water_EyePosition")->set(eye);
			 //getStateSet()->getUniform("ViewMatrix_ReflectCam")->set(/*_reflectionMirrorMatrix**/_reflectionViewMatrix);
			 //getStateSet()->getUniform("ProjectionMatrix_ReflectCam")->set(_reflectionProjMatrix);
			 //_oldFrame = frame;
		 }

		 void build( void )
		 {
			 if (_isStateDirty)
				 initStateSet();
			 
			 _isDirty =  false;
			 _isStateDirty = false;

			 GlbGeometryTypeEnum geoType = _glbgeo->GetType();
			 switch(geoType)
			 {
			 case GLB_GEO_POLYGON:
				 {					
					 osg::Geometry *geometry=buildGeometry(_obj,_glbgeo);
					 if (geometry!=NULL)
					 {
						 _buildSuccess=true;
						 this->addDrawable(geometry);
					 }
				 }					
				 break;
			 case GLB_GEO_MULTIPOLYGON:
				 {
					 CGlbMultiPolygon* multiPoly = dynamic_cast<CGlbMultiPolygon*>(_glbgeo);
					 glbInt32 polyCnt = multiPoly->GetCount();
					 for (int k = 0; k < polyCnt; k++)
					 {
						 CGlbPolygon* pPolyGon = const_cast<CGlbPolygon *>(multiPoly->GetPolygon(k));			

						 osg::Geometry *geometry=buildGeometry(_obj,pPolyGon);
						 if (geometry!=NULL)
						 {
							 _buildSuccess=true;
							 this->addDrawable(geometry);
						 }
					 }
				 }
				 break;
			 case GLB_GEO_TIN:
				 {
					 osg::Geometry *geometry=buildGeometry(_obj,_glbgeo);
					 if (geometry!=NULL)
					 {
						 _buildSuccess=true;
						 this->addDrawable(geometry);
					 }
				 }
				 break;
			 case GLB_GEO_MULTITIN:
				 {
					 CGlbMultiTin* multiTin = dynamic_cast<CGlbMultiTin*>(_glbgeo);
					 glbInt32 tinCnt = multiTin->GetCount();
					 for (int k = 0; k < tinCnt; k++)
					 {
						 CGlbTin* pTin = const_cast<CGlbTin *>(multiTin->GetTin(k));			

						 osg::Geometry *geometry=buildGeometry(_obj,pTin);
						 if (geometry!=NULL)
						 {
							 _buildSuccess=true;
							 this->addDrawable(geometry);
						 }
					 }
				 }
				 break;				
			 }
		 }

		 bool buildSuccess()const{return _buildSuccess;}

		 bool isVisible( osgUtil::CullVisitor& cv, bool eyeAboveWater )
		 {
			 if (getNodeMask() == 0) return false;

			 return true;
			 //// Use a small cutoff to unconditionally cull ocean surface.
			 //// This assumes the view frustum is 45 degrees wide...
			 //static const float cutoff = osg::PI/8;      // 45 degrees divided by 2
			 //osg::Vec3 lookVector = cv.getLookVectorLocal();
			 //float dotProduct = lookVector * osg::Vec3(0,0,1);
			 //return ( eyeAboveWater && dotProduct <  cutoff) ||
				// (!eyeAboveWater && dotProduct > -cutoff);

			 //// A better way would be to check if any of the frustum corners intersect 
			 //// the plane at (0,0,ocean_height) with normal (0,0,1), and if not then 
			 //// return true.
		 }

		 osg::MatrixTransform *getPolygonWaterOffsetMT()const{return polygonWaterOffset;}

		 osg::Plane *getPolygonWaterSurface()const{return polygonWaterPlane;}

		 glbDouble getObjRadius()const{return _objRadius;}

		 osg::Vec3d const &getObjCenter()const{return _objCenter;}
	private:
		class WaterSurfaceData: public osg::Referenced
		{
		private:
			CWaterSurface         &_waterSurface;
			const unsigned int     _NUMFRAMES;
			osg::Vec3f             _eye;
			float                  _time;
			const unsigned int     _FPS;
			float                  _msPerFrame;
			unsigned int           _frame;
			osg::Timer_t           _oldTime;
			osg::Timer_t           _newTime;

		public:
			WaterSurfaceData(CWaterSurface& waterSurface, unsigned int numFrames, unsigned int fps)
				:_waterSurface( waterSurface ),
				_NUMFRAMES    ( numFrames ),
				_time         ( 0.f ),
				_FPS          ( fps ), 
				_msPerFrame   ( 1000.f/(float)fps ),
				_frame        ( 0 ),
				_oldTime      ( 0 ),
				_newTime      ( 0 ){}
			inline void setEye( const osg::Vec3f& eye )
			{ 
				//double lll=eye.length();
				_eye = eye;
				if(_waterSurface.pEyePosUniform)
					_waterSurface.pEyePosUniform->set(eye);
			}

			void updateWater(void)
			{
				_oldTime = _newTime;
				_newTime = osg::Timer::instance()->tick();

				double dt = osg::Timer::instance()->delta_m(_oldTime, _newTime);
				_time += dt;

				if( _time >= _msPerFrame )
				{
					_frame += ( _time / _msPerFrame );

					if( _frame >= _NUMFRAMES ) 
						_frame = _frame%_NUMFRAMES; 

					_time = fmod( _time, _msPerFrame );
				}

				_waterSurface.update( _frame, dt, _eye );
			}
		};
		class WaterAnimationCallback: public osg::NodeCallback
		{
		public:
			virtual void operator()(osg::Node* node, osg::NodeVisitor* nv)
			{
				osg::ref_ptr<WaterSurfaceData> waterData = dynamic_cast<WaterSurfaceData*> ( node->getUserData() );
				if( waterData.valid() )
				{
					// If cull visitor update the current eye position
					if( nv->getVisitorType() == osg::NodeVisitor::CULL_VISITOR )
					{
						osgUtil::CullVisitor* cv = static_cast<osgUtil::CullVisitor*>(nv);
						osg::Vec3f eyepos=cv->getEyePoint();
						waterData->setEye( eyepos);
					}
					else if( nv->getVisitorType() == osg::NodeVisitor::UPDATE_VISITOR )
					{
						//osg::Vec3f eyepos=nv->getEyePoint();
						waterData->updateWater();
					}
				}

				traverse(node, nv);		
			}
		};
		void initStateSet()
		{
			_stateset=new osg::StateSet();
			pEyePosUniform=new osg::Uniform("water_EyePosition", osg::Vec3f() ) ;
			_stateset->addUniform(pEyePosUniform);
			//_stateset->addUniform( new osg::Uniform("ViewMatrix_ReflectCam", osg::Matrixf()) );
			//_stateset->addUniform( new osg::Uniform("ProjectionMatrix_ReflectCam", osg::Matrixf()) );
			//_stateset->addUniform( new osg::Uniform("WaterPolygon_UseCameraRefractionMap", false ) );
			//CGlbWString workdir= CGlbPath::GetExecDir();
			//CGlbWString heightMapFilePath = workdir + L"\\res\\waterHeight.png";
			//osg::Texture2D* waterHeight_tex = readTexture(heightMapFilePath, osg::Texture::MIRROR);
			//_stateset->setTextureAttributeAndModes(0, waterHeight_tex, osg::StateAttribute::ON );
			//
			//CGlbWString specifyRefractMapFilePath = workdir + L"\\res\\refractMap.jpg";
			//osg::Texture2D* specifyRefract_tex = readTexture(specifyRefractMapFilePath, osg::Texture::REPEAT);
			//_stateset->setTextureAttributeAndModes(3, specifyRefract_tex, osg::StateAttribute::ON );
			//_stateset->addUniform( new osg::Uniform("WaterPolygon_HeightMap", 0) );
			//_stateset->addUniform( new osg::Uniform("WaterPolygon_ReflectionMap", 1) );
			//_stateset->addUniform( new osg::Uniform("WaterPolygon_CameraRefractionMap", 2) );
			//_stateset->addUniform( new osg::Uniform("WaterPolygon_SpecifyRefractionMap", 3) );

			//_stateset->addUniform( new osg::Uniform("WaterPolygon_UseCameraRefractionMap", false ) );
			//_stateset->addUniform( new osg::Uniform("waterPolygon_EnableReflections", false ) );
			//_stateset->addUniform( new osg::Uniform("waterPolygon_EnableRefractions", false ) );


			//_stateset->setRenderBinDetails( 5, "RenderBin");
			//_stateset->setMode(GL_BLEND,osg::StateAttribute::ON);
			//osg::ref_ptr<osg::BlendFunc> pBlendFun = new osg::BlendFunc(osg::BlendFunc::SRC_ALPHA,osg::BlendFunc::ONE_MINUS_SRC_ALPHA );	//设置混合方程
			//_stateset->setAttributeAndModes(pBlendFun.get(),osg::StateAttribute::ON/*|osg::StateAttribute::OVERRIDE*/);
			//osg::ref_ptr<osg::Program> program = createShader();
			//if(program.valid())
			//	_stateset->setAttributeAndModes( program.get(), osg::StateAttribute::ON );



			_isStateDirty = false;
		}
		osg::Program* createShader(void)
		{
#if/*def*/ USE_LOCAL_SHADERS

			static const char polygonVertexShader[] = 
				"uniform mat4 osg_ViewMatrixInverse;\n"
				"\n"
				"uniform vec3 osgOcean_EyePosition;\n"
				"\n"
				"uniform vec3 osgOcean_NoiseCoords0;\n"
				"uniform vec3 osgOcean_NoiseCoords1;\n"
				"\n"
				"uniform vec4 osgOcean_WaveTop;\n"
				"uniform vec4 osgOcean_WaveBot;\n"
				"\n"
				"uniform float osgOcean_FoamScale;\n"
				"\n"
				"varying vec4 vVertex;\n"
				"varying vec4 vWorldVertex;\n"
				"varying vec3 vNormal;\n"
				"varying vec3 vViewerDir;\n"
				"varying vec3 vLightDir;\n"
				"\n"
				"varying vec3 vWorldViewDir;\n"
				"varying vec3 vWorldNormal;\n"
				"\n"
				"mat3 get3x3Matrix( mat4 m )\n"
				"{\n"
				"    mat3 result;\n"
				"\n"
				"    result[0][0] = m[0][0];\n"
				"    result[0][1] = m[0][1];\n"
				"    result[0][2] = m[0][2];\n"
				"\n"
				"    result[1][0] = m[1][0];\n"
				"    result[1][1] = m[1][1];\n"
				"    result[1][2] = m[1][2];\n"
				"\n"
				"    result[2][0] = m[2][0];\n"
				"    result[2][1] = m[2][1];\n"
				"    result[2][2] = m[2][2];\n"
				"\n"
				"    return result;\n"
				"}\n"
				"\n"
				"// -------------------------------\n"
				"//          Main Program\n"
				"// -------------------------------\n"
				"\n"
				"void main( void )\n"
				"{\n"
				"    gl_Position = ftransform();\n"
				"\n"
				"    // -----------------------------------------------------------\n"
				"\n"
				"    // In object space\n"
				"    vVertex = gl_Vertex;\n"
				"    vLightDir = normalize( vec3( gl_ModelViewMatrixInverse * ( gl_LightSource[osgOcean_LightID].position ) ) );\n"
				"    vViewerDir = gl_ModelViewMatrixInverse[3].xyz - gl_Vertex.xyz;\n"
				"    vNormal = normalize(gl_Normal);\n"
				"\n"
				"    vec4 waveColorDiff = osgOcean_WaveTop-osgOcean_WaveBot;\n"
				"\n"
				"    gl_FrontColor = waveColorDiff *\n"
				"        clamp((gl_Vertex.z + osgOcean_EyePosition.z) * 0.1111111 + vNormal.z - 0.4666667, 0.0, 1.0) + osgOcean_WaveBot;\n"
				"\n"
				"    // -------------------------------------------------------------\n"
				"\n"
				"    mat4 modelMatrix = osg_ViewMatrixInverse * gl_ModelViewMatrix;\n"
				"    mat3 modelMatrix3x3 = get3x3Matrix( modelMatrix );\n"
				"\n"
				"    // world space\n"
				"    vWorldVertex = modelMatrix * gl_Vertex;\n"
				"    vWorldNormal = modelMatrix3x3 * gl_Normal;\n"
				"    vWorldViewDir = vWorldVertex.xyz - osgOcean_EyePosition.xyz;\n"
				"\n"
				"    // ------------- Texture Coords ---------------------------------\n"
				"\n"
				"    // Normal Map Coords\n"
				"    gl_TexCoord[0].xy = ( gl_Vertex.xy * osgOcean_NoiseCoords0.z + osgOcean_NoiseCoords0.xy );\n"
				"    gl_TexCoord[0].zw = ( gl_Vertex.xy * osgOcean_NoiseCoords1.z + osgOcean_NoiseCoords1.xy );\n"
				"    gl_TexCoord[0].y = -gl_TexCoord[0].y;\n"
				"    gl_TexCoord[0].w = -gl_TexCoord[0].w;\n"
				"\n"
				"    // Foam coords\n"
				"    gl_TexCoord[1].st = gl_Vertex.xy * osgOcean_FoamScale;\n"
				"\n"
				"    // Fog coords\n"
				"    gl_FogFragCoord = gl_Position.z;\n"
				"}\n";

			static const char polygonFragmentShader[] = 
				"uniform bool osgOcean_EnableReflections;\n"
				"uniform bool osgOcean_EnableRefractions;\n"
				"uniform bool osgOcean_EnableCrestFoam;\n"
				"\n"
				"uniform bool osgOcean_EnableDOF;\n"
				"uniform bool osgOcean_EnableGlare;\n"
				"\n"
				"uniform float osgOcean_DOF_Near;\n"
				"uniform float osgOcean_DOF_Focus;\n"
				"uniform float osgOcean_DOF_Far;\n"
				"uniform float osgOcean_DOF_Clamp;\n"
				"uniform float osgOcean_FresnelMul;\n"
				"\n"
				"uniform samplerCube osgOcean_EnvironmentMap;\n"
				"uniform sampler2D   osgOcean_ReflectionMap;\n"
				"uniform sampler2D   osgOcean_RefractionMap;\n"
				"uniform sampler2D   osgOcean_FoamMap;\n"
				"uniform sampler2D   osgOcean_NoiseMap;\n"
				"\n"
				"uniform float osgOcean_UnderwaterFogDensity;\n"
				"uniform float osgOcean_AboveWaterFogDensity;\n"
				"uniform vec4  osgOcean_UnderwaterFogColor;\n"
				"uniform vec4  osgOcean_AboveWaterFogColor;\n"
				"\n"
				"uniform mat4 osg_ViewMatrixInverse;\n"
				"\n"
				"uniform float osgOcean_FoamCapBottom;\n"
				"uniform float osgOcean_FoamCapTop;\n"
				"\n"
				"varying vec3 vNormal;\n"
				"varying vec3 vViewerDir;\n"
				"varying vec3 vLightDir;\n"
				"varying vec4 vVertex;\n"
				"varying vec4 vWorldVertex;\n"
				"\n"
				"varying vec3 vWorldViewDir;\n"
				"varying vec3 vWorldNormal;\n"
				"\n"
				"mat4 worldObjectMatrix;\n"
				"\n"
				"const float shininess = 2000.0;\n"
				"\n"
				"vec4 distortGen( vec4 v, vec3 N )\n"
				"{\n"
				"    // transposed\n"
				"    const mat4 mr = mat4( 0.5, 0.0, 0.0, 0.0,\n"
				"                                 0.0, 0.5, 0.0, 0.0,\n"
				"                                 0.0, 0.0, 0.5, 0.0,\n"
				"                                 0.5, 0.5, 0.5, 1.0 );\n"
				"\n"
				"    mat4 texgen_matrix = mr * gl_ProjectionMatrix * gl_ModelViewMatrix;\n"
				"\n"
				"    //float disp = 8.0;\n"
				"    float disp = 4.0;\n"
				"\n"
				"    vec4 tempPos;\n"
				"\n"
				"    tempPos.xy = v.xy + disp * N.xy;\n"
				"    tempPos.z  = v.z;\n"
				"    tempPos.w  = 1.0;\n"
				"\n"
				"    return texgen_matrix * tempPos;\n"
				"}\n"
				"\n"
				"vec3 reorientate( vec3 v )\n"
				"{\n"
				"    float y = v.y;\n"
				"\n"
				"    v.y = -v.z;\n"
				"    v.z = y;\n"
				"\n"
				"    return v;\n"
				"}\n"
				"\n"
				"mat3 getLinearPart( mat4 m )\n"
				"{\n"
				"    mat3 result;\n"
				"\n"
				"    result[0][0] = m[0][0];\n"
				"    result[0][1] = m[0][1];\n"
				"    result[0][2] = m[0][2];\n"
				"\n"
				"    result[1][0] = m[1][0];\n"
				"    result[1][1] = m[1][1];\n"
				"    result[1][2] = m[1][2];\n"
				"\n"
				"    result[2][0] = m[2][0];\n"
				"    result[2][1] = m[2][1];\n"
				"    result[2][2] = m[2][2];\n"
				"\n"
				"    return result;\n"
				"}\n"
				"\n"
				"vec4 computeCubeMapColor( vec3 N, vec4 V, vec3 E )\n"
				"{\n"
				"    mat3 worldObjectMat3x3 = getLinearPart( worldObjectMatrix );\n"
				"    vec4 world_pos    = worldObjectMatrix *  V;\n"
				"\n"
				"    vec3 normal = normalize( worldObjectMat3x3 * N );\n"
				"    vec3 eye = normalize( world_pos.xyz - E );\n"
				"\n"
				"    vec3 coord = reflect( eye, normal );\n"
				"\n"
				"    vec3 reflection_vector = vec3( coord.x, coord.y, -coord.z );\n"
				"\n"
				"    return textureCube(osgOcean_EnvironmentMap, reflection_vector.xzy);\n"
				"}\n"
				"\n"
				"float calcFresnel( float dotEN, float mul )\n"
				"{\n"
				"    float fresnel = clamp( dotEN, 0.0, 1.0 ) + 1.0;\n"
				"    return pow(fresnel, -8.0) * mul;\n"
				"}\n"
				"\n"
				"float alphaHeight( float min, float max, float val)\n"
				"{\n"
				"    if(max-min == 0.0)\n"
				"        return 1.0;\n"
				"\n"
				"    return (val - min) / (max - min);\n"
				"}\n"
				"\n"
				"float computeDepthBlur(float depth, float focus, float near, float far, float clampval )\n"
				"{\n"
				"   float f;\n"
				"\n"
				"   if (depth < focus){\n"
				"      // scale depth value between near blur distance and focal distance to [-1, 0] range\n"
				"      f = (depth - focus)/(focus - near);\n"
				"   }\n"
				"   else{\n"
				"      // scale depth value between focal distance and far blur\n"
				"      // distance to [0, 1] range\n"
				"      f = (depth - focus)/(far - focus);\n"
				"\n"
				"      // clamp the far blur to a maximum blurriness\n"
				"      f = clamp(f, 0.0, clampval);\n"
				"   }\n"
				"\n"
				"   // scale and bias into [0, 1] range\n"
				"   return f * 0.5 + 0.5;\n"
				"}\n"
				"\n"
				"float luminance( vec4 color )\n"
				"{\n"
				"    return (0.3*color.r) + (0.59*color.g) + (0.11*color.b);\n"
				"}\n"
				"\n"
				"float computeFogFactor( float density, float fogCoord )\n"
				"{\n"
				"    return exp2(density * fogCoord * fogCoord );\n"
				"}\n"
				"\n"
				"// -------------------------------\n"
				"//          Main Program\n"
				"// -------------------------------\n"
				"\n"
				"void main( void )\n"
				"{\n"
				"    vec4 final_color;\n"
				"\n"
				"    vec3 noiseNormal = vec3( texture2D( osgOcean_NoiseMap, gl_TexCoord[0].xy ) * 2.0 - 1.0 );\n"
				"    noiseNormal += vec3( texture2D( osgOcean_NoiseMap, gl_TexCoord[0].zw ) * 2.0 - 1.0 );\n"
				"\n"
				"    worldObjectMatrix = osg_ViewMatrixInverse * gl_ModelViewMatrix;\n"
				"\n"
				"    if(gl_FrontFacing)\n"
				"    {\n"
				"        vec3 N = normalize( vNormal + noiseNormal );\n"
				"        vec3 L = normalize( vLightDir );\n"
				"        vec3 E = normalize( vViewerDir );\n"
				"        vec3 R = reflect( -L, N );\n"
				"\n"
				"        vec4 specular_color;\n"
				"\n"
				"        float lambertTerm = dot(N,L);\n"
				"\n"
				"        if( lambertTerm > 0.0 )\n"
				"        {\n"
				"            float specCoeff = pow( max( dot(R, E), 0.0 ), shininess );\n"
				"            specular_color = gl_LightSource[osgOcean_LightID].diffuse * specCoeff * 6.0;\n"
				"        }\n"
				"\n"
				"        float dotEN = dot(E, N);\n"
				"        float dotLN = dot(L, N);\n"
				"\n"
				"        vec4 refraction_color = vec4( gl_Color.rgb, 1.0 );\n"
				"\n"
				"        // To cubemap or not to cubemap that is the question\n"
				"        // projected reflection looks pretty nice anyway\n"
				"        // cubemap looks wrong with fixed skydome\n"
				"        //vec4 env_color = computeCubeMapColor(N, vWorldVertex, osgOcean_EyePosition);\n"
				"\n"
				"        float fresnel = calcFresnel(dotEN, osgOcean_FresnelMul );\n"
				"        \n"
				"        vec4 env_color;\n"
				"\n"
				"        if(osgOcean_EnableReflections)\n"
				"        {\n"
				"            env_color = texture2DProj( osgOcean_ReflectionMap, distortGen(vVertex, N) );    \n"
				"        }\n"
				"        else\n"
				"        {\n"
				"            env_color = gl_LightSource[osgOcean_LightID].diffuse;            \n"
				"        }\n"
				"        \n"
				"        final_color = mix(refraction_color, env_color, fresnel) + specular_color;\n"
				"\n"
				"        // Store the color here to compute luminance later, we don't want \n"
				"        // foam or fog to be taken into account for this calculation.\n"
				"        vec4 lumColor = final_color;\n"
				"\n"
				"        if(osgOcean_EnableCrestFoam)\n"
				"        {\n"
				"            if( vVertex.z > osgOcean_FoamCapBottom )\n"
				"            {\n"
				"                vec4 foam_color  = texture2D( osgOcean_FoamMap, gl_TexCoord[1].st );\n"
				"\n"
				"                float alpha = alphaHeight( osgOcean_FoamCapBottom, osgOcean_FoamCapTop, vVertex.z ) * (fresnel*2.0);\n"
				"\n"
				"                final_color = mix( final_color, foam_color, alpha );\n"
				"            }\n"
				"        }\n"
				"\n"
				"        // exp2 fog\n"
				"        float fogFactor = computeFogFactor( osgOcean_AboveWaterFogDensity, gl_FogFragCoord );\n"
				"        \n"
				"        final_color = mix( osgOcean_AboveWaterFogColor, final_color, fogFactor );\n"
				"\n"
				"        gl_FragColor = final_color;\n"
				"                \n"
				"        if(osgOcean_EnableGlare)\n"
				"        {\n"
				"            gl_FragColor.a = luminance(lumColor);\n"
				"        }\n"
				"    }\n"
				"    else\n"
				"    {\n"
				"        vec3 E = normalize( vViewerDir );\n"
				"        vec3 N = -normalize( (vWorldNormal + noiseNormal) );\n"
				"\n"
				"        vec3 incident = normalize( vWorldViewDir );\n"
				"\n"
				"        //------ Find the reflection\n"
				"        // not really usable as we would need to use cubemap again..\n"
				"        // the ocean is blue not much to reflect back\n"
				"        //vec3 reflected = reflect( incident, -N );\n"
				"        //reflected        = reorientate( reflected );\n"
				"        //vec3 reflVec    = normalize( reflected );\n"
				"\n"
				"        //------ Find the refraction from cubemap\n"
				"        vec3 refracted = refract( incident, N, 1.3333333333 );   // 1.1 looks better? - messes up position of godrays though\n"
				"        refracted.z = refracted.z - 0.015;                       // on the fringes push it down to show base texture color\n"
				"        refracted = reorientate( refracted );\n"
				"\n"
				"		vec4 refractColor = textureCube( osgOcean_EnvironmentMap, refracted );\n"
				"\n"
				"        //------ Project texture where the light isn't internally reflected\n"
				"        if(osgOcean_EnableRefractions)\n"
				"        {\n"
				"            // if alpha is 1.0 then it's a sky pixel\n"
				"			if(refractColor.a == 1.0 )\n"
				"			{\n"
				"                vec4 env_color = texture2DProj( osgOcean_RefractionMap, distortGen(vVertex, N) );\n"
				"				refractColor.rgb = mix( refractColor.rgb, env_color.rgb, env_color.a );\n"
				"			}\n"
				"        }\n"
				"\n"
				"        // if it's not refracting in, add a bit of highlighting with fresnel\n"
				"		if( refractColor.a == 0.0 )\n"
				"		{\n"
				"			float fresnel = calcFresnel( dot(E, N), 0.7 );\n"
				"            refractColor.rgb = osgOcean_UnderwaterFogColor.rgb*fresnel + (1.0-fresnel)* refractColor.rgb;\n"
				"		}\n"
				"\n"
				"        float fogFactor = computeFogFactor( osgOcean_UnderwaterFogDensity, gl_FogFragCoord );\n"
				"        final_color = mix( osgOcean_UnderwaterFogColor, refractColor, fogFactor );\n"
				"\n"
				"        if(osgOcean_EnableDOF)\n"
				"        {\n"
				"            final_color.a = computeDepthBlur( gl_FogFragCoord, osgOcean_DOF_Focus, osgOcean_DOF_Near, osgOcean_DOF_Far, osgOcean_DOF_Clamp );\n"
				"        }\n"
				"\n"
				"        gl_FragColor = final_color;\n"
				"    }\n"
				"}\n";
#else
            HMODULE hmd = GetModuleHandle(L"GlbGlobe.dll");
			CGlbWString workdir = CGlbPath::GetModuleDir(hmd);
            //static const char ocean_surface_vertex[]   = "D:\\SourceCode\\CreatarSourceCode\\trunck2014\\trunck\\bin\\X86_32\\release\\res\\water.vert";
			//static const char ocean_surface_fragment[] = "D:\\SourceCode\\CreatarSourceCode\\trunck2014\\trunck\\bin\\X86_32\\release\\res\\water.frag";
			CGlbWString polygonVertexShader =  workdir +L"\\res\\water.vert";
			CGlbWString polygonFragmentShader = workdir +L"\\res\\water.frag";
#endif
			osg::Program* program = createShaderProgram("polygonWaterShader", polygonVertexShader.ToString(), polygonFragmentShader.ToString(),true);
			return program;
		}		

		osg::Geometry *buildGeometry(CGlbGlobeRObject *obj, CGlbTin *geo)
		{
			if (NULL==obj||geo==NULL)
				return NULL;
			GlbRenderInfo *polygonInfo = obj->GetRenderInfo();
			GlbTinWaterSymbolInfo *polygonWaterInfo= dynamic_cast<GlbTinWaterSymbolInfo *>(polygonInfo);
			if(polygonWaterInfo == NULL)
				return NULL;	

			glbBool isGlobe=obj->GetGlobe()->GetType()==GLB_GLOBETYPE_GLOBE;
			glbref_ptr<CGlbExtent> pExt=obj->GetBound(isGlobe);
			glbDouble cetX,cetY,cetZ;
			pExt->GetCenter(&cetX,&cetY,&cetZ);
			_objCenter=osg::Vec3d(cetX,cetY,cetZ);

			osg::EllipsoidModel myEllipsoidModel(g_ellipsoidModel->getRadiusEquator(),g_ellipsoidModel->getRadiusPolar());
			osg::Matrixd polygonOrient;
			myEllipsoidModel.computeLocalToWorldTransformFromXYZ(cetX,cetY,cetZ,polygonOrient);
			osg::Matrixd polygonOrient_inv=polygonOrient.inverse(polygonOrient);

			glbInt32 vertexCount = geo->GetVertexCount();
			glbInt32 triangleCount = geo->GetTriangleCount();
			if(vertexCount < 1 || triangleCount < 1)
				return NULL;

			glbDouble ptx,pty,ptz;
			glbInt32 first_ptidx,second_ptidx,third_ptidx;
			//osg::ref_ptr<osg::DrawElementsUInt> indexArray = new osg::DrawElementsUInt(osg::DrawElementsUInt::TRIANGLES,0);
			osg::ref_ptr<osg::Vec3Array> drawPoints = new osg::Vec3Array();
			osg::ref_ptr<osg::Vec3Array> normalArray = new osg::Vec3Array(vertexCount);

			osg::ref_ptr<osg::Vec3Array> points=new osg::Vec3Array();

			GlbGlobeTypeEnum type = obj->GetGlobe()->GetType();
			osg::Vec3 normal;
			osg::Vec3d drawPoint;
			osg::Vec3d centerPoint;
			geo->GetExtent()->GetCenter(&ptx,&pty,&ptz);

			if(type == GLB_GLOBETYPE_GLOBE)
			{
				g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(pty),osg::DegreesToRadians(ptx),ptz,
					centerPoint.x(),centerPoint.y(),centerPoint.z());
			}
			else
			{
				centerPoint.x() = ptx;
				centerPoint.y() = pty;
				centerPoint.z() = ptz;
			}		
			for(glbInt32 i = 0; i < vertexCount;i++)
			{
				geo->GetVertex(i,&ptx,&pty,&ptz);
				points->push_back(osg::Vec3d(ptx,pty,ptz));

				if(type == GLB_GLOBETYPE_GLOBE)
				{
					g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(pty),osg::DegreesToRadians(ptx),ptz,
						drawPoint.x(),drawPoint.y(),drawPoint.z());
				}
				else
				{
					drawPoint.x() = ptx;
					drawPoint.y() = pty;
					drawPoint.z() = ptz;
				}
				drawPoints->push_back(drawPoint - centerPoint);
			}

			if (type == GLB_GLOBETYPE_GLOBE)
			{
				polygonWaterPlane=new osg::Plane(drawPoints->at(0) + centerPoint,drawPoints->at(1) + centerPoint,drawPoints->at(2) + centerPoint);
				polygonWaterPlane->makeUnitLength();
				polygonWaterPlane->flip();
			}
			else
			{
				polygonWaterPlane=new osg::Plane(drawPoints->at(0) + centerPoint,drawPoints->at(1) + centerPoint,drawPoints->at(2) + centerPoint);
				polygonWaterPlane->makeUnitLength();
				polygonWaterPlane->flip();
			}

			mpr_normal.set(0,0,0);

			osg::ref_ptr<osg::UIntArray> indexArray = new osg::UIntArray();
			for(glbInt32 i = 0; i < triangleCount;i++)
			{
				geo->GetTriangle(i,&first_ptidx,&second_ptidx,&third_ptidx);
				indexArray->push_back(first_ptidx);
				indexArray->push_back(second_ptidx);
				indexArray->push_back(third_ptidx);

				//计算三角形的法线
				osg::Vec3 vec0 = drawPoints->at(first_ptidx);
				osg::Vec3 vec1 = drawPoints->at(second_ptidx);
				osg::Vec3 vec2 = drawPoints->at(third_ptidx);
				normal = (vec1-vec0)^(vec2-vec0);
				normal.normalize();
				{// 法线求平均
					normalArray->at(first_ptidx) += normal;
					normalArray->at(second_ptidx) += normal;
					normalArray->at(third_ptidx) += normal;
				}	
				mpr_normal += normal;
			}

			for(glbInt32 k = 0; k < vertexCount; k++)
				normalArray->at(k).normalize();
			float nx = mpr_normal.x()/triangleCount;
			float ny = mpr_normal.y()/triangleCount;
			float nz = mpr_normal.z()/triangleCount;
			mpr_normal.set(nx,ny,nz);
			mpr_normal.normalize();

			osg::ref_ptr<osg::DrawElementsUInt> primitiveSet = new osg::DrawElementsUInt(osg::DrawElementsUInt::TRIANGLES,indexArray->size(),&indexArray->front());
			osg::ref_ptr<osg::Geometry> geometry = new osg::Geometry;
			geometry->setVertexArray(drawPoints.get());	
			geometry->setNormalArray(normalArray.get());
			geometry->setNormalBinding(osg::Geometry::BIND_PER_VERTEX);
			geometry->addPrimitiveSet(primitiveSet.get());
			
			// 纹理坐标
			double PolySizeX,PolySizeY,PolyCentX,PolyCentY;
			osg::Vec2Array* norTexCoords = buildNormalCoords(points.get(),PolySizeX,PolySizeY,PolyCentX,PolyCentY);//计算多边形纹理坐标
			double sizeX,sizeY,centX,centY;
			float texRotAngles=0.0f;
			if (polygonWaterInfo->refractTexRotation)
			{
				texRotAngles=polygonWaterInfo->refractTexRotation->value;
			}
			osg::Vec2Array* texcoords = buildTexCoordsEx(points.get(),osg::DegreesToRadians(texRotAngles),sizeX,sizeY,centX,centY);//计算多边形纹理坐标
			
			geometry->setTexCoordArray(0, norTexCoords);
			geometry->setTexCoordArray(1, texcoords);

			osg::Matrix m;
			m.makeTranslate(centerPoint);
			polygonWaterOffset->preMult(m);
			osg::StateSet *sset=geometry->getOrCreateStateSet();  
			sset->setMode( GL_CULL_FACE, osg::StateAttribute::ON );

			//sset->setRenderBinDetails( 0, "RenderBin");
			sset->setMode(GL_BLEND,osg::StateAttribute::ON);
			//sset->setMode(osg::PolygonMode::FRONT_AND_BACK,osg::PolygonMode::LINE);
			osg::ref_ptr<osg::BlendFunc> pBlendFun = new osg::BlendFunc(osg::BlendFunc::SRC_ALPHA,osg::BlendFunc::ONE_MINUS_SRC_ALPHA );	//设置混合方程
			_stateset->setAttributeAndModes(pBlendFun.get(),osg::StateAttribute::ON/*|osg::StateAttribute::OVERRIDE*/);
			sset->addUniform( new osg::Uniform("PolygonCenter", osg::Vec2f(PolyCentX,PolyCentY) ) );
			sset->addUniform( new osg::Uniform("PolygonWeightAndHeight", osg::Vec2f(PolySizeX,PolySizeY) ) );
			sset->addUniform( new osg::Uniform("PolygonRotateCenter", osg::Vec2f(centX,centY) ) );
			sset->addUniform( new osg::Uniform("PolygonRotateWeightAndHeight", osg::Vec2f(sizeX,sizeY) ) );

			sset->addUniform( new osg::Uniform("PolygonSelected", false) );
			sset->addUniform( new osg::Uniform("PolygonBlink", false) );

			// 1、透明
			glbInt32 polygonOpacity=100;
			if (polygonWaterInfo->opacity)
			{
				polygonOpacity=polygonWaterInfo->opacity->value;
			}
			if (polygonOpacity!=100)//透明
				sset->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
			else
				sset->setRenderingHint(osg::StateSet::OPAQUE_BIN);
			sset->addUniform( new osg::Uniform("PolygonOpacity", polygonOpacity ) );

			// 法线
			HMODULE hmd = GetModuleHandle(L"GlbGlobe.dll");
			CGlbWString workdir = CGlbPath::GetModuleDir(hmd);
			CGlbWString heightMapFilePath = workdir + L"\\res\\waterHeight.png";
			osg::Texture2D* waterHeight_tex = readTexture(heightMapFilePath, osg::Texture::MIRROR);
			sset->setTextureAttributeAndModes(0, waterHeight_tex, osg::StateAttribute::ON );
			sset->addUniform( new osg::Uniform("WaterPolygon_HeightMap", 0) );

			// 启用相机折射水底/或者贴水底折射贴图
			osg::Vec4f polygonColor(0.0,255.0,0.0,1.0);
			if (polygonWaterInfo->color)
			{
				glbInt32 plyClr=polygonWaterInfo->color->value;
				polygonColor.set(GetRValueInBGRA(plyClr),LOBYTE(((plyClr & 0xFFFF)) >> 8),GetBValueInBGRA(plyClr),1.0);
			}
			sset->addUniform( new osg::Uniform("PolygonColor", polygonColor) );
			bool enableRefract=false;
			if (polygonWaterInfo->enableRefractions)
			{
				enableRefract=polygonWaterInfo->enableRefractions->value;
			}
			sset->addUniform( new osg::Uniform("waterPolygon_EnableRefractions", enableRefract ) );
			bool buseTexColor=false;
			osg::Image *pImg=NULL;
			if (polygonWaterInfo->refractionImageData)
			{
				CGlbWString imageMapFilePath=polygonWaterInfo->refractionImageData->value;
				glbInt32 index = imageMapFilePath.find_first_of(L'.');
				if(index == 0)
				{// 处理当前执行文件的相对路径情况 ./AAA/....
					CGlbWString execDir = CGlbPath::GetExecDir();
					imageMapFilePath = execDir + imageMapFilePath.substr(1,imageMapFilePath.size());
				}
				pImg= loadImage(imageMapFilePath);
				if (pImg!=NULL)
					buseTexColor=true;
				else if (imageMapFilePath.length()>0)
					GlbLogWOutput(GLB_LOGTYPE_ERR,L"多边形水符号读取折射图像文件(%s)失败. \r\n",imageMapFilePath.c_str());

			}
			osg::Texture2D* specifyRefract_tex = loadTexture(pImg, osg::Texture::REPEAT);
			sset->setTextureAttributeAndModes(3, specifyRefract_tex, osg::StateAttribute::ON );
			sset->addUniform( new osg::Uniform("WaterPolygon_SpecifyRefractionMap", 3) );
			sset->addUniform( new osg::Uniform("PolygonUseTex", buseTexColor) );


			sset->addUniform( new osg::Uniform("WaterPolygon_ReflectionMap", 1) );
			sset->addUniform( new osg::Uniform("WaterPolygon_CameraRefractionMap", 2) );

			bool enableReflect=false;
			if (polygonWaterInfo->enableReflections)
			{
				enableReflect=polygonWaterInfo->enableReflections->value;
			}
			sset->addUniform( new osg::Uniform("waterPolygon_EnableReflections", enableReflect ) );

			// 风速、风向
			float windSpeed=1.0f;
			if (polygonWaterInfo->windStrong)
			{
				windSpeed=polygonWaterInfo->windStrong->value;
			}
			sset->addUniform( new osg::Uniform("windSpeed", windSpeed ) );
			windSpeed=0.0f;//吹向北方
			if (polygonWaterInfo->windDirection)
			{
				windSpeed=polygonWaterInfo->windDirection->value;
			}
			sset->addUniform( new osg::Uniform("windDirection", windSpeed) );

			osg::ref_ptr<osg::Program> program = createShader();
			if(program.valid())
				sset->setAttributeAndModes( program.get(), osg::StateAttribute::ON );

			return geometry.release();
		}

		osg::Geometry *buildGeometry(CGlbGlobeRObject *obj,IGlbGeometry *geo )
		{
			if (NULL==obj||geo==NULL)
				return NULL;
			GlbRenderInfo *polygonInfo = obj->GetRenderInfo();
			GlbPolygonWaterSymbolInfo *polygonWaterInfo= dynamic_cast<GlbPolygonWaterSymbolInfo *>(polygonInfo);
			if(polygonWaterInfo == NULL)
				return NULL;	

			//IGlbGeometry *pGlbGeometry=obj->GetGeometry();
			CGlbPolygon *pPolyGon=dynamic_cast<CGlbPolygon *>(geo);
			if (NULL==pPolyGon||pPolyGon->IsEmpty())
				return NULL;

			glbInt32 dimension = pPolyGon->GetCoordDimension();
			CGlbLine *pExtRing = const_cast<CGlbLine *>(pPolyGon->GetExtRing());
			if (NULL==pExtRing||pExtRing->IsEmpty())
				return NULL;

			osg::Vec3Array *points=new osg::Vec3Array();
			const glbDouble* pCoords=pExtRing->GetPoints();
			glbInt32 pntCnt=pExtRing->GetCount();
			for (glbInt32 i=0;i<pntCnt;++i)
			{
				osg::Vec3d pnt(pCoords[i*dimension],pCoords[i*dimension+1],((dimension>2) ? pCoords[i*dimension+2] : -10) );
				points->push_back(pnt);
			}

			glbUInt32 uniqueCnt=DelaunayTriangulator_uniqueifyPoints(points);// 判断多边形是否有效
			if (uniqueCnt<3)
				return NULL;

			glbBool isGlobe=obj->GetGlobe()->GetType()==GLB_GLOBETYPE_GLOBE;
			glbref_ptr<CGlbExtent> pExt=obj->GetBound(isGlobe);
			glbDouble cetX,cetY,cetZ;
			pExt->GetCenter(&cetX,&cetY,&cetZ);
			_objCenter=osg::Vec3d(cetX,cetY,cetZ);

			glbDouble xSize=pExt->GetXWidth();
			glbDouble ySize=pExt->GetYHeight();
			_objRadius=sqrt(xSize*xSize+ySize*ySize)/2.0*1.415;
			
			osg::Matrixd polygonOrient;
			osg::Matrixd polygonOrient_inv;
			if(isGlobe)
			{
				osg::EllipsoidModel myEllipsoidModel(g_ellipsoidModel->getRadiusEquator(),g_ellipsoidModel->getRadiusPolar());
				myEllipsoidModel.computeLocalToWorldTransformFromXYZ(cetX,cetY,cetZ,polygonOrient);
			}
			else
			{
				polygonOrient.makeTranslate(_objCenter);
			}
			polygonOrient_inv=polygonOrient.inverse(polygonOrient);

			osg::ref_ptr<osgUtil::DelaunayTriangulator> trig=new osgUtil::DelaunayTriangulator();
			trig->setInputPointArray(points);
			trig->triangulate();
			double PolySizeX,PolySizeY,PolyCentX,PolyCentY;
			osg::Vec2Array* norTexCoords = buildNormalCoords(points,PolySizeX,PolySizeY,PolyCentX,PolyCentY);//计算多边形纹理坐标
			double sizeX,sizeY,centX,centY;
			float texRotAngles=0.0f;
			if (polygonWaterInfo->refractTexRotation)
			{
				texRotAngles=polygonWaterInfo->refractTexRotation->value;
			}
			osg::Vec2Array* texcoords = buildTexCoordsEx(points,osg::DegreesToRadians(texRotAngles),sizeX,sizeY,centX,centY);//计算多边形纹理坐标
			
			osg::Vec3Array *newPoints=trig->getInputPointArray();
			osg::Vec3Array::size_type newPntCnt= newPoints->size();
			if (newPntCnt<3)
				return NULL;

			glbDouble newX,newY,newZ;
			if (isGlobe)
			{
				osg::ref_ptr<osg::Vec3dArray> polygonPlaneCoords=new osg::Vec3dArray();
				for (osg::Vec3Array::size_type i=0;i<newPntCnt;++i)
				{
					const osg::Vec3f &iPnt=newPoints->at(i);
					g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(iPnt.y()),osg::DegreesToRadians(iPnt.x()),iPnt.z(),
						newX,newY,newZ);
					newPoints->at(i)=osg::Vec3d(newX,newY,newZ)*polygonOrient_inv;
					polygonPlaneCoords->push_back(osg::Vec3d(newX,newY,newZ));
				}
				polygonWaterPlane=new osg::Plane(polygonPlaneCoords->at(0),polygonPlaneCoords->at(1),polygonPlaneCoords->at(2));
				polygonWaterPlane->makeUnitLength();
				polygonWaterPlane->flip();
			}
			else
			{
				for (osg::Vec3Array::size_type i=0;i<newPntCnt;++i)
				{
					osg::Vec3f &iPnt=newPoints->at(i);
					iPnt-=_objCenter;
				}
				polygonWaterPlane=new osg::Plane(newPoints->at(0),newPoints->at(1),newPoints->at(2));
				polygonWaterPlane->makeUnitLength();
				polygonWaterPlane->flip();
			}

			osg::Geometry *geom=new osg::Geometry();
			geom->setVertexArray(newPoints);
			geom->setTexCoordArray(0, norTexCoords);
			geom->setTexCoordArray(1, texcoords);
			geom->addPrimitiveSet(trig->getTriangles());

			osg::StateSet *sset=geom->getOrCreateStateSet();  
			sset->setMode( GL_CULL_FACE, osg::StateAttribute::ON );

			//sset->setRenderBinDetails( 0, "RenderBin");
			sset->setMode(GL_BLEND,osg::StateAttribute::ON);
			//sset->setMode(osg::PolygonMode::FRONT_AND_BACK,osg::PolygonMode::LINE);
			osg::ref_ptr<osg::BlendFunc> pBlendFun = new osg::BlendFunc(osg::BlendFunc::SRC_ALPHA,osg::BlendFunc::ONE_MINUS_SRC_ALPHA );	//设置混合方程
			_stateset->setAttributeAndModes(pBlendFun.get(),osg::StateAttribute::ON/*|osg::StateAttribute::OVERRIDE*/);
			sset->addUniform( new osg::Uniform("PolygonCenter", osg::Vec2f(PolyCentX,PolyCentY) ) );
			sset->addUniform( new osg::Uniform("PolygonWeightAndHeight", osg::Vec2f(PolySizeX,PolySizeY) ) );
			sset->addUniform( new osg::Uniform("PolygonRotateCenter", osg::Vec2f(centX,centY) ) );
			sset->addUniform( new osg::Uniform("PolygonRotateWeightAndHeight", osg::Vec2f(sizeX,sizeY) ) );

			sset->addUniform( new osg::Uniform("PolygonSelected", false) );
			sset->addUniform( new osg::Uniform("PolygonBlink", false) );

			

			// 1、透明
			glbInt32 polygonOpacity=100;
			if (polygonWaterInfo->opacity)
			{
				polygonOpacity=polygonWaterInfo->opacity->value;
			}
			if (polygonOpacity!=100)//透明
				sset->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
			else
				sset->setRenderingHint(osg::StateSet::OPAQUE_BIN);
			sset->addUniform( new osg::Uniform("PolygonOpacity", polygonOpacity ) );

			// 法线
			HMODULE hmd = GetModuleHandle(L"GlbGlobe.dll");
			CGlbWString workdir = CGlbPath::GetModuleDir(hmd);
			CGlbWString heightMapFilePath = workdir + L"\\res\\waterHeight.png";
			osg::Texture2D* waterHeight_tex = readTexture(heightMapFilePath, osg::Texture::MIRROR);
			sset->setTextureAttributeAndModes(0, waterHeight_tex, osg::StateAttribute::ON );
			sset->addUniform( new osg::Uniform("WaterPolygon_HeightMap", 0) );

			// 启用相机折射水底/或者贴水底折射贴图
			osg::Vec4f polygonColor(0.0,255.0,0.0,1.0);
			if (polygonWaterInfo->color)
			{
				glbInt32 plyClr=polygonWaterInfo->color->value;
				polygonColor.set(GetRValueInBGRA(plyClr),LOBYTE(((plyClr & 0xFFFF)) >> 8),GetBValueInBGRA(plyClr),1.0);
			}
			sset->addUniform( new osg::Uniform("PolygonColor", polygonColor) );
			bool enableRefract=false;
			if (polygonWaterInfo->enableRefractions)
			{
				enableRefract=polygonWaterInfo->enableRefractions->value;
			}
			sset->addUniform( new osg::Uniform("waterPolygon_EnableRefractions", enableRefract ) );
			bool buseTexColor=false;
			osg::Image *pImg=NULL;
			if (polygonWaterInfo->refractionImageData)
			{
				CGlbWString imageMapFilePath=polygonWaterInfo->refractionImageData->value;
				glbInt32 index = imageMapFilePath.find_first_of(L'.');
				if(index == 0)
				{// 处理当前执行文件的相对路径情况 ./AAA/....
					CGlbWString execDir = CGlbPath::GetExecDir();
					imageMapFilePath = execDir + imageMapFilePath.substr(1,imageMapFilePath.size());
				}
				pImg= loadImage(imageMapFilePath);
				if (pImg!=NULL)
					buseTexColor=true;
				else if (imageMapFilePath.length()>0)
					GlbLogWOutput(GLB_LOGTYPE_ERR,L"多边形水符号读取折射图像文件(%s)失败. \r\n",imageMapFilePath.c_str());
			}
			osg::Texture2D* specifyRefract_tex = loadTexture(pImg, osg::Texture::REPEAT);
			sset->setTextureAttributeAndModes(3, specifyRefract_tex, osg::StateAttribute::ON );
			sset->addUniform( new osg::Uniform("WaterPolygon_SpecifyRefractionMap", 3) );
			sset->addUniform( new osg::Uniform("PolygonUseTex", buseTexColor) );
			

			sset->addUniform( new osg::Uniform("WaterPolygon_ReflectionMap", 1) );
			sset->addUniform( new osg::Uniform("WaterPolygon_CameraRefractionMap", 2) );

			bool enableReflect=false;
			if (polygonWaterInfo->enableReflections)
			{
				enableReflect=polygonWaterInfo->enableReflections->value;
			}
			sset->addUniform( new osg::Uniform("waterPolygon_EnableReflections", enableReflect ) );

			// 风速、风向
			float windSpeed=1.0f;
			if (polygonWaterInfo->windStrong)
			{
				windSpeed=polygonWaterInfo->windStrong->value;
			}
			sset->addUniform( new osg::Uniform("windSpeed", windSpeed ) );
			windSpeed=0.0f;//吹向北方
			if (polygonWaterInfo->windDirection)
			{
				windSpeed=polygonWaterInfo->windDirection->value;
			}
			sset->addUniform( new osg::Uniform("windDirection", windSpeed) );

			osg::ref_ptr<osg::Program> program = createShader();
			if(program.valid())
				sset->setAttributeAndModes( program.get(), osg::StateAttribute::ON );


			polygonWaterOffset->preMult(polygonOrient);

			return geom;
		}
		bool buildNormalAndRefactionTexCoords(osg::Vec3Array *points,float rotAnge_rad,double &sizeX,double &sizeY,double &centX,double &centY,osg::Vec2Array *&refactioTexCord,osg::Vec2Array *&normalTexCord)const
		{
			osg::Vec3Array::size_type pntCnt = points->size();		
			refactioTexCord=new osg::Vec2Array(pntCnt);
			normalTexCord=new osg::Vec2Array(pntCnt);

			// 1、计算旋转中心
			double dmin_lon=std::numeric_limits<double>::max();
			double dmax_lon=std::numeric_limits<double>::min();
			double dmin_lat=std::numeric_limits<double>::max();
			double dmax_lat=std::numeric_limits<double>::min();
			osg::Vec3Array::const_iterator iterBegin=points->begin();
			osg::Vec3Array::const_iterator iterEnd=points->end();
			for (osg::Vec3Array::const_iterator iter=iterBegin;iter!=iterEnd;++iter)
			{
				if (iter->x()<dmin_lon)
					dmin_lon=iter->x();
				if (iter->x()>dmax_lon)
					dmax_lon=iter->x();
				if (iter->y()<dmin_lat)
					dmin_lat=iter->y();
				if (iter->y()>dmax_lat)
					dmax_lat=iter->y();
			}
			/*sizeX=dmax_lon-dmin_lon;
			sizeY=dmax_lat-dmin_lat;*/
			centX=(dmax_lon+dmin_lon)/2.0;
			centY=(dmax_lat+dmin_lat)/2.0;//旋转中心

			// 2、旋转
			osg::Vec3f rotCenter(centX,centY,0.0);
			osg::Matrixf rotMat=osg::Matrixf::rotate(rotAnge_rad,osg::Vec3f(0.0,0.0,1.0));		
			pntCnt=0;
			osg::Vec3d newUV;
			/*double */dmin_lon=std::numeric_limits<double>::max();
			/*double */dmax_lon=std::numeric_limits<double>::min();
			/*double */dmin_lat=std::numeric_limits<double>::max();
			/*double */dmax_lat=std::numeric_limits<double>::min();
			for (osg::Vec3Array::const_iterator iter=iterBegin;iter!=iterEnd;++iter)//旋转
			{
				newUV=*iter-rotCenter;
				refactioTexCord->at(pntCnt).set(newUV.x(),newUV.y());
				newUV=(*iter-rotCenter)*rotMat;
				refactioTexCord->at(pntCnt).set(newUV.x(),newUV.y());

				if (newUV.x()<dmin_lon)
					dmin_lon=newUV.x();
				if (newUV.x()>dmax_lon)
					dmax_lon=newUV.x();
				if (newUV.y()<dmin_lat)
					dmin_lat=newUV.y();
				if (newUV.y()>dmax_lat)
					dmax_lat=newUV.y();

				++pntCnt;
			}
			centX=(dmax_lon+dmin_lon)/2.0;
			centY=(dmax_lat+dmin_lat)/2.0;//旋转中心重新计算
			sizeX=dmax_lon-dmin_lon;
			sizeY=dmax_lat-dmin_lat;

			return true;
		}
		osg::Vec2Array * buildNormalCoords(osg::Vec3Array *points,double &sizeX,double &sizeY,double &centX,double &centY)const
		{
			osg::Vec3Array::size_type pntCnt = points->size();		
			osg::Vec2Array *norTexCoords=new osg::Vec2Array(pntCnt);

			// 1、计算旋转中心
			double dmin_lon=1.0E25;//std::numeric_limits<double>::max();
			double dmax_lon=-dmin_lon;//std::numeric_limits<double>::min();
			double dmin_lat=1.0E25;//std::numeric_limits<double>::max();
			double dmax_lat=-dmin_lat;//std::numeric_limits<double>::min();
			double dmin_alt=1.0E25;//std::numeric_limits<double>::max();
			double dmax_alt=-dmin_alt;//std::numeric_limits<double>::min();
			osg::Vec3Array::const_iterator iterBegin=points->begin();
			osg::Vec3Array::const_iterator iterEnd=points->end();
			for (osg::Vec3Array::const_iterator iter=iterBegin;iter!=iterEnd;++iter)
			{
				if (iter->x()<dmin_lon)
					dmin_lon=iter->x();
				if (iter->x()>dmax_lon)
					dmax_lon=iter->x();
				if (iter->y()<dmin_lat)
					dmin_lat=iter->y();
				if (iter->y()>dmax_lat)
					dmax_lat=iter->y();
				if (iter->z()<dmin_alt)
					dmin_alt=iter->z();
				if (iter->z()>dmax_alt)
					dmax_alt=iter->z();
			}
			double cx = dmax_lon-dmin_lon;
			double cy = dmax_lat-dmin_lat;
			double cz = dmax_alt-dmin_alt;

			sizeX=cx;
			sizeY=cy;
			centX=(dmax_lon+dmin_lon)/2.0;
			centY=(dmax_lat+dmin_lat)/2.0;//旋转中心

			int projMode = 0; // 0- XY平面投影 1-YZ平面， 2-XZ平面
			if (mpr_normal.x()>mpr_normal.y() && mpr_normal.x()>mpr_normal.z())
				projMode = 1;
			if (mpr_normal.y()>mpr_normal.x() && mpr_normal.y()>mpr_normal.z())
				projMode = 2;
			// 2、旋转
			osg::Vec3f rotCenter(centX,centY,0.0);
			pntCnt=0;
			osg::Vec3d newUV;
			for (osg::Vec3Array::const_iterator iter=iterBegin;iter!=iterEnd;++iter)//旋转
			{
				//newUV=(*iter-rotCenter);
				newUV=(*iter);
				switch (projMode)
				{
				case 0:
					norTexCoords->at(pntCnt).set(newUV.x(),newUV.y());
					break;
				case 1:
					{
						norTexCoords->at(pntCnt).set(newUV.y(),newUV.z());
						sizeX=cy;
						sizeY=cz;
						centX=(dmax_lat+dmin_lat)/2.0;
						centY=(dmax_alt+dmin_alt)/2.0;//旋转中心
					}
					break;
				case 2:
					{
						norTexCoords->at(pntCnt).set(newUV.x(),newUV.z());
						sizeX=cx;
						sizeY=cz;
						centX=(dmax_lon+dmin_lon)/2.0;
						centY=(dmax_alt+dmin_alt)/2.0;//旋转中心
					}					
					break;
				}
				++pntCnt;
			}

			return norTexCoords;
		}
		osg::Vec2Array * buildTexCoordsEx(osg::Vec3Array *points,float rotAnge_rad,double &sizeX,double &sizeY,double &centX,double &centY)const
	{
		osg::Vec3Array::size_type pntCnt = points->size();		
		osg::Vec2Array *texCoords=new osg::Vec2Array(pntCnt);

		// 1、计算旋转中心
		//double dmin_lon=std::numeric_limits<double>::max();
		//double dmax_lon=std::numeric_limits<double>::min();
		//double dmin_lat=std::numeric_limits<double>::max();
		//double dmax_lat=std::numeric_limits<double>::min();
		//double dmin_alt=std::numeric_limits<double>::max();
		//double dmax_alt=std::numeric_limits<double>::min();
		double dmin_lon=1.0E25;//std::numeric_limits<double>::max();
		double dmax_lon=-dmin_lon;//std::numeric_limits<double>::min();
		double dmin_lat=1.0E25;//std::numeric_limits<double>::max();
		double dmax_lat=-dmin_lat;//std::numeric_limits<double>::min();
		double dmin_alt=1.0E25;//std::numeric_limits<double>::max();
		double dmax_alt=-dmin_alt;//std::numeric_limits<double>::min();
		osg::Vec3Array::const_iterator iterBegin=points->begin();
		osg::Vec3Array::const_iterator iterEnd=points->end();
		for (osg::Vec3Array::const_iterator iter=iterBegin;iter!=iterEnd;++iter)
		{
			if (iter->x()<dmin_lon)
				dmin_lon=iter->x();
			if (iter->x()>dmax_lon)
				dmax_lon=iter->x();
			if (iter->y()<dmin_lat)
				dmin_lat=iter->y();
			if (iter->y()>dmax_lat)
				dmax_lat=iter->y();
			if (iter->z()<dmin_alt)
				dmin_alt=iter->z();
			if (iter->z()>dmax_alt)
				dmax_alt=iter->z();
		}
		/*sizeX=dmax_lon-dmin_lon;
		sizeY=dmax_lat-dmin_lat;*/
		centX=(dmax_lon+dmin_lon)/2.0;
		centY=(dmax_lat+dmin_lat)/2.0;//旋转中心
		double centZ = (dmax_alt+dmin_alt)/2.0;
		
		double cx = dmax_lon-dmin_lon;
		double cy = dmax_lat-dmin_lat;
		double cz = dmax_alt-dmin_alt;

		int projMode = 0; // 0- XY平面投影 1-YZ平面， 2-XZ平面
		if (mpr_normal.x()>mpr_normal.y() && mpr_normal.x()>mpr_normal.z())
			projMode = 1;
		if (mpr_normal.y()>mpr_normal.x() && mpr_normal.y()>mpr_normal.z())
			projMode = 2;
		// 2、旋转
		osg::Vec3f rotCenter(centX,centY,0.0);		
		osg::Matrixf rotMat=osg::Matrixf::rotate(rotAnge_rad,osg::Vec3f(0.0,0.0,1.0));		
		if (projMode==1)
		{
			rotCenter.set(0,centY,centZ);
			rotMat=osg::Matrixf::rotate(rotAnge_rad,osg::Vec3f(1.0,0.0,0.0));	
		}
		else if (projMode==2)
		{
			rotCenter.set(centX,0,centZ);
			rotMat=osg::Matrixf::rotate(rotAnge_rad,osg::Vec3f(0.0,1.0,0.0));	
		}
		pntCnt=0;
		osg::Vec3d newUV;
		dmin_lon=1.0E25;//std::numeric_limits<double>::max();
		dmax_lon=-dmin_lon;//std::numeric_limits<double>::min();
		dmin_lat=1.0E25;//std::numeric_limits<double>::max();
		dmax_lat=-dmin_lat;//std::numeric_limits<double>::min();
		dmin_alt=1.0E25;//std::numeric_limits<double>::max();
		dmax_alt=-dmin_alt;//std::numeric_limits<double>::min();
		for (osg::Vec3Array::const_iterator iter=iterBegin;iter!=iterEnd;++iter)//旋转
		{
			//texCoords->at(pntCnt).set(iter->x(),iter->y());
			//newUV=(*iter/*-rotCenter*/)*rotMat;
			newUV=(*iter-rotCenter)*rotMat;
			switch(projMode)
			{
			case 0:
				{//XY				
					texCoords->at(pntCnt).set(newUV.x(),newUV.y());
					//centX=(dmax_lon+dmin_lon)/2.0;
					//centY=(dmax_lat+dmin_lat)/2.0;//旋转中心重新计算
					//sizeX=dmax_lon-dmin_lon;
					//sizeY=dmax_lat-dmin_lat;

					if (newUV.x()<dmin_lon)
						dmin_lon=newUV.x();
					if (newUV.x()>dmax_lon)
						dmax_lon=newUV.x();
					if (newUV.y()<dmin_lat)
						dmin_lat=newUV.y();
					if (newUV.y()>dmax_lat)
						dmax_lat=newUV.y();
				}				
				break;
			case 1:
				{//YZ				
					texCoords->at(pntCnt).set(newUV.y(),newUV.z());
					//centX=(dmax_lat+dmin_lat)/2.0;
					//centY=(dmax_alt+dmin_alt)/2.0;//旋转中心重新计算
					//sizeX=dmax_lat-dmin_lat;
					//sizeY=dmax_alt-dmin_alt;

					if (newUV.z()<dmin_alt)
						dmin_alt=newUV.z();
					if (newUV.z()>dmax_alt)
						dmax_alt=newUV.z();
					if (newUV.y()<dmin_lat)
						dmin_lat=newUV.y();
					if (newUV.y()>dmax_lat)
						dmax_lat=newUV.y();
				}				
				break;
			case 2:
				{//XZ
					texCoords->at(pntCnt).set(newUV.x(),newUV.z());
					//centX=(dmax_lon+dmin_lon)/2.0;
					//centY=(dmax_alt+dmin_alt)/2.0;//旋转中心重新计算
					//sizeX=dmax_lon-dmin_lon;
					//sizeY=dmax_alt-dmin_alt;

					if (newUV.x()<dmin_lon)
						dmin_lon=newUV.x();
					if (newUV.x()>dmax_lon)
						dmax_lon=newUV.x();
					if (newUV.z()<dmin_alt)
						dmin_alt=newUV.z();
					if (newUV.z()>dmax_alt)
						dmax_alt=newUV.z();
				}				
				break;
			}
			
			/*newUV=(*iter-rotCenter)*rotMat;
			texCoords->at(pntCnt).set(newUV.x(),newUV.y());*/
			++pntCnt;
		}

		switch(projMode)
		{
		case 0:
			{//XY
				centX=(dmax_lon+dmin_lon)/2.0;
				centY=(dmax_lat+dmin_lat)/2.0;//旋转中心重新计算
				sizeX=dmax_lon-dmin_lon;
				sizeY=dmax_lat-dmin_lat;
			}break;
		case 1:
			{//YZ
				centX=(dmax_lat+dmin_lat)/2.0;
				centY=(dmax_alt+dmin_alt)/2.0;//旋转中心重新计算
				sizeX=dmax_lat-dmin_lat;
				sizeY=dmax_alt-dmin_alt;
			}break;
		case 2:
			{//XZ
				centX=(dmax_lon+dmin_lon)/2.0;
				centY=(dmax_alt+dmin_alt)/2.0;
				sizeX=dmax_lon-dmin_lon;
				sizeY=dmax_alt-dmin_alt;
			}break;
		}
		//centX=(dmax_lon+dmin_lon)/2.0;
		//centY=(dmax_lat+dmin_lat)/2.0;//旋转中心重新计算
		//sizeX=dmax_lon-dmin_lon;
		//sizeY=dmax_lat-dmin_lat;

		//// 3、计算旋转以后的uv
		//pntCnt=0;
		//osg::Vec2f downLeft(dmin_lon,dmin_lat);
		//osg::Vec2f rotCenter2(centX,centY);
		//osg::Vec2Array::iterator iterBegin_New=texCoords->begin();
		//osg::Vec2Array::iterator iterEnd_New=texCoords->end();
		//for (osg::Vec2Array::iterator iter=iterBegin_New;iter!=iterEnd_New;++iter)
		////for (osg::Vec3Array::const_iterator iter=iterBegin;iter!=iterEnd;++iter)
		//{
		//	//texCoords->at(pntCnt).set((iter->x()-dmin_lon)/sizeX,(iter->y()-dmin_lat)/sizeY);
		//	//texCoords->at(pntCnt).set((iter->x()+centX-dmin_lon)/sizeX,(iter->y()+centY-dmin_lat)/sizeY);
		//	/**iter=*iter+rotCenter2-downLeft;
		//	iter->set(iter->x()/sizeX,iter->y()/sizeY);*/


		//	++pntCnt;
		//}
		return texCoords;
	}

		osg::ref_ptr<osg::MatrixTransform>   polygonWaterOffset;
		osg::Plane *polygonWaterPlane;
		CGlbGlobeRObject *_obj;
		IGlbGeometry *_glbgeo;
		osg::Vec3d _objCenter;
		glbDouble _objRadius;
		bool _buildSuccess;
		bool _isDirty;
		bool _isStateDirty;
		osg::Uniform *pEyePosUniform;

		osg::Matrixf                 _reflectionMirrorMatrix;
		osg::Matrixf                 _reflectionViewMatrix;
		osg::Matrixf                 _reflectionProjMatrix;
	};
	//////////////////////////////////////////////////////////////////////////
	class CWaterScene : public osg::Group,public IWaterScene
	{
	public:
		CWaterScene(CGlbGlobeRObject *obj,IGlbGeometry *geometry):_reflectionTexSize( 512,512 ),_refractionTexSize( 512,512 )
		{
			_isDirty=true;
			_obj=obj;			
			_waterSurface=NULL;


			_reflectionSceneMask=0x1 ;
			_refractionSceneMask=0x2 ;
			_surfaceMask=0x8,
			_enableReflections=true;
			_enableRefractions=true;
			_heightUnit=0;
			_reflectionUnit=1;
			_refractionUnit=2;
			_surfaceStateSet = new osg::StateSet();

			CWaterSurface *_waterSurfaceTmp = new CWaterSurface(_obj,geometry);
			_buildSuccess=_waterSurfaceTmp->buildSuccess();
			if (!_buildSuccess)
				return;
			_waterSurface=_waterSurfaceTmp;
			_waterSurface->setNodeMask(_surfaceMask);

			//osg::Switch *pPolyGonSW=new osg::Switch();
			_switch = new osg::Switch();
			_switch->setNodeMask(_surfaceMask);
			_switch->addChild(_waterSurface);
			//_switch=pPolyGonSW;

			_waterPolygonOffsetMT=_waterSurface->getPolygonWaterOffsetMT();
			//_waterPolygonOffsetMT->addChild(_waterSurface);
			_waterPolygonOffsetMT->addChild(_switch.get());

			_waterPolygonOffsetMT->setNodeMask(_surfaceMask);
			addChild(_waterPolygonOffsetMT.get());
			this->setNodeMask(_surfaceMask);
		}
		~CWaterScene(){}
		bool buildSuccess()const{return _buildSuccess;} 

		void enableReflections( bool enable )
		{
			if (_enableReflections != enable)
			{
				_enableReflections = enable;
				_isDirty = true;
			}			
		}
		void enableRefractions( bool enable )
		{
			if (_enableRefractions != enable)
			{
				_enableRefractions = enable;
				_isDirty = true;
			}
		}
	private:
		class CameraCullCallback : public osg::NodeCallback
		{
		public:
			CameraCullCallback(CWaterScene *waterScene)
			{
				_waterScene=waterScene;
				CGlbGlobe *pGlobe=_waterScene->_obj->GetGlobe();
				CGlbGlobeView *pView=pGlobe->GetView();
				osg::Camera *pMainCam=pView->GetOsgCamera();
				_Group=pMainCam->getChild(0);
				_Group=pView->getReflectNode();
				//_Group=pView->mpr_p_starsNode.get();
				//_Group=pView->getTestNode();
			}
			virtual void operator()(osg::Node *nd, osg::NodeVisitor *nv)
			{
				_waterScene->osg::Group::traverse(*nv);
				//_Group->traverse(*nv);
			}
		private:
			CWaterScene *_waterScene;
			//osg::Group *_Group;
			osg::Node *_Group;
		};
		/// prerender
		class PrerenderCameraCullCallback : public osg::NodeCallback
		{
		public:
			PrerenderCameraCullCallback(CWaterScene *waterScene)
			{
				_waterScene=waterScene;
			}
			virtual void operator()(osg::Node*, osg::NodeVisitor* nv)
			{
				osgUtil::CullVisitor* cv = dynamic_cast<osgUtil::CullVisitor*> (nv);
				if(cv!=NULL)
				{
					bool eyeAboveWater  = _waterScene->isEyeAboveWaterSurface(cv->getEyePoint());
					bool surfaceVisible = _waterScene->getWaterSurface()->isVisible(*cv, eyeAboveWater);
					_waterScene->cull(*cv, eyeAboveWater, surfaceVisible);
				}
			}
		private:
			CWaterScene* _waterScene;
		};
		void traverse(osg::NodeVisitor& nv)
		{
			if( nv.getVisitorType() == osg::NodeVisitor::UPDATE_VISITOR )
			{
				if( _isDirty )
					init();

				update(nv);

				osg::Group::traverse(nv);
			}
			else if (nv.getVisitorType() == osg::NodeVisitor::CULL_VISITOR)
			{
				/*osg::Group::traverse(nv);
				return;*/
				osgUtil::CullVisitor* cv = dynamic_cast<osgUtil::CullVisitor*>(&nv);
				if (cv!=NULL) 
				{
					if (cv->getCurrentCamera()->getName() == "ShadowCamera" ||
						cv->getCurrentCamera()->getName() == "AnalysisCamera" )
						// Do not do reflections and everything if we're in a shadow pass.
						osg::Group::traverse(nv);
					else
					{
						bool eyeAboveWater  = isEyeAboveWaterSurface(cv->getEyePoint());

						//// Switch the fog state from underwater to above water or vice versa if necessary.
						//if (_eyeAboveWaterPreviousFrame != eyeAboveWater)
						//{
						//	_fog->setDensity(eyeAboveWater ? _aboveWaterFogDensity : _underwaterFogDensity);
						//	_fog->setColor(eyeAboveWater ? _aboveWaterFogColor : _underwaterFogColor);
						//	_eyeAboveWaterPreviousFrame = eyeAboveWater;
						//}

						bool surfaceVisible = _waterSurface->isVisible(*cv, eyeAboveWater);
						if (_waterSurface->getCullCallback())
							(*_waterSurface->getCullCallback())(_waterSurface.get(), &nv);
						

						preRenderCull(*cv, eyeAboveWater, surfaceVisible);     // reflections/refractions

						// Above water
						if( eyeAboveWater )
						{
							if(true)
								cull(*cv, eyeAboveWater, surfaceVisible);        // normal scene render
						}
						//// Below water passes
						//else 
						//{
						//	if(!_enableDOF)
						//		cull(*cv, eyeAboveWater, surfaceVisible);        // normal scene render
						//}

						postRenderCull(*cv, eyeAboveWater, surfaceVisible);    // god rays/dof/glare
					}
				}
				else
					osg::Group::traverse(nv);
			}
			else
				osg::Group::traverse(nv);
		}
		void init()
		{
			 //_refractionCamera = NULL;
			 //if( _reflectionClipNode.valid() )
			 //{
				// removeChild( _reflectionClipNode.get() );
				// _reflectionClipNode = NULL;
			 //}
			 _refractionCamera = NULL;

			if (_waterSurface.valid())
			{
				_globalStateSet = new osg::StateSet;

				_surfaceStateSet = new osg::StateSet;
				//_surfaceStateSet = _waterSurface->getStateSet();
				_surfaceStateSet->setMode( GL_CULL_FACE, osg::StateAttribute::ON);
				_surfaceStateSet->addUniform( new osg::Uniform("ViewMatrix_ReflectCam", osg::Matrixf()) );
				_surfaceStateSet->addUniform( new osg::Uniform("ProjectionMatrix_ReflectCam", osg::Matrixf()) );

				_surfaceStateSet->addUniform( new osg::Uniform("waterPolygon_EnableReflections", _enableReflections ) );
				_surfaceStateSet->addUniform( new osg::Uniform("waterPolygon_EnableRefractions", _enableRefractions ) );
				//_surfaceStateSet->addUniform( new osg::Uniform("WaterPolygon_HeightMap", _heightUnit ) );
				_surfaceStateSet->addUniform( new osg::Uniform("WaterPolygon_ReflectionMap", _reflectionUnit ) );//1
				_surfaceStateSet->addUniform( new osg::Uniform("WaterPolygon_CameraRefractionMap", 2) );
				//_surfaceStateSet->addUniform( new osg::Uniform("WaterPolygon_SpecifyRefractionMap", 3) );
	
				
				if( _enableReflections )
				{
					osg::Plane *plane=_waterSurface->getPolygonWaterSurface();
					osg::Vec3d nor=-plane->getNormal();
					double nd=-plane->distance(osg::Vec3d());
					calReflectionMatrix(nor.x(),nor.y(),nor.z(),nd);
					_reflectionTexSize=osg::Vec2s(1024,1024);
					osg::ref_ptr<osg::Texture2D> reflectionTexture = createTexture2D( _reflectionTexSize, GL_RGBA);

					// clip everything below water line
					_reflectionCamera=renderToTexturePass( reflectionTexture.get() );
					_reflectionCamera->setClearColor( osg::Vec4( 0.0, 0.0, 0.0, 0.0 ) );
					//_reflectionCamera->setClearColor( osg::Vec4( 0.1, 0.1, 0.1, 0.1 ) ); // test
					//_reflectionCamera->setCullMask( _reflectionSceneMask );
					_reflectionCamera->setCullMask(~_surfaceMask);
					_reflectionCamera->setCullCallback( new CameraCullCallback(this) );
					_reflectionCamera->getOrCreateStateSet()->setMode( GL_CLIP_PLANE0+0, osg::StateAttribute::ON );
					_reflectionCamera->getOrCreateStateSet()->setMode( GL_CULL_FACE, osg::StateAttribute::ON );
					/*_reflectionCamera->setComputeNearFarMode( osg::CullSettings::DO_NOT_COMPUTE_NEAR_FAR);
					_reflectionCamera->setCullingMode(osg::CullSettings::VIEW_FRUSTUM_CULLING);*/
					//_reflectionCamera->setComputeNearFarMode( osg::CullSettings::COMPUTE_NEAR_FAR_USING_BOUNDING_VOLUMES);
					//_reflectionCamera->setProjectionResizePolicy(osg::Camera::FIXED);

					_surfaceStateSet->setTextureAttributeAndModes( _reflectionUnit, reflectionTexture.get(), osg::StateAttribute::ON );

					//osg::ClipPlane* refClipPlane = new osg::ClipPlane(0);
					////refClipPlane->setClipPlane(nor.x(),nor.y(),nor.z(),-nd);
					//refClipPlane->setClipPlane(*plane);
					//_reflectionClipNode = new osg::ClipNode;
					//_reflectionClipNode->addClipPlane( refClipPlane );

					//addChild( _reflectionClipNode.get() );
				}
				if (_enableRefractions)
				{
					_refractionTexSize=osg::Vec2s(512,512);
					osg::ref_ptr<osg::Texture2D> refractionTexture = createTexture2D( _refractionTexSize, GL_RGBA );
					_refractionCamera=renderToTexturePass(refractionTexture);
					_refractionCamera->setClearColor(osg::Vec4( 0.0, 0.0, 0.0, 0.0));
					_refractionCamera->setCullMask(~_surfaceMask);
					_refractionCamera->setCullCallback(new CameraCullCallback(this));

					_surfaceStateSet->setTextureAttributeAndModes( _refractionUnit, refractionTexture, osg::StateAttribute::ON );

					osg::ref_ptr<osg::Texture2D> refractionTexture2 = createTexture2D( _refractionTexSize, GL_RGBA );
					_surfaceStateSet->setTextureAttributeAndModes( 3, refractionTexture2, osg::StateAttribute::ON );
				}
			}


			_isDirty = false;
		}
		void update( osg::NodeVisitor& nv )
		{
			/*if( _enableGodRays && _godrays.valid() )
				_godrays->accept(nv);

			if( _enableGodRays && _godRayBlendSurface.valid() )
				_godRayBlendSurface->accept(nv);

			if( _enableDistortion && _distortionSurface.valid() )
				_distortionSurface->accept(nv);*/
		}
		bool isEyeAboveWaterSurface(const osg::Vec3 &eye)
		{
			//return (eye.z() >= getOceanSurfaceHeight());
			return true;
		}
		CWaterSurface *getWaterSurface(){return _waterSurface;}
		void preRenderCull( osgUtil::CullVisitor& cv, bool eyeAboveWater, bool surfaceVisible )
		{
			// Above water
			if( eyeAboveWater )
			{
				//bool reflectionVisible = cv.getEyePoint().z() < _eyeHeightReflectionCutoff - getOceanSurfaceHeight();
				bool reflectionVisible = true;
				//bool refractionVisible = cv.getEyePoint().z() > _eyeHeightRefractionCutoff - getOceanSurfaceHeight();
				bool refractionVisible = true;
				

				bool b_reflection=_enableReflections && surfaceVisible && reflectionVisible &&
					_waterSurface.valid() && _reflectionCamera.valid();
				bool b_refraction=_enableRefractions && surfaceVisible && refractionVisible &&
					_waterSurface.valid() && _refractionCamera.valid();
				if (b_reflection||b_refraction)
				{
					osg::Vec3d const &objCenter=_waterSurface->getObjCenter();
					glbDouble objRadius=_waterSurface->getObjRadius();

					osg::Camera *mainCam=cv.getCurrentCamera();
					osg::Vec3d eye,cent,up;
					mainCam->getViewMatrixAsLookAt(eye,cent,up);
					double fovy,aspectRatio,zNear,zFar;
					mainCam->getProjectionMatrixAsPerspective(fovy,aspectRatio,zNear,zFar);
					
					glbDouble len=(eye-objCenter).length();
					double newfovy=osg::RadiansToDegrees(atan(objRadius/len)*2.0);

					// Render reflection if ocean surface is visible.
					if(b_reflection)
						/*if( _enableReflections && surfaceVisible && reflectionVisible &&
						_waterSurface.valid() && _reflectionCamera.valid() )*/
					{
						// update reflection camera and render reflected scene
						_waterSurface->_reflectionViewMatrix.makeIdentity();
						_waterSurface->_reflectionViewMatrix.makeLookAt(eye,objCenter,up);
						_waterSurface->_reflectionViewMatrix=mainCam->getViewMatrix();//测试 使用和主相机一样的视角
						_reflectionCamera->setViewMatrix(_waterSurface->_reflectionMirrorMatrix*_waterSurface->_reflectionViewMatrix);
						_waterSurface->_reflectionProjMatrix.makeIdentity();
						_waterSurface->_reflectionProjMatrix.makePerspective(newfovy,1.0,zNear,zFar+1000000);
						_waterSurface->_reflectionProjMatrix=mainCam->getProjectionMatrix();//测试 使用和主相机投影
						_reflectionCamera->setProjectionMatrix(_waterSurface->_reflectionProjMatrix);

						cv.pushStateSet(_globalStateSet.get());
						_reflectionCamera->accept( cv );
						cv.popStateSet();
					}

					// Render refraction if ocean surface is visible.
					if(b_refraction)
						/*if( _enableRefractions && surfaceVisible && refractionVisible &&
						_waterSurface.valid() && _refractionCamera.valid() )*/
					{
						// update refraction camera and render refracted scene
						_refractionCamera->setViewMatrix(mainCam->getViewMatrix() );
						_refractionCamera->setProjectionMatrix(mainCam->getProjectionMatrix());
						cv.pushStateSet(_globalStateSet.get());
						_refractionCamera->accept( cv );
						cv.popStateSet();
					}
					//if( _enableGlare )
					//{
					//	// set view and projection to match main camera
					//	_glarePasses.at(0)->setViewMatrix( cv.getCurrentCamera()->getViewMatrix() );
					//	_glarePasses.at(0)->setProjectionMatrix( cv.getCurrentCamera()->getProjectionMatrix() );

					//	for( unsigned int i=0; i<_glarePasses.size()-1; ++i )
					//	{
					//		_glarePasses.at(i)->accept(cv);
					//	}
					//}
				}
			}
			// Below water
			else
			{
				//bool refractionVisible = cv.getEyePoint().z() > _eyeHeightRefractionCutoff - getOceanSurfaceHeight();
				//_surfaceStateSet->getUniform("osgOcean_EnableRefractions")->set(refractionVisible);

				//// Render refraction if ocean surface is visible.
				//if( _enableRefractions && surfaceVisible && refractionVisible &&
				//	_oceanSurface.valid() && _refractionCamera.valid() )
				//{
				//	// update refraction camera and render refracted scene
				//	_refractionCamera->setViewMatrix( cv.getCurrentCamera()->getViewMatrix() );
				//	_refractionCamera->setProjectionMatrix( cv.getCurrentCamera()->getProjectionMatrix() );
				//	cv.pushStateSet(_globalStateSet.get());
				//	_refractionCamera->accept( cv );
				//	cv.popStateSet();
				//}

				//if( _enableGodRays && _godrayPreRender.valid() )
				//{
				//	// Render the god rays to texture
				//	_godrayPreRender->setViewMatrix( cv.getCurrentCamera()->getViewMatrix() );
				//	_godrayPreRender->setProjectionMatrix( cv.getCurrentCamera()->getProjectionMatrix() );
				//	_godrayPreRender->accept( cv );
				//}

				//if( _enableDOF )
				//{
				//	// set view and projection to match main camera
				//	_dofPasses.at(0)->setViewMatrix( cv.getCurrentCamera()->getViewMatrix() );
				//	_dofPasses.at(0)->setProjectionMatrix( cv.getCurrentCamera()->getProjectionMatrix() );

				//	// pass the cull visitor down the chain
				//	for(unsigned int i = 0; i<_dofPasses.size()-1; ++i)
				//	{
				//		_dofPasses.at(i)->accept(cv);
				//	}
				//}
			}
		}
		void cull(osgUtil::CullVisitor& cv, bool eyeAboveWater, bool surfaceVisible)
		{
			 unsigned int mask = cv.getTraversalMask();
			cv.pushStateSet(_globalStateSet.get());
			if ( _waterSurface.valid() && _waterSurface->getNodeMask() != 0 && surfaceVisible )
			{
				//_surfaceStateSet->getUniform("water_EyePosition")->set( cv.getEyePoint() );
				_surfaceStateSet->getUniform("ViewMatrix_ReflectCam")->set(/*_waterSurface->_reflectionMirrorMatrix**/_waterSurface->_reflectionViewMatrix);
				_surfaceStateSet->getUniform("ProjectionMatrix_ReflectCam")->set(_waterSurface->_reflectionProjMatrix);

				// HACK: Make sure masks are set correctly on children... This 
				// assumes that the ocean surface is the only child that should have
				// the _surfaceMask bit set. Otherwise other children will be
				// rendered twice.
				for (unsigned int i = 0; i < getNumChildren(); ++i)
				{
					osg::Node* child = getChild(i);
					if (child != _waterPolygonOffsetMT.get())
						child->setNodeMask((child->getNodeMask() & ~_surfaceMask)/* | _normalSceneMask*/);
				}

				// render ocean surface with reflection / refraction stateset
				cv.pushStateSet( _surfaceStateSet.get() );
				cv.setTraversalMask( mask & _surfaceMask );
				osg::Group::traverse(cv);
				// pop surfaceStateSet
				cv.popStateSet();
			}
			// render rest of scene
			cv.setTraversalMask( mask/* & _normalSceneMask */);
			osg::Group::traverse(cv);
			// pop globalStateSet
			cv.popStateSet(); 

			if( !eyeAboveWater )
			{
				/*if( _enableSilt )
				{
					cv.setTraversalMask( mask & _siltMask );
					osg::Group::traverse(cv);
				}*/
			}

			// put original mask back
			cv.setTraversalMask( mask );
		}
		void postRenderCull( osgUtil::CullVisitor& cv, bool eyeAboveWater, bool surfaceVisible )
		{
		}
		void calReflectionMatrix(osg::Matrixf::value_type nx,osg::Matrixf::value_type ny,osg::Matrixf::value_type nz,osg::Matrixf::value_type d)
		{//http://www.cppblog.com/Leaf/archive/2013/06/15/201017.html
			_waterSurface->_reflectionMirrorMatrix.set(1-2*nx*nx,-2*nx*ny, -2*nx*nz, 0,
				                  -2*ny*nx, 1-2*ny*ny,-2*ny*nz, 0,
								  -2*nz*nx, -2*nz*ny, 1-2*nz*nz,0,
								  -2*d*nx,  -2*d*ny,  -2*d*nz,  1);
		}


		unsigned int                 _reflectionSceneMask;
		unsigned int                 _refractionSceneMask;
		unsigned int                  _surfaceMask;
		//osg::Matrixf                 _reflectionMirrorMatrix;
		//osg::Matrixf                 _reflectionViewMatrix;
		//osg::Matrixf                 _reflectionProjMatrix;

		bool                         _enableReflections;
		bool                         _enableRefractions;
		int                          _heightUnit;
		int                          _reflectionUnit;
		int                          _refractionUnit;
		osg::Vec2s                   _reflectionTexSize;
		osg::Vec2s                   _refractionTexSize;
		
		float                        _fresnelMul;            /**< Fresnel multiplier uniform, typical values: (0.5-0.8). */
		osg::Vec2f                   _windDirection;         /**< Direction of wind. */
		osg::Vec2f                   _noiseWindDirection;    /**< Direction of wind for noise tile. */
		float                        _windSpeed;             /**< Wind speed m/s */
		float                        _noiseWindSpeed;        /**< Wind speed for noise tile m/s */
		float                        _waveScale;             /**< Wave scale modifier. */
		float                        _noiseWaveScale;        /**< Wave scale modifier for noise tile. */
		float                        _depth;                 /**< Depth (m). */
		bool                         _isStateDirty;

		osg::ref_ptr<osg::StateSet>  _globalStateSet;
		osg::ref_ptr<osg::StateSet>  _surfaceStateSet;


		osg::ref_ptr<osg::ClipNode>  _reflectionClipNode;
		osg::ref_ptr<osg::Camera>    _reflectionCamera;
		osg::ref_ptr<osg::Camera>    _refractionCamera;


		osg::ref_ptr<osg::MatrixTransform>  _waterPolygonOffsetMT;
		osg::ref_ptr<osg::Switch>           _switch;


		bool                         _isDirty;
		osg::ref_ptr<CWaterSurface>  _waterSurface;
		bool                         _buildSuccess;
		CGlbGlobeRObject            *_obj;
	};
	//////////////////////////////////////////////////////////////////////////
	class CameraTrackCallback: public osg::NodeCallback
	{
	public:
		CameraTrackCallback(CWaterScene *waterScene){_WaterScene=waterScene;}
		virtual void operator()(osg::Node* node, osg::NodeVisitor* nv)///update reflectionCamera _reflectionMatrix
		{
			if( nv->getVisitorType() == osg::NodeVisitor::CULL_VISITOR )
			{
				osgUtil::CullVisitor* cv = dynamic_cast<osgUtil::CullVisitor*>(nv);
				osg::Vec3f centre,up,eye;
				// get MAIN camera eye,centre,up
				cv->getRenderStage()->getCamera()->getViewMatrixAsLookAt(eye,centre,up);
				// update position
				osg::MatrixTransform* mt = dynamic_cast<osg::MatrixTransform*>(node);
				mt->setMatrix( osg::Matrix::translate( eye.x(), eye.y(), mt->getMatrix().getTrans().z() ) );
			}
			traverse(node, nv); 
		}
	private:
		CWaterScene *_WaterScene;
	};
	//////////////////////////////////////////////////////////////////////////
	CGlbGlobePolygonWaterSymbol::CGlbGlobePolygonWaterSymbol(void)
	{
	}

	CGlbGlobePolygonWaterSymbol::~CGlbGlobePolygonWaterSymbol(void)
	{
	}

	osg::Node * CGlbGlobePolygonWaterSymbol::Draw(CGlbGlobeRObject *obj,IGlbGeometry *geo)
	{
		glbref_ptr<IGlbGeometry> tempGeo = geo;
		osg::Node *polygonWaterNode=buildPolygonWaterSurfaceDrawable(obj,tempGeo.get());
		return polygonWaterNode;
	}

	void CGlbGlobePolygonWaterSymbol::DrawToImage( CGlbGlobeRObject *obj, IGlbGeometry* geom, glbByte *image,glbInt32 imageW, glbInt32 imageH,CGlbExtent &ext )
	{
		//throw std::exception("The method or operation is not implemented.");
	}

	IGlbGeometry * CGlbGlobePolygonWaterSymbol::GetOutline( CGlbGlobeRObject *obj,IGlbGeometry *geo )
	{
		//return CGlbGlobeSymbol::GetOutline(obj,geo);
		if (geo==NULL)
			return NULL;

		// 马林 2014.8.18
		glbInt32 i;

		GlbGeometryTypeEnum geoType=geo->GetType();
		switch(geoType)
		{
		case GLB_GEO_POLYGON:
			{
				glbref_ptr<CGlbMultiPolygon> _outlines = new CGlbMultiPolygon();	
				glbref_ptr<CGlbPolygon> _outline = new CGlbPolygon();

				CGlbPolygon* poly = dynamic_cast<CGlbPolygon*>(geo);
				CGlbLine* extRing = const_cast<CGlbLine *>(poly->GetExtRing());
				glbInt32 ptCnt = extRing->GetCount();
				glbDouble ptx,pty;

				CGlbLine* ln = new CGlbLine;
				_outline->SetExtRing(ln);
				for (i = 0; i < ptCnt; i++)
				{
					extRing->GetPoint(i,&ptx,&pty);
					ln->AddPoint(ptx,pty);
				}

				glbInt32 inRingCnt = poly->GetInRingCount();
				for (i = 0; i < inRingCnt; i++)
				{
					CGlbLine* inRing = const_cast<CGlbLine *>(poly->GetInRing(i));
					ln = new CGlbLine;
					_outline->AddInRing(ln);
					ptCnt = inRing->GetCount();
					for (glbInt32 j = 0; j < ptCnt; j++)
					{
						inRing->GetPoint(i,&ptx,&pty);
						ln->AddPoint(ptx,pty);
					}
				}

				_outlines->AddPolygon(_outline.get());
				mpt_outline = _outlines;
			}
			break;
		case GLB_GEO_MULTIPOLYGON:
			{
				glbref_ptr<CGlbMultiPolygon> _outlines = new CGlbMultiPolygon();	

				CGlbMultiPolygon* multipoly = dynamic_cast<CGlbMultiPolygon*>(geo);
				glbInt32 polyCnt = multipoly->GetCount();
				for(i =0; i < polyCnt; i++)
				{				
					CGlbPolygon* _outline = new CGlbPolygon();
					_outlines->AddPolygon(_outline);

					CGlbPolygon* poly = const_cast<CGlbPolygon *>(multipoly->GetPolygon(i));
					CGlbLine* extRing = const_cast<CGlbLine *>(poly->GetExtRing());
					glbInt32 ptCnt = extRing->GetCount();
					glbDouble ptx,pty;

					CGlbLine* ln = new CGlbLine;
					_outline->SetExtRing(ln);
					for (i = 0; i < ptCnt; i++)
					{
						extRing->GetPoint(i,&ptx,&pty);
						ln->AddPoint(ptx,pty);
					}

					glbInt32 inRingCnt = poly->GetInRingCount();
					for (i = 0; i < inRingCnt; i++)
					{
						CGlbLine* inRing = const_cast<CGlbLine *>(poly->GetInRing(i));

						ln = new CGlbLine;
						_outline->AddInRing(ln);
						ptCnt = inRing->GetCount();
						for (glbInt32 j = 0; j < ptCnt; j++)
						{
							inRing->GetPoint(i,&ptx,&pty);
							ln->AddPoint(ptx,pty);
						}
					}				
				}

				mpt_outline = _outlines;
			}
			break;
		}

		return mpt_outline.get();
	}

	osg::Node * CGlbGlobePolygonWaterSymbol::buildPolygonWaterSurfaceDrawable( CGlbGlobeRObject *obj,IGlbGeometry *geo )
	{
		osg::ref_ptr<CWaterScene> waterPolygon=new CWaterScene(obj,geo);
		if (waterPolygon->buildSuccess())
		{
			return waterPolygon.release();
		}
		return NULL;
	}
}
