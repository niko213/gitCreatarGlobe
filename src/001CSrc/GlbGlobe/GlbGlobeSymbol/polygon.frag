//#version 120
//#version 100
//uniform mat4 gl_ModelViewMatrix;               // ģ����ͼ�任����
//uniform mat4 osg_ViewMatrixInverse;
uniform float osg_FrameTime;

uniform bool PolygonSelected;
uniform bool PolygonBlink;

uniform vec2 PolygonDownLeft;                    // �������ͼ��������
uniform vec2 PolygonRotateCenter;                // �������ͼ��������
uniform vec2 PolygonWeightAndHeight;             // ����δ�С

uniform int PolygonOpacity;                      // ����β�͸����ϵ����0-��ȫ͸����100-Ϊ��ȫ��͸����
uniform vec4 PolygonColor;                       // �������ɫ(0-255)
uniform bool PolygonUseTex;                      // �����ʹ����ͼ
uniform int PolygonTexRepeatMode;                // ����������ظ�ģʽ
uniform float tilingU;                           // ����U����ƽ�̴���
uniform float tilingV;                           // ����V����ƽ�̴���
//uniform float PolygonTextureRotation;          // �����ƽǶ���ʱ��Ϊ�����򣨻��ȣ�

uniform sampler2D    PolygonTextureMap;


mat3 buildMat(float rotAngle)
{
     mat3 rotMat;// ������
	 rotMat[0][0] = cos(rotAngle);  rotMat[1][0] = sin(rotAngle);	 rotMat[2][0] = 0.0;
	 rotMat[0][1] = -sin(rotAngle); rotMat[1][1] = cos(rotAngle);	 rotMat[2][1] = 0.0;	 
	 rotMat[0][2] = 0.0;	        rotMat[1][2] = 0.0;	             rotMat[2][2] = 1.0;	 
	 return rotMat;
}

// Main Program
void main( void )
{     
	vec4 imageColor = PolygonColor/255.0;
	if(PolygonUseTex)
	{
	    //mat3 rotMat = buildMat(PolygonTextureRotation);
		vec2 uv=gl_TexCoord[0].xy;
	    uv-=PolygonRotateCenter;
    	uv/=PolygonWeightAndHeight;	
	    uv+=0.5;
	    //imageColor = texture2D(PolygonTextureMap, gl_TexCoord[0]);
		if(PolygonTexRepeatMode==0)
		{
		    uv.x *= tilingU;
			uv.y *= tilingV;
	        imageColor = texture2D(PolygonTextureMap, uv);	
		}
		else
		{
		    imageColor = texture2D(PolygonTextureMap, uv);
		}
	}	
	
	if(PolygonSelected)
	{
	    //imageColor.rgb*=vec3(0.3,0.85,0.3);
		imageColor.rgb=vec3(1.0,0.0,0.0);
	}	
	if(PolygonBlink)
	{	    
		int frameTimeFloor=int(floor(osg_FrameTime));		
	    frameTimeFloor=frameTimeFloor % 2;
	    if(frameTimeFloor==1)
	    {	
	        //imageColor.rgb*=vec3(0.85,0.25,0.25);
			imageColor.rgb=vec3(0.65,0.65,0.35);
	    }
	}

	gl_FragColor = imageColor;
	gl_FragColor.a=PolygonOpacity/100.0;
}
