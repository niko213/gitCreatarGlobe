uniform mat4 gl_ModelViewMatrix;                // 模型视图变换矩阵
//uniform mat4 gl_ModelMatrix;                  // 模型变换矩阵
uniform mat4 gl_ModelViewProjectionMatrix;
uniform mat4 gl_ViewMatrix;                   // 视图变换矩阵
uniform mat4 osg_ViewMatrix;                    // 视图矩阵
uniform mat4 osg_ViewMatrixInverse;             // 视图矩阵


uniform vec3 water_EyePosition;
uniform mat4 ViewMatrix_ReflectCam;             // 反射相机模型视图变换矩阵
uniform mat4 ProjectionMatrix_ReflectCam;       // 反射相机模型投影变换矩阵


varying vec3 eyePos;
varying vec3 eyeDirction;                       // 没有归一化
varying vec4 vertexCoords_inObj;
varying vec4 vertexCoords_inMainCam;
varying vec4 vertexCoords_inRefCam;
varying vec4 vertexCoords_inRefCamEx;

vec3 extractEyePostionFromVM(mat4 vMat)
{
     vec3 eyePosTmp;
	 eyePosTmp.x = vMat[3][0];
	 eyePosTmp.y = vMat[3][1];
	 eyePosTmp.z = vMat[3][2];
	 
	 return eyePosTmp;
}

//          Main Program
void main( void )
{    
    //gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
	gl_Position = ftransform();	
	gl_TexCoord[0] = gl_MultiTexCoord0;	
	gl_TexCoord[1] = gl_MultiTexCoord1;
	
	vertexCoords_inObj = gl_Vertex;
	vertexCoords_inMainCam = gl_Position;
	
	vec4 posInEye = gl_ModelViewMatrix * gl_Vertex;
	vec4 posInWorld = osg_ViewMatrixInverse * posInEye;            
	vertexCoords_inRefCam = ProjectionMatrix_ReflectCam * ViewMatrix_ReflectCam * posInWorld;	
	
	mat4 biaseMatrix;
	biaseMatrix[0][0]=0.5;biaseMatrix[0][1]=0.0;biaseMatrix[0][2]=0.0;biaseMatrix[0][3]=0.0;
	biaseMatrix[1][0]=0.0;biaseMatrix[1][1]=0.5;biaseMatrix[1][2]=0.0;biaseMatrix[1][3]=0.0;
	biaseMatrix[2][0]=0.0;biaseMatrix[2][1]=0.0;biaseMatrix[2][2]=0.0;biaseMatrix[2][3]=0.0;
	biaseMatrix[3][0]=0.5;biaseMatrix[3][1]=0.5;biaseMatrix[3][2]=0.0;biaseMatrix[3][3]=1.0;	 
	vertexCoords_inRefCamEx = biaseMatrix * vertexCoords_inRefCam;
	
	//eyePos = extractEyePostionFromVM(osg_ViewMatrix);
	//eyePos = (osg_ViewMatrixInverse*vec4(0.0,0.0,0.0,1.0)).rgb;
	eyePos = water_EyePosition;// 在物体本地空间
	
	eyeDirction = water_EyePosition-gl_Vertex.xyz;
}
