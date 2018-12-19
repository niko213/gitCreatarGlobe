//#version 120
uniform float osg_FrameTime;

uniform bool visibleLineSelected;
uniform bool visibleLineBlink;

uniform int visibleLineOpacity;                      // ����β�͸����ϵ����0-��ȫ͸����100-Ϊ��ȫ��͸����
uniform vec4 visibleLineColor;                       // �������ɫ(0-255)
uniform vec4 invisibleLineColor;                       // �������ɫ(0-255)



uniform sampler2D    VisualLine_DepthMap;

varying vec4 vertexProjCoords_inDepthCam;

// Main Program
void main( void )
{         
    vec4 lineColor = vec4(1.0,1.0,0.0,1.0);
	
	vec2 depthUV = vec2(0.3,0.2);
	float splitDepth = texture2D(VisualLine_DepthMap, depthUV).x;
	//float pixelDepth=gl_TexCoord[0].z;
	float pixelDepth=vertexProjCoords_inDepthCam.z;
	if(pixelDepth>splitDepth)
	{
	     lineColor.r=1.0;lineColor.g=0.0;
	}
	else
	{
	     lineColor.r=0.0;lineColor.g=1.0;
	}	
	gl_FragColor = lineColor;
	gl_FragColor.a=float(visibleLineOpacity)/100.0;
	return;
	
}
