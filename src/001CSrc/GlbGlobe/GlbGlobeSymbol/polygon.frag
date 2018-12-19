//#version 120
//#version 100
//uniform mat4 gl_ModelViewMatrix;               // 模型视图变换矩阵
//uniform mat4 osg_ViewMatrixInverse;
uniform float osg_FrameTime;

uniform bool PolygonSelected;
uniform bool PolygonBlink;

uniform vec2 PolygonDownLeft;                    // 多边形贴图环绕中心
uniform vec2 PolygonRotateCenter;                // 多边形贴图环绕中心
uniform vec2 PolygonWeightAndHeight;             // 多边形大小

uniform int PolygonOpacity;                      // 多边形不透明度系数（0-完全透明，100-为完全不透明）
uniform vec4 PolygonColor;                       // 多边形颜色(0-255)
uniform bool PolygonUseTex;                      // 多边形使用贴图
uniform int PolygonTexRepeatMode;                // 多边形纹理重复模式
uniform float tilingU;                           // 纹理U方向平铺次数
uniform float tilingV;                           // 纹理V方向平铺次数
//uniform float PolygonTextureRotation;          // 纹理环绕角度逆时针为正方向（弧度）

uniform sampler2D    PolygonTextureMap;


mat3 buildMat(float rotAngle)
{
     mat3 rotMat;// 列主序
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
