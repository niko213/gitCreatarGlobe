//#version 120
uniform mat4 gl_ModelViewMatrix;                // 模型视图变换矩阵
//uniform mat4 gl_ModelMatrix;                  // 模型变换矩阵
uniform mat4 gl_ModelViewProjectionMatrix;
uniform mat4 gl_ViewMatrix;                   // 视图变换矩阵
uniform mat4 osg_ViewMatrix;                    // 视图矩阵
uniform mat4 osg_ViewMatrixInverse;             // 视图矩阵


uniform mat4 ViewMatrix_DepthCam;             // 反射相机模型视图变换矩阵
uniform mat4 ProjectionMatrix_DepthCam;       // 反射相机模型投影变换矩阵

varying vec4 vertexProjCoords_inDepthCam;


//          Main Program
void main( void )
{   
	gl_Position = ftransform();	
	
	vec4 posInEye = gl_ModelViewMatrix * gl_Vertex;
	vec4 posInWorld = osg_ViewMatrixInverse * posInEye;
	
	vertexProjCoords_inDepthCam = ProjectionMatrix_DepthCam * ViewMatrix_DepthCam * posInWorld;	
	gl_TexCoord[0]=vertexProjCoords_inDepthCam;
}
