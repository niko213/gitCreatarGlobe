//#version 120
//uniform mat4 gl_ModelViewMatrix;                // ģ����ͼ�任����
//uniform mat4 osg_ViewMatrix;                    // ��ͼ����
//uniform mat4 osg_ViewMatrixInverse;             // ��ͼ����


//uniform float osg_FrameTime;


//varying float SysFrameTime;                  //


// Main Program
void main( void )
{    
    //gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex
	gl_Position = ftransform();
	
	gl_TexCoord[0] = gl_MultiTexCoord0;	
}
