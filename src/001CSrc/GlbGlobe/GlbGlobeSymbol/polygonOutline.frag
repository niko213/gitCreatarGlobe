//#version 120
//#version 100
//uniform mat4 gl_ModelViewMatrix;               // 模型视图变换矩阵
//uniform mat4 osg_ViewMatrixInverse;
uniform float osg_FrameTime;

uniform bool PolygonSelected;
uniform bool PolygonBlink;



uniform int PolygonOutlineOpacity;                      // 多边形不透明度系数（0-完全透明，100-为完全不透明）
uniform vec4 PolygonOutlineColor;                       // 多边形颜色(0-255)




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
