//#version 120
//#version 100
//uniform mat4 gl_ModelViewMatrix;               // ģ����ͼ�任����
//uniform mat4 osg_ViewMatrixInverse;
uniform float osg_FrameTime;

uniform bool PolygonSelected;
uniform bool PolygonBlink;



uniform int PolygonOutlineOpacity;                      // ����β�͸����ϵ����0-��ȫ͸����100-Ϊ��ȫ��͸����
uniform vec4 PolygonOutlineColor;                       // �������ɫ(0-255)




// Main Program
void main( void )
{     
	vec4 imageColor = PolygonOutlineColor/255.0;	
	
	if(PolygonSelected)
	{
	    imageColor.rgb*=vec3(0.3,0.85,0.3);
	}	
	if(PolygonBlink)
	{	    
		int frameTimeFloor=int(floor(osg_FrameTime));		
	    frameTimeFloor=frameTimeFloor % 2;
	    if(frameTimeFloor==1)
	    {	
	        imageColor.rgb*=vec3(0.85,0.25,0.25);
	    }
	}

	gl_FragColor = imageColor;
	gl_FragColor.a=PolygonOutlineOpacity/100.0;
}
