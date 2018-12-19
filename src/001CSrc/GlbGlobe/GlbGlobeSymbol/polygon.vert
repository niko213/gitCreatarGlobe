//#version 120
//uniform mat4 gl_ModelViewMatrix;                // 模型视图变换矩阵
//uniform mat4 osg_ViewMatrix;                    // 视图矩阵
//uniform mat4 osg_ViewMatrixInverse;             // 视图矩阵


//uniform float osg_FrameTime;


//varying float SysFrameTime;                  //


// Main Program
void main( void )
{    
    //gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex
	gl_Position = ftransform();
	
	gl_TexCoord[0] = gl_MultiTexCoord0;	
}
