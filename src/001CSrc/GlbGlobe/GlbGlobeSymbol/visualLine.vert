//#version 120
uniform mat4 gl_ModelViewMatrix;                // ģ����ͼ�任����
//uniform mat4 gl_ModelMatrix;                  // ģ�ͱ任����
uniform mat4 gl_ModelViewProjectionMatrix;
uniform mat4 gl_ViewMatrix;                   // ��ͼ�任����
uniform mat4 osg_ViewMatrix;                    // ��ͼ����
uniform mat4 osg_ViewMatrixInverse;             // ��ͼ����


uniform mat4 ViewMatrix_DepthCam;             // �������ģ����ͼ�任����
uniform mat4 ProjectionMatrix_DepthCam;       // �������ģ��ͶӰ�任����

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
